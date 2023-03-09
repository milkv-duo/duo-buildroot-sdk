/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: include/cvi_awb_comm.h
 * Description:
 */

#ifndef __CVI_AWB_COMM_H__
#define __CVI_AWB_COMM_H__

#include "cvi_comm_inc.h"
#include "cvi_comm_3a.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define CVI_AWB_LIB_NAME "cvi_awb_lib"

typedef struct cviAWB_SENSOR_DEFAULT_S {
	CVI_U16 u16WbRefTemp;
	CVI_U16 au16GainOffset[ISP_BAYER_CHN_NUM];
	//    CVI_S32  as32WbPara[AWB_CURVE_PARA_NUM];

	CVI_U16 u16GoldenRgain;
	CVI_U16 u16GoldenBgain;
	CVI_U16 u16SampleRgain;
	CVI_U16 u16SampleBgain;
	//    AWB_AGC_TABLE_S stAgcTbl;
	//    AWB_CCM_S stCcm;
	CVI_U16 u16InitRgain;
	CVI_U16 u16InitGgain;
	CVI_U16 u16InitBgain;
	CVI_U8 u8AWBRunInterval;
} AWB_SENSOR_DEFAULT_S;

typedef struct cviAWB_SENSOR_EXP_FUNC_S {
	CVI_S32 (*pfn_cmos_get_awb_default)(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft);
	//    CVI_S32(*pfn_cmos_get_awb_spec_default)(VI_PIPE ViPipe, AWB_SPEC_SENSOR_DEFAULT_S *pstAwbSpecSnsDft);
} AWB_SENSOR_EXP_FUNC_S;

typedef struct cviAWB_SENSOR_REGISTER_S {
	AWB_SENSOR_EXP_FUNC_S stAwbExp;
} AWB_SENSOR_REGISTER_S;

typedef struct _ISP_WB_CURVE_S {
	CVI_U16 u16CurveSize;
	CVI_U16 *u16RGain;
	CVI_U16 *u16BGain;
	CVI_U16 *u16BGainTop;
	CVI_U16 *u16BGainBottom;
} ISP_WB_CURVE_S;

typedef struct _ISP_WB_Q_INFO_S {
	CVI_U16 u16Rgain;
	CVI_U16 u16Grgain;
	CVI_U16 u16Gbgain;
	CVI_U16 u16Bgain;
	CVI_U16 u16Saturation;
	CVI_U16 u16ColorTemp;
	CVI_U16 u16LS0CT;
	CVI_U16 u16LS1CT;
	CVI_U16 u16LS0Area;
	CVI_U16 u16LS1Area;
	CVI_U8  u8MultiDegree;
	CVI_U16 u16ActiveShift;
	CVI_U32 u32FirstStableTime;
	ISP_AWB_INDOOR_OUTDOOR_STATUS_E enInOutStatus;
	CVI_S16 s16Bv;
	CVI_U16 u16GrayWorldRgain;
	CVI_U16 u16GrayWorldBgain;
} ISP_WB_Q_INFO_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_AWB_COMM_H__ */
