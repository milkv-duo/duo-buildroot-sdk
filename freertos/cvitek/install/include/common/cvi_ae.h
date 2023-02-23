/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: include/cvi_ae.h
 * Description:
 */

#ifndef __CVI_AE_H__
#define __CVI_AE_H__

#include "cvi_comm_isp.h"
#include "cvi_comm_3a.h"
#include "cvi_ae_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum _AE_EXPOSURE_PROGRAM {
	MANUAL_PROGRAM = 1,
	NORMAL_PROGRAM,
	APERATURE_PRIORITY_PROGRAM,
	SHUTTER_PRIORITY_PROGRAM,
	SLOW_SHUTTER_PROGRAM,
	FAST_SHUTTER_PROGRAM,
	PORTRAIT_PROGRAM,
	LANDSCAPE_PROGRAM,
} AE_EXPOSURE_PROGRAM;

typedef enum _AE_EXPOSURE_MODE {
	AUTO_MODE,
	MANUAL_MODE,
} AE_EXPOSURE_MODE;

/* The interface of ae lib register to isp. */
CVI_S32 CVI_AE_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);
CVI_S32 CVI_AE_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);

/* The callback function of sensor register to ae lib. */
CVI_S32 CVI_AE_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
				     AE_SENSOR_REGISTER_S *pstRegister);
CVI_S32 CVI_AE_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, SENSOR_ID SensorId);

CVI_S32 CVI_ISP_SetExposureAttr(VI_PIPE ViPipe, const ISP_EXPOSURE_ATTR_S *pstExpAttr);
CVI_S32 CVI_ISP_GetExposureAttr(VI_PIPE ViPipe, ISP_EXPOSURE_ATTR_S *pstExpAttr);

CVI_S32 CVI_ISP_QueryExposureInfo(VI_PIPE ViPipe, ISP_EXP_INFO_S *pstExpInfo);

CVI_S32 CVI_ISP_SetWDRExposureAttr(VI_PIPE ViPipe, const ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);
CVI_S32 CVI_ISP_GetWDRExposureAttr(VI_PIPE ViPipe, ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);

CVI_S32 CVI_ISP_SetAERouteAttr(VI_PIPE ViPipe, const ISP_AE_ROUTE_S *pstAERouteAttr);
CVI_S32 CVI_ISP_GetAERouteAttr(VI_PIPE ViPipe, ISP_AE_ROUTE_S *pstAERouteAttr);

CVI_S32 CVI_ISP_SetAERouteAttrEx(VI_PIPE ViPipe, const ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);
CVI_S32 CVI_ISP_GetAERouteAttrEx(VI_PIPE ViPipe, ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);

CVI_S32 CVI_ISP_SetSmartExposureAttr(VI_PIPE ViPipe, const ISP_SMART_EXPOSURE_ATTR_S *pstSmartExpAttr);
CVI_S32 CVI_ISP_GetSmartExposureAttr(VI_PIPE ViPipe, ISP_SMART_EXPOSURE_ATTR_S *pstSmartExpAttr);

CVI_S32 CVI_ISP_SetAEStatisticsConfig(VI_PIPE ViPipe, const ISP_AE_STATISTICS_CFG_S *pstAeStatCfg);
CVI_S32 CVI_ISP_GetAEStatisticsConfig(VI_PIPE ViPipe, ISP_AE_STATISTICS_CFG_S *pstAeStatCfg);

CVI_S32 CVI_ISP_SetAERouteSFAttr(VI_PIPE ViPipe, const ISP_AE_ROUTE_S *pstAERouteSFAttr);
CVI_S32 CVI_ISP_GetAERouteSFAttr(VI_PIPE ViPipe, ISP_AE_ROUTE_S *pstAERouteSFAttr);

CVI_S32 CVI_ISP_SetAERouteSFAttrEx(VI_PIPE ViPipe, const ISP_AE_ROUTE_EX_S *pstAERouteSFAttrEx);
CVI_S32 CVI_ISP_GetAERouteSFAttrEx(VI_PIPE ViPipe, ISP_AE_ROUTE_EX_S *pstAERouteSFAttrEx);

CVI_S32 CVI_ISP_SetAntiFlicker(VI_PIPE ViPipe, CVI_BOOL enable, CVI_U8 frequency);
CVI_S32 CVI_ISP_GetAntiFlicker(VI_PIPE ViPipe, CVI_BOOL *pEnable, CVI_U8 *pFrequency);

CVI_S32 CVI_ISP_SetWDRLEOnly(VI_PIPE ViPipe, CVI_BOOL wdrLEOnly);
CVI_S32 CVI_ISP_GetFrameID(VI_PIPE ViPipe, CVI_U32 *frameID);
CVI_S32 CVI_ISP_QueryFps(VI_PIPE ViPipe, CVI_FLOAT *pFps);


CVI_S32 CVI_ISP_GetCurrentLvX100(VI_PIPE ViPipe, CVI_S16 *ps16Lv);

CVI_S32 CVI_ISP_SetFastBootExposure(VI_PIPE ViPipe, CVI_U32 expLine, CVI_U32 again, CVI_U32 dgain, CVI_U32 ispdgain);

CVI_S32 CVI_ISP_SetIrisAttr(VI_PIPE ViPipe, const ISP_IRIS_ATTR_S *pstIrisAttr);
CVI_S32 CVI_ISP_GetIrisAttr(VI_PIPE ViPipe, ISP_IRIS_ATTR_S *pstIrisAttr);
CVI_S32 CVI_ISP_SetDcirisAttr(VI_PIPE ViPipe, const ISP_DCIRIS_ATTR_S *pstDcirisAttr);
CVI_S32 CVI_ISP_GetDcirisAttr(VI_PIPE ViPipe, ISP_DCIRIS_ATTR_S *pstDcirisAttr);

CVI_S32 CVI_ISP_SetAELogPath(const char *szPath);
CVI_S32 CVI_ISP_GetAELogPath(char *szPath, CVI_U32 pathSize);
CVI_S32 CVI_ISP_SetAELogName(const char *szName);
CVI_S32 CVI_ISP_GetAELogName(char *szName, CVI_U32 nameSize);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_AE_H__ */
