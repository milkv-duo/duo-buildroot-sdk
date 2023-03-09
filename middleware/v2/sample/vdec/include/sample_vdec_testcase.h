/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample_vdec_testcase.h
 * Description:
 */

#ifndef __SAMPLE_VDEC_TESTCASE_H__
#define __SAMPLE_VDEC_TESTCASE_H__

#include "sample_comm.h"
#include "sample_vdec_lib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum _SAMPLE_VDEC_TEST_MODE_ {
	VCODEC_SINGLE_STREAM_MODE = 0,
	H264_CONTI_DECODE_MODE = 1,
	H265_CONTI_DECODE_MODE = 2,
	JPEG_CONTI_DECODE_MODE = 3,
	SAMPLE_VDEC_TEST_MODE_MAX = 4,
} SAMPLE_VDEC_TEST_MODE;

CVI_U32 vdecGetNumTestcase(CVI_U32 testMode);
vdecChnInputCfg *vdecGetInputCfgTestcase(CVI_U32 testMode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif //__SAMPLE_VDEC_TESTCASE_H__

