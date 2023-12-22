/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 */

#include <common.h>
#include <clk.h>
#include <asm/gpio.h>
//#include <asm/hardware.h>
#include <asm/io.h>
#include <linux/stringify.h>
#include <linux/delay.h>

#include "reg.h"
#include "vip_common.h"
#include "scaler.h"
#include "dsi_phy.h"
#include <cvi_mipi.h>

/* cmd_mode: cmd_mode
 * bit[0]: dcs cmd mode. 0(hw)/1(sw)
 */
static int cmd_mode = 1;
unsigned int pixel_clk;
u8 lane_num;
u8 bits;

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

static void _cal_htt_extra(struct combo_dev_cfg_s *dev_cfg, u8 lane_num, u8 bits)
{
	unsigned short htt_old, htt_new, htt_new_extra = 0;
	unsigned short vtt;
	float fps;
	float bit_rate_MHz;
	float clk_hs_MHz;
	float clk_hs_ns;
	float line_rate_KHz, line_time_us;
	float over_head;
	float t_period_max, t_period_real;
	struct sync_info_s *sync_info = &dev_cfg->sync_info;

	htt_old = sync_info->vid_hsa_pixels + sync_info->vid_hbp_pixels
			+ sync_info->vid_hline_pixels + sync_info->vid_hfp_pixels;
	vtt = sync_info->vid_vsa_lines + sync_info->vid_vbp_lines
			+ sync_info->vid_active_lines + sync_info->vid_vfp_lines;
	fps = dev_cfg->pixel_clk * 1000.0 / (htt_old * vtt);
	bit_rate_MHz = dev_cfg->pixel_clk / 1000.0 * bits / lane_num;
	clk_hs_MHz = bit_rate_MHz / 2;
	clk_hs_ns = 1000 / clk_hs_MHz;
	line_rate_KHz = vtt * fps / 1000;
	line_time_us = 1000 / line_rate_KHz;
	over_head = (3 * 50 * 2 * 3) + clk_hs_ns * 360;
	t_period_max = line_time_us * 1000 - over_head;
	t_period_real = clk_hs_ns * sync_info->vid_hline_pixels * bits / 4 / 2;
	htt_new = (unsigned short)(htt_old * t_period_real / t_period_max);
	if (htt_new > htt_old) {
		if (htt_new & 0x0003)
			htt_new += (4 - (htt_new & 0x0003));
		htt_new_extra = htt_new - htt_old;
		sync_info->vid_hfp_pixels += htt_new_extra;
		dev_cfg->pixel_clk = htt_new * vtt * fps / 1000;
	}
}

int mipi_tx_set_combo_dev_cfg(const struct combo_dev_cfg_s *dev_cfg)
{
	int ret, i;
	bool data_en[LANE_MAX_NUM] = {false, false, false, false, false};
	struct sclr_disp_timing timing;
	enum sclr_dsi_fmt dsi_fmt;
	bool preamble_on = false;
	struct combo_dev_cfg_s dev_cfg_t = *dev_cfg;
	struct disp_ctrl_gpios ctrl_gpios;

	dphy_dsi_disable_lanes();
	for (i = 0; i < LANE_MAX_NUM; i++) {
		if ((dev_cfg_t.lane_id[i] < 0) || (dev_cfg_t.lane_id[i] >= MIPI_TX_LANE_MAX)) {
			data_en[i] = false;
			continue;
		}
		dphy_dsi_set_lane(i, dev_cfg_t.lane_id[i], dev_cfg_t.lane_pn_swap[i], true);
		if (dev_cfg_t.lane_id[i] != MIPI_TX_LANE_CLK) {
			++lane_num;
			data_en[dev_cfg_t.lane_id[i] - 1] = true;
		}
	}
	if (lane_num == 0) {
		printf("%s: no active mipi-dsi lane\n", __func__);
		return -EINVAL;
	}

	switch (dev_cfg_t.output_format) {
	case OUT_FORMAT_RGB_16_BIT:
		bits = 16;
		dsi_fmt = SCLR_DSI_FMT_RGB565;
	break;

	case OUT_FORMAT_RGB_18_BIT:
		bits = 18;
		dsi_fmt = SCLR_DSI_FMT_RGB666;
	break;

	case OUT_FORMAT_RGB_24_BIT:
		bits = 24;
		dsi_fmt = SCLR_DSI_FMT_RGB888;
	break;

	case OUT_FORMAT_RGB_30_BIT:
		bits = 30;
		dsi_fmt = SCLR_DSI_FMT_RGB101010;
	break;

	default:
	return -EINVAL;
	}
	_cal_htt_extra(&dev_cfg_t, lane_num, bits);
	pixel_clk = dev_cfg_t.pixel_clk;
	_fill_disp_timing(&timing, &dev_cfg_t.sync_info);
	preamble_on = (dev_cfg_t.pixel_clk * bits / lane_num) > 1500000;
	dphy_dsi_lane_en(true, data_en, preamble_on);
	dphy_dsi_set_pll(dev_cfg_t.pixel_clk, lane_num, bits);
	sclr_disp_set_intf(SCLR_VO_INTF_MIPI);
	sclr_dsi_config(lane_num, dsi_fmt, dev_cfg_t.sync_info.vid_hline_pixels);
	sclr_disp_set_timing(&timing);
	sclr_disp_tgen_enable(true);

	get_disp_ctrl_gpios(&ctrl_gpios);

	ret = dm_gpio_set_value(&ctrl_gpios.disp_power_ct_gpio,
				ctrl_gpios.disp_power_ct_gpio.flags & GPIOD_ACTIVE_LOW ? 0 : 1);
	if (ret < 0) {
		printf("dm_gpio_set_value(disp_power_ct_gpio, deassert) failed: %d", ret);
		return ret;
	}
	ret = dm_gpio_set_value(&ctrl_gpios.disp_pwm_gpio,
				ctrl_gpios.disp_pwm_gpio.flags & GPIOD_ACTIVE_LOW ? 0 : 1);
	if (ret < 0) {
		printf("dm_gpio_set_value(disp_pwm_gpio, deassert) failed: %d", ret);
		return ret;
	}
	ret = dm_gpio_set_value(&ctrl_gpios.disp_reset_gpio,
				ctrl_gpios.disp_reset_gpio.flags & GPIOD_ACTIVE_LOW ? 0 : 1);
	if (ret < 0) {
		printf("dm_gpio_set_value(disp_reset_gpio, deassert) failed: %d", ret);
		return ret;
	}
	mdelay(10);
	ret = dm_gpio_set_value(&ctrl_gpios.disp_reset_gpio,
				ctrl_gpios.disp_reset_gpio.flags & GPIOD_ACTIVE_LOW ? 1 : 0);
	if (ret < 0) {
		printf("dm_gpio_set_value(disp_reset_gpio, deassert) failed: %d", ret);
		return ret;
	}
	mdelay(10);
	ret = dm_gpio_set_value(&ctrl_gpios.disp_reset_gpio,
				ctrl_gpios.disp_reset_gpio.flags & GPIOD_ACTIVE_LOW ? 0 : 1);
	if (ret < 0) {
		printf("dm_gpio_set_value(disp_reset_gpio, deassert) failed: %d", ret);
		return ret;
	}
	mdelay(100);

	return ret;
}

int mipi_tx_set_cmd(struct cmd_info_s *cmd_info)
{
	if (cmd_info->cmd_size > CMD_MAX_NUM) {
		printf("cmd_size(%d) can't exceed %d!\n", cmd_info->cmd_size, CMD_MAX_NUM);
		return -EINVAL;
	} else if ((cmd_info->cmd_size != 0) && !cmd_info->cmd) {
		printf("cmd is NULL, but cmd_size(%d) isn't zero!\n", cmd_info->cmd_size);
		return -EINVAL;
	}

#if 0
	if (cmd_info->cmd_size > 2)
		pr_info("%s: %#x %#x %#x %#x\n", __func__, cmd_info->cmd[0], cmd_info->cmd[1]
			, cmd_info->cmd[2], cmd_info->cmd[3]);
	else
		pr_info("%s: %#x %#x\n", __func__, cmd_info->cmd[0], cmd_info->cmd[1]);
#endif

	return sclr_dsi_dcs_write_buffer(cmd_info->data_type, cmd_info->cmd, cmd_info->cmd_size, cmd_mode & 0x01);
}

int mipi_tx_get_cmd(struct get_cmd_info_s *get_cmd_info)
{
	int ret = 0;

	if (get_cmd_info->get_data_size > RX_MAX_NUM) {
		printf("get_data_size(%d) can't exceed %d!\n", get_cmd_info->get_data_size, RX_MAX_NUM);
		return -EINVAL;
	} else if ((get_cmd_info->get_data_size != 0) && (!get_cmd_info->get_data)) {
		printf("cmd is NULL, but cmd_size(%d) isn't zero!\n", get_cmd_info->get_data_size);
		return -EINVAL;
	}

	if (pixel_clk)
		dphy_dsi_set_pll(pixel_clk * 2, lane_num, bits);
	ret = sclr_dsi_dcs_read_buffer(get_cmd_info->data_type, get_cmd_info->data_param
		, get_cmd_info->get_data, get_cmd_info->get_data_size, cmd_mode & 0x01);
	if (pixel_clk)
		dphy_dsi_set_pll(pixel_clk, lane_num, bits);

	return ret;
}

void mipi_tx_set_mode(unsigned long mode_flags)
{
	if (mode_flags)
		sclr_dsi_set_mode(SCLR_DSI_MODE_HS);
	else
		sclr_dsi_set_mode(SCLR_DSI_MODE_IDLE);
}
