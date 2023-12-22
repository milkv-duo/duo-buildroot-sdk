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
	rt3593.h
 
    Abstract:
	3*3 Wireless Chip PCIe

    Revision History:
    Who			When			What
    ---------	----------		----------------------------------------------
	SampleLin	20091105		Initial version
 */

#ifndef __RT3593_H__
#define __RT3593_H__

#ifdef RT3593

struct _RTMP_ADAPTER;

/*
	MCS 16 ~ 23 Test Note:
	Use fix rate mode, HT_MCS = 23, and set bit 30 of MAC Reg 134C to 0
	(disable auto-fallback mode).
*/
#ifndef RTMP_RF_RW_SUPPORT
#error "For RT3593/RT3573, you should define the compile flag -DRTMP_RF_RW_SUPPORT"
#endif

#ifndef RT30xx
#error "For RT3593/RT3573, you should define the compile flag -DRT30xx"
#endif

#ifndef RT35xx
#error "For RT3593/RT3573, you should define the compile flag -DRT35xx"
#endif

/* General definition */

/* if you want to support PCIe power save function */
/* 2009/11/06, if open the function, the signal will be bad and sometimes crush */
/*#define PCIE_PS_SUPPORT */

/* */
/* Device ID & Vendor ID, these values should match EEPROM value */
/* */
#define RALINK_3593_VERSION				((UINT32)0x35930400)

#define RT3593_MAC_VERSION_CHECK(__Version)							\
	((__Version & 0xFFFF0000) == 0x35930000)


/* External */
extern REG_PAIR RF3053RegTable[];
extern UCHAR NUM_RF_3053_REG_PARMS;
extern UCHAR RT3593_NUM_BBP_REG_PARMS;


/* MACRO definition */


#define BBP_REG_BF			BBP_R163 /* TxBf control */

/* */
/* Extended EEPROM format (EEPROM_EXT_XXX) */
/* */

/* */
/* NIC configuration #2 */
/* */
#define EEPROM_EXT_NIC_CONFIGURATION_2									0x38

/* */
/* Country region code for 5G band */
/* */
#define EEPROM_EXT_COUNTRY_REGION_CODE_FOR_5G_BAND						0x3F

/* */
/* Maximum Tx power for 2.4 GHz and 5 GHz band */
/* */
#define EEPROM_EXT_MAX_TX_POWER_OVER_2DOT4G_AND_5G						0x40

/* */
/* Frequency offset */
/* */
#define EEPROM_EXT_FREQUENCY_OFFSET										0x44

/* */
/* LED mode setting */
/* */
#define EEPROM_EXT_LED_MODE_SETTING										0x43

/* */
/* LED A/G configuration */
/* */
#define EEPROM_EXT_LED_AG_CONFIGURATION									0x44

/* */
/* LED ACT configuration */
/* */
#define EEPROM_EXT_LED_ACT_CONFIGURATION								0x46

/* */
/* LED A/G/ACT polarity */
/* */
#define EEPROM_EXT_LED_AG_ACT_POLARITY									0x48

/* */
/* External LNA gain for 2.4 GHz band */
/* */
#define EEPROM_EXT_EXTERNAL_LNA_GAIN_FOR_2DOT4G							0x4C

/* */
/* External LNA gain for 5 GHz band (channel #36~#64) */
/* */
#define EEPROM_EXT_EXTERNAL_LNA_GAIN_FOR_5G_OVER_CH36_TO_CH64			0x4D

/* */
/* External LNA gain for 5 GHz band (channel #100~#128) */
/* */
#define EEPROM_EXT_EXTERNAL_LNA_GAIN_FOR_5G_OVER_CH100_TO_CH128			0x4E

/* */
/* External LNA gain for 5 GHz band (channel #132~#165) */
/* */
#define EEPROM_EXT_EXTERNAL_LNA_GAIN_FOR_5G_OVER_CH132_TO_CH165			0x4F

/* */
/* RSSI0 offset for 2.4 GHz band */
/* */
#define EEPROM_EXT_RSSI0_OVER_2DOT4G									0x50

/* */
/* RSSI1 offset for 2.4 GHz band */
/* */
#define EEPROM_EXT_RSSI1_OVER_2DOT4G									0x51

/* */
/* RSSI2 offset for 2.4 GHz band */
/* */
#define EEPROM_EXT_RSSI2_OVER_2DOT4G									0x52

/* */
/* RSSI0 offset for 5 GHz band */
/* */
#define EEPROM_EXT_RSSI0_OVER_5G										0x54

/* */
/* RSSI1 offset for 5 GHz band */
/* */
#define EEPROM_EXT_RSSI1_OVER_5G										0x55

/* */
/* RSSI2 offset for 5 GHz band */
/* */
#define EEPROM_EXT_RSSI2_OVER_5G										0x56

/* */
/* Tx0 power over 2.4 GHz */
/* */
#define EEPROM_EXT_TX0_OVER_2DOT4G										0x60

/* */
/* Tx1 power over 2.4 GHz */
/* */
#define EEPROM_EXT_TX1_OVER_2DOT4G										0x6E

/* */
/* Tx2 power over 2.4 GHz */
/* */
#define EEPROM_EXT_TX2_OVER_2DOT4G										0x7C

/* */
/* Tx0 power over 5 GHz */
/* */
#define EEPROM_EXT_TX0_OVER_5G											0x96

/* */
/* Tx1 power over 5 GHz */
/* */
#define EEPROM_EXT_TX1_OVER_5G											0xCA

/* */
/* Tx2 power over 5 GHz */
/* */
#define EEPROM_EXT_TX2_OVER_5G											0xFE

/* */
/* Tx power delta TSSI bounday over 2.4 GHz */
/* */
#define EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_2DOT4G						0x8A

/* */
/* Tx power delta TSSI bounday over 5 GHz */
/* */
#define EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_5G							0x134

/* */
/* Tx ALC step value for 2.4 GHz */
/* */
#define EEPROM_EXT_TX_ALC_STEP_VALUE_OVER_2DOT4G						0x93

/* */
/* Tx ALC step value for 5 GHz */
/* */
#define EEPROM_EXT_TX_ALC_STEP_VALUE_OVER_5G							0x13D

/* */
/* Tx power control over BW20 at 2.4G */
/* */
#define EEPROM_EXT_TX_PWR_CTRL_OVER_BW20_2DOT4G							0x140

/* */
/* Tx power control over BW40 at 2.4G */
/* */
#define EEPROM_EXT_TX_PWR_CTRL_OVER_BW40_2DOT4G							0x150

/* */
/* Tx power control over BW20 at 5G */
/* */
#define EEPROM_EXT_TX_PWR_CTRL_OVER_BW20_5G								0x160

/* */
/* Tx power control over BW40 at 5G */
/* */
#define EEPROM_EXT_TX_PWR_CTRL_OVER_BW40_5G								0x170

/* */
/* The 2.4G manual channel */
/* */
#define EEPROM_EXT_2DOTG_MANUAL_CHANNEL_OFFSET							0x190

/* */
/* The 5G manual channel (part #1) */
/* */
#define EEPROM_EXT_5G_MANUAL_CAHNNEL_PART_ONE_OFFSET					0x192

/* */
/* The 5G manual channel (part #2) */
/* */
#define EEPROM_EXT_5G_MANUAL_CHANNEL_PART_TWO_OFFSET					0x194

/* work around */
#ifdef RELEASE_EXCLUDE
	/*
		Work Around: 2009/11/11
		If not send any frame here, we can not receive any packet
		in 40MHz; no problem in 20MHz.
	*/
#endif /* RELEASE_EXCLUDE */
#define RT3593_WA_MONITOR(__pAd)											\
	if (IS_RT3593(__pAd))														\
	{																		\
		RTMPSendNullFrame(__pAd, __pAd->CommonCfg.TxRate,					\
			(OPSTATUS_TEST_FLAG(__pAd, fOP_STATUS_WMM_INUSED) ? TRUE:FALSE),\
			(__pAd)->CommonCfg.bAPSDForcePowerSave ? PWR_SAVE : (__pAd)->StaCfg.PwrMgmt.Psm);\
	}

/* receive frequency offset */
#define RT3593_FREQ_OFFSET_RCV_CHECK(_pAd, _pRxD, _pRxBlk, _pHeader)		\
	if (IS_RT3593(_pAd))													\
	{																		\
		if ((_pAd->FreqCalibrationCtrl.bEnableFrequencyCalibration == TRUE) && \
			(INFRA_ON(_pAd)) &&											\
			(_pRxD->Crc == 0) &&											\
			(_pHeader->FC.Type == FC_TYPE_MGMT) &&							\
			(_pHeader->FC.SubType == SUBTYPE_BEACON) &&					\
			(MAC_ADDR_EQUAL(&_pAd->CommonCfg.Bssid, &_pHeader->Addr2)))	\
		{																	\
			_pAd->FreqCalibrationCtrl.LatestFreqOffsetOverBeacon =			\
										GetFrequencyOffset(_pAd, _pRxBlk);	\
			DBGPRINT(RT_DEBUG_INFO, ("%s: Beacon, CRC error = %d,			\
					__pHeader->Sequence = %d,								\
					SA = %02X:%02X:%02X:%02X:%02X:%02X,						\
					frequency offset = %d, MCS = %d, BW = %d\n",			\
					__FUNCTION__,											\
					_pRxD->Crc, 											\
					_pHeader->Sequence,									\
					_pHeader->Addr2[0], _pHeader->Addr2[1],				\
					_pHeader->Addr2[2], _pHeader->Addr2[3],				\
					_pHeader->Addr2[4], _pHeader->Addr2[5],				\
					((CHAR)(_pRxBlk->rx_signal.freq_offset)),								\
					_pRxBlk->RxWIMCS,											\
					_pRxBlk->RxWIBW));											\
		}																	\
	}


/* get BA maximum size */
#ifdef CONFIG_AP_SUPPORT
#define RT3593_AP_MAX_BW_SIZE_GET(__pAd, __pEntry, __MaxSize)				\
	IF_DEV_CONFIG_OPMODE_ON_AP(__pAd)										\
		if (IS_RT3593(__pAd) && (__pEntry->HTCapability.MCSSet[2] != 0x00))	\
			__MaxSize = 31;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#define RT3593_STA_MAX_BW_SIZE_GET(__pAd, __pEntry, __MaxSize)				\
	IF_DEV_CONFIG_OPMODE_ON_STA(__pAd)										\
		if (IS_RT3593(__pAd) &&												\
			(__pAd->StaActive.SupportedPhyInfo.MCSSet[2] != 0x00) &&		\
			INFRA_ON(__pAd))												\
		{																	\
			/* the receive capability can accept MCS16 ~ MCS23 */			\
			__MaxSize = 31;													\
		}
#endif /* CONFIG_STA_SUPPORT */

/* update channel in monitor mode */
#ifdef CONFIG_STA_SUPPORT
#ifdef MONITOR_FLAG_11N_SNIFFER_SUPPORT
#define RT3593_MONITOR_CHAN_CHANGE(__pAd)									\
	if (IS_RT3593(__pAd))													\
	{																		\
		if (__pAd->StaCfg.BssMonitorFlag & MONITOR_FLAG_11N_SNIFFER)		\
			Set_NetworkType_Proc(__pAd, "Monitor2");						\
		else																\
			Set_NetworkType_Proc(__pAd, "Monitor");							\
	}

#else /* MONITOR_FLAG_11N_SNIFFER_SUPPORT */

#define RT3593_MONITOR_CHAN_CHANGE(__pAd)									\
	if (IS_RT3593(__pAd))													\
	{																		\
		Set_NetworkType_Proc(__pAd, "Monitor");								\
	}
#endif /* MONITOR_FLAG_11N_SNIFFER_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

/* read BBP R66 value */
#define RT3593_R66_MID_LOW_SENS_GET(__pAd, __Value)						\
	if (IS_RT3593(__pAd))													\
		__Value = (__pAd->hw_cfg.lan_gain * 5 / 3) + 0x20 + 0x10; /* 20MBW over 5GHz: EXT_LNA * 1.66 + 0x20*/

#define RT3593_R66_NON_MID_LOW_SEMS_GET(__pAd, __Value)					\
	if (IS_RT3593(__pAd))													\
		__Value = (__pAd->hw_cfg.lan_gain * 5 / 3) + 0x20; /* 40MBW over 5GHz: EXT_LNA * 1.66 + 0x20*/

/* AutoFallback enable/disable */
#define RT3593_AUTO_FALLBACK_ENABLE(__pAd)								\
	if (IS_RT3593(__pAd))													\
		AsicSetAutoFallBack(__pAd, TRUE);

/* read value from EEPROM */
#define RT3593_EEPROM_COUNTRY_REGION_READ(__pAd)						\
{																			\
	UCHAR __CountryRegion5G = 0, __CountryRegion2Dot4G = 0;					\
	USHORT __Value;															\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_COUNTRY_REGION_CODE_FOR_5G_BAND - 1), __Value);\
	/* Swap*/																\
	__CountryRegion5G = ((__Value & 0xFF00) >> 8);							\
	__CountryRegion2Dot4G = (__Value & 0x00FF);								\
	__Value = ((__CountryRegion2Dot4G << 8) | __CountryRegion5G);			\
	__pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] = __Value;			\
}

#define RT3593_EEPROM_TSSI_24G_READ(__pAd)									\
{																			\
	EEPROM_TX_PWR_STRUC __Power;											\
	USHORT __Value;															\
	RT28xx_EEPROM_READ16(__pAd, EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_2DOT4G, __Power.word);\
	__pAd->TssiMinusBoundaryG[4] = __Power.field.Byte0;						\
	__pAd->TssiMinusBoundaryG[3] = __Power.field.Byte1;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_2DOT4G + 2), __Power.word);\
	__pAd->TssiMinusBoundaryG[2] = __Power.field.Byte0;						\
	__pAd->TssiMinusBoundaryG[1] = __Power.field.Byte1;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_2DOT4G + 4), __Power.word);\
	__pAd->TssiRefG = __Power.field.Byte0;									\
	__pAd->TssiPlusBoundaryG[1] = __Power.field.Byte1;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_2DOT4G + 6), __Power.word);\
	__pAd->TssiPlusBoundaryG[2] = __Power.field.Byte0;						\
	__pAd->TssiPlusBoundaryG[3] = __Power.field.Byte1;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_2DOT4G + 8), __Power.word);\
	__pAd->TssiPlusBoundaryG[4] = __Power.field.Byte0;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_TX_ALC_STEP_VALUE_OVER_2DOT4G - 1), __Value);\
	__pAd->TxAgcStepG = ((__Value & 0xFF00) >> 8);							\
	__pAd->TxAgcCompensateG = 0;											\
	__pAd->TssiMinusBoundaryG[0] = __pAd->TssiRefG;							\
	__pAd->TssiPlusBoundaryG[0]  = __pAd->TssiRefG;							\
	/* Disable TxAgc if the based value is not right */						\
	if (__pAd->TssiRefG == 0xFF)											\
		__pAd->bAutoTxAgcG = FALSE;											\
}

#define RT3593_EEPROM_TSSI_5G_READ(__pAd)									\
{																			\
	EEPROM_TX_PWR_STRUC __Power;											\
	USHORT __Value;															\
	RT28xx_EEPROM_READ16(__pAd, EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_5G, __Power.word);\
	__pAd->TssiMinusBoundaryA[0][4] = __Power.field.Byte0;						\
	__pAd->TssiMinusBoundaryA[0][3] = __Power.field.Byte1;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_5G + 2), __Power.word);\
	__pAd->TssiMinusBoundaryA[0][2] = __Power.field.Byte0;						\
	__pAd->TssiMinusBoundaryA[0][1] = __Power.field.Byte1;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_5G + 4), __Power.word);\
	__pAd->TssiRefA = __Power.field.Byte0;									\
	__pAd->TssiPlusBoundaryA[0][1] = __Power.field.Byte1;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_5G + 6), __Power.word);\
	__pAd->TssiPlusBoundaryA[0][2] = __Power.field.Byte0;						\
	__pAd->TssiPlusBoundaryA[0][3] = __Power.field.Byte1;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_DELTA_TSSI_BOUNDARY_OVER_5G + 8), __Power.word);\
	__pAd->TssiPlusBoundaryA[0][4] = __Power.field.Byte0;						\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_TX_ALC_STEP_VALUE_OVER_5G - 1), __Value);\
	__pAd->TxAgcStepA = ((__Value & 0xFF00) >> 8);							\
	__pAd->TxAgcCompensateA = 0;											\
	__pAd->TssiMinusBoundaryA[0][0] = __pAd->TssiRefA;							\
	__pAd->TssiPlusBoundaryA[0][0]  = __pAd->TssiRefA;							\
	/* Disable TxAgc if the based value is not right */						\
	if (__pAd->TssiRefA == 0xFF)											\
		__pAd->bAutoTxAgcA = FALSE;											\
}

#define RT3593_EEPROM_RSSI01_OFFSET_24G_READ(__pAd)							\
{																			\
	USHORT __Value;															\
	RT28xx_EEPROM_READ16(__pAd, EEPROM_EXT_RSSI0_OVER_2DOT4G, __Value);		\
	__pAd->BGRssiOffset[0] = (__Value & 0x00FF);								\
	__pAd->BGRssiOffset[1] = ((__Value >> 8) & 0x00FF);						\
}

#define RT3593_EEPROM_RSSI2_OFFSET_ALNAGAIN1_24G_READ(__pAd)					\
{																			\
	USHORT __Value;															\
	RT28xx_EEPROM_READ16(__pAd, EEPROM_EXT_RSSI2_OVER_2DOT4G, __Value);		\
	__pAd->BGRssiOffset[2] = (__Value & 0x00FF);								\
	RT28xx_EEPROM_READ16(__pAd, EEPROM_EXT_EXTERNAL_LNA_GAIN_FOR_5G_OVER_CH100_TO_CH128, __Value);\
	__pAd->ALNAGain1 = (__Value & 0x00FF);									\
}

#define RT3593_EEPROM_BLNA_ALNA_GAIN0_24G_READ(__pAd)						\
{																			\
	USHORT __Value;															\
	RT28xx_EEPROM_READ16(__pAd, EEPROM_EXT_EXTERNAL_LNA_GAIN_FOR_2DOT4G, __Value);\
	__pAd->BLNAGain = (__Value & 0x00FF);									\
	__pAd->ALNAGain0 = (__Value >> 8);										\
}

#define RT3593_EEPROM_RSSI01_OFFSET_5G_READ(__pAd)							\
{																			\
	USHORT __Value;															\
	RT28xx_EEPROM_READ16(__pAd, EEPROM_EXT_RSSI0_OVER_5G, __Value);			\
	__pAd->ARssiOffset[0] = (__Value & 0x00FF);								\
	__pAd->ARssiOffset[1] = ((__Value >> 8) & 0x00FF);						\
}

#define RT3593_EEPROM_RSSI2_OFFSET_ALNAGAIN2_5G_READ(__pAd)					\
{																			\
	USHORT __Value;															\
	RT28xx_EEPROM_READ16(__pAd, EEPROM_EXT_RSSI2_OVER_5G, __Value);			\
	__pAd->ARssiOffset[2] = (__Value & 0x00FF);								\
	RT28xx_EEPROM_READ16(__pAd, (EEPROM_EXT_EXTERNAL_LNA_GAIN_FOR_5G_OVER_CH132_TO_CH165 - 1), __Value);\
	__pAd->ALNAGain2 = ((__Value >> 8) & 0x00FF);							\
}

/* config some registers by antenna number */
#define RT3593_CONFIG_SET_BY_ANTENNA(__pAd)									\
{																			\
	if (__pAd->Antenna.field.RxPath == 1) /* Rx = 1 antenna */				\
	{																		\
		RTMP_BBP_IO_WRITE8_BY_REG_ID(__pAd, BBP_R86, 0x00);					\
	}																		\
	else if ((__pAd->Antenna.field.RxPath == 2) ||							\
			(__pAd->Antenna.field.RxPath == 3)) /* Rx = 2/3 antennas*/		\
	{																		\
		RTMP_BBP_IO_WRITE8_BY_REG_ID(__pAd, BBP_R86, 0x46);					\
	}																		\
	else																	\
	{																		\
		RTMP_BBP_IO_WRITE8_BY_REG_ID(__pAd, BBP_R86, 0x46);					\
	}																		\
}


/* SNR mapping */
#define RT3593_SNR_MAPPING_INIT(__pAd)										\
{																			\
	RTMP_BBP_IO_WRITE8_BY_REG_ID(__pAd, BBP_R142, 6);						\
	RTMP_BBP_IO_WRITE8_BY_REG_ID(__pAd, BBP_R143, 160);						\
	RTMP_BBP_IO_WRITE8_BY_REG_ID(__pAd, BBP_R142, 7);						\
	RTMP_BBP_IO_WRITE8_BY_REG_ID(__pAd, BBP_R143, 161);						\
	RTMP_BBP_IO_WRITE8_BY_REG_ID(__pAd, BBP_R142, 8);						\
	RTMP_BBP_IO_WRITE8_BY_REG_ID(__pAd, BBP_R143, 162);						\
}


/* Public functions */
VOID RT3593_Init(
	IN struct _RTMP_ADAPTER			*pAd);

VOID NICInitRT3593MacRegisters(
	IN struct _RTMP_ADAPTER			*pAd);

VOID NICInitRT3593BbpRegisters(
	IN struct _RTMP_ADAPTER			*pAd);

VOID NICInitRT3593RFRegisters(
	IN struct _RTMP_ADAPTER			*pAd);

VOID RTMPVerifyTxPwrPerRateExt(
	IN		struct _RTMP_ADAPTER	*pAd, 
	INOUT	PUCHAR					pTxPwr);

VOID RTMPReadTxPwrPerRateExt(
	IN struct _RTMP_ADAPTER			*pAd);

VOID RT3593_AsicGetTxPowerOffset(
	IN 		struct _RTMP_ADAPTER			*pAd,
	INOUT 	PULONG 						pTxPwr);
#endif /* RT3593 */
#endif /*__RT3593_H__ */

