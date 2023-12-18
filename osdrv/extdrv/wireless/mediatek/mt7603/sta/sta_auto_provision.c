/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	sta_auto_provision

	Abstract:	
*/

#include "rt_config.h"

extern UCHAR WPS_OUI[];

#ifdef STA_EASY_CONFIG_SETUP
UCHAR AutoProvisionTableSort(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			BssType)
{
	INT			i = 0, Len = 0;
	BSS_ENTRY *pInBss;
	UCHAR Count = 0;
	WSC_CTRL *pWpsCtrl = &pAd->StaCfg.WscControl;
	PBEACON_EID_STRUCT	pEid = NULL;
	UCHAR				*pData;
	/*UCHAR				TempUuid[8][16], uuid[16]; */
	BOOLEAN				bMICValid = FALSE, bSameAP;
/*	UUID_BSSID_CH_INFO	apCandiList[8]; */
	UUID_BSSID_CH_INFO	*apCandiList = NULL;
	UUID_BSSID_CH_INFO	*pApCandEntry;

	CHAR				RssiThreshold = pAd->StaCfg.EasyConfigInfo.RssiThreshold;


	/* allocate memory */
	os_alloc_mem(NULL, (UCHAR **)&apCandiList, sizeof(UUID_BSSID_CH_INFO) * 8);
	if (apCandiList == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		return 0;
	}

	if (BssType == BSS_ADHOC)
		RssiThreshold += 10;

	NdisZeroMemory(&apCandiList[0], sizeof(apCandiList));

	for (i = 0; i < pAd->ScanTab.BssNr; i++) 
	{
		bSameAP = FALSE;		
		/* BSS entry for VarIE processing */
		pInBss  = (BSS_ENTRY *) &pAd->ScanTab.BssEntry[i];

		/* Check VarIE length */
		if (pInBss->VarIELen == 0)
			continue;

		if (pInBss->BssType != BssType)
			continue;

		if (pInBss->Rssi < RssiThreshold)
			continue;

		/* 2. Search for WSC IE - 0xdd xx 00 50 f2 04 */
		Len = (INT)pInBss->VarIELen;
		pData = pInBss->VarIEs;
		while (Len > 0)
		{
			pEid = (PBEACON_EID_STRUCT) pData;
			
			/* No match, skip the Eid and move forward, IE_WFA_WSC = 0xdd */
			if (pEid->Eid != IE_WFA_WSC)
			{
				/* Set the offset and look for next IE */
				pData += (pEid->Len + 2);
				Len   -= (pEid->Len + 2);
				continue;
			}
			else
			{
				/* Found IE with 0xdd */
				/* check for WSC OUI -- 00 50 f2 04 */
				if (NdisEqualMemory(pEid->Octet, WPS_OUI, 4) == FALSE)
				{
					/* Set the offset and look for next IE */
					pData += (pEid->Len + 2);
					Len   -= (pEid->Len + 2);
					continue;
				}				
			}
			
			bMICValid = WpsMICIntegrity(pAd, pEid->Octet, pEid->Len, pInBss->MacAddr, BSS0);

			if (bMICValid && (Count < 8))
			{
				INT		Length = pEid->Len;
				UCHAR	*Ptr = pEid->Octet + 4;
				UCHAR	uuidStr[16];
				
				pApCandEntry = &apCandiList[Count];
				while (Length > 0)
				{
					WSC_IE		WscIE;
					WSC_IE		*pWscIE = NULL;
					
					NdisMoveMemory(&WscIE, Ptr, sizeof(WSC_IE));
					/* Check for WSC IEs */
					pWscIE = &WscIE;

					/* UUID_E is optional for beacons, but mandatory for probe-request */
					if (be2cpu16(pWscIE->Type) == WSC_ID_UUID_E)
					{
						RTMPMoveMemory(uuidStr, (UCHAR *)(Ptr+4), 16);
						break;
					}
					
					/* Set the offset and look for PBC information */
					/* Since Type and Length are both short type, we need to offset 4, not 2 */
					Ptr		+= (be2cpu16(pWscIE->Length) + 4);
					Length	-= (be2cpu16(pWscIE->Length) + 4);
				}

				if (Count > 0)
				{
					INT j;

					for (j = 0; j < Count; j++)
					{
						if (RTMPEqualMemory(uuidStr, apCandiList[j].Uuid, 16) == TRUE)
						{
							bSameAP = TRUE;
							break;
						}
						else
						{
							INT macDiff, candMacNicVal, baseMacNicVal;

							candMacNicVal = (pInBss->Bssid[3] << 16) + (pInBss->Bssid[4] << 8) + pInBss->Bssid[5];
							baseMacNicVal = (apCandiList[j].Bssid[3] << 16) + (apCandiList[j].Bssid[4] << 8) + apCandiList[j].Bssid[5];
							macDiff = ((candMacNicVal > baseMacNicVal) ? (candMacNicVal - baseMacNicVal) : (baseMacNicVal - candMacNicVal));
							if (macDiff <=4)
								bSameAP = TRUE;
						}
					}
				}
			
				if (!bSameAP)
				{
					NdisZeroMemory(pApCandEntry->Ssid, MAX_LEN_OF_SSID);
					NdisZeroMemory(pApCandEntry->Bssid, MAC_ADDR_LEN);
					NdisZeroMemory(pApCandEntry->MacAddr, MAC_ADDR_LEN);
					NdisZeroMemory(pApCandEntry->Uuid, 16);

					pApCandEntry->Channel = pInBss->Channel;
					RTMPMoveMemory(pApCandEntry->Uuid, uuidStr, 16);
					pApCandEntry->SsidLen = pInBss->SsidLen;
					NdisMoveMemory(pApCandEntry->Ssid, pInBss->Ssid, pInBss->SsidLen);
					NdisMoveMemory(pApCandEntry->Bssid, pInBss->Bssid, MAC_ADDR_LEN);
					NdisMoveMemory(pApCandEntry->MacAddr, pInBss->MacAddr, MAC_ADDR_LEN);
					Count++;
				}
			}
			
			break;
		}		
	}

	if (Count != 1)
	{
		NdisZeroMemory(&pWpsCtrl->WscSsid, sizeof(NDIS_802_11_SSID));
		NdisZeroMemory(pWpsCtrl->WscBssid, MAC_ADDR_LEN);
		NdisZeroMemory(pWpsCtrl->WscPeerMAC, MAC_ADDR_LEN);
		pAd->MlmeAux.Channel = pAd->CommonCfg.Channel;
	}
	else
	{
		NdisZeroMemory(&pWpsCtrl->WscSsid, sizeof(NDIS_802_11_SSID));
		NdisZeroMemory(pWpsCtrl->WscBssid, MAC_ADDR_LEN);
		NdisZeroMemory(pWpsCtrl->WscPeerMAC, MAC_ADDR_LEN);
		
		pWpsCtrl->WscSsid.SsidLength = apCandiList[0].SsidLen;
		NdisMoveMemory(pWpsCtrl->WscSsid.Ssid, apCandiList[0].Ssid, apCandiList[0].SsidLen);
		NdisMoveMemory(pWpsCtrl->WscBssid, apCandiList[0].Bssid, MAC_ADDR_LEN);
		NdisMoveMemory(pWpsCtrl->WscPeerMAC, apCandiList[0].MacAddr, MAC_ADDR_LEN);
		pAd->MlmeAux.Channel = apCandiList[0].Channel;
	}

	if (apCandiList != NULL)
		os_free_mem(NULL, apCandiList);
	return Count;
}

VOID AutoProvisionScanTimeOutAction(
    IN PVOID SystemSpecific1,
    IN PVOID FunctionContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3)
{
    RTMP_ADAPTER    *pAd = (PRTMP_ADAPTER)FunctionContext;

	DBGPRINT(RT_DEBUG_TRACE, ("!!! AutoProvisionScanTimeOutAction !!!\n"));

	if (pAd != NULL)
	{
		PWSC_CTRL	pWpsCtrl = &pAd->StaCfg.WscControl;
		PEASY_CONFIG_INFO	pEasyConfig = &pAd->StaCfg.EasyConfigInfo;
		UCHAR		ApCount = 0;
		BOOLEAN		bCancelled;

		RTMPCancelTimer(&pAd->MlmeAux.ScanTimer, &bCancelled);
		
		/* Make sure connect state machine is idle */
		if (pAd->Mlme.CntlMachine.CurrState != CNTL_IDLE)
		{
			RTMP_MLME_RESET_STATE_MACHINE(pAd);
			DBGPRINT(RT_DEBUG_OFF, ("!!! MLME busy, reset MLME state machine !!!\n"));
		}
						
		ApCount = AutoProvisionTableSort(pAd, BSS_INFRA);
		DBGPRINT(RT_DEBUG_TRACE, ("%s:: %d Candidates\n", __FUNCTION__, ApCount));
		if ((pAd->ScanTab.BssNr == 0) ||
			(ApCount == 0))
		{
			if (pEasyConfig->bChangeMode)
			{
				pAd->Mlme.AutoProvisionMachine.CurrState = ADHOC_AUTO_PROVISION;
				if (pEasyConfig->ModuleType == MODULE_TV)
					MlmeEnqueue(pAd, AUTO_PROVISION_STATE_MACHINE, MT2_START_AUTO_PROVISION_IBSS, 0, NULL, 0);
				else
					MlmeEnqueue(pAd, AUTO_PROVISION_STATE_MACHINE, MT2_JOIN_AUTO_PROVISION_IBSS, 0, NULL, 0);
			}
			else
			{
				pAd->Mlme.AutoProvisionMachine.CurrState = INFRA_AUTO_PROVISION;
				MlmeEnqueue(pAd, AUTO_PROVISION_STATE_MACHINE, MT2_CONN_AP, 0, NULL, 0);
				RTMP_MLME_HANDLER(pAd);
			}
		}
		else
		{	
			if (ApCount == 1)
			{
				/* */
				/* Update Reconnect Ssid, that user desired to connect. */
				/* */
				NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, MAX_LEN_OF_SSID);
				NdisMoveMemory(pAd->MlmeAux.AutoReconnectSsid, pWpsCtrl->WscSsid.Ssid, pWpsCtrl->WscSsid.SsidLength);
				pAd->MlmeAux.AutoReconnectSsidLen = pWpsCtrl->WscSsid.SsidLength;

				pAd->bConfigChanged = TRUE;
				pAd->StaCfg.EasyConfigInfo.bRaAutoWpsAp = TRUE;
				pAd->StaCfg.EasyConfigInfo.bDoAutoWps = TRUE;
				MlmeEnqueue(pAd, 
							MLME_CNTL_STATE_MACHINE, 
							OID_802_11_BSSID,
							MAC_ADDR_LEN,
							pWpsCtrl->WscBssid, 0);
			}
			else
			{
				BOOLEAN bCancel;
				RTMPCancelTimer(&pEasyConfig->InfraConnTimer, &bCancel);
				/* Send Wireless Event, change WscStatus */
				RTMPSendWirelessEvent(pAd, IW_WSC_MANY_CANDIDATE, NULL, BSS0, 0);

				pAd->StaCfg.WscControl.WscConfMode = WSC_DISABLE;
				pAd->StaCfg.WscControl.WscState = WSC_STATE_OFF;
				if (pAd->StaCfg.WscControl.WscStatus != STATUS_WSC_CONFIGURED)
					pAd->StaCfg.WscControl.WscStatus = STATUS_WSC_NOTUSED;

				pAd->MlmeAux.AutoReconnectSsidLen= 32;
    			NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen);				
			}
		}

		pAd->StaCfg.bScanReqIsFromWebUI = FALSE;
		/* call Mlme handler to execute it */
		RTMP_MLME_HANDLER(pAd);
	}
}


VOID	AutoProvisionScanAction(
	IN	PRTMP_ADAPTER	pAd)
{
	PWSC_CTRL   pWpsCtrl = &pAd->StaCfg.WscControl;
		
	DBGPRINT(RT_DEBUG_TRACE, ("-----> %s\n", __FUNCTION__));

	/* Turn off WSC state matchine */
	WscStop(pAd,
#ifdef CONFIG_AP_SUPPORT
			FALSE,
#endif /* CONFIG_AP_SUPPORT */
			pWpsCtrl);
	
	/* Set WSC state to WSC_STATE_INIT */
	pWpsCtrl->WscState = WSC_STATE_INIT;
	pWpsCtrl->WscStatus = STATUS_WSC_SCAN_AP;

	/* Init Registrar pair structures */
	WscInitRegistrarPair(pAd, pWpsCtrl, BSS0);

	pAd->StaCfg.BssType = BSS_INFRA;
	pWpsCtrl->WscConfStatus = WSC_SCSTATE_UNCONFIGURED;
	pWpsCtrl->WscConfMode = WSC_ENROLLEE;
	pWpsCtrl->WscMode = 1;

	/* Call WscScan subroutine */
	pAd->StaCfg.bScanReqIsFromWebUI = TRUE;
	WscScanExec(pAd, pWpsCtrl);
	
	/* Set 8 second timer to invoke auto provision scan timeout action */
	RTMPSetTimer(&pAd->StaCfg.EasyConfigInfo.ScanTimer, 8000);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- %s\n", __FUNCTION__));
}

/*
    ==========================================================================
    Description:
        function to be executed at timer thread when infra connection timer expires
        
	IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
VOID InfraConnectionTimeout(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3)
{
	PRTMP_ADAPTER 	pAd = (RTMP_ADAPTER *)FunctionContext;
	PWSC_CTRL		pWpsCtrl = NULL;
    
    DBGPRINT(RT_DEBUG_TRACE,("InfraConnectionTimeout\n"));
	
	/* 
		Do nothing if pAd is NULL or the driver is starting halt state.
		This might happen when timer already been fired before cancel timer with mlmehalt
	*/
	if ((pAd == NULL) || RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;

	pWpsCtrl = &pAd->StaCfg.WscControl;
	if ((INFRA_ON(pAd) && (pAd->StaCfg.PortSecured == WPA_802_1X_PORT_SECURED)) ||
		(ADHOC_ON(pAd) && OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED)))
	{
		if (ADHOC_ON(pAd) && (pAd->StaCfg.EasyConfigInfo.ModuleType == MODULE_TV))
		{
			pWpsCtrl->WscConfMode = WSC_REGISTRAR;
			AsicDisableSync(pAd);
			WscBuildBeaconIE(pAd, pWpsCtrl->WscConfStatus, FALSE, 1, pWpsCtrl->WscConfigMethods, BSS0, NULL, 0, STA_MODE);
			WscBuildProbeRespIE(pAd, WSC_MSGTYPE_REGISTRAR, pWpsCtrl->WscConfStatus, FALSE, 1, pWpsCtrl->WscConfigMethods, BSS0, NULL, 0, STA_MODE);
			MakeIbssBeacon(pAd);        /* re-build BEACON frame */
			AsicEnableIbssSync(pAd);    /* copy BEACON frame to on-chip memory */
		}
		else
		{
			pWpsCtrl->WscConfMode = WSC_DISABLE;
			pWpsCtrl->WscState = WSC_STATE_OFF;
		}
		if (pWpsCtrl->WscStatus != STATUS_WSC_CONFIGURED)
			pWpsCtrl->WscStatus = STATUS_WSC_NOTUSED;
		return;
	}
	else
	{
		if (INFRA_ON(pAd))
		{
			MLME_DEAUTH_REQ_STRUCT      DeAuthReq;
			MLME_QUEUE_ELEM *pMsgElem;
    
			os_alloc_mem(pAd, (UCHAR **)&pMsgElem, sizeof(MLME_QUEUE_ELEM));
			if (pMsgElem)
			{
				COPY_MAC_ADDR(DeAuthReq.Addr, pAd->CommonCfg.Bssid);
				DeAuthReq.Reason = REASON_DEAUTH_STA_LEAVING;
				pMsgElem->MsgLen = sizeof(MLME_DEAUTH_REQ_STRUCT);
				NdisMoveMemory(pMsgElem->Msg, &DeAuthReq, sizeof(MLME_DEAUTH_REQ_STRUCT));
				MlmeDeauthReqAction(pAd, pMsgElem);
			    LinkDown(pAd, TRUE);
			    pAd->Mlme.AssocMachine.CurrState = ASSOC_IDLE;
				os_free_mem(NULL, pMsgElem);
			}
		}
		MlmeEnqueue(pAd, AUTO_PROVISION_STATE_MACHINE, MT2_CONN_AUTO_PROVISION_AP, 0, NULL, 0);
	    RTMP_MLME_HANDLER(pAd);
	}
}

VOID AdhocConnectionTimeout(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3) 
{
    RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;
	DBGPRINT(RT_DEBUG_TRACE,("AdhocConnectionTimeout\n"));

	/* 
		Do nothing if pAd is NULL or the driver is starting halt state.
		This might happen when timer already been fired before cancel timer with mlmehalt
	*/
	if ((pAd == NULL) || RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;

	pAd->StaCfg.WscControl.WscConfMode = WSC_DISABLE;
	pAd->StaCfg.WscControl.WscState = WSC_STATE_OFF;
	if (pAd->StaCfg.WscControl.WscStatus != STATUS_WSC_CONFIGURED)
		pAd->StaCfg.WscControl.WscStatus = STATUS_WSC_NOTUSED;
	
	if ((ADHOC_ON(pAd) && OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED)) ||
		(INFRA_ON(pAd) && (pAd->StaCfg.PortSecured == WPA_802_1X_PORT_SECURED)))
	{		
		/* If user selects AP/Ad-hoc peer from GUI and connects successfully */
		return;
	}
	else
	{
		NdisZeroMemory(pAd->CommonCfg.Ssid, MAX_LEN_OF_SSID);
		NdisMoveMemory(pAd->CommonCfg.Ssid, pAd->CommonCfg.LastSsid, pAd->CommonCfg.LastSsidLen);
		pAd->CommonCfg.SsidLen = pAd->CommonCfg.LastSsidLen;
		
		LinkDown(pAd, FALSE);
		pAd->StaCfg.BssType = BSS_INFRA;
		
		pAd->Mlme.AutoProvisionMachine.CurrState = INFRA_AUTO_PROVISION;
		MlmeEnqueue(pAd, AUTO_PROVISION_STATE_MACHINE, MT2_CONN_AP, 0, NULL, 0);
	    RTMP_MLME_HANDLER(pAd);
	}
}


VOID AutoProvisionIdleAction(
    IN PRTMP_ADAPTER	pAd, 
    IN PMLME_QUEUE_ELEM pElem) 
{
	PWSC_CTRL	pWpsCtrl = &pAd->StaCfg.WscControl;
	PEASY_CONFIG_INFO	pEasyConfig = &pAd->StaCfg.EasyConfigInfo;
	BOOLEAN		bCancel;

	if (pEasyConfig->bEnable == FALSE)
		return;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> %s\n", __FUNCTION__));

	RTMPCancelTimer(&pEasyConfig->ScanTimer, &bCancel);
	RTMPCancelTimer(&pEasyConfig->InfraConnTimer, &bCancel);
	RTMPCancelTimer(&pEasyConfig->AdhocConnTimer, &bCancel);

	/* Turn off WSC state matchine */
	WscStop(pAd, 
#ifdef CONFIG_AP_SUPPORT
			FALSE,
#endif /* CONFIG_AP_SUPPORT */
			pWpsCtrl);

	NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, MAX_LEN_OF_SSID);
	pAd->MlmeAux.AutoReconnectSsidLen = pAd->CommonCfg.SsidLen;
	NdisMoveMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->CommonCfg.Ssid, pAd->CommonCfg.SsidLen);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- %s\n", __FUNCTION__));
}

VOID AutoProvisionConnectAction(
    IN PRTMP_ADAPTER	pAd, 
    IN PMLME_QUEUE_ELEM pElem) 
{
	PWSC_CTRL	pWpsCtrl = &pAd->StaCfg.WscControl;
	PEASY_CONFIG_INFO	pEasyConfig = &pAd->StaCfg.EasyConfigInfo;

	if (pEasyConfig->bEnable == FALSE)
		return;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> %s\n", __FUNCTION__));

	/* Turn off WSC state matchine */
	WscStop(pAd, 
#ifdef CONFIG_AP_SUPPORT
			FALSE,
#endif /* CONFIG_AP_SUPPORT */
			pWpsCtrl);

	NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, MAX_LEN_OF_SSID);
	pAd->MlmeAux.AutoReconnectSsidLen = pAd->CommonCfg.SsidLen;
	NdisMoveMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->CommonCfg.Ssid, pAd->CommonCfg.SsidLen);
	
	RTMPSetTimer(&pEasyConfig->InfraConnTimer, INFRA_CONNECTION_TIMEOUT);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- %s\n", __FUNCTION__));
}

VOID AutoProvisionWpsAction(
    IN PRTMP_ADAPTER	pAd, 
    IN PMLME_QUEUE_ELEM pElem) 
{
	if (pAd->StaCfg.EasyConfigInfo.bEnable == FALSE)
		return;

#ifdef WAC_SUPPORT
	if (pAd->StaCfg.EasyConfigInfo.bWACAP)
		return;
#endif /* WAC_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("-----> %s\n", __FUNCTION__));	

	AutoProvisionScanAction(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("<----- %s\n", __FUNCTION__));
}

VOID AutoProvisionStartAction(
    IN PRTMP_ADAPTER	pAd, 
    IN PMLME_QUEUE_ELEM pElem) 
{
	NDIS_802_11_SSID	Ssid;
	PWSC_CTRL			pWpsCtrl = &pAd->StaCfg.WscControl;
	PEASY_CONFIG_INFO	pEasyConfig = &pAd->StaCfg.EasyConfigInfo;

	if (pEasyConfig->bEnable == FALSE)
		return;

	NdisZeroMemory(&Ssid, sizeof(NDIS_802_11_SSID));
	DBGPRINT(RT_DEBUG_TRACE, ("-----> %s\n", __FUNCTION__));
	pAd->StaCfg.BssType = BSS_ADHOC;
	snprintf(&Ssid.Ssid[0], sizeof(Ssid.Ssid), "SSID_%02X%02X%02X", RandomByte(pAd), RandomByte(pAd), RandomByte(pAd));
	Ssid.SsidLength = strlen(&Ssid.Ssid[0]);

	/* Set WSC state to WSC_STATE_INIT */
	pWpsCtrl->WscState = WSC_STATE_INIT;
	pWpsCtrl->WscStatus = STATUS_WSC_SCAN_AP;

	/* Init Registrar pair structures */
	WscInitRegistrarPair(pAd, pWpsCtrl, BSS0);

	pWpsCtrl->WscConfStatus = WSC_SCSTATE_UNCONFIGURED;
	pWpsCtrl->WscConfMode = WSC_REGISTRAR;
	NdisZeroMemory(&pWpsCtrl->WscProfile, sizeof(WSC_PROFILE));
	pEasyConfig->bDoAutoWps = TRUE;
	/* generate a radom number as BSSID */
	MacAddrRandomBssid(pAd, pAd->MlmeAux.Bssid);
	WscBuildBeaconIE(pAd, WSC_SCSTATE_UNCONFIGURED, FALSE, 1, pWpsCtrl->WscConfigMethods, BSS0, NULL, 0, STA_MODE);
	WscBuildProbeRespIE(pAd, WSC_MSGTYPE_REGISTRAR, WSC_SCSTATE_UNCONFIGURED, FALSE, 1, pWpsCtrl->WscConfigMethods, BSS0, NULL, 0, STA_MODE);

    pAd->MlmeAux.CurrReqIsFromNdis = TRUE;
	pAd->bConfigChanged = TRUE;

    MlmeEnqueue(pAd, 
                MLME_CNTL_STATE_MACHINE, 
                OID_802_11_SSID,
                sizeof(NDIS_802_11_SSID),
                (VOID *)&Ssid, 0);
	
	RTMP_MLME_HANDLER(pAd);

	RTMPSetTimer(&pEasyConfig->AdhocConnTimer, ADHOC_CONNECTION_TIMEOUT);
	DBGPRINT(RT_DEBUG_TRACE, ("<----- %s\n", __FUNCTION__));
}

VOID AutoProvisionJoinAction(
    IN PRTMP_ADAPTER	pAd, 
    IN PMLME_QUEUE_ELEM pElem) 
{
	PWSC_CTRL	pWpsCtrl = &pAd->StaCfg.WscControl;
	UCHAR		Count = 0;
	
	if (pAd->StaCfg.EasyConfigInfo.bEnable == FALSE)
		return;
	
	DBGPRINT(RT_DEBUG_TRACE, ("-----> %s\n", __FUNCTION__));

	pAd->StaCfg.BssType = BSS_ADHOC;
	Count = AutoProvisionTableSort(pAd, BSS_ADHOC);
	DBGPRINT(RT_DEBUG_TRACE, ("%s:: %d Candidates\n", __FUNCTION__, Count));
	if ((pAd->ScanTab.BssNr == 0) ||
		(Count == 0))
	{
		if (ADHOC_ON(pAd))
		{
			pAd->StaCfg.bNotFirstScan = FALSE;
			LinkDown(pAd, FALSE);
		}
		else
			pAd->StaCfg.BssType = BSS_INFRA;
		pAd->Mlme.AutoProvisionMachine.CurrState = INFRA_AUTO_PROVISION;
		MlmeEnqueue(pAd, AUTO_PROVISION_STATE_MACHINE, MT2_CONN_AP, 0, NULL, 0);
	}
	else
	{
		if (Count == 1)
		{
			/* Set WSC state to WSC_STATE_INIT */
			pWpsCtrl->WscState = WSC_STATE_INIT;
			pWpsCtrl->WscStatus = STATUS_WSC_SCAN_AP;

			/* Init Registrar pair structures */
			WscInitRegistrarPair(pAd, pWpsCtrl, BSS0);

			pWpsCtrl->WscConfStatus = WSC_SCSTATE_UNCONFIGURED;
			pWpsCtrl->WscConfMode = WSC_ENROLLEE;

			pAd->bConfigChanged = TRUE;
			pAd->StaCfg.BssType = BSS_ADHOC;
			MlmeEnqueue(pAd, 
						MLME_CNTL_STATE_MACHINE, 
						OID_802_11_BSSID,
						MAC_ADDR_LEN,
						pWpsCtrl->WscBssid, 0);
		}
		else
		{
			BOOLEAN bCancel;
			RTMPCancelTimer(&pAd->StaCfg.EasyConfigInfo.InfraConnTimer, &bCancel);
			/* Send Wireless Event, change WscStatus */
			RTMPSendWirelessEvent(pAd, IW_WSC_MANY_CANDIDATE, NULL, BSS0, 0);

			pAd->StaCfg.WscControl.WscConfMode = WSC_DISABLE;
			pAd->StaCfg.WscControl.WscState = WSC_STATE_OFF;
			if (pAd->StaCfg.WscControl.WscStatus != STATUS_WSC_CONFIGURED)
				pAd->StaCfg.WscControl.WscStatus = STATUS_WSC_NOTUSED;

			pAd->MlmeAux.AutoReconnectSsidLen= 32;
			NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen);
		}
	}
	DBGPRINT(RT_DEBUG_TRACE, ("<----- %s\n", __FUNCTION__));
}

VOID	AutoProvisionStop(
	IN  PRTMP_ADAPTER	pAd)
{
	PEASY_CONFIG_INFO	pEasyConfig = &pAd->StaCfg.EasyConfigInfo;
	BOOLEAN		bCancel;
	
	RTMPCancelTimer(&pEasyConfig->ScanTimer, &bCancel);
	RTMPCancelTimer(&pEasyConfig->InfraConnTimer, &bCancel);
	RTMPCancelTimer(&pEasyConfig->AdhocConnTimer, &bCancel);

	return;
}


void AutoProvisionStateMachineInit(
    IN PRTMP_ADAPTER pAd, 
    IN STATE_MACHINE *Sm, 
    OUT STATE_MACHINE_FUNC Trans[]) 
{
    StateMachineInit(Sm, 
    				 Trans, 
    				 MAX_AUTO_PROVISION_STATE, 
    				 MAX_AUTO_PROVISION_MSG, 
    				 (STATE_MACHINE_FUNC)Drop, 
    				 AUTO_PROVISION_IDLE, 
    				 AUTO_PROVISION_MACHINE_BASE);

	StateMachineSetAction(Sm, AUTO_PROVISION_IDLE, MT2_CONN_AP, (STATE_MACHINE_FUNC)AutoProvisionIdleAction);
	
	StateMachineSetAction(Sm, INFRA_AUTO_PROVISION, MT2_CONN_AP, (STATE_MACHINE_FUNC)AutoProvisionConnectAction);
	StateMachineSetAction(Sm, INFRA_AUTO_PROVISION, MT2_CONN_AUTO_PROVISION_AP, (STATE_MACHINE_FUNC)AutoProvisionWpsAction);

	StateMachineSetAction(Sm, ADHOC_AUTO_PROVISION, MT2_START_AUTO_PROVISION_IBSS, (STATE_MACHINE_FUNC)AutoProvisionStartAction);
	StateMachineSetAction(Sm, ADHOC_AUTO_PROVISION, MT2_JOIN_AUTO_PROVISION_IBSS, (STATE_MACHINE_FUNC)AutoProvisionJoinAction);
}

#endif /* STA_EASY_CONFIG_SETUP */

