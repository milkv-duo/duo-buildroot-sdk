/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Abstract:

	Provided APIs relative to network buffer management.

	History:
		1. 2009/09/24	Arvin Tai
			(1) Added following APIs.
				1. RTMP_NetPktPoolInit()
				2. RTMP_NetPktPoolDestory()
				3. RTMP_NetPktAlloc()
				4. RTMP_NetPktfree()
*/
#include "rt_config.h"

#define RTMP_NET_PKT_POOL_SIZE 1024
#define RTMP_NET_BUF_POOL_SIZE 1024

#define OVERHEAD      (((sizeof(ECOS_NET_PKT_POOL_ENTRY)  + 31) / 32) * sizeof(cyg_uint32))
#define ACTUAL_SIZE   ((sizeof(ECOS_NET_PKT_POOL_ENTRY) * RTMP_NET_BUF_POOL_SIZE) + OVERHEAD)

static cyg_uint32      pool_memory[(ACTUAL_SIZE + 3) / 4];
static cyg_handle_t    pool_handle;
static cyg_mempool_fix pool_data;

VOID RTMP_NetPktPoolInit(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	UINT mem_size = sizeof(ECOS_NET_PKT_POOL_ENTRY) * RTMP_NET_PKT_POOL_SIZE;
	
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->NetPktPoolLock);
	pEcosNetPktCb->pNetPktPool[0] = kmalloc(mem_size, GFP_ATOMIC);

	if (pEcosNetPktCb->pNetPktPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pNetPktPool[0], mem_size);
		initList(&pEcosNetPktCb->EcosNetPktFreeList);

		for (i = 0; i < RTMP_NET_PKT_POOL_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosNetPktFreeList,
				(RT_LIST_ENTRY *)(pEcosNetPktCb->pNetPktPool[0] + i));

		pEcosNetPktCb->CurNetPktPoolIdx = 1;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pNetPktPool",
					__FUNCTION__));
	}

	return;
}

VOID RTMP_NetPktPoolExpand(
		IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	ULONG CurNetPktPoolIdx = pEcosNetPktCb->CurNetPktPoolIdx;
	UINT mem_size = sizeof(ECOS_NET_PKT_POOL_ENTRY) * RTMP_NET_PKT_POOL_SIZE;
	
	if (CurNetPktPoolIdx >= RTMP_NET_PKT_POOL_SIZE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: reach max allocating limitation. CurPoolIdx-%d\n",
					__FUNCTION__, (INT) CurNetPktPoolIdx));
		return;
	}

	pEcosNetPktCb->pNetPktPool[CurNetPktPoolIdx] = kmalloc(mem_size, GFP_ATOMIC);

	if (pEcosNetPktCb->pNetPktPool[CurNetPktPoolIdx])
	{
		NdisZeroMemory(pEcosNetPktCb->pNetPktPool[CurNetPktPoolIdx], mem_size);
		initList(&pEcosNetPktCb->EcosNetPktFreeList);
		for (i = 0; i < RTMP_NET_PKT_POOL_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosNetPktFreeList,
				(RT_LIST_ENTRY *)(pEcosNetPktCb->pNetPktPool[CurNetPktPoolIdx] + i));
		pEcosNetPktCb->CurNetPktPoolIdx++;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pNetPktPool",
			__FUNCTION__));
	}

	return;
}

VOID RTMP_NetPktPoolDestory(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	for (i=0; i< pEcosNetPktCb->CurNetPktPoolIdx; i++)
	{
		if (pEcosNetPktCb->pNetPktPool[i])
			kfree(pEcosNetPktCb->pNetPktPool[i]);

		pEcosNetPktCb->pNetPktPool[i] = NULL;
	}
}

PECOS_NET_PKT_POOL_ENTRY RTMP_NetPktAlloc(
	IN PRTMP_ADAPTER pAd)
{
	PECOS_NET_PKT_POOL_ENTRY pNetPktEntry;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->NetPktPoolLock);

	pNetPktEntry = (PECOS_NET_PKT_POOL_ENTRY)removeHeadList(&pEcosNetPktCb->EcosNetPktFreeList);
	/* expand Net buffer. */
	if (pNetPktEntry == NULL)
	{
		RTMP_NetPktPoolExpand(pAd);
		pNetPktEntry = (PECOS_NET_PKT_POOL_ENTRY)removeHeadList(&pEcosNetPktCb->EcosNetPktFreeList);
	}
/*Eddy ? */
/*	if (pNetPktEntry == NULL) */
/*		pNetPktEntry->ref_cnt = 1; */

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->NetPktPoolLock);

	return pNetPktEntry;
}

VOID RTMP_NetPktFree(
	IN PRTMP_ADAPTER pAd,
	IN PECOS_NET_PKT_POOL_ENTRY pNetPkEntry)
{
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->NetPktPoolLock);

	insertTailList(&pEcosNetPktCb->EcosNetPktFreeList, (RT_LIST_ENTRY *)pNetPkEntry);

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->NetPktPoolLock);

	return;
}

VOID RTMP_NetRxBufPoolInit(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	UINT mem_size = sizeof(RTMP_NET_RXBUF_POOL_ENTRY) * RTMP_NET_BUF_POOL_SIZE;
	
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->NetRxBufPoolLock);

	pEcosNetPktCb->pNetRxBufPool[0] =
		kmalloc(mem_size, GFP_ATOMIC);
	if (pEcosNetPktCb->pNetRxBufPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pNetRxBufPool[0], mem_size);
		initList(&pEcosNetPktCb->EcosNetRxBufFreeList);
		for (i = 0; i < RTMP_NET_BUF_POOL_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosNetRxBufFreeList, (RT_LIST_ENTRY *)(pEcosNetPktCb->pNetRxBufPool[0] + i));
		pEcosNetPktCb->CurNetRxBufPoolIdx = 1;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pNetRxBufPool", __FUNCTION__));
	}

	return;
}

VOID RTMP_NetRxBufPoolDestory(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	for (i=0; i< pEcosNetPktCb->CurNetRxBufPoolIdx; i++)
	{
		if (pEcosNetPktCb->pNetRxBufPool[i])
			kfree(pEcosNetPktCb->pNetRxBufPool[i]);

		pEcosNetPktCb->pNetRxBufPool[i] = NULL;
	}
}

PRTMP_NET_RXBUF_POOL_ENTRY RTMP_NetRxBufAlloc(
	IN PRTMP_ADAPTER pAd)
{
	PRTMP_NET_RXBUF_POOL_ENTRY pNetBufEntry;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->NetRxBufPoolLock);

	pNetBufEntry = (PRTMP_NET_RXBUF_POOL_ENTRY)removeHeadList(&pEcosNetPktCb->EcosNetRxBufFreeList);
	
	if (pNetBufEntry == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s No NetRxBuf",
					__FUNCTION__));
	}

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->NetRxBufPoolLock);

	return pNetBufEntry;
}

VOID RTMP_NetRxBufFree(
	IN PRTMP_ADAPTER pAd,
	IN PECOS_NET_PKT_POOL_ENTRY pNetBufEntry)
{
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->NetRxBufPoolLock);

	insertTailList(&pEcosNetPktCb->EcosNetRxBufFreeList, (RT_LIST_ENTRY *)pNetBufEntry);

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->NetRxBufPoolLock);

	return;
}

VOID RTMP_NetTxBufPoolInit(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	UINT mem_size = sizeof(RTMP_NET_TXBUF_POOL_ENTRY) * RX_BUFFER_NORMSIZE;
	
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->NetTxBufPoolLock);

	pEcosNetPktCb->pNetTxBufPool[0] = kmalloc(mem_size, GFP_ATOMIC);
	if (pEcosNetPktCb->pNetTxBufPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pNetTxBufPool[0], mem_size);
		initList(&pEcosNetPktCb->EcosNetTxBufFreeList);
		for (i = 0; i < RTMP_NET_BUF_POOL_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosNetRxBufFreeList, (RT_LIST_ENTRY *)(pEcosNetPktCb->pNetTxBufPool[0] + i));
		pEcosNetPktCb->CurNetTxBufPoolIdx = 1;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pNetTxBufPool", __FUNCTION__));
	}

	return;
}

VOID RTMP_NetTxBufPoolDestory(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	for (i=0; i< pEcosNetPktCb->CurNetTxBufPoolIdx; i++)
	{
		if (pEcosNetPktCb->pNetTxBufPool[i])
			kfree(pEcosNetPktCb->pNetTxBufPool[i]);

		pEcosNetPktCb->pNetTxBufPool[i] = NULL;
	}
}

PRTMP_NET_TXBUF_POOL_ENTRY RTMP_NetTxBufAlloc(
	IN PRTMP_ADAPTER pAd)
{
	PRTMP_NET_RXBUF_POOL_ENTRY pNetBufEntry;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->NetTxBufPoolLock);

	pNetBufEntry = (PRTMP_NET_TXBUF_POOL_ENTRY)removeHeadList(&pEcosNetPktCb->EcosNetTxBufFreeList);
	
	if (pNetBufEntry == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s No NetTxBuf",
					__FUNCTION__));
	}

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->NetTxBufPoolLock);

	return pNetBufEntry;
}

VOID RTMP_NetTxBufFree(
	IN PRTMP_ADAPTER pAd,
	IN PECOS_NET_PKT_POOL_ENTRY pNetBufEntry)
{
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->NetTxBufPoolLock);

	insertTailList(&pEcosNetPktCb->EcosNetTxBufFreeList, (RT_LIST_ENTRY *)pNetBufEntry);

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->NetTxBufPoolLock);

	return;
}

VOID RTMP_TxDPoolInit(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	UINT mem_size = sizeof(ECOS_TXD_POOL_ENTRY)  * TX_RING_SIZE;

	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->TxDPoolLock);

	pEcosNetPktCb->pTxDPool[0] = kmalloc(mem_size, GFP_ATOMIC);

	if (pEcosNetPktCb->pTxDPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pTxDPool[0], mem_size);
		initList(&pEcosNetPktCb->EcosTxDFreeList);

		for (i = 0; i < TX_RING_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosTxDFreeList,
				(RT_LIST_ENTRY *)(pEcosNetPktCb->pTxDPool[0] + i));

		/*pEcosNetPktCb->CurTxDPoolIdx = 1; */
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pTxDPool",
					__FUNCTION__));
	}

	return;
}

VOID RTMP_TxDPoolDestory(
	IN PRTMP_ADAPTER pAd)
{	
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	
	if (pEcosNetPktCb->pTxDPool[0])
			kfree(pEcosNetPktCb->pTxDPool[0]);

	pEcosNetPktCb->pTxDPool[0] = NULL;
	
}

PECOS_TXD_POOL_ENTRY RTMP_TxDAlloc(
	IN PRTMP_ADAPTER pAd)
{
	PECOS_TXD_POOL_ENTRY pTXDEntry;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->TxDPoolLock);

	pTXDEntry = (PECOS_NET_PKT_POOL_ENTRY)removeHeadList(&pEcosNetPktCb->EcosTxDFreeList);

	if (pTXDEntry == NULL)
		DBGPRINT(RT_DEBUG_ERROR, ("%s No TXD",
					__FUNCTION__));

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->TxDPoolLock);

	return pTXDEntry;
}


VOID RTMP_TxDFree(
	IN PRTMP_ADAPTER pAd,
	IN PECOS_TXD_POOL_ENTRY pTxDEntry)
{
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->TxDPoolLock);

	insertTailList(&pEcosNetPktCb->EcosTxDFreeList, (RT_LIST_ENTRY *)pTxDEntry);

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->TxDPoolLock);

	return;
}

VOID RTMP_RxDPoolInit(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	UINT mem_size = sizeof(ECOS_RXD_POOL_ENTRY) * RX_RING_SIZE;
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->RxDPoolLock);

	pEcosNetPktCb->pRxDPool[0] = kmalloc(mem_size, GFP_ATOMIC);

	if (pEcosNetPktCb->pRxDPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pRxDPool[0], mem_size);
		initList(&pEcosNetPktCb->EcosRxDFreeList);

		for (i = 0; i < RX_RING_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosRxDFreeList,
				(RT_LIST_ENTRY *)(pEcosNetPktCb->pRxDPool[0] + i));

		/*pEcosNetPktCb->CurTxDPoolIdx = 1; */
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pRxDPool",
					__FUNCTION__));
	}

	return;
}

VOID RTMP_RxDPoolDestory(
	IN PRTMP_ADAPTER pAd)
{	
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	
	if (pEcosNetPktCb->pRxDPool[0])
			kfree(pEcosNetPktCb->pRxDPool[0]);

	pEcosNetPktCb->pRxDPool[0] = NULL;
	
}

PECOS_TXD_POOL_ENTRY RTMP_RxDAlloc(
	IN PRTMP_ADAPTER pAd)
{
	PECOS_TXD_POOL_ENTRY pRXDEntry;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->RxDPoolLock);

	pRXDEntry = (PECOS_NET_PKT_POOL_ENTRY)removeHeadList(&pEcosNetPktCb->EcosRxDFreeList);

	if (pRXDEntry == NULL)
		DBGPRINT(RT_DEBUG_ERROR, ("%s No RXD",
					__FUNCTION__));

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->RxDPoolLock);

	return pRXDEntry;
}


VOID RTMP_RxDFree(
	IN PRTMP_ADAPTER pAd,
	IN PECOS_TXD_POOL_ENTRY pRxDEntry)
{
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->RxDPoolLock);

	insertTailList(&pEcosNetPktCb->EcosRxDFreeList, (RT_LIST_ENTRY *)pRxDEntry);

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->RxDPoolLock);

	return;
}

VOID RTMP_1ST_TxBufPoolInit(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	UINT mem_size = sizeof(ECOS_1ST_TXBUF_POOL_ENTRY) * TX_RING_SIZE;
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->FirstTxBufPoolLock);

	pEcosNetPktCb->p1stTxBufPool[0] =kmalloc(mem_size, GFP_ATOMIC);
	if (pEcosNetPktCb->p1stTxBufPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->p1stTxBufPool[0], mem_size);
		initList(&pEcosNetPktCb->Ecos1stTxBufFreeList);

		for (i = 0; i < TX_RING_SIZE; i++)
			insertTailList(&pEcosNetPktCb->Ecos1stTxBufFreeList,
				(RT_LIST_ENTRY *)(pEcosNetPktCb->p1stTxBufPool[0] + i));
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pTxDPool",
					__FUNCTION__));
	}

	return;
}

VOID RTMP_1ST_TxBufDestory(
	IN PRTMP_ADAPTER pAd)
{	
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;
	
	if (pEcosNetPktCb->p1stTxBufPool[0])
			kfree(pEcosNetPktCb->p1stTxBufPool[0]);

	pEcosNetPktCb->p1stTxBufPool[0] = NULL;	
}

PECOS_TXD_POOL_ENTRY RTMP_1ST_TxBufAlloc(
	IN PRTMP_ADAPTER pAd)
{
	PECOS_TXD_POOL_ENTRY p1stTXBufEntry;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->FirstTxBufPoolLock);

	p1stTXBufEntry = (PECOS_NET_PKT_POOL_ENTRY)removeHeadList(&pEcosNetPktCb->Ecos1stTxBufFreeList);

	if (p1stTXBufEntry == NULL)
		DBGPRINT(RT_DEBUG_ERROR, ("%s No 1st TxBuf",
					__FUNCTION__));

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->FirstTxBufPoolLock);

	return p1stTXBufEntry;
}


VOID RTMP_1ST_TxBufFree(
	IN PRTMP_ADAPTER pAd,
	IN PECOS_TXD_POOL_ENTRY p1stTxBufEntry)
{
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	RTMP_SEM_LOCK(&pEcosNetPktCb->FirstTxBufPoolLock);

	insertTailList(&pEcosNetPktCb->Ecos1stTxBufFreeList, (RT_LIST_ENTRY *)p1stTxBufEntry);

	RTMP_SEM_UNLOCK(&pEcosNetPktCb->FirstTxBufPoolLock);

	return;
}

VOID RTMP_MemPoolInit(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;
	PECOS_NET_PKT_CTRL pEcosNetPktCb = &pAd->eCos_Net_Pkt_Ctrl;

	/* NetPkt memory pool */
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->NetPktPoolLock);

	pEcosNetPktCb->pNetPktPool[0] =
			kmalloc(sizeof(ECOS_NET_PKT_POOL_ENTRY) 
						* RTMP_NET_PKT_POOL_SIZE, GFP_ATOMIC);

	if (pEcosNetPktCb->pNetPktPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pNetPktPool[0],
				sizeof(ECOS_NET_PKT_POOL_ENTRY) * RTMP_NET_PKT_POOL_SIZE);
		initList(&pEcosNetPktCb->EcosNetPktFreeList);

		for (i = 0; i < RTMP_NET_PKT_POOL_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosNetPktFreeList,
				(RT_LIST_ENTRY *)(pEcosNetPktCb->pNetPktPool[0] + i));

		pEcosNetPktCb->CurNetPktPoolIdx = 1;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pNetPktPool",
					__FUNCTION__));
		return;
	}
	
	/* NetRxBuf memory pool */
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->NetRxBufPoolLock);

	pEcosNetPktCb->pNetRxBufPool[0] =
		kmalloc(sizeof(RTMP_NET_RXBUF_POOL_ENTRY)
					* RTMP_NET_BUF_POOL_SIZE, GFP_ATOMIC);
	if (pEcosNetPktCb->pNetRxBufPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pNetRxBufPool[0], sizeof(RTMP_NET_RXBUF_POOL_ENTRY) * RTMP_NET_BUF_POOL_SIZE);
		initList(&pEcosNetPktCb->EcosNetRxBufFreeList);
		for (i = 0; i < RTMP_NET_BUF_POOL_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosNetRxBufFreeList, (RT_LIST_ENTRY *)(pEcosNetPktCb->pNetRxBufPool[0] + i));
		pEcosNetPktCb->CurNetRxBufPoolIdx = 1;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pNetRxBufPool",
					__FUNCTION__));
		return;
	}

	/* NetTxBuf memory pool */
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->NetTxBufPoolLock);

	pEcosNetPktCb->pNetTxBufPool[0] =
		kmalloc(sizeof(RTMP_NET_TXBUF_POOL_ENTRY)
					* RX_BUFFER_NORMSIZE, GFP_ATOMIC);
	if (pEcosNetPktCb->pNetTxBufPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pNetTxBufPool[0], sizeof(RTMP_NET_TXBUF_POOL_ENTRY) * RTMP_NET_BUF_POOL_SIZE);
		initList(&pEcosNetPktCb->EcosNetTxBufFreeList);
		for (i = 0; i < RTMP_NET_BUF_POOL_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosNetRxBufFreeList, (RT_LIST_ENTRY *)(pEcosNetPktCb->pNetTxBufPool[0] + i));
		pEcosNetPktCb->CurNetTxBufPoolIdx = 1;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pNetTxBufPool",
					__FUNCTION__));
		return;
	}

	/* TxD memory pool */
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->TxDPoolLock);

	pEcosNetPktCb->pTxDPool[0] =
			kmalloc(sizeof(ECOS_TXD_POOL_ENTRY) 
						* TX_RING_SIZE, GFP_ATOMIC);

	if (pEcosNetPktCb->pTxDPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pTxDPool[0],
				sizeof(ECOS_TXD_POOL_ENTRY) * TX_RING_SIZE);
		initList(&pEcosNetPktCb->EcosTxDFreeList);

		for (i = 0; i < TX_RING_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosTxDFreeList,
				(RT_LIST_ENTRY *)(pEcosNetPktCb->pTxDPool[0] + i));

		/*pEcosNetPktCb->CurTxDPoolIdx = 1; */
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pTxDPool",
					__FUNCTION__));
		return;		
	}

	/* RxD memory pool */
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->RxDPoolLock);

	pEcosNetPktCb->pRxDPool[0] =
			kmalloc(sizeof(ECOS_RXD_POOL_ENTRY) 
						* RX_RING_SIZE, GFP_ATOMIC);

	if (pEcosNetPktCb->pRxDPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->pRxDPool[0],
				sizeof(ECOS_RXD_POOL_ENTRY) * RX_RING_SIZE);
		initList(&pEcosNetPktCb->EcosRxDFreeList);

		for (i = 0; i < RX_RING_SIZE; i++)
			insertTailList(&pEcosNetPktCb->EcosRxDFreeList,
				(RT_LIST_ENTRY *)(pEcosNetPktCb->pRxDPool[0] + i));

		/*pEcosNetPktCb->CurTxDPoolIdx = 1; */
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pRxDPool",
					__FUNCTION__));
		return;
	}

	/* 1stTxBuf memory pool */
	NdisAllocateSpinLock(pAd, &pEcosNetPktCb->FirstTxBufPoolLock);

	pEcosNetPktCb->p1stTxBufPool[0] =
			kmalloc(sizeof(ECOS_1ST_TXBUF_POOL_ENTRY) 
						* TX_RING_SIZE, GFP_ATOMIC);

	if (pEcosNetPktCb->p1stTxBufPool[0])
	{
		NdisZeroMemory(pEcosNetPktCb->p1stTxBufPool[0],
				sizeof(ECOS_1ST_TXBUF_POOL_ENTRY) * TX_RING_SIZE);
		initList(&pEcosNetPktCb->Ecos1stTxBufFreeList);

		for (i = 0; i < TX_RING_SIZE; i++)
			insertTailList(&pEcosNetPktCb->Ecos1stTxBufFreeList,
				(RT_LIST_ENTRY *)(pEcosNetPktCb->p1stTxBufPool[0] + i));
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->eCos_Net_Pkt_Ctrl.pTxDPool",
					__FUNCTION__));
		return;
	}
	
	return;
}


PVOID RtmpTxNetBuf_put(
	IN PECOS_NET_PKT_POOL_ENTRY pNetPkt,
	IN INT len)
{
	PUCHAR p;

	if (pNetPkt->pFrameBuf == NULL)
		return NULL;
	
	if ((pNetPkt->pFrameBufStart + pNetPkt->pktLen + len) > pNetPkt->pFrameBufEnd)
		return NULL;
	
	p = pNetPkt->pFrameBuf + pNetPkt->pktLen;
	pNetPkt->pktLen += len;
	
	return p;
}


PVOID RtmpTxNetBuf_push(
	IN PECOS_NET_PKT_POOL_ENTRY pNetPkt,
	IN INT len)
{
	ASSERT(pNetPkt);

	if (pNetPkt->pDataHead == NULL)
		return NULL;

	if ((pNetPkt->pDataPtr - len) < pNetPkt->pDataHead)
	{
/*		DBGPRINT(RT_DEBUG_TRACE, ("HdrRoomNotEnough, 0x%x-0x%x", */
/*					pNetPkt->pDataHead, pNetPkt->pDataPtr)); */
		return NULL;
	}
	pNetPkt->pDataPtr -= len;
	pNetPkt->pktLen += len;
	
	return pNetPkt->pDataPtr;
}

PNDIS_PACKET RtmpTxNetPktGet(
	IN PRTMP_ADAPTER pAd,
	IN PNET_DEV net_dev,
	IN unsigned long mBuf,
	IN struct eth_drv_sg *sg_list,
	IN int sg_len,
	IN ULONG PktLen)
{
	PECOS_NET_PKT_POOL_ENTRY pNetPkt;
	ra_dma_addr_t *pDMAAddr;


	pNetPkt = RTMP_NetPktAlloc(pAd);
	if (pNetPkt == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate NetBuf failed!\n", __FUNCTION__));
		goto RtmpTxNetBufGetFailError;
	}

	NdisZeroMemory(pNetPkt, sizeof(PECOS_NET_PKT_POOL_ENTRY));
	if (sg_list && (sg_len > 0))
	{
		INT i;
		RTMP_NET_TXBUF_POOL_ENTRY pNetTxBuf;

		pNetTxBuf = RTMP_NetTxBufAlloc(pAd);

		if (pNetTxBuf == NULL)
			goto RtmpTxNetBufGetBufAllocFailError;

		pNetTxBuf->ref_cnt = 1;
		pNetPkt->pNetTxBuf = pNetTxBuf;
		pNetPkt->sg_list = pNetTxBuf->sg_list;
		for (i=0;i<sg_len;i++)
		{
			memcpy(&pNetPkt->sg_list[i], sg_list[i]);
			pNetPkt->pktLen += sg_list[i].len;
		}

		pNetPkt->pDataHead = sg_list[0].buf;
		pNetPkt->pDataPtr = sg_list[0].buf;
		pNetPkt->net_dev = (PNET_DEV)net_dev;
		pNetPkt->key = mBuf;
	}
	else if (PktLen)
	{
		PRTMP_NET_BUF_POOL_ENTRY pNetRxBuf;
		pNetRxBuf = RTMP_NetRxBufAlloc(pAd);
		if (pNetRxBuf == NULL)
			goto RtmpTxNetBufGetBufAllocFailError;

		pNetRxBuf->ref_cnt = 1;
		pNetPkt->pNetRxBuf  = pNetRxBuf;
		pNetPkt->pFrameBuf = pNetPkt->pNetBuf.pNetbuf;
	}
		return pNetPkt;

RtmpTxNetBufGetBufAllocFailError:
		RTMP_NetPktFree(pAd, pNetPkt);

RtmpTxNetBufGetFailError:
		return NULL;	
	
	return pNetPkt;
}

VOID RtmpTxNetPktFree(
	IN PRTMP_ADAPTER pAd,
	IN PECOS_NET_PKT_POOL_ENTRY  pPacket)
{
}

