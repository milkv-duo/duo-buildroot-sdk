#ifndef __OV5647_CMOS_PARAM_H_
#define __OV5647_CMOS_PARAM_H_

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
#include "ov5647_cmos_ex.h"

static const OV5647_MODE_S g_astOv5647_mode[OV5647_MODE_NUM] = {
	[OV5647_MODE_1920X1080P30] = {
		.name = "1920x1080p30",
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
				.u32Width = 2592,
				.u32Height = 1944,
			},
		},

		.f32MaxFps = 30,
		.f32MinFps = 0.711, /* 0x4e2 * 30 / 0xFFFF */
		.u32HtsDef = 2416,
		.u32VtsDef = 1104,
		.stExp[0] = {
			.u16Min = 4,
			.u16Max = 1104 - 4,
			.u16Def = 400,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u32Min = 1024,
			.u32Max = 63448,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[0] = {
			.u32Min = 1024,
			.u32Max = 1024,
			.u32Def = 1024,
			.u32Step = 1,
		},
	},
};

static ISP_CMOS_BLACK_LEVEL_S g_stIspBlcCalibratio = {
	.bUpdate = CVI_TRUE,
	.blcAttr = {
		.Enable = 1,
		.enOpType = OP_TYPE_AUTO,
		.stManual = {60, 60, 60, 60, 0, 0, 0, 0
#ifdef ARCH_CV182X
			, 1039, 1039, 1039, 1039
#endif
		},
		.stAuto = {
			{60, 60, 60, 60, 60, 60, 60, 60, /*8*/60, 60, 60, 60, 60, 60, 60, 60},
			{60, 60, 60, 60, 60, 60, 60, 60, /*8*/60, 60, 60, 60, 60, 60, 60, 60},
			{60, 60, 60, 60, 60, 60, 60, 60, /*8*/60, 60, 60, 60, 60, 60, 60, 60},
			{60, 60, 60, 60, 60, 60, 60, 60, /*8*/60, 60, 60, 60, 60, 60, 60, 60},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039,
				/*8*/1039, 1039, 1039, 1039, 1204, 1039, 1039, 1039},
			{1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039,
				/*8*/1039, 1039, 1039, 1039, 1204, 1039, 1039, 1039},
			{1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039,
				/*8*/1039, 1039, 1039, 1039, 1204, 1039, 1039, 1039},
			{1039, 1039, 1039, 1039, 1039, 1039, 1039, 1039,
				/*8*/1039, 1039, 1039, 1039, 1204, 1039, 1039, 1039},
#endif
		},
	},
};


struct combo_dev_attr_s ov5647_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_400M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {0, 1, 2, -1, -1},
		.pn_swap = {0, 0, 0, 0, 0},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.dphy = {
			.enable = 1,
			.hs_settle = 8,
		},
	},
	.mclk = {
		.cam = 0,
		.freq = CAMPLL_FREQ_24M,
	},
	.devno = 0,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OV5647_CMOS_PARAM_H_ */

