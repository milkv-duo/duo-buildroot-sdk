/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
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
	rtmp_init.c

	Abstract:
	Miniport generic portion header file

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/
#include	"rt_config.h"

#ifdef OS_ABL_FUNC_SUPPORT
/* Os utility link: printk, scanf */
RTMP_OS_ABL_OPS RaOsOps, *pRaOsOps = &RaOsOps;
#endif /* OS_ABL_FUNC_SUPPORT */

UCHAR NUM_BIT8[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
#ifdef DBG
char *CipherName[] = {"none","wep64","wep128","TKIP","AES","CKIP64","CKIP128","CKIP152","SMS4","WEP152"};
#endif



/*
	Use the global variable is not a good solution.
	But we can not put it to pAd and use the lock in pAd of RALINK_TIMER_STRUCT;
	Or when the structure is cleared, we maybe get NULL for pAd and can not lock.
	Maybe we can put pAd in RTMPSetTimer/ RTMPModTimer/ RTMPCancelTimer.
*/
NDIS_SPIN_LOCK TimerSemLock;


/*
	========================================================================

	Routine Description:
		Allocate RTMP_ADAPTER data block and do some initialization

	Arguments:
		Adapter		Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS
		NDIS_STATUS_FAILURE

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
NDIS_STATUS RTMPAllocAdapterBlock(VOID *handle, VOID **ppAdapter)
{
	RTMP_ADAPTER *pAd = NULL;
	NDIS_STATUS	 Status;
	INT index;

#ifdef OS_ABL_FUNC_SUPPORT
	/* must put the function before any print message */
	/* init OS utilities provided from UTIL module */
	RtmpOsOpsInit(&RaOsOps);
#endif /* OS_ABL_FUNC_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("--> RTMPAllocAdapterBlock\n"));

	/* init UTIL module */
	RtmpUtilInit();

	*ppAdapter = NULL;

	do
	{
		/* Allocate RTMP_ADAPTER memory block*/
		Status = AdapterBlockAllocateMemory(handle, (PVOID *)&pAd, sizeof(RTMP_ADAPTER));
		if (Status != NDIS_STATUS_SUCCESS)
		{
			DBGPRINT_ERR(("Failed to allocate memory - ADAPTER\n"));
			break;
		}
		else
		{
			/* init resource list (must be after pAd allocation) */
			initList(&pAd->RscTimerMemList);
			initList(&pAd->RscTaskMemList);
			initList(&pAd->RscLockMemList);
			initList(&pAd->RscTaskletMemList);
			initList(&pAd->RscSemMemList);
			initList(&pAd->RscAtomicMemList);

			initList(&pAd->RscTimerCreateList);

			pAd->OS_Cookie = handle;
			((POS_COOKIE)(handle))->pAd_va = (ULONG)pAd;
		}
		DBGPRINT(RT_DEBUG_OFF, ("\n\n=== pAd = %p, size = %zu ===\n\n", pAd, sizeof(RTMP_ADAPTER)));

		if (RtmpOsStatsAlloc(&pAd->stats, &pAd->iw_stats) == FALSE)
		{
			Status = NDIS_STATUS_FAILURE;
			break;
		}

		/* Init spin locks*/
		NdisAllocateSpinLock(pAd, &pAd->MgmtRingLock);
#ifdef RTMP_MAC_PCI
		for (index = 0; index < NUM_OF_RX_RING; index++)
			NdisAllocateSpinLock(pAd, &pAd->RxRingLock[index]);

#ifdef CONFIG_ANDES_SUPPORT
		NdisAllocateSpinLock(pAd, &pAd->CtrlRingLock);
#endif /* CONFIG_ANDES_SUPPORT */

#ifdef MT_MAC
		NdisAllocateSpinLock(pAd, &pAd->BcnRingLock);
#endif /* MT_MAC */

		NdisAllocateSpinLock(pAd, &pAd->tssi_lock);
#endif /* RTMP_MAC_PCI */

#if defined(RT3290) || defined(RLT_MAC)
		NdisAllocateSpinLock(pAd, &pAd->WlanEnLock);
#endif /* defined(RT3290) || defined(RLT_MAC) */

		for (index =0 ; index < NUM_OF_TX_RING; index++)
		{
			NdisAllocateSpinLock(pAd, &pAd->TxSwQueueLock[index]);
			NdisAllocateSpinLock(pAd, &pAd->DeQueueLock[index]);
			pAd->DeQueueRunning[index] = FALSE;
		}

#ifdef RESOURCE_PRE_ALLOC
		/*
			move this function from rt28xx_init() to here. now this function only allocate memory and
			leave the initialization job to RTMPInitTxRxRingMemory() which called in rt28xx_init().
		*/
		Status = RTMPAllocTxRxRingMemory(pAd);
		if (Status != NDIS_STATUS_SUCCESS)
		{
			DBGPRINT_ERR(("Failed to allocate memory - TxRxRing\n"));
			break;
		}
#endif /* RESOURCE_PRE_ALLOC */

		NdisAllocateSpinLock(pAd, &pAd->irq_lock);

#ifdef RTMP_MAC_PCI
		NdisAllocateSpinLock(pAd, &pAd->LockInterrupt);
#endif /* RTMP_MAC_PCI */

		NdisAllocateSpinLock(pAd, &TimerSemLock);

#ifdef SPECIFIC_BCN_BUF_SUPPORT
#ifdef RTMP_MAC_PCI
		NdisAllocateSpinLock(pAd, &pAd->ShrMemLock);
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		RTMP_SEM_EVENT_INIT(&pAd->ShrMemSemaphore, &pAd->RscSemMemList);
#endif /* RTMP_MAC_USB */
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

#ifdef WMM_ACM_SUPPORT
		NdisAllocateSpinLock(pAd, &pAd->AcmTspecSemLock);
		NdisAllocateSpinLock(pAd, &pAd->AcmTspecIrqLock);
#endif /* WMM_ACM_SUPPORT */

#ifdef CONFIG_ATE
#ifdef RTMP_MAC_USB
		RTMP_OS_ATMOIC_INIT(&pAd->BulkOutRemained, &pAd->RscAtomicMemList);
		RTMP_OS_ATMOIC_INIT(&pAd->BulkInRemained, &pAd->RscAtomicMemList);
#endif /* RTMP_MAC_USB */
#endif /* CONFIG_ATE */

		*ppAdapter = (VOID *)pAd;
	} while (FALSE);

	if (Status != NDIS_STATUS_SUCCESS)
	{
		if (pAd)
		{
			if (pAd->stats) {
				os_free_mem(NULL, pAd->stats);
				pAd->stats = NULL;
			}

			if (pAd->iw_stats) {
				os_free_mem(NULL, pAd->iw_stats);
				pAd->iw_stats = NULL;
			}

			RtmpOsVfree(pAd); 

		}

		return Status;
	}

	/* Init ProbeRespIE Table */
	for (index = 0; index < MAX_LEN_OF_BSS_TABLE; index++)
	{
		if (os_alloc_mem(pAd,&pAd->ProbeRespIE[index].pIe, MAX_VIE_LEN) == NDIS_STATUS_SUCCESS)
			RTMPZeroMemory(pAd->ProbeRespIE[index].pIe, MAX_VIE_LEN);
		else
			pAd->ProbeRespIE[index].pIe = NULL;
	}

	/*init queue for ip assembly*/
#if defined(MT_MAC)  && defined(IP_ASSEMBLY)
	for(index = 0; index < NUM_OF_TX_RING ; index++)
	{
		DlListInit(&pAd->assebQueue[index]);
	}
#endif

	DBGPRINT_S(("<-- RTMPAllocAdapterBlock, Status=%x\n", Status));

	return Status;
}


BOOLEAN RTMPCheckPhyMode(RTMP_ADAPTER *pAd, UINT8 band_cap, UCHAR *pPhyMode)
{
	BOOLEAN RetVal = TRUE;

	if (band_cap == RFIC_24GHZ)
	{
		if (!WMODE_2G_ONLY(*pPhyMode))
		{
			DBGPRINT(RT_DEBUG_TRACE,
					("%s(): Warning! The board type is 2.4G only!\n",
					__FUNCTION__));
			RetVal =  FALSE;
		}
	}
	else if (band_cap == RFIC_5GHZ)
	{
		if (!WMODE_5G_ONLY(*pPhyMode))
		{
			DBGPRINT(RT_DEBUG_TRACE,
					("%s(): Warning! The board type is 5G only!\n",
					__FUNCTION__));
			RetVal =  FALSE;
		}
	}
	else if (band_cap == RFIC_DUAL_BAND)
	{
		RetVal = TRUE;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,
				("%s(): Unknown supported band (%u), assume dual band used.\n",
				__FUNCTION__, band_cap));

		RetVal = TRUE;
	}

	if (RetVal == FALSE)
	{
#ifdef DOT11_N_SUPPORT
		if (band_cap == RFIC_5GHZ) /*5G ony: change to A/N mode */
			*pPhyMode = PHY_11AN_MIXED;
		else /* 2.4G only or Unknown supported band: change to B/G/N mode */
			*pPhyMode = PHY_11BGN_MIXED;
#else
		if (band_cap == RFIC_5GHZ) /*5G ony: change to A mode */
			*pPhyMode = PHY_11A;
		else /* 2.4G only or Unknown supported band: change to B/G mode */
			*pPhyMode = PHY_11BG_MIXED;
#endif /* !DOT11_N_SUPPORT */

		DBGPRINT(RT_DEBUG_TRACE,
				("%s(): Changed PhyMode to %u\n",
				__FUNCTION__, *pPhyMode));
	}

	return RetVal;

}


/*
	========================================================================

	Routine Description:
		Set default value from EEPROM

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
VOID NICInitAsicFromEEPROM(RTMP_ADAPTER *pAd)
{
#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_PCI
	UINT32 data = 0;
#endif /* RTMP_MAC_PCI */
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_ATE
	USHORT value;
#endif /* CONFIG_ATE */
	EEPROM_NIC_CONFIG2_STRUC NicConfig2;
#if defined(RT30xx) || defined(RT5390) || defined(RT5392)
	UCHAR bbpreg = 0;
#endif /* defined(RT30xx) || defined(RT5390) || defined(RT5392) */

	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitAsicFromEEPROM\n"));

	NicConfig2.word = pAd->NicConfig2.word;

	/* finally set primary ant */
	AntCfgInit(pAd);

	RTMP_CHIP_ASIC_INIT_TEMPERATURE_COMPENSATION(pAd);

#ifdef RTMP_RF_RW_SUPPORT
	/*Init RFRegisters after read RFIC type from EEPROM*/
	InitRFRegisters(pAd);
#endif /* RTMP_RF_RW_SUPPORT */

#ifdef ANT_DIVERSITY_SUPPORT
	if ((pAd->CommonCfg.bHWRxAntDiversity) &&
		(pAd->CommonCfg.RxAntDiversityCfg == ANT_HW_DIVERSITY_ENABLE) &&
		(pAd->chipOps.HwAntEnable))
		pAd->chipOps.HwAntEnable(pAd);
#endif

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef RTMP_MAC_PCI
		/* Read Hardware controlled Radio state enable bit*/
		if (NicConfig2.field.HardwareRadioControl == 1)
		{
			BOOLEAN radioOff = FALSE;
			pAd->StaCfg.bHardwareRadio = TRUE;

#ifdef RT3290
			if (IS_RT3290(pAd))
			{
				/* Read GPIO pin0 as Hardware controlled radio state */
				RTMP_IO_FORCE_READ32(pAd, WLAN_FUN_CTRL, &data);
				if ((data & 0x100) == 0)
					radioOff = TRUE;
			}
			else
#endif /* RT3290 */
			{
				/* Read GPIO pin2 as Hardware controlled radio state*/
				RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &data);
				if ((data & 0x04) == 0)
					radioOff = TRUE;
			}

			if (radioOff)
			{
				pAd->StaCfg.bHwRadio = FALSE;
				pAd->StaCfg.bRadio = FALSE;
				/* RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, 0x00001818); */
				RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			}
		}
		else
#endif /* RTMP_MAC_PCI */		
			pAd->StaCfg.bHardwareRadio = FALSE;

		DBGPRINT(RT_DEBUG_TRACE, ("Use Hw Radio Control Pin=%d\n",
					pAd->StaCfg.bHardwareRadio));

#ifdef LED_CONTROL_SUPPORT
		RTMPSetLED(pAd, pAd->StaCfg.bRadio == FALSE ? LED_RADIO_OFF : LED_RADIO_ON);
#endif /* LED_CONTROL_SUPPORT */

#ifdef RTMP_MAC_PCI
		if (pAd->StaCfg.bRadio == TRUE)
		{
			AsicSendCmdToMcuAndWait(pAd, 0x30, PowerRadioOffCID, 0xff, 0x02, FALSE);

			/*AsicSendCommandToMcu(pAd, 0x30, 0xff, 0xff, 0x02, FALSE);*/
			AsicSendCmdToMcuAndWait(pAd, 0x31, PowerWakeCID, 0x00, 0x00, FALSE);
		}
#endif /* RTMP_MAC_PCI */
	}

#ifdef PCIE_PS_SUPPORT
#if defined(RT3090) || defined(RT3592) || defined(RT3390) || defined(RT3593) || defined(RT5390) || defined(RT5392) || defined(RT5592) || defined(RT3290)
		if (IS_RT3090(pAd)|| IS_RT3572(pAd) || IS_RT3390(pAd)
			|| IS_RT3593(pAd) || IS_RT5390(pAd) || IS_RT5392(pAd)
			|| IS_RT5592(pAd) || IS_RT3290(pAd))
		{
			RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
			if (pChipOps->AsicReverseRfFromSleepMode)
				pChipOps->AsicReverseRfFromSleepMode(pAd, TRUE);
		}
		/* 3090 MCU Wakeup command needs more time to be stable. */
		/* Before stable, don't issue other MCU command to prevent from firmware error.*/
		if ((((IS_RT3090(pAd)|| IS_RT3572(pAd) ||IS_RT3390(pAd)
			|| IS_RT3593(pAd) || IS_RT5390(pAd) || IS_RT5392(pAd))
			  && IS_VERSION_AFTER_F(pAd)) || IS_RT5592(pAd) || IS_RT3290(pAd))
			&& (pAd->StaCfg.PSControl.field.rt30xxPowerMode == 3)
			&& (pAd->StaCfg.PSControl.field.EnableNewPS == TRUE))
		{
			DBGPRINT(RT_DEBUG_TRACE,("%s::%d,release Mcu Lock\n",__FUNCTION__,__LINE__));
			RTMP_SEM_LOCK(&pAd->McuCmdLock);
			pAd->brt30xxBanMcuCmd = FALSE;
			RTMP_SEM_UNLOCK(&pAd->McuCmdLock);
		}
#endif /* defined(RT3090) || defined(RT3592) || defined(RT3390) || defined(RT3593) || defined(RT5390) || defined(RT5392) */
#endif /* PCIE_PS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
#ifdef RTMP_MAC_USB
		if (IS_RT30xx(pAd)|| IS_RT3572(pAd))
		{
			RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
			if (pChipOps->AsicReverseRfFromSleepMode)
				pChipOps->AsicReverseRfFromSleepMode(pAd, TRUE);
		}
#endif /* RTMP_MAC_USB */

#ifdef WIN_NDIS
	/* Turn off patching for cardbus controller */
	/*
	if (NicConfig2.field.CardbusAcceleration == 1)
		pAd->bTest1 = TRUE;
	*/
#endif /* WIN_NDIS */

	if (NicConfig2.field.DynamicTxAgcControl == 1)
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = TRUE;
	else
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = FALSE;

#ifdef RTMP_INTERNAL_TX_ALC
	/*
	    Internal Tx ALC support is starting from RT3370 / RT3390, which combine PA / LNA in single chip.
	    The old chipset don't have this, add new feature flag RTMP_INTERNAL_TX_ALC.
	 */

	/* Internal Tx ALC */
	if (((NicConfig2.field.DynamicTxAgcControl == 1) &&
            (NicConfig2.field.bInternalTxALC == 1)) ||
            ((!IS_RT3390(pAd)) && (!IS_RT3350(pAd)) &&
            (!IS_RT3352(pAd)) && (!IS_RT5350(pAd)) &&
            (!IS_RT5390(pAd)) && (!IS_RT3290(pAd)) && (!IS_RT6352(pAd)) && (!IS_MT7601(pAd))))
	{
		/*
			If both DynamicTxAgcControl and bInternalTxALC are enabled,
			it is a wrong configuration.
			If the chipset does not support Internal TX ALC, we shall disable it.
		*/
		pAd->TxPowerCtrl.bInternalTxALC = FALSE;
	}
	else
	{
		if (NicConfig2.field.bInternalTxALC == 1)
			pAd->TxPowerCtrl.bInternalTxALC = TRUE;
		else
			pAd->TxPowerCtrl.bInternalTxALC = FALSE;
	}


	/* Old 5390 NIC always disables the internal ALC */
	if ((pAd->MACVersion == 0x53900501)
#ifdef RT6352
		&& !IS_RBUS_INF(pAd)
#endif /* RT6352 */
	)
		pAd->TxPowerCtrl.bInternalTxALC = FALSE;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: pAd->TxPowerCtrl.bInternalTxALC = %d\n",
		__FUNCTION__, pAd->TxPowerCtrl.bInternalTxALC));
#endif /* RTMP_INTERNAL_TX_ALC */

#ifdef CONFIG_ATE
	RT28xx_EEPROM_READ16(pAd, EEPROM_TSSI_GAIN_AND_ATTENUATION, value);
	value = (value & 0x00FF);

	if (IS_RT5390(pAd))
		pAd->TssiGain = 0x02;	 /* RT5390 uses 2 as TSSI gain/attenuation default value */
	else
		pAd->TssiGain = 0x03; /* RT5392 uses 3 as TSSI gain/attenuation default value */

	if ((value != 0x00) && (value != 0xFF))
		pAd->TssiGain =  (UCHAR) (value & 0x000F);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: EEPROM_TSSI_GAIN_AND_ATTENUATION = 0x%X, pAd->TssiGain=0x%x\n",
				__FUNCTION__,
				value,
				pAd->TssiGain));
#endif /* CONFIG_ATE */

	AsicSetRxStream(pAd, pAd->Antenna.field.RxPath);

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef MT_MAC
		if (pAd->chipCap.hif_type == HIF_MT)
			AsicSetTxStream(pAd, pAd->Antenna.field.TxPath);
		else
#endif /* MT_MAC */
		ASIC_RLT_SET_TX_STREAM(pAd, OPMODE_STA, FALSE);
	}
#endif /* CONFIG_STA_SUPPORT */

	RTMP_EEPROM_ASIC_INIT(pAd);

#if defined(RT30xx) || defined(RT5390) || defined(RT5392)
	/* Initialize RT3070 serial MAC registers which is different from RT2870 serial*/
	if (IS_RT3090(pAd) || IS_RT3390(pAd) || IS_RT3593(pAd) || IS_RT5390(pAd) || IS_RT5392(pAd))
	{
		UINT32 mac_val;

		/* enable DC filter*/
		if ((pAd->MACVersion & 0xffff) >= 0x0211)
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R103, 0xc0);
		}

		/* improve power consumption in RT3071 Ver.E */
		if (((pAd->MACVersion & 0xffff) >= 0x0211) && !IS_RT3593(pAd))
		{
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R31, &bbpreg);
			bbpreg &= (~0x3);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R31, bbpreg);
		}

		RTMP_IO_WRITE32(pAd, TX_SW_CFG1, 0);

		/* RT3071 version E has fixed this issue*/
		if ((pAd->MACVersion & 0xffff) < 0x0211)
		{
			if (pAd->NicConfig2.field.DACTestBit == 1)
				mac_val = 0x2C;	/* To fix throughput drop drastically*/
			else
				mac_val = 0x0f;	/* To fix throughput drop drastically*/
		}
		else
			mac_val = 0x0;
		RTMP_IO_WRITE32(pAd, TX_SW_CFG2, mac_val);
	}
	else if (IS_RT3070(pAd))
	{
		if ((pAd->MACVersion & 0xffff) >= 0x0201)
		{
			/* enable DC filter*/
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R103, 0xc0);

			/* improve power consumption in RT3070 Ver.F*/
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R31, &bbpreg);
			bbpreg &= (~0x3);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R31, bbpreg);
		}
		/*
		     RT3070(E) Version[0200]
		     RT3070(F) Version[0201]
		 */
		if (((pAd->MACVersion & 0xffff) < 0x0201))
		{
			RTMP_IO_WRITE32(pAd, TX_SW_CFG1, 0);
			RTMP_IO_WRITE32(pAd, TX_SW_CFG2, 0x2C);	/* To fix throughput drop drastically*/
		}
		else
		{
			RTMP_IO_WRITE32(pAd, TX_SW_CFG2, 0);
		}
	}
	else if (IS_RT3071(pAd) || IS_RT3572(pAd))
	{
		UINT32 mac_val;

		RTMP_IO_WRITE32(pAd, TX_SW_CFG1, 0);
		if (((pAd->MACVersion & 0xffff) < 0x0211))
		{
			if (pAd->NicConfig2.field.DACTestBit == 1)
				mac_val = 0x1f;	/* To fix throughput drop drastically*/
			else
				mac_val = 0x0f;	/* To fix throughput drop drastically*/
		}
		else
			mac_val = 0;
		RTMP_IO_WRITE32(pAd, TX_SW_CFG2, mac_val);
	}
#endif /* defined(RT30xx) || defined(RT5390) || defined(RT5392) */

#ifdef RT30xx
	/* update registers from EEPROM for RT3071 or later(3572/3562/3592).*/
	if (IS_RT3090(pAd) || IS_RT3572(pAd) || IS_RT3390(pAd))
	{
		UCHAR RegIdx, RegValue;
		USHORT value, i;

		/* after RT3071, write BBP from EEPROM 0xF0 to 0x102*/
		for (i = 0xF0; i <= 0x102; i += 2)
		{
			value = 0xFFFF;
			RT28xx_EEPROM_READ16(pAd, i, value);
			if ((value != 0xFFFF) && (value != 0))
			{
				RegIdx = (UCHAR)(value >> 8);
				RegValue  = (UCHAR)(value & 0xff);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, RegIdx, RegValue);
				DBGPRINT(RT_DEBUG_TRACE, ("Update BBP Registers from EEPROM(0x%0x), BBP(0x%x) = 0x%x\n", i, RegIdx, RegValue));
			}
		}

		/* after RT3071, write RF from EEPROM 0x104 to 0x116*/
		for (i = 0x104; i <= 0x116; i += 2)
		{
			value = 0xFFFF;
			RT28xx_EEPROM_READ16(pAd, i, value);
			if ((value != 0xFFFF) && (value != 0))
			{
				RegIdx = (UCHAR)(value >> 8);
				RegValue  = (UCHAR)(value & 0xff);
				RT30xxWriteRFRegister(pAd, RegIdx, RegValue);
				DBGPRINT(RT_DEBUG_TRACE, ("Update RF Registers from EEPROM0x%x), BBP(0x%x) = 0x%x\n", i, RegIdx, RegValue));
			}
		}
	}
#endif /* RT30xx */

	DBGPRINT(RT_DEBUG_TRACE, ("TxPath = %d, RxPath = %d, RFIC=%d\n",
				pAd->Antenna.field.TxPath, pAd->Antenna.field.RxPath, pAd->RfIcType));
	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitAsicFromEEPROM\n"));
}


INT hif_sys_exit(RTMP_ADAPTER *pAd)
{
#ifdef RESOURCE_PRE_ALLOC
	RTMPResetTxRxRingMemory(pAd);
#else
	RTMPFreeTxRxRingMemory(pAd);
#endif /* RESOURCE_PRE_ALLOC */

	return TRUE;
}


INT hif_sys_init(RTMP_ADAPTER *pAd, BOOLEAN bHardReset)
{
	NDIS_STATUS status;

	DBGPRINT(RT_DEBUG_TRACE, ("%s()-->\n", __FUNCTION__));

	RT28XXDMADisable(pAd);
	DBGPRINT(RT_DEBUG_TRACE, ("%s():Disable WPDMA\n", __FUNCTION__));

#ifdef RESOURCE_PRE_ALLOC
	status = RTMPInitTxRxRingMemory(pAd);
#else
	status = RTMPAllocTxRxRingMemory(pAd);
#endif /* RESOURCE_PRE_ALLOC */
	if (status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("RTMPAllocTxRxMemory failed, Status[=0x%08x]\n", status));
		goto err;
	}

#ifdef RTMP_MAC_USB
	pAd->CommonCfg.bMultipleIRP = FALSE;
	if (pAd->CommonCfg.bMultipleIRP)
		pAd->CommonCfg.NumOfBulkInIRP = RX_RING_SIZE;
	else
		pAd->CommonCfg.NumOfBulkInIRP = 1;
#endif /* RTMP_MAC_USB */

#ifdef WLAN_SKB_RECYCLE
	skb_queue_head_init(&pAd->rx0_recycle);
#endif /* WLAN_SKB_RECYCLE */

#ifdef RTMP_MAC_PCI
	{
		/* pbf hardware reset, asic simulation sequence put this ahead before loading firmware */
#if defined(RTMP_MAC) || defined(RLT_MAC)
		if (pAd->chipCap.hif_type == HIF_RTMP || pAd->chipCap.hif_type == HIF_RLT)
		{
#ifdef MT76x2
			// TODO: shiang-usw, check why MT762x don't need to do this!
			if (!IS_MT76x2(pAd))
#endif /* MT76x2 */
				RTMP_IO_WRITE32(pAd, WPDMA_RST_IDX, 0xffffffff /*0x1003f*/);

#ifdef RT8592
			// TODO: shiang-single driver, sync with windows, does 765x need this??
			if (!IS_RT65XX(pAd))
#endif /* RT8592 */
			{
				RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0xe1f);
				RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0xe00);
			}
		}
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

		AsicInitTxRxRing(pAd);
	}
#endif /* RTMP_MAC_PCI */

#ifdef MT_MAC
	mt_hif_sys_init(pAd);
#endif /* MT_MAC */

	DBGPRINT(RT_DEBUG_TRACE, ("<--%s(), Success!\n", __FUNCTION__));
	return TRUE;

err:
	DBGPRINT(RT_DEBUG_TRACE, ("<--%s(), Err! status=%d\n", __FUNCTION__, status));
	return FALSE;
}


/*
	========================================================================

	Routine Description:
		Initialize NIC hardware

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
NDIS_STATUS	NICInitializeAdapter(RTMP_ADAPTER *pAd, BOOLEAN bHardReset)
{
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;
	UINT rty_cnt = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("%s():--> \n", __FUNCTION__));

	/* Set DMA global configuration except TX_DMA_EN and RX_DMA_EN bits */
retry:

	if (AsicWaitPDMAIdle(pAd, 100, 1000) != TRUE) {
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return NDIS_STATUS_FAILURE;
	}

	/* Initialze ASIC for TX & Rx operation */
	if (NICInitializeAsic(pAd , bHardReset) != NDIS_STATUS_SUCCESS)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return NDIS_STATUS_FAILURE;

		if (pAd->chipOps.loadFirmware)
		{
			if (rty_cnt++ == 0)
			{
				NICLoadFirmware(pAd);
				goto retry;
			}
		}
		return NDIS_STATUS_FAILURE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<--%s()\n", __FUNCTION__));
	return Status;
}


INT rtmp_hif_cyc_init(RTMP_ADAPTER *pAd, UINT8 val)
{

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return FALSE;
	}

#if defined(RTMP_MAC) || defined(RLT_MAC)
	if (pAd->chipCap.hif_type == HIF_RTMP || pAd->chipCap.hif_type == HIF_RLT)
	{
		US_CYC_CNT_STRUC USCycCnt;

		RTMP_IO_READ32(pAd, US_CYC_CNT, &USCycCnt.word);
		USCycCnt.field.UsCycCnt = val;
		RTMP_IO_WRITE32(pAd, US_CYC_CNT, USCycCnt.word);
	}
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

	return TRUE;
}


/*
	========================================================================

	Routine Description:
		Initialize ASIC

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
NDIS_STATUS NICInitializeAsic(RTMP_ADAPTER *pAd, BOOLEAN bHardReset)
{
	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitializeAsic\n"));

#ifdef RTMP_MAC_PCI
	NICInitPwrPinCfg(pAd);
#endif /* RTMP_MAC_PCI */

		// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support rtmp_mac_sys_reset () for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	} else {
#if defined(RTMP_MAC) || defined(RLT_MAC)
		if (rtmp_mac_sys_reset(pAd, bHardReset) == FALSE)
			return NDIS_STATUS_FAILURE;
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */
	}


#ifdef RTMP_PCI_SUPPORT
	pAd->CommonCfg.bPCIeBus = FALSE;
#ifdef RTMP_MAC
	if ((pAd->chipCap.hif_type == HIF_RTMP) && IS_PCI_INF(pAd))
	{
#ifdef RT3290
		if (!IS_RT3290(pAd))
#endif /* RT3290 */
		{
			UINT32 mac_val = 0;

			/* PCI and PCIe have different cyc value */
			RTMP_IO_READ32(pAd, PCI_CFG, &mac_val);
			if ((mac_val & 0x10000) == 0)
			{
				rtmp_hif_cyc_init(pAd, 0x7d);
				pAd->CommonCfg.bPCIeBus = TRUE;
			}
		}
	}
#endif /* RTMP_MAC */
	DBGPRINT(RT_DEBUG_TRACE, ("%s():device act as PCI%s driver\n",
					__FUNCTION__, (pAd->CommonCfg.bPCIeBus ? "-E" : "")));
#endif /* RTMP_PCI_SUPPORT */

#if defined(RTMP_MAC) || defined(RLT_MAC)
	if (pAd->chipCap.hif_type == HIF_RTMP || pAd->chipCap.hif_type == HIF_RLT)
	{
		if (pAd->chipOps.FwInit)
			pAd->chipOps.FwInit(pAd);

		rtmp_mac_init(pAd);

		rtmp_mac_bcn_buf_init(pAd, bHardReset);

		rtmp_hw_tb_init(pAd, bHardReset);

#if defined(CONFIG_CSO_SUPPORT) || defined(CONFIG_TSO_SUPPORT)
		// TODO: shiang-6590, we need configure TSO before can do tx/rx
		rlt_net_acc_init(pAd);
#endif /* CONFIG_CSO_SUPPORT */
	}
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

#ifdef MT_MAC
	if (pAd->chipCap.hif_type == HIF_MT) {
		// TODO: shiang-7603, init MAC setting
		// TODO: shiang-7603, init beacon buffer
		mt_mac_init(pAd);
		mt_hw_tb_init(pAd, bHardReset);
	}
#endif /* MT_MAC */

	/* Set the current MAC to ASIC */
	AsicSetDevMac(pAd, pAd->CurrentAddress, 0x0);

	DBGPRINT_RAW(RT_DEBUG_TRACE,("Current MAC: =%02x:%02x:%02x:%02x:%02x:%02x\n",
					PRINT_MAC(pAd->CurrentAddress)));

	NICInitBBP(pAd);

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd))
	{
		// TODO: shiang-usw, why USB need disableSync before change clock???
		AsicDisableSync(pAd);

		/* Default PCI clock cycle per ms is different as default setting, which is based on PCI.*/
		if (pAd->chipCap.hif_type == HIF_RTMP)
			rtmp_hif_cyc_init(pAd, 0x1e);
	}
#endif /* RTMP_MAC_USB */

	/* Clear raw counters*/
	NicResetRawCounters(pAd);

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Add radio off control */
		if (pAd->StaCfg.bRadio == FALSE)
		{
			/* RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, 0x00001818);*/
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			DBGPRINT(RT_DEBUG_TRACE, ("Set Radio Off\n"));
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitializeAsic\n"));

	return NDIS_STATUS_SUCCESS;
}



/*
	========================================================================

	Routine Description:
		Reset NIC from error

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		Reset NIC from error state

	========================================================================
*/
VOID NICResetFromError(RTMP_ADAPTER *pAd)
{
	UCHAR rf_channel = 0;

	/* Reset BBP (according to alex, reset ASIC will force reset BBP*/
	/* Therefore, skip the reset BBP*/
	/* RTMP_IO_WRITE32(pAd, MAC_CSR1, 0x2);*/
	// TODO: shaing-7603
	if (IS_MT7603(pAd) || IS_MT7628(pAd)) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): for MT7603\n", __FUNCTION__));
		
		NICInitializeAdapter(pAd, FALSE);
		
		NICInitAsicFromEEPROM(pAd);
		
		RTMPEnableRxTx(pAd);
		
		return;
	}

#ifndef MT_MAC
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x1);
	/* Remove ASIC from reset state*/
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x0);
#endif /*ndef MT_MAC */

	NICInitializeAdapter(pAd, FALSE);
	NICInitAsicFromEEPROM(pAd);

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		AsicBBPAdjust(pAd);
	}

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		AsicStaBbpTuning(pAd);
	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (INFRA_ON(pAd) && (pAd->CommonCfg.CentralChannel != pAd->CommonCfg.Channel)
			&& (pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth == BW_40))
			rf_channel = pAd->CommonCfg.CentralChannel;
		else
			rf_channel = pAd->CommonCfg.Channel;
	}
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		rf_channel = pAd->CommonCfg.CentralChannel;
	}
#endif /* CONFIG_AP_SUPPORT */

#if defined(CONFIG_AP_SUPPORT) || defined(CONFIG_STA_SUPPORT)
	AsicSwitchChannel(pAd, rf_channel, FALSE);
	AsicLockChannel(pAd, rf_channel);
#endif /* defined(CONFIG_AP_SUPPORT) || defined(CONFIG_STA_SUPPORT) */
}


VOID NICUpdateFifoStaCounters(RTMP_ADAPTER *pAd)
{
#ifdef CONFIG_ATE
	/* Nothing to do in ATE mode */
	if (ATE_ON(pAd))
		return;
#endif /* CONFIG_ATE */

#ifdef RTMP_MAC_USB
#ifdef CONFIG_STA_SUPPORT
	if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
		return;
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_AP_SUPPORT
#ifdef RT65xx
	if (pAd->MacTab.Size <= 8)
	{
		if (IS_RT65XX(pAd))
			return;
	}
#endif
#endif

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
//		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
//							__FUNCTION__, __LINE__));
		return;
	}

#if defined(RTMP_MAC) || defined(RLT_MAC)
	if (pAd->chipCap.hif_type == HIF_RTMP || pAd->chipCap.hif_type == HIF_RLT)
		rtmp_mac_fifo_stat_update(pAd);
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */
}


int load_patch(RTMP_ADAPTER *ad)
{
	int ret = NDIS_STATUS_SUCCESS;
	ULONG Old, New, Diff;

	if (ad->chipOps.load_rom_patch) {
		RTMP_GetCurrentSystemTick(&Old);
		ret = ad->chipOps.load_rom_patch(ad);
		RTMP_GetCurrentSystemTick(&New);
		Diff = (New - Old) * 1000 / OS_HZ;
		DBGPRINT(RT_DEBUG_TRACE, ("load rom patch spent %ldms\n", Diff));
	}

	return ret;
}


int NICLoadFirmware(RTMP_ADAPTER *ad)
{
	int ret	= NDIS_STATUS_SUCCESS;
	ULONG Old, New;

	if (ad->chipOps.loadFirmware) {
		RTMP_GetCurrentSystemTick(&Old);
		ret = ad->chipOps.loadFirmware(ad);
		RTMP_GetCurrentSystemTick(&New);
		DBGPRINT(RT_DEBUG_TRACE, ("load fw spent %ldms\n", (New - Old) * 1000 / OS_HZ));

	}

	return ret;
}


VOID NICEraseFirmware(RTMP_ADAPTER *pAd)
{
	if (pAd->chipOps.eraseFirmware)
		pAd->chipOps.eraseFirmware(pAd);
}


/*
	========================================================================

	Routine Description:
		Compare two memory block

	Arguments:
		pSrc1		Pointer to first memory address
		pSrc2		Pointer to second memory address

	Return Value:
		0:			memory is equal
		1:			pSrc1 memory is larger
		2:			pSrc2 memory is larger

	IRQL = DISPATCH_LEVEL

	Note:

	========================================================================
*/
ULONG RTMPCompareMemory(VOID *pSrc1, VOID *pSrc2, ULONG Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	ULONG	Index = 0;

	pMem1 = (PUCHAR) pSrc1;
	pMem2 = (PUCHAR) pSrc2;

	for (Index = 0; Index < Length; Index++)
	{
		if (pMem1[Index] > pMem2[Index])
			return (1);
		else if (pMem1[Index] < pMem2[Index])
			return (2);
	}

	/* Equal*/
	return (0);
}


/*
	========================================================================

	Routine Description:
		Zero out memory block

	Arguments:
		pSrc1		Pointer to memory address
		Length		Size

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	Note:

	========================================================================
*/
VOID RTMPZeroMemory(VOID *pSrc, ULONG Length)
{
	PUCHAR	pMem;
	ULONG	Index = 0;

	pMem = (PUCHAR) pSrc;

	for (Index = 0; Index < Length; Index++)
	{
		pMem[Index] = 0x00;
	}
}


/*
	========================================================================

	Routine Description:
		Copy data from memory block 1 to memory block 2

	Arguments:
		pDest		Pointer to destination memory address
		pSrc		Pointer to source memory address
		Length		Copy size

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	Note:

	========================================================================
*/
VOID RTMPMoveMemory(VOID *pDest, VOID *pSrc, ULONG Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	UINT	Index;

	ASSERT((Length==0) || (pDest && pSrc));
	if (Length <= 0 || !pDest || !pSrc)
		return;

	pMem1 = (PUCHAR) pDest;
	pMem2 = (PUCHAR) pSrc;

	for (Index = 0; Index < Length; Index++)
	{
		pMem1[Index] = pMem2[Index];
	}
}

#ifdef WPA3_SUPPORT
VOID RtmpAKMSuiteTableInit(IN RTMP_ADAPTER * pAd)
{
	UINT8 i;

	pAd->RSNAKMSuitesTable[0].dot11RSNAKMSuite = WPA_AKM_SUITE_NONE;
	pAd->RSNAKMSuitesTable[1].dot11RSNAKMSuite = WPA_AKM_SUITE_802_1X;
	pAd->RSNAKMSuitesTable[2].dot11RSNAKMSuite = WPA_AKM_SUITE_PSK;
	pAd->RSNAKMSuitesTable[3].dot11RSNAKMSuite = RSN_AKM_SUITE_NONE;
	pAd->RSNAKMSuitesTable[4].dot11RSNAKMSuite = RSN_AKM_SUITE_802_1X;
	pAd->RSNAKMSuitesTable[5].dot11RSNAKMSuite = RSN_AKM_SUITE_PSK;

#ifdef CFG_SUPPORT_802_11W
	pAd->RSNAKMSuitesTable[6].dot11RSNAKMSuite = RSN_AKM_SUITE_802_1X_SHA256;
	pAd->RSNAKMSuitesTable[7].dot11RSNAKMSuite = RSN_AKM_SUITE_PSK_SHA256;
#endif
	pAd->RSNAKMSuitesTable[8].dot11RSNAKMSuite = RSN_AKM_SUITE_8021X_SUITE_B;
	pAd->RSNAKMSuitesTable[9].dot11RSNAKMSuite = RSN_AKM_SUITE_8021X_SUITE_B_192;
	pAd->RSNAKMSuitesTable[10].dot11RSNAKMSuite = RSN_AKM_SUITE_SAE;
	pAd->RSNAKMSuitesTable[11].dot11RSNAKMSuite = RSN_AKM_SUITE_OWE;

	for (i = 0; i < MAX_NUM_SUPPORTED_AKM_SUITES; i++)
		pAd->RSNAKMSuitesTable[i].dot11RSNAKMSuiteEnabled = FALSE;
}

#endif

VOID UserCfgExit(RTMP_ADAPTER *pAd)
{
#ifdef RT_CFG80211_SUPPORT
	if (pAd->cfg80211_ctrl.FlgCfg80211Scanning)
		CFG80211OS_ScanEnd(CFG80211CB, TRUE);

	/* Reset the CFG80211 Internal Flag */
	RTMP_DRIVER_80211_RESET(pAd);
#endif /* RT_CFG80211_SUPPORT */

#ifdef DOT11_N_SUPPORT
	BATableExit(pAd);
#endif /* DOT11_N_SUPPORT */

#ifdef WFA_WFD_SUPPORT
	pAd->WfdIeInBeaconLen = 0;
	if (pAd->pWfdIeInBeacon)
	{
		os_free_mem(NULL, pAd->pWfdIeInBeacon);
		pAd->pWfdIeInBeacon = NULL;
	}
	pAd->WfdIeInProbeReqLen= 0;
	if (pAd->pWfdIeInProbeReq)
	{
		os_free_mem(NULL, pAd->pWfdIeInProbeReq);
		pAd->pWfdIeInProbeReq = NULL;
	}
	pAd->WfdIeInProbeRspLen= 0;
	if (pAd->pWfdIeInProbeRsp)
	{
		os_free_mem(NULL, pAd->pWfdIeInProbeRsp);
		pAd->pWfdIeInProbeRsp = NULL;
	}
	pAd->WfdIeInActionPktLen = 0;
	if (pAd->pWfdIeInActionPkt)
	{
		os_free_mem(NULL, pAd->pWfdIeInActionPkt);
		pAd->pWfdIeInActionPkt = NULL;
	}
	pAd->WfdIeInAssocReqLen= 0;
	if (pAd->pWfdIeInAssocReq)
	{
		os_free_mem(NULL, pAd->pWfdIeInAssocReq);
		pAd->pWfdIeInAssocReq = NULL;
	}
	pAd->WfdIeInAssocRspLen= 0;
	if (pAd->pWfdIeInAssocRsp)
	{
		os_free_mem(NULL, pAd->pWfdIeInAssocRsp);
		pAd->pWfdIeInAssocRsp = NULL;
	}
#endif /* WFA_WFD_SUPPORT */

	NdisFreeSpinLock(&pAd->MacTabLock);

#ifdef RTMP_PCI_SUPPORT
	NdisFreeSpinLock(&pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_DESTORY(&pAd->IndirectUpdateLock);
#endif

#ifdef MAC_REPEATER_SUPPORT
	NdisFreeSpinLock(&pAd->ApCfg.ReptCliEntryLock);
#endif /* MAC_REPEATER_SUPPORT */
}


/*
	========================================================================

	Routine Description:
		Initialize port configuration structure

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
VOID UserCfgInit(RTMP_ADAPTER *pAd)
{
	UINT i;
#ifdef CONFIG_AP_SUPPORT
	UINT j;
#endif /* CONFIG_AP_SUPPORT */
	UINT key_index, bss_index;

	DBGPRINT(RT_DEBUG_TRACE, ("--> UserCfgInit\n"));

	pAd->IndicateMediaState = NdisMediaStateDisconnected;

	/* part I. intialize common configuration */
	pAd->CommonCfg.BasicRateBitmap = 0xF;
	pAd->CommonCfg.BasicRateBitmapOld = 0xF;

#ifdef RTMP_MAC_USB
	pAd->BulkOutReq = 0;

	pAd->BulkOutComplete = 0;
	pAd->BulkOutCompleteOther = 0;
	pAd->BulkOutCompleteCancel = 0;
	pAd->bUsbTxBulkAggre = 0;

#ifdef LED_CONTROL_SUPPORT
	/* init as unsed value to ensure driver will set to MCU once.*/
	pAd->LedCntl.LedIndicatorStrength = 0xFF;
	pAd->LedCntl.LedConfigMethod = 0;
#ifdef LED_CONTROL_METHOD_1
	pAd->LedCntl.LedConfigMethod = 1;
#endif /* LED_CONTROL_Method1 */
#endif /* LED_CONTROL_SUPPORT */

	pAd->CommonCfg.MaxPktOneTxBulk = 2;
	pAd->CommonCfg.TxBulkFactor = 1;
	pAd->CommonCfg.RxBulkFactor =1;

	pAd->CommonCfg.TxPower = 100; /*mW*/

	NdisZeroMemory(&pAd->CommonCfg.IOTestParm, sizeof(pAd->CommonCfg.IOTestParm));
#ifdef CONFIG_STA_SUPPORT
	pAd->CountDowntoPsm = 0;
	pAd->StaCfg.Connectinfoflag = FALSE;
#endif /* CONFIG_STA_SUPPORT */

#endif /* RTMP_MAC_USB */

	for(key_index=0; key_index<SHARE_KEY_NUM; key_index++)
	{
		for(bss_index = 0; bss_index < MAX_MBSSID_NUM(pAd) + MAX_P2P_NUM; bss_index++)
		{
			pAd->SharedKey[bss_index][key_index].KeyLen = 0;
			pAd->SharedKey[bss_index][key_index].CipherAlg = CIPHER_NONE;
		}
	}
#ifdef WPA3_SUPPORT
	RtmpAKMSuiteTableInit(pAd);
#endif

	pAd->bLocalAdminMAC = FALSE;
	pAd->EepromAccess = FALSE;

	pAd->Antenna.word = 0;
	pAd->CommonCfg.BBPCurrentBW = BW_20;

#ifdef RTMP_MAC_PCI
#ifdef LED_CONTROL_SUPPORT
	pAd->LedCntl.LedIndicatorStrength = 0;
#endif /* LED_CONTROL_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	pAd->RLnkCtrlOffset = 0;
	pAd->HostLnkCtrlOffset = 0;
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_MAC_PCI */

	pAd->bAutoTxAgcA = FALSE;			/* Default is OFF*/
	pAd->bAutoTxAgcG = FALSE;			/* Default is OFF*/

#if defined(RTMP_INTERNAL_TX_ALC) || defined(RTMP_TEMPERATURE_COMPENSATION)
	pAd->TxPowerCtrl.bInternalTxALC = FALSE; /* Off by default */
	pAd->TxPowerCtrl.idxTxPowerTable = 0;
	pAd->TxPowerCtrl.idxTxPowerTable2 = 0;
#ifdef RTMP_TEMPERATURE_COMPENSATION
	pAd->TxPowerCtrl.LookupTableIndex = 0;
#endif /* RTMP_TEMPERATURE_COMPENSATION */
#endif /* RTMP_INTERNAL_TX_ALC || RTMP_TEMPERATURE_COMPENSATION */

#ifdef THERMAL_PROTECT_SUPPORT
	pAd->switch_tx_stream = FALSE;
	pAd->force_one_tx_stream = FALSE;
	pAd->last_thermal_pro_temp = 0;
#endif /* THERMAL_PROTECT_SUPPORT */

	pAd->RfIcType = RFIC_2820;

	/* Init timer for reset complete event*/
	pAd->CommonCfg.CentralChannel = 1;
	pAd->bForcePrintTX = FALSE;
	pAd->bForcePrintRX = FALSE;
	pAd->bStaFifoTest = FALSE;
	pAd->bProtectionTest = FALSE;
	pAd->bHCCATest = FALSE;
	pAd->bGenOneHCCA = FALSE;
	pAd->CommonCfg.Dsifs = 10;      /* in units of usec */
	pAd->CommonCfg.TxPower = 100; /* mW*/
	pAd->CommonCfg.TxPowerPercentage = 0xffffffff; /* AUTO*/
	pAd->CommonCfg.TxPowerDefault = 0xffffffff; /* AUTO*/
	pAd->CommonCfg.TxPreamble = Rt802_11PreambleAuto; /* use Long preamble on TX by defaut*/
	pAd->CommonCfg.bUseZeroToDisableFragment = FALSE;
	pAd->CommonCfg.RtsThreshold = 2347;
	pAd->CommonCfg.FragmentThreshold = 2346;
	pAd->CommonCfg.UseBGProtection = 0;    /* 0: AUTO*/
	pAd->CommonCfg.bEnableTxBurst = TRUE; /* 0;    	*/
	pAd->CommonCfg.PhyMode = 0xff;     /* unknown*/
	pAd->CommonCfg.SavedPhyMode = pAd->CommonCfg.PhyMode;
	pAd->CommonCfg.BandState = UNKNOWN_BAND;

	pAd->wmm_cw_min = 4;
	switch (pAd->OpMode)
	{
		case OPMODE_AP:
			pAd->wmm_cw_max = 6;
			break;
		case OPMODE_STA:
			pAd->wmm_cw_max = 10;
			break;
	}

#ifdef RT3052
#ifdef RELEASE_EXCLUDE
/*
  CID and CN(chip name) is used to check the chip version of 2880 for SoC
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RTMP_RBUS_SUPPORT
	if (pAd->infType == RTMP_DEV_INF_RBUS)
	{
		RTMP_SYS_IO_READ32(0xb000000c, &pAd->CommonCfg.CID);
		RTMP_SYS_IO_READ32(0xb0000000, &pAd->CommonCfg.CN);
	}
#endif /* RTMP_RBUS_SUPPORT */
#endif /* RT3052 */

#ifdef CONFIG_AP_SUPPORT
#ifdef RT2880
	RTMP_SYS_IO_READ32(0xa030000c, &pAd->CommonCfg.CID);
#endif
#ifdef AP_SCAN_SUPPORT
	pAd->ApCfg.ACSCheckTime = 0;
#endif /* AP_SCAN_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CARRIER_DETECTION_SUPPORT
	pAd->CommonCfg.CarrierDetect.delta = CARRIER_DETECT_DELTA;
	pAd->CommonCfg.CarrierDetect.div_flag = CARRIER_DETECT_DIV_FLAG;
	pAd->CommonCfg.CarrierDetect.criteria = CARRIER_DETECT_CRITIRIA;
	pAd->CommonCfg.CarrierDetect.threshold = CARRIER_DETECT_THRESHOLD;
	pAd->CommonCfg.CarrierDetect.recheck1 = CARRIER_DETECT_RECHECK_TIME;
	pAd->CommonCfg.CarrierDetect.CarrierGoneThreshold = CARRIER_GONE_TRESHOLD;
	pAd->CommonCfg.CarrierDetect.VGA_Mask = CARRIER_DETECT_DEFAULT_MASK;
	pAd->CommonCfg.CarrierDetect.Packet_End_Mask = CARRIER_DETECT_DEFAULT_MASK;
	pAd->CommonCfg.CarrierDetect.Rx_PE_Mask = CARRIER_DETECT_DEFAULT_MASK;
#endif /* CARRIER_DETECTION_SUPPORT */

#ifdef DFS_SUPPORT
	pAd->CommonCfg.RadarDetect.bDfsInit = FALSE;
#endif /* DFS_SUPPORT */

	pAd->Dot11_H.ChMovingTime = 65;

#ifdef UAPSD_SUPPORT
#ifdef CONFIG_AP_SUPPORT
{
	UINT32 IdMbss;

	for(IdMbss=0; IdMbss<HW_BEACON_MAX_NUM; IdMbss++)
		UAPSD_INFO_INIT(&pAd->ApCfg.MBSSID[IdMbss].wdev.UapsdInfo);
}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	pAd->StaCfg.wdev.UapsdInfo.bAPSDCapable = FALSE;
#endif /* CONFIG_STA_SUPPORT */
#endif /* UAPSD_SUPPORT */
	pAd->CommonCfg.bNeedSendTriggerFrame = FALSE;
	pAd->CommonCfg.TriggerTimerCount = 0;
	pAd->CommonCfg.bAPSDForcePowerSave = FALSE;
	/*pAd->CommonCfg.bCountryFlag = FALSE;*/
	pAd->CommonCfg.TxStream = 0;
	pAd->CommonCfg.RxStream = 0;

#ifdef DOT11_N_SUPPORT
	NdisZeroMemory(&pAd->CommonCfg.HtCapability, sizeof(pAd->CommonCfg.HtCapability));
	pAd->bBroadComHT = FALSE;
	pAd->CommonCfg.bRdg = FALSE;

#ifdef DOT11N_DRAFT3
	pAd->CommonCfg.Dot11OBssScanPassiveDwell = dot11OBSSScanPassiveDwell;	/* Unit : TU. 5~1000*/
	pAd->CommonCfg.Dot11OBssScanActiveDwell = dot11OBSSScanActiveDwell;	/* Unit : TU. 10~1000*/
	pAd->CommonCfg.Dot11BssWidthTriggerScanInt = dot11BSSWidthTriggerScanInterval;	/* Unit : Second	*/
	pAd->CommonCfg.Dot11OBssScanPassiveTotalPerChannel = dot11OBSSScanPassiveTotalPerChannel;	/* Unit : TU. 200~10000*/
	pAd->CommonCfg.Dot11OBssScanActiveTotalPerChannel = dot11OBSSScanActiveTotalPerChannel;	/* Unit : TU. 20~10000*/
	pAd->CommonCfg.Dot11BssWidthChanTranDelayFactor = dot11BSSWidthChannelTransactionDelayFactor;
	pAd->CommonCfg.Dot11OBssScanActivityThre = dot11BSSScanActivityThreshold;	/* Unit : percentage*/
	pAd->CommonCfg.Dot11BssWidthChanTranDelay = (pAd->CommonCfg.Dot11BssWidthTriggerScanInt * pAd->CommonCfg.Dot11BssWidthChanTranDelayFactor);

	pAd->CommonCfg.bBssCoexEnable = TRUE; /* by default, we enable this feature, you can disable it via the profile or ioctl command*/
	pAd->CommonCfg.BssCoexApCntThr = 0;
	pAd->CommonCfg.Bss2040NeedFallBack = 0;
#endif  /* DOT11N_DRAFT3 */

	pAd->CommonCfg.bRcvBSSWidthTriggerEvents = FALSE;

	NdisZeroMemory(&pAd->CommonCfg.AddHTInfo, sizeof(pAd->CommonCfg.AddHTInfo));
	pAd->CommonCfg.BACapability.field.MMPSmode = MMPS_DISABLE;
	pAd->CommonCfg.BACapability.field.MpduDensity = 0;
	pAd->CommonCfg.BACapability.field.Policy = IMMED_BA;
	pAd->CommonCfg.BACapability.field.RxBAWinLimit = 64; /*32;*/
	pAd->CommonCfg.BACapability.field.TxBAWinLimit = 64; /*32;*/
	DBGPRINT(RT_DEBUG_TRACE, ("--> UserCfgInit. BACapability = 0x%x\n", pAd->CommonCfg.BACapability.word));

	pAd->CommonCfg.BACapability.field.AutoBA = FALSE;
	BATableInit(pAd, &pAd->BATable);

	pAd->CommonCfg.bExtChannelSwitchAnnouncement = 1;
	pAd->CommonCfg.bHTProtect = 1;
	pAd->CommonCfg.bMIMOPSEnable = TRUE;
#ifdef GREENAP_SUPPORT
	pAd->ApCfg.bGreenAPEnable=FALSE;
	pAd->ApCfg.bGreenAPActive = FALSE;
	pAd->ApCfg.GreenAPLevel= GREENAP_WITHOUT_ANY_STAS_CONNECT;
#endif /* GREENAP_SUPPORT */
	pAd->CommonCfg.bBADecline = FALSE;
	pAd->CommonCfg.bDisableReordering = FALSE;

	if (pAd->MACVersion == 0x28720200)
		pAd->CommonCfg.TxBASize = 13; /*by Jerry recommend*/
	else
		pAd->CommonCfg.TxBASize = 7;

	pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;
#endif /* DOT11_N_SUPPORT */

	pAd->CommonCfg.TxRate = RATE_6;

	pAd->CommonCfg.MlmeTransmit.field.MCS = MCS_RATE_6;
	pAd->CommonCfg.MlmeTransmit.field.BW = BW_20;
	pAd->CommonCfg.MlmeTransmit.field.MODE = MODE_OFDM;

	pAd->CommonCfg.BeaconPeriod = 100;     /* in mSec*/

#ifdef STREAM_MODE_SUPPORT
	if (pAd->chipCap.FlgHwStreamMode)
	{
		pAd->CommonCfg.StreamMode = 3;
		pAd->CommonCfg.StreamModeMCS = 0x0B0B;
		NdisMoveMemory(&pAd->CommonCfg.StreamModeMac[0][0],
				BROADCAST_ADDR, MAC_ADDR_LEN);
	}
#endif /* STREAM_MODE_SUPPORT */

#ifdef TXBF_SUPPORT
	pAd->CommonCfg.ETxBfNoncompress = 0;
	pAd->CommonCfg.ETxBfIncapable = 0;
#endif /* TXBF_SUPPORT */

#ifdef NEW_RATE_ADAPT_SUPPORT
	pAd->CommonCfg.lowTrafficThrd = 2;
	pAd->CommonCfg.TrainUpRule = 2; // 1;
	pAd->CommonCfg.TrainUpRuleRSSI = -70; // 0;
	pAd->CommonCfg.TrainUpLowThrd = 90;
	pAd->CommonCfg.TrainUpHighThrd = 110;
#endif /* NEW_RATE_ADAPT_SUPPORT */

#if defined(RT2883) || defined(RT3883)
	if (IS_RT2883(pAd) || IS_RT3883(pAd))
	{
#ifdef PRE_ANT_SWITCH
		pAd->CommonCfg.PreAntSwitch = 1;
		pAd->CommonCfg.PreAntSwitchRSSI = -76;
		pAd->CommonCfg.PreAntSwitchTimeout = 0;
#endif /* PRE_ANT_SWITCH */
		pAd->CommonCfg.PhyRateLimit = 0;
		pAd->CommonCfg.FixedRate = -1;
	}
#endif // defined (RT2883) || defined (RT3883) //


#ifdef CFO_TRACK
#ifdef RT3883
	if (IS_RT3883(pAd))
		pAd->CommonCfg.CFOTrack = 8;		// No tracking
#endif /* RT3883 */
#endif /* CFO_TRACK */

#ifdef DBG_CTRL_SUPPORT
	pAd->CommonCfg.DebugFlags = 0;
#endif /* DBG_CTRL_SUPPORT */

#ifdef WAPI_SUPPORT
	pAd->CommonCfg.wapi_usk_rekey_method = REKEY_METHOD_DISABLE;
	pAd->CommonCfg.wapi_msk_rekey_method = REKEY_METHOD_DISABLE;
	pAd->CommonCfg.wapi_msk_rekey_cnt = 0;
#endif /* WAPI_SUPPORT */

#ifdef MCAST_RATE_SPECIFIC
	pAd->CommonCfg.MCastPhyMode.word = pAd->MacTab.Content[MCAST_WCID].HTPhyMode.word;
#endif /* MCAST_RATE_SPECIFIC */

	/* WFA policy - disallow TH rate in WEP or TKIP cipher */
	pAd->CommonCfg.HT_DisallowTKIP = TRUE;

	/* Frequency for rate adaptation */
	pAd->ra_interval = DEF_RA_TIME_INTRVAL;
	pAd->ra_fast_interval = DEF_QUICK_RA_TIME_INTERVAL;

#ifdef AGS_SUPPORT
	if (pAd->rateAlg == RATE_ALG_AGS)
		pAd->ra_fast_interval = AGS_QUICK_RA_TIME_INTERVAL;
#endif /* AGS_SUPPORT */

	/* Tx Sw queue length setting */
	pAd->TxSwQMaxLen = MAX_PACKETS_IN_QUEUE;
	// TODO: shiang-usw, we may need to move this function to better place.
	rtmp_tx_swq_init(pAd);

	pAd->CommonCfg.bRalinkBurstMode = FALSE;

#ifdef CONFIG_STA_SUPPORT
	/* part II. intialize STA specific configuration*/
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_DIRECT);
		RX_FILTER_CLEAR_FLAG(pAd, fRX_FILTER_ACCEPT_MULTICAST);
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_BROADCAST);
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_ALL_MULTICAST);

		pAd->StaCfg.PwrMgmt.Psm = PWR_ACTIVE;

		pAd->StaCfg.PairCipher = Ndis802_11EncryptionDisabled;
		pAd->StaCfg.GroupCipher = Ndis802_11EncryptionDisabled;
#ifdef WPA3_SUPPORT
		pAd->StaCfg.fgIsConnInitialized = FALSE;
		pAd->MlmeAux.fgPeerAuthCommitReceived = FALSE;
		pAd->MlmeAux.fgPeerAuthConfirmReceived = FALSE;
		pAd->MlmeAux.fgOwnAuthCommitSend = FALSE;
		pAd->MlmeAux.fgOwnAuthConfirmSend = FALSE;
#endif
		pAd->StaCfg.bMixCipher = FALSE;
		pAd->StaCfg.wdev.DefaultKeyId = 0;

		/* 802.1x port control*/
		pAd->StaCfg.PrivacyFilter = Ndis802_11PrivFilter8021xWEP;
		pAd->StaCfg.wdev.PortSecured = WPA_802_1X_PORT_NOT_SECURED;
		pAd->StaCfg.LastMicErrorTime = 0;
		pAd->StaCfg.MicErrCnt        = 0;
		pAd->StaCfg.bBlockAssoc      = FALSE;
		pAd->StaCfg.WpaState         = SS_NOTUSE;

		pAd->CommonCfg.NdisRadioStateOff = FALSE;		/* New to support microsoft disable radio with OID command*/

		pAd->StaCfg.RssiTrigger = 0;
		NdisZeroMemory(&pAd->StaCfg.RssiSample, sizeof(RSSI_SAMPLE));
		pAd->StaCfg.RssiTriggerMode = RSSI_TRIGGERED_UPON_BELOW_THRESHOLD;
		pAd->StaCfg.AtimWin = 0;
		pAd->StaCfg.DefaultListenCount = 3;/*default listen count;*/
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
		pAd->StaCfg.DefaultListenCount = 1;
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */
		pAd->StaCfg.BssType = BSS_INFRA;  /* BSS_INFRA or BSS_ADHOC or BSS_MONITOR*/
		pAd->StaCfg.bSkipAutoScanConn = FALSE;
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_WAKEUP_NOW);

		pAd->StaCfg.wdev.bAutoTxRateSwitch = TRUE;
		pAd->StaCfg.wdev.DesiredTransmitSetting.field.MCS = MCS_AUTO;
		pAd->StaCfg.bAutoConnectIfNoSSID = FALSE;
#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
		pAd->StaCfg.AdaptiveFreq = TRUE; /* Todo: iwpriv and profile support. */
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */
	}

#ifdef EXT_BUILD_CHANNEL_LIST
	pAd->StaCfg.IEEE80211dClientMode = Rt802_11_D_None;
#endif /* EXT_BUILD_CHANNEL_LIST */

#ifdef RELEASE_EXCLUDE
/*
	3090F could not execute any MCU commands after executing SLEEP comands exceptiong
	RADIO_OFF and WakeUp or the chips will crash. Therefore, We use brt30xxBanMcuCmd to block MCU commands.
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RTMP_MAC_PCI
	pAd->brt30xxBanMcuCmd = FALSE;
	pAd->StaCfg.PSControl.field.EnableNewPS=FALSE;

#ifdef PCIE_PS_SUPPORT
	pAd->StaCfg.PSControl.field.EnableNewPS=TRUE;
	pAd->b3090ESpecialChip = FALSE;
	/*The value of PowerMode could be 1 or 3. Level 3 could save more power than Level 1. */
	pAd->StaCfg.PSControl.field.rt30xxPowerMode=3;
	pAd->StaCfg.PSControl.field.rt30xxForceASPMTest=0;
	pAd->StaCfg.PSControl.field.rt30xxFollowHostASPM=1;

	if (IS_SUPPORT_PCIE_PS_L3(pAd))
	{
		pAd->chipCap.HW_PCIE_PS_L3_ENABLE=TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("Support PCIe PS3 \n"));
	}
#endif /* PCIE_PS_SUPPORT */
#endif /* RTMP_MAC_PCI */
#endif /* CONFIG_STA_SUPPORT */

	/* global variables mXXXX used in MAC protocol state machines*/
	OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_ADHOC_ON);
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_INFRA_ON);

	/* PHY specification*/
	pAd->CommonCfg.PhyMode = (WMODE_B | WMODE_G);		/* default PHY mode*/
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED);  /* CCK use LONG preamble*/

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* user desired power mode*/
		pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeCAM;
		pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeCAM;
		pAd->StaCfg.bWindowsACCAMEnable = FALSE;

		pAd->StaCfg.bHwRadio = TRUE; /* Default Hardware Radio status is On*/
		pAd->StaCfg.bSwRadio = TRUE; /* Default Software Radio status is On*/
		pAd->StaCfg.bRadio = TRUE; /* bHwRadio && bSwRadio*/
		pAd->StaCfg.bHardwareRadio = FALSE;		/* Default is OFF*/
		pAd->StaCfg.bShowHiddenSSID = FALSE;		/* Default no show*/

		/* Nitro mode control*/
#if defined(NATIVE_WPA_SUPPLICANT_SUPPORT) || defined(RT_CFG80211_SUPPORT)
		pAd->StaCfg.bAutoReconnect = FALSE;
#else
		pAd->StaCfg.bAutoReconnect = TRUE;
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT || RT_CFG80211_SUPPORT*/

		/* Save the init time as last scan time, the system should do scan after 2 seconds.*/
		/* This patch is for driver wake up from standby mode, system will do scan right away.*/
		NdisGetSystemUpTime(&pAd->StaCfg.LastScanTime);
		if (pAd->StaCfg.LastScanTime > 10 * OS_HZ)
			pAd->StaCfg.LastScanTime -= (10 * OS_HZ);

		NdisZeroMemory(pAd->nickname, IW_ESSID_MAX_SIZE+1);
#ifdef PROFILE_STORE
		pAd->bWriteDat = FALSE;
#endif /* PROFILE_STORE */

#ifdef WPA_SUPPLICANT_SUPPORT
		pAd->StaCfg.wdev.IEEE8021X = FALSE;
		pAd->StaCfg.wpa_supplicant_info.IEEE8021x_required_keys = FALSE;
		pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP = WPA_SUPPLICANT_DISABLE;
		pAd->StaCfg.wpa_supplicant_info.bRSN_IE_FromWpaSupplicant = FALSE;

#if defined(NATIVE_WPA_SUPPLICANT_SUPPORT) || defined(RT_CFG80211_SUPPORT)
		pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP = WPA_SUPPLICANT_ENABLE;
#ifdef PROFILE_STORE
		pAd->bWriteDat = TRUE;
#endif /* PROFILE_STORE */
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT || RT_CFG80211_SUPPORT */

		pAd->StaCfg.wpa_supplicant_info.bLostAp = FALSE;
		pAd->StaCfg.wpa_supplicant_info.pWpsProbeReqIe = NULL;
		pAd->StaCfg.wpa_supplicant_info.WpsProbeReqIeLen = 0;
		pAd->StaCfg.wpa_supplicant_info.pWpaAssocIe = NULL;
		pAd->StaCfg.wpa_supplicant_info.WpaAssocIeLen = 0;
		pAd->StaCfg.wpa_supplicant_info.WpaSupplicantScanCount = 0;
#ifdef CFG_TDLS_SUPPORT
		NdisZeroMemory(&(pAd->StaCfg.wpa_supplicant_info.CFG_Tdls_info) , sizeof(CFG_TDLS_STRUCT));
		pAd->StaCfg.wpa_supplicant_info.CFG_Tdls_info.bCfgTDLSCapable = 1;
		pAd->StaCfg.wpa_supplicant_info.CFG_Tdls_info.TdlsChSwitchSupp = 1;
#ifdef UAPSD_SUPPORT
		cfg_tdls_TimerInit(pAd);
#endif /*UAPSD_SUPPORT*/
#endif /* CFG_TDLS_SUPPORT */
#endif /* WPA_SUPPLICANT_SUPPORT */

#ifdef WSC_STA_SUPPORT
		{
			INT					idx;
			PWSC_CTRL 			pWscControl;
#ifdef STA_EASY_CONFIG_SETUP
			PEASY_CONFIG_INFO	pEasyConfig;
#endif /* STA_EASY_CONFIG_SETUP */
#ifdef WSC_V2_SUPPORT
			PWSC_V2_INFO	pWscV2Info;
#endif /* WSC_V2_SUPPORT */

			/*
				WscControl cannot be zero here, because WscControl timers are initial in MLME Initialize
				and MLME Initialize is called before UserCfgInit.
			*/
			pWscControl = &pAd->StaCfg.WscControl;
			pWscControl->WscConfMode = WSC_DISABLE;
			pWscControl->WscMode = WSC_PIN_MODE;
			pWscControl->WscConfStatus = WSC_SCSTATE_UNCONFIGURED;
#ifdef WSC_V2_SUPPORT
			pWscControl->WscConfigMethods= 0x238C;
#else
			pWscControl->WscConfigMethods= 0x008C;
#endif /* WSC_V2_SUPPORT */
#ifdef P2P_SUPPORT
			pWscControl->WscConfigMethods |= 0x0100;
#endif /* P2P_SUPPORT */
			pWscControl->WscState = WSC_STATE_OFF;
			pWscControl->WscStatus = STATUS_WSC_NOTUSED;
			pWscControl->WscPinCode = 0;
			pWscControl->WscLastPinFromEnrollee = 0;
			pWscControl->WscEnrollee4digitPinCode = FALSE;
			pWscControl->WscEnrolleePinCode = 0;
			pWscControl->WscSelReg = 0;
			NdisZeroMemory(&pAd->StaCfg.WscControl.RegData, sizeof(WSC_REG_DATA));
			NdisZeroMemory(&pWscControl->WscProfile, sizeof(WSC_PROFILE));
			pWscControl->WscUseUPnP = 0;
			pWscControl->WscEnAssociateIE = TRUE;
			pWscControl->WscEnProbeReqIE = TRUE;
			pWscControl->RegData.ReComputePke = 1;
			pWscControl->lastId = 1;
			pWscControl->EntryIfIdx = BSS0;
			pWscControl->pAd = pAd;
#ifdef DPA_T
			pWscControl->WscDriverAutoConnect = 0x00;	// 2009-11-14 changed.
#else /* DPA_T */
			pWscControl->WscDriverAutoConnect = 0x02;
#endif /* !DPA_T */
			pAd->WriteWscCfgToDatFile = 0xFF;
			pWscControl->WscRejectSamePinFromEnrollee = FALSE;
			pWscControl->WpsApBand = PREFERRED_WPS_AP_PHY_TYPE_AUTO_SELECTION;
			pWscControl->bCheckMultiByte = FALSE;
			pWscControl->bWscAutoTigeer = FALSE;
			/* Enrollee Nonce, first generate and save to Wsc Control Block*/
			for (idx = 0; idx < 16; idx++)
			{
				pWscControl->RegData.SelfNonce[idx] = RandomByte(pAd);
			}
			pWscControl->WscRxBufLen = 0;
			pWscControl->pWscRxBuf = NULL;
			os_alloc_mem(pAd, &pWscControl->pWscRxBuf, MGMT_DMA_BUFFER_SIZE);
			if (pWscControl->pWscRxBuf)
				NdisZeroMemory(pWscControl->pWscRxBuf, MGMT_DMA_BUFFER_SIZE);
			pWscControl->WscTxBufLen = 0;
			pWscControl->pWscTxBuf = NULL;
			os_alloc_mem(pAd, &pWscControl->pWscTxBuf, MGMT_DMA_BUFFER_SIZE);
			if (pWscControl->pWscTxBuf)
				NdisZeroMemory(pWscControl->pWscTxBuf, MGMT_DMA_BUFFER_SIZE);
			pWscControl->bWscFragment = FALSE;
			pWscControl->WscFragSize = 128;
			initList(&pWscControl->WscPeerList);
			NdisAllocateSpinLock(pAd, &pWscControl->WscPeerListSemLock);
#ifdef STA_EASY_CONFIG_SETUP
			pEasyConfig = &pAd->StaCfg.EasyConfigInfo;
			AutoProvisionGenWpsPTK(pAd, BSS0);
			pEasyConfig->ModuleType = MODULE_UNKNOW;
			pEasyConfig->bRaAutoWpsAp = FALSE;
			pEasyConfig->bDoAutoWps = FALSE;
			pEasyConfig->RssiThreshold = -50;
			pEasyConfig->bEnable = TRUE;
			pEasyConfig->bChangeMode = FALSE;
#ifdef WAC_SUPPORT
			pEasyConfig->bEnableWAC = TRUE;
			pEasyConfig->DoAutoWAC = 0xFF;
			pEasyConfig->pVendorInfoForProbeReq = NULL;
#endif /* WAC_SUPPORT */
#endif /* STA_EASY_CONFIG_SETUP */

#ifdef WSC_V2_SUPPORT
			pWscV2Info = &pWscControl->WscV2Info;
			pWscV2Info->bWpsEnable = TRUE;
			pWscV2Info->ExtraTlv.TlvLen = 0;
			pWscV2Info->ExtraTlv.TlvTag = 0;
			pWscV2Info->ExtraTlv.pTlvData = NULL;
			pWscV2Info->ExtraTlv.TlvType = TLV_ASCII;
			pWscV2Info->bEnableWpsV2 = TRUE;
			pWscV2Info->bForceSetAP = FALSE;
#endif /* WSC_V2_SUPPORT */

#ifdef DPA_T
			for (idx = 0; idx < 8; idx++)
			{
				pWscControl->WpsVendorExt[idx].Length = 0;
				pWscControl->WpsVendorExt[idx].pData = NULL;
			}
			pAd->StaCfg.bPriorityCtrl = FALSE;
			pAd->StaCfg.WscPbcExtraScanCount = 0;
#endif /* DPA_T */
		}
#ifdef IWSC_SUPPORT
		IWSC_Init(pAd);
#endif /* IWSC_SUPPORT */
#endif /* WSC_STA_SUPPORT */
		NdisZeroMemory(pAd->StaCfg.ReplayCounter, 8);

#ifdef DOT11R_FT_SUPPORT
		NdisZeroMemory(&pAd->StaCfg.Dot11RCommInfo, sizeof(DOT11R_CMN_STRUC));
#endif /* DOT11R_FT_SUPPORT */

		pAd->StaCfg.bAutoConnectByBssid = FALSE;
		pAd->StaCfg.BeaconLostTime = BEACON_LOST_TIME;
		NdisZeroMemory(pAd->StaCfg.WpaPassPhrase, 64);
		pAd->StaCfg.WpaPassPhraseLen = 0;
		pAd->StaCfg.bAutoRoaming = FALSE;
		pAd->StaCfg.bForceTxBurst = FALSE;
		pAd->StaCfg.bNotFirstScan = FALSE;
		pAd->StaCfg.bImprovedScan = FALSE;
#ifdef DOT11_N_SUPPORT
		pAd->StaCfg.bAdhocN = TRUE;
#endif /* DOT11_N_SUPPORT */
		pAd->StaCfg.bFastConnect = FALSE;
		pAd->StaCfg.bAdhocCreator = FALSE;
#ifdef WIDI_SUPPORT
		pAd->StaCfg.bWIDI = TRUE;
		pAd->MlmeAux.OldChannel = 0;
#ifdef WFA_WFD_SUPPORT
		pAd->pWfdIeInBeacon = NULL;
		pAd->WfdIeInBeaconLen = 0;
		pAd->pWfdIeInProbeReq = NULL;
		pAd->WfdIeInProbeReqLen = 0;
		pAd->pWfdIeInProbeRsp = NULL;
		pAd->WfdIeInProbeRspLen = 0;
		pAd->pWfdIeInActionPkt = NULL;
		pAd->WfdIeInActionPktLen = 0;

		pAd->pWfdIeInAssocReq = NULL;
		pAd->WfdIeInAssocReqLen = 0;
		pAd->pWfdIeInAssocRsp = NULL;
		pAd->WfdIeInAssocRspLen = 0;
#endif /* WFA_WFD_SUPPORT */
#endif /* WIDI_SUPPORT */
	}
#endif /* CONFIG_STA_SUPPORT */

	/* Default for extra information is not valid*/
	pAd->ExtraInfo = EXTRA_INFO_CLEAR;

	/* Default Config change flag*/
	pAd->bConfigChanged = FALSE;

	/*
		part III. AP configurations
	*/
#ifdef CONFIG_AP_SUPPORT
#if defined(P2P_APCLI_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT) || defined(CFG80211_MULTI_STA)
#else
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* P2P_APCLI_SUPPORT || RT_CFG80211_P2P_SUPPORT */
	{
		/* Set MBSS Default Configurations*/
		pAd->ApCfg.BssidNum = MAX_MBSSID_NUM(pAd);
		for(j = BSS0; j < pAd->ApCfg.BssidNum; j++)
		{
			BSS_STRUCT *mbss = &pAd->ApCfg.MBSSID[j];
			struct wifi_dev *wdev = &pAd->ApCfg.MBSSID[j].wdev;

			wdev->AuthMode = Ndis802_11AuthModeOpen;
			wdev->WepStatus = Ndis802_11EncryptionDisabled;
			wdev->GroupKeyWepStatus = Ndis802_11EncryptionDisabled;
			wdev->DefaultKeyId = 0;
			wdev->WpaMixPairCipher = MIX_CIPHER_NOTUSE;
			mbss->RekeyCountDown = 0;	/* it's used for WPA rekey */

#ifdef SPECIFIC_TX_POWER_SUPPORT
			if (IS_RT6352(pAd))
				mbss->TxPwrAdj = -1;
#endif /* SPECIFIC_TX_POWER_SUPPORT */

#ifdef DOT1X_SUPPORT
			mbss->wdev.IEEE8021X = FALSE;
			mbss->PreAuth = FALSE;

			/* PMK cache setting*/
			mbss->PMKCachePeriod = (10 * 60 * OS_HZ); /* unit : tick(default: 10 minute)*/
			NdisZeroMemory(&mbss->PMKIDCache, sizeof(NDIS_AP_802_11_PMKID));

			/* dot1x related per BSS */
			mbss->radius_srv_num = 0;
			mbss->NasIdLen = 0;
#endif /* DOT1X_SUPPORT */

			/* VLAN related */
			mbss->wdev.VLAN_VID = 0;

			/* Default MCS as AUTO*/
			wdev->bAutoTxRateSwitch = TRUE;
			wdev->DesiredTransmitSetting.field.MCS = MCS_AUTO;

			/* Default is zero. It means no limit.*/
			mbss->MaxStaNum = 0;
			mbss->StaCount = 0;

#ifdef WSC_AP_SUPPORT
			mbss->WscSecurityMode = 0xff;
			{
				PWSC_CTRL pWscControl;
				INT idx;
#ifdef WSC_V2_SUPPORT
				PWSC_V2_INFO	pWscV2Info;
#endif /* WSC_V2_SUPPORT */
				/*
					WscControl cannot be zero here, because WscControl timers are initial in MLME Initialize
					and MLME Initialize is called before UserCfgInit.
				*/
				pWscControl = &mbss->WscControl;
				NdisZeroMemory(&pWscControl->RegData, sizeof(WSC_REG_DATA));
				NdisZeroMemory(&pAd->CommonCfg.WscStaPbcProbeInfo, sizeof(WSC_STA_PBC_PROBE_INFO));
				pWscControl->WscMode = 1;
				pWscControl->WscConfStatus = 1;
#ifdef WSC_V2_SUPPORT
				pWscControl->WscConfigMethods= 0x238C;
#else
				pWscControl->WscConfigMethods= 0x0084;
#endif /* WSC_V2_SUPPORT */
#ifdef WSC_NFC_SUPPORT
				pWscControl->WscConfigMethods |= WPS_CONFIG_METHODS_ENT;
#endif /* WSC_NFC_SUPPORT */
#ifdef P2P_SUPPORT
				pWscControl->WscConfigMethods |= 0x0108;
#endif /* P2P_SUPPORT */
				pWscControl->RegData.ReComputePke = 1;
				pWscControl->lastId = 1;
				/* pWscControl->EntryIfIdx = (MIN_NET_DEVICE_FOR_MBSSID | j); */
				pWscControl->pAd = pAd;
				pWscControl->WscRejectSamePinFromEnrollee = FALSE;
				pAd->CommonCfg.WscPBCOverlap = FALSE;
#ifdef P2P_SUPPORT
				/*
					Set defaule value of WscConfMode to be (WSC_REGISTRAR | WSC_ENROLLEE) for WiFi P2P.
				*/
				pWscControl->WscConfMode = (WSC_REGISTRAR | WSC_ENROLLEE);
#else /* P2P_SUPPORT */
				pWscControl->WscConfMode = 0;
#endif /* !P2P_SUPPORT */
				pWscControl->WscStatus = 0;
				pWscControl->WscState = 0;
				pWscControl->WscPinCode = 0;
				pWscControl->WscLastPinFromEnrollee = 0;
				pWscControl->WscEnrollee4digitPinCode = FALSE;
				pWscControl->WscEnrolleePinCode = 0;
				pWscControl->WscSelReg = 0;
				pWscControl->WscUseUPnP = 0;
				pWscControl->bWCNTest = FALSE;
				pWscControl->WscKeyASCII = 0; /* default, 0 (64 Hex) */

				/*
					Enrollee 192 random bytes for DH key generation
				*/
				for (idx = 0; idx < 192; idx++)
					pWscControl->RegData.EnrolleeRandom[idx] = RandomByte(pAd);

				/* Enrollee Nonce, first generate and save to Wsc Control Block*/
				for (idx = 0; idx < 16; idx++)
					pWscControl->RegData.SelfNonce[idx] = RandomByte(pAd);

				NdisZeroMemory(&pWscControl->WscDefaultSsid, sizeof(NDIS_802_11_SSID));
				NdisZeroMemory(&pWscControl->Wsc_Uuid_Str[0], UUID_LEN_STR);
				NdisZeroMemory(&pWscControl->Wsc_Uuid_E[0], UUID_LEN_HEX);
				pWscControl->bCheckMultiByte = FALSE;
				pWscControl->bWscAutoTigeer = FALSE;
				pWscControl->bWscFragment = FALSE;
				pWscControl->WscFragSize = 128;
				pWscControl->WscRxBufLen = 0;
				pWscControl->pWscRxBuf = NULL;
				os_alloc_mem(pAd, &pWscControl->pWscRxBuf, MGMT_DMA_BUFFER_SIZE);
				if (pWscControl->pWscRxBuf)
					NdisZeroMemory(pWscControl->pWscRxBuf, MGMT_DMA_BUFFER_SIZE);
				pWscControl->WscTxBufLen = 0;
				pWscControl->pWscTxBuf = NULL;
				os_alloc_mem(pAd, &pWscControl->pWscTxBuf, MGMT_DMA_BUFFER_SIZE);
				if (pWscControl->pWscTxBuf)
					NdisZeroMemory(pWscControl->pWscTxBuf, MGMT_DMA_BUFFER_SIZE);
				initList(&pWscControl->WscPeerList);
				NdisAllocateSpinLock(pAd, &pWscControl->WscPeerListSemLock);
				pWscControl->PinAttackCount = 0;
				pWscControl->bSetupLock = FALSE;
#ifdef WSC_V2_SUPPORT
				pWscV2Info = &pWscControl->WscV2Info;
				pWscV2Info->bWpsEnable = TRUE;
				pWscV2Info->ExtraTlv.TlvLen = 0;
				pWscV2Info->ExtraTlv.TlvTag = 0;
				pWscV2Info->ExtraTlv.pTlvData = NULL;
				pWscV2Info->ExtraTlv.TlvType = TLV_ASCII;
				pWscV2Info->bEnableWpsV2 = TRUE;
				pWscControl->SetupLockTime = WSC_WPS_AP_SETUP_LOCK_TIME;
				pWscControl->MaxPinAttack = WSC_WPS_AP_MAX_PIN_ATTACK;
#ifdef WSC_NFC_SUPPORT
				pWscControl->NfcPasswdCaculate = 2;
#endif /* WSC_NFC_SUPPORT */

#endif /* WSC_V2_SUPPORT */
			}
#endif /* WSC_AP_SUPPORT */

#ifdef EASY_CONFIG_SETUP
			AutoProvisionGenWpsPTK(pAd, j);
			mbss->EasyConfigInfo.RssiThreshold = -50;
			mbss->EasyConfigInfo.bEnable = TRUE;
			NdisZeroMemory(mbss->EasyConfigInfo.WpsPinCode, MAC_ADDR_LEN);
#endif /* EASY_CONFIG_SETUP */
#ifdef WAC_SUPPORT
			mbss->EasyConfigInfo.bEnableWAC = TRUE;
			initList(&mbss->EasyConfigInfo.WAC_PeerList);
			NdisAllocateSpinLock(pAd, &mbss->EasyConfigInfo.WAC_PeerListSemLock);
#ifdef WAC_QOS_PRIORITY
			initList(&mbss->EasyConfigInfo.WAC_ForcePriorityList);
			NdisAllocateSpinLock(pAd, &mbss->EasyConfigInfo.WAC_ForcePriorityListSemLock);
#endif /* WAC_QOS_PRIORITY */
			mbss->EasyConfigInfo.pVendorInfoForBeacon = NULL;
			mbss->EasyConfigInfo.pVendorInfoForProbeRsp = NULL;
#endif /* WAC_SUPPORT */

			for(i = 0; i < WLAN_MAX_NUM_OF_TIM; i++)
	        		mbss->TimBitmaps[i] = 0;
		}
		pAd->ApCfg.DtimCount  = 0;
		pAd->ApCfg.DtimPeriod = DEFAULT_DTIM_PERIOD;

		pAd->ApCfg.ErpIeContent = 0;

		pAd->ApCfg.StaIdleTimeout = MAC_TABLE_AGEOUT_TIME;
		pAd->ApCfg.P2pGcIdleTimeout = MAC_TABLE_MIN_AGEOUT_TIME;

#ifdef IDS_SUPPORT
		/* Default disable IDS threshold and reset all IDS counters*/
		pAd->ApCfg.IdsEnable = FALSE;
		pAd->ApCfg.AuthFloodThreshold = 0;
		pAd->ApCfg.AssocReqFloodThreshold = 0;
		pAd->ApCfg.ReassocReqFloodThreshold = 0;
		pAd->ApCfg.ProbeReqFloodThreshold = 0;
		pAd->ApCfg.DisassocFloodThreshold = 0;
		pAd->ApCfg.DeauthFloodThreshold = 0;
		pAd->ApCfg.EapReqFloodThreshold = 0;
		RTMPClearAllIdsCounter(pAd);
#endif /* IDS_SUPPORT */

#ifdef WDS_SUPPORT
		APWdsInitialize(pAd);
#endif /* WDS_SUPPORT*/

#ifdef WSC_INCLUDED
		pAd->WriteWscCfgToDatFile = 0xFF;
		pAd->WriteWscCfgToAr9DatFile = FALSE;
#ifdef CONFIG_AP_SUPPORT
#ifdef RTMP_RBUS_SUPPORT
#ifdef __ECOS
		pAd->bWscDriverAutoUpdateCfg = TRUE;
#else
		pAd->bWscDriverAutoUpdateCfg = FALSE;
#endif /* __ECOS */
#else
		pAd->bWscDriverAutoUpdateCfg = TRUE;
#endif
#endif /* CONFIG_AP_SUPPORT */
#endif /* WSC_INCLUDED */

#ifdef APCLI_SUPPORT
		pAd->ApCfg.FlgApCliIsUapsdInfoUpdated = FALSE;
		pAd->ApCfg.ApCliNum = MAX_APCLI_NUM;
		for(j = 0; j < MAX_APCLI_NUM; j++)
		{
			APCLI_STRUCT *apcli_entry = &pAd->ApCfg.ApCliTab[j];
			struct wifi_dev *wdev = &apcli_entry->wdev;

			wdev->AuthMode = Ndis802_11AuthModeOpen;
			wdev->WepStatus = Ndis802_11WEPDisabled;
			wdev->bAutoTxRateSwitch = TRUE;
			wdev->DesiredTransmitSetting.field.MCS = MCS_AUTO;
			apcli_entry->wdev.UapsdInfo.bAPSDCapable = FALSE;
#ifdef WPA3_SUPPORT
			apcli_entry->fgIsConnInitialized = FALSE;
#endif

#ifdef WPA_SUPPLICANT_SUPPORT
			apcli_entry->wdev.IEEE8021X=FALSE;
			apcli_entry->wpa_supplicant_info.IEEE8021x_required_keys=FALSE;
			apcli_entry->wpa_supplicant_info.bRSN_IE_FromWpaSupplicant=FALSE;
			apcli_entry->wpa_supplicant_info.bLostAp=FALSE;
			apcli_entry->bScanReqIsFromWebUI=FALSE;
			apcli_entry->bConfigChanged=FALSE;
			apcli_entry->wpa_supplicant_info.DesireSharedKeyId=0;
			apcli_entry->wpa_supplicant_info.WpaSupplicantUP=WPA_SUPPLICANT_DISABLE;
			apcli_entry->wpa_supplicant_info.WpaSupplicantScanCount=0;
			apcli_entry->wpa_supplicant_info.pWpsProbeReqIe=NULL;
			apcli_entry->wpa_supplicant_info.WpsProbeReqIeLen=0;
			apcli_entry->wpa_supplicant_info.pWpaAssocIe=NULL;
			apcli_entry->wpa_supplicant_info.WpaAssocIeLen=0;
			apcli_entry->SavedPMKNum=0;
			RTMPZeroMemory(apcli_entry->SavedPMK, (PMKID_NO * sizeof(BSSID_INFO)));
#endif/*WPA_SUPPLICANT_SUPPORT*/

#ifdef WPA3_SUPPORT
			apcli_entry->MlmeAux.fgPeerAuthCommitReceived = FALSE;
			apcli_entry->MlmeAux.fgPeerAuthConfirmReceived = FALSE;
			apcli_entry->MlmeAux.fgOwnAuthCommitSend = FALSE;
			apcli_entry->MlmeAux.fgOwnAuthConfirmSend = FALSE;
#endif

		}
#endif /* APCLI_SUPPORT */
		pAd->ApCfg.EntryClientCount = 0;
	}

#ifdef DYNAMIC_VGA_SUPPORT
	if (IS_MT76x2(pAd)) {
		pAd->CommonCfg.lna_vga_ctl.bDyncVgaEnable = FALSE;
		pAd->CommonCfg.lna_vga_ctl.nFalseCCATh = 800;
		pAd->CommonCfg.lna_vga_ctl.nLowFalseCCATh = 10;
	}
#endif /* DYNAMIC_VGA_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef ETH_CONVERT_SUPPORT
	if (pAd->OpMode == OPMODE_STA)
	{
		NdisZeroMemory(pAd->EthConvert.EthCloneMac, MAC_ADDR_LEN);
		pAd->EthConvert.ECMode = ETH_CONVERT_MODE_DISABLE;
		pAd->EthConvert.CloneMacVaild = FALSE;
		/*pAd->EthConvert.nodeCount = 0;*/
		NdisZeroMemory(pAd->EthConvert.SSIDStr, MAX_LEN_OF_SSID);
		pAd->EthConvert.SSIDStrLen = 0;
		pAd->EthConvert.macAutoLearn = FALSE;
		pAd->StaCfg.bFragFlag = TRUE;
	}
#endif /* ETH_CONVERT_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef IP_ASSEMBLY
	if (pAd->OpMode == OPMODE_STA)
	{
		pAd->StaCfg.bFragFlag = TRUE;
	}
#endif /* IP_ASSEMBLY */
#endif /* CONFIG_STA_SUPPORT */

	/*
		part IV. others
	*/

	/* dynamic BBP R66:sensibity tuning to overcome background noise*/
	pAd->BbpTuning.bEnable = TRUE;
	pAd->BbpTuning.FalseCcaLowerThreshold = 100;
	pAd->BbpTuning.FalseCcaUpperThreshold = 512;
	pAd->BbpTuning.R66Delta = 4;
	pAd->Mlme.bEnableAutoAntennaCheck = TRUE;

	/* Also initial R66CurrentValue, RTUSBResumeMsduTransmission might use this value.*/
	/* if not initial this value, the default value will be 0.*/
	pAd->BbpTuning.R66CurrentValue = 0x38;

#ifdef RTMP_BBP
	pAd->Bbp94 = BBPR94_DEFAULT;
#endif /* RTMP_BBP */
	pAd->BbpForCCK = FALSE;

	/* initialize MAC table and allocate spin lock*/
	NdisZeroMemory(&pAd->MacTab, sizeof(MAC_TABLE));
	InitializeQueueHeader(&pAd->MacTab.McastPsQueue);
	NdisAllocateSpinLock(pAd, &pAd->MacTabLock);

	/*RTMPInitTimer(pAd, &pAd->RECBATimer, RECBATimerTimeout, pAd, TRUE);*/
	/*RTMPSetTimer(&pAd->RECBATimer, REORDER_EXEC_INTV);*/

#ifdef MESH_SUPPORT
	pAd->MeshTab.wdev.AuthMode = Ndis802_11AuthModeOpen;
	pAd->MeshTab.wdev.WepStatus = Ndis802_11EncryptionDisabled;
	pAd->MeshTab.wdev.DefaultKeyId = 0;
#endif /* MESH_SUPPORT */

	pAd->CommonCfg.bWiFiTest = FALSE;
#ifdef RTMP_MAC_PCI
    pAd->bPCIclkOff = FALSE;
#endif /* RTMP_MAC_PCI */

#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.EntryLifeCheck = MAC_ENTRY_LIFE_CHECK_CNT;

#ifdef DOT11R_FT_SUPPORT
	FT_CfgInitial(pAd);
#endif /* DOT11R_FT_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef PCIE_PS_SUPPORT
	RTMP_SET_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP);
#endif /* PCIE_PS_SUPPORT */
#ifdef DOT11Z_TDLS_SUPPORT
		pAd->StaCfg.TdlsInfo.bTDLSCapable = FALSE;
		pAd->StaCfg.TdlsInfo.TdlsChSwitchSupp = TRUE;
		pAd->StaCfg.TdlsInfo.TdlsPsmSupp = FALSE;
		pAd->StaCfg.TdlsInfo.TdlsKeyLifeTime = TDLS_LEY_LIFETIME;
#ifdef TDLS_AUTOLINK_SUPPORT
		initList(&pAd->StaCfg.TdlsInfo.TdlsDiscovPeerList);
		NdisAllocateSpinLock(&pAd->StaCfg.TdlsInfo.TdlsDiscovPeerListSemLock);
		initList(&pAd->StaCfg.TdlsInfo.TdlsBlackList);
		NdisAllocateSpinLock(&pAd->StaCfg.TdlsInfo.TdlsBlackListSemLock);

		pAd->StaCfg.TdlsInfo.TdlsAutoSetupRssiThreshold = TDLS_AUTO_SETUP_RSSI_THRESHOLD;
		pAd->StaCfg.TdlsInfo.TdlsAutoTeardownRssiThreshold = TDLS_AUTO_TEARDOWN_RSSI_THRESHOLD;
		pAd->StaCfg.TdlsInfo.TdlsRssiMeasurementPeriod = TDLS_RSSI_MEASUREMENT_PERIOD;
		pAd->StaCfg.TdlsInfo.TdlsDisabledPeriodByTeardown = TDLS_DISABLE_PERIOD_BY_TEARDOWN;
		pAd->StaCfg.TdlsInfo.TdlsAutoDiscoveryPeriod = TDLS_AUTO_DISCOVERY_PERIOD;
#endif /* TDLS_AUTOLINK_SUPPORT */
#endif /* DOT11Z_TDLS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	pAd->RxAnt.Pair1PrimaryRxAnt = 0;
	pAd->RxAnt.Pair1SecondaryRxAnt = 1;

		pAd->RxAnt.EvaluatePeriod = 0;
		pAd->RxAnt.RcvPktNumWhenEvaluate = 0;
#ifdef CONFIG_STA_SUPPORT
		pAd->RxAnt.Pair1AvgRssi[0] = pAd->RxAnt.Pair1AvgRssi[1] = 0;
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
		pAd->RxAnt.Pair1AvgRssiGroup1[0] = pAd->RxAnt.Pair1AvgRssiGroup1[1] = 0;
		pAd->RxAnt.Pair1AvgRssiGroup2[0] = pAd->RxAnt.Pair1AvgRssiGroup2[1] = 0;
#endif /* CONFIG_AP_SUPPORT */

#ifdef ANT_DIVERSITY_SUPPORT
	pAd->CommonCfg.RxAntDiversityCfg = ANT_DIVERSITY_DEFAULT;
	pAd->CommonCfg.bSWRxAntDiversity = FALSE;
	pAd->CommonCfg.bHWRxAntDiversity = FALSE;
	pAd->CommonCfg.nAntEval_Threshold = -55; /* dBm */
	pAd->CommonCfg.nAntMiss_Threshold = 7;	/* RX antenna mismatch threshold */
	pAd->CommonCfg.nAntMiss_Cnt = 0;		/* RX antenna mismatch count */
	pAd->CommonCfg.bAntEvalEnable = FALSE;
#endif /* ANT_DIVERSITY_SUPPORT */

#ifdef TXRX_SW_ANTDIV_SUPPORT
		pAd->chipCap.bTxRxSwAntDiv = FALSE;
#endif /* TXRX_SW_ANTDIV_SUPPORT */

#if 0
#ifdef CONFIG_STA_SUPPORT
RTMP_SET_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP);
#endif /* CONFIG_STA_SUPPORT */
#endif

#if defined(AP_SCAN_SUPPORT) || defined(CONFIG_STA_SUPPORT)
	for (i = 0; i < MAX_LEN_OF_BSS_TABLE; i++)
	{
		BSS_ENTRY *pBssEntry = &pAd->ScanTab.BssEntry[i];

		if (pAd->ProbeRespIE[i].pIe)
			pBssEntry->pVarIeFromProbRsp = pAd->ProbeRespIE[i].pIe;
		else
			pBssEntry->pVarIeFromProbRsp = NULL;
	}
#endif /* defined(AP_SCAN_SUPPORT) || defined(CONFIG_STA_SUPPORT) */

#ifdef HW_COEXISTENCE_SUPPORT
	pAd->bHWCoexistenceInit = FALSE;
	pAd->bWiMaxCoexistenceOn = FALSE;
#endif /* HW_COEXISTENCE_SUPPORT */
#ifdef BT_COEXISTENCE_SUPPORT
	MiscUserCfgInit(pAd);
#endif /* BT_COEXISTENCE_SUPPORT */

#ifdef WSC_INCLUDED
	NdisZeroMemory(&pAd->CommonCfg.WscStaPbcProbeInfo, sizeof(WSC_STA_PBC_PROBE_INFO));
	pAd->CommonCfg.WscPBCOverlap = FALSE;
#endif /* WSC_INCLUDED */

#ifdef RT3593
#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
	if (IS_RT3593(pAd))
	{
		RTMP_FREQ_CAL_DISABLE(pAd); /* Off by default*/
	}
#endif /* #ifdef RTMP_FREQ_CALIBRATION_SUPPORT */
#endif /* RT3593 */

#ifdef RMTP_RBUS_SUPPORT
#ifdef VIDEO_TURBINE_SUPPORT
	VideoConfigInit(pAd);
#endif /* VIDEO_TURBINE_SUPPORT */
#endif /* RMTP_RBUS_SUPPORT */

#ifdef P2P_SUPPORT
	P2pCfgInit(pAd);
#endif /* P2P_SUPPORT */

#ifdef WFD_SUPPORT
	WfdCfgInit(pAd);
#endif /* WFD_SUPPORT */

#ifdef RT3883
	if (IS_RT3883(pAd))
		pAd->FlgCWC = 0;
#endif /* RT3883 */

#if (defined(WOW_SUPPORT) && defined(RTMP_MAC_USB)) || defined(MT_WOW_SUPPORT)
	pAd->WOW_Cfg.bEnable = FALSE;
	pAd->WOW_Cfg.bWOWFirmware = FALSE;	/* load normal firmware */
	pAd->WOW_Cfg.bInBand = TRUE;		/* use in-band signal */
	pAd->WOW_Cfg.nSelectedGPIO = 2;
	pAd->WOW_Cfg.nDelay = 3; /* (3+1)*3 = 12 sec */
	pAd->WOW_Cfg.nHoldTime = 1000;	// unit is us 
	pAd->WOW_Cfg.nWakeupInterface = WOW_WAKEUP_BY_USB; /* USB as default */
	pAd->WOW_Cfg.bGPIOHighLow = WOW_GPIO_LOW_TO_HIGH;
	pAd->WOW_Cfg.bUpdateSeqFromWoWResume = FALSE;
	NdisZeroMemory(pAd->WOW_Cfg.PTK, LEN_PTK);
    NdisZeroMemory(pAd->WOW_Cfg.ReplayCounter, LEN_KEY_DESC_REPLAY);
	//DBGPRINT(RT_DEBUG_OFF, ("WOW Enable %d, WOWFirmware %d\n", pAd->WOW_Cfg.bEnable, pAd->WOW_Cfg.bWOWFirmware));
#endif /* (defined(WOW_SUPPORT) && defined(RTMP_MAC_USB)) || defined(MT_WOW_SUPPORT) */

#ifdef RTMP_MAC_USB
	pAd->dp_ctrl.nUsb2DisconMode = 0;
#endif

	/* 802.11H and DFS related params*/
	pAd->Dot11_H.CSCount = 0;
	pAd->Dot11_H.CSPeriod = 10;

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		pAd->Dot11_H.RDMode = RD_NORMAL_MODE;
#endif

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		pAd->Dot11_H.RDMode = RD_SILENCE_MODE;
#endif

	pAd->Dot11_H.ChMovingTime = 65;
	pAd->Dot11_H.bDFSIndoor = 1;

#if defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392)
	pAd->BbpResetFlag = 0;
	pAd->BbpResetFlagCount = 0;
	pAd->BbpResetFlagCountVale = 20;
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) */

#ifdef MAC_REPEATER_SUPPORT
	for (i = 0; i < MAX_APCLI_NUM; i++)
	{
		for (j = 0; j < MAX_EXT_MAC_ADDR_SIZE; j++)
		{
			NdisZeroMemory(pAd->ApCfg.ApCliTab[i].RepeaterCli[j].OriginalAddress, MAC_ADDR_LEN);
			NdisZeroMemory(pAd->ApCfg.ApCliTab[i].RepeaterCli[j].CurrentAddress, MAC_ADDR_LEN);
			pAd->ApCfg.ApCliTab[i].RepeaterCli[j].CliConnectState = 0;
			pAd->ApCfg.ApCliTab[i].RepeaterCli[j].CliEnable= FALSE;
			pAd->ApCfg.ApCliTab[i].RepeaterCli[j].CliValid= FALSE;
			pAd->ApCfg.ApCliTab[i].RepeaterCli[j].bEthCli = FALSE;
		}
	}
	NdisAllocateSpinLock(pAd, &pAd->ApCfg.ReptCliEntryLock);
	pAd->ApCfg.RepeaterCliSize = 0;

	NdisZeroMemory(&pAd->ApCfg.ReptControl, sizeof(REPEATER_CTRL_STRUCT));
#endif /* MAC_REPEATER_SUPPORT */

#ifdef RT6352
	if (IS_RT6352(pAd)) {
		pAd->Tx0_DPD_ALC_tag0 = 0;
		pAd->Tx0_DPD_ALC_tag1 = 0;
		pAd->Tx1_DPD_ALC_tag0 = 0;
		pAd->Tx1_DPD_ALC_tag1 = 0;
		pAd->Tx0_DPD_ALC_tag0_flag = 0x0;
		pAd->Tx0_DPD_ALC_tag1_flag = 0x0;
		pAd->Tx1_DPD_ALC_tag0_flag = 0x0;
		pAd->Tx1_DPD_ALC_tag1_flag = 0x0;
	}
#endif /* RT6352 */

#ifdef APCLI_SUPPORT
#ifdef APCLI_AUTO_CONNECT_SUPPORT
	pAd->ApCfg.ApCliAutoConnectRunning= FALSE;
	pAd->ApCfg.ApCliAutoConnectChannelSwitching = FALSE;
#endif /* APCLI_AUTO_CONNECT_SUPPORT */
#endif /* APCLI_SUPPORT */

#ifdef DOT11_VHT_AC
#ifdef WFA_VHT_PF
	pAd->force_amsdu = FALSE;
	pAd->force_noack = FALSE;
	pAd->force_vht_op_mode = FALSE;
	pAd->vht_force_sgi = FALSE;
	pAd->vht_force_tx_stbc = FALSE;
	pAd->CommonCfg.vht_nss_cap = pAd->chipCap.max_nss;
	pAd->CommonCfg.vht_mcs_cap = pAd->chipCap.max_vht_mcs;
	pAd->CommonCfg.vht_cent_ch2 = 0; // we don't support 160MHz BW now!
#endif /* WFA_VHT_PF */
#endif /* DOT11_VHT_AC */

#ifdef CONFIG_FPGA_MODE
	pAd->fpga_ctl.fpga_on = 0x0;
	pAd->fpga_ctl.tx_kick_cnt = 0;
	pAd->fpga_ctl.tx_data_phy = 0;
	pAd->fpga_ctl.tx_data_ldpc = 0;
	pAd->fpga_ctl.tx_data_mcs = 0;
	pAd->fpga_ctl.tx_data_bw = 0;
	pAd->fpga_ctl.tx_data_gi = 0;
	pAd->fpga_ctl.rx_data_phy = 0;
	pAd->fpga_ctl.rx_data_ldpc = 0;
	pAd->fpga_ctl.rx_data_mcs = 0;
	pAd->fpga_ctl.rx_data_bw = 0;
	pAd->fpga_ctl.rx_data_gi = 0;
#ifdef CAPTURE_MODE
	pAd->fpga_ctl.cap_type = 2; /* CAP_MODE_ADC8; */
	pAd->fpga_ctl.cap_trigger = 2; /* CAP_TRIGGER_AUTO; */
	pAd->fpga_ctl.trigger_offset = 200;
	pAd->fpga_ctl.cap_support = 0;
#endif /* CAPTURE_MODE */

	pAd->fpga_ctl.dma_mode = DMA_SCH_LMAC;

#endif /* CONFIG_FPGA_MODE */

#ifdef MICROWAVE_OVEN_SUPPORT
	if (pAd->OpMode == OPMODE_AP)
		pAd->CommonCfg.MO_Cfg.bEnable = TRUE;
	else
		pAd->CommonCfg.MO_Cfg.bEnable = FALSE;
	pAd->CommonCfg.MO_Cfg.nFalseCCATh = MO_FALSE_CCA_TH;
#endif /* MICROWAVE_OVEN_SUPPORT */

#ifdef DYNAMIC_VGA_SUPPORT
	pAd->CommonCfg.lna_vga_ctl.bDyncVgaEnable = TRUE;
	pAd->CommonCfg.lna_vga_ctl.nFalseCCATh = 600;
	pAd->CommonCfg.lna_vga_ctl.nLowFalseCCATh = 100;
#endif /* DYNAMIC_VGA_SUPPORT */

#ifdef RT6352
	pAd->CommonCfg.bEnTemperatureTrack = FALSE;
#endif /* RT6352 */

#ifdef MT76x0
	pAd->chipCap.last_vco_temp = 0x7FFF;
	pAd->chipCap.last_full_cal_temp = 0x7FFF;
	pAd->chipCap.current_temp = 0x7FFF;
#endif /* MT76x0 */

#ifdef DOT11_VHT_AC
	pAd->CommonCfg.bNonVhtDisallow = FALSE;
#endif /* DOT11_VHT_AC */

#ifdef RTMP_USB_SUPPORT
	pAd->usb_ctl.usb_aggregation = TRUE;
#endif

#ifdef MT_MAC
    pAd->chipCap.TmrEnable = 0;

#ifdef RTMP_PCI_SUPPORT
	NdisAllocateSpinLock(pAd, &pAd->IndirectUpdateLock);
#endif

#ifdef RTMP_USB_SUPPORT
	RTMP_SEM_EVENT_INIT(&(pAd->IndirectUpdateLock), &pAd->RscSemMemList);	
#endif
	pAd->PSEWatchDogEn = 0;
	pAd->RxPseCheckTimes = 0;
	pAd->PSEResetCount = 0;
	pAd->PSETriggerType1Count = 0;
	pAd->PSETriggerType1Count = 0;
	pAd->PSEResetFailCount = 0;
#endif
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
	pAd->Mlme.bStartScc = FALSE;
#endif /*RT_CFG80211_P2P_CONCURRENT_DEVICE */

#ifdef RTMP_MAC_PCI
	pAd->PDMAWatchDogEn = 0;
	pAd->PDMAWatchDogDbg = 0;
	pAd->TxDMAResetCount = 0;
	pAd->RxDMAResetCount = 0;
	pAd->PDMAResetFailCount = 0;
	pAd->TxDMACheckTimes = 0;
	pAd->RxDMACheckTimes = 0;
	pAd->RxRest = 0;
	pAd->DropInvalidPacket = 0;
#endif

#ifdef LOAD_FW_ONE_TIME
	pAd->FWLoad = 0;
#endif /* LOAD_FW_ONE_TIME */

#ifdef SMART_CARRIER_SENSE_SUPPORT
	pAd->SCSCtrl.SCSEnable = SCS_ENABLE;
	pAd->SCSCtrl.SCSMinRssi = 0;
	pAd->SCSCtrl.SCSStatus = SCS_STATUS_DEFAULT;
	pAd->SCSCtrl.SCSTrafficThreshold = 62500; /* 500Kbps */
	pAd->SCSCtrl.CurrSensitivity = -102;
	pAd->SCSCtrl.AdjustSensitivity = -102;
	pAd->SCSCtrl.FixedRssiBond = -72;
	pAd->SCSCtrl.FalseCcaUpBond = 600;
	pAd->SCSCtrl.FalseCcaLowBond = 60;
	pAd->SCSCtrl.SCSMinRssiTolerance = 10;
	pAd->SCSCtrl.ForceMode = 0;
#endif
    pAd->bPS_Retrieve =1;

	pAd->CommonCfg.bTXRX_RXV_ON = 0;

    pAd->CommonCfg.ManualTxop = 0;

    pAd->CommonCfg.ManualTxopThreshold = 10; // Mbps

    pAd->CommonCfg.ManualTxopUpBound = 20; // Ratio

    pAd->CommonCfg.ManualTxopLowBound = 5; // Ratio

	pAd->bDisableBGProtect = TRUE;

#ifdef CONFIG_SNIFFER_SUPPORT
	pAd->monitor_ctrl.current_monitor_mode = 0;
#endif /* CONFIG_SNIFFER_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("<-- UserCfgInit\n"));
}


/* IRQL = PASSIVE_LEVEL*/
UCHAR BtoH(RTMP_STRING ch)
{
	if (ch >= '0' && ch <= '9') return (ch - '0');        /* Handle numerals*/
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  /* Handle capitol hex digits*/
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  /* Handle small hex digits*/
	return(255);
}


/*
	FUNCTION: AtoH(char *, UCHAR *, int)

	PURPOSE:  Converts ascii string to network order hex

	PARAMETERS:
		src    - pointer to input ascii string
		dest   - pointer to output hex
		destlen - size of dest

	COMMENTS:

		2 ascii bytes make a hex byte so must put 1st ascii byte of pair
		into upper nibble and 2nd ascii byte of pair into lower nibble.

	IRQL = PASSIVE_LEVEL
*/
void AtoH(RTMP_STRING *src, PUCHAR dest, int destlen)
{
	RTMP_STRING *srcptr;
	PUCHAR destTemp;

	srcptr = src;
	destTemp = (PUCHAR) dest;

	while(destlen--)
	{
		*destTemp = BtoH(*srcptr++) << 4;    /* Put 1st ascii byte in upper nibble.*/
		*destTemp += BtoH(*srcptr++);      /* Add 2nd ascii byte to above.*/
		destTemp++;
	}
}


/*
========================================================================
Routine Description:
	Add a timer to the timer list.

Arguments:
	pAd				- WLAN control block pointer
	pRsc			- the OS resource

Return Value:
	None

Note:
========================================================================
*/
VOID RTMP_TimerListAdd(RTMP_ADAPTER *pAd, VOID *pRsc)
{
	LIST_HEADER *pRscList = &pAd->RscTimerCreateList;
	LIST_RESOURCE_OBJ_ENTRY *pObj;


	/* try to find old entry */
	pObj = (LIST_RESOURCE_OBJ_ENTRY *)(pRscList->pHead);
	while(1)
	{
		if (pObj == NULL)
			break;
		if ((ULONG)(pObj->pRscObj) == (ULONG)pRsc)
			return;
		pObj = pObj->pNext;
	}

	/* allocate a timer record entry */
	os_alloc_mem(NULL, (UCHAR **)&(pObj), sizeof(LIST_RESOURCE_OBJ_ENTRY));
	if (pObj == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: alloc timer obj fail!\n", __FUNCTION__));
		return;
	}
	else
	{
		pObj->pRscObj = pRsc;
		insertTailList(pRscList, (RT_LIST_ENTRY *)pObj);
		DBGPRINT(RT_DEBUG_INFO, ("%s: add timer obj %lx!\n", __FUNCTION__, (ULONG)pRsc));
	}
}


VOID RTMP_TimerListRelease(RTMP_ADAPTER *pAd, VOID *pRsc)
{
	LIST_HEADER *pRscList = &pAd->RscTimerCreateList;
	LIST_RESOURCE_OBJ_ENTRY *pObj;
	RT_LIST_ENTRY *pListEntry;

	pListEntry = pRscList->pHead;
	pObj = (LIST_RESOURCE_OBJ_ENTRY *)pListEntry;

	while (pObj)
	{
		if ((ULONG)(pObj->pRscObj) == (ULONG)pRsc)
		{
			pListEntry = (RT_LIST_ENTRY *)pObj;
			break;
		}

		pListEntry = pListEntry->pNext;
		pObj = (LIST_RESOURCE_OBJ_ENTRY *)pListEntry;
	}

	if (pListEntry)
	{
		delEntryList(pRscList, pListEntry);

		/* free a timer record entry */
		DBGPRINT(RT_DEBUG_INFO, ("%s: release timer obj %lx!\n", __FUNCTION__, (ULONG)pRsc));
		os_free_mem(NULL, pObj);
	}
}

/*
========================================================================
Routine Description:
	Cancel all timers in the timer list.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID RTMP_AllTimerListRelease(RTMP_ADAPTER *pAd)
{
	LIST_HEADER *pRscList = &pAd->RscTimerCreateList;
	LIST_RESOURCE_OBJ_ENTRY *pObj, *pObjOld;
	BOOLEAN Cancel;

	/* try to find old entry */
	pObj = (LIST_RESOURCE_OBJ_ENTRY *)(pRscList->pHead);
	while(1)
	{
		if (pObj == NULL)
			break;
		DBGPRINT(RT_DEBUG_INFO, ("%s: Cancel timer obj %lx!\n", __FUNCTION__, (ULONG)(pObj->pRscObj)));
		pObjOld = pObj;
		pObj = pObj->pNext;
		RTMPReleaseTimer(pObjOld->pRscObj, &Cancel);
		//os_free_mem(NULL, pObjOld);
	}

	/* reset TimerList */
	initList(&pAd->RscTimerCreateList);
}


/*
	========================================================================

	Routine Description:
		Init timer objects

	Arguments:
		pAd			Pointer to our adapter
		pTimer				Timer structure
		pTimerFunc			Function to execute when timer expired
		Repeat				Ture for period timer

	Return Value:
		None

	Note:

	========================================================================
*/
VOID RTMPInitTimer(
	IN	RTMP_ADAPTER *pAd,
	IN	RALINK_TIMER_STRUCT *pTimer,
	IN	VOID *pTimerFunc,
	IN	VOID *pData,
	IN	BOOLEAN	 Repeat)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	RTMP_TimerListAdd(pAd, pTimer);


	/* Set Valid to TRUE for later used.*/
	/* It will crash if we cancel a timer or set a timer */
	/* that we haven't initialize before.*/
	/* */
	pTimer->Valid      = TRUE;

	pTimer->PeriodicType = Repeat;
	pTimer->State      = FALSE;
	pTimer->cookie = (ULONG) pData;
	pTimer->pAd = pAd;

	RTMP_OS_Init_Timer(pAd, &pTimer->TimerObj,	pTimerFunc, (PVOID) pTimer, &pAd->RscTimerMemList);
	DBGPRINT(RT_DEBUG_INFO, ("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));

	RTMP_SEM_UNLOCK(&TimerSemLock);
}


/*
	========================================================================

	Routine Description:
		Init timer objects

	Arguments:
		pTimer				Timer structure
		Value				Timer value in milliseconds

	Return Value:
		None

	Note:
		To use this routine, must call RTMPInitTimer before.

	========================================================================
*/
VOID RTMPSetTimer(RALINK_TIMER_STRUCT *pTimer, ULONG Value)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		RTMP_ADAPTER *pAd;

		pAd = (RTMP_ADAPTER *)pTimer->pAd;
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		{
			DBGPRINT_ERR(("RTMPSetTimer failed, Halt in Progress!\n"));
			RTMP_SEM_UNLOCK(&TimerSemLock);
			return;
		}

		pTimer->TimerValue = Value;
		pTimer->State      = FALSE;
		if (pTimer->PeriodicType == TRUE)
		{
			pTimer->Repeat = TRUE;
			RTMP_SetPeriodicTimer(&pTimer->TimerObj, Value);
		}
		else
		{
			pTimer->Repeat = FALSE;
			RTMP_OS_Add_Timer(&pTimer->TimerObj, Value);
		}

		DBGPRINT(RT_DEBUG_INFO,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT_ERR(("RTMPSetTimer failed, Timer hasn't been initialize!\n"));
	}
	RTMP_SEM_UNLOCK(&TimerSemLock);
}


/*
	========================================================================

	Routine Description:
		Init timer objects

	Arguments:
		pTimer				Timer structure
		Value				Timer value in milliseconds

	Return Value:
		None

	Note:
		To use this routine, must call RTMPInitTimer before.

	========================================================================
*/
VOID RTMPModTimer(RALINK_TIMER_STRUCT *pTimer, ULONG Value)
{
	BOOLEAN	Cancel;


	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		pTimer->TimerValue = Value;
		pTimer->State      = FALSE;
		if (pTimer->PeriodicType == TRUE)
		{
			RTMP_SEM_UNLOCK(&TimerSemLock);
			RTMPCancelTimer(pTimer, &Cancel);
			RTMPSetTimer(pTimer, Value);
		}
		else
		{
			RTMP_OS_Mod_Timer(&pTimer->TimerObj, Value);
			RTMP_SEM_UNLOCK(&TimerSemLock);
		}
		DBGPRINT(RT_DEBUG_INFO, ("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT_ERR(("RTMPModTimer failed, Timer hasn't been initialize!\n"));
		RTMP_SEM_UNLOCK(&TimerSemLock);
	}
}


/*
	========================================================================

	Routine Description:
		Cancel timer objects

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	Note:
		1.) To use this routine, must call RTMPInitTimer before.
		2.) Reset NIC to initial state AS IS system boot up time.

	========================================================================
*/
VOID RTMPCancelTimer(RALINK_TIMER_STRUCT *pTimer, BOOLEAN *pCancelled)
{
	// TODO: shiang-usw, check the purpose of this SemLock!
	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		if (pTimer->State == FALSE)
			pTimer->Repeat = FALSE;

		RTMP_SEM_UNLOCK(&TimerSemLock);
		RTMP_OS_Del_Timer(&pTimer->TimerObj, pCancelled);
		RTMP_SEM_LOCK(&TimerSemLock);

		if (*pCancelled == TRUE)
			pTimer->State = TRUE;

#ifdef RTMP_TIMER_TASK_SUPPORT
		/* We need to go-through the TimerQ to findout this timer handler and remove it if */
		/*		it's still waiting for execution.*/
		RtmpTimerQRemove(pTimer->pAd, pTimer);
#endif /* RTMP_TIMER_TASK_SUPPORT */

		DBGPRINT(RT_DEBUG_INFO,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT(RT_DEBUG_INFO, ("RTMPCancelTimer failed, Timer hasn't been initialize!\n"));
	}

	RTMP_SEM_UNLOCK(&TimerSemLock);
}


VOID RTMPReleaseTimer(RALINK_TIMER_STRUCT *pTimer, BOOLEAN *pCancelled)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		if (pTimer->State == FALSE)
			pTimer->Repeat = FALSE;

		RTMP_SEM_UNLOCK(&TimerSemLock);
		RTMP_OS_Del_Timer(&pTimer->TimerObj, pCancelled);
		RTMP_SEM_LOCK(&TimerSemLock);

		if (*pCancelled == TRUE)
			pTimer->State = TRUE;

#ifdef RTMP_TIMER_TASK_SUPPORT
		/* We need to go-through the TimerQ to findout this timer handler and remove it if */
		/*		it's still waiting for execution.*/
		RtmpTimerQRemove(pTimer->pAd, pTimer);
#endif /* RTMP_TIMER_TASK_SUPPORT */

		/* release timer */
		RTMP_OS_Release_Timer(&pTimer->TimerObj);

		pTimer->Valid = FALSE;
		// TODO: shiang-usw, merge this from NXTC, make sure if that's necessary here!!
		RTMP_TimerListRelease(pTimer->pAd, pTimer);

		DBGPRINT(RT_DEBUG_INFO,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT(RT_DEBUG_INFO, ("RTMPReleasefailed, Timer hasn't been initialize!\n"));
	}

	RTMP_SEM_UNLOCK(&TimerSemLock);
}


/*
	========================================================================

	Routine Description:
		Enable RX

	Arguments:
		pAd						Pointer to our adapter

	Return Value:
		None

	IRQL <= DISPATCH_LEVEL

	Note:
		Before Enable RX, make sure you have enabled Interrupt.
	========================================================================
*/
VOID RTMPEnableRxTx(RTMP_ADAPTER *pAd)
{
	DBGPRINT(RT_DEBUG_TRACE, ("==> RTMPEnableRxTx\n"));

	RT28XXDMAEnable(pAd);

	AsicSetRxFilter(pAd);

#ifdef HW_COEXISTENCE_SUPPORT
#if defined(RT35xx) || defined (RT5390)
#ifdef HW_COEXISTENCE_SUPPORT
	if (pAd->bHWCoexistenceInit&& (!IS_RT5390BC8(pAd)) && (!IS_RT3592BC8(pAd)))
#endif /* HW_COEXISTENCE_SUPPORT */
#endif /* RT35xx || RT5390 */
	{
#ifdef BT_COEXISTENCE_SUPPORT
		if(IS_ENABLE_BT_WIFI_ACTIVE_PULL_HIGH_BY_TIMER(pAd) && (pAd->BT_BC_PERMIT_RXWIFI_ACTIVE==TRUE))
		{
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x224c);
		}
		if (IS_ENABLE_WIFI_ACTIVE_PULL_LOW_BY_FORCE(pAd))
		{
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, WLAN_WIFI_ACT_PULL_LOW);
		}
		else
#endif /* BT_COEXISTENCE_SUPPORT */
		{
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, WLAN_WIFI_ACT_PULL_HIGH);
		}
	}
	else
#endif /* HW_COEXISTENCE_SUPPORT */
	{
		if (pAd->CommonCfg.bTXRX_RXV_ON)
			AsicSetMacTxRx(pAd, ASIC_MAC_TXRX_RXV, TRUE);
		else
			AsicSetMacTxRx(pAd, ASIC_MAC_TXRX, TRUE);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<== RTMPEnableRxTx\n"));
}


void CfgInitHook(RTMP_ADAPTER *pAd)
{
	/*pAd->bBroadComHT = TRUE;*/
}


static INT RtmpChipOpsRegister(RTMP_ADAPTER *pAd, INT infType)
{
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
	int ret = 0;

	NdisZeroMemory(pChipOps, sizeof(RTMP_CHIP_OP));
	NdisZeroMemory(&pAd->chipCap, sizeof(RTMP_CHIP_CAP));

	ret = RtmpChipOpsHook(pAd);

	if (ret) {
		DBGPRINT(RT_DEBUG_ERROR, ("chipOps hook error\n"));
		return ret;
	}

	/* MCU related */
	ChipOpsMCUHook(pAd, pAd->chipCap.MCUType);

	get_dev_config_idx(pAd);

	return ret;
}


#ifdef RTMP_USB_SUPPORT
BOOLEAN PairEP(RTMP_ADAPTER *pAd, UINT8 EP)
{
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;
	int i;
	int found = 0;

	if (EP == pChipCap->CommandBulkOutAddr) {
		DBGPRINT(RT_DEBUG_OFF, ("Endpoint(%x) is for In-band Command\n", EP));
		found = 1;
	}

	for (i = 0; i < 4; i++) {
		if (EP == pChipCap->WMM0ACBulkOutAddr[i]) {
			DBGPRINT(RT_DEBUG_OFF, ("Endpoint(%x) is for WMM0 AC%d\n", EP, i));
			found = 1;
		}
	}

	if (EP == pChipCap->WMM1ACBulkOutAddr) {
		DBGPRINT(RT_DEBUG_OFF, ("Endpoint(%x) is for WMM1 AC0\n", EP));
		found = 1;
	}

	if (EP == pChipCap->DataBulkInAddr) {
		DBGPRINT(RT_DEBUG_OFF, ("Endpoint(%x) is for Data-In\n", EP));
		found = 1;
	}

	if (EP == pChipCap->CommandRspBulkInAddr) {
		DBGPRINT(RT_DEBUG_OFF, ("Endpoint(%x) is for Command Rsp\n", EP));
		found = 1;
	}

	if (!found) {
		DBGPRINT(RT_DEBUG_OFF, ("Endpoint(%x) do not pair\n", EP));
		return FALSE;
	} else {
		return TRUE;
	}
}
#endif /* RTMP_USB_SUPPORT */


PNET_DEV get_netdev_from_bssid(RTMP_ADAPTER *pAd, UCHAR wdev_idx)
{
	PNET_DEV dev_p = NULL;

	if (wdev_idx < WDEV_NUM_MAX)
		dev_p = pAd->wdev_list[wdev_idx]->if_dev;

	ASSERT((dev_p != NULL));
	return dev_p;
}


INT RtmpRaDevCtrlInit(VOID *pAdSrc, RTMP_INF_TYPE infType)
{
	RTMP_ADAPTER *pAd = (PRTMP_ADAPTER)pAdSrc;
#ifdef RTMP_MAC_USB
	UINT8 i;
#endif /* RTMP_MAC_USB */

	/* Assign the interface type. We need use it when do register/EEPROM access.*/
	pAd->infType = infType;

#ifdef CONFIG_STA_SUPPORT
	pAd->OpMode = OPMODE_STA;
	DBGPRINT(RT_DEBUG_OFF, ("STA Driver version-%s\n", STA_DRIVER_VERSION));
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
	pAd->OpMode = OPMODE_AP;
	DBGPRINT(RT_DEBUG_TRACE, ("AP Driver version-%s\n", AP_DRIVER_VERSION));
#endif /* CONFIG_AP_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("pAd->infType=%d\n", pAd->infType));

#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT) || defined(CFG80211_MULTI_STA)
	pAd->OpMode = OPMODE_STA;
#endif /* P2P_SUPPORT || RT_CFG80211_P2P_SUPPORT || CFG80211_MULTI_STA */

    RTMP_SEM_EVENT_INIT(&(pAd->AutoRateLock), &pAd->RscSemMemList);
	RTMP_SEM_EVENT_INIT(&(pAd->e2p_read_lock), &pAd->RscSemMemList);

#ifdef RTMP_MAC_USB
	RTMP_SEM_EVENT_INIT(&(pAd->UsbVendorReq_semaphore), &pAd->RscSemMemList);
#ifdef RLT_MAC
	RTMP_SEM_EVENT_INIT(&(pAd->WlanEnLock), &pAd->RscSemMemList);
#endif /* RLT_MAC */
	RTMP_SEM_EVENT_INIT(&(pAd->reg_atomic), &pAd->RscSemMemList);
	RTMP_SEM_EVENT_INIT(&(pAd->hw_atomic), &pAd->RscSemMemList);
	RTMP_SEM_EVENT_INIT(&(pAd->cal_atomic), &pAd->RscSemMemList);
	RTMP_SEM_EVENT_INIT(&(pAd->tssi_lock), &pAd->RscSemMemList);

#endif /* RTMP_MAC_USB */
#ifdef MULTIPLE_CARD_SUPPORT
#ifdef RTMP_FLASH_SUPPORT
/*	if ((IS_PCIE_INF(pAd))) */
	{
		/* specific for RT6855/RT6856 */
		pAd->E2P_OFFSET_IN_FLASH[0] = 0x40000;
		pAd->E2P_OFFSET_IN_FLASH[1] = 0x48000;
	}
#endif /* RTMP_FLASH_SUPPORT */
#endif /* MULTIPLE_CARD_SUPPORT */

	if (RtmpChipOpsRegister(pAd, infType))
		return FALSE;

#ifdef RTMP_MAC_USB
	for (i = 0; i < 6; i++)
	{
		if (!PairEP(pAd, pAd->BulkOutEpAddr[i]))
			DBGPRINT(RT_DEBUG_ERROR, ("Invalid bulk out ep(%x)\n", pAd->BulkOutEpAddr[i]));
	}

	for (i = 0; i < 2; i++)
	{
		if (!PairEP(pAd, pAd->BulkInEpAddr[i]))
			DBGPRINT(RT_DEBUG_ERROR, ("Invalid bulk in ep(%x)\n", pAd->BulkInEpAddr[i]));
	}
#endif /* RTMP_MAC_USB */

#ifdef MULTIPLE_CARD_SUPPORT
{
	extern BOOLEAN RTMP_CardInfoRead(PRTMP_ADAPTER pAd);

	/* find its profile path*/
	pAd->MC_RowID = -1; /* use default profile path*/
	RTMP_CardInfoRead(pAd);

	if (pAd->MC_RowID == -1)
#ifdef CONFIG_AP_SUPPORT
		strcpy(pAd->MC_FileName, AP_PROFILE_PATH);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		strcpy(pAd->MC_FileName, STA_PROFILE_PATH);
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("MC> ROW = %d, PATH = %s\n", pAd->MC_RowID, pAd->MC_FileName));
}
#endif /* MULTIPLE_CARD_SUPPORT */

#ifdef CONFIG_CSO_SUPPORT
	if (pAd->chipCap.asic_caps & fASIC_CAP_CSO)
		RTMP_SET_MORE_FLAG(pAd, fASIC_CAP_CSO);
#endif /* CONFIG_CSO_SUPPORT */
#ifdef CONFIG_TSO_SUPPORT
	if (pAd->chipCap.asic_caps & fASIC_CAP_TSO)
		RTMP_SET_MORE_FLAG(pAd, fASIC_CAP_TSO);
#endif /* CONFIG_TSO_SUPPORT */

#ifdef MCS_LUT_SUPPORT
	if (pAd->chipCap.asic_caps & fASIC_CAP_MCS_LUT) {
		if (MAX_LEN_OF_MAC_TABLE < 128) {
			RTMP_SET_MORE_FLAG(pAd, fASIC_CAP_MCS_LUT);
		} else {
			DBGPRINT(RT_DEBUG_WARN, ("%s(): MCS_LUT not used becasue MacTb size(%d) > 128!\n",
						__FUNCTION__, MAX_LEN_OF_MAC_TABLE));
		}
	}
#endif /* MCS_LUT_SUPPORT */

#ifdef SINGLE_SKU_V2
	pAd->SKUEn = 1;
#endif /* SINGLE_SKU_V2 */

#ifdef MT_MAC
	if (pAd->chipCap.hif_type != HIF_MT) 
#endif		
	{
		if (load_patch(pAd) != NDIS_STATUS_SUCCESS) {
			DBGPRINT_ERR(("load patch failed!\n"));
			return FALSE;
		}
	}

#ifdef E2P_WITHOUT_FW_SUPPORT
	/* hook e2p operation */
    	RtmpChipOpsEepromHook(pAd, pAd->infType, E2P_NONE);
#endif /* E2P_WITHOUT_FW_SUPPORT */	
	return 0;
}


BOOLEAN RtmpRaDevCtrlExit(IN VOID *pAdSrc)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)pAdSrc;

#ifdef MULTIPLE_CARD_SUPPORT
extern UINT8  MC_CardUsed[MAX_NUM_OF_MULTIPLE_CARD];

	if ((pAd->MC_RowID >= 0) && (pAd->MC_RowID <= MAX_NUM_OF_MULTIPLE_CARD))
		MC_CardUsed[pAd->MC_RowID] = 0; /* not clear MAC address*/
#endif /* MULTIPLE_CARD_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef CREDENTIAL_STORE
		NdisFreeSpinLock(&pAd->StaCtIf.Lock);
#endif /* CREDENTIAL_STORE */
#endif /* CONFIG_STA_SUPPORT */

#ifdef RLT_MAC
	if ((IS_MT76x0(pAd) || IS_MT76x2(pAd))&& (pAd->WlanFunCtrl.field.WLAN_EN == 1))
	{
		rlt_wlan_chip_onoff(pAd, FALSE, FALSE);
	}
#endif /* RLT_MAC */

    RTMP_SEM_EVENT_DESTORY(&(pAd->AutoRateLock));
	RTMP_SEM_EVENT_DESTORY(&(pAd->e2p_read_lock));

#ifdef RTMP_MAC_USB
	RTMP_SEM_EVENT_DESTORY(&(pAd->UsbVendorReq_semaphore));
#ifdef RLT_MAC
	RTMP_SEM_EVENT_DESTORY(&(pAd->WlanEnLock));
#endif /* RLT_MAC */
	RTMP_SEM_EVENT_DESTORY(&(pAd->reg_atomic));
	RTMP_SEM_EVENT_DESTORY(&(pAd->hw_atomic));
	RTMP_SEM_EVENT_DESTORY(&(pAd->cal_atomic));
	RTMP_SEM_EVENT_DESTORY(&(pAd->tssi_lock));
#ifdef SPECIFIC_BCN_BUF_SUPPORT
	RTMP_SEM_EVENT_DESTORY(&pAd->ShrMemSemaphore);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

	if (pAd->UsbVendorReqBuf)
		os_free_mem(pAd, pAd->UsbVendorReqBuf);
#endif /* RTMP_MAC_USB */

	return TRUE;
}

VOID RTMPFreeAdapterBlock(IN VOID *pAdSrc)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)pAdSrc;
	int index;

	/*
		Free ProbeRespIE Table
	*/
	for (index = 0; index < MAX_LEN_OF_BSS_TABLE; index++)
	{
		if (pAd->ProbeRespIE[index].pIe)
			os_free_mem(pAd, pAd->ProbeRespIE[index].pIe);
	}

#ifdef RESOURCE_PRE_ALLOC
	RTMPFreeTxRxRingMemory(pAd);
#endif /* RESOURCE_PRE_ALLOC */

	RTMPFreeAdapter(pAd);
}

#ifdef CONFIG_AP_SUPPORT
#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
VOID RTMP_11N_D3_TimerInit(RTMP_ADAPTER *pAd)
{
	RTMPInitTimer(pAd, &pAd->CommonCfg.Bss2040CoexistTimer, GET_TIMER_FUNCTION(Bss2040CoexistTimeOut), pAd, FALSE);
}
#endif /* DOT11N_DRAFT3 */
#endif /* DOT11_N_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


#ifdef VENDOR_FEATURE3_SUPPORT
VOID RTMP_IO_WRITE32(
	PRTMP_ADAPTER pAd,
	UINT32 Offset,
	UINT32 Value)
{
	_RTMP_IO_WRITE32(pAd, Offset, Value);
}
#endif /* VENDOR_FEATURE3_SUPPORT */


#ifdef RTMP_MAC_PCI
VOID CMDHandler(RTMP_ADAPTER *pAd)
{
	PCmdQElmt cmdqelmt;
	UCHAR *pData;
	//NDIS_STATUS NdisStatus = NDIS_STATUS_SUCCESS;

	while (pAd && pAd->CmdQ.size > 0)
	{
		//NdisStatus = NDIS_STATUS_SUCCESS;

		NdisAcquireSpinLock(&pAd->CmdQLock);
		RTThreadDequeueCmd(&pAd->CmdQ, &cmdqelmt);
		NdisReleaseSpinLock(&pAd->CmdQLock);

		if (cmdqelmt == NULL)
			break;

		pData = cmdqelmt->buffer;
#ifdef RELEASE_EXCLUDE
		DBGPRINT_RAW(RT_DEBUG_INFO, ("Cmd = %x\n", cmdqelmt->command));
#endif /* RELEASE_EXCLUDE */

		if(!(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) || RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)))
		{
			switch (cmdqelmt->command)
			{
#ifdef CONFIG_AP_SUPPORT
				case CMDTHREAD_CHAN_RESCAN:
					DBGPRINT(RT_DEBUG_TRACE, ("cmd> Re-scan channel! \n"));

					pAd->CommonCfg.Channel = AP_AUTO_CH_SEL(pAd, ChannelAlgCCA);

#ifdef DOT11_N_SUPPORT
					/* If WMODE_CAP_N(phymode) and BW=40 check extension channel, after select channel  */
					N_ChannelCheck(pAd);
#endif /* DOT11_N_SUPPORT */

					DBGPRINT(RT_DEBUG_TRACE, ("cmd> Switch to %d! \n", pAd->CommonCfg.Channel));
					APStop(pAd);
					APStartUp(pAd);

#ifdef AP_QLOAD_SUPPORT
					QBSS_LoadAlarmResume(pAd);
#endif /* AP_QLOAD_SUPPORT */
					break;
#endif /* CONFIG_AP_SUPPORT */

				case CMDTHREAD_REG_HINT:
#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
					RT_CFG80211_CRDA_REG_HINT(pAd, pData, cmdqelmt->bufferlength);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */
					break;

				case CMDTHREAD_REG_HINT_11D:
#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
					RT_CFG80211_CRDA_REG_HINT11D(pAd, pData, cmdqelmt->bufferlength);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */
					break;

				case CMDTHREAD_SCAN_END:
#ifdef CONFIG_STA_SUPPORT					
#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
					RT_CFG80211_SCAN_END(pAd, FALSE);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */
#endif /*CONFIG_STA_SUPPORT*/
					break;

				case CMDTHREAD_CONNECT_RESULT_INFORM:
#ifdef CONFIG_STA_SUPPORT					
#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
					RT_CFG80211_CONN_RESULT_INFORM(pAd,
												pAd->MlmeAux.Bssid,
												pData, cmdqelmt->bufferlength,
												pData, cmdqelmt->bufferlength,
												1);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */
#endif /* CONFIG_STA_SUPPORT */
					break;
#ifdef MT_MAC
#ifdef MT_PS
				case CMDTHREAD_PS_CLEAR:
					{
						UINT32 wlanidx = 0;
		
						NdisMoveMemory(&wlanidx , pData, sizeof(UINT32));
						DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_PS, ("wcid=0x%x, Send Ps Clear CMD to MCU\n", wlanidx));
						CmdPsClearReq(pAd, wlanidx, TRUE);
					}
					break;

				case CMDTHREAD_PS_RETRIEVE_START:
					{
						MAC_TABLE_ENTRY *pMacEntry = (MAC_TABLE_ENTRY *)(pData);

						DBGPRINT(RT_DEBUG_INFO | DBG_FUNC_PS, ("wcid=%d CmdPsRetrieveStartReq CMD to MCU\n", pMacEntry->wcid));
						CmdPsRetrieveStartReq(pAd, pMacEntry->wcid);
					}
					break;
#endif /* MT_PS */

				case CMDTHREAD_APCLI_PBC_TIMEOUT:
					{
						UCHAR channel = 0;
						RTMP_STRING ChStr[5] = {0};
						NdisMoveMemory(&channel , pData, sizeof(UCHAR));
						DBGPRINT(RT_DEBUG_TRACE | DBG_FUNC_PS, ("channel=%d CMDTHREAD_APCLI_PBC_TIMEOUT\n", channel));
						snprintf(ChStr, sizeof(ChStr), "%d", channel);
						Set_Channel_Proc(pAd, ChStr);
					}
					break;
#endif /* MT_MAC */
                case HWCMD_ID_BMC_CNT_UPDATE:
                    {
						CHAR idx = 0;
						DBGPRINT(RT_DEBUG_INFO,
							"cmd HWCMD_ID_BMC_CNT_UPDATE \n");
						NdisMoveMemory(&idx, pData, sizeof(CHAR));

                        /* BMC start */
                        AsicSetBmcQCR(pAd, BMC_CNT_UPDATE, CR_WRITE, idx, NULL);
                    }
                    break;
					case CMDTHREAD_PERODIC_CR_ACCESS_ASIC_UPDATE_PROTECT:
					{
						MtCmdAsicUpdateProtect(pAd, cmdqelmt);
					}
					break;
#ifdef CONFIG_STA_SUPPORT	
					case CMDTHREAD_PERODIC_CR_ACCESS_MLME_DYNAMIC_TX_RATE_SWITCHING:
					{
						MtCmdMlmeDynamicTxRateSwitching(pAd, cmdqelmt);
					}
					break;
#endif /*CONFIG_STA_SUPPORT*/					

					case CMDTHREAD_PERODIC_CR_ACCESS_NIC_UPDATE_RAW_COUNTERS:
					{
						MtCmdNICUpdateRawCounters(pAd, cmdqelmt);
					}
					break;

					case CMDTHREAD_PERODIC_CR_ACCESS_WTBL_RATE_TABLE_UPDATE:
					{
						MtCmdWtbl2RateTableUpdate(pAd, cmdqelmt);
					}
					break;

#ifdef CONFIG_STA_SUPPORT
					case CMDTHREAD_FORCE_WAKE_UP:
					{
						ForceWakeUpHdlr(pAd, cmdqelmt);
					}
					break;

					case CMDTHREAD_FORCE_SLEEP_AUTO_WAKEUP:
					{
						ForceSleepAutoWakeupHdlr(pAd, cmdqelmt);
					}
					break;
#endif /* CONFIG_STA_SUPPORT */					

					case CMDTHREAD_MLME_PERIOIDC_EXEC:
					{
						MlmePeriodicExec(pAd, cmdqelmt);
					}
					break;
					
				default:
					DBGPRINT(RT_DEBUG_ERROR, ("--> Control Thread !! ERROR !! Unknown(cmdqelmt->command=0x%x) !! \n", cmdqelmt->command));
					break;
			}
		}

		if (cmdqelmt->CmdFromNdis == TRUE)
		{
			if (cmdqelmt->buffer != NULL)
				os_free_mem(pAd, cmdqelmt->buffer);
			os_free_mem(pAd, cmdqelmt);
		}
		else
		{
			if ((cmdqelmt->buffer != NULL) && (cmdqelmt->bufferlength != 0))
				os_free_mem(pAd, cmdqelmt->buffer);
			os_free_mem(pAd, cmdqelmt);
		}
	}
}
#endif /* RTMP_MAC_PCI */


VOID AntCfgInit(RTMP_ADAPTER *pAd)
{
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}


#ifdef RELEASE_EXCLUDE
	/* Because profile is read before efuse, so if profile didn't set up ant, it needs to set by efuse's setting. */
#endif

#ifdef ANT_DIVERSITY_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("%s: RxAntDiversityCfg %d\n", __FUNCTION__, pAd->CommonCfg.RxAntDiversityCfg));

	/* determine EEPORM Ant Diversity Bit */
	if ((pAd->NicConfig2.word & 0x1800) == 0x800)
	{
		pAd->CommonCfg.bSWRxAntDiversity = TRUE;
		if (pAd->chipCap.FlgIsHwAntennaDiversitySup)
			pAd->CommonCfg.bHWRxAntDiversity = TRUE;
	}
#endif	/* ANT_DIVERSITY_SUPPORT */

#ifdef TXRX_SW_ANTDIV_SUPPORT
	/* EEPROM 0x34[15:12] = 0xF is invalid, 0x2~0x3 is TX/RX SW AntDiv */
	DBGPRINT(RT_DEBUG_OFF, ("%s: bTxRxSwAntDiv %d\n", __FUNCTION__, pAd->chipCap.bTxRxSwAntDiv));
	if (pAd->chipCap.bTxRxSwAntDiv)
	{
#ifdef ANT_DIVERSITY_SUPPORT
		/* if PPAD and TXRX AntDiv are both on, only enable PPAD */
		if ((pAd->CommonCfg.bHWRxAntDiversity) && (pAd->chipCap.FlgIsHwAntennaDiversitySup))
			pAd->chipCap.bTxRxSwAntDiv = FALSE;		/* for GPIO switch */
		else
			pAd->CommonCfg.bSWRxAntDiversity = TRUE; /* for sw diversity capability */
#endif /* ANT_DIVERSITY_SUPPORT */
		DBGPRINT(RT_DEBUG_OFF, ("Antenna word %X/%d, AntDiv %d\n",
					pAd->Antenna.word, pAd->Antenna.field.BoardType, pAd->NicConfig2.field.AntDiversity));
	}
#endif /* TXRX_SW_ANTDIV_SUPPORT */

#ifdef ANT_DIVERSITY_SUPPORT
	/* Because profile read before EEPROM, so profile can not determine what kind od diversity enable, */
	/* so postpone to select SW/HW Diversity, HW has the higher priority */
	if (pAd->CommonCfg.RxAntDiversityCfg == ANT_DIVERSITY_ENABLE)
	{
		if (pAd->CommonCfg.bHWRxAntDiversity)	/* EEPROM setting */
			pAd->CommonCfg.RxAntDiversityCfg = ANT_HW_DIVERSITY_ENABLE;	/* profile, ioctl setting */
		else if (pAd->CommonCfg.bSWRxAntDiversity)	/* EEPROM setting */
			pAd->CommonCfg.RxAntDiversityCfg = ANT_SW_DIVERSITY_ENABLE;	/* profile, ioctl setting */
		else
			pAd->CommonCfg.RxAntDiversityCfg = ANT_DIVERSITY_DEFAULT; /* by EEPROM */
	}


	if (pAd->CommonCfg.RxAntDiversityCfg == ANT_DIVERSITY_DEFAULT)
#endif
	{
		if (pAd->NicConfig2.field.AntOpt== 1) /* ant selected by efuse */
		{
			if (pAd->NicConfig2.field.AntDiversity == 0) /* main */
			{
				pAd->RxAnt.Pair1PrimaryRxAnt = 0;
				pAd->RxAnt.Pair1SecondaryRxAnt = 1;
			}
			else/* aux */
			{
				pAd->RxAnt.Pair1PrimaryRxAnt = 1;
				pAd->RxAnt.Pair1SecondaryRxAnt = 0;
			}
		}
		else if (pAd->NicConfig2.field.AntDiversity == 0) /* Ant div off: default ant is main */
		{
			pAd->RxAnt.Pair1PrimaryRxAnt = 0;
			pAd->RxAnt.Pair1SecondaryRxAnt = 1;
		}
		else if (pAd->NicConfig2.field.AntDiversity == 1)/* Ant div on */
#ifdef ANT_DIVERSITY_SUPPORT
			if (pAd->chipCap.FlgIsHwAntennaDiversitySup)
				pAd->CommonCfg.RxAntDiversityCfg = ANT_HW_DIVERSITY_ENABLE;
#else
		{/* eeprom on, but sw ant div support is not enabled: default ant is main */
			pAd->RxAnt.Pair1PrimaryRxAnt = 0;
			pAd->RxAnt.Pair1SecondaryRxAnt = 1;
		}
#endif
	}

	DBGPRINT(RT_DEBUG_OFF, ("%s: primary/secondary ant %d/%d\n",
					__FUNCTION__,
					pAd->RxAnt.Pair1PrimaryRxAnt,
					pAd->RxAnt.Pair1SecondaryRxAnt));
#ifdef ANT_DIVERSITY_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("%s: RxAntDiv %d/%d\n",
					__FUNCTION__,
					pAd->CommonCfg.bSWRxAntDiversity,
					pAd->CommonCfg.bHWRxAntDiversity));
#endif
}

#ifdef RESUME_WITH_USB_RESET_SUPPORT
VOID* RTMPCheckOsCookie(VOID *handle, VOID **ppAdapter)
{
	RTMP_ADAPTER *pAd = *ppAdapter;
	VOID *pCookie = NULL;

	if (pAd == NULL){
		DBGPRINT(RT_DEBUG_OFF, ("\x1b[31m pAd is Null!!!\x1b[m\n"));
		return NULL;
		}
	if(pAd->OS_Cookie == NULL){
		DBGPRINT(RT_DEBUG_OFF, ("\x1b[31m pAd->OS_COOKIE is Null!!!\x1b[m\n"));		
		return NULL;
		}
	pCookie = pAd->OS_Cookie;
	((POS_COOKIE)pCookie)->pUsb_Dev = ((POS_COOKIE)handle)->pUsb_Dev;
#if defined(CONFIG_PM) && defined(USB_SUPPORT_SELECTIVE_SUSPEND)
	((POS_COOKIE)pCookie)->intf = ((POS_COOKIE)handle)->intf;
#endif // endif // 

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
	return pCookie;
}
#endif /* RESUME_WITH_USB_RESET_SUPPORT */
