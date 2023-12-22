#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/module.h>
#include <linux/mtd/rawnand.h>
#include <linux/mtd/partitions.h>
#include <linux/reset.h>
#include <linux/jiffies.h>
#include "cvsnfc_common.h"
#include "cvsnfc_spi_ids.h"
#include "cvsnfc.h"
extern struct nand_flash_dev nand_flash_cvitek_supported_ids[];
#define NAND_NAME    "cvsnfc"

#define PTR_INC(base, offset) (void *)((uint8_t *)(base) + (offset))

static int cvsnfc_dev_ready(struct nand_chip *chip);
static void cvsnfc_ctrl_ecc(struct mtd_info *mtd, bool enable);
static void  cvsnfc_setup_intr(struct cvsnfc_host *host);
static void cvsnfc_set_qe(struct cvsnfc_host *host, uint32_t enable);
extern void cvsnfc_get_flash_info(struct nand_chip *chip, unsigned char *byte);

static void cv_spi_nand_dump_reg(struct cvsnfc_host *host)
{
	uint32_t i = 0;
	uint32_t reg = 0;

	for (i = 0; i <= 0x58; i += 4) {
		reg = cvsfc_read(host, i);
		pr_info("0x%x: 0x%x\n", 0x4060000 + i, reg);
	}
}

/*****************************************************************************/
static void wait_for_irq(struct cvsnfc_host *host, struct cvsnfc_irq_status_t *irq_mask,
		struct cvsnfc_irq_status_t *irq_status)
{
	unsigned long comp_res = 0;
	unsigned long timeout = msecs_to_jiffies(10000);

	do {
		udelay(10);
		comp_res =
			wait_for_completion_timeout(&host->complete, timeout);
		spin_lock_irq(&host->irq_lock);
		*irq_status = host->irq_status;

		if (irq_status->status & irq_mask->status) {
			pr_debug("host->irq_status.status %x irq_mask->status %x\n",
					host->irq_status.status, irq_mask->status);

			host->irq_status.status &= ~irq_mask->status;
			spin_unlock_irq(&host->irq_lock);
			break;
		}

		/*
		 * these are not the interrupts you are looking for; need to wait again
		 */
		spin_unlock_irq(&host->irq_lock);
	} while (comp_res != 0);

	if (comp_res == 0) {
		/* timeout */
		dev_err(host->dev, "timeout occurred:\t status = 0x%x, mask = 0x%x\n",
				irq_status->status, irq_mask->status);

		memset(irq_status, 0, sizeof(struct cvsnfc_irq_status_t));
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
		cvsnfc_send_nondata_cmd_and_wait(host);

		*val = cvsfc_read(host, REG_SPI_NAND_RX_DATA) & 0xff;

		pr_debug("cvsnfc: get feature [%#x]<==[%#x]\n", fe, *val);
	} else {
		uint32_t fe_set = fe | (*val << 8);

		pr_debug("cvsnfc: set feature [%#x] = [%#x]\n", fe, *val);

		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 2 << TRX_CMD_CONT_SIZE_SHIFT);
		cvsfc_write(host,  REG_SPI_NAND_TRX_CTRL3, BIT_REG_TRX_RW);
		cvsfc_write(host,  REG_SPI_NAND_TRX_CMD0, (fe_set << TRX_CMD_CONT0_SHIFT) | SPI_NAND_CMD_SET_FEATURE);
		cvsnfc_send_nondata_cmd_and_wait(host);

	}
}

/*****************************************************************************/
static void cvsnfc_send_cmd_pageprog(struct cvsnfc_host *host)
{

}

/*****************************************************************************/
static void cvsnfc_send_cmd_readstart(struct cvsnfc_host *host)
{

}

/*****************************************************************************/
static void cvsnfc_send_cmd_erase(struct cvsnfc_host *host)
{
	unsigned int val;
	uint32_t row_addr = host->addr_value[1];
	struct cvsnfc_chip_info *spi_nand = &host->spi_nand;
	struct spi_nand_driver *spi_driver = spi_nand->driver;
	uint32_t r_row_addr = ((row_addr & 0xff0000) >> 16) | (row_addr & 0xff00) |
		((row_addr & 0xff) << 16);

	pr_debug("%s row_addr 0x%x\n", __func__, host->addr_value[1]);

	if (spi_driver->select_die) {
		unsigned int die_id =
			row_addr / (host->diesize / host->pagesize);

		spi_driver->select_die(host, die_id);
	}

	val = spi_driver->wait_ready(host);
	if (val) {
		pr_err("cvsnfc: erase wait ready fail! status[%#x]\n", val);
		return;
	}

	if (spi_driver->write_enable(host)) {
		pr_err("%s erase write enable failed\n", __func__);
		return;
	}

	spi_feature_op(host, GET_OP, STATUS_ADDR, &val);
	pr_debug("Get feature addr[0xC0], val[%#x]\n", val);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 3 << TRX_CMD_CONT_SIZE_SHIFT); // 3 bytes for 24-bit row address
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0x0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, (r_row_addr << TRX_CMD_CONT0_SHIFT) | SPI_NAND_CMD_BLOCK_ERASE);

	cvsnfc_send_nondata_cmd_and_wait(host);

	val = spi_driver->wait_ready(host);
	if (val & STATUS_E_FAIL_MASK)
		pr_err("cvsnfc: erase failed! status[%#x]\n", val);
}

/*****************************************************************************/
static void cvsnfc_send_cmd_status(struct cvsnfc_host *host)
{
	unsigned int regval, addr = 0;

	if ((host->cmd_option.last_cmd == NAND_CMD_ERASE1)
			|| (host->cmd_option.last_cmd == NAND_CMD_PAGEPROG))
		addr = PROTECTION_ADDR;
	else
		addr = STATUS_ADDR;

	spi_feature_op(host, GET_OP, addr, &regval);

	pr_info("cvsnfc: %s get %#x status[%#x]\n",
			((host->cmd_option.last_cmd == NAND_CMD_ERASE1)
			 ? "erase" : "write"), addr, regval);
}

static void  cvsnfc_setup_intr(struct cvsnfc_host *host)
{
	cvsfc_write(host, REG_SPI_NAND_INT_EN, 0x1f1);
	cvsfc_write(host, REG_SPI_NAND_INT_CLR, BITS_SPI_NAND_INT_CLR_ALL);
	cvsfc_write(host, REG_SPI_NAND_INT_MASK, 0);
}

/* send non-data command and wait for finish */
int cvsnfc_send_nondata_cmd_and_wait(struct cvsnfc_host *host)
{
	struct cvsnfc_irq_status_t irq_mask, irq_status;

	irq_mask.status = BIT_REG_TRX_DONE_INT_CLR;
	irq_status.status = 0;

	cvsnfc_setup_intr(host);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	wait_for_irq(host, &irq_mask, &irq_status);

	if (irq_status.status == 0) {
		u32 int_status = cvsfc_read(host, REG_SPI_NAND_INT);
		dev_err(host->dev, "%s command timeout 0x%x\n", __func__, int_status);
		cv_spi_nand_dump_reg(host);
		return -ETIMEDOUT;
	}

	return 0;
}

/*****************************************************************************/
static void cvsnfc_send_cmd_readid(struct cvsnfc_host *host)
{
	pr_debug("%s\n", __func__);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 0x00030001);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, SPI_NAND_CMD_READ_ID);
	//cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL, BIT_REG_BOOT_PRD);

	cvsnfc_send_nondata_cmd_and_wait(host);

	pr_debug("%s exit\n", __func__);
}

/*****************************************************************************/
static void cvsnfc_send_cmd_reset(struct cvsnfc_host *host)
{
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, SPI_NAND_CMD_RESET);

	cvsnfc_send_nondata_cmd_and_wait(host);
}

/*****************************************************************************/
uint8_t cvsnfc_read_byte(struct nand_chip *chip)
{
	unsigned char value = 0;
	static unsigned int data;
	struct cvsnfc_host *host = chip->priv;

	pr_debug("%s r %p\n", __func__, __builtin_return_address(0));

	if ((host->offset % 4) == 0) {
		data = cvsfc_read(host, REG_SPI_NAND_RX_DATA);
		pr_debug("cvsnfc_read_byte read data 0x%x, offset %d\n", data, host->offset);
	}

	if (host->cmd_option.last_cmd == NAND_CMD_READID) {
		unsigned char off = host->offset % 4;

		value = (data & (0xff << (off * 8))) >> (off * 8);
		pr_debug("read id off %d, value 0x%x\n", host->offset, value);

		host->offset++;

		if (host->cmd_option.date_num == host->offset) {
			host->cmd_option.last_cmd = 0;
			host->offset = 0;
		}

		return value;
	}

	if ((host->cmd_option.last_cmd == NAND_CMD_ERASE1)
			|| (host->cmd_option.last_cmd == NAND_CMD_PAGEPROG)) {
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

	pr_debug("%s return\n", __func__);

	return 0;// FIXME : readb(host->buffer + host->column + host->offset - 1);
}

/*****************************************************************************/

/*****************************************************************************/
static void cvsnfc_write_buf(struct nand_chip *chip,
	const uint8_t *buf, int len)
{
}

/*****************************************************************************/
static void cvsnfc_read_buf(struct nand_chip *chip, uint8_t *buf, int len)
{
}

/*****************************************************************************/
static void cvsnfc_select_chip(struct nand_chip *chip, int chipselect)
{
}

static void cvsnfc_cmdfunc(struct nand_chip *chip, unsigned int cmd, int col,
						  int page)
{
	struct cvsnfc_host *host = nand_get_controller_data(chip);;

	pr_debug("=> %s, r 0x%p\n", __func__, __builtin_return_address(0));

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
		dev_warn(host->dev, "unsupported command received 0x%x\n", cmd);
		break;
	}

	pr_debug("<= %s, exit\n", __func__);

}

/*****************************************************************************/
static void cvsnfc_cmd_ctrl(struct nand_chip *chip, int dat, unsigned int ctrl)
{
	unsigned char cmd;
	int is_cache_invalid = 1;
	struct cvsnfc_host *host = chip->priv;

	pr_debug("%s\n", __func__);

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
			memset((unsigned char *)(chip->legacy.IO_ADDR_R), 0,
				MAX_ID_LEN);
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
		if (host->cmd_option.command == NAND_CMD_SEQIN
		    || host->cmd_option.command == NAND_CMD_READ0
		    || host->cmd_option.command == NAND_CMD_READID) {
			host->offset = 0x0;
			host->column = (host->addr_value[0] & 0xffff);
		}
	}

	if (is_cache_invalid) {
		host->cache_addr_value[0] = ~0;
		host->cache_addr_value[1] = ~0;
	}

	pr_info("%s exit\n", __func__);

}

/*****************************************************************************/
static int cvsnfc_waitfunc(struct nand_chip *chip)
{
	unsigned int regval;
	struct cvsnfc_host *host = nand_get_controller_data(chip);
	unsigned long start_time = jiffies;
	/* 4ms */
	unsigned long max_erase_time = 4 * 3;

	pr_debug("=>%s\n", __func__);

	do {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &regval);

		if (!(regval & STATUS_OIP_MASK))
			return NAND_STATUS_READY;

	} while (jiffies_to_msecs(jiffies - start_time) < max_erase_time);

	pr_err("%s timeout.\n", __func__);

	return NAND_STATUS_FAIL;
}

/*****************************************************************************/
static int cvsnfc_dev_ready(struct nand_chip *chip)
{
	unsigned int regval;
	struct cvsnfc_host *host = chip->priv;
	unsigned long start_time = jiffies;
	/* 4ms */
	unsigned long max_erase_time = 4 * 3;

	do {
		spi_feature_op(host, GET_OP, STATUS_ADDR, &regval);
		if (!(regval & STATUS_OIP_MASK))
			return 1;
	} while (jiffies_to_msecs(jiffies - start_time) < max_erase_time);

	pr_err("%s timeout.\n", __func__);

	return 0;
}

static int cvsnfc_ecc_probe(struct cvsnfc_host *host)
{
	struct nand_memory_organization *memorg;
	struct nand_chip *chip = &host->nand;
	struct nand_flash_dev *info = &host->spi_nand.nand_info;
	struct mtd_info *mtd = nand_to_mtd(&(host->nand));

	chip->ecc.size = info->ecc.step_ds;
	chip->ecc.strength = info->ecc.strength_ds;
	chip->ecc.steps = host->pagesize / chip->ecc.size;
	chip->ecc.bytes = host->oobsize / chip->ecc.steps;

	chip->ecc.engine_type = NAND_ECC_ENGINE_TYPE_ON_HOST;
	chip->ecc.placement = NAND_ECC_PLACEMENT_INTERLEAVED;

	if (chip->ecc.strength == 1)
		chip->ecc.algo = NAND_ECC_ALGO_HAMMING;
	else
		chip->ecc.algo = NAND_ECC_ALGO_BCH;

	mtd->bitflip_threshold = DIV_ROUND_UP(info->ecc.strength_ds * 3, 4);
	pr_info("mtd->bitflip_threshold %u\n", mtd->bitflip_threshold);

	memorg = nanddev_get_memorg(&(host->nand.base));
	memorg->bits_per_cell = 1;
	if (host->pagesize > SPI_NAND_MAX_PAGESIZE
			|| host->oobsize > SPI_NAND_MAX_OOBSIZE) {
		pr_err(ERSTR_DRIVER "Driver does not support this Nand Flash.\n");
		pr_err(ERSTR_DRIVER "Please increase SPI_NAND_MAX_PAGESIZE and SPI_NAND_MAX_OOBSIZE.\n");
	}

	return 0;
}

static int spi_nand_send_read_page_cmd(struct cvsnfc_host *host, int page)
{
	int row_addr = page;
	int r_row_addr = ((row_addr & 0xff0000) >> 16) | (row_addr & 0xff00) | ((row_addr & 0xff) << 16);

	pr_debug("%s row_addr 0x%x, r_row_addr 0x%x\n", __func__, row_addr, r_row_addr);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 0 << TRX_DATA_SIZE_SHIFT | 3 << TRX_CMD_CONT_SIZE_SHIFT);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, r_row_addr << TRX_CMD_CONT0_SHIFT | SPI_NAND_CMD_PAGE_READ_TO_CACHE);

	cvsnfc_send_nondata_cmd_and_wait(host);

	cvsnfc_dev_ready(&(host->nand));

	return 0;
}

/*
 * spi_nand user guide v0.3.docx
 * 5.4 : Write Command with DMA Data
 *
 * DW_axi_dmac_databook(sysdma).pdf
 *
 * rw : 0 for read, 1 for write
 */

static void cvsnfc_dma_complete(void *arg)
{
	struct cvsnfc_host *host = (struct cvsnfc_host *) arg;

	pr_debug("%s\n", __func__);

	complete(&host->comp);

}

static void cvsnfc_ctrl_ecc(struct mtd_info *mtd, bool enable)
{
	unsigned int status = 0;
	struct nand_chip *chip = mtd->priv;
	struct cvsnfc_host *host = chip->priv;

	pr_debug("%s %s ecc\n", __func__, (enable) ? "enable":"disable");

	spi_feature_op(host, GET_OP, FEATURE_ADDR, &status);

	if (enable == ENABLE_ECC) {
		status = status | (SPI_NAND_FEATURE0_ECC_EN);
		spi_feature_op(host, SET_OP, FEATURE_ADDR, &status);
	} else {
		status = status & ~(SPI_NAND_FEATURE0_ECC_EN);
		spi_feature_op(host, SET_OP, FEATURE_ADDR, &status);
	}
}

static void spi_nand_reset_ip(struct cvsnfc_host *host)
{
	uint32_t reg_0x4;
	uint32_t reg_0x24;

	// save 0x4 and 0x24 value
	reg_0x4 = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1);
	reg_0x24 = cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL);

	pr_info("org : 0x4 = 0x%x, 0x24 = 0x%x\n", reg_0x4, reg_0x24);

	// reset and wait at least 1u
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, BIT_REG_TRX_SW_RST);
	udelay(10);

	// restore 0x4 and 0x24
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL1, reg_0x4);
	cvsfc_write(host, REG_SPI_NAND_BOOT_CTRL, reg_0x24);

	reg_0x4 = cvsfc_read(host, REG_SPI_NAND_TRX_CTRL1);
	reg_0x24 = cvsfc_read(host, REG_SPI_NAND_BOOT_CTRL);

	pr_info("after reset : 0x4 = 0x%x, 0x24 = 0x%x\n", reg_0x4, reg_0x24);
}

#define SYSDMA_TIMEOUT_MS (5 * 1000) // 5 seconds

static int spi_nand_rw_dma_setup(struct cvsnfc_host *host, void *buf, int len, int rw)
{
	struct dma_async_tx_descriptor *desc;
	unsigned long flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;
	struct dma_slave_config conf;
	struct dma_chan *chan = rw ? host->dma_chan_tx : host->dma_chan_rx;
	unsigned long res = 0;
	int ret = 0;

	pr_debug("=>%s\n", __func__);

	memset(&conf, 0, sizeof(conf));
	if (rw) {
		// write to device

		/*
		 * Set direction to a sensible value even if the dmaengine driver
		 * should ignore it. With the default (DMA_MEM_TO_MEM), the amba-pl08x
		 * driver criticizes it as "alien transfer direction".
		 */
		conf.direction = DMA_MEM_TO_DEV;
		conf.src_addr = (host->dma_buffer);
		conf.dst_addr = (host->io_base_phy + 0x800);
	} else {
		// read data from device
		conf.direction = DMA_DEV_TO_MEM;
		conf.src_addr = (host->io_base_phy + 0xC00);
		conf.dst_addr = (host->dma_buffer);
	}

	/* DMA controller does flow control: */
	conf.device_fc = false;
	conf.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	conf.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	conf.src_maxburst = 4;
	conf.dst_maxburst = 4;

	if (dmaengine_slave_config(chan, &conf)) {
		dev_err(host->dev, "Failed to setup DMA slave\n");
		return -EPERM;
	}

	sg_init_one(&host->sgl, buf, len);

	res = dma_map_sg(host->dev, &host->sgl, 1, DMA_BIDIRECTIONAL);

	if (res == 0) {
		dev_err(host->dev, "Failed to map sg list. res=%lu\n", res);
		return -ENXIO;
	}

	desc = dmaengine_prep_slave_sg(chan, &host->sgl, res, conf.direction, // dwc_prep_dma_slave_sg
			flags);
	init_completion(&host->comp);

	desc->callback = cvsnfc_dma_complete;
	desc->callback_param = (void *) host;

	host->cookie = dmaengine_submit(desc); // dwc_tx_submit

	dma_async_issue_pending(chan); // dwc_issue_pending

	res = wait_for_completion_timeout(&host->comp, msecs_to_jiffies(SYSDMA_TIMEOUT_MS * 2));

	if (res == 0) {
		void __iomem *clk_reg;
		void __iomem *sysdma_reg;
		uint32_t i = 0;

		cv_spi_nand_dump_reg(host);

		spi_nand_reset_ip(host);

		clk_reg = ioremap(0x3002000, 0x10);
		sysdma_reg = ioremap(0x4330000, 0x800);

		pr_err("%s timeout, 0x3002000=0x%x, 0x3002004=0x%x, ch0 sar=0x%x ch1 dar=0x%x\n",
				__func__, readl(clk_reg), readl(clk_reg + 4), readl(sysdma_reg + 0x100),
				readl(sysdma_reg + 0x208));

		pr_err("CFG=0x%x, CHEN=0x%x, ch0 status=0x%x, ch1 status=0x%x, callback=%p\n",
				readl(sysdma_reg+0x10), readl(sysdma_reg + 0x18), readl(sysdma_reg + 0x130),
				readl(sysdma_reg + 0x230), desc->callback);

		pr_err("ch_id=%d\n", chan->chan_id);

		for (i = 0; i <= 0x40; i += 4) {
			pr_info("0x%x: 0x%x\n", (0x0 + i), readl(sysdma_reg + i));
		}
		for (i = 0; i <= 0x40; i += 4) {
			pr_info("0x%x: 0x%x\n", (0x100 + i), readl(sysdma_reg + 0x100 + i));
		}
		for (i = 0; i <= 0x40; i += 4) {
			pr_info("0x%x: 0x%x\n", (0x200 + i), readl(sysdma_reg + 0x200 + i));
		}

		iounmap(clk_reg);
		iounmap(sysdma_reg);

		ret = -ETIMEDOUT;
	}

	dma_unmap_sg(chan->device->dev, &host->sgl, 1, DMA_BIDIRECTIONAL);

	pr_debug("<=%s\n", __func__);

	return ret;
}

static void spi_nand_set_read_from_cache_mode(struct cvsnfc_host *host, uint32_t mode, uint32_t r_col_addr)
{
	struct spi_nand_driver *spi_driver = host->spi_nand.driver;
	switch (mode) {
	case SPI_NAND_READ_FROM_CACHE_MODE_X1:
		cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, BIT_REG_TRX_DMA_EN |
				BIT_REG_TRX_DUMMY_HIZ);
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

		if (spi_driver->qe_enable)
			spi_driver->qe_enable(host);
		break;
	default:
		pr_err("unsupport mode!\n");
		break;
	}
}

static void spi_nand_read_eccsr(struct cvsnfc_host *host, uint8_t read_ecc_opcode, uint32_t *eccsr)
{
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 1 << TRX_DATA_SIZE_SHIFT | 1 << TRX_CMD_CONT_SIZE_SHIFT);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, read_ecc_opcode);
	cvsnfc_send_nondata_cmd_and_wait(host);

	*eccsr = cvsfc_read(host, REG_SPI_NAND_RX_DATA) & 0xff;
}

static int parse_status_info(struct cvsnfc_host *host)
{
	struct cvsnfc_chip_info *spi_dev = &host->spi_nand;
	struct nand_ecc_info *ecc_info = &spi_dev->ecc_info;
	uint8_t *id = spi_dev->nand_info.id;
	uint32_t ecc_status0, ecc_status1, status;
	uint32_t mask, sr_mask;
	uint32_t corr_bit = 0;

	if (!ecc_info->ecc_sr_addr && !ecc_info->read_ecc_opcode) {
		pr_err("can not get ecc status\n");
		return -1;
	}

	/* read SR */
	spi_feature_op(host, GET_OP, ecc_info->ecc_sr_addr, &ecc_status0);
	mask = GENMASK(ecc_info->ecc_bits - 1, 0);
	status = (ecc_status0 >> ecc_info->ecc_bit_shift) & mask;

	if (status == 0)
		return 0;

	if (status == ecc_info->uncorr_val)
		return -EBADMSG;

	if (ecc_info->ecc_sr_addr && !ecc_info->read_ecc_opcode && !ecc_info->ecc_mbf_addr) {
		if (ecc_info->remap) {
			corr_bit = ecc_info->remap[status] != 0xff ? ecc_info->remap[status] : 0;
		} else {
			pr_info("ECC CORR, unknown bitflip so we guess it has corrected at least 1 bit\n");
			corr_bit = 1;
		}
	}

	/* only for GD */
	if (ecc_info->ecc_mbf_addr && id[0] == 0xc8) {
		spi_feature_op(host, GET_OP, ecc_info->ecc_mbf_addr, &ecc_status1);
		status = ((ecc_status0 & 0x30) >> 2) | ((ecc_status1 & 0x30) >> 4);
		corr_bit = ecc_info->remap[status] != 0xff ? ecc_info->remap[status] : 0;
	}

	if (ecc_info->ecc_mbf_addr && id[0] != 0xc8) {
		spi_feature_op(host, GET_OP, ecc_info->ecc_mbf_addr, &ecc_status0);
		corr_bit = (ecc_status0 >> ecc_info->ecc_bit_shift) & mask;
	}

	if (ecc_info->read_ecc_opcode) {
		spi_nand_read_eccsr(host, ecc_info->read_ecc_opcode, &ecc_status0);
		corr_bit = (ecc_status0 >> ecc_info->ecc_bit_shift) & mask;
	}
	//pr_info("ECC CORR, correct bits %u\n", corr_bit);
	return corr_bit;
}

static int spi_nand_read_from_cache(struct cvsnfc_host *host, struct mtd_info *mtd,
		int col_addr, int len, void *buf)
{
	int r_col_addr = ((col_addr & 0xff00) >> 8) | ((col_addr & 0xff) << 8);
	int ret = 0;
	unsigned int max_bitflips = 0;
	int retry = 3;

RETRY_READ_CMD:

	pr_debug("%s caddr 0x%x, r_raddr 0x%x, len %d\n", __func__, col_addr, r_col_addr, len);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, len << TRX_DATA_SIZE_SHIFT | 3 << TRX_CMD_CONT_SIZE_SHIFT);

	spi_nand_set_read_from_cache_mode(host, SPI_NAND_READ_FROM_CACHE_MODE_X2, r_col_addr);

	cvsnfc_setup_intr(host);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0,
			cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	ret = spi_nand_rw_dma_setup(host, buf, len, 0);

	if (ret == 0) {
		struct cvsnfc_irq_status_t irq_mask, irq_status;

		irq_mask.status =
			BIT_REG_DMA_DONE_INT_CLR | BIT_REG_TRX_DONE_INT_CLR;
		irq_status.status = 0;

		wait_for_irq(host, &irq_mask, &irq_status);

		if (irq_status.status == 0) {
			u32 int_status = cvsfc_read(host, REG_SPI_NAND_INT);
			dev_err(host->dev, "%s command timeout 0x%x\n", __func__, int_status);
			ret = -ETIMEDOUT;
		}
	} else if (ret == -ETIMEDOUT) {
		if (--retry) {
			pr_err("retry read cmd\n");
			goto RETRY_READ_CMD;
		} else {
			pr_err("retry read failed\n");
		}
	} else {
		pr_err("rw dma setup error %d\n", ret);
	}

	ret = parse_status_info(host);
	return ret;
}

	__attribute__((unused))
static void cvsnfc_set_qe(struct cvsnfc_host *host, uint32_t enable)
{
	unsigned int status = 0;

	if (enable) {
		spi_feature_op(host, GET_OP, FEATURE_ADDR, &status);
		status |= SPI_NAND_FEATURE0_QE;
		spi_feature_op(host, SET_OP, FEATURE_ADDR, &status);
		spi_feature_op(host, GET_OP, FEATURE_ADDR, &status);
	} else {

		spi_feature_op(host, GET_OP, FEATURE_ADDR, &status);
		status &= ~SPI_NAND_FEATURE0_QE;
		spi_feature_op(host, SET_OP, FEATURE_ADDR, &status);
		spi_feature_op(host, GET_OP, FEATURE_ADDR, &status);
	}
}

static int cvsnfc_read_page(struct nand_chip *chip,
		uint8_t *buf, int bytes, int row_addr)
{
	int ret = 0;
	uint32_t col_addr = 0;

	struct cvsnfc_host *host = chip->priv;
	uint32_t blk_idx = row_addr / host->block_page_cnt;
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct cvsnfc_chip_info *spi_nand = &host->spi_nand;
	struct spi_nand_driver *spi_driver = spi_nand->driver;
	unsigned int die_id;

	pr_debug("=>%s, row_addr 0x%x blk_idx %d\n", __func__, row_addr, blk_idx);
	host->last_row_addr = row_addr;

	if (spi_driver->select_die) {
		die_id = row_addr / (host->diesize / host->pagesize);
		spi_driver->select_die(host, die_id);
	}

	spi_nand_send_read_page_cmd(host, row_addr);

	if (host->flags & FLAGS_SET_PLANE_BIT && (blk_idx & BIT(0))) {
		pr_debug("%s set plane bit for blkidx %d\n", __func__, blk_idx);
		col_addr |= SPI_NAND_PLANE_BIT_OFFSET;
	}

	ret = spi_nand_read_from_cache(host, mtd, col_addr, mtd->writesize, host->buforg);

	memcpy(buf, (void *)host->buforg, mtd->writesize);

	if (ret) {
		pr_debug("%s row_addr 0x%x ret %d\n", __func__, row_addr, ret);
	}

	return ret;
}

static int read_oob_data(struct mtd_info *mtd, uint8_t *buf, int row_addr)
{
	struct nand_chip *chip = mtd->priv;
	struct cvsnfc_host *host = chip->priv;
	uint32_t col_addr = 0;
	uint32_t blk_idx = row_addr / host->block_page_cnt;
	int ret = 0;
	struct spi_nand_driver *spi_driver = host->spi_nand.driver;

	pr_debug("%s, row_addr 0x%x\n", __func__, row_addr);

	host->last_row_addr = row_addr;

	if (spi_driver->select_die) {
		unsigned int die_id =
			row_addr / (host->diesize / host->pagesize);

		spi_driver->select_die(host, die_id);
	}

	cvsnfc_ctrl_ecc(mtd, DISABLE_ECC);

	spi_nand_send_read_page_cmd(host, row_addr);

	if (host->flags & FLAGS_SET_PLANE_BIT && (blk_idx & BIT(0))) {
		pr_debug("%s set plane bit for blkidx %d\n", __func__, blk_idx);
		col_addr |= SPI_NAND_PLANE_BIT_OFFSET;
	}

	ret = spi_nand_read_from_cache(host, mtd, mtd->writesize, mtd->oobsize, host->buforg);

	memcpy(buf, (void *)host->buforg, mtd->oobsize);

	cvsnfc_ctrl_ecc(mtd, ENABLE_ECC);

	if (ret) {
		pr_err("%s row_addr 0x%x ret %d\n", __func__, row_addr, ret);
	}

	return ret;
}

static int cvsnfc_read_oob(struct nand_chip *chip,
			     int page)
{
    struct mtd_info *mtd = nand_to_mtd(chip);
	return read_oob_data(mtd, chip->oob_poi, page);
}

static int cvsnfc_read_page_raw(struct nand_chip *chip,
							   uint8_t *buf, int oob_required, int page)
{
	return 0;
}

static int cvsnfc_read_subpage(struct nand_chip *chip,
			      u32 data_offs, u32 readlen, u8 *buf, int row_addr)
{
	struct cvsnfc_host *host = chip->priv;
	uint32_t col_addr = 0;
	uint32_t blk_idx = row_addr / host->block_page_cnt;
	int ret = 0;
	struct spi_nand_driver *spi_driver = host->spi_nand.driver;
	struct mtd_info *mtd = nand_to_mtd(chip);

	pr_debug("=>%s, row_addr 0x%x, data_offs %d, readlen %d, buf %p\n",
			__func__, row_addr, data_offs, readlen, buf);

	host->last_row_addr = row_addr;

	if (spi_driver->select_die) {
		unsigned int die_id = row_addr / (host->diesize / host->pagesize);

		spi_driver->select_die(host, die_id);
	}

	cvsnfc_ctrl_ecc(mtd, ENABLE_ECC);

	spi_nand_send_read_page_cmd(host, row_addr);

	if (host->flags & FLAGS_SET_PLANE_BIT && (blk_idx & BIT(0))) {
		pr_debug("%s set plane bit for blkidx %d\n", __func__, blk_idx);
		col_addr |= SPI_NAND_PLANE_BIT_OFFSET;
	}

	ret = spi_nand_read_from_cache(host, mtd, col_addr, mtd->writesize,
			host->buforg);

	memcpy(buf, (void *)PTR_INC(host->buforg, data_offs), readlen);

	if (ret) {
		pr_debug("%s row_addr 0x%x ret %d\n", __func__, row_addr, ret);
	}

	return ret;
}

static int spi_nand_prog_load(struct cvsnfc_host *host, const uint8_t *buf,
		size_t size, uint32_t col_addr, uint32_t qe)
{
	uint8_t cmd = qe ? SPI_NAND_CMD_PROGRAM_LOADX4 : SPI_NAND_CMD_PROGRAM_LOAD;
	uint32_t r_col_addr = ((col_addr & 0xff00) >> 8) | ((col_addr & 0xff) << 8);
	uint32_t ctrl3 = 0;
	void *data_buf = (void *) buf;
	int ret = 0;
	int retry = 3;
	struct spi_nand_driver *spi_driver = host->spi_nand.driver;

	if (qe && spi_driver->qe_enable)
		spi_driver->qe_enable(host);

RETRY_WRITE_CMD:

	pr_debug("=>%s size %u, col_addr 0x%x, r_col_addr 0x%x,  qe %d\n",
			__func__, (int) size, col_addr, r_col_addr, qe);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2,
			(size << TRX_DATA_SIZE_SHIFT) | 2 << TRX_CMD_CONT_SIZE_SHIFT);

	ctrl3 = qe ?
		(BIT_REG_TRX_RW | BIT_REG_TRX_DMA_EN | SPI_NAND_CTRL3_IO_TYPE_X4_MODE) :
		(BIT_REG_TRX_RW | BIT_REG_TRX_DMA_EN);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, ctrl3);

	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, cmd | (r_col_addr << TRX_CMD_CONT0_SHIFT));

	cvsnfc_setup_intr(host);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL0, cvsfc_read(host, REG_SPI_NAND_TRX_CTRL0) | BIT_REG_TRX_START);

	ret = spi_nand_rw_dma_setup(host, data_buf, size, 1);
	if (ret == 0) {
		struct cvsnfc_irq_status_t irq_mask, irq_status;

		irq_mask.status =
			BIT_REG_DMA_DONE_INT_CLR | BIT_REG_TRX_DONE_INT_CLR;
		irq_status.status = 0;

		wait_for_irq(host, &irq_mask, &irq_status);

		if (irq_status.status == 0) {
			u32 int_status = cvsfc_read(host, REG_SPI_NAND_INT);
			dev_err(host->dev, "%s command timeout 0x%x\n", __func__, int_status);
			ret = -ETIMEDOUT;
		}
	} else if (ret == -ETIMEDOUT) {
		if (--retry) {
			pr_err("retry prog load cmd\n");
			goto RETRY_WRITE_CMD;
		} else {
			pr_err("retry write failed\n");
		}
	} else {
		pr_err("rw dma setup error %d\n", ret);
	}

	pr_debug("<=%s\n", __func__);

	return ret;
}

static int spi_nand_prog_exec(struct cvsnfc_host *host, uint32_t row_addr)
{
	uint32_t r_row_addr = ((row_addr & 0xff0000) >> 16) | (row_addr & 0xff00) | ((row_addr & 0xff) << 16);

	pr_debug("=>%s\n", __func__);
	pr_debug("row_addr 0x%x\n", row_addr);
	pr_debug("r_row_addr 0x%x\n", r_row_addr);

	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL2, 3 << TRX_CMD_CONT_SIZE_SHIFT);
	cvsfc_write(host, REG_SPI_NAND_TRX_CTRL3, 0);
	cvsfc_write(host, REG_SPI_NAND_TRX_CMD0, SPI_NAND_CMD_PROGRAM_EXECUTE | (r_row_addr << TRX_CMD_CONT0_SHIFT));
	cvsfc_write(host, REG_SPI_NAND_RSP_POLLING, 0xff00ff);

	cvsnfc_send_nondata_cmd_and_wait(host);

	cvsnfc_dev_ready(&host->nand);

	pr_debug("<=%s\n", __func__);

	return 0;
}

static int write_page_helper(struct mtd_info *mtd, struct nand_chip *chip,
		const uint8_t *buf, int row_addr)
{
	struct cvsnfc_host *host = chip->priv;
	unsigned int val;
	uint32_t col_addr = 0;
	uint32_t blk_idx = row_addr / host->block_page_cnt;
	struct spi_nand_driver *spi_driver = host->spi_nand.driver;
	int ret = 0;

	pr_debug("=>%s, buf %p, page 0x%x ", __func__, buf, row_addr);

	host->last_row_addr = row_addr;

	if (spi_driver->select_die) {
		unsigned int die_id =
			row_addr / (host->diesize / host->pagesize);

		spi_driver->select_die(host, die_id);
	}

	val = spi_driver->wait_ready(host);
	if (val) {
		pr_err("cvsnfc: write wait ready fail! status[%#x]\n", val);
		return -1;
	}

	if (spi_driver->write_enable(host)) {
		pr_err("%s write enable failed!\n", __func__);
		return -1;
	}

	cvsnfc_ctrl_ecc(mtd, ENABLE_ECC);

	spi_feature_op(host, GET_OP, STATUS_ADDR, &val);

	pr_debug("Get feature addr[0xC0], val[%#x]\n", val);
	pr_debug("blk idx %d\n", blk_idx);

	if (host->flags & FLAGS_SET_PLANE_BIT && (blk_idx & BIT(0))) {
		pr_debug("%s set plane bit for blkidx %d\n", __func__, blk_idx);
		col_addr |= SPI_NAND_PLANE_BIT_OFFSET;
	}

	memcpy((void *)host->buforg, buf, mtd->writesize);
	ret = spi_nand_prog_load(host, host->buforg, host->pagesize, col_addr,
			0);
	if (ret) {
		return ret;
	}

	ret = spi_nand_prog_exec(host, row_addr);
	if (ret) {
		pr_err("%s spi_nand_prog_exec failed! %d\n", __func__, ret);
		return ret;
	}

	val = spi_driver->wait_ready(host);
	if (val & STATUS_E_FAIL_MASK)
		pr_err("cvsnfc: write failed! status[%#x]\n", val);

	pr_debug("<=\n");

	return ret;
}

static int cvsnfc_write_page(struct nand_chip *chip,
			    const uint8_t *buf, int oob_required, int row_addr)
{
	int status = 0;
    struct mtd_info *mtd = nand_to_mtd(chip);

	/*
	 * for regular page writes, we let HW handle all the ECC
	 * data written to the device.
	 */

	status = write_page_helper(mtd, chip, buf, row_addr);

	if (status) {
		pr_err("write error\n");
		return status;
	}

	return status;
}

static int cvsnfc_attach_chip(struct nand_chip *chip)
{
	//struct mtd_info *mtd = nand_to_mtd(chip);
	struct cvsnfc_host *host = nand_get_controller_data(chip);


	if (chip->ecc.engine_type == NAND_ECC_ENGINE_TYPE_ON_HOST)
		cvsnfc_ecc_probe(host);

	return 0;
}

static const struct nand_controller_ops cvsnfc_controller_ops = {
	.attach_chip = cvsnfc_attach_chip,
};
/*****************************************************************************/
void cvsnfc_nand_init(struct nand_chip *chip)
{
	struct cvsnfc_host *host = chip->priv;

	cvsnfc_setup_intr(host);

	chip->legacy.read_byte   = cvsnfc_read_byte;
	chip->legacy.write_buf   = cvsnfc_write_buf;
	chip->legacy.read_buf    = cvsnfc_read_buf;

	chip->legacy.select_chip = cvsnfc_select_chip;
	chip->legacy.cmdfunc = cvsnfc_cmdfunc;
	chip->legacy.waitfunc = cvsnfc_waitfunc;
	chip->legacy.cmd_ctrl    = cvsnfc_cmd_ctrl;
	chip->legacy.dev_ready   = cvsnfc_dev_ready;

	chip->legacy.chip_delay  = CVSNFC_CHIP_DELAY;

	chip->options     = NAND_BROKEN_XD;

	chip->bbt_options = NAND_BBT_USE_FLASH | NAND_BBT_NO_OOB;

	/* override the default read operations */
	chip->ecc.read_page = cvsnfc_read_page;
	chip->ecc.read_oob = cvsnfc_read_oob;
	chip->ecc.read_page_raw = cvsnfc_read_page_raw;
	chip->ecc.read_subpage = cvsnfc_read_subpage;
	chip->ecc.write_page = cvsnfc_write_page;

	chip->options |= NAND_NO_SUBPAGE_WRITE;

	chip->legacy.dummy_controller.ops = &cvsnfc_controller_ops;
}

static void cvsnfc_clear_interrupt(struct cvsnfc_host *host, struct cvsnfc_irq_status_t *irq_status)
{
	pr_debug("%s\n", __func__);

	writel(irq_status->status, host->regbase + REG_SPI_NAND_INT_CLR);
}

static inline uint32_t irq_detected(struct cvsnfc_host *host, struct cvsnfc_irq_status_t *irq_status)
{

	irq_status->status = readl(host->regbase + REG_SPI_NAND_INT);

	pr_debug("%s irq_status->status 0x%x\n", __func__, irq_status->status);

	return irq_status->status;
}

/*
 * This is the interrupt service routine. It handles all interrupts
 * sent to this device.
 */
static irqreturn_t cvsnfc_isr(int irq, void *dev_id)
{
	struct cvsnfc_host *host = dev_id;
	struct cvsnfc_irq_status_t irq_status;
	irqreturn_t result = IRQ_NONE;

	pr_debug("%s\n", __func__);

	spin_lock(&host->irq_lock);

	if (irq_detected(host, &irq_status)) {
		/* handle interrupt */
		/* first acknowledge it */
		cvsnfc_clear_interrupt(host, &irq_status);
		/* store the status in the device context for someone to read */
		host->irq_status.status |= irq_status.status;
		/* notify anyone who cares that it happened */
		complete(&host->complete);
		/* tell the OS that we've handled this */
		result = IRQ_HANDLED;
	}
	spin_unlock(&host->irq_lock);

	return result;
}

static int cvsnfc_dma_setup(struct cvsnfc_host *host)
{
	struct mtd_info *mtd = nand_to_mtd(&(host->nand));
	dma_cap_mask_t mask;

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);
	host->dma_chan_rx = dma_request_slave_channel(host->dev, "rx");
	host->dma_chan_tx = dma_request_slave_channel(host->dev, "tx");
	if (!host->dma_chan_rx) {
		dev_err(mtd->dev.parent, "Failed to request DMA rx channel\n");
		return -EBUSY;
	}

	if (!host->dma_chan_tx) {
		dev_err(mtd->dev.parent, "Failed to request DMA tx channel\n");
		return -EBUSY;
	}

	return 0;
}

/*****************************************************************************/
int cvsnfc_host_init(struct cvsnfc_host *host)
{
	int ret = 0;
	struct mtd_info *mtd = nand_to_mtd(&host->nand);

	if (host->set_system_clock)
		host->set_system_clock(NULL, ENABLE);

	host->buforg = devm_kzalloc(host->dev, CVSNFC_BUFFER_LEN, GFP_DMA | GFP_KERNEL);

	if (!host->buforg) {
		ret = -ENOMEM;
		goto err;
	}

	ret = dma_set_mask(host->dev, DMA_BIT_MASK(64));

	if (ret) {
		dev_err(host->dev, "no usable DMA configuration\n");
		ret = -ENOMEM;
		goto err;
	}

	host->dma_buffer =
		dma_map_single(host->dev, host->buforg, CVSNFC_BUFFER_LEN,
				DMA_BIDIRECTIONAL);
	if (dma_mapping_error(host->dev, host->dma_buffer)) {
		dev_err(host->dev, "Failed to map DMA buffer\n");
		ret = -EIO;
		goto err;
	}

	/*
	 * Allocate a large enough buffer for a single huge page plus
	 * extra space for the spare area and ECC storage area
	 */
	host->dma_buf_len = CVSNFC_BUFFER_LEN;
	host->data_buf = devm_kzalloc(host->dev, host->dma_buf_len,
			GFP_KERNEL);

	if (!host->data_buf) {
		ret = -ENOMEM;
		goto err;
	}

	cvsnfc_dma_setup(host);

	host->send_cmd_pageprog  = cvsnfc_send_cmd_pageprog;
	host->send_cmd_status    = cvsnfc_send_cmd_status;
	host->send_cmd_readstart = cvsnfc_send_cmd_readstart;
	host->send_cmd_erase     = cvsnfc_send_cmd_erase;
	host->send_cmd_readid    = cvsnfc_send_cmd_readid;
	host->send_cmd_reset     = cvsnfc_send_cmd_reset;

	mtd->dev.parent = host->dev;

	if (request_irq(host->irq, cvsnfc_isr, IRQF_SHARED,
				NAND_NAME, host)) {
		dev_err(host->dev, "Unable to allocate IRQ\n");
		return -ENODEV;
	}

	mtd->name = NAND_NAME;
	mtd->owner = THIS_MODULE;
	mtd->priv = &host->nand;

	spin_lock_init(&host->irq_lock);
	init_completion(&host->complete);
	return 0;
err:
	return ret;
}

/*****************************************************************************/
EXPORT_SYMBOL(cvsnfc_host_init);

static void cvsnfc_irq_cleanup(int irqnum, struct cvsnfc_host *host)
{
	free_irq(irqnum, host);
}

/* driver exit point */
void cvsnfc_remove(struct cvsnfc_host *host)
{
	cvsnfc_irq_cleanup(host->irq, host);
}
EXPORT_SYMBOL(cvsnfc_remove);
