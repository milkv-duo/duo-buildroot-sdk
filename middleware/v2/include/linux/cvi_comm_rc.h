/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_rc.h
 * Description:
 *   Common rate control definitions.
 */

#ifndef __CVI_COMM_RC_H__
#define __CVI_COMM_RC_H__

#include <linux/cvi_defines.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef unsigned int CVI_FR32;

/* rc mode */
typedef enum _VENC_RC_MODE_E {
	VENC_RC_MODE_H264CBR = 1,
	VENC_RC_MODE_H264VBR,
	VENC_RC_MODE_H264AVBR,
	VENC_RC_MODE_H264QVBR,
	VENC_RC_MODE_H264FIXQP,
	VENC_RC_MODE_H264QPMAP,
	VENC_RC_MODE_H264UBR,

	VENC_RC_MODE_MJPEGCBR,
	VENC_RC_MODE_MJPEGVBR,
	VENC_RC_MODE_MJPEGFIXQP,

	VENC_RC_MODE_H265CBR,
	VENC_RC_MODE_H265VBR,
	VENC_RC_MODE_H265AVBR,
	VENC_RC_MODE_H265QVBR,
	VENC_RC_MODE_H265FIXQP,
	VENC_RC_MODE_H265QPMAP,
	VENC_RC_MODE_H265UBR,

	VENC_RC_MODE_BUTT,

} VENC_RC_MODE_E;

/* qpmap mode*/
typedef enum _VENC_RC_QPMAP_MODE_E {
	VENC_RC_QPMAP_MODE_MEANQP = 0,
	VENC_RC_QPMAP_MODE_MINQP,
	VENC_RC_QPMAP_MODE_MAXQP,

	VENC_RC_QPMAP_MODE_BUTT,
} VENC_RC_QPMAP_MODE_E;

/* the attribute of h264e fixqp*/
typedef struct _VENC_H264_FIXQP_S {
	CVI_U32 u32Gop; /* RW; Range:[1, 65536]; the interval of ISLICE. */
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	CVI_U32 u32IQp; ///< qp of the I frame, Range:[0, 51]
	CVI_U32 u32PQp; ///< qp of the P frame, Range:[0, 51]
	CVI_U32 u32BQp; ///< Not support
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_H264_FIXQP_S;

/* the attribute of h264e cbr*/
typedef struct _VENC_H264_CBR_S {
	CVI_U32 u32Gop; /* RW; Range:[1, 65536]; the interval of I Frame. */
	CVI_U32 u32StatTime; /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	CVI_U32 u32BitRate; /* RW; Range:[2, 409600]; average bitrate */
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_H264_CBR_S;

/* the attribute of h264e vbr*/
typedef struct _VENC_H264_VBR_S {
	CVI_U32 u32Gop; /* RW; Range:[1, 65536]; the interval of ISLICE. */
	CVI_U32 u32StatTime; /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	CVI_U32 u32MaxBitRate; /* RW; Range:[2, 409600];the max bitrate */
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_H264_VBR_S;

/* the attribute of h264e avbr*/
typedef struct _VENC_H264_AVBR_S {
	CVI_U32 u32Gop; /* RW; Range:[1, 65536]; the interval of ISLICE. */
	CVI_U32 u32StatTime; /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	CVI_U32 u32MaxBitRate; /* RW; Range:[2, 409600];the max bitrate */
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_H264_AVBR_S;

/* the attribute of h264e qpmap*/
typedef struct _VENC_H264_QPMAP_S {
	CVI_U32 u32Gop; /* RW; Range:[1, 65536]; the interval of ISLICE. */
	CVI_U32 u32StatTime; /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_H264_QPMAP_S;

typedef struct _VENC_H264_QVBR_S {
	CVI_U32 u32Gop; /*the interval of ISLICE. */
	CVI_U32 u32StatTime; /* the rate statistic time, the unit is senconds(s) */
	CVI_U32 u32SrcFrameRate; /* the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* the target frame rate of the venc channel */
	CVI_U32 u32TargetBitRate; /* the target bitrate */
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_H264_QVBR_S;

/* the attribute of h264e ubr*/
typedef struct _VENC_H264_UBR_S {
	CVI_U32 u32Gop; /* RW; Range:[1, 65536]; the interval of I Frame. */
	CVI_U32 u32StatTime; /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	CVI_U32 u32BitRate; /* RW; Range:[2, 409600]; average bitrate */
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_H264_UBR_S;

/* the attribute of h265e qpmap*/
typedef struct _VENC_H265_QPMAP_S {
	CVI_U32 u32Gop; /* RW; Range:[1, 65536]; the interval of ISLICE. */
	CVI_U32 u32StatTime; /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	VENC_RC_QPMAP_MODE_E enQpMapMode; /* RW;  the QpMap Mode.*/
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_H265_QPMAP_S;

typedef struct _VENC_H264_CBR_S VENC_H265_CBR_S;
typedef struct _VENC_H264_VBR_S VENC_H265_VBR_S;
typedef struct _VENC_H264_AVBR_S VENC_H265_AVBR_S;
typedef struct _VENC_H264_FIXQP_S VENC_H265_FIXQP_S;
typedef struct _VENC_H264_QVBR_S VENC_H265_QVBR_S;
typedef struct _VENC_H264_UBR_S VENC_H265_UBR_S;

/* the attribute of mjpege fixqp*/
typedef struct _VENC_MJPEG_FIXQP_S {
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	CVI_U32 u32Qfactor; /* RW; Range:[0,99];image quality. */
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_MJPEG_FIXQP_S;

/* the attribute of mjpege cbr*/
typedef struct _VENC_MJPEG_CBR_S {
	CVI_U32 u32StatTime; /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	CVI_U32 u32BitRate; /* RW; Range:[2, 409600]; average bitrate */
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_MJPEG_CBR_S;

/* the attribute of mjpege vbr*/
typedef struct _VENC_MJPEG_VBR_S {
	CVI_U32 u32StatTime; /* RW; Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
	CVI_U32 u32SrcFrameRate; /* RW; Range:[1, 240]; the input frame rate of the venc channel */
	CVI_FR32 fr32DstFrameRate; /* RW; Range:[0.015625, u32SrcFrmRate]; the target frame rate of the venc channel */
	CVI_U32 u32MaxBitRate; /* RW; Range:[2, 409600];the max bitrate */
	CVI_BOOL bVariFpsEn; /* RW; Range:[0, 1]; enable variable framerate */
} VENC_MJPEG_VBR_S;

/* the attribute of rc*/
typedef struct _VENC_RC_ATTR_S {
	VENC_RC_MODE_E enRcMode; /* RW; the type of rc*/
	union {
		VENC_H264_CBR_S stH264Cbr;
		VENC_H264_VBR_S stH264Vbr;
		VENC_H264_AVBR_S stH264AVbr;
		VENC_H264_QVBR_S stH264QVbr;
		VENC_H264_FIXQP_S stH264FixQp;
		VENC_H264_QPMAP_S stH264QpMap;
		VENC_H264_UBR_S stH264Ubr;

		VENC_MJPEG_CBR_S stMjpegCbr;
		VENC_MJPEG_VBR_S stMjpegVbr;
		VENC_MJPEG_FIXQP_S stMjpegFixQp;

		VENC_H265_CBR_S stH265Cbr;
		VENC_H265_VBR_S stH265Vbr;
		VENC_H265_AVBR_S stH265AVbr;
		VENC_H265_QVBR_S stH265QVbr;
		VENC_H265_FIXQP_S stH265FixQp;
		VENC_H265_QPMAP_S stH265QpMap;
		VENC_H265_UBR_S stH265Ubr;
	};
} VENC_RC_ATTR_S;

/*the super frame mode*/
typedef enum _VENC_SUPERFRM_MODE_E {
	SUPERFRM_NONE = 0, /* sdk don't care super frame */
	SUPERFRM_DISCARD, /* the super frame is discarded */
	SUPERFRM_REENCODE, /* the super frame is re-encode */
	SUPERFRM_REENCODE_IDR, /* the super frame is re-encode to IDR */
	SUPERFRM_BUTT
} VENC_SUPERFRM_MODE_E;

/* The param of H264e cbr*/
typedef struct _VENC_PARAM_H264_CBR_S {
	CVI_U32 u32MinIprop; /* RW; Range:[1, 100]; the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* RW; Range:(u32MinIprop, 100]; the max ratio of i frame and p frame */
	CVI_U32 u32MaxQp; /* RW; Range:(u32MinQp, 51];the max QP value */
	CVI_U32 u32MinQp; /* RW; Range:[0, 51]; the min QP value */
	CVI_U32 u32MaxIQp; /* RW; Range:(u32MinIQp, 51]; max qp for i frame */
	CVI_U32 u32MinIQp; /* RW; Range:[0, 51]; min qp for i frame */
	CVI_S32 s32MaxReEncodeTimes; /* RW; Range:[0, 3]; Range:max number of re-encode times.*/
	CVI_BOOL bQpMapEn; /* RW; Range:[0, 1]; enable qpmap.*/
} VENC_PARAM_H264_CBR_S;

/* The param of H264e vbr*/
typedef struct _VENC_PARAM_H264_VBR_S {
	CVI_S32 s32ChangePos;
	// RW; Range:[50, 100]; Indicates the ratio of the current bit rate to the maximum
	// bit rate when the QP value starts to be adjusted
	CVI_U32 u32MinIprop; /* RW; Range:[1, 100] ; the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* RW; Range:(u32MinIprop, 100] ; the max ratio of i frame and p frame */
	CVI_S32 s32MaxReEncodeTimes; /* RW; Range:[0, 3]; max number of re-encode times */
	CVI_BOOL bQpMapEn;

	CVI_U32 u32MaxQp; /* RW; Range:(u32MinQp, 51]; the max P B qp */
	CVI_U32 u32MinQp; /* RW; Range:[0, 51]; the min P B qp */
	CVI_U32 u32MaxIQp; /* RW; Range:(u32MinIQp, 51]; the max I qp */
	CVI_U32 u32MinIQp; /* RW; Range:[0, 51]; the min I qp */
} VENC_PARAM_H264_VBR_S;

/* The param of H264e avbr*/
typedef struct _VENC_PARAM_H264_AVBR_S {
	CVI_S32 s32ChangePos;
	// RW; Range:[50, 100]; Indicates the ratio of the current bit rate to the maximum
	// bit rate when the QP value starts to be adjusted
	CVI_U32 u32MinIprop; /* RW; Range:[1, 100] ; the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* RW; Range:(u32MinIprop, 100] ; the max ratio of i frame and p frame */
	CVI_S32 s32MaxReEncodeTimes; /* RW; Range:[0, 3]; max number of re-encode times */
	CVI_BOOL bQpMapEn;

	CVI_S32 s32MinStillPercent; /* RW; Range:[5, 100]; the min percent of target bitrate for still scene */
	CVI_U32 u32MaxStillQP; /* RW; Range:[u32MinIQp, u32MaxIQp]; the max QP value of I frame for still scene*/
	CVI_U32 u32MinStillPSNR; /* RW; reserved,Invalid member currently */

	CVI_U32 u32MaxQp; /* RW; Range:(u32MinQp, 51]; the max P B qp */
	CVI_U32 u32MinQp; /* RW; Range:[0, 51]; the min P B qp */
	CVI_U32 u32MaxIQp; /* RW; Range:(u32MinIQp, 51]; the max I qp */
	CVI_U32 u32MinIQp; /* RW; Range:[0, 51]; the min I qp */
	CVI_U32 u32MinQpDelta;
	// Difference between FrameLevelMinQp & u32MinQp, FrameLevelMinQp = u32MinQp(or u32MinIQp) + MinQpDelta

	CVI_U32 u32MotionSensitivity; /* RW; Range:[0, 100]; Motion Sensitivity */
	CVI_S32	s32AvbrFrmLostOpen; /* RW; Range:[0, 1]; Open Frame Lost */
	CVI_S32 s32AvbrFrmGap; /* RW; Range:[0, 100]; Maximim Gap of Frame Lost */
	CVI_S32 s32AvbrPureStillThr;
} VENC_PARAM_H264_AVBR_S;

typedef struct _VENC_PARAM_H264_QVBR_S {
	CVI_U32 u32MinIprop; /* the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* the max ratio of i frame and p frame */
	CVI_S32 s32MaxReEncodeTimes; /* max number of re-encode times [0, 3]*/
	CVI_BOOL bQpMapEn;

	CVI_U32 u32MaxQp; /* the max P B qp */
	CVI_U32 u32MinQp; /* the min P B qp */
	CVI_U32 u32MaxIQp; /* the max I qp */
	CVI_U32 u32MinIQp; /* the min I qp */

	CVI_S32 s32BitPercentUL; /*Indicate the ratio of bitrate  upper limit*/
	CVI_S32 s32BitPercentLL; /*Indicate the ratio of bitrate  lower limit*/
	CVI_S32 s32PsnrFluctuateUL; /*Reduce the target bitrate when the value of psnr approch the upper limit*/
	CVI_S32 s32PsnrFluctuateLL; /*Increase the target bitrate when the value of psnr approch the lower limit */
} VENC_PARAM_H264_QVBR_S;

/* The param of H264e ubr */
typedef struct _VENC_PARAM_H264_UBR_S {
	CVI_U32 u32MinIprop; /* RW; Range:[1, 100]; the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* RW; Range:(u32MinIprop, 100]; the max ratio of i frame and p frame */
	CVI_U32 u32MaxQp; /* RW; Range:(u32MinQp, 51];the max QP value */
	CVI_U32 u32MinQp; /* RW; Range:[0, 51]; the min QP value */
	CVI_U32 u32MaxIQp; /* RW; Range:(u32MinIQp, 51]; max qp for i frame */
	CVI_U32 u32MinIQp; /* RW; Range:[0, 51]; min qp for i frame */
	CVI_S32 s32MaxReEncodeTimes; /* RW; Range:[0, 3]; Range:max number of re-encode times.*/
	CVI_BOOL bQpMapEn; /* RW; Range:[0, 1]; enable qpmap.*/
} VENC_PARAM_H264_UBR_S;

/* The param of mjpege cbr*/
typedef struct _VENC_PARAM_MJPEG_CBR_S {
	CVI_U32 u32MaxQfactor; /* RW; Range:[MinQfactor, 99]; the max Qfactor value*/
	CVI_U32 u32MinQfactor; /* RW; Range:[1, 99]; the min Qfactor value */
} VENC_PARAM_MJPEG_CBR_S;

/* The param of mjpege vbr*/
typedef struct _VENC_PARAM_MJPEG_VBR_S {
	CVI_S32 s32ChangePos;
	// RW; Range:[50, 100]; Indicates the ratio of the current bit rate to the maximum
	// bit rate when the Qfactor value starts to be adjusted
	CVI_U32 u32MaxQfactor; /* RW; Range:[MinQfactor, 99]; max image quailty allowed */
	CVI_U32 u32MinQfactor; /* RW; Range:[1, 99]; min image quality allowed */
} VENC_PARAM_MJPEG_VBR_S;

/* The param of h265e cbr*/
typedef struct _VENC_PARAM_H265_CBR_S {
	CVI_U32 u32MinIprop; /* RW; Range: [1, 100]; the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* RW; Range: (u32MinIprop, 100] ;the max ratio of i frame and p frame */
	CVI_U32 u32MaxQp; /* RW; Range:(u32MinQp, 51];the max QP value */
	CVI_U32 u32MinQp; /* RW; Range:[0, 51];the min QP value */
	CVI_U32 u32MaxIQp; /* RW; Range:(u32MinIQp, 51];max qp for i frame */
	CVI_U32 u32MinIQp; /* RW; Range:[0, 51];min qp for i frame */
	CVI_S32 s32MaxReEncodeTimes; /* RW; Range:[0, 3]; Range:max number of re-encode times.*/
	CVI_BOOL bQpMapEn; /* RW; Range:[0, 1]; enable qpmap.*/
	VENC_RC_QPMAP_MODE_E enQpMapMode; /* RW; Qpmap Mode*/
} VENC_PARAM_H265_CBR_S;

/* The param of h265e vbr*/
typedef struct _VENC_PARAM_H265_VBR_S {
	CVI_S32 s32ChangePos;
	// RW; Range:[50, 100];Indicates the ratio of the current
	// bit rate to the maximum bit rate when the QP value starts to be adjusted
	CVI_U32 u32MinIprop; /* RW; [1, 100]the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* RW; (u32MinIprop, 100]the max ratio of i frame and p frame */
	CVI_S32 s32MaxReEncodeTimes; /* RW; Range:[0, 3]; Range:max number of re-encode times.*/

	CVI_U32 u32MaxQp; /* RW; Range:(u32MinQp, 51]; the max P B qp */
	CVI_U32 u32MinQp; /* RW; Range:[0, 51]; the min P B qp */
	CVI_U32 u32MaxIQp; /* RW; Range:(u32MinIQp, 51]; the max I qp */
	CVI_U32 u32MinIQp; /* RW; Range:[0, 51]; the min I qp */

	CVI_BOOL bQpMapEn; /* RW; Range:[0, 1]; enable qpmap.*/
	VENC_RC_QPMAP_MODE_E enQpMapMode; /* RW; Qpmap Mode*/
} VENC_PARAM_H265_VBR_S;

/* The param of h265e vbr*/
typedef struct _VENC_PARAM_H265_AVBR_S {
	CVI_S32 s32ChangePos;
	// RW; Range:[50, 100];Indicates the ratio of the current
	// bit rate to the maximum bit rate when the QP value starts to be adjusted
	CVI_U32 u32MinIprop; /* RW; [1, 100]the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* RW; (u32MinIprop, 100]the max ratio of i frame and p frame */
	CVI_S32 s32MaxReEncodeTimes; /* RW; Range:[0, 3]; Range:max number of re-encode times.*/

	CVI_S32 s32MinStillPercent; /* RW; Range:[5, 100]; the min percent of target bitrate for still scene */
	CVI_U32 u32MaxStillQP; /* RW; Range:[u32MinIQp, u32MaxIQp]; the max QP value of I frame for still scene*/
	CVI_U32 u32MinStillPSNR; /* RW; reserved */

	CVI_U32 u32MaxQp; /* RW; Range:(u32MinQp, 51];the max P B qp */
	CVI_U32 u32MinQp; /* RW; Range:[0, 51];the min P B qp */
	CVI_U32 u32MaxIQp; /* RW; Range:(u32MinIQp, 51];the max I qp */
	CVI_U32 u32MinIQp; /* RW; Range:[0, 51];the min I qp */
	CVI_U32 u32MinQpDelta;
	// Difference between FrameLevelMinQp & u32MinQp, FrameLevelMinQp = u32MinQp(or u32MinIQp) + MinQpDelta

	CVI_U32 u32MotionSensitivity; /* RW; Range:[0, 100]; Motion Sensitivity */
	CVI_S32	s32AvbrFrmLostOpen; /* RW; Range:[0, 1]; Open Frame Lost */
	CVI_S32 s32AvbrFrmGap; /* RW; Range:[0, 100]; Maximim Gap of Frame Lost */
	CVI_S32 s32AvbrPureStillThr;
	CVI_BOOL bQpMapEn; /* RW; Range:[0, 1]; enable qpmap.*/
	VENC_RC_QPMAP_MODE_E enQpMapMode; /* RW; Qpmap Mode*/
} VENC_PARAM_H265_AVBR_S;

typedef struct _VENC_PARAM_H265_QVBR_S {
	CVI_U32 u32MinIprop; /* the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* the max ratio of i frame and p frame */
	CVI_S32 s32MaxReEncodeTimes; /* max number of re-encode times [0, 3]*/

	CVI_BOOL bQpMapEn;
	VENC_RC_QPMAP_MODE_E enQpMapMode;

	CVI_U32 u32MaxQp; /* the max P B qp */
	CVI_U32 u32MinQp; /* the min P B qp */
	CVI_U32 u32MaxIQp; /* the max I qp */
	CVI_U32 u32MinIQp; /* the min I qp */

	CVI_S32 s32BitPercentUL; /* Indicate the ratio of bitrate  upper limit*/
	CVI_S32 s32BitPercentLL; /* Indicate the ratio of bitrate  lower limit*/
	CVI_S32 s32PsnrFluctuateUL; /* Reduce the target bitrate when the value of psnr approch the upper limit */
	CVI_S32 s32PsnrFluctuateLL; /* Increase the target bitrate when the value of psnr approch the lower limit */
} VENC_PARAM_H265_QVBR_S;

/* The param of h265e ubr*/
typedef struct _VENC_PARAM_H265_UBR_S {
	CVI_U32 u32MinIprop; /* RW; Range: [1, 100]; the min ratio of i frame and p frame */
	CVI_U32 u32MaxIprop; /* RW; Range: (u32MinIprop, 100]; the max ratio of i frame and p frame */
	CVI_U32 u32MaxQp; /* RW; Range:(u32MinQp, 51];the max QP value */
	CVI_U32 u32MinQp; /* RW; Range:[0, 51];the min QP value */
	CVI_U32 u32MaxIQp; /* RW; Range:(u32MinIQp, 51];max qp for i frame */
	CVI_U32 u32MinIQp; /* RW; Range:[0, 51];min qp for i frame */
	CVI_S32 s32MaxReEncodeTimes; /* RW; Range:[0, 3]; Range:max number of re-encode times.*/
	CVI_BOOL bQpMapEn; /* RW; Range:[0, 1]; enable qpmap.*/
	VENC_RC_QPMAP_MODE_E enQpMapMode; /* RW; Qpmap Mode*/
} VENC_PARAM_H265_UBR_S;

/* The param of rc*/
typedef struct _VENC_RC_PARAM_S {
	CVI_U32 u32ThrdI[RC_TEXTURE_THR_SIZE]; // RW; Range:[0, 255]; Mad threshold for
	// controlling the macroblock-level bit rate of I frames
	CVI_U32 u32ThrdP[RC_TEXTURE_THR_SIZE]; // RW; Range:[0, 255]; Mad threshold for
	// controlling the macroblock-level bit rate of P frames
	CVI_U32 u32ThrdB[RC_TEXTURE_THR_SIZE]; // RW; Range:[0, 255]; Mad threshold for
	// controlling the macroblock-level bit rate of B frames
	CVI_U32 u32DirectionThrd; /*RW; Range:[0, 16]; The direction for controlling the macroblock-level bit rate */
	CVI_U32 u32RowQpDelta;
	// RW; Range:[0, 10];the start QP value of each macroblock row relative to the start QP value
	CVI_S32 s32FirstFrameStartQp; /* RW; Range:[-1, 51];Start QP value of the first frame*/
	CVI_S32 s32InitialDelay;	// RW; Range:[10, 3000]; Rate control initial delay (ms).
	CVI_U32 u32ThrdLv; /*RW; Range:[0, 4]; Mad threshold for controlling the macroblock-level bit rate */
	CVI_BOOL bBgEnhanceEn; /* RW; Range:[0, 1];  Enable background enhancement */
	CVI_S32 s32BgDeltaQp; /* RW; Range:[-51, 51]; Backgournd Qp Delta */
	union {
		VENC_PARAM_H264_CBR_S stParamH264Cbr;
		VENC_PARAM_H264_VBR_S stParamH264Vbr;
		VENC_PARAM_H264_AVBR_S stParamH264AVbr;
		VENC_PARAM_H264_QVBR_S stParamH264QVbr;
		VENC_PARAM_H264_UBR_S stParamH264Ubr;
		VENC_PARAM_H265_CBR_S stParamH265Cbr;
		VENC_PARAM_H265_VBR_S stParamH265Vbr;
		VENC_PARAM_H265_AVBR_S stParamH265AVbr;
		VENC_PARAM_H265_QVBR_S stParamH265QVbr;
		VENC_PARAM_H265_UBR_S stParamH265Ubr;
		VENC_PARAM_MJPEG_CBR_S stParamMjpegCbr;
		VENC_PARAM_MJPEG_VBR_S stParamMjpegVbr;
	};
} VENC_RC_PARAM_S;

/* the frame lost mode*/
typedef enum _VENC_FRAMELOST_MODE_E {
	FRMLOST_NORMAL = 0, /*normal mode*/
	FRMLOST_PSKIP, /*pskip*/
	FRMLOST_BUTT,
} VENC_FRAMELOST_MODE_E;

/* The param of the frame lost mode*/
typedef struct _VENC_FRAMELOST_S {
	CVI_BOOL bFrmLostOpen; // RW; Range:[0,1];Indicates whether to discard frames
	// to ensure stable bit rate when the instant bit rate is exceeded
	CVI_U32 u32FrmLostBpsThr; /* RW; Range:[64k, 163840k];the instant bit rate threshold */
	VENC_FRAMELOST_MODE_E enFrmLostMode; /* frame lost strategy*/
	CVI_U32 u32EncFrmGaps; /* RW; Range:[0,65535]; the gap of frame lost*/
} VENC_FRAMELOST_S;

/* the rc priority*/
typedef enum _VENC_RC_PRIORITY_E {
	VENC_RC_PRIORITY_BITRATE_FIRST = 1, /* bitrate first */
	VENC_RC_PRIORITY_FRAMEBITS_FIRST, /* framebits first*/
	VENC_RC_PRIORITY_BUTT,
} VENC_RC_PRIORITY_E;

/* the config of the superframe */
typedef struct _VENC_SUPERFRAME_CFG_S {
	VENC_SUPERFRM_MODE_E enSuperFrmMode;
	/* RW; Indicates the mode of processing the super frame */
	CVI_U32 u32SuperIFrmBitsThr; // RW; Range:[0, 33554432];Indicate the threshold
	// of the super I frame for enabling the super frame processing mode
	CVI_U32 u32SuperPFrmBitsThr; // RW; Range:[0, 33554432];Indicate the threshold
	// of the super P frame for enabling the super frame processing mode
	CVI_U32 u32SuperBFrmBitsThr; // RW; Range:[0, 33554432];Indicate the threshold
	// of the super B frame for enabling the super frame processing mode
	VENC_RC_PRIORITY_E enRcPriority; /* RW; Rc Priority */
} VENC_SUPERFRAME_CFG_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_COMM_RC_H__ */
