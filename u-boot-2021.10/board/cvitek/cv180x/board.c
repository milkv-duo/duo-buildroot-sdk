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
#include <asm/io.h>
#include <linux/compiler.h>
#if defined(__aarch64__)
#include <asm/armv8/mmu.h>
#endif
#include <usb/dwc2_udc.h>
#include <usb.h>
#include "cv180x_reg.h"
#include "mmio.h"
#include "cv180x_reg_fmux_gpio.h"
#include "cv180x_pinlist_swconfig.h"
#include <linux/delay.h>
#include <bootstage.h>

#if defined(__riscv)
#include <asm/csr.h>
#endif

DECLARE_GLOBAL_DATA_PTR;
#define SD1_SDIO_PAD

#if defined(CV1801C_WEVB_0009A_SPINOR) ||	\
	defined(CV1800C_WEVB_0009A_SPINOR) ||	\
	defined(CV1801C_WEVB_0009A_SPINAND)
#define CV180X_QFN_88_PIN
#elif defined(CV1801C_WDMB_0009A_SPINOR)
#define CV180X_QFN_88_PIN_38
#else
#define CV180X_QFN_68_PIN
#endif

#if defined(__aarch64__)
static struct mm_region cv180x_mem_map[] = {
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
	}, {
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = cv180x_mem_map;
#endif

// #define PINMUX_CONFIG(PIN_NAME, FUNC_NAME) printf ("%s\n", PIN_NAME ##_ ##FUNC_NAME);
#define PINMUX_CONFIG(PIN_NAME, FUNC_NAME) \
		mmio_clrsetbits_32(PINMUX_BASE + FMUX_GPIO_FUNCSEL_##PIN_NAME, \
			FMUX_GPIO_FUNCSEL_##PIN_NAME##_MASK << FMUX_GPIO_FUNCSEL_##PIN_NAME##_OFFSET, \
			PIN_NAME##__##FUNC_NAME)

void pinmux_config(int io_type)
{
		switch (io_type) {
		case PINMUX_UART0:
			PINMUX_CONFIG(UART0_RX, UART0_RX);
			PINMUX_CONFIG(UART0_TX, UART0_TX);
		break;
		case PINMUX_SDIO0:
			PINMUX_CONFIG(SD0_CD, SDIO0_CD);
			PINMUX_CONFIG(SD0_PWR_EN, SDIO0_PWR_EN);
			PINMUX_CONFIG(SD0_CMD, SDIO0_CMD);
			PINMUX_CONFIG(SD0_CLK, SDIO0_CLK);
			PINMUX_CONFIG(SD0_D0, SDIO0_D_0);
			PINMUX_CONFIG(SD0_D1, SDIO0_D_1);
			PINMUX_CONFIG(SD0_D2, SDIO0_D_2);
			PINMUX_CONFIG(SD0_D3, SDIO0_D_3);
			break;

		case PINMUX_SPI0:
			PINMUX_CONFIG(SD0_CMD, SPI0_SDO);
			PINMUX_CONFIG(SD0_CLK, SPI0_SCK);
			PINMUX_CONFIG(SD0_D0, SPI0_SDI);
			PINMUX_CONFIG(SD0_D3, SPI0_CS_X);
			break;

		case PINMUX_SPI2:
			PINMUX_CONFIG(SD1_CMD, SPI2_SDO);
			PINMUX_CONFIG(SD1_CLK, SPI2_SCK);
			PINMUX_CONFIG(SD1_D0, SPI2_SDI);
			PINMUX_CONFIG(SD1_D3, SPI2_CS_X);
			break;

		case PINMUX_SDIO1:
#if defined(SD1_SDIO_PAD)
			/*
			 * Name            Address            SD1  MIPI
			 * reg_sd1_phy_sel REG_0x300_0294[10] 0x0  0x1
			 */
			mmio_write_32(TOP_BASE + 0x294,
				      (mmio_read_32(TOP_BASE + 0x294) & 0xFFFFFBFF));
			PINMUX_CONFIG(SD1_CMD, PWR_SD1_CMD);
			PINMUX_CONFIG(SD1_CLK, PWR_SD1_CLK);
			PINMUX_CONFIG(SD1_D0, PWR_SD1_D0);
			PINMUX_CONFIG(SD1_D1, PWR_SD1_D1);
			PINMUX_CONFIG(SD1_D2, PWR_SD1_D2);
			PINMUX_CONFIG(SD1_D3, PWR_SD1_D3);
#endif
			break;
		case PINMUX_SPI_NOR:
			PINMUX_CONFIG(SPINOR_HOLD_X, SPINOR_HOLD_X);
			PINMUX_CONFIG(SPINOR_SCK, SPINOR_SCK);
			PINMUX_CONFIG(SPINOR_MOSI, SPINOR_MOSI);
			PINMUX_CONFIG(SPINOR_WP_X, SPINOR_WP_X);
			PINMUX_CONFIG(SPINOR_MISO, SPINOR_MISO);
			PINMUX_CONFIG(SPINOR_CS_X, SPINOR_CS_X);
		break;
		case PINMUX_SPI_NAND:
			PINMUX_CONFIG(SPINOR_HOLD_X, SPINAND_HOLD);
			PINMUX_CONFIG(SPINOR_SCK, SPINAND_CLK);
			PINMUX_CONFIG(SPINOR_MOSI, SPINAND_MOSI);
			PINMUX_CONFIG(SPINOR_WP_X, SPINAND_WP);
			PINMUX_CONFIG(SPINOR_MISO, SPINAND_MISO);
			PINMUX_CONFIG(SPINOR_CS_X, SPINAND_CS);
		break;
		case PINMUX_USB:
#if defined(CV180X_QFN_88_PIN)
			PINMUX_CONFIG(PWR_GPIO0, PWR_GPIO_0);
			PINMUX_CONFIG(PWR_GPIO1, PWR_GPIO_1);
			PINMUX_CONFIG(ADC1, XGPIOB_3);
			PINMUX_CONFIG(USB_VBUS_DET, USB_VBUS_DET);
#elif defined(CV180X_QFN_88_PIN_38)
			PINMUX_CONFIG(ADC1, XGPIOB_3);
			PINMUX_CONFIG(USB_VBUS_DET, USB_VBUS_DET);
#elif defined(CV180X_QFN_68_PIN)
			PINMUX_CONFIG(SD1_GPIO0, PWR_GPIO_25);
			PINMUX_CONFIG(SD1_GPIO1, PWR_GPIO_26);
			PINMUX_CONFIG(ADC1, XGPIOB_3);
			PINMUX_CONFIG(USB_VBUS_DET, USB_VBUS_DET);
#endif
		break;
		default:
			break;
	}
}

#include "../cvi_board_init.c"

#if defined(CONFIG_PHY_CVITEK)
static void cv180x_ephy_id_init(void)
{
	// set rg_ephy_apb_rw_sel 0x0804@[0]=1/APB by using APB interface
	mmio_write_32(0x03009804, 0x0001);

	// Release 0x0800[0]=0/shutdown
	mmio_write_32(0x03009800, 0x0900);

	// Release 0x0800[2]=1/dig_rst_n, Let mii_reg can be accessabile
	mmio_write_32(0x03009800, 0x0904);

	// ANA INIT (PD/EN), switch to MII-page5
	mmio_write_32(0x0300907c, 0x0500);
	// Release ANA_PD p5.0x10@[13:8] = 6'b001100
	mmio_write_32(0x03009040, 0x0c00);
	// Release ANA_EN p5.0x10@[7:0] = 8'b01111110
	mmio_write_32(0x03009040, 0x0c7e);

	// Wait PLL_Lock, Lock_Status p5.0x12@[15] = 1
	//mdelay(1);

	// Release 0x0800[1] = 1/ana_rst_n
	mmio_write_32(0x03009800, 0x0906);

	// ANA INIT
	// @Switch to MII-page5
	mmio_write_32(0x0300907c, 0x0500);

	// PHY_ID
	mmio_write_32(0x03009008, 0x0043);
	mmio_write_32(0x0300900c, 0x5649);

	// switch to MDIO control by ETH_MAC
	mmio_write_32(0x03009804, 0x0000);
}

static void cv180x_ephy_led_pinmux(void)
{
	// LED PAD MUX
	mmio_write_32(0x0300109c, 0x05);
	mmio_write_32(0x030010a0, 0x05);
	//(SD1_CLK selphy)
	mmio_write_32(0x050270b0, 0x11111111);
	//(SD1_CMD selphy)
	mmio_write_32(0x050270b4, 0x11111111);
}
#endif

int board_init(void)
{
	extern uint32_t BOOT0_START_TIME;
	uint16_t start_time = DIV_ROUND_UP(BOOT0_START_TIME, SYS_COUNTER_FREQ_IN_SECOND / 1000);

	// Save uboot start time. time is from boot0.h
	mmio_write_16(TIME_RECORDS_FIELD_UBOOT_START, start_time);

#if defined(CONFIG_PHY_CVITEK) /* config cvitek cv180x eth internal phy on ASIC board */
	cv180x_ephy_id_init();
	cv180x_ephy_led_pinmux();
#endif

#if defined(CONFIG_NAND_SUPPORT)
	pinmux_config(PINMUX_SPI_NAND);
#elif defined(CONFIG_SPI_FLASH)
	pinmux_config(PINMUX_SPI_NOR);
#endif
	// pinmux_config(PINMUX_SDIO1);
	pinmux_config(PINMUX_USB);
	//pinmux_config(PINMUX_SPI0);
	//pinmux_config(PINMUX_SPI2);
	cvi_board_init();

	return 0;
}

#if defined(__aarch64__)
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
#endif

#ifdef CV_SYS_OFF
static void cv_system_off(void)
{
	mmio_write_32(REG_RTC_BASE + RTC_EN_SHDN_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_SHDN_REQ) != 0x01)
		;
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
	mmio_setbits_32(REG_RTC_CTRL_BASE + RTC_CTRL0, 0xFFFF0800 | (0x1 << 0));

	while (1)
		;
}
#endif

void cv_system_reset(void)
{
	mmio_write_32(REG_RTC_BASE + RTC_EN_WARM_RST_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_WARM_RST_REQ) != 0x01)
		;
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
	mmio_setbits_32(REG_RTC_CTRL_BASE + RTC_CTRL0, 0xFFFF0800 | (0x1 << 4));

	while (1)
		;
}

/*
 * Board specific reset that is system reset.
 */
void reset_cpu(void)
{
	cv_system_reset();
}

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

void board_save_time_record(uintptr_t saveaddr)
{
	uint64_t boot_us = 0;
#if defined(__aarch64__)
	boot_us = timer_get_boot_us();
#elif defined(__riscv)
	// Read from CSR_TIME directly. RISC-V timers is initialized later.
	boot_us = csr_read(CSR_TIME) / (SYS_COUNTER_FREQ_IN_SECOND / 1000000);
#else
#error "Unknown ARCH"
#endif

	mmio_write_16(saveaddr, DIV_ROUND_UP(boot_us, 1000));
}
