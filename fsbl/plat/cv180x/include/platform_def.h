/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#include <cpu.h>
#include <bl_common.h>

#ifndef __ASSEMBLY__

/*
 * These definition are used to verify struct size and offset.
 * Hard-coded value only. Do not use sizeof() or offsetof() here.
 */
// struct fip_param1->nand_info must be same as the definition in u-boot
#define NAND_INFO_OFFSET 16

struct spi_nand_info_t {
	uint32_t version;
	uint32_t id;
	uint32_t page_size;
	uint32_t spare_size;
	uint32_t block_size;
	uint32_t pages_per_block;
	uint32_t fip_block_cnt;
	uint8_t pages_per_block_shift;
	uint8_t badblock_pos;
	uint8_t dummy_data1[2];
	uint32_t flags;
	uint8_t ecc_en_feature_offset;
	uint8_t ecc_en_mask;
	uint8_t ecc_status_offset;
	uint8_t ecc_status_mask;
	uint8_t ecc_status_shift;
	uint8_t ecc_status_uncorr_val;
	uint8_t dummy_data2[2];
	uint32_t erase_count; // erase count for sys base block
	uint8_t sck_l;
	uint8_t sck_h;
	uint16_t max_freq;
	uint32_t sample_param;
	uint8_t xtal_switch;
	uint8_t dummy_data3[71];
};

struct spinor_info_t {
	uint32_t ctrl;
	uint32_t dly_ctrl;
	uint32_t tran_csr;
	uint32_t opt;
	uint32_t reserved_1;
	uint32_t reserved_2;
	uint32_t reserved_3;
	uint32_t reserved_4;
	uint32_t reserved_5;
} __packed;

struct chip_conf {
	uint32_t reg;
	uint32_t value;
} __packed;

struct fip_flags {
	struct {
		uint8_t rsa_size : 2;
		uint8_t scs : 2;
		uint8_t encrypted : 2;
		uint8_t reserved1 : 2;
	};
	uint8_t reserved2[7];
} __packed;

struct fip_param1 {
	uint64_t magic1;
	uint32_t magic2;
	uint32_t param_cksum;
	struct spi_nand_info_t nand_info;
	struct spinor_info_t spinor_info;
	struct fip_flags fip_flags;
	uint32_t chip_conf_size;
	uint32_t blcp_img_cksum;
	uint32_t blcp_img_size;
	uint32_t blcp_img_runaddr;
	uint32_t blcp_param_loadaddr;
	uint32_t blcp_param_size;
	uint32_t bl2_img_cksum;
	uint32_t bl2_img_size;
	uint32_t bld_img_size;
	uint32_t param2_loadaddr;
	uint32_t reserved1;
	struct chip_conf chip_conf[95];
	uint8_t bl_ek[32];
	uint8_t root_pk[512];
	uint8_t bl_pk[512];
	uint8_t bl_pk_sig[512];
	uint8_t chip_conf_sig[512];
	uint8_t bl2_img_sig[512];
	uint8_t blcp_img_sig[512];
} __packed __aligned(__alignof__(unsigned int));

struct blcp_param_head {
	uint32_t magic;
	uint32_t cksum;
} __packed;

#define BLCP_PARAM_MAGIC 0x52505043 // "CPPR"
#define BLCP_PARAM_MAX_SIZE 512
#define BLCP_PARAM_RETRY 4

struct bl2_head {
	uint64_t magic1;
	uint64_t magic2;
	uint32_t msid;
	uint32_t version;
	uint64_t reserved1;
} __packed;

/* this structure should be modified all of fsbl & MCU & osdrv side */
struct transfer_config_t {
	uint32_t conf_magic;
	uint32_t conf_size;  //conf_size exclude mcu_status & linux_status
	uint32_t isp_buffer_addr;
	uint32_t isp_buffer_size;
	uint32_t encode_img_addr;
	uint32_t encode_img_size;
	uint32_t encode_buf_addr;
	uint32_t encode_buf_size;
	uint8_t  dump_print_enable;
	uint8_t  dump_print_size_idx;
	uint16_t image_type;
	uint16_t checksum; // checksum exclude mcu_status & linux_status
	uint8_t  mcu_status;
	uint8_t  linux_status;
} __packed;

enum _MUC_STATUS_E {
	MCU_STATUS_NONOS_INIT = 1,
	MCU_STATUS_NONOS_RUNNING,
	MCU_STATUS_NONOS_DONE,
	MCU_STATUS_RTOS_T1_INIT,  // before linux running
	MCU_STATUS_RTOS_T1_RUNNING,
	MCU_STATUS_RTOS_T2_INIT,  // after linux running
	MCU_STATUS_RTOS_T2_RUNNING,
	MCU_STATUS_LINUX_INIT,
	MCU_STATUS_LINUX_RUNNING,
};

enum E_IMAGE_TYPE {
	E_FAST_JEPG = 1,
	E_FAST_H264,
	E_FAST_H265,
};

enum DUMP_PRINT_SIZE_E {
	DUMP_PRINT_SZ_IDX_0K = 0,
	DUMP_PRINT_SZ_IDX_4K = 12, // 4096 = 1<<12
	DUMP_PRINT_SZ_IDX_8K,
	DUMP_PRINT_SZ_IDX_16K,
	DUMP_PRINT_SZ_IDX_32K,
	DUMP_PRINT_SZ_IDX_LIMIT,
};

#define BOOT_SRC_TAG 0xCE00

// NO ZERO in boot_src
enum boot_src {
	// Read from flash
	BOOT_SRC_SPI_NAND = 0x0 | BOOT_SRC_TAG,
	BOOT_SRC_SPI_NOR = 0x2 | BOOT_SRC_TAG,
	BOOT_SRC_EMMC = 0x3 | BOOT_SRC_TAG,

	// Download
	BOOT_SRC_SD = 0xA0 | BOOT_SRC_TAG,
	BOOT_SRC_USB = 0xA3 | BOOT_SRC_TAG,
	BOOT_SRC_UART = 0xA5 | BOOT_SRC_TAG,
};

#define DOWNLOAD_BUTTON 0x1
#define DOWNLOAD_DISABLE 0x2

union sw_info {
	uint32_t value;
	struct {
		uint32_t dis_dbg_inject : 1;
		uint32_t usb_polling_time : 1;
		uint32_t dis_uart_msg : 1;
		uint32_t usb_utmi_rst : 1;
		uint32_t reserved : 1;
		uint32_t usb_vid : 16;
		uint32_t dis_usb_rxf : 1;
		uint32_t sd_dl : 2;
		uint32_t usd_dl : 2;
		uint32_t uart_dl : 2;
		uint32_t sd_polarity : 2;
		uint32_t reset_type : 1;
		uint32_t sw_info_enable : 1;
	};
} __packed;

struct _time_records {
	uint16_t fsbl_start;
	uint16_t ddr_init_start;
	uint16_t ddr_init_end;
	uint16_t release_blcp_2nd;
	uint16_t load_loader_2nd_end;
	uint16_t fsbl_decomp_start;
	uint16_t fsbl_decomp_end;
	uint16_t fsbl_exit;
	uint16_t uboot_start;
	uint16_t bootcmd_start;
	uint16_t decompress_kernel_start;
	uint16_t kernel_start;
	uint16_t kernel_run_init_start;
} __packed;

extern struct _time_records *time_records;

#endif /* __ASSEMBLY__ */

/*
 * PINMUX
 */
#define PINMUX_SPI0 11
#define PINMUX_SDIO0 22
#define PINMUX_EMMC 25
#define PINMUX_SPI_NOR 26
#define PINMUX_SPI_NAND 27

/*
 * SoC memory map
 */
#define SEC_SUBSYS_BASE 0x02000000
#define SEC_CRYPTODMA_BASE (SEC_SUBSYS_BASE + 0x00060000)
#define SEC_FAB_FIREWALL (SEC_SUBSYS_BASE + 0x00090000)
#define SEC_DDR_FIREWALL (SEC_SUBSYS_BASE + 0x000A0000)
#define SEC_SYS_BASE (SEC_SUBSYS_BASE + 0x000B0000)
#define SEC_EFUSE_BASE (SEC_SUBSYS_BASE + 0x000C0000)

#define TOP_BASE 0x03000000
#define PINMUX_BASE (TOP_BASE + 0x00001000)
#define CLKGEN_BASE (TOP_BASE + 0x00002000)
#define RST_BASE (TOP_BASE + 0x00003000)
#define WATCHDOG_BASE (TOP_BASE + 0x00010000)
#define GPIO_BASE (TOP_BASE + 0x00020000)
#define EFUSE_BASE (TOP_BASE + 0x00050000)
#define PLL_G2_BASE (TOP_BASE + 0x00002800)
#define PWM0_BASE (TOP_BASE + 0x00060000)

#define HSPERI_BASE 0x04000000
#define SPINAND_BASE (HSPERI_BASE + 0x00060000)
#define UART0_BASE (HSPERI_BASE + 0x00140000)
#define UART2_BASE (HSPERI_BASE + 0x00160000)
#define USB_BASE (HSPERI_BASE + 0x00340000)
#define EMMC_BASE (HSPERI_BASE + 0x00300000)
#define SDIO_BASE (HSPERI_BASE + 0x00310000)
#define SYSDMA_BASE (HSPERI_BASE + 0x00330000)
#define SPIF_BASE 0x10000000
#define SPIF1_BASE 0x05400000

#define RTC_SYS_BASE 0x05000000
#define RTC_GPIO_BASE (RTC_SYS_BASE + 0x00021000)

#define RTC_SRAM_BASE (RTC_SYS_BASE + 0x00200000)
#define RTC_SRAM_SIZE 0x6000 // 24KiB

#define AXI_SRAM_BASE 0x0E000000
#define AXI_SRAM_SIZE 0x40
#define AXI_SRAM_RTOS_OFS 0x7C
#define AXI_SRAM_RTOS_BASE (AXI_SRAM_BASE + AXI_SRAM_RTOS_OFS)
#define CVI_RTOS_MAGIC_CODE 0xABC0DEF

#define MAILBOX_FIELD 0x1900400

#define C906_MAGIC_HEADER 0xA55AC906 // master cpu is c906
#define CA53_MAGIC_HEADER 0xA55ACA53 // master cpu is ca53

#ifdef __riscv
#define RTOS_MAGIC_HEADER C906_MAGIC_HEADER
#else
#define RTOS_MAGIC_HEADER CA53_MAGIC_HEADER
#endif

#define ROM_SIZE 0x10000 // 64KiB
#define TPU_SRAM_ORIGIN_BASE 0x0C000000
#define TPU_SRAM_SIZE 0x10000 // 64KiB
#define VC_SRAM_ORIGIN_BASE 0x0BC00000
#define VC_SRAM_SIZE 0x00019000 // 100KiB

#ifdef __riscv
	#define ROM_BASE 0x04400000
	#define TPU_SRAM_BASE 0x3C000000 // Shadow_tpu_mem
	#define VC_RAM_BASE 0x3BC00000 // Shadow_vc_mem
	#define SYSMAP_MIRROR_OFFSET 0
#else
#error "Not support"
#endif

/*
 * AXI SRAM
 */
#define EFUSE_SW_INFO_ADDR (AXI_SRAM_BASE)
#define EFUSE_SW_INFO_SIZE 4

#define BOOT_SOURCE_FLAG_ADDR (EFUSE_SW_INFO_ADDR + EFUSE_SW_INFO_SIZE)
#define BOOT_SOURCE_FLAG_SIZE 4
#define MAGIC_NUM_USB_DL 0x4D474E31 // MGN1
#define MAGIC_NUM_SD_DL 0x4D474E32 // MGN2
#define MAGIC_NUM_UART_DL 0x4D474E33 // MGN3

#define BOOT_LOG_LEN_ADDR (BOOT_SOURCE_FLAG_ADDR + BOOT_SOURCE_FLAG_SIZE) // 0x0E000008
#define BOOT_LOG_LEN_SIZE 4

#define TIME_RECORDS_ADDR (AXI_SRAM_BASE + 0x10) // 0x0E000010

// only for debugging
#define ATF_DBG_REG (BOOT_LOG_LEN_ADDR + BOOT_LOG_LEN_SIZE)
#define ATF_ERR_REG (ATF_DBG_REG + 0x04)
#define ATF_ERR_INFO0 (ATF_DBG_REG + 0x08)
#define CP_STATE_REG (ATF_DBG_REG + 0x0C)

#define ATF_ERR (((unsigned int __volatile__ *)ATF_ERR_REG)[0])

/* End of AXI SRAM */

/*
 * Some data must be aligned on the biggest cache line size in the platform.
 * This is known only to the platform as it might have a combination of
 * integrated and external caches.
 */
#define CACHE_WRITEBACK_SHIFT 6
#define CACHE_WRITEBACK_GRANULE (1 << CACHE_WRITEBACK_SHIFT)

#define PLAT_PHY_ADDR_SPACE_SIZE (1ull << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE (1ull << 32)
#define MAX_MMAP_REGIONS 8
#define MAX_XLAT_TABLES 6 // varies when memory layout changes

#include <mmap.h>

/*
 * UART definitions
 */
#define PLAT_BOOT_UART_BASE UART0_BASE

/*
 * TOP registers.
 */
#define REG_TOP_CHIPID (TOP_BASE + 0x0)
#define REG_TOP_CONF_INFO (TOP_BASE + 0x4)
#define REG_TOP_USB_PHY_CTRL (TOP_BASE + 0x48)

#define BIT_C906L_BOOT_FROM_RTCSYS_EN (1 << 6)

#define REG_GP_REG0 (TOP_BASE + 0x80)
#define REG_GP_REG1 (TOP_BASE + 0x84)
#define REG_GP_REG2 (TOP_BASE + 0x88) // Trig simulation bench to increse cntpct_el0
#define REG_GP_REG3 (TOP_BASE + 0x8C)

#define REG_USB_ECO_REG (TOP_BASE + 0xB4)
#define REG_USB_ECO_RXF 0x80

#define REG_CLK_BYPASS_SEL_REG (CLKGEN_BASE + 0x30)
#define REG_CLK_DIV0_CTL_CA53_REG (CLKGEN_BASE + 0x40)
#define REG_CLK_DIV0_CTL_CPU_AXI0_REG (CLKGEN_BASE + 0x48)
#define REG_CLK_DIV0_CTL_TPU_AXI_REG (CLKGEN_BASE + 0x54)

#define REG_CLK_DIV_AXI4 (CLKGEN_BASE + 0xB8)

#define REG_PLL_G2_CTRL (PLL_G2_BASE + 0x0)
#define REG_APLL0_CSR (PLL_G2_BASE + 0x0C)
#define REG_DISPPLL_CSR (PLL_G2_BASE + 0x10)
#define REG_CAM0PLL_CSR (PLL_G2_BASE + 0x14)
#define REG_CAM1PLL_CSR (PLL_G2_BASE + 0x18)
#define REG_PLL_G2_SSC_SYN_CTRL (PLL_G2_BASE + 0x40)
#define REG_APLL_SSC_SYN_CTRL (PLL_G2_BASE + 0x50)
#define REG_APLL_SSC_SYN_SET (PLL_G2_BASE + 0x54)
#define REG_DISPPLL_SSC_SYN_CTRL (PLL_G2_BASE + 0x60)
#define REG_DISPPLL_SSC_SYN_SET (PLL_G2_BASE + 0x64)
#define REG_CAM0PLL_SSC_SYN_CTRL (PLL_G2_BASE + 0x70)
#define REG_CAM0PLL_SSC_SYN_SET (PLL_G2_BASE + 0x74)
#define REG_CAM1PLL_SSC_SYN_CTRL (PLL_G2_BASE + 0x80)
#define REG_CAM1PLL_SSC_SYN_SET (PLL_G2_BASE + 0x84)

#define SHIFT_TOP_USB_ID 8
#define SHIFT_TOP_USB_VBUS 9
#define BIT_TOP_USB_ID (1 << SHIFT_TOP_USB_ID)
#define BIT_TOP_USB_VBUS (1 << SHIFT_TOP_USB_VBUS)

#define REG_TOP_SD_PWRSW_CTRL (TOP_BASE + 0x1F4)
#define REG_TOP_SD_CTRL_OPT (TOP_BASE + 0x294)
#define BIT_IO_TRAP_SD0_PWR_DIN (1 << 27)
#define BIT_SD0_PWR_EN_POLARITY (1 << 16)
#define BIT_SD1_PWR_EN_POLARITY (1 << 17)

#define PWM_HLPERIOD0 0x0
#define PWM_PERIOD0 0x4
#define PWM_HLPERIOD1 0x8
#define PWM_PERIOD1 0xC
#define PWM_HLPERIOD2 0x10
#define PWM_PERIOD2 0x14
#define PWM_HLPERIOD3 0x18
#define PWM_PERIOD3 0x1C
#define PWM_START 0x44
#define PWM_OE 0xD0

/*
 * DEBUG register
 */
#define ATF_STATE_REG REG_GP_REG1
#define ATF_STATE (((unsigned int volatile *)ATF_STATE_REG)[0])

#define ATF_WAIT_DEBUG_REG REG_GP_REG0
#define ATF_WAIT_DEBUG_MAGIC 0x6526228C
#define ATF_WAIT_DEBUG_TIMEOUT 1000

/*
 * Firewall register
 */
#define FABFW_ROM_PSMSK 0x5C

/*
 * Arch timer definitions
 */
#define SYS_COUNTER_FREQ_IN_SECOND 25000000

/*
 * If enable, the global variable of emmc/sd clock could be changed by blp
 */
#define SUPPORT_SD_EMMC_CLOCK_ADJUSTMENT

/*
 * UART buadrate and clock
 */
#define PLAT_CONSOLE_BAUDRATE 115200
#define PLAT_UART_CLK_IN_HZ 25000000

/*
 * UART download
 */
#define UART_DL_MAGIC 0x5552444c // "URDL"
#define UART_DL_KERMIT_TIMEROUT 10000 // ms

/*
 * SD/EMMC definitions
 */
#define PLAT_SD_CLK 25000000
#define PLAT_EMMC_CLK 25000000

#define ENABLE_SDIO_IO_CELL_POWER
#define ENABLE_SDIO_SOURCE_SELECT_SETTING

#define EMMC_BUS_WIDTH EMMC_BUS_WIDTH_1
#define DEFAULT_DIV_EMMC_INIT_CLOCK 0x2

/*
 * USB definitions
 */
#define USB_PHY_DETECTION

#endif /* __PLATFORM_DEF_H__ */
