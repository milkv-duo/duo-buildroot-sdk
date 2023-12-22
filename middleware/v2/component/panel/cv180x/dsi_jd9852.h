#ifndef _MIPI_TX_PARAM_JD9852_H_
#define _MIPI_TX_PARAM_JD9852_H_

#include <linux/vo_mipi_tx.h>
#include <linux/cvi_comm_mipi_tx.h>

#define JD9852_HACT 240
#define JD9852_HSA  60
#define JD9852_HBP  50
#define JD9852_HFP  50

#define JD9852_VACT 320
#define JD9852_VSA  2
#define JD9852_VBP  2
#define JD9852_VFP  2

#define PIXEL_CLK(x) ((x##_VACT + x##_VSA + x##_VBP + x##_VFP) * (x##_HACT + x##_HSA + x##_HBP + x##_HFP) * 60 / 1000)

struct combo_dev_cfg_s dev_cfg_JD9852_240x320 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, -1, MIPI_TX_LANE_CLK, -1, -1},
	.lane_pn_swap = {true, true, true, true, true},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = JD9852_HSA,
		.vid_hbp_pixels = JD9852_HBP,
		.vid_hfp_pixels = JD9852_HFP,
		.vid_hline_pixels = JD9852_HACT,
		.vid_vsa_lines = JD9852_VSA,
		.vid_vbp_lines = JD9852_VBP,
		.vid_vfp_lines = JD9852_VFP,
		.vid_active_lines = JD9852_VACT,
		.vid_vsa_pos_polarity = true,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = PIXEL_CLK(JD9852),
};

const struct hs_settle_s hs_timing_cfg_JD9852_240x320 = {.prepare = 6, .zero = 32, .trail = 1};

static CVI_U8 data_JD9852_0[] = {0xdf, 0x98, 0x51, 0xe9}; // PASSWORD
static CVI_U8 data_JD9852_1[] = {0xde, 0x00}; // ---------------- PAGE0 --------------
static CVI_U8 data_JD9852_2[] = {0xb7, 0x1e, 0x7d, 0x1e, 0x2b};   // VGMP, VGSP, VGMN, VGSN 4.2
static CVI_U8 data_JD9852_3[] = {
    0xc8, 0x3f, 0x38, 0x33, 0x2f, 0x32, 0x34, 0x2f, 0x2f, 0x2d,
    0x2c, 0x27, 0x1a, 0x14, 0x0a, 0x06, 0x0e, 0x3f, 0x38, 0x33,
    0x2f, 0x32, 0x34, 0x2f, 0x2f, 0x2d, 0x2c, 0x27, 0x1a, 0x14,
    0x0a, 0x06, 0x0e
};  // Set_R_GAMMA
static CVI_U8 data_JD9852_4[] = {0xb9, 0x33, 0x08, 0xcc}; // POW_CTRL
static CVI_U8 data_JD9852_5[] = {0xbb, 0x46, 0x7a, 0x30, 0x40, 0x7c, 0x60, 0x70, 0x70};   // DCDC_SEL
static CVI_U8 data_JD9852_6[] = {0xbc, 0x38, 0x3c};   // VDDD_CTRL
static CVI_U8 data_JD9852_7[] = {0xc0, 0x31, 0x20};   // SETSTBA
static CVI_U8 data_JD9852_8[] = {0xc1, 0x12}; // SETPANEL(default)
static CVI_U8 data_JD9852_9[] = {0xc3, 0x08, 0x00, 0x0a, 0x10, 0x08, 0x54, 0x45, 0x71, 0x2c};  // SETRGBCYC
static CVI_U8 data_JD9852_10[] = {
    0xc4, 0x00, 0xa0, 0x79, 0x0e, 0x0a, 0x16, 0x79, 0x0e, 0x0a,
    0x16, 0x79, 0x0e, 0x0a, 0x16, 0x82, 0x00, 0x03
};  // SETRGBCYC(default)
static CVI_U8 data_JD9852_11[] = {0xd0, 0x04, 0x0c, 0x6b, 0x0f, 0x07, 0x03};
static CVI_U8 data_JD9852_12[] = {0xd7, 0x13, 0x00};
static CVI_U8 data_JD9852_13[] = {0xde, 0x02};
static CVI_U8 data_JD9852_14[] = {0xb8, 0x1d, 0xa0, 0x2f, 0x04, 0x33};
static CVI_U8 data_JD9852_15[] = {0xc1, 0x10, 0x66, 0x66, 0x01};
static CVI_U8 data_JD9852_16[] = {0xde, 0x00};
static CVI_U8 data_JD9852_17[] = {0x11};
static CVI_U8 data_JD9852_18[] = {0xde, 0x02};
static CVI_U8 data_JD9852_19[] = {0xc5, 0x4e, 0x00, 0x00};
static CVI_U8 data_JD9852_20[] = {0xca, 0x30, 0x20, 0xf4};
static CVI_U8 data_JD9852_21[] = {0xde, 0x04};
static CVI_U8 data_JD9852_22[] = {0xd3, 0x3c};
static CVI_U8 data_JD9852_23[] = {0xde, 0x00};
static CVI_U8 data_JD9852_24[] = {0x3a, 0x77};
static CVI_U8 data_JD9852_25[] = {0x29};

const struct dsc_instr dsi_init_cmds_JD9852_320x480[] = {
    {.delay = 0, .data_type = 0x29, .size = 4, .data = data_JD9852_0},
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_JD9852_1},
    {.delay = 0, .data_type = 0x29, .size = 5, .data = data_JD9852_2},
    {.delay = 0, .data_type = 0x29, .size = 33, .data = data_JD9852_3},
    {.delay = 0, .data_type = 0x29, .size = 4, .data = data_JD9852_4},
    {.delay = 0, .data_type = 0x29, .size = 9, .data = data_JD9852_5},
    {.delay = 0, .data_type = 0x29, .size = 3, .data = data_JD9852_6},
    {.delay = 0, .data_type = 0x29, .size = 3, .data = data_JD9852_7},
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_JD9852_8},
    {.delay = 0, .data_type = 0x29, .size = 10, .data = data_JD9852_9},
    {.delay = 0, .data_type = 0x29, .size = 18, .data = data_JD9852_10},
    {.delay = 0, .data_type = 0x29, .size = 7, .data = data_JD9852_11},
    {.delay = 0, .data_type = 0x29, .size = 3, .data = data_JD9852_12},
    {.delay = 1, .data_type = 0x15, .size = 2, .data = data_JD9852_13},
    {.delay = 0, .data_type = 0x29, .size = 6, .data = data_JD9852_14},
    {.delay = 0, .data_type = 0x29, .size = 5, .data = data_JD9852_15},
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_JD9852_16},
    {.delay = 120, .data_type = 0x05, .size = 1, .data = data_JD9852_17},
    {.delay = 1, .data_type = 0x15, .size = 2, .data = data_JD9852_18},
    {.delay = 1, .data_type = 0x29, .size = 4, .data = data_JD9852_19},
    {.delay = 1, .data_type = 0x29, .size = 4, .data = data_JD9852_20},
    {.delay = 1, .data_type = 0x15, .size = 2, .data = data_JD9852_21},
    {.delay = 1, .data_type = 0x15, .size = 2, .data = data_JD9852_22},
    {.delay = 1, .data_type = 0x15, .size = 2, .data = data_JD9852_23},
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_JD9852_24},
    {.delay = 0, .data_type = 0x05, .size = 1, .data = data_JD9852_25}
};

#else
#error "MIPI_TX_PARAM multi-delcaration!!"
#endif // _MIPI_TX_PARAM_JD9852_H_
