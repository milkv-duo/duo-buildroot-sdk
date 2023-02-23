#ifndef __VI_DRV_H__
#define __VI_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <mmio.h>

#include <isp_reg.h>
#include <vi_reg_blocks.h>
#include <vi_reg_fields.h>
#include <vi_tun_cfg.h>
#include <cvi_vi_ctx.h>
#include <vi_common.h>

#ifndef _OFST
#define _OFST(_BLK_T, _REG)       ((uintptr_t)&(((struct _BLK_T *)0)->_REG))
#endif

// #define _reg_read(addr) mmio_read_32((uintptr_t)addr)
// #define _reg_write(addr, data) mmio_write_32((uintptr_t)addr, (uint32_t)data)

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
		uintptr_t ofst = _OFST(_BLK_T, _REG);\
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

#define REG_ARRAY_UPDATE2_SIZE(addr, array, size)		\
	do {							\
		uint16_t i;					\
		for (i = 0; i < size; i += 2) {			\
			val = array[i];				\
			if ((i + 1) < size)			\
				val |= (array[i+1] << 16);	\
			_reg_write(addr + (i << 1), val);	\
		}						\
	} while (0)

#define REG_ARRAY_UPDATE2(addr, array)				\
	REG_ARRAY_UPDATE2_SIZE(addr, array, ARRAY_SIZE(array))

#define REG_ARRAY_UPDATE4(addr, array)				\
	do {							\
		uint16_t i;					\
		for (i = 0; i < ARRAY_SIZE(array); i += 4) {	\
			val = array[i];				\
			if ((i + 1) < ARRAY_SIZE(array))	\
				val |= (array[i+1] << 8);	\
			if ((i + 2) < ARRAY_SIZE(array))	\
				val |= (array[i+2] << 16);	\
			if ((i + 3) < ARRAY_SIZE(array))	\
				val |= (array[i+3] << 24);	\
			_reg_write(addr + i, val);		\
		}						\
	} while (0)

#define LTM_REG_ARRAY_UPDATE11(addr, array)                                   \
	do {                                                                  \
		uint32_t val;                                                 \
		val = array[0] | (array[1] << 5) | (array[2] << 10) |         \
		      (array[3] << 15) | (array[4] << 20) | (array[5] << 25); \
		_reg_write(addr, val);                                        \
		val = array[6] | (array[7] << 5) | (array[8] << 10) |         \
		      (array[9] << 15) | (array[10] << 20);                   \
		_reg_write(addr + 4, val);                                    \
	} while (0)

#define LTM_REG_ARRAY_UPDATE30(addr, array)                                   \
	do {                                                                  \
		uint8_t i, j;                                                 \
		uint32_t val;                                                 \
		for (i = 0, j = 0; i < ARRAY_SIZE(array); i += 6, j++) {      \
			val = array[i] | (array[i + 1] << 5) |                \
			      (array[i + 2] << 10) | (array[i + 3] << 15) |   \
			      (array[i + 4] << 20) | (array[i + 5] << 25);    \
			_reg_write(addr + j * 4, val);                        \
		}                                                             \
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
	ISP_FS_OUT_MOTION_PXL,
	ISP_FS_OUT_LE_BLD_WHT,
	ISP_FS_OUT_SE_BLD_WHT,
	ISP_FS_OUT_MOTION_LUT,
	ISP_FS_OUT_AC_FS,
	ISP_FS_OUT_DELTA_LE,
	ISP_FS_OUT_DELTA_SE,
	ISP_FS_OUT_MAX,
};

enum isp_dump_grp {
	ISP_DUMP_PRERAW = 0x1,
	ISP_DUMP_POSTRAW = 0x2,
	ISP_DUMP_ALL = 0x4,
	ISP_DUMP_DMA = 0x8,
	ISP_DUMP_ALL_DMA = 0x10,
};

enum ISP_CCM_ID {
	ISP_CCM_ID_0 = 0,
	ISP_CCM_ID_1,
	ISP_CCM_ID_2,
	ISP_CCM_ID_3,
	ISP_CCM_ID_4,
	ISP_CCM_ID_MAX,
};

enum ISP_LSCR_ID {
	ISP_LSCR_ID_PRE0_FE_LE = 0,
	ISP_LSCR_ID_PRE0_FE_SE,
	ISP_LSCR_ID_PRE1_FE_LE,
	ISP_LSCR_ID_PRE1_FE_SE,
	ISP_LSCR_ID_PRE_BE_LE,
	ISP_LSCR_ID_PRE_BE_SE,
	ISP_LSCR_ID_MAX
};

enum ISP_PRE_PROC_ID {
	ISP_IR_PRE_PROC_ID_LE,
	ISP_IR_PRE_PROC_ID_SE,
	ISP_IR_PRE_PROC_ID_MAX
};

struct lmap_cfg {
	u8 pre_chg[2]; //le/se
	u8 pre_w_bit;
	u8 pre_h_bit;
	u8 post_w_bit;
	u8 post_h_bit;
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

struct tile {
	u16 start;
	u16 end;
};

struct isp_ccm_cfg {
	u16 coef[3][3];
};

struct _fe_dbg_i {
	uint32_t		fe_idle_sts;
	uint32_t		fe_done_sts;
};

struct _be_dbg_i {
	uint32_t		be_done_sts;
	uint32_t		be_dma_idle_sts;
};

struct _post_dbg_i {
	uint32_t		top_sts;
};

struct _dma_dbg_i {
	uint32_t		wdma_0_err_sts;
	uint32_t		wdma_0_idle;
	uint32_t		wdma_1_err_sts;
	uint32_t		wdma_1_idle;
	uint32_t		rdma_err_sts;
	uint32_t		rdma_idle;
};

struct _isp_dg_info {
	struct _fe_dbg_i	fe_sts;
	struct _be_dbg_i	be_sts;
	struct _post_dbg_i	post_sts;
	struct _dma_dbg_i	dma_sts;
	uint32_t		isp_top_sts;
	uint32_t		bdg_chn_debug[ISP_FE_CHN_MAX];
	uint32_t		bdg_int_sts_0;
	uint32_t		bdg_int_sts_1;
	uint32_t		bdg_fifo_of_cnt;
	uint8_t			bdg_w_gt_cnt[ISP_FE_CHN_MAX];
	uint8_t			bdg_w_ls_cnt[ISP_FE_CHN_MAX];
	uint8_t			bdg_h_gt_cnt[ISP_FE_CHN_MAX];
	uint8_t			bdg_h_ls_cnt[ISP_FE_CHN_MAX];
};

struct isp_grid_s_info {
	u8 w_bit;
	u8 h_bit;
};

struct _isp_cfg {
	uint32_t		csibdg_width;
	uint32_t		csibdg_height;
	uint32_t		max_width;
	uint32_t		max_height;
	uint32_t		post_img_w;
	uint32_t		post_img_h;
	uint32_t		drop_ref_frm_num;
	uint32_t		drop_frm_cnt;
	struct vi_rect		crop;
	struct vi_rect		crop_se;
	struct vi_rect		postout_crop;
	struct _isp_dg_info	dg_info;
	struct isp_grid_s_info	rgbmap_i;
	struct isp_grid_s_info	lmap_i;
	enum ISP_BAYER_TYPE	rgb_color_mode;
	enum _VI_INTF_MODE_E	infMode;
	enum _VI_WORK_MODE_E	muxMode;
	enum _VI_YUV_DATA_SEQ_E enDataSeq;

	uint32_t		is_patgen_en		: 1;
	uint32_t		is_offline_preraw	: 1;
	uint32_t		is_yuv_bypass_path	: 1;
	uint32_t		is_hdr_on		: 1;
	uint32_t		is_hdr_detail_en	: 1;
	uint32_t		is_tile			: 1;
	uint32_t		is_fbc_on		: 1;
	uint32_t		is_rgbir_sensor		: 1;
	uint32_t		is_offline_scaler	: 1;
	uint32_t		is_stagger_vsync	: 1;
	uint32_t		is_slice_buf_on		: 1;
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
	uint32_t		src_width;
	uint32_t		src_height;
	uint32_t		img_width;
	uint32_t		img_height;
	uint32_t		crop_x;
	uint32_t		crop_y;
	uint32_t		crop_se_x;
	uint32_t		crop_se_y;

	uintptr_t		*phys_regs;

	struct _isp_cfg		isp_pipe_cfg[ISP_PRERAW_MAX];
	enum ISP_BAYER_TYPE	rgb_color_mode[ISP_PRERAW_MAX];
	uint8_t			rgbmap_prebuf_idx;
	uint8_t			rawb_chnstr_num;
	uint8_t			total_chn_num;

	uint8_t			cam_id;
	uint32_t		is_dual_sensor      : 1;
	uint32_t		is_yuv_sensor       : 1;
	uint32_t		is_hdr_on           : 1;
	uint32_t		is_3dnr_on          : 1;
	uint32_t		is_dpcm_on          : 1;
	uint32_t		is_offline_be       : 1;
	uint32_t		is_offline_postraw  : 1;
	uint32_t		is_sublvds_path     : 1;
	uint32_t		is_fbc_on           : 1;
	uint32_t		is_ctrl_inited      : 1;
	uint32_t		is_slice_buf_on     : 1;
};

struct vi_fbc_cfg {
	u8	cu_size;
	u8	target_cr; //compression ratio
	u8	is_lossless; // lossless or lossy
	u32	y_bs_size; //Y WDMA seglen
	u32	c_bs_size; //C WDMA seglen
	u32	y_buf_size; //total Y buf size
	u32	c_buf_size; //total C buf size
};

struct slc_cfg_s {
	u32 le_buf_size;
	u32 se_buf_size;
	u32 le_w_thshd;
	u32 se_w_thshd;
	u32 le_r_thshd;
	u32 se_r_thshd;
};

struct slice_buf_s {
	u16 line_delay; //sensor exposure ratio
	u16 buffer; //cover for read/write latency, axi latency..etc
	u8  max_grid_size; //rgbmap grid size
	u8  min_r_thshd; // minimum read threshold
	struct slc_cfg_s main_path;
	struct slc_cfg_s sub_path;
};

/**********************************************************
 *	SW scenario path check APIs
 **********************************************************/
u32 _is_fe_be_online(struct isp_ctx *ctx);
u32 _is_be_post_online(struct isp_ctx *ctx);
u32 _is_all_online(struct isp_ctx *ctx);
u32 _is_post_sclr_online(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);

/****************************************************************************
 * Interfaces
 ****************************************************************************/
uint64_t _mempool_get_addr(void);
int64_t _mempool_pop(uint32_t size);

void vi_set_base_addr(void *base);
uintptr_t *isp_get_phys_reg_bases(void);
void isp_debug_dump(struct isp_ctx *ctx);
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


struct isp_grid_s_info ispblk_rgbmap_info(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
struct isp_grid_s_info ispblk_lmap_info(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void ispblk_preraw_fe_config(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void ispblk_preraw_vi_sel_config(struct isp_ctx *ctx);
void ispblk_pre_wdma_ctrl_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_preraw_be_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_raw_rdma_ctrl_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_rawtop_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_rgbtop_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_yuvtop_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_isptop_config(struct isp_ctx *ctx);

void ispblk_crop_enable(struct isp_ctx *ctx, int crop_id, bool en);
int ispblk_crop_config(struct isp_ctx *ctx, int crop_id, struct vi_rect crop);
int ccm_find_hwid(int id);
int blc_find_hwid(int id);
void ispblk_blc_set_offset(struct isp_ctx *ctx, int blc_id,
				uint16_t roffset, uint16_t groffset,
				uint16_t gboffset, uint16_t boffset);
void ispblk_blc_set_2ndoffset(struct isp_ctx *ctx, int blc_id,
				uint16_t roffset, uint16_t groffset,
				uint16_t gboffset, uint16_t boffset);
void ispblk_blc_set_gain(struct isp_ctx *ctx, int blc_id,
				uint16_t rgain, uint16_t grgain,
				uint16_t gbgain, uint16_t bgain);
void ispblk_blc_enable(struct isp_ctx *ctx, int blc_id, bool en, bool bypass);
int wbg_find_hwid(int id);
int ispblk_wbg_config(struct isp_ctx *ctx, int wbg_id, uint16_t rgain, uint16_t ggain, uint16_t bgain);
int ispblk_wbg_enable(struct isp_ctx *ctx, int wbg_id, bool enable, bool bypass);
void ispblk_lscr_set_lut(struct isp_ctx *ctx, int lscr_id, uint16_t *gain_lut, uint8_t lut_count);
void ispblk_lscr_config(struct isp_ctx *ctx, int lscr_id, bool en);

uint64_t ispblk_dma_getaddr(struct isp_ctx *ctx, uint32_t dmaid);
int ispblk_dma_config(struct isp_ctx *ctx, int dmaid, uint64_t buf_addr);
void ispblk_dma_setaddr(struct isp_ctx *ctx, uint32_t dmaid, uint64_t buf_addr);
void ispblk_dma_enable(struct isp_ctx *ctx, uint32_t dmaid, uint32_t on, uint8_t dma_disable);
int ispblk_dma_buf_get_size2(struct isp_ctx *ctx, int dmaid, u8 raw_num);
void ispblk_dma_set_sw_mode(struct isp_ctx *ctx, uint32_t dmaid, bool is_sw_mode);

/****************************************************************************
 *	PRERAW FE SUBSYS
 ****************************************************************************/
void ispblk_csidbg_dma_wr_en(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num, const u8 chn_num, const u8 en);
void ispblk_csibdg_wdma_crop_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num, struct vi_rect crop);
void ispblk_csibdg_crop_update(struct isp_ctx *ctx, enum cvi_isp_raw raw_num, bool en);
int ispblk_csibdg_config(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void ispblk_rgbmap_config(struct isp_ctx *ctx, int map_id, bool en, enum cvi_isp_raw raw_num);
void ispblk_lmap_chg_size(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num, const enum cvi_isp_pre_chn_num chn_num);
void ispblk_lmap_config(struct isp_ctx *ctx, int map_id, bool en);

/****************************************************************************
 *	PRE BE SUBSYS
 ****************************************************************************/
void ispblk_dpc_config(struct isp_ctx *ctx, enum ISP_RAW_PATH path, bool enable, uint8_t test_case);
void ispblk_dpc_set_static(struct isp_ctx *ctx, enum ISP_RAW_PATH path,
			     uint16_t offset, uint32_t *bps, uint8_t count);
void ispblk_af_config(struct isp_ctx *ctx, bool enable);
void ispblk_rgbir_preproc_config(struct isp_ctx *ctx,
	uint8_t *wdata, int16_t *data_r, int16_t *data_g, int16_t *data_b);
void ispblk_ir_proc_config(struct isp_ctx *ctx, uint8_t *gamma);

/****************************************************************************
 *	RAW TOP SUBSYS
 ****************************************************************************/
void ispblk_bnr_config(struct isp_ctx *ctx, enum ISP_BNR_OUT out_sel, bool lsc_en, uint8_t ns_gain, uint8_t str);
void ispblk_cfa_config(struct isp_ctx *ctx);
void ispblk_aehist_reset(struct isp_ctx *ctx, int blk_id, enum cvi_isp_raw raw_num);
void ispblk_aehist_config(struct isp_ctx *ctx, int blk_id, bool enable);
void ispblk_gms_config(struct isp_ctx *ctx, bool enable);
void ispblk_rgbcac_config(struct isp_ctx *ctx, bool en);
void ispblk_lcac_config(struct isp_ctx *ctx, bool en, uint8_t test_case);

/****************************************************************************
 *	RGB TOP SUBSYS
 ****************************************************************************/
void ispblk_lsc_config(struct isp_ctx *ctx, bool en);
void ispblk_fusion_hdr_cfg(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void ispblk_fusion_config(struct isp_ctx *ctx, bool enable, bool mc_enable, enum ISP_FS_OUT out_sel);
void ispblk_ltm_d_lut(struct isp_ctx *ctx, uint8_t sel, uint16_t *data);
void ispblk_ltm_b_lut(struct isp_ctx *ctx, uint8_t sel, uint16_t *data);
void ispblk_ltm_g_lut(struct isp_ctx *ctx, uint8_t sel, uint16_t *data);
void ispblk_ltm_config(struct isp_ctx *ctx, u8 ltm_en, u8 dehn_en, u8 behn_en, u8 ee_en);
void ispblk_ccm_config(struct isp_ctx *ctx, enum ISP_BLK_ID_T blk_id, bool en, struct isp_ccm_cfg *cfg);
void ispblk_hist_v_config(struct isp_ctx *ctx, bool en, uint8_t test_case);
void ispblk_dhz_config(struct isp_ctx *ctx, bool en);
void ispblk_ygamma_config(struct isp_ctx *ctx, bool en,
				uint8_t sel, uint16_t *data, uint8_t inv, uint8_t test_case);
void ispblk_ygamma_enable(struct isp_ctx *ctx, bool enable);
void ispblk_gamma_config(struct isp_ctx *ctx, bool en, uint8_t sel, uint16_t *data, uint8_t inv);
void ispblk_gamma_enable(struct isp_ctx *ctx, bool enable);
void ispblk_clut_config(struct isp_ctx *ctx, bool en,
				int16_t *r_lut, int16_t *g_lut, int16_t *b_lut);
void ispblk_rgbdither_config(struct isp_ctx *ctx, bool en, bool mod_en, bool histidx_en, bool fmnum_en);
void ispblk_csc_config(struct isp_ctx *ctx);
void ispblk_manr_config(struct isp_ctx *ctx, bool en);
void ispblk_ir_merge_config(struct isp_ctx *ctx);

/****************************************************************************
 *	YUV TOP SUBSYS
 ****************************************************************************/
int ispblk_pre_ee_config(struct isp_ctx *ctx, bool en);
int ispblk_yuvdither_config(struct isp_ctx *ctx, uint8_t sel, bool en,
			    bool mod_en, bool histidx_en, bool fmnum_en);
void ispblk_tnr_config(struct isp_ctx *ctx, bool en, u8 test_case);
void ispblk_fbc_clear_fbcd_ring_base(struct isp_ctx *ctx, u8 raw_num);
void ispblk_fbc_chg_to_sw_mode(struct isp_ctx *ctx, u8 raw_num);
void vi_fbc_calculate_size(struct isp_ctx *ctx, u8 raw_num);
void ispblk_fbc_ring_buf_config(struct isp_ctx *ctx, u8 en);
void ispblk_fbcd_config(struct isp_ctx *ctx, bool en);
void ispblk_fbce_config(struct isp_ctx *ctx, bool en);
void ispblk_cnr_config(struct isp_ctx *ctx, bool en, bool pfc_en, uint8_t str_mode, uint8_t test_case);
void ispblk_ynr_config(struct isp_ctx *ctx, enum ISP_YNR_OUT out_sel, uint8_t ns_gain);
int ispblk_ee_config(struct isp_ctx *ctx, bool en);
void ispblk_dci_config(struct isp_ctx *ctx, bool en, uint16_t *lut, uint8_t test_case);
void ispblk_ldci_config(struct isp_ctx *ctx, bool en, uint8_t test_case);
void ispblk_ca_config(struct isp_ctx *ctx, bool en);
void ispblk_ca_lite_config(struct isp_ctx *ctx, bool en);
void ispblk_ycur_config(struct isp_ctx *ctx, bool en, uint8_t sel, uint16_t *data);
void ispblk_ycur_enable(struct isp_ctx *ctx, bool enable, uint8_t sel);

void isp_pre_trig(struct isp_ctx *ctx, enum cvi_isp_raw raw_num, const u8 chn_num);
void isp_post_trig(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);

void isp_intr_set_mask(struct isp_ctx *ctx);
void isp_intr_status(
	struct isp_ctx *ctx,
	union REG_ISP_TOP_INT_EVENT0 *s0,
	union REG_ISP_TOP_INT_EVENT1 *s1,
	union REG_ISP_TOP_INT_EVENT2 *s2);
void isp_csi_intr_status(
	struct isp_ctx *ctx,
	enum cvi_isp_raw raw_num,
	union REG_ISP_CSI_BDG_INTERRUPT_STATUS_0 *s0,
	union REG_ISP_CSI_BDG_INTERRUPT_STATUS_1 *s1);

void ispblk_tnr_rgbmap_chg(struct isp_ctx *ctx, enum cvi_isp_raw raw_num, const u8 chn_num);
void ispblk_tnr_post_chg(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);

/****************************************************************************
 *	Runtime Control Flow Config
 ****************************************************************************/
void isp_first_frm_reset(struct isp_ctx *ctx, uint8_t reset);
void ispblk_post_yuv_cfg_update(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_post_cfg_update(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
void ispblk_pre_be_cfg_update(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
int ispblk_dma_get_size(struct isp_ctx *ctx, int dmaid, uint32_t _w, uint32_t _h);
uint32_t ispblk_csibdg_chn_dbg(struct isp_ctx *ctx, enum cvi_isp_raw raw_num, enum cvi_isp_pre_chn_num chn);
struct _fe_dbg_i ispblk_fe_dbg_info(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
struct _be_dbg_i ispblk_be_dbg_info(struct isp_ctx *ctx);
struct _post_dbg_i ispblk_post_dbg_info(struct isp_ctx *ctx);
struct _dma_dbg_i ispblk_dma_dbg_info(struct isp_ctx *ctx);
int isp_frm_err_handler(struct isp_ctx *ctx, const enum cvi_isp_raw err_raw_num, const u8 step);

/****************************************************************************
 *	YUV Bypass Control Flow Config
 ****************************************************************************/
void ispblk_csibdg_yuv_bypass_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num);
u32 ispblk_dma_yuv_bypass_config(struct isp_ctx *ctx, uint32_t dmaid, uint64_t buf_addr,
					const enum cvi_isp_raw raw_num);

/****************************************************************************
 *	Slice buffer Control
 ****************************************************************************/
void vi_calculate_slice_buf_setting(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void isp_slice_buf_trig(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void manr_clear_prv_ring_base(struct isp_ctx *ctx, enum cvi_isp_raw raw_num);
void ispblk_slice_buf_config(struct isp_ctx *ctx, const enum cvi_isp_raw raw_num, u8 en);

/*******************************************************************************
 *	Tuning interfaces
 ******************************************************************************/
void vi_tuning_gamma_ips_update(
	struct isp_ctx *ctx,
	enum cvi_isp_raw raw_num);
void vi_tuning_dci_update(
	struct isp_ctx *ctx,
	enum cvi_isp_raw raw_num);
int vi_tuning_buf_setup(void);
void vi_tuning_buf_release(void);
void *vi_get_tuning_buf_addr(u32 *size);
void vi_tuning_buf_clear(void);


#ifdef __cplusplus
}
#endif

#endif /* __VI_DRV_H__ */
