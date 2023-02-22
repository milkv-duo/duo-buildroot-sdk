/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: cvi_spif.h
 *
 * Description: Cvitek SPI NOR flash driver header
 */

#ifndef __CVI_SPIF_H__
#define __CVI_SPIF_H__

/* spi register definitions */
#define REG_SPI_CTRL			0x00
#define REG_SPI_CE_CTRL			0x04
#define REG_SPI_DLY_CTRL		0x08
#define REG_SPI_DMMR			0x0C
#define REG_SPI_TRAN_CSR		0x10
#define REG_SPI_TRAN_NUM		0x14
#define REG_SPI_FIFO_PORT		0x18
#define REG_SPI_FIFO_PT			0x20
#define REG_SPI_INT_STS			0x28
#define REG_SPI_INT_EN			0x2C
#define REG_SPI_OPT			0x30

/* spi-nor commands */
#define SPI_CMD_WREN            0x06
#define SPI_CMD_WRDI            0x04
#define SPI_CMD_RDID            0x9F
#define SPI_CMD_RDSR            0x05
#define SPI_CMD_WRSR            0x01
#define SPI_CMD_READ            0x03
#define SPI_CMD_FAST_READ       0x0B
#define SPI_CMD_PP              0x02
#define SPI_CMD_SE              0x20
#define SPI_CMD_BE              0xD8
#define SPI_CMD_CE              0xC7

/* spi-nor status register */
#define SPI_STATUS_WIP          BIT(0)
#define SPI_STATUS_WEL          BIT(1)
#define SPI_STATUS_BP0          BIT(2)
#define SPI_STATUS_BP1          BIT(3)
#define SPI_STATUS_BP2          BIT(4)
#define SPI_STATUS_BP3          BIT(5)
#define SPI_STATUS_TP           BIT(6)
#define SPI_STATUS_SRWD         BIT(7)

/* register bit definition */
#define BIT_SPI_CTRL_CPHA		BIT(12)
#define BIT_SPI_CTRL_CPOL		BIT(13)
#define BIT_SPI_CTRL_HOLD_OL	BIT(14)
#define BIT_SPI_CTRL_WP_OL		BIT(15)
#define BIT_SPI_CTRL_FL			BIT(16)
#define BIT_SPI_CTRL_LSBF		BIT(20)
#define BIT_SPI_CTRL_SRST		BIT(21)
#define BIT_SPI_CTRL_SCK_DIV_SHIFT		0
#define BIT_SPI_CTRL_FRAME_LEN_SHIFT	16
#define BIT_SPI_CTRL_SCK_DIV_MASK		0x7FF // [10:0]
#define BIT_SPI_CE_CTRL_CEMANUAL		BIT(0)
#define BIT_SPI_CE_CTRL_CEMANUAL_EN		BIT(1)
#define BIT_SPI_DMMR_EN                 BIT(0)

#define BIT_SPI_TRAN_CSR_TRAN_MODE_RX				BIT(0)
#define BIT_SPI_TRAN_CSR_TRAN_MODE_TX				BIT(1)
#define BIT_SPI_TRAN_CSR_CNTNS_READ				BIT(2)
#define BIT_SPI_TRAN_CSR_FAST_MODE				BIT(3)
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_1_BIT			0x0
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_2_BIT			0x10
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_4_BIT			0x20
#define BIT_SPI_TRAN_CSR_DMA_EN					BIT(6)
#define BIT_SPI_TRAN_CSR_MISO_LEVEL				BIT(7)
#define BIT_SPI_TRAN_CSR_ADDR_BYTES_NO_ADDR			0
#define BIT_SPI_TRAN_CSR_WITH_CMD				BIT(11)
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_1_BYTE			0x0
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_2_BYTE			0x1000
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_4_BYTE			0x2000
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_8_BYTE			0x3000
#define BIT_SPI_TRAN_CSR_GO_BUSY				BIT(15)
#define BIT_SPI_TRAN_CSR_TRAN_MODE_MASK				0x3
#define BIT_SPI_TRAN_CSR_BUS_WIDTH_MASK				0x30
#define BIT_SPI_TRAN_CSR_ADDR_BYTES_MASK			0x700
#define BIT_SPI_TRAN_CSR_FIFO_TRG_LVL_MASK			0x3000
#define BIT_SPI_TRAN_CSR_DUMMY_MASK				0xF0000
#define BIT_SPI_TRAN_CSR_4BADDR_MASK				BIT(20)
#define BIT_SPI_TRAN_CSR_4BCMD_MASK				BIT(21)
#define BIT_SPI_DLY_CTRL_CET					(3 << 8)
#define BIT_SPI_DLY_CTRL_NEG_SAMPLE				BIT(14)

#define BIT_SPI_INT_TRAN_DONE               BIT(0)
#define BIT_SPI_INT_RD_FIFO                 BIT(2)
#define BIT_SPI_INT_WR_FIFO                 BIT(3)
#define BIT_SPI_INT_RX_FRAME                BIT(4)
#define BIT_SPI_INT_TX_FRAME                BIT(5)

#define BIT_SPI_INT_TRAN_DONE_EN            BIT(0)
#define BIT_SPI_INT_RD_FIFO_EN              BIT(2)
#define BIT_SPI_INT_WR_FIFO_EN              BIT(3)
#define BIT_SPI_INT_RX_FRAME_EN             BIT(4)
#define BIT_SPI_INT_TX_FRAME_EN             BIT(5)

/* general definition */
#define SPI_FLASH_BLOCK_SIZE             256
#define SPI_TRAN_CSR_ADDR_BYTES_SHIFT    8
#define SPI_MAX_FIFO_DEPTH               8
#define SPI_CLK_75M             1
#define SPI_CLK_50M             2
#define SPI_CLK_37M             3
#define SPI_CLK_30M             4
#define SPI_CLK_15M             9
#define CLK_1MHZ				1000000

/* struct */
struct cvi_spif_regs {
	unsigned int spi_ctrl;	/* 0x00 */
	unsigned int ce_ctrl;	/* 0x04 */
	unsigned int dly_ctrl;	/* 0x08 */
	unsigned int dmmr;		/* 0x0C */
	unsigned int tran_csr;	/* 0x10 */
	unsigned int tran_num;	/* 0x14 */
	unsigned int ff_port;	/* 0x18 */
	unsigned int rsvd;		/* 0x1C */
	unsigned int ff_pt;		/* 0x20 */
	unsigned int rsvd1;		/* 0x24 */
	unsigned int int_sts;	/* 0x28 */
	unsigned int int_en;	/* 0x2C */
};

struct cvitek_spi_priv {
	unsigned long ctrl_base;
	unsigned int freq;
	unsigned int mode;
	unsigned int sck_div;
	unsigned int sck;
	struct cvi_spif_regs *regs;
	unsigned int max_hz;
	unsigned int orig_tran_csr;

#define CMD_HAS_ADR		BIT(24)
#define CMD_HAS_DUMMY	BIT(25)
#define CMD_HAS_DATA	BIT(26)
};

struct dmmr_reg_t {
	uint8_t read_cmd;
	uint32_t dummy_clock;
	uint32_t reg_set;
};

const struct dmmr_reg_t dmmr_reg_set[16] = {
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

#endif	/* __CVI_SPIF_H__ */
