/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	cmm_asic.c

	Abstract:
	Functions used to communicate with ASIC

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
*/

#include "rt_config.h"
//#include "mac/mac_ral/rtmp_mac.h"

extern const UCHAR wmm_aci_2_hw_ac_queue[18] ;

UINT32 AsicGetCrcErrCnt(RTMP_ADAPTER *pAd)
{
	return 0;
}


UINT32 AsicGetPhyErrCnt(RTMP_ADAPTER *pAd)
{
	return 0;
}


UINT32 AsicGetCCACnt(RTMP_ADAPTER *pAd)
{
	return 0;
}


UINT32 AsicGetChBusyCnt(RTMP_ADAPTER *pAd, UCHAR ch_idx)
{
	UINT32	msdr16;
	RTMP_IO_READ32(pAd, MIB_MSDR16, &msdr16);
	msdr16 &= 0x00ffffff;
	return msdr16;
}


#ifdef CONFIG_STA_SUPPORT
VOID AsicUpdateAutoFallBackTable(RTMP_ADAPTER *pAd, UCHAR *pRateTable)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}
}
#endif /* CONFIG_STA_SUPPORT */


INT AsicSetAutoFallBack(RTMP_ADAPTER *pAd, BOOLEAN enable)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	}
	return FALSE;
}


INT32 AsicAutoFallbackInit(RTMP_ADAPTER *pAd)
{
	UINT32 Value;

	RTMP_IO_READ32(pAd, AGG_ARUCR, &Value);
	Value &= ~RATE1_UP_MPDU_LIMIT_MASK;
	Value |= RATE1_UP_MPDU_LINIT(2);
	Value &= ~RATE2_UP_MPDU_LIMIT_MASK;
	Value |= RATE2_UP_MPDU_LIMIT(2);
	Value &= ~RATE3_UP_MPDU_LIMIT_MASK;
	Value |= RATE3_UP_MPDU_LIMIT(2);
	Value &= ~RATE4_UP_MPDU_LIMIT_MASK;
	Value |= RATE4_UP_MPDU_LIMIT(2);
	Value &= ~RATE5_UP_MPDU_LIMIT_MASK;
	Value |= RATE5_UP_MPDU_LIMIT(1);
	Value &= ~RATE6_UP_MPDU_LIMIT_MASK;
	Value |= RATE6_UP_MPDU_LIMIT(1);
	Value &= ~RATE7_UP_MPDU_LIMIT_MASK;
	Value |= RATE7_UP_MPDU_LIMIT(1);
	Value &= ~RATE8_UP_MPDU_LIMIT_MASK;
	Value |= RATE8_UP_MPDU_LIMIT(1);
	RTMP_IO_WRITE32(pAd, AGG_ARUCR, Value);

	RTMP_IO_READ32(pAd, AGG_ARDCR, &Value);
	Value &= ~RATE1_DOWN_MPDU_LIMIT_MASK;
	Value |= RATE1_DOWN_MPDU_LIMIT(0);
	Value &= ~RATE2_DOWN_MPDU_LIMIT_MASK;
	Value |= RATE2_DOWN_MPDU_LIMIT(0);
	Value &= ~RATE3_DOWN_MPDU_LIMIT_MASK;
	Value |= RATE3_DOWN_MPDU_LIMIT(0);
	Value &= ~RATE4_DOWN_MPDU_LIMIT_MASK;
	Value |= RATE4_DOWN_MPDU_LIMIT(0);
	Value &= ~RATE5_DOWN_MPDU_LIMIT_MASK;
	Value |= RATE5_DOWN_MPDU_LIMIT(0);
	Value &= ~RATE6_DOWN_MPDU_LIMIT_MASK;
	Value |= RATE6_DOWN_MPDU_LIMIT(0);
	Value &= ~RATE7_DOWN_MPDU_LIMIT_MASK;
	Value |= RATE7_DOWN_MPDU_LIMIT(0);
	Value &= ~RATE8_DOWN_MPDU_LIMIT_MASK;
	Value |= RATE8_DOWN_MPDU_LIMIT(0);
	RTMP_IO_WRITE32(pAd, AGG_ARDCR, Value);

	RTMP_IO_READ32(pAd, AGG_ARCR, &Value);
	Value |= INI_RATE1;
	Value &= ~RTS_RATE_DOWN_TH_MASK;
	Value &= ~RATE_DOWN_EXTRA_RATIO_MASK;
	Value |= RATE_DOWN_EXTRA_RATIO(1);
	Value |= RATE_DOWN_EXTRA_RATIO_EN;
	Value &= ~RATE_UP_EXTRA_TH_MASK;
	Value |= RATE_UP_EXTRA_TH(4);
	RTMP_IO_WRITE32(pAd, AGG_ARCR, Value);

	return TRUE;
}


/*
	========================================================================

	Routine Description:
		Set MAC register value according operation mode.
		OperationMode AND bNonGFExist are for MM and GF Proteciton.
		If MM or GF mask is not set, those passing argument doesn't not take effect.

		Operation mode meaning:
		= 0 : Pure HT, no preotection.
		= 0x01; there may be non-HT devices in both the control and extension channel, protection is optional in BSS.
		= 0x10: No Transmission in 40M is protected.
		= 0x11: Transmission in both 40M and 20M shall be protected
		if (bNonGFExist)
			we should choose not to use GF. But still set correct ASIC registers.
	========================================================================
*/
typedef enum _PROT_REG_IDX_{
	REG_IDX_CCK = 0,	/* 0x1364 */
	REG_IDX_OFDM = 1,	/* 0x1368 */
	REG_IDX_MM20 = 2,  /* 0x136c */
	REG_IDX_MM40 = 3, /* 0x1370 */
	REG_IDX_GF20 = 4, /* 0x1374 */
	REG_IDX_GF40 = 5, /* 0x1378 */
}PROT_REG_IDX;


VOID AsicUpdateProtect(
	IN PRTMP_ADAPTER pAd,
	IN USHORT OperationMode,
	IN UCHAR SetMask,
	IN BOOLEAN bDisableBGProtect,
	IN BOOLEAN bNonGFExist)
{
	ASIC_UPDATE_PROTECT	rAsicUpdateProtect = {0};

	rAsicUpdateProtect.OperationMode = OperationMode;
	rAsicUpdateProtect.SetMask = SetMask;
	rAsicUpdateProtect.bDisableBGProtect = bDisableBGProtect;
	rAsicUpdateProtect.bNonGFExist = bNonGFExist;

	//DBGPRINT(RT_DEBUG_OFF, ("%s(%d)(%d, %d, %d, %d)\n", __FUNCTION__, __LINE__, rAsicUpdateProtect.OperationMode, rAsicUpdateProtect.SetMask, rAsicUpdateProtect.bDisableBGProtect, rAsicUpdateProtect.bNonGFExist ));

	RTEnqueueInternalCmd(pAd, CMDTHREAD_PERODIC_CR_ACCESS_ASIC_UPDATE_PROTECT, &rAsicUpdateProtect, sizeof(rAsicUpdateProtect));
}

NTSTATUS MtCmdAsicUpdateProtect(RTMP_ADAPTER *pAd, PCmdQElmt CMDQelmt)
{
	UINT32 Value = 0;
	PASIC_UPDATE_PROTECT	pAsicUpdateProtect = (PASIC_UPDATE_PROTECT)(CMDQelmt->buffer);
	USHORT OperationMode = pAsicUpdateProtect->OperationMode;
	UCHAR SetMask = pAsicUpdateProtect->SetMask;
	BOOLEAN bDisableBGProtect = pAsicUpdateProtect->bDisableBGProtect;
	BOOLEAN bNonGFExist;
	
    bDisableBGProtect = pAd->bDisableBGProtect;

	bNonGFExist = pAsicUpdateProtect->bNonGFExist;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
	{
		DBGPRINT(RT_DEBUG_INFO, ("%s(%d): H/W in PM4, return\n", __FUNCTION__, __LINE__));
		return NDIS_STATUS_SUCCESS;
	}

	//DBGPRINT(RT_DEBUG_OFF, ("%s(%d): --->\n", __FUNCTION__, __LINE__));
	//DBGPRINT(RT_DEBUG_OFF, ("%s(%d)(%d, %d, %d, %d)\n", __FUNCTION__, __LINE__, OperationMode, SetMask, bDisableBGProtect, bNonGFExist ));

    pAd->CommonCfg.RestoreProtection.OperationMode = OperationMode;
    pAd->CommonCfg.RestoreProtection.SetMask = SetMask;
    pAd->CommonCfg.RestoreProtection.bDisableBGProtect = bDisableBGProtect;
    pAd->CommonCfg.RestoreProtection.bNonGFExist = bNonGFExist;

#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
		return NDIS_STATUS_SUCCESS;
#endif /* CONFIG_ATE */

#ifdef DOT11_N_SUPPORT
	if (!(pAd->CommonCfg.bHTProtect) && (OperationMode != 8))
		return NDIS_STATUS_SUCCESS;
#endif /* DOT11_N_SUPPORT */

	if (pAd->chipCap.hif_type == HIF_MT) {
		/* Config ASIC RTS threshold register*/
    	RTMP_IO_READ32(pAd, AGG_PCR1, &Value);
	    Value &= ~RTS_THRESHOLD_MASK;
        Value &= ~RTS_PKT_NUM_THRESHOLD_MASK;

		if ((
#ifdef DOT11_N_SUPPORT
            (pAd->CommonCfg.BACapability.field.AmsduEnable) ||
#endif /* DOT11_N_SUPPORT */
            (pAd->bDisableRtsProtect == TRUE))
            && (pAd->CommonCfg.RtsThreshold == MAX_RTS_THRESHOLD))
        {
            Value |= RTS_THRESHOLD(0xFFFFF);
            Value |= RTS_PKT_NUM_THRESHOLD(0x7F);
        }
        else
        {
            Value |= RTS_THRESHOLD(pAd->CommonCfg.RtsThreshold);
            Value |= RTS_PKT_NUM_THRESHOLD(1);
            Value |= RTS_PKT_NUM_THRESHOLD(1);

        }

		RTMP_IO_WRITE32(pAd, AGG_PCR1, Value);

		/* Handle legacy(B/G) protection*/
	    if (bDisableBGProtect)
    	{
			RTMP_IO_READ32(pAd, AGG_PCR, &Value);
			Value &= ~ERP_PROTECTION_MASK;
            RTMP_IO_WRITE32(pAd, AGG_PCR, Value);
    	    pAd->FlgCtsEnabled = 0; /* CTS-self is not used */
	    }
    	else
	    {
			/* Enable ERP Protection */
			RTMP_IO_READ32(pAd, AGG_PCR, &Value);
			Value &= ~ERP_PROTECTION_MASK;
			Value |= ERP_PROTECTION(0x1f);
			RTMP_IO_WRITE32(pAd, AGG_PCR, Value);

	        pAd->FlgCtsEnabled = 1; /* CTS-self is used */
    	}

		RTMP_IO_READ32(pAd, AGG_PCR, &Value);

		if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE))
		{ /* Single Protection Mode */
			Value |= PROTECTION_MODE;
		}
		else
		{
			Value &= ~PROTECTION_MODE;
		}

#if 0
		if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED))
		{
			Value |= ERP_PROTECTION(0x1f);
		}
		else
		{
			Value &= ~ERP_PROTECTION_MASK;
		}
#endif

		RTMP_IO_WRITE32(pAd, AGG_PCR, Value);


#ifdef DOT11_N_SUPPORT
		/* Decide HT frame protection.*/
		if ((SetMask & ALLN_SETPROTECT) != 0)
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("AsicUpdateProtect===>OperationMode = %d. \n", OperationMode));
#endif /* RELEASE_EXCLUDE */
			switch(OperationMode)
			{
				case 0x0:
					/* NO PROTECT */
					/* 1.All STAs in the BSS are 20/40 MHz HT*/
					/* 2. in ai 20/40MHz BSS*/
					/* 3. all STAs are 20MHz in a 20MHz BSS*/
					/* Pure HT. no protection.*/
					RTMP_IO_READ32(pAd, AGG_PCR, &Value);
					Value &= ~(MM_PROTECTION | GF_PROTECTION | BW40_PROTECTION
									| RIFS_PROTECTION | BW80_PROTECTION | BW160_PROTECTION);

					RTMP_IO_WRITE32(pAd, AGG_PCR, Value);
					break;
 				case 0x1:
					/* This is "HT non-member protection mode." */
					/* If there may be non-HT STAs my BSS*/
					RTMP_IO_READ32(pAd, AGG_PCR, &Value);
					Value &= ~(MM_PROTECTION | GF_PROTECTION | BW40_PROTECTION
									| RIFS_PROTECTION | BW80_PROTECTION | BW160_PROTECTION);

					Value |= (MM_PROTECTION | GF_PROTECTION | BW40_PROTECTION);

					RTMP_IO_WRITE32(pAd, AGG_PCR, Value);
					break;
				case 0x2:
					/* If only HT STAs are in BSS. at least one is 20MHz. Only protect 40MHz packets */
					RTMP_IO_READ32(pAd, AGG_PCR, &Value);
					Value &= ~(MM_PROTECTION | GF_PROTECTION | BW40_PROTECTION
									| RIFS_PROTECTION | BW80_PROTECTION | BW160_PROTECTION);

					Value |= (BW40_PROTECTION);

					RTMP_IO_WRITE32(pAd, AGG_PCR, Value);
					break;

				case 0x3:
					/* HT mixed mode. PROTECT ALL!*/
					/* both 20MHz and 40MHz are protected. Whether use RTS or CTS-to-self depends on the */
					RTMP_IO_READ32(pAd, AGG_PCR, &Value);
					Value &= ~(MM_PROTECTION | GF_PROTECTION | BW40_PROTECTION
									| RIFS_PROTECTION | BW80_PROTECTION | BW160_PROTECTION);

					Value |= (MM_PROTECTION | GF_PROTECTION | BW40_PROTECTION);

					RTMP_IO_WRITE32(pAd, AGG_PCR, Value);
					pAd->CommonCfg.IOTestParm.bRTSLongProtOn = TRUE;
					break;

                case 0x8:
                    // TODO:
                    break;
				default:
					DBGPRINT(RT_DEBUG_ERROR, ("%s: unknown protection mode(%d)\n", __FUNCTION__, OperationMode));
			}
		}
#endif /* DOT11_N_SUPPORT */
	}

	return NDIS_STATUS_SUCCESS;
}


/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicSwitchChannel(RTMP_ADAPTER *pAd, UCHAR Channel, BOOLEAN bScan)
{
	UCHAR bw;

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))
		return;

#ifdef MT_WOW_SUPPORT
	if (pAd->WOW_Cfg.bWoWRunning){
		DBGPRINT(RT_DEBUG_OFF, ("[%s] WoW is running, skip!\n", __func__));
		return;
	}
#endif

#ifdef CONFIG_AP_SUPPORT
#ifdef AP_QLOAD_SUPPORT
	/* clear all statistics count for QBSS Load */
	QBSS_LoadStatusClear(pAd);
#endif /* AP_QLOAD_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	if (pAd->chipOps.ChipSwitchChannel)
		pAd->chipOps.ChipSwitchChannel(pAd, Channel, bScan);
	else
		DBGPRINT(RT_DEBUG_ERROR, ("For this chip, no specified channel switch function!\n"));

#ifdef MT_MAC
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		UINT32 val;

		RTMP_IO_READ32(pAd, RMAC_CHFREQ, &val);
		val = 1;
		RTMP_IO_WRITE32(pAd, RMAC_CHFREQ, val);

	}
#endif /* MT_MAC */

	/* R66 should be set according to Channel and use 20MHz when scanning*/
	if (bScan)
		bw = BW_20;
	else {
		bw = pAd->CommonCfg.BBPCurrentBW;
#ifdef IQ_CAL_SUPPORT
		RTMP_CHIP_IQ_CAL(pAd, Channel);
#endif /* IQ_CAL_SUPPORT */
	}
	RTMPSetAGCInitValue(pAd, bw);
}


/*
	==========================================================================
	Description:
		This function is required for 2421 only, and should not be used during
		site survey. It's only required after NIC decided to stay at a channel
		for a longer period.
		When this function is called, it's always after AsicSwitchChannel().

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicLockChannel(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR Channel)
{
}

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
#ifdef ANT_DIVERSITY_SUPPORT
VOID AsicAntennaSelect(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR Channel)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	}

	return;
}
#endif /* ANT_DIVERSITY_SUPPORT */


VOID AsicResetBBPAgent(RTMP_ADAPTER *pAd)
{
	/* Still need to find why BBP agent keeps busy, but in fact, hardware still function ok. Now clear busy first.	*/
	/* IF chipOps.AsicResetBbpAgent == NULL, run "else" part */
	if (pAd->chipOps.AsicResetBbpAgent != NULL)
		pAd->chipOps.AsicResetBbpAgent(pAd);
}


#ifdef CONFIG_STA_SUPPORT
/*
	==========================================================================
	Description:
		put PHY to sleep here, and set next wakeup timer. PHY doesn't not wakeup
		automatically. Instead, MCU will issue a TwakeUpInterrupt to host after
		the wakeup timer timeout. Driver has to issue a separate command to wake
		PHY up.

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicSleepThenAutoWakeup(
	IN PRTMP_ADAPTER pAd,
	IN USHORT TbttNumToNextWakeUp)
{
	RTMP_STA_SLEEP_THEN_AUTO_WAKEUP(pAd, TbttNumToNextWakeUp);
}

/*
	==========================================================================
	Description:
		AsicForceWakeup() is used whenever Twakeup timer (set via AsicSleepThenAutoWakeup)
		expired.

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	==========================================================================
 */
VOID AsicForceWakeup(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN    bFromTx)
{
    DBGPRINT(RT_DEBUG_INFO, ("--> AsicForceWakeup \n"));
    RTMP_STA_FORCE_WAKEUP(pAd, bFromTx);
}

NTSTATUS SetPSMBitHdlr(IN PRTMP_ADAPTER pAd, IN PCmdQElmt CMDQelmt)
{
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		USHORT *pPsm = (USHORT *)CMDQelmt->buffer;
		DBGPRINT(RT_DEBUG_TRACE, ("%s(line:%d)\n", __FUNCTION__, __LINE__));
		MlmeSetPsmBit(pAd, *pPsm);
	}

	return NDIS_STATUS_SUCCESS;
}


NTSTATUS ForceWakeUpHdlr(IN PRTMP_ADAPTER pAd, IN PCmdQElmt CMDQelmt)
{
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s:: --->\n", __FUNCTION__));
		AsicForceWakeup(pAd, TRUE);
		DBGPRINT(RT_DEBUG_TRACE, ("%s:: <---\n", __FUNCTION__));
	}

	return NDIS_STATUS_SUCCESS;
}

NTSTATUS ForceSleepAutoWakeupHdlr(IN PRTMP_ADAPTER pAd, IN PCmdQElmt CMDQelmt)
{
	USHORT  TbttNumToNextWakeUp;
	USHORT  NextDtim = pAd->StaCfg.DtimPeriod;
	ULONG   Now;

	NdisGetSystemUpTime(&Now);
	NextDtim -= (USHORT)(Now - pAd->StaCfg.LastBeaconRxTime)/pAd->CommonCfg.BeaconPeriod;

	TbttNumToNextWakeUp = pAd->StaCfg.DefaultListenCount;
	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM) && (TbttNumToNextWakeUp > NextDtim))
		TbttNumToNextWakeUp = NextDtim;

	RTMP_SET_PSM_BIT(pAd, PWR_SAVE);

	/* if WMM-APSD is failed, try to disable following line*/
	DBGPRINT(RT_DEBUG_TRACE, ("%s(line=%d): -->\n", __FUNCTION__, __LINE__));
	AsicSleepThenAutoWakeup(pAd, TbttNumToNextWakeUp);

	return NDIS_STATUS_SUCCESS;
}
#endif /* CONFIG_STA_SUPPORT */


/*
	==========================================================================
	Description:
		Set My BSSID

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
 /* CFG_TODO */
VOID AsicSetBssid(RTMP_ADAPTER *pAd, UCHAR *pBssid, UCHAR curr_bssid_idx)
{
	UINT32 val;
	// TODO: shiang-7603, now only configure Current BSSID Address 0
	ULONG curr_bssid_reg_base = RMAC_CB0R0;//register for Current_Bssid from 0x60140004

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): Set BSSID=%02x:%02x:%02x:%02x:%02x:%02x, curr_bssid_idx = %d\n",
				__FUNCTION__, PRINT_MAC(pBssid), curr_bssid_idx));


	val = (UINT32)((pBssid[0]) |
				  (UINT32)(pBssid[1] << 8) |
				  (UINT32)(pBssid[2] << 16) |
				  (UINT32)(pBssid[3] << 24));
	RTMP_IO_WRITE32(pAd, curr_bssid_reg_base + (curr_bssid_idx * 8), val);

	val = (UINT32)(pBssid[4]) | (UINT32)(pBssid[5] << 8) | (1 <<16);
	RTMP_IO_WRITE32(pAd, (curr_bssid_reg_base + 4) + (curr_bssid_idx * 8), val);

	// If we enable BSSID0, we should not enable MBSS0, or the BSSID index will wrong
	//RTMP_IO_READ32(pAd, RMAC_ACBEN, &val);
	//val |= 0x1;
	//RTMP_IO_WRITE32(pAd, RMAC_ACBEN, val);

	return;
}


INT AsicSetDevMac(RTMP_ADAPTER *pAd, UCHAR *addr, UCHAR omac_idx)
{
	UINT32 val;
	ULONG own_mac_reg_base = RMAC_OMA0R0;//register for Own_Mac from 0x60140024

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): Set OwnMac=%02x:%02x:%02x:%02x:%02x:%02x\n",
				__FUNCTION__, PRINT_MAC(addr)));

	val = (addr[0]) | (addr[1]<<8) |  (addr[2]<<16) | (addr[3]<<24);
	RTMP_IO_WRITE32(pAd, own_mac_reg_base + (omac_idx * 8), val);

	val = addr[4] | (addr[5]<<8) |  (1 <<16);
	RTMP_IO_WRITE32(pAd, (own_mac_reg_base + 4) + (omac_idx * 8), val);

	return TRUE;
}


#ifdef CONFIG_AP_SUPPORT
VOID AsicSetMbssMode(RTMP_ADAPTER *pAd, UCHAR NumOfBcns)
{
	UCHAR NumOfMacs;
	UINT32 regValue;

	/*
		Note:
			1.The MAC address of Mesh and AP-Client link are different from Main BSSID.
			2.If the Mesh link is included, its MAC address shall follow the last MBSSID's MAC by increasing 1.
			3.If the AP-Client link is included, its MAC address shall follow the Mesh interface MAC by increasing 1.
	*/
	NumOfMacs = pAd->ApCfg.BssidNum + MAX_MESH_NUM + MAX_APCLI_NUM;

	/* set Multiple BSSID mode */
	if (NumOfMacs <= 1)
	{
		pAd->ApCfg.MacMask = ~(1-1);
	}
	else if (NumOfMacs <= 2)
	{
		if ((pAd->CurrentAddress[5] % 2 != 0)
#ifdef P2P_SUPPORT
#ifdef P2P_ODD_MAC_ADJUST
			&& FALSE
#endif /* P2P_ODD_MAC_ADJUST */
#endif /* P2P_SUPPORT */
		)
			DBGPRINT(RT_DEBUG_ERROR, ("The 2-BSSID mode is enabled, the BSSID byte5 MUST be the multiple of 2\n"));

		pAd->ApCfg.MacMask = ~(2-1);
	}
	else if (NumOfMacs <= 4)
	{
		if (pAd->CurrentAddress[5] % 4 != 0)
			DBGPRINT(RT_DEBUG_ERROR, ("The 4-BSSID mode is enabled, the BSSID byte5 MUST be the multiple of 4\n"));

		pAd->ApCfg.MacMask = ~(4-1);
	}
	else if (NumOfMacs <= 8)
	{
		if (pAd->CurrentAddress[5] % 8 != 0)
			DBGPRINT(RT_DEBUG_ERROR, ("The 8-BSSID mode is enabled, the BSSID byte5 MUST be the multiple of 8\n"));

		pAd->ApCfg.MacMask = ~(8-1);
	}
	else if (NumOfMacs <= 16)
	{
		/* Set MULTI_BSSID_MODE_BIT4 in MAC register 0x1014 */
		pAd->ApCfg.MacMask = ~(16-1);
	}

	// TODO: shiang-7603
	RTMP_IO_READ32(pAd, RMAC_ACBEN, &regValue);
	regValue |=  ((1 << NumOfMacs) - 1);
	// TODO: shiang-MT7603, for MBSS0, now we use BSS0 instead of MBSS0, or the BSSID Index will be mismatch!
	regValue &= (~0x01);
	RTMP_IO_WRITE32(pAd, RMAC_ACBEN, regValue);

}
#endif /* CONFIG_AP_SUPPORT */


#ifdef APCLI_SUPPORT
#ifdef MAC_REPEATER_SUPPORT
INT AsicSetMacAddrExt(RTMP_ADAPTER *pAd, BOOLEAN bEnable)
{
	RMAC_MORE_STRUC rmac_more;
	if (pAd->chipCap.hif_type == HIF_MT) {
		RTMP_IO_READ32(pAd, RMAC_MORE, &rmac_more.word);

		if (bEnable == 0)
			rmac_more.field.muar_mode_sel = 0;
		else
			rmac_more.field.muar_mode_sel = 1;

		RTMP_IO_WRITE32(pAd, RMAC_MORE, rmac_more.word);
	}
	else
		return FALSE;

	return TRUE;
}

VOID RTMPInsertRepeaterAsicEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR CliIdx,
	IN PUCHAR pAddr)
{
	UCHAR tempMAC[MAC_ADDR_LEN];
        RMAC_MAR0_STRUC rmac_mcbcs0;
        RMAC_MAR1_STRUC rmac_mcbcs1;

	COPY_MAC_ADDR(tempMAC, pAddr);

	DBGPRINT(RT_DEBUG_ERROR, ("\n%s %02x:%02x:%02x:%02x:%02x:%02x-%02x\n",
			__func__, PRINT_MAC(tempMAC), CliIdx));

	NdisZeroMemory(&rmac_mcbcs0, sizeof(RMAC_MAR0_STRUC));
	rmac_mcbcs0.addr_31_0 = tempMAC[0] + (tempMAC[1] << 8) +(tempMAC[2] << 16) +(tempMAC[3] << 24);
	RTMP_IO_WRITE32(pAd, RMAC_MAR0, rmac_mcbcs0.addr_31_0);

	NdisZeroMemory(&rmac_mcbcs1, sizeof(RMAC_MAR1_STRUC));
	rmac_mcbcs1.field.addr_39_32 = tempMAC[4];
	rmac_mcbcs1.field.addr_47_40 = tempMAC[5];
	rmac_mcbcs1.field.access_start = 1;
	rmac_mcbcs1.field.readwrite = 1;
	rmac_mcbcs1.field.multicast_addr_index = CliIdx;
	RTMP_IO_WRITE32(pAd, RMAC_MAR1, rmac_mcbcs1.word);

	return;
}

VOID RTMPRemoveRepeaterAsicEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR CliIdx)
{
    RMAC_MAR0_STRUC rmac_mcbcs0;
    RMAC_MAR1_STRUC rmac_mcbcs1;

	DBGPRINT(RT_DEBUG_WARN, (" %s.\n", __FUNCTION__));

    NdisZeroMemory(&rmac_mcbcs0, sizeof(RMAC_MAR0_STRUC));
    RTMP_IO_WRITE32(pAd, RMAC_MAR0, rmac_mcbcs0.addr_31_0);

    NdisZeroMemory(&rmac_mcbcs1, sizeof(RMAC_MAR1_STRUC));
    rmac_mcbcs1.field.access_start = 1;
    rmac_mcbcs1.field.readwrite = 1;

    rmac_mcbcs1.field.multicast_addr_index = CliIdx;//start from idx 0

    RTMP_IO_WRITE32(pAd, RMAC_MAR1, rmac_mcbcs1.word);//clear client entry first.

    NdisZeroMemory(&rmac_mcbcs0, sizeof(RMAC_MAR0_STRUC));
    RTMP_IO_WRITE32(pAd, RMAC_MAR0, rmac_mcbcs0.addr_31_0);

    NdisZeroMemory(&rmac_mcbcs1, sizeof(RMAC_MAR1_STRUC));
    rmac_mcbcs1.field.access_start = 1;
    rmac_mcbcs1.field.readwrite = 1;

    rmac_mcbcs1.field.multicast_addr_index = 0x10 + CliIdx;//start from idx 16

    RTMP_IO_WRITE32(pAd, RMAC_MAR1, rmac_mcbcs1.word);//clear rootap entry.
}

void insert_repeater_root_entry(
	IN PRTMP_ADAPTER pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN  UCHAR *pAddr,
	IN UCHAR ReptCliIdx)
{
	struct rtmp_mac_ctrl *wtbl_ctrl = &pAd->mac_ctrl;
	struct wtbl_entry tb_entry;
	union WTBL_1_DW0 *dw0 = (union WTBL_1_DW0 *)&tb_entry.wtbl_1.wtbl_1_d0.word;
	union WTBL_1_DW1 *dw1 = (union WTBL_1_DW1 *)&tb_entry.wtbl_1.wtbl_1_d1.word;
	union WTBL_1_DW2 *dw2 = (union WTBL_1_DW2 *)&tb_entry.wtbl_1.wtbl_1_d2.word;
	RMAC_MAR0_STRUC rmac_mcbcs0;
	RMAC_MAR1_STRUC rmac_mcbcs1;
	//STA_TR_ENTRY *tr_entry;

	if (pEntry)
	{
		//tr_entry = &pAd->MacTab.tr_entry[pEntry->wcid];
		//tr_entry->PortSecured = WPA_802_1X_PORT_SECURED;
		//pEntry->AuthState = AS_AUTH_OPEN;//TODO, Carter, check this.
		//pEntry->Sst = SST_ASSOC;

		tb_entry.wtbl_addr[0] = wtbl_ctrl->wtbl_base_addr[0] +
							pEntry->wcid * wtbl_ctrl->wtbl_entry_size[0];

		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0], &tb_entry.wtbl_1.wtbl_1_d0.word);
		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0] + 4, &tb_entry.wtbl_1.wtbl_1_d1.word);
		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0] + 8, &tb_entry.wtbl_1.wtbl_1_d2.word);
		dw0->field.wm = 0;
		dw0->field.muar_idx = 0x20 + ReptCliIdx;
		dw0->field.rc_a1 = 1;
		RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0], dw0->word);

		DBGPRINT(RT_DEBUG_OFF, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
					__FUNCTION__, ReptCliIdx, tb_entry.wtbl_addr[0], dw0->word));
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
					__FUNCTION__, ReptCliIdx,  tb_entry.wtbl_addr[0] + 4, dw1->word));
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
					__FUNCTION__, ReptCliIdx, tb_entry.wtbl_addr[0] + 8, dw2->word));

		NdisZeroMemory(&rmac_mcbcs0, sizeof(RMAC_MAR0_STRUC));
		rmac_mcbcs0.addr_31_0 = pAddr[0] + (pAddr[1] << 8) +(pAddr[2] << 16) +(pAddr[3] << 24);
		RTMP_IO_WRITE32(pAd, RMAC_MAR0, rmac_mcbcs0.addr_31_0);

		NdisZeroMemory(&rmac_mcbcs1, sizeof(RMAC_MAR1_STRUC));
		rmac_mcbcs1.field.addr_39_32 = pAddr[4];
		rmac_mcbcs1.field.addr_47_40 = pAddr[5];
		rmac_mcbcs1.field.access_start = 1;
		rmac_mcbcs1.field.readwrite = 1;
		rmac_mcbcs1.field.multicast_addr_index = 0x10 + ReptCliIdx;
		RTMP_IO_WRITE32(pAd, RMAC_MAR1, rmac_mcbcs1.word);
	}
}


#endif /* MAC_REPEATER_SUPPORT */
#endif /* APCLI_SUPPORT */


INT AsicSetRxFilter(RTMP_ADAPTER *pAd)
{
	//UINT32 rx_filter_flag = 0;
	UINT32 Value;

	/* enable RX of MAC block*/
	if ((pAd->OpMode == OPMODE_AP)
#ifdef P2P_SUPPORT
		|| P2P_GO_ON(pAd)
#endif /* P2P_SUPPORT */
#ifdef RT_CFG80211_P2P_SUPPORT
		|| CFG_P2PGO_ON(pAd)
#endif /* RT_CFG80211_P2P_SUPPORT */
	)
	{
		//rx_filter_flag = APNORMAL;

#ifdef CONFIG_AP_SUPPORT
#ifdef IDS_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			if (pAd->ApCfg.IdsEnable)
			{
				//rx_filter_flag &= (~0x4);	/* Don't drop those not-U2M frames*/
			}
		}
#endif /* IDS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
	}
#ifdef CONFIG_STA_SUPPORT
	else
	{
		if ((MONITOR_ON(pAd))) /* Enable Rx with promiscuous reception */
		{
			//rx_filter_flag = 0x3;
		}
		//else
#ifdef XLINK_SUPPORT
		if (pAd->StaCfg.PSPXlink)
		{
			//rx_filter_flag = PSPXLINK;
		}
		//else
#endif /* XLINK_SUPPORT */
			//rx_filter_flag = STANORMAL;     /* Staion not drop control frame will fail WiFi Certification.*/
	}
#endif /* CONFIG_STA_SUPPORT */

	if (pAd->chipCap.hif_type == HIF_MT) {
		RTMP_IO_READ32(pAd, RMAC_RFCR, &Value);
		Value &= ~RM_FRAME_REPORT_EN;
		Value &= ~DROP_NOT_MY_BSSID;
		Value &= ~DROP_NOT_IN_MC_TABLE;
#if defined(MT7628_FPGA)
        Value = 0x1002;
#endif
		/*disable RX diff BSSID BCN filter */
		Value &= ~DROP_DIFF_BSSID_BCN;
		/*disable RX PROB request filter */
		Value &= ~DROP_PROBE_REQ;
		/* Disable Rx Duplicate Packet Drop filter */
		Value &= ~DROP_DUPLICATE;

#ifdef CONFIG_SNIFFER_SUPPORT
		if ((MONITOR_ON(pAd)) && pAd->monitor_ctrl.current_monitor_mode == MONITOR_MODE_FULL) /* Enable Rx with promiscuous reception */
			Value = 0x3;
#endif


		RTMP_IO_WRITE32(pAd, RMAC_RFCR, Value);
		RTMP_IO_WRITE32(pAd, RMAC_RFCR1, 0);
	}

	return TRUE;
}


#ifdef DOT11_N_SUPPORT
INT AsicWtblSetRDG(RTMP_ADAPTER *pAd, BOOLEAN bEnable)
{
    UINT32 wcid;
    MAC_TABLE *pMacTable;
    MAC_TABLE_ENTRY *pEntry;
    struct wtbl_entry tb_entry;
    union WTBL_1_DW2 *dw2 = (union WTBL_1_DW2 *)&tb_entry.wtbl_1.wtbl_1_d2.word;

    pMacTable = &pAd->MacTab;
    NdisZeroMemory(&tb_entry, sizeof(tb_entry));

    /* Search for the Ralink STA */
    for (wcid = 1; wcid < MAX_LEN_OF_MAC_TABLE; wcid++)
    {
        pEntry = &pMacTable->Content[wcid];

		if (CLIENT_STATUS_TEST_FLAG(pEntry, fCLIENT_STATUS_RALINK_CHIPSET)) {
			if (mt_wtbl_get_entry234(pAd, (UCHAR)wcid, &tb_entry) == FALSE) {
				DBGPRINT(RT_DEBUG_ERROR,
					("%s():Cannot found WTBL2/3/4 for WCID(%d)\n",
					__func__, wcid));
				return FALSE;
			}

            RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0] + (2 * 4), &dw2->word);

            if (bEnable)
            {
                dw2->field.r = 1;
                dw2->field.rdg_ba = 1;
            }
            else
            {
                dw2->field.r = 0;
                dw2->field.rdg_ba = 0;
            }

            RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + (2 * 4), dw2->word);
        }
    }

    return TRUE;
}


INT AsicSetRDG(RTMP_ADAPTER *pAd, BOOLEAN bEnable)
{
	UINT32 tmac_tcr, agg_pcr, tmac_trcr;


	RTMP_IO_READ32(pAd, TMAC_TCR, &tmac_tcr);
	RTMP_IO_READ32(pAd, AGG_PCR, &agg_pcr);
    RTMP_IO_READ32(pAd, TMAC_TRCR, &tmac_trcr);


	if (bEnable)
	{
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);
		/* enable RDG Rsp. also w/ Ralink Mode is necessary */
		tmac_tcr |= (RDG_RA_MODE | RDG_RESP_EN);
		/* LongNAV protect */
		agg_pcr &= ~(PROTECTION_MODE);

        tmac_trcr = tmac_trcr & ~I2T_CHK_EN;
	}
	else
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);
		/* disable RDG Rsp. also w/ Ralink Mode is necessary */
		tmac_tcr &= ~(RDG_RA_MODE | RDG_RESP_EN);
		/* single frame protect */
		agg_pcr |= PROTECTION_MODE;

        tmac_trcr = tmac_trcr | I2T_CHK_EN;
	}

	RTMP_IO_WRITE32(pAd, TMAC_TCR, tmac_tcr);
	RTMP_IO_WRITE32(pAd, AGG_PCR, agg_pcr);
    RTMP_IO_WRITE32(pAd, TMAC_TRCR, tmac_trcr);

	return TRUE;
}
#endif /* DOT11_N_SUPPORT */


/*
    ========================================================================
    Routine Description:
        Set/reset MAC registers according to bPiggyBack parameter

    Arguments:
        pAd         - Adapter pointer
        bPiggyBack  - Enable / Disable Piggy-Back

    Return Value:
        None

    ========================================================================
*/
VOID RTMPSetPiggyBack(RTMP_ADAPTER *pAd, BOOLEAN bPiggyBack)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	}

	return;
}


#define INT_TIMER_EN_PRE_TBTT	0x1
#define INT_TIMER_EN_GP_TIMER	0x2
static INT AsicSetIntTimerEn(RTMP_ADAPTER *pAd, BOOLEAN enable, UINT32 type, UINT32 timeout)
{

return 0;

}


INT AsicSetPreTbtt(RTMP_ADAPTER *pAd, BOOLEAN enable)
{
	UINT32 timeout = 0, bitmask = 0;
	INT bss_idx = 0; // TODO: this index may carried by parameters!

	
#ifdef RT_CFG80211_P2P_SUPPORT
/*P2P GO & SoftAP us HW BSSID1 for beacon*/
	if (pAd->cfg80211_ctrl.isCfgInApMode == RT_CMD_80211_IFTYPE_AP){

		bss_idx = CFG_GO_BSSID_IDX;
		DBGPRINT(RT_DEBUG_OFF, ("--->%s(),apidx:%d,enable:%d\n", __FUNCTION__,bss_idx,enable));
	}
#endif /* RT_CFG80211_P2P_SUPPORT */

	ASSERT(bss_idx <= 3);
    bitmask = 0xff << (bss_idx * 8);

	if (enable == TRUE) {
		/*
		   each HW BSSID has its own PreTBTT interval,
		   unit is 64us, 0x00~0xff is configurable.
		   Base on RTMP chip experience,
		   Pre-TBTT is 6ms before TBTT interrupt. 1~10 ms is reasonable.
		*/


		RTMP_IO_READ32(pAd, LPON_PISR, &timeout);
		timeout &= (~bitmask);
		timeout |= (PRETBTT_INTERVAL << (bss_idx * 8));
		RTMP_IO_WRITE32(pAd, LPON_PISR, timeout);

		DBGPRINT(RT_DEBUG_OFF, ("%s(): bss_idx=%d, PreTBTT timeout = 0x%x\n",
					__FUNCTION__, bss_idx, timeout));
	}
    else {
        RTMP_IO_READ32(pAd, LPON_PISR, &timeout);
		timeout &= (~bitmask);
		RTMP_IO_WRITE32(pAd, LPON_PISR, timeout);
        DBGPRINT(RT_DEBUG_OFF, ("%s(): bss_idx=%d, PreTBTT timeout = 0x%x\n",
					__FUNCTION__, bss_idx, timeout));
    }

	return TRUE;
}


INT AsicSetGPTimer(RTMP_ADAPTER *pAd, BOOLEAN enable, UINT32 timeout)
{
	return AsicSetIntTimerEn(pAd, enable, INT_TIMER_EN_GP_TIMER, timeout);
}


INT AsicSetChBusyStat(RTMP_ADAPTER *pAd, BOOLEAN enable)
{
	//UINT32 mac_val;

	/* Note: if bit 0 == 0, the function will be disabled */
	if (enable) {
		/*
			Count EIFS, NAV, RX busy, TX busy as channel busy and
			enable Channel statistic timer (bit 0)
		*/
		//mac_val = 0x0000001F;
	}
	else
		//mac_val = 0x0;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	}


	return TRUE;
}


INT AsicGetTsfTime(RTMP_ADAPTER *pAd, UINT32 *high_part, UINT32 *low_part)
{
    UINT32 Value = 0;
    RTMP_IO_READ32(pAd, LPON_T0CR, &Value);
    Value = (Value & 0xc) | TSF_TIMER_VALUE_READ;//keep HW mode value.
    //Value = Value | TSF_TIMER_VALUE_READ;
    RTMP_IO_WRITE32(pAd, LPON_T0CR, Value);

    RTMP_IO_READ32(pAd, LPON_UTTR0, low_part);
    RTMP_IO_READ32(pAd, LPON_UTTR1, high_part);

	return TRUE;
}


#ifdef LINUX
#ifdef RTMP_WLAN_HOOK_SUPPORT
EXPORT_SYMBOL(AsicGetTsfTime);
#endif /* RTMP_WLAN_HOOK_SUPPORT */
#endif /* LINUX */


#ifdef CONFIG_AP_SUPPORT
static UCHAR    check_point_num = 0;
static VOID DumpBcnQMessage(RTMP_ADAPTER *pAd, INT apidx)
{
	int j = 0;
	//BSS_STRUCT *pMbss;
	UINT32 tmp_value = 0, hif_br_start_base = 0x4540;
	CHAR tmp[5];

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)) {
		return;
	}

	//pMbss = &pAd->ApCfg.MBSSID[apidx];

	DBGPRINT(RT_DEBUG_ERROR, ("hif cr dump:\n"));
	for (j = 0; j < 80; j++)
	{
		RTMP_IO_READ32(pAd, (hif_br_start_base + (j * 4)), &tmp_value);
		DBGPRINT(RT_DEBUG_ERROR, ("CR:0x%x=%x\t", (hif_br_start_base + (j * 4)), tmp_value));
		if ((j % 4) == 3)
			DBGPRINT(RT_DEBUG_ERROR, ("\n"));
	}

	DBGPRINT(RT_DEBUG_ERROR, ("\ncheck PSE Q:\n"));
	for (j = 0; j <= 8; j++) {
		snprintf(tmp, sizeof(tmp), "%d", j);
		set_get_fid(pAd, tmp);
	}

	DBGPRINT(RT_DEBUG_ERROR, ("check TX_START and SLOT_IDLE\n"));
	for (j = 0; j < 10; j++) {
		RTMP_IO_READ32(pAd, ARB_BCNQCR0, &tmp_value);
		DBGPRINT(RT_DEBUG_ERROR, ("ARB_BCNQCR0: %x\n", tmp_value));
	}
#ifdef DBG
	if (RT_DEBUG_ERROR <= RTDebugLevel) {
		show_trinfo_proc(pAd, NULL);
	}
#endif /*DBG*/
}


VOID APCheckBcnQHandler(RTMP_ADAPTER *pAd, INT apidx, BOOLEAN *is_pretbtt_int)
{
	UINT32 val=0, temp = 0;
	int j = 0;
	BSS_STRUCT *pMbss;
	//struct wifi_dev *wdev;

	UINT32   Lowpart, Highpart;
	UINT32   int_delta;

	pMbss = &pAd->ApCfg.MBSSID[apidx];
	//wdev = &pMbss->wdev;

	if (pMbss->bcn_buf.bcn_state < BCN_TX_DMA_DONE) {
		if (apidx == 0) {
			check_point_num++;
			if (check_point_num > 10) {
				DumpBcnQMessage(pAd, apidx);
				check_point_num = 0;
			}
		}
		return;
	} else if (apidx == 0) {
		check_point_num = 0;
	}

	AsicGetTsfTime(pAd, &Highpart, &Lowpart);
	int_delta = Lowpart - pMbss->WriteBcnDoneTime[pMbss->timer_loop];
	if (int_delta < (pAd->CommonCfg.BeaconPeriod * 1024/* unit is usec */))
	{
		/* update beacon has been called more than once in 1 bcn period,
		it might be called other than HandlePreTBTT interrupt routine.*/
		*is_pretbtt_int = FALSE;
		return;
	}

	if (pMbss->bcn_not_idle_time % 10 == 9) {
		pMbss->bcn_not_idle_time = 0;

		if (apidx == 0)
			DumpBcnQMessage(pAd, apidx);

		*is_pretbtt_int = FALSE;
		return;
	}
	else if (pMbss->bcn_not_idle_time % 3 == 2) {
		pMbss->bcn_not_idle_time++;
		pMbss->bcn_recovery_num++;
		*is_pretbtt_int = TRUE;
	}
	else {
		pMbss->bcn_not_idle_time++;
		*is_pretbtt_int = FALSE;
		return;
	}

	if (apidx > 0)
		val = val | (1 << (apidx+15));
	else
		val = 1;

	j = 0;
	/* Flush Beacon Queue */
	RTMP_IO_WRITE32(pAd, ARB_BCNQCR1, val);
	while (1) {
		RTMP_IO_READ32(pAd, ARB_BCNQCR1, &temp);//check bcn_flush cr status
		if (temp & val) {
			j++;
			OS_WAIT(1);
			if (j > 1000) {
				DBGPRINT(RT_DEBUG_ERROR,
					("%s, bcn_flush too long!, j = %x\n", __func__, j));
				break;
			}
		}
		else {
			break;
		}
	}

	val = 0xa8c70000;
	j = 0;
	temp = 0;
	if (apidx > 0)
		val = val | 0x1000 | (apidx << 8);

	RTMP_IO_WRITE32(pAd, 0x21c08, val);//flush all stuck bcn

	while (1) {
		RTMP_IO_READ32(pAd, 0x21c08, &temp);//flush all stuck bcn
		if (temp >> 31) {
			j++;
			OS_WAIT(1);
			if (j > 1000) {
				DBGPRINT(RT_DEBUG_ERROR,
					("%s, flush all stuck bcn too long!! j = %x\n",
					__func__, j));
				break;
			}
		}
		else {
			break;
		}
	}

	//check filter resilt
	RTMP_IO_READ32(pAd, 0x21c0c, &temp);
	DBGPRINT(RT_DEBUG_ERROR, ("flush result = %x\n", temp));
	DBGPRINT(RT_DEBUG_ERROR, ("check pse fid Q7:"));
	set_get_fid(pAd, "7");

	val = 0;
	if (apidx > 0)
		val = val | (1 << (apidx+15));
	else
		val = 1;

	RTMP_IO_READ32(pAd, ARB_BCNQCR0, &temp);//re-enable bcn_start
	temp = temp | val;
	RTMP_IO_WRITE32(pAd, ARB_BCNQCR0, temp);

	pMbss->bcn_buf.bcn_state = BCN_TX_IDLE;
}


#endif /* CONFIG_AP_SUPPORT */

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicDisableSync(RTMP_ADAPTER *pAd)
{
	UINT32  value;
#if 0
#ifdef CONFIG_AP_SUPPORT
	INT32 IdBss, MaxNumBss = pAd->ApCfg.BssidNum;
    BSS_STRUCT *pMbss;
#endif
#endif
    DBGPRINT(RT_DEBUG_TRACE, ("--->Disable TSF synchronization\n"));

    pAd->TbttTickCount = 0;

    if (pAd->chipCap.hif_type == HIF_MT) {
#ifdef CONFIG_AP_SUPPORT
        //INT32 IdBss, MaxNumBss = pAd->ApCfg.BssidNum;

        /* Disable PreTBTT/TBTT interrupt for inform host */
        value = 0;
        RTMP_IO_WRITE32(pAd, HWIER3, value);

#if 0
        /*  Stop send TX packets */
        RTMP_OS_NETDEV_STOP_QUEUE(pAd->net_dev);
        RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);

        /*  Disable RX */
        AsicSetMacTxRx(pAd, ASIC_MAC_RX, FALSE);
        /*  Polling TX/RX path until packets empty */
        MTPciPollTxRxEmpty(pAd);
        /*  Disable PDMA */
        AsicSetWPDMA(pAd, PDMA_TX_RX, 0);
#ifdef RTMP_MAC_PCI
        /*  Disable Interrupt */
        if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE))
        {
            RTMP_ASIC_INTERRUPT_DISABLE(pAd);
        }
#endif /* RTMP_MAC_PCI */

        IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
        {
            if (MaxNumBss > MAX_MBSSID_NUM(pAd))
                MaxNumBss = MAX_MBSSID_NUM(pAd);

            /* first IdBss must not be 0 (BSS0), must be 1 (BSS1) */
            for (IdBss = FIRST_MBSSID; IdBss < MAX_MBSSID_NUM(pAd); IdBss++)
            {
                if (pAd->ApCfg.MBSSID[IdBss].wdev.if_dev) {
                    RTMP_OS_NETDEV_STOP_QUEUE(pAd->ApCfg.MBSSID[IdBss].wdev.if_dev);
                    pMbss = &pAd->ApCfg.MBSSID[IdBss];
                    //if (pMbss->bcn_buf.bcn_state > BCN_TX_IDLE)
                        pMbss->bcn_buf.bcn_state = BCN_TX_UNINIT;
                }
            }
        }
        pMbss = &pAd->ApCfg.MBSSID[0];
        //if (pMbss->bcn_buf.bcn_state > BCN_TX_IDLE)
            pMbss->bcn_buf.bcn_state = BCN_TX_UNINIT;
#endif
        return;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
    /* Configure Beacon interval */
	RTMP_IO_WRITE32(pAd, LPON_T0TPCR, value);

	/* Clear Pre-TBTT Trigger, and calcuate next TBTT timer by HW*/
	//enable PRETBTT0INT_EN, PRETBTT0TIMEUP_EN
	//and TBTT0PERIODTIMER_EN, TBTT0TIMEUP_EN
	RTMP_IO_WRITE32(pAd, LPON_MPTCR1, 0x99);//TODO: TBTT1, TBTT2.

	/* Enable interrupt */
	value = 0;
	RTMP_IO_WRITE32(pAd, HWIER3, value);

	/* Config BCN/BMC timoeut, or the normal Tx wil be blocked forever if no beacon frame in Queue */
	//Value = 0x01800180;
	//RTMP_IO_WRITE32(pAd, LPON_BCNTR, mac_val);

	/* Configure Beacon Queue Operation mode */
	RTMP_IO_READ32(pAd, ARB_SCR, &value);
	value &= 0xfffffffc;//Clear bit[0:1] for MBSSID 0
	RTMP_IO_WRITE32(pAd, ARB_SCR, value);

    RTMP_IO_READ32(pAd, LPON_T0CR, &value);
    value = value & 0xfffffff3;//sta-like HW mode.
    RTMP_IO_WRITE32(pAd, LPON_T0CR, value);

	/* Clear Beacon Queue */
	value = 0x1;
	RTMP_IO_WRITE32(pAd, ARB_BCNQCR1, value);

    return;
#endif /* CONFIG_STA_SUPPORT */

    }
}


#if 0
/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicEnableBssSync(RTMP_ADAPTER *pAd, USHORT BeaconPeriod)
{
	UINT32 mac_val;

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		// TODO: shiang-7603
		DBGPRINT(RT_DEBUG_TRACE, ("--->%s():BeaconPeriod = %d ms\n",
				__FUNCTION__, BeaconPeriod));

		RTMP_IO_READ32(pAd, ARB_SCR, &mac_val);
		mac_val |= 0x1;
		RTMP_IO_WRITE32(pAd, ARB_SCR, mac_val);

		RTMP_IO_READ32(pAd, ARB_BCNQCR0, &mac_val);
		mac_val |= 0x1;
		RTMP_IO_WRITE32(pAd, ARB_BCNQCR0, mac_val);
	}
#endif /* CONFIG_AP_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		// TODO: shiang-MT7603, TBD!
	}
#endif /* CONFIG_STA_SUPPORT */
}
#else

VOID AsicEnableBssSync(RTMP_ADAPTER *pAd, USHORT BeaconPeriod)
{
	UINT32 bitmask = 0;
	UINT32 Value;
	INT bss_idx = 0; // TODO: this index may carried by parameters!

    DBGPRINT(RT_DEBUG_TRACE, ("--->%s():\n", __FUNCTION__));
#if 0
#ifdef CONFIG_AP_SUPPORT
	INT32 IdBss, MaxNumBss = pAd->ApCfg.BssidNum;
#endif /* CONFIG_AP_SUPPORT */


#ifdef RTMP_MAC_PCI
	/*  Enable Interrupt */
	RTMP_ASIC_INTERRUPT_ENABLE(pAd);
#endif /* RTMP_MAC_PCI */

	/*  Enable PDMA */
	AsicSetWPDMA(pAd, PDMA_TX_RX, 1);

	/* Enable RX */
	AsicSetMacTxRx(pAd, ASIC_MAC_RX, TRUE);

	/*  Enable normal operation */
	RTMP_OS_NETDEV_START_QUEUE(pAd->net_dev);

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (MaxNumBss > MAX_MBSSID_NUM(pAd))
			MaxNumBss = MAX_MBSSID_NUM(pAd);

		/*  first IdBss must not be 0 (BSS0), must be 1 (BSS1) */
		for (IdBss = FIRST_MBSSID; IdBss < MAX_MBSSID_NUM(pAd); IdBss++)
		{
			if (pAd->ApCfg.MBSSID[IdBss].wdev.if_dev)
				RTMP_OS_NETDEV_START_QUEUE(pAd->ApCfg.MBSSID[IdBss].wdev.if_dev);
		}
	}
#endif
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
#endif

	/* Configure Beacon interval */
	RTMP_IO_READ32(pAd, LPON_T0TPCR, &Value);
	Value = 0;
	Value &= ~BEACONPERIODn_MASK;
	Value |= BEACONPERIODn(BeaconPeriod);
	Value |= TBTTn_CAL_EN;
	RTMP_IO_WRITE32(pAd, LPON_T0TPCR, Value);

	/* Enable Pre-TBTT Trigger, and calcuate next TBTT timer by HW*/
	//enable PRETBTT0INT_EN, PRETBTT0TIMEUP_EN
	//and TBTT0PERIODTIMER_EN, TBTT0TIMEUP_EN
	RTMP_IO_WRITE32(pAd, LPON_MPTCR0, 0x99);//TODO: TBTT1, TBTT2.

	/* Set Pre-TBTT interval */
	/*
	   each HW BSSID has its own PreTBTT interval,
	   unit is 64us, 0x00~0xff is configurable.
	   Base on RTMP chip experience,
	   Pre-TBTT is 6ms before TBTT interrupt. 1~10 ms is reasonable.
	*/
	ASSERT(bss_idx <= 3);
	bitmask = 0xff << (bss_idx * 8);
	RTMP_IO_READ32(pAd, LPON_PISR, &Value);
	Value &= (~bitmask);
	Value |= (0x50 << (bss_idx * 8));
	//mac_val =0x50505050;
	RTMP_IO_WRITE32(pAd, LPON_PISR, Value);

	/* Enable interrupt */
	RTMP_IO_READ32(pAd, HWIER3, &Value);
	Value = 0;
	Value |= TBTT0;
	Value |= PRETBTT0;
	RTMP_IO_WRITE32(pAd, HWIER3, Value);

	/* Config BCN/BMC timoeut, or the normal Tx wil be blocked forever if no beacon frame in Queue */
	Value = 0x01800180;
	//RTMP_IO_WRITE32(pAd, LPON_BCNTR, mac_val);

	/* Configure Beacon Queue Operation mode */
	RTMP_IO_READ32(pAd, ARB_SCR, &Value);

	Value &= (~(ARB_SCR_TBTT_BCN_CTRL)); // work-around to make BCN need to content with other ACs
    Value |= ARB_SCR_TBTT_BM_CTRL;
    Value |= ARB_SCR_BCNQ_EMPTY_CTRL;
	Value |= BCNQ_OP_MODE_AP;//TODO, Carter, when use other HWBSSID, shall could choose index to set correcorresponding bit.
    //Value = Value & 0xefffffff;
	RTMP_IO_WRITE32(pAd, ARB_SCR, Value);

	/* Start Beacon Queue */
	RTMP_IO_READ32(pAd, ARB_BCNQCR0, &Value);
	Value |= 0x1;
	RTMP_IO_WRITE32(pAd, ARB_BCNQCR0, Value);
}
#endif

/* YF: Generate Beacon using HW1 */
VOID AsicEnableApBssSync(RTMP_ADAPTER *pAd, USHORT BeaconPeriod)
{
	UINT32 bitmask = 0;
    UINT32 Value;
    INT bss_idx = 0; // TODO: this index may carried by parameters!
	DBGPRINT(RT_DEBUG_TRACE, ("--->%s():\n", __FUNCTION__));

#ifdef CONFIG_AP_SUPPORT
    //INT32 IdBss, MaxNumBss = pAd->ApCfg.BssidNum;
#endif /* CONFIG_AP_SUPPORT */

#ifdef RT_CFG80211_P2P_SUPPORT
	/*P2P GO & SoftAP us HW BSSID1 for beacon*/
	if (pAd->cfg80211_ctrl.isCfgInApMode == RT_CMD_80211_IFTYPE_AP){
		bss_idx = CFG_GO_BSSID_IDX;
		DBGPRINT(RT_DEBUG_OFF, ("--->%s(),apidx:%d\n", __FUNCTION__,bss_idx));
	}

#endif /*RT_CFG80211_P2P_SUPPORT*/

    /* Configure Beacon interval */
    RTMP_IO_READ32(pAd, LPON_T1TPCR, &Value);
    Value = 0;
    Value &= ~BEACONPERIODn_MASK;
    Value |= BEACONPERIODn(BeaconPeriod);
    Value |= TBTTn_CAL_EN;
    RTMP_IO_WRITE32(pAd, LPON_T1TPCR, Value);

    /* Enable Pre-TBTT Trigger, and calcuate next TBTT timer by HW*/
    //enable PRETBTT0INT_EN, PRETBTT0TIMEUP_EN
    //and TBTT0PERIODTIMER_EN, TBTT0TIMEUP_EN
    RTMP_IO_WRITE32(pAd, LPON_MPTCR0, 0x9900);//TODO: TBTT1, TBTT2.

    /*
	   Set Pre-TBTT interval :
       each HW BSSID has its own PreTBTT interval,
       unit is 64us, 0x00~0xff is configurable.
       Base on RTMP chip experience,
       Pre-TBTT is 6ms before TBTT interrupt. 1~10 ms is reasonable.
	*/
    ASSERT(bss_idx <= 3);
    bitmask = 0xff << (bss_idx * 8);
    RTMP_IO_READ32(pAd, LPON_PISR, &Value);
    Value &= (~bitmask);
    Value |= (PRETBTT_INTERVAL << (bss_idx * 8));
    //mac_val =0x50505050;
    RTMP_IO_WRITE32(pAd, LPON_PISR, Value);

    /* Enable interrupt */
    RTMP_IO_READ32(pAd, HWIER3, &Value);
    Value = 0;
    Value |= TBTT0;
    Value |= PRETBTT0;
    RTMP_IO_WRITE32(pAd, HWIER3, Value);

    /* Config BCN/BMC timoeut, or the normal Tx wil be blocked forever if no beacon frame in Queue */
    Value = 0x01800180;
    //RTMP_IO_WRITE32(pAd, LPON_BCNTR, mac_val);

    /* Configure Beacon Queue Operation mode */
    RTMP_IO_READ32(pAd, ARB_SCR, &Value);

    Value &= (~(ARB_SCR_TBTT_BCN_CTRL)); // work-around to make BCN need to content with other ACs
    Value |= ARB_SCR_TBTT_BM_CTRL;
    Value |= ARB_SCR_BCNQ_EMPTY_CTRL;
    Value |= (BCNQ_OP_MODE_AP << 2);//TODO, Carter, when use other HWBSSID, shall could choose index to set correcorresponding bit.
	//Value = Value & 0xefffffff;
    RTMP_IO_WRITE32(pAd, ARB_SCR, Value);

    /* Start Beacon Queue */
    RTMP_IO_READ32(pAd, ARB_BCNQCR0, &Value);
    Value |= 0x2;
    RTMP_IO_WRITE32(pAd, ARB_BCNQCR0, Value);
}


#ifdef CONFIG_STA_SUPPORT
/*
	==========================================================================
	Description:
	Note:
		BEACON frame in shared memory should be built ok before this routine
		can be called. Otherwise, a garbage frame maybe transmitted out every
		Beacon period.

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicEnableIbssSync(RTMP_ADAPTER *pAd)
{
	UINT32 bitmask = 0;
	UINT32 Value;
	INT bss_idx = 0; // TODO: this index may carried by parameters!

DBGPRINT(RT_DEBUG_TRACE, ("--->%s():\n", __FUNCTION__));

#if 0
//TODO:Carter, for AdHoc, is the code segment as below needed?
#ifdef RTMP_MAC_PCI
        RTMP_ASIC_INTERRUPT_ENABLE(pAd);
#endif /* RTMP_MAC_PCI */

        RT28XXDMAEnable(pAd);
        RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
#endif

	/* Configure Beacon interval */
	RTMP_IO_READ32(pAd, LPON_T0TPCR, &Value);
	Value = 0;
	Value &= ~BEACONPERIODn_MASK;
	Value |= BEACONPERIODn(pAd->CommonCfg.BeaconPeriod);
	Value |= TBTTn_CAL_EN;
	RTMP_IO_WRITE32(pAd, LPON_T0TPCR, Value);

	/* Enable Pre-TBTT Trigger, and calcuate next TBTT timer by HW*/
	//enable PRETBTT0INT_EN, PRETBTT0TIMEUP_EN
	//and TBTT0PERIODTIMER_EN, TBTT0TIMEUP_EN
	RTMP_IO_WRITE32(pAd, LPON_MPTCR0, 0x99);//TODO: TBTT1, TBTT2.

	/* Set Pre-TBTT interval */
	/*
	   each HW BSSID has its own PreTBTT interval,
	   unit is 64us, 0x00~0xff is configurable.
	   Base on RTMP chip experience,
	   Pre-TBTT is 6ms before TBTT interrupt. 1~10 ms is reasonable.
	*/
	ASSERT(bss_idx <= 3);
	bitmask = 0xff << (bss_idx * 8);
	RTMP_IO_READ32(pAd, LPON_PISR, &Value);
	Value &= (~bitmask);
	Value |= (0x50 << (bss_idx * 8));
	//mac_val =0x50505050;
	RTMP_IO_WRITE32(pAd, LPON_PISR, Value);

	/* Enable interrupt */
	RTMP_IO_READ32(pAd, HWIER3, &Value);
	Value = 0;
	Value |= TBTT0;
	Value |= PRETBTT0;
	RTMP_IO_WRITE32(pAd, HWIER3, Value);

    RTMP_IO_READ32(pAd, LPON_T0CR, &Value);
    Value = Value | 0xc;//Ad-hoc like HW mode.
    RTMP_IO_WRITE32(pAd, LPON_T0CR, Value);

    RTMP_IO_READ32(pAd, ARB_AIFSR0, &Value);
    Value |= (2 << 24);
    RTMP_IO_WRITE32(pAd, ARB_AIFSR0, Value);

    RTMP_IO_READ32(pAd, ARB_ACCWIR1, &Value);
    Value |= (0x1f << 16);
    RTMP_IO_WRITE32(pAd, ARB_ACCWIR1, Value);


	/* Config BCN/BMC timoeut, or the normal Tx wil be blocked forever if no beacon frame in Queue */
	//Value = 0x01800180;
	//RTMP_IO_WRITE32(pAd, LPON_BCNTR, mac_val);

	/* Configure Beacon Queue Operation mode */
	RTMP_IO_READ32(pAd, ARB_SCR, &Value);
	Value |= BCNQ_OP_MODE_ADHOC;//TODO, Carter, when use other HWBSSID, shall could choose index to set correcorresponding bit.
	RTMP_IO_WRITE32(pAd, ARB_SCR, Value);

	/* Start Beacon Queue */
	RTMP_IO_READ32(pAd, ARB_BCNQCR0, &Value);
	Value |= 1;
	RTMP_IO_WRITE32(pAd, ARB_BCNQCR0, Value);
}
#endif /* CONFIG_STA_SUPPORT */


typedef struct _RTMP_WMM_PAIR {
	UINT32 Address;
	UINT32 Mask;
	UINT32 Shift;
} RTMP_WMM_PAIR, *PRTMP_WMM_PAIR;


static RTMP_WMM_PAIR wmm_txop_mask[] = {
	{TMAC_ACTXOPLR1, 0x0000ffff, 0}, /* AC0 - BK */
	{TMAC_ACTXOPLR1, 0xffff0000, 16}, /* AC1 - BE */
	{TMAC_ACTXOPLR0, 0x0000ffff, 0}, /* AC2 - VI */
	{TMAC_ACTXOPLR0, 0xffff0000, 16}, /* AC3 - VO */
};


static RTMP_WMM_PAIR wmm_aifsn_mask[] = {
	{ARB_AIFSR0, 0x0000000f, 0}, /* AC0 - BK */
	{ARB_AIFSR0, 0x000000f0, 4}, /* AC1 - BE */
	{ARB_AIFSR0, 0x00000f00, 8}, /* AC2  - VI */
	{ARB_AIFSR0, 0x0000f000, 12}, /* AC3 - VO */
};

static RTMP_WMM_PAIR wmm_cwmin_mask[] = {
	{ARB_ACCWIR0, 0x000000ff, 0}, /* AC0 - BK */
	{ARB_ACCWIR0, 0x0000ff00, 8}, /* AC1 - BE */
	{ARB_ACCWIR0, 0x00ff0000, 16}, /* AC2  - VI */
	{ARB_ACCWIR0, 0xff000000, 24}, /* AC3 - VO */
};

static RTMP_WMM_PAIR wmm_cwmax_mask[] = {
	{ARB_ACCWXR0, 0x0000ffff, 0}, /* AC0 - BK */
	{ARB_ACCWXR0, 0xffff0000, 16}, /* AC1 - BE */
	{ARB_ACCWXR1, 0x0000ffff, 0}, /* AC2  - VI */
	{ARB_ACCWXR1, 0xffff0000, 16}, /* AC3 - VO */
};


UINT32 AsicGetWmmParam(RTMP_ADAPTER *pAd, UINT32 ac, UINT32 type)
{
	UINT32 addr = 0, cr_val, mask = 0, shift = 0;

	if (ac <= WMM_PARAM_AC_3)
	{
		switch (type)
        {
			case WMM_PARAM_TXOP:
				addr = wmm_txop_mask[ac].Address;
				mask = wmm_txop_mask[ac].Mask;
				shift = wmm_txop_mask[ac].Shift;
				break;
			case WMM_PARAM_AIFSN:
				addr = wmm_aifsn_mask[ac].Address;
				mask = wmm_aifsn_mask[ac].Mask;
				shift = wmm_aifsn_mask[ac].Shift;
				break;
			case WMM_PARAM_CWMIN:
				addr = wmm_cwmin_mask[ac].Address;
				mask = wmm_cwmin_mask[ac].Mask;
				shift = wmm_cwmin_mask[ac].Shift;
				break;
			case WMM_PARAM_CWMAX:
				addr = wmm_cwmax_mask[ac].Address;
				mask = wmm_cwmax_mask[ac].Mask;
				shift = wmm_cwmax_mask[ac].Shift;
				break;
			default:
				DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Error type=%d\n", __FUNCTION__, __LINE__, type));
				break;
        }
    }

	if (addr && mask)
    {
        RTMP_IO_READ32(pAd, addr, &cr_val);
        cr_val = (cr_val & mask) >> shift;

        return cr_val;
    }

    return 0xdeadbeef;
}


INT AsicSetWmmParam(RTMP_ADAPTER *pAd, UINT ac, UINT type, UINT val)
{
	CMD_EDCA_SET_T EdcaParam;
	P_TX_AC_PARAM_T pAcParam;

	NdisZeroMemory(&EdcaParam,sizeof(CMD_EDCA_SET_T));
	EdcaParam.ucTotalNum = 1;
	pAcParam = &EdcaParam.rAcParam[0];
	pAcParam->ucAcNum = (UINT8)ac;

	switch (type) {
	case WMM_PARAM_TXOP:
		pAcParam->ucVaildBit = CMD_EDCA_TXOP_BIT;
		pAcParam->u2Txop = (UINT16)val;
		break;
	case WMM_PARAM_AIFSN:
		pAcParam->ucVaildBit = CMD_EDCA_AIFS_BIT;
		pAcParam->ucAifs = (UINT8)val;
		break;
	case WMM_PARAM_CWMIN:
		pAcParam->ucVaildBit = CMD_EDCA_WIN_MIN_BIT;
		pAcParam->ucWinMin = (UINT8)val;
		break;
	case WMM_PARAM_CWMAX:
		pAcParam->ucVaildBit = CMD_EDCA_WIN_MAX_BIT;
		pAcParam->u2WinMax = (UINT16)val;
		break;
	default:
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Error type=%d\n", __FUNCTION__, __LINE__, type));
		break;
	}

    NdisCopyMemory(&pAd->CurrEdcaParam[ac], pAcParam, sizeof(TX_AC_PARAM_T));

	CmdEdcaParameterSet(pAd,EdcaParam);

	return TRUE;
}


INT AsicSetAllWmmParam(RTMP_ADAPTER *pAd,PEDCA_PARM pEdcaParm)
{
	CMD_EDCA_SET_T EdcaParam;
	P_TX_AC_PARAM_T pAcParam;
	UINT32 ac=0,index=0;;

	NdisZeroMemory(&EdcaParam,sizeof(CMD_EDCA_SET_T));
	EdcaParam.ucTotalNum = CMD_EDCA_AC_MAX;

	for ( ac=0; ac < CMD_EDCA_AC_MAX;  ac++)
	{
		index = wmm_aci_2_hw_ac_queue[ac];
		pAcParam = &EdcaParam.rAcParam[index];
		pAcParam->ucVaildBit = CMD_EDCA_ALL_BITS;
		pAcParam->ucAcNum = (UINT8)ac;
		pAcParam->ucAifs = pEdcaParm->Aifsn[index];
		pAcParam->ucWinMin= (1 << pEdcaParm->Cwmin[index]) -1;
		pAcParam->u2WinMax= (1 << pEdcaParm->Cwmax[index]) -1;
#ifdef CONFIG_STA_SUPPORT
		if(ac == QID_AC_VI && pAcParam->ucAifs == 10)
		{
			EdcaParam.rAcParam[QID_AC_BE].u2Txop = 0x60;
		}
#endif
		pAcParam->u2Txop= pEdcaParm->Txop[index];

        NdisCopyMemory(&pAd->CurrEdcaParam[index], pAcParam, sizeof(TX_AC_PARAM_T));
	}
	CmdEdcaParameterSet(pAd,EdcaParam);

	return TRUE;
}


/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicSetEdcaParm(RTMP_ADAPTER *pAd, PEDCA_PARM pEdcaParm)
{
	int i;

	if ((pEdcaParm == NULL) || (pEdcaParm->bValid == FALSE))
	{
		DBGPRINT(RT_DEBUG_TRACE,("AsicSetEdcaParm\n"));
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_WMM_INUSED);
		for (i=0; i < MAX_LEN_OF_MAC_TABLE; i++)
		{
			if (IS_ENTRY_CLIENT(&pAd->MacTab.Content[i]) || IS_ENTRY_APCLI(&pAd->MacTab.Content[i]))
				CLIENT_STATUS_CLEAR_FLAG(&pAd->MacTab.Content[i], fCLIENT_STATUS_WMM_CAPABLE);
		}

		NdisZeroMemory(&pAd->CommonCfg.APEdcaParm, sizeof(EDCA_PARM));

#ifdef WMM_ACM_SUPPORT
		/* no ACM function due to no WMM */
		ACMP_EnableFlagReset(pAd, 0, 0, 0, 0);
#endif /* WMM_ACM_SUPPORT */
	}
	else
	{
		OPSTATUS_SET_FLAG(pAd, fOP_STATUS_WMM_INUSED);

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			if (INFRA_ON(pAd))
				CLIENT_STATUS_SET_FLAG(&pAd->MacTab.Content[BSSID_WCID], fCLIENT_STATUS_WMM_CAPABLE);
		}
#endif /* CONFIG_STA_SUPPORT */

		NdisMoveMemory(&pAd->CommonCfg.APEdcaParm, pEdcaParm, sizeof(EDCA_PARM));
		if (!ADHOC_ON(pAd))
		{
			DBGPRINT(RT_DEBUG_TRACE,("EDCA [#%d]: AIFSN CWmin CWmax  TXOP(us)  ACM\n", pEdcaParm->EdcaUpdateCount));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_BE      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[0],
									 pEdcaParm->Cwmin[0],
									 pEdcaParm->Cwmax[0],
									 pEdcaParm->Txop[0]<<5,
									 pEdcaParm->bACM[0]));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_BK      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[1],
									 pEdcaParm->Cwmin[1],
									 pEdcaParm->Cwmax[1],
									 pEdcaParm->Txop[1]<<5,
									 pEdcaParm->bACM[1]));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_VI      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[2],
									 pEdcaParm->Cwmin[2],
									 pEdcaParm->Cwmax[2],
									 pEdcaParm->Txop[2]<<5,
									 pEdcaParm->bACM[2]));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_VO      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[3],
									 pEdcaParm->Cwmin[3],
									 pEdcaParm->Cwmax[3],
									 pEdcaParm->Txop[3]<<5,
									 pEdcaParm->bACM[3]));
		}

#ifdef WMM_ACM_SUPPORT
		ACMP_EnableFlagReset(pAd, pEdcaParm->bACM[0],
							pEdcaParm->bACM[1],
							pEdcaParm->bACM[2],
							pEdcaParm->bACM[3]);
#endif /* WMM_ACM_SUPPORT */
		if (pAd->chipCap.hif_type == HIF_MT)
		{
			AsicSetAllWmmParam(pAd,pEdcaParm);
		}
		}
	// TODO: shiang-MT7603, fix me after BurstMode is finished!
#if 0
	{
		UINT32 Ac0Cfg;
		pAd->CommonCfg.RestoreBurstMode = Ac0Cfg;
	}
#endif
}


#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_USB
#ifdef DOT11_N_SUPPORT
INT sta_wmm_adjust(RTMP_ADAPTER *pAd)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	}

	return FALSE;

#if 0
#ifdef RELEASE_EXCLUDE
	/*
		For 1X1 STA pass 11n wifi wmm, need to change txop per case;
		1x1 device for 802.11n WMM Test
	*/
#endif /* RELEASE_EXCLUDE */
	// TODO: shiang-usw, revise this if possible!!
	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
	{

		if ((pAd->Antenna.field.TxPath == 1)&&
		(pAd->StaActive.SupportedPhyInfo.bHtEnable == TRUE) &&
			(pAd->CommonCfg.BACapability.field.Policy == BA_NOTUSE)
#ifdef RT3290
		    && (!IS_RT3290(pAd))
#endif /* RT3290 */
		)
		{
			EDCA_AC_CFG_STRUC	Ac0Cfg;
			EDCA_AC_CFG_STRUC	Ac2Cfg;
			RTMP_IO_READ32(pAd, EDCA_AC2_CFG, &Ac2Cfg.word);
			RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &Ac0Cfg.word);

			if ((pAd->RalinkCounters.OneSecOsTxCount[QID_AC_VO] == 0) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_BK] == 0) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_BE] < 50) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_VI] >= 1000))
			{
			/*5.2.27/28 T7: Total throughput need to ~36Mbps*/
				if (Ac2Cfg.field.Aifsn!=0xc)
				{
					Ac2Cfg.field.Aifsn = 0xc;
					RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, Ac2Cfg.word, FALSE);
				}
			}
#ifdef RELEASE_EXCLUDE
			/* Patch from Windows driver to pass 5.2.39 */
#endif /* RELEASE_EXCLUDE */
			else if ( IS_RT3070(pAd) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_VO] == 0) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_BK] == 0) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_VI] == 0) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_BE] >= 300)&&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_BE] <= 1500)&&
			(pAd->CommonCfg.IOTestParm.bRTSLongProtOn==FALSE))
			{
#ifdef RELEASE_EXCLUDE
			/* 5.2.28 T4. Too many retry if no TxOP.(RT3070 bug ???)*/
#endif /* RELEASE_EXCLUDE */
				if (Ac0Cfg.field.AcTxop!=0x07)
				{
					Ac0Cfg.field.AcTxop = 0x07;
					Ac0Cfg.field.Aifsn = 0xc;
					RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Ac0Cfg.word, FALSE);
				}
			}
			else if ((pAd->RalinkCounters.OneSecOsTxCount[QID_AC_VO] == 0) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_BK] == 0) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_VI] == 0) &&
			(pAd->RalinkCounters.OneSecOsTxCount[QID_AC_BE] < 10))
			{
			/* restore default parameter of BE*/
				if ((Ac0Cfg.field.Aifsn!=3) ||(Ac0Cfg.field.AcTxop!=0))
				{
					if(Ac0Cfg.field.Aifsn!=3)
						Ac0Cfg.field.Aifsn = 3;
					if(Ac0Cfg.field.AcTxop!=0)
						Ac0Cfg.field.AcTxop = 0;
					RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Ac0Cfg.word);
				}

			/* restore default parameter of VI*/
				if (Ac2Cfg.field.Aifsn!=0x3)
				{
					Ac2Cfg.field.Aifsn = 0x3;
					RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, Ac2Cfg.word);
				}

			}
		}
	}
#endif
	return TRUE;
}
#endif /* RTMP_MAC_USB */
#endif /* DOT11_N_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
/*
	Wirte non-zero value to AC0 TXOP to boost performace
	To pass WMM, AC0 TXOP must be zero.
	It is necessary to turn AC0 TX_OP dynamically.
*/
VOID dynamic_tune_be_tx_op(RTMP_ADAPTER *pAd, ULONG nonBEpackets)
{
	//UINT32 RegValue;
	//AC_TXOP_CSR0_STRUC csr0;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
//		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
//							__FUNCTION__, __LINE__));
	}

	return;

	/* pAd->OneSecondnonBEpackets = 0; fix coverity issue, not run here, previous direct return. */
}
#endif /* CONFIG_AP_SUPPORT */


INT AsicSetRetryLimit(RTMP_ADAPTER *pAd, UINT32 type, UINT32 limit)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	}

	return FALSE;
}


UINT32 AsicGetRetryLimit(RTMP_ADAPTER *pAd, UINT32 type)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	}

	return 0;
}


/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicSetSlotTime(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN bUseShortSlotTime,
	IN UCHAR channel)
{
	UINT32 SlotTime, SifsTime;

#ifdef CONFIG_STA_SUPPORT
	if (pAd->CommonCfg.Channel > 14)
		bUseShortSlotTime = TRUE;
#endif /* CONFIG_STA_SUPPORT */

	if (bUseShortSlotTime && OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED))
		return;
	else if ((!bUseShortSlotTime) && (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED)))
		return;

	if (bUseShortSlotTime)
		OPSTATUS_SET_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);
	else
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);

	SlotTime = (bUseShortSlotTime)? 9 : 20;

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* force using short SLOT time for FAE to demo performance when TxBurst is ON*/
		if (((pAd->StaActive.SupportedPhyInfo.bHtEnable == FALSE) && (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_WMM_INUSED)))
#ifdef DOT11_N_SUPPORT
			|| ((pAd->StaActive.SupportedPhyInfo.bHtEnable == TRUE) && (pAd->CommonCfg.BACapability.field.Policy == BA_NOTUSE))
#endif /* DOT11_N_SUPPORT */
			)
		{
			/* In this case, we will think it is doing Wi-Fi test*/
			/* And we will not set to short slot when bEnableTxBurst is TRUE.*/
		}
		else if (pAd->CommonCfg.bEnableTxBurst)
		{
			OPSTATUS_SET_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);
			SlotTime = 9;
		}
	}
#endif /* CONFIG_STA_SUPPORT */


	/* For some reasons, always set it to short slot time.*/
	/* ToDo: Should consider capability with 11B*/
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->StaCfg.BssType == BSS_ADHOC)
		{
			OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);
			SlotTime = 20;
		}
	}
#endif /* CONFIG_STA_SUPPORT */



	if (channel > 14)
		SifsTime = SIFS_TIME_5G;
	else
		SifsTime = SIFS_TIME_24G;

	CmdSlotTimeSet(pAd, (UINT8)SlotTime,
		(UINT8)SifsTime, RIFS_TIME, EIFS_TIME);


	DBGPRINT(RT_DEBUG_TRACE, ("%s: SlotTime =%u us, SIFS Time = %u us,\
							RIFS Time = %d, EIFS Time = %d\n",\
							__FUNCTION__, SlotTime, SifsTime, RIFS_TIME, EIFS_TIME));
}


#define MAX_RX_PKT_LENGTH   0x400 /* WORD(4 Bytes) unit */
INT AsicSetMacMaxLen(RTMP_ADAPTER *pAd)
{
	// TODO: shiang-7603
	UINT32 val;

	// Rx max packet length
	RTMP_IO_READ32(pAd, DMA_DCR0, &val);
	val &= (~0xfffc);
	val |= (MAX_RX_PKT_LENGTH << 2);
	RTMP_IO_WRITE32(pAd, DMA_DCR0, val);
	DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Set the Max RxPktLen=%d!\n",
				__FUNCTION__, __LINE__, MAX_RX_PKT_LENGTH));

	return TRUE;
}


#ifdef CONFIG_AP_SUPPORT
VOID RTMPGetTxTscFromAsic(RTMP_ADAPTER *pAd, UCHAR apidx, UCHAR *pTxTsc)
{
	USHORT Wcid;
	USHORT offset;
	UCHAR IvEiv[8];
	INT i;

	/* Sanity check of apidx */
	if (apidx >= MAX_MBSSID_NUM(pAd))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("RTMPGetTxTscFromAsic : invalid apidx(%d)\n", apidx));
		return;
	}

	if (pAd->chipCap.hif_type == HIF_MT) {
		struct wtbl_entry tb_entry;
		UINT32 addr = 0, val = 0;

		GET_GroupKey_WCID(pAd, Wcid, apidx);
		NdisZeroMemory(&tb_entry, sizeof(tb_entry));
		if (mt_wtbl_get_entry234(pAd, (UCHAR)Wcid, &tb_entry) == FALSE) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s():Cannot found WTBL2/3/4 for WCID(%d)\n",
						__FUNCTION__, Wcid));
			return;
		}
		addr = pAd->mac_ctrl.wtbl_base_addr[1] + Wcid * pAd->mac_ctrl.wtbl_entry_size[1];
		RTMP_IO_READ32(pAd, addr, &val);

		*pTxTsc     = val & 0xff;
        *(pTxTsc+1) = (val >> 8) & 0xff;
        *(pTxTsc+2) = (val >> 16) & 0xff;
        *(pTxTsc+3) = (val >> 24) & 0xff;

		RTMP_IO_READ32(pAd, addr+4, &val);
		*(pTxTsc+4) = val & 0xff;
        *(pTxTsc+5) = (val >> 8) & 0xff;

		DBGPRINT(RT_DEBUG_INFO, ("RTMPGetTxTscFromAsic : WCID(%d) TxTsc 0x%02x-0x%02x-0x%02x-0x%02x-0x%02x-0x%02x \n",
									Wcid, *pTxTsc, *(pTxTsc+1), *(pTxTsc+2), *(pTxTsc+3), *(pTxTsc+4), *(pTxTsc+5)));

		return;
	}

	/* Initial value */
	NdisZeroMemory(IvEiv, 8);
	NdisZeroMemory(pTxTsc, 6);

	/* Get apidx for this BSSID */
	GET_GroupKey_WCID(pAd, Wcid, apidx);

	/* When the group rekey action is triggered, a count-down(3 seconds) is started.
	   During the count-down, use the initial PN as TSC.
	   Otherwise, get the IVEIV from ASIC. */
	if (pAd->ApCfg.MBSSID[apidx].RekeyCountDown > 0)
	{
		/*
		In IEEE 802.11-2007 8.3.3.4.3 described :
		The PN shall be implemented as a 48-bit monotonically incrementing
		non-negative integer, initialized to 1 when the corresponding
		temporal key is initialized or refreshed. */
		IvEiv[0] = 1;
	}
	else
	{
		UINT32 temp1, temp2;
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
		/* Read IVEIV from Asic */
		offset = (USHORT)(iveiv_tb_base + (Wcid * iveiv_tb_size));

		/* Use Read32 to avoid endian problem */
		RTMP_IO_READ32(pAd, offset, &temp1);
		RTMP_IO_READ32(pAd, offset+4, &temp2);
		for ( i=0; i<4; i++)
		{
			IvEiv[i] = (UCHAR)(temp1 >> (i*8));
			IvEiv[i+4] = (UCHAR)(temp2 >> (i*8));
		}
	}

	/* Record current TxTsc */
	if (pAd->ApCfg.MBSSID[apidx].wdev.GroupKeyWepStatus == Ndis802_11AESEnable)
	{	/* AES */
		*pTxTsc 	= IvEiv[0];
		*(pTxTsc+1) = IvEiv[1];
		*(pTxTsc+2) = IvEiv[4];
		*(pTxTsc+3) = IvEiv[5];
		*(pTxTsc+4) = IvEiv[6];
		*(pTxTsc+5) = IvEiv[7];
	}
	else
	{	/* TKIP */
		*pTxTsc 	= IvEiv[2];
		*(pTxTsc+1) = IvEiv[0];
		*(pTxTsc+2) = IvEiv[4];
		*(pTxTsc+3) = IvEiv[5];
		*(pTxTsc+4) = IvEiv[6];
		*(pTxTsc+5) = IvEiv[7];
	}
	DBGPRINT(RT_DEBUG_TRACE, ("RTMPGetTxTscFromAsic : WCID(%d) TxTsc 0x%02x-0x%02x-0x%02x-0x%02x-0x%02x-0x%02x \n",
									Wcid, *pTxTsc, *(pTxTsc+1), *(pTxTsc+2), *(pTxTsc+3), *(pTxTsc+4), *(pTxTsc+5)));


}
#endif /* CONFIG_AP_SUPPORT */


/*
	========================================================================
	Description:
		Add Shared key information into ASIC.
		Update shared key, TxMic and RxMic to Asic Shared key table
		Update its cipherAlg to Asic Shared key Mode.

    Return:
	========================================================================
*/
VOID AsicAddSharedKeyEntry(
	IN PRTMP_ADAPTER 	pAd,
	IN UCHAR		 	BssIndex,
	IN UCHAR		 	KeyIdx,
	IN PCIPHER_KEY		pCipherKey)
{
	ULONG offset; /*, csr0;*/
	//SHAREDKEY_MODE_STRUC csr1;
#if defined(RLT_MAC) || defined(RTMP_MAC)
	UCHAR org_bssindex = BssIndex;
#endif /* defined(RLT_MAC) || defined(RTMP_MAC) */
#ifdef RTMP_MAC_PCI
	INT   i;
#endif /* RTMP_MAC_PCI */

	PUCHAR		pKey = pCipherKey->Key;
	PUCHAR		pTxMic = pCipherKey->TxMic;
	PUCHAR		pRxMic = pCipherKey->RxMic;
	//UCHAR		CipherAlg = pCipherKey->CipherAlg;

	DBGPRINT(RT_DEBUG_TRACE, ("AsicAddSharedKeyEntry BssIndex=%d, KeyIdx=%d\n", BssIndex,KeyIdx));

	//org_bssindex = BssIndex;
	if (BssIndex >= 8)
		BssIndex -= 8;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}

	{
		/* fill key material - key + TX MIC + RX MIC*/
		UINT32 share_key_base = 0, share_key_size = 0;
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
			if (org_bssindex >= 8)
				share_key_base = RLT_SHARED_KEY_TABLE_BASE_EXT;
			else
				share_key_base = RLT_SHARED_KEY_TABLE_BASE;
			share_key_size = RLT_HW_KEY_ENTRY_SIZE;
		}
#endif /* RLT_MAC */
#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			if (org_bssindex >= 8)
				share_key_base = SHARED_KEY_TABLE_BASE_EXT;
			else
				share_key_base = SHARED_KEY_TABLE_BASE;
			share_key_size = HW_KEY_ENTRY_SIZE;
		}
#endif /* RTMP_MAC */

		offset = share_key_base + (4*BssIndex + KeyIdx)*share_key_size;
#ifdef RTMP_MAC_PCI
		{
			for (i=0; i<MAX_LEN_OF_SHARE_KEY; i++)
			{
				RTMP_IO_WRITE8(pAd, offset + i, pKey[i]);
			}

			offset += MAX_LEN_OF_SHARE_KEY;
			if (pTxMic)
			{
				for (i=0; i<8; i++)
				{
					RTMP_IO_WRITE8(pAd, offset + i, pTxMic[i]);
				}
			}

			offset += 8;
			if (pRxMic)
			{
				for (i=0; i<8; i++)
				{
					RTMP_IO_WRITE8(pAd, offset + i, pRxMic[i]);
				}
			}
		}
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
		{
			RTUSBMultiWrite(pAd, (USHORT)offset, pKey, MAX_LEN_OF_SHARE_KEY, FALSE);

			offset += MAX_LEN_OF_SHARE_KEY;
			if (pTxMic)
			{
				RTUSBMultiWrite(pAd, (USHORT)offset, pTxMic, 8, FALSE);
			}

			offset += 8;
			if (pRxMic)
			{
				RTUSBMultiWrite(pAd, (USHORT)offset, pRxMic, 8, FALSE);
			}
		}
#endif /* RTMP_MAC_USB */
	}

	{
		//UINT32 share_key_mode_base = 0;
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
			if (org_bssindex >= 8)
				share_key_mode_base = RLT_SHARED_KEY_MODE_BASE_EXT;
			else
				share_key_mode_base= RLT_SHARED_KEY_MODE_BASE;
		}
#endif /* RLT_MAC */
#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			if (org_bssindex >= 8)
				share_key_mode_base = SHARED_KEY_MODE_BASE_EXT;
			else
				share_key_mode_base = SHARED_KEY_MODE_BASE;
		}
#endif /* RTMP_MAC */

		/* Update cipher algorithm. WSTA always use BSS0*/
		//RTMP_IO_READ32(pAd, share_key_mode_base + 4 * (BssIndex/2), &csr1.word);
		//DBGPRINT(RT_DEBUG_TRACE,("Read: SHARED_KEY_MODE_BASE at this Bss[%d] KeyIdx[%d]= 0x%x \n", BssIndex,KeyIdx, csr1.word));
		//DBGPRINT(RT_DEBUG_TRACE,("Write: SHARED_KEY_MODE_BASE at this Bss[%d] = 0x%x \n", BssIndex, csr1.word));
		//RTMP_IO_WRITE32(pAd, share_key_mode_base+ 4 * (BssIndex/2), csr1.word);
	}
}


/*	IRQL = DISPATCH_LEVEL*/
VOID AsicRemoveSharedKeyEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 BssIndex,
	IN UCHAR		 KeyIdx)
{
	/*ULONG SecCsr0;*/
	//SHAREDKEY_MODE_STRUC csr1;

	DBGPRINT(RT_DEBUG_TRACE,("AsicRemoveSharedKeyEntry: #%d \n", BssIndex*4 + KeyIdx));

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}

	{
		//UINT32 share_key_mode_base = 0;
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT)
			share_key_mode_base= RLT_SHARED_KEY_MODE_BASE;
#endif /* RLT_MAC */
#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP)
			share_key_mode_base = SHARED_KEY_MODE_BASE;
#endif /* RTMP_MAC */

		//DBGPRINT(RT_DEBUG_TRACE,("Write: SHARED_KEY_MODE_BASE at this Bss[%d] = 0x%x \n", BssIndex, csr1.word));
		//RTMP_IO_WRITE32(pAd, share_key_mode_base+4*(BssIndex/2), csr1.word);
	}
	ASSERT(BssIndex < 4);
	ASSERT(KeyIdx < 4);

}


VOID AsicUpdateWCIDIVEIV(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN ULONG        uIV,
	IN ULONG        uEIV)
{
	ULONG	offset;
	UINT32 iveiv_tb_base = 0, iveiv_tb_size = 0;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_INFO, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}

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

	offset = iveiv_tb_base + (WCID * iveiv_tb_size);

	RTMP_IO_WRITE32(pAd, offset, uIV);
	RTMP_IO_WRITE32(pAd, offset + 4, uEIV);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: wcid(%d) 0x%08lx, 0x%08lx \n",
									__FUNCTION__, WCID, uIV, uEIV));
}


/**
 * Wtbl2RateTableUpdate
 *
 *
 *
 */
VOID Wtbl2RateTableUpdate(RTMP_ADAPTER *pAd, UCHAR ucWcid, UINT32 u4Wtbl2D9, UINT32* Rate)
{
	WTBL2_RATE_TABLE_UPDATE	rWtbl2RateTableUpdate = {0};

	rWtbl2RateTableUpdate.ucWcid = ucWcid;
	rWtbl2RateTableUpdate.u4Wtbl2D9 = u4Wtbl2D9;
	RTMPMoveMemory(rWtbl2RateTableUpdate.rate, Rate, sizeof(rWtbl2RateTableUpdate.rate));

	RTEnqueueInternalCmd(pAd, CMDTHREAD_PERODIC_CR_ACCESS_WTBL_RATE_TABLE_UPDATE, &rWtbl2RateTableUpdate, sizeof(rWtbl2RateTableUpdate));
}

//VOID MtCmdWtbl2RateTableUpdate(RTMP_ADAPTER *pAd, UCHAR ucWcid, UINT32 u4Wtbl2D9, UINT32* Rate)
NTSTATUS MtCmdWtbl2RateTableUpdate(RTMP_ADAPTER *pAd, PCmdQElmt CMDQelmt)
{
    UINT32 u4RegVal;
    UCHAR ucWaitCnt = 0;
#ifdef RTMP_USB_SUPPORT
    UINT32 Flags;
#endif

	PWTBL2_RATE_TABLE_UPDATE pWtbl2RateTableUpdate = (PWTBL2_RATE_TABLE_UPDATE)(CMDQelmt->buffer);
	UCHAR ucWcid = pWtbl2RateTableUpdate->ucWcid;
	UINT32 u4Wtbl2D9 = pWtbl2RateTableUpdate->u4Wtbl2D9;
	UINT32* Rate = pWtbl2RateTableUpdate->rate;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): H/W in PM4, return\n", __FUNCTION__, __LINE__));
		return NDIS_STATUS_SUCCESS;
	}

#ifdef RTMP_USB_SUPPORT
    RTMP_SEM_EVENT_WAIT(&pAd->IndirectUpdateLock, Flags);
	if(Flags != 0)
		DBGPRINT(RT_DEBUG_ERROR, ("%s:(%d) RTMP_SEM_EVENT_WAIT failed!\n",__FUNCTION__,Flags));
#endif

#ifdef RTMP_PCI_SUPPORT
	NdisAcquireSpinLock(&pAd->IndirectUpdateLock);
#endif

    do {
        RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &u4RegVal);
        if ((u4RegVal & 0x10000)  == 0)
            break;
        ucWaitCnt++;
        RtmpusecDelay(50);
    }while (ucWaitCnt < 100);

    if (ucWaitCnt == 100) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
                __FUNCTION__, u4RegVal));
#ifdef RTMP_USB_SUPPORT
		RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
		NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
        return NDIS_STATUS_SUCCESS;
    }

	RTMP_IO_WRITE32(pAd, WTBL_ON_RIUCR0, u4Wtbl2D9);
	u4RegVal = (Rate[0] | (Rate[1] << 12) | (Rate[2] << 24));
	RTMP_IO_WRITE32(pAd, WTBL_ON_RIUCR1, u4RegVal);
	u4RegVal = ((Rate[2] >> 8) | (Rate[3] << 4) | (Rate[4] << 16) | (Rate[5] << 28));
	RTMP_IO_WRITE32(pAd, WTBL_ON_RIUCR2, u4RegVal);
	u4RegVal = ((Rate[5] >> 4) | (Rate[6] << 8) | (Rate[7] << 20));
	RTMP_IO_WRITE32(pAd, WTBL_ON_RIUCR3, u4RegVal);

	// TODO: shiang-MT7603, shall we also clear TxCnt/RxCnt/AdmCnt here??
	u4RegVal = (ucWcid | (1 << 13) | (1 << 14));
	RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, u4RegVal);

#ifdef RTMP_PCI_SUPPORT
	NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

	return NDIS_STATUS_SUCCESS;
}


/**
 * Wtbl2TxRateCounterGet
 *
 *
 *
 */
VOID Wtbl2TxRateCounterGet(RTMP_ADAPTER *pAd, UCHAR ucWcid, TX_CNT_INFO *tx_cnt_info)
{
	UINT32 u4RegVal;
	UCHAR ucWaitCnt = 0;
	struct rtmp_mac_ctrl *wtbl_ctrl;
	UCHAR wtbl_idx;
	UINT32 addr/*, val[16]*/;
#ifdef RTMP_USB_SUPPORT
	UINT32 Flags;
#endif

	wtbl_ctrl = &pAd->mac_ctrl;
	if (wtbl_ctrl->wtbl_entry_cnt[0] > 0)
		wtbl_idx = (ucWcid < wtbl_ctrl->wtbl_entry_cnt[0] ? ucWcid : wtbl_ctrl->wtbl_entry_cnt[0] - 1);
	else {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():PSE not init yet!\n", __FUNCTION__));
		return;
	}

	addr = pAd->mac_ctrl.wtbl_base_addr[1] + wtbl_idx * pAd->mac_ctrl.wtbl_entry_size[1];
	RTMP_IO_READ32(pAd, addr + 5 * 4, &(tx_cnt_info->wtbl_2_d5.word));
	RTMP_IO_READ32(pAd, addr + 6 * 4, &(tx_cnt_info->wtbl_2_d6.word));
	RTMP_IO_READ32(pAd, addr + 7 * 4, &(tx_cnt_info->wtbl_2_d7.word));
	RTMP_IO_READ32(pAd, addr + 8 * 4, &(tx_cnt_info->wtbl_2_d8.word));
	RTMP_IO_READ32(pAd, addr + 9 * 4, &(tx_cnt_info->wtbl_2_d9.word));

#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_WAIT(&pAd->IndirectUpdateLock, Flags);
	if(Flags != 0)
		DBGPRINT(RT_DEBUG_ERROR, ("%s:(%d) RTMP_SEM_EVENT_WAIT failed!\n",__FUNCTION__,Flags));

#endif

#ifdef RTMP_PCI_SUPPORT
	NdisAcquireSpinLock(&pAd->IndirectUpdateLock);
#endif

	do {
		RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &u4RegVal);
		if ((u4RegVal & 0x10000)  == 0)
			break;
		ucWaitCnt++;
		RtmpusecDelay(50);
	}while (ucWaitCnt < 100);

	if (ucWaitCnt == 100) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
			__FUNCTION__, u4RegVal));
#ifdef RTMP_USB_SUPPORT
		RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
		NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
		return;
	}

	u4RegVal = (ucWcid | (1 << 14));
	RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, u4RegVal);

#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
	NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
}


/**
 * Wtbl2RcpiGet
 *
 *
 *
 */
VOID Wtbl2RcpiGet(RTMP_ADAPTER *pAd, UCHAR ucWcid, union WTBL_2_DW13 *wtbl_2_d13)
{
	struct rtmp_mac_ctrl *wtbl_ctrl;
	UCHAR wtbl_idx;
	UINT32 addr/*, val[16]*/;
#ifdef RTMP_SDIO_SUPPORT
	return;
#endif /*leonardo remark it  temporarily*/
	wtbl_ctrl = &pAd->mac_ctrl;
	if (wtbl_ctrl->wtbl_entry_cnt[0] > 0)
		wtbl_idx = (ucWcid < wtbl_ctrl->wtbl_entry_cnt[0] ? ucWcid : wtbl_ctrl->wtbl_entry_cnt[0] - 1);
	else {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():PSE not init yet!\n", __FUNCTION__));
		return;
	}

	addr = pAd->mac_ctrl.wtbl_base_addr[1] + wtbl_idx * pAd->mac_ctrl.wtbl_entry_size[1];
	RTMP_IO_READ32(pAd, addr + 13 * 4, &(wtbl_2_d13->word));
}


VOID AsicTxCntUpdate(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *pEntry, MT_TX_COUNTER *pTxInfo)
{
	TX_CNT_INFO tx_cnt_info;
	UINT32 TxSuccess;

	if (IS_VALID_ENTRY(pEntry)) {
		Wtbl2TxRateCounterGet(pAd, pEntry->wcid, &tx_cnt_info);
		pTxInfo->TxCount = tx_cnt_info.wtbl_2_d7.field.current_bw_tx_cnt ;
		pTxInfo->TxCount += tx_cnt_info.wtbl_2_d8.field.other_bw_tx_cnt;
		pTxInfo->TxFailCount = tx_cnt_info.wtbl_2_d7.field.current_bw_fail_cnt ;
		pTxInfo->TxFailCount += tx_cnt_info.wtbl_2_d8.field.other_bw_fail_cnt;

		pTxInfo->Rate1TxCnt = (UINT16)tx_cnt_info.wtbl_2_d5.field.rate_1_tx_cnt;
		pTxInfo->Rate1FailCnt = (UINT16)tx_cnt_info.wtbl_2_d5.field.rate_1_fail_cnt;
		pTxInfo->Rate2TxCnt = (UCHAR)tx_cnt_info.wtbl_2_d6.field.rate_2_tx_cnt;
		pTxInfo->Rate3TxCnt = (UCHAR)tx_cnt_info.wtbl_2_d6.field.rate_3_tx_cnt;
		pTxInfo->Rate4TxCnt = (UCHAR)tx_cnt_info.wtbl_2_d6.field.rate_4_tx_cnt;
		pTxInfo->Rate5TxCnt = (UCHAR)tx_cnt_info.wtbl_2_d6.field.rate_5_tx_cnt;

		pTxInfo->RateIndex = (UCHAR)tx_cnt_info.wtbl_2_d9.field.rate_idx;
/*
		if ( pTxInfo->TxFailCount == 0 )
			pEntry->OneSecTxNoRetryOkCount += pTxInfo->TxSuccessCount;
		else
		{
			pEntry->OneSecTxRetryOkCount += pTxInfo->TxSuccessCount;
			pEntry->OneSecTxFailCount += pTxInfo->TxFailCount;
		}
*/

		TxSuccess = pTxInfo->TxCount -pTxInfo->TxFailCount;

		if ( pTxInfo->TxFailCount == 0 )
			pAd->RalinkCounters.OneSecTxNoRetryOkCount += pTxInfo->TxCount;
		else
			pAd->RalinkCounters.OneSecTxRetryOkCount += pTxInfo->TxCount;
		pAd->RalinkCounters.OneSecTxFailCount += pTxInfo->TxFailCount;

#ifdef STATS_COUNT_SUPPORT
		pAd->WlanCounters.TransmittedFragmentCount.u.LowPart += TxSuccess;
		pAd->WlanCounters.FailedCount.u.LowPart += pTxInfo->TxFailCount;
#endif /* STATS_COUNT_SUPPORT */
	}

}


VOID AsicRssiUpdate(RTMP_ADAPTER *pAd)
{
	union WTBL_2_DW13 wtbl_2_d13;
	MAC_TABLE_ENTRY *pEntry;
	UINT16 i;
	INT32 Rssi0 = 0, Rssi1 = 0, Rssi2 = 0;
	INT32 TotalRssi0 = 0, TotalRssi1 = 0, TotalRssi2 = 0;

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if ( pAd->MacTab.Size == 0 )
		{
			pEntry = &pAd->MacTab.Content[MCAST_WCID];
			Wtbl2RcpiGet(pAd, pEntry->wcid, &wtbl_2_d13);

			Rssi0 = (wtbl_2_d13.field.resp_rcpi_0 - 220) / 2;
			Rssi1 = (wtbl_2_d13.field.resp_rcpi_1 - 220) / 2;
			Rssi2 = 0;

			pEntry->RssiSample.AvgRssi[0] =
				pEntry->RssiSample.LastRssi[0] = (CHAR)Rssi0;
			pEntry->RssiSample.AvgRssi[1] =
				pEntry->RssiSample.LastRssi[1] = (CHAR)Rssi1;
			pEntry->RssiSample.AvgRssi[2] =
				pEntry->RssiSample.LastRssi[2] = (CHAR)Rssi2;

			pAd->ApCfg.RssiSample.AvgRssi[0] = (CHAR)Rssi0;
			pAd->ApCfg.RssiSample.AvgRssi[1] = (CHAR)Rssi1;
			pAd->ApCfg.RssiSample.AvgRssi[2] = (CHAR)Rssi2;

			pAd->ApCfg.RssiSample.LastRssi[0] = (CHAR)Rssi0;
			pAd->ApCfg.RssiSample.LastRssi[1] = (CHAR)Rssi1;
			pAd->ApCfg.RssiSample.LastRssi[2] = (CHAR)Rssi2;
		}
		else
		{
			for (i = 1; i < MAX_LEN_OF_MAC_TABLE; i++)
			{
				pEntry = &pAd->MacTab.Content[i];
				if (IS_VALID_ENTRY(pEntry)) {
					Wtbl2RcpiGet(pAd, pEntry->wcid, &wtbl_2_d13);

					Rssi0 = (wtbl_2_d13.field.resp_rcpi_0 - 220) / 2;
					Rssi1 = (wtbl_2_d13.field.resp_rcpi_1 - 220) / 2;
					Rssi2 = 0;

					pEntry->RssiSample.AvgRssi[0] =
						pEntry->RssiSample.LastRssi[0] = (CHAR)Rssi0;
					pEntry->RssiSample.AvgRssi[1] =
						pEntry->RssiSample.LastRssi[1] = (CHAR)Rssi1;
					pEntry->RssiSample.AvgRssi[2] =
						pEntry->RssiSample.LastRssi[2] = (CHAR)Rssi2;

					TotalRssi0 += Rssi0;
					TotalRssi1 += Rssi1;
					TotalRssi2 += Rssi2;
				}
			}

			pAd->ApCfg.RssiSample.AvgRssi[0] =
				pAd->ApCfg.RssiSample.LastRssi[0] =
				(CHAR)(TotalRssi0 / pAd->MacTab.Size);
			pAd->ApCfg.RssiSample.AvgRssi[1] =
				pAd->ApCfg.RssiSample.LastRssi[1] =
				(CHAR)(TotalRssi1 / pAd->MacTab.Size);
			pAd->ApCfg.RssiSample.AvgRssi[2] =
				pAd->ApCfg.RssiSample.LastRssi[2] =
				(CHAR)(TotalRssi2 / pAd->MacTab.Size);
		}

	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		pEntry = &pAd->MacTab.Content[BSSID_WCID];
		Wtbl2RcpiGet(pAd, pEntry->wcid, &wtbl_2_d13);

		Rssi0 = (wtbl_2_d13.field.resp_rcpi_0 - 220) / 2;
		Rssi1 = (wtbl_2_d13.field.resp_rcpi_1 - 220) / 2;
		Rssi2 = 0;

		pEntry->RssiSample.AvgRssi[0] = pEntry->RssiSample.LastRssi[0] = (CHAR)Rssi0;
		pEntry->RssiSample.AvgRssi[1] = pEntry->RssiSample.LastRssi[1] = (CHAR)Rssi1;
		pEntry->RssiSample.AvgRssi[2] = pEntry->RssiSample.LastRssi[2] = (CHAR)Rssi2;

		pAd->StaCfg.RssiSample.AvgRssi[0] = (CHAR)Rssi0;
		pAd->StaCfg.RssiSample.AvgRssi[1] = (CHAR)Rssi1;
		pAd->StaCfg.RssiSample.AvgRssi[2] = (CHAR)Rssi2;

		pAd->StaCfg.RssiSample.LastRssi[0] = (CHAR)Rssi0;
		pAd->StaCfg.RssiSample.LastRssi[1] = (CHAR)Rssi1;
		pAd->StaCfg.RssiSample.LastRssi[2] = (CHAR)Rssi2;
	}
#endif /* CONFIG_STA_SUPPORT */
}


VOID AsicRcpiReset(RTMP_ADAPTER *pAd, UCHAR ucWcid)
{
    UINT32 u4RegVal;
    UCHAR ucWaitCnt = 0;

#ifdef RTMP_USB_SUPPORT
    UINT32 Flags;
#endif

#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_WAIT(&pAd->IndirectUpdateLock, Flags);
	if(Flags != 0)
		DBGPRINT(RT_DEBUG_ERROR, ("%s:(%d) RTMP_SEM_EVENT_WAIT failed!\n",__FUNCTION__,Flags));
#endif

#ifdef RTMP_PCI_SUPPORT
	NdisAcquireSpinLock(&pAd->IndirectUpdateLock);
#endif
    do {
        RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &u4RegVal);
        if ((u4RegVal & 0x10000)  == 0)
            break;
        ucWaitCnt++;
        RtmpusecDelay(50);
    }while (ucWaitCnt < 100);

    if (ucWaitCnt == 100) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
                __FUNCTION__, u4RegVal));
#ifdef RTMP_USB_SUPPORT
		RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
		NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
        return;
    }

    u4RegVal = (ucWcid | (1 << 15));
    RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, u4RegVal);

#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
	NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
}


VOID AsicSetSMPS(RTMP_ADAPTER *pAd, UCHAR wcid, UCHAR smps)
{
    struct wtbl_entry tb_entry;
    struct wtbl_1_struc wtbl_1;

    NdisZeroMemory((UCHAR *)&wtbl_1, sizeof(struct wtbl_1_struc));
    NdisZeroMemory(&tb_entry, sizeof(tb_entry));
    if (mt_wtbl_get_entry234(pAd, wcid, &tb_entry) == FALSE) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s: Cannot found WTBL2/3/4 for WCID(%d)\n",
                    __FUNCTION__, wcid));

        return ;
    }
    // WTBL1.dw2 bit19, support Dynamic SMPS
    RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0] + 8, &wtbl_1.wtbl_1_d2.word);
    wtbl_1.wtbl_1_d2.field.smps = smps;
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 8, wtbl_1.wtbl_1_d2.word);
}


#ifdef MCS_LUT_SUPPORT
UCHAR aucHtMaxRetryLimit[]={
	MCS_0,  4,
	MCS_1,  4,
	MCS_2,  7,
	MCS_3,  8,
	MCS_4,  9,
	MCS_5,  10,
	MCS_6,  11,
	MCS_7,  12,
	MCS_8,  4,
	MCS_9,  7,
	MCS_10, 8,
	MCS_11, 9,
	MCS_12, 10,
	MCS_13, 11,
	MCS_14, 12,
	MCS_15, 12,
};
VOID asic_mcs_lut_update(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *pEntry)
{
	union  WTBL_2_DW9 wtbl_2_d9 = {.word = 0};
	UINT32 /*reg_val,*/ rate[8];
	UCHAR stbc, bw, nss, preamble/*, wait_cnt = 0*/;
	CHAR rssi;
	UCHAR ucMaxTxRetryCnt = 0;

	// TODO: shiang-MT7603, shall we use MaxHTPhyMode.field.BW or HTPhyMode.field.BW here??
	switch (pEntry->HTPhyMode.field.BW)
	{
		case BW_80:
			bw = 2;
			break;
		case BW_40:
			bw = 1;
			break;
		case BW_20:
		//case BW_10:
		default:
			bw = 0;
			break;
	}
	wtbl_2_d9.field.fcap = bw;
	wtbl_2_d9.field.ccbw_sel = bw;
	wtbl_2_d9.field.cbrn = 7; // change bw as (fcap/2) if rate_idx > 7, temporary code


	if (pEntry->HTPhyMode.field.ShortGI) {
		wtbl_2_d9.field.g2 = 1;
		wtbl_2_d9.field.g4 = 1;
		wtbl_2_d9.field.g8 = 1;
		wtbl_2_d9.field.g16 = 1;
	} else {
		wtbl_2_d9.field.g2 = 0;
		wtbl_2_d9.field.g4 = 0;
		wtbl_2_d9.field.g8 = 0;
		wtbl_2_d9.field.g16 = 0;
	}
	wtbl_2_d9.field.rate_idx = 0;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED))
		preamble = SHORT_PREAMBLE;
	else
		preamble = LONG_PREAMBLE;

	stbc = (UCHAR)pEntry->HTPhyMode.field.STBC;

#ifdef THERMAL_PROTECT_SUPPORT
    if  (pAd->force_one_tx_stream == TRUE) {
        stbc = 0;
    }
#endif /* THERMAL_PROTECT_SUPPORT */

	nss = get_nss_by_mcs((UCHAR)pEntry->HTPhyMode.field.MODE,
		(UCHAR)pEntry->HTPhyMode.field.MCS, stbc);


	if (((pEntry->MmpsMode != MMPS_STATIC) || ( pEntry->HTPhyMode.field.MODE < MODE_HTMIX ))
#ifdef THERMAL_PROTECT_SUPPORT
			&& (pAd->force_one_tx_stream == FALSE)
#endif /* THERMAL_PROTECT_SUPPORT */
            )
	{
		rssi = RTMPMaxRssi(pAd, pEntry->RssiSample.AvgRssi[0], pEntry->RssiSample.AvgRssi[1], pEntry->RssiSample.AvgRssi[2]);
		if (rssi < -50 )
		{
			wtbl_2_d9.field.spe_en = 1;
		}
	}

	rate[0] = tx_rate_to_tmi_rate(
		(UCHAR)pEntry->HTPhyMode.field.MODE,
		(UCHAR)pEntry->HTPhyMode.field.MCS,
		nss,
		pEntry->HTPhyMode.field.MCS == MCS_32?0:stbc,
		preamble);
	rate[0] &= 0xfff;

	if ( pEntry->bAutoTxRateSwitch == TRUE )
	{
		UCHAR	ucIndex;
		UCHAR	DownRateIdx, CurrRateIdx;
		UCHAR mode, mcs;
		BOOLEAN fgLowestRate = FALSE;

		CurrRateIdx = pEntry->CurrTxRateIndex;
		DownRateIdx = CurrRateIdx;

#ifdef NEW_RATE_ADAPT_SUPPORT
#ifdef WAPI_SUPPORT
#ifdef RELEASE_EXCLUDE
	/*
		Security hardware limitation.
		When security mode is WAPI, the data rate cannot exceed MCS13.
	*/
#endif /* RELEASE_EXCLUDE */
	if (IS_MT7603(pAd) || IS_MT7628(pAd) || IS_MT7636(pAd))
	{
		if ((pEntry->AuthMode == Ndis802_11AuthModeWAICERT) || (pEntry->AuthMode == Ndis802_11AuthModeWAIPSK))
		{
			if (pEntry->pTable == RateSwitchTableAdapt11N2S)
			{
				if ((CurrRateIdx >= 14) && (CurrRateIdx <= 16))
				{
					CurrRateIdx = 13;
				}
			}
		}
	}
#endif /* WAPI_SUPPORT */
#endif /* NEW_RATE_ADAPT_SUPPORT */

		for ( ucIndex = 1; ucIndex < 8 ; ucIndex++ )
		{
			if (ADAPT_RATE_TABLE(pEntry->pTable)) {
				RTMP_RA_GRP_TB *pCurrTxRate;

				if ( ucIndex == 7 )
				{
					if (fgLowestRate == FALSE)
					{
						do {
							CurrRateIdx = DownRateIdx;
							DownRateIdx = MlmeSelectDownRate(pAd, pEntry, CurrRateIdx);
						} while ( CurrRateIdx != DownRateIdx );
					}
				}
				else
				{
					DownRateIdx = MlmeSelectDownRate(pAd, pEntry, CurrRateIdx);
					if (fgLowestRate == FALSE)
					{
						DownRateIdx = MlmeSelectDownRate(pAd, pEntry, CurrRateIdx);
					}
				}

				if (pEntry->HTPhyMode.field.ShortGI)
				{
					pCurrTxRate = PTX_RA_GRP_ENTRY(pEntry->pTable, DownRateIdx);

					if ( pCurrTxRate->CurrMCS == pEntry->HTPhyMode.field.MCS )
					{
						CurrRateIdx = DownRateIdx;
						DownRateIdx = MlmeSelectDownRate(pAd, pEntry, CurrRateIdx);
					}
				}

				pCurrTxRate = PTX_RA_GRP_ENTRY(pEntry->pTable, DownRateIdx);
				mode = pCurrTxRate->Mode;
				mcs = pCurrTxRate->CurrMCS;
			} else {
				mode = MODE_CCK;
				mcs = 0;
				DownRateIdx = 0;
				DBGPRINT(RT_DEBUG_ERROR, ("%s: Not support legacy table.\n", __FUNCTION__));
			}


			nss = get_nss_by_mcs(mode, mcs, (BOOLEAN)pEntry->HTPhyMode.field.STBC);
			rate[ucIndex] = tx_rate_to_tmi_rate(mode,
											mcs,
											nss,
											pEntry->HTPhyMode.field.MCS == MCS_32?0:stbc,
											preamble);

			rate[ucIndex] &= 0xfff;

			if (CurrRateIdx == DownRateIdx)
			{
				fgLowestRate = TRUE;
			}
			else
			{
				CurrRateIdx = DownRateIdx;
			}
		}
	}
	else
	{
		rate[1] = rate[2] = rate[3] = rate[4] = rate[5] = rate[6] = rate[7] = rate[0];
	}

#if 0
	// TODO: shiang-MT7603, dirty job, need to remove it after FW is ready or change as semaphore!
	do {
		RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &reg_val);
		if ((reg_val & 0x10000)  == 0)
			break;
		wait_cnt++;
		RtmpusecDelay(50);
	}while (wait_cnt < 100);

	if (wait_cnt == 100) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
					__FUNCTION__, reg_val));
		return;
	}

	reg_val = wtbl_2_d9.word;
	RTMP_IO_WRITE32(pAd, WTBL_ON_RIUCR0, reg_val);
	reg_val = (rate[0] | (rate[1] << 12) | (rate[2] << 24));
	RTMP_IO_WRITE32(pAd, WTBL_ON_RIUCR1, reg_val);
	reg_val = ((rate[2] >> 8) | (rate[3] << 4) | (rate[4] << 16) | (rate[5] << 28));
	RTMP_IO_WRITE32(pAd, WTBL_ON_RIUCR2, reg_val);
	reg_val = ((rate[5] >> 4) | (rate[6] << 8) | (rate[7] << 20));
	RTMP_IO_WRITE32(pAd, WTBL_ON_RIUCR3, reg_val);

	// TODO: shiang-MT7603, shall we also clear TxCnt/RxCnt/AdmCnt here??
	reg_val = (pEntry->wcid | (1 << 13));
	RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, reg_val);
#else
    Wtbl2RateTableUpdate(pAd, pEntry->wcid, wtbl_2_d9.word, rate);
#endif

	if (pEntry->MaxHTPhyMode.field.MODE == MODE_HTMIX)
	{
		if (pEntry->bAutoTxRateSwitch == TRUE)
		{
			if(pEntry->HTPhyMode.field.MODE == MODE_CCK)
			{
				if(pEntry->HTPhyMode.field.MCS == 0)
				{
					ucMaxTxRetryCnt = 3;
				}
				else
				{
					ucMaxTxRetryCnt = 4;
				}
			}
			else
			{
				if(pEntry->HTPhyMode.field.MCS == MCS_32)
				{
					ucMaxTxRetryCnt = 4;
				}
				else
				{
					if (((pEntry->HTPhyMode.field.MCS*2) + 1) <
						sizeof(aucHtMaxRetryLimit)/sizeof(UCHAR))
						ucMaxTxRetryCnt =
							aucHtMaxRetryLimit[
							(pEntry->HTPhyMode.field.MCS*2) + 1];
					else {
						DBGPRINT(RT_DEBUG_ERROR,
							("%s(): Invalid index!(%d)\n",
							__func__,
							(pEntry->HTPhyMode.field.MCS*2) + 1));
						return;
					}
				}
			}
		}
		else
		{
			ucMaxTxRetryCnt = MT_TX_SHORT_RETRY;
		}
	}
	else
	{
		ucMaxTxRetryCnt = MT_TX_SHORT_RETRY;
	}

	if (pAd->MacTab.Size > 3)
		pEntry->ucMaxTxRetryCnt = ucMaxTxRetryCnt;
	else
		pEntry->ucMaxTxRetryCnt = MT_TX_SHORT_RETRY;

	DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA, ("%s():WCID=%d, HTPhyMode=0x%x\n",
				__FUNCTION__, pEntry->wcid, pEntry->HTPhyMode.word));

	DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_RA, ("\tCurTxRateIdx=%d, Mode/BW/MCS/STBC/SGI=%d/%d/%d/%d/%d\n\n",
		pEntry->CurrTxRateIndex,
		pEntry->HTPhyMode.field.MODE,
		pEntry->HTPhyMode.field.BW,
		pEntry->HTPhyMode.field.MCS,
		pEntry->HTPhyMode.field.STBC,
		pEntry->HTPhyMode.field.ShortGI));
}
#endif /* MCS_LUT_SUPPORT */


UINT16 AsicGetTidSn(RTMP_ADAPTER *pAd, UCHAR Wcid, UCHAR Tid)
{
    UINT16 Sn = 0;
    UINT32 Reg = 0;
	struct wtbl_entry Entry = {0};
	struct wtbl_2_struc *Wtbl_2 = NULL;


    NdisZeroMemory((UCHAR *)(&Entry), sizeof(struct wtbl_entry));
	if(TRUE != mt_wtbl_get_entry234(pAd, Wcid, &Entry))
		return -1;

    Wtbl_2 = &Entry.wtbl_2;

    switch (Tid)
    {
        case 0:
            Reg = Entry.wtbl_addr[1] + (4 * 2); //WTBL2.DW2
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d2.word);
			Sn = (UINT16)Wtbl_2->wtbl_2_d2.field.tid_ac_0_sn;
            break;

        case 1:
            Reg = Entry.wtbl_addr[1] + (4 * 2); //WTBL2.DW2
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d2.word);
			Sn = (UINT16)Wtbl_2->wtbl_2_d2.field.tid_ac_1_sn;
            break;

        case 2:
            Reg = Entry.wtbl_addr[1] + (4 * 2); //WTBL2.DW2
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d2.word);
			Sn = (UINT16)Wtbl_2->wtbl_2_d2.field.tid_ac_2_sn_0;

            Reg = Entry.wtbl_addr[1] + (4 * 3); //WTBL2.DW3
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d3.word);
			Sn = (UINT16)((Wtbl_2->wtbl_2_d3.field.tid_ac_2_sn_9 << 8) | Sn);
            break;

        case 3:
            Reg = Entry.wtbl_addr[1] + (4 * 3); //WTBL2.DW3
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d3.word);
			Sn = (UINT16)Wtbl_2->wtbl_2_d3.field.tid_ac_3_sn;
            break;

        case 4:
            Reg = Entry.wtbl_addr[1] + (4 * 3); //WTBL2.DW3
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d3.word);
			Sn = (UINT16)Wtbl_2->wtbl_2_d3.field.tid_4_sn;
            break;

        case 5:
            Reg = Entry.wtbl_addr[1] + (4 * 3); //WTBL2.DW3
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d3.word);
			Sn = (UINT16)Wtbl_2->wtbl_2_d3.field.tid_5_sn_0;

            Reg = Entry.wtbl_addr[1] + (4 * 4); //WTBL2.DW4
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d4.word);
			Sn = (UINT16)((Wtbl_2->wtbl_2_d4.field.tid_5_sn_5 << 4) | Sn);
            break;

        case 6:
            Reg = Entry.wtbl_addr[1] + (4 * 4); //WTBL2.DW4
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d4.word);
			Sn = (UINT16)Wtbl_2->wtbl_2_d4.field.tid_6_sn;
            break;

        case 7:
            Reg = Entry.wtbl_addr[1] + (4 * 4); //WTBL2.DW4
            RTMP_IO_READ32(pAd, Reg, &Wtbl_2->wtbl_2_d4.word);
			Sn = (UINT16)Wtbl_2->wtbl_2_d4.field.tid_7_sn;
            break;

        default:
            DBGPRINT(RT_DEBUG_ERROR, ("%s: Unknown TID(%d) \n", __FUNCTION__, Tid));
            break;
    }

    return Sn;
}


static UCHAR ba_range[] = {4, 5, 8, 10, 16, 20, 21, 42};
VOID AsicUpdateBASession(RTMP_ADAPTER *pAd, UCHAR wcid, UCHAR tid, UINT16 sn, UCHAR basize, BOOLEAN isAdd, INT ses_type)
{
	struct wtbl_entry ent;
	struct wtbl_2_struc *wtbl_2;
	UINT32 range_mask = 0x7 << (tid * 3);
	UINT32 reg, value;
    MAC_TABLE_ENTRY *mac_entry;

	DBGPRINT(RT_DEBUG_TRACE, ("%s():Update BA Session Info of wcid(%d)=>tid=%d, sn = %d, basize=%d, isAdd=%d, ses_type=%s(%d)\n",
			__FUNCTION__, wcid, tid, sn, basize, isAdd, (ses_type == BA_SESSION_ORI ? "Ori" : "Recp"), ses_type));

	if (ses_type == BA_SESSION_RECP)
	{
        /* Reset BA SSN & Score Board Bitmap, for BA Receiptor */
		if (isAdd)
		{
			mac_entry = &pAd->MacTab.Content[wcid];
        	value = (mac_entry->Addr[0] | (mac_entry->Addr[1] << 8) |
                (mac_entry->Addr[2] << 16) | (mac_entry->Addr[3] << 24));
        	RTMP_IO_WRITE32(pAd, BSCR0, value);

        	RTMP_IO_READ32(pAd, BSCR1, &value);
        	value &= ~(BA_MAC_ADDR_47_32_MASK | RST_BA_TID_MASK | RST_BA_SEL_MASK);
        	value |= BA_MAC_ADDR_47_32((mac_entry->Addr[4] | (mac_entry->Addr[5] << 8)));
        	value |= (RST_BA_SEL(RST_BA_MAC_TID_MATCH) | RST_BA_TID(tid) | START_RST_BA_SB);
        	RTMP_IO_WRITE32(pAd, BSCR1, value);
		}
	}
	else
	{
		NdisZeroMemory((UCHAR *)(&ent), sizeof(struct wtbl_entry));
		if(TRUE != mt_wtbl_get_entry234(pAd, wcid, &ent))
			return;
		wtbl_2 = &ent.wtbl_2;

		if (isAdd)
		{
			INT idx = 0;

			while (ba_range[idx] < basize) {
				if (idx == 7)
					break;
				idx++;
			};

			if (ba_range[idx] > basize)
				idx--;

			reg = ent.wtbl_addr[1] + (15 * 4);
			RTMP_IO_READ32(pAd, reg, &wtbl_2->wtbl_2_d15.word);

			wtbl_2->wtbl_2_d15.field.ba_en |= 1 <<tid;
			wtbl_2->wtbl_2_d15.field.ba_win_size_tid &= (~range_mask);
			wtbl_2->wtbl_2_d15.field.ba_win_size_tid |= (idx << (tid * 3));

			RTMP_IO_WRITE32(pAd, reg, wtbl_2->wtbl_2_d15.word);
		}
		else
		{
			reg = ent.wtbl_addr[1] + (15 * 4);
			RTMP_IO_READ32(pAd, reg, &wtbl_2->wtbl_2_d15.word);

			wtbl_2->wtbl_2_d15.field.ba_en &=  (~(1 <<tid));
			wtbl_2->wtbl_2_d15.field.ba_win_size_tid &= (~range_mask);

			RTMP_IO_WRITE32(pAd, reg, wtbl_2->wtbl_2_d15.word);
		}

		DBGPRINT(RT_DEBUG_TRACE, ("%s(): ent->wtbl_addr[1]=0x%x, val=0x%08x\n", __FUNCTION__, reg, wtbl_2->wtbl_2_d15.word));
	}
}


VOID AsicUpdateRxWCIDTable(RTMP_ADAPTER *pAd, USHORT WCID, UCHAR *pAddr)
{
	struct wtbl_entry tb_entry;
    //struct wtbl_2_struc *wtbl_2;
	union WTBL_1_DW0 *dw0 = (union WTBL_1_DW0 *)&tb_entry.wtbl_1.wtbl_1_d0.word;
	union WTBL_1_DW1 *dw1 = (union WTBL_1_DW1 *)&tb_entry.wtbl_1.wtbl_1_d1.word;
	union WTBL_1_DW2 *dw2 = (union WTBL_1_DW2 *)&tb_entry.wtbl_1.wtbl_1_d2.word;
	union WTBL_1_DW3 *dw3 = (union WTBL_1_DW3 *)&tb_entry.wtbl_1.wtbl_1_d3.word;
	union WTBL_1_DW4 *dw4 = (union WTBL_1_DW4 *)&tb_entry.wtbl_1.wtbl_1_d4.word;
	struct rtmp_mac_ctrl *wtbl_ctrl = &pAd->mac_ctrl;
	MAC_TABLE_ENTRY *mac_entry;
	UINT32 Value, Index;
	UCHAR WaitCnt = 0;
#ifdef RTMP_USB_SUPPORT
	UINT32 Flags;
#endif
	INT apidx = MAIN_MBSSID;	

#ifdef RT_CFG80211_P2P_SUPPORT
	/* HW BSSID  will be 1, if in softap or P2P GO mode. Need this value to stop TX queue AC0~AC3 when update WCID*/
	if (pAd->cfg80211_ctrl.isCfgInApMode == RT_CMD_80211_IFTYPE_AP)
		apidx = CFG_GO_BSSID_IDX;
#endif

	
	if (wtbl_ctrl->wtbl_entry_cnt[0] > 0)
		WCID = (WCID < wtbl_ctrl->wtbl_entry_cnt[0] ? WCID : MCAST_WCID);
	else {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():PSE not init yet!\n", __FUNCTION__));
		return;
	}

	// TODO: shiang-7603

	NdisZeroMemory(&tb_entry, sizeof(tb_entry));
	if (mt_wtbl_get_entry234(pAd, (UCHAR)WCID, &tb_entry) == FALSE) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Cannot found WTBL2/3/4 for WCID(%d)\n",
					__FUNCTION__, WCID));
		return;
	}

    dw0->field.wm = 0;
    if (WCID == MCAST_WCID ) {
        dw0->field.muar_idx = 0xe;
        dw2->field.adm = 0;
        dw2->field.cipher_suit = WTBL_CIPHER_NONE;
		//YF TODO: check ?
        dw0->field.rv = 1;
	dw0->field.rc_a1 = 1;
	dw0->field.rc_a2 = 1;
    }
#ifdef CONFIG_STA_SUPPORT
    /* for ad-hoc BC/MC Entry using, Addr ==> FF:FF:FF:FF:FF:FF */
    else if (WCID == pAd->StaCfg.wdev.tr_tb_idx) {
	dw0->field.muar_idx = 0xe;
        dw2->field.adm = 0;
        dw2->field.cipher_suit = WTBL_CIPHER_NONE;
		dw0->field.rc_a1 = 1;
    }
#endif /* CONFIG_STA_SUPPORT */
    else if (WCID == APCLI_MCAST_WCID) {
        dw0->field.muar_idx = 0xe;
        dw0->field.rv = 1;
        dw2->field.adm = 0;
        dw2->field.cipher_suit = WTBL_CIPHER_NONE;
        dw0->field.rc_a1 = 1;
        dw0->field.rc_a2 = 1;
    }
    else {
		if (WCID < MAX_LEN_OF_MAC_TABLE)
			mac_entry = &pAd->MacTab.Content[WCID];
		else {
			DBGPRINT(RT_DEBUG_ERROR, ("%s():Invalid WCID(%d)!\n",
					__func__, WCID));
			return;
		}

        //dw0->field.muar_idx = 0x0; // TODO: need to change depends on different BssIdx!
        if (IS_ENTRY_CLIENT(mac_entry)) {
            if (mac_entry->func_tb_idx == 0)
                dw0->field.muar_idx = 0x0;
            else if (mac_entry->func_tb_idx >= 1 && mac_entry->func_tb_idx <= 15)
                dw0->field.muar_idx = 0x10 | mac_entry->func_tb_idx;

            /* for concurrent to handle HW_BSSID_1/2/3 */
            if (mac_entry->wdev->hw_bssid_idx != 0)
            	dw0->field.muar_idx = mac_entry->wdev->hw_bssid_idx ;

        }
        else if (IS_ENTRY_APCLI(mac_entry)) {
            dw0->field.muar_idx = 0x1;//Carter, MT_MAC apcli use HWBSSID1 to go.
            dw0->field.rc_a1 = 1;
        }
        else
            dw0->field.muar_idx = 0x0;

        dw0->field.rv = 1;
        DBGPRINT(RT_DEBUG_TRACE, ("%s(): mac_entry->type=%d, pMacEntry->MaxHTPhyMode.field.MODE=%d, StatusFlag=0x%lx\n",
                    __FUNCTION__, mac_entry->EntryType,
                    mac_entry->MaxHTPhyMode.field.MODE,
                    mac_entry->ClientStatusFlags));

        dw0->field.rc_a2 = 1;
        // TODO: shiang-MT7603, in which case we need to check A1???
        //dw0->field.rc_a1 = 1;
        dw2->field.adm = 1;
        switch (mac_entry->WepStatus)
        {
            case Ndis802_11WEPDisabled:
                dw2->field.cipher_suit = WTBL_CIPHER_NONE;
                break;
            default:
                break;
        }

		if (mac_entry->AuthMode == Ndis802_11AuthModeShared)
		{
			dw0->field.rkv = 0;
			dw2->field.cipher_suit = WTBL_CIPHER_WEP_40;
		}

        if (IS_HT_STA(mac_entry)) {
            dw2->field.ht = 1;
            dw2->field.qos = 1;
            dw2->field.mm = mac_entry->MpduDensity;
            dw2->field.af = mac_entry->MaxRAmpduFactor;
            if (CLIENT_STATUS_TEST_FLAG(mac_entry, fCLIENT_STATUS_RDG_CAPABLE)) {
                dw2->field.r = 1;
                dw2->field.rdg_ba = 1;
            }

            if (mac_entry->MmpsMode == MMPS_DYNAMIC)
                dw2->field.smps = 1;
            else
                dw2->field.smps = 0;

            dw4->field.partial_aid = WCID;
        }
    }

    dw0->field.addr_4 = pAddr[4];
    dw0->field.addr_5 = pAddr[5];
    dw1->word = pAddr[0] + (pAddr[1] << 8) +(pAddr[2] << 16) +(pAddr[3] << 24);
    dw3->field.wtbl2_fid = tb_entry.wtbl_fid[1];
    dw3->field.wtbl2_eid = tb_entry.wtbl_eid[1];
    dw3->field.wtbl4_fid = tb_entry.wtbl_fid[3];
    dw3->field.psm = 0;
#ifndef MT_PS
	dw3->field.i_psm = 1;
	dw3->field.du_i_psm = 1;
#endif /* !MT_PS */
    dw4->field.wtbl3_fid = tb_entry.wtbl_fid[2];
    dw4->field.wtbl3_eid = tb_entry.wtbl_eid[2];
    dw4->field.wtbl4_eid = tb_entry.wtbl_eid[3];

    
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 8, dw2->word);
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 4, dw1->word);
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0], dw0->word);

    DBGPRINT(RT_DEBUG_TRACE, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
                __FUNCTION__, WCID, tb_entry.wtbl_addr[0], dw0->word));
    DBGPRINT(RT_DEBUG_TRACE, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
                __FUNCTION__, WCID,  tb_entry.wtbl_addr[0] + 4, dw1->word));
    DBGPRINT(RT_DEBUG_TRACE, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
                __FUNCTION__, WCID, tb_entry.wtbl_addr[0] + 8, dw2->word));

    RTMP_IO_READ32(pAd, WTBL1OR, &Value);
    Value |= PSM_W_FLAG;
    RTMP_IO_WRITE32(pAd, WTBL1OR, Value);

    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 12, dw3->word);
    DBGPRINT(RT_DEBUG_TRACE, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
                __FUNCTION__, WCID, tb_entry.wtbl_addr[0] + 12, dw3->word));

    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 16, dw4->word);
    DBGPRINT(RT_DEBUG_TRACE, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
                __FUNCTION__, WCID, tb_entry.wtbl_addr[0] + 16, dw4->word));

    RTMP_IO_READ32(pAd, WTBL1OR, &Value);
    Value &= ~PSM_W_FLAG;
    RTMP_IO_WRITE32(pAd, WTBL1OR, Value);


    /* Clear BA Information */
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[1] + (15 * 4), tb_entry.wtbl_2.wtbl_2_d15.word);
    /* IOT issue with BCM's AP (ex: TP-LINK AC1900 C8/C9): Clear PN in wtbl2
    *  The PN shall be initialized when the corresponding TK is initialized or refreshed
    *  If the PN (Packet Number) exceed the threshold, AP will drop the data frames.
    */
    tb_entry.wtbl_2.wtbl_2_d0.pn_0 = 0x0;
    tb_entry.wtbl_2.wtbl_2_d1.field.pn_32 = 0x0;
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[1] + (0 * 4), tb_entry.wtbl_2.wtbl_2_d0.word);
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[1] + (1 * 4), tb_entry.wtbl_2.wtbl_2_d1.word);

    /* Clear WTBL2 TID SN to default */
/* We need change disable TX to just disable TX Q only, or RX path may have problem due to we can not TX ack*/
    //AsicSetMacTxRx(pAd, ASIC_MAC_TX, FALSE);
	MtAsicACQueue(pAd, AC_QUEUE_STOP, (UINT8)apidx, 0xF);
	
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[1] + (2 * 4), 0x0);
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[1] + (3 * 4), 0x0);
    RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[1] + (4 * 4), 0x0);
#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_WAIT(&pAd->IndirectUpdateLock, Flags);
	if(Flags != 0)
		DBGPRINT(RT_DEBUG_ERROR, ("%s:(%d) RTMP_SEM_EVENT_WAIT failed!\n",__FUNCTION__,Flags));
#endif
#ifdef RTMP_PCI_SUPPORT
	NdisAcquireSpinLock(&pAd->IndirectUpdateLock);
#endif
    RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
    Value &= ~WLAN_IDX_MASK;
    Value |= WLAN_IDX(WCID);
    Value |= WTBL2_UPDATE_FLAG;
    RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, Value);

    WaitCnt = 0;

    do {
        RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
        if ((Value & IU_BUSY)  == 0)
            break;
        WaitCnt++;
        RtmpusecDelay(50);
    } while (WaitCnt < 100);

    if (WaitCnt == 100) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
                    __FUNCTION__, Value));
#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif
#ifdef RTMP_PCI_SUPPORT
	NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
        //AsicSetMacTxRx(pAd, ASIC_MAC_TX, TRUE);
	MtAsicACQueue(pAd, AC_QUEUE_START, (UINT8)apidx, 0xF);

        return;
    }

#ifdef RTMP_USB_SUPPORT
    RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif
#ifdef RTMP_PCI_SUPPORT
    NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
    //AsicSetMacTxRx(pAd, ASIC_MAC_TX, TRUE);
	MtAsicACQueue(pAd, AC_QUEUE_START, (UINT8)apidx, 0xF);


#ifdef RTMP_USB_SUPPORT
RTMP_SEM_EVENT_WAIT(&pAd->IndirectUpdateLock, Flags);
#endif

#ifdef RTMP_PCI_SUPPORT
NdisAcquireSpinLock(&pAd->IndirectUpdateLock);
#endif

    /* RX Counter Clear */
    RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
    Value &= ~WLAN_IDX_MASK;
    Value |= WLAN_IDX(WCID);
    Value |= RX_CNT_CLEAR;
    RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, Value);

    WaitCnt = 0;

    do {
        RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
        if ((Value & IU_BUSY)  == 0)
            break;
        WaitCnt++;
        RtmpusecDelay(50);
    } while (WaitCnt < 100);

    if (WaitCnt == 100) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
                    __FUNCTION__, Value));

#ifdef RTMP_USB_SUPPORT
		RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
		NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
        return;
    }

    /* TX Counter Clear */
    RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
    Value &= ~WLAN_IDX_MASK;
    Value |= WLAN_IDX(WCID);
    Value |= TX_CNT_CLEAR;
    RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, Value);

    WaitCnt = 0;

    do {
        RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
        if ((Value & IU_BUSY)  == 0)
            break;
        WaitCnt++;
        RtmpusecDelay(50);
    } while (WaitCnt < 100);

    if (WaitCnt == 100) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
                    __FUNCTION__, Value));

#ifdef RTMP_USB_SUPPORT
		RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
		NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
        return;
    }

    /* Clear Cipher Key */
    for (Index = 0; Index < 8; Index++)
        RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[2] + (4 * Index), 0x0);

    /* Admission Control Counter Clear */
    RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
    Value &= ~WLAN_IDX_MASK;
    Value |= WLAN_IDX(WCID);
    Value |= ADM_CNT_CLEAR;
    RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, Value);

    WaitCnt = 0;

    do {
        RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
        if ((Value & IU_BUSY)  == 0)
            break;
        WaitCnt++;
        RtmpusecDelay(50);
    } while (WaitCnt < 100);

    if (WaitCnt == 100) {
        DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
                    __FUNCTION__, Value));

#ifdef RTMP_USB_SUPPORT
		RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
		NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
        return;
    }

#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
	NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
    
}


/*
   ========================================================================
Description:
Add Client security information into ASIC WCID table and IVEIV table.
Return:

Note :
The key table selection rule :
1.	Wds-links and Mesh-links always use Pair-wise key table.
2. 	When the CipherAlg is TKIP, AES, SMS4 or the dynamic WEP is enabled,
it needs to set key into Pair-wise Key Table.
3.	The pair-wise key security mode is set NONE, it means as no security.
4.	In STA Adhoc mode, it always use shared key table.
5.	Otherwise, use shared key table

========================================================================
 */
VOID AsicUpdateWcidAttributeEntry(
        IN	PRTMP_ADAPTER	pAd,
        IN	UCHAR			BssIdx,
        IN 	UCHAR		 	KeyIdx,
        IN 	UCHAR		 	CipherAlg,
        IN	UINT8			Wcid,
        IN	UINT8			KeyTabFlag)
{
    //	WCID_ATTRIBUTE_STRUC WCIDAttri;
    //USHORT offset;
    //UINT32 wcid_attr_base = 0, wcid_attr_size = 0;

    // TODO: shiang-7603
    if (pAd->chipCap.hif_type == HIF_MT) {
        DBGPRINT(RT_DEBUG_INFO, ("%s(%d): , Wcid=%d, BssIdx=%d, KeyIdx=%d, Not support for HIF_MT yet!\n",
                    __FUNCTION__, __LINE__, Wcid, BssIdx, KeyIdx));
        return;
    }

    /* The limitation of HW WCID table */
    if (Wcid > 254)
    {
        DBGPRINT(RT_DEBUG_WARN, ("%s:Invalid wcid(%d)\n", __FUNCTION__, Wcid));
        return;
    }

#if 0
    /* Initialize the content of WCID Attribue  */
    WCIDAttri.word = 0;

    /* Update the pairwise key security mode.
       Use bit10 and bit3~1 to indicate the pairwise cipher mode */
    WCIDAttri.field.PairKeyModeExt = ((CipherAlg & 0x08) >> 3);
    WCIDAttri.field.PairKeyMode = (CipherAlg & 0x07);

    /* Update the MBSS index.
       Use bit11 and bit6~4 to indicate the BSS index */
    WCIDAttri.field.BSSIdxExt = ((BssIdx & 0x08) >> 3);
    WCIDAttri.field.BSSIdx = (BssIdx & 0x07);

#ifdef WAPI_SUPPORT
    /* Update WAPI related information */
    if (CipherAlg == CIPHER_SMS4)
    {
        if (KeyTabFlag == SHAREDKEYTABLE)
            WCIDAttri.field.WAPI_MCBC = 1;
        WCIDAttri.field.WAPIKeyIdx = ((KeyIdx == 0) ? 0 : 1);
    }
#endif /* WAPI_SUPPORT */

    /* Assign Key Table selection */
    //WCIDAttri.field.KeyTab = KeyTabFlag;

    /* Update related information to ASIC */
    offset = wcid_attr_base + (Wcid * wcid_attr_size);
    //RTMP_IO_WRITE32(pAd, offset, WCIDAttri.word);

    //	DBGPRINT(RT_DEBUG_TRACE, ("%s:WCID #%d, KeyIdx #%d, WCIDAttri=0x%x, Alg=%s\n",
    //					__FUNCTION__, Wcid, KeyIdx, WCIDAttri.word, CipherName[CipherAlg]));
#endif
}


/*
   ==========================================================================
Description:

IRQL = DISPATCH_LEVEL

==========================================================================
 */
VOID AsicDelWcidTab(RTMP_ADAPTER *pAd, UCHAR wcid_idx)
{
    UCHAR cnt, cnt_s, cnt_e;
    struct wtbl_entry tb_entry;
    UCHAR WaitCnt = 0;
    UINT32 Value;
    union WTBL_1_DW0 *dw0 = (union WTBL_1_DW0 *)&tb_entry.wtbl_1.wtbl_1_d0.word;
    union WTBL_1_DW3 *dw3 = (union WTBL_1_DW3 *)&tb_entry.wtbl_1.wtbl_1_d3.word;
    union WTBL_1_DW4 *dw4 = (union WTBL_1_DW4 *)&tb_entry.wtbl_1.wtbl_1_d4.word;

#ifdef RTMP_USB_SUPPORT
	UINT32 Flags;
#endif

    DBGPRINT(RT_DEBUG_TRACE, ("AsicDelWcidTab==>wcid_idx = 0x%x\n",wcid_idx));

    if (wcid_idx == WCID_ALL) {
        cnt_s = 0;
#ifdef RT_CFG80211_P2P_SUPPORT
		//YF TODO: shall re-write the API parm
		cnt_s = 2;
#endif /* RT_CFG80211_P2P_SUPPORT */
        cnt_e = (WCID_ALL - 1);
    } else {
        cnt_s = cnt_e = wcid_idx;
    }

    for (cnt = cnt_s; cnt_s <= cnt_e; cnt_s++)
    {
        cnt = cnt_s;
        NdisZeroMemory(&tb_entry, sizeof(tb_entry));
        if (mt_wtbl_get_entry234(pAd, cnt, &tb_entry) == FALSE) {
            DBGPRINT(RT_DEBUG_ERROR, ("%s():Cannot found WTBL2/3/4 for WCID(%d)\n",
                        __FUNCTION__, cnt));
            return;
        }

        dw0->field.wm = 0;
        dw0->field.rc_a2 = 1;
        dw0->field.rv = 1;
        RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0], tb_entry.wtbl_1.wtbl_1_d0.word);
        RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 4, tb_entry.wtbl_1.wtbl_1_d1.word);
		/* Don't clear cipher suite for kr00k attack */
        RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0] + 8, &Value);
#ifdef RT_BIG_ENDIAN
		Value = Value & 0xF << 25;
#else
        Value = Value & 0xF <<3;
#endif
        RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 8, Value);

        RTMP_IO_READ32(pAd, WTBL1OR, &Value);
        Value |= PSM_W_FLAG;
        RTMP_IO_WRITE32(pAd, WTBL1OR, Value);

        dw3->field.wtbl2_fid = tb_entry.wtbl_fid[1];
        dw3->field.wtbl2_eid = tb_entry.wtbl_eid[1];
        dw3->field.wtbl4_fid = tb_entry.wtbl_fid[3];
        dw4->field.wtbl3_fid = tb_entry.wtbl_fid[2];
        dw4->field.wtbl3_eid = tb_entry.wtbl_eid[2];
        dw4->field.wtbl4_eid = tb_entry.wtbl_eid[3];

#ifdef MT_PS
        if (cnt == MCAST_WCID) {
            dw3->field.i_psm=1;
            dw3->field.du_i_psm=1;
        }
#endif /*leonardo workaround*/

        //RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 12, tb_entry.wtbl_1.wtbl_1_d3.word);
        //RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 16, tb_entry.wtbl_1.wtbl_1_d4.word);
        RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 12, dw3->word);

        RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 16, dw4->word);

        RTMP_IO_READ32(pAd, WTBL1OR, &Value);
        Value &= ~PSM_W_FLAG;
        RTMP_IO_WRITE32(pAd, WTBL1OR, Value);


        /* Clear BA Information */
        RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[1] + (15 * 4), tb_entry.wtbl_2.wtbl_2_d15.word);
#ifdef RTMP_USB_SUPPORT
		RTMP_SEM_EVENT_WAIT(&pAd->IndirectUpdateLock, Flags);
		if(Flags != 0)
			DBGPRINT(RT_DEBUG_ERROR, ("%s:(%d) RTMP_SEM_EVENT_WAIT failed!\n",__FUNCTION__,Flags));
#endif

#ifdef RTMP_PCI_SUPPORT
		NdisAcquireSpinLock(&pAd->IndirectUpdateLock);
#endif
        /* RX Counter Clear */
        RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
        Value &= ~WLAN_IDX_MASK;
        Value |= WLAN_IDX(cnt);
        Value |= RX_CNT_CLEAR;
        RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, Value);

        WaitCnt = 0;

        do {
            RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
            if ((Value & IU_BUSY)  == 0)
                break;
            WaitCnt++;
            RtmpusecDelay(50);
        } while (WaitCnt < 100);

        if (WaitCnt == 100) {
            DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
                        __FUNCTION__, Value));
#ifdef RTMP_USB_SUPPORT
			RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
			NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
            return;
        }

        /* TX Counter Clear */
        RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
        Value &= ~WLAN_IDX_MASK;
        Value |= WLAN_IDX(cnt);
        Value |= TX_CNT_CLEAR;
        RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, Value);

        WaitCnt = 0;

        do {
            RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
            if ((Value & IU_BUSY)  == 0)
                break;
            WaitCnt++;
            RtmpusecDelay(50);
        } while (WaitCnt < 100);

        if (WaitCnt == 100) {
            DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
                        __FUNCTION__, Value));

#ifdef RTMP_USB_SUPPORT
			RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
			NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
            return;
        }

        /* Admission Control Counter Clear */
        RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
        Value &= ~WLAN_IDX_MASK;
        Value |= WLAN_IDX(cnt);
        Value |= ADM_CNT_CLEAR;
        RTMP_IO_WRITE32(pAd, WTBL_OFF_WIUCR, Value);

        WaitCnt = 0;

        do {
            RTMP_IO_READ32(pAd, WTBL_OFF_WIUCR, &Value);
            if ((Value & IU_BUSY)  == 0)
                break;
            WaitCnt++;
            RtmpusecDelay(50);
        } while (WaitCnt < 100);

        if (WaitCnt == 100) {
            DBGPRINT(RT_DEBUG_ERROR, ("%s(): Previous update not applied by HW yet!(reg_val=0x%x)\n",
                        __FUNCTION__, Value));
#ifdef RTMP_USB_SUPPORT
			RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
			NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
            return;
        }

#ifdef RTMP_USB_SUPPORT
		RTMP_SEM_EVENT_UP(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_PCI_SUPPORT
		NdisReleaseSpinLock(&pAd->IndirectUpdateLock);
#endif
   }
}

VOID AsicStaUpdateGroupKey(IN	PRTMP_ADAPTER	pAd)
{
	UINT32	addr = 0, index = 0;
	UINT8	ucKeyLen = 0, keyIdx = 0;
	UCHAR	getKey[LEN_TKIP_TK];
	UINT32	*pGetKey = NULL;

	for (keyIdx = 1; keyIdx < 4; keyIdx++) {
		if (pAd->SharedKey[BSS0][keyIdx].CipherAlg == CIPHER_TKIP) {
			ucKeyLen = LEN_TKIP_TK;
			DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Cipher: %d\n",
				__func__, __LINE__, pAd->SharedKey[BSS0][keyIdx].CipherAlg));
		} else if (pAd->SharedKey[BSS0][keyIdx].CipherAlg == CIPHER_AES) {
			ucKeyLen = LEN_AES_TK;
			DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Cipher: %d\n",
				__func__, __LINE__, pAd->SharedKey[BSS0][keyIdx].CipherAlg));
		} else {
			DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support Cipher[%d]\n",
				__func__, __LINE__, pAd->SharedKey[BSS0][keyIdx].CipherAlg));
			continue;
		}

		addr = pAd->mac_ctrl.wtbl_base_addr[2] +
			MCAST_WCID * pAd->mac_ctrl.wtbl_entry_size[2];

		NdisZeroMemory(getKey, LEN_TKIP_TK);
		pGetKey = (UINT32 *)getKey;
		for (index = 0; index < ucKeyLen; index += 4) {
			RTMP_IO_READ32(pAd, addr + index, pGetKey++);

#ifdef RT_BIG_ENDIAN
			*pKey = SWAP32(*pKey);
#endif /* RT_BIG_ENDIAN */
		}

		DBGPRINT_RAW(RT_DEBUG_OFF, (" NEW KEY: "));
		for (index = 0; index < ucKeyLen; index++)
			DBGPRINT_RAW(RT_DEBUG_OFF, ("%02x ", getKey[index]));
		DBGPRINT_RAW(RT_DEBUG_OFF, ("\n"));

		DBGPRINT_RAW(RT_DEBUG_OFF, (" OLD KEY: "));
		for (index = 0; index < ucKeyLen; index++)
			DBGPRINT_RAW(RT_DEBUG_OFF, ("%02x ",
				pAd->SharedKey[BSS0][keyIdx].Key[index]));

		DBGPRINT_RAW(RT_DEBUG_OFF, ("\n"));

		NdisMoveMemory(pAd->SharedKey[BSS0][keyIdx].Key, getKey, ucKeyLen);
	}
}

VOID CmdProcAddRemoveKey(
        IN  PRTMP_ADAPTER 	pAd,
        IN	UCHAR			AddRemove,
        IN  UCHAR			BssIdx,
        IN	UCHAR			key_idx,
        IN 	UCHAR			Wcid,
        IN	UCHAR			KeyTabFlag,
        IN	PCIPHER_KEY		pCipherKey,
        IN	PUCHAR			PeerAddr)
{
    CMD_802_11_KEY CmdKey;
    UCHAR *pKey = pCipherKey->Key;
    UCHAR *pTxMic = pCipherKey->TxMic;
    UCHAR *pRxMic = pCipherKey->RxMic;
    UCHAR CipherAlg = pCipherKey->CipherAlg;
    struct wtbl_entry tb_entry;
    union WTBL_1_DW0 *dw0 = (union WTBL_1_DW0 *)&tb_entry.wtbl_1.wtbl_1_d0.word;
    union WTBL_1_DW1 *dw1 = (union WTBL_1_DW1 *)&tb_entry.wtbl_1.wtbl_1_d1.word;
    union WTBL_1_DW2 *dw2 = (union WTBL_1_DW2 *)&tb_entry.wtbl_1.wtbl_1_d2.word;
    //union WTBL_1_DW3 *dw3 = (union WTBL_1_DW3 *)&tb_entry.wtbl_1.wtbl_1_d3.word;
    //union WTBL_1_DW4 *dw4 = (union WTBL_1_DW4 *)&tb_entry.wtbl_1.wtbl_1_d4.word;
    //struct rtmp_mac_ctrl *wtbl_ctrl = &pAd->mac_ctrl;

    memset(&CmdKey, 0x00, sizeof(CMD_802_11_KEY));

    CmdKey.ucAddRemove = AddRemove;
    CmdKey.ucWlanIndex = Wcid;
    CmdKey.ucBssIndex = BssIdx;
    CmdKey.ucKeyId = key_idx;
    CmdKey.ucKeyType = KeyTabFlag;
    memcpy(CmdKey.aucPeerAddr, PeerAddr, 6);
    switch(CipherAlg)
    {
        case CIPHER_WEP64:
            CmdKey.ucAlgorithmId = 1;
            CmdKey.ucKeyLen = 5;
            memcpy(CmdKey.aucKeyMaterial, pKey, CmdKey.ucKeyLen);
            break;
        case CIPHER_WEP128:
            CmdKey.ucAlgorithmId = 5;
            CmdKey.ucKeyLen = 13;
            memcpy(CmdKey.aucKeyMaterial, pKey, CmdKey.ucKeyLen);
            break;
        case CIPHER_TKIP:
            CmdKey.ucAlgorithmId = 2;
            CmdKey.ucKeyLen = 32;
            memcpy(CmdKey.aucKeyMaterial, pKey, 16);
            memcpy(&CmdKey.aucKeyMaterial[16], pRxMic, 8);
            memcpy(&CmdKey.aucKeyMaterial[24], pTxMic, 8);
            break;
        case CIPHER_AES:
            CmdKey.ucAlgorithmId = 4;
            CmdKey.ucKeyLen = 16;
            memcpy(CmdKey.aucKeyMaterial, pKey, CmdKey.ucKeyLen);
            break;
        case CIPHER_SMS4:
            CmdKey.ucAlgorithmId = 8;
            CmdKey.ucKeyLen = 32;
            memcpy(CmdKey.aucKeyMaterial, pKey, 16);
#if 0
            memcpy(&CmdKey.aucKeyMaterial[16], pRxMic, 8);
            memcpy(&CmdKey.aucKeyMaterial[24], pTxMic, 8);
#else
		memcpy(&CmdKey.aucKeyMaterial[16], pTxMic, 8);
		memcpy(&CmdKey.aucKeyMaterial[24], pRxMic, 8);
#endif
            break;
        case CIPHER_WEP152:
            CmdKey.ucAlgorithmId = 7;
            CmdKey.ucKeyLen = 16;
            memcpy(CmdKey.aucKeyMaterial, pKey, CmdKey.ucKeyLen);
            break;
        case CIPHER_BIP:
            CmdKey.ucAlgorithmId = 6;
            CmdKey.ucKeyLen = 16;
            memcpy(CmdKey.aucKeyMaterial, pKey, CmdKey.ucKeyLen);
            break;
        default:
            if (CmdKey.ucAddRemove)
                break;
            DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support Cipher[%d] for HIF_MT yet!\n",
                        __FUNCTION__, __LINE__, CipherAlg));
            return;
    }

#if 0
    /* FW set security key */
    CmdSecKeyReq(pAd, CmdKey.ucAddRemove, CmdKey.ucKeyType, CmdKey.aucPeerAddr, CmdKey.ucAlgorithmId, CmdKey.ucKeyId, CmdKey.ucKeyLen, Wcid, CmdKey.aucKeyMaterial);

#else
    /* Driver set security key */
	if (CmdKey.ucAddRemove == 0) {
		DBGPRINT(RT_DEBUG_TRACE, ("add key table:wcid[%d]\n", CmdKey.ucWlanIndex));

		NdisZeroMemory(&tb_entry, sizeof(tb_entry));
		if (mt_wtbl_get_entry234(pAd, CmdKey.ucWlanIndex, &tb_entry) == FALSE) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s():Cannot found WTBL2/3/4 for WCID(%d)\n",
						__FUNCTION__, CmdKey.ucWlanIndex));
			return;
		}

		{
			UINT32 	addr = 0, index = 0, *pKey = (UINT32 *)CmdKey.aucKeyMaterial;
			addr = pAd->mac_ctrl.wtbl_base_addr[2] + CmdKey.ucWlanIndex * pAd->mac_ctrl.wtbl_entry_size[2];
			if ((CipherAlg == CIPHER_WEP64) || (CipherAlg == CIPHER_WEP128) || (CipherAlg == CIPHER_WEP152))
				addr = CmdKey.ucKeyId*16+addr;
			if (CipherAlg == CIPHER_SMS4)
				addr = CmdKey.ucKeyId*32+addr;
			if (CipherAlg == CIPHER_BIP)
				addr = addr+16;
			for (index=0;index<CmdKey.ucKeyLen;index+=4)
			{
#ifdef RT_BIG_ENDIAN
				*pKey=SWAP32(*pKey);
#endif /* RT_BIG_ENDIAN */
				RTMP_IO_WRITE32(pAd, addr+index, *(pKey++));
			}
		}

		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0], &dw0->word);
		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0]+4, &dw1->word);
		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0]+8, &dw2->word);

		dw0->field.wm = 0;
		if((Wcid != MCAST_WCID)
#ifdef APCLI_SUPPORT
          && (Wcid != APCLI_MCAST_WCID)
#endif /* APCLI_SUPPORT */
           )
        {
			dw0->field.addr_4 = CmdKey.aucPeerAddr[4];
			dw0->field.addr_5 = CmdKey.aucPeerAddr[5];
			dw1->word = CmdKey.aucPeerAddr[0] + (CmdKey.aucPeerAddr[1] << 8) +(CmdKey.aucPeerAddr[2] << 16) +(CmdKey.aucPeerAddr[3] << 24);
        }

#ifdef CONFIG_AP_SUPPORT
		if ((pAd->OpMode == OPMODE_AP)
#ifdef RT_CFG80211_P2P_SUPPORT
			 || (pAd->cfg80211_ctrl.isCfgInApMode == RT_CMD_80211_IFTYPE_AP)
#endif /* RT_CFG80211_P2P_SUPPORT */
		    )
		{
			if (KeyTabFlag == SHAREDKEYTABLE) {
				dw0->field.rv = 0;
				dw0->field.rkv = 0;
#ifdef APCLI_SUPPORT
            	if (Wcid == APCLI_MCAST_WCID) {
                	dw0->field.rv = 1;
                	dw0->field.rkv = 1;
            	}
#endif /* APCLI_SUPPORT */
#ifdef RT_CFG80211_P2P_SUPPORT
            	if (Wcid == BSSID_WCID ||
		    	    Wcid == MCAST_WCID)
				{
                	dw0->field.rv = 1;
                	dw0->field.rkv = 1;
					dw0->field.rc_id = 1;
					dw0->field.rc_a1 = 1;
					if (CipherAlg == CIPHER_SMS4)
						dw2->field.wpi_even = 1;
            	}
#endif /* RT_CFG80211_P2P_SUPPORT */
			}
			else {
				dw0->field.rv = 1;
				dw0->field.rkv = 1;
			}
		}
		else
#endif /* CONFIG_AP_SUPPORT */
{
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			dw0->field.rv = 1;
	        dw0->field.rkv = 1;
			dw0->field.rc_a1 = 1;
			dw0->field.rc_id = 1;
			if (CipherAlg == CIPHER_SMS4)
				dw2->field.wpi_even = 1;
		}
#endif /* CONFIG_STA_SUPPORT */
}
		dw2->field.adm = 1;
		dw2->field.cipher_suit = CmdKey.ucAlgorithmId;
		dw0->field.kid = CmdKey.ucKeyId;

		RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 8, dw2->word);
		RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 4, dw1->word);
		RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0], dw0->word);

		DBGPRINT(RT_DEBUG_INFO, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
					__FUNCTION__, CmdKey.ucWlanIndex, tb_entry.wtbl_addr[0], dw0->word));
		DBGPRINT(RT_DEBUG_INFO, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
					__FUNCTION__, CmdKey.ucWlanIndex,  tb_entry.wtbl_addr[0] + 4, dw1->word));
		DBGPRINT(RT_DEBUG_INFO, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
					__FUNCTION__, CmdKey.ucWlanIndex, tb_entry.wtbl_addr[0] + 8, dw2->word));

	} else {
		DBGPRINT(RT_DEBUG_TRACE, ("remove key table:wcid[%d]\n", CmdKey.ucWlanIndex));

		NdisZeroMemory(&tb_entry, sizeof(tb_entry));
		if (mt_wtbl_get_entry234(pAd, CmdKey.ucWlanIndex, &tb_entry) == FALSE) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s():Cannot found WTBL2/3/4 for WCID(%d)\n",
						__FUNCTION__, CmdKey.ucWlanIndex));
			return;
		}

		{
			UINT32 	addr = 0, index = 0;
			addr = pAd->mac_ctrl.wtbl_base_addr[2] + CmdKey.ucWlanIndex * pAd->mac_ctrl.wtbl_entry_size[2];
			if ((CipherAlg == CIPHER_WEP64) || (CipherAlg == CIPHER_WEP128) || (CipherAlg == CIPHER_WEP152))
				addr = CmdKey.ucKeyId*16+addr;

			for (index=0;index<CmdKey.ucKeyLen;index+=4)
				RTMP_IO_WRITE32(pAd, addr+index, 0);
		}

		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0], &dw0->word);
		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0]+4, &dw1->word);
		RTMP_IO_READ32(pAd, tb_entry.wtbl_addr[0]+8, &dw2->word);

		dw0->field.wm = 0;
		dw0->field.addr_4 = CmdKey.aucPeerAddr[4];
		dw0->field.addr_5 = CmdKey.aucPeerAddr[5];
		dw1->word = CmdKey.aucPeerAddr[0] + (CmdKey.aucPeerAddr[1] << 8) +(CmdKey.aucPeerAddr[2] << 16) +(CmdKey.aucPeerAddr[3] << 24);

		dw0->field.rv = 0;
		dw2->field.adm = 0;
		dw0->field.rkv = 0;
		dw2->field.cipher_suit = 0;
		dw0->field.kid = 0;

		RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0], dw0->word);
		RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 4, dw1->word);
		RTMP_IO_WRITE32(pAd, tb_entry.wtbl_addr[0] + 8, dw2->word);

		DBGPRINT(RT_DEBUG_TRACE, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
					__FUNCTION__, CmdKey.ucWlanIndex, tb_entry.wtbl_addr[0], dw0->word));

		DBGPRINT(RT_DEBUG_TRACE, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
					__FUNCTION__, CmdKey.ucWlanIndex,  tb_entry.wtbl_addr[0] + 4, dw1->word));

		DBGPRINT(RT_DEBUG_TRACE, ("%s(%d):Write WTBL 1 Addr:0x%x, Value:0x%x\n",
					__FUNCTION__, CmdKey.ucWlanIndex, tb_entry.wtbl_addr[0] + 8, dw2->word));


	}

	//dump_wtbl_info(pAd, Wcid);
#endif
}

/*
	========================================================================
	Description:
		Add Pair-wise key material into ASIC.
		Update pairwise key, TxMic and RxMic to Asic Pair-wise key table

    Return:
	========================================================================
*/
VOID AsicAddPairwiseKeyEntry(
	IN PRTMP_ADAPTER 	pAd,
	IN UCHAR			WCID,
	IN PCIPHER_KEY		pCipherKey)
{
	INT i;
	ULONG offset;
	UINT32 pairwise_key_base = 0, pairwise_key_len = 0;
	UCHAR *pKey = pCipherKey->Key;
	UCHAR *pTxMic = pCipherKey->TxMic;
	UCHAR *pRxMic = pCipherKey->RxMic;
	UCHAR CipherAlg = pCipherKey->CipherAlg;
#ifdef RTMP_MAC
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT
	unsigned long irqFlag = 0;
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */
#endif /* RTMP_MAC */

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_INFO, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
		pairwise_key_base = RLT_PAIRWISE_KEY_TABLE_BASE;
		pairwise_key_len = RLT_HW_KEY_ENTRY_SIZE;
	}
#endif /* RLT_MAC */
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP) {
		pairwise_key_base = PAIRWISE_KEY_TABLE_BASE;
		pairwise_key_len = HW_KEY_ENTRY_SIZE;
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT
		RTMP_MAC_SHR_MSEL_LOCK(pAd, LOWER_SHRMEM, irqFlag);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */
	}
#endif /* RTMP_MAC */

	/* EKEY */
	offset = pairwise_key_base + (WCID * pairwise_key_len);
#ifdef RTMP_MAC_PCI
	for (i=0; i<MAX_LEN_OF_PEER_KEY; i++)
	{
		RTMP_IO_WRITE8(pAd, offset + i, pKey[i]);
	}
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
	RTUSBMultiWrite(pAd, (USHORT)offset, &pCipherKey->Key[0], MAX_LEN_OF_PEER_KEY, FALSE);
#endif /* RTMP_MAC_USB */
	for (i=0; i<MAX_LEN_OF_PEER_KEY; i+=4)
	{
		UINT32 Value;
		RTMP_IO_READ32(pAd, offset + i, &Value);
	}
	offset += MAX_LEN_OF_PEER_KEY;

	/*  MIC KEY */
	if (pTxMic)
	{
#ifdef RTMP_MAC_PCI
		for (i=0; i<8; i++)
		{
			RTMP_IO_WRITE8(pAd, offset+i, pTxMic[i]);
		}
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		RTUSBMultiWrite(pAd, (USHORT)offset, &pCipherKey->TxMic[0], 8, FALSE);
#endif /* RTMP_MAC_USB */
	}
	offset += 8;

	if (pRxMic)
	{
#ifdef RTMP_MAC_PCI
		for (i=0; i<8; i++)
		{
			RTMP_IO_WRITE8(pAd, offset+i, pRxMic[i]);
		}
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		RTUSBMultiWrite(pAd, (USHORT)offset, &pCipherKey->RxMic[0], 8, FALSE);
#endif /* RTMP_MAC_USB */
	}
#ifdef RTMP_MAC
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT
	if (pAd->chipCap.hif_type == HIF_RTMP) {
		RTMP_MAC_SHR_MSEL_UNLOCK(pAd, LOWER_SHRMEM, irqFlag);
	}
#endif /* SPECIFIC_BCN_BUF_SUPPORT*/
#endif /* RTMP_MAC_PCI */
#endif /* RTMP_MAC */
	DBGPRINT(RT_DEBUG_TRACE,("AsicAddPairwiseKeyEntry: WCID #%d Alg=%s\n",WCID, CipherName[CipherAlg]));
	DBGPRINT(RT_DEBUG_TRACE,("	Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
		pKey[0],pKey[1],pKey[2],pKey[3],pKey[4],pKey[5],pKey[6],pKey[7],pKey[8],pKey[9],pKey[10],pKey[11],pKey[12],pKey[13],pKey[14],pKey[15]));
	if (pRxMic)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("	Rx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
			pRxMic[0],pRxMic[1],pRxMic[2],pRxMic[3],pRxMic[4],pRxMic[5],pRxMic[6],pRxMic[7]));
	}
	if (pTxMic)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("	Tx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
			pTxMic[0],pTxMic[1],pTxMic[2],pTxMic[3],pTxMic[4],pTxMic[5],pTxMic[6],pTxMic[7]));
	}
}


/*
	========================================================================
	Description:
		Remove Pair-wise key material from ASIC.

    Return:
	========================================================================
*/
VOID AsicRemovePairwiseKeyEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 Wcid)
{
	/* Set the specific WCID attribute entry as OPEN-NONE */
	AsicUpdateWcidAttributeEntry(pAd,
							  BSS0,
							  0,
							  CIPHER_NONE,
							  Wcid,
							  PAIRWISEKEYTABLE);
}


BOOLEAN AsicSendCommandToMcu(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR Command,
	IN UCHAR Token,
	IN UCHAR Arg0,
	IN UCHAR Arg1,
	IN BOOLEAN in_atomic)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_TRACE, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return TRUE;
	}

#ifdef RT65xx
	// TODO: shiang-6590, fix me, currently firmware is not ready yet, so ignore it!
	if (IS_RT65XX(pAd))
		return TRUE;
#endif /* RT65xx */

#ifdef MT7601
	if (IS_MT7601(pAd))
		return TRUE;
#endif /* MT7601 */

#ifdef RTMP_MAC_PCI
	if (IS_PCI_INF(pAd) || IS_RBUS_INF(pAd))
		in_atomic = TRUE;
#endif /* RTMP_MAC_PCI */
	if (pAd->chipOps.sendCommandToMcu)
		return pAd->chipOps.sendCommandToMcu(pAd, Command, Token, Arg0, Arg1, in_atomic);
	else
		return FALSE;
}


BOOLEAN AsicSendCmdToMcuAndWait(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR Command,
	IN UCHAR Token,
	IN UCHAR Arg0,
	IN UCHAR Arg1,
	IN BOOLEAN in_atomic)
{
	BOOLEAN cmd_done = TRUE;

	AsicSendCommandToMcu(pAd, Command, Token, Arg0, Arg1, in_atomic);

#ifdef RTMP_MAC_PCI
	cmd_done = AsicCheckCommanOk(pAd, Token);
#endif /* RTMP_MAC_PCI */

	return cmd_done;
}


BOOLEAN AsicSendCommandToMcuBBP(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 Command,
	IN UCHAR		 Token,
	IN UCHAR		 Arg0,
	IN UCHAR		 Arg1,
	IN BOOLEAN		FlgIsNeedLocked)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return TRUE;
	}

#ifdef RT65xx
	// TODO: shiang-6590, fix me, currently firmware is not ready yet, so ignore it!
	if (IS_RT65XX(pAd))
		return TRUE;
#endif /* RT65xx */

#ifdef MT7601
	if (IS_MT7601(pAd))
		return TRUE;
#endif /* MT7601 */

	if (pAd->chipOps.sendCommandToMcu)
		return pAd->chipOps.sendCommandToMcu(pAd, Command, Token, Arg0, Arg1, FlgIsNeedLocked);
	else
		return FALSE;
}


/*
	========================================================================
	Description:
		For 1x1 chipset : 2070 / 3070 / 3090 / 3370 / 3390 / 5370 / 5390
		Usage :	1. Set Default Antenna as initialize
				2. Antenna Diversity switching used
				3. iwpriv command switch Antenna

    Return:
	========================================================================
 */
VOID AsicSetRxAnt(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			Ant)
{
	if (pAd->chipOps.SetRxAnt)
		pAd->chipOps.SetRxAnt(pAd, Ant);
}


VOID AsicTurnOffRFClk(RTMP_ADAPTER *pAd, UCHAR Channel)
{
	if (pAd->chipOps.AsicRfTurnOff)
	{
		pAd->chipOps.AsicRfTurnOff(pAd);
	}
	else
	{
#if defined(RT28xx) || defined(RT2880) || defined(RT2883)
		/* RF R2 bit 18 = 0*/
		UINT32			R1 = 0, R2 = 0, R3 = 0;
		UCHAR			index;
		RTMP_RF_REGS	*RFRegTable;

		RFRegTable = RF2850RegTable;
#endif /* defined(RT28xx) || defined(RT2880) || defined(RT2883) */

		switch (pAd->RfIcType)
		{
#if defined(RT28xx) || defined(RT2880) || defined(RT2883)
#if defined(RT28xx) || defined(RT2880)
			case RFIC_2820:
			case RFIC_2850:
			case RFIC_2720:
			case RFIC_2750:
#endif /* defined(RT28xx) || defined(RT2880) */
#ifdef RT2883
			case RFIC_2853:
#endif /* RT2883 */
				for (index = 0; index < NUM_OF_2850_CHNL; index++)
				{
					if (Channel == RFRegTable[index].Channel)
					{
						R1 = RFRegTable[index].R1 & 0xffffdfff;
						R2 = RFRegTable[index].R2 & 0xfffbffff;
						R3 = RFRegTable[index].R3 & 0xfff3ffff;

						RTMP_RF_IO_WRITE32(pAd, R1);
						RTMP_RF_IO_WRITE32(pAd, R2);

						/* Program R1b13 to 1, R3/b18,19 to 0, R2b18 to 0. */
						/* Set RF R2 bit18=0, R3 bit[18:19]=0*/
						/*if (pAd->StaCfg.bRadio == FALSE)*/
						if (1)
						{
							RTMP_RF_IO_WRITE32(pAd, R3);

							DBGPRINT(RT_DEBUG_TRACE, ("AsicTurnOffRFClk#%d(RF=%d, ) , R2=0x%08x,  R3 = 0x%08x \n",
								Channel, pAd->RfIcType, R2, R3));
						}
						else
							DBGPRINT(RT_DEBUG_TRACE, ("AsicTurnOffRFClk#%d(RF=%d, ) , R2=0x%08x \n",
								Channel, pAd->RfIcType, R2));
						break;
					}
				}
				break;
#endif /* defined(RT28xx) || defined(RT2880) || defined(RT2883) */
			default:
				DBGPRINT(RT_DEBUG_TRACE, ("AsicTurnOffRFClk#%d : Unkonwn RFIC=%d\n",
											Channel, pAd->RfIcType));
				break;
		}
	}
}


#ifdef WAPI_SUPPORT
VOID AsicUpdateWAPIPN(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		 WCID,
	IN ULONG         pn_low,
	IN ULONG         pn_high)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}

	if (IS_HW_WAPI_SUPPORT(pAd))
	{
		UINT32 offset;
		UINT32 wapi_pn_base = 0, wapi_pn_size = 0;
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
			wapi_pn_base = RLT_WAPI_PN_TABLE_BASE;
			wapi_pn_size = RLT_WAPI_PN_ENTRY_SIZE;
		}
#endif /* RLT_MAC */
#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			wapi_pn_base = WAPI_PN_TABLE_BASE;
			wapi_pn_size = WAPI_PN_ENTRY_SIZE;
		}
#endif /* RTMP_MAC */

		offset = wapi_pn_base + (WCID * wapi_pn_size);

		RTMP_IO_WRITE32(pAd, offset, pn_low);
		RTMP_IO_WRITE32(pAd, offset + 4, pn_high);
	}
	else
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s : Not support HW_WAPI_PN_TABLE\n",
								__FUNCTION__));
	}

}
#endif /* WAPI_SUPPORT */



#ifdef VCORECAL_SUPPORT
VOID AsicVCORecalibration(RTMP_ADAPTER *pAd)
{

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}
}
#endif /* VCORECAL_SUPPORT */


#ifdef STREAM_MODE_SUPPORT
// StreamModeRegVal - return MAC reg value for StreamMode setting
UINT32 StreamModeRegVal(
	IN RTMP_ADAPTER *pAd)
{
	UINT32 streamWord;

	switch (pAd->CommonCfg.StreamMode)
	{
		case 1:
			streamWord = 0x030000;
			break;
		case 2:
			streamWord = 0x0c0000;
			break;
		case 3:
			streamWord = 0x0f0000;
			break;
		default:
			streamWord = 0x0;
			break;
	}

	return streamWord;
}


/*
	========================================================================
	Description:
		configure the stream mode of specific MAC or all MAC and set to ASIC.

	Prameters:
		pAd		 ---
		pMacAddr ---
		bClear	 --- disable the stream mode for specific macAddr when
						(pMacAddr!=NULL)

    Return:
	========================================================================
*/
VOID AsicSetStreamMode(
	IN RTMP_ADAPTER *pAd,
	IN PUCHAR pMacAddr,
	IN INT chainIdx,
	IN BOOLEAN bEnabled)
{
	UINT32 streamWord;
	UINT32 regAddr, regVal;


	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}

	if (!pAd->chipCap.FlgHwStreamMode)
		return;

	streamWord = StreamModeRegVal(pAd);
	if (!bEnabled)
		streamWord = 0;

	regAddr = TX_CHAIN_ADDR0_L + chainIdx * 8;
	RTMP_IO_WRITE32(pAd, regAddr,
					(UINT32)(pMacAddr[0]) |
					(UINT32)(pMacAddr[1] << 8)  |
					(UINT32)(pMacAddr[2] << 16) |
					(UINT32)(pMacAddr[3] << 24));

	RTMP_IO_READ32(pAd, regAddr + 4, &regVal);
	regVal &= (~0x000f0000);
	RTMP_IO_WRITE32(pAd, regAddr + 4,
					(regVal | streamWord) |
					(UINT32)(pMacAddr[4]) |
					(UINT32)(pMacAddr[5] << 8));

#if 0
    UINT32 regBase, reg;
	BOOLEAN bSyncAll;
	int i, emptyIdx, actionIdx;
	STREAM_MODE_ENTRY *pSMEntry;

	// Do operation for a specific mac entry
	if (pMacAddr)
	{
		/// find if the entry exist in the StreaModeSta table
		for (i = 0; i < STREAM_MODE_STA_NUM; i++)
		{
			pSMEntry = &pAd->CommonCfg.StreamModeMac[i];
			if (NdisEqualMemory(&pSMEntry->macAddr[0], pMacAddr, MAC_ADDR_LEN))
			{
				pSMEntry->flag |= STREAM_MODE_SYNCREQ;
				break;
			}
		}
		if (i == STREAM_MODE_STA_NUM)
		{

		}
		bSyncAll = FALSE;
	}
	else
	{
		bSyncAll = TRUE;
	}

	regBase = TX_CHAIN_ADDR0_L;
	for (i = 0; i < STREAM_MODE_STA_NUM; i++)
	{
		pSMEntry = &pAd->CommonCfg.StreamModeMac[i];
		if (bSyncAll || (pSMEntry->flag & STREAM_MODE_SYNCREQ))
		{

		}
	}
#endif
}


VOID RtmpStreamModeInit(RTMP_ADAPTER *pAd)
{
	int chainIdx;
	UCHAR *pMacAddr;

	if (pAd->chipCap.FlgHwStreamMode == FALSE)
		return;

	for (chainIdx = 0; chainIdx < STREAM_MODE_STA_NUM; chainIdx++)
	{
		pMacAddr = &pAd->CommonCfg.StreamModeMac[chainIdx][0];
		AsicSetStreamMode(pAd, pMacAddr, chainIdx, TRUE);
	}
}
#endif // STREAM_MODE_SUPPORT //


VOID AsicSetTxPreamble(RTMP_ADAPTER *pAd, USHORT TxPreamble)
{
	//AUTO_RSP_CFG_STRUC csr4;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}
#if 0
	RTMP_IO_READ32(pAd, AUTO_RSP_CFG, &csr4.word);
	if (TxPreamble == Rt802_11PreambleLong)
		csr4.field.AutoResponderPreamble = 0;
	else
		csr4.field.AutoResponderPreamble = 1;
	RTMP_IO_WRITE32(pAd, AUTO_RSP_CFG, csr4.word);
#endif
}


#ifdef DOT11_N_SUPPORT
INT AsicSetRalinkBurstMode(RTMP_ADAPTER *pAd, BOOLEAN enable)
{
#ifdef MT_MAC
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return FALSE;
	}
#else
	UINT32 Data = 0;

	RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &Data);
	if (enable)
	{
		pAd->CommonCfg.RestoreBurstMode = Data;
		Data  &= 0xFFF00000;
		Data  |= 0x86380;
	} else {
		Data = pAd->CommonCfg.RestoreBurstMode;
		Data &= 0xFFFFFF00;

		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_DYNAMIC_BE_TXOP_ACTIVE)
#ifdef DOT11_N_SUPPORT
			&& (pAd->MacTab.fAnyStationMIMOPSDynamic == FALSE)
#endif // DOT11_N_SUPPORT //
		)
		{
			if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE))
				Data |= 0x80;
			else if (pAd->CommonCfg.bEnableTxBurst)
				Data |= 0x20;
		}
	}
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);

	if (enable)
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RALINK_BURST_MODE);
	else
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RALINK_BURST_MODE);

#endif /* MT_MAC */
	return TRUE;
}


INT AsicUpdateTxOP(RTMP_ADAPTER *pAd, UINT32 ac_num, UINT32 txop_val)
{
    UINT32 last_txop_val;

    if (pAd->CommonCfg.ManualTxop)
    {
        return TRUE;
    }

    last_txop_val = pAd->CurrEdcaParam[ac_num].u2Txop;

    if (last_txop_val == txop_val)
    { /* No need to Update TxOP CR */
        return TRUE;
    }
    else if (last_txop_val == 0xdeadbeef)
    {
        DBGPRINT(RT_DEBUG_ERROR, ("%s: Error CR value for TxOP = 0x%08x\n", __FUNCTION__, last_txop_val));

        return FALSE;
    }
    else {}

    AsicSetWmmParam(pAd, ac_num, WMM_PARAM_TXOP, txop_val);

    return TRUE;
}
#endif // DOT11_N_SUPPORT //


#ifdef WOW_SUPPORT
#ifdef RTMP_MAC_USB

/* switch firmware
   a) before into WOW mode, switch firmware to WOW-enable firmware
   b) exit from WOW mode, switch firmware to normal firmware
*/
VOID AsicLoadWOWFirmware(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN WOW)
{
	if (WOW)
		pAd->WOW_Cfg.bWOWFirmware = TRUE;
	else
		pAd->WOW_Cfg.bWOWFirmware = FALSE;

	RtmpAsicLoadFirmware(pAd);
}

/* In WOW mode, 8051 mcu will send null frame, and pick data from 0x7780
 * the null frame includes TxWI and 802.11 header 						*/
VOID AsicWOWSendNullFrame(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR TxRate,
	IN BOOLEAN bQosNull)
{
	TXWI_STRUC *TxWI;
	PUCHAR NullFrame;
	UINT8  packet_len;
	PUCHAR ptr;
	USHORT offset;
	UINT32 cipher = pAd->StaCfg.GroupCipher;
	UINT32 Value;
	UINT8 TXWISize = pAd->chipCap.TXWISize;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}

	ComposeNullFrame(pAd);
	TxWI = (TXWI_STRUC *)&pAd->NullContext.TransferBuffer->field.WirelessPacket[TXINFO_SIZE];
	NullFrame = (PUCHAR)&pAd->NullFrame;
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
#ifdef MT7601
		if (IS_MT7601(pAd))
			packet_len = TxWI->TXWI_X.MPDUtotalByteCnt;
		else
#endif /* MT7601 */
			packet_len = TxWI->TXWI_N.MPDUtotalByteCnt;
	}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		packet_len = TxWI->TXWI_O.MPDUtotalByteCnt;
#endif /* RTMP_MAC */

	DBGPRINT(RT_DEBUG_OFF, ("TxWI:\n"));
	/* copy TxWI to MCU memory */
#ifdef RTMP_MAC_PCI
	ptr = (PUCHAR)&TxWI;
#else
	ptr = (PUCHAR)TxWI;
#endif /* RTMP_MAC_PCI */
	for (offset = 0; offset < TXWISize; offset += 4)
	{
		RTMPMoveMemory(&Value, ptr+offset, 4);
		DBGPRINT(RT_DEBUG_OFF, ("offset: %02d %08x\n", offset, Value));
		RTMP_IO_WRITE32(pAd, HW_NULL2_BASE + offset, Value);
	}

	DBGPRINT(RT_DEBUG_OFF, ("802.11 header:\n"));
	/* copy 802.11 header to memory */
	ptr = (PUCHAR)NullFrame;
	for (offset = 0; offset < packet_len; offset += 4)
	{
		RTMPMoveMemory(&Value, ptr+offset, 4);
		DBGPRINT(RT_DEBUG_OFF, ("offset: %02d %08x\n", offset, Value));
		RTMP_IO_WRITE32(pAd, HW_NULL2_BASE + TXWISize + offset, Value);
	}

	DBGPRINT(RT_DEBUG_OFF, ("Write GroupCipher Mode: %d\n", pAd->StaCfg.GroupCipher));

	{
		UINT32 share_key_mode_base = 0;
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT)
			share_key_mode_base= RLT_SHARED_KEY_MODE_BASE;
#endif /* RLT_MAC */
#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP)
			share_key_mode_base = SHARED_KEY_MODE_BASE;
#endif /* RTMP_MAC */

		RTMP_IO_READ32(pAd, share_key_mode_base, &Value);
		switch (cipher) /* don't care WEP, because it dosen't have re-key issue */
		{
			case Ndis802_11TKIPEnable: /* TKIP */
				Value |= 0x0330;
				break;
			case Ndis802_11AESEnable: /* AES */
				Value |= 0x0440;
				break;
		}
		RTMP_IO_WRITE32(pAd, share_key_mode_base, Value);
	}
}

#endif /* RTMP_MAC_USB */
#endif /* WOW_SUPPORT */


#ifdef MICROWAVE_OVEN_SUPPORT
VOID AsicMeasureFalseCCA(RTMP_ADAPTER *pAd)
{
	if (pAd->chipOps.AsicMeasureFalseCCA)
		pAd->chipOps.AsicMeasureFalseCCA(pAd);
}

VOID AsicMitigateMicrowave(RTMP_ADAPTER *pAd)
{
	if (pAd->chipOps.AsicMitigateMicrowave)
		pAd->chipOps.AsicMitigateMicrowave(pAd);
}
#endif /* MICROWAVE_OVEN_SUPPORT */


INT AsicWaitMacTxRxIdle(RTMP_ADAPTER *pAd)
{
	return TRUE;
}


INT32 AsicSetMacTxRx(RTMP_ADAPTER *pAd, INT32 TxRx, BOOLEAN Enable)
{
	UINT32 Value, Value1, Value2;

	RTMP_IO_READ32(pAd, ARB_SCR, &Value);
	RTMP_IO_READ32(pAd, ARB_TQCR0, &Value1);
	RTMP_IO_READ32(pAd, ARB_RQCR, &Value2);

	switch (TxRx)
	{
		case ASIC_MAC_TX:
			if (Enable)
			{
				Value &= ~MT_ARB_SCR_TXDIS;
				Value1 = 0xffffffff;
			}
			else
			{
				Value |= MT_ARB_SCR_TXDIS;
				Value1 = 0;
			}
			break;
		case ASIC_MAC_RX:
			if (Enable)
			{
				Value &= ~MT_ARB_SCR_RXDIS;
				Value2 |= ARB_RQCR_RX_START;
			}
			else
			{
				Value |= MT_ARB_SCR_RXDIS;
				Value2 &= ~ARB_RQCR_RX_START;
			}
			break;
		case ASIC_MAC_TXRX:
			if (Enable)
			{
				Value &= ~(MT_ARB_SCR_TXDIS | MT_ARB_SCR_RXDIS);
				Value1 = 0xffffffff;
				Value2 |= ARB_RQCR_RX_START;
			}
			else
			{
				Value |= (MT_ARB_SCR_TXDIS | MT_ARB_SCR_RXDIS);
				Value1 = 0;
				Value2 &= ~ARB_RQCR_RX_START;

			}
			break;
		case ASIC_MAC_TXRX_RXV:
			if (Enable)
			{
				Value &= ~(MT_ARB_SCR_TXDIS | MT_ARB_SCR_RXDIS);
				Value1 = 0xffffffff;
				Value2 |= (ARB_RQCR_RX_START | ARB_RQCR_RXV_START |
							ARB_RQCR_RXV_R_EN |	ARB_RQCR_RXV_T_EN);
			}
			else
			{
				Value |= (MT_ARB_SCR_TXDIS | MT_ARB_SCR_RXDIS);
				Value1 = 0;
				Value2 &= ~(ARB_RQCR_RX_START | ARB_RQCR_RXV_START |
							ARB_RQCR_RXV_R_EN |	ARB_RQCR_RXV_T_EN);
			}
			break;
		case ASIC_MAC_RXV:
			if (Enable)
			{
				Value &= ~MT_ARB_SCR_RXDIS;
				Value2 |= (ARB_RQCR_RXV_START |
							ARB_RQCR_RXV_R_EN |	ARB_RQCR_RXV_T_EN);
			}
			else
			{
				Value2 &= ~(ARB_RQCR_RXV_START |
							ARB_RQCR_RXV_R_EN |	ARB_RQCR_RXV_T_EN);
			}
			break;
		case ASIC_MAC_RX_RXV:
			if (Enable)
			{

				Value &= ~MT_ARB_SCR_RXDIS;
				Value2 |= (ARB_RQCR_RX_START | ARB_RQCR_RXV_START |
							ARB_RQCR_RXV_R_EN | ARB_RQCR_RXV_T_EN);

			}
			else
			{
				Value |= MT_ARB_SCR_RXDIS;
				Value2 &= ~(ARB_RQCR_RX_START | ARB_RQCR_RXV_START |
							ARB_RQCR_RXV_R_EN |	ARB_RQCR_RXV_T_EN);
			}
			break;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Unknown path (%d\n", __FUNCTION__,
										TxRx));
			break;
	}

	RTMP_IO_WRITE32(pAd, ARB_SCR, Value);
	RTMP_IO_WRITE32(pAd, ARB_TQCR0, Value1);
	RTMP_IO_WRITE32(pAd, ARB_RQCR, Value2);

	return TRUE;
}


INT AsicSetWPDMA(RTMP_ADAPTER *pAd, INT32 TxRx, BOOLEAN enable)
{
#ifdef RTMP_MAC_PCI
	WPDMA_GLO_CFG_STRUC GloCfg;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type != HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Invalid HIF type(%d)!\n",
							__FUNCTION__, __LINE__, pAd->chipCap.hif_type));
		return FALSE;
	}

	RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG , &GloCfg.word);

	switch (TxRx)
	{
		case PDMA_TX:
			if (enable == TRUE)
			{
				GloCfg.field.EnableTxDMA = 1;
				GloCfg.field.EnTXWriteBackDDONE = 1;
				GloCfg.field.WPDMABurstSIZE = pAd->chipCap.WPDMABurstSIZE;
			} else {
				GloCfg.field.EnableTxDMA = 0;
				GloCfg.field.EnTXWriteBackDDONE = 0;
			}
			break;
		case PDMA_RX:
			if (enable == TRUE)
			{
				GloCfg.field.EnableRxDMA = 1;
				GloCfg.field.WPDMABurstSIZE = pAd->chipCap.WPDMABurstSIZE;
			} else {
				GloCfg.field.EnableRxDMA = 0;
			}
			break;
		case PDMA_TX_RX:
			if (enable == TRUE)
			{
				GloCfg.field.EnableTxDMA = 1;
				GloCfg.field.EnableRxDMA = 1;
				GloCfg.field.EnTXWriteBackDDONE = 1;
				GloCfg.field.WPDMABurstSIZE = pAd->chipCap.WPDMABurstSIZE;
			} else {
				GloCfg.field.EnableRxDMA = 0;
				GloCfg.field.EnableTxDMA = 0;
				GloCfg.field.EnTXWriteBackDDONE = 0;
			}
			break;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Unknown path (%d\n", __FUNCTION__, TxRx));
			break;
	}

	RTMP_IO_WRITE32(pAd, MT_WPDMA_GLO_CFG, GloCfg.word);
#endif

	return TRUE;
}


BOOLEAN AsicWaitPDMAIdle(struct _RTMP_ADAPTER *pAd, INT round, INT wait_us)
{
#ifdef RTMP_MAC_PCI
	INT i = 0;
	WPDMA_GLO_CFG_STRUC GloCfg;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type != HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Invalid HIF type(%d)!\n",
							__FUNCTION__, __LINE__, pAd->chipCap.hif_type));
		return FALSE;
	}

	do {
		RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG, &GloCfg.word);
		if ((GloCfg.field.TxDMABusy == 0)  && (GloCfg.field.RxDMABusy == 0)) {
			DBGPRINT(RT_DEBUG_TRACE, ("==>  DMAIdle, GloCfg=0x%x\n", GloCfg.word));
			return TRUE;
		}
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return FALSE;
		RtmpusecDelay(wait_us);
	}while ((i++) < round);

	DBGPRINT(RT_DEBUG_TRACE, ("==>  DMABusy, GloCfg=0x%x\n", GloCfg.word));

	return FALSE;
#endif
	return TRUE;
}


INT AsicSetMacWD(RTMP_ADAPTER *pAd)
{
	return TRUE;
}


INT rtmp_asic_hif_init(RTMP_ADAPTER *pAd)
{

	return TRUE;
}


INT StopDmaRx(RTMP_ADAPTER *pAd, UCHAR Level)
{
	PNDIS_PACKET pRxPacket;
	RX_BLK RxBlk, *pRxBlk;
	UINT32 RxPending = 0, MacReg = 0, MTxCycle = 0;
	BOOLEAN bReschedule = FALSE;
#ifdef RTMP_MAC_USB
	UINT8 IdleNums = 0;
#endif /* RTMP_MAC_USB */

	DBGPRINT(RT_DEBUG_TRACE, ("====> %s\n", __FUNCTION__));

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return 0;
	}

	/*
		process whole rx ring
	*/
	while (1)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return 0;
		pRxBlk = &RxBlk;
		pRxPacket = GetPacketFromRxRing(pAd, pRxBlk, &bReschedule, &RxPending, 0);
		if ((RxPending == 0) && (bReschedule == FALSE))
			break;
		else if (pRxPacket)
			RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_SUCCESS);
	}

	/*
		Check DMA Rx idle
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
#ifdef RTMP_MAC_PCI

		RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG, &MacReg);
		if (MacReg & 0x8)
		{
			RtmpusecDelay(50);
		}
		else
			break;

#endif

#ifdef RTMP_MAC_USB
		USB_CFG_READ(pAd, &MacReg);
		if ((MacReg & 0x40000000) && (IdleNums < 10))
		{
			IdleNums++;
			RtmpusecDelay(50);
		}
		else
		{
			break;
		}
#endif

		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return 0;
		}
	}

	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s:RX DMA busy!! DMA_CFG = 0x%08x\n", __FUNCTION__, MacReg));
	}

	if (Level == RTMP_HALT)
	{
		/* Disable DMA RX */
#ifdef RTMP_MAC_PCI
		RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG, &MacReg);
		MacReg &= ~(0x4);
		RTMP_IO_WRITE32(pAd, MT_WPDMA_GLO_CFG, MacReg);
#endif
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<==== %s\n", __FUNCTION__));

	return 0;
}


INT StopDmaTx(RTMP_ADAPTER *pAd, UCHAR Level)
{
	UINT32 MacReg = 0, MTxCycle = 0;
#ifdef RTMP_MAC_USB
	UINT8 IdleNums = 0;
#endif

	DBGPRINT(RT_DEBUG_TRACE, ("====> %s\n", __FUNCTION__));

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return 0;
	}

	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
#ifdef RTMP_MAC_PCI
		RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG, &MacReg);
		if ((MacReg & 0x2) == 0)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("==>  DMA Tx Idle, MacReg=0x%x\n", MacReg));
			break;
		}
#endif

#ifdef RTMP_MAC_USB
		USB_CFG_READ(pAd, &MacReg);
		if (((MacReg & 0x80000000) == 0) && IdleNums > 10)
		{
			break;
		}
		else
		{
			IdleNums++;
			RtmpusecDelay(50);
		}
#endif

		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return 0;
		}
	}

	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("TX DMA busy!! DMA_CFG(%x)\n", MacReg));
	}

	if (Level == RTMP_HALT)
	{
#ifdef RTMP_MAC_PCI
		RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG, &MacReg);
		MacReg &= ~(0x00000001);
		RTMP_IO_WRITE32(pAd, MT_WPDMA_GLO_CFG, MacReg);
#endif

	}

	DBGPRINT(RT_DEBUG_TRACE, ("<==== %s\n", __FUNCTION__));

	return 0;
}


#ifdef DOT11_N_SUPPORT
INT AsicReadAggCnt(RTMP_ADAPTER *pAd, ULONG *aggCnt, int cnt_len)
{
	NdisZeroMemory(aggCnt, cnt_len * sizeof(ULONG));

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return FALSE;
	}

	return TRUE;
}

#endif /* DOT11_N_SUPPORT */


INT AsicSetTxStream(RTMP_ADAPTER *pAd, UINT32 StreamNums)
{
	UINT32 Value;

	RTMP_IO_READ32(pAd, TMAC_TCR, &Value);

	Value &= ~TMAC_TCR_TX_STREAM_NUM_MASK;
	Value |= TMAC_TCR_TX_STREAM_NUM(StreamNums - 1);

	RTMP_IO_WRITE32(pAd, TMAC_TCR, Value);

	return TRUE;
}


INT AsicSetRxStream(RTMP_ADAPTER *pAd, UINT32 StreamNums)
{
	UINT32 Value, Mask = 0;
	INT Ret = TRUE;

	RTMP_IO_READ32(pAd, RMAC_RMCR, &Value);

	Value &= ~(RMAC_RMCR_RX_STREAM_0 |
				RMAC_RMCR_RX_STREAM_1 |
				RMAC_RMCR_RX_STREAM_2);

	switch (StreamNums) {
		case 3:
			Mask |= RMAC_RMCR_RX_STREAM_2;
		case 2:
			Mask |= RMAC_RMCR_RX_STREAM_1;
		case 1:
			Mask |= RMAC_RMCR_RX_STREAM_0;
			break;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("illegal StreamNums(%d\n", StreamNums));
			Ret = FALSE;
			break;
	}

	if (Ret)
	{
		Value |= Mask;

		Value &= ~RMAC_RMCR_SMPS_MODE_MASK;
		Value |= RMAC_RMCR_SMPS_MODE(DISABLE_SMPS_RX_BYSETTING);

		RTMP_IO_WRITE32(pAd, RMAC_RMCR, Value);
	}

	return Ret;
}


INT AsicSetBW(RTMP_ADAPTER *pAd, INT bw)
{
	UINT32 val;

	RTMP_IO_READ32(pAd, AGG_BWCR, &val);
	val &= (~0x0c);
	switch (bw)
	{
		case BW_20:
			val |= (0);
			break;
		case BW_40:
			val |= (0x1 << 2);
			break;
		case BW_80:
			val |= (0x2 << 2);
			break;
	}
	RTMP_IO_WRITE32(pAd, AGG_BWCR, val);

	bbp_set_bw(pAd, (UINT8)bw);

	return TRUE;
}


INT AsicSetChannel(RTMP_ADAPTER *pAd, UCHAR ch, UINT8 bw, UINT8 ext_ch, BOOLEAN bScan)
{
	bbp_set_bw(pAd, bw);

	/*  Tx/RX : control channel setting */
	bbp_set_ctrlch(pAd, ext_ch);
	mt_mac_set_ctrlch(pAd, ext_ch);

	/* Let BBP register at 20MHz to do scan */
	AsicSwitchChannel(pAd, ch, bScan);
	AsicLockChannel(pAd, ch);

#ifdef RT28xx
	RT28xx_ch_tunning(pAd, bw);
#endif /* RT28xx */

	return 0;
}


INT AsicSetRxPath(RTMP_ADAPTER *pAd, UINT32 RxPathSel)
{
	UINT32 Value = 0, Mask = 0;
	INT Ret = TRUE;

	RTMP_IO_READ32(pAd, RMAC_RMCR, &Value);

	Value &= ~(RMAC_RMCR_RX_STREAM_0 |
				RMAC_RMCR_RX_STREAM_1 |
				RMAC_RMCR_RX_STREAM_2);

	switch (RxPathSel) {
		case 0: /* ALL */
			Mask = (RMAC_RMCR_RX_STREAM_0 | RMAC_RMCR_RX_STREAM_1 | RMAC_RMCR_RX_STREAM_2);
			if ((MTK_REV_GTE(pAd, MT7603, MT7603E1)) ||
				(MTK_REV_GTE(pAd, MT7628, MT7628E1)) ||
				(MTK_REV_GTE(pAd, MT7603, MT7603E2)))
			{
				RTMP_IO_WRITE32(pAd, CR_RXTD_39, 0x0004ba43);
            }
			break;
		case 1: /* RX0 */
			Mask = RMAC_RMCR_RX_STREAM_0;
			if ((MTK_REV_GTE(pAd, MT7603, MT7603E1)) ||
				(MTK_REV_GTE(pAd, MT7628, MT7628E1)) ||
				(MTK_REV_GTE(pAd, MT7603, MT7603E2)))
			{
				RTMP_IO_WRITE32(pAd, CR_RXTD_39, 0x0004ba43);
            }
			break;
		case 2: /* RX1 */
			Mask = RMAC_RMCR_RX_STREAM_1;
			if ((MTK_REV_GTE(pAd, MT7603, MT7603E1)) ||
				(MTK_REV_GTE(pAd, MT7628, MT7628E1)) ||
				(MTK_REV_GTE(pAd, MT7603, MT7603E2)))
			{
				RTMP_IO_WRITE32(pAd, CR_RXTD_39, 0x0005ba43);
			}
			break;
        case 3: /* RX2 */
            Mask = RMAC_RMCR_RX_STREAM_2;
            break;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("illegal RxPath(%d\n", RxPathSel));
			Ret = FALSE;
			break;
	}

	if (Ret)
	{
		Value |= Mask;

		Value &= ~RMAC_RMCR_SMPS_MODE_MASK;
		Value |= RMAC_RMCR_SMPS_MODE(DISABLE_SMPS_RX_BYSETTING);

		RTMP_IO_WRITE32(pAd, RMAC_RMCR, Value);
	}

	return Ret;

}

#ifdef CONFIG_ATE
INT AsicSetTxTonePower(RTMP_ADAPTER *pAd, INT dec0, INT dec1)
{
	INT Ret = TRUE;

	DBGPRINT(RT_DEBUG_OFF,("%s-v2 dec0 = %d, dec1 = %d\n",__FUNCTION__,dec0,dec1));
    if( dec0<0 || dec0>0xF || dec1>31 || dec1<-32) {
		DBGPRINT(RT_DEBUG_ERROR,("%s dec value invalid\n",__FUNCTION__));
	   	DBGPRINT(RT_DEBUG_ERROR,("%s dec0 0~F, dec1 -32~31\n",__FUNCTION__));
		if(dec0 > 0xf)
			dec0 = 0xf;
		if(dec1 > 31)
			dec1 = 31;
    }
 	if((MTK_REV_GTE(pAd, MT7603, MT7603E1))||
		(MTK_REV_GTE(pAd, MT7603, MT7603E2))){
		UCHAR PowerDec0 = dec0&0x0F;
        UINT32 SetValue = 0x04000000;
        ULONG Tempdec1 = 0;

		SetValue |= PowerDec0<<20;

        //RF Gain 1 db
        DBGPRINT(RT_DEBUG_TRACE,("%s RF 1db SetValue = 0x%x\n",__FUNCTION__,SetValue));

		CmdRFRegAccessWrite(pAd, 0, 0x70, SetValue); //bit 26(0x04000000) is enable
		CmdRFRegAccessWrite(pAd, 1, 0x70, SetValue);
        //DC Gain
        if(dec1<0){
			Tempdec1 = (0x40+dec1);
        }else{
            Tempdec1 = dec1;
        }
        SetValue = 0x40000000|(Tempdec1<<20);
        DBGPRINT(RT_DEBUG_TRACE,("%s DC 0.25db SetValue = 0x%x\n",__FUNCTION__,SetValue));
   		RTMP_IO_WRITE32(pAd, CR_TSSI_9, SetValue); //0x10D24, bit 30(0x40000000) isenable
   		RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_9, SetValue); //0x11D24, bit 30(0x40000000) isenable
     }else{
		DBGPRINT(RT_DEBUG_OFF,("%s, for MT7636\n ",__FUNCTION__));
	 	/* For 7636 FW command */
	 }

    return Ret;
}

INT AsicSetRfFreqOffset(RTMP_ADAPTER *pAd, UINT32 FreqOffset)
{
	UINT32 Value = 0;
	INT Ret = TRUE;

    if (FreqOffset > 127)
        FreqOffset = 127;

    RTMP_IO_READ32(pAd, XTAL_CTL13, &Value);
    Value &= ~DA_XO_C2_MASK; /* [14:8] (DA_XO_C2) */
    Value |= DA_XO_C2(0x3C); //set 60(DEC)
    RTMP_IO_WRITE32(pAd,XTAL_CTL13,Value);

    RTMP_IO_READ32(pAd, XTAL_CTL14, &Value);
    Value &= ~DA_XO_C2_MASK;
    Value |= DA_XO_C2(0x7F);
    RTMP_IO_WRITE32(pAd,XTAL_CTL14,Value);

    RTMP_IO_READ32(pAd, XTAL_CTL13, &Value);
    Value &= ~DA_XO_C2_MASK; /* [14:8] (DA_XO_C2) */
    Value |= DA_XO_C2(FreqOffset); //set 60(DEC)
    RTMP_IO_WRITE32(pAd,XTAL_CTL13,Value);


    return Ret;
}

INT AsicSetTSSI(RTMP_ADAPTER *pAd, UINT32 bOnOff, UCHAR WFSelect)
{
	UINT32 CRValue = 0x0;
	UINT32 WF0Offset = 0x10D04; /* WF_PHY_CR_FRONT CR_WF0_TSSI_1 */
	UINT32 WF1Offset = 0x11D04; /* WF_PHY_CR_FRONT CR_WF1_TSSI_1 */
    INT Ret = TRUE;
    /* !!TEST MODE ONLY!! Normal Mode control by FW and Never disable */
    /* WF0 = 0, WF1 = 1, WF ALL = 2 */

	if (FALSE == bOnOff)
		CRValue = 0xE3F3F800;
	else
		CRValue = 0xE1010800;

	if ((0 == WFSelect) || (2 == WFSelect)) {
		DBGPRINT(RT_DEBUG_TRACE,("%s, Set WF#%d TSSI off\n",__FUNCTION__, WFSelect));
		RTMP_IO_WRITE32(pAd,WF0Offset,CRValue);//3
	}

	if ((1 == WFSelect) || (2 == WFSelect)) {
		DBGPRINT(RT_DEBUG_TRACE,("%s, Set WF#%d TSSI on\n",__FUNCTION__, WFSelect));
		RTMP_IO_WRITE32(pAd,WF1Offset,CRValue);//3
	}

    return Ret;
}

INT AsicSetDPD(RTMP_ADAPTER *pAd, UINT32 bOnOff, UCHAR WFSelect)
{
	UINT32 CRValue = 0x0;
	ULONG WF0Offset = 0x10A08;
	ULONG WF1Offset = 0x11A08;
    INT Ret = TRUE;
    /* !!TEST MODE ONLY!! Normal Mode control by FW and Never disable */
    /* WF0 = 0, WF1 = 1, WF ALL = 2 */

	if (FALSE == bOnOff) {
		//WF0
		if((0 == WFSelect) || (2 == WFSelect)) {
			RTMP_IO_READ32(pAd, WF0Offset, &CRValue);
			CRValue |= 0xF0000000;
			RTMP_IO_WRITE32(pAd,WF0Offset,CRValue);//3
		}
		//WF1
		if((1 == WFSelect) || (2 == WFSelect)) {
			RTMP_IO_READ32(pAd, WF1Offset, &CRValue);
			CRValue |= 0xF0000000;
			RTMP_IO_WRITE32(pAd,WF1Offset,CRValue);//3
		}
		DBGPRINT(RT_DEBUG_TRACE,("%s, Set WFSelect: %d DPD off\n",__FUNCTION__, WFSelect));
	} else {
		if ((0 == WFSelect) || (2 == WFSelect)) {
			RTMP_IO_READ32(pAd, WF0Offset, &CRValue);
			CRValue &= (~0xF0000000);
			RTMP_IO_WRITE32(pAd,WF0Offset,CRValue);//3
		}
		if ((1 == WFSelect) || (2 == WFSelect)) {
			RTMP_IO_READ32(pAd, WF1Offset, &CRValue);
			CRValue &= (~0xF0000000);
			RTMP_IO_WRITE32(pAd,WF1Offset,CRValue);//3
		}
		DBGPRINT(RT_DEBUG_TRACE,("%s, Set WFSelect: %d DPD on\n",__FUNCTION__, WFSelect));
	}

    return Ret;
}
#ifdef CONFIG_QA

UINT32 AsicGetRxStat(RTMP_ADAPTER *pAd, UINT type)
{
    UINT32 value = 0;
    DBGPRINT(RT_DEBUG_TRACE, ("%s, Type:%d\n", __FUNCTION__, type));
    switch (type) {
        case HQA_RX_STAT_MACFCSERRCNT:
            RTMP_IO_READ32(pAd,MIB_MSDR4,&value);
            value = (value >> 16) & 0xFFFF; /* [31:16] FCS ERR */
            break;
        case HQA_RX_STAT_MAC_MDRDYCNT:
            RTMP_IO_READ32(pAd,MIB_MSDR10,&value);
            break;
        case HQA_RX_STAT_PHY_MDRDYCNT:
            /* [31:16] OFDM [15:0] CCK */
            RTMP_IO_READ32(pAd,RO_PHYCTRL_STS5,&value);
            break;
        case HQA_RX_STAT_PHY_FCSERRCNT:
            /* [31:16] OFDM [15:0] CCK */
            RTMP_IO_READ32(pAd,RO_PHYCTRL_STS4,&value);
            break;
        case HQA_RX_STAT_PD:
            /* [31:16] OFDM [15:0] CCK */
            RTMP_IO_READ32(pAd,RO_PHYCTRL_STS0,&value);
            break;
        case HQA_RX_STAT_CCK_SIG_SFD:
            /* [31:16] SIG [15:0] SFD */
            RTMP_IO_READ32(pAd,RO_PHYCTRL_STS1,&value);
            break;
        case HQA_RX_STAT_OFDM_SIG_TAG:
            /* [31:16] SIG [15:0] TAG */
            RTMP_IO_READ32(pAd,RO_PHYCTRL_STS2,&value);
            break;
        case HQA_RX_STAT_RSSI:
            /*[31:24]IBRSSI0 [23:16]WBRSSI0 [15:8]IBRSSI1 [7:0]WBRSSI1*/
            RTMP_IO_READ32(pAd,RO_AGC_DEBUG_2,&value);
            break;
        case HQA_RX_RESET_PHY_COUNT:
            RTMP_IO_READ32(pAd,CR_PHYCTRL_2,&value);
            value |= (1<<6); /* BIT6: CR_STSCNT_RST */
            RTMP_IO_WRITE32(pAd,CR_PHYCTRL_2,value);
            value &= (~(1<<6));
            RTMP_IO_WRITE32(pAd,CR_PHYCTRL_2,value);
            value |= (1<<7); /* BIT7: CR_STSCNT_EN */
            RTMP_IO_WRITE32(pAd,CR_PHYCTRL_2,value);
            break;
        default:
            break;
    }
    DBGPRINT(RT_DEBUG_TRACE, ("%s, Type(%d):%x\n", __FUNCTION__, type, value));
    return value;
}
#endif /* CONFIG_QA */

INT AsicSetTxToneTest(RTMP_ADAPTER *pAd, UINT32 bOnOff, UCHAR Type)
{
    DBGPRINT(RT_DEBUG_TRACE, ("%s, bOnOff:%d Type:%d\n", __FUNCTION__, bOnOff, Type));

    if (bOnOff == 0) { /* 0 = off 1 = on */
        //WF0
        RTMP_IO_WRITE32(pAd, CR_PHYCK_CTRL, 0x00000000); //0x10000
        RTMP_IO_WRITE32(pAd, CR_FR_CKG_CTRL, 0x00000000); //0x10004
        RTMP_IO_WRITE32(pAd, CR_TSSI_0, 0x80274027); //0x10D00
        RTMP_IO_WRITE32(pAd, CR_TSSI_1, 0xC0000800); //0x10D04
        RTMP_IO_WRITE32(pAd, CR_PHYMUX_3, 0x00000008); //0x1420C
        RTMP_IO_WRITE32(pAd, CR_PHYMUX_5, 0x00000580); //0x14214
        RTMP_IO_WRITE32(pAd, CR_TXFD_1, 0x00000000); //0x14704
        RTMP_IO_WRITE32(pAd, CR_TSSI_9, 0x00000000); //0x10D24
        RTMP_IO_WRITE32(pAd, CR_TXFE_3, 0x00000000); //0x10A08
        RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x00000000); //0x101A0
        RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x00000000); //0x101A4
        RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00000000); //0x1020C
        RTMP_IO_WRITE32(pAd, CR_TXFE_4, 0x00000000); //0x10A0C
        RTMP_IO_WRITE32(pAd, CR_DPD_CAL_03, 0x20300604); //0x1090C
        //WF1
        RTMP_IO_WRITE32(pAd, CR_PHYCK_CTRL, 0x00000000); //0x10000
        RTMP_IO_WRITE32(pAd, CR_FR_CKG_CTRL, 0x00000000); //0x10004
        RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_0, 0x80274027); //0x11D00
        RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_1, 0xC0000800); //0x11D04
        RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_3, 0x00000008); //0x1520C
        RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_5, 0x00000580); //0x15214
        RTMP_IO_WRITE32(pAd, CR_TXFD_1, 0x00000000); //0x14704
        RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_9, 0x00000000); //0x11D24
        RTMP_IO_WRITE32(pAd, CR_TXFE1_3, 0x00000000); //0x11A08
        RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x00000000); //0x101A0
        RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x00000000); //0x101A4
        RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00000000); //0x1020C
        RTMP_IO_WRITE32(pAd, CR_TXFE1_4, 0x00000000); //0x11A0C
        RTMP_IO_WRITE32(pAd, CR_DPD_CAL_03, 0x20300604); //0x1090C
    } else if (bOnOff == 1) {
        /* WF0 */
        if (Type == WF0_TX_ONE_TONE_5M || Type == WF0_TX_TWO_TONE_5M ||
            Type == WF0_TX_ONE_TONE_10M || Type == WF0_TX_ONE_TONE_DC) {
            /* 1. clock setup */
            RTMP_IO_WRITE32(pAd, CR_PHYCK_CTRL, 0x00000021);
            RTMP_IO_WRITE32(pAd, CR_FR_CKG_CTRL, 0x00000021);

            /* 2. TX setup */
            RTMP_IO_WRITE32(pAd, CR_TSSI_0, 0x00274027);
            RTMP_IO_WRITE32(pAd, CR_TSSI_1, 0xC0000400);
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_3, 0x80000008);
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_5, 0x00000597);
            RTMP_IO_WRITE32(pAd, CR_TXFD_1, 0x10000000);
            RTMP_IO_WRITE32(pAd, CR_TSSI_9, 0x60000000);
            RTMP_IO_WRITE32(pAd, CR_TXFE_3, 0xF0000000);

            /* 3. Gen Tone */
            if (Type == WF0_TX_ONE_TONE_5M) {
                RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x000C100C);
                RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x00000000);
                RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00010122);
                RTMP_IO_WRITE32(pAd, CR_TXFE_4, 0x000000C0);
            } else if (Type == WF0_TX_TWO_TONE_5M) {
                RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x000C104C);
                RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x00000000);
                RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00010122);
                RTMP_IO_WRITE32(pAd, CR_TXFE_4, 0x000000C0);
            } else if (Type == WF0_TX_ONE_TONE_10M) {
                RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x000C101C);
                RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x00000000);
                RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00011122);
                RTMP_IO_WRITE32(pAd, CR_TXFE_4, 0x000000C0);
            } else if (Type == WF0_TX_ONE_TONE_DC) {
                RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x000C1048);
                RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x07000700);
                RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00010122);
                RTMP_IO_WRITE32(pAd, CR_TXFE_4, 0x000000C0);
            } else {
                DBGPRINT(RT_DEBUG_WARN,("%s Type = %d error!!!\n",__FUNCTION__, Type));
            }

        } else if (Type == WF1_TX_ONE_TONE_5M || Type == WF1_TX_TWO_TONE_5M ||
                   Type == WF1_TX_ONE_TONE_10M || Type == WF1_TX_ONE_TONE_DC) {
            /* 1. clock setup */
            RTMP_IO_WRITE32(pAd, CR_PHYCK_CTRL, 0x00000021);
            RTMP_IO_WRITE32(pAd, CR_FR_CKG_CTRL, 0x00000021);

            /* 2. TX setup */
            RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_0, 0x00274027);
            RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_1, 0xC0000400);
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_3, 0x80000008);
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_5, 0x00000597);
            RTMP_IO_WRITE32(pAd, CR_TXFD_1, 0x10000000);
            RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_9, 0x60000000);
            RTMP_IO_WRITE32(pAd, CR_TXFE1_3, 0xF0000000);

            /* 3. Gen Tone */
            if (Type == WF1_TX_ONE_TONE_5M) {
                RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x000C100C);
                RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x00000000);
                RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00010122);
                RTMP_IO_WRITE32(pAd, CR_TXFE1_4, 0x000000C0);
            } else if (Type == WF1_TX_TWO_TONE_5M) {
                RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x000C104C);
                RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x00000000);
                RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00010122);
                RTMP_IO_WRITE32(pAd, CR_TXFE1_4, 0x000000C0);
            } else if (Type == WF1_TX_ONE_TONE_10M) {
                RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x000C101C);
                RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x00000000);
                RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00011122);
                RTMP_IO_WRITE32(pAd, CR_TXFE1_4, 0x000000C0);
            } else if (Type == WF1_TX_ONE_TONE_DC) {
                RTMP_IO_WRITE32(pAd, CR_TXPTN_00, 0x000C1048);
                RTMP_IO_WRITE32(pAd, CR_TXPTN_01, 0x07000700);
                RTMP_IO_WRITE32(pAd, CR_RFINTF_03, 0x00010122);
                RTMP_IO_WRITE32(pAd, CR_TXFE1_4, 0x000000C0);
            } else {
                DBGPRINT(RT_DEBUG_WARN,("%s Type = %d error!!!\n",__FUNCTION__, Type));
            }

        } else {
            DBGPRINT(RT_DEBUG_WARN,("%s Type = %d error!!!\n",__FUNCTION__, Type));
        }
    } else {
        DBGPRINT(RT_DEBUG_WARN,("%s bOnOff = %d error!!!\n",__FUNCTION__, bOnOff));
    }

	return 0;
}

INT AsicStartContinousTx(RTMP_ADAPTER *pAd, UINT32 PhyMode, UINT32 BW, UINT32 PriCh, UINT32 Mcs, UINT32 WFSel)
{
	ATE_CTRL *ATECtrl = &(pAd->ATECtrl);
	ATE_OPERATION *ATEOp = ATECtrl->ATEOp;

	UINT32 value = 0;
	/* Band Edge */
	UINT32 cr_hilo = 0;
	UINT32 bit = 0;

	UINT32 i = 0;
	UINT32 reg = 0;
	UINT32 reg1 = 0;
	UINT32 reg2 = 0;
	/* TSSI Patch */
	ATECtrl->Sgi = 0;
	ATECtrl->tssi0_wf0_cr = 0;
	ATECtrl->tssi0_wf1_cr = 0;
	ATECtrl->tssi1_wf0_cr = 0;
	ATECtrl->tssi1_wf1_cr = 0;
	ATECtrl->phy_mux_27 = 0;
	/* Change TSSI Training Time */
	RTMP_IO_READ32(pAd, CR_TSSI_0, &(ATECtrl->tssi0_wf0_cr));	//0x0D00
	RTMP_IO_READ32(pAd, CR_WF1_TSSI_0, &(ATECtrl->tssi0_wf1_cr));	//0x1D00
	RTMP_IO_READ32(pAd, CR_TSSI_1, &(ATECtrl->tssi1_wf0_cr));	//0x0D04
	RTMP_IO_READ32(pAd, CR_WF1_TSSI_1, &(ATECtrl->tssi1_wf1_cr));	//0x1D04

	RTMP_IO_READ32(pAd, CR_TSSI_6, &reg); /* 0x0D18 */
	RTMP_IO_READ32(pAd, CR_TSSI_13, &reg1);	/* 0x1D18 */

	RTMP_IO_WRITE32(pAd, CR_FR_RST_CTRL, 0xFFFFFFFF); /*0x10008*/
	RTMP_IO_WRITE32(pAd, CR_BK_RST_CTRL, 0xFFFFFFFF); /*0x14004*/
	DBGPRINT(RT_DEBUG_TRACE,("%s(): 0x0D18: 0x%x, tx_pwr(0x0D34):0x%x\n",	__FUNCTION__, reg, reg1));
	for(i=0; i<2; i++){
		mdelay(200);
		ATECtrl->TxCount = 8;
		ATECtrl->bQATxStart = TRUE;
		ATEOp->StartTx(pAd);
		DBGPRINT(RT_DEBUG_TRACE,("%s(): finish Start Tx\n",__FUNCTION__));
		mdelay(100);
		ATECtrl->bQATxStart = FALSE;
		ATEOp->StopTx(pAd, ATECtrl->Mode);
		DBGPRINT(RT_DEBUG_TRACE,("%s(): finish StOP Tx\n",__FUNCTION__));
	}
	DBGPRINT(RT_DEBUG_TRACE,("%s(): finish Tx TSSI Train \n",	__FUNCTION__));

	/* Original Flow */
	DBGPRINT(RT_DEBUG_TRACE,("%s Modulation = %d, BW=%d, pri_ch = %d, rate = %d, WFSelect = %d-->\n",__FUNCTION__, PhyMode, BW, PriCh, Mcs, WFSel));
	RTMP_IO_READ32(pAd, CR_TSSI_6, &reg);
	reg1 = reg;
	RTMP_IO_READ32(pAd, CR_WF1_TSSI_6, &reg2);
	DBGPRINT(RT_DEBUG_TRACE,("%s, 0x0D18: %08x, 0x1D18: %08x\n", __FUNCTION__, reg1, reg2));

	RTMP_IO_WRITE32(pAd, CR_PHYCK_CTRL,  0x00000078); 	//0x10000
    RTMP_IO_WRITE32(pAd, CR_FR_CKG_CTRL, 0x00000078);	//0x10004
	RTMP_IO_READ32(pAd, CR_TSSI_0, &(ATECtrl->tssi0_wf0_cr));	//0x0D00
	RTMP_IO_READ32(pAd, CR_WF1_TSSI_0, &(ATECtrl->tssi0_wf1_cr));	//0x1D00
	RTMP_IO_READ32(pAd, CR_TSSI_1, &(ATECtrl->tssi1_wf0_cr));	//0x0D04
	RTMP_IO_READ32(pAd, CR_WF1_TSSI_1, &(ATECtrl->tssi1_wf1_cr));	//0x1D04
	/* Contiuous Tx power patch */
	RTMP_IO_WRITE32(pAd, CR_TSSI_0,		0x024041C0);	//0x0D00
	RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_0,	0x024041C0);	//0x1D00
	RTMP_IO_WRITE32(pAd, CR_TSSI_1,		0x23F3F800);	//0x0D04
	RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_1, 0x23F3F800);	//0x1D04
 	RTMP_IO_WRITE32(pAd, CR_TSSI_6, reg1);	//0x0D18
	RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_6, reg2);	//0x1D18

	RTMP_IO_WRITE32(pAd, CR_FFT_MANU_CTRL, 0x0AA00000); //0x10704

    if (BW_20 == BW)
        value = 0x00000000;
    else if (BW_40 == BW)
        value = 0x01000000;
    else if (BW_80 == BW)
        value = 0x02000000;
    else
        DBGPRINT(RT_DEBUG_WARN,("%s BW = %d error!!!\n",__FUNCTION__, BW));

    if (0 == PriCh)
        value |= 0x00000000;
    else if (1 == PriCh)
        value |= 0x00100000;
    else
        DBGPRINT(RT_DEBUG_WARN,("%s pri_ch = %d error!!!\n",__FUNCTION__, PriCh));

    RTMP_IO_WRITE32(pAd, CR_PHYCTRL_0, value); //0x14100
    RTMP_IO_WRITE32(pAd, CR_PHYCTRL_DBGCTRL, 0x80000030); //0x14140
    RTMP_IO_WRITE32(pAd, CR_PHYMUX_3, 0x80680008); //0x1420C

    switch (WFSel) {
        case 0:
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_5, 0x00000597); //0x14214
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_5, 0x00000010); //0x15214
            break;
        case 1:
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_3, 0x80680008); //0x1520C
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_5, 0x00000590); //0x14214
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_5, 0x00000017); //0x15214
            break;
		case 2:
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_3, 0x80680008); //0x1520C
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_5, 0x00000597); //0x14214
            RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_5, 0x00000017); //0x15214
            break;
        default:
            DBGPRINT(RT_DEBUG_WARN,("%s WFSelect = %d error!!!\n",__FUNCTION__, WFSel)); /* No ALL?*/
            break;
    }

    if (BW_20 == BW)
        RTMP_IO_WRITE32(pAd, CR_TXFD_0, 0x00030000); //0x14700
    else if (BW_40 == BW)
        RTMP_IO_WRITE32(pAd, CR_TXFD_0, 0x14030000); //0x14700
    else
        DBGPRINT(RT_DEBUG_WARN,("%s BW = %d error!!!\n",__FUNCTION__, BW));

    if (0 == PhyMode) { /* MODULATION_SYSTEM_CCK */
        value = 0;
        switch (Mcs) {
            case MCS_0:
			case MCS_8:
				value = 0x00000000;
				break;
			case MCS_1:
			case MCS_9:
				value = 0x00200000;
				break;
			case MCS_2:
			case MCS_10:
				value = 0x00400000;
				break;
			case MCS_3:
			case MCS_11:
				value = 0x00600000;
				break;
			default:
                DBGPRINT(RT_DEBUG_WARN,("%s CCK rate = %d error!!!\n",__FUNCTION__, Mcs));
                break;
        }
        RTMP_IO_WRITE32(pAd, CR_TXFD_3, value);
    }
    else if (1 == PhyMode) { /* MODULATION_SYSTEM_OFDM */
		RTMP_IO_READ32(pAd, CR_PHYMUX_26, &cr_hilo); /* 0x4268 */
        value = 0;
		switch (Mcs) {
			case MCS_0:
				value = 0x01600000;
				cr_hilo &= BIT(0);
				break;
			case MCS_1:
				value = 0x01E00000;
				cr_hilo &= BIT(1);
				break;
			case MCS_2:
				value = 0x01400000;
				cr_hilo &= BIT(2);
				break;
			case MCS_3:
				value = 0x01C00000;
				cr_hilo &= BIT(3);
				break;
			case MCS_4:
				value = 0x01200000;
				cr_hilo &= BIT(4);
				break;
			case MCS_5:
				value = 0x01900000;
				cr_hilo &= BIT(5);
				break;
			case MCS_6:
				value = 0x01000000;
				cr_hilo &= BIT(6);
				break;
			case MCS_7:
				value = 0x01800000;
				cr_hilo &= BIT(7);
				break;
			default:
				DBGPRINT(RT_DEBUG_WARN,("%s OFDM rate = %d error!!!\n",__FUNCTION__, Mcs));
				break;
		}
        RTMP_IO_WRITE32(pAd, CR_TXFD_3, value); //0x1470C
		RTMP_IO_READ32(pAd, CR_PHYMUX_27, &value); //0x1426C
		ATECtrl->phy_mux_27 = value;
		if(cr_hilo == 0){ //Low Rate
			bit = value & BIT(12);
			value |= bit << 2;
		}else{ //High Rate
			bit = value & BIT(13);
			value |= bit << 1;
	}
		RTMP_IO_WRITE32(pAd, CR_PHYMUX_27, value); //0x1426C
    }
    else if (2 == PhyMode || 3 == PhyMode) {
		/* MODULATION_SYSTEM_HT20 || MODULATION_SYSTEM_HT40 */
		RTMP_IO_READ32(pAd, CR_PHYMUX_25, &cr_hilo); /* 0x4264 */
        value = 0;
        switch(Mcs) {
			case MCS_0:
				value = 0x00000000;
				cr_hilo &= BIT(0);
				break;
			case MCS_1:
				value = 0x00200000;
				cr_hilo &= BIT(1);
				break;
			case MCS_2:
				value = 0x00400000;
				cr_hilo &= BIT(2);
				break;
			case MCS_3:
				value = 0x00600000;
				cr_hilo &= BIT(3);
				break;
			case MCS_4:
				value = 0x00800000;
				cr_hilo &= BIT(4);
				break;
			case MCS_5:
				value = 0x00A00000;
				cr_hilo &= BIT(5);
				break;
			case MCS_6:
				value = 0x00C00000;
				cr_hilo &= BIT(6);
				break;
			case MCS_7:
				value = 0x00E00000;
				cr_hilo &= BIT(7);
				break;
			case MCS_32:
				value = 0x04000000;
				cr_hilo &= BIT(0);
				break;
			default:
				DBGPRINT(RT_DEBUG_WARN,("%s OFDM HT MCS = %d error!!!\n",__FUNCTION__, Mcs));
				break;
		}
        RTMP_IO_WRITE32(pAd, CR_TXFD_3, value); //0x1470C
		RTMP_IO_READ32(pAd, CR_PHYMUX_27, &value); //0x1426C
		ATECtrl->phy_mux_27 = value;
		if(cr_hilo == 0){ //Low Rate
			bit = value & BIT(12);
			value |= bit << 2;
		}else{ //High Rate
			bit = value & BIT(13);
			value |= bit << 1;
		}
		RTMP_IO_WRITE32(pAd, CR_PHYMUX_27, value); //0x1426C

    }
    else {
        DBGPRINT(RT_DEBUG_WARN,("%s Modulation = %d error!!!\n",__FUNCTION__, PhyMode));
    }

    if (0 == WFSel)
        RTMP_IO_WRITE32(pAd, CR_PHYMUX_11, 0xA0000000); //0x1422C
    else if (1 == WFSel)
        RTMP_IO_WRITE32(pAd, CR_PHYMUX_11, 0x90000000); //0x1422C
    else
        DBGPRINT(RT_DEBUG_WARN,("%s WFSelect = %d error!!!\n",__FUNCTION__, WFSel)); /* No ALL?*/

    if (0 == PhyMode) /* PREAMBLE_CCK */
        RTMP_IO_WRITE32(pAd, CR_TXFD_1, 0x300000F8); //0x14704
    else if (1 == PhyMode) /* PREAMBLE_OFDM */
        RTMP_IO_WRITE32(pAd, CR_TXFD_1, 0x310000F2); //0x14704
    else if (2 == PhyMode || 3 == PhyMode) /* PREAMBLE_GREEN_FIELD */
        RTMP_IO_WRITE32(pAd, CR_TXFD_1, 0x320000F2); //0x14704
    else
        DBGPRINT(RT_DEBUG_WARN,("%s Modulation = %d error!!!\n",__FUNCTION__, PhyMode));

    RTMP_IO_WRITE32(pAd, CR_TXFE_4, 0x000000C0); //0x10A0C
    RTMP_IO_WRITE32(pAd, CR_TXFE1_4, 0x000000C0); //0x11A0C

    return 0;
}

INT AsicStopContinousTx(RTMP_ADAPTER *pAd)
{
	ATE_CTRL *ATECtrl = &(pAd->ATECtrl);
    DBGPRINT(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));

    RTMP_IO_WRITE32(pAd, CR_TXFD_1, 0x00000005); //0x14704
    RTMP_IO_WRITE32(pAd, CR_TXFD_1, 0x00000000); //0x14704
    RTMP_IO_WRITE32(pAd, CR_PHYCK_CTRL, 0x00000045); //0x10000
    RTMP_IO_WRITE32(pAd, CR_FR_CKG_CTRL, 0x00000045); //0x10004
    RTMP_IO_WRITE32(pAd, CR_FFT_MANU_CTRL, 0x00000000); //0x10704
    RTMP_IO_WRITE32(pAd, CR_PHYCTRL_0, 0x00000000); //0x14100
    RTMP_IO_WRITE32(pAd, CR_PHYCTRL_DBGCTRL, 0x00000000); //0x14140
    RTMP_IO_WRITE32(pAd, CR_PHYMUX_3, 0x7C900408); //0x1420C
    RTMP_IO_WRITE32(pAd, CR_PHYMUX_5, 0x00000580); //0x14214
    RTMP_IO_WRITE32(pAd, CR_PHYMUX_WF1_5, 0x00000000); //0x15214
    RTMP_IO_WRITE32(pAd, CR_PHYMUX_10, 0x00000000); //0x14228
    RTMP_IO_WRITE32(pAd, CR_PHYMUX_11, 0x00000000); //0x1422C
    RTMP_IO_WRITE32(pAd, CR_TXFE_4, 0x00000000); //0x10A0C
    RTMP_IO_WRITE32(pAd, CR_TXFE1_4, 0x00000000); //0x11A0C
    RTMP_IO_WRITE32(pAd, CR_TXFD_0, 0x00000000); //0x14700
    RTMP_IO_WRITE32(pAd, CR_TXFD_3, 0x00000000); //0x1470C
    RTMP_IO_WRITE32(pAd, TMAC_PCTSR, 0x00000000); //0x21708
    RTMP_IO_WRITE32(pAd, CR_FR_RST_CTRL, 0xFFFFFFFF); //0x10008
    RTMP_IO_WRITE32(pAd, CR_BK_RST_CTRL, 0xFFFFFFFF); //0x14004
	/* Restore TSSI CR */
	RTMP_IO_WRITE32(pAd, CR_TSSI_0, ATECtrl->tssi0_wf0_cr);	//0x0D00
	RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_0, ATECtrl->tssi0_wf1_cr);	//0x1D00
	RTMP_IO_WRITE32(pAd, CR_TSSI_1, ATECtrl->tssi1_wf0_cr);	//0x0D04
	RTMP_IO_WRITE32(pAd, CR_WF1_TSSI_1, ATECtrl->tssi1_wf1_cr);	//0x1D04
	/* Restore Band Edge Patch CR */
	RTMP_IO_WRITE32(pAd, CR_RFINTF_01, 0x00000000); /* 0x0204 */
	RTMP_IO_WRITE32(pAd, CR_WF1_RFINTF_01, 0x00000000); /* 0x1204 */
	RTMP_IO_WRITE32(pAd, CR_PHYMUX_27, ATECtrl->phy_mux_27); //0x1426C

	return 0;
}

#endif /* CONFIG_ATE */
#ifdef MAC_APCLI_SUPPORT
/*
	==========================================================================
	Description:
		Set BSSID of Root AP

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicSetApCliBssid(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pBssid,
	IN UCHAR index)
{
	UINT32 val;

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): Set BSSID=%02x:%02x:%02x:%02x:%02x:%02x\n",
				__FUNCTION__, PRINT_MAC(pBssid)));

	val = (UINT32)((pBssid[0]) |
				  (UINT32)(pBssid[1] << 8) |
				  (UINT32)(pBssid[2] << 16) |
				  (UINT32)(pBssid[3] << 24));
	RTMP_IO_WRITE32(pAd, RMAC_CB1R0, val);

	val = (UINT32)(pBssid[4]) | (UINT32)(pBssid[5] << 8) | (1 <<16);
	RTMP_IO_WRITE32(pAd, RMAC_CB1R1, val);

	return;
}
#endif /* MAC_APCLI_SUPPORT */

VOID MTPciMlmeRadioOn(PRTMP_ADAPTER pAd)
{
#ifdef CONFIG_AP_SUPPORT
	INT32 IdBss, MaxNumBss = pAd->ApCfg.BssidNum;
#endif

	MCU_CTRL_INIT(pAd);
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF);
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);

#if 0
	RTMPRingCleanUp(pAd, QID_AC_BK);
   	RTMPRingCleanUp(pAd, QID_AC_BE);
   	RTMPRingCleanUp(pAd, QID_AC_VI);
   	RTMPRingCleanUp(pAd, QID_AC_VO);
   	RTMPRingCleanUp(pAd, QID_HCCA);
   	RTMPRingCleanUp(pAd, QID_MGMT);
   	RTMPRingCleanUp(pAd, QID_RX);

#ifdef CONFIG_ANDES_SUPPORT
	RTMPRingCleanUp(pAd, QID_CTRL);
#endif
#endif

#ifdef RTMP_MAC_PCI
	/*  Enable Interrupt */
	RTMP_ASIC_INTERRUPT_ENABLE(pAd);
#endif /* RTMP_MAC_PCI */

	/*  Enable PDMA */
	AsicSetWPDMA(pAd, PDMA_TX_RX, 1);

	/*  Send radio on command and wait for ack */
	if (pAd->iwpriv_command == TRUE)
	{
		UINT32 value;
		RTMP_IO_READ32(pAd, AGG_TEMP, &value);
		value &= 0x0000ffff;
		RTMP_IO_WRITE32(pAd, AGG_TEMP, value);
	} else {
#ifdef LOAD_FW_ONE_TIME
	{
		UINT32 value;
		RTMP_IO_READ32(pAd, AGG_TEMP, &value);
		value &= 0x0000ffff;
		RTMP_IO_WRITE32(pAd, AGG_TEMP, value);
	}
#else /* LOAD_FW_ONE_TIME */
		CmdRadioOnOffCtrl(pAd, WIFI_RADIO_ON);
#endif /* !LOAD_FW_ONE_TIME */
	}

	/* Send Led on command */

	/* Enable RX */
	AsicSetMacTxRx(pAd, ASIC_MAC_RX, TRUE);

	/*  Enable normal operation */
	RTMP_OS_NETDEV_START_QUEUE(pAd->net_dev);

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (MaxNumBss > MAX_MBSSID_NUM(pAd))
			MaxNumBss = MAX_MBSSID_NUM(pAd);

		/*  first IdBss must not be 0 (BSS0), must be 1 (BSS1) */
		for (IdBss = FIRST_MBSSID; IdBss < MAX_MBSSID_NUM(pAd); IdBss++)
		{
			if (pAd->ApCfg.MBSSID[IdBss].wdev.if_dev)
				RTMP_OS_NETDEV_START_QUEUE(pAd->ApCfg.MBSSID[IdBss].wdev.if_dev);
		}
	}
#endif

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
}


#ifdef RTMP_PCI_SUPPORT
VOID MTPciPollTxRxEmpty(RTMP_ADAPTER *pAd)
{
	UINT32 Loop, Value/*, RxPending = 0*/;
	UINT32 IdleTimes = 0;
	//PNDIS_PACKET pRxPacket;
	//RX_BLK RxBlk, *pRxBlk;
	//BOOLEAN bReschedule = FALSE;

	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));

	RtmpOsMsDelay(100);

	/* Fix Rx Ring FULL lead DMA Busy, when DUT is in reset stage */
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE);

	/* Poll Tx until empty */
	for (Loop = 0; Loop < 20000; Loop++)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return;

		RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG, &Value);

		if ((Value & TX_DMA_BUSY) == 0x00)
		{
			IdleTimes++;
			RtmpusecDelay(50);
		}

		if (IdleTimes > 5000)
			break;
	}

	if (Loop >= 20000)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s: TX DMA Busy!! WPDMA_GLO_CFG_STRUC = %d\n",
										__FUNCTION__, Value));
	}

	IdleTimes = 0;

	/*  Poll Rx to empty */
	for (Loop = 0; Loop < 20000; Loop++)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return;

#if 0
/* Fix Rx Ring FULL lead DMA Busy, when DUT is in reset stage */
		while (1)
		{
			pRxBlk = &RxBlk;
			pRxPacket = GetPacketFromRxRing(pAd, pRxBlk, &bReschedule, &RxPending, 0);

			if ((RxPending == 0) && (bReschedule == FALSE))
				break;
			else
				RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_SUCCESS);
		}
#endif
		RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG, &Value);

		if ((Value & RX_DMA_BUSY) == 0x00)
		{
			IdleTimes++;
			RtmpusecDelay(50);
		}

		if (IdleTimes > 5000)
			break;
	}

	if (Loop >= 20000)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s: RX DMA Busy!! WPDMA_GLO_CFG_STRUC = %d\n",
										__FUNCTION__, Value));
	}

	/* Fix Rx Ring FULL lead DMA Busy, when DUT is in reset stage */
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE);
}


VOID MTPciMlmeRadioOff(PRTMP_ADAPTER pAd)
{
#ifdef CONFIG_AP_SUPPORT
	INT32 IdBss, MaxNumBss = pAd->ApCfg.BssidNum;
#endif /* CONFIG_AP_SUPPORT */

	/*  Stop send TX packets */
	RTMP_OS_NETDEV_STOP_QUEUE(pAd->net_dev);

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (MaxNumBss > MAX_MBSSID_NUM(pAd))
			MaxNumBss = MAX_MBSSID_NUM(pAd);

		/* first IdBss must not be 0 (BSS0), must be 1 (BSS1) */
		for (IdBss = FIRST_MBSSID; IdBss < MAX_MBSSID_NUM(pAd); IdBss++)
		{
			if (pAd->ApCfg.MBSSID[IdBss].wdev.if_dev)
				RTMP_OS_NETDEV_STOP_QUEUE(pAd->ApCfg.MBSSID[IdBss].wdev.if_dev);
		}
	}
#endif /* CONFIG_AP_SUPPORT */

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);

	/*  Disable RX */
	AsicSetMacTxRx(pAd, ASIC_MAC_RX, FALSE);

	/*  Polling TX/RX path until packets empty */
	MTPciPollTxRxEmpty(pAd);

	/*  Send Led off command */

	/*  Send radio off command and wait for ack */
	if (pAd->iwpriv_command == TRUE)
	{
		UINT32 value;
		RTMP_IO_READ32(pAd, AGG_TEMP, &value);
		value |= 0xffff0000;
		RTMP_IO_WRITE32(pAd, AGG_TEMP, value);
	} else {
#ifdef LOAD_FW_ONE_TIME
{
		UINT32 value;
		RTMP_IO_READ32(pAd, AGG_TEMP, &value);
		value |= 0xffff0000;
		RTMP_IO_WRITE32(pAd, AGG_TEMP, value);
}
#else /* LOAD_FW_ONE_TIME */
		CmdRadioOnOffCtrl(pAd, WIFI_RADIO_OFF);
#endif /* !LOAD_FW_ONE_TIME */
	}

	/*  Disable PDMA */
	AsicSetWPDMA(pAd, PDMA_TX_RX, 0);

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);

	/*  Disable Interrupt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE))
	{
		RTMP_ASIC_INTERRUPT_DISABLE(pAd);
	}
}
#endif

INT32 MTUsbPollingTxQBufferAvailable(RTMP_ADAPTER *pAd)
{
#define WAIT_RETRY_COUNT      (5)
#define WAIT_TIME             (2)

	UINT32 u4Value = 0;
	UINT32 u4Count = 0;
	UINT32 u4MaxQuota = 0;
	UINT32 u4AvailablePage = 0;
	UINT32 u4Return = STATUS_UNSUCCESSFUL;

	RTMP_IO_READ32(pAd, FC_P0, &u4Value);
	u4MaxQuota = GET_MAX_QUOTA_P0(u4Value);

	u4Count = 0;
	while (u4Count++ < WAIT_RETRY_COUNT)
	{
		RTMP_IO_READ32(pAd, FC_RP0P1, &u4Value);

		u4AvailablePage = GET_RSRV_CNT_P0(u4Value);

		DBGPRINT(RT_DEBUG_TRACE, ("%s: u4Count:%d, (u4MaxQuota=%d, u4AvailablePage=%d).\n", 
			__FUNCTION__, u4Count, u4MaxQuota, u4AvailablePage));

		// if P0(HIF) reserve pages is available
		if (u4AvailablePage == u4MaxQuota) 
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: waiting u4Count(%d) for P0(HIF) reserve pages available(u4AvailablePage=%d).\n", 
				__FUNCTION__, u4Count, u4AvailablePage));
			u4Return = STATUS_SUCCESS;
			break;
		}
		msleep(WAIT_TIME);	
	}	

	return u4Return;
}

#ifdef RTMP_USB_SUPPORT
VOID MTUsbPollTxRxEmpty(RTMP_ADAPTER *pAd)
{
	UINT32 Loop;
	UINT32 IdleTimes = 0;
	USB_DMA_CFG_STRUC UsbCfg;

	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));

	RtmpOsMsDelay(100);

	/* Poll Tx until empty */
	for (Loop = 0; Loop < 2000; Loop++)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return;

		USB_CFG_READ(pAd, &UsbCfg.word);

		if (UsbCfg.field.TxBusy == 0x00)
		{
			IdleTimes++;
			RtmpusecDelay(50);
		}
		else
			RtmpOsMsDelay(10);

		if (IdleTimes > 200)
			break;
	}

	if (Loop >= 2000)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s: TX DMA Busy!! UDMA_WLCFG_0 = %d\n",
										__FUNCTION__, UsbCfg.word));
	}

	IdleTimes = 0;

	/*  Poll Rx to empty */
	for (Loop = 0; Loop < 2000; Loop++)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return;

		USB_CFG_READ(pAd, &UsbCfg.word);

		if (UsbCfg.field.RxBusy == 0x00)
		{
			IdleTimes++;
			RtmpusecDelay(50);
		}
		else
			RtmpOsMsDelay(10);

		if (IdleTimes > 200)
			break;

		UsbRxCmdMsgsReceive(pAd);
		RTUSBBulkReceive(pAd);
	}


	if (Loop >= 2000)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s: RX DMA Busy!!  UDMA_WLCFG_0 = %d\n",
										__FUNCTION__, UsbCfg.word));
	}

	RtmpOsMsDelay(500);

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE);

}


VOID MTUsbMlmeRadioOff(PRTMP_ADAPTER pAd)
{
#ifdef WSC_INCLUDED
#ifdef WSC_LED_SUPPORT
	UINT	WPSLedMode10;

	if(LED_MODE(pAd) == WPS_LED_MODE_10)
	{
		WPSLedMode10 = LINK_STATUS_WPS_MODE10_TURN_OFF;
		RTEnqueueInternalCmd(pAd, CMDTHREAD_LED_WPS_MODE10, &WPSLedMode10, sizeof(WPSLedMode10));
	}
#endif /* WSC_LED_SUPPORT */
#endif /* WSC_INCLUDED */

#ifdef CONFIG_STA_SUPPORT
	/* Clear PMKID cache.*/
	pAd->StaCfg.SavedPMKNum = 0;
	RTMPZeroMemory(pAd->StaCfg.SavedPMK, (PMKID_NO * sizeof(BSSID_INFO)));

	/* Link down first if any association exists*/
	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		if (INFRA_ON(pAd) || ADHOC_ON(pAd))
		{
			MLME_DISASSOC_REQ_STRUCT DisReq;
			MLME_QUEUE_ELEM *pMsgElem;

			os_alloc_mem(pAd, (UCHAR **)&pMsgElem, sizeof(MLME_QUEUE_ELEM));
			if (pMsgElem)
			{
				COPY_MAC_ADDR(&DisReq.Addr, pAd->CommonCfg.Bssid);
				DisReq.Reason =  REASON_DISASSOC_STA_LEAVING;

				pMsgElem->Machine = ASSOC_STATE_MACHINE;
				pMsgElem->MsgType = MT2_MLME_DISASSOC_REQ;
				pMsgElem->MsgLen = sizeof(MLME_DISASSOC_REQ_STRUCT);
				NdisMoveMemory(pMsgElem->Msg, &DisReq, sizeof(MLME_DISASSOC_REQ_STRUCT));

				MlmeDisassocReqAction(pAd, pMsgElem);
				os_free_mem(NULL, pMsgElem);

				RtmpusecDelay(1000);
			}
		}
	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Link down first if any association exists*/
		if (INFRA_ON(pAd) || ADHOC_ON(pAd))
			LinkDown(pAd, FALSE);
		RtmpusecDelay(10000);

		/*==========================================*/
		/* Clean up old bss table*/
#ifndef ANDROID_SUPPORT
/* because abdroid will get scan table when interface down, so we not clean scan table */
		BssTableInit(&pAd->ScanTab);
#endif /* ANDROID_SUPPORT */

	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		APStop(pAd);
#endif /* CONFIG_AP_SUPPORT */

#ifdef LED_CONTROL_SUPPORT
	RTMPSetLED(pAd, LED_RADIO_OFF);
#endif /* LED_CONTROL_SUPPORT */

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);

	/*  Disable RX */
	AsicSetMacTxRx(pAd, ASIC_MAC_RX, FALSE);

	/*  Polling TX/RX path until packets empty */
	MTUsbPollTxRxEmpty(pAd);

	if (IS_MT7603(pAd))
	{
	/*  Send radio off command and wait for ack */
	if (pAd->iwpriv_command == TRUE)
	{
		UINT32 value;
		RTMP_IO_READ32(pAd, AGG_TEMP, &value);
		value |= 0xffff0000;
		RTMP_IO_WRITE32(pAd, AGG_TEMP, value);
	} else {
#ifdef LOAD_FW_ONE_TIME

		UINT32 value;
		RTMP_IO_READ32(pAd, AGG_TEMP, &value);
		value |= 0xffff0000;
		RTMP_IO_WRITE32(pAd, AGG_TEMP, value);
#else /* LOAD_FW_ONE_TIME */
		CmdRadioOnOffCtrl(pAd, WIFI_RADIO_OFF);	
#endif /* !LOAD_FW_ONE_TIME */	
	}
	}

	/* Set Radio off flag*/
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
}


VOID MTUsbMlmeRadioOn(
	IN PRTMP_ADAPTER pAd)
{
	int i;
	
    DBGPRINT(RT_DEBUG_OFF, ("%s()\n", __func__));

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);
    /* Clear Radio off flag*/
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);

#ifdef RTMP_MAC_USB
	/* Support multi-BulkIn IRP, pAd->CommonCfg.NumOfBulkInIRP may > 1. */
	for(i=0; i<pAd->CommonCfg.NumOfBulkInIRP; i++)
	{
		RTUSBBulkReceive(pAd);
		DBGPRINT(RT_DEBUG_TRACE, ("RTUSBBulkReceive!\n" ));
	}
#endif
	
	if (IS_MT7603(pAd))
	{

		if (pAd->iwpriv_command == TRUE)
        	{
                	UINT32 value;
                	RTMP_IO_READ32(pAd, AGG_TEMP, &value);
	                value &= 0x0000ffff;
        	        RTMP_IO_WRITE32(pAd, AGG_TEMP, value);
       		 } else {

			CmdRadioOnOffCtrl(pAd, WIFI_RADIO_ON);
		}
	}

	NICResetFromError(pAd);

#ifdef RTMP_MAC_USB
	/*	Enable RX */
	AsicSetMacTxRx(pAd, ASIC_MAC_RX, TRUE);
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		APStartUp(pAd);
#endif /* CONFIG_AP_SUPPORT */
#if 0
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		RTUSBBulkReceive(pAd);
#endif /* CONFIG_STA_SUPPORT */
#endif

#ifdef LED_CONTROL_SUPPORT
	/* Set LED*/
#ifdef CONFIG_STA_SUPPORT
	RTMPSetLED(pAd, LED_RADIO_ON);
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
	RTMPSetLED(pAd, LED_LINK_UP);
#endif /* CONFIG_AP_SUPPORT */
#endif /* LED_CONTROL_SUPPORT */
}
#endif

#ifdef RTMP_SDIO_SUPPORT
VOID MTSdioMlmeRadioOff(PRTMP_ADAPTER pAd)
{
#ifdef WSC_INCLUDED
#ifdef WSC_LED_SUPPORT
	if(LED_MODE(pAd) == WPS_LED_MODE_10)
	{
		WPSLedMode10 = LINK_STATUS_WPS_MODE10_TURN_OFF;
		RTEnqueueInternalCmd(pAd, CMDTHREAD_LED_WPS_MODE10, &WPSLedMode10, sizeof(WPSLedMode10));
	}
#endif /* WSC_LED_SUPPORT */
#endif /* WSC_INCLUDED */

#ifdef CONFIG_STA_SUPPORT
	/* Clear PMKID cache.*/
	pAd->StaCfg.SavedPMKNum = 0;
	RTMPZeroMemory(pAd->StaCfg.SavedPMK, (PMKID_NO * sizeof(BSSID_INFO)));

	/* Link down first if any association exists*/
	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		if (INFRA_ON(pAd) || ADHOC_ON(pAd))
		{
			MLME_DISASSOC_REQ_STRUCT DisReq;
			MLME_QUEUE_ELEM *pMsgElem;

			os_alloc_mem(pAd, (UCHAR **)&pMsgElem, sizeof(MLME_QUEUE_ELEM));
			if (pMsgElem)
			{
				COPY_MAC_ADDR(&DisReq.Addr, pAd->CommonCfg.Bssid);
				DisReq.Reason =  REASON_DISASSOC_STA_LEAVING;

				pMsgElem->Machine = ASSOC_STATE_MACHINE;
				pMsgElem->MsgType = MT2_MLME_DISASSOC_REQ;
				pMsgElem->MsgLen = sizeof(MLME_DISASSOC_REQ_STRUCT);
				NdisMoveMemory(pMsgElem->Msg, &DisReq, sizeof(MLME_DISASSOC_REQ_STRUCT));

				MlmeDisassocReqAction(pAd, pMsgElem);
				os_free_mem(NULL, pMsgElem);

				RtmpusecDelay(1000);
			}
		}
	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Link down first if any association exists*/
		if (INFRA_ON(pAd) || ADHOC_ON(pAd))
			LinkDown(pAd, FALSE);
		RtmpusecDelay(10000);

		/*==========================================*/
		/* Clean up old bss table*/
#ifndef ANDROID_SUPPORT
/* because abdroid will get scan table when interface down, so we not clean scan table */
		BssTableInit(&pAd->ScanTab);
#endif /* ANDROID_SUPPORT */

	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		APStop(pAd);
#endif /* CONFIG_AP_SUPPORT */

#ifdef LED_CONTROL_SUPPORT
	RTMPSetLED(pAd, LED_RADIO_OFF);
#endif /* LED_CONTROL_SUPPORT */

	if (IS_MT7636(pAd))
	{
		//CmdPmStateCtrl(pAd, PM5, ENTER_PM_STATE); replaced by bwlow function
		CmdExtPmStateCtrl(pAd, BSSID_WCID, PM5, ENTER_PM_STATE);
	}
//	RTMP_SDIO_WRITE32(pAd, WHLPCR, W_FW_OWN_REQ_SET);
	/* Set Radio off flag*/
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
}

VOID MTSdioMlmeRadioOn(PRTMP_ADAPTER pAd)
{
	UINT32 Value=0;
	UINT32 counter=0;

	MTWF_LOG(DBG_CAT_ALL, DBG_LVL_TRACE,("%s()\n", __func__));

    	/* Clear Radio off flag*/
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);

/*
	RTMP_SDIO_WRITE32(pAd, WHLPCR, W_FW_OWN_REQ_CLR);

	while(!GET_W_FW_OWN_REQ_SET(Value)) {
		RTMP_SDIO_READ32(pAd, WHLPCR, &Value);
		MTWF_LOG(DBG_CAT_ALL, DBG_LVL_OFF, ("%s(): Request FW-Own processing: %x\n",__FUNCTION__,Value));
		counter++;
		RtmpOsMsDelay(50);
		if(counter >100){
			MTWF_LOG(DBG_CAT_ALL, DBG_LVL_ERROR, ("%s:  FW-Own back Faiure\n",__FUNCTION__));
			break;
		}
	}
*/
	if (IS_MT7636(pAd))
	{
		//CmdPmStateCtrl(pAd, PM5, EXIT_PM_STATE); replaced by bwlow function
		CmdExtPmStateCtrl(pAd, BSSID_WCID, PM5, EXIT_PM_STATE);
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		APStartUp(pAd);
#endif /* CONFIG_AP_SUPPORT */
#if 0
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		RTUSBBulkReceive(pAd);
#endif /* CONFIG_STA_SUPPORT */
#endif

#ifdef LED_CONTROL_SUPPORT
	/* Set LED*/
#ifdef CONFIG_STA_SUPPORT
	RTMPSetLED(pAd, LED_RADIO_ON);
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
	RTMPSetLED(pAd, LED_LINK_UP);
#endif /* CONFIG_AP_SUPPORT */
#endif /* LED_CONTROL_SUPPORT */
}
#endif

VOID AsicSetRxGroup(RTMP_ADAPTER *pAd, UINT32 Port, UINT32 Group, BOOLEAN Enable)
{
	UINT32 Value;

	if (Port == HIF_PORT)
	{
		RTMP_MCU_IO_READ32(pAd, RXINF, &Value);

		if (Enable)
		{
			if (Group & RXS_GROUP1)
				Value |= RXSH_GROUP1_EN;

			if (Group & RXS_GROUP2)
				Value |= RXSH_GROUP2_EN;

			if (Group & RXS_GROUP3)
				Value |= RXSH_GROUP3_EN;
		}
		else
		{
			if (Group & RXS_GROUP1)
				Value &= ~RXSH_GROUP1_EN;

			if (Group & RXS_GROUP2)
				Value &= ~RXSH_GROUP2_EN;

			if (Group & RXS_GROUP3)
				Value &= ~RXSH_GROUP3_EN;
		}

		RTMP_MCU_IO_WRITE32(pAd, RXINF, Value);
	}
	 else if (Port == MCU_PORT)
	{
		RTMP_IO_READ32(pAd, DMA_DCR1, &Value);

		if (Enable)
		{
			if (Group & RXS_GROUP1)
				Value |= RXSM_GROUP1_EN;

			if (Group & RXS_GROUP2)
				Value |= RXSM_GROUP2_EN;

			if (Group & RXS_GROUP3)
				Value |= RXSM_GROUP3_EN;
		}
		else
		{
			if (Group & RXS_GROUP1)
				Value &= ~RXSM_GROUP1_EN;

			if (Group & RXS_GROUP2)
				Value &= ~RXSM_GROUP2_EN;

			if (Group & RXS_GROUP3)
				Value &= ~RXSM_GROUP3_EN;
		}

		RTMP_IO_WRITE32(pAd, DMA_DCR1, Value);
	}
	else
	{
		DBGPRINT(RT_DEBUG_OFF, ("illegal port (%d\n", Port));
	}
}


#if defined(RTMP_MAC_PCI) || defined(RTMP_MAC_USB)
static CHAR *dma_sch_str[] = {
	"LMAC",
	"ByPass",
	"HyBrid",
	};


/*
    DMA scheduer reservation page assignment
	Q0~Q3: WMM1
	Q4: Management queue
	Q5: MCU CMD
	Q7: Beacon
	Q8: MC/BC
	Q9~Q12: WMM2
	Q13: Management queue
*/
#define MAX_BEACON_SIZE		512
#define MAX_BMCAST_SIZE		1536
#define MAX_BMCAST_COUNT	3
#define MAX_MCUCMD_SIZE	4096 /*must >= MAX_DATA_SIZE */
#define MAX_DATA_SIZE		1792 /* 0xe*128=1792 */
#define MAX_AMSDU_DATA_SIZE	4096

/*Nobody uses it currently*/
#if 0
static INT32 SetResrvPage(RTMP_ADAPTER *pAd, UINT page_size)
{
	UINT32 max_beacon_page_count = MAX_BEACON_SIZE/page_size;
	UINT32 max_bmcast_page_count = MAX_BMCAST_SIZE/page_size;
	UINT32 max_mcucmd_page_count = MAX_MCUCMD_SIZE/page_size;
	UINT32 max_data_page_count = MAX_DATA_SIZE/page_size;
	UINT32 Total_page_count = 0;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->AMSDU: %d\n",__FUNCTION__,pAd->CommonCfg.BACapability.field.AmsduEnable));

	if(pAd->CommonCfg.BACapability.field.AmsduEnable==TRUE)
	{
		max_data_page_count = MAX_AMSDU_DATA_SIZE/page_size;
	}

	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_0, max_data_page_count);
	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_1, max_data_page_count);
	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_2, max_data_page_count);
	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_3, max_data_page_count);
	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_4, max_data_page_count);
	Total_page_count += max_data_page_count*5;
	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_5, max_mcucmd_page_count);
	Total_page_count += max_mcucmd_page_count;
//	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_6, max_data_page_count);
//	Total_page_count += max_data_page_count;
#ifdef CONFIG_AP_SUPPORT
	if (pAd->ApCfg.BssidNum  > 1)
	{
		RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_7, (max_beacon_page_count*(pAd->ApCfg.BssidNum - 1) + max_data_page_count));
		Total_page_count += (max_beacon_page_count*(pAd->ApCfg.BssidNum - 1) + max_data_page_count);
	} else {
		RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_7, max_data_page_count);
		Total_page_count += max_data_page_count;
	}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_7, max_data_page_count);
	Total_page_count += max_data_page_count;
#endif /* CONFIG_STA_SUPPORT */
	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_8, (max_bmcast_page_count*MAX_BMCAST_COUNT + max_data_page_count));
	Total_page_count += (max_bmcast_page_count*MAX_BMCAST_COUNT + max_data_page_count);
/* For MCC use */
//	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_9, max_data_page_count);
//	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_10, max_data_page_count);
//	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_11, max_data_page_count);
//	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_12, max_data_page_count);
//	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_13, max_data_page_count);
//	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_14, max_data_page_count);
//	RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_15, max_data_page_count);
//	Total_page_count += max_data_page_count*7;

	return Total_page_count;
}
#endif

/*
	HYBRID Mode: DMA scheduler would ignore the tx op time information from LMAC, and also use FFA and RSV for enqueue cal.
	BYPASS Mode: Only for Firmware download
*/
INT32 AsicDMASchedulerInit(RTMP_ADAPTER *pAd, INT mode)
{
	UINT32 mac_val;
#ifdef RTMP_PCI_SUPPORT
	UINT32 mac_restore_val;
#endif /* RTMP_PCI_SUPPORT */
	UINT32 page_size = 128;
	UINT32 page_cnt = 0x1ae;
	INT dma_mode = mode;

#ifdef CONFIG_FPGA_MODE
	dma_mode = pAd->fpga_ctl.dma_mode;

	DBGPRINT(RT_DEBUG_OFF, ("%s(): DMA Scheduler Mode=%d(%s)\n",
					__FUNCTION__, pAd->fpga_ctl.dma_mode,
					dma_sch_str[pAd->fpga_ctl.dma_mode]));
#endif /* CONFIG_FPGA_MODE */

#if defined(MT7603_FPGA) || defined(MT7628_FPGA) || defined(MT7636_FPGA)
	page_size = 256;
#endif /* MT7603_FPGA */

	/* Got PSE P0 MAX Quota */
#ifdef RTMP_PCI_SUPPORT
	RTMP_IO_READ32(pAd, MCU_PCIE_REMAP_2, &mac_restore_val);
	RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, MT_PSE_BASE_ADDR);
	RTMP_IO_READ32(pAd, 0x80120, &mac_val);
	page_cnt = (mac_val & 0x0fff0000) >> 16;
	RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, mac_restore_val);
#endif
#ifdef RTMP_MAC_USB
	RTUSB_VendorRequest(pAd, (USBD_TRANSFER_DIRECTION_IN |
            USBD_SHORT_TRANSFER_OK),
            DEVICE_VENDOR_REQUEST_IN,
            0x63, (0xa0000120 & 0xffff0000) >> 16,
            (0xa0000120 & 0x0000ffff), &mac_val, 4);
    page_cnt = (mac_val & 0x0fff0000) >> 16;
#endif

	/* Setting PSE page free priority,P0(HIF) > P1(MCU) >  P2Q2(TXS) >  P2Q1(RXV) =  P2Q0(Rxdata)*/
	mac_val = 0x00004037;
	RTMP_IO_WRITE32(pAd, FC_FRP,mac_val);

	if (dma_mode == DMA_SCH_BYPASS)
	{
		RTMP_IO_WRITE32(pAd, MT_SCH_REG_4, 1<<5);

		/* Disable DMA scheduler */
		RTMP_IO_READ32(pAd, AGG_DSCR1, &mac_val);
		mac_val |= 0x80000000;
		RTMP_IO_WRITE32(pAd, AGG_DSCR1, mac_val);

#ifdef RTMP_PCI_SUPPORT
		/*
			Wei-Guo's comment:
			2DW/7DW => 0x800C_006C[14:12] = 3'b0
			3DW/8DW =>0x800C_006C[14:12] = 3'b1
		*/
		// In FPGA mode, we need to change tx pad by different DMA scheduler setting!
		RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, 0x80080000);
		RTMP_IO_READ32(pAd, 0xc006c, &mac_val);
		mac_val &= (~(7<<12));
		mac_val |= (1<<12);
		RTMP_IO_WRITE32(pAd, 0xc006c, mac_val);
		// After change the Tx Padding CR of PCI-E Client, we need to re-map for PSE region
		RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, MT_PSE_BASE_ADDR);
#endif /* RTMP_PCI_SUPPORT */

		// work around for un-sync of TxD between HIF and LMAC
		RTMP_IO_READ32(pAd, DMA_DCR1, &mac_val);
		mac_val &= (~(0x7<<8));
		mac_val |= (0x1<<8);
		RTMP_IO_WRITE32(pAd, DMA_DCR1, mac_val);
	}

	if ((dma_mode == DMA_SCH_LMAC) || (dma_mode == DMA_SCH_HYBRID))
	{
		UINT32 max_beacon_page_count = MAX_BEACON_SIZE/page_size;
		UINT32 max_bmcast_page_count = MAX_BMCAST_SIZE/page_size;
		UINT32 max_mcucmd_page_count = MAX_MCUCMD_SIZE/page_size;
		UINT32 max_data_page_count = MAX_DATA_SIZE/page_size;
		UINT32 mcu_restore_val=0;
		UINT32 bcn_restore_val=0;
		UINT32 mbc_restore_val=0;

		/* Highest Priority Q7: Beacon > Q8: MC/BC > Q5: MCU CMD */
		mac_val = 0x55555553;
		RTMP_IO_WRITE32(pAd, MT_HIGH_PRIORITY_1, mac_val);
		mac_val = 0x78555555;
		RTMP_IO_WRITE32(pAd, MT_HIGH_PRIORITY_2, mac_val);

		/* Queue Priority */
		mac_val = 0x2b1a096e;
		RTMP_IO_WRITE32(pAd, MT_QUEUE_PRIORITY_1, mac_val);
		mac_val = 0x785f4d3c;
		RTMP_IO_WRITE32(pAd, MT_QUEUE_PRIORITY_2, mac_val);

		RTMP_IO_WRITE32(pAd, MT_PRIORITY_MASK, 0xffffffff);

		/* Schedule Priority, page size/FFA, FFA = (page_cnt * page_size) */
		mac_val = (2 << 28) | (page_cnt);
		RTMP_IO_WRITE32(pAd, MT_SCH_REG_1, mac_val);
		mac_val = MAX_DATA_SIZE / page_size;
		RTMP_IO_WRITE32(pAd, MT_SCH_REG_2, mac_val);

		/* Resvervation page */
		if(pAd->CommonCfg.BACapability.field.AmsduEnable==TRUE)
			max_data_page_count = MAX_AMSDU_DATA_SIZE/page_size;

		RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_0, max_data_page_count);
		RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_1, max_data_page_count);
		RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_2, max_data_page_count);
		RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_3, max_data_page_count);
		RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_4, max_data_page_count);

		mcu_restore_val = max_mcucmd_page_count;
		RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_5, mcu_restore_val);

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			if (pAd->ApCfg.BssidNum  > 1)
				bcn_restore_val = max_beacon_page_count*(pAd->ApCfg.BssidNum - 1) + max_data_page_count;
			else
				bcn_restore_val = max_data_page_count;
			RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_7, bcn_restore_val);
		}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			bcn_restore_val = max_data_page_count;
			RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_7, bcn_restore_val);
		}
#endif /* CONFIG_STA_SUPPORT */
		mbc_restore_val = max_bmcast_page_count*MAX_BMCAST_COUNT + max_data_page_count;
		RTMP_IO_WRITE32(pAd, MT_PAGE_CNT_8, mbc_restore_val);

		/* FFA1 max threshold */
		RTMP_IO_WRITE32(pAd, MT_RSV_MAX_THD, page_cnt);

		/* Group Threshold */
		if (MTK_REV_GTE(pAd, MT7603, MT7603E1) && MTK_REV_LT(pAd, MT7603, MT7603E2)) {
			RTMP_IO_WRITE32(pAd, MT_GROUP_THD_0, page_cnt);
			RTMP_IO_WRITE32(pAd, MT_BMAP_0, 0xffff);
		} else {
			/* Group 0 + 1+ 2 = PSE P0  */
			RTMP_IO_WRITE32(pAd, MT_GROUP_THD_0, (page_cnt - bcn_restore_val - mcu_restore_val));
			RTMP_IO_WRITE32(pAd, MT_GROUP_THD_1, bcn_restore_val);
			RTMP_IO_WRITE32(pAd, MT_BMAP_0, 0x0080ff5f);
			RTMP_IO_WRITE32(pAd, MT_GROUP_THD_2, mcu_restore_val);
			RTMP_IO_WRITE32(pAd, MT_BMAP_1, 0x00000020);
		}

		if (dma_mode == DMA_SCH_LMAC) {
			/* config as LMAC prediction mode */
			RTMP_IO_WRITE32(pAd, MT_SCH_REG_4, 0x0);
		}

		if (dma_mode == DMA_SCH_HYBRID)
		{
			/* config as hybrid mode */
			RTMP_IO_WRITE32(pAd, MT_SCH_REG_4, 1<<6);
#if defined(MT7603_FPGA) || defined(MT7628_FPGA)
#ifdef RTMP_PCI_SUPPORT
			/*
				Wei-Guo's comment:
				2DW/7DW => 0x800C_006C[14:12] = 3'b0
				3DW/8DW =>0x800C_006C[14:12] = 3'b1
			*/
			// In FPGA mode, we need to change tx pad by different DMA scheduler setting
			RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, 0x80080000);
			RTMP_IO_READ32(pAd, 0xc006c, &mac_val);
			mac_val &= (~(7<<12));
			RTMP_IO_WRITE32(pAd, 0xc006c, mac_val);
			// After change the Tx Padding CR of PCI-E Client, we need to re-map for PSE region
			RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, MT_PSE_BASE_ADDR);
#endif /* RTMP_PCI_SUPPORT */
#endif /* MT7603_FPGA */

			// Disable TxD padding
			RTMP_IO_READ32(pAd, DMA_DCR1, &mac_val);
			mac_val &= (~(0x7<<8));
			RTMP_IO_WRITE32(pAd, DMA_DCR1, mac_val);
		}
	}

	if (MTK_REV_GTE(pAd, MT7603,MT7603E1)) {
		mac_val = 0xfffff;
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_0, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_1, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_2, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_3, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_4, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_5, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_6, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_7, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_8, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_9, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_10, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_11, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_12, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_13, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_14, mac_val);
		RTMP_IO_WRITE32(pAd, MT_TXTIME_THD_15, mac_val);
		RTMP_IO_READ32(pAd, MT_SCH_REG_4, &mac_val);
		mac_val |= 0x40;
		RTMP_IO_WRITE32(pAd, MT_SCH_REG_4, mac_val);
	}

	return TRUE;
}
#endif


VOID AsicSetBARTxCntLimit(RTMP_ADAPTER *pAd, BOOLEAN Enable, UINT32 Count)
{
	UINT32 Value;

	RTMP_IO_READ32(pAd, AGG_MRCR, &Value);

	if (Enable)
	{
		Value &= ~BAR_TX_CNT_LIMIT_MASK;
		Value |= BAR_TX_CNT_LIMIT(Count);
	}
	else
	{
		Value &= ~BAR_TX_CNT_LIMIT_MASK;
		Value |= BAR_TX_CNT_LIMIT(0);
	}

	RTMP_IO_WRITE32(pAd, AGG_MRCR, Value);
}

VOID AsicSetRTSTxCntLimit(RTMP_ADAPTER *pAd, BOOLEAN Enable, UINT32 Count)
{
	UINT32 Value;

	RTMP_IO_READ32(pAd, AGG_MRCR, &Value);

	if (Enable)
	{
		Value &= ~RTS_RTY_CNT_LIMIT_MASK;
		Value |= RTS_RTY_CNT_LIMIT(Count);
	}
	else
	{
		Value &= ~RTS_RTY_CNT_LIMIT_MASK;
		Value |= RTS_RTY_CNT_LIMIT(0);
	}

	RTMP_IO_WRITE32(pAd, AGG_MRCR, Value);
}

BOOLEAN AsicSetBmcQCR(
    IN struct _RTMP_ADAPTER *pAd,
    IN UCHAR Operation,
    IN UCHAR CrReadWrite,
    IN UINT32 apidx,
    INOUT UINT32    *pcr_val)
{
    UINT32  cr_base = 0;
    UINT32  cr_val = 0;
    UCHAR   fcnt = 0;
    BOOLEAN result = FALSE;

    switch (Operation)
    {
        case BMC_FLUSH:
        {
            cr_base = ARB_BMCQCR1;
            cr_val = *pcr_val;
            RTMP_IO_WRITE32(pAd, cr_base, cr_val);

            /* check flush result */
            for (fcnt=0;fcnt<100;fcnt++)
            {
                RTMP_IO_READ32(pAd, cr_base, &cr_val);
                if (cr_val == 0)
                    break;
            }
            *pcr_val = cr_val;

            if (fcnt == 100)
            {
                DBGPRINT(RT_DEBUG_OFF,
                        ("%s: flush take too long, flush cnt=%d\n",
                        __func__,
                        fcnt)
                    );
                return result;
            }
        }
        break;

        case BMC_ENABLE:
        {
            cr_base = ARB_BMCQCR0;
            cr_val = *pcr_val;
            RTMP_IO_WRITE32(pAd, cr_base, cr_val);
        }
        break;

        case BMC_CNT_UPDATE:
        {
            if ((apidx <= 4))
                cr_base = ARB_BMCQCR2;
            else if ((apidx >= 5) && (apidx <= 12))
                cr_base = ARB_BMCQCR3;
            else if ((apidx >=13) && (apidx <= 15))
                cr_base = ARB_BMCQCR4;
            else
            {
                DBGPRINT(RT_DEBUG_ERROR, ("%s: apidx(%d) not support\n", __FUNCTION__, apidx));
                return result;
            }

            if (CrReadWrite == CR_READ)
            {
                RTMP_IO_READ32(pAd, cr_base, &cr_val);
                *pcr_val = cr_val;
            }
            else if (CrReadWrite == CR_WRITE)
            {
                cr_val = 0x1;//add 1 to cnt.
                if (apidx == 0)
                    RTMP_IO_WRITE32(pAd, cr_base, cr_val);
                else if (apidx >= 1 && apidx <= 4)
                    RTMP_IO_WRITE32(pAd, cr_base, (cr_val << ( ((apidx) * 4))));
                else if (apidx >= 5 && apidx <= 12)
                    RTMP_IO_WRITE32(pAd, cr_base, (cr_val << ((apidx - 5) * 4)));
                else if (apidx >= 13 && apidx <= 15)
                    RTMP_IO_WRITE32(pAd, cr_base, (cr_val << ((apidx - 13) * 4)));
            }
        }
        break;

        default:
            DBGPRINT(RT_DEBUG_OFF,
                    ("%s(): Operation Error (%d)\n",
                    __func__,
                    Operation)
                    );
            return result;
    }
    result = TRUE;
    return result;
}

VOID AsicSetTxSClassifyFilter(RTMP_ADAPTER *pAd, UINT32 Port, UINT8 DestQ,
								UINT32 AggNums, UINT32 Filter)
{
	UINT32 Value;

	if (Port == TXS2HOST)
	{
		RTMP_IO_READ32(pAd, DMA_TCFR1, &Value);
		Value &= ~TXS2H_BIT_MAP_MASK;
		Value |= TXS2H_BIT_MAP(Filter);
		Value &= ~TXS2H_AGG_CNT_MASK;
		Value |= TXS2H_AGG_CNT(AggNums);
		if (DestQ == 0)
			Value &= ~TXS2H_QID;
		else
			Value |= TXS2H_QID;

		RTMP_IO_WRITE32(pAd, DMA_TCFR1, Value);

	}
	else if (Port == TXS2MCU)
	{
		RTMP_IO_READ32(pAd, DMA_TCFR0, &Value);
		Value &= ~TXS2M_BIT_MAP_MASK;
		Value |= TXS2M_BIT_MAP(Filter);
		Value &= ~TXS2M_AGG_CNT_MASK;
		Value |= TXS2M_AGG_CNT(AggNums);
		Value &= ~TXS2M_QID_MASK;
		Value |= TXS2M_QID(DestQ);
		RTMP_IO_WRITE32(pAd, DMA_TCFR0, Value);
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Unknow Port(%d)\n", __FUNCTION__, Port));
	}
}

VOID MtAsicSetRxPspollFilter(RTMP_ADAPTER *pAd, CHAR enable)
{
	UINT32 mac_val;

	RTMP_IO_READ32(pAd, DMA_RCFR0, &mac_val);
	if (enable)
		mac_val |= 0x00000008; //Non-BAR Control frame to MCU
	else
		mac_val &= 0xfffffff7; //Non-BAR Control frame to HIF

	RTMP_IO_WRITE32(pAd, DMA_RCFR0, mac_val);

}

#if defined(MT7603) || defined(MT7628)
INT32 MtAsicGetThemalSensor(RTMP_ADAPTER *pAd, CHAR type)
	{
	/* 0: get temperature; 1: get adc */
	/* Get Thermal sensor adc cal value: 0x80022000 bits(8,14)	*/
	INT32 result=0;

	if ((type == 0) || (type == 1)) {
		UINT32 mac_val;
#ifdef RTMP_PCI_SUPPORT	
		UINT32 mac_restore_val;
		RTMP_IO_READ32(pAd, MCU_PCIE_REMAP_2, &mac_restore_val);
		RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, MT_TOP_REMAP_ADDR);
		RTMP_IO_READ32(pAd, MT_TOP_REMAP_ADDR_THEMAL, &mac_val);
		result = (mac_val & 0x00007f00) >> 8;
		RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, mac_restore_val);
#endif
#ifdef RTMP_MAC_USB
		RTMP_IO_READ32(pAd, MT_TOP_THEMAL_ADC, &mac_val);
		result = (mac_val & 0x00007f00) >> 8;
#endif
		if (type == 0) {
			INT32 g_ucThermoRefAdcVal, g_cThermoSlopeVariation, g_cThermoRefOffset;
	
			if (pAd->EEPROMImage[TEMPERATURE_SENSOR_CALIBRATION] & 0x80)
					g_ucThermoRefAdcVal = pAd->EEPROMImage[TEMPERATURE_SENSOR_CALIBRATION] & THERMO_REF_ADC_VARIATION_MASK;
			else
					g_ucThermoRefAdcVal = 52;
	
			if (pAd->EEPROMImage[THADC_ANALOG_PART] & 0x80) {
				g_cThermoSlopeVariation = pAd->EEPROMImage[THADC_SLOP] & THERMO_SLOPE_VARIATION_MASK;
				if (g_cThermoSlopeVariation > 16)
					g_cThermoSlopeVariation -= 32;
	}else
					g_cThermoSlopeVariation = 0;
				
			g_cThermoRefOffset = pAd->EEPROMImage[THERMAL_COMPENSATION_OFFSET] + 28;
			result = (((result - g_ucThermoRefAdcVal) * (56 + g_cThermoSlopeVariation) )/30) + g_cThermoRefOffset;
		}
	}

	return result;
	}


/*
  *  ucation: 0: stop; 1: flush; 2: start
  */

VOID MtAsicACQueue(RTMP_ADAPTER *pAd, UINT8 ucation, UINT8 BssidIdx, UINT32 u4AcQueueMap)
{
	UINT32 ACQCR_0 = 0, ACQCR_1 = 0;
	UINT32 Value_0 = 0, Value_1 = 0;
	UINT8 ucQueueIdx;

	if (ucation > 2)
		return;

	switch (ucation)
	{
		case AC_QUEUE_STOP:
			ACQCR_0 = ARB_TQCR4;
			ACQCR_1 = ARB_TQCR5;
			break;
		case AC_QUEUE_FLUSH:
			ACQCR_0 = ARB_TQCR2;
			ACQCR_1 = ARB_TQCR3;
			break;
		case AC_QUEUE_START:
			ACQCR_0 = ARB_TQCR0;
			ACQCR_1 = ARB_TQCR1;
			break;
	}

	for (ucQueueIdx = 0; ucQueueIdx < 14; ucQueueIdx++)
	{
	        if (u4AcQueueMap & (1 << ucQueueIdx)) {
			switch (ucQueueIdx)
	{
				case 0:
				case 1:
				case 2:
					Value_0 |= (1 << (ucQueueIdx*5 + BssidIdx));
					break;
				case 3:
				case 4:
				case 5:
					Value_0 |= (1 << (ucQueueIdx*5 + BssidIdx + 1));
					break;
				case 6:
					Value_1 |= (1 << (BssidIdx + 26));
					break;
				case 10:
				case 11:
				case 12:
					Value_1 |= (1 << ((ucQueueIdx - 10)*5 + BssidIdx));
					break;
				case 13:
				case 14:
					Value_1 |= (1 << ((ucQueueIdx - 10)*5 + BssidIdx + 1));
					break;
			}
 		}
	}

	if (ACQCR_0 && Value_0) {
		RTMP_IO_WRITE32(pAd, ACQCR_0, Value_0);
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Write CR:%x, Value=%x\n", __FUNCTION__, ACQCR_0, Value_0));
	}
	
	if (ACQCR_1 && Value_1) {
		RTMP_IO_WRITE32(pAd, ACQCR_1, Value_1);
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Write CR:%x, Value=%x\n", __FUNCTION__, ACQCR_1, Value_1));
	}
}


#endif /* MT7603 ||MT7628  */

