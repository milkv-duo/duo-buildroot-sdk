/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_comm_sys.h
 * Description:
 *   The common sys type defination.
 */

#ifndef __CVI_COMM_SYS_H__
#define __CVI_COMM_SYS_H__

// #include <linux/cvi_comm_video.h>
// #include "cvi_errno.h"
// #include "cvi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


// #define BIND_DEST_MAXNUM 32
// #define BIND_NODE_MAXNUM 64

// typedef struct _MMF_BIND_DEST_S {
// 	CVI_U32   u32Num;
// 	MMF_CHN_S astMmfChn[BIND_DEST_MAXNUM];
// } MMF_BIND_DEST_S;

// typedef struct _BIND_NODE_S {
// 	CVI_BOOL bUsed;
// 	MMF_CHN_S src;
// 	MMF_BIND_DEST_S dsts;
// } BIND_NODE_S;

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


// typedef struct _VI_VPSS_MODE_S {
// 	VI_VPSS_MODE_E aenMode[VI_MAX_PIPE_NUM];
// } VI_VPSS_MODE_S;

// typedef enum _VPSS_MODE_E {
// 	VPSS_MODE_SINGLE = 0,
// 	VPSS_MODE_DUAL,
// 	VPSS_MODE_RGNEX,
// 	VPSS_MODE_BUTT
// } VPSS_MODE_E;

// typedef enum _VPSS_INPUT_E {
// 	VPSS_INPUT_MEM = 0,
// 	VPSS_INPUT_ISP,
// 	VPSS_INPUT_BUTT
// } VPSS_INPUT_E;


//  * enMode: decide vpss work as 1/2 device.
//  * aenInput: decide the input of each vpss device.
//  * ViPipe: only meaningful if enInput is ISP.
 
// typedef struct _VPSS_MODE_S {
// 	VPSS_MODE_E enMode;
// 	VPSS_INPUT_E aenInput[VPSS_IP_NUM];
// 	VI_PIPE ViPipe[VPSS_IP_NUM];
// } VPSS_MODE_S;

// typedef struct _CVI_TDMA_2D_S {
// 	uint64_t paddr_src;
// 	uint64_t paddr_dst;
// 	uint32_t w_bytes;
// 	uint32_t h;
// 	uint32_t stride_bytes_src;
// 	uint32_t stride_bytes_dst;
// } CVI_TDMA_2D_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __CVI_COMM_SYS_H__ */

