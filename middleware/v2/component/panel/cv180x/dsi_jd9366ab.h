#ifndef _MIPI_TX_PARAM_JD9366AB_H_
#define _MIPI_TX_PARAM_JD9366AB_H_

#include <linux/vo_mipi_tx.h>
#include <linux/cvi_comm_mipi_tx.h>

struct combo_dev_cfg_s dev_cfg_jd9366ab_800x1280 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_CLK, MIPI_TX_LANE_3, MIPI_TX_LANE_2},
	.lane_pn_swap = {true, true, true, true, true},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 6,
		.vid_hbp_pixels = 40,
		.vid_hfp_pixels = 112,
		.vid_hline_pixels = 800,
		.vid_vsa_lines = 4,
		.vid_vbp_lines = 15,
		.vid_vfp_lines = 21,
		.vid_active_lines = 1280,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = false,
	},
	.pixel_clk = 70000,
};

const struct hs_settle_s hs_timing_cfg_jd9366ab_800x1280 = { .prepare = 6, .zero = 16, .trail = 1 };

static CVI_U8 data_jd9366ab_0[] = { 0xe0, 0x00 };
static CVI_U8 data_jd9366ab_1[] = { 0xe1, 0x93 };
static CVI_U8 data_jd9366ab_2[] = { 0xe2, 0x65 };
static CVI_U8 data_jd9366ab_3[] = { 0xe3, 0xf8 };
static CVI_U8 data_jd9366ab_4[] = { 0x80, 0x03 };
static CVI_U8 data_jd9366ab_5[] = { 0xe0, 0x04 };
static CVI_U8 data_jd9366ab_6[] = { 0x2d, 0x03 };
static CVI_U8 data_jd9366ab_7[] = { 0xe0, 0x01 };
static CVI_U8 data_jd9366ab_8[] = { 0x00, 0x00 };
static CVI_U8 data_jd9366ab_9[] = { 0x01, 0xb7 };
static CVI_U8 data_jd9366ab_10[] = { 0x17, 0x00 };
static CVI_U8 data_jd9366ab_11[] = { 0x18, 0xcf };
static CVI_U8 data_jd9366ab_12[] = { 0x19, 0x01 };
static CVI_U8 data_jd9366ab_13[] = { 0x1a, 0x00 };
static CVI_U8 data_jd9366ab_14[] = { 0x1b, 0xcf };
static CVI_U8 data_jd9366ab_15[] = { 0x1c, 0x01 };
static CVI_U8 data_jd9366ab_16[] = { 0x1f, 0x3e };
static CVI_U8 data_jd9366ab_17[] = { 0x20, 0x28 };
static CVI_U8 data_jd9366ab_18[] = { 0x21, 0x28 };
static CVI_U8 data_jd9366ab_19[] = { 0x22, 0x0e };
static CVI_U8 data_jd9366ab_20[] = { 0x24, 0xc8 };
static CVI_U8 data_jd9366ab_21[] = { 0x26, 0xf1 };
static CVI_U8 data_jd9366ab_22[] = { 0x37, 0x29 };
static CVI_U8 data_jd9366ab_23[] = { 0x38, 0x05 };
static CVI_U8 data_jd9366ab_24[] = { 0x39, 0x08 };
static CVI_U8 data_jd9366ab_25[] = { 0x3a, 0x12 };
static CVI_U8 data_jd9366ab_26[] = { 0x3c, 0x78 };
static CVI_U8 data_jd9366ab_27[] = { 0x3d, 0xff };
static CVI_U8 data_jd9366ab_28[] = { 0x3e, 0xff };
static CVI_U8 data_jd9366ab_29[] = { 0x3f, 0xff };
static CVI_U8 data_jd9366ab_30[] = { 0x40, 0x06 };
static CVI_U8 data_jd9366ab_31[] = { 0x41, 0xa0 };
static CVI_U8 data_jd9366ab_32[] = { 0x43, 0x15 };
static CVI_U8 data_jd9366ab_33[] = { 0x44, 0x12 };
static CVI_U8 data_jd9366ab_34[] = { 0x45, 0x50 };
static CVI_U8 data_jd9366ab_35[] = { 0x4b, 0x04 };
static CVI_U8 data_jd9366ab_36[] = { 0x55, 0x0f };
static CVI_U8 data_jd9366ab_37[] = { 0x56, 0x01 };
static CVI_U8 data_jd9366ab_38[] = { 0x57, 0x89 };
static CVI_U8 data_jd9366ab_39[] = { 0x58, 0x0a };
static CVI_U8 data_jd9366ab_40[] = { 0x59, 0x2a };
static CVI_U8 data_jd9366ab_41[] = { 0x5a, 0x31 };
static CVI_U8 data_jd9366ab_42[] = { 0x5b, 0x15 };
static CVI_U8 data_jd9366ab_43[] = { 0x5d, 0x7c };
static CVI_U8 data_jd9366ab_44[] = { 0x5e, 0x50 };
static CVI_U8 data_jd9366ab_45[] = { 0x5f, 0x3b };
static CVI_U8 data_jd9366ab_46[] = { 0x60, 0x2b };
static CVI_U8 data_jd9366ab_47[] = { 0x61, 0x25 };
static CVI_U8 data_jd9366ab_48[] = { 0x62, 0x15 };
static CVI_U8 data_jd9366ab_49[] = { 0x63, 0x1a };
static CVI_U8 data_jd9366ab_50[] = { 0x64, 0x04 };
static CVI_U8 data_jd9366ab_51[] = { 0x65, 0x1c };
static CVI_U8 data_jd9366ab_52[] = { 0x66, 0x1a };
static CVI_U8 data_jd9366ab_53[] = { 0x67, 0x19 };
static CVI_U8 data_jd9366ab_54[] = { 0x68, 0x36 };
static CVI_U8 data_jd9366ab_55[] = { 0x69, 0x27 };
static CVI_U8 data_jd9366ab_56[] = { 0x6a, 0x2f };
static CVI_U8 data_jd9366ab_57[] = { 0x6b, 0x23 };
static CVI_U8 data_jd9366ab_58[] = { 0x6c, 0x21 };
static CVI_U8 data_jd9366ab_59[] = { 0x6d, 0x17 };
static CVI_U8 data_jd9366ab_60[] = { 0x6e, 0x05 };
static CVI_U8 data_jd9366ab_61[] = { 0x6f, 0x00 };
static CVI_U8 data_jd9366ab_62[] = { 0x70, 0x7c };
static CVI_U8 data_jd9366ab_63[] = { 0x71, 0x50 };
static CVI_U8 data_jd9366ab_64[] = { 0x72, 0x3b };
static CVI_U8 data_jd9366ab_65[] = { 0x73, 0x2b };
static CVI_U8 data_jd9366ab_66[] = { 0x74, 0x25 };
static CVI_U8 data_jd9366ab_67[] = { 0x75, 0x15 };
static CVI_U8 data_jd9366ab_68[] = { 0x76, 0x1a };
static CVI_U8 data_jd9366ab_69[] = { 0x77, 0x04 };
static CVI_U8 data_jd9366ab_70[] = { 0x78, 0x1c };
static CVI_U8 data_jd9366ab_71[] = { 0x79, 0x1a };
static CVI_U8 data_jd9366ab_72[] = { 0x7a, 0x19 };
static CVI_U8 data_jd9366ab_73[] = { 0x7b, 0x36 };
static CVI_U8 data_jd9366ab_74[] = { 0x7c, 0x27 };
static CVI_U8 data_jd9366ab_75[] = { 0x7d, 0x2f };
static CVI_U8 data_jd9366ab_76[] = { 0x7e, 0x23 };
static CVI_U8 data_jd9366ab_77[] = { 0x7f, 0x21 };
static CVI_U8 data_jd9366ab_78[] = { 0x80, 0x17 };
static CVI_U8 data_jd9366ab_79[] = { 0x81, 0x05 };
static CVI_U8 data_jd9366ab_80[] = { 0x82, 0x00 };
static CVI_U8 data_jd9366ab_81[] = { 0xe0, 0x02 };
static CVI_U8 data_jd9366ab_82[] = { 0x00, 0x00 };
static CVI_U8 data_jd9366ab_83[] = { 0x01, 0x04 };
static CVI_U8 data_jd9366ab_84[] = { 0x02, 0x08 };
static CVI_U8 data_jd9366ab_85[] = { 0x03, 0x05 };
static CVI_U8 data_jd9366ab_86[] = { 0x04, 0x09 };
static CVI_U8 data_jd9366ab_87[] = { 0x05, 0x06 };
static CVI_U8 data_jd9366ab_88[] = { 0x06, 0x0a };
static CVI_U8 data_jd9366ab_89[] = { 0x07, 0x07 };
static CVI_U8 data_jd9366ab_90[] = { 0x08, 0x0b };
static CVI_U8 data_jd9366ab_91[] = { 0x09, 0x1f };
static CVI_U8 data_jd9366ab_92[] = { 0x0a, 0x1f };
static CVI_U8 data_jd9366ab_93[] = { 0x0b, 0x1f };
static CVI_U8 data_jd9366ab_94[] = { 0x0c, 0x1f };
static CVI_U8 data_jd9366ab_95[] = { 0x0d, 0x1f };
static CVI_U8 data_jd9366ab_96[] = { 0x0e, 0x1f };
static CVI_U8 data_jd9366ab_97[] = { 0x0f, 0x17 };
static CVI_U8 data_jd9366ab_98[] = { 0x10, 0x37 };
static CVI_U8 data_jd9366ab_99[] = { 0x11, 0x10 };
static CVI_U8 data_jd9366ab_100[] = { 0x12, 0x1f };
static CVI_U8 data_jd9366ab_101[] = { 0x13, 0x1f };
static CVI_U8 data_jd9366ab_102[] = { 0x14, 0x1f };
static CVI_U8 data_jd9366ab_103[] = { 0x15, 0x1f };
static CVI_U8 data_jd9366ab_104[] = { 0x16, 0x00 };
static CVI_U8 data_jd9366ab_105[] = { 0x17, 0x04 };
static CVI_U8 data_jd9366ab_106[] = { 0x18, 0x08 };
static CVI_U8 data_jd9366ab_107[] = { 0x19, 0x05 };
static CVI_U8 data_jd9366ab_108[] = { 0x1a, 0x09 };
static CVI_U8 data_jd9366ab_109[] = { 0x1b, 0x06 };
static CVI_U8 data_jd9366ab_110[] = { 0x1c, 0x0a };
static CVI_U8 data_jd9366ab_111[] = { 0x1d, 0x07 };
static CVI_U8 data_jd9366ab_112[] = { 0x1e, 0x0b };
static CVI_U8 data_jd9366ab_113[] = { 0x1f, 0x1f };
static CVI_U8 data_jd9366ab_114[] = { 0x20, 0x1f };
static CVI_U8 data_jd9366ab_115[] = { 0x21, 0x1f };
static CVI_U8 data_jd9366ab_116[] = { 0x22, 0x1f };
static CVI_U8 data_jd9366ab_117[] = { 0x23, 0x1f };
static CVI_U8 data_jd9366ab_118[] = { 0x24, 0x1f };
static CVI_U8 data_jd9366ab_119[] = { 0x25, 0x17 };
static CVI_U8 data_jd9366ab_120[] = { 0x26, 0x37 };
static CVI_U8 data_jd9366ab_121[] = { 0x27, 0x10 };
static CVI_U8 data_jd9366ab_122[] = { 0x28, 0x1f };
static CVI_U8 data_jd9366ab_123[] = { 0x29, 0x1f };
static CVI_U8 data_jd9366ab_124[] = { 0x2a, 0x1f };
static CVI_U8 data_jd9366ab_125[] = { 0x2b, 0x1f };
static CVI_U8 data_jd9366ab_126[] = { 0x58, 0x01 };
static CVI_U8 data_jd9366ab_127[] = { 0x59, 0x00 };
static CVI_U8 data_jd9366ab_128[] = { 0x5a, 0x00 };
static CVI_U8 data_jd9366ab_129[] = { 0x5b, 0x00 };
static CVI_U8 data_jd9366ab_130[] = { 0x5c, 0x0c };
static CVI_U8 data_jd9366ab_131[] = { 0x5d, 0x60 };
static CVI_U8 data_jd9366ab_132[] = { 0x5e, 0x00 };
static CVI_U8 data_jd9366ab_133[] = { 0x5f, 0x00 };
static CVI_U8 data_jd9366ab_134[] = { 0x60, 0x30 };
static CVI_U8 data_jd9366ab_135[] = { 0x61, 0x00 };
static CVI_U8 data_jd9366ab_136[] = { 0x62, 0x00 };
static CVI_U8 data_jd9366ab_137[] = { 0x63, 0x03 };
static CVI_U8 data_jd9366ab_138[] = { 0x64, 0x6a };
static CVI_U8 data_jd9366ab_139[] = { 0x65, 0x45 };
static CVI_U8 data_jd9366ab_140[] = { 0x66, 0x14 };
static CVI_U8 data_jd9366ab_141[] = { 0x67, 0x73 };
static CVI_U8 data_jd9366ab_142[] = { 0x68, 0x10 };
static CVI_U8 data_jd9366ab_143[] = { 0x69, 0x06 };
static CVI_U8 data_jd9366ab_144[] = { 0x6a, 0x6a };
static CVI_U8 data_jd9366ab_145[] = { 0x6b, 0x00 };
static CVI_U8 data_jd9366ab_146[] = { 0x6c, 0x00 };
static CVI_U8 data_jd9366ab_147[] = { 0x6d, 0x03 };
static CVI_U8 data_jd9366ab_148[] = { 0x6e, 0x00 };
static CVI_U8 data_jd9366ab_149[] = { 0x6f, 0x08 };
static CVI_U8 data_jd9366ab_150[] = { 0x70, 0x00 };
static CVI_U8 data_jd9366ab_151[] = { 0x71, 0x00 };
static CVI_U8 data_jd9366ab_152[] = { 0x72, 0x06 };
static CVI_U8 data_jd9366ab_153[] = { 0x73, 0x7b };
static CVI_U8 data_jd9366ab_154[] = { 0x74, 0x00 };
static CVI_U8 data_jd9366ab_155[] = { 0x75, 0x80 };
static CVI_U8 data_jd9366ab_156[] = { 0x76, 0x00 };
static CVI_U8 data_jd9366ab_157[] = { 0x77, 0x05 };
static CVI_U8 data_jd9366ab_158[] = { 0x78, 0x1b };
static CVI_U8 data_jd9366ab_159[] = { 0x79, 0x00 };
static CVI_U8 data_jd9366ab_160[] = { 0x7a, 0x00 };
static CVI_U8 data_jd9366ab_161[] = { 0x7b, 0x00 };
static CVI_U8 data_jd9366ab_162[] = { 0x7c, 0x00 };
static CVI_U8 data_jd9366ab_163[] = { 0x7d, 0x03 };
static CVI_U8 data_jd9366ab_164[] = { 0x7e, 0x7b };
static CVI_U8 data_jd9366ab_165[] = { 0xe0, 0x01 };
static CVI_U8 data_jd9366ab_166[] = { 0x0e, 0x01 };
static CVI_U8 data_jd9366ab_167[] = { 0xe0, 0x03 };
static CVI_U8 data_jd9366ab_168[] = { 0x98, 0x2f };
static CVI_U8 data_jd9366ab_169[] = { 0xe0, 0x04 };
static CVI_U8 data_jd9366ab_170[] = { 0x09, 0x10 };
static CVI_U8 data_jd9366ab_171[] = { 0x2b, 0x2b };
static CVI_U8 data_jd9366ab_172[] = { 0x2e, 0x44 };
static CVI_U8 data_jd9366ab_173[] = { 0xe0, 0x00 };
static CVI_U8 data_jd9366ab_174[] = { 0xe6, 0x02 };
static CVI_U8 data_jd9366ab_175[] = { 0xe7, 0x06 };
static CVI_U8 data_jd9366ab_176[] = { 0x11 };
static CVI_U8 data_jd9366ab_177[] = { 0x29 };
static CVI_U8 data_jd9366ab_178[] = { 0x35, 0x00 };
const struct dsc_instr dsi_init_cmds_jd9366ab_800x1280[] = {
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_0 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_1 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_2 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_3 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_4 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_5 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_6 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_7 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_8 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_9 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_10 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_11 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_12 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_13 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_14 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_15 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_16 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_17 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_18 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_19 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_20 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_21 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_22 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_23 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_24 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_25 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_26 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_27 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_28 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_29 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_30 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_31 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_32 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_33 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_34 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_35 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_36 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_37 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_38 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_39 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_40 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_41 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_42 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_43 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_44 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_45 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_46 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_47 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_48 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_49 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_50 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_51 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_52 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_53 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_54 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_55 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_56 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_57 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_58 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_59 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_60 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_61 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_62 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_63 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_64 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_65 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_66 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_67 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_68 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_69 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_70 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_71 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_72 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_73 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_74 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_75 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_76 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_77 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_78 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_79 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_80 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_81 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_82 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_83 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_84 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_85 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_86 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_87 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_88 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_89 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_90 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_91 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_92 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_93 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_94 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_95 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_96 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_97 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_98 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_99 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_100 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_101 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_102 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_103 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_104 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_105 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_106 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_107 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_108 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_109 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_110 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_111 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_112 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_113 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_114 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_115 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_116 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_117 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_118 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_119 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_120 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_121 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_122 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_123 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_124 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_125 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_126 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_127 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_128 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_129 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_130 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_131 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_132 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_133 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_134 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_135 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_136 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_137 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_138 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_139 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_140 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_141 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_142 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_143 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_144 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_145 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_146 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_147 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_148 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_149 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_150 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_151 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_152 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_153 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_154 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_155 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_156 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_157 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_158 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_159 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_160 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_161 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_162 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_163 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_164 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_165 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_166 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_167 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_168 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_169 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_170 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_171 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_172 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_173 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_174 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_175 },
	{.delay = 120, .data_type = 0x05, .size = 1, .data = data_jd9366ab_176 },
	{.delay = 5, .data_type = 0x05, .size = 1, .data = data_jd9366ab_177 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_jd9366ab_178 }
};

#else
#error "MIPI_TX_PARAM multi-delcaration!!"
#endif // _MIPI_TX_PARAM_JD9366AB_H_
