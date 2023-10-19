#ifndef __VIP_SYS_H__
#define __VIP_SYS_H__

#include <stddef.h>
#include <stdint.h>

#include "mmio.h"
#include "reg_vip_sys.h"

/* VIP_SYS */

#include "mmio.h"

#define CV181X_SYS_CLOCK_BASE		0x03002000
#define REG_DIV_CLK_SRC_VIP_SYS_2       0x110

#define VIP_SYS_2_SRC_DISPPLL		(2 << 8) // 1188MHz
#define VIP_SYS_2_SRC_FPLL		(3 << 8) // 1500MHz
#define VIP_SYS_2_SRC_MASK		(3 << 8) // [8:9]

#define SET_VIP_SYS_2_CLK_DIV(s, v) \
	_reg_write(CV181X_SYS_CLOCK_BASE + REG_DIV_CLK_SRC_VIP_SYS_2, \
	(s == VIP_SYS_2_SRC_DISPPLL) ? ((v << 16) | 0x209) : ((v << 16) | 0x309))

#define VIP_SYS_BASE		0x0A0C8000

#define vip_sys_reg_write_mask(addr, mask, data) \
	mmio_clrsetbits_32((VIP_SYS_BASE + addr), mask, data)

#define VIP_NORM_CLK_RATIO_MASK(CLK_NAME) VIP_SYS_REG_NORM_DIV_##CLK_NAME##_MASK
#define VIP_NORM_CLK_RATIO_OFFSET(CLK_NAME) VIP_SYS_REG_NORM_DIV_##CLK_NAME##_OFFSET
#define VIP_NORM_CLK_RATIO_CONFIG(CLK_NAME, RATIO) \
	vip_sys_reg_write_mask(VIP_SYS_REG_NORM_DIV_##CLK_NAME, \
		VIP_NORM_CLK_RATIO_MASK(CLK_NAME), \
		RATIO << VIP_NORM_CLK_RATIO_OFFSET(CLK_NAME))

#define VIP_UPDATE_CLK_RATIO_MASK(CLK_NAME) VIP_SYS_REG_UPDATE_##CLK_NAME##_MASK
#define VIP_UPDATE_CLK_RATIO_OFFSET(CLK_NAME) VIP_SYS_REG_UPDATE_##CLK_NAME##_OFFSET
#define VIP_UPDATE_CLK_RATIO(CLK_NAME) \
	vip_sys_reg_write_mask(VIP_SYS_REG_UPDATE_##CLK_NAME, \
		VIP_UPDATE_CLK_RATIO_MASK(CLK_NAME), \
		1 << VIP_UPDATE_CLK_RATIO_OFFSET(CLK_NAME))

static void vip_toggle_reset(uint32_t mask)
{
	uint32_t reset;
	uint32_t reset_base = VIP_SYS_BASE + VIP_SYS_VIP_RESETS;

	reset = _reg_read(reset_base);

	reset |= mask;
	_reg_write(reset_base, reset);

	udelay(20);

	reset &= ~mask;
	_reg_write(reset_base, reset);
}

#endif
