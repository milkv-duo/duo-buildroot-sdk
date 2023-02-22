/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample_vdec_testcase.c
 * Description:
 */

#include <stdio.h>
#include <assert.h>
#include "sample_vdec_testcase.h"

vdecChnInputCfg h264_conti_dec_inputCfg_testcase[] = {
	{
		.codec = "264",
		.input_path = "enc-1-1.264",
		.output_path = "enc-1-1",
	},
	{
		.codec = "264",
		.input_path = "enc-1-2.264",
		.output_path = "enc-1-2",
	},
	{
		.codec = "264",
		.input_path = "enc-5.264",
		.output_path = "enc-5",
	},
	{
		.codec = "264",
		.input_path = "enc-6.264",
		.output_path = "enc-6",
	},
	{
		.codec = "264",
		.input_path = "enc-sc_src.264",
		.output_path = "enc-sc_src",
	},
	{
		.codec = "264",
		.input_path = "enc-cqp.264",
		.output_path = "enc-cqp",
	},
	{
		.codec = "264",
		.input_path = "enc-slice.264",
		.output_path = "enc-slice",
	},
	{
		.codec = "264",
		.input_path = "enc-rc.264",
		.output_path = "enc-rc",
	},
	{
		.codec = "264",
		.input_path = "enc-gop8.264",
		.output_path = "enc-gop8",
	},
	{
		.codec = "264",
		.input_path = "enc-1k.264",
		.output_path = "enc-1k",
	},
	{
		.codec = "264",
		.input_path = "enc-2k.264",
		.output_path = "enc-2k",
	},
	{
		.codec = "264",
		.input_path = "enc-2k-2.264",
		.output_path = "enc-2k-2",
	},
	{
		.codec = "264",
		.input_path = "enc-2k-3.264",
		.output_path = "enc-2k-3",
	},
	{
		.codec = "264",
		.input_path = "enc-2k-4.264",
		.output_path = "enc-2k-4",
	},
	{
		.codec = "264",
		.input_path = "enc-2k-5.264",
		.output_path = "enc-2k-5",
	},
	{
		.codec = "264",
		.input_path = "enc-2k-6.264",
		.output_path = "enc-2k-6",
	},
	#ifdef ARCH_CV183X
	{
		.codec = "264",
		.input_path = "enc-4k.264",
		.output_path = "enc-4k",
	},
	{
		.codec = "264",
		.input_path = "enc-4k-2.264",
		.output_path = "enc-4k-2",
	},
	#endif
	{
		.codec = "264",
		.input_path = "enc-roi.264",
		.output_path = "enc-roi",
	},
};

vdecChnInputCfg h265_conti_dec_inputCfg_testcase[] = {
	{
		.codec = "265",
		.input_path = "enc-1-1.265",
		.output_path = "enc-1-1",
	},
	{
		.codec = "265",
		.input_path = "enc-1-2.265",
		.output_path = "enc-1-2",
	},
	{
		.codec = "265",
		.input_path = "enc-2k.265",
		.output_path = "enc-2k",
	},
	{
		.codec = "265",
		.input_path = "enc-2k-1.265",
		.output_path = "enc-2k-1",
	},
	{
		.codec = "265",
		.input_path = "enc-2k-2.265",
		.output_path = "enc-2k-2",
	},
	{
		.codec = "265",
		.input_path = "enc-2k-3.265",
		.output_path = "enc-2k-3",
	},
	{
		.codec = "265",
		.input_path = "enc-5.265",
		.output_path = "enc-5",
	},
	{
		.codec = "265",
		.input_path = "enc-6-1.265",
		.output_path = "enc-6-1",
	},
	{
		.codec = "265",
		.input_path = "enc-6-2.265",
		.output_path = "enc-6-2",
	},
	{
		.codec = "265",
		.input_path = "enc-6-3.265",
		.output_path = "enc-6-3",
	},
	{
		.codec = "265",
		.input_path = "enc-13-1.265",
		.output_path = "enc-13-1",
	},
	{
		.codec = "265",
		.input_path = "enc-13-2.265",
		.output_path = "enc-13-2",
	},
	{
		.codec = "265",
		.input_path = "enc-13-4.265",
		.output_path = "enc-13-4",
	},
	{
		.codec = "265",
		.input_path = "enc-13-5.265",
		.output_path = "enc-13-5",
	},
	{
		.codec = "265",
		.input_path = "enc-16.265",
		.output_path = "enc-16",
	},
	{
		.codec = "265",
		.input_path = "enc-17.265",
		.output_path = "enc-17",
	},
	{
		.codec = "265",
		.input_path = "enc-18.265",
		.output_path = "enc-18",
	},
	{
		.codec = "265",
		.input_path = "enc-19.265",
		.output_path = "enc-19",
	},
	{
		.codec = "265",
		.input_path = "enc-20.265",
		.output_path = "enc-20",
	},
	{
		.codec = "265",
		.input_path = "enc-22.265",
		.output_path = "enc-22",
	},
	{
		.codec = "265",
		.input_path = "enc-roi.265",
		.output_path = "enc-roi",
	},
	{
		.codec = "265",
		.input_path = "enc-svc2l.265",
		.output_path = "enc-svc2l",
	},
	{
		.codec = "265",
		.input_path = "enc-svc3l.265",
		.output_path = "enc-svc3l",
	},
};

vdecChnInputCfg jpeg_conti_dec_inputCfg_testcase[] = {
	{
		.codec = "jpg",
		.input_path = "coastguard_352x288_300.jpg",
		.output_path = "coastguard_352x288_300.jpg",
	},
	{
		.codec = "jpg",
		.input_path = "long_day_cardrive_1920x1080_400.jpg",
		.output_path = "long_day_cardrive_1920x1080_400.jpg",
	},
	{
		.codec = "jpg",
		.input_path = "long_night_walk_1920x1080_400.jpg",
		.output_path = "long_night_walk_1920x1080_400.jpg",
	},
	{
		.codec = "jpg",
		.input_path = "ReadySteadyGo_1920x1080_600.jpg",
		.output_path = "ReadySteadyGo_1920x1080_600.jpg",
	},
	{
		.codec = "jpg",
		.input_path = "shields_1280x720_500.jpg",
		.output_path = "shields_1280x720_500.jpg",
	},
	{
		.codec = "jpg",
		.input_path = "sintel_trailer_720x480_1.jpg",
		.output_path = "sintel_trailer_720x480_1.jpg",
	},
	#if defined(ARCH_CV183X) || defined(FPGA_PORTING)
	{
		.codec = "jpg",
		.input_path = "lab_handwave_3840x2160_90.jpg",
		.output_path = "lab_handwave_3840x2160_90.jpg",
	},
	#endif
	{
		.codec = "mjp",
		.input_path = "coastguard_352x288_300.mjp",
		.output_path = "coastguard_352x288_300.mjp",
	},
	{
		.codec = "mjp",
		.input_path = "long_day_cardrive_1920x1080_400.mjp",
		.output_path = "long_day_cardrive_1920x1080_400.mjp",
	},
	{
		.codec = "mjp",
		.input_path = "ReadySteadyGo_640x480_300.mjp",
		.output_path = "ReadySteadyGo_640x480_300.mjp",
	},
	{
		.codec = "mjp",
		.input_path = "ReadySteadyGo_1920x1080_600.mjp",
		.output_path = "ReadySteadyGo_1920x1080_600.mjp",
	},
	{
		.codec = "mjp",
		.input_path = "sintel_trailer_720x480_10.mjp",
		.output_path = "sintel_trailer_720x480_10.mjp",
	},
};

CVI_U32 vdecGetNumTestcase(CVI_U32 testMode)
{
	CVI_U32 numTestCase = 0;

	assert(testMode < SAMPLE_VDEC_TEST_MODE_MAX);

	if (testMode == H264_CONTI_DECODE_MODE) {
		numTestCase = sizeof(h264_conti_dec_inputCfg_testcase) / sizeof(vdecChnInputCfg);
	} else if (testMode == H265_CONTI_DECODE_MODE) {
		numTestCase = sizeof(h265_conti_dec_inputCfg_testcase) / sizeof(vdecChnInputCfg);
	} else if (testMode == JPEG_CONTI_DECODE_MODE) {
		numTestCase = sizeof(jpeg_conti_dec_inputCfg_testcase) / sizeof(vdecChnInputCfg);
	}

	return numTestCase;
}

vdecChnInputCfg *vdecGetInputCfgTestcase(CVI_U32 testMode)
{
	vdecChnInputCfg *pTestIc = NULL;

	assert(testMode < SAMPLE_VDEC_TEST_MODE_MAX);

	if (testMode == H264_CONTI_DECODE_MODE) {
		pTestIc = h264_conti_dec_inputCfg_testcase;
	} else if (testMode == H265_CONTI_DECODE_MODE) {
		pTestIc = h265_conti_dec_inputCfg_testcase;
	} else if (testMode == JPEG_CONTI_DECODE_MODE) {
		pTestIc = jpeg_conti_dec_inputCfg_testcase;
	}

	return pTestIc;
}
