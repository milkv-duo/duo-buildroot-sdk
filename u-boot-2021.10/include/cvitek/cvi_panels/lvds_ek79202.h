#ifndef _LVDS_EK79202_H_
#define _LVDS_EK79202_H_

#include <cvi_lvds.h>

struct cvi_lvds_cfg_s lvds_ek79202_cfg = {
	.mode = LVDS_MODE_VESA,
	.out_bits = LVDS_OUT_8BIT,
	.chn_num = 1,
	.lane_id = {VO_LVDS_LANE_0, VO_LVDS_LANE_1, VO_LVDS_LANE_2, VO_LVDS_LANE_3, VO_LVDS_LANE_CLK},
	.lane_pn_swap = {false, false, false, false, false},
	.sync_info = {
		.vid_hsa_pixels = 10,
		.vid_hbp_pixels = 88,
		.vid_hfp_pixels = 62,
		.vid_hline_pixels = 1280,
		.vid_vsa_lines = 4,
		.vid_vbp_lines = 23,
		.vid_vfp_lines = 11,
		.vid_active_lines = 800,
		.vid_vsa_pos_polarity = 0,
		.vid_hsa_pos_polarity = 0,
	},
	.u16FrameRate = 60,
	.pixelclock = 72403,
};

#else
#error "_LVDS_EK79202_H_ multi-delcaration!!"
#endif // _LVDS_EK79202_H_
