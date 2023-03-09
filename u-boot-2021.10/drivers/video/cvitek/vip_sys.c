#include <common.h>
#include <linux/delay.h>
#include "vip_common.h"
#include "reg.h"

static uintptr_t reg_base;

void vip_set_base_addr(void *base)
{
	reg_base = (uintptr_t)base;
}

union vip_sys_isp_clk vip_get_isp_clk(void)
{
	union vip_sys_reg0 reg0;
	union vip_sys_isp_clk clk;

	reg0.raw = _reg_read(reg_base + VIP_SYS_VIP_RESETS);
	clk.raw = 0;
	clk.b.clk_isp_top_en = reg0.b.clk_isp_top_en;
	clk.b.clk_axi_isp_en = reg0.b.clk_axi_isp_en;
	clk.b.clk_csi_mac0_en = reg0.b.clk_csi_mac0_en;
	clk.b.clk_csi_mac1_en = reg0.b.clk_csi_mac1_en;

	return clk;
}

void vip_set_isp_clk(union vip_sys_isp_clk clk)
{
	union vip_sys_reg0 reg0;

	reg0.raw = _reg_read(reg_base + VIP_SYS_VIP_RESETS);
	reg0.b.clk_isp_top_en = clk.b.clk_isp_top_en;
	reg0.b.clk_axi_isp_en = clk.b.clk_axi_isp_en;
	reg0.b.clk_csi_mac0_en = clk.b.clk_csi_mac0_en;
	reg0.b.clk_csi_mac1_en = clk.b.clk_csi_mac1_en;
	_reg_write(reg_base + VIP_SYS_VIP_RESETS, reg0.raw);
}

union vip_sys_clk vip_get_clk_lp(void)
{
	union vip_sys_clk clk;

	clk.raw = _reg_read(reg_base + VIP_SYS_VIP_CLK_LP);
	return clk;
}

void vip_set_clk_lp(union vip_sys_clk clk)
{
	_reg_write(reg_base + VIP_SYS_VIP_CLK_LP, clk.raw);
}

union vip_sys_reset vip_get_reset(void)
{
	union vip_sys_reg0 reg0;
	union vip_sys_reg4 reg4;
	union vip_sys_reset reset;

	reg0.raw = _reg_read(reg_base + VIP_SYS_VIP_RESETS);
	reg4.raw = _reg_read(reg_base + VIP_SYS_VIP_ENABLE);
	reset.raw = reg0.raw & 0x0000cfff;
	reset.raw |= (reg4.raw & 0x03) << 12;
	return reset;
}

void vip_set_reset(union vip_sys_reset reset)
{
	union vip_sys_reg0 reg0;
	union vip_sys_reg4 reg4;

	reg0.raw = _reg_read(reg_base + VIP_SYS_VIP_RESETS);
	reg0.raw &= 0xffff0000;
	reg0.raw |= (reset.raw & 0x0000cfff);
	_reg_write(reg_base + VIP_SYS_VIP_RESETS, reg0.raw);

	reg4.raw = (reset.raw & 0x00003000) >> 12;
	_reg_write(reg_base + VIP_SYS_VIP_ENABLE, reg4.raw);
}

/**
 * vip_toggle_reset - enable/disable reset specified in mask. Lock protected.
 *
 * @param mask: resets want to be toggled.
 */
void vip_toggle_reset(union vip_sys_reset mask)
{
	union vip_sys_reset value;

	value = vip_get_reset();
	value.raw |= mask.raw;
	vip_set_reset(value);

	udelay(20);
	value.raw &= ~mask.raw;
	vip_set_reset(value);
}

union vip_sys_intr vip_get_intr_status(void)
{
	union vip_sys_intr intr;

	intr.raw = _reg_read(reg_base + VIP_SYS_VIP_INT);
	return intr;
}

union vip_sys_intr vip_get_intr_mask(void)
{
	union vip_sys_intr intr;

	intr.raw = _reg_read(reg_base + VIP_SYS_VIP_INT_EN);
	return intr;
}

void vip_set_intr_mask(union vip_sys_intr intr)
{
	_reg_write(reg_base + VIP_SYS_VIP_INT_EN, intr.raw);
}

void vip_isp_clk_reset(void)
{
	union vip_sys_reset reset;
	union vip_sys_isp_clk clk;

	memset(&reset, 0, sizeof(reset));
	reset.b.axi = 1;
	reset.b.isp_top = 1;
	reset.b.csi_mac0 = 1;
	reset.b.csi_mac1 = 1;
	vip_set_reset(reset);

	// enable all clk
	clk.raw = 0xffffffff;
	vip_set_isp_clk(clk);
	// disable all clk
	clk.raw = 0x0;
	vip_set_isp_clk(clk);

	// release reset clk-axi
	clk.b.clk_axi_isp_en = 1;
	vip_set_isp_clk(clk);
	reset.b.axi = 0;
	vip_set_reset(reset);
	clk.raw = 0x0;
	vip_set_isp_clk(clk);

	// release reset clk-isp
	clk.b.clk_isp_top_en = 1;
	vip_set_isp_clk(clk);
	reset.b.isp_top = 0;
	vip_set_reset(reset);
	clk.raw = 0x0;
	vip_set_isp_clk(clk);

	// release reset clk-mac
	clk.b.clk_csi_mac0_en = 1;
	clk.b.clk_csi_mac1_en = 1;
	vip_set_isp_clk(clk);
	reset.b.csi_mac0 = 0;
	reset.b.csi_mac1 = 0;
	vip_set_reset(reset);
	clk.raw = 0x0;
	vip_set_isp_clk(clk);

	// enable all clk
	clk.raw = 0xffffffff;
	vip_set_isp_clk(clk);

	//iowrite32((ioread32(ioremap(0x03002030, 0x4)) & 0xfc1fffff),  ioremap(0x03002030, 0x4));
	_reg_write_mask(0x03002030, 0xfc1fffff, 0);
}

void vip_sys_reg_read(uintptr_t addr)
{
	_reg_read(reg_base + addr);
}

void vip_sys_reg_write_mask(uintptr_t addr, u32 mask, u32 data)
{
	_reg_write_mask(reg_base + addr, mask, data);
}
