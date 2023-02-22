#ifndef __SC035GS_CMOS_PARAM_H_
#define __SC035GS_CMOS_PARAM_H_

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
#include "sc035gs_cmos_ex.h"

static const SC035GS_MODE_S g_astSC035GS_mode[SC035GS_MODE_NUM] = {
	[SC035GS_MODE_640X480P120] = {
		.name = "480p120",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 640,
				.u32Height = 480,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 640,
				.u32Height = 480,
			},
			.stMaxSize = {
				.u32Width = 640,
				.u32Height = 480,
			},
		},
		.f32MaxFps = 120,
		.f32MinFps = 1.25, /* 515 * 120 / 0xFFFF */
		.u32HtsDef = 878,
		.u32VtsDef = 683,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 677 << 4,// (vts - 6) * 16
			.u16Def = 400 << 4,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u32Min = 1024,
			.u32Max = 4096,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[0] = {
			.u32Min = 1024,
			.u32Max = 8160,
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
		.stManual = {145, 145, 145, 145, 0, 0, 0, 0
#ifdef ARCH_CV182X
		, 1061, 1061, 1061, 1061
#endif
		}, /* [TODO] check black leve*/
		.stAuto = {
			{145, 145, 145, 145, 145, 145, 145, 145, /*8*/145, 145, 145, 145, 145, 145, 145, 145},
			{145, 145, 145, 145, 145, 145, 145, 145, /*8*/145, 145, 145, 145, 145, 145, 145, 145},
			{145, 145, 145, 145, 145, 145, 145, 145, /*8*/145, 145, 145, 145, 145, 145, 145, 145},
			{145, 145, 145, 145, 145, 145, 145, 145, /*8*/145, 145, 145, 145, 145, 145, 145, 145},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1061, 1061, 1061, 1061, 1061, 1061, 1061, 1061,
				/*8*/1061, 1061, 1061, 1061, 1061, 1061, 1061, 1061},
			{1061, 1061, 1061, 1061, 1061, 1061, 1061, 1061,
				/*8*/1061, 1061, 1061, 1061, 1061, 1061, 1061, 1061},
			{1061, 1061, 1061, 1061, 1061, 1061, 1061, 1061,
				/*8*/1061, 1061, 1061, 1061, 1061, 1061, 1061, 1061},
			{1061, 1061, 1061, 1061, 1061, 1061, 1061, 1061,
				/*8*/1061, 1061, 1061, 1061, 1061, 1061, 1061, 1061},
#endif
		},
	},
};

struct combo_dev_attr_s sc035gs_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_12BIT,
		.lane_id = {2, 0, 1, -1, -1},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.pn_swap = {1, 1, 1, 0, 0},
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


#endif /* __SC035GS_CMOS_PARAM_H_ */
