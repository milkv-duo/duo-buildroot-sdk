#ifndef __F37P_CMOS_PARAM_H_
#define __F37P_CMOS_PARAM_H_

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
#include "f37p_cmos_ex.h"

static const F37P_MODE_S g_astF37P_mode[F37P_MODE_NUM] = {
	[F37P_MODE_1080P30] = {
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
			{64, 64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64},
			{64, 64, 64, 64, 64, 64, 64, 64, 64, /*8*/64, 64, 64, 64, 64, 64, 64},
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

struct combo_dev_attr_s f37p_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {2, 1, 3, -1, -1},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.dphy = {
			.enable = 1,
			.hs_settle = 8,
		}
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


#endif /* __F37P_CMOS_PARAM_H_ */
