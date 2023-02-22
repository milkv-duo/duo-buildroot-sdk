#ifndef _I80_PARAM_ST7789V_H_
#define _I80_PARAM_ST7789V_H_

#include <linux/cvi_comm_vo.h>

#define COMMAND 0
#define DATA	1

const VO_I80_CFG_S stI80Cfg = {
	.lane_s = {.CS = 0, .RS = 1, .WR = 2, .RD = 3},
	.fmt = VO_I80_FORMAT_RGB565,
	.cycle_time = 200,
};

const VO_I80_INSTR_S init_cmds[] = {
	{.delay = 0,   .data_type = COMMAND, .data = 0x11},
	{.delay = 0,   .data_type = COMMAND, .data = 0x35},
	{.delay = 0,   .data_type = DATA,    .data = 0x00},
	{.delay = 0,   .data_type = COMMAND, .data = 0x36},
	{.delay = 0,   .data_type = DATA,    .data = 0x00},
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
	{.delay = 0,   .data_type = COMMAND, .data = 0xE0},
	{.delay = 0,   .data_type = DATA,    .data = 0xD0},
	{.delay = 0,   .data_type = DATA,    .data = 0x05},
	{.delay = 0,   .data_type = DATA,    .data = 0x0D},
	{.delay = 0,   .data_type = DATA,    .data = 0x13},
	{.delay = 0,   .data_type = DATA,    .data = 0x14},
	{.delay = 0,   .data_type = DATA,    .data = 0x2D},
	{.delay = 0,   .data_type = DATA,    .data = 0x3C},
	{.delay = 0,   .data_type = DATA,    .data = 0x52},
	{.delay = 0,   .data_type = DATA,    .data = 0x49},
	{.delay = 0,   .data_type = DATA,    .data = 0x13},
	{.delay = 0,   .data_type = DATA,    .data = 0x0B},
	{.delay = 0,   .data_type = DATA,    .data = 0x09},
	{.delay = 0,   .data_type = DATA,    .data = 0x1A},
	{.delay = 0,   .data_type = DATA,    .data = 0x1B},
	{.delay = 0,   .data_type = COMMAND, .data = 0xE1},
	{.delay = 0,   .data_type = DATA,    .data = 0xD0},
	{.delay = 0,   .data_type = DATA,    .data = 0x05},
	{.delay = 0,   .data_type = DATA,    .data = 0x0C},
	{.delay = 0,   .data_type = DATA,    .data = 0x13},
	{.delay = 0,   .data_type = DATA,    .data = 0x14},
	{.delay = 0,   .data_type = DATA,    .data = 0x2F},
	{.delay = 0,   .data_type = DATA,    .data = 0x4C},
	{.delay = 0,   .data_type = DATA,    .data = 0x41},
	{.delay = 0,   .data_type = DATA,    .data = 0x4E},
	{.delay = 0,   .data_type = DATA,    .data = 0x2A},
	{.delay = 0,   .data_type = DATA,    .data = 0x1D},
	{.delay = 0,   .data_type = DATA,    .data = 0x1D},
	{.delay = 0,   .data_type = DATA,    .data = 0x1B},
	{.delay = 0,   .data_type = DATA,    .data = 0x1B},

	{.delay = 0,   .data_type = COMMAND, .data = 0x29},

	{.delay = 0,   .data_type = COMMAND, .data = 0x2A},
	{.delay = 0,   .data_type = DATA,	 .data = 0x0 },//Xstart
	{.delay = 0,   .data_type = DATA,	 .data = 0x0 },
	{.delay = 0,   .data_type = DATA,	 .data = 0x0 },//Xend
	{.delay = 0,   .data_type = DATA,	 .data = 0xEF},

	{.delay = 0,   .data_type = COMMAND, .data = 0x2B},
	{.delay = 0,   .data_type = DATA,	 .data = 0x0 },//Ystart
	{.delay = 0,   .data_type = DATA,	 .data = 0x0 },
	{.delay = 0,   .data_type = DATA,	 .data = 0x01},//Yend
	{.delay = 0,   .data_type = DATA,	 .data = 0x3F},

	{.delay = 0,   .data_type = COMMAND, .data = 0x2C},
};

#endif // _I80_PARAM_ST7789V_H_
