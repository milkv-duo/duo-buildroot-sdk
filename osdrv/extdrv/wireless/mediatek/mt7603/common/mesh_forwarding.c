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
	mesh_forwarding.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Arvin		2007-07-04      For mesh (802.11s) support.
*/
#ifdef MESH_SUPPORT


#include "rt_config.h"
#include "mesh_def.h"

NDIS_STATUS MeshEntryTable_Init(
			IN PRTMP_ADAPTER pAd)
{
	NdisAllocateSpinLock(pAd, &pAd->MeshTab.MeshEntryTabLock);

	pAd->MeshTab.pMeshEntryTab = vmalloc(sizeof(MESH_ENTRY_TABLE));

	if (pAd->MeshTab.pMeshEntryTab)
		NdisZeroMemory(pAd->MeshTab.pMeshEntryTab, sizeof(MESH_ENTRY_TABLE));
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->MeshTab.pMeshEntryTab", __FUNCTION__));

	return TRUE;
}

NDIS_STATUS MeshEntryTable_Exit(
			IN PRTMP_ADAPTER pAd)
{
	INT	i;
	MESH_ENTRY *pMeshEntry;
	PMESH_ENTRY_TABLE pEntryTab = pAd->MeshTab.pMeshEntryTab;

	if (pEntryTab)
	{
		if (pEntryTab->Size == 0)
		{
			vfree(pEntryTab);
			pEntryTab = NULL;
			return TRUE;
		}
	}
	else
	{
		return TRUE;
	}


	for (i=0; i < HASH_TABLE_SIZE; i++)
	{
		while((pMeshEntry = pEntryTab->Hash[i]) != NULL)
		{
			BOOLEAN Cancelled;

			if (pMeshEntry->PathReq)
			{
				RTMPCancelTimer(&pMeshEntry->PathReq->PathReqTimer, &Cancelled);
				os_free_mem(NULL, pMeshEntry->PathReq);
				pMeshEntry->PathReq = NULL;
			}

			pEntryTab->Hash[i] = pMeshEntry->pNext;
			os_free_mem(NULL, pMeshEntry);
		}
	}

	vfree(pEntryTab);
	pEntryTab = NULL;

	NdisFreeSpinLock(&pAd->MeshTab.MeshEntryTabLock);

	return TRUE;
}

PMESH_ENTRY MeshEntryTableLookUp(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR			DestAddr)
{
	UINT	HashIdx;
	PMESH_ENTRY	pEntry = NULL;
	PMESH_ENTRY_TABLE pEntryTab = pAd->MeshTab.pMeshEntryTab;

	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshEntryTableLookUp\n"));
#endif /* RELEASE_EXCLUDE */
	if (pEntryTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Entry Table doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshEntryTabLock);

	HashIdx = MAC_ADDR_HASH_INDEX(DestAddr);
	pEntry = pEntryTab->Hash[HashIdx];

	while (pEntry)
	{
		if (MAC_ADDR_EQUAL(pEntry->DestAddr, DestAddr)) 
			break;
		else
			pEntry = pEntry->pNext;
	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshEntryTabLock);
	
	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshEntryTableLookUp\n"));
#endif /* RELEASE_EXCLUDE */
	return pEntry;
}

PMESH_ENTRY MeshEntryTableInsert(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR		DestAddr,
	IN UCHAR			Idx)
{
	UINT	HashIdx;
	MESH_ENTRY	*pNewEntry =NULL, *pCurrEntry;
	PMESH_ENTRY_TABLE pEntryTab = pAd->MeshTab.pMeshEntryTab;

	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshEntryTableInsert\n"));
#endif /* RELEASE_EXCLUDE */
	if (pEntryTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Entry Table doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	if (MAC_ADDR_EQUAL(ZERO_MAC_ADDR, DestAddr))
		return NULL;

	if (pEntryTab->Size > MAX_HASH_ENTRY_TAB_SIZE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Entry Table size more than 256 .\n", __FUNCTION__));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshEntryTabLock);

	HashIdx = MAC_ADDR_HASH_INDEX(DestAddr);
	os_alloc_mem(NULL, (UCHAR **)&pNewEntry, sizeof(MESH_ENTRY));
	if (pNewEntry)
	{
		NdisZeroMemory(pNewEntry, sizeof(MESH_ENTRY));

		pNewEntry->Idx = Idx;
		COPY_MAC_ADDR(pNewEntry->DestAddr, DestAddr);
		pNewEntry->pNext = NULL;

		if (pEntryTab->Hash[HashIdx] == NULL)
		{	/* Hash list is empty, directly assign it. */
			pEntryTab->Hash[HashIdx] = pNewEntry;
		}
		else 
		{
			/* Ok, we insert the new entry into the Hash[HashIdx] */
			pCurrEntry = pEntryTab->Hash[HashIdx];
			while (pCurrEntry->pNext != NULL)
				pCurrEntry = pCurrEntry->pNext;
			pCurrEntry->pNext = pNewEntry;
		}
		pEntryTab->Size++;
	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshEntryTabLock);

	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshEntryTableInsert\n"));
#endif /* RELEASE_EXCLUDE */
	return pNewEntry;
}

BOOLEAN MeshEntryTableDelete(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR		DestAddr)
{
	UINT	HashIdx;
	MESH_ENTRY	*pEntry = NULL, *pPrev = NULL;
	PMESH_ENTRY_TABLE pEntryTab = pAd->MeshTab.pMeshEntryTab;

	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshEntryTableDelete\n"));
#endif /* RELEASE_EXCLUDE */
	if (pEntryTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Entry Table doesn't exist.\n", __FUNCTION__));
		return FALSE;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshEntryTabLock);

	HashIdx = MAC_ADDR_HASH_INDEX(DestAddr);
	pEntry = pPrev = pEntryTab->Hash[HashIdx];

	while (pEntry)
	{
		/* Find the existed Mapping entry */
		if (MAC_ADDR_EQUAL(pEntry->DestAddr, DestAddr))
		{
			if (pEntry->PathReq)
			{
				BOOLEAN	Cancelled;

				RTMPCancelTimer(&pEntry->PathReq->PathReqTimer, &Cancelled);

				os_free_mem(NULL, pEntry->PathReq);
				pEntry->PathReq = NULL;
			}
			pEntry->PathReqTimerRunning = FALSE;
   
			if (pPrev == pEntry)
				pEntryTab->Hash[HashIdx] = pEntry->pNext;
			else
				pPrev->pNext = pEntry->pNext;

			break;
		}
		else
		{
			pPrev = pEntry;
			pEntry = pEntry->pNext;
		}
	}

	/*remove this entry from Hash list. */
	if (pEntry)
	{
		pEntryTab->Size--;
		os_free_mem(NULL, pEntry);
	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshEntryTabLock);

	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshEntryTableDelete\n"));
#endif /* RELEASE_EXCLUDE */
	return TRUE;
}

PMESH_ENTRY MeshEntryTableUpdate(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR			DestAddr,
	IN UCHAR			Idx)
{
	UINT	HashIdx;
	MESH_ENTRY	*pEntry = NULL;
	PMESH_ENTRY_TABLE pEntryTab = pAd->MeshTab.pMeshEntryTab;

	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshEntryTableUpdate\n"));
#endif /* RELEASE_EXCLUDE */
	if (pEntryTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Entry Table doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshEntryTabLock);

	HashIdx = MAC_ADDR_HASH_INDEX(DestAddr);
	pEntry = pEntryTab->Hash[HashIdx];

	while (pEntry)
	{
		/* Find the existed Mapping entry */
		if (MAC_ADDR_EQUAL(pEntry->DestAddr, DestAddr))
		{
			pEntry->Idx = Idx;
			COPY_MAC_ADDR(pEntry->DestAddr, DestAddr);
			break;
		}
		else
		{
			pEntry = pEntry->pNext;
		}
	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshEntryTabLock);
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshEntryTableUpdate\n"));
#endif /* RELEASE_EXCLUDE */


	return pEntry;
}

VOID
MeshEntryTableGet(
	IN PRTMP_ADAPTER	pAd)
{
	INT i;
	MESH_ENTRY	*pEntry = NULL;
	PMESH_ENTRY_TABLE pEntryTab = pAd->MeshTab.pMeshEntryTab;

	if (pEntryTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Entry Table doesn't exist.\n", __FUNCTION__));
		return;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshEntryTabLock);

	for (i=0; i < HASH_TABLE_SIZE; i++)
	{
		pEntry = pEntryTab->Hash[i];
		while(pEntry != NULL)
		{
			DBGPRINT(RT_DEBUG_OFF, ("%02X:%02X:%02X:%02X:%02X:%02X  ",
				pEntry->DestAddr[0], pEntry->DestAddr[1], pEntry->DestAddr[2],
				pEntry->DestAddr[3], pEntry->DestAddr[4], pEntry->DestAddr[5]));
			DBGPRINT(RT_DEBUG_OFF, ("%-10d\n", (int)pEntry->Idx));
			pEntry = pEntry->pNext;
		}
	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshEntryTabLock);
}

NDIS_STATUS MeshProxyEntryTable_Init(
			IN PRTMP_ADAPTER	pAd)
{

	NdisAllocateSpinLock(pAd, &pAd->MeshTab.MeshProxyTabLock);

	pAd->MeshTab.pMeshProxyTab = vmalloc(sizeof(MESH_PROXY_ENTRY_TABLE));

	if (pAd->MeshTab.pMeshProxyTab)
	{
		PMESH_PROXY_ENTRY_TABLE	pMeshProxyTab=pAd->MeshTab.pMeshProxyTab;
		int i;

		NdisZeroMemory(pAd->MeshTab.pMeshProxyTab, sizeof(MESH_PROXY_ENTRY_TABLE));
		
		pMeshProxyTab->AgeList.Head=-1;
		pMeshProxyTab->AgeList.Tail=-1;
		pMeshProxyTab->AgeList.Number=0;

		pMeshProxyTab->FreeEntryList.Head=-1;
		pMeshProxyTab->FreeEntryList.Tail=-1;
		pMeshProxyTab->FreeEntryList.Number=0;
	
		pMeshProxyTab->Size=0;
		for (i=0;i<MAX_HASH_ENTRY_TAB_SIZE;i++)
			pMeshProxyTab->Hash[i]=-1;

		for (i=0;i<MAX_HASH_PROXY_ENTRY_TAB_SIZE;i++)
		{
			PMESH_PROXY_ENTRY pProxyEnrtry=&pMeshProxyTab->Content[i];	
			pProxyEnrtry->DLNext=-1;
			pProxyEnrtry->DLPrev=-1;
			pProxyEnrtry->Next=-1;
			SignalLinkListInsertHeadQueue(pAd->MeshTab.pMeshProxyTab->Content, &pAd->MeshTab.pMeshProxyTab->FreeEntryList, i);
		}
		
		
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->MeshTab.pMeshProxyTab", __FUNCTION__));

	return TRUE;
}

NDIS_STATUS MeshProxyEntryTable_Exit(
			IN PRTMP_ADAPTER	pAd)
{

	if (pAd->MeshTab.pMeshProxyTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Proxy Table doesn't exist.\n", __FUNCTION__));
		return TRUE;
	}

	if (pAd->MeshTab.pMeshProxyTab)
		{
		vfree(pAd->MeshTab.pMeshProxyTab);
		pAd->MeshTab.pMeshProxyTab = NULL;
		return TRUE;
	}

	NdisFreeSpinLock(&pAd->MeshTab.MeshProxyTabLock);

	return TRUE;
}

PMESH_PROXY_ENTRY MeshProxyEntryTableLookUp(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR			pSA)
{
	UINT		HashIdx;
	SHORT	EntryIndex;
	PMESH_PROXY_ENTRY	pEntry = NULL;
	PMESH_PROXY_ENTRY_TABLE	pProxyTab = pAd->MeshTab.pMeshProxyTab;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshProxyEntryTableLookUp\n"));
#endif /* RELEASE_EXCLUDE */

	if (pProxyTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Proxy Table doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	if ( !(pAd->MeshTab.OpMode & MESH_AP) &&  !(pAd->MeshTab.OpMode & MESH_POTAL))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Proxy does not enable!\n", __FUNCTION__));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshProxyTabLock);

	/*find entry first */
	HashIdx = MAC_ADDR_HASH_INDEX(pSA);
	EntryIndex=pProxyTab->Hash[HashIdx];

	while (EntryIndex!=-1)
	{
		if (MAC_ADDR_EQUAL(pProxyTab->Content[EntryIndex].MacAddr, pSA))
	{
			pEntry=&pProxyTab->Content[EntryIndex];
			break;
		}

		EntryIndex=pProxyTab->Content[EntryIndex].Next;
	}

	/*refresh age time */
	if (EntryIndex!=-1)
	{	
		DoubleLinkListRemoveNode(pProxyTab->Content, &pProxyTab->AgeList, EntryIndex);
		DoubleLinkListInsertTailQueue(pProxyTab->Content, &pProxyTab->AgeList, EntryIndex);
	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshProxyTabLock);
	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshProxyEntryTableLookUp\n"));
#endif /* RELEASE_EXCLUDE */

	return pEntry;
}

PMESH_PROXY_ENTRY MeshProxyEntryTableInsert(
	IN PRTMP_ADAPTER	pAd,
	IN  PUCHAR			pMeshSA,
	IN PUCHAR			pSA)
{
	ULONG	Now;
	UINT	HashIdx;
	SHORT	NewEntryIndex =-1, CurrEntryIndex=-1;
	MESH_PROXY_ENTRY	*pNewEntry =NULL, *pCurrEntry;
	PMESH_PROXY_ENTRY_TABLE	pProxyTab = pAd->MeshTab.pMeshProxyTab;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshProxyEntryTableInsert\n"));
#endif /* RELEASE_EXCLUDE */

	if (pProxyTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Proxy Table doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	if ( !(pAd->MeshTab.OpMode & MESH_AP) &&  !(pAd->MeshTab.OpMode & MESH_POTAL))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Proxy does not enable!\n", __FUNCTION__));
		return NULL;
	}

	if (GetMeshLinkId(pAd, (PCHAR)pSA) != BSS_NOT_FOUND)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: The address are neighbor peer mesh link .\n", __FUNCTION__));
		return NULL;
	}

	if (MAC_ADDR_EQUAL(ZERO_MAC_ADDR, pSA))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Receive Zero MAC Address .\n", __FUNCTION__));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshProxyTabLock);

	/* get free entry */
	/* if the table is full , we use the oldest entry */
	NewEntryIndex=SignalLinkListRemoveFromHeadQueue(pProxyTab->Content, &pProxyTab->FreeEntryList);
	
	if (NewEntryIndex==-1)
	{
		SHORT PrevEntryIndex=-1;
		NewEntryIndex=DoubleLinkListRemoveHeadQueue(pProxyTab->Content, &pProxyTab->AgeList);
		
		if (NewEntryIndex==-1)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Bug when remove doublelinkheadqueue .\n", __FUNCTION__));
			RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshProxyTabLock);
			return NULL;
		}

		/*remove from hash list */
		HashIdx=MAC_ADDR_HASH_INDEX(pProxyTab->Content[NewEntryIndex].MacAddr);	
		CurrEntryIndex=pProxyTab->Hash[HashIdx];

		RTMP_GetCurrentSystemTick(&Now);
		if (RTMP_TIME_AFTER(Now,pProxyTab->Content[NewEntryIndex].LastUpdateTime+MESH_PROXY_AGEOUT))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Proxy table is full , Age Out Proxy Entry: Index:%d HashIdx:%d\n",NewEntryIndex,HashIdx));	
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Age Out Expired Proxy Entry: Index:%d HashIdx:%d\n",NewEntryIndex,HashIdx));	
		}

		while(CurrEntryIndex!=-1 && CurrEntryIndex!=NewEntryIndex)
		{
			PrevEntryIndex=CurrEntryIndex;
			CurrEntryIndex=pProxyTab->Content[CurrEntryIndex].Next;
		}

		if (CurrEntryIndex==NewEntryIndex)
		{
			if (PrevEntryIndex==-1)
				pProxyTab->Hash[HashIdx]=pProxyTab->Content[NewEntryIndex].Next;
			else
				pProxyTab->Content[PrevEntryIndex].Next=pProxyTab->Content[NewEntryIndex].Next;
		}
		else
	{
			DBGPRINT(RT_DEBUG_ERROR, ("BUG in MeshProxyEntry Table %d \n",CurrEntryIndex));
			Set_MeshProxyInfo_Display_Proc(pAd, "");
			RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshProxyTabLock);
			return NULL;		
		}
		/* remove from hash list end */
	}

	pNewEntry=&pProxyTab->Content[NewEntryIndex];
	pNewEntry->DLNext=-1;
	pNewEntry->DLPrev=-1;
	pNewEntry->Next=-1;
	pNewEntry->isMesh=FALSE;
	pNewEntry->isProxied=FALSE;

	RTMP_GetCurrentSystemTick(&Now);
	pNewEntry->LastUpdateTime=Now;

	HashIdx = MAC_ADDR_HASH_INDEX(pSA);
	COPY_MAC_ADDR(pNewEntry->MacAddr, pSA);
	DBGPRINT(RT_DEBUG_TRACE, ("Proxy Entry = (%02x:%02x:%02x:%02x:%02x:%02x) !!!\n",
							pNewEntry->MacAddr[0], pNewEntry->MacAddr[1], pNewEntry->MacAddr[2],
							pNewEntry->MacAddr[3], pNewEntry->MacAddr[4], pNewEntry->MacAddr[5]));
	if (MacTableLookup(pAd, pSA))
		pNewEntry->isMesh = TRUE;
	else
		pNewEntry->isMesh = FALSE;
	pNewEntry->isProxied = TRUE;
	COPY_MAC_ADDR(pNewEntry->Owner, pMeshSA);

	/*insert to hash list */
	if (pProxyTab->Hash[HashIdx] == -1)
	{	/* Hash list is empty, directly assign it. */
		pProxyTab->Hash[HashIdx] = NewEntryIndex;
		pNewEntry->Next=-1;
	}
	else 
	{
		/* Ok, we insert the new entry into the Hash[HashIdx] */
		CurrEntryIndex=pProxyTab->Hash[HashIdx];
		pCurrEntry=&pProxyTab->Content[CurrEntryIndex];

		pProxyTab->Hash[HashIdx]=NewEntryIndex;
		pNewEntry->Next=CurrEntryIndex;
	}
	/*insert to hash list end */

	/*insert to age out list */
	DoubleLinkListInsertTailQueue(pProxyTab->Content, &pProxyTab->AgeList, NewEntryIndex);

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshProxyTabLock);
	return pNewEntry;
}

BOOLEAN MeshProxyEntryTableDelete(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR			pSA)
{
	UINT	HashIdx;
	SHORT	EntryIndex,CurrEntryIndex,PrevEntryIndex=-1;
	PMESH_PROXY_ENTRY	pEntry = NULL;
	PMESH_PROXY_ENTRY_TABLE	pProxyTab = pAd->MeshTab.pMeshProxyTab;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshProxyEntryTableDelete\n"));
#endif /* RELEASE_EXCLUDE */

	if (pProxyTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Proxy Table doesn't exist.\n", __FUNCTION__));
		return FALSE;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshProxyTabLock);

	/*find the entry first */
	HashIdx = MAC_ADDR_HASH_INDEX(pSA);
	EntryIndex=pProxyTab->Hash[HashIdx];

	while (EntryIndex!=-1)
	{
		if (MAC_ADDR_EQUAL(pProxyTab->Content[EntryIndex].MacAddr, pSA))
	{
			pEntry=&pProxyTab->Content[EntryIndex];
			break;		
		}
		EntryIndex=pProxyTab->Content[EntryIndex].Next;
	}

	if (EntryIndex==-1)
	{
		RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshProxyTabLock);
		return FALSE;
	}

	/*remove from hash list */
	CurrEntryIndex=pProxyTab->Hash[HashIdx];

	while(CurrEntryIndex!=-1 && CurrEntryIndex!=EntryIndex)
	{
		PrevEntryIndex=CurrEntryIndex;
		CurrEntryIndex=pProxyTab->Content[CurrEntryIndex].Next;
	}

	if (CurrEntryIndex==EntryIndex)
	{
		if (PrevEntryIndex==-1)
			pProxyTab->Hash[HashIdx]=pProxyTab->Content[EntryIndex].Next;
			else
			pProxyTab->Content[PrevEntryIndex].Next=pProxyTab->Content[EntryIndex].Next;
		}
		else
		{
		DBGPRINT(RT_DEBUG_ERROR, ("BUG in MeshProxyEntry Table %d \n",CurrEntryIndex));
		Set_MeshProxyInfo_Display_Proc(pAd, "");
		RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshProxyTabLock);
		return FALSE;		
	}

	DoubleLinkListRemoveNode(pProxyTab->Content, &pProxyTab->AgeList, EntryIndex);	
	SignalLinkListInsertHeadQueue(pProxyTab->Content, &pProxyTab->FreeEntryList, EntryIndex);

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshProxyTabLock);

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshProxyEntryTableDelete\n"));
#endif /* RELEASE_EXCLUDE */

	return TRUE;
}

PMESH_PROXY_ENTRY MeshProxyEntryTableUpdate(
	IN PRTMP_ADAPTER	pAd,
	IN PUCHAR			pMeshSA,
	IN PUCHAR			pSA)
{
	UINT	HashIdx;
	SHORT EntryIndex;
	MESH_PROXY_ENTRY	*pEntry = NULL;
	PMESH_PROXY_ENTRY_TABLE	pProxyTab = pAd->MeshTab.pMeshProxyTab;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshProxyEntryTableUpdate\n"));
#endif /* RELEASE_EXCLUDE */

	if (pProxyTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Proxy Table doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	if (MAC_ADDR_EQUAL(ZERO_MAC_ADDR, pSA))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Receive Zero MAC Address .\n", __FUNCTION__));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshProxyTabLock);

	HashIdx = MAC_ADDR_HASH_INDEX(pSA);
	EntryIndex=pProxyTab->Hash[HashIdx];

	while (EntryIndex!=-1)
	{
		if (MAC_ADDR_EQUAL(pProxyTab->Content[EntryIndex].MacAddr, pSA))
    	{
			pEntry=&pProxyTab->Content[EntryIndex];
    		break;
    	}

		EntryIndex=pProxyTab->Content[EntryIndex].Next;
	}

	if (EntryIndex!=-1)
    	{
		COPY_MAC_ADDR(pProxyTab->Content[EntryIndex].Owner, pMeshSA);
		DoubleLinkListRemoveNode(pProxyTab->Content, &pProxyTab->AgeList, EntryIndex);
		DoubleLinkListInsertTailQueue(pProxyTab->Content, &pProxyTab->AgeList, EntryIndex);
	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshProxyTabLock);

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshProxyEntryTableUpdate\n"));
#endif /* RELEASE_EXCLUDE */

	return pEntry;
}

NDIS_STATUS MeshRoutingTable_Init(
			IN PRTMP_ADAPTER	pAd)
{
	NdisAllocateSpinLock(pAd, &pAd->MeshTab.MeshRouteTabLock);

	pAd->MeshTab.pMeshRouteTab = vmalloc(sizeof(MESH_ROUTING_TABLE));

	if (pAd->MeshTab.pMeshRouteTab)
		NdisZeroMemory(pAd->MeshTab.pMeshRouteTab, sizeof(MESH_ROUTING_TABLE));
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->MeshTab.pMeshRouteTab", __FUNCTION__));

	return TRUE;
}

NDIS_STATUS MeshRoutingTable_Exit(
			IN PRTMP_ADAPTER	pAd)
{
	NdisFreeSpinLock(&pAd->MeshTab.MeshRouteTabLock);

	if (pAd->MeshTab.pMeshRouteTab)
		vfree(pAd->MeshTab.pMeshRouteTab);
	pAd->MeshTab.pMeshRouteTab = NULL;

	return TRUE;
}

PMESH_ROUTING_ENTRY MeshRoutingTableLookup(
	IN  PRTMP_ADAPTER		pAd, 
	IN  PUCHAR			MeshDA)
{
	ULONG HashIdx;
	PMESH_ROUTING_ENTRY pEntry = NULL;
	PMESH_ROUTING_TABLE	pRouteTab = pAd->MeshTab.pMeshRouteTab;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshRoutingTableLookup\n"));

	if (pRouteTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Route Table doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshRouteTabLock);

	HashIdx = MAC_ADDR_HASH_INDEX(MeshDA);
	pEntry = pRouteTab->Hash[HashIdx];

	while (pEntry)
	{
		if (MAC_ADDR_EQUAL(pEntry->MeshDA, MeshDA)) 
		{
			break;
		}
		else
			pEntry = pEntry->pNext;
	}
	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshRouteTabLock);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshRoutingTableLookup\n"));
	return pEntry;
}

PMESH_ROUTING_ENTRY MeshRoutingTableInsert(
    IN  PRTMP_ADAPTER	pAd,
    IN	PUCHAR			MeshDestAddr,
    IN	UINT32			Dsn,
    IN	PUCHAR			NextHop,
    IN	UCHAR			NextHopLinkID,
    IN	UINT32			Metric)
{
	UCHAR i, HashIdx;
	MESH_ROUTING_ENTRY *pEntry = NULL, *pCurrEntry;
	PMESH_ROUTING_TABLE	pRouteTab = pAd->MeshTab.pMeshRouteTab;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshRoutingTableInsert\n"));

	if (pRouteTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Route Table doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	/* if FULL, return */
	if (pRouteTab->Size >= MAX_ROUTE_TAB_SIZE) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Route Table size > %d.\n", __FUNCTION__, MAX_ROUTE_TAB_SIZE));
		return NULL;
	}

	if((pEntry = MeshRoutingTableLookup(pAd, MeshDestAddr)) != NULL)
		return pEntry;

	/* allocate one Mesh entry */
	RTMP_SEM_LOCK(&pAd->MeshTab.MeshRouteTabLock);

	for (i = 0; i< MAX_ROUTE_TAB_SIZE; i++)
	{
		if (pRouteTab->Content[i].Valid == FALSE)
		{
			pEntry = &pRouteTab->Content[i];
			NdisZeroMemory(pEntry, sizeof(MESH_ROUTING_ENTRY));
			pEntry->Valid = TRUE;
			COPY_MAC_ADDR(pEntry->MeshDA, MeshDestAddr);
			pEntry->Dsn = Dsn;
			COPY_MAC_ADDR(pEntry->NextHop, NextHop);
			pEntry->NextHopLinkID = NextHopLinkID;
			pEntry->PathMetric = Metric;
			pEntry->LifeTime = HWMP_FORWARD_TABLE_LIFE_TIME;
			pEntry->Idx = i;
			pRouteTab->Size ++;
			break;
		}		
	}

	/* add this MAC entry into HASH table */
	if (pEntry)
	{
		HashIdx = MAC_ADDR_HASH_INDEX(MeshDestAddr);
		if (pRouteTab->Hash[HashIdx] == NULL)
		{
			pRouteTab->Hash[HashIdx] = pEntry;
		}
		else
		{
			pCurrEntry = pRouteTab->Hash[HashIdx];
			while (pCurrEntry->pNext != NULL)
				pCurrEntry = pCurrEntry->pNext;
			pCurrEntry->pNext = pEntry;
		}

	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshRouteTabLock);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshRoutingTableInsert\n"));
	return pEntry;
}

BOOLEAN MeshRoutingTableDelete(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR MeshDA)
{
	USHORT HashIdx, i;
	MESH_ROUTING_ENTRY *pEntry = NULL, *pPrevEntry, *pProbeEntry;
	MESH_ENTRY *pMeshPrevEntry, *pMeshProbeEntry;
	PMESH_ROUTING_TABLE	pRouteTab = pAd->MeshTab.pMeshRouteTab;
	PMESH_ENTRY_TABLE pEntryTab = pAd->MeshTab.pMeshEntryTab;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshRoutingTableDelete\n"));
#endif /* RELEASE_EXCLUDE */

	if (pRouteTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Route Table doesn't exist.\n", __FUNCTION__));
		return FALSE;
	}

	if (pEntryTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Entry Table doesn't exist.\n", __FUNCTION__));
		return FALSE;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshRouteTabLock);

	for (i = 0; i< MAX_ROUTE_TAB_SIZE; i++)
	{
		pEntry = &pRouteTab->Content[i];

		if (pEntry->Valid == TRUE)
		{
			if (MAC_ADDR_EQUAL(pEntry->MeshDA, MeshDA))
				break;
		}
	}

	if (i == MAX_ROUTE_TAB_SIZE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: The Mesh Route  Entry doesn't exist.\n", __FUNCTION__));
		RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshRouteTabLock);
		return FALSE;
	}

	HashIdx = MAC_ADDR_HASH_INDEX(MeshDA);

	pPrevEntry = NULL;
	pProbeEntry = pRouteTab->Hash[HashIdx];

	ASSERT(pProbeEntry);

	/* update Hash list */
	do
	{
		if (pProbeEntry == pEntry)
		{
			if (pPrevEntry == NULL)
			{
				pRouteTab->Hash[HashIdx] = pEntry->pNext;
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

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshEntryTabLock);
	/* delete entry table mapping to the route idx */
	for (i=0; i < MAX_HASH_ENTRY_TAB_SIZE; i++)
	{
		if (pEntryTab->Hash[i] != NULL)
		{
			pMeshPrevEntry = NULL;
			pMeshProbeEntry = pEntryTab->Hash[i];

			do
			{
				if (pEntry->Idx == pMeshProbeEntry->Idx)
				{
					if (pMeshProbeEntry->PathReq)
					{
						BOOLEAN	Cancelled;

						RTMPCancelTimer(&pMeshProbeEntry->PathReq->PathReqTimer, &Cancelled);

						os_free_mem(NULL, pMeshProbeEntry->PathReq);
						pMeshProbeEntry->PathReq = NULL;
					}
					pMeshProbeEntry->PathReqTimerRunning = FALSE;
					
					if (pMeshPrevEntry == NULL)
						pEntryTab->Hash[i] = pMeshProbeEntry->pNext;
					else
						pMeshPrevEntry->pNext = pMeshProbeEntry->pNext;
					
					os_free_mem(NULL, pMeshProbeEntry);
					pEntryTab->Size--;
					break;
				}

				pMeshPrevEntry = pMeshProbeEntry;
				pMeshProbeEntry = pMeshProbeEntry->pNext;
			} while (pMeshProbeEntry);
		}
	}
	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshEntryTabLock);

	NdisZeroMemory(pEntry, sizeof(MESH_ROUTING_ENTRY));
	pRouteTab->Size--;

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshRouteTabLock);

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshRoutingTableDelete\n"));
#endif /* RELEASE_EXCLUDE */

	return TRUE;
}

PMESH_ROUTING_ENTRY MeshRoutingTableUpdate(
	IN  PRTMP_ADAPTER		pAd,
	IN	PUCHAR			MeshDestAddr,
	IN	UINT32			Dsn,
	IN	PUCHAR			NextHop,
	IN	UCHAR			NextHopLinkID,
	IN	UINT32			Metric)
{
	MESH_ROUTING_ENTRY *pEntry = NULL;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshRoutingTableUpdate\n"));
#endif /* RELEASE_EXCLUDE */

	if((pEntry = MeshRoutingTableLookup(pAd, MeshDestAddr)) == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: The Mesh Route  Entry doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshRouteTabLock);

	pEntry->Valid = TRUE;
	COPY_MAC_ADDR(pEntry->MeshDA, MeshDestAddr);
	pEntry->Dsn = Dsn;
	COPY_MAC_ADDR(pEntry->NextHop, NextHop);
	pEntry->NextHopLinkID = NextHopLinkID;
	pEntry->PathMetric = Metric;
	pEntry->LifeTime = HWMP_FORWARD_TABLE_LIFE_TIME;

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshRouteTabLock);

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshRoutingTableUpdate\n"));
#endif /* RELEASE_EXCLUDE */

	return pEntry;
}

PMESH_ROUTING_ENTRY MeshRoutingTablePrecursorUpdate(
	IN  PRTMP_ADAPTER		pAd,
	IN	PUCHAR			MeshDestAddr,
	IN	PUCHAR			Precursor)
{
	MESH_ROUTING_ENTRY *pEntry = NULL;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> MeshRoutingTablePrecursorUpdate\n"));
#endif /* RELEASE_EXCLUDE */
	if((pEntry = MeshRoutingTableLookup(pAd, MeshDestAddr)) == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Can't find the route entry !\n"));
		return NULL;
	}

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshRouteTabLock);

	if (pEntry->Valid == TRUE)
	{
		COPY_MAC_ADDR(pEntry->Precursor, Precursor);
		pEntry->bPrecursor = TRUE;
	}

	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshRouteTabLock);

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- MeshRoutingTablePrecursorUpdate\n"));
#endif /* RELEASE_EXCLUDE */
	return pEntry;
}

#endif /* MESH_SUPPORT */

