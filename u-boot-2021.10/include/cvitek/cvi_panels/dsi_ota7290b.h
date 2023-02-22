#ifndef _MIPI_TX_PARAM_OTA7290B_H_
#define _MIPI_TX_PARAM_OTA7290B_H_

#include <cvi_mipi.h>

// Not support BTA
const struct combo_dev_cfg_s dev_cfg_ota7290b_320x1280 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_CLK, MIPI_TX_LANE_2, MIPI_TX_LANE_3},
	.lane_pn_swap = {false, false, false, false, false},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = 30,
		.vid_hbp_pixels = 50,
		.vid_hfp_pixels = 150,
		.vid_hline_pixels = 320,
		.vid_vsa_lines = 20,
		.vid_vbp_lines = 30,
		.vid_vfp_lines = 150,
		.vid_active_lines = 1280,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = 52910,
};

const struct hs_settle_s hs_timing_cfg_ota7290b_320x1280 = { .prepare = 6, .zero = 32, .trail = 1 };

static u8 data_ota7290b_0[] = { 0xb0, 0x5a };
static u8 data_ota7290b_1[] = { 0xb1, 0x00 };
static u8 data_ota7290b_2[] = { 0x89, 0x01 };
#ifdef _MIPI_TX_BIST_MODE_
static u8 data_ota7290b_3[] = { 0x91, 0x16 };
#else
static u8 data_ota7290b_3[] = { 0x91, 0x17 };
#endif
static u8 data_ota7290b_4[] = { 0xb1, 0x03 };
static u8 data_ota7290b_5[] = { 0x2c, 0x28 };
static u8 data_ota7290b_6[] = { 0x00, 0xf1 };
static u8 data_ota7290b_7[] = { 0x01, 0x18 };
static u8 data_ota7290b_8[] = { 0x02, 0x00 };
static u8 data_ota7290b_9[] = { 0x03, 0x00 };
static u8 data_ota7290b_10[] = { 0x04, 0x00 };
static u8 data_ota7290b_11[] = { 0x05, 0x00 };
static u8 data_ota7290b_12[] = { 0x06, 0x00 };
static u8 data_ota7290b_13[] = { 0x07, 0x00 };
static u8 data_ota7290b_14[] = { 0x08, 0x00 };
static u8 data_ota7290b_15[] = { 0x09, 0x00 };
static u8 data_ota7290b_16[] = { 0x0a, 0x01 };
static u8 data_ota7290b_17[] = { 0x0b, 0x01 };
static u8 data_ota7290b_18[] = { 0x0c, 0x00 };
static u8 data_ota7290b_19[] = { 0x0d, 0x00 };
static u8 data_ota7290b_20[] = { 0x0e, 0x24 };
static u8 data_ota7290b_21[] = { 0x0f, 0x1c };
static u8 data_ota7290b_22[] = { 0x10, 0xc9 };
static u8 data_ota7290b_23[] = { 0x11, 0x60 };
static u8 data_ota7290b_24[] = { 0x12, 0x70 };
static u8 data_ota7290b_25[] = { 0x13, 0x01 };
static u8 data_ota7290b_26[] = { 0x14, 0xe7 };
static u8 data_ota7290b_27[] = { 0x15, 0xff };
static u8 data_ota7290b_28[] = { 0x16, 0x3d };
static u8 data_ota7290b_29[] = { 0x17, 0x0e };
static u8 data_ota7290b_30[] = { 0x18, 0x01 };
static u8 data_ota7290b_31[] = { 0x19, 0x00 };
static u8 data_ota7290b_32[] = { 0x1a, 0x00 };
static u8 data_ota7290b_33[] = { 0x1b, 0xfc };
static u8 data_ota7290b_34[] = { 0x1c, 0x0b };
static u8 data_ota7290b_35[] = { 0x1d, 0xa0 };
static u8 data_ota7290b_36[] = { 0x1e, 0x03 };
static u8 data_ota7290b_37[] = { 0x1f, 0x04 };
static u8 data_ota7290b_38[] = { 0x20, 0x0c };
static u8 data_ota7290b_39[] = { 0x21, 0x00 };
static u8 data_ota7290b_40[] = { 0x22, 0x04 };
static u8 data_ota7290b_41[] = { 0x23, 0x81 };
static u8 data_ota7290b_42[] = { 0x24, 0x1f };
static u8 data_ota7290b_43[] = { 0x25, 0x10 };
static u8 data_ota7290b_44[] = { 0x26, 0x9b };
static u8 data_ota7290b_45[] = { 0x2d, 0x01 };
static u8 data_ota7290b_46[] = { 0x2e, 0x84 };
static u8 data_ota7290b_47[] = { 0x2f, 0x00 };
static u8 data_ota7290b_48[] = { 0x30, 0x02 };
static u8 data_ota7290b_49[] = { 0x31, 0x08 };
static u8 data_ota7290b_50[] = { 0x32, 0x01 };
static u8 data_ota7290b_51[] = { 0x33, 0x1c };
static u8 data_ota7290b_52[] = { 0x34, 0x40 };
static u8 data_ota7290b_53[] = { 0x35, 0xff };
static u8 data_ota7290b_54[] = { 0x36, 0xff };
static u8 data_ota7290b_55[] = { 0x37, 0xff };
static u8 data_ota7290b_56[] = { 0x38, 0xff };
static u8 data_ota7290b_57[] = { 0x39, 0xff };
static u8 data_ota7290b_58[] = { 0x3a, 0x05 };
static u8 data_ota7290b_59[] = { 0x3b, 0x00 };
static u8 data_ota7290b_60[] = { 0x3c, 0x00 };
static u8 data_ota7290b_61[] = { 0x3d, 0x00 };
static u8 data_ota7290b_62[] = { 0x3e, 0x0f };
static u8 data_ota7290b_63[] = { 0x3f, 0x8c };
static u8 data_ota7290b_64[] = { 0x40, 0x2a };
static u8 data_ota7290b_65[] = { 0x41, 0xfc };
static u8 data_ota7290b_66[] = { 0x42, 0x01 };
static u8 data_ota7290b_67[] = { 0x43, 0x40 };
static u8 data_ota7290b_68[] = { 0x44, 0x05 };
static u8 data_ota7290b_69[] = { 0x45, 0xe8 };
static u8 data_ota7290b_70[] = { 0x46, 0x16 };
static u8 data_ota7290b_71[] = { 0x47, 0x00 };
static u8 data_ota7290b_72[] = { 0x48, 0x00 };
static u8 data_ota7290b_73[] = { 0x49, 0x88 };
static u8 data_ota7290b_74[] = { 0x4a, 0x08 };
static u8 data_ota7290b_75[] = { 0x4b, 0x05 };
static u8 data_ota7290b_76[] = { 0x4c, 0x03 };
static u8 data_ota7290b_77[] = { 0x4d, 0xd0 };
static u8 data_ota7290b_78[] = { 0x4e, 0x13 };
static u8 data_ota7290b_79[] = { 0x4f, 0xff };
static u8 data_ota7290b_80[] = { 0x50, 0x0a };
static u8 data_ota7290b_81[] = { 0x51, 0x53 };
static u8 data_ota7290b_82[] = { 0x52, 0x26 };
static u8 data_ota7290b_83[] = { 0x53, 0x22 };
static u8 data_ota7290b_84[] = { 0x54, 0x09 };
static u8 data_ota7290b_85[] = { 0x55, 0x22 };
static u8 data_ota7290b_86[] = { 0x56, 0x00 };
static u8 data_ota7290b_87[] = { 0x57, 0x1c };
static u8 data_ota7290b_88[] = { 0x58, 0x03 };
static u8 data_ota7290b_89[] = { 0x59, 0x3f };
static u8 data_ota7290b_90[] = { 0x5a, 0x28 };
static u8 data_ota7290b_91[] = { 0x5b, 0x01 };
static u8 data_ota7290b_92[] = { 0x5c, 0xcc };
static u8 data_ota7290b_93[] = { 0x5d, 0x21 };
static u8 data_ota7290b_94[] = { 0x5e, 0x84 };
static u8 data_ota7290b_95[] = { 0x5f, 0x84 };
static u8 data_ota7290b_96[] = { 0x60, 0x8e };
static u8 data_ota7290b_97[] = { 0x61, 0x89 };
static u8 data_ota7290b_98[] = { 0x62, 0xf0 };
static u8 data_ota7290b_99[] = { 0x63, 0xb9 };
static u8 data_ota7290b_100[] = { 0x64, 0xc6 };
static u8 data_ota7290b_101[] = { 0x65, 0x96 };
static u8 data_ota7290b_102[] = { 0x66, 0x0a };
static u8 data_ota7290b_103[] = { 0x67, 0x62 };
static u8 data_ota7290b_104[] = { 0x68, 0x90 };
static u8 data_ota7290b_105[] = { 0x69, 0x12 };
static u8 data_ota7290b_106[] = { 0x6a, 0x42 };
static u8 data_ota7290b_107[] = { 0x6b, 0x48 };
static u8 data_ota7290b_108[] = { 0x6c, 0xe8 };
static u8 data_ota7290b_109[] = { 0x6d, 0x98 };
static u8 data_ota7290b_110[] = { 0x6e, 0x08 };
static u8 data_ota7290b_111[] = { 0x6f, 0x9f };
static u8 data_ota7290b_112[] = { 0x70, 0x6b };
static u8 data_ota7290b_113[] = { 0x71, 0x6c };
static u8 data_ota7290b_114[] = { 0x72, 0xa9 };
static u8 data_ota7290b_115[] = { 0x73, 0x20 };
static u8 data_ota7290b_116[] = { 0x74, 0x06 };
static u8 data_ota7290b_117[] = { 0x75, 0x29 };
static u8 data_ota7290b_118[] = { 0x76, 0x00 };
static u8 data_ota7290b_119[] = { 0x77, 0x00 };
static u8 data_ota7290b_120[] = { 0x78, 0x0f };
static u8 data_ota7290b_121[] = { 0x79, 0xe0 };
static u8 data_ota7290b_122[] = { 0x7a, 0x01 };
static u8 data_ota7290b_123[] = { 0x7b, 0xff };
static u8 data_ota7290b_124[] = { 0x7c, 0xff };
static u8 data_ota7290b_125[] = { 0x7d, 0xff };
static u8 data_ota7290b_126[] = { 0x7e, 0x4f };
static u8 data_ota7290b_127[] = { 0x7f, 0xfe };
static u8 data_ota7290b_128[] = { 0xb1, 0x02 };
static u8 data_ota7290b_129[] = { 0x00, 0xff };
static u8 data_ota7290b_130[] = { 0x01, 0x05 };
static u8 data_ota7290b_131[] = { 0x02, 0xa0 };
static u8 data_ota7290b_132[] = { 0x03, 0x00 };
static u8 data_ota7290b_133[] = { 0x04, 0x54 };
static u8 data_ota7290b_134[] = { 0x05, 0x38 };
static u8 data_ota7290b_135[] = { 0x06, 0xa0 };
static u8 data_ota7290b_136[] = { 0x07, 0x0a };
static u8 data_ota7290b_137[] = { 0x08, 0xc0 };
static u8 data_ota7290b_138[] = { 0x09, 0x00 };
static u8 data_ota7290b_139[] = { 0x0a, 0x00 };
static u8 data_ota7290b_140[] = { 0x0b, 0x14 };
static u8 data_ota7290b_141[] = { 0x0c, 0xe6 };
static u8 data_ota7290b_142[] = { 0x0d, 0x0d };
static u8 data_ota7290b_143[] = { 0x0f, 0x08 };
static u8 data_ota7290b_144[] = { 0x10, 0x79 };
static u8 data_ota7290b_145[] = { 0x11, 0x38 };
static u8 data_ota7290b_146[] = { 0x12, 0x73 };
static u8 data_ota7290b_147[] = { 0x13, 0xb3 };
static u8 data_ota7290b_148[] = { 0x14, 0x29 };
static u8 data_ota7290b_149[] = { 0x15, 0x80 };
static u8 data_ota7290b_150[] = { 0x16, 0x07 };
static u8 data_ota7290b_151[] = { 0x17, 0x8a };
static u8 data_ota7290b_152[] = { 0x18, 0x8d };
static u8 data_ota7290b_153[] = { 0x19, 0xbf };
static u8 data_ota7290b_154[] = { 0x1a, 0x69 };
static u8 data_ota7290b_155[] = { 0x1b, 0x0e };
static u8 data_ota7290b_156[] = { 0x1c, 0xff };
static u8 data_ota7290b_157[] = { 0x1d, 0xff };
static u8 data_ota7290b_158[] = { 0x1e, 0xff };
static u8 data_ota7290b_159[] = { 0x1f, 0xff };
static u8 data_ota7290b_160[] = { 0x20, 0xff };
static u8 data_ota7290b_161[] = { 0x21, 0xff };
static u8 data_ota7290b_162[] = { 0x22, 0xff };
static u8 data_ota7290b_163[] = { 0x23, 0xff };
static u8 data_ota7290b_164[] = { 0x24, 0xff };
static u8 data_ota7290b_165[] = { 0x25, 0xff };
static u8 data_ota7290b_166[] = { 0x26, 0xff };
static u8 data_ota7290b_167[] = { 0x27, 0x1f };
static u8 data_ota7290b_168[] = { 0x28, 0xff };
static u8 data_ota7290b_169[] = { 0x29, 0xff };
static u8 data_ota7290b_170[] = { 0x2a, 0xff };
static u8 data_ota7290b_171[] = { 0x2b, 0xff };
static u8 data_ota7290b_172[] = { 0x2c, 0xff };
static u8 data_ota7290b_173[] = { 0x2d, 0x07 };
static u8 data_ota7290b_174[] = { 0x33, 0x06 };
static u8 data_ota7290b_175[] = { 0x35, 0x7e };
static u8 data_ota7290b_176[] = { 0x36, 0x06 };
static u8 data_ota7290b_177[] = { 0x38, 0x7e };
static u8 data_ota7290b_178[] = { 0x3a, 0x80 };
static u8 data_ota7290b_179[] = { 0x3b, 0x01 };
static u8 data_ota7290b_180[] = { 0x3c, 0x00 };
static u8 data_ota7290b_181[] = { 0x3d, 0x2a };
static u8 data_ota7290b_182[] = { 0x3e, 0x00 };
static u8 data_ota7290b_183[] = { 0x3f, 0x40 };
static u8 data_ota7290b_184[] = { 0x40, 0x05 };
static u8 data_ota7290b_185[] = { 0x41, 0x00 };
static u8 data_ota7290b_186[] = { 0x42, 0xa8 };
static u8 data_ota7290b_187[] = { 0x43, 0x00 };
static u8 data_ota7290b_188[] = { 0x44, 0x00 };
static u8 data_ota7290b_189[] = { 0x45, 0x05 };
static u8 data_ota7290b_190[] = { 0x46, 0x00 };
static u8 data_ota7290b_191[] = { 0x47, 0x00 };
static u8 data_ota7290b_192[] = { 0x48, 0x9b };
static u8 data_ota7290b_193[] = { 0x49, 0xd2 };
static u8 data_ota7290b_194[] = { 0x4a, 0x81 };
static u8 data_ota7290b_195[] = { 0x4b, 0x02 };
static u8 data_ota7290b_196[] = { 0x4c, 0x15 };
static u8 data_ota7290b_197[] = { 0x4d, 0xc0 };
static u8 data_ota7290b_198[] = { 0x4e, 0x0f };
static u8 data_ota7290b_199[] = { 0x4f, 0x61 };
static u8 data_ota7290b_200[] = { 0x50, 0x78 };
static u8 data_ota7290b_201[] = { 0x51, 0x7a };
static u8 data_ota7290b_202[] = { 0x52, 0x34 };
static u8 data_ota7290b_203[] = { 0x53, 0x99 };
static u8 data_ota7290b_204[] = { 0x54, 0xa2 };
static u8 data_ota7290b_205[] = { 0x55, 0x02 };
static u8 data_ota7290b_206[] = { 0x56, 0x14 };
static u8 data_ota7290b_207[] = { 0x57, 0xb8 };
static u8 data_ota7290b_208[] = { 0x58, 0xdc };
static u8 data_ota7290b_209[] = { 0x59, 0x34 };
static u8 data_ota7290b_210[] = { 0x5a, 0x1e };
static u8 data_ota7290b_211[] = { 0x5b, 0x8f };
static u8 data_ota7290b_212[] = { 0x5c, 0xc7 };
static u8 data_ota7290b_213[] = { 0x5d, 0xe3 };
static u8 data_ota7290b_214[] = { 0x5e, 0xf1 };
static u8 data_ota7290b_215[] = { 0x5f, 0x78 };
static u8 data_ota7290b_216[] = { 0x60, 0x3c };
static u8 data_ota7290b_217[] = { 0x61, 0x36 };
static u8 data_ota7290b_218[] = { 0x62, 0x1e };
static u8 data_ota7290b_219[] = { 0x63, 0x1b };
static u8 data_ota7290b_220[] = { 0x64, 0x8f };
static u8 data_ota7290b_221[] = { 0x65, 0xc7 };
static u8 data_ota7290b_222[] = { 0x66, 0xe3 };
static u8 data_ota7290b_223[] = { 0x67, 0x31 };
static u8 data_ota7290b_224[] = { 0x68, 0x0c };
static u8 data_ota7290b_225[] = { 0x69, 0x89 };
static u8 data_ota7290b_226[] = { 0x6a, 0x30 };
static u8 data_ota7290b_227[] = { 0x6b, 0x8c };
static u8 data_ota7290b_228[] = { 0x6c, 0x8d };
static u8 data_ota7290b_229[] = { 0x6d, 0x8d };
static u8 data_ota7290b_230[] = { 0x6e, 0x8d };
static u8 data_ota7290b_231[] = { 0x6f, 0x8d };
static u8 data_ota7290b_232[] = { 0x70, 0xc7 };
static u8 data_ota7290b_233[] = { 0x71, 0xe3 };
static u8 data_ota7290b_234[] = { 0x72, 0x31 };
static u8 data_ota7290b_235[] = { 0x73, 0x00 };
static u8 data_ota7290b_236[] = { 0x74, 0x00 };
static u8 data_ota7290b_237[] = { 0x75, 0x00 };
static u8 data_ota7290b_238[] = { 0x76, 0x00 };
static u8 data_ota7290b_239[] = { 0x77, 0x00 };
static u8 data_ota7290b_240[] = { 0x78, 0x00 };
static u8 data_ota7290b_241[] = { 0x79, 0x00 };
static u8 data_ota7290b_242[] = { 0x7a, 0xc6 };
static u8 data_ota7290b_243[] = { 0x7b, 0xc6 };
static u8 data_ota7290b_244[] = { 0x7c, 0xc6 };
static u8 data_ota7290b_245[] = { 0x7d, 0xc6 };
static u8 data_ota7290b_246[] = { 0x7e, 0xc6 };
static u8 data_ota7290b_247[] = { 0x7f, 0xe3 };
static u8 data_ota7290b_248[] = { 0x0b, 0x00 };
static u8 data_ota7290b_249[] = { 0xb1, 0x03 };
static u8 data_ota7290b_250[] = { 0x2c, 0x2c };
static u8 data_ota7290b_251[] = { 0xb1, 0x00 };
static u8 data_ota7290b_252[] = { 0x89, 0x03 };
const struct dsc_instr dsi_init_cmds_ota7290b_320x1280[] = {
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_0 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_1 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_2 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_3 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_4 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_5 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_6 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_7 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_8 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_9 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_10 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_11 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_12 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_13 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_14 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_15 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_16 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_17 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_18 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_19 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_20 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_21 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_22 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_23 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_24 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_25 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_26 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_27 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_28 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_29 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_30 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_31 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_32 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_33 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_34 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_35 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_36 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_37 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_38 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_39 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_40 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_41 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_42 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_43 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_44 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_45 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_46 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_47 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_48 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_49 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_50 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_51 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_52 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_53 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_54 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_55 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_56 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_57 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_58 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_59 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_60 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_61 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_62 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_63 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_64 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_65 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_66 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_67 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_68 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_69 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_70 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_71 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_72 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_73 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_74 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_75 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_76 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_77 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_78 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_79 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_80 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_81 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_82 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_83 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_84 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_85 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_86 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_87 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_88 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_89 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_90 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_91 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_92 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_93 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_94 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_95 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_96 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_97 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_98 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_99 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_100 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_101 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_102 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_103 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_104 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_105 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_106 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_107 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_108 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_109 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_110 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_111 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_112 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_113 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_114 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_115 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_116 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_117 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_118 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_119 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_120 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_121 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_122 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_123 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_124 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_125 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_126 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_127 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_128 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_129 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_130 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_131 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_132 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_133 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_134 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_135 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_136 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_137 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_138 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_139 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_140 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_141 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_142 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_143 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_144 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_145 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_146 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_147 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_148 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_149 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_150 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_151 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_152 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_153 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_154 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_155 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_156 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_157 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_158 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_159 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_160 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_161 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_162 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_163 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_164 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_165 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_166 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_167 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_168 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_169 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_170 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_171 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_172 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_173 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_174 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_175 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_176 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_177 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_178 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_179 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_180 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_181 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_182 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_183 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_184 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_185 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_186 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_187 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_188 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_189 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_190 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_191 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_192 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_193 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_194 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_195 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_196 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_197 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_198 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_199 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_200 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_201 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_202 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_203 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_204 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_205 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_206 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_207 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_208 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_209 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_210 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_211 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_212 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_213 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_214 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_215 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_216 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_217 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_218 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_219 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_220 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_221 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_222 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_223 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_224 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_225 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_226 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_227 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_228 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_229 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_230 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_231 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_232 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_233 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_234 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_235 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_236 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_237 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_238 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_239 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_240 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_241 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_242 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_243 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_244 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_245 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_246 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_247 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_248 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_249 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_250 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_251 },
	{.delay = 0, .data_type = 0x15, .size = 2, .data = data_ota7290b_252 }
};

#else
#error "_MIPI_TX_PARAM_OTA7290B_H_ multi-delcaration!!"
#endif // _MIPI_TX_PARAM_OTA7290B_H_

