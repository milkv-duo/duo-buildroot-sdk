#ifndef __U_VI_ISP_H__
#define __U_VI_ISP_H__

//#include <vi_snsr.h>
#include <stdbool.h>
#include <types.h>
#include <vi_uapi.h>
#include <vi_snsr.h>

enum ISP_SCENE_INFO {
	PRE_OFF_POST_OFF_SC,      //183x
	PRE_OFF_POST_ON_SC,       //183x
	FE_ON_BE_ON_POST_OFF_SC,  //182x
	FE_ON_BE_ON_POST_ON_SC,   //182x
	FE_ON_BE_OFF_POST_OFF_SC, //182x
	FE_ON_BE_OFF_POST_ON_SC,  //182x
	FE_OFF_BE_ON_POST_OFF_SC, //182x
	FE_OFF_BE_ON_POST_ON_SC,  //182x
};

enum cvi_isp_source {
	CVI_ISP_SOURCE_DEV = 0,
	CVI_ISP_SOURCE_FE,
	CVI_ISP_SOURCE_BE,
	CVI_ISP_SOURCE_MAX,
};

enum IP_INFO_GRP {
	IP_INFO_ID_MIN = 0,
	//Pre_raw_be
	IP_INFO_ID_PRE_RAW_BE = IP_INFO_ID_MIN,
	IP_INFO_ID_CROP4,
	IP_INFO_ID_CROP5,
	IP_INFO_ID_BLC4,
	IP_INFO_ID_BLC5,
	IP_INFO_ID_FPN,
	IP_INFO_ID_IR_PRE_PROC_LE,
	IP_INFO_ID_IR_PRE_PROC_SE,
	IP_INFO_ID_IR_PROC,
	IP_INFO_ID_AEHIST0,
	IP_INFO_ID_AEHIST1,
	IP_INFO_ID_AWB0,
	IP_INFO_ID_GMS,
	IP_INFO_ID_AF,
	IP_INFO_ID_WBG0,
	IP_INFO_ID_WBG1,
	IP_INFO_ID_DPC0,
	IP_INFO_ID_DPC1,
	IP_INFO_ID_INV_WBG0,
	IP_INFO_ID_INV_WBG1,
	IP_INFO_ID_LSCR4,
	IP_INFO_ID_LSCR5,
	//Pre_raw_0_fe
	IP_INFO_ID_PRE_RAW_FE0,
	IP_INFO_ID_CSIBDG0,
	IP_INFO_ID_CROP0,
	IP_INFO_ID_CROP1,
	IP_INFO_ID_BLC0,
	IP_INFO_ID_BLC1,
	IP_INFO_ID_LMP0,
	IP_INFO_ID_WBG11,
	IP_INFO_ID_LMP1,
	IP_INFO_ID_WBG12,
	IP_INFO_ID_RGBMAP0,
	IP_INFO_ID_WBG7,
	IP_INFO_ID_RGBMAP1,
	IP_INFO_ID_WBG8,
	IP_INFO_ID_LSCR0,
	IP_INFO_ID_LSCR1,
	//Pre_raw_1_fe
	IP_INFO_ID_PRE_RAW_FE1,
	IP_INFO_ID_CSIBDG1_R1,
	IP_INFO_ID_CROP2,
	IP_INFO_ID_CROP3,
	IP_INFO_ID_BLC2,
	IP_INFO_ID_BLC3,
	IP_INFO_ID_LMP2,
	IP_INFO_ID_WBG13,
	IP_INFO_ID_LMP3,
	IP_INFO_ID_WBG14,
	IP_INFO_ID_RGBMAP2,
	IP_INFO_ID_WBG9,
	IP_INFO_ID_RGBMAP3,
	IP_INFO_ID_WBG10,
	IP_INFO_ID_LSCR2,
	IP_INFO_ID_LSCR3,
	//Rawtop
	IP_INFO_ID_RAWTOP,
	IP_INFO_ID_CFA,
	IP_INFO_ID_BNR,
	IP_INFO_ID_CROP6,
	IP_INFO_ID_CROP7,
	//Rgbtop
	IP_INFO_ID_RGBTOP,
	IP_INFO_ID_LSCM0,
	IP_INFO_ID_CCM0,
	IP_INFO_ID_CCM1,
	IP_INFO_ID_CCM2,
	IP_INFO_ID_CCM3,
	IP_INFO_ID_CCM4,
	IP_INFO_ID_MANR,
	IP_INFO_ID_GAMMA,
	IP_INFO_ID_CLUT,
	IP_INFO_ID_DHZ,
	IP_INFO_ID_R2Y4,
	IP_INFO_ID_RGBDITHER,
	IP_INFO_ID_PREYEE,
	IP_INFO_ID_DCI,
	IP_INFO_ID_HIST_EDGE_V,
	IP_INFO_ID_HDRFUSION,
	IP_INFO_ID_HDRLTM,
	IP_INFO_ID_AWB2,
	//Yuvtop
	IP_INFO_ID_YUVTOP,
	IP_INFO_ID_444422,
	IP_INFO_ID_FBCE,
	IP_INFO_ID_FBCD,
	IP_INFO_ID_YUVDITHER,
	IP_INFO_ID_YNR,
	IP_INFO_ID_CNR,
	IP_INFO_ID_EE,
	IP_INFO_ID_YCURVE,
	IP_INFO_ID_CROP8,
	IP_INFO_ID_CROP9,
	IP_INFO_ID_ISPTOP,
	IP_INFO_ID_CSIBDG_LITE,
	IP_INFO_ID_MAX,
};

struct ip_info {
	__u32 str_addr; //IP start address
	__u32 size; //IP total registers size
};

#if 0
struct cvi_vip_isp_raw_blk {
	struct cvi_vip_memblock raw_dump;
	__u32 time_out;//msec
	__u16 src_w;
	__u16 src_h;
	__u16 crop_x;
	__u16 crop_y;
	__u8  is_b_not_rls;
	__u8  is_timeout;
	__u8  is_sig_int;
};

struct cvi_isp_sts_mem {
	__u8			raw_num;
	struct cvi_vip_memblock af;
	struct cvi_vip_memblock gms;
	struct cvi_vip_memblock ae_le;
	struct cvi_vip_memblock ae_se;
	struct cvi_vip_memblock awb;
	struct cvi_vip_memblock awb_post;
	struct cvi_vip_memblock dci;
	struct cvi_vip_memblock hist_edge_v;
	struct cvi_vip_memblock mmap;
};
#endif

struct cvi_isp_mbus_framefmt {
	__u32	width;
	__u32	height;
	__u32	code;
};

struct cvi_isp_rect {
	__s32	left;
	__s32	top;
	__u32	width;
	__u32	height;
};

struct cvi_isp_usr_pic_cfg {
	struct cvi_isp_mbus_framefmt fmt;
	struct cvi_isp_rect crop;
};
#if 0
struct cvi_isp_snr_info {
	__u8   raw_num;
	__u16  color_mode;
	__u32  pixel_rate;
	struct wdr_size_s snr_fmt;
};

struct cvi_isp_snr_update {
	__u8  raw_num;
	struct snsr_cfg_node_s snr_cfg_node;
};
#endif
struct cvi_vip_isp_yuv_param {
	__u8   raw_num;
	__u32  yuv_bypass_path;
};

struct cvi_isp_mmap_grid_size {
	__u8  raw_num;
	__u8  grid_size;
};

struct isp_proc_cfg {
	void *buffer;
#ifdef __arm__
	__u32 padding;
#endif
	size_t buffer_size;
};

struct cvi_vip_isp_awb_sts {
	__u8  raw_num;
	__u8  is_se;
	__u8  buf_idx;
};

struct cvi_vi_dma_buf_info {
	__u64  paddr;
	__u32  size;
};

struct cvi_isp_sc_online {
	__u8   raw_num;
	__u8   is_sc_online;
};

#endif // __U_VI_ISP_H__
