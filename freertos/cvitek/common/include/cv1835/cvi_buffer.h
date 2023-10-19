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

#include "cvi_math.h"
#include "cvi_type.h"
#include "cvi_common.h"
#include "cvi_comm_video.h"
#include "cvi_sys.h"

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#define CV183X_WA_FMT(x) (((x) == PIXEL_FORMAT_YUV_PLANAR_420) || ((x) == PIXEL_FORMAT_YUV_PLANAR_422))

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
	CVI_U32 u32ChipId;

	CVI_SYS_GetChipId(&u32ChipId);

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

			u32MainSize = u32YSize + (u32CSize << 1);
			pstCalConfig->plane_num = 3;
		} else if (enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_422) {
			u32CStride = ALIGN(((u32Width >> 1) * u8BitWidth + 7) >> 3, u32Align);
			u32CSize = u32CStride * u32AlignHeight;

			u32MainSize = u32YSize + (u32CSize << 1);
			pstCalConfig->plane_num = 3;
		} else if (enPixelFormat == PIXEL_FORMAT_RGB_888_PLANAR ||
			   enPixelFormat == PIXEL_FORMAT_BGR_888_PLANAR ||
			   enPixelFormat == PIXEL_FORMAT_HSV_888_PLANAR) {
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
		// [WA-01]
		if (IS_CHIP_CV183X(u32ChipId) && CV183X_WA_FMT(enPixelFormat))
			u32VBSize += 0x1000 * 3;
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
	pstCalConfig->u16AddrAlign = (IS_CHIP_CV183X(u32ChipId) && CV183X_WA_FMT(enPixelFormat))
				   ? 0x1000 : u32Align;
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
					    COMPRESS_MODE_E enCmpMode, CVI_U32 u32Align)
{
	CVI_U32 u32BitWidth;
	CVI_U32 u32Size = 0;
	CVI_U32 u32Stride = 0;

	/* u32Align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
	if (u32Align == 0)
		u32Align = DEFAULT_ALIGN;
	else if (u32Align > MAX_ALIGN)
		u32Align = MAX_ALIGN;
	else
		u32Align = (ALIGN(u32Align, DEFAULT_ALIGN));

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
	}

	return u32Size;
}

static inline CVI_U32 VDEC_GetPicBufferSize(PAYLOAD_TYPE_E enType,
		CVI_U32 u32Width, CVI_U32 u32Height,
		PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth,
		CVI_U32 *pu32YSize, CVI_U32 *pu32USize, CVI_U32 *pu32StrideY, CVI_U32 *pu32StrideC)
{
	CVI_U32 u32AlignWidth = 0, u32AlignHeight = 0;
	CVI_U32 u32Size = 0;
	CVI_U32 u32YSize = 0, u32USize = 0;
	CVI_U32 u32Stride = 0;

	UNUSED(enPixelFormat);
	UNUSED(enBitWidth);

	if (enType == PT_H264) {
		u32AlignWidth = ALIGN(u32Width, H264D_ALIGN_W);
		u32AlignHeight = ALIGN(u32Height, H264D_ALIGN_H);
		u32YSize = ALIGN((u32AlignWidth * u32AlignHeight), H264D_ALIGN_FRM);
		u32USize = ALIGN(((u32AlignWidth>>1) * (u32AlignHeight>>1)), H264D_ALIGN_FRM);
		u32Size = u32YSize + (u32USize << 1);
	} else if (enType == PT_H265) {
		u32AlignWidth = ALIGN(u32Width, H265D_ALIGN_W);
		u32AlignHeight = ALIGN(u32Height, H265D_ALIGN_H);
		u32YSize = ALIGN((u32AlignWidth * u32AlignHeight), H265D_ALIGN_FRM);
		u32USize = ALIGN(((u32AlignWidth>>1) * (u32AlignHeight>>1)), H265D_ALIGN_FRM);
		u32Size = u32YSize + (u32USize << 1);
	} else if ((enType == PT_JPEG) || (enType == PT_MJPEG)) {
		u32AlignWidth = ALIGN(u32Width, JPEGD_ALIGN_W);
		u32AlignHeight = ALIGN(u32Height, JPEGD_ALIGN_H);
		u32YSize = ALIGN(u32AlignWidth * u32AlignHeight, JPEGD_ALIGN_FRM);
		u32Stride = ALIGN((u32AlignWidth>>1), JPEGD_ALIGN_W);
		u32USize = ALIGN(u32Stride * (u32AlignHeight>>1), JPEGD_ALIGN_FRM);
		u32Size = u32YSize + (u32USize << 1);
	} else {
		u32Size = 0;
	}
	if (pu32YSize)
		*pu32YSize = u32YSize;
	if (pu32USize)
		*pu32USize = u32USize;
	if (pu32StrideY)
		*pu32StrideY = u32AlignWidth;
	if (pu32StrideC) {
		if ((enType == PT_JPEG) || (enType == PT_MJPEG))
			*pu32StrideC = u32Stride;
		else
			*pu32StrideC = u32AlignWidth >> 1;
	}

	return u32Size;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_BUFFER_H__ */
