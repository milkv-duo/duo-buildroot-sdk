#include <debug.h>
#include <console.h>
#include <platform.h>
#include <bl2.h>
#include <string.h>
#include <delay_timer.h>
#include <rom_api.h>

#ifdef RTOS_ENABLE_FREERTOS
int init_comm_info(int ret)
{
	struct transfer_config_t *transfer_config = (struct transfer_config_t *)MAILBOX_FIELD;
	struct transfer_config_t transfer_config_s;
	unsigned char *ptr = (unsigned char *)&transfer_config_s;
	unsigned short checksum = 0;
	/* mailbox field is 4 byte write access, and can not access byte by byte.
	 * so init parameters and copy all to mailbox field together.
	 */
	transfer_config_s.conf_magic = RTOS_MAGIC_HEADER;
	transfer_config_s.conf_size = ((uint64_t)&transfer_config_s.checksum - (uint64_t)&transfer_config_s.conf_magic);
	transfer_config_s.isp_buffer_addr = CVIMMAP_ISP_MEM_BASE_ADDR;
	transfer_config_s.isp_buffer_size = CVIMMAP_ISP_MEM_BASE_SIZE;
	transfer_config_s.encode_img_addr = CVIMMAP_H26X_BITSTREAM_ADDR;
	transfer_config_s.encode_img_size = CVIMMAP_H26X_BITSTREAM_SIZE;
	transfer_config_s.encode_buf_addr = CVIMMAP_H26X_ENC_BUFF_ADDR;
	transfer_config_s.encode_buf_size = CVIMMAP_H26X_ENC_BUFF_SIZE;
	transfer_config_s.dump_print_enable = RTOS_DUMP_PRINT_ENABLE;
	transfer_config_s.dump_print_size_idx = RTOS_DUMP_PRINT_SZ_IDX;
	transfer_config_s.image_type = RTOS_FAST_IMAGE_TYPE;
	transfer_config_s.mcu_status = MCU_STATUS_NONOS_DONE;
	for (int i = 0; i < transfer_config_s.conf_size; i++) {
		checksum += ptr[i];
	}

	transfer_config_s.checksum = checksum;
	memcpy(transfer_config, &transfer_config_s, sizeof(struct transfer_config_t));

	return ret;
}
#endif

#if !FSBL_SECURE_BOOT_SUPPORT
int dec_verify_image(const void *image, size_t size, size_t dec_skip, struct fip_param1 *fip_param1)
{
	return 0;
}
#endif

// Start of addition
#define UART_DLL 0x04140000
#define UART_DLH 0x04140004
#define UART_LCR 0x0414000C

void set_baudrate()
{
	// 14 for 115200, 13 for 128000
	int baud_divisor = 14;

	// set DLAB to 1 to set dll and dlh
	*(volatile uint32_t*)(UART_LCR) |= (uint32_t)0x80;

	// set divisor
	*(volatile uint32_t*)(UART_DLL) = (uint32_t)(baud_divisor & 0xff);
	*(volatile uint32_t*)(UART_DLH) = (uint32_t)((baud_divisor >> 8) & 0xff);

	// set DLAB back to 0
	*(volatile uint32_t*)(UART_LCR) &= (uint32_t)(~0x80);
}
// End of addition

void bl2_main(void)
{
	// Start of addition
	set_baudrate();
	// End of addition
	
	enum CHIP_CLK_MODE mode;
	uint32_t v = p_rom_api_get_boot_src();

	if (v == BOOT_SRC_UART) {
		console_init(0, PLAT_UART_CLK_IN_HZ, UART_DL_BAUDRATE);
	}

	ATF_STATE = ATF_STATE_BL2_MAIN;
	time_records->fsbl_start = read_time_ms();

	NOTICE("\nFSBL %s:%s\n", version_string, build_message);

	INFO("sw_info=0x%x\n", get_sw_info()->value);
	INFO("fip_param1: param_cksum=0x%x param2_loadaddr=0x%x\n", fip_param1->param_cksum,
	     fip_param1->param2_loadaddr);

	INFO("CP_STATE_REG=0x%x\n", mmio_read_32(0x0E000018));

	// print_sram_log();
	lock_efuse_chipsn();

	setup_dl_flag();

	switch_rtc_mode_1st_stage();

	set_rtc_en_registers();

	load_ddr();

#ifdef OD_CLK_SEL
	mode = CLK_OD;
#else
#ifdef VC_CLK_OVERDRIVE
	mode = CLK_VC_OD;
#else
	mode = CLK_ND;
#endif
#endif
	load_rest(mode);
	NOTICE("BL2 end.\n");

	while (1)
		;
}
