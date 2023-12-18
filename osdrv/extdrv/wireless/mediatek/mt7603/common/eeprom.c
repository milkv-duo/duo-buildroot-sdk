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
	eeprom.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Name		Date			Modification logs
*/
#include "rt_config.h"

struct chip_map{
	UINT32 ChipVersion;
	RTMP_STRING *name;
};

#define kalGetTimeTick()                            jiffies_to_msecs(jiffies)
#define kalMemCopy(pvDst, pvSrc, u4Size)            memcpy(pvDst, pvSrc, u4Size)

struct chip_map RTMP_CHIP_E2P_FILE_TABLE[] = {
	{0x3071,	"RT3092_PCIe_LNA_2T2R_ALC_V1_2.bin"},
	{0x3090,	"RT3092_PCIe_LNA_2T2R_ALC_V1_2.bin"},
	{0x3593,	"HMC_RT3593_PCIe_3T3R_V1_3.bin"},
	{0x5392,	"RT5392_PCIe_2T2R_ALC_V1_4.bin"},
	{0x5592,	"RT5592_PCIe_2T2R_V1_7.bin"},
	{0,}
};


struct chip_map chip_card_id_map[] ={
	{7620, ""},
};


INT rtmp_read_txmixer_gain_from_eeprom(RTMP_ADAPTER *pAd)
{
	UINT16 value;

	/*
		Get TX mixer gain setting
		0xff are invalid value
		Note:
			RT30xx default value is 0x00 and will program to RF_R17
				only when this value is not zero
			RT359x default value is 0x02
	*/
	if (IS_RT30xx(pAd) || IS_RT3572(pAd)  || IS_RT3593(pAd)
		|| IS_RT5390(pAd) || IS_RT5392(pAd) || IS_RT5592(pAd)
		|| IS_RT3290(pAd) || IS_RT65XX(pAd) || IS_MT7601(pAd))
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_TXMIXER_GAIN_2_4G, value);
		pAd->TxMixerGain24G = 0;
		value &= 0x00ff;
		if (value != 0xff)
		{
			value &= 0x07;
			pAd->TxMixerGain24G = (UCHAR)value;
		}

#ifdef RT3593
		if (IS_RT3593(pAd))
		{
			pAd->TxMixerGain24G = 0;
			pAd->TxMixerGain5G = 0;
		}
#endif /* RT3593 */
	}

#ifdef RT35xx
	/* EEPROM setting of TxMixer for 3572*/
	if (IS_RT3572(pAd))
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_TXMIXER_GAIN_5G, value);
		pAd->TxMixerGain5G = 0;
		value &= 0x00ff;
		if (value != 0xff)
		{
			value &= 0x07;
			pAd->TxMixerGain5G = (UCHAR)value;
		}
	}
#endif /* RT35xx */

	return TRUE;
}


INT rtmp_read_rssi_langain_from_eeprom(RTMP_ADAPTER *pAd)
{
	INT i;
	UINT16 value;

	/* Get RSSI Offset on EEPROM 0x9Ah & 0x9Ch.*/
	/* The valid value are (-10 ~ 10) */
	/* */
#ifdef RT3593
	if (IS_RT3593(pAd))
	{
		RT3593_EEPROM_RSSI01_OFFSET_24G_READ(pAd);
	}
	else
#endif /* RT3593 */
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_BG_OFFSET, value);
		pAd->BGRssiOffset[0] = (signed char)(value & 0x00ff);
		pAd->BGRssiOffset[1] = (signed char)(value >> 8);
	}

#ifdef RT3593
	if (IS_RT3593(pAd))
	{
		RT3593_EEPROM_RSSI2_OFFSET_ALNAGAIN1_24G_READ(pAd);
	}
	else
#endif /* RT3593 */
#ifdef MT7601
	if (IS_MT7601(pAd))
	{
		; // MT7601 not support BGRssiOffset[2]
	}
	else
#endif /* MT7601 */
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_BG_OFFSET+2, value);
#ifdef MT76x0
		/*
			External LNA gain for 5GHz Band(CH100~CH128)
		*/
		if (IS_MT76x0(pAd))
		{
			pAd->ALNAGain1 = (CHAR)(value >> 8);
		}
		else
#endif /* MT76x0 */
		{
/*			if (IS_RT2860(pAd))  RT2860 supports 3 Rx and the 2.4 GHz RSSI #2 offset is in the EEPROM 0x48*/
				pAd->BGRssiOffset[2] = (signed char)(value & 0x00ff);
			pAd->ALNAGain1 = (CHAR)(value >> 8);
		}
	}

#ifdef RT3593
	if (IS_RT3593(pAd))
	{
		RT3593_EEPROM_BLNA_ALNA_GAIN0_24G_READ(pAd);
	}
	else
#endif /* RT3593 */
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_LNA_OFFSET, value);
		pAd->BLNAGain = (CHAR)(value & 0x00ff);
		/* External LNA gain for 5GHz Band(CH36~CH64) */
		pAd->ALNAGain0 = (CHAR)(value >> 8);
	}

#ifdef RT3090
#ifdef RELEASE_EXCLUDE
/*
	Because only RT3090A has internal LNA,
	and its default value of  RT3090A's internal LNA gain is 0x0A

	RT3090A's internal LNA gain is 0x0A
*/
#endif /* RELEASE_EXCLUDE */
	if (IS_RT3090A(pAd))
	{
#define RT3090A_DEFAULT_INTERNAL_LNA_GAIN	0x0A

		pAd->BLNAGain = RT3090A_DEFAULT_INTERNAL_LNA_GAIN;
	}
#endif /* RT3090 */

#ifdef RT3593
	if (IS_RT3593(pAd))
	{
		RT3593_EEPROM_RSSI01_OFFSET_5G_READ(pAd);
	}
	else
#endif /* RT3593 */
#ifdef MT7601
	if (IS_MT7601(pAd))
	{
		;	// MT7601 not support A Band
	}
	else
#endif /* MT7601 */
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_A_OFFSET, value);
		pAd->ARssiOffset[0] = (signed char)(value & 0x00ff);
		pAd->ARssiOffset[1] = (signed char)(value >> 8);
	}

#ifdef RT3593
	if (IS_RT3593(pAd))
	{
		RT3593_EEPROM_RSSI2_OFFSET_ALNAGAIN2_5G_READ(pAd);
	}
	else
#endif /* RT3593 */
#ifdef MT7601
	if (IS_MT7601(pAd))
	{
		;	// MT7601 not support A Band
	}
	else
#endif /* MT7601 */
	{
		RT28xx_EEPROM_READ16(pAd, (EEPROM_RSSI_A_OFFSET+2), value);
#ifdef MT76x0
		if (IS_MT76x0(pAd))
		{
			/* External LNA gain for 5GHz Band(CH132~CH165) */
			pAd->ALNAGain2 = (value >> 8);
		}
		else
#endif /* MT76x0 */
		{
			pAd->ARssiOffset[2] = (signed char)(value & 0x00ff);
			pAd->ALNAGain2 = (CHAR)(value >> 8);
		}
	}

#if defined(RT2883) || defined(RT3883)
	if (IS_RT2883(pAd) || IS_RT3883(pAd))
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_LNA_OFFSET2, value);
		pAd->ALNAGain1 = value & 0x00ff;
		pAd->ALNAGain2 = (value >> 8);
	}
#endif /* defined(RT2883) || defined(RT3883) */

	if (((UCHAR)pAd->ALNAGain1 == 0xFF) || (pAd->ALNAGain1 == 0x00))
		pAd->ALNAGain1 = pAd->ALNAGain0;
	if (((UCHAR)pAd->ALNAGain2 == 0xFF) || (pAd->ALNAGain2 == 0x00))
		pAd->ALNAGain2 = pAd->ALNAGain0;

	DBGPRINT(RT_DEBUG_TRACE, ("ALNAGain0 = %d, ALNAGain1 = %d, ALNAGain2 = %d\n",
					pAd->ALNAGain0, pAd->ALNAGain1, pAd->ALNAGain2));

	/* Validate 11a/b/g RSSI 0/1/2 offset.*/
	for (i =0 ; i < 3; i++)
	{
		if ((pAd->BGRssiOffset[i] < -10) || (pAd->BGRssiOffset[i] > 10))
			pAd->BGRssiOffset[i] = 0;

		if ((pAd->ARssiOffset[i] < -10) || (pAd->ARssiOffset[i] > 10))
			pAd->ARssiOffset[i] = 0;
	}

	return TRUE;
}


/*
	CountryRegion byte offset (38h)
*/
INT rtmp_read_country_region_from_eeporm(RTMP_ADAPTER *pAd)
{
	UINT16 value, value2;

#ifdef RT3883
	if (IS_RT3883(pAd))
	{
		value = pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] & 0x00FF;		// 2.4G band
		value2 = pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] >> 8;	// 5G band
	}
	else
#endif // RT3883 //
	{
		value = pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] >> 8;		/* 2.4G band*/
		value2 = pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] & 0x00FF;	/* 5G band*/
	}

	if ((value <= REGION_MAXIMUM_BG_BAND) ||
		(value == REGION_31_BG_BAND) ||
		(value == REGION_32_BG_BAND) ||
		(value == REGION_33_BG_BAND) ||
		((value >= REGION_BAND_START) && (value <= REGION_BAND_END)))
		pAd->CommonCfg.CountryRegion = ((UCHAR) value) | EEPROM_IS_PROGRAMMED;

	if ((value2 <= REGION_MAXIMUM_A_BAND) ||
		((value2 >= REGION_BAND_START) && (value2 <= REGION_BAND_END)))
		pAd->CommonCfg.CountryRegionForABand = ((UCHAR) value2) | EEPROM_IS_PROGRAMMED;

	return TRUE;
}


/*
	Read frequency offset setting from EEPROM which used for RF
*/
INT rtmp_read_freq_offset_from_eeprom(RTMP_ADAPTER *pAd)
{
	UINT16 value;

#ifdef RT3593
	if (IS_RT3593(pAd))
		RT28xx_EEPROM_READ16(pAd, EEPROM_EXT_FREQUENCY_OFFSET, value);
	else
#endif /* RT3593 */
		RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET, value);

#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		pAd->RfFreqOffset = (UCHAR)(value & 0x00FF);
	}
	else
#endif /* RT6352 */
	{
		if ((value & 0x00FF) != 0x00FF)
			pAd->RfFreqOffset = (UCHAR)(value & 0x00FF);
		else
			pAd->RfFreqOffset = 0;
	}

#ifdef MT7601
	if ( IS_MT7601(pAd) )
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET_COMPERSATION, value);
		value = (value >> 8) & 0xFF;
		if ( value != 0xFF )
		{
			if ( value & 0x80 )
				pAd->RfFreqOffset -= (value & 0x7F);
			else
				pAd->RfFreqOffset += value;
		}

	}
#endif /* MT7601 */

#ifdef RTMP_RBUS_SUPPORT
	if (pAd->infType == RTMP_DEV_INF_RBUS)
	{
		if (pAd->RfFreqDelta & 0x10)
		{
			pAd->RfFreqOffset = (pAd->RfFreqOffset >= pAd->RfFreqDelta)? (pAd->RfFreqOffset - (pAd->RfFreqDelta & 0xf)) : 0;
		}
		else
		{
#ifdef RT6352
			if (IS_RT6352(pAd))
				pAd->RfFreqOffset = ((pAd->RfFreqOffset + pAd->RfFreqDelta) < 0xFF)? (pAd->RfFreqOffset + (pAd->RfFreqDelta & 0xf)) : 0xFF;
			else
#endif /* RT6352 */
				pAd->RfFreqOffset = ((pAd->RfFreqOffset + pAd->RfFreqDelta) < 0x40)? (pAd->RfFreqOffset + (pAd->RfFreqDelta & 0xf)) : 0x3f;
		}
	}
#endif /* RTMP_RBUS_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: RF FreqOffset=0x%x \n", pAd->RfFreqOffset));

	return TRUE;
}


INT rtmp_read_txpwr_from_eeprom(RTMP_ADAPTER *pAd)
{
	/* if not return early. cause fail at emulation.*/
	/* Init the channel number for TX channel power*/
#ifdef RT3883
	if (IS_RT3883(pAd))
		RTMPRT3883ReadChannelPwr(pAd);
	else
#endif /* RT3883 */
#ifdef RT2883
	if (IS_RT2883(pAd))
		RTMPRT2883ReadChannelPwr(pAd);
	else
#endif /* RT2883 */
#ifdef RT8592
	if (IS_RT8592(pAd))
		RT85592_ReadChannelPwr(pAd);
	else
#endif /* RT8592 */
#ifdef MT76x0
	if (IS_MT76x0(pAd))
		MT76x0_ReadChannelPwr(pAd);
	else
#endif /* MT76x0 */
#ifdef MT76x2
	if (IS_MT76x2(pAd))
		mt76x2_read_chl_pwr(pAd);
	else
#endif
#ifdef MT7601
	if (IS_MT7601(pAd))
		MT7601_ReadChannelPwr(pAd);
	else
#endif /* MT7601 */
#ifdef MT7603
	if (IS_MT7603(pAd))
		mt7603_read_chl_pwr(pAd);
	else
#endif
#ifdef MT7628
	if (IS_MT7628(pAd))
		mt7628_read_chl_pwr(pAd);
	else
#endif
		RTMPReadChannelPwr(pAd);

	RTMPReadTxPwrPerRate(pAd);


#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		UINT16 value, value2;

		/* init base power by e2p target power */
		RT28xx_EEPROM_READ16(pAd, 0xD0, pAd->E2p_D0_Value);
		DBGPRINT(RT_DEBUG_ERROR, ("E2PROM: D0 target power=0x%x \n", pAd->E2p_D0_Value));

#ifdef RTMP_TEMPERATURE_CALIBRATION
		RT28xx_EEPROM_READ16(pAd, EEPROM_NIC3_OFFSET, value);
		if (value & 0x0800)
		{
			pAd->bRef25CVaild = FALSE;
		}
		else
		{
			pAd->bRef25CVaild = TRUE;
			pAd->TemperatureRef25C = (pAd->E2p_D0_Value >> 8) & 0xFF;
			DBGPRINT(RT_DEBUG_ERROR, (" pAd->TemperatureRef25C = 0x%x\n", pAd->TemperatureRef25C));
		}
#endif /* RTMP_TEMPERATURE_CALIBRATION */

		/* Get 40 MW Power Delta */
	 	RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_DELTA, value);
	 	pAd->BW_Power_Delta = 0;
	 	if ((value & 0xff) != 0xff)
	 	{
	  		if ((value2 & 0x80))
	   			pAd->BW_Power_Delta = (value2&0xf);

	 		if ((value2 & 0x40) == 0)
	   			pAd->BW_Power_Delta = -1* pAd->BW_Power_Delta;
	 	}
		DBGPRINT(RT_DEBUG_ERROR, ("E2PROM: 40 MW Power Delta= %d \n", pAd->BW_Power_Delta));

#ifdef RT6352_EP_SUPPORT
		if ((pAd->NicConfig2.word != 0) && (pAd->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET] & 0xC000))
			pAd->bExtPA = TRUE;
		else
#endif /* RT6352_EP_SUPPORT */
			pAd->bExtPA = FALSE;
	}
#endif /* RT6352 */

#ifdef SINGLE_SKU
#ifdef RT3593
	if (IS_RT3593(pAd))
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_EXT_MAX_TX_POWER_OVER_2DOT4G_AND_5G, pAd->CommonCfg.DefineMaxTxPwr);
	}
	else
#endif /* RT3593 */
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_DEFINE_MAX_TXPWR, pAd->CommonCfg.DefineMaxTxPwr);
	}

	/*
		Some dongle has old EEPROM value, use ModuleTxpower for saving correct value fo DefineMaxTxPwr.
		ModuleTxpower will override DefineMaxTxPwr (value from EEPROM) if ModuleTxpower is not zero.
	*/
	if (pAd->CommonCfg.ModuleTxpower > 0)
		pAd->CommonCfg.DefineMaxTxPwr = pAd->CommonCfg.ModuleTxpower;

	DBGPRINT(RT_DEBUG_TRACE, ("TX Power set for SINGLE SKU MODE is : 0x%04x \n", pAd->CommonCfg.DefineMaxTxPwr));

	pAd->CommonCfg.bSKUMode = FALSE;
	if ((pAd->CommonCfg.DefineMaxTxPwr & 0xFF) <= 0x50)
	{
		if (IS_RT3883(pAd))
			pAd->CommonCfg.bSKUMode = TRUE;
		else if ((pAd->CommonCfg.AntGain > 0) && (pAd->CommonCfg.BandedgeDelta >= 0))
			pAd->CommonCfg.bSKUMode = TRUE;
	}
	DBGPRINT(RT_DEBUG_TRACE, ("Single SKU Mode is %s\n",
				pAd->CommonCfg.bSKUMode ? "Enable" : "Disable"));
#endif /* SINGLE_SKU */

#ifdef SINGLE_SKU_V2
	InitSkuRateDiffTable(pAd);
#endif /* SINGLE_SKU_V2 */

	return TRUE;
}

/*
	========================================================================

	Routine Description:
		Read initial parameters from EEPROM

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
INT NICReadEEPROMParameters(RTMP_ADAPTER *pAd, RTMP_STRING *mac_addr)
{
	USHORT i, value;
	EEPROM_VERSION_STRUC Version;
	EEPROM_ANTENNA_STRUC Antenna;
	EEPROM_NIC_CONFIG2_STRUC NicConfig2;

	UINT16 Value01 = 0;
	UINT16 Value23 = 0;
	UINT16 Value45 = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("%s()-->\n", __FUNCTION__));

#ifdef RT3290
	if (IS_RT3290(pAd))
		RT3290_eeprom_access_grant(pAd, TRUE);
#endif /* RT3290 */

	if (pAd->chipOps.eeinit)
	{
#ifndef MULTIPLE_CARD_SUPPORT
		/* If we are run in Multicard mode, the eeinit shall execute in RTMP_CardInfoRead() */
		pAd->chipOps.eeinit(pAd);
#endif /* MULTIPLE_CARD_SUPPORT */

	}

	RT28xx_EEPROM_READ16(pAd, 0x4, Value01);
	RT28xx_EEPROM_READ16(pAd, 0x6, Value23);
	RT28xx_EEPROM_READ16(pAd, 0x8, Value45);

	DBGPRINT(RT_DEBUG_TRACE, ("Initialize MAC Address from EEPROM!\n"));
	pAd->PermanentAddress[0] = (UCHAR)(Value01 & 0xff);
	pAd->PermanentAddress[1] = (UCHAR)(Value01 >> 8);
	pAd->PermanentAddress[2] = (UCHAR)(Value23 & 0xff);
	pAd->PermanentAddress[3] = (UCHAR)(Value23 >> 8);
	pAd->PermanentAddress[4] = (UCHAR)(Value45 & 0xff);
	pAd->PermanentAddress[5] = (UCHAR)(Value45 >> 8);

	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM MAC: =%02x:%02x:%02x:%02x:%02x:%02x\n",
								PRINT_MAC(pAd->PermanentAddress)));

	/* Assign the actually working MAC Address */
	if (pAd->bLocalAdminMAC)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from Configuration file(.dat). \n"));
#if defined(BB_SOC)&&!defined(NEW_MBSSID_MODE)
		//BBUPrepareMAC(pAd, pAd->CurrentAddress);
		COPY_MAC_ADDR(pAd->PermanentAddress, pAd->CurrentAddress);
		DBGPRINT(RT_DEBUG_TRACE, ("now bb MainSsid mac %02x:%02x:%02x:%02x:%02x:%02x\n",
									PRINT_MAC(pAd->CurrentAddress)));
#endif
	}
	else if (mac_addr &&
			 strlen((RTMP_STRING *)mac_addr) == 17 &&
			 (strcmp(mac_addr, "00:00:00:00:00:00") != 0))
	{
		INT j;
		RTMP_STRING *macptr;

		macptr = (RTMP_STRING *) mac_addr;
		for (j=0; j<MAC_ADDR_LEN; j++)
		{
			AtoH(macptr, &pAd->CurrentAddress[j], 1);
			macptr=macptr+3;
		}

		DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from Moudle Parameter. \n"));
	}
	else
	{
		COPY_MAC_ADDR(pAd->CurrentAddress, pAd->PermanentAddress);
		DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from EEPROM. \n"));
	}

	/*Send EEprom parameter to FW*/
#ifdef CONFIG_ATE
	if (!ATE_ON(pAd))
#endif
	{
#ifdef LOAD_FW_ONE_TIME
		DBGPRINT(RT_DEBUG_ERROR,
			("@@@	NICReadEEPROMParameters : pAd->FWLoad=%u\n", pAd->FWLoad));
		if (pAd->FWLoad == 0)
#endif /* LOAD_FW_ONE_TIME */
			CmdEfusBufferModeSet(pAd);
	}

	/* if E2PROM version mismatch with driver's expectation, then skip*/
	/* all subsequent E2RPOM retieval and set a system error bit to notify GUI*/
	RT28xx_EEPROM_READ16(pAd, EEPROM_VERSION_OFFSET, Version.word);
	pAd->EepromVersion = Version.field.Version + Version.field.FaeReleaseNumber * 256;
	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: Version = %d, FAE release #%d\n", Version.field.Version, Version.field.FaeReleaseNumber));

	/* Read BBP default value from EEPROM and store to array(EEPROMDefaultValue) in pAd */
	RT28xx_EEPROM_READ16(pAd, EEPROM_NIC1_OFFSET, value);
	pAd->EEPROMDefaultValue[EEPROM_NIC_CFG1_OFFSET] = value;

	/* EEPROM offset 0x36 - NIC Configuration 1 */
	RT28xx_EEPROM_READ16(pAd, EEPROM_NIC2_OFFSET, value);
	pAd->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET] = value;
	NicConfig2.word = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET];

#if defined(BT_COEXISTENCE_SUPPORT) || defined(RT3290) || defined(RT8592)
	RT28xx_EEPROM_READ16(pAd, EEPROM_NIC3_OFFSET, value);
#ifdef RT8592
	if (value == 0xffff)
		value = 0;
#endif /* RT8592 */

	pAd->EEPROMDefaultValue[EEPROM_NIC_CFG3_OFFSET] = value;
	pAd->NicConfig3.word = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG3_OFFSET];
#endif /* defined(BT_COEXISTENCE_SUPPORT) || defined(RT3290) || defined(RT8592) */

#ifdef RT3593
	if (IS_RT3593(pAd))
	{
		RT3593_EEPROM_COUNTRY_REGION_READ(pAd);
	}
	else
#endif /* RT3593 */
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_COUNTRY_REGION, value);	/* Country Region*/
		pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] = value;
		DBGPRINT(RT_DEBUG_OFF, ("Country Region from e2p = %x\n", value));
	}

	for(i = 0; i < 8; i++)
	{
#if defined(RT65xx) || defined(MT7601) || defined(MT7603) || defined(MT7628)/* MT7650 EEPROM doesn't have those BBP setting @20121001 */
		if (IS_RT65XX(pAd) || IS_MT7601(pAd) || IS_MT7603(pAd) || IS_MT7628(pAd))
			break;
#endif /* defined(RT65xx) || defined(MT7601) || defined(MT7603) */

		RT28xx_EEPROM_READ16(pAd, EEPROM_BBP_BASE_OFFSET + i*2, value);
		pAd->EEPROMDefaultValue[i+EEPROM_BBP_ARRAY_OFFSET] = value;
	}

	/* We have to parse NIC configuration 0 at here.*/
	/* If TSSI did not have preloaded value, it should reset the TxAutoAgc to false*/
	/* Therefore, we have to read TxAutoAgc control beforehand.*/
	/* Read Tx AGC control bit*/
#ifdef MT76x0
	if (IS_MT76x0(pAd))
	{
		EEPROM_NIC_CONFIG0_STRUC NicCfg0;
		UINT32 reg_val = 0;

		RT28xx_EEPROM_READ16(pAd, 0x24, value);
		RTMP_IO_READ32(pAd, 0x0104, &reg_val);
		DBGPRINT(RT_DEBUG_WARN, ("0x24 = 0x%04x, 0x0104 = 0x%08x\n", value, reg_val));
#if 0
		if (((value & 0x00EF) == 0x00EF) && (reg_val & 0x0010))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("EEPROM shows MT7610 is FEM mode but MAC register setting is not.\n", value, reg_val));
			/*
				FEM Mode:
					0x104[15:0] = 0xEF , recommend by MT7650 Project Leader @20120917
			*/
			reg_val &= ~(0xFFFF);
			reg_val |= 0x00EF;
			RTMP_IO_WRITE32(pAd, 0x0104, reg_val);
			DBGPRINT(RT_DEBUG_TRACE, ("Set 0x104=0x%08x\n", reg_val));
		}
#endif /* 0 */
		NicCfg0.word = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG1_OFFSET];
		pAd->chipCap.PAType = NicCfg0.field.PAType;
		Antenna.word = 0;
		Antenna.field.TxPath = NicCfg0.field.TxPath;
		Antenna.field.RxPath = NicCfg0.field.RxPath;

		pAd->chipCap.ext_pa_current_setting = (pAd->EEPROMDefaultValue[EEPROM_NIC_CFG1_OFFSET] & 0x0400) ? 1:0;
		DBGPRINT(RT_DEBUG_OFF, ("ext_pa_current_setting = %d\n", pAd->chipCap.ext_pa_current_setting));

#ifdef RTMP_MAC_PCI
		if (IS_MT7610E(pAd) && (pAd->chipCap.ext_pa_current_setting == 0))
		{
			UINT32 MacReg;
			/*
				Per ACS's request, 7610E WL_GPIO2(5G_TR_SW_N) & WL_GPIO3(PA_PE_A) should output 16mA, instead of default 8mA.
				Therefore, please add the following setting in driver initial, thanks! (this is for 7610E only)
				WL_GPIO2 output 16mA: 0x11C[1:0]=0x3
				WL_GPIO3 output 16mA: 0x11C[11:10]=0x3
			*/
			RTMP_IO_READ32(pAd, 0x11C, &MacReg);
			MacReg |= 0x00000C03;
			RTMP_IO_WRITE32(pAd, 0x11C, MacReg);
			RTMP_IO_READ32(pAd, 0x11C, &MacReg);
			DBGPRINT(RT_DEBUG_OFF, ("%s(): 0x11C = 0x%x\n", __FUNCTION__, MacReg));
		}
#endif /* RTMP_MAC_PCI */

	}
	else
#endif /* MT76x0 */
		Antenna.word = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG1_OFFSET];

#ifdef MT76x2
	if (IS_MT76x2(pAd))
		mt76x2_antenna_sel_ctl(pAd);
#endif /* MT76x2 */

#ifdef RT8592
	if (IS_RT8592(pAd)) {
		DBGPRINT(RT_DEBUG_OFF, ("RT85592: EEPROM(NicConfig1=0x%04x) - Antenna.RfIcType=%d, TxPath=%d, RxPath=%d\n",
					Antenna.word, Antenna.field.RfIcType, Antenna.field.TxPath, Antenna.field.RxPath));
		// TODO: fix me!!
		Antenna.word = 0;
		Antenna.field.BoardType = 0;
		Antenna.field.RfIcType = 0xf;
		Antenna.field.TxPath = 2;
		Antenna.field.RxPath = 2;
	}
#endif /* RT8592 */

#ifdef RTMP_MAC_USB
	/* must be put here, because RTMP_CHIP_ANTENNA_INFO_DEFAULT_RESET() will clear *
	 * EPROM 0x34~3 */
#ifdef TXRX_SW_ANTDIV_SUPPORT
	/* EEPROM 0x34[15:12] = 0xF is invalid, 0x2~0x3 is TX/RX SW AntDiv */
	if (((Antenna.word & 0xFF00) != 0xFF00) && (Antenna.word & 0x2000))
	{
		pAd->chipCap.bTxRxSwAntDiv = TRUE;		/* for GPIO switch */
		DBGPRINT(RT_DEBUG_OFF, ("\x1b[mAntenna word %X/%d, AntDiv %d\x1b[m\n",
					Antenna.word, Antenna.field.BoardType, pAd->NicConfig2.field.AntDiversity));
	}
#endif /* TXRX_SW_ANTDIV_SUPPORT */
#endif /* RTMP_MAC_USB */

#ifdef RT3593
	if (IS_RT3593(pAd))
	{
		RT3593_CONFIG_SET_BY_ANTENNA(pAd);
	}
#endif /* RT3593 */

	// TODO: shiang, why we only check oxff00??
	if (((Antenna.word & 0xFF00) == 0xFF00) || IS_MT76x2(pAd))
/*	if (Antenna.word == 0xFFFF)*/
		RTMP_CHIP_ANTENNA_INFO_DEFAULT_RESET(pAd, &Antenna);

// TODO: shiang-7603
	if (IS_MT7603(pAd) || IS_MT7628(pAd))
		RTMP_CHIP_ANTENNA_INFO_DEFAULT_RESET(pAd, &Antenna);

	/* Choose the desired Tx&Rx stream.*/
	if ((pAd->CommonCfg.TxStream == 0) || (pAd->CommonCfg.TxStream > Antenna.field.TxPath))
		pAd->CommonCfg.TxStream = (UCHAR)Antenna.field.TxPath;

	if ((pAd->CommonCfg.RxStream == 0) || (pAd->CommonCfg.RxStream > Antenna.field.RxPath))
	{
		pAd->CommonCfg.RxStream = (UCHAR)Antenna.field.RxPath;

		if ((pAd->MACVersion != RALINK_3883_VERSION) &&
			(pAd->MACVersion != RALINK_2883_VERSION) &&
#ifdef RT3593
			(!RT3593_MAC_VERSION_CHECK(pAd->MACVersion)) &&
#endif /* RT3593 */
			(pAd->CommonCfg.RxStream > 2))
		{
			/* only 2 Rx streams for RT2860 series*/
			pAd->CommonCfg.RxStream = 2;
		}
	}

	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s(): AfterAdjust, RxPath = %d, TxPath = %d\n",
					__FUNCTION__, Antenna.field.RxPath, Antenna.field.TxPath));

#ifdef WSC_INCLUDED
	/* WSC hardware push button function 0811 */
	if ((pAd->MACVersion == 0x28600100) || (pAd->MACVersion == 0x28700100))
		WSC_HDR_BTN_MR_HDR_SUPPORT_SET(pAd, NicConfig2.field.EnableWPSPBC);
#endif /* WSC_INCLUDED */

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (NicConfig2.word == 0xffff)
			NicConfig2.word = 0;
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if ((NicConfig2.word & 0x00ff) == 0xff)
			NicConfig2.word &= 0xff00;

		if ((NicConfig2.word >> 8) == 0xff)
			NicConfig2.word &= 0x00ff;
	}
#endif /* CONFIG_STA_SUPPORT */

	if (NicConfig2.field.DynamicTxAgcControl == 1) {
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = TRUE;
#ifdef RT8592
		if (IS_RT8592(pAd))
		{
			pAd->chipCap.bTempCompTxALC = TRUE;
			pAd->chipCap.rx_temp_comp = pAd->NicConfig3.field.rx_temp_comp;
		}
#endif /* RT8592 */
	}
	else
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = FALSE;

	/* Save value for future using */
	pAd->NicConfig2.word = NicConfig2.word;

	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s(): RxPath = %d, TxPath = %d, RfIcType = %d\n",
					__FUNCTION__, Antenna.field.RxPath, Antenna.field.TxPath,
					Antenna.field.RfIcType));

	/* Save the antenna for future use*/
	pAd->Antenna.word = Antenna.word;

	/* Set the RfICType here, then we can initialize RFIC related operation callbacks*/
	pAd->Mlme.RealRxPath = (UCHAR) Antenna.field.RxPath;
	pAd->RfIcType = (UCHAR) Antenna.field.RfIcType;

#ifdef RT8592
	// TODO: shiang-6590, currently we don't have eeprom value, so directly force to set it as 0xff
	if (IS_RT8592(pAd)) {
		pAd->Mlme.RealRxPath = 2;
		pAd->RfIcType = RFIC_UNKNOWN;
	}
#endif /* RT8592 */

#ifdef MT76x0
	if (IS_MT7650(pAd))
		pAd->RfIcType = RFIC_7650;

	if (IS_MT7630(pAd))
		pAd->RfIcType = RFIC_7630;

	if (IS_MT7610E(pAd))
		pAd->RfIcType = RFIC_7610E;

	if (IS_MT7610U(pAd))
		pAd->RfIcType = RFIC_7610U;
#endif /* MT76x0 */

#ifdef MT76x2
	if (IS_MT7662(pAd))
		pAd->RfIcType = RFIC_7662;

	if (IS_MT7612(pAd))
		pAd->RfIcType = RFIC_7612;
#endif

#ifdef MT7603
	if (IS_MT7603(pAd))
		pAd->RfIcType = RFIC_7603;
#endif /* MT7603 */

#ifdef MT7628
	if (IS_MT7628(pAd))
		pAd->RfIcType = RFIC_7628;//7628 rf same as 7603
#endif /* MT7628 */

	pAd->phy_ctrl.rf_band_cap = NICGetBandSupported(pAd);

	/* check if the chip supports 5G band */
	if (WMODE_CAP_5G(pAd->CommonCfg.PhyMode))
	{
		if (!RFIC_IS_5G_BAND(pAd))
		{
			DBGPRINT_RAW(RT_DEBUG_ERROR,
						("%s():Err! chip not support 5G band %d!\n",
						__FUNCTION__, pAd->RfIcType));
#ifdef DOT11_N_SUPPORT
			/* change to bgn mode */
			Set_WirelessMode_Proc(pAd, "9");
#else
			/* change to bg mode */
			Set_WirelessMode_Proc(pAd, "0");
#endif /* DOT11_N_SUPPORT */
			pAd->phy_ctrl.rf_band_cap = RFIC_24GHZ;
		}
		pAd->phy_ctrl.rf_band_cap = RFIC_24GHZ | RFIC_5GHZ;
	}
	else
	{
#ifdef MT76x0
		if (IS_MT7610E(pAd))
		{
			DBGPRINT_RAW(RT_DEBUG_ERROR,
						("%s():Err! chip not support 2G band (%d)!\n",
						__FUNCTION__, pAd->RfIcType));
#ifdef DOT11_N_SUPPORT
			/* change to an mode */
			Set_WirelessMode_Proc(pAd, "8");
#else
			/* change to a mode */
			Set_WirelessMode_Proc(pAd, "2");
#endif /* DOT11_N_SUPPORT */
			pAd->phy_ctrl.rf_band_cap = RFIC_5GHZ;
		}
		else
#endif /* MT76x0 */
			pAd->phy_ctrl.rf_band_cap = RFIC_24GHZ;
	}

#ifdef MT76x0
	if (IS_MT76x0(pAd))
		mt76x0_antenna_sel_ctl(pAd);
#endif /* MT76x0 */

	LoadTssiInfoFromEEPROM(pAd);

	pAd->BbpRssiToDbmDelta = 0x0;

	rtmp_read_freq_offset_from_eeprom(pAd);

	rtmp_read_country_region_from_eeporm(pAd);

	rtmp_read_rssi_langain_from_eeprom(pAd);

	rtmp_read_txmixer_gain_from_eeprom(pAd);

#ifdef LED_CONTROL_SUPPORT
	rtmp_read_led_setting_from_eeprom(pAd);
#endif /* LED_CONTROL_SUPPORT */

	rtmp_read_txpwr_from_eeprom(pAd);

#ifdef RTMP_EFUSE_SUPPORT
	RtmpEfuseSupportCheck(pAd);
#endif /* RTMP_EFUSE_SUPPORT */

#ifdef RTMP_INTERNAL_TX_ALC
#ifdef RT65xx
	if (IS_MT76x0(pAd) || IS_MT76x2(pAd))
	{
		; // TODO: wait TC6008 EEPROM format
	}
	else
#endif /* RT65xx */
	{
		/*
		    Internal Tx ALC support is starting from RT3370 / RT3390, which combine PA / LNA in single chip.
    		The old chipset don't have this, add new feature flag RTMP_INTERNAL_TX_ALC.
 		*/
		value = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET];
		if (value == 0xFFFF) /*EEPROM is empty*/
	    		pAd->TxPowerCtrl.bInternalTxALC = FALSE;
		else if (value & 1<<13)
		    	pAd->TxPowerCtrl.bInternalTxALC = TRUE;
		else
		    	pAd->TxPowerCtrl.bInternalTxALC = FALSE;
	}
	DBGPRINT(RT_DEBUG_TRACE, ("TXALC> bInternalTxALC = %d\n", pAd->TxPowerCtrl.bInternalTxALC));
#endif /* RTMP_INTERNAL_TX_ALC */


#ifdef IQ_CAL_SUPPORT
#ifdef RT8592
	if (IS_RT65XX(pAd))
		ReadIQCompensationConfiguraiton(pAd);
	else
#endif /* RT8592 */
		GetIQCalibration(pAd);
#endif /* IQ_CAL_SUPPORT */

#ifdef MT76x0
	if (IS_MT76x0(pAd))
	{
		RT28xx_EEPROM_READ16(pAd, 0xD0, value);
		value = (value & 0xFF00) >> 8;
		DBGPRINT(RT_DEBUG_TRACE, ("%s: EEPROM_MT76x0_TEMPERATURE_OFFSET = 0x%x\n", __FUNCTION__, value));
		if ((value & 0xFF) == 0xFF)
			pAd->chipCap.temp_offset = -10;
		else {
			/* Negative number */
			if ((value & 0x80) == 0x80)
				value |= 0xFF00;

			pAd->chipCap.temp_offset = (SHORT)value;
		}
		DBGPRINT(RT_DEBUG_TRACE, ("%s: TemperatureOffset = 0x%x\n", __FUNCTION__, pAd->chipCap.temp_offset));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_A_BAND_MB, value);
		pAd->chipCap.a_band_mid_ch = value & 0x00ff;
		if (pAd->chipCap.a_band_mid_ch == 0xFF)
			pAd->chipCap.a_band_mid_ch = 100;
		pAd->chipCap.a_band_high_ch = (value >> 8);
		if (pAd->chipCap.a_band_high_ch == 0xFF)
			pAd->chipCap.a_band_high_ch = 137;
		DBGPRINT(RT_DEBUG_TRACE, ("%s: a_band_mid_ch = %d, a_band_high_ch = %d\n",
			__FUNCTION__, pAd->chipCap.a_band_mid_ch, pAd->chipCap.a_band_high_ch));

#ifdef MT76x0_TSSI_CAL_COMPENSATION
		if (pAd->chipCap.bInternalTxALC)
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_2G_TARGET_POWER, value);
			pAd->chipCap.tssi_2G_target_power = value & 0x00ff;
			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_TARGET_POWER, value);
			pAd->chipCap.tssi_5G_target_power = value & 0x00ff;
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_2G_target_power = %d, tssi_5G_target_power = %d\n",
				__FUNCTION__, pAd->chipCap.tssi_2G_target_power, pAd->chipCap.tssi_5G_target_power));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_2G_SLOPE_OFFSET, value);
			pAd->chipCap.tssi_slope_2G = value & 0x00ff;
			pAd->chipCap.tssi_offset_2G = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_2G = 0x%x, tssi_offset_2G = 0x%x\n",
				__FUNCTION__, pAd->chipCap.tssi_slope_2G, pAd->chipCap.tssi_offset_2G));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET, value);
			pAd->chipCap.tssi_slope_5G[0] = value & 0x00ff;
			pAd->chipCap.tssi_offset_5G[0] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_1 = 0x%x, tssi_offset_5G_Group_1 = 0x%x\n",
				__FUNCTION__, pAd->chipCap.tssi_slope_5G[0], pAd->chipCap.tssi_offset_5G[0]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+2, value);
			pAd->chipCap.tssi_slope_5G[1] = value & 0x00ff;
			pAd->chipCap.tssi_offset_5G[1] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_2 = 0x%x, tssi_offset_5G_Group_2 = 0x%x\n",
				__FUNCTION__, pAd->chipCap.tssi_slope_5G[1], pAd->chipCap.tssi_offset_5G[1]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+4, value);
			pAd->chipCap.tssi_slope_5G[2] = value & 0x00ff;
			pAd->chipCap.tssi_offset_5G[2] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_3 = 0x%x, tssi_offset_5G_Group_3 = 0x%x\n",
				__FUNCTION__, pAd->chipCap.tssi_slope_5G[2], pAd->chipCap.tssi_offset_5G[2]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+6, value);
			pAd->chipCap.tssi_slope_5G[3] = value & 0x00ff;
			pAd->chipCap.tssi_offset_5G[3] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_4 = 0x%x, tssi_offset_5G_Group_4 = 0x%x\n",
				__FUNCTION__, pAd->chipCap.tssi_slope_5G[3], pAd->chipCap.tssi_offset_5G[3]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+8, value);
			pAd->chipCap.tssi_slope_5G[4] = value & 0x00ff;
			pAd->chipCap.tssi_offset_5G[4] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_5 = 0x%x, tssi_offset_5G_Group_5 = 0x%x\n",
				__FUNCTION__, pAd->chipCap.tssi_slope_5G[4], pAd->chipCap.tssi_offset_5G[4]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+10, value);
			pAd->chipCap.tssi_slope_5G[5] = value & 0x00ff;
			pAd->chipCap.tssi_offset_5G[5] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_6 = 0x%x, tssi_offset_5G_Group_6 = 0x%x\n",
				__FUNCTION__, pAd->chipCap.tssi_slope_5G[5], pAd->chipCap.tssi_offset_5G[5]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+12, value);
			pAd->chipCap.tssi_slope_5G[6] = value & 0x00ff;
			pAd->chipCap.tssi_offset_5G[6] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_7 = 0x%x, tssi_offset_5G_Group_7 = 0x%x\n",
				__FUNCTION__, pAd->chipCap.tssi_slope_5G[6], pAd->chipCap.tssi_offset_5G[6]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+14, value);
			pAd->chipCap.tssi_slope_5G[7] = value & 0x00ff;
			pAd->chipCap.tssi_offset_5G[7] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_8 = 0x%x, tssi_offset_5G_Group_8 = 0x%x\n",
				__FUNCTION__, pAd->chipCap.tssi_slope_5G[7], pAd->chipCap.tssi_offset_5G[7]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_CHANNEL_BOUNDARY, value);
			pAd->chipCap.tssi_5G_channel_boundary[0] = value & 0x00ff;
			pAd->chipCap.tssi_5G_channel_boundary[1] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_5G_channel_boundary_1 = %d, tssi_5G_channel_boundary_2 = %d\n",
				__FUNCTION__, pAd->chipCap.tssi_5G_channel_boundary[0], pAd->chipCap.tssi_5G_channel_boundary[1]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_CHANNEL_BOUNDARY+2, value);
			pAd->chipCap.tssi_5G_channel_boundary[2] = value & 0x00ff;
			pAd->chipCap.tssi_5G_channel_boundary[3] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_5G_channel_boundary_3 = %d, tssi_5G_channel_boundary_4 = %d\n",
				__FUNCTION__, pAd->chipCap.tssi_5G_channel_boundary[2], pAd->chipCap.tssi_5G_channel_boundary[3]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_CHANNEL_BOUNDARY+4, value);
			pAd->chipCap.tssi_5G_channel_boundary[4] = value & 0x00ff;
			pAd->chipCap.tssi_5G_channel_boundary[5] = (value >> 8);
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_5G_channel_boundary_5 = %d, tssi_5G_channel_boundary_6 = %d\n",
				__FUNCTION__, pAd->chipCap.tssi_5G_channel_boundary[4], pAd->chipCap.tssi_5G_channel_boundary[5]));

			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_CHANNEL_BOUNDARY+6, value);
			pAd->chipCap.tssi_5G_channel_boundary[6] = value & 0x00ff;
			DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_5G_channel_boundary_7 = %d\n",
				__FUNCTION__, pAd->chipCap.tssi_5G_channel_boundary[6]));
		}
#endif /* MT76x0_TSSI_CAL_COMPENSATION */
	}
#endif /* MT76x0 */


#ifdef RT3290
	if (IS_RT3290(pAd))
		RT3290_eeprom_access_grant(pAd, FALSE);
#endif /* RT3290 */

	DBGPRINT(RT_DEBUG_TRACE, ("%s: pAd->Antenna.field.BoardType = %d, IS_MINI_CARD(pAd) = %d, IS_RT5390U(pAd) = %d\n",
		__FUNCTION__,
		pAd->Antenna.field.BoardType,
		IS_MINI_CARD(pAd),
		IS_RT5390U(pAd)));

	DBGPRINT(RT_DEBUG_TRACE, ("<--%s()\n", __FUNCTION__));

	return TRUE;
}


void rtmp_eeprom_of_platform(RTMP_ADAPTER *pAd)
{
#if defined(CONFIG_RT_FIRST_CARD_EEPROM) || defined(CONFIG_RT_SECOND_CARD_EEPROM)
	UCHAR e2p_dafault = 0;
#endif /* defined(CONFIG_RT_FIRST_CARD_EEPROM) || defined(CONFIG_RT_SECOND_CARD_EEPROM) */
#ifdef CONFIG_RT_FIRST_CARD_EEPROM
	if ( pAd->dev_idx == 0 )
	{
		if ( RTMPEqualMemory("efuse", CONFIG_RT_FIRST_CARD_EEPROM, 5) )
			e2p_dafault = E2P_EFUSE_MODE;
		if ( RTMPEqualMemory("prom", CONFIG_RT_FIRST_CARD_EEPROM, 4) )
			e2p_dafault = E2P_EEPROM_MODE;
		if ( RTMPEqualMemory("flash", CONFIG_RT_FIRST_CARD_EEPROM, 5) )
			e2p_dafault = E2P_FLASH_MODE;

		pAd->E2pAccessMode = e2p_dafault;
	}
#endif /* CONFIG_RT_FIRST_CARD_EEPROM */

#ifdef CONFIG_RT_SECOND_CARD_EEPROM
	if ( pAd->dev_idx == 1 )
	{
		if ( RTMPEqualMemory("efuse", CONFIG_RT_SECOND_CARD_EEPROM, 5) )
			e2p_dafault = E2P_EFUSE_MODE;
		if ( RTMPEqualMemory("prom", CONFIG_RT_SECOND_CARD_EEPROM, 4) )
			e2p_dafault = E2P_EEPROM_MODE;
		if ( RTMPEqualMemory("flash", CONFIG_RT_SECOND_CARD_EEPROM, 5) )
			e2p_dafault = E2P_FLASH_MODE;

		pAd->E2pAccessMode = e2p_dafault;
	}
#endif /* CONFIG_RT_SECOND_CARD_EEPROM */
}

UCHAR RtmpEepromGetDefault(RTMP_ADAPTER *pAd)
{
	UCHAR e2p_dafault = 0;

#ifdef RTMP_FLASH_SUPPORT
	if (pAd->infType == RTMP_DEV_INF_RBUS)
		e2p_dafault = E2P_FLASH_MODE;
	else
#endif /* RTMP_FLASH_SUPPORT */
	{
#ifdef RTMP_EFUSE_SUPPORT
		if (pAd->bUseEfuse)
			e2p_dafault = E2P_EFUSE_MODE;
		else
#endif /* RTMP_EFUSE_SUPPORT */
			e2p_dafault = E2P_EEPROM_MODE;
	}

	DBGPRINT(RT_DEBUG_OFF, ("%s::e2p_dafault=%d\n", __FUNCTION__, e2p_dafault));
	return e2p_dafault;
}

#if 0
INT RtmpChipOpsEepromHook(RTMP_ADAPTER *pAd, INT iface_type)
{
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;

//+++Test code
	if (IS_RT6352(pAd))
		goto flash;

	if (IS_MT76x0(pAd) || IS_MT76x2(pAd))
		goto efuse;
//---Test code

flash:
#ifdef RTMP_FLASH_SUPPORT
#if defined(MT76x0) || defined(MT76x2)
	/*
		MT7610E alrady do rtmp_nv_init in MT76x0_WLAN_ChipOnOff.
	*/
	if (IS_MT76x0E(pAd) || IS_MT76x2E(pAd))
		pChipOps->eeinit = NULL;
	else
#endif /* defined(MT76x0) || defined(MT76x2) */

	pChipOps->eeinit = rtmp_nv_init;
	pChipOps->eeread = rtmp_ee_flash_read;
	pChipOps->eewrite = rtmp_ee_flash_write;
	pAd->flash_offset = DEFAULT_RF_OFFSET;

	return 0;
#endif /* RTMP_FLASH_SUPPORT */

efuse:
#ifdef RTMP_EFUSE_SUPPORT
	efuse_probe(pAd);
	if(pAd->bUseEfuse)
	{
		pChipOps->eeinit = eFuse_init;
		pChipOps->eeread = rtmp_ee_efuse_read16;
		pChipOps->eewrite = rtmp_ee_efuse_write16;
		DBGPRINT(RT_DEBUG_OFF, ("NVM is EFUSE\n"));
		DBGPRINT(RT_DEBUG_TRACE, ("Efuse Size=0x%x [Range:%x-%x] \n",
				pAd->chipCap.EFUSE_USAGE_MAP_SIZE,
				pAd->chipCap.EFUSE_USAGE_MAP_START,
				pAd->chipCap.EFUSE_USAGE_MAP_END));

		return 0 ;
	}
	else
	{
		pAd->bFroceEEPROMBuffer = FALSE;
		DBGPRINT(RT_DEBUG_OFF, ("NVM is EEPROM\n"));
	}
#endif /* RTMP_EFUSE_SUPPORT */

	switch(iface_type)
	{
#ifdef RTMP_PCI_SUPPORT
		case RTMP_DEV_INF_PCI:
		case RTMP_DEV_INF_PCIE:
			{
				UINT32 e2p_csr;

				/* Init EEPROM Address Number, before access EEPROM; if 93c46, EEPROMAddressNum=6, else if 93c66, EEPROMAddressNum=8 */
				RTMP_IO_READ32(pAd, E2PROM_CSR, &e2p_csr);
#ifdef RT3290
				if (IS_RT3290(pAd))
					pAd->EEPROMAddressNum = 8;     /* 93C66 */
				else
#endif /* RT3290 */
#ifdef RT65xx
				if (IS_RT65XX(pAd))
					pAd->EEPROMAddressNum = 8;     /* 93C66 */
				else
#endif /* RT65xx */
#ifdef MT7601
				if (IS_MT7601(pAd))
					pAd->EEPROMAddressNum = 8;     // 93C66
				else
#endif /* MT7601 */
				if ((e2p_csr & 0x30) == 0)
					pAd->EEPROMAddressNum = 6;		/* 93C46*/
				else if ((e2p_csr & 0x30) == 0x10)
					pAd->EEPROMAddressNum = 8;     /* 93C66*/
				else
					pAd->EEPROMAddressNum = 8;     /* 93C86*/
				DBGPRINT(RT_DEBUG_TRACE, ("--> E2PROM_CSR=0x%x, EEPROMAddressNum=%d\n",
											e2p_csr, pAd->EEPROMAddressNum ));
				pChipOps->eeinit = NULL;
				pChipOps->eeread = rtmp_ee_prom_read16;
				pChipOps->eewrite = rtmp_ee_prom_write16;
				break;
			}
#endif /* RTMP_PCI_SUPPORT */

#ifdef RTMP_RBUS_SUPPORT
		case RTMP_DEV_INF_RBUS:
			pChipOps->eeinit = rtmp_nv_init;
			pChipOps->eeread = rtmp_ee_flash_read;
			pChipOps->eewrite = rtmp_ee_flash_write;
			pChipOps->loadFirmware = NULL;
			break;
#endif /* RTMP_RBUS_SUPPORT */

#ifdef RTMP_USB_SUPPORT
		case RTMP_DEV_INF_USB:
			pChipOps->eeinit = NULL;
			pChipOps->eeread = RTUSBReadEEPROM16;
			pChipOps->eewrite = RTUSBWriteEEPROM16;
			DBGPRINT(RT_DEBUG_OFF, ("pChipOps->eeread = RTUSBReadEEPROM16\n"));
			DBGPRINT(RT_DEBUG_OFF, ("pChipOps->eewrite = RTUSBWriteEEPROM16\n"));
			break;
#endif /* RTMP_USB_SUPPORT */
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("RtmpChipOpsEepromHook() failed!\n"));
			break;
	}

	return 0;
}
#endif

#if defined(RTMP_EFUSE_SUPPORT) && defined(RTMP_FLASH_SUPPORT)
static USHORT EE_FLASH_ID_LIST[]={
#ifdef RT2860
	0x2860, 
#endif /* RT2860 */
#ifdef RT2880
	0x2880, 
#endif /* RT2880 */
#ifdef RT2883
	0x2883,
	0x2880,
#endif /* RT2883 */
#ifdef RT3883
	0x3662,
	0x3883,
#endif /* RT3883 */
#ifdef RT305x
	0x3052,
	0x3051,
	0x3050,
	0x3350,
#endif /* RT305x */
#ifdef RT3352
	0x3352,
#endif /* RT3352 */
#ifdef RT5350
	0x5350,
#endif /* RT5350 */
#ifdef RT35xx
	0x3572,
#endif /* RT35xx */
#ifdef RT3090
	0x3090,
	0x3091,
	0x3092,
#endif /* RT3090 */
#ifdef RT5390
	0x5390,
	0x5392,
#endif /* RT5390 */
#ifdef RT5592
#ifdef RTMP_MAC_PCI
	0x5592,
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	0x5572,
#endif /* RTMP_MAC_USB */
#endif /* RT5592 */

#ifdef RT6352
	0x6352,
	0x7620,
#endif /* RT6352 */

#ifdef RT3593
#ifdef RTMP_MAC_PCI
	0x3593,
#endif /* RTMP_MAC_PCI */
#endif /* RT3593 */

#ifdef MT76x0
#ifdef RTMP_MAC_PCI
	0x7610,
#endif /* RTMP_MAC_PCI */
#endif /* MT76x0 */

#ifdef MT76x2
#ifdef RTMP_MAC_PCI
	0x7662,
#endif /* RTMP_MAC_PCI */
#endif /* MT76x0 */

#ifdef MT7603
    0x7603,
#endif
#ifdef MT7628
    0x7628,
    0x7603,
#endif
};

#define EE_FLASH_ID_NUM  (sizeof(EE_FLASH_ID_LIST) / sizeof(USHORT))
#endif /* defined(RTMP_EFUSE_SUPPORT) && defined(RTMP_FLASH_SUPPORT) */




static NDIS_STATUS rtmp_ee_bin_init(PRTMP_ADAPTER pAd)
{
#if defined(CAL_FREE_IC_SUPPORT) && !defined(CONFIG_COB_SOLUTION_SUPPORT)
	BOOLEAN bCalFree=0;
#endif /* defined(CAL_FREE_IC_SUPPORT) && !defined(CONFIG_COB_SOLUTION_SUPPORT) */

	rtmp_ee_load_from_bin(pAd);

#if defined(CAL_FREE_IC_SUPPORT) && !defined(CONFIG_COB_SOLUTION_SUPPORT)
	RTMP_CAL_FREE_IC_CHECK(pAd,bCalFree);
	if (bCalFree)
	{
		DBGPRINT(RT_DEBUG_OFF, ("Cal Free IC!!\n"));
		RTMP_CAL_FREE_DATA_GET(pAd);
	}
	else
	{
		DBGPRINT(RT_DEBUG_OFF, ("Non Cal Free IC!!\n"));
	}
#endif /* defined(CAL_FREE_IC_SUPPORT) && !defined(CONFIG_COB_SOLUTION_SUPPORT) */

	return NDIS_STATUS_SUCCESS;
}

#if defined(RTMP_EFUSE_SUPPORT) && defined(RTMP_FLASH_SUPPORT)
static VOID RtmpEepromTypeAdjust(RTMP_ADAPTER *pAd, UCHAR *pE2pType)
{
	UINT EfuseFreeBlock=0;

	eFuseGetFreeBlockCount(pAd, &EfuseFreeBlock);	
	
	if (EfuseFreeBlock >= pAd->chipCap.EFUSE_RESERVED_SIZE)
	{
		DBGPRINT(RT_DEBUG_OFF, ("NVM is efuse and the information is too less to bring up the interface\n"));
		DBGPRINT(RT_DEBUG_OFF, ("Force to use Flash mode\n"));
		*pE2pType = E2P_FLASH_MODE;
	}
	else 
	{
		USHORT eeFlashId = 0;
		int listIdx;
		BOOLEAN bFound = FALSE;

		rtmp_ee_efuse_read16(pAd, 0, &eeFlashId);
		DBGPRINT(RT_DEBUG_OFF, ("%s:: eeFlashId = 0x%x.\n", __FUNCTION__, eeFlashId));
		for(listIdx =0 ; listIdx < EE_FLASH_ID_NUM; listIdx++)
		{
			if (eeFlashId == EE_FLASH_ID_LIST[listIdx])
			{			
				bFound = TRUE;
				break;
			}
		}

		if (bFound == FALSE)
		{
			*pE2pType = E2P_FLASH_MODE;
		}
	}
}
#endif /* defined(RTMP_EFUSE_SUPPORT) && defined(RTMP_FLASH_SUPPORT) */

INT RtmpChipOpsEepromHook(RTMP_ADAPTER *pAd, INT infType,INT forceMode)
{
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
	UCHAR e2p_type;
#ifdef RTMP_PCI_SUPPORT
	UINT32 val;
#endif /* RTMP_PCI_SUPPORT */
	UCHAR e2p_default = 0;

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
		return -1;

#ifdef RTMP_EFUSE_SUPPORT
	efuse_probe(pAd);
#endif /* RTMP_EFUSE_SUPPORT */

	rtmp_eeprom_of_platform(pAd);

	if(forceMode != E2P_NONE && forceMode < NUM_OF_E2P_MODE)
	{
		e2p_type = (UCHAR)forceMode;
		DBGPRINT(RT_DEBUG_OFF, ("%s::forceMode: %d , infType: %d\n",
					__FUNCTION__, e2p_type, infType));
		pAd->eeprom_type = (e2p_type == E2P_EFUSE_MODE) ? EEPROM_EFUSE : EEPROM_FLASH;
	}
	else
	{
		e2p_type = pAd->E2pAccessMode;

		DBGPRINT(RT_DEBUG_OFF, ("%s::e2p_type=%d, inf_Type=%d\n",
					__FUNCTION__, e2p_type, infType));

		e2p_default = RtmpEepromGetDefault(pAd);
		/* If e2p_type is out of range, get the default mode */
		e2p_type = ((e2p_type != 0) && (e2p_type < NUM_OF_E2P_MODE)) ? e2p_type : e2p_default;

		if (infType == RTMP_DEV_INF_RBUS)
		{
			e2p_type = E2P_FLASH_MODE;
			pChipOps->loadFirmware = NULL;
		}
#if defined(RTMP_EFUSE_SUPPORT) && defined(RTMP_FLASH_SUPPORT)
		else if (pAd->E2pAccessMode == E2P_NONE)
		{
			/*
				User doesn't set E2pAccessMode in profile, adjust access mode automatically here.
			*/
			RtmpEepromTypeAdjust(pAd, &e2p_type);
		}
#endif /* defined(RTMP_EFUSE_SUPPORT) && defined(RTMP_FLASH_SUPPORT) */

#ifdef RTMP_EFUSE_SUPPORT
		if (e2p_type != E2P_EFUSE_MODE)
			pAd->bUseEfuse = FALSE;
#endif /* RTMP_EFUSE_SUPPORT */

		DBGPRINT(RT_DEBUG_OFF, ("%s: E2P type(%d), E2pAccessMode = %d, E2P default = %d\n", __FUNCTION__, e2p_type, pAd->E2pAccessMode, e2p_default));
		pAd->eeprom_type = (e2p_type==E2P_EFUSE_MODE)  ? EEPROM_EFUSE: EEPROM_FLASH;
	}

	pAd->e2pCurMode = e2p_type;
	
	switch (e2p_type)
	{
		case E2P_EEPROM_MODE:
			break;
		case E2P_BIN_MODE:
		{
			pChipOps->eeinit = rtmp_ee_bin_init;
			pChipOps->eeread = rtmp_ee_bin_read16;
			pChipOps->eewrite = rtmp_ee_bin_write16;
			DBGPRINT(RT_DEBUG_OFF, ("NVM is BIN mode\n"));
			return 0;
		}

#ifdef RTMP_FLASH_SUPPORT
		case E2P_FLASH_MODE:
		{
			pChipOps->eeinit = rtmp_nv_init;
			pChipOps->eeread = rtmp_ee_flash_read;
			pChipOps->eewrite = rtmp_ee_flash_write;
			pAd->flash_offset = DEFAULT_RF_OFFSET;
#ifdef CONFIG_RT_FIRST_CARD
			if ( pAd->dev_idx == 0 )
				pAd->flash_offset = CONFIG_RT_FIRST_IF_RF_OFFSET;
#endif /* CONFIG_RT_FIRST_CARD */
#ifdef CONFIG_RT_SECOND_CARD
			if ( pAd->dev_idx == 1 )
				pAd->flash_offset = CONFIG_RT_SECOND_IF_RF_OFFSET;
#endif /* CONFIG_RT_FIRST_CARD */
			DBGPRINT(RT_DEBUG_OFF, ("NVM is FLASH mode, flash_offset = 0x%x\n", pAd->flash_offset));
			return 0;
		}
#endif /* RTMP_FLASH_SUPPORT */

#ifdef RTMP_EFUSE_SUPPORT
		case E2P_EFUSE_MODE:
			if (pAd->bUseEfuse)
			{
				pChipOps->eeinit = eFuse_init;
				pChipOps->eeread = rtmp_ee_efuse_read16;
				pChipOps->eewrite = rtmp_ee_efuse_write16;
				DBGPRINT(RT_DEBUG_OFF, ("NVM is EFUSE mode\n"));
				return 0;
			}
			else
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s::hook efuse mode failed\n", __FUNCTION__));
				break;
			}
#endif /* RTMP_EFUSE_SUPPORT */
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Do not support E2P type(%d), change to BIN mode\n", __FUNCTION__, e2p_type));

			pChipOps->eeinit = rtmp_ee_bin_init;
			pChipOps->eeread = rtmp_ee_bin_read16;
			pChipOps->eewrite = rtmp_ee_bin_write16;
			DBGPRINT(RT_DEBUG_OFF, ("NVM is BIN mode\n"));
			return 0;
	}

	/* Hook functions based on interface types for EEPROM */
	switch (infType)
	{
#ifdef RTMP_PCI_SUPPORT
		case RTMP_DEV_INF_PCI:
		case RTMP_DEV_INF_PCIE:
			RTMP_IO_READ32(pAd, E2PROM_CSR, &val);
			if (((val & 0x30) == 0) && (!IS_RT3290(pAd)))
				pAd->EEPROMAddressNum = 6; /* 93C46 */
			else
				pAd->EEPROMAddressNum = 8; /* 93C66 or 93C86 */

			pChipOps->eeinit = NULL;
			pChipOps->eeread = rtmp_ee_prom_read16;
			pChipOps->eewrite = rtmp_ee_prom_write16;
			break;
#endif /* RTMP_PCI_SUPPORT */

#ifdef RTMP_USB_SUPPORT
		case RTMP_DEV_INF_USB:
			pChipOps->eeinit = NULL;
			pChipOps->eeread = RTUSBReadEEPROM16;
			pChipOps->eewrite = RTUSBWriteEEPROM16;
			break;
#endif /* RTMP_USB_SUPPORT */

		default:
			DBGPRINT(RT_DEBUG_ERROR, ("%s::hook failed\n", __FUNCTION__));
			break;
	}

	DBGPRINT(RT_DEBUG_OFF, ("NVM is EEPROM mode\n"));
	return 0;
}


BOOLEAN rtmp_get_default_bin_file_by_chip(
	IN PRTMP_ADAPTER pAd,
	IN UINT32 	ChipVersion,
	OUT RTMP_STRING **pBinFileName)
{
	BOOLEAN found = FALSE;
	INT i;
	for (i = 0; RTMP_CHIP_E2P_FILE_TABLE[i].ChipVersion != 0; i++ )
	{
		if (RTMP_CHIP_E2P_FILE_TABLE[i].ChipVersion == ChipVersion)
		{
			*pBinFileName = RTMP_CHIP_E2P_FILE_TABLE[i].name;
			DBGPRINT(RT_DEBUG_OFF,
						("%s(): Found E2P bin file name:%s\n",
						__FUNCTION__, *pBinFileName));
			found = TRUE;
			break;
		}
	}

	if (found == TRUE)
		DBGPRINT(RT_DEBUG_OFF, ("%s::Found E2P bin file name=%s\n", __FUNCTION__, *pBinFileName));
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s::E2P bin file name not found\n", __FUNCTION__));

	return found;
}


BOOLEAN rtmp_ee_bin_read16(RTMP_ADAPTER *pAd, UINT16 Offset, UINT16 *pValue)
{
	BOOLEAN IsEmpty = 0;

	DBGPRINT(RT_DEBUG_INFO, ("%s::Read from EEPROM buffer\n", __FUNCTION__));
	NdisMoveMemory(pValue, &(pAd->EEPROMImage[Offset]), 2);
	*pValue = le2cpu16(*pValue);

	if ((*pValue == 0xffff) || (*pValue == 0x0000))
		IsEmpty = 1;

	return IsEmpty;
}


INT rtmp_ee_bin_write16(
	IN RTMP_ADAPTER 	*pAd,
	IN USHORT 			Offset,
	IN USHORT 			data)
{
	DBGPRINT(RT_DEBUG_INFO, ("%s::Write to EEPROM buffer\n", __FUNCTION__));
	data = le2cpu16(data);
	NdisMoveMemory(&(pAd->EEPROMImage[Offset]), &data, 2);

	return 0;
}


INT rtmp_ee_load_from_bin(
	IN PRTMP_ADAPTER 	pAd)
{
	RTMP_STRING *src = NULL;
	INT ret_val;
	RTMP_OS_FD srcf;
	RTMP_OS_FS_INFO osFSInfo;

#ifdef RT_SOC_SUPPORT
#ifdef MULTIPLE_CARD_SUPPORT
	RTMP_STRING bin_file_path[128];
	RTMP_STRING *bin_file_name = NULL;
	UINT32 chip_ver = (pAd->MACVersion >> 16);

	if (rtmp_get_default_bin_file_by_chip(pAd, chip_ver, &bin_file_name) == TRUE)
	{
		if (pAd->MC_RowID > 0)
			sprintf(bin_file_path, "%s%s", EEPROM_2ND_FILE_DIR, bin_file_name);
		else
			sprintf(bin_file_path, "%s%s", EEPROM_1ST_FILE_DIR, bin_file_name);

		src = bin_file_path;
	}
	else
#endif /* MULTIPLE_CARD_SUPPORT */
#endif /* RT_SOC_SUPPORT */
		src = BIN_FILE_PATH;

	DBGPRINT(RT_DEBUG_TRACE, ("%s::FileName=%s\n", __FUNCTION__, src));

	RtmpOSFSInfoChange(&osFSInfo, TRUE);

	if (src && *src)
	{
		srcf = RtmpOSFileOpen(src, O_RDONLY, 0);
		if (IS_FILE_OPEN_ERR(srcf))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s::Error opening %s\n", __FUNCTION__, src));
			goto error;
		}
		else
		{
			NdisZeroMemory(pAd->EEPROMImage, MAX_EEPROM_BIN_FILE_SIZE);
			ret_val = RtmpOSFileRead(srcf, (RTMP_STRING *)pAd->EEPROMImage, MAX_EEPROM_BIN_FILE_SIZE);

			if (ret_val > 0)
				ret_val = NDIS_STATUS_SUCCESS;
			else
				DBGPRINT(RT_DEBUG_ERROR, ("%s::Read file \"%s\" failed(errCode=%d)!\n", __FUNCTION__, src, ret_val));
      		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s::Error src or srcf is null\n", __FUNCTION__));
		goto error;
	}

	ret_val = RtmpOSFileClose(srcf);

	if (ret_val)
		DBGPRINT(RT_DEBUG_ERROR, ("%s::Error %d closing %s\n", __FUNCTION__, -ret_val, src));

	RtmpOSFSInfoChange(&osFSInfo, FALSE);
	return TRUE;

error:
	if (pAd->chipCap.EEPROM_DEFAULT_BIN != NULL)
	{
		NdisMoveMemory(pAd->EEPROMImage, pAd->chipCap.EEPROM_DEFAULT_BIN,
		pAd->chipCap.EEPROM_DEFAULT_BIN_SIZE > MAX_EEPROM_BUFFER_SIZE?MAX_EEPROM_BUFFER_SIZE:pAd->chipCap.EEPROM_DEFAULT_BIN_SIZE);
		DBGPRINT(RT_DEBUG_TRACE, ("Load EEPROM Buffer from default BIN.\n"));
	}

	return FALSE;
}


INT rtmp_ee_write_to_bin(
	IN PRTMP_ADAPTER 	pAd)
{
	RTMP_STRING *src = NULL;
	INT ret_val;
	RTMP_OS_FD srcf;
	RTMP_OS_FS_INFO osFSInfo;

#ifdef RT_SOC_SUPPORT
#ifdef MULTIPLE_CARD_SUPPORT
	RTMP_STRING bin_file_path[128];
	RTMP_STRING *bin_file_name = NULL;
	UINT32 chip_ver = (pAd->MACVersion >> 16);

	if (rtmp_get_default_bin_file_by_chip(pAd, chip_ver, &bin_file_name) == TRUE)
	{
		if (pAd->MC_RowID > 0)
			sprintf(bin_file_path, "%s%s", EEPROM_2ND_FILE_DIR, bin_file_name);
		else
			sprintf(bin_file_path, "%s%s", EEPROM_1ST_FILE_DIR, bin_file_name);

		src = bin_file_path;
	}
	else
#endif /* MULTIPLE_CARD_SUPPORT */
#endif /* RT_SOC_SUPPORT */
		src = BIN_FILE_PATH;

	DBGPRINT(RT_DEBUG_TRACE, ("%s::FileName=%s\n", __FUNCTION__, src));

	RtmpOSFSInfoChange(&osFSInfo, TRUE);

	if (src && *src)
	{
		srcf = RtmpOSFileOpen(src, O_WRONLY|O_CREAT, 0);

		if (IS_FILE_OPEN_ERR(srcf))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s::Error opening %s\n", __FUNCTION__, src));
			return FALSE;
		}
		else
			RtmpOSFileWrite(srcf, (RTMP_STRING *)pAd->EEPROMImage, MAX_EEPROM_BIN_FILE_SIZE);
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s::Error src or srcf is null\n", __FUNCTION__));
		return FALSE;
	}

	ret_val = RtmpOSFileClose(srcf);

	if (ret_val)
		DBGPRINT(RT_DEBUG_ERROR, ("%s::Error %d closing %s\n", __FUNCTION__, -ret_val, src));

	RtmpOSFSInfoChange(&osFSInfo, FALSE);
	return TRUE;
}


INT Set_LoadEepromBufferFromBin_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT bEnable = simple_strtol(arg, 0, 10);
	INT result;

	if (bEnable < 0)
		return FALSE;
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Load EEPROM buffer from BIN, and change to BIN buffer mode\n"));
		result = rtmp_ee_load_from_bin(pAd);

		if ( result == FALSE )
		{
			if ( pAd->chipCap.EEPROM_DEFAULT_BIN != NULL )
			{
				NdisMoveMemory(pAd->EEPROMImage, pAd->chipCap.EEPROM_DEFAULT_BIN,
					pAd->chipCap.EEPROM_DEFAULT_BIN_SIZE > MAX_EEPROM_BUFFER_SIZE?MAX_EEPROM_BUFFER_SIZE:pAd->chipCap.EEPROM_DEFAULT_BIN_SIZE);
				DBGPRINT(RT_DEBUG_TRACE, ("Load EEPROM Buffer from default BIN.\n"));
			}

		}

		/* Change to BIN eeprom buffer mode */
		RtmpChipOpsEepromHook(pAd, pAd->infType,E2P_BIN_MODE);
		return TRUE;
	}
}


INT Set_EepromBufferWriteBack_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT e2p_mode = simple_strtol(arg, 0, 10);

	if (e2p_mode >= NUM_OF_E2P_MODE)
		return FALSE;

	switch (e2p_mode)
	{
#ifdef RTMP_EFUSE_SUPPORT
		case E2P_EFUSE_MODE:
			DBGPRINT(RT_DEBUG_OFF, ("Write EEPROM buffer back to eFuse\n"));
			rtmp_ee_write_to_efuse(pAd);
			break;
#endif /* RTMP_EFUSE_SUPPORT */

#ifdef RTMP_FLASH_SUPPORT
		case E2P_FLASH_MODE:
			DBGPRINT(RT_DEBUG_OFF, ("Write EEPROM buffer back to Flash\n"));
			rtmp_ee_flash_write_all(pAd, (PUSHORT)pAd->EEPROMImage);
			break;
#endif /* RTMP_FLASH_SUPPORT */

		case E2P_EEPROM_MODE:
			DBGPRINT(RT_DEBUG_OFF, ("Write EEPROM buffer back to EEPROM\n"));
			rtmp_ee_write_to_prom(pAd);
			break;

		case E2P_BIN_MODE:
			DBGPRINT(RT_DEBUG_OFF, ("Write EEPROM buffer back to BIN\n"));
			rtmp_ee_write_to_bin(pAd);
			break;

		default:
			DBGPRINT(RT_DEBUG_ERROR, ("%s::do not support this EEPROM access mode\n", __FUNCTION__));
			return FALSE;
	}

	return TRUE;
}

INT set_buffer_mode(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	BOOLEAN ret = FALSE;
	UINT16 efuse_val = 0;
	UINT i;
	UINT32 value;

	value = (UINT32) simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_OFF, ("%s[%d]\tvalue=%d", __func__, __LINE__, value));
	if (value == 0) {
		NdisZeroMemory(pAd->EEPROMImage, MAX_EEPROM_BIN_FILE_SIZE);
		for (i = 0; i < MAX_EEPROM_BIN_FILE_SIZE; i += 2) {
			eFuseRead(pAd, (USHORT)i, &efuse_val, 2);
			efuse_val = cpu2le16(efuse_val);
			NdisMoveMemory(&pAd->EEPROMImage[i], &efuse_val, 2);
		}
		pAd->eeprom_type = EEPROM_EFUSE;
		ret = TRUE;
	} else if (value == 1) {
		pAd->eeprom_type = EEPROM_FLASH;
	}
	return ret;
}

