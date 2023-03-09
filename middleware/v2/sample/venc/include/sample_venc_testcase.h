/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample_venc_testcase.h
 * Description:
 */

#ifndef __SAMPLE_VENC_TESTCASE_H__
#define __SAMPLE_VENC_TESTCASE_H__

#include "sample_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum _SAMPLE_VENC_TEST_MODE_ {
	VCODEC_SINGLE_STREAM_MODE = 0,
	JPEG_CONTI_ENCODE_MODE,
	BIND_VI_VPSS_VENC,
	VPSS_VENC_FRM_MODE,
	VPSS_VENC_SBM_MODE,
	VI_VPSS_VENC_SBM_MODE,
	VPSS_VENC_SBM_MODE_TWO_INPUT,
	SAMPLE_VENC_TEST_MODE_MAX,
} SAMPLE_VENC_TEST_MODE;

CVI_U32 getNumTestcase(CVI_U32 testMode);
chnInputCfg *getInputCfgTestcase(CVI_U32 testMode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif //__SAMPLE_VENC_TESTCASE_H__
