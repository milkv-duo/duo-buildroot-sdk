#ifndef __PR2020_CMOS_PARAM_H_
#define __PR2020_CMOS_PARAM_H_

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
#include "pr2020_cmos_ex.h"

static const PR2020_MODE_S g_astPr2020_mode[PR2020_MODE_NUM] = {
	[PR2020_MODE_720P_25] = {
		.name = "720p25",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1280,
				.u32Height = 720,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 1280,
				.u32Height = 720,
			},
			.stMaxSize = {
				.u32Width = 1280,
				.u32Height = 720,
			},
		},
	},
	[PR2020_MODE_720P_30] = {
		.name = "720p30",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = 1280,
				.u32Height = 720,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = 1280,
				.u32Height = 720,
			},
			.stMaxSize = {
				.u32Width = 1280,
				.u32Height = 720,
			},
		},
	},
	[PR2020_MODE_1080P_25] = {
		.name = "1080p25",
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
	},
	[PR2020_MODE_1080P_30] = {
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
	},
};

struct combo_dev_attr_s pr2020_rx_attr = {
	.input_mode = INPUT_MODE_BT656_9B,
	.mac_clk = RX_MAC_CLK_200M,
	.mclk = {
		.cam = 0,
		.freq = CAMPLL_FREQ_NONE,
	},
	.ttl_attr = {
		.vi = TTL_VI_SRC_VI1,
		.func = {
			-1, -1, -1, -1,
			0, 1, 2, 3, 4, 5, 6, 7,
			-1, -1, -1, -1,
			-1, -1, -1, -1,
		},
	},
	.devno = 0,
};

struct combo_dev_attr_s pr2020_rx1_attr = {
	.input_mode = INPUT_MODE_BT656_9B,
	.mac_clk = RX_MAC_CLK_200M,
	.mclk = {
		.cam = 1,
		.freq = CAMPLL_FREQ_NONE,
	},
	.ttl_attr = {
		.vi = TTL_VI_SRC_VI1,
		.func = {
			-1, -1, -1, -1,
			0, 1, 2, 3, 4, 5, 6, 7,
			-1, -1, -1, -1,
			-1, -1, -1, -1,
		},
	},
	.devno = 1,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __PR2020_CMOS_PARAM_H_ */
