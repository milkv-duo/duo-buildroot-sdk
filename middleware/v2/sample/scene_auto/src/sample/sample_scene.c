#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "cvi_buffer.h"
#include "cvi_ae_comm.h"
#include "cvi_awb_comm.h"
#include "cvi_comm_isp.h"

#include "sample_scene.h"

CVI_S32 SAMPLE_SCENE_VoRotation_Start(TStreamInfo *ptStreamInfo)
{
	SAMPLE_SNS_TYPE_E  enSnsType        = SENSOR0_TYPE;
	WDR_MODE_E         enWDRMode        = WDR_MODE_NONE;
	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E     enPixFormat      = SAMPLE_PIXEL_FORMAT;
	VIDEO_FORMAT_E     enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E     enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;

	VB_CONFIG_S        stVbConf;
	PIC_SIZE_E         enPicSize;
	CVI_U32            u32BlkSize;
	SIZE_S             stSize;
	CVI_S32            s32Ret = CVI_SUCCESS;

	VI_PIPE_ATTR_S     stPipeAttr = {};

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&(ptStreamInfo->stViConfig));

	ptStreamInfo->stViConfig.astViInfo[0].stSnsInfo.enSnsType       = enSnsType;
	ptStreamInfo->stViConfig.s32WorkingViNum                        = 1;
	ptStreamInfo->stViConfig.as32WorkingViId[0]                     = 0;
	ptStreamInfo->stViConfig.astViInfo[0].stSnsInfo.MipiDev         = 0xFF;
	ptStreamInfo->stViConfig.astViInfo[0].stSnsInfo.s32BusId        = 3;
	ptStreamInfo->stViConfig.astViInfo[0].stDevInfo.ViDev           = ptStreamInfo->ViDev;
	ptStreamInfo->stViConfig.astViInfo[0].stDevInfo.enWDRMode       = enWDRMode;
	ptStreamInfo->stViConfig.astViInfo[0].stPipeInfo.enMastPipeMode = enMastPipeMode;
	ptStreamInfo->stViConfig.astViInfo[0].stPipeInfo.aPipe[0]       = ptStreamInfo->ViPipe;
	ptStreamInfo->stViConfig.astViInfo[0].stPipeInfo.aPipe[1]       = -1;
	ptStreamInfo->stViConfig.astViInfo[0].stPipeInfo.aPipe[2]       = -1;
	ptStreamInfo->stViConfig.astViInfo[0].stPipeInfo.aPipe[3]       = -1;
	ptStreamInfo->stViConfig.astViInfo[0].stChnInfo.ViChn           = ptStreamInfo->ViChn;
	ptStreamInfo->stViConfig.astViInfo[0].stChnInfo.enPixFormat     = enPixFormat;
	ptStreamInfo->stViConfig.astViInfo[0].stChnInfo.enDynamicRange  = enDynamicRange;
	ptStreamInfo->stViConfig.astViInfo[0].stChnInfo.enVideoFormat   = enVideoFormat;
	ptStreamInfo->stViConfig.astViInfo[0].stChnInfo.enCompressMode  = enCompressMode;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(ptStreamInfo->stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height,
		SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 8;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_SYS_Init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step4:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(&(ptStreamInfo->stViConfig));
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VI_StartSensor failed with %#x\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartDev(&(ptStreamInfo->stViConfig.astViInfo[ptStreamInfo->ViDev]));
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VI_StartDev failed with %#x\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartMIPI(&(ptStreamInfo->stViConfig));
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VI_StartMIPI failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stPipeAttr.bYuvSkip = CVI_FALSE;
	stPipeAttr.u32MaxW = stSize.u32Width;
	stPipeAttr.u32MaxH = stSize.u32Height;
	stPipeAttr.enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
	stPipeAttr.enBitWidth = DATA_BITWIDTH_12;
	stPipeAttr.stFrameRate.s32SrcFrameRate = -1;
	stPipeAttr.stFrameRate.s32DstFrameRate = -1;
	stPipeAttr.bNrEn = CVI_TRUE;
	s32Ret = CVI_VI_CreatePipe(ptStreamInfo->ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_CreatePipe failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_StartPipe(ptStreamInfo->ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_GetPipeAttr(ptStreamInfo->ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_CreateIsp(&(ptStreamInfo->stViConfig));
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VI_CreateIsp failed with %#x\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_COMM_VI_StartViChn(&(ptStreamInfo->stViConfig));

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP_ATTR_S     stVpssGrpAttr = {};
	VPSS_CHN_ATTR_S     astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {};

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate   = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate   = -1;
	stVpssGrpAttr.enPixelFormat                 = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                       = stSize.u32Width;
	stVpssGrpAttr.u32MaxH                       = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev                     = 0;

	astVpssChnAttr[ptStreamInfo->VpssChn].u32Width                      = 1280;
	astVpssChnAttr[ptStreamInfo->VpssChn].u32Height                     = 720;
	astVpssChnAttr[ptStreamInfo->VpssChn].enVideoFormat                 = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[ptStreamInfo->VpssChn].enPixelFormat                 = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[ptStreamInfo->VpssChn].stFrameRate.s32SrcFrameRate   = 30;
	astVpssChnAttr[ptStreamInfo->VpssChn].stFrameRate.s32DstFrameRate   = 30;
	astVpssChnAttr[ptStreamInfo->VpssChn].u32Depth                      = 0;
	astVpssChnAttr[ptStreamInfo->VpssChn].bMirror                       = CVI_FALSE;
	astVpssChnAttr[ptStreamInfo->VpssChn].bFlip                         = CVI_FALSE;
	astVpssChnAttr[ptStreamInfo->VpssChn].stAspectRatio.enMode          = ASPECT_RATIO_NONE;
	astVpssChnAttr[ptStreamInfo->VpssChn].stNormalize.bEnable           = CVI_FALSE;

	/*start vpss*/
	ptStreamInfo->abChnEnable[ptStreamInfo->VpssChn] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(ptStreamInfo->VpssGrp,
		ptStreamInfo->abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VPSS_Init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(ptStreamInfo->VpssGrp,
		ptStreamInfo->abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VPSS_Start failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ptStreamInfo->ViPipe,
		ptStreamInfo->ViChn, ptStreamInfo->VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VPSS failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step6:  Init VO
	 ************************************************/
	RECT_S stDefDispRect    = {0, 0, 720, 1280};
	SIZE_S stDefImageSize   = {720, 1280};

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&(ptStreamInfo->stVoConfig));
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
	}

	ptStreamInfo->stVoConfig.VoDev                      = ptStreamInfo->VoDev;
	ptStreamInfo->stVoConfig.stVoPubAttr.enIntfType     = VO_INTF_MIPI;
	ptStreamInfo->stVoConfig.stVoPubAttr.enIntfSync     = VO_OUTPUT_720x1280_60;
	ptStreamInfo->stVoConfig.stDispRect                 = stDefDispRect;
	ptStreamInfo->stVoConfig.stImageSize                = stDefImageSize;
	ptStreamInfo->stVoConfig.enPixFormat                = SAMPLE_PIXEL_FORMAT;
	ptStreamInfo->stVoConfig.enVoMode                   = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&(ptStreamInfo->stVoConfig));
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		return s32Ret;
	}

	CVI_VO_SetChnRotation(ptStreamInfo->VoDev, ptStreamInfo->VoChn, ROTATION_90);
	SAMPLE_COMM_VPSS_Bind_VO(ptStreamInfo->VpssGrp, ptStreamInfo->VpssChn,
		ptStreamInfo->VoDev, ptStreamInfo->VoChn);

	return s32Ret;
}

CVI_S32 SAMPLE_SCENE_VoRotation_Stop(TStreamInfo *ptStreamInfo)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	SAMPLE_COMM_VPSS_UnBind_VO(ptStreamInfo->VpssGrp, ptStreamInfo->VpssChn,
		ptStreamInfo->VoDev, ptStreamInfo->VoChn);

	SAMPLE_COMM_VO_StopVO(&(ptStreamInfo->stVoConfig));

	SAMPLE_COMM_VI_UnBind_VPSS(ptStreamInfo->ViPipe, ptStreamInfo->ViChn, ptStreamInfo->VpssGrp);

	SAMPLE_COMM_VPSS_Stop(ptStreamInfo->VpssGrp, ptStreamInfo->abChnEnable);

	SAMPLE_COMM_ISP_Stop(ptStreamInfo->ViDev);

	SAMPLE_COMM_VI_DestroyVi(&(ptStreamInfo->stViConfig));

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}
