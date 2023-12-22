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
	mt76x0.c

	Abstract:
	Specific funcitons and configurations for MT76x0

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/

#include "rt_config.h"

#if defined(MT7650) || defined(MT7630)
#include "mcu/mt7650_firmware.h"
#endif

#ifdef MT7610
#include "mcu/mt7610_firmware.h"
#endif

#define CSR_EE_CFG1 0x0104
#define IOCFG_6  	0x0124

#ifdef SINGLE_SKU_V2
#define MT76x0_RF_2G_PA_MODE0_DECODE		0
#define MT76x0_RF_2G_PA_MODE1_DECODE		29491	// 3.6 * 8192
#define MT76x0_RF_2G_PA_MODE3_DECODE		4096	// 0.5 * 8192

#define MT76x0_RF_5G_PA_MODE0_DECODE		0
#define MT76x0_RF_5G_PA_MODE1_DECODE		0
#endif /* SINGLE_SKU_V2 */

#define EEPROM_TXPOWER_BYRATE_STBC			0xEC
#define EEPROM_TXPOWER_BYRATE_5G			0x120

/*
	VHT BW80 delta power control (+4~-4dBm) for per-rate Tx power control
*/
#define EEPROM_VHT_BW80_TX_POWER_DELTA	0xD3

UCHAR MT76x0_EeBuffer[EEPROM_SIZE] = {
	0x83, 0x38, 0x01, 0x00, 0x00, 0x0c, 0x43, 0x28, 0x83, 0x00, 0x83, 0x28, 0x14, 0x18, 0xff, 0xff,
	0xff, 0xff, 0x83, 0x28, 0x14, 0x18, 0x00, 0x00, 0x01, 0x00, 0x6a, 0xff, 0x00, 0x02, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x0c, 0x43, 0x28, 0x83, 0x01, 0x00, 0x0c,
	0x43, 0x28, 0x83, 0x02, 0x33, 0x0a, 0xec, 0x00, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0x20, 0x01, 0x55, 0x77, 0xa8, 0xaa, 0x8c, 0x88, 0xff, 0xff, 0x0a, 0x08, 0x08, 0x06,
	0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x66, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa,
	0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xaa, 0xaa,
	0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa,
	0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xaa, 0xaa,
	0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	} ;


#define MT7650_EFUSE_CTRL	0x0024
#define LDO_CTRL1			0x0070

enum CALIBRATION_CHANNEL_LIST {
	G_BAND_CH = 7,
	A_BAND_LOW_CH = 42,
	A_BAND_MID_CH = 136,
	A_BAND_HIGH_CH = 155,
};

static RTMP_REG_PAIR	MT76x0_MACRegTable[] = {
#if 0
	{CSR_EE_CFG1,		0x27FFFF},
#endif
	{IOCFG_6,			0xA0040080},
	{PBF_SYS_CTRL,		0x80c00},
	{0x80,				0xFF000403},
	{0x20, 				0x00E00FFF},
	{MAC_SYS_CTRL,		0xC},
#if 0
	{RLT_PBF_CFG,			0x1f},
#else
	{RLT_PBF_CFG,			0x77723c1f},
#endif
	{FCE_PSE_CTRL,		0x1},

	/* Keep the default setting of 0x1018 for MT7650. @20120830 */
	/* {MAX_LEN_CFG,			MAX_AGGREGATION_SIZE | 0x00001000},	*/
	
	{PWR_PIN_CFG,		0x0},

	{AMPDU_MAX_LEN_20M1S,	0xBAA99887}, /* Recommended by JerryCK @20120905 */

	{TX_SW_CFG0,		0x601}, /* Delay bb_tx_pe for proper tx_mcs_pwr update */
	{TX_SW_CFG1,		0x00040000}, /* Set rf_tx_pe deassert time to 1us by Chee's comment @MT7650_CR_setting_1018.xlsx */
	{TX_SW_CFG2,		0x0},

// TODO: shiang-6590, check what tx report will send to us when following default value set as 2
	{0xa44,					0x0}, /* disable Tx info report */

#if defined(CONFIG_TSO_SUPPORT) || defined(CONFIG_CSO_SUPPORT)
	{0x80c,				0x8},
#endif /* defined(CONFIG_TSO_SUPPORT) || defined(CONFIG_CSO_SUPPORT) */

#ifdef HDR_TRANS_SUPPORT
	{HEADER_TRANS_CTRL_REG, 0x2}, /* 0x1: TX, 0x2: RX */
	{TSO_CTRL, 			0x7050},
#else
	{HEADER_TRANS_CTRL_REG, 0x0},
	{TSO_CTRL, 			0x0},
#endif /* HDR_TRANS_SUPPORT */


	/* BB_PA_MODE_CFG0(0x1214) Keep default value @20120903 */
	{BB_PA_MODE_CFG1, 0x00500055},

	/* RF_PA_MODE_CFG0(0x121C) Keep default value @20120903 */
	{RF_PA_MODE_CFG1, 0x00500055},

	{TX_ALC_CFG_0, 0x2F2F000C},
	{TX0_BB_GAIN_ATTEN, 0x00000000}, /* set BBP atten gain = 0 */

	{0x150C, 0x00000002}, /* Enable Tx length > 4095 byte */
	{0x1238, 0x001700C8}, /* Disable bt_abort_tx_en(0x1238[21] = 0) which is not used at MT7650 @MT7650_E3_CR_setting_1115.xlsx */
	{LDO_CTRL1, 0x6B006464}, /* Default LDO_DIG supply 1.26V, change to 1.2V */
};

static UCHAR MT76x0_NUM_MAC_REG_PARMS = (sizeof(MT76x0_MACRegTable) / sizeof(RTMP_REG_PAIR));

static RTMP_REG_PAIR MT76x0_DCOC_Tab[] = {
	{CAL_R47, 0x000010F0},
	{CAL_R48, 0x00008080},
	{CAL_R49, 0x00000F07},
	{CAL_R50, 0x00000040},
	{CAL_R51, 0x00000404},
	{CAL_R52, 0x00080803},
	{CAL_R53, 0x00000704},
	{CAL_R54, 0x00002828},
	{CAL_R55, 0x00005050},
};
static UCHAR MT76x0_DCOC_Tab_Size = (sizeof(MT76x0_DCOC_Tab) / sizeof(RTMP_REG_PAIR));

static RTMP_REG_PAIR MT76x0_BBP_Init_Tab[] = {
	{CORE_R1, 0x00000002},
	{CORE_R4, 0x00000000},
	{CORE_R24, 0x00000000},
	{CORE_R32, 0x4003000a},
	{CORE_R42, 0x00000000},
	{CORE_R44, 0x00000000},

#if 0
	/* Disable Tx FIFO empty check for hang issue - MT7650E3_BBP_CR_20121225 */
#endif
	{IBI_R11, 0x00000080},
	
	/*
		0x2300[5] Default Antenna:
		0 for WIFI main antenna
		1  for WIFI aux  antenna

	*/
	{AGC1_R0, 0x00021400},
	{AGC1_R1, 0x00000003},
	{AGC1_R2, 0x003A6464},
	{AGC1_R15, 0x88A28CB8},
	{AGC1_R22, 0x00001E21},
	{AGC1_R23, 0x0000272C},
	{AGC1_R24, 0x00002F3A},
	{AGC1_R25, 0x8000005A},
    	{AGC1_R26, 0x007C2005},
	{AGC1_R34, 0x000A0C0C},
	{AGC1_R37, 0x2121262C},
	{AGC1_R41, 0x38383E45},
	{AGC1_R57, 0x00001010},
	{AGC1_R59, 0xBAA20E96},
	{AGC1_R63, 0x00000001},

	{TXC_R0, 0x00280403},
	{TXC_R1, 0x00000000},

	{RXC_R1, 0x00000012},
	{RXC_R2, 0x00000011},
	{RXC_R3, 0x00000005},
	{RXC_R4, 0x00000000},
	{RXC_R5, 0xF977C4EC},
	{RXC_R7, 0x00000090},

	{TXO_R8, 0x00000000},

	{TXBE_R0, 0x00000000},
	{TXBE_R4, 0x00000004},
	{TXBE_R6, 0x00000000},
	{TXBE_R8, 0x00000014},
	{TXBE_R9, 0x20000000},
	{TXBE_R10, 0x00000000},
	{TXBE_R12, 0x00000000},
	{TXBE_R13, 0x00000000},
	{TXBE_R14, 0x00000000},
	{TXBE_R15, 0x00000000},
	{TXBE_R16, 0x00000000},
	{TXBE_R17, 0x00000000},

	{RXFE_R1, 0x00008800}, /* Add for E3 */
	{RXFE_R3, 0x00000000},
	{RXFE_R4, 0x00000000},

	{RXO_R13, 0x00000092},
	{RXO_R14, 0x00060612},
	{RXO_R15, 0xC8321B18},
	{RXO_R16, 0x0000001E},
	{RXO_R17, 0x00000000},
	{RXO_R18, 0xCC00A993},
	{RXO_R19, 0xB9CB9CB9},
	{RXO_R21, 0x00000001},
	{RXO_R24, 0x00000006},
};
static UCHAR MT76x0_BBP_Init_Tab_Size = (sizeof(MT76x0_BBP_Init_Tab) / sizeof(RTMP_REG_PAIR));

MT76x0_BBP_Table MT76x0_BPP_SWITCH_Tab[] = {

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R8, 0x0E344EF0}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R8, 0x122C54F2}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R14, 0x310F2E39}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R14, 0x310F2A3F}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R32, 0x00003230}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R32, 0x0000181C}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R33, 0x00003240}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R33, 0x00003218}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R35, 0x11112016}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R35, 0x11112016}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{RXO_R28, 0x0000008A}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{RXO_R28, 0x0000008A}},	
	
	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R4, 0x1FEDA049}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R4, 0x1FECA054}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R6, 0x00000045}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R6, 0x0000000A}},

	{RF_G_BAND | RF_BW_20,							{AGC1_R12, 0x05052879}},
	{RF_G_BAND | RF_BW_40,							{AGC1_R12, 0x050528F9}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R12, 0x050528F9}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R13, 0x35050004}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R13, 0x2C3A0406}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R27, 0x000000E1}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R27, 0x000000EC}},

	{RF_G_BAND | RF_BW_20,							{AGC1_R28, 0x00060806}},
	{RF_G_BAND | RF_BW_40,							{AGC1_R28, 0x00050806}},
	{RF_A_BAND | RF_BW_40,							{AGC1_R28, 0x00060801}},
	{RF_A_BAND | RF_BW_20 | RF_BW_80,				{AGC1_R28, 0x00060806}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R31, 0x00000F23}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R31, 0x00000F13}},

	{RF_G_BAND | RF_BW_20,							{AGC1_R39, 0x2A2A3036}},
	{RF_G_BAND | RF_BW_40,							{AGC1_R39, 0x2A2A2C36}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R39, 0x2A2A3036}},
	{RF_A_BAND | RF_BW_80,							{AGC1_R39, 0x2A2A2A36}},
	
	{RF_G_BAND | RF_BW_20,							{AGC1_R43, 0x27273438}},
	{RF_G_BAND | RF_BW_40,							{AGC1_R43, 0x27272D38}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R43, 0x27272B30}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R51, 0x17171C1C}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R51, 0xFFFFFFFF}},

	{RF_G_BAND | RF_BW_20,							{AGC1_R53, 0x26262A2F}},
	{RF_G_BAND | RF_BW_40,							{AGC1_R53, 0x2626322F}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R53, 0xFFFFFFFF}},

	{RF_G_BAND | RF_BW_20,							{AGC1_R55, 0x40404E58}},
	{RF_G_BAND | RF_BW_40,							{AGC1_R55, 0x40405858}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R55, 0xFFFFFFFF}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{AGC1_R58, 0x00001010}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{AGC1_R58, 0x00000000}},

	{RF_G_BAND | RF_BW_20 | RF_BW_40,				{RXFE_R0, 0x3D5000E0}},
	{RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{RXFE_R0, 0x895000E0}},
};

UCHAR MT76x0_BPP_SWITCH_Tab_Size = (sizeof(MT76x0_BPP_SWITCH_Tab) / sizeof(MT76x0_BBP_Table));

/* Bank	Register Value(Hex) */
static BANK_RF_REG_PAIR MT76x0_RF_Central_RegTb[] = {
/*
	Bank 0 - For central blocks: BG, PLL, XTAL, LO, ADC/DAC
*/
	{RF_BANK0,	RF_R01, 0x01},
	{RF_BANK0,	RF_R02, 0x11},

	/*
		R3 ~ R7: VCO Cal.
	*/	
	{RF_BANK0,	RF_R03, 0x73}, /* VCO Freq Cal - No Bypass, VCO Amp Cal - No Bypass */
	{RF_BANK0,	RF_R04, 0x30}, /* R4 b<7>=1, VCO cal */
	{RF_BANK0,	RF_R05, 0x00},
	{RF_BANK0,	RF_R06, 0x41}, /* Set the open loop amplitude to middle since bypassing amplitude calibration */
	{RF_BANK0,	RF_R07, 0x00},

	/*
		XO
	*/
	{RF_BANK0,	RF_R08, 0x00}, 
	{RF_BANK0,	RF_R09, 0x00},
	{RF_BANK0,	RF_R10, 0x0C},
	{RF_BANK0,	RF_R11, 0x00},
	{RF_BANK0,	RF_R12, 0x00},

	/*
		BG
	*/
	{RF_BANK0,	RF_R13, 0x00},
	{RF_BANK0,	RF_R14, 0x00},
	{RF_BANK0,	RF_R15, 0x00},

	/*
		LDO
	*/
	{RF_BANK0,	RF_R19, 0x20}, 
	/*
		XO
	*/
	{RF_BANK0,	RF_R20, 0x22},
	{RF_BANK0,	RF_R21, 0x12},
#if 0
	{RF_BANK0,	RF_R22, 0x26/*0x3F*/}, /* XTAL Freq offset, varies */
#endif
	{RF_BANK0,	RF_R23, 0x00},
	{RF_BANK0,	RF_R24, 0x33}, /* See band selection for R24<1:0> */
	{RF_BANK0,	RF_R25, 0x00},

	/*
		PLL, See Freq Selection
	*/
	{RF_BANK0,	RF_R26, 0x00},
	{RF_BANK0,	RF_R27, 0x00},
	{RF_BANK0,	RF_R28, 0x00},
	{RF_BANK0,	RF_R29, 0x00},
	{RF_BANK0,	RF_R30, 0x00},
	{RF_BANK0,	RF_R31, 0x00},
	{RF_BANK0,	RF_R32, 0x00},
	{RF_BANK0,	RF_R33, 0x00},
	{RF_BANK0,	RF_R34, 0x00},
	{RF_BANK0,	RF_R35, 0x00},
	{RF_BANK0,	RF_R36, 0x00},
	{RF_BANK0,	RF_R37, 0x00},

	/*
		LO Buffer
	*/
	{RF_BANK0,	RF_R38, 0x2F},
	
	/*
		Test Ports
	*/
	{RF_BANK0,	RF_R64, 0x00},
	{RF_BANK0,	RF_R65, 0x80},
	{RF_BANK0,	RF_R66, 0x01},
	{RF_BANK0,	RF_R67, 0x04},

	/*
		ADC/DAC
	*/
	{RF_BANK0,	RF_R68, 0x00},
	{RF_BANK0,	RF_R69, 0x08},
	{RF_BANK0,	RF_R70, 0x08},
	{RF_BANK0,	RF_R71, 0x40},
	{RF_BANK0,	RF_R72, 0xD0},
	{RF_BANK0,	RF_R73, 0x93},
};
static UINT32 MT76x0_RF_Central_RegTb_Size = (sizeof(MT76x0_RF_Central_RegTb) / sizeof(BANK_RF_REG_PAIR));

static BANK_RF_REG_PAIR MT76x0_RF_2G_Channel_0_RegTb[] = {
/*
	Bank 5 - Channel 0 2G RF registers	
*/
	/*
		RX logic operation
	*/
	/* RF_R00 Change in SelectBand6590 */
	{RF_BANK5,	RF_R03, 0x00},

	/*
		TX logic operation
	*/
	{RF_BANK5,	RF_R04, 0x00},
	{RF_BANK5,	RF_R05, 0x84},
	{RF_BANK5,	RF_R06, 0x02},

	/*
		LDO
	*/
	{RF_BANK5,	RF_R07, 0x00},
	{RF_BANK5,	RF_R08, 0x00},
	{RF_BANK5,	RF_R09, 0x00},

	/*
		RX
	*/
	{RF_BANK5,	RF_R10, 0x51},
	{RF_BANK5,	RF_R11, 0x22},
	{RF_BANK5,	RF_R12, 0x22},
	{RF_BANK5,	RF_R13, 0x0F},
	{RF_BANK5,	RF_R14, 0x47}, /* Increase mixer current for more gain */
	{RF_BANK5,	RF_R15, 0x25},
	{RF_BANK5,	RF_R16, 0xC7}, /* Tune LNA2 tank */
	{RF_BANK5,	RF_R17, 0x00},
	{RF_BANK5,	RF_R18, 0x00},
	{RF_BANK5,	RF_R19, 0x30}, /* Improve max Pin */
	{RF_BANK5,	RF_R20, 0x33},
	{RF_BANK5,	RF_R21, 0x02},
	{RF_BANK5,	RF_R22, 0x32}, /* Tune LNA1 tank */
	{RF_BANK5,	RF_R23, 0x00},
	{RF_BANK5,	RF_R24, 0x25},
#if 0 /* R25 is used for BT. Let BT driver write it. */
	{RF_BANK5,	RF_R25, 0x13},
#endif
	{RF_BANK5,	RF_R26, 0x00},
	{RF_BANK5,	RF_R27, 0x12},
	{RF_BANK5,	RF_R28, 0x0F},
	{RF_BANK5,	RF_R29, 0x00},

	/*
		LOGEN
	*/
	{RF_BANK5,	RF_R30, 0x51}, /* Tune LOGEN tank */
	{RF_BANK5,	RF_R31, 0x35},
	{RF_BANK5,	RF_R32, 0x31},
	{RF_BANK5,	RF_R33, 0x31},
	{RF_BANK5,	RF_R34, 0x34},
	{RF_BANK5,	RF_R35, 0x03},
	{RF_BANK5,	RF_R36, 0x00},

	/*
		TX
	*/
	{RF_BANK5,	RF_R37, 0xDD}, /* Improve 3.2GHz spur */
	{RF_BANK5,	RF_R38, 0xB3},
	{RF_BANK5,	RF_R39, 0x33},
	{RF_BANK5,	RF_R40, 0xB1},
	{RF_BANK5,	RF_R41, 0x71},
	{RF_BANK5,	RF_R42, 0xF2},
	{RF_BANK5,	RF_R43, 0x47},
	{RF_BANK5,	RF_R44, 0x77},
	{RF_BANK5,	RF_R45, 0x0E},
	{RF_BANK5,	RF_R46, 0x10},
	{RF_BANK5,	RF_R47, 0x00},
	{RF_BANK5,	RF_R48, 0x53},
	{RF_BANK5,	RF_R49, 0x03},
	{RF_BANK5,	RF_R50, 0xEF},
	{RF_BANK5,	RF_R51, 0xC7},
	{RF_BANK5,	RF_R52, 0x62},
	{RF_BANK5,	RF_R53, 0x62},
	{RF_BANK5,	RF_R54, 0x00},
	{RF_BANK5,	RF_R55, 0x00},
	{RF_BANK5,	RF_R56, 0x0F},
	{RF_BANK5,	RF_R57, 0x0F},
	{RF_BANK5,	RF_R58, 0x16},
	{RF_BANK5,	RF_R59, 0x16},
	{RF_BANK5,	RF_R60, 0x10},
	{RF_BANK5,	RF_R61, 0x10},
	{RF_BANK5,	RF_R62, 0xD0},
	{RF_BANK5,	RF_R63, 0x6C},
	{RF_BANK5,	RF_R64, 0x58},
	{RF_BANK5, 	RF_R65, 0x58},
	{RF_BANK5,	RF_R66, 0xF2},
	{RF_BANK5,	RF_R67, 0xE8},
	{RF_BANK5,	RF_R68, 0xF0},
	{RF_BANK5,	RF_R69, 0xF0},
	{RF_BANK5,	RF_R127, 0x04},
};
static UINT32 MT76x0_RF_2G_Channel_0_RegTb_Size = (sizeof(MT76x0_RF_2G_Channel_0_RegTb) / sizeof(BANK_RF_REG_PAIR));

static BANK_RF_REG_PAIR MT76x0_RF_5G_Channel_0_RegTb[] = {
/*
	Bank 6 - Channel 0 5G RF registers	
*/
	/*
		RX logic operation
	*/
	/* RF_R00 Change in mt76x0_band_select */

	{RF_BANK6,	RF_R02, 0x0C},
	{RF_BANK6,	RF_R03, 0x00},

	/*
		TX logic operation
	*/
	{RF_BANK6,	RF_R04, 0x00},
	{RF_BANK6,	RF_R05, 0x84},
	{RF_BANK6,	RF_R06, 0x02},

	/*
		LDO
	*/
	{RF_BANK6,	RF_R07, 0x00},
	{RF_BANK6,	RF_R08, 0x00},
	{RF_BANK6,	RF_R09, 0x00},

	/*
		RX
	*/
	{RF_BANK6,	RF_R10, 0x00},
	{RF_BANK6,	RF_R11, 0x01},
	
	{RF_BANK6,	RF_R13, 0x23},
	{RF_BANK6,	RF_R14, 0x00},
	{RF_BANK6,	RF_R15, 0x04},
	{RF_BANK6,	RF_R16, 0x22},

	{RF_BANK6,	RF_R18, 0x08},
	{RF_BANK6,	RF_R19, 0x00},
	{RF_BANK6,	RF_R20, 0x00},
	{RF_BANK6,	RF_R21, 0x00},
	{RF_BANK6,	RF_R22, 0xFB},

	/*
		LOGEN5G
	*/
	{RF_BANK6,	RF_R25, 0x76},
	{RF_BANK6,	RF_R26, 0x24},
	{RF_BANK6,	RF_R27, 0x04},
	{RF_BANK6,	RF_R28, 0x00},
	{RF_BANK6,	RF_R29, 0x00},

	/*
		TX
	*/
	{RF_BANK6,	RF_R37, 0xBB},
	{RF_BANK6,	RF_R38, 0xB3},

	{RF_BANK6,	RF_R40, 0x33},
	{RF_BANK6,	RF_R41, 0x33},
	
	{RF_BANK6,	RF_R43, 0x03},
	{RF_BANK6,	RF_R44, 0xB3},
	
	{RF_BANK6,	RF_R46, 0x17},
	{RF_BANK6,	RF_R47, 0x0E},
	{RF_BANK6,	RF_R48, 0x10},
	{RF_BANK6,	RF_R49, 0x07},
	
	{RF_BANK6,	RF_R62, 0x00},
	{RF_BANK6,	RF_R63, 0x00},
	{RF_BANK6,	RF_R64, 0xF1},
	{RF_BANK6,	RF_R65, 0x0F},
};
static UINT32 MT76x0_RF_5G_Channel_0_RegTb_Size = (sizeof(MT76x0_RF_5G_Channel_0_RegTb) / sizeof(BANK_RF_REG_PAIR));

static BANK_RF_REG_PAIR MT76x0_RF_VGA_Channel_0_RegTb[] = {
/*
	Bank 7 - Channel 0 VGA RF registers	
*/
	/* E3 CR */
	{RF_BANK7,	RF_R00, 0x47}, /* Allow BBP/MAC to do calibration */
	{RF_BANK7,	RF_R01, 0x00},
	{RF_BANK7,	RF_R02, 0x00},
	{RF_BANK7,	RF_R03, 0x00},
	{RF_BANK7,	RF_R04, 0x00},

	{RF_BANK7,	RF_R10, 0x13},
	{RF_BANK7,	RF_R11, 0x0F},
	{RF_BANK7,	RF_R12, 0x13}, /* For DCOC */
	{RF_BANK7,	RF_R13, 0x13}, /* For DCOC */
	{RF_BANK7,	RF_R14, 0x13}, /* For DCOC */
	{RF_BANK7,	RF_R15, 0x20}, /* For DCOC */
	{RF_BANK7,	RF_R16, 0x22}, /* For DCOC */

	{RF_BANK7,	RF_R17, 0x7C},

	{RF_BANK7,	RF_R18, 0x00},
	{RF_BANK7,	RF_R19, 0x00},
	{RF_BANK7,	RF_R20, 0x00},
	{RF_BANK7,	RF_R21, 0xF1},
	{RF_BANK7,	RF_R22, 0x11},
	{RF_BANK7,	RF_R23, 0xC2},
	{RF_BANK7,	RF_R24, 0x41},
	{RF_BANK7,	RF_R25, 0x20},
	{RF_BANK7,	RF_R26, 0x40},
	{RF_BANK7,	RF_R27, 0xD7},
	{RF_BANK7,	RF_R28, 0xA2},
	{RF_BANK7,	RF_R29, 0x60},
	{RF_BANK7,	RF_R30, 0x49},
	{RF_BANK7,	RF_R31, 0x20},
	{RF_BANK7,	RF_R32, 0x44},
	{RF_BANK7,	RF_R33, 0xC1},
	{RF_BANK7,	RF_R34, 0x60},
	{RF_BANK7,	RF_R35, 0xC0},

	{RF_BANK7,	RF_R61, 0x01},

	{RF_BANK7,	RF_R72, 0x3C},
	{RF_BANK7,	RF_R73, 0x34},
	{RF_BANK7,	RF_R74, 0x00},
};
static UINT32 MT76x0_RF_VGA_Channel_0_RegTb_Size = (sizeof(MT76x0_RF_VGA_Channel_0_RegTb) / sizeof(BANK_RF_REG_PAIR));

static MT76x0_FREQ_ITEM MT76x0_Frequency_Plan[] =
{
	{1,		RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xE2, 0x40, 0x02, 0x40, 0x02, 0, 0, 1, 0x28, 0, 0x30, 0, 0, 0x3}, /* Freq 2412 */
	{2, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xE4, 0x40, 0x07, 0x40, 0x02, 0, 0, 1, 0xA1, 0, 0x30, 0, 0, 0x1}, /* Freq 2417 */
	{3, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xE2, 0x40, 0x07, 0x40, 0x0B, 0, 0, 1, 0x50, 0, 0x30, 0, 0, 0x0}, /* Freq 2422 */
	{4, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xD4, 0x40, 0x02, 0x40, 0x09, 0, 0, 1, 0x50, 0, 0x30, 0, 0, 0x0}, /* Freq 2427 */
	{5, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x02, 0, 0, 1, 0xA2, 0, 0x30, 0, 0, 0x1}, /* Freq 2432 */
	{6, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x07, 0, 0, 1, 0xA2, 0, 0x30, 0, 0, 0x1}, /* Freq 2437 */
	{7, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xE2, 0x40, 0x02, 0x40, 0x07, 0, 0, 1, 0x28, 0, 0x30, 0, 0, 0x3}, /* Freq 2442 */
	{8, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x02, 0, 0, 1, 0xA3, 0, 0x30, 0, 0, 0x1}, /* Freq 2447 */
	{9, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xF2, 0x40, 0x07, 0x40, 0x0D, 0, 0, 1, 0x28, 0, 0x30, 0, 0, 0x3}, /* Freq 2452 */
	{10, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xD4, 0x40, 0x02, 0x40, 0x09, 0, 0, 1, 0x51, 0, 0x30, 0, 0, 0x0}, /* Freq 2457 */
	{11, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x02, 0, 0, 1, 0xA4, 0, 0x30, 0, 0, 0x1}, /* Freq 2462 */
	{12, 	RF_G_BAND,	0x02, 0x3F, 0x3C, 0xDD, 0xD4, 0x40, 0x07, 0x40, 0x07, 0, 0, 1, 0xA4, 0, 0x30, 0, 0, 0x1}, /* Freq 2467 */
	{13, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xF2, 0x40, 0x02, 0x40, 0x02, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 2472 */
	{14, 	RF_G_BAND,	0x02, 0x3F, 0x28, 0xDD, 0xF2, 0x40, 0x02, 0x40, 0x04, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 2484 */

	{183, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x28, 0, 0x30, 0, 0, 0x3}, /* Freq 4915 */
	{184, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x00, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4920 */
	{185, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4925 */
	{187, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4935 */
	{188, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4940 */
	{189, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4945 */
	{192, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4960 */
	{196, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x29, 0, 0x30, 0, 0, 0x3}, /* Freq 4980 */
	
	{36, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5180 */
	{37, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5185 */
	{38, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5190 */
	{39, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5195 */
	{40, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5200 */
	{41, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x09, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5205 */
	{42, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x05, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5210 */
	{43, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0B, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5215 */
	{44, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5220 */
	{45, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0D, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5225 */
	{46, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x07, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5230 */
	{47, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0F, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5235 */
	{48, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x08, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5240 */
	{49, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x11, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5245 */
	{50, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x09, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5250 */
	{51, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x13, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5255 */
	{52, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5260 */
	{53, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5265 */
	{54, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x0B, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5270 */
	{55, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x2B, 0, 0x30, 0, 0, 0x3}, /* Freq 5275 */
	{56, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x00, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5280 */
	{57, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5285 */
	{58, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x01, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5290 */
	{59, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5295 */
	{60, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5300 */
	{61, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5305 */
	{62, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5310 */
	{63, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5315 */
	{64, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x2C, 0, 0x30, 0, 0, 0x3}, /* Freq 5320 */

	{100, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x2D, 0, 0x30, 0, 0, 0x3}, /* Freq 5500 */
	{101, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x2D, 0, 0x30, 0, 0, 0x3}, /* Freq 5505 */
	{102, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x0B, 0, 0, 1, 0x2D, 0, 0x30, 0, 0, 0x3}, /* Freq 5510 */
	{103, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x2D, 0, 0x30, 0, 0, 0x3}, /* Freq 5515 */
	{104, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x00, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5520 */
	{105, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5525 */
	{106, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x01, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5530 */
	{107, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5535 */
	{108, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5540 */
	{109, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5545 */
	{110, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5550 */
	{111, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5555 */
	{112, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5560 */
	{113, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x09, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5565 */
	{114, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x05, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5570 */
	{115, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0B, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5575 */
	{116, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5580 */
	{117, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0D, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5585 */
	{118, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x07, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5590 */
	{119, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0F, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5595 */
	{120, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x08, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5600 */
	{121, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x11, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5605 */
	{122, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x09, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5610 */
	{123, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x13, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5615 */
	{124, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5620 */
	{125, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5625 */
	{126, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x0B, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5630 */
	{127, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x2E, 0, 0x30, 0, 0, 0x3}, /* Freq 5635 */
	{128, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x00, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5640 */
	{129, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5645 */
	{130, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x01, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5650 */
	{131, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5655 */
	{132, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5660 */
	{133, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5665 */
	{134, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5670 */
	{135, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5675 */
	{136, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5680 */

	{137, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x09, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5685 */
	{138, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x05, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5690 */
	{139, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0B, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5695 */
	{140, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5700 */
	{141, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0D, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5705 */
	{142, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x07, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5710 */	
	{143, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0F, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5715 */
	{144, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x08, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5720 */
	{145, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x11, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5725 */
	{146, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x09, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5730 */
	{147, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x13, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5735 */
	{148, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5740 */
	{149, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5745 */
	{150, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x0B, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5750 */	
	{151, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x70, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x17, 0, 0, 1, 0x2F, 0, 0x30, 0, 0, 0x3}, /* Freq 5755 */
	{152, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x00, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5760 */
	{153, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x01, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5765 */
	{154, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x01, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5770 */
	{155, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x03, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5775 */
	{156, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x02, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5780 */
	{157, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x05, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5785 */
	{158, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x03, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5790 */
	{159, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x07, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5795 */
	{160, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x04, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5800 */
	{161, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x09, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5805 */
	{162, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x05, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5810 */
	{163, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0B, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5815 */
	{164, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x06, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5820 */
	{165, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0D, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5825 */
	{166, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0xDD, 0xD2, 0x40, 0x04, 0x40, 0x07, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5830 */
	{167, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x0F, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5835 */
	{168, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x08, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5840 */
	{169, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x11, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5845 */
	{170, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x09, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5850 */
	{171, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x13, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5855 */
	{172, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x30, 0x97, 0xD2, 0x40, 0x04, 0x40, 0x0A, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5860 */
	{173, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x68, 0xDD, 0xD2, 0x40, 0x10, 0x40, 0x15, 0, 0, 1, 0x30, 0, 0x30, 0, 0, 0x3}, /* Freq 5865 */
};
UCHAR NUM_OF_MT76x0_CHNL = (sizeof(MT76x0_Frequency_Plan) / sizeof(MT76x0_FREQ_ITEM));


static MT76x0_FREQ_ITEM MT76x0_SDM_Frequency_Plan[] =
{
	{1,		RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0xCCCC,  0x3}, /* Freq 2412 */
	{2, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x12222, 0x3}, /* Freq 2417 */
	{3, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x17777, 0x3}, /* Freq 2422 */
	{4, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x1CCCC, 0x3}, /* Freq 2427 */
	{5, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x22222, 0x3}, /* Freq 2432 */
	{6, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x27777, 0x3}, /* Freq 2437 */
	{7, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x2CCCC, 0x3}, /* Freq 2442 */
	{8, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x32222, 0x3}, /* Freq 2447 */
	{9, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x37777, 0x3}, /* Freq 2452 */
	{10, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x3CCCC, 0x3}, /* Freq 2457 */
	{11, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x2222, 0x3}, /* Freq 2462 */
	{12, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x7777, 0x3}, /* Freq 2467 */
	{13, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0xCCCC, 0x3}, /* Freq 2472 */
	{14, 	RF_G_BAND,	0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x19999, 0x3}, /* Freq 2484 */

	{183, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x28, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 4915 */
	{184, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x0,     0x3}, /* Freq 4920 */
	{185, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x2AAA,  0x3}, /* Freq 4925 */
	{187, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x8000,  0x3}, /* Freq 4935 */
	{188, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0xAAAA,  0x3}, /* Freq 4940 */
	{189, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0xD555,  0x3}, /* Freq 4945 */
	{192, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 4960 */
	{196, 	(RF_A_BAND | RF_A_BAND_11J), 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x29, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 4980 */
	
	{36, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0xAAAA,  0x3}, /* Freq 5180 */
	{37, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0xD555,  0x3}, /* Freq 5185 */
	{38, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5190 */
	{39, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5195 */
	{40, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5200 */
	{41, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x18000, 0x3}, /* Freq 5205 */
	{42, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x1AAAA, 0x3}, /* Freq 5210 */
	{43, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x1D555, 0x3}, /* Freq 5215 */
	{44, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 5220 */
	{45, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x22AAA, 0x3}, /* Freq 5225 */
	{46, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x25555, 0x3}, /* Freq 5230 */
	{47, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x28000, 0x3}, /* Freq 5235 */
	{48, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x2AAAA, 0x3}, /* Freq 5240 */
	{49, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x2D555, 0x3}, /* Freq 5245 */
	{50, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x30000, 0x3}, /* Freq 5250 */
	{51, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x32AAA, 0x3}, /* Freq 5255 */
	{52, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5260 */
	{53, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5265 */
	{54, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x3AAAA, 0x3}, /* Freq 5270 */
	{55, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2B, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 5275 */
	{56, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x00000, 0x3}, /* Freq 5280 */
	{57, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x02AAA, 0x3}, /* Freq 5285 */
	{58, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x05555, 0x3}, /* Freq 5290 */
	{59, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x08000, 0x3}, /* Freq 5295 */
	{60, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x0AAAA, 0x3}, /* Freq 5300 */
	{61, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x0D555, 0x3}, /* Freq 5305 */
	{62, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5310 */
	{63, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5315 */
	{64, 	(RF_A_BAND | RF_A_BAND_LB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2C, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5320 */

	{100, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2D, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5500 */
	{101, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2D, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5505 */
	{102, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2D, 0, 0x0, 0x8, 0x3AAAA, 0x3}, /* Freq 5510 */
	{103, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2D, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 5515 */
	{104, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x00000, 0x3}, /* Freq 5520 */
	{105, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x02AAA, 0x3}, /* Freq 5525 */
	{106, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x05555, 0x3}, /* Freq 5530 */
	{107, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x08000, 0x3}, /* Freq 5535 */
	{108, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x0AAAA, 0x3}, /* Freq 5540 */
	{109, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x0D555, 0x3}, /* Freq 5545 */
	{110, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5550 */
	{111, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5555 */
	{112, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5560 */
	{113, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x18000, 0x3}, /* Freq 5565 */
	{114, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x1AAAA, 0x3}, /* Freq 5570 */
	{115, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x1D555, 0x3}, /* Freq 5575 */
	{116, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 5580 */
	{117, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x22AAA, 0x3}, /* Freq 5585 */
	{118, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x25555, 0x3}, /* Freq 5590 */
	{119, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x28000, 0x3}, /* Freq 5595 */
	{120, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x2AAAA, 0x3}, /* Freq 5600 */
	{121, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x2D555, 0x3}, /* Freq 5605 */
	{122, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x30000, 0x3}, /* Freq 5610 */
	{123, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x32AAA, 0x3}, /* Freq 5615 */
	{124, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5620 */
	{125, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5625 */
	{126, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x3AAAA, 0x3}, /* Freq 5630 */
	{127, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2E, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 5635 */
	{128, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x00000, 0x3}, /* Freq 5640 */
	{129, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x02AAA, 0x3}, /* Freq 5645 */
	{130, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x05555, 0x3}, /* Freq 5650 */
	{131, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x08000, 0x3}, /* Freq 5655 */
	{132, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x0AAAA, 0x3}, /* Freq 5660 */
	{133, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x0D555, 0x3}, /* Freq 5665 */
	{134, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5670 */
	{135, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5675 */
	{136, 	(RF_A_BAND | RF_A_BAND_MB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5680 */

	{137, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x18000, 0x3}, /* Freq 5685 */
	{138, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x1AAAA, 0x3}, /* Freq 5690 */
	{139, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x1D555, 0x3}, /* Freq 5695 */
	{140, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 5700 */
	{141, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x22AAA, 0x3}, /* Freq 5705 */
	{142, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x25555, 0x3}, /* Freq 5710 */	
	{143, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x28000, 0x3}, /* Freq 5715 */
	{144, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x2AAAA, 0x3}, /* Freq 5720 */
	{145, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x2D555, 0x3}, /* Freq 5725 */
	{146, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x30000, 0x3}, /* Freq 5730 */
	{147, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x32AAA, 0x3}, /* Freq 5735 */
	{148, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5740 */
	{149, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5745 */
	{150, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x3AAAA, 0x3}, /* Freq 5750 */	
	{151, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x2F, 0, 0x0, 0x8, 0x3D555, 0x3}, /* Freq 5755 */
	{152, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x00000, 0x3}, /* Freq 5760 */
	{153, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x02AAA, 0x3}, /* Freq 5765 */
	{154, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x05555, 0x3}, /* Freq 5770 */
	{155, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x08000, 0x3}, /* Freq 5775 */
	{156, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x0AAAA, 0x3}, /* Freq 5780 */
	{157, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x0D555, 0x3}, /* Freq 5785 */
	{158, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x10000, 0x3}, /* Freq 5790 */
	{159, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x12AAA, 0x3}, /* Freq 5795 */
	{160, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x15555, 0x3}, /* Freq 5800 */
	{161, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x18000, 0x3}, /* Freq 5805 */
	{162, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x1AAAA, 0x3}, /* Freq 5810 */
	{163, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x1D555, 0x3}, /* Freq 5815 */
	{164, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x20000, 0x3}, /* Freq 5820 */
	{165, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x22AAA, 0x3}, /* Freq 5825 */
	{166, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x25555, 0x3}, /* Freq 5830 */
	{167, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x28000, 0x3}, /* Freq 5835 */
	{168, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x2AAAA, 0x3}, /* Freq 5840 */
	{169, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x2D555, 0x3}, /* Freq 5845 */
	{170, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x30000, 0x3}, /* Freq 5850 */
	{171, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x32AAA, 0x3}, /* Freq 5855 */
	{172, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x35555, 0x3}, /* Freq 5860 */
	{173, 	(RF_A_BAND | RF_A_BAND_HB),	 0x02, 0x3F, 0x7F, 0xDD, 0xC3, 0x40, 0x0, 0x80, 0x0, 0/*0 -> 1*/, 0, 0, 0x30, 0, 0x0, 0x8, 0x38000, 0x3}, /* Freq 5865 */
};
UCHAR NUM_OF_MT76x0_SDM_CHNL = (sizeof(MT76x0_SDM_Frequency_Plan) / sizeof(MT76x0_FREQ_ITEM));

static UINT8 MT76x0_SDM_Channel[] = {
	183, 185, 43, 45, 54, 55, 57, 58, 102, 103, 105, 106, 115, 117, 126, 127, 129, 130, 139, 141, 150, 151, 153, 154, 163, 165
};
static UCHAR MT76x0_SDM_Channel_Size = (sizeof(MT76x0_SDM_Channel) / sizeof(UINT8));

static const MT76x0_RF_SWITCH_ITEM MT76x0_RF_BW_Switch[] =
{
	/*   Bank, 		Register,	Bw/Band, 	Value */
		{RF_BANK0,	RF_R17,		RF_G_BAND | BW_20,	0x00},
		{RF_BANK0,	RF_R17,		RF_G_BAND | BW_40,	0x00},
		{RF_BANK0,	RF_R17,		RF_A_BAND | BW_20,	0x00},
		{RF_BANK0,	RF_R17,		RF_A_BAND | BW_40,	0x00},
		{RF_BANK0,	RF_R17,		RF_A_BAND | BW_80,	0x00},

		// TODO: need to check B7.R6 & B7.R7 setting for 2.4G again @20121112
		{RF_BANK7,	RF_R06,		RF_G_BAND | BW_20,	0x40},
		{RF_BANK7,	RF_R06,		RF_G_BAND | BW_40,	0x1C},
		{RF_BANK7,	RF_R06,		RF_A_BAND | BW_20,	0x40},
		{RF_BANK7,	RF_R06,		RF_A_BAND | BW_40,	0x20},
		{RF_BANK7,	RF_R06,		RF_A_BAND | BW_80,	0x10},

		{RF_BANK7,	RF_R07,		RF_G_BAND | BW_20,	0x40},
		{RF_BANK7,	RF_R07,		RF_G_BAND | BW_40,	0x20},
		{RF_BANK7,	RF_R07,		RF_A_BAND | BW_20,	0x40},
		{RF_BANK7,	RF_R07,		RF_A_BAND | BW_40,	0x20},
		{RF_BANK7,	RF_R07,		RF_A_BAND | BW_80,	0x10},

		{RF_BANK7,	RF_R08,		RF_G_BAND | BW_20,	0x03},
		{RF_BANK7,	RF_R08,		RF_G_BAND | BW_40,	0x01},
		{RF_BANK7,	RF_R08,		RF_A_BAND | BW_20,	0x03},
		{RF_BANK7,	RF_R08,		RF_A_BAND | BW_40,	0x01},
		{RF_BANK7,	RF_R08,		RF_A_BAND | BW_80,	0x00},

		// TODO: need to check B7.R58 & B7.R59 setting for 2.4G again @20121112
		{RF_BANK7,	RF_R58,		RF_G_BAND | BW_20,	0x40},
		{RF_BANK7,	RF_R58,		RF_G_BAND | BW_40,	0x40},
		{RF_BANK7,	RF_R58,		RF_A_BAND | BW_20,	0x40},
		{RF_BANK7,	RF_R58,		RF_A_BAND | BW_40,	0x40},
		{RF_BANK7,	RF_R58,		RF_A_BAND | BW_80,	0x10},

		{RF_BANK7,	RF_R59,		RF_G_BAND | BW_20,	0x40},
		{RF_BANK7,	RF_R59,		RF_G_BAND | BW_40,	0x40},
		{RF_BANK7,	RF_R59,		RF_A_BAND | BW_20,	0x40},
		{RF_BANK7,	RF_R59,		RF_A_BAND | BW_40,	0x40},
		{RF_BANK7,	RF_R59,		RF_A_BAND | BW_80,	0x10},

		{RF_BANK7,	RF_R60,		RF_G_BAND | BW_20,	0xAA},
		{RF_BANK7,	RF_R60,		RF_G_BAND | BW_40,	0xAA},
		{RF_BANK7,	RF_R60,		RF_A_BAND | BW_20,	0xAA},
		{RF_BANK7,	RF_R60,		RF_A_BAND | BW_40,	0xAA},
		{RF_BANK7,	RF_R60,		RF_A_BAND | BW_80,	0xAA},

		{RF_BANK7,	RF_R76,		BW_20,	0x40},
		{RF_BANK7,	RF_R76,		BW_40,	0x40},
		{RF_BANK7,	RF_R76,		BW_80,	0x10},

		{RF_BANK7,	RF_R77,		BW_20,	0x40},
		{RF_BANK7,	RF_R77,		BW_40,	0x40},
		{RF_BANK7,	RF_R77,		BW_80,	0x10},
};
UCHAR MT76x0_RF_BW_Switch_Size = (sizeof(MT76x0_RF_BW_Switch) / sizeof(MT76x0_RF_SWITCH_ITEM));

static const MT76x0_RF_SWITCH_ITEM MT76x0_RF_Band_Switch[] =
{
	/*   Bank, 		Register,	Bw/Band, 		Value */
		{RF_BANK0,	RF_R16,		RF_G_BAND,		0x20},
		{RF_BANK0,	RF_R16,		RF_A_BAND,		0x20},
		
		{RF_BANK0,	RF_R18,		RF_G_BAND,		0x00},
		{RF_BANK0,	RF_R18,		RF_A_BAND,		0x00},

		{RF_BANK0,	RF_R39,		RF_G_BAND,		0x36},
		{RF_BANK0,	RF_R39,		RF_A_BAND_LB,	0x34},
		{RF_BANK0,	RF_R39,		RF_A_BAND_MB,	0x33},
		{RF_BANK0,	RF_R39,		RF_A_BAND_HB,	0x31},
		{RF_BANK0,	RF_R39,		RF_A_BAND_11J,	0x36},

		{RF_BANK6,	RF_R12,		RF_A_BAND_LB,	0x44},
		{RF_BANK6,	RF_R12,		RF_A_BAND_MB,	0x44},
		{RF_BANK6,	RF_R12,		RF_A_BAND_HB,	0x55},
		{RF_BANK6,	RF_R12,		RF_A_BAND_11J,	0x44},

		{RF_BANK6,	RF_R17,		RF_A_BAND_LB,	0x02},
		{RF_BANK6,	RF_R17,		RF_A_BAND_MB,	0x00},
		{RF_BANK6,	RF_R17,		RF_A_BAND_HB,	0x00},
		{RF_BANK6,	RF_R17,		RF_A_BAND_11J,	0x05},

		{RF_BANK6,	RF_R24,		RF_A_BAND_LB,	0xA1},
		{RF_BANK6,	RF_R24,		RF_A_BAND_MB,	0x41},
		{RF_BANK6,	RF_R24,		RF_A_BAND_HB,	0x21},
		{RF_BANK6,	RF_R24,		RF_A_BAND_11J,	0xE1},

		{RF_BANK6,	RF_R39,		RF_A_BAND_LB,	0x36},
		{RF_BANK6,	RF_R39,		RF_A_BAND_MB,	0x34},
		{RF_BANK6,	RF_R39,		RF_A_BAND_HB,	0x32},
		{RF_BANK6,	RF_R39,		RF_A_BAND_11J,	0x37},

		{RF_BANK6,	RF_R42,		RF_A_BAND_LB,	0xFB},
		{RF_BANK6,	RF_R42,		RF_A_BAND_MB,	0xF3},
		{RF_BANK6,	RF_R42,		RF_A_BAND_HB,	0xEB},
		{RF_BANK6,	RF_R42,		RF_A_BAND_11J,	0xEB},

		/* Move R6-R45, R50~R59 to MT76x0_RF_INT_PA_5G_Channel_0_RegTb/MT76x0_RF_EXT_PA_5G_Channel_0_RegTb */
	
		{RF_BANK6,	RF_R127,	RF_G_BAND,		0x84},
		{RF_BANK6,	RF_R127,	RF_A_BAND,		0x04},

		{RF_BANK7,	RF_R05,		RF_G_BAND,		0x40},
		{RF_BANK7,	RF_R05,		RF_A_BAND,		0x00},

		{RF_BANK7,	RF_R09,		RF_G_BAND,		0x00},
		{RF_BANK7,	RF_R09,		RF_A_BAND,		0x00},
		
		{RF_BANK7,	RF_R70,		RF_G_BAND,		0x00},
		{RF_BANK7,	RF_R70,		RF_A_BAND,		0x6D},

		{RF_BANK7,	RF_R71,		RF_G_BAND,		0x00},
		{RF_BANK7,	RF_R71,		RF_A_BAND,		0xB0},

		{RF_BANK7,	RF_R78,		RF_G_BAND,		0x00},
		{RF_BANK7,	RF_R78,		RF_A_BAND,		0x55},

		{RF_BANK7,	RF_R79,		RF_G_BAND,		0x00},
		{RF_BANK7,	RF_R79,		RF_A_BAND,		0x55},
};
UCHAR MT76x0_RF_Band_Switch_Size = (sizeof(MT76x0_RF_Band_Switch) / sizeof(MT76x0_RF_SWITCH_ITEM));

/*
	External PA
*/
static MT76x0_RF_SWITCH_ITEM MT76x0_RF_EXT_PA_RegTb[] = {
	{RF_BANK6,	RF_R45,		RF_A_BAND_LB,	0x63},
	{RF_BANK6,	RF_R45,		RF_A_BAND_MB,	0x43},
	{RF_BANK6,	RF_R45,		RF_A_BAND_HB,	0x33},
	{RF_BANK6,	RF_R45,		RF_A_BAND_11J,	0x73},

	{RF_BANK6,	RF_R50,		RF_A_BAND_LB,	0x02},
	{RF_BANK6,	RF_R50,		RF_A_BAND_MB,	0x02},
	{RF_BANK6,	RF_R50,		RF_A_BAND_HB,	0x02},
	{RF_BANK6,	RF_R50,		RF_A_BAND_11J,	0x02},

	{RF_BANK6,	RF_R51,		RF_A_BAND_LB,	0x02},
	{RF_BANK6,	RF_R51,		RF_A_BAND_MB,	0x02},
	{RF_BANK6,	RF_R51,		RF_A_BAND_HB,	0x02},
	{RF_BANK6,	RF_R51,		RF_A_BAND_11J,	0x02},

	{RF_BANK6,	RF_R52,		RF_A_BAND_LB,	0x08},
	{RF_BANK6,	RF_R52,		RF_A_BAND_MB,	0x08},
	{RF_BANK6,	RF_R52,		RF_A_BAND_HB,	0x08},
	{RF_BANK6,	RF_R52,		RF_A_BAND_11J,	0x08},

	{RF_BANK6,	RF_R53,		RF_A_BAND_LB,	0x08},
	{RF_BANK6,	RF_R53,		RF_A_BAND_MB,	0x08},
	{RF_BANK6,	RF_R53,		RF_A_BAND_HB,	0x08},
	{RF_BANK6,	RF_R53,		RF_A_BAND_11J,	0x08},

	{RF_BANK6,	RF_R54,		RF_A_BAND_LB,	0x0A},
	{RF_BANK6,	RF_R54,		RF_A_BAND_MB,	0x0A},
	{RF_BANK6,	RF_R54,		RF_A_BAND_HB,	0x0A},
	{RF_BANK6,	RF_R54,		RF_A_BAND_11J,	0x0A},

	{RF_BANK6,	RF_R55,		RF_A_BAND_LB,	0x0A},
	{RF_BANK6,	RF_R55,		RF_A_BAND_MB,	0x0A},
	{RF_BANK6,	RF_R55,		RF_A_BAND_HB,	0x0A},
	{RF_BANK6,	RF_R55,		RF_A_BAND_11J,	0x0A},

	{RF_BANK6,	RF_R56,		RF_A_BAND_LB,	0x05},
	{RF_BANK6,	RF_R56,		RF_A_BAND_MB,	0x05},
	{RF_BANK6,	RF_R56,		RF_A_BAND_HB,	0x05},
	{RF_BANK6,	RF_R56,		RF_A_BAND_11J,	0x05},

	{RF_BANK6,	RF_R57,		RF_A_BAND_LB,	0x05},
	{RF_BANK6,	RF_R57,		RF_A_BAND_MB,	0x05},
	{RF_BANK6,	RF_R57,		RF_A_BAND_HB,	0x05},
	{RF_BANK6,	RF_R57,		RF_A_BAND_11J,	0x05},

	{RF_BANK6,	RF_R58,		RF_A_BAND_LB,	0x05},
	{RF_BANK6,	RF_R58,		RF_A_BAND_MB,	0x03},
	{RF_BANK6,	RF_R58,		RF_A_BAND_HB,	0x02},
	{RF_BANK6,	RF_R58,		RF_A_BAND_11J,	0x07},

	{RF_BANK6,	RF_R59,		RF_A_BAND_LB,	0x05},
	{RF_BANK6,	RF_R59,		RF_A_BAND_MB,	0x03},
	{RF_BANK6,	RF_R59,		RF_A_BAND_HB,	0x02},
	{RF_BANK6,	RF_R59,		RF_A_BAND_11J,	0x07},
};
static UINT32 MT76x0_RF_EXT_PA_RegTb_Size = (sizeof(MT76x0_RF_EXT_PA_RegTb) / sizeof(MT76x0_RF_SWITCH_ITEM));

/*
	Internal PA
*/
static MT76x0_RF_SWITCH_ITEM MT76x0_RF_INT_PA_RegTb[] = {
#if 0	
	{RF_BANK6,	RF_R45,		RF_A_BAND_LB,	0x59},
	{RF_BANK6,	RF_R45,		RF_A_BAND_MB,	0x39},
	{RF_BANK6,	RF_R45,		RF_A_BAND_HB,	0x19},

	{RF_BANK6,	RF_R50,		RF_A_BAND_LB,	0x18},
	{RF_BANK6,	RF_R50,		RF_A_BAND_MB,	0x18},
	{RF_BANK6,	RF_R50,		RF_A_BAND_HB,	0x18},

	{RF_BANK6,	RF_R51,		RF_A_BAND_LB,	0x10},
	{RF_BANK6,	RF_R51,		RF_A_BAND_MB,	0x10},
	{RF_BANK6,	RF_R51,		RF_A_BAND_HB,	0x10},

	{RF_BANK6,	RF_R52,		RF_A_BAND_LB,	0x1F},
	{RF_BANK6,	RF_R52,		RF_A_BAND_MB,	0x1F},
	{RF_BANK6,	RF_R52,		RF_A_BAND_HB,	0x1F},

	{RF_BANK6,	RF_R53,		RF_A_BAND_LB,	0x18},
	{RF_BANK6,	RF_R53,		RF_A_BAND_MB,	0x18},
	{RF_BANK6,	RF_R53,		RF_A_BAND_HB,	0x18},

	{RF_BANK6,	RF_R54,		RF_A_BAND_LB,	0x0C},
	{RF_BANK6,	RF_R54,		RF_A_BAND_MB,	0x0C},
	{RF_BANK6,	RF_R54,		RF_A_BAND_HB,	0x0C},

	{RF_BANK6,	RF_R55,		RF_A_BAND_LB,	0x04},
	{RF_BANK6,	RF_R55,		RF_A_BAND_MB,	0x04},
	{RF_BANK6,	RF_R55,		RF_A_BAND_HB,	0x04},

	{RF_BANK6,	RF_R56,		RF_A_BAND_LB,	0x1F},
	{RF_BANK6,	RF_R56,		RF_A_BAND_MB,	0x1F},
	{RF_BANK6,	RF_R56,		RF_A_BAND_HB,	0x1F},

	{RF_BANK6,	RF_R57,		RF_A_BAND_LB,	0x1F},
	{RF_BANK6,	RF_R57,		RF_A_BAND_MB,	0x1F},
	{RF_BANK6,	RF_R57,		RF_A_BAND_HB,	0x1F},
		
	{RF_BANK6,	RF_R58,		RF_A_BAND_LB,	0x75},
	{RF_BANK6,	RF_R58,		RF_A_BAND_MB,	0x73},
	{RF_BANK6,	RF_R58,		RF_A_BAND_HB,	0x72},

	{RF_BANK6,	RF_R59,		RF_A_BAND_LB,	0x04},
	{RF_BANK6,	RF_R59,		RF_A_BAND_MB,	0x03},
	{RF_BANK6,	RF_R59,		RF_A_BAND_HB,	0x02},
#endif
};
static UINT32 MT76x0_RF_INT_PA_RegTb_Size = (sizeof(MT76x0_RF_INT_PA_RegTb) / sizeof(MT76x0_RF_SWITCH_ITEM));

/*
	NOTE: MAX_NUM_OF_CHANNELS shall equal sizeof(txpwr_chlist))
*/
static UCHAR mt76x0_txpwr_chlist[] = {
	1, 2,3,4,5,6,7,8,9,10,11,12,13,14,
	36,38,40,44,46,48,52,54,56,60,62,64,
	100,102,104,108,110,112,116,118,120,124,126,128,132,134,136,140,
	149,151,153,157,159,161,165,167,169,171,173,
	42, 58, 106, 122, 155,
};

BOOLEAN mt76x0_get_tssi_report(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN bResetTssiInfo,
	OUT PCHAR pTssiReport);

static void do_full_calibration(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR channel,
	IN BOOLEAN bSave);

static void mt76x0_show_pwr_info(RTMP_ADAPTER *ad);


/*
	Initialize FCE
*/
void init_fce(PRTMP_ADAPTER ad)
{
	L2_STUFFING_STRUC L2Stuffing;

	L2Stuffing.word = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: -->\n", __FUNCTION__));

	RTMP_IO_READ32(ad, FCE_L2_STUFF, &L2Stuffing.word);
	L2Stuffing.field.FS_WR_MPDU_LEN_EN = 0;
	RTMP_IO_WRITE32(ad, FCE_L2_STUFF, L2Stuffing.word);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: <--\n", __FUNCTION__));
}


/*
	Select 2.4/5GHz band
*/
void mt76x0_band_select(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR channel)
{
#if 0
	UINT32 IdReg = 0;
#endif
	if (!IS_MT76x0(pAd)) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Incorrect NIC\n", __FUNCTION__));
		return;
	}
	
	DBGPRINT(RT_DEBUG_INFO, ("%s: -->\n", __FUNCTION__));

	if (IS_MT7650E(pAd) || IS_MT7630E(pAd))
		rlt_rf_write(pAd, RF_BANK5, RF_R02, 0x1D); /* 5G+2G+BT (MT7650E) ; 2G+BT (MT7630E) */
	else if (IS_MT7610U(pAd))
		rlt_rf_write(pAd, RF_BANK5, RF_R02, 0x0C); /* 5G+2G */
	else if (IS_MT7610E(pAd))
		rlt_rf_write(pAd, RF_BANK5, RF_R02, 0x00); /* 5G only */
		
	if (channel <= 14) {
		/*
			Select 2.4GHz 
		*/
#if 0
		for (IdReg = 0; IdReg < MT76x0_RF_2G_Channel_0_RegTb_Size; IdReg++)
		{
			rlt_rf_write(pAd, 
						MT76x0_RF_2G_Channel_0_RegTb[IdReg].Bank,
						MT76x0_RF_2G_Channel_0_RegTb[IdReg].Register,
						MT76x0_RF_2G_Channel_0_RegTb[IdReg].Value);
		}
#else
    		RF_RANDOM_WRITE(pAd, MT76x0_RF_2G_Channel_0_RegTb, MT76x0_RF_2G_Channel_0_RegTb_Size);
#endif
		rlt_rf_write(pAd, RF_BANK5, RF_R00, 0x45); /* Enable G-Band */
		rlt_rf_write(pAd, RF_BANK6, RF_R00, 0x44); /* Disable A-band */

		rtmp_mac_set_band(pAd, BAND_24G);

		RTMP_IO_WRITE32(pAd, TX_ALC_VGA3, 0x00050007);
		RTMP_IO_WRITE32(pAd, TX0_RF_GAIN_CORR, 0x003E0002);
	} else {
		/*
			Select 5GHz 
		*/
#if 0
		for (IdReg = 0; IdReg < MT76x0_RF_5G_Channel_0_RegTb_Size; IdReg++)
		{
			rlt_rf_write(pAd, 
				     MT76x0_RF_5G_Channel_0_RegTb[IdReg].Bank,
				     MT76x0_RF_5G_Channel_0_RegTb[IdReg].Register,
				     MT76x0_RF_5G_Channel_0_RegTb[IdReg].Value);
		}
#else
		RF_RANDOM_WRITE(pAd, MT76x0_RF_5G_Channel_0_RegTb, MT76x0_RF_5G_Channel_0_RegTb_Size);
#endif		
		rlt_rf_write(pAd, RF_BANK5, RF_R00, 0x44); /* Disable G-band */
		rlt_rf_write(pAd, RF_BANK6, RF_R00, 0x45); /* Enable A-Band */
				
		rtmp_mac_set_band(pAd, BAND_5G);

		RTMP_IO_WRITE32(pAd, TX_ALC_VGA3, 0x00000005);
		RTMP_IO_WRITE32(pAd, TX0_RF_GAIN_CORR, 0x01010102);
	}

	DBGPRINT(RT_DEBUG_INFO, ("%s: <--\n", __FUNCTION__));
}


/*
	Set RF channel frequency parameters:	
	Rdiv: R24[1:0]
	N: R29[7:0], R30[0]
	Nominator: R31[4:0]
	Non-Sigma: !SDM R31[7:5]
	Den: (Denomina - 8) R32[4:0]
	Loop Filter Config: R33, R34
	Pll_idiv: frac comp R35[6:0]
*/
void mt76x0_set_rf_ch_freq_prm(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR Channel)
{
	UINT32 i = 0, RfBand = 0, MacReg = 0;
	UCHAR RFValue = 0;
	BOOLEAN bSDM = FALSE;
	MT76x0_FREQ_ITEM *pMT76x0_freq_item = NULL;

	if (!IS_MT76x0(pAd)) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Incorrect NIC\n", __FUNCTION__));		
		return;
	}
	
	DBGPRINT(RT_DEBUG_INFO, ("%s: -->\n", __FUNCTION__));

	for (i = 0; i < MT76x0_SDM_Channel_Size; i++)
	{
		if (Channel == MT76x0_SDM_Channel[i]) {
			bSDM = TRUE;
			break;
		}
	}
	
	for (i = 0; i < NUM_OF_MT76x0_CHNL; i++)
	{
		if (Channel == MT76x0_Frequency_Plan[i].Channel) {
			RfBand = MT76x0_Frequency_Plan[i].Band;			

			if (bSDM)
				pMT76x0_freq_item = &(MT76x0_SDM_Frequency_Plan[i]);
			else
				pMT76x0_freq_item = &(MT76x0_Frequency_Plan[i]);

			/* 
				R37
			*/
			rlt_rf_write(pAd, RF_BANK0, RF_R37, pMT76x0_freq_item->pllR37);
			
				/*
				R36
				*/
			rlt_rf_write(pAd, RF_BANK0, RF_R36, pMT76x0_freq_item->pllR36);

			/*
				R35
			*/
			rlt_rf_write(pAd, RF_BANK0, RF_R35, pMT76x0_freq_item->pllR35);

			/*
				R34
			*/
			rlt_rf_write(pAd, RF_BANK0, RF_R34, pMT76x0_freq_item->pllR34);

			/*
				R33
			*/
			rlt_rf_write(pAd, RF_BANK0, RF_R33, pMT76x0_freq_item->pllR33);

			/*
				R32<7:5>
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R32, &RFValue);
			RFValue &= ~(0xE0);
			RFValue |= pMT76x0_freq_item->pllR32_b7b5;
			rlt_rf_write(pAd, RF_BANK0, RF_R32, RFValue);
			
			/*
				R32<4:0> pll_den: (Denomina - 8)
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R32, &RFValue);
			RFValue &= ~(0x1F);
			RFValue |= pMT76x0_freq_item->pllR32_b4b0;
			rlt_rf_write(pAd, RF_BANK0, RF_R32, RFValue);

			/*
				R31<7:5>
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R31, &RFValue);
			RFValue &= ~(0xE0);
			RFValue |= pMT76x0_freq_item->pllR31_b7b5;
			rlt_rf_write(pAd, RF_BANK0, RF_R31, RFValue);

			/*
				R31<4:0> pll_k(Nominator)
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R31, &RFValue);
			RFValue &= ~(0x1F);
			RFValue |= pMT76x0_freq_item->pllR31_b4b0;
			rlt_rf_write(pAd, RF_BANK0, RF_R31, RFValue);
			
			/*
				R30<7> sdm_reset_n
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R30, &RFValue);
			RFValue &= ~(0x80);
			if (bSDM) {
				rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);
				RFValue |= (0x80);
				rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);
			} else {
				RFValue |= pMT76x0_freq_item->pllR30_b7;
				rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);
			}
			
			/*
				R30<6:2> sdmmash_prbs,sin
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R30, &RFValue);
			RFValue &= ~(0x7C);
			RFValue |= pMT76x0_freq_item->pllR30_b6b2;
			rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);
			
			/*
				R30<1> sdm_bp
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R30, &RFValue);
			RFValue &= ~(0x02);
			RFValue |= (pMT76x0_freq_item->pllR30_b1 << 1);
			rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);
			
			/*
				R30<0> R29<7:0> (hex) pll_n
			*/
			RFValue = pMT76x0_freq_item->pll_n & 0x00FF;
			rlt_rf_write(pAd, RF_BANK0, RF_R29, RFValue);

			rlt_rf_read(pAd, RF_BANK0, RF_R30, &RFValue);
			RFValue &= ~(0x1);
			RFValue |= ((pMT76x0_freq_item->pll_n >> 8) & 0x0001);
			rlt_rf_write(pAd, RF_BANK0, RF_R30, RFValue);
			
			/*
				R28<7:6> isi_iso
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R28, &RFValue);
			RFValue &= ~(0xC0);
			RFValue |= pMT76x0_freq_item->pllR28_b7b6;
			rlt_rf_write(pAd, RF_BANK0, RF_R28, RFValue);
			
			/*
				R28<5:4> pfd_dly
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R28, &RFValue);
			RFValue &= ~(0x30);
			RFValue |= pMT76x0_freq_item->pllR28_b5b4;
			rlt_rf_write(pAd, RF_BANK0, RF_R28, RFValue);
			
			/*
				R28<3:2> clksel option
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R28, &RFValue);
			RFValue &= ~(0x0C);
			RFValue |= pMT76x0_freq_item->pllR28_b3b2;
			rlt_rf_write(pAd, RF_BANK0, RF_R28, RFValue);

			/*
				R28<1:0> R27<7:0> R26<7:0> (hex) sdm_k
			*/
			RFValue = pMT76x0_freq_item->Pll_sdm_k & 0x000000FF;
			rlt_rf_write(pAd, RF_BANK0, RF_R26, RFValue);

			RFValue = ((pMT76x0_freq_item->Pll_sdm_k >> 8) & 0x000000FF);
			rlt_rf_write(pAd, RF_BANK0, RF_R27, RFValue);
			
			rlt_rf_read(pAd, RF_BANK0, RF_R28, &RFValue);
			RFValue &= ~(0x3);
			RFValue |= ((pMT76x0_freq_item->Pll_sdm_k >> 16) & 0x0003);
			rlt_rf_write(pAd, RF_BANK0, RF_R28, RFValue);
			
			/*
				R24<1:0> xo_div
			*/
			rlt_rf_read(pAd, RF_BANK0, RF_R24, &RFValue);
			RFValue &= ~(0x3);
			RFValue |= pMT76x0_freq_item->pllR24_b1b0;
			rlt_rf_write(pAd, RF_BANK0, RF_R24, RFValue);

			
			pAd->LatchRfRegs.Channel = Channel; /* Channel latch */
			pAd->hw_cfg.lan_gain = GET_LNA_GAIN(pAd);

			DBGPRINT(RT_DEBUG_TRACE,
				("%s: SwitchChannel#%d(Band = 0x%02X, RF=%d, %dT), "
				"0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, %u, 0x%02X, %u, 0x%02X, 0x%02X, 0x%02X, 0x%04X, 0x%02X, 0x%02X, 0x%02X, 0x%08X, 0x%02X\n",
				__FUNCTION__,
				Channel,
				RfBand,
				pAd->RfIcType,
				pAd->Antenna.field.TxPath,
				pMT76x0_freq_item->pllR37,
				pMT76x0_freq_item->pllR36,
				pMT76x0_freq_item->pllR35,
				pMT76x0_freq_item->pllR34,
				pMT76x0_freq_item->pllR33,
				pMT76x0_freq_item->pllR32_b7b5,
				pMT76x0_freq_item->pllR32_b4b0,
				pMT76x0_freq_item->pllR31_b7b5,
				pMT76x0_freq_item->pllR31_b4b0,
				pMT76x0_freq_item->pllR30_b7,
				pMT76x0_freq_item->pllR30_b6b2,
				pMT76x0_freq_item->pllR30_b1,
				pMT76x0_freq_item->pll_n,
				pMT76x0_freq_item->pllR28_b7b6,
				pMT76x0_freq_item->pllR28_b5b4,
				pMT76x0_freq_item->pllR28_b3b2,
				pMT76x0_freq_item->Pll_sdm_k,
				pMT76x0_freq_item->pllR24_b1b0));
			break;
		}
	}	

	for (i = 0; i < MT76x0_RF_BW_Switch_Size; i++)
	{
		if (pAd->CommonCfg.BBPCurrentBW == MT76x0_RF_BW_Switch[i].BwBand) {
			rlt_rf_write(pAd, 
						MT76x0_RF_BW_Switch[i].Bank,
						MT76x0_RF_BW_Switch[i].Register,
						MT76x0_RF_BW_Switch[i].Value);
		}
		else if ((pAd->CommonCfg.BBPCurrentBW == (MT76x0_RF_BW_Switch[i].BwBand & 0xFF)) &&
				 (RfBand & MT76x0_RF_BW_Switch[i].BwBand)) {
			rlt_rf_write(pAd, 
						MT76x0_RF_BW_Switch[i].Bank,
						MT76x0_RF_BW_Switch[i].Register,
						MT76x0_RF_BW_Switch[i].Value);
		}
	}

	for (i = 0; i < MT76x0_RF_Band_Switch_Size; i++)
	{
		if (MT76x0_RF_Band_Switch[i].BwBand & RfBand) {
			rlt_rf_write(pAd, 
						MT76x0_RF_Band_Switch[i].Bank,
						MT76x0_RF_Band_Switch[i].Register,
						MT76x0_RF_Band_Switch[i].Value);
		}
	}
	
	RTMP_IO_READ32(pAd, RF_MISC, &MacReg);
	MacReg &= ~(0xC); /* Clear 0x518[3:2] */
	RTMP_IO_WRITE32(pAd, RF_MISC, MacReg);

	DBGPRINT(RT_DEBUG_INFO, ("\n\n*********** PAType = %d ***********\n\n", pAd->chipCap.PAType));
	if ((pAd->chipCap.PAType == INT_PA_2G_5G) ||
		((pAd->chipCap.PAType == EXT_PA_5G_ONLY) && (RfBand & RF_G_BAND)) ||
		((pAd->chipCap.PAType == EXT_PA_2G_ONLY) && (RfBand & RF_A_BAND))) 
	{
		/* Internal PA */
		for (i = 0; i < MT76x0_RF_INT_PA_RegTb_Size; i++)
		{
			if (MT76x0_RF_INT_PA_RegTb[i].BwBand & RfBand) {
				rlt_rf_write(pAd, 
							MT76x0_RF_INT_PA_RegTb[i].Bank,
							MT76x0_RF_INT_PA_RegTb[i].Register,
							MT76x0_RF_INT_PA_RegTb[i].Value);

				DBGPRINT(RT_DEBUG_INFO, ("%s: INT_PA_RegTb - B%d.R%02d = 0x%02x\n", 
							__FUNCTION__, 
							MT76x0_RF_INT_PA_RegTb[i].Bank,
							MT76x0_RF_INT_PA_RegTb[i].Register,
							MT76x0_RF_INT_PA_RegTb[i].Value));
			}
		}
	} else {
		/*
			RF_MISC (offset: 0x0518)
			[2]1'b1: enable external A band PA, 1'b0: disable external A band PA
			[3]1'b1: enable external G band PA, 1'b0: disable external G band PA
		*/
		if (RfBand & RF_A_BAND) {
			RTMP_IO_READ32(pAd, RF_MISC, &MacReg);
			MacReg |= (0x4);
			RTMP_IO_WRITE32(pAd, RF_MISC, MacReg);
		} else {
			RTMP_IO_READ32(pAd, RF_MISC, &MacReg);
			MacReg |= (0x8);
			RTMP_IO_WRITE32(pAd, RF_MISC, MacReg);
		}
		
		/* External PA */
		for (i = 0; i < MT76x0_RF_EXT_PA_RegTb_Size; i++)
		{
			if (MT76x0_RF_EXT_PA_RegTb[i].BwBand & RfBand) {
				rlt_rf_write(pAd, 
							MT76x0_RF_EXT_PA_RegTb[i].Bank,
							MT76x0_RF_EXT_PA_RegTb[i].Register,
							MT76x0_RF_EXT_PA_RegTb[i].Value);

				DBGPRINT(RT_DEBUG_INFO, ("%s: EXT_PA_RegTb - B%d.R%02d = 0x%02x\n", 
							__FUNCTION__, 
							MT76x0_RF_EXT_PA_RegTb[i].Bank,
							MT76x0_RF_EXT_PA_RegTb[i].Register,
							MT76x0_RF_EXT_PA_RegTb[i].Value));
			}
		}
	}

	if (RfBand & RF_G_BAND) {
		/* 
			Set Atten mode = 2 for G band to disable Tx Inc DCOC Cal by Chee's comment. 
			@MT7650_CR_setting_1018.xlsx 
		*/	
		RTMP_IO_WRITE32(pAd, TX0_RF_GAIN_ATTEN, 0x63707400);
		RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &MacReg);
		MacReg &= 0x896400FF;
		RTMP_IO_WRITE32(pAd, TX_ALC_CFG_1, MacReg); 
	} else {
		/* 
			Set Atten mode = 0 For Ext A band to disable Tx Inc DCOC Cal by Chee's comment. 
			@MT7650_CR_setting_1018.xlsx 
		*/	
		RTMP_IO_WRITE32(pAd, TX0_RF_GAIN_ATTEN, 0x686A7800);
		RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &MacReg);
		MacReg &= 0x890400FF;
		RTMP_IO_WRITE32(pAd, TX_ALC_CFG_1, MacReg); 
	}
	
	DBGPRINT(RT_DEBUG_INFO, ("%s: <--\n", __FUNCTION__));
}


static void mt76x0_nic_init_rf_registers(PRTMP_ADAPTER pAd)
{
	UINT32 IdReg;
	UCHAR RFValue;

#if 0
	for (IdReg = 0; IdReg < MT76x0_RF_Central_RegTb_Size; IdReg++)
	{
		if ((MT76x0_RF_Central_RegTb[IdReg].Bank == RF_BANK0) &&
			(MT76x0_RF_Central_RegTb[IdReg].Register == RF_R21)) {
			USHORT e2p_val = 0;
			/* Check if we are co-clock mode */
			RT28xx_EEPROM_READ16(pAd, 0x42, e2p_val);
			if (!(e2p_val & (1 << 9)))
				MT76x0_RF_Central_RegTb[IdReg].Value = 0x12;
		}
		
		rlt_rf_write(pAd, 
					MT76x0_RF_Central_RegTb[IdReg].Bank,
					MT76x0_RF_Central_RegTb[IdReg].Register,
					MT76x0_RF_Central_RegTb[IdReg].Value);
	}
#else
	RF_RANDOM_WRITE(pAd, MT76x0_RF_Central_RegTb, MT76x0_RF_Central_RegTb_Size);
#endif

#if 0
	for(IdReg = 0; IdReg < MT76x0_RF_2G_Channel_0_RegTb_Size; IdReg++)
	{
		rlt_rf_write(pAd, 
					MT76x0_RF_2G_Channel_0_RegTb[IdReg].Bank,
					MT76x0_RF_2G_Channel_0_RegTb[IdReg].Register,
					MT76x0_RF_2G_Channel_0_RegTb[IdReg].Value);
	}
#else
	RF_RANDOM_WRITE(pAd, MT76x0_RF_2G_Channel_0_RegTb, MT76x0_RF_2G_Channel_0_RegTb_Size);
#endif

	if (IS_MT7650E(pAd) || IS_MT7630E(pAd))
		rlt_rf_write(pAd, RF_BANK5, RF_R02, 0x1D); /* 5G+2G+BT (MT7650E) ; 2G+BT (MT7630E) */
	else if (IS_MT7610U(pAd))
		rlt_rf_write(pAd, RF_BANK5, RF_R02, 0x0C); /* 5G+2G */
	else if (IS_MT7610E(pAd))
		rlt_rf_write(pAd, RF_BANK5, RF_R02, 0x00); /* 5G only */

#if 0
	for(IdReg = 0; IdReg < MT76x0_RF_5G_Channel_0_RegTb_Size; IdReg++)
	{
		rlt_rf_write(pAd, 
					MT76x0_RF_5G_Channel_0_RegTb[IdReg].Bank,
					MT76x0_RF_5G_Channel_0_RegTb[IdReg].Register,
					MT76x0_RF_5G_Channel_0_RegTb[IdReg].Value);
	}
#else
	RF_RANDOM_WRITE(pAd, MT76x0_RF_5G_Channel_0_RegTb, MT76x0_RF_5G_Channel_0_RegTb_Size);
#endif

#if 0
	for (IdReg = 0; IdReg < MT76x0_RF_VGA_Channel_0_RegTb_Size; IdReg++)
	{
		rlt_rf_write(pAd, 
					MT76x0_RF_VGA_Channel_0_RegTb[IdReg].Bank,
					MT76x0_RF_VGA_Channel_0_RegTb[IdReg].Register,
					MT76x0_RF_VGA_Channel_0_RegTb[IdReg].Value);
	}
#else
	RF_RANDOM_WRITE(pAd, MT76x0_RF_VGA_Channel_0_RegTb, MT76x0_RF_VGA_Channel_0_RegTb_Size);
#endif

	for (IdReg = 0; IdReg < MT76x0_RF_BW_Switch_Size; IdReg++)
	{
		if (pAd->CommonCfg.BBPCurrentBW == MT76x0_RF_BW_Switch[IdReg].BwBand) {
			rlt_rf_write(pAd, 
						MT76x0_RF_BW_Switch[IdReg].Bank,
						MT76x0_RF_BW_Switch[IdReg].Register,
						MT76x0_RF_BW_Switch[IdReg].Value);
		} else if ((BW_20 == (MT76x0_RF_BW_Switch[IdReg].BwBand & 0xFF)) &&
				 (RF_G_BAND & MT76x0_RF_BW_Switch[IdReg].BwBand)) {
			rlt_rf_write(pAd, 
						MT76x0_RF_BW_Switch[IdReg].Bank,
						MT76x0_RF_BW_Switch[IdReg].Register,
						MT76x0_RF_BW_Switch[IdReg].Value);
		}
	}

	for (IdReg = 0; IdReg < MT76x0_RF_Band_Switch_Size; IdReg++)
	{
		if (MT76x0_RF_Band_Switch[IdReg].BwBand & RF_G_BAND) {
			rlt_rf_write(pAd, 
						MT76x0_RF_Band_Switch[IdReg].Bank,
						MT76x0_RF_Band_Switch[IdReg].Register,
						MT76x0_RF_Band_Switch[IdReg].Value);
		}
	}

	/*
		Frequency calibration
		E1: B0.R22<6:0>: xo_cxo<6:0>
		E2: B0.R21<0>: xo_cxo<0>, B0.R22<7:0>: xo_cxo<8:1> 
	*/
	RFValue = (UCHAR)(pAd->RfFreqOffset & 0xFF);
	RFValue = min(RFValue, 0xBF); /* Max of 9-bit built-in crystal oscillator C1 code */
	rlt_rf_write(pAd, RF_BANK0, RF_R22, RFValue);
	
	rlt_rf_read(pAd, RF_BANK0, RF_R22, &RFValue);
	DBGPRINT(RT_DEBUG_TRACE, ("%s: B0.R22 = 0x%02x\n", __FUNCTION__, RFValue));

	/*
		Reset the DAC (Set B0.R73<7>=1, then set B0.R73<7>=0, and then set B0.R73<7>) during power up.
	*/
	rlt_rf_read(pAd, RF_BANK0, RF_R73, &RFValue);
	RFValue |= 0x80;
	rlt_rf_write(pAd, RF_BANK0, RF_R73, RFValue);	
	RFValue &= (~0x80);
	rlt_rf_write(pAd, RF_BANK0, RF_R73, RFValue);	
	RFValue |= 0x80;
	rlt_rf_write(pAd, RF_BANK0, RF_R73, RFValue);	

	/* 
		vcocal_en (initiate VCO calibration (reset after completion)) - It should be at the end of RF configuration. 
	*/
	rlt_rf_read(pAd, RF_BANK0, RF_R04, &RFValue);
	RFValue = ((RFValue & ~0x80) | 0x80); 
	rlt_rf_write(pAd, RF_BANK0, RF_R04, RFValue);
}


/*
========================================================================
Routine Description:
	Initialize specific MAC registers.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
static void mt76x0_nic_init_mac_registers(PRTMP_ADAPTER pAd)
{
#if 0
	UINT32 IdReg;
#endif
	UINT32 MacReg = 0;
	USHORT trsw_mode = 0;

	/*
		Enable PBF and MAC clock
		SYS_CTRL[11:10] = 0x3
	*/
#if 0
	for(IdReg=0; IdReg<MT76x0_NUM_MAC_REG_PARMS; IdReg++)
	{
		RTMP_IO_WRITE32(pAd, MT76x0_MACRegTable[IdReg].Register,
								MT76x0_MACRegTable[IdReg].Value);
	}
#else
	RANDOM_WRITE(pAd, MT76x0_MACRegTable, MT76x0_NUM_MAC_REG_PARMS);
#endif

	RT28xx_EEPROM_READ16(pAd, 0x24, trsw_mode);
	if (((trsw_mode & ~(0xFFCF)) >> 4) == 0x3) {
		RTMP_IO_WRITE32(pAd, TX_SW_CFG1, 0x00040200); /* Adjust TR_SW off delay for TRSW mode */
		DBGPRINT(RT_DEBUG_TRACE, ("%s: TRSW = 0x3\n", __FUNCTION__));
	}
	
#ifdef HDR_TRANS_TX_SUPPORT
	/*
		Enable Header Translation TX 
	*/
	RTMP_IO_READ32(pAd, HEADER_TRANS_CTRL_REG, &MacReg);
	MacReg |= 0x1; /* 0x1: TX, 0x2: RX */
	RTMP_IO_WRITE32(pAd, HEADER_TRANS_CTRL_REG, MacReg);
#endif /* HDR_TRANS_TX_SUPPORT */

	/*
		Release BBP and MAC reset
		MAC_SYS_CTRL[1:0] = 0x0
	*/
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacReg);
	MacReg &= ~(0x3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacReg);

#ifdef RTMP_MAC_PCI
	if (IS_MT7610E(pAd)) {
		/*
			Disable COEX_EN
		*/
		RTMP_IO_READ32(pAd, COEXCFG0, &MacReg);
		MacReg &= 0xFFFFFFFE;
		RTMP_IO_WRITE32(pAd, COEXCFG0, MacReg);
	}
#endif /* RTMP_MAC_PCI */

	/*
		Set 0x141C[15:12]=0xF
	*/
	RTMP_IO_READ32(pAd, EXT_CCA_CFG, &MacReg);
	MacReg |= (0x0000F000);
	RTMP_IO_WRITE32(pAd, EXT_CCA_CFG, MacReg);

	init_fce(pAd);

	/*
		TxRing 9 is for Mgmt frame.
		TxRing 8 is for In-band command frame.
		WMM_RG0_TXQMA: This register setting is for FCE to define the rule of TxRing 9.
		WMM_RG1_TXQMA: This register setting is for FCE to define the rule of TxRing 8.
	*/
	RTMP_IO_READ32(pAd, WMM_CTRL, &MacReg);
	MacReg &= ~(0x000003FF);
	MacReg |= (0x00000201);
	RTMP_IO_WRITE32(pAd, WMM_CTRL, MacReg);

	/*
		0x110:	Set PERST_N iopad to NO 75K pull up. (MT7650_E3_CR_setting_0124.xlsx 2013)
	*/
	RTMP_IO_READ32(pAd, 0x110, &MacReg);
	MacReg &= ~(0x00000200);
	RTMP_IO_WRITE32(pAd, 0x110, MacReg);

#ifdef MCS_LUT_SUPPORT
	RTMP_IO_READ32(pAd, TX_FBK_LIMIT, &MacReg);
	if (RTMP_TEST_MORE_FLAG(pAd, fASIC_CAP_MCS_LUT))
		MacReg |= 0x40000;
	else
		MacReg &= (~0x40000);
	RTMP_IO_WRITE32(pAd, TX_FBK_LIMIT, MacReg);
#endif /* MCS_LUT_SUPPORT */

#ifdef RELEASE_EXCLUDE
	/* A workaround solution for EU bandwidth adaptation test */
#endif /* RELEASE_EXCLUDE */

	if ((pAd->CommonCfg.Channel > 14) &&
		(pAd->CommonCfg.bIEEE80211H == TRUE) &&
		(pAd->CommonCfg.BBPCurrentBW == BW_80)) {
		/* Improve BW Adaptation */
		RTMP_IO_READ32(pAd, TXOP_CTRL_CFG, &MacReg);
		MacReg = (MacReg & 0xFFFF) | (0x410 << 16);
		RTMP_IO_WRITE32(pAd, TXOP_CTRL_CFG, MacReg);
		RTMP_IO_WRITE32(pAd, TXOP_HLDR_ET, 0x3);
	}
}


/*
========================================================================
Routine Description:
	Initialize specific BBP registers.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
static VOID NICInitMT76x0BbpRegisters(
	IN PRTMP_ADAPTER pAd)
{
	INT IdReg;

#if 0
	for(IdReg = 0; IdReg < MT76x0_BBP_Init_Tab_Size; IdReg++)
	{
		RTMP_BBP_IO_WRITE32(pAd, MT76x0_BBP_Init_Tab[IdReg].Register,
				MT76x0_BBP_Init_Tab[IdReg].Value);
	}
#else
	RANDOM_WRITE(pAd, MT76x0_BBP_Init_Tab, MT76x0_BBP_Init_Tab_Size);
#endif	
	
	for (IdReg = 0; IdReg < MT76x0_BPP_SWITCH_Tab_Size; IdReg++)
	{
		if (((RF_G_BAND | RF_BW_20) & MT76x0_BPP_SWITCH_Tab[IdReg].BwBand) == (RF_G_BAND | RF_BW_20)) {
			RTMP_BBP_IO_WRITE32(pAd, MT76x0_BPP_SWITCH_Tab[IdReg].RegDate.Register,
					MT76x0_BPP_SWITCH_Tab[IdReg].RegDate.Value);
		}
	}

#if 0
	for(IdReg=0; IdReg < MT76x0_DCOC_Tab_Size; IdReg++)
	{
		RTMP_BBP_IO_WRITE32(pAd, MT76x0_DCOC_Tab[IdReg].Register,
				MT76x0_DCOC_Tab[IdReg].Value);
	}
#else
	RANDOM_WRITE(pAd, MT76x0_DCOC_Tab, MT76x0_DCOC_Tab_Size);
#endif
}


static VOID MT76x0_AsicAntennaDefaultReset(
	IN struct _RTMP_ADAPTER	*pAd,
	IN EEPROM_ANTENNA_STRUC *pAntenna)
{
	pAntenna->word = 0;
	pAntenna->field.RfIcType = RFIC_7650;
	pAntenna->field.TxPath = 1;
	pAntenna->field.RxPath = 1;
}


static VOID MT76x0_ChipBBPAdjust(RTMP_ADAPTER *pAd)
{
	static char *ext_str[]={"extNone", "extAbove", "", "extBelow"};
	UINT8 rf_bw, ext_ch;

#ifdef DOT11_N_SUPPORT
	if (get_ht_cent_ch(pAd, &rf_bw, &ext_ch) == FALSE)
#endif /* DOT11_N_SUPPORT */
	{
		rf_bw = BW_20;
		ext_ch = EXTCHA_NONE;
		pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel;
	}

#ifdef DOT11_VHT_AC
	if (WMODE_CAP(pAd->CommonCfg.PhyMode, WMODE_AC) &&
		(pAd->CommonCfg.Channel > 14) &&
		(rf_bw == BW_40) &&
		(pAd->CommonCfg.vht_bw == VHT_BW_80) &&
		(pAd->CommonCfg.vht_cent_ch != pAd->CommonCfg.CentralChannel))
	{
		rf_bw = BW_80;
		pAd->CommonCfg.vht_cent_ch = vht_cent_ch_freq(pAd, pAd->CommonCfg.Channel);
	}

//+++Add by shiang for debug
	DBGPRINT(RT_DEBUG_OFF, ("%s():rf_bw=%d, ext_ch=%d, PrimCh=%d, HT-CentCh=%d, VHT-CentCh=%d\n",
				__FUNCTION__, rf_bw, ext_ch, pAd->CommonCfg.Channel,
				pAd->CommonCfg.CentralChannel, pAd->CommonCfg.vht_cent_ch));
//---Add by shiang for debug
#endif /* DOT11_VHT_AC */

	bbp_set_bw(pAd, rf_bw);

	/* TX/Rx : control channel setting */
	rtmp_mac_set_ctrlch(pAd, ext_ch);
	bbp_set_ctrlch(pAd, ext_ch);
		
#ifdef DOT11_N_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("%s() : %s, ChannelWidth=%d, Channel=%d, ExtChanOffset=%d(%d) \n",
					__FUNCTION__, ext_str[ext_ch],
					pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth,
					pAd->CommonCfg.Channel,
					pAd->CommonCfg.RegTransmitSetting.field.EXTCHA,
					pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset));
#endif /* DOT11_N_SUPPORT */
}


static VOID MT76x0_ChipSwitchChannel(
	struct _RTMP_ADAPTER *pAd,
	UCHAR Channel,
	BOOLEAN	 bScan)
{
	CHAR TxPwer = 0; /* Bbp94 = BBPR94_DEFAULT, TxPwer2 = DEFAULT_RF_TX_POWER; */
	UINT32 RegValue = 0;
	UINT32 Index;
	UINT32 Value = 0, rf_phy_mode, rf_bw = RF_BW_20;
	UCHAR bbp_ch_idx = 0;

#ifdef DOT11_VHT_AC
	bbp_ch_idx = vht_prim_ch_idx(Channel, pAd->CommonCfg.Channel);
#endif /* DOT11_VHT_AC */

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): MAC_STATUS_CFG = 0x%08x, bbp_ch_idx = %d\n", __FUNCTION__, RegValue, bbp_ch_idx));

	if (Channel > 14)
		rf_phy_mode = RF_A_BAND;
	else
		rf_phy_mode = RF_G_BAND;

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		UINT32 ret;
		
		RTMP_SEM_EVENT_WAIT(&pAd->hw_atomic, ret);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
			return;
		}
	}
#endif /* RTMP_MAC_USB */

	RTMP_IO_READ32(pAd, EXT_CCA_CFG, &RegValue);
	RegValue &= ~(0xFFF);
#ifdef DOT11_VHT_AC
	if (pAd->CommonCfg.BBPCurrentBW == BW_80)
	{
		rf_bw = RF_BW_80;
		if (bbp_ch_idx == 0)
		{
			RegValue |= 0x1e4;
		}
		else if (bbp_ch_idx == 1)
		{
			RegValue |= 0x2e1;
		}
		else if (bbp_ch_idx == 2)
		{
			RegValue |= 0x41e;
		}
		else if (bbp_ch_idx == 3)
		{
			RegValue |= 0x81b;
		}
	}
	else
#endif /* DOT11_VHT_AC */
	if (pAd->CommonCfg.BBPCurrentBW == BW_40)
	{
		rf_bw = RF_BW_40;
		if (pAd->CommonCfg.CentralChannel > pAd->CommonCfg.Channel)
			RegValue |= 0x1e4;
		else
			RegValue |= 0x2e1;
	}
	else
	{
		rf_bw = RF_BW_20;
		RegValue |= 0x1e4;
		
	}
	RTMP_IO_WRITE32(pAd, EXT_CCA_CFG, RegValue);

#ifdef CONFIG_AP_SUPPORT
#ifdef AP_QLOAD_SUPPORT
	/* clear all statistics count for QBSS Load */
	QBSS_LoadStatusClear(pAd);
#endif /* AP_QLOAD_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	/*
		Configure 2.4/5GHz before accessing other MAC/BB/RF registers
	*/
	mt76x0_band_select(pAd, Channel);

	/*
		Set RF channel frequency parameters (Rdiv, N, K, D and Ksd)
	*/
	mt76x0_set_rf_ch_freq_prm(pAd, Channel);

	for (Index = 0; Index < MAX_NUM_OF_CHANNELS; Index++)
	{
		if (Channel == pAd->TxPower[Index].Channel)
		{
			TxPwer = pAd->TxPower[Index].Power;
			break;
		}
	}	

	/* set Japan Tx filter at channel 14 */
	RTMP_BBP_IO_READ32(pAd, CORE_R1, &RegValue);
	if (Channel == 14)
		RegValue |= 0x20;		
	else
		RegValue &= (~0x20);
	RTMP_BBP_IO_WRITE32(pAd, CORE_R1, RegValue);
	
	for (Index = 0; Index < MT76x0_BPP_SWITCH_Tab_Size; Index++)
	{
		if (((rf_phy_mode | rf_bw) & MT76x0_BPP_SWITCH_Tab[Index].BwBand) == (rf_phy_mode | rf_bw))
		{
			if ((MT76x0_BPP_SWITCH_Tab[Index].RegDate.Register == AGC1_R8))
			{
				UINT32 eLNAgain = (MT76x0_BPP_SWITCH_Tab[Index].RegDate.Value & 0x0000FF00) >> 8;

				if (Channel > 14)
				{
					if (Channel < pAd->chipCap.a_band_mid_ch)
					eLNAgain -= (pAd->ALNAGain0*2);
					else if (Channel < pAd->chipCap.a_band_high_ch)
					eLNAgain -= (pAd->ALNAGain1*2);
				else
					eLNAgain -= (pAd->ALNAGain2*2);
				}
				else
					eLNAgain -= (pAd->BLNAGain*2);
				
				RTMP_BBP_IO_WRITE32(pAd, MT76x0_BPP_SWITCH_Tab[Index].RegDate.Register,
						(MT76x0_BPP_SWITCH_Tab[Index].RegDate.Value&(~0x0000FF00))|(eLNAgain << 8));				
#ifdef DYNAMIC_VGA_SUPPORT
				/* Backup LNA gain  */
				pAd->CommonCfg.lna_vga_ctl.agc_vga_init_0 = (UCHAR) eLNAgain;
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("Stored_BBP_InitGain=%x !!!\n", pAd->CommonCfg.lna_vga_ctl.agc_vga_init_0));
#endif /* RELEASE_EXCLUDE */
#endif /* DYNAMIC_VGA_SUPPORT */

			}
			else
			{
				RTMP_BBP_IO_WRITE32(pAd, MT76x0_BPP_SWITCH_Tab[Index].RegDate.Register,
						MT76x0_BPP_SWITCH_Tab[Index].RegDate.Value);
			}
		}
	}

	/* 
		VCO calibration (mode 3) 
	*/
	mt76x0_vco_calibration(pAd, Channel);
	
	if (bScan)
		mt76x0_calibration(pAd, Channel, FALSE, FALSE, FALSE);

	RtmpusecDelay(1000);

#ifdef SINGLE_SKU_V2
	mt76x0_adjust_per_rate_pwr(pAd);
#endif /* SINGLE_SKU_V2 */

#ifndef MT76x0_TSSI_CAL_COMPENSATION
	RTMP_IO_READ32(pAd, TX_ALC_CFG_0, &Value);
	Value = Value & (~0x3F3F);
	Value |= TxPwer;
	Value |= (0x2F2F << 16);
	RTMP_IO_WRITE32(pAd, TX_ALC_CFG_0, Value);
#endif /* !MT76x0_TSSI_CAL_COMPENSATION */

#if 0
	/*
	  On 11A, We should delay and wait RF/BBP to be stable
	  and the appropriate time should be 1000 micro seconds
	  2005/06/05 - On 11G, We also need this delay time. Otherwise it's difficult to pass the WHQL.
	*/
	RtmpusecDelay(1000);
#endif /* 0 */

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		RTMP_SEM_EVENT_UP(&pAd->hw_atomic);
	}
#endif /* RTMP_MAC_USB */

	if (Channel > 14) {
		RTMP_IO_WRITE32(pAd, XIFS_TIME_CFG, 0x33a41010);
	} else {
#ifdef RELEASE_EXCLUDE
		/* MT7650 MAC bug: MT7650_E3_CR_setting_20130801.xlsx */
#endif /* RELEASE_EXCLUDE */
		RTMP_IO_WRITE32(pAd, XIFS_TIME_CFG, 0x33a4100A);
	}
	return;
}


#ifdef CONFIG_STA_SUPPORT
static VOID MT76x0_NetDevNickNameInit(RTMP_ADAPTER *pAd)
{
#ifdef RTMP_MAC_PCI
	if (IS_MT7650E(pAd))
		snprintf((RTMP_STRING *) pAd->nickname, sizeof(pAd->nickname), "MT7650E_STA");
	else if (IS_MT7630E(pAd))
		snprintf((RTMP_STRING *) pAd->nickname, sizeof(pAd->nickname), "MT7630E_STA");
	else if (IS_MT7610E(pAd))	
		snprintf((RTMP_STRING *) pAd->nickname, sizeof(pAd->nickname), "MT7610E_STA");
#endif

#ifdef RTMP_MAC_USB
	if (IS_MT7650U(pAd))
		snprintf((RTMP_STRING *) pAd->nickname, sizeof(pAd->nickname), "MT7650U_STA");
	else if (IS_MT7630U(pAd))
		snprintf((RTMP_STRING *) pAd->nickname, sizeof(pAd->nickname), "MT7630U_STA");
	else if (IS_MT7610U(pAd))	
		snprintf((RTMP_STRING *) pAd->nickname, sizeof(pAd->nickname), "MT7610U_STA");
#endif
}
#endif /* CONFIG_STA_SUPPORT */


VOID MT76x0_NICInitAsicFromEEPROM(
	IN PRTMP_ADAPTER		pAd)
{
	USHORT e2p_value = 0;

	if (IS_MT7610(pAd) || IS_MT7650(pAd)) {
		/* MT7650_E3_CR_setting_20130416.xlsx */
		RT28xx_EEPROM_READ16(pAd, 0x24, e2p_value);
		if ((e2p_value & 0x30) == 0x30) {
			RTMP_IO_WRITE32(pAd, TX_SW_CFG1, 0x00040200);
		} else {
			RTMP_IO_WRITE32(pAd, TX_SW_CFG1, 0x00040000);
		}
	}
}


INT MT76x0_ReadChannelPwr(RTMP_ADAPTER *pAd)
{
	UINT32 i, choffset, idx, ss_offset_g, ss_num;
	EEPROM_TX_PWR_STRUC Power;
	CHAR tx_pwr1, tx_pwr2;

	DBGPRINT(RT_DEBUG_TRACE, ("%s()--->\n", __FUNCTION__));
	
	choffset = 0;
	ss_num = 1;

	for (i = 0; i < sizeof(mt76x0_txpwr_chlist); i++)
	{
		pAd->TxPower[i].Channel = mt76x0_txpwr_chlist[i];
		pAd->TxPower[i].Power = DEFAULT_RF_TX_POWER;	
	}


	/* 0. 11b/g, ch1 - ch 14, 1SS */
	ss_offset_g = EEPROM_G_TX_PWR_OFFSET;
	for (i = 0; i < 7; i++)
	{
		idx = i * 2;
		RT28xx_EEPROM_READ16(pAd, ss_offset_g + idx, Power.word);

		tx_pwr1 = tx_pwr2 = DEFAULT_RF_TX_POWER;

		if ((Power.field.Byte0 <= 0x3F) && (Power.field.Byte0 >= 0))
			tx_pwr1 = Power.field.Byte0;

		if ((Power.field.Byte1 <= 0x3F) || (Power.field.Byte1 >= 0))
			tx_pwr2 = Power.field.Byte1;

		pAd->TxPower[idx].Power = tx_pwr1;
		pAd->TxPower[idx + 1].Power = tx_pwr2;
		choffset++;
	}



	{
		/* 1. U-NII lower/middle band: 36, 38, 40; 44, 46, 48; 52, 54, 56; 60, 62, 64 (including central frequency in BW 40MHz)*/
		choffset = 14;
		ASSERT((pAd->TxPower[choffset].Channel == 36));
		for (i = 0; i < 6; i++)
		{
			idx = i * 2;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + idx, Power.word);

			if ((Power.field.Byte0 <= 0x3F) && (Power.field.Byte0 >= 0))
				pAd->TxPower[idx + choffset + 0].Power = Power.field.Byte0;

			if ((Power.field.Byte1 <= 0x3F) && (Power.field.Byte1 >= 0))
				pAd->TxPower[idx + choffset + 1].Power = Power.field.Byte1;
		}


		/* 2. HipperLAN 2 100, 102 ,104; 108, 110, 112; 116, 118, 120; 124, 126, 128; 132, 134, 136; 140 (including central frequency in BW 40MHz)*/
		choffset = 14 + 12;
		ASSERT((pAd->TxPower[choffset].Channel == 100));
		for (i = 0; i < 8; i++)
		{

			idx = i * 2;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + idx, Power.word);
			
			if ((Power.field.Byte0 <= 0x3F) && (Power.field.Byte0 >= 0))
				pAd->TxPower[idx + choffset + 0].Power = Power.field.Byte0;

			if ((Power.field.Byte1 <= 0x3F) && (Power.field.Byte1 >= 0))
				pAd->TxPower[idx + choffset + 1].Power = Power.field.Byte1;
		}


		/* 3. U-NII upper band: 149, 151, 153; 157, 159, 161; 165, 167, 169; 171, 173 (including central frequency in BW 40MHz)*/
		choffset = 14 + 12 + 16;
		ASSERT((pAd->TxPower[choffset].Channel == 149));
		for (i = 0; i < 6; i++)
		{
			idx = i * 2;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + idx, Power.word);

			if ((Power.field.Byte0 <= 0x3F) && (Power.field.Byte0 >= 0))
				pAd->TxPower[idx + choffset + 0].Power = Power.field.Byte0;

			if ((Power.field.Byte1 <= 0x3F) && (Power.field.Byte1 >= 0))
				pAd->TxPower[idx + choffset + 1].Power = Power.field.Byte1;
		}

		/* choffset = 14 + 12 + 16 + 7; */
		choffset = 14 + 12 + 16 + 11;

#ifdef DOT11_VHT_AC
		ASSERT((pAd->TxPower[choffset].Channel == 42));

		/* For VHT80MHz, we need assign tx power for central channel 42, 58, 106, 122, and 155 */
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Update Tx power control of the central channel (42, 58, 106, 122 and 155) for VHT BW80\n", __FUNCTION__));
		
		NdisMoveMemory(&pAd->TxPower[53], &pAd->TxPower[16], sizeof(CHANNEL_TX_POWER)); // channel 42 = channel 40
		NdisMoveMemory(&pAd->TxPower[54], &pAd->TxPower[22], sizeof(CHANNEL_TX_POWER)); // channel 58 = channel 56
		NdisMoveMemory(&pAd->TxPower[55], &pAd->TxPower[28], sizeof(CHANNEL_TX_POWER)); // channel 106 = channel 104
		NdisMoveMemory(&pAd->TxPower[56], &pAd->TxPower[34], sizeof(CHANNEL_TX_POWER)); // channel 122 = channel 120
		NdisMoveMemory(&pAd->TxPower[57], &pAd->TxPower[44], sizeof(CHANNEL_TX_POWER)); // channel 155 = channel 153

		pAd->TxPower[choffset].Channel = 42;
		pAd->TxPower[choffset+1].Channel = 58;
		pAd->TxPower[choffset+2].Channel = 106;
		pAd->TxPower[choffset+3].Channel = 122;
		pAd->TxPower[choffset+4].Channel = 155;
		
		choffset += 5;		/* the central channel of VHT80 */
		
		choffset = (MAX_NUM_OF_CHANNELS - 1);
#endif /* DOT11_VHT_AC */


		/* 4. Print and Debug*/
		for (i = 0; i < choffset; i++)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: TxPower[%03d], Channel=%d, Power[Tx:%d]\n",
						i, pAd->TxPower[i].Channel, pAd->TxPower[i].Power));
		}
	}

	return TRUE;
}


INT MT76x0_DisableTxRx(
	RTMP_ADAPTER *pAd,
	UCHAR Level)
{
	UINT32 MacReg = 0;
	UINT32 MTxCycle;
	BOOLEAN bResetWLAN = FALSE;

	if (!IS_MT76x0(pAd))
		return 0;

	DBGPRINT(RT_DEBUG_TRACE, ("----> %s\n", __FUNCTION__));

	if (Level == RTMP_HALT)
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s Tx success = %ld\n", 
		__FUNCTION__, (ULONG)pAd->WlanCounters.TransmittedFragmentCount.u.LowPart));
	DBGPRINT(RT_DEBUG_TRACE, ("%s Rx success = %ld\n", 
		__FUNCTION__, (ULONG)pAd->WlanCounters.ReceivedFragmentCount.QuadPart));

	StopDmaTx(pAd, Level);

	/*
		Check page count in TxQ,
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		BOOLEAN bFree = TRUE;
		RTMP_IO_READ32(pAd, 0x438, &MacReg);
		if (MacReg != 0)
			bFree = FALSE;
		RTMP_IO_READ32(pAd, 0xa30, &MacReg);
		if (MacReg & 0x000000FF)
			bFree = FALSE;
		RTMP_IO_READ32(pAd, 0xa34, &MacReg);
		if (MacReg & 0xFF00FF00)
			bFree = FALSE;
		if (bFree)
			break;
		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return 0;
		}
	}

	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check TxQ page count max\n"));
		RTMP_IO_READ32(pAd, 0x0a30, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x\n", MacReg));

		RTMP_IO_READ32(pAd, 0x0a34, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x\n", MacReg));

		RTMP_IO_READ32(pAd, 0x438, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x438 = 0x%08x\n", MacReg));
		bResetWLAN = TRUE;
	}

	/*
		Check MAC Tx idle
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		RTMP_IO_READ32(pAd, MAC_STATUS_CFG, &MacReg);
		if (MacReg & 0x1)
			RtmpusecDelay(50);
		else
			break;
		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return 0;
		}
	}

	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check MAC Tx idle max(0x%08x)\n", MacReg));
		bResetWLAN = TRUE;
	}
	
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) == FALSE)
	{
		if (Level == RTMP_HALT)
		{
			/*
				Disable MAC TX/RX
			*/
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacReg);
			MacReg &= ~(0x0000000c);
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacReg);
		}
		else
		{
			/*
				Disable MAC RX
			*/
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacReg);
			MacReg &= ~(0x00000008);
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacReg);
		}
	}

	/*
		Check page count in RxQ,
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		BOOLEAN bFree = TRUE;
		RTMP_IO_READ32(pAd, 0x430, &MacReg);
		if (MacReg & (0x00FF0000))
			bFree = FALSE;
		RTMP_IO_READ32(pAd, 0xa30, &MacReg);
		if (MacReg != 0)
			bFree = FALSE;
		RTMP_IO_READ32(pAd, 0xa34, &MacReg);
		if (MacReg != 0)
			bFree = FALSE;
		if (bFree)
			break;
		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return 0;
		}
#ifdef RTMP_MAC_USB
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE);
		usb_rx_cmd_msgs_receive(pAd);
		RTUSBBulkReceive(pAd);
#endif /* RTMP_MAC_USB */
	}

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE);
	
	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check RxQ page count max\n"));
		
		RTMP_IO_READ32(pAd, 0x0a30, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x\n", MacReg));

		RTMP_IO_READ32(pAd, 0x0a34, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x\n", MacReg));

		RTMP_IO_READ32(pAd, 0x0430, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0430 = 0x%08x\n", MacReg));
		bResetWLAN = TRUE;
	}

	/*
		Check MAC Rx idle
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		RTMP_IO_READ32(pAd, MAC_STATUS_CFG, &MacReg);
		if (MacReg & 0x2)
			RtmpusecDelay(50);
		else
			break;
		if (MacReg == 0xFFFFFFFF)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			return 0;
		}
	}
	
	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check MAC Rx idle max(0x%08x)\n", MacReg));
		bResetWLAN = TRUE;
	}

	StopDmaRx(pAd, Level);

	if ((Level == RTMP_HALT) &&
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) == FALSE))
	{
		if (!pAd->chipCap.IsComboChip)
			NICEraseFirmware(pAd);
		
		/*
 		 * Disable RF/MAC and do not do reset WLAN under below cases
 		 * 1. Combo card
 		 * 2. suspend including wow application
 		 * 3. radion off command
 		 */
		if ((pAd->chipCap.IsComboChip) || RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_SUSPEND)
				|| RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_CMD_RADIO_OFF))
			bResetWLAN = 0;
		
		rlt_wlan_chip_onoff(pAd, FALSE, bResetWLAN);
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("<---- %s\n", __FUNCTION__));

	return 0;
}


VOID MT76x0_AsicExtraPowerOverMAC(
	IN PRTMP_ADAPTER pAd)
{
	UINT32 ExtraPwrOverMAC = 0;
	UINT32 ExtraPwrOverTxPwrCfg7 = 0, ExtraPwrOverTxPwrCfg8 = 0, ExtraPwrOverTxPwrCfg9 = 0;

	/* 
		For OFDM_54 and HT_MCS_7, extra fill the corresponding register value into MAC 0x13D4 
		bit 21:16 -> HT/VHT MCS 7
		bit 5:0 -> OFDM 54
	*/
	RTMP_IO_READ32(pAd, TX_PWR_CFG_1, &ExtraPwrOverMAC);  
	ExtraPwrOverTxPwrCfg7 |= (ExtraPwrOverMAC & 0x00003F00) >> 8; /* Get Tx power for OFDM 54 */
	RTMP_IO_READ32(pAd, TX_PWR_CFG_2, &ExtraPwrOverMAC);  
	ExtraPwrOverTxPwrCfg7 |= (ExtraPwrOverMAC & 0x00003F00) << 8; /* Get Tx power for HT MCS 7 */			
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_7, ExtraPwrOverTxPwrCfg7);

	/*  
		For HT_MCS_15, extra fill the corresponding register value into MAC 0x13D8 
		bit 29:24 -> VHT 1SS MCS 9
		bit 21:16 -> VHT 1SS MCS 8
		bit 5:0 -> HT MCS 15
	*/
	RTMP_IO_READ32(pAd, TX_PWR_CFG_3, &ExtraPwrOverMAC);  
#ifdef DOT11_VHT_AC
	ExtraPwrOverTxPwrCfg8 = pAd->Tx80MPwrCfgABand[0] | (ExtraPwrOverMAC & 0x0000FF00) >> 8; /* Get Tx power for HT MCS 15 */	
#else
	ExtraPwrOverTxPwrCfg8 |= (ExtraPwrOverMAC & 0x0000FF00) >> 8; /* Get Tx power for HT MCS 15 */
#endif /* DOT11_VHT_AC */
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_8, ExtraPwrOverTxPwrCfg8);

	/* 
		For STBC_MCS_7, extra fill the corresponding register value into MAC 0x13DC 
		bit 5:0 -> STBC MCS 7
	*/
	RTMP_IO_READ32(pAd, TX_PWR_CFG_4, &ExtraPwrOverMAC);  
	ExtraPwrOverTxPwrCfg9 |= (ExtraPwrOverMAC & 0x00003F00) >> 8; /* Get Tx power for STBC MCS 7 */
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_9, ExtraPwrOverTxPwrCfg9);

	DBGPRINT(RT_DEBUG_INFO, ("0x13D4 = 0x%08X, 0x13D8 = 0x%08X, 0x13D4 = 0x%08X\n", 
			(UINT)ExtraPwrOverTxPwrCfg7, (UINT)ExtraPwrOverTxPwrCfg8, (UINT)ExtraPwrOverTxPwrCfg9));	
}


static VOID calc_bw_delta_pwr(
	IN BOOLEAN is_dec_delta,
	IN USHORT input_pwr,
	IN USHORT bw_delta,
	INOUT CHAR *tx_pwr1,
	INOUT CHAR *tx_pwr2)
{
	CHAR tp_pwr1 = 0, tp_pwr2 = 0;
	UCHAR temp = 0;

	bw_delta &= 0x1F;

	/* 6-bit representation ==> 8-bit representation (2's complement) */
	temp = (input_pwr & 0x3F);
	tp_pwr1 = (char)((temp & 0x20) ? (temp | 0xC0): (temp & 0x3f));

	temp = ((input_pwr & 0x3F00) >> 8);
	tp_pwr2 = (char)((temp & 0x20) ? (temp | 0xC0): (temp & 0x3f));

	if (is_dec_delta == FALSE) {
		tp_pwr1 += bw_delta;
		tp_pwr2 += bw_delta;
	} else {
		tp_pwr1 -= bw_delta;
		tp_pwr2 -= bw_delta;		
	}	

	/* 8-bit representation ==> 6-bit representation (2's complement) */
	*tx_pwr1 = (tp_pwr1 & 0x80) ? \
					((tp_pwr1 & 0x1f) | 0x20) : (tp_pwr1 & 0x1f);
	*tx_pwr2 = (tp_pwr2 & 0x80) ? \
					((tp_pwr2 & 0x1f) | 0x20) : (tp_pwr2 & 0x1f);
}


/*
	Read per-rate Tx power
*/
void mt76x0_read_per_rate_tx_pwr(
	IN PRTMP_ADAPTER pAd)
{
	UINT32 data;
	USHORT e2p_val = 0, e2p_val2 = 0;
	UCHAR bw40_gband_delta = 0, bw40_aband_delta = 0, bw80_aband_delta = 0;
	CHAR t1 = 0, t2 = 0, t3 = 0, t4 = 0;
	BOOLEAN dec_aband_bw40_delta = FALSE, dec_aband_bw80_delta = FALSE, dec_gband_bw40_delta = FALSE;

    	DBGPRINT(RT_DEBUG_TRACE, ("%s() -->\n", __FUNCTION__));
	
	/*
		Get power delta for BW40
		bit 5:0 -> 40M BW TX power delta value (MAX=4dBm)
		bit 6 -> 	1: increase 40M BW TX power with the delta value
			     	0: decrease 40M BW TX power with the delta value
		bit 7 -> 	enableTX power compensation
	*/
	RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_DELTA, e2p_val);
	pAd->chipCap.delta_tw_pwr_bw40_2G = (e2p_val & 0xFF) == 0xFF ? 0 : (e2p_val & 0xFF);
	pAd->chipCap.delta_tw_pwr_bw40_5G = (e2p_val & 0xFF00) == 0xFF00 ? 0 : ((e2p_val >> 8) & 0xFF);

	if ((e2p_val & 0xFF) != 0xFF) {
		if (e2p_val & 0x80)
			bw40_gband_delta = (e2p_val & 0x1F);

		if (e2p_val & 0x40)
			dec_gband_bw40_delta = FALSE;
		else
			dec_gband_bw40_delta = TRUE;
	}

	if ((e2p_val & 0xFF00) != 0xFF00) {
		if (e2p_val & 0x8000)
			bw40_aband_delta = ((e2p_val & 0x1F00) >> 8);

		if (e2p_val & 0x4000)
			dec_aband_bw40_delta = FALSE;
		else
			dec_aband_bw40_delta = TRUE;
	}
	
	/*
		Get power delta for BW80
	*/
	// TODO: check if any document to describe this ?
	RT28xx_EEPROM_READ16(pAd, EEPROM_VHT_BW80_TX_POWER_DELTA - 1, e2p_val);
	pAd->chipCap.delta_tw_pwr_bw80 = (e2p_val & 0xFF00) == 0xFF00 ? 0 : (e2p_val & 0xFF);

	if ((e2p_val & 0xFF00) != 0xFF00) {
		if (e2p_val & 0x8000)
			bw80_aband_delta = ((e2p_val & 0x1F00) >> 8);
	
		if (e2p_val & 0x4000)
			dec_aband_bw80_delta = FALSE;
		else
			dec_aband_bw80_delta = TRUE;
	}

#ifdef SINGLE_SKU_V2
	/*
		We don't need to update bw delta for per rate when SingleSKU is enabled
	*/
	dec_aband_bw40_delta = FALSE;
	dec_aband_bw80_delta = FALSE;
	dec_gband_bw40_delta = FALSE;
	bw40_aband_delta = 0;
	bw80_aband_delta = 0;
	bw40_gband_delta = 0;
#endif /* SINGLE_SKU_V2 */

	DBGPRINT(RT_DEBUG_TRACE, ("%s: dec_gband_bw40_delta = %d, bw40_gband_delta = %d\n", 
		__FUNCTION__, dec_gband_bw40_delta, bw40_gband_delta));	
	DBGPRINT(RT_DEBUG_TRACE, ("%s: dec_aband_bw40_delta = %d, bw40_aband_delta = %d\n", 
		__FUNCTION__, dec_aband_bw40_delta, bw40_aband_delta));
	DBGPRINT(RT_DEBUG_TRACE, ("%s: dec_aband_bw80_delta = %d, bw80_aband_delta = %d\n", 
		__FUNCTION__, dec_aband_bw80_delta, bw80_aband_delta));

	RT28xx_EEPROM_READ16(pAd, 0xDE, e2p_val);
	calc_bw_delta_pwr(dec_gband_bw40_delta, e2p_val, bw40_gband_delta, &t1, &t2);
	RT28xx_EEPROM_READ16(pAd, 0xE0, e2p_val2);
	calc_bw_delta_pwr(dec_gband_bw40_delta, e2p_val2, bw40_gband_delta, &t3, &t4);
	/* 
		bit 29:24 -> OFDM 12M/18M
		bit 21:16 -> OFDM 6M/9M
		bit 13:8 -> CCK 5.5M/11M
		bit 5:0 -> CCK 1M/2M
	*/
	data = (e2p_val2 << 16) | e2p_val;
	pAd->Tx20MPwrCfgGBand[0] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgGBand[0](0x1314) = 0x%08X\n", __FUNCTION__, data));
	data = (t4 << 24) | (t3 << 16) | (t2 << 8) | t1; 
	pAd->Tx40MPwrCfgGBand[0] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgGBand[0](0x1314) = 0x%08X\n", __FUNCTION__, data));

	RT28xx_EEPROM_READ16(pAd, 0xE2, e2p_val);
	calc_bw_delta_pwr(dec_gband_bw40_delta, e2p_val, bw40_gband_delta, &t1, &t2);
	RT28xx_EEPROM_READ16(pAd, 0xE4, e2p_val2);
	calc_bw_delta_pwr(dec_gband_bw40_delta, e2p_val2, bw40_gband_delta, &t3, &t4);
	/* 
		bit 29:24 -> HT MCS=2,3, VHT 1SS MCS=2,3
		bit 21:16 -> HT MCS=0,1, VHT 1SS MCS=0,1
		bit 13:8 -> OFDM 48M
		bit 5:0 -> OFDM 24M/36M
	*/
	data = (e2p_val2 << 16) | e2p_val;
	pAd->Tx20MPwrCfgGBand[1] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgGBand[1](0x1318) = 0x%08X\n", __FUNCTION__, data));
	data = (t4 << 24) | (t3 << 16) | (t2 << 8) | t1; 
	pAd->Tx40MPwrCfgGBand[1] = data; 
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgGBand[1](0x1318) = 0x%08X\n", __FUNCTION__, data));

	RT28xx_EEPROM_READ16(pAd, 0xE6, e2p_val);
	calc_bw_delta_pwr(dec_gband_bw40_delta, e2p_val, bw40_gband_delta, &t1, &t2);
	RT28xx_EEPROM_READ16(pAd, 0xE8, e2p_val2);
	calc_bw_delta_pwr(dec_gband_bw40_delta, e2p_val2, bw40_gband_delta, &t3, &t4);
	/*
		bit 29:24 -> HT MCS=10,11 (no need)
		bit 21:16 -> HT MCS=8,9 (no need)
		bit 13:8 -> HT MCS=6, VHT 1SS MCS=6
		bit 5:0 -> MCS=4,5, VHT 1SS MCS=4,5
	*/
	data = (e2p_val2 << 16) | e2p_val;
	pAd->Tx20MPwrCfgGBand[2] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgGBand[2](0x131C) = 0x%08X\n", __FUNCTION__, data));
	data = (t4 << 24) | (t3 << 16) | (t2 << 8) | t1; 
	pAd->Tx40MPwrCfgGBand[2] = data; 
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgGBand[2](0x131C) = 0x%08X\n", __FUNCTION__, data));

	RT28xx_EEPROM_READ16(pAd, 0xEA, e2p_val);
	calc_bw_delta_pwr(dec_gband_bw40_delta, e2p_val, bw40_gband_delta, &t1, &t2);
	RT28xx_EEPROM_READ16(pAd, 0xEC, e2p_val2);
	calc_bw_delta_pwr(dec_gband_bw40_delta, e2p_val2, bw40_gband_delta, &t3, &t4);
	/* 
		bit 29:24 -> HT/VHT STBC MCS=2, 3
		bit 21:16 -> HT/VHT STBC MCS=0, 1
		bit 13:8 -> HT MCS=14 (no need)
		bit 5:0 -> HT MCS=12,13 (no need)
	*/
	data = (e2p_val2 << 16) | e2p_val;
	pAd->Tx20MPwrCfgGBand[3] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgGBand[3](0x1320) = 0x%08X\n", __FUNCTION__, data));
	data = (t4 << 24) | (t3 << 16) | (t2 << 8) | t1; 
	pAd->Tx40MPwrCfgGBand[3] = data; 
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgGBand[3](0x1320) = 0x%08X\n", __FUNCTION__, data));

	RT28xx_EEPROM_READ16(pAd, 0xEE, e2p_val);
	calc_bw_delta_pwr(dec_gband_bw40_delta, e2p_val, bw40_gband_delta, &t1, &t2);
	/* 
		bit 13:8 -> HT/VHT STBC MCS=6
		bit 5:0 -> HT/VHT STBC MCS=4,5
	*/
	data = e2p_val;
	pAd->Tx20MPwrCfgGBand[4] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgGBand[4](0x1324) = 0x%08X\n", __FUNCTION__, data));
	data = (t2 << 8) | t1;
	pAd->Tx40MPwrCfgGBand[4] = data; 			
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgGBand[4](0x1324) = 0x%08X\n", __FUNCTION__, data));
	

	RT28xx_EEPROM_READ16(pAd, 0x120, e2p_val);
	calc_bw_delta_pwr(dec_aband_bw40_delta, e2p_val, bw40_aband_delta, &t3, &t4);
	/* 
		bit 29:24 -> OFDM 12M/18M
		bit 21:16 -> OFDM 6M/9M
	*/
	data = e2p_val;
	data = data << 16;
	pAd->Tx20MPwrCfgABand[0] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgABand[0](0x1314) = 0x%08X\n", __FUNCTION__, data));
	data = (t4 << 24) | (t3 << 16); 
	pAd->Tx40MPwrCfgABand[0] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgABand[0](0x1314) = 0x%08X\n", __FUNCTION__, data));	

	RT28xx_EEPROM_READ16(pAd, 0x122, e2p_val);
	calc_bw_delta_pwr(dec_aband_bw40_delta, e2p_val, bw40_aband_delta, &t1, &t2);
	RT28xx_EEPROM_READ16(pAd, 0x124, e2p_val2);
	calc_bw_delta_pwr(dec_aband_bw40_delta, e2p_val2, bw40_aband_delta, &t3, &t4);
	/* 
		bit 29:24 -> HT MCS=2,3, VHT 1SS MCS=2,3
		bit 21:16 -> HT MCS=0,1, VHT 1SS MCS=0,1
		bit 13:8 -> OFDM 48M
		bit 5:0 -> OFDM 24M/36M
	*/
	data = (e2p_val2 << 16) | e2p_val;
	pAd->Tx20MPwrCfgABand[1] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgABand[1](0x1318) = 0x%08X\n", __FUNCTION__, data));
	data = (t4 << 24) | (t3 << 16) | (t2 << 8) | t1; 
	pAd->Tx40MPwrCfgABand[1] = data;			
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgABand[1](0x1318) = 0x%08X\n", __FUNCTION__, data));

	RT28xx_EEPROM_READ16(pAd, 0x126, e2p_val);
	calc_bw_delta_pwr(dec_aband_bw40_delta, e2p_val, bw40_aband_delta, &t1, &t2);
	/*
		bit 13:8 -> HT MCS=6, VHT 1SS MCS=6
		bit 5:0 -> MCS=4,5, VHT 1SS MCS=4,5
	*/
	data = e2p_val;
	pAd->Tx20MPwrCfgABand[2] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgABand[2](0x131C) = 0x%08X\n", __FUNCTION__, data));
	data = (t2 << 8) | t1; 
	pAd->Tx40MPwrCfgABand[2] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgABand[2](0x131C) = 0x%08X\n", __FUNCTION__, data));

	RT28xx_EEPROM_READ16(pAd, 0xEC, e2p_val);
	calc_bw_delta_pwr(dec_aband_bw40_delta, e2p_val, bw40_aband_delta, &t3, &t4);
	/* 
		bit 29:24 -> HT/VHT STBC MCS=2, 3
		bit 21:16 -> HT/VHT STBC MCS=0, 1
	*/
	data = e2p_val;
	data = data << 16;
	pAd->Tx20MPwrCfgABand[3] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgABand[3](0x1320) = 0x%08X\n", __FUNCTION__, data));
	data = (t4 << 24) | (t3 << 16); 
	pAd->Tx40MPwrCfgABand[3] = data;
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgABand[3](0x1320) = 0x%08X\n", __FUNCTION__, data));

	RT28xx_EEPROM_READ16(pAd, 0xEE, e2p_val);
	calc_bw_delta_pwr(dec_aband_bw40_delta, e2p_val, bw40_aband_delta, &t1, &t2);
	/* 
		bit 13:8 -> HT/VHT STBC MCS=6
		bit 5:0 -> HT/VHT STBC MCS=4,5
	*/
	data = e2p_val;
	pAd->Tx20MPwrCfgABand[4] = data;	
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx20MPwrCfgABand[4](0x1324) = 0x%08X\n", __FUNCTION__, data));
	data = (t2 << 8) | t1;
	pAd->Tx40MPwrCfgABand[4] = data;	
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx40MPwrCfgABand[4](0x1324) = 0x%08X\n", __FUNCTION__, data));
	
	RT28xx_EEPROM_READ16(pAd, 0x12C, e2p_val);
	calc_bw_delta_pwr(dec_aband_bw80_delta, e2p_val, bw80_aband_delta, &t3, &t4);
	/* 
		bit 29:24 -> VHT 1SS MCS=9
		bit 21:16 -> VHT 1SS MCS=8
	*/
	data = (t3 << 24) | (t3 << 16); 
	pAd->Tx80MPwrCfgABand[0] = data;			
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s: Tx80MPwrCfgABand[0](0x13D8) = 0x%08X\n", __FUNCTION__, data));

#ifdef MT76x0_TSSI_CAL_COMPENSATION
	MT76x0_MakeUpTssiTable(pAd);
#endif /* MT76x0_TSSI_CAL_COMPENSATION */

    	DBGPRINT(RT_DEBUG_TRACE, ("%s: <--\n", __FUNCTION__));
}


#ifdef RTMP_MAC_PCI
#ifdef DBG
VOID MT76x0_ShowDmaIndexCupIndex(
	RTMP_ADAPTER *pAd)
{
	UINT32 RegVal = 0, index;
	RTMP_RX_RING *pRxRing;
	RTMP_TX_RING *pTxRing;

	/*
		RX
	*/
	for(index = 0; index < RX_RING_NUM; index++)
	{
		UINT32 RegVal = 0;
		pRxRing = &pAd->RxRing[index];

		RTMP_IO_READ32(pAd, pRxRing->hw_cidx_addr, &RegVal);
		printk(">>>> 0x%08x = 0x%08x\n", pRxRing->hw_cidx_addr, RegVal);
		RTMP_IO_READ32(pAd, pRxRing->hw_didx_addr, &RegVal);
		printk(">>>> 0x%08x = 0x%08x\n", pRxRing->hw_didx_addr, RegVal);
	}

	/*
		TX
	*/
	for(index = 0; index < NUM_OF_TX_RING; index++)
	{
		UINT32 RegVal = 0;
		pTxRing = &pAd->TxRing[index];
		RTMP_IO_READ32(pAd, pTxRing->hw_cidx_addr, &RegVal);
		printk(">>>> 0x%08x = 0x%08x\n", pTxRing->hw_cidx_addr, RegVal);
	}

	/*
		MGMT
	*/
	RTMP_IO_READ32(pAd, pAd->CtrlRing.hw_cidx_addr, &RegVal);
	printk(">>>> 0x%08x = 0x%08x\n", pAd->CtrlRing.hw_cidx_addr, RegVal);
}
#endif /* DBG */
#endif


static VOID mt76x0_asic_update_per_rate_pwr(
	IN PRTMP_ADAPTER pAd,
	INOUT PULONG per_rate_pwr)
{
	CONFIGURATION_OF_TX_POWER_CONTROL_OVER_MAC rate_pwr_table;
	INT32 idx = 0;
	
	DBGPRINT(RT_DEBUG_INFO, ("-->%s\n", __FUNCTION__));

	NdisZeroMemory(&rate_pwr_table, sizeof(rate_pwr_table));

	rate_pwr_table.NumOfEntries = 5; /* MAC 0x1314, 0x1318, 0x131C, 0x1320 and 1324 */
	rate_pwr_table.TxPwrCtrlOverMAC[0].MACRegisterOffset = TX_PWR_CFG_0;
	rate_pwr_table.TxPwrCtrlOverMAC[1].MACRegisterOffset = TX_PWR_CFG_1;
	rate_pwr_table.TxPwrCtrlOverMAC[2].MACRegisterOffset = TX_PWR_CFG_2;
	rate_pwr_table.TxPwrCtrlOverMAC[3].MACRegisterOffset = TX_PWR_CFG_3;
	rate_pwr_table.TxPwrCtrlOverMAC[4].MACRegisterOffset = TX_PWR_CFG_4;
	
	if (pAd->CommonCfg.BBPCurrentBW == BW_20) {
		if (pAd->CommonCfg.CentralChannel > 14) {
			for (idx = 0; idx < rate_pwr_table.NumOfEntries; idx++)
				rate_pwr_table.TxPwrCtrlOverMAC[idx].RegisterValue = pAd->Tx20MPwrCfgABand[idx];
		} else {
			for (idx = 0; idx < rate_pwr_table.NumOfEntries; idx++)
				rate_pwr_table.TxPwrCtrlOverMAC[idx].RegisterValue = pAd->Tx20MPwrCfgGBand[idx];
		}
	} else {
		if (pAd->CommonCfg.CentralChannel > 14) {
			for (idx = 0; idx < rate_pwr_table.NumOfEntries; idx++)
				rate_pwr_table.TxPwrCtrlOverMAC[idx].RegisterValue = pAd->Tx40MPwrCfgABand[idx];
		} else {
			for (idx = 0; idx < rate_pwr_table.NumOfEntries; idx++)
				rate_pwr_table.TxPwrCtrlOverMAC[idx].RegisterValue = pAd->Tx40MPwrCfgGBand[idx];
		}
	}
	
	NdisCopyMemory(per_rate_pwr, (UCHAR *)&rate_pwr_table, sizeof(rate_pwr_table));
	DBGPRINT(RT_DEBUG_INFO, ("<--%s\n", __FUNCTION__));
}


/*
========================================================================
Routine Description:
	Initialize MT76x0

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID MT76x0_Init(RTMP_ADAPTER *pAd)
{
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;
	UINT32 Value;

	DBGPRINT(RT_DEBUG_TRACE, ("-->%s():\n", __FUNCTION__));

	rlt_phy_probe(pAd);
	
	/* 
		Init chip capabilities
	*/
	RTMP_IO_READ32(pAd, 0x00, &Value);	
	pAd->ChipID = Value;

	pChipCap->hif_type = HIF_RLT;
	pChipCap->bbp_type= BBP_RLT;
	pChipCap->MCUType = ANDES;
	pChipCap->rf_type = RF_RLT,

	pChipCap->max_nss = 1;
	pChipCap->max_vht_mcs = VHT_MCS_CAP_9;

	pChipCap->TXWISize = 20;
	pChipCap->RXWISize = 28;
#ifdef RTMP_MAC_PCI
	pChipCap->WPDMABurstSIZE = 3;
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_PCI
	if (IS_PCI_INF(pAd)) {
		pChipCap->tx_hw_hdr_len = pChipCap->TXWISize + TSO_SIZE;
		pChipCap->rx_hw_hdr_len = RAL_RXINFO_SIZE + pChipCap->RXWISize;
	}
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		pChipCap->tx_hw_hdr_len = TXINFO_SIZE + pChipCap->TXWISize + TSO_SIZE;
		pChipCap->rx_hw_hdr_len = RXDMA_FIELD_SIZE + RAL_RXINFO_SIZE + pChipCap->RXWISize;
	}
#endif /* RTMP_MAC_USB */

#ifdef RTMP_FLASH_SUPPORT
	pChipCap->EEPROM_DEFAULT_BIN = MT76x0_EeBuffer;
#endif /* RTMP_FLASH_SUPPORT */

	pChipCap->SnrFormula = SNR_FORMULA2;
	pChipCap->FlgIsHwWapiSup = TRUE;
	pChipCap->VcoPeriod = 10;
	pChipCap->FlgIsVcoReCalMode = VCO_CAL_MODE_3;
	pChipCap->FlgIsHwAntennaDiversitySup = FALSE;
#ifdef STREAM_MODE_SUPPORT
	pChipCap->FlgHwStreamMode = FALSE;
#endif /* STREAM_MODE_SUPPORT */
#ifdef TXBF_SUPPORT
	pChipCap->FlgHwTxBfCap = FALSE;
#endif /* TXBF_SUPPORT */
#ifdef FIFO_EXT_SUPPORT
	pChipCap->FlgHwFifoExtCap = TRUE;
#endif /* FIFO_EXT_SUPPORT */

#ifdef CONFIG_CSO_SUPPORT
	pChipCap->asic_caps |= fASIC_CAP_CSO;
#endif /* CONFIG_CSO_SUPPORT */
#ifdef CONFIG_TSO_SUPPORT
	//pChipCap->asic_caps |= fASIC_CAP_TSO;
#endif /* CONFIG_TSO_SUPPORT */

	pChipCap->asic_caps |= (fASIC_CAP_PMF_ENC);

	if (IS_MT76x0U(pAd))
		pChipCap->phy_caps = (fPHY_CAP_24G | fPHY_CAP_5G);
	else
		pChipCap->phy_caps = fPHY_CAP_5G;
	
	pChipCap->phy_caps |= (fPHY_CAP_HT | fPHY_CAP_VHT);

	pChipCap->RfReg17WtMethod = RF_REG_WT_METHOD_STEP_ON;
		
	pChipCap->MaxNumOfRfId = MAX_RF_ID;
	pChipCap->pRFRegTable = NULL;

	pChipCap->MaxNumOfBbpId = 200;	
	pChipCap->pBBPRegTable = NULL;
	pChipCap->bbpRegTbSize = 0;

#ifdef DFS_SUPPORT
	pChipCap->DfsEngineNum = 4;
#endif /* DFS_SUPPORT */

#ifdef NEW_MBSSID_MODE
#ifdef ENHANCE_NEW_MBSSID_MODE
	pChipCap->MBSSIDMode = MBSSID_MODE4;
#else
	pChipCap->MBSSIDMode = MBSSID_MODE1;
#endif /* ENHANCE_NEW_MBSSID_MODE */
#else
	pChipCap->MBSSIDMode = MBSSID_MODE0;
#endif /* NEW_MBSSID_MODE */

#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
	/* Frequence Calibration */
	pChipCap->FreqCalibrationSupport = FALSE;
	pChipCap->FreqCalInitMode = 0;
	pChipCap->FreqCalMode = 0;
	pChipCap->RxWIFrqOffset = 0;
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

#ifdef RTMP_EFUSE_SUPPORT
	pChipCap->EFUSE_USAGE_MAP_START = 0x1e0;
	pChipCap->EFUSE_USAGE_MAP_END = 0x1FC;      
	pChipCap->EFUSE_USAGE_MAP_SIZE = 29;
	pChipCap->EFUSE_RESERVED_SIZE = pChipCap->EFUSE_USAGE_MAP_SIZE - 5;
#endif /* RTMP_EFUSE_SUPPORT */

#ifdef CONFIG_ANDES_SUPPORT
	pChipCap->WlanMemmapOffset = 0x410000;
	pChipCap->InbandPacketMaxLen = 192;
	pChipCap->CmdRspRxRing = RX_RING1;
	if (IS_MT7610(pAd))
		pChipCap->IsComboChip = FALSE;
	else
		pChipCap->IsComboChip = TRUE;

	pChipCap->need_load_fw = TRUE;
	pChipCap->load_iv = TRUE;
	pChipCap->ilm_offset = 0x00000;
	pChipCap->dlm_offset = 0x80000;
#endif /* CONFIG_ANDES_SUPPORT */

#ifdef RTMP_PCI_SUPPORT
	pChipCap->cmd_padding_len = 0;
#endif /* RTMP_PCI_SUPPORT */

#ifdef RTMP_USB_SUPPORT
	pChipCap->cmd_padding_len = 4;
	pChipCap->CommandBulkOutAddr = 0x8;
	pChipCap->WMM0ACBulkOutAddr[0] = 0x4;
	pChipCap->WMM0ACBulkOutAddr[1] = 0x5;
	pChipCap->WMM0ACBulkOutAddr[2] = 0x6;
	pChipCap->WMM0ACBulkOutAddr[3] = 0x7;
	pChipCap->WMM1ACBulkOutAddr	= 0x9;
	pChipCap->DataBulkInAddr = 0x84;
	pChipCap->CommandRspBulkInAddr = 0x85;	 
#endif /* RTMP_USB_SUPPORT */

	pChipCap->AMPDUFactor = 2;

#ifdef MT7650
	if (IS_MT7650(pAd))
		pChipCap->FWImageName = MT7650_FirmwareImage;
#endif /* MT7650 */

#ifdef MT7630
	if (IS_MT7630(pAd))
		pChipCap->FWImageName = MT7650_FirmwareImage;
#endif /* MT7630 */

#ifdef MT7610
	if (IS_MT7610(pAd))
		pChipCap->FWImageName = MT7610_FirmwareImage;
#endif /* MT7610 */

	pChipCap->temp_sensor_enable = TRUE;

	pChipCap->MACRegisterVer = "MT7650_E3_CR_setting_20130508.xlsx";
	pChipCap->BBPRegisterVer = "MT7650E3_BBP_CR_20130222.xls";
	pChipCap->RFRegisterVer = "MT7650E3_WiFi_RF_CR_20130130.xls";

	RTMP_DRS_ALG_INIT(pAd, RATE_ALG_GRP);
		
	/*
		Following function configure beacon related parameters
		in pChipCap
			FlgIsSupSpecBcnBuf / BcnMaxHwNum / 
			WcidHwRsvNum / BcnMaxHwSize / BcnBase[]
	*/
	rlt_bcn_buf_init(pAd);

	/*
		init operator
	*/

	/* BBP adjust */
	pChipOps->ChipBBPAdjust = MT76x0_ChipBBPAdjust;
	
#ifdef CONFIG_STA_SUPPORT
	pChipOps->ChipAGCAdjust = NULL;
#endif /* CONFIG_STA_SUPPORT */

	/* Channel */
	pChipOps->ChipSwitchChannel = MT76x0_ChipSwitchChannel;
	pChipOps->ChipAGCInit = NULL;

	pChipOps->AsicMacInit = mt76x0_nic_init_mac_registers;
	pChipOps->AsicBbpInit = NICInitMT76x0BbpRegisters;
	pChipOps->AsicRfInit = mt76x0_nic_init_rf_registers;
	pChipOps->AsicRfTurnOn = NULL;

	pChipOps->AsicHaltAction = NULL;
	pChipOps->AsicRfTurnOff = NULL;
	pChipOps->AsicReverseRfFromSleepMode = NULL;
	pChipOps->AsicResetBbpAgent = NULL;
	
	/* MAC */

	/* EEPROM */
	pChipOps->NICInitAsicFromEEPROM = MT76x0_NICInitAsicFromEEPROM;
	
	/* Antenna */
	pChipOps->AsicAntennaDefaultReset = MT76x0_AsicAntennaDefaultReset;

	/* TX ALC */
	pChipOps->InitDesiredTSSITable = NULL;
 	pChipOps->ATETssiCalibration = NULL;
	pChipOps->ATETssiCalibrationExtend = NULL;
	pChipOps->AsicTxAlcGetAutoAgcOffset = NULL;
	pChipOps->ATEReadExternalTSSI = NULL;
	pChipOps->TSSIRatio = NULL;
	
	pChipOps->AsicAdjustTxPower = mt76x0_asic_adjust_tx_power;
#ifdef RTMP_TEMPERATURE_COMPENSATION
	pChipOps->TemperCompensation = NULL;
#endif /* RTMP_TEMPERATURE_COMPENSATION */
	pChipOps->show_pwr_info = mt76x0_show_pwr_info;

	/* Others */
#ifdef CONFIG_STA_SUPPORT
	pChipOps->NetDevNickNameInit = MT76x0_NetDevNickNameInit;
#endif /* CONFIG_STA_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT
	pAd->chipCap.carrier_func = TONE_RADAR_V3;
	pChipOps->ToneRadarProgram = ToneRadarProgram_v3;
#endif /* CARRIER_DETECTION_SUPPORT */

	/* Chip tuning */
	pChipOps->RxSensitivityTuning = NULL;
	pChipOps->AsicTxAlcGetAutoAgcOffset = NULL;
	pChipOps->AsicGetTxPowerOffset = mt76x0_asic_update_per_rate_pwr;
	pChipOps->AsicExtraPowerOverMAC = MT76x0_AsicExtraPowerOverMAC;

/* 
	Following callback functions already initiailized in RtmpChipOpsHook() 
	1. Power save related
*/
#ifdef GREENAP_SUPPORT
	pChipOps->EnableAPMIMOPS = NULL;
	pChipOps->DisableAPMIMOPS = NULL;
#endif /* GREENAP_SUPPORT */

	pChipOps->DisableTxRx = MT76x0_DisableTxRx;

#ifdef RTMP_USB_SUPPORT
	pChipOps->AsicRadioOn = RT65xxUsbAsicRadioOn;
	pChipOps->AsicRadioOff = RT65xxUsbAsicRadioOff;
	pChipOps->usb_cfg_read = usb_cfg_read_v2;
	pChipOps->usb_cfg_write = usb_cfg_write_v2;
#endif /* RTMP_USB_SUPPORT */

#ifdef RTMP_PCI_SUPPORT
	pChipOps->AsicRadioOn = RT28xxPciAsicRadioOn;
	pChipOps->AsicRadioOff = RT28xxPciAsicRadioOff;
#endif /* RTMP_PCI_SUPPORT */

#ifdef HDR_TRANS_SUPPORT
	if (1) {
#if 0
		UINT8 cnt = HT_RX_WCID_SIZE/HT_RX_WCID_OFFSET;
		UINT32 RegVal;
#endif
		/* enable TX/RX Header Translation */
		RTMP_IO_WRITE32(pAd, HT_RX_WCID_EN_BASE , 0xFF);	/* all RX WCID enable */

		/* black list - skip EAP-888e/DLS-890d */
		RTMP_IO_WRITE32(pAd, HT_RX_BL_BASE, 0x888e890d);
		//RTMP_IO_WRITE32(pAd, HT_RX_BL_BASE, 0x08000806);

		/* tsc conrotl */
/*
		RTMP_IO_READ32(pAd, 0x250, &RegVal);
		RegVal |= 0x6000;
		RTMP_IO_WRITE32(pAd, 0x250, RegVal);
*/
	}	
#endif /* HDR_TRANS_SUPPORT */
}


void mt76x0_antenna_sel_ctl(RTMP_ADAPTER *ad)
{
	USHORT e2p_val = 0;
	UINT32 WlanFunCtrl = 0, CmbCtrl = 0, CoexCfg0 = 0, CoexCfg3 = 0;

#ifdef RTMP_MAC_PCI
	RTMP_SEM_LOCK(&ad->WlanEnLock);
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(ad)) {
		UINT32 ret;

		RTMP_SEM_EVENT_WAIT(&ad->WlanEnLock, ret);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("WlanEnLock get failed(ret=%d)\n", ret));
			return;
		}
	}
#endif /* RTMP_MAC_USB */

	RTMP_IO_READ32(ad, WLAN_FUN_CTRL, &WlanFunCtrl);
	RTMP_IO_READ32(ad, CMB_CTRL, &CmbCtrl);
	RTMP_IO_READ32(ad, COEXCFG0, &CoexCfg0);
	RTMP_IO_READ32(ad, COEXCFG3, &CoexCfg3);

	CoexCfg0 &= ~BIT2;
	CmbCtrl &= ~(BIT14 | BIT12);
	WlanFunCtrl &= ~(BIT6 | BIT5);
	CoexCfg3 &= ~(BIT5 | BIT4 | BIT3 | BIT2 | BIT1);
	
	/*
		0x23[7]
		0x1: Chip is in dual antenna mode
		0x0: Chip is in single antenna mode
	*/
	RT28xx_EEPROM_READ16(ad, 0x22, e2p_val);
		
	if (e2p_val & 0x8000) {
		if ((ad->NicConfig2.field.AntOpt == 0) && (ad->NicConfig2.field.AntDiversity == 1))
			CmbCtrl |= BIT12; /* 0x20[12]=1 */
		else
			CoexCfg3 |= BIT4; /* 0x4C[4]=1 */
		
		CoexCfg3 |= BIT3; /* 0x4C[3]=1 */
		
		if (WMODE_CAP_2G(ad->CommonCfg.PhyMode))
			WlanFunCtrl |= BIT6; /* 0x80[6]=1 */
		
		DBGPRINT(RT_DEBUG_TRACE, ("%s - Dual antenna mode\n", __FUNCTION__));
	} else {
		if (WMODE_CAP_5G(ad->CommonCfg.PhyMode))
			CoexCfg3 |= (BIT3 | BIT4); /* 0x4C[3]=1, 0x4C[4]=1 */
		else {
			WlanFunCtrl |= BIT6; /* 0x80[6]=1 */
			CoexCfg3 |= BIT1; /* 0x4C[1]=1 */
		}
		
		DBGPRINT(RT_DEBUG_TRACE, ("%s - Single antenna mode\n", __FUNCTION__));
	}

	RTMP_IO_WRITE32(ad, WLAN_FUN_CTRL, WlanFunCtrl);
	RTMP_IO_WRITE32(ad, CMB_CTRL, CmbCtrl);
	RTMP_IO_WRITE32(ad, COEXCFG0, CoexCfg0);
	RTMP_IO_WRITE32(ad, COEXCFG3, CoexCfg3);

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(ad)) {
		RTMP_SEM_EVENT_UP(&ad->WlanEnLock);
	}
#endif /* RTMP_MAC_USB */

#ifdef RTMP_MAC_PCI
	RTMP_SEM_UNLOCK(&ad->WlanEnLock);
#endif /* RTMP_MAC_PCI */
}


void mt76x0_vco_calibration(RTMP_ADAPTER *ad, UINT8 channel)
{
	/*
		VCO_Calibration_MT7650E2.docx:
		2.	Calibration Procedure:
			i.	Set the configuration (examples in section 12)
			ii.	Set B0.R04.[7] vcocal_en to "high" (1.2V).  
				After completing the calibration procedure, it would return to "low" automatically.
	*/

	UCHAR rf_val = 0, vco_mode = 0;

	rlt_rf_read(ad, RF_BANK0, RF_R04, &rf_val);
	vco_mode = (rf_val & 0x70);	
	
	if (vco_mode == 0x30)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s - Calibration Mode: Open loop, closed loop, and amplitude @ ch%d\n", 
						__FUNCTION__, channel));
		/*
			Calibration Mode - Open loop, closed loop, and amplitude:
			B0.R06.[0]: 1
			B0.R06.[3:1] bp_close_code: 100
			B0.R05.[7:0] bp_open_code: 0x0
			B0.R04.[2:0] cal_bits: 000
			B0.R03.[2:0] startup_time: 011
			B0.R03.[6:4] settle_time: 
						80MHz channel: 110
						40MHz channel: 101
						20MHz channel: 100
		*/
		rlt_rf_read(ad, RF_BANK0, RF_R06, &rf_val);
		rf_val &= ~(0x0F);
		rf_val |= 0x09;
		rlt_rf_write(ad, RF_BANK0, RF_R06, rf_val);

		rlt_rf_read(ad, RF_BANK0, RF_R05, &rf_val);
		if (rf_val != 0)
			rlt_rf_write(ad, RF_BANK0, RF_R05, 0x0);

		rlt_rf_read(ad, RF_BANK0, RF_R04, &rf_val);
		rf_val &= ~(0x07);
		rlt_rf_write(ad, RF_BANK0, RF_R04, rf_val);

		rlt_rf_read(ad, RF_BANK0, RF_R03, &rf_val);
		rf_val &= ~(0x77);
		if ((channel == 1) || (channel == 7) || (channel == 9) || (channel >= 13))
			rf_val |= 0x63;
		else if ((channel == 3) || (channel == 4) || (channel == 10))
			rf_val |= 0x53;
		else if ((channel == 2) || (channel == 5) || (channel == 6) || 
				(channel == 8) || (channel == 11) || (channel == 12))
			rf_val |= 0x43;
		else {
			DBGPRINT(RT_DEBUG_OFF, ("%s - wrong input channel\n", __FUNCTION__));
			return;
		}
		rlt_rf_write(ad, RF_BANK0, RF_R03, rf_val);

		rlt_rf_read(ad, RF_BANK0, RF_R04, &rf_val);
		rf_val = ((rf_val & ~(0x80)) | 0x80); 
		rlt_rf_write(ad, RF_BANK0, RF_R04, rf_val);

		RtmpusecDelay(2200); 
	}
}


static void do_full_calibration(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR channel,
	IN BOOLEAN bSave)
{
	UINT32 cal_mode, param, _byte0 = 0, _byte1= 0, _byte2 = 0, _byte3 = 0;
	/*
		Do calibration.
		The calibration sequence is very important, please do NOT change it.
		4  RX DCOC calibration
		5  LC tank calibration
		6  TX Filter BW --> not ready yet @20121003
		7  RX Filter BW --> not ready yet @20121003
		8  TX RF LOFT 
		9  TX I/Q
		10 TX Group Delay		
		11 RX I/Q
		12 RX Group Delay
		13 TSSI Zero Reference --> not ready yet @20121016
		14 TX 2G DPD
		15 TX 2G IM3 --> not ready yet @20121016 
		16 On-chip temp sensor reading --> not ready yet @20130129
		17 RX DCOC calibration
	*/

	if (channel > 14) {
		_byte0 = (1 /*External PA*/) ? 1 : 2;
		
		if (channel < 100)
			_byte1 = (bSave == TRUE) ? 0x3 : 0x7;
		else if (channel < 140)
			_byte1 = (bSave == TRUE) ? 0x4 : 0x8;
	else
			_byte1 = (bSave == TRUE) ? 0x5 : 0x9;
	} else {
		_byte0 = 0;
		_byte1 = (bSave == TRUE) ? 0x2 : 0x6;
		_byte2 = channel;
		_byte3 = pAd->CommonCfg.BBPCurrentBW;
	}

	param = _byte0 | (_byte1 << 8) | (_byte2 << 16) | (_byte3 << 24);		

	if (IS_DOT11_H_RADAR_STATE(pAd, RD_SILENCE_MODE))
		cal_mode = PARTIAL_CALIBRATION;
	else
		cal_mode = FULL_CALIBRATION;

	DBGPRINT(RT_DEBUG_OFF, ("%s - Channel = %u, param = 0x%x, bSave = %d, cal_mode = %u\n",
			__FUNCTION__, channel, param, bSave, cal_mode));
	
	if (cal_mode == FULL_CALIBRATION) {
		/* avoid race condition with temperature/TSSI reading */
		pAd->CommonCfg.bStopReadTemperature = TRUE;

		CHIP_CALIBRATION(pAd, FULL_CALIBRATION_ID, param);
#ifdef RTMP_PCI_SUPPORT
 		if (bSave)
 		{
 			RtmpOsMsDelay(350);
 		}
#endif /* RTMP_PCI_SUPPORT */
		CHIP_CALIBRATION(pAd, LC_CALIBRATION, 1);
#ifdef RTMP_PCI_SUPPORT
		RtmpOsMsDelay(30);
#endif /* RTMP_PCI_SUPPORT */

		pAd->CommonCfg.bStopReadTemperature = FALSE;
	} else {		
		/*
			4. RXDC Calibration parameter
				0:Back Ground Disable
		*/
		CHIP_CALIBRATION(pAd, RXDCOC_CALIBRATION, 0);
	
		/*
			5. LC-Calibration parameter
				Bit[0:7]
					0: 2G
					1: 5G + External PA
					2: 5G + Internal PA
				Bit[8:15]
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/
		if (cal_mode == FULL_CALIBRATION) {
			CHIP_CALIBRATION(pAd, LC_CALIBRATION, param);
		}
		
		/*
			6,7. BW-Calibration
				Bit[0:7] (0:RX, 1:TX)
				Bit[8:15] (0:BW20, 1:BW40, 2:BW80)
				Bit[16:23]
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/
	
		/*			
			8. RF LOFT-Calibration parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15] 
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/
		if (cal_mode == FULL_CALIBRATION) {
			CHIP_CALIBRATION(pAd, LOFT_CALIBRATION, param);
		}
		
		/*
			9. TXIQ-Calibration parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15] 
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/
		if (cal_mode == FULL_CALIBRATION) {
			CHIP_CALIBRATION(pAd, TXIQ_CALIBRATION, param);
		}
		
		/*
			10. TX Group-Delay Calibation parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15] 
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/
		if (cal_mode == FULL_CALIBRATION) {	
			CHIP_CALIBRATION(pAd, TX_GROUP_DELAY_CALIBRATION, param);
		}
		
		/*
			11. RXIQ-Calibration parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15] 
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
						
			12. RX Group-Delay Calibation parameter
				Bit[0:7] (0:G-Band, 1: A-Band)
				Bit[8:15] 
					0: Full Calibration
					1: Partial Calibration
					2: G-Band Full Calibration + Save
					3: A-Band (Low) Full Calibration + Save
					4: A-Band (Mid) Full Calibration + Save
					5: A-Band (High) Full Calibration + Save
					6: G-Band Restore Calibration
					7: A-Band (Low) Restore Calibration
					8: A-Band (Mid) Restore Calibration
					9: A-Band (High) Restore Calibration
		*/
		if (cal_mode == FULL_CALIBRATION) {
			CHIP_CALIBRATION(pAd, RXIQ_CALIBRATION, param);
			CHIP_CALIBRATION(pAd, RX_GROUP_DELAY_CALIBRATION, param);
		}
		
		/* 
			14. TX 2G DPD - Only 2.4G needs to do DPD Calibration. 
				Bit[0:7] (1~14 Channel)
				Bit[8:15] (0:BW20, 1:BW40)
				NOTE: disable DPD calibration for USB products
		*/			
		if (IS_MT76x0E(pAd)) {
			UINT32 dpd_val = 0;

			dpd_val = (pAd->CommonCfg.BBPCurrentBW << 8) | channel;
			CHIP_CALIBRATION(pAd, DPD_CALIBRATION, dpd_val);
			DBGPRINT(RT_DEBUG_OFF, ("%s - DPD_CALIBRATION = 0x%x\n", __FUNCTION__, dpd_val));
		}
	}
}


void mt76x0_calibration(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR channel,
	IN BOOLEAN bPowerOn,
	IN BOOLEAN bFullCal,
	IN BOOLEAN bSaveCal)
{
	UINT32 reg_val = 0, reg_tx_alc = 0;
	
	DBGPRINT(RT_DEBUG_OFF, ("%s - Channel = %d, bPowerOn = %d, bFullCal = %d, bSaveCal = %d\n",
				__FUNCTION__, channel, bPowerOn, bFullCal, bSaveCal));

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		UINT32 ret;
		
		RTMP_SEM_EVENT_WAIT(&pAd->cal_atomic, ret);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("cal_atomic get failed(ret=%d)\n", ret));
			return;
		}
	}
#endif /* RTMP_MAC_USB */

	if (!(bPowerOn || bFullCal || bSaveCal))
		goto RXDC_Calibration;
	
	if (bPowerOn) {
		/*
			Do Power on calibration.
			The calibration sequence is very important, please do NOT change it.
			1 XTAL Setup (already done in AsicRfInit)
			2 R-calibration
			3 VCO calibration
		*/

		/*
			2 R-calibration 
		*/
		CHIP_CALIBRATION(pAd, R_CALIBRATION, 0x0);

		/*
			3 VCO calibration (mode 3) 
		*/
		mt76x0_vco_calibration(pAd, channel);

#ifdef MT76x0_TSSI_CAL_COMPENSATION
		/* TSSI Calibration */
		if (bPowerOn && pAd->chipCap.bInternalTxALC) {
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x8);
			MT76x0_TSSI_DC_Calibration(pAd);
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0xC);
		}
#endif /* MT76x0_TSSI_CAL_COMPENSATION */
	}

	RTMP_IO_READ32(pAd, TX_ALC_CFG_0, &reg_tx_alc); /* We need to restore 0x13b0 after calibration. */
	RTMP_IO_WRITE32(pAd, TX_ALC_CFG_0, 0x0);
	RtmpusecDelay(500);
	
	RTMP_IO_READ32(pAd, 0x2124, &reg_val); /* We need to restore 0x2124 after calibration. */
	RTMP_IO_WRITE32(pAd, 0x2124, 0xFFFFFF7E); /* Disable 0x2704, 0x2708 controlled by MAC. */

	if (bSaveCal) {
		if (WMODE_CAP_2G(pAd->CommonCfg.PhyMode)) {
			AsicSwitchChannel(pAd, G_BAND_CH, FALSE);
			do_full_calibration(pAd, G_BAND_CH, TRUE);
		} else if (WMODE_CAP_5G(pAd->CommonCfg.PhyMode)) {
			UINT idx;
			UCHAR cal_ch[] = {A_BAND_LOW_CH, A_BAND_MID_CH, A_BAND_HIGH_CH, 0};
			for (idx = 0; cal_ch[idx] != 0; idx++)
			{
				AsicSwitchChannel(pAd, cal_ch[idx], FALSE);
				do_full_calibration(pAd, cal_ch[idx], TRUE);			
			}
		}

		/* Back to original channel */
		AsicSwitchChannel(pAd, channel, FALSE);
	}

	if (bFullCal) {
		do_full_calibration(pAd, channel, FALSE);
	}

	/* Restore 0x2124 & TX_ALC_CFG_0 after calibration completed */
	RTMP_IO_WRITE32(pAd, 0x2124, reg_val);
	RTMP_IO_WRITE32(pAd, TX_ALC_CFG_0, reg_tx_alc);
	RtmpusecDelay(100000); // TODO: check response packet from FW

RXDC_Calibration:
	/*
		17. RXDC Calibration parameter
			1:Back Ground Enable
	*/
	CHIP_CALIBRATION(pAd, RXDCOC_CALIBRATION, 1);
	
#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		RTMP_SEM_EVENT_UP(&pAd->cal_atomic);
	}
#endif /* RTMP_MAC_USB */
}


BOOLEAN mt76x0_get_tssi_report(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN bResetTssiInfo,
	OUT PCHAR pTssiReport)
{
	UINT32 wait = 0, reg_val = 0;
	UCHAR rf_b7_73 = 0, rf_b0_66 = 0, rf_b0_67 = 0;
	BOOLEAN status;
	
	/* avoid race condition between FW/driver */
	if (pAd->CommonCfg.bStopReadTemperature == TRUE)
	{
    	DBGPRINT(RT_DEBUG_OFF, ("\x1b[31m%s: FW is accessing RF Regs...\x1b[m\n", __FUNCTION__));
		return FALSE;
	}
	
	rlt_rf_read(pAd, RF_BANK7, RF_R73, &rf_b7_73);
	rlt_rf_read(pAd, RF_BANK0, RF_R66, &rf_b0_66);
	rlt_rf_read(pAd, RF_BANK0, RF_R67, &rf_b0_67);
	
	/*
		1. Set 0dB Gain:
			WIFI_RF_CR_WRITE(7,73,0x02) 
	*/
	rlt_rf_write(pAd, RF_BANK7, RF_R73, 0x02);

	/*
		2. Calibration Switches:
			WIFI_RF_CR_WRITE(0,66,0x23)
	*/
	rlt_rf_write(pAd, RF_BANK0, RF_R66, 0x23);

	/*
		3. Offset-measurement configuration:
			WIFI_RF_CR_WRITE(0,67,0x01)
	*/
	rlt_rf_write(pAd, RF_BANK0, RF_R67, 0x01);

	/*
		4. Select Level meter from ADC.q:
			WIFI_BBP_CR_WRITE(0x2088,0x00080055)
	*/
	RTMP_BBP_IO_WRITE32(pAd, CORE_R34, 0x00080055);

	/*
		5. Wait until it's done:
			wait until 0x2088[4] = 0
	*/
	for (wait = 0; wait < 2000; wait++) 
	{
		RTMP_BBP_IO_READ32(pAd, CORE_R34, &reg_val);
		if ((reg_val & 0x10) == 0)
			break;
		RtmpusecDelay(3);
	}

	if (wait >= 2000) {
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_TRACE,("Wait 0x2088[4]=0 but time's up, give up this time!\n"));
#endif /* RELEASE_EXCLUDE */
		reg_val &= ~(0x10);
		RTMP_BBP_IO_WRITE32(pAd, CORE_R34, reg_val);
		status = FALSE;
		goto done;
	}

	/*
		6. Read Dout (0x0041208c<7:0>=adc_out<8:1>):
			WIFI_BBP_CR_READ(0x208c) //$Dout

	*/
	RTMP_BBP_IO_READ32(pAd, CORE_R35, &reg_val);
	reg_val &= 0xFF;
	if ((reg_val & 0x80) == 0x80) 
		reg_val |= 0xFFFFFF00; /* Negative number */
	*pTssiReport = reg_val;
	
	status = TRUE;

done:	
	/*
		Restore RF CR
			B7. R73, B0.R66, B0.R67
	*/
	rlt_rf_write(pAd, RF_BANK7, RF_R73, rf_b7_73);
	rlt_rf_write(pAd, RF_BANK0, RF_R66, rf_b0_66);
	rlt_rf_write(pAd, RF_BANK0, RF_R67, rf_b0_67);

	return status;
}


void mt76x0_temp_sensor(RTMP_ADAPTER *ad)
{
	SHORT temperature = 0;
	CHAR Dout = 0;

	if (mt76x0_get_tssi_report(ad, TRUE, &Dout) == FALSE)
		return;

	/*
		Read D25 from EEPROM:
			Read EEPROM 0xD1 // $Offset (signed integer)
			
		Calculate temperature:
			T = 3.5 * (Dout - D25) + 25
	*/
	temperature = (35*(Dout-ad->chipCap.temp_offset))/10 + 25;
	DBGPRINT(RT_DEBUG_ERROR, ("%s - Dout=%d (0x%x), temp_offset=%d (0x%x), current_temp=%d (0x%x)\n",
			__FUNCTION__, Dout, Dout, ad->chipCap.temp_offset, 
			ad->chipCap.temp_offset, temperature, temperature));
	
	if (ad->chipCap.last_vco_temp == 0x7FFF)
		ad->chipCap.last_vco_temp = temperature;
	if (ad->chipCap.last_full_cal_temp == 0x7FFF)
		ad->chipCap.last_full_cal_temp = temperature;
	ad->chipCap.current_temp = temperature;
}


void mt76x0_temp_trigger_cal(RTMP_ADAPTER *ad)
{
	SHORT temp_diff = 0;
	
	mt76x0_temp_sensor(ad);

	temp_diff = ad->chipCap.current_temp - ad->chipCap.last_vco_temp;
	if ((temp_diff > 20) || (temp_diff < -20)) {						
		DBGPRINT(RT_DEBUG_OFF, ("%s - do VCO re-calibration (last_vco_temp=%d, current_temp=%d)\n", 
			__FUNCTION__, ad->chipCap.last_vco_temp, ad->chipCap.current_temp));
		
		ad->chipCap.last_vco_temp = ad->chipCap.current_temp;
		mt76x0_vco_calibration(ad, ad->hw_cfg.cent_ch);
	}

	temp_diff = ad->chipCap.current_temp - ad->chipCap.last_full_cal_temp;
	if ((temp_diff > 30) || (temp_diff < -30)) {						
		DBGPRINT(RT_DEBUG_OFF, ("%s - do full calibration again (last_full_cal_temp=%d, current_temp=%d)\n", 
			__FUNCTION__, ad->chipCap.last_full_cal_temp, ad->chipCap.current_temp));
		
		ad->chipCap.last_full_cal_temp = ad->chipCap.current_temp;
		mt76x0_calibration(ad, ad->hw_cfg.cent_ch, FALSE, TRUE, TRUE);
	}
}


#ifdef RTMP_FLASH_SUPPORT
VOID MT76x0_ReadFlashAndInitAsic(
	IN RTMP_ADAPTER *pAd)
{
	USHORT ee_val = 0;
	UINT32 reg_val = 0;

#if 0
	/*
		If EEPROM value are stored in falsh not in eFuse/EEPROM, 
		ASIC will read in-correct value from eFuse/EEPROM and write to register 0x20 & 0x104 when power on.
		Driver needs to read correct value from flash and update value to register 0x20 & 0x104.
	*/
#endif

	pAd->chipCap.EEPROM_DEFAULT_BIN = MT76x0_EeBuffer;
	rtmp_nv_init(pAd);

	rtmp_ee_flash_read(pAd, 0x22, &ee_val);
	DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x22 = 0x%x\n", __FUNCTION__, ee_val));
	RTMP_IO_READ32(pAd, CMB_CTRL, &reg_val);
	reg_val &= 0xFFFF0000;
	reg_val |= ee_val;
	RTMP_IO_WRITE32(pAd, CMB_CTRL, reg_val);

	rtmp_ee_flash_read(pAd, 0x24, &ee_val);
	DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x24 = 0x%x\n", __FUNCTION__, ee_val));
	RTMP_IO_READ32(pAd, 0x104, &reg_val);
	reg_val &= 0xFFFF0000;
	reg_val |= ee_val;
	RTMP_IO_WRITE32(pAd, 0x104, reg_val);
	return;
}
#endif /* RTMP_FLASH_SUPPORT */


#ifdef RTMP_MAC_PCI
#if 0
/*
	MT7650E2(3)_PCIe_L1_programming_sequence.xlsx
*/
#endif
VOID MT76x0_InitPCIeLinkCtrlValue(
	IN RTMP_ADAPTER *pAd)
{
	USHORT ee_val = 0;
	POS_COOKIE pObj;
	INT pos = 0;
	USHORT Configuration = 0;
	USHORT vendor_id = 0, device_id = 0;
	UINT32 reg_val = 0;

	if (IS_MT76x0(pAd) == FALSE)
		return;

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	pci_read_config_word(pObj->pci_dev, RTMP_OS_PCI_VENDOR_ID, &vendor_id);
	pci_read_config_word(pObj->pci_dev, RTMP_OS_PCI_DEVICE_ID, &device_id);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: vendor_id = 0x%04x, device_id = 0x%04x\n", __FUNCTION__, vendor_id, device_id));
	
	RT28xx_EEPROM_READ16(pAd, 0x26, ee_val);
	DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x26 = 0x%x\n", __FUNCTION__, ee_val));
	RT28xx_EEPROM_READ16(pAd, 0x28, ee_val);
	DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x28 = 0x%x\n", __FUNCTION__, ee_val));

	pos = pci_find_capability(pObj->pci_dev, PCI_CAP_ID_EXP);

	if (pos != 0)
	{
		/* Ralink PCIe Device's Link Control Register Offset */
		pAd->RLnkCtrlOffset = pos + PCI_EXP_LNKCTL;
		pci_read_config_word(pObj->pci_dev, pAd->RLnkCtrlOffset, &Configuration);
		pAd->RLnkCtrlConfiguration = Configuration & 0x3;
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Read 1 (Ralink PCIe Link Control Register) offset 0x%x = 0x%x\n", 
				__FUNCTION__, pAd->RLnkCtrlOffset, Configuration));

		RTMP_IO_READ32(pAd, 0x64, &reg_val);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Read 0x64 = 0x%x\n", __FUNCTION__, reg_val));
		reg_val |= 0x00000020;
		RTMP_IO_WRITE32(pAd, 0x64, reg_val);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: write 0x64 = 0x%x\n", __FUNCTION__, reg_val));

		RTMP_IO_WRITE32(pAd, 0x1F080, pAd->RLnkCtrlConfiguration); /* Please DON'T read it. */
		DBGPRINT(RT_DEBUG_TRACE, ("%s: write 0x1F080 = 0x%x\n", __FUNCTION__, pAd->RLnkCtrlConfiguration));
		
		Configuration &= 0x103;  /* 0x1: L0s, 0x2: L1, 0x3: L0s+L1 */
		pci_write_config_word(pObj->pci_dev, pAd->RLnkCtrlOffset, Configuration);
		pci_read_config_word(pObj->pci_dev, pAd->RLnkCtrlOffset, &Configuration);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Read 2 (Ralink PCIe Link Control Register) offset 0x%x = 0x%x\n", 
				__FUNCTION__, pAd->RLnkCtrlOffset, Configuration));
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Cannot locate PCIe Device's Link Control Register Offset!\n", __FUNCTION__));
	return;
}


VOID MT76x0_PciMlmeRadioOn(
	IN  RTMP_ADAPTER *pAd)
{
#ifdef CONFIG_AP_SUPPORT
	INT32 IdBss, MaxNumBss = pAd->ApCfg.BssidNum;
#endif /* CONFIG_AP_SUPPORT */

#if 0 /* System is not stable if driver would do PM-L1 */
	POS_COOKIE 	pObj;
	USHORT	reg16 = 0, offset = 0;

	pObj = (POS_COOKIE) pAd->OS_Cookie;

#if 0 
	/*
		IMPORTANT @20121018: This patch is for E2 workaround, E3 may not need this workaround.
		PM-L1 exit
			Write configuration space "0x44" to "0x8", it will set d-state to D0.
			Write "0x0001_F044" to "0x0000_0000"  (Please DON'T read it!)
			PCIe link will stay at L0 state.

	*/
#endif
	offset = 0x44;
	reg16 = 0x8;
	PCI_REG_WIRTE_WORD(pObj->pci_dev, offset, reg16);
	RTMP_IO_WRITE32(pAd, 0x1F044, 0x0);
#endif /* if 0 */

#if 0
	if (pObj->parent_pci_dev && (pAd->HostLnkCtrlOffset != 0))
	{
		PCI_REG_WIRTE_WORD(pObj->parent_pci_dev, pAd->HostLnkCtrlOffset, pAd->HostLnkCtrlConfiguration);
		DBGPRINT(RT_DEBUG_TRACE, ("Restore PCI host : offset 0x%x = 0x%x\n", pAd->HostLnkCtrlOffset, pAd->HostLnkCtrlConfiguration));

		PCI_REG_WIRTE_WORD(pObj->pci_dev, pAd->RLnkCtrlOffset, pAd->RLnkCtrlConfiguration);
		DBGPRINT(RT_DEBUG_TRACE, ("Restore Ralink : offset 0x%x = 0x%x\n", pAd->RLnkCtrlOffset, pAd->RLnkCtrlConfiguration));
	}
#endif

	/* Clear Radio off flag*/
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);

	RTMP_ASIC_INTERRUPT_ENABLE(pAd);
	andes_pwr_saving(pAd, RADIO_ON, 0, 0, 0, 0, 0);
	RtmpusecDelay(50);
	
	RTMPRingCleanUp(pAd, QID_AC_BK);
   	RTMPRingCleanUp(pAd, QID_AC_BE);
   	RTMPRingCleanUp(pAd, QID_AC_VI);
   	RTMPRingCleanUp(pAd, QID_AC_VO);
   	RTMPRingCleanUp(pAd, QID_HCCA);
   	RTMPRingCleanUp(pAd, QID_MGMT);
   	RTMPRingCleanUp(pAd, QID_RX);

#ifdef DOT11_VHT_AC
	if (pAd->CommonCfg.BBPCurrentBW == BW_80)
		pAd->hw_cfg.cent_ch = pAd->CommonCfg.vht_cent_ch;
	else
#endif /* DOT11_VHT_AC */
		pAd->hw_cfg.cent_ch = pAd->CommonCfg.CentralChannel;

	AsicSwitchChannel(pAd, pAd->hw_cfg.cent_ch, FALSE);
	AsicLockChannel(pAd, pAd->hw_cfg.cent_ch);

	mt76x0_calibration(pAd, pAd->hw_cfg.cent_ch, FALSE, TRUE, FALSE);

	/* Enable Tx/Rx*/
	RTMPEnableRxTx(pAd);
	
	/* Clear Radio off flag*/
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF);

#ifdef LED_CONTROL_SUPPORT
#ifdef CONFIG_AP_SUPPORT
	/* The LEN_RADIO_ON indicates "Radio on but link down", 
	   so AP shall set LED LINK_UP status */
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
    		RTMPSetLED(pAd, LED_LINK_UP);
	}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
	    /* Set LED*/
	    RTMPSetLED(pAd, LED_RADIO_ON);
	}
#endif /* CONFIG_STA_SUPPORT */
#endif /* LED_CONTROL_SUPPORT */

	RTMP_OS_NETDEV_START_QUEUE(pAd->net_dev);
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (MaxNumBss > MAX_MBSSID_NUM(pAd))
			MaxNumBss = MAX_MBSSID_NUM(pAd);
		
		/* first IdBss must not be 0 (BSS0), must be 1 (BSS1) */
		for (IdBss = FIRST_MBSSID; IdBss < MAX_MBSSID_NUM(pAd); IdBss++)
		{
			if (pAd->ApCfg.MBSSID[IdBss].wdev.if_dev)
				RTMP_OS_NETDEV_START_QUEUE(pAd->ApCfg.MBSSID[IdBss].wdev.if_dev);
		}
	}
#endif /* CONFIG_AP_SUPPORT */

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
}


VOID MT76x0_PciMlmeRadioOFF(
	IN  RTMP_ADAPTER *pAd)
{
	
	UINT32 pwr_level = 5;
	POS_COOKIE 	pObj;
#ifdef CONFIG_AP_SUPPORT
	INT32 IdBss, MaxNumBss = pAd->ApCfg.BssidNum;
#endif /* CONFIG_AP_SUPPORT */

	pObj = (POS_COOKIE) pAd->OS_Cookie;

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
	
	DBGPRINT(RT_DEBUG_TRACE, ("\npwr_level = %d\n", pwr_level));

#ifdef AP_SCAN_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		BOOLEAN		Cancelled;

		RTMPCancelTimer(&pAd->ScanCtrl.APScanTimer, &Cancelled);
	}
#endif /* AP_SCAN_SUPPORT */

#ifdef LED_CONTROL_SUPPORT
	RTMPSetLED(pAd, LED_RADIO_OFF);
#endif /* LED_CONTROL_SUPPORT */
	
	MT76x0_DisableTxRx(pAd, GUIRADIO_OFF);
	
	andes_pwr_saving(pAd, RADIO_OFF, pwr_level, 0, 0, 0, 0);	

	/*
		Wait for Andes firmware receiving this in-band command packet
	*/			
	RtmpusecDelay(50);
	
	/* Set Radio off flag*/
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE))
	{
		RTMP_ASIC_INTERRUPT_DISABLE(pAd);
	}
}
#endif /* RTMP_MAC_PCI */


VOID mt76x0_asic_adjust_tx_power(
	IN PRTMP_ADAPTER pAd)
{
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO,("-->%s\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */

#ifdef CONFIG_STA_SUPPORT
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
		return;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) || 
#ifdef RTMP_MAC_PCI
		(pAd->bPCIclkOff == TRUE) || RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF) ||
#endif /* RTMP_MAC_PCI */
		RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
		return;
#endif /* CONFIG_STA_SUPPORT */

#ifndef SINGLE_SKU_V2
	mt76x0_adjust_per_rate_pwr(pAd);
#endif /* !SINGLE_SKU_V2 */

#ifdef RTMP_TEMPERATURE_COMPENSATION
	mt76x0_temp_tx_alc(pAd);
#endif /* RTMP_TEMPERATURE_COMPENSATION */

#ifdef SINGLE_SKU_V2		
#ifdef MT76x0_TSSI_CAL_COMPENSATION
	if (pAd->chipCap.bInternalTxALC == FALSE)
#endif /* MT76x0_TSSI_CAL_COMPENSATION */
	{
		UINT32 mac_val;
		UCHAR delta_power = 0;	
		USHORT ee_val = 0;
		CHAR sku_base_pwr, ch_pwr_adj;
		UCHAR channel = pAd->hw_cfg.cent_ch; 
		
		if (channel > 14) {			
			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_TARGET_POWER, ee_val);
			pAd->DefaultTargetPwr = ee_val & 0x00ff;
#ifdef DOT11_VHT_AC
			if (pAd->CommonCfg.BBPCurrentBW == BW_80)
				delta_power = pAd->chipCap.delta_tw_pwr_bw80;
			else
#endif /* DOT11_VHT_AC */
				delta_power = pAd->chipCap.delta_tw_pwr_bw40_5G;
		} else {
			RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_2G_TARGET_POWER, ee_val);
			pAd->DefaultTargetPwr = ee_val & 0x00ff;
			delta_power = pAd->chipCap.delta_tw_pwr_bw40_2G;
		}
		
		if ((pAd->DefaultTargetPwr == 0x00) || (pAd->DefaultTargetPwr == 0xFF)) {
			pAd->DefaultTargetPwr = 0x20;
			DBGPRINT(RT_DEBUG_ERROR, ("%s: EEPROM target power Error! Use Default Target Power = 0x%x\n", 
				__FUNCTION__, pAd->DefaultTargetPwr));
		} else {
			DBGPRINT(RT_DEBUG_INFO, ("%s: DefaultTargetPwr = %d\n", 
				__FUNCTION__, pAd->DefaultTargetPwr));
		}

		/*
			EEPROM 0x50 - Power delta for 2.4G HT40
			EEPROM 0x51 - Power delta for 5G HT40
			EEPROM 0xD3 - Power delta for VHT80
			Bit<7>: Enable/disable power delta of this BW
			Bit<6>: 0: decrease power, 1: increase power
			Bit<5:0>: Each step represents 0.5dB, range from 0 to 4

			Increase or decrease 0x13b0<5:0> when bandwidth is changed
		*/
		if ((pAd->CommonCfg.BBPCurrentBW != BW_20) && (delta_power & 0x80)) {
			if (delta_power & 0x40)
				pAd->DefaultTargetPwr += (delta_power & 0x3F);
			else
				pAd->DefaultTargetPwr -= (delta_power & 0x3F);
		}

		sku_base_pwr = MT76x0_GetSkuChannelBasePwr(pAd, channel);
		
		if (pAd->DefaultTargetPwr > sku_base_pwr)
			ch_pwr_adj = sku_base_pwr - pAd->DefaultTargetPwr;
		else
			ch_pwr_adj = 0;

		if (ch_pwr_adj > 31)
			ch_pwr_adj = 31;
		
		if (ch_pwr_adj < -32)
			ch_pwr_adj = -32;

		RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &mac_val);
		mac_val = (mac_val & ~0x3F) | (ch_pwr_adj & 0x3F);
		RTMP_IO_WRITE32(pAd, TX_ALC_CFG_1, mac_val);
		DBGPRINT(RT_DEBUG_INFO, ("SkuBasePwr = 0x%x,  DefaultTargetPwr = 0x%x, ChannelPwrAdj 0x13b4: 0x%x\n", 
			sku_base_pwr, pAd->DefaultTargetPwr, mac_val));

		MT76x0_UpdateSkuPwr(pAd, channel);
	}
#endif /* SINGLE_SKU_V2 */
}


VOID MT76x0_MakeUpRatePwrTable(
	IN  RTMP_ADAPTER *pAd)
{
	UINT32 reg_val;

	// MCS POWER
	RTMP_IO_READ32(pAd, TX_PWR_CFG_0, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", TX_PWR_CFG_0, reg_val));
	pAd->chipCap.rate_pwr_table.CCK[0].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.CCK[0].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.CCK[0].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.CCK[1].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.CCK[1].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.CCK[1].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.CCK[2].MCS_Power = (CHAR)((reg_val&0x3F00)>>8);
	if ( pAd->chipCap.rate_pwr_table.CCK[2].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.CCK[2].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.CCK[3].MCS_Power = (CHAR)((reg_val&0x3F00)>>8);
	if ( pAd->chipCap.rate_pwr_table.CCK[3].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.CCK[3].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.OFDM[0].MCS_Power = (CHAR)((reg_val&0x3F0000)>>16);
	if ( pAd->chipCap.rate_pwr_table.OFDM[0].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.OFDM[0].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.OFDM[1].MCS_Power = (CHAR)((reg_val&0x3F0000)>>16);
	if ( pAd->chipCap.rate_pwr_table.OFDM[1].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.OFDM[1].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.OFDM[2].MCS_Power = (CHAR)((reg_val&0x3F000000)>>24);
	if ( pAd->chipCap.rate_pwr_table.OFDM[2].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.OFDM[2].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.OFDM[3].MCS_Power = (CHAR)((reg_val&0x3F000000)>>24);
	if ( pAd->chipCap.rate_pwr_table.OFDM[3].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.OFDM[3].MCS_Power -= 64;

	RTMP_IO_READ32(pAd, TX_PWR_CFG_1, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", TX_PWR_CFG_1, reg_val));
	pAd->chipCap.rate_pwr_table.OFDM[4].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.OFDM[4].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.OFDM[4].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.OFDM[5].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.OFDM[5].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.OFDM[5].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.OFDM[6].MCS_Power = (CHAR)((reg_val&0x3F00)>>8);
	if ( pAd->chipCap.rate_pwr_table.OFDM[6].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.OFDM[6].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.HT[0].MCS_Power = (CHAR)((reg_val&0x3F0000)>>16);
	if ( pAd->chipCap.rate_pwr_table.HT[0].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.HT[0].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.VHT[0].MCS_Power = pAd->chipCap.rate_pwr_table.HT[0].MCS_Power;
	
	pAd->chipCap.rate_pwr_table.MCS32.MCS_Power = pAd->chipCap.rate_pwr_table.HT[0].MCS_Power;
	if ( pAd->chipCap.rate_pwr_table.MCS32.MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.MCS32.MCS_Power -= 64;
	
	pAd->chipCap.rate_pwr_table.HT[1].MCS_Power = (CHAR)((reg_val&0x3F0000)>>16);
	if ( pAd->chipCap.rate_pwr_table.HT[1].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.HT[1].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.VHT[1].MCS_Power = pAd->chipCap.rate_pwr_table.HT[1].MCS_Power;
	
	pAd->chipCap.rate_pwr_table.HT[2].MCS_Power = (CHAR)((reg_val&0x3F000000)>>24);
	if ( pAd->chipCap.rate_pwr_table.HT[2].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.HT[2].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.VHT[2].MCS_Power = pAd->chipCap.rate_pwr_table.HT[2].MCS_Power;
	
	pAd->chipCap.rate_pwr_table.HT[3].MCS_Power = (CHAR)((reg_val&0x3F000000)>>24);
	if ( pAd->chipCap.rate_pwr_table.HT[3].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.HT[3].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.VHT[3].MCS_Power = pAd->chipCap.rate_pwr_table.HT[3].MCS_Power;

	RTMP_IO_READ32(pAd, TX_PWR_CFG_2, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", TX_PWR_CFG_2, reg_val));
	pAd->chipCap.rate_pwr_table.HT[4].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.HT[4].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.HT[4].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.VHT[4].MCS_Power = pAd->chipCap.rate_pwr_table.HT[4].MCS_Power;
	
	pAd->chipCap.rate_pwr_table.HT[5].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.HT[5].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.HT[5].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.VHT[5].MCS_Power = pAd->chipCap.rate_pwr_table.HT[5].MCS_Power;
	
	pAd->chipCap.rate_pwr_table.HT[6].MCS_Power = (CHAR)((reg_val&0x3F00)>>8);
	if ( pAd->chipCap.rate_pwr_table.HT[6].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.HT[6].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.VHT[6].MCS_Power = pAd->chipCap.rate_pwr_table.HT[6].MCS_Power;

	RTMP_IO_READ32(pAd, TX_PWR_CFG_3, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", TX_PWR_CFG_3, reg_val));
	pAd->chipCap.rate_pwr_table.STBC[0].MCS_Power = (CHAR)((reg_val&0x3F0000)>>16);
	if ( pAd->chipCap.rate_pwr_table.STBC[0].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.STBC[0].MCS_Power -= 64;
	
	pAd->chipCap.rate_pwr_table.STBC[1].MCS_Power = (CHAR)((reg_val&0x3F0000)>>16);
	if ( pAd->chipCap.rate_pwr_table.STBC[1].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.STBC[1].MCS_Power -= 64;
	
	pAd->chipCap.rate_pwr_table.STBC[2].MCS_Power = (CHAR)((reg_val&0x3F000000)>>24);
	if ( pAd->chipCap.rate_pwr_table.STBC[2].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.STBC[2].MCS_Power -= 64;
	
	pAd->chipCap.rate_pwr_table.STBC[3].MCS_Power = (CHAR)((reg_val&0x3F000000)>>24);
	if ( pAd->chipCap.rate_pwr_table.STBC[3].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.STBC[3].MCS_Power -= 64;

	RTMP_IO_READ32(pAd, TX_PWR_CFG_4, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", TX_PWR_CFG_4, reg_val));
	pAd->chipCap.rate_pwr_table.STBC[4].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.STBC[4].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.STBC[4].MCS_Power -= 64;
	
	pAd->chipCap.rate_pwr_table.STBC[5].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.STBC[5].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.STBC[5].MCS_Power -= 64;
	
	pAd->chipCap.rate_pwr_table.STBC[6].MCS_Power = (CHAR)((reg_val&0x3F00)>>8);
	if ( pAd->chipCap.rate_pwr_table.STBC[6].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.STBC[6].MCS_Power -= 64;

	RTMP_IO_READ32(pAd, TX_PWR_CFG_7, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", TX_PWR_CFG_7, reg_val));
	pAd->chipCap.rate_pwr_table.OFDM[7].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.OFDM[7].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.OFDM[7].MCS_Power -= 64;
	
	pAd->chipCap.rate_pwr_table.HT[7].MCS_Power = (CHAR)((reg_val&0x3F0000)>>16);
	if ( pAd->chipCap.rate_pwr_table.HT[7].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.HT[7].MCS_Power -= 64;
	pAd->chipCap.rate_pwr_table.VHT[7].MCS_Power = pAd->chipCap.rate_pwr_table.HT[7].MCS_Power;

	RTMP_IO_READ32(pAd, TX_PWR_CFG_8, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", TX_PWR_CFG_8, reg_val));
	pAd->chipCap.rate_pwr_table.VHT[8].MCS_Power = (CHAR)((reg_val&0x3F0000)>>16);;
	if ( pAd->chipCap.rate_pwr_table.VHT[8].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.VHT[8].MCS_Power -= 64;
	
	pAd->chipCap.rate_pwr_table.VHT[9].MCS_Power = (CHAR)((reg_val&0x3F000000)>>24);;
	if ( pAd->chipCap.rate_pwr_table.VHT[9].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.VHT[9].MCS_Power -= 64;

	RTMP_IO_READ32(pAd, TX_PWR_CFG_9, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", TX_PWR_CFG_9, reg_val));
	pAd->chipCap.rate_pwr_table.STBC[7].MCS_Power = (CHAR)(reg_val&0x3F);
	if ( pAd->chipCap.rate_pwr_table.STBC[7].MCS_Power & 0x20 )
		pAd->chipCap.rate_pwr_table.STBC[7].MCS_Power -= 64;

	// PA MODE
	RTMP_IO_READ32(pAd, RF_PA_MODE_CFG0, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", RF_PA_MODE_CFG0, reg_val));
	pAd->chipCap.rate_pwr_table.CCK[0].RF_PA_Mode = (UCHAR)(reg_val&0x00000003);
	pAd->chipCap.rate_pwr_table.CCK[1].RF_PA_Mode = (UCHAR)((reg_val&0x0000000C)>>2);
	pAd->chipCap.rate_pwr_table.CCK[2].RF_PA_Mode = (UCHAR)((reg_val&0x00000030)>>4);
	pAd->chipCap.rate_pwr_table.CCK[3].RF_PA_Mode = (UCHAR)((reg_val&0x000000C0)>>6);
	pAd->chipCap.rate_pwr_table.OFDM[0].RF_PA_Mode = (UCHAR)((reg_val&0x00000300)>>8);
	pAd->chipCap.rate_pwr_table.OFDM[1].RF_PA_Mode = (UCHAR)((reg_val&0x00000C00)>>10);
	pAd->chipCap.rate_pwr_table.OFDM[2].RF_PA_Mode = (UCHAR)((reg_val&0x00003000)>>12);
	pAd->chipCap.rate_pwr_table.OFDM[3].RF_PA_Mode = (UCHAR)((reg_val&0x0000C000)>>14);
	pAd->chipCap.rate_pwr_table.OFDM[4].RF_PA_Mode = (UCHAR)((reg_val&0x00030000)>>16);
	pAd->chipCap.rate_pwr_table.OFDM[5].RF_PA_Mode = (UCHAR)((reg_val&0x000C0000)>>18);
	pAd->chipCap.rate_pwr_table.OFDM[6].RF_PA_Mode = (UCHAR)((reg_val&0x00300000)>>20);
	pAd->chipCap.rate_pwr_table.OFDM[7].RF_PA_Mode = (UCHAR)((reg_val&0x00C00000)>>22);
	pAd->chipCap.rate_pwr_table.MCS32.RF_PA_Mode = (UCHAR)((reg_val&0x03000000)>>24);

	RTMP_IO_READ32(pAd, RF_PA_MODE_CFG1, &reg_val);
	DBGPRINT(RT_DEBUG_INFO, ("0x%x: 0x%x\n", RF_PA_MODE_CFG1, reg_val));
	pAd->chipCap.rate_pwr_table.HT[0].RF_PA_Mode = (UCHAR)(reg_val&0x00000003);
	pAd->chipCap.rate_pwr_table.VHT[0].RF_PA_Mode = pAd->chipCap.rate_pwr_table.HT[0].RF_PA_Mode;
	pAd->chipCap.rate_pwr_table.HT[1].RF_PA_Mode = (UCHAR)((reg_val&0x0000000C)>>2);
	pAd->chipCap.rate_pwr_table.VHT[1].RF_PA_Mode = pAd->chipCap.rate_pwr_table.HT[1].RF_PA_Mode;
	pAd->chipCap.rate_pwr_table.HT[2].RF_PA_Mode = (UCHAR)((reg_val&0x00000030)>>4);
	pAd->chipCap.rate_pwr_table.VHT[2].RF_PA_Mode = pAd->chipCap.rate_pwr_table.HT[2].RF_PA_Mode;
	pAd->chipCap.rate_pwr_table.HT[3].RF_PA_Mode = (UCHAR)((reg_val&0x000000C0)>>6);
	pAd->chipCap.rate_pwr_table.VHT[3].RF_PA_Mode = pAd->chipCap.rate_pwr_table.HT[3].RF_PA_Mode;
	pAd->chipCap.rate_pwr_table.HT[4].RF_PA_Mode = (UCHAR)((reg_val&0x00000300)>>8);
	pAd->chipCap.rate_pwr_table.VHT[4].RF_PA_Mode = pAd->chipCap.rate_pwr_table.HT[4].RF_PA_Mode;
	pAd->chipCap.rate_pwr_table.HT[5].RF_PA_Mode = (UCHAR)((reg_val&0x00000C00)>>10);
	pAd->chipCap.rate_pwr_table.VHT[5].RF_PA_Mode = pAd->chipCap.rate_pwr_table.HT[5].RF_PA_Mode;
	pAd->chipCap.rate_pwr_table.HT[6].RF_PA_Mode = (UCHAR)((reg_val&0x00003000)>>12);
	pAd->chipCap.rate_pwr_table.VHT[6].RF_PA_Mode = pAd->chipCap.rate_pwr_table.HT[6].RF_PA_Mode;
	pAd->chipCap.rate_pwr_table.HT[7].RF_PA_Mode = (UCHAR)((reg_val&0x0000C000)>>14);
	pAd->chipCap.rate_pwr_table.VHT[7].RF_PA_Mode = pAd->chipCap.rate_pwr_table.HT[7].RF_PA_Mode;
	pAd->chipCap.rate_pwr_table.VHT[8].RF_PA_Mode = (UCHAR)((reg_val&0x00030000)>>16);
	pAd->chipCap.rate_pwr_table.VHT[9].RF_PA_Mode = (UCHAR)((reg_val&0x000C0000)>>18);
}


#ifdef MT76x0_TSSI_CAL_COMPENSATION
VOID MT76x0_TSSI_DC_Calibration(
	IN  RTMP_ADAPTER *pAd)
{
	UCHAR RF_Value;
	UINT32 MAC_Value, BBP_Value;
	USHORT i = 0;

#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
	{
		pAd->hw_cfg.cent_ch = pAd->ate.Channel;
	}
#endif /* CONFIG_ATE */

	if( pAd->hw_cfg.cent_ch > 14 )
	{
		rlt_rf_read(pAd, RF_BANK0, RF_R67, &RF_Value);
		RF_Value &= 0xF0;
		rlt_rf_write(pAd, RF_BANK0, RF_R67, RF_Value);
	}

	// Enable 9bit I channel ADC and get TSSI DC point from BBP
	{
		// Bypass ADDA controls
		MAC_Value = 0x60002237;
		RTMP_IO_WRITE32(pAd, RLT_RF_SETTING_0, MAC_Value);
		MAC_Value = 0xFFFFFFFF;
		RTMP_IO_WRITE32(pAd, RLT_RF_BYPASS_0, MAC_Value);

		//********************************************************************//
		// BBP Soft Reset
		RTMP_IO_READ32(pAd, CORE_R4, &BBP_Value);
		BBP_Value |= 0x00000001;
		RTMP_IO_WRITE32(pAd, CORE_R4, BBP_Value);

		RtmpusecDelay(1);

		RTMP_IO_READ32(pAd, CORE_R4, &BBP_Value);
		BBP_Value &= 0xFFFFFFFE;
		RTMP_IO_WRITE32(pAd, CORE_R4, BBP_Value);
		//********************************************************************//

		if( pAd->hw_cfg.cent_ch > 14 )
		{
			// EXT TSSI
			// Set avg mode on Q channel
			BBP_Value = 0x00080055;
			RTMP_IO_WRITE32(pAd, CORE_R34, BBP_Value);
		}
		else
		{
			// Set avg mode on I channel
			BBP_Value = 0x00080050;
			RTMP_IO_WRITE32(pAd, CORE_R34, BBP_Value);
		}

        // Enable TX with 0 DAC inputs
        BBP_Value = 0x80000000;
		RTMP_IO_WRITE32(pAd, TXBE_R6, BBP_Value);

		// Wait until avg done
		do
		{
			RTMP_IO_READ32(pAd, CORE_R34, &BBP_Value);

			if ( (BBP_Value&0x10) == 0 )
				break;

			i++;
			if ( i >= 100 )
				break;

			RtmpusecDelay(10);

		} while (TRUE);

		// Read TSSI value
		RTMP_IO_READ32(pAd, CORE_R35, &BBP_Value);
		pAd->chipCap.tssi_current_DC = (CHAR)(BBP_Value&0xFF);

		// stop bypass ADDA
		//              MAC_Value = 0x0;
		//              rtmp.HwMemoryWriteDword(RA_RF_SETTING_0, MAC_Value, 4);
		MAC_Value = 0x0;
		RTMP_IO_WRITE32(pAd, RLT_RF_BYPASS_0, MAC_Value);

		// Stop TX
		BBP_Value = 0x0;
		RTMP_IO_WRITE32(pAd, TXBE_R6, BBP_Value);

		//********************************************************************//
		// BBP Soft Reset
		RTMP_IO_READ32(pAd, CORE_R4, &BBP_Value);
		BBP_Value |= 0x00000001;
		RTMP_IO_WRITE32(pAd, CORE_R4, BBP_Value);

		RtmpusecDelay(1);

		RTMP_IO_READ32(pAd, CORE_R4, &BBP_Value);
		BBP_Value &= 0xFFFFFFFE;
		RTMP_IO_WRITE32(pAd, CORE_R4, BBP_Value);
		//********************************************************************//
	}

	// Restore
	{
		if( pAd->hw_cfg.cent_ch > 14 )
		{
			// EXT TSSI
			// Reset tssi_cal

			rlt_rf_read(pAd, RF_BANK0, RF_R67, &RF_Value);
			RF_Value &= 0xF0;
			RF_Value |= 0x04;
			rlt_rf_write(pAd, RF_BANK0, RF_R67, RF_Value);

		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): Current_TSSI_DC = %d\n", __FUNCTION__, pAd->chipCap.tssi_current_DC));
}


BOOLEAN MT76x0_Enable9BitIchannelADC(
	IN  RTMP_ADAPTER *pAd,
	IN  UCHAR Channel,
	IN  SHORT *pTSSI_Linear)
{
	UINT32 bbp_val;
	UINT32 MTxCycle = 0, i = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): Channel = %d\n", __FUNCTION__, Channel));

	if(Channel > 14)
	{
		/*
			EXT TSSI
			Set avg mode on Q channel
		*/
		bbp_val = 0x00080055;
	}
	else
	{
		/*
			Set avg mode on I channel
		*/
		bbp_val = 0x00080050;
	}

	RTMP_BBP_IO_WRITE32(pAd, CORE_R34, bbp_val);

	/*
		Wait until it's done
		wait until 0x2088[4] = 0
	*/
	for (MTxCycle = 0; MTxCycle < 200; MTxCycle++)
	{
		RTMP_BBP_IO_READ32(pAd, CORE_R34, &bbp_val);
		if ((bbp_val & 0x10) == 0)
			break;
		RtmpusecDelay(10);
	}

	if (MTxCycle >= 200)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: We cannot wait too long, give up!\n", __FUNCTION__));
		bbp_val &= ~(0x10);
		RTMP_BBP_IO_WRITE32(pAd, CORE_R34, bbp_val);
		return FALSE;
	}
	
	/* 
		Read TSSI value 
	*/
	RTMP_BBP_IO_READ32(pAd, CORE_R35, &bbp_val);

	*pTSSI_Linear = (CHAR)(bbp_val&0xFF);
	DBGPRINT(RT_DEBUG_TRACE, ("%s: CORE_R35 = 0x%X, TSSI_Linear = (CHAR)(BBP_Value&0xFF) = 0x%X\n", __FUNCTION__, bbp_val, *pTSSI_Linear));

	if (Channel > 14)
	{
		*pTSSI_Linear = *pTSSI_Linear + 128;
		DBGPRINT(RT_DEBUG_TRACE, ("%s: TSSI_Linear = TSSI_Linear + 128 = 0x%X\n", __FUNCTION__, *pTSSI_Linear));
	}

	/*
		Set Packet Info#1 mode
	*/
	bbp_val = 0x00080041;
	RTMP_BBP_IO_WRITE32(pAd, CORE_R34, bbp_val);

	/*
		Read Info #1
	*/
	RTMP_BBP_IO_READ32(pAd, CORE_R35, &bbp_val);
	pAd->chipCap.tssi_info_1 = (UCHAR)(bbp_val&0xFF);

    /*
    	Set Packet Info#2 mode
    */
    bbp_val = 0x00080042;
	RTMP_BBP_IO_WRITE32(pAd, CORE_R34, bbp_val);

	/*
		Read Info #2
	*/
	RTMP_BBP_IO_READ32(pAd, CORE_R35, &bbp_val);
	pAd->chipCap.tssi_info_2 = (UCHAR)(bbp_val&0xFF);

	/*
		Set Packet Info#3 mode
	*/
	bbp_val = 0x00080043;
	RTMP_BBP_IO_WRITE32(pAd, CORE_R34, bbp_val);

	/* 
		Read Info #3
	*/
	RTMP_BBP_IO_READ32(pAd, CORE_R35, &bbp_val);
	pAd->chipCap.tssi_info_3 = (UCHAR)(bbp_val&0xFF);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: TSSI_Linear = 0x%X\n", __FUNCTION__, *pTSSI_Linear));
	DBGPRINT(RT_DEBUG_TRACE, ("%s: INFO_1 = 0x%X\n", __FUNCTION__, pAd->chipCap.tssi_info_1));
	DBGPRINT(RT_DEBUG_TRACE, ("%s: INFO_2 = 0x%X\n", __FUNCTION__, pAd->chipCap.tssi_info_2));
	DBGPRINT(RT_DEBUG_TRACE, ("%s: INFO_3 = 0x%X\n", __FUNCTION__, pAd->chipCap.tssi_info_3));
	return TRUE;
}


BOOLEAN MT76x0_GetTargetPower(
	IN  RTMP_ADAPTER *pAd,
	IN  CHAR *pTSSI_Tx_Mode,
	IN  CHAR *pTargetPower,
	IN  CHAR *pTargetPA_mode)
{
	UCHAR Tx_Rate, CurrentPower0;
	USHORT index;
	UINT32 reg_val = 0;
	CHAR Eas_power_adj = 0;

	RTMP_IO_READ32(pAd, TX_ALC_CFG_0, &reg_val);
	CurrentPower0 = (UCHAR)(reg_val&0x3F);

	*pTSSI_Tx_Mode = (pAd->chipCap.tssi_info_1 & 0x7);
	Eas_power_adj = (pAd->chipCap.tssi_info_3 & 0xF);

	if (*pTSSI_Tx_Mode == 0)
	{
		/*
			0: 1 Mbps, 1: 2 Mbps, 2: 5.5 Mbps, 3: 11 Mbps
		*/
		Tx_Rate = ((pAd->chipCap.tssi_info_1 & 0x60) >> 5);

		if (Tx_Rate > 3)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s ==> CCK Mode :: Unknown Tx_Rate = %d, return here.\n", __FUNCTION__, Tx_Rate));
			return FALSE;
		}

		*pTargetPower = (CHAR)(CurrentPower0 + pAd->chipCap.rate_pwr_table.CCK[Tx_Rate].MCS_Power);
		*pTargetPA_mode = (CHAR) pAd->chipCap.rate_pwr_table.CCK[Tx_Rate].RF_PA_Mode;

		DBGPRINT(RT_DEBUG_TRACE, ("==> CCK Mode :: TargetPower = %d\n", *pTargetPower));
	}
	else if (*pTSSI_Tx_Mode == 1)
	{
		Tx_Rate = ((pAd->chipCap.tssi_info_1 & 0xF0) >> 4);
		if ( Tx_Rate == 0xB )
			index = 0;
		else if ( Tx_Rate == 0xF )
			index = 1;
		else if ( Tx_Rate == 0xA )
			index = 2;
		else if ( Tx_Rate == 0xE )
			index = 3;
		else if ( Tx_Rate == 0x9 )
			index = 4;
		else if ( Tx_Rate == 0xD )
			index = 5;
		else if ( Tx_Rate == 0x8 )
			index = 6;
		else if ( Tx_Rate == 0xC )
			index = 7;
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s ==> OFDM Mode :: Unknown Tx_Rate = 0x%x, return here.\n", __FUNCTION__, Tx_Rate));
			return FALSE;
		}

		*pTargetPower = (CHAR)(CurrentPower0 + pAd->chipCap.rate_pwr_table.OFDM[index].MCS_Power);
		*pTargetPA_mode = pAd->chipCap.rate_pwr_table.OFDM[index].RF_PA_Mode;

		DBGPRINT(RT_DEBUG_TRACE, ("==> OFDM Mode :: TargetPower0 = %d (MCS%d)\n", *pTargetPower, index));
	}
	else if (*pTSSI_Tx_Mode == 4)
	{
		Tx_Rate = (pAd->chipCap.tssi_info_2 & 0x0F);

		if (Tx_Rate > 9)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s ==> VHT Mode :: Unknown Tx_Rate = %d, return here.\n", __FUNCTION__, Tx_Rate));
			return FALSE;
		}

		DBGPRINT(RT_DEBUG_TRACE, ("==> VHT Mode :: CurrentPower0 = %d, pAd->chipCap.tssi_table.VHT[%d].MCS_Power = %d\n", 
			CurrentPower0, Tx_Rate, pAd->chipCap.rate_pwr_table.VHT[Tx_Rate].MCS_Power));
		
		*pTargetPower = (CHAR)(CurrentPower0 + pAd->chipCap.rate_pwr_table.VHT[Tx_Rate].MCS_Power);
		*pTargetPA_mode = (CHAR) pAd->chipCap.rate_pwr_table.VHT[Tx_Rate].RF_PA_Mode;
		
		DBGPRINT(RT_DEBUG_TRACE, ("==> VHT Mode :: TargetPower0 = %d (MCS%d)\n", *pTargetPower, Tx_Rate));
	}
    else
    {
		Tx_Rate = (pAd->chipCap.tssi_info_2 & 0x7F);

		if ( Tx_Rate == 32 ) // MCS32
		{
			*pTargetPower = (CHAR)(CurrentPower0 + pAd->chipCap.rate_pwr_table.MCS32.MCS_Power);
			*pTargetPA_mode = pAd->chipCap.rate_pwr_table.MCS32.RF_PA_Mode;

		}
		else
		{
			if (Tx_Rate > 9)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s ==> HT Mode :: Unknown Tx_Rate = %d, return here.\n", __FUNCTION__, Tx_Rate));
				return FALSE;
			}
			
			*pTargetPower = (CHAR)(CurrentPower0 + pAd->chipCap.rate_pwr_table.HT[Tx_Rate].MCS_Power);
			*pTargetPA_mode = pAd->chipCap.rate_pwr_table.HT[Tx_Rate].RF_PA_Mode;
		}
		DBGPRINT(RT_DEBUG_TRACE, ("==> HT Mode :: TargetPower0 = %d (MCS%d)\n", *pTargetPower, Tx_Rate));
		return TRUE;
	}
	return TRUE;
}


VOID MT76x0_EstimateDeltaPower(
	IN  RTMP_ADAPTER *pAd,
	IN  CHAR TSSI_Tx_Mode,
	IN  SHORT TSSI_Linear,
	IN  CHAR TargetPower,
	IN  CHAR TargetPA_mode,
	IN  INT *tssi_delta0)
{
	USHORT EE_Value;
	INT tssi_slope=0;
	INT tssi_offset=0;
	INT tssi_target=0, tssi_delta_tmp;
	INT tssi_meas=0;
	INT tssi_dc;
	INT pkt_type_delta=0, bbp_6db_power=0;
	UINT32 BBP_Value;
	CHAR idx = 0;

	// a.  tssi_dc gotten from Power on calibration

	// b.  Read Slope: u.2.6 (MT7601)
	// c.  Read offset: s.3.4 (MT7601)
	if (pAd->hw_cfg.cent_ch > 14)
	{
		for (idx = 0; idx < 7; idx++)
		{
			if ((pAd->hw_cfg.cent_ch <= pAd->chipCap.tssi_5G_channel_boundary[idx])
				|| (pAd->chipCap.tssi_5G_channel_boundary[idx] == 0))
			{
				tssi_slope = pAd->chipCap.tssi_slope_5G[idx];
				tssi_offset = pAd->chipCap.tssi_offset_5G[idx];
				DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_5G_channel_boundary[%d] = %d\n", idx, pAd->chipCap.tssi_5G_channel_boundary[idx]));
				DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_slope_5G[%d] = %d\n", idx, pAd->chipCap.tssi_slope_5G[idx]));
				DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_offset_5G[%d] = %d\n", idx, pAd->chipCap.tssi_offset_5G[idx]));
				DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_slope = %d\n", tssi_slope));
				DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_offset = %d\n", tssi_offset));
				break;
			}
		}
		if (idx == 7)
		{
			tssi_slope = pAd->chipCap.tssi_slope_5G[idx];
			tssi_offset = pAd->chipCap.tssi_offset_5G[idx];
			DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_slope_5G[%d] = %d\n", idx, pAd->chipCap.tssi_slope_5G[idx]));
			DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_offset_5G[%d] = %d\n", idx, pAd->chipCap.tssi_offset_5G[idx]));
		}
	}
	else
	{
		tssi_slope = pAd->chipCap.tssi_slope_2G;
		tssi_offset = pAd->chipCap.tssi_offset_2G;
	}

	if ( pAd->hw_cfg.cent_ch > 14 )
	{
		/*
			0x40 ~ 0x7F remapping to -192 ~ -129
		*/
		if ( (tssi_offset >= 0x40) && (tssi_offset <= 0x7F) )
			tssi_offset = tssi_offset - 0x100;
		else
			tssi_offset = (tssi_offset & 0x80) ?  tssi_offset - 0x100 : tssi_offset;
	}
	else
	{
		tssi_offset = (tssi_offset & 0x80) ?  tssi_offset - 0x100 : tssi_offset;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("==> 1) tssi_offset = %d (0x%x)\n", tssi_offset, tssi_offset));

	DBGPRINT(RT_DEBUG_TRACE, ("\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("==> EstimateDeltaPower\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("==> TSSI_Tx_Mode = %d\n", TSSI_Tx_Mode));
	DBGPRINT(RT_DEBUG_TRACE, ("==> TargetPower = %d\n", TargetPower));
	DBGPRINT(RT_DEBUG_TRACE, ("==> TSSI_Linear = %d\n", TSSI_Linear));
	DBGPRINT(RT_DEBUG_TRACE, ("==> Current_TSSI_DC = %d\n", pAd->chipCap.tssi_current_DC));
	DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_slope0 = %d\n", tssi_slope));
	DBGPRINT(RT_DEBUG_TRACE, ("==> 2) tssi_offset0 = %d\n", tssi_offset));

	// d.
	// Cal delta0
	tssi_target = (TargetPower << 12);
	DBGPRINT(RT_DEBUG_TRACE, ("\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("==> (tssi_target = TargetPower0*4096) = %d\n", tssi_target));
	DBGPRINT(RT_DEBUG_TRACE, ("==> TargetPA_mode = %d\n", TargetPA_mode));
	
	switch(TargetPA_mode)
	{
		case 0: 
			tssi_target = tssi_target;
			DBGPRINT(RT_DEBUG_TRACE, ("==> (tssi_target = tssi_target) = %d\n", tssi_target));
			break;
		case 1: 
			if ( pAd->hw_cfg.cent_ch > 14 )
			{
				tssi_target = tssi_target + 0;
				DBGPRINT(RT_DEBUG_TRACE, ("==> (tssi_target = tssi_target + 0) = %d\n", tssi_target));
			}
			else
			{
				tssi_target = tssi_target + 29491; // 3.6 * 8192
				DBGPRINT(RT_DEBUG_TRACE, ("==> (tssi_target = tssi_target + 29491) = %d\n", tssi_target));
			}
			break;
		default: 
			tssi_target = tssi_target +  4424; // 0.54 * 8192
			DBGPRINT(RT_DEBUG_TRACE, ("==> (tssi_target = tssi_target +  4424) = %d\n", tssi_target));
			break;
	}

	RTMP_BBP_IO_READ32(pAd, CORE_R1, &BBP_Value);
	DBGPRINT(RT_DEBUG_TRACE, ("==> (0x%x = 0x%x)\n", CORE_R1, BBP_Value));
	switch(TSSI_Tx_Mode)
	{
		case 0:
			/*
				CCK power offset	With Japan filter	Without Japan filter
				7630E				+2.3db (2.3 * 8192)	+1.5db (1.5 * 8192)
				Other project		+0.8db (0.8 * 8192)	+0db
			*/
#ifdef RTMP_MAC_PCI
			if (IS_MT7630E(pAd))
			{
				if (BBP_Value&0x20)
					pkt_type_delta = 18841;//2.3 * 8192;
				else
					pkt_type_delta = 12288;//1.5 * 8192;
			}
			else
#endif /* RTMP_MAC_PCI */
			{
				if (BBP_Value&0x20)
					pkt_type_delta = 6554;//0.8 * 8192;
				else
					pkt_type_delta = 0;//0 * 8192;
			}
			break;
		default:
			pkt_type_delta = 0;
			break;
	}

	tssi_target = tssi_target + pkt_type_delta;

	RTMP_BBP_IO_READ32(pAd, TXBE_R4, &BBP_Value);
	DBGPRINT(RT_DEBUG_TRACE, ("==> TXBE_OFFSET+R4 = 0x%X\n", BBP_Value));
	switch( (BBP_Value&0x3) )
	{
		case 0: 
			bbp_6db_power = 0; 
			break;
		case 1: 
			bbp_6db_power = -49152; 
			break; //-6 dB*8192;
		case 2: 
			bbp_6db_power = -98304; 
			break; //-12 dB*8192;
		case 3: 
			bbp_6db_power = 49152; 
			break;  //6 dB*8192;
	}
	DBGPRINT(RT_DEBUG_TRACE, ("==> bbp_6db_power = %d\n", bbp_6db_power));
	
	tssi_target = tssi_target + bbp_6db_power;
	DBGPRINT(RT_DEBUG_TRACE, ("==> (tssi_target = tssi_target + bbp_6db_power) = %d\n", tssi_target));

	DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_target = %d\n", (tssi_target) >> 13));
	tssi_dc = pAd->chipCap.tssi_current_DC;
	DBGPRINT(RT_DEBUG_TRACE, ("==> TSSI_Linear0 = %d\n", TSSI_Linear)); 
	DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_dc = %d\n", tssi_dc)); 
	
	tssi_meas = MT76xx_lin2dBd( (TSSI_Linear - tssi_dc));
	DBGPRINT(RT_DEBUG_TRACE, ("==> Linear to dB = %d\n", tssi_meas)); 

	tssi_meas = tssi_meas *tssi_slope;
	DBGPRINT(RT_DEBUG_TRACE, ("==> dB x slope = %d (0x%x), tssi_offset = %d(0x%x)\n", tssi_meas, tssi_meas, tssi_offset, tssi_offset));
	DBGPRINT(RT_DEBUG_TRACE, ("==> (tssi_offset-50) = (%d)(0x%x)\n", (tssi_offset-50), (tssi_offset-50)));
	DBGPRINT(RT_DEBUG_TRACE, ("==> (tssi_offset-50)<< 10 = (%d)(0x%x)\n", ((tssi_offset-50) << 10), ((tssi_offset-50) << 10)));
	if( pAd->hw_cfg.cent_ch > 14 )
		tssi_meas += ((tssi_offset-50) << 10); // 5G: offset s4.3
	else
		tssi_meas += (tssi_offset << 9); // 2G: offset s3.4
	DBGPRINT(RT_DEBUG_TRACE, ("==> measure db = %d (0x%x) %d\n", tssi_meas, tssi_meas, (tssi_meas) >> 13));
	
	tssi_delta_tmp = tssi_target - tssi_meas;
	DBGPRINT(RT_DEBUG_TRACE, ("==> delta db = %d\n", tssi_delta_tmp));

	DBGPRINT(RT_DEBUG_TRACE, ("\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("==> TSSI_Linear0 = %d\n", TSSI_Linear));
	DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_delta_tmp = %d\n", tssi_delta_tmp));

	if( pAd->hw_cfg.cent_ch > 14 )
	{
		if((TSSI_Linear > 254) && (tssi_delta_tmp > 0)) //upper saturate
			tssi_delta_tmp = 0;
	}
	else
	{
		if((TSSI_Linear > 126) && (tssi_delta_tmp > 0)) //upper saturate
			tssi_delta_tmp = 0;
		if(((TSSI_Linear - tssi_dc) < 1  ) && (tssi_delta_tmp < 0)) //lower saturate
			tssi_delta_tmp = 0;
	}

	// stablize the compensation value
	// if previous compensation result is better than current, skip the compensation
	if( ((pAd->chipCap.tssi_pre_delta_pwr^tssi_delta_tmp) < 0) 
		&& ((tssi_delta_tmp < 4096) && (tssi_delta_tmp > -4096))
		&& ((pAd->chipCap.tssi_pre_delta_pwr < 4096) && (pAd->chipCap.tssi_pre_delta_pwr > -4096)) )
	{
		if((tssi_delta_tmp>0)&&((tssi_delta_tmp +pAd->chipCap.tssi_pre_delta_pwr) <= 0))
		    tssi_delta_tmp = 0;
		else if((tssi_delta_tmp<0)&&((tssi_delta_tmp +pAd->chipCap.tssi_pre_delta_pwr) > 0))
		    tssi_delta_tmp = 0;
		else
		    pAd->chipCap.tssi_pre_delta_pwr = tssi_delta_tmp;
}
	else
		pAd->chipCap.tssi_pre_delta_pwr = tssi_delta_tmp;

	DBGPRINT(RT_DEBUG_TRACE, ("==> TSSI_DELTA_PRE = %d\n", pAd->chipCap.tssi_pre_delta_pwr));
	DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_delta_tmp = %d\n", tssi_delta_tmp));

	// make the compensate value to the nearest compensate code
	tssi_delta_tmp = tssi_delta_tmp + ((tssi_delta_tmp > 0 ) ? 2048 : -2048);
	DBGPRINT(RT_DEBUG_TRACE, ("==> delta db = %d\n", tssi_delta_tmp));
	tssi_delta_tmp = tssi_delta_tmp >> 12;
	DBGPRINT(RT_DEBUG_TRACE, ("==> delta db = %d\n", tssi_delta_tmp));

	*tssi_delta0 = *tssi_delta0 + tssi_delta_tmp;
	DBGPRINT(RT_DEBUG_TRACE, ("==> *tssi_delta0 = %d\n", *tssi_delta0));
	if(*tssi_delta0 > 31)
		*tssi_delta0 = 31;
	else if(*tssi_delta0 < -32)
		*tssi_delta0 = -32;

	DBGPRINT(RT_DEBUG_TRACE, ("\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("==> tssi_delta0 = %d\n", *tssi_delta0));
	DBGPRINT(RT_DEBUG_TRACE, ("\n"));
}


VOID MT76x0_IntTxAlcProcess(
	IN  RTMP_ADAPTER *pAd)
{
	INT tssi_delta0 = 0;
	UINT32 reg_val = 0;
	CHAR tssi_write = 0;
	CHAR TargetPower = 0, TargetPA_mode = 0;
	SHORT TSSI_Linear = 0;
	CHAR TSSI_Tx_Mode = 0;

	if (MT76x0_Enable9BitIchannelADC(pAd, pAd->hw_cfg.cent_ch, &TSSI_Linear) == FALSE)
		return;

	if (MT76x0_GetTargetPower(pAd, &TSSI_Tx_Mode, &TargetPower, &TargetPA_mode) == FALSE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s ==> Get target power failed, return here.\n", __FUNCTION__));
		return;
	}

	RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &reg_val);
	DBGPRINT(RT_DEBUG_ERROR, ("(0x13B4) Before compensation 0x%08X\n", reg_val));
	tssi_delta0 = (CHAR)(reg_val&0x3F);
	if ( (tssi_delta0 &0x20) )
		tssi_delta0 -= 0x40;

	MT76x0_EstimateDeltaPower(pAd, TSSI_Tx_Mode, TSSI_Linear, TargetPower, TargetPA_mode, &tssi_delta0);

	tssi_write = tssi_delta0;

	reg_val &= 0xFFFFFFC0;
	reg_val |= (tssi_write&0x3F);
	DBGPRINT(RT_DEBUG_ERROR, ("%s ==> reg_val = 0x%08X\n", __FUNCTION__, reg_val));
	RTMP_IO_WRITE32(pAd, TX_ALC_CFG_1, reg_val);
	RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &reg_val);
	DBGPRINT(RT_DEBUG_ERROR, ("(0x13B4) After compensation 0x%08X\n", reg_val));
}
#endif /* MT76x0_TSSI_CAL_COMPENSATION */


#ifdef SINGLE_SKU_V2
UCHAR MT76x0_GetSkuChannelBasePwr(
	IN PRTMP_ADAPTER 	pAd,
	IN UCHAR 			channel)
{
	CH_POWER *ch, *ch_temp;
	UCHAR base_pwr = pAd->DefaultTargetPwr;
	int i;
	
	DlListForEachSafe(ch, ch_temp, &pAd->SingleSkuPwrList, CH_POWER, List)
	{
		if (channel == ch->StartChannel)
		{
			if (channel <= 14)
			{
				for ( i= 0 ; i < SINGLE_SKU_TABLE_CCK_LENGTH ; i++ )
				{
					if ( base_pwr > ch->PwrCCK[i] )
						base_pwr = ch->PwrCCK[i];
				}
			}

			for ( i= 0 ; i < SINGLE_SKU_TABLE_OFDM_LENGTH ; i++ )
			{
				if ( base_pwr > ch->PwrOFDM[i] )
					base_pwr = ch->PwrOFDM[i];
			}

			for ( i= 0 ; i < SINGLE_SKU_TABLE_HT_LENGTH ; i++ )
			{
				if ( base_pwr > ch->PwrHT20[i] )
					base_pwr = ch->PwrHT20[i];
			}
				
			if (pAd->CommonCfg.BBPCurrentBW == BW_40)
			{
				for ( i= 0 ; i < SINGLE_SKU_TABLE_HT_LENGTH ; i++ )
				{
					if ( ch->PwrHT40[i] == 0 )
						break;

					if ( base_pwr > ch->PwrHT40[i] )
						base_pwr = ch->PwrHT40[i];
				}
			}
			if (pAd->CommonCfg.BBPCurrentBW == BW_80)
			{
				for ( i= 0 ; i < SINGLE_SKU_TABLE_VHT_LENGTH ; i++ )
				{
					if ( ch->PwrVHT80[i] == 0 )
						break;

					if ( base_pwr > ch->PwrVHT80[i] )
						base_pwr = ch->PwrVHT80[i];
				}
			}
			break;
		}
	}

	return base_pwr;

}


VOID MT76x0_WriteNewPerRatePwr(
	IN  RTMP_ADAPTER *pAd)
{
	UINT32 data;
	UCHAR t1, t2, t3, t4;

	/* 
		bit 29:24 -> OFDM 12M/18M
		bit 21:16 -> OFDM 6M/9M
		bit 13:8 -> CCK 5.5M/11M
		bit 5:0 -> CCK 1M/2M
	*/
	t1 = pAd->chipCap.rate_pwr_table.CCK[0].MCS_Power;
	t1 = (t1 & 0x80) ? ((t1 & 0x1f) | 0x20) : (t1 & 0x3f);
	
	t2 = pAd->chipCap.rate_pwr_table.CCK[2].MCS_Power;
	t2 = (t2 & 0x80) ? ((t2 & 0x1f) | 0x20) : (t2 & 0x3f);

	t3 = pAd->chipCap.rate_pwr_table.OFDM[0].MCS_Power;
	t3 = (t3 & 0x80) ? ((t3 & 0x1f) | 0x20) : (t3 & 0x3f);
	
	t4 = pAd->chipCap.rate_pwr_table.OFDM[2].MCS_Power;
	t4 = (t4 & 0x80) ? ((t4 & 0x1f) | 0x20) : (t4 & 0x3f);
	data = (t4 << 24)|(t3 << 16)|(t2 << 8)|t1; 

	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0, data);

	/* 
		bit 29:24 -> HT MCS=2,3, VHT 1SS MCS=2,3
		bit 21:16 -> HT MCS=0,1, VHT 1SS MCS=0,1
		bit 13:8 -> OFDM 48M
		bit 5:0 -> OFDM 24M/36M
	*/	
	t1 = pAd->chipCap.rate_pwr_table.OFDM[4].MCS_Power;
	t1 = (t1 & 0x80) ? ((t1 & 0x1f) | 0x20) : (t1 & 0x3f);
	
	t2 = pAd->chipCap.rate_pwr_table.OFDM[6].MCS_Power;
	t2 = (t2 & 0x80) ? ((t2 & 0x1f) | 0x20) : (t2 & 0x3f);

	t3 = pAd->chipCap.rate_pwr_table.HT[0].MCS_Power;
	t3 = (t3 & 0x80) ? ((t3 & 0x1f) | 0x20) : (t3 & 0x3f);
	
	t4 = pAd->chipCap.rate_pwr_table.HT[2].MCS_Power;
	t4 = (t4 & 0x80) ? ((t4 & 0x1f) | 0x20) : (t4 & 0x3f);
	data = (t4 << 24)|(t3 << 16)|(t2 << 8)|t1; 

	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_1, data);

	/*
		bit 13:8 -> HT MCS=6, VHT 1SS MCS=6
		bit 5:0 -> MCS=4,5, VHT 1SS MCS=4,5
	*/
	t1 = pAd->chipCap.rate_pwr_table.HT[4].MCS_Power;
	t1 = (t1 & 0x80) ? ((t1 & 0x1f) | 0x20) : (t1 & 0x3f);
	
	t2 = pAd->chipCap.rate_pwr_table.HT[6].MCS_Power;
	t2 = (t2 & 0x80) ? ((t2 & 0x1f) | 0x20) : (t2 & 0x3f);
	data = (t2 << 8)|t1; 
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_2, data);

	/* 
		bit 29:24 -> HT/VHT STBC MCS=2, 3
		bit 21:16 -> HT/VHT STBC MCS=0, 1
	*/
	t3 = pAd->chipCap.rate_pwr_table.STBC[0].MCS_Power;
	t3 = (t3 & 0x80) ? ((t3 & 0x1f) | 0x20) : (t3 & 0x3f);
	
	t4 = pAd->chipCap.rate_pwr_table.STBC[2].MCS_Power;
	t4 = (t4 & 0x80) ? ((t4 & 0x1f) | 0x20) : (t4 & 0x3f);
	data = (t4 << 24)|(t3 << 16);
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_3, data);

	/* 
		bit 13:8 -> HT/VHT STBC MCS=6
		bit 5:0 -> HT/VHT STBC MCS=4,5
	*/
	t1 = pAd->chipCap.rate_pwr_table.STBC[4].MCS_Power;
	t1 = (t1 & 0x80) ? ((t1 & 0x1f) | 0x20) : (t1 & 0x3f);
	
	t2 = pAd->chipCap.rate_pwr_table.STBC[6].MCS_Power;
	t2 = (t2 & 0x80) ? ((t2 & 0x1f) | 0x20) : (t2 & 0x3f);	
	data = (t2 << 8)|t1; 
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_4, data);

	/* 
		For OFDM_54 and HT_MCS_7, extra fill the corresponding register value into MAC 0x13D4 
		bit 21:16 -> HT MCS=7, VHT 2SS MCS=7
		bit 5:0 -> OFDM 54
	*/
	t1 = pAd->chipCap.rate_pwr_table.OFDM[7].MCS_Power;
	t1 = (t1 & 0x80) ? ((t1 & 0x1f) | 0x20) : (t1 & 0x3f);
	
	t3 = pAd->chipCap.rate_pwr_table.HT[7].MCS_Power;
	t3 = (t3 & 0x80) ? ((t3 & 0x1f) | 0x20) : (t3 & 0x3f);
	data = (t3 << 16)|t1; 
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_7, data);

	/* 
		bit 29:24 -> VHT 1SS MCS=9
		bit 21:16 -> VHT 1SS MCS=8
	*/
	t3 = pAd->chipCap.rate_pwr_table.VHT[8].MCS_Power;
	t3 = (t3 & 0x80) ? ((t3 & 0x1f) | 0x20) : (t3 & 0x3f);
	
	t4 = pAd->chipCap.rate_pwr_table.VHT[9].MCS_Power;
	t4 = (t4 & 0x80) ? ((t4 & 0x1f) | 0x20) : (t4 & 0x3f);
	data = (t4 << 24)|(t3 << 16);
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_8, data);

	/* 
		For STBC_MCS_7, extra fill the corresponding register value into MAC 0x13DC 
		bit 5:0 -> HT/VHT STBC MCS=7
	*/
	data = pAd->chipCap.rate_pwr_table.STBC[7].MCS_Power;
	data = (data & 0x80) ? ((data & 0x1f) | 0x20) : (data & 0x3f);
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_9, data);
}


UCHAR MT76x0_UpdateSkuPwr(
	IN PRTMP_ADAPTER 	pAd,
	IN UCHAR 			channel)
{
	CH_POWER *ch, *ch_temp;
	INT32 i, pwr_delta = 0;
	UINT32 reg_val; 
	UCHAR ch_init_pwr = 0;
	CHAR ch_delta_pwr = 0;
	INT32 rate_pwr = 0;
	BOOLEAN bFound = FALSE;
	CHAR SkuBasePwr;
	const CHAR DefaultTargetPwr = pAd->DefaultTargetPwr;

	/*
		Get channel initial transmission gain.
	*/
	RTMP_IO_READ32(pAd, TX_ALC_CFG_0, &reg_val);
	ch_init_pwr = (UCHAR)(reg_val & 0x3F);
	if ( ch_init_pwr & 0x20 )
		ch_init_pwr -= 64;	

	RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &reg_val);
	ch_delta_pwr = (UCHAR)(reg_val & 0x3F);
	if ( ch_delta_pwr & 0x20 )
		ch_delta_pwr -= 64;
	
	SkuBasePwr = MT76x0_GetSkuChannelBasePwr(pAd, channel);
	
#if 1
	/* Load default rate power */
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0, pAd->Tx20MPwrCfgABand[0]);
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_1, pAd->Tx40MPwrCfgABand[0]);
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_2, pAd->Tx40MPwrCfgABand[1]);
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_3, pAd->Tx40MPwrCfgABand[2]);
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_4, pAd->Tx40MPwrCfgABand[3]);
#ifdef DOT11_VHT_AC
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_8, pAd->Tx80MPwrCfgABand[0]);
#endif /* DOT11_VHT_AC */
	MT76x0_AsicExtraPowerOverMAC(pAd);
#endif
	
	/*
		Get per rate register setting.
	*/
	MT76x0_MakeUpRatePwrTable(pAd);
	
	DlListForEachSafe(ch, ch_temp, &pAd->SingleSkuPwrList, CH_POWER, List)
	{
		if (channel == ch->StartChannel)
		{
#if 0 /* MT7610E doesn't have CCK mode. Need to consider CCK mode for MT7610U, MT7630E and MT7650E in the future. */			

			for ( i= 0 ; i < SINGLE_SKU_TABLE_CCK_LENGTH ; i++ )
			{
				if ( base_pwr > ch->PwrCCK[i] )
					base_pwr = ch->PwrCCK[i];
			}
#endif

			for (i = 0; i < SINGLE_SKU_TABLE_OFDM_LENGTH; i++)
			{
				pwr_delta = 0;
				
				rate_pwr = pAd->chipCap.rate_pwr_table.OFDM[i].MCS_Power + DefaultTargetPwr + ch_delta_pwr;
				pwr_delta = rate_pwr - ch->PwrOFDM[i];
				if ( pwr_delta > 0) //Power Exceed
					pAd->chipCap.rate_pwr_table.OFDM[i].MCS_Power -= pwr_delta;
			}

			for (i = 0; i < (SINGLE_SKU_TABLE_HT_LENGTH >> 1); i++)
			{
				pwr_delta = 0;

				rate_pwr = pAd->chipCap.rate_pwr_table.HT[i].MCS_Power + DefaultTargetPwr + ch_delta_pwr;
				if (pAd->CommonCfg.BBPCurrentBW == BW_20)
					pwr_delta = rate_pwr - ch->PwrHT20[i];
				else if (pAd->CommonCfg.BBPCurrentBW == BW_40)
					pwr_delta = rate_pwr - ch->PwrHT40[i];
				else if (pAd->CommonCfg.BBPCurrentBW == BW_80)
					pwr_delta = rate_pwr - ch->PwrVHT80[i];

				if ( pwr_delta > 0) //Power Exceed
					pAd->chipCap.rate_pwr_table.HT[i].MCS_Power -= pwr_delta;
			}

			for (i = 0; i < (SINGLE_SKU_TABLE_VHT_LENGTH >> 1); i++)
				{
				pwr_delta = 0;
				
				rate_pwr = pAd->chipCap.rate_pwr_table.VHT[i].MCS_Power + DefaultTargetPwr + ch_delta_pwr;
				if (pAd->CommonCfg.BBPCurrentBW == BW_20)
					pwr_delta = rate_pwr - ch->PwrHT20[i];
				else if (pAd->CommonCfg.BBPCurrentBW == BW_40)
					pwr_delta = rate_pwr - ch->PwrHT40[i];
				else if (pAd->CommonCfg.BBPCurrentBW == BW_80)
					pwr_delta = rate_pwr - ch->PwrVHT80[i];
				
				if ( pwr_delta > 0) //Power Exceed
					pAd->chipCap.rate_pwr_table.VHT[i].MCS_Power -= pwr_delta;
			}

			bFound = TRUE;
			break;
		}
	}

	if (bFound)
				{
		MT76x0_WriteNewPerRatePwr(pAd);
		return TRUE;
				}
				else
		return 0;
}
#endif /* SINGLE_SKU_V2 */


#ifdef RTMP_TEMPERATURE_COMPENSATION
BOOLEAN get_temp_tx_alc_level(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN enable_tx_alc,
	IN CHAR temp_ref,
	IN PCHAR temp_minus_bdy,
	IN PCHAR temp_plus_bdy,
	IN UINT8 max_bdy_level,
	IN UINT8 tx_alc_step,
	IN CHAR current_temp,
	OUT PCHAR comp_level)
{
	INT idx = 0;

	if ((temp_minus_bdy == NULL) || (temp_plus_bdy == NULL)) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): temp table boundary is NULL\n", __FUNCTION__)); 
		return FALSE;
	}
	
	if (enable_tx_alc) {
		if (current_temp < temp_minus_bdy[1]) {
			/* 	
				Reading is larger than the reference value check
				for how large we need to decrease the Tx power		
			*/
			for (idx = 1; idx < max_bdy_level; idx++)
			{
				if (current_temp >= temp_minus_bdy[idx]) 
					break; /* level range found */
			}

			/* The index is the step we should decrease, idx = 0 means there is nothing to compensate */
			*comp_level = -(tx_alc_step * (idx-1));
			DBGPRINT(RT_DEBUG_TRACE, 
				("-- Tx Power:: current_temp=%d, temp_ref=%d, tx_alc_step=%d, step = -%d, comp_level = %d\n",
			    	current_temp, temp_ref, tx_alc_step, idx-1, *comp_level));                    
		} else if (current_temp > temp_plus_bdy[1]) {
			/*	
				Reading is smaller than the reference value check
				for how large we need to increase the Tx power		
			*/
			for (idx = 1; idx < max_bdy_level; idx++)
			{
				if (current_temp <= temp_plus_bdy[idx])
					break; /* level range found */
			}

			/* The index is the step we should increase, idx = 0 means there is nothing to compensate */
			*comp_level = tx_alc_step * (idx-1);
			DBGPRINT(RT_DEBUG_TRACE,
				("++ Tx Power:: current_temp=%d, temp_ref=%d, tx_alc_step=%d, step = +%d, , comp_level = %d\n",
				current_temp, temp_ref, tx_alc_step, idx-1, *comp_level));
		} else {
			*comp_level = 0;
			DBGPRINT(RT_DEBUG_TRACE,
				("  Tx Power:: current_temp=%d, temp_ref=%d, tx_alc_step=%d, step = +%d\n",
				current_temp, temp_ref, tx_alc_step, 0));
		}
	} else {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): enable_tx_alc = %s\n", 
			__FUNCTION__, (enable_tx_alc) == TRUE ? "TRUE" : "FALSE")); 
		return FALSE;
	}

	return TRUE;
}
								

void mt76x0_temp_tx_alc(PRTMP_ADAPTER pAd)
{
	BOOLEAN bResetTssiInfo = TRUE, enable_tx_alc;
	PUCHAR temp_minus_bdy, temp_plus_bdy, tx_alc_comp;
	UCHAR temp_ref;
	
	if ((pAd->CommonCfg.Channel > 14) ? 
		(pAd->bAutoTxAgcA == FALSE) : (pAd->bAutoTxAgcG == FALSE))
		return;

	if (pAd->CommonCfg.Channel <= 14) {
		/* Use group settings of G band */
		temp_ref = pAd->TssiRefG;
		enable_tx_alc = pAd->bAutoTxAgcG;
		tx_alc_comp = &pAd->TxAgcCompensateG;
		temp_minus_bdy = pAd->TssiMinusBoundaryG;
		temp_plus_bdy = pAd->TssiPlusBoundaryG;
	} else if (pAd->CommonCfg.Channel <= pAd->ChBndryIdx) {
		/* Use group#1 settings of A band */
		temp_ref = pAd->TssiRefA;
		enable_tx_alc = pAd->bAutoTxAgcA;
		tx_alc_comp = &pAd->TxAgcCompensateA;
		temp_minus_bdy = pAd->TssiMinusBoundaryA[0];
		temp_plus_bdy = pAd->TssiPlusBoundaryA[0];
	} else {
		/* Use group#2 settings of A band */
		temp_ref = pAd->TssiRefA;
		enable_tx_alc = pAd->bAutoTxAgcA;
		tx_alc_comp = &pAd->TxAgcCompensateA;
		temp_minus_bdy = pAd->TssiMinusBoundaryA[1];
		temp_plus_bdy = pAd->TssiPlusBoundaryA[1];
	}

	if (mt76x0_get_tssi_report(pAd, bResetTssiInfo, &pAd->CurrTemperature) == TRUE) {
		if (get_temp_tx_alc_level(	
					pAd,
					enable_tx_alc,
					temp_ref,
					temp_minus_bdy,
					temp_plus_bdy,
					8, /* to do: make a definition */
					2,
					pAd->CurrTemperature,
					tx_alc_comp) == TRUE) 
		{
			UINT32 mac_val;
			CHAR last_delta_pwr, delta_pwr = 0;
			
			/* adjust compensation value by MP temperature readings (i.e., e2p[77h]) */
			if (pAd->CommonCfg.Channel <= 14) 
				delta_pwr = pAd->TxAgcCompensateG - pAd->mp_delta_pwr;
			else
				delta_pwr = pAd->TxAgcCompensateA - pAd->mp_delta_pwr; 
			
			RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &mac_val);
			/* 6-bit representation ==> 8-bit representation (2's complement) */
			pAd->DeltaPwrBeforeTempComp = (mac_val & 0x20) ? \
											((mac_val & 0x3F) | 0xC0): (mac_val & 0x3f);		

			last_delta_pwr = pAd->LastTempCompDeltaPwr;
			pAd->LastTempCompDeltaPwr = delta_pwr;
			pAd->DeltaPwrBeforeTempComp -= last_delta_pwr;
			delta_pwr += pAd->DeltaPwrBeforeTempComp;
			/* 8-bit representation ==> 6-bit representation (2's complement) */
			delta_pwr = (delta_pwr & 0x80) ? \
							((delta_pwr & 0x1f) | 0x20) : (delta_pwr & 0x3f);						
			/*	
				Write compensation value into TX_ALC_CFG_1, 
				delta_pwr (unit: 0.5dB) will be compensated by TX_ALC_CFG_1 
			*/     
			RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &mac_val);
			mac_val = (mac_val & (~0x3f)) | delta_pwr;
			RTMP_IO_WRITE32(pAd, TX_ALC_CFG_1, mac_val);

			DBGPRINT(RT_DEBUG_TRACE, 
				("%s - delta_pwr = %d, TssiCalibratedOffset = %d, TssiMpOffset = %d, 0x13B4 = 0x%08x, %s = %d, DeltaPwrBeforeTempComp = %d, LastTempCompDeltaPwr =%d\n",
				__FUNCTION__,
				pAd->LastTempCompDeltaPwr,
				pAd->TssiCalibratedOffset,
				pAd->mp_delta_pwr,
				mac_val,
				(pAd->CommonCfg.Channel <= 14) ? "TxAgcCompensateG" : "TxAgcCompensateA", 
				(pAd->CommonCfg.Channel <= 14) ? pAd->TxAgcCompensateG : pAd->TxAgcCompensateA, 
				pAd->DeltaPwrBeforeTempComp, 
				last_delta_pwr));	
		} else {
			DBGPRINT(RT_DEBUG_ERROR, ("%s(): failed to get the compensation level\n", __FUNCTION__)); 
		}
	}
}


static void adjust_temp_tx_alc_table(
	IN RTMP_ADAPTER *pAd,
	IN CHAR band,
	INOUT PCHAR temp_minus_bdy,
	INOUT PCHAR temp_plus_bdy,
	IN CHAR temp_reference)
{
	INT idx = 0;
	CHAR upper_bound = 127, lower_bound = -128;

	DBGPRINT(RT_DEBUG_OFF,("%s: upper_bound = 0x%02x (%d), lower_bound = 0x%02x (%d)\n",
		__FUNCTION__, upper_bound, upper_bound, lower_bound, lower_bound));
	
	DBGPRINT(RT_DEBUG_OFF,("*** %s: %s_temp_bdy_table[-7 .. +7] = %d %d %d %d %d %d %d * %d * %d %d %d %d %d %d %d, temp_reference=%d\n",
		__FUNCTION__,
		(band == A_BAND) ? "5G" : "2.4G",
		temp_minus_bdy[7], temp_minus_bdy[6], temp_minus_bdy[5],
		temp_minus_bdy[4], temp_minus_bdy[3], temp_minus_bdy[2], temp_minus_bdy[1],
		(band == A_BAND) ? (CHAR)pAd->TssiRefA : (CHAR)pAd->TssiRefG,
		temp_plus_bdy[1], temp_plus_bdy[2], temp_plus_bdy[3], temp_plus_bdy[4],
		temp_plus_bdy[5], temp_plus_bdy[6], temp_plus_bdy[7], temp_reference));

	for (idx = 0; idx < 8; idx++)
	{
		if ((lower_bound - temp_minus_bdy[idx]) <= temp_reference)
			temp_minus_bdy[idx] += temp_reference;
		else
			temp_minus_bdy[idx] = lower_bound;

		if ((upper_bound - temp_plus_bdy[idx]) >= temp_reference)
			temp_plus_bdy[idx] += temp_reference;
		else
			temp_plus_bdy[idx] = upper_bound;

		ASSERT(temp_minus_bdy[idx] >= lower_bound);
		ASSERT(temp_plus_bdy[idx] <= upper_bound);
	}

	if (band == A_BAND)
		pAd->TssiRefA = temp_minus_bdy[0];
	else
		pAd->TssiRefG = temp_minus_bdy[0];

	DBGPRINT(RT_DEBUG_OFF,("%s: %s_temp_bdy_table[-7 .. +7] = %d %d %d %d %d %d %d * %d * %d %d %d %d %d %d %d, temp_reference=%d\n",
		__FUNCTION__,
		(band == A_BAND) ? "5G" : "2.4G",
		temp_minus_bdy[7], temp_minus_bdy[6], temp_minus_bdy[5],
		temp_minus_bdy[4], temp_minus_bdy[3], temp_minus_bdy[2], temp_minus_bdy[1],
		(band == A_BAND) ? (CHAR)pAd->TssiRefA : (CHAR)pAd->TssiRefG,
		temp_plus_bdy[1], temp_plus_bdy[2], temp_plus_bdy[3], temp_plus_bdy[4],
		temp_plus_bdy[5], temp_plus_bdy[6], temp_plus_bdy[7], temp_reference));
}


static void adjust_mp_temp(
	IN RTMP_ADAPTER *pAd,
	IN PCHAR temp_minus_bdy,
	IN PCHAR temp_plus_bdy)
{
	EEPROM_TX_PWR_STRUC e2p_value;
	CHAR mp_temp, idx = 0, mp_offset = 0;
	
	RT28xx_EEPROM_READ16(pAd, 0x10C, e2p_value);
	mp_temp = e2p_value.field.Byte1;			

	if (mp_temp < temp_minus_bdy[1]) {
		/* 
			mp_temperature is larger than the reference value
			check for how large we need to adjust the Tx power 
		*/
		for (idx = 1; idx < 8; idx++)
		{
			if (mp_temp >= temp_minus_bdy[idx]) /* the range has been found */
				break;
		}

		/* 
			The index is the step we should decrease, 
			idx = 0 means there is no need to adjust the Tx power
		*/
		mp_offset = -(2 * (idx-1));
		pAd->mp_delta_pwr = mp_offset;
		DBGPRINT(RT_DEBUG_OFF, ("mp_temperature=0x%02x, step = -%d\n", mp_temp, idx-1));                    
	} else if (mp_temp > temp_plus_bdy[1]) {
		/* 
			mp_temperature is smaller than the reference value
			check for how large we need to adjust the Tx power 
		*/
		for (idx = 1; idx < 8; idx++)
		{
		    	if (mp_temp <= temp_plus_bdy[idx]) /* the range has been found */
	            		break;
		}

		/* 
			The index is the step we should increase, 
			idx = 0 means there is no need to adjust the Tx power
		*/
		mp_offset = 2 * (idx-1);
		pAd->mp_delta_pwr = mp_offset;
		DBGPRINT(RT_DEBUG_OFF, ("mp_temperature=0x%02x, step = +%d\n", mp_temp, idx-1));
	} else {
		pAd->mp_delta_pwr = 0;
		DBGPRINT(RT_DEBUG_OFF, ("mp_temperature=0x%02x, step = +%d\n", mp_temp, 0));
	}
}


BOOLEAN load_temp_tx_alc_table(
	IN RTMP_ADAPTER	*pAd,
	IN CHAR band,
	IN USHORT e2p_start_addr,
	IN USHORT e2p_end_addr,
	OUT	PUCHAR bdy_table,
	IN const INT start_idx,
	IN const UINT32 table_size)
{
	USHORT e2p_value;
	INT e2p_idx = 0, table_idx = 0;
	CHAR table_sign; /* +1 for plus table; -1 for minus table */

	table_sign = (e2p_start_addr < e2p_end_addr) ? 1 : (-1);

	if (start_idx < table_size) {
		table_idx = start_idx;
	} else {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): ERROR - incorrect start index (%d)\n",
			__FUNCTION__, start_idx));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_OFF, ("%s(): load %s %s table from eeprom 0x%x to 0x%x (start_idx = %d)\n",
		__FUNCTION__, 
		(band == A_BAND) ? "5G" : "2.4G",
		(table_sign == 1) ? "plus" : "minus",
		e2p_start_addr, e2p_end_addr, start_idx));

	for (e2p_idx = e2p_start_addr;
		 e2p_idx != (e2p_end_addr + (2*table_sign));
		 e2p_idx = e2p_idx + (2*table_sign))
	{
		if ((e2p_start_addr % 2) != 0) {
			e2p_start_addr--;
			e2p_idx--;
		}
		
		RT28xx_EEPROM_READ16(pAd, e2p_idx, e2p_value);

		if (e2p_idx == e2p_start_addr) {
			if (table_sign > 0) 
				bdy_table[table_idx++] = (UCHAR)((e2p_value >> 8) & 0xFF);
			else
 				bdy_table[table_idx++] = (UCHAR)(e2p_value & 0xFF);
		} else {
			if (table_sign > 0) {
				bdy_table[table_idx++] = (UCHAR)(e2p_value & 0xFF);
				bdy_table[table_idx++] = (UCHAR)((e2p_value >> 8) & 0xFF);
			} else {
				bdy_table[table_idx++] = (UCHAR)((e2p_value >> 8) & 0xFF);
				bdy_table[table_idx++] = (UCHAR)(e2p_value & 0xFF);
			}
		}
		
		if (table_idx >= table_size)
			break;
	}

	if (table_idx > table_size) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): WARNING - eeprom table may not be loaded completely\n", __FUNCTION__));
		return FALSE;
	} else {	
		for (table_idx = 0; table_idx < table_size; table_idx++)
			DBGPRINT(RT_DEBUG_TRACE, ("\tboundary_table[%d] = %3d (0x%02X)\n", 
				table_idx, (CHAR)bdy_table[table_idx], bdy_table[table_idx]));
	}

	return TRUE;
}


void mt76x0_temp_tx_alc_init(PRTMP_ADAPTER pAd)
{
	/* Initialize group settings of A band */
	pAd->TxAgcCompensateA = 0;
	pAd->TssiRefA = 0;
	pAd->TssiMinusBoundaryA[0][0] = 0;
	pAd->TssiMinusBoundaryA[1][0] = 0;
	pAd->TssiPlusBoundaryA[0][0] = 0;
	pAd->TssiPlusBoundaryA[1][0] = 0;

	adjust_temp_tx_alc_table(pAd, A_BAND, pAd->TssiMinusBoundaryA[0], 
								pAd->TssiPlusBoundaryA[0], pAd->TssiCalibratedOffset);
	adjust_mp_temp(pAd, pAd->TssiMinusBoundaryA[0], pAd->TssiPlusBoundaryA[0]);

	adjust_temp_tx_alc_table(pAd, A_BAND, pAd->TssiMinusBoundaryA[1], 
								pAd->TssiPlusBoundaryA[1], pAd->TssiCalibratedOffset);
	adjust_mp_temp(pAd, pAd->TssiMinusBoundaryA[1], pAd->TssiPlusBoundaryA[1]);
	
	if (IS_MT76x0U(pAd)) {
		/* Initialize group settings of G band */
		pAd->TxAgcCompensateG = 0;
		pAd->TssiRefG = 0;
		pAd->TssiMinusBoundaryG[0] = 0;
		pAd->TssiPlusBoundaryG[0] = 0;
		
		pAd->DeltaPwrBeforeTempComp = 0;
		pAd->LastTempCompDeltaPwr = 0;

		adjust_temp_tx_alc_table(pAd, BG_BAND, pAd->TssiMinusBoundaryG, 
									pAd->TssiPlusBoundaryG, pAd->TssiCalibratedOffset);
		adjust_mp_temp(pAd, pAd->TssiMinusBoundaryG, pAd->TssiPlusBoundaryG); 
	}
}
#endif /* RTMP_TEMPERATURE_COMPENSATION */


void mt76x0_read_tx_alc_info_from_eeprom(PRTMP_ADAPTER pAd)
{
	USHORT e2p_value = 0;

	if (IS_MT76x0(pAd)) {
		RT28xx_EEPROM_READ16(pAd, 0xD0, e2p_value);
		e2p_value = (e2p_value & 0xFF00) >> 8;
		DBGPRINT(RT_DEBUG_OFF, ("%s: EEPROM_MT76x0_TEMPERATURE_OFFSET (0xD1) = 0x%x\n", 
			__FUNCTION__, e2p_value));
		
		if ((e2p_value & 0xFF) == 0xFF) {
			pAd->chipCap.temp_offset = -10;
		} else {
			if ((e2p_value & 0x80) == 0x80) /* Negative number */
				e2p_value |= 0xFF00; 						
				
			pAd->chipCap.temp_offset = (SHORT)e2p_value;			
		}
		DBGPRINT(RT_DEBUG_OFF, ("%s: TemperatureOffset = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.temp_offset));
	}

#ifdef MT76x0_TSSI_CAL_COMPENSATION
	if (pAd->chipCap.bInternalTxALC)
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_2G_TARGET_POWER, e2p_value);
		pAd->chipCap.tssi_2G_target_power = e2p_value & 0x00ff;
		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_TARGET_POWER, e2p_value);
		pAd->chipCap.tssi_5G_target_power = e2p_value & 0x00ff;
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_2G_target_power = %d, tssi_5G_target_power = %d\n", 
			__FUNCTION__, pAd->chipCap.tssi_2G_target_power, pAd->chipCap.tssi_5G_target_power));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_2G_SLOPE_OFFSET, e2p_value);
		pAd->chipCap.tssi_slope_2G = e2p_value & 0x00ff;
		pAd->chipCap.tssi_offset_2G = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_2G = 0x%x, tssi_offset_2G = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.tssi_slope_2G, pAd->chipCap.tssi_offset_2G));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET, e2p_value);
		pAd->chipCap.tssi_slope_5G[0] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_offset_5G[0] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_1 = 0x%x, tssi_offset_5G_Group_1 = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.tssi_slope_5G[0], pAd->chipCap.tssi_offset_5G[0]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+2, e2p_value);
		pAd->chipCap.tssi_slope_5G[1] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_offset_5G[1] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_2 = 0x%x, tssi_offset_5G_Group_2 = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.tssi_slope_5G[1], pAd->chipCap.tssi_offset_5G[1]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+4, e2p_value);
		pAd->chipCap.tssi_slope_5G[2] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_offset_5G[2] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_3 = 0x%x, tssi_offset_5G_Group_3 = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.tssi_slope_5G[2], pAd->chipCap.tssi_offset_5G[2]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+6, e2p_value);
		pAd->chipCap.tssi_slope_5G[3] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_offset_5G[3] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_4 = 0x%x, tssi_offset_5G_Group_4 = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.tssi_slope_5G[3], pAd->chipCap.tssi_offset_5G[3]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+8, e2p_value);
		pAd->chipCap.tssi_slope_5G[4] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_offset_5G[4] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_5 = 0x%x, tssi_offset_5G_Group_5 = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.tssi_slope_5G[4], pAd->chipCap.tssi_offset_5G[4]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+10, e2p_value);
		pAd->chipCap.tssi_slope_5G[5] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_offset_5G[5] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_6 = 0x%x, tssi_offset_5G_Group_6 = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.tssi_slope_5G[5], pAd->chipCap.tssi_offset_5G[5]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+12, e2p_value);
		pAd->chipCap.tssi_slope_5G[6] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_offset_5G[6] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_7 = 0x%x, tssi_offset_5G_Group_7 = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.tssi_slope_5G[6], pAd->chipCap.tssi_offset_5G[6]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_SLOPE_OFFSET+14, e2p_value);
		pAd->chipCap.tssi_slope_5G[7] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_offset_5G[7] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_slope_5G_Group_8 = 0x%x, tssi_offset_5G_Group_8 = 0x%x\n", 
			__FUNCTION__, pAd->chipCap.tssi_slope_5G[7], pAd->chipCap.tssi_offset_5G[7]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_CHANNEL_BOUNDARY, e2p_value);
		pAd->chipCap.tssi_5G_channel_boundary[0] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_5G_channel_boundary[1] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_5G_channel_boundary_1 = %d, tssi_5G_channel_boundary_2 = %d\n", 
			__FUNCTION__, pAd->chipCap.tssi_5G_channel_boundary[0], pAd->chipCap.tssi_5G_channel_boundary[1]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_CHANNEL_BOUNDARY+2, e2p_value);
		pAd->chipCap.tssi_5G_channel_boundary[2] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_5G_channel_boundary[3] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_5G_channel_boundary_3 = %d, tssi_5G_channel_boundary_4 = %d\n", 
			__FUNCTION__, pAd->chipCap.tssi_5G_channel_boundary[2], pAd->chipCap.tssi_5G_channel_boundary[3]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_CHANNEL_BOUNDARY+4, e2p_value);
		pAd->chipCap.tssi_5G_channel_boundary[4] = e2p_value & 0x00ff;
		pAd->chipCap.tssi_5G_channel_boundary[5] = (e2p_value >> 8);
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_5G_channel_boundary_5 = %d, tssi_5G_channel_boundary_6 = %d\n", 
			__FUNCTION__, pAd->chipCap.tssi_5G_channel_boundary[4], pAd->chipCap.tssi_5G_channel_boundary[5]));

		RT28xx_EEPROM_READ16(pAd, EEPROM_MT76x0_5G_CHANNEL_BOUNDARY+6, e2p_value);
		pAd->chipCap.tssi_5G_channel_boundary[6] = e2p_value & 0x00ff;
		DBGPRINT(RT_DEBUG_OFF, ("%s: tssi_5G_channel_boundary_7 = %d\n", 
			__FUNCTION__, pAd->chipCap.tssi_5G_channel_boundary[6]));
	}
	else
#endif /* MT76x0_TSSI_CAL_COMPENSATION */
#ifdef RTMP_TEMPERATURE_COMPENSATION
	if (pAd->bAutoTxAgcG | pAd->bAutoTxAgcA) {	
		RT28xx_EEPROM_READ16(pAd, 0xD0, e2p_value);
		pAd->TssiCalibratedOffset = (e2p_value >> 8);
	
		/* 5G Tx power compensation channel boundary index */
		RT28xx_EEPROM_READ16(pAd, 0x10C, e2p_value);
		pAd->ChBndryIdx = (UCHAR)(e2p_value & 0xFF);
		DBGPRINT(RT_DEBUG_OFF, ("%s(): channel boundary index = %u, temp reference offset = %d\n",
			__FUNCTION__, pAd->ChBndryIdx, pAd->TssiCalibratedOffset));

		/* Load group#1 settings of A band */
		load_temp_tx_alc_table(
			pAd, A_BAND, 0xF6, 0xF0, pAd->TssiMinusBoundaryA[0], 1, sizeof(pAd->TssiMinusBoundaryA[0]));
		load_temp_tx_alc_table(
			pAd, A_BAND, 0xF7, 0xFD, pAd->TssiPlusBoundaryA[0], 1, sizeof(pAd->TssiPlusBoundaryA[0]));

		/* Load group#2 settings of A band */
		load_temp_tx_alc_table(
			pAd, A_BAND, 0x104, 0xFE, pAd->TssiMinusBoundaryA[1], 1, sizeof(pAd->TssiMinusBoundaryA[1]));
		load_temp_tx_alc_table(
			pAd, A_BAND, 0x105, 0x10B, pAd->TssiPlusBoundaryA[1], 1, sizeof(pAd->TssiPlusBoundaryA[1]));

		if (IS_MT76x0U(pAd)) {
			/* Load group settings of G band */
			load_temp_tx_alc_table(
				pAd, BG_BAND, 0x1B6, 0x1B0, pAd->TssiMinusBoundaryG, 1, sizeof(pAd->TssiMinusBoundaryG));
			load_temp_tx_alc_table(
				pAd, BG_BAND, 0x1B7, 0x1BD, pAd->TssiPlusBoundaryG, 1, sizeof(pAd->TssiPlusBoundaryG));
		}
		
		mt76x0_temp_tx_alc_init(pAd);
	}
	else
#endif /* RTMP_TEMPERATURE_COMPENSATION */
		DBGPRINT(RT_DEBUG_WARN, ("No TSSI or Temperature Tx ALC not enabled\n"));
}


void mt76x0_adjust_per_rate_pwr(RTMP_ADAPTER *ad)
{
	CONFIGURATION_OF_TX_POWER_CONTROL_OVER_MAC CfgOfTxPwrCtrlOverMAC = {0};	
	INT32 mac_idx = 0;

	DBGPRINT(RT_DEBUG_INFO,("-->%s\n", __FUNCTION__));

	/* Update per tx rate table */
	RTMP_CHIP_ASIC_TX_POWER_OFFSET_GET(ad, (PULONG)&CfgOfTxPwrCtrlOverMAC);

	/* Set new tx power per tx rate */
	for (mac_idx = 0; mac_idx < CfgOfTxPwrCtrlOverMAC.NumOfEntries; mac_idx++)
	{
		TX_POWER_CONTROL_OVER_MAC_ENTRY *pTxPwrEntry;
		pTxPwrEntry = &CfgOfTxPwrCtrlOverMAC.TxPwrCtrlOverMAC[mac_idx];
		
		if (pTxPwrEntry->RegisterValue != 0xFFFFFFFF) {
			RTMP_IO_WRITE32(ad, pTxPwrEntry->MACRegisterOffset, pTxPwrEntry->RegisterValue);
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("%s: after compensation - MAC offset = 0x%04X, CR value = 0x%08lX\n",
							__FUNCTION__, pTxPwrEntry->MACRegisterOffset, pTxPwrEntry->RegisterValue));
#endif /* RELEASE_EXCLUDE */
		}
	}

	/* Extra set MAC CRs to compensate tx power if any */
	RTMP_CHIP_ASIC_EXTRA_POWER_OVER_MAC(ad);
	
	DBGPRINT(RT_DEBUG_INFO, ("<--%s\n", __FUNCTION__));
}


static void mt76x0_show_pwr_info(RTMP_ADAPTER *ad)
{
	UINT32 data, index;
	UCHAR ch_init_pwr = 0;
	CHAR ch_delta_pwr = 0;
#ifdef SINGLE_SKU_V2
	CH_POWER *ch, *ch_temp;
#endif /* SINGLE_SKU_V2 */
	UINT32 tx_pwr_cfg_index[] = {TX_PWR_CFG_0, TX_PWR_CFG_1, TX_PWR_CFG_2,
								TX_PWR_CFG_3, TX_PWR_CFG_4, TX_PWR_CFG_5,
								TX_PWR_CFG_6, TX_PWR_CFG_7, TX_PWR_CFG_8,
								TX_PWR_CFG_9, 0};

	RTMP_IO_READ32(ad, TX_ALC_CFG_0, &data);
	ch_init_pwr = (UCHAR)(data & 0x3F);
	if ( ch_init_pwr & 0x20 )
		ch_init_pwr -= 64;
	DBGPRINT(RT_DEBUG_OFF, ("%s - 0x%08X = 0x%08X, ch_init_pwr = %d\n",
							__FUNCTION__, TX_ALC_CFG_0, data, ch_init_pwr));

	RTMP_IO_READ32(ad, TX_ALC_CFG_1, &data);
	ch_delta_pwr = (UCHAR)(data & 0x3F);
	if ( ch_delta_pwr & 0x20 )
		ch_delta_pwr -= 64;	
	DBGPRINT(RT_DEBUG_OFF, ("%s - 0x%08X = 0x%08X, ch_delta_pwr = %d\n",
							__FUNCTION__, TX_ALC_CFG_1, data, ch_delta_pwr));

	/* TX_PWR_CFG_0, MAC 0x1314 */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx20MPwrCfgGBand[0] = 0x%08lX\n", ad->Tx20MPwrCfgGBand[0]));
	/* TX_PWR_CFG_1, MAC 0x1318 */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx40MPwrCfgGBand[0] = 0x%08lX\n", ad->Tx40MPwrCfgGBand[0]));
	/* TX_PWR_CFG_2, MAC 0x131C */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx40MPwrCfgGBand[1] = 0x%08lX\n", ad->Tx40MPwrCfgGBand[1]));
	/* TX_PWR_CFG_3, MAC 0x1320 */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx40MPwrCfgGBand[2] = 0x%08lX\n", ad->Tx40MPwrCfgGBand[2]));
	/* TX_PWR_CFG_2, MAC 0x1324 */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx40MPwrCfgGBand[3] = 0x%08lX\n", ad->Tx40MPwrCfgGBand[3]));
	/* TX_PWR_CFG_0, MAC 0x1314 */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx20MPwrCfgABand[0] = 0x%08lX\n", ad->Tx20MPwrCfgABand[0]));
	/* TX_PWR_CFG_1, MAC 0x1318 */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx40MPwrCfgABand[0] = 0x%08lX\n", ad->Tx40MPwrCfgABand[0]));
	/* TX_PWR_CFG_2, MAC 0x131C */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx40MPwrCfgABand[1] = 0x%08lX\n", ad->Tx40MPwrCfgABand[1]));
	/* TX_PWR_CFG_3, MAC 0x1320 */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx40MPwrCfgABand[2] = 0x%08lX\n", ad->Tx40MPwrCfgABand[2]));
	/* TX_PWR_CFG_2, MAC 0x1324 */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx40MPwrCfgABand[3] = 0x%08lX\n", ad->Tx40MPwrCfgABand[3]));
#ifdef DOT11_VHT_AC
	/* TX_PWR_CFG_8, MAC 0x13D8 */
	DBGPRINT(RT_DEBUG_OFF, ("\t\t - Tx80MPwrCfgABand[0] = 0x%lX\n", ad->Tx80MPwrCfgABand[0]));
#endif /* DOT11_VHT_AC */
	
	DBGPRINT(RT_DEBUG_OFF, ("%s - Per-Rate Power:\n", __FUNCTION__));
	for (index = 0; tx_pwr_cfg_index[index] != 0; index++)
	{
		RTMP_IO_READ32(ad, tx_pwr_cfg_index[index], &data);
		DBGPRINT(RT_DEBUG_OFF, ("\t\t - 0x%x: 0x%08x\n", tx_pwr_cfg_index[index], data));
	}
	
#ifdef SINGLE_SKU_V2
	DlListForEachSafe(ch, ch_temp, &ad->SingleSkuPwrList, CH_POWER, List)
	{
		DBGPRINT(RT_DEBUG_OFF,
					("channel = %d, ch->channel = %d\n",
					ad->hw_cfg.cent_ch, ch->StartChannel));
		if (ad->hw_cfg.cent_ch == ch->StartChannel)
		{
#if 0 /* MT7610E doesn't have CCK mode. Need to consider CCK mode for MT7610U, MT7630E and MT7650E in the future. */			

			for ( i= 0 ; i < SINGLE_SKU_TABLE_CCK_LENGTH ; i++ )
			{
				if ( base_pwr > ch->PwrCCK[i] )
					base_pwr = ch->PwrCCK[i];
			}
#endif

			for (index = 0; index < SINGLE_SKU_TABLE_CCK_LENGTH; index++)
			{
				DBGPRINT(RT_DEBUG_OFF, 
							("\t\t ch->PwrCCK[%d] = %d, rate_delta:CCK[%d].MCS_Power = %d\n",
							index, ch->PwrCCK[index],
							index, ad->chipCap.rate_pwr_table.CCK[index].MCS_Power));
			}

			for (index = 0; index < SINGLE_SKU_TABLE_OFDM_LENGTH; index++)
			{
				DBGPRINT(RT_DEBUG_OFF, 
							("\t\t ch->PwrOFDM[%d] = %d, rate_delta:OFDM[%d].MCS_Power = %d\n",
							index, ch->PwrOFDM[index],
							index, ad->chipCap.rate_pwr_table.OFDM[index].MCS_Power));
			}

			for (index = 0; index < (SINGLE_SKU_TABLE_HT_LENGTH >> 1); index++)
			{
				DBGPRINT(RT_DEBUG_OFF,
							("\t\t ch->PwrHT20[%d] = %d, rate_delta :HT[%d].MCS_Power = %d\n",
							index, ch->PwrHT20[index],
							index, ad->chipCap.rate_pwr_table.HT[index].MCS_Power));
			}

			for (index = 0; index < (SINGLE_SKU_TABLE_VHT_LENGTH >> 1); index++)
			{
				DBGPRINT(RT_DEBUG_OFF,
							("\t\t ch->PwrVHT80[%d] = %d, rate_delta:VHT[%d].MCS_Power = %d\n",
							index, ch->PwrVHT80[index],
							index, ad->chipCap.rate_pwr_table.VHT[index].MCS_Power));
			}
			break;
		}
	}
#endif /* SINGLE_SKU_V2 */
}


/******************************* Command API *******************************/
INT Set_AntennaSelect_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT8 val = (UINT8)simple_strtol(arg, 0, 10);
	UINT32 reg_val = 0;

	/*
		0x2300[5] Default Antenna:
		0 for WIFI main antenna
		1 for WIFI aux  antenna

	*/
	RTMP_IO_READ32(pAd, AGC1_R0, &reg_val);
	reg_val &= ~(0x00000020);
	if (val != 0)
		reg_val |= 0x20;
	RTMP_IO_WRITE32(pAd, AGC1_R0, reg_val);

	RTMP_IO_READ32(pAd, AGC1_R0, &reg_val);

#if 0
	mt76x0_calibration(pAd, pAd->hw_cfg.cent_ch, FALSE, TRUE, TRUE);
#else
	CHIP_CALIBRATION(pAd, RXDCOC_CALIBRATION, 1);
#endif
	DBGPRINT(RT_DEBUG_TRACE, ("Set_AntennaSelect_Proc:: AntennaSelect = %d (0x%08x=0x%08x)\n", val, AGC1_R0, reg_val));

	return TRUE;
}
/******************************* Command API end ***************************/

