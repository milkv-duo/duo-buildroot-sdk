/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	mesh.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2007-08-06      For mesh (802.11s) support.
*/

#ifdef MESH_SUPPORT


#include "rt_config.h"

VOID BMPktSigTabInit(
	IN PRTMP_ADAPTER pAd)
{
	NdisAllocateSpinLock(pAd, &pAd->MeshTab.MeshBMPktTabLock);

	os_alloc_mem(pAd, (UCHAR **)&(pAd->MeshTab.pBMPktSigTab), sizeof(MESH_BMPKTSIG_TAB));
	if (pAd->MeshTab.pBMPktSigTab)
		NdisZeroMemory(pAd->MeshTab.pBMPktSigTab, sizeof(MESH_BMPKTSIG_TAB));
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->MeshTab.pBMPktSigTab", __FUNCTION__));

	return;
}

VOID BMPktSigTabExit(
	IN PRTMP_ADAPTER pAd)
{
	NdisFreeSpinLock(&pAd->MeshTab.MeshBMPktTabLock);

	if (pAd->MeshTab.pBMPktSigTab)
		os_free_mem(NULL, pAd->MeshTab.pBMPktSigTab);
	pAd->MeshTab.pBMPktSigTab = NULL;

	return;
}

PMESH_BMPKTSIG_ENTRY BMPktSigTabLookUp(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR			MeshSA)
{
	UINT HashIdx;
	ULONG Now;
	PMESH_BMPKTSIG_TAB pTab = pAd->MeshTab.pBMPktSigTab;
	PMESH_BMPKTSIG_ENTRY pEntry = NULL;
	PMESH_BMPKTSIG_ENTRY pPrevEntry = NULL;

	if (pTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: pBMPktSigTab doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshBMPktTabLock);

	HashIdx = BMPKT_MAC_ADDR_HASH_INDEX(MeshSA);
	pEntry = pTab->Hash[HashIdx];

	while (pEntry)
	{
		if (MAC_ADDR_EQUAL(pEntry->MeshSA, MeshSA)) 
			break;
		else
		{
			pPrevEntry = pEntry;
			pEntry = pEntry->pNext;
		}
	}

	if (pEntry)
	{
		NdisGetSystemUpTime(&Now);
		if (RTMP_TIME_AFTER((unsigned long)Now, (unsigned long)(pEntry->LastRefTime + MESH_BMPKT_RECORD_TIME)))
		{ /* remove ageout entry. */
			if (pPrevEntry == NULL)
				pTab->Hash[HashIdx] = pEntry->pNext;
			else
				pPrevEntry->pNext = pEntry->pNext;

			NdisZeroMemory(pEntry, sizeof(MESH_BMPKTSIG_ENTRY));
			pEntry = NULL;
			pTab->Size--;
		}
		else
			pEntry->LastRefTime = Now;
	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshBMPktTabLock);
	
	return pEntry;
}

PMESH_BMPKTSIG_ENTRY BMPktSigTabInsert(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR			MeshSA)
{
	INT i;
	ULONG HashIdx;
	PMESH_BMPKTSIG_TAB pTab = pAd->MeshTab.pBMPktSigTab;
	PMESH_BMPKTSIG_ENTRY pEntry = NULL, pCurrEntry;
	ULONG Now;

	if(pTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: pBMPktSigTab doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	pEntry = BMPktSigTabLookUp(pAd, MeshSA);
	if (pEntry == NULL)
	{
		/* if FULL, return */
		if (pTab->Size >= MAX_BMPKTSIG_TAB_SIZE)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: pBMPktSigTab FULL.\n", __FUNCTION__));
			return NULL;
		}

		RTMP_SEM_LOCK(&pAd->MeshTab.MeshBMPktTabLock);
		for (i = 0; i < MAX_BMPKTSIG_TAB_SIZE; i++)
		{
			NdisGetSystemUpTime(&Now);
			pEntry = &pTab->Content[i];
 
			if ((pEntry->Valid == TRUE)
				&& RTMP_TIME_AFTER((unsigned long)Now, (unsigned long)(pEntry->LastRefTime + MESH_BMPKT_RECORD_TIME)))
			{
				PMESH_BMPKTSIG_ENTRY pPrevEntry = NULL;
				ULONG HashIdx = BMPKT_MAC_ADDR_HASH_INDEX(pEntry->MeshSA);
				PMESH_BMPKTSIG_ENTRY pProbeEntry = pTab->Hash[HashIdx];

				/* update Hash list */
				do
				{
					if (pProbeEntry == pEntry)
					{
						if (pPrevEntry == NULL)
						{
							pTab->Hash[HashIdx] = pEntry->pNext;
						}
						else
						{
							pPrevEntry->pNext = pEntry->pNext;
						}
						break;
					}

					pPrevEntry = pProbeEntry;
					pProbeEntry = pProbeEntry->pNext;
				} while (pProbeEntry);

				NdisZeroMemory(pEntry, sizeof(MESH_BMPKTSIG_ENTRY));
				pTab->Size--;

				continue;
			}
 
			if (pEntry->Valid == FALSE)
				break;
		}

		if (i < MAX_BMPKTSIG_TAB_SIZE)
		{
			NdisGetSystemUpTime(&Now);
			pEntry->LastRefTime = Now;
			pEntry->Valid = TRUE;
			COPY_MAC_ADDR(pEntry->MeshSA, MeshSA);
			pTab->Size++;
		}
		else
		{
			pEntry = NULL;
			DBGPRINT(RT_DEBUG_ERROR, ("%s: pBMPktSigTab tab full.\n", __FUNCTION__));
		}

		/* add this Neighbor entry into HASH table */
		if (pEntry)
		{
			HashIdx = BMPKT_MAC_ADDR_HASH_INDEX(MeshSA);
			if (pTab->Hash[HashIdx] == NULL)
			{
				pTab->Hash[HashIdx] = pEntry;
			}
			else
			{
				pCurrEntry = pTab->Hash[HashIdx];
				while (pCurrEntry->pNext != NULL)
					pCurrEntry = pCurrEntry->pNext;
				pCurrEntry->pNext = pEntry;
			}
		}

		RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshBMPktTabLock);
	} 

	return pEntry;
}

VOID BMPktSigTabDelete(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR			MeshSA)
{
	PMESH_BMPKTSIG_TAB pTab = pAd->MeshTab.pBMPktSigTab;
	PMESH_BMPKTSIG_ENTRY pEntry = NULL;

	if(pTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: pBMPktSigTab doesn't exist.\n", __FUNCTION__));
		return;
	}

	/* if empty, return */
	if (pTab->Size == 0) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("pBMPktSigTab empty.\n"));
		return;
	}

	pEntry = BMPktSigTabLookUp(pAd, MeshSA);
	if (pEntry != NULL)
	{
		PMESH_BMPKTSIG_ENTRY pPrevEntry = NULL;
		ULONG HashIdx = BMPKT_MAC_ADDR_HASH_INDEX(pEntry->MeshSA);
		PMESH_BMPKTSIG_ENTRY pProbeEntry = pTab->Hash[HashIdx];

		RTMP_SEM_LOCK(&pAd->MeshTab.MeshBMPktTabLock);
		/* update Hash list */
		do
		{
			if (pProbeEntry == pEntry)
			{
				if (pPrevEntry == NULL)
				{
					pTab->Hash[HashIdx] = pEntry->pNext;
				}
				else
				{
					pPrevEntry->pNext = pEntry->pNext;
				}
				break;
			}

			pPrevEntry = pProbeEntry;
			pProbeEntry = pProbeEntry->pNext;
		} while (pProbeEntry);

		NdisZeroMemory(pEntry, sizeof(MESH_BMPKTSIG_ENTRY));
		pTab->Size--;

		RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshBMPktTabLock);
	}

	return;
}


BOOLEAN PktSigCheck(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR			pTA,
	IN PUCHAR			pDA,
	IN PUCHAR			pSA,
	IN UINT32			MeshSeq,
	IN ULONG			FrameType)
{
	BOOLEAN result = FALSE;
	PMESH_BMPKTSIG_ENTRY pBMPktSigEntry = NULL;

	do
	{
		if ((FrameType == FC_TYPE_MGMT) || (*pDA & 0x01))
		{
			if ((pBMPktSigEntry = BMPktSigTabLookUp(pAd, pSA)) == NULL)
			{
				if ((pBMPktSigEntry = BMPktSigTabInsert(pAd, pSA)) == NULL)
						break;

				pBMPktSigEntry->MeshSeqBased = MeshSeq;
				COPY_MAC_ADDR(pBMPktSigEntry->Precursor, pTA);
				NdisZeroMemory(pBMPktSigEntry->Offset, sizeof(UINT32) * 4);
			}
			else
			{
				UINT32 DevNum;
				UINT32 RemainNum;
				UINT32 SeqDiff;

				if (MESH_SEQ_AFTER(pBMPktSigEntry->MeshSeqBased, MeshSeq))
					break;

				SeqDiff = MESH_SEQ_SUB(MeshSeq, pBMPktSigEntry->MeshSeqBased);
				if (SeqDiff == 0)
					break;

				if (SeqDiff > 128)
				{
					pBMPktSigEntry->MeshSeqBased = MeshSeq;
					NdisZeroMemory(pBMPktSigEntry->Offset, sizeof(UINT32) * 4);
				}
				else
				{
					DevNum = (SeqDiff - 1) / 32;
					RemainNum = (SeqDiff - 1) % 32;
					if (pBMPktSigEntry->Offset[DevNum] & (1 << RemainNum))
						break;
					else
						pBMPktSigEntry->Offset[DevNum] |= (1 << RemainNum);
				}
			}
		}

#if 0
		/* mesh unicast packet doesn't need to check packet signature. */
		else
		{
			if((pBMPktSigEntry = BMPktSigTabLookUp(pAd, pSA)) == NULL)
			{
				if((pBMPktSigEntry = BMPktSigTabInsert(pAd, pSA)) == NULL)
					break;
				pBMPktSigEntry->UcaseMeshSeq = MeshSeq;
				COPY_MAC_ADDR(pBMPktSigEntry->Precursor, pTA);
			}
			else
			{
				if (MESH_SEQ_AFTER(pBMPktSigEntry->UcaseMeshSeq, MeshSeq))
					break;
				pBMPktSigEntry->UcaseMeshSeq = MeshSeq;
			}
		}
#endif 
		result = TRUE;
	} while (FALSE);

	if ((result == FALSE)
		&& (FrameType == FC_TYPE_DATA)
		&& IS_MULTICAST_MAC_ADDR(pDA)
		&& (pBMPktSigEntry != NULL)
		&& !MAC_ADDR_EQUAL(pBMPktSigEntry->Precursor, pTA))
	{
		MeshMultipathNotice(pAd,  pTA, pSA, 1);
	}

	return result;
}

VOID MeshClonePacket(
	IN PRTMP_ADAPTER pAd,
	IN PNDIS_PACKET pPacket,
	IN UINT8 MeshSrc,
	IN UINT8 MeshLinkIdx)
{
	INT idx;
	PUCHAR pSA = NULL;
	PUCHAR pDA = NULL;

	for (idx = 0; idx < MAX_MESH_LINKS; idx++)
	{
		if ((MeshSrc == MESH_FORWARD)
			&& (MeshLinkIdx == idx))
			continue;

		pSA = GET_OS_PKT_DATAPTR(pPacket) + MAC_ADDR_LEN;
		if (MAC_ADDR_EQUAL(pAd->MeshTab.MeshLink[idx].Entry.PeerMacAddr, pSA))
			continue;

		pDA = GET_OS_PKT_DATAPTR(pPacket);
		if (IS_MULTICAST_MAC_ADDR(pDA)
			&& (MultipathEntryLookUp(pAd, idx, pSA) != NULL))
			continue;

		if (PeerLinkValidCheck(pAd, idx) == TRUE)
		{
			PNDIS_PACKET pPacketClone;

/*			pPacketClone = skb_clone(RTPKT_TO_OSPKT(pPacket), GFP_ATOMIC); */
			OS_PKT_CLONE(pAd, pPacket, pPacketClone, GFP_ATOMIC);
			if (pPacketClone == NULL)
				continue;

			RTMP_SET_PACKET_MOREDATA(pPacketClone, FALSE);
			RTMP_SET_PACKET_WCID(pPacketClone, pAd->MeshTab.MeshLink[idx].Entry.MacTabMatchWCID);
			RTMP_SET_PACKET_WDEV(pPacket, pAd->MeshTab.wdev.wdev_idx);
			RTMP_SET_MESH_ROUTE_ID(pPacketClone, BMCAST_ROUTE_ID);
			RTMP_SET_MESH_SOURCE(pPacketClone, MeshSrc);
#ifdef CONFIG_AP_SUPPORT
			APSendPacket(pAd, pPacketClone);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
			STASendPacket(pAd, pPacketClone);
#endif /* CONFIG_STA_SUPPORT */
		}
	}

	return;
}

VOID MultipathPoolInit(
	IN PRTMP_ADAPTER pAd)
{
	ULONG i;

	NdisAllocateSpinLock(pAd, &pAd->MeshTab.MultipathTabLock);

	os_alloc_mem(pAd, (UCHAR **)&(pAd->MeshTab.pMultipathEntryPool), sizeof(MESH_MULTIPATH_ENTRY) * MULTIPATH_POOL_SIZE);
	if (pAd->MeshTab.pMultipathEntryPool)
	{
		NdisZeroMemory(pAd->MeshTab.pMultipathEntryPool, sizeof(MESH_BMPKTSIG_TAB));
		initList(&pAd->MeshTab.MultipathEntryFreeList);
		for (i = 0; i < MULTIPATH_POOL_SIZE; i++)
			insertTailList(&pAd->MeshTab.MultipathEntryFreeList, (RT_LIST_ENTRY *)(pAd->MeshTab.pMultipathEntryPool + (ULONG)i));
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->MeshTab.pMultipathEntryPool", __FUNCTION__));

	return;
}

VOID MultipathPoolExit(
	IN PRTMP_ADAPTER pAd)
{
	NdisFreeSpinLock(&pAd->MeshTab.MultipathTabLock);

	if (pAd->MeshTab.pMultipathEntryPool)
		os_free_mem(NULL, pAd->MeshTab.pMultipathEntryPool);
	pAd->MeshTab.pMultipathEntryPool = NULL;

	return;
}

PMESH_MULTIPATH_ENTRY MultipathEntyAlloc(
	IN PRTMP_ADAPTER pAd)
{
	PMESH_MULTIPATH_ENTRY pEntry;

	RTMP_SEM_LOCK(&pAd->MeshTab.MultipathTabLock);

	pEntry = (PMESH_MULTIPATH_ENTRY)removeHeadList(&pAd->MeshTab.MultipathEntryFreeList);

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MultipathTabLock);

	return pEntry;
}

VOID MultipathEntyFree(
	IN PRTMP_ADAPTER pAd,
	IN PMESH_MULTIPATH_ENTRY pEntry)
{
	RTMP_SEM_LOCK(&pAd->MeshTab.MultipathTabLock);

	insertTailList(&pAd->MeshTab.MultipathEntryFreeList, (RT_LIST_ENTRY *)pEntry);

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MultipathTabLock);

	return;
}

VOID MultipathEntryInsert(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR LinkIdx,
	IN PUCHAR pMac)
{
	PMESH_MULTIPATH_ENTRY pEntry;
	UINT8 HashId = *(pMac + 5);
	ULONG Now;

	if (!VALID_MESH_LINK_ID(LinkIdx))
		return;

	pEntry = MultipathEntryLookUp(pAd, LinkIdx, pMac);
	if(pEntry == NULL)
	{
		if ((pEntry = MultipathEntyAlloc(pAd)) == NULL)
			return;
		insertTailList(&pAd->MeshTab.MeshLink[LinkIdx].Entry.MultiPathHash[HashId], (RT_LIST_ENTRY *)pEntry);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Block multipath pkt to link(%d) from SA=%02x:%02x:%02x:%02x:%02x:%02x\n",
		LinkIdx, pMac[0], pMac[1], pMac[2], pMac[3], pMac[4], pMac[5]));

	NdisGetSystemUpTime(&Now);
	COPY_MAC_ADDR(pEntry->MeshSA, pMac);
	pEntry->ReferTime = Now;

	return;
}

VOID MultipathEntryDelete(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR LinkIdx,
	IN PUCHAR pMac)
{
	UINT8 HashId = *(pMac + 5);
	PMESH_MULTIPATH_ENTRY pEntry;

	if (!VALID_MESH_LINK_ID(LinkIdx))
		return;

	pEntry = MultipathEntryLookUp(pAd, LinkIdx, pMac);
	
	if (pEntry != NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("release multipath pkt to link(%d) from SA=%02x:%02x:%02x:%02x:%02x:%02x\n",
			LinkIdx, pMac[0], pMac[1], pMac[2], pMac[3], pMac[4], pMac[5]));

		delEntryList(&pAd->MeshTab.MeshLink[LinkIdx].Entry.MultiPathHash[HashId], (RT_LIST_ENTRY *)pEntry);
		MultipathEntyFree(pAd, pEntry);
	}
}

VOID MultipathListDelete(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR LinkIdx)
{
	ULONG i;
	PMESH_MULTIPATH_ENTRY pEntry;

	if (!VALID_MESH_LINK_ID(LinkIdx))
		return;

	DBGPRINT(RT_DEBUG_TRACE, ("release all multipath pkt of link(%d)\n", LinkIdx));

	for (i = 0; i < MULTIPATH_HASH_TAB_SIZE; i++)
	{
		pEntry = (PMESH_MULTIPATH_ENTRY)removeHeadList(&pAd->MeshTab.MeshLink[LinkIdx].Entry.MultiPathHash[i]);
		while (pEntry != NULL)
		{
			MultipathEntyFree(pAd, pEntry);
			pEntry = (PMESH_MULTIPATH_ENTRY)removeHeadList(&pAd->MeshTab.MeshLink[LinkIdx].Entry.MultiPathHash[i]);
		}
	}
	return;
}

PMESH_MULTIPATH_ENTRY MultipathEntryLookUp(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR LinkIdx,
	IN PUCHAR pMac)
{
	UINT8 HashId = *(pMac + 5);
	PMESH_MULTIPATH_ENTRY pEntry;	

	if (!VALID_MESH_LINK_ID(LinkIdx))
		return NULL;

	pEntry = (PMESH_MULTIPATH_ENTRY)(pAd->MeshTab.MeshLink[LinkIdx].Entry.MultiPathHash[HashId].pHead);
	while (pEntry != NULL)
	{
		if(MAC_ADDR_EQUAL(pEntry->MeshSA, pMac))
			break;
		pEntry = pEntry->pNext;
	}

	return pEntry;
}

VOID MultipathEntryMaintain(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR LinkIdx)
{
	ULONG i;
	PMESH_MULTIPATH_ENTRY pEntry;
	ULONG Now;

	if (!VALID_MESH_LINK_ID(LinkIdx))
		return;

	NdisGetSystemUpTime(&Now);
	for (i = 0; i < MULTIPATH_HASH_TAB_SIZE; i++)
	{
		pEntry = (PMESH_MULTIPATH_ENTRY)(pAd->MeshTab.MeshLink[LinkIdx].Entry.MultiPathHash[i].pHead);
		while (pEntry)
		{
			PMESH_MULTIPATH_ENTRY pEntryNext = pEntry->pNext;
			if (RTMP_TIME_AFTER(Now, pEntry->ReferTime + (pAd->MeshTab.MeshMultiCastAgeOut * OS_HZ / 1000)))
			{
				delEntryList(&pAd->MeshTab.MeshLink[LinkIdx].Entry.MultiPathHash[i], (RT_LIST_ENTRY *)pEntry);
				MultipathEntyFree(pAd, pEntry);
			}
			pEntry = pEntryNext;
		}
	}
	return;
}

#endif /* MESH_SUPPORT */

