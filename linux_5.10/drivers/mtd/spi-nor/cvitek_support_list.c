// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2005, Intec Automation Inc.
 * Copyright (C) 2014, Freescale Semiconductor, Inc.
 */

#include <linux/mtd/spi-nor.h>

#include "core.h"

static void sr_bit1_qe_default_init(struct spi_nor *nor)
{
	nor->params->quad_enable = spi_nor_sr_bit1_quad_enable;
	nor->flags &= ~SNOR_F_HAS_16BIT_SR;
}

static struct spi_nor_fixups sr_bit1_qe_fixups = {
	.default_init = sr_bit1_qe_default_init,
};

static void sr1_bit6_qe_default_init(struct spi_nor *nor)
{
	nor->params->quad_enable = spi_nor_sr1_bit6_quad_enable;
	nor->flags &= ~SNOR_F_HAS_16BIT_SR;
}

static struct spi_nor_fixups sr1_bit6_qe_fixups = {
	.default_init = sr1_bit6_qe_default_init,
};

static void sr2_bit1_qe_default_init(struct spi_nor *nor)
{
	nor->params->quad_enable = spi_nor_sr2_bit1_quad_enable;
	nor->flags &= ~SNOR_F_HAS_16BIT_SR;
}

static struct spi_nor_fixups sr2_bit1_qe_fixups = {
	.default_init = sr2_bit1_qe_default_init,
};

static void no_qe_default_init(struct spi_nor *nor)
{
	nor->params->quad_enable = NULL;
	nor->flags &= ~SNOR_F_HAS_16BIT_SR;
}

static struct spi_nor_fixups no_qe_fixups = {
	.default_init = no_qe_default_init,
};

static const struct flash_info cvitek_parts[] = {

	{ "XM25QH64A", INFO(0x207017, 0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)
		.fixups = &no_qe_fixups
	},
	{ "XM25QH128A", INFO(0x207018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)
		.fixups = &no_qe_fixups
	},
	{ "XM25Qx128A", INFO(0x207118, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)
		.fixups = &no_qe_fixups
	},
	{ "JY25VQ128A", INFO(0x1c4018, 0x0, 64 * 1024, 256,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP | SECT_4K)
		.fixups = &sr_bit1_qe_fixups },
	{ "JY25VQ64A", INFO(0x4a4017, 0x0, 64 * 1024, 128,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP | SECT_4K)
		.fixups = &sr_bit1_qe_fixups },
	{ "XM25QH64C", INFO(0x204017, 0x0, 64 * 1024, 128,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP | SECT_4K)
		.fixups = &sr_bit1_qe_fixups },
	{ "XM25QH128C", INFO(0x204018, 0x0, 64 * 1024, 256,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP | SECT_4K)
		.fixups = &sr_bit1_qe_fixups },
	{ "XM25QH256C", INFO(0x204019, 0x0, 64 * 1024, 512,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP | SECT_4K |
			SPI_NOR_4B_OPCODES)
		.fixups = &sr_bit1_qe_fixups },
	{ "XM25QH256B", INFO(0x206019, 0x0, 64 * 1024, 512,
			SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP | SECT_4K |
			SPI_NOR_4B_OPCODES)
		.fixups = &sr1_bit6_qe_fixups },
	{ "MT25QL256A",  INFO6(0x20ba19, 0x104400, 64 * 1024,  512,
			SECT_4K | USE_FSR | SPI_NOR_DUAL_READ |
			SPI_NOR_QUAD_OP | SPI_NOR_4B_OPCODES | SPI_NOR_HAS_FIX_DUMMY)
		.fixups = &no_qe_fixups},
	{ "W25Q128JV-IQ", INFO(0xef4018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "W25Q256JV-IQ", INFO(0xef4019, 0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP |
			SPI_NOR_4B_OPCODES)
		.fixups = &sr_bit1_qe_fixups },
	{ "W25Q64JV-IQ", INFO(0xef4017, 0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	/* Juyang 32M Nor Flash(JY25VQ256A) uses the same wafers as MXIC */
	{ "MX25L25645G", INFO(0xc22019, 0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP |
			SPI_NOR_4B_OPCODES)
		.fixups = &sr1_bit6_qe_fixups },
	{ "MX25L12835F", INFO(0xc22018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr1_bit6_qe_fixups },
	{ "EN25QH128A", INFO(0x1c7018, 0, 64 * 1024,  256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP) },
	{ "EN25QX64A", INFO(0x1c7117, 0, 64 * 1024,  128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP) },
	{ "EN25QX128A", INFO(0x1c7118, 0, 64 * 1024,  256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP) },
	{ "EN25QH256A",  INFO(0x1c7019, 0, 64 * 1024,  512,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP |
			SPI_NOR_4B_OPCODES) },
	{ "GD25Q128E", INFO(0xc84018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "GD25Q64E", INFO(0xc84017, 0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "GD25Q256E", INFO(0xc84019, 0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP |
			SPI_NOR_4B_OPCODES)},
	{ "IS25LP064D", INFO(0x9d6017, 0x0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr1_bit6_qe_fixups },
	{ "IS25LP128F", INFO(0x9d6018, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr1_bit6_qe_fixups },
	{ "IS25LP256E", INFO(0x9d6019, 0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP |
			SPI_NOR_4B_OPCODES)
		.fixups = &sr1_bit6_qe_fixups },
	{ "KH25L6433FM2I", INFO(0xc22017, 0x0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr1_bit6_qe_fixups },
	//{ "KH25L12833FM2I", INFO(0xc22018, 0x0, 64 * 1024, 256,
	//      SECT_4K | SPI_NOR_DUAL_READ) },
	{ "ZB25VQ64B", INFO(0x5e4017, 0x0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "ZB25VQ128A", INFO(0x5e4018, 0x0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "S25FL128L", INFO(0x016018, 0x0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP |
			SPI_NOR_HAS_FIX_DUMMY)
		.fixups = &sr2_bit1_qe_fixups
	},
	{ "S25FL256L", INFO(0x016019, 0x0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP |
			SPI_NOR_4B_OPCODES | SPI_NOR_HAS_FIX_DUMMY)
		.fixups = &sr2_bit1_qe_fixups },
	{ "XT25F64F", INFO(0x0b4017, 0x0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "XT25F128F", INFO(0x0b4018, 0x0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "FM25Q64", INFO(0xF83217, 0x0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "FM25Q128A", INFO(0xA14018, 0x0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "FM25W128A", INFO(0xA12818, 0x0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "BY25Q64ES", INFO(0x684017, 0x0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "BY25Q128AS", INFO(0x684018, 0x0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "BY25Q256FS", INFO(0x684919, 0x0, 64 * 1024, 512,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP |
			SPI_NOR_4B_OPCODES)
		.fixups = &sr_bit1_qe_fixups },
	{ "PY25Q128HA", INFO(0x852018, 0x0, 64 * 1024, 256,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "PY25Q64HA", INFO(0x852017, 0x0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{ "P25Q64SH", INFO(0x856017, 0x0, 64 * 1024, 128,
			SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_OP)
		.fixups = &sr_bit1_qe_fixups },
	{}


};

const struct spi_nor_manufacturer spi_nor_cvitek = {
	.name = "cvitek_support_list",
	.parts = cvitek_parts,
	.nparts = ARRAY_SIZE(cvitek_parts),
};
