/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_region.h
 * Description:
 *   Common region definitions.
 */

#ifndef __CVI_COMM_REGION_H__
#define __CVI_COMM_REGION_H__

#include "cvi_common.h"
#include "cvi_comm_video.h"
#include "cvi_errno.h"
#include "cvi_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RGN_COLOR_LUT_NUM 2
#define RGN_BATCHHANDLE_MAX 24
#define RGN_INVALID_HANDLE (-1U)

typedef CVI_U32 RGN_HANDLE;
typedef CVI_U32 RGN_HANDLEGROUP;

/* type of video regions */
typedef enum _RGN_TYPE_E {
	OVERLAYEX_RGN = 0,
	COVER_RGN,
	COVEREX_RGN,
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
	MOSAIC_BLK_SIZE_32, /* block size 32*32 of MOSAIC */
	MOSAIC_BLK_SIZE_64, /* block size 64*64 of MOSAIC */
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
 * u32BgColor: background color, format depends on "enPixelFormat"
 * stSize: region size
 * u32CanvasNum: num of canvas. 2 for double buffer.
 */
typedef struct _OVERLAYEX_ATTR_S {
	PIXEL_FORMAT_E enPixelFormat;

	CVI_U32 u32BgColor;

	SIZE_S stSize;
	CVI_U32 u32CanvasNum;
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
typedef struct _OVERLAYEX_CHN_ATTR_S {
	POINT_S stPoint;
	CVI_U32 u32Layer;
	OVERLAY_INVERT_COLOR_S stInvertColor;
} OVERLAYEX_CHN_ATTR_S;

typedef union _RGN_ATTR_U {
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
 * stCoverChn: attribute of cover region
 * stCoverExChn: attribute of coverex region
 * stOverlayExChn: attribute of overlayex region
 * stMosaicChn: attribute of mosic region
 */
typedef union _RGN_CHN_ATTR_U {
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

typedef struct _RGN_CANVAS_INFO_S {
	CVI_U64 u64PhyAddr;
	CVI_U8 *pu8VirtAddr;
	SIZE_S stSize;
	CVI_U32 u32Stride;
	PIXEL_FORMAT_E enPixelFormat;
} RGN_CANVAS_INFO_S;

/* PingPong buffer change when set attr, it needs to remap memory */
#define CVI_NOTICE_RGN_BUFFER_CHANGE CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_NOTICE, CVI_SUCCESS)

/* invalid device ID */
#define CVI_ERR_RGN_INVALID_DEVID CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invalid channel ID */
#define CVI_ERR_RGN_INVALID_CHNID CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at least one parameter is illegal ,eg, an illegal enumeration value  */
#define CVI_ERR_RGN_ILLEGAL_PARAM CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define CVI_ERR_RGN_EXIST CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* region unexist */
#define CVI_ERR_RGN_UNEXIST CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define CVI_ERR_RGN_NULL_PTR CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system, device or channel, before config attribute */
#define CVI_ERR_RGN_NOT_CONFIG CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define CVI_ERR_RGN_NOT_SUPPORT CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define CVI_ERR_RGN_NOT_PERM CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define CVI_ERR_RGN_NOMEM CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define CVI_ERR_RGN_NOBUF CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define CVI_ERR_RGN_BUF_EMPTY CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define CVI_ERR_RGN_BUF_FULL CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define CVI_ERR_RGN_BADADDR CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* resource is busy, eg. destroy a venc chn without unregistering it */
#define CVI_ERR_RGN_BUSY CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* System is not ready,maybe not initialized or loaded.
 * Returning the error code when opening a device file failed.
 */
#define CVI_ERR_RGN_NOTREADY CVI_DEF_ERR(CVI_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __CVI_COMM_REGION_H__ */
