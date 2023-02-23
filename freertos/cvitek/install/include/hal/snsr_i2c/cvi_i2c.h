/*
 * Copyright (c) 2016-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __CVI_I2C_H__
#define __CVI_I2C_H__

#define CVI_I2C_MAX_NUM		4
#define CVI_I2C_DMA_ENABLE      0

/* register definitions */
#define REG_I2C_CON                      0x000
#define REG_I2C_TAR                      0x004
#define REG_I2C_SAR                      0x008
#define REG_I2C_HS_MADDR                 0x00C
#define REG_I2C_DATA_CMD                 0x010
#define REG_I2C_SS_SCL_HCNT              0x014
#define REG_I2C_SS_SCL_LCNT              0x018
#define REG_I2C_FS_SCL_HCNT              0x01C
#define REG_I2C_FS_SCL_LCNT              0x020
#define REG_I2C_HS_SCL_HCNT              0x024
#define REG_I2C_HS_SCL_LCNT              0x028
#define REG_I2C_INT_STAT                 0x02C
#define REG_I2C_INT_MASK                 0x030
#define REG_I2C_RAW_INT_STAT             0x034
#define REG_I2C_RX_TL                    0x038
#define REG_I2C_TX_TL                    0x03C
#define REG_I2C_CLR_INTR                 0x040
#define REG_I2C_CLR_RX_UNDER             0x044
#define REG_I2C_CLR_RX_OVER              0x048
#define REG_I2C_CLR_TX_OVER              0x04C
#define REG_I2C_CLR_RD_REQ               0x050
#define REG_I2C_CLR_TX_ABRT              0x054
#define REG_I2C_CLR_RX_DONE              0x058
#define REG_I2C_CLR_ACTIVITY             0x05C
#define REG_I2C_CLR_STOP_DET             0x060
#define REG_I2C_CLR_START_DET            0x064
#define REG_I2C_CLR_GEN_ALL              0x068
#define REG_I2C_ENABLE                   0x06C
#define REG_I2C_STATUS                   0x070
#define REG_I2C_TXFLR                    0x074
#define REG_I2C_RXFLR                    0x078
#define REG_I2C_SDA_HOLD                 0x07C
#define REG_I2C_TX_ABRT_SOURCE           0x080
#define REG_I2C_SLV_DATA_NACK_ONLY       0x084
#define REG_I2C_DMA_CR                   0x088
#define REG_I2C_DMA_TDLR                 0x08C
#define REG_I2C_DMA_RDLR                 0x090
#define REG_I2C_SDA_SETUP                0x094
#define REG_I2C_ACK_GENERAL_CALL         0x098
#define REG_I2C_ENABLE_STATUS            0x09C
#define REG_I2C_FS_SPKLEN                0x0A0
#define REG_I2C_HS_SPKLEN                0x0A4
#define REG_I2C_COMP_PARAM_1             0x0F4
#define REG_I2C_COMP_VERSION             0x0F8
#define REG_I2C_COMP_TYPE                0x0FC

/* bit definition */
#define BIT_I2C_CON_MASTER_MODE              (0x01 << 0)
#define BIT_I2C_CON_STANDARD_SPEED           (0x01 << 1)
#define BIT_I2C_CON_FULL_SPEED               (0x02 << 1)
#define BIT_I2C_CON_HIGH_SPEED               (0x03 << 1)
#define BIT_I2C_CON_10B_ADDR_SLAVE           (0x01 << 3)
#define BIT_I2C_CON_10B_ADDR_MASTER          (0x01 << 4)
#define BIT_I2C_CON_RESTART_EN               (0x01 << 5)
#define BIT_I2C_CON_SLAVE_DIS                (0x01 << 6)

#define BIT_I2C_INT_RX_UNDER                 (0x01 << 0)
#define BIT_I2C_INT_RX_OVER                  (0x01 << 1)
#define BIT_I2C_INT_RX_FULL                  (0x01 << 2)
#define BIT_I2C_INT_TX_OVER                  (0x01 << 3)
#define BIT_I2C_INT_TX_EMPTY                 (0x01 << 4)
#define BIT_I2C_INT_RD_REQ                   (0x01 << 5)
#define BIT_I2C_INT_TX_ABRT                  (0x01 << 6)
#define BIT_I2C_INT_RX_DONE                  (0x01 << 7)
#define BIT_I2C_INT_ACTIVITY                 (0x01 << 8)
#define BIT_I2C_INT_STOP_DET                 (0x01 << 9)
#define BIT_I2C_INT_START_DET                (0x01 << 10)
#define BIT_I2C_INT_GEN_ALL                  (0x01 << 11)

#define BIT_I2C_INT_RX_UNDER_MASK            (0x01 << 0)
#define BIT_I2C_INT_RX_OVER_MASK             (0x01 << 1)
#define BIT_I2C_INT_RX_FULL_MASK             (0x01 << 2)
#define BIT_I2C_INT_TX_OVER_MASK             (0x01 << 3)
#define BIT_I2C_INT_TX_EMPTY_MASK            (0x01 << 4)
#define BIT_I2C_INT_RD_REQ_MASK              (0x01 << 5)
#define BIT_I2C_INT_TX_ABRT_MASK             (0x01 << 6)
#define BIT_I2C_INT_RX_DONE_MASK             (0x01 << 7)
#define BIT_I2C_INT_ACTIVITY_MASK            (0x01 << 8)
#define BIT_I2C_INT_STOP_DET_MASK            (0x01 << 9)
#define BIT_I2C_INT_START_DET_MASK           (0x01 << 10)
#define BIT_I2C_INT_GEN_ALL_MASK             (0x01 << 11)

#define BIT_I2C_INT_RX_UNDER_RAW             (0x01 << 0)
#define BIT_I2C_INT_RX_OVER_RAW              (0x01 << 1)
#define BIT_I2C_INT_RX_FULL_RAW              (0x01 << 2)
#define BIT_I2C_INT_TX_OVER_RAW              (0x01 << 3)
#define BIT_I2C_INT_TX_EMPTY_RAW             (0x01 << 4)
#define BIT_I2C_INT_RD_REQ_RAW               (0x01 << 5)
#define BIT_I2C_INT_TX_ABRT_RAW              (0x01 << 6)
#define BIT_I2C_INT_RX_DONE_RAW              (0x01 << 7)
#define BIT_I2C_INT_ACTIVITY_RAW             (0x01 << 8)
#define BIT_I2C_INT_STOP_DET_RAW             (0x01 << 9)
#define BIT_I2C_INT_START_DET_RAW            (0x01 << 10)
#define BIT_I2C_INT_GEN_ALL_RAW              (0x01 << 11)

#define BIT_I2C_CMD_DATA_READ_BIT            (0x01 << 8)
#define BIT_I2C_CMD_DATA_STOP_BIT            (0x01 << 9)
#define BIT_I2C_CMD_DATA_RESTART_BIT         (0x01 << 10)

#define BIT_I2C_DMA_CR_RDMAE                 (0x01 << 0)
#define BIT_I2C_DMA_CR_TDMAE                 (0x01 << 1)

#define CH_I2C_TX	5

#define I2C_MAX_SPEED		3400000	// 3.4MHz
#define I2C_HIGH_SPEED		1000000	// 1MHz
#define I2C_FAST_SPEED		400000	// 400KHz
#define I2C_STANDARD_SPEED	100000	// 100KHz

#define I2C_100KHZ	0
#define I2C_400KHZ	1

struct i2c_info {
	uint64_t ctrl_base;	// i2c base address
	uint8_t enable;
	uint8_t slave_id;
	uint8_t alen;		// address length
	uint8_t dlen;		// data length
} __attribute__((packed));

int cvi_i2c_master_init(uint8_t bus_id, uint16_t slave_id, uint16_t speed, uint8_t alen, uint8_t dlen);
int cvi_i2c_master_write(uint8_t bus_id, uint16_t reg, uint16_t value);
uint16_t cvi_i2c_master_read(uint8_t bus_id, uint16_t reg);

#if CVI_I2C_DMA_ENABLE
void cvi_i2c_master_dma_init(unsigned long base, uint32_t ch, uint16_t *tx_buf, uint32_t len);
void cvi_i2c_master_dma_write(uint32_t ch);
void cvi_i2c_enable_dma(void);
void cvi_i2c_disable_dma(void);
void cvi_i2c_enable_dma_ch(uint32_t ch);
uint8_t cvi_i2c_is_dma_ch_enable(uint32_t ch);
void cvi_i2c_dma_bind_ch(uint32_t i2c_ch);
#endif

#endif	/* __CVI_I2C_H__ */
