#include "rt_config.h"
#include <malloc.h>
#include <rt_pkt.h>
/*#define WLAN_LOOPBACK	1 */


ULONG RTDebugLevel = RT_DEBUG_WARN;
ULONG RTDebugFunc = 0;


/*
 * must aligne uncached memory in Cache_Line_Size 
 * to avoid memory coherence
 * for example: 
 *		cache line size = 16 bytes
 *      assume 0xA000 ~ 0xAFFF contents is all zero.
 *		cached 0xA000 <-- 0xeeeeeeee; cache load 0xA000 ~0xA00F contents
 *                                    write data to cache.
 *		uncached 0xA008 <-- 0x88888888;
 *      the cpu will write back cached 0xA000 cache line data into memory
 *      the content of uncached 0xA008 will be overwritten to old value.
 */

#ifndef CONFIG_USE_ESRAM
static TXD_STRUC tx_pool[NUM_OF_TX_RING][TX_RING_SIZE] ____cacheline_aligned;
static TXD_STRUC mgmt_pool[MGMT_RING_SIZE] ____cacheline_aligned;
static RXD_STRUC rxd_pool[RX_RING_SIZE] ____cacheline_aligned;
#else
static __esramdata TXD_STRUC tx_pool[NUM_OF_TX_RING][TX_RING_SIZE] ____cacheline_aligned;
static __esramdata TXD_STRUC mgmt_pool[MGMT_RING_SIZE] ____cacheline_aligned;
static __esramdata RXD_STRUC rxd_pool[RX_RING_SIZE] ____cacheline_aligned;
#endif 


static void CfgInitHook(
	IN PRTMP_ADAPTER pAd);



/* for wireless system event message */
char const *pWirelessSysEventText[IW_SYS_EVENT_TYPE_NUM] = {    
	/* system status event */
    "had associated successfully",							/* IW_ASSOC_EVENT_FLAG */
    "had disassociated",									/* IW_DISASSOC_EVENT_FLAG */
    "had deauthenticated",									/* IW_DEAUTH_EVENT_FLAG */
    "had been aged-out and disassociated",					/* IW_AGEOUT_EVENT_FLAG */
    "occurred CounterMeasures attack",						/* IW_COUNTER_MEASURES_EVENT_FLAG */	
    "occurred replay counter different in Key Handshaking",	/* IW_REPLAY_COUNTER_DIFF_EVENT_FLAG */
    "occurred RSNIE different in Key Handshaking",			/* IW_RSNIE_DIFF_EVENT_FLAG */
    "occurred MIC different in Key Handshaking",			/* IW_MIC_DIFF_EVENT_FLAG */
    "occurred ICV error in RX",								/* IW_ICV_ERROR_EVENT_FLAG */
    "occurred MIC error in RX",								/* IW_MIC_ERROR_EVENT_FLAG */
	"Group Key Handshaking timeout",						/* IW_GROUP_HS_TIMEOUT_EVENT_FLAG */ 
	"Pairwise Key Handshaking timeout",						/* IW_PAIRWISE_HS_TIMEOUT_EVENT_FLAG */ 
	"RSN IE sanity check failure",							/* IW_RSNIE_SANITY_FAIL_EVENT_FLAG */ 
	"set key done in WPA/WPAPSK",							/* IW_SET_KEY_DONE_WPA1_EVENT_FLAG */ 
	"set key done in WPA2/WPA2PSK",                         /* IW_SET_KEY_DONE_WPA2_EVENT_FLAG */ 
	"connects with our wireless client",                    /* IW_STA_LINKUP_EVENT_FLAG */ 
	"disconnects with our wireless client",                 /* IW_STA_LINKDOWN_EVENT_FLAG */
	"scan completed",										/* IW_SCAN_COMPLETED_EVENT_FLAG */
	"scan terminate!! Busy!! Enqueue fail!!",				/* IW_SCAN_ENQUEUE_FAIL_EVENT_FLAG */
	"channel switch to ",									/* IW_CHANNEL_CHANGE_EVENT_FLAG */
	"wireless mode is not support",							/* IW_STA_MODE_EVENT_FLAG */
	"blacklisted in MAC filter list"						/* IW_MAC_FILTER_LIST_EVENT_FLAG */
	};						

/* for wireless IDS_spoof_attack event message */
char const *pWirelessSpoofEventText[IW_SPOOF_EVENT_TYPE_NUM] = {   	
    "detected conflict SSID",								/* IW_CONFLICT_SSID_EVENT_FLAG */
    "detected spoofed association response",				/* IW_SPOOF_ASSOC_RESP_EVENT_FLAG */
    "detected spoofed reassociation responses",				/* IW_SPOOF_REASSOC_RESP_EVENT_FLAG */
    "detected spoofed probe response",						/* IW_SPOOF_PROBE_RESP_EVENT_FLAG */
    "detected spoofed beacon",								/* IW_SPOOF_BEACON_EVENT_FLAG */
    "detected spoofed disassociation",						/* IW_SPOOF_DISASSOC_EVENT_FLAG */
    "detected spoofed authentication",						/* IW_SPOOF_AUTH_EVENT_FLAG */
    "detected spoofed deauthentication",					/* IW_SPOOF_DEAUTH_EVENT_FLAG */
    "detected spoofed unknown management frame",			/* IW_SPOOF_UNKNOWN_MGMT_EVENT_FLAG */
	"detected replay attack"								/* IW_REPLAY_ATTACK_EVENT_FLAG */	
	};

/* for wireless IDS_flooding_attack event message */
char const *pWirelessFloodEventText[IW_FLOOD_EVENT_TYPE_NUM] = {   	
	"detected authentication flooding",						/* IW_FLOOD_AUTH_EVENT_FLAG */
    "detected association request flooding",				/* IW_FLOOD_ASSOC_REQ_EVENT_FLAG */
    "detected reassociation request flooding",				/* IW_FLOOD_REASSOC_REQ_EVENT_FLAG */
    "detected probe request flooding",						/* IW_FLOOD_PROBE_REQ_EVENT_FLAG */
    "detected disassociation flooding",						/* IW_FLOOD_DISASSOC_EVENT_FLAG */
    "detected deauthentication flooding",					/* IW_FLOOD_DEAUTH_EVENT_FLAG */
    "detected 802.1x eap-request flooding"					/* IW_FLOOD_EAP_REQ_EVENT_FLAG */	
	};


/* timeout -- ms */
VOID RTMP_SetPeriodicTimer(
	IN	NDIS_MINIPORT_TIMER *pTimer, 
	IN	unsigned long timeout)
{
	timeout = ((timeout*OS_HZ) / 1000);
	pTimer->expires = jiffies + timeout;
	add_timer(pTimer);
}

/* convert NdisMInitializeTimer --> RTMP_OS_Init_Timer */
VOID RTMP_OS_Init_Timer(
	IN	PRTMP_ADAPTER pAd,
	IN	NDIS_MINIPORT_TIMER *pTimer, 
	IN	TIMER_FUNCTION function,
	IN	PVOID data)
{
	init_timer(pTimer);
    pTimer->data = (unsigned long)data;
    pTimer->function = function;		
	printk("%s : %p\n", __FUNCTION__, pTimer); /* Roger test */
}


VOID RTMP_OS_Add_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer,
	IN	unsigned long timeout)
{
	timeout = ((timeout*OS_HZ) / 1000);
	pTimer->expires = jiffies + timeout;
	add_timer(pTimer);
}

VOID RTMP_OS_Mod_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer,
	IN	unsigned long timeout)
{
	timeout = ((timeout*OS_HZ) / 1000);
	mod_timer(pTimer, jiffies + timeout);
}

VOID RTMP_OS_Del_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer,
	OUT	BOOLEAN					*pCancelled)
{
	*pCancelled = del_timer_sync(pTimer);	
}

	
#if 1
/* Unify all delay routine by using udelay */
VOID RtmpusecDelay(ULONG usec)
{
	ULONG	i;

	for (i = 0; i < (usec / 50); i++)
		udelay(50);

	if (usec % 50)
		udelay(usec % 50);
}
#endif

void RTMP_GetCurrentSystemTime(LARGE_INTEGER *time)
{
	time->u.LowPart = jiffies;
}

NDIS_STATUS os_alloc_mem(
	IN	RTMP_ADAPTER *pAd,
	OUT	UCHAR **mem,
	IN	ULONG  size)
{	
	*mem = (PUCHAR) malloc(size);
	if (*mem)
		return (NDIS_STATUS_SUCCESS);
	else
		return (NDIS_STATUS_FAILURE);
}


NDIS_STATUS os_free_mem(
	IN	PRTMP_ADAPTER pAd,
	IN	PVOID mem)
{
	
	ASSERT(mem);
	free(mem);
	return (NDIS_STATUS_SUCCESS);
}


NDIS_STATUS AdapterBlockAllocateMemory(
	IN PVOID	handle,
	OUT	PVOID	*ppAd,
	IN UINT32	SizeOfpAd,
	IN UINT32	SizeOfDrvOps,
	IN UINT32	SizeOfNetOps)
{
	POS_COOKIE pObj = (POS_COOKIE) handle;
	*ppAd = malloc(sizeof(RTMP_ADAPTER));

	if (*ppAd) {
		NdisZeroMemory(*ppAd, sizeof(RTMP_ADAPTER));
		((PRTMP_ADAPTER)*ppAd)->OS_Cookie = handle;
		return (NDIS_STATUS_SUCCESS);
	} else {
		return (NDIS_STATUS_FAILURE);
	}

}


void RTMP_AllocateTxDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	void *p;

	p = tx_pool[Index];
#ifdef CONFIG_USE_ESRAM
	Cached = FALSE;
#endif

	if (Cached)  {
		*VirtualAddress = p;
	} else {
		*VirtualAddress = (PVOID) KSEG1ADDR((UINT32)p);
	}
	*PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) CPHYSADDR((UINT32)p);
}


void RTMP_AllocateMgmtDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	void *p;

	p = mgmt_pool;
#ifdef CONFIG_USE_ESRAM
	Cached = FALSE;
#endif


	if (Cached)  {
		*VirtualAddress = p;
	} else {
		*VirtualAddress = (PVOID) KSEG1ADDR((UINT32)p);
	}
	*PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) CPHYSADDR((UINT32)p);
}



void RTMP_AllocateRxDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	void *p;

	p = &rxd_pool[0];
#ifdef CONFIG_USE_ESRAM
	Cached = FALSE;
#endif

	if (Cached)  {
		*VirtualAddress = p;
	} else {
		*VirtualAddress = (PVOID) KSEG1ADDR((UINT32)p);
	}
	*PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) CPHYSADDR((UINT32)p);
}


void RTMP_AllocateFirstTxBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	void *p;

	p = malloc(sizeof(char)*Length);
	if (Cached)  {
		*VirtualAddress = p;
	} else {
		*VirtualAddress = (PVOID) KSEG1ADDR((UINT32)p);
	}
	*PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) CPHYSADDR((UINT32)p);
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
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	struct net_pkt_blk *pkt;
	int size_idx;

	size_idx = (Length<=2048) ? (NET_BUF_2048) : (NET_BUF_4096);

	pkt = net_pkt_alloc(size_idx);

	if (pkt == NULL) {
		printk("can't allocate rx %ld size packet\n",Length);
	}

	if (pkt) {
		*VirtualAddress = (PVOID) pkt->data;	
		*PhysicalAddress = PCI_MAP_SINGLE(pAd, *VirtualAddress, Length, RTMP_PCI_DMA_FROMDEVICE);
	} else {
		*VirtualAddress = (PVOID) NULL;
		*PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) NULL;
	}	

	return (PNDIS_PACKET) pkt;
}


PNDIS_PACKET RtmpOSNetPktAlloc(
	IN RTMP_ADAPTER *pAd, 
	IN int size)
{
	struct net_pkt_blk *pPktBlk;

	ASSERT((size < NET_BUF_2048));
	
	size = (size > NET_BUF_1024) ? NET_BUF_2048 : NET_BUF_1024;
	pPktBlk = net_pkt_alloc(size);

	return ((PNDIS_PACKET)pPktBlk);
}

PNDIS_PACKET RTMP_AllocateFragPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length)
{
	struct net_pkt_blk *pkt;

	pkt = net_pkt_alloc(Length);

	if (pkt == NULL) {
		DBGPRINT(RT_DEBUG_ERROR, ("can't allocate frag rx %ld size packet\n",Length));
	}

	return (PNDIS_PACKET) pkt;	
}

VOID	RTMPFreeAdapter(
	IN	PRTMP_ADAPTER	pAd)
{
	/* no need to do this, since re-run UCOS need to reload firmware.... */
}

#if 0
VOID build_tx_packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR	pFrame,
	IN	ULONG	FrameLen)
{

	struct net_pkt_blk	*pTxPkt;

	ASSERT(pPacket);
	pTxPkt = RTPKT_TO_OSPKT(pPacket);

	NdisMoveMemory(net_pkt_put(pTxPkt, FrameLen), pFrame, FrameLen);	
}

PNDIS_PACKET RTMP_AllocateTxPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress)
{
	struct net_pkt_blk *pkt;
	int size_idx;

	size_idx = (Length<=2048) ? (NET_BUF_2048) : (NET_BUF_4096);

	pkt = net_pkt_alloc(size_idx);

	if (pkt == NULL) {
		printk("can't allocate tx %ld size packet\n",Length);
	}

	if (pkt) {
		*VirtualAddress = (PVOID) pkt->data;	
	} else {
		*VirtualAddress = (PVOID) NULL;
	}	

	return (PNDIS_PACKET) pkt;
}

/* 
 * This function delays execution of the caller for a specified interval, 
 * in microseconds.
 */
void RTMP_MSleep(UINT32 delay)
{
   udelay(delay);	
}

BOOLEAN OS_Need_Clone_Packet(void)
{
	return (FALSE);	
}

/*
	========================================================================

	Routine Description:
		clone an input NDIS PACKET to another one. The new internally created NDIS PACKET
		must have only one NDIS BUFFER
		return - byte copied. 0 means can't create NDIS PACKET
		NOTE: internally created NDIS_PACKET should be destroyed by RTMPFreeNdisPacket
		
	Arguments:
		pAd 	Pointer to our adapter
		pInsAMSDUHdr	EWC A-MSDU format has extra 14-bytes header. if TRUE, insert this 14-byte hdr in front of MSDU.
		*pSrcTotalLen			return total packet length. This lenght is calculated with 802.3 format packet.
		
	Return Value:
		NDIS_STATUS_SUCCESS 	
		NDIS_STATUS_FAILURE 	
		
	Note:
	
	========================================================================
*/
NDIS_STATUS RTMPCloneNdisPacket(
	IN	PRTMP_ADAPTER	pAd,
	IN	BOOLEAN			pInsAMSDUHdr,
	IN	PNDIS_PACKET	pInPacket,
	OUT PNDIS_PACKET   *ppOutPacket)
{

	struct net_pkt_blk *pkt;

	ASSERT(pInPacket);
	ASSERT(ppOutPacket);

	/* 1. Allocate a packet */
	pkt = net_pkt_alloc(NET_BUF_2048);
	
	if (pkt == NULL) {
		return NDIS_STATUS_FAILURE;
	}

 	net_pkt_put(pkt, GET_OS_PKT_LEN(pInPacket));
	NdisMoveMemory(pkt->data, GET_OS_PKT_DATAPTR(pInPacket), GET_OS_PKT_LEN(pInPacket));  
	*ppOutPacket = OSPKT_TO_RTPKT(pkt);

	printk("###Clone###\n");

	return NDIS_STATUS_SUCCESS;

#if 0
	PACKET_INFO 	PacketInfo;
	ULONG			SrcBufLen, TotalLen;
	PNDIS_BUFFER	pBuffer;
	NDIS_STATUS 	Status;
	PUCHAR			pSrcBufVA, pDest;
	PNDIS_PACKET	pPacket;
	PRTMP_TXBUF 	pTxBuf;
	ULONG			TotalPacketLength;

	/* */
	/* 1. gather information of the input NDIS PACKET */
	/* */
	NdisQueryPacket(
		pInPacket,							/* Ndis packet */
		&PacketInfo.PhysicalBufferCount,	/* Physical buffer count */
		&PacketInfo.BufferCount,			/* Number of buffer descriptor */
		&PacketInfo.pFirstBuffer,			/* Pointer to first buffer descripotr */
		&PacketInfo.TotalPacketLength); 	/* Ndis packet length */

	NDIS_QUERY_BUFFER(PacketInfo.pFirstBuffer, &pSrcBufVA, &SrcBufLen);

	if (SrcBufLen < 14)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s() --> Ndis Packet buffer error !!!\n", __FUNCTION__));
		return (NDIS_STATUS_FAILURE);
	}

	DBGPRINT(RT_DEBUG_INFO,("---> RTMPCloneNdisPacket: in scatter # =%d, packet len = %d\n",
		PacketInfo.PhysicalBufferCount, PacketInfo.TotalPacketLength));
	
	/* */
	/* 2. Allocate the output NDIS PACKET */
	/* */

	/* 2.1. get NDIS PACKET */
	NdisAllocatePacket(&Status, &pPacket, pAd->FreeNdisPacketPoolHandle);
	if (Status != NDIS_STATUS_SUCCESS)
		return NDIS_STATUS_FAILURE;

	/* 2.2. get one pre-allocated shared memory */
	NdisAcquireSpinLock(&pAd->LocalTxBufQueueLock);
	pTxBuf = (PRTMP_TXBUF)RemoveHeadQueue(&pAd->LocalTxBufQueue)
	NdisReleaseSpinLock(&pAd->LocalTxBufQueueLock);
	if (pTxBuf == NULL)
	{
		NdisFreePacket(pPacket);
		return NDIS_STATUS_FAILURE;
	}

	TotalPacketLength = PacketInfo.TotalPacketLength;
	if (pInsAMSDUHdr == TRUE)
		TotalPacketLength += 14;

	/* 2.3. allocate NDIS BUFFER and link to the pre-shared shared memory */
	NdisAllocateBuffer(&Status, &pBuffer, pAd->FreeNdisBufferPoolHandle, pTxBuf->AllocVa, TotalPacketLength);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		InsertTailQueue(&pAd->LocalTxBufQueue, pTxBuf);
		NdisFreePacket(pPacket);
		return NDIS_STATUS_FAILURE;
	}
	NdisChainBufferAtFront(pPacket, pBuffer);

	/* */
	/* 4. clone packet content */
	/* */
	pDest = (PUCHAR)pTxBuf->AllocVa;
	TotalLen = 0;
	
	/* 4-1. if in EWC AMSDU format, insert 14 bytes hdr. */
	if (pInsAMSDUHdr == TRUE)
	{
		NdisMoveMemory(pDest, pSrcBufVA, 12);
		*(pDest +12) = PacketInfo.TotalPacketLength/256;
		*(pDest +13) = PacketInfo.TotalPacketLength%256;
		pDest += 14;
	}

	while (PacketInfo.pFirstBuffer)
	{
		NDIS_QUERY_BUFFER(PacketInfo.pFirstBuffer, &pSrcBufVA, &SrcBufLen);
		NdisMoveMemory(pDest, pSrcBufVA, SrcBufLen);
		pDest += SrcBufLen;
		TotalLen += SrcBufLen;
		NdisGetNextBuffer(PacketInfo.pFirstBuffer, &PacketInfo.pFirstBuffer);
	}
	/*RT2860B. check here. then erase. */
	if (TotalLen != (ULONG)PacketInfo.TotalPacketLength)
		DBGPRINT(RT_DEBUG_DMA,("RTMPCloneNdisPacket Chk1 (TotalLen=%d) TotalPacketLength = %d\n", TotalLen, PacketInfo.TotalPacketLength));

	/*RT2860B. check here. then erase. */
	NdisQueryPacket(
		pPacket,						  /* Ndis packet */
		&PacketInfo.PhysicalBufferCount,	/* Physical buffer count */
		&PacketInfo.BufferCount,			/* Number of buffer descriptor */
		&PacketInfo.pFirstBuffer,			/* Pointer to first buffer descripotr */
		&PacketInfo.TotalPacketLength); 	/* Ndis packet length */

	if ((TotalLen+14) != (ULONG)PacketInfo.TotalPacketLength)
		DBGPRINT(RT_DEBUG_DMA,("RTMPCloneNdisPacket Chk2 (TotalLen=%d) TotalPacketLength = %d\n", TotalLen, PacketInfo.TotalPacketLength));

	*ppOutPacket = pPacket;
#endif
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
	PNDIS_PACKET	pPacket;
	ASSERT(pData);
	ASSERT(DataLen);

	/* 1. Allocate a packet */
	pPacket = (PNDIS_PACKET *) net_pkt_alloc(NET_BUF_2048);
	if (pPacket == NULL) {
		*ppPacket = NULL;
#ifdef DEBUG
		printk("RTMPAllocateNdisPacket Fail\n\n");
#endif
		return NDIS_STATUS_FAILURE;
	}

	/* 2. clone the frame content */
	if ((HeaderLen > 0) && (pHeader != NULL))
		memmove(GET_OS_PKT_DATAPTR(pPacket), pHeader, HeaderLen);
	if ((DataLen > 0) && (pData != NULL))
		memmove(GET_OS_PKT_DATAPTR(pPacket) + HeaderLen, pData, DataLen);

	/* 3. update length of packet */
 	net_pkt_put(GET_OS_PKT_TYPE(pPacket), HeaderLen+DataLen);

/*	printk("%s : pPacket = %p, len = %d\n", __FUNCTION__, pPacket, GET_OS_PKT_LEN(pPacket)); */
	*ppPacket = pPacket;
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
	net_pkt_free(pPacket);
}


#if 1
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
#endif

#if 0
void RTMP_QueryNextPacketInfo(
	IN  PNDIS_PACKET *ppPacket,
	OUT PACKET_INFO  *pPacketInfo,
	OUT PUCHAR		 *pSrcBufVA,
	OUT	UINT		 *pSrcBufLen)
{
	PNDIS_PACKET pPacket = NULL;

	if (*ppPacket)
		pPacket = GET_OS_PKT_NEXT(*ppPacket);

	if (pPacket) {
		pPacketInfo->BufferCount = 1;
		pPacketInfo->pFirstBuffer = GET_OS_PKT_DATAPTR(pPacket);
		pPacketInfo->PhysicalBufferCount = 1;
		pPacketInfo->TotalPacketLength = GET_OS_PKT_LEN(pPacket);

		*pSrcBufVA = GET_OS_PKT_DATAPTR(pPacket);
		*pSrcBufLen = GET_OS_PKT_LEN(pPacket); 	
		*ppPacket = GET_OS_PKT_NEXT(pPacket);		
	} else {
		pPacketInfo->BufferCount = 0;
		pPacketInfo->pFirstBuffer = NULL;
		pPacketInfo->PhysicalBufferCount = 0;
		pPacketInfo->TotalPacketLength = 0;

		*pSrcBufVA = NULL;
		*pSrcBufLen = 0; 	
		*ppPacket = NULL;
	}
}
#endif /* Unused */


PNDIS_PACKET ClonePacket(PNET_DEV ndev, PNDIS_PACKET pkt, UCHAR *buf, ULONG sz)
{
	struct net_pkt_blk *pRxPkt;
	struct net_pkt_blk *pClonedPkt;

	ASSERT(pkt);
	ASSERT(sz < 1530);
	pRxPkt = RTPKT_TO_OSPKT(pkt);

	/* clone the packet */
	pClonedPkt = net_pkt_clone(pRxPkt);
	if (pClonedPkt)
	{
		/* set the correct dataptr and data len */
		pClonedPkt->dev = pRxPkt->dev;
		pClonedPkt->data = buf;
		pClonedPkt->len = sz;
		pClonedPkt->tail = pClonedPkt->data + pClonedPkt->len;
	}

	return pClonedPkt;
}


PNDIS_PACKET DuplicatePacket(PNET_DEV pNetDev, PNDIS_PACKET pPacket)
{
	struct net_pkt_blk *skb;
	PNDIS_PACKET	pRetPacket = NULL;
	USHORT			DataSize;
	UCHAR			*pData;

	DataSize = (USHORT) GET_OS_PKT_LEN(pPacket);
	pData = (PUCHAR) GET_OS_PKT_DATAPTR(pPacket);	


	skb = net_pkt_clone(RTPKT_TO_OSPKT(pPacket));
	
	if (skb)
	{
		skb->dev = pNetDev;
		pRetPacket = OSPKT_TO_RTPKT(skb);
	}

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
	struct net_pkt_blk	*skb;
	PNDIS_PACKET	pPacket = NULL;
	UINT16			VLAN_Size;

	skb = net_pkt_alloc(NET_BUF_2048);
	if (skb != NULL)
	{
		/* copy header (maybe +VLAN tag) */
		VLAN_Size = VLAN_8023_Header_Copy(VLAN_VID, VLAN_Priority,
											pHeader802_3, HdrLen,
											skb->tail, TPID);
		net_pkt_put(GET_OS_PKT_TYPE(skb), HdrLen + VLAN_Size);
		NdisMoveMemory(skb->tail, pData, DataSize);
		net_pkt_put(GET_OS_PKT_TYPE(skb), DataSize);

		pPacket = OSPKT_TO_RTPKT(skb);
	}

	return pPacket;
}


void clone_pkt_and_pass_up(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket,	
	IN	PUCHAR			pData,
	IN	ULONG			DataSize)
{
	struct net_pkt_blk	*pRxPkt;
	struct net_pkt_blk	*pClonedPkt;

	ASSERT(pPacket);
	pRxPkt = RTPKT_TO_OSPKT(pPacket);

	/* clone the packet */
	pClonedPkt = net_pkt_clone(pRxPkt);

	if (pClonedPkt)
	{
		pClonedPkt->dev = pRxPkt->dev;	
		pClonedPkt->data = pData;
		pClonedPkt->len = DataSize;
		pClonedPkt->tail = pClonedPkt->data + pClonedPkt->len;		
		ASSERT(DataSize < MAX_RX_PKT_LEN);
		announce_802_3_packet(pAd, pClonedPkt);
	}
	else
	{
		printk("clone fail\n");
	}
}


void  build_os_802_3_packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR pData,
	IN	ULONG DataSize,
	IN  UCHAR wdev_idx)
{
	struct net_pkt_blk	*pRxPkt;

	ASSERT(pPacket);
	pRxPkt = RTPKT_TO_OSPKT(pPacket);

	/*pRxPkt->dev = net_dev; */
	pRxPkt->dev = get_netdev_from_bssid(pAd, wdev_idx);
	pRxPkt->data = pData;
	pRxPkt->len = DataSize;
	pRxPkt->tail = pRxPkt->data + pRxPkt->len;
}

#if 0
void convert_802_11_to_802_3_packet(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR			p8023hdr,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN  UCHAR			wdev_idx)
{
	struct net_pkt_blk	*pRxPkt;

	ASSERT(pPacket);
	pRxPkt = RTPKT_TO_OSPKT(pPacket);

    /*pRxPkt->dev = net_dev; */
    pRxPkt->dev = get_netdev_from_bssid(pAd, wdev_idx);
    pRxPkt->data = pData;
   	pRxPkt->len = DataSize;
    pRxPkt->tail = pRxPkt->data + pRxPkt->len;

	/* */
	/* copy 802.3 header, if necessary */
	/* */
	if (p8023hdr)
	{
		
#ifdef CONFIG_AP_SUPPORT
		/* maybe insert VLAN tag to the received packet */
		UCHAR VLAN_Size = 0;
		UCHAR *data_p;

		/* VLAN related */
		if ((wdev->func_idx < pAd->ApCfg.BssidNum) &&
			(pAd->ApCfg.MBSSID[wdev->func_idx].VLAN_VID != 0))
		{
			VLAN_Size = LENGTH_802_1Q;
		}

		data_p = net_pkt_push(pRxPkt, LENGTH_802_3+VLAN_Size);

		VLAN_8023_Header_Copy(pAd, p8023hdr, LENGTH_802_3,
								data_p, TPID);
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		NdisMoveMemory(net_pkt_push(pRxPkt, LENGTH_802_3), p8023hdr, LENGTH_802_3);
#endif /* CONFIG_STA_SUPPORT */
	}
}
#endif /* Unused */

VOID RtmpOsPktInit(
	IN	PNDIS_PACKET		pRxPacket,
	IN	PNET_DEV			pNetDev,
	IN	UCHAR				*pData,
	IN	USHORT				DataSize)
{
	struct net_pkt_blk	*pOSPkt;

	ASSERT(pRxPacket);
	pOSPkt = RTPKT_TO_OSPKT(pRxPacket);

	pOSPkt->dev = pNetDev; 
	pOSPkt->data = pData;
	pOSPkt->len = DataSize;
	pOSPkt->tail = pOSPkt->data + pOSPkt->len;
}


UCHAR VLAN_8023_Header_Copy(
	IN	USHORT VLAN_VID,
	IN	USHORT VLAN_Priority,
	IN	UCHAR *pHeader802_3,
	IN	UINT HdrLen,
	OUT UCHAR *pData,
	IN	UCHAR *TPID)
{
	UINT16 TCI;
	UCHAR VLAN_Size = 0;


	if (VLAN_VID != 0)
	{
		/* need to insert VLAN tag */
		VLAN_Size = LENGTH_802_1Q;

		/* make up TCI field */
		TCI = (VLAN_VID & 0x0fff) | ((VLAN_Priority & 0x7)<<13);

#ifndef RT_BIG_ENDIAN
		TCI = SWAP16(TCI);
#endif /* RT_BIG_ENDIAN */

		/* copy dst + src MAC (12B) */
		memcpy(pData, pHeader802_3, LENGTH_802_3_NO_TYPE);

		/* copy VLAN tag (4B) */
		/* do NOT use memcpy to speed up */
		*(UINT16 *)(pData+LENGTH_802_3_NO_TYPE) = *(UINT16 *)TPID;
		*(UINT16 *)(pData+LENGTH_802_3_NO_TYPE+2) = TCI;

		/* copy type/len (2B) */
		*(UINT16 *)(pData+LENGTH_802_3_NO_TYPE+LENGTH_802_1Q) = \
				*(UINT16 *)&pHeader802_3[LENGTH_802_3-LENGTH_802_3_TYPE];

		/* copy tail if exist */
		if (HdrLen > LENGTH_802_3)
		{
			memcpy(pData+LENGTH_802_3+LENGTH_802_1Q,
					pHeader802_3+LENGTH_802_3,
					HdrLen - LENGTH_802_3);
		}
	}
	else
	{
		/* no VLAN tag is needed to insert */
		memcpy(pData, pHeader802_3, HdrLen);
	}

	return VLAN_Size;
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
	struct net_pkt_blk	*pOSPkt;

	ASSERT(pRxBlk->pRxPacket);
	ASSERT(pHeader802_3);

	pOSPkt = RTPKT_TO_OSPKT(pRxBlk->pRxPacket);

	pOSPkt->dev = pNetDev;
	pOSPkt->data = pRxBlk->pData;
	pOSPkt->len = pRxBlk->DataSize;
	pOSPkt->tail = pOSPkt->data + pOSPkt->len;

	/* */
	/* copy 802.3 header */
	/* */
	/* */
#ifdef CONFIG_AP_SUPPORT
	{
		/* maybe insert VLAN tag to the received packet */
		UCHAR VLAN_Size = 0;
		UCHAR *data_p;

		/* VLAN related */
		if (VLAN_VID != 0)
			VLAN_Size = LENGTH_802_1Q;

		data_p = net_pkt_push(pOSPkt, LENGTH_802_3+VLAN_Size);

		VLAN_8023_Header_Copy(VLAN_VID, VLAN_Priority,
								pHeader802_3, LENGTH_802_3,
								data_p, TPID);
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	NdisMoveMemory(net_pkt_push(pOSPkt, LENGTH_802_3), pHeader802_3, LENGTH_802_3);
#endif /* CONFIG_STA_SUPPORT */
	}

void announce_802_3_packet(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket)
{
	struct net_pkt_blk	*pRxPkt;

	ASSERT(pPacket);
	pRxPkt = RTPKT_TO_OSPKT(pPacket);
#ifdef APCLI_SUPPORT
	if (MATPktRxNeedConvert(pAd, pRxPkt->dev))
		MATEngineRxHandle(pAd, pPacket, 0);
#endif /* APCLI_SUPPORT */

	if (pRxPkt->len > MAX_RX_PKT_LEN)
	{
		hex_dump("Error Pkt", pRxPkt->data, 32);
		printk("Error packet len = %d\n", pRxPkt->len);
		net_pkt_free(pRxPkt);
		return;
	}
#if 0
	/*hex_dump("Rx", pRxPkt->data, 32); */
	net_pkt_debug(2); /* Roger debug */
#endif	
	bridge_rx_handle(RTPKT_TO_OSPKT(pPacket));
	
	
}

void announce_802_3_packet_with_copy(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			p8023hdr,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN	UCHAR			wdev_idx)
{
	struct net_pkt_blk	*skb;
	PNDIS_PACKET	pPacket = NULL;

	skb = net_pkt_alloc(NET_BUF_2048);
	if (skb != NULL)
	{
		skb->dev = get_netdev_from_bssid(pAd, wdev_idx);
		
		NdisMoveMemory(skb->tail, p8023hdr, LENGTH_802_3);
		net_pkt_put(GET_OS_PKT_TYPE(skb), LENGTH_802_3);
		NdisMoveMemory(skb->tail, pData, DataSize);
		net_pkt_put(GET_OS_PKT_TYPE(skb), DataSize);
		pPacket = OSPKT_TO_RTPKT(skb);
#if 0		
		net_pkt_debug(2); /* Roger debug */
#endif 		
		bridge_rx_handle(pPacket);

	}
}


/*
 * make an area consistent.
 */
void consistent_sync(void *vaddr, size_t size, int direction)
{
	unsigned long start = (unsigned long)vaddr;
	
	switch (direction) {
	case PCI_DMA_NONE:
		BUG();
	case PCI_DMA_FROMDEVICE:	/* invalidate only */
#ifndef CONFIG_PACKET_DMA_OPTIMIZED
		dma_cache_inv(start, size);
#else
		dma_cache_inv(start, 256 /*size*/);
#endif
		break;
	case PCI_DMA_TODEVICE:		/* writeback only */
		dma_cache_wback(start, size);
		break;
	case PCI_DMA_BIDIRECTIONAL:	/* writeback and invalidate */
		dma_cache_wback_inv(start, size);
		break;
	}
}



/*
 * invaild or writeback cache 
 * and convert virtual address to physical address 
 */
ra_dma_addr_t rt_pci_map_single(void *handle, void *ptr, size_t size, int direction)
{
	consistent_sync(ptr, size, direction);
	return (ra_dma_addr_t) CPHYSADDR(ptr);
}


void rt_pci_unmap_single(void *handle, ra_dma_addr_t dma_addr, size_t size, int direction)
{
#if 0
	if (direction == PCI_DMA_FROMDEVICE) {
		consistent_sync(dma_addr | 0x80000000, size, direction);
	}
#endif
	/* do nothing in rt2880 */
}



PRTMP_SCATTER_GATHER_LIST
rt_get_sg_list_from_packet(PNDIS_PACKET pPacket, RTMP_SCATTER_GATHER_LIST *sg)
{
	sg->NumberOfElements = 1;
	sg->Elements[0].Address =  GET_OS_PKT_DATAPTR(pPacket);	
	sg->Elements[0].Length = GET_OS_PKT_LEN(pPacket);	
	return (sg);
}


void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen)
{
	unsigned char *pt;
	int x;

	if (RTDebugLevel < RT_DEBUG_TRACE)
		return;
	
	pt = pSrcBufVA;
	printk("%s: %p, len = %d\n", str, pSrcBufVA, SrcBufLen);
	for (x=0; x<SrcBufLen; x++) {
		if (x % 16 == 0) 
			printk("0x%04x : ", x);
		printk("%02x ", ((unsigned char)pt[x]));
		if (x%16 == 15) printk("\n");
	}
	printk("\n");
}

#if 0	/* Dennis Lee + , I think use RT2860 EEPROM READ/WRITE is OK, maybe */

/* IRQL = PASSIVE_LEVEL */
USHORT RTMP_EEPROM_READ16(
    IN	PRTMP_ADAPTER	pAd,
    IN  USHORT Offset)
{
	USHORT value;
#ifndef SPI_DRV
	i2c_eeprom_read(Offset, &value, 2);
#else
	eeprom_read(Offset, 2, &value);
#endif
	return value;
}	/*ReadEEprom */

VOID RTMP_EEPROM_WRITE16(
    IN	PRTMP_ADAPTER	pAd,
    IN  USHORT Offset,
    IN  USHORT Data)
{
#ifndef SPI_DRV
	i2c_eeprom_write(Offset, &Data, 2);
#else
	eeprom_write(Offset, 2, &Data);
#endif
}


VOID rt_ee_read_all(PRTMP_ADAPTER pAd, USHORT *Data)
{	
#ifndef SPI_DRV
	i2c_eeprom_read(0, Data, 0x200);
#else
	eeprom_read(0, 0x200, Data);
#endif
}

VOID rt_ee_write_all(PRTMP_ADAPTER pAd, USHORT *Data)
{
#ifndef SPI_DRV
	i2c_eeprom_write(0, Data, 0x200);
#else
	eeprom_write(0, 0x200, Data);
#endif
}

#endif /* Dennis Lee - */

#ifdef SYSTEM_LOG_SUPPORT
/*
	========================================================================
	
	Routine Description:
		Send log message through wireless event

		Support standard iw_event with IWEVCUSTOM. It is used below.

		iwreq_data.data.flags is used to store event_flag that is defined by user. 
		iwreq_data.data.length is the length of the event log.

		The format of the event log is composed of the entry's MAC address and
		the desired log message (refer to pWirelessEventText).

			ex: 11:22:33:44:55:66 has associated successfully

		p.s. The requirement of Wireless Extension is v15 or newer. 

	========================================================================
*/
VOID RTMPSendWirelessEvent(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Event_flag,
	IN	PUCHAR 			pAddr,
	IN  UCHAR			wdev_idx,
	IN	CHAR			Rssi)
{
#if WIRELESS_EXT >= 15

	union 	iwreq_data      wrqu;
	PUCHAR 	pBuf = NULL, pBufPtr = NULL;
	USHORT	event, type, BufLen;	
	UCHAR	event_table_len = 0;	

	if (pAd->CommonCfg.bWirelessEvent == FALSE)
		return;

	type = Event_flag & 0xFF00;	
	event = Event_flag & 0x00FF;

	switch (type)
	{
		case IW_SYS_EVENT_FLAG_START:
			event_table_len = IW_SYS_EVENT_TYPE_NUM;
			break;

		case IW_SPOOF_EVENT_FLAG_START:
			event_table_len = IW_SPOOF_EVENT_TYPE_NUM;
			break;

		case IW_FLOOD_EVENT_FLAG_START:
			event_table_len = IW_FLOOD_EVENT_TYPE_NUM;
			break;
	}
	
	if (event_table_len == 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : The type(%0x02x) is not valid.\n", __FUNCTION__, type));			       		       		
		return;
}

	if (event >= event_table_len)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : The event(%0x02x) is not valid.\n", __FUNCTION__, event));			       		       		
		return;
	}	
 
	/*Allocate memory and copy the msg. */
	if((pBuf = kmalloc(IW_CUSTOM_MAX_LEN, GFP_ATOMIC)) != NULL)
	{
		/*Prepare the payload */
		memset(pBuf, 0, IW_CUSTOM_MAX_LEN);

		pBufPtr = pBuf;		

		if (pAddr)
			pBufPtr += sprintf(pBufPtr, "(RT2860) STA(%02x:%02x:%02x:%02x:%02x:%02x) ", PRINT_MAC(pAddr));				
		else if (BssIdx < MAX_MBSSID_NUM(pAd))
			pBufPtr += sprintf(pBufPtr, "(RT2860) BSS(ra%d) ", BssIdx);
		else
			pBufPtr += sprintf(pBufPtr, "(RT2860) ");

		if (type == IW_SYS_EVENT_FLAG_START)
		{
			pBufPtr += sprintf(pBufPtr, "%s", pWirelessSysEventText[event]);
			
			if (Event_flag == IW_CHANNEL_CHANGE_EVENT_FLAG)
			{
				pBufPtr += sprintf(pBufPtr, "%3d", Rssi);
			}			
		}
		else if (type == IW_SPOOF_EVENT_FLAG_START)
			pBufPtr += sprintf(pBufPtr, "%s (RSSI=%d)", pWirelessSpoofEventText[event], Rssi);
		else if (type == IW_FLOOD_EVENT_FLAG_START)
			pBufPtr += sprintf(pBufPtr, "%s", pWirelessFloodEventText[event]);
		else
			pBufPtr += sprintf(pBufPtr, "%s", "unknown event");
		
		pBufPtr[pBufPtr - pBuf] = '\0';
		BufLen = pBufPtr - pBuf;
		
		memset(&wrqu, 0, sizeof(wrqu));	
	    wrqu.data.flags = Event_flag;
		wrqu.data.length = BufLen;	
		
		/*send wireless event */
	    wireless_send_event(pAd->net_dev, IWEVCUSTOM, &wrqu, pBuf);
	
		/*DBGPRINT(RT_DEBUG_TRACE, ("%s : %s\n", __FUNCTION__, pBuf)); */
	
		kfree(pBuf);
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Can't allocate memory for wireless event.\n", __FUNCTION__));			       		       				
#else
	DBGPRINT(RT_DEBUG_ERROR, ("%s : The Wireless Extension MUST be v15 or newer.\n", __FUNCTION__));	
#endif  /* WIRELESS_EXT >= 15 */  
}
#endif /* SYSTEM_LOG_SUPPORT */

/*
 ========================================================================
 Routine Description:
    Build a L2 frame to upper layer.

 Arguments:

 Return Value:
    TRUE - send successfully
    FAIL - send fail

 Note:
 ========================================================================
*/
BOOLEAN RTMPL2FrameTxAction(
	IN  PRTMP_ADAPTER		pAd,
	IN	PNET_DEV			pNetDev,
	IN	RTMP_CB_8023_PACKET_ANNOUNCE _announce_802_3_packet,
	IN	UCHAR				apidx,
	IN	PUCHAR				pData,
	IN	UINT32				data_len)
{
	struct net_pkt_blk *skb = net_pkt_alloc(NET_BUF_1024);

	if (!skb)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Error! Can't allocate a skb.\n"));
		return FALSE;
	}

	GET_OS_PKT_NETDEV(skb) = get_netdev_from_bssid(pAd, wdev_idx);

	NdisMoveMemory(GET_OS_PKT_DATAPTR(skb), pData, data_len);

 	net_pkt_put(GET_OS_PKT_TYPE(skb), data_len);

	DBGPRINT(RT_DEBUG_TRACE, ("Notify 8021.x daemon to trigger EAP-SM for this sta(%02x:%02x:%02x:%02x:%02x:%02x)\n", PRINT_MAC(pEntry->Addr)));

	announce_802_3_packet(pAd, skb);

	return TRUE;
}



/* ATE part */
INT Set_ATE_Load_E2P_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PCHAR			arg)
{
	USHORT WriteEEPROM[(EEPROM_SIZE/2)];
	struct iwreq *wrq = (struct iwreq *)arg;
	
	DBGPRINT(RT_DEBUG_TRACE, ("===> %s (wrq->u.data.length = %d)\n\n", __FUNCTION__, wrq->u.data.length));

	if (wrq->u.data.length != EEPROM_SIZE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: error length (=%d) from host\n",
			   __FUNCTION__, wrq->u.data.length));
		return FALSE;
	}
	else/* (wrq->u.data.length == EEPROM_SIZE) */
	{
		/* zero the e2p buffer */
		NdisZeroMemory((PUCHAR)WriteEEPROM, EEPROM_SIZE);

		/* fill the local buffer */
		NdisMoveMemory((PUCHAR)WriteEEPROM, wrq->u.data.pointer, wrq->u.data.length);

		do
		{
			/* write the content of .bin file to EEPROM */
			rt_ee_write_all(pAd, WriteEEPROM);
				
		} while(FALSE);
	}

    DBGPRINT(RT_DEBUG_TRACE, ("<=== %s\n", __FUNCTION__));

    return TRUE;
	
}


int ate_copy_to_user(
	IN PUCHAR payload,
	IN PUCHAR msg, 
	IN INT len)
{
	memmove(payload, msg, len);
	return 0;
}


