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
	mt7601.h

    Abstract:

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */

#ifndef __MT7601_H__
#define __MT7601_H__

struct _RTMP_ADAPTER;

#define MAC_VERSION		""

#define BBP_VERSION		"MT7601E2_BBP_CSD_20121019"
#define RF_VERSION		"RT6390_RF_Register_20121122"

#define MT7601_VALID_EEPROM_VERSION		0x0C



#if 0
#define RF_PA_MODE0_DECODE		0
#define RF_PA_MODE1_DECODE		20480	// 2.5 * 8192
#define RF_PA_MODE2_DECODE		4096	// 0.5 * 8192
#define RF_PA_MODE3_DECODE		4096	// 0.5 * 8192
#else
#define RF_PA_MODE0_DECODE		0
#define RF_PA_MODE1_DECODE		8847	// 1.08 * 8192
#define RF_PA_MODE2_DECODE		-5734	// -0.7 * 8192
#define RF_PA_MODE3_DECODE		-5734	// -0.7 * 8192
#endif

#define MT7601_E2_TEMPERATURE_SLOPE	39

#define	BW20_MCS_POWER_CCK_1M			((pAd->Tx20MPwrCfgGBand[0] & 0xFF) < 0x20)?(pAd->Tx20MPwrCfgGBand[0] & 0xFF):(CHAR)((pAd->Tx20MPwrCfgGBand[0] & 0xFF) - 0x40)
#define	BW20_MCS_POWER_CCK_2M			((pAd->Tx20MPwrCfgGBand[0] & 0xFF) < 0x20)?(pAd->Tx20MPwrCfgGBand[0] & 0xFF):(CHAR)((pAd->Tx20MPwrCfgGBand[0] & 0xFF) - 0x40)
#define	BW20_MCS_POWER_CCK_5M			(((pAd->Tx20MPwrCfgGBand[0] & 0xFF00) >> 8) < 0x20)?((pAd->Tx20MPwrCfgGBand[0] & 0xFF00) >> 8):(CHAR)(((pAd->Tx20MPwrCfgGBand[0] & 0xFF00) >> 8)-0x40)
#define	BW20_MCS_POWER_CCK_11M			(((pAd->Tx20MPwrCfgGBand[0] & 0xFF00) >> 8) < 0x20)?((pAd->Tx20MPwrCfgGBand[0] & 0xFF00) >> 8):(CHAR)(((pAd->Tx20MPwrCfgGBand[0] & 0xFF00) >> 8)-0x40)
#define	BW40_MCS_POWER_CCK_1M			((pAd->Tx40MPwrCfgGBand[0] & 0xFF) < 0x20)?(pAd->Tx40MPwrCfgGBand[0] & 0xFF) :(CHAR)(((pAd->Tx40MPwrCfgGBand[0] & 0xFF) < 0x20)-0x40)
#define	BW40_MCS_POWER_CCK_2M			((pAd->Tx40MPwrCfgGBand[0] & 0xFF) < 0x20)?(pAd->Tx40MPwrCfgGBand[0] & 0xFF) :(CHAR)(((pAd->Tx40MPwrCfgGBand[0] & 0xFF) < 0x20)-0x40)
#define	BW40_MCS_POWER_CCK_5M			(((pAd->Tx40MPwrCfgGBand[0] & 0xFF00) >> 8) < 0x20)?((pAd->Tx40MPwrCfgGBand[0] & 0xFF00) >> 8):(CHAR)(((pAd->Tx40MPwrCfgGBand[0] & 0xFF00) >> 8)-0x40)
#define	BW40_MCS_POWER_CCK_11M			(((pAd->Tx40MPwrCfgGBand[0] & 0xFF00) >> 8) < 0x20)?((pAd->Tx40MPwrCfgGBand[0] & 0xFF00) >> 8):(CHAR)(((pAd->Tx40MPwrCfgGBand[0] & 0xFF00) >> 8)-0x40)

#define	BW20_MCS_POWER_OFDM_6M			(((pAd->Tx20MPwrCfgGBand[0] & 0xFF0000) >> 16) < 0x20)?((pAd->Tx20MPwrCfgGBand[0] & 0xFF0000) >> 16):(CHAR)(((pAd->Tx20MPwrCfgGBand[0] & 0xFF0000) >> 16)-0x40)
#define	BW20_MCS_POWER_OFDM_9M			(((pAd->Tx20MPwrCfgGBand[0] & 0xFF0000) >> 16) < 0x20)?((pAd->Tx20MPwrCfgGBand[0] & 0xFF0000) >> 16):(CHAR)(((pAd->Tx20MPwrCfgGBand[0] & 0xFF0000) >> 16)-0x40)
#define	BW20_MCS_POWER_OFDM_12M		(((pAd->Tx20MPwrCfgGBand[0] & 0xFF000000) >> 24) < 0x20)?((pAd->Tx20MPwrCfgGBand[0] & 0xFF000000) >> 24):(CHAR)(((pAd->Tx20MPwrCfgGBand[0] & 0xFF000000) >> 24)-0x40)
#define	BW20_MCS_POWER_OFDM_18M		(((pAd->Tx20MPwrCfgGBand[0] & 0xFF000000) >> 24) < 0x20)?((pAd->Tx20MPwrCfgGBand[0] & 0xFF000000) >> 24):(CHAR)(((pAd->Tx20MPwrCfgGBand[0] & 0xFF000000) >> 24)-0x40)
#define	BW20_MCS_POWER_OFDM_24M		((pAd->Tx20MPwrCfgGBand[1] & 0xFF) < 0x20)?(pAd->Tx20MPwrCfgGBand[1] & 0xFF):(CHAR)((pAd->Tx20MPwrCfgGBand[1] & 0xFF)-0x40)
#define	BW20_MCS_POWER_OFDM_36M		((pAd->Tx20MPwrCfgGBand[1] & 0xFF) < 0x20)?(pAd->Tx20MPwrCfgGBand[1] & 0xFF):(CHAR)((pAd->Tx20MPwrCfgGBand[1] & 0xFF)-0x40)
#define	BW20_MCS_POWER_OFDM_48M		(((pAd->Tx20MPwrCfgGBand[1] & 0xFF00) >> 8) < 0x20)?((pAd->Tx20MPwrCfgGBand[1] & 0xFF00) >> 8):(CHAR)(((pAd->Tx20MPwrCfgGBand[1] & 0xFF00) >> 8)-0x40)
#define	BW20_MCS_POWER_OFDM_54M		(((pAd->Tx20MPwrCfgGBand[1] & 0xFF00) >> 8) < 0x20)?((pAd->Tx20MPwrCfgGBand[1] & 0xFF00) >> 8):(CHAR)(((pAd->Tx20MPwrCfgGBand[1] & 0xFF00) >> 8)-0x40)
#define	BW40_MCS_POWER_OFDM_6M			(((pAd->Tx40MPwrCfgGBand[0] & 0xFF0000) >> 16) < 0x20)?((pAd->Tx40MPwrCfgGBand[0] & 0xFF0000) >> 16):(CHAR)(((pAd->Tx40MPwrCfgGBand[0] & 0xFF0000) >> 16)-0x40)
#define	BW40_MCS_POWER_OFDM_9M			(((pAd->Tx40MPwrCfgGBand[0] & 0xFF0000) >> 16) < 0x20)?((pAd->Tx40MPwrCfgGBand[0] & 0xFF0000) >> 16):(CHAR)(((pAd->Tx40MPwrCfgGBand[0] & 0xFF0000) >> 16)-0x40)
#define	BW40_MCS_POWER_OFDM_12M		(((pAd->Tx40MPwrCfgGBand[0] & 0xFF000000) >> 24) < 0x20)?((pAd->Tx40MPwrCfgGBand[0] & 0xFF000000) >> 24):(CHAR)(((pAd->Tx40MPwrCfgGBand[0] & 0xFF000000) >> 24)-0x40)
#define	BW40_MCS_POWER_OFDM_18M		(((pAd->Tx40MPwrCfgGBand[0] & 0xFF000000) >> 24) < 0x20)?((pAd->Tx40MPwrCfgGBand[0] & 0xFF000000) >> 24):(CHAR)(((pAd->Tx40MPwrCfgGBand[0] & 0xFF000000) >> 24)-0x40)
#define	BW40_MCS_POWER_OFDM_24M		((pAd->Tx40MPwrCfgGBand[1] & 0xFF) < 0x20)?(pAd->Tx40MPwrCfgGBand[1] & 0xFF):(CHAR)((pAd->Tx40MPwrCfgGBand[1] & 0xFF)-0x40)
#define	BW40_MCS_POWER_OFDM_36M		((pAd->Tx40MPwrCfgGBand[1] & 0xFF) < 0x20)?(pAd->Tx40MPwrCfgGBand[1] & 0xFF):(CHAR)((pAd->Tx40MPwrCfgGBand[1] & 0xFF)-0x40)
#define	BW40_MCS_POWER_OFDM_48M		(((pAd->Tx40MPwrCfgGBand[1] & 0xFF00) >> 8) < 0x20)?((pAd->Tx40MPwrCfgGBand[1] & 0xFF00) >> 8):(CHAR)(((pAd->Tx40MPwrCfgGBand[1] & 0xFF00) >> 8)-0x40)
#define	BW40_MCS_POWER_OFDM_54M		(((pAd->Tx40MPwrCfgGBand[1] & 0xFF00) >> 8) < 0x20)?((pAd->Tx40MPwrCfgGBand[1] & 0xFF00) >> 8):(CHAR)(((pAd->Tx40MPwrCfgGBand[1] & 0xFF00) >> 8)-0x40)

#define	BW20_MCS_POWER_HT_MCS0			(((pAd->Tx20MPwrCfgGBand[1] & 0xFF0000) >> 16) < 0x20)?((pAd->Tx20MPwrCfgGBand[1] & 0xFF0000) >> 16):(CHAR)(((pAd->Tx20MPwrCfgGBand[1] & 0xFF0000) >> 16)-0x40)
#define	BW20_MCS_POWER_HT_MCS1			(((pAd->Tx20MPwrCfgGBand[1] & 0xFF0000) >> 16) < 0x20)?((pAd->Tx20MPwrCfgGBand[1] & 0xFF0000) >> 16):(CHAR)(((pAd->Tx20MPwrCfgGBand[1] & 0xFF0000) >> 16)-0x40)
#define	BW20_MCS_POWER_HT_MCS2			(((pAd->Tx20MPwrCfgGBand[1] & 0xFF000000) >> 24) < 0x20)?((pAd->Tx20MPwrCfgGBand[1] & 0xFF000000) >> 24):(CHAR)(((pAd->Tx20MPwrCfgGBand[1] & 0xFF000000) >> 24)-0x40)
#define	BW20_MCS_POWER_HT_MCS3			(((pAd->Tx20MPwrCfgGBand[1] & 0xFF000000) >> 24) < 0x20)?((pAd->Tx20MPwrCfgGBand[1] & 0xFF000000) >> 24):(CHAR)(((pAd->Tx20MPwrCfgGBand[1] & 0xFF000000) >> 24)-0x40)
#define	BW20_MCS_POWER_HT_MCS4			((pAd->Tx20MPwrCfgGBand[2] & 0xFF) < 0x20)?(pAd->Tx20MPwrCfgGBand[2] & 0xFF):(CHAR)((pAd->Tx20MPwrCfgGBand[2] & 0xFF)-0x40)
#define	BW20_MCS_POWER_HT_MCS5			((pAd->Tx20MPwrCfgGBand[2] & 0xFF) < 0x20)?(pAd->Tx20MPwrCfgGBand[2] & 0xFF):(CHAR)((pAd->Tx20MPwrCfgGBand[2] & 0xFF)-0x40)
#define	BW20_MCS_POWER_HT_MCS6			(((pAd->Tx20MPwrCfgGBand[2] & 0xFF00 ) >> 8) < 0x20)?((pAd->Tx20MPwrCfgGBand[2] & 0xFF00 ) >> 8):(CHAR)(((pAd->Tx20MPwrCfgGBand[2] & 0xFF00 ) >> 8)-0x40)
#define	BW20_MCS_POWER_HT_MCS7			(((pAd->Tx20MPwrCfgGBand[2] & 0xFF00 ) >> 8) < 0x20)?((pAd->Tx20MPwrCfgGBand[2] & 0xFF00 ) >> 8):(CHAR)(((pAd->Tx20MPwrCfgGBand[2] & 0xFF00 ) >> 8)-0x40)
#define	BW40_MCS_POWER_HT_MCS0			(((pAd->Tx40MPwrCfgGBand[1] & 0xFF0000) >> 16) < 0x20)?((pAd->Tx40MPwrCfgGBand[1] & 0xFF0000) >> 16):(CHAR)(((pAd->Tx40MPwrCfgGBand[1] & 0xFF0000) >> 16)-0x40)
#define	BW40_MCS_POWER_HT_MCS1			(((pAd->Tx40MPwrCfgGBand[1] & 0xFF0000) >> 16) < 0x20)?((pAd->Tx40MPwrCfgGBand[1] & 0xFF0000) >> 16):(CHAR)(((pAd->Tx40MPwrCfgGBand[1] & 0xFF0000) >> 16)-0x40)
#define	BW40_MCS_POWER_HT_MCS2			(((pAd->Tx40MPwrCfgGBand[1] & 0xFF000000) >> 24) < 0x20)?((pAd->Tx40MPwrCfgGBand[1] & 0xFF000000) >> 24):(CHAR)(((pAd->Tx40MPwrCfgGBand[1] & 0xFF000000) >> 24)-0x40)
#define	BW40_MCS_POWER_HT_MCS3			(((pAd->Tx40MPwrCfgGBand[1] & 0xFF000000) >> 24) < 0x20)?((pAd->Tx40MPwrCfgGBand[1] & 0xFF000000) >> 24):(CHAR)(((pAd->Tx40MPwrCfgGBand[1] & 0xFF000000) >> 24)-0x40)
#define	BW40_MCS_POWER_HT_MCS4			((pAd->Tx40MPwrCfgGBand[2] & 0xFF) < 0x20)?(pAd->Tx40MPwrCfgGBand[2] & 0xFF):(CHAR)((pAd->Tx40MPwrCfgGBand[2] & 0xFF)-0x40)
#define	BW40_MCS_POWER_HT_MCS5			((pAd->Tx40MPwrCfgGBand[2] & 0xFF) < 0x20)?(pAd->Tx40MPwrCfgGBand[2] & 0xFF):(CHAR)((pAd->Tx40MPwrCfgGBand[2] & 0xFF)-0x40)
#define	BW40_MCS_POWER_HT_MCS6			(((pAd->Tx40MPwrCfgGBand[2] & 0xFF00) >> 8) < 0x20)?((pAd->Tx40MPwrCfgGBand[2] & 0xFF00) >> 8):(CHAR)(((pAd->Tx40MPwrCfgGBand[2] & 0xFF00) >> 8)-0x40)
#define	BW40_MCS_POWER_HT_MCS7			(((pAd->Tx40MPwrCfgGBand[2] & 0xFF00) >> 8) < 0x20)?((pAd->Tx40MPwrCfgGBand[2] & 0xFF00) >> 8):(CHAR)(((pAd->Tx40MPwrCfgGBand[2] & 0xFF00) >> 8)-0x40)

#define MT7601_ANDES_VCO_RESET(__pAd)				\
	{													\
		BANK_RF_REG_PAIR __MT7601_RF_VCO_RESET_CR[] = {	\
			{RF_BANK0, RF_R04, 0x0A},						\
			{RF_BANK0, RF_R05, 0x20}						\
		};												\
		RF_RANDOM_WRITE(__pAd, __MT7601_RF_VCO_RESET_CR, 2);		\
	}

#define MT7601_ANDES_SET_CHANNEL_CR(__pAd, __rf_r17, __rf_r18, __rf_r19, __rf_r20)	\
	{													\
		BANK_RF_REG_PAIR __MT7601_RF_CHANNEL_CR[] = {		\
			{RF_BANK0, RF_R17, __rf_r17},					\
			{RF_BANK0, RF_R18, __rf_r18},					\
			{RF_BANK0, RF_R19, __rf_r19},					\
			{RF_BANK0, RF_R20, __rf_r20}					\
		};												\
		RF_RANDOM_WRITE(__pAd, __MT7601_RF_CHANNEL_CR, 4);		\
	}

enum TEMPERATURE_MODE {
	TEMPERATURE_MODE_NORMAL,
	TEMPERATURE_MODE_LOW,
	TEMPERATURE_MODE_HIGH,
};

#ifdef RTMP_INTERNAL_TX_ALC
VOID MT7601_TssiDcGainCalibration(struct _RTMP_ADAPTER *pAd);

typedef struct _MT7601_TX_ALC_DATA {
	INT32	PowerDiffPre;
	INT32	MT7601_TSSI_T0_Delta_Offset;
	INT16	TSSI_DBOFFSET_HVGA;
	INT16	TSSI0_DB;
	UCHAR	TssiSlope;
	CHAR	TssiDC0;
	CHAR	TssiDC0_HVGA;
	UINT32	InitTxAlcCfg1;
	BOOLEAN	TSSI_USE_HVGA;
	BOOLEAN TssiTriggered;
	CHAR	MT7601_TSSI_OFFSET[3];
} MT7601_TX_ALC_DATA, *PMT7601_TX_ALC_DATA;

#endif /* RTMP_INTERNAL_TX_ALC */


/* 
 * Frequency plan item  for MT7601
 * K_R17[7:0]: sdm_k[7:0]
 * K_R18[7:0]: sdm_k[15:8]
 * K_R19[1:0]: sdm_k[17:16]
 * K_R19[3]: sdm_clk_sel
 * R_R20[7:0]: sdm_n[7:0]
 */
typedef struct _MT7601_FREQ_ITEM {
	UINT8 Channel;
	UINT8 K_R17;
	UINT8 K_R18;
	UINT8 K_R19;
	UINT8 N_R20;
} MT7601_FREQ_ITEM;

#define RF_G_BAND 		0x01
#define RF_A_BAND 		0x02
#define RF_A_BAND_LB	0x04
#define RF_A_BAND_MB	0x08
#define RF_A_BAND_HB	0x10
typedef struct _RT6590_RF_SWITCH_ITEM {
	UCHAR Bank;
	UCHAR Register;
	UCHAR Band; /* G_Band, A_Band_LB, A_Band_MB, A_Band_HB */
	UCHAR BW;
	UCHAR Value;
} RT6590_RF_SWITCH_ITEM, *PRT6590_RF_SWITCH_ITEM;

VOID MT7601_Init(struct _RTMP_ADAPTER *pAd);
VOID MT7601_RXDC_CAL(struct _RTMP_ADAPTER *pAd);
INT MT7601_ReadChannelPwr(struct _RTMP_ADAPTER *pAd);
VOID MT7601_ReadTxPwrPerRate(struct _RTMP_ADAPTER *pAd);
VOID MT7601_INIT_CAL(struct _RTMP_ADAPTER *pAd);
INT MT7601DisableTxRx(struct _RTMP_ADAPTER *pAd, UCHAR Level);
VOID MT7601AsicTemperatureCompensation(IN struct _RTMP_ADAPTER *pAd, IN BOOLEAN bPowerOn);
#ifdef RTMP_INTERNAL_TX_ALC
VOID MT7601_EnableTSSI(struct _RTMP_ADAPTER *pAd);
#endif /* RTMP_INTERNAL_TX_ALC */

#if defined(RTMP_INTERNAL_TX_ALC) || defined(SINGLE_SKU_V2)
VOID MT7601_InitPAModeTable(struct _RTMP_ADAPTER *pAd);
#endif /* defined(RTMP_INTERNAL_TX_ALC) || defined(SINGLE_SKU_V2) */

#ifdef MICROWAVE_OVEN_SUPPORT
VOID MT7601_AsicMitigateMicrowave(
	IN struct _RTMP_ADAPTER *pAd);

VOID MT7601_AsicMeasureFalseCCA(
	IN struct _RTMP_ADAPTER *pAd);
#endif /* MICROWAVE_OVEN_SUPPORT */

INT MT7601_Read_Temperature(
	struct _RTMP_ADAPTER *pAd, 
	OUT	CHAR*			Temperature);

INT MT7601_Bootup_Read_Temperature(
	struct _RTMP_ADAPTER *pAd, 
	OUT	CHAR*			Temperature);

VOID MT7601SetRxAnt(
	struct _RTMP_ADAPTER *pAd,
	IN UCHAR			Ant);

#ifdef ED_MONITOR
INT MT7601_set_ed_cca(struct _RTMP_ADAPTER *pAd, BOOLEAN enable);
#endif /* ED_MONITOR */

#endif /* __MT7601_H__ */

