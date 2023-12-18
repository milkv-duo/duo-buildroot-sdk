/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
    sy_sta_ioctl.c

    Abstract:
    SigmaTel Wifi IOCTL related subroutines

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/

#include "rt_config.h"

/**********************************************************************************
* Global variables
***********************************************************************************/
extern DC_NVRAM_OBJECT_t NVRAMWIFISetting;
extern UCHAR WPS_OUI[];
DC_WIFI_STATUS gWifiStatus;

typedef struct __SY_WIFI_WPS_STATUS__{
	DC_WIFI_WPS_STATUS DCWiFiWPSStatus;
	WIFI_WPS_STATUS WiFiWPSStatus;
}SY_WIFI_WPS_STATUS;

static SY_WIFI_WPS_STATUS SYWPSStatusMapTble1[] = {
	{DC_WIFI_WPS_STATUS_LAST, DC_WIFI_WPS_LAST_STATUS},		/* STATUS_WSC_NOTUSED */
	{DC_WIFI_WPS_STATUS_LAST, DC_WIFI_WPS_LAST_STATUS},		/* STATUS_WSC_IDLE */
	{DC_WIFI_WPS_STATUS_FAILED,DC_WIFI_WPS_INCOMPLETE},		/* STATUS_WSC_FAIL */
	{DC_WIFI_WPS_STATUS_SCANNING,DC_WIFI_WPS_IDLE},			/* STATUS_WSC_LINK_UP */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_REQSTART},	/* STATUS_WSC_EAPOL_START_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_REQSTART},	/* STATUS_WSC_EAP_REQ_ID_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_REQSTART},	/* STATUS_WSC_EAP_RSP_ID_RECEIVED */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},	/* STATUS_WSC_EAP_RSP_WRONG_SMI */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},	/* STATUS_WSC_EAP_RSP_WRONG_VENDOR_TYPE */
	{DC_WIFI_WPS_STATUS_CONNECTING, DC_WIFI_WPS_REQSTART},  /* STATUS_WSC_EAP_REQ_WSC_START */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M1},		/* STATUS_WSC_EAP_M1_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M1},		/* STATUS_WSC_EAP_M1_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M2},		/* STATUS_WSC_EAP_M2_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M2}, 		/* STATUS_WSC_EAP_M2_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M2D},		/* STATUS_WSC_EAP_M2D_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M3}, 		/* STATUS_WSC_EAP_M3_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M3}, 		/* STATUS_WSC_EAP_M3_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M4},		/* STATUS_WSC_EAP_M4_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M4}, 		/* STATUS_WSC_EAP_M4_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M5},		/* STATUS_WSC_EAP_M5_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M5},		/* STATUS_WSC_EAP_M5_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M6},		/* STATUS_WSC_EAP_M6_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M6},		/* STATUS_WSC_EAP_M6_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M7},		/* STATUS_WSC_EAP_M7_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M7},		/* STATUS_WSC_EAP_M7_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M8},		/* STATUS_WSC_EAP_M8_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_M8},		/* STATUS_WSC_EAP_M8_RECEIVED */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_REQSTART},	/* STATUS_WSC_EAP_RAP_RSP_ACK */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_DONE},		/* STATUS_WSC_EAP_RAP_REQ_DONE_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_DONE},		/* STATUS_WSC_EAP_RAP_RSP_DONE_SENT */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},	/* STATUS_WSC_EAP_FAIL_SENT */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},	/* STATUS_WSC_ERROR_HASH_FAIL */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},	/* STATUS_WSC_ERROR_HMAC_FAIL */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},	/* STATUS_WSC_ERROR_DEV_PWD_AUTH_FAIL */
	{DC_WIFI_WPS_STATUS_SUCCESSFUL, DC_WIFI_WPS_IDLE_AFTER_WPS_DONE},  /* STATUS_WSC_CONFIGURED */
	{DC_WIFI_WPS_STATUS_SCANNING, DC_WIFI_WPS_UNCONNECTED},	/* STATUS_WSC_SCAN_AP */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_REQSTART},	/* STATUS_WSC_EAPOL_START_SENT */
	{DC_WIFI_WPS_STATUS_REGISTERING, DC_WIFI_WPS_DONE},		/* STATUS_WSC_EAP_RSP_DONE_SENT */
	{DC_WIFI_WPS_STATUS_PIN_WAIT, DC_WIFI_WPS_PIN_READY},	/* STATUS_WSC_WAIT_PIN_CODE */
	{DC_WIFI_WPS_STATUS_SUCCESSFUL, DC_WIFI_WPS_ASSOCIATED} /* STATUS_WSC_START_ASSOC */
};

static SY_WIFI_WPS_STATUS SYWPSStatusMapTble2[] = {
	{DC_WIFI_WPS_STATUS_SESSION_OVERLAP, DC_WIFI_WPS_INCOMPLETE},	/* STATUS_WSC_PBC_TOO_MANY_AP */
	{DC_WIFI_WPS_STATUS_UNSUCCESSFUL, DC_WIFI_WPS_INCOMPLETE},		/* STATUS_WSC_PBC_NO_AP */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},  									/* STATUS_WSC_EAP_FAIL_RECEIVED */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},  		/* STATUS_WSC_EAP_NONCE_MISMATCH */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},			/* STATUS_WSC_EAP_INVALID_DATA */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},			/* STATUS_WSC_PASSWORD_MISMATCH */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},			/* STATUS_WSC_EAP_REQ_WRONG_SMI */
	{DC_WIFI_WPS_STATUS_FAILED, DC_WIFI_WPS_INCOMPLETE},			/* STATUS_WSC_EAP_REQ_WRONG_VENDOR_TYPE */
	{DC_WIFI_WPS_STATUS_SESSION_OVERLAP, DC_WIFI_WPS_INCOMPLETE}	/* STATUS_WSC_PBC_SESSION_OVERLAP */
};

BUILD_TIMER_FUNCTION(SYWIFIAPScanTimerTimeout);

#ifdef WSC_STA_SUPPORT
BUILD_TIMER_FUNCTION(SYWIFIWPSTimerTimeout);
BUILD_TIMER_FUNCTION(SYWIFIWPSFoolProofTimerTimeout);
#endif /* WSC_STA_SUPPORT */

/* Signal quality rating table */
static int SigQuaRangeLimit[] = { 
	15,
	30,
	50,
	70,
	85,
	100
};
static DC_WIFI_SIGNAL_RATING SigQuaRating[] = {
	DC_WIFI_SIGNAL_NOSIGNAL, 
	DC_WIFI_SIGNAL_VERYPOOR,
	DC_WIFI_SIGNAL_POOR,
	DC_WIFI_SIGNAL_DECENT,
	DC_WIFI_SIGNAL_GOOD,
	DC_WIFI_SIGNAL_EXCELLENT
}; 


void SYWIFIAPScanTimerTimeout(IN  PVOID SystemSpecific1,
							  IN  PVOID FunctionContext,
							  IN  PVOID SystemSpecific2, 
							  IN  PVOID SystemSpecific3)
{
	PRTMP_ADAPTER pAd = (RTMP_ADAPTER *)FunctionContext;

	/* FIXME: Should not change directly,
	 * need cancel driver scan timer, scan nex channel */
	/* Stop AP Scan */
	pAd->MlmeAux.Channel = 0;

	RTMPSendWirelessEvent(pAd, IW_SCAN_COMPLETED_EVENT_FLAG, NULL, BSS0, 0);
}


static void SYGetAPWifiCipherSuites(IN NDIS_802_11_AUTHENTICATION_MODE *pRTAuthMode,
								    IN NDIS_802_11_WEP_STATUS *pRTWepStatus,
								    IN UCHAR *pRTCipherAlg,
								    OUT DC_WIFI_AP_ENCR *pSYAPEncr)
{
	if (*pRTAuthMode == Ndis802_11AuthModeOpen)
	{
		if (*pRTWepStatus == Ndis802_11WEPEnabled)
		{
			if (*pRTCipherAlg == CIPHER_WEP64)
				*pSYAPEncr = DC_WIFI_AP_ENCR_WEP64;
			else if (*pRTCipherAlg == CIPHER_WEP128)
				*pSYAPEncr = DC_WIFI_AP_ENCR_WEP128;
		}
		else
			*pSYAPEncr = DC_WIFI_AP_ENCR_NONE;
	}
	else if (*pRTAuthMode == Ndis802_11AuthModeShared)
	{
		if (*pRTWepStatus == Ndis802_11WEPEnabled)
		{
			if (*pRTCipherAlg == CIPHER_WEP64)
				*pSYAPEncr = DC_WIFI_AP_ENCR_WEP64;
			else if (*pRTCipherAlg == CIPHER_WEP128)
				*pSYAPEncr = DC_WIFI_AP_ENCR_WEP128;
		}
	}
	else if (*pRTAuthMode == Ndis802_11AuthModeWPAPSK)
	{
		if (*pRTWepStatus == Ndis802_11TKIPEnable)
			*pSYAPEncr = DC_WIFI_AP_ENCR_WPAPSK_TKIP;
		else if (*pRTWepStatus == Ndis802_11AESEnable)
			*pSYAPEncr = DC_WIFI_AP_ENCR_WPAPSK_AES;
		else
			DBGPRINT(RT_DEBUG_ERROR, ("Unknown ciphersuite(RTAuthMode = %x, pRTWepStatus = %x", 
										*pRTAuthMode, *pRTWepStatus));
	}
	else if (*pRTAuthMode == Ndis802_11AuthModeWPA2PSK)
	{
		if (*pRTWepStatus == Ndis802_11TKIPEnable)
			*pSYAPEncr = DC_WIFI_AP_ENCR_WPA2PSK_TKIP;
		else if (*pRTWepStatus == Ndis802_11AESEnable)
			*pSYAPEncr = DC_WIFI_AP_ENCR_WPA2PSK_AES;
		else
			DBGPRINT(RT_DEBUG_ERROR, ("Unknown ciphersuite(RTAuthMode = %x, pRTWepStatus = %x", 
										*pRTAuthMode, *pRTWepStatus));
	}
}


static void SYAPSortByRSSI(IN int NumAPsFound, OUT BSS_ENTRY *pSYAPScanResult)
{
	BSS_ENTRY holdAP, j, pass;
	BOOLEAN Switched = TRUE;

	for (int pass = 0; pass < NumAPsFound - 1 && Switched == TRUE; pass++)
	{
		Switched = FALSE;
		
		for (int j = 0; j < NumAPsFound - pass - 1; j++)
		{
			if (pSYAPScanResult[j].Rssi >  pSYAPScanResult[j+1].Rssi)
			{
				Switched = TRUE;
				holdAP = pSYAPScanResult[j];
				pSYAPScanResult[j] = pSYAPScanResult[j+1];
				pSYAPScanResult[j+1] = holdAP;
			}
		}
	}
}


static void SYWIFIAPSiteSurvey(void)
{	
	PRTMP_ADAPTER pAd;
	UCHAR RTCipherAlg;
	PSY_CONTROL pSyControl;
	BSS_ENTRY *pRTAPBssEntry;
	WIFI_AP_RESULT_t *pSYAPScanResult;
	UCHAR *SYAPIEs;
	ULONG *SYIELength;
	int i, j;

	pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;

	pRTAPBssEntry = pAd->ScanTab.BssEntry;
	
	/* Sort AP by RSSI (from small to large) */
	SYAPSortByRSSI(pAd->ScanTab.BssNr, pRTAPBssEntry);

	pSyControl = &pAd->StaCfg.SYControl;
	if (pAd->ScanTab.BssNr > WIFI_MAX_AP_RESULTS)
		pSyControl->NumAPsFound = WIFI_MAX_AP_RESULTS;
	else
		pSyControl->NumAPsFound = pAd->ScanTab.BssNr;
	
	pSYAPScanResult = pSyControl->SYWiFiAPResult.pSYAPScanResult;

	NdisZeroMemory(pSYAPScanResult, sizeof(*pSYAPScanResult) * pSyControl->NumAPsFound);

	SYIELength = pSyControl->SYWiFiAPResult.IELength;
	SYAPIEs = pSyControl->SYWiFiAPResult.IEs;

	/* Get driver scan table result and store to SY specific AP result*/
	for (i = 0, j = (pAd->ScanTab.BssNr - 1); i < pSyControl->NumAPsFound; i++, j--)
	{	
		/* BSSID */ 
		NdisMoveMemory(pSYAPScanResult[i].BSSID, pRTAPBssEntry[j].Bssid, MAC_ADDR_LEN);
		
		/* RSSI */
		pSYAPScanResult[i].RSSI = pRTAPBssEntry[j].Rssi - pAd->BbpRssiToDbmDelta;

		/* Beacon Interval */
		pSYAPScanResult[i].BcnInterval = pRTAPBssEntry[j].BeaconPeriod;

		/* CapInfo */
		pSYAPScanResult[i].CapInfo = pRTAPBssEntry[j].CapabilityInfo;

		/* SSID */
		NdisMoveMemory(pSYAPScanResult[i].SSID, pRTAPBssEntry[j].Ssid, pRTAPBssEntry[j].SsidLen);

		/* Channel */
		pSYAPScanResult[i].Channel = pRTAPBssEntry[j].Channel;

		/* Encrption */
		RTCipherAlg = CIPHER_WEP64;
		SYGetAPWifiCipherSuites(&pRTAPBssEntry[j].AuthMode, &pRTAPBssEntry[j].WepStatus, 
							  &RTCipherAlg,
							  &pSYAPScanResult[i].Encr);

 		/* WPS Capability */
		/* TODO: What should it mean? use WpsAP to represent */
		pSYAPScanResult[i].WPSCap = pRTAPBssEntry[j].WpsAP;

		/* IE */
		if (!pRTAPBssEntry[j].VarIELen)
		{
			SYIELength[i] = sizeof(NDIS_802_11_FIXED_IEs);		
			NdisMoveMemory(SYAPIEs, &pRTAPBssEntry[j].FixIEs, sizeof(NDIS_802_11_FIXED_IEs));
		}else
		{
			SYIELength[i] = sizeof(NDIS_802_11_FIXED_IEs) + pRTAPBssEntry[j].VarIELen;
			NdisMoveMemory(SYAPIEs, &pRTAPBssEntry[j].FixIEs, sizeof(NDIS_802_11_FIXED_IEs));
			NdisMoveMemory(SYAPIEs + sizeof(NDIS_802_11_FIXED_IEs), pRTAPBssEntry[j].VarIEs, 
																	pRTAPBssEntry[j].VarIELen);
		}
		SYAPIEs += MAX_VIE_LEN;
	}
}


static int SYWIFIAPScanTrigger(IN PRTMP_ADAPTER pAd)
{
	int ret;
	PSY_CONTROL pSyControl;
	
	pSyControl = &pAd->StaCfg.SYControl;
	pSyControl->SYWiFiAPScanTrigger = TRUE;

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("AP Scan In Progress"));
		return NDIS_STATUS_SUCCESS;
	}
	
	if (pAd->Mlme.CntlMachine.CurrState != CNTL_IDLE)
	{
		RTMP_MLME_RESET_STATE_MACHINE(pAd);
		DBGPRINT(RT_DEBUG_TRACE, ("!!! MLME busy, reset MLME state machine !!!\n"));
	}

	/* tell CNTL state machine to call NdisMSetInformationComplete() after completing */
	/* this request, because this request is initiated by NDIS. */
	pAd->MlmeAux.CurrReqIsFromNdis = FALSE;
	/* Reset allowed scan retries */
	pAd->StaCfg.ScanCnt = 0;

	/* Trigger AP scan first */
	ret = MlmeEnqueue(pAd, 
					    MLME_CNTL_STATE_MACHINE,
					    OID_802_11_BSSID_LIST_SCAN,
					    0,
					    NULL, 0);

	if (!ret)
		return NDIS_STATUS_FAILURE;

	RTMP_MLME_HANDLER(pAd);

	return NDIS_STATUS_SUCCESS;
}


static void SYWiFiAPScanCallbackHdlr(IN ULONG APScanCallback)
{
	PRTMP_ADAPTER pAd;
	BOOLEAN Cancelled;
	PSY_CONTROL pSyControl;
	
	pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;
	pSyControl = &pAd->StaCfg.SYControl;

	/* Deactive AP Scan timer(for timeout) */
	if (pSyControl->SYWiFiAPScanTimeOut)
		RTMPCancelTimer(&pSyControl->SYWIFIAPScanTimer, &Cancelled);

	/* Get collected AP infomation into SYcontrol */
	SYWIFIAPSiteSurvey();
	
	/* Return AP results to system manager */
	((WifiAccessPointsCallback *)(APScanCallback))(pSyControl->NumAPsFound, pSyControl->SYWiFiAPResult.pSYAPScanResult);
}


static inline void SYMapDCWPSStatus(IN INT32 *pRTWPSStatus, 
									OUT DC_WIFI_WPS_STATUS *pSYDCWPSStatus)
{
	if (*pRTWPSStatus <= WPS_STATUS_TABLE_BOUNDARY)
		*pSYDCWPSStatus = SYWPSStatusMapTble1[*pRTWPSStatus].DCWiFiWPSStatus;
	else
		*pSYDCWPSStatus = SYWPSStatusMapTble2[*pRTWPSStatus].DCWiFiWPSStatus;
}


void SYWIFIWPSTimerTimeout(IN  PVOID SystemSpecific1, 
						   IN  PVOID FunctionContext, 
						   IN  PVOID SystemSpecific2, 
						   IN  PVOID SystemSpecific3)
{
	PRTMP_ADAPTER pAd;
	PSY_CONTROL pSyControl;

	pAd = (RTMP_ADAPTER *)FunctionContext;
	pSyControl = &pAd->StaCfg.SYControl;

	RTMPSendWirelessEvent(pAd , IW_WPS_END_EVENT_FLAG, NULL, BSS0, 0);

	/* Notify WPS to STOP */
	Set_WscStop_Proc(pAd, SYWSC_STOP);

	pSyControl->SYWiFiAPScanTrigger = FALSE;

	/* Call WPS callback directly */
	(pSyControl->pSYWiFiWPSCallback)(pSyControl->SYWPSStatus, &pSyControl->SYWPSWiFiIfaceParams);
	
}


VOID SYWIFIWPSFoolProofTimerTimeout(
	IN  PVOID SystemSpecific1, 
    IN  PVOID FunctionContext, 
    IN  PVOID SystemSpecific2, 
    IN  PVOID SystemSpecific3)
{
	PRTMP_ADAPTER pAd;
	PWSC_CTRL   pWscControl;
	static int i = 0;

	pAd = (RTMP_ADAPTER *)FunctionContext;
	pWscControl = &pAd->StaCfg.WscControl;
	
	/* Notify WPS to select next cadidate if WPS has not started registration procedrue yet */
	if (pWscControl->WscState <= WSC_STATE_START)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("SY:FoolProof = %d\n", i));
		RTMPSendWirelessEvent(pAd , IW_WSC_NEXT_CANDIDATE, NULL, BSS0, 0);
		i++;
	}
}


static void SYGetWifiCipherSuites(IN NDIS_802_11_AUTHENTICATION_MODE *pRTAuthMode,
								  IN NDIS_802_11_WEP_STATUS *pRTWepStatus,
								  IN UCHAR *pRTCipherAlg,
								  OUT DC_WIFI_IFACE_ENCR *pSYEncr)
{
	if (*pRTAuthMode == Ndis802_11AuthModeOpen)
	{
		if (*pRTWepStatus == Ndis802_11WEPEnabled)
		{
			if (*pRTCipherAlg == CIPHER_WEP64)
				*pSYEncr = DC_WIFI_IFACE_ENCR_WEP64;
			else if (*pRTCipherAlg == CIPHER_WEP128)
				*pSYEncr = DC_WIFI_IFACE_ENCR_WEP128;
		}
		else
			*pSYEncr = DC_WIFI_IFACE_ENCR_NONE;
	}
	else if (*pRTAuthMode == Ndis802_11AuthModeShared)
	{
		if (*pRTWepStatus == Ndis802_11WEPEnabled)
		{
			if (*pRTCipherAlg == CIPHER_WEP64)
				*pSYEncr = DC_WIFI_IFACE_ENCR_WEP64;
			else if (*pRTCipherAlg == CIPHER_WEP128)
				*pSYEncr = DC_WIFI_IFACE_ENCR_WEP128;
		}
	}
	else if (*pRTAuthMode == Ndis802_11AuthModeWPAPSK)
	{
		if (*pRTWepStatus == Ndis802_11TKIPEnable)
			*pSYEncr = DC_WIFI_IFACE_ENCR_WPAPSK_TKIP;
		else if (*pRTWepStatus == Ndis802_11AESEnable)
			*pSYEncr = DC_WIFI_IFACE_ENCR_WPAPSK_AES;
		else
			DBGPRINT(RT_DEBUG_ERROR, ("Unknown ciphersuite(RTAuthMode = %x, pRTWepStatus = %x", 
										*pRTAuthMode, *pRTWepStatus));
	}
	else if (*pRTAuthMode == Ndis802_11AuthModeWPA2PSK)
	{
		if (*pRTWepStatus == Ndis802_11TKIPEnable)
			*pSYEncr = DC_WIFI_IFACE_ENCR_WPA2PSK_TKIP;
		else if (*pRTWepStatus == Ndis802_11AESEnable)
			*pSYEncr = DC_WIFI_IFACE_ENCR_WPA2PSK_AES;
		else
			DBGPRINT(RT_DEBUG_ERROR, ("Unknown ciphersuite(RTAuthMode = %x, pRTWepStatus = %x", 
										*pRTAuthMode, *pRTWepStatus));
	}	
}


static void SYSetWifiCipherSuites(IN DC_WIFI_IFACE_ENCR *pSYEncr, 
								  IN DC_WIFI_IFACE_AUTH *pSYAuth,
								  OUT NDIS_802_11_AUTHENTICATION_MODE *pRTAuthMode,
								  OUT NDIS_802_11_WEP_STATUS *pRTWepStatus)
{
	switch(*pSYEncr) {
	case DC_WIFI_IFACE_ENCR_NONE:
		*pRTAuthMode = Ndis802_11AuthModeOpen;
		*pRTWepStatus = Ndis802_11WEPDisabled;
		break;
		
	case DC_WIFI_IFACE_ENCR_WEP64:
	case DC_WIFI_IFACE_ENCR_WEP128:
		if (*pSYAuth == DC_WIFI_IFACE_AUTH_OPEN)
		{
			*pRTAuthMode = Ndis802_11AuthModeOpen;
			*pRTWepStatus = Ndis802_11WEPEnabled;
		}
		else if (*pSYAuth == DC_WIFI_IFACE_AUTH_SHAREDKEY)
		{
			*pRTAuthMode = Ndis802_11AuthModeShared;
			*pRTWepStatus = Ndis802_11WEPEnabled;
		}
		break;
		
	case DC_WIFI_IFACE_ENCR_WPAPSK_TKIP:
		*pRTAuthMode = Ndis802_11AuthModeWPAPSK;
		*pRTWepStatus = Ndis802_11TKIPEnable;	
		break;
		
	case DC_WIFI_IFACE_ENCR_WPAPSK_AES:
		*pRTAuthMode = Ndis802_11AuthModeWPAPSK;
		*pRTWepStatus = Ndis802_11AESEnable;
		break;
		
	case DC_WIFI_IFACE_ENCR_WPA2PSK_TKIP:
	 	*pRTAuthMode = Ndis802_11AuthModeWPA2PSK;
		*pRTWepStatus = Ndis802_11TKIPEnable;
		break;
		
	case DC_WIFI_IFACE_ENCR_WPA2PSK_AES:
		*pRTAuthMode = Ndis802_11AuthModeWPA2PSK;
		*pRTWepStatus = Ndis802_11AESEnable;
		break;
	default:
		DBGPRINT(RT_DEBUG_ERROR, ("Unknown ciphersuite(SYEncr = %x, SYAuth = %x", 
										*pSYEncr, *pSYAuth));
	}
}


static void MapWscAuthType(IN USHORT *pWSCAuthType,
					OUT NDIS_802_11_AUTHENTICATION_MODE *pRTAuthMode)
{
	switch(*pWSCAuthType){
	case WSC_AUTHTYPE_OPEN:
		*pRTAuthMode = Ndis802_11AuthModeOpen;
		break;
	case WSC_AUTHTYPE_WPAPSK:
		*pRTAuthMode = Ndis802_11AuthModeWPAPSK;
		break;
	case WSC_AUTHTYPE_SHARED:
		*pRTAuthMode = Ndis802_11AuthModeShared;
		break;
	case WSC_AUTHTYPE_WPA:
		*pRTAuthMode = Ndis802_11AuthModeWPA;
		break;
	case WSC_AUTHTYPE_WPA2:
		*pRTAuthMode = Ndis802_11AuthModeWPA2;
		break;
	case WSC_AUTHTYPE_WPA2PSK:
		*pRTAuthMode = Ndis802_11AuthModeWPA2PSK;
		break;
	case WSC_AUTHTYPE_WPANONE:
		*pRTAuthMode = Ndis802_11AuthModeWPANone;
		break;
	default:
		DBGPRINT(RT_DEBUG_ERROR, ("Unknown WSCAuthType = %x", *pWSCAuthType));
		break;
	}
}


static void MapWscEncrType(IN USHORT *pWSCEncrType,
					OUT NDIS_802_11_WEP_STATUS *pRTWepStatus)
{
	switch(*pWSCEncrType){
	case WSC_ENCRTYPE_NONE:
		*pRTWepStatus = Ndis802_11WEPDisabled;
		break;
	case WSC_ENCRTYPE_WEP:
		*pRTWepStatus = Ndis802_11WEPEnabled;
		break;
	case WSC_ENCRTYPE_TKIP:
		*pRTWepStatus = Ndis802_11TKIPEnable;
		break;
	case WSC_ENCRTYPE_AES:
		*pRTWepStatus = Ndis802_11AESEnable;
		break;
	default:
		DBGPRINT(RT_DEBUG_ERROR, ("Unknow WSCEncrType = %x", *pWSCEncrType));
		break;
	}
}


static inline void MapWscCipherAlg(IN USHORT *pWSCKeyLength,
							OUT UCHAR *RTCipherAlg)
{
	if (*pWSCKeyLength == 5 || *pWSCKeyLength == 10)
		*RTCipherAlg = CIPHER_WEP64;
	else if (*pWSCKeyLength == 13 || *pWSCKeyLength == 26)
		*RTCipherAlg = CIPHER_WEP128;
	else 
		DBGPRINT(RT_DEBUG_ERROR, ("Invalid Key Length (%d)\n", *pWSCKeyLength));
}


static void SYSetWscWiFiIfaceParams(IN PRTMP_ADAPTER pAd,
									OUT DC_WIFI_IFACE_PARAMS *pWiFiIfaceParams)
{
	PWSC_CTRL pWscControl;
	PWSC_CREDENTIAL pWscCredential;
	NDIS_802_11_AUTHENTICATION_MODE RTAuthMode;
	NDIS_802_11_WEP_STATUS RTWepStatus;
	UCHAR RTCipherAlg;

	/* XXX: Mode may not be directly assigned */
	pWiFiIfaceParams->Mode = DC_WIFI_IFACE_MODE_INFRA;
	
	pWscControl = &pAd->StaCfg.WscControl;
	pWscCredential = &pWscControl->WscProfile.Profile[0];

	MapWscAuthType(&pWscCredential->AuthType, &RTAuthMode);
	MapWscEncrType(&pWscCredential->EncrType, &RTWepStatus);
	MapWscCipherAlg(&pWscCredential->KeyLength, &RTCipherAlg);
	SYGetWifiCipherSuites(&RTAuthMode,
						  &RTWepStatus,
						  &RTCipherAlg,
						  &pWiFiIfaceParams->Encr);

	/* XXX: definition say it is used in AdHoc mode, so how about Infra mode */
	pWiFiIfaceParams->Chan = pAd->CommonCfg.Channel;

	NdisMoveMemory(pWiFiIfaceParams->Ssid, pWscCredential->SSID.Ssid, pWscCredential->SSID.SsidLength);

	if ((pWiFiIfaceParams->Encr == DC_WIFI_IFACE_ENCR_WEP64) ||
			(pWiFiIfaceParams->Encr == DC_WIFI_IFACE_ENCR_WEP128))
	{
		pWiFiIfaceParams->DefKey = pWscCredential->KeyIndex;
		NdisMoveMemory(pWiFiIfaceParams->Keys[pWscCredential->KeyIndex], 
						pWscCredential->Key, pWscCredential->KeyLength);
	}
	else if ((pWiFiIfaceParams->Encr == DC_WIFI_IFACE_ENCR_WPAPSK_TKIP) ||
				(pWiFiIfaceParams->Encr == DC_WIFI_IFACE_ENCR_WPAPSK_AES) ||
				(pWiFiIfaceParams->Encr == DC_WIFI_IFACE_ENCR_WPA2PSK_TKIP) ||
				(pWiFiIfaceParams->Encr == DC_WIFI_IFACE_ENCR_WPA2PSK_AES))
	{
		NdisMoveMemory(pWiFiIfaceParams->PassPh, pWscCredential->Key, pWscCredential->KeyLength);
	}
	
	/* TODO: */
	/* pWiFiIfaceParams->RegDomain = */
	
	/* Just because SY manager WPSMode start index less one than RT driver */
	pWiFiIfaceParams->WpsMode =  pWscControl->WscMode - 1;

	if (pWiFiIfaceParams->WpsMode == DC_WIFI_IFACE_WPS_MODE_PIN)
		NdisMoveMemory(pWiFiIfaceParams->WpsPIN, pWscControl->RegData.PIN, pWscControl->RegData.PinCodeLen);

	/* TODO: What should we do for below two callback functions?
		void (* CallBack) (DC_NET_DRV_LINK_STATUS, int) ;
    	void (* FatalErrorCallBack) (int) ;
    */
}


static void SYStoreWPSAPInfo(IN DC_WIFI_IFACE_PARAMS *pSYWPSWiFiIfaceParams, 
							 OUT DC_WIFI_MAC_CONFIGURATION *pWPSWIFIMacConfig)
{
	pWPSWIFIMacConfig->Mode = pSYWPSWiFiIfaceParams->Mode;
	pWPSWIFIMacConfig->Encr = pSYWPSWiFiIfaceParams->Encr;
	pWPSWIFIMacConfig->Auth = pSYWPSWiFiIfaceParams->Auth;
	pWPSWIFIMacConfig->Chan = pSYWPSWiFiIfaceParams->Chan;
	memcpy(pWPSWIFIMacConfig->Ssid, pSYWPSWiFiIfaceParams->Ssid, MAX_SSID_SIZE);

	if ((pWPSWIFIMacConfig->Encr == DC_WIFI_IFACE_ENCR_WEP64) ||
			(pWPSWIFIMacConfig->Encr == DC_WIFI_IFACE_ENCR_WEP128))
	{
		pWPSWIFIMacConfig->DefKey = pSYWPSWiFiIfaceParams->DefKey;

		for (int i =0; i < MAX_NUM_KEYS; i++)
			memcpy(&pWPSWIFIMacConfig->Keys[i][0], &pSYWPSWiFiIfaceParams->Keys[i][0], 16);
		
	}else if ((pWPSWIFIMacConfig->Encr == DC_WIFI_IFACE_ENCR_WPAPSK_TKIP) ||
				(pWPSWIFIMacConfig->Encr == DC_WIFI_IFACE_ENCR_WPAPSK_AES) ||
				(pWPSWIFIMacConfig->Encr == DC_WIFI_IFACE_ENCR_WPA2PSK_TKIP) ||
				(pWPSWIFIMacConfig->Encr == DC_WIFI_IFACE_ENCR_WPA2PSK_AES))
	{
		memcpy(pWPSWIFIMacConfig->PassPh, pSYWPSWiFiIfaceParams->PassPh ,MAX_PASSPHRASE_SIZE + 1);
	}
	
	pWPSWIFIMacConfig->RegDomain = pSYWPSWiFiIfaceParams->RegDomain;
	pWPSWIFIMacConfig->WpsMode = pSYWPSWiFiIfaceParams->WpsMode;

	if (pSYWPSWiFiIfaceParams->WpsMode == DC_WIFI_IFACE_WPS_MODE_PIN)
		memcpy(pWPSWIFIMacConfig->WpsPin, pSYWPSWiFiIfaceParams->WpsPIN, MAX_WPS_PIN_SIZE);
}


static void SYWiFiWPSCallbackHdlr(IN ULONG wpsCallback)
{
	PRTMP_ADAPTER pAd;
	BOOLEAN Cancelled;
	PSY_CONTROL pSyControl;
	DC_WIFI_MAC_CONFIGURATION WPSWIFIMacConfig;

	pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;
	pSyControl = &pAd->StaCfg.SYControl;

	/* Deactive WPS timer(for timeout) */
	if (pSyControl->SYWiFiWPSTimeOut)
		RTMPCancelTimer(&pSyControl->SYWIFIWPSTimer, &Cancelled);

	/* Get Current wifi NVRAM info */
	wifi_getMacConfig(&WPSWIFIMacConfig);

	/* Copy WPS AP info */
	SYStoreWPSAPInfo(&pSyControl->SYWPSWiFiIfaceParams, &WPSWIFIMacConfig);

	/* Store WPS AP into NVRAM */
	SYWriteNVRAM(&WPSWIFIMacConfig);

	((WPSCallback *)(wpsCallback))(pSyControl->SYWPSStatus, &pSyControl->SYWPSWiFiIfaceParams);
}


static inline void SYConvertRssiToSignalQua(IN INT32 *RSSI, OUT UINT *SignalQua)
{
	if (*RSSI >= -50)
		*SignalQua = 100;
	else if (*RSSI >= -80)
		*SignalQua = (unsigned int)(24 + (*RSSI + 80) * 2.6);
	else if (*RSSI >= -90)
		*SignalQua = (unsigned int)((*RSSI + 90) * 2.6);
	else 
		*SignalQua = 0;
}


static DC_WIFI_SIGNAL_RATING SYSignalStrenthRating(IN INT32 RSSI)
{
	UINT SignalQua;
	UINT SignalQuaLevel;
	UINT MaxSignalQualLevel;
	DC_WIFI_SIGNAL_RATING SignalRating;

	SignalQuaLevel = 0;
	MaxSignalQualLevel = sizeof(SigQuaRating) / sizeof(DC_WIFI_SIGNAL_RATING) - 1;
	SignalRating = DC_WIFI_SIGNAL_EXCELLENT;

	SYConvertRssiToSignalQua(&RSSI,&SignalQua);
	while ((SignalRating == DC_WIFI_SIGNAL_EXCELLENT)
			&& (SignalQuaLevel < MaxSignalQualLevel))
	{
		if (SignalQua < SigQuaRangeLimit[SignalQuaLevel])
			SignalRating = SigQuaRating[SignalQuaLevel];
		
		SignalQuaLevel++;
	}

	return SignalRating;
}


static inline DC_WIFI_IFACE_MODE SYBSSMode(IN UCHAR BssType)
{
	DC_WIFI_IFACE_MODE Mode;
	
	switch(BssType) {
	case BSS_ADHOC:
		Mode = DC_WIFI_IFACE_MODE_ADHOC;
		break;
	case BSS_INFRA:
		Mode = DC_WIFI_IFACE_MODE_INFRA;
		break;
	default:
		DBGPRINT(RT_DEBUG_ERROR, ("Unknow BSSMode = %x", BssType));
	}

	return Mode;
}


static DC_WIFI_STATE SYWiFiState(IN PRTMP_ADAPTER pAd)
{
	DC_WIFI_STATE state;

	/* TODO: */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))
		return DC_WIFI_DISABLED;

	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP))
		return DC_WIFI_INITIALIZING_RADIO;

	if(!(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_INFRA_ON) ||
		OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADHOC_ON)))
	{
		/* check state machine */
		if((pAd->Mlme.CntlMachine.CurrState == CNTL_WAIT_SCAN_FOR_CONNECT) 
			||((pAd->Mlme.CntlMachine.CurrState >= CNTL_WAIT_JOIN) && 
				(pAd->Mlme.CntlMachine.CurrState == CNTL_WAIT_AUTH2))
			) 
			return DC_WIFI_CONNECTING;
		else
	    	return DC_WIFI_DISCONNECTED;
	}else if(pAd->StaCfg.SYControl.SYWiFiWPSTrigger == TRUE)
			return DC_WIFI_CONNECTING;
	else if(pAd->IndicateMediaState != NdisMediaStateConnected || 
			pAd->StaCfg.PortSecured != WPA_802_1X_PORT_SECURED)  /* port secured */
	    return DC_WIFI_CONNECTING;
	else
		return DC_WIFI_CONNECTED;
	
    /*error states: */
    /*DC_WIFI_FATAL_ERROR_INITIALIZING_RADIO, */
    /*DC_WIFI_ERROR_CONNECTING */
}


static inline UINT32 SYCalculateKbps(IN PRTMP_ADAPTER pAd)
{
	UINT32 Kbps;

	/* TODO: */
}


static inline void SYMapWPSStatus(IN INT32 *pRTWPSStatus,
								  OUT WIFI_WPS_STATUS *pSYWPSStatus)
{
	if (*pRTWPSStatus <= WPS_STATUS_TABLE_BOUNDARY)
		*pSYWPSStatus = SYWPSStatusMapTble1[*pRTWPSStatus].WiFiWPSStatus;
	else
		*pSYWPSStatus = SYWPSStatusMapTble2[*pRTWPSStatus].WiFiWPSStatus;
}


void SYAdjustSTAParamFromNVRAM(IN PRTMP_ADAPTER pAd)
{
	DC_WIFI_MAC_CONFIGURATION WiFiMacConf;
	CIPHER_KEY	*pSharedKey;
	char RegDomain[3];
	PWSC_CTRL	pWscControl;

	wifi_getMacConfig(&WiFiMacConf);

	/* NetWorkType */
	if(WiFiMacConf.Mode == DC_WIFI_IFACE_MODE_INFRA)
		pAd->StaCfg.BssType = BSS_INFRA;
	else if (WiFiMacConf.Mode == DC_WIFI_IFACE_MODE_ADHOC)
		pAd->StaCfg.BssType = BSS_ADHOC;

	/* Cipher Suites */
	SYSetWifiCipherSuites(&WiFiMacConf.Encr, &WiFiMacConf.Auth, 
							&pAd->StaCfg.AuthMode, &pAd->StaCfg.WepStatus);
	
	RTMPSetSTACipherSuites(pAd, pAd->StaCfg.WepStatus);

	/* Channel */
	pAd->CommonCfg.Channel = WiFiMacConf.Chan;

	/* SSID */
	RTMPSetSTASSID(pAd, WiFiMacConf.Ssid);

	/* Default Key */
	RTMPSetSTADefKeyId(pAd, (ULONG)WiFiMacConf.DefKey);

	/* WEP Keys */
	for (int KeyIdx = 0; KeyIdx < MAX_NUM_KEYS; KeyIdx++)
	{
		pSharedKey = &pAd->SharedKey[BSS0][KeyIdx];
		RT_CfgSetWepKey(pAd, &WiFiMacConf.Keys[KeyIdx][0], pSharedKey, KeyIdx);
	}

	/* PASSPHRASE */
	RTMPSetSTAPassPhrase(pAd, WiFiMacConf.PassPh);

	/* RegDomain */
	sprintf(RegDomain, "%d", WiFiMacConf.RegDomain);
	RTMPSetCountryCode(pAd, RegDomain);

	/* RadioEnabled */
	/* TODO: Check this field meaning */
	/* WiFiMacConf.RadioEnabled; */

	/* WPS mode */
	pWscControl = &pAd->StaCfg.WscControl;
	/* Just because SY manager WPSMode start index less one than RT driver */
	pWscControl->WscMode = (WiFiMacConf.WpsMode + 1);

	/* WPS pin code */
	Set_WscPinCode_Proc(pAd, WiFiMacConf.WpsPin);

	/* Mac address */	
	/* TODO: This field need to adjust? */
	/* WiFiMacConf.MacAddr */
}


static inline int SYReconnectSSID(RTMP_ADAPTER *pAd, RTMP_STRING *SSID)
{
	int ret;
	
	/* Check if the wifi interface is down */
    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
       	return FALSE;   
    }

	ret = Set_SSID_Proc(pAd, SSID);
	
	return ret;
}


static inline void SYWiFiInitWPS(IN PRTMP_ADAPTER pAd, 
								 IN WPSCallback *pWPSCallback, 
								 IN UINT16 stime)
{
	PSY_CONTROL pSyControl;

	pSyControl = &pAd->StaCfg.SYControl;
	pSyControl->pSYWiFiWPSCallback = pWPSCallback;
	pSyControl->SYWiFiWPSTrigger = TRUE;
	pSyControl->SYWiFiWPSSuccess = FALSE;
	pSyControl->StopWpsAPSearch = FALSE;

	/* Active WPS timer(for time out)  */
	pSyControl->SYWiFiWPSTimeOut = stime;
	
	if (pSyControl->SYWiFiWPSTimeOut)
		RTMPSetTimer(&pSyControl->SYWIFIWPSTimer, stime * 1000);
}


static void SYFindCandidateWpsAP(IN PRTMP_ADAPTER pAd,
							  	 IN USHORT DPID_FROM_STA,
							   	 OUT WIFI_AP_RESULT_t *pSYCandidate)
{
#define WPS_IE_ID 221

	BSS_ENTRY *pRTAPBssEntry;
	PSY_CONTROL pSyControl;
	WIFI_AP_RESULT_t *pSYAPScanResult;
	UINT32 APIndex;
	BOOLEAN CandidateAPFound;
	UCHAR *SYAPIEs;
	ULONG *SYIELength;

	pRTAPBssEntry = pAd->ScanTab.BssEntry;
	pSyControl = &pAd->StaCfg.SYControl;
	pSYAPScanResult = pSyControl->SYWiFiAPResult.pSYAPScanResult;
	SYIELength = pSyControl->SYWiFiAPResult.IELength;
	SYAPIEs = &pSyControl->SYWiFiAPResult.IEs[0][0];

	CandidateAPFound = FALSE;
	APIndex = 0;
	while (!CandidateAPFound && APIndex < pSyControl->NumAPsFound)
	{
		/* Find matched Device Password ID with selected registrar */
		UINT IELoc = 0;
		SYAPIEs += 	MAX_VIE_LEN;
		PNDIS_802_11_FIXED_IEs pFixIE = (PNDIS_802_11_FIXED_IEs)SYAPIEs;
		PNDIS_802_11_VARIABLE_IEs pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pFixIE 
														+ sizeof(NDIS_802_11_FIXED_IEs));
		IELoc += sizeof(NDIS_802_11_FIXED_IEs);

		while (SYIELength[APIndex] > (IELoc + sizeof(NDIS_802_11_VARIABLE_IEs)))
		{
			if ((pVarIE->ElementID == WPS_IE_ID) &&  (pVarIE->Length >= 14))
			{
				USHORT	DPID_FROM_AP = 0xFFFF;

				if (memcmp(pVarIE->data, WPS_OUI, 4) == 0)
				{
					WscCheckWpsIeFromWpsAP(pAd, (PEID_STRUCT)pVarIE, &DPID_FROM_AP);
	
					if (DPID_FROM_AP == DPID_FROM_STA)
					{
						CandidateAPFound = TRUE;
						break;
					}
				}
			}

			IELoc += pVarIE->Length;
			IELoc += 2;
			pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);
			if(pVarIE->Length <= 0)
				break;
		}
		
		APIndex++;
	}

	if (CandidateAPFound)
		*pSYCandidate = pSYAPScanResult[APIndex];
	else
		*pSYCandidate = pSYAPScanResult[0]; /* return first AP */
}


static void SYWPSPinContinueNetTask(unsigned long data)
{
	PRTMP_ADAPTER pAd;
	PSY_CONTROL pSyControl;
	WIFI_AP_RESULT_t *pSYAPScanResult;
	WIFI_AP_RESULT_t SYCandidate;
	UINT32 APIndex;
	INT ret;
	
	pAd = (PRTMP_ADAPTER)data;

	pSyControl = &pAd->StaCfg.SYControl;

SY_WPS_PIN_CONTINUE_START:

	/* Set Site Suvery flag for get site suvey after scan done */
	pSyControl->SYAPSiteSurveyOnly = TRUE;

	/* Scan AP to collect AP */
	SYWIFIAPScanTrigger(pAd);
		
	/* Wait for scan done wireless event */
	RTMP_SEM_EVENT_WAIT(&pSyControl->SYWaitScanDoneSem, ret);

	/* TODO: select more candidates */
	/* Find WPS AP candidates with DPID mode match, if not match return first AP to be a candidate */
	SYFindCandidateWpsAP(pAd, DEV_PASS_ID_PIN, &SYCandidate);

	/* Iterate each candidates to do WPS PIN mode */
	APIndex = 0;
	pSYAPScanResult = pSyControl->SYWiFiAPResult.pSYAPScanResult;
	while (pSyControl->StopWpsAPSearch != TRUE && APIndex < pSyControl->NumAPsFound)
	{
		/* Filter out WPSless Capability AP */
		if (!SYCandidate.WPSCap)
		{
			SYCandidate = pSYAPScanResult[++APIndex];
			continue;
		}
		
		/* Role: Enrollee */
		Set_WscConfMode_Proc(pAd, SYROLE_ENROLLEE);

		/* SSID */
		Set_WscSsid_Proc(pAd, (RTMP_STRING *)SYCandidate.SSID);
		
		pSyControl->SYWPSPinMode = TRUE;

		/* Trigger WPS registration protocol */
		Set_WscGetConf_Proc(pAd, SYWSC_TRIGGER);

		/* Active WPS fool proof timer */
		pSyControl->SYWiFiWPSFoolProofTimeOut = FOOL_PROOF_TIMEOUT;
		RTMPSetTimer(&pSyControl->SYWIFIWPSFoolProofTimer, pSyControl->SYWiFiWPSFoolProofTimeOut);

		DBGPRINT(RT_DEBUG_TRACE, ("SY:PinContinue(APIndex = %d, NumsAP = %d, SSID = %s)\n",
						APIndex, pSyControl->NumAPsFound, SYCandidate.SSID));

		/* Wait for WPS registration wireless event */
		RTMP_SEM_EVENT_WAIT(&pSyControl->SYWaitDoWPSPinSem, ret);
		
		SYCandidate = pSYAPScanResult[++APIndex];
	}
	
	if (pSyControl->StopWpsAPSearch != TRUE)
		goto SY_WPS_PIN_CONTINUE_START;
}


static inline int SYNetTaskInit(IN PRTMP_ADAPTER pAd)
{
	PSY_CONTROL pSyControl;
	int ret;

	pSyControl = &pAd->StaCfg.SYControl;
	/* Create the NetTask Queue for  "SY Net Jobs" to execute */
	ret = RTMPCreateNetTaskQueue("SYNetTask", RTMP_TX_TASK_PRIORITY - 1, 1, &pSyControl->SYNetTaskQueue);

	if (ret != NDIS_STATUS_SUCCESS)
		return ret;

	RTMP_NET_TASK_INIT(&pSyControl->WPSPinContinueTask, SYWPSPinContinueNetTask, 
															(unsigned long)pAd);
	return ret;
}


int SYControlInit(IN PRTMP_ADAPTER pAd)
{
	PSY_CONTROL pSyControl;
	WIFI_AP_RESULT_t *pSYAPScanResult;
	UCHAR *pSYAPIEs;
	
	int ret;

	pSyControl = &pAd->StaCfg.SYControl;
	
	/* Get default configuration from NVRAM */
	wifi_getMacConfig(&pSyControl->CurrentWiFiMacConf);

	RTMPInitTimer(pAd,
				  &pSyControl->SYWIFIAPScanTimer,
				  GET_TIMER_FUNCTION(SYWIFIAPScanTimerTimeout),
				  pAd,
				  FALSE);

	pSyControl->SYWiFiAPScanTrigger = FALSE;
	pSyControl->SYAPSiteSurveyOnly = FALSE;

	pSYAPScanResult = kmalloc(sizeof(WIFI_AP_RESULT_t) * WIFI_MAX_AP_RESULTS, GFP_KERNEL);

	if (!pSYAPScanResult)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Memory is not availabe for AP results!!"));
		goto ERROR_FAIL;
	}

	pSyControl->SYWiFiAPResult.pSYAPScanResult = pSYAPScanResult;

	ret = SYNetTaskInit(pAd);

	if (ret != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("SYTask init fail"));
		goto ERROR_FREE_AP_RESULT_MEM;
	}
		
#ifdef WSC_STA_SUPPORT
	RTMPInitTimer(pAd,
				  &pSyControl->SYWIFIWPSTimer,
				  GET_TIMER_FUNCTION(SYWIFIWPSTimerTimeout),
				  pAd,
				  FALSE);

	RTMPInitTimer(pAd,
				  &pSyControl->SYWIFIWPSFoolProofTimer,
				  GET_TIMER_FUNCTION(SYWIFIWPSFoolProofTimerTimeout),
				  pAd,
				  FALSE);
	
	pSyControl->SYWiFiWPSTrigger = FALSE;
	pSyControl->SYWiFiWPSSuccess = FALSE;
	pSyControl->SYWPSPinMode = FALSE;
	pSyControl->StopWpsAPSearch = FALSE;
	RTMP_SEM_EVENT_INIT_LOCKED(&pSyControl->SYWaitScanDoneSem, &pAd->RscSemMemList);
	RTMP_SEM_EVENT_INIT_LOCKED(&pSyControl->SYWaitDoWPSPinSem, &pAd->RscSemMemList);
#endif /* WSC_STA_SUPPORT */
		
	return NDIS_STATUS_SUCCESS;

ERROR_FREE_AP_RESULT_MEM:
	kfree(pSYAPScanResult);

ERROR_FAIL:
	return NDIS_STATUS_FAILURE;
}


/* TODO: */
inline int SYControlExit(IN PRTMP_ADAPTER pAd)
{
	PSY_CONTROL pSyControl;
	WIFI_AP_RESULT_t *pSYAPScanResult;

	pSyControl = &pAd->StaCfg.SYControl;
	pSYAPScanResult = pSyControl->SYWiFiAPResult.pSYAPScanResult;
	kfree(pSYAPScanResult);
	RTMP_SEM_EVENT_DESTORY(&pSyControl->SYWaitScanDoneSem);
	RTMP_SEM_EVENT_DESTORY(&pSyControl->SYWaitDoWPSPinSem);
	RTMPDestroyNetTaskQueue(&pSyControl->SYNetTaskQueue);
	
	return NDIS_STATUS_SUCCESS;
}


DC_RETURN_CODE SYWriteNVRAM(DC_WIFI_MAC_CONFIGURATION *structP)
{
	DC_RETURN_CODE ret;
	
	ret = dc_nvram_object_writelock(&NVRAMWIFISetting, TX_WAIT_FOREVER);

	if (ret != DC_SUCCESS)
	{
		dc_log_printf("NVRAMWIFISetting wrlock fail(%x)", ret);
		return ret;
	}

	NdisMoveMemory(NVRAMWIFISetting.image, structP, sizeof(*structP));
	
	ret = dc_nvram_object_unlock(&NVRAMWIFISetting, TX_WAIT_FOREVER);

	if (ret != DC_SUCCESS)
	{
		dc_log_printf("NVRAMWIFISetting unlock fail(%x)", ret);
		return ret;
	}
}

/* SY Wireless Event Handler */
/* IW_SCAN_COMPLETED_EVENT_FLAG */
inline void SYScanCompletedEventHdlr(IN PRTMP_ADAPTER pAd,
							 	 	 IN UCHAR wdev_idx)
{
	PSY_CONTROL pSyControl; 
	
	pSyControl = &pAd->StaCfg.SYControl;

	if (pSyControl->SYAPSiteSurveyOnly == TRUE)
	{
		SYWIFIAPSiteSurvey();
		pSyControl->SYAPSiteSurveyOnly = FALSE;
		RTMP_SEM_EVENT_UP(&pSyControl->SYWaitScanDoneSem);
	}
	else
		SYWiFiAPScanCallbackHdlr(pSyControl->pSYWiFiAPScanCallback);
			
	pSyControl->SYWiFiAPScanTrigger = FALSE;
}


#ifdef WSC_STA_SUPPORT
/* IW_WPS_END_EVENT_FLAG */
inline void SYWPSEndEventHdlr(IN PRTMP_ADAPTER pAd,
							  IN UCHAR wdev_idx)
{
	PSY_CONTROL pSyControl;
	PWSC_CTRL pWscControl;
	
	pSyControl = &pAd->StaCfg.SYControl;
	pWscControl = &pAd->StaCfg.WscControl;

	SYSetWscWiFiIfaceParams(pAd, &pSyControl->SYWPSWiFiIfaceParams);
	SYMapDCWPSStatus(&pWscControl->WscStatus, &pSyControl->SYWPSStatus);

	if (pWscControl->WscStatus == STATUS_WSC_CONFIGURED)
		pSyControl->SYWiFiWPSSuccess = TRUE;
}


/* IW_WSC_NEXT_CANDIDATE */
inline void SYNextCanidatedEventHdlr(IN PRTMP_ADAPTER pAd,
									 IN UCHAR wdev_idx)
{
	PSY_CONTROL pSyControl;
	
	pSyControl = &pAd->StaCfg.SYControl;
	
	/* WPS Pin Mode */
	if (pSyControl->SYWPSPinMode)
	{
		/* Notify WPS to STOP to continue next candidate */
		Set_WscStop_Proc(pAd, SYWSC_STOP);
					
		RTMP_SEM_EVENT_UP(&pSyControl->SYWaitDoWPSPinSem);
	}
}


/* IW_WSC_STATUS_FAIL */
void SYWSCStatusFailEventHdlr(IN PRTMP_ADAPTER pAd,
							  IN UCHAR wdev_idx)
{
	PSY_CONTROL pSyControl;
	PWSC_CTRL pWscControl;
	
	pSyControl = &pAd->StaCfg.SYControl;
	pWscControl = &pAd->StaCfg.WscControl;
	
	if(pWscControl->WscState < WSC_STATE_WAIT_M4)
	{
		/* WPS Pin Mode */
		if (pSyControl->SYWPSPinMode)
		{
			/* Notify WPS to STOP to continue next candidate */
			Set_WscStop_Proc(pAd, SYWSC_STOP);
					
			RTMP_SEM_EVENT_UP(&pSyControl->SYWaitDoWPSPinSem);
		}
	}
	else
	{
		SYWiFiWPSCallbackHdlr(pSyControl->pSYWiFiWPSCallback);
			
		pSyControl->StopWpsAPSearch = TRUE;
			
		/* WPS Pin Mode */
		if (pSyControl->SYWPSPinMode)
		{
			RTMP_SEM_EVENT_UP(&pSyControl->SYWaitDoWPSPinSem);
		}
		
		pSyControl->SYWiFiWPSTrigger = FALSE;
	}
}


/* IW_STA_LINKUP_EVENT_FLAG */
void SYSTALinkUpEventHdlr(IN PRTMP_ADAPTER pAd,
						  IN UCHAR wdev_idx)
{
	PSY_CONTROL pSyControl; 
	
	pSyControl = &pAd->StaCfg.SYControl;

	/* If becoming port secured is not after WPS, return */
	if (pSyControl->SYWiFiWPSSuccess == FALSE)
		return;

	SYWiFiWPSCallbackHdlr(pSyControl->pSYWiFiWPSCallback);
			
	/* WPS Pin Mode */
	if (pSyControl->SYWPSPinMode)
	{
		pSyControl->StopWpsAPSearch = TRUE;
		RTMP_SEM_EVENT_UP(&pSyControl->SYWaitDoWPSPinSem);
	}

	pSyControl->SYWiFiWPSTrigger = FALSE;
}
#endif /* WSC_STA_SUPPORT */

/******************************************************************************
* SigmaTel PUBLIC FUNCTION
******************************************************************************/

/*************************************************************************/*** */
*  1.This routine handles Wifi Mac Configuration command.  The settings struct 
*    will be saved in NVRAM and is therefore valid across power   
*    cycles.
*  2.Update wifi driver parameters and reconnect if necessary 
*  @param structP - The Wifi MAC Configuration 
*/
void wifi_setMacConfig(IN DC_WIFI_MAC_CONFIGURATION *structP)
{
	DC_RETURN_CODE ret;
	PRTMP_ADAPTER pAd;
	PSY_CONTROL pSyControl;

	pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;

	ret = SYWriteNVRAM(structP);
	
	/* Update wifi driver parameters */
	SYAdjustSTAParamFromNVRAM(pAd);

	if (structP->RadioEnabled)
	{
		MlmeRadioOn(pAd);
		ret = SYReconnectSSID(pAd, structP->Ssid); /* Reconnect if radio on */
	}
	else
		MlmeRadioOff(pAd); /* Radio off wifi */

	pSyControl = &pAd->StaCfg.SYControl;
	/* Change the last known wifi configuration value */
	pSyControl->CurrentWiFiMacConf = *structP;
}


/*
*  This routine returns the current wifi MACsettings.  The settings struct 
*  contained is used for configuration of the WifUnit.  
*   
*  @param structP ¡V The Wifi MAC Configuration settings. 
*/
void wifi_getMacConfig(OUT DC_WIFI_MAC_CONFIGURATION *structP)
{

	DC_RETURN_CODE ret;

	ret = dc_nvram_object_readlock(&NVRAMWIFISetting, TX_WAIT_FOREVER);
	
	if (ret != DC_SUCCESS)
	{
		dc_log_printf("NVRAMWIFISetting rdlock fail(%x)", ret);
		return;
	}
	
	NdisMoveMemory(structP, NVRAMWIFISetting.image, sizeof(*structP));
		
	ret = dc_nvram_object_unlock(&NVRAMWIFISetting, TX_NO_WAIT);

	if (ret != DC_SUCCESS)
	{
		dc_log_printf("NVRAMWIFISetting unlock fail(%x)", ret);
		return;
	}
}


/*
@description: 
Wifi_configUpdate compares the current DC_WIFI_MAC_CONFIGURATION with the one stored in nvram 
and if they are different is uses the wifi_setMacConfig to change it, if they are the same 
it raises a reconnect event.
*/
void wifi_configUpdate(IN DC_WIFI_MAC_CONFIGURATION *structP)
{
	PRTMP_ADAPTER pAd;
	PSY_CONTROL pSyControl;

	if (structP)
	{
		pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;
		pSyControl = &pAd->StaCfg.SYControl;
		if (memcmp(&pSyControl->CurrentWiFiMacConf, structP, sizeof(DC_WIFI_MAC_CONFIGURATION)))
			wifi_setMacConfig(structP);
		else
			SYReconnectSSID(pAd, structP->Ssid); /* Now, forcibly reconnect */
	}
}


/*
@description:  This function enables the WiFi Subsystem.  It also updates the NVRAM 
structure to configure the WiFi subsystem for subsequent AIO power up sequences.
*/
void wifi_power(IN boolean powerOn)
{
	DC_WIFI_MAC_CONFIGURATION WiFiConfig;

	wifi_getMacConfig(&WiFiConfig);

	/* radio on/off wifi */
	if (WiFiConfig.RadioEnabled != powerOn)
	{
		WiFiConfig.RadioEnabled = powerOn;
		wifi_setMacConfig(&WiFiConfig);
	}
}


/*
@description:  When this function is called, WiFi module will return status.  
@params   *structP ¡V pointer to the WiFi status structure which is declared as follows: 
*/
void wifi_getWifiStatus(OUT DC_WIFI_STATUS *structP)
{
	PRTMP_ADAPTER pAd;
	PWSC_CTRL pWscControl;

	pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;

	if(!pAd)
	{
		return;
	}

	structP->rssi = (INT32)(pAd->StaCfg.RssiSample.LastRssi[0] - pAd->BbpRssiToDbmDelta);
	structP->signalRating = SYSignalStrenthRating(structP->rssi);
	structP->kbpsSpeed = SYCalculateKbps(pAd);
	structP->state = SYWiFiState(pAd);
	structP->nwkInfo.Mode = SYBSSMode(pAd->StaCfg.BssType);
	
	SYGetWifiCipherSuites(&pAd->StaCfg.AuthMode, &pAd->StaCfg.WepStatus,
							&pAd->SharedKey[BSS0][0].CipherAlg,
								&structP->nwkInfo.Auth);
	
	structP->nwkInfo.RegDomain = pAd->CommonCfg.CountryRegion;
	NdisMoveMemory(structP->nwkInfo.BSSID, pAd->CommonCfg.Bssid, MAC_ADDR_LEN);
	NdisMoveMemory(structP->nwkInfo.Ssid, pAd->CommonCfg.Ssid, pAd->CommonCfg.SsidLen);
	structP->nwkInfo.Chan = pAd->CommonCfg.Channel;
	pWscControl = &pAd->StaCfg.WscControl;
	SYMapWPSStatus(&pWscControl->WscStatus, &structP->wps_status);
}


/*
@description:   When  this  function  is  called,  WiFi  module  will  get  a  list  of  available 
access points.

@params   UINT16 stime: Time in seconds to search for access points.  
                   *CallBack:  The callback declaration is as follows: 
 
typedef void WifiAccessPointsCallback(UINT16 numAPs, WIFI_AP_RESULT_t   *result); 
 
UINT16 numAPs ¡V number of access points found 
*result  WIFI_AP_RESULT_t ¡V pointer to an array of access point information structures   

*/
DC_RETURN_CODE wifi_APscan(IN UINT16 stime,
						   IN WifiAccessPointsCallback *pWiFiAPScancallback)
{
	DC_RETURN_CODE ret;
	PRTMP_ADAPTER pAd;
	PSY_CONTROL pSyControl;
	
	pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;

	/* Check if network interface is down */
	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Networks is down!!"));
		return DC_FAILURE;
	}

	if ((OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED)) &&
			((pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA) ||
			(pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPAPSK)) &&
			(pAd->StaCfg.PortSecured == WPA_802_1X_PORT_NOT_SECURED))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("!!! Link UP, Port Not Secured! ignore this set::OID_802_11_BSSID_LIST_SCAN\n"));
		return DC_FAILURE;
	}

	pSyControl = &pAd->StaCfg.SYControl;
	pSyControl->pSYWiFiAPScanCallback = pWiFiAPScancallback;
	pSyControl->SYWiFiAPScanTimeOut = stime;

	ret = SYWIFIAPScanTrigger(pAd);

	if (ret != NDIS_STATUS_SUCCESS)
		return DC_FAILURE;

	/* Active AP Scan timer(for time out)  */	
	if (stime)		
		RTMPSetTimer(&pSyControl->SYWIFIAPScanTimer, stime * 1000);

	return DC_SUCCESS;
}


/*
@description:	
	Respond to either push button or pin mode. In push button mode the wps connect is initialized right away
   	. In Pin mode the driver generates the pin and calls the WPSCallback right away, The SY will calls the
   	wifi_WPS_pin_continue() to initialize wps connect
 */
DC_RETURN_CODE wifi_WPS(IN UINT16 stime, 
						IN void *pWPSCallback)
{
	DC_RETURN_CODE ret;
	WPS_MODE WPSMode;
	PRTMP_ADAPTER pAd;
	PWSC_CTRL pWscControl;
	PSY_CONTROL pSyControl;

	pAd = (PRTMP_ADAPTER)RT_WIFI_DevCtrl;

	pSyControl = &pAd->StaCfg.SYControl;
	WPSMode = pSyControl->CurrentWiFiMacConf.WpsMode;
	
	/* WPS mode */
	if (WPSMode == WPS_MODE_PBC) /* PBC Mode */
	{
		SYWiFiInitWPS(pAd, pWPSCallback, stime);

		/* Role: Enrollee */
		ret = Set_WscConfMode_Proc(pAd, SYROLE_ENROLLEE);

		if (!ret)
			return DC_FAILURE;

		/* PBC Mode */
		ret = Set_WscMode_Proc(pAd, SYWSC_PBC_MODE);

		if (!ret)
			return DC_FAILURE;

		/* Trigger WPS registration protocol */
		ret = Set_WscGetConf_Proc(pAd, SYWSC_TRIGGER);
	
		if (!ret)
			return DC_FAILURE;		
	}
	else if (WPSMode == WPS_MODE_PIN) /* PIN Mode */
	{
		static DC_WIFI_IFACE_PARAMS WiFiIfParamsFromWPS;

		/* PIN Mode */
		ret = Set_WscMode_Proc(pAd, SYWSC_PIN_MODE);

		if(!ret)
			return DC_FAILURE;

		/* Generate PIN and pass it back to user
		   Go with WPS only after the users says "pin continue"
		 */
		pWscControl = &pAd->StaCfg.WscControl;

		ret = Set_WscGenPinCode_Proc(pAd, NULL);

		sprintf(WiFiIfParamsFromWPS.WpsPIN, "%08d", pWscControl->WscEnrolleePinCode);
		((WPSCallback *)(pWPSCallback))(DC_WIFI_WPS_STATUS_PIN_WAIT, &WiFiIfParamsFromWPS);	
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Unknown WPS Mode[%x]\n", WPSMode));
		return DC_FAILURE;
	}

	return DC_SUCCESS;
}


DC_RETURN_CODE wifi_WPS_pin_continue(IN UINT16 stime,
									 IN WPSCallback *pWPSCallback)
{
	DC_RETURN_CODE ret;
	PRTMP_ADAPTER pAd;
	PSY_CONTROL pSyControl;

	pAd = (PRTMP_ADAPTER)RT_WIFI_DevCtrl;

	SYWiFiInitWPS(pAd, pWPSCallback, stime);

	pSyControl = &pAd->StaCfg.SYControl;

	/* Use Net task to do latter job */
	RTMPQueueNetTask(&pSyControl->SYNetTaskQueue, &pSyControl->WPSPinContinueTask);

	return DC_SUCCESS;
}


DC_RETURN_CODE wifi_stop(void)
{
	DC_RETURN_CODE ret;

	return ret;
}


INT32 wifi_getFirmwareVersion(char *FWVersion, UINT32 FWVermaxLength)
{
	INT32 FWVerStrLen;

	FWVerStrLen = sizeof(STA_DRIVER_VERSION);
	
	if (FWVerStrLen <= FWVermaxLength)
	{
		memcpy(FWVersion, STA_DRIVER_VERSION, FWVerStrLen);
		return FWVerStrLen;
	}
	else
		return 0;
}
