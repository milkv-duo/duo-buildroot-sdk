/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <debug.h>
#include <platform_def.h>

#ifndef __ASSEMBLY__

#include <stdint.h>
#include <types.h>
#include <bl_common.h>
#include <mmio.h>

extern struct _time_records *time_records;
extern struct fip_param1 *fip_param1;

void panic_handler(void) __dead2;
void __system_reset(const char *file, unsigned int line) __dead2;
#define SYSTEM_RESET(...)                                                                                              \
	do {                                                                                                           \
		ERROR(__VA_ARGS__);                                                                                    \
		__system_reset(__FILE__, __LINE__);                                                                    \
	} while (1)

enum boot_src get_boot_src(void);
void set_boot_src(enum boot_src src);
void set_boot_src_from_trap(void);

int get_number_of_retries(void);

int is_uart_dl_enabled(void);
int is_usb_dl_enabled(void);
int is_sd_dl_enabled(void);

static inline union sw_info *get_sw_info(void)
{
	return (union sw_info *)EFUSE_SW_INFO_ADDR;
}

enum CHIP_CONF_CMD {
	CHIP_CONF_SCAN_START_0 = 0,
	CHIP_CONF_SCAN_START_1 = 0xFFFFFFA0,

	CHIP_CONF_CMD_DELAY_US = 0xFFFFFFFE,
	CHIP_CONF_CMD_DELAY_MS = 0xFFFFFFFD
};

enum CHIP_CLK_MODE {
	CLK_ND = 0,
	CLK_OD,
	CLK_VC_OD
};

void apply_chip_conf(const struct chip_conf chip_conf[], uint32_t size, enum CHIP_CONF_CMD scan_start,
		     enum CHIP_CONF_CMD scan_end);

void reset_c906l(uintptr_t reset_address);

void set_pinmux(int io_type);

int sd_get_clk(void);
int emmc_get_clk(void);
void sdio_pad_setting(void);
void sdio_pad_setting_no_card_inserted(void);

void usb_init(void);
int usb_polling(void *buf, uint32_t offset, uint32_t size);
void cv_usb_clk_init(void);
void cv_usb_clk_deinit(void);

int emmc_read_fip(uint32_t offset, uint32_t size, uintptr_t buf);

int sd_open_fip(void);
int sd_load_fip(void *buf, uint32_t offset, uint32_t size);

int kermit_download(void *buf, uint32_t offset, uint32_t size);

int wait_debug_port_polling(void);
int wait_debug_port_allow(void);

int load_image(void *buf, uint32_t offset, size_t image_size, int retry_num);
int device_setup(void);
int flash_init(void);

uint8_t usb_id_det(void);

static inline uintptr_t phys_to_dma(uintptr_t phys)
{
	uintptr_t dma = phys;

	if (IN_RANGE(phys, TPU_SRAM_BASE, TPU_SRAM_SIZE))
		dma = phys - TPU_SRAM_BASE + TPU_SRAM_ORIGIN_BASE;

	return dma;
}

static inline uintptr_t dma_to_phys(uintptr_t dma)
{
	uintptr_t phys = dma;

	if (IN_RANGE(dma, TPU_SRAM_ORIGIN_BASE, TPU_SRAM_SIZE))
		phys = dma - TPU_SRAM_BASE + TPU_SRAM_ORIGIN_BASE;

	return phys;
}

void setup_dl_flag(void);

void switch_rtc_mode_1st_stage(void);
void switch_rtc_mode_2nd_stage(void);
void set_rtc_en_registers(void);

void apply_analog_trimming_data(void);

void sys_pll_init(enum CHIP_CLK_MODE mode);
void sys_switch_all_to_pll(void);

void lock_efuse_chipsn(void);
int load_ddr(void);
int load_rest(enum CHIP_CLK_MODE mode);
void jump_to_warmboot_entry(enum CHIP_CLK_MODE mode);

#endif /* __ASSEMBLY__ */

#endif /* __PLATFORM_H__ */
