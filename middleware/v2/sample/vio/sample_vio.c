#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/prctl.h>
#include <inttypes.h>

#include "cvi_buffer.h"
#include "cvi_ae_comm.h"
#include "cvi_awb_comm.h"
#include "cvi_comm_isp.h"

#include "sample_comm.h"


#define NONE	"\033[m"
#define RED	"\033[0;32;31m"
#define GREEN	"\033[0;32;32m"

#define PACKED_32BIT(a, b, c, d) ((a<<24) + (b<<16) + (c<<8) + d)
#define PACKED_16BIT(c, d) ((c<<8) + d)

#define PR2100_WIDTH		1920
#define PR2100_HEIGHT		1080
#define PR2100_CHID_SIZE	2
#define PR2100_LINE_VALID(a) (((a>>11)&1) ? 1:0)
#define PR2100_LINE_NUMBER(a) (a&0x7ff)
#define PR2100_FRAME_NUMBER(a) ((a>>12)&0x7)
typedef struct _SAMPLE_VPSS_PARAM_S {
	VPSS_GRP  SrcGrp;
	VPSS_CHN  SrcChn;
	VPSS_GRP  DstGrp;
	VPSS_CHN  DstChn;
	RECT_S    stDispRect;
} SAMPLE_VPSS_PARAM_S;

typedef struct _SAMPLE_VPSS_CONFIG_S {
	CVI_S32   s32ChnNum;
	CVI_BOOL  stop_thread;
	pthread_t vpss_thread;
	SAMPLE_VPSS_PARAM_S astVpssParam[4];
} SAMPLE_VPSS_CONFIG_S;

#ifdef ARCH_CV182X
CVI_S32 SAMPLE_IMX307_ONTHEFLY_ONLINE_SC_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};
	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = SONY_IMX307_MIPI_2M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.MipiDev[0]   = 0xff,
		.s32BusId[0]  = 3,
	};

	printf("IMX307 onthefly online to sc\n");

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat1	    = PIXEL_FORMAT_NV21;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_TILE;
	// VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize, i;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;
	SIZE_S		stSizeSns0, stSizeOut;

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	//Set sensor number
	CVI_VI_SetDevNum(stIniCfg.devNum);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = s32WorkSnsId;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat1;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns0);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= stIniCfg.devNum;

	for (i = 0; i < stVbConf.u32MaxPoolCnt; i++) {
		SIZE_S stSize = stSizeSns0;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, enPixFormat1,
							DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
		stVbConf.astCommPool[i].u32BlkSize	= u32BlkSize;
		stVbConf.astCommPool[i].u32BlkCnt	= 7;
		SAMPLE_PRT("common pool[%d] BlkSize %d\n", i, u32BlkSize);
	}

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	VI_VPSS_MODE_S stVIVPSSMode;
	VPSS_MODE_S stVPSSMode;

	stVIVPSSMode.aenMode[0] = VI_ONLINE_VPSS_ONLINE;

	s32Ret = CVI_SYS_SetVIVPSSMode(&stVIVPSSMode);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_SetVIVPSSMode failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVPSSMode.enMode = VPSS_MODE_DUAL;
	stVPSSMode.aenInput[0] = VPSS_INPUT_ISP;
	stVPSSMode.ViPipe[0] = 0;
	stVPSSMode.aenInput[1] = VPSS_INPUT_MEM;
	stVPSSMode.ViPipe[1] = 0;

	s32Ret = CVI_SYS_SetVPSSModeEx(&stVPSSMode);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_SetVPSSModeEx failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = VPSS_ONLINE_GRP_0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	stSizeOut.u32Width  = 1280;
	stSizeOut.u32Height = 720;

	// snr0
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = enPixFormat1;
	stVpssGrpAttr.u32MaxW                        = stSizeSns0.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeSns0.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = stSizeOut.u32Width;
	astVpssChnAttr[VpssChn].u32Height                   = stSizeOut.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = PIXEL_FORMAT_NV21;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}
#if 0
	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(0, 0, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}
#endif
	/************************************************
	 * step4:  Init VI
	 ************************************************/
	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, stSizeOut.u32Height, stSizeOut.u32Width};
	SIZE_S stDefImageSize = {stSizeOut.u32Height, stSizeOut.u32Width};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVoConfig.VoDev	 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = PIXEL_FORMAT_NV21;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		return s32Ret;
	}

	VpssGrp = VPSS_ONLINE_GRP_0;
	VpssChn = 0;
	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);
	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	int op = 0;

	do {
		SAMPLE_PRT(GREEN "\nselect 255 exit!\n" NONE);

		scanf("%d", &op);
		if (op == 255) {
			break;
		}
	} while (1);

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);
	SAMPLE_COMM_VI_DestroyVi(&stViConfig);
	SAMPLE_COMM_VI_CLOSE();

	VpssGrp = VPSS_ONLINE_GRP_0;
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	VpssGrp = 2;
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

CVI_S32 SAMPLE_IMX307_PR2020_OFFLINE_ONLINE_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};
	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 2,
		.enSnsType[0] = SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,
		.enWDRMode[0] = WDR_MODE_2To1_LINE,
		//.enSnsType[0] = SONY_IMX307_MIPI_2M_30FPS_12BIT,
		//.enWDRMode[0] = WDR_MODE_NONE,
		.MipiDev[0]   = 0xff,
		.s32BusId[0]  = 3,
		.enSnsType[1] = PIXELPLUS_PR2020_2M_25FPS_8BIT,
		.enWDRMode[1] = WDR_MODE_NONE,
		.MipiDev[1]   = 0xff,
		.s32BusId[1]  = 2,
	};
	printf("IMX307 + PR2020\n");

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat1	    = PIXEL_FORMAT_NV21;
	PIXEL_FORMAT_E	   enPixFormat2	    = PIXEL_FORMAT_YUYV;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_TILE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize, i;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;
	SIZE_S		stSizeSns0, stSizeSns1, stSizeOut;

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	//Set sensor number
	CVI_VI_SetDevNum(stIniCfg.devNum);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = s32WorkSnsId;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = (s32WorkSnsId == 0) ?
										enPixFormat1 : enPixFormat2;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns0);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[1], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= stIniCfg.devNum;

	for (i = 0; i < stVbConf.u32MaxPoolCnt; i++) {
		SIZE_S stSize = (i == 0) ? stSizeSns0 : stSizeSns1;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height,
							(i == 0) ? enPixFormat1 : enPixFormat2,
							DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
		stVbConf.astCommPool[i].u32BlkSize	= u32BlkSize;
		stVbConf.astCommPool[i].u32BlkCnt	= 7;
		SAMPLE_PRT("common pool[%d] BlkSize %d\n", i, u32BlkSize);
	}

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	int op;

	SAMPLE_PRT("Offline(0)/Online(1):\n");
	scanf("%d", &op);

	if (op == 1) { //online
		VI_VPSS_MODE_S stVIVPSSMode;
		VPSS_MODE_S stVPSSMode;

		stVIVPSSMode.aenMode[0] = VI_OFFLINE_VPSS_ONLINE;

		s32Ret = CVI_SYS_SetVIVPSSMode(&stVIVPSSMode);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_SetVIVPSSMode failed with %#x\n", s32Ret);
			return s32Ret;
		}

		stVPSSMode.enMode = VPSS_MODE_DUAL;
		stVPSSMode.aenInput[0] = VPSS_INPUT_ISP;
		stVPSSMode.ViPipe[0] = 0;
		stVPSSMode.aenInput[1] = VPSS_INPUT_MEM;
		stVPSSMode.ViPipe[1] = 0;

		s32Ret = CVI_SYS_SetVPSSModeEx(&stVPSSMode);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_SetVPSSModeEx failed with %#x\n", s32Ret);
			return s32Ret;
		}
	}
	/************************************************
	 * step4:  Init VI
	 ************************************************/
	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = VPSS_ONLINE_GRP_0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	stSizeOut.u32Width  = 1280;
	stSizeOut.u32Height = 720;

	// snr0
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = enPixFormat1;
	stVpssGrpAttr.u32MaxW                        = stSizeSns0.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeSns0.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = stSizeOut.u32Width;
	astVpssChnAttr[VpssChn].u32Height                   = stSizeOut.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = PIXEL_FORMAT_NV21;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;

	if (op == 0) { //offline
		ViPipe = 0;
		ViChn = 0;
		s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}
	}

	// snr1
	VpssGrp	  = VPSS_ONLINE_GRP_1;
	if (op == 0) //offline
		stVpssGrpAttr.enPixelFormat                  = enPixFormat2;
	stVpssGrpAttr.u32MaxW                        = stSizeSns1.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeSns1.u32Height;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	if (op == 0) { //offline
		ViPipe = 0;
		ViChn = 1;
		s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}
	}

	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, stSizeOut.u32Height, stSizeOut.u32Width};
	SIZE_S stDefImageSize = {stSizeOut.u32Height, stSizeOut.u32Width};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVoConfig.VoDev	 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = PIXEL_FORMAT_NV21;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		return s32Ret;
	}

	VpssGrp = VPSS_ONLINE_GRP_0;
	VpssChn = 0;
	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);
	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	do {
		SAMPLE_PRT(GREEN "\nselect sensor: 0-imx307/1-PR2020, or 255 exit!\n" NONE);

		scanf("%d", &op);
		if (op == 255) {
			break;
		}
		VpssGrp = (op == 0) ? VPSS_ONLINE_GRP_0 : VPSS_ONLINE_GRP_1;
		SAMPLE_COMM_VPSS_UnBind_VO((VpssGrp ^ 1), VpssChn, stVoConfig.VoDev, VoChn);
		SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	} while (1);

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);
	SAMPLE_COMM_VI_DestroyVi(&stViConfig);
	SAMPLE_COMM_VI_CLOSE();

	VpssGrp = VPSS_ONLINE_GRP_0;
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	VpssGrp = VPSS_ONLINE_GRP_1;
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	VpssGrp = 2;
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

CVI_S32 SAMPLE_IMX335_PR2020_OFFLINE_ONLINE_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};
	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 2,
		.enSnsType[0] = SONY_IMX335_MIPI_4M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.MipiDev[0]   = 0xff,
		.s32BusId[0]  = 3,
		.enSnsType[1] = PIXELPLUS_PR2020_2M_25FPS_8BIT,
		.enWDRMode[1] = WDR_MODE_NONE,
		.MipiDev[1]   = 0xff,
		.s32BusId[1]  = 2,
	};

	printf("IMX335 + PR2020\n");

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat1	    = PIXEL_FORMAT_NV21;
	PIXEL_FORMAT_E	   enPixFormat2	    = PIXEL_FORMAT_YUYV;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_TILE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize, i;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;
	SIZE_S		stSizeSns0, stSizeSns1, stSizeOut;

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	//Set sensor number
	CVI_VI_SetDevNum(stIniCfg.devNum);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = s32WorkSnsId;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = (s32WorkSnsId == 0) ?
										enPixFormat1 : enPixFormat2;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns0);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[1], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= stIniCfg.devNum;

	for (i = 0; i < stVbConf.u32MaxPoolCnt; i++) {
		SIZE_S stSize = (i == 0) ? stSizeSns0 : stSizeSns1;

		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height,
							(i == 0) ? enPixFormat1 : enPixFormat2,
							DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
		stVbConf.astCommPool[i].u32BlkSize	= u32BlkSize;
		stVbConf.astCommPool[i].u32BlkCnt	= (i == 0) ? 5 : 9;
		SAMPLE_PRT("common pool[%d] BlkSize %d\n", i, u32BlkSize);
	}

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	int op, is_yuv_online = 1;

	SAMPLE_PRT("Offline(0)/Online(1):\n");
	scanf("%d", &op);
	if (op == 1) {
		SAMPLE_PRT("YUV sensor Offline(0)/Online(1):\n");
		scanf("%d", &is_yuv_online);
	}

	if (op == 1) { //online
		VI_VPSS_MODE_S stVIVPSSMode;
		VPSS_MODE_S stVPSSMode;

		stVIVPSSMode.aenMode[0] = VI_OFFLINE_VPSS_ONLINE;
		if (is_yuv_online)
			stVIVPSSMode.aenMode[1] = VI_OFFLINE_VPSS_ONLINE;
		else
			stVIVPSSMode.aenMode[1] = VI_OFFLINE_VPSS_OFFLINE;

		s32Ret = CVI_SYS_SetVIVPSSMode(&stVIVPSSMode);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_SetVIVPSSMode failed with %#x\n", s32Ret);
			return s32Ret;
		}

		stVPSSMode.enMode = VPSS_MODE_DUAL;
		stVPSSMode.aenInput[0] = VPSS_INPUT_ISP;
		stVPSSMode.ViPipe[0] = 0;
		stVPSSMode.aenInput[1] = VPSS_INPUT_MEM;
		stVPSSMode.ViPipe[1] = 0;

		s32Ret = CVI_SYS_SetVPSSModeEx(&stVPSSMode);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_SetVPSSModeEx failed with %#x\n", s32Ret);
			return s32Ret;
		}
	}
	/************************************************
	 * step4:  Init VI
	 ************************************************/
	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = VPSS_ONLINE_GRP_0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;

	stSizeOut.u32Width  = 1280;
	stSizeOut.u32Height = 720;

	// snr0
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = enPixFormat1;
	stVpssGrpAttr.u32MaxW                        = stSizeSns0.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeSns0.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = stSizeOut.u32Width;
	astVpssChnAttr[VpssChn].u32Height                   = stSizeOut.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = PIXEL_FORMAT_NV21;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable	    = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	if (op == 0) { //offline
		ViPipe = 0;
		ViChn = 0;
		s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}
	}

	// snr1
	VpssGrp = VPSS_ONLINE_GRP_1;
	if (op == 0) //offline
		stVpssGrpAttr.enPixelFormat                  = enPixFormat2;
	else if (!is_yuv_online) { //online, but yuv offline
		stVpssGrpAttr.enPixelFormat                  = enPixFormat2;
		stVpssGrpAttr.u8VpssDev                      = 1;
	}
	stVpssGrpAttr.u32MaxW                        = stSizeSns1.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeSns1.u32Height;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	if (op == 0 || !is_yuv_online) { //offline
		ViPipe = 0;
		ViChn = 1;
		s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}
	}

	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, stSizeOut.u32Height, stSizeOut.u32Width};
	SIZE_S stDefImageSize = {stSizeOut.u32Height, stSizeOut.u32Width};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVoConfig.VoDev	 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = PIXEL_FORMAT_NV21;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		return s32Ret;
	}

	VpssGrp = VPSS_ONLINE_GRP_0;
	VpssChn = 0;
	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);
	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	do {
		SAMPLE_PRT(GREEN "\nselect sensor: 0-imx335/1-PR2020, or 255 exit!\n" NONE);

		scanf("%d", &op);
		if (op == 255) {
			break;
		}
		VpssGrp = (op == 0) ? VPSS_ONLINE_GRP_0 : VPSS_ONLINE_GRP_1;
		SAMPLE_COMM_VPSS_UnBind_VO((VpssGrp ^ 1), VpssChn, stVoConfig.VoDev, VoChn);
		SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	} while (1);

	SAMPLE_COMM_VPSS_UnBind_VO(0, VpssChn, stVoConfig.VoDev, VoChn);
	SAMPLE_COMM_VPSS_UnBind_VO(1, VpssChn, stVoConfig.VoDev, VoChn);
	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_UnBind_VPSS(0, 0, 0);
	SAMPLE_COMM_VI_UnBind_VPSS(0, 1, 0);
	SAMPLE_COMM_VI_UnBind_VPSS(0, 0, 1);
	SAMPLE_COMM_VI_UnBind_VPSS(0, 1, 1);

	SAMPLE_COMM_VPSS_Stop(0, abChnEnable);
	SAMPLE_COMM_VPSS_Stop(1, abChnEnable);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);
	SAMPLE_COMM_VI_DestroyVi(&stViConfig);
	SAMPLE_COMM_VI_CLOSE();

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

#endif

#ifdef ARCH_CV183X
CVI_S32 pr2100_output_quick_by_vpss(CVI_S32 count, CVI_U8 max_chn, SIZE_S stSize, VPSS_GRP VpssDstGrp[])
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VIDEO_FRAME_INFO_S stVideoFrame, stVideoFrameDump;
	CVI_VOID *vir_addr;
	char *ybuf, *ubuf;
	CVI_U8 u8ChidSize = PR2100_CHID_SIZE * max_chn;
	CVI_U32 firstline[max_chn], line100[max_chn], lastline[max_chn];

	RECT_S stRectOut[max_chn * 2]; // [channel][half]
	RECT_S stRectChn[VPSS_MAX_PHY_CHN_NUM];
	CVI_U32 i;
	CVI_U8 chn;

	char output[32];
	struct  timeval start;
	struct  timeval end;
	unsigned long diff;

	VPSS_GRP           VpssGrp[max_chn];
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN_ATTR_S    stVpssChnAttr;
	VPSS_CROP_INFO_S   stGrpCropInfo;

	for (CVI_S32 num = 0; num <= count; num++) {
		if (CVI_VI_GetChnFrame(0, 0, &stVideoFrame, 2000) != 0)
			break;
		gettimeofday(&start, NULL);
		snprintf(output, sizeof(output), "f%d.yuv", num);
		// SAMPLE_COMM_FRAME_SaveToFile(output, &stVideoFrame);
		CVI_U32 image_size = stVideoFrame.stVFrame.u32Length[0]
				  + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];
		printf("VI f%d (%d:%d)\n", num, stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height);
		vir_addr = CVI_SYS_Mmap(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		CVI_SYS_IonInvalidateCache(stVideoFrame.stVFrame.u64PhyAddr[0], vir_addr, image_size);

		ybuf = (char *)vir_addr;
		ubuf = (char *)(vir_addr + stVideoFrame.stVFrame.u32Length[0]);

		// parsing CHID
		for (chn = 0; chn < max_chn; chn++) {
			i = 0; // line0
			//chn lower
			firstline[chn] = PACKED_16BIT(ubuf[i*stVideoFrame.stVFrame.u32Stride[1] + chn],
				ybuf[i*stVideoFrame.stVFrame.u32Stride[0] + chn*2]);

			i = 100; // line#100
			//chn line#100 lower
			line100[chn] = PACKED_16BIT(ubuf[i*stVideoFrame.stVFrame.u32Stride[1] + chn],
				ybuf[i*stVideoFrame.stVFrame.u32Stride[0] + chn*2]);

			i = stVideoFrame.stVFrame.u32Height - 1; // last line
			//chn last line lower
			lastline[chn] = PACKED_16BIT(ubuf[i*stVideoFrame.stVFrame.u32Stride[1] + chn],
				ybuf[i*stVideoFrame.stVFrame.u32Stride[0] + chn*2]);
		}
		CVI_SYS_Munmap(vir_addr, image_size);

		// fill 4 quadrant rect
		for (chn = 0; chn < max_chn; chn++) {
			printf("ch%d lower frame #%d, ", chn, PR2100_FRAME_NUMBER(lastline[0]));
			//parsing line number

			// find lower part
			if (PR2100_LINE_VALID(lastline[chn]) == 1) {
				lastline[chn] = PR2100_LINE_NUMBER(lastline[chn]);
				printf("last line num=%d ", lastline[chn]);
				stRectOut[chn + max_chn].s32X = chn * stSize.u32Width + u8ChidSize;
				stRectOut[chn + max_chn].s32Y = stVideoFrame.stVFrame.u32Height - 1 - lastline[chn];
				stRectOut[chn + max_chn].u32Width = stSize.u32Width;
				stRectOut[chn + max_chn].u32Height = lastline[chn] + 1;

			} else { // has no low part
				stRectOut[chn + max_chn].u32Height = 0;
			}
			// find upper part
			stRectOut[chn].s32X = chn * stSize.u32Width + u8ChidSize;
			stRectOut[chn].u32Width = stSize.u32Width;
			if (PR2100_LINE_VALID(firstline[chn]) == 1) {
				firstline[chn] = PR2100_LINE_NUMBER(firstline[chn]);
				printf("line0 num=%d ", firstline[chn]);
				stRectOut[chn].s32Y = 0;
				stRectOut[chn].u32Height = stSize.u32Height - firstline[chn];

			} else if (PR2100_LINE_VALID(line100[chn]) == 1) {
				line100[chn] = PR2100_LINE_NUMBER(line100[chn]);
				printf("line100 num=%d ", line100[chn]);
				stRectOut[chn].s32Y = 100 - line100[chn];
				stRectOut[chn].u32Height = stVideoFrame.stVFrame.u32Height - 100 + line100[chn];
				if (stRectOut[chn].u32Height > stSize.u32Height) // has full frame
					stRectOut[chn].u32Height = stSize.u32Height;

			} else
				printf("ERROR: can't find valid line number\n");
			printf("\n");
		}
/*
 *		for (chn = 0; chn < max_chn; chn++) {
 *			CVI_TRACE_LOG(CVI_DBG_ERR,"ch%d upper part=%d:%d:%d:%d, lower part=%d:%d:%d:%d\n"
 *				, chn
 *				, stRectOut[chn].s32X, stRectOut[chn].s32Y
 *				, stRectOut[chn].u32Width, stRectOut[chn].u32Height
 *				, stRectOut[chn+max_chn].s32X, stRectOut[chn+max_chn].s32Y
 *				, stRectOut[chn+max_chn].u32Width, stRectOut[chn+max_chn].u32Height
 *				);
 *		}
 */
		// do crop & merge image
		/************************************************
		 * step2:  Init VPSS
		 ************************************************/
		for (chn = 0; chn < max_chn; chn++) {
			if (num == 0) {
				VpssGrp[chn] = CVI_VPSS_GetAvailableGrp();
				memset(&stVpssGrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));
				stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
				stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
				stVpssGrpAttr.enPixelFormat                  = stVideoFrame.stVFrame.enPixelFormat;
				stVpssGrpAttr.u32MaxW                        = stVideoFrame.stVFrame.u32Width;
				stVpssGrpAttr.u32MaxH                        = stVideoFrame.stVFrame.u32Height;
				stVpssGrpAttr.u8VpssDev                      = 0;

				s32Ret = CVI_VPSS_CreateGrp(VpssGrp[chn], &stVpssGrpAttr);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_CreateGrp(grp:%d) failed with %#x!\n"
							, VpssGrp[chn], s32Ret);
					return CVI_FAILURE;
				}

				s32Ret = CVI_VPSS_StartGrp(VpssGrp[chn]);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_StartGrp failed with %#x\n", s32Ret);
					return CVI_FAILURE;
				}
			}

			if (num > 0) {
				memcpy(&stRectChn[chn], &stRectOut[chn], sizeof(RECT_S));

				/* set grp crop info */
				memset(&stGrpCropInfo, 0, sizeof(VPSS_CROP_INFO_S));
				stGrpCropInfo.bEnable = CVI_TRUE;
				stGrpCropInfo.stCropRect.s32X = stRectChn[chn].s32X;
				stGrpCropInfo.stCropRect.s32Y = stRectChn[chn].s32Y;
				stGrpCropInfo.stCropRect.u32Width = stRectChn[chn].u32Width;
				stGrpCropInfo.stCropRect.u32Height = stRectChn[chn].u32Height;
				CVI_VPSS_SetGrpCrop(VpssGrp[chn], &stGrpCropInfo);
				/* set chn attr info */
				memset(&stVpssChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));
				stVpssChnAttr.u32Width                    = stSize.u32Width;
				stVpssChnAttr.u32Height                   = stSize.u32Height;
				stVpssChnAttr.enVideoFormat               = VIDEO_FORMAT_LINEAR;
				stVpssChnAttr.enPixelFormat               = stVideoFrame.stVFrame.enPixelFormat;
				stVpssChnAttr.stFrameRate.s32SrcFrameRate = -1;
				stVpssChnAttr.stFrameRate.s32DstFrameRate = -1;
				stVpssChnAttr.u32Depth                    = 1;
				stVpssChnAttr.bMirror                     = CVI_FALSE;
				stVpssChnAttr.bFlip                       = CVI_FALSE;
				stVpssChnAttr.stNormalize.bEnable         = CVI_FALSE;
				stVpssChnAttr.stAspectRatio.enMode        = ASPECT_RATIO_MANUAL;
				stVpssChnAttr.stAspectRatio.stVideoRect.s32X = 0;
				stVpssChnAttr.stAspectRatio.stVideoRect.s32Y =
								stSize.u32Height - stRectChn[chn].u32Height;
				stVpssChnAttr.stAspectRatio.stVideoRect.u32Width  = stRectChn[chn].u32Width;
				stVpssChnAttr.stAspectRatio.stVideoRect.u32Height = stRectChn[chn].u32Height;

				s32Ret = CVI_VPSS_SetChnAttr(VpssGrp[chn], 0, &stVpssChnAttr);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
					return CVI_FAILURE;
				}

				s32Ret = CVI_VPSS_EnableChn(VpssGrp[chn], 0);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_EnableChn failed with %#x\n", s32Ret);
					return CVI_FAILURE;
				}

				CVI_VPSS_SendFrame(VpssGrp[chn], &stVideoFrame, -1);

				s32Ret = CVI_VPSS_GetChnFrame(VpssGrp[chn], 0, &stVideoFrameDump, 1000);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR
						, "CVI_VPSS_GetChnFrame for grp%d chn%d. s32Ret: 0x%x !\n"
						, VpssGrp[chn], 0, s32Ret);
				} else {
					snprintf(output, sizeof(output), "f%d-ch%d.yuv", num, chn);
					// SAMPLE_COMM_FRAME_SaveToFile(output, &stVideoFrameDump);
					CVI_VPSS_SendFrame(VpssDstGrp[chn], &stVideoFrameDump, -1);
					CVI_VPSS_ReleaseChnFrame(VpssGrp[chn], 0, &stVideoFrameDump);
				}
			}

			memcpy(&stRectChn[chn], &stRectOut[chn + max_chn], sizeof(RECT_S));

			if (stRectChn[chn].u32Height == 0) {
				s32Ret = CVI_VPSS_DisableChn(VpssGrp[chn], 0);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_DisableChn failed with %#x\n", s32Ret);
					return CVI_FAILURE;
				}
			} else {
				/* set grp crop info */
				memset(&stGrpCropInfo, 0, sizeof(VPSS_CROP_INFO_S));
				stGrpCropInfo.bEnable = CVI_TRUE;
				stGrpCropInfo.stCropRect.s32X = stRectChn[chn].s32X;
				stGrpCropInfo.stCropRect.s32Y = stRectChn[chn].s32Y;
				stGrpCropInfo.stCropRect.u32Width = stRectChn[chn].u32Width;
				stGrpCropInfo.stCropRect.u32Height = stRectChn[chn].u32Height;
				CVI_VPSS_SetGrpCrop(VpssGrp[chn], &stGrpCropInfo);
				/* set chn attr info */
				memset(&stVpssChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));
				stVpssChnAttr.u32Width                    = stSize.u32Width;
				stVpssChnAttr.u32Height                   = stSize.u32Height;
				stVpssChnAttr.enVideoFormat               = VIDEO_FORMAT_LINEAR;
				stVpssChnAttr.enPixelFormat               = stVideoFrame.stVFrame.enPixelFormat;
				stVpssChnAttr.stFrameRate.s32SrcFrameRate = -1;
				stVpssChnAttr.stFrameRate.s32DstFrameRate = -1;
				stVpssChnAttr.u32Depth                    = 1;
				stVpssChnAttr.bMirror                     = CVI_FALSE;
				stVpssChnAttr.bFlip                       = CVI_FALSE;
				stVpssChnAttr.stNormalize.bEnable         = CVI_FALSE;
				stVpssChnAttr.stAspectRatio.enMode        = ASPECT_RATIO_MANUAL;
				stVpssChnAttr.stAspectRatio.stVideoRect.s32X = 0;
				stVpssChnAttr.stAspectRatio.stVideoRect.s32Y = 0;
				stVpssChnAttr.stAspectRatio.stVideoRect.u32Width  = stRectChn[chn].u32Width;
				stVpssChnAttr.stAspectRatio.stVideoRect.u32Height = stRectChn[chn].u32Height;

				s32Ret = CVI_VPSS_SetChnAttr(VpssGrp[chn], 0, &stVpssChnAttr);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
					return CVI_FAILURE;
				}

				s32Ret = CVI_VPSS_EnableChn(VpssGrp[chn], 0);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_EnableChn failed with %#x\n", s32Ret);
					return CVI_FAILURE;
				}

				CVI_VPSS_SendFrame(VpssGrp[chn], &stVideoFrame, -1);

				s32Ret = CVI_VPSS_GetChnFrame(VpssGrp[chn], 0, &stVideoFrameDump, 1000);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR
						, "CVI_VPSS_GetChnFrame for grp%d chn%d. s32Ret: 0x%x !\n"
						, VpssGrp[chn], 0, s32Ret);
				} else {
					CVI_VPSS_SendChnFrame(VpssGrp[chn], 0, &stVideoFrameDump, -1);
					CVI_VPSS_ReleaseChnFrame(VpssGrp[chn], 0, &stVideoFrameDump);
				}
			}

			if (num == count) {
				if (stRectChn[chn].u32Height != 0) {
					s32Ret = CVI_VPSS_DisableChn(VpssGrp[chn], 0);
					if (s32Ret != CVI_SUCCESS) {
						CVI_TRACE_LOG(CVI_DBG_ERR, "failed with %#x!\n", s32Ret);
						return CVI_FAILURE;
					}
				}
				s32Ret = CVI_VPSS_StopGrp(VpssGrp[chn]);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "failed with %#x!\n", s32Ret);
					return CVI_FAILURE;
				}
				s32Ret = CVI_VPSS_DestroyGrp(VpssGrp[chn]);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "failed with %#x!\n", s32Ret);
					return CVI_FAILURE;
				}
			}
		}

		if (CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame) != 0)
			SAMPLE_PRT("CVI_VI_ReleaseChnFrame NG\n");

		gettimeofday(&end, NULL);
		diff = 1000000 * (end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
		SAMPLE_PRT("f%d cost time %ld us\n", num, diff);
	}

	return CVI_SUCCESS;
}

CVI_S32 pr2100_output_quick_by_mem(CVI_S32 count, CVI_U8 max_chn, SIZE_S stSize, VPSS_GRP VpssDstGrp[])
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VIDEO_FRAME_INFO_S stVideoFrame;
	CVI_VOID *vir_addr;
	char *ybuf, *ubuf;
	CVI_U8 u8ChidSize = PR2100_CHID_SIZE * max_chn;
	CVI_U32 firstline[max_chn], line100[max_chn], lastline[max_chn];

	RECT_S stRectOut[max_chn * 2]; // [channel][half]
	CVI_U32 i;
	CVI_U8 chn;

	char output[32];
	struct  timeval start;
	struct  timeval end;
	unsigned long diff;

	CVI_VOID *src_addr, *dst_addr;
	CVI_U32 u32Offset;
	CVI_U32 u32Lens;
	CVI_U32 u32FrameNum = max_chn * 2;
	VIDEO_FRAME_INFO_S stVideoFrameDst[u32FrameNum];
	VIDEO_FRAME_INFO_S stVideoFrameTmp;

	for (CVI_S32 num = 0; num <= count; num++) {
		if (CVI_VI_GetChnFrame(0, 0, &stVideoFrame, 2000) != 0)
			break;
		gettimeofday(&start, NULL);
		snprintf(output, sizeof(output), "f%d.yuv", num);
		// SAMPLE_COMM_FRAME_SaveToFile(output, &stVideoFrame);
		CVI_U32 image_size = stVideoFrame.stVFrame.u32Length[0]
				  + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];
		printf("VI f%d (%d:%d)\n", num, stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height);
		vir_addr = CVI_SYS_MmapCache(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);

		ybuf = (char *)vir_addr;
		ubuf = (char *)(vir_addr + stVideoFrame.stVFrame.u32Length[0]);

		// parsing CHID
		for (chn = 0; chn < max_chn; chn++) {
			i = 0; // line0
			//chn lower
			firstline[chn] = PACKED_16BIT(ubuf[i*stVideoFrame.stVFrame.u32Stride[1] + chn],
				ybuf[i*stVideoFrame.stVFrame.u32Stride[0] + chn*2]);

			i = 100; // line#100
			//chn line#100 lower
			line100[chn] = PACKED_16BIT(ubuf[i*stVideoFrame.stVFrame.u32Stride[1] + chn],
				ybuf[i*stVideoFrame.stVFrame.u32Stride[0] + chn*2]);

			i = stVideoFrame.stVFrame.u32Height - 1; // last line
			//chn last line lower
			lastline[chn] = PACKED_16BIT(ubuf[i*stVideoFrame.stVFrame.u32Stride[1] + chn],
				ybuf[i*stVideoFrame.stVFrame.u32Stride[0] + chn*2]);
		}
		CVI_SYS_Munmap(vir_addr, image_size);

		// fill 4 quadrant rect
		for (chn = 0; chn < max_chn; chn++) {
			printf("ch%d lower frame #%d, ", chn, PR2100_FRAME_NUMBER(lastline[0]));
			//parsing line number

			// find lower part
			if (PR2100_LINE_VALID(lastline[chn]) == 1) {
				lastline[chn] = PR2100_LINE_NUMBER(lastline[chn]);
				printf("last line num=%d ", lastline[chn]);
				stRectOut[chn + max_chn].s32X = chn * stSize.u32Width + u8ChidSize;
				stRectOut[chn + max_chn].s32Y = stVideoFrame.stVFrame.u32Height - 1 - lastline[chn];
				stRectOut[chn + max_chn].u32Width = stSize.u32Width;
				stRectOut[chn + max_chn].u32Height = lastline[chn] + 1;

			} else { // has no low part
				stRectOut[chn + max_chn].u32Height = 0;
			}
			// find upper part
			stRectOut[chn].s32X = chn * stSize.u32Width + u8ChidSize;
			stRectOut[chn].u32Width = stSize.u32Width;
			if (PR2100_LINE_VALID(firstline[chn]) == 1) {
				firstline[chn] = PR2100_LINE_NUMBER(firstline[chn]);
				printf("line0 num=%d ", firstline[chn]);
				stRectOut[chn].s32Y = 0;
				stRectOut[chn].u32Height = stSize.u32Height - firstline[chn];

			} else if (PR2100_LINE_VALID(line100[chn]) == 1) {
				line100[chn] = PR2100_LINE_NUMBER(line100[chn]);
				printf("line100 num=%d ", line100[chn]);
				stRectOut[chn].s32Y = 100 - line100[chn];
				stRectOut[chn].u32Height = stVideoFrame.stVFrame.u32Height - 100 + line100[chn];
				if (stRectOut[chn].u32Height > stSize.u32Height) // has full frame
					stRectOut[chn].u32Height = stSize.u32Height;

			} else
				printf("ERROR: can't find valid line number\n");
			printf("\n");
		}
/*
 *		for (chn = 0; chn < max_chn; chn++) {
 *			CVI_TRACE_LOG(CVI_DBG_ERR,"ch%d upper part=%d:%d:%d:%d, lower part=%d:%d:%d:%d\n"
 *				, chn
 *				, stRectOut[chn].s32X, stRectOut[chn].s32Y
 *				, stRectOut[chn].u32Width, stRectOut[chn].u32Height
 *				, stRectOut[chn+max_chn].s32X, stRectOut[chn+max_chn].s32Y
 *				, stRectOut[chn+max_chn].u32Width, stRectOut[chn+max_chn].u32Height
 *				);
 *		}
 */
		// do crop & merge image
		if (num == 0) {
			for (i = 0; i < u32FrameNum; ++i) {
				s32Ret = SAMPLE_COMM_PrepareFrame(stSize, stVideoFrame.stVFrame.enPixelFormat,
								&stVideoFrameDst[i]);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("SAMPLE_COMM_PrepareFrame(%d) failed with %#x\n", i, s32Ret);
					return s32Ret;
				}
			}
		}

		for (chn = 0; chn < max_chn; chn++) {
			for (CVI_U8 i = 0; i < 3; ++i) {
				stVideoFrame.stVFrame.pu8VirAddr[i]
					= CVI_SYS_MmapCache(stVideoFrame.stVFrame.u64PhyAddr[i]
							, stVideoFrame.stVFrame.u32Length[i]);
				stVideoFrameDst[chn].stVFrame.pu8VirAddr[i]
					= CVI_SYS_MmapCache(stVideoFrameDst[chn].stVFrame.u64PhyAddr[i]
							, stVideoFrameDst[chn].stVFrame.u32Length[i]);
				stVideoFrameDst[chn + max_chn].stVFrame.pu8VirAddr[i]
					= CVI_SYS_MmapCache(stVideoFrameDst[chn + max_chn].stVFrame.u64PhyAddr[i]
							, stVideoFrameDst[chn + max_chn].stVFrame.u32Length[i]);
				// frame N+1 top
				if (num > 0) {
					src_addr = stVideoFrame.stVFrame.pu8VirAddr[i];
					dst_addr = stVideoFrameDst[chn].stVFrame.pu8VirAddr[i];

					u32Offset = (i == 0)
							? (stRectOut[chn].s32X)
							: (stRectOut[chn].s32X)/2;
					u32Lens = (i == 0)
							? (stRectOut[chn].u32Width)
							: (stRectOut[chn].u32Width)/2;

					src_addr += stVideoFrame.stVFrame.u32Stride[i]
						    * stRectOut[chn].s32Y;
					dst_addr += stVideoFrameDst[chn].stVFrame.u32Stride[i]
						    * (stVideoFrameDst[chn].stVFrame.u32Height
						       - stRectOut[chn].u32Height);

					for (CVI_U32 j = 0; j < stRectOut[chn].u32Height; ++j) {
						memcpy(dst_addr, src_addr + u32Offset, u32Lens);
						src_addr += stVideoFrame.stVFrame.u32Stride[i];
						dst_addr += stVideoFrameDst[chn].stVFrame.u32Stride[i];
					}
				}
				// frame N bottom
				src_addr = stVideoFrame.stVFrame.pu8VirAddr[i];
				dst_addr = stVideoFrameDst[chn + max_chn].stVFrame.pu8VirAddr[i];

				u32Offset = (i == 0)
						? (stRectOut[chn + max_chn].s32X)
						: (stRectOut[chn + max_chn].s32X)/2;
				u32Lens = (i == 0)
						? (stRectOut[chn + max_chn].u32Width)
						: (stRectOut[chn + max_chn].u32Width)/2;

				src_addr += stVideoFrame.stVFrame.u32Stride[i]
					    * stRectOut[chn + max_chn].s32Y;

				for (CVI_U32 j = 0; j < stRectOut[chn + max_chn].u32Height; ++j) {
					memcpy(dst_addr, src_addr + u32Offset, u32Lens);
					src_addr += stVideoFrame.stVFrame.u32Stride[i];
					dst_addr += stVideoFrameDst[chn + max_chn].stVFrame.u32Stride[i];
				}

				CVI_SYS_Munmap(stVideoFrameDst[chn + max_chn].stVFrame.pu8VirAddr[i]
						, stVideoFrameDst[chn + max_chn].stVFrame.u32Length[i]);
				CVI_SYS_Munmap(stVideoFrameDst[chn].stVFrame.pu8VirAddr[i]
						, stVideoFrameDst[chn].stVFrame.u32Length[i]);
				CVI_SYS_Munmap(stVideoFrame.stVFrame.pu8VirAddr[i]
						, stVideoFrame.stVFrame.u32Length[i]);
			}

			stVideoFrameDst[chn + max_chn].stVFrame.u64PTS = stVideoFrame.stVFrame.u64PTS;

			if (num > 0) {
				snprintf(output, sizeof(output), "f%d-ch%d.yuv", num, chn);
				// SAMPLE_COMM_FRAME_SaveToFile(output, &stVideoFrameDst[chn]);
				s32Ret = CVI_VPSS_SendFrame(VpssDstGrp[chn], &stVideoFrameDst[chn], -1);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("CVI_VPSS_SendFrame failed with %#x\n", s32Ret);
					return s32Ret;
				}
			}
			// stVideoFrameDst swap
			stVideoFrameTmp = stVideoFrameDst[chn + max_chn];
			stVideoFrameDst[chn + max_chn] = stVideoFrameDst[chn];
			stVideoFrameDst[chn] = stVideoFrameTmp;
		}

		if (num == count) {
			for (i = 0; i < u32FrameNum; ++i) {
				VB_BLK blk;

				blk = CVI_VB_PhysAddr2Handle(stVideoFrameDst[i].stVFrame.u64PhyAddr[0]);
				if (blk != VB_INVALID_HANDLE) {
					CVI_VB_ReleaseBlock(blk);
				}
			}
		}

		if (CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame) != 0)
			SAMPLE_PRT("CVI_VI_ReleaseChnFrame NG\n");

		gettimeofday(&end, NULL);
		diff = 1000000 * (end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
		SAMPLE_PRT("f%d cost time %ld us\n", num, diff);
	}

	return CVI_SUCCESS;
}

#ifdef IVE_SUPPORT
#include "ive.h"
CVI_S32 pr2100_output_quick_by_ive(CVI_S32 count, CVI_U8 max_chn, SIZE_S stSize, VPSS_GRP VpssDstGrp[])
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VIDEO_FRAME_INFO_S stVideoFrame;
	CVI_VOID *vir_addr;
	char *ybuf, *ubuf;
	CVI_U8 u8ChidSize = PR2100_CHID_SIZE * max_chn;
	CVI_U32 firstline[max_chn], line100[max_chn], lastline[max_chn];

	RECT_S stRectOut[max_chn * 2]; // [channel][half]
	RECT_S stRectChn[VPSS_MAX_PHY_CHN_NUM];
	CVI_U32 i;
	CVI_U8 chn;

	char output[32];
	struct  timeval start;
	struct  timeval end;
	unsigned long diff;

	VIDEO_FRAME_INFO_S stVideoFrameDst[max_chn];

	IVE_HANDLE handle = CVI_NULL;
	IVE_IMAGE_S stImgSrc;
	IVE_IMAGE_S stImgDst[max_chn];
	IVE_IMAGE_S stImgSrcCropTop, stImgSrcCropBottom;
	IVE_IMAGE_S stImgDstCropTop, stImgDstCropBottom;
	IVE_DMA_CTRL_S stDmaCtrl = {IVE_DMA_MODE_DIRECT_COPY};
	CVI_U16 x1, y1, x2, y2;

	for (CVI_S32 num = 0; num <= count; num++) {
		if (CVI_VI_GetChnFrame(0, 0, &stVideoFrame, 2000) != 0)
			break;
		gettimeofday(&start, NULL);
		snprintf(output, sizeof(output), "f%d.yuv", num);
		// SAMPLE_COMM_FRAME_SaveToFile(output, &stVideoFrame);
		// sleep(3);
		// ive_copy(output, stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height);
		CVI_U32 image_size = stVideoFrame.stVFrame.u32Length[0]
				  + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];
		printf("VI f%d (%d:%d)\n", num, stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height);
		vir_addr = CVI_SYS_MmapCache(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		stVideoFrame.stVFrame.pu8VirAddr[0] = vir_addr;

		ybuf = (char *)vir_addr;
		ubuf = (char *)(vir_addr + stVideoFrame.stVFrame.u32Length[0]);

		// parsing CHID
		for (chn = 0; chn < max_chn; chn++) {
			i = 0; // line0
			//chn lower
			firstline[chn] = PACKED_16BIT(ubuf[i*stVideoFrame.stVFrame.u32Stride[1] + chn],
				ybuf[i*stVideoFrame.stVFrame.u32Stride[0] + chn*2]);

			i = 100; // line#100
			//chn line#100 lower
			line100[chn] = PACKED_16BIT(ubuf[i*stVideoFrame.stVFrame.u32Stride[1] + chn],
				ybuf[i*stVideoFrame.stVFrame.u32Stride[0] + chn*2]);

			i = stVideoFrame.stVFrame.u32Height - 1; // last line
			//chn last line lower
			lastline[chn] = PACKED_16BIT(ubuf[i*stVideoFrame.stVFrame.u32Stride[1] + chn],
				ybuf[i*stVideoFrame.stVFrame.u32Stride[0] + chn*2]);
		}
		CVI_SYS_Munmap(vir_addr, image_size);

		// fill 4 quadrant rect
		for (chn = 0; chn < max_chn; chn++) {
			printf("ch%d lower frame #%d, ", chn, PR2100_FRAME_NUMBER(lastline[0]));
			//parsing line number

			// find lower part
			if (PR2100_LINE_VALID(lastline[chn]) == 1) {
				lastline[chn] = PR2100_LINE_NUMBER(lastline[chn]);
				printf("last line num=%d ", lastline[chn]);
				stRectOut[chn + max_chn].s32X = chn * stSize.u32Width + u8ChidSize;
				stRectOut[chn + max_chn].s32Y = stVideoFrame.stVFrame.u32Height - 1 - lastline[chn];
				stRectOut[chn + max_chn].u32Width = stSize.u32Width;
				stRectOut[chn + max_chn].u32Height = lastline[chn] + 1;

			} else { // has no low part
				stRectOut[chn + max_chn].u32Height = 0;
			}
			// find upper part
			stRectOut[chn].s32X = chn * stSize.u32Width + u8ChidSize;
			stRectOut[chn].u32Width = stSize.u32Width;
			if (PR2100_LINE_VALID(firstline[chn]) == 1) {
				firstline[chn] = PR2100_LINE_NUMBER(firstline[chn]);
				printf("line0 num=%d ", firstline[chn]);
				stRectOut[chn].s32Y = 0;
				stRectOut[chn].u32Height = stSize.u32Height - firstline[chn];

			} else if (PR2100_LINE_VALID(line100[chn]) == 1) {
				line100[chn] = PR2100_LINE_NUMBER(line100[chn]);
				printf("line100 num=%d ", line100[chn]);
				stRectOut[chn].s32Y = 100 - line100[chn];
				stRectOut[chn].u32Height = stVideoFrame.stVFrame.u32Height - 100 + line100[chn];
				if (stRectOut[chn].u32Height > stSize.u32Height) // has full frame
					stRectOut[chn].u32Height = stSize.u32Height;

			} else
				printf("ERROR: can't find valid line number\n");
			printf("\n");
		}
/*
 *		for (chn = 0; chn < max_chn; chn++) {
 *			CVI_TRACE_LOG(CVI_DBG_ERR,"ch%d upper part=%d:%d:%d:%d, lower part=%d:%d:%d:%d\n"
 *				, chn
 *				, stRectOut[chn].s32X, stRectOut[chn].s32Y
 *				, stRectOut[chn].u32Width, stRectOut[chn].u32Height
 *				, stRectOut[chn+max_chn].s32X, stRectOut[chn+max_chn].s32Y
 *				, stRectOut[chn+max_chn].u32Width, stRectOut[chn+max_chn].u32Height
 *				);
 *		}
 */
		// do crop & merge image
		if (num == 0) {
			handle = CVI_IVE_CreateHandle();
			if (handle == CVI_NULL) {
				SAMPLE_PRT("CVI_IVE_CreateHandle failed\n");
				return s32Ret;
			}

			for (i = 0; i < max_chn; ++i) {
				s32Ret = SAMPLE_COMM_PrepareFrame(stSize, stVideoFrame.stVFrame.enPixelFormat,
								&stVideoFrameDst[i]);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("SAMPLE_COMM_PrepareFrame(%d) failed with %#x\n", i, s32Ret);
					return s32Ret;
				}

				CVI_U32 image_size = stVideoFrameDst[i].stVFrame.u32Length[0]
								+ stVideoFrameDst[i].stVFrame.u32Length[1]
								+ stVideoFrameDst[i].stVFrame.u32Length[2];
				vir_addr = CVI_SYS_MmapCache(stVideoFrameDst[i].stVFrame.u64PhyAddr[0], image_size);
				stVideoFrameDst[i].stVFrame.pu8VirAddr[0] = vir_addr;
				CVI_SYS_Munmap(vir_addr, image_size);

				memset(&stImgDst[i], 0, sizeof(IVE_DST_IMAGE_S));
				s32Ret = CVI_IVE_VideoFrameInfo2Image(&stVideoFrameDst[i], &stImgDst[i]);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("CVI_IVE_VideoFrameInfo2Image(%d) dst failed with %#x\n", i, s32Ret);
					return s32Ret;
				}
			}
		}

		memset(&stImgSrc, 0, sizeof(IVE_DST_IMAGE_S));
		s32Ret = CVI_IVE_VideoFrameInfo2Image(&stVideoFrame, &stImgSrc);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_IVE_VideoFrameInfo2Image(%d) src failed with %#x\n", num, s32Ret);
			return s32Ret;
		}

		for (chn = 0; chn < max_chn; chn++) {
			// frame N+1 top
			if (num > 0) {
				memcpy(&stRectChn[chn], &stRectOut[chn], sizeof(RECT_S));

				x1 = (CVI_U16)stRectChn[chn].s32X;
				y1 = (CVI_U16)stRectChn[chn].s32Y;
				x2 = (CVI_U16)(stRectChn[chn].s32X + stRectChn[chn].u32Width);
				y2 = (CVI_U16)(stRectChn[chn].s32Y + stRectChn[chn].u32Height);
				memset(&stImgSrcCropTop, 0, sizeof(IVE_DST_IMAGE_S));
				s32Ret = CVI_IVE_SubImage(handle, &stImgSrc, &stImgSrcCropTop, x1, y1, x2, y2);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("CVI_IVE_SubImage(%d) src failed with %#x\n", chn, s32Ret);
					return s32Ret;
				}

				x1 = 0;
				y1 = (CVI_U16)(stSize.u32Height - stRectChn[chn].u32Height);
				x2 = (CVI_U16)stRectChn[chn].u32Width;
				y2 = (CVI_U16)stSize.u32Height;
				memset(&stImgDstCropBottom, 0, sizeof(IVE_DST_IMAGE_S));
				s32Ret = CVI_IVE_SubImage(handle, &stImgDst[chn], &stImgDstCropBottom, x1, y1, x2, y2);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("CVI_IVE_SubImage(%d) dst failed with %#x\n", chn, s32Ret);
					return s32Ret;
				}

				s32Ret = CVI_IVE_DMA(handle, &stImgSrcCropTop, &stImgDstCropBottom, &stDmaCtrl, 0);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("CVI_IVE_DMA(%d) failed with %#x\n", chn, s32Ret);
					return s32Ret;
				}

				stVideoFrameDst[chn].stVFrame.u64PTS = stVideoFrame.stVFrame.u64PTS;
				snprintf(output, sizeof(output), "f%d-ch%d.yuv", num, chn);
				// SAMPLE_COMM_FRAME_SaveToFile(output, &stVideoFrameDst[chn]);
				s32Ret = CVI_VPSS_SendFrame(VpssDstGrp[chn], &stVideoFrameDst[chn], -1);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("CVI_VPSS_SendFrame failed with %#x\n", s32Ret);
					return s32Ret;
				}

				CVI_SYS_FreeI(handle, &stImgDstCropBottom);
				CVI_SYS_FreeI(handle, &stImgSrcCropTop);
			}
			// frame N bottom
			memcpy(&stRectChn[chn], &stRectOut[chn + max_chn], sizeof(RECT_S));

			if (stRectChn[chn].u32Height != 0) {
				x1 = (CVI_U16)stRectChn[chn].s32X;
				y1 = (CVI_U16)stRectChn[chn].s32Y;
				x2 = (CVI_U16)(stRectChn[chn].s32X + stRectChn[chn].u32Width);
				y2 = (CVI_U16)(stRectChn[chn].s32Y + stRectChn[chn].u32Height);
				memset(&stImgSrcCropBottom, 0, sizeof(IVE_DST_IMAGE_S));
				s32Ret = CVI_IVE_SubImage(handle, &stImgSrc, &stImgSrcCropBottom, x1, y1, x2, y2);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("CVI_IVE_SubImage(%d) src failed with %#x\n", chn, s32Ret);
					return s32Ret;
				}

				x1 = 0;
				y1 = 0;
				x2 = (CVI_U16)stRectChn[chn].u32Width;
				y2 = (CVI_U16)stRectChn[chn].u32Height;
				memset(&stImgDstCropTop, 0, sizeof(IVE_DST_IMAGE_S));
				s32Ret = CVI_IVE_SubImage(handle, &stImgDst[chn], &stImgDstCropTop, x1, y1, x2, y2);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("CVI_IVE_SubImage(%d) dst failed with %#x\n", chn, s32Ret);
					return s32Ret;
				}

				s32Ret = CVI_IVE_DMA(handle, &stImgSrcCropBottom, &stImgDstCropTop, &stDmaCtrl, 0);
				if (s32Ret != CVI_SUCCESS) {
					SAMPLE_PRT("CVI_IVE_DMA(%d) failed with %#x\n", chn, s32Ret);
					return s32Ret;
				}

				CVI_SYS_FreeI(handle, &stImgDstCropTop);
				CVI_SYS_FreeI(handle, &stImgSrcCropBottom);
			}
		}

		CVI_SYS_FreeI(handle, &stImgSrc);

		if (num == count) {
			for (i = 0; i < max_chn; ++i) {
				VB_BLK blk;

				blk = CVI_VB_PhysAddr2Handle(stVideoFrameDst[i].stVFrame.u64PhyAddr[0]);
				if (blk != VB_INVALID_HANDLE) {
					CVI_VB_ReleaseBlock(blk);
				}

				CVI_SYS_FreeI(handle, &stImgDst[i]);
			}

			s32Ret = CVI_IVE_DestroyHandle(handle);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("CVI_IVE_DestroyHandle failed with %#x\n", s32Ret);
				return s32Ret;
			}
		}

		if (CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame) != 0)
			SAMPLE_PRT("CVI_VI_ReleaseChnFrame NG\n");

		gettimeofday(&end, NULL);
		diff = 1000000 * (end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
		SAMPLE_PRT("f%d cost time %ld us\n", num, diff);
	}

	return CVI_SUCCESS;
}
#endif

static CVI_VOID *thread_send_vo(CVI_VOID *arg)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	VIDEO_FRAME_INFO_S stVideoFrame, stOverlayFrame;
	SAMPLE_VPSS_CONFIG_S *pstVpssCfg = (SAMPLE_VPSS_CONFIG_S *)arg;
	SAMPLE_VPSS_PARAM_S *pstVpssChn;
	VPSS_CHN_ATTR_S stChnAttr;
	VO_LAYER VoLayer = 0;
	VO_CHN VoChn = 0;

	prctl(PR_SET_NAME, "thread_send_vo");
	while (!pstVpssCfg->stop_thread) {
		int bFirstFrame = CVI_TRUE;

		for (CVI_S32 i = 0; i < pstVpssCfg->s32ChnNum; ++i) {
			pstVpssChn = &pstVpssCfg->astVpssParam[i];

			if (!bFirstFrame) {
				s32Ret = CVI_VPSS_SendChnFrame(pstVpssChn->DstGrp, pstVpssChn->DstChn
								, &stOverlayFrame, 1000);
				CVI_VPSS_ReleaseChnFrame(pstVpssChn->DstGrp, pstVpssChn->DstChn, &stOverlayFrame);
				if (s32Ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_SendChnFrame Grp%d Chn%d fail\n"
							, pstVpssChn->DstGrp, pstVpssChn->DstChn);
					continue;
				}
			}

			s32Ret = CVI_VPSS_GetChnFrame(pstVpssChn->SrcGrp, pstVpssChn->SrcChn, &stVideoFrame, 1000);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_GetChnFrame Grp%d Chn%d fail\n"
						, pstVpssChn->SrcGrp, pstVpssChn->SrcChn);
				continue;
			}

			CVI_VPSS_GetChnAttr(pstVpssChn->DstGrp, pstVpssChn->DstChn, &stChnAttr);
			stChnAttr.stAspectRatio.enMode           = ASPECT_RATIO_MANUAL;
			stChnAttr.stAspectRatio.stVideoRect.s32X = pstVpssChn->stDispRect.s32X;
			stChnAttr.stAspectRatio.stVideoRect.s32Y = pstVpssChn->stDispRect.s32Y;
			stChnAttr.stAspectRatio.stVideoRect.u32Width  = pstVpssChn->stDispRect.u32Width;
			stChnAttr.stAspectRatio.stVideoRect.u32Height = pstVpssChn->stDispRect.u32Height;
			CVI_VPSS_SetChnAttr(pstVpssChn->DstGrp, pstVpssChn->DstChn, &stChnAttr);

			s32Ret = CVI_VPSS_SendFrame(pstVpssChn->DstGrp, &stVideoFrame, 1000);
			CVI_VPSS_ReleaseChnFrame(pstVpssChn->SrcGrp, pstVpssChn->SrcChn, &stVideoFrame);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_SendFrame Grp%d fail\n"
						, pstVpssChn->DstGrp);
				continue;
			}

			s32Ret = CVI_VPSS_GetChnFrame(pstVpssChn->DstGrp, pstVpssChn->DstChn, &stOverlayFrame, 1000);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_GetChnFrame Grp%d Chn%d fail\n"
						, pstVpssChn->SrcGrp, pstVpssChn->SrcChn);
				continue;
			}
			bFirstFrame = CVI_FALSE;
		}
		if (bFirstFrame == CVI_TRUE) {
			continue;
		}

		s32Ret = CVI_VO_SendFrame(VoLayer, VoChn, &stOverlayFrame, 1000);
		CVI_VPSS_ReleaseChnFrame(pstVpssChn->DstGrp, pstVpssChn->DstChn, &stOverlayFrame);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VO_SendFrame faile\n");
		}
	}
	pthread_exit(NULL);
}

CVI_S32 SAMPLE_PR2100_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat	    = PIXEL_FORMAT_YUV_PLANAR_422;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U8		u8MaxChn;
	CVI_U32		u32BlkSize, u32BlkRotSize;
	SIZE_S		stSizeSns, stSizeChn, stSizeOut;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		// .enSnsType = PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT,
		.enSnsType[0] = PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.MipiDev[0]   = 0xff,
		.s32BusId[0]  = 0,
	};

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_DEBUG;
	CVI_LOG_SetLevelConf(&log_conf);

	if (stIniCfg.enSnsType[0] == PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT) {
		u8MaxChn = 2;
	} else if (stIniCfg.enSnsType[0] == PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT) {
		u8MaxChn = 4;
	}
	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = 0;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stSizeChn.u32Width = PR2100_WIDTH;
	stSizeChn.u32Height = PR2100_HEIGHT;

	// stSizeOut.u32Width = 1920;
	// stSizeOut.u32Height = 440;
	stSizeOut.u32Width = 1280;
	stSizeOut.u32Height = 720;
	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 6;

	u32BlkSize = COMMON_GetPicBufferSize(stSizeSns.u32Width, stSizeSns.u32Height, enPixFormat,
						DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 4;
	stVbConf.astCommPool[0].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(stSizeChn.u32Width, stSizeChn.u32Height, enPixFormat,
						DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[1].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[1].u32BlkCnt	= 8;
	stVbConf.astCommPool[1].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[1] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(stSizeOut.u32Width, stSizeOut.u32Height, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkRotSize = COMMON_GetPicBufferSize(stSizeOut.u32Height, stSizeOut.u32Width, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX(u32BlkSize, u32BlkRotSize);
	stVbConf.astCommPool[2].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[2].u32BlkCnt	= 6;
	stVbConf.astCommPool[2].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[2] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(1920, 1080, SAMPLE_PIXEL_FORMAT,
						DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[3].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[3].u32BlkCnt	= 2;
	stVbConf.astCommPool[3].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[3] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(640, 480, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[4].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[4].u32BlkCnt	= 4;
	stVbConf.astCommPool[4].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[4] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(320, 240, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[5].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[5].u32BlkCnt	= 2;
	stVbConf.astCommPool[5].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[5] BlkSize %d\n", u32BlkSize);
	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_SYS_SetVPSSMode(VPSS_MODE_DUAL);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_SYS_SetVPSSMode failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	sleep(1);

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	// snr0
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = 640;//stSizeChn.u32Width;
	stVpssGrpAttr.u32MaxH                        = 480;//stSizeChn.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	memset(&astVpssChnAttr[VpssChn], 0, sizeof(VPSS_CHN_ATTR_S));
	astVpssChnAttr[VpssChn].u32Width                    = stSizeOut.u32Width;
	astVpssChnAttr[VpssChn].u32Height                   = stSizeOut.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = -1;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = -1;
	astVpssChnAttr[VpssChn].u32Depth                    = 1;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	VPSS_GRP VpssDstGrp[u8MaxChn];

	for (CVI_U8 i = 0; i < u8MaxChn; ++i) {
		VpssDstGrp[i] = CVI_VPSS_GetAvailableGrp();

		memset(&stVpssGrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));
		stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
		stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
		stVpssGrpAttr.enPixelFormat                  = enPixFormat;
		stVpssGrpAttr.u32MaxW                        = stSizeChn.u32Width;
		stVpssGrpAttr.u32MaxH                        = stSizeChn.u32Height;
		stVpssGrpAttr.u8VpssDev                      = 1;

		VpssChn = VPSS_CHN0;
		abChnEnable[VpssChn] = CVI_TRUE;
		memset(&astVpssChnAttr[VpssChn], 0, sizeof(VPSS_GRP_ATTR_S));
		astVpssChnAttr[VpssChn].u32Width                    = 1920;
		astVpssChnAttr[VpssChn].u32Height                   = 1080;
		astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
		astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
		astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = -1;
		astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = -1;
		astVpssChnAttr[VpssChn].u32Depth                    = 0;
		astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
		astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
		astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;
		astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;

		VpssChn = VPSS_CHN1;
		abChnEnable[VpssChn] = CVI_TRUE;
		memset(&astVpssChnAttr[VpssChn], 0, sizeof(VPSS_GRP_ATTR_S));
		astVpssChnAttr[VpssChn].u32Width                    = 640;
		astVpssChnAttr[VpssChn].u32Height                   = 480;
		astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
		astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
		astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = -1;
		astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = -1;
		astVpssChnAttr[VpssChn].u32Depth                    = 1;
		astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
		astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
		astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;
		astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;

		VpssChn = VPSS_CHN2;
		abChnEnable[VpssChn] = CVI_TRUE;
		memset(&astVpssChnAttr[VpssChn], 0, sizeof(VPSS_GRP_ATTR_S));
		astVpssChnAttr[VpssChn].u32Width                    = 320;
		astVpssChnAttr[VpssChn].u32Height                   = 240;
		astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
		astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
		astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = -1;
		astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = -1;
		astVpssChnAttr[VpssChn].u32Depth                    = 0;
		astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
		astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
		astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;
		astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;

		/*start vpss*/
		s32Ret = SAMPLE_COMM_VPSS_Init(VpssDstGrp[i], abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}

		s32Ret = SAMPLE_COMM_VPSS_Start(VpssDstGrp[i], abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}
	}
	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, stSizeOut.u32Height, stSizeOut.u32Width};
	SIZE_S stDefImageSize = {stSizeOut.u32Height, stSizeOut.u32Width};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVoConfig.VoDev	 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	// stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_440x1920_60;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = SAMPLE_PIXEL_FORMAT;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		return s32Ret;
	}

	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);

	CVI_S32 count = 0, crop_mode = 0;

	SAMPLE_PRT("how many frames to dump:\n");
	scanf("%d", &count);

	SAMPLE_PRT("used vpss or memcpy to crop frame:\n");
	SAMPLE_PRT("0:vpss 1:memcpy 2:ive\n");
	scanf("%d", &crop_mode);

	SAMPLE_VPSS_CONFIG_S stVpssCfg;

	memset(&stVpssCfg, 0, sizeof(SAMPLE_VPSS_CONFIG_S));
	for (CVI_U8 i = 0; i < u8MaxChn; ++i) {
		stVpssCfg.s32ChnNum++;
		stVpssCfg.astVpssParam[i].SrcGrp = VpssDstGrp[i];
		stVpssCfg.astVpssParam[i].SrcChn = VPSS_CHN1;
		stVpssCfg.astVpssParam[i].DstGrp = VpssGrp;
		stVpssCfg.astVpssParam[i].DstChn = VPSS_CHN0;
		stVpssCfg.astVpssParam[i].stDispRect.u32Width = stSizeOut.u32Width / u8MaxChn;
		stVpssCfg.astVpssParam[i].stDispRect.u32Height = stSizeOut.u32Height;
		stVpssCfg.astVpssParam[i].stDispRect.s32X = stVpssCfg.astVpssParam[i].stDispRect.u32Width * i;
		stVpssCfg.astVpssParam[i].stDispRect.s32Y = 0;
	}
	// stVpssCfg.stop_thread = CVI_TRUE;
	if (pthread_create(&stVpssCfg.vpss_thread, NULL, thread_send_vo, (void *)&stVpssCfg) != 0) {
		SAMPLE_PRT("pthread_create fail!\n");
		return CVI_FAILURE;
	}

	if (crop_mode == 0) {
		SAMPLE_PRT("used vpss to crop\n");
		pr2100_output_quick_by_vpss(count, u8MaxChn, stSizeChn, VpssDstGrp);
	} else if (crop_mode == 1) {
		SAMPLE_PRT("used memcpy to crop\n");
		pr2100_output_quick_by_mem(count, u8MaxChn, stSizeChn, VpssDstGrp);
	}  else if (crop_mode == 2) {
#ifdef IVE_SUPPORT
		SAMPLE_PRT("used ive to crop\n");
		pr2100_output_quick_by_ive(count, u8MaxChn, stSizeChn, VpssDstGrp);
#else
		SAMPLE_PRT("ive option is not open\n");
#endif
	}

	SAMPLE_PRT("Dump finished\n");

	if (stVpssCfg.vpss_thread > 0) {
		stVpssCfg.stop_thread = CVI_TRUE;
		pthread_join(stVpssCfg.vpss_thread, NULL);
	}

	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

CVI_S32 SAMPLE_IMX327_N5_VI_VPSS_ONLINE_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};
	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 2,
		.enSnsType[0] = SONY_IMX327_MIPI_2M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.s32BusId[0]  = 3,
		.MipiDev[0]   = 0xff,
		.enSnsType[1] = NEXTCHIP_N5_2M_25FPS_8BIT,
		.enWDRMode[1] = WDR_MODE_NONE,
		.s32BusId[1] = 0,
		.MipiDev[1]  = 0xff,
		.u8UseMultiSns = 1,
	};

	system("echo 1,1 > /sys/module/cv183x_vip/parameters/viproc_en");
	printf("IMX327 + N5\n");

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize, u32BlkRotSize;
	SIZE_S		stSizeSns0, stSizeSns1, stSizeOut;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = 0;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = SAMPLE_PIXEL_FORMAT;
		if (stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType == PIXELPLUS_PR2020_2M_25FPS_8BIT)
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = PIXEL_FORMAT_YUV_PLANAR_422;

		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns0);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[1], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stSizeOut.u32Width = 1280;
	stSizeOut.u32Height = 720;
	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 3;

	u32BlkSize = COMMON_GetPicBufferSize(stSizeSns0.u32Width, stSizeSns0.u32Height, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkRotSize = COMMON_GetPicBufferSize(stSizeSns0.u32Height, stSizeSns0.u32Width, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX(u32BlkSize, u32BlkRotSize);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 4;
	stVbConf.astCommPool[0].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(stSizeSns1.u32Width, stSizeSns1.u32Height, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkRotSize = COMMON_GetPicBufferSize(stSizeSns1.u32Height, stSizeSns1.u32Width, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX(u32BlkSize, u32BlkRotSize);
	stVbConf.astCommPool[1].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[1].u32BlkCnt	= 4;
	stVbConf.astCommPool[1].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[1] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(stSizeOut.u32Width, stSizeOut.u32Height, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkRotSize = COMMON_GetPicBufferSize(stSizeOut.u32Height, stSizeOut.u32Width, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX(u32BlkSize, u32BlkRotSize);
	stVbConf.astCommPool[2].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[2].u32BlkCnt	= 10;
	stVbConf.astCommPool[2].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[2] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	VI_VPSS_MODE_S stVIVPSSMode;
	VPSS_MODE_S stVPSSMode;

	stVIVPSSMode.aenMode[0] = VI_OFFLINE_VPSS_ONLINE;

	s32Ret = CVI_SYS_SetVIVPSSMode(&stVIVPSSMode);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_SetVIVPSSMode failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVPSSMode.enMode = VPSS_MODE_DUAL;
	stVPSSMode.aenInput[0] = VPSS_INPUT_MEM;
	stVPSSMode.ViPipe[0] = 0;
	stVPSSMode.aenInput[1] = VPSS_INPUT_ISP;
	stVPSSMode.ViPipe[1] = 0;

	s32Ret = CVI_SYS_SetVPSSModeEx(&stVPSSMode);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_SYS_SetVPSSModeEx failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step4:  Init VI
	 ************************************************/
	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = VPSS_ONLINE_GRP_0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	// snr0
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSizeSns0.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeSns0.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 1;

	astVpssChnAttr[VpssChn].u32Width                    = stSizeOut.u32Width;
	astVpssChnAttr[VpssChn].u32Height                   = stSizeOut.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	// snr1
	VpssGrp	  = VPSS_ONLINE_GRP_1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSizeSns1.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeSns1.u32Height;
	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	// offline
	VpssGrp	  = 2;
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSizeOut.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeOut.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = stSizeOut.u32Width;
	astVpssChnAttr[VpssChn].u32Height                   = stSizeOut.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Bind_VPSS(VPSS_ONLINE_GRP_0, 0, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VPSS failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, stSizeOut.u32Height, stSizeOut.u32Width};
	SIZE_S stDefImageSize = {stSizeOut.u32Height, stSizeOut.u32Width};
	VO_CHN VoChn = 0;
	CVI_U32 j = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
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
		return s32Ret;
	}

	VpssGrp = VPSS_ONLINE_GRP_0;
	VpssChn = 0;
	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);
	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	do {
		SAMPLE_PRT(GREEN "\nselect sensor: 0-imx327/1-N5, or 255 exit!\n" NONE);

		scanf("%d", &j);
		if (j == 255) {
			break;
		}
		SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
		VpssGrp = (j == 0) ? VPSS_ONLINE_GRP_0 : VPSS_ONLINE_GRP_1;
		SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	} while (1);

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);
	SAMPLE_COMM_VI_DestroyVi(&stViConfig);
	SAMPLE_COMM_VI_CLOSE();

	VpssGrp = VPSS_ONLINE_GRP_0;
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	VpssGrp = VPSS_ONLINE_GRP_1;
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	VpssGrp = 2;
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

CVI_S32 SAMPLE_IMX327_N5_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};
	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 2,
		.enSnsType[0] = SONY_IMX327_MIPI_2M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.s32BusId[0]  = 3,
		.MipiDev[0]   = 0xff,
		.enSnsType[1] = NEXTCHIP_N5_2M_25FPS_8BIT,
		.enWDRMode[1] = WDR_MODE_NONE,
		.s32BusId[1] = 0,
		.MipiDev[1]  = 0xff,
		.u8UseMultiSns = 1,
	};

	system("echo 1,1 > /sys/module/cv183x_vip/parameters/viproc_en");
	printf("IMX327 + N5\n");

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize, u32BlkRotSize;
	SIZE_S		stSizeSns0, stSizeSns1, stSizeOut;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = 0;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = SAMPLE_PIXEL_FORMAT;
		if (stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType == PIXELPLUS_PR2020_2M_25FPS_8BIT ||
			stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType == NEXTCHIP_N5_2M_25FPS_8BIT) {
			stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = PIXEL_FORMAT_YUV_PLANAR_422;
		}
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns0);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[1], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSizeSns1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stSizeOut.u32Width = 1280;
	stSizeOut.u32Height = 720;
	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 3;

	u32BlkSize = COMMON_GetPicBufferSize(stSizeSns0.u32Width, stSizeSns0.u32Height, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkRotSize = COMMON_GetPicBufferSize(stSizeSns0.u32Height, stSizeSns0.u32Width, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX(u32BlkSize, u32BlkRotSize);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 4;
	stVbConf.astCommPool[0].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(stSizeSns1.u32Width, stSizeSns1.u32Height, PIXEL_FORMAT_YUV_PLANAR_422,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkRotSize = COMMON_GetPicBufferSize(stSizeSns1.u32Height, stSizeSns1.u32Width, PIXEL_FORMAT_YUV_PLANAR_422,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX(u32BlkSize, u32BlkRotSize);
	stVbConf.astCommPool[1].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[1].u32BlkCnt	= 4;
	stVbConf.astCommPool[1].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[1] BlkSize %d\n", u32BlkSize);

	u32BlkSize = COMMON_GetPicBufferSize(stSizeOut.u32Width, stSizeOut.u32Height, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkRotSize = COMMON_GetPicBufferSize(stSizeOut.u32Height, stSizeOut.u32Width, SAMPLE_PIXEL_FORMAT,
		DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	u32BlkSize = MAX(u32BlkSize, u32BlkRotSize);
	stVbConf.astCommPool[2].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[2].u32BlkCnt	= 10;
	stVbConf.astCommPool[2].enRemapMode	= VB_REMAP_MODE_CACHED;
	SAMPLE_PRT("common pool[2] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step4:  Init VI
	 ************************************************/
	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSizeSns0.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeSns0.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = stSizeOut.u32Width;
	astVpssChnAttr[VpssChn].u32Height                   = stSizeOut.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	ViPipe = 0;
	ViChn = 0;
	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	VpssGrp	  = 1;
	stVpssGrpAttr.enPixelFormat                  = PIXEL_FORMAT_YUV_PLANAR_422;
	stVpssGrpAttr.u32MaxW                        = stSizeSns1.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSizeSns1.u32Height;
	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	ViPipe = 0;
	ViChn = 1;
	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}
	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, stSizeOut.u32Height, stSizeOut.u32Width};
	SIZE_S stDefImageSize = {stSizeOut.u32Height, stSizeOut.u32Width};
	VO_CHN VoChn = 0;
	CVI_U32 j = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
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
		return s32Ret;
	}

	VpssGrp = 0;
	VpssChn = 0;
	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);
	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	do {
		SAMPLE_PRT(GREEN "\nselect sensor: 0-imx327/1-N5, or 255 exit!\n" NONE);

		scanf("%d", &j);
		if (j == 255) {
			break;
		}
		VpssGrp = (j == 0) ? 0 : 1;
		SAMPLE_COMM_VPSS_UnBind_VO((VpssGrp ^ 1), VpssChn, stVoConfig.VoDev, VoChn);
		SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	} while (1);

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	ViPipe = 0;
	ViChn = 0;
	VpssGrp = 0;
	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	ViPipe = 0;
	ViChn = 1;
	VpssGrp = 1;
	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);
	SAMPLE_COMM_VI_DestroyVi(&stViConfig);
	SAMPLE_COMM_VI_CLOSE();
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}


CVI_S32 _n5_dump_sample(int idx)
{
	VIDEO_FRAME_INFO_S stVideoFrame;

	if (CVI_VI_GetChnFrame(0, idx, &stVideoFrame, 1000) == 0) {
		FILE *output;
		CVI_U32 image_size = stVideoFrame.stVFrame.u32Length[0]
				  + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];
		CVI_VOID *vir_addr;
		CVI_U32 plane_offset, u32LumaSize, u32ChromaSize;
		char img_name[128] = {0, };

		printf("width: %d, height: %d, total_buf_length: %d\n",
				stVideoFrame.stVFrame.u32Width,
				stVideoFrame.stVFrame.u32Height,
				image_size);

		snprintf(img_name, sizeof(img_name), "sample_%d.yuv", idx);

		output = fopen(img_name, "wb");
		if (output == NULL) {
			memset(img_name, 0x0, sizeof(img_name));
			snprintf(img_name, sizeof(img_name), "/mnt/data/sample_%d.yuv", idx);
			output = fopen(img_name, "wb");
			if (output == NULL) {
				CVI_VI_ReleaseChnFrame(0, idx, &stVideoFrame);
				printf("fopen fail\n");
				return CVI_FAILURE;
			}
		}

		u32LumaSize =  stVideoFrame.stVFrame.u32Stride[0] * stVideoFrame.stVFrame.u32Height;
		u32ChromaSize =  stVideoFrame.stVFrame.u32Stride[1] * stVideoFrame.stVFrame.u32Height;

		vir_addr = CVI_SYS_Mmap(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		CVI_SYS_IonInvalidateCache(stVideoFrame.stVFrame.u64PhyAddr[0], vir_addr, image_size);
		plane_offset = 0;
		for (int i = 0; i < 3; i++) {
			stVideoFrame.stVFrame.pu8VirAddr[i] = vir_addr + plane_offset;
			plane_offset += stVideoFrame.stVFrame.u32Length[i];
			printf("plane(%d): paddr(0x%" PRIu64 ") vaddr(%p) stride(%d) length(%d)\n",
				   i, stVideoFrame.stVFrame.u64PhyAddr[i],
				   (void *)stVideoFrame.stVFrame.pu8VirAddr[i],
				   stVideoFrame.stVFrame.u32Stride[i],
				   stVideoFrame.stVFrame.u32Length[i]);
			fwrite((void *)stVideoFrame.stVFrame.pu8VirAddr[i]
				, (i == 0) ? u32LumaSize : u32ChromaSize, 1, output);
		}
		CVI_SYS_Munmap(vir_addr, image_size);

		if (CVI_VI_ReleaseChnFrame(0, idx, &stVideoFrame) != 0)
			printf("CVI_VI_ReleaseChnFrame NG\n");

		fclose(output);
		return CVI_SUCCESS;
	}
	printf("CVI_VI_GetChnFrame NG\n");
	return CVI_FAILURE;
}

CVI_S32 SAMPLE_N5_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat	    = PIXEL_FORMAT_YUV_PLANAR_422;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize, i;
	SIZE_S		stSize;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = NEXTCHIP_N5_2M_25FPS_8BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.MipiDev[0]   = 0xff,
	};

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = 0;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= stIniCfg.devNum;

	for (i = 0; (i < stVbConf.u32MaxPoolCnt); i++) {
		u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YUV_PLANAR_422,
							DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
		stVbConf.astCommPool[i].u32BlkSize	= u32BlkSize;
		stVbConf.astCommPool[i].u32BlkCnt	= 12;
		SAMPLE_PRT("common pool[i] BlkSize %d\n", u32BlkSize);
	}

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	sleep(1);

	CVI_S32 loop = 0, end;
	CVI_U32 ok = 0, ng = 0;

	do {
		SAMPLE_PRT("how many frames to dump:\n");
		scanf("%d", &loop);
		while (loop > 0) {
			if (_n5_dump_sample(0) == CVI_SUCCESS)
				++ok;
			else
				++ng;
			if (_n5_dump_sample(1) == CVI_SUCCESS)
				++ok;
			else
				++ng;

			loop--;
		}

		SAMPLE_PRT("Dump finished:(0/1)\n");
		scanf("%d", &end);
	} while (end != 1);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}
#endif
CVI_S32 _mcs369q_dump_sample(void)
{
	VIDEO_FRAME_INFO_S stVideoFrame;

	if (CVI_VI_GetChnFrame(0, 0, &stVideoFrame, 1000) == 0) {
		FILE *output;
		CVI_U32 image_size = stVideoFrame.stVFrame.u32Length[0]
				  + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];
		CVI_VOID *vir_addr;
		CVI_U32 plane_offset, u32LumaSize, u32ChromaSize;
		char img_name[128] = {0, };

		printf("width: %d, height: %d, total_buf_length: %d\n",
				stVideoFrame.stVFrame.u32Width,
				stVideoFrame.stVFrame.u32Height,
				image_size);

		snprintf(img_name, sizeof(img_name), "sample_0.yuv");

		output = fopen(img_name, "wb");
		if (output == NULL) {
			memset(img_name, 0x0, sizeof(img_name));
			snprintf(img_name, sizeof(img_name), "/mnt/data/sample_0.yuv");
			output = fopen(img_name, "wb");
			if (output == NULL) {
				CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame);
				printf("fopen fail\n");
				return CVI_FAILURE;
			}
		}

		u32LumaSize =  stVideoFrame.stVFrame.u32Stride[0] * stVideoFrame.stVFrame.u32Height;
		u32ChromaSize =  stVideoFrame.stVFrame.u32Stride[1] * stVideoFrame.stVFrame.u32Height;

		vir_addr = CVI_SYS_Mmap(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		CVI_SYS_IonInvalidateCache(stVideoFrame.stVFrame.u64PhyAddr[0], vir_addr, image_size);
		plane_offset = 0;
		for (int i = 0; i < 3; i++) {
			stVideoFrame.stVFrame.pu8VirAddr[i] = vir_addr + plane_offset;
			plane_offset += stVideoFrame.stVFrame.u32Length[i];
			printf("plane(%d): paddr(0x%" PRIu64 ") vaddr(%p) stride(%d) length(%d)\n",
				   i, stVideoFrame.stVFrame.u64PhyAddr[i],
				   (void *)stVideoFrame.stVFrame.pu8VirAddr[i],
				   stVideoFrame.stVFrame.u32Stride[i],
				   stVideoFrame.stVFrame.u32Length[i]);
			fwrite((void *)stVideoFrame.stVFrame.pu8VirAddr[i]
				, (i == 0) ? u32LumaSize : u32ChromaSize, 1, output);
		}
		CVI_SYS_Munmap(vir_addr, image_size);

		if (CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame) != 0)
			printf("CVI_VI_ReleaseChnFrame NG\n");

		fclose(output);
		return CVI_SUCCESS;
	}

	printf("CVI_VI_GetChnFrame NG\n");
	return CVI_FAILURE;
}

CVI_S32 SAMPLE_MCS369Q_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat	    = PIXEL_FORMAT_YUV_PLANAR_422;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	// VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize;
	SIZE_S		stSize;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = VIVO_MCS369Q_4M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.MipiDev[0]   = 0xff,
	};

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = 0;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YUV_PLANAR_422,
						DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 12;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	sleep(1);

	CVI_S32 loop = 0, end;
	CVI_U32 ok = 0, ng = 0;

	do {
		SAMPLE_PRT("how many frames to dump:\n");
		scanf("%d", &loop);
		while (loop > 0) {
			if (_mcs369q_dump_sample() == CVI_SUCCESS)
				++ok;
			else
				++ng;
			loop--;
		}

		SAMPLE_PRT("Dump finished:(0/1)\n");
		scanf("%d", &end);
	} while (end != 1);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

#ifdef ARCH_CV183X
CVI_S32 _mcs369_dump_sample(void)
{
	VIDEO_FRAME_INFO_S stVideoFrame;

	if (CVI_VI_GetChnFrame(0, 0, &stVideoFrame, 1000) == 0) {
		FILE *output;
		CVI_U32 image_size = stVideoFrame.stVFrame.u32Length[0]
				  + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];
		CVI_VOID *vir_addr;
		CVI_U32 plane_offset, u32LumaSize, u32ChromaSize;
		char img_name[128] = {0, };

		printf("width: %d, height: %d, total_buf_length: %d\n",
				stVideoFrame.stVFrame.u32Width,
				stVideoFrame.stVFrame.u32Height,
				image_size);

		snprintf(img_name, sizeof(img_name), "sample_0.yuv");

		output = fopen(img_name, "wb");
		if (output == NULL) {
			memset(img_name, 0x0, sizeof(img_name));
			snprintf(img_name, sizeof(img_name), "/mnt/data/sample_0.yuv");
			output = fopen(img_name, "wb");
			if (output == NULL) {
				CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame);
				printf("fopen fail\n");
				return CVI_FAILURE;
			}
		}

		u32LumaSize =  stVideoFrame.stVFrame.u32Stride[0] * stVideoFrame.stVFrame.u32Height;
		u32ChromaSize =  stVideoFrame.stVFrame.u32Stride[1] * stVideoFrame.stVFrame.u32Height;

		vir_addr = CVI_SYS_Mmap(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		CVI_SYS_IonInvalidateCache(stVideoFrame.stVFrame.u64PhyAddr[0], vir_addr, image_size);
		plane_offset = 0;
		for (int i = 0; i < 3; i++) {
			stVideoFrame.stVFrame.pu8VirAddr[i] = vir_addr + plane_offset;
			plane_offset += stVideoFrame.stVFrame.u32Length[i];
			printf("plane(%d): paddr(0x%" PRIu64 ") vaddr(%p) stride(%d) length(%d)\n",
				   i, stVideoFrame.stVFrame.u64PhyAddr[i],
				   (void *)stVideoFrame.stVFrame.pu8VirAddr[i],
				   stVideoFrame.stVFrame.u32Stride[i],
				   stVideoFrame.stVFrame.u32Length[i]);
			fwrite((void *)stVideoFrame.stVFrame.pu8VirAddr[i]
				, (i == 0) ? u32LumaSize : u32ChromaSize, 1, output);
		}
		CVI_SYS_Munmap(vir_addr, image_size);

		if (CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame) != 0)
			printf("CVI_VI_ReleaseChnFrame NG\n");

		fclose(output);
		return CVI_SUCCESS;
	}

	printf("CVI_VI_GetChnFrame NG\n");
	return CVI_FAILURE;
}

CVI_S32 SAMPLE_MCS369_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat	    = PIXEL_FORMAT_YUV_PLANAR_422;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize;
	SIZE_S		stSize;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = VIVO_MCS369_2M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.MipiDev[0]   = 0xff,
	};

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = 0;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YUV_PLANAR_422,
						DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 12;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	sleep(1);

	CVI_S32 loop = 0, end;
	CVI_U32 ok = 0, ng = 0;

	do {
		SAMPLE_PRT("how many frames to dump:\n");
		scanf("%d", &loop);
		while (loop > 0) {
			if (_mcs369_dump_sample() == CVI_SUCCESS)
				++ok;
			else
				++ng;
			loop--;
		}

		SAMPLE_PRT("Dump finished:(0/1)\n");
		scanf("%d", &end);
	} while (end != 1);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}
#endif

CVI_S32 _mm308m2_dump_sample(void)
{
	VIDEO_FRAME_INFO_S stVideoFrame;

	if (CVI_VI_GetChnFrame(0, 0, &stVideoFrame, 1000) == 0) {
		FILE *output;
		CVI_U32 image_size = stVideoFrame.stVFrame.u32Length[0]
				  + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];
		CVI_VOID *vir_addr;
		CVI_U32 plane_offset, u32LumaSize, u32ChromaSize;
		char img_name[128] = {0, };

		printf("width: %d, height: %d, total_buf_length: %d\n",
				stVideoFrame.stVFrame.u32Width,
				stVideoFrame.stVFrame.u32Height,
				image_size);

		snprintf(img_name, sizeof(img_name), "sample_0.yuv");

		output = fopen(img_name, "wb");
		if (output == NULL) {
			memset(img_name, 0x0, sizeof(img_name));
			snprintf(img_name, sizeof(img_name), "/mnt/data/sample_0.yuv");
			output = fopen(img_name, "wb");
			if (output == NULL) {
				CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame);
				printf("fopen fail\n");
				return CVI_FAILURE;
			}
		}

		u32LumaSize =  stVideoFrame.stVFrame.u32Stride[0] * stVideoFrame.stVFrame.u32Height;
		u32ChromaSize =  stVideoFrame.stVFrame.u32Stride[1] * stVideoFrame.stVFrame.u32Height;

		vir_addr = CVI_SYS_Mmap(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		CVI_SYS_IonInvalidateCache(stVideoFrame.stVFrame.u64PhyAddr[0], vir_addr, image_size);
		plane_offset = 0;
		for (int i = 0; i < 3; i++) {
			stVideoFrame.stVFrame.pu8VirAddr[i] = vir_addr + plane_offset;
			plane_offset += stVideoFrame.stVFrame.u32Length[i];
			printf("plane(%d): paddr(0x%" PRIu64 ") vaddr(%p) stride(%d) length(%d)\n",
				   i, stVideoFrame.stVFrame.u64PhyAddr[i],
				   (void *)stVideoFrame.stVFrame.pu8VirAddr[i],
				   stVideoFrame.stVFrame.u32Stride[i],
				   stVideoFrame.stVFrame.u32Length[i]);
			fwrite((void *)stVideoFrame.stVFrame.pu8VirAddr[i]
				, (i == 0) ? u32LumaSize : u32ChromaSize, 1, output);
		}
		CVI_SYS_Munmap(vir_addr, image_size);

		if (CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame) != 0)
			printf("CVI_VI_ReleaseChnFrame NG\n");

		fclose(output);
		return CVI_SUCCESS;
	}

	printf("CVI_VI_GetChnFrame NG\n");
	return CVI_FAILURE;
}

CVI_S32 SAMPLE_MM308M2_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat	    = PIXEL_FORMAT_YUV_PLANAR_422;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	// VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize;
	SIZE_S		stSize;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = VIVO_MM308M2_2M_25FPS_8BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.MipiDev[0]   = 0xff,
	};

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = 0;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YUV_PLANAR_422,
						DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 12;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	sleep(1);

	CVI_S32 loop = 0, end;
	CVI_U32 ok = 0, ng = 0;

	do {
		SAMPLE_PRT("how many frames to dump:\n");
		scanf("%d", &loop);
		while (loop > 0) {
			if (_mm308m2_dump_sample() == CVI_SUCCESS)
				++ok;
			else
				++ng;
			loop--;
		}

		SAMPLE_PRT("Dump finished:(0/1)\n");
		scanf("%d", &end);
	} while (end != 1);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

CVI_S32 _pico640_dump_sample(void)
{
	VIDEO_FRAME_INFO_S stVideoFrame;

	if (CVI_VI_GetChnFrame(0, 0, &stVideoFrame, 1000) == 0) {
		FILE *output;
		CVI_U32 image_size = stVideoFrame.stVFrame.u32Length[0]
				  + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];
		CVI_VOID *vir_addr;
		CVI_U32 plane_offset, u32LumaSize, u32ChromaSize;
		char img_name[128] = {0, };

		printf("width: %d, height: %d, total_buf_length: %d\n",
				stVideoFrame.stVFrame.u32Width,
				stVideoFrame.stVFrame.u32Height,
				image_size);

		snprintf(img_name, sizeof(img_name), "sample_0.yuv");

		output = fopen(img_name, "wb");
		if (output == NULL) {
			memset(img_name, 0x0, sizeof(img_name));
			snprintf(img_name, sizeof(img_name), "/mnt/data/sample_0.yuv");
			output = fopen(img_name, "wb");
			if (output == NULL) {
				CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame);
				printf("fopen fail\n");
				return CVI_FAILURE;
			}
		}

		u32LumaSize =  stVideoFrame.stVFrame.u32Stride[0] * stVideoFrame.stVFrame.u32Height;
		u32ChromaSize =  stVideoFrame.stVFrame.u32Stride[1] * stVideoFrame.stVFrame.u32Height;

		vir_addr = CVI_SYS_Mmap(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		CVI_SYS_IonInvalidateCache(stVideoFrame.stVFrame.u64PhyAddr[0], vir_addr, image_size);
		plane_offset = 0;
		for (int i = 0; i < 3; i++) {
			stVideoFrame.stVFrame.pu8VirAddr[i] = vir_addr + plane_offset;
			plane_offset += stVideoFrame.stVFrame.u32Length[i];
			printf("plane(%d): paddr(0x%" PRIu64 ") vaddr(%p) stride(%d) length(%d)\n",
				   i, stVideoFrame.stVFrame.u64PhyAddr[i],
				   (void *)stVideoFrame.stVFrame.pu8VirAddr[i],
				   stVideoFrame.stVFrame.u32Stride[i],
				   stVideoFrame.stVFrame.u32Length[i]);
			fwrite((void *)stVideoFrame.stVFrame.pu8VirAddr[i]
				, (i == 0) ? u32LumaSize : u32ChromaSize, 1, output);
		}
		CVI_SYS_Munmap(vir_addr, image_size);

		if (CVI_VI_ReleaseChnFrame(0, 0, &stVideoFrame) != 0)
			printf("CVI_VI_ReleaseChnFrame NG\n");

		fclose(output);
		return CVI_SUCCESS;
	}

	printf("CVI_VI_GetChnFrame NG\n");
	return CVI_FAILURE;
}

CVI_S32 SAMPLE_PICO640_TEST(void)
{
	SAMPLE_INI_CFG_S	   stIniCfg = {0};

	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat	    = PIXEL_FORMAT_YUV_PLANAR_422;
	VIDEO_FORMAT_E	   enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	// VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;
	SAMPLE_VI_CONFIG_S stViConfig;
	CVI_S32            s32WorkSnsId = 0;

	VB_CONFIG_S	stVbConf;
	PIC_SIZE_E	enPicSize;
	CVI_U32		u32BlkSize;
	SIZE_S		stSize;
	CVI_S32		s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = PICO640_THERMAL_479P,
		.enWDRMode[0] = WDR_MODE_NONE,
		.MipiDev[0]   = 0xff,
	};

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	for (; s32WorkSnsId < stIniCfg.devNum; s32WorkSnsId++) {
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = 0;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
		stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;
	}

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YUV_PLANAR_422,
						DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 12;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	sleep(1);

	CVI_S32 loop = 0, end;
	CVI_U32 ok = 0, ng = 0;

	do {
		SAMPLE_PRT("how many frames to dump:\n");
		scanf("%d", &loop);
		while (loop > 0) {
			if (_pico640_dump_sample() == CVI_SUCCESS)
				++ok;
			else
				++ng;
			loop--;
		}

		SAMPLE_PRT("Dump finished:(0/1)\n");
		scanf("%d", &end);
	} while (end != 1);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

/*
 * calculate checksum then compare to golden_checksum
 */
CVI_S32 compare_checksum(void)
{
	uint64_t chk_sum = 0;
	int ch2 = 0;
	FILE *fp2;
	CVI_S32 ret = 0;

#define DUMP_BIN	"/mnt/data/dump.bin"
#define VIP_GOLDEN_SUM	371931937

	fp2 = fopen(DUMP_BIN, "rb");
	if (fp2 == NULL) {
		SAMPLE_PRT("Error in opening file %s\n", DUMP_BIN);
		return -1;
	}

	while ((ch2 = fgetc(fp2)) != EOF)
		chk_sum += ch2;

	printf("chksum=%"PRIu64"\n", chk_sum);

	if (chk_sum == VIP_GOLDEN_SUM) {
		printf(GREEN "VIP SLT-TEST PASS\n" NONE);
		ret = 0;
	} else {
		printf(RED "VIP SLT-TEST FAIL\n" NONE);
		ret = -1;
	}

	return ret;
}

CVI_S32 SAMPLE_VIO_TWO_DEV_VO(void)
{
	MMF_VERSION_S stVersion;
	SAMPLE_INI_CFG_S	   stIniCfg = {0};
	SAMPLE_VI_CONFIG_S stViConfig;

	PIC_SIZE_E enPicSize;
	CVI_U32 chnID = 0;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 2,
		.enSnsType[0] = SONY_IMX327_2L_MIPI_2M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.s32BusId[0]  = 3,
		.MipiDev[0]   = 0xff,
		.enSnsType[1] = SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT,
		.s32BusId[1] = 0,
		.MipiDev[1] = 0xff,
	};

	CVI_SYS_GetVersion(&stVersion);
	SAMPLE_PRT("MMF Version:%s\n", stVersion.version);

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	// Get config from ini if found.
	if (SAMPLE_COMM_VI_ParseIni(&stIniCfg)) {
		SAMPLE_PRT("Parse complete\n");
	}

	//Set sensor number
	CVI_VI_SetDevNum(stIniCfg.devNum);


	/************************************************
	 * step1:  Config VI
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Init modules
	 ************************************************/
	s32Ret = SAMPLE_PLAT_SYS_INIT(stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sys init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	SIZE_S stSizeIn, stSizeOut;

	stSizeIn.u32Width   = stSize.u32Width;
	stSizeIn.u32Height  = stSize.u32Height;
	stSizeOut.u32Width  = 1280;
	stSizeOut.u32Height = 720;

	s32Ret = SAMPLE_PLAT_VPSS_INIT(0, stSizeIn, stSizeOut);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vpss init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VPSS_INIT(1, stSizeIn, stSizeOut);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vpss init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(0, 0, 0);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(0, 1, 1);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VO_INIT();
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vo init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	CVI_VO_SetChnRotation(0, 0, ROTATION_90);

	CVI_S32 vo_bind_vpssgrp = 0;

	SAMPLE_COMM_VPSS_Bind_VO(0, 0, 0, 0);

	sleep(1);

	do {
		printf("Show sensorID: ");
		scanf("%d", &chnID);

		if (chnID == 0) {
			if (vo_bind_vpssgrp == 0)
				continue;
			else if (vo_bind_vpssgrp == 1) {
				SAMPLE_COMM_VPSS_UnBind_VO(1, 0, 0, 0);
				SAMPLE_COMM_VPSS_Bind_VO(0, 0, 0, 0);
				vo_bind_vpssgrp = 0;
			}
		} else if (chnID == 1) {
			if (vo_bind_vpssgrp == 1)
				continue;
			else if (vo_bind_vpssgrp == 0) {
				SAMPLE_COMM_VPSS_UnBind_VO(0, 0, 0, 0);
				SAMPLE_COMM_VPSS_Bind_VO(1, 0, 0, 0);
				vo_bind_vpssgrp = 1;
			}
		}
	} while (chnID != 255);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

CVI_S32 SAMPLE_VI_VPSS_SLT(CVI_S32 frmNum, char *filename)
{
	SAMPLE_SNS_TYPE_E  enSnsType	    = SONY_IMX307_MIPI_2M_30FPS_12BIT;
	WDR_MODE_E	   enWDRMode	    = WDR_MODE_NONE;
	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E     enPixFormat	    = SAMPLE_PIXEL_FORMAT;
	VIDEO_FORMAT_E     enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;

	VB_CONFIG_S        stVbConf;
	PIC_SIZE_E         enPicSize;
	CVI_U32	       u32BlkSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

	VI_DEV ViDev = 0;
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;
	CVI_S32 s32WorkSnsId = 0;
	SAMPLE_VI_CONFIG_S stViConfig;
	VI_PIPE_ATTR_S     stPipeAttr;

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType	     = enSnsType;
	stViConfig.s32WorkingViNum				     = 1;
	stViConfig.as32WorkingViId[0]				     = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev	     = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId	     = 2;

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[0]   = 2;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[1]   = 1;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[2]   = 3;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[3]   = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[4]   = 4;

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[0]    = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[1]    = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[2]    = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[3]    = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[4]    = 0;

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.stMclkAttr.bMclkEn = 1;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.stMclkAttr.u8Mclk = 1;

	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev	     = ViDev;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode	     = enWDRMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	     = ViPipe;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = ViChn;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
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
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT,
					 DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 6;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step4:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartDev(&stViConfig.astViInfo[ViDev]);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartMIPI(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start MIPI failed with %#x\n", s32Ret);
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
	s32Ret = CVI_VI_CreatePipe(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_CreatePipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_StartPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_GetPipeAttr(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_StartIsp(&stViConfig.astViInfo[ViDev]);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_StartIsp failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	int ret = 0;

	SAMPLE_PRT("i2cset -f -y 2 0x1a 0x30 0x8c 0x21 i\n");
	ret = system("i2cset -f -y 2 0x1a 0x30 0x8c 0x21 i");
	if (ret == -1 || ret == 127) {
		SAMPLE_PRT("system call i2cset fail\n");
		return CVI_FAILURE;
	}

	SAMPLE_COMM_VI_StartViChn(&stViConfig);

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSize.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = 1280;
	astVpssChnAttr[VpssChn].u32Height                   = 720;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = PIXEL_FORMAT_RGB_888_PLANAR;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 1;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	do {
		VIDEO_FRAME_INFO_S stVideoFrame;
		FILE *fp;
		CVI_VOID *vir_addr;
		CVI_U32 plane_offset, u32LumaSize, u32ChromaSize;
		size_t image_size;

		s32Ret = CVI_VPSS_GetChnFrame(0, 0, &stVideoFrame, 10000);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_VPSS_GetChnFrame failed with %#x\n", s32Ret);
			return CVI_FAILURE;
		}

		image_size = stVideoFrame.stVFrame.u32Length[0] + stVideoFrame.stVFrame.u32Length[1]
				  + stVideoFrame.stVFrame.u32Length[2];

		u32LumaSize =  stVideoFrame.stVFrame.u32Stride[0] * stVideoFrame.stVFrame.u32Height;
		if (stVideoFrame.stVFrame.enPixelFormat == PIXEL_FORMAT_RGB_888_PLANAR)
			u32ChromaSize = u32LumaSize;
		else if (stVideoFrame.stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_422)
			u32ChromaSize =  stVideoFrame.stVFrame.u32Stride[1] * stVideoFrame.stVFrame.u32Height;
		else if (stVideoFrame.stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420)
			u32ChromaSize =  stVideoFrame.stVFrame.u32Stride[1] * stVideoFrame.stVFrame.u32Height / 2;
		else if (stVideoFrame.stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_400)
			u32ChromaSize =  0;
		else
			u32ChromaSize =  0;

		fp = fopen(filename, "wb");
		if (fp == CVI_NULL) {
			CVI_VPSS_ReleaseChnFrame(0, 0, &stVideoFrame);
			CVI_TRACE_LOG(CVI_DBG_ERR, "fopen fail\n");
			return CVI_FAILURE;
		}

		vir_addr = CVI_SYS_Mmap(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		plane_offset = 0;
		for (int i = 0; i < 3; i++) {
			if (stVideoFrame.stVFrame.u32Length[i] == 0)
				continue;

			stVideoFrame.stVFrame.pu8VirAddr[i] = vir_addr + plane_offset;
			plane_offset += stVideoFrame.stVFrame.u32Length[i];
			SAMPLE_PRT("plane(%d): paddr(%#"PRIx64") vaddr(%p) stride(%d)\n",
				   i, stVideoFrame.stVFrame.u64PhyAddr[i],
				   stVideoFrame.stVFrame.pu8VirAddr[i],
				   stVideoFrame.stVFrame.u32Stride[i]);
			fwrite((void *)(uintptr_t)stVideoFrame.stVFrame.pu8VirAddr[i]
				, (i == 0) ? u32LumaSize : u32ChromaSize, 1, fp);
		}
		CVI_SYS_Munmap(vir_addr, image_size);

		fclose(fp);

		// s32Ret = compare_checksum(fn_dumpbin);

		CVI_VPSS_ReleaseChnFrame(0, 0, &stVideoFrame);

		if (frmNum != 11111)
			frmNum--;
	} while ((s32Ret != -1) && (frmNum > 0));

	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}


CVI_S32 SAMPLE_VIO_VoRotation(void)
{
	SAMPLE_SNS_TYPE_E  enSnsType	    = SENSOR0_TYPE;
	WDR_MODE_E	   enWDRMode	    = WDR_MODE_NONE;
	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E     enPixFormat	    = SAMPLE_PIXEL_FORMAT;
	VIDEO_FORMAT_E     enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;

	VB_CONFIG_S        stVbConf;
	PIC_SIZE_E         enPicSize;
	CVI_U32	       u32BlkSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

	VI_DEV ViDev = 0;
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;
	CVI_S32 s32WorkSnsId = 0;
	SAMPLE_VI_CONFIG_S stViConfig;
	VI_PIPE_ATTR_S     stPipeAttr;

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType	     = enSnsType;
	stViConfig.s32WorkingViNum				     = 1;
	stViConfig.as32WorkingViId[0]				     = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev	     = 0xFF;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId	     = 3;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev	     = ViDev;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode	     = enWDRMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	     = ViPipe;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = ViChn;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
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
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8
					    , enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 8;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step4:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartDev(&stViConfig.astViInfo[ViDev]);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartMIPI(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start MIPI failed with %#x\n", s32Ret);
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
	s32Ret = CVI_VI_CreatePipe(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_CreatePipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_StartPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_GetPipeAttr(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_CreateIsp(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_CreateIsp failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_COMM_VI_StartViChn(&stViConfig);

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSize.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = 1280;
	astVpssChnAttr[VpssChn].u32Height                   = 720;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, 720, 1280};
	SIZE_S stDefImageSize = {720, 1280};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
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
		return s32Ret;
	}

	CVI_VO_SetChnRotation(stVoConfig.VoDev, VoChn, ROTATION_90);
	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	PAUSE();

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

CVI_S32 SAMPLE_VIO_ViVpssAspectRatio(void)
{
	SAMPLE_SNS_TYPE_E  enSnsType	    = SENSOR0_TYPE;
	WDR_MODE_E	   enWDRMode	    = WDR_MODE_NONE;
	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E     enPixFormat	    = SAMPLE_PIXEL_FORMAT;
	VIDEO_FORMAT_E     enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;

	VB_CONFIG_S        stVbConf;
	PIC_SIZE_E         enPicSize;
	CVI_U32	       u32BlkSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

	VI_DEV ViDev = 0;
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;
	CVI_S32 s32WorkSnsId = 0;
	SAMPLE_VI_CONFIG_S stViConfig;
	VI_PIPE_ATTR_S     stPipeAttr;

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType	     = enSnsType;
	stViConfig.s32WorkingViNum				     = 1;
	stViConfig.as32WorkingViId[0]				     = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev	     = 0xFF;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId	     = 3;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev	     = ViDev;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode	     = enWDRMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	     = ViPipe;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = ViChn;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
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
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8
					    , enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 8;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step4:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartDev(&stViConfig.astViInfo[ViDev]);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartMIPI(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start MIPI failed with %#x\n", s32Ret);
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
	s32Ret = CVI_VI_CreatePipe(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_CreatePipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_StartPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_GetPipeAttr(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_CreateIsp(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_CreateIsp failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_COMM_VI_StartViChn(&stViConfig);

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSize.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = 720;
	astVpssChnAttr[VpssChn].u32Height                   = 1280;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = PIXEL_FORMAT_RGB_888_PLANAR;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_AUTO;
	astVpssChnAttr[VpssChn].stAspectRatio.bEnableBgColor = CVI_TRUE;
	astVpssChnAttr[VpssChn].stAspectRatio.u32BgColor    = COLOR_RGB_BLACK;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step5:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, 720, 1280};
	SIZE_S stDefImageSize = {720, 1280};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVoConfig.VoDev	 = 0;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_MIPI;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_720x1280_60;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = PIXEL_FORMAT_RGB_888_PLANAR;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %#x\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	PAUSE();

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

CVI_S32 SAMPLE_VIO_ViRotation(void)
{
	SAMPLE_SNS_TYPE_E  enSnsType	    = SENSOR0_TYPE;
	WDR_MODE_E	   enWDRMode	    = WDR_MODE_NONE;
	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E     enPixFormat	    = SAMPLE_PIXEL_FORMAT;
	VIDEO_FORMAT_E     enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;

	VB_CONFIG_S        stVbConf;
	PIC_SIZE_E         enPicSize;
	CVI_U32            u32BlkSize, u32BlkRotSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

	VI_DEV ViDev = 0;
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;
	CVI_S32 s32WorkSnsId = 0;
	SAMPLE_VI_CONFIG_S stViConfig;
	VI_PIPE_ATTR_S     stPipeAttr;

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType	     = enSnsType;
	stViConfig.s32WorkingViNum				     = 1;
	stViConfig.as32WorkingViId[0]				     = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev	     = 0xFF;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId	     = 3;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev	     = ViDev;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode	     = enWDRMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	     = ViPipe;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = ViChn;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
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
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8
					    , enCompressMode, DEFAULT_ALIGN);
	u32BlkRotSize = COMMON_GetPicBufferSize(stSize.u32Height, stSize.u32Width, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8
					       , enCompressMode, DEFAULT_ALIGN);
	u32BlkSize = MAX(u32BlkSize, u32BlkRotSize);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 12;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step4:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartDev(&stViConfig.astViInfo[ViDev]);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartMIPI(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start MIPI failed with %#x\n", s32Ret);
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
	s32Ret = CVI_VI_CreatePipe(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_CreatePipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_StartPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_GetPipeAttr(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_CreateIsp(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_CreateIsp failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_COMM_VI_StartViChn(&stViConfig);

	s32Ret = CVI_VI_SetChnRotation(ViPipe, ViChn, ROTATION_90);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_SetChnRotation failed with %d\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSize.u32Height;
	stVpssGrpAttr.u32MaxH                        = stSize.u32Width;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = 720;
	astVpssChnAttr[VpssChn].u32Height                   = 1280;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stAspectRatio.u32BgColor    = COLOR_RGB_BLACK;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step5:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, 720, 1280};
	SIZE_S stDefImageSize = {720, 1280};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
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
		return s32Ret;
	}

	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	PAUSE();

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

CVI_S32 SAMPLE_VIO_VpssRotation(void)
{
	SAMPLE_SNS_TYPE_E  enSnsType	    = SENSOR0_TYPE;
	WDR_MODE_E	   enWDRMode	    = WDR_MODE_NONE;
	DYNAMIC_RANGE_E    enDynamicRange   = DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E     enPixFormat	    = SAMPLE_PIXEL_FORMAT;
	VIDEO_FORMAT_E     enVideoFormat    = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode   = COMPRESS_MODE_NONE;
	VI_VPSS_MODE_E	   enMastPipeMode   = VI_OFFLINE_VPSS_OFFLINE;

	VB_CONFIG_S        stVbConf;
	PIC_SIZE_E         enPicSize;
	CVI_U32	       u32BlkSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

	VI_DEV ViDev = 0;
	VI_PIPE ViPipe = 0;
	VI_CHN ViChn = 0;
	CVI_S32 s32WorkSnsId = 0;
	SAMPLE_VI_CONFIG_S stViConfig;
	VI_PIPE_ATTR_S     stPipeAttr;

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType	     = enSnsType;
	stViConfig.s32WorkingViNum				     = 1;
	stViConfig.as32WorkingViId[0]				     = 0;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev	     = 0xFF;
	stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId	     = 3;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev	     = ViDev;
	stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode	     = enWDRMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	     = ViPipe;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]	     = -1;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn	     = ViChn;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
	stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
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
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT,
					 DATA_BITWIDTH_8, enCompressMode, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 8;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step4:  Init VI ISP
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_StartSensor(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start sensor failed with %#x\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartDev(&stViConfig.astViInfo[ViDev]);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_StartDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}
	s32Ret = SAMPLE_COMM_VI_StartMIPI(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "system start MIPI failed with %#x\n", s32Ret);
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
	s32Ret = CVI_VI_CreatePipe(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_CreatePipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_StartPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_GetPipeAttr(ViPipe, &stPipeAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_StartPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VI_CreateIsp(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI_CreateIsp failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_COMM_VI_StartViChn(&stViConfig);

	/************************************************
	 * step5:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = SAMPLE_PIXEL_FORMAT;
	stVpssGrpAttr.u32MaxW                        = stSize.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = 1280;
	astVpssChnAttr[VpssChn].u32Height                   = 720;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	CVI_VPSS_SetChnRotation(VpssGrp, VpssChn, ROTATION_90);

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step6:  Init VO
	 ************************************************/
	SAMPLE_VO_CONFIG_S stVoConfig;
	RECT_S stDefDispRect  = {0, 0, 720, 1280};
	SIZE_S stDefImageSize = {720, 1280};
	VO_CHN VoChn = 0;

	s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VO_GetDefConfig failed with %#x\n", s32Ret);
		return s32Ret;
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
		return s32Ret;
	}

	SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	PAUSE();

	SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);

	SAMPLE_COMM_VO_StopVO(&stVoConfig);

	SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);

	SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);

	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

CVI_S32 SAMPLE_VIO_VpssFileIO(SIZE_S stSize)
{
	VB_CONFIG_S        stVbConf;
	CVI_U32	           u32BlkSize;
	CVI_S32            s32Ret = CVI_SUCCESS;

#define VPSS_FILENAME_IN  "input"
#define VPSS_FILENAME_OUT "output"

	/************************************************
	 * step1:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt		= 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_BGR_888,
					 DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 3;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step2:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp	  = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN           VpssChn        = VPSS_CHN0;
	CVI_BOOL           abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat                  = PIXEL_FORMAT_BGR_888;
	stVpssGrpAttr.u32MaxW                        = stSize.u32Width;
	stVpssGrpAttr.u32MaxH                        = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev                      = 0;

	astVpssChnAttr[VpssChn].u32Width                    = 1280;
	astVpssChnAttr[VpssChn].u32Height                   = 720;
	astVpssChnAttr[VpssChn].enVideoFormat               = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat               = SAMPLE_PIXEL_FORMAT;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth                    = 0;
	astVpssChnAttr[VpssChn].bMirror                     = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip                       = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}


	/************************************************
	 * step3:  VPSS work
	 ************************************************/
	VIDEO_FRAME_INFO_S stVideoFrame;

	SAMPLE_COMM_VPSS_SendFrame(VpssGrp, &stSize, PIXEL_FORMAT_BGR_888, VPSS_FILENAME_IN);
	s32Ret = CVI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stVideoFrame, 50);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_GetChnFrame for grp%d chn%d. s32Ret: 0x%x !\n"
			, VpssGrp, VpssChn, s32Ret);
	} else {
		SAMPLE_COMM_FRAME_SaveToFile(VPSS_FILENAME_OUT, &stVideoFrame);
		CVI_VPSS_ReleaseChnFrame(VpssGrp, VpssGrp, &stVideoFrame);
	}

	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

CVI_S32 SAMPLE_VIO_VpssCombine2File(SIZE_S stSize)
{
	VB_CONFIG_S	   stVbConf;
	CVI_U32		   u32BlkSize;
	CVI_S32		   s32Ret = CVI_SUCCESS;

#define VPSS_FILENAME_IN  "input"
#define VPSS_FILENAME_OUT "output"

	/************************************************
	 * step1:  Init SYS and common VB
	 ************************************************/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width * 2, stSize.u32Height, PIXEL_FORMAT_YUV_PLANAR_420,
					 DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 5;
	SAMPLE_PRT("common pool[0] BlkSize %d\n", u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return -1;
	}

	/************************************************
	 * step2:  Init VPSS
	 ************************************************/
	VPSS_GRP	   VpssGrp = 0;
	VPSS_GRP_ATTR_S    stVpssGrpAttr;
	VPSS_CHN	   VpssChn = VPSS_CHN0;
	CVI_BOOL	   abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	// grp0 for right half
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat		     = PIXEL_FORMAT_YUV_PLANAR_420;
	stVpssGrpAttr.u32MaxW			     = stSize.u32Width;
	stVpssGrpAttr.u32MaxH			     = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev			     = 0;

	astVpssChnAttr[VpssChn].u32Width		    = stSize.u32Width * 2;
	astVpssChnAttr[VpssChn].u32Height		    = stSize.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat		    = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat		    = PIXEL_FORMAT_YUV_PLANAR_420;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth		    = 1;
	astVpssChnAttr[VpssChn].bMirror			    = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip			    = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode	    = ASPECT_RATIO_MANUAL;
	astVpssChnAttr[VpssChn].stAspectRatio.bEnableBgColor   = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.s32X = 0;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.s32Y = 0;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.u32Width = stSize.u32Width;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.u32Height = stSize.u32Height;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	// grp1 for right half
	VpssGrp   = 1;
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
	stVpssGrpAttr.enPixelFormat		     = PIXEL_FORMAT_YUV_PLANAR_420;
	stVpssGrpAttr.u32MaxW			     = stSize.u32Width;
	stVpssGrpAttr.u32MaxH			     = stSize.u32Height;
	stVpssGrpAttr.u8VpssDev			     = 0;

	astVpssChnAttr[VpssChn].u32Width		    = stSize.u32Width * 2;
	astVpssChnAttr[VpssChn].u32Height		    = stSize.u32Height;
	astVpssChnAttr[VpssChn].enVideoFormat		    = VIDEO_FORMAT_LINEAR;
	astVpssChnAttr[VpssChn].enPixelFormat		    = PIXEL_FORMAT_YUV_PLANAR_420;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = 30;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = 30;
	astVpssChnAttr[VpssChn].u32Depth		    = 1;
	astVpssChnAttr[VpssChn].bMirror			    = CVI_FALSE;
	astVpssChnAttr[VpssChn].bFlip			    = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode	    = ASPECT_RATIO_MANUAL;
	astVpssChnAttr[VpssChn].stAspectRatio.bEnableBgColor   = CVI_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.s32X = stSize.u32Width;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.s32Y = 0;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.u32Width = stSize.u32Width;
	astVpssChnAttr[VpssChn].stAspectRatio.stVideoRect.u32Height = stSize.u32Height;
	astVpssChnAttr[VpssChn].stNormalize.bEnable         = CVI_FALSE;

	/*start vpss*/
	abChnEnable[0] = CVI_TRUE;
	s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  VPSS work
	 ************************************************/
	VIDEO_FRAME_INFO_S stVideoFrame;

	SAMPLE_COMM_VPSS_SendFrame(0, &stSize, PIXEL_FORMAT_YUV_PLANAR_420, VPSS_FILENAME_IN);
	s32Ret = CVI_VPSS_GetChnFrame(0, 0, &stVideoFrame, 100);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_GetChnFrame for grp0 chn0. s32Ret: 0x%x !\n", s32Ret);
		goto ERR_VPSS_COMBINE;
	}

	CVI_VPSS_SendChnFrame(1, 0, &stVideoFrame, -1);
	CVI_VPSS_ReleaseChnFrame(0, 0, &stVideoFrame);
	SAMPLE_COMM_VPSS_SendFrame(1, &stSize, PIXEL_FORMAT_YUV_PLANAR_420, VPSS_FILENAME_IN);

	s32Ret = CVI_VPSS_GetChnFrame(1, 0, &stVideoFrame, 100);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VPSS_GetChnFrame for grp1 chn0. s32Ret: 0x%x !\n", s32Ret);
		goto ERR_VPSS_COMBINE;
	}

	SAMPLE_COMM_FRAME_SaveToFile(VPSS_FILENAME_OUT, &stVideoFrame);
	CVI_VPSS_ReleaseChnFrame(1, 0, &stVideoFrame);

ERR_VPSS_COMBINE:
	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

CVI_S32 SAMPLE_VO_LVDS_TEST(CVI_VOID)
{
	VB_CONFIG_S	   stVbConf;
	CVI_U32		   u32BlkSize;
	CVI_S32		   s32Ret = CVI_SUCCESS;
	SAMPLE_VO_CONFIG_S stVoConfig;
	VO_CHN_ATTR_S stChnAttr;
	RECT_S stDefDispRect  = {0, 0, 1024, 600};
	SIZE_S stDefImageSize = {1024, 600};
	SIZE_S stSize = stDefImageSize;
	VO_DEV VoDev = 0;
	VO_LAYER VoLayer = VoDev;
	VO_CHN VoChn = 0;

	const VO_LVDS_ATTR_S lvds_ot07007_cfg = {
		.mode = VO_LVDS_MODE_VESA,
		.out_bits = VO_LVDS_OUT_6BIT,
		.chn_num = 1,
		.data_big_endian = 0,
		.lane_id = {VO_LVDS_LANE_0, VO_LVDS_LANE_1, VO_LVDS_LANE_2, VO_LVDS_LANE_CLK, -1},
		.lane_pn_swap = {false, false, false, false, false},
		.stSyncInfo = {
			.u16Hpw = 40,
			.u16Hbb = 180,
			.u16Hfb = 100,
			.u16Hact = 1024,
			.u16Vpw = 4,
			.u16Vbb = 20,
			.u16Vfb = 11,
			.u16Vact = 600,
			.bIvs = 0,
			.bIhs = 0,
			.bIop = 1,
			.u16FrameRate = 60
		},
		.pixelclock = 51206,
	};

	system("devmem 0x03001100 32 0x3"); // set pinmux to GPIOB5
	// set GPIOB5 to high
	SAMPLE_COMM_GPIO_SetValue(453, 1);
	// set GPIOE0 to low and GPIOE1 to high
	system("devmem 0x05021000 32 0x2");

	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height,
						PIXEL_FORMAT_NV21,
						DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u32BlkSize	= u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt	= 3;
	SAMPLE_PRT("common pool[%d] BlkSize %d\n", 0, u32BlkSize);

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("system init failed with %#x\n", s32Ret);
		return s32Ret;
	}

	stVoConfig.VoDev	 = VoDev;
	stVoConfig.stVoPubAttr.enIntfType  = VO_INTF_LCD_18BIT;
	stVoConfig.stVoPubAttr.enIntfSync  = VO_OUTPUT_USER; //
	stVoConfig.stVoPubAttr.stSyncInfo = lvds_ot07007_cfg.stSyncInfo;
	stVoConfig.stVoPubAttr.u32BgColor = COLOR_10_RGB_BLUE;
	stVoConfig.stDispRect	 = stDefDispRect;
	stVoConfig.stImageSize	 = stDefImageSize;
	stVoConfig.enPixFormat	 = PIXEL_FORMAT_NV21;
	stVoConfig.u32DisBufLen  = 3;
	stVoConfig.enVoMode	 = VO_MODE_1MUX;

	/********************************
	 * Set and start VO device VoDev#.
	 ********************************/

	stVoConfig.stVoPubAttr.stLvdsAttr = lvds_ot07007_cfg;

	s32Ret = CVI_VO_SetPubAttr(VoDev, &stVoConfig.stVoPubAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		goto ERR_VO_SETPUBATTR;
	}

	s32Ret = CVI_VO_Enable(VoDev);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		goto ERR_VO_ENABLE;
	}

	/******************************
	 * Set and start layer VoDev#.
	 ********************************/
	VO_VIDEO_LAYER_ATTR_S stLayerAttr = { 0 };

	stLayerAttr.stDispRect.s32X = 0;
	stLayerAttr.stDispRect.s32Y = 0;
	stLayerAttr.stDispRect = stDefDispRect;
	stLayerAttr.enPixFormat = stVoConfig.enPixFormat;

	stLayerAttr.stImageSize = stDefImageSize;

	if (stVoConfig.u32DisBufLen) {
		s32Ret = CVI_VO_SetDisplayBufLen(VoLayer, stVoConfig.u32DisBufLen);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_VO_SetDisplayBufLen failed with %#x!\n", s32Ret);
			goto ERR_VO_ENABLE;
		}
	}

	s32Ret = CVI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		goto ERR_VO_ENABLE;
	}

	s32Ret = CVI_VO_EnableVideoLayer(VoLayer);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		goto ERR_VO_ENABLE;
	}

	stChnAttr.stRect = stDefDispRect;
	s32Ret = CVI_VO_SetChnAttr(VoLayer, VoChn, &stChnAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		goto ERR_VO_ENABLE_VIDEOLAYER;
	}

	s32Ret = CVI_VO_EnableChn(VoLayer, VoChn);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		goto ERR_VO_ENABLE_VIDEOLAYER;
	}

	system("devmem 0x0a088094 32 0x0701000a"); //colorbar
	PAUSE();

	CVI_VO_DisableChn(VoLayer, VoChn);
ERR_VO_ENABLE_VIDEOLAYER:
	CVI_VO_DisableVideoLayer(VoLayer);
ERR_VO_ENABLE:
	CVI_VO_Disable(VoDev);
ERR_VO_SETPUBATTR:
	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}
