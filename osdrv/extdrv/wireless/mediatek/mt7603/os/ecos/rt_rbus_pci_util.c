/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rt_rbus_pci_util.c

	Abstract:

	Revision History:
	Who			When		What
    --------    ----------      ------------------------------------------
*/

#include "rt_config.h"

/***********************************************************************************
 *	Memory pool implement
 ***********************************************************************************/ 
#define MemPool_TYPE_Header 3
#define MemPool_TYPE_MBUF 4
#define BUFFER_HEAD_RESVERD 64

PUCHAR RTMP_MemPool_Alloc (
	IN PRTMP_ADAPTER pAd,
	IN ULONG Length,
	IN INT Type)
{
	/* Note: pAd maybe NULL */
    struct mbuf *pMBuf = NULL;

	switch (Type)
	{        
		case MemPool_TYPE_Header:
			MGETHDR(pMBuf, M_DONTWAIT, MT_DATA);
                        pMBuf->m_len = 0;
            break;
		case MemPool_TYPE_MBUF:
			MGETHDR(pMBuf, M_DONTWAIT, MT_DATA);                        
			if (pMBuf== NULL)
				return NULL;
			MCLGET(pMBuf, M_DONTWAIT);
			if ((pMBuf->m_flags & M_EXT) == 0)
                        {
				m_freem(pMBuf);
                                return NULL;
                        }
                        pMBuf->m_len = 0;
                        break;
        default:
	        DBGPRINT_ERR(("%s: Unknown Type %d\n", __FUNCTION__, Type));
    		break;
	}

	if (pMBuf != NULL)
		MEM_DBG_PKT_ALLOC_INC(pAd);

    return (PUCHAR)pMBuf;
}

VOID RTMP_MemPool_Free (
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pBuffer,
	IN INT Type)
{
	/* Note: pAd maybe NULL */

	if (pBuffer == NULL)
	{
		DBGPRINT_ERR(("%s: Error buffer pointer \n", __FUNCTION__));
       	goto exit;
	}

	switch (Type)
	{
        case MemPool_TYPE_Header:
            m_freem(pBuffer);
			MEM_DBG_PKT_FREE_INC(pAd);
            break;
        case MemPool_TYPE_MBUF:
            m_freem(pBuffer);
			MEM_DBG_PKT_FREE_INC(pAd);
            break;
        default:
	        DBGPRINT_ERR(("%s: Unknown Type %d\n", __FUNCTION__, Type));
            break;
    }
    
exit:
	return;
}

/***************************************************************************
  *
  *	Interface-depended memory allocation/Free related procedures.
  * Mainly for Hardware TxDesc/RxDesc/MgmtDesc, DMA Memory for TxData/RxData, etc.,
  *
  **************************************************************************/
#define DescMBuf_SIZE (NUM_OF_TX_RING*3)
static ECOS_DESC_BUFFER DescMBuf[DescMBuf_SIZE];
void RTMP_AllocateDescMemory(
	IN	PPCI_DEV pPciDev,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
    int i = 0;
    for (i = 0; i < DescMBuf_SIZE;i++)
    {
        if (DescMBuf[i].pMBuf == NULL)
        {
            DescMBuf[i].pMBuf = (struct mbuf *)RTMP_MemPool_Alloc(NULL, Length, MemPool_TYPE_MBUF);
            if (DescMBuf[i].pMBuf == NULL)
            {
        		DBGPRINT_ERR(("%s: Can't allocate buffer from memory pool\n"
        		, __FUNCTION__));
                goto err;
            }

            HAL_DCACHE_INVALIDATE(mtod(DescMBuf[i].pMBuf, PUCHAR), Length);
        	if (Cached)
        		*VirtualAddress = (PVOID) CYGARC_CACHED_ADDRESS(mtod(DescMBuf[i].pMBuf, PUCHAR));
        	else
        		*VirtualAddress = (PVOID) CYGARC_UNCACHED_ADDRESS(mtod(DescMBuf[i].pMBuf, PUCHAR));
        	*PhysicalAddress = (PNDIS_PHYSICAL_ADDRESS) CYGARC_PHYSICAL_ADDRESS(*VirtualAddress);
            DescMBuf[i].PhysicalAddress = *PhysicalAddress;
            return;
        }    
    }

err:
    *VirtualAddress = (PVOID) NULL;
    *PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) NULL;
    return;
}


/* Function for Tx/Rx/Mgmt Desc Memory allocation. */
void RtmpAllocDescBuf(
	IN PPCI_DEV pPciDev,
	IN UINT Index,
	IN ULONG Length,
	IN BOOLEAN Cached,
	OUT VOID **VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
    RTMP_AllocateDescMemory(pPciDev, Length, Cached, VirtualAddress, PhysicalAddress);
}


/* Function for free allocated Desc Memory. */
void RtmpFreeDescBuf(
	IN	PPCI_DEV	pPciDev,
	IN	ULONG	Length,
	IN	PVOID	VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
    int i = 0;
    for (i = 0; i < DescMBuf_SIZE;i++)
    {
        if ((DescMBuf[i].pMBuf != NULL) 
            && ((ULONG)(DescMBuf[i].PhysicalAddress) == (ULONG)PhysicalAddress))
        {
            RTMP_MemPool_Free(NULL, (PUCHAR) DescMBuf[i].pMBuf, MemPool_TYPE_MBUF);
            DescMBuf[i].pMBuf = NULL;
            DescMBuf[i].PhysicalAddress = NULL;
            return;
        }    
    }
}

/*EddyTODO */
/* Function for TxData DMA Memory allocation. */
static ULONG FirstTx_SIZE[NUM_OF_TX_RING][TX_RING_SIZE * TX_DMA_1ST_BUFFER_SIZE];
static INT FirstTx_Index = 0;
void RTMP_AllocateFirstTxBuffer(
	IN	PPCI_DEV pPciDev,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	if (Cached)
		*VirtualAddress = (PVOID) CYGARC_CACHED_ADDRESS(&FirstTx_SIZE[FirstTx_Index][0]);
	else
		*VirtualAddress = (PVOID) CYGARC_UNCACHED_ADDRESS(&FirstTx_SIZE[FirstTx_Index][0]);

    FirstTx_Index++;
    if (FirstTx_Index == NUM_OF_TX_RING)
        FirstTx_Index = 0;
	*PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) CYGARC_PHYSICAL_ADDRESS(*VirtualAddress);
}


void RTMP_FreeFirstTxBuffer(
	IN	PPCI_DEV				pPciDev,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	IN	PVOID	VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
}

PECOS_PKT_BUFFER RTMP_AllocatePacketHeader (
	IN	PRTMP_ADAPTER pAd)
{
    PECOS_PKT_BUFFER pPacket;
    struct mbuf     *pMBuf;

	pMBuf = (struct mbuf *)RTMP_MemPool_Alloc(pAd, sizeof(ECOS_PKT_BUFFER), MemPool_TYPE_Header);
	if (pMBuf == NULL)
		return NULL;
	pPacket = (PECOS_PKT_BUFFER) pMBuf->m_data;
	if (pPacket != NULL)
        NdisZeroMemory(pPacket, sizeof(ECOS_PKT_BUFFER));
	pPacket->pHeaderMBuf = pMBuf;

    return pPacket;
}


/*
 * FUNCTION: Allocate a packet buffer for DMA
 * ARGUMENTS:
 *     AdapterHandle:  AdapterHandle
 *     Length:  Number of bytes to allocate
 *     Cached:  Whether or not the memory can be cached
 *     VirtualAddress:  Pointer to memory is returned here
 *     PhysicalAddress:  Physical address corresponding to virtual address
 * Notes:
 *     Cached is ignored: always cached memory
 */

PNDIS_PACKET RTMP_AllocateRxPacketBuffer(
	IN	VOID					*pReserved,
	IN	VOID					*pPciDev,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
    PRTMP_ADAPTER	 pAd = (PRTMP_ADAPTER)pReserved;
    PECOS_PKT_BUFFER pPacket;
    
	pPacket = RTMP_AllocatePacketHeader(pReserved);
	if (pPacket == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN,("%s: Can't allocate packet structure\n"
		, __FUNCTION__));
        goto err;
    }

#ifdef DOT11_N_SUPPORT
    if ((mbstat.m_clfree == 0) && (pAd->ContinueMemAllocFailCount > 5))
    {
		pAd->ContinueMemAllocFailCount = 0;
        BaReOrderingBufferMaintain(pAd);
    }
#endif /* DOT11_N_SUPPORT */

    pPacket->pDataMBuf = (struct mbuf *)RTMP_MemPool_Alloc(pReserved, Length, MemPool_TYPE_MBUF);
	if (pPacket->pDataMBuf == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: Can't allocate buffer from memory pool\n"
		, __FUNCTION__));
#ifdef DOT11_N_SUPPORT
                pAd->ContinueMemAllocFailCount++;
#endif /* DOT11_N_SUPPORT */
        goto release_pkt_header;
    }

    HAL_DCACHE_INVALIDATE(mtod(pPacket->pDataMBuf, PUCHAR), Length);
        pPacket->pDataPtr = (PUCHAR) CYGARC_CACHED_ADDRESS(mtod(pPacket->pDataMBuf, PUCHAR));
    NdisZeroMemory(pPacket->pDataPtr, BUFFER_HEAD_RESVERD);
    pPacket->pDataPtr += BUFFER_HEAD_RESVERD;
    pPacket->MemPoolType = MemPool_TYPE_MBUF;
    pPacket->pktLen = 0;

	*VirtualAddress = (PVOID) GET_OS_PKT_DATAPTR(pPacket);	
   	*PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) CYGARC_PHYSICAL_ADDRESS(*VirtualAddress);

    return (PNDIS_PACKET) pPacket;
    
release_pkt_header:
	RTMP_MemPool_Free(pReserved, (PUCHAR) pPacket->pHeaderMBuf, MemPool_TYPE_Header);
err:
    *VirtualAddress = (PVOID) NULL;
    *PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) NULL;
    return NULL;
}

NDIS_STATUS RTMP_AllocateNdisPacket_AppandMbuf(
	IN	PRTMP_ADAPTER	pAd,
	OUT PNDIS_PACKET   *ppPacket,
	IN	struct mbuf    *pMBuf)
{
	PECOS_PKT_BUFFER	pPacket = NULL;

	if (pMBuf == NULL)
	{
		DBGPRINT_ERR(("%s: pMBuf is NULL\n"
		, __FUNCTION__));
        *ppPacket = NULL;
        return NDIS_STATUS_FAILURE;        
    }

	pPacket = RTMP_AllocatePacketHeader(pAd);
	if (pPacket == NULL)
	{
		*ppPacket = NULL;
		DBGPRINT(RT_DEBUG_WARN, ("RTMP_AllocateNdisPacket_AppandMbuf Fail\n"));
		return NDIS_STATUS_FAILURE;
	}

	MEM_DBG_PKT_ALLOC_INC(pAd);
    HAL_DCACHE_FLUSH(mtod(pMBuf, PUCHAR), pMBuf->m_len);    
    pPacket->pDataMBuf = pMBuf;
    pPacket->pDataPtr = (PUCHAR) CYGARC_UNCACHED_ADDRESS(mtod(pPacket->pDataMBuf, PUCHAR));
    pPacket->MemPoolType = MemPool_TYPE_MBUF;
	pPacket->pktLen = pMBuf->m_len;
	pPacket->net_dev = pAd->net_dev;

	*ppPacket = (PNDIS_PACKET) pPacket;
	return NDIS_STATUS_SUCCESS;
}


/* The allocated NDIS PACKET must be freed via RTMPFreeNdisPacket() for TX */
NDIS_STATUS RTMPAllocateNdisPacket(
	IN	VOID			*pAdSrc,
	OUT PNDIS_PACKET	*ppPacket,
	IN	PUCHAR			pHeader,
	IN	UINT			HeaderLen,
	IN	PUCHAR			pData,
	IN	UINT			DataLen)
{
	PRTMP_ADAPTER		pAd = (PRTMP_ADAPTER)pAdSrc;
	PECOS_PKT_BUFFER	pPacket = NULL;

	pPacket = RTMP_AllocatePacketHeader(pAd);
	if (pPacket == NULL)
	{
		*ppPacket = NULL;
		DBGPRINT(RT_DEBUG_WARN, ("RTMPAllocateNdisPacket Fail\n"));
		return NDIS_STATUS_FAILURE;
	}

	// 1. Allocate a packet /* TODO:add LEN_CCMP_HDR + LEN_CCMP_MIC for PMF*/
	pPacket->pDataMBuf = (struct mbuf *)RTMP_MemPool_Alloc(pAd, (HeaderLen + DataLen  + LEN_CCMP_HDR + LEN_CCMP_MIC), MemPool_TYPE_MBUF);
	if (pPacket->pDataMBuf == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: Can't allocate buffer from memory pool\n"
		, __FUNCTION__));
                *ppPacket = NULL;
		RTMP_MemPool_Free(pAd, pPacket->pHeaderMBuf, MemPool_TYPE_Header);
                return NDIS_STATUS_FAILURE;        
        }

    HAL_DCACHE_INVALIDATE(mtod(pPacket->pDataMBuf, PUCHAR), (HeaderLen + DataLen)); 
        pPacket->pDataPtr = (PUCHAR) CYGARC_UNCACHED_ADDRESS(mtod(pPacket->pDataMBuf, PUCHAR));  
        NdisZeroMemory(pPacket->pDataPtr, BUFFER_HEAD_RESVERD);
        pPacket->pDataPtr += BUFFER_HEAD_RESVERD;
        pPacket->MemPoolType = MemPool_TYPE_MBUF;
	pPacket->pktLen = 0;
	pPacket->net_dev = pAd->net_dev;

	if ((pHeader != NULL) && (HeaderLen > 0))
		NdisMoveMemory(GET_OS_PKT_DATAPTR(pPacket), pHeader, HeaderLen);

	if ((pData != NULL) && (DataLen > 0))
		NdisMoveMemory(GET_OS_PKT_DATAPTR(pPacket) + HeaderLen, pData, DataLen);

        if (pData != NULL) {
                GET_OS_PKT_LEN(pPacket) = (HeaderLen + DataLen);
                pPacket->pDataMBuf->m_len = GET_OS_PKT_LEN(pPacket);
        }
	DBGPRINT(RT_DEBUG_INFO, ("%s : pPacket = %p, len = %d\n", 
				__FUNCTION__, pPacket, (HeaderLen + DataLen)));
	*ppPacket = (PNDIS_PACKET) pPacket;
        
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
	IN VOID 		*pAd,
	IN PNDIS_PACKET  pPacket)
{
	PECOS_PKT_BUFFER pPkt;

	if (pPacket != NULL)
	{
        pPkt = (PECOS_PKT_BUFFER) pPacket;
        
       	switch (pPkt->MemPoolType)
       	{
            case MemPool_TYPE_MBUF:
                if (pPkt->pDataMBuf != NULL)
                    RTMP_MemPool_Free(pAd, (PUCHAR) pPkt->pDataMBuf, pPkt->MemPoolType);
                if (pPkt->pHeaderMBuf != NULL)
                    RTMP_MemPool_Free(pAd, (PUCHAR) pPkt->pHeaderMBuf, MemPool_TYPE_Header);
                pPacket = NULL;            
                break;
            default:
                kfree(pPkt);
                break;
        }
    }
    else
        DBGPRINT(RT_DEBUG_ERROR, ("%s: Packet is  NULL\n", __FUNCTION__));    
}

PNDIS_PACKET RtmpOSNetPktAlloc(
	IN VOID 		*pAdSrc,
	IN int size)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;
    PECOS_PKT_BUFFER        pPacket;
	PVOID					AllocVa;
	NDIS_PHYSICAL_ADDRESS	AllocPa;

    pPacket = (PECOS_PKT_BUFFER) RTMP_AllocateRxPacketBuffer(pAd, ((POS_COOKIE)(pAd->OS_Cookie))->pci_dev, size, FALSE, &AllocVa, &AllocPa);
    return (PNDIS_PACKET) pPacket;    
}

PNDIS_PACKET RTMP_AllocateFragPacketBuffer(
	IN VOID 		*pAdSrc,
	IN	ULONG	Length)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;
    PECOS_PKT_BUFFER        pPacket;
	PVOID					AllocVa;
	NDIS_PHYSICAL_ADDRESS	AllocPa;

    pPacket = (PECOS_PKT_BUFFER) RTMP_AllocateRxPacketBuffer(pAd, ((POS_COOKIE)(pAd->OS_Cookie))->pci_dev, Length, FALSE, &AllocVa, &AllocPa);
    return (PNDIS_PACKET) pPacket;
}

VOID build_tx_packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR	pFrame,
	IN	ULONG	FrameLen)
{
	PECOS_PKT_BUFFER	pPkt;

	ASSERT(pPacket);
    pPkt = (PECOS_PKT_BUFFER) pPacket;
    if (FrameLen > 0) {
    	NdisMoveMemory(GET_OS_PKT_DATAPTR(pPkt), pFrame, FrameLen);
    	GET_OS_PKT_TOTAL_LEN(pPkt) += FrameLen;
    }
}


void * skb_put(PECOS_PKT_BUFFER skb, INT len)
{
	PUCHAR p;

	ASSERT(skb);
	p = GET_OS_PKT_DATATAIL(skb);
	GET_OS_PKT_LEN(skb) += len;
	
	return p;
}


void * skb_push(PECOS_PKT_BUFFER skb, INT len)
{
	ASSERT(skb);

	GET_OS_PKT_DATAPTR(skb) -= len;
	GET_OS_PKT_LEN(skb) += len;
		
	return GET_OS_PKT_DATAPTR(skb);
}

void * skb_reserve(PECOS_PKT_BUFFER skb, INT len)
{
	ASSERT(skb);
    
	GET_OS_PKT_DATAPTR(skb) += len;
	GET_OS_PKT_LEN(skb) -= len;
    if (GET_OS_PKT_LEN(skb) < 0)
        GET_OS_PKT_LEN(skb) = 0;
		
	return GET_OS_PKT_DATAPTR(skb);
}


PNDIS_PACKET skb_copy(PNDIS_PACKET pSrcPkt, INT flags)
{
    PRTMP_ADAPTER           pAd = NULL;
    PNET_DEV                pNetDev = NULL;
	POS_COOKIE			    pOSCookie = NULL;
    PECOS_PKT_BUFFER        pPacket = NULL;
    PECOS_PKT_BUFFER        pNewPacket = NULL;
	PVOID					AllocVa;
	NDIS_PHYSICAL_ADDRESS	AllocPa;
    NDIS_STATUS             Status;

    pPacket = (PECOS_PKT_BUFFER) pSrcPkt;
    if (pPacket == NULL) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s: pSrcPkt is NULL\n", __FUNCTION__));
        goto err;
    }

	pNetDev = GET_OS_PKT_NETDEV(pPacket);
    if (pNetDev == NULL) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s: pNetDev is NULL\n", __FUNCTION__));
        goto err;
	}

	pAd = (PRTMP_ADAPTER)GET_OS_PKT_NETDEV(pPacket)->driver_private;
    if (pAd == NULL) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s: pAd is NULL\n", __FUNCTION__));
        goto err;
	}
	pOSCookie = (POS_COOKIE) pAd->OS_Cookie;

	pNewPacket = (PECOS_PKT_BUFFER) RTMP_AllocateRxPacketBuffer(pAd, pPacket->net_dev, RX_BUFFER_AGGRESIZE, FALSE, &AllocVa, &AllocPa);
	
    if (pNewPacket != NULL) {
        pNewPacket->net_dev = pPacket->net_dev;
        pNewPacket->pktLen = pPacket->pktLen;
        NdisCopyMemory(pNewPacket->cb, pPacket->cb, 48);
        NdisCopyMemory(pNewPacket->pDataPtr, pPacket->pDataPtr, pPacket->pktLen);
    }

    return (PNDIS_PACKET) pNewPacket;
err:
    return NULL;
}


PNDIS_PACKET skb_clone(PNDIS_PACKET pSrcPkt, INT flags)
{
    PRTMP_ADAPTER           pAd = NULL;
    PNET_DEV                pNetDev = NULL;
	POS_COOKIE			    pOSCookie = NULL;
    PECOS_PKT_BUFFER        pPacket = NULL;
    PECOS_PKT_BUFFER        pNewPacket = NULL;
	PVOID					AllocVa;
	NDIS_PHYSICAL_ADDRESS	AllocPa;
    NDIS_STATUS             Status;

    pPacket = (PECOS_PKT_BUFFER) pSrcPkt;
    if (pPacket == NULL) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s: pSrcPkt is NULL\n", __FUNCTION__));
        goto err;
    }

	pNetDev = GET_OS_PKT_NETDEV(pPacket);
    if (pNetDev == NULL) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s: pNetDev is NULL\n", __FUNCTION__));
        goto err;
    }

	pAd = (PRTMP_ADAPTER)GET_OS_PKT_NETDEV(pPacket)->driver_private;
    if (pAd == NULL) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s: pAd is NULL\n", __FUNCTION__));
        goto err;
    }
	pOSCookie = (POS_COOKIE) pAd->OS_Cookie;

	pNewPacket = RTMP_AllocatePacketHeader(pAd);
	if (pNewPacket == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: Can't allocate packet structure\n"
		, __FUNCTION__));
        goto err;
    }

    pNewPacket->pDataMBuf = m_copym(pPacket->pDataMBuf, 0, M_COPYALL, M_DONTWAIT);
	if (pNewPacket->pDataMBuf == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: Can't allocate buffer from memory pool\n", __FUNCTION__));
        goto release_pkt_header;
    }

    pNewPacket->MemPoolType = MemPool_TYPE_MBUF;
        pNewPacket->net_dev = pPacket->net_dev;
    NdisCopyMemory(pNewPacket->cb, pPacket->cb, 48);
    pNewPacket->pDataPtr = pPacket->pDataPtr;
        pNewPacket->pktLen = pPacket->pktLen;

    return (PNDIS_PACKET) pNewPacket;

release_pkt_header:
	RTMP_MemPool_Free(pAd, (PUCHAR) pNewPacket->pHeaderMBuf, MemPool_TYPE_Header);
err:
    return NULL;
}


void RTMP_QueryPacketInfo(
	IN  PNDIS_PACKET pPacket,
	OUT PACKET_INFO  *pPacketInfo,
	OUT PUCHAR		 *pSrcBufVA,
	OUT	UINT		 *pSrcBufLen)
{
	pPacketInfo->BufferCount = 1;
	pPacketInfo->pFirstBuffer = (PNDIS_BUFFER) GET_OS_PKT_DATAPTR(pPacket);
	pPacketInfo->PhysicalBufferCount = 1;
	pPacketInfo->TotalPacketLength = (UINT) GET_OS_PKT_LEN(pPacket);

	*pSrcBufVA = GET_OS_PKT_DATAPTR(pPacket);
	*pSrcBufLen = GET_OS_PKT_LEN(pPacket); 
}


PNDIS_PACKET ClonePacket(PNET_DEV ndev, PNDIS_PACKET pkt, UCHAR *buf, ULONG sz)
{
	PECOS_PKT_BUFFER pClonedMblk;

	ASSERT(sz < 1530);	
    
	pClonedMblk = (PECOS_PKT_BUFFER) skb_clone(pkt, MEM_ALLOC_FLAG);
	if (pClonedMblk != NULL)
	{
		/* set the correct dataptr and data len */
		pClonedMblk->pDataPtr = pClonedMblk->pDataPtr + (buf - GET_OS_PKT_DATAPTR(pkt));
		pClonedMblk->pktLen = sz;
	}
	
    return (PNDIS_PACKET) pClonedMblk;
}


PNDIS_PACKET DuplicatePacket(PNET_DEV pNetDev, PNDIS_PACKET pPacket)
{
	PNDIS_PACKET	pRetPacket = NULL;
	USHORT			DataSize;
	UCHAR			*pData;

	DataSize = (USHORT) GET_OS_PKT_LEN(pPacket);
	pData = (PUCHAR) GET_OS_PKT_DATAPTR(pPacket);	

#ifdef RELEASE_EXCLUDE
	/* 
	 * Eddy: don't use sky_clone here because the arp handling of ecos directly 
	 * use the mbuf of received packet to send response packet 
	 */
#endif /* RELEASE_EXCLUDE */	
	pRetPacket = skb_copy(RTPKT_TO_OSPKT(pPacket), MEM_ALLOC_FLAG);
	if (pRetPacket)
		SET_OS_PKT_NETDEV(pRetPacket, pNetDev);
	
	return pRetPacket;
}

	
PNDIS_PACKET duplicate_pkt_vlan(
	IN	PNET_DEV		pNetDev,
	IN	USHORT			VLAN_VID,
	IN	USHORT			VLAN_Priority,
	IN	PUCHAR			pHeader802_3,
	IN	UINT			HdrLen,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN	UCHAR			*TPID)
{
	RTMP_ADAPTER		*pAd = pNetDev->driver_private;
	NDIS_STATUS			Status;
	PECOS_PKT_BUFFER	pPacket = NULL;
	UINT16              VLAN_Size;
	INT skb_len = HdrLen + DataSize + 2;

#ifdef WIFI_VLAN_SUPPORT
	if (VLAN_VID != 0)
		skb_len += LENGTH_802_1Q;
#endif /* WIFI_VLAN_SUPPORT */

    Status = RTMPAllocateNdisPacket(pAd, &pPacket, NULL, 0, NULL, skb_len);
    if (Status != NDIS_STATUS_SUCCESS)
    {
        DBGPRINT(RT_DEBUG_WARN, ("%s:can't allocate NDIS PACKET\n", __FUNCTION__));
        return NULL;
    }

	skb_reserve(pPacket, 2);

	/* copy header (maybe +VLAN tag) */
	VLAN_Size = VLAN_8023_Header_Copy(VLAN_VID, VLAN_Priority, pHeader802_3, HdrLen,
											pPacket->pDataPtr, TPID);
	skb_put(pPacket, HdrLen + VLAN_Size);

	/* copy data body */
	NdisMoveMemory(pPacket->pDataPtr + HdrLen + VLAN_Size, pData, DataSize);
	skb_put(pPacket, DataSize);
	pPacket->net_dev = pNetDev;
    
    return (PNDIS_PACKET) pPacket;
}


#define TKIP_TX_MIC_SIZE		8
PNDIS_PACKET duplicate_pkt_with_TKIP_MIC(
	IN	VOID			*pAd,
	IN	PNDIS_PACKET	pPacket)
{
        PECOS_PKT_BUFFER pBuf = (PECOS_PKT_BUFFER) pPacket;
        pBuf->pDataMBuf->m_len = pBuf->pktLen;
        if (M_TRAILINGSPACE(pBuf->pDataMBuf) < TKIP_TX_MIC_SIZE)
        {
                DBGPRINT(RT_DEBUG_ERROR, ("Extend Tx.MIC for packet failed!, dropping packet!\n"));
                return NULL;
        }
        return pPacket;
}


/*
 * invaild or writeback cache 
 * and convert virtual address to physical address 
 */
ra_dma_addr_t ecos_pci_map_single(void *handle, void *ptr, size_t size, int sd_idx, int direction)
{	
	if (sd_idx == 1)
	{
		TX_BLK *pTxBlk = (TX_BLK *)ptr;
		if (pTxBlk->SrcBufLen > 0) {
			HAL_DCACHE_FLUSH((pTxBlk->pSrcBufData), pTxBlk->SrcBufLen);
			return CYGARC_PHYSICAL_ADDRESS(CYGARC_UNCACHED_ADDRESS(pTxBlk->pSrcBufData));
		} else
			return NULL;
	}
	else
	{
		HAL_DCACHE_FLUSH(ptr, size);    
		return CYGARC_PHYSICAL_ADDRESS(CYGARC_UNCACHED_ADDRESS(ptr));
	}
}


#ifdef PLATFORM_BUTTON_SUPPORT
/* Polling reset button in APSOC */
extern void CGI_reset_default(void);
VOID Restore_button_CheckHandler(
	IN	PRTMP_ADAPTER	pAd)
{
	UINT32 gpio_value, gpio_pol;
	BOOLEAN flg_pressed = 0;

        gpio_value = HAL_REG32(RTMP_PIO_CTL_ADDR + RTMP_PIO2100_DIR_OFFSET);
#ifdef RT5350
        gpio_value &= 0xfdff; /* GPIO 9 */
        HAL_REG32(RTMP_PIO_CTL_ADDR + RTMP_PIO2100_DIR_OFFSET) = gpio_value;
        gpio_value = HAL_REG32(RTMP_PIO_CTL_ADDR + RTMP_PIO2100_DATA_OFFSET);
        flg_pressed = ((~gpio_value) & 0x400)?1:0;
#endif
#ifdef RT6352
	gpio_value &= 0xfffd; /* GPIO 1 */
	HAL_REG32(RTMP_PIO_CTL_ADDR + RTMP_PIO2100_DIR_OFFSET) = gpio_value;
	gpio_value = HAL_REG32(RTMP_PIO_CTL_ADDR + RTMP_PIO2100_DATA_OFFSET);
	flg_pressed = ((~gpio_value) & 0x002)?1:0;
#endif

	if (flg_pressed)
	{
	        ULONG nowtime;
	        NdisGetSystemUpTime(&nowtime);

                if (pAd->CommonCfg.RestoreHdrBtnTimestamp == 0)
                        pAd->CommonCfg.RestoreHdrBtnTimestamp = nowtime; 
                else if (((nowtime - pAd->CommonCfg.RestoreHdrBtnTimestamp) / HZ) > 4)
                {
        		/* execute Reset function */
	        	DBGPRINT(RT_DEBUG_ERROR, ("Restore to factory default settings...\n"));			
			CGI_reset_default();
                        pAd->CommonCfg.RestoreHdrBtnTimestamp = 0;
                }
	}
	else
	{
		/* the button is released */
		pAd->CommonCfg.RestoreHdrBtnTimestamp = 0;		
	}
}
#endif /* PLATFORM_BUTTON_SUPPORT */

