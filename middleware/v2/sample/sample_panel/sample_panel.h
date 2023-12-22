/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: sample_dsi_panel.h
 * Description:
 */

#ifndef __SAMPLE_DSI_PANEL_H__
#define __SAMPLE_DSI_PANEL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <linux/cvi_comm_vo.h>
#include "dsi_hx8394_evb.h"
#include "dsi_ili9881c.h"
#include "dsi_ili9881d.h"
#include "dsi_jd9366ab.h"
#include "dsi_nt35521.h"
#include "dsi_ota7290b.h"
#include "dsi_ota7290b_1920.h"
#include "dsi_icn9707.h"
#include "dsi_3aml069lp01g.h"
#include "dsi_st7701.h"
#include "dsi_hx8399_1080p.h"
#include "dsi_gm8775c.h"
#include "dsi_lt9611.h"
#include "lvds_lcm185x56.h"
#include "hw_mcu_st7789v3.h"
#include "bt656_tp2803.h"
#include "bt1120_nvp6021.h"

typedef enum {
	PANEL_MODE_DSI,
	PANEL_MODE_LVDS,
	PANEL_MODE_BT,
	PANEL_MODE_MCU,
} PANEL_TYPE;

typedef struct dsi_panel_desc_s {
	struct combo_dev_cfg_s *dev_cfg;
	const struct hs_settle_s *hs_timing_cfg;
	const struct dsc_instr *dsi_init_cmds;
	int dsi_init_cmds_size;
} dsi_panel_desc;

struct panel_desc_s {
	char *panel_mode;
	PANEL_TYPE panel_type;
	union {
		dsi_panel_desc stdsicfg;
		VO_PUB_ATTR_S stVoPubAttr;
	};
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
