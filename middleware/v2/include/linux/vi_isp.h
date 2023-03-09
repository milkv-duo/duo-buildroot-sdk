#ifndef __U_VI_ISP_H__
#define __U_VI_ISP_H__

#include <linux/vi_snsr.h>
#include <linux/cvi_defines.h>

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
	IP_INFO_ID_PRE_RAW_FE0 = 0,
	IP_INFO_ID_CSIBDG0,
	IP_INFO_ID_DMA_CTL6,
	IP_INFO_ID_DMA_CTL7,
#if !defined(__CV180X__)
	IP_INFO_ID_DMA_CTL8,
	IP_INFO_ID_DMA_CTL9,
#endif
	IP_INFO_ID_BLC0,
	IP_INFO_ID_BLC1,
	IP_INFO_ID_RGBMAP0,
	IP_INFO_ID_WBG2,
	IP_INFO_ID_DMA_CTL10,
#if !defined(__CV180X__)
	IP_INFO_ID_RGBMAP1,
	IP_INFO_ID_WBG3,
	IP_INFO_ID_DMA_CTL11,

	IP_INFO_ID_PRE_RAW_FE1,
	IP_INFO_ID_CSIBDG1,
	IP_INFO_ID_DMA_CTL12,
	IP_INFO_ID_DMA_CTL13,
	IP_INFO_ID_DMA_CTL14,
	IP_INFO_ID_DMA_CTL15,
	IP_INFO_ID_BLC2,
	IP_INFO_ID_BLC3,
	IP_INFO_ID_RGBMAP2,
	IP_INFO_ID_WBG4,
	IP_INFO_ID_DMA_CTL16,
	IP_INFO_ID_RGBMAP3,
	IP_INFO_ID_WBG5,
	IP_INFO_ID_DMA_CTL17,

	IP_INFO_ID_PRE_RAW_FE2,
	IP_INFO_ID_CSIBDG2,
	IP_INFO_ID_DMA_CTL18,
	IP_INFO_ID_DMA_CTL19,
	IP_INFO_ID_BLC4,
	IP_INFO_ID_RGBMAP4,
	IP_INFO_ID_WBG6,
	IP_INFO_ID_DMA_CTL20,
#endif
	IP_INFO_ID_PRE_RAW_BE,
	IP_INFO_ID_CROP0,
	IP_INFO_ID_CROP1,
	IP_INFO_ID_BLC5,
#if !defined(__CV180X__)
	IP_INFO_ID_BLC6,
#endif
	IP_INFO_ID_AF,
	IP_INFO_ID_DMA_CTL21,
	IP_INFO_ID_DPC0,
#if !defined(__CV180X__)
	IP_INFO_ID_DPC1,
#endif
	IP_INFO_ID_DMA_CTL22,
#if !defined(__CV180X__)
	IP_INFO_ID_DMA_CTL23,
#endif
	IP_INFO_ID_PRE_WDMA,
	// IP_INFO_ID_PCHK0,
	// IP_INFO_ID_PCHK1,

	IP_INFO_ID_RAWTOP,
	IP_INFO_ID_CFA,
	IP_INFO_ID_LSC,
	IP_INFO_ID_DMA_CTL24,
	IP_INFO_ID_GMS,
	IP_INFO_ID_DMA_CTL25,
	IP_INFO_ID_AEHIST0,
	IP_INFO_ID_DMA_CTL26,
#if !defined(__CV180X__)
	IP_INFO_ID_AEHIST1,
	IP_INFO_ID_DMA_CTL27,
#endif
	IP_INFO_ID_DMA_CTL28,
#if !defined(__CV180X__)
	IP_INFO_ID_DMA_CTL29,
#endif
	IP_INFO_ID_RAW_RDMA,
	IP_INFO_ID_BNR,
	IP_INFO_ID_CROP2,
	IP_INFO_ID_CROP3,
	IP_INFO_ID_LMAP0,
	IP_INFO_ID_DMA_CTL30,
#if !defined(__CV180X__)
	IP_INFO_ID_LMAP1,
	IP_INFO_ID_DMA_CTL31,
#endif
	IP_INFO_ID_WBG0,
#if !defined(__CV180X__)
	IP_INFO_ID_WBG1,
#endif
	// IP_INFO_ID_PCHK2,
	// IP_INFO_ID_PCHK3,
	IP_INFO_ID_LCAC,
	IP_INFO_ID_RGBCAC,

	IP_INFO_ID_RGBTOP,
	IP_INFO_ID_CCM0,
#if !defined(__CV180X__)
	IP_INFO_ID_CCM1,
#endif
	IP_INFO_ID_RGBGAMMA,
	IP_INFO_ID_YGAMMA,
	IP_INFO_ID_MMAP,
	IP_INFO_ID_DMA_CTL32,
#if !defined(__CV180X__)
	IP_INFO_ID_DMA_CTL33,
#endif
	IP_INFO_ID_DMA_CTL34,
#if !defined(__CV180X__)
	IP_INFO_ID_DMA_CTL35,
#endif
	IP_INFO_ID_DMA_CTL36,
#if !defined(__CV180X__)
	IP_INFO_ID_DMA_CTL37,
#endif
	IP_INFO_ID_CLUT,
	IP_INFO_ID_DHZ,
	IP_INFO_ID_CSC,
	IP_INFO_ID_RGBDITHER,
	// IP_INFO_ID_PCHK4,
	// IP_INFO_ID_PCHK5,
	IP_INFO_ID_HIST_V,
	IP_INFO_ID_DMA_CTL38,
	IP_INFO_ID_HDRFUSION,
	IP_INFO_ID_HDRLTM,
	IP_INFO_ID_DMA_CTL39,
#if !defined(__CV180X__)
	IP_INFO_ID_DMA_CTL40,
#endif

	IP_INFO_ID_YUVTOP,
	IP_INFO_ID_TNR,
	IP_INFO_ID_DMA_CTL41,
	IP_INFO_ID_DMA_CTL42,
	IP_INFO_ID_FBCE,
	IP_INFO_ID_DMA_CTL43,
	IP_INFO_ID_DMA_CTL44,
	IP_INFO_ID_FBCD,
	IP_INFO_ID_YUVDITHER,
	IP_INFO_ID_CA,
	IP_INFO_ID_CA_LITE,
	IP_INFO_ID_YNR,
	IP_INFO_ID_CNR,
	IP_INFO_ID_EE,
	IP_INFO_ID_YCURVE,
	IP_INFO_ID_DCI,
	IP_INFO_ID_DMA_CTL45,
	// IP_INFO_ID_DCI_GAMMA,
	IP_INFO_ID_CROP4,
	IP_INFO_ID_DMA_CTL46,
	IP_INFO_ID_CROP5,
	IP_INFO_ID_DMA_CTL47,
	IP_INFO_ID_LDCI,
	IP_INFO_ID_DMA_CTL48,
	IP_INFO_ID_DMA_CTL49,
	IP_INFO_ID_PRE_EE,
	// IP_INFO_ID_PCHK6,
	// IP_INFO_ID_PCHK7,

	IP_INFO_ID_ISPTOP,
	IP_INFO_ID_WDMA_CORE0,
	IP_INFO_ID_RDMA_CORE,
	IP_INFO_ID_CSIBDG_LITE,
	IP_INFO_ID_DMA_CTL0,
	IP_INFO_ID_DMA_CTL1,
	IP_INFO_ID_DMA_CTL2,
	IP_INFO_ID_DMA_CTL3,
	IP_INFO_ID_WDMA_CORE1,
	IP_INFO_ID_PRE_RAW_VI_SEL,
	IP_INFO_ID_DMA_CTL4,
#if !defined(__CV180X__)
	IP_INFO_ID_DMA_CTL5,
#endif
	// IP_INFO_ID_CMDQ,

	IP_INFO_ID_MAX,
};

struct ip_info {
	__u32 str_addr; //IP start address
	__u32 size; //IP total registers size
};

/* struct cvi_vip_memblock
 * @base: the address of the memory allocated.
 * @size: Size in bytes of the memblock.
 */
struct cvi_vip_memblock {
	__u8  raw_num;
	__u64 phy_addr;
	void *vir_addr;
#ifdef __arm__
	__u32 padding;
#endif
	__u32 size;
};

struct cvi_vip_isp_raw_blk {
	struct cvi_vip_memblock raw_dump;
	__u32 frm_num;
	__u32 time_out;//msec
	__u16 src_w;
	__u16 src_h;
	__u16 crop_x;
	__u16 crop_y;
	__u8  is_b_not_rls;
	__u8  is_timeout;
	__u8  is_sig_int;
};

struct cvi_vip_isp_smooth_raw_param {
	struct cvi_vip_isp_raw_blk *raw_blk;
	__u8 raw_num;
	__u8 frm_num;
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
