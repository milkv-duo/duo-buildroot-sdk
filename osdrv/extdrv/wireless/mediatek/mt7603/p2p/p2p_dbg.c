#ifdef P2P_SUPPORT


#include "rt_config.h"

extern UCHAR	WILDP2PSSID[];
extern UCHAR	WILDP2PSSIDLEN;

#ifdef WFD_SUPPORT
UCHAR	default_wfd_device_info[SUBID_WFD_DEVICE_INFO_LEN] = {0x0};
UCHAR	default_wfd_audio_format[SUBID_WFD_AUDIO_FORMATS_LEN] = {0x0};
UCHAR	default_wfd_video_format[SUBID_WFD_VIDEO_FORMATS_LEN] = {0x0};
UCHAR	default_wfd_3d_video_format[SUBID_WFD_3D_VIDEO_FORMATS_LEN] = {0x0};
UCHAR	default_wfd_content_proctection[SUBID_WFD_CONTENT_PROTECTION_LEN] = {0x0};
UCHAR	default_wfd_couple_sink_info[SUBID_WFD_COUPLED_SINK_INFO_LEN] = {0x0};
UCHAR	default_wfd_extended_capability[SUBID_WFD_EXTENDED_CAP_LEN] = {0x0};
UCHAR	default_wfd_local_ip_address[SUBID_WFD_LOCAL_IP_ADDR_LEN] = {0x0};
#endif /* WFD_SUPPORT */

extern INT Set_AP_WscPinCode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)

INT Set_P2P_Print_Cfg(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if (P2P_INF_ON(pAd))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("P2P Device Config :\n"));
		DBGPRINT(RT_DEBUG_ERROR, ("=====================================================================\n"));
		DBGPRINT(RT_DEBUG_ERROR,("Device Name[%ld] = %s.\n", pP2PCtrl->DeviceNameLen, pP2PCtrl->DeviceName));
		DBGPRINT(RT_DEBUG_ERROR,("Device Addr = %02x:%02x:%02x:%02x:%02x:%02x.\n", PRINT_MAC(pP2PCtrl->CurrentAddress)));
		DBGPRINT(RT_DEBUG_ERROR, ("OpChannel = %d.        Listen Channel = %d. \n", pP2PCtrl->GroupChannel, pP2PCtrl->ListenChannel));
		DBGPRINT(RT_DEBUG_ERROR, ("My Go Intent = %d.\n", pP2PCtrl->GoIntentIdx));
		/*DBGPRINT(RT_DEBUG_ERROR, ("WscMode = %s.        ConfigMethod = %s.\n", (pP2PCtrl->WscMode == 1) ? "PIN" : "PBC", decodeConfigMethod(pP2PCtrl->ConfigMethod))); */
		if (pP2PCtrl->WscMode == 1)
			DBGPRINT(RT_DEBUG_ERROR, ("WscMode = PIN.\n"));
		else if (pP2PCtrl->WscMode == 2)
			DBGPRINT(RT_DEBUG_ERROR, ("WscMode = PBC.\n"));
		else
			DBGPRINT(RT_DEBUG_ERROR, ("WscMode = ***Unknown***.\n"));

		DBGPRINT(RT_DEBUG_ERROR, ("WscConfigMethod = %s.\n", decodeConfigMethod(pP2PCtrl->ConfigMethod)));
		DBGPRINT(RT_DEBUG_ERROR, ("WscDpid = %s.\n", decodeDpid(pP2PCtrl->Dpid)));
		if (pP2PCtrl->ConfigMethod == WSC_CONFMET_DISPLAY)
			DBGPRINT(RT_DEBUG_ERROR, ("My Self PIN Code = %08u\n", pAd->ApCfg.ApCliTab[0].WscControl.WscEnrolleePinCode));
		else if (pP2PCtrl->ConfigMethod == WSC_CONFMET_KEYPAD)
		{
			UINT PinCode = simple_strtol(pP2PCtrl->PinCode, 0, 10);
			DBGPRINT(RT_DEBUG_ERROR, ("Peer PIN Code = %08u\n", PinCode));
		}

		DBGPRINT(RT_DEBUG_ERROR, ("SSID[%d] = %s.\n", pP2PCtrl->SSIDLen, pP2PCtrl->SSID));
		DBGPRINT(RT_DEBUG_ERROR, ("NoA_Count = %d.        NoA_Duration = %ld.        NoA_Interval = %ld.        StartTime = %ld.\n", 
									pP2PCtrl->GONoASchedule.Count, pP2PCtrl->GONoASchedule.Duration, pP2PCtrl->GONoASchedule.Interval, pP2PCtrl->GONoASchedule.StartTime));
		DBGPRINT(RT_DEBUG_ERROR, ("ExtListenPeriod = %d.        ExtListenInterval = %d.\n", pP2PCtrl->ExtListenPeriod, pP2PCtrl->ExtListenInterval));
		DBGPRINT(RT_DEBUG_ERROR, ("Intra-Bss = %d.        \n", pP2PCtrl->bIntraBss));
		DBGPRINT(RT_DEBUG_ERROR, ("ConenctMAC = %02x:%02x:%02x:%02x:%02x:%02x.\n", PRINT_MAC(pP2PCtrl->ConnectingMAC)));
		DBGPRINT(RT_DEBUG_ERROR, ("p2pControl = %08x.    Persistent = %s. Invite = %s.    ClientDiscovery = %s.    IntraBss = %s.    ExtListen = %s.\n", pP2PCtrl->P2pControl.word, (IS_PERSISTENT_ON(pAd))? "ON" : "OFF", (IS_INVITE_ON(pAd))? "ON" : "OFF",
					(IS_CLIENT_DISCOVERY_ON(pAd))? "ON" : "OFF", (IS_INTRA_BSS_ON(pAd))? "ON" : "OFF", (IS_EXT_LISTEN_ON(pAd))? "ON" : "OFF"));
		DBGPRINT(RT_DEBUG_ERROR, ("                         Opps = %s.    SwNoATimer = %s.    ServiceDiscovery = %d.\n", (IS_OPPS_ON(pAd))? "ON" : "OFF", (IS_SW_NOA_TIMER(pAd))? "ON" : "OFF", pP2PCtrl->P2pControl.field.ServiceDiscovery));
#ifdef WFD_SUPPORT
		if (pAd->StaCfg.WfdCfg.bWfdEnable)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("WFD_DeviceType = %d.  SourceCoupled = %d.  SinkCoupled = %d\n", 
					pAd->StaCfg.WfdCfg.DeviceType, pAd->StaCfg.WfdCfg.SourceCoupled, pAd->StaCfg.WfdCfg.SinkCoupled));
			DBGPRINT(RT_DEBUG_ERROR, ("WFD_RTSP_PORT = %d.  WFD_MAX_THROUGHPUT = %d.\n", 
					pAd->StaCfg.WfdCfg.RtspPort, pAd->StaCfg.WfdCfg.MaxThroughput));
			DBGPRINT(RT_DEBUG_ERROR, ("WFD_SessionAvail = %d.  WSD = %d.  PC = %d.  CP = %d.  TimeSync = %d.\n", 
					pAd->StaCfg.WfdCfg.SessionAvail, pAd->StaCfg.WfdCfg.WSD, pAd->StaCfg.WfdCfg.PC, pAd->StaCfg.WfdCfg.CP, pAd->StaCfg.WfdCfg.TimeSync));
		}
#endif /* WFD_SUPPORT */
	}

	return TRUE;
}

INT Set_P2P_Enable(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UINT32 enable = 0;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	enable = (UCHAR) simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2P Enable = %d.\n", __FUNCTION__, enable));
	return TRUE;
}

INT Set_P2P_Listen_Channel(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UINT32 channel;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	channel = (UCHAR) simple_strtol(arg, 0, 10);
	/* check if this channel is valid */
	if (ChannelSanity(pAd, channel) == TRUE)
	{
		pAd->P2pCfg.ListenChannel = channel;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Listen Channel out of range, using default.\n"));
		pAd->P2pCfg.ListenChannel = 1;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s:: Listen Channel = %d.\n", __FUNCTION__, pAd->P2pCfg.ListenChannel));
	return TRUE;
}

INT Set_P2P_Operation_Channel(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UINT32 channel;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	channel = (UCHAR) simple_strtol(arg, 0, 10);
	/* check if this channel is valid */
	if (ChannelSanity(pAd, channel) == TRUE)
	{
		pAd->P2pCfg.GroupChannel = channel;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Opertation Channel out of range, using default.\n"));
		pAd->P2pCfg.GroupChannel = 1;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s:: Op Channel = %d.\n", __FUNCTION__, pAd->P2pCfg.GroupChannel));
	return TRUE;
}


INT Set_P2P_GO_Intent(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UINT32	intent;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	intent = simple_strtol(arg, 0, 10);
	if (intent <= 15)
		pAd->P2pCfg.GoIntentIdx = intent;		
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("GO Intent out of range 0 ~ 15, using default.\n"));
		pAd->P2pCfg.GoIntentIdx = 0;
	}

	DBGPRINT(RT_DEBUG_ERROR, ("%s:: GO Intent = %d.\n", __FUNCTION__, pAd->P2pCfg.GoIntentIdx));
	return TRUE;
}


INT Set_P2P_Device_Name(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if (strlen(arg) <= 32)
	{
		pAd->P2pCfg.DeviceNameLen = (UCHAR) strlen(arg);
		NdisZeroMemory(pAd->P2pCfg.DeviceName, 32);
		NdisMoveMemory(pAd->P2pCfg.DeviceName, arg, pAd->P2pCfg.DeviceNameLen);
		DBGPRINT(RT_DEBUG_ERROR, ("Set P2P Device Name - %s", pAd->P2pCfg.DeviceName));
	}

	DBGPRINT(RT_DEBUG_ERROR, ("%s:: Device Name = %s.\n", __FUNCTION__, pAd->P2pCfg.DeviceName));
	return TRUE;
}

INT Set_P2P_WSC_Mode(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	int wscMode;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	wscMode = simple_strtol(arg, 0, 10);
	if (wscMode <= 2 && wscMode >= 1)
		pAd->P2pCfg.WscMode= wscMode;
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("WscMode is invalid, using default and accept wsc method from peer's provision request action frame.\n"));
		pAd->P2pCfg.WscMode = WSC_PIN_MODE; /* PIN */
		pAd->P2pCfg.Dpid = DEV_PASS_ID_NOSPEC;
		pAd->P2pCfg.ConfigMethod = (WSC_CONFMET_PBC | WSC_CONFMET_KEYPAD | WSC_CONFMET_DISPLAY);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s:: WscMode = %s.\n", __FUNCTION__, (wscMode == 1) ? "PIN" : "PBC"));
	return TRUE;
}

INT Set_P2P_WSC_ConfMethod(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	int method;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	method = simple_strtol(arg, 0, 10);

	if (pAd->P2pCfg.WscMode == WSC_PIN_MODE)
	{
		if (method == 1)
		{
			/* Display PIN */
			pAd->P2pCfg.Dpid = DEV_PASS_ID_REG;
			pAd->P2pCfg.ConfigMethod =  WSC_CONFMET_DISPLAY;
			DBGPRINT(RT_DEBUG_TRACE, ("    *************************************************\n"));
			DBGPRINT(RT_DEBUG_TRACE, ("    *                                               *\n"));
			DBGPRINT(RT_DEBUG_TRACE, ("    *       PIN Code = %08u                     *\n", pAd->ApCfg.ApCliTab[0].WscControl.WscEnrolleePinCode));
			DBGPRINT(RT_DEBUG_TRACE, ("    *                                               *\n"));
			DBGPRINT(RT_DEBUG_TRACE, ("    *************************************************\n"));

		}
		else if (method == 2)
		{
			/* Enter PIN */
			pAd->P2pCfg.Dpid = DEV_PASS_ID_USER;
			pAd->P2pCfg.ConfigMethod =  WSC_CONFMET_KEYPAD;
		}
	}
	else if (pAd->P2pCfg.WscMode == WSC_PBC_MODE)
	{
		if (method == 3)
		{
			pAd->P2pCfg.Dpid = DEV_PASS_ID_PBC;
			pAd->P2pCfg.ConfigMethod = WSC_CONFMET_PBC;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s:: Config Method = %s.\n", __FUNCTION__, decodeConfigMethod(pAd->P2pCfg.ConfigMethod)));
	return TRUE;
}

INT Set_P2P_NoA_Count(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	pAd->P2pCfg.GONoASchedule.Count = simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s:: NoA Count = %d\n", __FUNCTION__, pAd->P2pCfg.GONoASchedule.Count));
	return TRUE;
}

INT Set_P2P_NoA_Duration(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	pAd->P2pCfg.GONoASchedule.Duration = simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s:: NoA Duration = %ld\n", __FUNCTION__, pAd->P2pCfg.GONoASchedule.Duration));
	return TRUE;
}

INT Set_P2P_NoA_Interval(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	pAd->P2pCfg.GONoASchedule.Interval = simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s:: NoA Interval = %ld\n", __FUNCTION__, pAd->P2pCfg.GONoASchedule.Interval));
	return TRUE;
}

INT Set_P2P_Extend_Listen(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR ExtListen;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	ExtListen = simple_strtol(arg, 0, 10);
	if (ExtListen)
		pAd->P2pCfg.bExtListen = TRUE;
	else
		pAd->P2pCfg.bExtListen = FALSE;
	DBGPRINT(RT_DEBUG_TRACE, ("%s:: Exted Listen = %d\n", __FUNCTION__, pAd->P2pCfg.bExtListen));
	return TRUE;
}

INT Set_P2P_Extend_Listen_Periodic(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	pAd->P2pCfg.ExtListenPeriod = simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s:: Extend Listen Interval = %d\n", __FUNCTION__, pAd->P2pCfg.ExtListenPeriod));
	return TRUE;
}

INT Set_P2P_Extend_Listen_Interval(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	pAd->P2pCfg.ExtListenInterval = simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s:: Extend Listen Interval = %d\n", __FUNCTION__, pAd->P2pCfg.ExtListenInterval));
	return TRUE;
}

INT Set_P2P_Intra_Bss(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR IntraBss;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	IntraBss = simple_strtol(arg, 0, 10);
	if (IntraBss)
		pAd->P2pCfg.bIntraBss = TRUE;
	else
		pAd->P2pCfg.bIntraBss = FALSE;
	DBGPRINT(RT_DEBUG_TRACE, ("%s:: IntraBss = %d\n", __FUNCTION__, pAd->P2pCfg.bIntraBss));
	return TRUE;
}

INT Set_P2P_Scan(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	int ScanType;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	ScanType = simple_strtol(arg, 0, 10);
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;


	switch(ScanType)
	{
		case P2P_STOP_SCAN:
			/* Stop scan and stop to response peer P2P scanning */
#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
			pAd->StaCfg.bAutoReconnect = FALSE;
#else
			pAd->StaCfg.bAutoReconnect = TRUE;
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
			pP2PCtrl->bSentProbeRSP = FALSE;
			P2pStopScan(pAd);
			break;
		case P2P_SCANNING:
			/* Do P2P scanning */
		pAd->StaCfg.bAutoReconnect = FALSE;
		pP2PCtrl->bSentProbeRSP = TRUE;
		P2pGroupTabInit(pAd);
		P2pScan(pAd);
			break;
		case P2P_STOP_SCAN_AND_LISTEN:
			/* Stop scan and lock at Listen Channel to response peer P2P scanning */
			pAd->P2pCfg.bSentProbeRSP = TRUE;
		P2pStopScan(pAd);
			break;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("Incorrect scan type:%d\n", ScanType));
	}
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_ERROR, ("----> %s\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
	return TRUE;
}

INT Set_P2P_Print_GroupTable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	int i, j;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	RTMP_SEM_LOCK(&pAd->P2pTableSemLock);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2pTable ClientNum = %d\n", __FUNCTION__, pAd->P2pTable.ClientNumber));
	for (i=0; i < pAd->P2pTable.ClientNumber; i++)
	{
		PRT_P2P_CLIENT_ENTRY pP2pEntry = &pAd->P2pTable.Client[i];
		DBGPRINT(RT_DEBUG_ERROR, ("Table.Client[%d]: DeviceName[%d][%s]\n", i, pP2pEntry->DeviceNameLen, pP2pEntry->DeviceName));
		DBGPRINT(RT_DEBUG_ERROR, ("                         Addr[%02x:%02x:%02x:%02x:%02x:%02x]\n", PRINT_MAC(pP2pEntry->addr)));
		DBGPRINT(RT_DEBUG_ERROR, ("                         BSSID[%02x:%02x:%02x:%02x:%02x:%02x]\n", PRINT_MAC(pP2pEntry->bssid)));
		DBGPRINT(RT_DEBUG_ERROR, ("                         InterfaceAddr[%02x:%02x:%02x:%02x:%02x:%02x]\n", PRINT_MAC(pP2pEntry->InterfaceAddr)));
		DBGPRINT(RT_DEBUG_ERROR, ("                         SSID["));
		for (j=0; j<pP2pEntry->SsidLen;j++)
			DBGPRINT(RT_DEBUG_ERROR, ("%c ", pP2pEntry->Ssid[j]));
		DBGPRINT(RT_DEBUG_ERROR, ("]\n"));
		DBGPRINT(RT_DEBUG_ERROR, ("                         OpChannel = %d.        ListenChannel = %d.\n", pP2pEntry->OpChannel, pP2pEntry->ListenChannel));
		DBGPRINT(RT_DEBUG_ERROR, ("                         P2pClientState = %s.        MyGOIndex = %d.\n", decodeP2PClientState(pP2pEntry->P2pClientState), pP2pEntry->MyGOIndex));
		DBGPRINT(RT_DEBUG_ERROR, ("                         Dpid = %s.        Rule = %s.\n", decodeDpid(pP2pEntry->Dpid), decodeMyRule(pP2pEntry->Rule)));

		if (pP2pEntry->WscMode == 1)
			DBGPRINT(RT_DEBUG_ERROR, ("                         WscMode = PIN.        PIN = %02x %02x %02x %02x %02x %02x %02x %02x.\n", 
					pP2pEntry->PIN[0], pP2pEntry->PIN[1], pP2pEntry->PIN[2], pP2pEntry->PIN[3], 
					pP2pEntry->PIN[4], pP2pEntry->PIN[5], pP2pEntry->PIN[6], pP2pEntry->PIN[7]));
		else if (pAd->P2pTable.Client[i].WscMode == 2)
			DBGPRINT(RT_DEBUG_ERROR, ("                         WscMode = PBC.\n"));
		else
			DBGPRINT(RT_DEBUG_ERROR, ("                         WscMode = ***Unknown***.\n"));
		DBGPRINT(RT_DEBUG_ERROR, ("                         CfgMethod = %s.        GoIntent = %d.\n", decodeConfigMethod(pAd->P2pTable.Client[i].ConfigMethod), pAd->P2pTable.Client[i].GoIntent));
		decodeDeviceCap(pP2pEntry->DevCapability);
		decodeGroupCap(pP2pEntry->GroupCapability);
		DBGPRINT(RT_DEBUG_ERROR, ("                         Rssi = %d.\n", pP2pEntry->Rssi));
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("                         PrimDevType[%02x %02x %02x %02x %02x %02x %02x %02x] SecDevType[%02x %02x %02x %02x %02x %02x %02x %02x]\n", pAd->P2pTable.Client[i].PrimaryDevType[0], pAd->P2pTable.Client[i].PrimaryDevType[1], pAd->P2pTable.Client[i].PrimaryDevType[2], 
					pAd->P2pTable.Client[i].PrimaryDevType[3], pAd->P2pTable.Client[i].PrimaryDevType[4], pAd->P2pTable.Client[i].PrimaryDevType[5], pAd->P2pTable.Client[i].PrimaryDevType[6], pAd->P2pTable.Client[i].PrimaryDevType[7],
					pAd->P2pTable.Client[i].SecondaryDevType[0], pAd->P2pTable.Client[i].SecondaryDevType[1], pAd->P2pTable.Client[i].SecondaryDevType[2], pAd->P2pTable.Client[i].SecondaryDevType[3], pAd->P2pTable.Client[i].SecondaryDevType[4], pAd->P2pTable.Client[i].SecondaryDevType[5], pAd->P2pTable.Client[i].SecondaryDevType[6], pAd->P2pTable.Client[i].SecondaryDevType[7]));
		DBGPRINT(RT_DEBUG_INFO, ("                         P2pIP = %08x.        P2pFlag = %d.\n", (UINT32)pAd->P2pTable.Client[i].Peerip, pAd->P2pTable.Client[i].P2pFlag));
		DBGPRINT(RT_DEBUG_INFO, ("                         GeneralToken = %d.        NoAToken = %d.\n", pAd->P2pTable.Client[i].GeneralToken, pAd->P2pTable.Client[i].NoAToken));
		DBGPRINT(RT_DEBUG_INFO, ("                         RegClass = %d.        ConfigTimeOut = %d.\n", pAd->P2pTable.Client[i].RegClass, pAd->P2pTable.Client[i].ConfigTimeOut));
		DBGPRINT(RT_DEBUG_INFO, ("                         ExtListenPeriod = %d.        ExtListenInterval = %d.\n", pAd->P2pTable.Client[i].ExtListenPeriod, pAd->P2pTable.Client[i].ExtListenInterval));

#endif /* RELEASE_EXCLUDE */
#ifdef WFD_SUPPORT
		DBGPRINT(RT_DEBUG_ERROR, ("                         WFD = %d.\n", pP2pEntry->WfdEntryInfo.bWfdClient));
		if (pP2pEntry->WfdEntryInfo.bWfdClient)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("                         DeviceType = %d. RTSP_Port = %d. MaxThroughput=%d.\n", 
					pP2pEntry->WfdEntryInfo.wfd_devive_type, pP2pEntry->WfdEntryInfo.rtsp_port, pP2pEntry->WfdEntryInfo.max_throughput));
			DBGPRINT(RT_DEBUG_ERROR, ("                         SessionAvail = %d. PC = %d\n", 
					pP2pEntry->WfdEntryInfo.session_avail, pP2pEntry->WfdEntryInfo.wfd_PC));
			hex_dump("device_info::", pP2pEntry->WfdEntryInfo.wfd_serv_disc_query_info.wfd_device_info_ie, 6);
			hex_dump("associate bssid::", pP2pEntry->WfdEntryInfo.wfd_serv_disc_query_info.wfd_associate_bssid_ie, 6);
			hex_dump("audio format::", pP2pEntry->WfdEntryInfo.wfd_serv_disc_query_info.wfd_audio_format_ie, 15);
			hex_dump("video format::", pP2pEntry->WfdEntryInfo.wfd_serv_disc_query_info.wfd_video_format_ie, 21);
			hex_dump("3d video format::", pP2pEntry->WfdEntryInfo.wfd_serv_disc_query_info.wfd_3d_video_format_ie, 13);
			hex_dump("content protection::", pP2pEntry->WfdEntryInfo.wfd_serv_disc_query_info.wfd_content_proctection, 1);
			hex_dump("couple sink::", pP2pEntry->WfdEntryInfo.wfd_serv_disc_query_info.wfd_couple_sink_info_ie, 1);
			hex_dump("extentded capability::", pP2pEntry->WfdEntryInfo.wfd_serv_disc_query_info.wfd_extent_capability_ie, 2);
			hex_dump("alternate mac address::", pP2pEntry->WfdEntryInfo.wfd_serv_disc_query_info.wfd_alternate_mac_addr_ie, SUBID_WFD_ALTERNATE_MAC_ADDR_LEN);
		}
#endif /* WFD_SUPPORT */
		DBGPRINT(RT_DEBUG_ERROR, ("\n"));
    }

	RTMP_SEM_UNLOCK(&pAd->P2pTableSemLock);
	return TRUE;
}

INT Set_P2P_Print_PersistentTable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	int i;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2pTable ClientNum = %d\n", __FUNCTION__, pAd->P2pTable.PerstNumber));
	for (i=0; i < pAd->P2pTable.PerstNumber; i++)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Table.PerstEntry[%d]: Rule = %s\n", i, decodeMyRule(pAd->P2pTable.PerstEntry[i].MyRule)));
		DBGPRINT(RT_DEBUG_ERROR, ("                         DevAddr = %02x:%02x:%02x:%02x:%02x:%02x.\n", PRINT_MAC(pAd->P2pTable.PerstEntry[i].Addr)));
		DBGPRINT(RT_DEBUG_ERROR, ("                         SSID[%d] = %s.\n", pAd->P2pTable.PerstEntry[i].Profile.SSID.SsidLength, pAd->P2pTable.PerstEntry[i].Profile.SSID.Ssid));
		DBGPRINT(RT_DEBUG_ERROR, ("                         MACAddr = %02x:%02x:%02x:%02x:%02x:%02x.\n", PRINT_MAC(pAd->P2pTable.PerstEntry[i].Profile.MacAddr)));
#ifdef WIFI_P2P_DEVICENAME_IN_PERSISTAB		
		DBGPRINT(RT_DEBUG_ERROR, ("                         Device Name = %s.\n", pAd->P2pTable.PerstEntry[i].DeviceName));
#endif
		DBGPRINT(RT_DEBUG_ERROR, ("                         Key[%d] = %02x %02x %02x %02x  %02x %02x %02x %02x.\n", pAd->P2pTable.PerstEntry[i].Profile.KeyIndex, pAd->P2pTable.PerstEntry[i].Profile.Key[0], pAd->P2pTable.PerstEntry[i].Profile.Key[1],
								pAd->P2pTable.PerstEntry[i].Profile.Key[2], pAd->P2pTable.PerstEntry[i].Profile.Key[3], pAd->P2pTable.PerstEntry[i].Profile.Key[4], pAd->P2pTable.PerstEntry[i].Profile.Key[5],
								pAd->P2pTable.PerstEntry[i].Profile.Key[6], pAd->P2pTable.PerstEntry[i].Profile.Key[7]));
		DBGPRINT(RT_DEBUG_ERROR, ("                                  %02x %02x %02x %02x  %02x %02x %02x %02x.\n", pAd->P2pTable.PerstEntry[i].Profile.Key[8], pAd->P2pTable.PerstEntry[i].Profile.Key[9],
								pAd->P2pTable.PerstEntry[i].Profile.Key[10], pAd->P2pTable.PerstEntry[i].Profile.Key[11], pAd->P2pTable.PerstEntry[i].Profile.Key[12], pAd->P2pTable.PerstEntry[i].Profile.Key[13],
								pAd->P2pTable.PerstEntry[i].Profile.Key[14], pAd->P2pTable.PerstEntry[i].Profile.Key[15]));
	}
	return TRUE;
}

INT Set_P2P_Provision_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR p2pindex;
	PUCHAR	pAddr;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	p2pindex = simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_ERROR, ("%s: TabIdx[%d]\n", __FUNCTION__, p2pindex));
	if (p2pindex < pAd->P2pTable.ClientNumber)
	{
/*		P2PPrintP2PEntry(pAd, P2pTabIdx); */
		pAddr = &pAd->P2pTable.Client[p2pindex].addr[0];
/*		pAd->P2pTable.Client[P2pTabIdx].StateCount = 10; */
/*		pAd->P2pTable.Client[P2pTabIdx].bValid = TRUE; */
/*		P2pProvision(pAd, pAddr); */
		P2pConnectPrepare(pAd, pAddr, P2PSTATE_PROVISION_COMMAND);
    }
    else
        DBGPRINT(RT_DEBUG_ERROR, ("Table Idx out of range!\n"));

	return TRUE;

}

INT Set_P2P_Invite_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR		p2pindex;
	PUCHAR	pAddr;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	p2pindex = simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: TabIdx[%d]\n", __FUNCTION__, p2pindex));

	if (p2pindex < pAd->P2pTable.ClientNumber)
	{
		pAddr = &pAd->P2pTable.Client[p2pindex].addr[0];
		P2pConnectPrepare(pAd, pAddr, P2PSTATE_INVITE_COMMAND);
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("Table Idx out of range!\n"));

	return TRUE;
}

INT Set_P2P_Device_Discoverability_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR		p2pindex;
	UCHAR		MyGOIdx;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	p2pindex = simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: TabIdx[%d]\n", __FUNCTION__, p2pindex));
	P2PPrintP2PEntry(pAd, p2pindex);
	if (p2pindex < MAX_P2P_GROUP_SIZE)
	{
		MyGOIdx = pAd->P2pTable.Client[p2pindex].MyGOIndex;
		if (MyGOIdx != P2P_NOT_FOUND)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Start P2P Device Discoverability = %02x:%02x:%02x:%02x:%02x:%02x.	\n",  PRINT_MAC(pAd->P2pTable.Client[p2pindex].addr)));
			pAd->P2pTable.Client[p2pindex].GeneralToken++;
				pAd->P2pTable.Client[MyGOIdx].P2pClientState = P2PSTATE_GO_DISCO_COMMAND;
				pAd->P2pTable.Client[p2pindex].P2pClientState = P2PSTATE_CLIENT_DISCO_COMMAND;
			P2pSetListenIntBias(pAd, 12);
		}
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("Table Idx out of range!\n"));

	return TRUE;
}

INT Set_P2P_Connect_GoIndex_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR		p2pindex;
	PUCHAR	pAddr;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	p2pindex = simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: TabIdx[%d]\n", __FUNCTION__, p2pindex));


	if (p2pindex < pAd->P2pTable.ClientNumber)
	{
		/*
		P2PPrintP2PEntry(pAd, P2pTabIdx);
		pAd->P2pCfg.ConnectingIndex = 0;
		if (pAd->P2pTable.Client[P2pTabIdx].P2pClientState == P2PSTATE_DISCOVERY)
			pAd->P2pTable.Client[P2pTabIdx].P2pClientState = P2PSTATE_CONNECT_COMMAND;
		COPY_MAC_ADDR(pAd->P2pCfg.ConnectingMAC, pAd->P2pTable.Client[P2pTabIdx].addr);
		pAd->P2pTable.Client[P2pTabIdx].StateCount = 10;
		pAd->P2pTable.Client[P2pTabIdx].bValid = TRUE;
		P2pConnect(pAd);
		 */
		pAddr = &pAd->P2pTable.Client[p2pindex].addr[0];
		P2pConnectPrepare(pAd, pAddr, P2PSTATE_CONNECT_COMMAND);
    }
    else
        DBGPRINT(RT_DEBUG_ERROR, ("Table Idx out of range!\n"));

	return TRUE;
}


INT Set_P2P_Connect_Dev_Addr_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR		p2pindex = P2P_NOT_FOUND;
	PUCHAR	pAddr;
	UCHAR ConnAddr[6];
	UINT32	i;
	extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];
	extern UCHAR BROADCAST_ADDR[MAC_ADDR_LEN];

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	DBGPRINT(RT_DEBUG_ERROR, ("%s::  Connect to DevAddr[%s]\n", __FUNCTION__, arg));


	/*
		If the input is the zero mac address, it means use our default(from EEPROM) MAC address as out-going 
		   MAC address.
		If the input is the broadcast MAC address, it means use the source MAC of first packet forwarded by
		   our device as the out-going MAC address.
		If the input is any other specific valid MAC address, use it as the out-going MAC address.
	*/

	NdisMoveMemory(&ConnAddr[0], &ZERO_MAC_ADDR[0], MAC_ADDR_LEN);
	if (rtstrmactohex(arg, (RTMP_STRING *)&ConnAddr[0]) == FALSE)
		return FALSE;

	DBGPRINT(RT_DEBUG_ERROR, ("%s:: DevMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	__FUNCTION__, PRINT_MAC(ConnAddr)));
	
	if (MAC_ADDR_EQUAL(ConnAddr, ZERO_MAC_ADDR))
	{
		P2pLinkDown(pAd, P2P_CONNECT_FAIL);
		return TRUE;
	}

	for (i=0; i < pAd->P2pTable.ClientNumber; i++)
	{
		if (MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].addr, ConnAddr) ||
			MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].bssid, ConnAddr) ||
			MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].InterfaceAddr, ConnAddr))
		{
			p2pindex = i;
			break;
		}
	}

	if ((p2pindex < pAd->P2pTable.ClientNumber) && (p2pindex != P2P_NOT_FOUND))
	{
#ifdef WFD_SUPPORT
		if ((pAd->StaCfg.WfdCfg.bWfdEnable) &&
			(pAd->P2pTable.Client[p2pindex].WfdEntryInfo.bWfdClient) &&
			(pAd->P2pTable.Client[p2pindex].WfdEntryInfo.session_avail == WFD_SESSION_NOT_AVAILABLE))
		{
			pAd->StaCfg.WfdCfg.PeerSessionAvail = WFD_SESSION_NOT_AVAILABLE;
			DBGPRINT(RT_DEBUG_ERROR, ("Session Avail is turn off!\n"));
			return FALSE;
		}
#endif /* WFD_SUPPORT */

		pAddr = &pAd->P2pTable.Client[p2pindex].addr[0];
		P2pConnectPrepare(pAd, pAddr, P2PSTATE_CONNECT_COMMAND);
    }
    else
        DBGPRINT(RT_DEBUG_ERROR, ("Table Idx out of range!\n"));

	return TRUE;
}

INT Set_P2P_Send_Invite_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR ConnAddr[6];
	extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];


	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	DBGPRINT(RT_DEBUG_ERROR, ("%s::  Send invite to DevAddr[%s]\n", __FUNCTION__, arg));

	/*
		If the input is the zero mac address, it means use our default(from EEPROM) MAC address as out-going 
		   MAC address.
		If the input is the broadcast MAC address, it means use the source MAC of first packet forwarded by
		   our device as the out-going MAC address.
		If the input is any other specific valid MAC address, use it as the out-going MAC address.
	*/

	NdisMoveMemory(&ConnAddr[0], &ZERO_MAC_ADDR[0], MAC_ADDR_LEN);
	if (rtstrmactohex(arg, (RTMP_STRING *)&ConnAddr[0]) == FALSE)
		return FALSE;

	DBGPRINT(RT_DEBUG_ERROR, ("%s:: DevMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	__FUNCTION__, PRINT_MAC(ConnAddr)));

	P2pConnectPrepare(pAd, ConnAddr, P2PSTATE_INVITE_COMMAND);

	return TRUE;
}

INT Set_P2P_Provision_Dev_Addr_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR		p2pindex = P2P_NOT_FOUND;
	PUCHAR	pAddr;
	UCHAR ConnAddr[6];
	UINT32	i;
	UINT16 retry_cnt = 0;
	extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];
	extern UCHAR BROADCAST_ADDR[MAC_ADDR_LEN];

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	DBGPRINT(RT_DEBUG_ERROR, ("%s::  Connect to DevAddr[%s]\n", __FUNCTION__, arg));


	/*
		If the input is the zero mac address, it means use our default(from EEPROM) MAC address as out-going 
		   MAC address.
		If the input is the broadcast MAC address, it means use the source MAC of first packet forwarded by
		   our device as the out-going MAC address.
		If the input is any other specific valid MAC address, use it as the out-going MAC address.
	*/

	if( pAd->P2pTable.ClientNumber == 0)
	{
		DBGPRINT( RT_DEBUG_ERROR, ("P2P Table is Empty! Scan First!\n"));
		P2pScan(pAd);
		OS_WAIT(2000);
	}

	NdisMoveMemory(&ConnAddr[0], &ZERO_MAC_ADDR[0], MAC_ADDR_LEN);
	if (rtstrmactohex(arg, (RTMP_STRING *) &ConnAddr[0]) == FALSE)
		return FALSE;

	DBGPRINT(RT_DEBUG_ERROR, ("%s:: DevMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	__FUNCTION__, PRINT_MAC(ConnAddr)));
	
	if (MAC_ADDR_EQUAL(ConnAddr, ZERO_MAC_ADDR))
	{
		P2pLinkDown(pAd, P2P_CONNECT_FAIL);
		return TRUE;
	}

retry:
	
	for (i=0; i < pAd->P2pTable.ClientNumber; i++)
	{
		if (MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].addr, ConnAddr) ||
			MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].bssid, ConnAddr) ||
			MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].InterfaceAddr, ConnAddr))
		{
			p2pindex = i;
			break;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE,("pAd->P2pTable.ClientNumber = %d, p2pindex = %d\n", pAd->P2pTable.ClientNumber, p2pindex));
	if ((p2pindex < pAd->P2pTable.ClientNumber) && (p2pindex != P2P_NOT_FOUND))
	{
		pAddr = &pAd->P2pTable.Client[p2pindex].addr[0];
#if 0
		if (P2P_GO_ON(pAd))
			P2pConnectPrepare(pAd, pAddr, P2PSTATE_INVITE_COMMAND);
		else
#endif
			P2pConnectPrepare(pAd, pAddr, P2PSTATE_PROVISION_COMMAND);
	}
	else
	{
		retry_cnt ++;
		if ( retry_cnt < 5 )
		{
			OS_WAIT(2000);
			goto retry;
		}
		DBGPRINT(RT_DEBUG_ERROR, ("Table Idx out of range!\n"));
#ifdef RT_P2P_SPECIFIC_WIRELESS_EVENT
		P2pSendWirelessEvent(pAd, RT_P2P_CONNECT_FAIL, NULL, NULL);
#endif /* RT_P2P_SPECIFIC_WIRELESS_EVENT */
	}

	return TRUE;
}

INT Set_P2P_State_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	DBGPRINT(RT_DEBUG_ERROR, ("P2P Current State\n"));	
	DBGPRINT(RT_DEBUG_ERROR, ("=====================================================================\n"));
	DBGPRINT(RT_DEBUG_ERROR, ("My Rule = %s\n", decodeMyRule(pP2PCtrl->Rule)));
	DBGPRINT(RT_DEBUG_ERROR, ("p2p_OpStatus = %04x\n", pAd->flg_p2p_OpStatusFlags));
	DBGPRINT(RT_DEBUG_ERROR, ("CTRL Machine State = %s.\n", decodeCtrlState(pP2PCtrl->CtrlCurrentState)));
	DBGPRINT(RT_DEBUG_ERROR, ("DISC Machine State = %s.\n", decodeDiscoveryState(pP2PCtrl->DiscCurrentState)));
	DBGPRINT(RT_DEBUG_ERROR, ("GO_FORM Machine State = %s.\n", decodeGroupFormationState(pP2PCtrl->GoFormCurrentState)));
	DBGPRINT(RT_DEBUG_ERROR, ("AutoReconn = %d\n", pAd->StaCfg.bAutoReconnect));
	/*DBGPRINT(RT_DEBUG_ERROR, ("P2PDiscoProvState = %s\n", decodeP2PState(pP2PCtrl->P2PDiscoProvState))); */
	DBGPRINT(RT_DEBUG_ERROR, ("P2PConnectState = %s\n", decodeP2PState(pP2PCtrl->P2PConnectState)));

	return TRUE;
}

INT Set_P2P_Reset_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	/*UINT32 DiscPerd = 0;*/
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if (P2P_CLI_ON(pAd))
		P2P_CliStop(pAd);
	else if (P2P_GO_ON(pAd))
	{
		P2P_GoStop(pAd);
		if (INFRA_ON(pAd))
			AsicEnableBssSync(pAd, pAd->CommonCfg.BeaconPeriod);
	}
	P2PCfgInit(pAd);
	P2pGroupTabInit(pAd);
	pP2PCtrl->Rule = P2P_IS_DEVICE;
	pAd->flg_p2p_OpStatusFlags = P2P_DISABLE;
	pP2PCtrl->ConfigMethod = 0x188;
	pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl.WscConfStatus = WSC_SCSTATE_UNCONFIGURED;
	pP2PCtrl->GoFormCurrentState = P2P_GO_FORM_IDLE;
	pP2PCtrl->DiscCurrentState = P2P_DISC_IDLE;
	pP2PCtrl->CtrlCurrentState = P2P_CTRL_IDLE;
	NdisZeroMemory(&pP2PCtrl->P2pCounter, sizeof(P2P_COUNTER_STRUCT));	
	P2pSetListenIntBias(pAd, 3);
	RTMPZeroMemory(pAd->P2pCfg.SSID, MAX_LEN_OF_SSID);
	RTMPMoveMemory(pAd->P2pCfg.SSID, WILDP2PSSID, WILDP2PSSIDLEN);
	pP2PCtrl->SSIDLen = WILDP2PSSIDLEN;
	/* Set Dpid to "not specified". it means, GUI doesn't set for connection yet. */
	pP2PCtrl->Dpid = DEV_PASS_ID_NOSPEC;
	RTMPZeroMemory(pAd->P2pCfg.ConnectingMAC, MAC_ADDR_LEN);
	return TRUE;
}

INT Set_P2P_Default_Config_Method_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)

{
	UINT32 ConfigMethod = 0;
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	ConfigMethod = simple_strtol(arg, 0, 10);
	if ((ConfigMethod >= 1) && (ConfigMethod <= 3))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s:: Default Config Method is %d.\n", __FUNCTION__, ConfigMethod));
		DBGPRINT(RT_DEBUG_ERROR, ("    1: DISPLAY.    2: KEYPAD.    3. PBC\n"));
		pP2PCtrl->DefaultConfigMethod = ConfigMethod;
	}
	return TRUE;
}

INT Set_P2P_Link_Down_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)

{
	POS_COOKIE			pObj;


	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	P2pLinkDown(pAd, P2P_DISCONNECTED);
	return TRUE;
}

INT Set_P2P_Sigma_Enable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)

{
	UINT32 SigmaEnabled = 0;
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	SigmaEnabled = simple_strtol(arg, 0, 10);
	pP2PCtrl->bSigmaEnabled = ((SigmaEnabled == 0) ? FALSE : TRUE);
	pP2PCtrl->CheckPeerChannelPolicy = ((SigmaEnabled == 0) ? CHECK_PEER_CHANNEL_IF_NO_CONNECTION : CHECK_PEER_CHANNEL_DISABLE);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2P Sigma Enable = %d.\n", __FUNCTION__, pP2PCtrl->bSigmaEnabled));

	return TRUE;
}

INT Set_P2P_QoS_NULL_Legacy_Rate_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)

{
	UINT32 QoSNull = 0;
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	QoSNull = simple_strtol(arg, 0, 10);
	pP2PCtrl->bLowRateQoSNULL = ((QoSNull == 0) ? FALSE : TRUE);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2P QoS NULL using Legacy Rate Enable = %d.\n", __FUNCTION__, pP2PCtrl->bLowRateQoSNULL));

	return TRUE;
}

INT Set_P2P_CLIENT_PM_Enable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)

{
	UINT32 P2pClientPm = 0;
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	P2pClientPm = simple_strtol(arg, 0, 10);
	pP2PCtrl->bP2pCliPmEnable= ((P2pClientPm == 0) ? FALSE : TRUE);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2P CLIENT PM Enable = %d.\n", __FUNCTION__, pP2PCtrl->bP2pCliPmEnable));

	return TRUE;
}

INT Set_P2P_Enter_WSC_PIN_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)

{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;


	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if ((strlen(arg) != 4) && (strlen(arg) != 8))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s:: Reject this PIN (%s.)\n", __FUNCTION__, arg));
		return 0;
	}

	NdisZeroMemory(&pP2PCtrl->PinCode[0], sizeof(pP2PCtrl->PinCode));
	RTMPMoveMemory(&pP2PCtrl->PinCode[0], arg, strlen(arg));
	Set_AP_WscPinCode_Proc(pAd, arg);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2P Enter Peer PIN Code = %s.\n", __FUNCTION__, pP2PCtrl->PinCode));

	return TRUE;
}

INT Set_P2P_Persistent_Enable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)

{
	UINT32 PersistentEnabled = 0;
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	PersistentEnabled = simple_strtol(arg, 0, 10);
	pP2PCtrl->P2pControl.field.EnablePresistent = PersistentEnabled;
	if ( PersistentEnabled == TRUE )
	{ 
		pP2PCtrl->P2pCapability[1] |= GRPCAP_PERSISTENT;
		pP2PCtrl->P2pCapability[1] |= GRPCAP_PERSISTENT_RECONNECT;
		pP2PCtrl->ExtListenInterval = P2P_EXT_LISTEN_INTERVAL;
		pP2PCtrl->ExtListenPeriod = P2P_EXT_LISTEN_PERIOD;
	}
	else
	{
		pP2PCtrl->P2pCapability[1] &= 0xDD; /* Set GRPCAP_PERSISTENT and GRPCAP_PERSISTENT_RECONNECT bits to 0 */
		pP2PCtrl->ExtListenInterval = 0;
		pP2PCtrl->ExtListenPeriod = 0;
	}

	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2P Persistent Enable = %d.\n", __FUNCTION__, pP2PCtrl->P2pControl.field.EnablePresistent));

	return TRUE;
}

INT Set_P2P_Dev_Discoverability_Enable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)

{
	UINT32 DevDiscoverEnabled = 0;
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	DevDiscoverEnabled = simple_strtol(arg, 0, 10);
	pP2PCtrl->P2pControl.field.ClientDiscovery = DevDiscoverEnabled;
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2P Device Discoverability Enable = %d.\n", __FUNCTION__, pP2PCtrl->P2pControl.field.ClientDiscovery));

	return TRUE;
}

INT Set_P2P_ProvisionByAddr_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE pObj;
	UCHAR p2pindex = 0;
	UCHAR ConnAddr[6] = {0};

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if (rtstrmactohex(arg, (RTMP_STRING *) &ConnAddr[0]) == FALSE)
		return FALSE;

	DBGPRINT(RT_DEBUG_TRACE, ("%s:: DevMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	__FUNCTION__, PRINT_MAC(ConnAddr)));

	p2pindex = P2pGroupTabSearch(pAd, ConnAddr);

	if (p2pindex < pAd->P2pTable.ClientNumber)
    {
		P2pConnectPrepare(pAd, ConnAddr, P2PSTATE_PROVISION_COMMAND);
    }
    else
        DBGPRINT(RT_DEBUG_ERROR, ("Table Idx out of range!\n"));

	return TRUE;

}

INT Set_P2P_Service_Discovery_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 ServiceDiscoverEnabled = 0;
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;
#ifdef WFD_SUPPORT
	PRT_WFD_CONFIG	pWFDCtrl = &pAd->StaCfg.WfdCfg;
#endif /* WFD_SUPPORT */

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	ServiceDiscoverEnabled = simple_strtol(arg, 0, 10);
	pP2PCtrl->P2pControl.field.ServiceDiscovery = ServiceDiscoverEnabled;
#ifdef WFD_SUPPORT
	pWFDCtrl->WSD = ServiceDiscoverEnabled;
#endif /* WFD_SUPPORT */
	if (ServiceDiscoverEnabled == TRUE)
		pP2PCtrl->P2pCapability[0] |= DEVCAP_SD;
	else
		pP2PCtrl->P2pCapability[0] &= ~(DEVCAP_SD);

#ifdef WFD_SUPPORT
	/* Service Discovery Info. */
	RTMPMoveMemory(pWFDCtrl->wfd_serv_disc_query_info.wfd_device_info_ie, 
				&default_wfd_device_info, SUBID_WFD_DEVICE_INFO_LEN);
	RTMPMoveMemory(pWFDCtrl->wfd_serv_disc_query_info.wfd_audio_format_ie, 
				&default_wfd_audio_format, SUBID_WFD_AUDIO_FORMATS_LEN);
	RTMPMoveMemory(pWFDCtrl->wfd_serv_disc_query_info.wfd_video_format_ie, 
				&default_wfd_video_format, SUBID_WFD_VIDEO_FORMATS_LEN);
	RTMPMoveMemory(pWFDCtrl->wfd_serv_disc_query_info.wfd_3d_video_format_ie, 
				&default_wfd_3d_video_format, SUBID_WFD_3D_VIDEO_FORMATS_LEN);
	RTMPMoveMemory(pWFDCtrl->wfd_serv_disc_query_info.wfd_content_proctection, 
				&default_wfd_content_proctection, SUBID_WFD_CONTENT_PROTECTION_LEN);
	RTMPMoveMemory(pWFDCtrl->wfd_serv_disc_query_info.wfd_couple_sink_info_ie, 
				&default_wfd_couple_sink_info, SUBID_WFD_COUPLED_SINK_INFO_LEN);
	RTMPMoveMemory(pWFDCtrl->wfd_serv_disc_query_info.wfd_extent_capability_ie, 
				&default_wfd_extended_capability, SUBID_WFD_EXTENDED_CAP_LEN);
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: WFD Service Discoverability Enable = %d.\n", __FUNCTION__, pP2PCtrl->P2pControl.field.ServiceDiscovery));
#endif /* WFD_SUPPORT */

	return TRUE;
}

INT Set_P2P_Service_Discovery_Capable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 ServiceDiscoverCapable = 0;
	POS_COOKIE			pObj;
//	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;
#ifdef WFD_SUPPORT
	PRT_WFD_CONFIG	pWFDCtrl = &pAd->StaCfg.WfdCfg;
#endif /* WFD_SUPPORT */

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

#ifdef WFD_SUPPORT
	ServiceDiscoverCapable = simple_strtol(arg, 0, 10);
	pWFDCtrl->WfdSerDiscCapable = ServiceDiscoverCapable;
	DBGPRINT(RT_DEBUG_ERROR, ("%s::  WFD Service Discovery Capability [%08x]\n", __FUNCTION__, ServiceDiscoverCapable));
	DBGPRINT(RT_DEBUG_ERROR, ("        DeviceInfo = %d.  AssociateBssid = %d. AudioFormat = %d.  VideoFormat = %d.\n"
								"        3D_VideoFormat = %d.  ContentProtection = %d.  CoupleSinkInfo = %d.  ExtendedCapability = %d\n"
								"        LocalIP = %d. SessionInfo = %d. AlternateMacAddress = %d.\n", 
			(ServiceDiscoverCapable & 0x1), (ServiceDiscoverCapable & (0x1 << 1)), (ServiceDiscoverCapable & (0x1 << 2)), (ServiceDiscoverCapable & (0x1 << 3)),
			(ServiceDiscoverCapable & (0x1 << 4)), (ServiceDiscoverCapable & (0x1 << 5)), (ServiceDiscoverCapable & (0x1 << 6)), (ServiceDiscoverCapable & (0x1 << 7)),
			(ServiceDiscoverCapable & (0x1 << 8)), (ServiceDiscoverCapable & (0x1 << 9)), (ServiceDiscoverCapable & (0x1 << 10))));
#endif /* WFD_SUPPORT */

	return TRUE;
}

INT Set_P2P_Send_Service_Discovery_Init_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR		p2pindex = P2P_NOT_FOUND;
	PUCHAR	pAddr;
	UCHAR ConnAddr[6];
	UINT32	i;
	extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];


	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	DBGPRINT(RT_DEBUG_ERROR, ("%s::  Send Service Discovery to DevAddr[%s]\n", __FUNCTION__, arg));

	NdisMoveMemory(&ConnAddr[0], &ZERO_MAC_ADDR[0], MAC_ADDR_LEN);
	if (rtstrmactohex(arg, (RTMP_STRING *) &ConnAddr[0]) == FALSE)
		return FALSE;

	DBGPRINT(RT_DEBUG_ERROR, ("%s:: DevMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	__FUNCTION__, PRINT_MAC(ConnAddr)));
	
	if (MAC_ADDR_EQUAL(ConnAddr, ZERO_MAC_ADDR))
	{
		P2pLinkDown(pAd, RT_P2P_DISCONNECTED);
		return TRUE;
	}

	for (i=0; i < pAd->P2pTable.ClientNumber; i++)
	{
		if (MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].addr, ConnAddr) ||
			MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].bssid, ConnAddr) ||
			MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].InterfaceAddr, ConnAddr))
		{
			p2pindex = i;
			break;
		}
	}

	if ((p2pindex < pAd->P2pTable.ClientNumber) && (p2pindex != P2P_NOT_FOUND))
	{
		pAddr = &pAd->P2pTable.Client[p2pindex].addr[0];
		P2pConnectPrepare(pAd, pAddr, P2PSTATE_SERVICE_DISCOVER_INIT_COMMAND);
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("Table Idx out of range!\n"));

	return TRUE;
}

INT Set_P2P_Send_Service_Discovery_Comeback_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR		p2pindex = P2P_NOT_FOUND;
	PUCHAR	pAddr;
	UCHAR ConnAddr[6];
	UINT32	i;
	extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];


	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	DBGPRINT(RT_DEBUG_ERROR, ("%s::  Send Service Discovery to DevAddr[%s]\n", __FUNCTION__, arg));

	NdisMoveMemory(&ConnAddr[0], &ZERO_MAC_ADDR[0], MAC_ADDR_LEN);
	if (rtstrmactohex(arg, (RTMP_STRING *) &ConnAddr[0]) == FALSE)
		return FALSE;

	DBGPRINT(RT_DEBUG_ERROR, ("%s:: DevMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	__FUNCTION__, PRINT_MAC(ConnAddr)));
	
	if (MAC_ADDR_EQUAL(ConnAddr, ZERO_MAC_ADDR))
	{
		P2pLinkDown(pAd, RT_P2P_DISCONNECTED);
		return TRUE;
	}

	for (i=0; i < pAd->P2pTable.ClientNumber; i++)
	{
		if (MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].addr, ConnAddr) ||
			MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].bssid, ConnAddr) ||
			MAC_ADDR_EQUAL(pAd->P2pTable.Client[i].InterfaceAddr, ConnAddr))
		{
			p2pindex = i;
			break;
		}
	}

	if ((p2pindex < pAd->P2pTable.ClientNumber) && (p2pindex != P2P_NOT_FOUND))
	{
		pAddr = &pAd->P2pTable.Client[p2pindex].addr[0];
		P2pConnectPrepare(pAd, pAddr, P2PSTATE_SERVICE_COMEBACK_COMMAND);
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("Table Idx out of range!\n"));

	return TRUE;
}


INT Set_P2P_DelDevByAddr_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE pObj;
	UCHAR DevAddr[6] = {0};
	PMAC_TABLE_ENTRY pEntry = NULL;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if (rtstrmactohex(arg, (RTMP_STRING *) &DevAddr[0]) == FALSE)
		return FALSE;

	DBGPRINT(RT_DEBUG_TRACE, ("%s:: DevMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	__FUNCTION__, PRINT_MAC(DevAddr)));

	pEntry = MacTableLookup(pAd, DevAddr);

	if (pEntry)
	{
		PUCHAR		pOutBuffer = NULL;		
		ULONG		FrameLen = 0;
		USHORT 		Reason = REASON_NO_LONGER_VALID;
		HEADER_802_11 DeAuthHdr;
		
		MlmeAllocateMemory(pAd, &pOutBuffer);

		if (pOutBuffer)
		{
			DBGPRINT(RT_DEBUG_WARN, ("Send DEAUTH - Reason = %d frame tO %02x:%02x:%02x:%02x:%02x:%02x \n",
										Reason, PRINT_MAC(DevAddr)));

			MgtMacHeaderInit(pAd, &DeAuthHdr, SUBTYPE_DEAUTH, 0, 
								pEntry->Addr,
								pEntry->wdev->if_addr,
								pEntry->bssid);
	    	MakeOutgoingFrame(pOutBuffer,            &FrameLen,
	    	                  sizeof(HEADER_802_11), &DeAuthHdr,
	    	                  2,                     &Reason,
	    	                  END_OF_ARGS);

	    	MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
	    	MlmeFreeMemory(pAd, pOutBuffer);
			MacTableDeleteEntry(pAd, pEntry->wcid, pEntry->Addr);
			P2pGroupTabDelete(pAd, P2P_NOT_FOUND, DevAddr);
		}
	}
	
	return TRUE;

}

INT Set_P2P_DevDiscPeriod_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	pP2PCtrl->DevDiscPeriod = (UINT32)simple_strtol(arg, 0, 10);
	
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: P2P Device Discovery Period = %lu.\n", __FUNCTION__, pP2PCtrl->DevDiscPeriod));
	return TRUE;
}

INT Set_P2P_PriDeviceType_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE		pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return FALSE;

	if (strlen(arg) != 16)
		return FALSE;

	AtoH(arg, pP2PCtrl->DevInfo.PriDeviceType, 8);
	
	hex_dump("Set_P2P_PriDeviceType_Proc:: PriDeviceType", pP2PCtrl->DevInfo.PriDeviceType, 8);
	return TRUE;
}

/*
	Usage Example:
	Reference P2P Spec. v1.1 Table 29 , Device Info attribute format.
	1). 01 = length , 0007 == > Category ID (Display), 0050F204 ==>WFA OUI, 0001 ==> Sub Category ID
	iwpriv p2p0 set p2p2ndDevTypeList=0100070050F2040001
	
	2). 02 = length , 0007 == > Category ID (Display), 0050F204 ==>WFA OUI, 0001 ==> Sub Category ID ;  0007 == > Category ID (Display), 0050F204 ==>WFA OUI, 0002 ==> Sub Category ID ;
	iwpriv p2p0 set p2p2ndDevTypeList=0200070050F204000100070050F2040002

	3). clean up 2nd Device Type List 
	iwpriv p2p0 set p2p2ndDevTypeList=	
*/

INT Set_P2P_SecDevTypeList_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE		pObj;
	UCHAR len=0;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return FALSE;

	len = strlen(arg);

	if (len == 0) //reset to zero
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: reset 2nd Device Type List!\n", __FUNCTION__));
		pP2PCtrl->DevInfo.SecDevTypList[0]=0x00; /* length is zero, so no don't need following 8 bytes*/
	}
	else if((len >= (9*2))) /* 9 = 1 (length) + 8 (Category ID + WFA OUI + Sub-Category ID */
	{
		if ((len % (8*2))==2)
			
		{
			UCHAR tpylen=0;
			AtoH(arg, (PUCHAR)&tpylen, 1);
			AtoH(arg, pP2PCtrl->DevInfo.SecDevTypList, (tpylen*8)+1);
			hex_dump("Set_P2P_SecDevTypeList_Proc : p2p2ndDevTypList",pP2PCtrl->DevInfo.SecDevTypList, (tpylen*8)+1);
			
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: arg length incorrect\n", __FUNCTION__));	
			return FALSE;
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: arg length is not enough!\n", __FUNCTION__));	
		return FALSE;
	}

	return TRUE;
}

INT Set_P2P_DelPerstTable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    	P2pPerstTabClean(pAd);
}

INT Set_P2P_DelPerstEntry_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	UCHAR MacAddr[6];

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	DBGPRINT(RT_DEBUG_ERROR, ("%s::  del perstEntry by DevAddr[%s]\n", __FUNCTION__, arg));

	if (rtstrmactohex(arg, (RTMP_STRING *) &MacAddr[0]) == FALSE)
		return FALSE;
	
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: DevMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	__FUNCTION__, PRINT_MAC(MacAddr)));

    	P2pPerstTabDelete(pAd, MacAddr);
}

INT Set_P2P_Cancel_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE		pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;
	PWSC_CTRL pWscControl = NULL;
	INT val = (INT)simple_strtol(arg, 0, 10);

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return FALSE;

	if (val == 1)
	{
		pP2PCtrl->P2PConnectState = P2P_CONNECT_IDLE;
		if (!MAC_ADDR_EQUAL(&ZERO_MAC_ADDR[0], &pP2PCtrl->ConnectingMAC[0]))
		{
			UCHAR p2pindex;
	
			p2pindex = P2pGroupTabSearch(pAd, pP2PCtrl->ConnectingMAC);
			if (p2pindex < MAX_P2P_GROUP_SIZE)
			{
				if (pAd->P2pTable.Client[p2pindex].Rule == P2P_IS_GO)
					pAd->P2pTable.Client[p2pindex].P2pClientState = P2PSTATE_DISCOVERY_GO;
				else
					pAd->P2pTable.Client[p2pindex].P2pClientState = P2PSTATE_DISCOVERY;
				pAd->P2pTable.Client[p2pindex].StateCount = 0;
			}
			NdisZeroMemory(pP2PCtrl->ConnectingMAC, MAC_ADDR_LEN);
		}

		if (P2P_GO_ON(pAd))
		{
			UINT32 i, p2pEntryCnt=0;
			MAC_TABLE_ENTRY	*pEntry;
			INT	 IsAPConfigured;
			
			pWscControl = &pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl;
			IsAPConfigured = pWscControl->WscConfStatus;
			if (pWscControl->bWscTrigger)
			{
				WscStop(pAd, FALSE, pWscControl);
				pWscControl->WscPinCode = 0;
				WscBuildBeaconIE(pAd, IsAPConfigured, FALSE, 0, 0, MAIN_MBSSID, NULL, 0, AP_MODE);
				WscBuildProbeRespIE(pAd, WSC_MSGTYPE_AP_WLAN_MGR, IsAPConfigured, FALSE, 0, 0, (MAIN_MBSSID | MIN_NET_DEVICE_FOR_P2P_GO), NULL, 0, AP_MODE);
				APUpdateBeaconFrame(pAd, pObj->ioctl_if);
			}

			for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
			{
				pEntry = &pAd->MacTab.Content[i];
				if (IS_P2P_GO_ENTRY(pEntry) && (pEntry->WpaState == AS_PTKINITDONE))
					p2pEntryCnt++;
			}
			DBGPRINT(RT_DEBUG_ERROR, ("%s:: Total= %d. p2pEntry = %d.\n", __FUNCTION__, pAd->MacTab.Size, p2pEntryCnt));
			if ((p2pEntryCnt == 0) && (pAd->flg_p2p_OpStatusFlags == P2P_GO_UP))
			{
#ifdef RTMP_MAC_USB
				RTEnqueueInternalCmd(pAd, CMDTHREAD_SET_P2P_LINK_DOWN, NULL, 0);	
#endif /* RTMP_MAC_USB */
			}
	    }
		else if (P2P_CLI_ON(pAd))
		{
			pWscControl = &pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl;
			pWscControl->WscPinCode = 0;
			if (pWscControl->bWscTrigger)
				WscStop(pAd, TRUE, pWscControl);
			P2pLinkDown(pAd, P2P_DISCONNECTED);
		}
		P2pStopScan(pAd);
		pP2PCtrl->bPeriodicListen = TRUE;
		/* Reset bConfirmByUI to its default value */
		pP2PCtrl->bConfirmByUI = pP2PCtrl->bP2pAutoAccept;

		if (INFRA_ON(pAd) || P2P_GO_ON(pAd) || P2P_CLI_ON(pAd))
		{
			INT ch;

			// TODO: shiang-6590, why we set bw as 40 when BBPCurrentBW == BW_40??
			if (pAd->CommonCfg.BBPCurrentBW == BW_40)
			{
				bbp_set_bw(pAd, BW_40);

				ch = pAd->CommonCfg.CentralChannel;
			}
			else
			{
				ch = pAd->CommonCfg.Channel;
			}

			AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
			AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s:: Cancel P2P action\n", __FUNCTION__));
	return TRUE;
}

INT Set_P2P_ConfirmByUI_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if (simple_strtol(arg, 0, 10) == 0)
		pP2PCtrl->bConfirmByUI = FALSE;
	else
		pP2PCtrl->bConfirmByUI = TRUE;
	
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: bConfirmByUI = %d\n", __FUNCTION__, pP2PCtrl->bConfirmByUI));
	return TRUE;
}

INT Set_P2P_Discoverable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if (simple_strtol(arg, 0, 10) == 0)
		pP2PCtrl->bSentProbeRSP = FALSE;
	else
		pP2PCtrl->bSentProbeRSP = TRUE;
	
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: bSentProbeRSP = %d\n", __FUNCTION__, pP2PCtrl->bSentProbeRSP));
	return TRUE;
}

INT Set_P2P_CheckPeerChannel_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;
	UINT32	PolicyMode;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	PolicyMode = simple_strtol(arg, 0, 10);
	if ((PolicyMode >= CHECK_PEER_CHANNEL_DISABLE) &&
		(PolicyMode <= CHECK_PEER_CHANNEL_EVEN_IF_CONNECTION))
		pP2PCtrl->CheckPeerChannelPolicy = PolicyMode;
	else
		pP2PCtrl->CheckPeerChannelPolicy = CHECK_PEER_CHANNEL_IF_NO_CONNECTION;
	
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: CheckPeerChannelPolicy = %d\n", __FUNCTION__, pP2PCtrl->CheckPeerChannelPolicy));
	return TRUE;
}

INT Set_P2P_AutoAccept_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if (simple_strtol(arg, 0, 10) == 0)
		pP2PCtrl->bP2pAutoAccept= FALSE;
	else
		pP2PCtrl->bP2pAutoAccept = TRUE;

	pP2PCtrl->bConfirmByUI = pP2PCtrl->bP2pAutoAccept;
	
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: bP2pAutoAccept = %d\n", __FUNCTION__, pP2PCtrl->bP2pAutoAccept));
	return TRUE;
}

INT Set_P2P_AutoChannelCtrl(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;
	INT val = (INT)simple_strtol(arg, 0, 10);

	UCHAR channel_idx;
	UCHAR channel_index;

	PCHANNELINFO pChannelInfo = pAd->pChannelInfo;
	   
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

       // no value
	if (strlen(arg) < 1)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s:: AutoOpCh = %d. default op ch = %d/%d, arg: %d \n", __FUNCTION__, pAd->P2pCfg.GroupChannel, pAd->P2pCfg.GroupChannel,pAd->P2pCfg.bAutoChannel ,pAd->ApCfg.AutoChannelAlg));

		DBGPRINT(RT_DEBUG_TRACE, ("=====================================================\n"));
 		for (channel_index=0 ; channel_index < pAd->ChannelListNum ; channel_index++)
			/* debug messages */
			DBGPRINT(RT_DEBUG_TRACE, ("Channel %d : Dirty = %ld, ApCnt=%ld, Busy Time = %d, Skip Channel = %s\n", 
			pAd->ChannelList[channel_index].Channel,
			pChannelInfo->dirtyness[channel_index], 
			pChannelInfo->ApCnt[channel_index],
#ifdef AP_QLOAD_SUPPORT
			pChannelInfo->chanbusytime[channel_index],
#else
			0,
#endif /* AP_QLOAD_SUPPORT */
			(pChannelInfo->SkipList[channel_index] == TRUE) ? "TRUE" : "FALSE"));
		DBGPRINT(RT_DEBUG_TRACE, ("=====================================================\n"));
   
		for (channel_idx = 0; channel_idx < pAd->ChannelListNum; channel_idx++)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Channel %d : Dirty = %ld, False CCA = %u, Busy Time = %u, Skip Channel = %s\n",
					pAd->ChannelList[channel_idx].Channel,
					pChannelInfo->dirtyness[channel_idx],
					pChannelInfo->FalseCCA[channel_idx],
#ifdef AP_QLOAD_SUPPORT
					pChannelInfo->chanbusytime[channel_idx],
#else
					0,
#endif /* AP_QLOAD_SUPPORT */
					(pChannelInfo->SkipList[channel_idx] == TRUE) ? "TRUE" : "FALSE"));
		}		
	
		return 0;
	}

	pP2PCtrl->bAutoChannel = (val & 0x1)  ? TRUE : FALSE;
	pP2PCtrl->bAutoChOnSocial = (val & 0x2)  ? TRUE : FALSE;


	DBGPRINT(RT_DEBUG_ERROR, ("%s:: bAutoChannel = %d\n", __FUNCTION__, pP2PCtrl->bAutoChannel));	
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: bAutoChOnSocial = %d\n", __FUNCTION__, pP2PCtrl->bAutoChOnSocial));
	return TRUE;
}

INT Set_P2P_Go_Accept_Invitation_Request(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE			pObj;
	PRT_P2P_CONFIG	pP2PCtrl = &pAd->P2pCfg;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	if (pObj->ioctl_if_type != INT_P2P)
		return 0;

	if (simple_strtol(arg, 0, 10) == 0)
		pP2PCtrl->bP2pGoAcceptInvitationReq = FALSE;
	else
		pP2PCtrl->bP2pGoAcceptInvitationReq = TRUE;
		
	DBGPRINT(RT_DEBUG_ERROR, ("%s:: bP2pGoAcceptInvitationReq = %d.\n", __FUNCTION__, pP2PCtrl->bP2pGoAcceptInvitationReq));
	return TRUE;
}

#endif /* P2P_SUPPORT */

