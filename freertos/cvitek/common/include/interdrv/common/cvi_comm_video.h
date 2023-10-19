/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_video.h
 * Description:
 *   Common video definitions.
 */

#ifndef __CVI_COMM_VIDEO_H__
#define __CVI_COMM_VIDEO_H__

#include <cvi_type.h>
#include <cvi_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SRC_LENS_COEF_SEG 2
#define DST_LENS_COEF_SEG 3
#define SRC_LENS_COEF_NUM 4
#define DST_LENS_COEF_NUM 4

#define ISP_BAYER_CHN (4)

#define IS_FMT_YUV(fmt) \
	((fmt == PIXEL_FORMAT_YUV_PLANAR_420) || (fmt == PIXEL_FORMAT_YUV_PLANAR_422) || \
	 (fmt == PIXEL_FORMAT_YUV_PLANAR_444) || (fmt == PIXEL_FORMAT_YUV_400) || \
	 (fmt == PIXEL_FORMAT_NV12) || (fmt == PIXEL_FORMAT_NV21) || \
	 (fmt == PIXEL_FORMAT_NV16) || (fmt == PIXEL_FORMAT_NV61) || \
	 (fmt == PIXEL_FORMAT_YUYV) || (fmt == PIXEL_FORMAT_UYVY) || \
	 (fmt == PIXEL_FORMAT_YVYU) || (fmt == PIXEL_FORMAT_VYUY))

#define IS_FMT_YUV420(fmt) \
	((fmt == PIXEL_FORMAT_YUV_PLANAR_420) || \
	 (fmt == PIXEL_FORMAT_NV12) || (fmt == PIXEL_FORMAT_NV21) || \
	 (fmt == PIXEL_FORMAT_YUYV) || (fmt == PIXEL_FORMAT_UYVY) || \
	 (fmt == PIXEL_FORMAT_YVYU) || (fmt == PIXEL_FORMAT_VYUY))

#define IS_FMT_YUV422(fmt) \
	((fmt == PIXEL_FORMAT_YUV_PLANAR_422) || \
	 (fmt == PIXEL_FORMAT_NV16) || (fmt == PIXEL_FORMAT_NV61))

#define IS_FRAME_OFFSET_INVALID(f) \
	((f).s16OffsetLeft < 0 || (f).s16OffsetRight < 0 || \
	 (f).s16OffsetTop < 0 || (f).s16OffsetBottom < 0 || \
	 ((CVI_U32)((f).s16OffsetLeft + (f).s16OffsetRight) > (f).u32Width) || \
	 ((CVI_U32)((f).s16OffsetTop + (f).s16OffsetBottom) > (f).u32Height))

typedef enum _OPERATION_MODE_E {
	OPERATION_MODE_AUTO = 0,
	OPERATION_MODE_MANUAL = 1,
	OPERATION_MODE_BUTT
} OPERATION_MODE_E;

/*Angle of rotation*/
typedef enum _ROTATION_E {
	ROTATION_0 = 0,
	ROTATION_90,
	ROTATION_180,
	ROTATION_270,
	ROTATION_MAX
} ROTATION_E;

typedef enum _VB_SOURCE_E {
	VB_SOURCE_COMMON = 0,
	VB_SOURCE_MODULE = 1,
	VB_SOURCE_PRIVATE = 2,
	VB_SOURCE_USER = 3,
	VB_SOURCE_BUTT
} VB_SOURCE_E;

typedef struct _BORDER_S {
	CVI_U32 u32TopWidth;
	CVI_U32 u32BottomWidth;
	CVI_U32 u32LeftWidth;
	CVI_U32 u32RightWidth;
	CVI_U32 u32Color;
} BORDER_S;

typedef struct _POINT_S {
	CVI_S32 s32X;
	CVI_S32 s32Y;
} POINT_S;

typedef struct _SIZE_S {
	CVI_U32 u32Width;
	CVI_U32 u32Height;
} SIZE_S;

/*
 * u32Width  : total pixels from snr
 * u32Height : total lines  from snr
 * u32StartX : crop x
 * u32StartY : crop y
 * u32ActiveW : actual video pixels
 * u32ActiveH : actual video lines
 */
typedef struct _SNR_SIZE_S {
	CVI_U32 u32Width;
	CVI_U32 u32Height;
	CVI_U32 u32StartX;
	CVI_U32 u32StartY;
	CVI_U32 u32ActiveW;
	CVI_U32 u32ActiveH;
} SNR_SIZE_S;

typedef struct _RECT_S {
	CVI_S32 s32X;
	CVI_S32 s32Y;
	CVI_U32 u32Width;
	CVI_U32 u32Height;
} RECT_S;

typedef struct _VIDEO_REGION_INFO_S {
	CVI_U32 u32RegionNum; /* W; count of the region */
	RECT_S *pstRegion; /* W; region attribute */
} VIDEO_REGION_INFO_S;

typedef struct _CROP_INFO_S {
	CVI_BOOL bEnable;
	RECT_S stRect;
} CROP_INFO_S;

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef struct _FRAME_RATE_CTRL_S {
	CVI_S32 s32SrcFrameRate; /* RW; source frame rate */
	CVI_S32 s32DstFrameRate; /* RW; dest frame rate */
} FRAME_RATE_CTRL_S;
// -------- If you want to change these interfaces, please contact the isp team. --------

/*
 * ASPECT_RATIO_NONE: full screen
 * ASPECT_RATIO_AUTO: Keep ratio, automatically get the region of video.
 * ASPECT_RATIO_MANUAL: Manully set the region of video.
 */
typedef enum _ASPECT_RATIO_E {
	ASPECT_RATIO_NONE = 0,
	ASPECT_RATIO_AUTO,
	ASPECT_RATIO_MANUAL,
	ASPECT_RATIO_MAX
} ASPECT_RATIO_E;

/*
 * enMode: aspect ratio mode: none/auto/manual
 * bEnableBgColor: fill bgcolor
 * u32BgColor: background color, RGB 888
 * stVideoRect: valid in ASPECT_RATIO_MANUAL mode
 */
typedef struct _ASPECT_RATIO_S {
	ASPECT_RATIO_E enMode;
	CVI_BOOL bEnableBgColor;
	CVI_U32 u32BgColor;
	RECT_S stVideoRect;
} ASPECT_RATIO_S;

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _PIXEL_FORMAT_E {
	PIXEL_FORMAT_RGB_888 = 0,
	PIXEL_FORMAT_BGR_888,
	PIXEL_FORMAT_RGB_888_PLANAR,
	PIXEL_FORMAT_BGR_888_PLANAR,

	PIXEL_FORMAT_ARGB_1555, // 4,
	PIXEL_FORMAT_ARGB_4444,
	PIXEL_FORMAT_ARGB_8888,

	PIXEL_FORMAT_RGB_BAYER_8BPP, // 7,
	PIXEL_FORMAT_RGB_BAYER_10BPP,
	PIXEL_FORMAT_RGB_BAYER_12BPP,
	PIXEL_FORMAT_RGB_BAYER_14BPP,
	PIXEL_FORMAT_RGB_BAYER_16BPP,

	PIXEL_FORMAT_YUV_PLANAR_422, // 12,
	PIXEL_FORMAT_YUV_PLANAR_420,
	PIXEL_FORMAT_YUV_PLANAR_444,
	PIXEL_FORMAT_YUV_400,

	PIXEL_FORMAT_HSV_888, // 16,
	PIXEL_FORMAT_HSV_888_PLANAR,

	PIXEL_FORMAT_NV12, // 18,
	PIXEL_FORMAT_NV21,
	PIXEL_FORMAT_NV16,
	PIXEL_FORMAT_NV61,
	PIXEL_FORMAT_YUYV,
	PIXEL_FORMAT_UYVY,
	PIXEL_FORMAT_YVYU,
	PIXEL_FORMAT_VYUY,

	PIXEL_FORMAT_FP32_C1 = 32, // 32
	PIXEL_FORMAT_FP32_C3_PLANAR,
	PIXEL_FORMAT_INT32_C1,
	PIXEL_FORMAT_INT32_C3_PLANAR,
	PIXEL_FORMAT_UINT32_C1,
	PIXEL_FORMAT_UINT32_C3_PLANAR,
	PIXEL_FORMAT_BF16_C1,
	PIXEL_FORMAT_BF16_C3_PLANAR,
	PIXEL_FORMAT_INT16_C1,
	PIXEL_FORMAT_INT16_C3_PLANAR,
	PIXEL_FORMAT_UINT16_C1,
	PIXEL_FORMAT_UINT16_C3_PLANAR,
	PIXEL_FORMAT_INT8_C1,
	PIXEL_FORMAT_INT8_C3_PLANAR,
	PIXEL_FORMAT_UINT8_C1,
	PIXEL_FORMAT_UINT8_C3_PLANAR,

	PIXEL_FORMAT_8BIT_MODE = 48, //48

	PIXEL_FORMAT_MAX
} PIXEL_FORMAT_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

/*
 * VIDEO_FORMAT_LINEAR: nature video line.
 */
// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _VIDEO_FORMAT_E {
	VIDEO_FORMAT_LINEAR = 0,
	VIDEO_FORMAT_MAX
} VIDEO_FORMAT_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

/*
 * COMPRESS_MODE_NONE: no compress.
 * COMPRESS_MODE_TILE: compress unit is a tile.
 * COMPRESS_MODE_LINE: compress unit is the whole line.
 * COMPRESS_MODE_FRAME: ompress unit is the whole frame.
 */
// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _COMPRESS_MODE_E {
	COMPRESS_MODE_NONE = 0,
	COMPRESS_MODE_TILE,
	COMPRESS_MODE_LINE,
	COMPRESS_MODE_FRAME,
	COMPRESS_MODE_BUTT
} COMPRESS_MODE_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _BAYER_FORMAT_E {
	BAYER_FORMAT_BG = 0,
	BAYER_FORMAT_GB,
	BAYER_FORMAT_GR,
	BAYER_FORMAT_RG,
	BAYER_FORMAT_MAX
} BAYER_FORMAT_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

typedef enum _VIDEO_DISPLAY_MODE_E {
	VIDEO_DISPLAY_MODE_PREVIEW = 0x0,
	VIDEO_DISPLAY_MODE_PLAYBACK = 0x1,

	VIDEO_DISPLAY_MODE_MAX
} VIDEO_DISPLAY_MODE_E;

/*
 * u32ISO:  ISP internal ISO : Again*Dgain*ISPgain
 * u32ExposureTime:  Exposure time (reciprocal of shutter speed),unit is us
 * u32FNumber: The actual F-number (F-stop) of lens when the image was taken
 * u32SensorID: which sensor is used
 * u32HmaxTimes: Sensor HmaxTimes,unit is ns
 * u32VcNum: when dump wdr frame, which is long or short  exposure frame.
 */
// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef struct _ISP_FRAME_INFO_S {
	CVI_U32 u32ISO;
	CVI_U32 u32ExposureTime;
	CVI_U32 u32IspDgain;
	CVI_U32 u32Again;
	CVI_U32 u32Dgain;
	CVI_U32 au32Ratio[3];
	CVI_U32 u32IspNrStrength;
	CVI_U32 u32FNumber;
	CVI_U32 u32SensorID;
	CVI_U32 u32SensorMode;
	CVI_U32 u32HmaxTimes;
	CVI_U32 u32VcNum;
} ISP_FRAME_INFO_S;
// -------- If you want to change these interfaces, please contact the isp team. --------

typedef struct _ISP_HDR_INFO_S {
	CVI_U32 u32ColorTemp;
	CVI_U16 au16CCM[9];
	CVI_U8 u8Saturation;
} ISP_HDR_INFO_S;

typedef struct _ISP_ATTACH_INFO_S {
	ISP_HDR_INFO_S stIspHdr;
	CVI_U32 u32ISO;
	CVI_U8 *u8SnsWDRMode;
} ISP_ATTACH_INFO_S;

typedef enum _FRAME_FLAG_E {
	FRAME_FLAG_SNAP_FLASH = 0x1 << 0,
	FRAME_FLAG_SNAP_CUR = 0x1 << 1,
	FRAME_FLAG_SNAP_REF = 0x1 << 2,
	FRAME_FLAG_SNAP_END = 0x1 << 31,
	FRAME_FLAG_MAX
} FRAME_FLAG_E;

/* RGGB=4 */
#define ISP_WB_GAIN_NUM 4
/* 3*3=9 matrix */
#define ISP_CAP_CCM_NUM 9
typedef struct _ISP_CONFIG_INFO_S {
	CVI_U32 u32ISO;
	CVI_U32 u32IspDgain;
	CVI_U32 u32ExposureTime;
	CVI_U32 au32WhiteBalanceGain[ISP_WB_GAIN_NUM];
	CVI_U32 u32ColorTemperature;
	CVI_U16 au16CapCCM[ISP_CAP_CCM_NUM];
} ISP_CONFIG_INFO_S;

/*
 * pJpegDCFVirAddr: JPEG_DCF_S, used in JPEG DCF
 * pIspInfoVirAddr: ISP_FRAME_INFO_S, used in ISP debug, when get raw and send raw
 * pLowDelayVirAddr: used in low delay
 */
typedef struct _VIDEO_SUPPLEMENT_S {
	CVI_U64 u64JpegDCFPhyAddr;
	CVI_U64 u64IspInfoPhyAddr;
	CVI_U64 u64LowDelayPhyAddr;
	CVI_U64 u64FrameDNGPhyAddr;

	CVI_VOID * ATTRIBUTE pJpegDCFVirAddr;
	CVI_VOID * ATTRIBUTE pIspInfoVirAddr;
	CVI_VOID * ATTRIBUTE pLowDelayVirAddr;
	CVI_VOID * ATTRIBUTE pFrameDNGVirAddr;
} VIDEO_SUPPLEMENT_S;

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _COLOR_GAMUT_E {
	COLOR_GAMUT_BT601 = 0,
	COLOR_GAMUT_BT709,
	COLOR_GAMUT_BT2020,
	COLOR_GAMUT_USER,
	COLOR_GAMUT_MAX
} COLOR_GAMUT_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

typedef struct _ISP_COLORGAMMUT_INFO_S {
	COLOR_GAMUT_E enColorGamut;
} ISP_COLORGAMMUT_INFO_S;

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _DYNAMIC_RANGE_E {
	DYNAMIC_RANGE_SDR8 = 0,
	DYNAMIC_RANGE_SDR10,
	DYNAMIC_RANGE_HDR10,
	DYNAMIC_RANGE_HLG,
	DYNAMIC_RANGE_SLF,
	DYNAMIC_RANGE_XDR,
	DYNAMIC_RANGE_MAX
} DYNAMIC_RANGE_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _DATA_BITWIDTH_E {
	DATA_BITWIDTH_8 = 0,
	DATA_BITWIDTH_10,
	DATA_BITWIDTH_12,
	DATA_BITWIDTH_14,
	DATA_BITWIDTH_16,
	DATA_BITWIDTH_MAX
} DATA_BITWIDTH_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

/**
 * @brief Define video frame
 *
 * s16OffsetTop: top offset of show area
 * s16OffsetBottom: bottom offset of show area
 * s16OffsetLeft: left offset of show area
 * s16OffsetRight: right offset of show area
 * u32FrameFlag: FRAME_FLAG_E, can be OR operation.
 */
// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef struct _VIDEO_FRAME_S {
	CVI_U32 u32Width;
	CVI_U32 u32Height;
	PIXEL_FORMAT_E enPixelFormat;
	BAYER_FORMAT_E enBayerFormat;
	VIDEO_FORMAT_E enVideoFormat;
	COMPRESS_MODE_E enCompressMode;
	DYNAMIC_RANGE_E enDynamicRange;
	COLOR_GAMUT_E enColorGamut;
	CVI_U32 u32Stride[3];

	CVI_U64 u64PhyAddr[3];
	CVI_U8 *pu8VirAddr[3];
#ifdef __arm__
	__u32 u32VirAddrPadding[3];
#endif
	CVI_U32 u32Length[3];

	CVI_S16 s16OffsetTop;
	CVI_S16 s16OffsetBottom;
	CVI_S16 s16OffsetLeft;
	CVI_S16 s16OffsetRight;

	CVI_U32 u32TimeRef;
	CVI_U64 u64PTS;

	void *pPrivateData;
#ifdef __arm__
	__u32 u32PrivateDataPadding;
#endif
	CVI_U32 u32FrameFlag;

	CVI_U32 u32DevNum;
} VIDEO_FRAME_S;
// -------- If you want to change these interfaces, please contact the isp team. --------

/**
 * @brief Define the information of video frame.
 *
 * stVFrame: Video frame info.
 * u32PoolId: VB pool ID.
 */
// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef struct _VIDEO_FRAME_INFO_S {
	VIDEO_FRAME_S stVFrame;
	CVI_U32 u32PoolId;
} VIDEO_FRAME_INFO_S;
// -------- If you want to change these interfaces, please contact the isp team. --------

/*
 * u32VBSize: size of VB needed.
 * u32MainStride: stride of planar0.
 * u32CStride: stride of planar1/2 if there is.
 * u32MainSize: size of all planars.
 * u32MainYSize: size of planar0.
 * u32MainCSize: size of planar1/2 if there is.
 * u16AddrAlign: address alignment needed between planar.
 */
typedef struct _VB_CAL_CONFIG_S {
	CVI_U32 u32VBSize;

	CVI_U32 u32MainStride;
	CVI_U32 u32CStride;
	CVI_U32 u32MainSize;
	CVI_U32 u32MainYSize;
	CVI_U32 u32MainCSize;
	CVI_U16 u16AddrAlign;
	CVI_U8  plane_num;
} VB_CAL_CONFIG_S;

/*
 * enPixelFormat: Bitmap's pixel format
 * u32Width: Bitmap's width
 * u32Height: Bitmap's height
 * pData: Address of Bitmap's data
 */
typedef struct _BITMAP_S {
	PIXEL_FORMAT_E enPixelFormat;
	CVI_U32 u32Width;
	CVI_U32 u32Height;

	CVI_VOID * ATTRIBUTE pData;
} BITMAP_S;

/*
 *
 * s32CenterXOffset: RW; Range: [-511, 511], horizontal offset of the image distortion center relative to image center
 * s32CenterYOffset: RW; Range: [-511, 511], vertical offset of the image distortion center relative to image center
 * s32DistortionRatio: RW; Range: [-300, 500], LDC Distortion ratio.
 *		    When spread on,s32DistortionRatio range should be [0, 500]
 */
// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef struct _LDC_ATTR_S {
	CVI_BOOL bAspect; /* RW;Whether aspect ration  is keep */
	CVI_S32 s32XRatio; /* RW; Range: [0, 100], field angle ration of  horizontal,valid when bAspect=0.*/
	CVI_S32 s32YRatio; /* RW; Range: [0, 100], field angle ration of  vertical,valid when bAspect=0.*/
	CVI_S32 s32XYRatio; /* RW; Range: [0, 100], field angle ration of  all,valid when bAspect=1.*/
	CVI_S32 s32CenterXOffset;
	CVI_S32 s32CenterYOffset;
	CVI_S32 s32DistortionRatio;
} LDC_ATTR_S;
// -------- If you want to change these interfaces, please contact the isp team. --------

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _WDR_MODE_E {
	WDR_MODE_NONE = 0,
	WDR_MODE_BUILT_IN,
	WDR_MODE_QUDRA,

	WDR_MODE_2To1_LINE,
	WDR_MODE_2To1_FRAME,
	WDR_MODE_2To1_FRAME_FULL_RATE,

	WDR_MODE_3To1_LINE,
	WDR_MODE_3To1_FRAME,
	WDR_MODE_3To1_FRAME_FULL_RATE,

	WDR_MODE_4To1_LINE,
	WDR_MODE_4To1_FRAME,
	WDR_MODE_4To1_FRAME_FULL_RATE,

	WDR_MODE_MAX,
} WDR_MODE_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
typedef enum _PROC_AMP_E {
	PROC_AMP_BRIGHTNESS = 0,
	PROC_AMP_CONTRAST,
	PROC_AMP_SATURATION,
	PROC_AMP_HUE,
	PROC_AMP_MAX,
} PROC_AMP_E;
// -------- If you want to change these interfaces, please contact the isp team. --------

typedef struct _PROC_AMP_CTRL_S {
	CVI_S32 minimum;
	CVI_S32 maximum;
	CVI_S32 step;
	CVI_S32 default_value;
} PROC_AMP_CTRL_S;

typedef struct _VCODEC_PERF_FPS_S {
	CVI_U32 u32InFPS;
	CVI_U32 u32OutFPS;
	CVI_U64 u64HwTime;
} VCODEC_PERF_FPS_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _CVI_COMM_VIDEO_H_ */
