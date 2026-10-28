/*
 * Copyright (c) 2016-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __CV_SPI_NAND_H__
#define __CV_SPI_NAND_H__
#include <platform_def.h>

#define SPI_NAND_READ_FROM_CACHE_MODE_X1 0
#define SPI_NAND_READ_FROM_CACHE_MODE_X2 1
#define SPI_NAND_READ_FROM_CACHE_MODE_X4 2

#define SPI_NAND_CMD_WREN			0x06
#define SPI_NAND_CMD_WRDI			0x04
#define SPI_NAND_CMD_GET_FEATURE		0x0F
#define SPI_NAND_CMD_SET_FEATURE		0x1F
#define SPI_NAND_CMD_PAGE_READ_TO_CACHE		0x13
#define SPI_NAND_CMD_READ_FROM_CACHE		0x03
#define SPI_NAND_CMD_READ_FROM_CACHE2		0x0B
#define SPI_NAND_CMD_READ_FROM_CACHEX2		0x3B
#define SPI_NAND_CMD_READ_FROM_CACHEX4		0x6B
#define SPI_NAND_CMD_READ_FROM_DUAL_IO		0xBB
#define SPI_NAND_CMD_READ_FROM_QUAL_IO		0xEB
#define SPI_NAND_CMD_READ_ID			0x9F
#define SPI_NAND_CMD_PROGRAM_LOAD		0x02
#define SPI_NAND_CMD_PROGRAM_LOADX4		0x32
#define SPI_NAND_CMD_PROGRAM_EXECUTE		0x10
#define SPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATA		0x84
#define SPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATAX4		0xC4
#define SPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATA_QUAD_IO	0x72

#define SPI_NAND_CMD_4K_SUBSECTOR_ERASE		0x20
#define SPI_NAND_CMD_32K_SUBSECTOR_ERASE	0x52
#define SPI_NAND_CMD_BLOCK_ERASE		0xD8
#define SPI_NAND_CMD_RESET			0xFF

#define SPI_NAND_FEATURE_PROTECTION	(0xA0)
#define FEATURE_PROTECTION_NONE			0
#define SPI_NAND_PROTECTION_CMP			(0x01 << 1)
#define SPI_NAND_PROTECTION_INV			(0x01 << 2)
#define SPI_NAND_PROTECTION_BP0			(0x01 << 3)
#define SPI_NAND_PROTECTION_BP1			(0x01 << 4)
#define SPI_NAND_PROTECTION_BP2			(0x01 << 5)
#define SPI_NAND_PROTECTION_BRWD		(0x01 << 7)


#define SPI_NAND_FEATURE_FEATURE0	(0xB0)
#define SPI_NAND_FEATURE0_QE			(0x01 << 0)
#define SPI_NAND_FEATURE0_ECC_EN		(0x01 << 4)
#define SPI_NAND_FEATURE0_OTP_EN		(0x01 << 6)
#define SPI_NAND_FEATURE0_OTP_PRT		(0x01 << 7)

#define SPI_NAND_FEATURE_ECC_CONFIG    (0x90)
#define SPI_NAND_FEATURE_CFG_ECC_EN    (0x01 << 4)

#define SPI_NAND_FEATURE_STATUS0	(0xC0)
#define SPI_NAND_STATUS0_OIP			(0x01 << 0)
#define SPI_NAND_STATUS0_WEL			(0x01 << 1)
#define SPI_NAND_STATUS0_E_FAIL			(0x01 << 2)
#define SPI_NAND_STATUS0_P_FAIL			(0x01 << 3)
#define SPI_NAND_STATUS0_ECCS0			(0x01 << 4)
#define SPI_NAND_STATUS0_ECCS1			(0x01 << 5)

#define SPI_NAND_FEATURE_FEATURE1	(0xD0)
#define SPI_NAND_FEATURE1_DS_S0			(0x01 << 5)
#define SPI_NAND_FEATURE1_DS_S1			(0x01 << 6)

#define SPI_NAND_FEATURE_STATUS1	(0xF0)
#define SPI_NAND_STATUS1_ECCSE0			(0x01 << 4)
#define SPI_NAND_STATUS1_ECCSE1			(0x01 << 5)

#define SPI_NAND_FLASH_BLOCK_SIZE             256
#define SPI_NAND_TRAN_CSR_ADDR_BYTES_SHIFT    8
#define SPI_NAND_MAX_FIFO_DEPTH               8

/*1880V2 spi nand definitions */
#define REG_SPI_NAND_TRX_CTRL0  0x000
#define BIT_REG_TRX_START           (0x01 << 0)
#define BIT_REG_TRX_SW_RST          (0x01 << 1)
#define BIT_REG_TRX_RST_DONE        (0x01 << 16)

#define REG_SPI_NAND_TRX_CTRL1  0x004
#define BIT_REG_TRX_TIME_START      (0x01 << 0)
#define BIT_REG_TRX_TIME_TA         (0x01 << 4)
#define BIT_REG_TRX_TIME_END        (0x01 << 4)
#define BIT_REG_TRX_TIME_WAIT       (0x01 << 8)
#define SPI_NAND_SCK_MASK				(0x00FF0000)
#define BIT_REG_TRX_SCK_H           (0x01 << 16)
#define SPI_NAND_SET_SCK_H(x)			(x << 16)
#define BIT_REG_TRX_SCK_L           (0x01 << 20)
#define SPI_NAND_SET_SCK_L(x)			(x << 20)
#define BIT_REG_IO_CPOL             (0x01 << 24)
#define BIT_REG_IO_CPHA             (0x01 << 25)


#define REG_SPI_NAND_TRX_CTRL2  0x008
#define BIT_REG_TRX_CMD_CONT_SIZE   (0x01 << 0)
#define BIT_REG_TRX_DUMMY_SIZE      (0x01 << 4)
#define BIT_REG_TRX_DATA_SIZE       (0x01 << 16)
#define TRX_CMD_CONT_SIZE_SHIFT    (0)
#define TRX_DUMMY_SIZE_SHIFT        (4)
#define TRX_DATA_SIZE_SHIFT         (16)

#define REG_SPI_NAND_TRX_CTRL3  0x00C
#define BIT_REG_IO_SIZE_MODE        (0x01 << 0)
#define SPI_NAND_CTRL3_IO_TYPE_X1_MODE      0
#define SPI_NAND_CTRL3_IO_TYPE_X2_MODE      2
#define SPI_NAND_CTRL3_IO_TYPE_X4_MODE      3
#define SPI_NAND_CTRL3_IO_TYPE_DUAL_MODE    6
#define SPI_NAND_CTRL3_IO_TYPE_QUAD_MODE    7

#define BIT_REG_TRX_RW              (0x01 << 16) // 1 for write, 0 for read
#define BIT_REG_TRX_DUMMY_HIZ       (0x01 << 17)
#define BIT_REG_TRX_DMA_EN          (0x01 << 18)
#define BIT_REG_RSP_CHK_EN          (0x01 << 19)


#define REG_SPI_NAND_INT_EN         0x010
#define BIT_REG_TRX_DONE_INT_EN         (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT_EN    (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT_EN      (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT_EN       (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT_EN      (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT_EN       (0x01 << 7)
#define BIT_REG_DMA_DONE_INT_EN         (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT_EN       (0x01 << 9)
#define BITS_SPI_NAND_INT_EN_ALL         (BIT_REG_TRX_DONE_INT_EN | BIT_REG_TRX_EXCEPTION_INT_EN \
					| BIT_REG_TX_PUSH_ERR_INT_EN | BIT_REG_TX_POP_ERR_INT_EN \
					| BIT_REG_RX_PUSH_ERR_INT_EN | BIT_REG_RX_POP_ERR_INT_EN \
					| BIT_REG_DMA_DONE_INT_EN | BIT_REG_DMA_E_TERM_INT_EN)


#define REG_SPI_NAND_INT_CLR        0x014
#define BIT_REG_TRX_DONE_INT_CLR        (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT_CLR   (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT_CLR     (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT_CLR      (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT_CLR     (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT_CLR      (0x01 << 7)
#define BIT_REG_DMA_DONE_INT_CLR        (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT_CLR      (0x01 << 9)
#define BITS_SPI_NAND_INT_CLR_ALL         (BIT_REG_TRX_DONE_INT_CLR | BIT_REG_TRX_EXCEPTION_INT_CLR \
					| BIT_REG_TX_PUSH_ERR_INT_CLR | BIT_REG_TX_POP_ERR_INT_CLR \
					| BIT_REG_RX_PUSH_ERR_INT_CLR | BIT_REG_RX_POP_ERR_INT_CLR \
					| BIT_REG_DMA_DONE_INT_CLR | BIT_REG_DMA_E_TERM_INT_CLR)


#define REG_SPI_NAND_INT_MASK       0x018
#define BIT_REG_TRX_DONE_INT_MSK        (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT_MSK   (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT_MSK     (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT_MSK      (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT_MSK     (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT_MSK      (0x01 << 7)
#define BIT_REG_DMA_DONE_INT_MSK        (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT_MSK      (0x01 << 9)
#define BITS_SPI_NAND_INT_MASK_ALL         (BIT_REG_TRX_DONE_INT_MSK | BIT_REG_TRX_EXCEPTION_INT_MSK \
					| BIT_REG_TX_PUSH_ERR_INT_MSK | BIT_REG_TX_POP_ERR_INT_MSK \
					| BIT_REG_RX_PUSH_ERR_INT_MSK | BIT_REG_RX_POP_ERR_INT_MSK \
					| BIT_REG_DMA_DONE_INT_MSK | BIT_REG_DMA_E_TERM_INT_MSK)


#define REG_SPI_NAND_INT            0x01C
#define BIT_REG_TRX_DONE_INT            (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT       (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT         (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT          (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT         (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT          (0x01 << 7)
#define BIT_REG_DMA_DONE_INT            (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT          (0x01 << 9)
#define BITS_REG_TRX_DMA_DONE_INT       (BIT_REG_TRX_DONE_INT | BIT_REG_DMA_DONE_INT)


#define REG_SPI_NAND_INT_RAW            0x020
#define BIT_REG_TRX_DONE_INT_RAW            (0x01 << 0)
#define BIT_REG_TRX_EXCEPTION_INT_RAW       (0x01 << 1)
#define BIT_REG_TX_PUSH_ERR_INT_RAW         (0x01 << 4)
#define BIT_REG_TX_POP_ERR_INT_RAW          (0x01 << 5)
#define BIT_REG_RX_PUSH_ERR_INT_RAW         (0x01 << 6)
#define BIT_REG_RX_POP_ERR_INT_RAW          (0x01 << 7)
#define BIT_REG_DMA_DONE_INT_RAW            (0x01 << 8)
#define BIT_REG_DMA_E_TERM_INT_RAW          (0x01 << 9)

#define REG_SPI_NAND_BOOT_CTRL          0x024
#define BIT_REG_BOOT_PRD                    (0x01 << 0)
#define SPI_NAND_BOOT_SAMPLE_MASK			(0xFFFFFFFE)
#define BIT_REG_RSP_DLY_SEL                 (0x01 << 8)
#define BIT_REG_RSP_NEG_SEL                 (0x01 << 12)



#define REG_SPI_NAND_IO_CTRL            0x028
#define BIT_REG_CSN0_OUT_OW_EN              (0x01 << 0)
#define BIT_REG_CSN1_OUT_OW_EN              (0x01 << 1)
#define BIT_REG_SCK_OUT_OW_EN               (0x01 << 3)
#define BIT_REG_MOSI_OUT_OW_EN              (0x01 << 4)
#define BIT_REG_MISO_OUT_OW_EN              (0x01 << 5)
#define BIT_REG_WPN_OUT_OW_EN               (0x01 << 6)
#define BIT_REG_HOLDN_OUT_OW_EN             (0x01 << 7)
#define BIT_REG_CSN0_OUT_OW_VAL             (0x01 << 8)
#define BIT_REG_CSN1_OUT_OW_VAL             (0x01 << 9)
#define BIT_REG_SCK_OUT_OW_VAL              (0x01 << 11)
#define BIT_REG_MOSI_OUT_OW_VAL             (0x01 << 12)
#define BIT_REG_MISO_OUT_OW_VAL             (0x01 << 13)
#define BIT_REG_WPN_OUT_OW_VAL              (0x01 << 14)
#define BIT_REG_HOLDN_OUT_OW_VAL            (0x01 << 15)
#define BIT_REG_CSN0_OEN_OW_EN              (0x01 << 16)
#define BIT_REG_CSN1_OEN_OW_EN              (0x01 << 17)
#define BIT_REG_SCK_OEN_OW_EN               (0x01 << 19)
#define BIT_REG_MOSI_OEN_OW_EN              (0x01 << 20)
#define BIT_REG_MISO_OEN_OW_EN              (0x01 << 21)
#define BIT_REG_WPN_OEN_OW_EN               (0x01 << 22)
#define BIT_REG_HOLDN_OEN_OW_EN             (0x01 << 23)
#define BIT_REG_CSN0_OEN_OW_VAL             (0x01 << 24)
#define BIT_REG_CSN1_OEN_OW_VAL             (0x01 << 25)
#define BIT_REG_SCK_OEN_OW_VAL              (0x01 << 27)
#define BIT_REG_MOSI_OEN_OW_VAL             (0x01 << 28)
#define BIT_REG_MISO_OEN_OW_VAL             (0x01 << 29)
#define BIT_REG_WPN_OEN_OW_VAL              (0x01 << 30)
#define BIT_REG_HOLDN_OEN_OW_VAL            (0x01 << 31)


#define REG_SPI_NAND_IO_STATUS      0x02C
#define BIT_REG_CSN0_VAL                (0x01 << 0)
#define BIT_REG_CSN1_VAL                (0x01 << 1)
#define BIT_REG_SCK_VAL                 (0x01 << 3)
#define BIT_REG_MOSI_VAL                (0x01 << 4)
#define BIT_REG_MISO_VAL                (0x01 << 5)
#define BIT_REG_WPN_VAL                 (0x01 << 6)
#define BIT_REG_HOLDN_VAL               (0x01 << 7)


#define REG_SPI_NAND_TRX_CMD0       0x30
#define BIT_REG_TRX_CMD_IDX             (0x01 << 0)
#define BIT_REG_TRX_CMD_CONT0           (0x01 << 8)
#define TRX_CMD_CONT0_SHIFT             (8)


#define REG_SPI_NAND_TRX_CMD1           0x034
//#define BIT_REG_TRX_CMD_CONT1               (0x01 << 0)


#define REG_SPI_NAND_TRX_CS             0x3C
#define BIT_REG_TRX_CS_SEL                  (0x01 << 0)


#define REG_SPI_NAND_TRX_DMA_CTRL       0x40
#define BIT_REG_DMA_WT_TH                   (0x01 << 0)
#define BIT_REG_DMA_RD_TH                   (0x01 << 8)
#define BIT_REG_DMA_REQ_SIZE                (0x01 << 16)
#define BIT_REG_DMA_TX_EMPTY_SEL            (0x01 << 24)
#define BIT_REG_DMA_RX_FULL_SEL             (0x01 << 25)


#define REG_SPI_NAND_TRX_DMA_STATUS     0x44
#define BIT_REG_DMA_REQ                     (0x01 << 0)
#define BIT_REG_DMA_SINGLE                  (0x01 << 1)
#define BIT_REG_DMA_LAST                    (0x01 << 2)
#define BIT_REG_DMA_ACK                     (0x01 << 3)
#define BIT_REG_DMA_FINISH                  (0x01 << 4)


#define REG_SPI_NAND_TRX_DMA_SW         0x48
#define BIT_REG_DMA_SW_MODE                 (0x01 << 0)
#define BIT_REG_DMA_SW_ACK                  (0x01 << 8)
#define BIT_REG_DMA_SW_FINISH               (0x01 << 9)


#define REG_SPI_NAND_TX_FIFO_STATUS     0x50
#define BIT_REG_TX_PUSH_EMPTY               (0x01 << 0)
#define BIT_REG_TX_PUSH_AE                  (0x01 << 1)
#define BIT_REG_TX_PUSH_HF                  (0x01 << 2)
#define BIT_REG_TX_PUSH_AF                  (0x01 << 3)
#define BIT_REG_TX_PUSH_FULL                (0x01 << 4)
#define BIT_REG_TX_PUSH_ERROR               (0x01 << 5)
#define BIT_REG_TX_PUSH_WORD_COUNT          (0x01 << 8)
#define BIT_REG_TX_POP_EMPTY                (0x01 << 16)
#define BIT_REG_TX_POP_AE                   (0x01 << 17)
#define BIT_REG_TX_POP_HF                   (0x01 << 18)
#define BIT_REG_TX_POP_AF                   (0x01 << 19)
#define BIT_REG_TX_POP_FULL                 (0x01 << 20)
#define BIT_REG_TX_POP_ERROR                (0x01 << 21)
#define BIT_REG_TX_POP_WORD_COUNT           (0x01 << 24)


#define REG_SPI_NAND_RX_FIFO_STATUS     0x54
#define BIT_REG_RX_PUSH_EMPTY               (0x01 << 0)
#define BIT_REG_RX_PUSH_AE                  (0x01 << 1)
#define BIT_REG_RX_PUSH_HF                  (0x01 << 2)
#define BIT_REG_RX_PUSH_AF                  (0x01 << 3)
#define BIT_REG_RX_PUSH_FULL                (0x01 << 4)
#define BIT_REG_RX_PUSH_ERROR               (0x01 << 5)
#define BIT_REG_RX_PUSH_WORD_COUNT          (0x01 << 8)
#define BIT_REG_RX_POP_EMPTY                (0x01 << 16)
#define BIT_REG_RX_POP_AE                   (0x01 << 17)
#define BIT_REG_RX_POP_HF                   (0x01 << 18)
#define BIT_REG_RX_POP_AF                   (0x01 << 19)
#define BIT_REG_RX_POP_FULL                 (0x01 << 20)
#define BIT_REG_RX_POP_ERROR                (0x01 << 21)
#define BIT_REG_RX_POP_WORD_COUNT           (0x01 << 24)

#define REG_SPI_NAND_CMPLT_BYTE_CNT     0x58
#define BIT_REG_CMPLT_CNT                   (0x01 << 0)

#define REG_SPI_NAND_TX_DATA            0x60
#define BIT_REG_TX_DATA                     (0x01 << 0)

#define REG_SPI_NAND_RX_DATA            0x64
#define BIT_REG_RX_DATA                     (0x01 << 0)

#define REG_SPI_NAND_RSP_POLLING        0x68
#define BIT_REG_RSP_EXP_MSK                 (0x01 << 0)
#define BIT_REG_RSP_EXP_VAL                 (0x01 << 8)
#define BIT_REG_RSP_WAIT_TIME_OFFSET        (16)

#define REG_SPI_NAND_SPARE0             0x70
#define BIT_REG_SPARE0                      (0x01 << 0)

#define REG_SPI_NAND_SPARE1             0x74
#define BIT_REG_SPARE1                      (0x01 << 0)

#define REG_SPI_NAND_SPARE_RO           0x78
#define BIT_REG_SPARE_RO                    (0x01 << 0)

#define REG_SPI_NAND_TX_FIFO            0x800
#define BIT_REG_TX_FIFO                     (0x01 << 0)

#define REG_SPI_NAND_RX_FIFO            0xC00
#define BIT_REG_RX_FIFO                     (0x01 << 0)

#define SPI_NAND_ID_GD5F1GQ4U           0xD1C8  // SPI NAND 1Gbit 3.3V
#define SPI_NAND_ID_GD5F1GQ4R           0xC1C8  // SPI NAND 1Gbit 1.8V

#define SPI_NAND_BLOCK_RA_SHIFT         (6)     // RA<15:6>

#define SPI_NAND_BLOCK_RA_NUM           (10)    // RA<15:6>
#define SPI_NAND_PAGE_RA_NUM            (6)     // RA<5:0>

#define SPI_NAND_PAGE_SIZE              (2048)

/*
 * P_FAIL : Program Fail
 *	This bit indicates that a program failure has occurred (P_FAIL set to 1). It will also be
 *	set if the user attempts to program an invalid address or a protected region, including
 *	the OTP area. This bit is cleared during the PROGRAM EXECUTE command
 *	sequence or a RESET command (P_FAIL = 0).
 *
 * E_FAIL : Erase Fail
 *	This bit indicates that an erase failure has occurred (E_FAIL set to 1). It will also be
 *	set if the user attempts to erase a locked region. This bit is cleared (E_FAIL = 0) at the
 *	start of the BLOCK ERASE command sequence or the RESET command.
 *
 * WEL : Write Enable Latch
 *	This bit indicates the current status of the write enable latch (WEL) and must be set
 *	(WEL = 1), prior to issuing a PROGRAM EXECUTE or BLOCK ERASE command. It
 *	is set by issuing the WRITE ENABLE command. WEL can also be disabled (WEL =
 *	0), by issuing the WRITE DISABLE command.
 *
 * OIP : Operation In Progress
 *	This bit is set (OIP = 1 ) when a PROGRAM EXECUTE, PAGE READ, BLOCK
 *	ERASE, or RESET command is executing, indicating the device is busy. When the bit
 *	is 0, the interface is in the ready state
 *
 * Register Addr. 7        6        5      4      3        2        1        0
 * Status C0H     Reserved Reserved ECCS1  ECCS0  P_FAIL   E_FAIL   WEL      OIP
 * Status F0H     Reserved Reserved ECCSE1 ECCSE0 Reserved Reserved Reserved Reserved
 *
 * ECCS1 ECCS0 ECCSE1 ECCSE0 Description
 *   0     0     x      x     No bit errors were detected during the previous read algorithm.
 *   0     1     0      0     Bit errors(<4) were detected and corrected.
 *   0     1     0      1     Bit errors(=5) were detected and corrected.
 *   0     1     1      0     Bit errors(=6) were detected and corrected.
 *   0     1     1      1     Bit errors(=7) were detected and corrected.
 *   1     0     x      x     Bit errors greater than ECC capability(8 bits) and not corrected
 *   1     1     x      x     Bit errors reach ECC capability( 8 bits) and corrected
 */

#define ECC_NOERR  0
#define ECC_CORR   1
#define ECC_UNCORR 2

#define BBP_LAST_PAGE			0x01
#define BBP_FIRST_PAGE			0x02
#define BBP_FIRST_2_PAGE		0x03

struct spinand_cmd {
	uint8_t		cmd;
	uint32_t	n_cmd_cont;		/* Number of command content */
	uint8_t		content[3];	/* Command content */
	uint32_t	n_dummy;	/* Dummy use */
	uint32_t	n_tx;		/* Number of tx bytes */
	uint32_t	*tx_buf;	/* Tx buf */
	uint32_t	n_rx;		/* Number of rx bytes */
	uint32_t	*rx_buf;	/* Rx buf */
	uint32_t	flags;		/* Flags */
};

#define FLAGS_SET_PLANE_BIT		(BIT(0))
#define FLAGS_SET_QE_BIT		(BIT(1))
#define FLAGS_ENABLE_X2_BIT	(BIT(2))
#define FLAGS_ENABLE_X4_BIT	(BIT(3))
#define FLAGS_OW_SETTING_BIT	(BIT(4)) // overwrite setting from sysvec

#define FIP_IMAGE_HEAD	"FIPH" /* FIP Image Header 1 */
#define FIP_IMAGE_BODY	"FIPB" /* FIP Image body */
#define FIP_MAGIC_NUMBER "CVBL01\n\0"



#define FIP_BK_TAG_SIZE	4
struct block_header_t {
	uint8_t tag[FIP_BK_TAG_SIZE];
	uint32_t bk_cnt_or_seq; /* for first block, it is page count. Otherwise, it is sequence number */
	uint32_t checknum; /* the check number to make sure all fip header and body are consistent */
	uint32_t dummy;
};

struct block_info_t {
	uint8_t used;
	uint8_t blk_idx;
	uint16_t dummy;
	uint32_t checknum;
};


uint16_t cv_spi_nand_read_id(void);
uint8_t cv_spi_nand_query_ecc_en_status(void);
uint32_t cv_spi_nand_read_page_non_ecc(uint32_t blk_id, uint32_t pg_nr, void *buf, uint32_t len);
void cv_spi_nand_dump_reg(void);
void cv_spi_nand_flash_read_sector(uint32_t addr, uint8_t *buf);
struct spi_nand_info_t *spi_nand_get_nand_info(void);
uint32_t cv_spi_nand_reset(void);
uint32_t cv_spi_nand_init(uint32_t base);
uint32_t cv_spi_nand_read_page_by_row_addr(uint32_t row_addr, void *buf, uint32_t len);
uint32_t cv_spi_nand_read_oob(uint32_t row_addr, void *buf);
void cv_spi_nand_set_qe(uint32_t enable);
void cv_spi_nand_set_default_info(void);
void cv_spi_nand_set_device(struct spi_nand_info_t *info);
struct spi_nand_info_t *cv_spi_nand_get_info(void);
void cv_spi_nand_dump_dma_reg(void);
void cv_spi_nand_current_freq(void);
void cv_spi_nand_phy_init(void);
extern void cv_spi_nand_g_param_init(void);
void spi_nand_downgrade_read_mode(void);
void spi_nand_downgrade_freq(void);
void cv_spi_nand_set_freq(struct spi_nand_info_t nand_info);
int get_nand_info(void);
int cv_spi_nand_read_data(void * buffer, uint32_t offset, uint32_t size);
#endif	/* __CV_SPI_NAND_H__ */
