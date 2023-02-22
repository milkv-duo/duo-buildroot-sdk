#ifndef _COMMON_VIP_H_
#define _COMMON_VIP_H_

#include <linux/kernel.h>
#include "reg_vip_sys.h"

#define BASE_OFFSET 0x000000000
#define GOP_FRAME_OFFSET 0x200000

#define CLIP(x, _min, _max) max(min(x, _max), _min)
#define VIP_ALIGN(x) (((x) + 0x1F) & ~0x1F)   // for 32byte alignment

#define R_IDX 0
#define G_IDX 1
#define B_IDX 2

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

#endif
