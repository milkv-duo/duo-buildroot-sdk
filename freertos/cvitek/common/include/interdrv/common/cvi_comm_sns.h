/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_common_sns.h
 * Description:
 */

#ifndef _CVI_COMM_SNS_H_
#define _CVI_COMM_SNS_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <cvi_type.h>
#include <cvi_defines.h>
#include "cvi_debug.h"
#include "cvi_comm_isp.h"

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++

#define NOISE_PROFILE_CHANNEL_NUM 4
#define NOISE_PROFILE_LEVEL_NUM 2
#define NOISE_PROFILE_ISO_NUM 16
#define USE_USER_SEN_DRIVER 1


typedef struct _ISP_CMOS_SENSOR_IMAGE_MODE_S {
	CVI_U16 u16Width;
	CVI_U16 u16Height;
	CVI_FLOAT f32Fps;
	CVI_U8 u8SnsMode;
} ISP_CMOS_SENSOR_IMAGE_MODE_S;

typedef struct _ISP_CMOS_BLACK_LEVEL_S {
	CVI_BOOL bUpdate;
	ISP_BLACK_LEVEL_ATTR_S blcAttr;
} ISP_CMOS_BLACK_LEVEL_S;

typedef struct _ISP_SNS_ATTR_INFO_S {
	CVI_U32 eSensorId;
} ISP_SNS_ATTR_INFO_S;

typedef struct cviISP_CMOS_NOISE_CALIBRATION_S {
	CVI_FLOAT CalibrationCoef[NOISE_PROFILE_ISO_NUM][NOISE_PROFILE_CHANNEL_NUM][NOISE_PROFILE_LEVEL_NUM];
} ISP_CMOS_NOISE_CALIBRATION_S;

typedef struct _ISP_CMOS_DEFAULT_S {
	ISP_CMOS_NOISE_CALIBRATION_S stNoiseCalibration;
} ISP_CMOS_DEFAULT_S;

typedef struct _ISP_SENSOR_EXP_FUNC_S {
	CVI_VOID (*pfn_cmos_sensor_init)(VI_PIPE ViPipe);
	CVI_VOID (*pfn_cmos_sensor_exit)(VI_PIPE ViPipe);
	CVI_VOID (*pfn_cmos_sensor_global_init)(VI_PIPE ViPipe);
	CVI_S32 (*pfn_cmos_set_image_mode)(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode);
	CVI_S32 (*pfn_cmos_set_wdr_mode)(VI_PIPE ViPipe, CVI_U8 u8Mode);

	/* the algs get data which is associated with sensor, except 3a */
	CVI_S32 (*pfn_cmos_get_isp_default)(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef);
	CVI_S32 (*pfn_cmos_get_isp_black_level)(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel);
	CVI_S32 (*pfn_cmos_get_sns_reg_info)(VI_PIPE ViPipe, ISP_SNS_SYNC_INFO_S *pstSnsRegsInfo);
	CVI_S32 (*pfn_cmos_set_sns_reg_info)(VI_PIPE ViPipe, ISP_SNS_SYNC_INFO_S *pstSnsRegsInfo);

	/* the function of sensor set pixel detect */
	//CVI_VOID (*pfn_cmos_set_pixel_detect)(VI_PIPE ViPipe, bool bEnable);
} ISP_SENSOR_EXP_FUNC_S;

typedef struct bmISP_SENSOR_REGISTER_S {
	ISP_SENSOR_EXP_FUNC_S stSnsExp;
} ISP_SENSOR_REGISTER_S;

typedef enum _MCLK_FREQ_E {
	MCLK_FREQ_NONE = 0,
	MCLK_FREQ_37P125M,
	MCLK_FREQ_25M,
	MCLK_FREQ_27M,
	MCLK_FREQ_NUM
} MCLK_FREQ_E;

typedef struct _SNS_MCLK_S {
	CVI_U32		u8Cam;
	MCLK_FREQ_E	enFreq;
} SNS_MCLK_S;

// -------- If you want to change these interfaces, please contact the isp team. --------

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _CVI_COMM_SNS_H_ */
