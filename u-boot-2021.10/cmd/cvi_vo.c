/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <command.h>
#include <common.h>
#include <stdlib.h>
#include <linux/delay.h>
#include <cpu_func.h>
#include <cvi_disp.h>
#include <cvi_lvds.h>
#include "../drivers/video/cvitek/scaler.h"
#include "../drivers/video/cvitek/dsi_phy.h"

#include <cvi_panels/cvi_panels.h>

#include <asm/io.h>

#define PANLE_ADAPTIVITY 0

enum sclr_vo_intf intf_type = SCLR_VO_INTF_MIPI;

static int lvds_panel_init(struct cvi_lvds_cfg_s *lvds_cfg)
{
	int ret = 0;
#ifdef CONFIG_DISPLAY_CVITEK_LVDS
	ret = lvds_init(lvds_cfg);
#endif
	return ret;
}

int i80_init(int devno, const struct _VO_I80_INSTR_S *cmds, int size)
{
	int ret = 0;
	unsigned int sw_cmd;

	for (int i = 0; i < size; i++) {
		sw_cmd = (i80_ctrl[cmds[i].data_type] << 8) | cmds[i].data;
		i80_set_cmd(sw_cmd);

		if (cmds[i].delay)
			mdelay(cmds[i].delay);
	}
	// pull high i80-lane
	i80_set_cmd(0xffff);
	i80_set_cmd(0x2ffff);

	return ret;
}

static int dsi_init(int devno, const struct dsc_instr *cmds, int size)
{
	int ret;

	for (int i = 0; i < size; i++) {
		const struct dsc_instr *instr = &cmds[i];
		struct cmd_info_s cmd_info = {
			.devno = devno,
			.cmd_size = instr->size,
			.data_type = instr->data_type,
			.cmd = (void *)instr->data
		};

		ret = mipi_tx_set_cmd(&cmd_info);
		if (instr->delay)
			mdelay(instr->delay);

		if (ret) {
			printf("dsi init failed at %d instr.\n", i);
			return ret;
		}
	}
	return ret;
}

#if PANLE_ADAPTIVITY
static int dsi_get_panel_id(int devno, unsigned int *id)
{
	int ret = 0;
	unsigned char param[3] = {0xDA, 0xDB, 0xDC};
	unsigned char buf[4];

	for (int i = 0; i < 3; i++) {
		struct get_cmd_info_s get_cmd_info = {
			.devno = devno,
			.data_type = 0x06,
			.data_param = param[i],
			.get_data_size = 0x01,
			.get_data = buf
		};
		memset(buf, 0, sizeof(buf));

		ret = mipi_tx_get_cmd(&get_cmd_info);
		if (ret < 0) {
			printf("dsi get panel id fail.\n");
			return ret;
		}

		*id |= (buf[0] << (i * 8));
	}

	return ret;
}

static void dsi_panel_init_adaptivity(void)
{
	unsigned int panelid = 0;
	const struct combo_dev_cfg_s *dev_cfg   = NULL;
	const struct hs_settle_s *hs_timing_cfg = NULL;
	const struct dsc_instr *dsi_init_cmds   = NULL;
	int size = 0;

	//use one type panel's cfg to init
	mipi_tx_set_combo_dev_cfg((struct combo_dev_cfg_s *)&dev_cfg_hx8394_720x1280);

	dsi_get_panel_id(0, &panelid);
	debug("[DBG] dsi_get_panel_id: 0x%X\n", panelid);

	switch (panelid) {
	case 0xF9483:
		dev_cfg = &dev_cfg_hx8394_720x1280;
		hs_timing_cfg = &hs_timing_cfg_hx8394_720x1280;
		dsi_init_cmds = dsi_init_cmds_hx8394_720x1280;
		size = ARRAY_SIZE(dsi_init_cmds_hx8394_720x1280);
	break;

	case 0xAA: //modify it before use
		dev_cfg = &dev_cfg_ili9881c_720x1280;
		hs_timing_cfg = &hs_timing_cfg_ili9881c_720x1280;
		dsi_init_cmds = dsi_init_cmds_ili9881c_720x1280;
		size = ARRAY_SIZE(dsi_init_cmds_ili9881c_720x1280);
	break;

	case 0xBB: //modify it before use
		dev_cfg = &dev_cfg_jd9366ab_800x1280;
		hs_timing_cfg = &hs_timing_cfg_jd9366ab_800x1280;
		dsi_init_cmds = dsi_init_cmds_jd9366ab_800x1280;
		size = ARRAY_SIZE(dsi_init_cmds_jd9366ab_800x1280);
	break;

	default:
	break;
	}

	if (panelid != 0xF9483)
		mipi_tx_set_combo_dev_cfg(dev_cfg);
	dsi_init(0, dsi_init_cmds, size);
	dphy_set_hs_settle(hs_timing_cfg->prepare, hs_timing_cfg->zero, hs_timing_cfg->trail);
}
#else
static void dsi_panel_init(void)
{
	u8 prepare = panel_desc.hs_timing_cfg->prepare;
	u8 zero = panel_desc.hs_timing_cfg->zero;
	u8 trail = panel_desc.hs_timing_cfg->trail;

	debug("Init panel %s\n", panel_desc.panel_name);
	mipi_tx_set_combo_dev_cfg(panel_desc.dev_cfg);
	dsi_init(0, panel_desc.dsi_init_cmds, panel_desc.dsi_init_cmds_size);
	dphy_set_hs_settle(prepare, zero, trail);
}
#endif

/***************************************************/
static int do_startvo(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	struct udevice *udev;
	int dev, intf, timing;
	char *endp;

	if (argc < 4)
		return CMD_RET_USAGE;

	dev = simple_strtoul(argv[1], &endp, 10);
	if (*argv[1] == 0 || *endp != 0)
		return CMD_RET_USAGE;
	intf = simple_strtoul(argv[2], &endp, 10);
	if (*argv[2] == 0 || *endp != 0)
		return CMD_RET_USAGE;
	timing = simple_strtoul(argv[3], &endp, 10);
	if (*argv[3] == 0 || *endp != 0)
		return CMD_RET_USAGE;
	uclass_get_device(UCLASS_VIDEO, 0, &udev);

	switch (intf) {
	case VO_INTF_MIPI: {
		mipi_tx_set_mode(0);
		#if PANLE_ADAPTIVITY
		dsi_panel_init_adaptivity();
		#else
		dsi_panel_init();
		#endif
		mipi_tx_set_mode(1);
	}
	break;

	case VO_INTF_I80:
		intf_type = SCLR_VO_INTF_I80;
		i80_set_combo_dev_cfg(panel_desc.i80_cfg);
		i80_set_sw_mode(1);
		i80_init(0, panel_desc.i80_init_cmds, panel_desc.i80_init_cmds_size);
		i80_set_sw_mode(0);
		i80_sclr_intr_clr();//intr_status should be clear, otherwise kernel will get stuck
	break;

	case VO_INTF_LCD_18BIT:
	case VO_INTF_LCD_24BIT:
	case VO_INTF_LCD_30BIT:
		intf_type = SCLR_VO_INTF_LVDS;
		lvds_panel_init(panel_desc.lvds_cfg);
	break;

	default:
	break;
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(startvo
	, 4, 0, do_startvo
	, "open vo device with a certain interface."
	, "    - startvo [dev intftype sync]"
);

/***************************************************/
static int do_stopvo(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	int dev;
	char *endp;

	if (argc < 2)
		return CMD_RET_USAGE;

	dev = simple_strtoul(argv[1], &endp, 10);
	if (*argv[1] == 0 || *endp != 0)
		return CMD_RET_USAGE;

	sclr_disp_tgen_enable(false);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(stopvo
	, 2, 0, do_stopvo
	, "close interface of vo device."
	, "    - stopvo [dev]"
);

/***************************************************/
static int do_startvl(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	int layer, stride;
	struct sclr_rect rect;
	struct sclr_disp_cfg *cfg;
	struct sclr_disp_timing *timing;
	u64 addr_in, addr_out;
	char *endp;
	int size_offset, align;

	if (argc < 6)
		return CMD_RET_USAGE;

	layer = simple_strtoul(argv[1], &endp, 10);
	if (*argv[1] == 0 || *endp != 0)
		return CMD_RET_USAGE;
	addr_in = simple_strtoul(argv[2], &endp, 16);
	if (*argv[2] == 0 || *endp != 0)
		return CMD_RET_USAGE;
	addr_out = simple_strtoul(argv[3], &endp, 16);
	if (*argv[3] == 0 || *endp != 0)
		return CMD_RET_USAGE;
	size_offset = simple_strtoul(argv[4], &endp, 16) - 8;
	if (*argv[4] == 0 || *endp != 0)
		return CMD_RET_USAGE;
	align = simple_strtoul(argv[5], &endp, 10);
	if (*argv[5] == 0 || *endp != 0)
		return CMD_RET_USAGE;

	timing = sclr_disp_get_timing();

	rect.w = *(int *)(addr_in + size_offset);
	rect.h = *(int *)(addr_in + size_offset + 4);
	stride = ALIGN(rect.w, align);

	rect.y = (timing->vmde_end - timing->vmde_start + 1 - rect.h) / 2;
	rect.x = (timing->hmde_end - timing->hmde_start + 1 - rect.w) / 2;

	sclr_disp_set_rect(rect);
	cfg = sclr_disp_get_cfg();

	if (intf_type == SCLR_VO_INTF_MIPI || intf_type == SCLR_VO_INTF_LVDS) {
		cfg->fmt = SCL_FMT_YUV420;
		cfg->in_csc = SCL_CSC_601_LIMIT_YUV2RGB;
		cfg->mem.width = rect.w;
		cfg->mem.height = rect.h;
		cfg->mem.pitch_y = ALIGN(stride, 32);
		cfg->mem.pitch_c = ALIGN(stride >> 1, 32);
	} else if (intf_type == SCLR_VO_INTF_I80) {
		unsigned char *in = (unsigned char *)(addr_in);
		unsigned char *out = (unsigned char *)(addr_out);
		unsigned short w = (*(in + 25) << 24) | (*(in + 24) << 16) | (*(in + 23) << 8) | *(in + 22);
		unsigned short h = (*(in + 21) << 24) | (*(in + 20) << 16) | (*(in + 19) << 8) | *(in + 18);

		debug("%c, %c, w:%d, h:%d\n", *in, *(in + 1), w, h);
		debug("in:%p, out:%p\n", in, out);

		if (*(in) == 'B' && *(in + 1) == 'M' && w == rect.w && h == rect.h) {
			debug("this is a bmp && %d*%d\n", h, w);
			in = in + 54;	 //Remove the bmp header
		} else {
			debug("no bmp or rect not match, display white!\n");
			in = in + 54;
			memset(in + 54, 0xFF, rect.w * rect.h * 3);
		}
		i80_package_frame(in, out, stride, 3, rect.w, rect.h);
		flush_cache(addr_out, ALIGN(((rect.w * 2 + 1) * 3), align) * rect.h);
		cfg->fmt = SCL_FMT_BGR_PACKED;
		cfg->in_csc = SCL_CSC_NONE;
		cfg->mem.width = rect.w * 2 + 1;
		cfg->mem.height = rect.h;
		cfg->mem.pitch_y = ALIGN((cfg->mem.width * 3), align);
		cfg->mem.pitch_c = 0;
	}

	cfg->mem.start_x = 0;
	cfg->mem.start_y = 0;
	cfg->mem.addr0 = addr_out;
	cfg->mem.addr1 = ALIGN(cfg->mem.addr0 + cfg->mem.pitch_y * ALIGN(((rect.h + 1) & ~(BIT(0))), 16), 0x1000);
	cfg->mem.addr2 = ALIGN(cfg->mem.addr1 + cfg->mem.pitch_c * ALIGN(((rect.h + 1) >> 1), 16), 0x1000);

	sclr_disp_set_cfg(cfg);

	if (intf_type == SCLR_VO_INTF_I80) {
		sclr_disp_reg_force_up();
		i80_set_run();
	}

	sclr_disp_enable_window_bgcolor(false);
	debug("\nstart vl(%d) addr_in(%#llx) addr_out(%#llx) stride(%d) rect(%d %d %d %d)!\n"
		, layer, addr_in, addr_out, stride, rect.x, rect.y, rect.w, rect.h);
	debug("\nstart vl cfg->mem.addr(%#llx-%#llx-%#llx) pitch_y:%d pitch_c:%d.\n"
		, cfg->mem.addr0, cfg->mem.addr1, cfg->mem.addr2, cfg->mem.pitch_y, cfg->mem.pitch_c);

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(startvl
	, 6, 0, do_startvl
	, "open video layer of the vo"
	, "- startvl [layer address_in address_out img_size_addr_offset alignment]"
);

/***************************************************/
static int do_stopvl(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	int layer;
	char *endp;

	if (argc < 2)
		return CMD_RET_USAGE;

	layer = simple_strtoul(argv[1], &endp, 10);
	if (*argv[1] == 0 || *endp != 0)
		return CMD_RET_USAGE;

	sclr_disp_enable_window_bgcolor(true);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(stopvl
	, 2, 0, do_stopvl
	, "close video layer of the vo"
	, "- stopvl [layer]"
);

/***************************************************/
static int do_setvobg(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	int dev;
	u32 bgcolor;
	u16 r, g, b;
	char *endp;

	if (argc < 3)
		return CMD_RET_USAGE;

	dev = simple_strtoul(argv[1], &endp, 10);
	if (*argv[1] == 0 || *endp != 0)
		return CMD_RET_USAGE;
	bgcolor = simple_strtoul(argv[2], &endp, 16);
	if (*argv[2] == 0 || *endp != 0)
		return CMD_RET_USAGE;

	r = (bgcolor >> 20) & 0x3ff;
	g = (bgcolor >> 10) & 0x3ff;
	b = bgcolor & 0x3ff;
	sclr_disp_set_frame_bgcolor(r, g, b);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(setvobg
	, 3, 0, do_setvobg
	, "set vo background color"
	, "    - setvobg [dev bgcolor]"
);

