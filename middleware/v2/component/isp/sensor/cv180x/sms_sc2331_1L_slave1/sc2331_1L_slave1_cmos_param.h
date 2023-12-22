#ifndef __SC2331_1L_slave1_CMOS_PARAM_H_
#define __SC2331_1L_slave1_CMOS_PARAM_H_

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
#include "sc2331_1L_slave1_cmos_ex.h"

static const SC2331_1L_slave1_MODE_S g_astSC2331_1L_slave1_mode[SC2331_1L_slave1_MODE_NUM] = {
	[SC2331_1L_slave1_MODE_1920X1080P30] = {
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
		.f32MinFps = 0.51, /* 1125 * 30 / 0xFFFF */
		.u32HtsDef = 2200,
		.u32VtsDef = 1125,
		.stExp[0] = {
			.u16Min = 2,//3
			.u16Max = 1125 - 13,
			.u16Def = 400,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u16Min = 1024,
			.u16Max = 32768,
			.u16Def = 1024,
			.u16Step = 1,
		},
		.stDgain[0] = {
			.u16Min = 1024,
			.u16Max = 4096,
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
		.stManual = {256, 256, 256, 256, 0, 0, 0, 0
		#ifdef ARCH_CV182X
		, 1092, 1092, 1092, 1092
		#endif
		},
		.stAuto = {
			{256, 256, 256, 256, 256, 256, 256, 256, /*8*/256, 256, 256, 256, 256, 256, 256, 256},
			{256, 256, 256, 256, 256, 256, 256, 256, /*8*/256, 256, 256, 256, 256, 256, 256, 256},
			{256, 256, 256, 256, 256, 256, 256, 256, /*8*/256, 256, 256, 256, 256, 256, 256, 256},
			{256, 256, 256, 256, 256, 256, 256, 256, /*8*/256, 256, 256, 256, 256, 256, 256, 256},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			#ifdef ARCH_CV182X
			{1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092,
				/*8*/1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092},
			{1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092,
				/*8*/1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092},
			{1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092,
				/*8*/1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092},
			{1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092,
				/*8*/1092, 1092, 1092, 1092, 1092, 1092, 1092, 1092},
			#endif
		},
	},
};

struct combo_dev_attr_s sc2331_1L_slave1_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {3, 2, -1, -1, -1},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.dphy = {
			.enable = 1,
			.hs_settle = 8,
		},
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


#endif /* __SC2331_1L_slave1_CMOS_PARAM_H_ */
