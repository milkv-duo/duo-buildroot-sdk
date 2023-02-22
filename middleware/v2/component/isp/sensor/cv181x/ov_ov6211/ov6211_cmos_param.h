#ifndef __OV6211_CMOS_PARAM_H_
#define __OV6211_CMOS_PARAM_H_

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
#include "ov6211_cmos_ex.h"

static const OV6211_MODE_S g_astOv6211_mode[OV6211_MODE_NUM] = {
	[OV6211_MODE_400X400P120] = {
		.name = "400X400P120",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 400,
				.u32Height = 400,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 400,
				.u32Height = 400,
			},
			.stMaxSize = {
				.u32Width = 400,
				.u32Height = 400,
			},
		},
		.f32MaxFps = 120,
		.f32MinFps = 0.80, /* 0x1b6 * 120 / 0xFFFF */
		.u32HtsDef = 1522,
		.u32VtsDef = 438,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 438 - 4,
			.u16Def = 100,
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
			.u16Max = 1024,
			.u16Def = 1024,
			.u16Step = 1,
		},
	},
};

static ISP_CMOS_BLACK_LEVEL_S g_stIspBlcCalibratio = {
	.bUpdate = CVI_TRUE,
	.blcAttr = {
		.Enable = 1,
		.enOpType = OP_TYPE_AUTO,
		.stManual = {128, 128, 128, 128, 0, 0, 0, 0
#ifdef ARCH_CV182X
			, 1057, 1057, 1057, 1057
#endif
		},
		.stAuto = {
			{128, 128, 128, 128, 128, 128, 128, 128, /*8*/128, 128, 128, 128, 128, 128, 128, 128},
			{128, 128, 128, 128, 128, 128, 128, 128, /*8*/128, 128, 128, 128, 128, 128, 128, 128},
			{128, 128, 128, 128, 128, 128, 128, 128, /*8*/128, 128, 128, 128, 128, 128, 128, 128},
			{128, 128, 128, 128, 128, 128, 128, 128, /*8*/128, 128, 128, 128, 128, 128, 128, 128},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1057, 1057, 1057, 1057, 1057, 1057, 1057, 1057,
				/*8*/1057, 1057, 1057, 1057, 1057, 1057, 1057, 1057},
			{1057, 1057, 1057, 1057, 1057, 1057, 1057, 1057,
				/*8*/1057, 1057, 1057, 1057, 1057, 1057, 1057, 1057},
			{1057, 1057, 1057, 1057, 1057, 1057, 1057, 1057,
				/*8*/1057, 1057, 1057, 1057, 1057, 1057, 1057, 1057},
			{1057, 1057, 1057, 1057, 1057, 1057, 1057, 1057,
				/*8*/1057, 1057, 1057, 1057, 1057, 1057, 1057, 1057},
#endif
		},
	},
};

struct combo_dev_attr_s ov6211_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {1, 0, -1, -1, -1},
		.pn_swap = {1, 1, 0, 0, 0},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.dphy = {
			.enable = 1,
			.hs_settle = 8,
		}
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


#endif /* __OV6211_CMOS_PARAM_H_ */
