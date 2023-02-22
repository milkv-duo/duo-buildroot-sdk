
#include <stdio.h>
#include <assert.h>
#include "sample_venc_testcase.h"

chnInputCfg jpeg_conti_inputCfg_testcase[] = {
	{
		.codec = "jpg",
		.width = 352,
		.height = 288,
		.input_path = "coastguard_352x288_300.yuv",
		.output_path = "coastguard_352x288_300",
		.num_frames = 1,
	},
	{
		.codec = "jpg",
		.width = 720,
		.height = 480,
		.input_path = "sintel_trailer_720x480_300.yuv",
		.output_path = "sintel_trailer_720x480_1",
		.num_frames = 1,
	},
	{
		.codec = "jpg",
		.width = 1280,
		.height = 720,
		.input_path = "shields_1280x720_500.yuv",
		.output_path = "shields_1280x720_500",
		.num_frames = 1,
	},
	{
		.codec = "jpg",
		.width = 1920,
		.height = 1080,
		.input_path = "long_night_walk_1920x1080_400.yuv",
		.output_path = "long_night_walk_1920x1080_400",
		.num_frames = 1,
	},
	{
		.codec = "jpg",
		.width = 1920,
		.height = 1080,
		.input_path = "long_day_cardrive_1920x1080_400.yuv",
		.output_path = "long_day_cardrive_1920x1080_400",
		.num_frames = 1,
	},
	{
		.codec = "jpg",
		.width = 1920,
		.height = 1080,
		.input_path = "ReadySteadyGo_1920x1080_600.yuv",
		.output_path = "ReadySteadyGo_1920x1080_600",
		.num_frames = 1,
	},
	#if defined(ARCH_CV183X) || defined(FPGA_PORTING)
	{
		.codec = "jpg",
		.width = 3840,
		.height = 2160,
		.input_path = "Bosphorus_3840x2160_150.yuv",
		.output_path = "Bosphorus_3840x2160_150",
		.num_frames = 1,
	},
	{
		.codec = "jpg",
		.width = 3840,
		.height = 2160,
		.input_path = "lab_handwave_3840x2160_90.yuv",
		.output_path = "lab_handwave_3840x2160_90",
		.num_frames = 1,
	},
	{
		.codec = "jpg",
		.width = 3840,
		.height = 2160,
		.input_path = "lab_handwave_3840x2160_30_422p.yuv",
		.output_path = "lab_handwave_3840x2160_30_422p",
		.num_frames = 1,
		.pixel_format = 1,
	},
	#endif
	{
		.codec = "jpg",
		.width = 1920,
		.height = 1080,
		.input_path = "ReadySteadyGo_1920x1080_600.yuv",
		.output_path = "quality_30",
		.num_frames = 1,
		.quality = 30,
	},
	{
		.codec = "jpg",
		.width = 1280,
		.height = 480,
		.input_path = "Cactus_1920x1080_500.yuv",
		.output_path = "crop",
		.num_frames = 1,
		.posX = 640,
		.posY = 480,
		.inWidth = 1920,
		.inHeight = 1080,
	},
	#if defined(ARCH_CV183X) || defined(FPGA_PORTING)
	{
		.codec = "jpg",
		.width = 1680,
		.height = 1080,
		.input_path = "lab_handwave_3840x2160_30_422p.yuv",
		.output_path = "crop_422",
		.num_frames = 1,
		.pixel_format = 1,
		.posX = 1680,
		.posY = 480,
		.inWidth = 3840,
		.inHeight = 2160,
	},
	#endif
	{
		.codec = "jpg",
		.width = 1920,
		.height = 1080,
		.input_path = "long_night_walk_1920x1080_400.yuv",
		.output_path = "long_night_walk_1920x1080_400_mcu",
		.num_frames = 1,
		.MCUPerECS = 120,
	},
};

CVI_U32 getNumTestcase(CVI_U32 testMode)
{
	CVI_U32 numTestCase = 0;

	assert(testMode < SAMPLE_VENC_TEST_MODE_MAX);

	if (testMode == JPEG_CONTI_ENCODE_MODE) {
		numTestCase = sizeof(jpeg_conti_inputCfg_testcase) / sizeof(chnInputCfg);
	}

	return numTestCase;
}

chnInputCfg *getInputCfgTestcase(CVI_U32 testMode)
{
	chnInputCfg *pTestIc = NULL;

	assert(testMode < SAMPLE_VENC_TEST_MODE_MAX);

	if (testMode == JPEG_CONTI_ENCODE_MODE) {
		pTestIc = jpeg_conti_inputCfg_testcase;
	}

	return pTestIc;
}
