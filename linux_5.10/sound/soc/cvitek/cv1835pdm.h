/* SPDX-License-Identifier: GPL-2.0-or-later
 * PDM driver on CVITEK CV1835
 *
 * Copyright 2018 CVITEK
 *
 * Author: EthanChen
 *
 */

#ifndef __CV1835PDM_H__
#define __CV1835PDM_H__

#include <linux/clk.h>
#include <linux/miscdevice.h>
/* register offset */
#define PDM_EN_REG	        0x30
#define PDM_EN				0x1
#define PDM_OFF				0x0

#define PDM_48K						3
#define PDM_44_1K					3
#define PDM_22_05K					7
#define PDM_16K						11
#define PDM_11_025K					15
#define PDM_8K						23

#define PDM_SETTING_REG		        0x34
#define PDM_CLK_DIV_MASK    0xFFFFFFE0
#define PDM_CLK_DIV(v)      (v << 0)
#define PDM_CLK_INV         (0x1 << 15)
#define PDM_DATA_CAP_MASK   0xFFC0FFFF
#define PDM_DATA_CAP(v)     (v << 16)
#define PDM_SEL_MASK        0xFEFFFFFF
#define PDM_BOND_SEL_0      (0 << 24)
#define PDM_BOND_SEL_1      (1 << 24)


#define I2S_48K						7
#define I2S_44_1K					7
#define I2S_22_05K					15
#define I2S_16K						23
#define I2S_11_025K					31
#define I2S_8K						47
#define I2S_SETTING_REG		        0x38
#define I2S_CLK_DIV_MASK            0xFFFFFF00
#define I2S_CLK_DIV(v)              (v << 0)
#define I2S_CHN_WIDTH_MASK          0xFFFFFCFF
#define I2S_CHN_WIDTH_8BIT          0x0
#define I2S_CHN_WIDTH_16BIT         0x1
#define I2S_CHN_WIDTH_24BIT         0x2
#define I2S_CHN_WIDTH_32BIT         0x3
#define I2S_CHN_WIDTH(v)            (v << 8)


struct cvi1835pdm {
	void __iomem *pdm_base;
	struct clk *clk;
	struct device *dev;
	struct miscdevice miscdev;
};

#endif  /* __CV1835PDM_H__ */
