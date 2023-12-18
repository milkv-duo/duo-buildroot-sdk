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
	rt_profile.c
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */
 
#include "rt_config.h"

#ifdef CONFIG_AP_SUPPORT
static PUCHAR nv_buffer = 
"#The word of \"Default\" must not be removed\n"
"Default\n"
"CountryRegion=5\n"
"CountryRegionABand=1\n"
"CountryCode=\n"
"BssidNum=2\n"
"SSID1=2880_VxAP\n"
"WirelessMode=8\n"
"TxRate=0\n"
"Channel=60\n"
"BasicRate=15\n"
"BeaconPeriod=100\n"
"DtimPeriod=1\n"
"TxPower=100\n"
"DisableOLBC=0\n"
"BGProtection=0\n"
"TxAntenna=\n"
"RxAntenna=\n"
"TxPreamble=0\n"
"RTSThreshold=2347\n"
"FragThreshold=2346\n"
"TxBurst=1\n"
"PktAggregate=1\n"
"TurboRate=0\n"
"WmmCapable=1\n"
"APAifsn=3;7;1;1\n"
"APCwmin=4;4;3;2\n"
"APCwmax=6;10;4;3\n"
"APTxop=0;0;94;47\n"
"APACM=0;0;0;0\n"
"BSSAifsn=3;7;2;2\n"
"BSSCwmin=4;4;3;2\n"
"BSSCwmax=10;10;4;3\n"
"BSSTxop=0;0;94;47\n"
"BSSACM=0;0;0;0\n"
"AckPolicy=0;0;0;0\n"
"APSDCapable=0\n"
"DLSCapable=0\n"
"NoForwarding=0\n"
"NoForwardingBTNBSSID=0\n"
"HideSSID=0\n"
"ShortSlot=1\n"
"AutoChannelSelect=0\n"
"SecurityMode=0\n"
"VLANEnable=0\n"
"VLANName=\n"
"VLANID=0\n"
"VLANPriority=0\n"
"WscConfMode=0\n"
"WscConfStatus=2\n"
"WscAKMP=1\n"
"WscConfigured=1\n"
"WscModeOption=0\n"
"WscActionIndex=9\n"
"WscPinCode=\n"
"WscRegResult=1\n"
"WscUseUPnP=1\n"
"WscUseUFD=0\n"
"WscSSID=RalinkInitialAP\n"
"WscKeyMGMT=WPA-EAP\n"
"WscConfigMethod=138\n"
"WscAuthType=1\n"
"WscEncrypType=1\n"
"WscNewKey=scaptest\n"
"IEEE8021X=0\n"
"IEEE80211H=0\n"
"CSPeriod=6\n"
"PreAuth=0\n"
"AuthMode=OPEN\n"
"EncrypType=NONE\n"
"RekeyInterval=3600\n"
"RekeyMethod=DISABLE\n"
"PMKCachePeriod=10\n"
/*"WPAPSK1=12345678\n" */
"DefaultKeyID=1\n"
"Key1Type=0\n"
"Key1Str1=\n"
"Key2Type=0\n"
"Key2Str1=\n"
"Key3Type=0\n"
"Key3Str1=\n"
"Key4Type=0\n"
"Key4Str1=\n"
"HSCounter=0\n"
"HT_HTC=1\n"
"HT_RDG=1\n"
"HT_LinkAdapt=0\n"
"HT_OpMode=0\n"
"HT_MpduDensity=5\n"
"HT_EXTCHA=1\n"
"HT_BW=1\n"
"HT_AutoBA=1\n"
"HT_BADecline=0\n"
"HT_AMSDU=0\n"
"HT_BAWinSize=64\n"
"HT_GI=1\n"
"HT_STBC=1\n"
"HT_MCS=33\n"
"HT_PROTECT=1\n"
"HT_MIMOPS=3\n"
"HT_40MHZ_INTOLERANT=0\n"
"HT_TxStream=2\n"
"HT_RxStream=2\n"
"NintendoCapable=0\n"
"AccessPolicy0=0\n"
"AccessControlList0=\n"
"AccessPolicy1=0\n"
"AccessControlList1=\n"
"AccessPolicy2=0\n"
"AccessControlList2=\n"
"AccessPolicy3=0\n"
"AccessControlList3=\n"
"WdsEnable=0\n"
"WdsPhyMode=0\n"
"WdsEncrypType=NONE\n"
"WdsList=\n"
"WdsKey=\n"
"WirelessEvent=0\n"
"RADIUS_Server=\n"
"RADIUS_Port=1812\n"
"RADIUS_Key=\n"
"RADIUS_Acct_Server=\n"
"RADIUS_Acct_Port=1813\n"
"RADIUS_Acct_Key=\n"
"session_timeout_interval=0\n"
"idle_timeout_interval=0\n"
"staWirelessMode=9\n"
"upnpEnabled=0\n"
"pppoeREnabled=0\n"
"RDRegion=JAP\n"
;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
static PUCHAR nv_buffer = 
"#The word of \"Default\" must not be removed\n"
"Default\n"
"CountryRegion=5\n"
"CountryRegionABand=7\n"
"CountryCode=\n"
"ChannelGeography=1\n"
"SSID=jay_kodak_ap\n"
"NetworkType=Infra\n"
"WirelessMode=5\n"
"Channel=0\n"
"BeaconPeriod=100\n"
"TxPower=100\n"
"BGProtection=0\n"
"TxPreamble=0\n"
"RTSThreshold=2347\n"
"FragThreshold=2346\n"
"TxBurst=1\n"
"PktAggregate=0\n"
"WmmCapable=1\n"
"AckPolicy=0;0;0;0\n"
"AuthMode=WPAPSK\n"
"EncrypType=AES\n"
"WPAPSK=1234567890\n"
"DefaultKeyID=2\n"
"Key1Type=0\n"
"Key1Str=0987654321\n"
"Key2Type=\n"
"Key2Str=1234567890\n"
"Key3Type=0\n"
"Key3Str=\n"
"Key4Type=0\n"
"Key4Str=\n"
"PSMode=CAM\n"
"AutoRoaming=0\n"
"RoamThreshold=70\n"
"APSDCapable=0\n"
"APSDAC=0;0;0;0\n"
"HT_RDG=0\n"
"HT_EXTCHA=0\n"
"HT_OpMode=1\n"
"HT_MpduDensity=4\n"
"HT_BW=0\n"
"HT_AutoBA=0\n"
"HT_BADecline=0\n"
"HT_AMSDU=0\n"
"HT_BAWinSize=64\n"
"HT_GI=1\n"
"HT_MCS=33\n"
"HT_MIMOPSMode=3\n"
"HT_DisallowTKIP=1\n"
"IEEE80211H=0\n"
"TGnWifiTest=0\n"
"WirelessEvent=0\n"
"CarrierDetect=0\n"
"AntDiversity=0\n"
"BeaconLostTime=4\n"
"FtSupport=1\n"
;
#endif /* CONFIG_STA_SUPPORT */

NDIS_STATUS	RTMPReadParametersHook(
	IN	PRTMP_ADAPTER pAd)
{
	RTMP_STRING *src = NULL;
	RTMP_OS_FD				srcf;
	RTMP_OS_FS_INFO			osFSInfo;
	INT 						retval, readLen;
	RTMP_STRING *buffer;
	INT						i = 0;
	BOOLEAN					bUseDefault = TRUE;

	buffer = kmalloc(MAX_INI_BUFFER_SIZE, MEM_ALLOC_FLAG);
	if(buffer == NULL)
		return NDIS_STATUS_FAILURE;

#ifdef RTMP_RBUS_SUPPORT
	if (pAd->infType == RTMP_DEV_INF_RBUS)
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			src = AP_PROFILE_PATH_RBUS;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			src = STA_PROFILE_PATH_RBUS;
		}
#endif /* CONFIG_STA_SUPPORT */
#ifdef MULTIPLE_CARD_SUPPORT
		src = pAd->MC_FileName;
#endif /* MULTIPLE_CARD_SUPPORT */
	}
	else
#endif /* RTMP_RBUS_SUPPORT */
	{	
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			src = AP_PROFILE_PATH;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			src = STA_PROFILE_PATH;
		}
#endif /* CONFIG_STA_SUPPORT */
#ifdef MULTIPLE_CARD_SUPPORT
		src = (RTMP_STRING *)pAd->MC_FileName;
#endif /* MULTIPLE_CARD_SUPPORT */
	}

	if (src && *src)
	{	
		RtmpOSFSInfoChange(&osFSInfo, TRUE);
		
		srcf = RtmpOSFileOpen(src, O_RDONLY, 0);
		if (IS_FILE_OPEN_ERR(srcf)) 
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Error opening profile \"%s\"\n", src));
		}
		else 
		{
			memset(buffer, 0x00, MAX_INI_BUFFER_SIZE);
			retval=RtmpOSFileRead(srcf, buffer, MAX_INI_BUFFER_SIZE);
			if (retval > 0)
				bUseDefault = FALSE;
			else
				DBGPRINT(RT_DEBUG_ERROR, ("Read file \"%s\" failed(errCode=%d)!\n", src, retval));

			retval=RtmpOSFileClose(srcf);
			if (retval)
				DBGPRINT(RT_DEBUG_ERROR, ("Close file \"%s\" failed(errCode=%d)!\n", src, retval));
		}
		
		RtmpOSFSInfoChange(&osFSInfo, FALSE);
	}

	if (bUseDefault)
		RTMPSetProfileParameters(pAd, nv_buffer);
	else
		RTMPSetProfileParameters(pAd, buffer);

#ifdef CONFIG_STA_SUPPORT
#ifdef SIGMATEL_SDK
	SYAdjustSTAParamFromNVRAM(pAd);
#endif /* SIGMATEL_SDK */
#endif /* CONFIG_STA_SUPPORT */
	
	kfree(buffer);
	
	return (NDIS_STATUS_SUCCESS);	
}

