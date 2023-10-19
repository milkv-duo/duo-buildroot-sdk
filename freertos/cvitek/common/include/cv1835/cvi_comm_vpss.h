/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_vpss.h
 * Description:
 *   The common data type defination for VPSS module.
 */

#ifndef __CVI_COMM_VPSS_H__
#define __CVI_COMM_VPSS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "cvi_type.h"
#include "cvi_common.h"
#include "cvi_errno.h"
#include "cvi_comm_video.h"

#define CVI_TRACE_VPSS(level, fmt, ...)  \
	CVI_TRACE(level, CVI_ID_VPSS, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CVI_ERR_VPSS_NULL_PTR CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define CVI_ERR_VPSS_NOTREADY CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define CVI_ERR_VPSS_INVALID_DEVID CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define CVI_ERR_VPSS_INVALID_CHNID CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define CVI_ERR_VPSS_EXIST CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define CVI_ERR_VPSS_UNEXIST CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define CVI_ERR_VPSS_NOT_SUPPORT CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define CVI_ERR_VPSS_NOT_PERM CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define CVI_ERR_VPSS_NOMEM CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define CVI_ERR_VPSS_NOBUF CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define CVI_ERR_VPSS_ILLEGAL_PARAM CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define CVI_ERR_VPSS_BUSY CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define CVI_ERR_VPSS_BUF_EMPTY CVI_DEF_ERR(CVI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)

#define VPSS_INVALID_FRMRATE -1
#define VPSS_CHN0             0
#define VPSS_CHN1             1
#define VPSS_CHN2             2
#define VPSS_CHN3             3
#define VPSS_INVALID_CHN     -1
#define VPSS_INVALID_GRP     -1

/*
 * VPSS_CROP_RATIO_COOR: Ratio coordinate.
 * VPSS_CROP_ABS_COOR: Absolute coordinate.
 */
typedef enum _VPSS_CROP_COORDINATE_E {
	VPSS_CROP_RATIO_COOR = 0,
	VPSS_CROP_ABS_COOR,
} VPSS_CROP_COORDINATE_E;

typedef enum _VPSS_ROUNDING_E {
	VPSS_ROUNDING_TO_EVEN = 0,
	VPSS_ROUNDING_AWAY_FROM_ZERO,
	VPSS_ROUNDING_TRUNCATE,
	VPSS_ROUNDING_MAX,
} VPSS_ROUNDING_E;

/*
 * bEnable: Whether Normalize is enabled.
 * factor: scaling factors for 3 planes.
 * mean: minus means for 3 planes.
 */
typedef struct _VPSS_NORMALIZE_S {
	CVI_BOOL bEnable;
	CVI_FLOAT factor[3];
	CVI_FLOAT mean[3];
	VPSS_ROUNDING_E rounding;
} VPSS_NORMALIZE_S;

/*
 * u32MaxW: Range: Width of source image.
 * u32MaxH: Range: Height of source image.
 * enPixelFormat: Pixel format of target image.
 * stFrameRate: Frame rate control info.
 * u8VpssDev: Only meaningful if VPSS_MODE_DUAL.
 */
typedef struct _VPSS_GRP_ATTR_S {
	CVI_U32 u32MaxW;
	CVI_U32 u32MaxH;
	PIXEL_FORMAT_E enPixelFormat;
	FRAME_RATE_CTRL_S stFrameRate;
	CVI_U8 u8VpssDev;
} VPSS_GRP_ATTR_S;

/*
 * u32Width: Width of target image.
 * u32Height: Height of target image.
 * enVideoFormat: Video format of target image.
 * enPixelFormat: Pixel format of target image.
 * stFrameRate: Frame rate control info.
 * bMirror: Mirror enable.
 * bFlip: Flip enable.
 * u32Depth: User get list depth.
 * stAspectRatio: Aspect Ratio info.
 */
typedef struct _VPSS_CHN_ATTR_S {
	CVI_U32 u32Width;
	CVI_U32 u32Height;
	VIDEO_FORMAT_E enVideoFormat;
	PIXEL_FORMAT_E enPixelFormat;
	FRAME_RATE_CTRL_S stFrameRate;
	CVI_BOOL bMirror;
	CVI_BOOL bFlip;
	CVI_U32 u32Depth;
	ASPECT_RATIO_S stAspectRatio;
	VPSS_NORMALIZE_S stNormalize;
} VPSS_CHN_ATTR_S;

/*
 * bEnable: RW; CROP enable.
 * enCropCoordinate: RW; Coordinate mode of the crop start point.
 * stCropRect: CROP rectangle.
 */
typedef struct _VPSS_CROP_INFO_S {
	CVI_BOOL bEnable;
	VPSS_CROP_COORDINATE_E enCropCoordinate;
	RECT_S stCropRect;
} VPSS_CROP_INFO_S;

/*
 * bEnable: Whether LDC is enbale.
 * stAttr: LDC Attribute.
 */
typedef struct _VPSS_LDC_ATTR_S {
	CVI_BOOL bEnable;
	LDC_ATTR_S stAttr;
} VPSS_LDC_ATTR_S;

typedef struct _VPSS_PARAM_MOD_S {
	CVI_U32 u32VpssVbSource;
	CVI_U32 u32VpssSplitNodeNum;
} VPSS_MOD_PARAM_S;

typedef enum _VPSS_SCALE_COEF_E {
	VPSS_SCALE_COEF_BICUBIC = 0,
	VPSS_SCALE_COEF_BILINEAR,
	VPSS_SCALE_COEF_NEAREST,
	VPSS_SCALE_COEF_OPENCV_BILINEAR,
	VPSS_SCALE_COEF_MAX,
} VPSS_SCALE_COEF_E;

typedef struct _VPSS_GRP_WORK_STATUS_S {
	CVI_U32 u32RecvCnt;
	CVI_U32 u32LostCnt;
	CVI_U32 u32StartFailCnt; //start job fail cnt
	CVI_U32 u32CostTime; // current job cost time in us
	CVI_U32 u32MaxCostTime;
} VPSS_GRP_WORK_STATUS_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __CVI_COMM_VPSS_H__ */
