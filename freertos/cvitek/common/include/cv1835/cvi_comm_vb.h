/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_vb.h
 * Description:
 *   The common data type defination for VB module.
 */

#ifndef __CVI_COMM_VB_H__
#define __CVI_COMM_VB_H__

#include "cvi_type.h"
#include "cvi_defines.h"
#include "cvi_errno.h"
#include "cvi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define VB_INVALID_POOLID (-1U)
#define VB_INVALID_HANDLE (-1U)
#define VB_STATIC_POOLID (-2U)

#define VB_MAX_COMM_POOLS       (16)
#define VB_MAX_MOD_COMM_POOLS   (16)
#define VB_POOL_MAX_BLK         (128)

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
 * u32MaxPoolCnt: number of common pools used.
 * astCommPool: pool cfg for the pools.
 */
typedef struct _VB_CONFIG_S {
	CVI_U32 u32MaxPoolCnt;
	VB_POOL_CONFIG_S astCommPool[VB_MAX_COMM_POOLS];
} VB_CONFIG_S;

typedef struct _VB_POOL_STATUS_S {
	CVI_U32 bIsCommPool;
	CVI_U32 u32BlkCnt;
	CVI_U32 u32FreeBlkCnt;
} VB_POOL_STATUS_S;

#define VB_SUPPLEMENT_JPEG_MASK 0x1
#define VB_SUPPLEMENT_ISPINFO_MASK 0x2
#define VB_SUPPLEMENT_MOTION_DATA_MASK 0x4
#define VB_SUPPLEMENT_DNG_MASK 0x8

typedef struct _VB_SUPPLEMENT_CONFIG_S {
	CVI_U32 u32SupplementConfig;
} VB_SUPPLEMENT_CONFIG_S;

#define CVI_ERR_VB_NULL_PTR CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define CVI_ERR_VB_INVALID CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_VB)
#define CVI_ERR_VB_NOMEM CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define CVI_ERR_VB_NOBUF CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define CVI_ERR_VB_UNEXIST CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define CVI_ERR_VB_ILLEGAL_PARAM CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define CVI_ERR_VB_NOTREADY CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define CVI_ERR_VB_BUSY CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define CVI_ERR_VB_NOT_PERM CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define CVI_ERR_VB_SIZE_NOT_ENOUGH CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_SIZE_NOT_ENOUGH)

#define CVI_ERR_VB_2MPOOLS CVI_DEF_ERR(CVI_ID_VB, EN_ERR_LEVEL_ERROR, EN_ERR_BUTT + 1)

#define CVI_TRACE_VB(level, fmt, ...)                                           \
	CVI_TRACE(level, CVI_ID_VB, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_COMM_VB_H_ */
