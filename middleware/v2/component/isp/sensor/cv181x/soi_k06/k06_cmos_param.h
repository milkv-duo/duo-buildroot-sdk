#ifndef __K06_CMOS_PARAM_H_
#define __K06_CMOS_PARAM_H_

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
#include "k06_cmos_ex.h"

static const K06_MODE_S g_astK06_mode[K06_MODE_NUM] = {
	[K06_MODE_1440P25] = {
		.name = "1440p25",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 2560,
				.u32Height = 1440,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 2560,
				.u32Height = 1440,
			},
			.stMaxSize = {
				.u32Width = 2560,
				.u32Height = 1440,
			},
		},
		.f32MaxFps = 25,
		.f32MinFps = 0.57, /* 1500 * 25 / 0xFFFF */
		.u32HtsDef = 4608,
		.u32VtsDef = 1500,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 1500,
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
		.stManual = {64, 64, 64, 64, 0, 0, 0, 0
#ifdef ARCH_CV182X
			, 1040, 1040, 1040, 1040
#endif
		},
		.stAuto = {
			{64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64, 64},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
				/*8*/1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
				/*8*/1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
				/*8*/1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
				/*8*/1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
#endif
		},
	},
};

struct combo_dev_attr_s k06_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {2, 1, 3, -1, -1},
		.pn_swap = {0, 0, 0, 0, 0},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
	},
	.mclk = {
		.cam = 1,
		.freq = CAMPLL_FREQ_24M,
	},
	.devno = 0,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __K06_CMOS_PARAM_H_ */
