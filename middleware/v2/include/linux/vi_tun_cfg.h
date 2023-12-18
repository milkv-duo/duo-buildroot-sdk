/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_vip_tun_cfg.h
 * Description:
 */

#ifndef _U_CVI_VIP_TUN_CFG_H_
#define _U_CVI_VIP_TUN_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "isp_reg.h"

#define TUNING_NODE_NUM  2

enum cvi_isp_raw {
	ISP_PRERAW_A,
	ISP_PRERAW_B,
	ISP_PRERAW_C,
	ISP_PRERAW_MAX,
	ISP_PRERAW_VIRT_A = ISP_PRERAW_MAX,
	ISP_PRERAW_VIRT_B,
	ISP_PRERAW_VIRT_MAX,
};

enum cvi_isp_chn_num {
	ISP_CHN0,
	ISP_CHN1,
	ISP_CHN2,
	ISP_CHN3,
	ISP_CHN_MAX,
};

enum cvi_isp_pre_chn_num {
	ISP_FE_CH0,
	ISP_FE_CH1,
	ISP_FE_CH2,
	ISP_FE_CH3,
	ISP_FE_CHN_MAX,
};

enum cvi_isp_be_chn_num {
	ISP_BE_CH0,
	ISP_BE_CH1,
	ISP_BE_CHN_MAX,
};

struct cvi_vip_isp_blc_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	__u8  bypass;
	__u16 roffset;
	__u16 groffset;
	__u16 gboffset;
	__u16 boffset;
	__u16 roffset_2nd;
	__u16 groffset_2nd;
	__u16 gboffset_2nd;
	__u16 boffset_2nd;
	__u16 rgain;
	__u16 grgain;
	__u16 gbgain;
	__u16 bgain;
};

struct cvi_vip_isp_wbg_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	__u8  bypass;
	__u16 rgain;
	__u16 ggain;
	__u16 bgain;
	__u32 rgain_fraction;
	__u32 ggain_fraction;
	__u32 bgain_fraction;
};

/* struct cvi_vip_isp_ccm_config
 * @enable: ccm module enable or not
 * @coef: s3.10, (2's complement)
 */
struct cvi_vip_isp_ccm_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	__u16 coef[3][3];
};

struct cvi_vip_isp_cacp_config {
	__u8  update;
	__u8  enable;
	__u8  mode;
	__u16 iso_ratio;
	__u16 ca_y_ratio_lut[256];
	__u8  cp_y_lut[256];
	__u8  cp_u_lut[256];
	__u8  cp_v_lut[256];
};

struct cvi_vip_isp_ca2_config {
	__u8  update;
	__u8  enable;
	__u16 lut_in[6];
	__u16 lut_out[6];
	__u16 lut_slp[5];
};

struct cvi_vip_isp_ygamma_config {
	__u8  update;
	__u8  enable;
	__u32 max;
	__u16 lut[256];
};

struct cvi_vip_isp_gamma_config {
	__u8  update;
	__u8  enable;
	__u16 max;
	__u16 lut[256];
};

struct cvi_isp_demosiac_tun_cfg {
	union REG_ISP_CFA_0C                     REG_0C;
	union REG_ISP_CFA_10                     REG_10;
	union REG_ISP_CFA_14                     REG_14;
	union REG_ISP_CFA_18                     REG_18;
	union REG_ISP_CFA_1C                     REG_1C;
};

struct cvi_isp_demosiac_tun_1_cfg {
	union REG_ISP_CFA_120                   REG_120;
	union REG_ISP_CFA_124                   REG_124;
	union REG_ISP_CFA_128                   REG_128;
	union REG_ISP_CFA_12C                   REG_12C;
	union REG_ISP_CFA_130                   REG_130;
	union REG_ISP_CFA_134                   REG_134;
	union REG_ISP_CFA_138                   REG_138;
	union REG_ISP_CFA_13C                   REG_13C;
	union REG_ISP_CFA_140                   REG_140;
	union REG_ISP_CFA_144                   REG_144;
	union REG_ISP_CFA_148                   REG_148;
	union REG_ISP_CFA_14C                   REG_14C;
	union REG_ISP_CFA_150                   REG_150;
	union REG_ISP_CFA_154                   REG_154;
	union REG_ISP_CFA_158                   REG_158;
	union REG_ISP_CFA_15C                   REG_15C;
	union REG_ISP_CFA_160                   REG_160;
	union REG_ISP_CFA_164                   REG_164;
	union REG_ISP_CFA_168                   REG_168;
	union REG_ISP_CFA_16C                   REG_16C;
	union REG_ISP_CFA_170                   REG_170;
	union REG_ISP_CFA_174                   REG_174;
	union REG_ISP_CFA_178                   REG_178;
	union REG_ISP_CFA_17C                   REG_17C;
	union REG_ISP_CFA_180                   REG_180;
	union REG_ISP_CFA_184                   REG_184;
	union REG_ISP_CFA_188                   REG_188;
	union REG_ISP_CFA_18C                   REG_18C;
};

struct cvi_isp_demosiac_tun_2_cfg {
	union REG_ISP_CFA_90                    REG_90;
	union REG_ISP_CFA_94                    REG_94;
	union REG_ISP_CFA_98                    REG_98;
	union REG_ISP_CFA_9C                    REG_9C;
	union REG_ISP_CFA_A0                    REG_A0;
	union REG_ISP_CFA_A4                    REG_A4;
	union REG_ISP_CFA_A8                    REG_A8;
};

struct cvi_vip_isp_demosiac_config {
	__u8  update;
	__u8  cfa_enable;
	__u16 cfa_edgee_thd2;
	__u8  cfa_out_sel;
	__u8  cfa_force_dir_enable;
	__u8  cfa_force_dir_sel;
	__u16 cfa_rbsig_luma_thd;
	__u8  cfa_ghp_lut[32];
	__u8  cfa_ymoire_enable;
	__u8  cfa_ymoire_dc_w;
	__u8  cfa_ymoire_lpf_w;
	struct cvi_isp_demosiac_tun_cfg demosiac_cfg;
	struct cvi_isp_demosiac_tun_1_cfg demosiac_1_cfg;
	struct cvi_isp_demosiac_tun_2_cfg demosiac_2_cfg;
};

struct cvi_vip_isp_lsc_config {
	__u8  update;
	__u8  enable;
	__u16 strength;
	__u8  debug;
	__u8  gain_base;
	__u8  gain_3p9_0_4p8_1;
	__u8  renormalize_enable;
	__u8  gain_bicubic_0_bilinear_1;
	__u8  boundary_interpolation_mode;
	__u8  boundary_interpolation_lf_range;
	__u8  boundary_interpolation_up_range;
	__u8  boundary_interpolation_rt_range;
	__u8  boundary_interpolation_dn_range;
	__u8  bldratio_enable;
	__u16 bldratio;
	__u32 intp_gain_max;
	__u32 intp_gain_min;
};

struct cvi_isp_bnr_tun_1_cfg {
	union REG_ISP_BNR_NS_LUMA_TH_R          NS_LUMA_TH_R;
	union REG_ISP_BNR_NS_SLOPE_R            NS_SLOPE_R;
	union REG_ISP_BNR_NS_OFFSET0_R          NS_OFFSET0_R;
	union REG_ISP_BNR_NS_OFFSET1_R          NS_OFFSET1_R;
	union REG_ISP_BNR_NS_LUMA_TH_GR         NS_LUMA_TH_GR;
	union REG_ISP_BNR_NS_SLOPE_GR           NS_SLOPE_GR;
	union REG_ISP_BNR_NS_OFFSET0_GR         NS_OFFSET0_GR;
	union REG_ISP_BNR_NS_OFFSET1_GR         NS_OFFSET1_GR;
	union REG_ISP_BNR_NS_LUMA_TH_GB         NS_LUMA_TH_GB;
	union REG_ISP_BNR_NS_SLOPE_GB           NS_SLOPE_GB;
	union REG_ISP_BNR_NS_OFFSET0_GB         NS_OFFSET0_GB;
	union REG_ISP_BNR_NS_OFFSET1_GB         NS_OFFSET1_GB;
	union REG_ISP_BNR_NS_LUMA_TH_B          NS_LUMA_TH_B;
	union REG_ISP_BNR_NS_SLOPE_B            NS_SLOPE_B;
	union REG_ISP_BNR_NS_OFFSET0_B          NS_OFFSET0_B;
	union REG_ISP_BNR_NS_OFFSET1_B          NS_OFFSET1_B;
	union REG_ISP_BNR_NS_GAIN               NS_GAIN;
	union REG_ISP_BNR_STRENGTH_MODE         STRENGTH_MODE;
};

struct cvi_isp_bnr_tun_2_cfg {
	union REG_ISP_BNR_VAR_TH                VAR_TH;
	union REG_ISP_BNR_WEIGHT_LUT            WEIGHT_LUT;
	union REG_ISP_BNR_WEIGHT_SM             WEIGHT_SM;
	union REG_ISP_BNR_WEIGHT_V              WEIGHT_V;
	union REG_ISP_BNR_WEIGHT_H              WEIGHT_H;
	union REG_ISP_BNR_WEIGHT_D45            WEIGHT_D45;
	union REG_ISP_BNR_WEIGHT_D135           WEIGHT_D135;
	union REG_ISP_BNR_NEIGHBOR_MAX          NEIGHBOR_MAX;
};

struct cvi_vip_isp_bnr_config {
	__u8  update;
	__u8  enable;
	__u8  out_sel;
	__u8  weight_lut[256];
	__u8  intensity_sel[8];
	__u8  weight_intra_0;
	__u8  weight_intra_1;
	__u8  weight_intra_2;
	__u8  weight_norm_1;
	__u8  weight_norm_2;
	__u16 k_smooth;
	__u16 k_texture;
	struct cvi_isp_bnr_tun_1_cfg bnr_1_cfg;
	struct cvi_isp_bnr_tun_2_cfg bnr_2_cfg;
};

struct cvi_vip_isp_clut_config {
	__u8  update;
	__u8  enable;
	__u8  is_update_partial;
	__u8  tbl_idx;
	__u16 r_lut[4913];
	__u16 g_lut[4913];
	__u16 b_lut[4913];
	__u16 update_length;
	__u32 lut[1024][2]; //0:addr, 1:value
};

struct cvi_isp_drc_tun_1_cfg {
	union REG_LTM_H90                       REG_H90;
	union REG_LTM_H94                       REG_H94;
	union REG_LTM_H98                       REG_H98;
	union REG_LTM_H9C                       REG_H9C;
	union REG_LTM_HA0                       REG_HA0;
	union REG_LTM_HA4                       REG_HA4;
	union REG_LTM_HA8                       REG_HA8;
	union REG_LTM_HAC                       REG_HAC;
	union REG_LTM_HB0                       REG_HB0;
	union REG_LTM_HB4                       REG_HB4;
	union REG_LTM_HB8                       REG_HB8;
	union REG_LTM_HBC                       REG_HBC;
	union REG_LTM_HC0                       REG_HC0;
	union REG_LTM_HC4                       REG_HC4;
	union REG_LTM_HC8                       REG_HC8;
	union REG_LTM_HCC                       REG_HCC;
	union REG_LTM_HD0                       REG_HD0;
	union REG_LTM_HD4                       REG_HD4;
	union REG_LTM_HD8                       REG_HD8;
	union REG_LTM_HDC                       REG_HDC;
};

struct cvi_isp_drc_tun_2_cfg {
	union REG_LTM_H14                       REG_H14;
	union REG_LTM_H18                       REG_H18;
	union REG_LTM_H1C                       REG_H1C;
	union REG_LTM_H20                       REG_H20;
	union REG_LTM_H24                       REG_H24;
	union REG_LTM_H28                       REG_H28;
	union REG_LTM_H2C                       REG_H2C;
	union REG_LTM_H30                       REG_H30;
};

struct cvi_isp_drc_tun_3_cfg {
	union REG_LTM_H64                       REG_H64;
	union REG_LTM_H68                       REG_H68;
	union REG_LTM_H6C                       REG_H6C;
	union REG_LTM_H70                       REG_H70;
	union REG_LTM_H74                       REG_H74;
	union REG_LTM_H78                       REG_H78;
	union REG_LTM_H7C                       REG_H7C;
	union REG_LTM_H80                       REG_H80;
	union REG_LTM_H84                       REG_H84;
	union REG_LTM_H88                       REG_H88;
};

struct cvi_vip_isp_drc_config {
	__u8  update;
	__u8  ltm_enable;
	__u8  dark_enh_en;
	__u8  brit_enh_en;
	__u8  dbg_mode;
	__u8  dark_tone_wgt_refine_en;
	__u8  brit_tone_wgt_refine_en;
	__u16 global_lut[769];
	__u16 dark_lut[257];
	__u16 brit_lut[513];
	__u8  lmap_enable;
	__u8  lmap_w_bit;
	__u8  lmap_h_bit;
	__u16 lmap_thd_l;
	__u16 lmap_thd_h;
	__u8  lmap_y_mode;
	__u8  de_strth_dshft;
	__u16 de_strth_gain;
	__u8  be_strth_dshft;
	__u16 be_strth_gain;
	__u8  hdr_pattern;
	struct cvi_isp_drc_tun_1_cfg drc_1_cfg;
	struct cvi_isp_drc_tun_2_cfg drc_2_cfg;
	struct cvi_isp_drc_tun_3_cfg drc_3_cfg;
};

struct cvi_isp_ynr_tun_1_cfg {
	union REG_ISP_YNR_NS0_LUMA_TH_00        NS0_LUMA_TH_00;
	union REG_ISP_YNR_NS0_LUMA_TH_01        NS0_LUMA_TH_01;
	union REG_ISP_YNR_NS0_LUMA_TH_02        NS0_LUMA_TH_02;
	union REG_ISP_YNR_NS0_LUMA_TH_03        NS0_LUMA_TH_03;
	union REG_ISP_YNR_NS0_LUMA_TH_04        NS0_LUMA_TH_04;
	union REG_ISP_YNR_NS0_LUMA_TH_05        NS0_LUMA_TH_05;
	union REG_ISP_YNR_NS0_SLOPE_00          NS0_SLOPE_00;
	union REG_ISP_YNR_NS0_SLOPE_01          NS0_SLOPE_01;
	union REG_ISP_YNR_NS0_SLOPE_02          NS0_SLOPE_02;
	union REG_ISP_YNR_NS0_SLOPE_03          NS0_SLOPE_03;
	union REG_ISP_YNR_NS0_SLOPE_04          NS0_SLOPE_04;
	union REG_ISP_YNR_NS0_OFFSET_00         NS0_OFFSET_00;
	union REG_ISP_YNR_NS0_OFFSET_01         NS0_OFFSET_01;
	union REG_ISP_YNR_NS0_OFFSET_02         NS0_OFFSET_02;
	union REG_ISP_YNR_NS0_OFFSET_03         NS0_OFFSET_03;
	union REG_ISP_YNR_NS0_OFFSET_04         NS0_OFFSET_04;
	union REG_ISP_YNR_NS0_OFFSET_05         NS0_OFFSET_05;
	union REG_ISP_YNR_NS1_LUMA_TH_00        NS1_LUMA_TH_00;
	union REG_ISP_YNR_NS1_LUMA_TH_01        NS1_LUMA_TH_01;
	union REG_ISP_YNR_NS1_LUMA_TH_02        NS1_LUMA_TH_02;
	union REG_ISP_YNR_NS1_LUMA_TH_03        NS1_LUMA_TH_03;
	union REG_ISP_YNR_NS1_LUMA_TH_04        NS1_LUMA_TH_04;
	union REG_ISP_YNR_NS1_LUMA_TH_05        NS1_LUMA_TH_05;
	union REG_ISP_YNR_NS1_SLOPE_00          NS1_SLOPE_00;
	union REG_ISP_YNR_NS1_SLOPE_01          NS1_SLOPE_01;
	union REG_ISP_YNR_NS1_SLOPE_02          NS1_SLOPE_02;
	union REG_ISP_YNR_NS1_SLOPE_03          NS1_SLOPE_03;
	union REG_ISP_YNR_NS1_SLOPE_04          NS1_SLOPE_04;
	union REG_ISP_YNR_NS1_OFFSET_00         NS1_OFFSET_00;
	union REG_ISP_YNR_NS1_OFFSET_01         NS1_OFFSET_01;
	union REG_ISP_YNR_NS1_OFFSET_02         NS1_OFFSET_02;
	union REG_ISP_YNR_NS1_OFFSET_03         NS1_OFFSET_03;
	union REG_ISP_YNR_NS1_OFFSET_04         NS1_OFFSET_04;
	union REG_ISP_YNR_NS1_OFFSET_05         NS1_OFFSET_05;
	union REG_ISP_YNR_NS_GAIN               NS_GAIN;
};

struct cvi_isp_ynr_tun_2_cfg {
	union REG_ISP_YNR_MOTION_LUT_00         MOTION_LUT_00;
	union REG_ISP_YNR_MOTION_LUT_01         MOTION_LUT_01;
	union REG_ISP_YNR_MOTION_LUT_02         MOTION_LUT_02;
	union REG_ISP_YNR_MOTION_LUT_03         MOTION_LUT_03;
	union REG_ISP_YNR_MOTION_LUT_04         MOTION_LUT_04;
	union REG_ISP_YNR_MOTION_LUT_05         MOTION_LUT_05;
	union REG_ISP_YNR_MOTION_LUT_06         MOTION_LUT_06;
	union REG_ISP_YNR_MOTION_LUT_07         MOTION_LUT_07;
	union REG_ISP_YNR_MOTION_LUT_08         MOTION_LUT_08;
	union REG_ISP_YNR_MOTION_LUT_09         MOTION_LUT_09;
	union REG_ISP_YNR_MOTION_LUT_10         MOTION_LUT_10;
	union REG_ISP_YNR_MOTION_LUT_11         MOTION_LUT_11;
	union REG_ISP_YNR_MOTION_LUT_12         MOTION_LUT_12;
	union REG_ISP_YNR_MOTION_LUT_13         MOTION_LUT_13;
	union REG_ISP_YNR_MOTION_LUT_14         MOTION_LUT_14;
	union REG_ISP_YNR_MOTION_LUT_15         MOTION_LUT_15;
};

struct cvi_isp_ynr_tun_3_cfg {
	union REG_ISP_YNR_ALPHA_GAIN            ALPHA_GAIN;
	union REG_ISP_YNR_VAR_TH                VAR_TH;
	union REG_ISP_YNR_WEIGHT_SM             WEIGHT_SM;
	union REG_ISP_YNR_WEIGHT_V              WEIGHT_V;
	union REG_ISP_YNR_WEIGHT_H              WEIGHT_H;
	union REG_ISP_YNR_WEIGHT_D45            WEIGHT_D45;
	union REG_ISP_YNR_WEIGHT_D135           WEIGHT_D135;
	union REG_ISP_YNR_NEIGHBOR_MAX          NEIGHBOR_MAX;
	union REG_ISP_YNR_RES_K_SMOOTH          RES_K_SMOOTH;
	union REG_ISP_YNR_RES_K_TEXTURE         RES_K_TEXTURE;
	union REG_ISP_YNR_FILTER_MODE_EN        FILTER_MODE_EN;
	union REG_ISP_YNR_FILTER_MODE_ALPHA     FILTER_MODE_ALPHA;
};

struct cvi_isp_ynr_tun_4_cfg {
	union REG_ISP_YNR_RES_MOT_LUT_00        RES_MOT_LUT_00;
	union REG_ISP_YNR_RES_MOT_LUT_01        RES_MOT_LUT_01;
	union REG_ISP_YNR_RES_MOT_LUT_02        RES_MOT_LUT_02;
	union REG_ISP_YNR_RES_MOT_LUT_03        RES_MOT_LUT_03;
	union REG_ISP_YNR_RES_MOT_LUT_04        RES_MOT_LUT_04;
	union REG_ISP_YNR_RES_MOT_LUT_05        RES_MOT_LUT_05;
	union REG_ISP_YNR_RES_MOT_LUT_06        RES_MOT_LUT_06;
	union REG_ISP_YNR_RES_MOT_LUT_07        RES_MOT_LUT_07;
	union REG_ISP_YNR_RES_MOT_LUT_08        RES_MOT_LUT_08;
	union REG_ISP_YNR_RES_MOT_LUT_09        RES_MOT_LUT_09;
	union REG_ISP_YNR_RES_MOT_LUT_10        RES_MOT_LUT_10;
	union REG_ISP_YNR_RES_MOT_LUT_11        RES_MOT_LUT_11;
	union REG_ISP_YNR_RES_MOT_LUT_12        RES_MOT_LUT_12;
	union REG_ISP_YNR_RES_MOT_LUT_13        RES_MOT_LUT_13;
	union REG_ISP_YNR_RES_MOT_LUT_14        RES_MOT_LUT_14;
	union REG_ISP_YNR_RES_MOT_LUT_15        RES_MOT_LUT_15;
};

struct cvi_vip_isp_ynr_config {
	__u8  update;
	__u8  enable;
	__u8  out_sel;
	__u8  weight_intra_0;
	__u8  weight_intra_1;
	__u8  weight_intra_2;
	__u8  weight_norm_1;
	__u8  weight_norm_2;
	__u8  res_max;
	__u8  res_motion_max;
	__u8  motion_ns_clip_max;
	__u8  weight_lut_h[64];
	struct cvi_isp_ynr_tun_1_cfg ynr_1_cfg;
	struct cvi_isp_ynr_tun_2_cfg ynr_2_cfg;
	struct cvi_isp_ynr_tun_3_cfg ynr_3_cfg;
	struct cvi_isp_ynr_tun_4_cfg ynr_4_cfg;
};

struct cvi_vip_isp_cnr_config {
	__u8  update;
	__u8  enable;
	__u8  strength_mode;
	__u8  diff_shift_val;
	__u8  diff_gain;
	__u8  ratio;
	__u8  fusion_intensity_weight;
	__u8  flag_neighbor_max_weight;
	__u8  weight_lut_inter[16];
	__u8  motion_enable;
	__u8  coring_motion_lut[16];
	__u8  motion_lut[16];
};

struct cvi_isp_tnr_tun_cfg {
	union REG_ISP_MMAP_0C                     REG_0C;
	union REG_ISP_MMAP_10                     REG_10;
	union REG_ISP_MMAP_14                     REG_14;
	union REG_ISP_MMAP_18                     REG_18;
};

struct cvi_isp_tnr_tun_5_cfg {
	union REG_ISP_MMAP_20                     REG_20;
	union REG_ISP_MMAP_24                     REG_24;
	union REG_ISP_MMAP_28                     REG_28;
	union REG_ISP_MMAP_2C                     REG_2C;
};

struct cvi_isp_tnr_tun_1_cfg {
	union REG_ISP_MMAP_4C                     REG_4C;
	union REG_ISP_MMAP_50                     REG_50;
	union REG_ISP_MMAP_54                     REG_54;
	union REG_ISP_MMAP_58                     REG_58;
	union REG_ISP_MMAP_5C                     REG_5C;
};

struct cvi_isp_tnr_tun_2_cfg {
	union REG_ISP_MMAP_70                     REG_70;
	union REG_ISP_MMAP_74                     REG_74;
	union REG_ISP_MMAP_78                     REG_78;
	union REG_ISP_MMAP_7C                     REG_7C;
	union REG_ISP_MMAP_80                     REG_80;
	union REG_ISP_MMAP_84                     REG_84;
	union REG_ISP_MMAP_88                     REG_88;
	union REG_ISP_MMAP_8C                     REG_8C;
	union REG_ISP_MMAP_90                     REG_90;
};

struct cvi_isp_tnr_tun_3_cfg {
	union REG_ISP_MMAP_A0                     REG_A0;
	union REG_ISP_MMAP_A4                     REG_A4;
	union REG_ISP_MMAP_A8                     REG_A8;
	union REG_ISP_MMAP_AC                     REG_AC;
	union REG_ISP_MMAP_B0                     REG_B0;
	union REG_ISP_MMAP_B4                     REG_B4;
	union REG_ISP_MMAP_B8                     REG_B8;
	union REG_ISP_MMAP_BC                     REG_BC;
	union REG_ISP_MMAP_C0                     REG_C0;
};

struct cvi_isp_tnr_tun_4_cfg {
	union REG_ISP_444_422_13                REG_13;
	union REG_ISP_444_422_14                REG_14;
	union REG_ISP_444_422_15                REG_15;
	union REG_ISP_444_422_16                REG_16;
	union REG_ISP_444_422_17                REG_17;
	union REG_ISP_444_422_18                REG_18;
	union REG_ISP_444_422_19                REG_19;
	union REG_ISP_444_422_20                REG_20;
	union REG_ISP_444_422_21                REG_21;
	union REG_ISP_444_422_22                REG_22;
	union REG_ISP_444_422_23                REG_23;
	union REG_ISP_444_422_24                REG_24;
	union REG_ISP_444_422_25                REG_25;
	union REG_ISP_444_422_26                REG_26;
	union REG_ISP_444_422_27                REG_27;
	union REG_ISP_444_422_28                REG_28;
	union REG_ISP_444_422_29                REG_29;
	union REG_ISP_444_422_30                REG_30;
	union REG_ISP_444_422_31                REG_31;
};

struct cvi_isp_tnr_tun_6_cfg {
	union REG_ISP_444_422_84                REG_84;
	union REG_ISP_444_422_88                REG_88;
	union REG_ISP_444_422_8C                REG_8C;
	union REG_ISP_444_422_90                REG_90;
	union REG_ISP_444_422_94                REG_94;
	union REG_ISP_444_422_98                REG_98;
	union REG_ISP_444_422_9C                REG_9C;
	union REG_ISP_444_422_A0                REG_A0;
	union REG_ISP_444_422_A4                REG_A4;
	union REG_ISP_444_422_A8                REG_A8;
	union REG_ISP_444_422_AC                REG_AC;
	union REG_ISP_444_422_B0                REG_B0;
	union REG_ISP_444_422_B4                REG_B4;
	union REG_ISP_444_422_B8                REG_B8;
	union REG_ISP_444_422_BC                REG_BC;
	union REG_ISP_444_422_C0                REG_C0;
	union REG_ISP_444_422_C4                REG_C4;
	union REG_ISP_444_422_C8                REG_C8;
	union REG_ISP_444_422_CC                REG_CC;
	union REG_ISP_444_422_D0                REG_D0;
	union REG_ISP_444_422_D4                REG_D4;
	union REG_ISP_444_422_D8                REG_D8;
	union REG_ISP_444_422_DC                REG_DC;
	union REG_ISP_444_422_E0                REG_E0;
	union REG_ISP_444_422_E4                REG_E4;
	union REG_ISP_444_422_E8                REG_E8;
	union REG_ISP_444_422_EC                REG_EC;
	union REG_ISP_444_422_F0                REG_F0;
};

struct cvi_isp_tnr_tun_7_cfg {
	union REG_ISP_MMAP_100                  REG_100;
	union REG_ISP_MMAP_104                  REG_104;
	union REG_ISP_MMAP_108                  REG_108;
	union REG_ISP_MMAP_10C                  REG_10C;
	union REG_ISP_MMAP_110                  REG_110;
	union REG_ISP_MMAP_114                  REG_114;
	union REG_ISP_MMAP_118                  REG_118;
	union REG_ISP_MMAP_11C                  REG_11C;
	union REG_ISP_MMAP_120                  REG_120;
	union REG_ISP_MMAP_124                  REG_124;
	union REG_ISP_MMAP_128                  REG_128;
};

struct cvi_vip_isp_tnr_config {
	__u8  update;
	__u8  manr_enable;
	__u8  rgbmap_w_bit;
	__u8  rgbmap_h_bit;
	__u8  mh_wgt;
	__u8  lpf[3][3];
	__u8  map_gain;
	__u8  map_thd_l;
	__u8  map_thd_h;
	__u8  uv_rounding_type_sel;
	__u8  history_sel_0;
	__u8  history_sel_1;
	__u8  history_sel_3;
	__u16 tdnr_debug_sel;
	__s16 luma_adapt_lut_slope_2;
	__u8  med_enable;
	__u16 med_wgt;
	__u8  mtluma_mode;
	__u8  avg_mode_write;
	__u8  drop_mode_write;
	__u8  tdnr_pixel_lp;
	__u8  tdnr_comp_gain_enable;
	__u16 tdnr_ee_comp_gain;
	struct cvi_isp_tnr_tun_cfg      tnr_cfg;
	struct cvi_isp_tnr_tun_1_cfg    tnr_1_cfg;
	struct cvi_isp_tnr_tun_2_cfg    tnr_2_cfg;
	struct cvi_isp_tnr_tun_3_cfg    tnr_3_cfg;
	struct cvi_isp_tnr_tun_4_cfg    tnr_4_cfg;
	struct cvi_isp_tnr_tun_5_cfg    tnr_5_cfg;
	struct cvi_isp_tnr_tun_6_cfg    tnr_6_cfg;
	struct cvi_isp_tnr_tun_7_cfg    tnr_7_cfg;
};

struct cvi_isp_ee_tun_1_cfg {
	union REG_ISP_EE_A4                     REG_A4;
	union REG_ISP_EE_A8                     REG_A8;
	union REG_ISP_EE_AC                     REG_AC;
	union REG_ISP_EE_B0                     REG_B0;
	union REG_ISP_EE_B4                     REG_B4;
	union REG_ISP_EE_B8                     REG_B8;
	union REG_ISP_EE_BC                     REG_BC;
	union REG_ISP_EE_C0                     REG_C0;
	union REG_ISP_EE_C4                     REG_C4;
	union REG_ISP_EE_C8                     REG_C8;
	union REG_ISP_EE_HCC                    REG_HCC;
	union REG_ISP_EE_HD0                    REG_HD0;
};

struct cvi_isp_ee_tun_2_cfg {
	union REG_ISP_EE_19C                    REG_19C;
	union REG_ISP_EE_1A0                    REG_1A0;
	union REG_ISP_EE_1A4                    REG_1A4;
	union REG_ISP_EE_1A8                    REG_1A8;
};

struct cvi_isp_ee_tun_3_cfg {
	union REG_ISP_EE_1C4                    REG_1C4;
	union REG_ISP_EE_1C8                    REG_1C8;
	union REG_ISP_EE_1CC                    REG_1CC;
	union REG_ISP_EE_1D0                    REG_1D0;
	union REG_ISP_EE_1D4                    REG_1D4;
	union REG_ISP_EE_1D8                    REG_1D8;
};

struct cvi_vip_isp_ee_config {
	__u8  update;
	__u8  enable;
	__u8  dbg_mode;
	__u8  total_coring;
	__u8  total_motion_coring;
	__u8  total_gain;
	__u8  total_oshtthrd;
	__u8  total_ushtthrd;
	__u8  pre_proc_enable;
	__u8  lumaref_lpf_en;
	__u8  luma_coring_en;
	__u8  luma_adptctrl_en;
	__u8  delta_adptctrl_en;
	__u8  delta_adptctrl_shift;
	__u8  chromaref_lpf_en;
	__u8  chroma_adptctrl_en;
	__u8  mf_core_gain;
	__u8  hf_blend_wgt;
	__u8  mf_blend_wgt;
	__u8  soft_clamp_enable;
	__u8  upper_bound_left_diff;
	__u8  lower_bound_right_diff;
	__u8  luma_adptctrl_lut[33];
	__u8  delta_adptctrl_lut[33];
	__u8  chroma_adptctrl_lut[33];
	struct cvi_isp_ee_tun_1_cfg ee_1_cfg;
	struct cvi_isp_ee_tun_2_cfg ee_2_cfg;
	struct cvi_isp_ee_tun_3_cfg ee_3_cfg;
};

struct cvi_vip_isp_pre_ee_config {
	__u8  update;
	__u8  enable;
	__u8  dbg_mode;
	__u8  total_coring;
	__u8  total_motion_coring;
	__u8  total_gain;
	__u8  total_oshtthrd;
	__u8  total_ushtthrd;
	__u8  pre_proc_enable;
	__u8  lumaref_lpf_en;
	__u8  luma_coring_en;
	__u8  luma_adptctrl_en;
	__u8  delta_adptctrl_en;
	__u8  delta_adptctrl_shift;
	__u8  chromaref_lpf_en;
	__u8  chroma_adptctrl_en;
	__u8  mf_core_gain;
	__u8  hf_blend_wgt;
	__u8  mf_blend_wgt;
	__u8  soft_clamp_enable;
	__u8  upper_bound_left_diff;
	__u8  lower_bound_right_diff;
	__u8  luma_adptctrl_lut[33];
	__u8  delta_adptctrl_lut[33];
	__u8  chroma_adptctrl_lut[33];
	struct cvi_isp_ee_tun_1_cfg pre_ee_1_cfg;
	struct cvi_isp_ee_tun_2_cfg pre_ee_2_cfg;
	struct cvi_isp_ee_tun_3_cfg pre_ee_3_cfg;
};

struct cvi_isp_fswdr_tun_cfg {
	union REG_FUSION_FS_SE_GAIN             FS_SE_GAIN;
	union REG_FUSION_FS_LUMA_THD            FS_LUMA_THD;
	union REG_FUSION_FS_WGT                 FS_WGT;
	union REG_FUSION_FS_WGT_SLOPE           FS_WGT_SLOPE;
};

struct cvi_isp_fswdr_tun_2_cfg {
	union REG_FUSION_FS_MOTION_LUT_IN       FS_MOTION_LUT_IN;
	union REG_FUSION_FS_MOTION_LUT_OUT_0    FS_MOTION_LUT_OUT_0;
	union REG_FUSION_FS_MOTION_LUT_OUT_1    FS_MOTION_LUT_OUT_1;
	union REG_FUSION_FS_MOTION_LUT_SLOPE_0  FS_MOTION_LUT_SLOPE_0;
	union REG_FUSION_FS_MOTION_LUT_SLOPE_1  FS_MOTION_LUT_SLOPE_1;
};

struct cvi_isp_fswdr_tun_3_cfg {
	union REG_FUSION_FS_CALIB_CTRL_0        FS_CALIB_CTRL_0;
	union REG_FUSION_FS_CALIB_CTRL_1        FS_CALIB_CTRL_1;
	union REG_FUSION_FS_SE_FIX_OFFSET_0     FS_SE_FIX_OFFSET_0;
	union REG_FUSION_FS_SE_FIX_OFFSET_1     FS_SE_FIX_OFFSET_1;
	union REG_FUSION_FS_SE_FIX_OFFSET_2     FS_SE_FIX_OFFSET_2;
	union REG_FUSION_FS_CALIB_OUT_0         FS_CALIB_OUT_0;
	union REG_FUSION_FS_CALIB_OUT_1         FS_CALIB_OUT_1;
	union REG_FUSION_FS_CALIB_OUT_2         FS_CALIB_OUT_2;
	union REG_FUSION_FS_CALIB_OUT_3         FS_CALIB_OUT_3;
	union REG_FUSION_FS_LMAP_DARK_THD       FS_LMAP_DARK_THD;
	union REG_FUSION_FS_LMAP_DARK_WGT       FS_LMAP_DARK_WGT;
	union REG_FUSION_FS_LMAP_DARK_WGT_SLOPE  FS_LMAP_DARK_WGT_SLOPE;
	union REG_FUSION_FS_LMAP_BRIT_THD       FS_LMAP_BRIT_THD;
	union REG_FUSION_FS_LMAP_BRIT_WGT       FS_LMAP_BRIT_WGT;
	union REG_FUSION_FS_LMAP_BRIT_WGT_SLOPE  FS_LMAP_BRIT_WGT_SLOPE;
};

struct cvi_vip_isp_fswdr_config {
	__u8  update;
	__u8  enable;
	__u8  mc_enable;
	__u8  dc_mode;
	__u8  luma_mode;
	__u8  lmap_guide_dc_mode;
	__u8  lmap_guide_luma_mode;
	__u32 s_max;
	__u8  fusion_type;
	__u16 fusion_lwgt;
	__u8  motion_ls_mode;
	__u8  mmap_mrg_mode;
	__u8  mmap_1_enable;
	__u8  motion_ls_sel;
	__u8  mmap_mrg_alph;
	__u8  history_sel_2;
	__u16 mmap_v_thd_l;
	__u16 mmap_v_thd_h;
	__u16 mmap_v_wgt_min;
	__u16 mmap_v_wgt_max;
	__s32 mmap_v_wgt_slp;
	__u8  le_in_sel;
	__u8  se_in_sel;
	struct cvi_isp_fswdr_tun_cfg fswdr_cfg;
	struct cvi_isp_fswdr_tun_2_cfg fswdr_2_cfg;
	struct cvi_isp_fswdr_tun_3_cfg fswdr_3_cfg;
};

struct cvi_vip_isp_fswdr_report {
	uint32_t cal_pix_num;
	int32_t diff_sum_r;
	int32_t diff_sum_g;
	int32_t diff_sum_b;
};

struct cvi_vip_isp_ldci_tun_1_cfg {
	union REG_ISP_LDCI_LUMA_WGT_MAX         LDCI_LUMA_WGT_MAX;
	union REG_ISP_LDCI_IDX_IIR_ALPHA        LDCI_IDX_IIR_ALPHA;
	union REG_ISP_LDCI_EDGE_SCALE           LDCI_EDGE_SCALE;
	union REG_ISP_LDCI_EDGE_CLAMP           LDCI_EDGE_CLAMP;
	union REG_ISP_LDCI_IDX_FILTER_NORM      LDCI_IDX_FILTER_NORM;
	union REG_ISP_LDCI_TONE_CURVE_IDX_00    LDCI_TONE_CURVE_IDX_00;
};

struct cvi_vip_isp_ldci_tun_2_cfg {
	union REG_ISP_LDCI_BLK_SIZE_X           LDCI_BLK_SIZE_X;
	union REG_ISP_LDCI_BLK_SIZE_X1          LDCI_BLK_SIZE_X1;
	union REG_ISP_LDCI_SUBBLK_SIZE_X        LDCI_SUBBLK_SIZE_X;
	union REG_ISP_LDCI_SUBBLK_SIZE_X1       LDCI_SUBBLK_SIZE_X1;
	union REG_ISP_LDCI_INTERP_NORM_LR       LDCI_INTERP_NORM_LR;
	union REG_ISP_LDCI_SUB_INTERP_NORM_LR   LDCI_SUB_INTERP_NORM_LR;
	union REG_ISP_LDCI_MEAN_NORM_X          LDCI_MEAN_NORM_X;
	union REG_ISP_LDCI_VAR_NORM_Y           LDCI_VAR_NORM_Y;
	union REG_ISP_LDCI_UV_GAIN_MAX          LDCI_UV_GAIN_MAX;
};

struct cvi_vip_isp_ldci_tun_3_cfg {
	union REG_ISP_LDCI_IDX_FILTER_LUT_00    LDCI_IDX_FILTER_LUT_00;
	union REG_ISP_LDCI_IDX_FILTER_LUT_02    LDCI_IDX_FILTER_LUT_02;
	union REG_ISP_LDCI_IDX_FILTER_LUT_04    LDCI_IDX_FILTER_LUT_04;
	union REG_ISP_LDCI_IDX_FILTER_LUT_06    LDCI_IDX_FILTER_LUT_06;
	union REG_ISP_LDCI_IDX_FILTER_LUT_08    LDCI_IDX_FILTER_LUT_08;
	union REG_ISP_LDCI_IDX_FILTER_LUT_10    LDCI_IDX_FILTER_LUT_10;
	union REG_ISP_LDCI_IDX_FILTER_LUT_12    LDCI_IDX_FILTER_LUT_12;
	union REG_ISP_LDCI_IDX_FILTER_LUT_14    LDCI_IDX_FILTER_LUT_14;
	union REG_ISP_LDCI_INTERP_NORM_LR1      LDCI_INTERP_NORM_LR1;
	union REG_ISP_LDCI_SUB_INTERP_NORM_LR1  LDCI_SUB_INTERP_NORM_LR1;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_00  LDCI_TONE_CURVE_LUT_00_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_02  LDCI_TONE_CURVE_LUT_00_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_04  LDCI_TONE_CURVE_LUT_00_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_06  LDCI_TONE_CURVE_LUT_00_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_08  LDCI_TONE_CURVE_LUT_00_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_10  LDCI_TONE_CURVE_LUT_00_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_12  LDCI_TONE_CURVE_LUT_00_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_00_14  LDCI_TONE_CURVE_LUT_00_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_00  LDCI_TONE_CURVE_LUT_01_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_02  LDCI_TONE_CURVE_LUT_01_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_04  LDCI_TONE_CURVE_LUT_01_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_06  LDCI_TONE_CURVE_LUT_01_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_08  LDCI_TONE_CURVE_LUT_01_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_10  LDCI_TONE_CURVE_LUT_01_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_12  LDCI_TONE_CURVE_LUT_01_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_01_14  LDCI_TONE_CURVE_LUT_01_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_00  LDCI_TONE_CURVE_LUT_02_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_02  LDCI_TONE_CURVE_LUT_02_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_04  LDCI_TONE_CURVE_LUT_02_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_06  LDCI_TONE_CURVE_LUT_02_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_08  LDCI_TONE_CURVE_LUT_02_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_10  LDCI_TONE_CURVE_LUT_02_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_12  LDCI_TONE_CURVE_LUT_02_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_02_14  LDCI_TONE_CURVE_LUT_02_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_00  LDCI_TONE_CURVE_LUT_03_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_02  LDCI_TONE_CURVE_LUT_03_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_04  LDCI_TONE_CURVE_LUT_03_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_06  LDCI_TONE_CURVE_LUT_03_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_08  LDCI_TONE_CURVE_LUT_03_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_10  LDCI_TONE_CURVE_LUT_03_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_12  LDCI_TONE_CURVE_LUT_03_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_03_14  LDCI_TONE_CURVE_LUT_03_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_00  LDCI_TONE_CURVE_LUT_04_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_02  LDCI_TONE_CURVE_LUT_04_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_04  LDCI_TONE_CURVE_LUT_04_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_06  LDCI_TONE_CURVE_LUT_04_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_08  LDCI_TONE_CURVE_LUT_04_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_10  LDCI_TONE_CURVE_LUT_04_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_12  LDCI_TONE_CURVE_LUT_04_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_04_14  LDCI_TONE_CURVE_LUT_04_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_00  LDCI_TONE_CURVE_LUT_05_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_02  LDCI_TONE_CURVE_LUT_05_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_04  LDCI_TONE_CURVE_LUT_05_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_06  LDCI_TONE_CURVE_LUT_05_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_08  LDCI_TONE_CURVE_LUT_05_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_10  LDCI_TONE_CURVE_LUT_05_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_12  LDCI_TONE_CURVE_LUT_05_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_05_14  LDCI_TONE_CURVE_LUT_05_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_00  LDCI_TONE_CURVE_LUT_06_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_02  LDCI_TONE_CURVE_LUT_06_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_04  LDCI_TONE_CURVE_LUT_06_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_06  LDCI_TONE_CURVE_LUT_06_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_08  LDCI_TONE_CURVE_LUT_06_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_10  LDCI_TONE_CURVE_LUT_06_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_12  LDCI_TONE_CURVE_LUT_06_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_06_14  LDCI_TONE_CURVE_LUT_06_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_00  LDCI_TONE_CURVE_LUT_07_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_02  LDCI_TONE_CURVE_LUT_07_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_04  LDCI_TONE_CURVE_LUT_07_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_06  LDCI_TONE_CURVE_LUT_07_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_08  LDCI_TONE_CURVE_LUT_07_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_10  LDCI_TONE_CURVE_LUT_07_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_12  LDCI_TONE_CURVE_LUT_07_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_07_14  LDCI_TONE_CURVE_LUT_07_14;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_00  LDCI_TONE_CURVE_LUT_P_00;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_02  LDCI_TONE_CURVE_LUT_P_02;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_04  LDCI_TONE_CURVE_LUT_P_04;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_06  LDCI_TONE_CURVE_LUT_P_06;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_08  LDCI_TONE_CURVE_LUT_P_08;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_10  LDCI_TONE_CURVE_LUT_P_10;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_12  LDCI_TONE_CURVE_LUT_P_12;
	union REG_ISP_LDCI_TONE_CURVE_LUT_P_14  LDCI_TONE_CURVE_LUT_P_14;
};

struct cvi_vip_isp_ldci_tun_4_cfg {
	union REG_ISP_LDCI_LUMA_WGT_LUT_00      LDCI_LUMA_WGT_LUT_00;
	union REG_ISP_LDCI_LUMA_WGT_LUT_04      LDCI_LUMA_WGT_LUT_04;
	union REG_ISP_LDCI_LUMA_WGT_LUT_08      LDCI_LUMA_WGT_LUT_08;
	union REG_ISP_LDCI_LUMA_WGT_LUT_12      LDCI_LUMA_WGT_LUT_12;
	union REG_ISP_LDCI_LUMA_WGT_LUT_16      LDCI_LUMA_WGT_LUT_16;
	union REG_ISP_LDCI_LUMA_WGT_LUT_20      LDCI_LUMA_WGT_LUT_20;
	union REG_ISP_LDCI_LUMA_WGT_LUT_24      LDCI_LUMA_WGT_LUT_24;
	union REG_ISP_LDCI_LUMA_WGT_LUT_28      LDCI_LUMA_WGT_LUT_28;
	union REG_ISP_LDCI_LUMA_WGT_LUT_32      LDCI_LUMA_WGT_LUT_32;
};

struct cvi_vip_isp_ldci_tun_5_cfg {
	union REG_ISP_LDCI_VAR_FILTER_LUT_00    LDCI_VAR_FILTER_LUT_00;
	union REG_ISP_LDCI_VAR_FILTER_LUT_02    LDCI_VAR_FILTER_LUT_02;
	union REG_ISP_LDCI_VAR_FILTER_LUT_04    LDCI_VAR_FILTER_LUT_04;
};

struct cvi_vip_isp_ldci_config {
	__u8  update;
	__u8  enable;
	__u8  stats_enable;
	__u8  map_enable;
	__u8  uv_gain_enable;
	__u8  first_frame_enable;
	__u8  image_size_div_by_16x12;
	__u16 strength;
	struct cvi_vip_isp_ldci_tun_1_cfg ldci_1_cfg;
	struct cvi_vip_isp_ldci_tun_2_cfg ldci_2_cfg;
	struct cvi_vip_isp_ldci_tun_3_cfg ldci_3_cfg;
	struct cvi_vip_isp_ldci_tun_4_cfg ldci_4_cfg;
	struct cvi_vip_isp_ldci_tun_5_cfg ldci_5_cfg;
};

struct cvi_vip_isp_ycur_config {
	__u8  update;
	__u8  enable;
	__u8  lut[64];
	__u16 lut_256;
};

struct cvi_vip_isp_dci_config {
	__u8  update;
	__u8  enable;
	__u8  map_enable;
	__u8  demo_mode;
	__u16 map_lut[256];
	__u8  per1sample_enable;
	__u8  hist_enable;
};

struct cvi_vip_isp_dhz_luma_tun_cfg {
	union REG_ISP_DEHAZE_9  LUMA_00;
	union REG_ISP_DEHAZE_10 LUMA_04;
	union REG_ISP_DEHAZE_11 LUMA_08;
	union REG_ISP_DEHAZE_12 LUMA_12;
};

struct cvi_vip_isp_dhz_skin_tun_cfg {
	union REG_ISP_DEHAZE_17 SKIN_00;
	union REG_ISP_DEHAZE_18 SKIN_04;
	union REG_ISP_DEHAZE_19 SKIN_08;
	union REG_ISP_DEHAZE_20 SKIN_12;
};

struct cvi_vip_isp_dhz_tmap_tun_cfg {
	union REG_ISP_DEHAZE_TMAP_00 TMAP_00;
	union REG_ISP_DEHAZE_TMAP_01 TMAP_01;
	union REG_ISP_DEHAZE_TMAP_02 TMAP_02;
	union REG_ISP_DEHAZE_TMAP_03 TMAP_03;
	union REG_ISP_DEHAZE_TMAP_04 TMAP_04;
	union REG_ISP_DEHAZE_TMAP_05 TMAP_05;
	union REG_ISP_DEHAZE_TMAP_06 TMAP_06;
	union REG_ISP_DEHAZE_TMAP_07 TMAP_07;
	union REG_ISP_DEHAZE_TMAP_08 TMAP_08;
	union REG_ISP_DEHAZE_TMAP_09 TMAP_09;
	union REG_ISP_DEHAZE_TMAP_10 TMAP_10;
	union REG_ISP_DEHAZE_TMAP_11 TMAP_11;
	union REG_ISP_DEHAZE_TMAP_12 TMAP_12;
	union REG_ISP_DEHAZE_TMAP_13 TMAP_13;
	union REG_ISP_DEHAZE_TMAP_14 TMAP_14;
	union REG_ISP_DEHAZE_TMAP_15 TMAP_15;
	union REG_ISP_DEHAZE_TMAP_16 TMAP_16;
	union REG_ISP_DEHAZE_TMAP_17 TMAP_17;
	union REG_ISP_DEHAZE_TMAP_18 TMAP_18;
	union REG_ISP_DEHAZE_TMAP_19 TMAP_19;
	union REG_ISP_DEHAZE_TMAP_20 TMAP_20;
	union REG_ISP_DEHAZE_TMAP_21 TMAP_21;
	union REG_ISP_DEHAZE_TMAP_22 TMAP_22;
	union REG_ISP_DEHAZE_TMAP_23 TMAP_23;
	union REG_ISP_DEHAZE_TMAP_24 TMAP_24;
	union REG_ISP_DEHAZE_TMAP_25 TMAP_25;
	union REG_ISP_DEHAZE_TMAP_26 TMAP_26;
	union REG_ISP_DEHAZE_TMAP_27 TMAP_27;
	union REG_ISP_DEHAZE_TMAP_28 TMAP_28;
	union REG_ISP_DEHAZE_TMAP_29 TMAP_29;
	union REG_ISP_DEHAZE_TMAP_30 TMAP_30;
	union REG_ISP_DEHAZE_TMAP_31 TMAP_31;
	union REG_ISP_DEHAZE_TMAP_32 TMAP_32;
};

/* struct cvi_vip_isp_dhz_config
 * @param strength:  (0~127) dehaze strength
 * @param th_smooth: (0~1023) threshold for edge/smooth classification.
 */
struct cvi_vip_isp_dhz_config {
	__u8  update;
	__u8  enable;
	__u8  strength;
	__u16 cum_th;
	__u16 hist_th;
	__u16 tmap_min;
	__u16 tmap_max;
	__u16 th_smooth;
	__u8  luma_lut_enable;
	__u8  skin_lut_enable;
	__u8  a_luma_wgt;
	__u8  blend_wgt;
	__u8  tmap_scale;
	__u8  d_wgt;
	__u16 sw_dc_th;
	__u16 sw_aglobal_r;
	__u16 sw_aglobal_g;
	__u16 sw_aglobal_b;
	__u16 aglobal_max;
	__u16 aglobal_min;
	__u8  skin_cb;
	__u8  skin_cr;
	struct cvi_vip_isp_dhz_luma_tun_cfg luma_cfg;
	struct cvi_vip_isp_dhz_skin_tun_cfg skin_cfg;
	struct cvi_vip_isp_dhz_tmap_tun_cfg tmap_cfg;
};

struct cvi_isp_rgbcac_tun_cfg {
	union REG_ISP_RGBCAC_PURPLE_TH          RGBCAC_PURPLE_TH;
	union REG_ISP_RGBCAC_PURPLE_CBCR        RGBCAC_PURPLE_CBCR;
	union REG_ISP_RGBCAC_PURPLE_CBCR2       RGBCAC_PURPLE_CBCR2;
	union REG_ISP_RGBCAC_PURPLE_CBCR3       RGBCAC_PURPLE_CBCR3;
	union REG_ISP_RGBCAC_GREEN_CBCR         RGBCAC_GREEN_CBCR;
	union REG_ISP_RGBCAC_EDGE_CORING        RGBCAC_EDGE_CORING;
	union REG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MIN  RGBCAC_DEPURPLE_STR_RATIO_MIN;
	union REG_ISP_RGBCAC_DEPURPLE_STR_RATIO_MAX  RGBCAC_DEPURPLE_STR_RATIO_MAX;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT0      RGBCAC_EDGE_WGT_LUT0;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT1      RGBCAC_EDGE_WGT_LUT1;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT2      RGBCAC_EDGE_WGT_LUT2;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT3      RGBCAC_EDGE_WGT_LUT3;
	union REG_ISP_RGBCAC_EDGE_WGT_LUT4      RGBCAC_EDGE_WGT_LUT4;
	union REG_ISP_RGBCAC_LUMA               RGBCAC_LUMA;
	union REG_ISP_RGBCAC_LUMA_BLEND         RGBCAC_LUMA_BLEND;
	union REG_ISP_RGBCAC_LUMA_FILTER0       RGBCAC_LUMA_FILTER0;
	union REG_ISP_RGBCAC_LUMA_FILTER1       RGBCAC_LUMA_FILTER1;
	union REG_ISP_RGBCAC_VAR_FILTER0        RGBCAC_VAR_FILTER0;
	union REG_ISP_RGBCAC_VAR_FILTER1        RGBCAC_VAR_FILTER1;
	union REG_ISP_RGBCAC_CHROMA_FILTER0     RGBCAC_CHROMA_FILTER0;
	union REG_ISP_RGBCAC_CHROMA_FILTER1     RGBCAC_CHROMA_FILTER1;
	union REG_ISP_RGBCAC_CBCR_STR           RGBCAC_CBCR_STR;
};

struct cvi_vip_isp_rgbcac_config {
	__u8  update;
	__u8  enable;
	__u8  out_sel;
	struct cvi_isp_rgbcac_tun_cfg rgbcac_cfg;
};

struct cvi_isp_cac_tun_cfg {
	union REG_ISP_CNR_PURPLE_CB             CNR_PURPLE_CB;
	union REG_ISP_CNR_GREEN_CB              CNR_GREEN_CB;
};

struct cvi_isp_cac_2_tun_cfg {
	union REG_ISP_CNR_EDGE_SCALE            CNR_EDGE_SCALE;
	union REG_ISP_CNR_EDGE_RATIO_SPEED      CNR_EDGE_RATIO_SPEED;
	union REG_ISP_CNR_DEPURPLE_WEIGHT_TH    CNR_DEPURPLE_WEIGHT_TH;
};

struct cvi_isp_cac_3_tun_cfg {
	union REG_ISP_CNR_EDGE_SCALE_LUT_0      CNR_EDGE_SCALE_LUT_0;
	union REG_ISP_CNR_EDGE_SCALE_LUT_4      CNR_EDGE_SCALE_LUT_4;
	union REG_ISP_CNR_EDGE_SCALE_LUT_8      CNR_EDGE_SCALE_LUT_8;
	union REG_ISP_CNR_EDGE_SCALE_LUT_12     CNR_EDGE_SCALE_LUT_12;
	union REG_ISP_CNR_EDGE_SCALE_LUT_16     CNR_EDGE_SCALE_LUT_16;
};

struct cvi_vip_isp_cac_config {
	__u8  update;
	__u8  enable;
	__u8  out_sel;
	__u8  purple_th;
	__u8  correct_strength;
	__u8  purple_cb2;
	__u8  purple_cr2;
	__u8  purple_cb3;
	__u8  purple_cr3;
	struct cvi_isp_cac_tun_cfg cac_cfg;
	struct cvi_isp_cac_2_tun_cfg cac_2_cfg;
	struct cvi_isp_cac_3_tun_cfg cac_3_cfg;
};

struct cvi_isp_lcac_tun_cfg {
	union REG_ISP_LCAC_REG04                REG04;
	union REG_ISP_LCAC_REG08                REG08;
	union REG_ISP_LCAC_REG0C                REG0C;
	union REG_ISP_LCAC_REG10                REG10;
	union REG_ISP_LCAC_REG14                REG14;
	union REG_ISP_LCAC_REG18                REG18;
	union REG_ISP_LCAC_REG1C                REG1C;
	union REG_ISP_LCAC_REG20                REG20;
	union REG_ISP_LCAC_REG24                REG24;
	union REG_ISP_LCAC_REG28                REG28;
	union REG_ISP_LCAC_REG2C                REG2C;
	union REG_ISP_LCAC_REG30                REG30;
	union REG_ISP_LCAC_REG34                REG34;
	union REG_ISP_LCAC_REG38                REG38;
	union REG_ISP_LCAC_REG3C                REG3C;
	union REG_ISP_LCAC_REG40                REG40;
	union REG_ISP_LCAC_REG44                REG44;
	union REG_ISP_LCAC_REG48                REG48;
	union REG_ISP_LCAC_REG4C                REG4C;
};

struct cvi_isp_lcac_2_tun_cfg {
	union REG_ISP_LCAC_REG50                REG50;
	union REG_ISP_LCAC_REG54                REG54;
	union REG_ISP_LCAC_REG58                REG58;
	union REG_ISP_LCAC_REG5C                REG5C;
	union REG_ISP_LCAC_REG60                REG60;
	union REG_ISP_LCAC_REG64                REG64;
	union REG_ISP_LCAC_REG68                REG68;
	union REG_ISP_LCAC_REG6C                REG6C;
};

struct cvi_isp_lcac_3_tun_cfg {
	union REG_ISP_LCAC_REG70                REG70;
	union REG_ISP_LCAC_REG74                REG74;
	union REG_ISP_LCAC_REG78                REG78;
	union REG_ISP_LCAC_REG7C                REG7C;
	union REG_ISP_LCAC_REG80                REG80;
	union REG_ISP_LCAC_REG84                REG84;
	union REG_ISP_LCAC_REG88                REG88;
	union REG_ISP_LCAC_REG8C                REG8C;
};

struct cvi_vip_isp_lcac_config {
	__u8  update;
	__u8  enable;
	__u8  out_sel;
	__u8  lti_luma_lut_32;
	__u8  fcf_luma_lut_32;
	struct cvi_isp_lcac_tun_cfg lcac_cfg;
	struct cvi_isp_lcac_2_tun_cfg lcac_2_cfg;
	struct cvi_isp_lcac_3_tun_cfg lcac_3_cfg;
};

struct cvi_vip_isp_csc_config {
	__u8  update;
	__u8  enable;
	__s16 coeff[9];
	__s16 offset[3];
};

struct cvi_isp_dpc_tun_cfg {
	union REG_ISP_DPC_3                     DPC_3;
	union REG_ISP_DPC_4                     DPC_4;
	union REG_ISP_DPC_5                     DPC_5;
	union REG_ISP_DPC_6                     DPC_6;
	union REG_ISP_DPC_7                     DPC_7;
	union REG_ISP_DPC_8                     DPC_8;
	union REG_ISP_DPC_9                     DPC_9;
};

struct cvi_vip_isp_dpc_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	__u8  staticbpc_enable;
	__u8  dynamicbpc_enable;
	__u8  cluster_size;
	__u32 bp_tbl[2047];
	__u16 bp_cnt;
	struct cvi_isp_dpc_tun_cfg dpc_cfg;
};

struct cvi_isp_ae_tun_cfg {
	union REG_ISP_AE_HIST_AE_FACE0_ENABLE   AE_FACE0_ENABLE;
	union REG_ISP_AE_HIST_AE_FACE0_STS_DIV  AE_FACE0_STS_DIV;
	union REG_ISP_AE_HIST_AE_FACE1_STS_DIV  AE_FACE1_STS_DIV;
	union REG_ISP_AE_HIST_AE_FACE2_STS_DIV  AE_FACE2_STS_DIV;
	union REG_ISP_AE_HIST_AE_FACE3_STS_DIV  AE_FACE3_STS_DIV;
	union REG_ISP_AE_HIST_STS_ENABLE        STS_ENABLE;
	union REG_ISP_AE_HIST_AE_ALGO_ENABLE    AE_ALGO_ENABLE;
	union REG_ISP_AE_HIST_AE_HIST_LOW       AE_HIST_LOW;
	union REG_ISP_AE_HIST_AE_HIST_HIGH      AE_HIST_HIGH;
	union REG_ISP_AE_HIST_AE_TOP            AE_TOP;
	union REG_ISP_AE_HIST_AE_BOT            AE_BOT;
	union REG_ISP_AE_HIST_AE_OVEREXP_THR    AE_OVEREXP_THR;
	union REG_ISP_AE_HIST_AE_NUM_GAPLINE    AE_NUM_GAPLINE;
};

struct cvi_isp_ae_2_tun_cfg {
	union REG_ISP_AE_HIST_AE_WGT_00         AE_WGT_00;
	union REG_ISP_AE_HIST_AE_WGT_01         AE_WGT_01;
	union REG_ISP_AE_HIST_AE_WGT_02         AE_WGT_02;
	union REG_ISP_AE_HIST_AE_WGT_03         AE_WGT_03;
	union REG_ISP_AE_HIST_AE_WGT_04         AE_WGT_04;
	union REG_ISP_AE_HIST_AE_WGT_05         AE_WGT_05;
	union REG_ISP_AE_HIST_AE_WGT_06         AE_WGT_06;
	union REG_ISP_AE_HIST_AE_WGT_07         AE_WGT_07;
	union REG_ISP_AE_HIST_AE_WGT_08         AE_WGT_08;
	union REG_ISP_AE_HIST_AE_WGT_09         AE_WGT_09;
	union REG_ISP_AE_HIST_AE_WGT_10         AE_WGT_10;
	union REG_ISP_AE_HIST_AE_WGT_11         AE_WGT_11;
	union REG_ISP_AE_HIST_AE_WGT_12         AE_WGT_12;
	union REG_ISP_AE_HIST_AE_WGT_13         AE_WGT_13;
	union REG_ISP_AE_HIST_AE_WGT_14         AE_WGT_14;
	union REG_ISP_AE_HIST_AE_WGT_15         AE_WGT_15;
	union REG_ISP_AE_HIST_AE_WGT_16         AE_WGT_16;
	union REG_ISP_AE_HIST_AE_WGT_17         AE_WGT_17;
	union REG_ISP_AE_HIST_AE_WGT_18         AE_WGT_18;
	union REG_ISP_AE_HIST_AE_WGT_19         AE_WGT_19;
	union REG_ISP_AE_HIST_AE_WGT_20         AE_WGT_20;
	union REG_ISP_AE_HIST_AE_WGT_21         AE_WGT_21;
	union REG_ISP_AE_HIST_AE_WGT_22         AE_WGT_22;
	union REG_ISP_AE_HIST_AE_WGT_23         AE_WGT_23;
	union REG_ISP_AE_HIST_AE_WGT_24         AE_WGT_24;
	union REG_ISP_AE_HIST_AE_WGT_25         AE_WGT_25;
	union REG_ISP_AE_HIST_AE_WGT_26         AE_WGT_26;
	union REG_ISP_AE_HIST_AE_WGT_27         AE_WGT_27;
	union REG_ISP_AE_HIST_AE_WGT_28         AE_WGT_28;
	union REG_ISP_AE_HIST_AE_WGT_29         AE_WGT_29;
	union REG_ISP_AE_HIST_AE_WGT_30         AE_WGT_30;
	union REG_ISP_AE_HIST_AE_WGT_31         AE_WGT_31;
};

struct cvi_vip_isp_ae_config {
	__u8  update;
	__u8  inst;
	__u8  ae_enable;
	__u16 ae_offsetx;
	__u16 ae_offsety;
	__u8  ae_numx;
	__u8  ae_numy;
	__u16 ae_width;
	__u16 ae_height;
	__u16 ae_sts_div;
	__u16 ae_face_offset_x[4];
	__u16 ae_face_offset_y[4];
	__u8  ae_face_size_minus1_x[4];
	__u8  ae_face_size_minus1_y[4];
	struct cvi_isp_ae_tun_cfg ae_cfg;
	struct cvi_isp_ae_2_tun_cfg ae_2_cfg;
};

struct cvi_isp_ge_tun_cfg {
	union REG_ISP_DPC_10                    DPC_10;
	union REG_ISP_DPC_11                    DPC_11;
	union REG_ISP_DPC_12                    DPC_12;
	union REG_ISP_DPC_13                    DPC_13;
	union REG_ISP_DPC_14                    DPC_14;
	union REG_ISP_DPC_15                    DPC_15;
	union REG_ISP_DPC_16                    DPC_16;
};

struct cvi_vip_isp_ge_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	struct cvi_isp_ge_tun_cfg ge_cfg;
};

struct cvi_vip_isp_af_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	__u8  dpc_enable;
	__u8  hlc_enable;
	__u8  square_enable;
	__u8  outshift;
	__u8  num_gapline;
	__u16 offsetx;
	__u16 offsety;
	__u16 block_width;
	__u16 block_height;
	__u8  block_numx;
	__u8  block_numy;
	__u8  h_low_pass_value_shift;
	__u32 h_corning_offset_0;
	__u32 h_corning_offset_1;
	__u16 v_corning_offset;
	__u16 high_luma_threshold;
	__u8  h_low_pass_coef[5];
	__u8  h_high_pass_coef_0[5];
	__u8  h_high_pass_coef_1[5];
	__u8  v_high_pass_coef[3];
	__u8  th_low;
	__u8  th_high;
	__u8  gain_low;
	__u8  gain_high;
	__u8  slop_low;
	__u8  slop_high;
};

struct cvi_vip_isp_hist_v_config {
	__u8  update;
	__u8  enable;
	__u8  luma_mode;
	__u16 offset_x;
	__u16 offset_y;
};

struct cvi_vip_isp_gms_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	__u16 offset_x;
	__u16 offset_y;
	__u16 x_section_size;
	__u16 y_section_size;
	__u8  x_gap;
	__u8  y_gap;
};

struct cvi_vip_isp_mono_config {
	__u8  update;
	__u8  force_mono_enable;
};

#if 0
struct cvi_vip_isp_3dlut_config {
	__u8  update;
	__u8  enable;
	__u8  h_clamp_wrap_opt;
	__u16 h_lut[3276];
	__u16 s_lut[3276];
	__u16 v_lut[3276];
};

struct cvi_vip_isp_lscr_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	__u16 strength;
	__u16 strength_ir;
	__u16 norm;
	__u16 norm_ir;
	__u16 centerx;
	__u16 centery;
	__u16 gain_lut[32];
	__u16 gain_lut1[32];
	__u16 gain_lut2[32];
	__u16 gain_lut_ir[32];
};

struct cvi_vip_isp_awb_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	__u8  bayer_start;
	__u16 awb_offsetx;
	__u16 awb_offsety;
	__u16 awb_sub_win_w;
	__u16 awb_sub_win_h;
	__u8  awb_numx;
	__u8  awb_numy;
	__u8  corner_avg_en;
	__u8  corner_size;
	__u8  awb_sts_div;
	__u16 r_lower_bound;
	__u16 r_upper_bound;
	__u16 g_lower_bound;
	__u16 g_upper_bound;
	__u16 b_lower_bound;
	__u16 b_upper_bound;
};

struct cvi_vip_isp_hsv_config {
	__u8  update;
	__u8  enable;
	__u8  htune_enable;
	__u8  stune_enable;
	__u8  hsgain_enable;
	__u8  hvgain_enable;
	__u16 h_lut[769];
	__u16 s_lut[513];
	__u16 sgain_lut[769];
	__u16 vgain_lut[769];
};

struct cvi_vip_isp_preproc_config {
	__u8  update;
	__u8  inst;
	__u8  enable;
	__s16 r_ir_ratio[128];
	__s16 g_ir_ratio[128];
	__s16 b_ir_ratio[128];
	__u8  w_lut[128];
};

#endif

struct cvi_vip_isp_fe_tun_cfg {
	struct cvi_vip_isp_blc_config	blc_cfg[2];
	struct cvi_vip_isp_wbg_config	wbg_cfg[2];
};

struct cvi_vip_isp_be_tun_cfg {
	struct cvi_vip_isp_blc_config		blc_cfg[2];
	struct cvi_vip_isp_dpc_config		dpc_cfg[2];
	struct cvi_vip_isp_ge_config		ge_cfg[2];
	struct cvi_vip_isp_af_config		af_cfg;
};

struct cvi_vip_isp_post_tun_cfg {
	struct cvi_vip_isp_bnr_config		bnr_cfg;
	struct cvi_vip_isp_lsc_config		lsc_cfg;
	struct cvi_vip_isp_ae_config		ae_cfg[2];
	struct cvi_vip_isp_rgbcac_config	rgbcac_cfg;
	struct cvi_vip_isp_lcac_config		lcac_cfg;
	struct cvi_vip_isp_wbg_config		wbg_cfg[2];
	struct cvi_vip_isp_ccm_config		ccm_cfg[2];
	struct cvi_vip_isp_ygamma_config	ygamma_cfg;
	struct cvi_vip_isp_gamma_config		gamma_cfg;
	struct cvi_vip_isp_dhz_config		dhz_cfg;
	struct cvi_vip_isp_csc_config		csc_cfg;
	struct cvi_vip_isp_dci_config		dci_cfg;
	struct cvi_vip_isp_ldci_config		ldci_cfg;
	struct cvi_vip_isp_pre_ee_config	pre_ee_cfg;
	struct cvi_vip_isp_tnr_config		tnr_cfg;
	struct cvi_vip_isp_cnr_config		cnr_cfg;
	struct cvi_vip_isp_cac_config		cac_cfg;
	struct cvi_vip_isp_ynr_config		ynr_cfg;
	struct cvi_vip_isp_ee_config		ee_cfg;
	struct cvi_vip_isp_cacp_config		cacp_cfg;
	struct cvi_vip_isp_ca2_config		ca2_cfg;
	struct cvi_vip_isp_ycur_config		ycur_cfg;
	struct cvi_vip_isp_demosiac_config	demosiac_cfg;
	struct cvi_vip_isp_clut_config		clut_cfg;
	struct cvi_vip_isp_drc_config		drc_cfg;
	struct cvi_vip_isp_fswdr_config		fswdr_cfg;
	struct cvi_vip_isp_hist_v_config	hist_v_cfg;
	struct cvi_vip_isp_gms_config		gms_cfg;
	struct cvi_vip_isp_mono_config		mono_cfg;
};

struct cvi_vip_isp_fe_cfg {
	uint8_t tun_update[TUNING_NODE_NUM];
	uint8_t tun_idx;
	struct cvi_vip_isp_fe_tun_cfg tun_cfg[TUNING_NODE_NUM];
};

struct cvi_vip_isp_be_cfg {
	uint8_t tun_update[TUNING_NODE_NUM];
	uint8_t tun_idx;
	struct cvi_vip_isp_be_tun_cfg tun_cfg[TUNING_NODE_NUM];
};

struct cvi_vip_isp_post_cfg {
	uint8_t tun_update[TUNING_NODE_NUM];
	uint8_t tun_idx;
	struct cvi_vip_isp_post_tun_cfg tun_cfg[TUNING_NODE_NUM];
};

struct isp_tuning_cfg {
	uint64_t  fe_addr[ISP_PRERAW_VIRT_MAX];
	void	  *fe_vir[ISP_PRERAW_VIRT_MAX];
#ifdef __arm__
	__u32	  fe_padding[ISP_PRERAW_VIRT_MAX];
#endif
	uint64_t  be_addr[ISP_PRERAW_VIRT_MAX];
	void	  *be_vir[ISP_PRERAW_VIRT_MAX];
#ifdef __arm__
	__u32	  be_padding[ISP_PRERAW_VIRT_MAX];
#endif
	uint64_t  post_addr[ISP_PRERAW_VIRT_MAX];
	void      *post_vir[ISP_PRERAW_VIRT_MAX];
#ifdef __arm__
	__u32	  post_padding[ISP_PRERAW_VIRT_MAX];
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* _U_CVI_VIP_TUN_CFG_H_ */
