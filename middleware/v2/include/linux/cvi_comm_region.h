/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_region.h
 * Description:
 *   Common region definitions.
 */

#ifndef __CVI_COMM_REGION_H__
#define __CVI_COMM_REGION_H__

#include <linux/types.h>
#include <linux/cvi_common.h>
#include <linux/cvi_comm_video.h>
#include <linux/cvi_defines.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RGN_COLOR_LUT_NUM 2
#define RGN_BATCHHANDLE_MAX 24
#define RGN_INVALID_HANDLE (-1U)
#define RGN_CMPR_MIN_SIZE 128000

typedef CVI_U32 RGN_HANDLE;
typedef CVI_U32 RGN_HANDLEGROUP;

/* type of video regions */
typedef enum _RGN_TYPE_E {
	OVERLAY_RGN = 0,
	COVER_RGN,
	COVEREX_RGN,
	OVERLAYEX_RGN,
	MOSAIC_RGN,
	RGN_BUTT
} RGN_TYPE_E;

typedef enum _RGN_AREA_TYPE_E {
	AREA_RECT = 0,
	AREA_QUAD_RANGLE,
	AREA_BUTT
} RGN_AREA_TYPE_E;

/*
 * RGN_ABS_COOR: Absolute coordinate.
 * RGN_RATIO_COOR: Ratio coordinate.
 */
typedef enum _RGN_COORDINATE_E {
	RGN_ABS_COOR = 0,
	RGN_RATIO_COOR
} RGN_COORDINATE_E;

/*
 * bSolid: whether solid or dashed quadrangle
 * u32Thick: Line Width of quadrangle, valid when dashed quadrangle
 * stPoint[4]: points of quadrilateral
 */
typedef struct _RGN_QUADRANGLE_S {
	CVI_BOOL bSolid;
	CVI_U32 u32Thick;
	POINT_S stPoint[4];
} RGN_QUADRANGLE_S;

/*
 * enCoverType: rect or arbitrary quadrilateral COVER
 * stRect: config of rect
 * stQuadRangle: config of arbitrary quadrilateral COVER
 * u32Color: color of region.
 * u32Layer: COVER region layer
 * enCoordinate: ratio coordiante or abs coordinate
 */
typedef struct _COVER_CHN_ATTR_S {
	RGN_AREA_TYPE_E enCoverType;
	union {
		RECT_S stRect;
		RGN_QUADRANGLE_S stQuadRangle;
	};
	CVI_U32 u32Color;
	CVI_U32 u32Layer;
	RGN_COORDINATE_E enCoordinate;
} COVER_CHN_ATTR_S;

/*
 * enCoverType: rect or arbitrary quadrilateral COVER
 * stRect: config of rect
 * stQuadRangle: config of arbitrary quadrilateral COVER
 * u32Color: color of region.
 * u32Layer: COVER region layer
 */
typedef struct _COVEREX_CHN_ATTR_S {
	RGN_AREA_TYPE_E enCoverType;
	union {
		RECT_S stRect;
		RGN_QUADRANGLE_S stQuadRangle;
	};
	CVI_U32 u32Color;
	CVI_U32 u32Layer;
} COVEREX_CHN_ATTR_S;

typedef enum _MOSAIC_BLK_SIZE_E {
	MOSAIC_BLK_SIZE_8 = 0, /* block size 8*8 of MOSAIC */
	MOSAIC_BLK_SIZE_16, /* block size 16*16 of MOSAIC */
	MOSAIC_BLK_SIZE_BUTT
} MOSAIC_BLK_SIZE_E;

/*
 * stRect: config of rect
 * enBlkSize: block size of MOSAIC
 * u32Layer: MOSAIC region layer range
 */
typedef struct _MOSAIC_CHN_ATTR_S {
	RECT_S stRect;
	MOSAIC_BLK_SIZE_E enBlkSize;
	CVI_U32 u32Layer;
} MOSAIC_CHN_ATTR_S;

/*
 * OSD_COMPRESS_MODE_NONE : No compression
 * OSD_COMPRESS_MODE_SW : data from SW compressor
 * OSD_COMPRESS_MODE_HW : data from HW compressor
 */
typedef enum _OSD_COMPRESS_MODE_E {
	OSD_COMPRESS_MODE_NONE = 0,
	OSD_COMPRESS_MODE_SW,
	OSD_COMPRESS_MODE_HW,
	OSD_COMPRESS_MODE_BUTT
} OSD_COMPRESS_MODE_E;

/*
 * enOSDCompressMode : Check the VGOP input data from SW/HW compressor.
 * u32EstCompressSize : Estimate bitstream size(SW compressor) for ion_alloc.
 * u32CompressSize : Real bitstream size(HW compressor) for ion_alloc.
 */
typedef struct _OSD_COMPRESS_INFO_S {
	OSD_COMPRESS_MODE_E enOSDCompressMode;
	CVI_U32 u32EstCompressedSize;
	CVI_U32 u32CompressedSize;
} OSD_COMPRESS_INFO_S;

/*
 * u32BgColor: background color, format depends on "enPixelFormat"
 * stSize: region size
 * u32CanvasNum: num of canvas. 2 for double buffer.
 */
typedef struct _OVERLAY_ATTR_S {
	PIXEL_FORMAT_E enPixelFormat;
	CVI_U32 u32BgColor;
	SIZE_S stSize;
	CVI_U32 u32CanvasNum;
	OSD_COMPRESS_INFO_S stCompressInfo;
} OVERLAY_ATTR_S;

/*
 * u32BgColor: background color, format depends on "enPixelFormat"
 * stSize: region size
 * u32CanvasNum: num of canvas. 2 for double buffer.
 */
typedef struct _OVERLAYEX_ATTR_S {
	PIXEL_FORMAT_E enPixelFormat;
	CVI_U32 u32BgColor;
	SIZE_S stSize;
	CVI_U32 u32CanvasNum;
	OSD_COMPRESS_INFO_S stCompressInfo;
} OVERLAYEX_ATTR_S;

typedef enum _INVERT_COLOR_MODE_E {
	LESSTHAN_LUM_THRESH = 0, /* the lum of the video is less than the lum threshold which is set by u32LumThresh  */
	MORETHAN_LUM_THRESH,     /* the lum of the video is more than the lum threshold which is set by u32LumThresh  */
	INVERT_COLOR_BUTT
} INVERT_COLOR_MODE_E;

typedef struct _OVERLAY_INVERT_COLOR_S {
	SIZE_S stInvColArea;
	CVI_U32 u32LumThresh;
	INVERT_COLOR_MODE_E enChgMod;
	CVI_BOOL bInvColEn;  /* The switch of inverting color. */
} OVERLAY_INVERT_COLOR_S;

/*
 * stPoint: position of region.
 * u32Layer: region layer.
 */
typedef struct _OVERLAY_CHN_ATTR_S {
	POINT_S stPoint;
	CVI_U32 u32Layer;
	OVERLAY_INVERT_COLOR_S stInvertColor;
} OVERLAY_CHN_ATTR_S;

/*
 * stPoint: position of region.
 * u32Layer: region layer.
 */
typedef struct _OVERLAYEX_CHN_ATTR_S {
	POINT_S stPoint;
	CVI_U32 u32Layer;
	OVERLAY_INVERT_COLOR_S stInvertColor;
} OVERLAYEX_CHN_ATTR_S;

typedef union _RGN_ATTR_U {
	OVERLAY_ATTR_S stOverlay; /* attribute of overlay region */
	OVERLAYEX_ATTR_S stOverlayEx; /* attribute of overlayex region */
} RGN_ATTR_U;

/* attribute of a region.
 *
 * enType: region type.
 * unAttr: region attribute.
 */
typedef struct _RGN_ATTR_S {
	RGN_TYPE_E enType;
	RGN_ATTR_U unAttr;
} RGN_ATTR_S;

/*
 * stOverlayChn: attribute of overlay region
 * stCoverChn: attribute of cover region
 * stCoverExChn: attribute of coverex region
 * stOverlayExChn: attribute of overlayex region
 * stMosaicChn: attribute of mosic region
 */
typedef union _RGN_CHN_ATTR_U {
	OVERLAY_CHN_ATTR_S stOverlayChn;
	COVER_CHN_ATTR_S stCoverChn;
	COVEREX_CHN_ATTR_S stCoverExChn;
	OVERLAYEX_CHN_ATTR_S stOverlayExChn;
	MOSAIC_CHN_ATTR_S stMosaicChn;
} RGN_CHN_ATTR_U;

/* attribute of a region
 *
 * bShow: region show or not.
 * enType: region type.
 * unChnAttr: region attribute.
 */
typedef struct _RGN_CHN_ATTR_S {
	CVI_BOOL bShow;
	RGN_TYPE_E enType;
	RGN_CHN_ATTR_U unChnAttr;
} RGN_CHN_ATTR_S;


typedef enum _RGN_CMPR_TYPE_E {
	RGN_CMPR_RECT = 0,
	RGN_CMPR_BIT_MAP,
	RGN_CMPR_LINE,
	RGN_CMPR_BUTT
} RGN_CMPR_TYPE_E;

typedef struct _RGN_LINE_ATTR_S {
	POINT_S stPointStart;
	POINT_S stPointEnd;
	CVI_U32 u32Thick;
	CVI_U32 u32Color;
} RGN_LINE_ATTR_S;

typedef struct _RGN_RECT_ATTR_S {
	RECT_S stRect;
	CVI_U32 u32Thick;
	CVI_U32 u32Color;
	CVI_U32 u32IsFill;
} RGN_RECT_ATTR_S;

typedef struct _RGN_BITMAP_ATTR_S {
	RECT_S stRect;
	CVI_U32 u32BitmapPAddr;
} RGN_BITMAP_ATTR_S;

typedef struct _RGN_CMPR_OBJ_ATTR_S {
	RGN_CMPR_TYPE_E enObjType;
	union {
		RGN_LINE_ATTR_S stLine;
		RGN_RECT_ATTR_S stRgnRect;
		RGN_BITMAP_ATTR_S stBitmap;
	};
} RGN_CMPR_OBJ_ATTR_S;

typedef struct _RGN_CANVAS_CMPR_ATTR_S {
	CVI_U32 u32Width;
	CVI_U32 u32Height;
	CVI_U32 u32BgColor;
	PIXEL_FORMAT_E enPixelFormat;
	CVI_U32 u32BsSize;
	CVI_U32 u32ObjNum;
} RGN_CANVAS_CMPR_ATTR_S;

typedef struct _RGN_CANVAS_INFO_S {
	CVI_U64 u64PhyAddr;
	CVI_U8 *pu8VirtAddr;
#ifdef __arm__
	__u32 padding; /* padding for keeping same size of this structure */
#endif
	SIZE_S stSize;
	CVI_U32 u32Stride;
	PIXEL_FORMAT_E enPixelFormat;
	CVI_BOOL bCompressed;
	CVI_U32 u32CompressedSize;
	OSD_COMPRESS_MODE_E enOSDCompressMode;
	RGN_CANVAS_CMPR_ATTR_S *pstCanvasCmprAttr;
	RGN_CMPR_OBJ_ATTR_S *pstObjAttr;
} RGN_CANVAS_INFO_S;

typedef struct _RGN_COMPONENT_INFO_S {
	CVI_S32 alen;
	CVI_S32 rlen;
	CVI_S32 glen;
	CVI_S32 blen;
} RGN_COMP_INFO_S;

typedef struct _RGN_RGBQUAD {
	CVI_U8 argbAlpha;
	CVI_U8 argbRed;
	CVI_U8 argbGreen;
	CVI_U8 argbBlue;
} RGN_RGBQUARD_S;

/* the color format OSD supported */
typedef enum _RGN_COLOR_FMT_E {
	RGN_COLOR_FMT_RGB444 = 0,
	RGN_COLOR_FMT_RGB4444 = 1,
	RGN_COLOR_FMT_RGB555 = 2,
	RGN_COLOR_FMT_RGB565 = 3,
	RGN_COLOR_FMT_RGB1555 = 4,
	RGN_COLOR_FMT_RGB888 = 6,
	RGN_COLOR_FMT_RGB8888 = 7,
	RGN_COLOR_FMT_ARGB4444 = 8,
	RGN_COLOR_FMT_ARGB1555 = 9,
	RGN_COLOR_FMT_ARGB8888 = 10,
	RGN_COLOR_FMT_BUTT
} RGN_COLOR_FMT_E;

typedef struct _RGN_PALETTE {
	RGN_RGBQUARD_S *pstPaletteTable;
	CVI_U16 lut_length;
	RGN_COLOR_FMT_E pixelFormat;
} RGN_PALETTE_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __CVI_COMM_REGION_H__ */
