#ifndef _MIPI_TX_PARAM__3AML069LP01G_H_
#define _MIPI_TX_PARAM__3AML069LP01G_H_

#include <cvi_mipi.h>

#define _3AML069LP01G_RX_VACT        1024
#define _3AML069LP01G_RX_VSA         3
#define _3AML069LP01G_RX_VBP         5
#define _3AML069LP01G_RX_VFP         7

#define _3AML069LP01G_RX_HACT        600
#define _3AML069LP01G_RX_HSA         20
#define _3AML069LP01G_RX_HBP         20
#define _3AML069LP01G_RX_HFP         90

#define PIXEL_CLK(x) ((x##_VACT + x##_VSA + x##_VBP + x##_VFP) \
	* (x##_HACT + x##_HSA + x##_HBP + x##_HFP) * 60 / 1000)

const struct combo_dev_cfg_s dev_cfg_3AML069LP01G_600x1024 = {
	.devno = 0,
#ifdef MIPI_PANEL_2_LANES
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_CLK, -1, -1},
#else
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_CLK, MIPI_TX_LANE_2, MIPI_TX_LANE_3},
#endif
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = _3AML069LP01G_RX_HSA,
		.vid_hbp_pixels = _3AML069LP01G_RX_HBP,
		.vid_hfp_pixels = _3AML069LP01G_RX_HFP,
		.vid_hline_pixels = _3AML069LP01G_RX_HACT,
		.vid_vsa_lines = _3AML069LP01G_RX_VSA,
		.vid_vbp_lines = _3AML069LP01G_RX_VBP,
		.vid_vfp_lines = _3AML069LP01G_RX_VFP,
		.vid_active_lines = _3AML069LP01G_RX_VACT,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = PIXEL_CLK(_3AML069LP01G_RX),
};

const struct hs_settle_s hs_timing_cfg_3AML069LP01G_600x1024 = { .prepare = 6, .zero = 32, .trail = 1 };

static u8 data_3aml069lp01g_0[] = { 0xee, 0x50 };
static u8 data_3aml069lp01g_1[] = { 0xea, 0x85, 0x55 };
static u8 data_3aml069lp01g_2[] = { 0x24, 0x20 };
static u8 data_3aml069lp01g_3[] = { 0x30, 0x00 };
static u8 data_3aml069lp01g_4[] = { 0x39, 0x02, 0x07, 0x10 };
static u8 data_3aml069lp01g_5[] = { 0x79, 0x00 };
static u8 data_3aml069lp01g_6[] = { 0x7b, 0x00 };
static u8 data_3aml069lp01g_7[] = { 0x7a, 0x00 };
static u8 data_3aml069lp01g_8[] = { 0x90, 0x50, 0xc0 };
static u8 data_3aml069lp01g_9[] = { 0x93, 0x80 };
static u8 data_3aml069lp01g_10[] = { 0x95, 0x74 };
static u8 data_3aml069lp01g_11[] = { 0x97, 0x37 };
static u8 data_3aml069lp01g_12[] = { 0x99, 0x00 };
static u8 data_3aml069lp01g_13[] = { 0x56, 0x83 };
static u8 data_3aml069lp01g_14[] = { 0x33, 0x83 };
static u8 data_3aml069lp01g_15[] = { 0x34, 0x3f };
static u8 data_3aml069lp01g_16[] = { 0xee, 0x60 };
static u8 data_3aml069lp01g_17[] = { 0x30, 0x03 };
static u8 data_3aml069lp01g_18[] = { 0x32, 0xd9 };
static u8 data_3aml069lp01g_19[] = { 0x3b, 0x00 };
static u8 data_3aml069lp01g_20[] = { 0x3c, 0x07 };
static u8 data_3aml069lp01g_21[] = { 0x3d, 0x11 };
static u8 data_3aml069lp01g_22[] = { 0x3e, 0x94 };
static u8 data_3aml069lp01g_23[] = { 0x42, 0x55 };
static u8 data_3aml069lp01g_24[] = { 0x43, 0x55 };
static u8 data_3aml069lp01g_25[] = { 0x86, 0x20 };
static u8 data_3aml069lp01g_26[] = { 0x8b, 0x90 };
static u8 data_3aml069lp01g_27[] = { 0x8d, 0x40 };
static u8 data_3aml069lp01g_28[] = { 0x91, 0x11 };
static u8 data_3aml069lp01g_29[] = { 0x92, 0x11 };
static u8 data_3aml069lp01g_30[] = { 0x93, 0x9f };
static u8 data_3aml069lp01g_31[] = { 0x9a, 0x07 };
static u8 data_3aml069lp01g_32[] = { 0x9b, 0x02, 0x00 };
static u8 data_3aml069lp01g_33[] = { 0x47, 0x05, 0x1e, 0x2f, 0x39, 0x40 };
static u8 data_3aml069lp01g_34[] = { 0x5a, 0x05, 0x1e, 0x2f, 0x39, 0x40 };
static u8 data_3aml069lp01g_35[] = { 0x4c, 0x53, 0x4a, 0x5d, 0x40, 0x40 };
static u8 data_3aml069lp01g_36[] = { 0x5f, 0x53, 0x4a, 0x5d, 0x40, 0x40 };
static u8 data_3aml069lp01g_37[] = { 0x51, 0x42, 0x29, 0x3e, 0x3d, 0x48 };
static u8 data_3aml069lp01g_38[] = { 0x64, 0x42, 0x29, 0x3e, 0x3d, 0x48 };
static u8 data_3aml069lp01g_39[] = { 0x56, 0x4c, 0x57, 0x66, 0x7f };
static u8 data_3aml069lp01g_40[] = { 0x56, 0x4c, 0x57, 0x66, 0x7f };
static u8 data_3aml069lp01g_41[] = { 0xee, 0x70 };
static u8 data_3aml069lp01g_42[] = { 0x00, 0x01, 0x04, 0x00, 0x01 };
static u8 data_3aml069lp01g_43[] = { 0x04, 0x06, 0x09, 0x44, 0x01 };
static u8 data_3aml069lp01g_44[] = { 0x0c, 0x05, 0x2d };
static u8 data_3aml069lp01g_45[] = { 0x10, 0x05, 0x09, 0x00, 0x00, 0x00 };
static u8 data_3aml069lp01g_46[] = { 0x15, 0x00, 0x19, 0x0c, 0x08, 0x00 };
static u8 data_3aml069lp01g_47[] = { 0x20, 0x01, 0x05, 0x00, 0x00, 0x00 };
static u8 data_3aml069lp01g_48[] = { 0x25, 0x00, 0x15, 0x0c, 0x07, 0x00 };
static u8 data_3aml069lp01g_49[] = { 0x29, 0x05, 0x2d };
static u8 data_3aml069lp01g_50[] = { 0x45, 0x01 };
static u8 data_3aml069lp01g_51[] = { 0x46, 0xff, 0x00, 0x00, 0x00, 0x50 };
static u8 data_3aml069lp01g_52[] = { 0x4b, 0x88 };
static u8 data_3aml069lp01g_53[] = { 0x60, 0x3c, 0x05, 0x07, 0x19, 0x1d };
static u8 data_3aml069lp01g_54[] = { 0x65, 0x1b, 0x1f, 0x11, 0x11, 0x3c };
static u8 data_3aml069lp01g_55[] = { 0x6a, 0x3c, 0x3c, 0x3c, 0x15, 0x15 };
static u8 data_3aml069lp01g_56[] = { 0x6f, 0x13, 0x13, 0x17, 0x17, 0x01 };
static u8 data_3aml069lp01g_57[] = { 0x74, 0x03, 0x3c };
static u8 data_3aml069lp01g_58[] = { 0x80, 0x3c, 0x04, 0x06, 0x18, 0x1c };
static u8 data_3aml069lp01g_59[] = { 0x85, 0x1a, 0x1e, 0x10, 0x10, 0x3c };
static u8 data_3aml069lp01g_60[] = { 0x8a, 0x3c, 0x3c, 0x3c, 0x14, 0x14 };
static u8 data_3aml069lp01g_61[] = { 0x8f, 0x12, 0x12, 0x16, 0x16, 0x00 };
static u8 data_3aml069lp01g_62[] = { 0x94, 0x02, 0x3c };
static u8 data_3aml069lp01g_63[] = { 0xea, 0x00, 0x00 };
static u8 data_3aml069lp01g_64[] = { 0xee, 0x00 };
static u8 data_3aml069lp01g_65[] = { 0x11 };
static u8 data_3aml069lp01g_66[] = { 0x29 };
#ifdef _MIPI_TX_BIST_MODE
static u8 data_3aml069lp01g_67[] = { 0xee, 0x60 };
static u8 data_3aml069lp01g_68[] = { 0xea, 0x7a, 0xaa };

static u8 data_3aml069lp01g_69[] = { 0x21, 0x10 };
static u8 data_3aml069lp01g_70[] = { 0x11 };
static u8 data_3aml069lp01g_71[] = { 0x29 };
#endif
const struct dsc_instr dsi_init_cmds_3AML069LP01G_600x1024[] = {
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_0 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_3aml069lp01g_1 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_2 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_3 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_3aml069lp01g_4 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_5 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_6 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_7 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_3aml069lp01g_8 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_9 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_10 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_11 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_12 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_13 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_14 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_15 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_16 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_17 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_18 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_19 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_20 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_21 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_22 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_23 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_24 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_25 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_26 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_27 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_28 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_29 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_30 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_31 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_3aml069lp01g_32 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_33 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_34 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_35 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_36 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_37 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_38 },
	{.delay = 0, .data_type = 0x29, .size = 5, .data = data_3aml069lp01g_39 },
	{.delay = 0, .data_type = 0x29, .size = 5, .data = data_3aml069lp01g_40 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_41 },
	{.delay = 0, .data_type = 0x29, .size = 5, .data = data_3aml069lp01g_42 },
	{.delay = 0, .data_type = 0x29, .size = 5, .data = data_3aml069lp01g_43 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_3aml069lp01g_44 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_45 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_46 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_47 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_48 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_3aml069lp01g_49 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_50 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_51 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_52 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_53 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_54 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_55 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_56 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_3aml069lp01g_57 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_58 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_59 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_60 },
	{.delay = 0, .data_type = 0x29, .size = 6, .data = data_3aml069lp01g_61 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_3aml069lp01g_62 },
	{.delay = 0, .data_type = 0x29, .size = 3, .data = data_3aml069lp01g_63 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_64 },
	{.delay = 120, .data_type = 0x05, .size = 1, .data = data_3aml069lp01g_65 },
	{.delay = 120, .data_type = 0x05, .size = 1, .data = data_3aml069lp01g_66 }
#ifdef _MIPI_TX_BIST_MODE
	{ .delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_67 },
	{ .delay = 200, .data_type = 0x29, .size = 3, .data = data_3aml069lp01g_68 },

	{ .delay = 0, .data_type = 0x15, .size = 2, .data = data_3aml069lp01g_69 },
	{ .delay = 40, .data_type = 0x05, .size = 1, .data = data_3aml069lp01g_70 },
	{ .delay = 1, .data_type = 0x05, .size = 1, .data = data_3aml069lp01g_71 },
#endif
};

#else
#error "_MIPI_TX_PARAM__3AML069LP01G_H_ multi-delcaration!!"
#endif // _MIPI_TX_PARAM_H_
