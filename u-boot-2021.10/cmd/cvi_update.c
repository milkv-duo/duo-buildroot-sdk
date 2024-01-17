#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <imgs.h>
#include <ubifs_uboot.h>
#include <serial.h>
#include <asm/global_data.h>
#include <linux/delay.h>
#ifdef CONFIG_NAND_SUPPORT
#include <nand.h>
#endif
#include "cvi_update.h"

#define HEADER_SIZE 64
#ifdef CONFIG_CMD_SAVEENV
#define SET_DL_COMPLETE()			\
	do {							\
		env_set("dl_flag", "prog");	\
		run_command("saveenv", 0);	\
	} while (0)
#else
#define SET_DL_COMPLETE() writel(0x50524F47, (unsigned int *)UPGRADE_SRAM_ADDR)
#endif /* CONFIG_CMD_SAVEENV */

#ifdef CONFIG_NAND_SUPPORT
static uint32_t lastend;
#endif

uint32_t update_magic;


#if (!defined CONFIG_TARGET_CVITEK_CV181X_FPGA) && (!defined CONFIG_TARGET_CVITEK_ATHENA2_FPGA) && \
	(!defined ATHENA2_FPGA_PALLDIUM_ENV)
static uint32_t bcd2hex4(uint32_t bcd)
{
	return ((bcd) & 0x0f) + (((bcd) >> 4) & 0xf0) + (((bcd) >> 8) & 0xf00) + (((bcd) >> 12) & 0xf000);
}

static int _storage_update(enum storage_type_e type);
#endif

int _prgImage(char *file, uint32_t chunk_header_size, char *file_name)
{
	uint32_t size = *(uint32_t *)((uintptr_t)file + 4);
	uint32_t offset = *(uint32_t *)((uintptr_t)file + 8);
#if (defined CONFIG_SPI_FLASH)/* || (defined CONFIG_NAND_SUPPORT)*/
	uint32_t part_size = *(uint32_t *)((uintptr_t)file + 12);
#endif
	//uint32_t header_crc = *(uint32_t *)((uintptr_t)file + 16);
	char cmd[255] = { '\0' };
	int ret = 0;

	//if (chunk_type == check_crc) {
	//	uint32_t crc = crc32(
	//		0, (unsigned char *)file + chunk_header_size, size);
	//	if (crc != header_crc) {
	//		printf("Crc check failed header(0x%08x) img(0x%08x), skip it\n",
	//		       header_crc, crc);
	//		return 0;
	//	} else {
	//		/* Invalidate crc to avoid program garbage */
	//		*(uint32_t *)((uintptr_t)file + 12) = 0;
	//	}
	//}
#ifdef CONFIG_NAND_SUPPORT
	int dev = nand_curr_device;
	struct mtd_info *mtd = nand_info[dev];
	uint32_t goodblocks = 0, blocks = 0;

	// Calculate real offset when programming chunk.
	if (offset < lastend)
		offset = lastend;
	else
		lastend = offset;

	blocks = (size & (mtd->erasesize - 1)) ? ALIGN(size, mtd->erasesize) : size;
	blocks /= mtd->erasesize;
	for (; goodblocks  < blocks; lastend += mtd->erasesize) {
		if (!nand_block_isbad(mtd, lastend))
			goodblocks++;
	}
	//pr_debug("offset:0x%x lastoffset:0x%x, end:0x%x\n", offset, lastend, part_size + offset);

	snprintf(cmd, 255, "nand write %p 0x%x 0x%x",
		 (void *)file + chunk_header_size, offset, size);
#elif defined(CONFIG_SPI_FLASH)
	if (update_magic == SD_UPDATE_MAGIC && (!strcmp(file_name, "fip_spl.bin")
		|| !strcmp(file_name, "fip.bin")
		|| !strcmp(file_name, "boot.spinor")
		|| !strcmp(file_name, "rootfs.spinor"))) {
		snprintf(cmd, 255, "sf update %p 0x%x 0x%x",
			 (void *)file + chunk_header_size, offset, size);
	} else {
		snprintf(cmd, 255, "sf erase %#x %#x;", offset, part_size);
		pr_debug("%s\n", cmd);
		run_command(cmd, 0);
		snprintf(cmd, 255, "sf write %p 0x%x 0x%x",
			 (void *)file + chunk_header_size, offset, size);
	}
#else
	if (size & (SECTOR_SIZE - 1))
		size = ALIGN(size, SECTOR_SIZE);

	size = size / SECTOR_SIZE;
	offset = offset / SECTOR_SIZE;
	snprintf(cmd, 255, "mmc write %p 0x%x 0x%x",
		 (void *)file + chunk_header_size, offset, size);
#endif
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
	if (ret)
		return 0;

	return size;
}

#if (!defined CONFIG_TARGET_CVITEK_CV181X_FPGA) && (!defined CONFIG_TARGET_CVITEK_ATHENA2_FPGA) && \
	(!defined ATHENA2_FPGA_PALLDIUM_ENV)
static int _checkHeader(char *file, char strStorage[10])
{
	char *magic = (void *)HEADER_ADDR;
	uint32_t version = *(uint32_t *)((uintptr_t)HEADER_ADDR + 4);
	uint32_t chunk_sz = *(uint32_t *)((uintptr_t)HEADER_ADDR + 8);
	uint32_t total_chunk = *(uint32_t *)((uintptr_t)HEADER_ADDR + 12);
	uint32_t file_sz = *(uint32_t *)((uintptr_t)HEADER_ADDR + 16);
#ifdef CONFIG_NAND_SUPPORT
	char *extra = (void *)((uintptr_t)HEADER_ADDR + 20);
	static char prevExtra[EXTRA_FLAG_SIZE + 1] = { '\0' };
#endif
	int ret = strncmp(magic, HEADER_MAGIC, 4);

	if (ret) {
		printf("File:%s Magic number is wrong, skip it\n", file);
		return ret;
	}
	printf("Header Version:%d\n", version);
	char cmd[255] = { '\0' };
	uint32_t pos = HEADER_SIZE;
#ifdef CONFIG_NAND_SUPPORT
	// Erase partition first
	if (strncmp(extra, prevExtra, EXTRA_FLAG_SIZE)) {
		strncpy(prevExtra, extra, EXTRA_FLAG_SIZE);
		snprintf(cmd, 255, "nand erase.part -y %s", prevExtra);
		pr_debug("%s\n", cmd);
		run_command(cmd, 0);
	}
#endif
	for (int i = 0; i < total_chunk; i++) {
		uint32_t load_size = file_sz > (MAX_LOADSIZE + chunk_sz) ?
				     MAX_LOADSIZE + chunk_sz :
				     file_sz;
		snprintf(cmd, 255, "fatload %s %p %s 0x%x 0x%x;", strStorage,
			 (void *)UPDATE_ADDR, file, load_size, pos);
		pr_debug("%s\n", cmd);
		ret = run_command(cmd, 0);
		if (ret)
			return ret;

		ret = _prgImage((void *)UPDATE_ADDR, chunk_sz, file);
		if (ret == 0) {
			printf("program file:%s failed\n", file);
			break;
		}
		pos += load_size;
		file_sz -= load_size;
	}
	return 0;
}

static int _storage_update(enum storage_type_e type)
{
	int ret = 0;
	char cmd[255] = { '\0' };
	char strStorage[10] = { '\0' };
	uint8_t sd_index = 0;
	uint8_t fip_name[16] = {0};

	if (type == sd_dl) {
		printf("Start SD downloading...\n");
		// Consider SD card with MBR as default
#if defined(CONFIG_NAND_SUPPORT) || defined(CONFIG_SPI_FLASH)
		strlcpy(strStorage, "mmc 0:1", 9);
		sd_index = 0;
#elif defined(CONFIG_EMMC_SUPPORT)
		sd_index = 1;
		strlcpy(strStorage, "mmc 1:1", 9);
#endif
		snprintf(cmd, 255, "mmc dev %u:1 SD_HS", sd_index);
		run_command(cmd, 0);
#if defined(CONFIG_SPL)
		strcpy(fip_name, "fip_spl.bin");
#else
		strcpy(fip_name, "fip.bin");
#endif
		snprintf(cmd, 255, "fatload %s %p %s;", strStorage,
			 (void *)HEADER_ADDR, fip_name);
		ret = run_command(cmd, 0);
		if (ret) {
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
			snprintf(cmd, 255, "fatload %s %p %s;", strStorage,
				 (void *)HEADER_ADDR, fip_name);
			ret = run_command(cmd, 0);
			if (ret)
				return ret;
		}
#if defined(CONFIG_NAND_SUPPORT)
		snprintf(cmd, 255, "cvi_sd_update %p spinand fip",
			 (void *)HEADER_ADDR);
		ret = run_command(cmd, 0);
#elif defined(CONFIG_SPI_FLASH)
		run_command("sf probe", 0);
		snprintf(cmd, 255,
			 "sf update %p ${fip_PART_OFFSET} ${filesize};",
			 (void *)HEADER_ADDR);
		ret = run_command(cmd, 0);
#elif defined(CONFIG_EMMC_SUPPORT)
		// Switch to boot partition
		run_command("mmc dev 0 1", 0);
		snprintf(cmd, 255, "mmc write %p 0 0x800;",
			 (void *)HEADER_ADDR);
		run_command(cmd, 0);
		snprintf(cmd, 255, "mmc write %p 0x800 0x800;;",
			 (void *)HEADER_ADDR);
		ret = run_command(cmd, 0);
		printf("Program fip.bin done\n");
		// Switch to user partition
		run_command("mmc dev 0 0", 0);
#endif
		if (ret == 0)
			SET_DL_COMPLETE();
		else
			return ret;
	}
	for (int i = 1; i < ARRAY_SIZE(imgs); i++) {
		snprintf(cmd, 255, "fatload %s %p %s 0x%x 0;", strStorage,
			 (void *)HEADER_ADDR, imgs[i], HEADER_SIZE);
		pr_debug("%s\n", cmd);
		ret = run_command(cmd, 0);
		if (ret) {
			printf("load %s failed, skip it!\n", imgs[i]);
			continue;
		}
		if (_checkHeader(imgs[i], strStorage))
			continue;
	}
	return 0;
}

static int _usb_update(uint32_t usb_pid)
{
	int ret = 0;
	char cmd[255] = { '\0' };
	char utask_cmd[255] = { '\0' };

	printf("Start USB downloading...\n");

	// Clean download flags
	writel(0x0, (unsigned int *)BOOT_SOURCE_FLAG_ADDR); //mw.l 0xe00fc00 0x0;
	// Always download Fip first
	snprintf(utask_cmd, 255, "cvi_utask vid 0x3346 pid 0x%x", usb_pid);
	ret = run_command(utask_cmd, 0);
#ifdef CONFIG_NAND_SUPPORT
	snprintf(cmd, 255, "cvi_sd_update %p spinand fip", (void *)UPDATE_ADDR);
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
#elif defined(CONFIG_SPI_FLASH)
	ret = run_command("sf probe", 0);
	snprintf(cmd, 255, "sf update %p ${fip_PART_OFFSET} ${fip_PART_SIZE};", (void *)UPDATE_ADDR)
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
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
	// Since device will reset by host tool, set flag first
	SET_DL_COMPLETE();
	while (1) {
		ret = run_command(utask_cmd, 0);
		if (ret) {
			pr_debug("cvi_utask failed(%d)\n", ret);
			return ret;
		}
		//_prgImage((void *)UPDATE_ADDR, readl(HEADER_ADDR + 8));
	};
	return 0;
}
#endif

DECLARE_GLOBAL_DATA_PTR;
static void set_baudrate(unsigned int baudrate)
{
	mdelay(50);
	gd->baudrate = baudrate;
	serial_setbrg();
	mdelay(50);
}

int uart_download(void *buf, const char *filename)
{
	int ret = 0;
	char cmd[255] = { '\0' };

	snprintf(cmd, 255, "loadb %p %d ", (void *)HEADER_ADDR, UART_DL_BAUDRATE);
	ret = run_command(cmd, 0);
	if (ret)
		return ret;

	char *magic = (void *)HEADER_ADDR;

	if (!strncmp(magic, "O", 1)) {
		printf("File %s not exist, skip it!\n", filename);
		return ret;
	}

	uint32_t chunk_header_sz = *(uint32_t *)((uintptr_t)HEADER_ADDR + 8);

	ret = strncmp(magic, HEADER_MAGIC, 4);
	if (ret) {
		printf("File %s's magic number is wrong, skip it!\n", filename);
		return ret;
	}

	ret = _prgImage((void *)(HEADER_ADDR + HEADER_SIZE), chunk_header_sz, NULL);
	if (ret == 0) {
		printf("Program file %s failed!\n", filename);
		return ret;
	}
	return 0;
}

static int _uart_update(void)
{
	int ret = 0;
	char cmd[255] = { '\0' };

	printf("Start UART downloading... Change boadrate to %d\n", UART_DL_BAUDRATE);
	set_baudrate(UART_DL_BAUDRATE);

	snprintf(cmd, 255, "loadb %p %d ", (void *)HEADER_ADDR, UART_DL_BAUDRATE);
	ret = run_command(cmd, 0);
	if (ret) {
		printf("Download fip.bin failed!\n");
		return ret;
	}

#ifdef CONFIG_NAND_SUPPORT
	snprintf(cmd, 255, "cvi_sd_update %p spinand fip", (void *)UPDATE_ADDR);
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
#elif defined(CONFIG_SPI_FLASH)
	ret = run_command("sf probe", 0);
	snprintf(cmd, 255, "sf update %p ${fip_PART_OFFSET} ${fip_PART_SIZE};", (void *)UPDATE_ADDR)
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
#else
	// Switch to boot partition
	ret = run_command("mmc dev 0 1", 0);
	snprintf(cmd, 255, "mmc write %p 0 0x800;", (void *)UPDATE_ADDR);
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
	snprintf(cmd, 255, "mmc write %p 0x800 0x800;", (void *)UPDATE_ADDR);
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
	// Switch to user partition
	ret = run_command("mmc dev 0 0", 0);
#endif
	if (ret) {
		printf("Program fip.bin failed!\n");
		return ret;
	}

	SET_DL_COMPLETE();
	printf("Program fip.bin done\n");

	for (int i = 1; i < ARRAY_SIZE(imgs); i++) {
		ret = uart_download((void *)HEADER_ADDR, imgs[i]);
		if (ret) {
			printf("Load %s failed, skip it!\n", imgs[i]);
			continue;
		}
	}
	// set_baudrate(CONFIG_BAUDRATE);

	return ret;
}

static int do_cvi_update(struct cmd_tbl *cmdtp, int flag, int argc,
			 char *const argv[])
{

#if (!defined CONFIG_TARGET_CVITEK_CV181X_FPGA) && (!defined CONFIG_TARGET_CVITEK_ATHENA2_FPGA) && \
	(!defined ATHENA2_FPGA_PALLDIUM_ENV)
	int ret = 1;
	uint32_t usb_pid = 0;

	if (argc == 1) {
		update_magic = readl((unsigned int *)BOOT_SOURCE_FLAG_ADDR);
		if (update_magic == UART_UPDATE_MAGIC) {
			run_command("env default -a", 0);
			ret = _uart_update();
		} else if (update_magic == SD_UPDATE_MAGIC) {
			run_command("env default -a", 0);
			ret = _storage_update(sd_dl);
		} else if (update_magic == USB_UPDATE_MAGIC) {
			run_command("env default -a", 0);
			usb_pid = in_be32(UBOOT_PID_SRAM_ADDR);
			usb_pid = bcd2hex4(usb_pid);
			ret = _usb_update(usb_pid);
		}
	} else {
		printf("Usage:\n%s\n", cmdtp->usage);
	}

	return ret;
#else
	return 0;
#endif
}

U_BOOT_CMD(
	cvi_update, 2, 0, do_cvi_update,
	"cvi_update [eth, sd, usb]- check boot status and update if necessary\n",
	"run cvi_update without parameter will check the boot status and try to update");
