/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 */

#include <common.h>
#include <clk.h>
#include <asm/gpio.h>
//#include <asm/hardware.h>
#include <asm/io.h>
//#include <dm/uclass-internal.h>
#include <linux/kernel.h>

#include "reg.h"
#include "vip_common.h"
#include "scaler.h"
#include "dsi_phy.h"
#include <cvi_lvds.h>

static void _fill_disp_timing(struct sclr_disp_timing *timing, struct sync_info_s *sync_info)
{
	timing->vtotal = sync_info->vid_vsa_lines + sync_info->vid_vbp_lines
			+ sync_info->vid_active_lines + sync_info->vid_vfp_lines - 1;
	timing->htotal = sync_info->vid_hsa_pixels + sync_info->vid_hbp_pixels
			+ sync_info->vid_hline_pixels + sync_info->vid_hfp_pixels - 1;
	timing->vsync_start = 1;
	timing->vsync_end = timing->vsync_start + sync_info->vid_vsa_lines - 1;
	timing->vfde_start = timing->vsync_start + sync_info->vid_vsa_lines + sync_info->vid_vbp_lines;
	timing->vfde_end = timing->vfde_start + sync_info->vid_active_lines - 1;
	timing->hsync_start = 1;
	timing->hsync_end = timing->hsync_start + sync_info->vid_hsa_pixels - 1;
	timing->hfde_start = timing->hsync_start + sync_info->vid_hsa_pixels + sync_info->vid_hbp_pixels;
	timing->hfde_end = timing->hfde_start + sync_info->vid_hline_pixels - 1;
	timing->vsync_pol = sync_info->vid_vsa_pos_polarity;
	timing->hsync_pol = sync_info->vid_hsa_pos_polarity;

	timing->vmde_start = timing->vfde_start;
	timing->vmde_end = timing->vfde_end;
	timing->hmde_start = timing->hfde_start;
	timing->hmde_end = timing->hfde_end;
}

int lvds_init(struct cvi_lvds_cfg_s *lvds_cfg)
{
	union sclr_lvdstx lvds_reg;
	bool data_en[LANE_MAX_NUM] = {false, false, false, false, false};
	struct sclr_disp_timing timing;
	struct disp_ctrl_gpios ctrl_gpios;
	int i = 0, ret = 0;

	for (i = 0; i < LANE_MAX_NUM; i++) {
		if (lvds_cfg->lane_id[i] < 0 || lvds_cfg->lane_id[i] >= LANE_MAX_NUM) {
			dphy_dsi_set_lane(i, VO_LVDS_LANE_MAX, false, false);
			continue;
		}
		dphy_dsi_set_lane(i, lvds_cfg->lane_id[i], lvds_cfg->lane_pn_swap[i], false);
		if (lvds_cfg->lane_id[i] != VO_LVDS_LANE_CLK) {
			data_en[lvds_cfg->lane_id[i] - 1] = true;
		}
	}

	dphy_dsi_lane_en(true, data_en, false);
	sclr_disp_set_intf(SCLR_VO_INTF_LVDS);

	lvds_reg.b.out_bit = lvds_cfg->out_bits;
	lvds_reg.b.vesa_mode = lvds_cfg->mode;
	if (lvds_cfg->chn_num == 1)
		lvds_reg.b.dual_ch = 0;
	else if (lvds_cfg->chn_num == 2)
		lvds_reg.b.dual_ch = 1;
	else {
		lvds_reg.b.dual_ch = 0;
		printf("invalid lvds chn_num(%d). Use 1 instead.", lvds_cfg->chn_num);
	}
	lvds_reg.b.vs_out_en = 1;
	lvds_reg.b.hs_out_en = 1;
	lvds_reg.b.hs_blk_en = 1;
	lvds_reg.b.ml_swap = 1;
	lvds_reg.b.ctrl_rev = 0;
	lvds_reg.b.oe_swap = 0;
	lvds_reg.b.en = 1;

	dphy_lvds_set_pll(lvds_cfg->pixelclock, lvds_cfg->chn_num);
	dphy_dsi_analog_setting(true);
	sclr_lvdstx_set(lvds_reg);

	_fill_disp_timing(&timing, &lvds_cfg->sync_info);
	sclr_disp_set_timing(&timing);
	sclr_disp_tgen_enable(true);

	get_disp_ctrl_gpios(&ctrl_gpios);
	ret = dm_gpio_set_value(&ctrl_gpios.disp_pwm_gpio,
				ctrl_gpios.disp_pwm_gpio.flags & GPIOD_ACTIVE_LOW ? 0 : 1);
	if (ret < 0)
		printf("dm_gpio_set_value(disp_pwm_gpio, deassert) failed: %d", ret);

	return ret;
}

