/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: cvi_spif.c
 *
 * Description: Cvitek SPI NOR flash driver
 */

#include <common.h>
#include <spi.h>
#include <asm/io.h>
#include <spi_flash.h>
#include "cvi_spif.h"
#include <spi-mem.h>
#include <dm.h>
#include <linux/delay.h>
#include <mmio.h>
#include <linux/mtd/spi-nor.h>

// #define DEBUG

static int cvitek_spi_probe(struct udevice *bus)
{
	struct cvitek_spi_priv *priv = dev_get_priv(bus);

	priv->ctrl_base = (typeof(priv->ctrl_base))dev_read_addr_ptr(bus);
	priv->sck = dev_read_u32_default(bus, "sck_mhz", 50) * CLK_1MHZ;
	priv->max_hz = dev_read_u32_default(bus, "spi-max-frequency", 0);

	if (priv->ctrl_base == 0)
		return -EINVAL;
	return 0;
}

static unsigned int match_value_for_read(u8 read_cmd)
{
	int i;
	uint32_t val;

	for (i = 0; i < sizeof(dmmr_reg_set); i++) {
		if (read_cmd == dmmr_reg_set[i].read_cmd) {
			val = dmmr_reg_set[i].reg_set;
			val &= ~(0xf << 16);
			val |= ((dmmr_reg_set[i].dummy_clock & 0xf) << 16);
			return val;
		}
	}
	return dmmr_reg_set[0].reg_set;
}

static void cvitek_set_sf_clk(unsigned long spi_base, uint32_t sck_div)
{
	uint32_t ctrl;

	/* disable DMMR */
	mmio_write_8(spi_base + REG_SPI_DMMR, 0);

	ctrl = mmio_read_32(spi_base + REG_SPI_CTRL);
	ctrl &= ~BIT_SPI_CTRL_SCK_DIV_MASK;
	ctrl |= sck_div;
	mmio_write_32(spi_base + REG_SPI_CTRL, ctrl);

	if (sck_div >= SPI_CLK_30M) /* if sck <= 30MHz */
		mmio_write_16(spi_base + REG_SPI_DLY_CTRL, BIT_SPI_DLY_CTRL_CET);
	else /* if sck > 30MHz */
		mmio_write_16(spi_base + REG_SPI_DLY_CTRL, BIT_SPI_DLY_CTRL_CET | BIT_SPI_DLY_CTRL_NEG_SAMPLE);
}

static void cvitek_enable_mmap(struct cvitek_spi_priv *priv, u8 read_cmd, u8 dummy_clk)
{
	uint32_t val;

	/* disable DMMR */
	mmio_write_32(priv->ctrl_base + REG_SPI_DMMR, 0);

	/* backup register settings */
	priv->orig_tran_csr = mmio_read_32(priv->ctrl_base + REG_SPI_TRAN_CSR);

	/* CE control by HW */
	mmio_write_32(priv->ctrl_base + REG_SPI_CE_CTRL, 0);

	/* set TRANS_CSR according to read cmd */
	val = match_value_for_read(read_cmd);
	if (dummy_clk) {
		val &= ~(0xf << 16);
		val |= ((dummy_clk & 0xf) << 16);
	}
	mmio_write_32(priv->ctrl_base + REG_SPI_TRAN_CSR, val);

	/* set spinor clock */
	cvitek_set_sf_clk(priv->ctrl_base, priv->sck_div);

	/* enable DMMR */
	mmio_write_32(priv->ctrl_base + REG_SPI_DMMR, 1);
}

static void cvitek_disable_mmap(struct cvitek_spi_priv *priv)
{
	/* restore register settings */
	mmio_write_32(priv->ctrl_base + REG_SPI_CE_CTRL, 0x3);
	mmio_write_32(priv->ctrl_base + REG_SPI_TRAN_CSR, priv->orig_tran_csr);
}

static uint8_t cvitek_spi_data_out_tran(struct cvitek_spi_priv *priv, struct spi_flash *flash,
					unsigned long spi_base, const uint8_t *src_buf,
					uint32_t data_bytes, unsigned long flags)
{
	uint32_t tran_csr = 0;
	uint32_t xfer_size, off;
	uint32_t wait = 0;
	int i;
	struct spi_slave *spi = flash->spi;

	debug("cvitek_spi_data_out_tran(): flags 0x%lx, opcode 0x%x, spi->mode 0x%x\n",
	      flags, flash->program_opcode, spi->mode);

	if (data_bytes > 65535) {
		printf("data out overflow, should be less than 65535 bytes(%d)\n", data_bytes);
		return -1;
	}

	cvitek_set_sf_clk(priv->ctrl_base, priv->sck_div);

	/* init TRANS_CSR (offset: 0x10) */
	tran_csr = mmio_read_16(spi_base + REG_SPI_TRAN_CSR);
	tran_csr &= ~(BIT_SPI_TRAN_CSR_TRAN_MODE_MASK
		| BIT_SPI_TRAN_CSR_BUS_WIDTH_MASK
		| BIT_SPI_TRAN_CSR_ADDR_BYTES_MASK
		| BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_MASK
		| BIT_SPI_TRAN_CSR_WITH_CMD);
	tran_csr |= BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_8_BYTE;
	tran_csr |= BIT_SPI_TRAN_CSR_TRAN_MODE_TX;

	if (flags & SPI_XFER_USER_DATA && !(flags & SPI_XFER_BEGIN)) {
		if (spi->mode & SPI_TX_QUAD)
			tran_csr |= BIT_SPI_TRAN_CSR_BUS_WIDTH_4_BIT;
		else
			tran_csr |= BIT_SPI_TRAN_CSR_BUS_WIDTH_1_BIT;
	}

	mmio_write_32(spi_base + REG_SPI_FIFO_PT, 0);

	/* issue tran */
	mmio_write_16(spi_base + REG_SPI_TRAN_NUM, data_bytes);
	tran_csr |= BIT_SPI_TRAN_CSR_GO_BUSY;

	debug("cvitek_spi_data_out_tran tran_csr 0x%x\n", tran_csr);

	mmio_write_16(spi_base + REG_SPI_TRAN_CSR, tran_csr);

	while ((mmio_read_8(spi_base + REG_SPI_INT_STS) & BIT_SPI_INT_WR_FIFO) == 0)
		;

	/* fill data */
	off = 0;
	while (off < data_bytes) {
		if (data_bytes - off >= SPI_MAX_FIFO_DEPTH)
			xfer_size = SPI_MAX_FIFO_DEPTH;
		else
			xfer_size = data_bytes - off;

		wait = 0;
		while ((mmio_read_8(spi_base + REG_SPI_FIFO_PT) & 0xF) != 0) {
			wait++;
			udelay(1);
			if (wait > 3000) { // 3ms
				printf("wait to write FIFO timeout\n");
				return -1;
			}
		}

		/*
		 * odd thing, if we use mmio_write_8, the BIT_SPI_INT_WR_FIFO bit can't
		 * be cleared after transfer done. and BIT_SPI_INT_RD_FIFO bit will not
		 * be set even when REG_SPI_FIFO_PT shows non-zero value.
		 */
		for (i = 0; i < xfer_size; i++)
			mmio_write_8(spi_base + REG_SPI_FIFO_PORT, *(src_buf + off + i));

		off += xfer_size;
	}

	/* wait tran done */
	while ((mmio_read_8(spi_base + REG_SPI_INT_STS) & BIT_SPI_INT_TRAN_DONE) == 0)
		;
	mmio_write_32(spi_base + REG_SPI_FIFO_PT, 0);

	/* clear interrupts */
	mmio_write_8(spi_base + REG_SPI_INT_STS, mmio_read_8(spi_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_TRAN_DONE);
	mmio_write_8(spi_base + REG_SPI_INT_STS, mmio_read_8(spi_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_WR_FIFO);
	return 0;
}

static int cvitek_spi_data_in_tran(struct cvitek_spi_priv *priv, struct spi_flash *flash,
				   unsigned long spi_base, uint8_t *dst_buf, int data_bytes,
				   unsigned long flags)
{
	uint32_t i, xfer_size, off, tran_csr = 0;

	/* disable DMMR (offset: 0xC) */
	mmio_write_32(priv->ctrl_base + REG_SPI_DMMR, 0);

	/* init TRANS_CSR (offset: 0x10) */
	tran_csr = mmio_read_16(spi_base + REG_SPI_TRAN_CSR);
	tran_csr &= ~(BIT_SPI_TRAN_CSR_TRAN_MODE_MASK
			| BIT_SPI_TRAN_CSR_ADDR_BYTES_MASK
			| BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_MASK
			| BIT_SPI_TRAN_CSR_WITH_CMD);
	tran_csr |= BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_8_BYTE;
	tran_csr |= BIT_SPI_TRAN_CSR_TRAN_MODE_RX;

	/* Lower down spi clock with no-addr cmd */
	if (flags & SPI_XFER_CMD_DATA)
		cvitek_set_sf_clk(priv->ctrl_base, SPI_CLK_30M);
	else
		cvitek_set_sf_clk(priv->ctrl_base, priv->sck_div);

	/* flush FIFO after tran */
	mmio_write_32(spi_base + REG_SPI_FIFO_PT, 0);

	/* issue tran */
	mmio_write_16(spi_base + REG_SPI_TRAN_NUM, data_bytes);
	tran_csr |= BIT_SPI_TRAN_CSR_GO_BUSY;
	mmio_write_16(spi_base + REG_SPI_TRAN_CSR, tran_csr);

	while ((mmio_read_8(spi_base + REG_SPI_INT_STS) & BIT_SPI_INT_RD_FIFO) == 0 &&
	       (mmio_read_8(spi_base + REG_SPI_INT_STS) & BIT_SPI_INT_TRAN_DONE) == 0)
		;

	/* get data */
	off = 0;
	while (off < data_bytes) {
		if (data_bytes - off >= SPI_MAX_FIFO_DEPTH)
			xfer_size = SPI_MAX_FIFO_DEPTH;
		else
			xfer_size = data_bytes - off;

		while ((mmio_read_8(spi_base + REG_SPI_FIFO_PT) & 0xF) < xfer_size)
			;
		for (i = 0; i < xfer_size; i++)
			*(dst_buf + off + i) = mmio_read_8(spi_base + REG_SPI_FIFO_PORT);

		off += xfer_size;
	}

	/* wait tran done */
	while ((mmio_read_8(spi_base + REG_SPI_INT_STS) & BIT_SPI_INT_TRAN_DONE) == 0)
		;
	/* flush FIFO after tran */
	mmio_write_8(spi_base + REG_SPI_FIFO_PT, 0);

	/* write 0 to clear interrupts */
	mmio_write_8(spi_base + REG_SPI_INT_STS, mmio_read_8(spi_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_TRAN_DONE);
	mmio_write_8(spi_base + REG_SPI_INT_STS, mmio_read_8(spi_base + REG_SPI_INT_STS) & ~BIT_SPI_INT_RD_FIFO);

	return 0;
}

void dump_mem(const void *start_addr, int len)
{
	unsigned int *curr_p = (unsigned int *)start_addr;
	unsigned char *curr_ch_p;
	int _16_fix_num = len / 16;
	int tail_num = len % 16;
	char buf[16];
	int i, j;

	if (!curr_p || len == 0) {
		printf("nothing to dump!\n");
		return;
	}

	printf("Base: %p\n", start_addr);
	// Fix section
	for (i = 0; i < _16_fix_num; i++) {
		printf("%03X: %08X %08X %08X %08X\n",
		       i * 16, *curr_p, *(curr_p + 1), *(curr_p + 2), *(curr_p + 3));
		curr_p += 4;
	}

	// Tail section
	if (tail_num > 0) {
		curr_ch_p = (unsigned char *)curr_p;
		for (j = 0; j < tail_num; j++) {
			buf[j] = *curr_ch_p;
			curr_ch_p++;
		}
		for (; j < 16; j++)
			buf[j] = 0;
		curr_p = (unsigned int *)buf;
		printf("%03X: %08X %08X %08X %08X\n",
		       i * 16, *curr_p, *(curr_p + 1), *(curr_p + 2), *(curr_p + 3));
	}
}

static int cvitek_spi_xfer(struct udevice *dev, unsigned int bitlen,
			   const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct cvitek_spi_priv *priv = dev_get_priv(bus);
	struct spi_flash *flash = dev_get_uclass_priv(dev);

	/* assume spi core configured to do 8 bit transfers */
	unsigned int bytes = bitlen / 8;

	debug("bitlen:%d flags:%lx dir:%s\n", bitlen, flags, (dout) ? "out" : "in");

	if (flags & SPI_XFER_MMAP) {
		debug("opcode:%#x, dummy:%u\n", flash->read_opcode, flash->read_dummy);
		cvitek_enable_mmap(priv, flash->read_opcode, flash->read_dummy);
		return 0;
	} else if (flags & SPI_XFER_MMAP_END) {
		cvitek_disable_mmap(priv);
		return 0;
	}

	if (bitlen == 0)
		goto done;

	if (bitlen % 8) {
		printf("xfer bit length not a multiple of 8 bits\n");
		flags |= SPI_XFER_END;
		goto done;
	}

	if (flags & SPI_XFER_BEGIN)
		mmio_write_8(priv->ctrl_base + REG_SPI_CE_CTRL, 0x2);

	if (dout) {
#ifdef DEBUG
		dump_mem(dout, bytes);
#endif
		cvitek_spi_data_out_tran(priv, flash, priv->ctrl_base, dout, bytes, flags);
	}
	if (din) {
		cvitek_spi_data_in_tran(priv, flash, priv->ctrl_base, din, bytes, flags);

#ifdef DEBUG
		dump_mem(din, (bytes > 256) ? 256 : bytes);
#endif
	}

done:
	if (flags & SPI_XFER_END)
		mmio_write_8(priv->ctrl_base + REG_SPI_CE_CTRL, 0x3);
	return 0;
}

static int cvitek_spi_set_speed(struct udevice *bus, uint speed)
{
	int tmp;

	struct cvitek_spi_priv *priv = dev_get_priv(bus);

	priv->freq = priv->max_hz;	/* don't care input speed */
	tmp = priv->sck / (priv->freq * 2) - 1;

	if (tmp > 0)
		priv->sck_div = priv->sck / (priv->freq * 2) - 1;
	else
		priv->sck_div = SPI_CLK_30M;	/* default value, 30MHz */


	debug("%s: %d Hz, sck_div: %d\n", __func__, priv->freq, priv->sck_div);

	cvitek_set_sf_clk(priv->ctrl_base, priv->sck_div);

	return 0;
}

static int cvitek_spi_set_mode(struct udevice *bus, uint mode)
{
	struct cvitek_spi_priv *priv = dev_get_priv(bus);
	uint32_t ctrl;

	mode = mode & 0x3;
	if (mode == 1 || mode == 2)
		mode = 0;

	debug("[%s %d]===> set spi nor mode:%u\n", __func__, __LINE__, mode);
	ctrl = mmio_read_32(priv->ctrl_base + REG_SPI_CTRL);
	ctrl &= ~(BIT_SPI_CTRL_CPHA | BIT_SPI_CTRL_CPOL);
	ctrl |= mode << 12;
	mmio_write_32(priv->ctrl_base + REG_SPI_CTRL, ctrl);
	priv->mode = mode;

	return 0;
}

#if 0
static bool cvitek_spi_supports_op(struct spi_slave *slave,
				 const struct spi_mem_op *op)
{
	debug("%s\n", __func__);
	return true;
}

static int cvitek_spi_adjust_op_size(struct spi_slave *slave,
				   struct spi_mem_op *op)
{
	debug("%s\n", __func__);
	return 0;
}
#endif

static int cvitek_spi_exec_op(struct spi_slave *slave,
			    const struct spi_mem_op *op)
{
	int ret = 0;
	struct udevice *bus = slave->dev->parent;
	struct cvitek_spi_priv *priv = dev_get_priv(bus);

	/* Only optimize read path. */
	if (!op->data.nbytes || op->data.dir != SPI_MEM_DATA_IN ||
	    !op->addr.nbytes || op->addr.nbytes > 4) {
		if (!op->addr.nbytes)
			cvitek_set_sf_clk(priv->ctrl_base, SPI_CLK_30M);
		else
			cvitek_set_sf_clk(priv->ctrl_base, priv->sck_div);

		return -ENOTSUPP;
	}

	ret = spi_xfer(slave, 0, NULL, NULL, SPI_XFER_MMAP);
	memcpy(op->data.buf.in, (void *)(priv->ctrl_base + op->addr.val), op->data.nbytes);
	ret = spi_xfer(slave, 0, NULL, NULL, SPI_XFER_MMAP_END);
	return ret;
}

static const struct spi_controller_mem_ops cvitek_spi_mem_ops = {
	// .supports_op = cvitek_spi_supports_op,
	.exec_op = cvitek_spi_exec_op,
	// .adjust_op_size = cvitek_spi_adjust_op_size,
};

static const struct dm_spi_ops cvitek_spi_ops = {
	.xfer		= cvitek_spi_xfer,
	.mem_ops	= &cvitek_spi_mem_ops,
	.set_speed	= cvitek_spi_set_speed,
	.set_mode	= cvitek_spi_set_mode,
};

static const struct udevice_id cvitek_spi_ids[] = {
	{ .compatible = "cvitek,cvi-spif" },
	{ }
};

U_BOOT_DRIVER(cvitek_spi) = {
	.name	= "cvitek_spi",
	.id	= UCLASS_SPI,
	.of_match = cvitek_spi_ids,
	.ops	= &cvitek_spi_ops,
	.priv_auto	= sizeof(struct cvitek_spi_priv),
	.probe	= cvitek_spi_probe,
};
