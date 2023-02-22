/*
 * (C) Copyright 2013
 * David Feng <fenghua@phytium.com.cn>
 * Sharma Bhupesh <bhupesh.sharma@freescale.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <dm/platform_data/serial_pl01x.h>
#include <asm/armv8/mmu.h>
#include <mmio.h>
#include <usb/dwc2_udc.h>
#include <usb.h>
#include "cv1822_reg.h"
#include "cvi_reboot.h"
#include "cv1822_reg_fmux_gpio.h"
#include "cv1822_pinlist_swconfig.h"
#include <linux/delay.h>
DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_PL011_SERIAL
static const struct pl01x_serial_platdata serial_platdata = {
	.base = V2M_UART0,
	.type = TYPE_PL011,
	.clock = CONFIG_PL011_CLOCK,
};

U_BOOT_DEVICE(vexpress_serials) = {
	.name = "serial_pl01x",
	.platdata = &serial_platdata,
};
#endif

static struct mm_region vexpress64_mem_map[] = {
	{
		.virt = 0x0UL,
		.phys = 0x0UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		.virt = PHYS_SDRAM_1,
		.phys = PHYS_SDRAM_1,
		.size = PHYS_SDRAM_1_SIZE,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
#ifdef BM_UPDATE_FW_START_ADDR
	}, {
		.virt = BM_UPDATE_FW_START_ADDR,
		.phys = BM_UPDATE_FW_START_ADDR,
		/*
		 * this area is for bmtest under uboot. -- added by Xun Li
		 * [0x110000000, 0x190000000] size = 2G
		 */
		.size = BM_UPDATE_FW_SIZE,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
#else
	}, {
		/*
		 * be aware we'll need 256MB more other than PHYS_SDRAM_1_SIZE for the fake flash area
		 * of itb file during ram boot, and MMC's DMA buffer (BM_UPDATE_ALIGNED_BUFFER).
		 * so either cover it here or in video's region.
		 * also be carefull with BM_SPIF_BUFFER_ADDR and BM_UPDATE_FW_START_ADDR...
		 */
		.virt = PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE,
		.phys = PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE,
		.size = 0x10000000,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
#endif
	}, {
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = vexpress64_mem_map;

// #define PINMUX_CONFIG(PIN_NAME, FUNC_NAME) printf ("%s\n", PIN_NAME ##_ ##FUNC_NAME);
#define PINMUX_CONFIG(PIN_NAME, FUNC_NAME) \
		mmio_clrsetbits_32(PINMUX_BASE + fmux_gpio_funcsel_##PIN_NAME, \
			fmux_gpio_funcsel_##PIN_NAME##_MASK << fmux_gpio_funcsel_##PIN_NAME##_OFFSET, \
			PIN_NAME##__##FUNC_NAME)

static void pinmux_config(int io_type)
{
		switch (io_type) {
		case PINMUX_UART0:
			PINMUX_CONFIG(UART0_RX, UART0_RX);
			PINMUX_CONFIG(UART0_TX, UART0_TX);
		break;
		case PINMUX_I2S1:
			PINMUX_CONFIG(PAD_AUD_AOUTR, IIS1_DI);
			PINMUX_CONFIG(AUX0, IIS1_MCLK);
			PINMUX_CONFIG(PAD_AUD_AINR_MIC, IIS1_DO);
			PINMUX_CONFIG(PAD_AUD_AINL_MIC, IIS1_BCLK);
			PINMUX_CONFIG(PAD_AUD_AOUTL, IIS1_LRCK);
		break;
		case PINMUX_I2S2:
			PINMUX_CONFIG(UART2_RX, IIS2_DI);
			PINMUX_CONFIG(IIC2_SDA, IIS2_MCLK);
			PINMUX_CONFIG(UART2_RTS, IIS2_DO);
			PINMUX_CONFIG(UART2_TX, IIS2_BCLK);
			PINMUX_CONFIG(UART2_CTS, IIS2_LRCK);
		break;
		case PINMUX_EMMC:
			PINMUX_CONFIG(EMMC_RSTN, EMMC_RSTN);
			PINMUX_CONFIG(EMMC_DAT2, EMMC_DAT_2);
			PINMUX_CONFIG(EMMC_CLK, EMMC_CLK);
			PINMUX_CONFIG(EMMC_DAT0, EMMC_DAT_0);
			PINMUX_CONFIG(EMMC_DAT3, EMMC_DAT_3);
			PINMUX_CONFIG(EMMC_CMD, EMMC_CMD);
			PINMUX_CONFIG(EMMC_DAT1, EMMC_DAT_1);
		break;
		case PINMUX_SPI_NOR:
			PINMUX_CONFIG(EMMC_CLK, SPINOR_SCK);
			PINMUX_CONFIG(EMMC_CMD, SPINOR_MISO);
			PINMUX_CONFIG(EMMC_DAT1, SPINOR_CS_X);
			PINMUX_CONFIG(EMMC_DAT0, SPINOR_MOSI);
			PINMUX_CONFIG(EMMC_DAT2, SPINOR_HOLD_X);
			PINMUX_CONFIG(EMMC_DAT3, SPINOR_WP_X);
		break;
		case PINMUX_SPI_NAND:
			PINMUX_CONFIG(EMMC_DAT2, SPINAND_HOLD);
			PINMUX_CONFIG(EMMC_CLK, SPINAND_CLK);
			PINMUX_CONFIG(EMMC_DAT0, SPINAND_MOSI);
			PINMUX_CONFIG(EMMC_DAT3, SPINAND_WP);
			PINMUX_CONFIG(EMMC_CMD, SPINAND_MISO);
			PINMUX_CONFIG(EMMC_DAT1, SPINAND_CS);
		break;
		case PINMUX_SDIO1:
			PINMUX_CONFIG(SD1_CMD, PWR_SD1_CMD);
			PINMUX_CONFIG(SD1_CLK, PWR_SD1_CLK);
			PINMUX_CONFIG(SD1_D0, PWR_SD1_D0);
			PINMUX_CONFIG(SD1_D1, PWR_SD1_D1);
			PINMUX_CONFIG(SD1_D2, PWR_SD1_D2);
			PINMUX_CONFIG(SD1_D3, PWR_SD1_D3);
		break;
		case PINMUX_DSI:
#if (defined(CV1822_WDMB_0004A_SPINAND) || defined(CV1822_WDMB_0004B_SPINAND))
			// no panel
#elif (defined(CV1822_WEVB_0005B_SPINAND) || defined(CV1822_WEVB_0005B_64MB_SPINAND))
			PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
			PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
			PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
			PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
			PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
			PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
#else
			PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
			PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
			PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
			PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
			PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
			PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
			PINMUX_CONFIG(PAD_MIPI_TXM3, XGPIOC_20);
			PINMUX_CONFIG(PAD_MIPI_TXP3, XGPIOC_21);
			PINMUX_CONFIG(PAD_MIPI_TXM4, XGPIOC_18);
			PINMUX_CONFIG(PAD_MIPI_TXP4, XGPIOC_19);
#endif
		break;
		default:
		break;
	}
}

void config_pinmux_for_low_power(void)
{
#if 0
	// Board-dependent settings
	mmio_write_32(0x03001090, 0x03);	// Set PWR_WAKEUP0 to GPIO
	mmio_write_32(0x03001094, 0x03);	// Set PWR_WAKEUP1 to GPIO
	mmio_write_32(0x0300109C, 0x03);	// Set PWR_ON to GPIO
	mmio_write_32(0x030010B0, 0x03);	// Set CLK32K to GPIO
	mmio_write_32(0x030010B4, 0x03);	// Set CLK25M to GPIO

	mmio_write_32(0x05027008, 0x40);	// PWR_SEQ1: remove PD
	mmio_write_32(0x0502700C, 0x40);	// PWR_SEQ2: remove PD
	mmio_write_32(0x05027010, 0x40);	// PWR_SEQ3: remove PD

	mmio_write_32(0x05027018, 0x40);	// PWR_WAKEUP0: remove PD, on board PU VDDIO_RTC
	mmio_write_32(0x0502701C, 0x40);	// PWR_WAKEUP1: remove PD, on board PU VDDIO_RTC
	mmio_write_32(0x05027024, 0x40);	// PWR_ON: remove PD
	mmio_write_32(0x05027034, 0x40);	// PWR_GPIO[2]: remove PD, on board PU VDDIO_WIFI
	mmio_write_32(0x05027038, 0x40);	// CLK32K: remove PD, on board PU VDDIO_RTC
	mmio_write_32(0x0502703C, 0x40);	// CLK25M: remove PD, on board PU VDDIO_RTC
	mmio_write_32(0x05027040, 0x40);	// IIC2_SCL: remove PD, on board PU VDDIO_WIFI
	mmio_write_32(0x05027044, 0x40);	// IIC2_SDA: remove PD, on board PU VDDIO_WIFI
	mmio_write_32(0x0502704C, 0x40);	// UART2_RTS: remove PD, on board PU VDDIO_WIFI
	mmio_write_32(0x05027054, 0x40);	// UART2_CTS: remove PD, on board PU VDDIO_WIFI

	mmio_write_32(0x05027030, 0x44);	// PWR_GPIO[1]: pull up
	mmio_clrbits_32(0x05021000, 0x1 << 1);	// Set PWR_GPIO[1] to input
	mmio_clrbits_32(0x05021004, 0x1 << 1);	// Set PWR_GPIO[1] to input
#endif
}

void enable_pwr_drop_protection(void)
{
	mmio_setbits_32(TOP_BASE + 0x22C, 0x3);
}

#ifdef CONFIG_CMD_CVI_SAPD
static void config_rtc_ctrl_for_low_power(void)
{
	mmio_write_32(0x05025084, 0x00030003); // m51 iso_en, sd_iso_en
	mmio_write_32(0x05025080, 0x00000003); // m51 power down , sd power up
	mmio_write_32(0x05025084, 0x00030002); // m51 iso en and sd iso dis
	mmio_write_32(0x05025098, 0x000009A6); // sd sram power up
	mmio_write_32(0x05025034, 0x06450036); // sd clock en
	//info("Drc_lp\n");
}
#endif

#include "../cvi_board_init.c"

// #define CV182X_RTC_EXTERNAL_32K
int board_init(void)
{
#if defined(CONFIG_TARGET_CVITEK_CV1822_ASIC) /* config eth internal phy on ASIC board */
	unsigned int val;

	val = readl(0x03009000) & ETH_PHY_INIT_MASK;
	writel((val | ETH_PHY_SHUTDOWN) & ETH_PHY_RESET, 0x03009000);
	mdelay(1);
	writel(val & ETH_PHY_POWERUP & ETH_PHY_RESET, 0x03009000);
	mdelay(20);
	writel((val & ETH_PHY_POWERUP) | ETH_PHY_RESET_N, 0x03009000);
	mdelay(1);
#endif

/* Set ethernet clock resource */
#if defined(CONFIG_TARGET_CVITEK_CV1835_FPGA)
	writel(0x000000C0, 0x03000034); /* Set eth0 RGMII, eth1 RMII clk resource and interface type*/
#elif defined(CONFIG_TARGET_CVITEK_CV1835_ASIC)
	writel(0x00000004, 0x03000034); /* Set eth0 RMII, eth1 RGMII clk resource and interface type*/
#elif defined(CONFIG_TARGET_CVITEK_CV1835_PALLADIUM)
	writel(0x00000004, 0x03000034); /* Set eth0 RMII, eth1 RGMII clk resource and interface type*/
#endif

#ifdef CONFIG_CMD_CVI_SAPD
	mmio_write_32(RTC_INFO0, CVI_SAPD_FLAG);
	config_rtc_ctrl_for_low_power();
#endif

	enable_pwr_drop_protection();

#if defined(CONFIG_NAND_SUPPORT)
	pinmux_config(PINMUX_SPI_NAND);
#elif defined(CONFIG_SPI_FLASH)
	pinmux_config(PINMUX_SPI_NOR);
#elif defined(CONFIG_EMMC_SUPPORT)
	pinmux_config(PINMUX_EMMC);
#endif

#ifdef CONFIG_DISPLAY_CVITEK_MIPI
	pinmux_config(PINMUX_DSI);
#endif

#if defined(CV1822_WDMB_0002A_SPINAND)
	PINMUX_CONFIG(JTAG_CPU_TCK, XGPIOA_18); //IIC2_SHDN
	PINMUX_CONFIG(JTAG_CPU_TMS, XGPIOA_19);//AMP_MUTE
#elif (defined(CV1822_WEVB_0005B_SPINAND) || defined(CV1822_WEVB_0005B_64MB_SPINAND))
	PINMUX_CONFIG(SPK_EN, XGPIOA_15);
	PINMUX_CONFIG(JTAG_CPU_TCK, XGPIOA_18);
	PINMUX_CONFIG(JTAG_CPU_TMS, XGPIOA_19);//AMP_MUTE
#elif defined(CV1822_WDMB_0001A_SPINAND)
	PINMUX_CONFIG(UART1_RX, UART1_RX); //UART_RX,TX,CTS
	PINMUX_CONFIG(UART1_TX, UART1_TX);
	PINMUX_CONFIG(UART1_CTS, UART1_CTS);
#endif

#if defined(CONFIG_MMC_SDHCI_CVITEK_WIFI)
	pinmux_config(PINMUX_SDIO1);
#else
	/* If not support wifi, then switch pinmux to enable LED */
	PINMUX_CONFIG(SD1_CMD, EPHY_LNK_LED);
	PINMUX_CONFIG(SD1_CLK, EPHY_SPD_LED);
#endif

#if (defined(CV1822_WDMB_0004A_SPINAND) || defined(CV1822_WDMB_0004B_SPINAND))
	PINMUX_CONFIG(PAD_MIPI_TXP0, CAM_MCLK0);
	PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
	PINMUX_CONFIG(IIC2_SCL, PWR_GPIO_12);
	PINMUX_CONFIG(IIC2_SDA, PWR_GPIO_13);
	PINMUX_CONFIG(PAD_MIPI_TXP1, IIC2_SCL);
	PINMUX_CONFIG(PAD_MIPI_TXM1, IIC2_SDA);
	PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
#elif (defined(CV1821_WEVB_0005B_SPINAND) ||	\
	defined(CV1822_WEVB_0005B_128MB_SPINAND) ||	\
	defined(CV1822_WEVB_0005B_64MB_SPINAND) ||	\
	defined(CV1822_WEVB_0005B_SPINAND)) || \
	defined(CV1822_WEVB_0005B_SPINOR) || \
	defined(CV1821_WEVB_0005B_SPINOR) || \
	defined(CV1821_WEVB_0005B_64MB_SPINOR) || \
	defined(CV1820_WEVB_0005B_SPINOR)
	PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
	PINMUX_CONFIG(PAD_MIPI_TXM0, CAM_MCLK1);
	PINMUX_CONFIG(IIC2_SCL, PWR_GPIO_12);
	PINMUX_CONFIG(IIC2_SDA, PWR_GPIO_13);
	PINMUX_CONFIG(PAD_MIPI_TXP1, IIC2_SCL);
	PINMUX_CONFIG(PAD_MIPI_TXM1, IIC2_SDA);
#elif defined(CV1822_WDMB_0004C_SPINAND)
	PINMUX_CONFIG(SD1_CMD, PWR_GPIO_22); //PANEL RESET
	PINMUX_CONFIG(SD1_D0, IIC1_SDA); //IIC1 SDA
	PINMUX_CONFIG(SD1_D3, IIC1_SCL); //IIC1 SCL
	PINMUX_CONFIG(SD1_D2, CAM_MCLK0); //MCLK
	PINMUX_CONFIG(SD1_D1, PWR_GPIO_20); //CAM RESET
	PINMUX_CONFIG(SD1_CLK, PWR_GPIO_23); //PWDN
#else
	PINMUX_CONFIG(IIC2_SCL, IIC2_SCL); //IIC2
	PINMUX_CONFIG(IIC2_SDA, IIC2_SDA);
#endif

#if 0
#ifdef CONFIG_PWM_CVITEK
	PINMUX_CONFIG(IIC2_SCL, PWM_14);//PWM_14
	PINMUX_CONFIG(IIC2_SDA, PWM_15);//PWM_15
	PINMUX_CONFIG(PWM0_BUCK, PWM_0);//PWM_0
#endif
#endif

	cvi_board_init();

	config_pinmux_for_low_power();

	return 0;
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}

void software_root_reset(void)
{
	// clear spinand sv vec in case stuck at next boot.
	mmio_setbits_32(CLKGEN_BASE, BIT(4)); // Enable clk_tpu
	memset((void *)BM_IO_BUF_BASE, 0x0, BM_IO_BUF_SIZE); //8KB

	mmio_write_32(REG_RTC_BASE + RTC_EN_WARM_RST_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_WARM_RST_REQ) != 0x01)
		;
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
	mmio_setbits_32(REG_RTC_CTRL_BASE + RTC_CTRL0, 0xFFFF0800 | (0x1 << 4));
	while (1)
		;
}
void reset_cpu(void)
{
}

/*
 * Board specific ethernet initialization routine.
 */
#if 0
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_SMC91111
	rc = smc91111_initialize(0, CONFIG_SMC91111_BASE);
#endif
#ifdef CONFIG_SMC911X
	rc = smc911x_initialize(0, CONFIG_SMC911X_BASE);
#endif
	return rc;
}
#endif
#ifdef CONFIG_USB_GADGET_DWC2_OTG
struct dwc2_plat_otg_data cv182x_otg_data = {
	.regs_otg = USB_BASE,
	.usb_gusbcfg    = 0x40081400,
	.rx_fifo_sz     = 512,
	.np_tx_fifo_sz  = 512,
	.tx_fifo_sz     = 512,
};

int board_usb_init(int index, enum usb_init_type init)
{
	uint32_t value;

	value = mmio_read_32(TOP_BASE + REG_TOP_SOFT_RST) & (~BIT_TOP_SOFT_RST_USB);
	mmio_write_32(TOP_BASE + REG_TOP_SOFT_RST, value);
	udelay(50);
	value = mmio_read_32(TOP_BASE + REG_TOP_SOFT_RST) | BIT_TOP_SOFT_RST_USB;
	mmio_write_32(TOP_BASE + REG_TOP_SOFT_RST, value);

	/* Set USB phy configuration */
	value = mmio_read_32(REG_TOP_USB_PHY_CTRL);
	mmio_write_32(REG_TOP_USB_PHY_CTRL, value | BIT_TOP_USB_PHY_CTRL_EXTVBUS
					| USB_PHY_ID_OVERRIDE_ENABLE
					| USB_PHY_ID_VALUE);

	/* Enable ECO RXF */
	mmio_write_32(REG_TOP_USB_ECO, mmio_read_32(REG_TOP_USB_ECO) | BIT_TOP_USB_ECO_RX_FLUSH);

	printf("cvi_usb_hw_init done\n");

	return dwc2_udc_probe(&cv182x_otg_data);
}
#endif
