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
	apcli_assoc.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2006-6-23		modified for rt61-APClinent
*/

#ifdef P2P_SUPPORT

#include "rt_config.h"

static VOID ApCliAssocTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

static VOID ApCliMlmeAssocReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

static VOID ApCliMlmeDisassocReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

static VOID ApCliPeerAssocRspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

static VOID ApCliPeerDisassocAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

static VOID ApCliAssocTimeoutAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

static VOID ApCliInvalidStateWhenAssoc(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

static VOID ApCliInvalidStateWhenDisassociate(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

static VOID ApCliAssocPostProc(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pAddr2, 
	IN USHORT CapabilityInfo, 
	IN USHORT IfIndex, 
	IN UCHAR SupRate[], 
	IN UCHAR SupRateLen,
	IN UCHAR ExtRate[],
	IN UCHAR ExtRateLen,
	IN PEDCA_PARM pEdcaParm,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN UCHAR HtCapabilityLen, 
	IN ADD_HT_INFO_IE *pAddHtInfo);

DECLARE_TIMER_FUNCTION(ApCliAssocTimeout);
BUILD_TIMER_FUNCTION(ApCliAssocTimeout);

/*  
    ==========================================================================
    Description: 
        association state machine init, including state transition and timer init
    Parameters: 
        S - pointer to the association state machine
    Note:
        The state machine looks like the following 
    ==========================================================================
 */
VOID ApCliAssocStateMachineInit(
	IN	PRTMP_ADAPTER	pAd, 
	IN  STATE_MACHINE *S, 
	OUT STATE_MACHINE_FUNC Trans[]) 
{
	UCHAR i;

	StateMachineInit(S, (STATE_MACHINE_FUNC*)Trans,
		APCLI_MAX_ASSOC_STATE, APCLI_MAX_ASSOC_MSG,
		(STATE_MACHINE_FUNC)Drop, APCLI_ASSOC_IDLE,
		APCLI_ASSOC_MACHINE_BASE);

	/* first column */
	StateMachineSetAction(S, APCLI_ASSOC_IDLE, APCLI_MT2_MLME_ASSOC_REQ, (STATE_MACHINE_FUNC)ApCliMlmeAssocReqAction);
	StateMachineSetAction(S, APCLI_ASSOC_IDLE, APCLI_MT2_MLME_DISASSOC_REQ, (STATE_MACHINE_FUNC)ApCliMlmeDisassocReqAction);
	StateMachineSetAction(S, APCLI_ASSOC_IDLE, APCLI_MT2_PEER_DISASSOC_REQ, (STATE_MACHINE_FUNC)ApCliPeerDisassocAction);
   
	/* second column */
	StateMachineSetAction(S, APCLI_ASSOC_WAIT_RSP, APCLI_MT2_MLME_ASSOC_REQ, (STATE_MACHINE_FUNC)ApCliInvalidStateWhenAssoc);
	StateMachineSetAction(S, APCLI_ASSOC_WAIT_RSP, APCLI_MT2_MLME_DISASSOC_REQ, (STATE_MACHINE_FUNC)ApCliInvalidStateWhenDisassociate);
	StateMachineSetAction(S, APCLI_ASSOC_WAIT_RSP, APCLI_MT2_PEER_DISASSOC_REQ, (STATE_MACHINE_FUNC)ApCliPeerDisassocAction);
	StateMachineSetAction(S, APCLI_ASSOC_WAIT_RSP, APCLI_MT2_PEER_ASSOC_RSP, (STATE_MACHINE_FUNC)ApCliPeerAssocRspAction);
	StateMachineSetAction(S, APCLI_ASSOC_WAIT_RSP, APCLI_MT2_ASSOC_TIMEOUT, (STATE_MACHINE_FUNC)ApCliAssocTimeoutAction);

	/* timer init */
	RTMPInitTimer(pAd, &pAd->ApCfg.ApCliTab[0].MlmeAux.ApCliAssocTimer, GET_TIMER_FUNCTION(ApCliAssocTimeout), pAd, FALSE);

	for (i=0; i < MAX_APCLI_NUM; i++)
		pAd->ApCfg.ApCliTab[i].AssocCurrState = APCLI_ASSOC_IDLE;

	return;
}

/*
    ==========================================================================
    Description:
        Association timeout procedure. After association timeout, this function 
        will be called and it will put a message into the MLME queue
    Parameters:
        Standard timer parameters
    ==========================================================================
 */
static VOID ApCliAssocTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;

	DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - enqueue APCLI_MT2_ASSOC_TIMEOUT \n"));

	MlmeEnqueue(pAd, APCLI_ASSOC_STATE_MACHINE, APCLI_MT2_ASSOC_TIMEOUT, 0, NULL, 0);
	RTMP_MLME_HANDLER(pAd);

	return;
}

/*
    ==========================================================================
    Description:
        mlme assoc req handling procedure
    Parameters:
        Adapter - Adapter pointer
        Elem - MLME Queue Element
    Pre:
        the station has been authenticated and the following information is stored in the config
            -# SSID
            -# supported rates and their length
            -# listen interval (Adapter->PortCfg.default_listen_count)
            -# Transmit power  (Adapter->PortCfg.tx_power)
    Post  :
        -# An association request frame is generated and sent to the air
        -# Association timer starts
        -# Association state -> ASSOC_WAIT_RSP
        
    ==========================================================================
 */
static VOID ApCliMlmeAssocReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem) 
{
	NDIS_STATUS		 NStatus;
	BOOLEAN          Cancelled;
	UCHAR            ApAddr[6];
	HEADER_802_11    AssocHdr;
	UCHAR            WmeIe[9] = {IE_VENDOR_SPECIFIC, 0x07, 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x00};
	USHORT           ListenIntv;
	ULONG            Timeout;
	USHORT           CapabilityInfo;
	PUCHAR           pOutBuffer = NULL;
	ULONG            FrameLen = 0;
	ULONG            tmp;
	UCHAR            SsidIe    = IE_SSID;
	UCHAR            SupRateIe = IE_SUPP_RATES;
	UCHAR            ExtRateIe = IE_EXT_SUPP_RATES;
	APCLI_CTRL_MSG_STRUCT ApCliCtrlMsg;
	USHORT ifIndex = (USHORT)(Elem->Priv);
	PULONG pCurrState = &pAd->ApCfg.ApCliTab[ifIndex].AssocCurrState;
	APCLI_STRUCT *apcli_entry;
	struct wifi_dev *wdev;



	if (ifIndex >= MAX_APCLI_NUM)
		return;

	apcli_entry = &pAd->ApCfg.ApCliTab[ifIndex];
	wdev = &apcli_entry->wdev;


	
	/* Block all authentication request durning WPA block period */
	if (apcli_entry->bBlockAssoc == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - Block Auth request durning WPA block period!\n"));
		*pCurrState = APCLI_ASSOC_IDLE;
		ApCliCtrlMsg.Status = MLME_STATE_MACHINE_REJECT;
		MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_ASSOC_RSP,
			sizeof(APCLI_CTRL_MSG_STRUCT), &ApCliCtrlMsg, ifIndex);
	}
	else if(MlmeAssocReqSanity(pAd, Elem->Msg, Elem->MsgLen, ApAddr, &CapabilityInfo, &Timeout, &ListenIntv))
	{
		RTMPCancelTimer(&apcli_entry->MlmeAux.ApCliAssocTimer, &Cancelled);

		/* allocate and send out AssocRsp frame */
		NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);  /*Get an unused nonpaged memory */
		if (NStatus != NDIS_STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - ApCliMlmeAssocReqAction() allocate memory failed \n"));
			*pCurrState = APCLI_ASSOC_IDLE;

			ApCliCtrlMsg.Status = MLME_FAIL_NO_RESOURCE;
			MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_ASSOC_RSP,
				sizeof(APCLI_CTRL_MSG_STRUCT), &ApCliCtrlMsg, ifIndex);

			return;
		}

		DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - Send ASSOC request...\n"));
		ApCliMgtMacHeaderInit(pAd, &AssocHdr, SUBTYPE_ASSOC_REQ, 0, ApAddr, ApAddr, ifIndex);

		/* Build basic frame first */
		MakeOutgoingFrame(pOutBuffer,               &FrameLen,
			sizeof(HEADER_802_11),    &AssocHdr,
			2,                        &CapabilityInfo,
			2,                        &ListenIntv,
			1,                        &SsidIe,
			1,                        &apcli_entry->MlmeAux.SsidLen, 
			apcli_entry->MlmeAux.SsidLen,     apcli_entry->MlmeAux.Ssid,
			1,                        &SupRateIe,
			1,                        &apcli_entry->MlmeAux.SupRateLen,
			apcli_entry->MlmeAux.SupRateLen,  apcli_entry->MlmeAux.SupRate,
			END_OF_ARGS);

		if(apcli_entry->MlmeAux.ExtRateLen != 0)
		{
			MakeOutgoingFrame(pOutBuffer + FrameLen,    &tmp,
				1,                        &ExtRateIe,
				1,                        &apcli_entry->MlmeAux.ExtRateLen,
				apcli_entry->MlmeAux.ExtRateLen,  apcli_entry->MlmeAux.ExtRate,
				END_OF_ARGS);
			FrameLen += tmp;
		}

#ifdef DOT11_N_SUPPORT
		/* HT */
		if ((apcli_entry->MlmeAux.HtCapabilityLen > 0) && 
			WMODE_CAP_N(pAd->CommonCfg.PhyMode))
		{
			ULONG TmpLen;
#ifdef RT_BIG_ENDIAN
		        HT_CAPABILITY_IE HtCapabilityTmp;
#endif			

#ifndef RT_BIG_ENDIAN
			{
				MakeOutgoingFrame(pOutBuffer + FrameLen, &TmpLen,
							1, &HtCapIe,
							1, &pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.HtCapabilityLen,
							pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.HtCapabilityLen, &pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.HtCapability, 
							END_OF_ARGS);
			}
#else
                	NdisZeroMemory(&HtCapabilityTmp, sizeof(HT_CAPABILITY_IE));
               	 	NdisMoveMemory(&HtCapabilityTmp, &apcli_entry->MlmeAux.HtCapability, apcli_entry->MlmeAux.HtCapabilityLen);
        		*(USHORT *)(&HtCapabilityTmp.HtCapInfo) = SWAP16(*(USHORT *)(&HtCapabilityTmp.HtCapInfo));
        		*(USHORT *)(&HtCapabilityTmp.ExtHtCapInfo) = SWAP16(*(USHORT *)(&HtCapabilityTmp.ExtHtCapInfo));
        		MakeOutgoingFrame(pOutBuffer + FrameLen,         &TmpLen,
        							1,                           &HtCapIe,
        							1,                           &apcli_entry->MlmeAux.HtCapabilityLen,
        							apcli_entry->MlmeAux.HtCapabilityLen, &HtCapabilityTmp, 
        							END_OF_ARGS);
#endif
			FrameLen += TmpLen;
		}
#endif /* DOT11_N_SUPPORT */

#ifdef AGGREGATION_SUPPORT
		/*
			add Ralink proprietary IE to inform AP this STA is going to use AGGREGATION or PIGGY-BACK+AGGREGATION
			Case I: (Aggregation + Piggy-Back)
				1. user enable aggregation, AND
				2. Mac support piggy-back
				3. AP annouces it's PIGGY-BACK+AGGREGATION-capable in BEACON
			Case II: (Aggregation)
				1. user enable aggregation, AND
				2. AP annouces it's AGGREGATION-capable in BEACON
		*/
		if (pAd->CommonCfg.bAggregationCapable)
		{
#ifdef PIGGYBACK_SUPPORT
			if ((pAd->CommonCfg.bPiggyBackCapable) && ((apcli_entry->MlmeAux.APRalinkIe & 0x00000003) == 3))
			{
				ULONG TmpLen;
				UCHAR RalinkIe[9] = {IE_VENDOR_SPECIFIC, 7, 0x00, 0x0c, 0x43, 0x03, 0x00, 0x00, 0x00}; 
				MakeOutgoingFrame(pOutBuffer+FrameLen,           &TmpLen,
								  9,                             RalinkIe,
								  END_OF_ARGS);
				FrameLen += TmpLen;
			} else
#endif /* PIGGYBACK_SUPPORT */
			if (apcli_entry->MlmeAux.APRalinkIe & 0x00000001)
			{
				ULONG TmpLen;
				UCHAR RalinkIe[9] = {IE_VENDOR_SPECIFIC, 7, 0x00, 0x0c, 0x43, 0x01, 0x00, 0x00, 0x00}; 
				MakeOutgoingFrame(pOutBuffer+FrameLen,           &TmpLen,
								  9,                             RalinkIe,
								  END_OF_ARGS);
				FrameLen += TmpLen;
			}
		}
		else
		{
			ULONG TmpLen;
			UCHAR RalinkIe[9] = {IE_VENDOR_SPECIFIC, 7, 0x00, 0x0c, 0x43, 0x06, 0x00, 0x00, 0x00}; 
			MakeOutgoingFrame(pOutBuffer+FrameLen,		 &TmpLen,
							  9,						 RalinkIe,
							  END_OF_ARGS);
			FrameLen += TmpLen;
		}
#endif  /* AGGREGATION_SUPPORT */

		if (apcli_entry->MlmeAux.APEdcaParm.bValid)
		{
			if (apcli_entry->wdev.UapsdInfo.bAPSDCapable &&
				apcli_entry->MlmeAux.APEdcaParm.bAPSDCapable)
			{
				QBSS_STA_INFO_PARM QosInfo;

				NdisZeroMemory(&QosInfo, sizeof(QBSS_STA_INFO_PARM));
				QosInfo.UAPSD_AC_BE = pAd->CommonCfg.bAPSDAC_BE;
				QosInfo.UAPSD_AC_BK = pAd->CommonCfg.bAPSDAC_BK;
				QosInfo.UAPSD_AC_VI = pAd->CommonCfg.bAPSDAC_VI;
				QosInfo.UAPSD_AC_VO = pAd->CommonCfg.bAPSDAC_VO;
				QosInfo.MaxSPLength = pAd->CommonCfg.MaxSPLength;
				WmeIe[8] |= *(PUCHAR)&QosInfo;
			}
			else
			{
                /* The Parameter Set Count is set to ¡§0¡¨ in the association request frames */
                /* WmeIe[8] |= (pAd->MlmeAux.APEdcaParm.EdcaUpdateCount & 0x0f); */
			}

			MakeOutgoingFrame(pOutBuffer + FrameLen,    &tmp,
							  9,                        &WmeIe[0],
							  END_OF_ARGS);
			FrameLen += tmp;
		}

		/* Append RSN_IE when WPAPSK OR WPA2PSK, */
		if (((wdev->AuthMode == Ndis802_11AuthModeWPAPSK) || 
			(wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)
#ifdef WPA3_SUPPORT
			|| (wdev->AuthMode == Ndis802_11AuthModeWPA3SAE)
#endif
			)
#ifdef WSC_AP_SUPPORT
			&& (apcli_entry->WscControl.WscConfMode == WSC_DISABLE)
#endif /* WSC_AP_SUPPORT */
            )
		{
			UCHAR RSNIe = IE_WPA;
			
			if ((wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)
#ifdef WPA3_SUPPORT
			|| (wdev->AuthMode == Ndis802_11AuthModeWPA3SAE)
#endif
			)
				RSNIe = IE_WPA2;
#ifdef WPA3_SUPPORT
			RTMPMakeRSNIE(pAd, wdev->AuthMode, wdev->WepStatus, (ifIndex + MIN_NET_DEVICE_FOR_APCLI));
#endif
			MakeOutgoingFrame(pOutBuffer + FrameLen,    				&tmp,
			              	1,                                      	&RSNIe,
	                        1,                                      	&pAd->ApCfg.ApCliTab[ifIndex].RSNIE_Len,
	                        pAd->ApCfg.ApCliTab[ifIndex].RSNIE_Len,		pAd->ApCfg.ApCliTab[ifIndex].RSN_IE,
	                        END_OF_ARGS);
			
			FrameLen += tmp;	
		}

#ifdef WSC_AP_SUPPORT
		if (pAd->ApCfg.ApCliTab[ifIndex].WscControl.WscConfMode != WSC_DISABLE)
		{
			UCHAR *pWscBuf = NULL, WscIeLen = 0;
			ULONG WscTmpLen = 0;

			os_alloc_mem(pAd, (UCHAR **) &pWscBuf, 512);
			if (pWscBuf != NULL) {
				NdisZeroMemory(pWscBuf, 512);
				WscBuildAssocReqIE(&pAd->ApCfg.ApCliTab[ifIndex].WscControl, pWscBuf, &WscIeLen);

				MakeOutgoingFrame(pOutBuffer + FrameLen, &WscTmpLen, 
								  WscIeLen, pWscBuf, 
								  END_OF_ARGS);

				FrameLen += WscTmpLen;
				os_free_mem(NULL, pWscBuf);
			} else
				DBGPRINT(RT_DEBUG_WARN,("%s:: WscBuf Allocate failed!\n", __FUNCTION__));
		}
#endif /* WSC_AP_SUPPORT */

#ifdef P2P_SUPPORT
	if (P2P_CLI_ON(pAd))
	{
		ULONG TmpLen;
		PUCHAR pData;
		pData = pOutBuffer + FrameLen;
		P2pMakeP2pIE(pAd, SUBTYPE_ASSOC_REQ, pData, &TmpLen);
		FrameLen += TmpLen;
		DBGPRINT(RT_DEBUG_TRACE, ("ASSOC RSP - Insert P2P IE \n"));
	}

#ifdef WFD_SUPPORT
	{
		PUCHAR	pData;
		ULONG	WfdIeLen = 0;
//iversonnote
		pData = pOutBuffer + FrameLen;
		WfdMakeWfdIE(pAd, SUBTYPE_ASSOC_REQ, pData, &WfdIeLen);
		FrameLen += WfdIeLen;
	}
#endif /* WFD_SUPPORT */

#ifdef WFA_WFD_SUPPORT
	if (pAd->P2pCfg.bWIDI)
	{
		if (pAd->pWfdIeInAssocReq && (pAd->WfdIeInAssocReqLen != 0))
		{
			ULONG WfdIeTmpLen = 0;

			MakeOutgoingFrame(pOutBuffer+FrameLen,	&WfdIeTmpLen,
				  pAd->WfdIeInAssocReqLen,	pAd->pWfdIeInAssocReq,
				  END_OF_ARGS);
			FrameLen += WfdIeTmpLen;
		}
	}
#endif /* WFA_WFD_SUPPORT */
#endif /* P2P_SUPPORT */

		MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
		MlmeFreeMemory(pAd, pOutBuffer);

		RTMPSetTimer(&apcli_entry->MlmeAux.ApCliAssocTimer, Timeout);
		*pCurrState = APCLI_ASSOC_WAIT_RSP;
	} 
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - ApCliMlmeAssocReqAction() sanity check failed. BUG!!!!!! \n"));
		*pCurrState = APCLI_ASSOC_IDLE;

		ApCliCtrlMsg.Status = MLME_INVALID_FORMAT;
		MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_ASSOC_RSP,
			sizeof(APCLI_CTRL_MSG_STRUCT), &ApCliCtrlMsg, ifIndex);
	}

	return;
}

/*
    ==========================================================================
    Description:
        Upper layer issues disassoc request
    Parameters:
        Elem -
    ==========================================================================
 */
static VOID ApCliMlmeDisassocReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem) 
{
	PMLME_DISASSOC_REQ_STRUCT pDisassocReq;
	HEADER_802_11         DisassocHdr;
	PUCHAR                 pOutBuffer = NULL;
	ULONG                 FrameLen = 0;
	NDIS_STATUS           NStatus;
	APCLI_CTRL_MSG_STRUCT ApCliCtrlMsg;
	USHORT ifIndex = (USHORT)(Elem->Priv);
	PULONG pCurrState = &pAd->ApCfg.ApCliTab[ifIndex].AssocCurrState;


	/* skip sanity check */
	pDisassocReq = (PMLME_DISASSOC_REQ_STRUCT)(Elem->Msg);

	/* allocate and send out DeassocReq frame */
	NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);  /*Get an unused nonpaged memory */
	if (NStatus != NDIS_STATUS_SUCCESS) 
	{
		DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - ApCliMlmeDisassocReqAction() allocate memory failed\n"));
		*pCurrState = APCLI_ASSOC_IDLE;

		ApCliCtrlMsg.Status = MLME_FAIL_NO_RESOURCE;
		MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_DEASSOC_RSP,
			sizeof(APCLI_CTRL_MSG_STRUCT), &ApCliCtrlMsg, ifIndex);
		return;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - Send DISASSOC request [BSSID::%02x:%02x:%02x:%02x:%02x:%02x] \n", 
				pDisassocReq->Addr[0], pDisassocReq->Addr[1], pDisassocReq->Addr[2],
				pDisassocReq->Addr[3], pDisassocReq->Addr[4], pDisassocReq->Addr[5]));
	ApCliMgtMacHeaderInit(pAd, &DisassocHdr, SUBTYPE_DISASSOC, 0, pDisassocReq->Addr, pDisassocReq->Addr, ifIndex);
	MakeOutgoingFrame(pOutBuffer,				&FrameLen, 
						sizeof(HEADER_802_11),	&DisassocHdr, 
						2,						&pDisassocReq->Reason, 
						END_OF_ARGS);
	MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
	MlmeFreeMemory(pAd, pOutBuffer);

	/* Set the control aux SSID to prevent it reconnect to old SSID */
	/* Since calling this indicate user don't want to connect to that SSID anymore. */
	/* 2004-11-10 can't reset this info, cause it may be the new SSID that user requests for */
	/* pAd->MlmeAux.SsidLen = MAX_LEN_OF_SSID; */
	/* NdisZeroMemory(pAd->MlmeAux.Ssid, MAX_LEN_OF_SSID); */
	/* NdisZeroMemory(pAd->MlmeAux.Bssid, MAC_ADDR_LEN); */

	//pAd->PortCfg.DisassocReason = REASON_DISASSOC_STA_LEAVING; */
	//COPY_MAC_ADDR(pAd->PortCfg.DisassocSta, pDisassocReq->Addr); */


    *pCurrState = APCLI_ASSOC_IDLE;

	ApCliCtrlMsg.Status = MLME_SUCCESS;
	MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_DEASSOC_RSP,
		sizeof(APCLI_CTRL_MSG_STRUCT), &ApCliCtrlMsg, ifIndex);

	return;
}


/*
    ==========================================================================
    Description:
        peer sends assoc rsp back
    Parameters:
        Elme - MLME message containing the received frame
    ==========================================================================
 */
static VOID ApCliPeerAssocRspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem) 
{
	BOOLEAN				Cancelled;
	USHORT				CapabilityInfo, Status, Aid;
	UCHAR				SupRate[MAX_LEN_OF_SUPPORTED_RATES], SupRateLen;
	UCHAR				ExtRate[MAX_LEN_OF_SUPPORTED_RATES], ExtRateLen;
	UCHAR				Addr2[MAC_ADDR_LEN];
	EDCA_PARM			EdcaParm;
	UCHAR				CkipFlag;
	APCLI_CTRL_MSG_STRUCT	ApCliCtrlMsg;
	HT_CAPABILITY_IE	HtCapability;
	ADD_HT_INFO_IE		AddHtInfo;	/* AP might use this additional ht info IE */
	UCHAR				HtCapabilityLen;
	UCHAR				AddHtInfoLen;
	UCHAR				NewExtChannelOffset = 0xff;
	USHORT ifIndex = (USHORT)(Elem->Priv);
	PULONG pCurrState = &pAd->ApCfg.ApCliTab[ifIndex].AssocCurrState;
	PAPCLI_STRUCT pApCliEntry = NULL;
	ULONG	P2PSubelementLen = 0;
	UCHAR	*P2pSubelement;
#ifdef WPA3_SUPPORT
	P_WLAN_ASSOC_RSP_FRAME_T prAssocRspFrame = NULL;
#endif

	os_alloc_mem(NULL, (UCHAR **)&P2pSubelement, MAX_VIE_LEN);

	if (ApCliPeerAssocRspSanity(pAd, Elem->Msg, Elem->MsgLen, Addr2, &CapabilityInfo, &Status, &Aid, &P2PSubelementLen, P2pSubelement, SupRate, &SupRateLen, ExtRate, &ExtRateLen, 
		&HtCapability, &AddHtInfo, &HtCapabilityLen,&AddHtInfoLen,&NewExtChannelOffset, &EdcaParm, &CkipFlag))
	{
		/* The frame is for me ? */
		if(MAC_ADDR_EQUAL(Addr2, pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - receive ASSOC_RSP to me (status=%d)\n", Status));
			RTMPCancelTimer(&pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.ApCliAssocTimer, &Cancelled);

			pApCliEntry = &pAd->ApCfg.ApCliTab[ifIndex];
			if (P2PSubelementLen > 0)
			{
				/*UCHAR P2pIdx = P2P_NOT_FOUND;
				ULONG TmpLen;
				PUCHAR pData;*/
				pApCliEntry->bP2pClient = TRUE;
			}
			else
				pApCliEntry->bP2pClient = FALSE;

			DBGPRINT(RT_DEBUG_TRACE, ("%s:: recv peer ASSOC RSP from %02x:%02x:%02x:%02x:%02x:%02x.    bP2pClient = %d\n", __FUNCTION__, PRINT_MAC(Addr2), pApCliEntry->bP2pClient));

#ifdef WPA3_SUPPORT
			/* The BSS from cfg80211_ops.assoc must give back to
			 * cfg80211_send_rx_assoc() or to cfg80211_assoc_timeout().
			 * To ensure proper refcounting, new association requests
			 * while already associating must be rejected.
			 */
			DBGPRINT(RT_DEBUG_TRACE, ("Report RX Assoc to upper layer, %s\n", pApCliEntry->ConnInfo.bss ? "DO IT" : "Oops"));

			if (pApCliEntry->ConnInfo.bss) {
				/*Report Rx assoc frame to upper layer*/
				prAssocRspFrame = (P_WLAN_ASSOC_RSP_FRAME_T) Elem->Msg;

#if KERNEL_VERSION(3, 18, 0) <= CFG80211_VERSION_CODE
				/* [TODO] Set uapsd_queues field to zero first, fill it if needed  */
				cfg80211_rx_assoc_resp(pApCliEntry->wdev.if_dev, pApCliEntry->ConnInfo.bss,
					(const u8 *)prAssocRspFrame, Elem->MsgLen, 0);
#else
				cfg80211_rx_assoc_resp(pApCliEntry->wdev.if_dev, pApCliEntry->ConnInfo.bss,
					(const u8 *)prAssocRspFrame, Elem->MsgLen);
#endif
				DBGPRINT(RT_DEBUG_TRACE, ("Report RX Assoc to upper layer, Done\n"));
				pApCliEntry->ConnInfo.bss = NULL;
			} else
				DBGPRINT(RT_DEBUG_TRACE, ("Rx Assoc Resp without specific BSS\n"));
#endif

			if(Status == MLME_SUCCESS) 
			{
#ifdef DPA_S
#ifdef P2P_SUPPORT
				UCHAR p2pIndex = P2P_NOT_FOUND;

				p2pIndex = P2pGroupTabSearch(pAd, pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid);
				if (p2pIndex != P2P_NOT_FOUND)
				{
					PRT_P2P_CLIENT_ENTRY pP2pEntry = &pAd->P2pTable.Client[p2pIndex];
					P2pSendSamsungEvent(pAd, P2P_NOTIF_SOFTAP_STA_ASSOC, pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid, pP2pEntry->DeviceName, pP2pEntry->DeviceNameLen);
				}
				else
					P2pSendSamsungEvent(pAd, P2P_NOTIF_SOFTAP_STA_ASSOC, pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid, NULL, 0);
#endif /* P2P_SUPPORT */
#endif /* DPA_S */
				/* go to procedure listed on page 376 */
				ApCliAssocPostProc(pAd, Addr2, CapabilityInfo, ifIndex, SupRate, SupRateLen,
					ExtRate, ExtRateLen, &EdcaParm, &HtCapability, HtCapabilityLen, &AddHtInfo);  	

				pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Aid=Aid;

				ApCliCtrlMsg.Status = MLME_SUCCESS;
				MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_ASSOC_RSP,
					sizeof(APCLI_CTRL_MSG_STRUCT), &ApCliCtrlMsg, ifIndex);
				*pCurrState = APCLI_ASSOC_IDLE;
			}
#ifndef WPA3_SUPPORT
			else
			{
				ApCliCtrlMsg.Status = Status;
				MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_ASSOC_RSP,
					sizeof(APCLI_CTRL_MSG_STRUCT), &ApCliCtrlMsg, ifIndex);
				*pCurrState = APCLI_ASSOC_IDLE;
			}
#endif
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - ApCliPeerAssocRspAction() sanity check fail\n"));
	}

	if (P2pSubelement != NULL)
		os_free_mem(NULL, P2pSubelement);
	return;
}

/*
    ==========================================================================
    Description:
        left part of IEEE 802.11/1999 p.374 
    Parameters:
        Elem - MLME message containing the received frame
    ==========================================================================
 */
static VOID ApCliPeerDisassocAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem) 
{
	UCHAR         Addr2[MAC_ADDR_LEN];
	USHORT        Reason;
	USHORT ifIndex = (USHORT)(Elem->Priv);
	ULONG *pCurrState = &pAd->ApCfg.ApCliTab[ifIndex].AssocCurrState;
#ifdef WPA3_SUPPORT
	P_WLAN_DISASSOC_FRAME_T prDisassocFrame;

	prDisassocFrame = (P_WLAN_DISASSOC_FRAME_T) Elem->Msg;

	DBGPRINT(RT_DEBUG_TRACE,
		("ASSOC - Rx Disassoc frame from BSSID[%02x:%02x:%02x:%02x:%02x:%02x], DA[%02x:%02x:%02x:%02x:%02x:%02x] ReasonCode[0x%x]\n",
		PRINT_MAC(prDisassocFrame->aucBSSID), PRINT_MAC(prDisassocFrame->aucDestAddr),
		prDisassocFrame->u2ReasonCode));
#endif

	if(PeerDisassocSanity(pAd, Elem->Msg, Elem->MsgLen, Addr2, &Reason))
	{
		if (MAC_ADDR_EQUAL(pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid, Addr2))
		{
#ifdef DPA_S
#ifdef P2P_SUPPORT
			UCHAR p2pIndex = P2P_NOT_FOUND;
#endif /* P2P_SUPPORT */
#endif /* DPA_S */
#ifdef WPA3_SUPPORT
			DBGPRINT(RT_DEBUG_TRACE, ("notification of RX disassociation %d\n", Elem->MsgLen));
			cfg80211_rx_mlme_mgmt(pAd->ApCfg.ApCliTab[ifIndex].wdev.if_dev, (PUINT8)prDisassocFrame, (size_t)Elem->MsgLen);
			DBGPRINT(RT_DEBUG_TRACE, ("notification of RX disassociation Done\n"));
#endif

			*pCurrState = APCLI_ASSOC_IDLE;

			MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_PEER_DISCONNECT_REQ, 0, NULL, ifIndex);

#ifdef DPA_S
#ifdef P2P_SUPPORT
			p2pIndex = P2pGroupTabSearch(pAd, Addr2);
			if (p2pIndex != P2P_NOT_FOUND)
			{
				PRT_P2P_CLIENT_ENTRY pP2pEntry = &pAd->P2pTable.Client[p2pIndex];
				P2pSendSamsungEvent(pAd, P2P_NOTIF_SOFTAP_STA_DISASSOC, Addr2, pP2pEntry->DeviceName, pP2pEntry->DeviceNameLen);
			}
			else
				P2pSendSamsungEvent(pAd, P2P_NOTIF_SOFTAP_STA_DISASSOC, Addr2, NULL, 0);
#endif /* P2P_SUPPORT */
#endif /* DPA_S */
        }
    }
    else
    {
        DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - ApCliPeerDisassocAction() sanity check fail\n"));
    }
	
	return;
}

/*
    ==========================================================================
    Description:
        what the state machine will do after assoc timeout
    ==========================================================================
 */
static VOID ApCliAssocTimeoutAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem) 
{
	USHORT ifIndex = (USHORT)(Elem->Priv);
	PULONG pCurrState = &pAd->ApCfg.ApCliTab[ifIndex].AssocCurrState;

	DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - ApCliAssocTimeoutAction\n"));
	*pCurrState = APCLI_ASSOC_IDLE;

	MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_ASSOC_REQ_TIMEOUT, 0, NULL, ifIndex);

	return;
}

static VOID ApCliInvalidStateWhenAssoc(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem) 
{
	APCLI_CTRL_MSG_STRUCT ApCliCtrlMsg;
	USHORT ifIndex = (USHORT)(Elem->Priv);
	PULONG pCurrState = &pAd->ApCfg.ApCliTab[ifIndex].AssocCurrState;

	DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - ApCliInvalidStateWhenAssoc(state=%ld), reset APCLI_ASSOC state machine\n", *pCurrState));
	*pCurrState = APCLI_ASSOC_IDLE;

	ApCliCtrlMsg.Status = MLME_STATE_MACHINE_REJECT;
	MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_ASSOC_RSP,
		sizeof(APCLI_CTRL_MSG_STRUCT), &ApCliCtrlMsg, ifIndex);

	return;
}

static VOID ApCliInvalidStateWhenDisassociate(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem) 
{
	APCLI_CTRL_MSG_STRUCT ApCliCtrlMsg;
	USHORT ifIndex = (USHORT)(Elem->Priv);
	ULONG *pCurrState = &pAd->ApCfg.ApCliTab[ifIndex].AssocCurrState;

	DBGPRINT(RT_DEBUG_TRACE, ("APCLI_ASSOC - InvalidStateWhenApCliDisassoc(state=%ld), reset APCLI_ASSOC state machine\n", *pCurrState));
	*pCurrState = APCLI_ASSOC_IDLE;

	ApCliCtrlMsg.Status = MLME_STATE_MACHINE_REJECT;
	MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_DEASSOC_RSP,
		sizeof(APCLI_CTRL_MSG_STRUCT), &ApCliCtrlMsg, ifIndex);

	return;
}

/*
    ==========================================================================
    Description:
        procedures on IEEE 802.11/1999 p.376 
    Parametrs:
    ==========================================================================
 */
static VOID ApCliAssocPostProc(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pAddr2, 
	IN USHORT CapabilityInfo, 
	IN USHORT IfIndex, 
	IN UCHAR SupRate[], 
	IN UCHAR SupRateLen,
	IN UCHAR ExtRate[],
	IN UCHAR ExtRateLen,
	IN PEDCA_PARM pEdcaParm,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN UCHAR HtCapabilityLen, 
	IN ADD_HT_INFO_IE *pAddHtInfo)
{
	APCLI_STRUCT *pApCliEntry = NULL;

	pApCliEntry = &pAd->ApCfg.ApCliTab[0];

	pApCliEntry->MlmeAux.BssType = BSS_INFRA;	
	pApCliEntry->MlmeAux.CapabilityInfo = CapabilityInfo & SUPPORTED_CAPABILITY_INFO;
	NdisMoveMemory(&pApCliEntry->MlmeAux.APEdcaParm, pEdcaParm, sizeof(EDCA_PARM));

	/* filter out un-supported rates */
	pApCliEntry->MlmeAux.SupRateLen = SupRateLen;
	NdisMoveMemory(pApCliEntry->MlmeAux.SupRate, SupRate, SupRateLen);
    RTMPCheckRates(pAd, pApCliEntry->MlmeAux.SupRate, &(pApCliEntry->MlmeAux.SupRateLen));

	/* filter out un-supported rates */
	pApCliEntry->MlmeAux.ExtRateLen = ExtRateLen;
	NdisMoveMemory(pApCliEntry->MlmeAux.ExtRate, ExtRate, ExtRateLen);
    RTMPCheckRates(pAd, pApCliEntry->MlmeAux.ExtRate, &(pApCliEntry->MlmeAux.ExtRateLen));

	DBGPRINT(RT_DEBUG_TRACE, (HtCapabilityLen ? "%s===> 11n HT STA\n" : "%s===> legacy STA\n", __FUNCTION__));

#ifdef DOT11_N_SUPPORT
	if (HtCapabilityLen > 0 && WMODE_CAP_N(pAd->CommonCfg.PhyMode))
		ApCliCheckHt(pAd, IfIndex, pHtCapability, pAddHtInfo);
#endif /* DOT11_N_SUPPORT */

}

#endif /* P2P_SUPPORT */

