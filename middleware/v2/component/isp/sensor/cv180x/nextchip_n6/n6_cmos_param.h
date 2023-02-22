#ifndef __N6_CMOS_PARAM_H_
#define __N6_CMOS_PARAM_H_

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
#include "n6_cmos_ex.h"

static const N6_MODE_S g_astN6_mode[N6_MODE_NUM] = {
	[N6_MODE_1080P_25P] = {
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
};

struct combo_dev_attr_s n6_rx_attr = {
	.input_mode = INPUT_MODE_MIPI,
	.mac_clk = RX_MAC_CLK_400M,
	.mclk = {
		.cam = 0,
		.freq = CAMPLL_FREQ_27M,
	},
	.mipi_attr = {
		.raw_data_type = YUV422_8BIT,
		.lane_id = {2, 0, 1, 3, 4},
		.pn_swap = {1, 1, 1, 1, 1},
		.wdr_mode = CVI_MIPI_WDR_MODE_VC,
		.demux = {
			.demux_en = 1,
			.vc_mapping = {0, 1, 2, 3},
		},
	},
	.devno = 0,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __N6_CMOS_PARAM_H_ */
