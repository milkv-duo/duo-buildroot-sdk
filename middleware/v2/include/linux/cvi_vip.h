/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_vip.h
 * Description:
 */

#ifndef _U_CVI_VIP_H_
#define _U_CVI_VIP_H_
#include <linux/version.h>
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum cvi_vip_pattern {
	CVI_VIP_PAT_OFF = 0,
	CVI_VIP_PAT_SNOW,
	CVI_VIP_PAT_AUTO,
	CVI_VIP_PAT_RED,
	CVI_VIP_PAT_GREEN,
	CVI_VIP_PAT_BLUE,
	CVI_VIP_PAT_COLORBAR,
	CVI_VIP_PAT_GRAY_GRAD_H,
	CVI_VIP_PAT_GRAY_GRAD_V,
	CVI_VIP_PAT_BLACK,
	CVI_VIP_PAT_MAX,
};

enum cvi_rgn_format {
	CVI_RGN_FMT_ARGB8888,
	CVI_RGN_FMT_ARGB4444,
	CVI_RGN_FMT_ARGB1555,
	CVI_RGN_FMT_256LUT,
	CVI_RGN_FMT_16LUT,
	CVI_RGN_FMT_FONT,
	CVI_RGN_FMT_MAX
};

struct cvi_vpss_rect {
	__s32 left;
	__s32 top;
	__u32 width;
	__u32 height;
};

struct cvi_vpss_frmsize {
	__u32 width;
	__u32 height;
};

struct cvi_rgn_param {
	enum cvi_rgn_format fmt;
	struct cvi_vpss_rect rect;
	__u32 stride;
	__u64 phy_addr;
};

struct cvi_rgn_odec {
	__u8 enable;
	__u8 attached_ow;
	__u32 bso_sz;
};

struct cvi_rgn_cfg {
	struct cvi_rgn_param param[8];
	struct cvi_rgn_odec odec;
	__u8 num_of_rgn;
	__u8 hscale_x2;
	__u8 vscale_x2;
	__u8 colorkey_en;
	__u32 colorkey;
};

struct cvi_rgn_ex_cfg {
	struct cvi_rgn_param rgn_ex_param[16];
	struct cvi_rgn_odec odec;
	__u8 num_of_rgn_ex;
	__u8 hscale_x2;
	__u8 vscale_x2;
	__u8 colorkey_en;
	__u32 colorkey;
};

struct cvi_rgn_coverex_param {
	struct cvi_vpss_rect rect;
	__u32 color;
	__u8 enable;
};

struct cvi_rgn_coverex_cfg {
	struct cvi_rgn_coverex_param rgn_coverex_param[4];
};

struct cvi_rgn_mosaic_cfg {
	__u8 enable;
	__u8 blk_size;  //0: 8x8   1:16x16
	__u16 start_x;
	__u16 start_y;
	__u16 end_x;
	__u16 end_y;
	__u64 phy_addr;
};

struct cvi_rgn_lut_cfg {
	__u16 lut_length;
	__u16 *lut_addr;
	__u8 lut_layer;
	__u8 rgnex_en;
};

struct cvi_csc_cfg {
	__u16 coef[3][3];
	__u8 sub[3];
	__u8 add[3];
};

enum cvi_input_type {
	CVI_VIP_INPUT_ISP = 0,
	CVI_VIP_INPUT_DWA,
	CVI_VIP_INPUT_MEM,
	CVI_VIP_INPUT_ISP_POST,
	CVI_VIP_INPUT_MAX,
};

struct cvi_vpss_grp_cfg {
	__u8 grp_id;
	__u8 chn_enable[4];
	struct cvi_vpss_frmsize src_size;
	__u32 pixelformat;
	__u32 bytesperline[2];
	struct cvi_vpss_rect crop;
	struct cvi_csc_cfg csc_cfg;
};

struct cvi_vpss_rgnex_cfg {
	struct cvi_rgn_ex_cfg cfg;
	__u32 pixelformat;
	__u32 bytesperline[2];
	__u64 addr[3];
};

struct cvi_vip_plane {
	__u32 length;
	__u64 addr;
};

#ifdef __cplusplus
}
#endif

#endif /* _U_CVI_VIP_H_ */
