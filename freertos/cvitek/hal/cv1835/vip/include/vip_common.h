#ifndef _COMMON_VIP_H_
#define _COMMON_VIP_H_

#include "reg_vip_sys.h"

#define BASE_OFFSET 0x000000000
#define GOP_FRAME_OFFSET 0x200000

#define VIP_ALIGNMENT 0x20
#define MIN(a, b) (((a) < (b))?(a):(b))
#define MAX(a, b) (((a) > (b))?(a):(b))
#define CLIP(x, min, max) MAX(MIN(x, max), min)
#define VIP_ALIGN(x) (((x) + 0x1F) & ~0x1F)   // for 32byte alignment
#define UPPER(x, y) (((x) + ((1 << (y)) - 1)) >> (y))   // for alignment
#define CEIL(x, y) (((x) + ((1 << (y)))) >> (y))   // for alignment

#define R_IDX 0
#define G_IDX 1
#define B_IDX 2

#define VIP_CLK_RATIO_MASK(CLK_NAME) VIP_SYS_REG_CK_COEF_##CLK_NAME##_MASK
#define VIP_CLK_RATIO_OFFSET(CLK_NAME) VIP_SYS_REG_CK_COEF_##CLK_NAME##_OFFSET
#define VIP_CLK_RATIO_CONFIG(CLK_NAME, RATIO) \
	vip_sys_reg_write_mask(VIP_SYS_REG_CK_COEF_##CLK_NAME, \
		VIP_CLK_RATIO_MASK(CLK_NAME), \
		RATIO << VIP_CLK_RATIO_OFFSET(CLK_NAME))
#define VIP_CLK_RATIO_VALUE(CLK_NAME) \
	((vip_sys_reg_read(VIP_SYS_REG_CK_COEF_##CLK_NAME) & VIP_CLK_RATIO_MASK(CLK_NAME)) \
		>> VIP_CLK_RATIO_OFFSET(CLK_NAME))

#define VIP_RST_MASK(MOD_NAME) VIP_SYS_REG_RST_##MOD_NAME##_MASK
#define VIP_RST_OFFSET(MOD_NAME) VIP_SYS_REG_RST_##MOD_NAME##_OFFSET
#define VIP_RST_CONFIG(MOD_NAME, VALUE) \
	vip_sys_reg_write_mask(VIP_SYS_REG_RST_##MOD_NAME, \
		VIP_RST_MASK(MOD_NAME), \
		VALUE << VIP_RST_OFFSET(MOD_NAME))
#define VIP_RST_VALUE(MOD_NAME) \
	((vip_sys_reg_read(VIP_SYS_REG_RST_##MOD_NAME) & VIP_RST_MASK(MOD_NAME)) \
		>> VIP_RST_OFFSET(MOD_NAME))

struct vip_rect {
	u16 x;
	u16 y;
	u16 w;
	u16 h;
};

union vip_sys_reg0 {
	struct {
		u32 rst_axi : 1;
		u32 rst_isp_top : 1;
		u32 rst_img_d : 1;
		u32 rst_img_v : 1;
		u32 rst_sc_top : 1;
		u32 rst_sc_d : 1;
		u32 rst_sc_v1 : 1;
		u32 rst_sc_v2 : 1;
		u32 rst_sc_v3 : 1;
		u32 rst_disp : 1;
		u32 rst_bt : 1;
		u32 rst_dsi_mac : 1;
		u32 resv_12 : 2;
		u32 rst_dwa : 1;
		u32 rst_clk_div : 1;
		u32 rsv_16 : 2;
		u32 clk_isp_top_en : 1;
		u32 clk_axi_isp_en : 1;
		u32 clk_csi_mac0_en : 1;
		u32 clk_csi_mac1_en : 1;
		u32 rsv_22 : 10;
	} b;
	u32 raw;
};

union vip_sys_reg4 {
	struct {
		u32 rst_csi_mac0 : 1;
		u32 rst_csi_mac1 : 1;
		u32 rsv_2 : 30;
	} b;
	u32 raw;
};

union vip_sys_reset {
	struct {
		u32 axi : 1;
		u32 isp_top : 1;
		u32 img_d : 1;
		u32 img_v : 1;
		u32 sc_top : 1;
		u32 sc_d : 1;
		u32 sc_v1 : 1;
		u32 sc_v2 : 1;
		u32 sc_v3 : 1;
		u32 disp : 1;
		u32 bt : 1;
		u32 dsi_mac : 1;
		u32 csi_mac0 : 1;
		u32 csi_mac1 : 1;
		u32 dwa : 1;
		u32 clk_div : 1;
		u32 rsv : 16;
	} b;
	u32 raw;
};

union vip_sys_isp_clk {
	struct {
		u32 rsv : 18;
		u32 clk_isp_top_en : 1;
		u32 clk_axi_isp_en : 1;
		u32 clk_csi_mac0_en : 1;
		u32 clk_csi_mac1_en : 1;
	} b;
	u32 raw;
};

union vip_sys_clk {
	struct {
		u32 sc_top : 1;
		u32 isp_top : 1;
		u32 dwa : 1;
		u32 rot_top : 1;
		u32 vip_sys : 1;
		u32 rsv1 : 3;
		u32 csi_mac0 : 1;
		u32 csi_mac1 : 1;
		u32 rsv2 : 6;
		u32 auto_sc_top : 1;
		u32 auto_isp_top : 1;
		u32 auto_dwa : 1;
		u32 auto_rot_top : 1;
		u32 auto_vip_sys : 1;
		u32 rsv3 : 3;
		u32 auto_csi_mac0 : 1;
		u32 auto_csi_mac1 : 1;
	} b;
	u32 raw;
};

union vip_sys_intr {
	struct {
		u32 sc : 1;
		u32 rsv1 : 15;
		u32 isp : 1;
		u32 rsv2 : 7;
		u32 dwa : 1;
		u32 rsv3 : 3;
		u32 rot : 1;
		u32 csi_mac0 : 1;
		u32 csi_mac1 : 1;
	} b;
	u32 raw;
};

enum vip_sys_axi_bus {
	VIP_SYS_AXI_BUS_SC_TOP = 0,
	VIP_SYS_AXI_BUS_ISP_RAW,
	VIP_SYS_AXI_BUS_ISP_YUV,
	VIP_SYS_AXI_BUS_MAX,
};

struct sc_cfg_cb {
	struct cvi_vip_dev *dev;
	u8  snr_num;
};

/********************************************************************
 *   APIs to replace bmtest's standard APIs
 ********************************************************************/
void vip_set_base_addr(void *base);
union vip_sys_clk vip_get_clk_lp(void);
void vip_set_clk_lp(union vip_sys_clk clk);
union vip_sys_isp_clk vip_get_isp_clk(void);
void vip_set_isp_clk(union vip_sys_isp_clk clk);
union vip_sys_reset vip_get_reset(void);
void vip_set_reset(union vip_sys_reset reset);
void vip_toggle_reset(union vip_sys_reset mask);
union vip_sys_intr vip_get_intr_status(void);
union vip_sys_intr vip_get_intr_mask(void);
void vip_set_intr_mask(union vip_sys_intr intr);
void vip_isp_clk_reset(void);
void vip_sys_reg_read(uintptr_t addr);
void vip_sys_reg_write_mask(uintptr_t addr, u32 mask, u32 data);
void vip_sys_set_offline(enum vip_sys_axi_bus bus, bool offline);
//int vip_sys_register_cmm_cb(unsigned long cmm, void *hdlr, void *cb);
//int vip_sys_cmm_cb_i2c(unsigned int cmd, void *arg);
//int vip_sys_cmm_cb_ssp(unsigned int cmd, void *arg);
//int vip_sys_register_cif_cb(void *hdlr, void *cb);
//int vip_sys_cif_cb(unsigned int cmd, void *arg);
//u32 cvi_sc_cfg_cb(struct sc_cfg_cb *post_para);
#endif
