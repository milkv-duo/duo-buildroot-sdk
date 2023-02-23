/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: include/cvi_common_inc.h
 * Description:
 */


#ifndef __CVI_COMM_INC_H__
#define __CVI_COMM_INC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "stddef.h"
#include "stdint.h"
#include <sys/time.h>

#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
#include <cvi_common.h>
#include <cvi_comm_vi.h>
#include <cvi_comm_video.h>
#include <cvi_defines.h>
#elif defined(ARCH_RTOS_CV181X)
#include <cvi_common.h>
#include <cvi_comm_vi.h>
#include <cvi_comm_video.h>
#include <cvi_defines.h>
#elif defined(ARCH_CV181X) || defined(ARCH_CV180X) || defined(__CV181X__) || defined(__CV180X__)
#include <linux/cvi_common.h>
#include <linux/cvi_comm_vi.h>
#include <linux/cvi_comm_video.h>
#include <linux/cvi_defines.h>


#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_COMM_INC_H__ */
