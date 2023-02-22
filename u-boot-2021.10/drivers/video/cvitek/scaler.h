#ifndef _CVI_SCL_H_
#define _CVI_SCL_H_

#include <asm/gpio.h>

#define SCL_INTR_SCL_NUM 139
#define SCL_MAX_INST 4
#define SCL_MIN_WIDTH  32
#define SCL_MIN_HEIGHT 32
#define SCL_MAX_WIDTH  2688
#define SCL_MAX_HEIGHT 2688
#define SCL_MAX_DSI_LP 16
#define SCL_MAX_DSI_SP 2

struct sclr_size {
	u16 w;
	u16 h;
};

struct sclr_point {
	u16 x;
	u16 y;
};

struct sclr_rect {
	u16 x;
	u16 y;
	u16 w;
	u16 h;
};

struct sclr_status {
	u8 crop_idle : 1;
	u8 hscale_idle : 1;
	u8 vscale_idle : 1;
	u8 gop_idle : 1;
	u8 wdma_idle : 1;
	u8 rsv : 2;
};

enum sclr_img_in {
	SCL_IMG_V,  // for video-encoder
	SCL_IMG_D,  // for display
	SCL_IMG_MAX,
};

struct sclr_csc_matrix {
	u16 coef[3][3];
	u8 sub[3];
	u8 add[3];
};

union sclr_intr {
	struct {
		u32 disp_frame_start : 1;
		u32 disp_frame_end : 1;
		u32 img_in_d_frame_start : 1;
		u32 img_in_d_frame_end : 1;
		u32 img_in_v_frame_start : 1;
		u32 img_in_v_frame_end : 1;
		u32 scl0_frame_end : 1;
		u32 scl1_frame_end : 1;
		u32 scl2_frame_end : 1;
		u32 scl3_frame_end : 1;
		u32 prog_too_late : 1;
		u32 scl0_line_target_hit : 1;
		u32 scl1_line_target_hit : 1;
		u32 scl2_line_target_hit : 1;
		u32 scl3_line_target_hit : 1;
		u32 scl0_cycle_line_target_hit : 1;
		u32 scl1_cycle_line_target_hit : 1;
		u32 scl2_cycle_line_target_hit : 1;
		u32 scl3_cycle_line_target_hit : 1;
		u32 map_cv_frame_end : 1;
		u32 cmdq : 1;
		u32 cmdq_start : 1;
		u32 cmdq_end : 1;
		u32 cmdq_lint_hit : 1;
		u32 cmdq_cycle_line_hit : 1;
	} b;
	u32 raw;
};

enum sclr_format {
	SCL_FMT_YUV420,
	SCL_FMT_YUV422,
	SCL_FMT_RGB_PLANAR,
	SCL_FMT_BGR_PACKED, // B lsb
	SCL_FMT_RGB_PACKED, // R lsb
	SCL_FMT_Y_ONLY,
	SCL_FMT_MAX
};

enum sclr_csc {
	SCL_CSC_NONE,
	SCL_CSC_601_LIMIT_YUV2RGB,
	SCL_CSC_601_FULL_YUV2RGB,
	SCL_CSC_709_LIMIT_YUV2RGB,
	SCL_CSC_709_FULL_YUV2RGB,
	SCL_CSC_601_LIMIT_RGB2YUV,
	SCL_CSC_601_FULL_RGB2YUV,
	SCL_CSC_709_LIMIT_RGB2YUV,
	SCL_CSC_709_FULL_RGB2YUV,
	SCL_CSC_MAX,
};

enum sclr_gop_format {
	SCL_GOP_FMT_ARGB8888,
	SCL_GOP_FMT_ARGB4444,
	SCL_GOP_FMT_ARGB1555,
	SCL_GOP_FMT_256LUT,
	SCL_GOP_FMT_FONT,
	SCL_GOP_FMT_MAX
};

enum sclr_disp_drop_mode {
	SCL_DISP_DROP_MODE_DITHER = 1,
	SCL_DISP_DROP_MODE_ROUNDING = 2,
	SCL_DISP_DROP_MODE_DROP = 3,
	SCL_DISP_DROP_MODE_MAX,
};

enum sclr_img_trig_src {
	SCL_IMG_TRIG_SRC_SW = 0,
	SCL_IMG_TRIG_SRC_DISP,
	SCL_IMG_TRIG_SRC_ISP,
	SCL_IMG_TRIG_SRC_MAX,
};

struct sclr_top_cfg {
	bool ip_trig_src;    // 0(IMG_V), 1(IMG_D)
	bool sclr_enable[4];
	bool disp_enable;
	bool disp_from_sc;   // 0(DRAM), 1(SCL_D)
	bool sclr_d_src;     // 0(IMG_D), 1(IMG_V)
	enum sclr_img_trig_src img_in_d_trig_src;
	enum sclr_img_trig_src img_in_v_trig_src;
};

struct sclr_mem {
	u64 addr0;
	u64 addr1;
	u64 addr2;
	u16 pitch_y;
	u16 pitch_c;
	u16 start_x;
	u16 start_y;
	u16 width;
	u16 height;
};

struct sclr_gop_ow_cfg {
	enum sclr_gop_format fmt;
	struct sclr_point start;
	struct sclr_point end;
	u64 addr;
	u16 pitch;
	struct sclr_size mem_size;
	struct sclr_size img_size;
};

struct sclr_gop_cfg {
	union {
		struct {
			u16 ow0_en : 1;
			u16 ow1_en : 1;
			u16 ow2_en : 1;
			u16 ow3_en : 1;
			u16 ow4_en : 1;
			u16 ow5_en : 1;
			u16 ow6_en : 1;
			u16 ow7_en : 1;
			u16 hscl_en: 1;
			u16 vscl_en: 1;
			u16 colorkey_en : 1;
			u16 resv   : 1;
			u16 burst  : 4;
		} b;
		u16 raw;
	};
	u32 colorkey;       // RGB888
	u16 font_fg_color;  // ARGB4444
	u16 font_bg_color;  // ARGB4444
	struct sclr_gop_ow_cfg ow_cfg[8];
};

enum sclr_disp_pat_color {
	SCL_PAT_COLOR_WHITE,
	SCL_PAT_COLOR_RED,
	SCL_PAT_COLOR_GREEN,
	SCL_PAT_COLOR_BLUE,
	SCL_PAT_COLOR_CYAN,
	SCL_PAT_COLOR_MAGENTA,
	SCL_PAT_COLOR_YELLOW,
	SCL_PAT_COLOR_BAR,
	SCL_PAT_COLOR_USR,
	SCL_PAT_COLOR_MAX
};

enum sclr_disp_pat_type {
	SCL_PAT_TYPE_FULL,
	SCL_PAT_TYPE_H_GRAD,
	SCL_PAT_TYPE_V_GRAD,
	SCL_PAT_TYPE_AUTO,
	SCL_PAT_TYPE_SNOW,
	SCL_PAT_TYPE_OFF,
	SCL_PAT_TYPE_MAX
};

struct disp_ctrl_gpios {
	struct gpio_desc disp_reset_gpio;
	struct gpio_desc disp_pwm_gpio;
	struct gpio_desc disp_power_ct_gpio;

};

struct sclr_disp_cfg {
	bool disp_from_sc;  // 0(DRAM), 1(scaler_d)
	bool cache_mode;
	bool sync_ext;
	bool tgen_en;
	bool dw1_en;
	bool dw2_en;
	enum sclr_format fmt;
	enum sclr_csc in_csc;
	enum sclr_csc out_csc;
	u8 burst;       // 0~15
	u8 out_bit;     // 6/8/10-bit
	enum sclr_disp_drop_mode drop_mode;
	struct sclr_mem mem;
	struct sclr_gop_cfg gop_cfg;
	struct disp_ctrl_gpios ctrl_gpios;
};

/**
 * @ vsync_pol: vsync polarity
 * @ hsync_pol: hsync polarity
 * @ vtotal: total line of each frame, should sub 1,
 *           start line is included, end line isn't included
 * @ htotal: total pixel of each line, should sub 1,
 *           start pixel is included, end pixel isn't included
 * @ vsync_start: start line of vsync
 * @ vsync_end: end line of vsync, should sub 1
 * @ vfde_start: start line of actually video data
 * @ vfde_end: end line of actually video data, should sub 1
 * @ vmde_start: equal to vfde_start
 * @ vmde_end: equal to vfde_end
 * @ hsync_start: start pixel of hsync
 * @ hsync_end: end pixel of hsync, should sub 1
 * @ hfde_start: start pixel of actually video data in each line
 * @ hfde_end: end pixel of actually video data in each line, should sub 1
 * @ hmde_start: equal to hfde_start
 * @ hmde_end: equal to hfde_end
 */
struct sclr_disp_timing {
	bool vsync_pol;
	bool hsync_pol;
	u16 vtotal;
	u16 htotal;
	u16 vsync_start;
	u16 vsync_end;
	u16 vfde_start;
	u16 vfde_end;
	u16 vmde_start;
	u16 vmde_end;
	u16 hsync_start;
	u16 hsync_end;
	u16 hfde_start;
	u16 hfde_end;
	u16 hmde_start;
	u16 hmde_end;
};

union sclr_disp_dbg_status {
	struct {
		u32 bw_fail     : 1;
		u32 bw_fail_clr : 1;
		u32 resv0       : 2;
		u32 err_fwr_y   : 1;
		u32 err_fwr_u   : 1;
		u32 err_fwr_v   : 1;
		u32 err_fwr_clr : 1;
		u32 err_erd_y   : 1;
		u32 err_erd_u   : 1;
		u32 err_erd_v   : 1;
		u32 err_erd_clr : 1;
		u32 lb_full_y   : 1;
		u32 lb_full_u   : 1;
		u32 lb_full_v   : 1;
		u32 resv1       : 1;
		u32 lb_empty_y  : 1;
		u32 lb_empty_u  : 1;
		u32 lb_empty_v  : 1;
		u32 resv2       : 13;
	} b;
	u32 raw;
};

/**
 * @ out_bit: 0(6-bit), 1(8-bit), others(10-bit)
 * @ vesa_mode: 0(JEIDA), 1(VESA)
 * @ dual_ch: dual link
 * @ vs_out_en: vs output enable
 * @ hs_out_en: hs output enable
 * @ hs_blk_en: vertical blanking hs output enable
 * @ ml_swap: lvdstx hs data msb/lsb swap
 * @ ctrl_rev: serializer 0(msb first), 1(lsb first)
 * @ oe_swap: lvdstx even/odd link swap
 * @ en: lvdstx enable
 */
union sclr_lvdstx {
	struct {
		u32 out_bit	: 2;
		u32 vesa_mode	: 1;
		u32 dual_ch	: 1;
		u32 vs_out_en	: 1;
		u32 hs_out_en	: 1;
		u32 hs_blk_en	: 1;
		u32 resv_1	: 1;
		u32 ml_swap	: 1;
		u32 ctrl_rev	: 1;
		u32 oe_swap	: 1;
		u32 en		: 1;
		u32 resv	: 20;
	} b;
	u32 raw;
};

/**
 * @fmt_sel: [0] clk select
 *		0: bt clock 2x of disp clock
 *		1: bt clock 2x of disp clock
 *	     [1] sync signal index
 *		0: with sync pattern
 *		1: without sync pattern
 * @hde_gate: gate output hde with vde
 * @data_seq: fmt_sel[0] = 0
 *		00: Cb0Y0Cr0Y1
 *		01: Cr0Y0Cb0Y1
 *		10: Y0Cb0Y1Cr0
 *		11: Y0Cr0Y1Cb0
 *	      fmt_sel[0] = 1
 *		0: Cb0Cr0
 *		1: Cr0Cb0
 * @clk_inv: clock rising edge at middle of data
 * @vs_inv: vs low active
 * @hs_inv: hs low active
 */
union sclr_bt_enc {
	struct {
		u32 fmt_sel	: 2;
		u32 resv_1	: 1;
		u32 hde_gate	: 1;
		u32 data_seq	: 2;
		u32 resv_2	: 2;
		u32 clk_inv	: 1;
		u32 hs_inv	: 1;
		u32 vs_inv	: 1;
	} b;
	u32 raw;
};

/**
 * @ sav_vld: sync pattern for start of valid data
 * @ sav_blk: sync pattern for start of blanking data
 * @ eav_vld: sync pattern for end of valid data
 * @ eav_blk: sync pattern for end of blanking data
 */
union sclr_bt_sync_code {
	struct {
		u8 sav_vld;
		u8 sav_blk;
		u8 eav_vld;
		u8 eav_blk;
	} b;
	u32 raw;
};

enum sclr_vo_sel {
	SCLR_VO_SEL_DISABLE,
	SCLR_VO_SEL_SW = 2,
	SCLR_VO_SEL_I80,
	SCLR_VO_SEL_BT601,
	SCLR_VO_SEL_BT656,
	SCLR_VO_SEL_BT1120,
	SCLR_VO_SEL_BT1120R,
	SCLR_VO_SEL_MAX,
};

enum sclr_vo_intf {
	SCLR_VO_INTF_DISABLE,
	SCLR_VO_INTF_SW,
	SCLR_VO_INTF_I80,
	SCLR_VO_INTF_BT601,
	SCLR_VO_INTF_BT656,
	SCLR_VO_INTF_BT1120,
	SCLR_VO_INTF_MIPI,
	SCLR_VO_INTF_LVDS,
	SCLR_VO_INTF_MAX,
};

enum sclr_dsi_mode {
	SCLR_DSI_MODE_IDLE = 0,
	SCLR_DSI_MODE_SPKT = 1,
	SCLR_DSI_MODE_ESC = 2,
	SCLR_DSI_MODE_HS = 4,
	SCLR_DSI_MODE_UNKNOWN,
	SCLR_DSI_MODE_MAX = SCLR_DSI_MODE_UNKNOWN,
};

enum sclr_dsi_fmt {
	SCLR_DSI_FMT_RGB888 = 0,
	SCLR_DSI_FMT_RGB666,
	SCLR_DSI_FMT_RGB565,
	SCLR_DSI_FMT_RGB101010,
	SCLR_DSI_FMT_MAX,
};

enum sclr_i80_mode {
	SCLR_I80_MODE_IDLE = 0,
	SCLR_I80_MODE_SW = 1,
};

void sclr_set_base_addr(void *base);
void sclr_top_set_cfg(struct sclr_top_cfg *cfg);
struct sclr_top_cfg *sclr_top_get_cfg(void);
void sclr_top_reg_done(void);
void sclr_top_reg_force_up(void);
u8 sclr_top_pg_late_get_bus(void);
void sclr_top_pg_late_clr(void);

void sclr_img_set_trig(u8 inst, enum sclr_img_trig_src trig_src);

void sclr_disp_set_cfg(struct sclr_disp_cfg *cfg);
struct sclr_disp_cfg *sclr_disp_get_cfg(void);
void sclr_disp_set_timing(struct sclr_disp_timing *timing);
struct sclr_disp_timing *sclr_disp_get_timing(void);
int sclr_disp_set_rect(struct sclr_rect rect);
void sclr_disp_set_mem(struct sclr_mem *mem);
void sclr_disp_set_addr(u64 addr0, u64 addr1, u64 addr2);
void sclr_disp_set_csc(struct sclr_csc_matrix *cfg);
void sclr_disp_set_in_csc(enum sclr_csc csc);
void sclr_disp_set_out_csc(enum sclr_csc csc);
void sclr_disp_set_pattern(enum sclr_disp_pat_type type,
			   enum sclr_disp_pat_color color, const u16 *rgb);
void sclr_disp_set_frame_bgcolor(u16 r, u16 g, u16 b);
void sclr_disp_set_window_bgcolor(u16 r, u16 g, u16 b);
void sclr_disp_enable_window_bgcolor(bool enable);
void sclr_disp_reg_force_up(void);
bool sclr_disp_tgen_enable(bool enable);
union sclr_disp_dbg_status sclr_disp_get_dbg_status(bool clr);
void sclr_disp_gamma_ctrl(bool enable, bool pre_osd);
void sclr_disp_gamma_lut_update(const u8 *b, const u8 *g, const u8 *r);
void sclr_lvdstx_set(union sclr_lvdstx cfg);
void sclr_lvdstx_get(union sclr_lvdstx *cfg);
void sclr_bt_set(union sclr_bt_enc enc, union sclr_bt_sync_code sync);
void sclr_bt_get(union sclr_bt_enc *enc, union sclr_bt_sync_code *sync);
void sclr_disp_set_intf(enum sclr_vo_intf intf);

enum sclr_dsi_mode sclr_dsi_get_mode(void);
int sclr_dsi_set_mode(enum sclr_dsi_mode mode);
void sclr_dsi_clr_mode(void);
int sclr_dsi_chk_mode_done(enum sclr_dsi_mode mode);
int sclr_dsi_long_packet(u8 di, const u8 *data, u8 count, bool sw_mode);
int sclr_dsi_long_packet_raw(const u8 *data, u8 count);
int sclr_dsi_short_packet(u8 di, const u8 *data, u8 count, bool sw_mode);
int sclr_dsi_dcs_write_buffer(u8 di, const void *data, size_t len, bool sw_mode);
int sclr_dsi_dcs_read_buffer(u8 di, const u16 data_param, u8 *data, size_t len, bool sw_mode);
int sclr_dsi_config(u8 lane_num, enum sclr_dsi_fmt fmt, u16 width);
void sclr_i80_sw_mode(bool enable);
void sclr_i80_packet(u32 cmd);
void sclr_i80_run(void);

void sclr_gop_set_cfg(u8 inst, struct sclr_gop_cfg *cfg);
struct sclr_gop_cfg *sclr_gop_get_cfg(u8 inst);
void sclr_gop_ow_set_cfg(u8 inst, u8 ow_inst, struct sclr_gop_ow_cfg *cfg);
void sclr_gop_setup_LUT(u8 inst, u16 *data);
int sclr_gop_update_LUT(u8 inst, u8 index, u16 data);

void sclr_ctrl_init(void);
int sclr_ctrl_set_disp_src(bool disp_from_sc);
void sclr_intr_clr(union sclr_intr intr_mask);
union sclr_intr sclr_intr_status(void);

int set_disp_ctrl_gpios(struct disp_ctrl_gpios *ctrl_gpios);
int get_disp_ctrl_gpios(struct disp_ctrl_gpios *ctrl_gpios);

#endif  //_CVI_SCL_H_
