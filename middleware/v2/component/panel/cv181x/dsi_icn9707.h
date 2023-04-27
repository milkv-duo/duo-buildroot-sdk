#ifndef _MIPI_TX_PARAM_ICN9707_H_
#define _MIPI_TX_PARAM_ICN9707_H_

#include <linux/vo_mipi_tx.h>
#include <linux/cvi_comm_mipi_tx.h>

struct combo_dev_cfg_s dev_cfg_icn9707_480x1920 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_3, MIPI_TX_LANE_2, MIPI_TX_LANE_CLK, MIPI_TX_LANE_1, MIPI_TX_LANE_0},
	.lane_pn_swap = {true, true, true, true, true},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 4,
		.vid_hbp_pixels = 53,
		.vid_hfp_pixels = 53,
		.vid_hline_pixels = 480,
		.vid_vsa_lines = 4,
		.vid_vbp_lines = 12,
		.vid_vfp_lines = 32,
		.vid_active_lines = 1920,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 69660,
};

const struct hs_settle_s hs_timing_cfg_icn9707_480x1920 = { .prepare = 6, .zero = 32, .trail = 1 };

static CVI_U8 data_icn9707_0[] = { 0xf0, 0x5a, 0x59 };
static CVI_U8 data_icn9707_1[] = { 0xf1, 0xa5, 0xa6 };
static CVI_U8 data_icn9707_2[] = {
	0xb4, 0x1d, 0x1c, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x0c, 0x0d,
	0x0e, 0x0f, 0x00, 0x1e, 0x1f, 0x04, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03
};
static CVI_U8 data_icn9707_3[] = {
	0xb0, 0x60, 0x00, 0x00, 0x08, 0x66, 0x66, 0x33, 0x33, 0x66,
	0x00, 0x00, 0xaf, 0x00, 0x00, 0x0f
};
static CVI_U8 data_icn9707_4[] = {
	0xb1, 0x53, 0xa0, 0x00, 0x85, 0x0e, 0x00, 0x00, 0x62, 0x00,
	0x00
};
static CVI_U8 data_icn9707_5[] = {
	0xb2, 0x37, 0x09, 0x08, 0x8b, 0x08, 0x00, 0x22, 0x00, 0x44,
	0xd9
};
static CVI_U8 data_icn9707_6[] = { 0xb6, 0x49, 0x49 };
static CVI_U8 data_icn9707_7[] = {
	0xb7, 0x01, 0x01, 0x09, 0x0d, 0x11, 0x19, 0x1d, 0x15, 0x00,
	0x25, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0xf7, 0x38
};
static CVI_U8 data_icn9707_8[] = { 0xb8, 0x34, 0x53, 0x02, 0xcc };
static CVI_U8 data_icn9707_9[] = { 0xba, 0x27, 0x33 };
static CVI_U8 data_icn9707_10[] = {
	0xbd, 0x43, 0x0e, 0x0e, 0x4b, 0x4b, 0x14, 0x14
};
static CVI_U8 data_icn9707_11[] = {
	0xc1, 0x00, 0x0c, 0x20, 0x04, 0x00, 0x32, 0x32, 0x04
};
static CVI_U8 data_icn9707_12[] = { 0xc2, 0x31, 0xc0 };
static CVI_U8 data_icn9707_13[] = { 0xc3, 0x22, 0x31 };
static CVI_U8 data_icn9707_14[] = {
	0xc6, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00
};
static CVI_U8 data_icn9707_15[] = {
	0xc8, 0x7c, 0x66, 0x56, 0x49, 0x46, 0x37, 0x3a, 0x23, 0x3b,
	0x38, 0x38, 0x55, 0x43, 0x4c, 0x40, 0x3c, 0x2f, 0x1c, 0x06,
	0x7c, 0x65, 0x56, 0x4a, 0x46, 0x37, 0x3a, 0x23, 0x3a, 0x38,
	0x38, 0x55, 0x43, 0x4c, 0x40, 0x3c, 0x2f, 0x1c, 0x06
};
static CVI_U8 data_icn9707_16[] = { 0xd0, 0x07, 0xff, 0xff };
static CVI_U8 data_icn9707_17[] = { 0xd2, 0x63, 0x0b, 0x08, 0x88 };
static CVI_U8 data_icn9707_18[] = {
	0xd4, 0x00, 0x00, 0x00, 0x32, 0x04, 0x54
};
static CVI_U8 data_icn9707_19[] = { 0xf1, 0x5a, 0x59 };
static CVI_U8 data_icn9707_20[] = { 0xf0, 0xa5, 0xa6 };
static CVI_U8 data_icn9707_21[] = { 0x11 };
static CVI_U8 data_icn9707_22[] = { 0x29 };

#ifdef _MIPI_TX_BIST_MODE
static CVI_U8 data_icn9707_23[] = { 0x01 };
static CVI_U8 data_icn9707_24[] = { 0xF0, 0x5A, 0x59 };
static CVI_U8 data_icn9707_25[] = { 0xF1, 0xA5, 0xA6 };
static CVI_U8 data_icn9707_26[] = { 0xC0, 0x11 };
static CVI_U8 data_icn9707_27[] = { 0x11 };
static CVI_U8 data_icn9707_28[] = { 0x29 };
#endif

const struct dsc_instr dsi_init_cmds_icn9707_480x1920[] = {
#ifdef _MIPI_TX_BIST_MODE
	{ .delay = 120, .data_type = 0x05, .size = 1, .data = data_icn9707_23 },
	{ .delay = 0, .data_type = 0x29, .size = 3, .data = data_icn9707_24 },
	{ .delay = 0, .data_type = 0x29, .size = 1, .data = data_icn9707_25 },
	{ .delay = 0, .data_type = 0x15, .size = 2, .data = data_icn9707_26 },
	{ .delay = 200, .data_type = 0x05, .size = 1, .data = data_icn9707_27 },
	{ .delay = 50, .data_type = 0x05, .size = 1, .data = data_icn9707_28 }
#else
	{.delay = 1, .data_type = 0x29, .size = 3, .data = data_icn9707_0 },
	{.delay = 1, .data_type = 0x29, .size = 3, .data = data_icn9707_1 },
	{.delay = 1, .data_type = 0x29, .size = 23, .data = data_icn9707_2 },
	{.delay = 1, .data_type = 0x29, .size = 16, .data = data_icn9707_3 },
	{.delay = 1, .data_type = 0x29, .size = 11, .data = data_icn9707_4 },
	{.delay = 1, .data_type = 0x29, .size = 11, .data = data_icn9707_5 },
	{.delay = 1, .data_type = 0x29, .size = 3, .data = data_icn9707_6 },
	{.delay = 1, .data_type = 0x29, .size = 19, .data = data_icn9707_7 },
	{.delay = 1, .data_type = 0x29, .size = 5, .data = data_icn9707_8 },
	{.delay = 1, .data_type = 0x29, .size = 3, .data = data_icn9707_9 },
	{.delay = 1, .data_type = 0x29, .size = 8, .data = data_icn9707_10 },
	{.delay = 1, .data_type = 0x29, .size = 9, .data = data_icn9707_11 },
	{.delay = 1, .data_type = 0x29, .size = 3, .data = data_icn9707_12 },
	{.delay = 1, .data_type = 0x29, .size = 3, .data = data_icn9707_13 },
	{.delay = 1, .data_type = 0x29, .size = 9, .data = data_icn9707_14 },
	{.delay = 1, .data_type = 0x29, .size = 39, .data = data_icn9707_15 },
	{.delay = 1, .data_type = 0x29, .size = 4, .data = data_icn9707_16 },
	{.delay = 1, .data_type = 0x29, .size = 5, .data = data_icn9707_17 },
	{.delay = 1, .data_type = 0x29, .size = 7, .data = data_icn9707_18 },
	{.delay = 1, .data_type = 0x29, .size = 3, .data = data_icn9707_19 },
	{.delay = 1, .data_type = 0x29, .size = 3, .data = data_icn9707_20 },
	{.delay = 200, .data_type = 0x05, .size = 1, .data = data_icn9707_21 },
	{.delay = 50, .data_type = 0x05, .size = 1, .data = data_icn9707_22 }
#endif
};

#else
#error "MIPI_TX_PARAM multi-delcaration!!"
#endif // _MIPI_TX_PARAM_ICN9707_H_
