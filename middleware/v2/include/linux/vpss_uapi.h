/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: vpss_uapi.h
 * Description:
 */

#ifndef _U_VPSS_UAPI_H_
#define _U_VPSS_UAPI_H_

#include <linux/cvi_comm_vpss.h>
#include <linux/cvi_comm_sys.h>

#ifdef __cplusplus
extern "C" {
#endif

struct vpss_crt_grp_cfg {
	VPSS_GRP VpssGrp;
	VPSS_GRP_ATTR_S stGrpAttr;
};

struct vpss_str_grp_cfg {
	VPSS_GRP VpssGrp;
};

struct vpss_grp_attr {
	VPSS_GRP VpssGrp;
	VPSS_GRP_ATTR_S stGrpAttr;
};

struct vpss_grp_crop_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CROP_INFO_S stCropInfo;
};

struct vpss_grp_frame_cfg {
	VPSS_GRP VpssGrp;
	VIDEO_FRAME_INFO_S stVideoFrame;
};

struct vpss_snd_frm_cfg {
	__u8 VpssGrp;
	VIDEO_FRAME_INFO_S stVideoFrame;
	__s32 s32MilliSec;
};

struct vpss_chn_frm_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VIDEO_FRAME_INFO_S stVideoFrame;
	CVI_S32 s32MilliSec;
};

struct vpss_chn_attr {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_CHN_ATTR_S stChnAttr;
};

struct vpss_en_chn_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
};

struct vpss_chn_crop_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_CROP_INFO_S stCropInfo;
};

struct vpss_chn_rot_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	ROTATION_E enRotation;
};

struct vpss_chn_ldc_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	ROTATION_E enRotation;
	VPSS_LDC_ATTR_S stLDCAttr;
	CVI_U64 meshHandle;
};

struct vpss_chn_align_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	CVI_U32 u32Align;
};

struct vpss_chn_yratio_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	CVI_U32 YRatio;
};

struct vpss_chn_coef_level_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_SCALE_COEF_E enCoef;
};

struct vpss_chn_buf_wrap_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_CHN_BUF_WRAP_S stBufWrap;
};

// prevent mw build error
struct vpss_chn_wrap_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VPSS_CHN_BUF_WRAP_S wrap;
};

/* prevent mw build error */
struct vpss_get_chn_frm_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VIDEO_FRAME_INFO_S stFrameInfo;
	CVI_S32 s32MilliSec;
};

struct cvi_vpss_vc_sb_cfg {
	__u8 img_inst;
	__u8 sc_inst;
	__u8 img_in_src_sel; /* 0: ISP, 2: DRAM */
	__u8 img_in_isp; /* 0: trig by reg_img_in_x_trl, 1: trig by isp vsync */
	__u32 img_in_width;
	__u32 img_in_height;
	__u32 img_in_fmt;
	__u64 img_in_address[3];
	__u32 odma_width;
	__u32 odma_height;
	__u32 odma_fmt;
	__u64 odma_address[3];
};

struct vpss_grp_csc_cfg {
	VPSS_GRP VpssGrp;
	CVI_S32 proc_amp[PROC_AMP_MAX];
	__u16 coef[3][3];
	__u8 sub[3];
	__u8 add[3];
	__u8 scene;
};

struct vpss_int_normalize {
	__u8 enable;
	__u16 sc_frac[3];
	__u8  sub[3];
	__u16 sub_frac[3];
	__u8 rounding;
};

struct vpss_vb_pool_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	__u32 hVbPool;
};

struct vpss_snap_cfg {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	__u32 frame_cnt;
};

struct vpss_bld_cfg {
	__u8 enable;
	__u8 fix_alpha;
	__u8 blend_y;
	__u8 y2r_enable;
	__u16 alpha_factor;
	__u16 alpha_stp;
	__u16 wd;
};

struct vpss_proc_amp_ctrl_cfg {
	PROC_AMP_E type;
	PROC_AMP_CTRL_S ctrl;
};

struct vpss_proc_amp_cfg {
	VPSS_GRP VpssGrp;
	CVI_S32 proc_amp[PROC_AMP_MAX];
};

struct vpss_all_proc_amp_cfg {
	CVI_S32 proc_amp[VPSS_MAX_GRP_NUM][PROC_AMP_MAX];
};

/*for record ISP bindata secne*/
struct vpss_scene {
	VPSS_GRP VpssGrp;
	CVI_U8 scene;
};

/* Public */
#define CVI_VPSS_CREATE_GROUP _IOW('S', 0x00, struct vpss_crt_grp_cfg)
#define CVI_VPSS_DESTROY_GROUP _IOW('S', 0x01, VPSS_GRP)
#define CVI_VPSS_GET_AVAIL_GROUP _IOWR('S', 0x02, VPSS_GRP)
#define CVI_VPSS_START_GROUP _IOW('S', 0x03, struct vpss_str_grp_cfg)
#define CVI_VPSS_STOP_GROUP _IOW('S', 0x04, VPSS_GRP)
#define CVI_VPSS_RESET_GROUP _IOW('S', 0x05, VPSS_GRP)
#define CVI_VPSS_SET_GRP_ATTR _IOW('S', 0x06, struct vpss_grp_attr)
#define CVI_VPSS_GET_GRP_ATTR _IOWR('S', 0x07, struct vpss_grp_attr)
#define CVI_VPSS_SET_GRP_CROP _IOW('S', 0x08, struct vpss_grp_crop_cfg)
#define CVI_VPSS_GET_GRP_CROP _IOWR('S', 0x09, struct vpss_grp_crop_cfg)
#define CVI_VPSS_GET_GRP_FRAME _IOWR('S', 0x0a, struct vpss_grp_frame_cfg)
#define CVI_VPSS_SET_RELEASE_GRP_FRAME _IOW('S', 0x0b, struct vpss_grp_frame_cfg)
#define CVI_VPSS_SEND_FRAME _IOW('S', 0x0c, struct vpss_snd_frm_cfg)

#define CVI_VPSS_SEND_CHN_FRAME _IOW('S', 0x20, struct vpss_chn_frm_cfg)
#define CVI_VPSS_SET_CHN_ATTR _IOW('S', 0x21, struct vpss_chn_attr)
#define CVI_VPSS_GET_CHN_ATTR _IOWR('S', 0x22, struct vpss_chn_attr)
#define CVI_VPSS_ENABLE_CHN _IOW('S', 0x23, struct vpss_en_chn_cfg)
#define CVI_VPSS_DISABLE_CHN _IOW('S', 0x24, struct vpss_en_chn_cfg)
#define CVI_VPSS_SET_CHN_CROP _IOW('S', 0x25, struct vpss_chn_crop_cfg)
#define CVI_VPSS_GET_CHN_CROP _IOWR('S', 0x26, struct vpss_chn_crop_cfg)
#define CVI_VPSS_SET_CHN_ROTATION _IOW('S', 0x27, struct vpss_chn_rot_cfg)
#define CVI_VPSS_GET_CHN_ROTATION _IOWR('S', 0x28, struct vpss_chn_rot_cfg)
#define CVI_VPSS_SET_CHN_LDC _IOW('S', 0x29, struct vpss_chn_ldc_cfg)
#define CVI_VPSS_GET_CHN_LDC _IOWR('S', 0x2a, struct vpss_chn_ldc_cfg)
#define CVI_VPSS_GET_CHN_FRAME _IOWR('S', 0x2b, struct vpss_chn_frm_cfg)
#define CVI_VPSS_RELEASE_CHN_FRAME _IOWR('S', 0x2c, struct vpss_chn_frm_cfg)
#define CVI_VPSS_SET_CHN_ALIGN _IOW('S', 0x2d, struct vpss_chn_align_cfg)
#define CVI_VPSS_GET_CHN_ALIGN _IOWR('S', 0x2e, struct vpss_chn_align_cfg)
#define CVI_VPSS_SET_CHN_YRATIO _IOW('S', 0x2f, struct vpss_chn_yratio_cfg)
#define CVI_VPSS_GET_CHN_YRATIO _IOWR('S', 0x30, struct vpss_chn_yratio_cfg)
#define CVI_VPSS_SET_CHN_SCALE_COEFF_LEVEL _IOW('S', 0x31, struct vpss_chn_coef_level_cfg)
#define CVI_VPSS_GET_CHN_SCALE_COEFF_LEVEL _IOWR('S', 0x32, struct vpss_chn_coef_level_cfg)
#define CVI_VPSS_SHOW_CHN _IOW('S', 0x33, struct vpss_en_chn_cfg)
#define CVI_VPSS_HIDE_CHN _IOW('S', 0x34, struct vpss_en_chn_cfg)
#define CVI_VPSS_SET_CHN_BUF_WRAP _IOW('S', 0x35, struct vpss_chn_wrap_cfg)
#define CVI_VPSS_GET_CHN_BUF_WRAP _IOWR('S', 0x36, struct vpss_chn_wrap_cfg)
#define CVI_VPSS_ATTACH_VB_POOL _IOW('S', 0x37, struct vpss_vb_pool_cfg)
#define CVI_VPSS_DETACH_VB_POOL _IOW('S', 0x38, struct vpss_vb_pool_cfg)
#define CVI_VPSS_TRIGGER_SNAP_FRAME _IOW('S', 0x39, struct vpss_snap_cfg)

/* Internal use */
#define CVI_VPSS_SET_MODE _IOW('S', 0x75, __u32)
#define CVI_VPSS_SET_MODE_EX _IOW('S', 0x76, VPSS_MODE_S)

#define CVI_VPSS_SET_GRP_CSC_CFG _IOW('S', 0x78, struct vpss_grp_csc_cfg)
#define CVI_VPSS_SET_BLD_CFG _IOW('S', 0x79, struct vpss_bld_cfg)
#define CVI_VPSS_GET_AMP_CTRL _IOWR('S', 0x7a, struct vpss_proc_amp_ctrl_cfg)
#define CVI_VPSS_GET_AMP_CFG _IOWR('S', 0x7b, struct vpss_proc_amp_cfg)
#define CVI_VPSS_GET_ALL_AMP _IOWR('S', 0x7c, struct vpss_all_proc_amp_cfg)
#define CVI_VPSS_GET_SCENE _IOWR('S', 0x7d, struct vpss_scene)

#ifdef __cplusplus
}
#endif

#endif /* _U_VPSS_UAPI_H_ */
