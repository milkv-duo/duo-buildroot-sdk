#ifndef _CIF_DRV_H_
#define _CIF_DRV_H_

#define MAX_LINK_NUM		3
#define MAX_HDR_FRAME_NUM	2
#define MAX_VC_NUM		4


#define EXPORT_SYMBOL_GPL(x)

#ifndef _OFST
#define _OFST(_BLK_T, _REG)       ((uintptr_t)&(((struct _BLK_T *)0)->_REG))
#endif

#define CIF_RD_REG(_BA, _BLK_T, _REG) \
	(_reg_read(_BA+_OFST(_BLK_T, _REG)))

#define CIF_RD_BITS(_BA, _BLK_T, _REG, _FLD) \
	({\
		typeof(((struct _BLK_T *)0)->_REG) _r;\
		_r.raw = _reg_read(_BA+_OFST(_BLK_T, _REG));\
		_r.bits._FLD;\
	})

#define CIF_WR_REG(_BA, _BLK_T, _REG, _V) \
	(_reg_write((_BA+_OFST(_BLK_T, _REG)), _V))

#define CIF_WR_BITS(_BA, _BLK_T, _REG, _FLD, _V) \
	do {\
		typeof(((struct _BLK_T *)0)->_REG) _r;\
		_r.raw = _reg_read(_BA+_OFST(_BLK_T, _REG));\
		_r.bits._FLD = _V;\
		_reg_write((_BA+_OFST(_BLK_T, _REG)), _r.raw);\
	} while (0)

#define CIF_WR_BITS_GRP2(_BA, _BLK_T, _REG, _FLD1, _V1, _FLD2, _V2) \
	do {\
		typeof(((struct _BLK_T *)0)->_REG) _r;\
		_r.raw = _reg_read(_BA+_OFST(_BLK_T, _REG));\
		_r.bits._FLD1 = _V1;\
		_r.bits._FLD2 = _V2;\
		_reg_write((_BA+_OFST(_BLK_T, _REG)), _r.raw);\
	} while (0)

#define CIF_WR_BITS_GRP3(_BA, _BLK_T, _REG, _FLD1, _V1, _FLD2, _V2, \
			_FLD3, _V3) \
	do {\
		typeof(((struct _BLK_T *)0)->_REG) _r;\
		_r.raw = _reg_read(_BA+_OFST(_BLK_T, _REG));\
		_r.bits._FLD1 = _V1;\
		_r.bits._FLD2 = _V2;\
		_r.bits._FLD3 = _V3;\
		_reg_write((_BA+_OFST(_BLK_T, _REG)), _r.raw);\
	} while (0)

#define CIF_WO_BITS(_BA, _BLK_T, _REG, _FLD, _V) \
	do {\
		typeof(((struct _BLK_T *)0)->_REG) _r;\
		_r.raw = 0;\
		_r.bits._FLD = _V;\
		_reg_write((_BA+_OFST(_BLK_T, _REG)), _r.raw);\
	} while (0)

#define CIF_INT_STS_ECC_ERR_OFFSET	0U
#define CIF_INT_STS_CRC_ERR_OFFSET	1U
#define CIF_INT_STS_HDR_ERR_OFFSET	2U
#define CIF_INT_STS_WC_ERR_OFFSET	3U
#define CIF_INT_STS_FIFO_FULL_OFFSET	4U

#define CIF_INT_STS_ECC_ERR_MASK	(1U<<CIF_INT_STS_ECC_ERR_OFFSET)
#define CIF_INT_STS_CRC_ERR_MASK	(1U<<CIF_INT_STS_CRC_ERR_OFFSET)
#define CIF_INT_STS_HDR_ERR_MASK	(1U<<CIF_INT_STS_HDR_ERR_OFFSET)
#define CIF_INT_STS_WC_ERR_MASK		(1U<<CIF_INT_STS_WC_ERR_OFFSET)
#define CIF_INT_STS_FIFO_FULL_MASK	(1U<<CIF_INT_STS_FIFO_FULL_OFFSET)

enum cif_type {
	CIF_TYPE_CSI,
	CIF_TYPE_SUBLVDS,
	CIF_TYPE_HISPI,
	CIF_TYPE_TTL,
	CIF_TYPE_BT_DMUX,
	CIF_TYPE_NUM
};

enum sublvds_fmt {
	CIF_SLVDS_8_BIT,
	CIF_SLVDS_10_BIT,
	CIF_SLVDS_12_BIT
};

enum sublvds_hdr {
	CIF_SLVDS_HDR_PAT1,
	CIF_SLVDS_HDR_PAT2
};

enum hispi_mode {
	CIF_HISPI_MODE_PKT_SP,
	CIF_HISPI_MODE_STREAM_SP
};

enum ttl_sensor_fmt {
	TTL_SENSOR_8_BIT,
	TTL_SENSOR_10_BIT,
	TTL_SENSOR_12_BIT,
	TTL_SENSOR_16_BIT
};

enum ttl_bt_fmt_out {
	TTL_BT_FMT_OUT_CBYCRY,
	TTL_BT_FMT_OUT_CRYCBY,
	TTL_BT_FMT_OUT_YCBYCR,
	TTL_BT_FMT_OUT_YCRYCB,
};

enum ttl_fmt {
	TTL_SYNC_PAT_9B_BT656,
	TTL_SYNC_PAT_17B_BT1120,
	TTL_VHS_11B_BT601,
	TTL_VHS_19B_BT601,
	TTL_VDE_11B_BT601,
	TTL_VDE_19B_BT601,
	TTL_VSDE_11B_BT601,
	TTL_VSDE_19B_BT601,
	TTL_SYNC_PAT_SENSOR = 8,
	TTL_VHS_SENSOR = 10,
	TTL_VDE_SENSOR = 12,
	TTL_VSDE_SENSOR = 14,
	TTL_CUSTOM_0,
};

enum csi_fmt {
	CSI_YUV422_8B,
	CSI_YUV422_10B,
	CSI_RAW_8,
	CSI_RAW_10,
	CSI_RAW_12
};

enum csi_vs_gen_mode {
	CSI_VGEN_MODE_FS,
	CSI_VGEN_MODE_FE,
	CSI_VGEN_MODE_FS_FE
};

enum csi_hdr_mode {
	CSI_HDR_MODE_VC,
	CSI_HDR_MODE_ID,
	CSI_HDR_MODE_DT,
	CSI_HDR_MODE_DOL
};

enum cif_endian {
	CIF_SLVDS_ENDIAN_MSB,
	CIF_SLVDS_ENDIAN_LSB
};

struct sublvds_sync_code {
	uint16_t		n0_lef_sav;
	uint16_t		n0_lef_eav;
	uint16_t		n0_sef_sav;
	uint16_t		n0_sef_eav;
	uint16_t		n1_lef_sav;
	uint16_t		n1_lef_eav;
	uint16_t		n1_sef_sav;
	uint16_t		n1_sef_eav;
	uint16_t		n0_lsef_sav;
	uint16_t		n0_lsef_eav;
	uint16_t		n1_lsef_sav;
	uint16_t		n1_lsef_eav;
};

struct hispi_sync_code {
	uint16_t		t1_sol;
	uint16_t		t1_eol;
	uint16_t		t2_sol;
	uint16_t		t2_eol;
	uint16_t		t1_sof;
	uint16_t		t1_eof;
	uint16_t		t2_sof;
	uint16_t		t2_eof;
	uint16_t		vsync_gen;
};

struct sync_code_s {
	uint16_t		norm_bk_sav;
	uint16_t		norm_bk_eav;
	uint16_t		norm_sav;
	uint16_t		norm_eav;
	uint16_t		n0_bk_sav;
	uint16_t		n0_bk_eav;
	uint16_t		n1_bk_sav;
	uint16_t		n1_bk_eav;
	union {
		struct sublvds_sync_code	slvds;
		struct hispi_sync_code		hispi;
	};
};

/*
 * @v_front_porch: the line inteval between the last data line and created VSYNC
 * @lane_nm: lane number
 * @hdr_hblank: the pixel interval between the lef and sef line.
 * @h_size: the pixel number of one data line.
 * @hdr_mode: SUBLVDS pattern
 * @endian: the endian of the raw data.
 * @info_line_num: information line num, for hdr pattern 2 only.
 * @hdr_v_fp: v_front_porch in hdr mode.
 */
struct param_sublvds {
	uint16_t		v_front_porch;
	uint16_t		lane_num;
	uint16_t		hdr_hblank[2];
	uint16_t		h_size;
	enum sublvds_hdr	hdr_mode;
	enum cif_endian		endian;
	enum cif_endian		wrap_endian;
	enum sublvds_fmt	fmt;
	uint16_t		hdr_v_fp;
	struct sync_code_s	sync_code;
};

/*
 * @lane_nm: lane number
 * @h_size: the pixel number of one data line.
 * @v_size: the line number of one frame
 * @hdr_se_line_shift: the line interval between the le and se readout.
 * @mode: HiSPi packetized-SP or streaming-SP
 * @endian: the endian of the raw data.
 */
struct param_hispi {
	uint16_t		lane_num;
	uint16_t		h_size;
	enum hispi_mode		mode;
	enum cif_endian		endian;
	enum cif_endian		wrap_endian;
	enum sublvds_fmt	fmt;
	struct sync_code_s	sync_code;
};

enum mipi_dlane_state_e {
	HS_IDLE = 0,
	HS_SYNC,
	HS_SKEW_CAL,
	HS_ALT_CAL,
	HS_PREAMPLE,
	HS_HST,
	HS_ERR,
	HS_STATE_NUM
};

enum mipi_deskew_state_e {
	DESKEW_IDLE = 0,
	DESKEW_START,
	DESKEW_DONE,
	DESKEW_NUM
};

union mipi_phy_state {
	uint32_t raw;
	struct {
		uint32_t clk_hs_state		: 1;
		uint32_t clk_ulps_state		: 1;
		uint32_t clk_stop_state		: 1;
		uint32_t clk_err_state		: 1;
		uint32_t p1_clk_hs_state	: 1;
		uint32_t p1_clk_ulps_state	: 1;
		uint32_t p1_clk_stop_state	: 1;
		uint32_t p1_clk_err_state	: 1;
		uint32_t d0_datahs_state	: 3;
		uint32_t reserve1		: 1;
		uint32_t d1_datahs_state	: 3;
		uint32_t reserve2		: 1;
		uint32_t d2_datahs_state	: 3;
		uint32_t reserve3		: 1;
		uint32_t d3_datahs_state	: 3;
		uint32_t reserve4		: 1;
		uint32_t deskew_state		: 2;
		uint32_t p1_deskew_state	: 2;
		uint32_t reserve5		: 4;
	} bits;
};

/*
 * @fmt: ttl format
 */
enum ttl_vi_mode_e {
	VI_RAW = 1,
	VI_BT601,
	VI_BT656,
	VI_BT1120,
};

enum ttl_vi_from_e {
	FROM_VI0 = 0,
	FROM_VI1,
	FROM_VI2,
};

enum ttl_vi_func_e {
	VI_FUNC_VS,
	VI_FUNC_HS,
	VI_FUNC_VDE,
	VI_FUNC_HDE,
	VI_FUNC_D0,
	VI_FUNC_D1,
	VI_FUNC_D2,
	VI_FUNC_D3,
	VI_FUNC_D4,
	VI_FUNC_D5,
	VI_FUNC_D6,
	VI_FUNC_D7,
	VI_FUNC_D8,
	VI_FUNC_D9,
	VI_FUNC_D10,
	VI_FUNC_D11,
	VI_FUNC_D12,
	VI_FUNC_D13,
	VI_FUNC_D14,
	VI_FUNC_D15,
	VI_FUNC_NUM,
};

struct param_ttl {
	enum ttl_fmt		fmt;
	enum ttl_sensor_fmt	sensor_fmt;
	enum ttl_bt_fmt_out	fmt_out;
	uint16_t		width;
	uint16_t		height;
	uint16_t		v_bp;
	uint16_t		h_bp;
	uint32_t		clk_inv;
	uint32_t		vi_sel;
	enum ttl_vi_from_e	vi_from;
};

/*
 * @lane_nm: lane number
 * @fmt: ttl format
 */
struct param_csi {
	uint16_t		lane_num;
	enum csi_fmt		fmt;
	enum csi_vs_gen_mode	vs_gen_mode;
	enum csi_hdr_mode	hdr_mode;
	uint16_t		data_type[MAX_HDR_FRAME_NUM];
	uint16_t		decode_type;
	uint8_t			vc_mapping[MAX_VC_NUM];
};

/*
 * @fmt: bt_demux format
 */
#define MAX_BT_DEMUX_NUM	4

enum cif_btdmux_mode_e {
	CIF_BTDMUX_DISABLE = 0,
	CIF_BTDMUX_2,
	CIF_BTDMUX_3,
	CIF_BTDMUX_4,
};

struct cif_btdmux_sync_s {
	uint8_t			sav_vld;
	uint8_t			sav_blk;
	uint8_t			eav_vld;
	uint8_t			eav_blk;
};

struct param_btdemux {
	enum ttl_fmt		fmt;
	enum cif_btdmux_mode_e	demux;
	uint16_t		width;
	uint16_t		height;
	uint16_t		v_fp;
	uint16_t		h_fp;
	uint16_t		v_bp;
	uint16_t		h_bp;
	uint32_t		clk_inv;
	uint8_t			sync_code_part_A[3];
	struct cif_btdmux_sync_s
				sync_code_part_B[MAX_BT_DEMUX_NUM];
	uint8_t			yc_exchg;
};

/*
 * @sublvds: config of sublvds
 * @hispi: config of hispi
 * @ttl: config of ttl
 * @csi: config of csi
 */
union cif_cfg {
	struct param_sublvds		sublvds;
	struct param_hispi		hispi;
	struct param_ttl		ttl;
	struct param_csi		csi;
	struct param_btdemux		btdemux;
};

/*
 * @type: camera interface type
 * @cfg: union of interface config
 * @hdr_manual: hdr manual mode enable
 * @hdr_shfit: line shift between le and se
 * @hdr_vsize: vertical size
 * @hdr_en: multi frame enable
 */
struct cif_param {
	enum cif_type		type;
	union cif_cfg		cfg;
	uint32_t		hdr_manual;
	uint16_t		hdr_shift;
	uint16_t		hdr_vsize;
	uint16_t		hdr_rm_padding;
	uint16_t		info_line_num;
	uint16_t		hdr_en;
};

/*
 * @pyhs_regs: index by enum ISP_BLK_ID_T, always phys reg
 * @vreg_bases: index by enum ISP_BLK_ID_T
 * @vreg_bases_pa: index by enum ISP_BLK_ID_T
 */
struct cif_ctx {
	uintptr_t		*mac_phys_regs;
	uintptr_t		*wrap_phys_regs;
	struct cif_param	*cur_config;
	uint16_t		mac_num;
};

enum phy_lane_id_e {
	CIF_PHY_LANE_0 = 0,
	CIF_PHY_LANE_1,
	CIF_PHY_LANE_2,
	CIF_PHY_LANE_3,
	CIF_PHY_LANE_4,
	CIF_PHY_LANE_5,
	CIF_PHY_LANE_NUM,
};

enum lane_id_e {
	CIF_LANE_CLK = 0,
	CIF_LANE_0,
	CIF_LANE_1,
	CIF_LANE_2,
	CIF_LANE_3,
	CIF_LANE_NUM,
};

enum csi_decode_fmt_e {
	DEC_FMT_YUV422_8 = 0,
	DEC_FMT_YUV422_10,
	DEC_FMT_RAW8,
	DEC_FMT_RAW10,
	DEC_FMT_RAW12,
	DEC_FMT_NUM,
};

enum cif_clk_edge_e {
	CIF_CLK_RISING = 0,
	CIF_CLK_FALLING,
};

enum cif_clk_dir_e {
	CIF_CLK_P02P1 = 0,
	CIF_CLK_P12P0,
	CIF_CLK_FREERUN,
};

void cif_set_base_addr(uint32_t link, void *mac_base, void *wrap_base);

/**
 * cif_init - setup cif
 *
 * @param :
 */
void cif_init(struct cif_ctx *ctx);

/**
 * cif_uninit - clear cif setting
 *
 * @param :
 */
void cif_uninit(struct cif_ctx *ctx);

/**
 * cif_reset - do reset. This can be activated only if dma stop to avoid
 * hang fabric.
 *
 */
void cif_reset(struct cif_ctx *ctx);

/**
 * cif_config - configuration cif.
 *
 */
void cif_config(struct cif_ctx *ctx, struct cif_param *param);


uintptr_t *cif_get_mac_phys_reg_bases(uint32_t link);
uintptr_t *cif_get_wrap_phys_reg_bases(uint32_t link);
void cif_set_lane_id(struct cif_ctx *ctx, enum lane_id_e lane,
			uint32_t select, uint32_t pn_swap);
void cif_hdr_enable(struct cif_ctx *ctx, struct cif_param *param, uint32_t on);
void cif_streaming(struct cif_ctx *ctx, uint32_t on, uint32_t hdr);
void cif_hdr_manual_config(struct cif_ctx *ctx,
			   struct cif_param *param,
			   uint32_t sw_up);
void cif_crop_info_line(struct cif_ctx *ctx, uint32_t line_num, uint32_t sw_up);
void cif_set_clk_edge(struct cif_ctx *ctx,
		      enum phy_lane_id_e lane, enum cif_clk_edge_e edge);
void cif_set_lvds_endian(struct cif_ctx *ctx,
			 enum cif_endian mac, enum cif_endian wrap);
void cif_set_lane_deskew(struct cif_ctx *ctx,
		      enum phy_lane_id_e lane, uint8_t phase);
uint8_t cif_get_lane_data(struct cif_ctx *ctx, enum phy_lane_id_e lane);
void cif_set_lvds_vsync_gen(struct cif_ctx *ctx, uint32_t fp);
int cif_check_csi_int_sts(struct cif_ctx *ctx, uint32_t mask);
void cif_mask_csi_int_sts(struct cif_ctx *ctx, uint32_t mask);
void cif_unmask_csi_int_sts(struct cif_ctx *ctx, uint32_t mask);
void cif_clear_csi_int_sts(struct cif_ctx *ctx);
int cif_check_csi_fifo_full(struct cif_ctx *ctx);
int cif_check_lvds_fifo_full(struct cif_ctx *ctx);
int cif_get_csi_decode_fmt(struct cif_ctx *ctx);
int cif_get_csi_phy_state(struct cif_ctx *ctx, union mipi_phy_state *state);
void cif_set_hs_settle(struct cif_ctx *ctx, uint8_t hs_settle);
void cif_set_bt_fmt_out(struct cif_ctx *ctx, enum ttl_bt_fmt_out fmt_out);
void cif_set_clk_dir(struct cif_ctx *ctx, enum cif_clk_dir_e dir);
void cif_set_ttl_pinmux(struct cif_ctx *ctx,
			enum ttl_vi_from_e vi, enum ttl_vi_func_e func, uint32_t pad);

#endif // _CIF_DRV_H_
