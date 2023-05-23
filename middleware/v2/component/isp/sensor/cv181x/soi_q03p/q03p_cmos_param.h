#ifndef __Q03P_CMOS_PARAM_H_
#define __Q03P_CMOS_PARAM_H_

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
#include "q03p_cmos_ex.h"

static const Q03P_MODE_S g_astQ03P_mode[Q03P_MODE_NUM] = {
	[Q03P_MODE_1296p30] = {
		.name = "1296p30",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 2304,
				.u32Height = 1296,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 2304,
				.u32Height = 1296,
			},
			.stMaxSize = {
				.u32Width = 2304,
				.u32Height = 1296,
			},
		},
		.f32MaxFps = 30,
		.f32MinFps = 0.64, /* 1400 * 30 / 0xFFFF */
		.u32HtsDef = 3600,
		.u32VtsDef = 1400,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 1400,
			.u16Def = 400,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u16Min = 1024,
			.u16Max = 15872,/* 1024 * 15.5 */
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
		.stManual = {64, 64, 64, 64, 0, 0, 0, 0.
#ifdef ARCH_CV182X
		, 1040, 1040, 1040, 1040
#endif
		},

		.stAuto = {
			{64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
					1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
					1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
					1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
			{1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040,
					1040, 1040, 1040, 1040, 1040, 1040, 1040, 1040},
#endif
		},
	},
};

struct combo_dev_attr_s q03p_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {2, 3, 4, -1, -1},
		.pn_swap = {0, 0, 0, 0, 0},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
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


#endif /* __Q03P_CMOS_PARAM_H_ */
