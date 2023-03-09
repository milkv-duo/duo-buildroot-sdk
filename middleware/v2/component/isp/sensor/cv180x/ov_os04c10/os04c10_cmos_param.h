#ifndef __OS04C10_CMOS_PARAM_H_
#define __OS04C10_CMOS_PARAM_H_

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
#include "os04c10_cmos_ex.h"

static const OS04C10_MODE_S g_astOs04c10_mode[OS04C10_MODE_NUM] = {
	[OS04C10_MODE_2688X1520P30] = {
		.name = "2688x1520p30",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 2688,
				.u32Height = 1520,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 2688,
				.u32Height = 1520,
			},
			.stMaxSize = {
				.u32Width = 2688,
				.u32Height = 1520,
			},
		},
		.f32MaxFps = 30,
		.f32MinFps = 0.73, /* 0x626 * 30 / 0xFFFF */
		.u32HtsDef = 3116,
		.u32VtsDef = 1574,
		.stExp[0] = {
			.u16Min = 2,
			.u16Max = 1574 - 8,
			.u16Def = 500,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u32Min = 1024,
			.u32Max = 45495,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[0] = {
			.u32Min = 1024,
			.u32Max = 16384,
			.u32Def = 1024,
			.u32Step = 1,
		},
	},
	[OS04C10_MODE_2560X1440P30] = {
		.name = "2560x1440p30",
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
		.f32MaxFps = 30,
		.f32MinFps = 0.73, /* 0x626 * 30 / 0xFFFF */
		.u32HtsDef = 3116,
		.u32VtsDef = 1574,
		.stExp[0] = {
			.u16Min = 2,
			.u16Max = 1574 - 8,
			.u16Def = 500,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u32Min = 1024,
			.u32Max = 45495,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[0] = {
			.u32Min = 1024,
			.u32Max = 16384,
			.u32Def = 1024,
			.u32Step = 1,
		},
	},
	[OS04C10_MODE_2688X1520P30_WDR] = {
		.name = "2688x1520p30wdr",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 2688,
				.u32Height = 1520,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 2688,
				.u32Height = 1520,
			},
			.stMaxSize = {
				.u32Width = 2688,
				.u32Height = 1520,
			},
		},
		.astImg[1] = {
			.stSnsSize = {
				.u32Width = 2688,
				.u32Height = 1520,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 2688,
				.u32Height = 1520,
			},
			.stMaxSize = {
				.u32Width = 2688,
				.u32Height = 1520,
			},
		},
		.f32MaxFps = 30,
		.f32MinFps = 0.77, /* 1682 * 30 / 0xFFFF */
		.u32HtsDef = 2972,
		.u32VtsDef = 1682,
		.u16L2sOffset = 4,
		.u16TopBoundary = 24,
		.u16BotBoundary = 244,
		.stExp[0] = {
			.u16Min = 8,
			.u16Max = 88,
			.u16Def = 88,
			.u16Step = 1,
		},
		.stExp[1] = {
			.u16Min = 8,
			.u16Max = 0x486 - 4 - 88,
			.u16Def = 500,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u32Min = 1024,
			.u32Max = 15872,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stAgain[1] = {
			.u32Min = 1024,
			.u32Max = 15872,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[0] = {
			.u32Min = 1024,
			.u32Max = 16384,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[1] = {
			.u32Min = 1024,
			.u32Max = 16384,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.u32L2S_offset = 4,
		.u32IspResTime = 50,	/* about 1ms * line rate */
		.u32HdrMargin = 40,	/* black_line + zero_line + ISP_offset * 2 */
	},
	[OS04C10_MODE_2560X1440P30_WDR] = {
		.name = "2560x1440p30wdr",
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
		.astImg[1] = {
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
		.f32MaxFps = 30,
		.f32MinFps = 0.77, /* 1682 * 30 / 0xFFFF */
		.u32HtsDef = 2972,
		.u32VtsDef = 1682,
		.u16L2sOffset = 4,
		.u16TopBoundary = 24,
		.u16BotBoundary = 244,
		.stExp[0] = {
			.u16Min = 8,
			.u16Max = 88,
			.u16Def = 88,
			.u16Step = 1,
		},
		.stExp[1] = {
			.u16Min = 8,
			.u16Max = 0x486 - 4 - 88,
			.u16Def = 500,
			.u16Step = 1,
		},
		.stAgain[0] = {
			.u32Min = 1024,
			.u32Max = 15872,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stAgain[1] = {
			.u32Min = 1024,
			.u32Max = 15872,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[0] = {
			.u32Min = 1024,
			.u32Max = 16384,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.stDgain[1] = {
			.u32Min = 1024,
			.u32Max = 16384,
			.u32Def = 1024,
			.u32Step = 1,
		},
		.u32L2S_offset = 4,
		.u32IspResTime = 50,	/* about 1ms * line rate */
		.u32HdrMargin = 40,	/* black_line + zero_line + ISP_offset * 2 */
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

static ISP_CMOS_BLACK_LEVEL_S g_stIspBlcCalibratio10Bit = {
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

struct combo_dev_attr_s os04c10_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_200M,
	.mipi_attr = {
		.raw_data_type = RAW_DATA_12BIT,
		.lane_id = {0, 1, 2, -1, -1},
		.pn_swap = {1, 1, 1, 0, 0},
		.wdr_mode = CVI_MIPI_WDR_MODE_VC,
	},
	.mclk = {
		.cam = 0,
		.freq = CAMPLL_FREQ_25M,
	},
	.devno = 0,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OS04C10_CMOS_PARAM_H_ */
