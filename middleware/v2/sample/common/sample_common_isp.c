/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/common/sample_common_isp.c
 * Description:
 *   Common ctrl code for isp.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/time.h>

#include <linux/cvi_defines.h>
#include "sample_comm.h"
#include "cvi_awb.h"
#include "cvi_af.h"

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

static pthread_t g_IspPid[VI_MAX_DEV_NUM];
static CVI_U32 g_au32IspSnsId[ISP_MAX_DEV_NUM] = { 0, 1, 2};

SAMPLE_SNS_TYPE_E g_enSnsType[VI_MAX_DEV_NUM] = {
	SONY_IMX327_MIPI_2M_30FPS_12BIT,
	SONY_IMX290_MIPI_1M_30FPS_12BIT,
};

static ISP_INIT_ATTR_S gstInitAttr[ISP_MAX_DEV_NUM];

ISP_PUB_ATTR_S ISP_PUB_ATTR_SAMPLE = { { 0, 0, 1920, 1080 }, { 1920, 1080 }, 30, BAYER_RGGB, WDR_MODE_NONE, 0};

void callback_FPS(int fps)
{
	static CVI_FLOAT uMaxFPS[VI_MAX_DEV_NUM] = {0};
	int i;

	for (i = 0; i < VI_MAX_DEV_NUM && g_IspPid[i]; i++) {
		ISP_PUB_ATTR_S pubAttr = {0};

		CVI_ISP_GetPubAttr(i, &pubAttr);
		if (uMaxFPS[i] == 0) {
			uMaxFPS[i] = pubAttr.f32FrameRate;
		}
		if (fps == 0) {
			pubAttr.f32FrameRate = uMaxFPS[i];
		} else {
			pubAttr.f32FrameRate = (CVI_FLOAT) fps;
		}
		CVI_ISP_SetPubAttr(i, &pubAttr);
	}
}

static CVI_VOID *SAMPLE_COMM_ISP_Thread(void *arg)
{
	CVI_S32 s32Ret = 0;
	CVI_U8 IspDev = *(CVI_U8 *)arg;
	char szThreadName[20];

	free(arg);
	snprintf(szThreadName, sizeof(szThreadName), "ISP%d_RUN", IspDev);
	prctl(PR_SET_NAME, szThreadName, 0, 0, 0);

	if (IspDev > 0) {
		SAMPLE_PRT("ISP Dev %d return\n", IspDev);
		return NULL;
	}

	CVI_SYS_RegisterThermalCallback(callback_FPS);

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

CVI_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S *pstPubAttr)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	PIC_SIZE_E enPicSize;
	SIZE_S stSize;

	memcpy(pstPubAttr, &ISP_PUB_ATTR_SAMPLE, sizeof(ISP_PUB_ATTR_S));

	SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enPicSize);
	SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);

	pstPubAttr->stSnsSize.u32Width = stSize.u32Width;
	pstPubAttr->stSnsSize.u32Height = stSize.u32Height;
	pstPubAttr->stWndRect.u32Width = stSize.u32Width;
	pstPubAttr->stWndRect.u32Height = stSize.u32Height;

	// WDR mode
	if (enSnsType >= SAMPLE_SNS_TYPE_LINEAR_BUTT)
		pstPubAttr->enWDRMode = WDR_MODE_2To1_LINE;

	// FPS
	switch (enSnsType) {
	case SMS_SC035GS_MIPI_480P_120FPS_12BIT:
	case SMS_SC035GS_1L_MIPI_480P_120FPS_10BIT:
	case SMS_SC035HGS_MIPI_480P_120FPS_12BIT:
	case OV_OV6211_MIPI_400P_120FPS_10BIT:
	case OV_OV7251_MIPI_480P_120FPS_10BIT:
		pstPubAttr->f32FrameRate = 120;
		break;
	case SMS_SC1346_1L_MIPI_1M_60FPS_10BIT:
	case SMS_SC1346_1L_MIPI_1M_60FPS_10BIT_WDR2TO1:
	case SMS_SC1346_1L_SLAVE_MIPI_1M_60FPS_10BIT:
	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
		pstPubAttr->f32FrameRate = 60;
		break;
	case TECHPOINT_TP2850_MIPI_2M_30FPS_8BIT:
	case TECHPOINT_TP2850_MIPI_4M_30FPS_8BIT:
	case SMS_SC1346_1L_MIPI_1M_30FPS_10BIT:
	case SMS_SC1346_1L_MIPI_1M_30FPS_10BIT_WDR2TO1:
	case SMS_SC1346_1L_SLAVE_MIPI_1M_30FPS_10BIT:
	case SONY_IMX335_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
		pstPubAttr->f32FrameRate = 30;
		break;
	case GCORE_GC2145_MIPI_2M_12FPS_8BIT:
		pstPubAttr->f32FrameRate = 12;
		break;
#ifdef FPGA_PORTING
	case SONY_IMX327_MIPI_1M_30FPS_10BIT:
	case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
		pstPubAttr->f32FrameRate = 10;
		break;
#endif
	default:
		pstPubAttr->f32FrameRate = 25;
		break;
	}

	switch (enSnsType) {
	case SOI_K06_MIPI_4M_25FPS_10BIT:
		pstPubAttr->enBayer = BAYER_GBRG;
		break;
	// Sony
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
#ifdef FPGA_PORTING
	case SONY_IMX327_MIPI_1M_30FPS_10BIT:
	case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
#endif
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX335_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_2L_MIPI_4M_30FPS_10BIT:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
	// GalaxyCore
	case GCORE_GC02M1_MIPI_2M_30FPS_10BIT:
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case GCORE_GC4023_MIPI_4M_30FPS_10BIT:
		pstPubAttr->enBayer = BAYER_RGGB;
		break;
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
	case GCORE_GC4653_SLAVE_MIPI_4M_30FPS_10BIT:
	case TECHPOINT_TP2850_MIPI_2M_30FPS_8BIT:
	case TECHPOINT_TP2850_MIPI_4M_30FPS_8BIT:
		pstPubAttr->enBayer = BAYER_GRBG;
		break;
#ifdef ARCH_CV182X
	case SOI_F23_MIPI_2M_30FPS_10BIT:
		pstPubAttr->enBayer = BAYER_BGRGI;
	break;
#endif
	default:
		pstPubAttr->enBayer = BAYER_BGGR;
		break;
	};

	return s32Ret;
}

/******************************************************************************
 * funciton : stop ISP, and stop isp thread
 ******************************************************************************/
CVI_VOID SAMPLE_COMM_ISP_Stop(CVI_U8 IspDev)
{
	CVI_S32 s32Ret = CVI_FAILURE;
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
		s32Ret = CVI_ISP_Exit(IspDev);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_ISP_Exit fail with %#x!\n", s32Ret);
			return;
		}
		pthread_join(g_IspPid[IspDev], NULL);
		g_IspPid[IspDev] = 0;
		SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(IspDev);
		SAMPLE_COMM_ISP_Aelib_UnCallback(IspDev);
		SAMPLE_COMM_ISP_Awblib_UnCallback(IspDev);
		#if ENABLE_AF_LIB
		SAMPLE_COMM_ISP_Aflib_UnCallback(IspDev);
		#endif
	}
}

CVI_VOID SAMPLE_COMM_All_ISP_Stop(CVI_VOID)
{
	for (ISP_DEV IspDev = 0; IspDev < VI_MAX_DEV_NUM; IspDev++)
		SAMPLE_COMM_ISP_Stop(IspDev);
}

CVI_S32 SAMPLE_COMM_ISP_Awblib_Callback(ISP_DEV IspDev)
{
	ALG_LIB_S stAwbLib;
	CVI_S32 s32Ret = 0;

	stAwbLib.s32Id = IspDev;
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(stAwbLib.acLibName));
	s32Ret = CVI_AWB_Register(IspDev, &stAwbLib);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AWB Algo register failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Awblib_UnCallback(ISP_DEV IspDev)
{
	CVI_S32 s32Ret = 0;
	ALG_LIB_S stAwbLib;

	stAwbLib.s32Id = IspDev;
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(stAwbLib.acLibName));
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
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(stAeLib.acLibName));
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
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(stAeLib.acLibName));
	s32Ret = CVI_AE_UnRegister(IspDev, &stAeLib);
	if (s32Ret) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AE Algo unRegister failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Aflib_Callback(ISP_DEV IspDev)
{
	ALG_LIB_S stAfLib;
	CVI_S32 s32Ret = 0;

	stAfLib.s32Id = IspDev;
	strncpy(stAfLib.acLibName, CVI_AF_LIB_NAME, sizeof(stAfLib.acLibName));
	s32Ret = CVI_AF_Register(IspDev, &stAfLib);

	if (s32Ret != CVI_SUCCESS) {
		printf("AF Algo register failed!, error: %d\n", s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_ISP_Aflib_UnCallback(ISP_DEV IspDev)
{
	CVI_S32 s32Ret = 0;
	ALG_LIB_S stAfLib;

	stAfLib.s32Id = IspDev;
	strncpy(stAfLib.acLibName, CVI_AF_LIB_NAME, sizeof(stAfLib.acLibName));
	s32Ret = CVI_AF_UnRegister(IspDev, &stAfLib);
	if (s32Ret) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "AF Algo unRegister failed!, error: %d\n",	s32Ret);
		return s32Ret;
	}
	return CVI_SUCCESS;
}


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

CVI_VOID *SAMPLE_COMM_GetSnsObj(SAMPLE_SNS_TYPE_E enSnsType)
{
	CVI_VOID *pSnsObj;

	switch (enSnsType) {
#if defined(SENSOR_BRIGATES_BG0808)
	case BRIGATES_BG0808_MIPI_2M_30FPS_10BIT:
	case BRIGATES_BG0808_MIPI_2M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsBG0808_Obj;
		break;
#endif
#if defined(SENSOR_GCORE_GC02M1)
	case GCORE_GC02M1_MIPI_2M_30FPS_10BIT:
		return &stSnsGc02m1_Obj;
#endif
#if defined(SENSOR_GCORE_GC1054)
	case GCORE_GC1054_MIPI_1M_30FPS_10BIT:
		return &stSnsGc1054_Obj;
#endif
#if defined(SENSOR_GCORE_GC2053)
	case GCORE_GC2053_MIPI_2M_30FPS_10BIT:
		return &stSnsGc2053_Obj;
#endif
#if defined(SENSOR_GCORE_GC2053_SLAVE)
	case GCORE_GC2053_SLAVE_MIPI_2M_30FPS_10BIT:
		return &stSnsGc2053_Slave_Obj;
#endif
#if defined(SENSOR_GCORE_GC2053_1L)
	case GCORE_GC2053_1L_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsGc2053_1l_Obj;
		break;
#endif
#if defined(SENSOR_GCORE_GC2093)
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_MIPI_2M_30FPS_10BIT_WDR2TO1:
		return &stSnsGc2093_Obj;
#endif
#if defined(SENSOR_GCORE_GC2093_SLAVE)
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT:
	case GCORE_GC2093_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		return &stSnsGc2093_Slave_Obj;
#endif
#if defined(SENSOR_GCORE_GC2145)
	case GCORE_GC2145_MIPI_2M_12FPS_8BIT:
		return &stSnsGc2145_Obj;
#endif
#if defined(SENSOR_GCORE_GC4023)
	case GCORE_GC4023_MIPI_4M_30FPS_10BIT:
		return &stSnsGc4023_Obj;
#endif
#if defined(SENSOR_GCORE_GC4653)
	case GCORE_GC4653_MIPI_4M_30FPS_10BIT:
		return &stSnsGc4653_Obj;
#endif
#if defined(SENSOR_GCORE_GC4653_SLAVE)
	case GCORE_GC4653_SLAVE_MIPI_4M_30FPS_10BIT:
		return &stSnsGc4653_Slave_Obj;
#endif
#if defined(SENSOR_NEXTCHIP_N5)
	case NEXTCHIP_N5_2M_25FPS_8BIT:
	case NEXTCHIP_N5_1M_2CH_25FPS_8BIT:
		pSnsObj = &stSnsN5_Obj;
		break;
#endif
#if defined(SENSOR_NEXTCHIP_N6)
	case NEXTCHIP_N6_2M_4CH_25FPS_8BIT:
		pSnsObj = &stSnsN6_Obj;
		break;
#endif
#if defined(SENSOR_OV_OS02D10)
	case OV_OS02D10_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsOs02d10_Obj;
		break;
#endif
#if defined(SENSOR_OV_OS02D10_SLAVE)
	case OV_OS02D10_SLAVE_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsOs02d10_Slave_Obj;
		break;
#endif
#if defined(SENSOR_OV_OS02K10_SLAVE)
	case OV_OS02K10_SLAVE_MIPI_2M_30FPS_12BIT:
		pSnsObj = &stSnsOs02k10_Slave_Obj;
		break;
#endif
#if defined(SENSOR_OV_OS04A10)
	case OV_OS04A10_MIPI_4M_1440P_30FPS_12BIT:
	case OV_OS04A10_MIPI_4M_1440P_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsOs04a10_Obj;
		break;
#endif
#if defined(SENSOR_OV_OS04C10)
	case OV_OS04C10_MIPI_4M_30FPS_12BIT:
	case OV_OS04C10_MIPI_4M_1440P_30FPS_12BIT:
	case OV_OS04C10_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS04C10_MIPI_4M_1440P_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsOs04c10_Obj;
		break;
#endif
#if defined(SENSOR_OV_OS04C10_SLAVE)
	case OV_OS04C10_SLAVE_MIPI_4M_30FPS_12BIT:
	case OV_OS04C10_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsOs04c10_Slave_Obj;
		break;
#endif
#if defined(SENSOR_OV_OS08A20)
	case OV_OS08A20_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsOs08a20_Obj;
		break;
#endif
#if defined(SENSOR_OV_OS08A20_SLAVE)
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT:
	case OV_OS08A20_SLAVE_MIPI_8M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsOs08a20_Slave_Obj;
		break;
#endif
#if defined(SENSOR_OV_OV4689)
	case OV_OV4689_MIPI_4M_30FPS_10BIT:
		pSnsObj = &stSnsOv4689_Obj;
		break;
#endif
#if defined(SENSOR_OV_OV6211)
	case OV_OV6211_MIPI_400P_120FPS_10BIT:
		pSnsObj = &stSnsOv6211_Obj;
		break;
#endif
#if defined(SENSOR_OV_OV7251)
	case OV_OV7251_MIPI_480P_120FPS_10BIT:
		pSnsObj = &stSnsOv7251_Obj;
		break;
#endif
#if defined(SENSOR_PICO_384)
	case PICO384_THERMAL_384X288:
		pSnsObj = &stSnsPICO384_Obj;
		break;
#endif
#if defined(SENSOR_PICO_640)
	case PICO640_THERMAL_479P:
		pSnsObj = &stSnsPICO640_Obj;
		break;
#endif
#if defined(SENSOR_PIXELPLUS_PR2020)
	case PIXELPLUS_PR2020_1M_25FPS_8BIT:
	case PIXELPLUS_PR2020_1M_30FPS_8BIT:
	case PIXELPLUS_PR2020_2M_25FPS_8BIT:
	case PIXELPLUS_PR2020_2M_30FPS_8BIT:
		pSnsObj = &stSnsPR2020_Obj;
		break;
#endif
#if defined(SENSOR_PIXELPLUS_PR2100)
	case PIXELPLUS_PR2100_2M_25FPS_8BIT:
	case PIXELPLUS_PR2100_2M_2CH_25FPS_8BIT:
	case PIXELPLUS_PR2100_2M_4CH_25FPS_8BIT:
		pSnsObj = &stSnsPR2100_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC035GS)
	case SMS_SC035GS_MIPI_480P_120FPS_12BIT:
		pSnsObj = &stSnsSC035GS_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC035GS_1L)
	case SMS_SC035GS_1L_MIPI_480P_120FPS_10BIT:
		pSnsObj = &stSnsSC035GS_1L_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC035HGS)
	case SMS_SC035HGS_MIPI_480P_120FPS_12BIT:
		pSnsObj = &stSnsSC035HGS_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC1346_1L)
	case SMS_SC1346_1L_MIPI_1M_30FPS_10BIT:
	case SMS_SC1346_1L_MIPI_1M_30FPS_10BIT_WDR2TO1:
	case SMS_SC1346_1L_MIPI_1M_60FPS_10BIT:
	case SMS_SC1346_1L_MIPI_1M_60FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC1346_1L_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC1346_1L_SLAVE)
	case SMS_SC1346_1L_SLAVE_MIPI_1M_30FPS_10BIT:
	case SMS_SC1346_1L_SLAVE_MIPI_1M_60FPS_10BIT:
		pSnsObj = &stSnsSC1346_1L_Slave_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC200AI)
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT:
	case SMS_SC200AI_MIPI_2M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC200AI_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC301IOT)
	case SMS_SC301IOT_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsSC301IOT_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC401AI)
	case SMS_SC401AI_MIPI_4M_30FPS_10BIT:
	case SMS_SC401AI_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsSC401AI_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC500AI)
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT:
	case SMS_SC500AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC500AI_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC501AI_2L)
	case SMS_SC501AI_2L_MIPI_5M_30FPS_10BIT:
		pSnsObj = &stSnsSC501AI_2L_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC531AI_2L)
	case SMS_SC531AI_2L_MIPI_5M_30FPS_10BIT:
		pSnsObj = &stSnsSC531AI_2L_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC850SL)
	case SMS_SC850SL_MIPI_8M_30FPS_12BIT:
	case SMS_SC850SL_MIPI_8M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC850SL_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC3332)
	case SMS_SC3332_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsSC3332_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC3335)
	case SMS_SC3335_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsSC3335_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC3335_SLAVE)
	case SMS_SC3335_SLAVE_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsSC3335_Slave_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC3336)
	case SMS_SC3336_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsSC3336_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC2335)
	case SMS_SC2335_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsSC2335_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC2336)
	case SMS_SC2336_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsSC2336_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC4210)
	case SMS_SC4210_MIPI_4M_30FPS_12BIT:
	case SMS_SC4210_MIPI_4M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC4210_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC4336)
	case SMS_SC4336_MIPI_4M_30FPS_10BIT:
		pSnsObj = &stSnsSC4336_Obj;
		break;
#endif
#if defined(SENSOR_SMS_SC8238)
	case SMS_SC8238_MIPI_8M_30FPS_10BIT:
	case SMS_SC8238_MIPI_8M_15FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsSC8238_Obj;
		break;
#endif
#if defined(SENSOR_SOI_F23)
	case SOI_F23_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsF23_Obj;
		break;
#endif
#if defined(SENSOR_SOI_F35)
	case SOI_F35_MIPI_2M_30FPS_10BIT:
	case SOI_F35_MIPI_2M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsF35_Obj;
		break;
#endif
#if defined(SENSOR_SOI_F35_SLAVE)
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT:
	case SOI_F35_SLAVE_MIPI_2M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsF35_Slave_Obj;
		break;
#endif
#if defined(SENSOR_SOI_F37P)
	case SOI_F37P_MIPI_2M_30FPS_10BIT:
		pSnsObj = &stSnsF37P_Obj;
		break;
#endif
#if defined(SENSOR_SOI_H65)
	case SOI_H65_MIPI_1M_30FPS_10BIT:
		pSnsObj = &stSnsH65_Obj;
		break;
#endif
#if defined(SENSOR_SOI_K06)
	case SOI_K06_MIPI_4M_25FPS_10BIT:
		return &stSnsK06_Obj;
#endif
#if defined(SENSOR_SOI_Q03)
	case SOI_Q03_MIPI_3M_30FPS_10BIT:
		pSnsObj = &stSnsQ03_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX290_2L)
	case SONY_IMX290_MIPI_1M_30FPS_12BIT:
	case SONY_IMX290_MIPI_2M_60FPS_12BIT:
		pSnsObj = &stSnsImx290_2l_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX307)
	case SONY_IMX307_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_MIPI_2M_60FPS_12BIT:
	case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx307_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX307_SLAVE)
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx307_Slave_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX307_2L)
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx307_2l_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX307_SUBLVDS)
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_60FPS_12BIT:
	case SONY_IMX307_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx307_Sublvds_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327)
	case SONY_IMX327_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_MIPI_2M_60FPS_12BIT:
	case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327_SLAVE)
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_Slave_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327_2L)
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
	case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_2l_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327_FPGA) && defined(FPGA_PORTING)
	case SONY_IMX327_MIPI_1M_30FPS_10BIT:
	case SONY_IMX327_MIPI_1M_30FPS_10BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_fpga_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX327_SUBLVDS)
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT:
	case SONY_IMX327_SUBLVDS_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx327_Sublvds_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX334)
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx334_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX335)
	case SONY_IMX335_MIPI_2M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_4M_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_2L_MIPI_4M_30FPS_10BIT:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_12BIT:
	case SONY_IMX335_MIPI_4M_1600P_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_5M_30FPS_12BIT:
	case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
	case SONY_IMX335_MIPI_2M_60FPS_10BIT:
	case SONY_IMX335_MIPI_4M_60FPS_10BIT:
	case SONY_IMX335_MIPI_5M_60FPS_10BIT:
		pSnsObj = &stSnsImx335_Obj;
		break;
#endif
#if defined(SENSOR_SONY_IMX347)
	case SONY_IMX347_MIPI_4M_60FPS_12BIT:
	case SONY_IMX347_MIPI_4M_30FPS_12BIT_WDR2TO1:
		return &stSnsImx347_Obj;
#endif
#if defined(SENSOR_SONY_IMX385)
	case SONY_IMX385_MIPI_2M_30FPS_12BIT:
	case SONY_IMX385_MIPI_2M_30FPS_12BIT_WDR2TO1:
		pSnsObj = &stSnsImx385_Obj;
		break;
#endif
#if defined(SENSOR_TECHPOINT_TP2850)
	case TECHPOINT_TP2850_MIPI_2M_30FPS_8BIT:
	case TECHPOINT_TP2850_MIPI_4M_30FPS_8BIT:
		pSnsObj = &stSnsTP2850_Obj;
		break;
#endif
#if defined(SENSOR_VIVO_MCS369)
	case VIVO_MCS369_2M_30FPS_12BIT:
		pSnsObj = &stSnsMCS369_Obj;
		break;
#endif
#if defined(SENSOR_VIVO_MCS369Q)
	case VIVO_MCS369Q_4M_30FPS_12BIT:
		pSnsObj = &stSnsMCS369Q_Obj;
		break;
#endif
#if defined(SENSOR_VIVO_MM308M2)
	case VIVO_MM308M2_2M_25FPS_8BIT:
		pSnsObj = &stSnsMM308M2_Obj;
		break;
#endif
	default:
		pSnsObj = CVI_NULL;
		break;
	}

	return pSnsObj;
}

CVI_VOID *SAMPLE_COMM_ISP_GetSnsObj(CVI_U32 u32SnsId)
{
	SAMPLE_SNS_TYPE_E enSnsType;

	enSnsType = g_enSnsType[u32SnsId];
	return SAMPLE_COMM_GetSnsObj(enSnsType);
}

CVI_S32 SAMPLE_COMM_ISP_PatchSnsObj(CVI_U32 u32SnsId, SAMPLE_SENSOR_INFO_S *pstSnsInfo)
{
	ISP_SNS_OBJ_S *pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	RX_INIT_ATTR_S stRxInitAttr;
	unsigned int i;

	if (pstSnsObj == CVI_NULL) {
		return CVI_FAILURE;
	}

	memset(&stRxInitAttr, 0, sizeof(RX_INIT_ATTR_S));

	stRxInitAttr.MipiDev = pstSnsInfo->MipiDev;
	if (pstSnsInfo->stMclkAttr.bMclkEn) {
		stRxInitAttr.stMclkAttr.bMclkEn = CVI_TRUE;
		stRxInitAttr.stMclkAttr.u8Mclk  = pstSnsInfo->stMclkAttr.u8Mclk;
	}

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

		if (stSnsrSensorFunc.pfn_cmos_set_image_mode) {
			s32Ret = stSnsrSensorFunc.pfn_cmos_set_image_mode(ViPipe, &stSnsrMode);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "sensor set image mode failed!\n");
				return s32Ret;
			}
		}

		if (stSnsrSensorFunc.pfn_cmos_set_wdr_mode) {
			s32Ret = stSnsrSensorFunc.pfn_cmos_set_wdr_mode(ViPipe, wdrMode);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "sensor set wdr mode failed!\n");
				return s32Ret;
			}
		}
	}
	return s32Ret;
}

static SNS_BDG_MUX_MODE_E SAMPLE_COMM_ISP_GetSnsBdgMode(SAMPLE_SNS_TYPE_E enSnsType)
{
	VI_DEV_ATTR_S       stViDevAttr;
	SNS_BDG_MUX_MODE_E  MuxMode;

	SAMPLE_COMM_VI_GetDevAttrBySns(enSnsType, &stViDevAttr);
	switch (stViDevAttr.enWorkMode) {
	case VI_WORK_MODE_1Multiplex:
		MuxMode = SNS_BDG_MUX_NONE;
		break;
	case VI_WORK_MODE_2Multiplex:
		MuxMode = SNS_BDG_MUX_2;
		break;
	case VI_WORK_MODE_3Multiplex:
		MuxMode = SNS_BDG_MUX_3;
		break;
	case VI_WORK_MODE_4Multiplex:
		MuxMode = SNS_BDG_MUX_4;
		break;
	default:
		MuxMode = SNS_BDG_MUX_NONE;
		break;
	}

	return MuxMode;
}

CVI_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, CVI_U32 u32SnsId, CVI_S32 s32BusId,
						CVI_S32 s32I2cAddr)
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

	if (u32SnsId > VI_MAX_DEV_NUM) {
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
		(enSnsType == OV_OS08A20_MIPI_5M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == OV_OS08A20_MIPI_4M_30FPS_10BIT_WDR2TO1) ||
		(enSnsType == OV_OS08A20_SLAVE_MIPI_4M_30FPS_10BIT_WDR2TO1)) {
		pstInitAttr->enL2SMode = SNS_L2S_MODE_FIX;
	}
	pstInitAttr->enSnsBdgMuxMode = SAMPLE_COMM_ISP_GetSnsBdgMode(enSnsType);
	if (pstSnsObj->pfnSetInit) {
		s32Ret = pstSnsObj->pfnSetInit(u32SnsId, pstInitAttr);
		if (s32Ret < 0) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "pfnSetInit error id: %d s32Ret %d\n", IspDev, s32Ret);
			return CVI_FAILURE;
		}
	}
	/* set i2c bus info */
	if (SNSBUS_VLD(s32BusId))
		unSnsrBusInfo.s8I2cDev = (CVI_S8)s32BusId;
	s32Ret = pstSnsObj->pfnSetBusInfo(u32SnsId, unSnsrBusInfo);
	if (s32Ret < 0) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pfnSetBusInfo error id: %d s32Ret %d\n", IspDev, s32Ret);
		return CVI_FAILURE;
	}
	if (pstSnsObj->pfnPatchI2cAddr)
		pstSnsObj->pfnPatchI2cAddr(s32I2cAddr);

	stAeLib.s32Id = IspDev;
	stAwbLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(stAeLib.acLibName));
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(stAwbLib.acLibName));
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

	if (u32SnsId > VI_MAX_DEV_NUM) {
		SAMPLE_PRT("%s: invalid sensor id: %d\n", __func__, u32SnsId);
		return CVI_FAILURE;
	}

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

	if (pstSnsObj == CVI_NULL) {
		return CVI_FAILURE;
	}

	stAeLib.s32Id = IspDev;
	stAwbLib.s32Id = IspDev;
	strncpy(stAeLib.acLibName, CVI_AE_LIB_NAME, sizeof(stAeLib.acLibName));
	strncpy(stAwbLib.acLibName, CVI_AWB_LIB_NAME, sizeof(stAwbLib.acLibName));
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
