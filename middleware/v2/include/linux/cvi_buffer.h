/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_buffer.h
 * Description:
 *   The count defination of buffer size
 */

#ifndef __CVI_BUFFER_H__
#define __CVI_BUFFER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include <linux/cvi_math.h>
#include <linux/cvi_type.h>
#include <linux/cvi_common.h>
#include <linux/cvi_comm_video.h>

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#define CV183X_WA_FMT(x) (((x) == PIXEL_FORMAT_YUV_PLANAR_420) || ((x) == PIXEL_FORMAT_YUV_PLANAR_422) || \
			  ((x) == PIXEL_FORMAT_YUV_PLANAR_444) || ((x) == PIXEL_FORMAT_RGB_888_PLANAR) || \
			  ((x) == PIXEL_FORMAT_BGR_888_PLANAR) || ((x) == PIXEL_FORMAT_HSV_888_PLANAR))

static inline CVI_VOID COMMON_GetPicBufferConfig(CVI_U32 u32Width, CVI_U32 u32Height,
	PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth,
	COMPRESS_MODE_E enCmpMode, CVI_U32 u32Align, VB_CAL_CONFIG_S *pstCalConfig)
{
	CVI_U8  u8BitWidth = 0;
	CVI_U32 u32VBSize = 0;
	CVI_U32 u32AlignHeight = 0;
	CVI_U32 u32MainStride = 0;
	CVI_U32 u32CStride = 0;
	CVI_U32 u32MainSize = 0;
	CVI_U32 u32YSize = 0;
	CVI_U32 u32CSize = 0;

	/* u32Align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
	if (u32Align == 0)
		u32Align = DEFAULT_ALIGN;
	else if (u32Align > MAX_ALIGN)
		u32Align = MAX_ALIGN;
	else
		u32Align = (ALIGN(u32Align, DEFAULT_ALIGN));

	switch (enBitWidth) {
	case DATA_BITWIDTH_8: {
		u8BitWidth = 8;
		break;
	}
	case DATA_BITWIDTH_10: {
		u8BitWidth = 10;
		break;
	}
	case DATA_BITWIDTH_12: {
		u8BitWidth = 12;
		break;
	}
	case DATA_BITWIDTH_14: {
		u8BitWidth = 14;
		break;
	}
	case DATA_BITWIDTH_16: {
		u8BitWidth = 16;
		break;
	}
	default: {
		u8BitWidth = 0;
		break;
	}
	}

	if ((enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420)
	 || (enPixelFormat == PIXEL_FORMAT_NV12)
	 || (enPixelFormat == PIXEL_FORMAT_NV21)) {
		u32AlignHeight = ALIGN(u32Height, 2);
	} else
		u32AlignHeight = u32Height;

	if (enCmpMode == COMPRESS_MODE_NONE) {
		u32MainStride = ALIGN((u32Width * u8BitWidth + 7) >> 3, u32Align);
		u32YSize = u32MainStride * u32AlignHeight;

		if (enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420) {
			u32CStride = ALIGN(((u32Width >> 1) * u8BitWidth + 7) >> 3, u32Align);
			u32CSize = (u32CStride * u32AlignHeight) >> 1;

			u32MainStride = u32CStride * 2;
			u32YSize = u32MainStride * u32AlignHeight;
			u32MainSize = u32YSize + (u32CSize << 1);
			pstCalConfig->plane_num = 3;
		} else if (enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_422) {
			u32CStride = ALIGN(((u32Width >> 1) * u8BitWidth + 7) >> 3, u32Align);
			u32CSize = u32CStride * u32AlignHeight;

			u32MainSize = u32YSize + (u32CSize << 1);
			pstCalConfig->plane_num = 3;
		} else if (enPixelFormat == PIXEL_FORMAT_RGB_888_PLANAR ||
			   enPixelFormat == PIXEL_FORMAT_BGR_888_PLANAR ||
			   enPixelFormat == PIXEL_FORMAT_HSV_888_PLANAR ||
			   enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_444) {
			u32CStride = u32MainStride;
			u32CSize = u32YSize;

			u32MainSize = u32YSize + (u32CSize << 1);
			pstCalConfig->plane_num = 3;
		} else if (enPixelFormat == PIXEL_FORMAT_RGB_BAYER_12BPP) {
			u32MainSize = u32YSize;
			pstCalConfig->plane_num = 1;
		} else if (enPixelFormat == PIXEL_FORMAT_YUV_400) {
			u32MainSize = u32YSize;
			pstCalConfig->plane_num = 1;
		} else if (enPixelFormat == PIXEL_FORMAT_NV12 || enPixelFormat == PIXEL_FORMAT_NV21) {
			u32CStride = ALIGN((u32Width * u8BitWidth + 7) >> 3, u32Align);
			u32CSize = (u32CStride * u32AlignHeight) >> 1;

			u32MainSize = u32YSize + u32CSize;
			pstCalConfig->plane_num = 2;
		} else if (enPixelFormat == PIXEL_FORMAT_NV16 || enPixelFormat == PIXEL_FORMAT_NV61) {
			u32CStride = ALIGN((u32Width * u8BitWidth + 7) >> 3, u32Align);
			u32CSize = u32CStride * u32AlignHeight;

			u32MainSize = u32YSize + u32CSize;
			pstCalConfig->plane_num = 2;
		} else if (enPixelFormat == PIXEL_FORMAT_YUYV || enPixelFormat == PIXEL_FORMAT_YVYU ||
			   enPixelFormat == PIXEL_FORMAT_UYVY || enPixelFormat == PIXEL_FORMAT_VYUY) {
			u32MainStride = ALIGN(((u32Width * u8BitWidth + 7) >> 3) * 2, u32Align);
			u32YSize = u32MainStride * u32AlignHeight;
			u32MainSize = u32YSize;
			pstCalConfig->plane_num = 1;
		} else if (enPixelFormat == PIXEL_FORMAT_ARGB_1555 || enPixelFormat == PIXEL_FORMAT_ARGB_4444) {
			// packed format
			u32MainStride = ALIGN((u32Width * 16 + 7) >> 3, u32Align);
			u32YSize = u32MainStride * u32AlignHeight;
			u32MainSize = u32YSize;
			pstCalConfig->plane_num = 1;
		} else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
			// packed format
			u32MainStride = ALIGN((u32Width * 32 + 7) >> 3, u32Align);
			u32YSize = u32MainStride * u32AlignHeight;
			u32MainSize = u32YSize;
			pstCalConfig->plane_num = 1;
		} else {
			// packed format
			u32MainStride = ALIGN(((u32Width * u8BitWidth + 7) >> 3) * 3, u32Align);
			u32YSize = u32MainStride * u32AlignHeight;
			u32MainSize = u32YSize;
			pstCalConfig->plane_num = 1;
		}

		u32VBSize = u32MainSize;
	} else {
		// TODO: compression mode
		pstCalConfig->plane_num = 0;
	}

	pstCalConfig->u32VBSize = u32VBSize;
	pstCalConfig->u32MainStride = u32MainStride;
	pstCalConfig->u32CStride = u32CStride;
	pstCalConfig->u32MainYSize = u32YSize;
	pstCalConfig->u32MainCSize = u32CSize;
	pstCalConfig->u32MainSize = u32MainSize;
	pstCalConfig->u16AddrAlign = u32Align;
}

static inline CVI_U32 COMMON_GetPicBufferSize(CVI_U32 u32Width, CVI_U32 u32Height, PIXEL_FORMAT_E enPixelFormat,
						DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode, CVI_U32 u32Align)
{
	VB_CAL_CONFIG_S stCalConfig;

	COMMON_GetPicBufferConfig(u32Width, u32Height, enPixelFormat, enBitWidth, enCmpMode, u32Align, &stCalConfig);

	return stCalConfig.u32VBSize;
}


#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))

static inline CVI_U32 COMMON_GetVencFrameBufferSize(CVI_S32  s32Codec, CVI_U32  u32Width, CVI_U32  u32Height)
{
	CVI_U32 u32RetSize = 0;

	if (s32Codec == PT_H264) {
		CVI_U32 u32WidthAlign = (ALIGN(u32Width, DEFAULT_ALIGN));
		CVI_U32 u32HeightAlign = (ALIGN(u32Width, DEFAULT_ALIGN));

		u32RetSize = (u32WidthAlign * u32HeightAlign * 2 * 3) / 2;
	} else if (s32Codec == PT_H265) {
		u32RetSize = ((u32Width * u32Height * 3) / 2);
		u32RetSize = ROUND_UP(u32RetSize, 4096) * 2;
	} else
		u32RetSize = 0;

	return u32RetSize;
}
static inline CVI_U32 VI_GetRawBufferSize(CVI_U32 u32Width, CVI_U32 u32Height, PIXEL_FORMAT_E enPixelFormat,
					    COMPRESS_MODE_E enCmpMode, CVI_U32 u32Align, CVI_BOOL isTile)
{
	CVI_U32 u32BitWidth;
	CVI_U32 u32Size = 0;
	CVI_U32 u32Stride = 0;

	/* u32Align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
	if (u32Align == 0)
		u32Align = 16;
	else if (u32Align > MAX_ALIGN)
		u32Align = MAX_ALIGN;
	else
		u32Align = (ALIGN(u32Align, 16));

	switch (enPixelFormat) {
	case PIXEL_FORMAT_RGB_BAYER_8BPP: {
		u32BitWidth = 8;
		break;
	}

	case PIXEL_FORMAT_RGB_BAYER_10BPP: {
		u32BitWidth = 10;
		break;
	}

	case PIXEL_FORMAT_RGB_BAYER_12BPP: {
		u32BitWidth = 12;
		break;
	}

	case PIXEL_FORMAT_RGB_BAYER_14BPP: {
		u32BitWidth = 14;
		break;
	}

	case PIXEL_FORMAT_RGB_BAYER_16BPP: {
		u32BitWidth = 16;
		break;
	}

	default: {
		u32BitWidth = 0;
		break;
	}
	}

	if (enCmpMode == COMPRESS_MODE_NONE) {
		u32Stride = ALIGN(ALIGN(u32Width * u32BitWidth, 8) / 8, u32Align);
		u32Size = u32Stride * u32Height;
	} else if (enCmpMode == COMPRESS_MODE_TILE || enCmpMode == COMPRESS_MODE_FRAME) {
		u32Width = (isTile) ? ((u32Width + 8) >> 1) : (u32Width >> 1);
		u32Width = 3 * ((u32Width + 1) >> 1);

		u32Stride = ALIGN(u32Width, 16);
		u32Size = u32Stride * u32Height;
	}

	return u32Size;
}

static inline CVI_VOID VDEC_GetPicBufferConfig(PAYLOAD_TYPE_E enType,
	CVI_U32 u32Width, CVI_U32 u32Height,
	PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth,
	COMPRESS_MODE_E enCmpMode, VB_CAL_CONFIG_S *pstVbCfg)
{
	CVI_U32 u32AlignWidth = 0, u32AlignHeight = 0, u32Align = 0;

	if (enType == PT_H264) {
		u32AlignWidth = ALIGN(u32Width, H264D_ALIGN_W);
		u32AlignHeight = ALIGN(u32Height, H264D_ALIGN_H);
		u32Align = H264D_ALIGN_W;
	} else if (enType == PT_H265) {
		u32AlignWidth = ALIGN(u32Width, H265D_ALIGN_W);
		u32AlignHeight = ALIGN(u32Height, H265D_ALIGN_H);
		u32Align = H265D_ALIGN_W;
	} else if ((enType == PT_JPEG) || (enType == PT_MJPEG)) {
		u32AlignWidth = ALIGN(u32Width, JPEGD_ALIGN_W);
		u32AlignHeight = ALIGN(u32Height, JPEGD_ALIGN_H);
		u32Align = JPEGD_ALIGN_W;
	} else {
		u32AlignWidth = ALIGN(u32Width, DEFAULT_ALIGN);
		u32AlignHeight = ALIGN(u32Height, DEFAULT_ALIGN);
		u32Align = DEFAULT_ALIGN;
	}

	COMMON_GetPicBufferConfig(u32AlignWidth, u32AlignHeight, enPixelFormat,
		enBitWidth, enCmpMode, u32Align, pstVbCfg);
}

static inline CVI_U32 VDEC_GetPicBufferSize(PAYLOAD_TYPE_E enType,
	CVI_U32 u32Width, CVI_U32 u32Height,
	PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth,
	COMPRESS_MODE_E enCmpMode)
{
	VB_CAL_CONFIG_S stVbCfg;

	memset(&stVbCfg, 0, sizeof(stVbCfg));
	VDEC_GetPicBufferConfig(enType, u32Width, u32Height, enPixelFormat, enBitWidth, enCmpMode, &stVbCfg);
	return stVbCfg.u32VBSize;
}

static inline CVI_VOID VENC_GetPicBufferConfig(CVI_U32 u32Width, CVI_U32 u32Height,
	PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode,
	VB_CAL_CONFIG_S *pstVbCfg)
{
	CVI_U32 u32AlignWidth = ALIGN(u32Width, VENC_ALIGN_W);
	CVI_U32 u32AlignHeight = ALIGN(u32Height, VENC_ALIGN_H);
	CVI_U32 u32Align = VENC_ALIGN_W;

	COMMON_GetPicBufferConfig(u32AlignWidth, u32AlignHeight, enPixelFormat,
		enBitWidth, enCmpMode, u32Align, pstVbCfg);
}

static inline CVI_U32 VENC_GetPicBufferSize(CVI_U32 u32Width, CVI_U32 u32Height,
	PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode)
{
	VB_CAL_CONFIG_S stVbCfg;

	memset(&stVbCfg, 0, sizeof(stVbCfg));
	VENC_GetPicBufferConfig(u32Width, u32Height, enPixelFormat, enBitWidth, enCmpMode, &stVbCfg);
	return stVbCfg.u32VBSize;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_BUFFER_H__ */
