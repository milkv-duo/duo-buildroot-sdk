#ifndef _HDMI_LT9611_H_
#define _HDMI_LT9611_H_

#include "linux/cvi_comm_video.h"

static struct combo_dev_cfg_s dev_cfg_lt9611_1920x1080_60Hz = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_2, MIPI_TX_LANE_CLK, MIPI_TX_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 44,
		.vid_hbp_pixels = 148,
		.vid_hfp_pixels = 88,
		.vid_hline_pixels = 1920,
		.vid_vsa_lines = 5,
		.vid_vbp_lines = 36,
		.vid_vfp_lines = 4,
		.vid_active_lines = 1080,
		.vid_vsa_pos_polarity = true,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 148500,
};

static struct combo_dev_cfg_s dev_cfg_lt9611_1920x1080_30Hz = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_2, MIPI_TX_LANE_CLK, MIPI_TX_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 44,
		.vid_hbp_pixels = 148,
		.vid_hfp_pixels = 88,
		.vid_hline_pixels = 1920,
		.vid_vsa_lines = 5,
		.vid_vbp_lines = 36,
		.vid_vfp_lines = 4,
		.vid_active_lines = 1080,
		.vid_vsa_pos_polarity = true,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 74250,
};

static struct combo_dev_cfg_s dev_cfg_lt9611_1280x720_60Hz = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_2, MIPI_TX_LANE_CLK, MIPI_TX_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 40,
		.vid_hbp_pixels = 220,
		.vid_hfp_pixels = 110,
		.vid_hline_pixels = 1280,
		.vid_vsa_lines = 5,
		.vid_vbp_lines = 20,
		.vid_vfp_lines = 5,
		.vid_active_lines = 720,
		.vid_vsa_pos_polarity = true,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 74250,
};

static struct combo_dev_cfg_s dev_cfg_lt9611_1024x768_60Hz = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_2, MIPI_TX_LANE_CLK, MIPI_TX_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 136,
		.vid_hbp_pixels = 160,
		.vid_hfp_pixels = 24,
		.vid_hline_pixels = 1024,
		.vid_vsa_lines = 6,
		.vid_vbp_lines = 29,
		.vid_vfp_lines = 3,
		.vid_active_lines = 768,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = false,
	},
	.pixel_clk = 65000,
};

static struct combo_dev_cfg_s dev_cfg_lt9611_1280x1024_60Hz = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_2, MIPI_TX_LANE_CLK, MIPI_TX_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 112,
		.vid_hbp_pixels = 248,
		.vid_hfp_pixels = 48,
		.vid_hline_pixels = 1280,
		.vid_vsa_lines = 3,
		.vid_vbp_lines = 38,
		.vid_vfp_lines = 1,
		.vid_active_lines = 1024,
		.vid_vsa_pos_polarity = true,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 108000,
};

static struct combo_dev_cfg_s dev_cfg_lt9611_1600x1200_60Hz = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_2, MIPI_TX_LANE_CLK, MIPI_TX_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 192,
		.vid_hbp_pixels = 304,
		.vid_hfp_pixels = 64,
		.vid_hline_pixels = 1600,
		.vid_vsa_lines = 3,
		.vid_vbp_lines = 46,
		.vid_vfp_lines = 1,
		.vid_active_lines = 1200,
		.vid_vsa_pos_polarity = true,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 162000,
};

static struct hs_settle_s hs_timing_cfg_lt9611 = { .prepare = 6, .zero = 32, .trail = 5 };

#endif // _HDMI_LT9611_H_