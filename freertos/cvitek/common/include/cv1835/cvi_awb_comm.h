/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_awb_comm.h
 * Description:
 */

#ifndef __CVI_AWB_COMM_H__
#define __CVI_AWB_COMM_H__

#include "cvi_comm_video.h"
#include "cvi_defines.h"
#include "cvi_comm_3a.h"
#include "cvi_type.h"

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

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_AWB_COMM_H__ */
