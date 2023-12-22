#include "rt_config.h"

#include "mcu/mt7662_rom_patch.h"
#include "mcu/mt7662_firmware.h"

#ifdef RTMP_EFUSE_SUPPORT
#include "eeprom/mt76x2_e2p.h"
#endif /* RTMP_EFUSE_SUPPORT */

#define MT7662_EFUSE_CTRL	0x0024
static RTMP_REG_PAIR mt76x2_mac_cr_table[] = {
	{PBF_SYS_CTRL, 0x80c00},
	{RLT_PBF_CFG, 0x1efebcff},
	{FCE_PSE_CTRL, 0x1},
	{MAC_SYS_CTRL, 0x0C},
	{MAX_LEN_CFG, 0x003E3FFF},
	{AMPDU_MAX_LEN_20M1S_MCS0_7, 0xFFFFA844},
	{AMPDU_MAX_LEN_20M1S_MCS8_9, 0xFF},
	{XIFS_TIME_CFG, 0x33A4100A},
	{BKOFF_SLOT_CFG, 0x209},
	{TBTT_SYNC_CFG, 0x422010},
	{PWR_PIN_CFG, 0x0},
	{0x1238, 0x001700C8},
	{TX_SW_CFG0, 0x00101001},
	{TX_SW_CFG1, 0x00010000},
	{TX_SW_CFG2, 0x0},
	{TXOP_CTRL_CFG, 0x0000583F},
	{TX_RTS_CFG, 0x00092B20},
	{TX_TIMEOUT_CFG, 0x000A2290},
	{TX_RTY_CFG, 0x47D01F0F},
	{EXP_ACK_TIME, 0x002C00DC},
	{TX_PROT_CFG6, 0xE3F42004},
	{TX_PROT_CFG7, 0xE3F42084},
	{TX_PROT_CFG8, 0xE3F42104},
	{PIFS_TX_CFG, 0x00060FFF},
	{RX_FILTR_CFG, 0x00015F97},
	{LEGACY_BASIC_RATE, 0x0000017F},
	{HT_BASIC_RATE, 0x00008003},
	{0x150C, 0x00000002}, /* Enable TX length > 4095 bytes */
	{0x1608, 0x00000002},
	{0xa44,	0x0},
#if defined(CONFIG_TSO_SUPPORT) || defined(CONFIG_CSO_SUPPORT)
	{0x80c,	0x8},
#endif /* defined(CONFIG_TSO_SUPPORT) || defined(CONFIG_CSO_SUPPORT) */
#ifdef HDR_TRANS_SUPPORT
	{HEADER_TRANS_CTRL_REG, 0x2}, /* 0x1: TX, 0x2: RX */
	{TSO_CTRL, 0x7050},
	{0x148, 0x15000001},
#else
	{HEADER_TRANS_CTRL_REG, 0x0},
	{TSO_CTRL, 0x0},
#endif /* HDR_TRANS_SUPPORT */
	{AUX_CLK_CFG, 0x0},
	{DACCLK_EN_DLY_CFG, 0x0}, /* MAC dynamic control TX 960MHZ */
	{TX_ALC_CFG_4, 0x00000000},
	{TX_ALC_VGA3, 0x0},
	{TX_PWR_CFG_0, 0x3A3A3A3A},
	{TX_PWR_CFG_1, 0x3A3A3A3A},
	{TX_PWR_CFG_2, 0x3A3A3A3A},
	{TX_PWR_CFG_3, 0x3A3A3A3A},
	{TX_PWR_CFG_4, 0x3A3A3A3A},
	{TX_PWR_CFG_7, 0x3A3A3A3A},
	{TX_PWR_CFG_8, 0x3A},
	{TX_PWR_CFG_9, 0x3A},
	{MT7662_EFUSE_CTRL, 0xD000},
	{PER_PORT_PAUSE_ENABLE_CONTROL1, 0x0},
	{0x210, 0x94ff0000},
#ifdef TXBF_SUPPORT
	{TX_TXBF_CFG_0,		0x4004FC21},	/* Force MCS4 for sounding response*/
	{TX_TXBF_CFG_1,		0xFE23727F},	
	{TX_TXBF_CFG_2,		0xFFFFFFFF},	/* The explicit TxBF feedback is applied only when the value of (local TSF timer) - 
	                                                               (TSF timestamp of the feedback frame) is greater then or equal to 0xFFFFFFFF */
#endif
	{0x1478, 0x00000004}
};
static UCHAR mt76x2_mac_cr_nums = (sizeof(mt76x2_mac_cr_table) / sizeof(RTMP_REG_PAIR));

RTMP_REG_PAIR mt76x2_mac_g_band_cr_table[] = {
	{BB_PA_MODE_CFG0, 0x010055FF},
	{BB_PA_MODE_CFG1, 0x00550055},
	{RF_PA_MODE_CFG0, 0x010055FF},
	{RF_PA_MODE_CFG1, 0x00550055},
	{PAMODE_PWR_ADJ0, 0xF4000200},
	{PAMODE_PWR_ADJ1, 0xFA000200},
	{TX_ALC_CFG_2, 0x35160A00},
	{TX_ALC_CFG_3, 0x35160A06},
	{TX_ALC_CFG_4, 0x00000606},

};
UCHAR mt76x2_mac_g_band_cr_nums = (sizeof(mt76x2_mac_g_band_cr_table) / sizeof(RTMP_REG_PAIR));

RTMP_REG_PAIR mt76x2_mac_g_band_internal_pa_cr_table[] = {
	{TX0_RF_GAIN_CORR, 0x0F3C3C3C},
	{TX1_RF_GAIN_CORR, 0x0F3C3C3C},
	{TX0_BB_GAIN_ATTEN, 0x00000606},
};
UCHAR mt76x2_mac_g_band_internal_pa_cr_nums = (sizeof(mt76x2_mac_g_band_internal_pa_cr_table) / sizeof(RTMP_REG_PAIR));

RTMP_REG_PAIR mt76x2_mac_g_band_external_pa_cr_table[] = {
	{TX0_RF_GAIN_CORR, 0x3C3C023C},
	{TX1_RF_GAIN_CORR, 0x3C3C023C},
	{TX0_BB_GAIN_ATTEN, 0x00001818},
};
UCHAR mt76x2_mac_g_band_external_pa_cr_nums = (sizeof(mt76x2_mac_g_band_external_pa_cr_table) / sizeof(RTMP_REG_PAIR));

RTMP_REG_PAIR mt76x2_mac_g_band_external_pa_low_temp_cr_table[] = {
	{TX0_RF_GAIN_CORR, 0x3C3C023C},
	{TX1_RF_GAIN_CORR, 0x3C3C023C},
	{TX0_BB_GAIN_ATTEN, 0x00001F1F},
};
UCHAR mt76x2_mac_g_band_external_pa_low_temp_cr_nums = (sizeof(mt76x2_mac_g_band_external_pa_low_temp_cr_table) / sizeof(RTMP_REG_PAIR));

RTMP_REG_PAIR mt76x2_mac_a_band_cr_table[] = {
	{BB_PA_MODE_CFG0, 0x0000FFFF},
	{BB_PA_MODE_CFG1, 0x00FF00FF},
	{RF_PA_MODE_CFG0, 0x0000FFFF},
	{RF_PA_MODE_CFG1, 0x00FF00FF}, 
	{PAMODE_PWR_ADJ0, 0x00000000},
	{PAMODE_PWR_ADJ1, 0x00000000},
	{TX_ALC_CFG_2, 0x1B0F0400},
	{TX_ALC_CFG_3, 0x1B0F0476},
	{TX_ALC_CFG_4, 0x00000000},
};
UCHAR mt76x2_mac_a_band_cr_nums = (sizeof(mt76x2_mac_g_band_cr_table) / sizeof(RTMP_REG_PAIR));

RTMP_REG_PAIR mt76x2_mac_a_band_internal_pa_cr_table[] = {
	{TX0_RF_GAIN_CORR, 0x383C023C},
	{TX1_RF_GAIN_CORR, 0x24282E28},
	{TX0_BB_GAIN_ATTEN, 0x00000000},
};
UCHAR mt76x2_mac_a_band_internal_pa_cr_nums = (sizeof(mt76x2_mac_a_band_internal_pa_cr_table) / sizeof(RTMP_REG_PAIR));

RTMP_REG_PAIR mt76x2_mac_a_band_external_pa_cr_table[] = {
	{TX0_RF_GAIN_CORR, 0x3C3C023C},
	{TX1_RF_GAIN_CORR, 0x3C3C023C},
	{TX0_BB_GAIN_ATTEN, 0x00001818},
};
UCHAR mt76x2_mac_a_band_external_pa_cr_nums = (sizeof(mt76x2_mac_a_band_external_pa_cr_table) / sizeof(RTMP_REG_PAIR));

RTMP_REG_PAIR mt76x2_mac_a_band_external_pa_low_temp_cr_table[] = {
	{TX0_RF_GAIN_CORR, 0x3C3C023C},
	{TX1_RF_GAIN_CORR, 0x3C3C023C},
	{TX0_BB_GAIN_ATTEN, 0x00001F1F},
};
UCHAR mt76x2_mac_a_band_external_pa_low_temp_cr_nums = (sizeof(mt76x2_mac_a_band_external_pa_low_temp_cr_table) / sizeof(RTMP_REG_PAIR));

struct RF_INDEX_OFFSET mt76x2_rf_index_offset[] = {
	{0, 0x0000, 0x033c},
	{1, 0x0000, 0x033c}, 
};

#ifdef CONFIG_CALIBRATION_COLLECTION
void record_calibration_info(RTMP_ADAPTER *pAd, UINT32 cal_id)
{
	switch(cal_id)
	{
		case RXDCOC_CALIBRATION_7662:
			//RF0 214
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_214] = 0x214;
			mt76x2_rf_read(pAd, 0, 0x214, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_214]);
			//RF0 218
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_218] = 0x218;
			mt76x2_rf_read(pAd, 0, 0x218, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_218]);
			//RF0 21C
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_21C] = 0x21C;
			mt76x2_rf_read(pAd, 0, 0x21C, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_21C]);
			//RF0 220
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_220] = 0x220;
			mt76x2_rf_read(pAd, 0, 0x220, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_220]);
			//RF0 224
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_224] = 0x224;
			mt76x2_rf_read(pAd, 0, 0x224, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_224]);
			//RF0 228
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_228] = 0x228;
			mt76x2_rf_read(pAd, 0, 0x228, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_228]);
			//RF1 214
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_214] = 0x214;
			mt76x2_rf_read(pAd, 1, 0x214, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_214]);
			//RF1 218
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_218] = 0x218;
			mt76x2_rf_read(pAd, 1, 0x218, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_218]);
			//RF1 21C
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_21C] = 0x21C;
			mt76x2_rf_read(pAd, 1, 0x21C, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_21C]);
			//RF1 220
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_220] = 0x220;
			mt76x2_rf_read(pAd, 1, 0x220, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_220]);
			//RF1 224
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_224] = 0x224;
			mt76x2_rf_read(pAd, 1, 0x224, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_224]);
			//RF1 228
			pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_228] = 0x228;
			mt76x2_rf_read(pAd, 1, 0x228, &pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_228]);
			break;
		case RC_CALIBRATION_7662:
			//RF0 24C
			pAd->calibration_info[RC_CALIBRATION_7662].addr[RF0_24C] = 0x24C;
			mt76x2_rf_read(pAd, 0, 0x24C, &pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_24C]);	
			//RF0 250
			pAd->calibration_info[RC_CALIBRATION_7662].addr[RF0_250] = 0x250;
			mt76x2_rf_read(pAd, 0, 0x250, &pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_250]);
			//RF0 264
			pAd->calibration_info[RC_CALIBRATION_7662].addr[RF0_264] = 0x264;
			mt76x2_rf_read(pAd, 0, 0x264, &pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_264]);
			//RF0 278
			pAd->calibration_info[RC_CALIBRATION_7662].addr[RF0_278] = 0x278;
			mt76x2_rf_read(pAd, 0, 0x278, &pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_278]);
			//RF1 24C
			pAd->calibration_info[RC_CALIBRATION_7662].addr[RF1_24C] = 0x24C;
			mt76x2_rf_read(pAd, 1, 0x24C, &pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_24C]);
			//RF1 250
			pAd->calibration_info[RC_CALIBRATION_7662].addr[RF1_250] = 0x250;
			mt76x2_rf_read(pAd, 1, 0x250, &pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_250]);
			//RF1 264
			pAd->calibration_info[RC_CALIBRATION_7662].addr[RF1_264] = 0x264;
			mt76x2_rf_read(pAd, 1, 0x264, &pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_264]);
			//RF1 278
			pAd->calibration_info[RC_CALIBRATION_7662].addr[RF1_278] = 0x278;
			mt76x2_rf_read(pAd, 1, 0x278, &pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_278]);
			break;
		case LC_CALIBRATION_7662:
			//BBP 2774
			pAd->calibration_info[LC_CALIBRATION_7662].addr[BBP_2774] = 0x2774;
			RTMP_BBP_IO_READ32(pAd, 0x2774, &pAd->calibration_info[LC_CALIBRATION_7662].value[BBP_2774]);
			break;
		case TX_LOFT_CALIBRATION_7662:
			//BBP 2780
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2780] = 0x2780;
			RTMP_BBP_IO_READ32(pAd, 0x2780, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2780]);
			//BBP 2784
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2784] = 0x2784;
			RTMP_BBP_IO_READ32(pAd, 0x2784, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2784]);
			//BBP 2788
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2788] = 0x2788;
			RTMP_BBP_IO_READ32(pAd, 0x2788, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2788]);
			//BBP 278C
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_278C] = 0x278C;
			RTMP_BBP_IO_READ32(pAd, 0x278C, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_278C]);
			//BBP 2790
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2790] = 0x2790;
			RTMP_BBP_IO_READ32(pAd, 0x2790, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2790]);
			//BBP 2794
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2794] = 0x2794;
			RTMP_BBP_IO_READ32(pAd, 0x2794, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2794]);
			//BBP 27A8
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27A8] = 0x27A8;
			RTMP_BBP_IO_READ32(pAd, 0x27A8, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27A8]);
			//BBP 27AC
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27AC] = 0x27AC;
			RTMP_BBP_IO_READ32(pAd, 0x27AC, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27AC]);
			//BBP 27B0
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27B0] = 0x27B0;
			RTMP_BBP_IO_READ32(pAd, 0x27B0, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27B0]);
			//BBP 27B4
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27B4] = 0x27B4;
			RTMP_BBP_IO_READ32(pAd, 0x27B4, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27B4]);
			//BBP 27B8
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27B8] = 0x27B8;
			RTMP_BBP_IO_READ32(pAd, 0x27B8, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27B8]);
			//BBP 27BC	
			pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27BC] = 0x27BC;
			RTMP_BBP_IO_READ32(pAd, 0x27BC, &pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27BC]);
			break;
		case TXIQ_CALIBRATION_7662:
			//BBP 2798
			pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_2798] = 0x2798;
			RTMP_BBP_IO_READ32(pAd, 0x2798, &pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_2798]);
			//BBP 279C
			pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_279C] = 0x279C;
			RTMP_BBP_IO_READ32(pAd, 0x279C, &pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_279C]);
			//BBP 27A0
			pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27A0] = 0x27A0;
			RTMP_BBP_IO_READ32(pAd, 0x27A0, &pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27A0]);
			//BBP 27A4
			pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27A4] = 0x27A4;
			RTMP_BBP_IO_READ32(pAd, 0x27A4, &pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27A4]);
			//BBP 27C0
			pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27C0] = 0x27C0;
			RTMP_BBP_IO_READ32(pAd, 0x27C0, &pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27C0]);
			//BBP 27C4
			pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27C4] = 0x27C4;
			RTMP_BBP_IO_READ32(pAd, 0x27C4, &pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27C4]);
			//BBP 27C8
			pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27C8] = 0x27C8;
			RTMP_BBP_IO_READ32(pAd, 0x27C8, &pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27C8]);
			//BBP 27CC
			pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27CC] = 0x27CC;
			RTMP_BBP_IO_READ32(pAd, 0x27CC, &pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27CC]);
			break;	
		case TSSI_CALIBRATION_7662:
			//BBP 208C
			pAd->calibration_info[TSSI_CALIBRATION_7662].addr[BBP_208C] = 0x208C;
			RTMP_BBP_IO_READ32(pAd, 0x208C, &pAd->calibration_info[TSSI_CALIBRATION_7662].value[BBP_208C]);	
			break;
		case DPD_CALIBRATION_7662:
			//BBP 2720
			pAd->calibration_info[DPD_CALIBRATION_7662].addr[BBP_2720] = 0x2720;
			RTMP_BBP_IO_READ32(pAd, 0x2720, &pAd->calibration_info[DPD_CALIBRATION_7662].value[BBP_2720]);	
			break;	
		case RXIQC_FI_CALIBRATION_7662:
			//BBP 2C60
			pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].addr[BBP_2C60] = 0x2C60;
			RTMP_BBP_IO_READ32(pAd, 0x2C60, &pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C60]);
			//BBP 2C64
			pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].addr[BBP_2C64] = 0x2C64;
			RTMP_BBP_IO_READ32(pAd, 0x2C64, &pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C64]);
			//BBP 2C70
			pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].addr[BBP_2C70] = 0x2C70;
			RTMP_BBP_IO_READ32(pAd, 0x2C70, &pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C70]);
			//BBP 2C74
			pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].addr[BBP_2C74] = 0x2C74;
			RTMP_BBP_IO_READ32(pAd, 0x2C74, &pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C74]);
			break;	
		case RXIQC_FD_CALIBRATION_7662:
			//BBP 2818
			pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_2818] = 0x2818;
			RTMP_BBP_IO_READ32(pAd, 0x2818, &pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2818]);
			//BBP 281C
			pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_281C] = 0x281C;
			RTMP_BBP_IO_READ32(pAd, 0x281C, &pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_281C]);
			//BBP 2820
			pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_2820] = 0x2820;
			RTMP_BBP_IO_READ32(pAd, 0x2820, &pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2820]);
			//BBP 2824
			pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_2824] = 0x2824;
			RTMP_BBP_IO_READ32(pAd, 0x2824, &pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2824]);
			//BBP 2828
			pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_2828] = 0x2828;
			RTMP_BBP_IO_READ32(pAd, 0x2828, &pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2828]);
			//BBP 282C
			pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_282C] = 0x282C;
			RTMP_BBP_IO_READ32(pAd, 0x282C, &pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_282C]);
			break;	
		default:
			break;
	}
}

void dump_calibration_info_for_robust(RTMP_ADAPTER *pAd, UINT32 cal_id)
{
	switch(cal_id) {
		case RXDCOC_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_214], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_218], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_21C], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_220], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_224], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_228], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_214], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_218], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_21C], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_220], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_224], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_228])); 
			break;
		case RC_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_24C], pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_250], pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_264], pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_278], pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_24C], pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_250], pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_264], pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_278]));
			break;
		case LC_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("0x%08x\n", pAd->calibration_info[LC_CALIBRATION_7662].value[BBP_2774])); 

			break;
		case TX_LOFT_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2780], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2784], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2788], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_278C], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2790], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2794], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27A8], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27AC], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27B0], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27B4], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27B8], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27BC]));
			break;
		case TXIQ_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_2798], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_279C], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27A0], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27A4], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27C0], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27C4], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27C8], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27CC]));
			break;
		case TSSI_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("0x%08x\n", pAd->calibration_info[TSSI_CALIBRATION_7662].value[BBP_208C])); 

			break;
		case DPD_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("0x%08x\n", pAd->calibration_info[DPD_CALIBRATION_7662].value[BBP_2720]));
			break;
		case RXIQC_FI_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("0x%08x 0x%08x 0x%08x 0x%08x\n", pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C60], pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C64], pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C70], pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C74]));
			break;
		case RXIQC_FD_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2818], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_281C], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2820], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2824], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2828], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_282C]));
			break; 
		default:
			DBGPRINT(RT_DEBUG_OFF, ("unknbow cal id = %d\n", cal_id));
			break;
	}

}

void do_calibrtion_info_for_robust(RTMP_ADAPTER *ad, UINT32 cal_id, UINT8 channel)
{
	UINT32 i;

	switch(cal_id) {
		case RXDCOC_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("\nRXDCOC_Calibration\n"));
			DBGPRINT(RT_DEBUG_OFF, ("RF0_214 RF0_218 RF0_21C RF0_220 RF0_224 RF0_228 RF1_214 RF1_218 RF1_21C RF1_220 RF1_224 RF1_228\n"));
			for (i = 0; i < 100; i++) {
				CHIP_CALIBRATION(ad, RXDCOC_CALIBRATION_7662, channel);
				record_calibration_info(ad, cal_id);
				dump_calibration_info_for_robust(ad, cal_id);
			}

			break;
		case RC_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("\nRC_Calibration\n"));
			DBGPRINT(RT_DEBUG_OFF, ("RF0_24C RF0_250 RF0_264 RF0_278 RF1_24C RF1_250 RF1_264 RF1_278\n"));
			for (i = 0; i < 100; i++) {
				CHIP_CALIBRATION(ad, RC_CALIBRATION_7662, 0x00);
				record_calibration_info(ad, cal_id);
				dump_calibration_info_for_robust(ad, cal_id);
			}
			break;
		case LC_CALIBRATION_7662:
			if (channel > 14) {
				DBGPRINT(RT_DEBUG_OFF, ("\nLC_Calibration\n"));
				DBGPRINT(RT_DEBUG_OFF, ("BBP2774\n"));
				for (i = 0; i < 100; i++) {
					CHIP_CALIBRATION(ad, LC_CALIBRATION_7662, 0x00);
					record_calibration_info(ad, cal_id);
					dump_calibration_info_for_robust(ad, cal_id);
				}
			}
			break;
		case TX_LOFT_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("\nTXLOFT_Calibration\n"));
			DBGPRINT(RT_DEBUG_OFF, ("BBP2780 BBP2784 BBP2788 BBP278C BBP2790 BBP2794 BBP27A8 BBP27AC BBP27B0 BBP27B4 BBP27B8 BBP27BC\n"));
			for (i = 0; i < 100; i++) {
				if (channel > 14) {
					CHIP_CALIBRATION(ad, TX_LOFT_CALIBRATION_7662, 0x1);
				} else {
					CHIP_CALIBRATION(ad, TX_LOFT_CALIBRATION_7662, 0x0);
				}

				record_calibration_info(ad, cal_id);
				dump_calibration_info_for_robust(ad, cal_id);
			}
			break;
		case TXIQ_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("\nTXIQ_Calibration\n"));
			DBGPRINT(RT_DEBUG_OFF, ("BBP2798 BBP279C BBP27A0 BBP27A4 BBP27C0 BBP27C4 BBP27C8 BBP27CC\n"));
			for (i = 0; i < 100; i++) {
				if (channel > 14) {
					CHIP_CALIBRATION(ad, TXIQ_CALIBRATION_7662, 0x1);
				} else {
					CHIP_CALIBRATION(ad, TXIQ_CALIBRATION_7662, 0x0);
				}

				record_calibration_info(ad, cal_id);
				dump_calibration_info_for_robust(ad, cal_id);
			}
			break;
		case TSSI_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("\nTSSI_Calibration\n"));
			DBGPRINT(RT_DEBUG_OFF, ("BBP208C\n"));
			for (i = 0; i < 100; i++) {
				mt76x2_tssi_calibration(ad, channel);
				record_calibration_info(ad, cal_id);
				dump_calibration_info_for_robust(ad, cal_id);
			}
			break;
		case DPD_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("\nDPD_Calibration\n"));
			DBGPRINT(RT_DEBUG_OFF, ("BBP2720\n"));
			for (i = 0; i < 100; i++) {
				CHIP_CALIBRATION(ad, DPD_CALIBRATION_7662, channel);
				record_calibration_info(ad, cal_id);
				dump_calibration_info_for_robust(ad, cal_id);
			}
			break;
		case RXIQC_FI_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("\nRXIQC_FI_Calibration\n"));
			DBGPRINT(RT_DEBUG_OFF, ("BBP2C60 BBP2C64 BBP2C70 BBP2C74\n"));
			for (i = 0; i < 100; i++) {
				if (channel > 14) {
					CHIP_CALIBRATION(ad, RXIQC_FI_CALIBRATION_7662, 0x1);
				} else {
					CHIP_CALIBRATION(ad, RXIQC_FI_CALIBRATION_7662, 0x0);
				}
				record_calibration_info(ad, cal_id);
				dump_calibration_info_for_robust(ad, cal_id);
			}
			break;
		case RXIQC_FD_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_OFF, ("\nnot supoort RXIQC_FD_CALIBRATION_7662 yet\n"));
			break; 
		default:
			DBGPRINT(RT_DEBUG_OFF, ("\nunknbow cal id = %d\n", cal_id));
			break;
	}
}

void dump_calibration_info(RTMP_ADAPTER *pAd, UINT32 cal_id)
{
	switch(cal_id)
	{
		case RXDCOC_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_ERROR, ("\nRXDCOC_CALIBRATION_7662\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_214], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_214]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_218], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_218]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_21C], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_21C]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_220], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_220]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_224], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_224]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF0_228], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF0_228]));			
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_214], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_214]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_218], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_218]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_21C], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_21C]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_220], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_220]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_224], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_224]));	
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RXDCOC_CALIBRATION_7662].addr[RF1_228], pAd->calibration_info[RXDCOC_CALIBRATION_7662].value[RF1_228]));			
			break;
		case RC_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_ERROR, ("\nRC_CALIBRATION_7662\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RC_CALIBRATION_7662].addr[RF0_24C], pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_24C]));
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RC_CALIBRATION_7662].addr[RF0_250], pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_250]));
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RC_CALIBRATION_7662].addr[RF0_264], pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_264]));
			DBGPRINT(RT_DEBUG_ERROR, ("RF0[0x%04x]=0x%08x\n", pAd->calibration_info[RC_CALIBRATION_7662].addr[RF0_278], pAd->calibration_info[RC_CALIBRATION_7662].value[RF0_278]));
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RC_CALIBRATION_7662].addr[RF1_24C], pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_24C]));
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RC_CALIBRATION_7662].addr[RF1_250], pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_250]));
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RC_CALIBRATION_7662].addr[RF1_264], pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_264]));
			DBGPRINT(RT_DEBUG_ERROR, ("RF1[0x%04x]=0x%08x\n", pAd->calibration_info[RC_CALIBRATION_7662].addr[RF1_278], pAd->calibration_info[RC_CALIBRATION_7662].value[RF1_278]));
			break;
		case LC_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_ERROR, ("\nLC_CALIBRATION_7662\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[LC_CALIBRATION_7662].addr[BBP_2774], pAd->calibration_info[LC_CALIBRATION_7662].value[BBP_2774]));			
			break;
		case TX_LOFT_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_ERROR, ("\nTX_LOFT_CALIBRATION_7662\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2780], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2780]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2784], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2784]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2788], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2788]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_278C], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_278C]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2790], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2790]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_2794], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_2794]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27A8], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27A8]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27AC], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27AC]));	
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27B0], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27B0]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27B4], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27B4]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27B8], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27B8]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TX_LOFT_CALIBRATION_7662].addr[BBP_27BC], pAd->calibration_info[TX_LOFT_CALIBRATION_7662].value[BBP_27BC]));	
			break;
		case TXIQ_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_ERROR, ("\nTXIQ_CALIBRATION_7662\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_2798], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_2798]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_279C], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_279C]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27A0], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27A0]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27A4], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27A4]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27C0], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27C0]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27C4], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27C4]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27C8], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27C8]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TXIQ_CALIBRATION_7662].addr[BBP_27CC], pAd->calibration_info[TXIQ_CALIBRATION_7662].value[BBP_27CC]));	
			break;
		case TSSI_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_ERROR, ("\nTSSI_CALIBRATION_7662\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[TSSI_CALIBRATION_7662].addr[BBP_208C], pAd->calibration_info[TSSI_CALIBRATION_7662].value[BBP_208C]));			
			break;
		case DPD_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_ERROR, ("\nDPD_CALIBRATION_7662\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[DPD_CALIBRATION_7662].addr[BBP_2720], pAd->calibration_info[DPD_CALIBRATION_7662].value[BBP_2720]));			
			break;
		case RXIQC_FI_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_ERROR, ("\nRXIQC_FI_CALIBRATION_7662\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].addr[BBP_2C60], pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C60]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].addr[BBP_2C64], pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C64]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].addr[BBP_2C70], pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C70]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].addr[BBP_2C74], pAd->calibration_info[RXIQC_FI_CALIBRATION_7662].value[BBP_2C74]));			
			break;
		case RXIQC_FD_CALIBRATION_7662:
			DBGPRINT(RT_DEBUG_ERROR, ("\nRXIQC_FD_CALIBRATION_7662\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_2818], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2818]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_281C], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_281C]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_2820], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2820]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_2824], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2824]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_2828], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_2828]));			
			DBGPRINT(RT_DEBUG_ERROR, ("BBP[0x%04x]=0x%08x\n", pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].addr[BBP_282C], pAd->calibration_info[RXIQC_FD_CALIBRATION_7662].value[BBP_282C]));			
			break;
		default:
			break;
	}
}
#endif

static VOID mt76x2_bbp_adjust(RTMP_ADAPTER *pAd)
{
	static char *ext_str[]={"extNone", "extAbove", "", "extBelow"};
	UCHAR rf_bw, ext_ch;

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

	DBGPRINT(RT_DEBUG_OFF, ("%s():rf_bw=%d, ext_ch=%d, PrimCh=%d, HT-CentCh=%d, VHT-CentCh=%d\n",
				__FUNCTION__, rf_bw, ext_ch, pAd->CommonCfg.Channel,
				pAd->CommonCfg.CentralChannel, pAd->CommonCfg.vht_cent_ch));
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

char get_chl_grp(UINT8 channel)
{
	char chl_grp = A_BAND_GRP0_CHL;

	if (channel >= 184 && channel <= 196)
		chl_grp = A_BAND_GRP0_CHL;
	else if (channel >= 36 && channel <= 48)
		chl_grp = A_BAND_GRP1_CHL;
	else if (channel >= 52 && channel <= 64)
		chl_grp = A_BAND_GRP2_CHL;
	else if (channel >= 98 && channel <= 114)
		chl_grp = A_BAND_GRP3_CHL;
	else if (channel >= 116 && channel <= 144)
		chl_grp = A_BAND_GRP4_CHL;
	else if (channel >= 149 && channel <= 165)
		chl_grp = A_BAND_GRP5_CHL;
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s:illegal channel (%d)\n", __FUNCTION__, channel));

	return chl_grp;
}

void mt76x2_adjust_per_rate_pwr_delta(RTMP_ADAPTER *ad, UINT8 channel)
{
	UINT32 value;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	unsigned int band;
	
	if (channel > 14)
		band = _A_BAND;
	else
		band = _G_BAND;

	RTMP_IO_READ32(ad, TX_PWR_CFG_0, &value);
	value &= ~TX_PWR_CCK_1_2_MASK;

	value |= TX_PWR_CCK_1_2(cap->tx_pwr_cck_1_2);

	value &= ~TX_PWR_CCK_5_11_MASK;
	value |= TX_PWR_CCK_5_11(cap->tx_pwr_cck_5_11);

	value &= ~TX_PWR_OFDM_6_9_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_OFDM_6_9(cap->tx_pwr_g_band_ofdm_6_9);
	else
		value |= TX_PWR_OFDM_6_9(cap->tx_pwr_a_band_ofdm_6_9);

	value &= ~TX_PWR_OFDM_12_18_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_OFDM_12_18(cap->tx_pwr_g_band_ofdm_12_18);
	else
		value |= TX_PWR_OFDM_12_18(cap->tx_pwr_a_band_ofdm_12_18);
	RTMP_IO_WRITE32(ad, TX_PWR_CFG_0, value);

	RTMP_IO_READ32(ad, TX_PWR_CFG_1, &value);
	value &= ~TX_PWR_OFDM_24_36_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_OFDM_24_36(cap->tx_pwr_g_band_ofdm_24_36);
	else
		value |= TX_PWR_OFDM_24_36(cap->tx_pwr_a_band_ofdm_24_36);

	value &= ~TX_PWR_OFDM_48_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_OFDM_48(cap->tx_pwr_g_band_ofdm_48_54);
	else
		value |= TX_PWR_OFDM_48(cap->tx_pwr_a_band_ofdm_48_54);

	value &= ~TX_PWR_HT_VHT_1SS_MCS_0_1_MASK;
	value |= TX_PWR_HT_VHT_1SS_MCS_0_1(cap->tx_pwr_ht_mcs_0_1);

	value &= ~TX_PWR_HT_VHT_1SS_MCS_2_3_MASK;
	value |= TX_PWR_HT_VHT_1SS_MCS_2_3(cap->tx_pwr_ht_mcs_2_3);
	RTMP_IO_WRITE32(ad, TX_PWR_CFG_1, value);

	RTMP_IO_READ32(ad, TX_PWR_CFG_2, &value);
	value &= ~TX_PWR_HT_VHT_1SS_MCS_4_5_MASK;
	value |= TX_PWR_HT_VHT_1SS_MCS_4_5(cap->tx_pwr_ht_mcs_4_5);

	value &= ~TX_PWR_HT_VHT_1SS_MCS_6_MASK;
	value |= TX_PWR_HT_VHT_1SS_MCS_6(cap->tx_pwr_ht_mcs_6_7);

	value &= ~TX_PWR_HT_MCS_8_9_VHT_2SS_0_1_MASK;
	value |= TX_PWR_HT_MCS_8_9_VHT_2SS_0_1(cap->tx_pwr_ht_mcs_8_9);

	value &= ~TX_PWR_HT_MCS_10_11_VHT_2SS_MCS_2_3_MASK;
	value |= TX_PWR_HT_MCS_10_11_VHT_2SS_MCS_2_3(cap->tx_pwr_ht_mcs_10_11);
	RTMP_IO_WRITE32(ad, TX_PWR_CFG_2, value);

	RTMP_IO_READ32(ad, TX_PWR_CFG_3, &value);
	value &= ~TX_PWR_HT_MCS_12_13_VHT_2SS_MCS_4_5_MASK;
	value |= TX_PWR_HT_MCS_12_13_VHT_2SS_MCS_4_5(cap->tx_pwr_ht_mcs_12_13);

	value &= ~TX_PWR_HT_MCS_14_VHT_2SS_MCS_6_MASK;
	value |= TX_PWR_HT_MCS_14_VHT_2SS_MCS_6(cap->tx_pwr_ht_mcs_14_15);

	value &= ~TX_PWR_HT_VHT_STBC_MCS_0_1_MASK;
	value |= TX_PWR_HT_VHT_STBC_MCS_0_1(cap->tx_pwr_ht_mcs_0_1);

	value &= ~TX_PWR_HT_VHT_STBC_MCS_2_3_MASK;
	value |= TX_PWR_HT_VHT_STBC_MCS_2_3(cap->tx_pwr_ht_mcs_2_3);
	RTMP_IO_WRITE32(ad, TX_PWR_CFG_3, value);

	RTMP_IO_READ32(ad, TX_PWR_CFG_4, &value);
	value &= ~TX_PWR_HT_VHT_STBC_MCS_4_5_MASK;
	value |= TX_PWR_HT_VHT_STBC_MCS_4_5(cap->tx_pwr_ht_mcs_4_5);

	value &= ~TX_PWR_HT_VHT_STBC_MCS_6_MASK;
	value |= TX_PWR_HT_VHT_STBC_MCS_6(cap->tx_pwr_ht_mcs_6_7);
	RTMP_IO_WRITE32(ad, TX_PWR_CFG_4, value);

	RTMP_IO_READ32(ad, TX_PWR_CFG_7, &value);
	value &= ~TX_PWR_OFDM_54_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_OFDM_54(cap->tx_pwr_g_band_ofdm_48_54);
	else
		value |= TX_PWR_OFDM_54(cap->tx_pwr_a_band_ofdm_48_54);

	value &= ~TX_PWR_VHT_2SS_MCS_8_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_VHT_2SS_MCS_8(cap->tx_pwr_2g_vht_mcs_8_9);
	else
		value |= TX_PWR_VHT_2SS_MCS_8(cap->tx_pwr_5g_vht_mcs_8_9);

	value &= ~TX_PWR_HT_MCS_7_VHT_1SS_MCS_7_MASK;
	value |= TX_PWR_HT_MCS_7_VHT_1SS_MCS_7(cap->tx_pwr_ht_mcs_6_7);
	
	value &= ~TX_PWR_VHT_2SS_MCS_9_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_VHT_2SS_MCS_9(cap->tx_pwr_2g_vht_mcs_8_9);
	else
		value |= TX_PWR_VHT_2SS_MCS_9(cap->tx_pwr_5g_vht_mcs_8_9);

	RTMP_IO_WRITE32(ad, TX_PWR_CFG_7, value);

	RTMP_IO_READ32(ad, TX_PWR_CFG_8, &value);
	value &= ~TX_PWR_HT_MCS_15_VHT_2SS_MCS7_MASK;
	value |= TX_PWR_HT_MCS_15_VHT_2SS_MCS7(cap->tx_pwr_ht_mcs_14_15);

	value &= ~TX_PWR_VHT_1SS_MCS_8_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_VHT_1SS_MCS_8(cap->tx_pwr_2g_vht_mcs_8_9);
	else
		value |= TX_PWR_VHT_1SS_MCS_8(cap->tx_pwr_5g_vht_mcs_8_9);
	
	value &= ~TX_PWR_VHT_1SS_MCS_9_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_VHT_1SS_MCS_9(cap->tx_pwr_2g_vht_mcs_8_9);
	else
		value |= TX_PWR_VHT_1SS_MCS_9(cap->tx_pwr_5g_vht_mcs_8_9);
	
	RTMP_IO_WRITE32(ad, TX_PWR_CFG_8, value);

	RTMP_IO_READ32(ad, TX_PWR_CFG_9, &value);
	value &= ~TX_PWR_HT_VHT_STBC_MCS_7_MASK;
	value |= TX_PWR_HT_VHT_STBC_MCS_7(cap->tx_pwr_ht_mcs_6_7);

	value &= ~TX_PWR_VHT_STBC_MCS_8_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_VHT_STBC_MCS_8(cap->tx_pwr_2g_vht_mcs_8_9);
	else
		value |= TX_PWR_VHT_STBC_MCS_8(cap->tx_pwr_5g_vht_mcs_8_9);
	
	value &= ~TX_PWR_VHT_STBC_MCS_9_MASK;
	if (band == _G_BAND)
		value |= TX_PWR_VHT_STBC_MCS_9(cap->tx_pwr_2g_vht_mcs_8_9);
	else
		value |= TX_PWR_VHT_STBC_MCS_9(cap->tx_pwr_5g_vht_mcs_8_9);

	RTMP_IO_WRITE32(ad, TX_PWR_CFG_9, value);
}

static void mt76x2_tx_pwr_gain(RTMP_ADAPTER *ad, UINT8 channel, UINT8 bw)
{
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	CHAR tx_0_pwr, tx_1_pwr;
	UINT32 value;

	/* set 54Mbps target power */
	if (channel <= 14) {
		tx_0_pwr = cap->tx_0_target_pwr_g_band;
		tx_0_pwr += cap->tx_0_chl_pwr_delta_g_band[get_low_mid_hi_index(channel)];
		if (bw == BW_40)
			tx_0_pwr += cap->delta_tx_pwr_bw40_g_band;

		tx_1_pwr = cap->tx_1_target_pwr_g_band;
		tx_1_pwr += cap->tx_1_chl_pwr_delta_g_band[get_low_mid_hi_index(channel)];
		if (bw == BW_40)
			tx_1_pwr += cap->delta_tx_pwr_bw40_g_band;

	} else {
		tx_0_pwr = cap->tx_0_target_pwr_a_band[get_chl_grp(channel)];
		tx_0_pwr += cap->tx_0_chl_pwr_delta_a_band[get_chl_grp(channel)][get_low_mid_hi_index(channel)];

		if (bw == BW_40)
			tx_0_pwr += cap->delta_tx_pwr_bw40_a_band;
		else if (bw == BW_80)
			tx_0_pwr += cap->delta_tx_pwr_bw80;
		
		tx_1_pwr = cap->tx_1_target_pwr_a_band[get_chl_grp(channel)];
		tx_1_pwr += cap->tx_1_chl_pwr_delta_a_band[get_chl_grp(channel)][get_low_mid_hi_index(channel)];

		if (bw == BW_40)
			tx_1_pwr += cap->delta_tx_pwr_bw40_a_band;
		else if (bw == BW_80)
			tx_1_pwr += cap->delta_tx_pwr_bw80;
	}

	/* range 0~23.5 db */
	if (tx_0_pwr >= 0x2f)
		tx_0_pwr = 0x2f;

	if (tx_0_pwr < 0)
		tx_0_pwr = 0;

	if (tx_1_pwr >= 0x2f)
		tx_1_pwr = 0x2f;

	if (tx_1_pwr < 0)
		tx_1_pwr = 0;

	/* TX0 channel initial transmission gain setting */
	RTMP_IO_READ32(ad, TX_ALC_CFG_0, &value);
	value = value & (~TX_ALC_CFG_0_CH_INT_0_MASK);
	value |= TX_ALC_CFG_0_CH_INT_0(tx_0_pwr);
	//value |= TX_ALC_CFG_0_CH_INT_0(0x7);
	DBGPRINT(RT_DEBUG_INFO, ("tx_0_pwr = %d\n", tx_0_pwr));
	RTMP_IO_WRITE32(ad, TX_ALC_CFG_0, value);
	
	/* TX1 channel initial transmission gain setting */
	RTMP_IO_READ32(ad, TX_ALC_CFG_0, &value);
	value = value & (~TX_ALC_CFG_0_CH_INT_1_MASK);
	value |= TX_ALC_CFG_0_CH_INT_1(tx_1_pwr);
	//value |= TX_ALC_CFG_0_CH_INT_1(0x7);
	DBGPRINT(RT_DEBUG_INFO, ("tx_1_pwr = %d\n", tx_1_pwr));
	RTMP_IO_WRITE32(ad, TX_ALC_CFG_0, value);
}

#define EXT_CH_NONE  0x00
#define EXT_CH_ABOVE 0X01
#define EXT_CH_BELOW 0x03

static void mt76x2_switch_channel(RTMP_ADAPTER *ad, UINT8 channel, BOOLEAN scan)
{
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	unsigned int latch_band, band, bw, tx_rx_setting;
	UINT32 ret, i, value, value1, restore_value, loop = 0;
	UINT16 e2p_value;
	UCHAR bbp_ch_idx;
	BOOLEAN band_change = FALSE;

	UINT32 RegValue = 0;

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(ad)) {
		RTMP_SEM_EVENT_WAIT(&ad->hw_atomic, ret);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
			return STATUS_UNSUCCESSFUL;
		}
	}
#endif /* RTMP_MAC_USB */

#ifdef DOT11_VHT_AC
	if (ad->CommonCfg.BBPCurrentBW == BW_80) {
		bbp_ch_idx = vht_prim_ch_idx(channel, ad->CommonCfg.Channel);
	} else 
#endif /* DOT11_VHT_AC */
	if (ad->CommonCfg.BBPCurrentBW == BW_40) {
		if (ad->CommonCfg.CentralChannel > ad->CommonCfg.Channel)
			bbp_ch_idx = EXT_CH_ABOVE;
		else
			bbp_ch_idx = EXT_CH_BELOW;
	} else {
		bbp_ch_idx = EXT_CH_NONE;
	}
	
	RTMP_IO_READ32(ad, EXT_CCA_CFG, &RegValue);
	RegValue &= ~(0xFFF);
#ifdef DOT11_VHT_AC
	if (ad->CommonCfg.BBPCurrentBW == BW_80)
	{
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
	if (ad->CommonCfg.BBPCurrentBW == BW_40)
	{
		if (ad->CommonCfg.CentralChannel > ad->CommonCfg.Channel)
			RegValue |= 0x1e4;
		else
			RegValue |= 0x2e1;
	}
	else
	{
		RegValue |= 0x1e4;
	}
	RTMP_IO_WRITE32(ad, EXT_CCA_CFG, RegValue);

	/* determine channel flags */
	if (channel > 14)
		band = _A_BAND;
	else
		band = _G_BAND;
	
	if (!ad->MCUCtrl.power_on) {
		band_change = TRUE;
	} else {
		if (ad->LatchRfRegs.Channel > 14)
			latch_band = _A_BAND;
		else
			latch_band = _G_BAND;

		if (band != latch_band)
			band_change = TRUE;
		else
			band_change = FALSE;
	}
	
	if (ad->CommonCfg.BBPCurrentBW == BW_80)
		bw = 2;
	else if (ad->CommonCfg.BBPCurrentBW == BW_40)
		bw = 1;
	else
		bw = 0;

	if ((ad->CommonCfg.TxStream == 1) && (ad->CommonCfg.RxStream == 1))
		tx_rx_setting = 0x101;
	else if ((ad->CommonCfg.TxStream == 2) && (ad->CommonCfg.RxStream == 1))
		tx_rx_setting = 0x201;
	else if ((ad->CommonCfg.TxStream == 1) && (ad->CommonCfg.RxStream == 2))
		tx_rx_setting = 0x102;
	else if ((ad->CommonCfg.TxStream == 2) && (ad->CommonCfg.RxStream == 2))
		tx_rx_setting = 0x202;
	else 
		tx_rx_setting = 0x202;
		

#ifdef RTMP_PCI_SUPPORT
	/* mac setting per band */
	if (IS_PCI_INF(ad)) {
		if (band_change) {
			if (band == _G_BAND) {
				for(i = 0; i < mt76x2_mac_g_band_cr_nums; i++) {
					RTMP_IO_WRITE32(ad, mt76x2_mac_g_band_cr_table[i].Register,
									mt76x2_mac_g_band_cr_table[i].Value);
				}
				if ( ad->chipCap.PAType & INT_PA_2G ) {
					for(i = 0; i < mt76x2_mac_g_band_internal_pa_cr_nums; i++) {
						RTMP_IO_WRITE32(ad, mt76x2_mac_g_band_internal_pa_cr_table[i].Register,
										mt76x2_mac_g_band_internal_pa_cr_table[i].Value);
					}
				} else {
					for(i = 0; i < mt76x2_mac_g_band_external_pa_cr_nums; i++) {
						RTMP_IO_WRITE32(ad, mt76x2_mac_g_band_external_pa_cr_table[i].Register,
										mt76x2_mac_g_band_external_pa_cr_table[i].Value);
					}
				}
			} else {
				for(i = 0; i < mt76x2_mac_a_band_cr_nums; i++) {
					RTMP_IO_WRITE32(ad, mt76x2_mac_a_band_cr_table[i].Register,
										mt76x2_mac_a_band_cr_table[i].Value);
				}
				if ( ad->chipCap.PAType & INT_PA_5G ) {
					for(i = 0; i < mt76x2_mac_a_band_internal_pa_cr_nums; i++) {
						RTMP_IO_WRITE32(ad, mt76x2_mac_a_band_internal_pa_cr_table[i].Register,
										mt76x2_mac_a_band_internal_pa_cr_table[i].Value);
					}
				} else {
					for(i = 0; i < mt76x2_mac_a_band_external_pa_cr_nums; i++) {
						RTMP_IO_WRITE32(ad, mt76x2_mac_a_band_external_pa_cr_table[i].Register,
										mt76x2_mac_a_band_external_pa_cr_table[i].Value);
					}
				}
			}
		}
	}
#endif

#ifdef RTMP_USB_SUPPORT
	if (IS_USB_INF(ad)) {
		if (band_change) {
			if (band == _G_BAND) {
				RANDOM_WRITE(ad, mt76x2_mac_g_band_cr_table, mt76x2_mac_g_band_cr_nums);
				if ( ad->chipCap.PAType & INT_PA_2G ) {
					RANDOM_WRITE(ad, mt76x2_mac_g_band_internal_pa_cr_table, mt76x2_mac_g_band_internal_pa_cr_nums);
				} else {
					RANDOM_WRITE(ad, mt76x2_mac_g_band_external_pa_cr_table, mt76x2_mac_g_band_external_pa_cr_nums);
				}
			} else {
				RANDOM_WRITE(ad, mt76x2_mac_a_band_cr_table, mt76x2_mac_a_band_cr_nums);
				if ( ad->chipCap.PAType & INT_PA_5G ) {
					RANDOM_WRITE(ad, mt76x2_mac_a_band_internal_pa_cr_table, mt76x2_mac_a_band_internal_pa_cr_nums);
				} else {
					RANDOM_WRITE(ad, mt76x2_mac_a_band_external_pa_cr_table, mt76x2_mac_a_band_external_pa_cr_nums);
				}
			}
		}
	}
#endif

	/* Fine tune tx power ramp on time based on BBP Tx delay */
	if (isExternalPAMode(ad, channel))
	{
       if (bw == 0)
       		RTMP_IO_WRITE32(ad, TX_SW_CFG0, 0x00101101);
       else
            RTMP_IO_WRITE32(ad, TX_SW_CFG0, 0x000B0C01);		

		RTMP_IO_WRITE32(ad, TX_SW_CFG1, 0x00010200);
	}
	else
	{
		if (bw == 0)
			RTMP_IO_WRITE32(ad, TX_SW_CFG0, 0x00101001);
		else
			RTMP_IO_WRITE32(ad, TX_SW_CFG0, 0x000B0B01);

		RTMP_IO_WRITE32(ad, TX_SW_CFG1, 0x00010000);
	}

	/* tx pwr gain setting */
	mt76x2_tx_pwr_gain(ad, channel, bw);

	/* per-rate power delta */
	mt76x2_adjust_per_rate_pwr_delta(ad, channel);
			
	AndesRltSwitchChannel(ad, channel, scan, bw, tx_rx_setting, bbp_ch_idx);

	if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3)) {
		/* LDPC RX */
		RTMP_BBP_IO_READ32(ad, 0x2934, &value);
		value |= (1 << 10);
		RTMP_BBP_IO_WRITE32(ad, 0x2934, value);
	}

	/* fine tune PD threshold for channel mode E */
	RTMP_BBP_IO_WRITE32(ad, 0x2394, 0x1010161C);

	/* channel smoothing threshold */
	RTMP_BBP_IO_READ32(ad, 0x2948, &value);
	value &= ~(0x1);
	value |= (0x1);
	RTMP_BBP_IO_WRITE32(ad, 0x2948, value);
	
	RTMP_BBP_IO_WRITE32(ad, 0x294C, 0xB9CB9FF9);

	/* backup mac 1004 value */
	RTMP_IO_READ32(ad, 0x1004, &restore_value);
	
	/* Backup the original RTS retry count and then set to 0 */
	RTMP_IO_READ32(ad, 0x1344, &ad->rts_tx_retry_num);
	
	/* disable mac tx/rx */
	value = restore_value;
	value &= ~0xC;
	RTMP_IO_WRITE32(ad, 0x1004, value);

	/* set RTS retry count = 0 */	
	RTMP_IO_WRITE32(ad, 0x1344, 0x00092B00);

	/* wait mac 0x1200, bbp 0x2130 idle */
	do {
		RTMP_IO_READ32(ad, 0x1200, &value);
		value &= 0x1;
		RTMP_BBP_IO_READ32(ad, 0x2130, &value1);
		DBGPRINT(RT_DEBUG_INFO,("%s:: Wait until MAC 0x1200 bit0 and BBP 0x2130 become 0\n", __FUNCTION__));
		RtmpusecDelay(1);
		loop++;
	} while (((value != 0) || (value1 != 0)) && (loop < 300));

	if (loop >= 300) {
		DBGPRINT(RT_DEBUG_OFF, ("%s:: Wait until MAC 0x1200 bit0 and BBP 0x2130 become 0 > 300 times\n", __FUNCTION__));
	}
	
	if (!ad->MCUCtrl.power_on) {
		RT28xx_EEPROM_READ16(ad, BT_RCAL_RESULT, e2p_value);

		if ((e2p_value & 0xff) != 0xff) {
			DBGPRINT(RT_DEBUG_OFF, ("r-cal result = %d\n", e2p_value & 0xff));
			CHIP_CALIBRATION(ad, R_CALIBRATION_7662, 0x00);
		}
	}

	/* RXDCOC calibration */	
	CHIP_CALIBRATION(ad, RXDCOC_CALIBRATION_7662, channel);

	if (!ad->MCUCtrl.power_on) {
		/* RX LPF calibration */
		CHIP_CALIBRATION(ad, RC_CALIBRATION_7662, 0x00);
	}

#if 1
#ifdef RTMP_MAC_USB
	if (IS_USB_INF(ad)) {
		RTMP_SEM_EVENT_WAIT(&ad->tssi_lock, ret);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("tssi_lock get failed(ret=%d)\n", ret));
			return STATUS_UNSUCCESSFUL;
		}
	}
#endif /* RTMP_MAC_USB */

#ifdef RTMP_PCI_SUPPORT
	if(IS_PCI_INF(ad)) {
		NdisAcquireSpinLock(&ad->tssi_lock);
	}
#endif

	/* TSSI Clibration */
	if (!IS_DOT11_H_RADAR_STATE(ad, RD_SILENCE_MODE))
		mt76x2_tssi_calibration(ad, channel);

	/* enable TX/RX */
	RTMP_IO_WRITE32(ad, 0x1004, 0xc);

	/* Restore RTS retry count */
	RTMP_IO_WRITE32(ad, 0x1344, ad->rts_tx_retry_num);		
	
	if (!ad->MCUCtrl.power_on && ad->chipCap.tssi_enable) {
		RTMP_IO_READ32(ad, TX_ALC_CFG_1, &value);
		value = value & (~TX_ALC_CFG_1_TX0_TEMP_COMP_MASK);
		value |= TX_ALC_CFG_1_TX0_TEMP_COMP(0x38);
		RTMP_IO_WRITE32(ad, TX_ALC_CFG_1, value);
		DBGPRINT(RT_DEBUG_OFF, ("TX0 power compensation = 0x%x\n", value & 0x3f)); 
		RTMP_IO_READ32(ad, TX_ALC_CFG_2, &value);
		value = value & (~TX_ALC_CFG_2_TX1_TEMP_COMP_MASK);
		value |= TX_ALC_CFG_2_TX1_TEMP_COMP(0x38);
		RTMP_IO_WRITE32(ad, TX_ALC_CFG_2, value);
		DBGPRINT(RT_DEBUG_OFF, ("TX1 power compensation = 0x%x\n", value & 0x3f));
	}
	
#ifdef RTMP_PCI_SUPPORT
	if (IS_PCI_INF(ad)) {
		NdisReleaseSpinLock(&ad->tssi_lock);
	}
#endif

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(ad)) {
		RTMP_SEM_EVENT_UP(&ad->tssi_lock);
	}
#endif

#endif

	/* Channel latch */
	ad->LatchRfRegs.Channel = channel;
	ad->hw_cfg.lan_gain = GET_LNA_GAIN(ad);
	
	if (!ad->MCUCtrl.power_on)
		ad->MCUCtrl.power_on = TRUE;

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(ad)) {
		RTMP_SEM_EVENT_UP(&ad->hw_atomic);
	}
#endif
	
	//mt76x2_set_ed_cca(ad, TRUE);

#ifdef DYNAMIC_VGA_SUPPORT


	RTMP_BBP_IO_WRITE32(ad, AGC1_R61, 0xFF64A4E2); /* microwave's function initial gain */
	RTMP_BBP_IO_WRITE32(ad, AGC1_R7, 0x08081010); /* microwave's ED CCA threshold */
	RTMP_BBP_IO_WRITE32(ad, AGC1_R11, 0x00000404); /* microwave's ED CCA threshold */
	RTMP_BBP_IO_WRITE32(ad, AGC1_R2, 0x00007070); /* initial ED CCA threshold */
	RTMP_IO_WRITE32(ad, TXOP_CTRL_CFG, 0x04101B3F);
#endif /* DYNAMIC_VGA_SUPPORT */

	DBGPRINT(RT_DEBUG_OFF,
			("%s(): Switch to Ch#%d(%dT%dR), BBP_BW=%d, bbp_ch_idx=%d)\n",
			__FUNCTION__,
			channel,
			ad->CommonCfg.TxStream,
			ad->CommonCfg.RxStream,
			ad->CommonCfg.BBPCurrentBW,
			bbp_ch_idx));
}


void mt76x2_tssi_calibration(RTMP_ADAPTER *ad, UINT8 channel)
{
	/* TSSI Clibration */
	if (ad->chipCap.tssi_enable) {
		ad->chipCap.tssi_stage = TSSI_CAL_STAGE;
		if (channel > 14) {
			if (ad->chipCap.PAType == EXT_PA_2G_5G)
				CHIP_CALIBRATION(ad, TSSI_CALIBRATION_7662, 0x0101);
			else if (ad->chipCap.PAType == EXT_PA_5G_ONLY)
				CHIP_CALIBRATION(ad, TSSI_CALIBRATION_7662, 0x0101);
			else
				CHIP_CALIBRATION(ad, TSSI_CALIBRATION_7662, 0x0001); 
		} else {
			if (ad->chipCap.PAType == EXT_PA_2G_5G)
				CHIP_CALIBRATION(ad, TSSI_CALIBRATION_7662, 0x0100);
			else if ((ad->chipCap.PAType == EXT_PA_5G_ONLY) ||
					(ad->chipCap.PAType == INT_PA_2G_5G))
				CHIP_CALIBRATION(ad, TSSI_CALIBRATION_7662, 0x0000);
			else if (ad->chipCap.PAType == EXT_PA_2G_ONLY)
				CHIP_CALIBRATION(ad, TSSI_CALIBRATION_7662, 0x0100);
			 else
				DBGPRINT(RT_DEBUG_ERROR, ("illegal PA Type(%d)\n", ad->chipCap.PAType));
		}
		ad->chipCap.tssi_stage = TSSI_TRIGGER_STAGE;
	}
}


void mt76x2_tssi_compensation(RTMP_ADAPTER *ad, UINT8 channel)
{
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	ANDES_CALIBRATION_PARAM param;
	UINT32 pa_mode, tssi_slope_offset, value;
	UINT32 ret;
	
#ifdef RTMP_USB_SUPPORT
	if (IS_USB_INF(ad)) {
		RTMP_SEM_EVENT_WAIT(&ad->tssi_lock, ret);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("tssi_lock get failed(ret=%d)\n", ret));
			return STATUS_UNSUCCESSFUL;
		}
	}
#endif

#ifdef RTMP_PCI_SUPPORT
//	if (IS_PCI_INF(ad)) {
//		NdisAcquireSpinLock(&ad->tssi_lock);
//	}
#endif

	if (ad->chipCap.tssi_stage <= TSSI_CAL_STAGE)
		goto done;
	
	if (cap->tssi_stage == TSSI_TRIGGER_STAGE) {
		DBGPRINT(RT_DEBUG_INFO, ("%s:TSS_TRIGGER(channel = %d)\n", __FUNCTION__, channel));
		param.mt76x2_tssi_comp_param.pa_mode = (1 << 8);
		param.mt76x2_tssi_comp_param.tssi_slope_offset = 0;
		
		/* TSSI Trigger */
		if(ad->chipOps.Calibration != NULL)
			ad->chipOps.Calibration(ad, TSSI_COMPENSATION_7662, &param);
		else 
			goto done;

		cap->tssi_stage = TSSI_COMP_STAGE;

		goto done;
	}

	/* Check 0x2088[4] = 0 */
	RTMP_BBP_IO_READ32(ad, CORE_R34, &value);
	
	if ((value & (1 << 4)) == 0) {

		DBGPRINT(RT_DEBUG_INFO, ("%s:TSSI_COMP(channel = %d)\n", __FUNCTION__, channel));

		if (channel > 14) {
			if (ad->chipCap.PAType == EXT_PA_2G_5G)
				pa_mode = 1;
			else if (ad->chipCap.PAType == EXT_PA_5G_ONLY)
				pa_mode = 1;
			else
				pa_mode = 0;
		} else {
			if (ad->chipCap.PAType == EXT_PA_2G_5G)
				pa_mode = 1;
			else if ((ad->chipCap.PAType == EXT_PA_5G_ONLY) ||
					(ad->chipCap.PAType == INT_PA_2G_5G))
				pa_mode = 0;
			else if (ad->chipCap.PAType == EXT_PA_2G_ONLY)
				pa_mode = 1;
		} 
	
		if (channel <= 14) {
			tssi_slope_offset &= ~TSSI_PARAM2_SLOPE0_MASK;
			tssi_slope_offset |= TSSI_PARAM2_SLOPE0(cap->tssi_0_slope_g_band);
			tssi_slope_offset &= ~TSSI_PARAM2_SLOPE1_MASK;
			tssi_slope_offset |= TSSI_PARAM2_SLOPE1(cap->tssi_1_slope_g_band);
			tssi_slope_offset &= ~TSSI_PARAM2_OFFSET0_MASK;
			tssi_slope_offset |= TSSI_PARAM2_OFFSET0(cap->tssi_0_offset_g_band);
			tssi_slope_offset &= ~TSSI_PARAM2_OFFSET1_MASK;
			tssi_slope_offset |= TSSI_PARAM2_OFFSET1(cap->tssi_1_offset_g_band);
		} else {
			tssi_slope_offset &= ~TSSI_PARAM2_SLOPE0_MASK;
			tssi_slope_offset |= TSSI_PARAM2_SLOPE0(cap->tssi_0_slope_a_band[get_chl_grp(channel)]);
			tssi_slope_offset &= ~TSSI_PARAM2_SLOPE1_MASK;
			tssi_slope_offset |= TSSI_PARAM2_SLOPE1(cap->tssi_1_slope_a_band[get_chl_grp(channel)]);
			tssi_slope_offset &= ~TSSI_PARAM2_OFFSET0_MASK;
			tssi_slope_offset |= TSSI_PARAM2_OFFSET0(cap->tssi_0_offset_a_band[get_chl_grp(channel)]);
			tssi_slope_offset &= ~TSSI_PARAM2_OFFSET1_MASK;
			tssi_slope_offset |= TSSI_PARAM2_OFFSET1(cap->tssi_1_offset_a_band[get_chl_grp(channel)]);
		}

		param.mt76x2_tssi_comp_param.pa_mode = (pa_mode | ((0x1) << 9));
		param.mt76x2_tssi_comp_param.tssi_slope_offset = tssi_slope_offset;

		/* TSSI Compensation */
		if(ad->chipOps.Calibration != NULL)
			ad->chipOps.Calibration(ad, TSSI_COMPENSATION_7662, &param);

		cap->tssi_stage = TSSI_TRIGGER_STAGE;

		if (!ad->MCUCtrl.dpd_on) {
#ifdef RTMP_PCI_SUPPORT
			RtmpOsMsDelay(10);
#endif
			/* DPD Calibration */
			if ( (ad->chipCap.PAType== INT_PA_2G_5G) 
				|| ((ad->chipCap.PAType == INT_PA_5G) && ( channel > 14 ) )
				|| ((ad->chipCap.PAType == INT_PA_2G) && ( channel <= 14 ) )
			)
				CHIP_CALIBRATION(ad, DPD_CALIBRATION_7662, channel);
				ad->MCUCtrl.dpd_on = TRUE;
		}
	}
	
done:

#ifdef RTMP_PCI_SUPPORT
	//if (IS_PCI_INF(ad)) {
	//	NdisReleaseSpinLock(&ad->tssi_lock);
	//}
#endif

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(ad)) {
		RTMP_SEM_EVENT_UP(&ad->tssi_lock);
	}
#endif
}

void mt76x2_calibration(RTMP_ADAPTER *ad, UINT8 channel)
{
	UINT32 value, value1, restore_value, loop = 0;

	/* backup mac 1004 value */
	RTMP_IO_READ32(ad, 0x1004, &restore_value);
	
	/* Backup the original RTS retry count and then set to 0 */
	RTMP_IO_READ32(ad, 0x1344, &ad->rts_tx_retry_num);
	
	/* disable mac tx/rx */
	value = restore_value;
	value &= ~0xC;
	RTMP_IO_WRITE32(ad, 0x1004, value);

	/* set RTS retry count = 0 */	
	RTMP_IO_WRITE32(ad, 0x1344, 0x00092B00);

	/* wait mac 0x1200, bbp 0x2130 idle */
	do {
		RTMP_IO_READ32(ad, 0x1200, &value);
		value &= 0x1;
		RTMP_BBP_IO_READ32(ad, 0x2130, &value1);
		DBGPRINT(RT_DEBUG_INFO, ("%s:: Wait until MAC 0x1200 bit0 and BBP 0x2130 become 0\n", __FUNCTION__));
		RtmpusecDelay(1);
		loop++;
	} while (((value != 0) || (value1 != 0)) && (loop < 300));
	
	if (loop >= 300) {
		DBGPRINT(RT_DEBUG_OFF, ("%s:: Wait until MAC 0x1200 bit0 and BBP 0x2130 become 0 > 300 times\n", __FUNCTION__));
	}

	if ( IS_DOT11_H_RADAR_STATE(ad, RD_SILENCE_MODE))
	{
		DBGPRINT(RT_DEBUG_OFF,
			("%s():RDMode  is in Silent State, do not calibration.\n", __FUNCTION__));
		return;
	}
	DBGPRINT(RT_DEBUG_OFF, ("%s(channel = %d)\n", __FUNCTION__, channel));
	
	/* LC Calibration */
	if (channel > 14) {
		CHIP_CALIBRATION(ad, LC_CALIBRATION_7662, 0x00);
	}

	/* TX LOFT */
	if (channel > 14) {
		CHIP_CALIBRATION(ad, TX_LOFT_CALIBRATION_7662, 0x1);
	} else {
		CHIP_CALIBRATION(ad, TX_LOFT_CALIBRATION_7662, 0x0);
	}

	/* TXIQ Clibration */
	if (channel > 14) {
		CHIP_CALIBRATION(ad, TXIQ_CALIBRATION_7662, 0x1);
	} else {
		CHIP_CALIBRATION(ad, TXIQ_CALIBRATION_7662, 0x0);
	}

#if 0
	/* DPD Calibration */
	if ((ad->chipCap.PAType== INT_PA_2G_5G) 
		|| ((ad->chipCap.PAType == INT_PA_5G) && (channel > 14))
		|| ((ad->chipCap.PAType == INT_PA_2G) && (channel <= 14))) {
		CHIP_CALIBRATION(ad, DPD_CALIBRATION_7662, channel);
	}
#endif		
	
	/* RXIQC-FI */
	if (channel > 14) {
		CHIP_CALIBRATION(ad, RXIQC_FI_CALIBRATION_7662, 0x1);
	} else {
		CHIP_CALIBRATION(ad, RXIQC_FI_CALIBRATION_7662, 0x0);
	}

#if 0
	/* RXIQC-FD */
	if (channel > 14)
		CHIP_CALIBRATION(ad, RXIQC_FD_CALIBRATION_7662, 0x0);
#endif
#ifdef TXBF_SUPPORT
		/* Do a Divider Calibration and update BBP registers */
		//if (ad->CommonCfg.RegTransmitSetting.field.ITxBfEn)
		{
			ULONG stTimeChk0, stTimeChk1;
			UCHAR i;
			
			NdisGetSystemUpTime(&stTimeChk0);
			
			/* Disable TX Phase Compensation */
			RTMP_IO_READ32(ad, TXBE_R12, &value);
			RTMP_IO_WRITE32(ad, TXBE_R12, value & (~0x28));
	
			/* Clear Tx/Rx Phase compensated values */
			RTMP_IO_READ32(ad, CAL_R0, &value);
			value &= ~0x60;
	
			// Clear Tx phase
			RTMP_IO_WRITE32(ad, CAL_R0, value);
			RTMP_IO_WRITE32(ad, TXBE_R13, 0);
	
			// Clear Rx phase
			for (i=0; i<3; i++)
			{
				RTMP_IO_WRITE32(ad, CAL_R0, value | (i << 5));
				RTMP_IO_WRITE32(ad, RXFE_R3,  0);
			}		
	
			//ad->chipOps.fITxBfLNAPhaseCompensate(ad);
	
			/* Start to do the divider calibration */
			ad->chipOps.fITxBfDividerCalibration(ad, 3, 0, NULL);
	
			// Residual phase calculation
			//ITxBFPhaseCalibrationStartUp(ad, 0);
	
			NdisGetSystemUpTime(&stTimeChk1);
	
			DBGPRINT(RT_DEBUG_INFO,("%s : Divider calibration duration = %d ms\n", __FUNCTION__, (stTimeChk1 - stTimeChk0)*1000/OS_HZ));
		}
#endif /* TXBF_SUPPORT */	


	/* enable TX/RX */
	RTMP_IO_WRITE32(ad, 0x1004, restore_value);

	/* Restore RTS retry count */
	RTMP_IO_WRITE32(ad, 0x1344, ad->rts_tx_retry_num);		

#ifdef CONFIG_CALIBRATION_COLLECTION
	RtmpOsMsDelay(200);

	UCHAR i = 0;
	for (i = R_CALIBRATION_7662; i <= RXIQC_FD_CALIBRATION_7662; i++) 
	{
		switch(i)
		{
			case RXDCOC_CALIBRATION_7662:
				record_calibration_info(ad, RXDCOC_CALIBRATION_7662);
				break;
			case RC_CALIBRATION_7662:
				record_calibration_info(ad, RC_CALIBRATION_7662);
				break;
			case LC_CALIBRATION_7662:
				record_calibration_info(ad, LC_CALIBRATION_7662);
				break;
			case TX_LOFT_CALIBRATION_7662:
				record_calibration_info(ad, TX_LOFT_CALIBRATION_7662);
				break;
			case TXIQ_CALIBRATION_7662:
				record_calibration_info(ad, TXIQ_CALIBRATION_7662);
				break;
			case TSSI_CALIBRATION_7662:
				record_calibration_info(ad, TSSI_CALIBRATION_7662);
				break;
			case DPD_CALIBRATION_7662:
				record_calibration_info(ad, DPD_CALIBRATION_7662);
				break;		
			case RXIQC_FI_CALIBRATION_7662:
				record_calibration_info(ad, RXIQC_FI_CALIBRATION_7662);
				break;		
			case RXIQC_FD_CALIBRATION_7662:
				record_calibration_info(ad, RXIQC_FD_CALIBRATION_7662);
				break;			
			default:
				break;
		}
	}
#endif /* CONFIG_CALIBRATION_COLLECTION */
}

static void mt76x2_cal_test(RTMP_ADAPTER *ad, UINT32 type)
{
	UCHAR cent_ch;

#ifdef DOT11_VHT_AC
	if(ad->CommonCfg.BBPCurrentBW == BW_80)
		cent_ch = ad->CommonCfg.vht_cent_ch;
	else
#endif /* DOT11_VHT_AC */
		cent_ch = ad->CommonCfg.CentralChannel;

#ifdef CONFIG_CALIBRATION_COLLECTION
	if (type == CAL_ROBUST_TEST) {
		do_calibrtion_info_for_robust(ad, RXDCOC_CALIBRATION_7662, cent_ch);
		do_calibrtion_info_for_robust(ad, RC_CALIBRATION_7662, cent_ch);
		do_calibrtion_info_for_robust(ad, LC_CALIBRATION_7662, cent_ch);
		do_calibrtion_info_for_robust(ad, TX_LOFT_CALIBRATION_7662, cent_ch);
		do_calibrtion_info_for_robust(ad, TXIQ_CALIBRATION_7662, cent_ch);
		do_calibrtion_info_for_robust(ad, TSSI_CALIBRATION_7662, cent_ch);
		do_calibrtion_info_for_robust(ad, DPD_CALIBRATION_7662, cent_ch);
		do_calibrtion_info_for_robust(ad, RXIQC_FI_CALIBRATION_7662, cent_ch);
		do_calibrtion_info_for_robust(ad, RXIQC_FD_CALIBRATION_7662, cent_ch);
	}
#endif /* CONFIG_CALIBRATION_COLLECTION */
}

/*
 * Initialize FCE
 */
VOID init_fce(RTMP_ADAPTER *ad)
{
	L2_STUFFING_STRUC reg;

	reg.word = 0;

#ifdef RTMP_PCI_SUPPORT
	RTMP_IO_READ32(ad, FCE_L2_STUFF, &reg.word);
	reg.field.FS_WR_MPDU_LEN_EN = 0;
	RTMP_IO_WRITE32(ad, FCE_L2_STUFF, reg.word);
#endif

#ifdef RTMP_USB_SUPPORT
	RTMP_IO_READ32(ad, FCE_L2_STUFF, &reg.word);
	reg.field.FS_WR_MPDU_LEN_EN = 0;
	RTUSBWriteMACRegister(ad, FCE_L2_STUFF, reg.word, FALSE);
#endif
}

static void mt76x2_init_mac_cr(RTMP_ADAPTER *ad)
{
	UINT32 i;
	UINT32 value = 0;
	UINT16 e2p_value;

	/*
		Enable PBF and MAC clock
		SYS_CTRL[11:10] = 0x3
	*/
#ifdef RTMP_PCI_SUPPORT
	if (IS_PCI_INF(ad)) {
		for(i = 0; i < mt76x2_mac_cr_nums; i++) {
			RTMP_IO_WRITE32(ad, mt76x2_mac_cr_table[i].Register,
									mt76x2_mac_cr_table[i].Value);
		}
	}
#endif
			
#ifdef RTMP_USB_SUPPORT
	if (IS_USB_INF(ad)) {
		RANDOM_WRITE(ad, mt76x2_mac_cr_table, mt76x2_mac_cr_nums);
	}
#endif

#ifdef HDR_TRANS_TX_SUPPORT
	/*
 	 * Enable Header Translation TX
 	 */
	RTMP_IO_READ32(ad, HEADER_TRANS_CTRL_REG, &value);
	value |= 0x1; /* 0x1: TX, 0x2: RX */
	RTMP_IO_WRITE32(ad, HEADER_TRANS_CTRL_REG, value);
#endif /* HDR_TRANS_TX_SUPPORT */

	/*
 	 * Release BBP and MAC reset
 	 * MAC_SYS_CTRL[1:0] = 0x0
 	 */
	RTMP_IO_READ32(ad, MAC_SYS_CTRL, &value);
	value &= ~(0x3);
	RTMP_IO_WRITE32(ad, MAC_SYS_CTRL, value);

	if (IS_MT7612(ad))
	{
		/*
 		 * Disable COEX_EN
 		 */
		RTMP_IO_READ32(ad, COEXCFG0, &value);
		value &= 0xFFFFFFFE;
		RTMP_IO_WRITE32(ad, COEXCFG0, value);
	}

	/*
		Set 0x141C[15:12]=0xF
	*/
	RTMP_IO_READ32(ad, EXT_CCA_CFG, &value);
	value |= (0x0000F000);
	RTMP_IO_WRITE32(ad, EXT_CCA_CFG, value);


	/*
 	 * Set 0x13C0[31] = 0x0
 	 */ 
	RTMP_IO_READ32(ad, TX_ALC_CFG_4, &value);
	value &= ~WL_LOWGAIN_CH_EN;
	RTMP_IO_WRITE32(ad, TX_ALC_CFG_4, value);
	
	/*
 	 * Check crystal trim2 first 
 	 */ 
	RT28xx_EEPROM_READ16(ad, G_BAND_BANDEDGE_PWR_BACK_OFF, e2p_value);
	
	if (((e2p_value & 0xff) == 0x00) || ((e2p_value & 0xff) == 0xff))
	{
		/*
 		 * Compesate crystal trim1
 		 */
		RT28xx_EEPROM_READ16(ad, XTAL_TRIM1, e2p_value);

		/* crystal trim default value set to 0x14 */
		if (((e2p_value & 0xff) == 0x00) || ((e2p_value & 0xff) == 0xff))
			e2p_value = 0x14;

		/* Set crystal trim1 */
		read_reg(ad, 0x40, XO_CTRL5, &value);
		value &= 0xffff80ff;
		value |= ((e2p_value & XTAL_TRIM1_MASK) << 8);
		write_reg(ad, 0x40, XO_CTRL5, value);

		/* Enable */
		read_reg(ad, 0x40, XO_CTRL6, &value);
		value &= 0xffff80ff;
		value |= (0x7f << 8);
		write_reg(ad, 0x40, XO_CTRL6, value);
	} else {
		/* Set crystal trim2 */
		read_reg(ad, 0x40, XO_CTRL5, &value);
		value &= 0xffff80ff;
		value |= (e2p_value & XTAL_TRIM2_MASK);
		write_reg(ad, 0x40, XO_CTRL5, value);

		/* Enable */
		read_reg(ad, 0x40, XO_CTRL6, &value);
		value &= 0xffff80ff;
		value |= (0x7f << 8);
		write_reg(ad, 0x40, XO_CTRL6, value);
	}

	/* 
 	 * add 504, 50c value per ben kao suggestion for rx receivce packet, need to revise this bit 
     * only mt7662u do not this setting 
	 */
	if (IS_MT76x2E(ad) || IS_MT76x2U(ad)) {	
		RTMP_IO_WRITE32(ad, 0x504, 0x06000000);
		RTMP_IO_WRITE32(ad, 0x50c, 0x08800000);
		RtmpOsMsDelay(5);
		RTMP_IO_WRITE32(ad, 0x504, 0x0);
	}
	
	/* Decrease MAC OFDM SIFS from 16 to 14us */ 
	RTMP_IO_READ32(ad, XIFS_TIME_CFG, &value);
	value = value & (~XIFS_TIME_OFDM_SIFS_MASK);
	value |= XIFS_TIME_OFDM_SIFS(0x0e);
	RTMP_IO_WRITE32(ad, XIFS_TIME_CFG, value);

#if 1
	RTMP_IO_READ32(ad, BKOFF_SLOT_CFG, &value);
	value &= ~(BKOFF_SLOT_CFG_CC_DELAY_TIME_MASK);
	value |= BKOFF_SLOT_CFG_CC_DELAY_TIME(0x01);
	RTMP_IO_WRITE32(ad, BKOFF_SLOT_CFG, value);
#endif

	init_fce(ad);

#ifdef MCS_LUT_SUPPORT
	RTMP_IO_READ32(ad, TX_FBK_LIMIT, &value);
	if (RTMP_TEST_MORE_FLAG(ad, fASIC_CAP_MCS_LUT))
		value |= 0x40000;
	else
		value &= (~0x40000);
	RTMP_IO_WRITE32(ad, TX_FBK_LIMIT, value);
#endif /* MCS_LUT_SUPPORT */
}

static void mt76x2_init_rf_cr(RTMP_ADAPTER *ad)
{
	UINT16 value;

	AndesRltLoadCr(ad, RF_BBP_CR, 0, 0);
}

void mt76x2_get_agc_gain(RTMP_ADAPTER *ad)
{
	UCHAR val;
	USHORT val16;
	UINT32 bbp_val;

	RTMP_BBP_IO_READ32(ad, AGC1_R8, &bbp_val);
	val = ((bbp_val & (0x00007f00)) >> 8) & 0x7f;
	ad->CommonCfg.lna_vga_ctl.agc_vga_init_0 = val;
	val16 = ((bbp_val & (0xffff0000)) >> 16) & (0xffff);
	ad->CommonCfg.lna_vga_ctl.agc_0_vga_set1_2 = val16;
	DBGPRINT(RT_DEBUG_TRACE, ("initial vga value(chain0) = %x\n",  ad->CommonCfg.lna_vga_ctl.agc_vga_init_0));

	if (ad->CommonCfg.RxStream >= 2) {
		RTMP_BBP_IO_READ32(ad, AGC1_R9, &bbp_val);
		val = ((bbp_val & (0x00007f00)) >> 8) & 0x7f;
		ad->CommonCfg.lna_vga_ctl.agc_vga_init_1 = val;
		val16 = ((bbp_val & (0xffff0000)) >> 16) & (0xffff);
		ad->CommonCfg.lna_vga_ctl.agc_1_vga_set1_2 = val16;

		DBGPRINT(RT_DEBUG_TRACE, ("initial vga value(chain1) = %x\n",  ad->CommonCfg.lna_vga_ctl.agc_vga_init_1));
	}

	ad->CommonCfg.lna_vga_ctl.bDyncVgaEnable = TRUE;
}

int mt76x2_reinit_agc_gain(RTMP_ADAPTER *ad, UINT8 channel)
{
	UINT32 value0, value1;
	CHAR agc_vga0, agc_vga1;
	UINT8 chl_grp;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	
	RTMP_BBP_IO_READ32(ad, AGC1_R8, &value0);
	agc_vga0 = ((value0 & (0x00007f00)) >> 8) & 0x7f;
	
	RTMP_BBP_IO_READ32(ad, AGC1_R9, &value1);
	agc_vga1 = ((value1 & (0x00007f00)) >> 8) & 0x7f;

	DBGPRINT(RT_DEBUG_OFF, ("%s:original agc_vga0 = 0x%x, agc_vga1 = 0x%x\n", __FUNCTION__, agc_vga0, agc_vga1));

	if (channel > 14) {
		chl_grp = get_chl_grp(channel);
		switch (chl_grp) {
			case A_BAND_GRP0_CHL:
				agc_vga0 += cap->rf0_5g_grp0_rx_high_gain;
				agc_vga1 += cap->rf1_5g_grp0_rx_high_gain;
				break;
			case A_BAND_GRP1_CHL:
				agc_vga0 += cap->rf0_5g_grp1_rx_high_gain;
				agc_vga1 += cap->rf1_5g_grp1_rx_high_gain;
				break;
			case A_BAND_GRP2_CHL:
				agc_vga0 += cap->rf0_5g_grp2_rx_high_gain;
				agc_vga1 += cap->rf1_5g_grp2_rx_high_gain;
				break;
			case A_BAND_GRP3_CHL:
				agc_vga0 += cap->rf0_5g_grp3_rx_high_gain;
				agc_vga1 += cap->rf1_5g_grp3_rx_high_gain;
				break;
			case A_BAND_GRP4_CHL:
				agc_vga0 += cap->rf0_5g_grp4_rx_high_gain;
				agc_vga1 += cap->rf1_5g_grp4_rx_high_gain;
				break;
			case A_BAND_GRP5_CHL:
				agc_vga0 += cap->rf0_5g_grp5_rx_high_gain;
				agc_vga1 += cap->rf1_5g_grp5_rx_high_gain;
				break;
			default:
				DBGPRINT(RT_DEBUG_OFF, ("illegal channel group(%d)\n", chl_grp));
				break;
		}
	} else {
		agc_vga0 += cap->rf0_2g_rx_high_gain;
		agc_vga1 += cap->rf1_2g_rx_high_gain;
	}
	
	DBGPRINT(RT_DEBUG_OFF, ("%s:updated agc_vga0 = 0x%x, agc_vga1 = 0x%x\n", __FUNCTION__, agc_vga0, agc_vga1));
		
	value0 &= 0xffff80ff;
	value0 |= ((0x7f & agc_vga0) << 8);

	value1 &= 0xffff80ff;
	value1 |= ((0x7f & agc_vga1) << 8);

	RTMP_BBP_IO_WRITE32(ad, AGC1_R8, value0);
	RTMP_BBP_IO_WRITE32(ad, AGC1_R9, value1);

	return 0;
}

int mt76x2_reinit_hi_lna_gain(RTMP_ADAPTER *ad, UINT8 channel)
{
	UINT32 value0, value1;
	CHAR hi_lna0, hi_lna1;
	UINT8 chl_grp;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	
	RTMP_BBP_IO_READ32(ad, AGC1_R4, &value0);
	hi_lna0 = ((value0 & (0x003f0000)) >> 16) & 0x3f;
	
	RTMP_BBP_IO_READ32(ad, AGC1_R5, &value1);
	hi_lna1 = ((value1 & (0x003f0000)) >> 16) & 0x3f;

	DBGPRINT(RT_DEBUG_OFF, ("%s:original hi_lna0 = 0x%x, hi_lna1 = 0x%x\n", __FUNCTION__, hi_lna0, hi_lna1));

	if (channel > 14) {
		chl_grp = get_chl_grp(channel);
		switch (chl_grp) {
			case A_BAND_GRP0_CHL:
				hi_lna0 -= (cap->rf0_5g_grp0_rx_high_gain / 2);
				hi_lna1 -= (cap->rf1_5g_grp0_rx_high_gain / 2);
				break;
			case A_BAND_GRP1_CHL:
				hi_lna0 -= (cap->rf0_5g_grp1_rx_high_gain / 2);
				hi_lna1 -= (cap->rf1_5g_grp1_rx_high_gain / 2);
				break;
			case A_BAND_GRP2_CHL:
				hi_lna0 -= (cap->rf0_5g_grp2_rx_high_gain / 2);
				hi_lna1 -= (cap->rf1_5g_grp2_rx_high_gain / 2);
				break;
			case A_BAND_GRP3_CHL:
				hi_lna0 -= (cap->rf0_5g_grp3_rx_high_gain / 2);
				hi_lna1 -= (cap->rf1_5g_grp3_rx_high_gain / 2);
				break;
			case A_BAND_GRP4_CHL:
				hi_lna0 -= (cap->rf0_5g_grp4_rx_high_gain / 2);
				hi_lna1 -= (cap->rf1_5g_grp4_rx_high_gain / 2);
				break;
			case A_BAND_GRP5_CHL:
				hi_lna0 -= (cap->rf0_5g_grp5_rx_high_gain / 2);
				hi_lna1 -= (cap->rf1_5g_grp5_rx_high_gain / 2);
				break;
			default:
				DBGPRINT(RT_DEBUG_OFF, ("illegal channel group(%d)\n", chl_grp));
				break;
		}
	} else {
		hi_lna0 -= (cap->rf0_2g_rx_high_gain / 2);
		hi_lna1 -= (cap->rf1_2g_rx_high_gain / 2);
	}
	
	DBGPRINT(RT_DEBUG_OFF, ("%s:updated hi_lna0 = 0x%x, hi_lna1 = 0x%x\n", __FUNCTION__, hi_lna0, hi_lna1));
		
	value0 &= 0xffc0ffff;
	value0 |= ((0x3f & hi_lna0) << 16);

	value1 &= 0xffc0ffff;
	value1 |= ((0x3f & hi_lna1) << 16);

	RTMP_BBP_IO_WRITE32(ad, AGC1_R4, value0);
	RTMP_BBP_IO_WRITE32(ad, AGC1_R5, value1);

	return 0;
}

int mt76x2_get_rx_high_gain(RTMP_ADAPTER *ad)
{
	UINT16 value;
	RTMP_CHIP_CAP *cap = &ad->chipCap;

	RT28xx_EEPROM_READ16(ad, RF_2G_RX_HIGH_GAIN, value);
	if ((value & 0xff00) == 0x0000 || ((value & 0xff00) == 0xff00)) {
		cap->rf0_2g_rx_high_gain = 0;
		cap->rf1_2g_rx_high_gain = 0;
	} else {
		if (value & RF0_2G_RX_HIGH_GAIN_SIGN)
			cap->rf0_2g_rx_high_gain = ((value & RF0_2G_RX_HIGH_GAIN_MASK) >> 8);
		else
			cap->rf0_2g_rx_high_gain = -((value & RF0_2G_RX_HIGH_GAIN_MASK) >> 8);

		if (value & RF1_2G_RX_HIGH_GAIN_SIGN)
			cap->rf1_2g_rx_high_gain = ((value & RF1_2G_RX_HIGH_GAIN_MASK) >> 12);
		else
			cap->rf1_2g_rx_high_gain = -((value & RF1_2G_RX_HIGH_GAIN_MASK) >> 12); 
	}

	RT28xx_EEPROM_READ16(ad, RF_5G_GRP0_1_RX_HIGH_GAIN, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->rf0_5g_grp0_rx_high_gain = 0;
		cap->rf1_5g_grp0_rx_high_gain = 0;
	} else {
		if (value & RF0_5G_GRP0_RX_HIGH_GAIN_SIGN)
			cap->rf0_5g_grp0_rx_high_gain = (value & RF0_5G_GRP0_RX_HIGH_GAIN_MASK);
		else
			cap->rf0_5g_grp0_rx_high_gain = -(value & RF0_5G_GRP0_RX_HIGH_GAIN_MASK);

		if (value & RF1_5G_GRP0_RX_HIGH_GAIN_SIGN)
			cap->rf1_5g_grp0_rx_high_gain = ((value & RF1_5G_GRP0_RX_HIGH_GAIN_MASK) >> 4);
		else
			cap->rf1_5g_grp0_rx_high_gain = -((value & RF1_5G_GRP0_RX_HIGH_GAIN_MASK) >> 4);
	}

	if ((value & 0xff00) == 0x0000 || ((value & 0xff00) == 0xff00)) {
		cap->rf0_5g_grp1_rx_high_gain = 0;
		cap->rf1_5g_grp1_rx_high_gain = 0;
	} else {
		if (value & RF0_5G_GRP1_RX_HIGH_GAIN_SIGN)
			cap->rf0_5g_grp1_rx_high_gain = ((value & RF0_5G_GRP1_RX_HIGH_GAIN_MASK) >> 8); 
		else
			cap->rf0_5g_grp1_rx_high_gain = -((value & RF0_5G_GRP1_RX_HIGH_GAIN_MASK) >> 8);

		if (value & RF1_5G_GRP1_RX_HIGH_GAIN_SIGN)
			cap->rf1_5g_grp1_rx_high_gain = ((value & RF1_5G_GRP1_RX_HIGH_GAIN_MASK) >> 12);
		else
			cap->rf1_5g_grp1_rx_high_gain = -((value & RF1_5G_GRP1_RX_HIGH_GAIN_MASK) >> 12); 

	}
	
	RT28xx_EEPROM_READ16(ad, RF_5G_GRP2_3_RX_HIGH_GAIN, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->rf0_5g_grp2_rx_high_gain = 0;
		cap->rf1_5g_grp2_rx_high_gain = 0;
	} else {
		if (value & RF0_5G_GRP2_RX_HIGH_GAIN_SIGN)
			cap->rf0_5g_grp2_rx_high_gain = (value & RF0_5G_GRP2_RX_HIGH_GAIN_MASK);
		else
			cap->rf0_5g_grp2_rx_high_gain = -(value & RF0_5G_GRP2_RX_HIGH_GAIN_MASK);

		if (value & RF1_5G_GRP2_RX_HIGH_GAIN_SIGN)
			cap->rf1_5g_grp2_rx_high_gain = ((value & RF1_5G_GRP2_RX_HIGH_GAIN_MASK) >> 4);
		else
			cap->rf1_5g_grp2_rx_high_gain = -((value & RF1_5G_GRP2_RX_HIGH_GAIN_MASK) >> 4);
	}

	if ((value & 0xff00) == 0x0000 || ((value & 0xff00) == 0xff00)) {
		cap->rf0_5g_grp3_rx_high_gain = 0;
		cap->rf1_5g_grp3_rx_high_gain = 0;
	} else {
		if (value & RF0_5G_GRP3_RX_HIGH_GAIN_SIGN)
			cap->rf0_5g_grp3_rx_high_gain = ((value & RF0_5G_GRP3_RX_HIGH_GAIN_MASK) >> 8); 
		else
			cap->rf0_5g_grp3_rx_high_gain = -((value & RF0_5G_GRP3_RX_HIGH_GAIN_MASK) >> 8);

		if (value & RF1_5G_GRP3_RX_HIGH_GAIN_SIGN)
			cap->rf1_5g_grp3_rx_high_gain = ((value & RF1_5G_GRP3_RX_HIGH_GAIN_MASK) >> 12);
		else
			cap->rf1_5g_grp3_rx_high_gain = -((value & RF1_5G_GRP3_RX_HIGH_GAIN_MASK) >> 12);
	}
	
	RT28xx_EEPROM_READ16(ad, RF_5G_GRP4_5_RX_HIGH_GAIN, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->rf0_5g_grp4_rx_high_gain = 0;
		cap->rf1_5g_grp4_rx_high_gain = 0;
	} else {
		if (value & RF0_5G_GRP4_RX_HIGH_GAIN_SIGN)
			cap->rf0_5g_grp4_rx_high_gain = (value & RF0_5G_GRP4_RX_HIGH_GAIN_MASK);
		else
			cap->rf0_5g_grp4_rx_high_gain = -(value & RF0_5G_GRP4_RX_HIGH_GAIN_MASK);

		if (value & RF1_5G_GRP4_RX_HIGH_GAIN_SIGN)
			cap->rf1_5g_grp4_rx_high_gain = ((value & RF1_5G_GRP4_RX_HIGH_GAIN_MASK) >> 4);
		else
			cap->rf1_5g_grp4_rx_high_gain = -((value & RF1_5G_GRP4_RX_HIGH_GAIN_MASK) >> 4);
	}

	if ((value & 0xff00) == 0x0000 || ((value & 0xff00) == 0xff00)) {
		cap->rf0_5g_grp5_rx_high_gain = 0;
		cap->rf1_5g_grp5_rx_high_gain = 0;
	} else {
		if (value & RF0_5G_GRP5_RX_HIGH_GAIN_SIGN)
			cap->rf0_5g_grp5_rx_high_gain = ((value & RF0_5G_GRP5_RX_HIGH_GAIN_MASK) >> 8); 
		else
			cap->rf0_5g_grp5_rx_high_gain = -((value & RF0_5G_GRP5_RX_HIGH_GAIN_MASK) >> 8);

		if (value & RF1_5G_GRP5_RX_HIGH_GAIN_SIGN)
			cap->rf1_5g_grp5_rx_high_gain = ((value & RF1_5G_GRP5_RX_HIGH_GAIN_MASK) >> 12);
		else
			cap->rf1_5g_grp5_rx_high_gain = -((value & RF1_5G_GRP5_RX_HIGH_GAIN_MASK) >> 12);
	}
}

static int mt76x2_get_tx_pwr_info(RTMP_ADAPTER *ad)
{
	UINT16 value;
	RTMP_CHIP_CAP *cap = &ad->chipCap;

	RT28xx_EEPROM_READ16(ad, G_BAND_20_40_BW_PWR_DELTA, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->delta_tx_pwr_bw40_g_band = 0;
	} else {
		if (value & G_BAND_20_40_BW_PWR_DELTA_EN) {
			if (value & G_BAND_20_40_BW_PWR_DELTA_SIGN)
				cap->delta_tx_pwr_bw40_g_band = (value & G_BAND_20_40_BW_PWR_DELTA_MASK);
			else
				cap->delta_tx_pwr_bw40_g_band = -(value & G_BAND_20_40_BW_PWR_DELTA_MASK);
		} else {
			cap->delta_tx_pwr_bw40_g_band = 0;
		}
	}

	if (((value & 0xff00) == 0x0000) || ((value & 0xff00) == 0xff00)) {
		cap->delta_tx_pwr_bw40_a_band = 0;
	} else {
		if (value & A_BAND_20_40_BW_PWR_DELTA_EN) {
			if (value & A_BAND_20_40_BW_PWR_DELTA_SIGN)
				cap->delta_tx_pwr_bw40_a_band = ((value & A_BAND_20_40_BW_PWR_DELTA_MASK) >> 8);
			else
				cap->delta_tx_pwr_bw40_a_band = -((value & A_BAND_20_40_BW_PWR_DELTA_MASK) >> 8);
		} else {
			cap->delta_tx_pwr_bw40_a_band = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, A_BAND_20_80_BW_PWR_DELTA, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->delta_tx_pwr_bw80 = 0;
	} else {
		if (value & A_BAND_20_80_BW_PWR_DELTA_EN) {
			if (value & A_BAND_20_80_BW_PWR_DELTA_SIGN)
				cap->delta_tx_pwr_bw80 = (value & A_BAND_20_80_BW_PWR_DELTA_MASK);
			else
				cap->delta_tx_pwr_bw80 = -(value & A_BAND_20_80_BW_PWR_DELTA_MASK);
		} else {
			cap->delta_tx_pwr_bw80 = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX0_G_BAND_TSSI_SLOPE, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tssi_0_slope_g_band = TSSI_0_SLOPE_G_BAND_DEFAULT_VALUE;
	} else {
		cap->tssi_0_slope_g_band = (value & TX0_G_BAND_TSSI_SLOPE_MASK);
	}
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tssi_0_offset_g_band = TSSI_0_OFFSET_G_BAND_DEFAULT_VALUE; 
	} else {
		cap->tssi_0_offset_g_band = ((value & TX0_G_BAND_TSSI_OFFSET_MASK) >> 8);
	}	

	RT28xx_EEPROM_READ16(ad, TX0_G_BAND_TARGET_PWR, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_0_target_pwr_g_band = TX_TARGET_PWR_DEFAULT_VALUE; 
	} else {
		cap->tx_0_target_pwr_g_band = (value & TX0_G_BAND_TARGET_PWR_MASK);
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_0_chl_pwr_delta_g_band[G_BAND_LOW] = 0;
	} else {
		if (value & TX0_G_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & TX0_G_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_0_chl_pwr_delta_g_band[G_BAND_LOW] = ((value & TX0_G_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
			else
				cap->tx_0_chl_pwr_delta_g_band[G_BAND_LOW] = -((value & TX0_G_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
		} else {
			cap->tx_0_chl_pwr_delta_g_band[G_BAND_LOW] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX0_G_BAND_CHL_PWR_DELTA_MID, value);
	
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_0_chl_pwr_delta_g_band[G_BAND_MID] = 0;
	} else {
		if (value & TX0_G_BAND_CHL_PWR_DELTA_MID_EN) {
			if (value & TX0_G_BAND_CHL_PWR_DELTA_MID_SIGN)
				cap->tx_0_chl_pwr_delta_g_band[G_BAND_MID] = (value & TX0_G_BAND_CHL_PWR_DELTA_MID_MASK);
			else
				cap->tx_0_chl_pwr_delta_g_band[G_BAND_MID] = -(value & TX0_G_BAND_CHL_PWR_DELTA_MID_MASK);
		} else {
			cap->tx_0_chl_pwr_delta_g_band[G_BAND_MID] = 0;
		}
	}
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_0_chl_pwr_delta_g_band[G_BAND_HI] = 0;
	} else {
		if (value & TX0_G_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & TX0_G_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_0_chl_pwr_delta_g_band[G_BAND_HI] = ((value & TX0_G_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
			else
				cap->tx_0_chl_pwr_delta_g_band[G_BAND_HI] = -((value & TX0_G_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
		} else {
			cap->tx_0_chl_pwr_delta_g_band[G_BAND_HI] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX1_G_BAND_TSSI_SLOPE, value);

	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_1_slope_g_band = TSSI_1_SLOPE_G_BAND_DEFAULT_VALUE;
	else
		cap->tssi_1_slope_g_band = (value & TX1_G_BAND_TSSI_SLOPE_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_1_offset_g_band = TSSI_1_OFFSET_G_BAND_DEFAULT_VALUE;
	else
		cap->tssi_1_offset_g_band = ((value & TX1_G_BAND_TSSI_OFFSET_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, TX1_G_BAND_TARGET_PWR, value);
	
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tx_1_target_pwr_g_band = TX_TARGET_PWR_DEFAULT_VALUE; 
	else
		cap->tx_1_target_pwr_g_band = (value & TX1_G_BAND_TARGET_PWR_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_1_chl_pwr_delta_g_band[G_BAND_LOW] =  0;
	} else {
		if (value & TX1_G_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & TX1_G_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_1_chl_pwr_delta_g_band[G_BAND_LOW] = ((value & TX1_G_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
			else	
				cap->tx_1_chl_pwr_delta_g_band[G_BAND_LOW] = -((value & TX1_G_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
		} else {
			cap->tx_1_chl_pwr_delta_g_band[G_BAND_LOW] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX1_G_BAND_CHL_PWR_DELTA_MID, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_1_chl_pwr_delta_g_band[G_BAND_MID] = 0;
	} else {
		if (value & TX1_G_BAND_CHL_PWR_DELTA_MID_EN) {
			if (value & TX1_G_BAND_CHL_PWR_DELTA_MID_SIGN)
				cap->tx_1_chl_pwr_delta_g_band[G_BAND_MID] = (value & TX1_G_BAND_CHL_PWR_DELTA_MID_MASK);
			else
				cap->tx_1_chl_pwr_delta_g_band[G_BAND_MID] = -(value & TX1_G_BAND_CHL_PWR_DELTA_MID_MASK);
		} else {
			cap->tx_1_chl_pwr_delta_g_band[G_BAND_MID] = 0;
		}
	}
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_1_chl_pwr_delta_g_band[G_BAND_HI] = 0;
	} else {
		if (value & TX1_G_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & TX1_G_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_1_chl_pwr_delta_g_band[G_BAND_HI] = ((value & TX1_G_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
			else
				cap->tx_1_chl_pwr_delta_g_band[G_BAND_HI] = -((value & TX1_G_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
		} else {
			cap->tx_1_chl_pwr_delta_g_band[G_BAND_HI] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP0_TX0_A_BAND_TSSI_SLOPE, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_0_slope_a_band[A_BAND_GRP0_CHL] = TSSI_0_SLOPE_A_BAND_GRP0_DEFAULT_VALUE;
	else
		cap->tssi_0_slope_a_band[A_BAND_GRP0_CHL] = (value & GRP0_TX0_A_BAND_TSSI_SLOPE_MASK);

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_0_offset_a_band[A_BAND_GRP0_CHL] = TSSI_0_OFFSET_A_BAND_GRP0_DEFAULT_VALUE;
	else
		cap->tssi_0_offset_a_band[A_BAND_GRP0_CHL] = ((value & GRP0_TX0_A_BAND_TSSI_OFFSET_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP0_TX0_A_BAND_TARGET_PWR, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tx_0_target_pwr_a_band[A_BAND_GRP0_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;	
	else
		cap->tx_0_target_pwr_a_band[A_BAND_GRP0_CHL] = (value & GRP0_TX0_A_BAND_TARGET_PWR_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP0_TX0_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP0_TX0_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = ((value & GRP0_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
			else
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = -((value & GRP0_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP0_TX0_A_BAND_CHL_PWR_DELTA_HI, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP0_TX0_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP0_TX0_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = (value & GRP0_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK);
			else
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = -(value & GRP0_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_0_slope_a_band[A_BAND_GRP1_CHL] = TSSI_0_SLOPE_A_BAND_GRP1_DEFAULT_VALUE; 
	else
		cap->tssi_0_slope_a_band[A_BAND_GRP1_CHL] = ((value & GRP1_TX0_A_BAND_TSSI_SLOPE_MASK) >> 8);


	RT28xx_EEPROM_READ16(ad, GRP1_TX0_A_BAND_TSSI_OFFSET, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_0_offset_a_band[A_BAND_GRP1_CHL] = TSSI_0_OFFSET_A_BAND_GRP1_DEFAULT_VALUE;
	else
		cap->tssi_0_offset_a_band[A_BAND_GRP1_CHL] = (value & GRP1_TX0_A_BAND_TSSI_OFFSET_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tx_0_target_pwr_a_band[A_BAND_GRP1_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_0_target_pwr_a_band[A_BAND_GRP1_CHL] = ((value & GRP1_TX0_A_BAND_TARGET_PWR_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP1_TX0_A_BAND_CHL_PWR_DELTA_LOW, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP1_TX0_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP1_TX0_A_BAND_CHL_PWR_DELTA_LOW_SIGN) {
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = (value & GRP1_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK);
			} else {
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = -(value & GRP1_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK);
			}
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP1_TX0_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP1_TX0_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = ((value & GRP1_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
			else
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = -((value & GRP1_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP0_TX0_A_BAND_TSSI_SLOPE, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_0_slope_a_band[A_BAND_GRP2_CHL] = TSSI_0_SLOPE_A_BAND_GRP2_DEFAULT_VALUE;
	else
		cap->tssi_0_slope_a_band[A_BAND_GRP2_CHL] = (value & GRP2_TX0_A_BAND_TSSI_SLOPE_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_0_offset_a_band[A_BAND_GRP2_CHL] = TSSI_0_OFFSET_A_BAND_GRP2_DEFAULT_VALUE; 
	else
		cap->tssi_0_offset_a_band[A_BAND_GRP2_CHL] = ((value & GRP2_TX0_A_BAND_TSSI_OFFSET_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP2_TX0_A_BAND_TARGET_PWR, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tx_0_target_pwr_a_band[A_BAND_GRP2_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_0_target_pwr_a_band[A_BAND_GRP2_CHL] = (value & GRP2_TX0_A_BAND_TARGET_PWR_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP2_TX0_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP2_TX0_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = ((value & GRP2_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
			else
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = -((value & GRP2_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP2_TX0_A_BAND_CHL_PWR_DELTA_HI, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP2_TX0_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP2_TX0_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = (value & GRP2_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK);
			else
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = -(value & GRP2_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_0_slope_a_band[A_BAND_GRP3_CHL] = TSSI_0_SLOPE_A_BAND_GRP3_DEFAULT_VALUE;
	else
		cap->tssi_0_slope_a_band[A_BAND_GRP3_CHL] = ((value & GRP3_TX0_A_BAND_TSSI_SLOPE_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP3_TX0_A_BAND_TSSI_OFFSET, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_0_offset_a_band[A_BAND_GRP3_CHL] = TSSI_0_OFFSET_A_BAND_GRP3_DEFAULT_VALUE;
	else
		cap->tssi_0_offset_a_band[A_BAND_GRP3_CHL] = (value & GRP3_TX0_A_BAND_TSSI_OFFSET_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tx_0_target_pwr_a_band[A_BAND_GRP3_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_0_target_pwr_a_band[A_BAND_GRP3_CHL] = ((value & GRP3_TX0_A_BAND_TARGET_PWR_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP3_TX0_A_BAND_CHL_PWR_DELTA_LOW, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP3_TX0_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP3_TX0_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = (value & GRP3_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK);
			else
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = -(value & GRP3_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = 0;
		}
	} 	

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP3_TX0_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP3_TX0_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = ((value & GRP3_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
			else
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = -((value & GRP3_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP4_TX0_A_BAND_TSSI_SLOPE, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_0_slope_a_band[A_BAND_GRP4_CHL] = TSSI_0_SLOPE_A_BAND_GRP4_DEFAULT_VALUE;
	else
		cap->tssi_0_slope_a_band[A_BAND_GRP4_CHL] = (value & GRP4_TX0_A_BAND_TSSI_SLOPE_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) 
		cap->tssi_0_offset_a_band[A_BAND_GRP4_CHL] = TSSI_0_OFFSET_A_BAND_GRP4_DEFAULT_VALUE;
	else
		cap->tssi_0_offset_a_band[A_BAND_GRP4_CHL] = ((value & GRP4_TX0_A_BAND_TSSI_OFFSET_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP4_TX0_A_BAND_TARGET_PWR, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tx_0_target_pwr_a_band[A_BAND_GRP4_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_0_target_pwr_a_band[A_BAND_GRP4_CHL] = (value & GRP4_TX0_A_BAND_TARGET_PWR_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP4_TX0_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP4_TX0_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = ((value & GRP4_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
			else
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = -((value & GRP4_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP4_TX0_A_BAND_CHL_PWR_DELTA_HI, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP4_TX0_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP4_TX0_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = (value & GRP4_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK);
			else	
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = -(value & GRP4_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = 0;
		}
	} 

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_0_slope_a_band[A_BAND_GRP5_CHL] = TSSI_0_SLOPE_A_BAND_GRP5_DEFAULT_VALUE;
	else
		cap->tssi_0_slope_a_band[A_BAND_GRP5_CHL] = ((value & GRP5_TX0_A_BAND_TSSI_SLOPE_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP5_TX0_A_BAND_TSSI_OFFSET, value);
	
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_0_offset_a_band[A_BAND_GRP5_CHL] = TSSI_0_OFFSET_A_BAND_GRP5_DEFAULT_VALUE;
	else
		cap->tssi_0_offset_a_band[A_BAND_GRP5_CHL] = (value & GRP5_TX0_A_BAND_TSSI_OFFSET_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tx_0_target_pwr_a_band[A_BAND_GRP5_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_0_target_pwr_a_band[A_BAND_GRP5_CHL] = ((value & GRP5_TX0_A_BAND_TARGET_PWR_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP5_TX0_A_BAND_CHL_PWR_DELTA_LOW, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP5_TX0_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP5_TX0_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = (value & GRP5_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK);
			else
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = -(value & GRP5_TX0_A_BAND_CHL_PWR_DELTA_LOW_MASK);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = 0;
		}
	}
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP5_TX0_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP5_TX0_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = ((value & GRP5_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
			else	
				cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = -((value & GRP5_TX0_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
		} else {
			cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = 0;
		}
	}

	/* 5G TX1 chl pwr */
	RT28xx_EEPROM_READ16(ad, GRP0_TX1_A_BAND_TSSI_SLOPE, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_1_slope_a_band[A_BAND_GRP0_CHL] = TSSI_1_SLOPE_A_BAND_GRP0_DEFAULT_VALUE;
	else
		cap->tssi_1_slope_a_band[A_BAND_GRP0_CHL] = (value & GRP0_TX1_A_BAND_TSSI_SLOPE_MASK);

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_1_offset_a_band[A_BAND_GRP0_CHL] = TSSI_1_OFFSET_A_BAND_GRP0_DEFAULT_VALUE;
	else
		cap->tssi_1_offset_a_band[A_BAND_GRP0_CHL] = ((value & GRP0_TX1_A_BAND_TSSI_OFFSET_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP0_TX1_A_BAND_TARGET_PWR, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tx_1_target_pwr_a_band[A_BAND_GRP0_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_1_target_pwr_a_band[A_BAND_GRP0_CHL] = (value & GRP0_TX1_A_BAND_TARGET_PWR_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP0_TX1_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP0_TX1_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = ((value & GRP0_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
			else
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = -((value & GRP0_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP0_TX1_A_BAND_CHL_PWR_DELTA_HI, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP0_TX1_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP0_TX1_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = (value & GRP0_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK);
			else
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = -(value & GRP0_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_1_slope_a_band[A_BAND_GRP1_CHL] = TSSI_1_SLOPE_A_BAND_GRP1_DEFAULT_VALUE;
	else
		cap->tssi_1_slope_a_band[A_BAND_GRP1_CHL] = ((value & GRP1_TX1_A_BAND_TSSI_SLOPE_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP1_TX1_A_BAND_TSSI_OFFSET, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_1_offset_a_band[A_BAND_GRP1_CHL] = TSSI_1_OFFSET_A_BAND_GRP1_DEFAULT_VALUE;
	else
		cap->tssi_1_offset_a_band[A_BAND_GRP1_CHL] = (value & GRP1_TX1_A_BAND_TSSI_OFFSET_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tx_1_target_pwr_a_band[A_BAND_GRP1_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_1_target_pwr_a_band[A_BAND_GRP1_CHL] = ((value & GRP1_TX1_A_BAND_TARGET_PWR_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP1_TX1_A_BAND_CHL_PWR_DELTA_LOW, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP1_TX1_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP1_TX1_A_BAND_CHL_PWR_DELTA_LOW_SIGN) {
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = (value & GRP1_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK);
			} else {
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = -(value & GRP1_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK);
			}
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP1_TX1_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP1_TX1_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = ((value & GRP1_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
			else
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = -((value & GRP1_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP0_TX1_A_BAND_TSSI_SLOPE, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_1_slope_a_band[A_BAND_GRP2_CHL] = TSSI_1_SLOPE_A_BAND_GRP2_DEFAULT_VALUE;
	else
		cap->tssi_1_slope_a_band[A_BAND_GRP2_CHL] = (value & GRP2_TX1_A_BAND_TSSI_SLOPE_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_1_offset_a_band[A_BAND_GRP2_CHL] = TSSI_1_OFFSET_A_BAND_GRP2_DEFAULT_VALUE;
	else
		cap->tssi_1_offset_a_band[A_BAND_GRP2_CHL] = ((value & GRP2_TX1_A_BAND_TSSI_OFFSET_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP2_TX1_A_BAND_TARGET_PWR, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tx_1_target_pwr_a_band[A_BAND_GRP2_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_1_target_pwr_a_band[A_BAND_GRP2_CHL] = (value & GRP2_TX1_A_BAND_TARGET_PWR_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP2_TX1_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP2_TX1_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = ((value & GRP2_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
			else
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = -((value & GRP2_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP2_TX1_A_BAND_CHL_PWR_DELTA_HI, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP2_TX1_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP2_TX1_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = (value & GRP2_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK);
			else
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = -(value & GRP2_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_1_slope_a_band[A_BAND_GRP3_CHL] = TSSI_1_SLOPE_A_BAND_GRP3_DEFAULT_VALUE;
	else
		cap->tssi_1_slope_a_band[A_BAND_GRP3_CHL] = ((value & GRP3_TX1_A_BAND_TSSI_SLOPE_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP3_TX1_A_BAND_TSSI_OFFSET, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_1_offset_a_band[A_BAND_GRP3_CHL] = TSSI_1_OFFSET_A_BAND_GRP3_DEFAULT_VALUE;
	else
		cap->tssi_1_offset_a_band[A_BAND_GRP3_CHL] = (value & GRP3_TX1_A_BAND_TSSI_OFFSET_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tx_1_target_pwr_a_band[A_BAND_GRP3_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_1_target_pwr_a_band[A_BAND_GRP3_CHL] = ((value & GRP3_TX1_A_BAND_TARGET_PWR_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP3_TX1_A_BAND_CHL_PWR_DELTA_LOW, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP3_TX1_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP3_TX1_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = (value & GRP3_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK);
			else
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = -(value & GRP3_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP3_TX1_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP3_TX0_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = ((value & GRP3_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
			else
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = -((value & GRP3_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = 0;
		}
	}


	RT28xx_EEPROM_READ16(ad, GRP4_TX1_A_BAND_TSSI_SLOPE, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_1_slope_a_band[A_BAND_GRP4_CHL] = TSSI_1_SLOPE_A_BAND_GRP4_DEFAULT_VALUE;
	else
		cap->tssi_1_slope_a_band[A_BAND_GRP4_CHL] = (value & GRP4_TX1_A_BAND_TSSI_SLOPE_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_1_offset_a_band[A_BAND_GRP4_CHL] = TSSI_1_OFFSET_A_BAND_GRP4_DEFAULT_VALUE;
	else
		cap->tssi_1_offset_a_band[A_BAND_GRP4_CHL] = ((value & GRP4_TX1_A_BAND_TSSI_OFFSET_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP4_TX1_A_BAND_TARGET_PWR, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tx_1_target_pwr_a_band[A_BAND_GRP4_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_1_target_pwr_a_band[A_BAND_GRP4_CHL] = (value & GRP4_TX1_A_BAND_TARGET_PWR_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP4_TX1_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP4_TX1_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = ((value & GRP4_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
			else
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = -((value & GRP4_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK) >> 8);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, GRP4_TX1_A_BAND_CHL_PWR_DELTA_HI, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP4_TX1_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP4_TX1_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = (value & GRP4_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK);
			else	
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = -(value & GRP4_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tssi_1_slope_a_band[A_BAND_GRP5_CHL] = TSSI_1_SLOPE_A_BAND_GRP5_DEFAULT_VALUE;
	else
		cap->tssi_1_slope_a_band[A_BAND_GRP5_CHL] = ((value & GRP5_TX0_A_BAND_TSSI_SLOPE_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP5_TX1_A_BAND_TSSI_OFFSET, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff))
		cap->tssi_1_offset_a_band[A_BAND_GRP5_CHL] = TSSI_1_OFFSET_A_BAND_GRP5_DEFAULT_VALUE;
	else
		cap->tssi_1_offset_a_band[A_BAND_GRP5_CHL] = (value & GRP5_TX1_A_BAND_TSSI_OFFSET_MASK);
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00))
		cap->tx_1_target_pwr_a_band[A_BAND_GRP5_CHL] = TX_TARGET_PWR_DEFAULT_VALUE;
	else
		cap->tx_1_target_pwr_a_band[A_BAND_GRP5_CHL] = ((value & GRP5_TX1_A_BAND_TARGET_PWR_MASK) >> 8);

	RT28xx_EEPROM_READ16(ad, GRP5_TX1_A_BAND_CHL_PWR_DELTA_LOW, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = 0;
	} else {
		if (value & GRP5_TX1_A_BAND_CHL_PWR_DELTA_LOW_EN) {
			if (value & GRP5_TX1_A_BAND_CHL_PWR_DELTA_LOW_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = (value & GRP5_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK);
			else
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = -(value & GRP5_TX1_A_BAND_CHL_PWR_DELTA_LOW_MASK);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = 0;
		}
	}
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = 0;
	} else {
		if (value & GRP5_TX1_A_BAND_CHL_PWR_DELTA_HI_EN) {
			if (value & GRP5_TX1_A_BAND_CHL_PWR_DELTA_HI_SIGN)
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = ((value & GRP5_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
			else	
				cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = -((value & GRP5_TX1_A_BAND_CHL_PWR_DELTA_HI_MASK) >> 8);
		} else {
			cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = 0;
		}
	}

	/* check tssi if enable */
	RT28xx_EEPROM_READ16(ad, NIC_CONFIGURE_1, value);
	if (value & INTERNAL_TX_ALC_EN)
		cap->tssi_enable = TRUE;
	else
		cap->tssi_enable = FALSE;

	/* check PA type combination */
	RT28xx_EEPROM_READ16(ad, EEPROM_NIC1_OFFSET, value);
	cap->PAType= GET_PA_TYPE(value);
}

static UINT8 mt76x2_txpwr_chlist[] = {
	1, 2,3,4,5,6,7,8,9,10,11,12,13,14,
	36,38,40,44,46,48,52,54,56,60,62,64,
	100,102,104,108,110,112,116,118,120,124,126,128,132,134,136,140,
	149,151,153,157,159,161,165,167,169,171,173,
	42, 58, 106, 122, 155,
};

int mt76x2_read_chl_pwr(RTMP_ADAPTER *ad)
{
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	UINT32 i, choffset;
	
	mt76x2_get_tx_pwr_info(ad);

	/* Read Tx power value for all channels*/
	/* Value from 1 - 0x7f. Default value is 24.*/
	/* Power value : 2.4G 0x00 (0) ~ 0x1F (31)*/
	/*             : 5.5G 0xF9 (-7) ~ 0x0F (15)*/

	DBGPRINT(RT_DEBUG_TRACE, ("%s()--->\n", __FUNCTION__));
	
	for (i = 0; i < sizeof(mt76x2_txpwr_chlist); i++) {
		ad->TxPower[i].Channel = mt76x2_txpwr_chlist[i];
		ad->TxPower[i].Power = TX_TARGET_PWR_DEFAULT_VALUE;
		ad->TxPower[i].Power2 = TX_TARGET_PWR_DEFAULT_VALUE;
	}

	/* 0. 11b/g, ch1 - ch 14, 2SS */
	for (i = 0; i < 14; i++) {
		ad->TxPower[i].Power = cap->tx_0_target_pwr_g_band;
		ad->TxPower[i].Power2 = cap->tx_1_target_pwr_g_band;
	}

	/* 1. U-NII lower/middle band: 36, 38, 40; 44, 46, 48; 52, 54, 56; 60, 62, 64 (including central frequency in BW 40MHz)*/
	choffset = 14;
	ASSERT((ad->TxPower[choffset].Channel == 36));

	for (i = 0; i < 39; i++) {
		ad->TxPower[i + choffset].Power = cap->tx_0_target_pwr_a_band[get_chl_grp(ad->TxPower[i+choffset].Channel)];
		ad->TxPower[i + choffset].Power2 = cap->tx_1_target_pwr_a_band[get_chl_grp(ad->TxPower[i+choffset].Channel)];
	}

	choffset = 14 + 12 + 16 + 11;

#ifdef DOT11_VHT_AC
	ASSERT((ad->TxPower[choffset].Channel == 42));

	// TODO: shiang-6590, fix me for the TxPower setting code here!
	/* For VHT80MHz, we need assign tx power for central channel 42, 58, 106, 122, and 155 */
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Update Tx power control of the central channel (42, 58, 106, 122 and 155) for VHT BW80\n", __FUNCTION__));
		
	NdisMoveMemory(&ad->TxPower[53], &ad->TxPower[16], sizeof(CHANNEL_TX_POWER)); // channel 42 = channel 40
	NdisMoveMemory(&ad->TxPower[54], &ad->TxPower[22], sizeof(CHANNEL_TX_POWER)); // channel 58 = channel 56
	NdisMoveMemory(&ad->TxPower[55], &ad->TxPower[28], sizeof(CHANNEL_TX_POWER)); // channel 106 = channel 104
	NdisMoveMemory(&ad->TxPower[56], &ad->TxPower[34], sizeof(CHANNEL_TX_POWER)); // channel 122 = channel 120
	NdisMoveMemory(&ad->TxPower[57], &ad->TxPower[44], sizeof(CHANNEL_TX_POWER)); // channel 155 = channel 153

	ad->TxPower[choffset].Channel = 42;
	ad->TxPower[choffset+1].Channel = 58;
	ad->TxPower[choffset+2].Channel = 106;
	ad->TxPower[choffset+3].Channel = 122;
	ad->TxPower[choffset+4].Channel = 155;

	choffset += 5;		/* the central channel of VHT80 */
	choffset = (MAX_NUM_OF_CHANNELS - 1);
#endif /* DOT11_VHT_AC */

	/* 4. Print and Debug*/
	for (i = 0; i < choffset; i++)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: TxPower[%03d], Channel=%d, Power[Tx0:%d, Tx1:%d]\n",
					i, ad->TxPower[i].Channel, ad->TxPower[i].Power, ad->TxPower[i].Power2 ));
	}

	return TRUE;
}

static INT rf_tr_agc_config(RTMP_ADAPTER *pAd, INT rf_bw)
{
	signed char rx_agc_fc_offset[3] = {2,2,2}; /* array idx 0: 20M, 1:40M, 2:80m */
	UINT8 tx_agc_fc_offset[3] = {0,0,0}; /* array idx 0: 20M, 1:40M, 2:80m */
	CHAR rf32_val, rf31_val, rf_diff;
}

void mt76x2_get_tx_pwr_per_rate(RTMP_ADAPTER *ad)
{
	UINT16 value;
	RTMP_CHIP_CAP *cap = &ad->chipCap;

	RT28xx_EEPROM_READ16(ad, TX_PWR_CCK_1_2M, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_cck_1_2 = 0;
	} else {
		if (value & TX_PWR_CCK_1_2M_EN) {
			if (value & TX_PWR_CCK_1_2M_SIGN)
				cap->tx_pwr_cck_1_2 = (value & TX_PWR_CCK_1_2M_MASK);
			else
				cap->tx_pwr_cck_1_2 = -(value & TX_PWR_CCK_1_2M_MASK);
		} else {
			cap->tx_pwr_cck_1_2 = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_cck_5_11 = 0;
	} else {
		if (value & TX_PWR_CCK_5_11M_EN) {
			if (value & TX_PWR_CCK_5_11M_SIGN)
				cap->tx_pwr_cck_5_11 = ((value & TX_PWR_CCK_5_11M_MASK) >> 8);
			else
				cap->tx_pwr_cck_5_11 = -((value & TX_PWR_CCK_5_11M_MASK) >> 8);
		} else {
			cap->tx_pwr_cck_5_11 = 0;
		}


	}

	RT28xx_EEPROM_READ16(ad, TX_PWR_G_BAND_OFDM_6_9M, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_g_band_ofdm_6_9 = 0;
	} else {
		if (value & TX_PWR_G_BAND_OFDM_6_9M_EN) {
			if (value & TX_PWR_G_BAND_OFDM_6_9M_SIGN)
				cap->tx_pwr_g_band_ofdm_6_9 = (value & TX_PWR_G_BAND_OFDM_6_9M_MASK);
			else
				cap->tx_pwr_g_band_ofdm_6_9 = -(value & TX_PWR_G_BAND_OFDM_6_9M_MASK);
		} else {
			cap->tx_pwr_g_band_ofdm_6_9 = 0;
		}
	} 

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_g_band_ofdm_12_18 = 0;
	} else {
		if (value & TX_PWR_G_BAND_OFDM_12_18M_EN) {
			if (value & TX_PWR_G_BAND_OFDM_12_18M_SIGN)
				cap->tx_pwr_g_band_ofdm_12_18 = ((value & TX_PWR_G_BAND_OFDM_12_18M_MASK) >> 8);
			else
				cap->tx_pwr_g_band_ofdm_12_18 = -((value & TX_PWR_G_BAND_OFDM_12_18M_MASK) >> 8);
		} else {
			cap->tx_pwr_g_band_ofdm_12_18 = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX_PWR_G_BAND_OFDM_24_36M, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_g_band_ofdm_24_36 = 0;
	} else {
		if (value & TX_PWR_G_BAND_OFDM_24_36M_EN) {
			if (value & TX_PWR_G_BAND_OFDM_24_36M_SIGN)
				cap->tx_pwr_g_band_ofdm_24_36 = (value & TX_PWR_G_BAND_OFDM_24_36M_MASK);
			else
				cap->tx_pwr_g_band_ofdm_24_36 = -(value & TX_PWR_G_BAND_OFDM_24_36M_MASK);
		} else {
			cap->tx_pwr_g_band_ofdm_24_36 = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_g_band_ofdm_48_54 = 0;
	} else {
		if (value & TX_PWR_G_BAND_OFDM_48_54M_EN) {
			if (value & TX_PWR_G_BAND_OFDM_48_54M_SIGN)
				cap->tx_pwr_g_band_ofdm_48_54 = ((value & TX_PWR_G_BAND_OFDM_48_54M_MASK) >> 8);
			else
				cap->tx_pwr_g_band_ofdm_48_54 = -((value & TX_PWR_G_BAND_OFDM_48_54M_MASK) >> 8);
		} else {
			cap->tx_pwr_g_band_ofdm_48_54 = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX_PWR_HT_MCS_0_1, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_ht_mcs_0_1 = (value & TX_PWR_HT_MCS_0_1_MASK);
	} else {
		if (value & TX_PWR_HT_MCS_0_1_EN) {
			if (value & TX_PWR_HT_MCS_0_1_SIGN)
				cap->tx_pwr_ht_mcs_0_1 = (value & TX_PWR_HT_MCS_0_1_MASK);
			else
				cap->tx_pwr_ht_mcs_0_1 = -(value & TX_PWR_HT_MCS_0_1_MASK);
		} else {
			cap->tx_pwr_ht_mcs_0_1 = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_ht_mcs_2_3 = ((value & TX_PWR_HT_MCS_2_3_MASK) >> 8);
	} else {
		if (value & TX_PWR_HT_MCS_2_3_EN) {
			if (value & TX_PWR_HT_MCS_2_3_SIGN)
				cap->tx_pwr_ht_mcs_2_3 = ((value & TX_PWR_HT_MCS_2_3_MASK) >> 8);
			else
				cap->tx_pwr_ht_mcs_2_3 = -((value & TX_PWR_HT_MCS_2_3_MASK) >> 8);
		} else {
			cap->tx_pwr_ht_mcs_2_3 = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX_PWR_HT_MCS_4_5, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_ht_mcs_4_5 = 0;
	} else {
		if (value & TX_PWR_HT_MCS_4_5_EN) {
			if (value & TX_PWR_HT_MCS_4_5_SIGN)
				cap->tx_pwr_ht_mcs_4_5 = (value & TX_PWR_HT_MCS_4_5_MASK);
			else
				cap->tx_pwr_ht_mcs_4_5 = -(value & TX_PWR_HT_MCS_4_5_MASK);
		} else {
			cap->tx_pwr_ht_mcs_4_5 = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_ht_mcs_6_7 = 0;
	} else {
		if (value & TX_PWR_HT_MCS_6_7_EN) {
			if (value & TX_PWR_HT_MCS_6_7_SIGN)
				cap->tx_pwr_ht_mcs_6_7 = ((value & TX_PWR_HT_MCS_6_7_MASK) >> 8);
			else
				cap->tx_pwr_ht_mcs_6_7 = -((value & TX_PWR_HT_MCS_6_7_MASK) >> 8);
		} else {
			cap->tx_pwr_ht_mcs_6_7 = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX_PWR_HT_MCS_8_9, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_ht_mcs_8_9 = 0;
	} else {
		if (value & TX_PWR_HT_MCS_8_9_EN) {
			if (value & TX_PWR_HT_MCS_8_9_SIGN)
				cap->tx_pwr_ht_mcs_8_9 = (value & TX_PWR_HT_MCS_8_9_MASK);
			else
				cap->tx_pwr_ht_mcs_8_9 = -(value & TX_PWR_HT_MCS_8_9_MASK);
		} else {
			cap->tx_pwr_ht_mcs_8_9 = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_ht_mcs_10_11 = 0;
	} else {
		if (value & TX_PWR_HT_MCS_10_11_EN) {
			if (value & TX_PWR_HT_MCS_10_11_SIGN)
				cap->tx_pwr_ht_mcs_10_11 = ((value & TX_PWR_HT_MCS_10_11_MASK) >> 8);
			else
				cap->tx_pwr_ht_mcs_10_11 = -((value & TX_PWR_HT_MCS_10_11_MASK) >> 8);
		} else {
			cap->tx_pwr_ht_mcs_10_11 = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX_PWR_HT_MCS_12_13, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_ht_mcs_12_13 = 0;
	} else {
		if (value & TX_PWR_HT_MCS_12_13_EN) {
			if (value & TX_PWR_HT_MCS_12_13_SIGN)
				cap->tx_pwr_ht_mcs_12_13 = (value & TX_PWR_HT_MCS_12_13_MASK);
			else
				cap->tx_pwr_ht_mcs_12_13 = -(value & TX_PWR_HT_MCS_12_13_MASK);
		} else {
			cap->tx_pwr_ht_mcs_12_13 = 0;
		} 
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_ht_mcs_14_15 = 0;
	} else {
		if (value & TX_PWR_HT_MCS_14_15_EN) {
			if (value & TX_PWR_HT_MCS_14_15_SIGN)
				cap->tx_pwr_ht_mcs_14_15 = ((value & TX_PWR_HT_MCS_14_15_MASK) >> 8);
			else
				cap->tx_pwr_ht_mcs_14_15 = -((value & TX_PWR_HT_MCS_14_15_MASK) >> 8);
		} else {
			cap->tx_pwr_ht_mcs_14_15 = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX_PWR_A_BAND_OFDM_6_9M, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_a_band_ofdm_6_9 = 0;
	} else {
		if (value & TX_PWR_A_BAND_OFDM_6_9M_EN) {
			if (value & TX_PWR_A_BAND_OFDM_6_9M_SIGN)
				cap->tx_pwr_a_band_ofdm_6_9 = (value & TX_PWR_A_BAND_OFDM_6_9M_MASK);
			else
				cap->tx_pwr_a_band_ofdm_6_9 = (value & TX_PWR_A_BAND_OFDM_6_9M_MASK);
		} else {
			cap->tx_pwr_a_band_ofdm_6_9 = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_a_band_ofdm_12_18 = 0;
	} else {
		if (value & TX_PWR_A_BAND_OFDM_12_18M_EN) {
			if (value & TX_PWR_A_BAND_OFDM_12_18M_SIGN)
				cap->tx_pwr_a_band_ofdm_12_18 = ((value & TX_PWR_A_BAND_OFDM_12_18M_MASK) >> 8);
			else
				cap->tx_pwr_a_band_ofdm_12_18 = -((value & TX_PWR_A_BAND_OFDM_12_18M_MASK) >> 8);
		} else {
			cap->tx_pwr_a_band_ofdm_12_18 = 0;
		}
	}
	
	RT28xx_EEPROM_READ16(ad, TX_PWR_A_BAND_OFDM_24_36M, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_a_band_ofdm_24_36 = 0; 
	} else {
		if (value & TX_PWR_A_BAND_OFDM_24_36M_EN) {
			if (value & TX_PWR_A_BAND_OFDM_24_36M_SIGN)
				cap->tx_pwr_a_band_ofdm_24_36 = (value & TX_PWR_A_BAND_OFDM_24_36M_MASK);
			else
				cap->tx_pwr_a_band_ofdm_24_36 = -(value & TX_PWR_A_BAND_OFDM_24_36M_MASK);
		} else {
			cap->tx_pwr_a_band_ofdm_24_36 = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_a_band_ofdm_48_54 = 0;
	} else {
		if (value & TX_PWR_A_BAND_OFDM_48_54M_EN) {
			if (value & TX_PWR_A_BAND_OFDM_48_54M_SIGN)
				cap->tx_pwr_a_band_ofdm_48_54 = ((value & TX_PWR_A_BAND_OFDM_48_54M_MASK) >> 8);
			else
				cap->tx_pwr_a_band_ofdm_48_54 = -((value & TX_PWR_A_BAND_OFDM_48_54M_MASK) >> 8);
		} else {
			cap->tx_pwr_a_band_ofdm_48_54 = 0;
		}
	}
	
	
	RT28xx_EEPROM_READ16(ad, TX_PWR_VHT_MCS_0_1, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_vht_mcs_0_1 = 0;
	} else {
		if (value & TX_PWR_VHT_MCS_0_1_EN) {
			if (value & TX_PWR_VHT_MCS_0_1_SIGN)
				cap->tx_pwr_vht_mcs_0_1 = (value & TX_PWR_VHT_MCS_0_1_MASK);
			else
				cap->tx_pwr_vht_mcs_0_1 = -(value & TX_PWR_VHT_MCS_0_1_MASK);
		} else {
			cap->tx_pwr_vht_mcs_0_1 = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_vht_mcs_2_3 = 0;
	} else {
		if (value & TX_PWR_VHT_MCS_2_3_EN) {
			if (value & TX_PWR_VHT_MCS_2_3_SIGN)
				cap->tx_pwr_vht_mcs_2_3 = ((value & TX_PWR_VHT_MCS_2_3_MASK) >> 8);
			else
				cap->tx_pwr_vht_mcs_2_3 = -((value & TX_PWR_VHT_MCS_2_3_MASK) >> 8);
		} else {
			cap->tx_pwr_vht_mcs_2_3 = 0;
		}
	}

	RT28xx_EEPROM_READ16(ad, TX_PWR_VHT_MCS_4_5, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_vht_mcs_4_5 = 0;
	} else {
		if (value & TX_PWR_VHT_MCS_4_5_EN) {
			if (value & TX_PWR_VHT_MCS_4_5_SIGN)
				cap->tx_pwr_vht_mcs_4_5 = (value & TX_PWR_VHT_MCS_4_5_MASK);
			else
				cap->tx_pwr_vht_mcs_4_5 = -(value & TX_PWR_VHT_MCS_4_5_MASK);
		} else {
			cap->tx_pwr_vht_mcs_4_5 = 0;
		}
	}

	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_vht_mcs_6_7 = 0;
	} else {
		if (value & TX_PWR_VHT_MCS_6_7_EN) {
			if (value & TX_PWR_VHT_MCS_6_7_SIGN)
				cap->tx_pwr_vht_mcs_6_7 = ((value & TX_PWR_VHT_MCS_6_7_MASK) >> 8);
			else
				cap->tx_pwr_vht_mcs_6_7 = -((value & TX_PWR_VHT_MCS_6_7_MASK) >> 8);
		} else {
			cap->tx_pwr_vht_mcs_6_7 = 0;
		}
	}
	
	RT28xx_EEPROM_READ16(ad, TX_PWR_5G_VHT_MCS_8_9, value);
	if (((value & 0xff) == 0x00) || ((value & 0xff) == 0xff)) {
		cap->tx_pwr_5g_vht_mcs_8_9 = 0;
	} else {
		if (value & TX_PWR_5G_VHT_MCS_8_9_EN) {
			if (value & TX_PWR_5G_VHT_MCS_8_9_SIGN)
				cap->tx_pwr_5g_vht_mcs_8_9 = (value & TX_PWR_5G_VHT_MCS_8_9_MASK);
			else
				cap->tx_pwr_5g_vht_mcs_8_9 = -(value & TX_PWR_5G_VHT_MCS_8_9_MASK);
		} else {
			cap->tx_pwr_5g_vht_mcs_8_9 = 0;
		}
	}
	
	if (((value & 0xff00) == 0x00) || ((value & 0xff00) == 0xff00)) {
		cap->tx_pwr_2g_vht_mcs_8_9 = 0;
	} else {
		if (value & TX_PWR_2G_VHT_MCS_8_9_EN) {
			if (value & TX_PWR_2G_VHT_MCS_8_9_SIGN)
				cap->tx_pwr_2g_vht_mcs_8_9 = ((value & TX_PWR_2G_VHT_MCS_8_9_MASK) >> 8);
			else
				cap->tx_pwr_2g_vht_mcs_8_9 = -((value & TX_PWR_2G_VHT_MCS_8_9_MASK) >> 8);
		} else {
			cap->tx_pwr_2g_vht_mcs_8_9 = 0;
		}
	}
}

static void mt76x2_show_pwr_info(RTMP_ADAPTER *ad)
{
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	UINT32 value;

	DBGPRINT(RT_DEBUG_OFF, ("\n===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("channel info related to power\n"));
	DBGPRINT(RT_DEBUG_OFF, ("===================================\n"));
	
	if (ad->LatchRfRegs.Channel < 14) {
		DBGPRINT(RT_DEBUG_OFF, ("central channel = %d, low_mid_hi = %d\n", ad->LatchRfRegs.Channel,
							get_low_mid_hi_index(ad->LatchRfRegs.Channel)));
								
	} else {
		DBGPRINT(RT_DEBUG_OFF, ("central channel = %d, group = %d, low_mid_hi = %d\n", 
							ad->LatchRfRegs.Channel,
							get_chl_grp(ad->LatchRfRegs.Channel),
							get_low_mid_hi_index(ad->LatchRfRegs.Channel)));
	}

	DBGPRINT(RT_DEBUG_OFF, ("\n===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("channel power(unit: 0.5dbm)\n"));
	DBGPRINT(RT_DEBUG_OFF, ("===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_target_pwr_g_band = 0x%x\n", cap->tx_0_target_pwr_g_band));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_target_pwr_g_band = 0x%x\n", cap->tx_1_target_pwr_g_band));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_target_pwr_a_band[A_BAND_GRP0_CHL] = 0x%x\n", cap->tx_0_target_pwr_a_band[A_BAND_GRP0_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_target_pwr_a_band[A_BAND_GRP1_CHL] = 0x%x\n", cap->tx_0_target_pwr_a_band[A_BAND_GRP1_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_target_pwr_a_band[A_BAND_GRP2_CHL] = 0x%x\n", cap->tx_0_target_pwr_a_band[A_BAND_GRP2_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_target_pwr_a_band[A_BAND_GRP3_CHL] = 0x%x\n", cap->tx_0_target_pwr_a_band[A_BAND_GRP3_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_target_pwr_a_band[A_BAND_GRP4_CHL] = 0x%x\n", cap->tx_0_target_pwr_a_band[A_BAND_GRP4_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_target_pwr_a_band[A_BAND_GRP5_CHL] = 0x%x\n", cap->tx_0_target_pwr_a_band[A_BAND_GRP5_CHL]));
	
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_target_pwr_a_band[A_BAND_GRP0_CHL] = 0x%x\n", cap->tx_1_target_pwr_a_band[A_BAND_GRP0_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_target_pwr_a_band[A_BAND_GRP1_CHL] = 0x%x\n", cap->tx_1_target_pwr_a_band[A_BAND_GRP1_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_target_pwr_a_band[A_BAND_GRP2_CHL] = 0x%x\n", cap->tx_1_target_pwr_a_band[A_BAND_GRP2_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_target_pwr_a_band[A_BAND_GRP3_CHL] = 0x%x\n", cap->tx_1_target_pwr_a_band[A_BAND_GRP3_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_target_pwr_a_band[A_BAND_GRP4_CHL] = 0x%x\n", cap->tx_1_target_pwr_a_band[A_BAND_GRP4_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_target_pwr_a_band[A_BAND_GRP5_CHL] = 0x%x\n", cap->tx_1_target_pwr_a_band[A_BAND_GRP5_CHL]));
	
	/* channel power delta */
	DBGPRINT(RT_DEBUG_OFF, ("\n===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("channel power deltai(unit: 0.5db)\n"));
	DBGPRINT(RT_DEBUG_OFF, ("===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_g_band[G_BAND_LOW] = 0x%x\n", cap->tx_0_chl_pwr_delta_g_band[G_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_g_band[G_BAND_MID] = 0x%x\n", cap->tx_0_chl_pwr_delta_g_band[G_BAND_MID]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_g_band[G_BAND_HI] = 0x%x\n", cap->tx_0_chl_pwr_delta_g_band[G_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_g_band[G_BAND_LOW] = 0x%x\n", cap->tx_1_chl_pwr_delta_g_band[G_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_g_band[G_BAND_MID] = 0x%x\n", cap->tx_1_chl_pwr_delta_g_band[G_BAND_MID]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_g_band[G_BAND_HI] = 0x%x\n", cap->tx_1_chl_pwr_delta_g_band[G_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = %d\n", cap->tx_0_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI]));
	
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP0_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP1_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP2_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP3_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP4_CHL][A_BAND_HI]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW] = %d\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_LOW]));
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI] = %d\n\n", cap->tx_1_chl_pwr_delta_a_band[A_BAND_GRP5_CHL][A_BAND_HI]));


	/* bw power delta */
	DBGPRINT(RT_DEBUG_OFF, ("\n===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("bw power delta(unit: 0.5db)\n"));
	DBGPRINT(RT_DEBUG_OFF, ("===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("delta_tx_pwr_bw40_g_band = %d\n", cap->delta_tx_pwr_bw40_g_band));
	DBGPRINT(RT_DEBUG_OFF, ("delta_tx_pwr_bw40_a_band = %d\n", cap->delta_tx_pwr_bw40_a_band));
	DBGPRINT(RT_DEBUG_OFF, ("delta_tx_pwr_bw80 = %d\n", cap->delta_tx_pwr_bw80));

	/* 54Mbps target power in MAC 0x13b0[5:0] and 0x13b0[13:8] */
	DBGPRINT(RT_DEBUG_OFF, ("\n===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("54Mbps target power(unit: 0.5dbm) MAC 0x13b0[5:0] and 0x13b0[13:8]\n");
	DBGPRINT(RT_DEBUG_OFF, ("===================================\n"));
	
	RTMP_IO_READ32(ad, TX_ALC_CFG_0, &value);
	value = value & TX_ALC_CFG_0_CH_INT_0_MASK;
	DBGPRINT(RT_DEBUG_OFF, ("tx_0_pwr(0x13b0[5:0])= 0x%x\n", value));
	
	RTMP_IO_READ32(ad, TX_ALC_CFG_0, &value);
	value = (value & TX_ALC_CFG_0_CH_INT_1_MASK) >> 8); 
	DBGPRINT(RT_DEBUG_OFF, ("tx_1_pwr(0x13b0[13:8]) = 0x%x\n", value));


	/* per-rate power delta */
	DBGPRINT(RT_DEBUG_OFF, ("\n===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("per-rate power delta\n"));
	DBGPRINT(RT_DEBUG_OFF, ("===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_cck_1_2 = %d\n", cap->tx_pwr_cck_1_2));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_cck_5_11 = %d\n", cap->tx_pwr_cck_5_11));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_g_band_ofdm_6_9 = %d\n", cap->tx_pwr_g_band_ofdm_6_9));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_g_band_ofdm_12_18 = %d\n", cap->tx_pwr_g_band_ofdm_12_18));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_g_band_ofdm_24_36 = %d\n", cap->tx_pwr_g_band_ofdm_24_36));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_g_band_ofdm_48_54 = %d\n", cap->tx_pwr_g_band_ofdm_48_54));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_ht_mcs_0_1 = %d\n", cap->tx_pwr_ht_mcs_0_1));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_ht_mcs_2_3 = %d\n", cap->tx_pwr_ht_mcs_2_3));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_ht_mcs_4_5 = %d\n", cap->tx_pwr_ht_mcs_4_5));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_ht_mcs_6_7 = %d\n", cap->tx_pwr_ht_mcs_6_7));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_ht_mcs_8_9 = %d\n", cap->tx_pwr_ht_mcs_8_9));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_ht_mcs_10_11 = %d\n", cap->tx_pwr_ht_mcs_10_11));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_ht_mcs_12_13 = %d\n", cap->tx_pwr_ht_mcs_12_13));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_ht_mcs_14_15 = %d\n", cap->tx_pwr_ht_mcs_14_15));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_a_band_ofdm_6_9 = %d\n", cap->tx_pwr_a_band_ofdm_6_9));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_a_band_ofdm_12_18 = %d\n", cap->tx_pwr_a_band_ofdm_12_18));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_a_band_ofdm_24_36 = %d\n", cap->tx_pwr_a_band_ofdm_24_36));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_a_band_ofdm_48_54 = %d\n", cap->tx_pwr_a_band_ofdm_48_54));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_vht_mcs_0_1 = %d\n", cap->tx_pwr_vht_mcs_0_1));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_vht_mcs_2_3 = %d\n", cap->tx_pwr_vht_mcs_2_3));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_vht_mcs_4_5 = %d\n", cap->tx_pwr_vht_mcs_4_5));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_vht_mcs_6_7 = %d\n", cap->tx_pwr_vht_mcs_6_7));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_5g_vht_mcs_8_9 = %d\n", cap->tx_pwr_5g_vht_mcs_8_9));
	DBGPRINT(RT_DEBUG_OFF, ("tx_pwr_2g_vht_mcs_8_9 = %d\n", cap->tx_pwr_2g_vht_mcs_8_9));

	/* per rate delta in mac 0x1314 ~ 0x13DC */
	DBGPRINT(RT_DEBUG_OFF, ("\n===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("per-rate power delta in MAC 0x1314 ~ 0x13DC\n"));
	DBGPRINT(RT_DEBUG_OFF, ("===================================\n"));
	RTMP_IO_READ32(ad, TX_PWR_CFG_0, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX_PWR_CFG_0 = 0x%x\n", value)); 
	RTMP_IO_READ32(ad, TX_PWR_CFG_1, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX_PWR_CFG_1 = 0x%x\n", value)); 
	RTMP_IO_READ32(ad, TX_PWR_CFG_2, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX_PWR_CFG_2 = 0x%x\n", value)); 
	RTMP_IO_READ32(ad, TX_PWR_CFG_3, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX_PWR_CFG_3 = 0x%x\n", value)); 
	RTMP_IO_READ32(ad, TX_PWR_CFG_4, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX_PWR_CFG_4 = 0x%x\n", value)); 
	RTMP_IO_READ32(ad, TX_PWR_CFG_7, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX_PWR_CFG_7 = 0x%x\n", value)); 
	RTMP_IO_READ32(ad, TX_PWR_CFG_8, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX_PWR_CFG_8 = 0x%x\n", value)); 
	RTMP_IO_READ32(ad, TX_PWR_CFG_9, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX_PWR_CFG_9 = 0x%x\n", value)); 
		
	/* TSSI info */
	DBGPRINT(RT_DEBUG_OFF, ("\n===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("TSSI info\n"));
	DBGPRINT(RT_DEBUG_OFF, ("===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("PA type = %d\n", cap->PAType));
	DBGPRINT(RT_DEBUG_OFF, ("TSSI enable = %d\n", cap->tssi_enable));
	DBGPRINT(RT_DEBUG_OFF, ("tssi_0_slope_g_band = 0x%x\n", cap->tssi_0_slope_g_band));
	DBGPRINT(RT_DEBUG_OFF, ("tssi_1_slope_g_band = 0x%x\n", cap->tssi_1_slope_g_band));
	DBGPRINT(RT_DEBUG_OFF, ("tssi_0_offset_g_band = 0x%x\n", cap->tssi_0_offset_g_band));
	DBGPRINT(RT_DEBUG_OFF, ("tssi_1_offset_g_band = 0x%x\n", cap->tssi_1_offset_g_band));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_slope_a_band[A_BAND_GRP0_CHL] = 0x%x\n", cap->tssi_0_slope_a_band[A_BAND_GRP0_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_slope_a_band[A_BAND_GRP1_CHL] = 0x%x\n", cap->tssi_0_slope_a_band[A_BAND_GRP1_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_slope_a_band[A_BAND_GRP2_CHL] = 0x%x\n", cap->tssi_0_slope_a_band[A_BAND_GRP2_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_slope_a_band[A_BAND_GRP3_CHL] = 0x%x\n", cap->tssi_0_slope_a_band[A_BAND_GRP3_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_slope_a_band[A_BAND_GRP4_CHL] = 0x%x\n", cap->tssi_0_slope_a_band[A_BAND_GRP4_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_slope_a_band[A_BAND_GRP5_CHL] = 0x%x\n", cap->tssi_0_slope_a_band[A_BAND_GRP5_CHL]));
	
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_offset_a_band[A_BAND_GRP0_CHL] = 0x%x\n", cap->tssi_0_offset_a_band[A_BAND_GRP0_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_offset_a_band[A_BAND_GRP1_CHL] = 0x%x\n", cap->tssi_0_offset_a_band[A_BAND_GRP1_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_offset_a_band[A_BAND_GRP2_CHL] = 0x%x\n", cap->tssi_0_offset_a_band[A_BAND_GRP2_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_offset_a_band[A_BAND_GRP3_CHL] = 0x%x\n", cap->tssi_0_offset_a_band[A_BAND_GRP3_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_offset_a_band[A_BAND_GRP4_CHL] = 0x%x\n", cap->tssi_0_offset_a_band[A_BAND_GRP4_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_0_offset_a_band[A_BAND_GRP5_CHL] = 0x%x\n", cap->tssi_0_offset_a_band[A_BAND_GRP5_CHL]));
	
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_slope_a_band[A_BAND_GRP0_CHL] = 0x%x\n", cap->tssi_1_slope_a_band[A_BAND_GRP0_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_slope_a_band[A_BAND_GRP1_CHL] = 0x%x\n", cap->tssi_1_slope_a_band[A_BAND_GRP1_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_slope_a_band[A_BAND_GRP2_CHL] = 0x%x\n", cap->tssi_1_slope_a_band[A_BAND_GRP2_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_slope_a_band[A_BAND_GRP3_CHL] = 0x%x\n", cap->tssi_1_slope_a_band[A_BAND_GRP3_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_slope_a_band[A_BAND_GRP4_CHL] = 0x%x\n", cap->tssi_1_slope_a_band[A_BAND_GRP4_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_slope_a_band[A_BAND_GRP5_CHL] = 0x%x\n", cap->tssi_1_slope_a_band[A_BAND_GRP5_CHL]));
	
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_offset_a_band[A_BAND_GRP0_CHL] = 0x%x\n", cap->tssi_1_offset_a_band[A_BAND_GRP0_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_offset_a_band[A_BAND_GRP1_CHL] = 0x%x\n", cap->tssi_1_offset_a_band[A_BAND_GRP1_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_offset_a_band[A_BAND_GRP2_CHL] = 0x%x\n", cap->tssi_1_offset_a_band[A_BAND_GRP2_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_offset_a_band[A_BAND_GRP3_CHL] = 0x%x\n", cap->tssi_1_offset_a_band[A_BAND_GRP3_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_offset_a_band[A_BAND_GRP4_CHL] = 0x%x\n", cap->tssi_1_offset_a_band[A_BAND_GRP4_CHL]));
	DBGPRINT(RT_DEBUG_OFF, ("cap->tssi_1_offset_a_band[A_BAND_GRP5_CHL] = 0x%x\n", cap->tssi_1_offset_a_band[A_BAND_GRP5_CHL]));
	
	/* TSSI compensation value */
	DBGPRINT(RT_DEBUG_OFF, ("\n===================================\n"));
	DBGPRINT(RT_DEBUG_OFF, ("TSSI compensation value(unit: 0.5db) in mac 0x13B4 and 0x13A8\n"));
	DBGPRINT(RT_DEBUG_OFF, ("===================================\n"));
	RTMP_IO_READ32(ad, TX_ALC_CFG_1, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX0 power compensation = 0x%x\n", value & 0x3f)); 
	RTMP_IO_READ32(ad, TX_ALC_CFG_2, &value);
	DBGPRINT(RT_DEBUG_OFF, ("TX1 power compensation = 0x%x\n", value & 0x3f)); 
}

static void mt76x2_antenna_default_reset(struct _RTMP_ADAPTER	*pAd,
										 EEPROM_ANTENNA_STRUC *pAntenna)
{
	pAntenna->word = 0;
	pAntenna->field.RfIcType = RFIC_7662;
	pAntenna->field.TxPath = 2;
	pAntenna->field.RxPath = 2;
}

#ifdef CONFIG_STA_SUPPORT
static VOID mt76x2_init_dev_nick_name(RTMP_ADAPTER *ad)
{
#ifdef RTMP_MAC_PCI
	if (IS_MT7662E(ad))
		snprintf((RTMP_STRING *) ad->nickname, sizeof(ad->nickname), "mt7662e_sta");
	else if (IS_MT7632E(ad))
		snprintf((RTMP_STRING *) ad->nickname, sizeof(ad->nickname), "mt7632e_sta");
	else if (IS_MT7612E(ad))	
		snprintf((RTMP_STRING *) ad->nickname, sizeof(ad->nickname), "mt7612e_sta");
#endif

#ifdef RTMP_MAC_USB
	if (IS_MT7662U(ad))
		snprintf((RTMP_STRING *) ad->nickname, sizeof(ad->nickname), "mt7662u_sta");
	else if (IS_MT7632U(ad))
		snprintf((RTMP_STRING *) ad->nickname, sizeof(ad->nickname), "mt7632u_sta");
	else if (IS_MT7612U(ad))	
		snprintf((RTMP_STRING *) ad->nickname, sizeof(ad->nickname), "mt7612u_sta");
#endif
}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CAL_FREE_IC_SUPPORT
static BOOLEAN mt76x2_is_cal_free_ic(RTMP_ADAPTER *ad)
{
	UINT16	NicConfig, FrequencyOffset;
	UINT16	PowerDelta, TssiSlope, TxPower;
	UINT16	EfuseValue;
	UINT	EfuseFreeBlock=0;

	eFuseGetFreeBlockCount(ad, &EfuseFreeBlock);

	if ( EfuseFreeBlock < ad->chipCap.EFUSE_RESERVED_SIZE )
		return FALSE;

	eFuseReadRegisters(ad, XTAL_TRIM1, 2, &FrequencyOffset);
	eFuseReadRegisters(ad, NIC_CONFIGURE_0, 2, &NicConfig);

	if ( !((NicConfig == 0x0) && ( FrequencyOffset != 0xFFFF ))) {
		return FALSE;
	}

	eFuseReadRegisters(ad, G_BAND_20_40_BW_PWR_DELTA, 2, &PowerDelta);
	eFuseReadRegisters(ad, TX0_G_BAND_TSSI_SLOPE, 2, &TssiSlope);

	if ( !((PowerDelta == 0x0) && ( TssiSlope != 0xFFFF ))) {
		return FALSE;
	}

	eFuseReadRegisters(ad, GRP3_TX0_A_BAND_CHL_PWR_DELTA_LOW, 2, &TxPower);
	eFuseReadRegisters(ad, GRP4_TX0_A_BAND_TSSI_SLOPE, 2, &TssiSlope);

	if ( !((TxPower == 0x0) && ( TssiSlope != 0xFFFF ))) {
		return FALSE;
	}

	return TRUE;

}


static VOID mt76x2_cal_free_data_get(RTMP_ADAPTER *ad)
{
	UINT16 value;

	DBGPRINT(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));

	/* 0x3A */
	eFuseReadRegisters(ad, XTAL_TRIM1, 2, &value);
	ad->EEPROMImage[XTAL_TRIM1] = value & 0xFF;

	/* 0x55 0x56 0x57 0x5C 0x5D */
	eFuseReadRegisters(ad, A_BAND_EXT_PA_SETTING, 2, &value);
	ad->EEPROMImage[A_BAND_EXT_PA_SETTING + 1] = (value >> 8) & 0xFF;
	eFuseReadRegisters(ad, TX0_G_BAND_TSSI_SLOPE, 2, &value);
	*(UINT16 *)(&ad->EEPROMImage[TX0_G_BAND_TSSI_SLOPE]) = value;
	eFuseReadRegisters(ad, TX1_G_BAND_TSSI_SLOPE, 2, &value);
	*(UINT16 *)(&ad->EEPROMImage[TX1_G_BAND_TSSI_SLOPE]) = value;

	/* 0x62 0x63 0x67 0x68 0x6C 0x6D */
	eFuseReadRegisters(ad, GRP0_TX0_A_BAND_TSSI_SLOPE, 2, &value);
	if ( value != 0 )
		*(UINT16 *)(&ad->EEPROMImage[GRP0_TX0_A_BAND_TSSI_SLOPE]) = value;
	eFuseReadRegisters(ad, GRP0_TX0_A_BAND_CHL_PWR_DELTA_HI, 2, &value);
	ad->EEPROMImage[GRP0_TX0_A_BAND_CHL_PWR_DELTA_HI + 1] = (value & GRP1_TX0_A_BAND_TSSI_SLOPE_MASK) >> 8;
	eFuseReadRegisters(ad, GRP1_TX0_A_BAND_TSSI_OFFSET, 2, &value);
	ad->EEPROMImage[GRP1_TX0_A_BAND_TSSI_OFFSET] = value & GRP1_TX0_A_BAND_TSSI_OFFSET_MASK;
	eFuseReadRegisters(ad, GRP2_TX0_A_BAND_TSSI_SLOPE, 2, &value);
	*(UINT16 *)(&ad->EEPROMImage[GRP2_TX0_A_BAND_TSSI_SLOPE]) = value;

	/* 0x71 0x72 0x76 0x77 0x7B 0x7C */
	eFuseReadRegisters(ad, GRP2_TX0_A_BAND_CHL_PWR_DELTA_HI, 2, &value);
	ad->EEPROMImage[GRP2_TX0_A_BAND_CHL_PWR_DELTA_HI + 1] = (value & GRP3_TX0_A_BAND_TSSI_SLOPE_MASK) >> 8;
	eFuseReadRegisters(ad, GRP3_TX0_A_BAND_TSSI_OFFSET, 2, &value);
	ad->EEPROMImage[GRP3_TX0_A_BAND_TSSI_OFFSET] = value & GRP3_TX0_A_BAND_TSSI_OFFSET_MASK;
	eFuseReadRegisters(ad, GRP4_TX0_A_BAND_TSSI_SLOPE, 2, &value);
	*(UINT16 *)(&ad->EEPROMImage[GRP4_TX0_A_BAND_TSSI_SLOPE]) = value;
	eFuseReadRegisters(ad, GRP4_TX0_A_BAND_CHL_PWR_DELTA_HI, 2, &value);
	ad->EEPROMImage[GRP4_TX0_A_BAND_CHL_PWR_DELTA_HI + 1] = (value & GRP5_TX0_A_BAND_TSSI_SLOPE_MASK) >> 8;
	eFuseReadRegisters(ad, GRP5_TX0_A_BAND_TSSI_OFFSET, 2, &value);
	ad->EEPROMImage[GRP5_TX0_A_BAND_TSSI_OFFSET] = value & GRP5_TX0_A_BAND_TSSI_OFFSET_MASK;

	/* 0x80 0x81 0x85 0x86 0x8A 0x8B 0x8F */
	eFuseReadRegisters(ad, GRP0_TX1_A_BAND_TSSI_SLOPE, 2, &value);
	if ( value != 0 )
		*(UINT16 *)(&ad->EEPROMImage[GRP0_TX1_A_BAND_TSSI_SLOPE]) = value;
	eFuseReadRegisters(ad, GRP0_TX1_A_BAND_CHL_PWR_DELTA_HI, 2, &value);
	ad->EEPROMImage[GRP0_TX1_A_BAND_CHL_PWR_DELTA_HI + 1] = (value & GRP1_TX1_A_BAND_TSSI_SLOPE_MASK) >> 8;
	eFuseReadRegisters(ad, GRP1_TX1_A_BAND_TSSI_OFFSET, 2, &value);
	ad->EEPROMImage[GRP1_TX1_A_BAND_TSSI_OFFSET] = value & GRP1_TX1_A_BAND_TSSI_OFFSET_MASK;
	eFuseReadRegisters(ad, GRP2_TX1_A_BAND_TSSI_SLOPE, 2, &value);
	*(UINT16 *)(&ad->EEPROMImage[GRP2_TX1_A_BAND_TSSI_SLOPE]) = value;
	eFuseReadRegisters(ad, GRP2_TX1_A_BAND_CHL_PWR_DELTA_HI, 2, &value);
	ad->EEPROMImage[GRP2_TX1_A_BAND_CHL_PWR_DELTA_HI + 1] = (value & GRP3_TX1_A_BAND_TSSI_SLOPE_MASK) >> 8;

	/* 0x90 0x94 0x95 0x99 0x9A */
	eFuseReadRegisters(ad, GRP3_TX1_A_BAND_TSSI_OFFSET, 2, &value);
	ad->EEPROMImage[GRP3_TX1_A_BAND_TSSI_OFFSET] = value & GRP3_TX1_A_BAND_TSSI_OFFSET_MASK;
	eFuseReadRegisters(ad, GRP4_TX1_A_BAND_TSSI_SLOPE, 2, &value);
	*(UINT16 *)(&ad->EEPROMImage[GRP4_TX1_A_BAND_TSSI_SLOPE]) = value;
	eFuseReadRegisters(ad, GRP4_TX1_A_BAND_CHL_PWR_DELTA_HI, 2, &value);
	ad->EEPROMImage[GRP4_TX1_A_BAND_CHL_PWR_DELTA_HI + 1] = (value & GRP5_TX1_A_BAND_TSSI_SLOPE_MASK) >> 8;
	eFuseReadRegisters(ad, GRP5_TX1_A_BAND_TSSI_OFFSET, 2, &value);
	ad->EEPROMImage[GRP5_TX1_A_BAND_TSSI_OFFSET] = value & GRP5_TX1_A_BAND_TSSI_OFFSET_MASK;

	/* 0xF9 0xFA 0xFB 0xFC 0xFD 0xFE 0xFF */
	eFuseReadRegisters(ad, RF_2G_RX_HIGH_GAIN, 2, &value);
	ad->EEPROMImage[RF_2G_RX_HIGH_GAIN+1] = (value >> 8) & 0xFF;
	eFuseReadRegisters(ad, RF_5G_GRP0_1_RX_HIGH_GAIN, 2, &value);
	*(UINT16 *)(&ad->EEPROMImage[RF_5G_GRP0_1_RX_HIGH_GAIN]) = value;
	eFuseReadRegisters(ad, RF_5G_GRP2_3_RX_HIGH_GAIN, 2, &value);
	*(UINT16 *)(&ad->EEPROMImage[RF_5G_GRP2_3_RX_HIGH_GAIN]) = value;
	eFuseReadRegisters(ad, RF_5G_GRP4_5_RX_HIGH_GAIN, 2, &value);
	*(UINT16 *)(&ad->EEPROMImage[RF_5G_GRP4_5_RX_HIGH_GAIN]) = value;

	/* BT: 0x138 0x13D 0x13E 0x13F */
	eFuseReadRegisters(ad, BT_RCAL_RESULT, 2, &value);
	if ( value != 0xFFFF )
		ad->EEPROMImage[BT_RCAL_RESULT] = value & 0xFF;
	eFuseReadRegisters(ad, BT_VCDL_CALIBRATION, 2, &value);
	if ( value != 0xFFFF )
		ad->EEPROMImage[BT_VCDL_CALIBRATION+1] = (value >> 8) & 0xFF;
	eFuseReadRegisters(ad, BT_PMUCFG, 2, &value);
	if ( value != 0xFFFF )
		*(UINT16 *)(&ad->EEPROMImage[BT_PMUCFG]) = value;

}
#endif /* CAL_FREE_IC_SUPPORT */

VOID mt76x2_antenna_sel_ctl(
	IN RTMP_ADAPTER *ad)
{
/* Antenna selection control in 76x2 is controlled by fw/rom_patch */
#if 0
	USHORT e2p_val = 0;
	UINT32 WlanFunCtrl = 0, CmbCtrl = 0, CoexCfg0 = 0, CoexCfg3 = 0;
	UINT32 ret;
	RTMP_CHIP_CAP *cap = &ad->chipCap;

	DBGPRINT(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));

#ifdef RTMP_MAC_PCI
	RTMP_SEM_LOCK(&ad->WlanEnLock);
#endif

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(ad)) {
		RTMP_SEM_EVENT_WAIT(&ad->hw_atomic, ret);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
			return STATUS_UNSUCCESSFUL;
		}
	}
#endif /* RTMP_MAC_USB */

	RTMP_IO_READ32(ad, WLAN_FUN_CTRL, &WlanFunCtrl);
	RTMP_IO_READ32(ad, CMB_CTRL, &CmbCtrl);
	RTMP_IO_READ32(ad, COEXCFG0, &CoexCfg0);
	RTMP_IO_READ32(ad, COEXCFG3, &CoexCfg3);

	/* Check with about this bit, follow 7650 first */
	CoexCfg0 &= ~COEXCFG0_FIX_WL_ANT_EN;

	/* TR_SW control setting */
	CoexCfg3 &= ~COEXCFG3_CSR_FRC_TR_SW0;

	/* BTWL_SW control setting */
	CmbCtrl	&= ~CMB_CTRL_AUX_OPT_ANTSEL;
	
	WlanFunCtrl &= ~WLAN_FUN_CTRL_INV_ANT_SEL;
	
	if (cap->IsComboChip) {
		CoexCfg3 &= ~(COEXCFG3_FIX_IO_ANT_SEL_EN | COEXCFG3_REG_IO_ANT_SEL_EN);
		CoexCfg3 |= (COEXCFG3_TDM_EN | COEXCFG3_COEX_VER); 
	} else {
		CoexCfg3 &= ~(COEXCFG3_REG_IO_ANT_SEL_EN | COEXCFG3_TDM_EN);
		CoexCfg3 |= (COEXCFG3_FIX_IO_ANT_SEL_EN | COEXCFG3_COEX_VER); 
	}
	
	RTMP_IO_WRITE32(ad, WLAN_FUN_CTRL, WlanFunCtrl);
	RTMP_IO_WRITE32(ad, CMB_CTRL, CmbCtrl);
	RTMP_IO_WRITE32(ad, COEXCFG0, CoexCfg0);
	RTMP_IO_WRITE32(ad, COEXCFG3, CoexCfg3);

	/* PTA back to inital sequence */
	write_reg(ad, 0x40, 0xf4, 0x00000003);
	write_reg(ad, 0x40, 0xf4, 0x80800003);
	write_reg(ad, 0x40, 0xf4, 0x00000003);
	write_reg(ad, 0x40, 0xf4, 0x00000000);

#ifdef RTMP_MAC_PCI
	RTMP_SEM_UNLOCK(&ad->WlanEnLock);
#endif

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(ad)) {
		RTMP_SEM_EVENT_UP(&ad->hw_atomic);
	}
#endif /* RTMP_MAC_USB */
#endif
}

static const RTMP_CHIP_CAP MT76x2_ChipCap = {
	.max_nss = 2,
#ifdef DOT11_VHT_AC
	.max_vht_mcs = VHT_MCS_CAP_9,
#endif
	.TXWISize = 20,
	.RXWISize = 28,
#ifdef RTMP_MAC_PCI
	.WPDMABurstSIZE = 3,
#endif
	.SnrFormula = SNR_FORMULA3,
	.FlgIsHwWapiSup = TRUE,
	.VcoPeriod = 10,
	.FlgIsVcoReCalMode = VCO_CAL_DISABLE,
	.FlgIsHwAntennaDiversitySup = FALSE,
	.Flg7662ChipCap = TRUE,
#ifdef STREAM_MODE_SUPPORT
	.FlgHwStreamMode = FALSE,
#endif
#ifdef TXBF_SUPPORT
	.FlgHwTxBfCap = FALSE,
	.FlgITxBfBinWrite = FALSE,
#endif
#ifdef FIFO_EXT_SUPPORT
	.FlgHwFifoExtCap = TRUE,
#endif
#ifdef CONFIG_CSO_SUPPORT
	.asic_caps |= fASIC_CAP_CSO,
#endif
#ifdef CONFIG_TSO_SUPPORT
	.asic_caps |= fASIC_CAP_TSO,
#endif
	.asic_caps = (fASIC_CAP_PMF_ENC | fASIC_CAP_MCS_LUT),
	.phy_caps = (fPHY_CAP_24G | fPHY_CAP_5G | fPHY_CAP_HT | fPHY_CAP_VHT | fPHY_CAP_LDPC),
	.MaxNumOfRfId = MAX_RF_ID,
	.pRFRegTable = NULL,
	.MaxNumOfBbpId = 200,
	.pBBPRegTable = NULL,
	.bbpRegTbSize = 0,
#ifdef DFS_SUPPORT
	.DfsEngineNum = 4,
#endif
#ifdef CARRIER_DETECTION_SUPPORT
	.carrier_func = TONE_RADAR_V3,
#endif /* CARRIER_DETECTION_SUPPORT */
#ifdef NEW_MBSSID_MODE
#ifdef ENHANCE_NEW_MBSSID_MODE
	.MBSSIDMode = MBSSID_MODE4,
#else
	.MBSSIDMode = MBSSID_MODE1,
#endif /* ENHANCE_NEW_MBSSID_MODE */
#else
	.MBSSIDMode = MBSSID_MODE0,
#endif /* NEW_MBSSID_MODE */
#ifdef RTMP_EFUSE_SUPPORT
	.EFUSE_USAGE_MAP_START = 0x1e0,
	.EFUSE_USAGE_MAP_END = 0x1fc,
	.EFUSE_USAGE_MAP_SIZE = 29,
	.EFUSE_RESERVED_SIZE = 21,	// Cal-Free is 22 free block
#endif
	.EEPROM_DEFAULT_BIN = MT76x2_E2PImage,
	.EEPROM_DEFAULT_BIN_SIZE = sizeof(MT76x2_E2PImage),
#ifdef CONFIG_ANDES_SUPPORT
	.WlanMemmapOffset = 0x410000,
	.InbandPacketMaxLen = 192,
	.CmdRspRxRing = RX_RING1,
	.IsComboChip = TRUE,
	.need_load_fw = TRUE,
	.need_load_rom_patch = TRUE,
	.ram_code_protect = FALSE,
	.rom_code_protect = TRUE,
	.load_iv = FALSE,
	.ilm_offset = 0x80000,
	.dlm_offset = 0x110000,
	.rom_patch_offset = 0x90000,
#endif
	.MCUType = ANDES,
	.cmd_header_len = 4,
#ifdef RTMP_PCI_SUPPORT
	.cmd_padding_len = 0,
#endif
#ifdef RTMP_USB_SUPPORT
	.cmd_padding_len = 4,
	.CommandBulkOutAddr = 0x8,
	.WMM0ACBulkOutAddr[0] = 0x4,
	.WMM0ACBulkOutAddr[1] = 0x5,
	.WMM0ACBulkOutAddr[2] = 0x6,
	.WMM0ACBulkOutAddr[3] = 0x7,
	.WMM1ACBulkOutAddr	= 0x9,
	.DataBulkInAddr = 0x84,
	.CommandRspBulkInAddr = 0x85, 
#endif
	.fw_header_image = MT7662_FirmwareImage,
	.fw_bin_file_name = "mtk/WIFI_RAM_CODE_ALL.bin",
	.fw_len = sizeof(MT7662_FirmwareImage),
	.rom_patch_header_image = mt7662_rom_patch,
	.rom_patch_len = sizeof(mt7662_rom_patch),
	.rom_patch_bin_file_name = "mtk/mt7662_patch_e1_hdr.bin",
	.load_code_method = HEADER_METHOD,
	.MACRegisterVer = "",
	.BBPRegisterVer = "MT7662E1_BBP_CR_20130116.xls",
	.RFRegisterVer = "",
#ifdef CONFIG_WIFI_TEST
	.MemMapStart = 0x0000,
	.MemMapEnd = 0xffff,
	.MacMemMapOffset = 0x1000,
	.MacStart = 0x0000,
	.MacEnd = 0x0600,
	.BBPMemMapOffset = 0x2000,
	.BBPStart = 0x0000,
	.BBPEnd = 0x0f00,
	.RFIndexNum = sizeof(mt76x2_rf_index_offset) / sizeof(struct RF_INDEX_OFFSET),
	.RFIndexOffset = mt76x2_rf_index_offset,
	.E2PStart = 0x0000,
	.E2PEnd = 0x00fe,
#endif
	.hif_type = HIF_RLT,
	.rf_type = RF_MT,
#ifdef DYNAMIC_VGA_SUPPORT
	.dynamic_vga_support = TRUE,
#endif
	.chl_smth_enable = TRUE,
};

static const RTMP_CHIP_OP MT76x2_ChipOp = {
	.ChipBBPAdjust = mt76x2_bbp_adjust,
	.ChipSwitchChannel = mt76x2_switch_channel,
	.AsicMacInit = mt76x2_init_mac_cr,
	.AsicRfInit = mt76x2_init_rf_cr,
	.AsicAntennaDefaultReset = mt76x2_antenna_default_reset,

	// ITxBf phase calibration
#ifdef TXBF_SUPPORT	
	.fITxBfDividerCalibration = ITxBFDividerCalibrationStartUp,
	.fITxBfLNAPhaseCompensate = mt76x2_ITxBFLoadLNAComp,
	.fITxBfCal = mt76x2_Set_ITxBfCal_Proc,
	.fITxBfLNACalibration = ITxBFLNACalibrationStartUp,
#endif
	
	.ChipAGCInit = NULL,
#ifdef CONFIG_STA_SUPPORT
	.ChipAGCAdjust = NULL,
#endif
	.AsicRfTurnOn = NULL,
	.AsicHaltAction = NULL,
	.AsicRfTurnOff = NULL,
	.AsicReverseRfFromSleepMode = NULL,
	.AsicResetBbpAgent = NULL,
	.InitDesiredTSSITable = NULL,
 	.ATETssiCalibration = NULL,
	.ATETssiCalibrationExtend = NULL,
	.AsicTxAlcGetAutoAgcOffset = NULL,
	.ATEReadExternalTSSI = NULL,
	.TSSIRatio = NULL,
#ifdef CONFIG_STA_SUPPORT
	.NetDevNickNameInit = mt76x2_init_dev_nick_name,
#endif
#ifdef CARRIER_DETECTION_SUPPORT
	.ToneRadarProgram = ToneRadarProgram_v3,
#endif 
	.RxSensitivityTuning = NULL,
	.DisableTxRx = RT65xxDisableTxRx,
#ifdef RTMP_USB_SUPPORT
	.AsicRadioOn = RT65xxUsbAsicRadioOn,
	.AsicRadioOff = RT65xxUsbAsicRadioOff,
	.usb_cfg_read = usb_cfg_read_v3,
	.usb_cfg_write = usb_cfg_write_v3,
#endif
#ifdef RTMP_PCI_SUPPORT
	.AsicRadioOn = RT28xxPciAsicRadioOn,
	.AsicRadioOff = RT28xxPciAsicRadioOff,
#endif
#ifdef CAL_FREE_IC_SUPPORT
	.is_cal_free_ic = mt76x2_is_cal_free_ic,
	.cal_free_data_get = mt76x2_cal_free_data_get,
#endif /* CAL_FREE_IC_SUPPORT */
	.show_pwr_info = mt76x2_show_pwr_info,
	.cal_test = mt76x2_cal_test,
};

VOID mt76x2_init(RTMP_ADAPTER *pAd)
{
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;

	memcpy(&pAd->chipCap, &MT76x2_ChipCap, sizeof(RTMP_CHIP_CAP));
	memcpy(&pAd->chipOps, &MT76x2_ChipOp, sizeof(RTMP_CHIP_OP));

	rlt_phy_probe(pAd);

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

	if (IS_MT7612(pAd)) {
		pChipCap->IsComboChip = FALSE;
		pChipCap->rom_code_protect = FALSE;
	}

#ifdef CONFIG_CSO_SUPPORT
	pChipCap->asic_caps |= fASIC_CAP_CSO,
#endif

#ifdef CONFIG_TSO_SUPPORT
	pChipCap->asic_caps |= fASIC_CAP_TSO,
#endif	

	pChipCap->asic_caps |= (fASIC_CAP_MCS_LUT);

	RTMP_DRS_ALG_INIT(pAd, RATE_ALG_GRP);
		
	rlt_bcn_buf_init(pAd);

#ifdef HDR_TRANS_SUPPORT
	UINT8 cnt = HT_RX_WCID_SIZE/HT_RX_WCID_OFFSET;
	UINT32 RegVal;

	/* enable TX/RX Header Translation */
	RTMP_IO_WRITE32(pAd, HT_RX_WCID_EN_BASE , 0xFF);	/* all RX WCID enable */

	/* black list - skip EAP-888e/DLS-890d */
	RTMP_IO_WRITE32(pAd, HT_RX_BL_BASE, 0x888e890d);
#endif /* HDR_TRANS_SUPPORT */

	pChipCap->tssi_stage = TSSI_TRIGGER_STAGE;
}

static void patch_BBPL_on(RTMP_ADAPTER *pAd)
{
	UINT32 value = 0;
    
	read_reg(pAd, 0x40, 0x130, &value);
    value |= ((1<<16) | (1<<0));
    write_reg(pAd, 0x40, 0x130, value);
    
    RtmpusecDelay(1); 
    
    read_reg(pAd, 0x40, 0x64, &value);
    if ((value >> 29) & 0x1)
    {
        read_reg(pAd, 0x40, 0x1c, &value);
    	value &= 0xFFFFFF00;
    	write_reg(pAd, 0x40, 0x1c, value);
    	
        read_reg(pAd, 0x40, 0x1c, &value);
    	value |= 0x30;
    	write_reg(pAd, 0x40, 0x1c, value);
    }
    else
    {
        read_reg(pAd, 0x40, 0x1c, &value);
    	value &= 0xFFFFFF00;
    	write_reg(pAd, 0x40, 0x1c, value);
    	
        read_reg(pAd, 0x40, 0x1c, &value);
    	value |= 0x30;
    	write_reg(pAd, 0x40, 0x1c, value);
    }
    value = 0x0000484F;
    write_reg(pAd, 0x40, 0x14, value);
    	
    RtmpusecDelay(1); 
    
    read_reg(pAd, 0x40, 0x130, &value);
    value |= (1<<17);
    write_reg(pAd, 0x40, 0x130, value);
    
    RtmpusecDelay(125); 
    
    read_reg(pAd, 0x40, 0x130, &value);
    value  &= ~(1<<16);
    write_reg(pAd, 0x40, 0x130, value);
    
    RtmpusecDelay(50); 
    
    read_reg(pAd, 0x40, 0x14C, &value);
    value  |= ((1<<20) | (1<<19));
    write_reg(pAd, 0x40, 0x14C, value);
}

static VOID WF_CTRL(RTMP_ADAPTER *pAd, UINT8 wfID, UINT8 isON)
{
	UINT32 value = 0;
    if(wfID == 0)
    {
        if(isON == 1) /* WIFI ON mode */
        {
            /* Enable WF0 BG */
    		read_reg(pAd, 0x40, 0x130, &value);
    		value |= (1<<0);
    		write_reg(pAd, 0x40, 0x130, value);
    		
            RtmpusecDelay(10); 
            
            /* Enable RFDIG LDO/AFE/ABB/ADDA */
            read_reg(pAd, 0x40, 0x130, &value);
    		value |= ((1<<1)|(1<<3)|(1<<4)|(1<<5));
    		write_reg(pAd, 0x40, 0x130, value);
    		
            RtmpusecDelay(10); 
            
            /* Switch WF0 RFDIG power to internal LDO */
            read_reg(pAd, 0x40, 0x130, &value);
    		value &= ~(1<<2);
    		write_reg(pAd, 0x40, 0x130, value);
    		
            patch_BBPL_on(pAd);
            
            RTMP_IO_READ32(pAd, 0x530, &value);
    		value  |= 0xF;
    		RTMP_IO_WRITE32(pAd, 0x530, value);
        }
    }
    else
    {
        if(isON == 1) /* WIFI ON mode */
        {
            /* Enable WF1 BG */
            read_reg(pAd, 0x40, 0x130, &value);
    		value |= (1<<8);
    		write_reg(pAd, 0x40, 0x130, value);
    		
            RtmpusecDelay(10); 
            
            /* Enable RFDIG LDO/AFE/ABB/ADDA */
            read_reg(pAd, 0x40, 0x130, &value);
    		value |= ((1<<9)|(1<<11)|(1<<12)|(1<<13)); 
    		write_reg(pAd, 0x40, 0x130, value);
    		
            RtmpusecDelay(10); 
            /* Switch WF1 RFDIG power to internal LDO */
            read_reg(pAd, 0x40, 0x130, &value);
    		value &= ~(1<<10);
    		write_reg(pAd, 0x40, 0x130, value);
    		
            patch_BBPL_on(pAd);
            
            RTMP_IO_READ32(pAd, 0x530, &value);
    		value  |= 0xF;
    		RTMP_IO_WRITE32(pAd, 0x530, value);
        }
    }
}

static void WL_POWER_ON(RTMP_ADAPTER *pAd)
{
    UINT32 cnt = 0;
    UINT32 regval = 0;
    UINT32 value = 0;
    
    read_reg(pAd, 0x40, 0x148, &value);
    value |= 0x1;
    write_reg(pAd, 0x40, 0x148, value); // turn on WL MTCMOS
    do{
        read_reg(pAd, 0x40, 0x148, &regval);
        if((((regval>>28) & 0x1) == 0x1) && (((regval>>12) & 0x3) == 0x3))
            break;
        RtmpusecDelay(10); 
        cnt++;
    }while(cnt < 100);

    read_reg(pAd, 0x40, 0x148, &value);
    value &= ~(0x7F<<16);
    write_reg(pAd, 0x40, 0x148, value);
    
    RtmpusecDelay(10); 
    read_reg(pAd, 0x40, 0x148, &value);
    value &= ~(0xF<<24);
    write_reg(pAd, 0x40, 0x148, value);
    RtmpusecDelay(10); 
    
    read_reg(pAd, 0x40, 0x148, &value);
    value |= (0xF<<24);
    write_reg(pAd, 0x40, 0x148, value);
    
    read_reg(pAd, 0x40, 0x148, &value);
    value &= ~(0xFFF);
    write_reg(pAd, 0x40, 0x148, value);
    
    /* Set 1'b0 to turn on AD/DA power down */
    read_reg(pAd, 0x40, 0x1204, &value);
    value &= ~(0x1<<3);
    write_reg(pAd, 0x40, 0x1204, value);
    
    /* WLAN function enable */
    read_reg(pAd, 0x40, 0x80, &value);
    value |= (0x1<<0);
    write_reg(pAd, 0x40, 0x80, value);
    
    /* release "BBP software reset */
    read_reg(pAd, 0x40, 0x64, &value);
    value &= ~(0x1<<18);
    write_reg(pAd, 0x40, 0x64, value);
}

void mt76x2_pwrOn(RTMP_ADAPTER *pAd)
{
    /* Radio On */
    DBGPRINT(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));
    WL_POWER_ON(pAd);
    WF_CTRL(pAd, 0, 1);
    WF_CTRL(pAd, 1, 1);
}


int mt76x2_set_ed_cca(RTMP_ADAPTER *ad, UINT8 enable)
{
        UINT32 mac_val;
        UINT32 bbp_val;

        if (enable) {
                RTMP_IO_READ32(ad, CH_TIME_CFG, &mac_val);
                mac_val |= 0x05; // enable channel status check
                RTMP_IO_WRITE32(ad, CH_TIME_CFG, mac_val);

                // BBP: latched ED_CCA and high/low threshold
                RTMP_BBP_IO_READ32(ad, AGC1_R2, &bbp_val);
                //bbp_val &= 0xFFFF;
                bbp_val = 0x80000808;
                RTMP_BBP_IO_WRITE32(ad, AGC1_R2, bbp_val);

                // MAC: enable ED_CCA/ED_2nd_CCA
                RTMP_IO_READ32(ad, TXOP_CTRL_CFG, &mac_val);
                mac_val |= ((1<<20) | (1<<7));
                RTMP_IO_WRITE32(ad, TXOP_CTRL_CFG, mac_val);

                RTMP_IO_READ32(ad, TXOP_HLDR_ET, &mac_val);
                mac_val |= 2;
                RTMP_IO_WRITE32(ad, TXOP_HLDR_ET, mac_val);
        }
        else
        {
                // MAC: disable ED_CCA/ED_2nd_CCA
                RTMP_IO_READ32(ad, TXOP_CTRL_CFG, &mac_val);
                mac_val &= (~((1<<20) | (1<<7)));
                RTMP_IO_WRITE32(ad, TXOP_CTRL_CFG, mac_val);

                RTMP_IO_READ32(ad, TXOP_HLDR_ET, &mac_val);
                mac_val &= ~2;
                RTMP_IO_WRITE32(ad, TXOP_HLDR_ET, mac_val);
        }

        /* Clear previous status */
        RTMP_IO_READ32(ad, CH_IDLE_STA, &mac_val);
        RTMP_IO_READ32(ad, CH_BUSY_STA, &mac_val);
        RTMP_IO_READ32(ad, CH_BUSY_STA_SEC, &mac_val);
        RTMP_IO_READ32(ad, 0x1140, &mac_val);

        return TRUE;
}

