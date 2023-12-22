/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rt_usb.c

	Abstract:

	Revision History:
	Who			When		What
	--------	----------	----------------------------------------------
	Name		Date				Modification logs
	Shiang		2009/07/23		Initial version for EBS RTIP Network stack
*/


#ifdef EBS_RTIP
#include "rtipapi.h"
#include "rt_config.h"


INT RtmpIfaceId;
PIFACE RtmpPI;

typedef struct _RTMP_DC_TCP_SEQ_STRUCT_{
	unsigned short srcPort;
	unsigned short dstPort;
	unsigned int lastSeq;
	unsigned int expSeq;
	unsigned char bUsed;
}RTMP_DC_TCP_SEQ_STRUCT;

static UCHAR SeqSrcAddr[4] = {0xc0, 0xa8, 0x64, 0x4};
static UCHAR SeqDstAddr[4] = {0xc0, 0xa8, 0x64, 0x2};

static RTMP_DC_TCP_SEQ_STRUCT tcpSeqTouple[10] = {0};

int RtmpTCPSeqCheck(DCU *pDcu)
{
	static UINT32 lastSeq = 0, curSeq, expectSeq = 0;
	BOOLEAN bInSeq = TRUE;
	PUCHAR pDataPtr;
	PETHER pEthHdr;
	int dumpLen;

	
	pDataPtr = DCUTODATA(pDcu);
	dumpLen = DCUTOPACKET(pDcu)->length;

/*	dc_log_printf("AnnounceLen=%d", dumpLen); */
/*	dumpLen = dumpLen > 64 ? 64: dumpLen; */
/*	hex_dump("Announce", DCUTODATA(pDcu), dumpLen); */
		
	pEthHdr = (PETHER)pDataPtr;

	/* We only check for ethernet frame type is 0x0800 and IP protocol is 0x06 */
	if ( pEthHdr->eth_type == 0x0008 /*pDataPtr[12] == 0x08 && pDataPtr[13] == 0x00*/)
	{
		PIPPKT pip;
		int IpPktLen;
		
		pip = &pDataPtr[14];
		IpPktLen = ntohs(pip->ip_len);
/*		hex_dump("IPHdr", pip, (pip->ip_verlen & 0x0f) * 4); */
/*		dc_log_printf("ipProt=0x%x,Len=%d", pip->ip_proto, IpPktLen); */
		if (pip->ip_proto == 0x06)
		{
			PTCPPKT ptcp;
			int tcpLen;
			ptcp = &pDataPtr[14 + (pip->ip_verlen & 0x0f) * 4];

/*			hex_dump("TCPHdr", ptcp, sizeof(*ptcp)); */
			if (NdisEqualMemory(&pip->ip_src[0], SeqSrcAddr, 4)== TRUE &&
				NdisEqualMemory(&pip->ip_dest[0], SeqDstAddr, 4)== TRUE)
			{
				int idx, emptyIdx = -1;
				UINT16 srcPort, dstPort;
				
				/* Ok, we got the packet from src and dst ip we required to monitor */
				curSeq = (((ptcp->tcp_seq[0]<<8) & 0xff00) | ((ptcp->tcp_seq[0] >>8)& 0xff)) << 16;
				curSeq += (((ptcp->tcp_seq[1]<<8) & 0xff00) | ((ptcp->tcp_seq[1]>>8) & 0xff));
				
				tcpLen = IpPktLen - 20  -((ptcp->tcp_hlen & 0xf0)>>2); /* 20 for IP header */
				if ((tcpLen == 0) && (ptcp->tcp_flags &  0x02))
				{
					expectSeq =curSeq + 1;
				}
				else
					expectSeq = curSeq + tcpLen;

/*				dc_log_printf("TCPPkt, hlen=%d,len=%d,Seq=0x%x,expSeq=0x%x", (ptcp->tcp_hlen & 0xf0)>>2, */
/*							tcpLen, curSeq, expectSeq); */

				for (idx = 0; idx < 10; idx++)
				{
					/* First find the srcPort and dstPort */
					if ((tcpSeqTouple[idx].bUsed == FALSE) && (emptyIdx < 0))
						emptyIdx = idx;
					
					if ((tcpSeqTouple[idx].bUsed == TRUE) && 
						(tcpSeqTouple[idx].srcPort == ptcp->tcp_source) && 
						(tcpSeqTouple[idx].dstPort == ptcp->tcp_dest))
					{
						break;
					}
				}

				/*dc_log_printf("Idx=%d, emptyIdx=%d", idx, emptyIdx); */
				if (idx == 10) 
				{
					if (emptyIdx < 0)
						emptyIdx = 0;
					
					/* This's a new session, we use a new tuple sapce to save it */
					tcpSeqTouple[emptyIdx].srcPort = ptcp->tcp_source;
					tcpSeqTouple[emptyIdx].dstPort = ptcp->tcp_dest;
					tcpSeqTouple[emptyIdx].lastSeq = curSeq;
					tcpSeqTouple[emptyIdx].expSeq = expectSeq;
					tcpSeqTouple[emptyIdx].bUsed = TRUE;
					dc_log_printf("NewSeqTuple, SP=0x%x, DP=0x%x, seq=0x%x, len=%d, expSeq=0x%x", 
							ptcp->tcp_source, ptcp->tcp_dest, curSeq, tcpLen, expectSeq);
				}
				else if (idx < 10)
				{
					/* This is a old session, check the seq now */
					if (tcpSeqTouple[idx].expSeq ==curSeq)
					{

					}
					else if (tcpSeqTouple[idx].lastSeq ==curSeq)
					{
						dc_log_printf("DuplicatSeq=0x%x", curSeq);
					}
					else if ((tcpSeqTouple[idx].lastSeq ==0) && (tcpSeqTouple[idx].expSeq == 0))
					{
						dc_log_printf("SeqCkInit, seq=%x, expSeq=%x", curSeq, expectSeq);
					}
					else
					{
						if (tcpSeqTouple[idx].expSeq != curSeq)
						{
							dc_log_printf("WARNING:eSeq=%x,cSeq=%x,lSeq=%x", tcpSeqTouple[idx].expSeq, curSeq, tcpSeqTouple[idx].lastSeq);
							hex_dump("DumpTCPHdr", (PUCHAR)ptcp, ptcp->tcp_hlen);
							bInSeq = FALSE;
						}
					}
					tcpSeqTouple[idx].lastSeq = curSeq;
					tcpSeqTouple[idx].expSeq = expectSeq;
				}
				else
				{
					dc_log_printf("Error, Cannot find Tuple");
				}
			}
		}
	}

	return bInSeq;
}


void hex_dump_DCU(PUCHAR callerStr, DCU pDcu, BOOLEAN bDumpPkt)
{
	int headroom;
	PUCHAR pDataPtr;


	dc_log_printf("%s(): pDcu->pi=0x%x(0x%x)", callerStr, pDcu->ctrl.pi, RtmpPI);
	dc_log_printf("  dcu_flags=0x%x", pDcu->ctrl.dcu_flags);
	dc_log_printf("  data=0x%x", pDcu->data);
	dc_log_printf("  length=%d", pDcu->length);
	dc_log_printf("  inet.ptr=0x%x", DCUTOPACKET(pDcu)->inet.ptr);
	dc_log_printf("  prot.ptr=0x%x", DCUTOPACKET(pDcu)->prot.ptr);
	dc_log_printf("  ip_info.ip_hlen=%d", pDcu->ip_info.ip_hlen);
	dc_log_printf("  ip_info.ip_len=%d", pDcu->ip_info.ip_len);
	
	if (bDumpPkt)
		hex_dump(callerStr, pDcu->data, pDcu->length);
}


void hex_dump_PIFACE(PUCHAR callerStr, PIFACE pi)
{
	dc_log_printf("%s(): pi=0x%x(%s)", callerStr, pi, pi->pdev->device_name);
	dc_log_printf("  open_count=%d", pi->open_count);
	dc_log_printf("  minorNo.=%d", pi->minor_number);
	dc_log_printf("  IFFlags=%d", pi->iface_flags);
	dc_log_printf("  AddressingRelatedInfo:");
	dc_log_printf("   Mac=%02x:%02x:%02x:%02x:%02x:%02x", PRINT_MAC(pi->addr.my_hw_addr));
	dc_log_printf("   IPAddr=%d.%d.%d.%d", pi->addr.my_ip_addr[0],pi->addr.my_ip_addr[1],pi->addr.my_ip_addr[2],pi->addr.my_ip_addr[3]);
	dc_log_printf("   NetMask=%d.%d.%d.%d", pi->addr.my_ip_mask[0],pi->addr.my_ip_mask[1],pi->addr.my_ip_mask[2],pi->addr.my_ip_mask[3]);
	dc_log_printf("   BCAddr=%d.%d.%d.%d", pi->addr.my_net_bc_ip_addr[0],pi->addr.my_net_bc_ip_addr[1],pi->addr.my_net_bc_ip_addr[2],pi->addr.my_net_bc_ip_addr[3]);
	dc_log_printf("   ipFlags=%d", pi->addr.iface_ip_flags);
	dc_log_printf("  DeviceRelatedInfo:");
	dc_log_printf("   devID=%d", pi->pdev->device_id);
	dc_log_printf("  StatusRelatedInfo:");
	dc_log_printf("   pktIn=%d", pi->stats.interface_packets_in);
	dc_log_printf("   pktOut=%d", pi->stats.interface_packets_out);
	dc_log_printf("   pktlost=%d", pi->stats.interface_packets_lost);
	dc_log_printf("   ByteIn=%d", pi->stats.interface_bytes_in);
	dc_log_printf("   ByteOut=%d", pi->stats.interface_bytes_out);
	dc_log_printf("   ErrIn=%d", pi->stats.interface_errors_in);	
	dc_log_printf("   ErrOut=%d", pi->stats.interface_errors_out);	
	dc_log_printf("   ArpIn=%d", pi->stats.rcv_arp_packets);
}

/*
========================================================================
Routine Description:
	Early check if the packet has enough packet headeroom for latter handling.

Arguments:
	pEndObj		the end object.
	pMblk		the pointer refer to a packet buffer
	
Return Value:
	packet buffer pointer:	if sanity check success.
	NULL:				if failed

Note:
	This function is the entry point of Tx Path for Os delivery packet to 
	our driver. You only can put OS-depened & STA/AP common handle procedures 
	in here.
========================================================================
*/
static inline RTMP_RTIP_NET_BUF *RtmpTxNetPktCheck(
	IN PIFACE pi,
	IN DCU pDcu)
{
	RTMP_RTIP_NET_BUF *pNetBuf;
	RTMP_ADAPTER	*pAd;
	PDEVTABLE		pTable;
	

#if 0 /*def THREADX_TX_TASK */
	pNetBuf = RtmpTxNetBufGet(DCUTOPACKET(pDcu)->length, 0, NULL);
	if (pNetBuf)
	{
		pAd = RTMP_OS_NETDEV_GET_PRIV(pi);
		pNetBuf->pDcuBuf = NULL;
		memcpy(pNetBuf->pDataPtr, DCUTODATA(pDcu), DCUTOPACKET(pDcu)->length);
		pNetBuf->pktLen = DCUTOPACKET(pDcu)->length;
		pNetBuf->pi = pDcu->ctrl.pi;

		ks_invoke_output(pi, 1);
	}
#else
	pNetBuf = RtmpTxNetBufGet(0, 0, pDcu);
	if (pNetBuf)
	{
		pAd = RTMP_OS_NETDEV_GET_PRIV(pi);
		SET_OS_PKT_NETDEV(pNetBuf, pi);
	}
#endif

	return pNetBuf;
}


void * RtmpTxNetBuf_put(RTMP_RTIP_NET_BUF *pNetBuf, int n)
{
	PUCHAR p;

	if (pNetBuf->pFrameBuf == NULL)
		return NULL;
	
	if ((pNetBuf->pktLen + n) > RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_PKT].blkSize)
		return NULL;
	
	p = pNetBuf->pFrameBuf + pNetBuf->pktLen;
	pNetBuf->pktLen += n;
	
	return p;
}


void *RtmpTxNetBuf_push(RTMP_RTIP_NET_BUF *pNetBuf, int n)
{
	ASSERT(pNetBuf);

	if (pNetBuf->pDataHead == NULL)
		return NULL;

	if ((pNetBuf->pDataPtr - n) < pNetBuf->pDataHead)
	{
		dc_log_printf("HdrRoomNotEnough, 0x%x-0x%x", pNetBuf->pDataHead, pNetBuf->pDataPtr);
		return NULL;
	}
	pNetBuf->pDataPtr -= n;
	pNetBuf->pktLen += n;
	
	return pNetBuf->pDataPtr;
}


/*
Network buffer related functions
*/
RTMP_RTIP_NET_BUF *RtmpTxNetBufGet(
	IN int length,
	IN int headerResvLen,
	IN DCU pDcuBuf)
{
	RTMP_RTIP_NET_BUF *pNetBuf;
	RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamBlk;
	ra_dma_addr_t *pDMAAddr;


	pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_NETBUF];	
	RtmpDCRamBlockMemAlloc(pDcRamBlk, sizeof(RTMP_RTIP_NET_BUF), (void **)&pNetBuf, &pDMAAddr);
	if (pNetBuf == NULL)
	{
		dc_log_printf("Allocate NetBuf failed!\n");
		return NULL;
	}
	NdisZeroMemory(pNetBuf, sizeof(RTMP_RTIP_NET_BUF));
	
	/* 
		If the NetBufReq is need extra memory space, we need to allocate a buffer 
		as the temporary space to copy the content from bulk-in buffer, and after 
		handle done for the buffer and ready to copy to the upper layer, we copy the 
		content from this buffer to DCU.
	*/
	if (length > 0)
	{
		pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_PKT];
		ASSERT(length < pDcRamBlk->blkSize);
		RtmpDCRamBlockMemAlloc(pDcRamBlk, length, (void **)(&pNetBuf->pFrameBuf), &pDMAAddr);
		if (pNetBuf->pFrameBuf == NULL)
		{
			dc_log_printf("Allocate PktBuf failed!\n");
			goto PktAllocFail;
		}
		pNetBuf->pDataHead = pNetBuf->pFrameBuf;
		pNetBuf->pDataPtr = (UCHAR *)(pNetBuf->pFrameBuf + headerResvLen);
		pNetBuf->pktLen = 0;
	}
	else if (pDcuBuf)
	{
		pNetBuf->pDcuBuf = pDcuBuf;
		pNetBuf->pDataHead = DCUTODATA(pDcuBuf);
		pNetBuf->pDataPtr = DCUTODATA(pDcuBuf);
		pNetBuf->pktLen = DCUTOPACKET(pDcuBuf)->length;
		pNetBuf->pi = pDcuBuf->ctrl.pi;
	}
	
	return pNetBuf;
	
PktAllocFail:
	if (pNetBuf)
		RtmpDCRamBlockMemFree(pNetBuf);
	
	return NULL;
}


PNDIS_PACKET RtmpOSNetPktAlloc(
	IN RTMP_ADAPTER *pAd, 
	IN int size)
{
	RTMP_RTIP_NET_BUF *pNetBuf;
	/* Add 2 more bytes for ip header alignment*/
	pNetBuf = RtmpTxNetBufGet(size, 0, NULL);
	
	return ((PNDIS_PACKET)pNetBuf);
}


PNDIS_PACKET RTMP_AllocateFragPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length)
{
	RTMP_RTIP_NET_BUF *pNetBuf;
	POS_COOKIE	pObj;

	pNetBuf = RtmpTxNetBufGet(Length, 0, NULL);
	
	if (!pNetBuf)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("can't allocate frag rx %ld size packet\n",Length));
	}

	return (PNDIS_PACKET) pNetBuf;
	
}

#if 0
/* TODO: shiang, following function are not implemented for ThreadX yet!!! */
VOID build_tx_packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR	pFrame,
	IN	ULONG	FrameLen)
{
	DCU	pTxPkt;

	ASSERT(pPacket);
	pTxPkt = RTPKT_TO_OSPKT(pPacket);

	/*NdisMoveMemory(m_data_put(pTxPkt, FrameLen), pFrame, FrameLen); */
	GET_OS_PKT_TOTAL_LEN(pTxPkt) += FrameLen;
}

PNDIS_PACKET RTMP_AllocateTxPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress)
{
	DCU pPkt;

	pPkt = RtmpTxNetBufGet(Length, 0, NULL);

	if (pPkt == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("can't allocate tx %ld size packet\n",Length));
		*VirtualAddress = (PVOID) NULL;

		return NULL;
	}

	*VirtualAddress = (PVOID) pPkt->data;	

	return (PNDIS_PACKET) pPkt;
}

BOOLEAN OS_Need_Clone_Packet(void)
{
    /* Shiang: Not used now!! */
    return FALSE;
}

NDIS_STATUS RTMPCloneNdisPacket(
	IN	PRTMP_ADAPTER	pAd,
	IN	BOOLEAN			pInsAMSDUHdr,
	IN	PNDIS_PACKET	pInPacket,
	OUT PNDIS_PACKET   *ppOutPacket)
{
    /* Shiang: Not used now!! */
    return NDIS_STATUS_SUCCESS;
}
#endif /* Unused */

/* the allocated NDIS PACKET must be freed via RTMPFreeNdisPacket() */
NDIS_STATUS RTMPAllocateNdisPacket(
	IN	PRTMP_ADAPTER	pAd,
	OUT PNDIS_PACKET   *ppPacket,
	IN	PUCHAR			pHeader,
	IN	UINT			HeaderLen,
	IN	PUCHAR			pData,
	IN	UINT			DataLen)
{
	int totalLen;
	RTMP_RTIP_NET_BUF *pNetBuf;
	
	ASSERT(pData);
	ASSERT(DataLen);

	totalLen = HeaderLen + DataLen + RTMP_PKT_TAIL_PADDING;
	pNetBuf = (PNDIS_PACKET) RtmpTxNetBufGet(totalLen, 0, NULL);
	if (pNetBuf == NULL)
	{
		*ppPacket = NULL;
		DBGPRINT(RT_DEBUG_ERROR, ("RTMPAllocateNdisPacket Fail\n\n"));
		return NDIS_STATUS_FAILURE;
	}

	/* 2. clone the frame content */
	if ((HeaderLen > 0) && (pHeader != NULL))
	{
		NdisMoveMemory(GET_OS_PKT_DATAPTR(pNetBuf), pHeader, HeaderLen);
	}
	if ((DataLen > 0) && (pData != NULL))
	{
		NdisMoveMemory(GET_OS_PKT_DATAPTR(pNetBuf) + HeaderLen, pData, DataLen);
	}

	/* 3. update length of packet */
	GET_OS_PKT_LEN(pNetBuf) = (HeaderLen + DataLen);

/*	DBGPRINT(RT_DEBUG_TRACE, ("%s : pPacket = %p, len = %d\n", */
/*				__FUNCTION__, pPacket, GET_OS_PKT_LEN(pPacket))); */
	*ppPacket = (PNDIS_PACKET)pNetBuf;

	return NDIS_STATUS_SUCCESS;
}


/*
  ========================================================================
  Description:
	This routine frees a miniport internally allocated NDIS_PACKET and its
	corresponding NDIS_BUFFER and allocated memory.
  ========================================================================
*/
VOID RTMPFreeNdisPacket(
	IN PRTMP_ADAPTER pAd,
	IN PNDIS_PACKET  pPacket)
{
	RTMP_RTIP_NET_BUF *pNetBuf;
	RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamBlk;
	PIFACE pi;


	pNetBuf = (RTMP_RTIP_NET_BUF *)pPacket;
	pi = pNetBuf->pi;
	if ((pNetBuf->pDcuBuf))
	{
		if(pNetBuf->bDcuFlg == DRIVER_ALLOC)
		{
			os_free_packet(pNetBuf->pDcuBuf);
		}
		else if (pi)
		{
			pi->xmit_status = 0;
			ks_invoke_output(pi, 1);
		}
	}

	if (pNetBuf->pFrameBuf)
	{	/* Free Internal buffer first */
		pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_PKT];
		RtmpDCRamBlockMemFree(pNetBuf->pFrameBuf);
	}

	/* Free the NetBuf structure */
	pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_NETBUF];
	RtmpDCRamBlockMemFree((void *)pNetBuf);
}


PNDIS_PACKET skb_copy(PNDIS_PACKET pSrcPkt, INT flags)
{
	RTMP_RTIP_NET_BUF *pNewNetBuf, *pOldNetBuf;

	pOldNetBuf = (RTMP_RTIP_NET_BUF *)pSrcPkt;
	pNewNetBuf = RtmpTxNetBufGet(pOldNetBuf->pktLen, 0, NULL);		
	if (pNewNetBuf != NULL)
	{
		NdisCopyMemory(pNewNetBuf->pFrameBuf, pOldNetBuf->pFrameBuf, pOldNetBuf->pktLen);
		pNewNetBuf->pktLen = pOldNetBuf->pktLen;
		pNewNetBuf->pDataPtr = pOldNetBuf->pFrameBuf + (pOldNetBuf->pDataPtr - pOldNetBuf->pFrameBuf);
	}

	return (PNDIS_PACKET)pNewNetBuf;
}


PNDIS_PACKET skb_clone(PNDIS_PACKET pSrcPkt, int flags)
{
	RTMP_RTIP_NET_BUF *pNewNetBuf, *pOldNetBuf;

	pOldNetBuf = (RTMP_RTIP_NET_BUF *)pSrcPkt;
	pNewNetBuf = RtmpTxNetBufGet(pOldNetBuf->pktLen, 0, NULL);
	if (pNewNetBuf == NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): get Mblk failed!\n", __FUNCTION__));
		return NULL;
	}

	NdisCopyMemory(pNewNetBuf->pFrameBuf, pOldNetBuf->pFrameBuf, pOldNetBuf->pktLen);
	pNewNetBuf->pktLen = pOldNetBuf->pktLen;
	pNewNetBuf->pDataPtr = pOldNetBuf->pFrameBuf + (pOldNetBuf->pDataPtr - pOldNetBuf->pFrameBuf);
	
	return pNewNetBuf;
}


void RTMP_QueryPacketInfo(
	IN  PNDIS_PACKET pPacket,
	OUT PACKET_INFO  *pPacketInfo,
	OUT PUCHAR		 *pSrcBufVA,
	OUT	UINT		 *pSrcBufLen)
{
	pPacketInfo->BufferCount = 1;
	pPacketInfo->pFirstBuffer = GET_OS_PKT_DATAPTR(pPacket);
	pPacketInfo->PhysicalBufferCount = 1;
	pPacketInfo->TotalPacketLength = GET_OS_PKT_LEN(pPacket);

	*pSrcBufVA = GET_OS_PKT_DATAPTR(pPacket);
	*pSrcBufLen = GET_OS_PKT_LEN(pPacket); 
}

#if 0
void RTMP_QueryNextPacketInfo(
	IN  PNDIS_PACKET *ppPacket,
	OUT PACKET_INFO  *pPacketInfo,
	OUT PUCHAR		 *pSrcBufVA,
	OUT	UINT		 *pSrcBufLen)
{
    /* Shiang: Not used now!! */
}
#endif /* Unused */


PNDIS_PACKET ClonePacket(PNET_DEV ndev, PNDIS_PACKET pkt, UCHAR *buf, ULONG sz)
{
	RTMP_RTIP_NET_BUF *pClonedNetBuf;
	PIFACE pi = (PIFACE)ndev;

	ASSERT(sz < 1530);	

	pClonedNetBuf = skb_clone(pkt, MEM_ALLOC_FLAG);
	if (pClonedNetBuf != NULL)
	{
	    	/* set the correct dataptr and data len */
	    	SET_OS_PKT_NETDEV(pClonedNetBuf, pi);

    		/* set the correct dataptr and data len */
	    	pClonedNetBuf->pDataPtr = pClonedNetBuf->pFrameBuf + (buf - GET_OS_PKT_DATAPTR(pkt));
    		pClonedNetBuf->pktLen = sz;
	}

	if(0)
	{
		int dumpLen;
		dc_log_printf("ClonedPktLen=%d, DS=%d", pClonedNetBuf->pktLen, sz);

		dumpLen = pClonedNetBuf->pktLen > 64 ? 64: pClonedNetBuf->pktLen;
		hex_dump("ClonedPkt", pClonedNetBuf->pDataPtr, dumpLen);
		hex_dump("pPacket", buf, dumpLen);
	}
	
	return pClonedNetBuf;
	
}


/* TODO: need to check is there any diffence between following four functions???? */
PNDIS_PACKET DuplicatePacket(PNET_DEV pNetDev, PNDIS_PACKET pPacket)
{
	PNDIS_PACKET	pRetPacket = NULL;
	USHORT			DataSize;
	UCHAR			*pData;

	DataSize = (USHORT) GET_OS_PKT_LEN(pPacket);
	pData = (PUCHAR) GET_OS_PKT_DATAPTR(pPacket);	


	pRetPacket = skb_clone(RTPKT_TO_OSPKT(pPacket), MEM_ALLOC_FLAG);
	if (pRetPacket)
		SET_OS_PKT_NETDEV(pRetPacket, pNetDev);

	/*DBGPRINT(RT_DEBUG_TRACE, ("%s(): pRetPacket = 0x%x!\n", __FUNCTION__, (unsigned int)pRetPacket)); */
	
	return pRetPacket;
}


PNDIS_PACKET duplicate_pkt_vlan(
	IN PNET_DEV pNetDev,
	IN USHORT VLAN_VID,
	IN USHORT VLAN_Priority,
	IN PUCHAR pHeader802_3,
	IN UINT HdrLen,
	IN PUCHAR pData,
	IN ULONG DataSize,
	IN UCHAR *TPID)
{
	DCU		pMblk;
	POS_COOKIE		pObj;
	PNDIS_PACKET	pPacket = NULL;
	UINT16			VLAN_Size;
	int bufLen = HdrLen + DataSize + 2;
	PUCHAR			pMdataPtr;
	PIFACE 		*pi;

#ifdef WIFI_VLAN_SUPPORT
	if (VLAN_VID != 0)
		bufLen += LENGTH_802_1Q;
#endif /* WIFI_VLAN_SUPPORT */

	/* TODO: Shiang, Not finish yet!!! */
	pObj = pAd->OS_Cookie;
	pMblk = RtmpTxNetBufGet(bufLen, 0, NULL);

	if (pMblk != NULL)
	{
		/* copy header (maybe +VLAN tag) */
		VLAN_Size = VLAN_8023_Header_Copy(VLAN_VID, VLAN_Priority,
											pHeader802_3, HdrLen,
											pMblk->m_data, TPID);
		skb_put(pMblk, HdrLen + VLAN_Size);

		/* copy data body */
		pMdataPtr = GET_OS_PKT_DATATAIL(pMblk);
		NdisMoveMemory(pMdataPtr, pData, DataSize);
		skb_put(pMblk, DataSize);
		
		pi = (PIFACE)pNetDev;
		SET_OS_PKT_NETDEV(pMblk, pi);

		pPacket = OSPKT_TO_RTPKT(pMblk);
	}

	return pPacket;
}


#define TKIP_TX_MIC_SIZE		8
PNDIS_PACKET duplicate_pkt_with_TKIP_MIC(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket)
{
	RTMP_RTIP_NET_BUF *pNetBuf, *pNewNetBuf;
	int 				bufLen, tailRoom;
	POS_COOKIE		pObj;
	PIFACE pi;
	
	pObj = pAd->OS_Cookie;
	pNetBuf = RTPKT_TO_OSPKT(pPacket);
	pi = GET_OS_PKT_NETDEV(pNetBuf);

	if (pNetBuf->pDcuBuf != NULL)
	{
		bufLen = pNetBuf->pktLen + TKIP_TX_MIC_SIZE;
		
		/* alloc a new skb and copy the packet */
		pNewNetBuf = RtmpTxNetBufGet(bufLen, 0, NULL);
		if (pNewNetBuf == NULL)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Extend Tx.MIC for packet failed!, dropping packet!\n"));
			return NULL;
		}

		NdisCopyMemory(pNewNetBuf->pDataPtr, pNetBuf->pDataPtr, bufLen);
	
		pNewNetBuf->pktLen = bufLen;
		SET_OS_PKT_NETDEV(pNewNetBuf, pi);

		/* release old packet and assign the pointer to the new one */
		RELEASE_NDIS_PACKET(pAd, pNetBuf, NDIS_STATUS_SUCCESS);
		pNetBuf = pNewNetBuf;
	}
	
	return OSPKT_TO_RTPKT(pNetBuf);
}


/* */
/* change OS packet DataPtr and DataLen */
/* */
VOID RtmpOsPktInit(
	IN	PNDIS_PACKET		pRxPacket,
	IN	PNET_DEV			pNetDev,
	IN	UCHAR				*pData,
	IN	USHORT				DataSize)
{
	DCU pMblkPkt;

	ASSERT(pRxPacket);
	pMblkPkt = RTPKT_TO_OSPKT(pRxPacket);

	SET_OS_PKT_NETDEV(pMblkPkt, pNetDev);
}


void wlan_802_11_to_802_3_packet(
	IN PNET_DEV pNetDev,
	IN UCHAR OpMode,
	IN USHORT VLAN_VID,
	IN USHORT VLAN_Priority,
	IN PNDIS_PACKET pRxPacket,
	IN UCHAR *pData,
	IN ULONG DataSize,
	IN PUCHAR pHeader802_3,
	IN UCHAR *TPID)
{
	RTMP_RTIP_NET_BUF *pNetPkt;

	ASSERT(pRxBlk->pRxPacket);
	ASSERT(pHeader802_3);

	pNetPkt = RTPKT_TO_OSPKT(pRxBlk->pRxPacket);
	SET_OS_PKT_NETDEV(pNetPkt, pNetDev);

	SET_OS_PKT_DATAPTR(pNetPkt, pRxBlk->pData);
	SET_OS_PKT_LEN(pNetPkt, pRxBlk->DataSize);

#if 0	
	DBGPRINT(RT_DEBUG_TRACE, ("pRxBlk.pData=0x%x,Size=%d!\n", pRxBlk->pData, pRxBlk->DataSize));
	DBGPRINT(RT_DEBUG_TRACE, ("pNetPkt.data=0x%x(0x%x), Size=%d", pNetPkt->pDataPtr, pNetPkt->pFrameBuf, pNetPkt->pktLen));
	hex_dump("pHeader", pHeader802_3, 14);
	hex_dump("pRxBlk", pRxBlk->pData, pRxBlk->DataSize);
#endif

	/* copy 802.3 header */
#ifdef CONFIG_AP_SUPPORT
/*	IF_DEV_CONFIG_OPMODE_ON_AP(pAd) */
	if (OpMode == OPMODE_AP)
	{
		/* maybe insert VLAN tag to the received packet */
		UCHAR VLAN_Size = 0;
		UCHAR *data_p;

		if (VLAN_VID != 0)
			VLAN_Size = LENGTH_802_1Q;

		data_p = skb_push(pNetPkt, LENGTH_802_3+VLAN_Size);

		VLAN_8023_Header_Copy(VLAN_VID, VLAN_Priority,
								pHeader802_3, LENGTH_802_3,
								data_p, TPID);
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
/*	IF_DEV_CONFIG_OPMODE_ON_STA(pAd) */
	if (OpMode == OPMODE_STA)
		NdisMoveMemory(skb_push(pNetPkt, LENGTH_802_3), pHeader802_3, LENGTH_802_3);
#endif /* CONFIG_STA_SUPPORT */
#if 0
	DBGPRINT(RT_DEBUG_TRACE, ("AfHdrCp: pNetPkt.Data=0x%x, len=%d\n", 
				GET_OS_PKT_DATAPTR(pNetPkt), GET_OS_PKT_LEN(pNetPkt)));
	hex_dump("pMblkPkt", GET_OS_PKT_DATAPTR(pNetPkt), GET_OS_PKT_LEN(pNetPkt));
#endif
}


void announce_802_3_packet(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket)
{
	PIFACE pNetDev;
	DCU pDcu;
	UINT32 PktLen;
	RTMP_RTIP_NET_BUF *pNetBuf;
	
	pNetDev  = GET_OS_PKT_NETDEV(pPacket);

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (MATPktRxNeedConvert(pAd, pNetDev))
			MATEngineRxHandle(pAd, pPacket, 0);
	}
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef ETH_CONVERT_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->EthConvert.ECMode & ETH_CONVERT_MODE_DONGLE)
			MATEngineRxHandle(pAd, pPacket, 0);
	}
#endif /* ETH_CONVERT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

#ifdef EBS_RTIP
	pNetBuf = (RTMP_RTIP_NET_BUF *)pPacket;
	if (pNetBuf->pDcuBuf == NULL)
	{
		PktLen = GET_OS_PKT_LEN(pPacket);
		pDcu = os_alloc_packet_input(PktLen, DRIVER_ALLOC);
		if (pDcu)
		{
			memcpy(DCUTODATA(pDcu), (void *)GET_OS_PKT_DATAPTR(pPacket), PktLen);
			DCUTOPACKET(pDcu)->length = PktLen;
#if TCP_PARTIAL_CHKSUM
			DCUTOPACKET(pDcu)->tcp_partial_chksum_present = 0;
#endif
		}
	}
	else
	{
		pDcu = pNetBuf->pDcuBuf;
	}

	if(pDcu)
	{
		/*hex_dump("AnnounceDcu", DCUTODATA(pDcu), DCUTOPACKET(pDcu)->length); */
		/*RtmpTCPSeqCheck(pDcu); */

		/* Signal Stack */
#if (RTIP_VERSION > 24)
		ks_invoke_input(pNetDev, pDcu);
#else
		os_sndx_input_list(pNetDev, pDcu) ;
		ks_invoke_input(pNetDev);
#endif

		/* We clear the DCU buffer pointer here, because it will freed by upper layer */
		pNetBuf->pDcuBuf = NULL;
		pNetBuf->bDcuFlg = 0;
	}
	else
	{
		dc_log_printf("get dcu(sz=%d) failed\n", PktLen);
	}
#endif /* EBS_RTIP */
	RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_SUCCESS);

}


#ifdef CONFIG_SNIFFER_SUPPORT
void STA_MonPktSend(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RX_BLK			*pRxBlk)
{

}
#endif /* CONFIG_STA_SUPPORT */


PRTMP_SCATTER_GATHER_LIST
rt_get_sg_list_from_packet(PNDIS_PACKET pPacket, RTMP_SCATTER_GATHER_LIST *sg)
{
    /*Shiang: Not used now!!!! */
    return NULL;
}


/*
========================================================================
Routine Description:
    Close raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
	1. if open fail, kernel will not call the close function.
	2. Free memory for
		(1) Mlme Memory Handler:		MlmeHalt()
		(2) TX & RX:					RTMPFreeTxRxRingMemory()
		(3) BA Reordering: 				ba_reordering_resource_release()
========================================================================
*/
int rt28xx_close(IN VOID *dev)
{
	PIFACE pi = (PIFACE)dev;
	RTMP_ADAPTER	*pAd = RTMP_OS_NETDEV_GET_PRIV(pi);
	UINT32			i = 0;

#ifdef RTMP_MAC_USB
	/*DECLARE_WAIT_QUEUE_HEAD(unlink_wakeup); */
	/*DECLARE_WAITQUEUE(wait, current); */

	/*RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_REMOVE_IN_PROGRESS); */
#endif /* RTMP_MAC_USB */


    DBGPRINT(RT_DEBUG_TRACE, ("===> rt28xx_close\n"));

	/* Sanity check for pAd */
	if (pAd == NULL)
		return 0; /* close ok */

#ifdef WMM_ACM_SUPPORT
	/* must call first */
	ACMP_Release(pAd);
#endif /* WMM_ACM_SUPPORT */

#ifdef MESH_SUPPORT
	/* close all mesh link before the interface go down. */
	if (MESH_ON(pAd))
		MeshDown(pAd, TRUE);
#endif /* MESH_SUPPORT */

#ifdef WDS_SUPPORT
	WdsDown(pAd);
#endif /* WDS_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		BOOLEAN Cancelled;
#ifdef RTMP_MAC_PCI
		RTMPPCIeLinkCtrlValueRestore(pAd, RESTORE_CLOSE);
#endif /* RTMP_MAC_PCI */

		/* If dirver doesn't wake up firmware here, */
		/* NICLoadFirmware will hang forever when interface is up again. */
		if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
        {      
		    AsicForceWakeup(pAd, TRUE);
        }

#ifdef QOS_DLS_SUPPORT
		/* send DLS-TEAR_DOWN message, */
		if (pAd->CommonCfg.bDLSCapable)
		{
			UCHAR i;

			/* tear down local dls table entry */
			for (i=0; i<MAX_NUM_OF_INIT_DLS_ENTRY; i++)
			{
				if (pAd->StaCfg.DLSEntry[i].Valid && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH))
				{
					RTMPSendDLSTearDownFrame(pAd, pAd->StaCfg.DLSEntry[i].MacAddr);
					pAd->StaCfg.DLSEntry[i].Status	= DLS_NONE;
					pAd->StaCfg.DLSEntry[i].Valid	= FALSE;
				}
			}

			/* tear down peer dls table entry */
			for (i=MAX_NUM_OF_INIT_DLS_ENTRY; i<MAX_NUM_OF_DLS_ENTRY; i++)
			{
				if (pAd->StaCfg.DLSEntry[i].Valid && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH))
				{
					RTMPSendDLSTearDownFrame(pAd, pAd->StaCfg.DLSEntry[i].MacAddr);
					pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
					pAd->StaCfg.DLSEntry[i].Valid	= FALSE;
				}
			}
			RTMP_MLME_HANDLER(pAd);
		}
#endif /* QOS_DLS_SUPPORT */

		if (INFRA_ON(pAd) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
		{
			MLME_DISASSOC_REQ_STRUCT	DisReq;
			MLME_QUEUE_ELEM *MsgElem = (MLME_QUEUE_ELEM *) kmalloc(sizeof(MLME_QUEUE_ELEM), MEM_ALLOC_FLAG);
			if (MsgElem != NULL)
			{
				COPY_MAC_ADDR(DisReq.Addr, pAd->CommonCfg.Bssid);
				DisReq.Reason =  REASON_DEAUTH_STA_LEAVING;

				MsgElem->Machine = ASSOC_STATE_MACHINE;
				MsgElem->MsgType = MT2_MLME_DISASSOC_REQ;
				MsgElem->MsgLen = sizeof(MLME_DISASSOC_REQ_STRUCT);
				NdisMoveMemory(MsgElem->Msg, &DisReq, sizeof(MLME_DISASSOC_REQ_STRUCT));

				/* Prevent to connect AP again in STAMlmePeriodicExec */
				pAd->MlmeAux.AutoReconnectSsidLen= 32;
				NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen);

				pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_OID_DISASSOC;
				MlmeDisassocReqAction(pAd, MsgElem);
				kfree(MsgElem);
			
				RtmpusecDelay(1000);
			}
		}

#ifdef RTMP_MAC_USB
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_REMOVE_IN_PROGRESS);
#endif /* RTMP_MAC_USB */

#ifdef CCX_SUPPORT
		RTMPCancelTimer(&pAd->StaCfg.LeapAuthTimer, &Cancelled);
#endif

		RTMPCancelTimer(&pAd->StaCfg.StaQuickResponeForRateUpTimer, &Cancelled);
		RTMPCancelTimer(&pAd->StaCfg.WpaDisassocAndBlockAssocTimer, &Cancelled);

#ifdef WPA_SUPPLICANT_SUPPORT
#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
		/* send wireless event to wpa_supplicant for infroming interface down. */
		RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CUSTOM, RT_INTERFACE_DOWN, NULL, NULL, 0);
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
#endif /* WPA_SUPPLICANT_SUPPORT */

		MlmeRadioOff(pAd);
#ifdef RTMP_MAC_PCI
		pAd->bPCIclkOff = FALSE;    
#endif /* RTMP_MAC_PCI */
	}
#endif /* CONFIG_STA_SUPPORT */

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

	for (i = 0 ; i < NUM_OF_TX_RING; i++)
	{
		while (pAd->DeQueueRunning[i] == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Waiting for TxQueue[%d] done..........\n", i));
			RtmpusecDelay(1000);
		}
	}
	
#ifdef RTMP_MAC_USB
	/* ensure there are no more active urbs. */
	/*add_wait_queue (&unlink_wakeup, &wait); */
	/*pAd->wait = &unlink_wakeup; */

	/* maybe wait for deletions to finish. */
	i = 0;
	while(i < 25)
	{
		unsigned long IrqFlags;

		RTMP_IRQ_LOCK(&pAd->BulkInLock, IrqFlags);
		if (pAd->PendingRx == 0)
		{
			RTMP_IRQ_UNLOCK(&pAd->BulkInLock, IrqFlags);
			break;
		}
		RTMP_IRQ_UNLOCK(&pAd->BulkInLock, IrqFlags);
                RtmpusecDelay(UNLINK_TIMEOUT_MS*1000);	/*Time in microsecond */
		i++;
	}
	/*pAd->wait = NULL; */
	/*remove_wait_queue (&unlink_wakeup, &wait); */
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_AP_SUPPORT

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef RTMP_MAC_USB
	  /*RTMPCancelTimer(&pAd->CommonCfg.BeaconUpdateTimer, &Cancelled); */
#endif /* RTMP_MAC_USB */

#ifdef DOT11N_DRAFT3
		if (pAd->CommonCfg.Bss2040CoexistFlag & BSS_2040_COEXIST_TIMER_FIRED)
		{
			RTMPCancelTimer(&pAd->CommonCfg.Bss2040CoexistTimer, &Cancelled);
			pAd->CommonCfg.Bss2040CoexistFlag  = 0;
		}
#endif /* DOT11N_DRAFT3 */

		/* PeriodicTimer already been canceled by MlmeHalt() API. */
		/*RTMPCancelTimer(&pAd->PeriodicTimer,	&Cancelled); */
	}
#endif /* CONFIG_AP_SUPPORT */

	/* Close kernel threads or tasklets */
	RtmpMgmtTaskExit(pAd);

	/* Stop Mlme state machine */
	MlmeHalt(pAd);

	/* Close kernel threads or tasklets */
	RtmpNetTaskExit(pAd);


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		MacTableReset(pAd, 1);
#ifdef MAT_SUPPORT
		MATEngineExit(pAd);
#endif /* MAT_SUPPORT */
	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef MAT_SUPPORT
		MATEngineExit(pAd);
#endif /* MAT_SUPPORT */

		/* Shutdown Access Point function, release all related resources */
		APShutdown(pAd);

		/* Free BssTab & ChannelInfo tabbles. */
		AutoChBssTableDestroy(pAd);
		ChannelInfoDestroy(pAd);
	}
#endif /* CONFIG_AP_SUPPORT */

	MeasureReqTabExit(pAd);
	TpcReqTabExit(pAd);

#ifdef WSC_INCLUDED
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		WscStop(pAd, FALSE, &pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		WscStop(pAd,
#ifdef CONFIG_AP_SUPPORT
				FALSE,
#endif /* CONFIG_AP_SUPPORT */
				&pAd->StaCfg.WscControl);
#endif /* CONFIG_STA_SUPPORT */

#ifdef CRYPT_GPL_ALGORITHM 
#ifdef OLD_DH_KEY
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	    	WSC_VFREE_KEY_MEM(pAd->ApCfg.MBSSID[0].WscControl.pPubKeyMem, pAd->ApCfg.MBSSID[0].WscControl.pSecKeyMem);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		WSC_VFREE_KEY_MEM(pAd->StaCfg.WscControl.pPubKeyMem, pAd->StaCfg.WscControl.pSecKeyMem);
#endif /* CONFIG_STA_SUPPORT */
#endif /* OLD_DH_KEY */

#ifndef OLD_DH_KEY
	DH_freeall();
#endif /* OLD_DH_KEY */
#endif /* CRYPT_GPL_ALGORITHM */

	/* WSC hardware push button function 0811 */
	WSC_HDR_BTN_Stop(pAd);
#endif /* WSC_INCLUDED */

#ifdef RTMP_MAC_PCI
	{
			BOOLEAN brc;
			/*	ULONG			Value; */

			if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE))
			{
				RTMP_ASIC_INTERRUPT_DISABLE(pAd);
			}

			/* Receive packets to clear DMA index after disable interrupt. */
			/*RTMPHandleRxDoneInterrupt(pAd); */
			/* put to radio off to save power when driver unload.  After radiooff, can't write /read register.  So need to finish all */
			/* register access before Radio off. */


			brc=RT28xxPciAsicRadioOff(pAd, RTMP_HALT, 0);
			if (brc==FALSE)
			{
				DBGPRINT(RT_DEBUG_ERROR,("%s call RT28xxPciAsicRadioOff fail !!\n", __FUNCTION__)); 
			}
	}
	

/*
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE))
	{
		RTMP_ASIC_INTERRUPT_DISABLE(pAd);
	}

	/* Disable Rx, register value supposed will remain after reset */
	NICIssueReset(pAd);
*/

	/* Free IRQ */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
	{
		/* Deregister interrupt function */
		RtmpOSIRQRelease(pEndObj);
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);
	}
#endif /* RTMP_MAC_PCI */

	/* Free Ring or USB buffers */
	RTMPFreeTxRxRingMemory(pAd);

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

#ifdef DOT11_N_SUPPORT
	/* Free BA reorder resource */
	ba_reordering_resource_release(pAd);
#endif /* DOT11_N_SUPPORT */

#ifdef RTMP_MAC_USB
#ifdef INF_AMAZON_SE
	if (pAd->UsbVendorReqBuf)
		os_free_mem(pAd, pAd->UsbVendorReqBuf);
#endif /* INF_AMAZON_SE */
#endif /* RTMP_MAC_USB */


	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_START_UP);

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt28xx_close\n"));
	
	return 0; /* close ok */
}


/*
========================================================================
Routine Description:
    Open raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
========================================================================
*/
int rt28xx_open(IN VOID *dev)
{				 
	PIFACE *pi = (PIFACE)dev;
	PRTMP_ADAPTER pAd;
	int retval = 0;


	 pAd = RTMP_OS_NETDEV_GET_PRIV(pi);
	/* Sanity check for pAd */
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free; So the net_dev->priv will be NULL in 2rd open */
		DBGPRINT(RT_DEBUG_ERROR, ("pAd is NULL!\n"));
		return -1;
	}

	/* Request interrupt service routine for PCI device */
	/* register the interrupt routine with the os */

	/* Init IRQ parameters */
	rtmp_irq_init(pAd);
	
	/* Chip & other init */
	if (rt28xx_init(pAd, NULL, NULL) == FALSE)
	{
		dc_log_printf("rt28xx_init failed\n");
		goto err;
	}

#ifdef MBSS_SUPPORT
	/* the function can not be moved to RT2860_probe() even register_netdev()
	   is changed as register_netdevice().
	   Or in some PC, kernel will panic (Fedora 4) */
	RT28xx_MBSS_Init(pAd, pAd->net_dev);
#endif /* MBSS_SUPPORT */

#ifdef WDS_SUPPORT
	RT28xx_WDS_Init(pAd, pAd->net_dev);
#endif /* WDS_SUPPORT */

#ifdef APCLI_SUPPORT
	RT28xx_ApCli_Init(pAd, pAd->net_dev);
#endif /* APCLI_SUPPORT */

#ifdef MESH_SUPPORT
	RTMP_Mesh_Init(pAd, pAd->net_dev, pHostName);
#endif /* MESH_SUPPORT */	

	/* Enable Interrupt */
	RTMP_IRQ_ENABLE(pAd);

	/* Now Enable RxTx */
	RTMPEnableRxTx(pAd);
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_START_UP);

	{
		UINT32 reg = 0;
		RTMP_IO_READ32(pAd, 0x1300, &reg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x1300 = %08x\n", reg));
	}

	return (retval);

err:
	return (-1);
}


/*
========================================================================
Routine Description:
    The entry point for threadX sent packet to our driver.

Arguments:
    RTMP_RTIP_NET_BUF *pNetPkt		the pointer refer to a RTMP_RTIP_NET_BUF.

Return Value:
    0					

Note:
	This function is the entry point of Tx Path for Os delivery packet to 
	our driver. You only can put OS-depened & STA/AP common handle procedures 
	in here.
========================================================================
*/
int rt28xx_packet_xmit(RTMP_RTIP_NET_BUF *pNetPkt)
{
	int status = 0;
	PIFACE pi;
	RTMP_ADAPTER *pAd;
	PNDIS_PACKET pPacket, pNewPacket;
	
	pi = GET_OS_PKT_NETDEV(pNetPkt);
	pAd =  (RTMP_ADAPTER *)(RTMP_OS_NETDEV_GET_PRIV(pi));
	pPacket= (PNDIS_PACKET) pNetPkt;

	/* RT2870STA does this in RTMPSendPackets() */
#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
	{
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_RESOURCES);
		return 0;
	}
#endif /* CONFIG_ATE */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Drop send request since we are in monitor mode */
		if (MONITOR_ON(pAd))
		{
			RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
			goto done;
		}
	}
#endif /* CONFIG_STA_SUPPORT */

        /* EapolStart size is 18 */
	if ( GET_OS_PKT_LEN(pNetPkt) < 14)
	{
		/*DBGPRINT(RT_DEBUG_TRACE, ("bad packet size: %d\n", pkt->len)); */
		hex_dump("bad packet", GET_OS_PKT_DATAPTR(pNetPkt), GET_OS_PKT_LEN(pNetPkt));
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
		goto done;
	}

#ifdef CONFIG_5VT_ENHANCE
	RTMP_SET_PACKET_5VT(pPacket, 0);
#endif /* CONFIG_5VT_ENHANCE */

#ifdef CONFIG_STA_SUPPORT
#ifdef ETH_CONVERT_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* TODO: For ThreadX, need to add code segment to do ether convert support */
		/* Don't move this checking into wdev_tx_pkts(), becasue the net_device is OS-depeneded. */
	}
#endif /* ETH_CONVERT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	wdev_tx_pkts((NDIS_HANDLE)pAd, (PPNDIS_PACKET) &pPacket, 1);

	status = 0;

done:
			   
	return status;
}


/*
========================================================================
Routine Description:
    Send a packet to WLAN.

Arguments:
    skb_p           points to our adapter
    dev_p           which WLAN network interface

Return Value:
    0: transmit successfully
    otherwise: transmit fail

Note:
========================================================================
*/
int rt28xx_send_packets(
	PIFACE pi,
	DCU pDcu)
{
	RTMP_RTIP_NET_BUF *pNetBuf;
	int status = 0;
	
#if 0	
	NDIS_STATUS		status = NDIS_STATUS_FAILURE;	
	PUCHAR pDstMac;
	int len;
	PUCHAR *pPi;
	PIFACE pTmpPi;
	PETHER pEthHdr;
	DCU msg;


	len = DCUTOPACKET(pDcu)->length;
	pEthHdr = DCUTOETHERPKT(pDcu);
	pDstMac = DCUDATA(pDcu);

	dc_log_printf("AAAAA,RtmpIfaceId=%d,pi=0x%x", RtmpIfaceId, pi);
	dc_log_printf("DCU(0x%x)->Len=%d", pDcu, pDcu->length);
		
	dc_log_printf("TxDa:%02x:%02x:%02x:%02x:%02x:%02x", PRINT_MAC(pEthHdr->eth_dest));
	dc_log_printf("TxSa:%02x:%02x:%02x:%02x:%02x:%02x", PRINT_MAC(pEthHdr->eth_source));
	dc_log_printf("TxTp:0x%04x", pEthHdr->eth_type);
	hex_dump("pTxDcu", pDstMac, len);
#endif

	pNetBuf = RtmpTxNetPktCheck(pi, pDcu);
	if (pNetBuf)
		status = rt28xx_packet_xmit(pNetBuf);
#if 0
	pi->xmit_status = 0;
	ks_invoke_output(pi, 1);
#endif
	return status;
	
}


RTP_BOOL rt28xx_send_packets_done(
	IN struct _iface RTP_FAR *pi,
	IN DCU msg, 
	IN RTP_BOOL success)
{
#if 0
	PETHER pEthHdr;
	
	dc_log_printf("BBBBBBB,len=%d", DCUTOPACKET(msg)->length);

	pEthHdr = DCUTOETHERPKT(msg);

	dc_log_printf("DTxDa:%02x:%02x:%02x:%02x:%02x:%02x", PRINT_MAC(pEthHdr->eth_dest));
	dc_log_printf("DTxSa:%02x:%02x:%02x:%02x:%02x:%02x", PRINT_MAC(pEthHdr->eth_source));
	dc_log_printf("DTxTp:0x%04x", pEthHdr->eth_type);
#endif

	return 1;
}



/*
    ========================================================================

    Routine Description:
        return ethernet statistics counter

    Arguments:
        net_dev                     Pointer to net_device

    Return Value:
        net_device_stats*

    Note:

    ========================================================================
*/
static RTP_BOOL RT28xx_get_ether_stats(
    IN  struct net_device *net_dev)
{

	return 0;
}


/*
	This DEVICE ID should also defined in rtipapi.h
*/
#define RT_WIFI_DEVICE_ID	52 /* Ralink Wireless Device */

EDEVTABLE RTP_FAR RT_WIFI_Device=
{
	MainVirtualIF_open, /* DEVICE_OPEN open */
	MainVirtualIF_close, /* DEVICE_CLOSE close */
	rt28xx_send_packets, /* DEVICE_XMIT xmit */
	rt28xx_send_packets_done, /* DEVICE_XMIT_DONE xmit_done */
	NULLP_FUNC, /* DEVICE_PROCESS_INTERRUPTS proc_interrupts */
	NULLP_FUNC, /* DEVICE_STATS statistics */
	NULLP_FUNC, /* DEVICE_SETMCAST setmcast */
	/*RT_WIFI_DEVICE_ID*/MARVELL8388_DEVICE, /* int  device_id, same as device type in iface struct */
	"ra", /* char device_name[MAX_DEVICE_NAME_LEN] */
	MINOR_0, /* int minor_number; if one device of a type it is 0, if two devices, the devices are 0 and 1, etc.*/
	ETHER_IFACE, /* interface type (ETHER_IFACE for example) */
	SNMP_DEVICE_INFO(CFG_OID_NETWORK, CFG_SPEED_NETWORK)  /* mib and bandwidth in bits/sec definitions specific to the interface */
	CFG_ETHER_MAX_MTU, /* RTP_UINT16        mtu */
	CFG_ETHER_MAX_MSS, /* RTP_UINT16        max_mss_out */
	CFG_ETHER_MAX_WIN_IN, /* RTP_UINT16      window_size_in */
	CFG_ETHER_MAX_WIN_OUT, /* RTP_UINT16      window_size_out */
	EN(0), /* defaults */
	EN(0), /* default2 */
	EN(0) /* default3 */
};


int xn_bind_rt_wifi(int minor)
{
	/* Register RT_WIFI Device Interface to RTIP device table */
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): DevTable Add\n", __FUNCTION__));
	if (xn_device_table_add(RT_WIFI_Device.device_id, minor,
					RT_WIFI_Device.iface_type,
					RT_WIFI_Device.device_name,
					SNMP_DEVICE_INFO(RT_WIFI_Device.media_mib, RT_WIFI_Device.speed)
                                   (DEV_OPEN)RT_WIFI_Device.open,
					(DEV_CLOSE)RT_WIFI_Device.close,
					(DEV_XMIT)RT_WIFI_Device.xmit,
					(DEV_XMIT_DONE)RT_WIFI_Device.xmit_done,
					(DEV_PROCESS_INTERRUPTS)RT_WIFI_Device.proc_interrupts,
					(DEV_STATS)RT_WIFI_Device.statistics,
					(DEV_SETMCAST)RT_WIFI_Device.setmcast) != 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("RT_WIFI: DevTable add failed\n"));
		return -1;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): RT_WIFI: DevTable add success!\n", __FUNCTION__));
}


/*
  *	Assign the network dev name for created Ralink WiFi interface.
  */
static int RtmpOSNetDevRequestName(
	IN RTMP_ADAPTER *pAd, 
	IN RTMP_STRING *pPrefixStr, 
	IN INT	devIdx,
	OUT INT	*avilNum)
{
	int	ifNameIdx;
	int	Status;
	
	if(ifNameIdx < 32)
	{
		*avilNum = ifNameIdx;
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("Assign the net device name as %s%d\n", pPrefixStr, *avilNum));
#endif /* RELEASE_EXCLUDE */
		Status = NDIS_STATUS_SUCCESS;
	}
	else 
	{
		DBGPRINT(RT_DEBUG_ERROR, 
					("Cannot request DevName with preifx(%s) and in range(0~32) as suffix from OS!\n", pPrefixStr));
		Status = NDIS_STATUS_FAILURE;
	}

	return Status;
}


void RtmpOSNetDevClose(
	IN PNET_DEV pNetDev)
{

}


void RtmpOSNetDevFree(PNET_DEV pNetDev)
{
	ASSERT(pNetDev);
}


int RtmpOSNetDevAddrSet(
	IN UCHAR	OpMode,
	IN PNET_DEV pNetDev,
	IN PUCHAR	pMacAddr,
	IN PUCHAR	dev_name)
{
	NdisMoveMemory(&pNetDev->addr.my_hw_addr[0], pMacAddr, ETH_ALEN);
	return 0;
}


void RtmpOSNetDevDetach(
	IN PNET_DEV pNetDev)
{
	void *pCookie = (void *)(pNetDev);
}


int RtmpOSNetDevAttach(
	IN PNET_DEV pNetDev, 
	IN RTMP_OS_NETDEV_OP_HOOK *pDevOpHook)
{
	int ifaceId;

	/*	
		For ThreadX, the pNetDev ID should be NULL here.
	*/

	DBGPRINT(RT_DEBUG_ERROR, ("Open ra0 interface\n"));
	ifaceId = xn_interface_open_config(RT_WIFI_Device.device_id, MINOR_0,0,0,0);
	if (ifaceId == -1)
	{
		dc_log_printf("%s(): Iface Open FAIL\n", __FUNCTION__) ;
                return -1;
	}

	RtmpIfaceId = ifaceId;

	return NDIS_STATUS_SUCCESS;
}


PNET_DEV RtmpOSNetDevCreate(
	IN RTMP_ADAPTER *pAd,
	IN INT 			devType, 
	IN INT			devNum,
	IN INT			privMemSize,
	IN RTMP_STRING *pNamePrefix)
{
	PIFACE pNetDev = NULL;
	struct os_cookie	*pObj = pAd->OS_Cookie;
	int				status, availableNum;
	void 			*endLoadFunc;
	char				devName[IFNAMSIZ];


	memset(devName, 0, IFNAMSIZ);

	ASSERT((strlen(pNamePrefix) < IFNAMSIZ));
	strcpy(devName, pNamePrefix);
#ifdef MULTIPLE_CARD_SUPPORT
	if (pAd->MC_RowID >= 0)
	{
		char suffixName[4];
		sprintf(suffixName, "%02d_", pAd->MC_RowID);
		strcat(devName, suffixName);
	}
#endif /* MULTIPLE_CARD_SUPPORT */

	status = RtmpOSNetDevRequestName(pAd, devName, devNum, &availableNum);
	if (status == NDIS_STATUS_FAILURE)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): request devNum failed!\n", __FUNCTION__));
		return NULL;
	}

	return pNetDev;
}

#endif /* EBS_RTIP */

