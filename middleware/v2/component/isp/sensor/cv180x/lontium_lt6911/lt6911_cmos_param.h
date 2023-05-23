#ifndef __LT6911_CMOS_PARAM_H_
#define __LT6911_CMOS_PARAM_H_

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
#include "lt6911_cmos_ex.h"

// not real time resolution
#define WIDTH   3840//3840 1920
#define HEIGHT  2160//2160 1080

static LT6911_MODE_S g_astLt6911_mode[LT6911_MODE_NUM] = {
	[LT6911_MODE_NORMAL] = {
		.name = "lt6911",
		.astImg[0] = {
			.stSnsSize = {
				.u32Width = WIDTH,
				.u32Height = HEIGHT,
			},
			.stWndRect = {
				.s32X = 0,
				.s32Y = 0,
				.u32Width = WIDTH,
				.u32Height = HEIGHT,
			},
			.stMaxSize = {
				.u32Width = WIDTH,
				.u32Height = HEIGHT,
			},
		},
	},
};

struct combo_dev_attr_s lt6911_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_600M,
	.mipi_attr = {
		.raw_data_type = YUV422_8BIT,
		.lane_id = {2, 0, 1, 3, 4}, //3, 0, 1, 2, 4  ;   1, 4, 3, 2, 0  2, 0, 1, 3, 4   3, 1, 2, 4, 0
		.pn_swap = {1, 1, 1, 1, 1},
		.wdr_mode = CVI_MIPI_WDR_MODE_NONE,
		.dphy = {
			.enable = 1,
			.hs_settle = 8,
		},
	},
	.mclk = {
		.cam = 0,
		.freq = CAMPLL_FREQ_NONE,
	},
	.devno = 1,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __LT6911_CMOS_PARAM_H_ */