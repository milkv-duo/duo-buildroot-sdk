#ifndef __GC2145_CMOS_PARAM_H_
#define __GC2145_CMOS_PARAM_H_

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
#include "gc2145_cmos_ex.h"

static const GC2145_MODE_S g_astGc2145_mode = {
	.name = "1600X1200P12",
	.stImg = {
		.stSnsSize = {
			.u32Width = 1600,
			.u32Height = 1200,
		},
		.stWndRect = {
			.s32X = 0,
			.s32Y = 0,
			.u32Width = 1600,
			.u32Height = 1200,
		},
		.stMaxSize = {
			.u32Width = 1600,
			.u32Height = 1200,
		},
	},
};

struct combo_dev_attr_s gc2145_rx_attr = {
	.input_mode = INPUT_MODE_BT601,
	.mac_clk = RX_MAC_CLK_400M,
	.ttl_attr = {
		.vi = TTL_VI_SRC_VI1,
		.ttl_fmt = TTL_VSDE_11B,
		.raw_data_type = RAW_DATA_8BIT,
		.func = {
			8, -1, -1, 7,
			2, 3, 4, 5,
			6, 9, 10, 1,
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


#endif /* __GC2145_CMOS_PARAM_H_ */
