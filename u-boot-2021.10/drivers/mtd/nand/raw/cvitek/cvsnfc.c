#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <malloc.h>
//#include <match_table.h>
#include <linux/mtd/nand.h>
#include <cvsnfc_common.h>
#include <cvsnfc_spi_ids.h>
#include <cvsnfc.h>
#include <watchdog.h>

#ifdef CONFIG_ARCH_MMU
#include <asm/cache-cp15.h>
#endif

#if CONFIG_IS_ENABLED(RISCV)
#include <asm/cache.h>
#endif

//#define CVSNFC_DEBUG
#ifdef CVSNFC_DEBUG
#define DEBUG_ERASE	1
#define DEBUG_WRITE	1
#define DEBUG_READ	1
#define DEBUG_DMA	1
#define DEBUG_REG	1
#define DEBUG_CMD_FLOW	1
#define DEBUG_ECC	1
#else
#define DEBUG_ERASE	0
#define DEBUG_WRITE	0
#define DEBUG_READ	0
#define DEBUG_DMA	0
#define DEBUG_REG	0
#define DEBUG_CMD_FLOW	0
#define DEBUG_ECC	0
#endif

#ifdef DEBUG_WRITE_TWICE
uint8_t dbg_pg_buf[MAX_PAGE_SIZE + MAX_SPARE_SIZE];
#endif

static int cvsnfc_dev_ready(struct mtd_info *mtd);
//static void cvsnfc_ctrl_ecc(struct mtd_info *mtd, bool enable);
static void  cvsnfc_setup_intr(struct cvsnfc_host *host);

static struct cvsnfc_host snfc_host = {
	.chip = NULL,
};

/*****************************************************************************/
void cvsnfc_register_dump(void)
{
	if (DEBUG_REG) {
		int ix;
		void __iomem *regbase = (void __iomem *)CONFIG_CVSNFC_REG_BASE_ADDRESS;

		printf("Register dump:");
		for (ix = 0; ix <= 0x7C; ix += 0x04) {
			if (!(ix & 0x0F))
				printf("\n0x%08lX: ", (uintptr_t)(regbase + ix));
			printf("%08X ", readl(regbase + ix));
		}
		printf("\n");
	}
}

/*****************************************************************************/
/* Send set/get features command to SPI Nand flash */
#define DEBUG_SPI_NAND_DRV 0

void spi_feature_op(struct cvsnfc_host *host, int op, int fe, unsigned int *val)
{
	if (op == GET_OP) {
		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 1 << TRX_DATA_SIZE_SHIFT | 1 << TRX_CMD_CONT_SIZE_SHIFT);
		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
		cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, fe << 8 | SPI_NAND_CMD_GET_FEATURE);

		cvsnfc_setup_intr(host);
		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

		CVSNFC_CMD_WAIT_CPU_FINISH(host);
		CVSNFC_CLEAR_INT(host);

		*val = cvsfc_read(host, REG_SPI_NAND_RX_DATA) & 0xff;
		if (DEBUG_CMD_FLOW)
			pr_info("cvsnfc: get feature [%#x]<==[%#x]\n", fe, *val);
	} else {
		uint32_t fe_set = fe | (*val << 8);

		if (DEBUG_CMD_FLOW)
			pr_info("cvsnfc: set feature [%#x] = [%#x]\n", fe, *val);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 2 << TRX_CMD_CONT_SIZE_SHIFT);
		cvsfc_write(host,  REG_SPI_NAND_TRX_CTRL3, BIT_REG_TRX_RW);
		cvsfc_write(host,  REG_SPI_NAND_TRX_CMD0, (fe_set << TRX_CMD_CONT0_SHIFT) | SPI_NAND_CMD_SET_FEATURE);

		cvsnfc_setup_intr(host);
		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

		CVSNFC_CMD_WAIT_CPU_FINISH(host);
		CVSNFC_CLEAR_INT(host);
	}
}

/*****************************************************************************/
static void cvsnfc_send_cmd_pageprog(struct cvsnfc_host *host)
{
	unsigned char pages_per_block_shift;
	unsigned int val, block_num, block_num_h, page_num;
	struct cvsnfc_op *spi = host->spi;
	struct nand_chip *chip = host->chip;
#ifdef CVSNFC_SUPPORT_REG_WRITE
	const char *op_type = "reg";
#else
	const char *op_type = "dma";
#endif

	if (DEBUG_WRITE)
		pr_info("* Enter %s page program!\n", op_type);

	val = spi->driver->wait_ready(spi);
	if (val) {
		printf("%s: %s page program wait ready fail! status[%#x]\n",
		       __func__, op_type, val);
		return;
	}

	if (spi->driver->write_enable(spi)) {
		printf("%s %s page program write enable failed!\n", __func__,
		       op_type);
		return;
	}

	host->set_system_clock(spi->write, ENABLE);

	val = CVSNFC_INT_CLR_ALL;
	cvsfc_write(host, CVSNFC_INT_CLR, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG INT_CLR[0x14]%#x\n", val);

	val = CVSNFC_OP_CFG_MEM_IF_TYPE(spi->write->iftype);
	cvsfc_write(host, CVSNFC_OP_CFG, val);

	if (DEBUG_WRITE)
		pr_info("  Set REG OP_CFG[0x28]%#x\n", val);

	pages_per_block_shift = chip->phys_erase_shift - chip->page_shift;
	block_num = host->addr_value[1] >> pages_per_block_shift;
	block_num_h = block_num >> REG_CNT_HIGH_BLOCK_NUM_SHIFT;
	val = CVSNFC_ADDRH_SET(block_num_h);
	cvsfc_write(host, CVSNFC_ADDRH, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG ADDRH[0x2c]%#x\n", val);

	page_num = host->addr_value[1] - (block_num << pages_per_block_shift);
	val = ((block_num & REG_CNT_BLOCK_NUM_MASK) << REG_CNT_BLOCK_NUM_SHIFT)
	     | ((page_num & REG_CNT_PAGE_NUM_MASK) << REG_CNT_PAGE_NUM_SHIFT);
	cvsfc_write(host, CVSNFC_ADDRL, val);

	if (DEBUG_WRITE)
		pr_info("  Set REG ADDRL[0x30]%#x\n", val);

#ifndef CVSNFC_SUPPORT_REG_WRITE
	val = CVSNFC_DMA_CTRL_ALL_ENABLE;
	cvsfc_write(host, CVSNFC_DMA_CTRL, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG DMA_CTRL[0x3c]%#x\n", val);

	val = host->dma_buffer;
	cvsfc_write(host, CVSNFC_DMA_SADDR_D, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG DMA_SADDR_D[0x40]%#x\n", val);

	val = host->dma_oob;
	cvsfc_write(host, CVSNFC_DMA_SADDR_OOB, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG DMA_SADDR_OOB[%#x]%#x\n",
			 CVSNFC_DMA_SADDR_OOB, val);
#ifdef CONFIG_ARCH_MMU
	dcache_clean_range((unsigned int)host->dma_buffer,
			   (unsigned int)(host->dma_buffer +
			   host->pagesize + host->oobsize));
#endif

#endif

	val = CVSNFC_OP_CTRL_WR_OPCODE(spi->write->cmd)
		| CVSNFC_OP_CTRL_CS_OP(host->cmd_option.chipselect)
#ifdef CVSNFC_SUPPORT_REG_WRITE
		| CVSNFC_OP_CTRL_OP_TYPE(OP_TYPE_REG)
#else
		| CVSNFC_OP_CTRL_OP_TYPE(OP_TYPE_DMA)
#endif
		| CVSNFC_OP_CTRL_RW_OP(RW_OP_WRITE)
		| CVSNFC_OP_CTRL_OP_READY;
	cvsfc_write(host, CVSNFC_OP_CTRL, val);
	if (DEBUG_WRITE)
		pr_info("  Set REG OP_CTRL[0x34]%#x\n", val);

	CVSNFC_DMA_WAIT_INT_FINISH(host);

	if (DEBUG_WRITE) {
		val = spi->driver->wait_ready(spi);

		if (val & STATUS_P_FAIL_MASK)
			printf("cvsnfc: %s page program failed! status[%#x]\n",
			       op_type, val);
	}

	if (DEBUG_WRITE)
		pr_info("* End %s page program!\n", op_type);
}

/*****************************************************************************/
static void cvsnfc_send_cmd_readstart(struct cvsnfc_host *host)
{
	unsigned char pages_per_block_shift, only_oob = 0;
	unsigned short wrap = 0;
	unsigned int val, block_num, block_num_h, page_num, addr_of = 0;
	struct cvsnfc_op *spi = host->spi;
	struct nand_chip *chip = host->chip;

#ifdef CVSNFC_SUPPORT_REG_READ
	char *op_type = "reg";
#else
	char *op_type = "dma";
#endif

	if (DEBUG_READ)
		pr_info("* Enter %s page read start!\n", op_type);

	if ((host->addr_value[0] == host->cache_addr_value[0]) &&
	    (host->addr_value[1] == host->cache_addr_value[1])) {
		if (DEBUG_READ)
			pr_info("* %s page read cache hit! addr1[%#x], addr0[%#x]\n",
				 op_type, host->addr_value[1], host->addr_value[0]);
		return;
	}

	val = spi->driver->wait_ready(spi);
	if (val) {
		printf("%s: %s read wait ready fail! status[%#x]\n", __func__,
		       op_type, val);
		return;
	}

	host->set_system_clock(spi->read, ENABLE);

	val = CVSNFC_INT_CLR_ALL;
	cvsfc_write(host, CVSNFC_INT_CLR, val);
	if (DEBUG_READ)
		pr_info("  Set REG INT_CLR[0x14]%#x\n", val);

	if (host->cmd_option.last_cmd == NAND_CMD_READOOB) {
		only_oob = 1;
		host->cmd_option.op_config =
				CVSNFC_OP_CFG_RD_OP_SEL(RD_OP_READ_OOB);
	} else
		host->cmd_option.op_config =
				CVSNFC_OP_CFG_RD_OP_SEL(RD_OP_READ_PAGE);

	val = host->cmd_option.op_config
		| CVSNFC_OP_CFG_MEM_IF_TYPE(spi->read->iftype)
		| CVSNFC_OP_CFG_DUMMY_ADDR_NUM(spi->read->dummy);
	cvsfc_write(host, CVSNFC_OP_CFG, val);
	if (DEBUG_READ)
		pr_info("  Set REG OP_CFG[0x28]%#x\n", val);

	pages_per_block_shift = chip->phys_erase_shift - chip->page_shift;
	block_num = host->addr_value[1] >> pages_per_block_shift;
	block_num_h = block_num >> REG_CNT_HIGH_BLOCK_NUM_SHIFT;

	val = CVSNFC_ADDRH_SET(block_num_h);
	cvsfc_write(host, CVSNFC_ADDRH, val);
	if (DEBUG_READ)
		pr_info("  Set REG ADDRH[0x2c]%#x\n", val);

	page_num = host->addr_value[1] - (block_num << pages_per_block_shift);
	if (only_oob)
		switch (host->ecctype) {
		case NAND_ECC_8BIT:
			addr_of = REG_CNT_ECC_8BIT_OFFSET;
			break;
		case NAND_ECC_16BIT:
			addr_of = REG_CNT_ECC_16BIT_OFFSET;
			break;
		case NAND_ECC_24BIT:
			addr_of = REG_CNT_ECC_24BIT_OFFSET;
			break;
		case NAND_ECC_0BIT:
		default:
			break;
		}

	val = (((block_num & REG_CNT_BLOCK_NUM_MASK) << REG_CNT_BLOCK_NUM_SHIFT)
		| ((page_num & REG_CNT_PAGE_NUM_MASK) << REG_CNT_PAGE_NUM_SHIFT)
		| ((wrap & REG_CNT_WRAP_MASK) << REG_CNT_WRAP_SHIFT)
		| (addr_of & REG_CNT_ECC_OFFSET_MASK));
	cvsfc_write(host, CVSNFC_ADDRL, val);
	if (DEBUG_READ)
		pr_info("  Set REG ADDRL[0x30]%#x\n", val);

#ifndef CVSNFC_SUPPORT_REG_READ
	val = CVSNFC_DMA_CTRL_ALL_ENABLE;
	cvsfc_write(host, CVSNFC_DMA_CTRL, val);
	if (DEBUG_READ)
		pr_info("  Set REG DMA_CTRL[0x3c]%#x\n", val);

	val = host->dma_buffer;
	cvsfc_write(host, CVSNFC_DMA_SADDR_D, val);
	if (DEBUG_READ)
		pr_info("  Set REG DMA_SADDR_D[0x40]%#x\n", val);

	val = host->dma_oob;
	cvsfc_write(host, CVSNFC_DMA_SADDR_OOB, val);
	if (DEBUG_READ)
		pr_info("  Set REG DMA_SADDR_OOB[%#x]%#x\n",
			 CVSNFC_DMA_SADDR_OOB, val);

#ifdef CONFIG_ARCH_MMU
	dcache_inv_range((unsigned int)host->dma_buffer,
			 (unsigned int)(host->dma_buffer +
			 host->pagesize + host->oobsize));
#endif

#endif

	val = CVSNFC_OP_CTRL_RD_OPCODE(spi->read->cmd)
		| CVSNFC_OP_CTRL_CS_OP(host->cmd_option.chipselect)
#ifdef CVSNFC_SUPPORT_REG_READ
		| CVSNFC_OP_CTRL_OP_TYPE(OP_TYPE_REG)
#else
		| CVSNFC_OP_CTRL_OP_TYPE(OP_TYPE_DMA)
#endif
		| CVSNFC_OP_CTRL_RW_OP(RW_OP_READ)
		| CVSNFC_OP_CTRL_OP_READY;
	cvsfc_write(host, CVSNFC_OP_CTRL, val);
	if (DEBUG_READ)
		pr_info("  Set REG OP_CTRL[0x34]%#x\n", val);

	CVSNFC_DMA_WAIT_INT_FINISH(host);

	host->cache_addr_value[0] = host->addr_value[0];
	host->cache_addr_value[1] = host->addr_value[1];

	if (DEBUG_READ)
		pr_info("* End %s page read start!\n", op_type);
}

/*****************************************************************************/
void cvsnfc_send_cmd_erase(struct cvsnfc_host *host)
{
	unsigned int val;
	struct cvsnfc_op *spi = host->spi;
	uint32_t row_addr = host->addr_value[1];
	uint32_t r_row_addr = ((row_addr & 0xff0000) >> 16) | (row_addr & 0xff00)
			    | ((row_addr & 0xff) << 16);

	if (DEBUG_ERASE) {
		printf("* Enter send cmd erase\n");
		printf("%s row_addr 0x%x\n", __func__, host->addr_value[1]);
	}
	if (spi->driver->select_die) {
		unsigned int die_id =
			row_addr / (host->diesize / host->pagesize);

		spi->driver->select_die(spi, die_id);
	}

	val = spi->driver->wait_ready(spi);
	if (val) {
		printf("cvsnfc: erase wait ready fail! status[%#x]\n", val);
		return;
	}

	if (spi->driver->write_enable(spi)) {
		printf("%s erase write enable failed\n", __func__);
		return;
	}

	if (DEBUG_ERASE) {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &val);
		pr_info("  Get feature addr[0xC0], val[%#x]\n", val);
	}

	if (DEBUG_ERASE) {
		pr_info("%s row_addr 0x%x\n", __func__, host->addr_value[1]);
		pr_info("r_row_addr 0x%x\n", r_row_addr);
	}

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 3 << TRX_CMD_CONT_SIZE_SHIFT); // 3 bytes for 24-bit row address
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0x0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, (r_row_addr << TRX_CMD_CONT0_SHIFT) | SPI_NAND_CMD_BLOCK_ERASE);

	cvsnfc_setup_intr(host);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	CVSNFC_CMD_WAIT_CPU_FINISH(host);
	CVSNFC_CLEAR_INT(host);

	val = spi->driver->wait_ready(spi);
	if (val & STATUS_E_FAIL_MASK)
		printf("cvsnfc: erase failed! status[%#x]\n", val);

}

/*****************************************************************************/
static void cvsnfc_send_cmd_status(struct cvsnfc_host *host)
{
	unsigned int regval, addr = 0;

	if ((host->cmd_option.last_cmd == NAND_CMD_ERASE1) ||
	    (host->cmd_option.last_cmd == NAND_CMD_PAGEPROG))
		addr = PROTECTION_ADDR;
	else
		addr = STATUS_ADDR;

	spi_feature_op(host, GET_OP, addr, &regval);

	if (DEBUG_ERASE || DEBUG_WRITE)
		pr_info("cvsnfc: %s get %#x status[%#x]\n",
			 ((host->cmd_option.last_cmd == NAND_CMD_ERASE1)  ? "erase" : "write"),
			addr, regval);
}

void  cvsnfc_setup_intr(struct cvsnfc_host *host)
{
	cvsfc_write(host, REG_SPI_NAND_INT_EN, 0x1f1);
	cvsfc_write(host, REG_SPI_NAND_INT_CLR, BITS_SPI_NAND_INT_CLR_ALL);
	cvsfc_write(host, REG_SPI_NAND_INT_MASK, 0);
}

/*****************************************************************************/
static void cvsnfc_send_cmd_readid(struct cvsnfc_host *host)
{
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 0x00030001);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, SPI_NAND_CMD_READ_ID);
	//cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL, BIT_REG_BOOT_PRD);

	cvsnfc_setup_intr(host);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	CVSNFC_CMD_WAIT_CPU_FINISH(host);
	CVSNFC_CLEAR_INT(host);
}

/*****************************************************************************/
static void cvsnfc_send_cmd_reset(struct cvsnfc_host *host)
{
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, SPI_NAND_CMD_RESET);

	cvsnfc_setup_intr(host);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	CVSNFC_CMD_WAIT_CPU_FINISH(host);
	CVSNFC_CLEAR_INT(host);
}

/*****************************************************************************/
static uint8_t cvsnfc_read_byte(struct mtd_info *mtd)
{
	unsigned char value = 0;
	struct nand_chip *chip = mtd->priv;
	static unsigned int data;

	struct cvsnfc_host *host = chip->priv;

	if ((host->offset % 4) == 0) {
		data = cvsfc_read(host, REG_SPI_NAND_RX_DATA);
		pr_info("cvsnfc_read_byte read data 0x%x, offset %d\n", data, host->offset);
	}

	if (host->cmd_option.last_cmd == NAND_CMD_READID) {
		unsigned char off = host->offset % 4;

		value = (data & (0xff << (off * 8))) >> (off * 8);
		pr_info("read id off %d, value 0x%x\n", host->offset, value);

		host->offset++;

		if (host->cmd_option.date_num == host->offset) {
			host->cmd_option.last_cmd = 0;
			host->offset = 0;
		}

		return value;
	}

	if ((host->cmd_option.last_cmd == NAND_CMD_ERASE1) ||
	    (host->cmd_option.last_cmd == NAND_CMD_PAGEPROG)) {
		return value;
	}

	if (host->cmd_option.last_cmd == NAND_CMD_ERASE2) {
		return value;
	}

	if (host->cmd_option.command == NAND_CMD_STATUS) {
		return value;
	}

	if (host->cmd_option.last_cmd == NAND_CMD_READOOB) {
		return value;
	}

	host->offset++;

	pr_info("%s return\n", __func__);

	return 0;// FIXME : readb(host->buffer + host->column + host->offset - 1);
}

/*****************************************************************************/
static u16 cvsnfc_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct cvsnfc_host *host = chip->priv;

	host->offset += 2;
	return readw(host->buffer + host->column + host->offset - 2);
}

/*****************************************************************************/
static void cvsnfc_write_buf(struct mtd_info *mtd,
			     const uint8_t *buf, int len)
{
}

/*****************************************************************************/
static void cvsnfc_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
}

/*****************************************************************************/
static void cvsnfc_select_chip(struct mtd_info *mtd, int chipselect)
{
}

static void cvsnfc_cmdfunc(struct mtd_info *mtd, unsigned int cmd, int col,
			   int page)
{
	struct nand_chip *chip = mtd->priv;
	struct cvsnfc_host *host = chip->priv;

	if (DEBUG_CMD_FLOW)
		pr_info("=> %s, r 0x%p\n", __func__, __builtin_return_address(0));

	switch (cmd) {
	case NAND_CMD_PAGEPROG:
		break;
	case NAND_CMD_STATUS:
		host->offset = 0;

		break;
	case NAND_CMD_READID:
		host->offset = 0;
		host->cmd_option.last_cmd = cmd;
		host->cmd_option.date_num = MAX_ID_LEN;
		host->send_cmd_readid(host); // cvsnfc_send_cmd_readid
		break;
	case NAND_CMD_PARAM:

		break;
	case NAND_CMD_READ0:
	case NAND_CMD_SEQIN:
		break;
	case NAND_CMD_RESET:
		/* resets a specific device connected to the core */
		host->cmd_option.last_cmd = cmd;
//		cvsnfc_send_cmd_reset(host);
//		cvsnfc_dev_ready(mtd);

		break;
	case NAND_CMD_READOOB:
		break;
	case NAND_CMD_RNDOUT:
		break;
	case NAND_CMD_ERASE1:
		host->addr_value[0] = 0;
		host->addr_value[1] = page;
		host->cmd_option.last_cmd = cmd;
		host->send_cmd_erase(host); //cvsnfc_send_cmd_erase
		break;
	case NAND_CMD_ERASE2:
		break;
	default:
		pr_info("unsupported command received 0x%x\n", cmd);
		break;
	}

	if (DEBUG_CMD_FLOW)
		pr_info("<= %s, exit\n", __func__);
}

/*****************************************************************************/
static void cvsnfc_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
	unsigned char cmd;
	int is_cache_invalid = 1;
	struct nand_chip *chip = mtd->priv;
	struct cvsnfc_host *host = chip->priv;

	if (DEBUG_CMD_FLOW)
		pr_info("%s\n", __func__);

	if (ctrl & NAND_ALE) {
		unsigned int addr_value = 0;
		unsigned int addr_offset = 0;

		if (ctrl & NAND_CTRL_CHANGE) {
			host->addr_cycle = 0x0;
			host->addr_value[0] = 0x0;
			host->addr_value[1] = 0x0;
		}
		addr_offset = host->addr_cycle << 3;

		if (host->addr_cycle >= CVSNFC_ADDR_CYCLE_MASK) {
			addr_offset = (host->addr_cycle -
					CVSNFC_ADDR_CYCLE_MASK) << 3;
			addr_value = 1;
		}

		host->addr_value[addr_value] |=
			((dat & 0xff) << addr_offset);

		host->addr_cycle++;
	}

	if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE)) {
		cmd = dat & 0xff;
		host->cmd_option.command = cmd;
		switch (cmd) {
		case NAND_CMD_PAGEPROG:
			host->offset = 0;
			host->send_cmd_pageprog(host);
			break;

		case NAND_CMD_READSTART:
			is_cache_invalid = 0;
			if (host->addr_value[0] == host->pagesize)
				host->cmd_option.last_cmd = NAND_CMD_READOOB;
			host->send_cmd_readstart(host);
			break;

		case NAND_CMD_ERASE2:
			host->cmd_option.last_cmd = cmd;
			host->send_cmd_erase(host);
			break;

		case NAND_CMD_READID:
			memset((unsigned char *)(chip->IO_ADDR_R), 0, MAX_ID_LEN);
			host->cmd_option.last_cmd = cmd;
			host->cmd_option.date_num = MAX_ID_LEN;
			host->send_cmd_readid(host);
			break;

		case NAND_CMD_STATUS:
			host->send_cmd_status(host);
			break;

		case NAND_CMD_SEQIN:
			break;

		case NAND_CMD_ERASE1:
			break;

		case NAND_CMD_READ0:
			host->cmd_option.last_cmd = cmd;
			break;

		case NAND_CMD_RESET:
			host->send_cmd_reset(host);
			break;

		default:
			break;
		}
	}

	if ((dat == NAND_CMD_NONE) && host->addr_cycle) {
		if (host->cmd_option.command == NAND_CMD_SEQIN ||
		    host->cmd_option.command == NAND_CMD_READ0 ||
		    host->cmd_option.command == NAND_CMD_READID) {
			host->offset = 0x0;
			host->column = (host->addr_value[0] & 0xffff);
		}
	}

	if (is_cache_invalid) {
		host->cache_addr_value[0] = ~0;
		host->cache_addr_value[1] = ~0;
	}

	if (DEBUG_CMD_FLOW)
		pr_info("%s exit\n", __func__);
}

/*****************************************************************************/
static int cvsnfc_waitfunc(struct mtd_info *mtd, struct nand_chip *this)
{
	unsigned int regval;
	unsigned int deadline = 0;
	struct cvsnfc_host *host = this->priv;

	if (DEBUG_CMD_FLOW)
		pr_info("=>%s\n", __func__);

	do {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &regval);

		if (!(regval & STATUS_OIP_MASK))
			return NAND_STATUS_READY;

		udelay(1);
	} while (deadline++ < (40 << 5));

	if (DEBUG_CMD_FLOW)
		pr_info("Wait spi nand flash ready timeout.\n");

	return NAND_STATUS_FAIL;
}

/*****************************************************************************/
static int cvsnfc_dev_ready(struct mtd_info *mtd)
{
	unsigned int regval;
	unsigned int deadline = 0;
	struct nand_chip *chip = mtd->priv;
	struct cvsnfc_host *host = chip->priv;

	do {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &regval);
		if (!(regval & STATUS_OIP_MASK))
			return 1;
		//udelay(1);
	} while (deadline++ < (40 << 5));

	if (DEBUG_CMD_FLOW)
		pr_info("Wait spi nand flash ready timeout.\n");

	return 0;
}

/*****************************************************************************/
/*
 * 'host->epm' only use the first oobfree[0] field, it looks very simple, But...
 */
static struct nand_ecclayout nand_ecc_default = {
	.oobfree = {{2, 30} }
};

/*****************************************************************************/
static struct nand_config_info cvsnfc_spi_nand_config_table[] = {
	{NAND_PAGE_4K,	NAND_ECC_24BIT,	200,		&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_16BIT,	144,		&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_8BIT,	128/*88*/,	&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_0BIT,	32,		&nand_ecc_default},

	{NAND_PAGE_2K,	NAND_ECC_24BIT,	128/*116*/,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_16BIT,	88,		&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_8BIT,	64/*60*/,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_0BIT,	32,		&nand_ecc_default},

	{0, 0, 0, NULL},
};

/*****************************************************************************/
/* used the best correct arithmetic. */
struct nand_config_info *cvsnfc_get_best_ecc(struct mtd_info *mtd)
{
	struct nand_config_info *best = NULL;
	struct nand_config_info *info = cvsnfc_spi_nand_config_table;

	for (; info->layout; info++) {
		if (nandpage_type2size(info->pagetype) != mtd->writesize)
			continue;

		if (mtd->oobsize < info->oobsize)
			continue;

		if (!best || (best->ecctype < info->ecctype))
			best = info;
	}

	if (!best)
		pr_info("Driver does not support the pagesize (%d) and oobsize(%d).\n",
			 mtd->writesize, mtd->oobsize);

	return best;
}

/*****************************************************************************/
/* force the pagesize and ecctype */
struct nand_config_info *cvsnfc_force_ecc(struct mtd_info *mtd, int pagetype,
					  int ecctype, char *cfgmsg, int allow_pagediv)
{
	int pagesize;
	struct nand_config_info *fit = NULL;
	struct nand_config_info *info = cvsnfc_spi_nand_config_table;

	if (DEBUG_ECC)
		printf("pagetype %d, ecctype %d\n", pagetype, ecctype);

	// NAND_PAGE_2K, NAND_ECC_8BIT
	for (; info->layout; info++) {
		if (info->pagetype == pagetype && info->ecctype == ecctype) {
			if (DEBUG_ECC)
				printf("pagetype %d, ecctype %d\n", pagetype, ecctype);
			fit = info;
			break;
		}
	}

	if (!fit) {
		cvsnfc_register_dump();
		return NULL;
	}

	pagesize = nandpage_type2size(pagetype);

	if (DEBUG_ECC)
		printf("pagesize %d, mtd->writesize %d\n", pagesize, mtd->writesize);

	if ((pagesize != mtd->writesize) &&
	    (pagesize > mtd->writesize || !allow_pagediv)) {
		cvsnfc_register_dump();
		return NULL;
	}

	if (DEBUG_ECC)
		printf("fit->oobsize %d, mtd->oobsize %d\n", fit->oobsize, mtd->oobsize);

	if (fit->oobsize > mtd->oobsize)
		return NULL;

	return fit;
}

/*****************************************************************************/
int cvsnfc_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip,
		     struct nand_flash_dev_ex *flash_dev_ex)
{
	char *start_type = "unknown";
	struct nand_config_info *best = NULL;
	struct cvsnfc_host *host = chip->priv;
	unsigned int pagetype, ecctype;


#ifdef CONFIG_CVSNFC_HARDWARE_PAGESIZE_ECC

	pagetype = nandpage_size2type(mtd->writesize);
	ecctype = NAND_ECC_8BIT; // FIXME!!

	best = cvsnfc_force_ecc(mtd, pagetype, ecctype, "hardware config", 0);
	start_type = "Hardware";

#endif /* CONFIG_CVSNFC_HARDWARE_PAGESIZE_ECC */

	if (!best)
		DBG_BUG(ERSTR_HARDWARE
			"Please configure SPI Nand Flash pagesize and ecctype!\n");

	if (DEBUG_ECC)
		printf("best->oobsize %d best->ecctype %d\n", best->oobsize, best->ecctype);

	if (best->ecctype != NAND_ECC_0BIT)
		mtd->oobsize = best->oobsize;

	if (flash_dev_ex->oobsize > mtd->oobsize)
		mtd->oobsize = flash_dev_ex->oobsize;

	if (DEBUG_ECC)
		printf("mtd->oobsize %d\n", mtd->oobsize);

	chip->ecc.layout = best->layout;

	host->ecctype  = NAND_ECC_8BIT;
	host->pagesize = mtd->writesize;
	host->oobsize  = mtd->oobsize;
	host->block_page_cnt = (mtd->erasesize / mtd->writesize);
	host->block_page_mask = ((mtd->erasesize / mtd->writesize) - 1);

	pr_info("host->block_page_cnt %d\n", host->block_page_cnt);
	pr_info("host->block_page_mask %d\n", host->block_page_mask);

	host->dma_oob = host->dma_buffer + host->pagesize;

	host->bbm = (unsigned char *)(host->buffer + host->pagesize
			+ HINFC_BAD_BLOCK_POS);

	host->epm = (unsigned short *)(host->buffer + host->pagesize
			+ chip->ecc.layout->oobfree[0].offset + 28);

	if (mtd->writesize > SPI_NAND_MAX_PAGESIZE ||
	    mtd->oobsize > SPI_NAND_MAX_OOBSIZE) {
		pr_info("Driver does not support this Nand Flash.\n");
		pr_info("Please increase SPI_NAND_MAX_PAGESIZE and SPI_NAND_MAX_OOBSIZE.\n");
	}

	flash_dev_ex->start_type = start_type;
	flash_dev_ex->ecctype = host->ecctype;

	return 0;
}

static int spi_nand_read_page(struct cvsnfc_host *host, int page)
{
	int row_addr = page;
	int r_row_addr = ((row_addr & 0xff0000) >> 16) | (row_addr & 0xff00) | ((row_addr & 0xff) << 16);

	if (DEBUG_READ)
		pr_info("%s row_addr 0x%x\n", __func__, row_addr);

	if (DEBUG_READ)
		pr_info("%s r_row_addr 0x%x\n", __func__, r_row_addr);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 0 << TRX_DATA_SIZE_SHIFT | 3 << TRX_CMD_CONT_SIZE_SHIFT);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, r_row_addr << TRX_CMD_CONT0_SHIFT | SPI_NAND_CMD_PAGE_READ_TO_CACHE);

	cvsnfc_setup_intr(host);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	CVSNFC_CMD_WAIT_CPU_FINISH(host);
	CVSNFC_CLEAR_INT(host);

	cvsnfc_dev_ready(host->mtd);

	if (DEBUG_READ)
		pr_info("<=%s\n", __func__);

	//flush_dcache_all();

	return 0;
}

/*
 * spi_nand user guide v0.3.docx
 * 5.4 : Write Command with DMA Data
 *
 * rw : 0 for read, 1 for write
 */
static void spi_nand_rw_dma_setup(struct cvsnfc_host *host, const uint8_t *buf, int len, int rw)
{
	int ch = 0;
	void __iomem *remap_base = (void __iomem *)CVI_SYSDMA_REMAP_BASE;

	if (DEBUG_DMA)
		pr_info("%s: buf %p, len %d, %s, dmabase %p\n", __func__, buf, len,
			 rw == 1 ? "dma write" : "dma read", host->dmabase);

	writel(0x00000003, host->dmabase + 0x010);
	writel(0x00000f00, host->dmabase + 0x018);

	writel((len / 4) - 1, host->dmabase + (0x110 + ch * (0x100)));
	writel(0x000f0792, host->dmabase + (0x11C + ch * (0x100)));

	if (rw) {
//		// for dma write
		writel(((uintptr_t)buf) & 0xFFFFFFFF, host->dmabase + 0x100 + ch * (0x100));
		writel(((uintptr_t)buf) >> 32, host->dmabase + 0x104 + ch * (0x100));

		if (DEBUG_DMA)
			pr_info("w0x100: 0x%x\n", readl(host->dmabase + 0x100 + ch * (0x100)));
		if (DEBUG_DMA)
			pr_info("w0x104: 0x%x\n", readl(host->dmabase + 0x104 + ch * (0x100)));

		writel((uintptr_t)host->regbase + 0x800, host->dmabase + 0x108 + ch * (0x100));
		writel(0, host->dmabase + 0x10C + ch * (0x100));

		if (DEBUG_DMA)
			pr_info("w0x108: 0x%x\n", readl(host->dmabase + 0x108 + ch * (0x100)));
		if (DEBUG_DMA)
			pr_info("w0x10C: 0x%x\n", readl(host->dmabase + 0x10C + ch * (0x100)));

		writel(0x00045441, host->dmabase + 0x118 + ch * (0x100));
		writel(0x00000001, host->dmabase + 0x124 + ch * (0x100)); // [0:2] = 1 : MEM_TO_PER_DMAC, PER dst = 0

		writel(0x26 << (ch * 8), remap_base); //PER dst => remap chx
		uint32_t val = readl(remap_base) | 0x80000000; // update bit

		writel(val, remap_base); // set sdma remap update bit

		if (DEBUG_DMA)
			pr_info("w0x03000154: 0x%x\n", readl(remap_base));

		flush_dcache_range((unsigned long)buf, (unsigned long)(buf + len));
	} else {
		// for dma read
		writel((uintptr_t)host->regbase + 0xC00, host->dmabase + 0x100 + ch * (0x100));
		writel(0, host->dmabase + 0x104 + ch * (0x100));

		if (DEBUG_DMA)
			pr_info("r0x100: 0x%x\n", readl(host->dmabase + 0x100 + ch * (0x100)));
		if (DEBUG_DMA)
			pr_info("r0x104: 0x%x\n", readl(host->dmabase + 0x104 + ch * (0x100)));

		writel(((uintptr_t)buf) & 0xFFFFFFFF, host->dmabase + 0x108 + ch * (0x100));
		writel(((uintptr_t)buf) >> 32, host->dmabase + 0x10C + ch * (0x100));

		if (DEBUG_DMA)
			pr_info("r0x108: 0x%x\n", readl(host->dmabase + 0x108 + ch * (0x100)));
		if (DEBUG_DMA)
			pr_info("r0x10C: 0x%x\n", readl(host->dmabase + 0x10C + ch * (0x100)));

		writel(0x00046214, host->dmabase + 0x118 + ch * (0x100));
		writel(0x00000002, host->dmabase + 0x124 + ch * (0x100)); // [0:2] = 2 : PER_TO_MEM_DMAC, PER src = 0

		writel(0x26 << (ch * 8), remap_base); //PER src => remap chx
		uint32_t val = readl(remap_base) | 0x80000000; // update bit

		writel(val, remap_base); // set sdma remap update bit

		if (DEBUG_DMA)
			pr_info("r0x03000154: 0x%x\n", readl(remap_base));
	}

	//flush_dcache_range((unsigned long)buf, (unsigned long)(buf + len));

	writel(0, host->dmabase + 0x120 + ch * (0x100));
	writel(0x00000101, host->dmabase + 0x018);
}

static void spi_nand_set_read_from_cache_mode(struct cvsnfc_host *host, uint32_t mode, uint32_t r_col_addr)
{
	switch (mode) {
	case SPI_NAND_READ_FROM_CACHE_MODE_X1:
		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, BIT_REG_TRX_DMA_EN | BIT_REG_TRX_DUMMY_HIZ);
		cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, r_col_addr << TRX_CMD_CONT0_SHIFT |
			    SPI_NAND_CMD_READ_FROM_CACHE);
		break;
	case SPI_NAND_READ_FROM_CACHE_MODE_X2:
		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, BIT_REG_TRX_DMA_EN |
			    SPI_NAND_CTRL3_IO_TYPE_X2_MODE | BIT_REG_TRX_DUMMY_HIZ);
		cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, r_col_addr << TRX_CMD_CONT0_SHIFT |
			    SPI_NAND_CMD_READ_FROM_CACHEX2);

		break;
	case SPI_NAND_READ_FROM_CACHE_MODE_X4:
		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, BIT_REG_TRX_DMA_EN |
			    SPI_NAND_CTRL3_IO_TYPE_X4_MODE | BIT_REG_TRX_DUMMY_HIZ);
		cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, r_col_addr << TRX_CMD_CONT0_SHIFT |
			    SPI_NAND_CMD_READ_FROM_CACHEX4);
		break;
	default:
		pr_err("unsupport mode!\n");
		break;
	}
}

static int parse_status_info(struct cvsnfc_host *host)
{
	uint32_t statusc0 = 0;
	uint8_t ecc_sts = 0;
	int ret;

	spi_feature_op(host, GET_OP, host->nand_chip_info->ecc_status_offset, &statusc0);
	ecc_sts = (statusc0 & host->nand_chip_info->ecc_status_mask) >> host->nand_chip_info->ecc_status_shift;

	if (ecc_sts == 0) {
		pr_debug("ECC pass!!\n");
		ret = 0;
	} else if (ecc_sts == host->nand_chip_info->ecc_status_uncorr_val) {
		pr_info("%s statusc0 0x%x\n", __func__, statusc0);
		pr_info("ECC_UNCORR!!\n");
		ret = -EBADMSG;
	} else {
		pr_debug("%s statusc0 0x%x\n", __func__, statusc0);
		pr_debug("ECC Corr!!\n");
		ret = 1;
	}

	return ret;
}

static int spi_nand_read_from_cache(struct cvsnfc_host *host, int col_addr, int len, void *buf)
{

	int ret = 0;
	int r_col_addr = ((col_addr & 0xff00) >> 8) | ((col_addr & 0xff) << 8);

	if (DEBUG_READ)
		pr_info("=>%s\n", __func__);

	if (DEBUG_READ)
		pr_info("%s col_addr 0x%x, len %d\n", __func__, col_addr, len);

	spi_nand_rw_dma_setup(host, buf, len, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, len << TRX_DATA_SIZE_SHIFT | 3 << TRX_CMD_CONT_SIZE_SHIFT);

	//here might be changed to use 4 bit mode if support QE bit
	if (host->nand_chip_info->flags & FLAGS_ENABLE_X4_BIT)
		spi_nand_set_read_from_cache_mode(host, SPI_NAND_READ_FROM_CACHE_MODE_X4, r_col_addr);
	else if (host->nand_chip_info->flags & FLAGS_ENABLE_X2_BIT)
		spi_nand_set_read_from_cache_mode(host, SPI_NAND_READ_FROM_CACHE_MODE_X2, r_col_addr);
	else
		spi_nand_set_read_from_cache_mode(host, SPI_NAND_READ_FROM_CACHE_MODE_X1, r_col_addr);


	cvsnfc_setup_intr(host);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0,
		    cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	flush_dcache_range((unsigned long)buf, (unsigned long)(buf + len));

	CVSNFC_CMD_WAIT_CPU_FINISH(host);
	CVSNFC_WAIT_DMA_FINISH(host);
	CVSNFC_CLEAR_INT(host);

	ret = parse_status_info(host);

	return ret;
}

//extern void bbt_dump_buf(char *s, void *buf, int len);
#define SPI_NAND_PLANE_BIT_OFFSET	BIT(12)

int cvsnfc_read_page(struct mtd_info *mtd, struct nand_chip *chip,
		     uint8_t *buf, int bytes, int page)
{
	struct cvsnfc_host *host = chip->priv;
	uint32_t col_addr = 0;
	struct cvsnfc_op *spi = host->spi;
	int ret = 0;

	if (DEBUG_READ)
		printf("=>%s, page 0x%x\n", __func__, page);

	if (spi->driver->select_die) {
		unsigned int die_id = page / (host->diesize / host->pagesize);

		spi->driver->select_die(spi, die_id);
	}

	//cvsnfc_ctrl_ecc(mtd, 1);

	spi_nand_read_page(host, page);

	uint32_t blk_idx = page / host->block_page_cnt;

	if (DEBUG_READ)
		pr_info("%s, blk_idx %d, page 0x%x, chip->options 0x%x\n",
			 __func__, blk_idx, page, chip->options);

	if (host->flags & FLAGS_SET_PLANE_BIT && (blk_idx & BIT(0))) {
		if (DEBUG_READ)
			pr_info("%s set plane bit for blkidx %d\n", __func__, blk_idx);
		col_addr |= SPI_NAND_PLANE_BIT_OFFSET;
	}

	ret = spi_nand_read_from_cache(host, col_addr, mtd->writesize, buf);

	if (ret == -EBADMSG) {
		pr_info("%s : ECC status ECC_UNCORR on page 0x%x, block %d\n", __func__, page, blk_idx);
	}

	return ret;
}

static void cv_spi_nand_adjust_freq(struct cvsnfc_host *host, uint8_t sck_l, uint8_t sck_h, uint16_t max_freq)
{
	uint32_t val;

	switch (max_freq) {
	case SPI_NAND_FREQ_XTAL: /* use XTAL, 25/4 = 6.25Mhz */
		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL,
			    (cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL) | BIT_REG_BOOT_PRD));

		val = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1) & ~BIT_REG_NAND_SCK_MASK;

		val |= SPI_NAND_SET_SCK_L(1) | SPI_NAND_SET_SCK_H(0);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, val);
		break;
	case SPI_NAND_FREQ_23MHz: /* use FPLL, 187.5/8 = 23.4375Mhz */
		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL,
			    (cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL) & ~BIT_REG_BOOT_PRD));

		val = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1) & ~BIT_REG_NAND_SCK_MASK;

		if ((sck_l + sck_h) != 6)
			val |= SPI_NAND_SET_SCK_L(3) | SPI_NAND_SET_SCK_H(3);
		else
			val |= SPI_NAND_SET_SCK_L(sck_l) | SPI_NAND_SET_SCK_H(sck_h);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, val);
		break;
	case SPI_NAND_FREQ_26MHz: /* use FPLL, 187.5/7 = 26.7857Mhz */
		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL,
			    (cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL) & ~BIT_REG_BOOT_PRD));

		val = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1) & ~BIT_REG_NAND_SCK_MASK;

		if ((sck_l + sck_h) != 5)
			val |= SPI_NAND_SET_SCK_L(3) | SPI_NAND_SET_SCK_H(2);
		else
			val |= SPI_NAND_SET_SCK_L(sck_l) | SPI_NAND_SET_SCK_H(sck_h);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, val);
		break;
	case SPI_NAND_FREQ_31MHz: /* use FPLL, 187.5/6 = 31.25Mhz */
		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL,
			    (cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL) & ~BIT_REG_BOOT_PRD));

		val = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1) & ~BIT_REG_NAND_SCK_MASK;

		if ((sck_l + sck_h) != 4)
			val |= SPI_NAND_SET_SCK_L(2) | SPI_NAND_SET_SCK_H(2);
		else
			val |= SPI_NAND_SET_SCK_L(sck_l) | SPI_NAND_SET_SCK_H(sck_h);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, val);
		break;
	case SPI_NAND_FREQ_37MHz: /* use FPLL, 187.5/5 = 37.5Mhz */
		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL,
			    (cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL) & ~BIT_REG_BOOT_PRD));

		val = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1) & ~BIT_REG_NAND_SCK_MASK;

		if ((sck_l + sck_h) != 3)
			val |= SPI_NAND_SET_SCK_L(2) | SPI_NAND_SET_SCK_H(1);
		else
			val |= SPI_NAND_SET_SCK_L(sck_l) | SPI_NAND_SET_SCK_H(sck_h);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, val);
		break;
	case SPI_NAND_FREQ_46MHz: /* use FPLL, 187.5/4 = 46.875Mhz */
		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL,
			    (cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL) & ~BIT_REG_BOOT_PRD));

		val = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1) & ~BIT_REG_NAND_SCK_MASK;

		if ((sck_l + sck_h) != 2)
			val |= SPI_NAND_SET_SCK_L(1) | SPI_NAND_SET_SCK_H(1);
		else
			val |= SPI_NAND_SET_SCK_L(sck_l) | SPI_NAND_SET_SCK_H(sck_h);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, val);
		break;
	case SPI_NAND_FREQ_62MHz: /* use FPLL, 187.5/3 = 62.5Mhz */
		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL,
			    (cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL) & ~BIT_REG_BOOT_PRD));

		val = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1) & ~BIT_REG_NAND_SCK_MASK;

		if ((sck_l + sck_h) != 1)
			val |= SPI_NAND_SET_SCK_L(1) | SPI_NAND_SET_SCK_H(0);
		else
			val |= SPI_NAND_SET_SCK_L(sck_l) | SPI_NAND_SET_SCK_H(sck_h);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, val);
		break;
	case SPI_NAND_FREQ_93MHz: /* use FPLL, 187.5/2 = 93.75Mhz */
		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL,
			    (cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL) & ~BIT_REG_BOOT_PRD));

		val = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1) & ~BIT_REG_NAND_SCK_MASK;

		val |= SPI_NAND_SET_SCK_L(0) | SPI_NAND_SET_SCK_H(0);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, val);
		break;
	default:
		break;
	}
}

#define PATTERN1_OFFSET 0
#define PATTERN2_OFFSET 8

int spi_nand_adjust_max_freq(struct cvsnfc_host *host, uint32_t *pattern1, uint32_t *pattern2)
{
	uint dly_ow_val, rsp_pos_sel, dly_line_sel;
	int ori_boot_ctl;
	int ori_trx_ctl;
	int val1, val2;
	unsigned char header_buf[32];
	struct cvsnfc_dly_param_grp dly_grp[10];
	unsigned int dly_grp_idx;
	unsigned int dly_param[128]; /* the range of dly_line_sel is 0 ~ 127 */
	unsigned int dly_param_idx;
	unsigned int old_dly_param;
	unsigned int max_count;
	unsigned int select_grp;

	dly_ow_val = 1; /* fix with 1 clk*/
	rsp_pos_sel = 0; /* fix to select nagetive edge */
	max_count = 0;
	select_grp = 0;

	ori_boot_ctl = cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL);
	ori_trx_ctl = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1);

	val1 = ori_boot_ctl & ~BIT_REG_RSP_DLY_MASK;
	host->nand_chip_info->max_freq = SPI_NAND_FREQ_93MHz;

	printf("Tuning SPINAND run on maximum speed...\n");

freq_retry:
	dly_grp_idx = 0;
	dly_param_idx = 0;
	old_dly_param = 0;

	memset(&dly_grp, 0x0, sizeof(struct cvsnfc_dly_param_grp) * 10);

	cv_spi_nand_adjust_freq(host, host->nand_chip_info->sck_l, host->nand_chip_info->sck_h,
				host->nand_chip_info->max_freq);

	for (dly_line_sel = 0; dly_line_sel < 128; dly_line_sel++) {
		val2 = val1;
		unsigned int nand_id;

		val2 = (BIT_REG_RSP_DLY_OW_EN | (dly_ow_val << 8) | (rsp_pos_sel << 12) | (dly_line_sel << 16));
		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL, val2);

		cvsnfc_send_cmd_readid(host);

		nand_id = cvsfc_read(host, REG_SPI_NAND_RX_DATA);

		if (!memcmp(&nand_id, host->nand_chip_info->id, host->nand_chip_info->id_len)) {
			int ret;

			memset(header_buf, 0x0, 0x20);
			ret = spi_nand_read_from_cache(host, 0, 0x20, header_buf); /* read first 32 bytes */
			if (ret == -EBADMSG) {
				printf("%s : ECC status ECC_UNCORR\n", __func__);
			}
			if (!memcmp(header_buf + PATTERN1_OFFSET, pattern1, 4) &&
			    !memcmp(header_buf + PATTERN2_OFFSET, pattern2, 4)) {
				if (dly_param_idx == 0 && dly_grp_idx == 0) {
					dly_grp[dly_grp_idx].start = dly_param_idx;
					dly_grp[dly_grp_idx].count = 1;
				} else if ((val2 - old_dly_param) == 0x10000) {
					dly_grp[dly_grp_idx].count++;
				} else {
					dly_grp_idx++;
					dly_grp[dly_grp_idx].start = dly_param_idx;
					dly_grp[dly_grp_idx].count = 1;
				}

				old_dly_param = val2;
				dly_param[dly_param_idx] = cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL);
				dly_param_idx++;
			}
		}
	}

	pr_info("dly_param_idx=%d, dly_grp_idx=%d\n", dly_param_idx, dly_grp_idx);
	if (dly_param_idx != 0) { /* at least find 1 parameter */
		for (int i = 0; i <= dly_grp_idx; i++) {

			if (dly_grp[i].count > max_count) {
				select_grp = i;
				max_count = dly_grp[i].count;
			}
			pr_info("dly_param[%d].start=%d, count=%d\n", i, dly_grp[i].start, dly_grp[i].count);
		}

		cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL, dly_param[dly_grp[select_grp].start + (max_count / 2)]);
		return 0;
	}

	if (dly_param_idx == 0 && host->nand_chip_info->max_freq != 0) {
		if (host->nand_chip_info->max_freq > 0) {
			host->nand_chip_info->max_freq--;
			goto freq_retry;
		}
	}

	/* Can't find an suitable parameter for this frequency, use original one */
	cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL, ori_boot_ctl);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, ori_trx_ctl);

	return -EINVAL;
}

void cvsnfc_ctrl_ecc(struct mtd_info *mtd, bool enable)
{
	unsigned int status = 0;
	struct nand_chip *chip = mtd->priv;
	struct cvsnfc_host *host = chip->priv;

	if (DEBUG_CMD_FLOW)
		pr_info("%s %s ecc\n", __func__, (enable) ? "enable" : "disable");

	spi_feature_op(host, GET_OP, FEATURE_ADDR, &status);

	if (enable) {
		status = status | (SPI_NAND_FEATURE0_ECC_EN);
		spi_feature_op(host, SET_OP, FEATURE_ADDR, &status);
	} else {	//disable ecc
		status = status & ~(SPI_NAND_FEATURE0_ECC_EN);
		spi_feature_op(host, SET_OP, FEATURE_ADDR, &status);
	}
}

static int read_oob_data(struct mtd_info *mtd, uint8_t *buf, int page)
{
	struct nand_chip *chip = mtd->priv;
	struct cvsnfc_host *host = chip->priv;
	uint32_t col_addr = mtd->writesize;
	uint32_t blk_idx = page / host->block_page_cnt;
	struct cvsnfc_op *spi = host->spi;
	int ret = 0;

	if (DEBUG_READ)
		pr_info("%s, page 0x%x, buf %p, mtd->oobsize %d\n", __func__,
			 page, buf, mtd->oobsize);

	if (spi->driver->select_die) {
		unsigned int die_id = page / (host->diesize / host->pagesize);

		spi->driver->select_die(spi, die_id);
	}

	cvsnfc_ctrl_ecc(mtd, 0);

	spi_nand_read_page(host, page);

	if (host->flags & FLAGS_SET_PLANE_BIT && (blk_idx & BIT(0))) {
		pr_info("%s set plane bit for blkidx %d\n", __func__, blk_idx);
		col_addr |= SPI_NAND_PLANE_BIT_OFFSET;
	}

	ret = spi_nand_read_from_cache(host, col_addr, mtd->oobsize, buf);

	if (DEBUG_READ) {
		bbt_dump_buf("1oob data:", buf, 16);
	}

	cvsnfc_ctrl_ecc(mtd, 1);

	if (ret != 0) {
		printf("%s : ECC status ret %d page 0x%x\n", __func__, ret, page);
	}

	return ret;
}

static int cvsnfc_read_oob(struct mtd_info *mtd, struct nand_chip *chip,
			   int page)
{
	if (DEBUG_READ)
		printf("%s\n", __func__);

	return read_oob_data(mtd, chip->oob_poi, page);
}

int cvsnfc_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
			 uint8_t *buf, int oob_required, int page)
{
	struct cvsnfc_host *host = chip->priv;
	uint32_t col_addr = 0;
	uint32_t blk_idx = page / host->block_page_cnt;
	struct cvsnfc_op *spi = host->spi;

	if (DEBUG_READ)
		pr_info("%s, page 0x%x\n", __func__, page);

	if (spi->driver->select_die) {
		unsigned int die_id = page / (host->diesize / host->pagesize);

		spi->driver->select_die(spi, die_id);
	}

	cvsnfc_ctrl_ecc(mtd, 0);

	spi_nand_read_page(host, page);

	if (host->flags & FLAGS_SET_PLANE_BIT && (blk_idx & BIT(0))) {
		if (DEBUG_READ)
			pr_info("%s set plane bit for blkidx %d\n", __func__, blk_idx);
		col_addr |= SPI_NAND_PLANE_BIT_OFFSET;
	}

	spi_nand_read_from_cache(host, col_addr, mtd->writesize + mtd->oobsize, buf);

	if (DEBUG_READ)
		bbt_dump_buf("read raw page:", buf, mtd->writesize + mtd->oobsize);

	cvsnfc_ctrl_ecc(mtd, 1);

	return 0;
}

static int cvsnfc_read_subpage(struct mtd_info *mtd, struct nand_chip *chip,
			       u32 data_offs, u32 readlen, u8 *buf, int page)
{
	struct cvsnfc_host *host = chip->priv;
	uint32_t col_addr = 0;
	struct cvsnfc_op *spi = host->spi;
	int ret = 0;

	if (DEBUG_READ)
		pr_info("=>%s, page 0x%x, data_offs %d, readlen %d\n",
			 __func__, page, data_offs, readlen);

	if (spi->driver->select_die) {
		unsigned int die_id = page / (host->diesize / host->pagesize);

		spi->driver->select_die(spi, die_id);
	}

	//cvsnfc_ctrl_ecc(mtd, 1);

	spi_nand_read_page(host, page);

	uint32_t blk_idx = page / host->block_page_cnt;

	if (host->flags & FLAGS_SET_PLANE_BIT && (blk_idx & BIT(0))) {
		pr_info("%s set plane bit for blkidx %d\n", __func__, blk_idx);
		col_addr |= SPI_NAND_PLANE_BIT_OFFSET;
	}

	ret = spi_nand_read_from_cache(host, col_addr, mtd->writesize, host->buffer);

	if (ret != 0) {
		printf("%s : ECC status ret %d page 0x%x\n", __func__, ret, page);
	}

	if (DEBUG_READ)
		bbt_dump_buf("data:", buf, 16);

	memcpy(buf, host->buffer, host->pagesize);

	if (DEBUG_READ)
		pr_info("<=%s\n", __func__);

	return 0;
}

static uint8_t spi_nand_prog_load(struct cvsnfc_host *host, const uint8_t *buf,
				  size_t size, uint32_t col_addr, uint32_t qe)
{
	uint8_t cmd = qe ? SPI_NAND_CMD_PROGRAM_LOADX4 : SPI_NAND_CMD_PROGRAM_LOAD;
	uint32_t r_col_addr = ((col_addr & 0xff00) >> 8) | ((col_addr & 0xff) << 8);
	uint32_t ctrl3 = 0;

	if (DEBUG_WRITE)
		pr_info("=>%s size %ld, col_addr 0x%x, r_col_addr 0x%x,  qe %d\n",
			 __func__, size, col_addr, r_col_addr, qe);

	spi_nand_rw_dma_setup(host, buf, size, 1);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, (size << TRX_DATA_SIZE_SHIFT) | 2 << TRX_CMD_CONT_SIZE_SHIFT);

	ctrl3 = qe ? (BIT_REG_TRX_RW | BIT_REG_TRX_DMA_EN | SPI_NAND_CTRL3_IO_TYPE_X4_MODE) :
		      (BIT_REG_TRX_RW | BIT_REG_TRX_DMA_EN);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, ctrl3);

	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, cmd | (r_col_addr << TRX_CMD_CONT0_SHIFT));

	cvsnfc_setup_intr(host);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	CVSNFC_CMD_WAIT_CPU_FINISH(host);
	CVSNFC_WAIT_DMA_FINISH(host);
	CVSNFC_CLEAR_INT(host);

	if (DEBUG_WRITE)
		pr_info("<=%s\n", __func__);

	return 0;
}

static int spi_nand_prog_exec(struct cvsnfc_host *host, uint32_t row_addr)
{
	uint32_t r_row_addr = ((row_addr & 0xff0000) >> 16) | (row_addr & 0xff00) | ((row_addr & 0xff) << 16);

	if (DEBUG_WRITE)
		pr_info("row_addr 0x%x\n", row_addr);

	if (DEBUG_WRITE)
		pr_info("r_row_addr 0x%x\n", r_row_addr);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 3 << TRX_CMD_CONT_SIZE_SHIFT);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, SPI_NAND_CMD_PROGRAM_EXECUTE | (r_row_addr << TRX_CMD_CONT0_SHIFT));
	cvsfc_write(host, REG_SPI_NAND_RSP_POLLING, 0xff00ff);

	cvsnfc_setup_intr(host);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	CVSNFC_CMD_WAIT_CPU_FINISH(host);
	CVSNFC_CLEAR_INT(host);

	cvsnfc_dev_ready(host->mtd);

	if (DEBUG_WRITE)
		pr_info("<=%s\n", __func__);

	return 0;
}

static int write_page(struct mtd_info *mtd, struct nand_chip *chip,
		      const uint8_t *buf, int oob_required, int page)
{
	struct cvsnfc_host *host = chip->priv;
	struct cvsnfc_op *spi = host->spi;
	uint32_t col_addr = 0;

	if (DEBUG_WRITE)
		printf("=>%s, buf %p, page 0x%x\n", __func__, buf, page);

	if (spi->driver->select_die) {
		unsigned int die_id = page / (host->diesize / host->pagesize);

		spi->driver->select_die(spi, die_id);
	}

	if (spi->driver->write_enable(spi)) {
		printf("%s write enable failed!\n", __func__);
		return -1;
	}

	uint32_t row_addr = page;
	uint32_t blk_idx = page /  host->block_page_cnt;

	if (DEBUG_WRITE)
		pr_info("blk idx %d\n", blk_idx);

	cvsnfc_ctrl_ecc(mtd, 1);

	if (host->flags & FLAGS_SET_PLANE_BIT && (blk_idx & BIT(0))) {
		if (DEBUG_WRITE)
			pr_info("%s set plane bit for blkidx %d\n", __func__, blk_idx);
		col_addr |= SPI_NAND_PLANE_BIT_OFFSET;
	}

	spi_nand_prog_load(host, buf, host->pagesize, col_addr, 0);

	WATCHDOG_RESET();

	spi_nand_prog_exec(host, row_addr);

	if (DEBUG_WRITE)
		pr_info("<=%s\n", __func__);

	return 0;
}

int cvsnfc_write_page(struct mtd_info *mtd, struct nand_chip *chip,
		      const uint8_t *buf, int oob_required, int page)
{
	/*
	 * for regular page writes, we let HW handle all the ECC
	 * data written to the device.
	 */

#ifdef DEBUG_WRITE_TWICE
	struct cvsnfc_host *host = chip->priv;

	if (host->flags & FLAGS_SUPPORT_W_TWICE_DEBUG) {
		int status = cvsnfc_read_page_raw(mtd, chip, dbg_pg_buf, 0, page);
		u32 page_size = mtd->writesize;
		u32 spare_size = mtd->oobsize;
		u32 offset = page_size + (spare_size / 2);
		u32 i = 0;
		u32 write_twice = 0;

		if (status) {
			printf("read status %d\n", status);
			assert(0);
		}

		u8 *mark = (u8 *)&dbg_pg_buf[offset];

		for (i = 0; i < spare_size / 2; i++) {
			if (mark[i] != 0xff) {
				write_twice = 1;
				printf("offset %d, mark 0x%x\n", i, mark[i]);
			}
		}

		if (write_twice) {
			printf("%s pg 0x%x already programmed\n", __func__, page);
			return 0;
		}
	}
#endif

	return write_page(mtd, chip, buf, oob_required, page);
}

/*****************************************************************************/
void cvsnfc_nand_init(struct nand_chip *chip)
{
	struct cvsnfc_host *host = chip->priv;

	cvsnfc_setup_intr(host);

	chip->read_byte   = cvsnfc_read_byte;
	chip->read_word   = cvsnfc_read_word;
	chip->write_buf   = cvsnfc_write_buf;
	chip->read_buf    = cvsnfc_read_buf;

	chip->select_chip = cvsnfc_select_chip;
	chip->cmdfunc = cvsnfc_cmdfunc;
	chip->waitfunc = cvsnfc_waitfunc;
	chip->cmd_ctrl    = cvsnfc_cmd_ctrl;
	chip->dev_ready   = cvsnfc_dev_ready;

	chip->chip_delay  = CVSNFC_CHIP_DELAY;

	chip->options     = NAND_BROKEN_XD;// | NAND_BBT_SCANNED;

	chip->bbt_options = NAND_BBT_USE_FLASH | NAND_BBT_NO_OOB;

	chip->ecc.layout  = NULL;
//	chip->ecc.mode    = NAND_ECC_NONE;

	/* override the default read operations */
	chip->ecc.size = 512; // FIXME!!
	chip->ecc.strength = 8; // FIXME!!
	chip->ecc.read_page = cvsnfc_read_page;
	chip->ecc.read_oob = cvsnfc_read_oob;
	chip->ecc.read_page_raw = cvsnfc_read_page_raw;
	chip->ecc.read_subpage = cvsnfc_read_subpage;
	chip->ecc.write_page = cvsnfc_write_page;

	chip->options |= NAND_SUBPAGE_READ;

	chip->ecc.mode = NAND_ECC_HW;
	chip->ecc.bytes = 0;
}

/*****************************************************************************/
int cvsnfc_host_init(struct cvsnfc_host *host)
{
	host->tx_iobase  = (void __iomem *)SPI_NAND_TX_DATA_BASE;
	host->rx_iobase  = (void __iomem *)SPI_NAND_RX_DATA_BASE;
	host->regbase = (void __iomem *)CONFIG_CVSNFC_REG_BASE_ADDRESS;
	host->dmabase = (void __iomem *)0x04330000;

	host->set_system_clock   = 0;// FIXME : cvsnfc_set_system_clock;

	if (host->set_system_clock)
		host->set_system_clock(NULL, ENABLE);

	host->buforg = kmalloc((CVSNFC_BUFFER_LEN + CVSNFC_DMA_ALIGN),
				GFP_KERNEL);

//	if (!host->buforg) {
//		pr_err("cvsnfc: Can't malloc memory for NAND driver.\n");
//		return 1;
//	}

	/* 32 bytes alignment */
	host->buffer = (char *)((uintptr_t)(host->buforg
		+ CVSNFC_DMA_ALIGN - 1) & ~(CVSNFC_DMA_ALIGN - 1));
	memset(host->buffer, 0xff, CVSNFC_BUFFER_LEN);

	host->dma_buffer = (uintptr_t)host->buffer;

	host->send_cmd_pageprog  = cvsnfc_send_cmd_pageprog;
	host->send_cmd_status    = cvsnfc_send_cmd_status;
	host->send_cmd_readstart = cvsnfc_send_cmd_readstart;
	host->send_cmd_erase     = cvsnfc_send_cmd_erase;
	host->send_cmd_readid    = cvsnfc_send_cmd_readid;
	host->send_cmd_reset     = cvsnfc_send_cmd_reset;

	spi_nand_ids_register();

	cvsnfc_register_dump();

	return 0;
}

struct cvsnfc_host *cvsnfc_get_host(void)
{
	return &snfc_host;
}

/*****************************************************************************/
int board_nand_init(struct nand_chip *nand)
{
	struct cvsnfc_host *host = &snfc_host;
	struct mtd_info *mtd = nand_to_mtd(nand);

	if (host->chip)
		return 0;

	pr_info("Init SPI Nand Flash Controller ... ");

	mtd->priv = nand;

	memset((char *)host, 0, sizeof(struct cvsnfc_host));

	host->chip = nand;
	if (cvsnfc_host_init(host)) {
		printf("cvsnfc: failed to allocate device buffer.\n");
		host->chip = 0;
		return -ENOMEM;
	}

	nand->priv = host;
	cvsnfc_nand_init(nand);

	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.bytes = 0;

	spi_nand_ids_register();
	nand_oob_resize = cvsnfc_ecc_probe;

	return 0;
}
