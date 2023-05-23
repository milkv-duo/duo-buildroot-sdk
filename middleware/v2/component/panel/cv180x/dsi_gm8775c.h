#ifndef _MIPI_TX_PARAM_GM8775C_1080P_H_
#define _MIPI_TX_PARAM_GM8775C_1080P_H_

#include <linux/vo_mipi_tx.h>
#include "linux/cvi_comm_mipi_tx.h"

#define _BIST_COLOR             0

#define _HX8775C_RX_HACT        1920
#define _HX8775C_RX_HFP         88
#define _HX8775C_RX_HSA         44
#define _HX8775C_RX_HBP         148

#define _HX8775C_RX_VACT        1080
#define _HX8775C_RX_VFP         4
#define _HX8775C_RX_VSA         5
#define _HX8775C_RX_VBP         36

#define PIXEL_CLK(x) ((x##_VACT + x##_VSA + x##_VBP + x##_VFP) \
	* (x##_HACT + x##_HSA + x##_HBP + x##_HFP) * 60 / 1000)

struct combo_dev_cfg_s dev_cfg_gm8775c = {
	.devno = 0,
#ifdef MIPI_PANEL_2_LANES
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_CLK, -1, -1},
#else
	.lane_id = {MIPI_TX_LANE_0, MIPI_TX_LANE_1, MIPI_TX_LANE_CLK, MIPI_TX_LANE_2, MIPI_TX_LANE_3},
#endif
	.lane_pn_swap = {true, true, true, true, true},

	.output_mode = OUTPUT_MODE_DSI_VIDEO,
	.video_mode = BURST_MODE,
	.output_format = OUT_FORMAT_RGB_24_BIT,
	.sync_info = {
		.vid_hsa_pixels = _HX8775C_RX_HSA,
		.vid_hbp_pixels = _HX8775C_RX_HBP,
		.vid_hfp_pixels = _HX8775C_RX_HFP,
		.vid_hline_pixels = _HX8775C_RX_HACT,
		.vid_vsa_lines = _HX8775C_RX_VSA,
		.vid_vbp_lines = _HX8775C_RX_VBP,
		.vid_vfp_lines = _HX8775C_RX_VFP,
		.vid_active_lines = _HX8775C_RX_VACT,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = true,
	},
	.pixel_clk = PIXEL_CLK(_HX8775C_RX),
};

const struct hs_settle_s hs_timing_cfg_gm8775c = { .prepare = 6, .zero = 32, .trail = 1 };

static CVI_U8 data_gm8775c_0[] = { 0x27, 0xAA };
static CVI_U8 data_gm8775c_1[] = { 0x48, 0x02 };
static CVI_U8 data_gm8775c_2[] = { 0xB6, 0x20 };
static CVI_U8 data_gm8775c_3[] = { 0x01, 0x80 };
static CVI_U8 data_gm8775c_4[] = { 0x02, 0x38 };
static CVI_U8 data_gm8775c_5[] = { 0x03, 0x47 };
static CVI_U8 data_gm8775c_6[] = { 0x04, 0x58 };
static CVI_U8 data_gm8775c_7[] = { 0x05, 0x2c };
static CVI_U8 data_gm8775c_8[] = { 0x06, 0x94 };
static CVI_U8 data_gm8775c_9[] = { 0x07, 0x00 };
static CVI_U8 data_gm8775c_10[] = { 0x08, 0x04 };
static CVI_U8 data_gm8775c_11[] = { 0x09, 0x05 };
static CVI_U8 data_gm8775c_12[] = { 0x0A, 0x24 };

//use mipi clk
static CVI_U8 data_gm8775c_13[] = { 0x0B, 0x82 };
static CVI_U8 data_gm8775c_14[] = { 0x0C, 0x14 };

static CVI_U8 data_gm8775c_15[] = { 0x0D, 0x01 };
static CVI_U8 data_gm8775c_16[] = { 0x0E, 0x80 };
static CVI_U8 data_gm8775c_17[] = { 0x0F, 0x20 };
static CVI_U8 data_gm8775c_18[] = { 0x10, 0x20 };
static CVI_U8 data_gm8775c_19[] = { 0x11, 0x03 };
static CVI_U8 data_gm8775c_20[] = { 0x12, 0x1B };
static CVI_U8 data_gm8775c_21[] = { 0x13, 0x53 };
static CVI_U8 data_gm8775c_22[] = { 0x14, 0x01 };
static CVI_U8 data_gm8775c_23[] = { 0x15, 0x23 };
static CVI_U8 data_gm8775c_24[] = { 0x16, 0x40 };
static CVI_U8 data_gm8775c_25[] = { 0x17, 0x00 };
static CVI_U8 data_gm8775c_26[] = { 0x18, 0x01 };
static CVI_U8 data_gm8775c_27[] = { 0x19, 0x23 };
static CVI_U8 data_gm8775c_28[] = { 0x1A, 0x40 };
static CVI_U8 data_gm8775c_29[] = { 0x1B, 0x00 };
static CVI_U8 data_gm8775c_30[] = { 0x1E, 0x46 };
static CVI_U8 data_gm8775c_31[] = { 0x51, 0x30 };
static CVI_U8 data_gm8775c_32[] = { 0x1F, 0x10 };
//debug
// static CVI_U8 data_gm8775c_35[] = { 0x7B, 0x4E };
// static CVI_U8 data_gm8775c_36[] = { 0x7C, 0x4F };
// static CVI_U8 data_gm8775c_37[] = { 0x7D, 0x4D };

#if _BIST_COLOR
static CVI_U8 data_gm8775c_33[] = { 0x2A, 0x4D };
#else
static CVI_U8 data_gm8775c_34[] = { 0x2A, 0x01 };

// static CVI_U8 data_gm8775c_35[] = { 0x6A, 0x08 };
// static CVI_U8 data_gm8775c_36[] = { 0x6C, 0x9E };
// static CVI_U8 data_gm8775c_37[] = { 0x6D, 0x07 };
// static CVI_U8 data_gm8775c_38[] = { 0x6E, 0x00 };
// static CVI_U8 data_gm8775c_39[] = { 0x6F, 0x8A };
// static CVI_U8 data_gm8775c_40[] = { 0x70, 0x19 };
// static CVI_U8 data_gm8775c_41[] = { 0x71, 0x00 };
#endif

const struct dsc_instr dsi_init_cmds_gm8775c[] = {
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_0 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_1 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_2 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_3 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_4 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_5 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_6 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_7 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_8 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_9 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_10 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_11 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_12 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_13 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_14 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_15 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_16 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_17 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_18 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_19 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_20 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_21 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_22 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_23 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_24 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_25 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_26 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_27 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_28 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_29 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_30 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_31 },
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_32 },
	//debug
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_35 },
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_36 },
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_37 },
#if _BIST_COLOR
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_33 },
#else
	{.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_34 }
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_35 },
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_36 },
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_37 },
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_38 },
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_39 },
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_40 },
	// {.delay = 0, .data_type = 0x23, .size = 2, .data = data_gm8775c_41 }
#endif
};

#else
#error "MIPI_TX_PARAM multi-delcaration!!"
#endif // _MIPI_TX_PARAM_GM8775C_1080P_H_
