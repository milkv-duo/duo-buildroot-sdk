#ifndef __N5_CMOS_PARAM_H_
#define __N5_CMOS_PARAM_H_

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
#include "n5_cmos_ex.h"

static const N5_MODE_S g_astN5_mode[N5_MODE_NUM] = {
	[N5_MODE_1080P_25P] = {
		.name = "1080p25",
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
};

struct combo_dev_attr_s n5_rx_attr = {
	.input_mode = INPUT_MODE_BT_DEMUX,
	.mac_clk = RX_MAC_CLK_400M,
	.mclk = {
		.cam = 1,
		.freq = CAMPLL_FREQ_27M,
	},
	.bt_demux_attr = {
		.func = {
			-1, -1, -1, -1,
			0, 1, 2, 3, 4, 5, 6, 7,
			-1, -1, -1, -1,
			-1, -1, -1, -1
		},
		.v_fp = 0xF,
		.h_fp = 0xF,
		.v_bp = 0,
		.h_bp = 0,
		.mode = BT_DEMUX_2,
		.sync_code_part_A = {0xFF, 0, 0},
		.sync_code_part_B[0] = {
			.sav_vld = 0x80,
			.sav_blk = 0xa0,
			.eav_vld = 0x90,
			.eav_blk = 0xb0,
		},
		.sync_code_part_B[1] = {
			.sav_vld = 0x81,
			.sav_blk = 0xa1,
			.eav_vld = 0x91,
			.eav_blk = 0xb1,
		},
		.sync_code_part_B[2] = {
			.sav_vld = 0x82,
			.sav_blk = 0xa2,
			.eav_vld = 0x92,
			.eav_blk = 0xb2,
		},
		.sync_code_part_B[3] = {
			.sav_vld = 0x83,
			.sav_blk = 0xa3,
			.eav_vld = 0x93,
			.eav_blk = 0xb3,
		},
		.yc_exchg = 0xF,
	},
	.devno = 0,	// btdemux must use mac0
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __N5_CMOS_PARAM_H_ */
