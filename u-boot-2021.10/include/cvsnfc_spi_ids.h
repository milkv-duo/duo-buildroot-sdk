#ifndef __CVSNFC_SPI_IDSH
#define __CVSNFC_SPI_IDSH

/*****************************************************************************/
#define INFINITE			(0xFFFFFFFF)

#define DEFAULT_ID_LEN			2
#define MAX_ID_LEN			3
#define MAX_SPI_NAND_OP			8

#define BBP_LAST_PAGE			0x01
#define BBP_FIRST_PAGE			0x02
#define BBP_FIRST_2_PAGE		0x03

/*****************************************************************************/
#define SPI_IF_READ_STD			(0x01)
#define SPI_IF_READ_FAST		(0x02)
#define SPI_IF_READ_DUAL		(0x04)
#define SPI_IF_READ_DUAL_ADDR		(0x08)
#define SPI_IF_READ_QUAD		(0x10)
#define SPI_IF_READ_QUAD_ADDR		(0x20)

#define SPI_IF_WRITE_STD		(0x01)
#define SPI_IF_WRITE_DUAL		(0x02)
#define SPI_IF_WRITE_DUAL_ADDR		(0x04)
#define SPI_IF_WRITE_QUAD		(0x08)
#define SPI_IF_WRITE_QUAD_ADDR		(0x10)

#define SPI_IF_ERASE_SECTOR_128K	(0x08)  /* 128K */
#define SPI_IF_ERASE_SECTOR_256K	(0x10)  /* 256K */

/******************************************************************************/
#define CVSNFC_SUPPORT_READ		(SPI_IF_READ_STD \
					| SPI_IF_READ_FAST \
					| SPI_IF_READ_DUAL \
					| SPI_IF_READ_DUAL_ADDR \
					| SPI_IF_READ_QUAD \
					| SPI_IF_READ_QUAD_ADDR)

#define CVSNFC_SUPPORT_WRITE		(SPI_IF_WRITE_STD | SPI_IF_WRITE_QUAD)

#define CVSNFC_SUPPORT_MAX_DUMMY	8

#define SPI_NAND_READ			0
#define SPI_NAND_WRITE			1

#define CVSNFC_IFCYCLE_STD				0
#define CVSNFC_IFCYCLE_DUAL				1
#define CVSNFC_IFCYCLE_DUAL_ADDR			2
#define CVSNFC_IFCYCLE_QUAD				3
#define CVSNFC_IFCYCLE_QUAD_ADDR			4

/*****************************************************************************/
#define SPI_CMD_WREN			0x06   /* Write Enable */
#define SPI_CMD_WRDI			0x04   /* Write Disable */

#define SPI_CMD_GET_FEATURES		0x0F   /* Get Features */
#define SPI_CMD_SET_FEATURE		0x1F   /* Set Feature */

#define SPI_CMD_PAGE_READ		0x13   /* Page Read to Cache */
#define SPI_CMD_READ_STD	0x03   /* Read From Cache at Standard Speed */
#define SPI_CMD_READ_FAST	0x0B   /* Read From Cache at Higher Speed */
#define SPI_CMD_READ_DUAL	0x3B   /* Read From Cache at Dual Output */
#define SPI_CMD_READ_QUAD	0x6B   /* Read From Cache at Quad Output */
#define SPI_CMD_READ_DUAL_ADDR	0xBB   /* Read From Cache at Dual I/O */
#define SPI_CMD_READ_QUAD_ADDR	0xEB   /* Read From Cache at Quad I/O */

#define SPI_CMD_RDID			0x9F   /* Read Identification */

#define SPI_CMD_WRITE_STD	0x02   /* Page Load at Standard Input */
#define SPI_CMD_WRITE_QUAD	0x32   /* Page Load at Quad Input */

#define SPI_CMD_SE_128K			0xD8   /* 128KB sector Erase */
#define SPI_CMD_SE_256K			0xD8   /* 256KB sector Erase */

#define SPI_CMD_RESET			0xff   /* Reset the device */

/*****************************************************************************/
/* SPI operation information */
struct spi_op_info {
	unsigned char  iftype;
	unsigned char  cmd;
	unsigned char  dummy;
	unsigned int   size;
	unsigned int   clock;
};

struct spi_nand_driver;

struct cvsnfc_op {
	void *host;
	struct spi_nand_driver *driver;
	struct spi_op_info  read[1];
	struct spi_op_info  write[1];
	struct spi_op_info  erase[MAX_SPI_NAND_OP];
};

struct spi_nand_driver {
	int (*wait_ready)(struct cvsnfc_op *spi);
	int (*write_enable)(struct cvsnfc_op *spi);
	int (*qe_enable)(struct cvsnfc_op *spi);
	int (*ecc_enable)(struct cvsnfc_op *spi);
	int (*select_die)(struct cvsnfc_op *spi, unsigned int id);
	unsigned char ecc_uncorr;
};

#define FLAGS_SET_PLANE_BIT		BIT(0)
#define NAND_SET_PLANE_BIT	BIT(0)
#define SPI_NAND_MULTIPLANE_BIT		BIT(12)

#define FLAGS_SET_QE_BIT		(BIT(1))
#define FLAGS_ENABLE_X2_BIT		(BIT(2))
#define FLAGS_ENABLE_X4_BIT		(BIT(3))
#define FLAGS_OW_SETTING_BIT	(BIT(4))
#define FLAGS_ONLY_ONEBIT_MODE_BIT	(BIT(5))
#define FLAGS_CONTORL_WP_BIT		(BIT(6))
#define FLAGS_SUPPORT_W_TWICE_DEBUG	(BIT(7))

#define SPI_NAND_FREQ_XTAL	0 /* 6.25 Mhz */
#define SPI_NAND_FREQ_23MHz	1 /* 23.4375 Mhz */
#define SPI_NAND_FREQ_26MHz	2 /* 26.7857 Mhz */
#define SPI_NAND_FREQ_31MHz	3 /* 31.25 Mhz */
#define SPI_NAND_FREQ_37MHz	4 /* 37.5 Mhz */
#define SPI_NAND_FREQ_46MHz	5 /* 46.875 Mhz */
#define SPI_NAND_FREQ_62MHz	6 /* 62.5 Mhz */
#define SPI_NAND_FREQ_93MHz	7 /* 93.75 Mhz */

struct cvsnfc_chip_info {
	char *name;
	unsigned char id[MAX_ID_LEN];
	unsigned char id_len;
	unsigned long long chipsize;
	unsigned long long diesize;
	unsigned int erasesize;
	unsigned int pagesize;
	unsigned int oobsize;
	unsigned int badblock_pos;
	unsigned int flags;
	struct spi_op_info *read[MAX_SPI_NAND_OP];
	struct spi_op_info *write[MAX_SPI_NAND_OP];
	struct spi_op_info *erase[MAX_SPI_NAND_OP];
	struct spi_nand_driver *driver;
	uint8_t ecc_en_feature_offset;
	uint8_t ecc_en_mask;
	uint8_t ecc_status_offset;
	uint8_t ecc_status_mask;
	uint8_t ecc_status_shift;
	uint8_t ecc_status_uncorr_val;
	uint8_t sck_l;
	uint8_t sck_h;
	uint16_t max_freq;
	uint32_t sample_param;
	uint8_t xtal_switch;
};

/*****************************************************************************/
void spi_nand_ids_register(void);

void cvsnfc_get_best_clock(unsigned int *clock);

struct cvsnfc_host;

void spi_feature_op(struct cvsnfc_host *host, int op, int addr, unsigned int *val);

/******************************************************************************/

#endif /* __CVSNFC_SPI_IDSH */
