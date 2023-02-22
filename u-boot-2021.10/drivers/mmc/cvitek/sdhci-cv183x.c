/*
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/sizes.h>
#include <linux/libfdt.h>
#include <reset.h>
#include <mmc.h>
#include <sdhci.h>

#ifdef DEBUG
#define pr_debug(fmt, ...) \
	printf(fmt, ##__VA_ARGS__)
#endif

struct cvi_sdhci_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

struct cvi_sdhci_host {
	struct sdhci_host host;
	int pll_index;
	int pll_reg;
	int has_phy;
	int no_1_8_v;
	int is_64_addressing;
	int reset_tx_rx_phy;
	uint32_t mmc_init_freq;
	uint32_t mmc_trans_freq;
	struct reset_ctl reset_ctl;
};

static void cvi_emmc_pad_setting(void)
{
	mmio_clrsetbits_32(REG_EMMC_CLK_PAD_REG, REG_EMMC_PAD_CLR_MASK, REG_EMMC_CLK_PAD_VALUE << REG_EMMC_PAD_SHIFT);

	mmio_clrsetbits_32(REG_EMMC_RSTN_PAD_REG, REG_EMMC_PAD_CLR_MASK, REG_EMMC_RSTN_PAD_VALUE << REG_EMMC_PAD_SHIFT);

	mmio_clrsetbits_32(REG_EMMC_CMD_PAD_REG, REG_EMMC_PAD_CLR_MASK, REG_EMMC_CMD_PAD_VALUE << REG_EMMC_PAD_SHIFT);

	mmio_clrsetbits_32(REG_EMMC_DAT1_PAD_REG, REG_EMMC_PAD_CLR_MASK, REG_EMMC_DAT1_PAD_VALUE << REG_EMMC_PAD_SHIFT);

	mmio_clrsetbits_32(REG_EMMC_DAT0_PAD_REG, REG_EMMC_PAD_CLR_MASK, REG_EMMC_DAT0_PAD_VALUE << REG_EMMC_PAD_SHIFT);

	mmio_clrsetbits_32(REG_EMMC_DAT2_PAD_REG, REG_EMMC_PAD_CLR_MASK, REG_EMMC_DAT2_PAD_VALUE << REG_EMMC_PAD_SHIFT);

	mmio_clrsetbits_32(REG_EMMC_DAT3_PAD_REG, REG_EMMC_PAD_CLR_MASK, REG_EMMC_DAT3_PAD_VALUE << REG_EMMC_PAD_SHIFT);
}

static void cvi_sdio1_pad_setting(void)
{
	mmio_clrsetbits_32(REG_SDIO1_CLK_PAD_REG, REG_SDIO1_PAD_CLR_MASK,
			   REG_SDIO1_CLK_PAD_VALUE << REG_SDIO1_PAD_SHIFT);

	mmio_clrsetbits_32(REG_SDIO1_CMD_PAD_REG, REG_SDIO1_PAD_CLR_MASK,
			   REG_SDIO1_CMD_PAD_VALUE << REG_SDIO1_PAD_SHIFT);

	mmio_clrsetbits_32(REG_SDIO1_DAT1_PAD_REG, REG_SDIO1_PAD_CLR_MASK,
			   REG_SDIO1_DAT1_PAD_VALUE << REG_SDIO1_PAD_SHIFT);

	mmio_clrsetbits_32(REG_SDIO1_DAT0_PAD_REG, REG_SDIO1_PAD_CLR_MASK,
			   REG_SDIO1_DAT0_PAD_VALUE << REG_SDIO1_PAD_SHIFT);

	mmio_clrsetbits_32(REG_SDIO1_DAT2_PAD_REG, REG_SDIO1_PAD_CLR_MASK,
			   REG_SDIO1_DAT2_PAD_VALUE << REG_SDIO1_PAD_SHIFT);

	mmio_clrsetbits_32(REG_SDIO1_DAT3_PAD_REG, REG_SDIO1_PAD_CLR_MASK,
			   REG_SDIO1_DAT3_PAD_VALUE << REG_SDIO1_PAD_SHIFT);
}

static void cvi_sdio0_pad_setting(bool reset)
{
	if (reset) {
		mmio_clrsetbits_32(REG_SDIO0_CD_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_CD_PAD_RESET << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_CLK_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_CLK_PAD_RESET << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_CMD_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_CMD_PAD_RESET << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_DAT1_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_DAT1_PAD_RESET << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_DAT0_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_DAT0_PAD_RESET << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_DAT2_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_DAT2_PAD_RESET << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_DAT3_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_DAT3_PAD_RESET << REG_SDIO0_PAD_SHIFT);

	} else {
		mmio_clrsetbits_32(REG_SDIO0_CD_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_CD_PAD_VALUE << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_CLK_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_CLK_PAD_VALUE << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_CMD_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_CMD_PAD_VALUE << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_DAT1_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_DAT1_PAD_VALUE << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_DAT0_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_DAT0_PAD_VALUE << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_DAT2_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_DAT2_PAD_VALUE << REG_SDIO0_PAD_SHIFT);

		mmio_clrsetbits_32(REG_SDIO0_DAT3_PAD_REG, REG_SDIO0_PAD_CLR_MASK,
				   REG_SDIO0_DAT3_PAD_VALUE << REG_SDIO0_PAD_SHIFT);
	}
}

static void cvi_sdio0_pad_function(bool bunplug)
{
	/* Name                unplug plug
	 * PAD_SDIO0_CD        SDIO0  SDIO0
	 * PAD_SDIO0_PWR_EN    SDIO0  SDIO0
	 * PAD_SDIO0_CLK       XGPIO  SDIO0
	 * PAD_SDIO0_CMD       XGPIO  SDIO0
	 * PAD_SDIO0_D0        XGPIO  SDIO0
	 * PAD_SDIO0_D1        XGPIO  SDIO0
	 * PAD_SDIO0_D2        XGPIO  SDIO0
	 * PAD_SDIO0_D3        XGPIO  SDIO0
	 * 0x0: SDIO0 function
	 * 0x3: XGPIO function
	 */

	u8 val = (bunplug) ? 0x3 : 0x0;

	mmio_write_32(PAD_SDIO0_CD_REG, 0x0);
	mmio_write_32(PAD_SDIO0_PWR_EN_REG, 0x0);
	mmio_write_32(PAD_SDIO0_CLK_REG, val);
	mmio_write_32(PAD_SDIO0_CMD_REG, val);
	mmio_write_32(PAD_SDIO0_D0_REG, val);
	mmio_write_32(PAD_SDIO0_D1_REG, val);
	mmio_write_32(PAD_SDIO0_D2_REG, val);
	mmio_write_32(PAD_SDIO0_D3_REG, val);
}

static int cvi_ofdata_to_platdata(struct udevice *dev)
{
	struct cvi_sdhci_host *cvi_host = dev_get_priv(dev);
	struct sdhci_host *host = &cvi_host->host;
	int node = dev_of_offset(dev);
	int rtc_sdio1 = fdtdec_get_uint(gd->fdt_blob, node, "rtc_sdio1", 0);

	host->name = strdup(dev->name);
	host->ioaddr = (void *)devfdt_get_addr(dev);
	host->bus_width = fdtdec_get_int(gd->fdt_blob, node, "bus-width", 4);
	host->index = fdtdec_get_uint(gd->fdt_blob, node, "index", 0);
	host->max_clk = fdtdec_get_uint(gd->fdt_blob, node, "max-frequency", 0);

	cvi_host->mmc_init_freq = fdtdec_get_uint(gd->fdt_blob, node, "mmc_init_freq", 200000);
	cvi_host->mmc_trans_freq = fdtdec_get_uint(gd->fdt_blob, node, "mmc_trans_freq", 0);
	cvi_host->is_64_addressing = fdtdec_get_bool(gd->fdt_blob, node, "64_addressing");
	cvi_host->reset_tx_rx_phy = fdtdec_get_bool(gd->fdt_blob, node, "reset_tx_rx_phy");
	cvi_host->has_phy = fdtdec_get_bool(gd->fdt_blob, node, "has_phy");
	cvi_host->no_1_8_v = fdtdec_get_bool(gd->fdt_blob, node, "no-1-8-v");
	cvi_host->pll_index = fdtdec_get_uint(gd->fdt_blob, node, "pll_index", 0);
	cvi_host->pll_reg = fdtdec_get_uint(gd->fdt_blob, node, "pll_reg", 0);

	if (cvi_host->no_1_8_v)
		host->quirks |= SDHCI_QUIRK_NO_1_8_V;

	if (rtc_sdio1) {
		// set rtc sdio1 related register
		writel(0x1, 0x03000248);
		writel(0x10, 0x0502501c);
		writel(0xfffffffc, 0x05025030);
		//writel(0x0, 0x0502507c);
	}

	if (host->ioaddr == (void *)FDT_ADDR_T_NONE)
		return -EINVAL;

	return 0;
}

static int cvi_sdhci_bind(struct udevice *dev)
{
	struct cvi_sdhci_plat *plat = dev_get_plat(dev);

	pr_debug("[hq] %s\n", __func__);
	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

/* TODO */
static int cvi_sdhci_phy_init(struct cvi_sdhci_host *cvi_host)
{
	struct sdhci_host *host = &cvi_host->host;

	// Asset reset of phy
	sdhci_writel(host, sdhci_readl(host, SDHCI_P_PHY_CNFG) & ~(1 << PHY_CNFG_PHY_RSTN), SDHCI_P_PHY_CNFG);

	// Set PAD_SN PAD_SP
	sdhci_writel(host, (1 << PHY_CNFG_PHY_PWRGOOD) | (0x9 << PHY_CNFG_PAD_SP) | (0x8 << PHY_CNFG_PAD_SN), SDHCI_P_PHY_CNFG);

	// Set CMDPAD
	sdhci_writew(host, (0x2 << PAD_CNFG_RXSEL) | (1 << PAD_CNFG_WEAKPULL_EN) |
			(0x3 << PAD_CNFG_TXSLEW_CTRL_P) | (0x2 << PAD_CNFG_TXSLEW_CTRL_N), SDHCI_P_CMDPAD_CNFG);

	// Set DATAPAD
	sdhci_writew(host, (0x2 << PAD_CNFG_RXSEL) | (1 << PAD_CNFG_WEAKPULL_EN) |
			(0x3 << PAD_CNFG_TXSLEW_CTRL_P) | (0x2 << PAD_CNFG_TXSLEW_CTRL_N), SDHCI_P_DATPAD_CNFG);

	// Set CLKPAD
	sdhci_writew(host, (0x2 << PAD_CNFG_RXSEL) | (0x3 << PAD_CNFG_TXSLEW_CTRL_P) | (0x2 << PAD_CNFG_TXSLEW_CTRL_N),
		     SDHCI_P_CLKPAD_CNFG);

	// Set STB_PAD
	sdhci_writew(host, (0x2 << PAD_CNFG_RXSEL) | (0x2 << PAD_CNFG_WEAKPULL_EN) |
			(0x3 << PAD_CNFG_TXSLEW_CTRL_P) | (0x2 << PAD_CNFG_TXSLEW_CTRL_N), SDHCI_P_STBPAD_CNFG);

	// Set RSTPAD
	sdhci_writew(host, (0x2 << PAD_CNFG_RXSEL) | (1 << PAD_CNFG_WEAKPULL_EN) |
			(0x3 << PAD_CNFG_TXSLEW_CTRL_P) | (0x2 << PAD_CNFG_TXSLEW_CTRL_N), SDHCI_P_RSTNPAD_CNFG);

	// Set SDCLKDL_CNFG, EXTDLY_EN = 1, fix delay
	sdhci_writeb(host, (1 << SDCLKDL_CNFG_EXTDLY_EN), SDHCI_P_SDCLKDL_CNFG);

	// Add 70 * 10 ps = 0.7ns
	sdhci_writeb(host, 0xA, SDHCI_P_SDCLKDL_DC);

	if (host->index == 1) {
		// Set SMPLDL_CNFG, Bypass
		sdhci_writeb(host, (1 << SMPLDL_CNFG_BYPASS_EN), SDHCI_P_SMPLDL_CNFG);
	} else {
		// Set SMPLDL_CNFG, INPSEL_CNFG = 0x2
		sdhci_writeb(host, (0x2 << SMPLDL_CNFG_INPSEL_CNFG), SDHCI_P_SMPLDL_CNFG);
	}

	// Set ATDL_CNFG, tuning clk not use for init
	sdhci_writeb(host, (2 << ATDL_CNFG_INPSEL_CNFG), SDHCI_P_ATDL_CNFG);

	// Deasset reset of phy
	sdhci_writel(host, sdhci_readl(host, SDHCI_P_PHY_CNFG) | (1 << PHY_CNFG_PHY_RSTN), SDHCI_P_PHY_CNFG);

	return 0;
}

#ifdef CONFIG_MMC_SUPPORTS_TUNING
static void cvi_mmc_set_tap(struct sdhci_host *host, u16 tap)
{
	pr_debug("%s %d\n", __func__, tap);
	// Set sd_clk_en(0x2c[2]) to 0
	sdhci_writew(host, sdhci_readw(host, SDHCI_CLOCK_CONTROL) & (~(0x1 << 2)), SDHCI_CLOCK_CONTROL);
	sdhci_writew(host, 0, CVI_SDHCI_VENDOR_MSHC_CTRL_R);
	sdhci_writew(host, 0x18, CVI_SDHCI_VENDOR_A_CTRL_R);
	sdhci_writel(host, sdhci_readl(host, SDHCI_RX_DELAY_LINE) | 0x300000, SDHCI_RX_DELAY_LINE);
	sdhci_writew(host, tap, CVI_SDHCI_VENDOR_A_STAT_R);
	// Set sd_clk_en(0x2c[2]) to 1
	sdhci_writew(host, sdhci_readw(host, SDHCI_CLOCK_CONTROL) | (0x1 << 2), SDHCI_CLOCK_CONTROL);
}

static inline uint32_t CHECK_MASK_BIT(void *_mask, uint32_t bit)
{
	uint32_t w = bit / 8;
	uint32_t off = bit % 8;

	return ((uint8_t *)_mask)[w] & (1 << off);
}

static inline void SET_MASK_BIT(void *_mask, uint32_t bit)
{
	uint32_t byte = bit / 8;
	uint32_t offset = bit % 8;
	((uint8_t *)_mask)[byte] |= (1 << offset);
}

static void reset_after_tuning_pass(struct sdhci_host *host)
{
	pr_debug("tuning pass\n");

	/* Clear BUF_RD_READY intr */
	sdhci_writew(host, sdhci_readw(host, SDHCI_INT_STATUS) & (~(0x1 << 5)),
		     SDHCI_INT_STATUS);

	/* Set SDHCI_SOFTWARE_RESET.SW_RST_DAT = 1 to clear buffered tuning block */
	sdhci_writeb(host, sdhci_readb(host, SDHCI_SOFTWARE_RESET) | (0x1 << 2), SDHCI_SOFTWARE_RESET);

	/* Set SDHCI_SOFTWARE_RESET.SW_RST_CMD = 1	*/
	sdhci_writeb(host, sdhci_readb(host, SDHCI_SOFTWARE_RESET) | (0x1 << 1), SDHCI_SOFTWARE_RESET);

	while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & 0x3)
		;
}

int cvi_general_execute_tuning(struct mmc *mmc, u8 opcode)
{
	u16 min = 0;
	u32 k = 0;
	s32 ret;
	u32 retry_cnt = 0;

	u32 tuning_result[4] = {0, 0, 0, 0};
	u32 rx_lead_lag_result[4] = {0, 0, 0, 0};
	char tuning_graph[TUNE_MAX_PHCODE + 1];
	char rx_lead_lag_graph[TUNE_MAX_PHCODE + 1];

	u32 reg = 0;
	u32 reg_rx_lead_lag = 0;
	s32 max_lead_lag_idx = -1;
	s32 max_window_idx = -1;
	s32 cur_window_idx = -1;
	u16 max_lead_lag_size = 0;
	u16 max_window_size = 0;
	u16 cur_window_size = 0;
	s32 rx_lead_lag_phase = -1;
	s32 final_tap = -1;
	u32 rate = 0;

	struct cvi_sdhci_host *cvi_host = dev_get_priv(mmc->dev);
	struct sdhci_host *host = &cvi_host->host;

	u32 norm_stat_en_b, err_stat_en_b;
	u32 norm_signal_en_b, ctl2;

	norm_stat_en_b = sdhci_readw(host, SDHCI_INT_ENABLE);
	err_stat_en_b = sdhci_readw(host, SDHCI_ERR_INT_STATUS_EN);
	norm_signal_en_b = sdhci_readl(host, SDHCI_SIGNAL_ENABLE);

	sdhci_writel(host, 0x0, CVI_SDHCI_VENDOR_MSHC_CTRL_R);

	reg = sdhci_readw(host, SDHCI_ERR_INT_STATUS);
	pr_debug("mmc%d : SDHCI_ERR_INT_STATUS 0x%x\n", host->index, reg);

	reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	pr_debug("mmc%d : host ctrl2 0x%x\n", host->index, reg);
	/* Set Host_CTRL2_R.SAMPLE_CLK_SEL=0 */
	sdhci_writew(host,
		     sdhci_readw(host, SDHCI_HOST_CONTROL2) & (~(0x1 << 7)),
		     SDHCI_HOST_CONTROL2);
	sdhci_writew(host,
		     sdhci_readw(host, SDHCI_HOST_CONTROL2) & (~(0x3 << 4)),
		     SDHCI_HOST_CONTROL2);

	reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	pr_debug("mmc%d : host ctrl2 0x%x\n", host->index, reg);

	/* Set ATR_CTRL_R.SW_TNE_EN=1 */
	reg = sdhci_readl(host, CVI_SDHCI_VENDOR_A_CTRL_R);
	pr_debug("mmc%d : A ctrl 0x%x\n", host->index, reg);
	sdhci_writel(host,
		     sdhci_readl(host, CVI_SDHCI_VENDOR_A_CTRL_R) | (0x1 << 4),
		     CVI_SDHCI_VENDOR_A_CTRL_R);
	reg = sdhci_readl(host, CVI_SDHCI_VENDOR_A_CTRL_R);
	pr_debug("mmc%d : A ctrl 0x%x\n", host->index, reg);

	while (min < TUNE_MAX_PHCODE) {
		retry_cnt = 0;
		cvi_mmc_set_tap(host, min);
		reg_rx_lead_lag = sdhci_readw(host, 0x312) & BIT(1);

retry_tuning:
		ret = mmc_send_tuning(host->mmc, opcode, NULL);

		if (!ret && retry_cnt < MAX_TUNING_CMD_RETRY_COUNT) {
			retry_cnt++;
			goto retry_tuning;
		}

		if (ret) {
			SET_MASK_BIT(tuning_result, min);
		}

		if (reg_rx_lead_lag) {
			SET_MASK_BIT(rx_lead_lag_result, min);
		}

		min++;
	}

	reset_after_tuning_pass(host);

	pr_debug("mmc%d : tuning result:	  0x%08x 0x%08x 0x%08x 0x%08x\n", host->index,
		 tuning_result[0], tuning_result[1],
		 tuning_result[2], tuning_result[3]);
	pr_debug("mmc%d : rx_lead_lag result: 0x%08x 0x%08x 0x%08x 0x%08x\n", host->index,
		 rx_lead_lag_result[0], rx_lead_lag_result[1],
		 rx_lead_lag_result[2], rx_lead_lag_result[3]);
	for (k = 0; k < TUNE_MAX_PHCODE; k++) {
		if (CHECK_MASK_BIT(tuning_result, k) == 0)
			tuning_graph[k] = '-';
		else
			tuning_graph[k] = 'x';
		if (CHECK_MASK_BIT(rx_lead_lag_result, k) == 0)
			rx_lead_lag_graph[k] = '0';
		else
			rx_lead_lag_graph[k] = '1';
	}
	tuning_graph[TUNE_MAX_PHCODE] = '\0';
	rx_lead_lag_graph[TUNE_MAX_PHCODE] = '\0';

	pr_debug("mmc%d : tuning graph:      %s\n", host->index, tuning_graph);
	pr_debug("mmc%d : rx_lead_lag graph: %s\n", host->index, rx_lead_lag_graph);

	// Find a final tap as median of maximum window
	for (k = 0; k < TUNE_MAX_PHCODE; k++) {
		if (CHECK_MASK_BIT(tuning_result, k) == 0) {
			if (-1 == cur_window_idx) {
				cur_window_idx = k;
			}
			cur_window_size++;

			if (cur_window_size > max_window_size) {
				max_window_size = cur_window_size;
				max_window_idx = cur_window_idx;
				if (max_window_size >= TAP_WINDOW_THLD)
					final_tap = cur_window_idx + (max_window_size / 2);
			}
		} else {
			cur_window_idx = -1;
			cur_window_size = 0;
		}
	}

	cur_window_idx = -1;
	cur_window_size = 0;
	for (k = 0; k < TUNE_MAX_PHCODE; k++) {
		if (CHECK_MASK_BIT(rx_lead_lag_result, k) == 0) {
			//from 1 to 0 and window_size already computed.
			if ((rx_lead_lag_phase == 1) && (cur_window_size > 0)) {
				max_lead_lag_idx = cur_window_idx;
				max_lead_lag_size = cur_window_size;
				break;
			}
			if (cur_window_idx == -1) {
				cur_window_idx = k;
			}
			cur_window_size++;
			rx_lead_lag_phase = 0;
		} else {
			rx_lead_lag_phase = 1;
			if ((cur_window_idx != -1) && (cur_window_size > 0)) {
				cur_window_size++;
				max_lead_lag_idx = cur_window_idx;
				max_lead_lag_size = cur_window_size;
			} else {
				cur_window_size = 0;
			}
		}
	}
	rate = max_window_size * 100 / max_lead_lag_size;
	pr_debug("mmc%d : MaxWindow[Idx, Width]:[%d,%u] Tuning Tap: %d\n", host->index, max_window_idx, max_window_size, final_tap);
	pr_debug("mmc%d : RX_LeadLag[Idx, Width]:[%d,%u] rate = %d\n", host->index, max_lead_lag_idx, max_lead_lag_size, rate);

	cvi_mmc_set_tap(host, final_tap);
	//cvi_host->final_tap = final_tap;
	ret = mmc_send_tuning(host->mmc, opcode, NULL);
	printf("mmc%d : finished tuning, code:%d\n", host->index, final_tap);

	ctl2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	ctl2 &= ~SDHCI_CTRL_EXEC_TUNING;
	sdhci_writew(host, ctl2, SDHCI_HOST_CONTROL2);

	sdhci_writew(host, norm_stat_en_b, SDHCI_INT_ENABLE);
	sdhci_writel(host, norm_signal_en_b, SDHCI_SIGNAL_ENABLE);
	sdhci_writew(host, err_stat_en_b, SDHCI_ERR_INT_STATUS_EN);

	return ret;
}
#endif

static void cvi_general_reset(struct sdhci_host *host, u8 mask)
{
	u16 ctrl_2;

	ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	pr_debug("%s-%d MMC%d : ctrl_2 = 0x%04x\n", __func__, __LINE__, host->index, ctrl_2);

	ctrl_2 &= SDHCI_CTRL_UHS_MASK;
	if (ctrl_2 == SDHCI_CTRL_UHS_SDR104) {
		//reg_0x508[0] = 0
		sdhci_writew(host,
			     sdhci_readw(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R) & (~(BIT(0))),
			     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x30C[21:20] = 3
		sdhci_writel(host,
			     sdhci_readl(host, SDHCI_RX_DELAY_LINE) | (BIT(21) | BIT(20)),
			     SDHCI_RX_DELAY_LINE);
		//reg_0x31C[22:16] = 0, reg_0x31C[11:10] = 0
		sdhci_writel(host, sdhci_readl(host, SDHCI_TX_DELAY_LINE) & (~(0x7F0C00)), SDHCI_TX_DELAY_LINE);
		//reg_0x544[6:0] = tap
		sdhci_writew(host, 0 & 0x7F, CVI_SDHCI_VENDOR_A_STAT_R);
	} else {
		//Reset as DS/HS setting.
		//reg_0x508[0] = 1
		sdhci_writew(host,
			     sdhci_readw(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(0),
			     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x30C[21:20] = 0
		sdhci_writel(host,
			     sdhci_readl(host, SDHCI_RX_DELAY_LINE) & (~(BIT(21) | BIT(20))),
			     SDHCI_RX_DELAY_LINE);
		//reg_0x31C[22:16] = 0, reg_0x31C[11:10] = 0
		sdhci_writel(host, sdhci_readl(host, SDHCI_TX_DELAY_LINE) & (~(0x7F0C00)), SDHCI_TX_DELAY_LINE);
		//reg_0x544[6:0] = 0
		sdhci_writew(host,
			     sdhci_readw(host, CVI_SDHCI_VENDOR_A_STAT_R) & (~(0x7F)),
			     CVI_SDHCI_VENDOR_A_STAT_R);
	}
	pr_debug("reg_0x508 = 0x%08x, reg_0x30C = 0x%08x\n",
		 sdhci_readl(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R), sdhci_readl(host, SDHCI_RX_DELAY_LINE));
	pr_debug("reg_0x31C = 0x%08x, reg_0x544 = 0x%08x\n",
		 sdhci_readl(host, SDHCI_TX_DELAY_LINE), sdhci_readl(host, CVI_SDHCI_VENDOR_A_STAT_R));
}

#ifdef CONFIG_MMC_UHS_SUPPORT
static void cvi_sd_voltage_switch(struct mmc *mmc)
{
	pr_debug("%s-%d\n", __func__, __LINE__);

	//enable SDIO0_CLK[7:5] to set CLK max strengh
	mmio_setbits_32(REG_SDIO0_CLK_PAD_REG, BIT(7) | BIT(6) | BIT(5));

	//Voltage switching flow (1.8v)
	//reg_pwrsw_auto=1, reg_pwrsw_disc=0, pwrsw_vsel=1(1.8v), reg_en_pwrsw=1
	mmio_write_32(TOP_BASE + REG_TOP_SD_PWRSW_CTRL, 0xB);

	//set SDIO0 PAD to 1.8V mode
	//=>  0x03000018[5:4] = 2'b11  ,( reg_sd0_ms_ow=1. reg_sd0_ms_sw=0) => MS overwrite to 1 (1.8v mode)
	mmio_setbits_32(0x3000018, BIT(4) | BIT(5));

	//wait 1ms
	mdelay(1);
}
#endif

int cvi_get_cd(struct sdhci_host *host)
{
	u32 reg;

	reg = sdhci_readl(host, SDHCI_PRESENT_STATE);
	pr_debug("%s reg = 0x08%x\n", __func__, reg);
	if (reg & SDHCI_CARD_PRESENT) {
		return 1;
	} else {
		return 0;
	}
}

static const struct sdhci_ops cvi_sdhci_ops = {
	.get_cd	= cvi_get_cd,
#ifdef CONFIG_MMC_SUPPORTS_TUNING
	.platform_execute_tuning = cvi_general_execute_tuning,
#endif
#ifdef CONFIG_MMC_UHS_SUPPORT
	.voltage_switch = cvi_sd_voltage_switch,
#endif
	.reset = cvi_general_reset,
};

static const struct sdhci_ops cvi_sdhci_emmc_ops = {
#ifdef CONFIG_MMC_SUPPORTS_TUNING
	.platform_execute_tuning = cvi_general_execute_tuning,
#endif
	.reset = cvi_general_reset,
};

static int cvi_sdhci_probe(struct udevice *dev)
{
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct cvi_sdhci_plat *plat = dev_get_plat(dev);
	struct cvi_sdhci_host *cvi_host = dev_get_priv(dev);
	struct sdhci_host *host = &cvi_host->host;
	int ret;

	pr_debug("%s-%d: mmc%d probe\n", __func__, __LINE__, host->index);
	ret = reset_get_by_name(dev, "sdhci", &cvi_host->reset_ctl);
	if (ret) {
		pr_debug("warning: reset_get_by_name failed\n");
	} else {
		// Try to solve 1.8 to 3.3v converter HW issue
		ret = reset_assert(&cvi_host->reset_ctl);
		if (ret) {
			printf("%s failed assert reset\n", __func__);
			return ret;
		}

		ret = reset_deassert(&cvi_host->reset_ctl);
		if (ret) {
			printf("%s failed deassert reset\n", __func__);
			return ret;
		}
	}

	upriv->mmc = &plat->mmc;
	host->mmc = &plat->mmc;
	host->mmc->priv = host;
	host->mmc->dev = dev;
	pr_debug("host %p, mmc %p, priv %p\n", host, host->mmc, host->mmc->priv);

	ret = sdhci_setup_cfg(&plat->cfg, host, cvi_host->mmc_trans_freq, cvi_host->mmc_init_freq);

	if (ret)
		return ret;


	if (host->index == 0) {
		host->ops = &cvi_sdhci_emmc_ops;
		cvi_emmc_pad_setting();
		sdhci_writew(host, sdhci_readw(host, CVI_SDHCI_VENDOR_EMMC_CTRL_R) | 0x1 << 0,
			     CVI_SDHCI_VENDOR_EMMC_CTRL_R);
	} else if (host->index == 1) {
		host->ops = &cvi_sdhci_ops;
		if (host->ops && host->ops->get_cd) {
			int present = host->ops->get_cd(host);

			//set SDIO0 PAD to 3.3V mode
			//=>  0x03000018[5:4] = 2'b01
			mmio_setbits_32(0x3000018, BIT(4));
			pr_debug("SDIO0 PAD 0x%x\n", mmio_read_32(0x3000018));

			if (present == 1) {
				//Voltage switching flow (3.3)
				//(reg_pwrsw_auto=1, reg_pwrsw_disc=0, reg_pwrsw_vsel=0(3.0v), reg_en_pwrsw=1)
				mmio_write_32(TOP_BASE + REG_TOP_SD_PWRSW_CTRL, 0x9);
				cvi_sdio0_pad_function(false);
				cvi_sdio0_pad_setting(false);
			} else {
				//Voltage close flow
				//(reg_pwrsw_auto=1, reg_pwrsw_disc=1, reg_pwrsw_vsel=1(1.8v), reg_en_pwrsw=0)
				mmio_write_32(TOP_BASE + REG_TOP_SD_PWRSW_CTRL, 0xE);
				cvi_sdio0_pad_function(true);
				cvi_sdio0_pad_setting(true);
			}
		}
	} else if (host->index == 2) {
		pr_debug("sdio1 probe\n");
		cvi_sdio1_pad_setting();
	} else {
		printf("wrong host index : %d !!\n", host->index);
		return -ENXIO;
	}

	ret = sdhci_probe(dev);

	if (cvi_host->has_phy) {
		cvi_sdhci_phy_init(cvi_host);
	}

	if (host->max_clk == MMC_MAX_CLOCK) {
		// set IP clock to 375Mhz
		pr_debug("set IP clock to 375Mhz\n");
		mmio_write_32(cvi_host->pll_reg, MMC_MAX_CLOCK_DIV_VALUE);

		pr_debug("Be sure to switch clock source to PLL\n");
		mmio_clrbits_32(CLOCK_BYPASS_SELECT_REGISTER, BIT(cvi_host->pll_index));
		pr_debug("XTAL->PLL reg = 0x%x\n", mmio_read_32(CLOCK_BYPASS_SELECT_REGISTER));

		pr_debug("eMMC/SD CLK is %d in FPGA_ASIC\n", host->max_clk);
	}

	if (cvi_host->is_64_addressing) {
		sdhci_writew(host, sdhci_readw(host, SDHCI_HOST_CONTROL2)
				| SDHCI_HOST_VER4_ENABLE | SDHCI_HOST_ADDRESSING,
				SDHCI_HOST_CONTROL2);
	}

	if (cvi_host->reset_tx_rx_phy) {
		pr_debug("set tx rx src sel to 0 ======================\n");
		sdhci_writel(host, 0, SDHCI_RX_DELAY_LINE);
		sdhci_writel(host, 0, SDHCI_TX_DELAY_LINE);
	}

	return ret;
}

static const struct udevice_id cvi_sdhci_match[] = {
	{ .compatible = "cvitek,synopsys-sdhc" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(cvi_sdhci) = {
	.name = "cvi_sdhci",
	.id = UCLASS_MMC,
	.of_match = cvi_sdhci_match,
	.of_to_plat = cvi_ofdata_to_platdata,
	.bind = cvi_sdhci_bind,
	.probe = cvi_sdhci_probe,
	.priv_auto = sizeof(struct cvi_sdhci_host),
	.plat_auto = sizeof(struct cvi_sdhci_plat),
	.ops = &sdhci_ops,
};
