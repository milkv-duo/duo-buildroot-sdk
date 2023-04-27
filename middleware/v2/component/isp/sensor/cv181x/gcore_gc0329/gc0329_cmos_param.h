#ifndef __GC0329_CMOS_PARAM_H_
#define __GC0329_CMOS_PARAM_H_

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
#include "gc0329_cmos_ex.h"

static const GC0329_MODE_S g_astGc0329_mode = {
	.name = "640X480P10",
	.stImg = {
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
};

struct combo_dev_attr_s gc0329_rx_attr = {
	.input_mode = INPUT_MODE_BT601,
	.mac_clk = RX_MAC_CLK_200M,
	.ttl_attr = {
		.vi = TTL_VI_SRC_VI0,
		.ttl_fmt = TTL_VSDE_11B,
		.raw_data_type = RAW_DATA_8BIT,
		.func = {
			8, -1, -1, 12,
			0, 1, 2, 3,
			4, 13, 14, 11,
			-1, -1, -1, -1,
			-1, -1, -1, -1,
		},
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


#endif /* __GC0329_CMOS_PARAM_H_ */
