/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <debug.h>
#include <bitwise_ops.h>
#include <console.h>
#include <platform.h>
#include <rom_api.h>
#include <bl2.h>
#include <ddr.h>
#include <ddr_sys.h>
#include <rtc.h>
#include <string.h>
#include <decompress.h>
#include <delay_timer.h>
#include <security/security.h>
#include <cv_usb.h>

struct _time_records *time_records = (void *)TIME_RECORDS_ADDR;
struct fip_param1 *fip_param1 = (void *)PARAM1_BASE;
enum CHIP_CLK_MODE chip_clk_mode = CLK_ND;
static struct fip_param2 fip_param2 __aligned(BLOCK_SIZE);
static union {
	struct ddr_param ddr_param;
	struct loader_2nd_header loader_2nd_header;
	uint8_t buf[BLOCK_SIZE];
} sram_union_buf __aligned(BLOCK_SIZE);

int init_comm_info(int ret) __attribute__((weak));
int init_comm_info(int ret)
{
	return ret;
}

void print_sram_log(void)
{
	uint32_t *const log_size = (void *)BOOT_LOG_LEN_ADDR;
	uint8_t *const log_buf = (void *)phys_to_dma(BOOT_LOG_BUF_BASE);
	uint32_t i;

	static const char m1[] = "\nSRAM Log: ========================================\n";
	static const char m2[] = "\nSRAM Log end: ====================================\n";

	for (i = 0; m1[i]; i++)
		console_putc(m1[i]);

	for (i = 0; i < *log_size; i++)
		console_putc(log_buf[i]);

	for (i = 0; m2[i]; i++)
		console_putc(m2[i]);
}

void lock_efuse_chipsn(void)
{
	int value = mmio_read_32(EFUSE_W_LOCK0_REG);

	if (efuse_power_on()) {
		NOTICE("efuse power on fail\n");
		return;
	}

	if ((value & (0x1 << BIT_FTSN3_LOCK)) == 0)
		efuse_program_bit(0x26, BIT_FTSN3_LOCK);

	if ((value & (0x1 << BIT_FTSN4_LOCK)) == 0)
		efuse_program_bit(0x26, BIT_FTSN4_LOCK);

	if (efuse_refresh_shadow()) {
		NOTICE("efuse refresh shadow fail\n");
		return;
	}

	value = mmio_read_32(EFUSE_W_LOCK0_REG);
	if (((value & (0x3 << BIT_FTSN3_LOCK)) >> BIT_FTSN3_LOCK) !=  0x3)
		NOTICE("lock efuse chipsn fail\n");

	if (efuse_power_off()) {
		NOTICE("efuse power off fail\n");
		return;
	}
}

#ifdef USB_DL_BY_FSBL
int load_image_by_usb(void *buf, uint32_t offset, size_t image_size, int retry_num)
{

	int ret = -1;

	if (usb_polling(buf, offset, image_size) == CV_USB_DL)
		ret = 0;
	else
		ret = -2;

	INFO("LIE/%d/%p/0x%x/%lu.\n", ret, buf, offset, image_size);

	return ret;
}
#endif

int load_param2(int retry)
{
	uint32_t crc;
	int ret = -1;

	NOTICE("P2S/0x%lx/%p.\n", sizeof(fip_param2), &fip_param2);

#ifdef USB_DL_BY_FSBL
	if (p_rom_api_get_boot_src() == BOOT_SRC_USB)
		ret = load_image_by_usb(&fip_param2, fip_param1->param2_loadaddr, PARAM2_SIZE, retry);
	else
#endif
		ret = p_rom_api_load_image(&fip_param2, fip_param1->param2_loadaddr, PARAM2_SIZE, retry);

	if (ret < 0) {
		return ret;
	}

	if (fip_param2.magic1 != FIP_PARAM2_MAGIC1) {
		WARN("LP2_NOMAGIC\n");
		return -1;
	}

	crc = p_rom_api_image_crc(&fip_param2.reserved1, sizeof(fip_param2) - 12);
	if (crc != fip_param2.param2_cksum) {
		ERROR("param2_cksum (0x%x/0x%x)\n", crc, fip_param2.param2_cksum);
		return -1;
	}

	NOTICE("P2E.\n");

	return 0;
}

int load_ddr_param(int retry)
{
	uint32_t crc;
	int ret = -1;

	NOTICE("DPS/0x%x/0x%x.\n", fip_param2.ddr_param_loadaddr, fip_param2.ddr_param_size);

	if (fip_param2.ddr_param_size >= sizeof(sram_union_buf.ddr_param))
		fip_param2.ddr_param_size = sizeof(sram_union_buf.ddr_param);

#ifdef USB_DL_BY_FSBL
	if (p_rom_api_get_boot_src() == BOOT_SRC_USB)
		ret = load_image_by_usb(&sram_union_buf.ddr_param, fip_param2.ddr_param_loadaddr,
		fip_param2.ddr_param_size,  retry);
	else
#endif
		ret = p_rom_api_load_image(&sram_union_buf.ddr_param, fip_param2.ddr_param_loadaddr,
		fip_param2.ddr_param_size, retry);
	if (ret < 0) {
		return ret;
	}

	crc = p_rom_api_image_crc(&sram_union_buf.ddr_param, fip_param2.ddr_param_size);
	if (crc != fip_param2.ddr_param_cksum) {
		ERROR("ddr_param_cksum (0x%x/0x%x)\n", crc, fip_param2.ddr_param_cksum);
		return -1;
	}

	NOTICE("DPE.\n");

	return 0;
}

int load_ddr(void)
{
	int retry = 0;

retry_from_flash:
	for (retry = 0; retry < p_rom_api_get_number_of_retries(); retry++) {
		if (load_param2(retry) < 0)
			continue;
		if (load_ddr_param(retry) < 0)
			continue;

		break;
	}

	if (retry >= p_rom_api_get_number_of_retries()) {
		switch (p_rom_api_get_boot_src()) {
		case BOOT_SRC_UART:
		case BOOT_SRC_SD:
		case BOOT_SRC_USB:
			WARN("DL cancelled. Load flash. (%d).\n", retry);
			// Continue to boot from flash if boot from external source
			p_rom_api_flash_init();
			goto retry_from_flash;
		default:
			ERROR("Failed to load DDR param (%d).\n", retry);
			panic_handler();
		}
	}

	time_records->ddr_init_start = read_time_ms();
	ddr_init(&sram_union_buf.ddr_param);
	time_records->ddr_init_end = read_time_ms();

	return 0;
}

int load_blcp_2nd(int retry)
{
	uint32_t crc, rtos_base;
	int ret = -1;

	// if no blcp_2nd, release_blcp_2nd should be ddr_init_end
	time_records->release_blcp_2nd = time_records->ddr_init_end;

	NOTICE("C2S/0x%x/0x%x/0x%x.\n", fip_param2.blcp_2nd_loadaddr, fip_param2.blcp_2nd_runaddr,
	       fip_param2.blcp_2nd_size);

	if (!fip_param2.blcp_2nd_runaddr) {
		NOTICE("No C906L image.\n");
		return 0;
	}

	if (!IN_RANGE(fip_param2.blcp_2nd_runaddr, DRAM_BASE, DRAM_SIZE)) {
		ERROR("blcp_2nd_runaddr (0x%x) is not in DRAM.\n", fip_param2.blcp_2nd_runaddr);
		panic_handler();
	}

	if (!IN_RANGE(fip_param2.blcp_2nd_runaddr + fip_param2.blcp_2nd_size, DRAM_BASE, DRAM_SIZE)) {
		ERROR("blcp_2nd_size (0x%x) is not in DRAM.\n", fip_param2.blcp_2nd_size);
		panic_handler();
	}

#ifdef USB_DL_BY_FSBL
	if (p_rom_api_get_boot_src() == BOOT_SRC_USB)
		ret = load_image_by_usb((void *)(uintptr_t)fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_loadaddr,
					fip_param2.blcp_2nd_size, retry);
	else
#endif
		ret = p_rom_api_load_image((void *)(uintptr_t)fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_loadaddr,
					fip_param2.blcp_2nd_size, retry);
	if (ret < 0) {
		return ret;
	}

	crc = p_rom_api_image_crc((void *)(uintptr_t)fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_size);
	if (crc != fip_param2.blcp_2nd_cksum) {
		ERROR("blcp_2nd_cksum (0x%x/0x%x)\n", crc, fip_param2.blcp_2nd_cksum);
		return -1;
	}

	ret = dec_verify_image((void *)(uintptr_t)fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_size, 0, fip_param1);
	if (ret < 0) {
		ERROR("verify blcp 2nd (%d)\n", ret);
		return ret;
	}

	flush_dcache_range(fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_size);

	rtos_base = mmio_read_32(AXI_SRAM_RTOS_BASE);
	init_comm_info(0);

	time_records->release_blcp_2nd = read_time_ms();
	if (rtos_base == CVI_RTOS_MAGIC_CODE) {
		mmio_write_32(AXI_SRAM_RTOS_BASE, fip_param2.blcp_2nd_runaddr);
	} else {
		reset_c906l(fip_param2.blcp_2nd_runaddr);
	}

	NOTICE("C2E.\n");

	return 0;
}

int load_monitor(int retry, uint64_t *monitor_entry)
{
	uint32_t crc;
	int ret = -1;

	NOTICE("MS/0x%x/0x%x/0x%x.\n", fip_param2.monitor_loadaddr, fip_param2.monitor_runaddr,
	       fip_param2.monitor_size);

	if (!fip_param2.monitor_runaddr) {
		NOTICE("No monitor.\n");
		return 0;
	}

	if (!IN_RANGE(fip_param2.monitor_runaddr, DRAM_BASE, DRAM_SIZE)) {
		ERROR("monitor_runaddr (0x%x) is not in DRAM.\n", fip_param2.monitor_runaddr);
		panic_handler();
	}

	if (!IN_RANGE(fip_param2.monitor_runaddr + fip_param2.monitor_size, DRAM_BASE, DRAM_SIZE)) {
		ERROR("monitor_size (0x%x) is not in DRAM.\n", fip_param2.monitor_size);
		panic_handler();
	}

#ifdef USB_DL_BY_FSBL
	if (p_rom_api_get_boot_src() == BOOT_SRC_USB)
		ret = load_image_by_usb((void *)(uintptr_t)fip_param2.monitor_runaddr, fip_param2.monitor_loadaddr,
					fip_param2.monitor_size, retry);
	else
#endif
		ret = p_rom_api_load_image((void *)(uintptr_t)fip_param2.monitor_runaddr, fip_param2.monitor_loadaddr,
					fip_param2.monitor_size, retry);
	if (ret < 0) {
		return ret;
	}

	crc = p_rom_api_image_crc((void *)(uintptr_t)fip_param2.monitor_runaddr, fip_param2.monitor_size);
	if (crc != fip_param2.monitor_cksum) {
		ERROR("monitor_cksum (0x%x/0x%x)\n", crc, fip_param2.monitor_cksum);
		return -1;
	}

	ret = dec_verify_image((void *)(uintptr_t)fip_param2.monitor_runaddr, fip_param2.monitor_size, 0, fip_param1);
	if (ret < 0) {
		ERROR("verify monitor (%d)\n", ret);
		return ret;
	}

	flush_dcache_range(fip_param2.monitor_runaddr, fip_param2.monitor_size);
	NOTICE("ME.\n");

	*monitor_entry = fip_param2.monitor_runaddr;

	return 0;
}

int load_loader_2nd(int retry, uint64_t *loader_2nd_entry)
{
	struct loader_2nd_header *loader_2nd_header = &sram_union_buf.loader_2nd_header;
	uint32_t crc;
	int ret = -1;
	const int cksum_offset =
		offsetof(struct loader_2nd_header, cksum) + sizeof(((struct loader_2nd_header *)0)->cksum);

	enum COMPRESS_TYPE comp_type = COMP_NONE;
	int reading_size;
	void *image_buf;

	NOTICE("L2/0x%x.\n", fip_param2.loader_2nd_loadaddr);

#ifdef USB_DL_BY_FSBL
	if (p_rom_api_get_boot_src() == BOOT_SRC_USB)
		ret = load_image_by_usb(loader_2nd_header, fip_param2.loader_2nd_loadaddr, BLOCK_SIZE, retry);
	else
#endif
		ret = p_rom_api_load_image(loader_2nd_header, fip_param2.loader_2nd_loadaddr, BLOCK_SIZE, retry);
	if (ret < 0) {
		return -1;
	}

	reading_size = ROUND_UP(loader_2nd_header->size, BLOCK_SIZE);

	NOTICE("L2/0x%x/0x%x/0x%lx/0x%x/0x%x\n", loader_2nd_header->magic, loader_2nd_header->cksum,
	       loader_2nd_header->runaddr, loader_2nd_header->size, reading_size);

	switch (loader_2nd_header->magic) {
	case LOADER_2ND_MAGIC_LZMA:
		comp_type = COMP_LZMA;
		break;
	case LOADER_2ND_MAGIC_LZ4:
		comp_type = COMP_LZ4;
		break;
	default:
		comp_type = COMP_NONE;
		break;
	}

	if (comp_type) {
		NOTICE("COMP/%d.\n", comp_type);
		image_buf = (void *)DECOMP_BUF_ADDR;
	} else {
		image_buf = (void *)loader_2nd_header->runaddr;
	}

#ifdef USB_DL_BY_FSBL
	if (p_rom_api_get_boot_src() == BOOT_SRC_USB)
		ret = load_image_by_usb(image_buf, fip_param2.loader_2nd_loadaddr, reading_size, retry);
	else
#endif
		ret = p_rom_api_load_image(image_buf, fip_param2.loader_2nd_loadaddr, reading_size, retry);
	if (ret < 0) {
		return -1;
	}

	crc = p_rom_api_image_crc(image_buf + cksum_offset, loader_2nd_header->size - cksum_offset);
	if (crc != loader_2nd_header->cksum) {
		ERROR("loader_2nd_cksum (0x%x/0x%x)\n", crc, loader_2nd_header->cksum);
		return -1;
	}

	ret = dec_verify_image(image_buf + cksum_offset, loader_2nd_header->size - cksum_offset,
			       sizeof(struct loader_2nd_header) - cksum_offset, fip_param1);
	if (ret < 0) {
		ERROR("verify loader 2nd (%d)\n", ret);
		return ret;
	}

	time_records->load_loader_2nd_end = read_time_ms();

	sys_switch_all_to_pll();

	time_records->fsbl_decomp_start = read_time_ms();
	if (comp_type) {
		size_t dst_size = DECOMP_DST_SIZE;

		// header is not compressed.
		void *dst = (void *)loader_2nd_header->runaddr;

		memcpy(dst, image_buf, sizeof(struct loader_2nd_header));
		image_buf += sizeof(struct loader_2nd_header);

		ret = decompress(dst + sizeof(struct loader_2nd_header), &dst_size, image_buf, loader_2nd_header->size,
				 comp_type);
		if (ret < 0) {
			ERROR("Failed to decompress loader_2nd (%d/%lu)\n", ret, dst_size);
			return -1;
		}

		reading_size = dst_size;
	}

	flush_dcache_range(loader_2nd_header->runaddr, reading_size);
	time_records->fsbl_decomp_end = read_time_ms();
	NOTICE("Loader_2nd loaded.\n");

	*loader_2nd_entry = loader_2nd_header->runaddr + sizeof(struct loader_2nd_header);

	return 0;
}

#ifndef NO_DDR_CFG //for fpga
static void *get_warmboot_entry(void)
{
	/*
	 * "FSM state change to ST_ON from the state
	 * 4'h0 = state changed from ST_OFF to ST_ON
	 * 4'h3 = state changed to ST_PWR_CYC or ST_WARM_RESET then back to ST_ON
	 * 4'h9 = state changed from ST_SUSP to ST_ON
	 */
#define WANTED_STATE 0x9

	NOTICE("\nREG_RTC_ST_ON_REASON=0x%x\n", mmio_read_32(REG_RTC_ST_ON_REASON));
	NOTICE("\nRTC_SRAM_FLAG_ADDR%x=0x%x\n", RTC_SRAM_FLAG_ADDR, mmio_read_32(RTC_SRAM_FLAG_ADDR));
	/* Check if RTC state changed from ST_SUSP */
	if ((mmio_read_32(REG_RTC_ST_ON_REASON) & 0xF) == WANTED_STATE)
		return (void *)(uintptr_t)mmio_read_32(RTC_SRAM_FLAG_ADDR);

	return 0;
}
#endif
void rtc_set_ddr_pwrok(void)
{
	mmio_setbits_32(REG_RTC_BASE + RTC_PG_REG, 0x00000001);
}

void rtc_set_rmio_pwrok(void)
{
	mmio_setbits_32(REG_RTC_BASE + RTC_PG_REG, 0x00000002);
}

#ifndef NO_DDR_CFG //for fpga
static void ddr_resume(void)
{
	rtc_set_ddr_pwrok();
	rtc_set_rmio_pwrok();
	ddr_sys_resume();
}

void jump_to_warmboot_entry(void)
{
	void (*warmboot_entry)() = get_warmboot_entry();

	// treat next reset as normal boot
	mmio_write_64(RTC_SRAM_FLAG_ADDR, 0);
	if (warmboot_entry) {
		INFO("WE=0x%lx\n", (uintptr_t)warmboot_entry);

		NOTICE("ddr resume...\n");
		ddr_resume();
		NOTICE("ddr resume end\n");

		sys_pll_init();
		sys_switch_all_to_pll();

		warmboot_entry();
	}
}
#endif
int load_rest(void)
{
	int retry = 0;
	uint64_t monitor_entry = 0;
	uint64_t loader_2nd_entry = 0;

	// Init sys PLL and switch clocks to PLL
	sys_pll_init();

retry_from_flash:
	for (retry = 0; retry < p_rom_api_get_number_of_retries(); retry++) {
		if (load_blcp_2nd(retry) < 0)
			continue;

		if (load_monitor(retry, &monitor_entry) < 0)
			continue;

		if (load_loader_2nd(retry, &loader_2nd_entry) < 0)
			continue;

		break;
	}

	if (retry >= p_rom_api_get_number_of_retries()) {
		switch (p_rom_api_get_boot_src()) {
		case BOOT_SRC_UART:
		case BOOT_SRC_SD:
		case BOOT_SRC_USB:
			WARN("DL cancelled. Load flash. (%d).\n", retry);
			// Continue to boot from flash if boot from external source
			p_rom_api_flash_init();
			goto retry_from_flash;
		default:
			ERROR("Failed to load rest (%d).\n", retry);
			panic_handler();
		}
	}

	sync_cache();
	console_flush();

	switch_rtc_mode_2nd_stage();

	if (monitor_entry) {
		NOTICE("Jump to monitor at 0x%lx.\n", monitor_entry);
		jump_to_monitor(monitor_entry, loader_2nd_entry);
	} else {
		NOTICE("Jump to loader_2nd at 0x%lx.\n", loader_2nd_entry);
		jump_to_loader_2nd(loader_2nd_entry);
	}

	return 0;
}

int load_rest_od_sel(void)
{
	int retry = 0;
	uint64_t monitor_entry = 0;
	uint64_t loader_2nd_entry = 0;

	// Init sys PLL and switch clocks to PLL
	sys_pll_init_od_sel();

retry_from_flash:
	for (retry = 0; retry < p_rom_api_get_number_of_retries(); retry++) {
		if (load_blcp_2nd(retry) < 0)
			continue;

		if (load_monitor(retry, &monitor_entry) < 0)
			continue;

		if (load_loader_2nd(retry, &loader_2nd_entry) < 0)
			continue;

		break;
	}

	if (retry >= p_rom_api_get_number_of_retries()) {
		switch (p_rom_api_get_boot_src()) {
		case BOOT_SRC_UART:
		case BOOT_SRC_SD:
		case BOOT_SRC_USB:
			WARN("DL cancelled. Load flash. (%d).\n", retry);
			// Continue to boot from flash if boot from external source
			p_rom_api_flash_init();
			goto retry_from_flash;
		default:
			ERROR("Failed to load rest (%d).\n", retry);
			panic_handler();
		}
	}

	sync_cache();
	console_flush();

	switch_rtc_mode_2nd_stage();

	if (monitor_entry) {
		NOTICE("Jump to monitor at 0x%lx.\n", monitor_entry);
		jump_to_monitor(monitor_entry, loader_2nd_entry);
	} else {
		NOTICE("Jump to loader_2nd at 0x%lx.\n", loader_2nd_entry);
		jump_to_loader_2nd(loader_2nd_entry);
	}

	return 0;
}