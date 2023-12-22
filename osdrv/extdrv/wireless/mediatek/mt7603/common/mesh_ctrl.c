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
	mesh_ctrl.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2007-06-25      For mesh (802.11s) support.
*/
#ifdef MESH_SUPPORT


#include "rt_config.h"
#include "mesh_sanity.h"

static VOID MeshPldTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

static VOID MeshMcsTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

static VOID
MeshCtrlJoinAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshCtrlDisconnectAtDicoveryAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshCtrlFinishDiscoveryAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshCtrlDiscoveryAbortAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshCtrlPeriodPLDAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshCtrlDisconnectAtActivatedAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshCtrlUcgWhenActivated(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshPeerUcgWhenActivated(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshPeerUcgWhenUcg(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshCtrlDisconnectAtUCGAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID
MeshCtrlMcsTimeoutAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

DECLARE_TIMER_FUNCTION(MeshPldTimeout);
DECLARE_TIMER_FUNCTION(MeshMcsTimeout);

BUILD_TIMER_FUNCTION(MeshPldTimeout);
BUILD_TIMER_FUNCTION(MeshMcsTimeout);

static VOID
NeighborEntrySet(
	OUT PMESH_NEIGHBOR_ENTRY pNeighbor, 
	IN CHAR RealRssi,
	IN PUCHAR pPeerMacAddr, 
	IN RTMP_STRING *pHostName, 
	IN PCHAR pMeshId, 
	IN UCHAR MeshIdLen, 
	IN UCHAR Channel,
	IN UINT8 ChBW,
	IN UINT8 ExtChOffset,
	IN USHORT 	CapabilityInfo,
	IN PUCHAR	pVIE,
	IN USHORT	LenVIE,
	IN PMESH_CONFIGURAION_IE pMeshConfig);

static PMESH_NEIGHBOR_ENTRY
NeighMPWithMaxCPI(
	IN PRTMP_ADAPTER pAd,
	IN PMESH_NEIGHBOR_TAB pNeighborTab);

static VOID
EnqueChSwAnnouncement(
		IN PRTMP_ADAPTER pAd,
		IN PUCHAR PeerMac,
		IN UINT8 MeshTTL,
		IN UINT32 MeshSeq,
		IN UINT8 ChSwMode,
		IN UCHAR NewCh,
		IN UINT32 NewCPI,
		IN UINT8 ChSwCnt,
		IN PUCHAR pMeshSa);

BOOLEAN MeshValid(PMESH_STRUCT pMesh)
{
	return (pMesh->CtrlCurrentState == MESH_CTRL_ACTIVATED) ? TRUE : FALSE;
}

/*
    ==========================================================================
    Description:
        The mesh control state machine, 
    Parameters:
        Sm - pointer to the state machine
    Note:
        the state machine looks like the following
    ==========================================================================
 */
VOID MeshCtrlStateMachineInit(
	IN PRTMP_ADAPTER pAd,
	IN STATE_MACHINE *Sm,
	OUT STATE_MACHINE_FUNC Trans[])
{
	StateMachineInit(Sm, (STATE_MACHINE_FUNC*)Trans, (ULONG)MESH_CTRL_MAX_STATES,
		(ULONG)MESH_CTRL_MAX_EVENTS, (STATE_MACHINE_FUNC)Drop, MESH_CTRL_IDLE, MESH_CTRL_IDLE);

	/* MESH_CTRL_IDLE state */
	StateMachineSetAction(Sm, MESH_CTRL_IDLE, MESH_CTRL_JOIN, (STATE_MACHINE_FUNC)MeshCtrlJoinAction);

	/* MESH_CTRL_DISCOVERY state */
	StateMachineSetAction(Sm, MESH_CTRL_DISCOVERY, MESH_CTRL_DISCONNECT, (STATE_MACHINE_FUNC)MeshCtrlDisconnectAtDicoveryAction);
	StateMachineSetAction(Sm, MESH_CTRL_DISCOVERY, APMT2_MLME_SCAN_FINISH, (STATE_MACHINE_FUNC)MeshCtrlFinishDiscoveryAction);
	StateMachineSetAction(Sm, MESH_CTRL_DISCOVERY, APMT2_MLME_SCAN_ABORT, (STATE_MACHINE_FUNC)MeshCtrlDiscoveryAbortAction);

	/* MESH_CTRL_ACTIVATED state */
	StateMachineSetAction(Sm, MESH_CTRL_ACTIVATED, MESH_CTRL_PLD, (STATE_MACHINE_FUNC)MeshCtrlPeriodPLDAction);
	StateMachineSetAction(Sm, MESH_CTRL_ACTIVATED, MESH_CTRL_DISCONNECT, (STATE_MACHINE_FUNC)MeshCtrlDisconnectAtActivatedAction);
	StateMachineSetAction(Sm, MESH_CTRL_ACTIVATED, MESH_CTRL_UCG_EVT, (STATE_MACHINE_FUNC)MeshCtrlUcgWhenActivated);
	StateMachineSetAction(Sm, MESH_CTRL_ACTIVATED, MESH_PEER_UCG_EVT, (STATE_MACHINE_FUNC)MeshPeerUcgWhenActivated);

	/* MESH_CTRL_UCG state */
	StateMachineSetAction(Sm, MESH_CTRL_UCG, MESH_CTRL_DISCONNECT, (STATE_MACHINE_FUNC)MeshCtrlDisconnectAtUCGAction);
	StateMachineSetAction(Sm, MESH_CTRL_UCG, MESH_PEER_UCG_EVT, (STATE_MACHINE_FUNC)MeshPeerUcgWhenUcg);
	StateMachineSetAction(Sm, MESH_CTRL_UCG, MESH_CTRL_MCS_TIMEOUT, (STATE_MACHINE_FUNC)MeshCtrlMcsTimeoutAction);

	/* init all Mesh ctrl state. */
	pAd->MeshTab.CtrlCurrentState = MESH_CTRL_IDLE;

	/* init all timer relative to mesh ctrl sm. */
	RTMPInitTimer(pAd, &pAd->MeshTab.PldTimer, GET_TIMER_FUNCTION(MeshPldTimeout), pAd, FALSE);
	RTMPInitTimer(pAd, &pAd->MeshTab.McsTimer, GET_TIMER_FUNCTION(MeshMcsTimeout), pAd, FALSE);

	return;
}

/*
    ==========================================================================
    Description:
        Peer-Link-Discovery timeout procedure.
    Parameters:
        Standard timer parameters
    ==========================================================================
 */
static VOID MeshPldTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;

	DBGPRINT(RT_DEBUG_TRACE, ("%s - enqueue MESH_CTRL_PLD to MESH_CTRL State-Machine.\n", __FUNCTION__));

	MlmeEnqueue(pAd, MESH_CTRL_STATE_MACHINE, MESH_CTRL_PLD, 0, NULL, 0);
    RTMP_MLME_HANDLER(pAd);

	return;
}

/*
    ==========================================================================
    Description:
        Mesh-Channel-Switch timeout procedure.
    Parameters:
        Standard timer parameters
    ==========================================================================
 */
static VOID MeshMcsTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;

	DBGPRINT(RT_DEBUG_TRACE, ("%s - enqueue MESH_CTRL_MCS_TIMEOUT to MESH_CTRL State-Machine.\n", __FUNCTION__));

	MlmeEnqueue(pAd, MESH_CTRL_STATE_MACHINE, MESH_CTRL_MCS_TIMEOUT, 0, NULL, 0);
    RTMP_MLME_HANDLER(pAd);

	return;
}

static VOID
MeshCtrlJoinAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);

#ifdef CONFIG_STA_SUPPORT
	/* Initialize RF register to default value */
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		MeshChannelInit(pAd);
	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef INTEL_CMPC
	/* Set PLD timer. */
	/* transit state to MESH_CTRL_ACTIVATED. */

	RTMPSetTimer(&pAd->MeshTab.PldTimer, PLD_TIME);
	*pCurrState = MESH_CTRL_ACTIVATED;
#else
#ifdef RELEASE_EXCLUDE
	/* send APMT2_MLME_SCAN_REQ to AP-SYNC FSM. */
	/* transit state to MESH_CTRL_DISCOVERY state. */
#endif /* RELEASE_EXCLUDE */

	MLME_SCAN_REQ_STRUCT ScanReq;
	UCHAR BroadSsid[MAX_LEN_OF_SSID];

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get Join evt when Idle.\n", __FUNCTION__));

	AsicDisableSync(pAd);

	BssTableInit(&pAd->ScanTab);
	BroadSsid[0] = '\0';

	ScanReq.SsidLen = 0;
	NdisMoveMemory(ScanReq.Ssid, BroadSsid, ScanReq.SsidLen);
	ScanReq.BssType = BSS_ANY;
	ScanReq.ScanType = SCAN_PASSIVE;

#ifdef CONFIG_AP_SUPPORT
	pAd->Mlme.ApSyncMachine.CurrState = AP_SYNC_IDLE;
	MlmeEnqueue(pAd, AP_SYNC_STATE_MACHINE, APMT2_MLME_SCAN_REQ, sizeof(MLME_SCAN_REQ_STRUCT), &ScanReq, 0);
	RTMP_MLME_HANDLER(pAd);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	ScanParmFill(pAd, &ScanReq, "", 0, BSS_ANY, SCAN_ACTIVE);
	MlmeEnqueue(pAd, SYNC_STATE_MACHINE, MT2_MLME_SCAN_REQ, 
		sizeof(MLME_SCAN_REQ_STRUCT), &ScanReq, 0);
#endif /* CONFIG_STA_SUPPORT */

	*pCurrState = MESH_CTRL_DISCOVERY;
#endif
}

static VOID
MeshCtrlDisconnectAtDicoveryAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
#ifdef RELEASE_EXCLUDE
	/* send MESH_LINK_MNG_CNCL to MESH_LINK_MNG FSM. */
	/* send APMT2_MLME_SCAN_CNCL to AP_SYNC FSM. */
	/* transit state to MESH_CTRL_IDLE. */
#endif /* RELEASE_EXCLUDE */

	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);
	INT i;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get Disconnect evt when Discovery.\n", __FUNCTION__));

	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		MlmeEnqueue(pAd, MESH_LINK_MNG_STATE_MACHINE, MESH_LINK_MNG_CNCL, 0, NULL, i);
	}
	
#ifdef AP_SCAN_SUPPORT
	MlmeEnqueue(pAd, AP_SYNC_STATE_MACHINE, APMT2_MLME_SCAN_CNCL, 0, NULL, 0);
#endif
	*pCurrState = MESH_CTRL_IDLE;
}

static VOID
MeshCtrlFinishDiscoveryAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
#ifdef RELEASE_EXCLUDE
	/* send MESH_LINK_MNG_PASOPN or MESH_LINK_MNG_ACTOPN to MESH_LINK_MNG FSM. */
	/* Set PLD timer. */
	/* transite state to MESH_CTRL_ACTIVATED. */
#endif /* RELEASE_EXCLUDE */

	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);
	INT i;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get Scan-Finish evt when Discovery.\n", __FUNCTION__));

	NeighborTableUpdate(pAd);
	CandidateMPSelect(pAd);

	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		if(pAd->MeshTab.MeshLink[i].Entry.Valid)
			MlmeEnqueue(pAd, MESH_LINK_MNG_STATE_MACHINE, MESH_LINK_MNG_ACTOPN, 0, NULL, i);
	}

	RTMPSetTimer(&pAd->MeshTab.PldTimer, PLD_TIME + RANDOM_TIME(pAd));
	*pCurrState = MESH_CTRL_ACTIVATED;
}

static VOID
MeshCtrlDiscoveryAbortAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);

#ifdef RELEASE_EXCLUDE
	/* skip the scan result since scan abort. */
	/* Set PLD timer. */
	/* transite state to MESH_CTRL_ACTIVATED. */
#endif /* RELEASE_EXCLUDE */

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get Scan-Abort evt when Discovery.\n", __FUNCTION__));

	RTMPSetTimer(&pAd->MeshTab.PldTimer, PLD_TIME + RANDOM_TIME(pAd));
	*pCurrState = MESH_CTRL_ACTIVATED;
}

static VOID
MeshCtrlPeriodPLDAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
#ifdef INTEL_CMPC
	/* select candidate MPs from neighbor list. */
	/* set PLD timer. */

	INT i;

	CandidateMPSelect(pAd);
	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		BOOLEAN CnetFlag = FALSE;

		if (pAd->MeshTab.MeshAutoLink == TRUE)
			CnetFlag = pAd->MeshTab.MeshLink[i].Entry.Valid && !PeerLinkValidCheck(pAd, i);
		else
			CnetFlag = pAd->MeshTab.MeshLink[i].Entry.Valid
				&& (pAd->MeshTab.MeshLink[i].Entry.LinkType == MESH_LINK_STATIC)
				&& !PeerLinkValidCheck(pAd, i);
			
		if (CnetFlag)
			MlmeEnqueue(pAd, MESH_LINK_MNG_STATE_MACHINE, MESH_LINK_MNG_ACTOPN, 0, NULL, i);
	}

	RTMPSetTimer(&pAd->MeshTab.PldTimer, PLD_TIME);
#else
#ifdef RELEASE_EXCLUDE
	/* send APMT2_MLME_SCAN_REQ to AP-SYNC FSM. */
	/* transit state to MESH_CTRL_DISCOVERY state. */
#endif /* RELEASE_EXCLUDE */

	PULONG pCurrState = &(pAd->MeshTab.CtrlCurrentState);
	INT i;
	BOOLEAN Valid = TRUE;
	MLME_SCAN_REQ_STRUCT ScanReq;
	UCHAR BroadSsid[MAX_LEN_OF_SSID];

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get PLD evt when Activated.\n", __FUNCTION__));

	AsicDisableSync(pAd);

	BssTableInit(&pAd->ScanTab);
	BroadSsid[0] = '\0';

	ScanReq.SsidLen = 0;
	NdisMoveMemory(ScanReq.Ssid, BroadSsid, ScanReq.SsidLen);
	ScanReq.BssType = BSS_ANY;
	ScanReq.ScanType = SCAN_PASSIVE;

#ifdef RELEASE_EXCLUDE
	/* should check that all valid peer links finish establish. */
	/* shall not scan air during Peer link action. */
#endif /* RELEASE_EXCLUDE */
	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		if (PeerLinkMngRuning(pAd, i))
		{
			Valid = FALSE;
			break;
		}
	}

	if (Valid)
	{
#ifdef CONFIG_AP_SUPPORT
		pAd->Mlme.ApSyncMachine.CurrState = AP_SYNC_IDLE;
		MlmeEnqueue(pAd, AP_SYNC_STATE_MACHINE, APMT2_MLME_SCAN_REQ, sizeof(MLME_SCAN_REQ_STRUCT), &ScanReq, 0);
		RTMP_MLME_HANDLER(pAd);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	ScanParmFill(pAd, &ScanReq, "", 0, BSS_ANY, SCAN_ACTIVE);
	MlmeEnqueue(pAd, SYNC_STATE_MACHINE, MT2_MLME_SCAN_REQ, 
		sizeof(MLME_SCAN_REQ_STRUCT), &ScanReq, 0);
#endif /* CONFIG_STA_SUPPORT */
		*pCurrState = MESH_CTRL_DISCOVERY;
	}
	else
	{
		RTMPSetTimer(&pAd->MeshTab.PldTimer, PLD_TIME + RANDOM_TIME(pAd));
	}
#endif /* INTEL_CMPC */
}

static VOID
MeshCtrlDisconnectAtActivatedAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
#ifdef RELEASE_EXCLUDE
	/* send MESH_LINK_MNG_CNCL to MESH_LINK_MNG FSM. */
	/* Cancel PLD timer. */
	/* transit state to MESH_CTRL_IDLE. */
#endif /* RELEASE_EXCLUDE */

	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);
	INT i;
	BOOLEAN Cancelled;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get Disconnect evt when Activated.\n", __FUNCTION__));

	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		MlmeEnqueue(pAd, MESH_LINK_MNG_STATE_MACHINE, MESH_LINK_MNG_CNCL, 0, NULL, i);
	}
	RTMPCancelTimer(&pAd->MeshTab.PldTimer, &Cancelled);

	*pCurrState = MESH_CTRL_IDLE;
}

static VOID
MeshCtrlUcgWhenActivated(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
#ifdef RELEASE_EXCLUDE
	/* Cancel PLD timer. */
	/* chooses a meshchannel switch time. And set MCS timer with the waiting time value. */
	/* increase CPI with a random value. */
	/* send MeshChSwAnnouncement frame to each peer MP. */
	/* transit state to MESH_CTRL_UCG. */
#endif /* RELEASE_EXCLUDE */

	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);
	INT i;
	BOOLEAN Cancelled;
	UINT8 ChSwCnt = RandomChSwWaitTime(pAd);
	UINT32 NewCPI = pAd->MeshTab.CPI += RandomByte(pAd);
	UINT32 ChSwMode = 1;

	RTMPCancelTimer(&pAd->MeshTab.PldTimer, &Cancelled);
	RTMPSetTimer(&pAd->MeshTab.McsTimer, ChSwCnt * MESH_TU);

	/* propagate channel switch announcement to each peer link. */
	INC_MESH_SEQ(pAd->MeshTab.MeshSeq);
	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		if (PeerLinkValidCheck(pAd, i) == TRUE)
			EnqueChSwAnnouncement(
				pAd,
				pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr,
				pAd->MeshTab.TTL,
				pAd->MeshTab.MeshSeq,
				ChSwMode,
				pAd->MeshTab.MeshChannel,
				NewCPI,
				ChSwCnt,
				pAd->MeshTab.wdev.if_addr);
	}

	pAd->MeshTab.NewCPI = NewCPI;
	*pCurrState = MESH_CTRL_UCG;
	
	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get UCG evt when Activated.\n", __FUNCTION__));
}

static VOID
MeshPeerUcgWhenActivated(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
#ifdef RELEASE_EXCLUDE
	/* check the CPI value from ChSwAnnouncement frame larger than current CPI value. */
	/* if yes copy CPI from frame and start Ucg. if not then ignore the frame. */

	/* Cancel PLD timer. */
	/* Cancel MCS timer. */
	/* set CPI with the CPI value with the frame. */
	/* set MCS timer with the ChSwWaitTime value with the frame. */
	/* send MeshChSwAnnouncement frame to each peer MP. */
	/* transit state to MESH_CTRL_UCG. */
#endif /* RELEASE_EXCLUDE */

	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);
	BOOLEAN Cancelled;
	PMESH_CH_SW_ANN_MSG_STRUCT pInfo = (PMESH_CH_SW_ANN_MSG_STRUCT)(Elem->Msg);
	UINT32 NewCPI = pInfo->NewCPI;
	PUCHAR pMeshSA = pInfo->MeshSA;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get UCG evt when Activated.\n", __FUNCTION__));

	if ((NewCPI < pAd->MeshTab.CPI)
		|| ((NewCPI == pAd->MeshTab.CPI) && (memcmp(pAd->MeshTab.wdev.if_addr, pMeshSA, MAC_ADDR_LEN) >= 0))
		)
	{
		return;
	}

	RTMPCancelTimer(&pAd->MeshTab.PldTimer, &Cancelled);
	MeshPeerUcgWhenUcg(pAd, Elem);
	*pCurrState = MESH_CTRL_UCG;
	
	return;
}

static VOID
MeshPeerUcgWhenUcg(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
#ifdef RELEASE_EXCLUDE
	/* check the CPI value from ChSwAnnouncement frame larger than current CPI value. */
	/* if yes copy CPI from frame and start Ucg. if not then ignore the frame. */

	/* Cancel PLD timer. */
	/* Cancel MCS timer. */
	/* set CPI with the CPI value with the frame. */
	/* set MCS timer with the ChSwWaitTime value with the frame. */
	/* send MeshChSwAnnouncement frame to each peer MP. */
	/* transit state to MESH_CTRL_UCG. */
#endif /* RELEASE_EXCLUDE */

	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);
	INT i;
	BOOLEAN Cancelled;
	PMESH_CH_SW_ANN_MSG_STRUCT pInfo = (PMESH_CH_SW_ANN_MSG_STRUCT)(Elem->Msg);
	UINT32 NewCPI = pInfo->NewCPI;
	UINT32 MeshSeq = pInfo->MeshSeq;
	UINT8 MeshTTL = pInfo->MeshTTL;
	UINT8 ChSwCnt = pInfo->ChSwCnt;
	UINT8 NewCh = pInfo->NewCh;
	UINT8 ChSwMode = pInfo->ChSwMode;
	UINT8 LinkId = pInfo->LinkId;
	PUCHAR pMeshSA = pInfo->MeshSA;

	if ((NewCPI < pAd->MeshTab.CPI)
		|| ((NewCPI == pAd->MeshTab.CPI) && (memcmp(pAd->MeshTab.wdev.if_addr, pMeshSA, MAC_ADDR_LEN) >= 0))
		)
	{
		return;
	}

	if (*pCurrState == MESH_CTRL_UCG)
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Get UCG evt when Activated.\n", __FUNCTION__));

	pAd->MeshTab.MeshChannel = NewCh;
	RTMPCancelTimer(&pAd->MeshTab.McsTimer, &Cancelled);
	RTMPSetTimer(&pAd->MeshTab.McsTimer, ChSwCnt * MESH_TU);

	/* foward ChSwAnnouncement to each peer link. */
	MeshTTL--;
	do
	{
		if (MeshTTL == 0)
			break;

	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
			if (PeerLinkValidCheck(pAd, i) == FALSE)
				continue;

			if (LinkId == i)
				continue;

			if (MAC_ADDR_EQUAL(pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr, pMeshSA) == TRUE)
				continue;

			EnqueChSwAnnouncement(
				pAd,
				pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr,
				MeshTTL,
				MeshSeq,
				ChSwMode,
				NewCh,
				NewCPI,
				ChSwCnt,
				pMeshSA);
	}
	} while(FALSE);

	pAd->MeshTab.NewCPI = NewCPI;

	return;
}

static VOID
MeshCtrlDisconnectAtUCGAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
#ifdef RELEASE_EXCLUDE
	/* send MESH_LINK_MNG_CNCL to MESH_LINK_MNG */
	/* cancel MCS timer. */
	/* transit state to MESH_CTRL_IDLE. */
#endif /* RELEASE_EXCLUDE */

	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);
	INT i;
	BOOLEAN Cancelled;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get Disconnect evt when UCG.\n", __FUNCTION__));

	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		MlmeEnqueue(pAd, MESH_LINK_MNG_STATE_MACHINE, MESH_LINK_MNG_CNCL, 0, NULL, i);
	}
	RTMPCancelTimer(&pAd->MeshTab.McsTimer, &Cancelled);

	*pCurrState = MESH_CTRL_IDLE;
}

static VOID
MeshCtrlMcsTimeoutAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
#ifdef RELEASE_EXCLUDE
	/* switch to a new channel. */
	/* set PLD timer. */
	/* transit state to MESH_CTRL_ACTIVATED. */
#endif /* RELEASE_EXCLUDE */

	MESH_CTRL_STATE *pCurrState = &(pAd->MeshTab.CtrlCurrentState);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Get Mcs evt when UCG.\n", __FUNCTION__));

	if (!INFRA_ON(pAd) && !ADHOC_ON(pAd))
	{
		/*SET_MESH_CHANNEL(pAd); */
		pAd->CommonCfg.Channel = pAd->MeshTab.MeshChannel;
#ifdef DOT11_N_SUPPORT
		N_ChannelCheck(pAd);
#endif /* DOT11_N_SUPPORT */
		AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
		AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
		
		pAd->MeshTab.CPI = pAd->MeshTab.NewCPI;
	}
	else
		pAd->MeshTab.MeshChannel = pAd->CommonCfg.Channel;

	RTMPSetTimer(&pAd->MeshTab.PldTimer, PLD_TIME);

	*pCurrState = MESH_CTRL_ACTIVATED;
}

BOOLEAN NeighborMPCheck(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pMeshId,
	IN UINT8 PathSelProtocolId,
	IN UINT8 PathSelMetricId)
{
	BOOLEAN result = FALSE;

	if ((NdisEqualMemory(pMeshId, pAd->MeshTab.MeshId, pAd->MeshTab.MeshIdLen))
		&& (PathSelProtocolId == pAd->MeshTab.PathProtocolId)
		&& (PathSelMetricId == pAd->MeshTab.PathMetricId))
		result = TRUE;

	return result;
}

PMESH_NEIGHBOR_ENTRY InsertNeighborMP(
	IN PRTMP_ADAPTER pAd,
	IN CHAR RealRssi,
	IN PUCHAR pPeerMacAddr,
	IN UCHAR Channel,
	IN UINT8 ChBW,
	IN UINT8 ExtChOffset,
	IN PUCHAR pHostName,
	IN PUCHAR pMeshId,
	IN UCHAR MeshIdLen,
	IN USHORT 	CapabilityInfo,
	IN PUCHAR	pVIE,
	IN USHORT	LenVIE,
	IN PMESH_CONFIGURAION_IE pMeshConfig)
{
	INT i;
	ULONG HashIdx;
	PMESH_NEIGHBOR_TAB pNeighborTab = pAd->MeshTab.pMeshNeighborTab;
	PMESH_NEIGHBOR_ENTRY pNeighborEntry = NULL, pNeighborCurrEntry;

	if(pNeighborTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("pAd->MeshTab.pMeshNeighborTab equal NULL.\n"));
		return NULL;
	}

	pNeighborEntry = NeighborSearch(pAd, pPeerMacAddr);
	if (pNeighborEntry == NULL)
	{
		/* if FULL, return */
		if (pNeighborTab->NeighborNr >= MAX_NEIGHBOR_MP) 
		{
			DBGPRINT(RT_DEBUG_ERROR, ("pAd->MeshTab.pMeshNeighborTab FULL.\n"));
			return NULL;
		}

		for (i = 0; i < MAX_NEIGHBOR_MP; i++)
		{
			pNeighborEntry = &pAd->MeshTab.pMeshNeighborTab->NeighborMP[i];
			if (pNeighborEntry->Valid == FALSE)
				break;
		}

		if (i < MAX_NEIGHBOR_MP)
		{
			ULONG Idx;

			NeighborEntrySet(pNeighborEntry, RealRssi, pPeerMacAddr, (RTMP_STRING *)pHostName, (PCHAR)pMeshId, MeshIdLen,
								Channel, ChBW, ExtChOffset, CapabilityInfo, pVIE, LenVIE, pMeshConfig);
			Idx = GetMeshLinkId(pAd, (PCHAR)pPeerMacAddr);
			if (Idx != BSS_NOT_FOUND)
			{ 
				pNeighborEntry->State = CANDIDATE_MP;
				pNeighborEntry->MeshLinkIdx = Idx;
			}
			else
				pNeighborEntry->State = NEIGHBOR_MP;
				
			pNeighborTab->NeighborNr++;
		}
		else
		{
			pNeighborEntry = NULL;
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Neighbor tab full.\n", __FUNCTION__));
		}

		/* add this Neighbor entry into HASH table */
		if (pNeighborEntry)
		{
			HashIdx = MESH_MAC_ADDR_HASH_INDEX(pPeerMacAddr);
			if (pNeighborTab->Hash[HashIdx] == NULL)
			{
				pNeighborTab->Hash[HashIdx] = pNeighborEntry;
			}
			else
			{
				pNeighborCurrEntry = pNeighborTab->Hash[HashIdx];
				while (pNeighborCurrEntry->pNext != NULL)
					pNeighborCurrEntry = pNeighborCurrEntry->pNext;
				pNeighborCurrEntry->pNext = pNeighborEntry;
			}
		}
	} 
	else
	{
		NeighborEntrySet(pNeighborEntry, RealRssi, pPeerMacAddr, (RTMP_STRING *)pHostName, (PCHAR)pMeshId, MeshIdLen,
							Channel, ChBW, ExtChOffset, CapabilityInfo, pVIE, LenVIE, pMeshConfig);
	}

	return pNeighborEntry;
}

VOID DeleteNeighborMP(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pPeerMacAddr)
{
	PMESH_NEIGHBOR_TAB pNeighborTab = pAd->MeshTab.pMeshNeighborTab;
	PMESH_NEIGHBOR_ENTRY pNeighborEntry = NULL;

	if(pNeighborTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("pAd->MeshTab.pMeshNeighborTab equal NULL.\n"));
		return;
	}

	/* if empty, return */
	if (pNeighborTab->NeighborNr == 0) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("pMeshNeighborTab empty.\n"));
		return;
	}

	pNeighborEntry = NeighborSearch(pAd, pPeerMacAddr);
	if (pNeighborEntry != NULL)
	{
		PMESH_NEIGHBOR_ENTRY pPrevEntry = NULL;
		ULONG HashIdx = MESH_MAC_ADDR_HASH_INDEX(pNeighborEntry->PeerMac);
		PMESH_NEIGHBOR_ENTRY pProbeEntry = pNeighborTab->Hash[HashIdx];

		/* update Hash list */
		do
		{
			if (pProbeEntry == pNeighborEntry)
			{
				if (pPrevEntry == NULL)
				{
					pNeighborTab->Hash[HashIdx] = pNeighborEntry->pNext;
				}
				else
				{
					pPrevEntry->pNext = pNeighborEntry->pNext;
				}
				break;
			}

			pPrevEntry = pProbeEntry;
			pProbeEntry = pProbeEntry->pNext;
		} while (pProbeEntry);

		NdisZeroMemory(pNeighborEntry, sizeof(MESH_NEIGHBOR_ENTRY));
		pNeighborTab->NeighborNr--;
	}

	return;
}

PMESH_NEIGHBOR_ENTRY NeighborSearch(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pPeerMacAddr)
{
	ULONG HashIdx;
	PMESH_NEIGHBOR_ENTRY pNighborEntry = NULL;
	
	HashIdx = MESH_MAC_ADDR_HASH_INDEX(pPeerMacAddr);
	pNighborEntry = pAd->MeshTab.pMeshNeighborTab->Hash[HashIdx];

	while (pNighborEntry)
	{
		if (MAC_ADDR_EQUAL(pNighborEntry->PeerMac, pPeerMacAddr)) 
		{
			break;
		}
		else
			pNighborEntry = pNighborEntry->pNext;
	}

	return pNighborEntry;
}

static VOID NeighborEntrySet(
	OUT PMESH_NEIGHBOR_ENTRY pNeighbor, 
	IN CHAR RealRssi,
	IN PUCHAR pPeerMacAddr, 
	IN RTMP_STRING *pHostName, 
	IN PCHAR pMeshId, 
	IN UCHAR MeshIdLen, 
	IN UCHAR Channel,
	IN UINT8 ChBW,
	IN UINT8 ExtChOffset,
	IN USHORT 	CapabilityInfo,
	IN PUCHAR	pVIE,
	IN USHORT	LenVIE,
	IN PMESH_CONFIGURAION_IE pMeshConfig)
{
	ULONG Now;

	NdisGetSystemUpTime(&Now);

	pNeighbor->Valid = TRUE;
	COPY_MAC_ADDR(pNeighbor->PeerMac, pPeerMacAddr);

	/* record host name. */
	if(strlen(pHostName) > 0 && strlen(pHostName) < MAX_HOST_NAME_LEN)
		strcpy((RTMP_STRING *) pNeighbor->HostName, pHostName);
	else
		strcpy((RTMP_STRING *) pNeighbor->HostName, DEFAULT_MESH_HOST_NAME);

	/* record mesh id. */
	if (MeshIdLen > 0)
	{
		NdisMoveMemory(pNeighbor->MeshId, pMeshId, MAX_MESH_ID_LEN);
		pNeighbor->MeshIdLen = MeshIdLen;
	}

	pNeighbor->RealRssi = RealRssi;
	pNeighbor->Channel = Channel;
	pNeighbor->ChBW = ChBW;
	pNeighbor->ExtChOffset = ExtChOffset;
	pNeighbor->CPI = pMeshConfig->CPI;
	pNeighbor->PathProtocolId = pMeshConfig->PathSelProtocolId;
	pNeighbor->PathMetricId = pMeshConfig->PathSelMetricId;
	pNeighbor->MeshCapability = pMeshConfig->MeshCapability;
	pNeighbor->Version = pMeshConfig->Version;

	pNeighbor->IdleCnt = 0;
	pNeighbor->LastBeaconTime = Now;

	/*pNeighbor->MeshEncrypType = MeshCipher; */
	pNeighbor->CapabilityInfo = CapabilityInfo;
	
	if (LenVIE > 0)
	{
		NdisMoveMemory(pNeighbor->RSNIE, pVIE, LenVIE);
		pNeighbor->RSNIE_Len = LenVIE;
	}
	else
	{
		pNeighbor->RSNIE_Len = 0;
	}

	return;
}

static PMESH_NEIGHBOR_ENTRY NeighMPWithMaxCPI(
	IN PRTMP_ADAPTER pAd,
	IN PMESH_NEIGHBOR_TAB pNeighborTab)
{
	INT i;
	UINT32 MaxCPI = 0;
	PMESH_NEIGHBOR_ENTRY pMaxCpiEntry = NULL;
	PMESH_NEIGHBOR_ENTRY pNeighborEntry = NULL;

	for (i = 0; i < MAX_NEIGHBOR_MP; i++)
	{
		pNeighborEntry = &pAd->MeshTab.pMeshNeighborTab->NeighborMP[i];
		if (pNeighborEntry->Valid == FALSE)
			continue;

		if (!NeighborMPCheck(pAd, pNeighborEntry->MeshId,
				(UINT8)pNeighborEntry->PathProtocolId,
				(UINT8)pNeighborEntry->PathMetricId))
			continue;

		if (MaxCPI < pNeighborEntry->CPI)
		{
			MaxCPI = pNeighborEntry->CPI;
			pMaxCpiEntry = pNeighborEntry;
		}
	}

	return pMaxCpiEntry;
}

VOID NeighborTableUpdate(
	IN PRTMP_ADAPTER pAd)
{
#ifdef RELEASE_EXCLUDE
	/* 1. Update IdleCnt. */
	/* 2. Maintain Tab. remove dead Neighbor MPs. */
#endif /* RELEASE_EXCLUDE */

	INT i;
	PMESH_NEIGHBOR_TAB pNeighborTab = pAd->MeshTab.pMeshNeighborTab;
	PMESH_NEIGHBOR_ENTRY pNeighborEntry = NULL;
	ULONG Now;

	if(pNeighborTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("pAd->MeshTab.pMeshNeighborTab equal NULL.\n"));
		return;
	}

	for (i = 0; i < MAX_NEIGHBOR_MP; i++)
	{
		pNeighborEntry = &pAd->MeshTab.pMeshNeighborTab->NeighborMP[i];
		if (pNeighborEntry->Valid == FALSE)
			continue;

		NdisGetSystemUpTime(&Now);
		/*if ((++pNeighborEntry->IdleCnt > NEIGHBOR_MP_IDLE_CNT)) */
		if(RTMP_TIME_AFTER(Now, pNeighborEntry->LastBeaconTime + (MESH_NEIGHBOR_BEACON_IDLE_TIME * OS_HZ / 1000) ))
		{
			if (MeshValid(&pAd->MeshTab)
				&& (pNeighborEntry->State == CANDIDATE_MP)
				&& (PeerLinkValidCheck(pAd, pNeighborEntry->MeshLinkIdx) == TRUE))
			{
				MlmeEnqueue(pAd, MESH_LINK_MNG_STATE_MACHINE, MESH_LINK_MNG_CNCL, 0, NULL, pNeighborEntry->MeshLinkIdx);
			}
#ifdef RELEASE_EXCLUDE
			/* some neighbor may not the channel as current Mesh network's. */
			/* so don't update such neighbors. */
#endif /* RELEASE_EXCLUDE */
			if ( (pAd->MeshTab.UCGEnable && pNeighborEntry->Channel == pAd->MeshTab.MeshChannel)
				|| !pAd->MeshTab.UCGEnable)
				DeleteNeighborMP(pAd, pNeighborEntry->PeerMac);
		}
		else
		{
			if (VALID_MESH_LINK_ID(pNeighborEntry->MeshLinkIdx))
			{
				if ((pNeighborEntry->State == LINK_AVAILABLE)
					&& (pNeighborEntry->ExtChOffset != pAd->MeshTab.MeshLink[pNeighborEntry->MeshLinkIdx].Entry.ExtChOffset))
				{
					DBGPRINT(RT_DEBUG_TRACE, ("Link%d:Neighbor ExtChOffset change from %d to %d , kill the link!\n"
						,pNeighborEntry->MeshLinkIdx
						,pNeighborEntry->ExtChOffset,pAd->MeshTab.MeshLink[pNeighborEntry->MeshLinkIdx].Entry.ExtChOffset));
					MlmeEnqueue(pAd, MESH_LINK_MNG_STATE_MACHINE, MESH_LINK_MNG_CNCL, 0, NULL, pNeighborEntry->MeshLinkIdx);
					RTMP_MLME_HANDLER(pAd);
				}
			}
		}

	}

	return;
}

VOID CandidateMPSelect(
	IN PRTMP_ADAPTER pAd)
{
#ifdef RELEASE_EXCLUDE
	/* 1. find max CPI. */
	/* 2. select candidate MP. */
#endif /* RELEASE_EXCLUDE */

	INT i;
	PMESH_NEIGHBOR_TAB pNeighborTab = pAd->MeshTab.pMeshNeighborTab;
	PMESH_NEIGHBOR_ENTRY pNeighborEntry = NULL;

	if(pNeighborTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("pAd->MeshTab.pMeshNeighborTab equal NULL.\n"));
		return;
	}

	if ((pAd->MeshTab.CPI == 0)
		&& (pNeighborTab->NeighborNr == 0))
	{
#ifdef RELEASE_EXCLUDE
		/* have no avaiable Neighbor MP. */
		/* 1. Random Generate a CPI value. */
		/* 2. Add those IEs in Beacon. */
#endif /* RELEASE_EXCLUDE */
		pAd->MeshTab.CPI = RandomMeshCPI(pAd);
	}
	else
	{
#ifdef RELEASE_EXCLUDE
		/* 1. find the channel of Neighbor MP which had Max CPI value. */
		/* 2. switch to the channel. */
		/* 3. insert Mesh-IEs into Beacon. */
		/* 4. select candidate MPs. */
#endif /* RELEASE_EXCLUDE */

		pNeighborEntry = NeighMPWithMaxCPI(pAd, pNeighborTab);
		if ((pNeighborEntry != NULL)
			&& (pAd->MeshTab.CPI < pNeighborEntry->CPI))
		{
			if (pAd->CommonCfg.Channel != pNeighborEntry->Channel)
			{
				pAd->MeshTab.CPI = pNeighborEntry->CPI;
				/* start UCG Procedural. */
			}
		}

		if (pAd->MeshTab.MeshAutoLink == FALSE)
			return;

		for (i = 0; i < pNeighborTab->NeighborNr; i++)
		{
			pNeighborEntry = &pNeighborTab->NeighborMP[i];

			if (!NeighborMPCheck(pAd, pNeighborEntry->MeshId,
					(UINT8)pNeighborEntry->PathProtocolId,
					(UINT8)pNeighborEntry->PathMetricId))
				continue;

			DBGPRINT(RT_DEBUG_TRACE, ("(%d) CPI=%d Apl=%d channel=%d state=%d\n", i, pNeighborEntry->CPI,
				pNeighborEntry->MeshCapability.field.AcceptPeerLinks, pNeighborEntry->Channel, pNeighborEntry->State));

			DBGPRINT(RT_DEBUG_TRACE, ("%d %d %d %d %d\n",
				(GetMeshSecurity(pAd)==MeshCheckPeerMpCipher(pNeighborEntry->CapabilityInfo, pNeighborEntry->RSNIE, pNeighborEntry->RSNIE_Len)),
				(pNeighborEntry->MeshCapability.field.AcceptPeerLinks),
				(pNeighborEntry->State == NEIGHBOR_MP), MeshChCheck(pAd, pNeighborEntry),
				((pAd->MeshTab.CPI <= pNeighborEntry->CPI)
					||((pAd->MeshTab.CPI == pNeighborEntry->CPI)
						&& (memcmp(pNeighborEntry->PeerMac, pAd->MeshTab.wdev.if_addr, MAC_ADDR_LEN) < 0)))));

			if ((GetMeshSecurity(pAd)==MeshCheckPeerMpCipher(pNeighborEntry->CapabilityInfo, pNeighborEntry->RSNIE, pNeighborEntry->RSNIE_Len))
				&& (pNeighborEntry->MeshCapability.field.AcceptPeerLinks)
				&& (pNeighborEntry->State == NEIGHBOR_MP)
				&& MeshChCheck(pAd, pNeighborEntry)
				&& ((pAd->MeshTab.CPI <= pNeighborEntry->CPI)
					||((pAd->MeshTab.CPI == pNeighborEntry->CPI)
						&& (memcmp(pNeighborEntry->PeerMac, pAd->MeshTab.wdev.if_addr, MAC_ADDR_LEN) < 0)))
				)
			{
				ULONG LinkIdx = MeshLinkAlloc(pAd, pNeighborEntry->PeerMac, MESH_LINK_DYNAMIC);

				DBGPRINT(RT_DEBUG_TRACE, ("(%d) pick LinkId=%ld\n", i, LinkIdx));

				if (VALID_MESH_LINK_ID(LinkIdx))
				{
					pAd->MeshTab.CPI = pNeighborEntry->CPI;
					pNeighborEntry->State = CANDIDATE_MP;
					pNeighborEntry->MeshLinkIdx = LinkIdx;
				}
			}
		}
	}

	return;
}

void NeighborTableReset(
	IN PRTMP_ADAPTER pAd)
{
	if (pAd->MeshTab.pMeshNeighborTab)
		NdisZeroMemory(pAd->MeshTab.pMeshNeighborTab, sizeof(MESH_NEIGHBOR_TAB));
	else
		DBGPRINT(RT_DEBUG_ERROR, ("pAd->MeshTab.pMeshNeighborTab equal NULL.\n"));

	return;
}

void NeighborTableInit(
	IN PRTMP_ADAPTER pAd)
{
	os_alloc_mem(pAd, (UCHAR **)&(pAd->MeshTab.pMeshNeighborTab), sizeof(MESH_NEIGHBOR_TAB));
	if (pAd->MeshTab.pMeshNeighborTab)
		NdisZeroMemory(pAd->MeshTab.pMeshNeighborTab, sizeof(MESH_NEIGHBOR_TAB));
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s Fail to alloc memory for pAd->MeshTab.pMeshNeighborTab", __FUNCTION__));

	return;
}

void NeighborTableDestroy(
	IN PRTMP_ADAPTER pAd)
{
	if (pAd->MeshTab.pMeshNeighborTab)
		os_free_mem(NULL, pAd->MeshTab.pMeshNeighborTab);
	pAd->MeshTab.pMeshNeighborTab = NULL;

	return;
}

VOID MeshChSwAnnounceProcess(
	IN PRTMP_ADAPTER pAd,
	IN RX_BLK *pRxBlk)
{
	PHEADER_802_11 pHeader = (PHEADER_802_11)pRxBlk->pHeader;
	MESH_CH_SW_ANN_MSG_STRUCT PeerChSwAnn;
	PUCHAR	pDA;
	PUCHAR	pSA;
	UINT32	MeshSeq;
	UINT8	MeshTTL;
	PUCHAR	pFrame;
	ULONG	FrameLen;
	UINT	MeshHdrLen;
	ULONG	LinkIdx;
	UINT8	ChSwMode;
	UINT8	NewCh;
	UINT32	NewCPI;
	UINT8	ChSwCnt;
	UCHAR	MeshSA[MAC_ADDR_LEN];

	DBGPRINT(RT_DEBUG_TRACE, ("-----> %s\n", __FUNCTION__));

	if ((LinkIdx = GetMeshLinkId(pAd, (PCHAR)pHeader->Addr2)) == BSS_NOT_FOUND)
		return;

	if (!PeerLinkValidCheck(pAd, LinkIdx))
		return;

	MeshSeq = GetMeshSeq(pRxBlk->pData);
	MeshTTL = GetMeshTTL(pRxBlk->pData);
	pDA = pHeader->Addr1;
	pSA = GetMeshAddr4(pRxBlk->pData);
	if (pSA == NULL)
		pSA = pHeader->Addr2;
	MeshHdrLen = GetMeshHederLen(pRxBlk->pData);

	/* skip Mesh Header */
	pRxBlk->pData += MeshHdrLen;
	pRxBlk->DataSize -= MeshHdrLen;

	/* skip Category and ActionCode */
	pFrame = (PUCHAR)(pRxBlk->pData + 2);
	FrameLen = pRxBlk->DataSize - 2;

	if (PktSigCheck(pAd, pHeader->Addr2, pHeader->Addr1, pSA, MeshSeq, FC_TYPE_MGMT) == FALSE)
		return;

	MeshChannelSwitchAnnouncementSanity( pAd,
							pFrame,
							FrameLen,
							&ChSwMode,
							&NewCh,
							&NewCPI,
							&ChSwCnt,
							MeshSA);

	PeerChSwAnn.ChSwMode = ChSwMode;
	PeerChSwAnn.NewCh = NewCh;
	PeerChSwAnn.NewCPI = NewCPI;
	PeerChSwAnn.ChSwCnt = ChSwCnt;
	PeerChSwAnn.LinkId = LinkIdx;
	PeerChSwAnn.MeshSeq = MeshSeq;
	PeerChSwAnn.MeshTTL = MeshTTL;
	COPY_MAC_ADDR(PeerChSwAnn.MeshSA, MeshSA);

	MlmeEnqueue(pAd, MESH_CTRL_STATE_MACHINE, MESH_PEER_UCG_EVT, sizeof(MESH_CH_SW_ANN_MSG_STRUCT), &PeerChSwAnn, 0);
	DBGPRINT(RT_DEBUG_TRACE, ("<----- %s\n", __FUNCTION__));
}

static VOID EnqueChSwAnnouncement(
		IN PRTMP_ADAPTER pAd,
		IN PUCHAR PeerMac,
		IN UINT8 MeshTTL,
		IN UINT32 MeshSeq,
		IN UINT8 ChSwMode,
		IN UCHAR NewCh,
		IN UINT32 NewCPI,
		IN UINT8 ChSwCnt,
		IN PUCHAR pMeshSa)
{
	HEADER_802_11 MeshHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen;
	MESH_FLAG MeshFlag;

	NStatus = MlmeAllocateMemory(pAd, (PVOID)&pOutBuffer);  /*Get an unused nonpaged memory */
	if(NStatus != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s() allocate memory failed \n", __FUNCTION__));
		return;
	}

	MeshHeaderInit(pAd, &MeshHdr,
		PeerMac,		/* addr1 */
		pAd->MeshTab.wdev.if_addr,	/* addr2 */
		ZERO_MAC_ADDR);		/* addr3 */
	NdisMoveMemory(pOutBuffer, (PCHAR)&MeshHdr, sizeof(HEADER_802_11));
	FrameLen = sizeof(HEADER_802_11);

	/* Mesh Header */
	MeshFlag.word = 0;
	MeshFlag.field.AE = 0;	/* Peer-Link manager frame never carry 6 addresses. */
	InsertMeshHeader(pAd, (pOutBuffer + FrameLen), &FrameLen, MeshFlag.word,
		MeshTTL, MeshSeq, NULL, NULL, NULL);

	/* Action field */
	InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_RES_COORDINATION, RESOURCE_CHANNEL_SWITCH_ANNOUNCEMENT);

	/* Channel Switch Announcement IE. */
	InsertMeshChSwAnnIE(pAd, (pOutBuffer + FrameLen), &FrameLen, ChSwMode, NewCh, NewCPI, ChSwCnt, pMeshSa);

	MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
	MlmeFreeMemory(pAd, pOutBuffer);

	return;
}

/*
	========================================================================
	
	Routine Description:
		Prior to beginning the MSA authentication mechanism, the MP 
		determines if it is the Selector MP for the duration of the protocol.
		The MP is the Selector MP if its MAC address is numerically larger 
		than that of the candidate peer MP. 

		This routine shall determine if the local MP is Selector MP

	Arguments:

	Return Value:

	Note:
		refer to 802.11s-D1.06	11A.4.2.2

	========================================================================
*/
void ValidateLocalMPAsSelector(
	IN PRTMP_ADAPTER pAd,
	IN INT		 	 idx)
{	
	UCHAR	i = 0;

	/* clear this flag */
	pAd->MeshTab.MeshLink[idx].Entry.bValidLocalMpAsSelector = FALSE;

#ifdef RELEASE_EXCLUDE
	/* Prior to beginning the MSA authentication mechanism, the MP determines if it is the Selector MP for the */
	/* duration of the protocol. The MP is the Selector MP if its MAC address is numerically larger than that of the */
	/* candidate peer MP. */
#endif /* RELEASE_EXCLUDE */
	while(i < MAC_ADDR_LEN)
	{
		if (pAd->MeshTab.wdev.if_addr[i] != pAd->MeshTab.MeshLink[idx].Entry.PeerMacAddr[i])
		{
			if (pAd->MeshTab.wdev.if_addr[i] > pAd->MeshTab.MeshLink[idx].Entry.PeerMacAddr[i])
			{
				pAd->MeshTab.MeshLink[idx].Entry.bValidLocalMpAsSelector = TRUE;
				DBGPRINT(RT_DEBUG_TRACE, ("The local MP is the Selector MP"));
			}
			else
			{
				pAd->MeshTab.MeshLink[idx].Entry.bValidLocalMpAsSelector = FALSE;
				DBGPRINT(RT_DEBUG_TRACE, ("The local MP is NOT the Selector MP"));	
			}

			/* Initial MSAIE */
			LocalMsaIeInit(pAd, idx);
			
			return;
		}		
		i++;
	}

	return;
}

/*
	========================================================================
	
	Routine Description:
		The MP shall perform the 802.1X role selection procedure based on
		the contents of the received peer link open frame and its own 
		configuration. 

		This routine shall determine if the local MP is Authenticator MP

	Arguments:

	Return Value:

	Note:
		refer to 802.11s-D1.06	11A.4.2.2.2

	========================================================================
*/
void ValidateLocalMPAsAuthenticator(
	IN PRTMP_ADAPTER 	pAd,
	IN UCHAR		 	idx,
	IN PUCHAR			pMscIe,
	IN PUCHAR			pMsaIe)
{		
	UCHAR	offset = 0;
	PMESH_LINK_ENTRY pMeshLinkEntry;
	PMESH_SECURITY_CAPABILITY_IE pPeerMSCIE;
	PMSA_HANDSHAKE_IE pPeerMSAIE;
	
	pMeshLinkEntry = &pAd->MeshTab.MeshLink[idx].Entry;
	pPeerMSCIE = (PMESH_SECURITY_CAPABILITY_IE)pMscIe;
	pPeerMSAIE = (PMSA_HANDSHAKE_IE)pMsaIe;
	
	/* clear this flag */
	pMeshLinkEntry->bValidLocalMpAsAuthenticator = FALSE;

#ifdef RELEASE_EXCLUDE
	/* If neither MP has the "Connected to MKD" bit set to 1, */
	/* then the 802.1X Authenticator is the Selector MP. */
#endif /* RELEASE_EXCLUDE */
	if (pPeerMSCIE->MeshSecurityConfig.field.ConnectedToMKD == 0 && 
		pAd->MeshTab.LocalMSCIE.MeshSecurityConfig.field.ConnectedToMKD == 0)
	{
		if (pMeshLinkEntry->bValidLocalMpAsSelector)
			pMeshLinkEntry->bValidLocalMpAsAuthenticator = TRUE;
	}
	/*If both MPs have "Connected to MKD" bit set to 1, then: */
	else if (pPeerMSCIE->MeshSecurityConfig.field.ConnectedToMKD == 1 && 
		pAd->MeshTab.LocalMSCIE.MeshSecurityConfig.field.ConnectedToMKD == 1)
	{
#ifdef RELEASE_EXCLUDE
		/* If both MPs request authentication during this handshake or */
		/* If neither MP requests authentication during this handshake, */
		/* then the 802.1X Authenticator is the Selector MP. */
#endif /* RELEASE_EXCLUDE */
		if ((pPeerMSAIE->MeshHSControl.field.RequestAuth == 1 && pAd->MeshTab.bInitialMsaDone) ||
			(pPeerMSAIE->MeshHSControl.field.RequestAuth == 0 && !pAd->MeshTab.bInitialMsaDone))
		{
			if (pMeshLinkEntry->bValidLocalMpAsSelector)
				pMeshLinkEntry->bValidLocalMpAsAuthenticator = TRUE;
		}
#ifdef RELEASE_EXCLUDE
		/* Otherwise, the MP that requests authentication is the 802.1X Supplicant, */
		/* and the other MP is the 802.1X Authenticator */
#endif /* RELEASE_EXCLUDE */
		else
		{
			if (!pAd->MeshTab.bInitialMsaDone)
				pMeshLinkEntry->bValidLocalMpAsAuthenticator = TRUE;
		}
	}
	else
	{
#ifdef RELEASE_EXCLUDE
		/* If only one MP has the "Connected to MKD" bit set to 1, */
		/* then that MP is the 802.1X Authenticator. */
#endif /* RELEASE_EXCLUDE */
		if (pAd->MeshTab.LocalMSCIE.MeshSecurityConfig.field.ConnectedToMKD == 1)
			pMeshLinkEntry->bValidLocalMpAsAuthenticator = TRUE;
	}

	offset += sizeof(MESH_HANDSHAKE_CONTROL);
	
	/* Once the Authenticator MP is decided, fill in MA-ID of MSAIE for local MP */
	if (pMeshLinkEntry->bValidLocalMpAsAuthenticator)
		NdisMoveMemory(&pMeshLinkEntry->LocalMsaIe[offset], pAd->MeshTab.wdev.if_addr, MAC_ADDR_LEN);
	else
		NdisMoveMemory(&pMeshLinkEntry->LocalMsaIe[offset], pMeshLinkEntry->PeerMacAddr, MAC_ADDR_LEN);
	
	return;
}

#ifdef RELEASE_EXCLUDE
/*
	The local MP shall perform the key selection procedure based on the contents of the peer link open frame.
	The result of the procedure determines if a PMK-MA is available to be used to secure the link, or if Initial
	MSA Authentication must occur.

 */
#endif /* RELEASE_EXCLUDE */

BOOLEAN MeshKeySelectionAction(
	IN PRTMP_ADAPTER 		pAd,
	IN PMESH_LINK_ENTRY  	pMeshLinkEntry,
	IN PUCHAR				pMscIe,
	IN PUCHAR				pPmkId,
	IN UCHAR				PmkIdLen)
{	
	PMESH_SECURITY_CAPABILITY_IE pPeerMSCIE;		
	BOOLEAN				bValidLocalKey = FALSE;
	BOOLEAN				bCachedPeerKey = FALSE;

	if (pAd->MeshTab.EasyMeshSecurity)
	{
		pMeshLinkEntry->MeshKeySelection = MESH_KEY_NONE;
		return TRUE;
	}

	pPeerMSCIE = (PMESH_SECURITY_CAPABILITY_IE)pMscIe;	
			
	/* The key selection procedure first determines */
	/* if Initial MSA Authentication shall occur. */
	if (PmkIdLen == 0 || 				
		pAd->MeshTab.bInitialMsaDone == FALSE || 						
		pAd->MeshTab.PMKID_Len == 0 ||
		!NdisEqualMemory(pPeerMSCIE->MKDDID, pAd->MeshTab.LocalMSCIE.MKDDID, MAC_ADDR_LEN))						
	{
		pMeshLinkEntry->MeshKeySelection = MESH_KEY_NONE;
		return TRUE;
	}
	
#ifdef RELEASE_EXCLUDE
	/* Check Valid-local-key */
	/* The table input Valid-local-key is set to true if PMK-MAName(receiver), contained in the PMKID list field */
	/* in the RSNIE of the received peer link open frame, identifies the PMK-MA belonging to the local MP's key */
	/* hierarchy that is currently valid for securing the link with the peer MP; otherwise, and when there is only */
	/* one PMK-MAName entry, it is false. */
#endif /* RELEASE_EXCLUDE */
	if (PmkIdLen == MESH_MAX_PMKID_LEN)
	{
		if (NdisEqualMemory(pMeshLinkEntry->PMK_MA_NAME, pPmkId + LEN_PMKID, MESH_PMK_NAME_LEN))
			bValidLocalKey = TRUE;						
	}
	
#ifdef RELEASE_EXCLUDE
	/* Check Cached-peer-key */
	/* The table input Cached-peer-key is set to true if the key named by PMK-MAName(sender), contained in the */
	/* PMKID list field in the RSNIE of the received peer link open frame, is cached by the MA function of the */
	/* local MP and is currently valid for securing the link. Otherwise, it is false. */
#endif /* RELEASE_EXCLUDE */
	if (PmkIdLen >= LEN_PMKID && pAd->MeshTab.bKeyholderDone)
	{
		if (NdisEqualMemory(pMeshLinkEntry->PMK_MA_NAME, pPmkId, MESH_PMK_NAME_LEN))
			bCachedPeerKey = TRUE;	
	}

	/* The key selection procedure, it refers to IEEE 802.11s/D1.06 Table-s46 */
	if (bValidLocalKey == FALSE && bCachedPeerKey == FALSE)
	{
		/* No PMK-MA available and no connection to MKD available, it shall close this link */
		if (pPeerMSCIE->MeshSecurityConfig.field.ConnectedToMKD == 0 && 
			pAd->MeshTab.LocalMSCIE.MeshSecurityConfig.field.ConnectedToMKD == 0)
		{
			return FALSE;
		}
		/* Local MP connected to MKD, but peer MP not */
		else if (pPeerMSCIE->MeshSecurityConfig.field.ConnectedToMKD == 0 && 
			pAd->MeshTab.LocalMSCIE.MeshSecurityConfig.field.ConnectedToMKD == 1)
		{
			pMeshLinkEntry->MeshKeySelection = MESH_KEY_PMKMA_PEER;
		}
		/* Peer MP connected to MKD, but local MP not */
		else if (pPeerMSCIE->MeshSecurityConfig.field.ConnectedToMKD == 1 && 
			pAd->MeshTab.LocalMSCIE.MeshSecurityConfig.field.ConnectedToMKD == 0)
		{
			pMeshLinkEntry->MeshKeySelection = MESH_KEY_PMKMA_LOCAL;			
		}
		/* Peer and local MP both connected to MKD */
		else
		{
			if (pMeshLinkEntry->bValidLocalMpAsSelector)
				pMeshLinkEntry->MeshKeySelection = MESH_KEY_PMKMA_PEER;
			else
				pMeshLinkEntry->MeshKeySelection = MESH_KEY_PMKMA_LOCAL;
		}

	}
	else if (bValidLocalKey == FALSE && bCachedPeerKey == TRUE)
	{
		pMeshLinkEntry->MeshKeySelection = MESH_KEY_PMKMA_PEER;
	}
	else if (bValidLocalKey == TRUE && bCachedPeerKey == FALSE)
	{
		pMeshLinkEntry->MeshKeySelection = MESH_KEY_PMKMA_LOCAL;
	}
	else
	{
		if (pMeshLinkEntry->bValidLocalMpAsSelector)
			pMeshLinkEntry->MeshKeySelection = MESH_KEY_PMKMA_PEER;
		else
			pMeshLinkEntry->MeshKeySelection = MESH_KEY_PMKMA_LOCAL;
	}	

	return TRUE;
}

#endif /* MESH_SUPPORT */

