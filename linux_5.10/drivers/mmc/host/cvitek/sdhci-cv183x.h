/*
 * drivers/mmc/host/sdhci-cvi.c - CVITEK SDHCI Platform driver
 *
 * Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __SDHCI_CV_H
#define __SDHCI_CV_H

#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/delay.h>
#include <linux/mmc/mmc.h>
#include <linux/slab.h>

#define MAX_TUNING_CMD_RETRY_COUNT 50
#define TUNE_MAX_PHCODE	128
#define TAP_WINDOW_THLD 20
#define DISPPLL_MHZ 1188
#define FPLL_MHZ 1500

#define TOP_BASE	0x3000000
#define OFFSET_SD_PWRSW_CTRL	0x1F4

#define PINMUX_BASE 0x03001000
#define CLKGEN_BASE 0x03002000

#define CVI_183X_SDHCI_VENDOR_OFFSET		0x500
#define CVI_183X_SDHCI_VENDOR_MSHC_CTRL_R	(CVI_183X_SDHCI_VENDOR_OFFSET + 0x08)
#define CVI_183X_SDHCI_VENDOR_A_CTRL_R		(CVI_183X_SDHCI_VENDOR_OFFSET + 0x40)
#define CVI_183X_SDHCI_VENDOR_A_STAT_R		(CVI_183X_SDHCI_VENDOR_OFFSET + 0x44)

/* PHY register */
#define SDHCI_PHY_R_OFFSET			0x300

#define SDHCI_P_PHY_CNFG           (SDHCI_PHY_R_OFFSET + 0x00)
#define SDHCI_RX_DELAY_LINE        (SDHCI_PHY_R_OFFSET + 0x0C)
#define SDHCI_TX_DELAY_LINE        (SDHCI_PHY_R_OFFSET + 0x1C)
#define SDHCI_P_SDCLKDL_DC         (SDHCI_PHY_R_OFFSET + 0x1E)
#define SDHCI_LEAD_LAG_FLAG        (SDHCI_PHY_R_OFFSET + 0x12)

#define PHY_CNFG_PHY_RSTN			0
#define PHY_CNFG_PHY_PWRGOOD		1
#define PHY_CNFG_PAD_SP				16
#define PHY_CNFG_PAD_SP_MSK			0xf
#define PHY_CNFG_PAD_SN				20
#define PHY_CNFG_PAD_SN_MSK			0xf

#define SDHCI_GPIO_CD_DEBOUNCE_TIME	15
#define SDHCI_GPIO_CD_DEBOUNCE_DELAY_TIME	200

#ifdef CONFIG_PM_SLEEP
struct cvi_rtc_sdhci_reg_context {};
#endif

struct sdhci_cvi_host {
	struct sdhci_host *host;
	struct platform_device *pdev;
	void __iomem *core_mem; /* mmio address */
	struct clk *clk;    /* main SD/MMC bus clock */
	struct clk *clk100k;
	struct clk *clkaxi;
	struct mmc_host *mmc;
	struct reset_control *reset;

	struct reset_control *clk_rst_axi_emmc_ctrl;
	struct reset_control *clk_rst_emmc_ctrl;
	struct reset_control *clk_rst_100k_emmc_ctrl;

	int gpio_card_cd;
	int gpio_card_cd_active;

	void __iomem *topbase;
	void __iomem *pinmuxbase;
	void __iomem *clkgenbase;

	u32 reg_ctrl2;
	u32 reg_clk_ctrl;
	u32 reg_host_ctrl;
	u8 final_tap;
	u8 sdio0_voltage_1_8_v;
	int sd_save_count;
	struct mmc_gpio *cvi_gpio;
#ifdef CONFIG_PM_SLEEP
	struct cvi_rtc_sdhci_reg_context *rtc_reg_ctx;
#endif

};
#endif
