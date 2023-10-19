/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: isp_drv.h
 * Description:
 */

#ifndef _ISP_DRV_H_
#define _ISP_DRV_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "vip_common.h"
#include "uapi/cvi_vip_tun_cfg.h"

#define _OFST(_BLK_T, _REG)       ((uint64_t)&(((struct _BLK_T *)0)->_REG))

#define ISP_RD_REG_BA(_BA) \
	(_reg_read(_BA))

#define ISP_RD_REG(_BA, _BLK_T, _REG) \
	(_reg_read(_BA+_OFST(_BLK_T, _REG)))

#define ISP_RD_BITS(_BA, _BLK_T, _REG, _FLD) \
	({\
		typeof(((struct _BLK_T *)0)->_REG) _r;\
		_r.raw = _reg_read(_BA+_OFST(_BLK_T, _REG));\
		_r.bits._FLD;\
	})

#define ISP_WR_REG(_BA, _BLK_T, _REG, _V) \
	(_reg_write((_BA+_OFST(_BLK_T, _REG)), _V))

#define ISP_WR_REG_OFT(_BA, _BLK_T, _REG, _OFT, _V) \
	(_reg_write((_BA+_OFST(_BLK_T, _REG) + _OFT), _V))

#define ISP_WR_BITS(_BA, _BLK_T, _REG, _FLD, _V) \
	do {\
		typeof(((struct _BLK_T *)0)->_REG) _r;\
		_r.raw = _reg_read(_BA+_OFST(_BLK_T, _REG));\
		_r.bits._FLD = _V;\
		_reg_write((_BA+_OFST(_BLK_T, _REG)), _r.raw);\
	} while (0)

#define ISP_WO_BITS(_BA, _BLK_T, _REG, _FLD, _V) \
	do {\
		typeof(((struct _BLK_T *)0)->_REG) _r;\
		_r.raw = 0;\
		_r.bits._FLD = _V;\
		_reg_write((_BA+_OFST(_BLK_T, _REG)), _r.raw);\
	} while (0)

#define ISP_WR_REGS_BURST(_BA, _BLK_T, _REG, _SIZE, _STR)\
	do {\
		u32 k = 0;\
		u64 ofst = _OFST(_BLK_T, _REG);\
		for (; k < sizeof(_SIZE) / 0x4; k++) {\
			u32 val = (&_STR + k)->raw;\
			_reg_write((_BA + ofst + (k * 0x4)), val);\
		} \
	} while (0)

#define ISP_WR_REG_LOOP_SHFT(_BA, _BLK_T, _REG, _TOTAL_SIZE, _SFT_SIZE, _LUT, _SFT_BIT) \
	do {\
		uint16_t i = 0, j = 0;\
		uint32_t val = 0;\
		for (; i < _TOTAL_SIZE / _SFT_SIZE; i++) {\
			val = 0;\
			for (j = 0; j < _SFT_SIZE; j++) {\
				val += (_LUT[(i * _SFT_SIZE) + j] << (_SFT_BIT * j));\
			} \
			_reg_write((_BA + _OFST(_BLK_T, _REG) + (i * 0x4)), val);\
		} \
	} while (0)

enum ISP_RGB_PROB_OUT {
	ISP_RGB_PROB_OUT_CFA = 0,
	ISP_RGB_PROB_OUT_RGBEE,
	ISP_RGB_PROB_OUT_CCM,
	ISP_RGB_PROB_OUT_GMA,
	ISP_RGB_PROB_OUT_DHZ,
	ISP_RGB_PROB_OUT_HSV,
	ISP_RGB_PROB_OUT_RGBDITHER,
	ISP_RGB_PROB_OUT_CSC,
	ISP_RGB_PROB_OUT_MAX,
};

enum ISP_RAW_PATH {
	ISP_RAW_PATH_LE = 0,
	ISP_RAW_PATH_SE,
	ISP_RAW_PATH_MAX,
};

/*
 * To indicate the 1st two pixel in the bayer_raw.
 */
enum ISP_BAYER_TYPE {
	ISP_BAYER_TYPE_BG = 0,
	ISP_BAYER_TYPE_GB,
	ISP_BAYER_TYPE_GR,
	ISP_BAYER_TYPE_RG,
	ISP_BAYER_TYPE_MAX,
};

enum ISP_BNR_OUT {
	ISP_BNR_OUT_BYPASS = 0,
	ISP_BNR_OUT_B_DELAY,
	ISP_BNR_OUT_FACTOR,
	ISP_BNR_OUT_B_NL,
	ISP_BNR_OUT_RESV_0,
	ISP_BNR_OUT_RESV_1,
	ISP_BNR_OUT_RESV_2,
	ISP_BNR_OUT_RESV_3,
	ISP_BNR_OUT_B_OUT,
	ISP_BNR_OUT_INTENSITY,
	ISP_BNR_OUT_DELTA,
	ISP_BNR_OUT_NOT_SM,
	ISP_BNR_OUT_FLAG_V,
	ISP_BNR_OUT_FLAG_H,
	ISP_BNR_OUT_FLAG_D45,
	ISP_BNR_OUT_FLAG_D135,
	ISP_BNR_OUT_MAX,
};

enum ISP_YNR_OUT {
	ISP_YNR_OUT_BYPASS = 0,
	ISP_YNR_OUT_Y_DELAY,
	ISP_YNR_OUT_FACTOR,
	ISP_YNR_OUT_ALPHA,
	ISP_YNR_OUT_Y_BF,
	ISP_YNR_OUT_Y_NL,
	ISP_YNR_OUT_RESV_0,
	ISP_YNR_OUT_RESV_1,
	ISP_YNR_OUT_Y_OUT,
	ISP_YNR_OUT_INTENSITY,
	ISP_YNR_OUT_DELTA,
	ISP_YNR_OUT_NOT_SM,
	ISP_YNR_OUT_FLAG_V,
	ISP_YNR_OUT_FLAG_H,
	ISP_YNR_OUT_FLAG_D45,
	ISP_YNR_OUT_FLAG_D135,
	ISP_YNR_OUT_MAX,
};

enum ISP_FS_OUT {
	ISP_FS_OUT_FS = 0,
	ISP_FS_OUT_LONG,
	ISP_FS_OUT_SHORT,
	ISP_FS_OUT_SHORT_EX,
	ISP_FS_OUT_MAX,
};

struct isp_param {
	uint32_t img_width;
	uint32_t img_height;
	uint32_t img_plane;
	uint32_t img_format;
	uint32_t img_stride;
};

enum ISPCQ_ID_T {
	ISPCQ_ID_PRERAW0 = 0,
	ISPCQ_ID_PRERAW1,
	ISPCQ_ID_POSTRAW,
	ISPCQ_ID_MAX
};

enum ISPCQ_OP_MODE {
	ISPCQ_OP_SINGLE_CMDSET,
	ISPCQ_OP_ADMA,
};

struct ispcq_config {
	uint32_t op_mode;
	uint32_t intr_en;

	enum ISPCQ_ID_T cq_id;
	union {
		uint64_t adma_table_pa;
		uint64_t cmdset_pa;
	};
	uint32_t cmdset_size;
};

enum isp_dump_grp {
	ISP_DUMP_PRERAW = 0x1,
	ISP_DUMP_POSTRAW = 0x2,
	ISP_DUMP_ALL = 0x4,
	ISP_DUMP_DMA = 0x8,
	ISP_DUMP_ALL_DMA = 0x10,
};

struct isp_dump_info {
	uint64_t phy_base;
	uint64_t reg_base;
	uint32_t blk_size;
};

struct isp_vblock_info {
	uint32_t block_id;
	uint32_t block_size;
	uint64_t reg_base;
};

struct isp_rgbmap_info {
	u8 w_bit;
	u8 h_bit;
};

struct isp_dma_cfg {
	uint16_t width;
	uint16_t height;
	uint16_t stride;
	uint16_t format;
};

struct tile {
	u16 start;
	u16 end;
};

struct isp_dhz_cfg {
	uint8_t  strength;
	uint16_t th_smooth;
	uint16_t cum_th;
	uint16_t hist_th;
	uint16_t tmap_min;
	uint16_t tmap_max;
	uint16_t sw_dc_th;
	uint16_t sw_aglobal;
	bool sw_dc_trig;
};

struct isp_ccm_cfg {
	u16 coef[3][3];
};

struct _isp_dg_info {
	uint32_t		preraw_debug_sts;
	uint32_t		isp_top_sts;
	uint32_t		bdg_debug_sts;
	uint32_t		bdg_sts;
	uint32_t		bdg_fifo_of_cnt;
	uint8_t			bdg_w_gt_cnt;
	uint8_t			bdg_w_ls_cnt;
	uint8_t			bdg_h_gt_cnt;
	uint8_t			bdg_h_ls_cnt;
};

struct _isp_cfg {
	uint32_t		csibdg_width;
	uint32_t		csibdg_height;
	uint32_t		max_width;
	uint32_t		max_height;
	uint32_t		post_img_w;
	uint32_t		post_img_h;
	struct vip_rect		crop;
	struct vip_rect		crop_se;
	struct _isp_dg_info	dg_info;
	struct isp_rgbmap_info	rgbmap_i;

	uint32_t		is_patgen_en		: 1;
	uint32_t		is_offline_preraw	: 1;
	uint32_t		is_yuv_bypass_path	: 1;
	uint32_t		is_hdr_on		: 1;
	uint32_t		is_hdr_detail_en	: 1;
};

/*
 * @src_width: width of original image
 * @src_height: height of original image
 * @img_width: width of image after crop
 * @img_height: height of image after crop
 * @pyhs_regs: index by enum ISP_BLK_ID_T, always phys reg
 * @vreg_bases: index by enum ISP_BLK_ID_T
 * @vreg_bases_pa: index by enum ISP_BLK_ID_T
 *
 * @rgb_color_mode: bayer_raw type after crop could change
 *
 * @cam_id: preraw(0,1)
 * @is_offline_preraw: preraw src offline(from dram)
 * @is_offline_postraw: postraw src offline(from dram)
 */
struct isp_ctx {
	struct isp_param	inparm;
	uint32_t		src_width;
	uint32_t		src_height;
	uint32_t		img_width;
	uint32_t		img_height;
	uint32_t		crop_x;
	uint32_t		crop_y;
	uint32_t		crop_se_x;
	uint32_t		crop_se_y;
	struct _tile_cfg {
		struct tile l_in;
		struct tile l_out;
		struct tile r_in;
		struct tile r_out;
	} tile_cfg;


	uint64_t		*phys_regs;
	uintptr_t		*vreg_bases;
	uintptr_t		*vreg_bases_pa;
	uintptr_t		adma_table[ISPCQ_ID_MAX];
	uintptr_t		adma_table_pa[ISPCQ_ID_MAX];

	struct _isp_cfg		isp_pipe_cfg[ISP_PRERAW_MAX];
	enum ISP_BAYER_TYPE	rgb_color_mode[ISP_PRERAW_MAX];
	uint8_t			sensor_bitdepth;
	uint8_t			rgbmap_prebuf_idx;

	uint8_t			cam_id;
	uint32_t		is_dual_sensor      : 1;
	uint32_t		is_yuv_sensor       : 1;
	uint32_t		is_hdr_on           : 1;
	uint32_t		is_3dnr_on          : 1;
	uint32_t		is_dpcm_on          : 1;
	uint32_t		is_offline_postraw  : 1;
	uint32_t		is_offline_scaler   : 1;
	uint32_t		is_tile             : 1;
	uint32_t		is_work_on_r_tile   : 1;
	uint32_t		is_sublvds_path     : 1;

	uint32_t		vreg_page_idx;
};

union isp_intr {
	uint32_t raw;
	struct {
		uint32_t FRAME_DONE_PRE                  : 1;
		uint32_t FRAME_DONE_PRE1                 : 1;
		uint32_t FRAME_DONE_POST                 : 1;
		uint32_t SHAW_DONE_PRE                   : 1;
		uint32_t SHAW_DONE_PRE1                  : 1;
		uint32_t SHAW_DONE_POST                  : 1;
		uint32_t FRAME_ERR_PRE                   : 1;
		uint32_t FRAME_ERR_PRE1                  : 1;
		uint32_t FRAME_ERR_POST                  : 1;
		uint32_t CMDQ1_INT                       : 1;
		uint32_t CMDQ2_INT                       : 1;
		uint32_t CMDQ3_INT                       : 1;
		uint32_t FRAME_START_PRE                 : 1;
		uint32_t FRAME_START_PRE1                : 1;
		uint32_t _rsv_14                         : 2;
		uint32_t PCHK0_ERR_PRE                   : 1;
		uint32_t PCHK0_ERR_PRE1                  : 1;
		uint32_t PCHK0_ERR_RAW                   : 1;
		uint32_t PCHK0_ERR_RGB                   : 1;
		uint32_t PCHK0_ERR_YUV                   : 1;
		uint32_t PCHK1_ERR_PRE                   : 1;
		uint32_t PCHK1_ERR_PRE1                  : 1;
		uint32_t PCHK1_ERR_RAW                   : 1;
		uint32_t PCHK1_ERR_RGB                   : 1;
		uint32_t PCHK1_ERR_YUV                   : 1;
		uint32_t LINE_REACH_INT_PRE              : 1;
		uint32_t LINE_REACH_INT_PRE1             : 1;
	} bits;
};

union isp_csi_intr {
	uint32_t raw;
	struct {
		uint32_t FRAME_DROP_INT                  : 1;
		uint32_t FIFO_OVERFLOW_INT               : 1;
		uint32_t FRAME_WIDTH_GT_INT              : 1;
		uint32_t FRAME_WIDTH_LS_INT              : 1;
		uint32_t FRAME_HEIGHT_GT_INT             : 1;
		uint32_t FRAME_HEIGHT_LS_INT             : 1;
		uint32_t VSYNC_RISING_INT                : 1;
		uint32_t FRAME_START_INT                 : 1;
		uint32_t FRAME_WIDTH_OVER_MAX_INT        : 1;
		uint32_t FRAME_HEIGHT_OVER_MAX_INT       : 1;
		uint32_t LINE_INTP_INT                   : 1;
	} bits;
};

enum ISP_BLC_ID {
	ISP_BLC_ID_PRE0_LE = 0,
	ISP_BLC_ID_PRE0_SE,
	ISP_BLC_ID_POST_LE,
	ISP_BLC_ID_POST_SE,
	ISP_BLC_ID_PRE1_LE,
	ISP_BLC_ID_PRE1_SE,
	ISP_BLC_ID_MAX,
};

enum ISP_WBG_ID {
	ISP_WBG_ID_PRE0_LE = 0,
	ISP_WBG_ID_PRE0_SE,
	ISP_WBG_ID_POST_LE,
	ISP_WBG_ID_POST_SE,
	ISP_WBG_ID_POST_FS,
	ISP_WBG_ID_PRE1_LE,
	ISP_WBG_ID_PRE1_SE,
	ISP_WBG_ID_MAX,
};

struct dci_param {
	uint16_t cliplimit_sel;
	uint16_t strecth_th_0;		// black stretching threshold
	uint16_t strecth_th_1;		// white stretching threshold
	uint16_t strecth_strength_0;	// black stretching strength
	uint16_t strecth_strength_1;	// white stretching strength
};

void isp_set_base_addr(void *base);

/**
 * isp_init - setup isp
 *
 * @param :
 */
void isp_init(struct isp_ctx *ctx);


/**
 * isp_reset - do reset. This can be activated only if dma stop to avoid
 * hang fabric.
 *
 */
void isp_reset(struct isp_ctx *ctx);

/**
 * isp_stream_on - start/stop isp stream.
 *
 * @param on: 1 for stream start, 0 for stream stop
 */
void isp_streaming(struct isp_ctx *ctx, uint32_t on, enum cvi_isp_raw raw_num);


uint64_t *isp_get_phys_reg_bases(void);

int isp_get_vblock_info(struct isp_vblock_info **pinfo, uint32_t *nblocks,
			enum ISPCQ_ID_T cq_group);

int ispcq_init_cmdset(char *cmdset_ba, int size, uint64_t reg_base);
int ispcq_set_end_cmdset(char *cmdset_ba, int size);
int ispcq_init_adma_table(char *adma_tb, int num_cmdset);
int ispcq_add_descriptor(char *adma_tb, int index, uint64_t cmdset_addr,
			 uint32_t cmdset_size);
uint64_t ispcq_get_desc_addr(char *adma_tb, int index);
int ispcq_set_link_desc(char *adma_tb, int index,
			uint64_t target_desc_addr, int is_link);
int ispcq_set_end_desc(char *adma_tb, int index, int is_end);
int ispcq_engine_config(uint64_t *phys_regs, struct ispcq_config *cfg);
int ispcq_engine_start(uint64_t *phys_regs, enum ISPCQ_ID_T id);

struct vip_rect ispblk_crop_get_offset(struct isp_ctx *ctx, int crop_id, enum cvi_isp_raw raw_num);
void ispblk_crop_enable(struct isp_ctx *ctx, int crop_id, bool en);
int ispblk_crop_config(struct isp_ctx *ctx, int crop_id, struct vip_rect crop, enum cvi_isp_raw raw_num);
int ispblk_lscr_config(struct isp_ctx *ctx, int lscr_id, bool en, enum cvi_isp_raw raw_num);
int ispblk_lscr_set_lut(struct isp_ctx *ctx, int lscr_id, uint16_t *gain_lut,
			uint8_t lut_count, enum cvi_isp_raw raw_num);
int ispblk_lsc_config(struct isp_ctx *ctx, int lsc_id, bool en);
int ispblk_rgbmap_config(struct isp_ctx *ctx, int map_id, enum cvi_isp_raw raw_num);
void ispblk_lmap_chg_size(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
int ispblk_lmap_config(struct isp_ctx *ctx, int map_id, bool en, enum cvi_isp_raw raw_num);
void ispblk_aehist_reset(struct isp_ctx *ctx, int blk_id, enum cvi_isp_raw raw_num);
int ispblk_aehist_config(struct isp_ctx *ctx, int blk_id, bool enable, enum cvi_isp_raw raw_num);
int ispblk_awb_config(struct isp_ctx *ctx, int blk_id, bool enable, enum cvi_isp_raw raw_num);
int ispblk_af_config(struct isp_ctx *ctx, int blk_id, bool enable, enum cvi_isp_raw raw_num);
int ispblk_af_gamma_config(struct isp_ctx *ctx, int blk_id,
			   uint8_t sel, uint8_t *data, enum cvi_isp_raw raw_num);
int ispblk_af_gamma_enable(struct isp_ctx *ctx, int blk_id,
			   bool enable, uint8_t sel, enum cvi_isp_raw raw_num);
int ispblk_gms_config(struct isp_ctx *ctx, int blk_id, bool enable, enum cvi_isp_raw raw_num);
int ispblk_dpc_config(struct isp_ctx *ctx, enum ISP_RAW_PATH path,
		      union REG_ISP_DPC_2 reg2);
int ispblk_dpc_set_static(struct isp_ctx *ctx, enum ISP_RAW_PATH path,
			     uint16_t offset, uint32_t *bps, uint8_t count);
int ispblk_blc_set_offset(struct isp_ctx *ctx, int blc_id, uint16_t roffset,
			  uint16_t groffset, uint16_t gboffset,
			  uint16_t boffset);
int ispblk_blc_set_gain(struct isp_ctx *ctx, int blc_id, uint16_t rgain,
			uint16_t grgain, uint16_t gbgain, uint16_t bgain);
int ispblk_blc_enable(struct isp_ctx *ctx, int blc_id, bool en, bool bypass);
int ispblk_wbg_config(struct isp_ctx *ctx, int wbg_id, uint16_t rgain,
		      uint16_t ggain, uint16_t bgain);
int ispblk_wbg_enable(struct isp_ctx *ctx, int wbg_id, bool enable,
		      bool bypass);
void isp_fpn_config(struct isp_ctx *ctx, int fpn_id, bool enable);
void ispblk_fusion_hdr_cfg(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
int ispblk_fusion_config(struct isp_ctx *ctx, bool enable, bool bypass,
			 bool mc_enable, enum ISP_FS_OUT out_sel);
void ispblk_ltm_d_lut(struct isp_ctx *ctx, uint8_t sel, uint16_t *data);
void ispblk_ltm_b_lut(struct isp_ctx *ctx, uint8_t sel, uint16_t *data);
void ispblk_ltm_g_lut(struct isp_ctx *ctx, uint8_t sel, uint16_t *data);
void ispblk_ltm_enable(struct isp_ctx *ctx, bool en, bool bypass);
void ispblk_ltm_config(struct isp_ctx *ctx, bool dehn_en, bool dlce_en,
		       bool behn_en, bool blce_en);
void ispblk_manr_config(struct isp_ctx *ctx, bool en);
int ispblk_bnr_config(struct isp_ctx *ctx, enum ISP_BNR_OUT out_sel,
		      bool lsc_en, uint8_t ns_gain, uint8_t str);

int ispblk_rgb_config(struct isp_ctx *ctx);
void ispblk_rgb_prob_set_pos(struct isp_ctx *ctx, enum ISP_RGB_PROB_OUT out,
			     uint16_t x, uint16_t y);
void ispblk_rgb_prob_get_values(struct isp_ctx *ctx, uint16_t *r, uint16_t *g,
				uint16_t *b);
int ispblk_cfa_config(struct isp_ctx *ctx);
int ispblk_rgbee_config(struct isp_ctx *ctx, bool en, uint16_t osgain,
			uint16_t usgain);
int ispblk_rgbee_config(struct isp_ctx *ctx, bool en, uint16_t osgain,
			uint16_t usgain);
int ispblk_gamma_config(struct isp_ctx *ctx, uint8_t sel, uint16_t *data);
int ispblk_gamma_enable(struct isp_ctx *ctx, bool enable, uint8_t sel);
int ispblk_rgbdither_config(struct isp_ctx *ctx, bool en, bool mod_en,
			    bool histidx_en, bool fmnum_en);
int ispblk_ccm_config(struct isp_ctx *ctx, bool en, struct isp_ccm_cfg *cfg);
int ispblk_dhz_config(struct isp_ctx *ctx, bool en, struct isp_dhz_cfg *cfg);
int ispblk_hsv_config(struct isp_ctx *ctx, uint8_t sel, uint8_t type,
		      uint16_t *lut);
int ispblk_hsv_enable(struct isp_ctx *ctx, bool en, uint8_t sel, bool hsgain_en,
		      bool hvgain_en, bool htune_en, bool stune_en);
int ispblk_csc_config(struct isp_ctx *ctx);

int ispblk_yuvdither_config(struct isp_ctx *ctx, uint8_t sel, bool en,
			    bool mod_en, bool histidx_en, bool fmnum_en);
int ispblk_444_422_config(struct isp_ctx *ctx);
int ispblk_ynr_config(struct isp_ctx *ctx, enum ISP_YNR_OUT out_sel,
		      uint8_t ns_gain, uint8_t str);
int ispblk_cnr_config(struct isp_ctx *ctx, bool en, bool pfc_en,
		      uint8_t str_mode);
int ispblk_dci_config(struct isp_ctx *ctx, bool en, uint16_t *lut,
		      uint16_t lut_size);
void ispblk_dci_hist_gen(uint8_t *raw, uint16_t *hist);
void ispblk_dci_lut_gen(struct isp_ctx *ctx, uint16_t *hist,
			struct dci_param *param);
int ispblk_ee_config(struct isp_ctx *ctx, bool bypass);
int ispblk_ycur_config(struct isp_ctx *ctx, uint8_t sel, uint16_t *data);
int ispblk_ycur_enable(struct isp_ctx *ctx, bool enable, uint8_t sel);
int ispblk_3dlut_config(struct isp_ctx *ctx);

int ispblk_preraw_config(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
int ispblk_rawtop_config(struct isp_ctx *ctx);
int ispblk_yuvtop_config(struct isp_ctx *ctx);
int ispblk_isptop_config(struct isp_ctx *ctx);
void ispblk_csibdg_sw_reset(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void ispblk_csibdg_line_reach_config(struct isp_ctx *ctx, enum cvi_isp_raw raw_num, bool enable);
int ispblk_csibdg_config(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
struct vip_rect ispblk_csibdg_get_size(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void ispblk_csibdg_update_size(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
int ispblk_dma_buf_get_size(struct isp_ctx *ctx, int dmaid);
void ispblk_post_in_dma_update(struct isp_ctx *ctx, int dmaid, enum cvi_isp_raw raw_num);
void ispblk_dma_crop_update(struct isp_ctx *ctx, int dmaid, struct vip_rect crop);
int ispblk_dma_get_size(struct isp_ctx *ctx, int dmaid, uint32_t _w, uint32_t _h);
int ispblk_dma_config(struct isp_ctx *ctx, int dmaid, uint64_t buf_addr);
void ispblk_dma_setaddr(struct isp_ctx *ctx, uint32_t dmaid, uint64_t buf_addr);
uint64_t ispblk_dma_getaddr(struct isp_ctx *ctx, uint32_t dmaid);
int ispblk_dma_enable(struct isp_ctx *ctx, uint32_t dmaid, uint32_t on);
int ispblk_dma_dbg_st(struct isp_ctx *ctx, uint32_t dmaid, uint32_t bus_sel);

void isp_pchk_config(struct isp_ctx *ctx, uint8_t en_mask);
void isp_pchk_chk_status(struct isp_ctx *ctx, uint8_t en_mask,
			 uint32_t intr_status);

void isp_pre_trig(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void isp_post_trig(struct isp_ctx *ctx);
enum ISP_BAYER_TYPE isp_bayer_remap(enum ISP_BAYER_TYPE bayer_id,
	uint16_t x_offset, uint16_t y_offset);

union isp_intr isp_intr_get_mask(struct isp_ctx *ctx);
void isp_intr_set_mask(struct isp_ctx *ctx, union isp_intr intr_status);
union isp_intr isp_intr_status(struct isp_ctx *ctx);
void isp_intr_clr(struct isp_ctx *ctx, union isp_intr intr_status);
union isp_csi_intr isp_csi_intr_status(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void isp_csi_intr_clr(struct isp_ctx *ctx, union isp_csi_intr intr_status, enum cvi_isp_raw raw_num);
#if defined(__LINUX__)
void isp_register_dump(struct isp_ctx *ctx, struct seq_file *m, enum isp_dump_grp grp);
#endif
int ispblk_rgbmap_get_w_bit(struct isp_ctx *ctx, int dmaid);
struct isp_rgbmap_info ispblk_rgbmap_info(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void ispblk_tnr_rgbmap_chg(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void ispblk_tnr_grid_chg(struct isp_ctx *ctx);
void ispblk_tnr_post_chg(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void isp_first_frm_reset(struct isp_ctx *ctx, uint8_t reset);
uint32_t ispblk_preraw_dg_info(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
uint32_t ispblk_csibdg_dg_info(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
uint32_t ispblk_isptop_dg_info(struct isp_ctx *ctx);
void ispblk_csibdg_yuv_bypass_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_isptop_yuv_bypass_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_dma_yuv_bypass_config(struct isp_ctx *ctx, int dmaid, uint64_t buf_addr,
					const enum cvi_isp_raw raw_num);
void ispblk_post_cfg_update(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);


void ispblk_rawtop_tile(struct isp_ctx *ctx);
void ispblk_dpc_tile(struct isp_ctx *ctx, enum ISP_RAW_PATH path);
void ispblk_lsc_tile(struct isp_ctx *ctx, int lsc_id);
void ispblk_fusion_tile(struct isp_ctx *ctx);
void ispblk_ltm_tile(struct isp_ctx *ctx);
void ispblk_manr_tile(struct isp_ctx *ctx);
void ispblk_bnr_tile(struct isp_ctx *ctx);
void ispblk_cfa_tile(struct isp_ctx *ctx);
void ispblk_rgbee_tile(struct isp_ctx *ctx);
void ispblk_dhz_tile(struct isp_ctx *ctx);
void ispblk_rgbdither_tile(struct isp_ctx *ctx);
void ispblk_444_422_tile(struct isp_ctx *ctx);
void ispblk_cnr_tile(struct isp_ctx *ctx);
void ispblk_ee_tile(struct isp_ctx *ctx);
void ispblk_yuvdither_tile(struct isp_ctx *ctx);
void ispblk_dci_tile(struct isp_ctx *ctx);

void ispblk_ge_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_ge_config *cfg);
void ispblk_gamma_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_gamma_config *cfg);
void ispblk_ee_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_ee_config *cfg);
void ispblk_bnr_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_bnr_config *cfg);
void ispblk_cnr_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_cnr_config *cfg);
void ispblk_ynr_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_ynr_config *cfg);
void ispblk_pfc_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_pfc_config *cfg);
void ispblk_tnr_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_tnr_config *cfg,
	enum cvi_isp_raw raw_num);
void ispblk_dci_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_dci_config *cfg);
void ispblk_demosiac_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_demosiac_config *cfg);
void ispblk_3dlut_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_3dlut_config *cfg);
void ispblk_dpc_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_dpc_config *cfg);
void ispblk_lsc_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_lsc_config *cfg,
	enum cvi_isp_raw raw_num);
void ispblk_lscr_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_lscr_config *cfg);
void ispblk_ae_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_ae_config *cfg);
void ispblk_awb_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_awb_config *cfg);
void ispblk_af_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_af_config *cfg);
void ispblk_fswdr_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_fswdr_config *cfg);
void ispblk_drc_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_drc_config *cfg,
	enum cvi_isp_raw raw_num);
void ispblk_mono_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_mono_config *cfg);
void ispblk_hsv_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_hsv_config *cfg);
void ispblk_gms_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_gms_config *cfg);
void ispblk_ycur_tun_cfg(
	struct isp_ctx *ctx,
	struct cvi_vip_isp_ycur_config *cfg);

#ifdef __cplusplus
}
#endif

#endif /* _ISP_DRV_H_ */
