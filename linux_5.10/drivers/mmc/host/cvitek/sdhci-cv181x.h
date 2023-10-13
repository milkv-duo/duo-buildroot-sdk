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

#define TOP_BASE	0x03000000
#define OFFSET_SD_PWRSW_CTRL	0x1F4

#define PINMUX_BASE 0x03001000
#define CLKGEN_BASE 0x03002000


#define CVI_CV181X_SDHCI_VENDOR_OFFSET		0x200
#define CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R	(CVI_CV181X_SDHCI_VENDOR_OFFSET + 0x0)
#define CVI_CV181X_SDHCI_PHY_TX_RX_DLY		(CVI_CV181X_SDHCI_VENDOR_OFFSET + 0x40)
#define CVI_CV181X_SDHCI_PHY_DS_DLY			(CVI_CV181X_SDHCI_VENDOR_OFFSET + 0x44)
#define CVI_CV181X_SDHCI_PHY_DLY_STS		(CVI_CV181X_SDHCI_VENDOR_OFFSET + 0x48)
#define CVI_CV181X_SDHCI_PHY_CONFIG			(CVI_CV181X_SDHCI_VENDOR_OFFSET + 0x4C)

#define SDHCI_GPIO_CD_DEBOUNCE_TIME	10
#define SDHCI_GPIO_CD_DEBOUNCE_DELAY_TIME	200

#ifdef CONFIG_PM_SLEEP
#ifdef CONFIG_ARCH_CV181X_ASIC

#define RTC_CTRL_BASE		0x5025000
#define RTCSYS_CLKMUX	0x1C
#define RTCSYS_CLKBYP	0x30
#define RTCSYS_MCU51_ICTRL1	0x7C

#define RTCSYS_CTRL		0x248

struct cvi_rtc_sdhci_reg_context {
	u32 rtcsys_clkmux;
	u32 rtcsys_clkbyp;
	u32 rtcsys_mcu51_ictrl1;
	u32 rtcsys_ctrl;
};
#else
struct cvi_rtc_sdhci_reg_context {};
#endif
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
	struct delayed_work cd_debounce_work;
	spinlock_t cd_debounce_lock;
	int pre_gpio_cd;
	bool is_debounce_work_running;
#ifdef CONFIG_PM_SLEEP
	struct cvi_rtc_sdhci_reg_context *rtc_reg_ctx;
#endif

};
#endif
