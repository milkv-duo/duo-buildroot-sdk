/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: cvi_panels.h
 * Description:
 */

#ifndef __CVI_PANEL_H__
#define __CVI_PANEL_H__

#include "cvi_i80.h"
#include "cvi_lvds.h"

struct panel_desc_s {
	char *panel_name;
	const struct combo_dev_cfg_s *dev_cfg;
	const struct hs_settle_s *hs_timing_cfg;
	const struct dsc_instr *dsi_init_cmds;
	int dsi_init_cmds_size;
	const struct _VO_I80_CFG_S *i80_cfg;
	const struct _VO_I80_INSTR_S *i80_init_cmds;
	int i80_init_cmds_size;
	struct cvi_lvds_cfg_s *lvds_cfg;
};

#ifdef MIPI_PANEL_HX8394
#include "dsi_hx8394_evb.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "HX8394-720x1280",
	.dev_cfg = &dev_cfg_hx8394_720x1280,
	.hs_timing_cfg = &hs_timing_cfg_hx8394_720x1280,
	.dsi_init_cmds = dsi_init_cmds_hx8394_720x1280,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_hx8394_720x1280)
};
#elif defined(MIPI_PANEL_ILI9881C)
#include "dsi_ili9881c.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "ILI9881C-720x1280",
	.dev_cfg = &dev_cfg_ili9881c_720x1280,
	.hs_timing_cfg = &hs_timing_cfg_ili9881c_720x1280,
	.dsi_init_cmds = dsi_init_cmds_ili9881c_720x1280,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_ili9881c_720x1280)
};
#elif defined(MIPI_PANEL_ILI9881D)
#include "dsi_ili9881d.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "ILI9881D-720x1280",
	.dev_cfg = &dev_cfg_ili9881d_720x1280,
	.hs_timing_cfg = &hs_timing_cfg_ili9881d_720x1280,
	.dsi_init_cmds = dsi_init_cmds_ili9881d_720x1280,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_ili9881d_720x1280)
};
#elif defined(MIPI_PANEL_JD9366AB)
#include "dsi_jd9366ab.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "JD9366AB-800x1280",
	.dev_cfg = &dev_cfg_jd9366ab_800x1280,
	.hs_timing_cfg = &hs_timing_cfg_jd9366ab_800x1280,
	.dsi_init_cmds = dsi_init_cmds_jd9366ab_800x1280,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_jd9366ab_800x1280)
};
#elif defined(MIPI_PANEL_NT35521)
#include "dsi_nt35521.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "NT35521-800x1280",
	.dev_cfg = &dev_cfg_nt35521_800x1280,
	.hs_timing_cfg = &hs_timing_cfg_nt35521_800x1280,
	.dsi_init_cmds = dsi_init_cmds_nt35521_800x1280,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_nt35521_800x1280)
};
#elif defined(MIPI_PANEL_OTA7290B)
#include "dsi_ota7290b.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "OTA7290B-320x1280",
	.dev_cfg = &dev_cfg_ota7290b_320x1280,
	.hs_timing_cfg = &hs_timing_cfg_ota7290b_320x1280,
	.dsi_init_cmds = dsi_init_cmds_ota7290b_320x1280,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_ota7290b_320x1280)
};
#elif defined(MIPI_PANEL_OTA7290B_1920)
#include "dsi_ota7290b_1920.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "OTA7290B-440x1920",
	.dev_cfg = &dev_cfg_ota7290b_440x1920,
	.hs_timing_cfg = &hs_timing_cfg_ota7290b_440x1920,
	.dsi_init_cmds = dsi_init_cmds_ota7290b_440x1920,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_ota7290b_440x1920)
};
#elif defined(MIPI_PANEL_ICN9707)
#include "dsi_icn9707.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "ICN9707-480x1920",
	.dev_cfg = &dev_cfg_icn9707_480x1920,
	.hs_timing_cfg = &hs_timing_cfg_icn9707_480x1920,
	.dsi_init_cmds = dsi_init_cmds_icn9707_480x1920,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_icn9707_480x1920)
};
#elif defined(MIPI_PANEL_3AML069LP01G)
#include "dsi_3aml069lp01g.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "3AML069LP01G-600x1024",
	.dev_cfg = &dev_cfg_3AML069LP01G_600x1024,
	.hs_timing_cfg = &hs_timing_cfg_3AML069LP01G_600x1024,
	.dsi_init_cmds = dsi_init_cmds_3AML069LP01G_600x1024,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_3AML069LP01G_600x1024)
};
#elif defined(MIPI_PANEL_ST7701)
#include "dsi_st7701.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "ST7701-480x800",
	.dev_cfg = &dev_cfg_st7701_480x800,
	.hs_timing_cfg = &hs_timing_cfg_st7701_480x800,
	.dsi_init_cmds = dsi_init_cmds_st7701_480x800,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_st7701_480x800)
};
#elif defined(I80_PANEL_ST7789V)
#include "i80_st7789v.h"
static struct panel_desc_s panel_desc = {
	.i80_cfg = &i80_st7789v_cfg,
	.i80_init_cmds = i80_st7789v_init_cmds,
	.i80_init_cmds_size = ARRAY_SIZE(i80_st7789v_init_cmds)
};
#elif defined(LVDS_PANEL_EK79202)
#include "lvds_ek79202.h"
static struct panel_desc_s panel_desc = {
	.lvds_cfg = &lvds_ek79202_cfg
};
#else
#include "dsi_hx8394_evb.h"
static struct panel_desc_s panel_desc = {
	.panel_name = "HX8394-720x1280",
	.dev_cfg = &dev_cfg_hx8394_720x1280,
	.hs_timing_cfg = &hs_timing_cfg_hx8394_720x1280,
	.dsi_init_cmds = dsi_init_cmds_hx8394_720x1280,
	.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_hx8394_720x1280)
};
#endif

#endif
