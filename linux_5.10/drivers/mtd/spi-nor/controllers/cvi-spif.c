/*
 * SPI-NOR driver for Cvitek SPI Flash Controller
 *
 * Copyright (C) 2015 Joachim Eastwood <manabian@gmail.com>
 *
 * Based on Freescale QuadSPI driver:
 * Copyright (C) 2013 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/spi-nor.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/iopoll.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/mtd/spi-nor.h>

/* SPINOR DMMR read mode uses SYSDMA */
#define DMMR_DMA_MODE

/* Cvitek SPIF registers, bits and macros */
#define SPI_FLASH_BLOCK_SIZE             256
#define SPI_TRAN_CSR_ADDR_BYTES_SHIFT    8
#define SPI_MAX_FIFO_DEPTH               8

/* register definitions */
#define REG_SPI_CTRL                     0x000
#define REG_SPI_CE_CTRL                  0x004
#define REG_SPI_DLY_CTRL                 0x008
#define REG_SPI_DMMR                     0x00C
#define REG_SPI_TRAN_CSR                 0x010
#define REG_SPI_TRAN_NUM                 0x014
#define REG_SPI_FIFO_PORT                0x018
#define REG_SPI_FIFO_PT                  0x020
#define REG_SPI_INT_STS                  0x028
#define REG_SPI_INT_EN                   0x02C

/* bit definition */
#define BIT_SPI_CTRL_CPHA                    (0x01 << 12)
#define BIT_SPI_CTRL_CPOL                    (0x01 << 13)
#define BIT_SPI_CTRL_HOLD_OL                 (0x01 << 14)
#define BIT_SPI_CTRL_WP_OL                   (0x01 << 15)
#define BIT_SPI_CTRL_LSBF                    (0x01 << 20)
#define BIT_SPI_CTRL_SRST                    (0x01 << 21)
#define BIT_SPI_CTRL_SCK_DIV_SHIFT           0
#define BIT_SPI_CTRL_FRAME_LEN_SHIFT         16
#define BIT_SPI_CTRL_SCK_DIV_MASK            0x7FF

#define BIT_SPI_CE_CTRL_CEMANUAL             (0x01 << 0)
#define BIT_SPI_CE_CTRL_CEMANUAL_EN          (0x01 << 1)

#define BIT_SPI_CTRL_FM_INTVL_SHIFT          0
#define BIT_SPI_CTRL_CET_SHIFT               8

#define BIT_SPI_DMMR_EN                      (0x01 << 0)

#define BIT_SPI_TRAN_CSR_TRAN_MODE_RX        (0x01 << 0)
#define BIT_SPI_TRAN_CSR_TRAN_MODE_TX        (0x01 << 1)
#define BIT_SPI_TRAN_CSR_CNTNS_READ          (0x01 << 2)
#define BIT_SPI_TRAN_CSR_FAST_MODE           (0x01 << 3)
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_1_BIT     (0x0 << 4)
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_2_BIT     (0x01 << 4)
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_4_BIT     (0x02 << 4)
#define BIT_SPI_TRAN_CSR_DMA_EN              (0x01 << 6)
#define BIT_SPI_TRAN_CSR_MISO_LEVEL          (0x01 << 7)
#define BIT_SPI_TRAN_CSR_ADDR_BYTES_NO_ADDR  (0x0 << 8)
#define BIT_SPI_TRAN_CSR_WITH_CMD            (0x01 << 11)
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_1_BYTE (0x0 << 12)
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_2_BYTE (0x01 << 12)
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_4_BYTE (0x02 << 12)
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_8_BYTE (0x03 << 12)
#define BIT_SPI_TRAN_CSR_GO_BUSY             (0x01 << 15)
#define SPI_TRAN_CSR_DUMMY_MASK              (0xf << 16)

#define BIT_SPI_TRAN_CSR_TRAN_MODE_MASK      0x0003
#define BIT_SPI_TRAN_CSR_ADDR_BYTES_MASK     0x0700
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_MASK   0x3000
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_MASK     (0x3 << 4)

#define BIT_SPI_INT_TRAN_DONE                (0x01 << 0)
#define BIT_SPI_INT_RD_FIFO                  (0x01 << 2)
#define BIT_SPI_INT_WR_FIFO                  (0x01 << 3)
#define BIT_SPI_INT_RX_FRAME                 (0x01 << 4)
#define BIT_SPI_INT_TX_FRAME                 (0x01 << 5)

#define BIT_SPI_INT_TRAN_DONE_EN             (0x01 << 0)
#define BIT_SPI_INT_RD_FIFO_EN               (0x01 << 2)
#define BIT_SPI_INT_WR_FIFO_EN               (0x01 << 3)
#define BIT_SPI_INT_RX_FRAME_EN              (0x01 << 4)
#define BIT_SPI_INT_TX_FRAME_EN              (0x01 << 5)

#define SPI_NOR_MAX_ID_LEN	6
#define SPI_NOR_MAX_ADDR_WIDTH	4

/* SPI Flash Device Table */
struct flash_dev_info {
	char	*name;
	u32	jedec_id;
	u32	max_freq_div;
	u32	dmmr_setting;
};

struct cvi_spif {
	void __iomem *io_base;
	struct device *dev;
	struct platform_device	*pdev;
	u32 pending;
	struct spi_nor nor;
	struct completion cmd_completion;
	int irq;
	spinlock_t irq_lock;
	struct flash_dev_info *dev_info;

	bool memory_mode;
	u32 org_ce_ctrl;
	u32 org_tran_csr;

	dma_addr_t		dmmr_base_phy;
	struct completion	rx_dma_complete;
	struct dma_chan		*rx_chan;
	dma_cookie_t		cookie;
};

struct dmmr_reg_t {
	uint8_t read_cmd;
	uint8_t dummy_clock;
	uint32_t reg_set;
};

static unsigned int match_value_for_read(u8 opcode)
{
	int i;
	uint32_t val = 0;
	const struct dmmr_reg_t dmmr_reg_set[] = {
		{0x03, 0x0, 0x003B81},
		{0x0B, 0x8, 0x003B89},
		{0x3B, 0x8, 0x003B91},
		{0xBB, 0x4, 0x003B99},
		{0x6B, 0x8, 0x003BA1},
		{0xEB, 0x6, 0x003BA9},
		{0x13, 0x0, 0x303C81},
		{0x0C, 0x8, 0x303C89},
		{0x3C, 0x8, 0x303C91},
		{0xBC, 0x4, 0x303C99},
		{0x6C, 0x8, 0x303CA1},
		{0xEC, 0x6, 0x303CA9},
		{0x0, 0x0, 0x0}
	};

	for (i = 0; i < ARRAY_SIZE(dmmr_reg_set); i++) {
		if (opcode == dmmr_reg_set[i].read_cmd) {
			val = dmmr_reg_set[i].reg_set & (~SPI_TRAN_CSR_DUMMY_MASK);
			val |= (dmmr_reg_set[i].dummy_clock & 0xf) << 16;
			return val;
		}
	}
	return dmmr_reg_set[0].reg_set;
}

static int cvi_spif_read_reg(struct spi_nor *nor, u8 opcode, u8 *buf, size_t len);
static int cvi_spif_write_reg(struct spi_nor *nor, u8 opcode, const u8 *buf, size_t len);
static void cvi_spif_dump_reg(struct cvi_spif *spif);

static void cvi_spif_rx_dma_callback(void *param)
{
	struct cvi_spif *spif = param;

	pr_debug("%s\n", __func__);

	complete(&spif->rx_dma_complete);
}

static void cvi_spif_dma_setup(struct cvi_spif *spif)
{
	dma_cap_mask_t mask;
	int ret;

	dma_cap_zero(mask);
	dma_cap_set(DMA_MEMCPY, mask);
	ret = dma_set_mask(spif->dev, DMA_BIT_MASK(64));
	if (ret)
		dev_err(spif->dev, "no usable DMA configuration\n");

	spif->rx_chan = dma_request_chan_by_mask(&mask);
	if (IS_ERR(spif->rx_chan)) {
		dev_err(&spif->pdev->dev, "No Rx DMA available\n");
		spif->rx_chan = NULL;
	}
	init_completion(&spif->rx_dma_complete);
}


static void timeout_dump_reg(struct cvi_spif *spif, struct dma_chan *chan)
{
	void __iomem *clk_reg;
	void __iomem *sysdma_reg;
	uint32_t i = 0;

	cvi_spif_dump_reg(spif);

	clk_reg = ioremap(0x3002000, 0x10);
	sysdma_reg = ioremap(0x4330000, 0x800);

	pr_err("%s timeout, 0x3002000=0x%x, 0x3002004=0x%x, ch0 sar=0x%x ch1 dar=0x%x\n",
	       __func__, readl(clk_reg), readl(clk_reg+4), readl(sysdma_reg + 0x100),
	       readl(sysdma_reg + 0x208));

	pr_err("CFG=0x%x, CHEN=0x%x, ch0 status=0x%x, ch1 status=0x%x\n",
	       readl(sysdma_reg+0x10), readl(sysdma_reg + 0x18), readl(sysdma_reg + 0x130),
	       readl(sysdma_reg + 0x230));

	pr_err("ch_id=%d\n", chan->chan_id);

	for (i = 0; i <= 0x20; i += 4) {
		pr_info("0x%x: 0x%x\n", (0x0 + i), readl(sysdma_reg + i));
	}
	for (i = 0; i <= 0x20; i += 4) {
		pr_info("0x%x: 0x%x\n", (0x100 + i), readl(sysdma_reg + 0x100 + i));
	}
	for (i = 0; i <= 0x20; i += 4) {
		pr_info("0x%x: 0x%x\n", (0x200 + i), readl(sysdma_reg + 0x200 + i));
	}

	iounmap(clk_reg);
	iounmap(sysdma_reg);
}


#define DMA_MIN_THLD 1024

static int cvi_spif_direct_read_execute(struct spi_nor *nor, u_char *buf,
					loff_t from, size_t len)
{
	struct cvi_spif *spif = nor->priv;
	enum dma_ctrl_flags flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;
	dma_addr_t dma_src = (dma_addr_t)spif->dmmr_base_phy + from;
	int ret = 0;
	struct dma_async_tx_descriptor *tx;
	dma_cookie_t cookie;
	dma_addr_t dma_dst;

	pr_debug("%s from 0x%x, len 0x%x\n", __func__, (int)from, (int)len);

	if (!spif->rx_chan || !virt_addr_valid(buf)  || len <= DMA_MIN_THLD) {
		memcpy_fromio(buf, spif->io_base + from, len);
		pr_debug("cpy no dma\n");
		return 0;
	}

	dma_dst = dma_map_single(nor->dev, buf, len, DMA_FROM_DEVICE);
	if (dma_mapping_error(nor->dev, dma_dst)) {
		dev_err(nor->dev, "dma mapping failed\n");
		return -ENOMEM;
	}
	tx = dmaengine_prep_dma_memcpy(spif->rx_chan, dma_dst, dma_src,
				       len, flags);
	if (!tx) {
		dev_err(nor->dev, "device_prep_dma_memcpy error\n");
		ret = -EIO;
		goto err_unmap;
	}

	tx->callback = cvi_spif_rx_dma_callback;
	tx->callback_param = spif;
	cookie = tx->tx_submit(tx);
	reinit_completion(&spif->rx_dma_complete);

	ret = dma_submit_error(cookie);
	if (ret) {
		dev_err(nor->dev, "dma_submit_error %d\n", cookie);
		ret = -EIO;
		goto err_unmap;
	}

	dma_async_issue_pending(spif->rx_chan);

	if (!wait_for_completion_timeout(&spif->rx_dma_complete,
					 msecs_to_jiffies(len))) {

		pr_err("spinor dma timeout f 0x%x l 0x%x\n",
		       (int)from, (int)len);
		timeout_dump_reg(spif, spif->rx_chan);

		dmaengine_terminate_sync(spif->rx_chan);

		ret = -ETIMEDOUT;
	}

err_unmap:
	dma_unmap_single(nor->dev, dma_dst, len, DMA_FROM_DEVICE);

	return ret;
}

static int cvi_spif_wait_for_tran_done(struct cvi_spif *spif)
{
	u8 stat;
	int err = 0;

	/* wait tran done */
	err = readb_poll_timeout(spif->io_base + REG_SPI_INT_STS, stat,
				 (stat & BIT_SPI_INT_TRAN_DONE), 1, 300*1000);
	if (err)
		dev_warn(spif->dev, " %s command timed out\n", __func__);


	writeb(readb(spif->io_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_TRAN_DONE, spif->io_base + REG_SPI_INT_STS);
	writeb(readb(spif->io_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_RD_FIFO, spif->io_base + REG_SPI_INT_STS);

	return err;
}

static int cvi_spif_clk_setup(struct cvi_spif *spif, u32 sck_div)
{
	u32 reg, sck_div_backup;

	writel(0, spif->io_base + REG_SPI_DMMR);

	/* set positive edge sampling if SCK <= 30MHz */
	if (sck_div >= 4) {
		reg = readl(spif->io_base + REG_SPI_DLY_CTRL);
		reg &= ~BIT(14);
	} else { /* set negative edge sampling otherwise */
		reg = readl(spif->io_base + REG_SPI_DLY_CTRL);
		reg |= BIT(14);
	}
	writel(reg, spif->io_base + REG_SPI_DLY_CTRL);

	/* set spinor clock divider */
	reg = readl(spif->io_base + REG_SPI_CTRL);
	sck_div_backup = reg & BIT_SPI_CTRL_SCK_DIV_MASK;
	reg &= ~BIT_SPI_CTRL_SCK_DIV_MASK;
	reg |= sck_div;
	writel(reg, spif->io_base + REG_SPI_CTRL);

	return sck_div_backup;
}

static void cvi_spif_set_memory_mode_off(struct spi_nor *nor)
{
	struct cvi_spif *spif = nor->priv;

	writel(0, spif->io_base + REG_SPI_DMMR);
	writel(0x3, spif->io_base + REG_SPI_CE_CTRL);
	writel(spif->org_tran_csr, spif->io_base + REG_SPI_TRAN_CSR);
}

static void cvi_spif_set_memory_mode_on(struct spi_nor *nor)
{
	struct cvi_spif *spif = nor->priv;
	u32 reg = match_value_for_read(nor->read_opcode);

	writel(0, spif->io_base + REG_SPI_DMMR);
	spif->org_tran_csr = readl(spif->io_base + REG_SPI_TRAN_CSR);

	/* support only 1 ce */
	writel(0, spif->io_base + REG_SPI_CE_CTRL);
	if ((reg >> 16 & 0xf) != nor->read_dummy) {
		reg &= ~SPI_TRAN_CSR_DUMMY_MASK;
		reg |= (nor->read_dummy & 0xf) << 16;
	}
	writel(reg, spif->io_base + REG_SPI_TRAN_CSR);
	writel(1, spif->io_base + REG_SPI_DMMR);
}

static void cvi_spif_dump_reg(struct cvi_spif *spif)
{
	int i = 0;
	u32 dmmrreg = readl(spif->io_base + REG_SPI_TRAN_CSR);

	pr_info("%s spif->memory_mode %d, dmmr reg 0x%x, lr %p\n",
		__func__, spif->memory_mode, dmmrreg, __builtin_return_address(0));

	writel(0, spif->io_base + REG_SPI_DMMR);

	for (i = 0; i <= 0x28; i = i + 4) {
		pr_info("0x%x : 0x%x\n", i, readl(spif->io_base + i));
	}

	writel(1, spif->io_base + REG_SPI_DMMR);
}

static u8 cvi_spi_data_out_tran(struct spi_nor *nor, const u8 *src_buf,
				uint32_t data_bytes, unsigned char bus_width)
{
	struct cvi_spif *spif = nor->priv;
	uint32_t tran_csr = 0;
	uint32_t xfer_size, off;
	int i;
	int ret;
	u32 stat;

	pr_debug("src[0] 0x%x data_bytes %d\n", src_buf[0], data_bytes);

	if (data_bytes > 65535) {
		pr_err("data out overflow, should be less than 65535 bytes(%d)\n", data_bytes);
		return -1;
	}

	writel(0, spif->io_base + REG_SPI_INT_STS);
	writel(BIT_SPI_INT_TRAN_DONE, spif->io_base + REG_SPI_INT_EN);

	/* init tran_csr */
	tran_csr = readw(spif->io_base + REG_SPI_TRAN_CSR);
	tran_csr &= ~(BIT_SPI_TRAN_CSR_TRAN_MODE_MASK
		     | BIT_SPI_TRAN_CSR_ADDR_BYTES_MASK
		     | BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_MASK
		     | BIT_SPI_TRAN_CSR_BUS_WIDTH_MASK
		     | BIT_SPI_TRAN_CSR_WITH_CMD);
	tran_csr |= BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_8_BYTE;
	tran_csr |= BIT_SPI_TRAN_CSR_TRAN_MODE_TX;

	writel(0, spif->io_base + REG_SPI_FIFO_PT);

	/* issue tran */
	writew(data_bytes, spif->io_base + REG_SPI_TRAN_NUM);
	tran_csr |= BIT_SPI_TRAN_CSR_GO_BUSY;
	tran_csr |= (bus_width / 2) << 4;

	writew(tran_csr, spif->io_base + REG_SPI_TRAN_CSR);
	/* 300 ms */
	ret = readb_poll_timeout(spif->io_base + REG_SPI_INT_STS, stat,
				 (stat & BIT_SPI_INT_WR_FIFO), 1, 300 * 1000);
	if (ret) {
		dev_warn(spif->dev, "%s BIT_SPI_INT_WR_FIFO timed out\n", __func__);
		return ret;
	}

	/* fill data */
	off = 0;
	while (off < data_bytes) {
		if (data_bytes - off >= SPI_MAX_FIFO_DEPTH)
			xfer_size = SPI_MAX_FIFO_DEPTH;
		else
			xfer_size = data_bytes - off;

		ret = readb_poll_timeout(spif->io_base + REG_SPI_FIFO_PT, stat,
					 (stat & 0xF) == 0, 1, 300 * 1000); // 300ms

		if (ret) {
			dev_warn(spif->dev, "%s REG_SPI_FIFO_PT timed out\n", __func__);
			return ret;
		}

		/*
		 * odd thing, if we use writeb, the BIT_SPI_INT_WR_FIFO bit can't
		 * be cleared after transfer done. and BIT_SPI_INT_RD_FIFO bit will not
		 * be set even when REG_SPI_FIFO_PT shows non-zero value.
		 */
		for (i = 0; i < xfer_size; i++) {
			writeb(*(src_buf + off + i), spif->io_base + REG_SPI_FIFO_PORT);
		}

		off += xfer_size;
	}

	/* wait tran done */
	ret = cvi_spif_wait_for_tran_done(spif);
	if (ret)
		return ret;

	writel(0, spif->io_base + REG_SPI_FIFO_PT);

	/* clear interrupts */
	writeb(readb(spif->io_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_TRAN_DONE, spif->io_base + REG_SPI_INT_STS);
	writeb(readb(spif->io_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_WR_FIFO, spif->io_base + REG_SPI_INT_STS);

	pr_debug("%s exit\n", __func__);

	return 0;
}

static int cvi_spi_data_in_tran(struct spi_nor *nor, u8 *dst_buf,
				int data_bytes, unsigned char bus_width)
{
	struct cvi_spif *spif = nor->priv;
	uint32_t tran_csr = 0;
	int i, xfer_size, off;
	u32 stat;
	int ret;

	if (data_bytes > 65535) {
		pr_err("SPI data in overflow, should be less than 65535 bytes(%d)\n", data_bytes);
		return -1;
	}

	writel(0, spif->io_base + REG_SPI_INT_STS);
	writel(BIT_SPI_INT_TRAN_DONE, spif->io_base + REG_SPI_INT_EN);

	/* init tran_csr */
	tran_csr = readw(spif->io_base + REG_SPI_TRAN_CSR);
	tran_csr &= ~(BIT_SPI_TRAN_CSR_TRAN_MODE_MASK
			| BIT_SPI_TRAN_CSR_ADDR_BYTES_MASK
			| BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_MASK
			| BIT_SPI_TRAN_CSR_BUS_WIDTH_MASK
			| BIT_SPI_TRAN_CSR_WITH_CMD);
	tran_csr |= BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_8_BYTE;
	tran_csr |= BIT_SPI_TRAN_CSR_TRAN_MODE_RX;

	writel(0, spif->io_base + REG_SPI_FIFO_PT);

	/* issue tran */
	writew(data_bytes, spif->io_base + REG_SPI_TRAN_NUM);
	tran_csr |= BIT_SPI_TRAN_CSR_GO_BUSY;
	tran_csr |= (bus_width / 2) << 4;
	writew(tran_csr, spif->io_base + REG_SPI_TRAN_CSR);

	ret = readb_poll_timeout(spif->io_base + REG_SPI_INT_STS, stat,
				 stat & (BIT_SPI_INT_RD_FIFO | BIT_SPI_INT_TRAN_DONE), 1, 300 * 1000); // 300ms

	if (ret)
		dev_warn(spif->dev, "%s timedout2\n", __func__);

	/* get data */
	off = 0;
	while (off < data_bytes) {
		if (data_bytes - off >= SPI_MAX_FIFO_DEPTH)
			xfer_size = SPI_MAX_FIFO_DEPTH;
		else
			xfer_size = data_bytes - off;

		/*
		 * sometimes we get more than we want, why...
		 */
		ret = readb_poll_timeout(spif->io_base + REG_SPI_FIFO_PT, stat,
					 (stat & 0xF) >= xfer_size, 1, 300 * 1000); // 300ms

		if (ret)
			dev_warn(spif->dev, "%s timedout3\n", __func__);

		for (i = 0; i < xfer_size; i++)
			*(dst_buf + off + i) = readb(spif->io_base + REG_SPI_FIFO_PORT);

		off += xfer_size;
	}

	ret = cvi_spif_wait_for_tran_done(spif);
	if (ret)
		return ret;

	writeb(0, spif->io_base + REG_SPI_FIFO_PT); // flush unwanted data

	/* write 0 to clear interrupts */
	writeb(readb(spif->io_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_TRAN_DONE, spif->io_base + REG_SPI_INT_STS);
	writeb(readb(spif->io_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_RD_FIFO, spif->io_base + REG_SPI_INT_STS);
	return 0;
}

static int cvi_spif_read_reg(struct spi_nor *nor, u8 opcode, u8 *buf, size_t len)
{
	struct cvi_spif *spif = nor->priv;
	u8 bus_width;
	u32 sck_div_orig;

	/* always 1 bit bus width */
	bus_width = spi_nor_get_protocol_inst_nbits(nor->read_proto);

	pr_debug("%s opcode 0x%x\n", __func__, opcode);

	/* set clock to 30MHz for no-addr cmd */
	sck_div_orig = cvi_spif_clk_setup(spif, 4);

	writel(0, spif->io_base + REG_SPI_DMMR);
	writel(0x2, spif->io_base + REG_SPI_CE_CTRL);

	cvi_spi_data_out_tran(nor, &opcode, 1, bus_width);

	cvi_spi_data_in_tran(nor, buf, len, bus_width);

	writel(0x3, spif->io_base + REG_SPI_CE_CTRL);

	/* restore higher speed */
	cvi_spif_clk_setup(spif, sck_div_orig);

	return 0;
}

static int cvi_spif_write_reg(struct spi_nor *nor, u8 opcode, const u8 *buf, size_t len)
{
	struct cvi_spif *spif = nor->priv;
	u8 bus_width;
	u32 sck_div_orig;

	pr_debug("%s opcode 0x%x\n", __func__, opcode);

	/* always 1 bit bus width */
	bus_width = spi_nor_get_protocol_inst_nbits(nor->read_proto);

	/* set clock to 30MHz for no-addr cmd */
	sck_div_orig = cvi_spif_clk_setup(spif, 4);

	writel(0x2, spif->io_base + REG_SPI_CE_CTRL);

	cvi_spi_data_out_tran(nor, &opcode, 1, bus_width);

	if (len)
		cvi_spi_data_out_tran(nor, buf, len, bus_width);

	writel(0x3, spif->io_base + REG_SPI_CE_CTRL);

	/* restore higher speed */
	cvi_spif_clk_setup(spif, sck_div_orig);

	return 0;
}

static ssize_t cvi_spif_read(struct spi_nor *nor, loff_t from, size_t len,
			      u_char *buf)
{
#ifndef DMMR_DMA_MODE
	struct cvi_spif *spif = nor->priv;
#endif
	pr_debug("from 0x%llx, len %lu\n", from, len);

	cvi_spif_set_memory_mode_on(nor);

#ifdef DMMR_DMA_MODE
	cvi_spif_direct_read_execute(nor, buf, from, len);
#else
	memcpy_fromio(buf, spif->io_base + from, len);
#endif

	cvi_spif_set_memory_mode_off(nor);

	return len;
}

static void spi_flash_addr(struct spi_nor *nor, u32 addr, u8 *cmd)
{
	if (nor->addr_width == 4) {
		/* cmd[0] is actual command */
		cmd[1] = addr >> 24;
		cmd[2] = addr >> 16;
		cmd[3] = addr >> 8;
		cmd[4] = addr >> 0;

		pr_debug("cmd %2X%2X%2X%2X\n", cmd[1], cmd[2], cmd[3], cmd[4]);
	} else {
		/* cmd[0] is actual command */
		cmd[1] = addr >> 16;
		cmd[2] = addr >> 8;
		cmd[3] = addr >> 0;
		pr_debug("cmd %2X%2X%2X\n", cmd[1], cmd[2], cmd[3]);
	}
}

#define SPI_FLASH_3B_ADDR_LEN		3
#define SPI_FLASH_4B_ADDR_LEN		4
#define SPI_FLASH_BUF_LEN		(1 + SPI_FLASH_4B_ADDR_LEN)
#define SPI_FLASH_16MB_BOUN		0x1000000

static int cvi_spif_nor_write(struct cvi_spif *spif, struct spi_nor *nor,
		u8 opcode, unsigned int to, const u8 *buf,
		unsigned int len)
{

	u8 cmd[SPI_FLASH_BUF_LEN];
	u8 bus_width;

	pr_debug("%s opcode 0x%x\n", __func__, opcode);

	cmd[0] = opcode;

	spi_flash_addr(nor, to, cmd);
	/* cmd */
	bus_width = spi_nor_get_protocol_inst_nbits(nor->write_proto);
	cvi_spi_data_out_tran(nor, cmd, 1, bus_width);

	/* addr */
	bus_width = spi_nor_get_protocol_addr_nbits(nor->write_proto);
	cvi_spi_data_out_tran(nor, cmd + 1, nor->addr_width, bus_width);

	/* data */
	if (len) {
		bus_width = spi_nor_get_protocol_data_nbits(nor->write_proto);
		cvi_spi_data_out_tran(nor, buf, len, bus_width);
	}
	return 0;
}

static ssize_t cvi_spif_write(struct spi_nor *nor, loff_t to, size_t len,
			       const u_char *buf)
{

	struct cvi_spif *spif = nor->priv;

	pr_debug("to 0x%llx, len 0x%x\n", to, len);

	writel(0, spif->io_base + REG_SPI_DMMR);

	writel(0x2, spif->io_base + REG_SPI_CE_CTRL);

	cvi_spif_nor_write(spif, nor, nor->program_opcode, to, buf, len);

	writel(0x3, spif->io_base + REG_SPI_CE_CTRL);

	return len;
}

static const struct spi_nor_controller_ops cvi_controller_ops = {
	.read_reg = cvi_spif_read_reg,
	.write_reg = cvi_spif_write_reg,
	.read = cvi_spif_read,
	.write = cvi_spif_write,
};

static int cvi_spif_setup_flash(struct cvi_spif *spif,
				 struct device_node *np)
{
	struct spi_nor_hwcaps hwcaps = {
		.mask = SNOR_HWCAPS_READ	|
			SNOR_HWCAPS_READ_FAST	|
			SNOR_HWCAPS_READ_1_1_2	|
			SNOR_HWCAPS_READ_1_1_4	|
			SNOR_HWCAPS_READ_1_4_4	|
			SNOR_HWCAPS_PP		|
			SNOR_HWCAPS_PP_1_1_4,
	};
	int ret;

	spif->nor.dev   = spif->dev;
	spi_nor_set_flash_node(&spif->nor, np);
	spif->nor.priv  = spif;
	spif->nor.controller_ops = &cvi_controller_ops;

	ret = spi_nor_scan(&spif->nor, NULL, &hwcaps);
	if (ret) {
		dev_err(spif->dev, "device scan failed\n");
		return ret;
	}

	ret = mtd_device_register(&spif->nor.mtd, NULL, 0);
	if (ret) {
		dev_err(spif->dev, "mtd device parse failed\n");
		return ret;
	}

	return 0;
}

static int cvi_spif_probe(struct platform_device *pdev)
{
	struct device_node *flash_np;
	struct cvi_spif *spif;
	struct resource *res;
	int ret = 0;

	spif = devm_kzalloc(&pdev->dev, sizeof(*spif), GFP_KERNEL);
	if (!spif)
		return -ENOMEM;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "spif");
	spif->io_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(spif->io_base)) {
		dev_err(&pdev->dev, "Cannot remap AHB address.\n");
		return PTR_ERR(spif->io_base);
	}

	spif->dmmr_base_phy = (dma_addr_t)res->start;

	spif->dev = &pdev->dev;
	spif->pdev = pdev;
	platform_set_drvdata(pdev, spif);

	init_completion(&spif->cmd_completion);

	/* Request the IRQ */
	spif->irq = platform_get_irq(pdev, 0);

	if (spif->irq < 0) {
		dev_err(&pdev->dev, "missing IRQ\n");
		ret = spif->irq;
		goto bailout;
	}

	flash_np = of_get_next_available_child(pdev->dev.of_node, NULL);
	if (!flash_np) {
		dev_err(&pdev->dev, "no SPI flash device to configure\n");
		return -ENODEV;
	}

	ret = cvi_spif_setup_flash(spif, flash_np);
	if (ret) {
		dev_err(&pdev->dev, "unable to setup flash chip\n");
		return -ENODEV;
	}

	/* Ignore init clock, u-boot has init it already */
	// cvi_spif_clk_setup(spif, 1);
	cvi_spif_dma_setup(spif);

bailout:
	return ret;
}

static int cvi_spif_remove(struct platform_device *pdev)
{
	struct cvi_spif *spif = platform_get_drvdata(pdev);

	pr_info("%s\n", __func__);

	mtd_device_unregister(&spif->nor.mtd);

	return 0;
}

static void cvi_spif_shutdown(struct platform_device *pdev)
{
	struct cvi_spif *spif = platform_get_drvdata(pdev);

	if (spif == NULL)
		return;

	spi_nor_restore(&spif->nor);
}

static const struct of_device_id cvi_spif_match[] = {
	{.compatible = "cvitek,cvi-spif"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, cvi_spif_match);

static struct platform_driver cvi_spif_driver = {
	.probe	= cvi_spif_probe,
	.remove	= cvi_spif_remove,
	.shutdown = cvi_spif_shutdown,
	.driver	= {
		.name = "cvi-spif",
		.of_match_table = cvi_spif_match,
	},
};
module_platform_driver(cvi_spif_driver);

MODULE_DESCRIPTION("Cvitek SPI Flash Interface driver");
MODULE_LICENSE("GPL v2");
