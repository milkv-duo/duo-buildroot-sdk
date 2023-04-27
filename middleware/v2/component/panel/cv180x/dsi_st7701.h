#ifndef _MIPI_TX_PARAM_ST_7701_H_
#define _MIPI_TX_PARAM_ST_7701_H_

#include <linux/vo_mipi_tx.h>
#include <linux/cvi_comm_mipi_tx.h>

#define PANEL_NAME "NETEASE-2"

#define ST7701_NETEASE_VACT	800
#define ST7701_NETEASE_VSA		10
#define ST7701_NETEASE_VBP		20
#define ST7701_NETEASE_VFP		20

#define ST7701_NETEASE_HACT		480
#define ST7701_NETEASE_HSA		10
#define ST7701_NETEASE_HBP		50
#define ST7701_NETEASE_HFP		50

#define PIXEL_CLK(x) ((x##_VACT + x##_VSA + x##_VBP + x##_VFP) \
	* (x##_HACT + x##_HSA + x##_HBP + x##_HFP) * 60 / 1000)

struct combo_dev_cfg_s dev_cfg_st7701_480x800 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_1, MIPI_TX_LANE_0, MIPI_TX_LANE_CLK, -1, -1},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = ST7701_NETEASE_HSA,
		.vid_hbp_pixels = ST7701_NETEASE_HBP,
		.vid_hfp_pixels = ST7701_NETEASE_HFP,
		.vid_hline_pixels = ST7701_NETEASE_HACT,
		.vid_vsa_lines = ST7701_NETEASE_VSA,
		.vid_vbp_lines = ST7701_NETEASE_VBP,
		.vid_vfp_lines = ST7701_NETEASE_VFP,
		.vid_active_lines = ST7701_NETEASE_VACT,
		.vid_vsa_pos_polarity = true,
		.vid_hsa_pos_polarity = false,
	},
	.pixel_clk = PIXEL_CLK(ST7701_NETEASE),
};

const struct hs_settle_s hs_timing_cfg_st7701_480x800 = { .prepare = 6, .zero = 32, .trail = 1 };

static CVI_U8 data_st7701_0[] = { 0xff, 0x77, 0x01, 0x00, 0x00, 0x13 };
static CVI_U8 data_st7701_1[] = { 0xef, 0x08 };
static CVI_U8 data_st7701_2[] = { 0xff, 0x77, 0x01, 0x00, 0x00, 0x10 };
static CVI_U8 data_st7701_3[] = { 0xc0, 0x63, 0x00 };
static CVI_U8 data_st7701_4[] = { 0xc1, 0x11, 0x0c };
static CVI_U8 data_st7701_5[] = { 0xc2, 0x07, 0x08 };
static CVI_U8 data_st7701_6[] = { 0xcc, 0x10 };
static CVI_U8 data_st7701_7[] = {
	0xb0, 0x00, 0x0c, 0x13, 0x0d, 0x10, 0x06, 0x01, 0x08, 0x07,
	0x1e, 0x04, 0x13, 0x10, 0x2d, 0x31, 0x10
};
static CVI_U8 data_st7701_8[] = {
	0xb1, 0x00, 0x0c, 0x13, 0x0d, 0x10, 0x06, 0x02, 0x08, 0x07,
	0x1f, 0x05, 0x12, 0x10, 0x27, 0x31, 0x1f
};
static CVI_U8 data_st7701_9[] = { 0xff, 0x77, 0x01, 0x00, 0x00, 0x11 };
static CVI_U8 data_st7701_10[] = { 0xb0, 0x70 };
static CVI_U8 data_st7701_11[] = { 0xb1, 0x93 };
static CVI_U8 data_st7701_12[] = { 0xb2, 0x87 };
static CVI_U8 data_st7701_13[] = { 0xb3, 0x80 };
static CVI_U8 data_st7701_14[] = { 0xb5, 0x49 };
static CVI_U8 data_st7701_15[] = { 0xb7, 0x87 };
static CVI_U8 data_st7701_16[] = { 0xb8, 0x21 };
static CVI_U8 data_st7701_17[] = { 0xb9, 0x10, 0x1f };
static CVI_U8 data_st7701_18[] = { 0xbb, 0x03 };
static CVI_U8 data_st7701_19[] = { 0xc0, 0x89 };
static CVI_U8 data_st7701_20[] = { 0xc1, 0x08 };
static CVI_U8 data_st7701_21[] = { 0xc2, 0x08 };
static CVI_U8 data_st7701_22[] = { 0xc8, 0xbe };
static CVI_U8 data_st7701_23[] = { 0xd0, 0x88 };
static CVI_U8 data_st7701_24[] = {
	0xe0, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0c
};
static CVI_U8 data_st7701_25[] = {
	0xe1, 0x05, 0x8c, 0x07, 0x8c, 0x06, 0x8c, 0x08, 0x8c, 0x00,
	0x44, 0x44
};
static CVI_U8 data_st7701_26[] = {
	0xe2, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x02, 0x00, 0x00,
	0x00, 0x02, 0x00
};
static CVI_U8 data_st7701_27[] = { 0xe3, 0x00, 0x00, 0x33, 0x33 };
static CVI_U8 data_st7701_28[] = { 0xe4, 0x44, 0x44 };
static CVI_U8 data_st7701_29[] = {
	0xe5, 0x0d, 0x3f, 0x0c, 0xa0, 0x0f, 0x41, 0x0c, 0xa0, 0x09,
	0x3b, 0x0c, 0xa0, 0x0b, 0x3d, 0x0c, 0xa0
};
static CVI_U8 data_st7701_30[] = { 0xe6, 0x00, 0x00, 0x33, 0x33 };
static CVI_U8 data_st7701_31[] = { 0xe7, 0x44, 0x44 };
static CVI_U8 data_st7701_32[] = {
	0xe8, 0x0e, 0x40, 0x0c, 0xa0, 0x10, 0x42, 0x0c, 0xa0, 0x0a,
	0x3c, 0x0c, 0xa0, 0x0c, 0x3e, 0x0c, 0xa0
};
static CVI_U8 data_st7701_33[] = {
	0xeb, 0x00, 0x01, 0xe4, 0xe4, 0x44, 0x00
};
static CVI_U8 data_st7701_34[] = {
	0xed, 0xf3, 0xc1, 0xba, 0x0f, 0x66, 0x77, 0x44, 0x55, 0x55,
	0x44, 0x77, 0x66, 0xf0, 0xab, 0x1c, 0x3f
};
static CVI_U8 data_st7701_35[] = {
	0xef, 0x10, 0x0d, 0x04, 0x08, 0x3f, 0x1f
};
static CVI_U8 data_st7701_36[] = { 0xff, 0x77, 0x01, 0x00, 0x00, 0x13 };
static CVI_U8 data_st7701_37[] = { 0x11 };
static CVI_U8 data_st7701_38[] = { 0x29 };
static CVI_U8 data_st7701_39[] = { 0x36, 0x00 };
static CVI_U8 data_st7701_40[] = { 0x35, 0x00 };

const struct dsc_instr dsi_init_cmds_st7701_480x800[] = {
	{.delay = 0, .data_type = 0x39, .size = 6, .data = data_st7701_0 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_1 },
	{.delay = 0, .data_type = 0x39, .size = 6, .data = data_st7701_2 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701_3 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701_4 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701_5 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_6 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701_7 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701_8 },
	{.delay = 0, .data_type = 0x39, .size = 6, .data = data_st7701_9 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_10 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_11 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_12 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_13 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_14 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_15 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_16 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701_17 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_18 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_19 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_20 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_21 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_22 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_23 },
	{.delay = 0, .data_type = 0x39, .size = 7, .data = data_st7701_24 },
	{.delay = 0, .data_type = 0x39, .size = 12, .data = data_st7701_25 },
	{.delay = 0, .data_type = 0x39, .size = 13, .data = data_st7701_26 },
	{.delay = 0, .data_type = 0x39, .size = 5, .data = data_st7701_27 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701_28 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701_29 },
	{.delay = 0, .data_type = 0x39, .size = 5, .data = data_st7701_30 },
	{.delay = 0, .data_type = 0x39, .size = 3, .data = data_st7701_31 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701_32 },
	{.delay = 0, .data_type = 0x39, .size = 7, .data = data_st7701_33 },
	{.delay = 0, .data_type = 0x39, .size = 17, .data = data_st7701_34 },
	{.delay = 0, .data_type = 0x39, .size = 7, .data = data_st7701_35 },
	{.delay = 0, .data_type = 0x39, .size = 6, .data = data_st7701_36 },
	{.delay = 120, .data_type = 0x05, .size = 1, .data = data_st7701_37 },
	{.delay = 20, .data_type = 0x05, .size = 1, .data = data_st7701_38 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_39 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_st7701_40 }
};

#else
#error "MIPI_TX_PARAM multi-delcaration!!"
#endif // _MIPI_TX_PARAM_ST_7701_H_
