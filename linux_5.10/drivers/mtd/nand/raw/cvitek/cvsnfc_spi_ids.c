#include "linux/types.h"
#include <linux/mtd/rawnand.h>
#include <linux/io.h>
#include <linux/delay.h>
#include "cvsnfc_spi_ids.h"
#include "cvsnfc.h"

#define DEBUG_SPI_IDS 0

struct nand_flash_dev *(*match_spi_nand_by_id)(struct mtd_info *mtd, unsigned char *id) = NULL;

static int spi_nand_gd_qe_enable(struct cvsnfc_host *host);

/* Read status[C0H]:[0]bit OIP, judge whether the device is busy or not */
static int spi_general_wait_ready(struct cvsnfc_host *host);

/* Send write enable command to SPI Nand, status[C0H]:[2]bit WEL must be set 1 */
static int spi_general_write_enable(struct cvsnfc_host *host);

static int spi_nand_winbond_select_die(struct cvsnfc_host *host, unsigned int id);

static int spi_nand_tsb_set_ecc_detect_bits(struct cvsnfc_host *host, unsigned int bits);

static struct spi_nand_driver  spi_nand_driver_general = {
	.wait_ready   = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
};

static struct spi_nand_driver  spi_nand_driver_gd = {
	.wait_ready   = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_nand_gd_qe_enable,
};

static struct spi_nand_driver  spi_nand_driver_winbond_multi = {
	.wait_ready   = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.select_die = spi_nand_winbond_select_die,
};

static struct spi_nand_driver  spi_nand_driver_toshiba = {
	.wait_ready   = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.set_ecc_detect_bits = spi_nand_tsb_set_ecc_detect_bits,
};

/*****************************************************************************/
#define SPI_NAND_ID_TAB_VER		"1.4"

/*
 *	ECCS2	ECCS1	ECCS0	Description
 *	0	0	0	No errors
 *	0	0	1	1-3 bit error detected and corrected
 *	0	1	0	9-bits or more than 9-bits errors detected and not corrected
 *	0	1	1	4-6 bit error detected and corrected
 *	1	0	1	7-8 bit error detected and corrected
 *	1	0	0	Reserved
 *	1	1	0	Reserved
 *	1	1	1	Reserved
 */

/* only for 8 bit threshold */
short ECC_3bits_remap[8] = {0, 1, -1, 4, 0xff, 7, 0xff, 0xff };

/*
 *	ECCS1	ECCS0	Description
 *	0	0	No bit errors were detected in last page read
 *	0	1	Bit errors were detected and corrected (<=7 bits)
 *	1	0	Bit errors greater than ECC capability (8bits) and not corrected
 *	1	1	Bit errors were detected and corrected,Bit errors count was equal
 *				to the threshold bit count (8 bits)
 */
short ECC_XT26G11C[4] = {0, 1, -1, 8};

/*
 *	ECCS1	ECCS0	Description
 *	0	0	0 bit error
 *	0	1	1 ~ 4 bits error and been corrected.
 *	1	0	More than 4-bit error and not corrected.
 *	1	1	Reserved
 */
short ECC_2bits_remap[4] = {0, 1, -1, 0xff};
short ECC_1bits_remap[4] = {0, 1, -1, -1};

/*
 *	ECCS1	ECCS0	ECCSE1	ECCSE0	Description
 *	0	0	x	x	No bit errors were detected during the previous read algorithm
 *	0	1	0	0	Bit errors(=1) were detected and corrected
 *	0	1	0	1	Bit errors (=2) were detected and corrected
 *	0	1	1	0	Bit errors (=3) were detected and corrected
 *	0	1	1	1	Bit errors (=4) were detected and corrected
 *	1	1	x	x	Reserved
 *	1	0	x	x	Bit errors greater than ECC capability(4 bits) and not corrected
 *	ECCS0-ECCS1 is located in field 4-5 of addr of 0xC0
 *	ECCSE0-ECCSE1 is located in field 4-5 of addr of 0xF0
 */
short ECC_GD_4bit_remap[16] = {0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff};
short ECC_GD_8bit_remap[16] = {0, 0, 0, 0, 4, 5, 6, 7, 0, 0, 0, 0, 8, 8, 8, 8};
short ECC_HYF2G_remap[4] = {0, 1, -1, 14};

struct cvsnfc_chip_info nand_flash_cvitek_supported_ids[] = {
	{
		{	.name = "GSS01GSAK1",
			.id = {0x52, 0xba, 0x13},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = 64,
			{	.strength_ds = 4,
				.step_ds = 512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_2bits_remap,
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "GSS02GSAK1",
			.id = {0x52, 0xba, 0x23},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = 128,
			{	.strength_ds = 4,
				.step_ds = 512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_2bits_remap,
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "F50L1G41LB",
			.id = {0xC8, 0x01},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = 64,
			{	.strength_ds = 1,
				.step_ds = 512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "F50L512M41A",
			.id = {0xC8, 0x20},
			.pagesize = SZ_2K,
			.chipsize = SZ_64,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 1,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "F50L1G41A",
			.id = {0xC8, 0x21},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 1,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "F50L2G41KA",
			.id = {0xC8, 0x41},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 3,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_3bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "F50L2G41XA",
			.id = {0x2c, 0x24},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 3,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_3bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = FLAGS_SET_PLANE_BIT
	},

	{
		{	.name = "F50L4G41XB",
			.id = {0x2c, 0x34},
			.pagesize = SZ_4K,
			.chipsize = SZ_512,
			.erasesize = SZ_256K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_256,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 3,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_3bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "XT26G11C",
			.id = {0x0b, 0x15},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_XT26G11C
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F1GQ4UAYIG",
			.id = {0xc8, 0xf1},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_4bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F1GQ4UBYIG",
			.id = {0xc8, 0xd1},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{ .strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_4bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F2GQ4UAYIG",
			.id = {0xc8, 0xf2},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_8bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F2GQ4UBYIG",
			.id = {0xc8, 0xd2},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_4bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F4GQ4UAYIG",
			.id = {0xc8, 0xf4},
			.pagesize = SZ_2K,
			.chipsize = SZ_512,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_8bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F4GQ4UBYIG",
			.id = {0xc8, 0xd4},
			.pagesize = SZ_4K,
			.chipsize = SZ_512,
			.erasesize = SZ_256K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_256,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_8bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F1GQ5UExxH",
			.id = {0xc8, 0x31},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_4bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F1GQ5UExxG",
			.id = {0xc8, 0x51},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_4bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F1GQ5UExxH",
			.id = {0xc8, 0x31},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_4bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F2GQ5UExxH",
			.id = {0xc8, 0x52},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_4bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "GD5F4GQ6UEYIG",
			.id = {0xC8, 0x55},
			.pagesize = SZ_2K,
			.chipsize = SZ_512,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_8bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	/* GD GD5F1GM7 1Gbit */
	{
		{	.name = "GD5F1GM7UEYIG",
			.id = {0xC8, 0x91},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_8bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},
	/* GD GD5F2GM7 2Gbit */
	{
		{	.name = "GD5F2GM7UEYIG",
			.id = {0xC8, 0x92},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0xf0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_GD_8bit_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "MT29F1G01",
			.id = {0x2c, 0x12},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_2bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "MT29F2G01",
			.id = {0x2c, 0x22},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_2bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "MT29F4G01",
			.id = {0x2c, 0x32},
			.pagesize = SZ_2K,
			.chipsize = SZ_512,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_2bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "MT29F1G01ABAFDWB",
			.id = {0x2c, 0x14},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 3,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_3bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "MX35LF1GE4AB",
			.id = {0xc2, 0x12},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0x7c,
			.ecc_bits = 4,
			.ecc_bit_shift = 0,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "MX35LF2GE4AB",
			.id = {0xc2, 0x22},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0x7c,
			.ecc_bits = 4,
			.ecc_bit_shift = 0,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "MX35LF2GE4AD",
			.id = {0xc2, 0x26, 0x03},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_64,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0x7c,
			.ecc_bits = 4,
			.ecc_bit_shift = 0,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "MX35LF4GE4AD",
			.id = {0xc2, 0x37, 0x03},
			.pagesize = SZ_4K,
			.chipsize = SZ_512,
			.erasesize = SZ_256K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0x7c,
			.ecc_bits = 4,
			.ecc_bit_shift = 0,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "TC58CVG2S0HRAIJ",
			.id = {0x98, 0xed, 0x51},
			.pagesize = SZ_4K,
			.chipsize = SZ_512,
			.erasesize = SZ_256K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x30,
			.read_ecc_opcode = 0,
			.ecc_bits = 4,
			.ecc_bit_shift = 4,
			.uncorr_val = 2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_toshiba,
		.flags = 0
	},

	{
		{	.name = "TC58CVG1S0HRAIJ",
			.id = {0x98, 0xeb, 0x40},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x30,
			.read_ecc_opcode = 0,
			.ecc_bits = 4,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_toshiba,
		.flags = 0
	},

	{
		{	.name = "TC58CVG1S3HxAI",
			.id = {0x98, 0xcb},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x30,
			.read_ecc_opcode = 0,
			.ecc_bits = 4,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_toshiba,
		.flags = 0
	},

	{
		{	.name = "TC58CVG0S3HRAIJ",
			.id = {0x98, 0xe2},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x30,
			.read_ecc_opcode = 0,
			.ecc_bits = 4,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_toshiba,
		.flags = 0
	},

	{
		{	.name = "W25N01GV",
			.id = {0xef, 0xaa, 0x21},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_2bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "W25M02GV",
			.id = {0xef, 0xab, 0x21},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_2bits_remap
		},
		.driver = &spi_nand_driver_winbond_multi,
		.flags = FLAGS_NAND_HAS_TWO_DIE
	},

	/* Winbond W25N01KVxxIR 1Gbit */
	{
		{	.name = "W25N01KV",
			.id = {0xef, 0xae, 0x21},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x30,
			.read_ecc_opcode = 0,
			.ecc_bits = 3,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_toshiba,
		.flags = 0
	},

	{
		{	.name = "W25N02KV",
			.id = {0xef, 0xaa, 0x22},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_64,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x30,
			.read_ecc_opcode = 0,
			.ecc_bits = 4,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_toshiba,
		.flags = 0
	},

	{
		{	.name = "W25N04KV",
			.id = {0xef, 0xaa, 0x23},
			.pagesize = SZ_2K,
			.chipsize = SZ_512,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x30,
			.read_ecc_opcode = 0,
			.ecc_bits = 4,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_toshiba,
		.flags = 0
	},

	{
		{	.name = "DS35Q1GA-IB",
			.id = {0xe5, 0x71},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 4,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_2bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	/* DOSILICON DS35X1GBXXX 1Gbit */
	{
		{	.name = "DS35Q1GB-IB",
			.id = {0xe5, 0xf1},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0,
			.read_ecc_opcode = 0,
			.ecc_bits = 3,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_3bits_remap
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "F35SQA512M",
			.id = {0xcd, 0x70, 0x70},
			.pagesize = SZ_2K,
			.chipsize = SZ_64,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_64,
			{	.strength_ds = 1,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "F35SQA001G",
			.id = {0xcd, 0x71, 0x71},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 3,
			.oobsize = SZ_64,
			{	.strength_ds = 1,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "F35SQA002G",
			.id = {0xCD, 0x72, 0x72},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 1,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = NULL
		},
		.driver = &spi_nand_driver_general,
		.flags = 0
	},

	{
		{	.name = "HYF2GQ4UAACAE",
			.id = {0xC9, 0x52},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 14,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_HYF2G_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "FM25S01A",
			.id = {0xA1, 0xE4},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 1,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_1bits_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "FM25S02A",
			.id = {0xA1, 0xE5},
			.pagesize = SZ_2K,
			.chipsize = SZ_256,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_64,
			{	.strength_ds = 1,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x0,
			.read_ecc_opcode = 0,
			.ecc_bits = 2,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_1bits_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{
		{	.name = "FM25S01B",
			.id = {0xA1, 0xD4},
			.pagesize = SZ_2K,
			.chipsize = SZ_128,
			.erasesize = SZ_128K,
			.options = 0,
			.id_len = 2,
			.oobsize = SZ_128,
			{	.strength_ds = 8,
				.step_ds = SZ_512
			},
		},

		{	.ecc_sr_addr = 0xc0,
			.ecc_mbf_addr = 0x0,
			.read_ecc_opcode = 0,
			.ecc_bits = 3,
			.ecc_bit_shift = 4,
			.uncorr_val = 0x2,
			.remap = ECC_3bits_remap
		},
		.driver = &spi_nand_driver_gd,
		.flags = 0
	},

	{ NULL }
};

/* addr: 0xb0
 * qe bit shhift: 0
 *
 */
static int spi_nand_gd_qe_enable(struct cvsnfc_host *host)
{
	unsigned int regval = 0;

	spi_feature_op(host, GET_OP, FEATURE_ADDR, &regval);

	regval |= FEATURE_QE_ENABLE;
	spi_feature_op(host, SET_OP, FEATURE_ADDR, &regval);

	/* check qe bit */
	spi_feature_op(host, GET_OP, FEATURE_ADDR, &regval);
	if (!(regval & FEATURE_ADDR))
		return -1;

	return 0;
}

/* Read status[C0H]:[0]bit OIP, judge whether the device is busy or not */
static int spi_general_wait_ready(struct cvsnfc_host *host)
{
	unsigned int regval = 0;
	unsigned int deadline = 0;

	if (DEBUG_SPI_IDS)
		pr_info("%s\n", __func__);

	do {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &regval);
		if (!(regval & STATUS_OIP_MASK)) {
			if ((host->cmd_option.last_cmd == NAND_CMD_ERASE2)
					&& (regval & STATUS_E_FAIL_MASK))
				return regval;
			if ((host->cmd_option.last_cmd == NAND_CMD_PAGEPROG)
					&& (regval & STATUS_P_FAIL_MASK))
				return regval;
			return 0;
		}
		udelay(1);
	} while (deadline++ < (40 << 20));

	if (DEBUG_SPI_IDS)
		pr_info("cvsnfc: wait ready timeout.\n");

	return 1;
}

/* Send write enable command to SPI Nand, status[C0H]:[2]bit WEL must be set 1 */
static int spi_general_write_enable(struct cvsnfc_host *host)
{
	unsigned int val = FEATURE_PROTECTION_NONE;

	if (DEBUG_SPI_IDS)
		pr_info("%s\n", __func__);

	spi_feature_op(host, SET_OP, SPI_NAND_FEATURE_PROTECTION, &val);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 0x0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0x0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, SPI_NAND_CMD_WREN);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0,
			cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) |
			BIT_REG_TRX_START);

	cvsnfc_send_nondata_cmd_and_wait(host);

	spi_feature_op(host, GET_OP, WRITE_ENABLE_STATUS_ADDR, &val);

	if (!(val & STATUS_WEL_MASK)) {
		pr_info("cvsnfc: write enable failed! val[%#x]\n", val);
		return 1;
	}

	return 0;
}

static int spi_nand_winbond_select_die(struct cvsnfc_host *host, unsigned int id)
{
	static uint8_t pre_id = 0xff;

	if (id == pre_id) {
		return 0;
	}

	// Select Die
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 0x1);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0x0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0,
			(id << TRX_CMD_CONT0_SHIFT) | 0xC2);
	// Wait for completion
	cvsnfc_send_nondata_cmd_and_wait(host);
	pre_id = id;
	return 0;
}

static int spi_nand_tsb_set_ecc_detect_bits(struct cvsnfc_host *host, unsigned int bits)
{
	unsigned int val = 0;
	unsigned int val_readback = 0;

	pr_info("%s set detect bits 0x%x\n", __func__, bits);

	spi_feature_op(host, GET_OP, 0x10, &val);
	pr_info("orginal BFD %u\n", val);

	val = (bits & 0xF) << 4;
	spi_feature_op(host, SET_OP, 0x10, &val);

	spi_feature_op(host, GET_OP, 0x10, &val_readback);
	pr_info("new BFD %u\n", val_readback);

	if (val != val_readback) {
		pr_err("unable to set BFD val %u, val_readback %u\n", val, val_readback);
		return -1;
	}

	return 0;
}

void cvsnfc_enable_ecc(struct cvsnfc_host *host, struct cvsnfc_chip_info *spi_nand, int op)
{
	uint32_t regval;
	struct spi_nand_driver *spi = spi_nand->driver;

	spi_feature_op(host, GET_OP, FEATURE_ADDR, &regval);
	if (op)
		regval &= FEATURE_ECC_ENABLE;
	else
		regval &= ~FEATURE_ECC_ENABLE;

	spi_feature_op(host, SET_OP, FEATURE_ADDR, &regval);

	spi->wait_ready(host);

	spi_feature_op(host, GET_OP, FEATURE_ADDR, &regval);
	if (regval & FEATURE_ECC_ENABLE)
		pr_info("%s Internal ECC %s fail! val[%#x]\n",
				__func__, (op ? "enable" : "disable"), regval);

}

void cvsnfc_disable_block_protect(struct cvsnfc_host *host,
		struct cvsnfc_chip_info *spi_nand)
{
	uint32_t regval;
	struct spi_nand_driver *spi = spi_nand->driver;

	if (DEBUG_SPI_IDS)
		pr_info("%s\n", __func__);

	spi_feature_op(host, GET_OP, PROTECTION_ADDR, &regval);

	if (ANY_BP_ENABLE(regval)) {
		regval &= ~ALL_BP_MASK;
		spi_feature_op(host, SET_OP, PROTECTION_ADDR, &regval);

		spi->wait_ready(host);

		spi_feature_op(host, GET_OP, PROTECTION_ADDR, &regval);
		if (ANY_BP_ENABLE(regval))
			pr_info("%s write protection disable fail! val[%#x]\n",
					__func__, regval);
	}

}

/*****************************************************************************/
void cvsnfc_spi_nand_init(struct cvsnfc_host *host)
{
	int ecc_bits;

	struct cvsnfc_chip_info *spi_nand = &host->spi_nand;
	struct nand_flash_dev *info = &host->spi_nand.nand_info;
	struct spi_nand_driver *driver = host->spi_nand.driver;

	ecc_bits = info->ecc.strength_ds;
	cvsnfc_disable_block_protect(host, spi_nand);
#ifdef RW_DEBUG
	cvsnfc_enable_ecc(host, spi_nand, 0);
#endif

	if (driver->set_ecc_detect_bits) {
		if (driver->set_ecc_detect_bits(host, ecc_bits))
			pr_info("set ecc detect bit failed, ecc bits: %d\n", ecc_bits);
	}

}

extern uint8_t cvsnfc_read_byte(struct nand_chip *chip);
int cvsnfc_scan_nand(struct cvsnfc_host *host)
{
	int i;
	u8 *ids = host->nand.id.data;
	struct cvsnfc_chip_info *spi_nand = nand_flash_cvitek_supported_ids;
	uint32_t val;

	host->send_cmd_reset(host);
	udelay(500);
	host->send_cmd_readid(host);
	pr_info("SPI Nand ID Table Version %s\n", SPI_NAND_ID_TAB_VER);
	val = cvsfc_read(host, REG_SPI_NAND_RX_DATA);
	for (i = sizeof(val) - 1; i >= 0 ; i--) {
		ids[i] = (val >> i * 8) & 0xff;
		pr_debug("id%d:0x%x\n", i, ids[i]);
	}

	for (; spi_nand; spi_nand++) {
		if (!memcmp(ids, spi_nand->nand_info.id, spi_nand->nand_info.id_len)) {
			host->pagesize = spi_nand->nand_info.pagesize;
			host->oobsize = spi_nand->nand_info.oobsize;
			host->dma_oob = host->dma_buffer + host->pagesize;
			// The number of pages per block
			host->block_page_cnt = spi_nand->nand_info.erasesize / spi_nand->nand_info.pagesize;
			host->flags = spi_nand->flags;
			if (spi_nand->flags & FLAGS_NAND_HAS_TWO_DIE)
				/* chipsize / 2 * 1024 * 1024 */
				host->diesize = spi_nand->nand_info.chipsize << 19;
			else
				host->diesize = 0;

			memcpy(&host->spi_nand, spi_nand, sizeof(struct cvsnfc_chip_info));
			return nand_scan_with_ids(&host->nand, 1, &spi_nand->nand_info);
		}
	}
	pr_info("can not scan spi nand!\n");
	return -1;
}

