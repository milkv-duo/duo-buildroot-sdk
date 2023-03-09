// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2005, Intec Automation Inc.
 * Copyright (C) 2014, Freescale Semiconductor, Inc.
 */

#include <linux/mtd/spi-nor.h>

#include "core.h"

static const struct flash_info xmc_parts[] = {
	/* XMC (Wuhan Xinxin Semiconductor Manufacturing Corp.) */
	{ "XM25QH64C", INFO(0x204017, 0x0, 64 * 1024, 128,
				SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SECT_4K)},
	{ "XM25QH128C", INFO(0x204018, 0x0, 64 * 1024, 256,
				SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SECT_4K)},
	{ "XM25QH256C", INFO(0x204019, 0x0, 64 * 1024, 512,
				SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SECT_4K |
				SPI_NOR_4B_OPCODES)},
	{ "XM25QH64A", INFO(0x207017, 0, 64 * 1024, 128,
			    SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "XM25QH128A", INFO(0x207018, 0, 64 * 1024, 256,
			    SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "XM25Qx128A", INFO(0x207118, 0, 64 * 1024, 256,
				SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },


	{ "JY25VQ128A", INFO(0x1c4018, 0x0, 64 * 1024, 256,
				SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SECT_4K)},
	{ "JY25VQ64A", INFO(0x4a4017, 0x0, 64 * 1024, 128,
				SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SECT_4K)},

	{ "ZB25VQ64B", INFO(0x5e4017, 0x0, 64 * 1024, 128,
				SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
	{ "ZB25VQ128A", INFO(0x5e4018, 0x0, 64 * 1024, 256,
				SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},

	{ "XT25F64F", INFO(0x0b4017, 0x0, 64 * 1024, 128,
				SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
	{ "XT25F128F", INFO(0x0b4018, 0x0, 64 * 1024, 256,
				SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},

	{ "FM25Q64", INFO(0xF83217, 0x0, 64 * 1024, 128,
				SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
};

const struct spi_nor_manufacturer spi_nor_xmc = {
	.name = "xmc",
	.parts = xmc_parts,
	.nparts = ARRAY_SIZE(xmc_parts),
};
