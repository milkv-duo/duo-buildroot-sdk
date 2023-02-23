/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/common/sample_common_isp.c
 * Description:
 *   Common ctrl code for isp.
 */
#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
//#include <stdint.h>
#include <string.h>
//#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//#include <unistd.h>
//#include <sys/prctl.h>
//#include <sys/time.h>

#include "sample_comm.h"
#include "cvi_awb_comm.h"
#include "cvi_sns_ctrl.h"
#include "cvi_ae.h"
#include "cvi_isp.h"

#ifdef SUPPORT_ISP_PQTOOL
#include <dlfcn.h>
static CVI_BOOL g_ISPDaemon = CVI_FALSE;
static void *g_ISPDHandle;
#define ISPD_LIBNAME "libcvi_ispd.so"
#define ISPD_CONNECT_PORT 5566
#endif //

//static pthread_t g_IspPid[VI_MAX_DEV_NUM];
static CVI_U32 g_au32IspSnsId[ISP_MAX_DEV_NUM] = { 0, 1 };

SAMPLE_SNS_TYPE_E g_enSnsType[MAX_SENSOR_NUM] = {
	SONY_IMX327_MIPI_2M_30FPS_12BIT,
	SONY_IMX290_MIPI_1M_30FPS_12BIT,
};

static ISP_INIT_ATTR_S gstInitAttr[ISP_MAX_DEV_NUM];
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 30, BAYER_RGGB, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_5M_30FPS = { { 0, 0, 2592, 1944 }, { 2592, 1944 }, 30, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_4M_30FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 }, 30, BAYER_RGGB, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX334_8M_30FPS = { { 0, 0, 3840, 2160 }, { 3840, 2160 }, 30, BAYER_RGGB, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 30, BAYER_RGGB, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_F35_2M_30FPS = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 30, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_H65_1M_30FPS = { { 0, 0, 1280, 720 }, { 1280, 720 }, 30, BAYER_BGGR, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OS08A20_8M_30FPS = { { 0, 0, 3840, 2160 }, { 3840, 2160 },
						30, BAYER_BGGR, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OS08A20_5M_30FPS = { { 0, 0, 2592, 1944 }, { 2592, 1944 },
						30, BAYER_BGGR, WDR_MODE_NONE, 0};

ISP_PUB_ATTR_S ISP_PUB_ATTR_PICO_640 = { { 0, 0, 632, 479 }, { 632, 479 },
						30, BAYER_BGGR, WDR_MODE_NONE, 0};


ISP_PUB_ATTR_S ISP_PUB_ATTR_PICO_384 = { { 0, 0, 384, 288 }, { 384, 288 },
						30, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_VIVO_MCS369Q = { { 0, 0, 2560, 1440 }, { 2560, 1440 },
						30, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_VIVO_MCS369 = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						30, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_VIVO_MM308M2 = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_NEXTCHIP_N5 = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC3335_3M_30FPS = { { 0, 0, 2304, 1296 }, { 2304, 1296 },
						30, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_PIXELPLUS_PR2020 = { { 0, 0, 1920, 1080 }, { 1920, 1080 },
						25, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC8238_8M_30FPS = { { 0, 0, 3840, 2160 }, { 3840, 2160 },
						30, BAYER_BGGR, WDR_MODE_NONE, 0};
ISP_PUB_ATTR_S ISP_PUB_ATTR_SC4210_4M_30FPS = { { 0, 0, 2560, 1440 }, { 2560, 1440 },
						30, BAYER_BGGR, WDR_MODE_NONE, 0};

#if 0
static CVI_VOID *SAMPLE_COMM_ISP_Thread(void *arg)
{
	CVI_S32 s32Ret = 0;
	CVI_U8 IspDev = *(CVI_U8 *)arg;
	char szThreadName[20];

	free(arg);
	snprintf(szThreadName, sizeof(szThreadName), "ISP%d_RUN", IspDev);
	prctl(PR_SET_NAME, szThreadName, 0, 0, 0);

	SAMPLE_PRT("ISP Dev %d running!\n", IspDev);
	s32Ret = CVI_ISP_Run(IspDev);
	if (s32Ret != 0)
		SAMPLE_PRT("CVI_ISP_Run failed with %#x!\n", s32Ret);

	return NULL;
}

CVI_S32 SAMPLE_COMM_ISP_Run(CVI_U8 IspDev)
{
	CVI_S32 s32Ret = 0;
	CVI_U8 *arg = malloc(sizeof(*arg));
	struct sched_param param;
	pthread_attr_t attr;

	if (arg == NULL) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "malloc failed\n");
		goto out;
	}

	*arg = IspDev;
	param.sched_priority = 80;

	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	s32Ret = pthread_create(&g_IspPid[IspDev], &attr, SAMPLE_COMM_ISP_Thread, arg);
	if (s32Ret != 0) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "create isp running thread failed!, error: %d, %s\r\n",
					s32Ret, strerror(s32Ret));
		goto out;
	}

#ifdef SUPPORT_ISP_PQTOOL
	if (!g_ISPDaemon) {
		g_ISPDHandle = dlopen(ISPD_LIBNAME, RTLD_NOW);

		if (g_ISPDHandle) {
			char *error = NULL;
			void (*daemon_init)(unsigned int port);

			printf("Load dynamic library %s success\n", ISPD_LIBNAME);

			dlerror();
			daemon_init = dlsym(g_ISPDHandle, "isp_daemon_init");
			error = dlerror();
			if (error == NULL) {
				(*daemon_init)(ISPD_CONNECT_PORT);
				g_ISPDaemon = CVI_TRUE;
			} else {
				printf("Run daemon initial fail\n");
				dlclose(g_ISPDHandle);
			}
		} else {
			printf("Load dynamic library %s fail\n", ISPD_LIBNAME);
		}
	}
#endif //

out:

	return s32Ret;
}
#endif
CVI_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S *pstPubAttr)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	switch (enSnsType) {
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;

	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX334_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX334_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_F35_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_F35_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;

	case SOI_H65_MIPI_1M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_H65_1M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;

	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_OS08A20_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	case PICO640_THERMAL_479P:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PICO_640, sizeof(ISP_PUB_ATTR_S));
		break;
	case PICO384_THERMAL_384X288:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PICO_384, sizeof(ISP_PUB_ATTR_S));
		break;
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_VIVO_MCS369Q, sizeof(ISP_PUB_ATTR_S));
		break;
	case VIVO_MCS369_2M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_VIVO_MCS369, sizeof(ISP_PUB_ATTR_S));
		break;
	case VIVO_MM308M2_2M_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_VIVO_MM308M2, sizeof(ISP_PUB_ATTR_S));
		break;
	case NEXTCHIP_N5_2M_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_NEXTCHIP_N5, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC3335_3M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_PIXELPLUS_PR2020, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC8238_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC8238_8M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		pstPubAttr->f32FrameRate = 15;
		break;
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC4210_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		break;
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
		memcpy(pstPubAttr, &ISP_PUB_ATTR_SC4210_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;
		break;
	default:
		s32Ret = CVI_FAILURE;
		break;
	}
	return s32Ret;
}
#if 0
/******************************************************************************
 * funciton : stop ISP, and stop isp thread
 ******************************************************************************/
CVI_VOID SAMPLE_COMM_ISP_Stop(CVI_U8 IspDev)
{
#ifdef SUPPORT_ISP_PQTOOL
	if (g_ISPDaemon) {
		char *error = NULL;
		void (*daemon_uninit)(void);

		daemon_uninit = dlsym(g_ISPDHandle, "isp_daemon_uninit");
		error = dlerror();
		if (error == NULL)
			(*daemon_uninit)();

		dlclose(g_ISPDHandle);
		g_ISPDHandle = NULL;
		g_ISPDaemon = CVI_FALSE;
	}
#endif //

	if (g_IspPid[IspDev]) {
		CVI_ISP_Exit(IspDev);
		pthread_join(g_IspPid[IspDev], NULL);
		g_IspPid[IspDev] = 0;
	}

	SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(IspDev);
	SAMPLE_COMM_ISP_Aelib_UnCallback(IspDev);
	SAMPLE_COMM_ISP_Awblib_UnCallback(IspDev);
}

CVI_VOID SAMPLE_COMM_All_ISP_Stop(CVI_VOID)
{
	for (ISP_DEV IspDev = 0; IspDev < VI_MAX_DEV_NUM; IspDev++)
		SAMPLE_COMM_ISP_Stop(IspDev);
}

CVI_S32 SAMPLE_COMM_ISP_Awblib_Callback(ISP_DEV IspDev)
{
	ALG_LIB_S stAwbLib;

	stAwbLib.s32Id = IspDev;
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(CVI_AWB_LIB_NAME));
	CVI_AWB_Register(IspDev, &stAwbLib);
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Awblib_UnCallback(ISP_DEV IspDev)
{
	CVI_S32 s32Ret = 0;
	ALG_LIB_S stAwbLib;

	stAwbLib.s32Id = IspDev;
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(CVI_AWB_LIB_NAME));
	s32Ret = CVI_AWB_UnRegister(IspDev, &stAwbLib);
	if (s32Ret) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AWB Algo unRegister failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Aelib_Callback(ISP_DEV IspDev)
{
	CVI_S32 s32Ret = 0;
	ALG_LIB_S stAeLib;

	stAeLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(CVI_AE_LIB_NAME));
	s32Ret = CVI_AE_Register(IspDev, &stAeLib);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AE Algo register failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Aelib_UnCallback(ISP_DEV IspDev)
{
	CVI_S32 s32Ret = 0;
	ALG_LIB_S stAeLib;

	stAeLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(CVI_AE_LIB_NAME));
	s32Ret = CVI_AE_UnRegister(IspDev, &stAeLib);
	if (s32Ret) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AE Algo unRegister failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}
#endif

CVI_S32 SAMPLE_COMM_ISP_SetSnsObj(CVI_U32 u32SnsId, SAMPLE_SNS_TYPE_E enSnsType)
{
	if (u32SnsId >= ARRAY_SIZE(g_enSnsType))
		return CVI_FAILURE;

	g_enSnsType[u32SnsId] = enSnsType;
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_SetSnsInit(CVI_U32 u32SnsId, CVI_U8 u8HwSync)
{
	if (u32SnsId >= ARRAY_SIZE(g_enSnsType))
		return CVI_FAILURE;

	gstInitAttr[u32SnsId].u16UseHwSync = u8HwSync;

	return CVI_SUCCESS;
}

CVI_VOID *SAMPLE_COMM_ISP_GetSnsObj(CVI_U32 u32SnsId)
{
	SAMPLE_SNS_TYPE_E enSnsType;

	enSnsType = g_enSnsType[u32SnsId];
	switch (enSnsType) {
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx327_Obj;
#if 0
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
	case SONY_IMX290_MIPI_2M_60FPS_12BIT:
		return &stSnsImx290_2l_Obj;
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx327_2l_Obj;
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx327_Slave_Obj;
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx307_Obj;
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx307_2l_Obj;
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx307_Slave_Obj;
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
		return &stSnsF35_Obj;
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		return &stSnsF35_Slave_Obj;
	case SOI_H65_MIPI_1M_30FPS_10BIT:
		return &stSnsH65_Obj;
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
		return &stSnsOs08a20_Obj;
	case PICO640_THERMAL_479P:
		return &stSnsPICO640_Obj;
	case PICO384_THERMAL_384X288:
		return &stSnsPICO384_Obj;
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx327_Sublvds_Obj;
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx307_Sublvds_Obj;
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		return &stSnsMCS369Q_Obj;
	case VIVO_MCS369_2M_30FPS_12BIT:
		return &stSnsMCS369_Obj;
	case VIVO_MM308M2_2M_25FPS_8BIT:
		return &stSnsMM308M2_Obj;
	case NEXTCHIP_N5_2M_25FPS_8BIT:
		return &stSnsN5_Obj;
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
		return &stSnsSC3335_Obj;
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx334_Obj;
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
		return &stSnsImx335_Obj;
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
		return &stSnsPR2020_Obj;
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		return &stSnsSC8238_Obj;
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
		return &stSnsSC4210_Obj;
#endif
	default:
		return CVI_NULL;
	}
}

CVI_S32 SAMPLE_COMM_ISP_PatchSnsObj(CVI_U32 u32SnsId, SAMPLE_SENSOR_INFO_S *pstSnsInfo)
{
	ISP_SNS_OBJ_S *pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	RX_INIT_ATTR_S stRxInitAttr;
	CVI_U32 i;

	if (pstSnsObj == CVI_NULL) {
		return CVI_FAILURE;
	}

	stRxInitAttr.MipiDev = pstSnsInfo->MipiDev;
	for (i = 0; i < sizeof(stRxInitAttr.as16LaneId)/sizeof(CVI_S16); i++) {
		stRxInitAttr.as16LaneId[i] = pstSnsInfo->as16LaneId[i];
	}
	for (i = 0; i < sizeof(stRxInitAttr.as8PNSwap)/sizeof(CVI_S8); i++) {
		stRxInitAttr.as8PNSwap[i] = pstSnsInfo->as8PNSwap[i];
	}

	return (pstSnsObj->pfnPatchRxAttr) ? pstSnsObj->pfnPatchRxAttr(&stRxInitAttr) : CVI_SUCCESS;
}

#if 0
CVI_S32 SAMPLE_COMM_ISP_GetRxAttr(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret, i;
	ISP_SENSOR_EXP_FUNC_S stSnsrSensorFunc;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	const ISP_SNS_OBJ_S *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[i];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;

		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		pstSnsObj.pfnGetRxAttr(ViPipe, &stDevAttr);
	}
}
#endif

CVI_S32 SAMPLE_COMM_ISP_SetSensorMode(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = CVI_SUCCESS, i;
	CVI_U32 u32SnsId;
	VI_PIPE ViPipe;
	WDR_MODE_E wdrMode;
	ISP_PUB_ATTR_S stPubAttr;
	ISP_SENSOR_EXP_FUNC_S stSnsrSensorFunc;
	ISP_CMOS_SENSOR_IMAGE_MODE_S stSnsrMode;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	const ISP_SNS_OBJ_S *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		wdrMode = pstViInfo->stDevInfo.enWDRMode;
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;

		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		if (SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPubAttr) != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "Can't get sns attr!\n");
			return s32Ret;
		}
		stSnsrMode.u16Width = stPubAttr.stSnsSize.u32Width;
		stSnsrMode.u16Height = stPubAttr.stSnsSize.u32Height;
		stSnsrMode.f32Fps = stPubAttr.f32FrameRate;
		printf("stSnsrMode.u16Width %d stSnsrMode.u16Height %d %f wdrMode %d pstSnsObj %p\n",
		       stSnsrMode.u16Width, stSnsrMode.u16Height, stSnsrMode.f32Fps, wdrMode, pstSnsObj);
		pstSnsObj->pfnExpSensorCb(&stSnsrSensorFunc);
		s32Ret = stSnsrSensorFunc.pfn_cmos_set_image_mode(ViPipe, &stSnsrMode);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor set image mode failed!\n");
			return s32Ret;
		}

		s32Ret = stSnsrSensorFunc.pfn_cmos_set_wdr_mode(ViPipe, wdrMode);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor set wdr mode failed!\n");
			return s32Ret;
		}
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, CVI_U32 u32SnsId, CVI_S32 s32BusId)
{
	CVI_S32 s32Ret = -1;
	SAMPLE_SNS_TYPE_E enSnsType = g_enSnsType[u32SnsId];
	ALG_LIB_S stAeLib;
	ALG_LIB_S stAwbLib;
	const ISP_SNS_OBJ_S *pstSnsObj;
	ISP_INIT_ATTR_S *pstInitAttr = &gstInitAttr[u32SnsId];
	ISP_SNS_COMMBUS_U unSnsrBusInfo = {
		.s8I2cDev = 3,
	};

	#define SNSBUS_VLD(x)		(x >= 0)

	if (u32SnsId > MAX_SENSOR_NUM) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "invalid sensor id: %d\n", u32SnsId);
		return CVI_FAILURE;
	}

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	if (pstSnsObj == CVI_NULL) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d not exist!\n", u32SnsId);
		return CVI_FAILURE;
	}

	pstInitAttr->enGainMode = SNS_GAIN_MODE_SHARE;
	if ((enSnsType == SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1)) {
		pstInitAttr->enL2SMode = SNS_L2S_MODE_FIX;
	}
	s32Ret = pstSnsObj->pfnSetInit(u32SnsId, pstInitAttr);
	if (s32Ret < 0) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pfnSetInit error id: %d s32Ret %d\n", IspDev, s32Ret);
		return CVI_FAILURE;
	}
	/* set i2c bus info */
	if (SNSBUS_VLD(s32BusId))
		unSnsrBusInfo.s8I2cDev = (CVI_S8)s32BusId;
	s32Ret = pstSnsObj->pfnSetBusInfo(u32SnsId, unSnsrBusInfo);
	if (s32Ret < 0) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pfnSetBusInfo error id: %d s32Ret %d\n", IspDev, s32Ret);
		return CVI_FAILURE;
	}

	stAeLib.s32Id = IspDev;
	stAwbLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(CVI_AE_LIB_NAME));
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(CVI_AWB_LIB_NAME));
	//  strncpy(stAfLib.acLibName, CVI_AF_LIB_NAME, sizeof(CVI_AF_LIB_NAME));

	if (pstSnsObj->pfnRegisterCallback != CVI_NULL) {
		s32Ret = pstSnsObj->pfnRegisterCallback(IspDev, &stAeLib, &stAwbLib);

		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor_register_callback failed with %#x!\n", s32Ret);
			return s32Ret;
		}
	} else {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor_register_callback failed with CVI_NULL!\n");
		return CVI_FAILURE;
	}

	g_au32IspSnsId[IspDev] = u32SnsId;

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev)
{
	ALG_LIB_S stAeLib;
	ALG_LIB_S stAwbLib;
	CVI_U32 u32SnsId;
	const ISP_SNS_OBJ_S *pstSnsObj;
	CVI_S32 s32Ret = -1;

	u32SnsId = g_au32IspSnsId[IspDev];

	if (u32SnsId > MAX_SENSOR_NUM) {
		SAMPLE_PRT("%s: invalid sensor id: %d\n", __func__, u32SnsId);
		return CVI_FAILURE;
	}

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

	if (pstSnsObj == CVI_NULL) {
		return CVI_FAILURE;
	}

	stAeLib.s32Id = IspDev;
	stAwbLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(CVI_AE_LIB_NAME));
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(CVI_AWB_LIB_NAME));
	//   strncpy(stAfLib.acLibName, CVI_AF_LIB_NAME, sizeof(CVI_AF_LIB_NAME));

	if (pstSnsObj->pfnUnRegisterCallback != CVI_NULL) {
		s32Ret = pstSnsObj->pfnUnRegisterCallback(IspDev, &stAeLib, &stAwbLib);

		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("sensor_unregister_callback failed with %#x!\n", s32Ret);
			return s32Ret;
		}
	} else {
		SAMPLE_PRT("sensor_unregister_callback failed with CVI_NULL!\n");
	}

	return CVI_SUCCESS;
}
