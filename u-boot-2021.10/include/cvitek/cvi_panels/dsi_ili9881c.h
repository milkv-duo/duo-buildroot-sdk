#ifndef _MIPI_TX_PARAM_ILI9881C_H_
#define _MIPI_TX_PARAM_ILI9881C_H_

#include <cvi_mipi.h>

const struct combo_dev_cfg_s dev_cfg_ili9881c_720x1280 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_CLK, MIPI_TX_LANE_2, MIPI_TX_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 64,
		.vid_hbp_pixels = 36,
		.vid_hfp_pixels = 128,
		.vid_hline_pixels = 720,
		.vid_vsa_lines = 16,
		.vid_vbp_lines = 4,
		.vid_vfp_lines = 6,
		.vid_active_lines = 1280,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 74250,
};

const struct hs_settle_s hs_timing_cfg_ili9881c_720x1280 = { .prepare = 6, .zero = 32, .trail = 1 };

static u8 data_ili9881c_0[] = { 0xff, 0x98, 0x81, 0x03 };
static u8 data_ili9881c_1[] = { 0x01, 0x00 };
static u8 data_ili9881c_2[] = { 0x02, 0x00 };
static u8 data_ili9881c_3[] = { 0x03, 0x73 };
static u8 data_ili9881c_4[] = { 0x04, 0x00 };
static u8 data_ili9881c_5[] = { 0x05, 0x00 };
static u8 data_ili9881c_6[] = { 0x06, 0x0a };
static u8 data_ili9881c_7[] = { 0x07, 0x00 };
static u8 data_ili9881c_8[] = { 0x08, 0x00 };
static u8 data_ili9881c_9[] = { 0x09, 0x01 };
static u8 data_ili9881c_10[] = { 0x0a, 0x00 };
static u8 data_ili9881c_11[] = { 0x0b, 0x00 };
static u8 data_ili9881c_12[] = { 0x0c, 0x01 };
static u8 data_ili9881c_13[] = { 0x0d, 0x00 };
static u8 data_ili9881c_14[] = { 0x0e, 0x00 };
static u8 data_ili9881c_15[] = { 0x0f, 0x1d };
static u8 data_ili9881c_16[] = { 0x10, 0x1d };
static u8 data_ili9881c_17[] = { 0x11, 0x00 };
static u8 data_ili9881c_18[] = { 0x12, 0x00 };
static u8 data_ili9881c_19[] = { 0x13, 0x00 };
static u8 data_ili9881c_20[] = { 0x14, 0x00 };
static u8 data_ili9881c_21[] = { 0x15, 0x00 };
static u8 data_ili9881c_22[] = { 0x16, 0x00 };
static u8 data_ili9881c_23[] = { 0x17, 0x00 };
static u8 data_ili9881c_24[] = { 0x18, 0x00 };
static u8 data_ili9881c_25[] = { 0x19, 0x00 };
static u8 data_ili9881c_26[] = { 0x1a, 0x00 };
static u8 data_ili9881c_27[] = { 0x1b, 0x00 };
static u8 data_ili9881c_28[] = { 0x1c, 0x00 };
static u8 data_ili9881c_29[] = { 0x1d, 0x00 };
static u8 data_ili9881c_30[] = { 0x1e, 0x40 };
static u8 data_ili9881c_31[] = { 0x1f, 0x80 };
static u8 data_ili9881c_32[] = { 0x20, 0x06 };
static u8 data_ili9881c_33[] = { 0x21, 0x02 };
static u8 data_ili9881c_34[] = { 0x22, 0x00 };
static u8 data_ili9881c_35[] = { 0x23, 0x00 };
static u8 data_ili9881c_36[] = { 0x24, 0x00 };
static u8 data_ili9881c_37[] = { 0x25, 0x00 };
static u8 data_ili9881c_38[] = { 0x26, 0x00 };
static u8 data_ili9881c_39[] = { 0x27, 0x00 };
static u8 data_ili9881c_40[] = { 0x28, 0x33 };
static u8 data_ili9881c_41[] = { 0x29, 0x03 };
static u8 data_ili9881c_42[] = { 0x2a, 0x00 };
static u8 data_ili9881c_43[] = { 0x2b, 0x00 };
static u8 data_ili9881c_44[] = { 0x2c, 0x00 };
static u8 data_ili9881c_45[] = { 0x2d, 0x00 };
static u8 data_ili9881c_46[] = { 0x2e, 0x00 };
static u8 data_ili9881c_47[] = { 0x2f, 0x00 };
static u8 data_ili9881c_48[] = { 0x30, 0x00 };
static u8 data_ili9881c_49[] = { 0x31, 0x00 };
static u8 data_ili9881c_50[] = { 0x32, 0x00 };
static u8 data_ili9881c_51[] = { 0x33, 0x00 };
static u8 data_ili9881c_52[] = { 0x34, 0x04 };
static u8 data_ili9881c_53[] = { 0x35, 0x00 };
static u8 data_ili9881c_54[] = { 0x36, 0x00 };
static u8 data_ili9881c_55[] = { 0x37, 0x00 };
static u8 data_ili9881c_56[] = { 0x38, 0x3c };
static u8 data_ili9881c_57[] = { 0x39, 0x00 };
static u8 data_ili9881c_58[] = { 0x3a, 0x40 };
static u8 data_ili9881c_59[] = { 0x3b, 0x40 };
static u8 data_ili9881c_60[] = { 0x3c, 0x00 };
static u8 data_ili9881c_61[] = { 0x3d, 0x00 };
static u8 data_ili9881c_62[] = { 0x3e, 0x00 };
static u8 data_ili9881c_63[] = { 0x3f, 0x00 };
static u8 data_ili9881c_64[] = { 0x40, 0x00 };
static u8 data_ili9881c_65[] = { 0x41, 0x00 };
static u8 data_ili9881c_66[] = { 0x42, 0x00 };
static u8 data_ili9881c_67[] = { 0x43, 0x00 };
static u8 data_ili9881c_68[] = { 0x44, 0x00 };
static u8 data_ili9881c_69[] = { 0x50, 0x01 };
static u8 data_ili9881c_70[] = { 0x51, 0x23 };
static u8 data_ili9881c_71[] = { 0x52, 0x45 };
static u8 data_ili9881c_72[] = { 0x53, 0x67 };
static u8 data_ili9881c_73[] = { 0x54, 0x89 };
static u8 data_ili9881c_74[] = { 0x55, 0xab };
static u8 data_ili9881c_75[] = { 0x56, 0x01 };
static u8 data_ili9881c_76[] = { 0x57, 0x23 };
static u8 data_ili9881c_77[] = { 0x58, 0x45 };
static u8 data_ili9881c_78[] = { 0x59, 0x67 };
static u8 data_ili9881c_79[] = { 0x5a, 0x89 };
static u8 data_ili9881c_80[] = { 0x5b, 0xab };
static u8 data_ili9881c_81[] = { 0x5c, 0xcd };
static u8 data_ili9881c_82[] = { 0x5d, 0xef };
static u8 data_ili9881c_83[] = { 0x5e, 0x11 };
static u8 data_ili9881c_84[] = { 0x5f, 0x01 };
static u8 data_ili9881c_85[] = { 0x60, 0x00 };
static u8 data_ili9881c_86[] = { 0x61, 0x15 };
static u8 data_ili9881c_87[] = { 0x62, 0x14 };
static u8 data_ili9881c_88[] = { 0x63, 0x0e };
static u8 data_ili9881c_89[] = { 0x64, 0x0f };
static u8 data_ili9881c_90[] = { 0x65, 0x0c };
static u8 data_ili9881c_91[] = { 0x66, 0x0d };
static u8 data_ili9881c_92[] = { 0x67, 0x06 };
static u8 data_ili9881c_93[] = { 0x68, 0x02 };
static u8 data_ili9881c_94[] = { 0x69, 0x07 };
static u8 data_ili9881c_95[] = { 0x6a, 0x02 };
static u8 data_ili9881c_96[] = { 0x6b, 0x02 };
static u8 data_ili9881c_97[] = { 0x6c, 0x02 };
static u8 data_ili9881c_98[] = { 0x6d, 0x02 };
static u8 data_ili9881c_99[] = { 0x6e, 0x02 };
static u8 data_ili9881c_100[] = { 0x6f, 0x02 };
static u8 data_ili9881c_101[] = { 0x70, 0x02 };
static u8 data_ili9881c_102[] = { 0x71, 0x02 };
static u8 data_ili9881c_103[] = { 0x72, 0x02 };
static u8 data_ili9881c_104[] = { 0x73, 0x02 };
static u8 data_ili9881c_105[] = { 0x74, 0x02 };
static u8 data_ili9881c_106[] = { 0x75, 0x01 };
static u8 data_ili9881c_107[] = { 0x76, 0x00 };
static u8 data_ili9881c_108[] = { 0x77, 0x14 };
static u8 data_ili9881c_109[] = { 0x78, 0x15 };
static u8 data_ili9881c_110[] = { 0x79, 0x0e };
static u8 data_ili9881c_111[] = { 0x7a, 0x0f };
static u8 data_ili9881c_112[] = { 0x7b, 0x0c };
static u8 data_ili9881c_113[] = { 0x7c, 0x0d };
static u8 data_ili9881c_114[] = { 0x7d, 0x06 };
static u8 data_ili9881c_115[] = { 0x7e, 0x02 };
static u8 data_ili9881c_116[] = { 0x7f, 0x07 };
static u8 data_ili9881c_117[] = { 0x80, 0x02 };
static u8 data_ili9881c_118[] = { 0x81, 0x02 };
static u8 data_ili9881c_119[] = { 0x82, 0x02 };
static u8 data_ili9881c_120[] = { 0x83, 0x02 };
static u8 data_ili9881c_121[] = { 0x84, 0x02 };
static u8 data_ili9881c_122[] = { 0x85, 0x02 };
static u8 data_ili9881c_123[] = { 0x86, 0x02 };
static u8 data_ili9881c_124[] = { 0x87, 0x02 };
static u8 data_ili9881c_125[] = { 0x88, 0x02 };
static u8 data_ili9881c_126[] = { 0x89, 0x02 };
static u8 data_ili9881c_127[] = { 0x8a, 0x02 };
static u8 data_ili9881c_128[] = { 0xff, 0x98, 0x81, 0x04 };
static u8 data_ili9881c_129[] = { 0x6c, 0x15 };
static u8 data_ili9881c_130[] = { 0x6e, 0x2b };
static u8 data_ili9881c_131[] = { 0x6f, 0x33 };
static u8 data_ili9881c_132[] = { 0x8d, 0x18 };
static u8 data_ili9881c_133[] = { 0x87, 0xba };
static u8 data_ili9881c_134[] = { 0x26, 0x76 };
static u8 data_ili9881c_135[] = { 0xb2, 0xd1 };
static u8 data_ili9881c_136[] = { 0xb5, 0x06 };
static u8 data_ili9881c_137[] = { 0x3a, 0x24 };
static u8 data_ili9881c_138[] = { 0x35, 0x1f };
static u8 data_ili9881c_139[] = { 0xff, 0x98, 0x81, 0x01 };
static u8 data_ili9881c_140[] = { 0x22, 0x09 };
static u8 data_ili9881c_141[] = { 0x31, 0x00 };
static u8 data_ili9881c_142[] = { 0x40, 0x33 };
static u8 data_ili9881c_143[] = { 0x53, 0xa2 };
static u8 data_ili9881c_144[] = { 0x55, 0x92 };
static u8 data_ili9881c_145[] = { 0x50, 0x96 };
static u8 data_ili9881c_146[] = { 0x51, 0x96 };
static u8 data_ili9881c_147[] = { 0x60, 0x22 };
static u8 data_ili9881c_148[] = { 0x61, 0x00 };
static u8 data_ili9881c_149[] = { 0x62, 0x19 };
static u8 data_ili9881c_150[] = { 0x63, 0x00 };
static u8 data_ili9881c_151[] = { 0xa0, 0x08 };
static u8 data_ili9881c_152[] = { 0xa1, 0x11 };
static u8 data_ili9881c_153[] = { 0xa2, 0x19 };
static u8 data_ili9881c_154[] = { 0xa3, 0x0d };
static u8 data_ili9881c_155[] = { 0xa4, 0x0d };
static u8 data_ili9881c_156[] = { 0xa5, 0x1e };
static u8 data_ili9881c_157[] = { 0xa6, 0x14 };
static u8 data_ili9881c_158[] = { 0xa7, 0x17 };
static u8 data_ili9881c_159[] = { 0xa8, 0x4f };
static u8 data_ili9881c_160[] = { 0xa9, 0x1a };
static u8 data_ili9881c_161[] = { 0xaa, 0x27 };
static u8 data_ili9881c_162[] = { 0xab, 0x49 };
static u8 data_ili9881c_163[] = { 0xac, 0x1a };
static u8 data_ili9881c_164[] = { 0xad, 0x18 };
static u8 data_ili9881c_165[] = { 0xae, 0x4c };
static u8 data_ili9881c_166[] = { 0xaf, 0x22 };
static u8 data_ili9881c_167[] = { 0xb0, 0x27 };
static u8 data_ili9881c_168[] = { 0xb1, 0x4b };
static u8 data_ili9881c_169[] = { 0xb2, 0x60 };
static u8 data_ili9881c_170[] = { 0xb3, 0x39 };
static u8 data_ili9881c_171[] = { 0xc0, 0x08 };
static u8 data_ili9881c_172[] = { 0xc1, 0x11 };
static u8 data_ili9881c_173[] = { 0xc2, 0x19 };
static u8 data_ili9881c_174[] = { 0xc3, 0x0d };
static u8 data_ili9881c_175[] = { 0xc4, 0x0d };
static u8 data_ili9881c_176[] = { 0xc5, 0x1e };
static u8 data_ili9881c_177[] = { 0xc6, 0x14 };
static u8 data_ili9881c_178[] = { 0xc7, 0x17 };
static u8 data_ili9881c_179[] = { 0xc8, 0x4f };
static u8 data_ili9881c_180[] = { 0xc9, 0x1a };
static u8 data_ili9881c_181[] = { 0xca, 0x27 };
static u8 data_ili9881c_182[] = { 0xcb, 0x49 };
static u8 data_ili9881c_183[] = { 0xcc, 0x1a };
static u8 data_ili9881c_184[] = { 0xcd, 0x18 };
static u8 data_ili9881c_185[] = { 0xce, 0x4c };
static u8 data_ili9881c_186[] = { 0xcf, 0x33 };
static u8 data_ili9881c_187[] = { 0xd0, 0x27 };
static u8 data_ili9881c_188[] = { 0xd1, 0x4b };
static u8 data_ili9881c_189[] = { 0xd2, 0x60 };
static u8 data_ili9881c_190[] = { 0xd3, 0x39 };
static u8 data_ili9881c_191[] = { 0xff, 0x98, 0x81, 0x00 };
static u8 data_ili9881c_192[] = { 0x36 };
static u8 data_ili9881c_193[] = { 0x35 };
static u8 data_ili9881c_194[] = { 0x11 };
static u8 data_ili9881c_195[] = { 0x29 };

const struct dsc_instr dsi_init_cmds_ili9881c_720x1280[] = {
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881c_0 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_1 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_2 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_3 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_4 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_5 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_6 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_7 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_8 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_9 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_10 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_11 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_12 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_13 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_14 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_15 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_16 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_17 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_18 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_19 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_20 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_21 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_22 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_23 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_24 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_25 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_26 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_27 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_28 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_29 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_30 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_31 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_32 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_33 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_34 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_35 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_36 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_37 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_38 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_39 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_40 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_41 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_42 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_43 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_44 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_45 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_46 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_47 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_48 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_49 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_50 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_51 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_52 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_53 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_54 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_55 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_56 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_57 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_58 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_59 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_60 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_61 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_62 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_63 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_64 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_65 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_66 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_67 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_68 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_69 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_70 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_71 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_72 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_73 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_74 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_75 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_76 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_77 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_78 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_79 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_80 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_81 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_82 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_83 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_84 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_85 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_86 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_87 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_88 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_89 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_90 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_91 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_92 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_93 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_94 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_95 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_96 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_97 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_98 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_99 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_100 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_101 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_102 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_103 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_104 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_105 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_106 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_107 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_108 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_109 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_110 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_111 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_112 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_113 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_114 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_115 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_116 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_117 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_118 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_119 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_120 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_121 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_122 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_123 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_124 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_125 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_126 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_127 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881c_128 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_129 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_130 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_131 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_132 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_133 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_134 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_135 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_136 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_137 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_138 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881c_139 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_140 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_141 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_142 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_143 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_144 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_145 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_146 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_147 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_148 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_149 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_150 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_151 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_152 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_153 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_154 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_155 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_156 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_157 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_158 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_159 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_160 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_161 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_162 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_163 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_164 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_165 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_166 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_167 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_168 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_169 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_170 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_171 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_172 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_173 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_174 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_175 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_176 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_177 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_178 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_179 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_180 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_181 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_182 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_183 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_184 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_185 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_186 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_187 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_188 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_189 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881c_190 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881c_191 },
	{.delay = 0, .data_type = 0x05, .size = 1, .data = data_ili9881c_192 },
	{.delay = 0, .data_type = 0x05, .size = 1, .data = data_ili9881c_193 },
	{.delay = 120, .data_type = 0x05, .size = 1, .data = data_ili9881c_194 },
	{.delay = 20, .data_type = 0x05, .size = 1, .data = data_ili9881c_195 }
};

#else
#error "_MIPI_TX_PARAM_ILI9881C_H_ multi-delcaration!!"
#endif // _MIPI_TX_PARAM_ILI9881C_H_
