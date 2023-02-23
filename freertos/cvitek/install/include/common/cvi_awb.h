/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
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

#define AWB_LOG_BUFF_SIZE	(48*1024)
#define AWB_SNAP_LOG_BUFF_SIZE	(AWB_LOG_BUFF_SIZE)

/* The interface of awb lib register to isp. */
CVI_S32 CVI_AWB_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);
CVI_S32 CVI_AWB_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);

/* The callback function of sensor register to awb lib. */
CVI_S32 CVI_AWB_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
				AWB_SENSOR_REGISTER_S *pstRegister);
CVI_S32 CVI_AWB_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, SENSOR_ID SensorId);

CVI_S32 CVI_AWB_QueryInfo(VI_PIPE ViPipe, ISP_WB_Q_INFO_S *pstWB_Q_Info);

CVI_S32 CVI_ISP_SetWBAttr(VI_PIPE ViPipe, const ISP_WB_ATTR_S *pstWBAttr);
CVI_S32 CVI_ISP_GetWBAttr(VI_PIPE ViPipe, ISP_WB_ATTR_S *pstWBAttr);
CVI_S32 CVI_ISP_SetAWBAttrEx(VI_PIPE ViPipe, const ISP_AWB_ATTR_EX_S *pstAWBAttrEx);
CVI_S32 CVI_ISP_GetAWBAttrEx(VI_PIPE ViPipe, ISP_AWB_ATTR_EX_S *pstAWBAttrEx);

CVI_S32 CVI_ISP_QueryWBInfo(VI_PIPE ViPipe, ISP_WB_INFO_S *pstWBInfo);

CVI_S32 CVI_ISP_GetGrayWorldAwbInfo(VI_PIPE ViPipe, CVI_U16 *pRgain, CVI_U16 *pBgain);

CVI_S32 CVI_ISP_SetAWBLogPath(const char *szPath);
CVI_S32 CVI_ISP_SetAWBLogName(const char *szName);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_AWB_H__ */
