/*
 * drivers/mmc/host/sdhci-cv.c - CVITEK SDHCI Platform driver
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

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/slab.h>
#include <linux/reset.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/device.h>
#include <linux/export.h>
#include <linux/io.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/of_gpio.h>
#include <linux/mmc/slot-gpio.h>
#include <linux/ktime.h>
#include <linux/clk.h>
#include <linux/sizes.h>
#include <linux/dma-mapping.h>
#include <linux/kernel.h>

#include "../../core/card.h"
#include "../sdhci-pltfm.h"
#include "sdhci-cv181x.h"

#define DRIVER_NAME "cvi"
#define SDHCI_DUMP(f, x...) \
	pr_err("%s: " DRIVER_NAME ": " f, mmc_hostname(host->mmc), ## x)

#define MAX_CARD_TYPE 4
#define MAX_SPEED_MODE 5

#define CVI_PARENT "cvi"
#define CVI_STATS_PROC "cvi_info"
#define MAX_CLOCK_SCALE (4)

#define UNSTUFF_BITS(resp, start, size)                 \
	({                                                      \
	const int __size = size;                                \
	const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1; \
	const int __off = 3 - ((start) / 32);                   \
	const int __shft = (start) & 31;                        \
	u32 __res;                                              \
	__res = resp[__off] >> __shft;                          \
	if (__size + __shft > 32)                               \
		__res |= resp[__off - 1] << ((32 - __shft) % 32);   \
	__res & __mask;                                         \
	})

#define BOUNDARY_OK(addr, len) \
	((addr | (SZ_128M - 1)) == ((addr + len - 1) | (SZ_128M - 1)))

static struct proc_dir_entry *proc_cvi_dir;

static char *card_type[MAX_CARD_TYPE + 1] = {
	"MMC card", "SD card", "SDIO card", "SD combo (IO+mem) card", "unknown"
};

static char *cvi_get_card_type(unsigned int sd_type)
{
	if (sd_type >= MAX_CARD_TYPE)
		return card_type[MAX_CARD_TYPE];
	else
		return card_type[sd_type];
}

static inline int is_card_uhs(unsigned char timing)
{
	return timing >= MMC_TIMING_UHS_SDR12 && timing <= MMC_TIMING_UHS_DDR50;
};

static inline int is_card_hs(unsigned char timing)
{
	return timing == MMC_TIMING_SD_HS || timing == MMC_TIMING_MMC_HS;
};

static void cvi_stats_seq_printout(struct seq_file *s)
{
	const char *type = NULL;
	unsigned int present;
	unsigned char timing;
	struct sdhci_cvi_host *cvi_host = NULL;
	struct mmc_host *mmc = NULL;
	struct mmc_card *card = NULL;
	const char *uhs_bus_speed_mode = "";
	u32 speed_class, grade_speed_uhs;
	static const char *const uhs_speeds[] = {
		[UHS_SDR12_BUS_SPEED] = "SDR12 ", [UHS_SDR25_BUS_SPEED] = "SDR25 ",
		[UHS_SDR50_BUS_SPEED] = "SDR50 ", [UHS_SDR104_BUS_SPEED] = "SDR104 ",
		[UHS_DDR50_BUS_SPEED] = "DDR50 ",
	};
	cvi_host = (struct sdhci_cvi_host *)s->private;

	if (!cvi_host || !cvi_host->mmc) {
		seq_printf(s, "cvi s : %p: s->private %p\n", s, s->private);
		return;
	}

	seq_printf(s, "cvi.%d", cvi_host->mmc->index);

	mmc = cvi_host->mmc;
	present = mmc->ops->get_cd(mmc);

	if (present) {
		seq_puts(s, ": plugged");
	} else {
		seq_puts(s, ": unplugged");
	}

	card = mmc->card;

	if (!card) {
		seq_puts(s, "_disconnected\n");
	} else {
		seq_puts(s, "_connected\n");
		seq_printf(s, "\tType: %s", cvi_get_card_type(card->type));

		//if (card->state & MMC_STATE_BLOCKADDR) {
		if (mmc_card_blockaddr(card)) {
			if (card->state & MMC_CARD_SDXC)
				type = "SDXC";
			else
				type = "SDHC";
			seq_printf(s, "(%s)\n", type);
		}

		timing = mmc->ios.timing;

		if (is_card_uhs(mmc->ios.timing) &&
		    card->sd_bus_speed < ARRAY_SIZE(uhs_speeds))
			uhs_bus_speed_mode = uhs_speeds[card->sd_bus_speed];

		seq_printf(s, "\tMode: %s%s%s%s\n",
			   is_card_uhs(timing) ? "UHS "
			   : (is_card_hs(timing) ? "HS " : ""),
			   timing == MMC_TIMING_MMC_HS400
			   ? "HS400 "
			   : (timing == MMC_TIMING_MMC_HS200 ? "HS200 " : ""),
			   timing == MMC_TIMING_MMC_DDR52 ? "DDR " : "",
			   uhs_bus_speed_mode);

		speed_class = UNSTUFF_BITS(card->raw_ssr, 440 - 384, 8);
		grade_speed_uhs = UNSTUFF_BITS(card->raw_ssr, 396 - 384, 4);
		seq_printf(s, "\tSpeed Class: Class %s\n",
			   (speed_class == 0x00)
			   ? "0"
			   : (speed_class == 0x01)
			   ? "2"
			   : (speed_class == 0x02)
			   ? "4"
			   : (speed_class == 0x03)
			   ? "6"
			   : (speed_class == 0x04)
			   ? "10"
			   : "Reserved");
		seq_printf(s, "\tUhs Speed Grade: %s\n",
			   (grade_speed_uhs == 0x00)
			   ? "Less than 10MB/sec(0h)"
			   : (grade_speed_uhs == 0x01)
			   ? "10MB/sec and above(1h)"
			   : "Reserved");
	}
}

/* proc interface setup */
static void *cvi_seq_start(struct seq_file *s, loff_t *pos)
{
	/*   counter is used to tracking multi proc interfaces
	 *  We have only one interface so return zero
	 *  pointer to start the sequence.
	 */
	static unsigned long counter;

	if (*pos == 0) {
		return &counter;
	}

	*pos = 0;
	return NULL;
}

/* define parameters where showed in proc file */
static int cvi_stats_seq_show(struct seq_file *s, void *v)
{
	cvi_stats_seq_printout(s);
	return 0;
}

/* proc interface stop */
static void cvi_seq_stop(struct seq_file *s, void *v) {}

/* proc interface operation */
static const struct seq_operations cvi_stats_seq_ops = {
	.start = cvi_seq_start,
	.stop = cvi_seq_stop,
	.show = cvi_stats_seq_show
};

/* proc file open*/
static int cvi_stats_proc_open(struct inode *inode, struct file *file)
{
//	return seq_open(file, &cvi_stats_seq_ops);
	return single_open(file, cvi_stats_seq_show, PDE_DATA(inode));
};

/* proc file operation */
static const struct proc_ops cvi_stats_proc_ops = {
	.proc_open = cvi_stats_proc_open,
	.proc_read = seq_read,
	.proc_release = single_release,
};

int cvi_proc_init(struct sdhci_cvi_host *cvi_host)
{
	struct proc_dir_entry *proc_stats_entry;

	pr_info("%s cvi_host 0x%p\n", __func__, cvi_host);

	proc_cvi_dir = proc_mkdir(CVI_PARENT, NULL);

	if (!proc_cvi_dir) {
		pr_err("%s: failed to create proc file %s\n", __func__, CVI_PARENT);
		return 1;
	}

	proc_stats_entry = proc_create_data(CVI_STATS_PROC, 0400, proc_cvi_dir, &cvi_stats_proc_ops,
					    (void *)cvi_host);

	if (!proc_stats_entry) {
		pr_err("%s: failed to create proc file %s\n", __func__, CVI_STATS_PROC);
		return 1;
	}

	return 0;
}

int cvi_proc_shutdown(struct sdhci_cvi_host *cvi_host)
{
	pr_info("%s\n", __func__);

	if (proc_cvi_dir) {
		remove_proc_entry(CVI_STATS_PROC, proc_cvi_dir);
		remove_proc_entry(CVI_PARENT, NULL);
		proc_cvi_dir = NULL;
	}

	return 0;
}

static void sdhci_cv181x_emmc_setup_pad(struct sdhci_host *host)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);

	/* Name              Offset
	 * PAD_EMMC_RSTN     0x48
	 * PAD_EMMC_CLK      0x50
	 * PAD_EMMC_CMD      0x5C
	 * PAD_EMMC_DAT0     0x54
	 * PAD_EMMC_DAT1     0x60
	 * PAD_EMMC_DAT2     0x4C
	 * PAD_EMMC_DAT3     0x58

	 */

	u8 val = 0x0;

	writeb(val, cvi_host->pinmuxbase + 0x48);
	writeb(val, cvi_host->pinmuxbase + 0x50);
	writeb(val, cvi_host->pinmuxbase + 0x5C);
	writeb(val, cvi_host->pinmuxbase + 0x54);
	writeb(val, cvi_host->pinmuxbase + 0x60);
	writeb(val, cvi_host->pinmuxbase + 0x4C);
	writeb(val, cvi_host->pinmuxbase + 0x58);
}

static void sdhci_cv181x_sd_setup_pad(struct sdhci_host *host, bool bunplug)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);

	/* Name              Offset unplug plug
	 * PAD_SDIO0_CD      0x34   SDIO0  SDIO0
	 * PAD_SDIO0_PWR_EN  0x38   SDIO0  SDIO0
	 * PAD_SDIO0_CLK     0x1C   XGPIO  SDIO0
	 * PAD_SDIO0_CMD     0x20   XGPIO  SDIO0
	 * PAD_SDIO0_D0      0x24   XGPIO  SDIO0
	 * PAD_SDIO0_D1      0x28   XGPIO  SDIO0
	 * PAD_SDIO0_D2      0x2C   XGPIO  SDIO0
	 * PAD_SDIO0_D3      0x30   XGPIO  SDIO0
	 * 0x0: SDIO0 function
	 * 0x3: XGPIO function
	 */

	u8 val = (bunplug) ? 0x3 : 0x0;

	if (cvi_host->cvi_gpio && cvi_host->cvi_gpio->cd_gpio)
		writeb(0x3, cvi_host->pinmuxbase + 0x34);
	else
		writeb(0x0, cvi_host->pinmuxbase + 0x34);

	writeb(0x0, cvi_host->pinmuxbase + 0x38);
	writeb(val, cvi_host->pinmuxbase + 0x1C);
	writeb(val, cvi_host->pinmuxbase + 0x20);
	writeb(val, cvi_host->pinmuxbase + 0x24);
	writeb(val, cvi_host->pinmuxbase + 0x28);
	writeb(val, cvi_host->pinmuxbase + 0x2C);
	writeb(val, cvi_host->pinmuxbase + 0x30);
}

static void sdhci_cv181x_sd_setup_io(struct sdhci_host *host, bool reset)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);

	/*
	 * Name              Offset reset sd0
	 * REG_SDIO0_CD      0x900  PU    PU
	 * REG_SDIO0_PWR_EN  0x904  PD    PD
	 * REG_SDIO0_CLK     0xA00  PD    PD
	 * REG_SDIO0_CMD     0xA04  PD    PU
	 * REG_SDIO0_D0      0xA08  PD    PU
	 * REG_SDIO0_D1      0xA0C  PD    PU
	 * REG_SDIO0_D2      0xA10  PD    PU
	 * REG_SDIO0_D3      0xA14  PD    PU
	 * BIT(2) : PU   enable(1)/disable(0)
	 * BIT(3) : PD   enable(1)/disable(0)
	 */

	u8 raise_bit = (reset) ?  BIT(3) : BIT(2);
	u8 down_bit  = (reset) ?  BIT(2) : BIT(3);

	writeb(((readb(cvi_host->pinmuxbase + 0x900) | BIT(2)) & ~(BIT(3))),
		cvi_host->pinmuxbase + 0x900);
	writeb(((readb(cvi_host->pinmuxbase + 0x904) | BIT(3)) & ~(BIT(2))),
		cvi_host->pinmuxbase + 0x904);
	writeb(((readb(cvi_host->pinmuxbase + 0xA00) | BIT(3)) & ~(BIT(2))),
		cvi_host->pinmuxbase + 0xA00);
	writeb(((readb(cvi_host->pinmuxbase + 0xA04) | raise_bit) & ~(down_bit)),
		cvi_host->pinmuxbase + 0xA04);
	writeb(((readb(cvi_host->pinmuxbase + 0xA08) | raise_bit) & ~(down_bit)),
		cvi_host->pinmuxbase + 0xA08);
	writeb(((readb(cvi_host->pinmuxbase + 0xA0C) | raise_bit) & ~(down_bit)),
		cvi_host->pinmuxbase + 0xA0C);
	writeb(((readb(cvi_host->pinmuxbase + 0xA10) | raise_bit) & ~(down_bit)),
		cvi_host->pinmuxbase + 0xA10);
	writeb(((readb(cvi_host->pinmuxbase + 0xA14) | raise_bit) & ~(down_bit)),
		cvi_host->pinmuxbase + 0xA14);
}

static void sdhci_cvi_reset_helper(struct sdhci_host *host, u8 mask)
{
	// disable Intr before reset
	sdhci_writel(host, 0, SDHCI_INT_ENABLE);
	sdhci_writel(host, 0, SDHCI_SIGNAL_ENABLE);

	sdhci_reset(host, mask);

	sdhci_writel(host, host->ier, SDHCI_INT_ENABLE);
	sdhci_writel(host, host->ier, SDHCI_SIGNAL_ENABLE);
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

static int sdhci_cv181x_general_select_drive_strength(struct sdhci_host *host,
		struct mmc_card *card, unsigned int max_dtr, int host_drv,
		int card_drv, int *drv_type)
{
	return MMC_SET_DRIVER_TYPE_A;
}

static void sdhci_cvi_general_set_uhs_signaling(struct sdhci_host *host, unsigned int uhs)
{
	struct mmc_host *mmc = host->mmc;
	u16 ctrl_2;

	ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	/* Select Bus Speed Mode for host */
	ctrl_2 &= ~SDHCI_CTRL_UHS_MASK;
	switch (uhs) {
	case MMC_TIMING_UHS_SDR12:
		ctrl_2 |= SDHCI_CTRL_UHS_SDR12;
		break;
	case MMC_TIMING_UHS_SDR25:
		ctrl_2 |= SDHCI_CTRL_UHS_SDR25;
		break;
	case MMC_TIMING_UHS_SDR50:
		ctrl_2 |= SDHCI_CTRL_UHS_SDR50;
		break;
	case MMC_TIMING_MMC_HS200:
	case MMC_TIMING_UHS_SDR104:
		ctrl_2 |= SDHCI_CTRL_UHS_SDR104;
		break;
	case MMC_TIMING_UHS_DDR50:
	case MMC_TIMING_MMC_DDR52:
		ctrl_2 |= SDHCI_CTRL_UHS_DDR50;
		break;
	}

	/*
	 * When clock frequency is less than 100MHz, the feedback clock must be
	 * provided and DLL must not be used so that tuning can be skipped. To
	 * provide feedback clock, the mode selection can be any value less
	 * than 3'b011 in bits [2:0] of HOST CONTROL2 register.
	 */
	if (host->clock <= 100000000 &&
	    (uhs == MMC_TIMING_MMC_HS400 ||
	     uhs == MMC_TIMING_MMC_HS200 ||
	     uhs == MMC_TIMING_UHS_SDR104))
		ctrl_2 &= ~SDHCI_CTRL_UHS_MASK;

	dev_dbg(mmc_dev(mmc), "%s: clock=%u uhs=%u ctrl_2=0x%x\n",
		mmc_hostname(host->mmc), host->clock, uhs, ctrl_2);
	sdhci_writew(host, ctrl_2, SDHCI_HOST_CONTROL2);
}

static unsigned int sdhci_cvi_general_get_max_clock(struct sdhci_host *host)
{
	pr_debug(DRIVER_NAME ":%s : %d\n", __func__, host->mmc->f_src);
	return host->mmc->f_src;
}

/* Used for wifi driver due if no SD card detect pin implemented */
static struct mmc_host *wifi_mmc;

int cvi_sdio_rescan(void)
{

	if (!wifi_mmc) {
		pr_err("invalid wifi mmc, please check the argument\n");
		return -EINVAL;
	}

	mmc_detect_change(wifi_mmc, 0);

	wifi_mmc->rescan_entered = 0;

	return 0;
}
EXPORT_SYMBOL_GPL(cvi_sdio_rescan);


void sdhci_cvi_emmc_voltage_switch(struct sdhci_host *host)
{
}

static void sdhci_cvi_cv181x_set_tap(struct sdhci_host *host, unsigned int tap)
{
	pr_debug("%s %d\n", __func__, tap);
	// Set sd_clk_en(0x2c[2]) to 0
	sdhci_writew(host, sdhci_readw(host, SDHCI_CLOCK_CONTROL) & (~(0x1 << 2)), SDHCI_CLOCK_CONTROL);
	sdhci_writel(host,
		sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) & (~(BIT(1))),
		CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
	sdhci_writel(host, BIT(8) | tap << 16,
		     CVI_CV181X_SDHCI_PHY_TX_RX_DLY);
	sdhci_writel(host, 0, CVI_CV181X_SDHCI_PHY_CONFIG);
	// Set sd_clk_en(0x2c[2]) to 1
	sdhci_writew(host, sdhci_readw(host, SDHCI_CLOCK_CONTROL) | (0x1 << 2), SDHCI_CLOCK_CONTROL);
	mdelay(1);
}

static int sdhci_cv181x_general_execute_tuning(struct sdhci_host *host, u32 opcode)
{
	u16 min = 0;
	u32 k = 0;
	s32 ret;
	u32 retry_cnt = 0;

	u32 tuning_result[4] = {0, 0, 0, 0};
	u32 rx_lead_lag_result[4] = {0, 0, 0, 0};
	char tuning_graph[TUNE_MAX_PHCODE+1];
	char rx_lead_lag_graph[TUNE_MAX_PHCODE+1];

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

	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);

	reg = sdhci_readw(host, SDHCI_ERR_INT_STATUS);
	pr_debug("%s : SDHCI_ERR_INT_STATUS 0x%x\n", mmc_hostname(host->mmc),
		 reg);

	reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	pr_debug("%s : host ctrl2 0x%x\n", mmc_hostname(host->mmc), reg);
	/* Set Host_CTRL2_R.SAMPLE_CLK_SEL=0 */
	sdhci_writew(host,
			 sdhci_readw(host, SDHCI_HOST_CONTROL2) & (~(0x1 << 7)),
			 SDHCI_HOST_CONTROL2);
	sdhci_writew(host,
			 sdhci_readw(host, SDHCI_HOST_CONTROL2) & (~(0x3 << 4)),
			 SDHCI_HOST_CONTROL2);

	reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	pr_debug("%s : host ctrl2 0x%x\n", mmc_hostname(host->mmc), reg);

	while (min < TUNE_MAX_PHCODE) {
		retry_cnt = 0;
		sdhci_cvi_cv181x_set_tap(host, min);
		reg_rx_lead_lag = sdhci_readw(host, CVI_CV181X_SDHCI_PHY_DLY_STS) & BIT(1);

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

	pr_debug("tuning result:      0x%08x 0x%08x 0x%08x 0x%08x\n",
		tuning_result[0], tuning_result[1], tuning_result[2], tuning_result[3]);
	pr_debug("rx_lead_lag result: 0x%08x 0x%08x 0x%08x 0x%08x\n",
		rx_lead_lag_result[0], rx_lead_lag_result[1], rx_lead_lag_result[2], rx_lead_lag_result[3]);
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

	pr_debug("tuning graph:      %s\n", tuning_graph);
	pr_debug("rx_lead_lag graph: %s\n", rx_lead_lag_graph);

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
					final_tap = cur_window_idx + (max_window_size/2);
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
	pr_debug("MaxWindow[Idx, Width]:[%d,%u] Tuning Tap: %d\n", max_window_idx, max_window_size, final_tap);
	pr_debug("RX_LeadLag[Idx, Width]:[%d,%u] rate = %d\n", max_lead_lag_idx, max_lead_lag_size, rate);

	sdhci_cvi_cv181x_set_tap(host, final_tap);
	cvi_host->final_tap = final_tap;
	pr_debug("%s finished tuning, code:%d\n", __func__, final_tap);

	return mmc_send_tuning(host->mmc, opcode, NULL);
}

static void sdhci_cv181x_emmc_reset(struct sdhci_host *host, u8 mask)
{
	u16 ctrl_2;
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);

	pr_debug("%s mask = 0x%x\n", __func__, mask);
	sdhci_cvi_reset_helper(host, mask);

	//reg_0x200[0] = 1 for mmc
	sdhci_writel(host,
			 sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(0),
			 CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);

	ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	ctrl_2 &= SDHCI_CTRL_UHS_MASK;
	if (ctrl_2 == SDHCI_CTRL_UHS_SDR104) {
		//reg_0x200[1] = 0
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) & ~(BIT(1)),
			CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x24c[0] = 0
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_PHY_CONFIG) & ~(BIT(0)),
			CVI_CV181X_SDHCI_PHY_CONFIG);
		//reg_0x240[22:16] = tap reg_0x240[9:8] = 1 reg_0x240[6:0] = 0
		sdhci_writel(host,
			(BIT(8) | ((cvi_host->final_tap & 0x7F) << 16)),
			CVI_CV181X_SDHCI_PHY_TX_RX_DLY);
	} else {
		//Reset as DS/HS setting.
		//reg_0x200[1] = 1
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(1),
			CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x24c[0] = 1
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_PHY_CONFIG) | BIT(0),
			CVI_CV181X_SDHCI_PHY_CONFIG);
		//reg_0x240[25:24] = 1 reg_0x240[22:16] = 0 reg_0x240[9:8] = 1 reg_0x240[6:0] = 0
		sdhci_writel(host, 0x1000100, CVI_CV181X_SDHCI_PHY_TX_RX_DLY);
	}
}

static void sdhci_cv181x_sd_reset(struct sdhci_host *host, u8 mask)
{
	u16 ctrl_2;
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);

	pr_debug("%s mask = 0x%x\n", __func__, mask);
	sdhci_cvi_reset_helper(host, mask);

	ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	ctrl_2 &= SDHCI_CTRL_UHS_MASK;
	if (ctrl_2 == SDHCI_CTRL_UHS_SDR104) {
		//reg_0x200[1] = 0
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) & ~(BIT(1)),
			CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x24c[0] = 0
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_PHY_CONFIG) & ~(BIT(0)),
			CVI_CV181X_SDHCI_PHY_CONFIG);
		//reg_0x240[22:16] = tap reg_0x240[9:8] = 1 reg_0x240[6:0] = 0
		sdhci_writel(host,
			(BIT(8) | ((cvi_host->final_tap & 0x7F) << 16)),
			CVI_CV181X_SDHCI_PHY_TX_RX_DLY);
	} else {
		//Reset as DS/HS setting.
		//reg_0x200[1] = 1
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(1),
			CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x24c[0] = 1
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_PHY_CONFIG) | BIT(0),
			CVI_CV181X_SDHCI_PHY_CONFIG);
		//reg_0x240[25:24] = 1 reg_0x240[22:16] = 0 reg_0x240[9:8] = 1 reg_0x240[6:0] = 0
		sdhci_writel(host, 0x1000100, CVI_CV181X_SDHCI_PHY_TX_RX_DLY);
	}
}

static void sdhci_cv181x_sdio_reset(struct sdhci_host *host, u8 mask)
{
	u16 ctrl_2;
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);

	pr_debug("%s mask = 0x%x\n", __func__, mask);
	sdhci_cvi_reset_helper(host, mask);

	ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	ctrl_2 &= SDHCI_CTRL_UHS_MASK;
	if (ctrl_2 == SDHCI_CTRL_UHS_SDR104) {
		//reg_0x200[1] = 0
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) & ~(BIT(1)),
			CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x200[16] = 1 for sd1
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(16),
			CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x24c[0] = 0
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_PHY_CONFIG) & ~(BIT(0)),
			CVI_CV181X_SDHCI_PHY_CONFIG);
		//reg_0x240[22:16] = tap reg_0x240[9:8] = 1 reg_0x240[6:0] = 0
		sdhci_writel(host,
			(BIT(8) | ((cvi_host->final_tap & 0x7F) << 16)),
			CVI_CV181X_SDHCI_PHY_TX_RX_DLY);
	} else {
		//Reset as DS/HS setting.
		//reg_0x200[1] = 1
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(1),
			CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x200[16] = 1 for sd1
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(16),
			CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
		//reg_0x24c[0] = 1
		sdhci_writel(host,
			sdhci_readl(host, CVI_CV181X_SDHCI_PHY_CONFIG) | BIT(0),
			CVI_CV181X_SDHCI_PHY_CONFIG);
		//reg_0x240[25:24] = 1 reg_0x240[22:16] = 0 reg_0x240[9:8] = 1 reg_0x240[6:0] = 0
		sdhci_writel(host, 0x1000100, CVI_CV181X_SDHCI_PHY_TX_RX_DLY);
	}
}

void sdhci_cv181x_sd_voltage_switch(struct sdhci_host *host)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);

	pr_debug("%s\n", __func__);

	// enable SDIO0_CLK[7:5] to set CLK max strengh
	writeb((readb(cvi_host->pinmuxbase + 0xA00) | BIT(7) | BIT(6) | BIT(5)),
		cvi_host->pinmuxbase + 0xA00);

	//Voltage switching flow (1.8v)
	//reg_pwrsw_auto=1, reg_pwrsw_disc=0, pwrsw_vsel=1(1.8v), reg_en_pwrsw=1
	writel(0xB | (readl(cvi_host->topbase + OFFSET_SD_PWRSW_CTRL) & 0xFFFFFFF0),
		cvi_host->topbase + OFFSET_SD_PWRSW_CTRL);
	pr_debug("sd PWRSW 0x%x\n", readl(cvi_host->topbase + OFFSET_SD_PWRSW_CTRL));
	cvi_host->sdio0_voltage_1_8_v = 1;

	mdelay(1);
}

void sdhci_cv181x_sd_voltage_restore(struct sdhci_host *host, bool bunplug)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);

	pr_debug("%s\n", __func__);

	if (bunplug) {
		//Voltage close flow
		//(reg_pwrsw_auto=1, reg_pwrsw_disc=1, reg_pwrsw_vsel=1(1.8v), reg_en_pwrsw=0)
		writel(0xE | (readl(cvi_host->topbase + OFFSET_SD_PWRSW_CTRL) & 0xFFFFFFF0),
			cvi_host->topbase + OFFSET_SD_PWRSW_CTRL);
		cvi_host->sdio0_voltage_1_8_v = 0;
	} else {
		if (!cvi_host->sdio0_voltage_1_8_v) {
			//Voltage switching flow (3.3)
			//(reg_pwrsw_auto=1, reg_pwrsw_disc=0, reg_pwrsw_vsel=0(3.0v), reg_en_pwrsw=1)
			writel(0x9 | (readl(cvi_host->topbase + OFFSET_SD_PWRSW_CTRL) & 0xFFFFFFF0),
				cvi_host->topbase + OFFSET_SD_PWRSW_CTRL);
		}
	}

	//wait 1ms
	mdelay(1);

	// restore to DS/HS setting
	sdhci_writel(host,
		sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(1) | BIT(8) | BIT(9),
		CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R);
	sdhci_writel(host, 0x1000100, CVI_CV181X_SDHCI_PHY_TX_RX_DLY);
	sdhci_writel(host, 1, CVI_CV181X_SDHCI_PHY_CONFIG);

	mdelay(1);
}

static void sdhci_cv181x_sd_set_power(struct sdhci_host *host, unsigned char mode,
				unsigned short vdd)
{
	struct mmc_host *mmc = host->mmc;

	pr_debug("%s:mode %u, vdd %u\n", __func__, mode, vdd);

	if (mode == MMC_POWER_ON && mmc->ops->get_cd(mmc)) {
		sdhci_set_power_noreg(host, mode, vdd);
		sdhci_cv181x_sd_voltage_restore(host, false);
		sdhci_cv181x_sd_setup_pad(host, false);
		sdhci_cv181x_sd_setup_io(host, false);
		mdelay(5);
	} else if (mode == MMC_POWER_OFF) {
		sdhci_cv181x_sd_setup_pad(host, true);
		sdhci_cv181x_sd_setup_io(host, true);
		sdhci_cv181x_sd_voltage_restore(host, true);
		sdhci_set_power_noreg(host, mode, vdd);
		mdelay(30);
	}
}

static void sdhci_cv181x_emmc_dump_vendor_regs(struct sdhci_host *host)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);
	u8 clk_source_select = 0;
	u8 PAD_EMMC_RSTN = 0;
	u8 PAD_EMMC_CLK  = 0;
	u8 PAD_EMMC_CMD  = 0;
	u8 PAD_EMMC_DAT0 = 0;
	u8 PAD_EMMC_DAT1 = 0;
	u8 PAD_EMMC_DAT2 = 0;
	u8 PAD_EMMC_DAT3 = 0;
	u8 REG_EMMC_RSTN = 0;
	u8 REG_EMMC_CLK  = 0;
	u8 REG_EMMC_CMD  = 0;
	u8 REG_EMMC_DAT0 = 0;
	u8 REG_EMMC_DAT1 = 0;
	u8 REG_EMMC_DAT2 = 0;
	u8 REG_EMMC_DAT3 = 0;

	SDHCI_DUMP(": Reg_200:   0x%08x | Reg_240:  0x%08x\n",
		   sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R),
		   sdhci_readl(host, CVI_CV181X_SDHCI_PHY_TX_RX_DLY));
	SDHCI_DUMP(": Reg_244:   0x%08x | Reg_248:  0x%08x\n",
		   sdhci_readl(host, CVI_CV181X_SDHCI_PHY_DS_DLY),
		   sdhci_readw(host, CVI_CV181X_SDHCI_PHY_DLY_STS));
	SDHCI_DUMP(": Reg_24C:   0x%08x\n",
		   sdhci_readl(host, CVI_CV181X_SDHCI_PHY_CONFIG));

	PAD_EMMC_RSTN = readb(cvi_host->pinmuxbase + 0x48) & 0x07;
	PAD_EMMC_CLK  = readb(cvi_host->pinmuxbase + 0x50) & 0x07;
	PAD_EMMC_CMD  = readb(cvi_host->pinmuxbase + 0x5C) & 0x07;
	PAD_EMMC_DAT0 = readb(cvi_host->pinmuxbase + 0x54) & 0x07;
	PAD_EMMC_DAT1 = readb(cvi_host->pinmuxbase + 0x60) & 0x07;
	PAD_EMMC_DAT2 = readb(cvi_host->pinmuxbase + 0x4C) & 0x07;
	PAD_EMMC_DAT3 = readb(cvi_host->pinmuxbase + 0x58) & 0x07;
	REG_EMMC_RSTN = readb(cvi_host->pinmuxbase + 0x914);
	REG_EMMC_CLK  = readb(cvi_host->pinmuxbase + 0x91D);
	REG_EMMC_CMD  = readb(cvi_host->pinmuxbase + 0x928);
	REG_EMMC_DAT0 = readb(cvi_host->pinmuxbase + 0x920);
	REG_EMMC_DAT1 = readb(cvi_host->pinmuxbase + 0x92D);
	REG_EMMC_DAT2 = readb(cvi_host->pinmuxbase + 0x918);
	REG_EMMC_DAT3 = readb(cvi_host->pinmuxbase + 0x924);

	SDHCI_DUMP(": PAD_EMMC_RSTN:0x%02x PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_EMMC_RSTN, (REG_EMMC_RSTN & 0x04)>>2, (REG_EMMC_RSTN & 0x08)>>3,
		(REG_EMMC_RSTN & 0x80)>>7, (REG_EMMC_RSTN & 0x40)>>6, (REG_EMMC_RSTN & 0x20)>>5);
	SDHCI_DUMP(": PAD_EMMC_CLK:0x%02x  PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_EMMC_CLK, (REG_EMMC_CLK & 0x04)>>2, (REG_EMMC_CLK & 0x08)>>3,
		(REG_EMMC_CLK & 0x80)>>7, (REG_EMMC_CLK & 0x40)>>6, (REG_EMMC_CLK & 0x20)>>5);
	SDHCI_DUMP(": PAD_EMMC_CMD:0x%02x  PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_EMMC_CMD, (REG_EMMC_CMD & 0x04)>>2, (REG_EMMC_CMD & 0x08)>>3,
		(REG_EMMC_CMD & 0x80)>>7, (REG_EMMC_CMD & 0x40)>>6, (REG_EMMC_CMD & 0x20)>>5);
	SDHCI_DUMP(": PAD_EMMC_DAT0:0x%02x PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_EMMC_DAT0, (REG_EMMC_DAT0 & 0x04)>>2, (REG_EMMC_DAT0 & 0x08)>>3,
		(REG_EMMC_DAT0 & 0x80)>>7, (REG_EMMC_DAT0 & 0x40)>>6, (REG_EMMC_DAT0 & 0x20)>>5);
	SDHCI_DUMP(": PAD_EMMC_DAT1:0x%02x PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_EMMC_DAT1, (REG_EMMC_DAT1 & 0x04)>>2, (REG_EMMC_DAT1 & 0x08)>>3,
		(REG_EMMC_DAT1 & 0x80)>>7, (REG_EMMC_DAT1 & 0x40)>>6, (REG_EMMC_DAT1 & 0x20)>>5);
	SDHCI_DUMP(": PAD_EMMC_DAT2:0x%02x PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_EMMC_DAT2, (REG_EMMC_DAT2 & 0x04)>>2, (REG_EMMC_DAT2 & 0x08)>>3,
		(REG_EMMC_DAT2 & 0x80)>>7, (REG_EMMC_DAT2 & 0x40)>>6, (REG_EMMC_DAT2 & 0x20)>>5);
	SDHCI_DUMP(": PAD_EMMC_DAT3:0x%02x PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_EMMC_DAT3, (REG_EMMC_DAT3 & 0x04)>>2, (REG_EMMC_DAT3 & 0x08)>>3,
		(REG_EMMC_DAT3 & 0x80)>>7, (REG_EMMC_DAT3 & 0x40)>>6, (REG_EMMC_DAT3 & 0x20)>>5);

	clk_source_select = (readb(cvi_host->clkgenbase + 0x20) & 0x20) >> 5;

	SDHCI_DUMP(": clk_emmc enable[16]:0x%08x\n", readl(cvi_host->clkgenbase));
	SDHCI_DUMP(": clk_emmc source_select:%u\n", clk_source_select);
	if (clk_source_select == 0) {
		SDHCI_DUMP(": clk_emmc REG:0x03002068 = 0x%08x\n",
		readl(cvi_host->clkgenbase + 0x68));
		if (readl(cvi_host->clkgenbase + 0x68) == 0x00000001)
			SDHCI_DUMP(": clk_emmc %d MHz\n", DISPPLL_MHZ/12);
	} else if (clk_source_select == 1) {
		SDHCI_DUMP(": clk_emmc REG:0x03002064 = 0x%08x\n",
		readl(cvi_host->clkgenbase + 0x64));
		if (readl(cvi_host->clkgenbase + 0x64) == 0x00040009)
			SDHCI_DUMP(": clk_emmc %d MHz\n", FPLL_MHZ/4);
	}
}

static void sdhci_cv181x_sd_dump_vendor_regs(struct sdhci_host *host)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);
	u8 clk_source_select = 0;
	u8 PAD_SDIO0_PWR_EN = 0;
	u8 PAD_SDIO0_CD  = 0;
	u8 PAD_SDIO0_CLK = 0;
	u8 PAD_SDIO0_CMD = 0;
	u8 PAD_SDIO0_D0  = 0;
	u8 PAD_SDIO0_D1  = 0;
	u8 PAD_SDIO0_D2  = 0;
	u8 PAD_SDIO0_D3  = 0;
	u8 REG_SDIO0_PWR_EN = 0;
	u8 REG_SDIO0_CD  = 0;
	u8 REG_SDIO0_CLK = 0;
	u8 REG_SDIO0_CMD = 0;
	u8 REG_SDIO0_D0  = 0;
	u8 REG_SDIO0_D1  = 0;
	u8 REG_SDIO0_D2  = 0;
	u8 REG_SDIO0_D3  = 0;

	SDHCI_DUMP(": Reg_200:   0x%08x | Reg_240:  0x%08x\n",
		   sdhci_readl(host, CVI_CV181X_SDHCI_VENDOR_MSHC_CTRL_R),
		   sdhci_readl(host, CVI_CV181X_SDHCI_PHY_TX_RX_DLY));
	SDHCI_DUMP(": Reg_244:   0x%08x | Reg_248:  0x%08x\n",
		   sdhci_readl(host, CVI_CV181X_SDHCI_PHY_DS_DLY),
		   sdhci_readw(host, CVI_CV181X_SDHCI_PHY_DLY_STS));
	SDHCI_DUMP(": Reg_24C:   0x%08x | unplugg:  0x%08x\n",
		   sdhci_readl(host, CVI_CV181X_SDHCI_PHY_CONFIG),
		   host->mmc->ever_unplugged);

	PAD_SDIO0_PWR_EN = readb(cvi_host->pinmuxbase + 0x38) & 0x07;
	PAD_SDIO0_CD  = readb(cvi_host->pinmuxbase + 0x34) & 0x07;
	PAD_SDIO0_CLK = readb(cvi_host->pinmuxbase + 0x1C) & 0x07;
	PAD_SDIO0_CMD = readb(cvi_host->pinmuxbase + 0x20) & 0x07;
	PAD_SDIO0_D0  = readb(cvi_host->pinmuxbase + 0x24) & 0x07;
	PAD_SDIO0_D1  = readb(cvi_host->pinmuxbase + 0x28) & 0x07;
	PAD_SDIO0_D2  = readb(cvi_host->pinmuxbase + 0x2C) & 0x07;
	PAD_SDIO0_D3  = readb(cvi_host->pinmuxbase + 0x30) & 0x07;
	REG_SDIO0_PWR_EN = readb(cvi_host->pinmuxbase + 0x904);
	REG_SDIO0_CD  = readb(cvi_host->pinmuxbase + 0x900);
	REG_SDIO0_CLK = readb(cvi_host->pinmuxbase + 0xA00);
	REG_SDIO0_CMD = readb(cvi_host->pinmuxbase + 0xA04);
	REG_SDIO0_D0  = readb(cvi_host->pinmuxbase + 0xA08);
	REG_SDIO0_D1  = readb(cvi_host->pinmuxbase + 0xA0C);
	REG_SDIO0_D2  = readb(cvi_host->pinmuxbase + 0xA10);
	REG_SDIO0_D3  = readb(cvi_host->pinmuxbase + 0xA14);

	SDHCI_DUMP(": PAD_SDIO0_PWR_EN:0x%02x PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_SDIO0_PWR_EN, (REG_SDIO0_PWR_EN & 0x04)>>2, (REG_SDIO0_PWR_EN & 0x08)>>3,
		(REG_SDIO0_PWR_EN & 0x80)>>7, (REG_SDIO0_PWR_EN & 0x40)>>6, (REG_SDIO0_PWR_EN & 0x20)>>5);
	SDHCI_DUMP(": PAD_SDIO0_CD:0x%02x  PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_SDIO0_CD, (REG_SDIO0_CD & 0x04)>>2, (REG_SDIO0_CD & 0x08)>>3,
		(REG_SDIO0_CD & 0x80)>>7, (REG_SDIO0_CD & 0x40)>>6, (REG_SDIO0_CD & 0x20)>>5);
	SDHCI_DUMP(": PAD_SDIO0_CLK:0x%02x PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_SDIO0_CLK, (REG_SDIO0_CLK & 0x04)>>2, (REG_SDIO0_CLK & 0x08)>>3,
		(REG_SDIO0_CLK & 0x80)>>7, (REG_SDIO0_CLK & 0x40)>>6, (REG_SDIO0_CLK & 0x20)>>5);
	SDHCI_DUMP(": PAD_SDIO0_CMD:0x%02x PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_SDIO0_CMD, (REG_SDIO0_CMD & 0x04)>>2, (REG_SDIO0_CMD & 0x08)>>3,
		(REG_SDIO0_CMD & 0x80)>>7, (REG_SDIO0_CMD & 0x40)>>6, (REG_SDIO0_CMD & 0x20)>>5);
	SDHCI_DUMP(": PAD_SDIO0_D0:0x%02x  PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_SDIO0_D0, (REG_SDIO0_D0 & 0x04)>>2, (REG_SDIO0_D0 & 0x08)>>3,
		(REG_SDIO0_D0 & 0x80)>>7, (REG_SDIO0_D0 & 0x40)>>6, (REG_SDIO0_D0 & 0x20)>>5);
	SDHCI_DUMP(": PAD_SDIO0_D1:0x%02x  PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_SDIO0_D1, (REG_SDIO0_D1 & 0x04)>>2, (REG_SDIO0_D1 & 0x08)>>3,
		(REG_SDIO0_D1 & 0x80)>>7, (REG_SDIO0_D1 & 0x40)>>6, (REG_SDIO0_D1 & 0x20)>>5);
	SDHCI_DUMP(": PAD_SDIO0_D2:0x%02x  PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_SDIO0_D2, (REG_SDIO0_D2 & 0x04)>>2, (REG_SDIO0_D2 & 0x08)>>3,
		(REG_SDIO0_D2 & 0x80)>>7, (REG_SDIO0_D2 & 0x40)>>6, (REG_SDIO0_D2 & 0x20)>>5);
	SDHCI_DUMP(": PAD_SDIO0_D3:0x%02x  PU:%u PD:%u DS[2:0]:%u%u%u\n",
		PAD_SDIO0_D3, (REG_SDIO0_D3 & 0x04)>>2, (REG_SDIO0_D3 & 0x08)>>3,
		(REG_SDIO0_D3 & 0x80)>>7, (REG_SDIO0_D3 & 0x40)>>6, (REG_SDIO0_D3 & 0x20)>>5);

	clk_source_select = (readb(cvi_host->clkgenbase + 0x20) & 0x40) >> 6;

	SDHCI_DUMP(": clk_sd0 enable[19]:0x%08x\n", readl(cvi_host->clkgenbase));
	SDHCI_DUMP(": clk_sd0 source_select:%u\n", clk_source_select);
	if (clk_source_select == 0) {
		SDHCI_DUMP(": clk_sd0 REG:0x03002074 = 0x%08x\n",
		readl(cvi_host->clkgenbase + 0x74));
		if (readl(cvi_host->clkgenbase + 0x74) == 0x00000001)
			SDHCI_DUMP(": clk_sd0 %d MHz\n", DISPPLL_MHZ/12);
	} else if (clk_source_select == 1) {
		SDHCI_DUMP(": clk_sd0 REG:0x03002070 = 0x%08x\n",
		readl(cvi_host->clkgenbase + 0x70));
		if (readl(cvi_host->clkgenbase + 0x70) == 0x00040009)
			SDHCI_DUMP(": clk_sd0 %d MHz\n", FPLL_MHZ/4);
	}
}

static void cvi_adma_write_desc(struct sdhci_host *host, void **desc,
		dma_addr_t addr, int len, unsigned int cmd)
{
	int tmplen, offset;

	if (likely(!len || BOUNDARY_OK(addr, len))) {
		sdhci_adma_write_desc(host, desc, addr, len, cmd);
		return;
	}

	offset = addr & (SZ_128M - 1);
	tmplen = SZ_128M - offset;
	sdhci_adma_write_desc(host, desc, addr, tmplen, cmd);

	addr += tmplen;
	len -= tmplen;
	sdhci_adma_write_desc(host, desc, addr, len, cmd);
}

static const struct sdhci_ops sdhci_cv181x_emmc_ops = {
	.reset = sdhci_cv181x_emmc_reset,
	.set_clock = sdhci_set_clock,
	.set_bus_width = sdhci_set_bus_width,
	.get_max_clock = sdhci_cvi_general_get_max_clock,
	.voltage_switch = sdhci_cvi_emmc_voltage_switch,
	.set_uhs_signaling = sdhci_cvi_general_set_uhs_signaling,
	.platform_execute_tuning = sdhci_cv181x_general_execute_tuning,
	.select_drive_strength = sdhci_cv181x_general_select_drive_strength,
	.dump_vendor_regs = sdhci_cv181x_emmc_dump_vendor_regs,
	.adma_write_desc = cvi_adma_write_desc,
};

static const struct sdhci_ops sdhci_cv181x_sd_ops = {
	.reset = sdhci_cv181x_sd_reset,
	.set_clock = sdhci_set_clock,
	.set_power = sdhci_cv181x_sd_set_power,
	.set_bus_width = sdhci_set_bus_width,
	.get_max_clock = sdhci_cvi_general_get_max_clock,
	.voltage_switch = sdhci_cv181x_sd_voltage_switch,
	.set_uhs_signaling = sdhci_cvi_general_set_uhs_signaling,
	.platform_execute_tuning = sdhci_cv181x_general_execute_tuning,
	.select_drive_strength = sdhci_cv181x_general_select_drive_strength,
	.dump_vendor_regs = sdhci_cv181x_sd_dump_vendor_regs,
	.adma_write_desc = cvi_adma_write_desc,
};

static const struct sdhci_ops sdhci_cv181x_sdio_ops = {
	.reset = sdhci_cv181x_sdio_reset,
	.set_clock = sdhci_set_clock,
	.set_bus_width = sdhci_set_bus_width,
	.get_max_clock = sdhci_cvi_general_get_max_clock,
	.voltage_switch = sdhci_cv181x_sd_voltage_switch,
	.set_uhs_signaling = sdhci_cvi_general_set_uhs_signaling,
	.select_drive_strength = sdhci_cv181x_general_select_drive_strength,
	.platform_execute_tuning = sdhci_cv181x_general_execute_tuning,
	.adma_write_desc = cvi_adma_write_desc,
};

static const struct sdhci_ops sdhci_cv181x_fpga_emmc_ops = {
	.reset = sdhci_cv181x_sd_reset,
	.set_clock = sdhci_set_clock,
	.set_bus_width = sdhci_set_bus_width,
	.get_max_clock = sdhci_cvi_general_get_max_clock,
	.voltage_switch = sdhci_cvi_emmc_voltage_switch,
	.set_uhs_signaling = sdhci_cvi_general_set_uhs_signaling,
	.platform_execute_tuning = sdhci_cv181x_general_execute_tuning,
	.select_drive_strength = sdhci_cv181x_general_select_drive_strength,
	.dump_vendor_regs = sdhci_cv181x_emmc_dump_vendor_regs,
};

static const struct sdhci_ops sdhci_cv181x_fpga_sd_ops = {
	.reset = sdhci_cv181x_sd_reset,
	.set_clock = sdhci_set_clock,
	.set_power = sdhci_cv181x_sd_set_power,
	.set_bus_width = sdhci_set_bus_width,
	.get_max_clock = sdhci_cvi_general_get_max_clock,
	.voltage_switch = sdhci_cv181x_sd_voltage_switch,
	.set_uhs_signaling = sdhci_cvi_general_set_uhs_signaling,
	.platform_execute_tuning = sdhci_cv181x_general_execute_tuning,
	.select_drive_strength = sdhci_cv181x_general_select_drive_strength,
	.dump_vendor_regs = sdhci_cv181x_sd_dump_vendor_regs,
};

static const struct sdhci_pltfm_data sdhci_cv181x_emmc_pdata = {
	.ops = &sdhci_cv181x_emmc_ops,
	.quirks = SDHCI_QUIRK_INVERTED_WRITE_PROTECT | SDHCI_QUIRK_CAP_CLOCK_BASE_BROKEN,
	.quirks2 = SDHCI_QUIRK2_PRESET_VALUE_BROKEN,
};

static const struct sdhci_pltfm_data sdhci_cv181x_sd_pdata = {
	.ops = &sdhci_cv181x_sd_ops,
	.quirks = SDHCI_QUIRK_INVERTED_WRITE_PROTECT | SDHCI_QUIRK_CAP_CLOCK_BASE_BROKEN,
	.quirks2 = SDHCI_QUIRK2_PRESET_VALUE_BROKEN,
};

static const struct sdhci_pltfm_data sdhci_cv181x_sdio_pdata = {
	.ops = &sdhci_cv181x_sdio_ops,
	.quirks = SDHCI_QUIRK_INVERTED_WRITE_PROTECT | SDHCI_QUIRK_CAP_CLOCK_BASE_BROKEN,
	.quirks2 = SDHCI_QUIRK2_PRESET_VALUE_BROKEN,
};

static const struct sdhci_pltfm_data sdhci_cv181x_fpga_emmc_pdata = {
	.ops = &sdhci_cv181x_fpga_emmc_ops,
	.quirks = SDHCI_QUIRK_INVERTED_WRITE_PROTECT | SDHCI_QUIRK_CAP_CLOCK_BASE_BROKEN,
	.quirks2 = SDHCI_QUIRK2_PRESET_VALUE_BROKEN | SDHCI_QUIRK2_BROKEN_HS200,
};

static const struct sdhci_pltfm_data sdhci_cv181x_fpga_sd_pdata = {
	.ops = &sdhci_cv181x_fpga_sd_ops,
	.quirks = SDHCI_QUIRK_INVERTED_WRITE_PROTECT | SDHCI_QUIRK_CAP_CLOCK_BASE_BROKEN,
	.quirks2 = SDHCI_QUIRK2_PRESET_VALUE_BROKEN | SDHCI_QUIRK2_NO_1_8_V,
};

static const struct of_device_id sdhci_cvi_dt_match[] = {
	{.compatible = "cvitek,cv181x-fpga-emmc", .data = &sdhci_cv181x_fpga_emmc_pdata},
	{.compatible = "cvitek,cv181x-fpga-sd", .data = &sdhci_cv181x_fpga_sd_pdata},
	{.compatible = "cvitek,cv181x-emmc", .data = &sdhci_cv181x_emmc_pdata},
	{.compatible = "cvitek,cv181x-sd", .data = &sdhci_cv181x_sd_pdata},
	{.compatible = "cvitek,cv181x-sdio", .data = &sdhci_cv181x_sdio_pdata},

	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, sdhci_cvi_dt_match);

static unsigned long sdhci_get_time_ms(void)
{
	ktime_t cur;

	cur = ktime_get();
	// Get milliseconds
	return ktime_to_ms(cur);
}

static void sdhci_cvi_cd_debounce_work(struct work_struct *work)
{
	struct sdhci_cvi_host *cvi_host = container_of(work, struct sdhci_cvi_host,
						  cd_debounce_work.work);
	struct mmc_host *host = cvi_host->mmc;
	unsigned long start_time = sdhci_get_time_ms();
	int pre_gpio_cd;
	unsigned long flag;

	spin_lock_irqsave(&cvi_host->cd_debounce_lock, flag);
	pre_gpio_cd = cvi_host->pre_gpio_cd;
	cvi_host->is_debounce_work_running = true;
	spin_unlock_irqrestore(&cvi_host->cd_debounce_lock, flag);

	while (1) {
		if ((sdhci_get_time_ms() - start_time) >= SDHCI_GPIO_CD_DEBOUNCE_TIME) {
			if (pre_gpio_cd == mmc_gpio_get_cd(host)) {
				host->ops->card_event(host);
				mmc_detect_change(host, msecs_to_jiffies(SDHCI_GPIO_CD_DEBOUNCE_DELAY_TIME));
				break;
			}
			pre_gpio_cd = mmc_gpio_get_cd(host);
			start_time = sdhci_get_time_ms();
		}
	}

	spin_lock_irqsave(&cvi_host->cd_debounce_lock, flag);
	cvi_host->is_debounce_work_running = false;
	spin_unlock_irqrestore(&cvi_host->cd_debounce_lock, flag);
}

static irqreturn_t sdhci_cvi_cd_handler(int irq, void *dev_id)
{
	/* Schedule a card detection after a debounce timeout */
	struct mmc_host *host = dev_id;
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(mmc_priv(host));
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);
	unsigned long flag;

	spin_lock_irqsave(&cvi_host->cd_debounce_lock, flag);
	cvi_host->pre_gpio_cd = mmc_gpio_get_cd(host);
	if (!cvi_host->pre_gpio_cd)
		host->ever_unplugged = true;
	if (!cvi_host->is_debounce_work_running) {
		cancel_delayed_work(&cvi_host->cd_debounce_work);
		schedule_delayed_work(&cvi_host->cd_debounce_work, 0);
	}
	spin_unlock_irqrestore(&cvi_host->cd_debounce_lock, flag);

	return IRQ_HANDLED;
}

static int sdhci_cvi_probe(struct platform_device *pdev)
{
	struct sdhci_host *host;
	struct sdhci_pltfm_host *pltfm_host;
	struct sdhci_cvi_host *cvi_host;
	const struct of_device_id *match;
	const struct sdhci_pltfm_data *pdata;
	struct clk *clk_sd;
	int ret;
	int gpio_cd = -EINVAL;
	u32 extra;

	pr_info(DRIVER_NAME ":%s\n", __func__);

	match = of_match_device(sdhci_cvi_dt_match, &pdev->dev);
	if (!match)
		return -EINVAL;

	pdata = match->data;

	host = sdhci_pltfm_init(pdev, pdata, sizeof(*cvi_host));
	if (IS_ERR(host))
		return PTR_ERR(host);

	pltfm_host = sdhci_priv(host);
	cvi_host = sdhci_pltfm_priv(pltfm_host);
	cvi_host->host = host;
	cvi_host->mmc = host->mmc;
	cvi_host->pdev = pdev;
	cvi_host->core_mem = host->ioaddr;
	cvi_host->topbase = ioremap(TOP_BASE, 0x2000);
	cvi_host->pinmuxbase = ioremap(PINMUX_BASE, 0x1000);
	cvi_host->clkgenbase = ioremap(CLKGEN_BASE, 0x100);

	sdhci_cv181x_sd_voltage_restore(host, false);

	ret = mmc_of_parse(host->mmc);
	if (ret)
		goto pltfm_free;

	sdhci_get_of_property(pdev);

	if (pdata->ops->hw_reset) {
		cvi_host->reset = devm_reset_control_get(&pdev->dev, "sdio");
		if (IS_ERR(cvi_host->reset)) {
			ret = PTR_ERR(cvi_host->reset);
			goto pltfm_free;
		}
	}

	if (pdev->dev.of_node) {
		gpio_cd = of_get_named_gpio(pdev->dev.of_node, "cvi-cd-gpios", 0);
	}

	if (gpio_is_valid(gpio_cd)) {
		cvi_host->cvi_gpio = devm_kzalloc(&cvi_host->pdev->dev,
					sizeof(struct mmc_gpio), GFP_KERNEL);
		if (cvi_host->cvi_gpio) {
			cvi_host->cvi_gpio->cd_gpio_isr = sdhci_cvi_cd_handler;
			cvi_host->cvi_gpio->cd_debounce_delay_ms = SDHCI_GPIO_CD_DEBOUNCE_DELAY_TIME;
			cvi_host->cvi_gpio->cd_label = devm_kzalloc(&cvi_host->pdev->dev,
						strlen("cd-gpio-irq") + 1, GFP_KERNEL);
			strcpy(cvi_host->cvi_gpio->cd_label, "cd-gpio-irq");
			host->mmc->slot.handler_priv = cvi_host->cvi_gpio;
			ret = mmc_gpiod_request_cd(host->mmc, "cvi-cd",
					0, false, SDHCI_GPIO_CD_DEBOUNCE_TIME);
			if (ret) {
				pr_err("card detect request cd failed: %d\n", ret);
			} else {
				writeb(0x3, cvi_host->pinmuxbase + 0x34);
				INIT_DELAYED_WORK(&cvi_host->cd_debounce_work, sdhci_cvi_cd_debounce_work);
				spin_lock_init(&cvi_host->cd_debounce_lock);
				mmc_gpiod_request_cd_irq(host->mmc);
			}
		}
	}
	/*
	 * extra adma table cnt for cross 128M boundary handling.
	 */
	extra = DIV_ROUND_UP_ULL(dma_get_required_mask(&pdev->dev), SZ_128M);
	if (extra > SDHCI_MAX_SEGS)
		extra = SDHCI_MAX_SEGS;
	host->adma_table_cnt += extra;

	ret = sdhci_add_host(host);
	if (ret)
		goto err_add_host;

	platform_set_drvdata(pdev, cvi_host);

	if (strstr(dev_name(mmc_dev(host->mmc)), "wifi-sd"))
		wifi_mmc = host->mmc;
	else
		wifi_mmc = NULL;

	/* device proc entry */
	if ((!proc_cvi_dir) &&
		(strstr(dev_name(mmc_dev(host->mmc)), "cv-sd"))) {
		ret = cvi_proc_init(cvi_host);
		if (ret)
			pr_err("device proc init is failed!");
	}

	if (strstr(dev_name(mmc_dev(host->mmc)), "cv-emmc"))
		sdhci_cv181x_emmc_setup_pad(host);

	return 0;

err_add_host:
pltfm_free:
	sdhci_pltfm_free(pdev);
	return ret;
}

static int sdhci_cvi_remove(struct platform_device *pdev)
{
	struct sdhci_host *host = platform_get_drvdata(pdev);
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_cvi_host *cvi_host = sdhci_pltfm_priv(pltfm_host);
	int dead = (readl_relaxed(host->ioaddr + SDHCI_INT_STATUS) == 0xffffffff);

	sdhci_remove_host(host, dead);
	sdhci_pltfm_free(pdev);

	cvi_proc_shutdown(cvi_host);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
#ifdef CONFIG_ARCH_CV181X_ASIC

static int save_rtc_reg(struct sdhci_cvi_host *cvi_host)
{
	void __iomem *topbase;
	void __iomem *rtcbase;

	topbase = ioremap(TOP_BASE, 0x250);
	rtcbase = ioremap(RTC_CTRL_BASE, 0x80);

	if (!cvi_host->rtc_reg_ctx) {
		cvi_host->rtc_reg_ctx = devm_kzalloc(&cvi_host->pdev->dev,
								sizeof(struct cvi_rtc_sdhci_reg_context), GFP_KERNEL);
		if (!cvi_host->rtc_reg_ctx)
			return -ENOMEM;
	}
	cvi_host->rtc_reg_ctx->rtcsys_clkmux = readl(rtcbase + RTCSYS_CLKMUX);
	cvi_host->rtc_reg_ctx->rtcsys_clkbyp = readl(rtcbase + RTCSYS_CLKBYP);
	cvi_host->rtc_reg_ctx->rtcsys_mcu51_ictrl1 = readl(rtcbase + RTCSYS_MCU51_ICTRL1);
	cvi_host->rtc_reg_ctx->rtcsys_ctrl = readl(topbase + RTCSYS_CTRL);

	iounmap(topbase);
	iounmap(rtcbase);

	return 0;
}

static void restore_rtc_reg(struct sdhci_cvi_host *cvi_host)
{
	void __iomem *topbase;
	void __iomem *rtcbase;

	topbase = ioremap(TOP_BASE, 0x250);
	rtcbase = ioremap(RTC_CTRL_BASE, 0x80);

	writel(cvi_host->rtc_reg_ctx->rtcsys_clkmux, rtcbase + RTCSYS_CLKMUX);
	writel(cvi_host->rtc_reg_ctx->rtcsys_clkbyp, rtcbase + RTCSYS_CLKBYP);
	writel(cvi_host->rtc_reg_ctx->rtcsys_mcu51_ictrl1, rtcbase + RTCSYS_MCU51_ICTRL1);
	writel(cvi_host->rtc_reg_ctx->rtcsys_ctrl, topbase + RTCSYS_CTRL);

	iounmap(topbase);
	iounmap(rtcbase);
}
#else
static int save_rtc_reg(struct sdhci_cvi_host *cvi_host)
{
	return 0;
}
static void restore_rtc_reg(struct sdhci_cvi_host *cvi_host) {}
#endif

static void save_reg(struct sdhci_host *host, struct sdhci_cvi_host *cvi_host)
{
	save_rtc_reg(cvi_host);
	cvi_host->reg_ctrl2 = sdhci_readl(host, SDHCI_HOST_CONTROL2);
	cvi_host->reg_clk_ctrl = sdhci_readl(host, SDHCI_CLOCK_CONTROL);
	cvi_host->reg_host_ctrl = sdhci_readl(host, SDHCI_HOST_CONTROL);
}

static void restore_reg(struct sdhci_host *host, struct sdhci_cvi_host *cvi_host)
{
	restore_rtc_reg(cvi_host);
	sdhci_writel(host, host->ier, SDHCI_INT_ENABLE);
	sdhci_writel(host, host->ier, SDHCI_SIGNAL_ENABLE);
	sdhci_writel(host, cvi_host->reg_ctrl2, SDHCI_HOST_CONTROL2);
	sdhci_writel(host, cvi_host->reg_clk_ctrl, SDHCI_CLOCK_CONTROL);
	sdhci_writel(host, cvi_host->reg_host_ctrl, SDHCI_HOST_CONTROL);
}

static int sdhci_cvi_suspend(struct device *dev)
{
	struct sdhci_cvi_host *cvi_host = dev_get_drvdata(dev);
	struct sdhci_host *host = cvi_host->host;

	if (!host)
		return 0;

	save_reg(host, cvi_host);

	return 0;
}

static int sdhci_cvi_resume(struct device *dev)
{
	struct sdhci_cvi_host *cvi_host = dev_get_drvdata(dev);
	struct sdhci_host *host = cvi_host->host;

	if (!host)
		return 0;

	restore_reg(host, cvi_host);

	return 0;
}

#endif

static const struct dev_pm_ops sdhci_cvi_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(sdhci_cvi_suspend, sdhci_cvi_resume)
};

static struct platform_driver sdhci_cvi_driver = {
	.probe = sdhci_cvi_probe,
	.remove = sdhci_cvi_remove,
	.driver = {
		.name = DRIVER_NAME,
		.pm = &sdhci_cvi_pm_ops,
		.of_match_table = sdhci_cvi_dt_match,
	},
};

module_platform_driver(sdhci_cvi_driver);

MODULE_DESCRIPTION("Cvitek Secure Digital Host Controller Interface driver");
MODULE_LICENSE("GPL v2");
