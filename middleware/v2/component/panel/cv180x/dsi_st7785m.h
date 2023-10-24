#ifndef _MIPI_TX_PARAM_ST7785M_H_
#define _MIPI_TX_PARAM_ST7785M_H_

#include <linux/vo_mipi_tx.h>
#include <linux/cvi_comm_mipi_tx.h>

#define ST7785M_HACT	240
#define ST7785M_HSA		10
#define ST7785M_HBP		80
#define ST7785M_HFP		80

#define ST7785M_VACT	320
#define ST7785M_VSA		10
#define ST7785M_VBP		20
#define ST7785M_VFP		20

#define PIXEL_CLK(x) ((x##_VACT + x##_VSA + x##_VBP + x##_VFP) \
	* (x##_HACT + x##_HSA + x##_HBP + x##_HFP) * 60 / 1000)

struct combo_dev_cfg_s dev_cfg_st7785m_240x320 = {
	.devno = 0,
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_CLK, -1, -1, -1},
	.lane_pn_swap = {true, true, true, true, true},
	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = ST7785M_HSA,
		.vid_hbp_pixels = ST7785M_HBP,
		.vid_hfp_pixels = ST7785M_HFP,
		.vid_hline_pixels = ST7785M_HACT,
		.vid_vsa_lines = ST7785M_VSA,
		.vid_vbp_lines = ST7785M_VBP,
		.vid_vfp_lines = ST7785M_VFP,
		.vid_active_lines = ST7785M_VACT,
		.vid_vsa_pos_polarity = true,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = PIXEL_CLK(ST7785M),
};

const struct hs_settle_s hs_timing_cfg_st7785m_240x320 = { .prepare = 6, .zero = 32, .trail = 1 };

static CVI_U8 data_st7785m_0[] = { 0x11 };
static CVI_U8 data_st7785m_1[] = { 0x36,0x00, 0x00 };
static CVI_U8 data_st7785m_2[] = { 0x3a,0x00, 0x66};
static CVI_U8 data_st7785m_3[] = { 0xb0,0x00, 0x10 };
static CVI_U8 data_st7785m_4[] = { 0xb2,0x00, 0x0c, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x33, 0x00, 0x33 };
static CVI_U8 data_st7785m_5[] = { 0xb7, 0x00, 0x51 };
static CVI_U8 data_st7785m_6[] = { 0xbb, 0x00, 0x1e };
static CVI_U8 data_st7785m_7[] = { 0xc0, 0x00, 0x2c };
static CVI_U8 data_st7785m_8[] = { 0xc2, 0x00, 0x01 };
static CVI_U8 data_st7785m_9[] = { 0xc3,0x00, 0x13 };
static CVI_U8 data_st7785m_10[] = { 0xc6,0x00, 0x0f };
static CVI_U8 data_st7785m_11[] = { 0xd0,0x00, 0xa7 };
static CVI_U8 data_st7785m_12[] = { 0xd0,0x00, 0xa4, 0x00, 0xa1 };
static CVI_U8 data_st7785m_13[] = { 0xe0,0x00, 0xF0, 0x00, 0x0C, 0x00, 
									0x14, 0x00, 0x0C, 0x00, 0x0C, 0x00, 
									0x09, 0x00, 0x3B, 0x00, 0x44, 0x00, 
									0x52, 0x00, 0x3A, 0x00, 0x14, 0x00, 
									0x15, 0x00, 0x35, 0x00, 0x3B };
static CVI_U8 data_st7785m_14[] = { 0xe1,0x00, 0xF0, 0x00, 0x0F, 0x00, 
									0x16,0x00, 0x09, 0x00, 0x08, 0x00, 
									0x23,0x00, 0x3B, 0x00, 0x33, 0x00, 
									0x52,0x00, 0x25, 0x00, 0x0F, 0x00, 
									0x11, 0x00, 0x33, 0x00, 0x39 };

static CVI_U8 data_st7785m_15[] = { 0x21 };
static CVI_U8 data_st7785m_16[] = { 0x11 };
static CVI_U8 data_st7785m_17[] = { 0x29 };

const struct dsc_instr dsi_init_cmds_st7785m_240x320[] = {
	{.delay = 120, .data_type = 0x05, .size = 1, .data = data_st7785m_0 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_1 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_2 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_3 },
	{.delay = 10, .data_type = 0x29, .size = 11, .data = data_st7785m_4 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_5 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_6 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_7 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_8 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_9 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_10 },
	{.delay = 10, .data_type = 0x29, .size = 3, .data = data_st7785m_11 },
	{.delay = 10, .data_type = 0x29, .size = 5, .data = data_st7785m_12 },
	{.delay = 10, .data_type = 0x29, .size = 29, .data = data_st7785m_13 },
	{.delay = 10, .data_type = 0x29, .size = 29, .data = data_st7785m_14 },
	{.delay = 10, .data_type = 0x05, .size = 1, .data = data_st7785m_15 },
	{.delay = 120, .data_type = 0x05, .size = 1, .data = data_st7785m_16 },
	{.delay = 20, .data_type = 0x05, .size = 1, .data = data_st7785m_17 },

};

#else
#error "MIPI_TX_PARAM multi-delcaration!!"
#endif // _MIPI_TX_PARAM_ST7785M_H_
