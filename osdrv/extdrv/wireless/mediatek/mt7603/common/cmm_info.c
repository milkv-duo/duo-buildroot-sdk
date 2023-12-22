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
	cmm_info.c
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */
 
#include	"rt_config.h"


#ifdef MT_MAC
INT SetManualTxOP(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->CommonCfg.ManualTxop = (UCHAR) simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_OFF, ("CURRENT: Set ManualTxOP = %d \n", pAd->CommonCfg.ManualTxop));

    return TRUE;
}


INT SetManualTxOPThreshold(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    pAd->CommonCfg.ManualTxopThreshold = (UCHAR) simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_OFF, ("CURRENT: Set ManualTxOP TP Threshold = %lu (Mbps)\n", pAd->CommonCfg.ManualTxopThreshold));

    return TRUE;
}


INT SetManualTxOPUpBound(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    pAd->CommonCfg.ManualTxopUpBound = (UCHAR) simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_OFF, ("CURRENT: Set ManualTxOP Traffic Upper Bound = %d (Ratio)\n", pAd->CommonCfg.ManualTxopUpBound));

    return TRUE;
}


INT SetManualTxOPLowBound(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    pAd->CommonCfg.ManualTxopLowBound = (UCHAR) simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_OFF, ("CURRENT: Set ManualTxOP Traffic Low Bound = %d (Ratio)\n", pAd->CommonCfg.ManualTxopLowBound));

    return TRUE;
}
#endif /* MT_MAC */

/*
    ==========================================================================
    Description:
        Get Driver version.

    Return:
    ==========================================================================
*/
#ifdef SMART_CARRIER_SENSE_SUPPORT
INT SetSCSEnable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 value;

	value = simple_strtol(arg, 0, 10);
	if (value > 500) {
		DBGPRINT(RT_DEBUG_OFF, ("==>%s (Traffice Threshold=%d)\n", __FUNCTION__, value));
	} else {
		if (value == 1) {
			pAd->SCSCtrl.SCSEnable = SCS_ENABLE;
			DBGPRINT(RT_DEBUG_OFF, ("==>%s (ON)\n", __FUNCTION__));
		} else if (value == 0) {
			pAd->SCSCtrl.SCSEnable = SCS_DISABLE;
			DBGPRINT(RT_DEBUG_OFF, ("==>%s (OFF)\n", __FUNCTION__));
			/* Restore to default */
			RTMP_IO_WRITE32(pAd, CR_AGC_0, pAd->SCSCtrl.CR_AGC_0_default);
			RTMP_IO_WRITE32(pAd, CR_AGC_0_RX1, pAd->SCSCtrl.CR_AGC_0_default);
			RTMP_IO_WRITE32(pAd, CR_AGC_3, pAd->SCSCtrl.CR_AGC_3_default);
			RTMP_IO_WRITE32(pAd, CR_AGC_3_RX1, pAd->SCSCtrl.CR_AGC_3_default);

			pAd->SCSCtrl.SCSStatus = SCS_STATUS_DEFAULT;
		} else
			DBGPRINT(RT_DEBUG_OFF, ("==>%s (Unknow value = %d)\n", __FUNCTION__, value));
	}

	return TRUE;
}

INT SetSCSCfg_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT32	Recv = 0;
	INT	SCSMinRssiTolerance = 0, SCSTrafficThreshold = 0, FalseCcaUpBond = 0;
	INT FalseCcaLowBond = 0, FixedBond = 0, ForceMode = 0;

	Recv = sscanf(arg, "%d-%d-%d-%d-%d-%d", &(SCSMinRssiTolerance), &(SCSTrafficThreshold),
		&(FalseCcaUpBond), &(FalseCcaLowBond), &(FixedBond), &(ForceMode));
	if (Recv != 6) {
		DBGPRINT(RT_DEBUG_OFF, ("Format Error!\n"));
		DBGPRINT(RT_DEBUG_OFF, ("iwpriv ra0 set SCSCfg=[MinRssiTolerance]-"));
		DBGPRINT(RT_DEBUG_OFF, ("[TrafficThreshold]-[FalseCcaUpBoundary]-"));
		DBGPRINT(RT_DEBUG_OFF, ("[FalseCcaLowBoundary]-[FixedBoundary]-[ForceMode]\n"));
		DBGPRINT(RT_DEBUG_OFF, ("PS: FiexedBond is Negative number. Ex:70 means -70dBm"));
	} else {
		pAd->SCSCtrl.SCSMinRssiTolerance = (UINT8)SCSMinRssiTolerance;
		pAd->SCSCtrl.SCSTrafficThreshold = SCSTrafficThreshold;
		pAd->SCSCtrl.FalseCcaUpBond = (UINT16)FalseCcaUpBond;
		pAd->SCSCtrl.FalseCcaLowBond = (UINT16)FalseCcaLowBond;
		pAd->SCSCtrl.FixedRssiBond = (0 - (CHAR)FixedBond);
		pAd->SCSCtrl.ForceMode = (BOOLEAN)ForceMode;
	}
	return TRUE;
}

INT SetSCSDbgLogEnable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 value;

	value = simple_strtol(arg, 0, 10);
	#if 0
	if (value == SCS_DBG_LOG_DISABLE) {
		pAd->SCSCtrl.SCSDbgLogLv = RT_DEBUG_INFO;
		DBGPRINT(RT_DEBUG_OFF, ("==>%s: Disable SCS debug log\n", __FUNCTION__));
	} else if (value == SCS_DBG_LOG_ENABLE) {
		pAd->SCSCtrl.SCSDbgLogLv = RT_DEBUG_OFF;
		DBGPRINT(RT_DEBUG_OFF, ("==>%s: Enable SCS debug log\n", __FUNCTION__));
	} else
		DBGPRINT(RT_DEBUG_OFF, ("==>%s: Wrong value(0:Disable;1:Enable)\n", __FUNCTION__));
	#endif
	return TRUE;
}
INT32 ShowSCSInfo(RTMP_ADAPTER *pAd, RTMP_STRING *Arg)
{
	DBGPRINT(RT_DEBUG_OFF, ("========Configuration==========\n"));
	DBGPRINT(RT_DEBUG_OFF, ("SCS enable =%d\n", pAd->SCSCtrl.SCSEnable));
	DBGPRINT(RT_DEBUG_OFF, ("MinRssi Tolerance=%d\n", pAd->SCSCtrl.SCSMinRssiTolerance));
	DBGPRINT(RT_DEBUG_OFF, ("FixedRssiBoundary=%d\n", pAd->SCSCtrl.FixedRssiBond));
	DBGPRINT(RT_DEBUG_OFF, ("False CCA up boundayr =%d\n", pAd->SCSCtrl.FalseCcaUpBond));
	DBGPRINT(RT_DEBUG_OFF, ("False CCA low boundayr =%d\n", pAd->SCSCtrl.FalseCcaLowBond));
	DBGPRINT(RT_DEBUG_OFF, ("Traffic threshold =%d\n", pAd->SCSCtrl.SCSTrafficThreshold));
	DBGPRINT(RT_DEBUG_OFF, ("ForceMode = %d\n", pAd->SCSCtrl.ForceMode));
	DBGPRINT(RT_DEBUG_OFF, ("========Statistic /Status==========\n"));
	DBGPRINT(RT_DEBUG_OFF, ("Min RSSI=%d\n", pAd->SCSCtrl.SCSMinRssi));
	DBGPRINT(RT_DEBUG_OFF, ("PD count=%x\n", pAd->SCSCtrl.PdCount));
	DBGPRINT(RT_DEBUG_OFF, ("MDRDY count=%x\n", pAd->SCSCtrl.MdrdyCount));
	DBGPRINT(RT_DEBUG_OFF, ("FalseCCA count=%d\n", pAd->SCSCtrl.FalseCCA));
	DBGPRINT(RT_DEBUG_OFF, ("Current RSSI boundary =%d\n", pAd->SCSCtrl.RSSIBoundary));
	DBGPRINT(RT_DEBUG_OFF, ("RTS count =%d\n", pAd->SCSCtrl.RtsCount));
	DBGPRINT(RT_DEBUG_OFF, ("RTS retry count =%d\n", pAd->SCSCtrl.RtsRtyCount));
	DBGPRINT(RT_DEBUG_OFF, ("Tx Traffic = %d\n",
		pAd->RalinkCounters.OneSecTransmittedByteCount));
	DBGPRINT(RT_DEBUG_OFF, ("Rx Traffic = %d\n", pAd->RalinkCounters.OneSecReceivedByteCount));
	DBGPRINT(RT_DEBUG_OFF, ("Current Sensitivity = %d\n", pAd->SCSCtrl.CurrSensitivity));
	DBGPRINT(RT_DEBUG_OFF, ("Adjust Sensitivity = %d\n", pAd->SCSCtrl.AdjustSensitivity));

	return TRUE;
}
#endif /* SMART_CARRIER_SENSE_SUPPORT */

INT Set_DriverVersion_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		DBGPRINT(RT_DEBUG_OFF, ("Driver version-%s\n", AP_DRIVER_VERSION));
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		DBGPRINT(RT_DEBUG_OFF, ("Driver version-%s \n", STA_DRIVER_VERSION));
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_ANDES_SUPPORT
	if (pAd->chipCap.MCUType == ANDES) { 
		UINT32 loop = 0;
		RTMP_CHIP_CAP *cap = &pAd->chipCap;
		
		if (pAd->chipCap.need_load_fw) {
			USHORT fw_ver, build_ver;
			fw_ver = (*(cap->FWImageName + 11) << 8) | (*(cap->FWImageName + 10));
			build_ver = (*(cap->FWImageName + 9) << 8) | (*(cap->FWImageName + 8));
			
			DBGPRINT(RT_DEBUG_OFF, ("fw version:%d.%d.%02d ", (fw_ver & 0xf000) >> 8,
							(fw_ver & 0x0f00) >> 8, fw_ver & 0x00ff));
			DBGPRINT(RT_DEBUG_OFF, ("build:%x\n", build_ver));
			DBGPRINT(RT_DEBUG_OFF, ("build time:"));
			
			for (loop = 0; loop < 16; loop++)
				DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->FWImageName + 16 + loop)));

			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}

		if (pAd->chipCap.need_load_rom_patch) {
			DBGPRINT(RT_DEBUG_OFF, ("rom patch version = \n"));

			for (loop = 0; loop < 4; loop++)
				DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + 24 + loop)));

			DBGPRINT(RT_DEBUG_OFF, ("\n"));
			
			DBGPRINT(RT_DEBUG_OFF, ("build time = \n")); 
	
			for (loop = 0; loop < 16; loop++)
				DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + loop)));
	
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}
	}
#endif

    return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set Country Region.
        This command will not work, if the field of CountryRegion in eeprom is programmed.
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT Set_CountryRegion_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int retval;
	
#ifdef EXT_BUILD_CHANNEL_LIST
	return -EOPNOTSUPP;
#endif /* EXT_BUILD_CHANNEL_LIST */

	retval = RT_CfgSetCountryRegion(pAd, arg, BAND_24G);
	if (retval == FALSE)
		return FALSE;
	
	/* if set country region, driver needs to be reset*/
	BuildChannelList(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_CountryRegion_Proc::(CountryRegion=%d)\n", pAd->CommonCfg.CountryRegion));
	
	return TRUE;
}


/* 
    ==========================================================================
    Description:
        Set Country Region for A band.
        This command will not work, if the field of CountryRegion in eeprom is programmed.
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT Set_CountryRegionABand_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int retval;

#ifdef EXT_BUILD_CHANNEL_LIST
	return -EOPNOTSUPP;
#endif /* EXT_BUILD_CHANNEL_LIST */

	retval = RT_CfgSetCountryRegion(pAd, arg, BAND_5G);
	if (retval == FALSE)
		return FALSE;

	/* if set country region, driver needs to be reset*/
	BuildChannelList(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_CountryRegionABand_Proc::(CountryRegion=%d)\n", pAd->CommonCfg.CountryRegionForABand));
	
	return TRUE;
}


INT	Set_Cmm_WirelessMode_Proc(
	IN RTMP_ADAPTER *pAd,
	IN RTMP_STRING *arg,
	IN BOOLEAN FlgIsDiffMbssModeUsed)
{
	INT	success = TRUE;
#ifdef CONFIG_AP_SUPPORT
	UINT32 i = 0;
#ifdef MBSS_SUPPORT
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if (FlgIsDiffMbssModeUsed) {
		LONG cfg_mode = simple_strtol(arg, 0, 10);
		
		/* assign wireless mode for the BSS */
		pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev.PhyMode = 
					cfgmode_2_wmode((UCHAR)cfg_mode);

		/*
			If the band is different with other BSS, we will correct it in
			RT_CfgSetMbssWirelessMode()
		*/
		success = RT_CfgSetMbssWirelessMode(pAd, arg);
	}
	else
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
	success = RT_CfgSetWirelessMode(pAd, arg);

	if (success)
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			/* recover Wmm Capable for "each" BSS */
			/* all phy mode of MBSS are the same */
			for(i=0; i<pAd->ApCfg.BssidNum; i++)
			{
				pAd->ApCfg.MBSSID[i].wdev.bWmmCapable = \
										pAd->ApCfg.MBSSID[i].bWmmCapableOrg;

#ifdef MBSS_SUPPORT
				/* In Same-MBSS Mode, all phy modes are the same */
				if (FlgIsDiffMbssModeUsed == 0)
					pAd->ApCfg.MBSSID[i].wdev.PhyMode = pAd->CommonCfg.PhyMode;
#endif /* MBSS_SUPPORT */
			}

			RTMPSetPhyMode(pAd, pAd->CommonCfg.PhyMode);
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			/* clean up previous SCAN result */
			BssTableInit(&pAd->ScanTab);
			pAd->StaCfg.LastScanTime = 0;
			
			RTMPSetPhyMode(pAd, pAd->CommonCfg.PhyMode);
#ifdef DOT11_N_SUPPORT
			if (WMODE_CAP_N(pAd->CommonCfg.PhyMode))
			{
				pAd->CommonCfg.BACapability.field.AutoBA = TRUE;
				pAd->CommonCfg.REGBACapability.field.AutoBA = TRUE;
			}
			else
			{
				pAd->CommonCfg.BACapability.field.AutoBA = FALSE;
				pAd->CommonCfg.REGBACapability.field.AutoBA = FALSE;
			}
#endif /* DOT11_N_SUPPORT */
			/* Set AdhocMode rates*/
			if (pAd->StaCfg.BssType == BSS_ADHOC)
			{
				MlmeUpdateTxRates(pAd, FALSE, 0);
				MakeIbssBeacon(pAd);           /* re-build BEACON frame*/
				AsicEnableIbssSync(pAd);       /* copy to on-chip memory*/
			}
		}
#endif /* CONFIG_STA_SUPPORT */

#ifdef RT305x
		RTMP_CHIP_SPECIFIC(pAd, RT305x_WLAN_MODE_CHANGE, NULL, 0);
#endif /* RT305x */

#ifdef CONFIG_AP_SUPPORT
#ifdef MBSS_SUPPORT
		DBGPRINT(RT_DEBUG_TRACE, ("Set_Cmm_WirelessMode_Proc::(=%d)\n", pAd->CommonCfg.PhyMode));
		DBGPRINT(RT_DEBUG_TRACE, ("Set_Cmm_WirelessMode_Proc::(BSS%d=%d)\n",
				pObj->ioctl_if, pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev.PhyMode));

		for(i=0; i<pAd->ApCfg.BssidNum; i++)
		{
			/*
				When last mode is not 11B-only, new mode is 11B, we need to re-make
				beacon frame content.

				Because we put support rate/extend support rate element in
				APMakeBssBeacon(), not APUpdateBeaconFrame().
			*/
			APMakeBssBeacon(pAd, i);
		}
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_WirelessMode_Proc::parameters out of range\n"));
	}
	
	return success;
}


#ifdef CONFIG_AP_SUPPORT
#ifdef MBSS_SUPPORT
/* 
    ==========================================================================
    Description:
        Set Wireless Mode for MBSS
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_MBSS_WirelessMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	return Set_Cmm_WirelessMode_Proc(pAd, arg, 1);
}
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

/* 
    ==========================================================================
    Description:
        Set Wireless Mode
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT Set_WirelessMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	return Set_Cmm_WirelessMode_Proc(pAd, arg, 0);
}
#ifdef RT_CFG80211_SUPPORT
INT Set_DisableCfg2040Scan_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->cfg80211_ctrl.FlgCfg8021Disable2040Scan = (UCHAR) simple_strtol(arg, 0, 10);	
	DBGPRINT(RT_DEBUG_TRACE, ("pAd->cfg80211_ctrl.FlgCfg8021Disable2040Scan  %d \n",pAd->cfg80211_ctrl.FlgCfg8021Disable2040Scan ));
	return TRUE;
}
#endif
#if defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392)
INT	Set_BbpResetFlagCountVale(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{

	pAd->BbpResetFlagCountVale = (UCHAR) simple_strtol(arg, 0, 10);
	
	DBGPRINT(RT_DEBUG_TRACE, ("pAd->BbpResetFlagCountVale  %d \n",pAd->BbpResetFlagCountVale ));
	return TRUE;
}
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) */


/* 
    ==========================================================================
    Description:
        Set Channel
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_Channel_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
#ifdef CONFIG_AP_SUPPORT
	INT32 i;
#endif /* CONFIG_AP_SUPPORT */
 	INT32 Success = TRUE;
	UCHAR Channel;	
	UCHAR RFChannel;

	Channel = (UCHAR) simple_strtol(arg, 0, 10);
			
	pAd->CommonCfg.Channel = Channel;

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
#ifdef APCLI_AUTO_CONNECT_SUPPORT
			if (pAd->ApCfg.ApCliAutoConnectChannelSwitching == FALSE)
				pAd->ApCfg.ApCliAutoConnectChannelSwitching = TRUE;
#endif /* APCLI_AUTO_CONNECT_SUPPORT */
		}
#endif /* CONFIG_AP_SUPPORT */

	/* check if this channel is valid*/
	if (ChannelSanity(pAd, Channel) == TRUE)
	{
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			/* Save the channel on MlmeAux for CntlOidRTBssidProc used. */
			pAd->MlmeAux.Channel = Channel;

#ifdef DOT11_N_SUPPORT
			N_ChannelCheck(pAd);
			if (WMODE_CAP_N(pAd->CommonCfg.PhyMode) &&
					pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40)
				RFChannel = N_SetCenCh(pAd, pAd->CommonCfg.Channel);
			else
#endif /* DOT11_N_SUPPORT */
				RFChannel = pAd->CommonCfg.Channel;

			AsicSwitchChannel(pAd, RFChannel, FALSE);
				
			DBGPRINT(RT_DEBUG_TRACE, ("%s(): CtrlChannel(%d), CentralChannel(%d) \n", 
							__FUNCTION__, pAd->CommonCfg.Channel,
							pAd->CommonCfg.CentralChannel));

#ifdef RT3593
			RT3593_MONITOR_CHAN_CHANGE(pAd);
#endif /* RT3593 */
		}
#endif /* CONFIG_STA_SUPPORT */
		Success = TRUE;
	}
	else
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			Channel = FirstChannel(pAd);
			DBGPRINT(RT_DEBUG_WARN,("This channel is out of channel list, set as the first channel(%d) \n ", Channel));
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		Success = FALSE;
		DBGPRINT(RT_DEBUG_WARN,("This channel is out of channel list, nothing to do!\n "));
#endif /* CONFIG_STA_SUPPORT */
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if ((WMODE_CAP_5G(pAd->CommonCfg.PhyMode))
			&& (pAd->CommonCfg.bIEEE80211H == TRUE))
		{
			for (i = 0; i < pAd->ChannelListNum; i++)
			{
				if (pAd->ChannelList[i].Channel == Channel)
				{
					if (pAd->ChannelList[i].RemainingTimeForUse > 0)
					{
						DBGPRINT(RT_DEBUG_ERROR, ("ERROR: previous detection of a radar on this channel(Channel=%d)\n", Channel));
						Success = FALSE;
						break;
					}
					else
					{
						DBGPRINT(RT_DEBUG_INFO, ("RemainingTimeForUse %d ,Channel %d\n",
								pAd->ChannelList[i].RemainingTimeForUse, Channel));
					}
				}
			}
		}

		if (Success == TRUE)
		{
			if ((pAd->CommonCfg.Channel > 14 )
				&& (pAd->CommonCfg.bIEEE80211H == TRUE))
			{
				pAd->Dot11_H.org_ch = pAd->CommonCfg.Channel;
			}

			pAd->CommonCfg.Channel = Channel; //lk added

#ifdef DOT11_N_SUPPORT
			N_ChannelCheck(pAd);
			
			if (WMODE_CAP_N(pAd->CommonCfg.PhyMode) &&
					pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40)
				RFChannel = N_SetCenCh(pAd, pAd->CommonCfg.Channel);
			else
#endif /* DOT11_N_SUPPORT */
				RFChannel = pAd->CommonCfg.Channel;

			DBGPRINT(RT_DEBUG_TRACE, ("%s(): CtrlChannel(%d), CentralChannel(%d) \n", 
							__FUNCTION__, pAd->CommonCfg.Channel,
							pAd->CommonCfg.CentralChannel));

			if ((pAd->CommonCfg.Channel > 14 )
				&& (pAd->CommonCfg.bIEEE80211H == TRUE))
			{
				if (pAd->Dot11_H.RDMode == RD_SILENCE_MODE)
				{
					AsicSwitchChannel(pAd, RFChannel, FALSE);
					APStop(pAd);
					APStartUp(pAd);
				}
				else
				{
					NotifyChSwAnnToPeerAPs(pAd, ZERO_MAC_ADDR, pAd->CurrentAddress, 1, pAd->CommonCfg.Channel);
					pAd->Dot11_H.RDMode = RD_SWITCHING_MODE;
					pAd->Dot11_H.CSCount = 0;
					pAd->Dot11_H.new_channel = Channel;
				}
			}
			else
			{
				AsicSwitchChannel(pAd, RFChannel, FALSE);
				APStop(pAd);
				APStartUp(pAd);
			}
		}
	}
#endif /* CONFIG_AP_SUPPORT */

	if (Success == TRUE)
		DBGPRINT(RT_DEBUG_TRACE, ("Set_Channel_Proc::(Channel=%d)\n", pAd->CommonCfg.Channel));

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
#ifdef APCLI_AUTO_CONNECT_SUPPORT
			pAd->ApCfg.ApCliAutoConnectChannelSwitching = FALSE;
#endif /* APCLI_AUTO_CONNECT_SUPPORT */
		}
#endif /* CONFIG_AP_SUPPORT */
	return Success;
}


/* 
    ==========================================================================
    Description:
        Set Short Slot Time Enable or Disable
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ShortSlot_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int retval;
	
	retval = RT_CfgSetShortSlot(pAd, arg);
	if (retval == TRUE)
		DBGPRINT(RT_DEBUG_TRACE, ("Set_ShortSlot_Proc::(ShortSlot=%d)\n", pAd->CommonCfg.bUseShortSlotTime));

	return retval;
}


/* 
    ==========================================================================
    Description:
        Set Tx power
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_TxPower_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	LONG TxPower;
	INT   success = FALSE;

	TxPower = simple_strtol(arg, 0, 10);
	if (TxPower <= 100)
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
			pAd->CommonCfg.TxPowerPercentage = TxPower;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			pAd->CommonCfg.TxPowerDefault = TxPower;
			pAd->CommonCfg.TxPowerPercentage = pAd->CommonCfg.TxPowerDefault;
		}
#endif /* CONFIG_STA_SUPPORT */
		success = TRUE;
	}
	else
		success = FALSE;

#ifdef MT_MAC
	if (pAd->chipCap.hif_type == HIF_MT)
	{
		CmdSetTxPowerCtrl(pAd, pAd->hw_cfg.cent_ch);
	}
#endif

	DBGPRINT(RT_DEBUG_TRACE, ("Set_TxPower_Proc::(TxPowerPercentage=%ld)\n", pAd->CommonCfg.TxPowerPercentage));

	return success;
}

/* 
    ==========================================================================
    Description:
        Set 11B/11G Protection
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_BGProtection_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	switch (simple_strtol(arg, 0, 10))
	{
		case 0: /*AUTO*/
			pAd->CommonCfg.UseBGProtection = 0;
			break;
		case 1: /*Always On*/
			pAd->CommonCfg.UseBGProtection = 1;
			break;
		case 2: /*Always OFF*/
			pAd->CommonCfg.UseBGProtection = 2;
			break;		
		default:  /*Invalid argument */
			return FALSE;
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		APUpdateCapabilityAndErpIe(pAd);
#endif /* CONFIG_AP_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_BGProtection_Proc::(BGProtection=%ld)\n", pAd->CommonCfg.UseBGProtection));	

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set TxPreamble
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_TxPreamble_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	RT_802_11_PREAMBLE	Preamble;

	Preamble = (RT_802_11_PREAMBLE)simple_strtol(arg, 0, 10);

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	if (Preamble == Rt802_11PreambleAuto)
		return FALSE;
#endif /* CONFIG_AP_SUPPORT */

	switch (Preamble)
	{
		case Rt802_11PreambleShort:
			pAd->CommonCfg.TxPreamble = Preamble;
#ifdef CONFIG_STA_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
				MlmeSetTxPreamble(pAd, Rt802_11PreambleShort);
#endif /* CONFIG_STA_SUPPORT */
			break;
		case Rt802_11PreambleLong:
#ifdef CONFIG_STA_SUPPORT
		case Rt802_11PreambleAuto:
			/* 
				If user wants AUTO, initialize to LONG here, then change according to AP's
				capability upon association
			*/
#endif /* CONFIG_STA_SUPPORT */
			pAd->CommonCfg.TxPreamble = Preamble;
#ifdef CONFIG_STA_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
				MlmeSetTxPreamble(pAd, Rt802_11PreambleLong);
#endif /* CONFIG_STA_SUPPORT */
			break;
		default: /*Invalid argument */
			return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_TxPreamble_Proc::(TxPreamble=%ld)\n", pAd->CommonCfg.TxPreamble));

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set RTS Threshold
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_RTSThreshold_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	 NDIS_802_11_RTS_THRESHOLD           RtsThresh;

	RtsThresh = simple_strtol(arg, 0, 10);

	if((RtsThresh > 0) && (RtsThresh <= MAX_RTS_THRESHOLD))
		pAd->CommonCfg.RtsThreshold  = (USHORT)RtsThresh;
#ifdef CONFIG_STA_SUPPORT
	else if (RtsThresh == 0)
		pAd->CommonCfg.RtsThreshold = MAX_RTS_THRESHOLD;
#endif /* CONFIG_STA_SUPPORT */
	else
		return FALSE; /*Invalid argument */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_RTSThreshold_Proc::(RTSThreshold=%d)\n", pAd->CommonCfg.RtsThreshold));

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set Fragment Threshold
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_FragThreshold_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	 NDIS_802_11_FRAGMENTATION_THRESHOLD     FragThresh;

	FragThresh = simple_strtol(arg, 0, 10);

	if (FragThresh > MAX_FRAG_THRESHOLD || FragThresh < MIN_FRAG_THRESHOLD)
	{ 
		/*Illegal FragThresh so we set it to default*/
		pAd->CommonCfg.FragmentThreshold = MAX_FRAG_THRESHOLD;
	}
	else if (FragThresh % 2 == 1)
	{
		/*
			The length of each fragment shall always be an even number of octets, 
			except for the last fragment of an MSDU or MMPDU, which may be either 
			an even or an odd number of octets.
		*/
		pAd->CommonCfg.FragmentThreshold = (USHORT)(FragThresh - 1);
	}
	else
	{
		pAd->CommonCfg.FragmentThreshold = (USHORT)FragThresh;
	}

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->CommonCfg.FragmentThreshold == MAX_FRAG_THRESHOLD)
			pAd->CommonCfg.bUseZeroToDisableFragment = TRUE;
		else
			pAd->CommonCfg.bUseZeroToDisableFragment = FALSE;
	}
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_FragThreshold_Proc::(FragThreshold=%d)\n", pAd->CommonCfg.FragmentThreshold));

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set TxBurst
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_TxBurst_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	LONG TxBurst;

	TxBurst = simple_strtol(arg, 0, 10);
	if (TxBurst == 1)
		pAd->CommonCfg.bEnableTxBurst = TRUE;
	else if (TxBurst == 0)
		pAd->CommonCfg.bEnableTxBurst = FALSE;
	else
		return FALSE;  /*Invalid argument */
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_TxBurst_Proc::(TxBurst=%d)\n", pAd->CommonCfg.bEnableTxBurst));

	return TRUE;
}


#ifdef RTMP_MAC_PCI
INT Set_ShowRF_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int ShowRF = simple_strtol(arg, 0, 10);
	
	if (ShowRF == 1)
		pAd->ShowRf = TRUE;
	else
		pAd->ShowRf = FALSE;
	
	return TRUE;
}
#endif /* RTMP_MAC_PCI */


#ifdef AGGREGATION_SUPPORT
/* 
    ==========================================================================
    Description:
        Set TxBurst
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_PktAggregate_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	LONG aggre;

	aggre = simple_strtol(arg, 0, 10);

	if (aggre == 1)
		pAd->CommonCfg.bAggregationCapable = TRUE;
	else if (aggre == 0)
		pAd->CommonCfg.bAggregationCapable = FALSE;
	else
		return FALSE;  /*Invalid argument */

#ifdef CONFIG_AP_SUPPORT
#ifdef PIGGYBACK_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		pAd->CommonCfg.bPiggyBackCapable = pAd->CommonCfg.bAggregationCapable;
		RTMPSetPiggyBack(pAd, pAd->CommonCfg.bPiggyBackCapable);
	}
#endif /* PIGGYBACK_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_PktAggregate_Proc::(AGGRE=%d)\n", pAd->CommonCfg.bAggregationCapable));

	return TRUE;
}
#endif


#ifdef INF_PPA_SUPPORT
INT	Set_INF_AMAZON_SE_PPA_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	UINT status;
	UCHAR aggre;
	UINT32 g_if_id;
	NDIS_STATUS	re_val;
	
	aggre = simple_strtol(arg, 0, 10);

	if (ppa_hook_directpath_register_dev_fn == NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s::There is no PPA module\n", __FUNCTION__));
		return FALSE;
	}	

	if (aggre == 1)
	{
		if (pAd->PPAEnable == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("PPA already enabled\n"));
		}
		else
		{
			if (pAd->pDirectpathCb == NULL) 
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Allocate memory for pDirectpathCb\n"));
				re_val = os_alloc_mem(NULL, (UCHAR **)&(pAd->pDirectpathCb), sizeof(PPA_DIRECTPATH_CB));

				if (re_val != NDIS_STATUS_SUCCESS)
					return FALSE;
			}

			/* Register callback */
			pAd->pDirectpathCb->rx_fn = ifx_ra_start_xmit;
			pAd->pDirectpathCb->stop_tx_fn = NULL;
			pAd->pDirectpathCb->start_tx_fn = NULL;

			status = ppa_hook_directpath_register_dev_fn(
						&g_if_id, pAd->net_dev, pAd->pDirectpathCb, PPA_F_DIRECTPATH_REGISTER|PPA_F_DIRECTPATH_ETH_IF);

			if (status == IFX_SUCCESS)
			{
				pAd->g_if_id = g_if_id;
				pAd->PPAEnable = TRUE;
				DBGPRINT(RT_DEBUG_TRACE, ("Register PPA success::ret=%d, id=%d\n", status, pAd->g_if_id));
			}
			else
				DBGPRINT(RT_DEBUG_TRACE, ("Register PPA fail::ret=%d\n", status));
		}
	}
	else if (aggre == 0)
	{
		if (pAd->PPAEnable == FALSE)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("PPA already disabled\n"));
		}
		else
		{
			g_if_id = pAd->g_if_id;
			status = ppa_hook_directpath_register_dev_fn(&g_if_id, pAd->net_dev, NULL, 0 /*PPA_F_DIRECTPATH_REGISTER*/);

			if (status == IFX_SUCCESS)
			{
				pAd->g_if_id = 0;
				pAd->PPAEnable = FALSE;
				DBGPRINT(RT_DEBUG_TRACE, ("Unregister PPA success::ret=%d, if_id=%d\n", status, pAd->g_if_id));
			}
			else
				DBGPRINT(RT_DEBUG_TRACE, ("Unregister PPA fail::ret=%d\n", status));
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s::Invalid argument=%d\n", __FUNCTION__, aggre));
		return FALSE;
	}	

	return TRUE;
}
#endif /* INF_PPA_SUPPORT */


/* 
    ==========================================================================
    Description:
        Set IEEE80211H.
        This parameter is 1 when needs radar detection, otherwise 0
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_IEEE80211H_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    LONG ieee80211h;

	ieee80211h = simple_strtol(arg, 0, 10);

	if (ieee80211h == 1)
		pAd->CommonCfg.bIEEE80211H = TRUE;
	else if (ieee80211h == 0)
		pAd->CommonCfg.bIEEE80211H = FALSE;
	else
		return FALSE;  /*Invalid argument */
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_IEEE80211H_Proc::(IEEE80211H=%d)\n", pAd->CommonCfg.bIEEE80211H));

	return TRUE;
}

#ifdef EXT_BUILD_CHANNEL_LIST
/* 
    ==========================================================================
    Description:
        Set Country Code.
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT Set_ExtCountryCode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s can only be used when interface is down.\n", __FUNCTION__));
		return TRUE;
	}
	
	if(strlen(arg) == 2)
	{
		NdisMoveMemory(pAd->CommonCfg.CountryCode, arg, 2);
		pAd->CommonCfg.bCountryFlag = TRUE;
	}
	else
	{
		NdisZeroMemory(pAd->CommonCfg.CountryCode, sizeof(pAd->CommonCfg.CountryCode));
		pAd->CommonCfg.bCountryFlag = FALSE;
	}	

	{	
		UCHAR CountryCode[3] = {0};
		NdisMoveMemory(CountryCode, pAd->CommonCfg.CountryCode, 2);
		DBGPRINT(RT_DEBUG_TRACE, ("Set_CountryCode_Proc::(bCountryFlag=%d, CountryCode=%s)\n",
							pAd->CommonCfg.bCountryFlag,
							CountryCode));
	}
	return TRUE;
}
/* 
    ==========================================================================
    Description:
        Set Ext DFS Type
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT Set_ExtDfsType_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR	*pDfsType = &pAd->CommonCfg.DfsType;
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s can only be used when interface is down.\n", __FUNCTION__));
		return TRUE;
	}

	if (!strcmp(arg, "CE"))
		*pDfsType = CE;
	else if (!strcmp(arg, "FCC"))
		*pDfsType = FCC;
	else if (!strcmp(arg, "JAP"))
		*pDfsType = JAP;
	else
		DBGPRINT(RT_DEBUG_TRACE, ("Unsupported DFS type:%s (Legal types are: CE, FCC, JAP)\n", arg));

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Add new channel list
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT Set_ChannelListAdd_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CH_DESP		inChDesp;
	PCH_REGION pChRegion = NULL;
	
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s can only be used when interface is down.\n", __FUNCTION__));
		return TRUE;
	}

	/* Get Channel Region (CountryCode)*/
	{
		INT loop = 0;

		while (strcmp((RTMP_STRING *) ChRegion[loop].CountReg, "") != 0)
		{
			if (strncmp((RTMP_STRING *) ChRegion[loop].CountReg, pAd->CommonCfg.CountryCode, 2) == 0)
			{
				pChRegion = &ChRegion[loop];
				break;
			}
			loop++;
		}
		if (pChRegion == NULL)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("CountryCode is not configured or not valid\n"));
			return TRUE;
		}
	}

	/* Parsing the arg, IN:arg; OUT:inChRegion */
	{
		UCHAR strBuff[64], count = 0;
		PUCHAR	pStart, pEnd, tempIdx, tempBuff[5];
		
		if (strlen(arg) <64)
			NdisCopyMemory(strBuff, arg, strlen(arg));

		if ((pStart = rtstrchr(strBuff, '[')) != NULL)
		{
			if ((pEnd = rtstrchr(pStart++, ']')) != NULL)
			{
				tempBuff[count++] = pStart;
				for(tempIdx = pStart ;tempIdx != pEnd; tempIdx++)
				{
					if(*tempIdx == ',')
					{
						*tempIdx = '\0';
						tempBuff[count++] = ++tempIdx;
					}
				}
				*(pEnd) = '\0';

				if (count != 5)
				{
					DBGPRINT(RT_DEBUG_TRACE, ("Input Error. Too more or too less parameters.\n"));
					return TRUE;
				}
				else
				{
					inChDesp.FirstChannel = (UCHAR) simple_strtol(tempBuff[0], 0, 10);
					inChDesp.NumOfCh = (UCHAR) simple_strtol(tempBuff[1], 0, 10);
					inChDesp.MaxTxPwr = (UCHAR) simple_strtol(tempBuff[2], 0, 10);
					inChDesp.Geography = (!strcmp(tempBuff[3], "BOTH") ? BOTH: (!strcmp(tempBuff[3], "IDOR") ? IDOR : ODOR));
					inChDesp.DfsReq= (!strcmp(tempBuff[4], "TRUE") ? TRUE : FALSE);
				} 
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Missing End \"]\"\n"));
				return TRUE;
			}
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: Invalid input format.\n", __FUNCTION__));
			return TRUE;
		}
	}

	/* Add entry to Channel List*/
	{
		UCHAR EntryIdx;
		PCH_DESP pChDesp = NULL;
		UCHAR CountryCode[3] = {0};
		if (pAd->CommonCfg.pChDesp == NULL)
		{			
			os_alloc_mem(pAd,  &pAd->CommonCfg.pChDesp, MAX_PRECONFIG_DESP_ENTRY_SIZE*sizeof(CH_DESP));
			pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;
			if (pChDesp)
			{
				for (EntryIdx= 0; pChRegion->pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
				{	
					if (EntryIdx == (MAX_PRECONFIG_DESP_ENTRY_SIZE-2)) /* Keep an NULL entry in the end of table*/
					{
						DBGPRINT(RT_DEBUG_TRACE, ("Table is full.\n"));
						return TRUE;
					}
					NdisCopyMemory(&pChDesp[EntryIdx], &pChRegion->pChDesp[EntryIdx], sizeof(CH_DESP));
				}
				/* Copy the NULL entry*/
				NdisCopyMemory(&pChDesp[EntryIdx], &pChRegion->pChDesp[EntryIdx], sizeof(CH_DESP));
			}
			else
			{
				DBGPRINT(RT_DEBUG_ERROR, ("os_alloc_mem failded.\n"));
				return FALSE;
			}
		}
		else
		{	
			pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;
			for (EntryIdx= 0; pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
			{	
				if(EntryIdx ==  (MAX_PRECONFIG_DESP_ENTRY_SIZE-2)) /* Keep an NULL entry in the end of table*/
				{
					DBGPRINT(RT_DEBUG_TRACE, ("Table is full.\n"));
					return TRUE;
				}
			}
		}				
		NdisMoveMemory(CountryCode, pAd->CommonCfg.CountryCode, 2);
		DBGPRINT(RT_DEBUG_TRACE, ("Add channel lists {%u, %u, %u, %s, %s} to %s.\n",
							inChDesp.FirstChannel,
							inChDesp.NumOfCh,
							inChDesp.MaxTxPwr,
							(inChDesp.Geography == BOTH) ? "BOTH" : (inChDesp.Geography == IDOR) ?  "IDOR" : "ODOR",
							(inChDesp.DfsReq == TRUE) ? "TRUE" : "FALSE",
							CountryCode));
		NdisCopyMemory(&pChDesp[EntryIdx], &inChDesp, sizeof(CH_DESP));
		pChDesp[++EntryIdx].FirstChannel = 0; 
	}
	return TRUE;
}

INT Set_ChannelListShow_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PCH_REGION	pChRegion = NULL;
	UCHAR		EntryIdx, CountryCode[3]={0};
	
	/* Get Channel Region (CountryCode)*/
	{
		INT loop = 0;

		while (strcmp((RTMP_STRING *) ChRegion[loop].CountReg, "") != 0)
		{
			if (strncmp((RTMP_STRING *) ChRegion[loop].CountReg, pAd->CommonCfg.CountryCode, 2) == 0)
			{
				pChRegion = &ChRegion[loop];
				break;
			}
			loop++;
		}
		if (pChRegion == NULL)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("CountryCode is not configured or not valid\n"));
			return TRUE;
		}
	}

	NdisMoveMemory(CountryCode, pAd->CommonCfg.CountryCode, 2);
	if (pAd->CommonCfg.DfsType == MAX_RD_REGION)
		pAd->CommonCfg.DfsType = pChRegion->DfsType;
	DBGPRINT(RT_DEBUG_ERROR, ("=========================================\n"));
	DBGPRINT(RT_DEBUG_ERROR, ("CountryCode:%s\n", CountryCode));
	DBGPRINT(RT_DEBUG_ERROR, ("DfsType:%s\n",
					(pAd->CommonCfg.DfsType == JAP) ? "JAP" :
					((pAd->CommonCfg.DfsType == FCC) ? "FCC" : "CE" )));
					
	if (pAd->CommonCfg.pChDesp != NULL)
	{
		PCH_DESP pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;
		for (EntryIdx = 0; pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%u. {%3u, %2u, %2u, %s, %5s}.\n",
						EntryIdx,
						pChDesp[EntryIdx].FirstChannel,
						pChDesp[EntryIdx].NumOfCh,
						pChDesp[EntryIdx].MaxTxPwr,
						(pChDesp[EntryIdx].Geography == BOTH) ? "BOTH" : (pChDesp[EntryIdx].Geography == IDOR) ?  "IDOR" : "ODOR",
						(pChDesp[EntryIdx].DfsReq == TRUE) ? "TRUE" : "FALSE"));
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Default channel list table:\n"));
		for (EntryIdx = 0; pChRegion->pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%u. {%3u, %2u, %2u, %s, %5s}.\n",
						EntryIdx,
						pChRegion->pChDesp[EntryIdx].FirstChannel,
						pChRegion->pChDesp[EntryIdx].NumOfCh,
						pChRegion->pChDesp[EntryIdx].MaxTxPwr,
						(pChRegion->pChDesp[EntryIdx].Geography == BOTH) ? "BOTH" : (pChRegion->pChDesp[EntryIdx].Geography == IDOR) ?  "IDOR" : "ODOR",
						(pChRegion->pChDesp[EntryIdx].DfsReq == TRUE) ? "TRUE" : "FALSE"));
		}	
	}
	DBGPRINT(RT_DEBUG_ERROR, ("=========================================\n"));
	return TRUE;
}

INT Set_ChannelListDel_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR EntryIdx, TargetIdx, NumOfEntry;
	PCH_REGION	pChRegion = NULL;
	PCH_DESP pChDesp = NULL; 
	TargetIdx = simple_strtol(arg, 0, 10);
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s can only be used when interface is down.\n", __FUNCTION__));
		return TRUE;
	}
	
	/* Get Channel Region (CountryCode)*/
	{
		INT loop = 0;
		while (strcmp((RTMP_STRING *) ChRegion[loop].CountReg, "") != 0)
		{
			if (strncmp((RTMP_STRING *) ChRegion[loop].CountReg, pAd->CommonCfg.CountryCode, 2) == 0)
			{
				pChRegion = &ChRegion[loop];
				break;
			}
			loop++;
		}
		if (pChRegion == NULL)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("CountryCode is not configured or not valid\n"));
			return TRUE;
		}
	}

	if (pAd->CommonCfg.pChDesp == NULL)
	{
		os_alloc_mem(pAd,  &pAd->CommonCfg.pChDesp, MAX_PRECONFIG_DESP_ENTRY_SIZE*sizeof(CH_DESP));
		if (pAd->CommonCfg.pChDesp)
		{
			pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;
			for (EntryIdx= 0; pChRegion->pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
			{	
				if (EntryIdx == (MAX_PRECONFIG_DESP_ENTRY_SIZE-2)) /* Keep an NULL entry in the end of table*/
				{
					DBGPRINT(RT_DEBUG_TRACE, ("Table is full.\n"));
					return TRUE;
				}
				NdisCopyMemory(&pChDesp[EntryIdx], &pChRegion->pChDesp[EntryIdx], sizeof(CH_DESP));
			}
			/* Copy the NULL entry*/
			NdisCopyMemory(&pChDesp[EntryIdx], &pChRegion->pChDesp[EntryIdx], sizeof(CH_DESP));
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("os_alloc_mem failded.\n"));
			return FALSE;
		}
	}
	else
		pChDesp = (PCH_DESP) pAd->CommonCfg.pChDesp;
		
	if (!strcmp(arg, "default"))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Default table used.\n" ));
		if (pAd->CommonCfg.pChDesp != NULL)
			os_free_mem(NULL, pAd->CommonCfg.pChDesp);
		pAd->CommonCfg.pChDesp = NULL;
		pAd->CommonCfg.DfsType = MAX_RD_REGION;
	}
	else if (!strcmp(arg, "all"))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Remove all entries.\n" ));
		for (EntryIdx = 0; EntryIdx < MAX_PRECONFIG_DESP_ENTRY_SIZE; EntryIdx++)
			NdisZeroMemory(&pChDesp[EntryIdx], sizeof(CH_DESP));
	}
	else if (TargetIdx < (MAX_PRECONFIG_DESP_ENTRY_SIZE-1))
	{
		for (EntryIdx= 0; pChDesp[EntryIdx].FirstChannel != 0; EntryIdx++)
		{	
			if(EntryIdx ==  (MAX_PRECONFIG_DESP_ENTRY_SIZE-2)) /* Keep an NULL entry in the end of table */
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Last entry should be NULL.\n"));
				pChDesp[EntryIdx].FirstChannel = 0;
				return TRUE;
			}
		}
		NumOfEntry = EntryIdx;
		if (TargetIdx >= NumOfEntry)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Out of table range.\n"));
			return TRUE;
		}
		for (EntryIdx = TargetIdx; EntryIdx < NumOfEntry; EntryIdx++)
			NdisCopyMemory(&pChDesp[EntryIdx], &pChDesp[EntryIdx+1], sizeof(CH_DESP));
		NdisZeroMemory(&pChDesp[EntryIdx], sizeof(CH_DESP)); /*NULL entry*/
		DBGPRINT(RT_DEBUG_TRACE, ("Entry %u deleted.\n", TargetIdx));
	}
	else
		DBGPRINT(RT_DEBUG_TRACE, ("Entry not found.\n"));
	
	return TRUE;
}
#endif /* EXT_BUILD_CHANNEL_LIST  */

#ifdef WSC_INCLUDED
INT	Set_WscGenPinCode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    PWSC_CTRL   pWscControl = NULL;
    POS_COOKIE  pObj = (POS_COOKIE) pAd->OS_Cookie;
    UCHAR	    apidx = pObj->ioctl_if;
    
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef APCLI_SUPPORT
	    if (pObj->ioctl_if_type == INT_APCLI)
	    {
	        pWscControl = &pAd->ApCfg.ApCliTab[apidx].WscControl;
	        DBGPRINT(RT_DEBUG_TRACE, ("IF(apcli%d) Set_WscGenPinCode_Proc:: This command is from apcli interface now.\n", apidx));
	    }
	    else
#endif /* APCLI_SUPPORT */
	    {
			pWscControl = &pAd->ApCfg.MBSSID[apidx].WscControl;
	        DBGPRINT(RT_DEBUG_TRACE, ("IF(ra%d) Set_WscGenPinCode_Proc:: This command is from ra interface now.\n", apidx));
	    }
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef P2P_SUPPORT
	if (pObj->ioctl_if_type == INT_P2P)
	{
			pWscControl = &pAd->ApCfg.ApCliTab[apidx].WscControl;
			DBGPRINT(RT_DEBUG_TRACE, ("IF(p2p%d) Set_WscGenPinCode_Proc:: This command is from apcli interface now.\n", apidx));
		}
#endif /* P2P_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef P2P_SUPPORT
		if (pObj->ioctl_if_type != INT_P2P)
#endif /* P2P_SUPPORT */
{
    	pWscControl = &pAd->StaCfg.WscControl;
	}
	}
#endif /* CONFIG_STA_SUPPORT */

	if (pWscControl == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: pWscControl == NULL!\n", __FUNCTION__));
		return TRUE;
	}

	if (pWscControl->WscEnrollee4digitPinCode)	
	{
		pWscControl->WscEnrolleePinCodeLen = 4;
		pWscControl->WscEnrolleePinCode = WscRandomGen4digitPinCode(pAd);
	}
	else
	{
		pWscControl->WscEnrolleePinCodeLen = 8;
#ifdef P2P_SSUPPORT
#ifdef WIDI_SUPPORT
        if (pObj->ioctl_if_type == INT_P2P)
		{
            pWscControl->WscEnrolleePinCode = WscRandomGenerateP2PPinCode(pAd, apidx);
        }else
#endif /* WIDI_SUPPORT */		
#endif /* P2P_SUPPORT */
		pWscControl->WscEnrolleePinCode = WscRandomGeneratePinCode(pAd, apidx);
	}

#ifdef P2P_SUPPORT
	if (pObj->ioctl_if_type == INT_P2P)	
	{
		PWSC_CTRL   pApWscControl = &pAd->ApCfg.MBSSID[apidx].WscControl;
		pApWscControl->WscEnrolleePinCodeLen  = pWscControl->WscEnrolleePinCodeLen;
		pApWscControl->WscEnrolleePinCode  = pWscControl->WscEnrolleePinCode;
	}
#endif /* P2P_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_WscGenPinCode_Proc:: Enrollee PinCode\t\t%08u\n", pWscControl->WscEnrolleePinCode));

	return TRUE;
}

#ifdef BB_SOC
INT	Set_WscResetPinCode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PWSC_CTRL   pWscControl = NULL;
	POS_COOKIE  pObj = (POS_COOKIE) pAd->OS_Cookie;
	UCHAR	    apidx = pObj->ioctl_if;
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		{
			pWscControl = &pAd->ApCfg.MBSSID[apidx].WscControl;
			DBGPRINT(RT_DEBUG_TRACE, ("IF(ra%d) Set_WscResetPinCode_Proc:: This command is from ra interface now.\n", apidx));
		}

		pWscControl->WscEnrolleePinCode = GenerateWpsPinCode(pAd, 0, apidx);
	}
#endif // CONFIG_AP_SUPPORT //

		DBGPRINT(RT_DEBUG_TRACE, ("Set_WscResetPinCode_Proc:: Enrollee PinCode\t\t%08u\n", pWscControl->WscEnrolleePinCode));

		return TRUE;
}
#endif

INT Set_WscVendorPinCode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PWSC_CTRL   pWscControl = NULL;
	POS_COOKIE  pObj = (POS_COOKIE) pAd->OS_Cookie;
	UCHAR       apidx = pObj->ioctl_if;

#ifdef CONFIG_AP_SUPPORT
/*	POS_COOKIE  pObj = (POS_COOKIE) pAd->OS_Cookie;*/
/*	UCHAR       apidx = pObj->ioctl_if;*/

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef APCLI_SUPPORT
		if (pObj->ioctl_if_type == INT_APCLI)
		{
			pWscControl = &pAd->ApCfg.ApCliTab[apidx].WscControl;
			DBGPRINT(RT_DEBUG_TRACE, ("Set_WscVendorPinCode_Proc() for apcli(%d)\n", apidx));
		}
		else
#endif /* APCLI_SUPPORT */
		{
			pWscControl = &pAd->ApCfg.MBSSID[apidx].WscControl;
			DBGPRINT(RT_DEBUG_TRACE, ("Set_WscVendorPinCode_Proc() for ra%d!\n", apidx));
		}
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef P2P_SUPPORT
	if (pObj->ioctl_if_type == INT_P2P)
	{
		pWscControl = &pAd->ApCfg.MBSSID[apidx].WscControl;
		RT_CfgSetWscPinCode(pAd, arg, pWscControl);

		pWscControl = &pAd->ApCfg.ApCliTab[apidx].WscControl;
		RT_CfgSetWscPinCode(pAd, arg, pWscControl);

		DBGPRINT(RT_DEBUG_TRACE, ("Set_WscVendorPinCode_Proc() for p2p(%d)\n", apidx));
		return TRUE;
	}
#endif /* P2P_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
 	{
#ifdef P2P_SUPPORT
		if (pObj->ioctl_if_type != INT_P2P)
#endif /* P2P_SUPPORT */
 		pWscControl = &pAd->StaCfg.WscControl;
 	}
#endif /* CONFIG_STA_SUPPORT */

	if (!pWscControl)
		return FALSE;
	else
	return RT_CfgSetWscPinCode(pAd, arg, pWscControl);
}
#endif /* WSC_INCLUDED */


#ifdef DBG
INT rx_temp_dbg = 0;

/* 
    ==========================================================================
    Description:
        For Debug information
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT Set_Debug_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG dbg;

	DBGPRINT_S(("==>%s()\n", __FUNCTION__));

	dbg = simple_strtol(arg, 0, 10);
	if( dbg <= RT_DEBUG_MAX)
		RTDebugLevel = dbg;

	DBGPRINT_S(("<==%s(RTDebugLevel = %ld)\n", __FUNCTION__, RTDebugLevel));

	return TRUE;
}


/* 
    ==========================================================================
    Description:
        For DebugFunc information
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_DebugFunc_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG func;
	DBGPRINT_S(("==>%s()\n", __FUNCTION__));

	func = simple_strtol(arg, 0, 10);
	if (func <= 0xffffff)
		func <<= 8;
	RTDebugFunc = func;
	DBGPRINT_S(("Set RTDebugFunc = 0x%lx\n", RTDebugFunc));

	return TRUE;
}


static BOOLEAN ascii2hex(RTMP_STRING *in, UINT32 *out)
{
	UINT32 hex_val, val;
	CHAR *p, asc_val;

	hex_val = 0;
	p = (char *)in;
	while((*p) != 0)
	{
		val = 0;
		asc_val = *p;
		if ((asc_val >= 'a') && (asc_val <= 'f'))
			val = asc_val - 87;
		else if ((*p >= 'A') && (asc_val <= 'F'))
			val = asc_val - 55;
		else if ((asc_val >= '0') && (asc_val <= '9'))
			val = asc_val - 48;
		else
			return FALSE;

		hex_val = (hex_val << 4) + val;
		p++;
	}
	*out = hex_val;

	return TRUE;
}


/* 
    ==========================================================================
    Description:
        Read / Write MAC
    Arguments:
        pAd                    Pointer to our adapter
        wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
        Usage: 
               1.) iwpriv ra0 mac 0        ==> read MAC where Addr=0x0
               2.) iwpriv ra0 mac 0=12     ==> write MAC where Addr=0x0, value=12
    ==========================================================================
*/
VOID RTMPIoctlMAC(RTMP_ADAPTER *pAd, RTMP_IOCTL_INPUT_STRUCT *wrq)
{
	RTMP_STRING *seg_str, *addr_str, *val_str, *range_str;
	RTMP_STRING *mpool, *msg;
	RTMP_STRING *arg, *ptr;
	UINT32 macAddr, macVal = 0;
	UINT32 macValue;
	BOOLEAN bIsPrintAllMAC = FALSE, bFromUI, is_write, is_range;
	UINT32 IdMac, mac_s = 0x1000, mac_e = 0x1700, mac_range = 0xffff;
	ULONG status=0; 

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("==>RTMPIoctlMAC\n"));
#endif /* RELEASE_EXCLUDE */

	os_alloc_mem(NULL, (UCHAR **)&mpool, sizeof(CHAR)*(4096+256+12));
	if (!mpool)
		return;
	

#if defined(MT7603) || defined(MT7628) || defined(MT7636)
	if (IS_MT7603(pAd) || IS_MT7628(pAd) || IS_MT7636(pAd))
		mac_range = 0xcffff;
#endif /* MT7603 */

	bFromUI = ((wrq->u.data.flags & RTPRIV_IOCTL_FLAG_UI) == RTPRIV_IOCTL_FLAG_UI) ? TRUE : FALSE;
	
	msg = (RTMP_STRING *)((ULONG)(mpool+3) & (ULONG)~0x03);
	arg = (RTMP_STRING *)((ULONG)(msg+4096+3) & (ULONG)~0x03);

	memset(msg, 0x00, 4096);
	memset(arg, 0x00, 256);

	if (wrq->u.data.length > 1) {
#ifdef LINUX
	status = copy_from_user(arg, wrq->u.data.pointer, (wrq->u.data.length > 255) ? 255 : wrq->u.data.length);
#else
		NdisMoveMemory(arg, wrq->u.data.pointer, (wrq->u.data.length > 255) ? 255 : wrq->u.data.length);
#endif /* LINUX */
		arg[255] = 0x00;
	}

	ptr = arg;
	if ((ptr!= NULL) && (strlen(ptr) > 0)) {
		while ((*ptr != 0) && (*ptr == 0x20)) // remove space
			ptr++;
	}
	
	
	DBGPRINT(RT_DEBUG_OFF, ("%s():wrq->u.data.length=%d, pointer(%p)=%s!\n",
				__FUNCTION__, wrq->u.data.length,
				wrq->u.data.pointer, wrq->u.data.pointer));
	if ((ptr == NULL) || strlen(ptr) == 0) {
		bIsPrintAllMAC = TRUE;
		goto print_all;
	}
	DBGPRINT(RT_DEBUG_OFF, ("%s():after trim space, ptr len=%zu, pointer(%p)=%s!\n",
				__FUNCTION__, strlen(ptr), ptr, ptr));



#ifdef VXWORKS 
	if (!(wrq->u.data.length == 4096 && bFromUI))
#endif
	{
		while ((seg_str = strsep((char **)&ptr, ",")) != NULL)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("seg_str[%zu]=%s\n", strlen(seg_str), seg_str));
			is_write = FALSE;
			addr_str = seg_str;
			val_str = NULL;
			if ((val_str = strchr(seg_str, '=')) != NULL) {
				*val_str++ = 0;
				is_write = 1;
			} else {
				is_write = 0;
			}

			if (addr_str) {
				if ((range_str = strchr(addr_str, '-')) != NULL) {
					*range_str++ = 0;
					is_range = 1;
				} else {
					is_range = 0;
				}

				if ((ascii2hex(addr_str, &mac_s) == FALSE)) {
					DBGPRINT(RT_DEBUG_ERROR, ("Invalid MAC CR Addr, str=%s\n", addr_str));
					break;
				}

#if defined(MT7603) || defined(MT7628) || defined(MT7636)
				if (IS_MT7603(pAd) || IS_MT7628(pAd) || IS_MT7636(pAd))
					mt_mac_cr_range_mapping(&mac_s);
#endif				
				if (mac_s >= mac_range) {
					DBGPRINT(RT_DEBUG_ERROR, ("MAC CR Addr[0x%x] out of range[0x%x], str=%s\n",
								mac_s, mac_range, addr_str));
					break;
				}
				
				if (is_range) {
					if (ascii2hex(range_str, &mac_e) == FALSE) {
						DBGPRINT(RT_DEBUG_ERROR, ("Invalid Range End MAC CR Addr[0x%x], str=%s\n",
									mac_e, range_str));
						break;
					}

#if defined(MT7603) || defined(MT7628) || defined(MT7636)
					if (IS_MT7603(pAd) || IS_MT7628(pAd) || IS_MT7636(pAd))
						mt_mac_cr_range_mapping(&mac_e);
#endif
					if (mac_e >= mac_range) {
						DBGPRINT(RT_DEBUG_ERROR, ("MAC CR Addr[0x%x] out of range[0x%x], str=%s\n",
									mac_e, mac_range, range_str));
						break;
					}
					
					if (mac_e < mac_s) {
						DBGPRINT(RT_DEBUG_ERROR, ("Invalid Range MAC Addr[%s - %s] => [0x%x - 0x%x]\n", 
									addr_str, range_str, mac_s, mac_e));
						break;
					}
				} else {
					mac_e = mac_s;
				}
			}
			
			if (val_str) {
				if ((strlen(val_str) == 0) || ascii2hex(val_str, &macVal) == FALSE) {
					DBGPRINT(RT_DEBUG_ERROR, ("Invalid MAC value[0x%s]\n", val_str));
					break;
				}
			}
		
			if (is_write) {
				RTMP_IO_WRITE32(pAd, mac_s, macVal);
				snprintf(msg+strlen(msg), 4096-strlen(msg)
						, "[0x%04x]:%08x  ", mac_s, macVal);
				if (!bFromUI)
					DBGPRINT(RT_DEBUG_INFO, ("MacAddr=0x%x, MacValue=0x%x\n", mac_s, macVal));
			} else {
				for(IdMac = mac_s; IdMac <= mac_e; IdMac += 4)
				{
					RTMP_IO_READ32(pAd, IdMac, &macVal);
					snprintf(msg+strlen(msg), 4096-strlen(msg)
							, "[0x%04x]:%08x  ", IdMac, macVal);
					if (!bFromUI)
						DBGPRINT(RT_DEBUG_TRACE, ("MacAddr=0x%x, MacValue=0x%x\n", IdMac, macVal));
				}
			}

#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("msg=%s\n", msg));
#endif /* RELEASE_EXCLUDE */
			
			if (ptr)
				DBGPRINT(RT_DEBUG_TRACE, ("NextRound: ptr[%zu]=%s\n", strlen(ptr), ptr));
		}

	}

print_all:
	if (bIsPrintAllMAC)
	{
		int mac_idx = 0;

		mac_s = 0x1000;
		mac_e = 0x1700;
#if defined(RT65xx) || defined (MT7601)
		if (IS_RT65XX(pAd) || IS_MT7601(pAd))
			mac_s = 0x0;
#endif /* defined(RT65xx) || defined (MT7601) */

#if defined(MT7603) || defined(MT7628) || defined(MT7636)
		if (IS_MT7603(pAd) || IS_MT7628(pAd) || IS_MT7636(pAd)) {
			static UINT32 mac_range_7603[]={
				0x20000, 0x201ff, /* WF_CFG: 0x6000_0000 ~ 0x6000_01FF */
				WF_TRB_BASE, 0x211ff, /* WF_TRB: 0x6010_0000 ~ 0x6010_01FF */
				WF_AGG_BASE, 0x213ff, /* WF_AGG: 0x6011_0000 ~ 0x6011_01FF */
				WF_ARB_BASE, 0x214ff, /* WF_ARB: 0x6012_0000 ~ 0x6012_01FF */
				WF_TMAC_BASE, 0x216ff, /* WF_TMAC: 0x6013_0000 ~ 0x6013_01FF */
				WF_RMAC_BASE, 0x21908, /* WF_RMAC: 0x6014_0000 ~ 0x6014_01FF */
				WF_SEC_BASE, 0x21A04, /* WF_SEC: 0x6015_0000 ~ 0x6015_01FF */
				WF_DMA_BASE, 0x21C88, /* WF_DMA: 0x6016_0000 ~ 0x6016_0088 */
				//0x21E00, 0x1740, /* WF_CFGOFF: 0x6015_0000 ~ 0x6015_01FF */
				//0x22000, 0x1740, /* WF_PF: 0x6015_0000 ~ 0x6015_01FF */
				//0x23000, 0x1740, /* WF_WTBLOFF: 0x6015_0000 ~ 0x6015_01FF */
				//0x23200, 0x1740, /* WF_ETBF: 0x6015_0000 ~ 0x6015_01FF */
				//0x24000, 0x1740, /* WF_LPON: 0x6015_0000 ~ 0x6015_01FF */
				//0x24400, 0x244FF, /* WF_INT: 0x6015_0000 ~ 0x6015_01FF */
				//0x28000, 0x280ff, /* WF_WTBLON: 0x6015_0000 ~ 0x6015_01FF */
				0x2C000, 0x2C1FF, /* WF_MIB: 0x6015_0000 ~ 0x6015_01FF */
				0x2D000, 0x2D1FF, /* WF_AON: 0x6015_0000 ~ 0x6015_01FF */
				0,0
			};

			snprintf(msg, 4096, "\n");
			mac_idx = 0;
			while (mac_range_7603[mac_idx] != 0)
			{
				mac_s = mac_range_7603[mac_idx];
				mac_e = mac_range_7603[mac_idx + 1];
				for (macAddr = mac_s; macAddr <= mac_e; macAddr += 4) {
					RTMP_IO_READ32(pAd, macAddr, &macValue);
					DBGPRINT(RT_DEBUG_TRACE, ("[%08x] = %08x\n", macAddr, macValue));
				}
				mac_idx += 2;
			}
		}
		else
#endif /* MT7603 */
#if defined(RT65xx) || defined(MT7601)
if (IS_RT65XX(pAd))
{
		UINT32 *pBufMac = NULL, *pBuf;
		UINT32 AddrStart = 0x1000, AddrEnd = 0x1800;
		UINT32 IdAddr;

#if defined(RT65xx) || defined(MT7601)
			if (IS_RT65XX(pAd)) {
				AddrStart = 0x0; AddrEnd = 0x800;
			}
#endif /* defined(RT65xx) || defined(MT7601) */

		ASSERT((AddrEnd >= AddrStart));

		/* *2 for safe */
		os_alloc_mem(NULL, (UCHAR **)&pBufMac, (AddrEnd - AddrStart)*2);
		if (pBufMac != NULL)
		{
			pBuf = pBufMac;
			for(IdAddr=AddrStart; IdAddr<=AddrEnd; IdAddr+=4, pBuf++)
				RTMP_IO_READ32(pAd, IdAddr, pBuf);
			RtmpDrvAllMacPrint(pAd, pBufMac, AddrStart, AddrEnd, 4);

			os_free_mem(NULL, pBufMac);
		}
} else 
#endif /* defined(RT65xx) || defined(MT7601) */
		{
			for(IdMac = mac_s; IdMac < mac_e; IdMac += 4)
			{
				RTMP_IO_READ32(pAd, IdMac, &macValue);
				DBGPRINT(RT_DEBUG_TRACE, ("%08x = %08x\n", IdMac, macValue));
			}
		}
	}

#ifdef VXWORKS
#ifdef RALINK_3052
	if (bIsPrintAllMAC) {
		int mac_idx = 0;
		UINT32 mac_s, mac_e;
		static UINT32 mac_range_3052[]={
			0x1000, 0x1134,
			0x1200, 0x1208,
			0x1300, 0x1380,
			0x1400, 0x1418,
			0x1500, 0x1508,
			0x1600, 0x1614,
			0x1700, 0x1740,
			0,0
		};

		snprintf(msg, 4096, "\n");
		while (mac_range_3052[mac_idx] != 0)
		{
			mac_s = mac_range_3052[mac_idx];
			mac_e = mac_range_3052[mac_idx + 1];
			for (macAddr = mac_s; macAddr <= mac_e; macAddr += 4) {
				RTMP_IO_READ32(pAd, macAddr, &macValue);
				snprintf(msg+strlen(msg), 4096-strlen(msg),
					"[0x%04x]:%08x\n", macAddr, macValue);
			}
			mac_idx += 2;
		}
	}
#endif /* RALINK_3052 */
#ifdef RT2883 
	if (bIsPrintAllMAC)
	{
		int mac_idx = 0;
		UINT32 mac_s, mac_e;
		static UINT32 mac_range_2883[]={
			0x1000, 0x1068,
			0x1100, 0x113c,
			0x1300, 0x13D0,
			0x1400, 0x1418,
			0x1600, 0x1614,
			0x1700, 0x1748,
			0,0
		};

		while (mac_range_2883[mac_idx] != 0)
		{
			mac_s = mac_range_2883[mac_idx];
			mac_e = mac_range_2883[mac_idx + 1];
			for (macAddr = mac_s; macAddr <= mac_e; macAddr += 4) {
				RTMP_IO_READ32(pAd, macAddr, &macValue);
				snprintf(msg+strlen(msg), 4096-strlen(msg),
					"[0x%04x]:%08x\n", macAddr, macValue);
			}
			mac_idx += 2;
		}
	}
#endif /* RT2883 */
#endif /* VXWORKS */
	
	if(strlen(msg) == 1)
		snprintf(msg+strlen(msg), 4096-strlen(msg), "===>Error command format!");
	
#ifdef LINUX
	/* Copy the information into the user buffer */
	wrq->u.data.length = (UINT16)strlen(msg);
	if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: copy_to_user() fail\n", __FUNCTION__));			
	}
#endif /* LINUX */

#ifdef VXWORKS
     if (bFromUI && (wrq->u.data.length >= strlen(msg))) {
		wrq->u.data.length = strlen(msg);
		copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);
	} else {
		DBGPRINT(RT_DEBUG_OFF, ("%s\n", msg));	
	}
#endif /* VXWORKS */

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("copy to user [msg=%s]\n", msg));
	DBGPRINT(RT_DEBUG_INFO, ("strlen(msg) =%d\n", (UINT32)strlen(msg)));
#endif /* RELEASE_EXCLUDE */

	os_free_mem(NULL, mpool);
	if (!bFromUI)	
		DBGPRINT(RT_DEBUG_INFO, ("<==%s()\n", __FUNCTION__));
}

#endif /* DBG */


INT	Show_DescInfo_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
#ifdef RTMP_MAC_PCI
	INT32 i, QueIdx;
	RXD_STRUC *pRxD;
	TXD_STRUC *pTxD;
#ifdef RT_BIG_ENDIAN
	RXD_STRUC *pDestRxD, RxD;
	TXD_STRUC *pDestTxD, TxD;	
#endif /* RT_BIG_ENDIAN */
	RTMP_TX_RING *pTxRing;
	RTMP_MGMT_RING *pMgmtRing = &pAd->MgmtRing;	
	RTMP_RX_RING *pRxRing;
	RTMP_BCN_RING *pBcnRing = &pAd->BcnRing;
	RTMP_TX_RING *pTxBmcRing = &pAd->TxBmcRing;
	RTMP_CTRL_RING *pCtrlRing = &pAd->CtrlRing;
	PUCHAR pDMAHeaderBufVA;
	
	for (QueIdx = 0; QueIdx < NUM_OF_TX_RING; QueIdx++)
	{
		pTxRing = &pAd->TxRing[QueIdx];

		DBGPRINT(RT_DEBUG_OFF, ("Tx Ring %d ---------------------------------\n", QueIdx));
		for(i = 0; i < TX_RING_SIZE; i++)
		{	
			pDMAHeaderBufVA = (UCHAR *)pTxRing->Cell[i].DmaBuf.AllocVa;
#ifdef RT_BIG_ENDIAN
			pDestTxD = (TXD_STRUC *)pTxRing->Cell[i].AllocVa;
			TxD = *pDestTxD;
			pTxD = &TxD;
			RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
			MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#else
	    	pTxD = (TXD_STRUC *)pTxRing->Cell[i].AllocVa;
#endif /* RT_BIG_ENDIAN */
	    	DBGPRINT(RT_DEBUG_OFF, ("Desc #%d\n",i));
			dump_txd(pAd, pTxD);
			dump_tmac_info(pAd, pDMAHeaderBufVA);
			DBGPRINT(RT_DEBUG_OFF, ("pkt physical address = %lx\n", 
						(ULONG)pTxRing->Cell[i].PacketPa));
#ifdef RT_BIG_ENDIAN
			MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#endif
		}
	}

	DBGPRINT(RT_DEBUG_OFF, ("Mgmt Ring ------------------------------------------\n"));
	for(i = 0; i < MGMT_RING_SIZE; i++)
	{	
		pDMAHeaderBufVA = (UCHAR *)pMgmtRing->Cell[i].DmaBuf.AllocVa;
#ifdef RT_BIG_ENDIAN
		pDestTxD = (TXD_STRUC *)pMgmtRing->Cell[i].AllocVa;
		TxD = *pDestTxD;
		pTxD = &TxD;
		RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
		MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#else
	    pTxD = (TXD_STRUC *)pMgmtRing->Cell[i].AllocVa;
#endif /* RT_BIG_ENDIAN */
	    DBGPRINT(RT_DEBUG_OFF, ("Desc #%d\n",i));	
		dump_txd(pAd, pTxD);
		dump_tmac_info(pAd, pDMAHeaderBufVA);
		DBGPRINT(RT_DEBUG_OFF, ("pkt physical address = %lx\n", 
						(ULONG)pMgmtRing->Cell[i].PacketPa));
#ifdef RT_BIG_ENDIAN
		MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#endif
	}    

	DBGPRINT(RT_DEBUG_OFF, ("BCN ring---------------------------------------------\n"));
	for (i = 0; i < BCN_RING_SIZE; i++)
	{
		pDMAHeaderBufVA = (UCHAR *)pBcnRing->Cell[i].DmaBuf.AllocVa;
#ifdef RT_BIG_ENDIAN
		pDestTxD = (TXD_STRUC *)pBcnRing->Cell[i].AllocVa;
		TxD = *pDestTxD;
		pTxD = &TxD;
		RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
		MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#else
	    pTxD = (TXD_STRUC *)pBcnRing->Cell[i].AllocVa;
#endif /* RT_BIG_ENDIAN */
	    DBGPRINT(RT_DEBUG_OFF, ("Desc #%d\n",i));	
		dump_txd(pAd, pTxD);
		dump_tmac_info(pAd, pDMAHeaderBufVA);
		DBGPRINT(RT_DEBUG_OFF, ("pkt physical address = %lx\n", 
						(ULONG)pBcnRing->Cell[i].PacketPa));
#ifdef RT_BIG_ENDIAN
		MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#endif
	}

	DBGPRINT(RT_DEBUG_OFF, ("BMC ring---------------------------------------------\n"));
	for (i = 0; i < TX_RING_SIZE; i++)
	{
		pDMAHeaderBufVA = (UCHAR *)pTxBmcRing->Cell[i].DmaBuf.AllocVa;
#ifdef RT_BIG_ENDIAN
		pDestTxD = (TXD_STRUC *)pTxBmcRing->Cell[i].AllocVa;
		TxD = *pDestTxD;
		pTxD = &TxD;
		RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
		MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#else
	    pTxD = (TXD_STRUC *)pTxBmcRing->Cell[i].AllocVa;
#endif /* RT_BIG_ENDIAN */
	    DBGPRINT(RT_DEBUG_OFF, ("Desc #%d\n",i));	
		dump_txd(pAd, pTxD);
		dump_tmac_info(pAd, pDMAHeaderBufVA);
		DBGPRINT(RT_DEBUG_OFF, ("pkt physical address = %lx\n", 
						(ULONG)pTxBmcRing->Cell[i].PacketPa));
#ifdef RT_BIG_ENDIAN
		MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#endif
	}
	
	DBGPRINT(RT_DEBUG_OFF, ("Control ring---------------------------------------------\n"));
	for(i = 0; i < MGMT_RING_SIZE; i++)
	{
		pDMAHeaderBufVA = (UCHAR *)pCtrlRing->Cell[i].DmaBuf.AllocVa;
#ifdef RT_BIG_ENDIAN
		pDestTxD = (TXD_STRUC *)pCtrlRing->Cell[i].AllocVa;
		TxD = *pDestTxD;
		pTxD = &TxD;
		RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
		MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#else
	    pTxD = (TXD_STRUC *)pCtrlRing->Cell[i].AllocVa;
#endif /* RT_BIG_ENDIAN */
	    DBGPRINT(RT_DEBUG_OFF, ("Desc #%d\n",i));	
		dump_txd(pAd, pTxD);
		dump_tmac_info(pAd, pDMAHeaderBufVA);
		DBGPRINT(RT_DEBUG_OFF, ("pkt physical address = %lx\n", 
						(ULONG)pCtrlRing->Cell[i].PacketPa));
#ifdef RT_BIG_ENDIAN
		MTMacInfoEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA), TYPE_TMACINFO, 32);
#endif
	}


	for (QueIdx = 0; QueIdx < NUM_OF_RX_RING; QueIdx++) 
	{
		pRxRing = &pAd->RxRing[QueIdx];
		
		DBGPRINT(RT_DEBUG_OFF, ("Rx Ring %d ---------------------------------\n", QueIdx));
		for(i = 0;i < RX_RING_SIZE; i++)
		{
#ifdef RT_BIG_ENDIAN
			pDestRxD = (RXD_STRUC *)pRxRing->Cell[i].AllocVa;
			RxD = *pDestRxD;
			pRxD = &RxD;
			RTMPDescriptorEndianChange((PUCHAR)pRxD, TYPE_RXD);
#else
	    	pRxD = (RXD_STRUC *)pRxRing->Cell[i].AllocVa;
#endif /* RT_BIG_ENDIAN */
			DBGPRINT(RT_DEBUG_OFF, ("Desc #%d\n",i));
			dump_rxd(pAd, pRxD);
	    	DBGPRINT(RT_DEBUG_OFF, ("pRxD->DDONE = %x\n", pRxD->DDONE));
		}
	}
#endif /* RTMP_MAC_PCI */

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Reset statistics counter

    Arguments:
        pAd            Pointer to our adapter
        arg                 

    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ResetStatCounter_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{  
	DBGPRINT(RT_DEBUG_TRACE, ("==>Set_ResetStatCounter_Proc\n"));

	/* add the most up-to-date h/w raw counters into software counters*/
	NICUpdateRawCounters(pAd);
    
	NdisZeroMemory(&pAd->WlanCounters, sizeof(COUNTER_802_11));
	NdisZeroMemory(&pAd->Counters8023, sizeof(COUNTER_802_3));
	NdisZeroMemory(&pAd->RalinkCounters, sizeof(COUNTER_RALINK));

#ifdef CONFIG_ATE
#ifdef CONFIG_QA
	pAd->ATECtrl.U2M = 0;
	pAd->ATECtrl.OtherCount = 0;
	pAd->ATECtrl.OtherData = 0;
#endif /* CONFIG_QA */
#endif /* CONFIG_ATE */

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
		DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

#ifdef TXBF_SUPPORT
	if (pAd->chipCap.FlgHwTxBfCap)
	{
		int i;
		for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
			NdisZeroMemory(&pAd->MacTab.Content[i].TxBFCounters, sizeof(pAd->MacTab.Content[i].TxBFCounters));
	}
#endif /* TXBF_SUPPORT */

	return TRUE;
}


BOOLEAN RTMPCheckStrPrintAble(
    IN  CHAR *pInPutStr, 
    IN  UCHAR strLen)
{
    UCHAR i=0;
    
    for (i=0; i<strLen; i++)
    {
        if ((pInPutStr[i] < 0x20) || (pInPutStr[i] > 0x7E))
            return FALSE;
    }
    
    return TRUE;
}


/*
	========================================================================
	
	Routine Description:
		Remove WPA Key process

	Arguments:
		pAd 					Pointer to our adapter
		pBuf							Pointer to the where the key stored

	Return Value:
		NDIS_SUCCESS					Add key successfully

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
#ifdef CONFIG_STA_SUPPORT
VOID RTMPSetDesiredRates(RTMP_ADAPTER *pAd, LONG Rates)
{
	NDIS_802_11_RATES aryRates;
	struct wifi_dev *wdev = &pAd->StaCfg.wdev;

	memset(&aryRates, 0x00, sizeof(NDIS_802_11_RATES));
	switch (pAd->CommonCfg.PhyMode)
	{
		case (UINT)(WMODE_A): /* A only*/
            switch (Rates)
            {
                case 6000000: /*6M*/
                    aryRates[0] = 0x0c; /* 6M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_0;
                    break;
                case 9000000: /*9M*/
                    aryRates[0] = 0x12; /* 9M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_1;
                    break;
                case 12000000: /*12M*/
                    aryRates[0] = 0x18; /* 12M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_2;
                    break;
                case 18000000: /*18M*/
                    aryRates[0] = 0x24; /* 18M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_3;
                    break;
                case 24000000: /*24M*/
                    aryRates[0] = 0x30; /* 24M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_4;
                    break;
                case 36000000: /*36M*/
                    aryRates[0] = 0x48; /* 36M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_5;
                    break;
                case 48000000: /*48M*/
                    aryRates[0] = 0x60; /* 48M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_6;
                    break;
                case 54000000: /*54M*/
                    aryRates[0] = 0x6c; /* 54M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_7;
                    break;
                case -1: /*Auto*/
                default:
                    aryRates[0] = 0x6c; /* 54Mbps*/
                    aryRates[1] = 0x60; /* 48Mbps*/
                    aryRates[2] = 0x48; /* 36Mbps*/
                    aryRates[3] = 0x30; /* 24Mbps*/
                    aryRates[4] = 0x24; /* 18M*/
                    aryRates[5] = 0x18; /* 12M*/
                    aryRates[6] = 0x12; /* 9M*/
                    aryRates[7] = 0x0c; /* 6M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_AUTO;
                    break;
            }
            break;
		case (UINT)(WMODE_B | WMODE_G): /* B/G Mixed*/
		case (UINT)(WMODE_B): /* B only*/
		case (UINT)(WMODE_A | WMODE_B | WMODE_G): /* A/B/G Mixed*/
        default:
            switch (Rates)
            {
                case 1000000: /*1M*/
                    aryRates[0] = 0x02;
                    wdev->DesiredTransmitSetting.field.MCS = MCS_0;
                    break;
                case 2000000: /*2M*/
                    aryRates[0] = 0x04;
                    wdev->DesiredTransmitSetting.field.MCS = MCS_1;
                    break;
                case 5000000: /*5.5M*/
                    aryRates[0] = 0x0b; /* 5.5M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_2;
                    break;
                case 11000000: /*11M*/
                    aryRates[0] = 0x16; /* 11M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_3;
                    break;
                case 6000000: /*6M*/
                    aryRates[0] = 0x0c; /* 6M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_0;
                    break;
                case 9000000: /*9M*/
                    aryRates[0] = 0x12; /* 9M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_1;
                    break;
                case 12000000: /*12M*/
                    aryRates[0] = 0x18; /* 12M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_2;
                    break;
                case 18000000: /*18M*/
                    aryRates[0] = 0x24; /* 18M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_3;
                    break;
                case 24000000: /*24M*/
                    aryRates[0] = 0x30; /* 24M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_4;
                    break;
                case 36000000: /*36M*/
                    aryRates[0] = 0x48; /* 36M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_5;
                    break;
                case 48000000: /*48M*/
                    aryRates[0] = 0x60; /* 48M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_6;
                    break;
                case 54000000: /*54M*/
                    aryRates[0] = 0x6c; /* 54M*/
                    wdev->DesiredTransmitSetting.field.MCS = MCS_7;
                    break;
                case -1: /*Auto*/
                default:
                    if (pAd->CommonCfg.PhyMode == WMODE_B)
                    { /*B Only*/
                        aryRates[0] = 0x16; /* 11Mbps*/
                        aryRates[1] = 0x0b; /* 5.5Mbps*/
                        aryRates[2] = 0x04; /* 2Mbps*/
                        aryRates[3] = 0x02; /* 1Mbps*/
                    }
                    else
                    { /*(B/G) Mixed or (A/B/G) Mixed*/
                        aryRates[0] = 0x6c; /* 54Mbps*/
                        aryRates[1] = 0x60; /* 48Mbps*/
                        aryRates[2] = 0x48; /* 36Mbps*/
                        aryRates[3] = 0x30; /* 24Mbps*/
                        aryRates[4] = 0x16; /* 11Mbps*/
                        aryRates[5] = 0x0b; /* 5.5Mbps*/
                        aryRates[6] = 0x04; /* 2Mbps*/
                        aryRates[7] = 0x02; /* 1Mbps*/
                    }
                    wdev->DesiredTransmitSetting.field.MCS = MCS_AUTO;
                    break;
            }
            break;
    }

    NdisZeroMemory(pAd->CommonCfg.DesireRate, MAX_LEN_OF_SUPPORTED_RATES);
    NdisMoveMemory(pAd->CommonCfg.DesireRate, &aryRates, sizeof(NDIS_802_11_RATES));
    DBGPRINT(RT_DEBUG_TRACE, (" RTMPSetDesiredRates (%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)\n",
        pAd->CommonCfg.DesireRate[0], pAd->CommonCfg.DesireRate[1],
        pAd->CommonCfg.DesireRate[2], pAd->CommonCfg.DesireRate[3],
        pAd->CommonCfg.DesireRate[4], pAd->CommonCfg.DesireRate[5],
        pAd->CommonCfg.DesireRate[6], pAd->CommonCfg.DesireRate[7] ));
    /* Changing DesiredRate may affect the MAX TX rate we used to TX frames out*/
    MlmeUpdateTxRates(pAd, FALSE, 0);
}
#endif /* CONFIG_STA_SUPPORT */


#if defined(CONFIG_STA_SUPPORT) || defined(WPA_SUPPLICANT_SUPPORT)
NDIS_STATUS RTMPWPARemoveKeyProc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PVOID			pBuf)
{
	PNDIS_802_11_REMOVE_KEY pKey;
	ULONG					KeyIdx;
	NDIS_STATUS 			Status = NDIS_STATUS_FAILURE;
	BOOLEAN 	bTxKey; 		/* Set the key as transmit key*/
	BOOLEAN 	bPairwise;		/* Indicate the key is pairwise key*/
	//BOOLEAN 	bKeyRSC;		/* indicate the receive  SC set by KeyRSC value.*/
								/* Otherwise, it will set by the NIC.*/
	//BOOLEAN 	bAuthenticator; /* indicate key is set by authenticator.*/
	INT 		i;
#ifdef APCLI_SUPPORT
#ifdef WPA_SUPPLICANT_SUPPORT
	UCHAR ifIndex;
	INT BssIdx;
	POS_COOKIE	pObj = (POS_COOKIE) pAd->OS_Cookie;
#endif/*WPA_SUPPLICANT_SUPPORT*/
#endif/*APCLI_SUPPORT*/
	DBGPRINT(RT_DEBUG_TRACE,("---> RTMPWPARemoveKeyProc\n"));
	
	pKey = (PNDIS_802_11_REMOVE_KEY) pBuf;
	KeyIdx = pKey->KeyIndex & 0xff;
	/* Bit 31 of Add-key, Tx Key*/
	bTxKey = (pKey->KeyIndex & 0x80000000) ? TRUE : FALSE;
	/* Bit 30 of Add-key PairwiseKey*/
	bPairwise = (pKey->KeyIndex & 0x40000000) ? TRUE : FALSE;
	/* Bit 29 of Add-key KeyRSC*/
	//bKeyRSC = (pKey->KeyIndex & 0x20000000) ? TRUE : FALSE;
	/* Bit 28 of Add-key Authenticator*/
	//bAuthenticator = (pKey->KeyIndex & 0x10000000) ? TRUE : FALSE;

	/* 1. If bTx is TRUE, return failure information*/
	if (bTxKey == TRUE)
		return(NDIS_STATUS_INVALID_DATA);

	/* 2. Check Pairwise Key*/
	if (bPairwise)
	{
		/* a. If BSSID is broadcast, remove all pairwise keys.*/
		/* b. If not broadcast, remove the pairwise specified by BSSID*/
		for (i = 0; i < SHARE_KEY_NUM; i++)
		{
#ifdef APCLI_SUPPORT
#ifdef WPA_SUPPLICANT_SUPPORT
			if (pObj->ioctl_if_type == INT_APCLI)
			{
				/*if (MAC_ADDR_EQUAL(pAd->ApCfg.ApCliTab[ifIndex].SharedKey[i].BssId, pKey->BSSID)) */
				{
					ifIndex = (UCHAR)pObj->ioctl_if;
					BssIdx = pAd->ApCfg.BssidNum + MAX_MESH_NUM + ifIndex;
					DBGPRINT(RT_DEBUG_TRACE,("APCLI RTMPWPARemoveKeyProc(KeyIdx=%d)\n", i));
					pAd->ApCfg.ApCliTab[ifIndex].SharedKey[i].KeyLen = 0;
					pAd->ApCfg.ApCliTab[ifIndex].SharedKey[i].CipherAlg = CIPHER_NONE;
					AsicRemoveSharedKeyEntry(pAd, (UCHAR)BssIdx, (UCHAR)i);
					Status = NDIS_STATUS_SUCCESS;
					break;
				}
			}
			else
#endif/*WPA_SUPPLICANT_SUPPORT*/
#endif/*APCLI_SUPPORT*/
			{
#ifdef CONFIG_STA_SUPPORT
				if (MAC_ADDR_EQUAL(pAd->SharedKey[BSS0][i].BssId, pKey->BSSID))
				{
					DBGPRINT(RT_DEBUG_TRACE,("RTMPWPARemoveKeyProc(KeyIdx=%d)\n", i));
					pAd->SharedKey[BSS0][i].KeyLen = 0;
					pAd->SharedKey[BSS0][i].CipherAlg = CIPHER_NONE;
					AsicRemoveSharedKeyEntry(pAd, (UCHAR)BSS0, (UCHAR)i);
					Status = NDIS_STATUS_SUCCESS;
					break;
				}
#endif/*CONFIG_STA_SUPPORT*/
			}
		}
	}
	/* 3. Group Key*/
	else
	{
		/* a. If BSSID is broadcast, remove all group keys indexed*/
		/* b. If BSSID matched, delete the group key indexed.*/
		DBGPRINT(RT_DEBUG_TRACE,("RTMPWPARemoveKeyProc(KeyIdx=%ld)\n", KeyIdx));
		pAd->SharedKey[BSS0][KeyIdx].KeyLen = 0;
		pAd->SharedKey[BSS0][KeyIdx].CipherAlg = CIPHER_NONE;
		AsicRemoveSharedKeyEntry(pAd, (UCHAR)BSS0, (UCHAR)KeyIdx);
		Status = NDIS_STATUS_SUCCESS;
	}

	return (Status);
}
#endif /* defined(CONFIG_STA_SUPPORT) || defined(WPA_SUPPLICANT_SUPPORT) */


#ifdef CONFIG_STA_SUPPORT
/*
	========================================================================
	
	Routine Description:
		Remove All WPA Keys

	Arguments:
		pAd 					Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
VOID RTMPWPARemoveAllKeys(RTMP_ADAPTER *pAd)
{
	UCHAR i;
	struct wifi_dev *wdev = &pAd->StaCfg.wdev;


	DBGPRINT(RT_DEBUG_TRACE,("RTMPWPARemoveAllKeys(AuthMode=%d, WepStatus=%d)\n",
				wdev->AuthMode, wdev->WepStatus));
	/*
		For WEP/CKIP/WPA-None, there is no need to remove it, since WinXP won't set it 
		again after Link up. And it will be replaced if user changed it.
	*/
	if (wdev->AuthMode < Ndis802_11AuthModeWPA ||
		wdev->AuthMode == Ndis802_11AuthModeWPANone)
		return;

#ifdef PCIE_PS_SUPPORT
        RTMP_CLEAR_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP);
#endif /* PCIE_PS_SUPPORT */

	/* set BSSID wcid entry of the Pair-wise Key table as no-security mode*/
	AsicRemovePairwiseKeyEntry(pAd, BSSID_WCID);

	/* set all shared key mode as no-security. */
	for (i = 0; i < SHARE_KEY_NUM; i++)
    {
		DBGPRINT(RT_DEBUG_TRACE,("remove %s key #%d\n", CipherName[pAd->SharedKey[BSS0][i].CipherAlg], i));
		NdisZeroMemory(&pAd->SharedKey[BSS0][i], sizeof(CIPHER_KEY));  						

		AsicRemoveSharedKeyEntry(pAd, BSS0, i);
	}
#ifdef PCIE_PS_SUPPORT
	RTMP_SET_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP);
#endif /* PCIE_PS_SUPPORT */

}
#endif /* CONFIG_STA_SUPPORT */	


/*
	========================================================================
	Routine Description:
		Change NIC PHY mode. Re-association may be necessary

	Arguments:
		pAd - Pointer to our adapter
		phymode  - 

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	========================================================================
*/
VOID RTMPSetPhyMode(RTMP_ADAPTER *pAd, ULONG phymode)
{
	INT i;
	//COMMON_CONFIG *cmm_cfg = &pAd->CommonCfg;

	/* the selected phymode must be supported by the RF IC encoded in E2PROM*/

	pAd->CommonCfg.PhyMode = (UCHAR)phymode;

	DBGPRINT(RT_DEBUG_TRACE,("%s(): PhyMode=%d, channel=%d \n",
				__FUNCTION__, pAd->CommonCfg.PhyMode,
				pAd->CommonCfg.Channel));

#ifdef EXT_BUILD_CHANNEL_LIST
	BuildChannelListEx(pAd);
#else
	BuildChannelList(pAd);
#endif /* EXT_BUILD_CHANNEL_LIST */

#ifdef P2P_CHANNEL_LIST_SEPARATE
	P2PBuildChannelList(pAd);
#endif /* P2P_CHANNEL_LIST_SEPARATE */ 


	/* sanity check user setting*/
	for (i = 0; i < pAd->ChannelListNum; i++)
	{
		if (pAd->CommonCfg.Channel == pAd->ChannelList[i].Channel)
			break;
	}

	if (i == pAd->ChannelListNum)
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		if (pAd->CommonCfg.Channel != 0)
				pAd->CommonCfg.Channel = FirstChannel(pAd);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			pAd->CommonCfg.Channel = FirstChannel(pAd);
#endif /* CONFIG_STA_SUPPORT */
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): channel out of range, use first ch=%d\n", 
					__FUNCTION__, pAd->CommonCfg.Channel));
	}
	
	NdisZeroMemory(pAd->CommonCfg.SupRate, MAX_LEN_OF_SUPPORTED_RATES);
	NdisZeroMemory(pAd->CommonCfg.ExtRate, MAX_LEN_OF_SUPPORTED_RATES);
	NdisZeroMemory(pAd->CommonCfg.DesireRate, MAX_LEN_OF_SUPPORTED_RATES);
	switch (phymode) {
	case (UINT)(WMODE_B):
			pAd->CommonCfg.SupRate[0]  = 0x82;	  /* 1 mbps, in units of 0.5 Mbps, basic rate */
			pAd->CommonCfg.SupRate[1]  = 0x84;	  /* 2 mbps, in units of 0.5 Mbps, basic rate */
			pAd->CommonCfg.SupRate[2]  = 0x8B;	  /* 5.5 mbps, in units of 0.5 Mbps, basic rate */
			pAd->CommonCfg.SupRate[3]  = 0x96;	  /* 11 mbps, in units of 0.5 Mbps, basic rate */
			pAd->CommonCfg.SupRateLen  = 4;
			pAd->CommonCfg.ExtRateLen  = 0;
			pAd->CommonCfg.DesireRate[0]  = 2;	   /* 1 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[1]  = 4;	   /* 2 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[2]  = 11;    /* 5.5 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[3]  = 22;    /* 11 mbps, in units of 0.5 Mbps*/
			/*pAd->CommonCfg.HTPhyMode.field.MODE = MODE_CCK;  This MODE is only FYI. not use*/
			break;

		/*
			In current design, we will put supported/extended rate element in
			beacon even we are 11n-only mode.
			Or some 11n stations will not connect to us if we do not put
			supported/extended rate element in beacon.
		*/
		case (UINT)(WMODE_B | WMODE_G):
		case (UINT)(WMODE_A | WMODE_B | WMODE_G):
#ifdef DOT11_N_SUPPORT
		case (UINT)(WMODE_A | WMODE_B | WMODE_G | WMODE_GN | WMODE_AN):
		case (UINT)(WMODE_B | WMODE_G | WMODE_GN):
#endif /* DOT11_N_SUPPORT */
#ifdef DOT11_VHT_AC
		case (UINT)(WMODE_A | WMODE_B | WMODE_G | WMODE_GN | WMODE_AN | WMODE_AC):
#endif /* DOT11_VHT_AC */
			pAd->CommonCfg.SupRate[0]  = 0x82;	  /* 1 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[1]  = 0x84;	  /* 2 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[2]  = 0x8B;	  /* 5.5 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[3]  = 0x96;	  /* 11 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[4]  = 0x12;	  /* 9 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[5]  = 0x24;	  /* 18 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[6]  = 0x48;	  /* 36 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[7]  = 0x6c;	  /* 54 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRateLen  = 8;
			pAd->CommonCfg.ExtRate[0]  = 0x0C;	  /* 6 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.ExtRate[1]  = 0x18;	  /* 12 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.ExtRate[2]  = 0x30;	  /* 24 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.ExtRate[3]  = 0x60;	  /* 48 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.ExtRateLen  = 4;
			pAd->CommonCfg.DesireRate[0]  = 2;	   /* 1 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[1]  = 4;	   /* 2 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[2]  = 11;    /* 5.5 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[3]  = 22;    /* 11 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[4]  = 12;    /* 6 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[5]  = 18;    /* 9 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[6]  = 24;    /* 12 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[7]  = 36;    /* 18 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[8]  = 48;    /* 24 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[9]  = 72;    /* 36 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[10] = 96;    /* 48 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[11] = 108;   /* 54 mbps, in units of 0.5 Mbps*/
			break;

		case (UINT)(WMODE_A):
		case (UINT)(WMODE_G):
#ifdef DOT11_N_SUPPORT
		case (UINT)(WMODE_A | WMODE_AN):
		case (UINT)(WMODE_A | WMODE_G | WMODE_GN | WMODE_AN):
		case (UINT)(WMODE_G | WMODE_GN):
		case (UINT)(WMODE_GN):
		case (UINT)(WMODE_AN):
#endif /* DOT11_N_SUPPORT */
#ifdef DOT11_VHT_AC
		case (UINT)(WMODE_A | WMODE_AN | WMODE_AC):
		case (UINT)(WMODE_AN | WMODE_AC):
		case (UINT)(WMODE_G | WMODE_GN | WMODE_A | WMODE_AN | WMODE_AC):
#endif /* DOT11_VHT_AC */
			pAd->CommonCfg.SupRate[0]  = 0x8C;	  /* 6 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[1]  = 0x12;	  /* 9 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[2]  = 0x98;	  /* 12 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[3]  = 0x24;	  /* 18 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[4]  = 0xb0;	  /* 24 mbps, in units of 0.5 Mbps, basic rate*/
			pAd->CommonCfg.SupRate[5]  = 0x48;	  /* 36 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[6]  = 0x60;	  /* 48 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRate[7]  = 0x6c;	  /* 54 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.SupRateLen  = 8;
			pAd->CommonCfg.ExtRateLen  = 0;
			pAd->CommonCfg.DesireRate[0]  = 12;    /* 6 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[1]  = 18;    /* 9 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[2]  = 24;    /* 12 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[3]  = 36;    /* 18 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[4]  = 48;    /* 24 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[5]  = 72;    /* 36 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[6]  = 96;    /* 48 mbps, in units of 0.5 Mbps*/
			pAd->CommonCfg.DesireRate[7]  = 108;   /* 54 mbps, in units of 0.5 Mbps*/
			/*pAd->CommonCfg.HTPhyMode.field.MODE = MODE_OFDM;  This MODE is only FYI. not use*/
			break;

		default:
			break;
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		UINT	apidx;
		
		for (apidx = 0; apidx < pAd->ApCfg.BssidNum; apidx++)
		{
			MlmeUpdateTxRates(pAd, FALSE, (UCHAR)apidx);
		}	
#ifdef WDS_SUPPORT
		for (apidx = 0; apidx < MAX_WDS_ENTRY; apidx++)
		{				
			MlmeUpdateTxRates(pAd, FALSE, (UCHAR)(apidx + MIN_NET_DEVICE_FOR_WDS));
		}
#endif /* WDS_SUPPORT */
#ifdef APCLI_SUPPORT
		for (apidx = 0; apidx < MAX_APCLI_NUM; apidx++)
		{				
			MlmeUpdateTxRates(pAd, FALSE, (UCHAR)(apidx + MIN_NET_DEVICE_FOR_APCLI));
		}
#endif /* APCLI_SUPPORT */		
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s: Update for STA\n", __func__));
		MlmeUpdateTxRates(pAd, FALSE, (UCHAR)BSS0);
	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef P2P_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		UINT	apidx;
		for (apidx = 0; apidx < pAd->ApCfg.BssidNum; apidx++)
		{
			MlmeUpdateTxRates(pAd, FALSE, (UCHAR)(apidx + MIN_NET_DEVICE_FOR_P2P_GO));
		}

		for (apidx = 0; apidx < MAX_APCLI_NUM; apidx++)
		{				
			MlmeUpdateTxRates(pAd, FALSE, (UCHAR)(apidx + MIN_NET_DEVICE_FOR_APCLI));
		}
	}
#endif /* P2P_SUPPORT */

//CFG_TODO
#ifdef RT_CFG80211_P2P_SUPPORT
	NdisZeroMemory(pAd->cfg80211_ctrl.P2pSupRate, MAX_LEN_OF_SUPPORTED_RATES);
	NdisZeroMemory(pAd->cfg80211_ctrl.P2pExtRate, MAX_LEN_OF_SUPPORTED_RATES);
	
	pAd->cfg80211_ctrl.P2pSupRate[0]  = 0x8C;        /* 6 mbps, in units of 0.5 Mbps, basic rate*/
	pAd->cfg80211_ctrl.P2pSupRate[1]  = 0x12;        /* 9 mbps, in units of 0.5 Mbps*/
	pAd->cfg80211_ctrl.P2pSupRate[2]  = 0x98;        /* 12 mbps, in units of 0.5 Mbps, basic rate*/
	pAd->cfg80211_ctrl.P2pSupRate[3]  = 0x24;        /* 18 mbps, in units of 0.5 Mbps*/
	pAd->cfg80211_ctrl.P2pSupRate[4]  = 0xb0;        /* 24 mbps, in units of 0.5 Mbps, basic rate*/
	pAd->cfg80211_ctrl.P2pSupRate[5]  = 0x48;        /* 36 mbps, in units of 0.5 Mbps*/
	pAd->cfg80211_ctrl.P2pSupRate[6]  = 0x60;        /* 48 mbps, in units of 0.5 Mbps*/
	pAd->cfg80211_ctrl.P2pSupRate[7]  = 0x6c;        /* 54 mbps, in units of 0.5 Mbps*/
	pAd->cfg80211_ctrl.P2pSupRateLen  = 8;
	
	pAd->cfg80211_ctrl.P2pExtRateLen  = 0;
	
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
	DBGPRINT(RT_DEBUG_OFF, ("%s: Update for AP\n", __func__));
	MlmeUpdateTxRates(pAd, FALSE
				, (UCHAR)(MAIN_MBSSID + MIN_NET_DEVICE_FOR_CFG80211_VIF_P2P_GO));

	DBGPRINT(RT_DEBUG_OFF, ("%s: Update for APCLI\n", __func__));
	MlmeUpdateTxRates(pAd, FALSE, (UCHAR)(MAIN_MBSSID + MIN_NET_DEVICE_FOR_APCLI));
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */
#endif /* RT_CFG80211_P2P_SUPPORT */

#ifdef DOT11_N_SUPPORT
	SetCommonHT(pAd);
#endif /* DOT11_N_SUPPORT */

#ifdef DOT11_VHT_AC
	SetCommonVHT(pAd);
#endif /* DOT11_VHT_AC */
}


/*
	========================================================================
	Description:
		Add Client security information into ASIC WCID table and IVEIV table.
    Return:
	========================================================================
*/
VOID RTMPAddWcidAttributeEntry(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR BssIdx,
	IN UCHAR KeyIdx,
	IN UCHAR CipherAlg,
	IN MAC_TABLE_ENTRY *pEntry)
{
	UINT32		WCIDAttri = 0;
	USHORT		offset;
	UCHAR		IVEIV = 0;
	USHORT		Wcid = 0;
#ifdef CONFIG_AP_SUPPORT
	BOOLEAN		IEEE8021X = FALSE;
#endif /* CONFIG_AP_SUPPORT */


	// TODO: shiang-7603!! fix me
	if (IS_MT7603(pAd) || IS_MT7628(pAd) || IS_MT7636(pAd)) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): MT7603 Not support yet!\n", __FUNCTION__));
		return;
	}

#ifdef MESH_SUPPORT			
	if (BssIdx >= MIN_NET_DEVICE_FOR_MESH)
	{		
		if (pEntry)		
		{
			BssIdx -= MIN_NET_DEVICE_FOR_MESH;
			Wcid = pEntry->wcid;
		}	
		else
		{
			DBGPRINT(RT_DEBUG_WARN, ("RTMPAddWcidAttributeEntry: Mesh link doesn't need to set Group WCID Attribute. \n"));	
			return;
		}
	}	
	else
#endif /* MESH_SUPPORT */
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
#ifdef APCLI_SUPPORT
			if (BssIdx >= MIN_NET_DEVICE_FOR_APCLI)
			{
				if (pEntry)		
					BssIdx -= MIN_NET_DEVICE_FOR_APCLI;		
				else
				{
					DBGPRINT(RT_DEBUG_WARN, ("RTMPAddWcidAttributeEntry: AP-Client link doesn't need to set Group WCID Attribute. \n"));	
					return;
				}
			}	
			else 
#endif /* APCLI_SUPPORT */
#ifdef WDS_SUPPORT
			if (BssIdx >= MIN_NET_DEVICE_FOR_WDS)
			{
				if (pEntry)		
					BssIdx = BSS0;		
				else
				{
					DBGPRINT(RT_DEBUG_WARN, ("RTMPAddWcidAttributeEntry: WDS link doesn't need to set Group WCID Attribute. \n"));	
					return;
				}
			}	
			else
#endif /* WDS_SUPPORT */
			{
				if (BssIdx >= pAd->ApCfg.BssidNum)
				{
					DBGPRINT(RT_DEBUG_ERROR, ("RTMPAddWcidAttributeEntry: The BSS-index(%d) is out of range for MBSSID link. \n", BssIdx));	
					return;
				}
			}

			/* choose wcid number*/
			if (pEntry)
				Wcid = pEntry->wcid;
			else
				GET_GroupKey_WCID(pAd, Wcid, BssIdx);		

#ifdef DOT1X_SUPPORT
			if ((BssIdx < pAd->ApCfg.BssidNum) && (BssIdx < MAX_MBSSID_NUM(pAd)) && (BssIdx < HW_BEACON_MAX_NUM))
				IEEE8021X = pAd->ApCfg.MBSSID[BssIdx].wdev.IEEE8021X;
#endif /* DOT1X_SUPPORT */			
		}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			if (BssIdx > BSS0)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("RTMPAddWcidAttributeEntry: The BSS-index(%d) is out of range for Infra link. \n", BssIdx));	
				return;
			}

			/*
				1.	In ADHOC mode, the AID is wcid number. And NO mesh link exists.
				2.	In Infra mode, the AID:1 MUST be wcid of infra STA.
									the AID:2~ assign to mesh link entry.
			*/
#if 0
			if (pEntry && ADHOC_ON(pAd))
				Wcid = pEntry->wcid;
			else if (pEntry && INFRA_ON(pAd))
			{
#ifdef QOS_DLS_SUPPORT
				if (IS_ENTRY_DLS(pEntry))
					Wcid = pEntry->wcid;
				else
#endif /* QOS_DLS_SUPPORT */
				Wcid = BSSID_WCID;
			}
#endif		
			if (pEntry)
				Wcid = pEntry->wcid;
			else
				Wcid = MCAST_WCID;
		}
#endif /* CONFIG_STA_SUPPORT */
	}

	/* Update WCID attribute table*/
	{
		UINT32 wcid_attr_base = 0, wcid_attr_size = 0;
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
			wcid_attr_base = RLT_MAC_WCID_ATTRIBUTE_BASE;
			wcid_attr_size = RLT_HW_WCID_ATTRI_SIZE;
		}
#endif /* RLT_MAC */
#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			wcid_attr_base = MAC_WCID_ATTRIBUTE_BASE;
			wcid_attr_size = HW_WCID_ATTRI_SIZE;
		}
#endif /* RTMP_MAC */

		offset = (USHORT)(wcid_attr_base + (Wcid * wcid_attr_size));
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			/*
				1.	Wds-links and Mesh-links always use Pair-wise key table.
				2. 	When the CipherAlg is TKIP, AES or the dynamic WEP is enabled,
					it needs to set key into Pair-wise Key Table.
				3.	The pair-wise key security mode is set NONE, it means as no security.
			*/
			if (pEntry && (IS_ENTRY_WDS(pEntry) || IS_ENTRY_MESH(pEntry)))
				WCIDAttri = (BssIdx<<4) | (CipherAlg<<1) | PAIRWISEKEYTABLE;
			else if ((pEntry) && 
					((CipherAlg == CIPHER_TKIP) || 
					 (CipherAlg == CIPHER_AES) || 
					 (CipherAlg == CIPHER_NONE) || 
					 (IEEE8021X == TRUE)))
				WCIDAttri = (BssIdx<<4) | (CipherAlg<<1) | PAIRWISEKEYTABLE;
			else
				WCIDAttri = (BssIdx<<4) | (CipherAlg<<1) | SHAREDKEYTABLE;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			if (pEntry && IS_ENTRY_MESH(pEntry))
				WCIDAttri = (CipherAlg<<1) | PAIRWISEKEYTABLE;
#if defined(DOT11Z_TDLS_SUPPORT) || defined(QOS_DLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
			else if ((pEntry) && (IS_ENTRY_DLS(pEntry) || IS_ENTRY_TDLS(pEntry)) &&
						((CipherAlg == CIPHER_TKIP) || 
						(CipherAlg == CIPHER_AES) || 
					 	(CipherAlg == CIPHER_NONE))) 
				WCIDAttri = (CipherAlg<<1) | PAIRWISEKEYTABLE;
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(QOS_DLS_SUPPORT) */
			else
				WCIDAttri = (CipherAlg<<1) | SHAREDKEYTABLE;
		}
#endif /* CONFIG_STA_SUPPORT */

		RTMP_IO_WRITE32(pAd, offset, WCIDAttri);
	}
		
	/* Update IV/EIV table*/
	{
		UINT32 iveiv_tb_base = 0, iveiv_tb_size = 0;

#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
			iveiv_tb_base = RLT_MAC_IVEIV_TABLE_BASE;
			iveiv_tb_size = RLT_HW_IVEIV_ENTRY_SIZE;
		}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			iveiv_tb_base = MAC_IVEIV_TABLE_BASE;
			iveiv_tb_size = HW_IVEIV_ENTRY_SIZE;
		}
#endif /* RTMP_MAC */

		offset = (USHORT)(iveiv_tb_base + (Wcid * iveiv_tb_size));

		/* WPA mode*/
		if ((CipherAlg == CIPHER_TKIP) || (CipherAlg == CIPHER_AES))
		{	
			/* Eiv bit on. keyid always is 0 for pairwise key */
			IVEIV = (KeyIdx <<6) | 0x20;	
		}	 
		else
		{
			/* WEP KeyIdx is default tx key. */
			IVEIV = (KeyIdx << 6);	
		}

		/* For key index and ext IV bit, so only need to update the position(offset+3).*/
#ifdef RTMP_MAC_PCI	
		RTMP_IO_WRITE8(pAd, offset+3, IVEIV);
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		RTUSBMultiWrite_OneByte(pAd, offset+3, &IVEIV);
#endif /* RTMP_MAC_USB */
	}
	DBGPRINT(RT_DEBUG_TRACE,("RTMPAddWcidAttributeEntry: WCID #%d, KeyIndex #%d, Alg=%s\n",Wcid, KeyIdx, CipherName[CipherAlg]));
	DBGPRINT(RT_DEBUG_TRACE,("	WCIDAttri = 0x%x \n",  WCIDAttri));	

}

/* 
    ==========================================================================
    Description:
        Parse encryption type
Arguments:
    pAdapter                    Pointer to our adapter
    wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
    ==========================================================================
*/
RTMP_STRING *GetEncryptType(CHAR enc)
{
    if(enc == Ndis802_11WEPDisabled)
        return "NONE";
    if(enc == Ndis802_11WEPEnabled)
    	return "WEP";
    if(enc == Ndis802_11TKIPEnable)
    	return "TKIP";
    if(enc == Ndis802_11AESEnable)
    	return "AES";
	if(enc == Ndis802_11TKIPAESMix)
    	return "TKIPAES";
#ifdef WAPI_SUPPORT
	if(enc == Ndis802_11EncryptionSMS4Enabled)
    	return "SMS4";
#endif /* WAPI_SUPPORT */
    else
    	return "UNKNOW";
}

RTMP_STRING *GetAuthMode(CHAR auth)
{
    if(auth == Ndis802_11AuthModeOpen)
    	return "OPEN";
    if(auth == Ndis802_11AuthModeShared)
    	return "SHARED";
	if(auth == Ndis802_11AuthModeAutoSwitch)
    	return "AUTOWEP";
    if(auth == Ndis802_11AuthModeWPA)
    	return "WPA";
    if(auth == Ndis802_11AuthModeWPAPSK)
    	return "WPAPSK";
    if(auth == Ndis802_11AuthModeWPANone)
    	return "WPANONE";
    if(auth == Ndis802_11AuthModeWPA2)
    	return "WPA2";
    if(auth == Ndis802_11AuthModeWPA2PSK)
    	return "WPA2PSK";
	if(auth == Ndis802_11AuthModeWPA1WPA2)
    	return "WPA1WPA2";
	if(auth == Ndis802_11AuthModeWPA1PSKWPA2PSK)
		return "WPA1PSKWPA2PSK";
#ifdef WPA3_SUPPORT
	if(auth == Ndis802_11AuthModeWPA3SAE)
    	return "WPA3SAE";
	if(auth == Ndis802_11AuthModeWPA2PSKWPA3SAE)
    	return "WPA2PSKWPA3SAE";
#endif 
#ifdef WAPI_SUPPORT
	if(auth == Ndis802_11AuthModeWAICERT)
    	return "WAI-CERT";
	if(auth == Ndis802_11AuthModeWAIPSK)
    	return "WAI-PSK";
#endif /* WAPI_SUPPORT */
	
    	return "UNKNOW";
}		

#ifdef DPA_T
VOID GetMixedCipherStr(
	IN  BSS_ENTRY *pBss,
	IN  NDIS_802_11_AUTHENTICATION_MODE AuthMode,
	INOUT RTMP_STRING tmpSecurityStr[])
{
	UCHAR		chiper = 1;
	
	/* WPA or WPAPSK */
	if (AuthMode == Ndis802_11AuthModeWPAPSK)
		sprintf(tmpSecurityStr, "%sWPAPSK-", tmpSecurityStr);
	else if (AuthMode == Ndis802_11AuthModeWPA2PSK)	
	{
		sprintf(tmpSecurityStr, "%sWPA2PSK-", tmpSecurityStr);
		chiper = 2;
	}
#ifdef WPA3_SUPPORT
	else if (AuthMode == Ndis802_11AuthModeWPA3SAE) {
		sprintf(tmpSecurityStr, "%sWPA3SAE-", tmpSecurityStr);
		chiper = 2;
	}
#endif
	else if (AuthMode == Ndis802_11AuthModeWPA)
		sprintf(tmpSecurityStr, "%sWPA-", tmpSecurityStr);
	else if (AuthMode == Ndis802_11AuthModeWPA2)
	{
		sprintf(tmpSecurityStr, "%sWPA2-", tmpSecurityStr);
		chiper = 2;
	}
	if (chiper == 1)
	{
		if ((pBss->WPA.PairCipher != pBss->WPA.PairCipherAux) &&
			(pBss->WPA.PairCipherAux != Ndis802_11WEPDisabled))
		{
			if (pBss->WPA.PairCipherAux == Ndis802_11TKIPEnable)
				sprintf(tmpSecurityStr, "%sTKIP", tmpSecurityStr);
			else if (pBss->WPA.PairCipherAux == Ndis802_11AESEnable)
				sprintf(tmpSecurityStr, "%sAES", tmpSecurityStr);
		}
		if (pBss->WPA.PairCipher == Ndis802_11TKIPEnable)
			sprintf(tmpSecurityStr, "%sTKIP", tmpSecurityStr);
		else if (pBss->WPA.PairCipher == Ndis802_11AESEnable)
			sprintf(tmpSecurityStr, "%sAES", tmpSecurityStr);
	}
	else
	{
		if ((pBss->WPA2.PairCipher != pBss->WPA2.PairCipherAux) &&
			(pBss->WPA2.PairCipherAux != Ndis802_11WEPDisabled))
		{
			if (pBss->WPA2.PairCipherAux == Ndis802_11TKIPEnable)
				sprintf(tmpSecurityStr, "%sTKIP", tmpSecurityStr);
			else if (pBss->WPA2.PairCipherAux == Ndis802_11AESEnable)
				sprintf(tmpSecurityStr, "%sAES", tmpSecurityStr);
		}
		if (pBss->WPA2.PairCipher == Ndis802_11TKIPEnable)
			sprintf(tmpSecurityStr, "%sTKIP", tmpSecurityStr);
		else if (pBss->WPA2.PairCipher == Ndis802_11AESEnable)
			sprintf(tmpSecurityStr, "%sAES", tmpSecurityStr);
	}
}
#endif /* DPA_T */

#ifdef WPA3_SUPPORT
VOID APSetCipherAndAuthMode(IN BSS_ENTRY *pBss, OUT NDIS_802_11_ENCRYPTION_STATUS ap_cipher,
									OUT NDIS_802_11_AUTHENTICATION_MODE ap_auth_mode, OUT char *SecurityStr)
{
	if ((pBss->AuthMode[0] == Ndis802_11AuthModeWPA3SAE) ||
		(pBss->AuthMode[0] == Ndis802_11AuthModeWPA2PSKWPA3SAE)) {
		if ((pBss->AuthMode[1] == Ndis802_11AuthModeWPA3SAE) ||
			(pBss->AuthMode[1] == Ndis802_11AuthModeWPA2PSKWPA3SAE))
			ap_auth_mode = Ndis802_11AuthModeWPA2PSKWPA3SAE;
		else
			ap_auth_mode = pBss->AuthMode[0];

		if (pBss->WPA.PairCipher[0] != pBss->WPA2.PairCipher[0])
			ap_cipher = Ndis802_11Encryption3Enabled;
		else if ((pBss->WPA.PairCipher[0] == pBss->WPA2.PairCipher[0]) &&
				 (pBss->WPA.PairCipher[1] != pBss->WPA2.PairCipher[1]))
			ap_cipher = Ndis802_11Encryption3Enabled;
		else if ((pBss->WPA.PairCipher[0] == pBss->WPA2.PairCipher[0]) &&
				 (pBss->WPA.PairCipher[1] == pBss->WPA2.PairCipher[1]) &&
				 (pBss->WPA.PairCipher[1] != Ndis802_11WEPDisabled))
			ap_cipher = Ndis802_11Encryption3Enabled;
		else
			ap_cipher = pBss->WPA.PairCipher[0];

		sprintf(SecurityStr, "%s/%s", GetAuthMode((CHAR)ap_auth_mode), GetEncryptType((CHAR)ap_cipher));
	} else if ((Ndis802_11AuthModeWPA <= pBss->AuthMode[0]) &&
		(pBss->AuthMode[0] <= Ndis802_11AuthModeWPA2PSKWPA3SAE)) {
		if (pBss->AuthMode[0] == Ndis802_11AuthModeWPANone) {
			ap_auth_mode = pBss->AuthMode[0];
				ap_cipher = pBss->WPA.PairCipher[0];
		} else if (pBss->AuthMode[1] == Ndis802_11AuthModeOpen) {
			ap_auth_mode = pBss->AuthMode[0];
			if ((ap_auth_mode == Ndis802_11AuthModeWPA) ||
				(ap_auth_mode == Ndis802_11AuthModeWPAPSK)) {
				if (pBss->WPA.PairCipher[1] == Ndis802_11WEPDisabled)
					ap_cipher = pBss->WPA.PairCipher[0];
				else
					ap_cipher = Ndis802_11Encryption4Enabled;
			} else if ((ap_auth_mode == Ndis802_11AuthModeWPA2) ||
					 (ap_auth_mode == Ndis802_11AuthModeWPA2PSK)) {
				if (pBss->WPA2.PairCipher[1] == Ndis802_11WEPDisabled)
					ap_cipher = pBss->WPA2.PairCipher[0];
				else
					ap_cipher = Ndis802_11Encryption4Enabled;
			}
		} else if ((pBss->AuthMode[0] == Ndis802_11AuthModeWPAPSK) ||
				 (pBss->AuthMode[0] == Ndis802_11AuthModeWPA2PSK)) {
			if ((pBss->AuthMode[1] == Ndis802_11AuthModeWPAPSK) ||
				(pBss->AuthMode[1] == Ndis802_11AuthModeWPA2PSK))
				ap_auth_mode = Ndis802_11AuthModeWPA1PSKWPA2PSK;
			else
				ap_auth_mode = pBss->AuthMode[0];

			if (pBss->WPA.PairCipher[0] != pBss->WPA2.PairCipher[0])
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher[0] == pBss->WPA2.PairCipher[0]) &&
					 (pBss->WPA.PairCipher[1] != pBss->WPA2.PairCipher[1]))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher[0] == pBss->WPA2.PairCipher[0]) &&
					 (pBss->WPA.PairCipher[1] == pBss->WPA2.PairCipher[1]) &&
					 (pBss->WPA.PairCipher[1] != Ndis802_11WEPDisabled))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher[0] == pBss->WPA2.PairCipher[0]) &&
					 (pBss->WPA.PairCipher[1] == pBss->WPA2.PairCipher[1]) &&
					 (pBss->WPA.PairCipher[1] == Ndis802_11WEPDisabled))
				ap_cipher = pBss->WPA.PairCipher[0];
		} else if ((pBss->AuthMode[0] == Ndis802_11AuthModeWPA) ||
				 (pBss->AuthMode[0] == Ndis802_11AuthModeWPA2)) {
			if ((pBss->AuthMode[1] == Ndis802_11AuthModeWPA) ||
				(pBss->AuthMode[1] == Ndis802_11AuthModeWPA2))
				ap_auth_mode = Ndis802_11AuthModeWPA1WPA2;
			else
				ap_auth_mode = pBss->AuthMode[0];

			if (pBss->WPA.PairCipher[0] != pBss->WPA2.PairCipher[0])
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher[0] == pBss->WPA2.PairCipher[0]) &&
					 (pBss->WPA.PairCipher[1] != pBss->WPA2.PairCipher[1]))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher[0] == pBss->WPA2.PairCipher[0]) &&
					 (pBss->WPA.PairCipher[1] == pBss->WPA2.PairCipher[1]) &&
					 (pBss->WPA.PairCipher[1] != Ndis802_11WEPDisabled))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher[0] == pBss->WPA2.PairCipher[0]) &&
					 (pBss->WPA.PairCipher[1] == pBss->WPA2.PairCipher[1]) &&
					 (pBss->WPA.PairCipher[1] == Ndis802_11WEPDisabled))
				ap_cipher = pBss->WPA.PairCipher[0];
		}

		sprintf(SecurityStr, "%s/%s", GetAuthMode((CHAR)ap_auth_mode), GetEncryptType((CHAR)ap_cipher));
	} else {
		ap_auth_mode = pBss->AuthMode[0];
		ap_cipher = pBss->WepStatus;
		if (ap_cipher == Ndis802_11WEPDisabled)
			sprintf(SecurityStr, "NONE");
		else if (ap_cipher == Ndis802_11WEPEnabled)
			sprintf(SecurityStr, "WEP");
		else
			sprintf(SecurityStr, "%s/%s", GetAuthMode((CHAR)ap_auth_mode), GetEncryptType((CHAR)ap_cipher));
	}
}
#endif /* DPA_T */

/* 
    ==========================================================================
    Description:
        Get site survey results
	Arguments:
	    pAdapter                    Pointer to our adapter
	    wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
        Usage:
        		1.) UI needs to wait 4 seconds after issue a site survey command
        		2.) iwpriv ra0 get_site_survey
        		3.) UI needs to prepare at least 4096bytes to get the results
    ==========================================================================
*/
#ifdef DPA_T
#define	LINE_LEN	(4+4+33+20+33+9+7+7+3)	// Channel+LEN+SSID+Bssid+Security+Signal+WiressMode+ExtCh+NetworkType
#else
#define	LINE_LEN	(4+33+20+23+9+7+7+3)	/* Channel+SSID+Bssid+Security+Signal+WiressMode+ExtCh+NetworkType*/
#endif

#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
#define	WPS_LINE_LEN	(4+5)	/* WPS+DPID*/
#endif /* WSC_STA_SUPPORT */
#ifdef DOT11R_FT_SUPPORT
#define DOT11R_LINE_LEN	(5+9+10)	/* MDId+FToverDS+RsrReqCap*/
#endif /* DOT11R_FT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
VOID RTMPCommSiteSurveyData(
	IN  RTMP_STRING *msg,
	IN  BSS_ENTRY *pBss,
	IN  UINT32 MsgLen)
{
	INT         Rssi = 0;
	UINT        Rssi_Quality = 0;
	NDIS_802_11_NETWORK_TYPE    wireless_mode;
	CHAR		Ssid[MAX_LEN_OF_SSID +1];
	RTMP_STRING SecurityStr[32] = {0};
	NDIS_802_11_ENCRYPTION_STATUS	ap_cipher = Ndis802_11EncryptionDisabled;
	NDIS_802_11_AUTHENTICATION_MODE	ap_auth_mode = Ndis802_11AuthModeOpen;

		/*Channel*/
		snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-4d", pBss->Channel);

#ifdef DPA_T
	/* SSID Length */
	snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-4d", pBss->SsidLen);
#endif /* DPA_T */

	if (pBss->SsidLen > MAX_LEN_OF_SSID) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Invalid SsidLen (%d)\n"
				, __func__, pBss->SsidLen));
		return;
	}

		/*SSID*/
	NdisZeroMemory(Ssid, (MAX_LEN_OF_SSID +1));
	if (RTMPCheckStrPrintAble((PCHAR)pBss->Ssid, pBss->SsidLen))
		NdisMoveMemory(Ssid, pBss->Ssid, pBss->SsidLen);
	else
	{
		INT idx = 0;
		snprintf(Ssid, sizeof(Ssid), "0x");
		for (idx = 0; (idx < 14) && (idx < pBss->SsidLen); idx++)
			snprintf(Ssid + 2 + (idx*2), sizeof(Ssid)-2-(idx*2)
						, "%02X", (UCHAR)pBss->Ssid[idx]);
	}
		snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-33s", Ssid);
		
		/*BSSID*/
		snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%02x:%02x:%02x:%02x:%02x:%02x   ",
			pBss->Bssid[0], 
			pBss->Bssid[1],
			pBss->Bssid[2], 
			pBss->Bssid[3], 
			pBss->Bssid[4], 
			pBss->Bssid[5]);
	
	/*Security*/
	RTMPZeroMemory(SecurityStr, 32);
#ifdef WPA3_SUPPORT
	APSetCipherAndAuthMode(pBss, ap_cipher, ap_auth_mode, &SecurityStr[0]);
#else
	if ((Ndis802_11AuthModeWPA <= pBss->AuthMode) &&
		(pBss->AuthMode <= Ndis802_11AuthModeWPA1PSKWPA2PSK))
	{
		if (pBss->AuthMode == Ndis802_11AuthModeWPANone)
		{
			ap_auth_mode = pBss->AuthMode;
				ap_cipher = pBss->WPA.PairCipher;
#ifdef DPA_T
			snprintf(SecurityStr, sizeof(SecurityStr), "%s-%s"
			, GetAuthMode((CHAR)ap_auth_mode), GetEncryptType((CHAR)ap_cipher));
#endif /* DPA_T */
		}
		else if (pBss->AuthModeAux == Ndis802_11AuthModeOpen)
		{
			ap_auth_mode = pBss->AuthMode;
			if ((ap_auth_mode == Ndis802_11AuthModeWPA) || 
				(ap_auth_mode == Ndis802_11AuthModeWPAPSK))
			{
				if (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled)
					ap_cipher = pBss->WPA.PairCipher;
				else 
					ap_cipher = Ndis802_11TKIPAESMix;
			}
			else if ((ap_auth_mode == Ndis802_11AuthModeWPA2) || 
					(ap_auth_mode == Ndis802_11AuthModeWPA2PSK)
			) {
				if (pBss->WPA2.PairCipherAux == Ndis802_11WEPDisabled)
					ap_cipher = pBss->WPA2.PairCipher;
				else 
					ap_cipher = Ndis802_11TKIPAESMix;
			}
#ifdef DPA_T
			snprintf(SecurityStr, sizeof(SecurityStr), "%s-%s"
			, GetAuthMode((CHAR)ap_auth_mode), GetEncryptType((CHAR)ap_cipher));
#endif /* DPA_T */
		}
#ifdef DPA_T		
		else
		{
			GetMixedCipherStr(pBss, pBss->AuthMode, &SecurityStr[0]);
			snprintf(SecurityStr, sizeof(SecurityStr), "%s/", SecurityStr);
			GetMixedCipherStr(pBss, pBss->AuthModeAux, &SecurityStr[0]);
		}
#else /* DPA_T */
		else if ((pBss->AuthMode == Ndis802_11AuthModeWPAPSK) || 
				 (pBss->AuthMode == Ndis802_11AuthModeWPA2PSK))
		{
			if ((pBss->AuthModeAux == Ndis802_11AuthModeWPAPSK) ||
				(pBss->AuthModeAux == Ndis802_11AuthModeWPA2PSK))
				ap_auth_mode = Ndis802_11AuthModeWPA1PSKWPA2PSK;
			else
				ap_auth_mode = pBss->AuthMode;
			
			if (pBss->WPA.PairCipher != pBss->WPA2.PairCipher)
				ap_cipher = Ndis802_11TKIPAESMix;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux != pBss->WPA2.PairCipherAux))
				ap_cipher = Ndis802_11TKIPAESMix;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux != Ndis802_11WEPDisabled))
				ap_cipher = Ndis802_11TKIPAESMix;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled))
				ap_cipher = pBss->WPA.PairCipher;
		}
		else if ((pBss->AuthMode == Ndis802_11AuthModeWPA) || 
				 (pBss->AuthMode == Ndis802_11AuthModeWPA2))
		{
			if ((pBss->AuthModeAux == Ndis802_11AuthModeWPA) ||
				(pBss->AuthModeAux == Ndis802_11AuthModeWPA2))
				ap_auth_mode = Ndis802_11AuthModeWPA1WPA2;
			else
				ap_auth_mode = pBss->AuthMode;
			
			if (pBss->WPA.PairCipher != pBss->WPA2.PairCipher)
				ap_cipher = Ndis802_11TKIPAESMix;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux != pBss->WPA2.PairCipherAux))
				ap_cipher = Ndis802_11TKIPAESMix;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux != Ndis802_11WEPDisabled))
				ap_cipher = Ndis802_11TKIPAESMix;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled))
				ap_cipher = pBss->WPA.PairCipher;
		}

		snprintf(SecurityStr, sizeof(SecurityStr), "%s/%s"
					, GetAuthMode((CHAR)ap_auth_mode)
					, GetEncryptType((CHAR)ap_cipher));
#endif /* !DPA_T */
	}			
	else
	{
		ap_auth_mode = pBss->AuthMode;
		ap_cipher = pBss->WepStatus;		
		if (ap_cipher == Ndis802_11WEPDisabled)
			snprintf(SecurityStr, sizeof(SecurityStr), "NONE");
		else if (ap_cipher == Ndis802_11WEPEnabled)
			snprintf(SecurityStr, sizeof(SecurityStr), "WEP");
		else
			snprintf(SecurityStr, sizeof(SecurityStr), "%s/%s"
						, GetAuthMode((CHAR)ap_auth_mode)
						, GetEncryptType((CHAR)ap_cipher));
	}
#endif /* WPA3_SUPPORT */
#ifdef DPA_T
	snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-33s", SecurityStr);
#else /* DPA_T */
	snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-23s", SecurityStr);
#endif /* !DPA_T */

		/* Rssi*/
		Rssi = (INT)pBss->Rssi;
		if (Rssi >= -50)
			Rssi_Quality = 100;
		else if (Rssi >= -80)    /* between -50 ~ -80dbm*/
			Rssi_Quality = (UINT)(24 + ((Rssi + 80) * 26)/10);
		else if (Rssi >= -90)   /* between -80 ~ -90dbm*/
			Rssi_Quality = (UINT)(((Rssi + 90) * 26)/10);
		else    /* < -84 dbm*/
			Rssi_Quality = 0;
		snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-9d", Rssi_Quality);
		
		/* Wireless Mode*/
		wireless_mode = NetworkTypeInUseSanity(pBss);
		if (wireless_mode == Ndis802_11FH ||
			wireless_mode == Ndis802_11DS)
			snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", "11b");
		else if (wireless_mode == Ndis802_11OFDM5)
			snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", "11a");
		else if (wireless_mode == Ndis802_11OFDM5_N)
			snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", "11a/n");
		else if (wireless_mode == Ndis802_11OFDM24)
			snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", "11b/g");
		else if (wireless_mode == Ndis802_11OFDM24_N)
			snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", "11b/g/n");
		else
			snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", "unknown");

		/* Ext Channel*/
		if (pBss->AddHtInfoLen > 0)
		{
			if (pBss->AddHtInfo.AddHtInfo.ExtChanOffset == EXTCHA_ABOVE)
				snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", " ABOVE");
			else if (pBss->AddHtInfo.AddHtInfo.ExtChanOffset == EXTCHA_BELOW)
				snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", " BELOW");
			else
				snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", " NONE");
		}
		else
		{
			snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-7s", " NONE");
		}
		
		/*Network Type		*/
		if (pBss->BssType == BSS_ADHOC)
			snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-3s", " Ad");
		else
			snprintf(msg+strlen(msg), MsgLen-strlen(msg), "%-3s", " In");

		snprintf(msg+strlen(msg), MsgLen-strlen(msg), "\n");
	
	return;
}

#if defined (AP_SCAN_SUPPORT) || defined (CONFIG_STA_SUPPORT)
VOID RTMPIoctlGetSiteSurvey(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq)
{
	RTMP_STRING *msg;
	INT 		i=0;	 
	INT			WaitCnt;
	ULONG		Status=0;	
    INT         max_len = LINE_LEN;		
	BSS_ENTRY *pBss;
	UINT32 TotalLen, BufLen = IW_SCAN_MAX_DATA;

#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
	max_len += WPS_LINE_LEN;
#endif /* WSC_STA_SUPPORT */
#ifdef DOT11R_FT_SUPPORT
	max_len += DOT11R_LINE_LEN;
#endif /* DOT11R_FT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	TotalLen = sizeof(CHAR)*((MAX_LEN_OF_BSS_TABLE)*max_len) + 100;

	if (wrq->u.data.length == 0)
		BufLen = IW_SCAN_MAX_DATA;	
	else
		BufLen = wrq->u.data.length;

	os_alloc_mem(NULL, (PUCHAR *)&msg, TotalLen);

	if (msg == NULL)
	{   
		DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlGetSiteSurvey - msg memory alloc fail.\n"));
		return;
	}

	memset(msg, 0 , TotalLen);
	sprintf(msg,"%s","\n");

#ifdef DPA_T
	sprintf(msg+strlen(msg),"%-4s%-4s%-33s%-20s%-33s%-9s%-7s%-7s%-3s\n",
		"Ch", "Len", "SSID", "BSSID", "Security", "Siganl(%)", "W-Mode", " ExtCH"," NT");	
#else /* DPA_T */
	sprintf(msg+strlen(msg),"%-4s%-33s%-20s%-23s%-9s%-7s%-7s%-3s\n",
	    "Ch", "SSID", "BSSID", "Security", "Siganl(%)", "W-Mode", " ExtCH"," NT");	
#endif /* !DPA_T */

#ifdef WSC_INCLUDED
	sprintf(msg+strlen(msg)-1,"%-4s%-5s\n", " WPS", " DPID");
#endif /* WSC_INCLUDED */

#ifdef CONFIG_STA_SUPPORT
#ifdef DOT11R_FT_SUPPORT
	sprintf(msg+strlen(msg)-1,"%-5s%-9s%-10s\n", " MDId", " FToverDS", " RsrReqCap");
#endif /* DOT11R_FT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	WaitCnt = 0;
#ifdef CONFIG_STA_SUPPORT
	pAdapter->StaCfg.bSkipAutoScanConn = TRUE;
#endif /* CONFIG_STA_SUPPORT */

#ifdef DPA_S
	OS_WAIT(100);;	
#else /* DPA_S */
	while ((ScanRunning(pAdapter) == TRUE) && (WaitCnt++ < 200))
		OS_WAIT(500);	
#endif /* !DPA_S */

	for(i=0; i<pAdapter->ScanTab.BssNr ;i++)
	{
		pBss = &pAdapter->ScanTab.BssEntry[i];
		
		if( pBss->Channel==0)
			break;

		if((strlen(msg)+100 ) >= BufLen)
			break;

		if (pBss->SsidLen > MAX_LEN_OF_SSID) {
			DBGPRINT(RT_DEBUG_WARN,
					("%s - Invalid SsidLen %d\n", __func__, pBss->SsidLen));
			break;
		}

		RTMPCommSiteSurveyData(msg, pBss, TotalLen);
		
#ifdef WSC_INCLUDED
        /*WPS*/
        if (pBss->WpsAP & 0x01)
			sprintf(msg+strlen(msg)-1,"%-4s", " YES");
		else
			sprintf(msg+strlen(msg)-1,"%-4s", "  NO");

		if (pBss->WscDPIDFromWpsAP == DEV_PASS_ID_PIN)
			sprintf(msg+strlen(msg),"%-5s\n", " PIN");
		else if (pBss->WscDPIDFromWpsAP == DEV_PASS_ID_PBC)
			sprintf(msg+strlen(msg),"%-5s\n", " PBC");
		else
			sprintf(msg+strlen(msg),"%-5s\n", " ");
#endif /* WSC_INCLUDED */

#ifdef CONFIG_STA_SUPPORT
#ifdef DOT11R_FT_SUPPORT
		if (pBss->bHasMDIE)
		{
			sprintf(msg+strlen(msg)-1," %02x%02x", pBss->FT_MDIE.MdId[0], pBss->FT_MDIE.MdId[1]);
			if (pBss->FT_MDIE.FtCapPlc.field.FtOverDs)
				sprintf(msg+strlen(msg),"%-9s", " TRUE");
			else
				sprintf(msg+strlen(msg),"%-9s", " FALSE");
			if (pBss->FT_MDIE.FtCapPlc.field.RsrReqCap)
				sprintf(msg+strlen(msg),"%-10s\n", " TRUE");
			else
				sprintf(msg+strlen(msg),"%-10s\n", " FALSE");
		}
#endif /* DOT11R_FT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
	}

#ifdef CONFIG_STA_SUPPORT
	pAdapter->StaCfg.bSkipAutoScanConn = FALSE;
#endif /* CONFIG_STA_SUPPORT */
	wrq->u.data.length = (UINT16)strlen(msg);
	Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlGetSiteSurvey - wrq->u.data.length = %d\n", wrq->u.data.length));
	os_free_mem(NULL, (PUCHAR)msg);	
}
#endif


VOID RTMPIoctlGetMacTableStaInfo(
	IN PRTMP_ADAPTER pAd, 
	IN RTMP_IOCTL_INPUT_STRUCT *wrq)
{
	INT i;
	RT_802_11_MAC_TABLE *pMacTab = NULL;
	PRT_802_11_MAC_ENTRY pDst;
	MAC_TABLE_ENTRY *pEntry;

	/* allocate memory */
	os_alloc_mem(NULL, (UCHAR **)&pMacTab, sizeof(RT_802_11_MAC_TABLE));
	if (pMacTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		return;
	}

	NdisZeroMemory(pMacTab, sizeof(RT_802_11_MAC_TABLE));
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		pEntry = &(pAd->MacTab.Content[i]);
		if (IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst == SST_ASSOC))
		{
			pDst = &pMacTab->Entry[pMacTab->Num];

			pDst->ApIdx = pEntry->func_tb_idx;
			COPY_MAC_ADDR(pDst->Addr, &pEntry->Addr);
			pDst->Aid = (UCHAR)pEntry->Aid;
			pDst->Psm = pEntry->PsMode;

#ifdef DOT11_N_SUPPORT
			pDst->MimoPs = pEntry->MmpsMode;
#endif /* DOT11_N_SUPPORT */

			/* Fill in RSSI per entry*/
			pDst->AvgRssi0 = pEntry->RssiSample.AvgRssi[0];
			pDst->AvgRssi1 = pEntry->RssiSample.AvgRssi[1];
			pDst->AvgRssi2 = pEntry->RssiSample.AvgRssi[2];

			/* the connected time per entry*/
			pDst->ConnectedTime = pEntry->StaConnectTime;
			pDst->TxRate.word = pEntry->HTPhyMode.word;

			pDst->LastRxRate = pEntry->LastRxRate;
#if defined (RT2883) || defined (RT3883)
			if (IS_RT2883(pAd) || IS_RT3883(pAd))
			{
				pDst->StreamSnr[0] = pEntry->BF_SNR[0];
				pDst->StreamSnr[1] = pEntry->BF_SNR[1];
				pDst->StreamSnr[2] = pEntry->BF_SNR[2];
#ifdef TXBF_SUPPORT
				if (pAd->chipCap.FlgHwTxBfCap)
				{
					pDst->SoundingRespSnr[0] = pEntry->sndg0Snr0;
					pDst->SoundingRespSnr[1] = pEntry->sndg0Snr1;
					pDst->SoundingRespSnr[2] = pEntry->sndg0Snr2;
				}
#endif /* TXBF_SUPPORT */
/*				pDst->TxPER = pEntry->SaveTxPER; */
			}
#endif /* defined (RT2883) || defined (RT3883) */
									
			pMacTab->Num += 1;
		}
	}

	wrq->u.data.length = sizeof(RT_802_11_MAC_TABLE);
	if (copy_to_user(wrq->u.data.pointer, pMacTab, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: copy_to_user() fail\n", __FUNCTION__));
	}

	if (pMacTab != NULL)
		os_free_mem(NULL, pMacTab);
}


#define	MAC_LINE_LEN	(1+14+4+4+4+4+10+10+10+6+6)	/* "\n"+Addr+aid+psm+datatime+rxbyte+txbyte+current tx rate+last tx rate+"\n" */
VOID RTMPIoctlGetMacTable(
	IN PRTMP_ADAPTER pAd, 
	IN RTMP_IOCTL_INPUT_STRUCT *wrq)
{
	INT i;
/*	RT_802_11_MAC_TABLE MacTab;*/
	RT_802_11_MAC_TABLE *pMacTab = NULL;
	RT_802_11_MAC_ENTRY *pDst;
	MAC_TABLE_ENTRY *pEntry;
#ifdef DPA_S
	STA_TR_ENTRY *tr_entry;
#endif /* DPA_S */
	char *msg;
	int msg_buf_len;
#ifdef DPA_S //yiwei
	UCHAR	index;
	BOOLEAN	bFound;
#endif /* DPA_S */

	/* allocate memory */
	os_alloc_mem(NULL, (UCHAR **)&pMacTab, sizeof(RT_802_11_MAC_TABLE));
	if (pMacTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		return;
	}

	NdisZeroMemory(pMacTab, sizeof(RT_802_11_MAC_TABLE));
	pMacTab->Num = 0;
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		pEntry = &(pAd->MacTab.Content[i]);
#ifdef DPA_S
		tr_entry = &(pAd->MacTab.tr_entry[i]);

		if ((IS_P2P_GO_ENTRY(pEntry) || IS_P2P_CLI_ENTRY(pEntry)) 
			&& (tr_entry->PortSecured == WPA_802_1X_PORT_SECURED))
#else /* DPA_S */
		if (IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst == SST_ASSOC))
#endif /* !DPA_S */
		{
			pDst = &pMacTab->Entry[pMacTab->Num];

#ifdef DPA_S
			bFound = FALSE;
			if (IS_P2P_GO_ENTRY(pEntry) || IS_P2P_CLI_ENTRY(pEntry))
			{
				index = P2pGroupTabSearch(pAd, pEntry->Addr);
				if(index != P2P_NOT_FOUND)
					bFound = TRUE;
			}

			if(bFound)
			{
				RTMPMoveMemory(&(pDst->DeviceName[0]), &pAd->P2pTable.Client[index].DeviceName[0], pAd->P2pTable.Client[index].DeviceNameLen);
				pDst->DeviceNameLen = pAd->P2pTable.Client[index].DeviceNameLen;
				pDst->DeviceName[pDst->DeviceNameLen]=0x0;
			}
			else
			{
				pDst->DeviceNameLen = 0;
			}
#endif /* DPA_S */

			pDst->ApIdx = (UCHAR)pEntry->func_tb_idx;
			COPY_MAC_ADDR(pDst->Addr, &pEntry->Addr);
			pDst->Aid = (UCHAR)pEntry->Aid;
			pDst->Psm = pEntry->PsMode;
#ifdef DOT11_N_SUPPORT
			pDst->MimoPs = pEntry->MmpsMode;
#endif /* DOT11_N_SUPPORT */

			/* Fill in RSSI per entry*/
			pDst->AvgRssi0 = pEntry->RssiSample.AvgRssi[0];
			pDst->AvgRssi1 = pEntry->RssiSample.AvgRssi[1];
			pDst->AvgRssi2 = pEntry->RssiSample.AvgRssi[2];

			/* the connected time per entry*/
			pDst->ConnectedTime = pEntry->StaConnectTime;
			pDst->TxRate.word = pEntry->HTPhyMode.word;
									
#ifdef RTMP_RBUS_SUPPORT
			pDst->LastRxRate = pEntry->LastRxRate;
#if defined(RT2883) || defined(RT3883)
			if (IS_RT2883(pAd) || IS_RT3883(pAd))
			{
				pDst->StreamSnr[0] = pEntry->BF_SNR[0];
				pDst->StreamSnr[1] = pEntry->BF_SNR[1];
				pDst->StreamSnr[2] = pEntry->BF_SNR[2];
#ifdef TXBF_SUPPORT
				pDst->SoundingRespSnr[0] = pEntry->sndg0Snr0;
				pDst->SoundingRespSnr[1] = pEntry->sndg0Snr1;
				pDst->SoundingRespSnr[2] = pEntry->sndg0Snr2;
#endif /* TXBF_SUPPORT */
			}
#endif /* defined(RT2883) || defined(RT3883) */
/*			pDst->TxPER = pEntry->SaveTxPER; */
#endif /* RTMP_RBUS_SUPPORT */
									
			pMacTab->Num += 1;
		}
	}

	wrq->u.data.length = sizeof(RT_802_11_MAC_TABLE);
	if (copy_to_user(wrq->u.data.pointer, pMacTab, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: copy_to_user() fail\n", __FUNCTION__));
	}

	msg_buf_len = MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN;
	os_alloc_mem(NULL, (UCHAR **)&msg, sizeof(CHAR)*msg_buf_len);
	if (msg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Alloc memory failed\n", __FUNCTION__));
		goto LabelOK;
	}
	memset(msg, 0, msg_buf_len);
	snprintf(msg, msg_buf_len, "%s", "\n");
	snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
		, "%-14s%-4s%-4s%-4s%-4s%-6s%-6s%-10s%-10s%-10s\n"
		, "MAC", "AP",  "AID", "PSM", "AUTH", "CTxR", "LTxR", "LDT", "RxB", "TxB");
	
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[i];
#ifdef DPA_S
		STA_TR_ENTRY *tr_entry = &(pAd->MacTab.tr_entry[i]);

		if ((IS_P2P_GO_ENTRY(pEntry) || IS_P2P_CLI_ENTRY(pEntry)) 
			&& (tr_entry->PortSecured == WPA_802_1X_PORT_SECURED))
#else /* DPA_S */
		if (IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst == SST_ASSOC))
#endif /* !DPA_S */
		{
			if((strlen(msg)+MAC_LINE_LEN ) >= (MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN) )
				break;	
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
				, "%02x%02x%02x%02x%02x%02x  ", PRINT_MAC(pEntry->Addr));
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-4d"
				, (int)pEntry->func_tb_idx);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-4d"
				, (int)pEntry->Aid);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-4d"
				, (int)pEntry->PsMode);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-4d"
				, (int)pEntry->AuthState);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-6d"
				, RateIdToMbps[pAd->MacTab.Content[i].CurrTxRate]);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-6d"
				, 0/*RateIdToMbps[pAd->MacTab.Content[i].HTPhyMode.word]*/);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-10d"
				, 0/*pAd->MacTab.Content[i].HSCounter.LastDataPacketTime*/);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-10d"
				, 0/*pAd->MacTab.Content[i].HSCounter.TotalRxByteCount*/);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-10d\n"
				, 0/*pAd->MacTab.Content[i].HSCounter.TotalTxByteCount*/);

		}
	} 
	/* for compatible with old API just do the printk to console*/
#if 0
	wrq->u.data.length = strlen(msg);
	copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);
#endif

	DBGPRINT(RT_DEBUG_TRACE, ("%s", msg));
	os_free_mem(NULL, msg);

LabelOK:
	if (pMacTab != NULL)
		os_free_mem(NULL, pMacTab);
}

#if defined(INF_AR9) || defined(BB_SOC)
#if defined(AR9_MAPI_SUPPORT) || defined(BB_SOC)
#ifdef CONFIG_AP_SUPPORT
VOID RTMPAR9IoctlGetMacTable(
	IN PRTMP_ADAPTER pAd, 
	IN RTMP_IOCTL_INPUT_STRUCT *wrq)
{
	INT i;
	char *msg;
	int msg_buf_len;

	msg_buf_len = MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN;
	os_alloc_mem(NULL, (UCHAR **)&msg, sizeof(CHAR)*msg_buf_len);
	if (msg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Alloc memory failed\n", __FUNCTION__));
		return;
	}
	memset(msg, 0, msg_buf_len);
	snprintf(msg, msg_buf_len, "%s", "\n");
	snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
		, "%-14s%-4s%-4s%-4s%-4s%-6s%-6s%-10s%-10s%-10s\n"
		, "MAC", "AP",  "AID", "PSM", "AUTH", "CTxR", "LTxR", "LDT", "RxB", "TxB");
	
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
		if (IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst == SST_ASSOC))
		{
			if((strlen(msg)+MAC_LINE_LEN ) >= (MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN) )
				break;	
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
				, "%02x%02x%02x%02x%02x%02x  "
				, pEntry->Addr[0], pEntry->Addr[1], pEntry->Addr[2]
				, pEntry->Addr[3], pEntry->Addr[4], pEntry->Addr[5]);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-4d"
				, (int)pEntry->func_tb_idx);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-4d"
				, (int)pEntry->Aid);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-4d"
				, (int)pEntry->PsMode);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-4d"
				, (int)pEntry->AuthState);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-6d"
				, RateIdToMbps[pAd->MacTab.Content[i].CurrTxRate]);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-6d"
				, 0/*RateIdToMbps[pAd->MacTab.Content[i].HTPhyMode.word]*/);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-10d"
				, 0/*pAd->MacTab.Content[i].HSCounter.LastDataPacketTime*/);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-10d"
				, 0/*pAd->MacTab.Content[i].HSCounter.TotalRxByteCount*/);
			snprintf(msg+strlen(msg), msg_buf_len-strlen(msg), "%-10d\n"
				, 0/*pAd->MacTab.Content[i].HSCounter.TotalTxByteCount*/);

		}
	} 
	/* for compatible with old API just do the printk to console*/
	wrq->u.data.length = strlen(msg);
	if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s", msg));
	}

	os_free_mem(NULL, msg);
}

VOID RTMPIoctlGetSTAT2(
	IN PRTMP_ADAPTER pAd, 
	IN RTMP_IOCTL_INPUT_STRUCT *wrq)
{
	char *msg;
	int msg_buf_len;
	BSS_STRUCT *pMbss;
	INT apidx;

	msg_buf_len = pAd->ApCfg.BssidNum*(14*128);
	os_alloc_mem(NULL, (UCHAR **)&msg, sizeof(CHAR)*msg_buf_len);
	if (msg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Alloc memory failed\n", __FUNCTION__));
		return;
	}
	memset(msg, 0, msg_buf_len);
	snprintf(msg, msg_buf_len, "%s", "\n");
	
	for (apidx=0; apidx<pAd->ApCfg.BssidNum; apidx++)
	{
		pMbss=&pAd->ApCfg.MBSSID[apidx];
		
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "ra%d\n", apidx);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			 , "bytesTx = %ld\n", (pMbss->TransmittedByteCount));
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "bytesRx = %ld\n", (pMbss->ReceivedByteCount));
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "pktsTx = %ld\n", pMbss->TxCount);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "pktsRx = %ld\n", pMbss->RxCount);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "errorsTx = %ld\n", pMbss->TxErrorCount);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "errorsRx = %ld\n", pMbss->RxErrorCount);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "discardPktsTx = %ld\n", pMbss->TxDropCount);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "discardPktsRx = %ld\n", pMbss->RxDropCount);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "ucPktsTx = %ld\n", pMbss->ucPktsTx);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "ucPktsRx = %ld\n", pMbss->ucPktsRx);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "mcPktsTx = %ld\n", pMbss->mcPktsTx);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "mcPktsRx = %ld\n", pMbss->mcPktsRx);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "bcPktsTx = %ld\n", pMbss->bcPktsTx);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "bcPktsRx = %ld\n", pMbss->bcPktsRx);
		
	}

	wrq->u.data.length = strlen(msg);
	if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s", msg));
	}

	os_free_mem(NULL, msg);
}


VOID RTMPIoctlGetRadioDynInfo(
	IN PRTMP_ADAPTER pAd, 
	IN RTMP_IOCTL_INPUT_STRUCT *wrq)
{
	char *msg;
	int msg_buf_len;
	BSS_STRUCT *pMbss;
	INT status,bandwidth,ShortGI;
	struct wifi_dev *wdev;

	msg_buf_len = 4069;
	os_alloc_mem(NULL, (UCHAR **)&msg, sizeof(CHAR)*(msg_buf_len));
	if (msg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Alloc memory failed\n", __FUNCTION__));
		return;
	}
	memset(msg, 0, msg_buf_len);
	snprintf(msg, msg_buf_len, "%s", "\n");
	

		pMbss=&pAd->ApCfg.MBSSID[0];
		wdev = &pMbss->wdev;
		if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))
			status = 0;
		else
			status = 1;

		if(pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)
			bandwidth = 1;
		else
			bandwidth = 0;

		if(pAd->CommonCfg.RegTransmitSetting.field.ShortGI == GI_800)
			ShortGI = 1;
		else
			ShortGI = 0;

		
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "status = %d\n", status);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "channelsInUse = %d\n", pAd->ChannelListNum);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "channel = %d\n", pAd->CommonCfg.Channel);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "chanWidth = %d\n", bandwidth);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "guardIntvl = %d\n", ShortGI);
		snprintf(msg+strlen(msg), msg_buf_len-strlen(msg)
			, "MCS = %d\n", wdev->DesiredTransmitSetting.field.MCS);
		
	wrq->u.data.length = strlen(msg);

	if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s", msg));
	}

	os_free_mem(NULL, msg);
}
#endif/*CONFIG_AP_SUPPORT*/
#endif/*AR9_MAPI_SUPPORT*/
#endif/* INF_AR9 */

#ifdef DOT11_N_SUPPORT
INT	Set_BASetup_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    UCHAR mac[6], tid;
	RTMP_STRING *token;
	RTMP_STRING sepValue[] = ":", DASH = '-';
	INT i;
    MAC_TABLE_ENTRY *pEntry;

/*
	The BASetup inupt string format should be xx:xx:xx:xx:xx:xx-d, 
		=>The six 2 digit hex-decimal number previous are the Mac address, 
		=>The seventh decimal number is the tid value.
*/
	/*DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));*/
	
	if(strlen(arg) < 19)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.*/
		return FALSE;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		tid = (UCHAR) simple_strtol((token+1), 0, 10);
		/* tid is 0 ~ 7; Or kernel will crash in BAOriSessionSetUp() */
		if (tid > (NUM_OF_TID-1))
			return FALSE;
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return FALSE;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x\n", 
								mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], tid));

	    pEntry = MacTableLookup(pAd, (PUCHAR) mac);

    	if (pEntry) {
        	DBGPRINT(RT_DEBUG_OFF, ("\nSetup BA Session: Tid = %d\n", tid));
	        BAOriSessionSetUp(pAd, pEntry, tid, 0, 100, TRUE);
    	}

		return TRUE;
	}

	return FALSE;

}

INT	Set_BADecline_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG bBADecline;

	bBADecline = simple_strtol(arg, 0, 10);

	if (bBADecline == 0)
	{
		pAd->CommonCfg.bBADecline = FALSE;
	}
	else if (bBADecline == 1)
	{
		pAd->CommonCfg.bBADecline = TRUE;
	}
	else 
	{
		return FALSE; /*Invalid argument*/
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_BADecline_Proc::(BADecline=%d)\n", pAd->CommonCfg.bBADecline));

	return TRUE;
}

INT	Set_BAOriTearDown_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    UCHAR mac[6], tid;
	RTMP_STRING *token;
	RTMP_STRING sepValue[] = ":", DASH = '-';
	INT i, ret;
	MAC_TABLE_ENTRY *pEntry;
	LONG ret_value;

    /*DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));*/
/*
	The BAOriTearDown inupt string format should be xx:xx:xx:xx:xx:xx-d, 
		=>The six 2 digit hex-decimal number previous are the Mac address, 
		=>The seventh decimal number is the tid value.
*/
    if(strlen(arg) < 19)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.*/
		return FALSE;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		ret = kstrtol((token+1), 10, &ret_value);
		tid = (UCHAR)ret_value;

		/* tid will be 0 ~ 7; Or kernel will crash in BAOriSessionTearDown() */
		if (tid > (NUM_OF_TID-1))
			return FALSE;
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return FALSE;

	    DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x", 
								PRINT_MAC(mac), tid));

	    pEntry = MacTableLookup(pAd, (PUCHAR) mac);

	    if (pEntry) {
	        DBGPRINT(RT_DEBUG_OFF, ("\nTear down Ori BA Session: Tid = %d\n", tid));
	        BAOriSessionTearDown(pAd, pEntry->wcid, tid, FALSE, TRUE);
	    }

		return TRUE;
	}

	return FALSE;

}

INT	Set_BARecTearDown_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    UCHAR mac[6], tid;
	RTMP_STRING *token;
	RTMP_STRING sepValue[] = ":", DASH = '-';
	INT i, ret;
	MAC_TABLE_ENTRY *pEntry;
	LONG ret_value;

    /*DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));*/
/*
	The BARecTearDown inupt string format should be xx:xx:xx:xx:xx:xx-d, 
		=>The six 2 digit hex-decimal number previous are the Mac address, 
		=>The seventh decimal number is the tid value.
*/
    if(strlen(arg) < 19)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.*/
		return FALSE;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		ret = kstrtol((token+1), 10, &ret_value);
		tid = (UCHAR)ret_value;
		/* tid will be 0 ~ 7; Or kernel will crash in BARecSessionTearDown() */
		if (tid > (NUM_OF_TID-1))
			return FALSE;
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return FALSE;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x", 
								PRINT_MAC(mac), tid));

		pEntry = MacTableLookup(pAd, (PUCHAR) mac);

		if (pEntry) {
		    DBGPRINT(RT_DEBUG_OFF, ("\nTear down Rec BA Session: Tid = %d\n", tid));
		    BARecSessionTearDown(pAd, pEntry->wcid, tid, FALSE);
		}

		return TRUE;
	}

	return FALSE;

}

INT	Set_HtBw_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG HtBw;

	HtBw = simple_strtol(arg, 0, 10);

	if (HtBw == BW_40)
		pAd->CommonCfg.RegTransmitSetting.field.BW  = BW_40;
	else if (HtBw == BW_20)
		pAd->CommonCfg.RegTransmitSetting.field.BW  = BW_20;
	else
		return FALSE;  /*Invalid argument */

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtBw_Proc::(HtBw=%d)\n", pAd->CommonCfg.RegTransmitSetting.field.BW));

	return TRUE;
}


INT	Set_HtMcs_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
#ifdef CONFIG_AP_SUPPORT    
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
#endif /* CONFIG_AP_SUPPORT */	
#ifdef CONFIG_STA_SUPPORT    
	BOOLEAN bAutoRate = FALSE;
#endif /* CONFIG_STA_SUPPORT */
	UCHAR HtMcs = MCS_AUTO, Mcs_tmp, ValidMcs = 15;
#ifdef DOT11_VHT_AC
	RTMP_STRING *mcs_str, *ss_str;
	UCHAR ss = 0, mcs = 0;
#endif /* DOT11_VHT_AC */
	struct wifi_dev *wdev;
	INT ret;
	LONG ret_value;

#ifdef DOT11_VHT_AC
	ss_str = arg;
	if ((mcs_str = rtstrchr(arg, ':'))!= NULL)
	{
		*mcs_str = 0;
		mcs_str++;

		DBGPRINT(RT_DEBUG_TRACE, ("%s(): ss_str=%s, mcs_str=%s\n",
						__FUNCTION__, ss_str, mcs_str));

		if (strlen(ss_str) && strlen(mcs_str)) {
			ret = kstrtol(mcs_str, 10, &ret_value);
			mcs = (UCHAR)ret_value;

			ret = kstrtol(ss_str, 10, &ret_value);
			ss = (UCHAR)ret_value;

			if ((ss <= pAd->CommonCfg.TxStream) && (mcs <= 7))
				HtMcs = ((ss - 1) <<4) | mcs;
			else {
				HtMcs = MCS_AUTO;
				ss = 0;
			}
			DBGPRINT(RT_DEBUG_TRACE, ("%s(): %dSS-MCS%d, Auto=%s\n",
						__FUNCTION__, ss, mcs,
						(HtMcs == MCS_AUTO && ss == 0) ? "TRUE" : "FALSE"));
			Set_FixedTxMode_Proc(pAd, "VHT");
		}
	}
	else
#endif /* DOT11_VHT_AC */
	{
#ifdef DOT11N_SS3_SUPPORT
		if (pAd->CommonCfg.TxStream >= 3)
			ValidMcs = 23;
#endif /* DOT11N_SS3_SUPPORT */

		ret = kstrtol(arg, 10, &ret_value);
		Mcs_tmp = (UCHAR)ret_value;
		if (Mcs_tmp <= ValidMcs || Mcs_tmp == 32)
			HtMcs = Mcs_tmp;
		else
			HtMcs = MCS_AUTO;
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		wdev = &pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev;

		wdev->DesiredTransmitSetting.field.MCS = HtMcs;
		DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMcs_Proc::(HtMcs=%d) for ra%d\n", 
				wdev->DesiredTransmitSetting.field.MCS, pObj->ioctl_if));
	}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		wdev = &pAd->StaCfg.wdev;
		
		wdev->DesiredTransmitSetting.field.MCS = HtMcs;
		wdev->bAutoTxRateSwitch = (HtMcs == MCS_AUTO) ? TRUE:FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMcs_Proc::(HtMcs=%d, bAutoTxRateSwitch = %d)\n", 
					wdev->DesiredTransmitSetting.field.MCS, wdev->bAutoTxRateSwitch));

		if ((!WMODE_CAP_N(pAd->CommonCfg.PhyMode)) ||
			(pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.MODE < MODE_HTMIX))
		{
			if ((wdev->DesiredTransmitSetting.field.MCS != MCS_AUTO) && 
				(HtMcs <= 3) && 
				(wdev->DesiredTransmitSetting.field.FixedTxMode == FIXED_TXMODE_CCK))
			{
				RTMPSetDesiredRates(pAd, (LONG) (RateIdToMbps[HtMcs] * 1000000));
			}
			else if ((wdev->DesiredTransmitSetting.field.MCS != MCS_AUTO) && 
					(HtMcs <= 7) &&
	            			(wdev->DesiredTransmitSetting.field.FixedTxMode == FIXED_TXMODE_OFDM))
			{
				RTMPSetDesiredRates(pAd, (LONG) (RateIdToMbps[HtMcs+4] * 1000000));
			}
			else
				bAutoRate = TRUE;

			if (bAutoRate)
			{
				wdev->DesiredTransmitSetting.field.MCS = MCS_AUTO;
				RTMPSetDesiredRates(pAd, -1);
			}
	        DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMcs_Proc::(FixedTxMode=%d)\n",
					wdev->DesiredTransmitSetting.field.FixedTxMode));
		}
        if (ADHOC_ON(pAd))
            return TRUE;
	}
#endif /* CONFIG_STA_SUPPORT */

	SetCommonHT(pAd);

#ifdef WFA_VHT_PF
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		INT idx;
			
		NdisAcquireSpinLock(&pAd->MacTabLock);
		for (idx = 1; idx < MAX_LEN_OF_MAC_TABLE; idx++)
		{
			MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[idx];

			if (IS_ENTRY_CLIENT(pEntry) && (pEntry->func_tb_idx == pObj->ioctl_if)) {
				if ((HtMcs == MCS_AUTO) &&  ss == 0) {
					UCHAR TableSize = 0;

					pEntry->bAutoTxRateSwitch = TRUE;
					
					MlmeSelectTxRateTable(pAd, pEntry, &pEntry->pTable, &TableSize, &pEntry->CurrTxRateIndex);
					MlmeNewTxRate(pAd, pEntry);

#ifdef NEW_RATE_ADAPT_SUPPORT
					if (! ADAPT_RATE_TABLE(pEntry->pTable))
#endif /* NEW_RATE_ADAPT_SUPPORT */
						pEntry->HTPhyMode.field.ShortGI = GI_800;
				}
				else
				{
					pEntry->HTPhyMode.field.MCS = pMbss->HTPhyMode.field.MCS;
					pEntry->bAutoTxRateSwitch = FALSE;
			
					/* If the legacy mode is set, overwrite the transmit setting of this entry. */
					RTMPUpdateLegacyTxSetting((UCHAR)pMbss->DesiredTransmitSetting.field.FixedTxMode, pEntry);
				}
			}
		}
		NdisReleaseSpinLock(&pAd->MacTabLock);
	}
#endif /* CONFIG_AP_SUPPORT */
#endif /* WFA_VHT_PF */

	return TRUE;
}

INT	Set_HtGi_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG HtGi;

	HtGi = simple_strtol(arg, 0, 10);
		
	if ( HtGi == GI_400)			
		pAd->CommonCfg.RegTransmitSetting.field.ShortGI = GI_400;
	else if ( HtGi == GI_800 )
		pAd->CommonCfg.RegTransmitSetting.field.ShortGI = GI_800;
	else 
		return FALSE; /* Invalid argument */

	SetCommonHT(pAd);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtGi_Proc::(ShortGI=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.ShortGI));

	return TRUE;
}


INT	Set_HtTxBASize_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR Size;
	INT ret;
	LONG ret_value;

	ret = kstrtol(arg, 10, &ret_value);
	Size = (UCHAR)ret_value;
		
	if (Size <=0 || Size >=64)
	{
		Size = 8;
	}
	pAd->CommonCfg.TxBASize = Size-1;
	DBGPRINT(RT_DEBUG_ERROR, ("Set_HtTxBASize ::(TxBASize= %d)\n", Size));

	return TRUE;
}

INT	Set_HtDisallowTKIP_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
		
	if (Value == 1)
	{
		pAd->CommonCfg.HT_DisallowTKIP = TRUE;
	}
	else
	{
		pAd->CommonCfg.HT_DisallowTKIP = FALSE;
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtDisallowTKIP_Proc ::%s\n", 
				(pAd->CommonCfg.HT_DisallowTKIP == TRUE) ? "enabled" : "disabled"));

	return TRUE;
}

INT	Set_HtOpMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{

	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);

	if (Value == HTMODE_GF)
		pAd->CommonCfg.RegTransmitSetting.field.HTMODE  = HTMODE_GF;
	else if ( Value == HTMODE_MM )
		pAd->CommonCfg.RegTransmitSetting.field.HTMODE  = HTMODE_MM;
	else 
		return FALSE; /*Invalid argument */

	SetCommonHT(pAd);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtOpMode_Proc::(HtOpMode=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.HTMODE));

	return TRUE;

}	

INT	Set_HtStbc_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{

	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value == STBC_USE)
		pAd->CommonCfg.RegTransmitSetting.field.STBC = STBC_USE;
	else if ( Value == STBC_NONE )
		pAd->CommonCfg.RegTransmitSetting.field.STBC = STBC_NONE;
	else 
		return FALSE; /*Invalid argument */

	SetCommonHT(pAd);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_Stbc_Proc::(HtStbc=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.STBC));

	return TRUE;											
}

			
INT	Set_HtExtcha_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{

	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value == 0)
		pAd->CommonCfg.RegTransmitSetting.field.EXTCHA  = EXTCHA_BELOW;
	else if ( Value ==1 )
		pAd->CommonCfg.RegTransmitSetting.field.EXTCHA = EXTCHA_ABOVE;
	else 
		return FALSE; /*Invalid argument 	*/
	
	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtExtcha_Proc::(HtExtcha=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.EXTCHA));

	return TRUE;			
}

INT	Set_HtMpduDensity_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value <=7)
		pAd->CommonCfg.BACapability.field.MpduDensity = Value;
	else
		pAd->CommonCfg.BACapability.field.MpduDensity = 4;

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMpduDensity_Proc::(HtMpduDensity=%d)\n",pAd->CommonCfg.BACapability.field.MpduDensity));

	return TRUE;																																	
}

INT	Set_HtBaWinSize_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);

#ifdef CONFIG_AP_SUPPORT
	/* Intel IOT*/
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		Value = 64;
#endif /* CONFIG_AP_SUPPORT */

	if (Value >=1 && Value <= 64)
	{
		pAd->CommonCfg.REGBACapability.field.RxBAWinLimit = Value;
		pAd->CommonCfg.BACapability.field.RxBAWinLimit = Value;
	}
	else
	{
        pAd->CommonCfg.REGBACapability.field.RxBAWinLimit = 64;
		pAd->CommonCfg.BACapability.field.RxBAWinLimit = 64;
	}
	
	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtBaWinSize_Proc::(HtBaWinSize=%d)\n",pAd->CommonCfg.BACapability.field.RxBAWinLimit));

	return TRUE;																																	
}		


INT	Set_HtRdg_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value == 0)			
		pAd->CommonCfg.bRdg = FALSE;
	else if (Value ==1)
        	pAd->CommonCfg.bRdg = TRUE;
	else 
		return FALSE; /*Invalid argument*/

#ifdef RELEASE_EXCLUDE
        /*
        *   2014, April 7, disable RDG for mt_mac at present.
        */
#endif
#ifdef MT_MAC
    if (pAd->chipCap.hif_type == HIF_MT)
        pAd->CommonCfg.bRdg = FALSE;
#endif

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtRdg_Proc::(HtRdg=%d)\n",pAd->CommonCfg.bRdg));

	return TRUE;																																	
}		

INT	Set_HtLinkAdapt_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->bLinkAdapt = FALSE;
	else if ( Value ==1 )
		pAd->bLinkAdapt = TRUE;
	else
		return FALSE; /*Invalid argument*/
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtLinkAdapt_Proc::(HtLinkAdapt=%d)\n",pAd->bLinkAdapt));

	return TRUE;																																	
}		


INT	Set_HtAmsdu_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	pAd->CommonCfg.BACapability.field.AmsduEnable = (Value == 0) ? FALSE : TRUE;
	SetCommonHT(pAd);	
		
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtAmsdu_Proc::(HtAmsdu=%d)\n",pAd->CommonCfg.BACapability.field.AmsduEnable));

	return TRUE;																																	
}			


INT	Set_HtAutoBa_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
	{
		pAd->CommonCfg.BACapability.field.AutoBA = FALSE;
		pAd->CommonCfg.BACapability.field.Policy = BA_NOTUSE;
	}
    else if (Value == 1)	
    {
		pAd->CommonCfg.BACapability.field.AutoBA = TRUE;
		pAd->CommonCfg.BACapability.field.Policy = IMMED_BA;
    }
	else
		return FALSE; /*Invalid argument*/
	
    pAd->CommonCfg.REGBACapability.field.AutoBA = pAd->CommonCfg.BACapability.field.AutoBA;
	pAd->CommonCfg.REGBACapability.field.Policy = pAd->CommonCfg.BACapability.field.Policy;
	SetCommonHT(pAd);	
		
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtAutoBa_Proc::(HtAutoBa=%d)\n",pAd->CommonCfg.BACapability.field.AutoBA));

	return TRUE;				
		
}		
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																						

INT	Set_HtProtect_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->CommonCfg.bHTProtect = FALSE;
    else if (Value == 1)	
		pAd->CommonCfg.bHTProtect = TRUE;
	else
		return FALSE; /*Invalid argument*/

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtProtect_Proc::(HtProtect=%d)\n",pAd->CommonCfg.bHTProtect));

	return TRUE;
}

INT	Set_SendSMPSAction_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    UCHAR mac[6], mode;
	RTMP_STRING *token;
	RTMP_STRING sepValue[] = ":", DASH = '-';
	INT i, ret;
    MAC_TABLE_ENTRY *pEntry;
	LONG ret_value;

    /*DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));*/
/*
	The BARecTearDown inupt string format should be xx:xx:xx:xx:xx:xx-d, 
		=>The six 2 digit hex-decimal number previous are the Mac address, 
		=>The seventh decimal number is the mode value.
*/
    if(strlen(arg) < 19)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and mode value in decimal format.*/
		return FALSE;

   	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		ret = kstrtol((token+1), 10, &ret_value);
		mode = (UCHAR)ret_value;

		if (mode > MMPS_DISABLE)
			return FALSE;
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return FALSE;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x", 
					PRINT_MAC(mac), mode));

		pEntry = MacTableLookup(pAd, mac);

		if (pEntry) {
		    DBGPRINT(RT_DEBUG_OFF, ("\nSendSMPSAction SMPS mode = %d\n", mode));
		    SendSMPSAction(pAd, pEntry->wcid, mode);
		}

		return TRUE;
	}

	return FALSE;


}

INT	Set_HtMIMOPSmode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value <=3)
		pAd->CommonCfg.BACapability.field.MMPSmode = Value;
	else
		pAd->CommonCfg.BACapability.field.MMPSmode = 3;

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMIMOPSmode_Proc::(MIMOPS mode=%d)\n",pAd->CommonCfg.BACapability.field.MMPSmode));

	return TRUE;																																	
}

#ifdef CONFIG_AP_SUPPORT
/* 
    ==========================================================================
    Description:
        Set Tx Stream number
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_HtTxStream_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG 	Value;	
		
	Value = simple_strtol(arg, 0, 10);

	if ((Value <= 3) && (Value >= 1) && (Value <= pAd->Antenna.field.TxPath)) /* 3*3*/
		pAd->CommonCfg.TxStream = (UCHAR)Value;
	else
		pAd->CommonCfg.TxStream = (UCHAR)pAd->Antenna.field.TxPath;

	if ((pAd->MACVersion < RALINK_2883_VERSION) &&
		(pAd->CommonCfg.TxStream > 2))
	{
		pAd->CommonCfg.TxStream = 2; /* only 2 TX streams for RT2860 series*/
	}

	SetCommonHT(pAd);

	APStop(pAd);
	APStartUp(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtTxStream_Proc::(Tx Stream=%d)\n",pAd->CommonCfg.TxStream));
		
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set Rx Stream number
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_HtRxStream_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG 	Value;	
		
	Value = simple_strtol(arg, 0, 10);

	if ((Value <= 3) && (Value >= 1) && (Value <= pAd->Antenna.field.RxPath))
		pAd->CommonCfg.RxStream = (UCHAR)Value;
	else
		pAd->CommonCfg.RxStream = (UCHAR)pAd->Antenna.field.RxPath;

	if ((pAd->MACVersion < RALINK_2883_VERSION) &&
		(pAd->CommonCfg.RxStream > 2)) /* 3*3*/
	{
		pAd->CommonCfg.RxStream = 2; /* only 2 RX streams for RT2860 series*/
	}

	SetCommonHT(pAd);

	APStop(pAd);
	APStartUp(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtRxStream_Proc::(Rx Stream=%d)\n",pAd->CommonCfg.RxStream));
		
	return TRUE;
}

#ifdef DOT11_N_SUPPORT
#ifdef GREENAP_SUPPORT
INT	Set_GreenAP_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
	{
		pAd->ApCfg.bGreenAPActive=FALSE;
		pAd->ApCfg.bGreenAPEnable = FALSE;
	}
	else if (Value == 1)	
		pAd->ApCfg.bGreenAPEnable = TRUE;
	else
		return FALSE; /*Invalid argument*/

	DBGPRINT(RT_DEBUG_TRACE, ("Set_GreenAP_Proc::(bGreenAPEnable=%d)\n",pAd->ApCfg.bGreenAPEnable));

	return TRUE;
}
#endif /* GREENAP_SUPPORT */
#endif /* DOT11_N_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

INT	Set_ForceShortGI_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->WIFItestbed.bShortGI = FALSE;
	else if (Value == 1)	
		pAd->WIFItestbed.bShortGI = TRUE;
	else
		return FALSE; /*Invalid argument*/

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ForceShortGI_Proc::(ForceShortGI=%d)\n", pAd->WIFItestbed.bShortGI));

	return TRUE;
}



INT	Set_ForceGF_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->WIFItestbed.bGreenField = FALSE;
	else if (Value == 1)	
		pAd->WIFItestbed.bGreenField = TRUE;
	else
		return FALSE; /*Invalid argument*/

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ForceGF_Proc::(ForceGF=%d)\n", pAd->WIFItestbed.bGreenField));

	return TRUE;
}

INT	Set_HtMimoPs_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->CommonCfg.bMIMOPSEnable = FALSE;
	else if (Value == 1)	
		pAd->CommonCfg.bMIMOPSEnable = TRUE;
	else
		return FALSE; /*Invalid argument*/

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMimoPs_Proc::(HtMimoPs=%d)\n",pAd->CommonCfg.bMIMOPSEnable));

	return TRUE;
}


#ifdef DOT11N_DRAFT3
INT Set_HT_BssCoex_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *pParam)
{
	UCHAR bBssCoexEnable = simple_strtol(pParam, 0, 10);

	pAd->CommonCfg.bBssCoexEnable = ((bBssCoexEnable == 1) ? TRUE: FALSE);

	DBGPRINT(RT_DEBUG_TRACE, ("Set bBssCoexEnable=%d!\n", pAd->CommonCfg.bBssCoexEnable));
	
	if ((pAd->CommonCfg.bBssCoexEnable == FALSE)
		&& pAd->CommonCfg.bRcvBSSWidthTriggerEvents)
	{
		/* switch back 20/40 */
		DBGPRINT(RT_DEBUG_TRACE, ("Set bBssCoexEnable:  Switch back 20/40. \n"));
		pAd->CommonCfg.bRcvBSSWidthTriggerEvents = FALSE;
		if ((pAd->CommonCfg.Channel <=14) && (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth == BW_40))
		{
            pAd->CommonCfg.AddHTInfo.AddHtInfo.RecomWidth = 1;
			pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset = pAd->CommonCfg.RegTransmitSetting.field.EXTCHA;
		}
	}
	
	return TRUE;
}


INT Set_HT_BssCoexApCntThr_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *pParam)
{
	pAd->CommonCfg.BssCoexApCntThr = simple_strtol(pParam, 0, 10);

	DBGPRINT(RT_DEBUG_TRACE, ("Set BssCoexApCntThr=%d!\n", pAd->CommonCfg.BssCoexApCntThr));
	
	return TRUE;
}
#endif /* DOT11N_DRAFT3 */

#endif /* DOT11_N_SUPPORT */


#ifdef DOT11_VHT_AC
INT	Set_VhtBw_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG vht_cw;
	UCHAR cent_ch;
	vht_cw = simple_strtol(arg, 0, 10);


	if (vht_cw == VHT_BW_80)
		pAd->CommonCfg.vht_bw = VHT_BW_80;
	else
		pAd->CommonCfg.vht_bw = VHT_BW_2040;

	if (!WMODE_CAP_AC(pAd->CommonCfg.PhyMode))
		goto direct_done;
	
	SetCommonHT(pAd);
	if(pAd->CommonCfg.BBPCurrentBW == BW_80)
		cent_ch = pAd->CommonCfg.vht_cent_ch;
	else
		cent_ch = pAd->CommonCfg.CentralChannel;

	AsicSwitchChannel(pAd, cent_ch, FALSE);
	AsicLockChannel(pAd, cent_ch);

	DBGPRINT(RT_DEBUG_TRACE, ("BW_%s, PrimaryChannel(%d), %s CentralChannel = %d, apply it immediately\n", 
						(pAd->CommonCfg.BBPCurrentBW == BW_80 ? "80":
							(pAd->CommonCfg.BBPCurrentBW == BW_40 ? "40" :
							"20")),
						pAd->CommonCfg.Channel,
						(pAd->CommonCfg.BBPCurrentBW == BW_80 ? "VHT" : "HT"), cent_ch));

direct_done:

	DBGPRINT(RT_DEBUG_TRACE, ("Set_VhtBw_Proc::(VHT_BW=%d)\n", pAd->CommonCfg.vht_bw));

	return TRUE;
}


INT set_VhtBwSignal_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG bw_signal = simple_strtol(arg, 0, 10);

	if (bw_signal <= 2)
		pAd->CommonCfg.vht_bw_signal = bw_signal;
	else
		pAd->CommonCfg.vht_bw_signal = BW_SIGNALING_DISABLE;
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): vht_bw_signal=%d(%s)\n",
				__FUNCTION__, pAd->CommonCfg.vht_bw_signal,
				(pAd->CommonCfg.vht_bw_signal == BW_SIGNALING_DYNAMIC ? "Dynamic" :
				(pAd->CommonCfg.vht_bw_signal == BW_SIGNALING_STATIC ? "Static" : "Disable"))));

	return TRUE;
}


INT	Set_VhtStbc_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value == STBC_USE)
		pAd->CommonCfg.vht_stbc = STBC_USE;
	else if ( Value == STBC_NONE )
		pAd->CommonCfg.vht_stbc = STBC_NONE;
	else 
		return FALSE; /*Invalid argument */

	SetCommonHT(pAd);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_VhtStbc_Proc::(VhtStbc=%d)\n", pAd->CommonCfg.vht_stbc));

	return TRUE;											
}

INT	Set_VhtDisallowNonVHT_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value == 0)
		pAd->CommonCfg.bNonVhtDisallow = FALSE;
	else
		pAd->CommonCfg.bNonVhtDisallow = TRUE;
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_VhtDisallowNonVHT_Proc::(bNonVhtDisallow=%d)\n", pAd->CommonCfg.bNonVhtDisallow));

	return TRUE;											
}
#endif /* DOT11_VHT_AC */


#ifdef ETH_CONVERT_SUPPORT
INT Set_EthConvertMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{

	/*
		Dongle mode: it means use our default MAC address to connect to AP, and
					support multiple internal PCs connect to Internet via this default MAC
		Clone mode : it means use one specific MAC address to connect to remote AP, and
					just the node who owns the MAC address can connect to Internet.
		Hybrid mode: it means use some specific MAC address to connecto to remote AP, and
					support mulitple internal PCs connect to Internet via this specified MAC address.
	*/
	if (rtstrcasecmp(arg, "dongle") == TRUE)
	{
		pAd->EthConvert.ECMode = ETH_CONVERT_MODE_DONGLE;
		NdisMoveMemory(&pAd->EthConvert.EthCloneMac[0], &pAd->CurrentAddress[0], MAC_ADDR_LEN);
		pAd->EthConvert.CloneMacVaild = TRUE;
	}
	else if (rtstrcasecmp(arg, "clone") == TRUE)
	{
		pAd->EthConvert.ECMode = ETH_CONVERT_MODE_CLONE;
		pAd->EthConvert.CloneMacVaild = FALSE;
	}
	else if (rtstrcasecmp(arg, "hybrid") == TRUE)
	{
		pAd->EthConvert.ECMode = ETH_CONVERT_MODE_HYBRID;
		pAd->EthConvert.CloneMacVaild = FALSE;
	}
	else
	{
		pAd->EthConvert.ECMode = ETH_CONVERT_MODE_DISABLE;
		pAd->EthConvert.CloneMacVaild = FALSE;
	}
	pAd->EthConvert.macAutoLearn = FALSE;
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_EthConvertMode_Proc(): EthConvertMode=%d!\n", pAd->EthConvert.ECMode));
	
	return TRUE;
}


INT Set_EthCloneMac_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];
	extern UCHAR BROADCAST_ADDR[MAC_ADDR_LEN];
	
	/*
		If the input is the zero mac address, it means use our default(from EEPROM) MAC address as out-going 
		   MAC address.
		If the input is the broadcast MAC address, it means use the source MAC of first packet forwarded by
		   our device as the out-going MAC address.
		If the input is any other specific valid MAC address, use it as the out-going MAC address.
	*/
	
	pAd->EthConvert.macAutoLearn = FALSE;
	if (strlen(arg) == 0)
	{	
		NdisZeroMemory(&pAd->EthConvert.EthCloneMac[0], MAC_ADDR_LEN);
		goto done;
	}
	
	if (rtstrmactohex(arg, (RTMP_STRING *) &pAd->EthConvert.EthCloneMac[0]) == FALSE)
		goto fail;

done:
	DBGPRINT(RT_DEBUG_TRACE, ("Set_EthCloneMac_Proc(): CloneMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	
	    pAd->EthConvert.EthCloneMac[0], pAd->EthConvert.EthCloneMac[1], pAd->EthConvert.EthCloneMac[2], 
	    pAd->EthConvert.EthCloneMac[3], pAd->EthConvert.EthCloneMac[4], pAd->EthConvert.EthCloneMac[5]));
	
	if (NdisEqualMemory(&pAd->EthConvert.EthCloneMac[0], &ZERO_MAC_ADDR[0], MAC_ADDR_LEN))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Use our default Mac address for cloned MAC!\n"));
		NdisMoveMemory(&pAd->EthConvert.EthCloneMac[0], &pAd->CurrentAddress[0], MAC_ADDR_LEN);
		pAd->EthConvert.CloneMacVaild = TRUE;
	}
	else if (NdisEqualMemory(&pAd->EthConvert.EthCloneMac[0], &BROADCAST_ADDR[0], MAC_ADDR_LEN))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Use first frowarded Packet's source Mac for cloned MAC!\n"));
		NdisMoveMemory(&pAd->EthConvert.EthCloneMac[0], &pAd->CurrentAddress[0], MAC_ADDR_LEN);
		pAd->EthConvert.CloneMacVaild = FALSE;
		pAd->EthConvert.macAutoLearn = TRUE;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Use user assigned spcific Mac address for cloned MAC!\n"));
		pAd->EthConvert.CloneMacVaild = TRUE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_EthCloneMac_Proc(): After ajust, CloneMac = %02x:%02x:%02x:%02x:%02x:%02x\n",	
	    pAd->EthConvert.EthCloneMac[0], pAd->EthConvert.EthCloneMac[1], pAd->EthConvert.EthCloneMac[2], 
	    pAd->EthConvert.EthCloneMac[3], pAd->EthConvert.EthCloneMac[4], pAd->EthConvert.EthCloneMac[5]));
	
	return TRUE;

fail:
	DBGPRINT(RT_DEBUG_ERROR, ("Set_EthCloneMac_Proc: wrong Mac Address format or length!\n"));
	NdisMoveMemory(&pAd->EthConvert.EthCloneMac[0], &pAd->CurrentAddress[0], MAC_ADDR_LEN);
	
	return FALSE;
	
}
#endif /* ETH_CONVERT_SUPPORT */

INT	Set_FixedTxMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	struct wifi_dev *wdev = NULL;
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
#endif /* CONFIG_AP_SUPPORT */		
	INT	fix_tx_mode = RT_CfgSetFixedTxPhyMode(arg);


#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		wdev = &pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev;
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		wdev = &pAd->StaCfg.wdev;
#endif /* CONFIG_STA_SUPPORT */

	if (wdev)
		wdev->DesiredTransmitSetting.field.FixedTxMode = (USHORT)fix_tx_mode;

	DBGPRINT(RT_DEBUG_TRACE, ("%s():(FixedTxMode=%d)\n",
								__FUNCTION__, fix_tx_mode));

	return TRUE;
}

#ifdef CONFIG_APSTA_MIXED_SUPPORT
INT	Set_OpMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

#ifdef RTMP_MAC_PCI
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP))
#endif /* RTMP_MAC_USB */
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Can not switch operate mode on interface up !! \n"));
		return FALSE;
	}

	if (Value == 0)
		pAd->OpMode = OPMODE_STA;
	else if (Value == 1)	
		pAd->OpMode = OPMODE_AP;
	else
		return FALSE; /*Invalid argument*/

	DBGPRINT(RT_DEBUG_TRACE, ("Set_OpMode_Proc::(OpMode=%s)\n", pAd->OpMode == 1 ? "AP Mode" : "STA Mode"));

	return TRUE;
}
#endif /* CONFIG_APSTA_MIXED_SUPPORT */


#ifdef DBG_CTRL_SUPPORT
#ifdef INCLUDE_DEBUG_QUEUE
/* ---------------------- DEBUG QUEUE ------------------------*/

#define DBQ_LENGTH	512
#define DBQ_DATA_LENGTH	8

/* Define to include TX and RX HT Control field in log */
/* #define DBQ_INCLUDE_HTC */

typedef
struct {
	UCHAR type;					/* type of data*/
	ULONG timestamp;			/* sec/usec timestamp from gettimeofday*/
	UCHAR data[DBQ_DATA_LENGTH];	/* data*/
} DBQUEUE_ENTRY;

/* Type field definitions */
#define DBQ_TYPE_EMPTY	0
#define DBQ_TYPE_TXWI	0x70		/* TXWI*/
#define DBQ_TYPE_TXHDR	0x72		/* TX Header*/
#define DBQ_TYPE_TXFIFO	0x73		/* TX Stat FIFO*/
#define DBQ_TYPE_RXWI	0x78		/* RXWI uses 0x78 to 0x7A for 5 longs*/
#define DBQ_TYPE_RXHDR	0x7B		/* RX Header*/
#define DBQ_TYPE_TXQHTC	0x7c		/* RX Qos+HT Control field*/
#define DBQ_TYPE_RXQHTC	0x7d		/* RX Qos+HT Control field */
#define DBQ_TYPE_RALOG	0x7e		/* RA Log */

#define DBQ_INIT_SIG	0x4442484E	/* 'DBIN' - dbqInit initialized flag*/
#define DBQ_ENA_SIG		0x4442454E	/* 'DBEN' - dbqEnable enabled flag*/

static DBQUEUE_ENTRY dbQueue[DBQ_LENGTH];
static ULONG dbqTail=0;
static ULONG dbqEnable=0;
static ULONG dbqInit=0;

/* dbQueueInit - initialize Debug Queue variables and clear the queue*/
void dbQueueInit(void)
{
	int i;

	for (i=0; i<DBQ_LENGTH; i++)
		dbQueue[i].type = DBQ_TYPE_EMPTY;
	dbqTail = 0;
	dbqInit = DBQ_INIT_SIG;
}

/* dbQueueEnqueue - enqueue data*/
void dbQueueEnqueue(UCHAR type, UCHAR *data)
{
	DBQUEUE_ENTRY *oldTail;
	struct timeval tval;

	if (dbqEnable!=DBQ_ENA_SIG || data==NULL)
		return;

	if (dbqInit!=DBQ_INIT_SIG || dbqTail>=DBQ_LENGTH)
		dbQueueInit();

	oldTail = &dbQueue[dbqTail];

	/* Advance tail and mark as empty*/
	if (dbqTail >= DBQ_LENGTH-1)
		dbqTail = 0;
	else
		dbqTail++;
	dbQueue[dbqTail].type = DBQ_TYPE_EMPTY;

	/* Enqueue data*/
	oldTail->type = type;
	do_gettimeofday(&tval);
	oldTail->timestamp = tval.tv_sec*1000000L + tval.tv_usec;
	memcpy(oldTail->data, data, DBQ_DATA_LENGTH);
}

void dbQueueEnqueueTxFrame(UCHAR *pTxWI, UCHAR *pHeader_802_11)
{
	dbQueueEnqueue(DBQ_TYPE_TXWI, pTxWI);

	/* 802.11 Header */
	if (pHeader_802_11 != NULL) {
		dbQueueEnqueue(DBQ_TYPE_TXHDR, pHeader_802_11);
#ifdef DBQ_INCLUDE_HTC
		/* Qos+HT Control field */
		if ((pHeader_802_11[0] & 0x08) && (pHeader_802_11[1] & 0x80))
			dbQueueEnqueue(DBQ_TYPE_TXQHTC, pHeader_802_11+24);
#endif /* DBQ_INCLUDE_HTC */
	}
}

void dbQueueEnqueueRxFrame(UCHAR *pRxWI, UCHAR *pHeader_802_11, ULONG flags)
{
	/* Ignore Beacons if disabled */
	if ((flags & DBF_DBQ_NO_BCN) && (pHeader_802_11[0] & 0xfc)==0x80)
		return;

	/* RXWI */
	dbQueueEnqueue(DBQ_TYPE_RXWI, pRxWI);
	if (flags & DBF_DBQ_RXWI_FULL) {
		dbQueueEnqueue(DBQ_TYPE_RXWI+1, pRxWI+8);
		dbQueueEnqueue(DBQ_TYPE_RXWI+2, pRxWI+16);
	}

	/* 802.11 Header */
	dbQueueEnqueue(DBQ_TYPE_RXHDR, (UCHAR *)pHeader_802_11);

#ifdef DBQ_INCLUDE_HTC
	/* Qos+HT Control field */
	if ((pHeader_802_11[0] & 0x08) &&
		(pHeader_802_11[1] & 0x80))
		dbQueueEnqueue(DBQ_TYPE_RXQHTC, pHeader_802_11+24);
#endif /* DBQ_INCLUDE_HTC */
}


/* dbQueueDisplayPhy - Display PHY rate */
static void dbQueueDisplayPHY(USHORT phyRate)
{
	static CHAR *mode[4] = {" C", "oM","mM", "gM"};

	DBGPRINT(RT_DEBUG_OFF, ("%2s%02d %c%c%c%c",
		//(phyRate>>8) & 0xFF, phyRate & 0xFF,
		mode[(phyRate>>14) & 0x3],							// Mode: c, o, m, g
		phyRate & 0x7F,										// MCS
		(phyRate & 0x0100)? 'S': 'L',						// Guard Int: S or L
		(phyRate & 0x0080)? '4': '2',						// BW: 4 or 2
		(phyRate & 0x0200)? 'S': 's',						// STBC:  S or s
		(phyRate & 0x2000)? 'I': ((phyRate & 0x800)? 'E': '_')	// Beamforming:  E or I or _
		) );
}

/* dbQueueDump - dump contents of debug queue*/
static void dbQueueDump(
	IN  PRTMP_ADAPTER   pAd,
	BOOLEAN decode)
{
	DBQUEUE_ENTRY *oldTail;
	int i, origMCS, succMCS;
	ULONG lastTimestamp=0;
	BOOLEAN showTimestamp;
	USHORT phyRate;

	if (dbqInit!=DBQ_INIT_SIG || dbqTail>=DBQ_LENGTH)
		return;

	oldTail = &dbQueue[dbqTail];

	for (i=0; i<DBQ_LENGTH; i++) {
		if (++oldTail >= &dbQueue[DBQ_LENGTH])
			oldTail = dbQueue;

		/* Skip empty entries*/
		if (oldTail->type == DBQ_TYPE_EMPTY)
			continue;

		showTimestamp = FALSE;

		switch (oldTail->type) {
		case 0x70:	/* TXWI - 2 longs, MSB to LSB */
		case 0x78:	/* RXWI - 2 longs, MSB to LSB */
			showTimestamp = TRUE;

			if (decode && oldTail->type==0x70) {
				DBGPRINT(RT_DEBUG_OFF, ("\nTxWI ") );
				dbQueueDisplayPHY(oldTail->data[3]*256 + oldTail->data[2]);
				DBGPRINT(RT_DEBUG_OFF, ("%c s=%03X %02X %s-",
						(oldTail->data[0] & 0x10)? 'A': '_',				// AMPDU
						(oldTail->data[7]*256 + oldTail->data[6]) & 0xFFF,	// Size
						oldTail->data[5],									// WCID
						(oldTail->data[4] & 0x01)? "AK": "NA" ));			// ACK/NACK
			}
			else if (decode && oldTail->type==0x78) {
				DBGPRINT(RT_DEBUG_OFF, ("\nRxWI ") );
				dbQueueDisplayPHY(oldTail->data[7]*256 + oldTail->data[6]);
				DBGPRINT(RT_DEBUG_OFF, (" s=%03X %02X %02X%01X-",
						(oldTail->data[3]*256 + oldTail->data[2]) & 0xFFF,	// Size
						oldTail->data[0],									// WCID
						oldTail->data[5], oldTail->data[4]>>4 ));			// Seq Number
			}
			else
				DBGPRINT(RT_DEBUG_OFF, ("\n%cxWI %02X%02X %02X%02X-%02X%02X %02X%02X----",
					oldTail->type==0x70? 'T': 'R',
					oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0],
					oldTail->data[7], oldTail->data[6], oldTail->data[5], oldTail->data[4]) );
			break;

		case 0x79:	/* RXWI - next 2 longs, MSB to LSB */
			if (decode) {
				struct raw_rssi_info rssi_info;

				rssi_info.raw_rssi[0] = (CHAR)oldTail->data[0];
				rssi_info.raw_rssi[1] = (CHAR)oldTail->data[1];
				rssi_info.raw_rssi[2] = (CHAR)oldTail->data[2];
				DBGPRINT(RT_DEBUG_OFF, ("Rx2  %2d %2d %2d S:%d %d %d ",
						ConvertToRssi(pAd, &rssi_info, RSSI_IDX_0),
						ConvertToRssi(pAd, &rssi_info, RSSI_IDX_1),
						ConvertToRssi(pAd, &rssi_info, RSSI_IDX_2),
						(oldTail->data[4]*3 + 8)/16,
						(oldTail->data[5]*3 + 8)/16,
						(oldTail->data[6]*3 + 8)/16) );
			}
			else
				DBGPRINT(RT_DEBUG_OFF, ("Rx2  %02X%02X %02X%02X-%02X%02X %02X%02X    ",
						oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0],
						oldTail->data[7], oldTail->data[6], oldTail->data[5], oldTail->data[4]) );
			break;

#ifdef RTMP_RBUS_SUPPORT
#if defined(RT2883) || defined(RT3883)
		case 0x7a:	/* RXWI - next long, MSB to LSB. Decode BF SNR */
			if (decode)
				DBGPRINT(RT_DEBUG_OFF, (" BF:%02d %02d %02d 0x%02X\n",
						(BF_SNR_OFFSET + (CHAR)(oldTail->data[1]) + 2)/4,
						(BF_SNR_OFFSET + (CHAR)(oldTail->data[2]) + 2)/4,
						(BF_SNR_OFFSET + (CHAR)(oldTail->data[3]) + 2)/4,
						oldTail->data[0]) );
			else
				DBGPRINT(RT_DEBUG_OFF, ("Rx4  %02X%02X %02X%02X-%02X%02X %02X%02X    \n",
					oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0],
					oldTail->data[7], oldTail->data[6], oldTail->data[5], oldTail->data[4]) );
			break;
#endif /* defined(RT2883) || defined(RT3883) */
#endif /* RTMP_RBUS_SUPPORT */

		case 0x7c:	/* TX HTC+QoS, 6 bytes, MSB to LSB */
		case 0x7d:	/* RX HTC+QoS, 6 bytes, MSB to LSB */
			DBGPRINT(RT_DEBUG_OFF, ("%cxHTC  H:%02X%02X%02X%02X Q:%02X%02X   ",
					oldTail->type==0x7c? 'T': 'R',
					oldTail->data[5], oldTail->data[4], oldTail->data[3], oldTail->data[2],
					oldTail->data[1], oldTail->data[0]) );
			break;

		case 0x72:	/* Tx 802.11 header, MSB to LSB, translate type/subtype*/
		case 0x7b:	/* Rx*/
			{
			UCHAR tCode;
			struct _typeTableEntry {
				UCHAR code;	/* Type/subtype*/
				CHAR  str[4];
			} *pTab;
			static struct _typeTableEntry typeTable[] = {
				{0x00, "mARq"}, {0x01, "mARp"}, {0x02, "mRRq"}, {0x03, "mRRp"},
				{0x04, "mPRq"}, {0x05, "mPRp"}, {0x08, "mBcn"}, {0x09, "mATI"},
				{0x0a, "mDis"}, {0x0b, "mAut"}, {0x0c, "mDAu"}, {0x0d, "mAct"},
				{0x0e, "mANA"},
				{0x17, "cCWr"}, {0x18, "cBAR"}, {0x19, "cBAc"}, {0x1a, "cPSP"},
				{0x1b, "cRTS"}, {0x1c, "cCTS"}, {0x1d, "cACK"}, {0x1e, "cCFE"},
				{0x1f, "cCEA"},
				{0x20, "dDat"}, {0x21, "dDCA"}, {0x22, "dDCP"}, {0x23, "dDAP"},
				{0x24, "dNul"}, {0x25, "dCFA"}, {0x26, "dCFP"}, {0x27, "dCAP"},
				{0x28, "dQDa"}, {0x29, "dQCA"}, {0x2a, "dQCP"}, {0x2b, "dQAP"},
				{0x2c, "dQNu"}, {0x2e, "dQNP"}, {0x2f, "dQNA"},
				{0xFF, "RESV"}};

			tCode = ((oldTail->data[0]<<2) & 0x30) | ((oldTail->data[0]>>4) & 0xF);
			for (pTab=typeTable; pTab->code!=0xFF; pTab++) {
				if (pTab->code == tCode)
					break;
			}

			DBGPRINT(RT_DEBUG_OFF, ("%cxH  %c%c%c%c [%02X%02X %02X%02X]       \n",
					oldTail->type==0x72? 'T': 'R',
					pTab->str[0], pTab->str[1], pTab->str[2], pTab->str[3],
					oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0]) );
			}
			break;

		case 0x73:	/* TX STAT FIFO*/
			showTimestamp = TRUE;

			/* origMCS is limited to 4 bits. Check for case of MCS16 to 23*/
			origMCS = (oldTail->data[0]>>1) & 0xF;
			succMCS = (oldTail->data[2] & 0x7F);
			if (succMCS>origMCS && origMCS<8)
				origMCS += 16;
			phyRate = (oldTail->data[3]<<8) + oldTail->data[2];

			DBGPRINT(RT_DEBUG_OFF, ("TxFI %02X%02X%02X%02X=%c%c%c%c%c M%02d/%02d%c%c",
					oldTail->data[3], oldTail->data[2],
					oldTail->data[1], oldTail->data[0],
					(phyRate & 0x0100)? 'S': 'L',				/* Guard Int:    S or L */
					(phyRate & 0x0080)? '4': '2',				/* BW: 			 4 or 2 */
					(phyRate & 0x0200)? 'S': 's',				/* STBC:         S or s */
					(phyRate & 0x2000)? 'I': ((phyRate & 0x0800)? 'E': '_'), /* Beamforming:  E or I or _ */
					(oldTail->data[0] & 0x40)? 'A': '_',		/* Aggregated:   A or _ */
					succMCS, origMCS,							/* MCS:          <Final>/<orig> */
					succMCS==origMCS? ' ': '*',					/* Retry:        '*' if MCS doesn't match */
					(oldTail->data[0] & 0x20)? ' ': 'F') );		/* Success/Fail  _ or F */
			break;
		case 0x7E:	/* RA Log info */
			{
				struct {USHORT phy; USHORT per; USHORT tp; USHORT bfPer;} *p = (void*)(oldTail->data);
				DBGPRINT(RT_DEBUG_OFF, ("RALog %02X%02X %d %d %d    ",
											(p->phy>>8) & 0xFF, p->phy & 0xFF, p->per, p->tp, p->bfPer) );
			}
			break;

		default:
			DBGPRINT(RT_DEBUG_OFF, ("%02X   %02X%02X %02X%02X %02X%02X %02X%02X   ", oldTail->type,
					oldTail->data[0], oldTail->data[1], oldTail->data[2], oldTail->data[3], 
					oldTail->data[4], oldTail->data[5], oldTail->data[6], oldTail->data[7]) );
			break;
		}

		if (showTimestamp)
		{
			ULONG t = oldTail->timestamp;
			ULONG dt = oldTail->timestamp-lastTimestamp;

			DBGPRINT(RT_DEBUG_OFF, ("%lu.%06lu ", t/1000000L, t % 1000000L) );

			if (dt>999999L)
			DBGPRINT(RT_DEBUG_OFF, ("+%lu.%06lu s\n", dt/1000000L, dt % 1000000L) );
			else
			DBGPRINT(RT_DEBUG_OFF, ("+%lu us\n", dt) );
			lastTimestamp = oldTail->timestamp;
		}
	}
}

/*
	Set_DebugQueue_Proc - Control DBQueue
		iwpriv ra0 set DBQueue=dd.
			dd: 0=>disable, 1=>enable, 2=>dump, 3=>clear, 4=>dump & decode
*/
INT Set_DebugQueue_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    ULONG argValue = simple_strtol(arg, 0, 10);

	switch (argValue) {
	case 0:
		dbqEnable = 0;
		break;
	case 1:
		dbqEnable = DBQ_ENA_SIG;
		break;
	case 2:
		dbQueueDump(pAd, FALSE);
		break;
	case 3:
		dbQueueInit();
		break;
	case 4:
		dbQueueDump(pAd, TRUE);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}
#endif /* INCLUDE_DEBUG_QUEUE */
#endif /* DBG_CTRL_SUPPORT */

#ifdef STREAM_MODE_SUPPORT
/*
	========================================================================
	Routine Description:
		Set the enable/disable the stream mode
		
	Arguments:
		1:	enable for 1SS 
		2:	enable for 2SS
		3:	enable for 1SS and 2SS
		0:	disable

	Notes:
		Currently only support 1SS
	========================================================================
*/
INT Set_StreamMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 streamWord, reg, regAddr;
	
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): Not support for HIF_MT yet!\n",
					__FUNCTION__));
		return FALSE;
	}

	if (pAd->chipCap.FlgHwStreamMode == FALSE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("chip not supported feature\n"));
		return FALSE;
	}
	
	pAd->CommonCfg.StreamMode = (simple_strtol(arg, 0, 10) & 0x3);
	DBGPRINT(RT_DEBUG_TRACE, ("%s():StreamMode=%d\n", __FUNCTION__, pAd->CommonCfg.StreamMode));

	streamWord = StreamModeRegVal(pAd);
	for (regAddr = TX_CHAIN_ADDR0_H; regAddr <= TX_CHAIN_ADDR3_H; regAddr += 8)
	{
		RTMP_IO_READ32(pAd, regAddr, &reg);
		reg &= (~0x000F0000);
		RTMP_IO_WRITE32(pAd, regAddr, streamWord | reg);
	}

	return TRUE;
}


INT Set_StreamModeMac_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	return FALSE;
}


INT Set_StreamModeMCS_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->CommonCfg.StreamModeMCS = simple_strtol(arg, 0, 16);
	DBGPRINT(RT_DEBUG_TRACE, ("%s():StreamModeMCS=%02X\n", 
				__FUNCTION__, pAd->CommonCfg.StreamModeMCS));
	
	return TRUE;
}
#endif /* STREAM_MODE_SUPPORT */


#ifdef PRE_ANT_SWITCH
/*
	Set_PreAntSwitch_Proc - enable/disable Preamble Antenna Switch
		usage: iwpriv ra0 set PreAntSwitch=[0 | 1]
*/
INT Set_PreAntSwitch_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    pAd->CommonCfg.PreAntSwitch = simple_strtol(arg, 0, 10)!=0;
    DBGPRINT(RT_DEBUG_TRACE, ("%s():(PreAntSwitch=%d)\n",
				__FUNCTION__, pAd->CommonCfg.PreAntSwitch));
	return TRUE;
}


/*
	Set_PreAntSwitchRSSI_Proc - set Preamble Antenna Switch RSSI threshold
		usage: iwpriv ra0 set PreAntSwitchRSSI=<RSSI threshold in dBm>
*/
INT Set_PreAntSwitchRSSI_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    pAd->CommonCfg.PreAntSwitchRSSI = simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_TRACE, ("%s():(PreAntSwitchRSSI=%d)\n", 
				__FUNCTION__, pAd->CommonCfg.PreAntSwitchRSSI));
	return TRUE;
}

/*
	Set_PreAntSwitchTimeout_Proc - set Preamble Antenna Switch Timeout threshold
		usage: iwpriv ra0 set PreAntSwitchTimeout=<timeout in seconds, 0=>disabled>
*/
INT Set_PreAntSwitchTimeout_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    pAd->CommonCfg.PreAntSwitchTimeout = simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_TRACE, ("%s():(PreAntSwitchTimeout=%d)\n", 
				__FUNCTION__, pAd->CommonCfg.PreAntSwitchTimeout));
	return TRUE;
}
#endif /* PRE_ANT_SWITCH */


#if defined(RT2883) || defined(RT3883)
/*
	Set_PhyRateLimit_Proc - limit max PHY rate
		usage: iwpriv ra0 set PhyRateLimit=<PHY rate in Mbps>
*/
INT Set_PhyRateLimit_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    pAd->CommonCfg.PhyRateLimit = simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_TRACE, ("%s():(PhyRateLimit=%ld)\n", 
				__FUNCTION__, pAd->CommonCfg.PhyRateLimit));
	return TRUE;
}

#ifdef DBG
/*
	Set_FixedRate_Proc - Use fixed MCS
		usage: iwpriv ra0 set PhyRateLimit=<Item in RateSwitch table> or -1 to disable
		Parameter is ItemNo that is used to index into the RateSwitch table
*/
INT Set_FixedRate_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int rate;
    
	rate = simple_strtol(arg, 0, 10);
	if (rate<-1 || rate > MAX_TX_RATE_INDEX)
		return FALSE;
    pAd->CommonCfg.FixedRate = rate;
    DBGPRINT(RT_DEBUG_TRACE, ("Set_FixedRate_Proc::(FixedRate=%d)\n", pAd->CommonCfg.FixedRate));
	return TRUE;
}
#endif /* DBG */
#endif /* defined(RT2883) || defined(RT3883) */


#ifdef CFO_TRACK
/*
	Set_CFOTrack_Proc - enable/disable CFOTrack
		usage: iwpriv ra0 set CFOTrack=[0..8]
*/
INT Set_CFOTrack_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    pAd->CommonCfg.CFOTrack = simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_TRACE, ("%s():(CFOTrack=%d)\n",
				__FUNCTION__, pAd->CommonCfg.CFOTrack));
	return TRUE;
}
#endif /* CFO_TRACK */


#ifdef DBG_CTRL_SUPPORT
INT Set_DebugFlags_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    pAd->CommonCfg.DebugFlags = simple_strtol(arg, 0, 16);

    DBGPRINT(RT_DEBUG_TRACE, ("Set_DebugFlags_Proc::(DebugFlags=%02lX)\n", pAd->CommonCfg.DebugFlags));
	return TRUE;
}
#endif /* DBG_CTRL_SUPPORT */


#if defined(RT305x)||defined(RT3070)
INT Set_HiPower_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->CommonCfg.HighPowerPatchDisabled = !(simple_strtol(arg, 0, 10));

	if (pAd->CommonCfg.HighPowerPatchDisabled != 0)
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, 0x20);
#ifdef RT3070
		if ((IS_RT3070(pAd) && ((pAd->MACVersion & 0xffff) < 0x0201)))
#endif /* RT3070 */
		RT30xxWriteRFRegister(pAd, RF_R27, 0x23); 
	}
	return TRUE;
}
#endif /* defined(RT305x) || defined(RT3070) */


#ifdef RT305x
#if defined(RT3352) || defined(RT5350)
#ifdef RTMP_INTERNAL_TX_ALC
INT Set_TSSIMaxRange_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT8 range = (UINT8)simple_strtol(arg, 0, 10);	

	if ((!IS_RT3352(pAd)) && (!IS_RT5350(pAd)))
	{
		DBGPRINT_ERR(("The chipset is neither RT3352 nor RT5350 !\n"));
		return FALSE;
	}

	if ((range != 0) && (range != 2) && (range != 4))
	{
		DBGPRINT_ERR(("You should input 0, 2, or 4 !\n"));
		return FALSE;
	}

	/* one step means 0.5 dB */
	pAd->chipCap.TxPowerMaxCompenStep = (range << 1);
	DBGPRINT(RT_DEBUG_TRACE, ("Set_TSSIMaxRange_Proc::(TxPowerMaxCompenStep=%d)\n",
				pAd->chipCap.TxPowerMaxCompenStep));

	return TRUE;
}
#endif /* RTMP_INTERNAL_TX_ALC */
#endif /* defined(RT3352) || defined(RT5350) */
#endif /* RT305x */


INT Set_LongRetryLimit_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR LongRetryLimit = (UCHAR)simple_strtol(arg, 0, 10);

	AsicSetRetryLimit(pAd, TX_RTY_CFG_RTY_LIMIT_LONG, LongRetryLimit);
	DBGPRINT(RT_DEBUG_TRACE, ("IF Set_LongRetryLimit_Proc::(LongRetryLimit=0x%x)\n", LongRetryLimit));
	return TRUE;
}


INT Set_ShortRetryLimit_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR ShortRetryLimit = (UCHAR)simple_strtol(arg, 0, 10);

#ifdef MT_MAC
	pAd->shortretry = ShortRetryLimit;
#else/* !MT_MAC */
	AsicSetRetryLimit(pAd, TX_RTY_CFG_RTY_LIMIT_SHORT, ShortRetryLimit);
#endif /* MT_MAC */
	DBGPRINT(RT_DEBUG_TRACE, ("IF Set_ShortRetryLimit_Proc::(ShortRetryLimit=0x%x)\n", ShortRetryLimit));
	return TRUE;
}


INT Set_AutoFallBack_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	return RT_CfgSetAutoFallBack(pAd, arg);
}


#ifdef RELEASE_EXCLUDE
#ifdef RTMP_MAC_USB
INT	Show_RxBulk_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	return TRUE;
}

INT Show_TxBulk_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int i;
	HT_TX_CONTEXT	*pHTTXContext;
	TX_CONTEXT		*pMLMEContext, *pPsPollContext, *pNULLContext;
	unsigned long IrqFlags = 0;
	UINT32 value;

	DBGPRINT(RT_DEBUG_OFF, ("\nTx Data Ring Info:\n"));
	for (i = 0 ; i < NUM_OF_TX_RING; i++)
	{
		RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[i], IrqFlags);
		pHTTXContext = &pAd->TxContext[i];
		DBGPRINT(RT_DEBUG_OFF, ("TxContext[%d]:CWPos=%ld, NBPos=%ld, ENBPos=%ld, bCopy=%d, pending=%d!\n", 
						i, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition, 
						pHTTXContext->ENextBulkOutPosition, pHTTXContext->bCopySavePad, pAd->BulkOutPending[i]));
		DBGPRINT(RT_DEBUG_OFF, ("\tBulkOut Req=0x%lx, Complete=0x%lx, Other=0x%lx\n", 
								pAd->BulkOutReq, pAd->BulkOutComplete, pAd->BulkOutCompleteOther));
		if ((pAd->BulkOutPending[i] == TRUE) && (pHTTXContext->IRPPending == TRUE))
			dump_urb(pHTTXContext->pUrb);
		RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[i], IrqFlags);
	}

	DBGPRINT(RT_DEBUG_OFF, ("\nTx Ctrl Ring Info:\n"));
	{
		pNULLContext = &pAd->NullContext;
		pPsPollContext = &pAd->PsPollContext;
		DBGPRINT(RT_DEBUG_OFF, ("NullContext: InUse=%d, bWaitingBulkOut=%d, IRPPending=%d, BulkOutSize=%ld!\n", 
				pNULLContext->InUse, pNULLContext->bWaitingBulkOut, pNULLContext->IRPPending, pNULLContext->BulkOutSize));
		DBGPRINT(RT_DEBUG_OFF, ("PsPollContext: InUse=%d, bWaitingBulkOut=%d, IRPPending=%d, BulkOutSize=%ld!\n", 
				pPsPollContext->InUse, pPsPollContext->bWaitingBulkOut, pPsPollContext->IRPPending, pPsPollContext->BulkOutSize));
	}

	DBGPRINT(RT_DEBUG_OFF, ("\nTx Mgmt Ring Info:\n"));
	RTMP_IRQ_LOCK(&pAd->BulkOutLock[MGMTPIPEIDX], IrqFlags);	
	DBGPRINT(RT_DEBUG_OFF, ("\tTxDmaIdx=%d, TxCpuIdx=%d, tTxSwFreeIdx=%d!\n", 
				pAd->MgmtRing.TxDmaIdx, pAd->MgmtRing.TxCpuIdx, pAd->MgmtRing.TxSwFreeIdx));
	for(i = 0; i < MGMT_RING_SIZE; i++)
	{
		pMLMEContext = (PTX_CONTEXT)pAd->MgmtRing.Cell[i].AllocVa;
					
		DBGPRINT(RT_DEBUG_OFF, ("MgmtRing.Cell[%d]:InUse=%d, bWaitingBulkOut=%d, IRPPending=%d, BulkOutSize=%ld!\n", 
						i, pMLMEContext->InUse, pMLMEContext->bWaitingBulkOut, pMLMEContext->IRPPending, pMLMEContext->BulkOutSize));
		if ((pAd->BulkOutPending[MGMTPIPEIDX] == TRUE) && (pMLMEContext->IRPPending == TRUE))
			dump_urb(pMLMEContext->pUrb);
	}
	RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[MGMTPIPEIDX], IrqFlags);
	
	RTMP_IO_READ32(pAd, 0x438, &value);
	DBGPRINT(RT_DEBUG_OFF, ("Register 0x438=0x%x!\n", value));
	RTMP_IO_READ32(pAd, 0x43c, &value);	
	DBGPRINT(RT_DEBUG_OFF, ("Register 0x43c=0x%x!\n", value));
	
	return TRUE;
}
#endif /* RTMP_MAC_USB */
#endif /* RELEASE_EXCLUDE */

RTMP_STRING *RTMPGetRalinkAuthModeStr(
    IN  NDIS_802_11_AUTHENTICATION_MODE authMode)
{
	switch(authMode)
	{
		case Ndis802_11AuthModeOpen:
			return "OPEN";
		case Ndis802_11AuthModeWPAPSK:
			return "WPAPSK";
		case Ndis802_11AuthModeShared:
			return "SHARED";
		case Ndis802_11AuthModeAutoSwitch:
			return "WEPAUTO";
		case Ndis802_11AuthModeWPA:
			return "WPA";
		case Ndis802_11AuthModeWPA2:
			return "WPA2";
		case Ndis802_11AuthModeWPA2PSK:
			return "WPA2PSK";
        case Ndis802_11AuthModeWPA1PSKWPA2PSK:
			return "WPAPSKWPA2PSK";
        case Ndis802_11AuthModeWPA1WPA2:
			return "WPA1WPA2";
#ifdef WPA3_SUPPORT
		case Ndis802_11AuthModeWPA2PSKWPA3SAE:
			return "WPA2PSKWPA3SAE";
		case Ndis802_11AuthModeWPA3SAE:
			return "WPA3SAE";
#endif
		case Ndis802_11AuthModeWPANone:
			return "WPANONE";
		default:
			return "UNKNOW";
	}
}

RTMP_STRING *RTMPGetRalinkEncryModeStr(
    IN  USHORT encryMode)
{
	switch(encryMode)
	{
		case Ndis802_11WEPDisabled:
			return "NONE";
		case Ndis802_11WEPEnabled:
			return "WEP";        
		case Ndis802_11TKIPEnable:
			return "TKIP";
		case Ndis802_11AESEnable:
			return "AES";
        case Ndis802_11TKIPAESMix:
			return "TKIPAES";
		default:
			return "UNKNOW";
	}
}


INT	Show_SSID_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	UCHAR	ssid_str[33];


	NdisZeroMemory(&ssid_str[0], 33);
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
		NdisMoveMemory(&ssid_str[0], 
						pAd->ApCfg.MBSSID[pObj->ioctl_if].Ssid, 
						pAd->ApCfg.MBSSID[pObj->ioctl_if].SsidLen);
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		NdisMoveMemory(&ssid_str[0], 
						pAd->CommonCfg.Ssid,
						pAd->CommonCfg.SsidLen);
	}
#endif /* CONFIG_STA_SUPPORT */

	snprintf(pBuf, BufLen, "\t%s", ssid_str);
	return 0;
}

INT	Show_WirelessMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{

	switch(pAd->CommonCfg.PhyMode)
	{
		case (UCHAR)(WMODE_B | WMODE_G):
			snprintf(pBuf, BufLen, "\t11B/G");
			break;
		case (UCHAR)(WMODE_B):
			snprintf(pBuf, BufLen, "\t11B");
			break;
		case (UCHAR)(WMODE_A):
			snprintf(pBuf, BufLen, "\t11A");
			break;
		case (UCHAR)(WMODE_A | WMODE_B | WMODE_G):
			snprintf(pBuf, BufLen, "\t11A/B/G");
			break;
		case (UCHAR)(WMODE_G):
			snprintf(pBuf, BufLen, "\t11G");
			break;
#ifdef DOT11_N_SUPPORT
		case (UCHAR)(WMODE_A | WMODE_B | WMODE_G | WMODE_GN | WMODE_AN):
			snprintf(pBuf, BufLen, "\t11A/B/G/N");
			break;
		case (UCHAR)(WMODE_GN):
			snprintf(pBuf, BufLen, "\t11N only with 2.4G");
			break;
		case (UCHAR)(WMODE_G | WMODE_GN):
			snprintf(pBuf, BufLen, "\t11G/N");
			break;
		case (UCHAR)(WMODE_A | WMODE_AN):
			snprintf(pBuf, BufLen, "\t11A/N");
			break;
		case (UCHAR)(WMODE_B | WMODE_G | WMODE_GN):
			snprintf(pBuf, BufLen, "\t11B/G/N");
			break;
		case (UCHAR)(WMODE_A | WMODE_G | WMODE_GN | WMODE_AN):
			snprintf(pBuf, BufLen, "\t11A/G/N");
			break;
		case (UCHAR)(WMODE_AN):
			snprintf(pBuf, BufLen, "\t11N only with 5G");
			break;
#endif /* DOT11_N_SUPPORT */
		default:
			snprintf(pBuf, BufLen, "\tUnknow Value(%d)", pAd->CommonCfg.PhyMode);
			break;
	}

	return 0;
}


INT	Show_TxBurst_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%s", pAd->CommonCfg.bEnableTxBurst ? "TRUE":"FALSE");
	return 0;
}

INT	Show_TxPreamble_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	switch(pAd->CommonCfg.TxPreamble)
	{
		case Rt802_11PreambleShort:
			snprintf(pBuf, BufLen, "\tShort");
			break;
		case Rt802_11PreambleLong:
			snprintf(pBuf, BufLen, "\tLong");
			break;
		case Rt802_11PreambleAuto:
			snprintf(pBuf, BufLen, "\tAuto");
			break;
		default:
			snprintf(pBuf, BufLen, "\tUnknown Value(%lu)", pAd->CommonCfg.TxPreamble);
			break;
	}
	
	return 0;
}

INT	Show_TxPower_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%lu", pAd->CommonCfg.TxPowerPercentage);
	return 0;
}

INT	Show_Channel_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%d", pAd->CommonCfg.Channel);
	return 0;
}

INT	Show_BGProtection_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	switch(pAd->CommonCfg.UseBGProtection)
	{
		case 1: /*Always On*/
			snprintf(pBuf, BufLen, "\tON");
			break;
		case 2: /*Always OFF*/
			snprintf(pBuf, BufLen, "\tOFF");
			break;
		case 0: /*AUTO*/
			snprintf(pBuf, BufLen, "\tAuto");
			break;
		default:
			snprintf(pBuf, BufLen, "\tUnknow Value(%lu)", pAd->CommonCfg.UseBGProtection);
			break;
	}
	return 0;
}

INT	Show_RTSThreshold_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%u", pAd->CommonCfg.RtsThreshold);
	return 0;
}

INT	Show_FragThreshold_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%u", pAd->CommonCfg.FragmentThreshold);
	return 0;
}

#ifdef DOT11_N_SUPPORT
INT	Show_HtBw_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	if (pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40)
	{
		snprintf(pBuf, BufLen, "\t40 MHz");
	}
	else
	{
        snprintf(pBuf, BufLen, "\t20 MHz");
	}
	return 0;
}

INT	Show_HtMcs_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	struct wifi_dev *wdev = NULL;
#ifdef CONFIG_AP_SUPPORT    
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		wdev = &pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		wdev = &pAd->StaCfg.wdev;
#endif /* CONFIG_STA_SUPPORT */
	if (wdev)
		snprintf(pBuf, BufLen, "\t%u", wdev->DesiredTransmitSetting.field.MCS);
	return 0;
}

INT	Show_HtGi_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	switch(pAd->CommonCfg.RegTransmitSetting.field.ShortGI)
	{
		case GI_400:
			snprintf(pBuf, BufLen, "\tGI_400");
			break;
		case GI_800:
			snprintf(pBuf, BufLen, "\tGI_800");
			break;
		default:
			snprintf(pBuf, BufLen, "\tUnknow Value(%u)", pAd->CommonCfg.RegTransmitSetting.field.ShortGI);
			break;
	}
	return 0;
}

INT	Show_HtOpMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	switch(pAd->CommonCfg.RegTransmitSetting.field.HTMODE)
	{
		case HTMODE_GF:
			snprintf(pBuf, BufLen, "\tGF");
			break;
		case HTMODE_MM:
			snprintf(pBuf, BufLen, "\tMM");
			break;
		default:
			snprintf(pBuf, BufLen, "\tUnknow Value(%u)", pAd->CommonCfg.RegTransmitSetting.field.HTMODE);
			break;
	}
	return 0;
}

INT	Show_HtExtcha_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	switch(pAd->CommonCfg.RegTransmitSetting.field.EXTCHA)
	{
		case EXTCHA_BELOW:
			snprintf(pBuf, BufLen, "\tBelow");
			break;
		case EXTCHA_ABOVE:
			snprintf(pBuf, BufLen, "\tAbove");
			break;
		default:
			snprintf(pBuf, BufLen, "\tUnknow Value(%u)", pAd->CommonCfg.RegTransmitSetting.field.EXTCHA);
			break;
	}
	return 0;
}


INT	Show_HtMpduDensity_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%u", pAd->CommonCfg.BACapability.field.MpduDensity);
	return 0;
}

INT	Show_HtBaWinSize_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%u", pAd->CommonCfg.BACapability.field.RxBAWinLimit);
	return 0;
}

INT	Show_HtRdg_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%s", pAd->CommonCfg.bRdg ? "TRUE":"FALSE");
	return 0;
}

INT	Show_HtAmsdu_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%s", pAd->CommonCfg.BACapability.field.AmsduEnable ? "TRUE":"FALSE");
	return 0;
}

INT	Show_HtAutoBa_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%s", pAd->CommonCfg.BACapability.field.AutoBA ? "TRUE":"FALSE");
	return 0;
}
#endif /* DOT11_N_SUPPORT */

INT	Show_CountryRegion_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%d", pAd->CommonCfg.CountryRegion);
	return 0;
}

INT	Show_CountryRegionABand_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%d", pAd->CommonCfg.CountryRegionForABand);
	return 0;
}

INT	Show_CountryCode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%s", pAd->CommonCfg.CountryCode);
	return 0;
}

#ifdef AGGREGATION_SUPPORT
INT	Show_PktAggregate_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%s", pAd->CommonCfg.bAggregationCapable ? "TRUE":"FALSE");
	return 0;
}
#endif /* AGGREGATION_SUPPORT */

INT	Show_WmmCapable_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		snprintf(pBuf, BufLen, "\t%s", pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev.bWmmCapable ? "TRUE":"FALSE");
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		snprintf(pBuf, BufLen, "\t%s", pAd->CommonCfg.bWmmCapable ? "TRUE":"FALSE");
#endif /* CONFIG_STA_SUPPORT */
	
	return 0;
}


INT	Show_IEEE80211H_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\t%s", pAd->CommonCfg.bIEEE80211H ? "TRUE":"FALSE");
	return 0;
}

#ifdef CONFIG_STA_SUPPORT
INT	Show_NetworkType_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	switch(pAd->StaCfg.BssType)
	{
		case BSS_ADHOC:
			snprintf(pBuf, BufLen, "\tAdhoc");
			break;
		case BSS_INFRA:
			snprintf(pBuf, BufLen, "\tInfra");
			break;
		case BSS_ANY:
			snprintf(pBuf, BufLen, "\tAny");
			break;
		case BSS_MONITOR:
			snprintf(pBuf, BufLen, "\tMonitor");
			break;
		default:
			snprintf(pBuf, BufLen, "\tUnknow Value(%d)", pAd->StaCfg.BssType);
			break;
	}
	return 0;
}

#ifdef WSC_STA_SUPPORT
INT	Show_WpsPbcBand_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	switch(pAd->StaCfg.WscControl.WpsApBand)
	{
		case PREFERRED_WPS_AP_PHY_TYPE_2DOT4_G_FIRST:
			snprintf(pBuf, BufLen, "\t2.4G");
			break;
		case PREFERRED_WPS_AP_PHY_TYPE_5_G_FIRST:
			snprintf(pBuf, BufLen, "\t5G");
			break;
		case PREFERRED_WPS_AP_PHY_TYPE_AUTO_SELECTION:
			snprintf(pBuf, BufLen, "\tAuto");
			break;
		default:
			snprintf(pBuf, BufLen, "\tUnknow Value(%d)", pAd->StaCfg.WscControl.WpsApBand);
			break;
	}
	return 0;
}
#endif /* WSC_STA_SUPPORT */

INT	Show_WPAPSK_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	if ((pAd->StaCfg.WpaPassPhraseLen >= 8) &&
		(pAd->StaCfg.WpaPassPhraseLen < 64))
		snprintf(pBuf, BufLen, "\tWPAPSK = %s", pAd->StaCfg.WpaPassPhrase);
	else
	{
		INT idx;
		snprintf(pBuf, BufLen, "\tWPAPSK = ");
		for (idx = 0; idx < 32; idx++)
			snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "%02X", pAd->StaCfg.WpaPassPhrase[idx]);	
	}

	return 0;
}

INT	Show_AutoReconnect_Proc(
	IN	PRTMP_ADAPTER	pAd,
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\tAutoReconnect = %d", pAd->StaCfg.bAutoReconnect);
	return 0;
}

#ifdef STA_EASY_CONFIG_SETUP
INT	Show_AutoProvisionEn_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\tAutoProvisionEn = %d", pAd->StaCfg.EasyConfigInfo.bEnable);
	return 0;
}

INT	Show_ChangeMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\tChangeMode = %d", pAd->StaCfg.EasyConfigInfo.bChangeMode);
	return 0;
}
#endif /* STA_EASY_CONFIG_SETUP */
#endif /* CONFIG_STA_SUPPORT */

INT	Show_AuthMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	NDIS_802_11_AUTHENTICATION_MODE	AuthMode = Ndis802_11AuthModeOpen;
	struct wifi_dev *wdev = NULL;
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		wdev  = &pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		wdev = &pAd->StaCfg.wdev;

#endif /* CONFIG_STA_SUPPORT */

	if (!wdev)
		return -1;

	AuthMode = wdev->AuthMode;
	if ((AuthMode >= Ndis802_11AuthModeOpen) && 
		(AuthMode <= Ndis802_11AuthModeWPA1PSKWPA2PSK))
		snprintf(pBuf, BufLen, "\t%s", RTMPGetRalinkAuthModeStr(AuthMode));
#ifdef WAPI_SUPPORT
	else if (AuthMode == Ndis802_11AuthModeWAICERT)
		snprintf(pBuf, BufLen, "\t%s", "WAI_CERT");
	else if (AuthMode == Ndis802_11AuthModeWAIPSK)
		snprintf(pBuf, BufLen, "\t%s", "WAI_PSK");				 
#endif /* WAPI_SUPPORT */
	else
		snprintf(pBuf, BufLen, "\tUnknow Value(%d)", AuthMode);
	
	return 0;
}

INT	Show_EncrypType_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	struct wifi_dev *wdev = NULL;
	NDIS_802_11_WEP_STATUS	WepStatus = Ndis802_11WEPDisabled;
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		wdev = &pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		wdev = &pAd->StaCfg.wdev;
#endif /* CONFIG_STA_SUPPORT */

	if (!wdev)
		return -1;

	WepStatus = wdev->WepStatus;
	if ((WepStatus >= Ndis802_11WEPEnabled) && 
		(WepStatus <= Ndis802_11Encryption4KeyAbsent))
		snprintf(pBuf, BufLen, "\t%s", RTMPGetRalinkEncryModeStr((USHORT)WepStatus));
#ifdef WAPI_SUPPORT
	else if (WepStatus == Ndis802_11EncryptionSMS4Enabled)
		snprintf(pBuf, BufLen, "\t%s", "WPI_SMS4");
#endif /* WAPI_SUPPORT */
	else
		snprintf(pBuf, BufLen, "\tUnknow Value(%d)", WepStatus);
	
	return 0;
}

INT	Show_DefaultKeyID_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	struct wifi_dev *wdev = NULL;
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		wdev = &pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		wdev = &pAd->StaCfg.wdev;
#endif /* CONFIG_STA_SUPPORT */

	if (!wdev)
		return -1;
	
	snprintf(pBuf, BufLen, "\t%d", wdev->DefaultKeyId);

	return 0;
}


INT	Show_WepKey_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN  INT				KeyIdx,
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	UCHAR   Key[16] = {0}, KeyLength = 0;
	INT		index = BSS0, idx = 0, len = 0, ucMaxKeySize = 0;
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		index = pObj->ioctl_if;
#endif /* CONFIG_AP_SUPPORT */

	KeyLength = pAd->SharedKey[index][KeyIdx].KeyLen;
	if (KeyLength > sizeof(Key)) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Invalid KeyLength (%d)\n"
				, __func__, KeyLength));
		return 0;
	}
	NdisMoveMemory(Key, pAd->SharedKey[index][KeyIdx].Key, KeyLength);		
		
	/*check key string is ASCII or not*/
	if (RTMPCheckStrPrintAble((PCHAR)Key, KeyLength))
		snprintf(pBuf, BufLen, "\t%s", Key);
    else
    {
    	len = strlen(pBuf);
		ucMaxKeySize = sizeof(Key)/sizeof(Key[0]);
		snprintf(pBuf, BufLen, "\t");
        for (idx = 0; idx < KeyLength; idx++)
        {
			if (idx < ucMaxKeySize)
				len += snprintf(pBuf+len, BufLen-len, "%02X", Key[idx]);
        }
    }
	return 0;
}

INT	Show_Key1_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	Show_WepKey_Proc(pAd, 0, pBuf, BufLen);
	return 0;
}

INT	Show_Key2_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	Show_WepKey_Proc(pAd, 1, pBuf, BufLen);
	return 0;
}

INT	Show_Key3_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	Show_WepKey_Proc(pAd, 2, pBuf, BufLen);
	return 0;
}

INT	Show_Key4_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	Show_WepKey_Proc(pAd, 3, pBuf, BufLen);
	return 0;
}

INT	Show_PMK_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	INT 	idx;
	UCHAR	PMK[32] = {0};

#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		NdisMoveMemory(PMK, pAd->ApCfg.MBSSID[pObj->ioctl_if].PMK, 32);
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		NdisMoveMemory(PMK, pAd->StaCfg.PMK, 32);
#endif /* CONFIG_STA_SUPPORT */
	
	snprintf(pBuf, BufLen, "\tPMK = ");
	for (idx = 0; idx < 32; idx++)
		snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "%02X", PMK[idx]);

	return 0;
}


INT	Show_STA_RAInfo_Proc(
	IN	PRTMP_ADAPTER	pAd,
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\n");
#ifdef PRE_ANT_SWITCH
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "PreAntSwitch: %d\n"
			, pAd->CommonCfg.PreAntSwitch);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "PreAntSwitchRSSI: %d\n"
			, pAd->CommonCfg.PreAntSwitchRSSI);
#endif /* PRE_ANT_SWITCH */

#if defined (RT2883) || defined (RT3883)
	snprintf(pBuf+strlen(pBuf), "FixedRate: %d\n", pAd->CommonCfg.FixedRate);
#endif // defined (RT2883) || defined (RT3883) //

#ifdef NEW_RATE_ADAPT_SUPPORT
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "LowTrafficThrd: %d\n"
		, pAd->CommonCfg.lowTrafficThrd);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "TrainUpRule: %d\n"
		, pAd->CommonCfg.TrainUpRule);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "TrainUpRuleRSSI: %d\n"
		, pAd->CommonCfg.TrainUpRuleRSSI);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "TrainUpLowThrd: %d\n"
		, pAd->CommonCfg.TrainUpLowThrd);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "TrainUpHighThrd: %d\n"
		, pAd->CommonCfg.TrainUpHighThrd);
#endif // NEW_RATE_ADAPT_SUPPORT //

#ifdef STREAM_MODE_SUPPORT
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "StreamMode: %d\n"
		, pAd->CommonCfg.StreamMode);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "StreamModeMCS: 0x%04x\n"
		, pAd->CommonCfg.StreamModeMCS);
#endif // STREAM_MODE_SUPPORT //
#ifdef TXBF_SUPPORT
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "ITxBfEn: %d\n"
		, pAd->CommonCfg.RegTransmitSetting.field.ITxBfEn);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "ITxBfTimeout: %ld\n"
		, pAd->CommonCfg.ITxBfTimeout);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "ETxBfTimeout: %ld\n"
		, pAd->CommonCfg.ETxBfTimeout);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "ETxBfEnCond: %ld\n"
		, pAd->CommonCfg.ETxBfEnCond);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "ETxBfNoncompress: %d\n"
		, pAd->CommonCfg.ETxBfNoncompress);
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "ETxBfIncapable: %d\n"
		, pAd->CommonCfg.ETxBfIncapable);
#endif // TXBF_SUPPORT //

#ifdef DBG_CTRL_SUPPORT
	snprintf(pBuf+strlen(pBuf), BufLen-strlen(pBuf), "DebugFlags: 0x%lx\n"
		, pAd->CommonCfg.DebugFlags);
#endif /* DBG_CTRL_SUPPORT */
	return 0;
}


static INT dump_mac_table(RTMP_ADAPTER *pAd, UINT32 ent_type, BOOLEAN bReptCli)
{
	INT i;
	ULONG DataRate=0;

	
	DBGPRINT(RT_DEBUG_OFF, ("\n"));

#ifdef DOT11_N_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("HT Operating Mode : %d\n"
			, pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode));
	DBGPRINT(RT_DEBUG_OFF, ("\n"));
#endif /* DOT11_N_SUPPORT */
	
	DBGPRINT(RT_DEBUG_OFF
		, ("\n%-19s%-5s%-5s%-4s%-4s%-4s%-7s%-10s%-6s%-4s%-6s%-6s%-6s%-7s%-7s%-7s\n"
		, "MAC", "MODE", "AID", "BSS", "PSM", "WMM", "MIMOPS", "RSSI0/1/2"
		, "PhMd", "BW", "MCS", "SGI", "STBC", "Idle", "Rate", "QosMap"));

	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];

		if ((ent_type == ENTRY_NONE))
		{
			/* dump all MacTable entries */
			if (pEntry->EntryType == ENTRY_NONE) 
				continue;
		} else {
			/* dump MacTable entries which match the EntryType */
			if (pEntry->EntryType != ent_type)
				continue;

			if ((IS_ENTRY_CLIENT(pEntry) || IS_ENTRY_APCLI(pEntry)) 
				&& (pEntry->Sst != SST_ASSOC))
				continue;

#ifdef MAC_REPEATER_SUPPORT
			if (bReptCli == FALSE)
			{	
				/* only dump the apcli entry which not a RepeaterCli */
				if (IS_ENTRY_APCLI(pEntry) && (pEntry->bReptCli == TRUE))
					continue;
			}
#endif /* MAC_REPEATER_SUPPORT */
		}

		DataRate=0;
		getRate(pEntry->HTPhyMode, &DataRate);
		DBGPRINT(RT_DEBUG_OFF, ("%02X:%02X:%02X:%02X:%02X:%02X  "
				, PRINT_MAC(pEntry->Addr)));
		DBGPRINT(RT_DEBUG_OFF, ("%-5x", pEntry->EntryType));
		DBGPRINT(RT_DEBUG_OFF, ("%-5d", (int)pEntry->Aid));
		DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->func_tb_idx));
		DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->PsMode));
		DBGPRINT(RT_DEBUG_OFF, ("%-4d"
			, (int)CLIENT_STATUS_TEST_FLAG(pEntry, fCLIENT_STATUS_WMM_CAPABLE)));
#ifdef DOT11_N_SUPPORT
		DBGPRINT(RT_DEBUG_OFF, ("%-7d", (int)pEntry->MmpsMode));
#endif /* DOT11_N_SUPPORT */
		DBGPRINT(RT_DEBUG_OFF, ("%-3d/%-3d/%-3d", pEntry->RssiSample.AvgRssi[0],
					pEntry->RssiSample.AvgRssi[1],
					pEntry->RssiSample.AvgRssi[2]));
		DBGPRINT(RT_DEBUG_OFF, ("%-6s", get_phymode_str(pEntry->HTPhyMode.field.MODE)));
		DBGPRINT(RT_DEBUG_OFF, ("%-4s", get_bw_str(pEntry->HTPhyMode.field.BW)));
#ifdef DOT11_VHT_AC
		if (pEntry->HTPhyMode.field.MODE == MODE_VHT)
			DBGPRINT(RT_DEBUG_OFF, ("%dS-M%d", ((pEntry->HTPhyMode.field.MCS>>4) + 1)
					, (pEntry->HTPhyMode.field.MCS & 0xf)));
		else
#endif /* DOT11_VHT_AC */
		DBGPRINT(RT_DEBUG_OFF, ("%-6d", pEntry->HTPhyMode.field.MCS));
		DBGPRINT(RT_DEBUG_OFF, ("%-4d", pEntry->HTPhyMode.field.ShortGI));
		DBGPRINT(RT_DEBUG_OFF, ("%-5d", pEntry->HTPhyMode.field.STBC));
		DBGPRINT(RT_DEBUG_OFF, ("%-7d"
				, (int)(pEntry->StaIdleTimeout - pEntry->NoDataIdleCount)));
		DBGPRINT(RT_DEBUG_OFF, ("%-7d", (int)DataRate));
#ifdef CONFIG_HOTSPOT_R2
		DBGPRINT(RT_DEBUG_OFF, ("%-7d", (int)pEntry->QosMapSupport));
#endif	
		DBGPRINT(RT_DEBUG_OFF, ("%-10d, %d, %d%%\n"
			, pEntry->DebugFIFOCount, pEntry->DebugTxCount
			, (pEntry->DebugTxCount) ?
			((pEntry->DebugTxCount-pEntry->DebugFIFOCount)*100/pEntry->DebugTxCount)
			: 0));
#ifdef CONFIG_HOTSPOT_R2							
		if (pEntry->QosMapSupport)
		{	
			int k = 0;

			DBGPRINT(RT_DEBUG_OFF, ("DSCP Exception:\n"));
			for(k=0;k<pEntry->DscpExceptionCount/2;k++)
			{
				DBGPRINT(RT_DEBUG_OFF, ("[Value: %4d] [UP: %4d]\n"
					, pEntry->DscpException[k] & 0xff
					, (pEntry->DscpException[k] >> 8) & 0xff));
			}
			BGPRINT(RT_DEBUG_OFF, ("DSCP Range:\n"));
			for(k=0;k<8;k++)
			{
				DBGPRINT(RT_DEBUG_OFF
					, ("[UP :%3d][Low Value: %4d] [High Value: %4d]\n"
					, k, pEntry->DscpRange[k] & 0xff
					, (pEntry->DscpRange[k] >> 8) & 0xff));
			}
		}
#endif	
//+++Add by shiang for debug
		DBGPRINT(RT_DEBUG_OFF, ("\t\t\t\t\t\t\tMaxCap:%-10s"
				, get_phymode_str(pEntry->MaxHTPhyMode.field.MODE)));
		DBGPRINT(RT_DEBUG_OFF, ("%-6s", get_bw_str(pEntry->MaxHTPhyMode.field.BW)));
#ifdef DOT11_VHT_AC
		if (pEntry->MaxHTPhyMode.field.MODE == MODE_VHT)
			DBGPRINT(RT_DEBUG_OFF, ("%dS-M%d"
					, ((pEntry->MaxHTPhyMode.field.MCS>>4) + 1)
					, (pEntry->MaxHTPhyMode.field.MCS & 0xf)));
		else
#endif /* DOT11_VHT_AC */
		DBGPRINT(RT_DEBUG_OFF, ("%-6d", pEntry->MaxHTPhyMode.field.MCS));
		DBGPRINT(RT_DEBUG_OFF, ("%-4d", pEntry->MaxHTPhyMode.field.ShortGI));
		DBGPRINT(RT_DEBUG_OFF, ("%-5d\n", pEntry->MaxHTPhyMode.field.STBC));
//---Add by shiang for debug
		DBGPRINT(RT_DEBUG_OFF, ("\n"));
	} 

	return TRUE;
}


INT Show_MacTable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 ent_type = ENTRY_CLIENT;
	
	DBGPRINT(RT_DEBUG_OFF, ("%s(): arg=%s\n", __FUNCTION__, (arg == NULL ? "" : arg)));
	if (arg && strlen(arg)) {
		if (rtstrcasecmp(arg, "sta") == TRUE)
			ent_type = ENTRY_CLIENT;
		else if (rtstrcasecmp(arg, "ap") == TRUE)
			ent_type = ENTRY_AP;
		else
			ent_type = ENTRY_NONE;
	}

	DBGPRINT(RT_DEBUG_OFF, ("Dump MacTable entries info, EntType=0x%x\n", ent_type));
	
	return dump_mac_table(pAd, ent_type, FALSE);
}

#ifdef MT_MAC
static INT dump_ps_table(RTMP_ADAPTER *pAd, UINT32 ent_type, BOOLEAN bReptCli)
{
   INT i,j;
   struct wtbl_entry tb_entry;
   union WTBL_1_DW3 *dw3 = (union WTBL_1_DW3 *)&tb_entry.wtbl_1.wtbl_1_d3.word;
   UINT32  rPseRdTabAccessReg;  
   BOOLEAN pfgForce;
   UCHAR pucPort, pucQueue;
   INT Total_Packet_Number = 0 ;

	DBGPRINT(RT_DEBUG_OFF, ("\n"));

#ifdef DOT11_N_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("HT Operating Mode : %d\n"
			, pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode));
	DBGPRINT(RT_DEBUG_OFF, ("\n"));
#endif /* DOT11_N_SUPPORT */
	
	DBGPRINT(RT_DEBUG_OFF
	, ("\n%-19s\t%-10s\t%-5s\t%-5s\t%-5s\t%-5s\t%-5s\t%-5s\t%-5s\t%-5s\t%-5s\t%-6s\t%-6s"
	, "MAC", "EntryType", "AID", "BSS", "PSM", "ipsm", "iips"
	, "sktx", "redt", "port", "queu", "pktnum", "psnum"));
#ifdef UAPSD_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("\t%-7s", "APSD"));
#endif /* UAPSD_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("\n"));
		
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
           PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
           STA_TR_ENTRY *tr_entry = &pAd->MacTab.tr_entry[pEntry->wcid];
           Total_Packet_Number = 0 ;

		if ((ent_type == ENTRY_NONE))
		{
			/* dump all MacTable entries */
			if (pEntry->EntryType == ENTRY_NONE) 
				continue;
		} 
		else 
		{
			/* dump MacTable entries which match the EntryType */
			if (pEntry->EntryType != ent_type)
				continue;

			if ((IS_ENTRY_CLIENT(pEntry) || IS_ENTRY_APCLI(pEntry)) 
				&& (pEntry->Sst != SST_ASSOC))
				continue;

#ifdef MAC_REPEATER_SUPPORT
			if (bReptCli == FALSE)
			{	
				/* only dump the apcli entry which not a RepeaterCli */
				if (IS_ENTRY_APCLI(pEntry) && (pEntry->bReptCli == TRUE))
					continue;
			}
#endif /* MAC_REPEATER_SUPPORT */
		}

		NdisZeroMemory(&tb_entry, sizeof(tb_entry));      
		if (mt_wtbl_get_entry234(pAd, pEntry->wcid, &tb_entry) == FALSE) 
		{
			 DBGPRINT(RT_DEBUG_ERROR, ("%s():Cannot found WTBL2/3/4\n",__FUNCTION__));
			 return FALSE;
		}
		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0]+12, &dw3->word);

		//get PSE register

		//      rPseRdTabAccessReg.field.rd_kick_busy=1;
		//      rPseRdTabAccessReg.field.rd_tag=pEntry->wcid;
		rPseRdTabAccessReg = PSE_RTA_RD_KICK_BUSY |PSE_RTA_TAG(pEntry->wcid); 
		RTMP_IO_WRITE32(pAd, PSE_RTA,rPseRdTabAccessReg);

		do
		{          
			RTMP_IO_READ32(pAd,PSE_RTA,&rPseRdTabAccessReg);

			pfgForce = ( BOOLEAN ) GET_PSE_RTA_RD_RULE_F(rPseRdTabAccessReg);
			pucPort  = ( UCHAR )  GET_PSE_RTA_RD_RULE_PID(rPseRdTabAccessReg);
			pucQueue = ( UCHAR )  GET_PSE_RTA_RD_RULE_QID(rPseRdTabAccessReg);
		}      
		while ( GET_PSE_RTA_RD_KICK_BUSY(rPseRdTabAccessReg) == 1 );

		Total_Packet_Number = Total_Packet_Number + tr_entry->ps_queue.Number;
		for (j = 0; j < WMM_QUE_NUM; j++)
			Total_Packet_Number = Total_Packet_Number + tr_entry->tx_queue[j].Number;

		DBGPRINT(RT_DEBUG_OFF, ("%02X:%02X:%02X:%02X:%02X:%02X", PRINT_MAC(pEntry->Addr)));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-10x", pEntry->EntryType));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-5d", (int)pEntry->Aid));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-5d", (int)pEntry->func_tb_idx));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-5d", (int)pEntry->PsMode));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-5d", (int)dw3->field.i_psm));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-5d", (int)dw3->field.du_i_psm));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-5d", (int)dw3->field.skip_tx));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-5d", (int)pfgForce));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-5d", (int)pucPort));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-5d", (int)pucQueue));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-6d", (int)Total_Packet_Number));
		DBGPRINT(RT_DEBUG_OFF, ("\t%-6d", (int)tr_entry->ps_queue.Number));
#ifdef UAPSD_SUPPORT
		DBGPRINT(RT_DEBUG_OFF, ("\t%d,%d,%d,%d"
			, (int)pEntry->bAPSDCapablePerAC[QID_AC_BE]
			, pEntry->bAPSDCapablePerAC[QID_AC_BK]
			, pEntry->bAPSDCapablePerAC[QID_AC_VI]
			, pEntry->bAPSDCapablePerAC[QID_AC_VO]));
#endif /* UAPSD_SUPPORT */
		DBGPRINT(RT_DEBUG_OFF, ("\n"));
	}

	return TRUE;
}


INT Show_PSTable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 ent_type = ENTRY_CLIENT;
	
	DBGPRINT(RT_DEBUG_ERROR, ("%s(): arg=%s\n", __FUNCTION__, (arg == NULL ? "" : arg)));
	if (arg && strlen(arg)) {
		if (rtstrcasecmp(arg, "sta") == TRUE)
			ent_type = ENTRY_CLIENT;
		else if (rtstrcasecmp(arg, "ap") == TRUE)
			ent_type = ENTRY_AP;
		else
			ent_type = ENTRY_NONE;
	}

	DBGPRINT(RT_DEBUG_OFF, ("Dump MacTable entries info, EntType=0x%x\n", ent_type));
	
	return dump_ps_table(pAd, ent_type, FALSE);
}
#endif /* MT_MAC */


#ifdef DOT11_N_SUPPORT
INT Show_BaTable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT i, j;
	BA_ORI_ENTRY *pOriBAEntry;
	BA_REC_ENTRY *pRecBAEntry;
	RTMP_STRING tmpBuf[6];
	
	for (i=0; i < MAX_LEN_OF_MAC_TABLE; i++)
	{
		PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
		if (IS_ENTRY_NONE(pEntry))
			continue;

		if ((IS_ENTRY_CLIENT(pEntry) || IS_ENTRY_APCLI(pEntry))
			&& (pEntry->Sst != SST_ASSOC))
			continue;

		if (IS_ENTRY_APCLI(pEntry))
			strncpy(tmpBuf, "ApCli", sizeof(tmpBuf)-1);
		else if (IS_ENTRY_WDS(pEntry))
			strncpy(tmpBuf, "WDS", sizeof(tmpBuf)-1);
		else if (IS_ENTRY_MESH(pEntry))
			strncpy(tmpBuf, "Mesh", sizeof(tmpBuf)-1);
		else if (IS_ENTRY_AP(pEntry))
			strncpy(tmpBuf, "AP", sizeof(tmpBuf)-1);
		else
			strncpy(tmpBuf, "STA", sizeof(tmpBuf)-1);

		tmpBuf[sizeof(tmpBuf)-1] = 0;
	
		DBGPRINT(RT_DEBUG_OFF, ("%02X:%02X:%02X:%02X:%02X:%02X (Aid = %d) (%s) -\n",
			PRINT_MAC(pEntry->Addr), pEntry->Aid, tmpBuf));

		DBGPRINT(RT_DEBUG_OFF, ("[Recipient]\n"));
		for (j=0; j < NUM_OF_TID; j++)
		{
			if (pEntry->BARecWcidArray[j] != 0)
			{
				pRecBAEntry =&pAd->BATable.BARecEntry[pEntry->BARecWcidArray[j]];
				DBGPRINT(RT_DEBUG_OFF, (
					"TID=%d, BAWinSize=%d, LastIndSeq=%d, ReorderingPkts=%d\n"
					, j, pRecBAEntry->BAWinSize, pRecBAEntry->LastIndSeq
					, pRecBAEntry->list.qlen));
			}
		}
		DBGPRINT(RT_DEBUG_OFF, ("\n"));

		DBGPRINT(RT_DEBUG_OFF, ("[Originator]\n"));
		for (j=0; j < NUM_OF_TID; j++)
		{
			if (pEntry->BAOriWcidArray[j] != 0)
			{
				pOriBAEntry =&pAd->BATable.BAOriEntry[pEntry->BAOriWcidArray[j]];
				DBGPRINT(RT_DEBUG_OFF
					, ("TID=%d, BAWinSize=%d, StartSeq=%d, CurTxSeq=%d\n"
					, j, pOriBAEntry->BAWinSize, pOriBAEntry->Sequence
					, pAd->MacTab.tr_entry[pEntry->wcid].TxSeq[j]));
			}
		}
		DBGPRINT(RT_DEBUG_OFF, ("\n\n"));
	}

	return TRUE;
}
#endif /* DOT11_N_SUPPORT */


#ifdef MT_MAC
INT show_wtbl_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT i, start, end, idx = -1;
	//WTBL_ENTRY wtbl_entry;

	DBGPRINT(RT_DEBUG_OFF, ("%s(): arg=%s\n", __FUNCTION__, (arg == NULL ? "" : arg)));
	if (arg!=NULL && strlen(arg)) {
		idx = simple_strtoul(arg, NULL, 10);
		start = end = idx;
	} else {
		start = 0;
		end = pAd->mac_ctrl.wtbl_entry_cnt[0] - 1;
	}

	DBGPRINT(RT_DEBUG_OFF, ("Dump WTBL entries info, start=%d, end=%d, idx=%d\n", 
				start, end, idx));

	dump_wtbl_base_info(pAd);
	for (i = start; i <= end; i++)
	{
#if 1
		dump_wtbl_info(pAd, i);
#else
		DBGPRINT(RT_DEBUG_OFF, ("WLAN IDX:%d\n", i));

		NdisZeroMemory(&wtbl_entry, sizeof(WTBL_ENTRY));
		mt_wtbl_get_entry234(pAd, i, &wtbl_entry);
#endif	
	}
	return TRUE;
}


INT show_temp_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT32 temp;
	temp = MtAsicGetThemalSensor(pAd, 0);
	
	DBGPRINT(RT_DEBUG_OFF, ("Temperture: %d\n", temp));
	return TRUE;
}

INT show_mib_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 mac_val, idx, ampdu_cnt[5];
	UINT32 msdr6, msdr7, msdr8, msdr9, msdr10, msdr16, msdr17, msdr18;
	UINT32 mbxsdr[4][3];
	UINT32 mbtcr[16], mbtbcr[16], mbrcr[16], mbrbcr[16];
	UINT32 btcr[4], btbcr[4], brcr[4], brbcr[4];
	
	RTMP_IO_READ32(pAd, MIB_MSCR, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("MIB Status Control=0x%x\n", mac_val));
	RTMP_IO_READ32(pAd, MIB_MPBSCR, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("MIB Per-BSS Status Control=0x%x\n", mac_val));

	RTMP_IO_READ32(pAd, MIB_MSDR6, &msdr6);
	RTMP_IO_READ32(pAd, MIB_MSDR7, &msdr7);
	RTMP_IO_READ32(pAd, MIB_MSDR8, &msdr8);
	RTMP_IO_READ32(pAd, MIB_MSDR9, &msdr9);
	RTMP_IO_READ32(pAd, MIB_MSDR10, &msdr10);
	RTMP_IO_READ32(pAd, MIB_MSDR16, &msdr16);
	RTMP_IO_READ32(pAd, MIB_MSDR17, &msdr17);
	RTMP_IO_READ32(pAd, MIB_MSDR18, &msdr18);
	DBGPRINT(RT_DEBUG_OFF, ("===Phy/Timing Related Counters===\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tChannelIdleCnt=0x%x\n", msdr6 & 0xffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tCCA_NAV_Tx_Time=0x%x\n", msdr9 & 0xffffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tRx_MDRDY_CNT=0x%x\n", msdr10 & 0x3ffffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tPrim CCA Time=0x%x\n", msdr16 & 0xffffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tSec CCA Time=0x%x\n", msdr17 & 0xffffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tPrim ED Time=0x%x\n", msdr18 & 0xffffff));
	
	DBGPRINT(RT_DEBUG_OFF, ("===Tx Related Counters(Generic)===\n"));
	RTMP_IO_READ32(pAd, MIB_MSDR0, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tBeaconTxCnt=0x%x\n", mac_val));
	RTMP_IO_READ32(pAd, MIB_MDR0, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tTx 40MHz Cnt=0x%x\n", (mac_val >> 16) & 0xffff));
	RTMP_IO_READ32(pAd, MIB_MDR1, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tTx 80MHz Cnt=0x%x\n", mac_val& 0xffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tTx 160MHz Cnt=0x%x\n", (mac_val >> 16) & 0xffff));
	
	DBGPRINT(RT_DEBUG_OFF, ("===AMPDU Related Counters===\n"));
	RTMP_IO_READ32(pAd, MIB_MSDR12, &ampdu_cnt[0]);
	RTMP_IO_READ32(pAd, MIB_MSDR14, &ampdu_cnt[1]);
	RTMP_IO_READ32(pAd, MIB_MSDR15, &ampdu_cnt[2]);
	RTMP_IO_READ32(pAd, MIB_MDR2, &ampdu_cnt[3]);
	RTMP_IO_READ32(pAd, MIB_MDR3, &ampdu_cnt[4]);
	DBGPRINT(RT_DEBUG_OFF, ("\tRx BA_Cnt=0x%x\n", ampdu_cnt[0] & 0xffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tTx AMPDU_Burst_Cnt=0x%x\n", (ampdu_cnt[0] >> 16 ) & 0xffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tTx AMPDU_Pkt_Cnt=0x%x\n", ampdu_cnt[1] & 0xffffff));	
	DBGPRINT(RT_DEBUG_OFF, ("\tAMPDU SuccessCnt=0x%x\n", ampdu_cnt[2] & 0xffffff));	
	DBGPRINT(RT_DEBUG_OFF, ("\tTx Agg Range: \t1 \t2~5 \t6~15 \t16~\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\t0x%x \t0x%x \t0x%x \t0x%x \n", 
					(ampdu_cnt[3]) & 0xffff, (ampdu_cnt[3] >> 16) & 0xffff,
					(ampdu_cnt[4]) & 0xffff, (ampdu_cnt[4] >> 16) & 0xfff));

	DBGPRINT(RT_DEBUG_OFF, ("===Rx Related Counters(Generic)===\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tVector Overflow Drop Cnt=0x%x\n", (msdr6 >> 16 ) & 0xffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tVector Missmacth Cnt=0x%x\n", (msdr7 >> 16 ) & 0xffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tDelimiter Fail Cnt=0x%x\n", msdr8& 0xffff));
	RTMP_IO_READ32(pAd, MIB_MSDR4, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tRxFifoFullCnt=0x%x\n", mac_val & 0xffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tRxFCSErrCnt=0x%x\n", (mac_val >> 16 ) & 0xffff));
	RTMP_IO_READ32(pAd, MIB_MSDR5, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tRxMPDUCnt=0x%x\n", mac_val & 0xffff));
	DBGPRINT(RT_DEBUG_OFF, ("\tPFDropCnt=0x%x\n", (mac_val >> 16 ) & 0x00ff));
	RTMP_IO_READ32(pAd, MIB_MSDR22, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tRx AMPDU Cnt=0x%x\n", mac_val & 0xffff));
	RTMP_IO_READ32(pAd, MIB_MSDR23, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tRx Total ByteCnt=0x%x\n", mac_val));

	for (idx = 0; idx < 4; idx++) {
		RTMP_IO_READ32(pAd, WTBL_BTCRn + idx * 4, &btcr[idx]);
		RTMP_IO_READ32(pAd, WTBL_BTBCRn + idx * 4, &btbcr[idx]);
		RTMP_IO_READ32(pAd, WTBL_BRCRn + idx * 4, &brcr[idx]);
		RTMP_IO_READ32(pAd, WTBL_BRBCRn + idx * 4, &brbcr[idx]);
	}
	DBGPRINT(RT_DEBUG_OFF, ("===Per-BSS Related Tx/Rx Counters===\n"));
	DBGPRINT(RT_DEBUG_OFF, ("BSS Idx   TxCnt/DataCnt  TxByteCnt  RxCnt/DataCnt  RxByteCnt\n"));
	for (idx = 0; idx < 4; idx++) {
		DBGPRINT(RT_DEBUG_OFF, ("%d\t 0x%x/0x%x\t 0x%x \t 0x%x/0x%x \t 0x%x\n",
					idx, (btcr[idx] >> 16) & 0xffff, btcr[idx] & 0xffff, btbcr[idx],
					(brcr[idx] >> 16) & 0xffff, brcr[idx] & 0xffff, brbcr[idx]));
	}


	for (idx = 0; idx < 4; idx++)
	{
		RTMP_IO_READ32(pAd, MIB_MB0SDR0 + idx * 0x10, &mbxsdr[idx][0]);
		RTMP_IO_READ32(pAd, MIB_MB0SDR1 + idx * 0x10, &mbxsdr[idx][1]);
		RTMP_IO_READ32(pAd, MIB_MB0SDR2 + idx * 0x10, &mbxsdr[idx][2]);
	}
	DBGPRINT(RT_DEBUG_OFF, ("===Per-MBSS Related MIB Counters===\n"));
	DBGPRINT(RT_DEBUG_OFF, ("BSS Idx   RTSTx/RetryCnt  BAMissCnt  AckFailCnt  FrmRetry1/2Cnt\n"));
	for (idx = 0; idx < 4; idx++) {
		DBGPRINT(RT_DEBUG_OFF, ("%d:\t0x%x/0x%x  0x%x \t 0x%x \t  0x%x/0x%x\n",
					idx, mbxsdr[idx][0], (mbxsdr[idx][0] >> 16) & 0xffff,
					mbxsdr[idx][1], (mbxsdr[idx][1] >> 16) & 0xffff,
					mbxsdr[idx][2], (mbxsdr[idx][2] >> 16) & 0xffff));
	}


	for (idx = 0; idx < 8; idx++) {
		RTMP_IO_READ32(pAd, WTBL_MBTCRn + idx * 4, &mbtcr[idx]);
		RTMP_IO_READ32(pAd, WTBL_MBRCRn + idx * 4, &mbrcr[idx]);
	}
	
	for (idx = 0; idx < 16; idx++) {
		RTMP_IO_READ32(pAd, WTBL_MBTBCRn + idx * 4, &mbtbcr[idx]);
		RTMP_IO_READ32(pAd, WTBL_MBRBCRn + idx * 4, &mbrbcr[idx]);
	}	
	DBGPRINT(RT_DEBUG_OFF, ("===Per-MBSS Related Tx/Rx Counters===\n"));
	DBGPRINT(RT_DEBUG_OFF, ("MBSSIdx   TxDataCnt  TxByteCnt  RxDataCnt  RxByteCnt\n"));
	for (idx = 0; idx < 16; idx++) {
		DBGPRINT(RT_DEBUG_OFF, ("%d\t 0x%x\t 0x%x \t 0x%x \t 0x%x\n",
					idx, 
					((idx % 2 == 1) ? (mbtcr[idx/2] >> 16) & 0xffff : mbtcr[idx/2] & 0xffff), 
					mbtbcr[idx],
					((idx % 2 == 1)  ? (mbrcr[idx/2] >> 16) & 0xffff : mbrcr[idx/2] & 0xffff), 
					mbrbcr[idx]));
	}
	
	return TRUE;
}


INT32 ShowTmacInfo(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 Value;

	RTMP_IO_READ32(pAd, TMAC_TCR, &Value); 
	DBGPRINT(RT_DEBUG_OFF, ("TX Stream = %d\n", GET_TMAC_TCR_TX_STREAM_NUM(Value) + 1));
	DBGPRINT(RT_DEBUG_OFF, ("TX RIFS Enable = %d\n", GET_TX_RIFS_EN(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("RX RIFS Mode = %d\n", GET_RX_RIFS_MODE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("TXOP TBTT Control = %d\n", GET_TXOP_TBTT_CONTROL(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("TXOP TBTT Stop Control = %d\n", GET_TBTT_TX_STOP_CONTROL(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("TXOP Burst Stop = %d\n", GET_TXOP_BURST_STOP(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("RDG Mode = %d\n", GET_RDG_RA_MODE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("RDG Responser Enable = %d\n", GET_RDG_RESP_EN(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Smoothing = %d\n", GET_SMOOTHING(Value))); 

	RTMP_IO_READ32(pAd, TMAC_PSCR, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("AP Power Save RXPE Off Time(unit 2us) = %d\n", GET_APS_OFF_TIME(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("AP Power Save RXPE On Time(unit 2us) = %d\n", APS_ON_TIME(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("AP Power Save Halt Time (unit 32us) = %d\n", GET_APS_HALT_TIME(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("AP Power Enable = %d\n", GET_APS_EN(Value)));    

	RTMP_IO_READ32(pAd, TMAC_ACTXOPLR1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("AC0 TXOP = 0x%x (unit: 32us)\n", GET_AC0LIMIT(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("AC1 TXOP = 0x%x (unit: 32us)\n", GET_AC1LIMIT(Value)));
			
	RTMP_IO_READ32(pAd, TMAC_ACTXOPLR0, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("AC2 TXOP = 0x%x (unit: 32us)\n", GET_AC2LIMIT(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("AC3 TXOP = 0x%x (unit: 32us)\n", GET_AC3LIMIT(Value)));
				
	RTMP_IO_READ32(pAd, TMAC_ACTXOPLR3, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("AC10 TXOP = 0x%x (unit: 32us)\n", GET_AC10LIMIT(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("AC11 TXOP = 0x%x (unit: 32us)\n", GET_AC11LIMIT(Value)));
		
	RTMP_IO_READ32(pAd, TMAC_ACTXOPLR2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("AC12 TXOP = 0x%x (unit: 32us)\n", GET_AC12LIMIT(Value)));
			DBGPRINT(RT_DEBUG_OFF, ("AC13 TXOP = 0x%x (unit: 32us)\n", GET_AC13LIMIT(Value)));

	RTMP_IO_READ32(pAd, TMAC_ICR, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("EIFS Time (unit: 1us) = %d\n", GET_ICR_EIFS_TIME(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("RIFS Time (unit: 1us) = %d\n", GET_ICR_RIFS_TIME(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("SIFS Time (unit: 1us) = %d\n", GET_ICR_SIFS_TIME(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("SLOT Time (unit: 1us) = %d\n", GET_ICR_SLOT_TIME(Value))); 

	RTMP_IO_READ32(pAd, ATCR, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Aggregation Timeout (unit: 50ns) = 0x%x\n", GET_AGG_TOUT(Value))); 

	return 0;
}


INT32 ShowAggInfo(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 Value;

	RTMP_IO_READ32(pAd, AGG_PCR, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("MM Protection = %d\n", GET_MM_PROTECTION(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("GF Protection = %d\n", GET_GF_PROTECTION(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Protection Mode = %d\n", GET_PROTECTION_MODE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("BW40 Protection = %d\n", GET_BW40_PROTECTION(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("RIFS Protection = %d\n", GET_RIFS_PROTECTION(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("BW80 Protection = %d\n", GET_BW80_PROTECTION(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("BW160 Protection = %d\n", GET_BW160_PROTECTION(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("ERP Protection = 0x%x\n", GET_ERP_PROTECTION(Value)));   

	RTMP_IO_READ32(pAd, AGG_PCR1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("RTS Threshold(packet length) = 0x%x\n", GET_RTS_THRESHOLD(Value)));  
	DBGPRINT(RT_DEBUG_OFF, ("RTS PKT Nums Threshold = %d\n", GET_RTS_PKT_NUM_THRESHOLD(Value)));
	RTMP_IO_READ32(pAd, AGG_MRCR, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("RTS Retry Count Limit = %d\n", GET_RTS_RTY_CNT_LIMIT(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("BAR Frame Tx Count Limit = %d\n", GET_BAR_TX_CNT_LIMIT(Value))); 
	RTMP_IO_READ32(pAd, AGG_ACR, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("AMPDU No BA Rule = %d\n", GET_AMPDU_NO_BA_RULE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("AMPDU No BA AR Rule = %d\n", GET_AGG_ACR_AMPDU_NO_BA_AR_RULE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("BAR Tx Rate = 0x%x\n", GET_BAR_RATE_TX_RATE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("BAR Tx Mode = 0x%x\n", GET_BAR_RATE_TX_MODE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("BAR Nsts = %d\n", GET_BAR_RATE_NSTS(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("BAR STBC = %d\n", GET_BAR_RATE_STBC(Value)));

	RTMP_IO_READ32(pAd, AGG_AALCR, &Value); 
	DBGPRINT(RT_DEBUG_OFF, ("AC0 Agg limit = %d\n", GET_AC0_AGG_LIMIT(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("AC1 Agg limit = %d\n", GET_AC1_AGG_LIMIT(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("AC2 Agg limit = %d\n", GET_AC2_AGG_LIMIT(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("AC3 Agg limit = %d\n", GET_AC3_AGG_LIMIT(Value)));

	RTMP_IO_READ32(pAd, AGG_AALCR1, &Value); 
	DBGPRINT(RT_DEBUG_OFF, ("AC10 Agg limit = %d\n", GET_AC10_AGG_LIMIT(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("AC11 Agg limit = %d\n", GET_AC11_AGG_LIMIT(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("AC12 Agg limit = %d\n", GET_AC12_AGG_LIMIT(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("AC13 Agg limit = %d\n", GET_AC13_AGG_LIMIT(Value)));

	RTMP_IO_READ32(pAd, AGG_AWSCR, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Winsize0 limit = %d\n", GET_WINSIZE0(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("Winsize1 limit = %d\n", GET_WINSIZE1(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("Winsize2 limit = %d\n", GET_WINSIZE2(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("Winsize3 limit = %d\n", GET_WINSIZE3(Value))); 

	RTMP_IO_READ32(pAd, AGG_AWSCR1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Winsize4 limit = %d\n", GET_WINSIZE4(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("Winsize5 limit = %d\n", GET_WINSIZE5(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("Winsize6 limit = %d\n", GET_WINSIZE6(Value))); 
	DBGPRINT(RT_DEBUG_OFF, ("Winsize7 limit = %d\n", GET_WINSIZE7(Value))); 

	return 0;
}


INT ShowManualTxOP(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 txop;	

    DBGPRINT(RT_DEBUG_OFF, ("CURRENT: ManualTxOP = %d\n", pAd->CommonCfg.ManualTxop));
    DBGPRINT(RT_DEBUG_OFF, ("       : bEnableTxBurst = %d\n", pAd->CommonCfg.bEnableTxBurst));
    DBGPRINT(RT_DEBUG_OFF, ("       : MacTab.Size = %d\n", pAd->MacTab.Size));
    DBGPRINT(RT_DEBUG_OFF, ("       : RDG_ACTIVE = %d\n", RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE)));
	RTMP_IO_READ32(pAd, TMAC_ACTXOPLR1, &txop);
    DBGPRINT(RT_DEBUG_OFF, ("       : AC0 TxOP = 0x%x\n", GET_AC0LIMIT(txop)));
    DBGPRINT(RT_DEBUG_OFF, ("       : AC1 TxOP = 0x%x\n", GET_AC1LIMIT(txop)));

    return TRUE;
}
static VOID DumpPseFrameInfo(RTMP_ADAPTER *pAd, UINT8 PID, UINT8 QID)
{
	UINT32 FirstFID, CurFID, NextFID, FrameNums = 0;
	UINT32 Value;

	DBGPRINT(RT_DEBUG_OFF, ("PID = %d, QID = %d\n", PID, QID));

	RTMP_IO_READ32(pAd, C_GFF, &Value);
	Value &= ~GET_QID_MASK;
	Value |= SET_GET_QID(QID);
	Value &= ~GET_PID_MASK;
	Value |= SET_GET_PID(PID);
	RTMP_IO_WRITE32(pAd, C_GFF, Value);

	RTMP_IO_READ32(pAd, C_GFF, &Value);
	FirstFID = GET_FIRST_FID(Value);

	if (FirstFID == 0xfff)
	{
		DBGPRINT(RT_DEBUG_OFF, ("FrameNums = %d\n", FrameNums));
		return;
	}

	CurFID = FirstFID;
	FrameNums++;

	DBGPRINT(RT_DEBUG_OFF, ("FID = %d", CurFID));

	while (1)
	{
		RTMP_IO_READ32(pAd, C_GF, &Value);
		Value &= ~CURR_FID_MASK;
		Value |= SET_CURR_FID(CurFID);
		RTMP_IO_WRITE32(pAd, C_GF, Value);
		RTMP_IO_READ32(pAd, C_GF, &Value);
		NextFID = GET_RETURN_FID(Value);

		if (NextFID == 0xfff)
		{
			DBGPRINT(RT_DEBUG_OFF, ("\nFrameNums = %d\n", FrameNums));
			return;
		}
		else
		{
			CurFID = NextFID;
			DBGPRINT(RT_DEBUG_OFF, (", FID = %d\n", CurFID));
			FrameNums++;
		}

	}

	DBGPRINT(RT_DEBUG_OFF, ("\nFrameNums = %d\n", FrameNums));
}


INT32 ShowPseInfo(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 Value;

	RTMP_IO_READ32(pAd, BC, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Total Page Nums = 0x%x, Page Size = %d\n", GET_TOTAL_PAGE_NUM(Value), GET_PAGE_SIZE_CFG(Value)));

	RTMP_IO_READ32(pAd, FC_FFC, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Free page counter status = 0x%x\n", GET_FREE_PAGE_CNT(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Free for all(FFA) counter status = 0x%x\n", GET_FFA_CNT(Value)));

	RTMP_IO_READ32(pAd, FC_FRP, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Reserve priority:\n"));	
	DBGPRINT(RT_DEBUG_OFF, ("P0(HIF) = 0x%x, ", GET_RSRV_PRI_P0(Value)));	
	DBGPRINT(RT_DEBUG_OFF, ("P1(MCU) = 0x%x\n", GET_RSRV_PRI_P1(Value)));	
	DBGPRINT(RT_DEBUG_OFF, ("P2 RQ0(Rx Data) = 0x%x, ", GET_RSRV_PRI_P2_RQ0(Value)));	
	DBGPRINT(RT_DEBUG_OFF, ("P2 RQ1(RxV) = 0x%x, ", GET_RSRV_PRI_P2_RQ1(Value)));	
	DBGPRINT(RT_DEBUG_OFF, ("P2 RQ2(TxS)  = 0x%x\n", GET_RSRV_PRI_P2_RQ2(Value)));	

	RTMP_IO_READ32(pAd, FC_P0, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("P0(HIF):\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tmin reserve setting = 0x%x\n", GET_MIN_RSRV_P0(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("\tmax quota setting = 0x%x\n", GET_MAX_QUOTA_P0(Value)));
	RTMP_IO_READ32(pAd, FC_RP0P1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\treserve pages = 0x%x\n", GET_RSRV_CNT_P0(Value))); 
	RTMP_IO_READ32(pAd, FC_SP0P1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\tused pages = 0x%x\n", GET_SRC_CNT_P0(Value)));

	RTMP_IO_READ32(pAd, FC_P1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("P1(MCU):\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tmin reserve setting = 0x%x\n", GET_MIN_RSRV_P1(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("\tmax quota setting = 0x%x\n", GET_MAX_QUOTA_P1(Value)));
	RTMP_IO_READ32(pAd, FC_RP0P1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\treserve pages = 0x%x\n", GET_RSRV_CNT_P1(Value))); 
	RTMP_IO_READ32(pAd, FC_SP0P1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\tused pages = 0x%x\n", GET_SRC_CNT_P1(Value)));

	RTMP_IO_READ32(pAd, FC_P2Q0, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("P2 RQ0(Rx Data):\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tmin reserve setting = 0x%x\n", GET_MIN_RSRV_P2_RQ0(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("\tmax quota setting = 0x%x\n", GET_MAX_QUOTA_P2_RQ0(Value)));
	RTMP_IO_READ32(pAd, FC_RP2Q0Q1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\treserve pages = 0x%x\n", GET_RSRV_CNT_P2_RQ0(Value))); 
	RTMP_IO_READ32(pAd, FC_SP2Q0Q1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\tused pages = 0x%x\n", GET_SRC_CNT_P2_RQ0(Value)));


	RTMP_IO_READ32(pAd, FC_P2Q1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("P2 RQ1(RxV):\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tmin reserve setting = 0x%x\n", GET_MIN_RSRV_P2_RQ1(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("\tmax quota setting = 0x%x\n", GET_MAX_QUOTA_P2_RQ1(Value)));
	RTMP_IO_READ32(pAd, FC_RP2Q0Q1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\treserve pages = 0x%x\n", GET_RSRV_CNT_P2_RQ1(Value))); 
	RTMP_IO_READ32(pAd, FC_SP2Q0Q1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\tused pages = 0x%x\n", GET_SRC_CNT_P2_RQ1(Value)));

	
	RTMP_IO_READ32(pAd, FC_P2Q2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("P2 RQ2(TxS):\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tmin reserve setting = 0x%x\n", GET_MIN_RSRV_P2_RQ2(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("\tmax quota setting = 0x%x\n", GET_MAX_QUOTA_P2_RQ2(Value)));
	RTMP_IO_READ32(pAd, FC_RP2Q2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\treserve pages = 0x%x\n", GET_RSRV_CNT_P2_RQ2(Value))); 
	RTMP_IO_READ32(pAd, FC_SP2Q2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("\tused pages = 0x%x\n", GET_SRC_CNT_P2_RQ2(Value)));

	/* HIF port frame information */
	DBGPRINT(RT_DEBUG_OFF, ("HIF HTX_L0 frame information\n"));
	DumpPseFrameInfo(pAd, 0, 0);

	DBGPRINT(RT_DEBUG_OFF, ("HIF HTX_HI frame information\n"));
	DumpPseFrameInfo(pAd, 0, 1);

	DBGPRINT(RT_DEBUG_OFF, ("HIF HRX frame information\n"));
	DumpPseFrameInfo(pAd, 0, 2);

	/* MCU port frame information */
	DBGPRINT(RT_DEBUG_OFF, ("MCU CTX_Q0 frame information\n"));
	DumpPseFrameInfo(pAd, 1, 0);

	DBGPRINT(RT_DEBUG_OFF, ("MCU CTX_Q1 frame information\n"));
	DumpPseFrameInfo(pAd, 1, 1);

	DBGPRINT(RT_DEBUG_OFF, ("MCU CTX_Q2 frame information\n"));
	DumpPseFrameInfo(pAd, 1, 2);

	DBGPRINT(RT_DEBUG_OFF, ("MCU CTX_Q3 frame information\n"));
	DumpPseFrameInfo(pAd, 1, 3);

	/* WLAN port frame information */
	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC0 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 0);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC1 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 1);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC2 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 2);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC3 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 3);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC4 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 4);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC5 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 5);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC6 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 6);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN BC/MC frame information\n"));
	DumpPseFrameInfo(pAd, 2, 7);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN Beacon frame information\n"));
	DumpPseFrameInfo(pAd, 2, 8);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC10 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 9);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC11 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 10);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC12 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 11);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC13 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 12);

	DBGPRINT(RT_DEBUG_OFF, ("WLAN AC14 frame information\n"));
	DumpPseFrameInfo(pAd, 2, 13);

	return 0;
}


INT32 ShowPseData(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CHAR *Param;
	UINT8 StartFID, CurFID, FrameNums, Index, DwIndex;
	UINT32 Value;
	UINT32 PseData[4 * 8];
	INT ret;
	LONG ret_value;

	Param = rstrtok(arg, ",");

	if (Param != NULL)
	{
		ret = kstrtol(Param, 10, &ret_value);
		StartFID = (UINT8)ret_value;
	}
	else
	{
		goto error;
	}

	Param = rstrtok(NULL, ",");

	if (Param != NULL)
	{
		ret = kstrtol(Param, 10, &ret_value);
		FrameNums = (UINT8)ret_value;
	}
	else
	{
		goto error;
	}

	CurFID = StartFID;

	for (Index = 0; Index < FrameNums; Index++)
	{
		for (DwIndex = 0; DwIndex < 8; DwIndex++)
		{
			RTMP_IO_READ32(pAd, (MT_PCI_REMAP_ADDR_1 + (((CurFID) * 128)) + (DwIndex * 4)),
				&PseData[DwIndex * 4]);
		}

		DBGPRINT(RT_DEBUG_OFF, ("FID:0x%x\n", CurFID));

#ifdef RT_BIG_ENDIAN
		MTMacInfoEndianChange(pAd, (PUCHAR)(PseData), TYPE_TMACINFO, 32);
#endif
		dump_tmac_info(pAd, (PUCHAR)PseData);

#ifdef RT_BIG_ENDIAN
		MTMacInfoEndianChange(pAd, (PUCHAR)(PseData), TYPE_TMACINFO, 32);
#endif
		RTMP_IO_READ32(pAd, C_GF, &Value);
		Value &= ~CURR_FID_MASK;
		Value |= SET_CURR_FID(CurFID);
		RTMP_IO_WRITE32(pAd, C_GF, Value);
		RTMP_IO_READ32(pAd, C_GF, &Value);
		CurFID = (UINT8)GET_RETURN_FID(Value);

		if (CurFID == 0xff)
			break;
	}

error:
	DBGPRINT(RT_DEBUG_OFF, ("%s: param = %s not correct\n", __FUNCTION__, arg));
	DBGPRINT(RT_DEBUG_OFF, ("%s: iwpriv ra0 show psedata=startfid,framenums\n", __FUNCTION__));
	return 0;
}


#if defined(RTMP_PCI_SUPPORT) || defined(RTMP_USB_SUPPORT)
INT32 ShowDMASchInfo(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 Value;
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_0, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue0 reservation page thd = 0x%x\n", GET_PAGE_CNT_0(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue1 reservation page thd = 0x%x\n", GET_PAGE_CNT_1(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue2 reservation page thd = 0x%x\n", GET_PAGE_CNT_2(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_3, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue3 reservation page thd = 0x%x\n", GET_PAGE_CNT_3(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_4, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue4 reservation page thd = 0x%x\n", GET_PAGE_CNT_4(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_5, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue5 reservation page thd = 0x%x\n", GET_PAGE_CNT_5(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_6, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue6 reservation page thd = 0x%x\n", GET_PAGE_CNT_6(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_7, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue7 reservation page thd = 0x%x\n", GET_PAGE_CNT_7(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_8, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue8 reservation page thd = 0x%x\n", GET_PAGE_CNT_8(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_9, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue9 reservation page thd = 0x%x\n", GET_PAGE_CNT_9(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_10, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue10 reservation page thd = 0x%x\n", GET_PAGE_CNT_10(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_11, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue11 reservation page thd = 0x%x\n", GET_PAGE_CNT_11(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_12, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue12 reservation page thd = 0x%x\n", GET_PAGE_CNT_12(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_13, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue13 reservation page thd = 0x%x\n", GET_PAGE_CNT_13(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_14, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue14 reservation page thd = 0x%x\n", GET_PAGE_CNT_14(Value)));
	RTMP_IO_READ32(pAd, MT_PAGE_CNT_15, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue15 reservation page thd = 0x%x\n", GET_PAGE_CNT_15(Value)));

	RTMP_IO_READ32(pAd, MT_QUEUE_PRIORITY_1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue0 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_0(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue1 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_1(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue2 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_2(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue3 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_3(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue4 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_4(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue5 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_5(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue6 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_6(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue7 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_7(Value)));

	RTMP_IO_READ32(pAd, MT_QUEUE_PRIORITY_2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue8 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_8(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue9 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_9(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue10 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_10(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue11 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_11(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue12 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_12(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue13 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_13(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue14 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_14(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue15 priority = 0x%x\n", GET_RG_QUEUE_PRIORITY_15(Value)));

	RTMP_IO_READ32(pAd, MT_SCH_REG_1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Free for all buffer thd = 0x%x\n", GET_RG_FFA_THD(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Packet bytes per page = %d\n", GET_RG_PAGE_SIZE(Value)));

	RTMP_IO_READ32(pAd, MT_SCH_REG_2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Max packet size in one packet = 0x%x\n", GET_RG_MAX_PKT_SIZE(Value)));

	RTMP_IO_READ32(pAd, MT_SCH_REG_4, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Force qid = %d\n", GET_FORCE_QID(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Force mode = %d\n", GET_FORCE_MODE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Bypass mode = %d\n", GET_BYPASS_MODE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Hybird mode = %d\n", GET_HYBIRD_MODE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Force in predict mode = %d\n", GET_RG_PREDICT_NO_MASK(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("SW mode = %d\n", GET_SW_MODE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Rate map(0x%x) when sw mode = 1\n", GET_RG_RATE_MAP(Value)));

	RTMP_IO_READ32(pAd, MT_GROUP_THD_0, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 0 page thd = 0x%x\n", GET_GROUP_THD_0(Value)));
	RTMP_IO_READ32(pAd, MT_GROUP_THD_1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 1 page thd = 0x%x\n", GET_GROUP_THD_1(Value)));
	RTMP_IO_READ32(pAd, MT_GROUP_THD_2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 2 page thd = 0x%x\n", GET_GROUP_THD_2(Value)));
	RTMP_IO_READ32(pAd, MT_GROUP_THD_3, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 3 page thd = 0x%x\n", GET_GROUP_THD_3(Value)));
	RTMP_IO_READ32(pAd, MT_GROUP_THD_4, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 4 page thd = 0x%x\n", GET_GROUP_THD_4(Value)));
	RTMP_IO_READ32(pAd, MT_GROUP_THD_5, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 5 page thd = 0x%x\n", GET_GROUP_THD_5(Value)));

	RTMP_IO_READ32(pAd, MT_BMAP_0, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("One queue on bit mapping(0x%x) for group 0\n", GET_RG_BMAP_0(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("One queue on bit mapping(0x%x) for group 1\n", GET_RG_BMAP_1(Value)));

	RTMP_IO_READ32(pAd, MT_BMAP_1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("One queue on bit mapping(0x%x) for group 2\n", GET_RG_BMAP_2(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("One queue on bit mapping(0x%x) for group 3\n", GET_RG_BMAP_3(Value)));

	RTMP_IO_READ32(pAd, MT_BMAP_2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("One queue on bit mapping(0x%x) for group 4\n", GET_RG_BMAP_4(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("One queue on bit mapping(0x%x) for group 5\n", GET_RG_BMAP_5(Value)));

	RTMP_IO_READ32(pAd, MT_HIGH_PRIORITY_1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue0 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_0(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue1 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_1(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue2 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_2(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue3 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_3(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue4 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_4(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue5 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_5(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue6 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_6(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue7 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_7(Value)));

	RTMP_IO_READ32(pAd, MT_HIGH_PRIORITY_2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue8 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_8(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue9 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_9(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue10 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_10(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue11 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_11(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue12 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_12(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue13 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_13(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue14 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_14(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue15 high priority = 0x%x\n", GET_RG_HIGH_PRIORITY_15(Value)));

	RTMP_IO_READ32(pAd, MT_PRIORITY_MASK, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Mask for queue priority = 0x%x\n", GET_RG_QUEUE_PRIORITY(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Mask for high priority = 0x%x\n", GET_RG_HIGH_PRIORITY(Value)));

	RTMP_IO_READ32(pAd, MT_RSV_MAX_THD, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("FFA1 max thd = 0x%x\n", GET_RG_RSV_MAX_THD(Value)));

	RTMP_IO_READ32(pAd, RSV_AC_CNT_0, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 0 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_0(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 1 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_1(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 2 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_2(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_3, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 3 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_3(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_4, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 4 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_4(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_5, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 5 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_5(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_6, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 6 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_6(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_7, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 7 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_7(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_8, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 8 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_8(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_9, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 9 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_9(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_10, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 10 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_10(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_11, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 11 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_11(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_12, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 12 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_12(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_13, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 13 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_13(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_14, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 14 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_14(Value)));
	RTMP_IO_READ32(pAd, RSV_AC_CNT_15, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Queue 15 rsv paged used count = 0x%x\n", GET_RSV_AC_CNT_15(Value)));

	RTMP_IO_READ32(pAd, SCH_DBG0_0, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Each queue rsv space if enough(0x%x)\n", GET_RSV_ENOUGH(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Group remain page great than max pkt page = %d\n", GET_GROUP_ENOUGH(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Queue remain page great than max pkt pagew = %d\n", GET_QUEUE_ENOUGH(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Pedict mode = %d\n", GET_PREDICT_MODE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Use queue reservation = %d\n", GET_REST_QUEUE_EN(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Current queue tx op time if enough = %d\n", GET_ENOUGH_TXTIME(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("Current queue pse page space if enough = %d\n", GET_ENOUGH_BUF(Value)));

	RTMP_IO_READ32(pAd, SCH_DBG_1, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Free for all buffer used counter = 0x%x\n", GET_FFA_PAGE_CNT(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_2, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Current packet tx op time = 0x%x\n", GET_PKT_TX_TIME(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_3, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("tx op time period per byte = 0x%x\n", GET_TX_TIME_PER_BYTE(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_4, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("PSE poer resrvation latched by dma scheduler = 0x%x\n", GET_PSE_RSV_SPACE(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("PSE port reservation space = 0x%x\n", GET_HIF_RSV_PCNT(Value)));
	DBGPRINT(RT_DEBUG_OFF, ("PSE poer resrvation space update = %d\n", GET_HIF_RSV_PCNT_UPDT(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_5, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Per-Queue group is enough page space for en-queue = 0x%x\n", GET_GROUP_EN(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_6, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 0 used page count = 0x%x\n", GET_USED_GROUP_0(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_7, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 1 used page count = 0x%x\n", GET_USED_GROUP_1(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_8, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 2 used page count = 0x%x\n", GET_USED_GROUP_2(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_9, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 3 used page count = 0x%x\n", GET_USED_GROUP_3(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_10, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 4 used page count = 0x%x\n", GET_USED_GROUP_4(Value)));
	RTMP_IO_READ32(pAd, SCH_DBG_11, &Value);
	DBGPRINT(RT_DEBUG_OFF, ("Group 5 used page count = 0x%x\n", GET_USED_GROUP_5(Value)));

	return TRUE;
}
#endif
#endif


INT Show_sta_tr_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT idx;
	STA_TR_ENTRY *tr_entry;
	
	for (idx = 0; idx < MAX_LEN_OF_TR_TABLE; idx++)
	{
		tr_entry = &pAd->MacTab.tr_entry[idx];
		if (IS_VALID_ENTRY(tr_entry))
			dump_tr_entry(pAd, idx, (RTMP_STRING *)__FUNCTION__, __LINE__);
	}

	return TRUE;
}


INT show_stainfo_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT i;
	ULONG DataRate=0, irqflags = 0;
	UCHAR mac_addr[MAC_ADDR_LEN];
	RTMP_STRING *token;
	CHAR sep[1] = {':'};
	MAC_TABLE_ENTRY *pEntry;
	STA_TR_ENTRY *tr_entry;

	DBGPRINT(RT_DEBUG_OFF, ("%s(): Input string=%s\n", 
				__FUNCTION__, arg));
	for (i = 0, token = rstrtok(arg, &sep[0]); token; token = rstrtok(NULL, &sep[0]), i++)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s(): token(len=%zu) =%s\n", 
					__FUNCTION__, strlen(token), token));
		if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
			return FALSE;
		AtoH(token, (&mac_addr[i]), 1);
	}

	DBGPRINT(RT_DEBUG_OFF, ("%s(): i= %d\n", __FUNCTION__, i));
	if(i != 6)
		return FALSE;

	DBGPRINT(RT_DEBUG_OFF, ("\nAddr %02x:%02x:%02x:%02x:%02x:%02x\n", 
				PRINT_MAC(mac_addr)));

	pEntry = MacTableLookup(pAd, (UCHAR *)mac_addr);
    	if (!pEntry)
		return FALSE;

	if (IS_ENTRY_NONE(pEntry)) {
		DBGPRINT(RT_DEBUG_OFF, ("Invalid MAC address!\n"));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	DBGPRINT(RT_DEBUG_OFF, ("EntryType : %d\n", pEntry->EntryType));
	DBGPRINT(RT_DEBUG_OFF, ("Entry Capability:\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tPhyMode:%-10s\n"
			, get_phymode_str(pEntry->MaxHTPhyMode.field.MODE)));
	DBGPRINT(RT_DEBUG_OFF, ("\tBW:%-6s\n", get_bw_str(pEntry->MaxHTPhyMode.field.BW)));
	DBGPRINT(RT_DEBUG_OFF, ("\tDataRate:\n"));
#ifdef DOT11_VHT_AC
	if (pEntry->MaxHTPhyMode.field.MODE == MODE_VHT)
		DBGPRINT(RT_DEBUG_OFF, ("%dS-M%d"
			, ((pEntry->MaxHTPhyMode.field.MCS>>4) + 1)
			, (pEntry->MaxHTPhyMode.field.MCS & 0xf)));
	else
#endif /* DOT11_VHT_AC */
	DBGPRINT(RT_DEBUG_OFF, (" %-6d", pEntry->MaxHTPhyMode.field.MCS));
	DBGPRINT(RT_DEBUG_OFF, (" %-6d", pEntry->MaxHTPhyMode.field.ShortGI));
	DBGPRINT(RT_DEBUG_OFF, (" %-6d\n", pEntry->MaxHTPhyMode.field.STBC));

	DBGPRINT(RT_DEBUG_OFF, ("Entry Operation Features\n"));
	DBGPRINT(RT_DEBUG_OFF
		, ("\t%-4s%-4s%-4s%-4s%-8s%-7s%-7s%-7s%-10s%-6s%-6s%-6s%-6s%-7s%-7s\n"
		, "AID", "BSS", "PSM", "WMM", "MIMOPS", "RSSI0", "RSSI1"
		, "RSSI2", "PhMd", "BW", "MCS", "SGI", "STBC", "Idle", "Rate"));
	
	DataRate=0;
	getRate(pEntry->HTPhyMode, &DataRate);
	DBGPRINT(RT_DEBUG_OFF, ("\t%-4d", (int)pEntry->Aid));
	DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->func_tb_idx));
	DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->PsMode));
	DBGPRINT(RT_DEBUG_OFF, ("%-4d"
		, (int)CLIENT_STATUS_TEST_FLAG(pEntry, fCLIENT_STATUS_WMM_CAPABLE)));
#ifdef DOT11_N_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("%-8d", (int)pEntry->MmpsMode));
#endif /* DOT11_N_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("%-7d", pEntry->RssiSample.AvgRssi[0]));
	DBGPRINT(RT_DEBUG_OFF, ("%-7d", pEntry->RssiSample.AvgRssi[1]));
	DBGPRINT(RT_DEBUG_OFF, ("%-7d", pEntry->RssiSample.AvgRssi[2]));
	DBGPRINT(RT_DEBUG_OFF, ("%-10s", get_phymode_str(pEntry->HTPhyMode.field.MODE)));
	DBGPRINT(RT_DEBUG_OFF, ("%-6s", get_bw_str(pEntry->HTPhyMode.field.BW)));
#ifdef DOT11_VHT_AC
	if (pEntry->HTPhyMode.field.MODE == MODE_VHT)
		DBGPRINT(RT_DEBUG_OFF, ("%dS-M%d"
			, ((pEntry->HTPhyMode.field.MCS>>4) + 1)
			, (pEntry->HTPhyMode.field.MCS & 0xf)));
	else
#endif /* DOT11_VHT_AC */
		DBGPRINT(RT_DEBUG_OFF, ("%-6d", pEntry->HTPhyMode.field.MCS));
	DBGPRINT(RT_DEBUG_OFF, ("%-6d", pEntry->HTPhyMode.field.ShortGI));
	DBGPRINT(RT_DEBUG_OFF, ("%-6d", pEntry->HTPhyMode.field.STBC));
	DBGPRINT(RT_DEBUG_OFF, ("%-7d", (int)(pEntry->StaIdleTimeout - pEntry->NoDataIdleCount)));
	DBGPRINT(RT_DEBUG_OFF, ("%-7d", (int)DataRate));
	DBGPRINT(RT_DEBUG_OFF, ("%-10d, %d, %d%%\n"
			, pEntry->DebugFIFOCount
			, pEntry->DebugTxCount
			, (pEntry->DebugTxCount) ?
			((pEntry->DebugTxCount-pEntry->DebugFIFOCount)*100/pEntry->DebugTxCount)
			: 0));

	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	ASSERT(pEntry->wcid <= MAX_LEN_OF_MAC_TABLE);
	tr_entry = &pAd->MacTab.tr_entry[pEntry->wcid];
	DBGPRINT(RT_DEBUG_OFF, ("Entry TxRx Info\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tEntryType : %d\n", tr_entry->EntryType));
	DBGPRINT(RT_DEBUG_OFF, ("\tHookingWdev : %p\n", tr_entry->wdev));
	DBGPRINT(RT_DEBUG_OFF, ("\tIndexing : FuncTd=%d, WCID=%d\n"
			, tr_entry->func_tb_idx, tr_entry->wcid));
	DBGPRINT(RT_DEBUG_OFF, ("Entry TxRx Features\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tIsCached, PortSecured, PsMode, LockTx, VndAth\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\t%d\t%d\t%d\t%d\t%d\n", tr_entry->isCached, tr_entry->PortSecured,
				tr_entry->PsMode, tr_entry->LockEntryTx, 
				tr_entry->bIAmBadAtheros));

	DBGPRINT(RT_DEBUG_OFF, ("\t%-6s%-6s%-6s%-6s%-6s%-6s%-6s\n"
			, "TxQId", "PktNum", "QHead", "QTail", "EnQCap", "DeQCap", "PktSeq"));
	for (i = 0; i < WMM_QUE_NUM;  i++){
		RTMP_IRQ_LOCK(&pAd->irq_lock /* &tr_entry->txq_lock[i] */, irqflags);
		DBGPRINT(RT_DEBUG_OFF, ("\t%d %6d  %p  %6p %d %d %d\n",
				i, 
				tr_entry->tx_queue[i].Number, 
				tr_entry->tx_queue[i].Head,
				tr_entry->tx_queue[i].Tail,
				tr_entry->enq_cap, tr_entry->deq_cap,
				tr_entry->TxSeq[i]));
		RTMP_IRQ_UNLOCK(&pAd->irq_lock /* &tr_entry->txq_lock[i] */, irqflags);
	}
	RTMP_IRQ_LOCK(&pAd->irq_lock /* &tr_entry->ps_queue_lock */, irqflags);
	DBGPRINT(RT_DEBUG_OFF, ("\tpsQ %6d  %p  %p %d %d  NoQ:%d\n",
				tr_entry->ps_queue.Number, 
				tr_entry->ps_queue.Head,
				tr_entry->ps_queue.Tail,
				tr_entry->enq_cap, tr_entry->deq_cap,
				tr_entry->NonQosDataSeq));
	RTMP_IRQ_UNLOCK(&pAd->irq_lock /* &tr_entry->ps_queue_lock */, irqflags);

	DBGPRINT(RT_DEBUG_OFF, ("\n"));
	return TRUE;
}


INT show_devinfo_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR *pstr;
#if defined(RTMP_MAC) || defined(RLT_MAC)
	UINT32 mac_val;
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

	DBGPRINT(RT_DEBUG_OFF, ("Device MAC\n"));
	if (pAd->OpMode == OPMODE_AP)
		pstr = "AP";
	else if (pAd->OpMode == OPMODE_STA)
		pstr = "STA";
	else
		pstr = "Unknown";
	DBGPRINT(RT_DEBUG_OFF, ("Operation Mode: %s\n", pstr));

	pstr = wmode_2_str(pAd->CommonCfg.PhyMode);
	if (pstr) {
		DBGPRINT(RT_DEBUG_OFF, ("WirelessMode: %s(%d)\n", pstr, pAd->CommonCfg.PhyMode));
		os_free_mem(pAd, pstr);
	}

	DBGPRINT(RT_DEBUG_OFF, ("Channel: %d\n", pAd->CommonCfg.Channel));
	DBGPRINT(RT_DEBUG_OFF, ("\tCentralChannel: %d\n", pAd->CommonCfg.CentralChannel));
#ifdef DOT11_VHT_AC
	if (WMODE_CAP_AC(pAd->CommonCfg.PhyMode))
		DBGPRINT(RT_DEBUG_OFF, ("\tVHT CentralChannel: %d\n", pAd->CommonCfg.vht_cent_ch));
#endif /* DOT11_VHT_AC */
	DBGPRINT(RT_DEBUG_OFF, ("\tRF Channel: %d\n", pAd->LatchRfRegs.Channel));

	DBGPRINT(RT_DEBUG_OFF, ("Bandwidth\n"));
	pstr = (pAd->CommonCfg.RegTransmitSetting.field.BW) ? "20/40" : "20";
	DBGPRINT(RT_DEBUG_OFF, ("\tHT-BW: %s\n", pstr));
#ifdef DOT11_VHT_AC
	if (WMODE_CAP_AC(pAd->CommonCfg.PhyMode))
	{
		if (pAd->CommonCfg.vht_bw)
			pstr = "80";
		DBGPRINT(RT_DEBUG_OFF, ("\tVHT-BW: %s\n", pstr));
	}
#endif /* DOT11_VHT_AC */

#if defined(RTMP_MAC) || defined(RLT_MAC)
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type != HIF_MT) {
		RTMP_IO_READ32(pAd, BKOFF_SLOT_CFG, &mac_val);
		DBGPRINT(RT_DEBUG_OFF
			, ("BackOff Slot      : %s slot time, BKOFF_SLOT_CFG(0x1104) = 0x%08x\n"
			, OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED) ? "short" : "long"
			, mac_val));
	}
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

#ifdef RT65xx
	if (IS_RT65XX(pAd))
	{
		dump_bw_info(pAd);
#ifdef RT8592
		if (IS_RT8592(pAd))
			dump_pwr_info(pAd);
#endif /* RT8592 */
	}
#endif /* RT65xx */

	DBGPRINT(RT_DEBUG_OFF, ("Security\n"));

	return TRUE;
}


INT show_wdev_info(RTMP_ADAPTER *pAd, struct wifi_dev *wdev)
{
	INT idx;
	
	for (idx = 0 ; idx < WDEV_NUM_MAX; idx++)
	{
		if (pAd->wdev_list[idx] == wdev) {
			break;
		}
	}

	if (idx >= WDEV_NUM_MAX)
	{
		DBGPRINT(RT_DEBUG_OFF, ("ERR! Cannot found required wdev(%p)!\n", wdev));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_OFF, ("WDEV Instance(%d) Info:\n", idx));
	
	return TRUE;
}


CHAR *wdev_type_str[]={"AP", "STA", "ADHOC", "WDS", "MESH", "Unknown"};

RTMP_STRING *wdev_type2str(int type)
{
	switch (type)
	{
		case WDEV_TYPE_AP:
			return wdev_type_str[0];
		case WDEV_TYPE_STA:
			return wdev_type_str[1];
		case WDEV_TYPE_ADHOC:
			return wdev_type_str[2];
		case WDEV_TYPE_WDS:
			return wdev_type_str[3];
		case WDEV_TYPE_MESH:
			return wdev_type_str[4];
		default:
			return wdev_type_str[5];
	}
}


INT show_sysinfo_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT idx;
	UINT32 total_size = 0, cntr_size;
	struct wifi_dev *wdev;

	
	DBGPRINT(RT_DEBUG_OFF, ("Device Instance\n"));
	for (idx = 0; idx < WDEV_NUM_MAX; idx++) {
		DBGPRINT(RT_DEBUG_OFF, ("\tWDEV %02d:", idx));
		if (pAd->wdev_list[idx])
		{
			UCHAR *str = NULL;

			wdev = pAd->wdev_list[idx];
			DBGPRINT(RT_DEBUG_OFF, ("\n\t\tName/Type:%s/%s\n",
						RTMP_OS_NETDEV_GET_DEVNAME(wdev->if_dev),
						wdev_type2str(wdev->wdev_type)));
			DBGPRINT(RT_DEBUG_OFF, ("\t\tWdev(list) Idx:%d\n", wdev->wdev_idx));
			DBGPRINT(RT_DEBUG_OFF, ("\t\tMacAddr:%02x:%02x:%02x:%02x:%02x:%02x\n",
						PRINT_MAC(wdev->if_addr)));
			DBGPRINT(RT_DEBUG_OFF, ("\t\tBSSID:%02x:%02x:%02x:%02x:%02x:%02x\n",
						PRINT_MAC(wdev->bssid)));
			str = wmode_2_str(wdev->PhyMode);
			if (str) {
				DBGPRINT(RT_DEBUG_OFF, ("\t\tPhyMode:%s\n", str));
				os_free_mem(pAd, str);
			}
			DBGPRINT(RT_DEBUG_OFF, ("\t\tChannel:%d\n", wdev->channel));
			DBGPRINT(RT_DEBUG_OFF, ("\t\tPortSecured/AllowTx: %d(%sSecured):%d\n",
						wdev->PortSecured,
						(wdev->PortSecured == WPA_802_1X_PORT_SECURED ? "" : "Not"),
						wdev->allow_data_tx));
			DBGPRINT(RT_DEBUG_OFF, ("\t\ttx_pkt_checker:%p\n", wdev->tx_pkt_allowed));
			DBGPRINT(RT_DEBUG_OFF, ("\t\ttx_pkt_handlerer:%p\n", wdev->tx_pkt_handle));
			DBGPRINT(RT_DEBUG_OFF, ("\t\ttx_pkt_hardTransmit:%p\n", wdev->wdev_hard_tx));

			DBGPRINT(RT_DEBUG_OFF, ("\t\tif_dev:0x%p\tfunc_dev:[%d]0x%p\tsys_handle:0x%p\n",
						wdev->if_dev, wdev->func_idx, wdev->func_dev, wdev->sys_handle));
		}
		else
		{
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}
		
	}

	DBGPRINT(RT_DEBUG_OFF, ("Memory Statistics:\n"));

	DBGPRINT(RT_DEBUG_OFF, ("\tsize>\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\t\tpAd = \t\t%zu bytes\n\n", sizeof(*pAd)));

	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tCommonCfg = \t%zu bytes\n", sizeof(pAd->CommonCfg)));
	total_size += sizeof(pAd->CommonCfg);
#ifdef CONFIG_AP_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tApCfg = \t%zu bytes\n", sizeof(pAd->ApCfg)));
	total_size += sizeof(pAd->ApCfg);
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\t\tMBSSID = \t%zu B (PerMBSS =%zu B, Total MBSS Num= %d)\n",
				sizeof(pAd->ApCfg.MBSSID), sizeof(struct _BSS_STRUCT), HW_BEACON_MAX_NUM));
#ifdef APCLI_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\t\t\tAPCLI = \t%zu bytes (PerAPCLI =%zu bytes, Total APCLI Num= %d)\n",
				sizeof(pAd->ApCfg.ApCliTab), sizeof(struct _APCLI_STRUCT), MAX_APCLI_NUM));
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef RTMP_MAC_PCI
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tTxRing = \t%zu bytes\n", sizeof(pAd->TxRing)));
	total_size += sizeof(pAd->TxRing);
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tRxRing = \t%zu bytes\n", sizeof(pAd->RxRing)));
	total_size += sizeof(pAd->RxRing);
#ifdef CONFIG_ANDES_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tCtrlRing = \t%zu bytes\n", sizeof(pAd->CtrlRing)));
	total_size += sizeof(pAd->CtrlRing);
#endif /* CONFIG_ANDES_SUPPORT */

#ifdef MT_MAC
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tBcnRing = \t%zu bytes\n", sizeof(pAd->BcnRing)));
	total_size += sizeof(pAd->BcnRing);
#endif /* MT_MAC */
#endif /* RTMP_MAC_PCI */

	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tMgmtRing = \t%zu bytes\n", sizeof(pAd->MgmtRing)));
	total_size += sizeof(pAd->MgmtRing);
		
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tMlme = \t%zu bytes\n", sizeof(pAd->Mlme)));
	total_size += sizeof(pAd->Mlme);
#ifdef CONFIG_STA_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tMlmeAux = \t%zu bytes\n", sizeof(pAd->MlmeAux)));
	total_size += sizeof(pAd->MlmeAux);
#endif /* CONFIG_STA_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tMacTab = \t%zu bytes\n", sizeof(pAd->MacTab)));
	total_size += sizeof(pAd->MacTab);

#ifdef DOT11_N_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tBATable = \t%zu bytes\n", sizeof(pAd->BATable)));
	total_size += sizeof(pAd->BATable);
#endif /* DOT11_N_SUPPORT */

	cntr_size = sizeof(pAd->Counters8023) + sizeof(pAd->WlanCounters) +
			sizeof(pAd->RalinkCounters) + /* sizeof(pAd->DrsCounters) */+
			sizeof(pAd->PrivateInfo);
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tCounter** = \t%d bytes\n", cntr_size));
	total_size += cntr_size;

#if defined (AP_SCAN_SUPPORT) || defined (CONFIG_STA_SUPPORT)
	DBGPRINT(RT_DEBUG_OFF, ("\t\t\tScanTab = \t%zu bytes\n", sizeof(pAd->ScanTab)));
	total_size += sizeof(pAd->ScanTab);
#endif
	DBGPRINT(RT_DEBUG_OFF, ("\tsize> Total = \t\t%d bytes, Others = %zu bytes\n\n",
			total_size, sizeof(*pAd)-total_size));

	return TRUE;
}


INT show_trinfo_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
#if defined(RTMP_PCI_SUPPORT) || defined(RTMP_RBUS_SUPPORT)
	if (IS_RBUS_INF(pAd) || IS_PCI_INF(pAd))
	{
		ULONG irq_flags = 0;
		UINT32 tbase[NUM_OF_TX_RING], tcnt[NUM_OF_TX_RING];
		UINT32 tcidx[NUM_OF_TX_RING], tdidx[NUM_OF_TX_RING];
		UINT32 rbase[NUM_OF_RX_RING], rcnt[NUM_OF_RX_RING];
		UINT32 rcidx[NUM_OF_RX_RING], rdidx[NUM_OF_RX_RING];
		UINT32 mbase[4] = {0}, mcnt[4] = {0}, mcidx[4] = {0}, mdidx[4] = {0};
		UINT32 sys_ctrl[4];
		UINT32 cr_int_src = 0, cr_int_mask = 0, cr_delay_int = 0, cr_wpdma_glo_cfg = 0;
#ifdef RLT_MAC
		UINT32 pbf_val, pcnt[5];
#endif /* RLT_MAC */
		INT idx;
		
		RTMP_IRQ_LOCK(&pAd->irq_lock, irq_flags);
		for (idx = 0; idx < NUM_OF_TX_RING; idx++)
		{
			RTMP_IO_READ32(pAd, pAd->TxRing[idx].hw_desc_base, &tbase[idx]);
			RTMP_IO_READ32(pAd, pAd->TxRing[idx].hw_cnt_addr, &tcnt[idx]);
			RTMP_IO_READ32(pAd, pAd->TxRing[idx].hw_cidx_addr, &tcidx[idx]);
			RTMP_IO_READ32(pAd, pAd->TxRing[idx].hw_didx_addr, &tdidx[idx]);
		}

		for (idx = 0; idx < NUM_OF_RX_RING; idx++)
		{
			RTMP_IO_READ32(pAd, pAd->RxRing[idx].hw_desc_base, &rbase[idx]);
			RTMP_IO_READ32(pAd, pAd->RxRing[idx].hw_cnt_addr, &rcnt[idx]);
			RTMP_IO_READ32(pAd, pAd->RxRing[idx].hw_cidx_addr, &rcidx[idx]);
			RTMP_IO_READ32(pAd, pAd->RxRing[idx].hw_didx_addr, &rdidx[idx]);
		}

		RTMP_IO_READ32(pAd, pAd->MgmtRing.hw_desc_base, &mbase[0]);
		RTMP_IO_READ32(pAd, pAd->MgmtRing.hw_cnt_addr, &mcnt[0]);
		RTMP_IO_READ32(pAd, pAd->MgmtRing.hw_cidx_addr, &mcidx[0]);
		RTMP_IO_READ32(pAd, pAd->MgmtRing.hw_didx_addr, &mdidx[0]);

#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
#ifdef CONFIG_ANDES_SUPPORT
			RTMP_IO_READ32(pAd, pAd->CtrlRing.hw_desc_base, &mbase[1]);
			RTMP_IO_READ32(pAd, pAd->CtrlRing.hw_cnt_addr, &mcnt[1]);
			RTMP_IO_READ32(pAd, pAd->CtrlRing.hw_cidx_addr, &mcidx[1]);
			RTMP_IO_READ32(pAd, pAd->CtrlRing.hw_didx_addr, &mdidx[1]);
#endif /* CONFIG_ANDES_SUPPORT */

			RTMP_IO_READ32(pAd, RLT_PBF_CFG, &pbf_val);
			RTMP_IO_READ32(pAd, TX_MAX_PCNT, &pcnt[0]);
			RTMP_IO_READ32(pAd, RX_MAX_PCNT, &pcnt[1]);
			RTMP_IO_READ32(pAd, TXQ_STA, &pcnt[2]);
			RTMP_IO_READ32(pAd, RXQ_STA, &pcnt[3]);
			RTMP_IO_READ32(pAd, TXRXQ_PCNT, &pcnt[4]);
			RTMP_IO_READ32(pAd, PBF_DBG, &pcnt[5]);
		}
#endif /* RLT_MAC */

#ifdef MT_MAC
		if (pAd->chipCap.hif_type == HIF_MT) {
#ifdef CONFIG_ANDES_SUPPORT
			RTMP_IO_READ32(pAd, pAd->CtrlRing.hw_desc_base, &mbase[1]);
			RTMP_IO_READ32(pAd, pAd->CtrlRing.hw_cnt_addr, &mcnt[1]);
			RTMP_IO_READ32(pAd, pAd->CtrlRing.hw_cidx_addr, &mcidx[1]);
			RTMP_IO_READ32(pAd, pAd->CtrlRing.hw_didx_addr, &mdidx[1]);
#endif /* CONFIG_ANDES_SUPPORT */

			RTMP_IO_READ32(pAd, pAd->BcnRing.hw_desc_base, &mbase[2]);
			RTMP_IO_READ32(pAd, pAd->BcnRing.hw_cnt_addr, &mcnt[2]);
			RTMP_IO_READ32(pAd, pAd->BcnRing.hw_cidx_addr, &mcidx[2]);
			RTMP_IO_READ32(pAd, pAd->BcnRing.hw_didx_addr, &mdidx[2]);
			
			RTMP_IO_READ32(pAd, pAd->TxBmcRing.hw_desc_base, &mbase[3]);
			RTMP_IO_READ32(pAd, pAd->TxBmcRing.hw_cnt_addr, &mcnt[3]);
			RTMP_IO_READ32(pAd, pAd->TxBmcRing.hw_cidx_addr, &mcidx[3]);
			RTMP_IO_READ32(pAd, pAd->TxBmcRing.hw_didx_addr, &mdidx[3]);
		}
#endif /* MT_MAC */

#ifdef MT_MAC
	// TODO: shiang-7603
		if (pAd->chipCap.hif_type == HIF_MT) {
			cr_int_src = MT_INT_SOURCE_CSR;
			cr_int_mask = MT_INT_MASK_CSR;
			cr_delay_int = MT_DELAY_INT_CFG;
			cr_wpdma_glo_cfg = MT_WPDMA_GLO_CFG;
		}
#endif /* MT_MAC */

#if defined(RTMP_MAC) || defined(RLT_MAC)
		if (pAd->chipCap.hif_type == HIF_RLT || pAd->chipCap.hif_type == HIF_RTMP) {
			cr_int_src = INT_SOURCE_CSR;
			cr_int_mask = INT_MASK_CSR;
			cr_delay_int = DELAY_INT_CFG;
			cr_wpdma_glo_cfg = WPDMA_GLO_CFG;
		}
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */
		RTMP_IO_READ32(pAd, cr_int_src, &sys_ctrl[0]);
		RTMP_IO_READ32(pAd, cr_int_mask, &sys_ctrl[1]);
		RTMP_IO_READ32(pAd, cr_delay_int, &sys_ctrl[2]);
		RTMP_IO_READ32(pAd, cr_wpdma_glo_cfg, &sys_ctrl[3]);
		
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, irq_flags);
		
		DBGPRINT(RT_DEBUG_OFF, ("TxRing Configuration\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\tRingIdx Reg \tBase \t\tCnt \tCIDX \tDIDX \tSWIDX\n"));
		for (idx = 0; idx < NUM_OF_TX_RING; idx++) {
			DBGPRINT(RT_DEBUG_OFF, ("\t%d \t0x%04x \t0x%08x \t0x%x \t0x%x \t0x%x \t0x%x\n",
						idx, pAd->TxRing[idx].hw_desc_base, tbase[idx], tcnt[idx], tcidx[idx], tdidx[idx],pAd->TxRing[idx].TxSwFreeIdx));
		}

		DBGPRINT(RT_DEBUG_OFF, ("\nRxRing Configuration\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\tRingIdx Reg \tBase \t\tCnt \tCIDX \tDIDX\tSWIDX\n"));
		for (idx = 0; idx < NUM_OF_RX_RING; idx++) {
			DBGPRINT(RT_DEBUG_OFF, ("\t%d \t0x%04x \t0x%08x \t0x%x \t0x%x \t0x%x \t0x%x\n",
						idx, pAd->RxRing[idx].hw_desc_base, rbase[idx], rcnt[idx], rcidx[idx], rdidx[idx],pAd->RxRing[idx].RxSwReadIdx));
		}

		DBGPRINT(RT_DEBUG_OFF, ("\nMgmtRing Configuration\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\tRingIdx Reg \tBase \t\tCnt \tCIDX \tDIDX\tSWIDX\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\t%d \t0x%04x \t0x%08x \t0x%x \t0x%x \t0x%x \t0x%x\n",
									0, pAd->MgmtRing.hw_desc_base, mbase[0], mcnt[0], mcidx[0], mdidx[0],pAd->MgmtRing.TxSwFreeIdx));

#ifdef CONFIG_ANDES_SUPPORT
		DBGPRINT(RT_DEBUG_OFF, ("\nCtrlRing Configuration\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\tRingIdx Reg \tBase \t\tCnt \tCIDX \tDIDX\tSWIDX\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\t%d \t0x%04x \t0x%08x \t0x%x \t0x%x \t0x%x \t0x%x\n",
									0, pAd->CtrlRing.hw_desc_base, mbase[1], mcnt[1], mcidx[1], mdidx[1],pAd->CtrlRing.TxSwFreeIdx));
#endif /* CONFIG_ANDES_SUPPORT */

#ifdef MT_MAC
		if (pAd->chipCap.hif_type == HIF_MT) {
			DBGPRINT(RT_DEBUG_OFF, ("\nBcnRing Configuration\n"));
			DBGPRINT(RT_DEBUG_OFF, ("\tRingIdx Reg \tBase \t\tCnt \tCIDX \tDIDX \tSWIDX\n"));
			DBGPRINT(RT_DEBUG_OFF, ("\t%d \t0x%04x \t0x%08x \t0x%x \t0x%x \t0x%x \t0x%x\n",
									0, pAd->BcnRing.hw_desc_base, mbase[2], mcnt[2], mcidx[2], mdidx[2],pAd->BcnRing.TxSwFreeIdx));
		}

		DBGPRINT(RT_DEBUG_OFF, ("\nBMCRing Configuration\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\tRingIdx Reg \tBase \t\tCnt \tCIDX \tDIDX \tSWIDX\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\t%d \t0x%04x \t0x%08x \t0x%x \t0x%x \t0x%x \t0x%x\n",
								0, pAd->TxBmcRing.hw_desc_base, mbase[3], mcnt[3], mcidx[3], mdidx[3],pAd->TxBmcRing.TxSwFreeIdx));
#endif /* MT_MAC */

#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
			DBGPRINT(RT_DEBUG_OFF, ("\nPBF Configuration\n"
									"\tRLT_PBF_CFG: 0x%08x\n", pbf_val));
			DBGPRINT(RT_DEBUG_OFF, ("\tPCNT_CFG:\n"
									"\t\tTxMax[0x%04x] -0x%08x\n", TX_MAX_PCNT, pcnt[0]));
			DBGPRINT(RT_DEBUG_OFF, ("\t\tTxStat[0x%04x] -0x%08x\n", RX_MAX_PCNT, pcnt[2]));
			DBGPRINT(RT_DEBUG_OFF, ("\t\tTxUsed[0x%04x] -0x%08x\n", TXRXQ_PCNT, pcnt[4]));
			DBGPRINT(RT_DEBUG_OFF, ("\t\tRxMax[0x%04x] -0x%08x\n", RX_MAX_PCNT, pcnt[1]));
			DBGPRINT(RT_DEBUG_OFF, ("\t\tRxStat[0x%04x] -0x%08x\n", RXQ_STA, pcnt[3]));
			DBGPRINT(RT_DEBUG_OFF, ("\t\tFreCNT[0x%04x] -0x%08x\n", PBF_DBG, pcnt[5]));
		}
#endif /* RLT_MAC */

		DBGPRINT(RT_DEBUG_OFF, ("Interrupt Configuration\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\tIntCSR \tIntMask \tDelayINT\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\t0x%x \t0x%x \t0x%x\n", sys_ctrl[0], sys_ctrl[1], sys_ctrl[2]));
		DBGPRINT(RT_DEBUG_OFF, ("DMA Configuration(0x%x)\n", sys_ctrl[3]));
		DBGPRINT(RT_DEBUG_OFF, ("\tTx/RxDMAEn=%d/%d, \tTx/RxDMABusy=%d/%d\n", 
									sys_ctrl[3] & 0x1, sys_ctrl[3] & 0x4, 
									sys_ctrl[3] & 0x2, sys_ctrl[3] & 0x8));
		
		DBGPRINT(RT_DEBUG_OFF, ("PDMA Info\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\tPDMAMonitorEn=%d, TxRCounter = %lu, TxDMACheckTimes = %d,  RxRCounter = %lu, RxDMACheckTimes = %d, PDMARFailCount = %lu\n", pAd->PDMAWatchDogEn, pAd->TxDMAResetCount, pAd->TxDMACheckTimes, pAd->RxDMAResetCount, pAd->RxDMACheckTimes, pAd->PDMAResetFailCount));
		DBGPRINT(RT_DEBUG_OFF, ("PSE Info\n"));
		DBGPRINT(RT_DEBUG_OFF, ("\tPSEMonitorEn=%d, RCounter = %lu, RxPseCheckTimes = %d, PSETriggerType1Count = %lu, PSETriggerType2Count = %lu, PSERFailCount = %lu\n", pAd->PSEWatchDogEn, pAd->PSEResetCount, pAd->RxPseCheckTimes, pAd->PSETriggerType1Count, pAd->PSETriggerType2Count, pAd->PSEResetFailCount));
		DBGPRINT(RT_DEBUG_OFF, ("\n"));
		DBGPRINT(RT_DEBUG_OFF, ("SkipTxRcount=%lu\n", pAd->SkipTxRCount));		
	}
#endif /* defined(RTMP_PCI_SUPPORT) || defined(RTMP_RBUS_SUPPORT) */

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd))
	{
		DBGPRINT(RT_DEBUG_OFF, ("TxRing Configuration\n"));

		DBGPRINT(RT_DEBUG_OFF, ("\nRxRing Configuration\n"));

		DBGPRINT(RT_DEBUG_OFF, ("\nPBF Configuration\n"));
	}
#endif /* RTMP_USB_SUPPORT */

	return TRUE;
}


INT show_txqinfo_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT qidx;
	ULONG irqflag = 0;
	UCHAR *TxSwQ = NULL;
	struct tx_swq_fifo *fifo_swq;

	os_alloc_mem(NULL, &TxSwQ, 512 * 4);
	if (TxSwQ == NULL)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s():mem alloc failed!\n", __FUNCTION__));
		return FALSE;
	}
	
	DBGPRINT(RT_DEBUG_OFF, ("%s():Dump TxQ Info\n", __FUNCTION__));
	for (qidx = 0; qidx < 4; qidx++)
	{
		fifo_swq = &pAd->tx_swq[qidx];

		RTMP_IRQ_LOCK(&pAd->irq_lock, irqflag);
		NdisMoveMemory(&TxSwQ[qidx * 512], fifo_swq->swq, 512);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, irqflag);
	}
	
	os_free_mem(NULL, TxSwQ);

	rtmp_tx_swq_dump(pAd, 0);
	rtmp_sta_txq_dump(pAd, &pAd->MacTab.tr_entry[1], 0);
	
	return TRUE;
}


#ifdef WSC_STA_SUPPORT
INT	Show_WpsManufacturer_Proc(
	IN	PRTMP_ADAPTER	pAd,
	OUT	RTMP_STRING *pBuf,
IN ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\tManufacturer = %s"
			, pAd->StaCfg.WscControl.RegData.SelfInfo.Manufacturer);
	return 0;
}

INT	Show_WpsModelName_Proc(
	IN	PRTMP_ADAPTER	pAd,
	OUT	RTMP_STRING *pBuf,
	IN ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\tModelName = %s"
			, pAd->StaCfg.WscControl.RegData.SelfInfo.ModelName);
	return 0;
}

INT	Show_WpsDeviceName_Proc(
	IN	PRTMP_ADAPTER	pAd,
	OUT	RTMP_STRING *pBuf,
	IN ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\tDeviceName = %s"
			, pAd->StaCfg.WscControl.RegData.SelfInfo.DeviceName);
	return 0;
}

INT	Show_WpsModelNumber_Proc(
	IN	PRTMP_ADAPTER	pAd,
	OUT	RTMP_STRING *pBuf,
	IN ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\tModelNumber = %s"
			, pAd->StaCfg.WscControl.RegData.SelfInfo.ModelNumber);
	return 0;
}

INT	Show_WpsSerialNumber_Proc(
	IN	PRTMP_ADAPTER	pAd,
	OUT	RTMP_STRING *pBuf,
	IN ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\tSerialNumber = %s"
			, pAd->StaCfg.WscControl.RegData.SelfInfo.SerialNumber);
	return 0;
}
#endif /* WSC_STA_SUPPORT */


#ifdef SINGLE_SKU
INT	Show_ModuleTxpower_Proc(
	IN	PRTMP_ADAPTER	pAd,
	OUT	RTMP_STRING *pBuf,
	IN	ULONG			BufLen)
{
	snprintf(pBuf, BufLen, "\tModuleTxpower = %d", pAd->CommonCfg.ModuleTxpower);
	return 0;
}
#endif /* SINGLE_SKU */

#ifdef APCLI_SUPPORT
 INT RTMPIoctlConnStatus(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
 
 	INT i=0;
 	POS_COOKIE pObj;
 	UCHAR ifIndex;
	BOOLEAN bConnect=FALSE;
 	
 	pObj = (POS_COOKIE) pAd->OS_Cookie;
 
 	DBGPRINT(RT_DEBUG_TRACE, ("==>RTMPIoctlConnStatus\n"));
 
 	if (pObj->ioctl_if_type != INT_APCLI)
 		return FALSE;
 
	ifIndex = (UCHAR)pObj->ioctl_if;
 	
 	DBGPRINT(RT_DEBUG_OFF, ("=============================================================\n"));
 	if((pAd->ApCfg.ApCliTab[ifIndex].CtrlCurrState == APCLI_CTRL_CONNECTED)
 		&& (pAd->ApCfg.ApCliTab[ifIndex].SsidLen != 0))
 	{
 		for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
 		{
 			PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
 			STA_TR_ENTRY *tr_entry = &pAd->MacTab.tr_entry[i];
 
 			if ( IS_ENTRY_APCLI(pEntry)
				&& (pEntry->Sst == SST_ASSOC)
				&& (tr_entry->PortSecured == WPA_802_1X_PORT_SECURED))
 				{
					DBGPRINT(RT_DEBUG_OFF, ("ApCli%d Connected AP : %02X:%02X:%02X:%02X:%02X:%02X   SSID:%s\n",
							ifIndex, PRINT_MAC(pEntry->Addr), pAd->ApCfg.ApCliTab[ifIndex].Ssid));
					bConnect=TRUE;
 				}
 		}

		if (!bConnect)
			DBGPRINT(RT_DEBUG_OFF, ("ApCli%d Connected AP : Disconnect\n",ifIndex));
 	}
 	else
 	{
 		DBGPRINT(RT_DEBUG_OFF, ("ApCli%d Connected AP : Disconnect\n",ifIndex));
 	}
 	DBGPRINT(RT_DEBUG_OFF, ("=============================================================\n"));
     	DBGPRINT(RT_DEBUG_TRACE, ("<==RTMPIoctlConnStatus\n"));
 	return TRUE;
}
#endif/*APCLI_SUPPORT*/

void  getRate(HTTRANSMIT_SETTING HTSetting, ULONG* fLastTxRxRate)

{
	 INT MCSMappingRateTable[] =
        {2,  4,   11,  22, /* CCK*/
        12, 18,   24,  36, 48, 72, 96, 108, /* OFDM*/
        13, 26,   39,  52,  78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260, /* 20MHz, 800ns GI, MCS: 0 ~ 15*/
        39, 78,  117, 156, 234, 312, 351, 390,  /* 20MHz, 800ns GI, MCS: 16 ~ 23*/
        27, 54,   81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540, /* 40MHz, 800ns GI, MCS: 0 ~ 15*/
        81, 162, 243, 324, 486, 648, 729, 810,  /* 40MHz, 800ns GI, MCS: 16 ~ 23*/
        14, 29,   43,  57,  87, 115, 130, 144, 29, 59, 87, 115, 173, 230, 260, 288, /* 20MHz, 400ns GI, MCS: 0 ~ 15*/
        43, 87,  130, 173, 260, 317, 390, 433,  /* 20MHz, 400ns GI, MCS: 16 ~ 23*/
        30, 60,   90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600, /* 40MHz, 400ns GI, MCS: 0 ~ 15*/
        90, 180, 270, 360, 540, 720, 810, 900,
        13, 26,   39,  52,  78, 104, 117, 130, 156, /* 11ac: 20Mhz, 800ns GI, MCS: 0~8 */
        27, 54,   81, 108, 162, 216, 243, 270, 324, 360, /*11ac: 40Mhz, 800ns GI, MCS: 0~9 */
        59, 117, 176, 234, 351, 468, 527, 585, 702, 780, /*11ac: 80Mhz, 800ns GI, MCS: 0~9 */
        14, 29,   43,  57,  87, 115, 130, 144, 173, /* 11ac: 20Mhz, 400ns GI, MCS: 0~8 */
        30, 60,   90, 120, 180, 240, 270, 300, 360, 400, /*11ac: 40Mhz, 400ns GI, MCS: 0~9 */
        65, 130, 195, 260, 390, 520, 585, 650, 780, 867 /*11ac: 80Mhz, 400ns GI, MCS: 0~9 */
        };

	int rate_count = sizeof(MCSMappingRateTable)/sizeof(int);
	int rate_index = 0;  
	int value = 0;

#ifdef DOT11_VHT_AC
    if (HTSetting.field.MODE >= MODE_VHT)
    {
       if (HTSetting.field.BW == BW_20) {
               rate_index = 108 +
                       ((UCHAR)HTSetting.field.ShortGI * 29) +
                       ((UCHAR)HTSetting.field.MCS);
       }
       else if (HTSetting.field.BW == BW_40) {
               rate_index = 117 +
                       ((UCHAR)HTSetting.field.ShortGI * 29) +
                       ((UCHAR)HTSetting.field.MCS);
       }
       else if (HTSetting.field.BW == BW_80) {
               rate_index = 127 +
                       ((UCHAR)HTSetting.field.ShortGI * 29) +
                       ((UCHAR)HTSetting.field.MCS);
       }
    }
    else
#endif /* DOT11_VHT_AC */
#ifdef DOT11_N_SUPPORT
    if (HTSetting.field.MODE >= MODE_HTMIX)
    {
/*    	rate_index = 12 + ((UCHAR)ht_setting.field.BW *16) + ((UCHAR)ht_setting.field.ShortGI *32) + ((UCHAR)ht_setting.field.MCS);*/
    	rate_index = 12 + ((UCHAR)HTSetting.field.BW *24) + ((UCHAR)HTSetting.field.ShortGI *48) + ((UCHAR)HTSetting.field.MCS);
    }
    else 
#endif /* DOT11_N_SUPPORT */
    if (HTSetting.field.MODE == MODE_OFDM)                
    	rate_index = (UCHAR)(HTSetting.field.MCS) + 4;
    else if (HTSetting.field.MODE == MODE_CCK)   
    	rate_index = (UCHAR)(HTSetting.field.MCS);

    if (rate_index < 0)
        rate_index = 0;
    
    if (rate_index >= rate_count)
        rate_index = rate_count-1;

    value = (MCSMappingRateTable[rate_index] * 5)/10;
	*fLastTxRxRate=(ULONG)value;
	return;
}


#ifdef TXBF_SUPPORT
#if defined(RT2883) || defined(RT3883) || defined(RT3593)
/*
	Set_ReadITxBf_Proc - Read Implicit BF profile and display it
		iwpriv ra0 set ReadITxBf=<profile number>
*/
INT	Set_ReadITxBf_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int profileNum = simple_strtol(arg, 0, 10);
	int scIndex, i, maxCarriers;

	Read_TxBfProfile(pAd, &profData, profileNum, TRUE);

	/* Display profile. Note: each column is displayed as a row. This shortens the display */
	DBGPRINT(RT_DEBUG_OFF, ("---ITxBF Profile: %d - %dx%d, %dMHz\n",
		profileNum, profData.rows, profData.columns, profData.fortyMHz? 40: 20));

	maxCarriers = profData.fortyMHz? PROFILE_MAX_CARRIERS_40: PROFILE_MAX_CARRIERS_20;

	for (scIndex=0; scIndex<maxCarriers; scIndex++) {
		for (i=0; i<profData.rows; i++) {
			DBGPRINT(RT_DEBUG_OFF, ("%d %d    ", Unpack_IBFValue(profData.data[scIndex], 2*i+1), Unpack_IBFValue(profData.data[scIndex], 2*i)));
		}
		DBGPRINT(RT_DEBUG_OFF, ("\n"));

		if (profData.columns>1) {
			for (i=0; i<profData.rows; i++) {
				DBGPRINT(RT_DEBUG_OFF, ("%d %d    ", Unpack_IBFValue(profData.data[scIndex], 2*i+7), Unpack_IBFValue(profData.data[scIndex], 2*i+6)));
			}
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}
	}

	return TRUE;
}


/*
	Set_ReadETxBf_Proc - Read Explicit BF profile and display it
		usage: iwpriv ra0 set ReadETxBf=<profile number>
*/
INT	Set_ReadETxBf_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int profileNum = simple_strtol(arg, 0, 10);
	int scIndex, i, maxCarriers;

	Read_TxBfProfile(pAd, &profData, profileNum, FALSE);

	/* Dump ETxBF profile values. Note: each column is displayed as a row. This shortens the display */
	DBGPRINT(RT_DEBUG_OFF, ("---ETxBF Profile: %d - %dx%d, %dMHz, grp=%d\n",
		profileNum, profData.rows, profData.columns, profData.fortyMHz? 40: 20, profData.grouping));

	maxCarriers = profData.fortyMHz? PROFILE_MAX_CARRIERS_40: PROFILE_MAX_CARRIERS_20;

	for (scIndex=0; scIndex<maxCarriers; scIndex++) {
		for (i=0; i<profData.rows; i++) {
			DBGPRINT(RT_DEBUG_OFF, ("%d %d\t", (CHAR)(profData.data[scIndex][6*i]), (CHAR)(profData.data[scIndex][6*i+1]) ));
		}
		DBGPRINT(RT_DEBUG_OFF, ("\n"));

		if (profData.columns>1) {
			for (i=0; i<profData.rows; i++) {
				DBGPRINT(RT_DEBUG_OFF, ("%d %d    ", (CHAR)(profData.data[scIndex][6*i+2]), (CHAR)(profData.data[scIndex][6*i+3]) ));
			}
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}

		if (profData.columns>2) {
			for (i=0; i<profData.rows; i++) {
				DBGPRINT(RT_DEBUG_OFF, ("%d %d    ", (CHAR)(profData.data[scIndex][6*i+4]), (CHAR)(profData.data[scIndex][6*i+5]) ));
			}
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}
	}

	return TRUE;
}


/*
	Set_WriteITxBf_Proc - Write Implicit BF matrix
		usage: iwpriv ra0 set WriteITxBf=<profile number>
		Assumes profData contains a valid Implicit Profile
*/
INT	Set_WriteITxBf_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int profileNum = simple_strtol(arg, 0, 10);

	if (!profData.impProfile)
		return FALSE;

	Write_TxBfProfile(pAd, &profData, profileNum);

	return TRUE;
}


/*
	Set_WriteETxBf_Proc - Write Explicit BF matrix
		usage: iwpriv ra0 set WriteETxBf=<profile number>
		Assumes profData contains a valid Explicit Profile
*/
INT	Set_WriteETxBf_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int profileNum = simple_strtol(arg, 0, 10);

	if (profData.impProfile)
		return FALSE;

	Write_TxBfProfile(pAd, &profData, profileNum);

	return TRUE;
}


/*
	Set_StatITxBf_Proc - Compute power of each chain in Implicit BF matrix
		usage: iwpriv ra0 set StatITxBf=<profile number>
*/
INT	Set_StatITxBf_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int scIndex, maxCarriers, i;
	unsigned long col1Power[3] = {0,0,0}, col2Power[3] = {0,0,0};
	int profileNum = simple_strtol(arg, 0, 10);
	PROFILE_DATA *pProfData;

	os_alloc_mem(NULL, (UCHAR **)&pProfData, sizeof(PROFILE_DATA));
	if (pProfData == NULL)
	{
		DBGPRINT(RT_DEBUG_OFF, ("Set_StatITxBf_Proc: mem alloc failed\n"));
		return FALSE;
	}

	Read_TxBfProfile(pAd, pProfData, profileNum, TRUE);

	maxCarriers = pProfData->fortyMHz? PROFILE_MAX_CARRIERS_40: PROFILE_MAX_CARRIERS_20;

	for (scIndex=0; scIndex<maxCarriers; scIndex++) {
		for (i=0; i<pProfData->rows; i++) {
			int ival = Unpack_IBFValue(pProfData->data[scIndex], 2*i+1);
			int qval = Unpack_IBFValue(pProfData->data[scIndex], 2*i);
			col1Power[i] += ival*ival+qval*qval;

			if (pProfData->columns==2) {
				ival = Unpack_IBFValue(pProfData->data[scIndex], 2*i+7);
				qval = Unpack_IBFValue(pProfData->data[scIndex], 2*i+6);
				col2Power[i] += ival*ival+qval*qval;
			}
		}
	}

	/* Remove implied scale factor of 2^-16. Convert to thousandths */
	for (i=0; i<pProfData->rows; i++) {
		col1Power[i] >>= 12;
		col1Power[i] = ((col1Power[i]*1000)/maxCarriers)>>4;
		col2Power[i] >>= 12;
		col2Power[i] = ((col2Power[i]*1000)/maxCarriers)>>4;
	}

	/* Display stats */
	DBGPRINT(RT_DEBUG_OFF, ("ITxBF Stats:\n  %dx1=[0.%03lu 0.%03lu, 0.%03lu]\n",
				pProfData->rows, col1Power[0], col1Power[1], col1Power[2]));
	if (pProfData->columns==2) {
		DBGPRINT(RT_DEBUG_OFF, ("  %dx2=[0.%03lu 0.%03lu, 0.%03lu]\n",
					pProfData->rows, (col1Power[0]+col2Power[0])/2, (col1Power[1]+col2Power[1])/2,
					(col1Power[2]+col2Power[2])/2) );
	}

	os_free_mem(NULL, pProfData);

	return TRUE;
}

/*
	Set_StatETxBf_Proc - Compute power of each chain in Explicit BF matrix
		usage: iwpriv ra0 set StatETxBf=<profile number>
*/
INT	Set_StatETxBf_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int scIndex, maxCarriers, i;
	unsigned long col1Power[3] = {0,0,0}, col2Power[3] = {0,0,0}, col3Power[3] = {0,0,0};
	int profileNum = simple_strtol(arg, 0, 10);
	PROFILE_DATA *pProfData;

	os_alloc_mem(NULL, (UCHAR **)&pProfData, sizeof(PROFILE_DATA));
	if (pProfData == NULL)
	{
		DBGPRINT(RT_DEBUG_OFF, ("Set_StatETxBf_Proc: mem alloc failed\n"));
		return FALSE;
	}

	Read_TxBfProfile(pAd, pProfData, profileNum, FALSE);

	maxCarriers = pProfData->fortyMHz? PROFILE_MAX_CARRIERS_40: PROFILE_MAX_CARRIERS_20;

	for (scIndex=0; scIndex<maxCarriers; scIndex++) {
		for (i=0; i<pProfData->rows; i++) {
			int ival = (CHAR)(pProfData->data[scIndex][6*i]);
			int qval = (CHAR)(pProfData->data[scIndex][6*i+1]);
			col1Power[i] += ival*ival+qval*qval;

			if (pProfData->columns>1) {
				ival = (CHAR)(pProfData->data[scIndex][6*i+2]);
				qval = (CHAR)(pProfData->data[scIndex][6*i+3]);
				col2Power[i] += ival*ival+qval*qval;
			}

			if (pProfData->columns>2) {
				ival = (CHAR)(pProfData->data[scIndex][6*i+4]);
				qval = (CHAR)(pProfData->data[scIndex][6*i+5]);
				col3Power[i] += ival*ival+qval*qval;
			}
		}
	}

	/* Remove implied scale factor of 2^-14. Convert to thousandths */
	for (i=0; i<pProfData->rows; i++) {
		col1Power[i] >>= 10;
		col1Power[i] = ((col1Power[i]*1000)/maxCarriers)>>4;
		col2Power[i] >>= 10;
		col2Power[i] = ((col2Power[i]*1000)/maxCarriers)>>4;
		col3Power[i] >>= 10;
		col3Power[i] = ((col3Power[i]*1000)/maxCarriers)>>4;
	}

	/* Display stats */
	DBGPRINT(RT_DEBUG_OFF, ("ETxBF Stats:\n  %dx1=[0.%03lu 0.%03lu, 0.%03lu]\n",
				pProfData->rows, col1Power[0], col1Power[1], col1Power[2]));
	if (pProfData->columns==2) {
		DBGPRINT(RT_DEBUG_OFF, ("  %dx2=[0.%03lu 0.%03lu, 0.%03lu]\n",
					pProfData->rows, (col1Power[0]+col2Power[0])/2,
					(col1Power[1]+col2Power[1])/2, (col1Power[2]+col2Power[2])/2) );
		}
	if (pProfData->columns==3) {
		DBGPRINT(RT_DEBUG_OFF, ("  %dx3=[0.%03lu 0.%03lu, 0.%03lu]\n",
					pProfData->rows, (col1Power[0]+col2Power[0]+col3Power[0])/3,
					(col1Power[1]+col2Power[1]+col3Power[1])/3,
					(col1Power[2]+col2Power[2]+col3Power[2])/3) );
	}

	os_free_mem(NULL, pProfData);

	return TRUE;
}


/*
	Set_TxBfTag_Proc - Display BF Profile Tags
		usage: "iwpriv ra0 set TxBfTag=n"
					n: 0=>all,
					   1=>Explicit,
					   2=>Implicit,
					   3=>dump Power table
*/
INT	Set_TxBfTag_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int argVal = simple_strtol(arg, 0, 10);
	int profileNum;

	if (argVal==0 || argVal==1) {
		/* Display Explicit tagfield */
		DBGPRINT(RT_DEBUG_OFF, ("---Explicit TxBfTag:\n"));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 4);
		for (profileNum=0; profileNum<4; profileNum++)
			displayTagfield(pAd, profileNum, FALSE);
	}

	if (argVal==0 || argVal==2) {
		/* Display Implicit tagfield */
		DBGPRINT(RT_DEBUG_OFF, ("---Implicit TxBfTag:\n"));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 0);
		for (profileNum=0; profileNum<4; profileNum++)
			displayTagfield(pAd, profileNum, TRUE);
	}
#ifdef DOT11N_SS3_SUPPORT
	if (argVal==3) {
		int i;
		/* 4. Dump power table */
		for (i = 0; i < (14 + 12 + 16 + 7); i++)
			DBGPRINT(RT_DEBUG_OFF, ("%d: Ch%2d=[%d, %d %d]\n", i,
						pAd->TxPower[i].Channel, pAd->TxPower[i].Power,
						pAd->TxPower[i].Power2, pAd->TxPower[i].Power3));
	}
#endif /* DOT11N_SS3_SUPPORT */	

	return TRUE;
}
#endif


/*
	Set_InvTxBfTag_Proc - Invalidate BF Profile Tags
		usage: "iwpriv ra0 set InvTxBfTag=n"
		Reset Valid bit and zero out MAC address of each profile. The next profile will be stored in profile 0
*/
INT	Set_InvTxBfTag_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int profileNum;
	UCHAR row[EXP_MAX_BYTES];
	UCHAR r163Value = 0;
	UINT  rValue;

	/* Disable Profile Updates during access */
#ifndef MT76x2
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R163, &r163Value);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R163, r163Value & ~0x88);

	/* Invalidate Implicit tags */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 0);
	for (profileNum=0; profileNum<4; profileNum++) {
		Read_TagField(pAd, row, profileNum);
		row[0] &= 0x7F;
		row[1] = row[2] = row[3] = row[4] = row[5] = row[6] = 0xAA;
		Write_TagField(pAd, row, profileNum);
	}

	/* Invalidate Explicit tags */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 4);
	for (profileNum=0; profileNum<4; profileNum++) {
		Read_TagField(pAd, row, profileNum);
		row[0] &= 0x7F;
		row[1] = row[2] = row[3] = row[4] = row[5] = row[6] = 0x55;
		Write_TagField(pAd, row, profileNum);
	}

	/* Restore Profile Updates */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R163, r163Value);
#else
	PFMU_PROFILE	   *prof;

	if (os_alloc_mem(pAd, (UCHAR **)&prof, sizeof(PROFILE_DATA))!= NDIS_STATUS_SUCCESS)
	{
		return -3;
	}

	RTMP_IO_READ32(pAd, PFMU_R1, &rValue);
	RTMP_IO_WRITE32(pAd, PFMU_R1, rValue & ~0x330);

	for (profileNum=0; profileNum<8; profileNum++) {
		// Clear the profile valid bit
		prof->validFlg = 0;
		TxBfProfileTagValid(pAd, prof, profileNum);

		// Clear the mac address
		TxBfProfileTagRead(pAd, prof, profileNum);
		prof->DMAC[0] = prof->DMAC[1] = prof->DMAC[2] = 0xFF;
		prof->DMAC[3] = prof->DMAC[4] = prof->DMAC[5] = 0xFF;
		prof->CMDInIdx = 2;
		TxBfProfileTagWrite(pAd, prof, profileNum);
	}

	/* Restore Profile Updates */
	RTMP_IO_WRITE32(pAd, PFMU_R1, rValue);

	os_free_mem(pAd, prof);
#endif

	return TRUE;
}


#if defined(RT2883) || defined(RT3883) || defined(RT3593)
/*
	Set_ITxBfTimeout_Proc - Set ITxBF timeout value
	usage: iwpriv ra0 set ITxBfTimeout=<decimal timeout in units of 25 microsecs>
*/
INT Set_ITxBfTimeout_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG t = simple_strtol(arg, 0, 10);

	if (t > 65535) {
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ITxBfTimeout_Proc: value > 65535!\n"));
		return FALSE;
	}

    pAd->CommonCfg.ITxBfTimeout = t;
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 0x02);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, 0);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R182, pAd->CommonCfg.ITxBfTimeout & 0xFF);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, 1);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R182, (pAd->CommonCfg.ITxBfTimeout>>8) & 0xFF);
	
    DBGPRINT(RT_DEBUG_TRACE, ("Set_ITxBfTimeout_Proc::(ITxBfTimeout=%d)\n", (int)pAd->CommonCfg.ITxBfTimeout));
	return TRUE;
}


/*
	Set_ETxBfTimeout_Proc - Set ITxBF timeout value
		usage: iwpriv ra0 set ETxBfTimeout=<decimal timeout in units of 25 microsecs>
*/
INT Set_ETxBfTimeout_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG t = simple_strtol(arg, 0, 10);

	if (t > 65535) {
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ETxBfTimeout_Proc: value > 65535!\n"));
		return FALSE;
	}

    pAd->CommonCfg.ETxBfTimeout = t;

	// TODO: shiang-7603!! fix me
#if defined(MT7603_FPGA) || defined(MT7628_FPGA) || defined(MT7636_FPGA)
	if (IS_MT7603(pAd) || IS_MT7628(pAd) || IS_MT7636(pAd)) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): MT7603 Not support yet!\n", __FUNCTION__));
		return FALSE;
	}
#endif /* MT7603_FPGA */

	RTMP_IO_WRITE32(pAd, TX_TXBF_CFG_3, pAd->CommonCfg.ETxBfTimeout);
    DBGPRINT(RT_DEBUG_TRACE, ("Set_ETxBfTimeout_Proc::(ETxBfTimeout=%d)\n", (int)pAd->CommonCfg.ETxBfTimeout));
	return TRUE;
}
#endif


/*
	Set_ETxBfCodebook_Proc - Set ETxBf Codebook 
	usage: iwpriv ra0 set ETxBfCodebook=0 to 3
*/
INT Set_ETxBfCodebook_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	TX_TXBF_CFG_0_STRUC regValue;
	ULONG t = simple_strtol(arg, 0, 10);

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): Not support for HIF_MT yet!\n",
					__FUNCTION__));
		return FALSE;
	}

	if (t > 3) {
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ETxBfCodebook_Proc: value > 3!\n"));
		return FALSE;
	}

	RTMP_IO_READ32(pAd, TX_TXBF_CFG_0, &regValue.word);
	regValue.field.EtxbfFbkCode = t;
	RTMP_IO_WRITE32(pAd, TX_TXBF_CFG_0, regValue.word);
	return TRUE;
}


/*
	Set_ETxBfCoefficient_Proc - Set ETxBf Coefficient 
		usage: iwpriv ra0 set ETxBfCoefficient=0 to 3
*/
INT Set_ETxBfCoefficient_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	TX_TXBF_CFG_0_STRUC regValue;
	ULONG t = simple_strtol(arg, 0, 10);

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): Not support for HIF_MT yet!\n",
					__FUNCTION__));
		return FALSE;
	}

	if (t > 3) {
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ETxBfCoefficient_Proc: value > 3!\n"));
		return FALSE;
	}

	RTMP_IO_READ32(pAd, TX_TXBF_CFG_0, &regValue.word);
	regValue.field.EtxbfFbkCoef = t;
	RTMP_IO_WRITE32(pAd, TX_TXBF_CFG_0, regValue.word);
	return TRUE;
}


/*
	Set_ETxBfGrouping_Proc - Set ETxBf Grouping 
		usage: iwpriv ra0 set ETxBfGrouping=0 to 2
*/
INT Set_ETxBfGrouping_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	TX_TXBF_CFG_0_STRUC regValue;
	ULONG t = simple_strtol(arg, 0, 10);

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): Not support for HIF_MT yet!\n",
					__FUNCTION__));
		return FALSE;
	}

	if (t > 2) {
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ETxBfGrouping_Proc: value > 2!\n"));
		return FALSE;
	}

	RTMP_IO_READ32(pAd, TX_TXBF_CFG_0, &regValue.word);
	regValue.field.EtxbfFbkNg = t;
	RTMP_IO_WRITE32(pAd, TX_TXBF_CFG_0, regValue.word);
	return TRUE;
}


/*
	Set_ETxBfNoncompress_Proc - Set ETxBf Noncompress option 
		usage: iwpriv ra0 set ETxBfNoncompress=0 or 1
*/
INT Set_ETxBfNoncompress_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG t = simple_strtol(arg, 0, 10);

	if (t > 1) {
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ETxBfNoncompress_Proc: value > 1!\n"));
		return FALSE;
	}

	pAd->CommonCfg.ETxBfNoncompress = t;
	return TRUE;
}


/*
	Set_ETxBfIncapable_Proc - Set ETxBf Incapable option
		usage: iwpriv ra0 set ETxBfIncapable=0 or 1
*/
INT Set_ETxBfIncapable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG t = simple_strtol(arg, 0, 10);

	if (t > 1)
		return FALSE;

	pAd->CommonCfg.ETxBfIncapable = t;
	setETxBFCap(pAd, &pAd->CommonCfg.HtCapability.TxBFCap);

	return TRUE;
}


/*
	Set_ITxBfDivCal_Proc - Calculate ITxBf Divider Calibration parameters
	usage: iwpriv ra0 set ITxBfDivCal=dd
			0=>display calibration parameters
			1=>update EEPROM values
			2=>update BBP R176
			10=>display calibration parameters and dump capture data
			11=>Skip divider calibration, just capture and dump capture data
*/
INT	Set_ITxBfDivCal_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int calFunction;

	UINT32 value, value1, restore_value, loop = 0;

	/* backup mac 1004 value */
	RTMP_IO_READ32(pAd, 0x1004, &restore_value);
	
	/* Backup the original RTS retry count and then set to 0 */
	RTMP_IO_READ32(pAd, 0x1344, &pAd->rts_tx_retry_num);
	
	/* disable mac tx/rx */
	value = restore_value;
	value &= ~0xC;
	RTMP_IO_WRITE32(pAd, 0x1004, value);

	/* set RTS retry count = 0 */	
	RTMP_IO_WRITE32(pAd, 0x1344, 0x00092B00);

	/* wait mac 0x1200, bbp 0x2130 idle */
	do {
		RTMP_IO_READ32(pAd, 0x1200, &value);
		value &= 0x1;
		RTMP_BBP_IO_READ32(pAd, 0x2130, &value1);
		DBGPRINT(RT_DEBUG_INFO, ("%s:: Wait until MAC 0x1200 bit0 and BBP 0x2130 become 0\n", __FUNCTION__));
		RtmpusecDelay(1);
		loop++;
	} while (((value != 0) || (value1 != 0)) && (loop < 300));
	
	if (loop >= 300) {
		DBGPRINT(RT_DEBUG_OFF, ("%s:: Wait until MAC 0x1200 bit0 and BBP 0x2130 become 0 > 300 times\n", __FUNCTION__));
		return FALSE;
	}

	calFunction = simple_strtol(arg, 0, 10);
	pAd->chipOps.fITxBfDividerCalibration(pAd, calFunction, 0, NULL);

	/* enable TX/RX */
	RTMP_IO_WRITE32(pAd, 0x1004, restore_value);

	/* Restore RTS retry count */
	RTMP_IO_WRITE32(pAd, 0x1344, pAd->rts_tx_retry_num);	

	return TRUE;
}


/*
	Set_ITxBfLNACal_Proc - Calculate ITxBf LNA Calibration parameters
	usage: iwpriv ra0 set ITxBfLnaCal=dd
			0=>display calibration parameters
			1=>update EEPROM values
			2=>update BBP R174
			10=>display calibration parameters and dump capture data
*/
INT	Set_ITxBfLnaCal_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR channel = pAd->CommonCfg.Channel;
	int calFunction;

#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
		channel = pAd->ate.Channel;
#endif /* CONFIG_ATE */

	calFunction = simple_strtol(arg, 0, 10);

	return pAd->chipOps.fITxBfLNACalibration(pAd, calFunction, 0, channel<=14);
}


#if defined(RT2883) || defined(RT3883) || defined(RT3593)
/*
	Set_ITxBfCal_Proc - Calculate ITxBf Calibration parameters
	usage: "iwpriv ra0 set ITxBfCal=[0 | 1]"
			0=>calculate values, 1=>update BBP and EEPROM
*/
INT	Set_ITxBfCal_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int calFunction = simple_strtol(arg, 0, 10);
	int calParams[2];
	int ret;
	UCHAR channel = pAd->CommonCfg.Channel;

#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
		channel = pAd->ate.Channel;
#endif /* CONFIG_ATE */

	ret = iCalcCalibration(pAd, calParams, 0);
	if (ret < 0) {
		if (ret == -3)
			DBGPRINT(RT_DEBUG_OFF, ("Set_ITxBfCal_Proc: mem alloc failed\n"));
		else if (ret == -2)
		DBGPRINT(RT_DEBUG_OFF, ("Set_ITxBfCal_Proc: MAC Address mismatch\n"));
		else
		DBGPRINT(RT_DEBUG_OFF, ("Set_ITxBfCal_Proc: Invalid profiles\n"));
		return FALSE;
	}

	/* Display result */
	DBGPRINT((calFunction==0? RT_DEBUG_OFF: RT_DEBUG_WARN),
				("ITxBfCal Result = [0x%02x 0x%02x]\n", calParams[0], calParams[1]));

#ifdef CONFIG_ATE
	pAd->ate.calParams[0] = (UCHAR)calParams[0];
	pAd->ate.calParams[1] = (UCHAR)calParams[1];

	/* Double check */
	DBGPRINT((calFunction==0? RT_DEBUG_OFF: RT_DEBUG_WARN),
				("ITxBfCal Result in ATE = [0x%02x 0x%02x]\n", pAd->ate.calParams[0], pAd->ate.calParams[1]));
#endif /* CONFIG_ATE */

	/* Update BBP R176 and EEPROM for Ant 0 and 2 */
	if (calFunction == 1) {
		UCHAR r27Value = 0, r173Value = 0;
		ITXBF_PHASE_PARAMS phaseParams;
		UCHAR divPhase[2] = {0}, phaseValues[2] = {0};

		/* Read R173 to see if Phase compensation is already enabled */
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R173, &r173Value);

		/* Select Ant 0 */
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &r27Value);
		r27Value &= ~0x60;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, r27Value);

		/* Update R176 */
		if (r173Value & 0x08) {
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R176, &phaseValues[0]);
			phaseValues[0] += calParams[0];
		}
		else
			phaseValues[0] = calParams[0];
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, phaseValues[0]);

		/* Select Ant 2 */
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, r27Value | 0x40);

		/* Update R176 */
		if (r173Value & 0x08) {
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R176, &phaseValues[1]);
			phaseValues[1] += calParams[1];
		}
		else
			phaseValues[1] = calParams[1];
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, phaseValues[1]);

		/* Enable TX Phase Compensation */
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R173, r173Value | 0x08);

		/* Remove Divider phase */
		pAd->chipOps.fITxBfDividerCalibration(pAd, 3, 0, divPhase);
		phaseValues[0] -= divPhase[0];
		phaseValues[1] -= divPhase[1];

		/* Update EEPROM */

		ITxBFGetEEPROM(pAd, &phaseParams, 0, 0);

		/* Only allow calibration on specific channels */
		if (channel == 1) {
			phaseParams.E0gBeg[0] = phaseValues[0];
			phaseParams.E0gBeg[1] = phaseValues[1];
		}
		else if (channel == 14) {
			phaseParams.E0gEnd[0] = phaseValues[0];
			phaseParams.E0gEnd[1] = phaseValues[1];
		}
		else if (channel == 36) {
			phaseParams.E0aLowBeg[0] = phaseValues[0];
			phaseParams.E0aLowBeg[1] = phaseValues[1];
		}
		else if (channel == 64) {
			phaseParams.E0aLowEnd[0] = phaseValues[0];
			phaseParams.E0aLowEnd[1] = phaseValues[1];
		}
		else if (channel == 100) {
			phaseParams.E0aMidBeg[0] = phaseValues[0];
			phaseParams.E0aMidBeg[1] = phaseValues[1];
		}
		else if (channel == 128) {
			phaseParams.E0aMidEnd[0] = phaseValues[0];
			phaseParams.E0aMidEnd[1] = phaseValues[1];
		}
		else if (channel == 132) {
			phaseParams.E0aHighBeg[0] = phaseValues[0];
			phaseParams.E0aHighBeg[1] = phaseValues[1];
		}
		else if (channel == 165) {
			phaseParams.E0aHighEnd[0] = phaseValues[0];
			phaseParams.E0aHighEnd[1] = phaseValues[1];
		}
		else {
			DBGPRINT(RT_DEBUG_OFF,
					("Invalid channel: %d\nMust calibrate channel 1, 14, 36, 64, 100, 128, 132 or 165", channel) );
			return FALSE;
		}
		ITxBFSetEEPROM(pAd, &phaseParams, 0, 0);

		DBGPRINT(RT_DEBUG_WARN, ("Set_ITxBfCal_Proc: Calibration Parameters updated\n"));
	}

	return TRUE;
}
#endif

#ifdef MT76x2
INT	mt76x2_Set_ITxBfCal_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int calFunction = simple_strtol(arg, 0, 10);
	int calParams[2];
	int ret;
	UINT value32, r27Value = 0, r173Value = 0;
	ITXBF_PHASE_PARAMS phaseParams;
	UCHAR divPhase[2] = {0}, phaseValues[2] = {0};
	UCHAR channel = pAd->CommonCfg.Channel;

#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
		channel = pAd->ate.Channel;
#endif /* CONFIG_ATE */

	ret = iCalcCalibration(pAd, calParams, 0);
	if (ret < 0) {
		if (ret == -3)
			DBGPRINT(RT_DEBUG_OFF, ("Set_ITxBfCal_Proc: kmalloc failed\n"));
		else if (ret == -2)
			DBGPRINT(RT_DEBUG_OFF, ("Set_ITxBfCal_Proc: MAC Address mismatch\n"));
		else
			DBGPRINT(RT_DEBUG_OFF, ("Set_ITxBfCal_Proc: Invalid profiles\n"));

		return FALSE;
	}

	/* Update BBP R176 and EEPROM for Ant 0 and 2 */
	switch (calFunction)
	{
		case 0:
			/* Remove Divider phase */
			pAd->chipOps.fITxBfDividerCalibration(pAd, 10, 0, divPhase);
			phaseValues[0] = calParams[0] - divPhase[0];
			DBGPRINT(RT_DEBUG_OFF, ("ITxBfCal Result = [0x%02x]\n", phaseValues[0]));
#ifdef CONFIG_ATE
			pAd->ate.calParams[0] = (UCHAR)phaseValues[0];

			/* Double check */
			DBGPRINT((calFunction==0? RT_DEBUG_OFF: RT_DEBUG_WARN),
					("ITxBfCal Result in ATE = [0x%02x]\n", pAd->ate.calParams[0]));
#endif /* CONFIG_ATE */
			break;

		case 1:
			/* Display result */
			DBGPRINT(RT_DEBUG_OFF, ("ITxBfCal Result = [0x%02x]\n", calParams[0]));
#ifdef CONFIG_ATE
			pAd->ate.calParams[0] = (UCHAR)calParams[0];

			/* Double check */
			DBGPRINT((calFunction==0? RT_DEBUG_OFF: RT_DEBUG_WARN),
					("ITxBfCal Result in ATE = [0x%02x]\n", pAd->ate.calParams[0]));
#endif /* CONFIG_ATE */

			//mt_rf_write(pAd, RF_Path0,	   RFDIGI_TRX4, 0x28585); // Auto LNA gain
			//mt_rf_write(pAd, RF_Path1,	   RFDIGI_TRX4, 0x28585); // Auto LNA gain
			//RTMP_IO_WRITE32(pAd,CORE_R4,   0x00000001); // core soft reset enable
			//RTMP_IO_WRITE32(pAd,CORE_R4,   0x00000000); // core soft reset disable

			/* Select Ant 0 */
			RTMP_IO_WRITE32(pAd, CAL_R0, 0);

			/* Update TXBE_R13 */
			//phaseValues[0] = 0x64;
			RTMP_IO_WRITE32(pAd, TXBE_R13, calParams[0]);

			/* Remove Divider phase */
			pAd->chipOps.fITxBfDividerCalibration(pAd, 10, 0, divPhase);
			phaseValues[0] = calParams[0] - divPhase[0];
			DBGPRINT(RT_DEBUG_OFF, (
				    "%s : \n"
				    "Phase compensated value = 0x%x\n"
			        "Residual phase          = 0x%x\n",
			        __FUNCTION__, calParams[0], phaseValues[0]));

			/* Enable TX Phase Compensation */
			RTMP_IO_READ32(pAd, TXBE_R12, &value32);
			RTMP_IO_WRITE32(pAd, TXBE_R12, value32 | 0x08);

			/* Enable Rx Phase compensation */
			//pAd->chipOps.fITxBfLNAPhaseCompensate(pAd);

			/* Update EEPROM */
			ITxBFGetEEPROM(pAd, &phaseParams, 0, 0);

			/* Only allow calibration on specific channels */
			if (channel == 1) {
				phaseParams.E1gBeg = phaseValues[0];
			}
			else if (channel == 14) {
				phaseParams.E1gEnd = phaseValues[0];
			}
			else if (channel == 36) {
				phaseParams.E1aLowBeg = phaseValues[0];
			}
			else if (channel == 64) {
				phaseParams.E1aLowEnd = phaseValues[0];
			}
			else if (channel == 100) {
				phaseParams.E1aMidBeg = phaseValues[0];
			}
			else if (channel == 120) {
				phaseParams.E1aMidMid = phaseValues[0];
			}
			else if (channel == 140) {
				phaseParams.E1aMidEnd = phaseValues[0];
			}
			else if (channel == 149) {
				phaseParams.E1aHighBeg = phaseValues[0];
			}
			else if (channel == 173) {
				phaseParams.E1aHighEnd = phaseValues[0];
			}
			else {
				DBGPRINT(RT_DEBUG_OFF,
						("Invalid channel: %d\nMust calibrate channel 1, 14, 36, 64, 100, 128, 132 or 165", channel) );
				return FALSE;
			}

			ITxBFSetEEPROM(pAd, &phaseParams, 0, 0);

			DBGPRINT(RT_DEBUG_WARN, ("Set_ITxBfCal_Proc: Calibration Parameters updated\n"));

			break;
		default:
			break;
	}

	return TRUE;
}

#endif // MT76x2


/*
	Set_ETxBfEnCond_Proc - enable/disable ETxBF
	usage: iwpriv ra0 set ETxBfEnCond=dd
		0=>disable, 1=>enable
	Note: After use this command, need to re-run apStartup()/LinkUp() operations to sync all status.
		  If ETxBfIncapable!=0 then we don't need to reassociate.
*/
INT	Set_ETxBfEnCond_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR i, enableETxBf;
	MAC_TABLE_ENTRY		*pEntry;
	UINT8	byteValue;

	enableETxBf = simple_strtol(arg, 0, 10);

	if (enableETxBf > 1)
		return FALSE;

	pAd->CommonCfg.ETxBfEnCond = enableETxBf && (pAd->Antenna.field.TxPath > 1);
	pAd->CommonCfg.RegTransmitSetting.field.TxBF = enableETxBf==0? 0: 1;

	setETxBFCap(pAd, &pAd->CommonCfg.HtCapability.TxBFCap);
#ifdef VHT_TXBF_SUPPORT
	setVHTETxBFCap(pAd, &pAd->CommonCfg.vht_cap_ie.vht_cap);
#endif
	rtmp_asic_set_bf(pAd);

	if (enableETxBf) 
	{
		RTMP_IO_WRITE32(pAd,PFMU_R54, 0x150); // Solve the MCS8 and MCS9 TP degradation when PN on
	}	
	else
	{
		RTMP_IO_WRITE32(pAd,PFMU_R54, 0x100);
	}


	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		pEntry = &pAd->MacTab.Content[i];
		if (!IS_ENTRY_NONE(pEntry))
		{
#ifdef VHT_TXBF_SUPPORT
			if (WMODE_CAP_AC(pAd->CommonCfg.PhyMode) &&
				(pAd->CommonCfg.Channel > 14))
				pEntry->eTxBfEnCond = clientSupportsVHTETxBF(pAd, &pEntry->vht_cap_ie.vht_cap) ? enableETxBf: 0;
			else
#endif
				pEntry->eTxBfEnCond = clientSupportsETxBF(pAd, &pEntry->HTCapability.TxBFCap)? enableETxBf: 0;
			pEntry->bfState = READY_FOR_SNDG0;
			pEntry->HTPhyMode.field.eTxBF = pEntry->eTxBfEnCond;
#ifdef MCS_LUT_SUPPORT
			asic_mcs_lut_update(pAd, pEntry);
#endif
		}

		DBGPRINT(RT_DEBUG_TRACE, ("Set ETxBfEn=%d, Final ETxBF status =%d!\n",
					enableETxBf , pEntry->eTxBfEnCond));
	}


#ifndef MT76x2
	if (pAd->CommonCfg.RegTransmitSetting.field.ITxBfEn || enableETxBf)
	{
		RT30xxReadRFRegister(pAd, RF_R39, (PUCHAR)&byteValue);
		byteValue |= 0x40;
		RT30xxWriteRFRegister(pAd, RF_R39, (UCHAR)byteValue);

		RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)&byteValue);
		byteValue |= 0x20;
		RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)byteValue);
	}
	else
	{
		/* depends on Gary Tsao's comments. we shall disable it */
		if (pAd->CommonCfg.RegTransmitSetting.field.ITxBfEn == 0)
		{
			RT30xxReadRFRegister(pAd, RF_R39, (PUCHAR)&byteValue);
			byteValue &= (~0x40);
			RT30xxWriteRFRegister(pAd, RF_R39, (UCHAR)byteValue);

			RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)&byteValue);
			byteValue &= (~0x20);
			RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)byteValue);
		}
	}
#endif

	return TRUE;	
}

INT	Set_NoSndgCntThrd_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR i;
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++){
		pAd->MacTab.Content[i].noSndgCntThrd = simple_strtol(arg, 0, 10);
	}
	return TRUE;	
}

INT	Set_NdpSndgStreams_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR i;
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++){
		pAd->MacTab.Content[i].ndpSndgStreams = simple_strtol(arg, 0, 10);
	}
	return TRUE;	
}


INT	Set_Trigger_Sounding_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR			macAddr[MAC_ADDR_LEN];
	CHAR			*value;
	INT				i;
	MAC_TABLE_ENTRY *pEntry = NULL;

	/* Mac address acceptable format 01:02:03:04:05:06 length 17 */
	if(strlen(arg) != 17)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":")) 
	{
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
			return FALSE;  /*Invalid*/

		AtoH(value, &macAddr[i++], 1);
	}

	/*DBGPRINT(RT_DEBUG_TRACE, ("TriggerSounding=%02x:%02x:%02x:%02x:%02x:%02x\n",*/
	/*		macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5], macAddr[6]) );*/
	pEntry = MacTableLookup(pAd, macAddr);
	if (pEntry==NULL)
		return FALSE;

	Trigger_Sounding_Packet(pAd, SNDG_TYPE_SOUNDING, 0, pEntry->sndgMcs, pEntry);

	return TRUE;
}

/*
	Set_ITxBfEn_Proc - enable/disable ITxBF
	usage: iwpriv ra0 set ITxBfEn=dd
	0=>disable, 1=>enable
*/
#ifndef MT76x2
INT	Set_ITxBfEn_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR i;
	UCHAR enableITxBF;
	BOOLEAN bCalibrated;
	UINT8 byteValue;

	enableITxBF = simple_strtol(arg, 0, 10);

	if (enableITxBF > 1)
		return FALSE;

	bCalibrated = rtmp_chk_itxbf_calibration(pAd);
	DBGPRINT(RT_DEBUG_TRACE, ("Set ITxBfEn=%d, calibration of ITxBF=%d, so enableITxBF=%d!\n", 
					enableITxBF , bCalibrated, (enableITxBF & bCalibrated)));
	
	enableITxBF &= bCalibrated;
	
	pAd->CommonCfg.RegTransmitSetting.field.ITxBfEn = enableITxBF && (pAd->Antenna.field.TxPath > 1);

	rtmp_asic_set_bf(pAd);

	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		MAC_TABLE_ENTRY *pMacEntry = &pAd->MacTab.Content[i];
		if ((!IS_ENTRY_NONE(pMacEntry)) && (pAd->Antenna.field.TxPath> 1))
			pMacEntry->iTxBfEn = enableITxBF;
	}

	if (enableITxBF || pAd->CommonCfg.ETxBfEnCond)
	{
		RT30xxReadRFRegister(pAd, RF_R39, (PUCHAR)&byteValue);
		byteValue |= 0x40;
		RT30xxWriteRFRegister(pAd, RF_R39, (UCHAR)byteValue);

		RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)&byteValue);
		byteValue |= 0x20;
		RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)byteValue);
	}
	
	/* If enabling ITxBF then set LNA compensation, do a Divider Calibration and update BBP registers */
	if (enableITxBF) {
		pAd->chipOps.fITxBfLNAPhaseCompensate(pAd);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 2, 0, NULL);
	}
	else
	{
		/* depends on Gary Tsao's comments. */
		if (pAd->CommonCfg.ETxBfEnCond == 0)
		{
		RT30xxReadRFRegister(pAd, RF_R39, (PUCHAR)&byteValue);
			byteValue &= (~0x40);
		RT30xxWriteRFRegister(pAd, RF_R39, (UCHAR)byteValue);

		RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)&byteValue);
			byteValue &= (~0x20);
		RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)byteValue);
	}
	
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R173, 0);

		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &byteValue);
		byteValue &= ~0x60;
		for ( i = 0; i < 3; i++)
		{	
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, (byteValue & (i << 5)));
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R174, 0);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, 0);
		}
	}
	return TRUE;	
}

#else
INT	Set_ITxBfEn_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32  i;
	UCHAR   enableITxBF;
	BOOLEAN bCalibrated;
	UINT8   byteValue;
	UINT    value32;
	ULONG   irq_flags;


	enableITxBF = simple_strtol(arg, 0, 10);

	if (enableITxBF > 1)
		return FALSE;

	bCalibrated = rtmp_chk_itxbf_calibration(pAd);
	DBGPRINT(RT_DEBUG_TRACE, ("Set ITxBfEn=%d, calibration of ITxBF=%d, so enableITxBF=%d!\n",
					enableITxBF , bCalibrated, (enableITxBF & bCalibrated)));

	enableITxBF &= bCalibrated;

	pAd->CommonCfg.RegTransmitSetting.field.ITxBfEn = enableITxBF && (pAd->Antenna.field.TxPath > 1);
	DBGPRINT(RT_DEBUG_TRACE, ("Final ITxBf status=%d, TxPath num=%d\n",
					pAd->CommonCfg.RegTransmitSetting.field.ITxBfEn , pAd->Antenna.field.TxPath > 1));

	rtmp_asic_set_bf(pAd);

	RTMP_IO_READ32(pAd, PFMU_R1, &value32);
	DBGPRINT(RT_DEBUG_TRACE, ("TxBf HW status=0x%x\n", value32));

	/* Check how many clients could be applied ITxBf */
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		MAC_TABLE_ENTRY *pMacEntry = &pAd->MacTab.Content[i];
		if ((!IS_ENTRY_NONE(pMacEntry)) && (pAd->Antenna.field.TxPath> 1))
		{
			pMacEntry->iTxBfEn = enableITxBF;
			pMacEntry->HTPhyMode.field.iTxBF = enableITxBF;

#ifdef MCS_LUT_SUPPORT
			asic_mcs_lut_update(pAd, pMacEntry);
#endif
		}

		DBGPRINT(RT_DEBUG_OFF, ("MCS LUT check\n"));
	}

	/* If enabling ITxBF then set LNA compensation, do a Divider Calibration and update BBP registers */
	if (enableITxBF) {
		/* Enable TX/Rx Phase Compensation */
		//mt_rf_write(pAd, RF_Path0,	   RFDIGI_TRX4, 0x28585);
		//mt_rf_write(pAd, RF_Path1,	   RFDIGI_TRX4, 0x28585);
		pAd->chipOps.fITxBfLNAPhaseCompensate(pAd);

		/* Enable TX Phase Compensation */
		RTMP_IO_READ32(pAd, TXBE_R12, &value32);
		RTMP_IO_WRITE32(pAd, TXBE_R12, value32 | 0x08);

		RtmpOsMsDelay(10); // waiting 10ms

		// Residual phase calculation
		//////ITxBFPhaseCalibrationStartUp(pAd, 0);
	}
	else
	{
		/* Disable Tx/Rx Phase Compensation */
		RTMP_IO_READ32(pAd, TXBE_R12, &value32);
		RTMP_IO_WRITE32(pAd, TXBE_R12, value32 & (~0x0FF));
	}

	return TRUE;
}

#endif
#endif /* TXBF_SUPPORT */


#ifdef VHT_TXBF_SUPPORT
/*
	The VhtNDPA sounding inupt string format should be xx:xx:xx:xx:xx:xx-d,
		=>The six 2 digit hex-decimal number previous are the Mac address, 
		=>The seventh decimal number is the MCS value.
*/
INT Set_VhtNDPA_Sounding_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR mac[6];
	UINT mcs;
	RTMP_STRING *token;
	RTMP_STRING sepValue[] = ":", DASH = '-';
	INT i;
	MAC_TABLE_ENTRY *pEntry = NULL;

	DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));

	/*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and MCS value in decimal format.*/
	if(strlen(arg) < 19)
		return FALSE;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		mcs = (UINT)simple_strtol((token+1), 0, 10);
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&mac[i]), 1);
		}
		if (i != 6)
			return FALSE;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x\n", 
					PRINT_MAC(mac), mcs));

		pEntry = MacTableLookup(pAd, (PUCHAR) mac);
	    	if (pEntry) {
#ifdef SOFT_SOUNDING
			pEntry->snd_rate.field.MODE = MODE_VHT;
			pEntry->snd_rate.field.BW = (mcs / 100) > BW_80 ? BW_80 : (mcs / 100);
			mcs %= 100;
			pEntry->snd_rate.field.MCS = ((mcs / 10) << 4 | (mcs % 10));

			DBGPRINT(RT_DEBUG_OFF, ("%s():Trigger VHT NDPA Sounding=%02x:%02x:%02x:%02x:%02x:%02x, snding rate=VHT-%sHz, %dSS-MCS%d\n",
					__FUNCTION__, PRINT_MAC(mac),
					get_bw_str(pEntry->snd_rate.field.BW),
					(pEntry->snd_rate.field.MCS >> 4) + 1,
					pEntry->snd_rate.field.MCS & 0xf));
#endif
			trigger_vht_ndpa(pAd, pEntry);
	    	}

		return TRUE;
	}

	return FALSE;
}
#endif /* VHT_TXBF_SUPPORT */

#ifdef MT76x2
#ifdef TXBF_SUPPORT
USHORT  PFMU_TimeOut;
UCHAR	MatrixForm[5];
UCHAR	StsSnr[2];
UCHAR	TxScale[4];
UCHAR	macAddr[MAC_ADDR_LEN];
UCHAR   FlgStatus[4];
USHORT  CMDInIdx = 0, dCMDInIdx = 0;
UCHAR   psi21;
UCHAR   phill;



INT Set_TxBfProfileTag_Help(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	DBGPRINT(RT_DEBUG_OFF, (
		"====================================================================================================\n"
		   "TxBfProfile Tag setting example :\n"
	       "iwpriv ra0 set TxBfProfileTagMatrix=ng:cw:nrow:nol:LM\n"
	       "iwpriv ra0 set TxBfProfileTagMac=xx:xx:xx:xx:xx:xx\n"
	       "iwpriv ra0 set TxBfProfileTagTxScale=Tx1_scale_2ss[7:0]:Tx0_scale_2ss[7:0]:Tx1_scale_1ss[7:0]:Tx0_scale_1ss[7:0]\n"
	       "iwpriv ra0 set TxBfProfileTagSNR=STS1_SNR[7:0]:STS0_SNR[7:0]\n"
	       "iwpriv ra0 set TxBfProfileTagTimeOut=timeout[15:0]\n"
	       "iwpriv ra0 set TxBfProfileTagFlg=LD:EO:IO:I/E\n"
	       "\n\n"
	       "iwpriv ra0 set ATETXBF=xx (0:no BF; 1:ETxBF, 2:ITxBF, 3:Enable TXBF support, 4:Disable TXBF support)\n"
		   "iwpriv ra0 set TxBfProfileTagValid=0/1:profile index\n"
		   "\n"
	       "Read TxBf profile Tag :\n"
	       "iwpriv ra0 set TxBfProfileTagRead=xx (select a profile only)\n"
	       "\n"
	       "Write TxBf profile Tag :\n"
	       "iwpriv ra0 set TxBfProfileTagWrite=xx (select a profile only)\n"
		   "\n\n"
		   "Read TxBf profile Data	:\n"
	       "iwpriv ra0 set TxBfProfileDataRead=xx:xx (select a profile and subcarrier to load seting)\n"
	       "iwpriv ra0 set TxBfProfileDataReadAll=TxBfType:subcarrier:BW  (TxBfType : 0x00 (IxBf), 0x01 (ETxBf)\n"
	       "                                                              (BW       : 0x00 (20M) , 0x01 (40M), 0x02 (80M)\n"
	       "\n"
	       "Write TxBf profile Data :\n"
	       "iwpriv ra0 set TxBfProfileDataWrite=profile:subcarrier:psi21:pill\n"
	       "iwpriv ra0 set TxBfProfileDataWriteAll=TxBfType:subcarrier:BW  (TxBfType : 0x00 (IxBf), 0x01 (ETxBf)\n"
	       "                                                               (BW       : 0x00 (20M) , 0x01 (40M), 0x02 (80M)\n"
		   "======================================================================\n"));
	CMDInIdx = 0; // Clear index
	return TRUE;
}


INT Set_TxBfProfileTagValid(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR   profileIdx, validFlg;
	UCHAR   Input[2];
	CHAR	*value;
	UINT    value32, readValue32[5];
	INT 	i;

	/* Profile Select : Subcarrier Select */
	if(strlen(arg) != 5)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &Input[i++], 1);
	}

	validFlg    = Input[0];
	profileIdx = Input[1];

	// Read PFMU_R10 (0x2f28) first
	RTMP_IO_READ32(pAd, PFMU_R10, &value32);
	value32 &= (~0x3C00);
	// Wite PFMU_R10 to trigger read command
	RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|value32));
	// Read PFMU_R11 ~ R15
	RTMP_IO_READ32(pAd, PFMU_R11, &readValue32[0]);
	RTMP_IO_READ32(pAd, PFMU_R12, &readValue32[1]);
	RTMP_IO_READ32(pAd, PFMU_R13, &readValue32[2]);
	RTMP_IO_READ32(pAd, PFMU_R14, &readValue32[3]);
	RTMP_IO_READ32(pAd, PFMU_R15, &readValue32[4]);


	/*
	    Update the valid bit
	*/
	readValue32[0] &= ~(1 << 7);
	readValue32[0] |= validFlg << 7;

	// Read PFMU_R10 (0x2f28) first
	RTMP_IO_READ32(pAd, PFMU_R10, &value32);
	value32 &= (~0x3C00);
	// Wite PFMU_R10 to trigger read command
	RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|value32));
	// Write PFMU_R11 ~ R15
	RTMP_IO_WRITE32(pAd, PFMU_R15, readValue32[4]);
	RTMP_IO_WRITE32(pAd, PFMU_R14, readValue32[3]);
	RTMP_IO_WRITE32(pAd, PFMU_R13, readValue32[2]);
	RTMP_IO_WRITE32(pAd, PFMU_R12, readValue32[1]);
	RTMP_IO_WRITE32(pAd, PFMU_R11, readValue32[0]);

	return TRUE;
}


INT Set_TxBfProfileTagRead(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG   value64;
	UCHAR   profileIdx, subcarrierIdx;
	UCHAR   Input[2];
	CHAR	*value, value8;
	UINT    value32, readValue32[5];
	INT 	i;
	UCHAR   validFlg;

	profileIdx = simple_strtol(arg, 0, 10);

	// Read PFMU_R10 (0x2f28) first
	RTMP_IO_READ32(pAd, PFMU_R10, &value32);
	value32 &= (~0x3C00);
	// Wite PFMU_R10 to trigger read command
	RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|value32));
	// Read PFMU_R19 ~ R23
	RTMP_IO_READ32(pAd, PFMU_R11, &readValue32[0]);
	RTMP_IO_READ32(pAd, PFMU_R12, &readValue32[1]);
	RTMP_IO_READ32(pAd, PFMU_R13, &readValue32[2]);
	RTMP_IO_READ32(pAd, PFMU_R14, &readValue32[3]);
	RTMP_IO_READ32(pAd, PFMU_R15, &readValue32[4]);

	/*
	    Bit 63:62       ng[1:0]
	         61:60       cw[1:0]
	         59:58       nrow[1:0]
	         57:56       ncol[1:0]
	         1 :  0       LM[1:0]
	*/
	MatrixForm[0] = (UCHAR)((readValue32[1] >> 30) & 0x3);
	MatrixForm[1] = (UCHAR)((readValue32[1] >> 28) & 0x3);
	MatrixForm[2] = (UCHAR)((readValue32[1] >> 26) & 0x3);
	MatrixForm[3] = (UCHAR)((readValue32[1] >> 24) & 0x3);
	MatrixForm[4] = (UCHAR)(readValue32[0] & 0x3);

	/*
	    Bit 55 : 8       Mac Adress [47:0]
	*/
	macAddr[0] = (UCHAR)((readValue32[1] >> 16) & 0x0FF);
	macAddr[1] = (UCHAR)((readValue32[1] >> 8) & 0x0FF);
	macAddr[2] = (UCHAR)((readValue32[1]) & 0x0FF);
	macAddr[3] = (UCHAR)((readValue32[0] >> 24) & 0x0FF);
	macAddr[4] = (UCHAR)((readValue32[0] >> 16) & 0x0FF);
	macAddr[5] = (UCHAR)((readValue32[0] >> 8)  & 0x0FF);

	/*
	    Bit 63 : 56       Tx1_scale_2ss[7:0]
	         55 : 48       Tx0_scale_2ss[7:0]
	         47 : 40       Tx1_scale_1ss[7:0]
	         36 : 32       Tx0_scale_2ss[7:0]
	*/
	TxScale[0] = (UCHAR)((readValue32[3] >> 24) & 0x0FF);
	TxScale[1] = (UCHAR)((readValue32[3] >> 16) & 0x0FF);
	TxScale[2] = (UCHAR)((readValue32[3] >> 8) & 0x0FF);
	TxScale[3] = (UCHAR)((readValue32[3] >> 0) & 0x0FF);

	/*
	    Bit 31 : 24       STS1_SNR[7:0]
	         23 : 16       STS0_SNR[7:0]
	*/
	StsSnr[0] = (UCHAR)((readValue32[2] >> 24) & 0x0FF);
	StsSnr[1] = (UCHAR)((readValue32[2] >> 16) & 0x0FF);

	/*
	    Bit 15 : 0       PFMU_TimeOut[15:0]
	*/
	PFMU_TimeOut = (USHORT)(readValue32[2] & 0x0FFFF);

	/*
	    LD, EO, IO, I/E
	*/
	FlgStatus[0] = (UCHAR)((readValue32[4] >> 8) & 0x1);
	FlgStatus[1] = (UCHAR)((readValue32[4] >> 7) & 0x1);
	FlgStatus[2] = (UCHAR)((readValue32[4] >> 6) & 0x1);
	FlgStatus[3] = (UCHAR)((readValue32[4] >> 5) & 0x1);

	/*
	    Check profile valid staus
	*/
	validFlg = (readValue32[0] >> 7) & 0x1;

	DBGPRINT(RT_DEBUG_OFF, (
		"============================= TxBf profile Tage Info ==============================\n"
		   "Pofile valid status = %d\n"
		   "Profile index = %d\n\n"
		   "PFMU_11 = 0x%x, PFMU_R12=0x%x, PFMU_R13=0x%x, PFMU_R14=0x%x\n\n"
		   "ng[1:0]=%d, cw[1:0]=%d, nrow[1:0]=%d, ncol[1:0]=%d, LM[1:0]=%d\n\n"
		   "Destimation MAC Address=%x:%x:%x:%x:%x:%x\n\n"
		   "Tx1_scal_2ss=0x%x, Tx0_scale_2ss=0x%x, Tx1_scal_1ss=0x%x, Tx0_scale_1ss=0x%x\n\n"
		   "STS1_SNR=0x%x, STS0_SNR=0x%x\n"
		   "timeout[15:0]=0x%x\n"
		   "LD=%d, EO=%d, IO=%d, I/E=%d\n"
		   "===================================================================================\n",
		   validFlg,
		   profileIdx,
		   readValue32[0], readValue32[1], readValue32[2], readValue32[3],
		   MatrixForm[0], MatrixForm[1], MatrixForm[2], MatrixForm[3], MatrixForm[4],
		   macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5],
		   TxScale[0], TxScale[1], TxScale[2], TxScale[3],
		   StsSnr[0], StsSnr[1],
		   PFMU_TimeOut,
		   FlgStatus[0], FlgStatus[1], FlgStatus[2], FlgStatus[3]));
	return TRUE;
}


INT Set_TxBfProfileTagWrite(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR   profileIdx, subcarrierIdx;
	UCHAR   Input[2];
	CHAR	*value;
	UINT    value32, readValue32[5];
	INT 	i;

	profileIdx = simple_strtol(arg, 0, 10);

	// Read PFMU_R10 (0x2f28) first
	RTMP_IO_READ32(pAd, PFMU_R10, &value32);
	value32 &= (~0x3C00);
	// Wite PFMU_R10 to trigger read command
	RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|value32));
	// Read PFMU_R19 ~ R23
	RTMP_IO_READ32(pAd, PFMU_R11, &readValue32[0]);
	RTMP_IO_READ32(pAd, PFMU_R12, &readValue32[1]);
	RTMP_IO_READ32(pAd, PFMU_R13, &readValue32[2]);
	RTMP_IO_READ32(pAd, PFMU_R14, &readValue32[3]);
	RTMP_IO_READ32(pAd, PFMU_R15, &readValue32[4]);

	DBGPRINT(RT_DEBUG_OFF, (
		"============================= TxBf profile Tage Write ==============================\n"
		   "Profile index = %d\n\n",
		   profileIdx));

	/*
	    Bit 63:62       ng[1:0]
	         61:60       cw[1:0]
	         59:58       nrow[1:0]
	         57:56       ncol[1:0]
	         1 :  0       LM[1:0]
	*/
	if (CMDInIdx & 1)
	{
		value32  = (LONG)MatrixForm[0] << 30;
		value32 |= (LONG)MatrixForm[1] << 28;
		value32 |= (LONG)MatrixForm[2] << 26;
		value32 |= (LONG)MatrixForm[3] << 24;
		readValue32[1] &= 0x00FFFFFF;
		readValue32[1] |= value32;

		readValue32[0] &= ~(0x3);
		readValue32[0] |= MatrixForm[4];

		DBGPRINT(RT_DEBUG_OFF, (
			"ng[1:0]=%d, cw[1:0]=%d, nrow[1:0]=%d, ncol[1:0]=%d, LM[1:0]=%d\n\n",
			   MatrixForm[0], MatrixForm[1], MatrixForm[2],
			   MatrixForm[3], MatrixForm[4]));
	}

	/*
	    Bit 55 : 8       Mac Adress [47:0]
	*/
	if (CMDInIdx & 2)
	{
		value32  = (LONG)macAddr[0] << 16;
		value32 |= (LONG)macAddr[1] << 8;
		value32 |= (LONG)macAddr[2] << 0;
		readValue32[1] &= 0xFF000000;
		readValue32[1] |= value32;

		value32 = (LONG)macAddr[3] << 24;
		value32 |= (LONG)macAddr[4] << 16;
		value32 |= (LONG)macAddr[5] << 8;
		readValue32[0] &= 0x000000FF;
		readValue32[0] |= value32;

		DBGPRINT(RT_DEBUG_OFF, ("Destimation MAC Address=%x:%x:%x:%x:%x:%x\n\n",
			   macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]));
	}

	/*
	    Bit 63 : 56       Tx1_scale_2ss[7:0]
	         55 : 48       Tx0_scale_2ss[7:0]
	         47 : 40       Tx1_scale_1ss[7:0]
	         36 : 32       Tx0_scale_2ss[7:0]
	*/
	if (CMDInIdx & 4)
	{
		value32  = (LONG)TxScale[0] << 24;
		value32 |= (LONG)TxScale[1] << 16;
		value32 |= (LONG)TxScale[2] << 8;
		value32 |= (LONG)TxScale[3] << 0;
		readValue32[3] = value32;

		DBGPRINT(RT_DEBUG_OFF, (
		"Tx1_scal_2ss=0x%x, Tx0_scale_2ss=0x%x, Tx1_scal_1ss=0x%x, Tx0_scale_1ss=0x%x\n\n",
			   TxScale[0], TxScale[1], TxScale[2], TxScale[3]));
	}

	/*
	    Bit 31 : 24       STS1_SNR[7:0]
	         23 : 16       STS0_SNR[7:0]
	*/
	if (CMDInIdx & 8)
	{
		value32  = (LONG)StsSnr[0] << 24;
		value32 |= (LONG)StsSnr[1] << 16;
		readValue32[2] &= 0x0000FFFF;
		readValue32[2] |= value32;

		DBGPRINT(RT_DEBUG_OFF, ("STS1_SNR=0x%x, STS0_SNR=0x%x\n", StsSnr[0], StsSnr[1]));
	}

	/*
	    Bit 15 : 0       timeout[15:0]
	*/
	if (CMDInIdx & 16)
	{
		readValue32[2] &= 0xFFFF0000;
		readValue32[2] |= (ULONG)PFMU_TimeOut;

		DBGPRINT(RT_DEBUG_OFF, ("timeout[15:0]=0x%x\n", PFMU_TimeOut));
	}

	DBGPRINT(RT_DEBUG_OFF, ("PFMU_11 = 0x%x, PFMU_R12=0x%x, PFMU_R13=0x%x, PFMU_R14=0x%x\n\n",
		   readValue32[0], readValue32[1], readValue32[2], readValue32[3]));

	/*
			Bit 15 : 0		 timeout[15:0]
	*/
	if (CMDInIdx & 32)
	{
		readValue32[4] &= 0xFFFFFE00;
		readValue32[4] |= FlgStatus[0] << 8;
		readValue32[4] |= FlgStatus[1] << 7;
		readValue32[4] |= FlgStatus[2] << 6;
		readValue32[4] |= FlgStatus[3] << 5;

		DBGPRINT(RT_DEBUG_OFF, ("LD=%d, EO=%d, IO=%d, I/E=%d\n"
			   "===================================================================================\n",
			   FlgStatus[0], FlgStatus[1], FlgStatus[2], FlgStatus[3]));
	}


	// Read PFMU_R10 (0x2f28) first
	RTMP_IO_READ32(pAd, PFMU_R10, &value32);
	value32 &= (~0x3C00);
	// Wite PFMU_R10 to trigger read command
	RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|value32));
	// Write PFMU_R11 ~ R15
	RTMP_IO_WRITE32(pAd, PFMU_R15, readValue32[4]);
	RTMP_IO_WRITE32(pAd, PFMU_R14, readValue32[3]);
	RTMP_IO_WRITE32(pAd, PFMU_R13, readValue32[2]);
	RTMP_IO_WRITE32(pAd, PFMU_R12, readValue32[1]);
	RTMP_IO_WRITE32(pAd, PFMU_R11, readValue32[0]);

	CMDInIdx = 0; // clear tag indicator

	return TRUE;
}



INT Set_TxBfProfileDataRead(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR   profileIdx, subcarrierIdx;
	UCHAR   Input[2];
	CHAR	*value;
	UINT    value32, readValue32[5];
	INT 	i;

	DBGPRINT(RT_DEBUG_OFF, ("arg length=%d\n", strlen(arg)));

	/* Profile Select : Subcarrier Select */
	if(strlen(arg) != 5)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &Input[i++], 1);
	}

	profileIdx    = Input[0];
	subcarrierIdx = Input[1];

	// Read PFMU_R10 (0x2f28) first
	RTMP_IO_READ32(pAd, PFMU_R10, &value32);
	// Wite PFMU_R10 to trigger read command
	RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|subcarrierIdx));
	// Read PFMU_R19 ~ R23
	RTMP_IO_READ32(pAd, PFMU_R19, &readValue32[0]);
	RTMP_IO_READ32(pAd, PFMU_R20, &readValue32[1]);
	RTMP_IO_READ32(pAd, PFMU_R21, &readValue32[2]);
	RTMP_IO_READ32(pAd, PFMU_R22, &readValue32[3]);
	RTMP_IO_READ32(pAd, PFMU_R23, &readValue32[4]);

	psi21 = (readValue32[0] >> 16) & 0x00FF;
	phill = (readValue32[0] >> 0)  & 0x00FF;

	DBGPRINT(RT_DEBUG_OFF, (
		"============================= TxBf profile Data Info ==============================\n"
		   "Profile index = %d,    subcarrierIdx = %d\n\n"
		   "PFMU_19 = 0x%x, PFMU_R20=0x%x, PFMU_R21=0x%x, PFMU_R22=0x%x\n\n"
		   "psi21 = 0x%x\n\n"
		   "phill = 0x%x\n\n"
		   "===================================================================================\n",
		   profileIdx, subcarrierIdx,
		   readValue32[0], readValue32[1], readValue32[2], readValue32[3],
		   psi21, phill));

	return TRUE;
}



INT Set_TxBfProfileDataWrite(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR   profileIdx, subcarrierIdx;
	UCHAR   Input[4];
	CHAR	*value;
	UINT    value32, readValue32[5];
	INT 	i;

	/* Profile Select : Subcarrier Select */
	if(strlen(arg) != 11)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &Input[i++], 1);
	}

	profileIdx    = Input[0];
	subcarrierIdx = Input[1];

	psi21 = Input[2];
	phill = Input[3];

	// Read PFMU_R10 (0x2f28) first
	RTMP_IO_READ32(pAd, PFMU_R10, &value32);
	// Wite PFMU_R10 to trigger read command
	RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|subcarrierIdx));
	// Read PFMU_R19 ~ R23
	RTMP_IO_READ32(pAd, PFMU_R19, &readValue32[0]);
	RTMP_IO_READ32(pAd, PFMU_R20, &readValue32[1]);
	RTMP_IO_READ32(pAd, PFMU_R21, &readValue32[2]);
	RTMP_IO_READ32(pAd, PFMU_R22, &readValue32[3]);
	RTMP_IO_READ32(pAd, PFMU_R23, &readValue32[4]);

	readValue32[0] = (psi21 << 16) | phill;

	// Read PFMU_R10 (0x2f28) first
	RTMP_IO_READ32(pAd, PFMU_R10, &value32);
	// Wite PFMU_R10 to trigger read command
	RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|subcarrierIdx));
	// Wite PFMU_R19 ~ R23
	RTMP_IO_WRITE32(pAd, PFMU_R23, readValue32[4]);
	RTMP_IO_WRITE32(pAd, PFMU_R22, readValue32[3]);
	RTMP_IO_WRITE32(pAd, PFMU_R21, readValue32[2]);
	RTMP_IO_WRITE32(pAd, PFMU_R20, readValue32[1]);
	RTMP_IO_WRITE32(pAd, PFMU_R19, readValue32[0]);

	dCMDInIdx = 0; // clear profile data write indicator

	return TRUE;
}


static SC_TABLE_ENTRY impSCTable[3] = { {224, 255, 1, 31}, {198, 254, 2, 58}, {134, 254, 2, 122} };
static SC_TABLE_ENTRY expSCTable[3] = { {224, 255, 1, 31}, {198, 254, 2, 58}, {134, 254, 2, 122} };


INT Set_TxBfProfileDataWriteAll(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR   profileIdx;
	USHORT  scIndex;
	UCHAR   TxBfType, BW, psi21_I, phill_I;
	CHAR	*value;
	UINT    value32, readValue32[5];
	SC_TABLE_ENTRY *pTab;
	UCHAR   Input[3];
	INT 	i;


	/* Profile Select : Subcarrier Select */
	if(strlen(arg) != 8)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &Input[i++], 1);
	}

	TxBfType	  = Input[0];
	profileIdx    = Input[1];
	BW            = Input[2];

	psi21_I       = 0x00;
	phill_I       = 0xFF;

	if (TxBfType == 0) // ITxBf
		pTab = &impSCTable[BW];
	else
		pTab = &expSCTable[BW];


	/* Negative subcarriers */
	for (scIndex = pTab->lwb1; scIndex <= pTab->upb1; scIndex++)
	{
		// Read PFMU_R10 (0x2f28) first
		RTMP_IO_READ32(pAd, PFMU_R10, &value32);
		// Wite PFMU_R10 to trigger read command
		RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|scIndex));
		// Read PFMU_R19 ~ R23
		RTMP_IO_READ32(pAd, PFMU_R19, &readValue32[0]);
		RTMP_IO_READ32(pAd, PFMU_R20, &readValue32[1]);
		RTMP_IO_READ32(pAd, PFMU_R21, &readValue32[2]);
		RTMP_IO_READ32(pAd, PFMU_R22, &readValue32[3]);
		RTMP_IO_READ32(pAd, PFMU_R23, &readValue32[4]);

		readValue32[0] = (psi21_I << 16) | phill_I;

		// Read PFMU_R10 (0x2f28) first
		RTMP_IO_READ32(pAd, PFMU_R10, &value32);
		// Wite PFMU_R10 to trigger read command
		RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|scIndex));
		// Wite PFMU_R19 ~ R23
		RTMP_IO_WRITE32(pAd, PFMU_R23, readValue32[4]);
		RTMP_IO_WRITE32(pAd, PFMU_R22, readValue32[3]);
		RTMP_IO_WRITE32(pAd, PFMU_R21, readValue32[2]);
		RTMP_IO_WRITE32(pAd, PFMU_R20, readValue32[1]);
		RTMP_IO_WRITE32(pAd, PFMU_R19, readValue32[0]);

	}

	/* Positive subcarriers */
	for (scIndex = pTab->lwb2; scIndex <= pTab->upb2; scIndex++)
	{
		// Read PFMU_R10 (0x2f28) first
		RTMP_IO_READ32(pAd, PFMU_R10, &value32);
		// Wite PFMU_R10 to trigger read command
		RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|scIndex));
		// Read PFMU_R19 ~ R23
		RTMP_IO_READ32(pAd, PFMU_R19, &readValue32[0]);
		RTMP_IO_READ32(pAd, PFMU_R20, &readValue32[1]);
		RTMP_IO_READ32(pAd, PFMU_R21, &readValue32[2]);
		RTMP_IO_READ32(pAd, PFMU_R22, &readValue32[3]);
		RTMP_IO_READ32(pAd, PFMU_R23, &readValue32[4]);

		readValue32[0] = (psi21_I << 16) | phill_I;

		// Read PFMU_R10 (0x2f28) first
		RTMP_IO_READ32(pAd, PFMU_R10, &value32);
		// Wite PFMU_R10 to trigger read command
		RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|scIndex));
		// Wite PFMU_R19 ~ R23
		RTMP_IO_WRITE32(pAd, PFMU_R23, readValue32[4]);
		RTMP_IO_WRITE32(pAd, PFMU_R22, readValue32[3]);
		RTMP_IO_WRITE32(pAd, PFMU_R21, readValue32[2]);
		RTMP_IO_WRITE32(pAd, PFMU_R20, readValue32[1]);
		RTMP_IO_WRITE32(pAd, PFMU_R19, readValue32[0]);
	}

	return TRUE;
}


INT Set_TxBfProfileDataReadAll(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR   profileIdx;
	UCHAR   TxBfType, BW, psi21_I, phill_I;
	CHAR	*value;
	UINT    value32, readValue32[5];
	UCHAR   Input[3];
	SC_TABLE_ENTRY *pTab;
	INT 	i, scIndex, idx;



	/* Profile Select : Subcarrier Select */
	if(strlen(arg) != 8)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &Input[i++], 1);
	}

	TxBfType	  = Input[0];
	profileIdx    = Input[1];
	BW            = Input[2];

	if (TxBfType == 0) // ITxBf
		pTab = &impSCTable[BW];
	else
		pTab = &expSCTable[BW];


	/* Negative subcarriers */
	idx = 0;
	for (scIndex = pTab->lwb1; scIndex <= pTab->upb1; scIndex++)
	{
		// Read PFMU_R10 (0x2f28) first
		RTMP_IO_READ32(pAd, PFMU_R10, &value32);
		// Wite PFMU_R10 to trigger read command
		RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|scIndex));
		// Read PFMU_R19 ~ R23
		RTMP_IO_READ32(pAd, PFMU_R19, &readValue32[0]);
		RTMP_IO_READ32(pAd, PFMU_R20, &readValue32[1]);
		RTMP_IO_READ32(pAd, PFMU_R21, &readValue32[2]);
		RTMP_IO_READ32(pAd, PFMU_R22, &readValue32[3]);
		RTMP_IO_READ32(pAd, PFMU_R23, &readValue32[4]);

		phill_I = readValue32[0] & 0x0FF;
		psi21_I = (readValue32[0] >> 16) & 0x0FF;

		if ((idx & 0x7) == 0)
		{
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
			DBGPRINT(RT_DEBUG_OFF, ("(%x, %x) ", psi21_I, phill_I));
		}
		else
			DBGPRINT(RT_DEBUG_OFF, ("(%x, %x) ", psi21_I, phill_I));

		idx++;
	}

	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	/* Positive subcarriers */
	idx = 0;
	for (scIndex = pTab->lwb2; scIndex <= pTab->upb2; scIndex++)
	{
		// Read PFMU_R10 (0x2f28) first
		RTMP_IO_READ32(pAd, PFMU_R10, &value32);
		// Wite PFMU_R10 to trigger read command
		RTMP_IO_WRITE32(pAd, PFMU_R10, ((profileIdx << 10)|scIndex));
		// Read PFMU_R19 ~ R23
		RTMP_IO_READ32(pAd, PFMU_R19, &readValue32[0]);
		RTMP_IO_READ32(pAd, PFMU_R20, &readValue32[1]);
		RTMP_IO_READ32(pAd, PFMU_R21, &readValue32[2]);
		RTMP_IO_READ32(pAd, PFMU_R22, &readValue32[3]);
		RTMP_IO_READ32(pAd, PFMU_R23, &readValue32[4]);

		phill_I = readValue32[0] & 0x0FF;
		psi21_I = (readValue32[0] >> 16) & 0x0FF;

		if ((idx & 0x7) == 0)
		{
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
			DBGPRINT(RT_DEBUG_OFF, ("(%x, %x) ", psi21_I, phill_I));
		}
		else
			DBGPRINT(RT_DEBUG_OFF, ("(%x, %x) ", psi21_I, phill_I);

		idx++;
	}

	return TRUE;
}



INT Set_TxBfProfileTag_TimeOut(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PFMU_TimeOut = simple_strtol(arg, 0, 10);

	CMDInIdx |= 16;

	return TRUE;
}


INT Set_TxBfProfileTag_Matrix(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CHAR	*value;
	INT 	i;

    /* ng[1:0], cw[1:0], nrow[1:0], ncol[1:0] */
	if(strlen(arg) != 14)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &MatrixForm[i++], 1);
	}

	CMDInIdx |= 1;

	DBGPRINT(RT_DEBUG_OFF,
		("ng[1:0]=%x\n cw[1:0]=%x\n nrow[1:0]=%x\n nol[1:0]=%x\n LM[1:0]=%x\n",
		MatrixForm[0], MatrixForm[1], MatrixForm[2], MatrixForm[3], MatrixForm[4]));

	return TRUE;
}


INT Set_TxBfProfileTag_SNR(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CHAR	*value;
	INT 	i;

	/* STS1_SNR: STS0_SNR */
	if(strlen(arg) != 5)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &StsSnr[i++], 1);
	}

	CMDInIdx |= 8;

	DBGPRINT(RT_DEBUG_OFF, ("STS1_SNR=0x%x\n STS1_SNR=0x%x\n",
		   StsSnr[0], StsSnr[1]));

	return TRUE;
}


INT Set_TxBfProfileTag_TxScale(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CHAR	*value;
	INT 	i;

	/* Tx1_scale_2ss: Tx0_scale_2ss: Tx1_scale_1ss: Tx0_scale_1ss*/
	if(strlen(arg) != 11)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &TxScale[i++], 1);
	}

	CMDInIdx |= 4;

	DBGPRINT(RT_DEBUG_OFF, (
		"Tx1_scale_2ss=0x%x\n Tx0_scale_2ss=0x%x\n Tx1_scale_1ss=0x%x\n Tx0_scale_1ss=0x%x\n",
		   TxScale[0], TxScale[1], TxScale[2], TxScale[3]));

	return TRUE;
}



INT Set_TxBfProfileTag_MAC(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CHAR	*value;
	INT		i;

	/* Mac address acceptable format 01:02:03:04:05:06 length 17 */
	if(strlen(arg) != 17)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &macAddr[i++], 1);
	}

	CMDInIdx |= 2;

	DBGPRINT(RT_DEBUG_OFF, (
		"Set_TxBfProfileTage_MAC\n DS MAC=0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n",
		   macAddr[0],
		   macAddr[1],
		   macAddr[2],
		   macAddr[3],
		   macAddr[4],
		   macAddr[5]));

    return TRUE;
}


INT Set_TxBfProfileTag_Flg(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CHAR	*value;
	INT 	i;

    /* LD, EO, IO, I/E */
	if(strlen(arg) != 11)
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
	{
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) )
			return FALSE;  /*Invalid*/

		AtoH(value, &FlgStatus[i++], 1);
	}

	CMDInIdx |= 32;

	DBGPRINT(RT_DEBUG_OFF, ("LD=%d\n EO=%d\n IO=%d\n I/E=%d\n",
		   FlgStatus[0], FlgStatus[1], FlgStatus[2], FlgStatus[3]));

	return TRUE;
}
#endif  //TXBF_SUPPORT
#endif  //MT76x2


#ifdef DOT11_N_SUPPORT
void assoc_ht_info_debugshow(
	IN PRTMP_ADAPTER pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN UCHAR ht_cap_len,
	IN HT_CAPABILITY_IE *pHTCapability)
{
	HT_CAP_INFO			*pHTCap;
	HT_CAP_PARM		*pHTCapParm;
	EXT_HT_CAP_INFO		*pExtHT;
#ifdef TXBF_SUPPORT
	HT_BF_CAP			*pBFCap;
#endif /* TXBF_SUPPORT */


	if (pHTCapability && (ht_cap_len > 0))
	{
		pHTCap = &pHTCapability->HtCapInfo;
		pHTCapParm = &pHTCapability->HtCapParm;
		pExtHT = &pHTCapability->ExtHtCapInfo;
#ifdef TXBF_SUPPORT
		pBFCap = &pHTCapability->TxBFCap;
#endif /* TXBF_SUPPORT */

		DBGPRINT(RT_DEBUG_TRACE, ("Peer - 11n HT Info\n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\tHT Cap Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t HT_RX_LDPC(%d), BW(%d), MIMOPS(%d), GF(%d), ShortGI_20(%d), ShortGI_40(%d)\n",
			pHTCap->ht_rx_ldpc, pHTCap->ChannelWidth, pHTCap->MimoPs, pHTCap->GF,
			pHTCap->ShortGIfor20, pHTCap->ShortGIfor40));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t TxSTBC(%d), RxSTBC(%d), DelayedBA(%d), A-MSDU(%d), CCK_40(%d)\n",
			pHTCap->TxSTBC, pHTCap->RxSTBC, pHTCap->DelayedBA, pHTCap->AMsduSize, pHTCap->CCKmodein40));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t PSMP(%d), Forty_Mhz_Intolerant(%d), L-SIG(%d)\n",
			pHTCap->PSMP, pHTCap->Forty_Mhz_Intolerant, pHTCap->LSIGTxopProSup));

		DBGPRINT(RT_DEBUG_TRACE, ("\tHT Parm Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t MaxRx A-MPDU Factor(%d), MPDU Density(%d)\n",
			pHTCapParm->MaxRAmpduFactor, pHTCapParm->MpduDensity));

		DBGPRINT(RT_DEBUG_TRACE, ("\tHT MCS set: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t RxMCS(%02x %02x %02x %02x %02x) MaxRxMbps(%d) TxMCSSetDef(%02x)\n",
			pHTCapability->MCSSet[0], pHTCapability->MCSSet[1], pHTCapability->MCSSet[2],
			pHTCapability->MCSSet[3], pHTCapability->MCSSet[4],
			(pHTCapability->MCSSet[11]<<8) + pHTCapability->MCSSet[10],
			pHTCapability->MCSSet[12]));

		DBGPRINT(RT_DEBUG_TRACE, ("\tExt HT Cap Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t PCO(%d), TransTime(%d), MCSFeedback(%d), +HTC(%d), RDG(%d)\n",
			pExtHT->Pco, pExtHT->TranTime, pExtHT->MCSFeedback, pExtHT->PlusHTC, pExtHT->RDGSupport));

#ifdef TXBF_SUPPORT
        	DBGPRINT(RT_DEBUG_TRACE, ("\tTX BF Cap: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t ImpRxCap(%d), RXStagSnd(%d), TXStagSnd(%d), RxNDP(%d), TxNDP(%d) ImpTxCap(%d)\n",
			pBFCap->TxBFRecCapable, pBFCap->RxSoundCapable, pBFCap->TxSoundCapable,
			pBFCap->RxNDPCapable, pBFCap->TxNDPCapable, pBFCap->ImpTxBFCapable));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t Calibration(%d), ExpCSICapable(%d), ExpComSteerCapable(%d), ExpCSIFbk(%d), ExpNoComBF(%d) ExpComBF(%d)\n",
			pBFCap->Calibration, pBFCap->ExpCSICapable, pBFCap->ExpComSteerCapable,
			pBFCap->ExpCSIFbk, pBFCap->ExpNoComBF, pBFCap->ExpComBF));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\t MinGrouping(%d), CSIBFAntSup(%d), NoComSteerBFAntSup(%d), ComSteerBFAntSup(%d), CSIRowBFSup(%d) ChanEstimation(%d)\n",
			pBFCap->MinGrouping, pBFCap->CSIBFAntSup, pBFCap->NoComSteerBFAntSup,
			pBFCap->ComSteerBFAntSup, pBFCap->CSIRowBFSup, pBFCap->ChanEstimation));
#endif /* TXBF_SUPPORT */

		DBGPRINT(RT_DEBUG_TRACE, ("\nPeer - MODE=%d, BW=%d, MCS=%d, ShortGI=%d, MaxRxFactor=%d, MpduDensity=%d, MIMOPS=%d, AMSDU=%d\n",
			pEntry->HTPhyMode.field.MODE, pEntry->HTPhyMode.field.BW,
			pEntry->HTPhyMode.field.MCS, pEntry->HTPhyMode.field.ShortGI,
			pEntry->MaxRAmpduFactor, pEntry->MpduDensity,
			pEntry->MmpsMode, pEntry->AMsduSize));

#ifdef DOT11N_DRAFT3
		DBGPRINT(RT_DEBUG_TRACE, ("\tExt Cap Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tBss2040CoexistMgmt=%d\n", pEntry->BSS2040CoexistenceMgmtSupport));
#endif /* DOT11N_DRAFT3 */
	}
}


INT	Set_BurstMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	pAd->CommonCfg.bRalinkBurstMode = ((Value == 1) ? TRUE : FALSE);

	AsicSetRalinkBurstMode(pAd, pAd->CommonCfg.bRalinkBurstMode);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_BurstMode_Proc ::%s\n", 
				(pAd->CommonCfg.bRalinkBurstMode == TRUE) ? "enabled" : "disabled"));

	return TRUE;
}
#endif /* DOT11_N_SUPPORT */


#ifdef DOT11_VHT_AC
VOID assoc_vht_info_debugshow(
	IN RTMP_ADAPTER *pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN VHT_CAP_IE *vht_cap,
	IN VHT_OP_IE *vht_op)
{
	VHT_CAP_INFO *cap_info;
	VHT_MCS_SET *mcs_set;
	VHT_OP_INFO *op_info;
	VHT_MCS_MAP *mcs_map;


	if (!WMODE_CAP_AC(pAd->CommonCfg.PhyMode))
		return;

	DBGPRINT(RT_DEBUG_TRACE, ("Peer - 11AC VHT Info\n"));
	if (vht_cap)
	{
		cap_info = &vht_cap->vht_cap;
		mcs_set = &vht_cap->mcs_set;

		hex_dump("peer vht_cap raw data", (UCHAR *)cap_info, sizeof(VHT_CAP_INFO));
		hex_dump("peer vht_mcs raw data", (UCHAR *)mcs_set, sizeof(VHT_MCS_SET));

		DBGPRINT(RT_DEBUG_TRACE, ("\tVHT Cap Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tMaxMpduLen(%d), BW(%d), SGI_80M(%d), RxLDPC(%d), TxSTBC(%d), RxSTBC(%d), +HTC-VHT(%d)\n",
				cap_info->max_mpdu_len, cap_info->ch_width, cap_info->sgi_80M, cap_info->rx_ldpc, cap_info->tx_stbc,
				cap_info->rx_stbc, cap_info->htc_vht_cap));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tMaxAmpduExp(%d), VhtLinkAdapt(%d), RxAntConsist(%d), TxAntConsist(%d)\n",
				cap_info->max_ampdu_exp, cap_info->vht_link_adapt, cap_info->rx_ant_consistency, cap_info->tx_ant_consistency));
		mcs_map = &mcs_set->rx_mcs_map;
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tRxMcsSet: HighRate(%d), RxMCSMap(%d,%d,%d,%d,%d,%d,%d)\n",
			mcs_set->rx_high_rate, mcs_map->mcs_ss1, mcs_map->mcs_ss2, mcs_map->mcs_ss3,
			mcs_map->mcs_ss4, mcs_map->mcs_ss5, mcs_map->mcs_ss6, mcs_map->mcs_ss7));
		mcs_map = &mcs_set->tx_mcs_map;
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tTxMcsSet: HighRate(%d), TxMcsMap(%d,%d,%d,%d,%d,%d,%d)\n",
			mcs_set->tx_high_rate, mcs_map->mcs_ss1, mcs_map->mcs_ss2, mcs_map->mcs_ss3,
			mcs_map->mcs_ss4, mcs_map->mcs_ss5, mcs_map->mcs_ss6, mcs_map->mcs_ss7));
#ifdef VHT_TXBF_SUPPORT
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tETxBfCap: Bfer(%d), Bfee(%d), SndDim(%d)\n",
			cap_info->bfer_cap_su, cap_info->bfee_cap_su, cap_info->num_snd_dimension));
#endif
	}

	if (vht_op)
	{
		op_info = &vht_op->vht_op_info;
		mcs_map = &vht_op->basic_mcs_set;		
		DBGPRINT(RT_DEBUG_TRACE, ("\tVHT OP Info: \n"));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tChannel Width(%d), CenteralFreq1(%d), CenteralFreq2(%d)\n",
			op_info->ch_width, op_info->center_freq_1, op_info->center_freq_2));
		DBGPRINT(RT_DEBUG_TRACE, ("\t\tBasicMCSSet(SS1:%d, SS2:%d, SS3:%d, SS4:%d, SS5:%d, SS6:%d, SS7:%d)\n",
			mcs_map->mcs_ss1, mcs_map->mcs_ss2, mcs_map->mcs_ss3,
			mcs_map->mcs_ss4, mcs_map->mcs_ss5, mcs_map->mcs_ss6,
			mcs_map->mcs_ss7));
	}

	DBGPRINT(RT_DEBUG_TRACE, ("\n"));

}
#endif /* DOT11_VHT_AC */


INT Set_RateAdaptInterval(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 ra_time, ra_qtime;
	RTMP_STRING *token;
	char sep = ':';
	ULONG irqFlags = 0;

/*
	The ra_interval inupt string format should be d:d, in units of ms. 
		=>The first decimal number indicates the rate adaptation checking period, 
		=>The second decimal number indicates the rate adaptation quick response checking period.
*/
	DBGPRINT(RT_DEBUG_TRACE,("%s():%s\n", __FUNCTION__, arg));
	
	token = strchr(arg, sep);
	if (token != NULL)
	{
		*token = '\0';

		if (strlen(arg) && strlen(token+1))
		{
			ra_time = simple_strtol(arg, 0, 10);
			ra_qtime = simple_strtol(token+1, 0, 10);
			DBGPRINT(RT_DEBUG_OFF, ("%s():Set RateAdaptation TimeInterval as(%d:%d) ms\n", 
						__FUNCTION__, ra_time, ra_qtime));

			RTMP_IRQ_LOCK(&pAd->irq_lock, irqFlags);
			pAd->ra_interval = ra_time;
			pAd->ra_fast_interval = ra_qtime;
#ifdef CONFIG_AP_SUPPORT
			if (pAd->ApCfg.ApQuickResponeForRateUpTimerRunning == TRUE)
			{
				BOOLEAN Cancelled;
				
				RTMPCancelTimer(&pAd->ApCfg.ApQuickResponeForRateUpTimer, &Cancelled);	
				pAd->ApCfg.ApQuickResponeForRateUpTimerRunning = FALSE;
			}
#endif /* CONFIG_AP_SUPPORT  */
			RTMP_IRQ_UNLOCK(&pAd->irq_lock, irqFlags);
			return TRUE;
		}
	}
	
	return FALSE;

}

#ifdef CONFIG_SNIFFER_SUPPORT
INT Set_MonitorMode_Proc(
	IN RTMP_ADAPTER	*pAd,
	IN RTMP_STRING *arg)
{
	//UINT32	Value = 0;

	pAd->monitor_ctrl.current_monitor_mode = simple_strtol(arg, 0, 10);
	if(pAd->monitor_ctrl.current_monitor_mode > MONITOR_MODE_FULL || pAd->monitor_ctrl.current_monitor_mode < MONITOR_MODE_OFF)
		pAd->monitor_ctrl.current_monitor_mode = MONITOR_MODE_OFF;
		
	DBGPRINT(RT_DEBUG_ERROR,
			("set Current Monitor Mode = %d , range(%d ~ %d)\n"
			, pAd->monitor_ctrl.current_monitor_mode,MONITOR_MODE_OFF,MONITOR_MODE_FULL));

	switch(pAd->monitor_ctrl.current_monitor_mode)
	{
	case MONITOR_MODE_OFF:			//reset to normal
#ifdef CONFIG_STA_SUPPORT
		pAd->StaCfg.BssType = BSS_INFRA;
#else
		pAd->ApCfg.BssType = BSS_INFRA;
#endif		
		AsicSetRxFilter(pAd);
		
		break;
	case MONITOR_MODE_REGULAR_RX:			//report probe_request only , normal rx filter
#ifdef CONFIG_STA_SUPPORT
		pAd->StaCfg.BssType = BSS_MONITOR;
#else
		pAd->ApCfg.BssType = BSS_MONITOR;
#endif
		
		/* ASIC supporsts sniffer function with replacing RSSI with timestamp. 
		RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value); 
		Value |= (0x80); 
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);		*/
		break;
	
	case MONITOR_MODE_FULL:			//fully report, Enable Rx with promiscuous reception
#ifdef CONFIG_STA_SUPPORT
		pAd->StaCfg.BssType = BSS_MONITOR;
#else
		pAd->ApCfg.BssType = BSS_MONITOR;
#endif
		AsicSetRxFilter(pAd);
		
		/*RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value); 
		Value |= (0x80); 
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);	*/	
		break;
	}
	return TRUE;
}
#endif /* CONFIG_SNIFFER_SUPPORT */


INT Set_VcoPeriod_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT ret;
	LONG ret_value;

	ret = kstrtol(arg, 10, &ret_value);
	pAd->chipCap.VcoPeriod = (UINT8)ret_value;

	DBGPRINT(RT_DEBUG_TRACE,
			("VCO Period = %d seconds\n", pAd->chipCap.VcoPeriod));
	return TRUE;
}

#ifdef SINGLE_SKU
INT Set_ModuleTxpower_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT16 Value;

	if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Do NOT accept this command after interface is up.\n"));
		return FALSE;
	}

	Value = (UINT16)simple_strtol(arg, 0, 10);
	pAd->CommonCfg.ModuleTxpower = Value;
	DBGPRINT(RT_DEBUG_TRACE, ("IF Set_ModuleTxpower_Proc::(ModuleTxpower=%d)\n", pAd->CommonCfg.ModuleTxpower));
	return TRUE;
}
#endif /* SINGLE_SKU */


#ifdef CONFIG_FPGA_MODE
#ifdef CONFIG_STA_SUPPORT
INT set_con_bssid_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR mac_addr[MAC_ADDR_LEN] = {0};
	
	NdisMoveMemory(&pAd->CommonCfg.Bssid[0], &mac_addr[0], MAC_ADDR_LEN);
	

	return TRUE;
}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CAPTURE_MODE
INT set_cap_dump(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG seg = simple_strtol(arg, 0, 10);;
	CHAR *buf1, *buf2;
	UINT32 offset = 0;
	
	seg = ((seg > 0 && seg <= 4)  ? seg : 1);
	if (pAd->fpga_ctl.cap_done == TRUE && (pAd->fpga_ctl.cap_buf != NULL)) {
		switch (seg)
		{
			case 1:
				offset = 0;
				break;
			case 2:
				offset = 0x2000;
				break;
			case 3:
				offset = 0x4000;
				break;
			case 4:
				offset = 0x6000;
				break;
		}
		cap_dump(pAd,
					(pAd->fpga_ctl.cap_buf + offset), 
					(pAd->fpga_ctl.cap_buf + 0x8000 + offset),
					0x2000);
	}

	return TRUE;
}


INT set_cap_start(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG cap_start;
	BOOLEAN do_cap;
	BOOLEAN cap_done;	/* 1: capture done, 0: capture not finish yet */


	cap_start = simple_strtol(arg, 0, 10);
	do_cap = cap_start == 1 ? TRUE : FALSE;

	if (!pAd->fpga_ctl.cap_support) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): cap mode is not support yet!\n", __FUNCTION__));
		return FALSE;
	}

	if (do_cap)
	{
		/*
			start to do cap,
			if auto 	=>will triggered depends on trigger condition,
			if manual =>start immediately
		*/
		if (pAd->fpga_ctl.do_cap == FALSE)
			asic_cap_start(pAd);
		else
		{
			DBGPRINT(RT_DEBUG_OFF, ("%s(): alreay in captureing\n", __FUNCTION__));
		}
	}
	else
	{
		
		if (pAd->fpga_ctl.do_cap == TRUE) {
			DBGPRINT(RT_DEBUG_OFF, ("%s(): force stop captureing\n", __FUNCTION__));
			// TODO: force stop capture!
			asic_cap_stop(pAd);
		}
		else
		{

		}
		pAd->fpga_ctl.do_cap = FALSE;
	}
	return TRUE;
}


INT set_cap_trigger_offset(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG trigger_offset;	/* in unit of us */

	trigger_offset = simple_strtol(arg, 0, 10);

	pAd->fpga_ctl.trigger_offset = (UINT32)trigger_offset;
	DBGPRINT(RT_DEBUG_OFF, ("%s():set trigger_offset=%d\n",
				__FUNCTION__, pAd->fpga_ctl.trigger_offset));

	return TRUE;
}



INT set_cap_trigger(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG trigger;	/* 1: manual trigger, 2: auto trigger */


	trigger = simple_strtol(arg, 0, 10);
	pAd->fpga_ctl.cap_trigger = trigger <= 2 ? trigger : 0;
	DBGPRINT(RT_DEBUG_OFF, ("%s():set cap_trigger=%s trigger\n", __FUNCTION__,
						(pAd->fpga_ctl.cap_trigger == 0 ? "Invalid" :
						(pAd->fpga_ctl.cap_trigger == 1 ? "Manual" : "Auto"))));

	return TRUE;
}


INT set_cap_type(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG cap_type;			/* 1: ADC6, 2: ADC8, 3: FEQ */
	
	cap_type = simple_strtol(arg, 0, 10);

	pAd->fpga_ctl.cap_type = cap_type <= 3 ? cap_type : 0;
	DBGPRINT(RT_DEBUG_OFF, ("%s():set cap_type=%s\n",
				__FUNCTION__,
				pAd->fpga_ctl.cap_type == 1 ? "ADC6" :\
				(pAd->fpga_ctl.cap_type == 2  ? "ADC8" : "FEQ")));
	
	return TRUE;
}


INT set_cap_support(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR cap_support;	/* 0: no cap mode; 1: cap mode enable */

	cap_support = simple_strtol(arg, 0, 10);
	pAd->fpga_ctl.cap_support = (cap_support == 1 ?  TRUE : FALSE);

	DBGPRINT(RT_DEBUG_OFF, ("%s():set cap_support=%s\n",
				__FUNCTION__,
				(pAd->fpga_ctl.cap_support == TRUE ? "TRUE" : "FALSE")));

	return TRUE;
}
#endif /* CAPTURE_MODE */


INT set_vco_cal(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR vco_cal;
	INT ret;
	LONG ret_value;

	ret = kstrtol(arg, 10, &ret_value);
	vco_cal = (UCHAR)ret_value;
	pAd->fpga_ctl.vco_cal = vco_cal ? TRUE : FALSE;	
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): vco_cal=%s\n", 
				__FUNCTION__, (pAd->fpga_ctl.vco_cal ? "Enabled" : "Stopped")));

	return TRUE;

}

INT set_tr_stop(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR stop;
	INT ret;
	LONG ret_value;

	ret = kstrtol(arg, 10, &ret_value);
	stop = (UCHAR)ret_value;
	pAd->fpga_ctl.fpga_tr_stop = (stop <= 4 ? stop : 0);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): fpga_tr_stop=0x%x\n", __FUNCTION__, pAd->fpga_ctl.fpga_tr_stop));

	return TRUE;
}


INT set_tx_kickcnt(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->fpga_ctl.tx_kick_cnt = (INT)simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s():tx_kick_cnt=%d\n", __FUNCTION__, pAd->fpga_ctl.tx_kick_cnt));

	return TRUE;
}


INT set_data_phy_mode(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->fpga_ctl.tx_data_phy = (INT)simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): tx_data_phy=%d\n", __FUNCTION__, pAd->fpga_ctl.tx_data_phy));
	
	return TRUE;
}


INT set_data_bw(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->fpga_ctl.tx_data_bw = (UCHAR)simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): tx_data_bw=%d\n", __FUNCTION__, pAd->fpga_ctl.tx_data_bw));

	return TRUE;
}


INT set_data_mcs(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR mcs = (UCHAR)simple_strtol(arg, 0, 10);

	pAd->fpga_ctl.tx_data_mcs = mcs;
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): tx_data_mcs=%d\n", __FUNCTION__, pAd->fpga_ctl.tx_data_mcs));

	return TRUE;
}

INT set_data_ldpc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->fpga_ctl.tx_data_ldpc = (UCHAR)simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): tx_data_ldpc=%d\n", __FUNCTION__, pAd->fpga_ctl.tx_data_ldpc));

	return TRUE;
}

INT set_data_gi(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->fpga_ctl.tx_data_gi = (UCHAR)simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): tx_data_gi=%d\n", __FUNCTION__, pAd->fpga_ctl.tx_data_gi));

	return TRUE;
}


INT set_data_basize(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->fpga_ctl.data_basize = (UCHAR)simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): data_basize=%d\n", __FUNCTION__, pAd->fpga_ctl.data_basize));

	return TRUE;
}


#ifdef MT_MAC
INT set_txs_report_type(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	MT_MAC_TXS_TYPE txs_mode = TXS_NONE;
	
	if (arg && strlen(arg)) {
		if (strstr(arg, "data") != NULL)
			txs_mode |= TXS_DATA;

		if (strstr(arg, "qdata") != NULL)
			txs_mode |= TXS_QDATA;

		if (strstr(arg, "noqdata") != NULL)
			txs_mode |= TXS_NON_QDATA;

		if (strstr(arg, "mgmt") != NULL)
			txs_mode |= TXS_MGMT;
		if (strstr(arg, "bcn") != NULL)
			txs_mode |= TXS_BCN;

		if (strstr(arg, "mgmt_other") != NULL)
			txs_mode |= TXS_MGMT_OTHER;

		if (strstr(arg, "ctrl") != NULL)
			txs_mode |= TXS_CTRL;

		if (strstr(arg, "all") != NULL)
			txs_mode |= TXS_ALL;
	}

	pAd->fpga_ctl.txs_type = txs_mode;
	DBGPRINT(RT_DEBUG_OFF, ("%s(): Set TXS Report type as: 0x%x\n",
				__FUNCTION__, pAd->fpga_ctl.txs_type));
	
	return TRUE;
}


INT set_no_bcn(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG no_bcn;

	no_bcn = simple_strtol(arg, 0, 10);
	pAd->fpga_ctl.no_bcn = (no_bcn ? 1 : 0);
	DBGPRINT(RT_DEBUG_OFF, ("%s(): Set no beacon as:%d\n",
							__FUNCTION__, pAd->fpga_ctl.no_bcn));

	return TRUE;
}
#endif /* MT_MAC */


INT set_fpga_mode(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG fpga_on;
#ifdef CONFIG_AP_SUPPORT
	struct wifi_dev *wdev = &pAd->ApCfg.MBSSID[0].wdev;
#endif /* CONFIG_AP_SUPPORT */

	fpga_on = simple_strtol(arg, 0, 10);

	if (fpga_on & 2)
		pAd->fpga_ctl.tx_data_mcs = 7;

	if (fpga_on & 4)
		pAd->fpga_ctl.tx_data_mcs = (1 << 4) | 7;

#ifdef CONFIG_AP_SUPPORT
	if (fpga_on & 0x6) {
		pAd->fpga_ctl.tx_data_phy = MODE_VHT;
		pAd->fpga_ctl.tx_data_ldpc = 0;
		pAd->fpga_ctl.tx_data_bw = BW_80;
		pAd->fpga_ctl.tx_data_gi = 1;
		pAd->fpga_ctl.data_basize = 31;
		wdev->bAutoTxRateSwitch = FALSE;
	}
	else
	{
		wdev->bAutoTxRateSwitch = TRUE;
	}
#endif /* CONFIG_AP_SUPPORT */

	pAd->fpga_ctl.fpga_on = (UINT8)fpga_on;
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): fpga_on=%d\n", __FUNCTION__, pAd->fpga_ctl.fpga_on));
	DBGPRINT(RT_DEBUG_TRACE, ("\tdata_phy=%d\n", pAd->fpga_ctl.tx_data_phy));
	DBGPRINT(RT_DEBUG_TRACE, ("\tdata_bw=%d\n", pAd->fpga_ctl.tx_data_bw));
	DBGPRINT(RT_DEBUG_TRACE, ("\tdata_mcs=%d\n", pAd->fpga_ctl.tx_data_mcs));
	DBGPRINT(RT_DEBUG_TRACE, ("\tdata_gi=%d\n", pAd->fpga_ctl.tx_data_gi));
	DBGPRINT(RT_DEBUG_TRACE, ("\tdata_basize=%d\n", pAd->fpga_ctl.data_basize));

#ifdef CONFIG_AP_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("\tbAutoTxRateSwitch=%d\n",
				wdev->bAutoTxRateSwitch));
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}
#endif /* CONFIG_FPGA_MODE */


#if defined(WFA_VHT_PF) || defined(MT7603_FPGA) || defined(MT7628_FPGA)
INT set_force_amsdu(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->force_amsdu = (simple_strtol(arg, 0, 10) > 0 ? TRUE : FALSE);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): force_amsdu=%d\n",
				__FUNCTION__, pAd->force_amsdu));
	return TRUE;
}
#endif /* defined(WFA_VHT_PF) || defined(MT7603_FPGA) */


#ifdef WFA_VHT_PF
INT set_vht_nss_mcs_cap(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CHAR *token, sep[2] = {':', '-'};
	UCHAR val[3] = {0}, ss, mcs_l, mcs_h, mcs_cap, status = FALSE;
	INT idx = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("%s():intput string=%s\n", __FUNCTION__, arg));
	ss = mcs_l = mcs_h = 0;

	while (arg)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s():intput string[len=%d]=%s\n", __FUNCTION__, strlen(arg), arg));
		if (idx < 2) {
			token = rtstrchr(arg, sep[idx]);
			if (!token) {
				DBGPRINT(RT_DEBUG_TRACE, ("cannot found token '%c' in string \"%s\"!\n", sep[idx], arg));
				return FALSE;
			}
			*token++ = 0;
		} else
			token = NULL;

		if (strlen(arg)) {
			val[idx] = (UCHAR)simple_strtoul(arg, NULL, 10);
			DBGPRINT(RT_DEBUG_TRACE, ("%s():token string[len=%d]=%s, val[%d]=%d\n",
						__FUNCTION__, strlen(arg), arg, idx, val[idx]));
			idx++;
		}
		arg = token;
		if (idx == 3)
			break;
	}

	if (idx <3)
		return FALSE;

	ss = val[0];
	mcs_l = val[1];
	mcs_h = val[2];
	DBGPRINT(RT_DEBUG_TRACE, ("ss=%d, mcs_l=%d, mcs_h=%d\n", ss, mcs_l, mcs_h));
	if (ss && mcs_h)
	{
		if (ss <= pAd->chipCap.max_nss)
			pAd->CommonCfg.vht_nss_cap = ss;
		else
			pAd->CommonCfg.vht_nss_cap = pAd->chipCap.max_nss;

		switch (mcs_h)
		{
			case 7:
				mcs_cap = VHT_MCS_CAP_7;
				break;
			case 8:
				mcs_cap = VHT_MCS_CAP_8;
				break;
			case 9:
				mcs_cap = VHT_MCS_CAP_9;
				break;
			default:
				mcs_cap = VHT_MCS_CAP_9;
				break;
		}
		
		if (mcs_h <= pAd->chipCap.max_vht_mcs)
			pAd->CommonCfg.vht_mcs_cap = mcs_cap;
		else
			pAd->CommonCfg.vht_mcs_cap = pAd->chipCap.max_vht_mcs;

		DBGPRINT(RT_DEBUG_TRACE, ("%s():ss=%d, mcs_cap=%d, vht_nss_cap=%d, vht_mcs_cap=%d\n",
					__FUNCTION__, ss, mcs_cap,
					pAd->CommonCfg.vht_nss_cap,
					pAd->CommonCfg.vht_mcs_cap));
		status = TRUE;
	}

	return status;
}


INT set_vht_nss_mcs_opt(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{


	DBGPRINT(RT_DEBUG_TRACE, ("%s():intput string=%s\n", __FUNCTION__, arg));

	return Set_HtMcs_Proc(pAd, arg);
}


INT set_vht_opmode_notify_ie(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CHAR *token;
	UINT ss, bw;
	BOOLEAN status = FALSE;

	DBGPRINT(RT_DEBUG_TRACE, ("%s():intput string=%s\n", __FUNCTION__, arg));
	token = rtstrchr(arg, ':');
	if (!token)
		return FALSE;

	*token = 0;
	token++;
	if (strlen(arg) && strlen(token))
	{
		ss = simple_strtoul(arg, NULL, 10);
		bw = simple_strtoul(token, NULL, 10);

		DBGPRINT(RT_DEBUG_TRACE, ("%s():ss=%d, bw=%d\n", __FUNCTION__, ss, bw));
		if (ss > 0 && ss <= 2)
			pAd->vht_pf_op_ss = ss;
		else
			pAd->vht_pf_op_ss = pAd->Antenna.field.RxPath;

		switch (bw) {
			case 20:
				pAd->vht_pf_op_bw = BAND_WIDTH_20;
				break;
			case 40:
				pAd->vht_pf_op_bw = BAND_WIDTH_40;
				break;
			case 80:
			default:
				pAd->vht_pf_op_bw = BAND_WIDTH_80;
				break;
		}
		status = TRUE;
	}

	pAd->force_vht_op_mode = status;

	DBGPRINT(RT_DEBUG_TRACE, ("%s():force_vht_op_mode=%d, vht_pf_op_ss=%d, vht_pf_op_bw=%d\n",
				__FUNCTION__, pAd->force_vht_op_mode, pAd->vht_pf_op_ss, pAd->vht_pf_op_bw));

	return status;
}


INT set_force_operating_mode(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->force_vht_op_mode = (simple_strtol(arg, 0, 10) > 0 ? TRUE : FALSE);

	if (pAd->force_vht_op_mode == TRUE) {
		pAd->vht_pf_op_ss = 1; // 1SS
		pAd->vht_pf_op_bw = BAND_WIDTH_20; // 20M
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): force_operating_mode=%d\n",
				__FUNCTION__, pAd->force_vht_op_mode));
	if (pAd->force_vht_op_mode == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("\tforce_operating_mode as %dSS in 20MHz BW\n",
					pAd->vht_pf_op_ss));
	}

	return TRUE;
}


INT set_force_noack(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->force_noack = (simple_strtol(arg, 0, 10) > 0 ? TRUE : FALSE);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): force_noack=%d\n",
				__FUNCTION__, pAd->force_noack));
	
	return TRUE;
}


INT set_force_vht_sgi(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->vht_force_sgi = (simple_strtol(arg, 0, 10) > 0 ? TRUE : FALSE);
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): vht_force_sgi=%d\n",
				__FUNCTION__, pAd->vht_force_sgi));

	return TRUE;
}


INT set_force_vht_tx_stbc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	pAd->vht_force_tx_stbc = (simple_strtol(arg, 0, 10) > 0 ? TRUE : FALSE);
	if (pAd->CommonCfg.TxStream < 2)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): TxStream=%d is not enough for TxSTBC!\n",
				__FUNCTION__, pAd->CommonCfg.TxStream));
		pAd->vht_force_tx_stbc = 0;
	}
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): vht_force_tx_stbc=%d\n",
				__FUNCTION__, pAd->vht_force_tx_stbc));
	
	return TRUE;
}


INT set_force_ext_cca(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG cca_cfg;
	UINT32 mac_val;

	cca_cfg = (simple_strtol(arg, 0, 10) > 0 ? TRUE : FALSE);
	if (cca_cfg)
		mac_val = 0x04101b3f;
	else
		mac_val = 0x583f;
	RTMP_IO_WRITE32(pAd, TXOP_CTRL_CFG, mac_val);

	return TRUE;
}
#endif /* WFA_VHT_PF */


#ifdef DOT11_N_SUPPORT

#ifdef MT76x2
#define MAX_AGG_CNT	48
#elif defined(RT65xx) || defined(MT7601)
#define MAX_AGG_CNT	32
#elif defined(RT2883) || defined(RT3883)
#define MAX_AGG_CNT	16
#else
#define MAX_AGG_CNT	8
#endif

/* DisplayTxAgg - display Aggregation statistics from MAC */
void DisplayTxAgg (RTMP_ADAPTER *pAd)
{
	ULONG totalCount;
	ULONG aggCnt[MAX_AGG_CNT + 2];
	int i;

	AsicReadAggCnt(pAd, aggCnt, sizeof(aggCnt) / sizeof(ULONG));
	totalCount = aggCnt[0] + aggCnt[1];
	if (totalCount > 0)
		for (i=0; i<MAX_AGG_CNT; i++) {
			DBGPRINT(RT_DEBUG_OFF, ("\t%d MPDU=%ld (%ld%%)\n", i+1, aggCnt[i+2], aggCnt[i+2]*100/totalCount));
		}
	DBGPRINT(RT_DEBUG_OFF, ("====================\n"));

}
#endif /* DOT11_N_SUPPORT */


#ifdef RLT_RF
static INT32 SetRltRF(RTMP_ADAPTER *pAd, RTMP_STRING *Arg)
{
	INT bank_id = 0, rf_id = 0, rv = 0, rf_v;
	UCHAR rf_val = 0;
	
	if (Arg)
	{
		rv = sscanf(Arg, "%d-%d-%x", &(bank_id), &(rf_id), &(rf_v));
		DBGPRINT(RT_DEBUG_TRACE, ("%s():rv = %d, bank_id = %d, rf_id = %d, rf_val = 0x%02x\n", __FUNCTION__, rv, bank_id, rf_id, rf_v));
		rf_val = (UCHAR )rf_v;
		if (rv == 3)
		{
			rlt_rf_write(pAd, (UCHAR)bank_id, (UCHAR)rf_id, (UCHAR)rf_val);
			
			rlt_rf_read(pAd, bank_id, rf_id, &rf_val);
			DBGPRINT(RT_DEBUG_TRACE, ("%s():%d %03d 0x%02X\n", __FUNCTION__, bank_id, rf_id, rf_val));
		}
		else if (rv == 2)
		{
			rlt_rf_read(pAd, bank_id, rf_id, &rf_val);
			DBGPRINT(RT_DEBUG_TRACE, ("%s():%d %03d 0x%02X\n", __FUNCTION__, bank_id, rf_id, rf_val));
		}
	}

	return TRUE;
}
#endif


#ifdef MT76x2
static INT32 SetMT76x2RF(RTMP_ADAPTER *pAd, RTMP_STRING *Arg)
{
	INT rf_idx = 0, offset = 0, rv = 0;
	UINT rf_val = 0;
	
	if (Arg)
	{
		rv = sscanf(Arg, "%d-%x-%x", &(rf_idx), &(offset), &(rf_val));
		DBGPRINT(RT_DEBUG_TRACE, ("%s():rv = %d, rf_idx = %d, offset = 0x%04x, rf_val = 0x%08x\n", __FUNCTION__, rv, rf_idx, offset, rf_val));
		if (rv == 3)
		{
			mt76x2_rf_write(pAd, (UINT8)rf_idx, (UINT16)offset, (UINT32)rf_val);
			rf_val = 0;
			mt76x2_rf_read(pAd, (UINT8)rf_idx, (UINT16)offset, (UINT32*)&rf_val);
			
			DBGPRINT(RT_DEBUG_TRACE, ("%s():%d 0x%04x 0x%04x\n", __FUNCTION__, rf_idx, offset, rf_val));
		}
		else if (rv == 2)
		{
			mt76x2_rf_read(pAd, (UINT8)rf_idx, (UINT16)offset, (UINT32*)&rf_val);
			DBGPRINT(RT_DEBUG_TRACE, ("%s():%d 0x%04x 0x%08x\n", __FUNCTION__, rf_idx, offset, rf_val));
		}
		else if (rv == 1)
		{
			//read all offset in the same rf index
			for (offset = 0; offset <= 0x33c; offset = offset+4)
			{
				mt76x2_rf_read(pAd, (UINT8)rf_idx, (UINT16)offset, (UINT32*)&rf_val);
				DBGPRINT(RT_DEBUG_TRACE, ("%s():%d 0x%04x 0x%08x\n", __FUNCTION__, rf_idx, offset, rf_val));
				rf_val = 0;
			}
			offset = 0xfff;
			mt76x2_rf_read(pAd, (UINT8)rf_idx, (UINT16)offset, (UINT32*)&rf_val);
			DBGPRINT(RT_DEBUG_TRACE, ("%s():%d 0x%04x 0x%08x\n", __FUNCTION__, rf_idx, offset, rf_val));
		}
	}

	return TRUE;
}
#endif


#ifdef MT_MAC
static INT32 SetMTRF(RTMP_ADAPTER *pAd, RTMP_STRING *Arg)
{
	int RFIdx, Rv;
	UINT32 Offset, Value;
	if (Arg)
	{
		Rv = sscanf(Arg, "%d-%x-%x", &(RFIdx), &(Offset), &(Value));
		//DBGPRINT(RT_DEBUG_TRACE, ("RfIdx = %d, Offset = 0x%04x, Value = 0x%08x\n", RFIdx, Offset, Value));
		
		if (Rv == 2)
		{
			Value = 0;
			CmdRFRegAccessRead(pAd, (UINT32)RFIdx, (UINT32)Offset, (UINT32 *)&Value);
			
			DBGPRINT(RT_DEBUG_OFF, ("%s():%d 0x%04x 0x%08x\n", __FUNCTION__, RFIdx, Offset, Value));
		
		}		
		if (Rv == 3)
		{
			CmdRFRegAccessWrite(pAd, (UINT32)RFIdx, (UINT32)Offset, (UINT32)Value);
			Value = 0;
			CmdRFRegAccessRead(pAd, (UINT32)RFIdx, (UINT32)Offset, (UINT32 *)&Value);
			
			DBGPRINT(RT_DEBUG_OFF, ("%s():%d 0x%04x 0x%08x\n", __FUNCTION__, RFIdx, Offset, Value));
		
		}
	}

	return TRUE;
}
#endif


INT32 SetRF(RTMP_ADAPTER *pAd, RTMP_STRING *Arg)
{
	INT32 Ret = 0;

#ifdef RLT_RF
	if (pAd->chipCap.rf_type == RF_RLT)
		Ret = SetRltRF(pAd, Arg);
#endif

#ifdef MT76x2
	if (pAd->chipCap.rf_type == RF_MT76x2)
		Ret = SetMT76x2RF(pAd, Arg);
#endif

#ifdef MT_MAC
	if (pAd->chipCap.rf_type == RF_MT)
		Ret = SetMTRF(pAd, Arg);
#endif

	return Ret;
}


static struct {
	RTMP_STRING *name;
	INT (*show_proc)(RTMP_ADAPTER *pAd, RTMP_STRING *arg, ULONG BufLen);
} *PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC, RTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC[] = {
#ifdef DBG
	{"SSID",					Show_SSID_Proc}, 
	{"WirelessMode",			Show_WirelessMode_Proc},       
	{"TxBurst",					Show_TxBurst_Proc},
	{"TxPreamble",				Show_TxPreamble_Proc},
	{"TxPower",					Show_TxPower_Proc},
	{"Channel",					Show_Channel_Proc},            
	{"BGProtection",			Show_BGProtection_Proc},
	{"RTSThreshold",			Show_RTSThreshold_Proc},       
	{"FragThreshold",			Show_FragThreshold_Proc},      
#ifdef DOT11_N_SUPPORT
	{"HtBw",					Show_HtBw_Proc},
	{"HtMcs",					Show_HtMcs_Proc},
	{"HtGi",					Show_HtGi_Proc},
	{"HtOpMode",				Show_HtOpMode_Proc},
	{"HtExtcha",				Show_HtExtcha_Proc},
	{"HtMpduDensity",			Show_HtMpduDensity_Proc},
	{"HtBaWinSize",		        Show_HtBaWinSize_Proc},
	{"HtRdg",		        	Show_HtRdg_Proc},
	{"HtAmsdu",		        	Show_HtAmsdu_Proc},
	{"HtAutoBa",		        Show_HtAutoBa_Proc},
#endif /* DOT11_N_SUPPORT */
	{"CountryRegion",			Show_CountryRegion_Proc},
	{"CountryRegionABand",		Show_CountryRegionABand_Proc},
	{"CountryCode",				Show_CountryCode_Proc},
#ifdef AGGREGATION_SUPPORT
	{"PktAggregate",			Show_PktAggregate_Proc},       
#endif

	{"WmmCapable",				Show_WmmCapable_Proc},         

	{"IEEE80211H",				Show_IEEE80211H_Proc},
#ifdef CONFIG_STA_SUPPORT	
    {"NetworkType",				Show_NetworkType_Proc},        
#ifdef WSC_STA_SUPPORT
    {"WpsApBand",				Show_WpsPbcBand_Proc},
    {"Manufacturer",			Show_WpsManufacturer_Proc},
    {"ModelName",				Show_WpsModelName_Proc},
    {"DeviceName",				Show_WpsDeviceName_Proc},
    {"ModelNumber",				Show_WpsModelNumber_Proc},
    {"SerialNumber",			Show_WpsSerialNumber_Proc},
#endif /* WSC_STA_SUPPORT */
	{"WPAPSK",					Show_WPAPSK_Proc},
	{"AutoReconnect", 			Show_AutoReconnect_Proc},
#ifdef STA_EASY_CONFIG_SETUP
	{"AutoProvisionEn", 		Show_AutoProvisionEn_Proc},
	{"ChangeMode", 				Show_ChangeMode_Proc},
#endif /* STA_EASY_CONFIG_SETUP */
#endif /* CONFIG_STA_SUPPORT */
	{"AuthMode",				Show_AuthMode_Proc},           
	{"EncrypType",				Show_EncrypType_Proc},         
	{"DefaultKeyID",			Show_DefaultKeyID_Proc},       
	{"Key1",					Show_Key1_Proc},               
	{"Key2",					Show_Key2_Proc},               
	{"Key3",					Show_Key3_Proc},               
	{"Key4",					Show_Key4_Proc},               
	{"PMK",						Show_PMK_Proc},
#ifdef SINGLE_SKU
	{"ModuleTxpower",			Show_ModuleTxpower_Proc},
#endif /* SINGLE_SKU */
#endif /* DBG */
	{"rainfo",					Show_STA_RAInfo_Proc},
	{NULL, NULL}
};


INT RTMPShowCfgValue(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RTMP_STRING *pName,
	IN	RTMP_STRING *pBuf,
	IN	UINT32			MaxLen)
{
	INT	Status = 0;	
	
	for (PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC = RTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC; PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name; PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC++)
	{
		if (!strcmp(pName, PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name)) 
		{						
			if(PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->show_proc(pAd, pBuf, MaxLen))
				Status = -EINVAL;
			break;  /*Exit for loop.*/
		}
	}

	if(PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name == NULL)
	{
		snprintf(pBuf, MaxLen, "\n");
		for (PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC = RTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC; PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name; PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC++)
		{
			if ((strlen(pBuf) + strlen(PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name)) >= MaxLen)
				break;
			snprintf(pBuf, MaxLen, "%s%s\n", pBuf
					, PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name);
		}
	}
	
	return Status;
}


INT show_pwr_info(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	if (pAd->chipOps.show_pwr_info) {
		pAd->chipOps.show_pwr_info(pAd);
	}

	return 0;
}


#ifdef MT76x2
#ifdef CONFIG_CALIBRATION_COLLECTION
INT Show_Cal_Info(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR i = 0;
	for (i=R_CALIBRATION_7662;i<=RXIQC_FD_CALIBRATION_7662;i++)
		dump_calibration_info(pAd, i);
		
	return TRUE;
}
#endif	
#endif /* MT76x2 */
	

INT32 ShowBBPInfo(RTMP_ADAPTER *pAd, RTMP_STRING *Arg)
{
	ShowAllBBP(pAd);

	return TRUE;
}


INT32 ShowRFInfo(RTMP_ADAPTER *pAd, RTMP_STRING *Arg)
{
		
	ShowAllRF(pAd);

	return 0;
}
	
