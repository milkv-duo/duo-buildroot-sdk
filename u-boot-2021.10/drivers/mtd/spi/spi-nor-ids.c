// SPDX-License-Identifier: GPL-2.0+
/*
 *
 * Copyright (C) 2013 Jagannadha Sutradharudu Teki, Xilinx Inc.
 * Copyright (C) 2016 Jagan Teki <jagan@openedev.com>
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 */

#include <common.h>
#include <spi.h>
#include <spi_flash.h>
#include "sf_internal.h"
#include <command.h>

/* Exclude chip names for SPL to save space */
#if !CONFIG_IS_ENABLED(SPI_FLASH_TINY)
#define INFO_NAME(_name) .name = _name,
#else
#define INFO_NAME(_name)
#endif

/* Used when the "_ext_id" is two bytes at most */
#define INFO(_name, _jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
		INFO_NAME(_name)					\
		.id = {							\
			((_jedec_id) >> 16) & 0xff,			\
			((_jedec_id) >> 8) & 0xff,			\
			(_jedec_id) & 0xff,				\
			((_ext_id) >> 8) & 0xff,			\
			(_ext_id) & 0xff,				\
			},						\
		.id_len = (!(_jedec_id) ? 0 : (3 + ((_ext_id) ? 2 : 0))),	\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags),

#define INFO6(_name, _jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
		INFO_NAME(_name)					\
		.id = {							\
			((_jedec_id) >> 16) & 0xff,			\
			((_jedec_id) >> 8) & 0xff,			\
			(_jedec_id) & 0xff,				\
			((_ext_id) >> 16) & 0xff,			\
			((_ext_id) >> 8) & 0xff,			\
			(_ext_id) & 0xff,				\
			},						\
		.id_len = 6,						\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags),

/* NOTE: double check command sets and memory organization when you add
 * more nor chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 *
 * All newly added entries should describe *hardware* and should use SECT_4K
 * (or SECT_4K_PMC) if hardware supports erasing 4 KiB sectors. For usage
 * scenarios excluding small sectors there is config option that can be
 * disabled: CONFIG_SPI_FLASH_USE_4K_SECTORS.
 * For historical (and compatibility) reasons (before we got above config) some
 * old entries may be missing 4K flag.
 */
const struct flash_info spi_nor_ids[] = {
	/* The CVITEK Support List Of Spi Nor Flash */
	{ INFO("JY25VQ128A", 0x1c4018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("JY25VQ64A", 0x4a4017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	/* Juyang 32M Nor Flash(JY25VQ256A) uses the same wafers as MXIC */
	{ INFO("MX25L25645G", 0xc22019, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SECT_4K | SPI_NOR_4B_OPCODES) },
	{ INFO("MX25L12835F", 0xc22018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("EN25QH128A", 0x1c7018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("EN25QX64A", 0x1c7117, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("EN25QX128A", 0x1c7118, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("EN25QH256A", 0x1c7019, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES) },
	{ INFO("W25Q256JV-IQ", 0xef4019, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SECT_4K | SPI_NOR_4B_OPCODES) },
	{ INFO("W25Q128JV-IQ", 0xef4018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("W25Q64JV-IQ", 0xef4017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("GD25Q128E", 0xc84018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("GD25Q256E", 0xc84019, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SECT_4K | SPI_NOR_4B_OPCODES) },
	{ INFO("GD25Q64E", 0xc84017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("GD25LQ256D", 0xc86019, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("XM25QH64C", 0x204017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("XM25QH128C", 0x204018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("XM25QH256C", 0x204019, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SECT_4K | SPI_NOR_4B_OPCODES) },
	{ INFO("XM25QH256B", 0x206019, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SECT_4K | SPI_NOR_4B_OPCODES) },
	// N25Q256 N25L256
	{ INFO("N25Q256", 0x20ba19, 0x0,  64 * 1024,   512,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("IS25LP064D", 0x9D6017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("IS25LP128F", 0x9d6018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("IS25LP256E", 0x9d6019, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SECT_4K | SPI_NOR_4B_OPCODES) },
	{ INFO("KH25L6433FM2I", 0xc22017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	//{ INFO("KH25L12833FM2I", 0xc22018, 0x0, 64 * 1024, 256,
	//      SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("ZB25VQ64B", 0x5E4017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("ZB25VQ128A", 0x5E4018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("S25FL128L", 0x016018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("S25FL256L", 0x016019, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SECT_4K | SPI_NOR_4B_OPCODES) },
	{ INFO("XT25F64F", 0x0B4017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("XT25F128F", 0x0B4018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("FM25Q64", 0xF83217, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("FM25Q128A", 0xA14018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("FM25W128", 0xA12818, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("BY25Q64ES", 0x684017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("BY25Q128AS", 0x684018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("BY25Q256FS", 0x684919, 0x0, 64 * 1024, 512,
		SPI_NOR_QUAD_READ | SECT_4K | SPI_NOR_4B_OPCODES) },
	{ INFO("PY25Q128HA", 0x852018, 0x0, 64 * 1024, 256,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("PY25Q64HA", 0x852017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ INFO("P25Q64SH", 0x856017, 0x0, 64 * 1024, 128,
		SPI_NOR_QUAD_READ | SECT_4K) },
	{ },
};

static int do_spinor_show_support_list(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	char buf[128] = {0};
	const struct flash_info *info = spi_nor_ids;
	uint32_t id, size;

	printf("\t###################### nor flash list #########################\n\n");
	sprintf(buf, "\t%-16s		%-12s		%-6s\n", "NAME", "JEDEC_ID", "SIZE");
	printf("%s\n", buf);
	for (; info->name; info++) {
		id = 0;
		size = 0;
		id = info->id[0] << 16 | info->id[1] << 8 | info->id[2];
		size = (info->sector_size * info->n_sectors) >> 20;
		sprintf(buf, "\t%-16s		0x%-6x		%-2u M\n", info->name, id, size);
		printf("%s\n", buf);
	}
	printf("\t###############################################################\n\n");
	return 0;
}

U_BOOT_CMD(norshow, 2, 0, do_spinor_show_support_list, "norshow", "\nshow nor flash list\n");
