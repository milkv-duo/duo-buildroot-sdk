#ifndef CVSNFC_SPI_IDSH
#define CVSNFC_SPI_IDSH

/*****************************************************************************/
#define INFINITE			(0xFFFFFFFF)

#define DEFAULT_ID_LEN			2
#define MAX_ID_LEN			8
#define MAX_SPI_NAND_OP			8

#define BBP_LAST_PAGE			0x01
#define BBP_FIRST_PAGE			0x02

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

#define FLAGS_SET_PLANE_BIT			BIT(0)
#define FLAGS_ONLY_ONEBIT_MODE_BIT		BIT(1)
#define FLAGS_CONTORL_WP_BIT			BIT(2)
#define FLAGS_SUPPORT_W_TWICE_DEBUG		BIT(3)
#define FLAGS_SUPPORT_EARLY_BAD_BLOCK		BIT(4)
#define FLAGS_SUPPORT_READ_ECCSR		BIT(5)
#define FLAGS_SUPPORT_4BIT_READ			BIT(6)
#define FLAGS_SUPPORT_4BIT_WRITE		BIT(7)
#define FLAGS_ECC_STATUS_REMAP1			BIT(8)
#define FLAGS_NAND_NO_QE			BIT(9)
#define FLAGS_NAND_HAS_TWO_DIE			BIT(10)

#define FLAGS_FOUND_EARLY_BAD_BLOCK	BIT(31)


/*****************************************************************************/
struct cvsnfc_host;
int cvsnfc_scan_nand(struct cvsnfc_host *host);
void cvsnfc_get_best_clock(unsigned int *clock);
void spi_feature_op(struct cvsnfc_host *host, int op, int addr, unsigned int *val);

/******************************************************************************/

#endif /* CVSNFC_SPI_IDSH */
