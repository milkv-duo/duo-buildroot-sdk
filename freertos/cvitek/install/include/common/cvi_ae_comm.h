/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: include/cvi_ae_comm.h
 * Description:
 */

#ifndef __CVI_AE_COMM_H__
#define __CVI_AE_COMM_H__

#include "cvi_comm_inc.h"
#include "cvi_comm_3a.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define CVI_AE_LIB_NAME "cvi_ae_lib"

typedef enum _AE_ACCURACY_E {
	AE_ACCURACY_DB = 0,
	AE_ACCURACY_LINEAR,
	AE_ACCURACY_TABLE,

	AE_ACCURACY_BUTT,
} AE_ACCURACY_E;

typedef enum _AE_BLC_TYPE_E {
	AE_BLC_TYPE_LINEAR = 0,
	AE_BLC_TYPE_LADDER,

	AE_BLC_TYPE_BUTT,
} AE_BLC_TYPE_E;

typedef enum _ISP_SNS_GAIN_MODE_E {
	SNS_GAIN_MODE_SHARE = 0,	/* gain setting for all wdr frames*/
	SNS_GAIN_MODE_WDR_2F,		/* separate gain for 2-frame wdr mode*/
	SNS_GAIN_MODE_WDR_3F,		/* separate gain for 3-frame wdr mode*/
	SNS_GAIN_MODE_ONLY_LEF		/* gain setting only apply to lef and sef is fixed to 1x */
} ISP_SNS_GAIN_MODE_E;


typedef struct _AE_ACCURACY_S {
	AE_ACCURACY_E enAccuType;
	float f32Accuracy;
	float f32Offset;
} AE_ACCURACY_S;

typedef struct _AE_SENSOR_DEFAULT_S {
	CVI_U8 au8HistThresh[HIST_THRESH_NUM];
	CVI_U8 u8AeCompensation;

	CVI_U32 u32LinesPer500ms;
	CVI_U32 u32FlickerFreq;
	CVI_U32 u32HmaxTimes; /* unit is ns */
	CVI_U32 u32InitExposure;
	CVI_U32 u32InitAESpeed;
	CVI_U32 u32InitAETolerance;

	CVI_U32 u32FullLinesStd;
	CVI_U32 u32FullLinesMax;
	CVI_U32 u32FullLines;
	CVI_U32 u32MaxIntTime; /* RW;unit is line */
	CVI_U32 u32MinIntTime;
	CVI_U32 u32MaxIntTimeTarget;
	CVI_U32 u32MinIntTimeTarget;
	AE_ACCURACY_S stIntTimeAccu;

	CVI_U32 u32MaxAgain;
	CVI_U32 u32MinAgain;
	CVI_U32 u32MaxAgainTarget;
	CVI_U32 u32MinAgainTarget;
	AE_ACCURACY_S stAgainAccu;

	CVI_U32 u32MaxDgain;
	CVI_U32 u32MinDgain;
	CVI_U32 u32MaxDgainTarget;
	CVI_U32 u32MinDgainTarget;
	AE_ACCURACY_S stDgainAccu;

	CVI_U32 u32MaxISPDgainTarget;
	CVI_U32 u32MinISPDgainTarget;
	CVI_U32 u32ISPDgainShift;

	CVI_U32 u32MaxIntTimeStep;
	CVI_U32 u32LFMaxShortTime;
	CVI_U32 u32LFMinExposure;
#if 0
	ISP_AE_ROUTE_S stAERouteAttr;
	CVI_BOOL bAERouteExValid;
	ISP_AE_ROUTE_EX_S stAERouteAttrEx;

	CVI_U16 u16ManRatioEnable;
	CVI_U32 au32Ratio[EXP_RATIO_NUM];

	ISP_IRIS_TYPE_E  enIrisType;
	ISP_PIRIS_ATTR_S stPirisAttr;
	ISP_IRIS_F_NO_E  enMaxIrisFNO;
	ISP_IRIS_F_NO_E  enMinIrisFNO;
#endif
	ISP_AE_STRATEGY_E enAeExpMode;

	CVI_U16 u16ISOCalCoef;
	CVI_U8 u8AERunInterval;
	CVI_FLOAT f32Fps;
	CVI_FLOAT f32MinFps;
	CVI_U32 denom;
	CVI_U32 u32AEResponseFrame;
	CVI_U32 u32SnsStableFrame;	/* delay for stable statistic after sensor init. (unit: frame) */
	AE_BLC_TYPE_E enBlcType;
	ISP_SNS_GAIN_MODE_E	enWDRGainMode;
} AE_SENSOR_DEFAULT_S;

typedef struct _AE_FSWDR_ATTR_S {
	ISP_FSWDR_MODE_E enFSWDRMode;
} AE_FSWDR_ATTR_S;

typedef struct _AE_SENSOR_EXP_FUNC_S {
	CVI_S32 (*pfn_cmos_get_ae_default)(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft);

	/* the function of sensor set fps */
	CVI_S32 (*pfn_cmos_fps_set)(VI_PIPE ViPipe, CVI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft);
	CVI_S32 (*pfn_cmos_slow_framerate_set)(VI_PIPE ViPipe, CVI_U32 u32FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft);

	/* while isp notify ae to update sensor regs, ae call these funcs. */
	CVI_S32 (*pfn_cmos_inttime_update)(VI_PIPE ViPipe, CVI_U32 *u32IntTime);
	CVI_S32 (*pfn_cmos_gains_update)(VI_PIPE ViPipe, CVI_U32 *u32Again, CVI_U32 *u32Dgain);

	CVI_S32 (*pfn_cmos_again_calc_table)(VI_PIPE ViPipe, CVI_U32 *pu32AgainLin, CVI_U32 *pu32AgainDb);
	CVI_S32 (*pfn_cmos_dgain_calc_table)(VI_PIPE ViPipe, CVI_U32 *pu32DgainLin, CVI_U32 *pu32DgainDb);

	CVI_S32 (*pfn_cmos_get_inttime_max)
	(VI_PIPE ViPipe, CVI_U16 u16ManRatioEnable, CVI_U32 *au32Ratio, CVI_U32 *au32IntTimeMax,
	 CVI_U32 *au32IntTimeMin, CVI_U32 *pu32LFMaxIntTime);

	/* long frame mode set */
	CVI_S32 (*pfn_cmos_ae_fswdr_attr_set)(VI_PIPE ViPipe, AE_FSWDR_ATTR_S *pstAeFSWDRAttr);
} AE_SENSOR_EXP_FUNC_S;

typedef struct _AE_SENSOR_REGISTER_S {
	AE_SENSOR_EXP_FUNC_S stAeExp;
} AE_SENSOR_REGISTER_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_AE_COMM_H__ */
