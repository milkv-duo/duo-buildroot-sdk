/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_vb.h
 * Description:
 *   The common data type defination for VB module.
 */

#ifndef __CVI_COMM_VB_H__
#define __CVI_COMM_VB_H__

#include <linux/cvi_type.h>
#include <linux/cvi_defines.h>
#include <linux/cvi_errno.h>
#include "cvi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define VB_INVALID_POOLID (-1U)
#define VB_INVALID_HANDLE (-1U)
#define VB_STATIC_POOLID (-2U)
#define VB_EXTERNAL_POOLID (-3U)

#define VB_MAX_COMM_POOLS       (16)
#define VB_POOL_MAX_BLK         (64)

/* user ID for VB */
#define VB_MAX_USER VB_UID_BUTT

typedef enum _VB_UID_E {
	VB_UID_VI = 0,
	VB_UID_VO = 1,
	VB_UID_VPSS = 2,
	VB_UID_VENC = 3,
	VB_UID_VDEC = 4,
	VB_UID_H265E = 5,
	VB_UID_H264E = 6,
	VB_UID_JPEGE = 7,
	VB_UID_H264D = 8,
	VB_UID_JPEGD = 9,
	VB_UID_DIS = 10,
	VB_UID_USER = 11,
	VB_UID_AI = 12,
	VB_UID_AENC = 13,
	VB_UID_RC = 14,
	VB_UID_VFMW = 15,
	VB_UID_GDC = 16,
	VB_UID_BUTT,

} VB_UID_E;

/* Generall common pool use this owner id, module common pool use VB_UID as owner id */
#define POOL_OWNER_COMMON -1

/* Private pool use this owner id */
#define POOL_OWNER_PRIVATE -2

typedef CVI_U32 VB_POOL;
#ifdef __arm__
typedef CVI_U32 VB_BLK;
#else
typedef CVI_U64 VB_BLK;
#endif

/*
 * VB_REMAP_MODE_NONE: no remap.
 * VB_REMAP_MODE_NOCACHE: no cache remap.
 * VB_REMAP_MODE_CACHED: cache remap. flush cache is needed.
 */
typedef enum _VB_REMAP_MODE_E {
	VB_REMAP_MODE_NONE = 0,
	VB_REMAP_MODE_NOCACHE = 1,
	VB_REMAP_MODE_CACHED = 2,
	VB_REMAP_MODE_BUTT
} VB_REMAP_MODE_E;

/*
 * u32BlkSize: size of blk in the pool.
 * u32BlkCnt: number of blk in the pool.
 * enRemapMode: remap mode.
 */
#define MAX_VB_POOL_NAME_LEN (32)
typedef struct _VB_POOL_CONFIG_S {
	CVI_U32 u32BlkSize;
	CVI_U32 u32BlkCnt;
	VB_REMAP_MODE_E enRemapMode;
	CVI_CHAR acName[MAX_VB_POOL_NAME_LEN];
} VB_POOL_CONFIG_S;

/*
 * au64PhyAddr: Y/U/V base addr.
 */
typedef struct _VB_USER_BLOCK_S {
	CVI_U64 au64PhyAddr[3];
} VB_USER_BLOCK_S;

/*
 * u32BlkCnt: number of blk in the pool.
 * VB_USER_BLOCK_S: block addr.
 */
typedef struct _VB_POOL_CONFIG_EX_S {
	CVI_U32 u32BlkCnt;
	VB_USER_BLOCK_S astUserBlk[VB_POOL_MAX_BLK];
} VB_POOL_CONFIG_EX_S;

/*
 * u32MaxPoolCnt: number of common pools used.
 * astCommPool: pool cfg for the pools.
 */
typedef struct _VB_CONFIG_S {
	CVI_U32 u32MaxPoolCnt;
	VB_POOL_CONFIG_S astCommPool[VB_MAX_COMM_POOLS];
} VB_CONFIG_S;

#define VB_SUPPLEMENT_JPEG_MASK 0x1
#define VB_SUPPLEMENT_ISPINFO_MASK 0x2
#define VB_SUPPLEMENT_MOTION_DATA_MASK 0x4
#define VB_SUPPLEMENT_DNG_MASK 0x8

typedef struct _VB_SUPPLEMENT_CONFIG_S {
	CVI_U32 u32SupplementConfig;
} VB_SUPPLEMENT_CONFIG_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_COMM_VB_H_ */
