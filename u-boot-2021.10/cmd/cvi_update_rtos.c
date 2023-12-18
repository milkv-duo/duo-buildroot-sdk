/***************************************
 * function: mars alios update
 * *****************************************/

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <ubifs_uboot.h>
#ifdef CONFIG_NAND_SUPPORT
#include <nand.h>
#endif
#include "cvi_update.h"

// #define DEBUG_ALIOS_UPDATE	// Used when developing modes
#define PUBLIC_KEY_NAME_SIZE 8
#define MTB_IMAGE_NAME_SIZE 8
#define MTB_OS_VERSION_LEN_V4 64 /* for version 4.0, os version length is 64*/
#define ONCE_UPDATE_FILE_MAX_SIZE                                              \
	(16 * 1024 * 1024) /* once update file max size */

enum status_type {
	STATUS_OK = 0,
	ERROR_FATSIZE,
	ERROR_FATLOAD,
	ERROR_MMCDEV,
	ERROR_ERASE,
	ERROR_WRITE,
	ERROR_PART,
	ERROR_UNKONW
};

enum storage_device_type {
	MEM_DEVICE_TYPE_EFLASH = 0,
	MEM_DEVICE_TYPE_SPI_NOR_FLASH,
	MEM_DEVICE_TYPE_SPI_NAND_FLASH,
	MEM_DEVICE_TYPE_SD,
	MEM_DEVICE_TYPE_EMMC,
	MEM_DEVICE_TYPE_USB,
	MEM_DEVICE_TYPE_EFUSE,

	MEM_DEVICE_TYPE_MAX
};

struct tb_flag {
	uint16_t encrypted : 1;
	uint16_t reserve : 7;
	uint16_t update_flag : 1;
	uint16_t reserve2 : 7;
};

struct storage_info {
	uint32_t id : 8;
	uint32_t type : 8; // enum storage_device_type
	uint32_t area : 4; // such as emmc area.
	uint32_t hot_plug : 1;
	uint32_t rsv : 11;
};

struct imtb_head_v4 {
	uint32_t magic;
	uint16_t version;
	struct tb_flag flag;
	uint16_t digest_sch;
	uint16_t signature_sch;
	char pub_key_name[PUBLIC_KEY_NAME_SIZE];
	uint16_t partition_count;
	uint16_t size;
};

struct imtb_partition_info_v4 {
	char name[MTB_IMAGE_NAME_SIZE];
	struct storage_info storage_info;
	uint32_t preload_size;
	uint16_t block_count_h;
	uint16_t block_count;
	uint32_t block_offset;
	uint32_t load_address;
	uint32_t img_size;
};

static uint32_t bcd2hex4(uint32_t bcd)
{
	return ((bcd) & 0x0f) + (((bcd) >> 4) & 0xf0) + (((bcd) >> 8) & 0xf00) +
	       (((bcd) >> 12) & 0xf000);
}

#ifdef DEBUG_ALIOS_UPDATE
void show_imtb(struct imtb_partition_info_v4 *part_at)
{
	printf("part_at->name = %s\n", part_at->name);
	printf("part_at->storage_info.type = 0x%x\n",
	       part_at->storage_info.type);
	printf("part_at->storage_info.area = 0x%x\n",
	       part_at->storage_info.area);
	printf("part_at->preload_size = 0x%x\n", part_at->preload_size);
	printf("part_at->block_count_h = 0x%x\n", part_at->block_count_h);
	printf("part_at->block_count = 0x%x\n", part_at->block_count);
	printf("part_at->block_offset = 0x%x\n", part_at->block_offset);
	printf("part_at->load_address = 0x%x\n", part_at->load_address);
	printf("part_at->img_size = 0x%x\n", part_at->img_size);
}
#endif

static int _storage_update_rtos(enum storage_type_e type)
{
	char cmd[255] = { '\0' };
	char strStorage[10] = { '\0' };
	void *load_addr = NULL;
	struct imtb_head_v4 *head = NULL;
	struct imtb_partition_info_v4 *part_at = NULL;
	uint32_t erase_size = 0;
	uint32_t erase_pos = 0, erase_pos_next = 0;
	uint32_t pro_addr = 0, pro_size = 0;
	uint8_t sd_index = 0;
	uint32_t block_count32 = 0, update_file_size = 0;

	if (type == sd_dl) {
		printf("Start SD downloading...\n");
		// Consider SD card with MBR as default
#if defined(CONFIG_NAND_SUPPORT) || defined(CONFIG_SPI_FLASH)
		strlcpy(strStorage, "mmc 0:1", 9);
#elif defined(CONFIG_EMMC_SUPPORT)
		strlcpy(strStorage, "mmc 1:1", 9);
#endif
		snprintf(cmd, 255, "mmc dev %u:1 SD_HS", sd_index);
		run_command(cmd, 0);
		snprintf(cmd, 255, "fatload %s %p imtb;", strStorage,
			 (void *)HEADER_ADDR);
		if (run_command(cmd, 0)) {
			// Consider SD card without MBR
			printf("** Trying use partition 0 (without MBR) **\n");
#if defined(CONFIG_NAND_SUPPORT) || defined(CONFIG_SPI_FLASH)
			strlcpy(strStorage, "mmc 0:0", 9);
			sd_index = 0;
#elif defined(CONFIG_EMMC_SUPPORT)
			sd_index = 1;
			strlcpy(strStorage, "mmc 1:0", 9);
#endif
			snprintf(cmd, 255, "mmc dev %u:0 SD_HS", sd_index);
			run_command(cmd, 0);
			snprintf(cmd, 255, "fatload %s %p imtb;", strStorage,
				 (void *)HEADER_ADDR);
			if (run_command(cmd, 0)) {
				printf("load imtb error\n");
				return -1;
			}
		}
	} else {
		return -1;
	}

#if defined(CONFIG_SPI_FLASH)
	run_command("sf probe", 0);
#endif

	head = (struct imtb_head_v4 *)HEADER_ADDR;
	part_at =
		(struct imtb_partition_info_v4 *)((uint8_t *)head +
						  sizeof(struct imtb_head_v4) +
						  MTB_OS_VERSION_LEN_V4);
	load_addr = part_at + head->partition_count;

	if (part_at->storage_info.type == MEM_DEVICE_TYPE_SPI_NOR_FLASH) {
		snprintf(cmd, 255, "sf erase_size 0x%p;", &erase_size);
		run_command(cmd, 0);
		printf("get flash erase size = 0x%x\n", erase_size);
	}

	for (int i = 0; i < head->partition_count; i++, part_at++) {
#ifdef DEBUG_ALIOS_UPDATE
		show_imtb(part_at);
#else
		enum status_type status = STATUS_OK;
		block_count32 =
			(part_at->block_count_h << 16) | part_at->block_count;

		// The file storage addresses in the imtb go from small to large
		if (part_at->storage_info.type ==
		    MEM_DEVICE_TYPE_SPI_NOR_FLASH) {
			memset(load_addr, 0, block_count32 * 512);
			snprintf(cmd, 255, "fatload %s %p %s;", strStorage,
				 (void *)load_addr, part_at->name);
			if (run_command(cmd, 0)) {
				status = ERROR_FATLOAD;
				printf("program %s failed, skip it!, status=%d\n",
				       part_at->name, status);
				continue;
			}

			pro_addr = part_at->block_offset * 512;
			pro_size = block_count32 * 512;
			if (erase_pos <= (pro_addr + pro_size)) {
				if (pro_addr >= erase_size + erase_pos) {
					erase_pos =
						pro_addr & (~(erase_size - 1));
				}
				erase_pos_next =
					(pro_addr + pro_size + erase_size) &
					(~(erase_size - 1));
				if (erase_pos_next > erase_pos) {
					snprintf(cmd, 255,
						 "sf erase 0x%x 0x%x;",
						 erase_pos,
						 (erase_pos_next - erase_pos));
					run_command(cmd, 0);
					erase_pos = erase_pos_next;
				}
			}

			snprintf(cmd, 255, "sf write %p 0x%x 0x%x",
				 (void *)load_addr,
				 (part_at->block_offset * 512),
				 (block_count32 * 512));
			if (run_command(cmd, 0))
				status = ERROR_WRITE;

		} else if (part_at->storage_info.type == MEM_DEVICE_TYPE_EMMC) {
			/* 1. get update file size */
			snprintf(cmd, 255, "fatsize %s %s;", strStorage,
				 part_at->name);
			if (run_command(cmd, 0)) {
				status = ERROR_FATSIZE;
				printf("get %s size failed, skip it!, status=%d\n",
				       part_at->name, status);
				continue;
			}

			update_file_size = env_get_hex("filesize", 0);
			if (update_file_size <= 0) {
				status = ERROR_FATSIZE;
				printf("get %s size failed, skip it!, status=%d\n",
				       part_at->name, status);
				continue;
			}

			if (update_file_size > (block_count32 * 512)) {
				status = ERROR_FATSIZE;
				printf("%s size too large, skip it!, status=%d\n",
				       part_at->name, status);
				continue;
			}

			/* 2. erase storage partition size */
			snprintf(cmd, 255, "mmc dev 0 %u;",
				 part_at->storage_info.area);
			if (run_command(cmd, 0)) {
				status = ERROR_MMCDEV;
				printf("%s mmc dev failed, skip it!, status=%d\n",
				       part_at->name, status);
				continue;
			}

			snprintf(cmd, 255, "mmc erase 0x%x 0x%x;",
				 part_at->block_offset, block_count32);
			if (run_command(cmd, 0)) {
				status = ERROR_ERASE;
				printf("%s mmc erase failed, skip it!, status=%d\n",
				       part_at->name, status);
				continue;
			}

			/* whether the file needs to be upgraded multiple times */
			if (update_file_size <= ONCE_UPDATE_FILE_MAX_SIZE) {
				/* 3. load update file to dram from sd */
				snprintf(cmd, 255, "fatload %s %p %s;",
					 strStorage, (void *)load_addr,
					 part_at->name);
				if (run_command(cmd, 0)) {
					status = ERROR_FATLOAD;
					printf("program %s failed, skip it!, status=%d\n",
					       part_at->name, status);
					continue;
				}

				/* 4. write update file to storage */
				snprintf(cmd, 255, "mmc write %p 0x%x 0x%x;",
					 (void *)load_addr,
					 part_at->block_offset, block_count32);
				if (run_command(cmd, 0)) {
					status = ERROR_WRITE;
					printf("program %s failed, skip it!, status=%d\n",
					       part_at->name, status);
					continue;
				}

			} else {
				uint32_t offset_blk = 0;
				int file_par_max = update_file_size /
						   ONCE_UPDATE_FILE_MAX_SIZE;
				for (int file_par_num = 0;
				     file_par_num < file_par_max;
				     file_par_num++) {
					/* 3. load update file to dram from sd */
					snprintf(cmd, 255,
						 "fatload %s %p %s 0x%x 0x%x;",
						 strStorage, (void *)load_addr,
						 part_at->name,
						 ONCE_UPDATE_FILE_MAX_SIZE,
						 offset_blk * 512);
					if (run_command(cmd, 0)) {
						status = ERROR_FATLOAD;
						printf("program %s failed, skip it!, status=%d\n",
						       part_at->name, status);
						break;
					}

					/* 4. write update file to storage */
					snprintf(cmd, 255,
						 "mmc write %p 0x%x 0x%x;",
						 (void *)load_addr,
						 part_at->block_offset +
							 offset_blk,
						 ONCE_UPDATE_FILE_MAX_SIZE /
							 512);
					if (run_command(cmd, 0)) {
						status = ERROR_WRITE;
						printf("program %s failed, skip it!, status=%d\n",
						       part_at->name, status);
						break;
					}

					offset_blk +=
						ONCE_UPDATE_FILE_MAX_SIZE / 512;
				}

				if (status != STATUS_OK)
					continue;

				/* write the last part of update file */
				if (update_file_size %
				    ONCE_UPDATE_FILE_MAX_SIZE) {
					uint32_t last_part_size =
						update_file_size -
						offset_blk * 512;

					memset(load_addr, 0,
					       ONCE_UPDATE_FILE_MAX_SIZE);
					/* 3. load update file to dram from sd */
					snprintf(cmd, 255,
						 "fatload %s %p %s 0x%x 0x%x;",
						 strStorage, (void *)load_addr,
						 part_at->name, last_part_size,
						 offset_blk * 512);
					if (run_command(cmd, 0)) {
						status = ERROR_FATLOAD;
						printf("program %s failed, skip it!, status=%d\n",
						       part_at->name, status);
						continue;
					}

					/* 4. write update file to storage */
					snprintf(cmd, 255,
						 "mmc write %p 0x%x 0x%x;",
						 (void *)load_addr,
						 part_at->block_offset +
							 offset_blk,
						 last_part_size % 512 ?
							 last_part_size / 512 +
								 1 :
							 last_part_size / 512);
					if (run_command(cmd, 0)) {
						status = ERROR_WRITE;
						printf("program %s failed, skip it!, status=%d\n",
						       part_at->name, status);
						continue;
					}
				}
			}
		} else {
			printf("Unknown storage type:0x%x\n",
			       part_at->storage_info.type);
			status = ERROR_UNKONW;
		}

		if (status != STATUS_OK) {
			printf("program %s failed, skip it!, status=%d\n",
			       part_at->name, status);
		} else {
			printf("program %s success\n", part_at->name);
		}
#endif
	}

	return 0;
}

static int _usb_update_rtos(uint32_t usb_pid)
{
	int ret = 0;
	char cmd[255] = { '\0' };

	printf("Start USB downloading...\n");

#ifdef CONFIG_SPI_FLASH
	ret = run_command("sf probe", 0);
#endif
	// Clean download flags
	writel(0x0,
	       (unsigned int *)BOOT_SOURCE_FLAG_ADDR); //mw.l 0xe00fc00 0x0;
	// Always download Fip first
	snprintf(cmd, 255, "cvi_utask vid 0x3346 pid 0x%x", usb_pid);
	ret = run_command(cmd, 0);
#ifdef CONFIG_NAND_SUPPORT
	snprintf(cmd, 255, "cvi_sd_update %p spinand fip", (void *)UPDATE_ADDR);
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
#elif defined(CONFIG_SPI_FLASH)
	ret = run_command("sf probe", 0);
#else
	// Switch to boot partition
	run_command("mmc dev 0 1", 0);
	snprintf(cmd, 255, "mmc write %p 0 0x800;", (void *)UPDATE_ADDR);
	pr_debug("%s\n", cmd);
	run_command(cmd, 0);
	snprintf(cmd, 255, "mmc write %p 0x800 0x800;", (void *)UPDATE_ADDR);
	pr_debug("%s\n", cmd);
	run_command(cmd, 0);
	printf("Program fip.bin done\n");
	// Switch to user partition
	run_command("mmc dev 0 0", 0);
#endif

	snprintf(cmd, 255, "cvi_utask vid 0x3346 pid 0x%x", usb_pid);
	while (1) {
		ret = run_command(cmd, 0);
		if (ret) {
			pr_debug("cvi_utask failed(%d)\n", ret);
			return ret;
		}
		//_prgImage((void *)UPDATE_ADDR, readl(HEADER_ADDR + 8));
	};
	return 0;
}

int do_cvi_update_rtos(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
{
	int ret = 1;
	uint32_t usb_pid = 0;
	uint32_t update_magic;

	if (argc == 1) {
		update_magic = readl((unsigned int *)BOOT_SOURCE_FLAG_ADDR);
		if (update_magic == SD_UPDATE_MAGIC) {
			run_command("env default -a", 0);
			ret = _storage_update_rtos(sd_dl);
		} else if (update_magic == USB_UPDATE_MAGIC) {
			run_command("env default -a", 0);
			usb_pid = in_be32(UBOOT_PID_SRAM_ADDR);
			usb_pid = bcd2hex4(usb_pid);
			ret = _usb_update_rtos(usb_pid);
		}
	} else {
		printf("Usage:\n%s\n", cmdtp->usage);
	}

	return ret;
}

U_BOOT_CMD(cvi_update_rtos, 2, 0, do_cvi_update_rtos,
	   "cvi_update_rtos [eth, sd, usb]- check boot status and update if necessary\n",
	   "run cvi_update without parameter will check the boot status and try to update"
);
