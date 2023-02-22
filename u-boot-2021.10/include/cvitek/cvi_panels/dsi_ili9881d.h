#ifndef _MIPI_TX_PARAM_ILI9881D_H_
#define _MIPI_TX_PARAM_ILI9881D_H_

#include <cvi_mipi.h>

const struct combo_dev_cfg_s dev_cfg_ili9881d_720x1280 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_2, MIPI_TX_LANE_CLK, MIPI_TX_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 60,
		.vid_hbp_pixels = 60,
		.vid_hfp_pixels = 140,
		.vid_hline_pixels = 720,
		.vid_vsa_lines = 16,
		.vid_vbp_lines = 24,
		.vid_vfp_lines = 8,
		.vid_active_lines = 1280,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 70118,
};

const struct hs_settle_s hs_timing_cfg_ili9881d_720x1280 = { .prepare = 6, .zero = 32, .trail = 1 };

static u8 data_ili9881d_0[] = { 0xff, 0x98, 0x81, 0x01 };
static u8 data_ili9881d_1[] = { 0x91, 0x00 };
static u8 data_ili9881d_2[] = { 0x92, 0x00 };
static u8 data_ili9881d_3[] = { 0x93, 0x72 };
static u8 data_ili9881d_4[] = { 0x94, 0x00 };
static u8 data_ili9881d_5[] = { 0x95, 0x00 };
static u8 data_ili9881d_6[] = { 0x96, 0x09 };
static u8 data_ili9881d_7[] = { 0x97, 0x00 };
static u8 data_ili9881d_8[] = { 0x98, 0x00 };
static u8 data_ili9881d_9[] = { 0x09, 0x01 };
static u8 data_ili9881d_10[] = { 0x0a, 0x00 };
static u8 data_ili9881d_11[] = { 0x0b, 0x00 };
static u8 data_ili9881d_12[] = { 0x0c, 0x01 };
static u8 data_ili9881d_13[] = { 0x0d, 0x00 };
static u8 data_ili9881d_14[] = { 0x0e, 0x00 };
static u8 data_ili9881d_15[] = { 0x0f, 0x00 };
static u8 data_ili9881d_16[] = { 0x10, 0x00 };
static u8 data_ili9881d_17[] = { 0x11, 0x00 };
static u8 data_ili9881d_18[] = { 0x12, 0x00 };
static u8 data_ili9881d_19[] = { 0x13, 0x00 };
static u8 data_ili9881d_20[] = { 0x14, 0x00 };
static u8 data_ili9881d_21[] = { 0x15, 0x00 };
static u8 data_ili9881d_22[] = { 0x16, 0x00 };
static u8 data_ili9881d_23[] = { 0x17, 0x00 };
static u8 data_ili9881d_24[] = { 0x18, 0x00 };
static u8 data_ili9881d_25[] = { 0x19, 0x00 };
static u8 data_ili9881d_26[] = { 0x1a, 0x00 };
static u8 data_ili9881d_27[] = { 0x1b, 0x00 };
static u8 data_ili9881d_28[] = { 0x1c, 0x00 };
static u8 data_ili9881d_29[] = { 0x1d, 0x00 };
static u8 data_ili9881d_30[] = { 0x1e, 0xc0 };
static u8 data_ili9881d_31[] = { 0x1f, 0x40 };
static u8 data_ili9881d_32[] = { 0x20, 0x05 };
static u8 data_ili9881d_33[] = { 0x21, 0x02 };
static u8 data_ili9881d_34[] = { 0x22, 0x00 };
static u8 data_ili9881d_35[] = { 0x23, 0x00 };
static u8 data_ili9881d_36[] = { 0x24, 0x00 };
static u8 data_ili9881d_37[] = { 0x25, 0x00 };
static u8 data_ili9881d_38[] = { 0x26, 0x00 };
static u8 data_ili9881d_39[] = { 0x27, 0x00 };
static u8 data_ili9881d_40[] = { 0x28, 0x33 };
static u8 data_ili9881d_41[] = { 0x29, 0x02 };
static u8 data_ili9881d_42[] = { 0x2a, 0x00 };
static u8 data_ili9881d_43[] = { 0x2b, 0x00 };
static u8 data_ili9881d_44[] = { 0x2c, 0x00 };
static u8 data_ili9881d_45[] = { 0x2d, 0x00 };
static u8 data_ili9881d_46[] = { 0x2e, 0x00 };
static u8 data_ili9881d_47[] = { 0x2f, 0x00 };
static u8 data_ili9881d_48[] = { 0x30, 0x00 };
static u8 data_ili9881d_49[] = { 0x31, 0x00 };
static u8 data_ili9881d_50[] = { 0x32, 0x00 };
static u8 data_ili9881d_51[] = { 0x33, 0x00 };
static u8 data_ili9881d_52[] = { 0x34, 0x04 };
static u8 data_ili9881d_53[] = { 0x35, 0x00 };
static u8 data_ili9881d_54[] = { 0x36, 0x00 };
static u8 data_ili9881d_55[] = { 0x37, 0x00 };
static u8 data_ili9881d_56[] = { 0x38, 0x3c };
static u8 data_ili9881d_57[] = { 0x39, 0x07 };
static u8 data_ili9881d_58[] = { 0x3a, 0x00 };
static u8 data_ili9881d_59[] = { 0x3b, 0x00 };
static u8 data_ili9881d_60[] = { 0x3c, 0x00 };
static u8 data_ili9881d_61[] = { 0x40, 0x03 };
static u8 data_ili9881d_62[] = { 0x41, 0x20 };
static u8 data_ili9881d_63[] = { 0x42, 0x00 };
static u8 data_ili9881d_64[] = { 0x43, 0x40 };
static u8 data_ili9881d_65[] = { 0x44, 0x03 };
static u8 data_ili9881d_66[] = { 0x45, 0x00 };
static u8 data_ili9881d_67[] = { 0x46, 0x01 };
static u8 data_ili9881d_68[] = { 0x47, 0x08 };
static u8 data_ili9881d_69[] = { 0x48, 0x00 };
static u8 data_ili9881d_70[] = { 0x49, 0x00 };
static u8 data_ili9881d_71[] = { 0x4a, 0x00 };
static u8 data_ili9881d_72[] = { 0x4b, 0x00 };
static u8 data_ili9881d_73[] = { 0x4c, 0x01 };
static u8 data_ili9881d_74[] = { 0x4d, 0x45 };
static u8 data_ili9881d_75[] = { 0x4e, 0x9b };
static u8 data_ili9881d_76[] = { 0x4f, 0x57 };
static u8 data_ili9881d_77[] = { 0x50, 0x29 };
static u8 data_ili9881d_78[] = { 0x51, 0x27 };
static u8 data_ili9881d_79[] = { 0x52, 0x22 };
static u8 data_ili9881d_80[] = { 0x53, 0x22 };
static u8 data_ili9881d_81[] = { 0x54, 0x22 };
static u8 data_ili9881d_82[] = { 0x55, 0x22 };
static u8 data_ili9881d_83[] = { 0x56, 0x22 };
static u8 data_ili9881d_84[] = { 0x57, 0x01 };
static u8 data_ili9881d_85[] = { 0x58, 0x45 };
static u8 data_ili9881d_86[] = { 0x59, 0x8a };
static u8 data_ili9881d_87[] = { 0x5a, 0x46 };
static u8 data_ili9881d_88[] = { 0x5b, 0x28 };
static u8 data_ili9881d_89[] = { 0x5c, 0x26 };
static u8 data_ili9881d_90[] = { 0x5d, 0x22 };
static u8 data_ili9881d_91[] = { 0x5e, 0x22 };
static u8 data_ili9881d_92[] = { 0x5f, 0x22 };
static u8 data_ili9881d_93[] = { 0x60, 0x22 };
static u8 data_ili9881d_94[] = { 0x61, 0x22 };
static u8 data_ili9881d_95[] = { 0x62, 0x06 };
static u8 data_ili9881d_96[] = { 0x63, 0x01 };
static u8 data_ili9881d_97[] = { 0x64, 0x00 };
static u8 data_ili9881d_98[] = { 0x65, 0xa4 };
static u8 data_ili9881d_99[] = { 0x66, 0xa5 };
static u8 data_ili9881d_100[] = { 0x67, 0x58 };
static u8 data_ili9881d_101[] = { 0x68, 0x5a };
static u8 data_ili9881d_102[] = { 0x69, 0x54 };
static u8 data_ili9881d_103[] = { 0x6a, 0x56 };
static u8 data_ili9881d_104[] = { 0x6b, 0x06 };
static u8 data_ili9881d_105[] = { 0x6c, 0x02 };
static u8 data_ili9881d_106[] = { 0x6d, 0x08 };
static u8 data_ili9881d_107[] = { 0x6e, 0x02 };
static u8 data_ili9881d_108[] = { 0x6f, 0x02 };
static u8 data_ili9881d_109[] = { 0x70, 0x02 };
static u8 data_ili9881d_110[] = { 0x71, 0x02 };
static u8 data_ili9881d_111[] = { 0x72, 0x02 };
static u8 data_ili9881d_112[] = { 0x73, 0x02 };
static u8 data_ili9881d_113[] = { 0x74, 0x02 };
static u8 data_ili9881d_114[] = { 0x75, 0x02 };
static u8 data_ili9881d_115[] = { 0x76, 0x02 };
static u8 data_ili9881d_116[] = { 0x77, 0x02 };
static u8 data_ili9881d_117[] = { 0x78, 0x02 };
static u8 data_ili9881d_118[] = { 0x79, 0x01 };
static u8 data_ili9881d_119[] = { 0x7a, 0x00 };
static u8 data_ili9881d_120[] = { 0x7b, 0xa4 };
static u8 data_ili9881d_121[] = { 0x7c, 0xa5 };
static u8 data_ili9881d_122[] = { 0x7d, 0x59 };
static u8 data_ili9881d_123[] = { 0x7e, 0x5b };
static u8 data_ili9881d_124[] = { 0x7f, 0x55 };
static u8 data_ili9881d_125[] = { 0x80, 0x57 };
static u8 data_ili9881d_126[] = { 0x81, 0x07 };
static u8 data_ili9881d_127[] = { 0x82, 0x02 };
static u8 data_ili9881d_128[] = { 0x83, 0x09 };
static u8 data_ili9881d_129[] = { 0x84, 0x02 };
static u8 data_ili9881d_130[] = { 0x85, 0x02 };
static u8 data_ili9881d_131[] = { 0x86, 0x02 };
static u8 data_ili9881d_132[] = { 0x87, 0x02 };
static u8 data_ili9881d_133[] = { 0x88, 0x02 };
static u8 data_ili9881d_134[] = { 0x89, 0x02 };
static u8 data_ili9881d_135[] = { 0x8a, 0x02 };
static u8 data_ili9881d_136[] = { 0x8b, 0x02 };
static u8 data_ili9881d_137[] = { 0x8c, 0x02 };
static u8 data_ili9881d_138[] = { 0x8d, 0x02 };
static u8 data_ili9881d_139[] = { 0x8e, 0x02 };
static u8 data_ili9881d_140[] = { 0xa0, 0x35 };
static u8 data_ili9881d_141[] = { 0xa1, 0x00 };
static u8 data_ili9881d_142[] = { 0xa2, 0x00 };
static u8 data_ili9881d_143[] = { 0xa3, 0x00 };
static u8 data_ili9881d_144[] = { 0xa4, 0x00 };
static u8 data_ili9881d_145[] = { 0xa5, 0x00 };
static u8 data_ili9881d_146[] = { 0xa6, 0x00 };
static u8 data_ili9881d_147[] = { 0xa7, 0x00 };
static u8 data_ili9881d_148[] = { 0xa8, 0x00 };
static u8 data_ili9881d_149[] = { 0xa9, 0x00 };
static u8 data_ili9881d_150[] = { 0xaa, 0x00 };
static u8 data_ili9881d_151[] = { 0xab, 0x00 };
static u8 data_ili9881d_152[] = { 0xac, 0x00 };
static u8 data_ili9881d_153[] = { 0xad, 0x00 };
static u8 data_ili9881d_154[] = { 0xae, 0xff };
static u8 data_ili9881d_155[] = { 0xaf, 0x00 };
static u8 data_ili9881d_156[] = { 0xb0, 0x00 };
static u8 data_ili9881d_157[] = { 0xff, 0x98, 0x81, 0x02 };
static u8 data_ili9881d_158[] = {
	0xa0, 0x00, 0x0e, 0x1a, 0x11, 0x13, 0x25, 0x19, 0x1c, 0x6b,
	0x1b, 0x28, 0x66, 0x1b, 0x19, 0x4d, 0x22, 0x27, 0x53, 0x63,
	0x2e
};

static u8 data_ili9881d_159[] = {
	0xc0, 0x00, 0x0e, 0x1a, 0x11, 0x13, 0x25, 0x19, 0x1c, 0x6b,
	0x1b, 0x28, 0x66, 0x1b, 0x19, 0x4d, 0x22, 0x27, 0x53, 0x63,
	0x2e
};

//================GIP code finish ================//
static u8 data_ili9881d_160[] = { 0x18, 0xf4 };
static u8 data_ili9881d_161[] = { 0xff, 0x98, 0x81, 0x04 };
static u8 data_ili9881d_162[] = { 0x5d, 0x8b };
static u8 data_ili9881d_163[] = { 0x5e, 0x8b };
static u8 data_ili9881d_164[] = { 0x60, 0x68 };
static u8 data_ili9881d_165[] = { 0x62, 0x52 };
static u8 data_ili9881d_166[] = { 0x82, 0x38 };
static u8 data_ili9881d_167[] = { 0x84, 0x38 };
static u8 data_ili9881d_168[] = { 0x86, 0x1c };
static u8 data_ili9881d_169[] = { 0x66, 0x04 };
static u8 data_ili9881d_170[] = { 0xc1, 0x70 };
static u8 data_ili9881d_171[] = { 0x70, 0x60 };
static u8 data_ili9881d_172[] = { 0x71, 0x00 };
static u8 data_ili9881d_173[] = { 0x5b, 0x33 };
static u8 data_ili9881d_174[] = { 0x6c, 0x10 };
static u8 data_ili9881d_175[] = { 0x77, 0x03 };
static u8 data_ili9881d_176[] = { 0x7b, 0x02 };
static u8 data_ili9881d_177[] = { 0xff, 0x98, 0x81, 0x01 };
static u8 data_ili9881d_178[] = { 0xf0, 0x00 };
static u8 data_ili9881d_179[] = { 0xf1, 0xc8 };
static u8 data_ili9881d_180[] = { 0xff, 0x98, 0x81, 0x05 };
static u8 data_ili9881d_181[] = { 0x22, 0x3a };
static u8 data_ili9881d_182[] = { 0xff, 0x98, 0x81, 0x00 };
static u8 data_ili9881d_183[] = { 0x35, 0x00 };
static u8 data_ili9881d_184[] = { 0x11, 0x00 };
static u8 data_ili9881d_185[] = { 0x29, 0x00 };

const struct dsc_instr dsi_init_cmds_ili9881d_720x1280[] = {
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881d_0 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_1 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_2 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_3 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_4 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_5 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_6 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_7 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_8 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_9 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_10 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_11 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_12 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_13 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_14 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_15 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_16 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_17 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_18 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_19 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_20 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_21 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_22 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_23 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_24 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_25 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_26 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_27 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_28 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_29 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_30 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_31 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_32 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_33 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_34 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_35 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_36 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_37 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_38 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_39 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_40 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_41 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_42 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_43 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_44 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_45 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_46 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_47 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_48 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_49 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_50 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_51 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_52 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_53 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_54 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_55 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_56 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_57 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_58 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_59 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_60 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_61 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_62 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_63 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_64 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_65 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_66 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_67 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_68 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_69 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_70 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_71 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_72 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_73 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_74 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_75 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_76 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_77 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_78 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_79 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_80 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_81 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_82 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_83 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_84 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_85 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_86 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_87 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_88 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_89 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_90 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_91 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_92 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_93 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_94 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_95 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_96 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_97 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_98 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_99 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_100 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_101 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_102 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_103 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_104 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_105 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_106 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_107 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_108 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_109 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_110 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_111 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_112 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_113 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_114 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_115 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_116 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_117 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_118 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_119 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_120 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_121 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_122 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_123 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_124 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_125 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_126 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_127 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_128 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_129 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_130 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_131 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_132 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_133 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_134 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_135 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_136 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_137 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_138 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_139 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_140 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_141 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_142 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_143 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_144 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_145 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_146 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_147 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_148 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_149 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_150 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_151 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_152 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_153 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_154 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_155 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_156 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881d_157 },
	{.delay = 0, .data_type = 0x29, .size = 21, .data = data_ili9881d_158 },
	{.delay = 0, .data_type = 0x29, .size = 21, .data = data_ili9881d_159 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_160 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881d_161 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_162 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_163 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_164 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_165 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_166 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_167 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_168 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_169 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_170 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_171 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_172 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_173 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_174 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_175 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_176 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881d_177 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_178 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_179 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881d_180 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_181 },
	{.delay = 0, .data_type = 0x29, .size = 4, .data = data_ili9881d_182 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ili9881d_183 },
	{.delay = 120, .data_type = 0x15, .size = 2, .data = data_ili9881d_184 },
	{.delay = 20, .data_type = 0x15, .size = 2, .data = data_ili9881d_185 }
};

#else
#error "_MIPI_TX_PARAM_ILI9881D_H_ multi-delcaration!!"
#endif // _MIPI_TX_PARAM_ILI9881D_H_
