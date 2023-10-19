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

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define BIND_DEST_MAXNUM 64
#define BIND_NODE_MAXNUM 64

struct MMF_BIND_DEST_S {
	uint32_t u32Num;
	struct MMF_CHN_S astMmfChn[BIND_DEST_MAXNUM];
};

struct BIND_NODE_S {
	bool bUsed;
	struct MMF_CHN_S src;
	struct MMF_BIND_DEST_S dsts;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __CVI_COMM_SYS_H__ */

