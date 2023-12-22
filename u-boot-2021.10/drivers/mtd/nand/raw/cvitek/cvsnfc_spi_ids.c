#include <common.h>
#include <linux/mtd/nand.h>
#include "cvsnfc_spi_ids.h"
#include "cvsnfc_common.h"
#include "cvsnfc.h"
#include <asm/io.h>
#include <command.h>

#define DEBUG_SPI_IDS 0

/* ESMT spi nand don't support QE enable. */
static int spi_nand_no_qe_enable(struct cvsnfc_op *spi)
{
	if (DEBUG_SPI_IDS)
		printf("%s\n", __func__);

	return 0;
}

/* register addr:0xb0
 * QE bit pos:BIT(0)
 *
 */
static int spi_nand_qe_enable(struct cvsnfc_op *spi)
{
	unsigned int regval = 0;
	struct cvsnfc_host *host = (struct cvsnfc_host *)spi->host;

	spi_feature_op(host, GET_OP, FEATURE_ADDR, &regval);
	regval |= FEATURE_QE_ENABLE;
	spi_feature_op(host, SET_OP, FEATURE_ADDR, &regval);

	/* check it */
	spi_feature_op(host, GET_OP, FEATURE_ADDR, &regval);
	if (!(regval & FEATURE_QE_ENABLE))
		return -1;

	return 0;
}

/* status register addr is C0h. bit0 is OIP
 * it indicates whether the device is busy
 * bit1 is WEL, it indicates whether the device
 * is set to internal write enable latch.
 * block lock register addr is A0h
 */
static int spi_general_wait_ready(struct cvsnfc_op *spi)
{
	unsigned int regval = 0;
	unsigned int deadline = 0;
	struct cvsnfc_host *host = (struct cvsnfc_host *)spi->host;

	if (DEBUG_SPI_IDS)
		printf("%s\n", __func__);

	do {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &regval);
		if (!(regval & STATUS_OIP_MASK)) {
			if ((host->cmd_option.last_cmd == NAND_CMD_ERASE2) &&
			    (regval & STATUS_E_FAIL_MASK))
				return regval;
			if ((host->cmd_option.last_cmd == NAND_CMD_PAGEPROG) &&
			    (regval & STATUS_P_FAIL_MASK))
				return regval;
			return 0;
		}
		udelay(1);
	} while (deadline++ < (40 << 20));

	if (DEBUG_SPI_IDS)
		printf("cvsnfc: wait ready timeout.\n");

	return 1;
}

/* Send write enable command to SPI Nand, status[C0H]:[2]bit WEL must be set 1 */
static int spi_general_write_enable(struct cvsnfc_op *spi)
{
	unsigned int val = FEATURE_PROTECTION_NONE;
	struct cvsnfc_host *host = (struct cvsnfc_host *)spi->host;

	if (DEBUG_SPI_IDS)
		printf("%s\n", __func__);
	spi_feature_op(host, SET_OP, SPI_NAND_FEATURE_PROTECTION, &val);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 0x0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0x0);
	/* write enable */
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, SPI_NAND_CMD_WREN);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	CVSNFC_CMD_WAIT_CPU_FINISH(host);
	CVSNFC_CLEAR_INT(host);
	/* read and check WEL bit */
	spi_feature_op(host, GET_OP, WRITE_ENABLE_STATUS_ADDR, &val);
	if (!(val & STATUS_WEL_MASK)) {
		printf("cvsnfc: write enable failed! val[%#x]\n", val);
		return 1;
	}

	return 0;
}

static int spi_nand_esmt_ecc_enable(struct cvsnfc_op *spi)
{
	return 0;
}

static int spi_nand_winbond_select_die(struct cvsnfc_op *spi, unsigned int id)
{
	struct cvsnfc_host *host = (struct cvsnfc_host *)spi->host;
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
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0,
		    cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) |
			    BIT_REG_TRX_START);

	CVSNFC_CMD_WAIT_CPU_FINISH(host);
	CVSNFC_CLEAR_INT(host);

	pre_id = id;
	return 0;
}

static struct spi_nand_driver  spi_nand_driver_esmt = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_nand_no_qe_enable,
	.ecc_enable = spi_nand_esmt_ecc_enable,
};

static struct spi_nand_driver spi_nand_driver_winbond_multi = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_nand_no_qe_enable,
	.ecc_enable = spi_nand_esmt_ecc_enable,
	.select_die = spi_nand_winbond_select_die,
};

static struct spi_nand_driver  spi_nand_driver_toshiba = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_nand_no_qe_enable,
};

static struct spi_nand_driver spi_nand_driver_gd = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_nand_qe_enable,
};

static struct spi_nand_driver spi_nand_driver_mxic = {
	.wait_ready = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_nand_qe_enable,
};

/*****************************************************************************/
#define SPI_NAND_ID_TAB_VER		"1.3"

struct cvsnfc_chip_info cvsnfc_spi_nand_flash_table[] = {
	{
		.name      = "GSS01GSAK1",
		.id        = {0x52, 0xba, 0x13},
		.id_len    = 3,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_toshiba,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,

	},

	{
		.name      = "GSS02GSAK1",
		.id        = {0x52, 0xba, 0x23},
		.id_len    = 3,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_toshiba,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,

	},

	/* ESMT F50L1G41LB 1Gbit */
	{
		.name      = "F50L1G41LB",
		.id        = {0xc8, 0x01},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,

	},

	/* ESMT F50L512M41A 512Mbit */
	{
		.name      = "F50L512M41A",
		.id        = {0xc8, 0x20},
		.id_len    = 2,
		.chipsize  = _64M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* ESMT F50L1G41A 1Gbit */
	{
		.name      = "F50L1G41A",
		.id        = {0xc8, 0x21},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* ESMT F50L1G41B 1Gbit */
	{
		.name      = "F50L1G41B",
		.id        = {0xc8, 0x01},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* ESMT F50L2G41KA 2Gbit */
	{
		.name      = "F50L2G41KA",
		.id        = {0xc8, 0x41},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* ESMT F50L2G41XA(2B) 3.3v 2Gbit */
	{
		.name	   = "F50L2G41XA",
		.id	   = {0x2c, 0x24},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_SET_PLANE_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* ESMT F50L4G41XB 3.3v 2Gbit */
	{
		.name	   = "F50L4G41XB",
		.id	   = {0x2c, 0x34},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _256K,
		.pagesize  = _4K,
		.oobsize   = 256,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* ESMT F50D1G41 1Gbit */
	{
		.name	   = "F50D1G41",
		.id	   = {0xc8, 0x11},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_46MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD5F1GQ4UAYIG 1Gbit */
	{
		.name      = "GD5F1GQ4UAYIG",
		.id        = {0xc8, 0xf1},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD5F1GQ4UBYIG 1Gbit */
	{
		.name      = "GD5F1GQ4UBYIG",
		.id        = {0xc8, 0xd1},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD 5F2GQ4UAYIG 2Gbit */
	{
		.name      = "GD5F2GQ4UAYIG",
		.id        = {0xc8, 0xf2},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD 5F2GQ4UBYIG 2Gbit */
	{
		.name      = "GD5F2GQ4UBYIG",
		.id        = {0xc8, 0xd2},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD 5F4GQ4UAYIG 4Gbit */
	{
		.name      = "GD5F4GQ4UAYIG",
		.id        = {0xc8, 0xf4},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD 5F4GQ4UBYIG 4Gbit */
	{
		.name      = "GD5F4GQ4UBYIG",
		.id        = {0xc8, 0xd4},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _256K,
		.pagesize  = _4K,
		.oobsize   = 256,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD5F1GQ5UExxH 1Gbit */
	{
		.name	   = "GD5F1GQ5UExxH",
		.id	   = {0xc8, 0x31},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD5F1GQ5UExxG 1Gbit */
	{
		.name	   = "GD5F1GQ5UExxG",
		.id	   = {0xc8, 0x51},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD5F2GQ5UExxH 2Gbit */
	{
		.name	   = "GD5F2GQ5UExxH",
		.id	   = {0xc8, 0x32},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD GD5F2GQ5RExxH 2Gbit */
	{
		.name	   = "GD5F2GQ5RExxH",
		.id	   = {0xc8, 0x22},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD GD5F2GQ5UEYIH 2Gbit */
	{
		.name	   = "GD5F2GQ5UEYIH",
		.id	   = {0xc8, 0x52},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD GD5F4GQ6UEYIG 4Gbit */
	{
		.name	   = "GD5F4GQ6UEYIG",
		.id	   = {0xc8, 0x55},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD GD5F1GM7 1Gbit */
	{
		.name	   = "GD5F1GM7UEYIG",
		.id	   = {0xc8, 0x91},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* GD GD5F2GM7 2Gbit */
	{
		.name	   = "GD5F2GM7UEYIG",
		.id	   = {0xc8, 0x92},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* Micron MT29F1G01 */
	{
		.name      = "MT29F1G01",
		.id        = {0x2c, 0x12},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_46MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* Micron MT29F2G01 */
	{
		.name      = "MT29F2G01",
		.id        = {0x2c, 0x22},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_46MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* Micron MT29F4G01 */
	{
		.name      = "MT29F4G01",
		.id        = {0x2c, 0x32},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_46MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* Micron MT29F1G01ABAFDWB */
	{
		.name      = "MT29F1G01ABAFDWB",
		.id        = {0x2c, 0x14},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos    = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* MXIC MX35LF1GE4AB, 1Gb, 3V */
	{
		.name	   = "MX35LF1GE4AB",
		.id	   = {0xc2, 0x12},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_mxic,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* MXIC MX35LF2GE4AB 2Gbit */
	{
		.name      = "MX35LF2GE4AB",
		.id        = {0xc2, 0x22},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_mxic,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/*  MXIC MX35LF2GE4AD 2Gbit */
	{
		.name      = "MX35LF2GE4AD",
		.id        = {0xc2, 0x26, 0x03},
		.id_len    = 3,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_mxic,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/*  MXIC MX35LF4GE4AD 4Gbit */
	{
		.name      = "MX35LF4GE4AD",
		.id        = {0xc2, 0x37, 0x03},
		.id_len    = 3,
		.chipsize  = _512M,
		.erasesize = _256K,
		.pagesize  = _4K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_mxic,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* TOSHIBA TC58CVG2S0H 4Gbit */
	{
		.name      = "TC58CVG2S0HRAIJ",
		.id        = {0x98, 0xed, 0x51},
		.id_len    = 3,
		.chipsize  = _512M,
		.erasesize = _256K,
		.pagesize  = _4K,
		.oobsize   = 256,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_toshiba,
		.flags = FLAGS_SUPPORT_W_TWICE_DEBUG | FLAGS_SET_QE_BIT
				| FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* TOSHIBA TC58CVG1S3HRAIJ 2Gbit */
	{

		.name      = "TC58CVG1S3HRAIJ",
		.id        = {0x98, 0xeb, 0x40},
		.id_len    = 3,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_toshiba,
		.flags = FLAGS_SUPPORT_W_TWICE_DEBUG | FLAGS_SET_QE_BIT
				| FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* TOSHIBA TC58CVG0S3HRAIJ 2Gbit */
	{

		.name      = "TC58CVG0S3HRAIJ",
		.id        = {0x98, 0xc2},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_toshiba,
		.flags = FLAGS_SUPPORT_W_TWICE_DEBUG | FLAGS_SET_QE_BIT
				| FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* TOSHIBA TC58CVG0S3HRAIJ 2Gbit */
	{
		.name      = "TC58CVG0S3HRAIJ",
		.id        = {0x98, 0xe2},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_toshiba,
		.flags = FLAGS_SUPPORT_W_TWICE_DEBUG | FLAGS_SET_QE_BIT
				| FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* Winbond W25N01GVxxIG 1Gbit */
	{
		.name      = "W25N01GVxxIG",
		.id        = {0xef, 0xaa, 0x21},
		.id_len    = 3,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* Winbond W25M02GV 2Gbit */
	{
		.name      = "W25M02GV",
		.id        = {0xef, 0xab, 0x21},
		.id_len    = 3,
		.chipsize  = _256M,
		.diesize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_winbond_multi,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* Winbond W25N04KV 4Gbit */
	{
		.name      = "W25N04KV",
		.id        = {0xef, 0xaa, 0x23},
		.id_len    = 3,
		.chipsize  = _512M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* Winbond W25N01KVxxIR 1Gbit */
	{
		.name      = "W25N01KV",
		.id        = {0xef, 0xae, 0x21},
		.id_len    = 3,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* Winbond W25N02KV 2Gbit */
	{
		.name      = "W25N02KV",
		.id        = {0xef, 0xaa, 0x22},
		.id_len    = 3,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_esmt,
		.flags = FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* XTX XT26G11C 1Gbit */
	{
		.name      = "XT26G11C",
		.id        = {0x0b, 0x15},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* TOSHIBA TC58CVG1S3HxAIx 2Gbit */
	{

		.name      = "TC58CVG1S3HxAIx",
		.id        = {0x98, 0xcb},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_toshiba,
		.flags = FLAGS_SUPPORT_W_TWICE_DEBUG
				| FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* DOSILICON DS35Q1GA-IB 1Gbit */
	{
		.name      = "DS35Q1GA-IB",
		.id        = {0xe5, 0x71},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* DOSILICON DS35X1GBXXX 1Gbit */
	{
		.name      = "DS35Q1GB-IB",
		.id        = {0xe5, 0xf1},		// 3.3V
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x70, /* bit 4 & 5 & 6*/
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 0,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* FORESEE F35SQA001G 1Gbit */
	{
		.name      = "F35SQA001G",
		.id        = {0xcd, 0x71, 0x71},
		.id_len    = 3,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	/* FORESEE F35SQA002G 2Gbit */
	{
		.name      = "F35SQA002G",
		.id        = {0xcd, 0x72, 0x72},
		.id_len    = 3,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	{
		.name      = "HYF2GQ4UAACAE",
		.id        = {0xC9, 0x52},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	{
		.name      = "FM25S01A",
		.id        = {0xA1, 0xE4},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	{
		.name      = "FM25S02A",
		.id        = {0xA1, 0xE5},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	{
		.name      = "FM25S01B",
		.id        = {0xA1, 0xD4},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x70, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},
  
	{
		.name      = "F35SQA512M",
		.id        = {0xcd, 0x70, 0x70},
		.id_len    = 3,
		.chipsize  = _64M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos = BBP_FIRST_PAGE,
		.driver    = &spi_nand_driver_gd,
		.flags = FLAGS_SET_QE_BIT | FLAGS_ENABLE_X2_BIT | FLAGS_ENABLE_X4_BIT,
		.ecc_en_feature_offset = 0xb0, /* Configuration register */
		.ecc_en_mask = 1 << 4, /* bit 4 */
		.ecc_status_offset = 0xc0, /* Status register */
		.ecc_status_mask = 0x30, /* bit 4 & 5 */
		.ecc_status_shift = 4,
		.ecc_status_uncorr_val = 0x2,
		.sck_l = 1,
		.sck_h = 1,
		.max_freq = SPI_NAND_FREQ_62MHz,
		.sample_param = 0x40001000,
		.xtal_switch = 1,
	},

	{	.id_len    = 0,	},
};

static int do_spinand_show_support_list(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	char buf[128] = {0};
	struct cvsnfc_chip_info *info = cvsnfc_spi_nand_flash_table;
	uint32_t id, size, i;

	printf("\t###################### nand flash list #########################\n\n");

	sprintf(buf, "\t%-16s           %-12s		   %-4s\n", "NAME", "JEDEC_ID", "SIZE");
	printf("%s\n", buf);
	for (; info->name; info++) {
		id = 0;
		size = 0;

		for (i = 0; i < info->id_len; i++)
			id |= info->id[i] << (info->id_len - i - 1) * 8;

		size = info->chipsize >> 20;
		sprintf(buf, "\t%-16s           0x%-12x          %-4u M\n", info->name, id, size);
		printf("%s\n", buf);
	}
	printf("\t###############################################################\n\n");
	return 0;
}

U_BOOT_CMD(nandshow, 2, 0, do_spinand_show_support_list, "nandshow", "\nshow nand flash list\n");

static inline unsigned int ANY_BP_ENABLE(unsigned int val)
{
	return ((PROTECTION_BP3_MASK & (val)) || (PROTECTION_BP2_MASK & (val)) ||
		(PROTECTION_BP1_MASK & (val)) || (PROTECTION_BP0_MASK & (val)));
}

/*****************************************************************************/
static void cvsnfc_spi_probe(struct cvsnfc_host *host,
			     struct cvsnfc_chip_info *spi_dev)
{
	unsigned int regval;
	struct cvsnfc_op *spi = host->spi;

	if (DEBUG_SPI_IDS)
		printf("%s\n", __func__);

	spi->host = host;
	spi->driver = spi_dev->driver;

	spi_feature_op(host, GET_OP, PROTECTION_ADDR, &regval);

	spi->driver->wait_ready(spi);

	if (ANY_BP_ENABLE(regval)) {
		regval &= ~ALL_BP_MASK;
		spi_feature_op(host, SET_OP, PROTECTION_ADDR, &regval);


		spi->driver->wait_ready(spi);
		spi_feature_op(host, GET_OP, PROTECTION_ADDR, &regval);
		if (ANY_BP_ENABLE(regval))
			printf("%s write protection disable fail! val[%#x]\n",
			       __func__, regval);
	}
}

/*****************************************************************************/
static void dump_nand_info(struct cvsnfc_chip_info *nandinfo)
{
	pr_info("====== SPI NAND INFO ======\n");

	switch (nandinfo->id_len) {
	case 2:
		pr_info("model name: %s, MID=0x%x, DID=0x%x\n", nandinfo->name,
			nandinfo->id[0], nandinfo->id[1]);
		break;
	case 3:
		pr_info("model name: %s, MID=0x%x, DID=0x%x 0x%x\n", nandinfo->name,
			nandinfo->id[0], nandinfo->id[1], nandinfo->id[2]);
		break;
	default:
		pr_info("model name: %s with wrong id length\n", nandinfo->name);
	}

	pr_info("chip size=%lldMB, diesize=%lldMB, erase size=%dKB\n", (nandinfo->chipsize >> 20),
		(nandinfo->diesize >> 20), (nandinfo->erasesize >> 10));

	pr_info("page size=%dKB, oob size=%dB, badblock_pos=0x%x, flags=0x%x\n", (nandinfo->pagesize >> 10),
		nandinfo->oobsize, nandinfo->badblock_pos, nandinfo->flags);

	pr_info("ECC enable offset=0x%x, ECC_EN mask=0x%x, STATUS offset=0x%x, STATUS mask=0x%x\n",
		nandinfo->ecc_en_feature_offset, nandinfo->ecc_en_mask, nandinfo->ecc_status_offset,
		nandinfo->ecc_status_mask);
	pr_info("STATUS shift=%d, STATUS valid_val=0x%x\n", nandinfo->ecc_status_shift,
		nandinfo->ecc_status_uncorr_val);
	pr_info("sck_l=%d, sck_h=%d, max_freq=0x%x, sample_param=0x%x\n", nandinfo->sck_l, nandinfo->sck_h,
		nandinfo->max_freq, nandinfo->sample_param);
	pr_info("xtal_switch=%d\n", nandinfo->xtal_switch);

	pr_info("\n");
}

static struct nand_flash_dev *spi_nand_get_flash_info(struct mtd_info *mtd,
						      struct nand_chip *chip,
						      struct nand_flash_dev_ex *flash_dev_ex)
{
	struct cvsnfc_host *host = (struct cvsnfc_host *)chip->priv;
	struct cvsnfc_chip_info *spi_dev = cvsnfc_spi_nand_flash_table;
	unsigned char i = 0;
	unsigned char *byte = flash_dev_ex->ids;
	struct nand_flash_dev *flash_type = &flash_dev_ex->flash_dev;

	host->flags = 0;

	// find match device ID and assign correct parameter and ops
	for (; spi_dev->id_len; spi_dev++) {

		if (memcmp(byte, spi_dev->id, DEFAULT_ID_LEN))
			continue;

		if (spi_dev->id_len > DEFAULT_ID_LEN) {
			if (!i) {
				chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);
				host->offset = DEFAULT_ID_LEN;
				for (i = DEFAULT_ID_LEN;
					i < spi_dev->id_len; i++)
					byte[i] = chip->read_byte(mtd);
			}

			if (memcmp(byte, spi_dev->id, spi_dev->id_len))
				continue;
		}

		flash_type->name = spi_dev->name;
		flash_type->mfr_id = byte[0];
		flash_type->dev_id = byte[1];
		flash_type->pagesize  = spi_dev->pagesize;
		flash_type->chipsize = spi_dev->chipsize >> 20;
		flash_type->erasesize = spi_dev->erasesize;
		host->diesize = spi_dev->diesize;

		printf("Device ID : ");

		for (int j = 0; j < spi_dev->id_len; j++)
			printf("%#x ", byte[j]);

		if (flash_type->name)
			printf(" Model: %s", flash_type->name);
		printf("\n");

		pr_info("SPI NAND device support plane bit ! chip->options 0x%x\n", chip->options);

		if (spi_dev->flags & FLAGS_SET_PLANE_BIT) {
			printf("Device support plane bit\n");
			host->flags |= FLAGS_SET_PLANE_BIT;
		}

		if (spi_dev->flags & FLAGS_CONTORL_WP_BIT) {
			printf("Control wp pin\n");
			host->flags |= FLAGS_CONTORL_WP_BIT;
		}

		if (spi_dev->flags & FLAGS_SUPPORT_W_TWICE_DEBUG) {
			printf("Device support write twice debugging\n");
			host->flags |= FLAGS_SUPPORT_W_TWICE_DEBUG;
		}

		flash_dev_ex->oobsize = spi_dev->oobsize;

		if (host->mtd != mtd)
			host->mtd = mtd;
		cvsnfc_spi_probe(host, spi_dev);
		host->nand_chip_info = spi_dev;

		dump_nand_info(host->nand_chip_info);

		if ((host->nand_chip_info->flags & FLAGS_SET_QE_BIT) && host->nand_chip_info->driver->qe_enable)
			host->nand_chip_info->driver->qe_enable(host->spi);

		return flash_type;
	}

	printf("unrecognized device id!!!\nDevice ID : ");
	for (int j = 0; j < DEFAULT_ID_LEN; j++)
		printf("%#x ", byte[j]);
	printf("\n");
	assert(0);

	return NULL;
}

/*****************************************************************************/
void spi_nand_ids_register(void)
{
	nand_get_spl_flash_type = spi_nand_get_flash_info;
}
