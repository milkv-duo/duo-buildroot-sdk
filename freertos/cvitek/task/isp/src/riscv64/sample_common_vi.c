#include <stdio.h>

#include <cvi_type.h>
#include <cvi_defines.h>
#include "cvi_comm_video.h"

#include "cvi_awb_comm.h"
#include "cvi_af_comm.h"
#include "cvi_comm_isp.h"
#include "cvi_comm_vi.h"
#include "sample_comm.h"
#include "cvi_sns_ctrl.h"

#include "cvi_common.h"
#include "cvi_ae_comm.h"

#include "cvi_mipi.h"

const char *snsr_type_name[SAMPLE_SNS_TYPE_BUTT] = {
	/* ------ LINEAR BEGIN ------*/
	"SONY_IMX290_MIPI_1M_30FPS_12BIT",
	"SONY_IMX290_MIPI_2M_60FPS_12BIT",
	"SONY_IMX327_MIPI_1M_30FPS_10BIT",
	"SONY_IMX327_MIPI_2M_30FPS_12BIT",
	"SONY_IMX307_MIPI_2M_30FPS_12BIT",
	"SONY_IMX327_2L_MIPI_2M_30FPS_12BIT",
	"SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT",
	"SONY_IMX307_2L_MIPI_2M_30FPS_12BIT",
	"SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT",
	"OV_OS08A20_MIPI_8M_30FPS_10BIT",
	"OV_OS08A20_MIPI_5M_30FPS_10BIT",
	"OV_OS08A20_MIPI_4M_30FPS_10BIT",
	"OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT",
	"OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT",
	"OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT",
	"OV_OS04C10_MIPI_4M_30FPS_12BIT",
	"OV_OS04C10_SLAVE_MIPI_4M_30FPS_12BIT",
	"SOI_F23_MIPI_2M_30FPS_10BIT",
	"SOI_F35_MIPI_2M_30FPS_10BIT",
	"SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT",
	"SOI_H65_MIPI_2M_30FPS_10BIT",
	"PICO640_THERMAL_479P",
	"PICO384_THERMAL_384X288",
	"SONY_IMX327_SUBLVDS_2M_30FPS_12BIT",
	"SONY_IMX307_SUBLVDS_2M_30FPS_12BIT",
	"VIVO_MCS369Q_4M_30FPS_12BIT",
	"VIVO_MM308M2_2M_25FPS_8BIT",
	"NEXTCHIP_N5_2M_25FPS_8BIT",
	"SMS_SC3335_MIPI_3M_30FPS_10BIT",
	"SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT",
	"SONY_IMX335_MIPI_5M_30FPS_12BIT",
	"SONY_IMX335_MIPI_4M_30FPS_12BIT",
	"SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT",
	"SONY_IMX335_2L_MIPI_4M_30FPS_10BIT",
	"PIXELPLUS_PR2020_1M_25FPS_8BIT",
	"SONY_IMX385_MIPI_2M_30FPS_12BIT",
	"PIXELPLUS_PR2020_1M_30FPS_8BIT",
	"PIXELPLUS_PR2020_2M_25FPS_8BIT",
	"PIXELPLUS_PR2020_2M_30FPS_8BIT",
	"PIXELPLUS_PR2100_2M_25FPS_8BIT",
	"PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT",
	"PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT",
	"SONY_IMX334_MIPI_8M_30FPS_12BIT",
	"SMS_SC8238_MIPI_8M_30FPS_10BIT",
	"VIVO_MCS369_2M_30FPS_12BIT",
	"SMS_SC4210_MIPI_4M_30FPS_12BIT",
	"SMS_SC200AI_MIPI_2M_30FPS_10BIT",
	"SMS_SC850SL_MIPI_8M_30FPS_12BIT",
	"NEXTCHIP_N6_2M_4CH_25FPS_8BIT",
	"NEXTCHIP_N5_1M_2CH_25FPS_8BIT",
	"GCORE_GC2053_MIPI_2M_30FPS_10BIT",
	"GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT",
	"GCORE_GC2093_MIPI_2M_30FPS_10BIT",
	"GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT",
	"GCORE_GC4653_MIPI_4M_30FPS_10BIT",
	"GCORE_GC4653_SLAVE_MIPI_4M_30FPS_10BIT",
	"SONY_IMX335_MIPI_5M_60FPS_10BIT",
	"SONY_IMX335_MIPI_4M_60FPS_10BIT",
	"GCORE_GC1054_MIPI_1M_30FPS_10BIT",
	"SONY_IMX327_MIPI_2M_60FPS_12BIT",
	"SONY_IMX347_MIPI_4M_60FPS_12BIT",
	"SONY_IMX307_SUBLVDS_2M_60FPS_12BIT",
	"SONY_IMX307_MIPI_2M_60FPS_12BIT",
	"GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT",
	"SONY_IMX335_MIPI_2M_60FPS_10BIT",
	"SMS_SC035HGS_MIPI_480P_120FPS_12BIT",
	"SMS_SC500AI_MIPI_5M_30FPS_10BIT",
	"SMS_SC500AI_MIPI_4M_30FPS_10BIT",
	"SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT",
	"SMS_SC401AI_MIPI_4M_30FPS_10BIT",
	"SMS_SC401AI_MIPI_3M_30FPS_10BIT",
	"SMS_SC035GS_MIPI_480P_120FPS_12BIT",
	"TECHPOINT_TP2850_MIPI_2M_30FPS_8BIT",
	"TECHPOINT_TP2850_MIPI_4M_30FPS_8BIT",
	/* ------ LINEAR END ------*/

	/* ------ WDR 2TO1 BEGIN ------*/
	"SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1",
	"OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"OV_OS04C10_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"OV_OS04C10_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1",
	"SONY_IMX335_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1",
	"SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1",
	"SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1",
	"SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1",
	"SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"SMS_SC850SL_MIPI_8M_30FPS_10BIT_WDR2TO1",
	"GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1",
	"SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1",
	/* ------ WDR 2TO1 END ------*/
};

// default is MIPI-CSI Bayer format sensor
VI_DEV_ATTR_S DEV_ATTR_SENSOR_BASE = {
	VI_MODE_MIPI,
	VI_WORK_MODE_1Multiplex,
	VI_SCAN_PROGRESSIVE,
	{-1, -1, -1, -1},
	VI_DATA_SEQ_YUYV,

	{
	/*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
	VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH,
	VI_VSYNC_VALID_SIGNAL, VI_VSYNC_VALID_NEG_HIGH,

	/*hsync_hfb    hsync_act    hsync_hhb*/
	{0,            1920,        0,
	/*vsync0_vhb vsync0_act vsync0_hhb*/
	 0,            1080,        0,
	/*vsync1_vhb vsync1_act vsync1_hhb*/
	 0,            0,            0}
	},
	VI_DATA_TYPE_RGB,
	{1920, 1080, 0, 0, 1920, 1080},
	{
		WDR_MODE_NONE,
		1080
	},
	.enBayerFormat = BAYER_FORMAT_BG,
	1, 30,
};

CVI_S32 SAMPLE_COMM_VI_StartIsp(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32 s32Ret = 0, i;
	VI_PIPE ViPipe = 0;
	ISP_PUB_ATTR_S stPubAttr;
	ISP_STATISTICS_CFG_S stsCfg = {0};
	ISP_BIND_ATTR_S stBindAttr;

	for (i = 0; i < WDR_MAX_PIPE_NUM; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  &&
			pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			ViPipe = pstViInfo->stPipeInfo.aPipe[0];

			SAMPLE_COMM_ISP_Aelib_Callback(ViPipe);
			SAMPLE_COMM_ISP_Awblib_Callback(ViPipe);
			// #if ENABLE_AF_LIB
			// SAMPLE_COMM_ISP_Aflib_Callback(ViPipe);
			// #endif

			snprintf(stBindAttr.stAeLib.acLibName, sizeof(CVI_AE_LIB_NAME), "%s", CVI_AE_LIB_NAME);
			stBindAttr.stAeLib.s32Id = ViPipe;
			stBindAttr.sensorId = 0;
			snprintf(stBindAttr.stAwbLib.acLibName, sizeof(CVI_AWB_LIB_NAME), "%s", CVI_AWB_LIB_NAME);
			stBindAttr.stAwbLib.s32Id = ViPipe;
			#if ENABLE_AF_LIB
			snprintf(stBindAttr.stAfLib.acLibName, sizeof(CVI_AF_LIB_NAME), "%s", CVI_AF_LIB_NAME);
			stBindAttr.stAfLib.s32Id = ViPipe;
			#endif
			s32Ret = CVI_ISP_SetBindAttr(ViPipe, &stBindAttr);
			if (s32Ret != CVI_SUCCESS) {
				printf("Bind Algo failed with %#x!\n", s32Ret);
			}
			s32Ret = CVI_ISP_MemInit(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				printf("Init Ext memory failed with %#x!\n", s32Ret);
				return s32Ret;
			}
			SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPubAttr);
			s32Ret = CVI_ISP_SetPubAttr(ViPipe, &stPubAttr);
			if (s32Ret != CVI_SUCCESS) {
				printf("SetPubAttr failed with %#x!\n", s32Ret);
				return s32Ret;
			}
			CVI_ISP_GetStatisticsConfig(0, &stsCfg);
			stsCfg.stAECfg.stCrop[0].bEnable = 0;
			stsCfg.stAECfg.stCrop[0].u16X = stsCfg.stAECfg.stCrop[0].u16Y = 0;
			stsCfg.stAECfg.stCrop[0].u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stAECfg.stCrop[0].u16H = stPubAttr.stWndRect.u32Height;

			#ifdef ARCH_CV183X
			stsCfg.stAECfg.stCrop[1].bEnable = 0;
			stsCfg.stAECfg.stCrop[1].u16X = stsCfg.stAECfg.stCrop[1].u16Y = 0;
			stsCfg.stAECfg.stCrop[1].u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stAECfg.stCrop[1].u16H = stPubAttr.stWndRect.u32Height;
			#endif

			stsCfg.stWBCfg.u16ZoneRow = AWB_ZONE_ORIG_ROW;
			stsCfg.stWBCfg.u16ZoneCol = AWB_ZONE_ORIG_COLUMN;
			stsCfg.stWBCfg.stCrop.bEnable = 0;
			stsCfg.stWBCfg.stCrop.u16X = stsCfg.stWBCfg.stCrop.u16Y = 0;
			stsCfg.stWBCfg.stCrop.u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stWBCfg.stCrop.u16H = stPubAttr.stWndRect.u32Height;
			stsCfg.stWBCfg.u16BlackLevel = 0;
			stsCfg.stWBCfg.u16WhiteLevel = 4095;
			stsCfg.stFocusCfg.stConfig.bEnable = 1;
			stsCfg.stFocusCfg.stConfig.u8HFltShift = 1;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[0] = 1;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[1] = 2;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[2] = 3;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[3] = 5;
			stsCfg.stFocusCfg.stConfig.s8HVFltLpCoeff[4] = 10;
			stsCfg.stFocusCfg.stConfig.stRawCfg.PreGammaEn = 0;
			stsCfg.stFocusCfg.stConfig.stPreFltCfg.PreFltEn = 1;
			stsCfg.stFocusCfg.stConfig.u16Hwnd = 17;
			stsCfg.stFocusCfg.stConfig.u16Vwnd = 15;
			stsCfg.stFocusCfg.stConfig.stCrop.bEnable = 0;
			// AF offset and size has some limitation.
			stsCfg.stFocusCfg.stConfig.stCrop.u16X = AF_XOFFSET_MIN;
			stsCfg.stFocusCfg.stConfig.stCrop.u16Y = AF_YOFFSET_MIN;
			stsCfg.stFocusCfg.stConfig.stCrop.u16W = stPubAttr.stWndRect.u32Width - AF_XOFFSET_MIN * 2;
			stsCfg.stFocusCfg.stConfig.stCrop.u16H = stPubAttr.stWndRect.u32Height - AF_YOFFSET_MIN * 2;
			//Horizontal HP0
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[0] = 0;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[1] = 0;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[2] = 13;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[3] = 24;
			stsCfg.stFocusCfg.stHParam_FIR0.s8HFltHpCoeff[4] = 0;
			//Horizontal HP1
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[0] = 1;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[1] = 2;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[2] = 4;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[3] = 8;
			stsCfg.stFocusCfg.stHParam_FIR1.s8HFltHpCoeff[4] = 0;
			//Vertical HP
			stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[0] = 13;
			stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[1] = 24;
			stsCfg.stFocusCfg.stVParam_FIR.s8VFltHpCoeff[2] = 0;
			stsCfg.unKey.bit1FEAeGloStat = stsCfg.unKey.bit1FEAeLocStat =
				stsCfg.unKey.bit1AwbStat1 = stsCfg.unKey.bit1AwbStat2 = stsCfg.unKey.bit1FEAfStat = 1;
			//LDG
			stsCfg.stFocusCfg.stConfig.u8ThLow = 0;
			stsCfg.stFocusCfg.stConfig.u8ThHigh = 255;
			stsCfg.stFocusCfg.stConfig.u8GainLow = 30;
			stsCfg.stFocusCfg.stConfig.u8GainHigh = 20;
			stsCfg.stFocusCfg.stConfig.u8SlopLow = 8;
			stsCfg.stFocusCfg.stConfig.u8SlopHigh = 15;
			s32Ret = CVI_ISP_SetStatisticsConfig(ViPipe, &stsCfg);
			if (s32Ret != CVI_SUCCESS) {
				printf("ISP Set Statistic failed with %#x!\n", s32Ret);
				return s32Ret;
			}

			s32Ret = CVI_ISP_Init(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				printf("ISP Init failed with %#x!\n", s32Ret);
				return s32Ret;
			}
		}
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_CreateIsp(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 i;
	CVI_S32 s32ViNum;
	CVI_S32 s32Ret = CVI_SUCCESS;

	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (!pstViConfig) {
		printf("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		s32ViNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32ViNum];

		s32Ret = SAMPLE_COMM_VI_StartIsp(pstViInfo);
		if (s32Ret != CVI_SUCCESS) {
			printf("SAMPLE_COMM_VI_StartIsp failed !\n");
			return CVI_FAILURE;
		}

		// s32Ret = SAMPLE_COMM_ISP_Run(s32ViNum);
		// if (s32Ret != CVI_SUCCESS) {
		// 	printf("ISP_Run failed with %#x!\n", s32Ret);
		// 	return s32Ret;
		// }
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_DestroyIsp(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 i;
	CVI_S32 s32ViNum;
	CVI_S32 s32Ret = CVI_SUCCESS;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (!pstViConfig) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		s32ViNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32ViNum];

		s32Ret = SAMPLE_COMM_VI_StopIsp(pstViInfo);

		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_VI_StopIsp failed !\n");
			return CVI_FAILURE;
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_StopIsp(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32 ret = CVI_SUCCESS;
	VI_PIPE ViPipe;

	for (CVI_U32 i = 0; i < WDR_MAX_PIPE_NUM; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			ViPipe = pstViInfo->stPipeInfo.aPipe[i];
			SAMPLE_COMM_ISP_Stop(ViPipe);
		}
	}

	return ret;
}

CVI_S32 SAMPLE_COMM_VI_GetDevAttrBySns(CVI_U32 u32SnsId, SAMPLE_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S *pstViDevAttr)
{
	PIC_SIZE_E enPicSize;
	SIZE_S stSize;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	ISP_SNS_OBJ_S *pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

	memcpy(pstViDevAttr, &DEV_ATTR_SENSOR_BASE, sizeof(VI_DEV_ATTR_S));

	SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enPicSize);
	SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (pstSnsObj)
		pstSnsObj->pfnGetRxAttr(u32SnsId, &stDevAttr);

	pstViDevAttr->stSnrSize.u32Width = stDevAttr.img_size.width;
	pstViDevAttr->stSnrSize.u32Height = stDevAttr.img_size.height;
	pstViDevAttr->stSnrSize.u32StartX = stDevAttr.img_size.x;
	pstViDevAttr->stSnrSize.u32StartY = stDevAttr.img_size.y;
	pstViDevAttr->stSnrSize.u32ActiveW = stSize.u32Width;
	pstViDevAttr->stSnrSize.u32ActiveH = stSize.u32Height;
	pstViDevAttr->stWDRAttr.u32CacheLine = stSize.u32Height;

	// WDR mode
	if (enSnsType >= SAMPLE_SNS_TYPE_LINEAR_BUTT)
		pstViDevAttr->stWDRAttr.enWDRMode = WDR_MODE_2To1_LINE;

	// YUV Sensor
	switch (enSnsType) {
	case PIXELPLUS_PR2020_1M_25FPS_8BIT:
	case PIXELPLUS_PR2020_1M_30FPS_8BIT:
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_30FPS_8BIT:
	case PIXELPLUS_PR2100_2M_25FPS_8BIT:
	case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
	case NEXTCHIP_N5_2M_25FPS_8BIT:
	case NEXTCHIP_N6_2M_4CH_25FPS_8BIT:
	case PICO640_THERMAL_479P:
	case PICO384_THERMAL_384X288:
	case VIVO_MM308M2_2M_25FPS_8BIT:
	case VIVO_MCS369_2M_30FPS_12BIT:
	case VIVO_MCS369Q_4M_30FPS_12BIT:
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		pstViDevAttr->enDataSeq = VI_DATA_SEQ_UYVY;
		pstViDevAttr->enInputDataType = VI_DATA_TYPE_YUV;
		pstViDevAttr->enIntfMode = VI_MODE_MIPI_YUV422;
		break;
	default:
		break;
	};

	// BT656
	switch (enSnsType) {
	case PIXELPLUS_PR2020_1M_25FPS_8BIT:
	case PIXELPLUS_PR2020_1M_30FPS_8BIT:
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_30FPS_8BIT:
	case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
	case NEXTCHIP_N5_2M_25FPS_8BIT:
		pstViDevAttr->enIntfMode = VI_MODE_BT656;
		break;
	default:
		break;
	};

	// BT1120
	switch (enSnsType) {
	case VIVO_MCS369_2M_30FPS_12BIT:
	case VIVO_MCS369Q_4M_30FPS_12BIT:
	case VIVO_MM308M2_2M_25FPS_8BIT:
		pstViDevAttr->enIntfMode = VI_MODE_BT1120_STANDARD;
		break;
	default:
		break;
	};

	// subLVDS
	switch (enSnsType) {
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
		pstViDevAttr->enIntfMode = VI_MODE_LVDS;
		break;
	default:
		break;
	};

	switch (enSnsType) {
	// Sony
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
	case SONY_IMX335_2L_MIPI_4M_30FPS_10BIT:
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
	case SONY_IMX335_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
	// GalaxyCore
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
		pstViDevAttr->enBayerFormat = BAYER_FORMAT_RG;
		break;
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
	case GCORE_GC4653_SLAVE_MIPI_4M_30FPS_10BIT:
	case TECHPOINT_TP2850_MIPI_2M_30FPS_8BIT:
	case TECHPOINT_TP2850_MIPI_4M_30FPS_8BIT:
		pstViDevAttr->enBayerFormat = BAYER_FORMAT_GR;
		break;
	default:
		pstViDevAttr->enBayerFormat = BAYER_FORMAT_BG;
		break;
	};

	// virtual channel for multi-ch
#ifndef ARCH_CV183X
	switch (enSnsType) {
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
		pstViDevAttr->enWorkMode = VI_WORK_MODE_2Multiplex;
		break;
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		pstViDevAttr->enWorkMode = VI_WORK_MODE_4Multiplex;
		break;
	default:
		pstViDevAttr->enWorkMode = VI_WORK_MODE_1Multiplex;
		break;
	}
#endif

	return CVI_SUCCESS;
}

void SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 i;

	memset(pstViConfig, 0, sizeof(*pstViConfig));

	for (i = 0; i < VI_MAX_DEV_NUM; i++) {
		pstViConfig->astViInfo[i].stSnsInfo.s32SnsId = i;
		pstViConfig->astViInfo[i].stSnsInfo.s32BusId = i;
		pstViConfig->astViInfo[i].stSnsInfo.MipiDev  = i;
		// memset(&pstViConfig->astViInfo[i].stSnapInfo, 0, sizeof(SAMPLE_SNAP_INFO_S));
		pstViConfig->astViInfo[i].stPipeInfo.bMultiPipe = CVI_FALSE;
		pstViConfig->astViInfo[i].stPipeInfo.bVcNumCfged = CVI_FALSE;
	}

	pstViConfig->astViInfo[0].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_60FPS_12BIT;
	pstViConfig->astViInfo[1].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_60FPS_12BIT;
}

/******************************************************************************
 * funciton : Get enSize by diffrent sensor
 ******************************************************************************/
CVI_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (!penSize)
		return CVI_FAILURE;

	switch (enMode) {
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
	case SOI_H65_MIPI_1M_30FPS_10BIT:
	case PIXELPLUS_PR2020_1M_25FPS_8BIT:
	case PIXELPLUS_PR2020_1M_30FPS_8BIT:
	case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
	case SONY_IMX327_MIPI_1M_30FPS_10BIT:
	case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
		*penSize = PIC_720P;
		break;
	case SONY_IMX290_MIPI_2M_60FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SOI_F23_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case VIVO_MM308M2_2M_25FPS_8BIT:
	case NEXTCHIP_N5_2M_25FPS_8BIT:
	case NEXTCHIP_N6_2M_4CH_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_30FPS_8BIT:
	case PIXELPLUS_PR2100_2M_25FPS_8BIT:
	case VIVO_MCS369_2M_30FPS_12BIT:
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT:
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
	case TECHPOINT_TP2850_MIPI_2M_30FPS_8BIT:
	case SONY_IMX335_MIPI_2M_30FPS_10BIT_WDR2TO1:
		*penSize = PIC_1080P;
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
	case SMS_SC850SL_MIPI_8M_30FPS_12BIT:
	case SMS_SC850SL_MIPI_8M_30FPS_10BIT_WDR2TO1:
		*penSize = PIC_3840x2160;
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
		*penSize = PIC_2592x1944;
		break;
	case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
		*penSize = PIC_2560x1600;
		break;
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_2L_MIPI_4M_30FPS_10BIT:
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
	case GCORE_GC4653_SLAVE_MIPI_4M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT:
	case SMS_SC401AI_MIPI_4M_30FPS_10BIT:
	case TECHPOINT_TP2850_MIPI_4M_30FPS_8BIT:
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		*penSize = PIC_1440P;
		break;
	case PICO640_THERMAL_479P:
		*penSize = PIC_479P;
		break;
	case PICO384_THERMAL_384X288:
		*penSize = PIC_288P;
		break;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
	case SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT:
	case SMS_SC401AI_MIPI_3M_30FPS_10BIT:
		*penSize = PIC_2304x1296;
		break;
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		*penSize = PIC_1080P;
		break;
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
	/* fallthrough */
	case OV_OS04C10_MIPI_4M_30FPS_12BIT:
	/* fallthrough */
	case OV_OS04C10_MIPI_4M_30FPS_10BIT_WDR2TO1:
	/* fallthrough */
	case OV_OS04C10_SLAVE_MIPI_4M_30FPS_12BIT:
	/* fallthrough */
	case OV_OS04C10_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
		*penSize = PIC_2688x1520;
		break;
	case SMS_SC035HGS_MIPI_480P_120FPS_12BIT:
	case SMS_SC035GS_MIPI_480P_120FPS_12BIT:
		*penSize = PIC_640x480;
		break;
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT:
	case SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT:
		*penSize = PIC_2880x1620;
		break;
	default:
		s32Ret = CVI_FAILURE;
		break;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S *pstSize)
{
	switch (enPicSize) {
	case PIC_CIF:   /* 352 * 288 */
		pstSize->u32Width  = 352;
		pstSize->u32Height = 288;
		break;

	case PIC_D1_PAL:   /* 720 * 576 */
		pstSize->u32Width  = 720;
		pstSize->u32Height = 576;
		break;

	case PIC_D1_NTSC:   /* 720 * 480 */
		pstSize->u32Width  = 720;
		pstSize->u32Height = 480;
		break;

	case PIC_720P:   /* 1280 * 720 */
		pstSize->u32Width  = 1280;
		pstSize->u32Height = 720;
		break;

	case PIC_1080P:  /* 1920 * 1080 */
		pstSize->u32Width  = 1920;
		pstSize->u32Height = 1080;
		break;

	case PIC_1088:  /* 1920 * 1088*/
		pstSize->u32Width  = 1920;
		pstSize->u32Height = 1088;
		break;

	case PIC_1440P:  /* 2560 * 1440 */
		pstSize->u32Width  = 2560;
		pstSize->u32Height = 1440;
		break;

	case PIC_2304x1296:
		pstSize->u32Width  = 2304;
		pstSize->u32Height = 1296;
		break;

	case PIC_2592x1520:
		pstSize->u32Width  = 2592;
		pstSize->u32Height = 1520;
		break;

	case PIC_2560x1600:
		pstSize->u32Width  = 2560;
		pstSize->u32Height = 1600;
		break;

	case PIC_2592x1944:
		pstSize->u32Width  = 2592;
		pstSize->u32Height = 1944;
		break;

	case PIC_2592x1536:
		pstSize->u32Width  = 2592;
		pstSize->u32Height = 1536;
		break;

	case PIC_2688x1520:
		pstSize->u32Width  = 2688;
		pstSize->u32Height = 1520;
		break;

	case PIC_2716x1524:
		pstSize->u32Width  = 2716;
		pstSize->u32Height = 1524;
		break;

	case PIC_2880x1620:
		pstSize->u32Width  = 2880;
		pstSize->u32Height = 1620;
		break;

	case PIC_3844x1124:
		pstSize->u32Width  = 3844;
		pstSize->u32Height = 1124;
		break;

	case PIC_3840x2160:
		pstSize->u32Width  = 3840;
		pstSize->u32Height = 2160;
		break;

	case PIC_3000x3000:
		pstSize->u32Width  = 3000;
		pstSize->u32Height = 3000;
		break;

	case PIC_4000x3000:
		pstSize->u32Width  = 4000;
		pstSize->u32Height = 3000;
		break;

	case PIC_4096x2160:
		pstSize->u32Width  = 4096;
		pstSize->u32Height = 2160;
		break;

	case PIC_3840x8640:
		pstSize->u32Width = 3840;
		pstSize->u32Height = 8640;
		break;

	case PIC_7688x1124:
		pstSize->u32Width = 7688;
		pstSize->u32Height = 1124;
		break;

	case PIC_640x480:
		pstSize->u32Width = 640;
		pstSize->u32Height = 480;
		break;
	case PIC_479P:  /* 632 * 479 */
		pstSize->u32Width  = 632;
		pstSize->u32Height = 479;
		break;
	case PIC_288P:  /* 384 * 288 */
		pstSize->u32Width  = 384;
		pstSize->u32Height = 288;
		break;
	default:
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_StartSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret, i;
	CVI_U32 u32SnsId;
	VI_PIPE ViPipe;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (pstViConfig->s32WorkingViNum <= 0) {
		printf("Error WorkingViNum = %d\n", pstViConfig->s32WorkingViNum);
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		s32Ret = SAMPLE_COMM_ISP_SetSnsObj(u32SnsId, pstViInfo->stSnsInfo.enSnsType);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "update sensor obj(%d) failed!\n", u32SnsId);
			return s32Ret;
		}
		s32Ret = SAMPLE_COMM_ISP_SetSnsInit(u32SnsId, pstViInfo->stSnsInfo.u8HwSync);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "update sensor(%d) hwsync failed !\n", u32SnsId);
			return s32Ret;
		}
		s32Ret = SAMPLE_COMM_ISP_PatchSnsObj(u32SnsId, &pstViInfo->stSnsInfo);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "patch rx attr(%d) failed!\n", u32SnsId);
			return s32Ret;
		}
		s32Ret = SAMPLE_COMM_ISP_Sensor_Regiter_callback(ViPipe, u32SnsId, pstViInfo->stSnsInfo.s32BusId,
								pstViInfo->stSnsInfo.s32SnsI2cAddr);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d register callback failed!\n", i);
			return s32Ret;
		}
	}
	s32Ret = SAMPLE_COMM_ISP_SetSensorMode(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d register callback failed!\n", i);
		return s32Ret;
	}
	return s32Ret;
}

#if 0
CVI_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = CVI_SUCCESS, i;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	const ISP_SNS_OBJ_S *pstSnsObj;

	/*TODO@CF. Need add sample function.*/
	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		pstSnsObj->pfnGetRxAttr(ViPipe, &stDevAttr);
		SAMPLE_PRT("sensor %d stDevAttr.devno %d\n", i, stDevAttr.devno);
		pstViInfo->stSnsInfo.MipiDev = stDevAttr.devno;
	}
	s32Ret = SAMPLE_COMM_VI_ResetSensor(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "ResetSensor failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_ResetMipi(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "ResetMipi failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_SetMipiAttr(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_SetMipiAttr failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_EnableSensorClock(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "EnableSensorClock failed! with %#x!\n", s32Ret);
		return s32Ret;
	}

	usleep(20);
	s32Ret = SAMPLE_COMM_VI_UnresetSensor(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "UnresetSensor failed! with %#x!\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_PLAT_VI_INIT(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	PIC_SIZE_E	   enPicSize;
	SIZE_S		   stSize;

	VI_DEV ViDev = 0;
	VI_PIPE ViPipe = 0;
	VI_PIPE_ATTR_S	   stPipeAttr;

	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 i = 0, j = 0;
	CVI_S32 s32DevNum;

	memcpy((void *)&stViConfigSys, (void *)pstViConfig, sizeof(SAMPLE_VI_CONFIG_S));

	/************************************************
	 * step1:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(pstViConfig->astViInfo[ViDev].stSnsInfo.enSnsType, &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		goto error;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		goto error;
	}


	/************************************************
	 * step2:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		goto error;
	}

	// for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
	// 	ViDev = i;

	// 	s32Ret = SAMPLE_COMM_VI_StartDev(&pstViConfig->astViInfo[ViDev]);
	// 	if (s32Ret != CVI_SUCCESS) {
	// 		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
	// 		goto error;
	// 	}
	// }

	s32Ret = SAMPLE_COMM_VI_StartMIPI(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start MIPI failed with %#x\n", s32Ret);
		goto error;
	}

	s32Ret = SAMPLE_COMM_VI_SensorProbe(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system sensor probe failed with %#x\n", s32Ret);
		goto error;
	}

	stPipeAttr.bYuvSkip = CVI_FALSE;
	stPipeAttr.u32MaxW = stSize.u32Width;
	stPipeAttr.u32MaxH = stSize.u32Height;
	stPipeAttr.enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
	stPipeAttr.enBitWidth = DATA_BITWIDTH_12;
	stPipeAttr.stFrameRate.s32SrcFrameRate = -1;
	stPipeAttr.stFrameRate.s32DstFrameRate = -1;
	stPipeAttr.bNrEn = CVI_TRUE;
	stPipeAttr.bYuvBypassPath = CVI_FALSE;
	stPipeAttr.enCompressMode = pstViConfig->astViInfo[0].stChnInfo.enCompressMode;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		SAMPLE_VI_INFO_S *pstViInfo = NULL;

		s32DevNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32DevNum];

		if ((pstViInfo->stSnsInfo.enSnsType == PICO640_THERMAL_479P) ||
			(pstViInfo->stSnsInfo.enSnsType == TECHPOINT_TP2850_MIPI_2M_30FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == TECHPOINT_TP2850_MIPI_4M_30FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == VIVO_MCS369Q_4M_30FPS_12BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == VIVO_MCS369_2M_30FPS_12BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == VIVO_MM308M2_2M_25FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == NEXTCHIP_N5_2M_25FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == PIXELPLUS_PR2020_1M_25FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == PIXELPLUS_PR2020_1M_30FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == PIXELPLUS_PR2020_2M_25FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == PIXELPLUS_PR2020_2M_30FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == PIXELPLUS_PR2100_2M_25FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT)) {
			stPipeAttr.bYuvBypassPath = CVI_TRUE;
		} else {
			stPipeAttr.bYuvBypassPath = CVI_FALSE;
		}

		for (j = 0; j < WDR_MAX_PIPE_NUM; j++) {
			if (pstViInfo->stPipeInfo.aPipe[j] >= 0 && pstViInfo->stPipeInfo.aPipe[j] < VI_MAX_PIPE_NUM) {
				ViPipe = pstViInfo->stPipeInfo.aPipe[j];
				s32Ret = CVI_VI_CreatePipe(ViPipe, &stPipeAttr);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_CreatePipe failed with %#x!\n", s32Ret);
					goto error;
				}

				s32Ret = CVI_VI_StartPipe(ViPipe);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_StartPipe failed with %#x!\n", s32Ret);
					goto error;
				}

				s32Ret = CVI_VI_GetPipeAttr(ViPipe, &stPipeAttr);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_GetPipeAttr failed with %#x!\n", s32Ret);
					goto error;
				}
			}
		}
	}

	s32Ret = SAMPLE_COMM_VI_CreateIsp(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_CreateIsp failed with %#x!\n", s32Ret);
		goto error;
	}

	s32Ret = SAMPLE_COMM_VI_StartViChn(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartViChn failed with %#x!\n", s32Ret);
		goto error;
	}

	return s32Ret;
error:
	// _SAMPLE_PLAT_ERR_Exit();
	return s32Ret;
}
#endif
