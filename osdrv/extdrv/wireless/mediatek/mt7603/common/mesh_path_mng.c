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
#include "mesh_sanity.h"


VOID
MeshPreqRcvProcess(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	PHEADER_802_11 pHeader = (PHEADER_802_11)pRxBlk->pHeader;
	UINT8	MeshTTL = 0;
	UINT32	MeshSeq = 0;
	UCHAR	Ie, IeLen;
	UINT8	PreqFlag;
	UCHAR	PreqHopCount, PreqTTL;
	UINT32	PreqID;
	UCHAR	PreqOrigAddr[MAC_ADDR_LEN];
	UINT32	PreqOrigDsn;
	UCHAR	PreqProxyAddr[MAC_ADDR_LEN];
	PUCHAR	pPreqProxyAddr = NULL;
	UINT32	PreqLifeTime;
	UINT32	Metric;
	UCHAR	PreqDestCount;
	MESH_DEST_ENTRY PreqDestEntry;
	UCHAR	VarOffset;
	MESH_ROUTING_ENTRY *pRouteEntry;
	PMAC_TABLE_ENTRY pMacEntry = NULL;
	PMESH_ENTRY pMeshEntry = NULL;
	BOOLEAN bForward = TRUE;
	PUCHAR pFrame;
	ULONG FrameLen;
	PUCHAR pMeshHdr = (PUCHAR)(pRxBlk->pData);
	UINT MeshHdrLen;
	struct wifi_dev *wdev;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPreqRcvProcess\n"));

	if (GetMeshLinkId(pAd, (PCHAR)pHeader->Addr2) == BSS_NOT_FOUND)
		return;

	wdev = &pAd->MeshTab.wdev;
#ifdef RELEASE_EXCLUDE
	/*case 1. (destination count = 1, no PREP generation) */
	/*			All of the following applies: */
	/*			1.the MP has received and accepted a PREQ */
	/*			2.Destination_count = 1 */
	/*			3.the MP is not the destination of the PREQ OR the destination of the PREQ is the MAC broadcast address */
	/*			4.the MP is not the proxy of the destination address */
	/*			5.the MP has no valid forwarding information for the requested destination */
	/*			6.[Destination Only flag of the destination in the PREQ is ON (DO = 1)] OR */
	/*			  [{Destination Only flag of the destination in the PREQ is OFF (DO = 0)} AND {MP has no */
	/*			  active forwarding information for the requested destination preq.destination_address}] */
#endif /* RELEASE_EXCLUDE */

	MeshTTL = GetMeshTTL(pMeshHdr);
	MeshSeq = GetMeshSeq(pMeshHdr);

	if (--MeshTTL <= 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Mesh Time to Live is zero !\n"));
		return;
	}

	MeshHdrLen = GetMeshHederLen(pRxBlk->pData);
	/* skip Mesh Header */
	pRxBlk->pData += MeshHdrLen;
	pRxBlk->DataSize -= MeshHdrLen;

	/* skip Category and ActionCode */
	pFrame = (PUCHAR)(pRxBlk->pData + 2);
	FrameLen = pRxBlk->DataSize - 2;

	VarOffset = 0;
	/* Get Path Select IE */
	NdisMoveMemory(&Ie, pFrame + VarOffset, 1);
	VarOffset += 1;
	if (Ie != IE_MESH_PREQ)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Receive frame packet Element IE wrong. The IE are not Path Request!!!\n"));
		return;
	}

	/* Get IE length */
	NdisMoveMemory(&IeLen, pFrame + VarOffset, 1);
	VarOffset += 1;

	if (IeLen < 37)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Receive frame packet length wrong, the length must > 37!!!\n"));
		return;
	}

	/* Get Path Request Flags */
	NdisMoveMemory(&PreqFlag, pFrame + VarOffset, 1);
	VarOffset += 1;
	
	/* Get HopCount */
	NdisMoveMemory(&PreqHopCount, pFrame + VarOffset, 1);
	VarOffset += 1;

	/* Get Path Request TTL */
	NdisMoveMemory(&PreqTTL, pFrame + VarOffset, 1);
	VarOffset += 1;

	/* Get Path Request ID */
	NdisMoveMemory(&PreqID, pFrame + VarOffset, 4);
	VarOffset += 4;
	PreqID = le2cpu32(PreqID);

	/* Get Originator Address */
	NdisMoveMemory(&PreqOrigAddr, pFrame + VarOffset, 6);
	VarOffset += 6;

	/* Check the MP whether it's Originator of PREQ. */
	if (MAC_ADDR_EQUAL(wdev->if_addr, PreqOrigAddr))
		return;

	/* Get Originator Dsn */
	NdisMoveMemory(&PreqOrigDsn, pFrame + VarOffset, 4);
	VarOffset += 4;
	PreqOrigDsn = le2cpu32(PreqOrigDsn);

	if (((PMESH_PREQ_FLAG)&PreqFlag)->field.AE == 1)
	{
		/* Get Proxy Address */
		NdisMoveMemory(&PreqProxyAddr, pFrame + VarOffset, 6);
		pPreqProxyAddr = PreqProxyAddr;
		VarOffset += 6;
	}

	/* Get Life Time */
	NdisMoveMemory(&PreqLifeTime, pFrame + VarOffset, 4);
	VarOffset += 4;
	PreqLifeTime = le2cpu32(PreqLifeTime);

	/* Get Metric */
	NdisMoveMemory(&Metric, pFrame + VarOffset, 4);
	VarOffset += 4;
	Metric = le2cpu32(Metric);

	/* Get DestCount */
	NdisMoveMemory(&PreqDestCount, pFrame + VarOffset, 1);
	VarOffset += 1;

	/* Get MeshDestEntry */
	NdisMoveMemory(&PreqDestEntry, pFrame + VarOffset, 11);

	if (IeLen < (26 + (PreqDestCount * 11)))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Receive frame packet format wrong!!!\n"));
		return;
	}

	/* create or update Originator route path on our route table */
	pMacEntry = MeshTableLookup(pAd, pHeader->Addr2, TRUE); 

	/* check Transmission MP already exist on our LinkMP table. */
	if (pMacEntry == NULL)
		return;

#ifdef RELEASE_EXCLUDE
	/* 11A.8.5.3.1 Acceptance criteria */
	/* The PREQ is discarded if any of the following is true: */
	/*	1. The Originator DSN < previous Originator DSN */
	/*	2. (DSN = previous DSN) AND (updated path metric is worse than previous path metric) */
	/* Otherwise, the PREQ information element is accepted. */
#endif /* RELEASE_EXCLUDE */

	pRouteEntry = MeshRoutingTableLookup(pAd, PreqOrigAddr);

	if (pRouteEntry)
	{
		/* check whether the Originator Dsn < previous Originator Dsn */
		if (pRouteEntry->Dsn > PreqOrigDsn)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("The Originator DSN=(%d) < previous Originator DSN=(%d)!!!\n",
							PreqOrigDsn, pRouteEntry->Dsn));
			bForward = FALSE;
		}

		/* (DSN = previous DSN) AND (updated path metric is worse than previous path metric) */
		if (pRouteEntry->Dsn == PreqOrigDsn)
		{
			if (pRouteEntry->PathMetric <= (Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("The Originator Metric=(%d) > previous Originator metric=(%d)!!!\n",
								Metric, pRouteEntry->PathMetric));
				bForward = FALSE;
			}
		}

		if (bForward == TRUE)
			pRouteEntry = MeshRoutingTableUpdate(pAd,
											PreqOrigAddr,
											PreqOrigDsn,
											pHeader->Addr2,
											pMacEntry->func_tb_idx,
											Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);

		if (((PMESH_PREQ_FLAG)&PreqFlag)->field.AE == 1)
		{
			pMeshEntry = MeshEntryTableLookUp(pAd, PreqProxyAddr);

			if (pMeshEntry == NULL)
				MeshEntryTableInsert(pAd,
									PreqProxyAddr,
									pRouteEntry->Idx);
			else
				MeshEntryTableUpdate(pAd,
									PreqProxyAddr,
									pRouteEntry->Idx);
		}

		pMeshEntry = MeshEntryTableLookUp(pAd, PreqOrigAddr);

		if (pMeshEntry == NULL)
			MeshEntryTableInsert(pAd,
								PreqOrigAddr,
								pRouteEntry->Idx);
		else
			MeshEntryTableUpdate(pAd,
								PreqOrigAddr,
								pRouteEntry->Idx);
	}
	else
	{
		pRouteEntry = MeshRoutingTableInsert(pAd,
										PreqOrigAddr,
										PreqOrigDsn,
										pHeader->Addr2,
										pMacEntry->func_tb_idx,
										Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);

		if (((PMESH_PREQ_FLAG)&PreqFlag)->field.AE == 1)
		{
			pMeshEntry = MeshEntryTableLookUp(pAd, PreqProxyAddr);

			if (pMeshEntry == NULL)
				MeshEntryTableInsert(pAd,
									PreqProxyAddr,
									pRouteEntry->Idx);
			else
				MeshEntryTableUpdate(pAd,
									PreqProxyAddr,
									pRouteEntry->Idx);
		}

		pMeshEntry = MeshEntryTableLookUp(pAd, PreqOrigAddr);

		if (pMeshEntry == NULL)
			MeshEntryTableInsert(pAd,
								PreqOrigAddr,
								pRouteEntry->Idx);
		else
			MeshEntryTableUpdate(pAd,
								PreqOrigAddr,
								pRouteEntry->Idx);
	}

	/* create or update neighbor MP route path on our route table */
	if (!MAC_ADDR_EQUAL(PreqOrigAddr, pHeader->Addr2))
	{
		pMeshEntry = MeshEntryTableLookUp(pAd, pHeader->Addr2);
		pRouteEntry = MeshRoutingTableLookup(pAd, pHeader->Addr2);

		if (pMeshEntry && pRouteEntry)
		{
			/* update neighbor MP route information */
			MeshRoutingTableUpdate(pAd,
								pHeader->Addr2,
								0,
								pHeader->Addr2,
								pMacEntry->func_tb_idx,
								pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);
			MeshEntryTableUpdate(pAd,
								pHeader->Addr2,
								pRouteEntry->Idx);
		}
		else
		{
			if (pRouteEntry == NULL)
				pRouteEntry = MeshRoutingTableInsert(pAd,
												pHeader->Addr2,
												0,
												pHeader->Addr2,
												pMacEntry->func_tb_idx,
												pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);
			else
				pRouteEntry = MeshRoutingTableUpdate(pAd,
												pHeader->Addr2,
												0,
												pHeader->Addr2,
												pMacEntry->func_tb_idx,
												pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);

			if (pMeshEntry == NULL)
				MeshEntryTableInsert(pAd,
									pHeader->Addr2,
									pRouteEntry->Idx);
			else
				MeshEntryTableUpdate(pAd,
									pHeader->Addr2,
									pRouteEntry->Idx);
		}
	}

#ifdef RELEASE_EXCLUDE
	/* A PREP is transmitted if the MP has received a PREQ fulfilling all of the following conditions: */
	/*	a.One of the following applies: */
	/*		1.The Destination Address of the PREQ is the same as MAC address of the receiving MP */
	/*		2.The Destination Address of the PREQ = all 1's (broadcast) and the PREP flag is set to 1 ("Proactive PREP") */
	/*		3.The Destination Address of the PREQ is currently proxied by the MP */
	/*	b.One of the following applies: */
	/*		1.The Originator DSN of the PREQ (preq.orig_dsn) is greater than the DSN of the last */
	/*			PREQ received from the same originator address (that includes the case that there is */
	/*			no path to the originating MP) */
	/*		2.The Metric is better than the path selection metric currently associated with the */
	/*			Originator Address and the Originator DSN of the PREQ (preq.orig_dsn) is equal to */
	/*			the DSN of the last PREQ received from the same originator address */
#endif /* RELEASE_EXCLUDE */
				
	if (MeshAllowToSendPathResponse(pAd, PreqDestEntry.DestAddr))
	{
		pRouteEntry = MeshRoutingTableLookup(pAd, PreqOrigAddr);

		if (pRouteEntry)
		if ((pRouteEntry->Dsn <= PreqOrigDsn) ||
			(pRouteEntry->PathMetric >= (Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics)))
		{
			if (MAC_ADDR_EQUAL(wdev->if_addr, PreqDestEntry.DestAddr))
			{
				MeshPrepCreate(pAd, pRouteEntry->NextHop, pRouteEntry->MeshDA, pRouteEntry->Dsn, NULL);
			}
			else
			{
				MeshPrepCreate(pAd, pRouteEntry->NextHop, pRouteEntry->MeshDA, pRouteEntry->Dsn, PreqDestEntry.DestAddr);
			}
		}

		bForward = FALSE;
	}

	if (bForward == TRUE)
	{
		if (pAd->MeshTab.MeshCapability.field.Forwarding == 0)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Mesh Forwarding bit disable !!!\n"));
			goto out;
		}

		if (PreqTTL <= 0)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("MeshPreqRcvProcess: PreqTTL = 0 !!!\n"));
		}
		else
		{
			MeshPreqForward(pAd,
							MeshTTL,
							MeshSeq,
							PreqFlag,
							PreqHopCount,
							PreqTTL,
							PreqID,
							PreqOrigAddr,
							PreqOrigDsn,
							pPreqProxyAddr,
							PreqLifeTime,
							(Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics),
							PreqDestCount,
							PreqDestEntry,
							pHeader->Addr2);
		}
	}

out:

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPreqRcvProcess\n"));

	return;
}

VOID
MeshPreqCreate(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pProxyAddr,
	IN PUCHAR pDestAddr,
	IN ULONG DestDsn)
{
	HEADER_802_11 MeshHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen;
	MESH_FLAG MeshFlag;
	MESH_PREQ_FLAG MeshPreqFlag;
	MESH_DEST_ENTRY MeshDestEntry;
	UINT32 MeshHeaderSeq;
	UINT32 MeshPreqID;
	ULONG OrigDsn;
	UCHAR i = 0;
	struct wifi_dev *wdev;


	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPreqCreate\n"));

	wdev = &pAd->MeshTab.wdev;
	MeshHeaderSeq = INC_MESH_SEQ(pAd->MeshTab.MeshSeq);
	MeshPreqID = pAd->MeshTab.MeshPreqID++;
	OrigDsn = pAd->MeshTab.MeshOriginatorDsn++;

	for(i = 0; i < MAX_MESH_LINKS; i++)
	{
		if (PeerLinkValidCheck(pAd, i) == TRUE)
		{
			NStatus = MlmeAllocateMemory(pAd, (PVOID)&pOutBuffer);  /*Get an unused nonpaged memory */
			if(NStatus != NDIS_STATUS_SUCCESS) 
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Mesh - %s() allocate memory failed \n", __FUNCTION__));
				return;
			}

			/* send metrics report frame to (DestAddrs). */
			EnqueuePeerLinkMetricReport(pAd,
										pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr,
										MESH_LinkMetricUpdate(pAd, (PCHAR)pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr));

			MeshHeaderInit(pAd, &MeshHdr,
							pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr,	/*Addr1 */
							wdev->if_addr,				/*Addr2 */
							ZERO_MAC_ADDR);						/*Addr3 */

			NdisMoveMemory(pOutBuffer, (PCHAR)&MeshHdr, sizeof(HEADER_802_11));
			FrameLen = sizeof(HEADER_802_11);

			/* Mesh Header */
			MeshFlag.word = 0;
			MeshFlag.field.AE = 0;
			InsertMeshHeader(pAd,
							(pOutBuffer + FrameLen),
							&FrameLen, MeshFlag.word,
							pAd->MeshTab.TTL,
							MeshHeaderSeq,
							NULL,
							NULL,
							NULL);

			/* Action field */
			InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_PATH_SELECTION, ACT_CODE_PATH_REQUEST);

#ifdef RELEASE_EXCLUDE
			/*Octets:1 	 1 		1 		1 		1 	  4 	 6 			4 		 0 or 6 	4 */
			/*------------------------------------------------------------------------------------ */
			/*|Element|Length|Flags|Hopcount|Time to|PREQ|Originator|Originator|Proxied |Lifetime| */
			/*|ID	  |		 |	   |		|Live	|ID	 |Address	|Sequence  |Address	|		 | */
			/*|		  |		 |	   |		|		|	 |			|Number	   |		|		 | */
			/*------------------------------------------------------------------------------------ */
			/*	4		1				1				6			4 */
			/*------------------------------------------------------------ */
			/*|Metric|Destination|Per Destination|Destination|Destination| */
			/*|		 |Count		 |Flags			 |Address	 |SeqNum	 | */
			/*------------------------------------------------------------ */

			/* PREQ Flags field is set as follows: */
			/* Bit 0: Portal Role (0 = non-portal, 1 = portal) */
			/* Bit 1: (0 = group addressed, 1 = individually addressed) */
			/* Bit 2: Proactive PREP (0 = off, 1 = on) */
			/* Bit 3 ¡V 5: Reserved */
			/* Bit 6: Address Extension (AE) (1= (destination count ==1 && proxied device address present), 0 = otherwise) */
			/* Bit 7: Reserved */
#endif /* RELEASE_EXCLUDE */

			MeshPreqFlag.word = 0;
			MeshPreqFlag.field.PortalRole = 0;
			MeshPreqFlag.field.AddrType = 0;
			MeshPreqFlag.field.ProactivePrep = 0;

			if (pProxyAddr)
				MeshPreqFlag.field.AE = 1;
			else
				MeshPreqFlag.field.AE = 0;

#ifdef RELEASE_EXCLUDE
			/* The Hop Count field is set to the number of hops from the originator to the MP transmitting the request. */
			/* The Time to Live field is set to the maximum number of hops allowed for this element. */
			/* The PREQ ID field is set to some unique ID for this PREQ. */
			/* The Originator Address is set to the originator MAC address. */
			/* The Originator Sequence Number is set to a sequence number specific to the originator. */
			/* The Proxied Address field is the MAC address of a proxied entity (e.g. STA) in case the PREQ is generated */
			/*		because of a frame received from outside the mesh (e.g. BSS) and the proxied entity is the source of the */
			/*		frame. This field is only present if the AE flag is set to 1. */
			/* The Lifetime field is set to the time for which MPs receiving the PREQ consider the forwarding information */
			/*		to be valid. */
			/* The Metric is set to the cumulative metric from the originator to the MP transmitting the PREQ. */
			/* The Destination Count N gives the number of Destinations (N) contained in this PREQ. */
			/* Per Destination Flags are set as follows. */
			/* Bit 0: DO (Destination Only) */
			/* Bit 1: RF (Reply-and-Forward) */
			/* Bit 2-7: Reserved */
#endif /* RELEASE_EXCLUDE */
			
			MeshDestEntry.PerDestFlag = 0;
			((PMESH_PER_DEST_FLAG)&MeshDestEntry.PerDestFlag)->field.DO = 1;
			((PMESH_PER_DEST_FLAG)&MeshDestEntry.PerDestFlag)->field.RF = 1;
			COPY_MAC_ADDR(MeshDestEntry.DestAddr, pDestAddr);
			MeshDestEntry.Dsn = DestDsn;

			/* The Destination Address is the MAC address of the destination MP. */
			InsertMeshPathRequestIE(pAd,
								(pOutBuffer + FrameLen),
								&FrameLen, MeshPreqFlag.word,
								0,
								HWMP_NET_DIAMETER,
								MeshPreqID,
								wdev->if_addr,
								OrigDsn,
								pProxyAddr,
								HWMP_ACTIVE_PATH_TIMEOUT,
								0,
								1,
								MeshDestEntry);

			MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
			MlmeFreeMemory(pAd, pOutBuffer);
			pOutBuffer = NULL;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPreqCreate\n"));
}

VOID
MeshPreqForward(
	IN PRTMP_ADAPTER pAd,
	IN UINT8 MeshTTL,
	IN UINT32 MeshSeq,
	IN UINT8 MeshPreqFlag,
	IN UCHAR HopCount,
	IN UCHAR PreqTTL,
	IN ULONG ReqID,
	IN PUCHAR pOrigAddr,
	IN ULONG OrigDsn,
	IN PUCHAR OrigProxyAddr,
	IN ULONG LifeTime,
	IN ULONG Metrics,
	IN UCHAR DestCount,
	IN MESH_DEST_ENTRY MeshDestEntry,
	IN PUCHAR TransmissionAddr)
{
	HEADER_802_11 MeshHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen;
	MESH_FLAG MeshFlag;
	UCHAR CurrentHopCount, CurrentPreqTTL, i;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPreqForward\n"));

	CurrentHopCount = HopCount++;
	CurrentPreqTTL = PreqTTL--;

	for(i = 0; i < MAX_MESH_LINKS; i++)
	{
		if ((PeerLinkValidCheck(pAd, i) == TRUE) && !MAC_ADDR_EQUAL(TransmissionAddr, pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr))
		{
			NStatus = MlmeAllocateMemory(pAd, (PVOID)&pOutBuffer);  /*Get an unused nonpaged memory */
			if(NStatus != NDIS_STATUS_SUCCESS) 
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Mesh - %s() allocate memory failed \n", __FUNCTION__));
				return;
			}

			/* send metrics report frame to (PreqDestEntry->DestAddr) */
			EnqueuePeerLinkMetricReport(pAd,
										pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr,
										MESH_LinkMetricUpdate(pAd, (PCHAR)pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr));

			MeshHeaderInit(pAd, &MeshHdr,
							pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr,	/*Addr1 */
							pAd->MeshTab.wdev.if_addr,				/*Addr2 */
							ZERO_MAC_ADDR);						/*Addr3 */

			MakeOutgoingFrame(pOutBuffer,
								&FrameLen,
								sizeof(HEADER_802_11),
								&MeshHdr,
								END_OF_ARGS);

			/* Mesh Header */
			MeshFlag.word = 0;
			MeshFlag.field.AE = 0;
			InsertMeshHeader(pAd,
							(pOutBuffer + FrameLen),
							&FrameLen, MeshFlag.word,
							MeshTTL,
							MeshSeq,
							NULL,
							NULL,
							NULL);
	
			/* Action field */
			InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_PATH_SELECTION, ACT_CODE_PATH_REQUEST);

			/* Path Request IE */
			InsertMeshPathRequestIE(pAd,
									(pOutBuffer + FrameLen),
									&FrameLen,
									MeshPreqFlag,
									CurrentHopCount,
									CurrentPreqTTL,
									ReqID,
									pOrigAddr,
									OrigDsn,
									OrigProxyAddr,
									LifeTime,
									Metrics,
									1,
									MeshDestEntry);

			MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
			MlmeFreeMemory(pAd, pOutBuffer);
			pOutBuffer = NULL;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPreqForward\n"));
}

VOID
MeshPrepRcvProcess(
	IN PRTMP_ADAPTER pAd,
	IN	RX_BLK	*pRxBlk)
{
	PHEADER_802_11 pHeader = (PHEADER_802_11)pRxBlk->pHeader;
	UINT8	MeshTTL;
	UINT32	MeshSeq;
	UCHAR	Ie;
	UCHAR	IeLen;
	UINT8	PrepFlag;
	UCHAR	HopCount;
	UCHAR	PrepTTL;
	UCHAR	DestMacAddr[MAC_ADDR_LEN];
	UINT32	DestSeq;
	UCHAR	PrepProxyAddr[MAC_ADDR_LEN];
	PUCHAR	pPrepProxyAddr = NULL;
	UINT32	LifeTime;
	UINT32	Metric;
	UCHAR	OrigMacAddr[MAC_ADDR_LEN];
	UINT32	OrigSeq;
	MESH_ROUTING_ENTRY *pRouteEntry;
	PMAC_TABLE_ENTRY pMacEntry = NULL;
	BOOLEAN	bForward = TRUE;
	PMESH_ENTRY pMeshEntry = NULL;
	PUCHAR pFrame;
	ULONG FrameLen;
	UCHAR	VarOffset;
	PUCHAR pMeshHdr = (PUCHAR)(pRxBlk->pData);
	UINT MeshHdrLen;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPrepRcvProcess\n"));

	if (GetMeshLinkId(pAd, (PCHAR)pHeader->Addr2) == BSS_NOT_FOUND)
		return;

	MeshTTL = GetMeshTTL(pMeshHdr);
	MeshSeq = GetMeshSeq(pMeshHdr);

	if (--MeshTTL <= 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Mesh Time to Live is zero !\n"));
		return;
	}

	MeshHdrLen = GetMeshHederLen(pRxBlk->pData);
	/* skip Mesh Header */
	pRxBlk->pData += MeshHdrLen;
	pRxBlk->DataSize -= MeshHdrLen;

	/* skip Category and ActionCode */
	pFrame = (PUCHAR)(pRxBlk->pData + 2);
	FrameLen = pRxBlk->DataSize - 2;

	VarOffset = 0;
	/* Get Path Select IE */
	NdisMoveMemory(&Ie, pFrame + VarOffset, 1);
	if (Ie != IE_MESH_PREP)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Mesh Path Reply Element IE wrong !!!\n"));
		return;
	}
	VarOffset += 1;

	/* Get IE length */
	NdisMoveMemory(&IeLen, pFrame + VarOffset, 1);
	if (IeLen < 31)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Mesh Path Reply frame length < 33 !!!\n"));
		return;
	}
	VarOffset += 1;

	/* Get Path Response Flag */
	NdisMoveMemory(&PrepFlag, pFrame + VarOffset, 1);
	VarOffset += 1;

	/* Get HopCount */
	NdisMoveMemory(&HopCount, pFrame + VarOffset, 1);
	VarOffset += 1;

	/* Get Path Response TTL */
	NdisMoveMemory(&PrepTTL, pFrame + VarOffset, 1);
	VarOffset += 1;

	/* Get Destnation Address */
	NdisMoveMemory(&DestMacAddr, pFrame + VarOffset, 6);
	VarOffset += 6;

	/* Check the MP whether it's Originator of PREP. */
	if (MAC_ADDR_EQUAL(pAd->MeshTab.wdev.if_addr, DestMacAddr))
		return;

	/* Get Destnation Sequence Number */
	NdisMoveMemory(&DestSeq, pFrame + VarOffset, 4);
	VarOffset += 4;
	DestSeq = le2cpu32(DestSeq);

	if (((PMESH_PREP_FLAG)&PrepFlag)->field.AE == 1)
	{
		/* Get Proxied Address */
		NdisMoveMemory(&PrepProxyAddr, pFrame + VarOffset, 6);
		pPrepProxyAddr = PrepProxyAddr;
		VarOffset += 6;
	}

	/* Get LifeTime */
	NdisMoveMemory(&LifeTime, pFrame + VarOffset, 4);
	VarOffset += 4;
	LifeTime = le2cpu32(LifeTime);

	/* Get Metric */
	NdisMoveMemory(&Metric, pFrame + VarOffset, 4);
	VarOffset += 4;
	Metric = le2cpu32(Metric);

	/* Get Originator Address */
	NdisMoveMemory(&OrigMacAddr, pFrame + VarOffset, 6);
	VarOffset += 6;

	/* Get Originator Sequence Number */
	NdisMoveMemory(&OrigSeq, pFrame + VarOffset, 4);

	if((pMacEntry = MeshTableLookup(pAd, pHeader->Addr2, TRUE)) == NULL)
		return;

	pRouteEntry = MeshRoutingTableLookup(pAd, DestMacAddr);
	
	if (MAC_ADDR_EQUAL(pAd->MeshTab.wdev.if_addr, OrigMacAddr))
	{
		if (((PMESH_PREP_FLAG)&PrepFlag)->field.AE == 1)
			pMeshEntry = MeshEntryTableLookUp(pAd, PrepProxyAddr);
		else
			pMeshEntry = MeshEntryTableLookUp(pAd, DestMacAddr);

		if (pMeshEntry == NULL)
			return;

		if (pMeshEntry->PathReqTimerRunning == TRUE)
		{
			if (pMeshEntry->PathReq)
			{
				BOOLEAN	Cancelled;

				RTMPCancelTimer(&pMeshEntry->PathReq->PathReqTimer, &Cancelled);

				os_free_mem(NULL, pMeshEntry->PathReq);
				pMeshEntry->PathReq = NULL;
			}
			pMeshEntry->PathReqTimerRunning = FALSE;
		}

		if (pRouteEntry == NULL)
		{
			pRouteEntry = MeshRoutingTableInsert(pAd,
											DestMacAddr,
											DestSeq,
											pHeader->Addr2,
											pMacEntry->func_tb_idx,
											Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);
		}
		else
		{
			if (pRouteEntry->Dsn < DestSeq)
			{
				pRouteEntry = MeshRoutingTableUpdate(pAd,
												DestMacAddr,
												DestSeq,
												pHeader->Addr2,
												pMacEntry->func_tb_idx,
												Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);
			}
		}

		if (((PMESH_PREP_FLAG)&PrepFlag)->field.AE == 1)
		{
			pMeshEntry = MeshEntryTableLookUp(pAd, PrepProxyAddr);

			if (pMeshEntry == NULL)
				MeshEntryTableInsert(pAd,
									PrepProxyAddr,
									pRouteEntry->Idx);
			else
				MeshEntryTableUpdate(pAd,
									PrepProxyAddr,
									pRouteEntry->Idx);

			pMeshEntry = MeshEntryTableLookUp(pAd, DestMacAddr);

			if (pMeshEntry == NULL)
				MeshEntryTableInsert(pAd,
									DestMacAddr,
									pRouteEntry->Idx);
			else
				MeshEntryTableUpdate(pAd,
									DestMacAddr,
									pRouteEntry->Idx);
		}
		else
		{
			pMeshEntry = MeshEntryTableLookUp(pAd, DestMacAddr);

			if (pMeshEntry == NULL)
				MeshEntryTableInsert(pAd,
									DestMacAddr,
									pRouteEntry->Idx);
			else
				MeshEntryTableUpdate(pAd,
									DestMacAddr,
									pRouteEntry->Idx);
		}

		bForward = FALSE;
	}
	else
	{
		if (pRouteEntry == NULL)
		{
			pRouteEntry = MeshRoutingTableInsert(pAd,
											DestMacAddr,
											DestSeq,
											pHeader->Addr2,
											pMacEntry->func_tb_idx,
											Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);

			if (((PMESH_PREP_FLAG)&PrepFlag)->field.AE == 1)
			{
				pMeshEntry = MeshEntryTableLookUp(pAd, PrepProxyAddr);

				if (pMeshEntry == NULL)
					MeshEntryTableInsert(pAd,
										PrepProxyAddr,
										pRouteEntry->Idx);
				else
					MeshEntryTableUpdate(pAd,
										PrepProxyAddr,
										pRouteEntry->Idx);

				pMeshEntry = MeshEntryTableLookUp(pAd, DestMacAddr);

				if (pMeshEntry == NULL)
					MeshEntryTableInsert(pAd,
										DestMacAddr,
										pRouteEntry->Idx);
				else
					MeshEntryTableUpdate(pAd,
										DestMacAddr,
										pRouteEntry->Idx);
			}
			else
			{
				pMeshEntry = MeshEntryTableLookUp(pAd, DestMacAddr);

				if (pMeshEntry == NULL)
					MeshEntryTableInsert(pAd,
										DestMacAddr,
										pRouteEntry->Idx);
				else
					MeshEntryTableUpdate(pAd,
										DestMacAddr,
										pRouteEntry->Idx);
			}
		}
		else
		{
			if (pRouteEntry->Dsn > DestSeq)
			{
				bForward = FALSE;
			}
			else
			{
				MeshRoutingTableUpdate(pAd,
										DestMacAddr,
										DestSeq,
										pHeader->Addr2,
										pMacEntry->func_tb_idx,
										Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);

				if (((PMESH_PREP_FLAG)&PrepFlag)->field.AE == 1)
				{
					pMeshEntry = MeshEntryTableLookUp(pAd, PrepProxyAddr);

					if (pMeshEntry == NULL)
						MeshEntryTableInsert(pAd,
											PrepProxyAddr,
											pRouteEntry->Idx);
					else
						MeshEntryTableUpdate(pAd,
											PrepProxyAddr,
											pRouteEntry->Idx);

					pMeshEntry = MeshEntryTableLookUp(pAd, DestMacAddr);

					if (pMeshEntry == NULL)
						MeshEntryTableInsert(pAd,
											DestMacAddr,
											pRouteEntry->Idx);
					else
						MeshEntryTableUpdate(pAd,
											DestMacAddr,
											pRouteEntry->Idx);
				}
				else
				{
					pMeshEntry = MeshEntryTableLookUp(pAd, DestMacAddr);

					if (pMeshEntry == NULL)
						MeshEntryTableInsert(pAd,
											DestMacAddr,
											pRouteEntry->Idx);
					else
						MeshEntryTableUpdate(pAd,
											DestMacAddr,
											pRouteEntry->Idx);
				}
			}
		}

		if (PrepTTL <= 1)
		{
			bForward = FALSE;
		}
	}

	/* add or update Transmission MP Route to route table */
	if (!MAC_ADDR_EQUAL(DestMacAddr, pHeader->Addr2))
	{
		pMeshEntry = MeshEntryTableLookUp(pAd, pHeader->Addr2);
		pRouteEntry = MeshRoutingTableLookup(pAd, pHeader->Addr2);

		if (pMeshEntry && pRouteEntry)
		{
			MeshRoutingTableUpdate(pAd,
								pHeader->Addr2,
								0,
								pHeader->Addr2,
								pMacEntry->func_tb_idx,
								pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);
			MeshEntryTableUpdate(pAd,
								pHeader->Addr2,
								pRouteEntry->Idx);
		}
		else
		{
			if (pRouteEntry == NULL)
				pRouteEntry = MeshRoutingTableInsert(pAd,
												pHeader->Addr2,
												0,
												pHeader->Addr2,
												pMacEntry->func_tb_idx,
												pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);
			else
				pRouteEntry = MeshRoutingTableUpdate(pAd,
												pHeader->Addr2,
												0,
												pHeader->Addr2,
												pMacEntry->func_tb_idx,
												pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics);
	
			if (pMeshEntry == NULL)
				MeshEntryTableInsert(pAd,
									pHeader->Addr2,
									pRouteEntry->Idx);
			else
				MeshEntryTableUpdate(pAd,
									pHeader->Addr2,
									pRouteEntry->Idx);
		}
	}

	if (bForward == TRUE)
	{
		UCHAR TowardOrigNextHop[MAC_ADDR_LEN]; 

		if (pAd->MeshTab.MeshCapability.field.Forwarding == 0)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Mesh Forwarding bit disable !!!\n"));
			goto out;
		}

		/* Update precursor to route table */
		pRouteEntry = MeshRoutingTableLookup(pAd, OrigMacAddr);
		if (pRouteEntry)
		{
			MeshRoutingTablePrecursorUpdate(pAd,
										OrigMacAddr,
										pHeader->Addr2);
			COPY_MAC_ADDR(TowardOrigNextHop, pRouteEntry->NextHop);
		}

		pRouteEntry = MeshRoutingTableLookup(pAd, DestMacAddr);
		if (pRouteEntry)
			MeshRoutingTablePrecursorUpdate(pAd, DestMacAddr, TowardOrigNextHop);

		/* If the receiving MP is not the final destination of the PREP, the PREP is propagated */
		MeshPrepForward(pAd,
						MeshTTL,
						MeshSeq,
						PrepFlag,
						HopCount,
						PrepTTL,
						DestMacAddr,
						DestSeq,
						pPrepProxyAddr,
						LifeTime,
						(Metric + pAd->MeshTab.MeshLink[pMacEntry->func_tb_idx].Entry.Metrics),
						OrigMacAddr,
						OrigSeq);
	}

out:
	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPrepRcvProcess\n"));
}

VOID
MeshPrepCreate(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR	pNextHop,
	IN PUCHAR	pOrigAddr,
	IN ULONG	OrigDsn,
	IN PUCHAR	pProxyAddr)
{
	HEADER_802_11 MeshHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen;
	MESH_FLAG MeshFlag;
	MESH_PREP_FLAG MeshPrepFlag;
	UINT32 MeshSeq = INC_MESH_SEQ(pAd->MeshTab.MeshSeq);

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPrepCreate\n"));

	NStatus = MlmeAllocateMemory(pAd, (PVOID)&pOutBuffer);  /*Get an unused nonpaged memory */
	if(NStatus != NDIS_STATUS_SUCCESS) 
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Mesh - %s() allocate memory failed \n", __FUNCTION__));
		return;
	}

	MeshHeaderInit(pAd, &MeshHdr,
					pNextHop,					/*Addr1 */
					pAd->MeshTab.wdev.if_addr,	/*Addr2 */
					ZERO_MAC_ADDR);					/*Addr3 */

	NdisMoveMemory(pOutBuffer, (PCHAR)&MeshHdr, sizeof(HEADER_802_11));
	FrameLen = sizeof(HEADER_802_11);

	/* Mesh Header */
	MeshFlag.word = 0;
	MeshFlag.field.AE = 0;
	InsertMeshHeader(pAd,
					(pOutBuffer + FrameLen),
					&FrameLen,
					MeshFlag.word,
					MESH_TTL,
					MeshSeq,
					NULL,
					NULL,
					NULL);

	/* Action field */
	InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_PATH_SELECTION, ACT_CODE_PATH_REPLY);

	MeshPrepFlag.word = 0;
	if (pProxyAddr)
		MeshPrepFlag.field.AE = 1;
	else
		MeshPrepFlag.field.AE = 0;

	InsertMeshPathResponseIE(pAd,
							(pOutBuffer + FrameLen),
							&FrameLen,
							MeshPrepFlag.word,
							0,
							HWMP_NET_DIAMETER,
							pAd->MeshTab.wdev.if_addr,
							pAd->MeshTab.MeshOriginatorDsn++,
							pProxyAddr,
							HWMP_ACTIVE_PATH_TIMEOUT,
							0,
							pOrigAddr,
							OrigDsn);

	MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
	MlmeFreeMemory(pAd, pOutBuffer);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPrepCreate\n"));
}

VOID
MeshPrepForward(
	IN PRTMP_ADAPTER pAd,
	IN UINT8 MeshTTL,
	IN UINT32 MeshSeq,
	IN UINT8 MeshPrepFlag,
	IN UCHAR HopCount,
	IN UCHAR PrepTTL,
	IN PUCHAR pDestAddr,
	IN ULONG DestSeq,
	IN PUCHAR pProxyAddr,
	IN ULONG LifeTime,
	IN ULONG Metric,
	IN PUCHAR pOrigAddr,
	IN ULONG OrigSeq)
{
	HEADER_802_11 MeshHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen;
	MESH_FLAG MeshFlag;
	MESH_ROUTING_ENTRY *pEntry;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPrepForward\n"));

	NStatus = MlmeAllocateMemory(pAd, (PVOID)&pOutBuffer);  /*Get an unused nonpaged memory */
	if(NStatus != NDIS_STATUS_SUCCESS) 
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Mesh - %s() allocate memory failed \n", __FUNCTION__));
		return;
	}
	
	pEntry = MeshRoutingTableLookup(pAd, pOrigAddr);

	if (pEntry == NULL)
		return;

	MeshHeaderInit(pAd, &MeshHdr,
					pEntry->NextHop ,				/*Addr1 */
					pAd->MeshTab.wdev.if_addr,	/*Addr2 */
					ZERO_MAC_ADDR);					/*Addr3 */

	NdisMoveMemory(pOutBuffer, (PCHAR)&MeshHdr, sizeof(HEADER_802_11));
	FrameLen = sizeof(HEADER_802_11);

	MeshFlag.word = 0;
	MeshFlag.field.AE = 0;
	InsertMeshHeader(pAd,
					(pOutBuffer + FrameLen),
					&FrameLen,
					MeshFlag.word,
					MeshTTL,
					MeshSeq,
					NULL,
					NULL,
					NULL);

	InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_PATH_SELECTION, ACT_CODE_PATH_REPLY);

	InsertMeshPathResponseIE(pAd,
							(pOutBuffer + FrameLen),
							&FrameLen,
							MeshPrepFlag,
							HopCount+1,
							PrepTTL-1,
							pDestAddr,
							DestSeq,
							pProxyAddr,
							LifeTime,
							Metric,
							pOrigAddr,
							OrigSeq);

	MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
	MlmeFreeMemory(pAd, pOutBuffer);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPrepForward\n"));
}

VOID
MeshPerrRcvProcess(
	IN PRTMP_ADAPTER pAd,
	IN	RX_BLK	*pRxBlk)
{
	PHEADER_802_11 pHeader = (PHEADER_802_11)pRxBlk->pHeader;
	UINT8	MeshTTL = 0;
	UINT32	MeshSeq = 0;
	PUCHAR	pFrame;
	ULONG	FrameLen;
	PUCHAR	pMeshHdr = (PUCHAR)(pRxBlk->pData);
	UINT		i, MeshHdrLen;
	UINT8	PerrFlag;
	BOOLEAN	bForward = FALSE;
	UCHAR	VarOffset;
	UCHAR	Ie;
	UCHAR	IeLen;
	UCHAR	PerrDestCount;
	PUCHAR	pBuf = NULL;
	MESH_PERR_TABLE PerrTab;
	PMESH_PERR_ENTRY pErrEntry = NULL;
	PMESH_ROUTING_ENTRY 	pRouteEntry = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPerrRcvProcess\n"));

	if (GetMeshLinkId(pAd, (PCHAR)pHeader->Addr2) == BSS_NOT_FOUND)
		return;

	MeshTTL = GetMeshTTL(pMeshHdr);
	MeshSeq = GetMeshSeq(pMeshHdr);

	if (--MeshTTL <= 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Mesh Time to Live is zero !\n"));
		return;
	}

	MeshHdrLen = GetMeshHederLen(pRxBlk->pData);
	/* skip Mesh Header */
	pRxBlk->pData += MeshHdrLen;
	pRxBlk->DataSize -= MeshHdrLen;

	/* skip Category and ActionCode */
	pFrame = (PUCHAR)(pRxBlk->pData + 2);
	FrameLen = pRxBlk->DataSize - 2;

	VarOffset = 0;

	/* Get Path Select IE */
	NdisMoveMemory(&Ie, pFrame + VarOffset, 1);
	VarOffset += 1;
	if (Ie != IE_MESH_PERR)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Receive frame packet Element IE wrong. The IE are not Path Error!!!\n"));
		return;
	}

	/* Get IE length */
	NdisMoveMemory(&IeLen, pFrame + VarOffset, 1);
	VarOffset += 1;

	if (IeLen < 12)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Receive frame packet length wrong, the length must > 14!!!\n"));
		return;
	}

	/* Get Path Error Flags */
	NdisMoveMemory(&PerrFlag, pFrame + VarOffset, 1);
	VarOffset += 1;

	/* Get DestCount */
	NdisMoveMemory(&PerrDestCount, pFrame + VarOffset, 1);
	VarOffset += 1;

	pBuf = (pFrame + VarOffset);

	if (IeLen < (2 + (PerrDestCount * 10)))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Receive frame packet format wrong!!!\n"));
		return;
	}

	NdisZeroMemory(&PerrTab, sizeof(MESH_PERR_TABLE));

	for (i = 0; i < PerrDestCount; i++)
	{
		pErrEntry = (PMESH_PERR_ENTRY) (pFrame + VarOffset);
		COPY_MAC_ADDR(PerrTab.PerrEntry[PerrTab.size].DestAddr, pErrEntry->DestAddr);
		PerrTab.PerrEntry[PerrTab.size].Dsn = pErrEntry->Dsn;
		PerrTab.size++;
		VarOffset += 10;
	}

#ifdef RELEASE_EXCLUDE
	/*	The PERR is not discarded if the following applies: */
	/*	The MP that receives the PERR has forwarding information stored where */
	/*	1. the destination is contained in the list of unreachable destinations of the PERR and */
	/*	2. the next hop is the transmitter of the received PERR */
#endif /* RELEASE_EXCLUDE */

	for (i = 0; i < PerrDestCount; i++)
	{
		if ((pRouteEntry = MeshRoutingTableLookup(pAd, PerrTab.PerrEntry[i].DestAddr)))
		{
			if (MAC_ADDR_EQUAL(pRouteEntry->NextHop, pHeader->Addr2))
			{
				UCHAR DestAddr[MAC_ADDR_LEN];

				COPY_MAC_ADDR(DestAddr, pRouteEntry->MeshDA);
				if (pRouteEntry->bPrecursor == TRUE)
					bForward = TRUE;
				MeshRoutingTableDelete(pAd, DestAddr);
				if (BMPktSigTabLookUp(pAd, DestAddr))
					BMPktSigTabDelete(pAd, DestAddr);
			}
		}
	}

	if (bForward == TRUE)
	{
		if (pAd->MeshTab.MeshCapability.field.Forwarding == 0)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Mesh Forwarding bit disable !!!\n"));
			goto out;
		}

		MeshPerrForward(pAd,
						MeshTTL,
						MeshSeq,
						PerrDestCount,
						(PMESH_PERR_ENTRY) pBuf,
						pHeader->Addr2);
	}

out:

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPerrRcvProcess\n"));
}

VOID
MeshPerrCreate(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR			DestCount,
	IN PMESH_PERR_ENTRY	pDestEntry)
{
	HEADER_802_11 MeshHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen;
	MESH_FLAG MeshFlag;
	UINT32 MeshHeaderSeq;
	UCHAR i;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPerrCreate\n"));

	MeshHeaderSeq = INC_MESH_SEQ(pAd->MeshTab.MeshSeq);

	for(i = 0; i < MAX_MESH_LINKS; i++)
	{
		if (PeerLinkValidCheck(pAd, i) == TRUE)
		{
			NStatus = MlmeAllocateMemory(pAd, (PUCHAR *)&pOutBuffer);  /*Get an unused nonpaged memory */
			if(NStatus != NDIS_STATUS_SUCCESS) 
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Mesh - %s() allocate memory failed \n", __FUNCTION__));
				return;
			}

			MeshHeaderInit(pAd, &MeshHdr, 
							pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr,	/*Addr1 */
							pAd->MeshTab.wdev.if_addr,				/*Addr2 */
							ZERO_MAC_ADDR);						/*Addr3 */

			NdisMoveMemory(pOutBuffer, (PCHAR)&MeshHdr, sizeof(HEADER_802_11));
			FrameLen = sizeof(HEADER_802_11);

			/* Mesh Header */
			MeshFlag.word = 0;
			MeshFlag.field.AE = 0;
			InsertMeshHeader(pAd,
							(pOutBuffer + FrameLen),
							&FrameLen, MeshFlag.word,
							pAd->MeshTab.TTL,
							MeshHeaderSeq,
							NULL,
							NULL,
							NULL);

			/* Action field */
			InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_PATH_SELECTION, ACT_CODE_PATH_ERROR);

			InsertMeshPathErrorIE(pAd, (pOutBuffer + FrameLen), &FrameLen, DestCount, pDestEntry);

			MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
			MlmeFreeMemory(pAd, pOutBuffer);
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPerrCreate\n"));
}

VOID
MeshPerrForward(
	IN PRTMP_ADAPTER pAd,
	IN UINT8			MeshTTL,
	IN UINT32		MeshSeq,
	IN UCHAR			DestCount,
	IN PMESH_PERR_ENTRY	pDestEntry,
	IN PUCHAR TxAddr)
{
	HEADER_802_11 MeshHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen;
	MESH_FLAG MeshFlag;
	UCHAR i;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPerrForward\n"));

	for(i = 0; i < MAX_MESH_LINKS; i++)
	{
		if ((PeerLinkValidCheck(pAd, i) == TRUE) && !MAC_ADDR_EQUAL(TxAddr, pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr))
		{
			NStatus = MlmeAllocateMemory(pAd, (PUCHAR *)&pOutBuffer);  /*Get an unused nonpaged memory */
			if(NStatus != NDIS_STATUS_SUCCESS) 
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Mesh - %s() allocate memory failed \n", __FUNCTION__));
				return;
			}

			MeshHeaderInit(pAd, &MeshHdr, 
							pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr,	/*Addr1 */
							pAd->MeshTab.wdev.if_addr,				/*Addr2 */
							BROADCAST_ADDR);						/*Addr3 */

			NdisMoveMemory(pOutBuffer, (PCHAR)&MeshHdr, sizeof(HEADER_802_11));
			FrameLen = sizeof(HEADER_802_11);

			/* Mesh Header */
			MeshFlag.word = 0;
			MeshFlag.field.AE = 0;
			InsertMeshHeader(pAd,
							(pOutBuffer + FrameLen),
							&FrameLen,
							MeshFlag.word,
							MeshTTL,
							MeshSeq,
							pAd->MeshTab.wdev.if_addr,
							NULL,
							NULL);

			/* Action field */
			InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_PATH_SELECTION, ACT_CODE_PATH_ERROR);

			InsertMeshPathErrorIE(pAd,
								(pOutBuffer + FrameLen),
								&FrameLen,
								DestCount,
								pDestEntry);

			MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
			MlmeFreeMemory(pAd, pOutBuffer);
		}
	}
	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPerrForward\n"));
	return;
}

VOID
MeshCreatePreqAction(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pProxyAddr,
	IN PUCHAR pDestAddr)
{
	UCHAR DestAddr[MAC_ADDR_LEN];
	PMESH_ENTRY pMeshEntry = NULL;
	PUCHAR pBuf = NULL;
	PPATH_REQ_ENTRY	pNewPReqEntry = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshCreatePreqAction\n"));

	COPY_MAC_ADDR(DestAddr, pDestAddr);

	if (pAd->MeshTab.LinkSize == 0)
		return;

	pMeshEntry = MeshEntryTableLookUp(pAd, DestAddr);

	if (pMeshEntry)
	{
		if (pMeshEntry->PathReqTimerRunning)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Path discovery of Mesh Entry (%02x:%02x:%02x:%02x:%02x:%02x) is running !!\n",
								pMeshEntry->DestAddr[0], pMeshEntry->DestAddr[1], pMeshEntry->DestAddr[2],
								pMeshEntry->DestAddr[3], pMeshEntry->DestAddr[4], pMeshEntry->DestAddr[5]));
			return;
		}

		if (pMeshEntry->PathReq != NULL)
			{
			BOOLEAN Cancelled;

			RTMPCancelTimer(&pMeshEntry->PathReq->PathReqTimer, &Cancelled);
			os_free_mem(NULL, pMeshEntry->PathReq);
			pMeshEntry->PathReq = NULL;
		}
	}
	else
	{
		pMeshEntry = MeshEntryTableInsert(pAd, DestAddr, 0xff);

		if (!pMeshEntry)
			return;
	}

	if (pMeshEntry)
	{
		os_alloc_mem(NULL, (UCHAR **)&pNewPReqEntry, sizeof(PATH_REQ_ENTRY));

		if (pNewPReqEntry)
		{
			NdisZeroMemory(pNewPReqEntry, sizeof(PATH_REQ_ENTRY));

			pMeshEntry->PathReq = pNewPReqEntry;

			/* Initialize Path Req session */
			pNewPReqEntry->RetryCount = 0;
			if (pProxyAddr)
			{
				COPY_MAC_ADDR(pNewPReqEntry->ProxyAddr, pProxyAddr);
				pNewPReqEntry->bProxied = TRUE;
			}
			COPY_MAC_ADDR(pNewPReqEntry->DestAddr, DestAddr);
			pNewPReqEntry->DestDsn = 0;
			pNewPReqEntry->pAdapter = pAd;

			RTMPInitTimer(pAd, &pNewPReqEntry->PathReqTimer, GET_TIMER_FUNCTION(MeshPathReqTimeoutAction),
							pNewPReqEntry, FALSE);

			/* set timer to send Path Request message */
			pMeshEntry->PathReqTimerRunning = TRUE;
			RTMPSetTimer(&pNewPReqEntry->PathReqTimer, HWMP_NETDIAMETER_TRAVERSAL_TIME);

			MeshPreqCreate(pAd, pProxyAddr, DestAddr, 0);
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Create Path Req Timer Entry fail\n"));
			return;
		}
	}

	if (pBuf != NULL)
		os_free_mem(NULL, pBuf);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshCreatePreqAction\n"));
}

VOID MeshPathReqTimeoutAction(
							 IN PVOID SystemSpecific1, 
							 IN PVOID FunctionContext, 
							 IN PVOID SystemSpecific2, 
							 IN PVOID SystemSpecific3) 
{
	PATH_REQ_ENTRY	*pPathReqEntry = (PATH_REQ_ENTRY *)FunctionContext;
	PRTMP_ADAPTER	pAd;
	PMESH_ENTRY		pMeshEntry = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshPathReqTimeoutAction\n"));

	if (pPathReqEntry == NULL)
		return;

	pAd = pPathReqEntry->pAdapter;

	if (pPathReqEntry->RetryCount < HWMP_MAX_PREQ_RETRIES)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Path Request Session Timeout(%d) : Send Path Request again\n", pPathReqEntry->RetryCount));

		pPathReqEntry->RetryCount++;
		RTMPSetTimer(&pPathReqEntry->PathReqTimer, HWMP_NETDIAMETER_TRAVERSAL_TIME);

		if (pPathReqEntry->bProxied == TRUE)
			MeshPreqCreate(pAd,
							pPathReqEntry->ProxyAddr,
							pPathReqEntry->DestAddr,
							pPathReqEntry->DestDsn);
		else
			MeshPreqCreate(pAd,
							NULL,
							pPathReqEntry->DestAddr,
							pPathReqEntry->DestDsn);
	}
	else
	{
		UCHAR	DestAddr[MAC_ADDR_LEN];

		COPY_MAC_ADDR(DestAddr, pPathReqEntry->DestAddr);

		pMeshEntry = MeshEntryTableLookUp(pAd, DestAddr);

		if (pMeshEntry && pMeshEntry->PathReq)
		{
			BOOLEAN Cancelled;

			RTMPCancelTimer(&pMeshEntry->PathReq->PathReqTimer, &Cancelled);
			os_free_mem(NULL, pMeshEntry->PathReq);
			pMeshEntry->PathReq = NULL;

			DBGPRINT(RT_DEBUG_TRACE, ("Path discovery of Mesh Entry (%02x:%02x:%02x:%02x:%02x:%02x) retry limit !!\n",
								pMeshEntry->DestAddr[0], pMeshEntry->DestAddr[1], pMeshEntry->DestAddr[2],
								pMeshEntry->DestAddr[3], pMeshEntry->DestAddr[4], pMeshEntry->DestAddr[5]));
		}
		
		MeshEntryTableDelete(pAd, DestAddr);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshPathReqTimeoutAction\n"));
}

VOID
MeshCreatePerrAction(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pBreakMeshPoint)
{
	PMESH_ROUTING_ENTRY	pEntry;
	UINT i, precursor_size = 0;
	MESH_PERR_TABLE PerrTab;
	PUCHAR pBuf = NULL, pPtr;
	PMESH_PERR_ENTRY pErrEntry = NULL;
	PMESH_ROUTING_TABLE	pRouteTab = pAd->MeshTab.pMeshRouteTab;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshCreatePerrAction\n"));

	if (pRouteTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Route Table doesn't exist.\n", __FUNCTION__));
		return;
	}

	NdisZeroMemory(&PerrTab, sizeof(MESH_PERR_TABLE));

	RTMP_SEM_LOCK(&pAd->MeshTab.MeshRouteTabLock);
	for (i = 0; i< MAX_ROUTE_TAB_SIZE; i++)
	{
		pEntry = &pRouteTab->Content[i];
		if (pEntry->Valid == TRUE)
		{
			if (MAC_ADDR_EQUAL(pEntry->NextHop, pBreakMeshPoint))
			{
				COPY_MAC_ADDR(PerrTab.PerrEntry[PerrTab.size].DestAddr, pEntry->MeshDA);
				PerrTab.PerrEntry[PerrTab.size].Dsn = pEntry->Dsn;
				PerrTab.size++;

				if (pEntry->bPrecursor == TRUE)
					precursor_size++;
			}
		}
	}
	RTMP_SEM_UNLOCK(&pAd->MeshTab.MeshRouteTabLock);

	if (PerrTab.size == 0)
		return;

	os_alloc_mem(NULL, (UCHAR **)&pBuf, (PerrTab.size*sizeof(MESH_PERR_ENTRY)));
	if (pBuf)
		NdisZeroMemory(pBuf, PerrTab.size*sizeof(MESH_PERR_ENTRY));
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pBuf \n", __FUNCTION__));
		return;
	}

	pPtr = pBuf;

	for (i = 0; i< PerrTab.size; i++)
	{
		pErrEntry = (PMESH_PERR_ENTRY) pPtr;
		COPY_MAC_ADDR(pErrEntry->DestAddr , PerrTab.PerrEntry[i].DestAddr);
		pErrEntry->Dsn = PerrTab.PerrEntry[i].Dsn;
		pPtr = pPtr + sizeof(MESH_PERR_ENTRY);

		MeshRoutingTableDelete(pAd, PerrTab.PerrEntry[i].DestAddr);
		if (BMPktSigTabLookUp(pAd, PerrTab.PerrEntry[i].DestAddr))
			BMPktSigTabDelete(pAd, PerrTab.PerrEntry[i].DestAddr);
	}

	if (precursor_size > 0)
	{
		MeshPerrCreate(pAd, PerrTab.size, (PMESH_PERR_ENTRY)pBuf);
	}

	os_free_mem(NULL, pBuf);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshCreatePerrAction\n"));

	return;
}

VOID
MeshMultipathNotice(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pPeerMac,
	IN PUCHAR pMeshSA,
	IN UCHAR SwitchOn)
{
	HEADER_802_11 MeshHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen;
	MESH_FLAG MeshFlag;

	UINT32 MeshSeq = INC_MESH_SEQ(pAd->MeshTab.MeshSeq);

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshMultipathNotice\n"));

	NStatus = MlmeAllocateMemory(pAd, (PVOID)&pOutBuffer);  /*Get an unused nonpaged memory */
	if(NStatus != NDIS_STATUS_SUCCESS) 
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Mesh - %s() allocate memory failed \n", __FUNCTION__));
		return;
	}

	MeshHeaderInit(pAd, &MeshHdr,
			pPeerMac,					/* addr1 */
			pAd->MeshTab.wdev.if_addr,	/* addr2 */
			ZERO_MAC_ADDR);					/* addr3 */

	NdisMoveMemory(pOutBuffer, (PCHAR)&MeshHdr, sizeof(HEADER_802_11));
	FrameLen = sizeof(HEADER_802_11);

	/* Mesh Header */
	MeshFlag.word = 0;
	MeshFlag.field.AE = 0;
	InsertMeshHeader(pAd, (pOutBuffer + FrameLen), &FrameLen, MeshFlag.word,
		pAd->MeshTab.TTL, MeshSeq, NULL, NULL, NULL);

	/* Action field */
	InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_PATH_SELECTION, 
														ACT_CODE_MULTIPATH_NOTICE);

	InsertMeshCongestionNotofocationIE(pAd, (pOutBuffer + FrameLen), &FrameLen, pMeshSA, SwitchOn);

	MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
	MlmeFreeMemory(pAd, pOutBuffer);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshMultipathNotice\n"));
}

VOID MeshMultipathNoticeRcvProcess(
	IN PRTMP_ADAPTER pAd,
	IN RX_BLK *pRxBlk)
{
	PHEADER_802_11 pHeader = (PHEADER_802_11)pRxBlk->pHeader;
	PUCHAR	pFrame;
	ULONG	FrameLen;
	UINT	MeshHdrLen;
	UINT8	Flag;
	UCHAR	MeshSA[MAC_ADDR_LEN];
	ULONG	LinkIdx;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MeshMultipathNoticeRcvProcess\n"));

	if ((LinkIdx = GetMeshLinkId(pAd, (PCHAR)pHeader->Addr2)) == BSS_NOT_FOUND)
		return;

	if (!PeerLinkValidCheck(pAd, LinkIdx))
		return;

	MeshHdrLen = GetMeshHederLen(pRxBlk->pData);
	/* skip Mesh Header */
	pRxBlk->pData += MeshHdrLen;
	pRxBlk->DataSize -= MeshHdrLen;

	/* skip Category and ActionCode */
	pFrame = (PUCHAR)(pRxBlk->pData + 2);
	FrameLen = pRxBlk->DataSize - 2;

	MeshPathSelMultipathNoticeSanity( pAd,
							pFrame,
							FrameLen,
							&Flag,
							MeshSA);

	if ((Flag & 0x01) == 0)
		MultipathEntryDelete(pAd, LinkIdx, MeshSA);
	else
		MultipathEntryInsert(pAd, LinkIdx, MeshSA);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MeshMultipathNoticeRcvProcess\n"));
}

BOOLEAN MeshAllowToSendPathResponse(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR		pSA)
{
	if (MAC_ADDR_EQUAL(pAd->MeshTab.wdev.if_addr, pSA))
		return TRUE;

	if ((pAd->MeshTab.OpMode & MESH_AP) ||(pAd->MeshTab.OpMode & MESH_POTAL))
	{
		PMESH_PROXY_ENTRY pMeshProxyEntry = NULL;

		pMeshProxyEntry = MeshProxyEntryTableLookUp(pAd, pSA);

		if (pMeshProxyEntry)
		{
			if (MAC_ADDR_EQUAL(pAd->MeshTab.wdev.if_addr, pMeshProxyEntry->Owner))
				return TRUE;
		}
	}

	return FALSE;
}

#endif /* MESH_SUPPORT */

