// SPDX-License-Identifier: GPL-2.0
/* cvitek.c - cvitek machine
 *
 * Copyright (c) 2019 Cvitek Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/system_info.h>

#define CVI_PLAT_GP_REG_BASE 0x3000080
#define CVI_PLAT_GP_REG3 0xc

static void __init cv182x_init_late(void)
{
	void __iomem *plat_info = ioremap(CVI_PLAT_GP_REG_BASE, SZ_4K);

	system_rev = __raw_readw(plat_info + CVI_PLAT_GP_REG3);
	iounmap(plat_info);
}

static const char *const cv182x_compat[] __initconst = {
	"cvitek,cv182x",
	NULL,
};

DT_MACHINE_START(cv182x, "Cvitek cv182x (Flattened Device Tree)")
	.dt_compat	= cv182x_compat,
	.init_late	= cv182x_init_late,
MACHINE_END
