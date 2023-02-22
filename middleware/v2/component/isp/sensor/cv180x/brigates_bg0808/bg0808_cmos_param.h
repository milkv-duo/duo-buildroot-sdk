#ifndef __BG0808_CMOS_PARAM_H_
#define __BG0808_CMOS_PARAM_H_

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
#include "bg0808_cmos_ex.h"

static const BG0808_MODE_S g_astBG0808_mode[BG0808_MODE_NUM] = {
	[BG0808_MODE_1920X1080P30] = {
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
				.u32Height = 1920,
			},
		},
		.f32MaxFps = 30,
		.f32MinFps = 0.52, /* 1125(0x465) * 30 / 0xFFFF */
		.u32HtsDef = 2200,
		.u32VtsDef = 1125,
		.stExp[0] = {
			.u16Min = 1,
			.u16Max = 1124,//vts - 1
			.u16Def = 400,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u32Min = 1024,
			.u32Max = 5120,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[0] = {
			.u32Min = 1024,
			.u32Max = 16324,
			.u32Def = 1024,
			.u32Step = 1,
		},
	},
	[BG0808_MODE_1920X1080P30_WDR] = {
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
		.f32MinFps = 0.52, /* 1125(0x465) * 30 / 0xFFFF */
		.u32HtsDef = 2200,
		.u32VtsDef = 1125,
		.stExp[0] = {//sexp
			.u16Min = 1,
			.u16Max = 1124,//vts - 1
			.u16Def = 400,
			.u16Step = 1,
		},
		.stExp[1] = {
			.u16Min = 1,
			.u16Max = 1124,//vts - 1
			.u16Def = 400,
			.u16Step = 1,
		},
		.u32IspResTime = 34, /* ceil((u32Vts * f32MaxFps) / 1000); about 1ms*/
		.stAgain[0] = {//sagain
			.u32Min = 1024,
			.u32Max = 5120,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stAgain[1] = {
			.u32Min = 1024,
			.u32Max = 5120,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[0] = {//sagain
			.u32Min = 1024,
			.u32Max = 16324,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[1] = {
			.u32Min = 1024,
			.u32Max = 16324,
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
		#if 0
		.stManual = {0, 0, 0, 0, 0, 0, 0, 0, 1024, 1024, 1024, 1024},
		.stAuto = {
			{0, 0, 0, 0, 0, 0, 0, 0, /*8*/0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, /*8*/0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, /*8*/0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, /*8*/0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
				/*8*/1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024},
			{1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
				/*8*/1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024},
			{1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
				/*8*/1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024},
			{1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
				/*8*/1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024},
		},
		#endif
		#if 1	//use test result as blc offset
		.stManual = {28, 28, 28, 28, 0, 0, 0, 0
#ifdef ARCH_CV182X
			, 1031, 1031, 1031, 1031
#endif
		},
		.stAuto = {
			{28, 28, 28, 28, 28, 28, 28, 28, /*8*/28, 28, 28, 28, 28, 28, 28, 28},
			{28, 28, 28, 28, 28, 28, 28, 28, /*8*/28, 28, 28, 28, 28, 28, 28, 28},
			{28, 28, 28, 28, 28, 28, 28, 28, /*8*/28, 28, 28, 28, 28, 28, 28, 28},
			{28, 28, 28, 28, 28, 28, 28, 28, /*8*/28, 28, 28, 28, 28, 28, 28, 28},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef ARCH_CV182X
			{1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031,
				/*8*/1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031},
			{1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031,
				/*8*/1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031},
			{1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031,
				/*8*/1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031},
			{1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031,
				/*8*/1031, 1031, 1031, 1031, 1031, 1031, 1031, 1031},
#endif
		},
		#endif
	},
};

struct combo_dev_attr_s bg0808_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_10BIT,
		.lane_id = {2, 0, 1, -1, -1},
		.wdr_mode = CVI_MIPI_WDR_MODE_VC,
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


#endif /* __BG0808_CMOS_PARAM_H_ */
