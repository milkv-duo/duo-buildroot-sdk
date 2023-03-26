#include <debug.h>
#include <console.h>
#include <platform.h>
#include <bl2.h>
#include <string.h>
#include <delay_timer.h>

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

void bl2_main(void)
{
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
	load_rest_od_sel();
#else
	load_rest();
#endif

	NOTICE("BL2 end.\n");

	while (1)
		;
}
