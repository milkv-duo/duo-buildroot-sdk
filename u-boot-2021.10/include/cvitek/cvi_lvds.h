/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 */

#ifndef _CVI_LVDS_H_
#define _CVI_LVDS_H_

#include "cvi_mipi.h"

#define LANE_MAX_NUM   5

enum lvds_lane_id {
	VO_LVDS_LANE_CLK = 0,
	VO_LVDS_LANE_0,
	VO_LVDS_LANE_1,
	VO_LVDS_LANE_2,
	VO_LVDS_LANE_3,
	VO_LVDS_LANE_MAX,
};

enum LVDS_OUT_BIT {
	LVDS_OUT_6BIT = 0,
	LVDS_OUT_8BIT,
	LVDS_OUT_10BIT,
	LVDS_OUT_MAX,
};

enum LVDS_MODE {
	LVDS_MODE_JEIDA = 0,
	LVDS_MODE_VESA,
	LVDS_MODE_MAX,
};

/*
 * @pixelclock: pixel clock in kHz
 */
struct cvi_lvds_cfg_s {
	enum LVDS_OUT_BIT       out_bits;
	enum LVDS_MODE          mode;
	unsigned char           chn_num;
	bool                    data_big_endian;
	enum lvds_lane_id       lane_id[LANE_MAX_NUM];
	bool                    lane_pn_swap[LANE_MAX_NUM];
	struct sync_info_s      sync_info;
	unsigned short          u16FrameRate;
	unsigned int            pixelclock;
};

int lvds_init(struct cvi_lvds_cfg_s *lvds_cfg);

#endif // _CVI_LVDS_H_
