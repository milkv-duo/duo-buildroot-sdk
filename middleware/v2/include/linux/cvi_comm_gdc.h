/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_gdc.h
 * Description:
 *   Common gdc definitions.
 */

#ifndef __CVI_COMM_GDC_H__
#define __CVI_COMM_GDC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include <linux/cvi_type.h>
#include <linux/cvi_common.h>
#include <linux/cvi_comm_video.h>


#define FISHEYE_MAX_REGION_NUM 4
#define AFFINE_MAX_REGION_NUM 32


#ifdef __arm__
typedef CVI_S32 GDC_HANDLE;
#else
typedef CVI_S64 GDC_HANDLE;
#endif

/*
 * stImgIn: Input picture
 * stImgOut: Output picture
 * au64privateData[4]: RW; Private data of task
 * reserved: RW; Debug information,state of current picture
 */
typedef struct _GDC_TASK_ATTR_S {
	VIDEO_FRAME_INFO_S stImgIn;
	VIDEO_FRAME_INFO_S stImgOut;
	CVI_U64 au64privateData[4];
	CVI_U64 reserved;
} GDC_TASK_ATTR_S;

/* Mount mode of device */
typedef enum _FISHEYE_MOUNT_MODE_E {
	FISHEYE_DESKTOP_MOUNT = 0,
	FISHEYE_CEILING_MOUNT = 1,
	FISHEYE_WALL_MOUNT = 2,
	FISHEYE_MOUNT_MODE_BUTT
} FISHEYE_MOUNT_MODE_E;

/* View mode of client*/
typedef enum _FISHEYE_VIEW_MODE_E {
	FISHEYE_VIEW_360_PANORAMA = 0,
	FISHEYE_VIEW_180_PANORAMA = 1,
	FISHEYE_VIEW_NORMAL = 2,
	FISHEYE_NO_TRANSFORMATION = 3,
	FISHEYE_VIEW_MODE_BUTT
} FISHEYE_VIEW_MODE_E;

/*Fisheye region correction attribute
 *
 * enViewMode: gdc view mode
 * u32InRadius: inner radius of gdc correction region
 * u32OutRadius: out radius of gdc correction region
 * u32Pan: Range: [0, 360]
 * u32Tilt: Range: [0, 360]
 * u32HorZoom: Range: [1, 4095]
 * u32VerZoom: Range: [1, 4095]
 * stOutRect: out Imge rectangle attribute
 */
typedef struct _FISHEYE_REGION_ATTR_S {
	FISHEYE_VIEW_MODE_E enViewMode;
	CVI_U32 u32InRadius;
	CVI_U32 u32OutRadius;
	CVI_U32 u32Pan;
	CVI_U32 u32Tilt;
	CVI_U32 u32HorZoom;
	CVI_U32 u32VerZoom;
	RECT_S stOutRect;
} FISHEYE_REGION_ATTR_S;

typedef enum _USAGE_MODE {
	MODE_PANORAMA_360 = 1,
	MODE_PANORAMA_180 = 2,
	MODE_01_1O = 3,
	MODE_02_1O4R = 4,
	MODE_03_4R = 5,
	MODE_04_1P2R = 6,
	MODE_05_1P2R = 7,
	MODE_06_1P = 8,
	MODE_07_2P = 9,
	MODE_MAX
} USAGE_MODE;

/* Fisheye all regions correction attribute
 *
 * bEnable: RW; Range: [0, 1], whether enable fisheye correction or not
 * bBgColor: RW; Range: [0, 1], whether use background color or not
 * u32BgColor: RW; Range: [0,0xffffff], the background color RGB888
 *
 * s32HorOffset: RW; Range: [-511, 511], the horizontal offset between image center and physical center of len
 * s32VerOffset: RW; Range: [-511, 511], the vertical offset between image center and physical center of len
 *
 * u32TrapezoidCoef: RW; Range: [0, 32], strength coefficient of trapezoid correction
 * s32FanStrength: RW; Range: [-760, 760], strength coefficient of fan correction
 * enMountMode: gdc mount mode
 *
 * enUseMode: easy scenario. If this isn't MODE_MAX, then reference this parameter and ignore following region attrs.
 * u32RegionNum: RW; Range: [1, FISHEYE_MAX_REGION_NUM], gdc correction region number
 * astFishEyeRegionAttr: RW; attribute of gdc correction region
 */
typedef struct _FISHEYE_ATTR_S {
	CVI_BOOL bEnable;
	CVI_BOOL bBgColor;
	CVI_U32 u32BgColor;

	CVI_S32 s32HorOffset;
	CVI_S32 s32VerOffset;

	CVI_U32 u32TrapezoidCoef;
	CVI_S32 s32FanStrength;

	FISHEYE_MOUNT_MODE_E enMountMode;

	USAGE_MODE enUseMode;
	CVI_U32 u32RegionNum;
	FISHEYE_REGION_ATTR_S astFishEyeRegionAttr[FISHEYE_MAX_REGION_NUM];
} FISHEYE_ATTR_S;

/*Spread correction attribute
 *
 * bEnable: RW; whether enable spread or not, When spread on,ldc DistortionRatio range should be [0, 500]
 * u32SpreadCoef: RW; Range: [0, 18],strength coefficient of spread correction
 * stDestSize: RW; dest size of spread
 */
typedef struct _SPREAD_ATTR_S {
	CVI_BOOL bEnable;
	CVI_U32 u32SpreadCoef;
	SIZE_S stDestSize;
} SPREAD_ATTR_S;

typedef struct _POINT2F_S {
	CVI_FLOAT x;
	CVI_FLOAT y;
} POINT2F_S;

/* Affine all regions correction attribute
 *
 * u32RegionNum: Range: [1, AFFINE_MAX_REGION_NUM], gdc region number
 * astRegionAttr: source point of gdc affine region
 * stDestSize: dest size of each affine region
 */
typedef struct _AFFINE_ATTR_S {
	CVI_U32 u32RegionNum;
	POINT2F_S astRegionAttr[AFFINE_MAX_REGION_NUM][4];
	SIZE_S stDestSize;
} AFFINE_ATTR_S;

/* Buffer Wrap
 *
 * bEnable: Whether bufwrap is enabled.
 * u32BufLine: buffer line number.
 *             Support 64, 128, 192, 256.
 * u32WrapBufferSize: buffer size.
 */
typedef struct _DWA_BUF_WRAP_S {
	CVI_BOOL bEnable;
	CVI_U32 u32BufLine;
	CVI_U32 u32WrapBufferSize;
} DWA_BUF_WRAP_S;

typedef struct _VI_MESH_ATTR_S {
	VI_CHN chn;
} VI_MESH_ATTR_S;

typedef struct _VPSS_MESH_ATTR_S {
	VPSS_GRP grp;
	VPSS_CHN chn;
} VPSS_MESH_ATTR_S;

typedef struct _MESH_DUMP_ATTR_S {
	CVI_CHAR binFileName[128];
	MOD_ID_E enModId;
	union {
		VI_MESH_ATTR_S viMeshAttr;
		VPSS_MESH_ATTR_S vpssMeshAttr;
	};
} MESH_DUMP_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_COMM_GDC_H__ */
