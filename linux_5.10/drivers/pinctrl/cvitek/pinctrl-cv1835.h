/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: pinctrl-cv1835.h
 * Description:
 */

#ifndef __PINCTRL_CV1835_H__
#define __PINCTRL_CV1835_H__

#include "../core.h"
#include "cv1835_pinlist_swconfig.h"
#include "cv1835_reg_fmux_gpio.h"

#define  PAD_MIPI_TXM4__MIPI_TXM4 0
#define  PAD_MIPI_TXP4__MIPI_TXP4 0
#define  PAD_MIPI_TXM3__MIPI_TXM3 0
#define  PAD_MIPI_TXP3__MIPI_TXP3 0
#define  PAD_MIPI_TXM2__MIPI_TXM2 0
#define  PAD_MIPI_TXP2__MIPI_TXP2 0
#define  PAD_MIPI_TXM1__MIPI_TXM1 0
#define  PAD_MIPI_TXP1__MIPI_TXP1 0
#define  PAD_MIPI_TXM0__MIPI_TXM0 0
#define  PAD_MIPI_TXP0__MIPI_TXP0 0

#define PINMUX_BASE 0x03001000
#define PINMUX_MASK(PIN_NAME) fmux_gpio_funcsel_##PIN_NAME##_MASK
#define PINMUX_OFFSET(PIN_NAME) fmux_gpio_funcsel_##PIN_NAME##_OFFSET
#define PINMUX_VALUE(PIN_NAME, FUNC_NAME) PIN_NAME##__##FUNC_NAME
#define PINMUX_CONFIG(PIN_NAME, FUNC_NAME) \
		mmio_clrsetbits_32(PINMUX_BASE + fmux_gpio_funcsel_##PIN_NAME, \
		PINMUX_MASK(PIN_NAME) << PINMUX_OFFSET(PIN_NAME), \
		PINMUX_VALUE(PIN_NAME, FUNC_NAME))

static inline void mmio_clrsetbits_32(uintptr_t addr,
				      uint32_t clear,
				      uint32_t set)
{
	void __iomem *tpreg;

	tpreg = ioremap(addr, 0x4);
	if (IS_ERR(tpreg)) {
		pr_err("ioremap %p failed\n", (void *)addr);
		return;
	}

	iowrite32((ioread32(tpreg) & ~clear) | set, tpreg);

	iounmap(tpreg);
}

#endif /* __PINCTRL_CV1835_H__ */
