/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_awb.h
 * Description:
 */

 #ifndef __CVI_AWB_H__
#define __CVI_AWB_H__

#include "cvi_comm_isp.h"
#include "cvi_comm_3a.h"
#include "cvi_awb_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define AWB_LOG_BUFF_SIZE	(150*1024)
#define AWB_SNAP_LOG_BUFF_SIZE	(48*1024)

typedef struct cviISP_WB_CURVE_S {
	CVI_U16 u16CurveSize;
	CVI_U16 *u16RGain;
	CVI_U16 *u16BGain;
	CVI_U16 *u16BGainTop;
	CVI_U16 *u16BGainBottom;
} ISP_WB_CURVE_S;



/* The interface of awb lib register to isp. */
CVI_S32 CVI_AWB_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);
CVI_S32 CVI_AWB_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);

/* The callback function of sensor register to awb lib. */
CVI_S32 CVI_AWB_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
				AWB_SENSOR_REGISTER_S *pstRegister);
CVI_S32 CVI_AWB_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, SENSOR_ID SensorId);

CVI_S32 CVI_ISP_SetWBAttr(VI_PIPE ViPipe, const ISP_WB_ATTR_S *pstWBAttr);
CVI_S32 CVI_ISP_GetWBAttr(VI_PIPE ViPipe, ISP_WB_ATTR_S *pstWBAttr);

CVI_S32 CVI_ISP_QueryWBInfo(VI_PIPE ViPipe, ISP_WB_INFO_S *pstWBInfo);
CVI_S32 CVI_ISP_SetWBCalibration(VI_PIPE ViPipe, const ISP_AWB_Calibration_Gain_S *pstWBCalib);
CVI_S32 CVI_ISP_GetWBCalibration(VI_PIPE ViPipe, ISP_AWB_Calibration_Gain_S *pstWBCalib);
CVI_S32 CVI_ISP_SetAWBWinStatictics(VI_PIPE ViPipe, const ISP_WB_WIN_STATISTICS_CFG_S *stAwbWinCfg);
CVI_S32 CVI_ISP_GetAWBWinStatictics(VI_PIPE ViPipe, ISP_WB_WIN_STATISTICS_CFG_S *stAwbWinCfg);

CVI_S32 CVI_ISP_GetAWBSnapLogBuf(VI_PIPE ViPipe, CVI_U8 *buf, CVI_U32 bufSize);
CVI_S32 CVI_ISP_GetAWBDbgBinBuf(VI_PIPE ViPipe, CVI_U8 *buf, CVI_U32 bufSize);
CVI_S32 CVI_ISP_GetAWBDbgBinSize(VI_PIPE ViPipe);

CVI_S32 CVI_ISP_GetAWBCurve(VI_PIPE ViPipe, ISP_WB_CURVE_S *pshWBCurve);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_AWB_H__ */
