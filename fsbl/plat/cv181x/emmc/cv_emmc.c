/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <cpu.h>
#include <debug.h>
#include <delay_timer.h>
#include <emmc.h>
#include <errno.h>
#include <mmio.h>
#include <platform.h>
#include <string.h>
#include <utils.h>
#include <cv_emmc.h>
#include <platform.h>

#define EMMC_INIT_FREQ (200 * 1000)
#ifdef SUPPORT_SD_EMMC_CLOCK_ADJUSTMENT
#define EMMC_TRAN_FREQ (emmc_tran_freq)
#else
#define EMMC_TRAN_FREQ (12 * 1000 * 1000)
#endif

static void bm_emmc_hw_init(void);
static void bm_emmc_dev_reset(void);
static int bm_emmc_send_cmd(emmc_cmd_t *cmd);
static int bm_emmc_set_ios(int clk, int width);
static int bm_emmc_prepare(int lba, uintptr_t buf, size_t size);
static int bm_emmc_read(int lba, uintptr_t buf, size_t size);
static int bm_emmc_write(int lba, uintptr_t buf, size_t size);

static const emmc_ops_t bm_emmc_ops = {
	.init = bm_emmc_hw_init,
	.reset = bm_emmc_dev_reset,
	.send_cmd = bm_emmc_send_cmd,
	.set_ios = bm_emmc_set_ios,
	.prepare = bm_emmc_prepare,
	.read = bm_emmc_read,
	.write = bm_emmc_write,
};

static bm_emmc_params_t bm_params = { .reg_base = EMMC_BASE,
				      .clk_rate = 375 * 1000 * 1000,
				      .bus_width = EMMC_BUS_WIDTH,
				      .flags = 0 };

#ifdef SUPPORT_SD_EMMC_CLOCK_ADJUSTMENT
const uint32_t emmc_tran_freq = 12 * 1000 * 1000;
#endif

static int bm_emmc_send_cmd_with_data(emmc_cmd_t *cmd)
{
	uintptr_t base;
	uint32_t mode = 0, stat, dma_addr, flags = 0;
	uint32_t emmc_timer = 0;

	base = bm_params.reg_base;

	//make sure cmd line is clear
	while (1) {
		if (!(mmio_read_32(base + SDHCI_PSTATE) & SDHCI_CMD_INHIBIT))
			break;
	}

	switch (cmd->cmd_idx) {
	case EMMC_CMD17:
	case EMMC_CMD18:
		mode = SDHCI_TRNS_DMA | SDHCI_TRNS_BLK_CNT_EN | SDHCI_TRNS_MULTI | SDHCI_TRNS_READ;
		break;
	case EMMC_CMD24:
	case EMMC_CMD25:
		mode = (SDHCI_TRNS_DMA | SDHCI_TRNS_BLK_CNT_EN | SDHCI_TRNS_MULTI) & ~SDHCI_TRNS_READ;
		break;
	default:
		ERROR("%s Invalid CMD: %d\n", __func__, cmd->cmd_idx);
		return -EINVAL;
	}

	mmio_write_16(base + SDHCI_TRANSFER_MODE, mode);
	mmio_write_32(base + SDHCI_ARGUMENT, cmd->cmd_arg);

	// set cmd flags
	if (cmd->cmd_idx == 0)
		flags |= SDHCI_CMD_RESP_NONE;
	else if (cmd->resp_type & EMMC_RESPONSE_R2)
		flags |= SDHCI_CMD_RESP_LONG;
	else
		flags |= SDHCI_CMD_RESP_SHORT;

	flags |= SDHCI_CMD_CRC;
	flags |= SDHCI_CMD_INDEX;
	flags |= SDHCI_CMD_DATA;

	// issue the cmd
	mmio_write_16(base + SDHCI_COMMAND, SDHCI_MAKE_CMD(cmd->cmd_idx, flags));

	// check cmd complete if necessary
	if ((mmio_read_16(base + SDHCI_TRANSFER_MODE) & SDHCI_TRNS_RESP_INT) == 0) {
		emmc_timer = get_timer(0);
		while (1) {
			stat = mmio_read_16(base + SDHCI_INT_STATUS);
			if (stat & SDHCI_INT_ERROR) {
				ERROR("%s CMD: %u INT_STAT: 0x%04x INT_ERR: 0x%04x\n", __func__, cmd->cmd_idx,
				      mmio_read_16(base + SDHCI_INT_STATUS), mmio_read_16(base + SDHCI_ERR_INT_STATUS));
				return -EIO;
			}
			if (stat & SDHCI_INT_CMD_COMPLETE) {
				mmio_write_16(base + SDHCI_INT_STATUS, stat | SDHCI_INT_CMD_COMPLETE);
				break;
			}
			if (get_timer(emmc_timer) > EMMC_CMD_TIMEOUT) {
				ERROR("%s send CMD %u timeout!\n", __func__, cmd->cmd_idx);
				return -EIO;
			}
		}

		// get cmd respond
		if (!(flags & SDHCI_CMD_RESP_NONE))
			cmd->resp_data[0] = mmio_read_32(base + SDHCI_RESPONSE_01);
		if (flags & SDHCI_CMD_RESP_LONG) {
			cmd->resp_data[1] = mmio_read_32(base + SDHCI_RESPONSE_23);
			cmd->resp_data[2] = mmio_read_32(base + SDHCI_RESPONSE_45);
			cmd->resp_data[3] = mmio_read_32(base + SDHCI_RESPONSE_67);
		}
	}

	// check dma/transfer complete
	emmc_timer = get_timer(0);
	while (1) {
		stat = mmio_read_16(base + SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			ERROR("%s CMD: %u INT_STAT: 0x%04x INT_ERR: 0x%04x\n", __func__, cmd->cmd_idx,
			      mmio_read_16(base + SDHCI_INT_STATUS), mmio_read_16(base + SDHCI_ERR_INT_STATUS));
			return -EIO;
		}

		if (stat & SDHCI_INT_XFER_COMPLETE) {
			mmio_write_16(base + SDHCI_INT_STATUS, stat);
			break;
		}

		if (get_timer(emmc_timer) > EMMC_DATA_CMD_TIMEOUT) {
			ERROR("%s DATA_Transfer %u timeout!\n", __func__, cmd->cmd_idx);
			return -EIO;
		}

		if (stat & SDHCI_INT_DMA_END) {
			mmio_write_16(base + SDHCI_INT_STATUS, stat);
			if (mmio_read_16(base + SDHCI_HOST_CONTROL2) & SDHCI_HOST_VER4_ENABLE) {
				dma_addr = mmio_read_32(base + SDHCI_ADMA_SA_LOW);
				mmio_write_32(base + SDHCI_ADMA_SA_LOW, dma_addr);
				mmio_write_32(base + SDHCI_ADMA_SA_HIGH, 0);
			} else {
				dma_addr = mmio_read_32(base + SDHCI_DMA_ADDRESS);
				mmio_write_32(base + SDHCI_DMA_ADDRESS, dma_addr);
			}
		}
	}

	return 0;
}

static int bm_emmc_send_cmd_without_data(emmc_cmd_t *cmd)
{
	uintptr_t base;
	uint32_t stat, flags = 0x0;
	uint32_t emmc_timer = 0;

	base = bm_params.reg_base;

	//make sure cmd line is clear
	while (1) {
		if (!(mmio_read_32(base + SDHCI_PSTATE) & SDHCI_CMD_INHIBIT))
			break;
	}

	// set cmd flags
	if (cmd->cmd_idx == EMMC_CMD0)
		flags |= SDHCI_CMD_RESP_NONE;
	else if (cmd->cmd_idx == EMMC_CMD1)
		flags |= SDHCI_CMD_RESP_SHORT;
	else if (cmd->cmd_idx == EMMC_CMD6)
		flags |= SDHCI_CMD_RESP_SHORT_BUSY;
	else if (cmd->resp_type & EMMC_RESPONSE_R2) {
		flags |= SDHCI_CMD_RESP_LONG;
		flags |= SDHCI_CMD_CRC;
	} else {
		flags |= SDHCI_CMD_RESP_SHORT;
		flags |= SDHCI_CMD_CRC;
		flags |= SDHCI_CMD_INDEX;
	}

	// make sure dat line is clear if necessary
	if (flags & SDHCI_CMD_RESP_SHORT_BUSY) {
		while (1) {
			if (!(mmio_read_32(base + SDHCI_PSTATE) & SDHCI_CMD_INHIBIT_DAT))
				break;
		}
	}

	// issue the cmd
	mmio_write_32(base + SDHCI_ARGUMENT, cmd->cmd_arg);
	mmio_write_16(base + SDHCI_COMMAND, SDHCI_MAKE_CMD(cmd->cmd_idx, flags));

	// check cmd complete
	emmc_timer = get_timer(0);
	while (1) {
		stat = mmio_read_16(base + SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			ERROR("%s CMD: %u INT_STAT: 0x%04x INT_ERR: 0x%04x\n", __func__, cmd->cmd_idx,
			      mmio_read_16(base + SDHCI_INT_STATUS), mmio_read_16(base + SDHCI_ERR_INT_STATUS));
			return -EIO;
		}
		if (stat & SDHCI_INT_CMD_COMPLETE) {
			mmio_write_16(base + SDHCI_INT_STATUS, stat | SDHCI_INT_CMD_COMPLETE);
			break;
		}
		if (get_timer(emmc_timer) > EMMC_CMD_TIMEOUT) {
			ERROR("%s send CMD %u timeout!\n", __func__, cmd->cmd_idx);
			return -EIO;
		}
	}

	// get cmd respond
	if (!(flags & SDHCI_CMD_RESP_NONE))
		cmd->resp_data[0] = mmio_read_32(base + SDHCI_RESPONSE_01);
	if (flags & SDHCI_CMD_RESP_LONG) {
		cmd->resp_data[1] = mmio_read_32(base + SDHCI_RESPONSE_23);
		cmd->resp_data[2] = mmio_read_32(base + SDHCI_RESPONSE_45);
		cmd->resp_data[3] = mmio_read_32(base + SDHCI_RESPONSE_67);
	}

	return 0;
}

static int bm_emmc_send_cmd(emmc_cmd_t *cmd)
{
#if 0
	INFO("send cmd, cmd_idx=%d, cmd_arg=0x%x, resp_type=0x%x\n",
		cmd->cmd_idx,
		cmd->cmd_arg,
		cmd->resp_type);
#endif

	switch (cmd->cmd_idx) {
	case EMMC_CMD17:
	case EMMC_CMD18:
	case EMMC_CMD24:
	case EMMC_CMD25:
		return bm_emmc_send_cmd_with_data(cmd);
	default:
		return bm_emmc_send_cmd_without_data(cmd);
	}
}

static int bm_emmc_set_ext_csd(unsigned int index, unsigned int value)
{
	emmc_cmd_t cmd;

	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD6;
	cmd.cmd_arg = EXTCSD_WRITE_BYTES | EXTCSD_CMD(index) | EXTCSD_VALUE(value) | 1;

	return bm_emmc_send_cmd(&cmd);
}

int bm_emmc_set_clk(int clk)
{
	int base, div;

	if (clk <= 0) {
		ERROR("%s clk: %d is invalid\n", __func__, clk);
		return -EINVAL;
	}

	if (bm_params.clk_rate <= clk) {
		div = 0;
	} else {
		for (div = 0x1; div < 0xFF; div++) {
			if (bm_params.clk_rate / (2 * div) <= clk)
				break;
		}
	}

	if (div > 0xFF) {
		ERROR("%s div: 0x%x is invalid\n", __func__, div);
		return -EINVAL;
	}

	base = bm_params.reg_base;
	if (mmio_read_16(base + SDHCI_HOST_CONTROL2) & 1 << 15) {
		INFO("Use SDCLK Preset Value\n");
	} else {
		INFO("Set SDCLK by driver. (Source, Target, div, Actual) = (%d, %d, %d, %d)\n", bm_params.clk_rate, clk,
		     div, bm_params.clk_rate / (2 * div));
		mmio_write_16(base + SDHCI_CLK_CTRL,
			      mmio_read_16(base + SDHCI_CLK_CTRL) & ~0x9); // disable INTERNAL_CLK_EN and PLL_ENABLE
		mmio_write_16(base + SDHCI_CLK_CTRL,
			      (mmio_read_16(base + SDHCI_CLK_CTRL) & 0xDF) | div << 8); // set clk div
		mmio_write_16(base + SDHCI_CLK_CTRL,
			      mmio_read_16(base + SDHCI_CLK_CTRL) | 0x1); // set INTERNAL_CLK_EN
		udelay(150);
		if ((mmio_read_16(base + SDHCI_CLK_CTRL) & 0x2) == 0x0) {
			ERROR("eMMC clock seting FAILED!\n");
			return -EIO;
		}
		mmio_write_16(base + SDHCI_CLK_CTRL,
			      mmio_read_16(base + SDHCI_CLK_CTRL) | 0x8); // set PLL_ENABLE
		udelay(150);
		if ((mmio_read_16(base + SDHCI_CLK_CTRL) & 0x2) == 0x0) {
			ERROR("eMMC PLL seting FAILED!\n");
			return -EIO;
		}
	}
	return 0;
}

static void bm_emmc_hw_init(void)
{
	uintptr_t base, vendor_base;

	base = bm_params.reg_base;
	vendor_base = base + (mmio_read_16(base + P_VENDOR_SPECIFIC_AREA) & ((1 << 12) - 1));
	bm_params.vendor_base = vendor_base;

	// reset data & cmd
	mmio_write_8(base + SDHCI_SOFTWARE_RESET, 0x6);

	// init common parameters
	mmio_write_8(base + SDHCI_PWR_CONTROL, SDHCI_BUS_VOL_VDD1_1_8V);
	mmio_write_8(base + SDHCI_TOUT_CTRL, 0xe); // for TMCLK 50Khz
	mmio_setbits_16(base + SDHCI_HOST_CONTROL2, 1 << 11); // set cmd23 support
	mmio_setbits_16(vendor_base + VENDOR_EMMC_CTRL, 0x1);

	// set clk.Toshiba/Scandisk eMMC don't support 400khz.
	bm_emmc_set_clk(EMMC_INIT_FREQ);

	// set host version 4 parameters
	mmio_setbits_16(base + SDHCI_HOST_CONTROL2, 0x1 << 12); // set HOST_VER4_ENABLE
	if (mmio_read_32(base + SDHCI_CAPABILITIES1) & 0x1 << 27)
		mmio_setbits_16(base + SDHCI_HOST_CONTROL2, 0x1 << 13); // set 64bit addressing

	// set rx src sel to 1
	mmio_clrsetbits_32(base + SDHCI_RX_DELAY_LINE, REG_RX_SRC_SEL_CLR_MASK,
			   REG_RX_SRC_SEL_CLK_TX_INV << REG_RX_SRC_SEL_SHIFT);

	// set tx src sel to 1
	mmio_clrsetbits_32(base + SDHCI_TX_DELAY_LINE, REG_TX_SRC_SEL_CLR_MASK,
			   REG_TX_SRC_SEL_CLK_TX_INV << REG_TX_SRC_SEL_SHIFT);

	// set PHY_TX_BPS to 1
	mmio_clrsetbits_32(base + SDHCI_PHY_CONFIG, REG_TX_BPS_SEL_CLR_MASK,
			   REG_TX_BPS_SEL_BYPASS << REG_TX_BPS_SEL_SHIFT);

	// eMMc card setup
	mmio_clrbits_16(base + SDHCI_HOST_CONTROL2, (0x1 << 8)); // clr UHS2_IF_ENABLE
	mmio_write_8(base + SDHCI_PWR_CONTROL,
		     mmio_read_8(base + SDHCI_PWR_CONTROL) | 0x1); // set SD_BUS_PWR_VDD1
	mmio_clrbits_16(base + SDHCI_HOST_CONTROL2, 0x7); // clr UHS_MODE_SEL

	mmio_setbits_16(base + SDHCI_HOST_CONTROL2, (0x1 << 3)); // set 1.8v signal

	mmio_setbits_16(base + SDHCI_CLK_CTRL, 0x1 << 2); // set SD_CLK_EN
	udelay(400); // wait for voltage ramp up time at least 74 cycle,1/200k*80=400us

	//we enable all interrupt status here for testing
	mmio_setbits_16(base + SDHCI_INT_STATUS_EN, 0xFFFF);
	mmio_setbits_16(base + SDHCI_ERR_INT_STATUS_EN, 0xFFFF);

	INFO("eMMC init done\n");
}

#ifdef CONFIG_ENABLE_EMMC_HW_RESET_QFN

#define GPIO_SWPORTA_DDR 0x004
#define GPIO_SWPORTA_DR 0x000
#define GPIO0_BASE 0x03020000
#define GPIO_NUM 19

static void bm_emmc_dev_reset(void)
{
	INFO("emmc hw reset: qfn\n");

	/* Set pinmux for emmc reset */
	mmio_write_8(PINMUX_BASE + 0x64, 0x3);//0x03001064
	INFO("qfn rst: pinmux:%x\n", mmio_read_8(PINMUX_BASE + 0x64));

	uint32_t base = GPIO0_BASE;
	uint32_t dir_reg_base = base + GPIO_SWPORTA_DDR;
	uint32_t val_reg_base = base + GPIO_SWPORTA_DR;

	uint32_t dir_reg = mmio_read_32(dir_reg_base);
	uint32_t val_reg = mmio_read_32(val_reg_base);

	/* Set direction to output */
	dir_reg &=  ~(1 << GPIO_NUM);
	dir_reg |= (1 << GPIO_NUM);
	mmio_write_32(dir_reg_base, dir_reg);
	udelay(100);
	INFO("qfn rst 0: dir:%x, val:%x\n", mmio_read_32(dir_reg_base), mmio_read_32(val_reg_base));

	/* Set Value to 0, reset */
	val_reg	&=  ~(1 << GPIO_NUM);
	mmio_write_32(val_reg_base, val_reg);
	udelay(500);
	INFO("qfn rst 1: dir:%x, val:%x\n", mmio_read_32(dir_reg_base), mmio_read_32(val_reg_base));

	/* Set value to 1 */
	val_reg	&=  ~(1 << GPIO_NUM);
	val_reg |= (1 << GPIO_NUM);
	mmio_write_32(val_reg_base, val_reg);
	udelay(500);
	INFO("qfn rst 2: dir:%x, val:%x\n", mmio_read_32(dir_reg_base), mmio_read_32(val_reg_base));

}
#else
static void bm_emmc_dev_reset(void)
{
	INFO("emmc hw reset: bga\n");

	uintptr_t base, vendor_base;

	base = bm_params.reg_base;
	vendor_base = base + (mmio_read_16(base + P_VENDOR_SPECIFIC_AREA) & ((1 << 12) - 1));

	INFO("eMMC RST_n 0x%x\n", mmio_read_16(vendor_base + VENDOR_EMMC_CTRL));
	mmio_clrbits_16(vendor_base + VENDOR_EMMC_CTRL, SDHCI_RESET_N);
	INFO("eMMC RST_n1 n 0x%x\n", mmio_read_16(vendor_base + VENDOR_EMMC_CTRL));
	mdelay(1);
	mmio_setbits_16(vendor_base + VENDOR_EMMC_CTRL, SDHCI_RESET_N);
	INFO("eMMC RST_n2 n 0x%x\n", mmio_read_16(vendor_base + VENDOR_EMMC_CTRL));
	mdelay(1);
}
#endif

static int bm_emmc_set_ios(int clk, int width)
{
	int ret;

	switch (width) {
	case EMMC_BUS_WIDTH_1:
		ret = bm_emmc_set_ext_csd(183, 0);
		if (ret < 0)
			return ret;
		mmio_write_8(bm_params.reg_base + SDHCI_HOST_CONTROL,
			     mmio_read_8(bm_params.reg_base + SDHCI_HOST_CONTROL) & ~SDHCI_DAT_XFER_WIDTH);
		break;
	case EMMC_BUS_WIDTH_4:
		ret = bm_emmc_set_ext_csd(183, 1);
		if (ret < 0)
			return ret;
		mmio_write_8(bm_params.reg_base + SDHCI_HOST_CONTROL,
			     mmio_read_8(bm_params.reg_base + SDHCI_HOST_CONTROL) | SDHCI_DAT_XFER_WIDTH);
		break;
	case EMMC_BUS_WIDTH_8:
		ret = bm_emmc_set_ext_csd(183, 2);
		if (ret < 0)
			return ret;
		mmio_write_8(bm_params.reg_base + SDHCI_HOST_CONTROL,
			     mmio_read_8(bm_params.reg_base + SDHCI_HOST_CONTROL) | SDHCI_EXT_DAT_XFER);
		break;
	default:
		ERROR("%s Invalid Bus width: %d\n", __func__, width);
		return -EINVAL;
	}

	return bm_emmc_set_clk(clk);
}

static int bm_emmc_prepare(int lba, uintptr_t buf, size_t size)
{
	uint64_t load_addr;
	uint32_t base, block_cnt;
	uint8_t tmp;

	load_addr = phys_to_dma(buf);
	if (((buf & EMMC_BLOCK_MASK) != 0) || ((size % EMMC_BLOCK_SIZE) != 0))
		return -EINVAL;

	base = bm_params.reg_base;
	block_cnt = size / EMMC_BLOCK_SIZE;

	if (mmio_read_16(base + SDHCI_HOST_CONTROL2) & SDHCI_HOST_VER4_ENABLE) {
		mmio_write_32(base + SDHCI_ADMA_SA_LOW, load_addr);
		mmio_write_32(base + SDHCI_ADMA_SA_HIGH, (load_addr >> 32));
		mmio_write_32(base + SDHCI_DMA_ADDRESS, block_cnt);
		mmio_write_16(base + SDHCI_BLOCK_COUNT, 0);
	} else {
		if ((load_addr >> 32) != 0)
			return -EINVAL;
		mmio_write_32(base + SDHCI_DMA_ADDRESS, load_addr);
		mmio_write_16(base + SDHCI_BLOCK_COUNT, block_cnt);
	}
	mmio_write_16(base + SDHCI_BLOCK_SIZE, SDHCI_MAKE_BLKSZ(7, 512));

	// select SDMA
	tmp = mmio_read_8(base + SDHCI_HOST_CONTROL);
	tmp &= ~SDHCI_CTRL_DMA_MASK;
	tmp |= SDHCI_CTRL_SDMA;
	mmio_write_8(base + SDHCI_HOST_CONTROL, tmp);

	return 0;
}

static int bm_emmc_read(int lba, uintptr_t buf, size_t size)
{
	return 0;
}

void bm_emmc_read_data(unsigned int *buf)
{
	int i;
	uint32_t base;

	base = bm_params.reg_base;
	for (i = 0; i < 128; i++)
		buf[i] = (unsigned int)mmio_read_32(base + SDHCI_BUF_DATA_R);

	INFO("read data over\n");
}

static int bm_emmc_write(int lba, uintptr_t buf, size_t size)
{
	return 0;
}

static void bm_emmc_hw_reset(void)
{
	uintptr_t base = bm_params.reg_base;

	// set default clk div before reset IP to avoid CLK glitch
	mmio_write_16(base + SDHCI_CLK_CTRL,
		      (mmio_read_16(base + SDHCI_CLK_CTRL) & 0x3F) | DEFAULT_DIV_EMMC_INIT_CLOCK << 8);

	mdelay(1);

	// reset IP
	mmio_write_8(base + SDHCI_SOFTWARE_RESET, 0x7);
	while (mmio_read_8(base + SDHCI_SOFTWARE_RESET))
		;
}


#define IO_PAD_CLR_MASK 0xC
#define IO_PAD_SHIFT 2
#define PULL_UP 1
#define PULL_DOWN 2

#define REG_EMMC_RSTN_PAD (PINMUX_BASE + 0x914) // 0x03001914
#define REG_EMMC_CLK_PAD (PINMUX_BASE + 0x91C) // 0x0300191C
#define REG_EMMC_CMD_PAD (PINMUX_BASE + 0x928) // 0x03001928
#define REG_EMMC_DAT0_PAD (PINMUX_BASE + 0x920) // 0x03001920
#define REG_EMMC_DAT1_PAD (PINMUX_BASE + 0x92C) // 0x0300192C
#define REG_EMMC_DAT2_PAD (PINMUX_BASE + 0x918) // 0x03001918
#define REG_EMMC_DAT3_PAD (PINMUX_BASE + 0x924) // 0x03001924

#define REG_EMMC_RSTN_PIO (PINMUX_BASE + 0x48) // 0x03001048
#define REG_EMMC_CLK_PIO (PINMUX_BASE + 0x50) // 0x03001050
#define REG_EMMC_CMD_PIO (PINMUX_BASE + 0x5C) // 0x0300105C
#define REG_EMMC_DAT0_PIO (PINMUX_BASE + 0x54) // 0x03001054
#define REG_EMMC_DAT1_PIO (PINMUX_BASE + 0x60) // 0x03001060
#define REG_EMMC_DAT2_PIO (PINMUX_BASE + 0x4C) // 0x0300104C
#define REG_EMMC_DAT3_PIO (PINMUX_BASE + 0x58) // 0x03001058


static void emmc_pad_setting(void)
{
	mmio_clrbits_32(REG_EMMC_RSTN_PIO, 0x7);
	mmio_clrbits_32(REG_EMMC_CLK_PIO, 0x7);
	mmio_clrbits_32(REG_EMMC_CMD_PIO, 0x7);
	mmio_clrbits_32(REG_EMMC_DAT0_PIO, 0x7);
	mmio_clrbits_32(REG_EMMC_DAT1_PIO, 0x7);
	mmio_clrbits_32(REG_EMMC_DAT2_PIO, 0x7);
	mmio_clrbits_32(REG_EMMC_DAT3_PIO, 0x7);
	mmio_clrsetbits_32(REG_EMMC_CLK_PAD, IO_PAD_CLR_MASK, PULL_DOWN << IO_PAD_SHIFT);
	mmio_clrsetbits_32(REG_EMMC_RSTN_PAD, IO_PAD_CLR_MASK, PULL_UP << IO_PAD_SHIFT);
	mmio_clrsetbits_32(REG_EMMC_CMD_PAD, IO_PAD_CLR_MASK, PULL_UP << IO_PAD_SHIFT);
	mmio_clrsetbits_32(REG_EMMC_DAT0_PAD, IO_PAD_CLR_MASK, PULL_UP << IO_PAD_SHIFT);
	mmio_clrsetbits_32(REG_EMMC_DAT1_PAD, IO_PAD_CLR_MASK, PULL_UP << IO_PAD_SHIFT);
	mmio_clrsetbits_32(REG_EMMC_DAT2_PAD, IO_PAD_CLR_MASK, PULL_UP << IO_PAD_SHIFT);
	mmio_clrsetbits_32(REG_EMMC_DAT3_PAD, IO_PAD_CLR_MASK, PULL_UP << IO_PAD_SHIFT);
}

void bm_emmc_phy_init(void)
{
	bm_emmc_hw_reset();
	emmc_pad_setting();
}

void bm_emmc_init(void)
{
	NOTICE("EMI/%d/%d.", bm_params.clk_rate, EMMC_TRAN_FREQ);
	bm_emmc_phy_init();
	if (emmc_init(&bm_emmc_ops, EMMC_TRAN_FREQ, bm_params.bus_width, bm_params.flags) < 0)
		ERROR("eMMC initializing failed\n");
}
