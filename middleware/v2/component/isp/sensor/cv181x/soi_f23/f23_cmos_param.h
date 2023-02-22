#ifndef __F23_CMOS_PARAM_H_
#define __F23_CMOS_PARAM_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef ARCH_CV182X
#include <linux/cvi_vip_cif.h>
#include <linux/cvi_vip_snsr.h>
#include "cvi_type.h"
#else
#include <linux/cif_uapi.h>
#include <linux/vi_snsr.h>
#include <linux/cvi_type.h>
#endif
#include "cvi_sns_ctrl.h"
#include "f23_cmos_ex.h"

static const F23_MODE_S g_astF23_mode[F23_MODE_NUM] = {
	[F23_MODE_1080P30] = {
		.name = "1080p30",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stMaxSize = {
				.u32Width = 1920,
				.u32Height = 1080,
			},
		},
		.f32MaxFps = 30,
		.f32MinFps = 0.52, /* 1125 * 30 / 0xFFFF */
		.u32HtsDef = 2560,
		.u32VtsDef = 1125,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 1120,
			.u16Def = 400,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u16Min = 1024,
			.u16Max = 15872,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stDgain[0] = {
			.u16Min = 1024,
			.u16Max = 4096,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.u8DgainReg = 0x50,
	},
	[F23_MODE_1080P30_WDR] = {
		.name = "1080p30wdr",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stMaxSize = {
				.u32Width = 1920,
				.u32Height = 1080,
			},
		},
		.astImg[1] = {
			.stSnsSize = {
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 1920,
				.u32Height = 1080,
			},
			.stMaxSize = {
				.u32Width = 1920,
				.u32Height = 1080,
			},
		},
		.f32MaxFps = 30,
		.f32MinFps = 1.03, /* 2250 * 30 / 0xFFFF */
		.u32HtsDef = 640,
		.u32VtsDef = 2250,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 131,
			.u16Def = 13,
			.u16Step = 2,
		},
		.stExp[1] = {
			.u16Min = 132,
			.u16Max = 2096,
			.u16Def = 828,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u16Min = 1024,
			.u16Max = 1024,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stAgain[1] = {
			.u16Min = 1024,
			.u16Max = 15872,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stDgain[0] = {
			.u16Min = 1024,
			.u16Max = 1024,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stDgain[1] = {
			.u16Min = 1024,
			.u16Max = 4096,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.u8DgainReg = 0x50,
	},
};

static ISP_CMOS_BLACK_LEVEL_S g_stIspBlcCalibratio = {
	.bUpdate = CVI_TRUE,
	.blcAttr = {
		.Enable = 1,
		.enOpType = OP_TYPE_AUTO,
		.stManual = {68, 68, 68, 68, 0, 0, 0, 0
#ifdef ARCH_CV182X
			, 1096, 1096, 1096, 1096
#endif
		},
		.stAuto = {
			{68, 66, 67, 65, 135, 271, 255, 255, /*8*/255, 255, 255, 255, 256, 255, 256, 256},
			{68, 66, 67, 65, 135, 255, 255, 255, /*8*/255, 255, 255, 255, 256, 255, 256, 256},
			{68, 66, 67, 65, 128, 271, 271, 271, /*8*/255, 255, 256, 256, 256, 256, 256, 256},
			{68, 66, 67, 65, 128, 255, 271, 271, /*8*/255, 255, 256, 255, 256, 256, 256, 256},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1040, 1040, 1040, 1040, 1058, 1096, 1091, 1091,
					/*8*/1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091},
			{1040, 1040, 1040, 1040, 1058, 1091, 1091, 1091,
					/*8*/1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091},
			{1040, 1040, 1040, 1040, 1057, 1096, 1096, 1096,
					/*8*/1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091},
			{1040, 1040, 1040, 1040, 1057, 1091, 1096, 1096,
					/*8*/1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091},
#endif
		},
	},
};

struct combo_dev_attr_s f23_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
#ifdef FPGA_PORTING
		.lane_id = {0, 4, -1, -1, -1},
#else
		.lane_id = {1, 0, 2, -1, -1},
#endif
//		.wdr_mode = CVI_MIPI_WDR_MODE_MANUAL,
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
	},
	.mclk = {
		.cam = 0,
		.freq = CAMPLL_FREQ_27M,
	},
	.devno = 0,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __F23_CMOS_PARAM_H_ */
