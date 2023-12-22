/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/common/sample_common_vi.c
 * Description:
 *   Common sample code for video input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cvi_mipi.h"
#include "cvi_sns_ctrl.h"
#include <linux/cvi_defines.h>
#include <linux/cvi_common.h>
#include "cvi_awb_comm.h"
#include "cvi_af_comm.h"
#include "cvi_comm_isp.h"
#include "sample_comm.h"
#include "cvi_isp.h"

// default is output YUV420 image
VI_CHN_ATTR_S CHN_ATTR_420_SDR8 = {
	{1920, 1080},
	PIXEL_FORMAT_YUV_PLANAR_420,
	DYNAMIC_RANGE_SDR8,
	VIDEO_FORMAT_LINEAR,
	COMPRESS_MODE_NONE,
	CVI_FALSE, CVI_FALSE,
	0,
	{ -1, -1},
	-1
};

CVI_CHAR *SAMPLE_COMM_VI_GetSnsrTypeName(void)
{
	return SAMPLE_COMM_SNS_GetSnsrTypeName();
}

CVI_S32 SAMPLE_COMM_VI_GetYuvBypassSts(SAMPLE_SNS_TYPE_E enSnsType)
{
	return SAMPLE_COMM_SNS_GetYuvBypassSts(enSnsType);
}

CVI_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_COMM_SNS_GetSize(enMode, penSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SNS_GetSize failed with %#x\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S *pstSize)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_COMM_SNS_GetPicSize(enPicSize, pstSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SNS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_GetDevAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S *pstViDevAttr)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_COMM_SNS_GetDevAttr(enSnsType, pstViDevAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SNS_GetDevAttr failed with %#x\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_ParseIni(SAMPLE_INI_CFG_S *pstIniCfg)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_COMM_SNS_ParseIni(pstIniCfg);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SNS_ParseIni failed with %#x\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_SetIniPath(const CVI_CHAR *iniPath)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_COMM_SNS_SetIniPath(iniPath);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SNS_SetIniPath failed with %#x\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

void SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 i;

	memset(pstViConfig, 0, sizeof(*pstViConfig));

	for (i = 0; i < VI_MAX_DEV_NUM; i++) {
		pstViConfig->astViInfo[i].stSnsInfo.s32SnsId = i;
		pstViConfig->astViInfo[i].stSnsInfo.s32BusId = i;
		pstViConfig->astViInfo[i].stSnsInfo.MipiDev  = i;
		memset(&pstViConfig->astViInfo[i].stSnapInfo, 0, sizeof(SAMPLE_SNAP_INFO_S));
		pstViConfig->astViInfo[i].stPipeInfo.bMultiPipe = CVI_FALSE;
		pstViConfig->astViInfo[i].stPipeInfo.bVcNumCfged = CVI_FALSE;
	}

	pstViConfig->astViInfo[0].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_60FPS_12BIT;
	pstViConfig->astViInfo[1].stSnsInfo.enSnsType = SONY_IMX290_MIPI_2M_60FPS_12BIT;
}

CVI_S32 SAMPLE_COMM_VI_GetChnAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S *pstChnAttr)
{
	VI_DEV_ATTR_S stViDevAttr;

	memcpy(pstChnAttr, &CHN_ATTR_420_SDR8, sizeof(VI_CHN_ATTR_S));

	SAMPLE_COMM_VI_GetDevAttrBySns(enSnsType, &stViDevAttr);
	if (stViDevAttr.enInputDataType == VI_DATA_TYPE_YUV)
		pstChnAttr->enPixelFormat = PIXEL_FORMAT_YUV_PLANAR_422;

	pstChnAttr->stSize.u32Width = stViDevAttr.stSize.u32Width;
	pstChnAttr->stSize.u32Height = stViDevAttr.stSize.u32Height;

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_ResetSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		if (pstViConfig->astViInfo[i].stPipeInfo.aPipe[0] >= VI_MAX_PHY_DEV_NUM)
			return CVI_SUCCESS;
		s32Ret = CVI_MIPI_SetSensorReset(devno, 1);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d reset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_ResetMipi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		if (pstViConfig->astViInfo[i].stPipeInfo.aPipe[0] >= VI_MAX_PHY_DEV_NUM)
			return CVI_SUCCESS;
		s32Ret = CVI_MIPI_SetMipiReset(devno, 1);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "mipi %d reset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_UnresetSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		if (pstViConfig->astViInfo[i].stPipeInfo.aPipe[0] >= VI_MAX_PHY_DEV_NUM)
			return CVI_SUCCESS;
		s32Ret = CVI_MIPI_SetSensorReset(devno, 0);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d unreset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_UnresetMipi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		if (pstViConfig->astViInfo[i].stPipeInfo.aPipe[0] >= VI_MAX_PHY_DEV_NUM)
			return CVI_SUCCESS;
		s32Ret = CVI_MIPI_SetMipiReset(devno, 0);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("mipi %d unreset failed!\n", i);
			return s32Ret;
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	SAMPLE_SNS_TYPE_E enSnsType;

	const ISP_SNS_OBJ_S *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		enSnsType = g_enSnsType[u32SnsId];
		/* need to invert the clk for timnig issue. */
		if (enSnsType == VIVO_MCS369Q_4M_30FPS_12BIT ||
				enSnsType == VIVO_MCS369_2M_30FPS_12BIT)
			CVI_MIPI_SetClkEdge(ViPipe, 0);
		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		pstSnsObj->pfnGetRxAttr(ViPipe, &stDevAttr);
		CVI_MIPI_SetMipiAttr(ViPipe, (CVI_VOID *)&stDevAttr);
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_EnableSensorClock(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = 0, i;
	CVI_U32 devno = 0;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		devno = pstViInfo->stSnsInfo.MipiDev;
		if (pstViConfig->astViInfo[i].stPipeInfo.aPipe[0] >= VI_MAX_PHY_DEV_NUM)
			return CVI_SUCCESS;
		s32Ret = CVI_MIPI_SetSensorClock(devno, 1);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d clock enable failed!\n", i);
			return s32Ret;
		}
	}

	return CVI_SUCCESS;
}

struct VI_PM_DATA_S {
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	CVI_S32 s32DevNo;
};

static struct VI_PM_DATA_S ViPmData[VI_MAX_DEV_NUM] = { 0 };

static CVI_S32 SAMPLE_COMM_VI_SuspendSensor(CVI_VOID *pvData)
{
	struct VI_PM_DATA_S *pstPmData = (struct VI_PM_DATA_S *)pvData;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	ISP_SNS_OBJ_S *pstSnsObj;

	if (!pvData) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pvData is NULL!\n");
		return CVI_FAILURE;
	}
	ViPipe = pstPmData->ViPipe;
	u32SnsId = pstPmData->u32SnsId;

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	if (!pstSnsObj) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d object get failed!\n", u32SnsId);
		return CVI_FAILURE;
	}
	if (!pstSnsObj->pfnStandby) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "no sensor %d object standby callback\n", u32SnsId);
		return CVI_FAILURE;
	}
	pstSnsObj->pfnStandby(ViPipe);

	return CVI_SUCCESS;
}

static CVI_S32 SAMPLE_COMM_VI_ResumeSensor(CVI_VOID *pvData)
{
	struct VI_PM_DATA_S *pstPmData = (struct VI_PM_DATA_S *)pvData;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	ISP_SNS_OBJ_S *pstSnsObj;
	ISP_SENSOR_EXP_FUNC_S pfnSnsExp;

	if (!pvData) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pvData is NULL!\n");
		return CVI_FAILURE;
	}
	ViPipe = pstPmData->ViPipe;
	u32SnsId = pstPmData->u32SnsId;

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	if (!pstSnsObj) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d object get failed!\n", u32SnsId);
		return CVI_FAILURE;
	}

	if (!pstSnsObj->pfnExpSensorCb) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "no sensor %d object ExpSensor callback\n", u32SnsId);
		return CVI_FAILURE;
	}

	memset(&pfnSnsExp, 0, sizeof(pfnSnsExp));
	pstSnsObj->pfnExpSensorCb(&pfnSnsExp);
	if (!pfnSnsExp.pfn_cmos_sensor_init) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "no sensor %d object pfn_cmos_sensor_init callback\n", u32SnsId);
		return CVI_FAILURE;
	}
	pfnSnsExp.pfn_cmos_sensor_init(ViPipe);

	return CVI_SUCCESS;
}

static CVI_S32 SAMPLE_COMM_VI_SuspendMipi(CVI_VOID *pvData)
{
	struct VI_PM_DATA_S *pstPmData = (struct VI_PM_DATA_S *)pvData;
	CVI_S32 devno;
	CVI_S32 s32Ret = 0;

	if (!pvData) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pvData is NULL!\n");
		return CVI_FAILURE;
	}
	devno = pstPmData->s32DevNo;

	s32Ret = CVI_MIPI_SetSensorClock(devno, 0);
	if (s32Ret != CVI_SUCCESS)
		CVI_TRACE_LOG(CVI_DBG_ERR, "dev %d clock enable failed!\n", devno);

	return s32Ret;
}

static CVI_S32 SAMPLE_COMM_VI_ResumeMipi(CVI_VOID *pvData)
{
	struct VI_PM_DATA_S *pstPmData = (struct VI_PM_DATA_S *)pvData;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	CVI_S32 devno;
	ISP_SNS_OBJ_S *pstSnsObj;
	SNS_COMBO_DEV_ATTR_S stDevAttr;
	SAMPLE_SNS_TYPE_E enSnsType;
	CVI_S32 s32Ret = 0;

	if (!pvData) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "pvData is NULL!\n");
		return CVI_FAILURE;
	}

	ViPipe = pstPmData->ViPipe;
	u32SnsId = pstPmData->u32SnsId;
	devno = pstPmData->s32DevNo;

	pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
	if (!pstSnsObj) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d object get failed!\n", u32SnsId);
		return CVI_FAILURE;
	}

	/* get mipi-rx attribute from sensor driver. */
	pstSnsObj->pfnGetRxAttr(ViPipe, &stDevAttr);
	/* sensor reset */
	s32Ret = CVI_MIPI_SetSensorReset(devno, 1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d reset failed!\n", u32SnsId);
		return s32Ret;
	}
	/* mipi-rx reset */
	s32Ret = CVI_MIPI_SetMipiReset(devno, 1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "mipi %d reset failed!\n", u32SnsId);
		return s32Ret;
	}
	/* set mipi-rx attribute */
	enSnsType = g_enSnsType[u32SnsId];
	/* need to invert the clk for timnig issue. */
	if (enSnsType == VIVO_MCS369Q_4M_30FPS_12BIT ||
			enSnsType == VIVO_MCS369_2M_30FPS_12BIT)
		CVI_MIPI_SetClkEdge(ViPipe, 0);
	CVI_MIPI_SetMipiAttr(ViPipe, (CVI_VOID *)&stDevAttr);
	/* enable sensor clock. */
	s32Ret = CVI_MIPI_SetSensorClock(devno, 1);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d clock enable failed!\n", u32SnsId);
		return s32Ret;
	}
	usleep(20);
	/* sensor unreset */
	s32Ret = CVI_MIPI_SetSensorReset(devno, 0);
	if (s32Ret != CVI_SUCCESS)
		CVI_TRACE_LOG(CVI_DBG_ERR, "sensor %d reset failed!\n", u32SnsId);
	usleep(10000);

	return s32Ret;
}

static VI_PM_OPS_S vi_ops = {
	.pfnSnsSuspend = SAMPLE_COMM_VI_SuspendSensor,
	.pfnSnsResume = SAMPLE_COMM_VI_ResumeSensor,
	.pfnMipiSuspend = SAMPLE_COMM_VI_SuspendMipi,
	.pfnMipiResume = SAMPLE_COMM_VI_ResumeMipi,
};

CVI_S32 SAMPLE_COMM_VI_StartSensor(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret, i;
	CVI_U32 u32SnsId;
	VI_PIPE ViPipe;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

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

CVI_S32 SAMPLE_COMM_VI_SensorProbe(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32 s32Ret = CVI_SUCCESS, i;
	VI_PIPE ViPipe;
	CVI_U32 u32SnsId;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;
	const ISP_SNS_OBJ_S *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		pstViInfo = &pstViConfig->astViInfo[i];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];
		u32SnsId = pstViInfo->stSnsInfo.s32SnsId;
		pstSnsObj = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);
		if (pstSnsObj->pfnSnsProbe) {
			s32Ret = pstSnsObj->pfnSnsProbe(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "cannot probe the sensorObj(%p) SnsId(%d)\n",
					pstSnsObj, u32SnsId);
				return s32Ret;
			}
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StartDev(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32             s32Ret;
	VI_DEV              ViDev;
	SAMPLE_SNS_TYPE_E   enSnsType;
	VI_DEV_ATTR_S       stViDevAttr;
	ISP_PUB_ATTR_S      pstPubAttr;

	ViDev       = pstViInfo->stDevInfo.ViDev;
	enSnsType   = pstViInfo->stSnsInfo.enSnsType;

	SAMPLE_COMM_VI_GetDevAttrBySns(enSnsType, &stViDevAttr);
	SAMPLE_COMM_ISP_GetIspAttrBySns(enSnsType, &pstPubAttr);
	stViDevAttr.stWDRAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;
	stViDevAttr.snrFps = (CVI_U32)pstPubAttr.f32FrameRate;

	if (pstViInfo->stSnsInfo.u8MuxDev) {
		stViDevAttr.isMux = true;
		stViDevAttr.switchGpioPin = pstViInfo->stSnsInfo.s16SwitchGpio;
		stViDevAttr.switchGPioPol = pstViInfo->stSnsInfo.u8SwitchPol;
	}

	s32Ret = CVI_VI_SetDevAttr(ViDev, &stViDevAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_SetDevAttr failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_EnableDev(ViDev);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_EnableDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_StopDev(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32 s32Ret;
	VI_DEV ViDev;

	ViDev   = pstViInfo->stDevInfo.ViDev;
	s32Ret  = CVI_VI_DisableDev(ViDev);

	CVI_VI_UnRegChnFlipMirrorCallBack(0, ViDev);
	CVI_VI_UnRegPmCallBack(ViDev);
	memset(&ViPmData[ViDev], 0, sizeof(struct VI_PM_DATA_S));

	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_DisableDev failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_BindPipeDev(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32             i;
	CVI_S32             s32PipeCnt = 0;
	CVI_S32             s32Ret;
	VI_DEV_BIND_PIPE_S  stDevBindPipe = {0};

	for (i = 0; i < 4; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			stDevBindPipe.PipeId[s32PipeCnt] = pstViInfo->stPipeInfo.aPipe[i];
			s32PipeCnt++;
			stDevBindPipe.u32Num = s32PipeCnt;
		}
	}

	s32Ret = CVI_VI_SetDevBindPipe(pstViInfo->stDevInfo.ViDev, &stDevBindPipe);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_VI_SetDevBindPipe failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StartViChn(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32             i;
	CVI_S32             s32Ret = CVI_SUCCESS;
	VI_PIPE             ViPipe = 0;
	VI_CHN              ViChn = 0;
	VI_DEV              ViDev = 0;
	CVI_U32             u32SnsId = 0;
	VI_CHN_ATTR_S       stChnAttr;
	ISP_SNS_OBJ_S       *pstSnsObj;

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		if (i < VI_MAX_DEV_NUM) {
			ViPipe	    = pstViConfig->astViInfo[i].stPipeInfo.aPipe[0];
			ViChn	    = pstViConfig->astViInfo[i].stChnInfo.ViChn;
			ViDev	    = pstViConfig->astViInfo[i].stDevInfo.ViDev;
			u32SnsId    = pstViConfig->astViInfo[i].stSnsInfo.s32SnsId;
			pstSnsObj   = (ISP_SNS_OBJ_S *)SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

			SAMPLE_COMM_VI_GetChnAttrBySns(pstViConfig->astViInfo[i].stSnsInfo.enSnsType, &stChnAttr);

			stChnAttr.enDynamicRange = pstViConfig->astViInfo[i].stChnInfo.enDynamicRange;
			stChnAttr.enVideoFormat  = pstViConfig->astViInfo[i].stChnInfo.enVideoFormat;
			stChnAttr.enCompressMode = pstViConfig->astViInfo[i].stChnInfo.enCompressMode;
			stChnAttr.enPixelFormat = pstViConfig->astViInfo[i].stChnInfo.enPixFormat;
			/* fill the sensor orientation */
			if (pstViConfig->astViInfo[i].stSnsInfo.u8Orien <= 3) {
				stChnAttr.bMirror = pstViConfig->astViInfo[i].stSnsInfo.u8Orien & 0x1;
				stChnAttr.bFlip = pstViConfig->astViInfo[i].stSnsInfo.u8Orien & 0x2;
			}

			s32Ret = CVI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_SetChnAttr failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}

			if (pstSnsObj && pstSnsObj->pfnMirrorFlip)
				CVI_VI_RegChnFlipMirrorCallBack(ViPipe, ViDev, (void *)pstSnsObj->pfnMirrorFlip);

			/* register the power management ops. */
			ViPmData[ViDev].ViPipe = ViPipe;
			ViPmData[ViDev].u32SnsId = u32SnsId;
			ViPmData[ViDev].s32DevNo = pstViConfig->astViInfo[i].stSnsInfo.MipiDev;
			s32Ret = CVI_VI_RegPmCallBack(ViDev, &vi_ops, (CVI_VOID *)&ViPmData[ViDev]);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_RegPmCallBack failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}

			s32Ret = CVI_VI_EnableChn(ViPipe, ViChn);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_EnableChn failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StopViChn(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32             s32Ret = CVI_SUCCESS;
	VI_PIPE             ViPipe = 0;
	VI_CHN              ViChn;
	VI_VPSS_MODE_E      enMastPipeMode;

	ViChn  = pstViInfo->stChnInfo.ViChn;

	if (ViChn < VI_MAX_CHN_NUM) {
		enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;

		if (enMastPipeMode == VI_OFFLINE_VPSS_OFFLINE
		    || enMastPipeMode == VI_ONLINE_VPSS_OFFLINE) {
			s32Ret = CVI_VI_DisableChn(ViPipe, ViChn);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_DisableChn failed with %#x!\n",
								s32Ret);
				return s32Ret;
			}
		}
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_CreateIsp(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	#define USE_OLDAPI_LOADPARA		0

	CVI_S32 i;
	CVI_S32 s32ViNum;
	VI_PIPE ViPipe;
	CVI_S32 s32Ret = CVI_SUCCESS;

	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (!pstViConfig) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		s32ViNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32ViNum];
		ViPipe = pstViInfo->stPipeInfo.aPipe[0];

		s32Ret = SAMPLE_COMM_VI_StartIsp(pstViInfo);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_VI_StartIsp failed !\n");
			return CVI_FAILURE;
		}

		#if USE_OLDAPI_LOADPARA
		s32Ret = SAMPLE_COMM_BIN_ReadBlockParaFrombin(CVI_BIN_ID_ISP0 + i);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_WARN, "read block(%d) para fail: %#x!\n", CVI_BIN_ID_ISP0 + i, s32Ret);
		}
		#else
		if (i == (pstViConfig->s32WorkingViNum - 1)) {
			s32Ret = SAMPLE_COMM_BIN_ReadParaFrombin();
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_WARN, "read para fail: %#x,use default para!\n", s32Ret);
			}
		}
		#endif

		s32Ret = SAMPLE_COMM_ISP_Run(ViPipe);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "ISP_Run failed with %#x!\n", s32Ret);
			return s32Ret;
		}
	}
	return CVI_SUCCESS;
}

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
			#if ENABLE_AF_LIB
			SAMPLE_COMM_ISP_Aflib_Callback(ViPipe);
			#endif

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
				CVI_TRACE_LOG(CVI_DBG_ERR, "Bind Algo failed with %#x!\n", s32Ret);
			}
			s32Ret = CVI_ISP_MemInit(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "Init Ext memory failed with %#x!\n", s32Ret);
				return s32Ret;
			}
			SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPubAttr);
			s32Ret = CVI_ISP_SetPubAttr(ViPipe, &stPubAttr);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "SetPubAttr failed with %#x!\n", s32Ret);
				return s32Ret;
			}
			CVI_ISP_GetStatisticsConfig(0, &stsCfg);
			stsCfg.stAECfg.stCrop[0].bEnable = 0;
			stsCfg.stAECfg.stCrop[0].u16X = stsCfg.stAECfg.stCrop[0].u16Y = 0;
			stsCfg.stAECfg.stCrop[0].u16W = stPubAttr.stWndRect.u32Width;
			stsCfg.stAECfg.stCrop[0].u16H = stPubAttr.stWndRect.u32Height;
			memset(stsCfg.stAECfg.au8Weight, 1,
				AE_WEIGHT_ZONE_ROW * AE_WEIGHT_ZONE_COLUMN * sizeof(CVI_U8));

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
#ifndef ARCH_CV183X
			//LDG
			stsCfg.stFocusCfg.stConfig.u8ThLow = 0;
			stsCfg.stFocusCfg.stConfig.u8ThHigh = 255;
			stsCfg.stFocusCfg.stConfig.u8GainLow = 30;
			stsCfg.stFocusCfg.stConfig.u8GainHigh = 20;
			stsCfg.stFocusCfg.stConfig.u8SlopLow = 8;
			stsCfg.stFocusCfg.stConfig.u8SlopHigh = 15;
#endif
			s32Ret = CVI_ISP_SetStatisticsConfig(ViPipe, &stsCfg);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "ISP Set Statistic failed with %#x!\n", s32Ret);
				return s32Ret;
			}

			s32Ret = CVI_ISP_Init(ViPipe);
			if (s32Ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "ISP Init failed with %#x!\n", s32Ret);
				return s32Ret;
			}
		}
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

static CVI_S32 SAMPLE_COMM_VI_StopSingleViPipe(VI_PIPE ViPipe)
{
	CVI_S32  s32Ret;

	s32Ret = CVI_VI_StopPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_StopPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	s32Ret = CVI_VI_DestroyPipe(ViPipe);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "CVI_VI_DestroyPipe failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VI_StopViPipe(SAMPLE_VI_INFO_S *pstViInfo)
{
	CVI_S32  i, ret = CVI_SUCCESS;
	VI_PIPE ViPipe;

	for (i = 0; i < WDR_MAX_PIPE_NUM; i++) {
		if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM) {
			ViPipe = pstViInfo->stPipeInfo.aPipe[i];
			ret = SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
			if (ret != CVI_SUCCESS) {
				CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_StopViPipe is fail\n");
				return ret;
			}
		}
	}

	return CVI_SUCCESS;
}

static CVI_S32 SAMPLE_COMM_VI_DestroySingleVi(SAMPLE_VI_INFO_S *pstViInfo)
{
	SAMPLE_COMM_VI_StopViChn(pstViInfo);

	SAMPLE_COMM_VI_StopViPipe(pstViInfo);

	SAMPLE_COMM_VI_StopDev(pstViInfo);

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_DestroyVi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
	CVI_S32           i;
	CVI_S32           s32ViNum;
	SAMPLE_VI_INFO_S *pstViInfo = CVI_NULL;

	if (!pstViConfig) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "null ptr\n");
		return CVI_FAILURE;
	}

	for (i = 0; i < pstViConfig->s32WorkingViNum; i++) {
		s32ViNum  = pstViConfig->as32WorkingViId[i];
		pstViInfo = &pstViConfig->astViInfo[s32ViNum];

		SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_OPEN(CVI_VOID)
{
	CVI_S32 s32ret = CVI_SUCCESS;

	s32ret = CVI_SYS_VI_Open();
	if (s32ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI reopen err\n");
	}

	return s32ret;
}

CVI_S32 SAMPLE_COMM_VI_CLOSE(CVI_VOID)
{
	CVI_S32 s32ret = CVI_SUCCESS;

	s32ret = CVI_SYS_VI_Close();
	if (s32ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "VI close err\n");
	}

	return s32ret;
}

/* Helper API to fill the stViConfig according to the pstIniCfg. */
CVI_S32 SAMPLE_COMM_VI_IniToViCfg(SAMPLE_INI_CFG_S *pstIniCfg, SAMPLE_VI_CONFIG_S *pstViConfig)
{
	DYNAMIC_RANGE_E    enDynamicRange	= DYNAMIC_RANGE_SDR8;
	PIXEL_FORMAT_E	   enPixFormat		= VI_PIXEL_FORMAT;
	VIDEO_FORMAT_E	   enVideoFormat	= VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E    enCompressMode	= COMPRESS_MODE_TILE;
	VI_VPSS_MODE_E	   enMastPipeMode	= VI_OFFLINE_VPSS_OFFLINE;
	CVI_S32 s32WorkSnsId = 0;

	if (!pstIniCfg) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	if (!pstViConfig) {
		SAMPLE_PRT("%s: null ptr\n", __func__);
		return CVI_FAILURE;
	}

	SAMPLE_COMM_VI_GetSensorInfo(pstViConfig);

	for (; s32WorkSnsId < pstIniCfg->devNum; s32WorkSnsId++) {
		pstViConfig->s32WorkingViNum					= 1 + s32WorkSnsId;
		pstViConfig->as32WorkingViId[s32WorkSnsId]			= s32WorkSnsId;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.enSnsType	= pstIniCfg->enSnsType[s32WorkSnsId];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.MipiDev		= pstIniCfg->MipiDev[s32WorkSnsId];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.stMclkAttr.bMclkEn =
			pstIniCfg->stMclkAttr[s32WorkSnsId].bMclkEn;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.stMclkAttr.u8Mclk =
			pstIniCfg->stMclkAttr[s32WorkSnsId].u8Mclk;
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.s32BusId		= pstIniCfg->s32BusId[s32WorkSnsId];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.s32SnsI2cAddr	=
			pstIniCfg->s32SnsI2cAddr[s32WorkSnsId];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.u8HwSync		= pstIniCfg->u8HwSync[s32WorkSnsId];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.u8Orien		= pstIniCfg->u8Orien[s32WorkSnsId];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.u8MuxDev		= pstIniCfg->u8MuxDev[s32WorkSnsId];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.s16SwitchGpio	=
			pstIniCfg->s16SwitchGpio[s32WorkSnsId];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.u8SwitchPol	= pstIniCfg->u8SwitchPol[s32WorkSnsId];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[0]	=
			pstIniCfg->as16LaneId[s32WorkSnsId][0];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[1]	=
			pstIniCfg->as16LaneId[s32WorkSnsId][1];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[2]	=
			pstIniCfg->as16LaneId[s32WorkSnsId][2];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[3]	=
			pstIniCfg->as16LaneId[s32WorkSnsId][3];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as16LaneId[4]	=
			pstIniCfg->as16LaneId[s32WorkSnsId][4];

		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[0]	=
			pstIniCfg->as8PNSwap[s32WorkSnsId][0];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[1]	=
			pstIniCfg->as8PNSwap[s32WorkSnsId][1];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[2]	=
			pstIniCfg->as8PNSwap[s32WorkSnsId][2];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[3]	=
			pstIniCfg->as8PNSwap[s32WorkSnsId][3];
		pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.as8PNSwap[4]	=
			pstIniCfg->as8PNSwap[s32WorkSnsId][4];

		if (pstIniCfg->u8MuxDev[s32WorkSnsId] && pstIniCfg->u8AttachDev[s32WorkSnsId] > 0) {
			pstViConfig->astViInfo[s32WorkSnsId].stDevInfo.ViDev		=
				pstIniCfg->u8AttachDev[s32WorkSnsId] + VI_MAX_PHY_DEV_NUM - 1;
			pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	=
				pstIniCfg->u8AttachDev[s32WorkSnsId] + VI_MAX_PHY_DEV_NUM - 1;
		} else {
			pstViConfig->astViInfo[s32WorkSnsId].stDevInfo.ViDev		= s32WorkSnsId;
			pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]	= s32WorkSnsId;
		}

		pstViConfig->astViInfo[s32WorkSnsId].stDevInfo.enWDRMode	= pstIniCfg->enWDRMode[s32WorkSnsId];

		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;

		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]	= -1;
		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]	= -1;
		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]	= -1;
		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[4]	= -1;
		pstViConfig->astViInfo[s32WorkSnsId].stPipeInfo.aPipe[5]	= -1;

		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.ViChn		= s32WorkSnsId;
		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.enPixFormat	= enPixFormat;
		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange	= enDynamicRange;
		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat	= enVideoFormat;
		pstViConfig->astViInfo[s32WorkSnsId].stChnInfo.enCompressMode	= enCompressMode;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VI_DefaultConfig(CVI_VOID)
{
	SAMPLE_INI_CFG_S stIniCfg = {0};
	SAMPLE_VI_CONFIG_S stViConfig;
	PIC_SIZE_E enPicSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = SONY_IMX327_MIPI_2M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.s32BusId[0]  = 3,
		.s32SnsI2cAddr[0] = -1,
		.MipiDev[0]   = 0xFF,
		.u8UseMultiSns = 0,
	};

	// Get config from ini if found.
	if (SAMPLE_COMM_VI_ParseIni(&stIniCfg)) {
		SAMPLE_PRT("Parse complete\n");
	}

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

	if (stIniCfg.enSource == VI_PIPE_FRAME_SOURCE_DEV) {
		s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
		if (s32Ret != CVI_SUCCESS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
			return s32Ret;
		}
	}

	return s32Ret;
}