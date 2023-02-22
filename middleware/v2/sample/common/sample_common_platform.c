#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/param.h>
#include <signal.h>

#include "cvi_buffer.h"
#include "cvi_ae_comm.h"
#include "cvi_awb_comm.h"
#include "cvi_comm_isp.h"

#include "sample_comm.h"

SAMPLE_VI_CONFIG_S stViConfigSys;

void _SAMPLE_PLAT_ERR_Exit(void)
{
	if (stViConfigSys.s32WorkingViNum != 0) {
		SAMPLE_COMM_VI_DestroyIsp(&stViConfigSys);
		SAMPLE_COMM_VI_DestroyVi(&stViConfigSys);
	}
	SAMPLE_COMM_SYS_Exit();
}

void _SAMPLE_PLAT_SYS_HandleSig(int nSignal, siginfo_t *si, void *arg)
{
	UNUSED(nSignal);
	UNUSED(si);
	UNUSED(arg);

	_SAMPLE_PLAT_ERR_Exit();

	exit(1);
}

CVI_S32 SAMPLE_PLAT_SYS_INIT(SIZE_S stSize)
{
	VB_CONFIG_S	   stVbConf;
	CVI_U32        u32BlkSize, u32BlkRotSize;
	CVI_S32 s32Ret = CVI_SUCCESS;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	struct sigaction sa = {};

	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = _SAMPLE_PLAT_SYS_HandleSig;
	sa.sa_flags = SA_SIGINFO|SA_RESETHAND;	// Reset signal handler to system default after signal triggered
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	u32BlkRotSize = COMMON_GetPicBufferSize(stSize.u32Height, stSize.u32Width, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	u32BlkSize = MAX(u32BlkSize, u32BlkRotSize);

	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
#if !defined(DDR_64MB_SIZE)
	stVbConf.astCommPool[0].u32BlkCnt	= 8;
#else
	stVbConf.astCommPool[0].u32BlkCnt	= 4;
#endif
	stVbConf.astCommPool[0].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system init failed with %#x\n", s32Ret);
		goto error;
	}

	return s32Ret;
error:
	_SAMPLE_PLAT_ERR_Exit();
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
#if USE_USER_SEN_DRIVER
	s32Ret = SAMPLE_COMM_VI_StartSensor(pstViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		goto error;
	}
#endif
	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		ViDev = i;

		s32Ret = SAMPLE_COMM_VI_StartDev(&pstViConfig->astViInfo[ViDev]);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
			goto error;
		}
	}

#if USE_USER_SEN_DRIVER
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
#endif

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

		if ((pstViInfo->stSnsInfo.enSnsType == GCORE_GC2145_MIPI_2M_12FPS_8BIT) ||
			(pstViInfo->stSnsInfo.enSnsType == PICO640_THERMAL_479P) ||
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
	_SAMPLE_PLAT_ERR_Exit();
	return s32Ret;
}

CVI_S32 SAMPLE_PLAT_VPSS_INIT(VPSS_GRP VpssGrp, SIZE_S stSizeIn, SIZE_S stSizeOut)
{
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];
	CVI_S32 s32Ret = CVI_SUCCESS;

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSizeIn.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeIn.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = stSizeOut.u32Width;
	astVpssChnAttr[VpssChn].u32Height                   = stSizeOut.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_TRUE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_TRUE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_AUTO;
	astVpssChnAttr[VpssChn].stAspectRatio.bEnableBgColor = CVI_TRUE;
	astVpssChnAttr[VpssChn].stAspectRatio.u32BgColor    = COLOR_RGB_BLACK;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		goto error;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		goto error;
	}

	return s32Ret;
error:
	_SAMPLE_PLAT_ERR_Exit();
	return s32Ret;
}

CVI_S32 SAMPLE_PLAT_VO_INIT_BT656(void)
{
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, 720, 576};
	SIZE_S stDefImageSize = {720, 576};
	CVI_S32 s32Ret = CVI_SUCCESS;

	CVI_U32 panel_init = false;
	VO_PUB_ATTR_S stVoPubAttr;

	CVI_VO_Get_Panel_Status(0, 0, &panel_init);
	if (panel_init) {
		CVI_VO_GetPubAttr(0, &stVoPubAttr);
		CVI_TRACE_LOG(CVI_DBG_NOTICE, "Panel w=%d, h=%d.\n",\
				stVoPubAttr.stSyncInfo.u16Hact, stVoPubAttr.stSyncInfo.u16Vact);
		stDefDispRect.u32Width = stVoPubAttr.stSyncInfo.u16Hact;
		stDefDispRect.u32Height = stVoPubAttr.stSyncInfo.u16Vact;
		stDefImageSize.u32Width = stVoPubAttr.stSyncInfo.u16Hact;
		stDefImageSize.u32Height = stVoPubAttr.stSyncInfo.u16Vact;
	}

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		goto error;
	}

	stVoConfig.VoDev	 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_BT656;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_576P50;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = SAMPLE_PIXEL_FORMAT;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		goto error;
	}

	return s32Ret;
error:
	_SAMPLE_PLAT_ERR_Exit();
	return s32Ret;
}

CVI_S32 SAMPLE_PLAT_VO_INIT(void)
{
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, 720, 1280};
	SIZE_S stDefImageSize = {720, 1280};
	CVI_S32 s32Ret = CVI_SUCCESS;

	CVI_U32 panel_init = false;
	VO_PUB_ATTR_S stVoPubAttr;

	CVI_VO_Get_Panel_Status(0, 0, &panel_init);
	if (panel_init) {
		CVI_VO_GetPubAttr(0, &stVoPubAttr);
		CVI_TRACE_LOG(CVI_DBG_NOTICE, "Panel w=%d, h=%d.\n",\
				stVoPubAttr.stSyncInfo.u16Hact, stVoPubAttr.stSyncInfo.u16Vact);
		stDefDispRect.u32Width = stVoPubAttr.stSyncInfo.u16Hact;
		stDefDispRect.u32Height = stVoPubAttr.stSyncInfo.u16Vact;
		stDefImageSize.u32Width = stVoPubAttr.stSyncInfo.u16Hact;
		stDefImageSize.u32Height = stVoPubAttr.stSyncInfo.u16Vact;
	}

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		goto error;
	}

	stVoConfig.VoDev	 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = SAMPLE_PIXEL_FORMAT;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		goto error;
	}

	return s32Ret;
error:
	_SAMPLE_PLAT_ERR_Exit();
	return s32Ret;
}
