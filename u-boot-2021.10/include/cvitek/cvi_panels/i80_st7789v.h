#ifndef _I80_PARAM_ST7789V_H_
#define _I80_PARAM_ST7789V_H_

#include <cvi_i80.h>

const struct _VO_I80_CFG_S i80_st7789v_cfg = {
	.lane_s = {.CS = 0, .RS = 1, .WR = 2, .RD = 3},
	.fmt = VO_I80_FORMAT_RGB565,
	.cycle_time = 66,
	.sync_info = {
		.vid_hsa_pixels = 0,
		.vid_hbp_pixels = 0,
		.vid_hfp_pixels = 0,
		.vid_hline_pixels = 240,
		.vid_vsa_lines = 0,
		.vid_vbp_lines = 0,
		.vid_vfp_lines = 0,
		.vid_active_lines = 320,
		.vid_vsa_pos_polarity = false,
		.vid_hsa_pos_polarity = false,
	},
};

const struct _VO_I80_INSTR_S i80_st7789v_init_cmds[] = {
	{.delay = 0,   .data_type = COMMAND, .data = 0x11},
	{.delay = 0,   .data_type = COMMAND, .data = 0x35},
	{.delay = 0,   .data_type = DATA,    .data = 0x00},
	{.delay = 0,   .data_type = COMMAND, .data = 0x36},//Display Setting
	{.delay = 0,   .data_type = DATA,    .data = (1 << 6)/*(1<<5)|(1<<6)*/},
	{.delay = 0,   .data_type = COMMAND, .data = 0x3A},
	{.delay = 0,   .data_type = DATA,    .data = 0x05},
	{.delay = 0,   .data_type = COMMAND, .data = 0xB2},
	{.delay = 0,   .data_type = DATA,    .data = 0x0C},
	{.delay = 0,   .data_type = DATA,    .data = 0x0C},
	{.delay = 0,   .data_type = DATA,    .data = 0x00},
	{.delay = 0,   .data_type = DATA,    .data = 0x33},
	{.delay = 0,   .data_type = DATA,    .data = 0x33},
	{.delay = 0,   .data_type = COMMAND, .data = 0xB7},
	{.delay = 0,   .data_type = DATA,    .data = 0x75},
	{.delay = 0,   .data_type = COMMAND, .data = 0xBB},
	{.delay = 0,   .data_type = DATA,    .data = 0x19},
	{.delay = 0,   .data_type = COMMAND, .data = 0xC0},
	{.delay = 0,   .data_type = DATA,    .data = 0x2C},
	{.delay = 0,   .data_type = COMMAND, .data = 0xC2},
	{.delay = 0,   .data_type = DATA,    .data = 0x01},
	{.delay = 0,   .data_type = COMMAND, .data = 0xC3},
	{.delay = 0,   .data_type = DATA,    .data = 0x0C},
	{.delay = 0,   .data_type = COMMAND, .data = 0xC4},
	{.delay = 0,   .data_type = DATA,    .data = 0x20},
	{.delay = 0,   .data_type = COMMAND, .data = 0xC6},
	{.delay = 0,   .data_type = DATA,    .data = 0x0F},
	{.delay = 0,   .data_type = COMMAND, .data = 0xD0},
	{.delay = 0,   .data_type = DATA,    .data = 0xA4},
	{.delay = 0,   .data_type = DATA,    .data = 0xA1},
	{.delay = 0,   .data_type = COMMAND, .data = 0xE0},//Gamma setting
	{.delay = 0,   .data_type = DATA,    .data = 0xD0},
	{.delay = 0,   .data_type = DATA,    .data = 0x04},
	{.delay = 0,   .data_type = DATA,    .data = 0x0C},
	{.delay = 0,   .data_type = DATA,    .data = 0x0E},
	{.delay = 0,   .data_type = DATA,    .data = 0x0E},
	{.delay = 0,   .data_type = DATA,    .data = 0x29},
	{.delay = 0,   .data_type = DATA,    .data = 0x37},
	{.delay = 0,   .data_type = DATA,    .data = 0x44},
	{.delay = 0,   .data_type = DATA,    .data = 0x47},
	{.delay = 0,   .data_type = DATA,    .data = 0x0B},
	{.delay = 0,   .data_type = DATA,    .data = 0x17},
	{.delay = 0,   .data_type = DATA,    .data = 0x16},
	{.delay = 0,   .data_type = DATA,    .data = 0x1B},
	{.delay = 0,   .data_type = DATA,    .data = 0x1F},
	{.delay = 0,   .data_type = COMMAND, .data = 0xE1},
	{.delay = 0,   .data_type = DATA,    .data = 0xD0},
	{.delay = 0,   .data_type = DATA,    .data = 0x04},
	{.delay = 0,   .data_type = DATA,    .data = 0x0C},
	{.delay = 0,   .data_type = DATA,    .data = 0x0E},
	{.delay = 0,   .data_type = DATA,    .data = 0x0F},
	{.delay = 0,   .data_type = DATA,    .data = 0x29},
	{.delay = 0,   .data_type = DATA,    .data = 0x37},
	{.delay = 0,   .data_type = DATA,    .data = 0x44},
	{.delay = 0,   .data_type = DATA,    .data = 0x4A},
	{.delay = 0,   .data_type = DATA,    .data = 0x0C},
	{.delay = 0,   .data_type = DATA,    .data = 0x17},
	{.delay = 0,   .data_type = DATA,    .data = 0x16},
	{.delay = 0,   .data_type = DATA,    .data = 0x1B},
	{.delay = 0,   .data_type = DATA,    .data = 0x1F},
	{.delay = 0,   .data_type = COMMAND, .data = 0x29},

	{.delay = 0,   .data_type = COMMAND, .data = 0x2A},
	{.delay = 0,   .data_type = DATA,    .data = 0x0 },//Xstart
	{.delay = 0,   .data_type = DATA,    .data = 0x0 },
	{.delay = 0,   .data_type = DATA,    .data = 0x0 },//Xend
	{.delay = 0,   .data_type = DATA,    .data = 0xEF},
	{.delay = 0,   .data_type = COMMAND, .data = 0x2B},
	{.delay = 0,   .data_type = DATA,    .data = 0x0 },//Ystart
	{.delay = 0,   .data_type = DATA,    .data = 0x0 },
	{.delay = 0,   .data_type = DATA,    .data = 0x01},//Yend
	{.delay = 0,   .data_type = DATA,    .data = 0x3F},
	{.delay = 0,   .data_type = COMMAND, .data = 0x2C},
};
#else
#error "_I80_PARAM_ST7789V_H_ multi-delcaration!!"
#endif // _I80_PARAM_ST7789V_H_
