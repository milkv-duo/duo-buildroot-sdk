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
#endif

static pthread_t g_IspPid[VI_MAX_DEV_NUM];
static CVI_U32 g_au32IspSnsId[ISP_MAX_DEV_NUM] = { 0, 1, 2, 4, 5};

SAMPLE_SNS_TYPE_E g_enSnsType[VI_MAX_DEV_NUM] = {
	SONY_IMX327_MIPI_2M_30FPS_12BIT,
	SONY_IMX290_MIPI_1M_30FPS_12BIT,
};

static ISP_INIT_ATTR_S gstInitAttr[ISP_MAX_DEV_NUM];

CVI_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S *pstPubAttr)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_COMM_SNS_GetIspAttrBySns(enSnsType, pstPubAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SNS_GetIspAttrBySns failed with %#x\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

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

CVI_VOID *SAMPLE_COMM_ISP_GetSnsObj(CVI_U32 u32SnsId)
{
	SAMPLE_SNS_TYPE_E enSnsType;

	enSnsType = g_enSnsType[u32SnsId];
	return SAMPLE_COMM_SNS_GetSnsObj(enSnsType);
}

CVI_S32 SAMPLE_COMM_ISP_PatchSnsObj(CVI_U32 u32SnsId, SAMPLE_SENSOR_INFO_S *pstSnsInfo)
{
	ISP_SNS_OBJ_S *pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	RX_INIT_ATTR_S stRxInitAttr;
	unsigned int i;

	if (pstSnsObj == CVI_NULL) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "fail to find pstSnsObj\n");
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
		s32Ret = pstSnsObj->pfnSetInit(IspDev, pstInitAttr);
		if (s32Ret < 0) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "pfnSetInit error id: %d s32Ret %d\n", IspDev, s32Ret);
			return CVI_FAILURE;
		}
	}
	/* set i2c bus info */
	if (SNSBUS_VLD(s32BusId))
		unSnsrBusInfo.s8I2cDev = (CVI_S8)s32BusId;
	s32Ret = pstSnsObj->pfnSetBusInfo(IspDev, unSnsrBusInfo);
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
