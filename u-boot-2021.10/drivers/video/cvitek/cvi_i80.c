/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 */

#include <common.h>
#include <clk.h>
#include <asm/gpio.h>
//#include <asm/hardware.h>
#include <asm/io.h>

#include "reg.h"
#include "vip_common.h"
#include "scaler.h"
#include "cvi_i80.h"
#include "dsi_phy.h"

unsigned char i80_ctrl[I80_CTRL_MAX] = { 0x31, 0x75, 0xff };

unsigned int _MAKECOLOR(unsigned char r, unsigned char g, unsigned char b, enum _VO_I80_FORMAT fmt)
{
	unsigned char r1, g1, b1;
	unsigned char r_len, g_len, b_len;

	switch (fmt) {
	case VO_I80_FORMAT_RGB444:
		r_len = 4;
		g_len = 4;
		b_len = 4;
		break;

	default:
	case VO_I80_FORMAT_RGB565:
		r_len = 5;
		g_len = 6;
		b_len = 5;
		break;

	case VO_I80_FORMAT_RGB666:
		r_len = 6;
		g_len = 6;
		b_len = 6;
		break;
	}

	r1 = r >> (8 - r_len);
	g1 = g >> (8 - g_len);
	b1 = b >> (8 - b_len);
	return (b1 | (g1 << b_len) | (r1 << (b_len + g_len)));
}

#if 1
/* 90 rotation */
void i80_package_frame(unsigned char *in, unsigned char *out, unsigned int stride, unsigned char byte_cnt,
		       unsigned short w, unsigned short h)
{
	unsigned int out_offset = 0;
	unsigned int pixel;
	int x, y;
	unsigned char (*b_in)[960];//BMP BGR888, 320x240x2
	unsigned char (*b_out)[1472];

	b_in = (void *)in;
	b_out = (void *)out;

	for (y = 0; y < h; ++y) {
		out_offset = 0;
		for (x = 0; x < w; ++x) {
			//_get_frame_rgb(&in, stride, x, y, &r, &g, &b);
			//pixel = _MAKECOLOR(r, g, b, VO_I80_FORMAT_RGB565);
			pixel = _MAKECOLOR(b_in[w - 1 - x][y * byte_cnt + 2], b_in[w - 1 - x][y * byte_cnt + 1],
					   b_in[w - 1 - x][y * byte_cnt], VO_I80_FORMAT_RGB565);
			b_out[y][out_offset++] = pixel >> 8;
			b_out[y][out_offset++] = i80_ctrl[I80_CTRL_DATA];
			b_out[y][out_offset++] = I80_OP_GO;
			b_out[y][out_offset++] = pixel & 0xFF;
			b_out[y][out_offset++] = i80_ctrl[I80_CTRL_DATA];
			b_out[y][out_offset++] = I80_OP_GO;
		}
		b_out[y][w * byte_cnt * 2 + 0] = 0xff;
		b_out[y][w * byte_cnt * 2 + 1] = i80_ctrl[I80_CTRL_EOF];
		b_out[y][w * byte_cnt * 2 + 2] = I80_OP_GO;
	}
	b_out[h - 1][w * byte_cnt * 2 + 0] = 0x00;
	b_out[h - 1][w * byte_cnt * 2 + 1] = i80_ctrl[I80_CTRL_EOF];
	b_out[h - 1][w * byte_cnt * 2 + 2] = I80_OP_DONE;
}
#else
static void _i80_package_eol(unsigned char buffer[3])
{
	buffer[0] = 0xff;
	buffer[1] = i80_ctrl[I80_CTRL_EOF];
	buffer[2] = I80_OP_GO;
}

static void _i80_package_eof(unsigned char buffer[3])
{
	buffer[0] = 0x00;
	buffer[1] = i80_ctrl[I80_CTRL_EOF];
	buffer[2] = I80_OP_DONE;
}

static void _i80_package_rgb(unsigned char r, unsigned char g, unsigned char b,
			     unsigned char *buffer, unsigned char byte_cnt)
{
	unsigned int pixel;

	pixel = _MAKECOLOR(r, g, b, VO_I80_FORMAT_RGB565);

	for (unsigned char i = 0, offset = 0; i < byte_cnt; ++i) {
		*(buffer + offset++) = pixel >> ((byte_cnt - i - 1) << 3);
		*(buffer + offset++) = i80_ctrl[I80_CTRL_DATA];
		*(buffer + offset++) = I80_OP_GO;
	}
}

static void _get_frame_rgb(unsigned char **buf, unsigned int stride, unsigned short x, unsigned short y,
			   unsigned char *r, unsigned char *g, unsigned char *b)
{
	unsigned int offset = 3 * x + stride * y;
	//bgr
	*b = *(buf[0] + offset);
	*g = *(buf[0] + offset + 1);
	*r = *(buf[0] + offset + 2);
}

void i80_package_frame(unsigned char *in, unsigned char *out, unsigned int stride, unsigned char byte_cnt,
		       unsigned short w, unsigned short h)
{
	unsigned int out_offset = 0;
	unsigned short line_data = (1 + w * byte_cnt) * 3;
	unsigned short padding = ALIGN(line_data, 32) - line_data;
	unsigned char r, g, b;

	for (int y = (h - 1); y >= 0; --y) {
		for (int x = 0; x < w; ++x) {
			_get_frame_rgb(&in, stride, x, y, &r, &g, &b);
			_i80_package_rgb(r, g, b, out + out_offset, byte_cnt);
			out_offset += byte_cnt * 3;
		}
		_i80_package_eol(out + out_offset);
		out_offset += 3;
		memset(out + out_offset, 0, padding);
		out_offset += padding;
	}
	// replace last eol with eof
	_i80_package_eof(out + out_offset - 3 - padding);
}
#endif

static void _fill_disp_timing(struct sclr_disp_timing *timing, const struct sync_info_s *sync_info)
{
	timing->vtotal = sync_info->vid_vsa_lines + sync_info->vid_vbp_lines
			 + sync_info->vid_active_lines + sync_info->vid_vfp_lines;
	timing->htotal = sync_info->vid_hsa_pixels + sync_info->vid_hbp_pixels
			 + sync_info->vid_hline_pixels + sync_info->vid_hfp_pixels;
	timing->vsync_start = 1;
	timing->vsync_end = timing->vsync_start + sync_info->vid_vsa_lines;
	timing->vfde_start = timing->vsync_end + sync_info->vid_vbp_lines;
	timing->vfde_end = timing->vfde_start + sync_info->vid_active_lines - 1;
	timing->hsync_start = 1;
	timing->hsync_end = timing->hsync_start + sync_info->vid_hsa_pixels;
	timing->hfde_start = timing->hsync_end + sync_info->vid_hbp_pixels;
	timing->hfde_end = timing->hfde_start + sync_info->vid_hline_pixels - 1;
	timing->vsync_pol = sync_info->vid_vsa_pos_polarity;
	timing->hsync_pol = sync_info->vid_hsa_pos_polarity;

	timing->vmde_start = timing->vfde_start;
	timing->vmde_end = timing->vfde_end;
	timing->hmde_start = timing->hfde_start;
	timing->hmde_end = timing->hfde_end;
}

int i80_set_combo_dev_cfg(const struct _VO_I80_CFG_S *i80_cfg)
{
	int ret;
	unsigned int clkkHz;

	union sclr_bt_enc enc;
	union sclr_bt_sync_code sync;
	struct sclr_disp_timing timing;

	_fill_disp_timing(&timing, &i80_cfg->sync_info);
	sclr_disp_set_intf(SCLR_VO_INTF_I80);
	enc.raw = 0;
	enc.b.fmt_sel = 2;
	enc.b.clk_inv = 1;
	sync.raw = 0;
	sync.b.sav_vld = 0x80;
	sync.b.sav_blk = 0xab;
	sync.b.eav_vld = 0x9d;
	sync.b.eav_blk = 0xb6;
	sclr_bt_set(enc, sync);

	i80_ctrl[I80_CTRL_CMD] = BIT(i80_cfg->lane_s.RD) | ((BIT(i80_cfg->lane_s.RD) | BIT(i80_cfg->lane_s.WR)) << 4);
	i80_ctrl[I80_CTRL_DATA] = (BIT(i80_cfg->lane_s.RD) | BIT(i80_cfg->lane_s.RS)) |
				  ((BIT(i80_cfg->lane_s.RD) | BIT(i80_cfg->lane_s.WR) | BIT(i80_cfg->lane_s.RS)) << 4);
	i80_ctrl[I80_CTRL_EOF] = 0xff;

	clkkHz = 1000000 / (i80_cfg->cycle_time / 2);
	if (clkkHz < 8000) {
		debug("i80 disp set clk less than 8000 kHz.\n");
		return -EINVAL;
	}
	dphy_dsi_set_pll(clkkHz, 4, 24);
	sclr_disp_set_timing(&timing);
	sclr_disp_tgen_enable(false);

	//#if defined(CONFIG_BOARD_ZAM180)
	_reg_write_mask(0x03021004, 0x1000010, 0x1000010); //backlight & reset
	_reg_write_mask(0x03021000, 0x1000010, 0x1000010);
	//#endif

	return ret;
}

void i80_set_sw_mode(unsigned long mode_flags)
{
	if (mode_flags)
		sclr_i80_sw_mode(SCLR_I80_MODE_SW);
	else
		sclr_i80_sw_mode(SCLR_I80_MODE_IDLE);
}

int i80_set_cmd(unsigned int cmd)
{
	int ret = 0;

	sclr_i80_packet(cmd);

	return ret;
}

int i80_set_run(void)
{
	int ret = 0;

	sclr_i80_run();

	return ret;
}

void i80_sclr_intr_clr(void)
{
	sclr_intr_clr(sclr_intr_status());
}

