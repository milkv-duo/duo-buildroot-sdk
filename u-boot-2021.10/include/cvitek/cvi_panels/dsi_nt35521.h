#ifndef _MIPI_TX_PARAM_NT35521_H_
#define _MIPI_TX_PARAM_NT35521_H_

#include <cvi_mipi.h>

const struct combo_dev_cfg_s dev_cfg_nt35521_800x1280 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_CLK, MIPI_TX_LANE_0, MIPI_TX_LANE_2, MIPI_TX_LANE_3, MIPI_TX_LANE_1},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 80,
		.vid_hbp_pixels = 80,
		.vid_hfp_pixels = 168,
		.vid_hline_pixels = 800,
		.vid_vsa_lines = 6,
		.vid_vbp_lines = 40,
		.vid_vfp_lines = 18,
		.vid_active_lines = 1280,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 90962,
};

const struct hs_settle_s hs_timing_cfg_nt35521_800x1280 = { .prepare = 6, .zero = 32, .trail = 1 };

//=====================Page 0 relative===================
static u8 data_nt35521_0[] = { 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00 };
static u8 data_nt35521_1[] = { 0xb1, 0x6c, 0x01 };
static u8 data_nt35521_2[] = { 0xb5, 0xc8, 0x00 };
static u8 data_nt35521_3[] = { 0xbc, 0x00 };
static u8 data_nt35521_4[] = { 0xbd, 0x96, 0xb0, 0x0c, 0x08, 0x01 };
static u8 data_nt35521_5[] = { 0xc8, 0x80 };
//=====================Page 1 relative===================
static u8 data_nt35521_6[] = { 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x01 };
static u8 data_nt35521_7[] = { 0xb3, 0x26 };
static u8 data_nt35521_8[] = { 0xb4, 0x0f };
static u8 data_nt35521_9[] = { 0xbb, 0x15 };
static u8 data_nt35521_10[] = { 0xbc, 0xa8 };
static u8 data_nt35521_11[] = { 0xbd, 0xa8 };
static u8 data_nt35521_12[] = { 0xbe, 0x28 };
//=====================Page 2 relative===================
static u8 data_nt35521_13[] = { 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x02 };
static u8 data_nt35521_14[] = { 0xee, 0x01 };
static u8 data_nt35521_15[] = {
	0xb0, 0x00, 0x00, 0x00, 0x13, 0x00, 0x33, 0x00, 0x4f, 0x00,
	0x65, 0x00, 0x79, 0x00, 0x8b, 0x00, 0x9c
};

static u8 data_nt35521_16[] = {
	0xb1, 0x00, 0xac, 0x00, 0xe0, 0x01, 0x09, 0x01, 0x4b, 0x01,
	0x80, 0x01, 0xd1, 0x02, 0x13, 0x02, 0x14
};

static u8 data_nt35521_17[] = {
	0xb2, 0x02, 0x50, 0x02, 0x91, 0x02, 0xba, 0x02, 0xf0, 0x03,
	0x12, 0x03, 0x3d, 0x03, 0x4a, 0x03, 0x57
};

static u8 data_nt35521_18[] = {
	0xb3, 0x03, 0x5f, 0x03, 0x74, 0x03, 0x8f, 0x03, 0xb8, 0x03,
	0xfc, 0x03, 0xff
};

static u8 data_nt35521_19[] = {
	0xe9, 0x52, 0x60, 0x63, 0x11, 0x21, 0x52, 0x60, 0x63, 0x11,
	0x21
};

static u8 data_nt35521_20[] = {
	0xea, 0x52, 0x60, 0x63, 0x11, 0x21, 0x52, 0x60, 0x63, 0x11,
	0x21
};

static u8 data_nt35521_21[] = {
	0xeb, 0x52, 0x60, 0x63, 0x11, 0x21, 0x52, 0x60, 0x63, 0x11,
	0x21
};

//=====================Page 3 relative===================
static u8 data_nt35521_22[] = { 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x03 };
static u8 data_nt35521_23[] = { 0xb2, 0x00, 0x0b, 0x08, 0x00, 0x00 };
static u8 data_nt35521_24[] = { 0xb3, 0x00, 0x09, 0x06, 0x00, 0x00 };
static u8 data_nt35521_25[] = { 0xba, 0x44, 0x00, 0x00, 0x10 };
static u8 data_nt35521_26[] = { 0xc0, 0x00, 0x34, 0x00 };
static u8 data_nt35521_27[] = { 0xc1, 0x00, 0x00, 0x34 };
//=====================Page 4 relative===================
static u8 data_nt35521_28[] = { 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x04 };
static u8 data_nt35521_29[] = { 0xb1, 0x03, 0x02, 0x00, 0x15, 0x16 };
//=====================Page 5 relative===================
static u8 data_nt35521_30[] = { 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x05 };
static u8 data_nt35521_31[] = { 0xb0, 0x06 };
static u8 data_nt35521_32[] = { 0xb2, 0x06, 0x00 };
static u8 data_nt35521_33[] = { 0xb3, 0x0e, 0x00, 0x00, 0x00, 0x00 };
static u8 data_nt35521_34[] = { 0xb4, 0x06, 0x00, 0x00, 0x00, 0x00 };
static u8 data_nt35521_35[] = { 0xb7, 0x06, 0x00, 0x00 };
static u8 data_nt35521_36[] = { 0xbc, 0x00, 0x00, 0x00, 0x02 };
static u8 data_nt35521_37[] = { 0xbd, 0x01, 0x03, 0x00, 0x03, 0x03 };
static u8 data_nt35521_38[] = { 0xc0, 0x07, 0x70 };
static u8 data_nt35521_39[] = { 0xc4, 0x00, 0x00, 0x3c };
static u8 data_nt35521_40[] = { 0xc5, 0x00, 0x00, 0x3c };
static u8 data_nt35521_41[] = { 0xd1, 0x00, 0x05, 0x01, 0x00, 0x00 };
static u8 data_nt35521_42[] = { 0xe3, 0x84 };
static u8 data_nt35521_43[] = { 0xe5, 0x1a };
static u8 data_nt35521_44[] = { 0xe6, 0x1a };
static u8 data_nt35521_45[] = { 0xe8, 0x1a };
static u8 data_nt35521_46[] = { 0xe9, 0x1a };
static u8 data_nt35521_47[] = { 0xea, 0x1a };
//=====================Page 6 relative===================
static u8 data_nt35521_48[] = { 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x06 };
static u8 data_nt35521_49[] = { 0xb0, 0x30, 0x31, 0x2c, 0x2d, 0x14 };
static u8 data_nt35521_50[] = { 0xb1, 0x16, 0x10, 0x12, 0x00, 0x35 };
static u8 data_nt35521_51[] = { 0xb2, 0x35, 0x35, 0x35, 0x02, 0x31 };
static u8 data_nt35521_52[] = { 0xb3, 0x31, 0x31, 0x35, 0x35, 0x35 };
static u8 data_nt35521_53[] = { 0xb4, 0x35, 0x35, 0x35, 0x31, 0x31 };
static u8 data_nt35521_54[] = { 0xb5, 0x31, 0x03, 0x35, 0x35, 0x35 };
static u8 data_nt35521_55[] = { 0xb6, 0x35, 0x01, 0x13, 0x11, 0x17 };
static u8 data_nt35521_56[] = { 0xb7, 0x15, 0x2d, 0x2c, 0x31, 0x30 };
static u8 data_nt35521_57[] = { 0xf0, 0x55, 0xaa, 0x52, 0x00, 0x00 };
static u8 data_nt35521_58[] = { 0x35, 0x00 };
static u8 data_nt35521_59[] = { 0x11 };
static u8 data_nt35521_60[] = { 0x29 };
#ifdef _MIPI_TX_BIST_MODE
//=====================BIST relative===================
static u8 data_nt35521_61[] = { 0x10 };
static u8 data_nt35521_62[] = { 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x00 };
static u8 data_nt35521_63[] = { 0xEE, 0x87, 0x78, 0xff, 0xff };
#endif

const struct dsc_instr dsi_init_cmds_nt35521_800x1280[] = {
#ifdef _MIPI_TX_BIST_MODE
	//=====================BIST relative===================
	{ .delay = 0, .data_type = 0x05, .size = 1, .data = data_nt35521_61 },
	{ .delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_62 },
	{ .delay = 0, .data_type = 0x29, .size = 5, .data = data_nt35521_63 },
#else
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_0 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_nt35521_1 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_nt35521_2 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_3 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_4 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_5 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_6 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_7 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_8 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_9 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_10 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_11 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_12 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_13 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_14 },
	{.delay = 0, .data_type = 0x29, .size = 17, .data = data_nt35521_15 },
	{.delay = 0, .data_type = 0x29, .size = 17, .data = data_nt35521_16 },
	{.delay = 0, .data_type = 0x29, .size = 17, .data = data_nt35521_17 },
	{.delay = 0, .data_type = 0x29, .size = 13, .data = data_nt35521_18 },
	{.delay = 0, .data_type = 0x29, .size = 11, .data = data_nt35521_19 },
	{.delay = 0, .data_type = 0x29, .size = 11, .data = data_nt35521_20 },
	{.delay = 0, .data_type = 0x29, .size = 11, .data = data_nt35521_21 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_22 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_23 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_24 },
	{.delay = 0, .data_type = 0x29, .size = 5, .data = data_nt35521_25 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_nt35521_26 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_nt35521_27 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_28 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_29 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_30 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_31 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_nt35521_32 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_33 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_34 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_nt35521_35 },
	{.delay = 0, .data_type = 0x29, .size = 5, .data = data_nt35521_36 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_37 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_nt35521_38 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_nt35521_39 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_nt35521_40 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_41 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_42 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_43 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_44 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_45 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_46 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_47 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_48 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_49 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_50 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_51 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_52 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_53 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_54 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_55 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_56 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_nt35521_57 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_nt35521_58 },
	{.delay = 120, .data_type = 0x05, .size = 1, .data = data_nt35521_59 },
	{.delay = 20, .data_type = 0x05, .size = 1, .data = data_nt35521_60 }
#endif
};

#else
#error "_MIPI_TX_PARAM_NT35521_H_ multi-delcaration!!"
#endif // _MIPI_TX_PARAM_NT35521_H_
