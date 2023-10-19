/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_sys.h
 * Description:
 *   The common sys type defination.
 */

#ifndef __CVI_COMM_SYS_H__
#define __CVI_COMM_SYS_H__

#include "cvi_comm_video.h"
#include "cvi_errno.h"
#include "cvi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define CVI_TRACE_SYS(level, fmt, ...)                                           \
	CVI_TRACE(level, CVI_ID_SYS, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define BIND_DEST_MAXNUM 64
#define BIND_NODE_MAXNUM 64

typedef struct _MMF_BIND_DEST_S {
	CVI_U32   u32Num;
	MMF_CHN_S astMmfChn[BIND_DEST_MAXNUM];
} MMF_BIND_DEST_S;

typedef struct _BIND_NODE_S {
	CVI_BOOL bUsed;
	MMF_CHN_S src;
	MMF_BIND_DEST_S dsts;
} BIND_NODE_S;

typedef enum _VI_VPSS_MODE_E {
	VI_OFFLINE_VPSS_OFFLINE = 0,
	VI_OFFLINE_VPSS_ONLINE,
	VI_ONLINE_VPSS_OFFLINE,
	VI_ONLINE_VPSS_ONLINE,
	VI_BE_OFL_POST_OL_VPSS_OFL,
	VI_BE_OFL_POST_OFL_VPSS_OFL,
	VI_BE_OL_POST_OFL_VPSS_OFL,
	VI_BE_OL_POST_OL_VPSS_OFL,
	VI_VPSS_MODE_BUTT
} VI_VPSS_MODE_E;


typedef struct _VI_VPSS_MODE_S {
	VI_VPSS_MODE_E aenMode[VI_MAX_PIPE_NUM];
} VI_VPSS_MODE_S;

typedef enum _VPSS_MODE_E {
	VPSS_MODE_SINGLE = 0,
	VPSS_MODE_DUAL,
	VPSS_MODE_BUTT
} VPSS_MODE_E;

typedef enum _VPSS_INPUT_E {
	VPSS_INPUT_MEM = 0,
	VPSS_INPUT_ISP,
	VPSS_INPUT_BUTT
} VPSS_INPUT_E;

/*
 * enMode: decide vpss work as 1/2 device.
 * aenInput: decide the input of each vpss device.
 * ViPipe: only meaningful if enInput is ISP.
 */
typedef struct _VPSS_MODE_S {
	VPSS_MODE_E enMode;
	VPSS_INPUT_E aenInput[VPSS_IP_NUM];
	VI_PIPE ViPipe[VPSS_IP_NUM];
} VPSS_MODE_S;


#define CVI_ERR_SYS_NULL_PTR         CVI_DEF_ERR(CVI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define CVI_ERR_SYS_NOTREADY         CVI_DEF_ERR(CVI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define CVI_ERR_SYS_NOT_PERM         CVI_DEF_ERR(CVI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define CVI_ERR_SYS_NOMEM            CVI_DEF_ERR(CVI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define CVI_ERR_SYS_ILLEGAL_PARAM    CVI_DEF_ERR(CVI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define CVI_ERR_SYS_BUSY             CVI_DEF_ERR(CVI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define CVI_ERR_SYS_NOT_SUPPORT      CVI_DEF_ERR(CVI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __CVI_COMM_SYS_H__ */

