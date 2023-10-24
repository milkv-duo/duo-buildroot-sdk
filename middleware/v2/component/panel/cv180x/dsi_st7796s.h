#ifndef _MIPI_TX_PARAM_ST7796S_H_
#define _MIPI_TX_PARAM_ST7796S_H_

#include <linux/vo_mipi_tx.h>
#include <linux/cvi_comm_mipi_tx.h>

#define ST7796S_HACT	320
#define ST7796S_HSA		20
#define ST7796S_HBP		40
#define ST7796S_HFP		10

#define ST7796S_VACT	480
#define ST7796S_VSA		2
#define ST7796S_VBP		2
#define ST7796S_VFP		2

#define PIXEL_CLK(x) ((x##_VACT + x##_VSA + x##_VBP + x##_VFP) \
	* (x##_HACT + x##_HSA + x##_HBP + x##_HFP) * 60 / 1000)

struct combo_dev_cfg_s dev_cfg_ST7796S_320x480 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, -1, MIPI_TX_LANE_CLK, -1, -1},
	.lane_pn_swap = {true, true, true, true, true},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = ST7796S_HSA,
		.vid_hbp_pixels = ST7796S_HBP,
		.vid_hfp_pixels = ST7796S_HFP,
		.vid_hline_pixels = ST7796S_HACT,
		.vid_vsa_lines = ST7796S_VSA,
		.vid_vbp_lines = ST7796S_VBP,
		.vid_vfp_lines = ST7796S_VFP,
		.vid_active_lines = ST7796S_VACT,
		.vid_vsa_pos_polarity = true,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = PIXEL_CLK(ST7796S),
};

const struct hs_settle_s hs_timing_cfg_ST7796S_320x480 = { .prepare = 6, .zero = 32, .trail = 1 };

static CVI_U8 data_ST7796S_0[] = { 0x11 };   // Sleep Out
static CVI_U8 data_ST7796S_1[] = { 0x36, 0x48 };    // Memory Data Access Control
static CVI_U8 data_ST7796S_2[] = { 0x3a, 0x77 };    // Interface Pixel Format
static CVI_U8 data_ST7796S_3[] = { 0xf0, 0xc3 };    // Command Set Control
static CVI_U8 data_ST7796S_4[] = { 0xf0, 0x96 };
static CVI_U8 data_ST7796S_5[] = { 0xb4, 0x01 };    // , 1-Dot INV
static CVI_U8 data_ST7796S_6[] = { 0xb7, 0xc6 };
static CVI_U8 data_ST7796S_7[] = { 0xb9, 0x02, 0xe0 };
static CVI_U8 data_ST7796S_8[] = { 0xc0, 0x80, 0x75 };  // , VGH = 15V, VGL = -10V
static CVI_U8 data_ST7796S_9[] = { 0xc1, 0x13 };    // 4.5V
static CVI_U8 data_ST7796S_10[] = { 0xc2, 0xa7 };
static CVI_U8 data_ST7796S_11[] = { 0xc5, 0x1d };   // VCOM Control, 1.15V
static CVI_U8 data_ST7796S_12[] = { 0xe8, 0x40, 0x8a, 0x00, 0x00, 0x29, 0x19, 0xa5, 0x33 };
static CVI_U8 data_ST7796S_13[] = { 0xe0, 0xd0, 0x02, 0x0a, 0x10,
                                    0x11, 0x0c, 0x35, 0x44, 0x46,
                                    0x3C, 0x15, 0x13, 0x17, 0x1a };
static CVI_U8 data_ST7796S_14[] = { 0xe1, 0xd0, 0x01, 0x08, 0x0b,
                                    0x0c, 0x17, 0x35, 0x44, 0x47,
                                    0x09, 0x18, 0x17, 0x19, 0x1d };

static CVI_U8 data_ST7796S_15[] = { 0x35, 0x00 };
static CVI_U8 data_ST7796S_16[] = { 0xf0, 0x3c };
static CVI_U8 data_ST7796S_17[] = { 0xf0, 0x69 };
static CVI_U8 data_ST7796S_18[] = { 0x21 }; // Display Inversion On
static CVI_U8 data_ST7796S_19[] = { 0x29 }; // Display ON

const struct dsc_instr dsi_init_cmds_ST7796S_320x480[] = {
    {.delay = 120, .data_type = 0x05, .size = 1, .data = data_ST7796S_0 },
    {.delay = 120, .data_type = 0x15, .size = 2, .data = data_ST7796S_1 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_2 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_3 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_4 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_5 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_6 },
    {.delay = 0, .data_type = 0x29, .size = 3, .data = data_ST7796S_7 },
    {.delay = 0, .data_type = 0x29, .size = 3, .data = data_ST7796S_8 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_9 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_10 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_11 },
    {.delay = 0, .data_type = 0x29, .size = 9, .data = data_ST7796S_12 },
    {.delay = 0, .data_type = 0x29, .size = 15, .data = data_ST7796S_13 },
    {.delay = 0, .data_type = 0x29, .size = 15, .data = data_ST7796S_14 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_15 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_16 },
    {.delay = 0, .data_type = 0x15, .size = 2, .data = data_ST7796S_17 },
    {.delay = 0, .data_type = 0x05, .size = 1, .data = data_ST7796S_18 },
    {.delay = 120, .data_type = 0x05, .size = 1, .data = data_ST7796S_19 }
};

#else
#error "MIPI_TX_PARAM multi-delcaration!!"
#endif // _MIPI_TX_PARAM_ST7796S_H_
