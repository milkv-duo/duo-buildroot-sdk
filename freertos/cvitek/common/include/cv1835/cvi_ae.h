/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
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

#define AE_BRACKETING_CAPTURE_NUM		11

#define AE_LOG_BUFF_SIZE	(200*1024)
#define AE_SNAP_LOG_BUFF_SIZE	(6*1024)

#define MAX_FACE_NUM	(3)
typedef struct _CVI_ISP_FACE_DETECT_INFO {
	CVI_U8	u8FDNum;
	CVI_U16 u16FDPosX[MAX_FACE_NUM];
	CVI_U16 u16FDPosY[MAX_FACE_NUM];
	CVI_U16 u16FDWidth[MAX_FACE_NUM];
	CVI_U16 u16FDHeight[MAX_FACE_NUM];
	CVI_U16	u16FrameWidth;
	CVI_U16	u16FrameHeight;
} CVI_ISP_FACE_DETECT_INFO;

/* The interface of ae lib register to isp. */
CVI_S32 CVI_AE_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);
CVI_S32 CVI_AE_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);

/* The callback function of sensor register to ae lib. */
CVI_S32 CVI_AE_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
				     AE_SENSOR_REGISTER_S *pstRegister);
CVI_S32 CVI_AE_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, SENSOR_ID SensorId);

void CVI_ISP_ExposureAttr_Init(void);

CVI_S32 CVI_ISP_SetExposureAttr(VI_PIPE ViPipe, const ISP_EXPOSURE_ATTR_S *pstExpAttr);
CVI_S32 CVI_ISP_GetExposureAttr(VI_PIPE ViPipe, ISP_EXPOSURE_ATTR_S *pstExpAttr);

CVI_S32 CVI_ISP_QueryExposureInfo(VI_PIPE ViPipe, ISP_EXP_INFO_S *pstExpInfo);

CVI_S32 CVI_ISP_SetWDRExposureAttr(VI_PIPE ViPipe, const ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);
CVI_S32 CVI_ISP_GetWDRExposureAttr(VI_PIPE ViPipe, ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);

CVI_S32 CVI_ISP_SetAERouteAttr(VI_PIPE ViPipe, const ISP_AE_ROUTE_S *pstAERouteAttr);
CVI_S32 CVI_ISP_GetAERouteAttr(VI_PIPE ViPipe, ISP_AE_ROUTE_S *pstAERouteAttr);

CVI_S32 CVI_ISP_ConfigAEBracketingExpsoure(VI_PIPE ViPipe);
CVI_S32 CVI_ISP_SetAEBracketingExpsoure(VI_PIPE ViPipe, CVI_U8 Num);
CVI_S32 CVI_ISP_AEBracketingExpsoure_Reset(VI_PIPE ViPipe);
CVI_S32 CVI_ISP_GetAESnapLogBuf(VI_PIPE ViPipe, CVI_U8 *buf, CVI_U32 bufSize);
CVI_S32 CVI_ISP_SetAEWinStatictics(VI_PIPE ViPipe, const ISP_AE_WIN_STATISTICS_CFG_S *stAeWinCfg);
CVI_S32 CVI_ISP_GetAEWinStatictics(VI_PIPE ViPipe, ISP_AE_WIN_STATISTICS_CFG_S *stAeWinCfg);
CVI_S32 CVI_ISP_SetFaceAeInfo(VI_PIPE ViPipe, const CVI_ISP_FACE_DETECT_INFO *stFaceInfo);
CVI_S32 CVI_ISP_GetFaceAeInfo(VI_PIPE ViPipe, CVI_ISP_FACE_DETECT_INFO *stFaceInfo);

CVI_S32 CVI_ISP_SetAntiFlciker(VI_PIPE ViPipe, CVI_BOOL enable, CVI_U8 frequency);
CVI_S32 CVI_ISP_GetAntiFlciker(VI_PIPE ViPipe, CVI_BOOL *pEnable, CVI_U8 *pFrequency);
CVI_S32 CVI_ISP_SetAEFps(VI_PIPE ViPipe, CVI_U16 fps);
CVI_S32 CVI_ISP_GetAEFps(VI_PIPE ViPipe, CVI_U16 *pFps);

#if 0

CVI_S32 CVI_ISP_SetHDRExposureAttr(VI_PIPE ViPipe, const ISP_HDR_EXPOSURE_ATTR_S *pstHDRExpAttr);
CVI_S32 CVI_ISP_GetHDRExposureAttr(VI_PIPE ViPipe, ISP_HDR_EXPOSURE_ATTR_S *pstHDRExpAttr);





CVI_S32 CVI_ISP_SetIrisAttr(VI_PIPE ViPipe, const ISP_IRIS_ATTR_S *pstIrisAttr);
CVI_S32 CVI_ISP_GetIrisAttr(VI_PIPE ViPipe, ISP_IRIS_ATTR_S *pstIrisAttr);

CVI_S32 CVI_ISP_SetDcirisAttr(VI_PIPE ViPipe, const ISP_DCIRIS_ATTR_S *pstDcirisAttr);
CVI_S32 CVI_ISP_GetDcirisAttr(VI_PIPE ViPipe, ISP_DCIRIS_ATTR_S *pstDcirisAttr);

CVI_S32 CVI_ISP_SetPirisAttr(VI_PIPE ViPipe, const ISP_PIRIS_ATTR_S *pstPirisAttr);
CVI_S32 CVI_ISP_GetPirisAttr(VI_PIPE ViPipe, ISP_PIRIS_ATTR_S *pstPirisAttr);

CVI_S32 CVI_ISP_SetAERouteAttrEx(VI_PIPE ViPipe, const ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);
CVI_S32 CVI_ISP_GetAERouteAttrEx(VI_PIPE ViPipe, ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_AE_H__ */
