/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 */

#ifndef _CVI_I80_H_
#define _CVI_I80_H_

#include "cvi_mipi.h"

#define COMMAND 0
#define DATA	1

enum _PIXEL_FORMAT_E {
	PIXEL_FORMAT_RGB_888 = 0,
	PIXEL_FORMAT_BGR_888,
	PIXEL_FORMAT_RGB_888_PLANAR,
};

enum _VO_I80_FORMAT {
	VO_I80_FORMAT_RGB444 = 0,
	VO_I80_FORMAT_RGB565,
	VO_I80_FORMAT_RGB666,
	VO_I80_FORMAT_MAX
};

enum i80_op_type {
	I80_OP_GO = 0,
	I80_OP_TIMER,
	I80_OP_DONE,
	I80_OP_MAX,
};

enum i80_ctrl_type {
	I80_CTRL_CMD = 0,
	I80_CTRL_DATA,
	I80_CTRL_EOL = I80_CTRL_DATA,
	I80_CTRL_EOF,
	I80_CTRL_END = I80_CTRL_EOF,
	I80_CTRL_MAX
};

/* Define I80's lane (0~3)
 *
 * CS: Chip Select
 * RS(DCX): Data/Command
 * WR: MCU Write to bus
 * RD: MCU Read from bus
 */
struct _VO_I80_LANE_S {
	unsigned char CS;
	unsigned char RS;
	unsigned char WR;
	unsigned char RD;
};

/* Define I80's config
 *
 * lane_s: lane mapping
 * fmt: format of data
 * cycle_time: cycle time of WR/RD, unit ns, max 250
 */
struct _VO_I80_CFG_S {
	struct _VO_I80_LANE_S lane_s;
	enum _VO_I80_FORMAT fmt;
	unsigned short cycle_time;
	struct sync_info_s sync_info;
};

/* Define I80's cmd
 *
 * delay: ms to delay after instr
 * data_type: Data(1)/Command(0)
 * data: data to send
 */
struct _VO_I80_INSTR_S {
	unsigned char delay;
	unsigned char data_type;
	unsigned char data;
};

extern unsigned char i80_ctrl[I80_CTRL_MAX];

void i80_package_frame(unsigned char *in, unsigned char *out, unsigned int stride, unsigned char byte_cnt,
		       unsigned short w, unsigned short h);
int i80_set_combo_dev_cfg(const struct _VO_I80_CFG_S *i80_cfg);
void i80_set_sw_mode(unsigned long mode_flags);
int i80_set_cmd(unsigned int cmd);
int i80_set_run(void);
void i80_sclr_intr_clr(void);

#endif // _CVI_I80_H_

