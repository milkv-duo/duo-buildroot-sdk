#ifndef __CVSNFC_H__
#define __CVSNFC_H__

#include <asm/io.h>
#include <cvsnfc_spi_ids.h>

/******************************************************************************/
/* These macroes are for debug only, reg option is slower then dma option */
#undef CVSNFC_SUPPORT_REG_READ
/* #define CVSNFC_SUPPORT_REG_READ */

#undef CVSNFC_SUPPORT_REG_WRITE
/* #define CVSNFC_SUPPORT_REG_WRITE */

#define CVI_SYSDMA_REMAP_BASE	0x3000154

/*****************************************************************************/
// legacy register define
#define CVSNFC_CFG					0x00
#define CVSNFC_CFG_BOOT_MODE(_mode)			((_mode) << 17)
#define CVSNFC_CFG_DEVICE_INTERNAL_ECC_ENABLE	BIT(16)
#define CVSNFC_CFG_FLASH_SIZE_CS1(_size)		(((_size) & 0xf) << 10)
#define CVSNFC_CFG_FLASH_SIZE_CS0(_size)		(((_size) & 0xf) << 6)
#define CVSNFC_CFG_ADDR_MODE(_mode)			((_mode) << 5)
#define CVSNFC_CFG_ECC_TYPE(type)			(((type) & 0x3) << 3)
#define CVSNFC_CFG_PAGE_SIZE(size)			((size) << 2)
#define CVSNFC_CFG_DEVICE_TYPE(_type)		((_type) << 1)
#define CVSNFC_CFG_OP_MODE(_mode)			((_mode) << 0)

#define BOOT_MODE_MASK				(0x1 << 17)

#define FLASH_SIZE_CS1_SHIFT			10
#define FLASH_SIZE_CS1_MASK			(0xf << 10)

#define FLASH_SIZE_CS0_SHIFT			6
#define FLASH_SIZE_CS0_MASK			(0xf << 6)

#define ECC_TYPE_SHIFT				3
#define ECC_TYPE_MASK				(0x3 << 3)

#define PAGE_SIZE_SHIFT				2
#define PAGE_SIZE_MASK				(0x1 << 2)

#define DEVICE_TYPE_SHIFT			1
#define DEVICE_TYPE_MASK			(0x1 << 1)
#define DEVICE_TYPE_NOR_FLASH			0
#define DEVICE_TYPE_NAND_FLASH			1

#define OP_MODE_MASK				(0x1 << 0)
#define OP_MODE_BOOT				0
#define OP_MODE_NORMAL				1

/*****************************************************************************/
#define CVSNFC_GLOBAL_CFG			0x04
#define CVSNFC_GLOBAL_CFG_WP_ENABLE		BIT(5)

/*****************************************************************************/
#define CVSNFC_TIMING_CFG			0x08
#define CVSNFC_TIMING_CFG_TCSH(_n)		(((_n) & 0xf) << 8)
#define CVSNFC_TIMING_CFG_TCSS(_n)		(((_n) & 0xf) << 4)
#define CVSNFC_TIMING_CFG_TSHSL(_n)		((_n) & 0xf)

#define CS_HOLD_TIME				0x6
#define CS_SETUP_TIME				0x6
#define CS_DESELECT_TIME			0xf

/*****************************************************************************/
#define CVSNFC_INT				0x0c
#define CVSNFC_INT_OP_DONE			BIT(0)

/*****************************************************************************/
#define CVSNFC_INT_CLR			0x14
#define CVSNFC_INT_CLR_DMA_ERR		BIT(5)
#define CVSNFC_INT_CLR_OP_FAIL		BIT(2)
#define CVSNFC_INT_CLR_OP_DONE		BIT(0)

#define CVSNFC_INT_CLR_ALL			0x7f

/*****************************************************************************/
#define CVSNFC_OPCODE			0x18

/*****************************************************************************/
#define CVSNFC_OP_ADDR			0x1c
#define CVSNFC_OP_ADDRH_BLOCK_MASK(_page)	(((_page) & 0xffff) << 16)
#define CVSNFC_OP_ADDRL_BLOCK_MASK(_page)	((_page) & 0xffc0)

#define READ_ID_ADDR				0x00
#define PROTECTION_ADDR				0xa0
#define FEATURE_ADDR				0xb0
#define STATUS_ADDR				0xc0
#define WRITE_ENABLE_STATUS_ADDR		STATUS_ADDR

/*****************************************************************************/
#define CVSNFC_OP				0x20
#define CVSNFC_OP_SEL_CS(_cs)		((_cs) << 11)
#define CVSNFC_OP_ADDR_NUM(_n)		(((_n) & 0x7) << 8)
#define CVSNFC_OP_DUMMY_CMD_EN(_en)		((_en) << 7)
#define CVSNFC_OP_DUMMY_ADDR_EN(_en)		((_en) << 6)
#define CVSNFC_OP_OPCODE_EN(_en)		((_en) << 5)
#define CVSNFC_OP_ADDR_EN(_en)		((_en) << 4)
#define CVSNFC_OP_DATE_WRITE_EN(_en)		((_en) << 3)
#define CVSNFC_OP_DATE_READ_EN(_en)		((_en) << 2)
#define CVSNFC_OP_STATUS_READ_EN(_en)	((_en) << 1)
#define CVSNFC_OP_START			BIT(0)

#define STD_OP_ADDR_NUM				3
#define READ_ID_ADDR_NUM			1

#define FEATURES_OP_ADDR_NUM			1

/*****************************************************************************/
#define CVSNFC_DATA_NUM			0x24
#define CVSNFC_DATA_NUM_CNT(_n)		((_n) & 0x1fff)

#define FEATURES_DATE_NUM			1

#define READ_OOB_BB_LEN				1

/*****************************************************************************/
#define CVSNFC_OP_CFG			0x28
#define CVSNFC_OP_CFG_DIR_TRANS_ENABLE	BIT(11)
#define CVSNFC_OP_CFG_RD_OP_SEL(_type)	(((_type) & 0x3) << 9)
#define CVSNFC_OP_CFG_MEM_IF_TYPE(_type)	(((_type) & 0x7) << 6)
#define CVSNFC_OP_CFG_DUMMY_CMD_NUM(_no)	(((_no) & 0x7) << 3)
#define CVSNFC_OP_CFG_DUMMY_ADDR_NUM(_no)	(((_no) & 0x7) << 0)

#define RD_OP_READ_PAGE				0x0
#define RD_OP_READ_DATE				0x1
#define RD_OP_READ_OOB				0x2

/*****************************************************************************/
#define CVSNFC_ADDRH				0x2c
#define CVSNFC_ADDRH_SET(_addr)		((_addr) & 0xff)

/*****************************************************************************/
#define CVSNFC_ADDRL				0x30

/*****************************************************************************/
#define CVSNFC_OP_CTRL			0x34
#define CVSNFC_OP_CTRL_RD_OPCODE(_code)	(((_code) & 0xff) << 16)
#define CVSNFC_OP_CTRL_WR_OPCODE(_code)	(((_code) & 0xff) << 8)
#define CVSNFC_OP_CTRL_CS_OP(_cs)		((_cs) << 3)
#define CVSNFC_OP_CTRL_OP_TYPE(_type)	((_type) << 2)
#define CVSNFC_OP_CTRL_RW_OP(_op)		((_op) << 1)
#define CVSNFC_OP_CTRL_OP_READY		BIT(0)

#define OP_TYPE_REG				0
#define OP_TYPE_DMA				1

#define RW_OP_READ				0
#define RW_OP_WRITE				1

/*****************************************************************************/
#define CVSNFC_DMA_CTRL			0x3c

#define CVSNFC_DMA_CTRL_ALL_ENABLE		0x7

/*****************************************************************************/
#define CVSNFC_DMA_SADDR_D			0x40

/*****************************************************************************/
#define CVSNFC_DMA_SADDR_OOB			0x44

/*****************************************************************************/
#define CVSNFC_DMA_LEN			0x48
#define CVSNFC_DMA_LEN_SET(_len)		((_len) & 0xfffffff)

/*****************************************************************************/
#define CVSNFC_STATUS			0x54

#define GET_OP					0
#define SET_OP					1

#define PROTECTION_BRWD_MASK			BIT(7)
#define PROTECTION_BP3_MASK			BIT(6)
#define PROTECTION_BP2_MASK			BIT(5)
#define PROTECTION_BP1_MASK			BIT(4)
#define PROTECTION_BP0_MASK			BIT(3)

#define ALL_BP_MASK				(PROTECTION_BP3_MASK \
						| PROTECTION_BP2_MASK \
						| PROTECTION_BP1_MASK \
						| PROTECTION_BP0_MASK)

#define FEATURE_ECC_ENABLE			BIT(4)
#define FEATURE_QE_ENABLE			BIT(0)

#define STATUS_ECC_MASK				(0x3 << 4)
#define STATUS_P_FAIL_MASK			BIT(3)
#define STATUS_E_FAIL_MASK			BIT(2)
#define STATUS_WEL_MASK				BIT(1)
#define STATUS_OIP_MASK				BIT(0)

/*****************************************************************************/
#define CVSNFC_VERSION			0x68

/*****************************************************************************/
#define REG_CNT_HIGH_BLOCK_NUM_SHIFT		10

#define REG_CNT_BLOCK_NUM_MASK			0x3ff
#define REG_CNT_BLOCK_NUM_SHIFT			22

#define REG_CNT_PAGE_NUM_MASK			0x3f
#define REG_CNT_PAGE_NUM_SHIFT			16

#define REG_CNT_WRAP_MASK			0xf
#define REG_CNT_WRAP_SHIFT			12

#define REG_CNT_ECC_OFFSET_MASK			0xfff
#define REG_CNT_ECC_8BIT_OFFSET			1054
#define REG_CNT_ECC_16BIT_OFFSET		1056
#define REG_CNT_ECC_24BIT_OFFSET		1082

/* latest register definition */
#define SPI_NAND_PROGRAM_LOAD_X1 0
#define SPI_NAND_PROGRAM_LOAD_X4 1

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
#define SPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATAX4	0xC4
#define SPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATA_QUAD_IO	0x72

#define SPI_NAND_CMD_4K_SUBSECTOR_ERASE		0x20
#define SPI_NAND_CMD_32K_SUBSECTOR_ERASE	0x52
#define SPI_NAND_CMD_BLOCK_ERASE		0xD8
#define SPI_NAND_CMD_RESET			0xFF

#define SPI_NAND_FEATURE_ECC_CONFIG	(0x90)
#define SPI_NAND_FEATURE_CFG_ECC_EN	(0x01 << 4)

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
#define BIT_REG_TRX_SCK_H           (0x01 << 16)
#define BIT_REG_TRX_SCK_L           (0x01 << 20)
#define BIT_REG_NAND_SCK_MASK		(0x00FF0000)
#define SPI_NAND_SET_SCK_H(x)		((x) << 16)
#define SPI_NAND_SET_SCK_L(x)		((x) << 20)
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
//#define SPI_NAND_CTRL3_IO_TYPE_DUAL_MODE    6 // not support
//#define SPI_NAND_CTRL3_IO_TYPE_QUAD_MODE    7 // not support

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
#define BIT_REG_RSP_DLY_SEL                 (0x01 << 8)
#define BIT_REG_RSP_NEG_SEL                 (0x01 << 12)
#define BIT_REG_RSP_DLY_OW_EN				(0x01 << 30)
#define BIT_REG_RSP_DLY_MASK				0xFFFFFFFE

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
 * WEL : Write Enable  Latch
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
 *  0     0     x      x     No bit errors were detected during the previous read algorithm.
 *  0     1     0      0     Bit errors(<4) were detected and corrected.
 *  0     1     0      1     Bit errors(=5) were detected and corrected.
 *  0     1     1      0     Bit errors(=6) were detected and corrected.
 *  0     1     1      1     Bit errors(=7) were detected and corrected.
 *  1     0     x      x     Bit errors greater than ECC capability(8 bits) and not corrected
 *  1     1     x      x     Bit errors reach ECC capability( 8 bits) and corrected
 */

#define ECC_NOERR  0
#define ECC_CORR   1
#define ECC_UNCORR 2

struct spinand_cmd {
	uint8_t		cmd;
	uint32_t		n_cmd_cont;		/* Number of command content */
	uint8_t		content[3];	/* Command content */
	uint32_t		n_dummy;	/* Dummy use */
	uint32_t		n_tx;		/* Number of tx bytes */
	uint32_t	*tx_buf;	/* Tx buf */
	uint32_t		n_rx;		/* Number of rx bytes */
	uint32_t		*rx_buf;	/* Rx buf */
	uint32_t		flags;		/* Flags */
};

/*****************************************************************************/
#define SPI_NAND_MAX_PAGESIZE			4096
#define SPI_NAND_MAX_OOBSIZE			256

#define CVSNFC_BUFFER_LEN	(SPI_NAND_MAX_PAGESIZE + SPI_NAND_MAX_OOBSIZE)

/* DMA address align with 32 bytes. */
#define CVSNFC_DMA_ALIGN			32

#define CVSNFC_CHIP_DELAY			25

#define CVSNFC_ADDR_CYCLE_MASK		0x2

/*****************************************************************************/
struct cvsfc_cmd_option {
	unsigned char chipselect;
	unsigned char command;
	unsigned char last_cmd;
	unsigned char address_h;
	unsigned int address_l;
	unsigned int date_num;
	unsigned short option;
	unsigned short op_config;
};

#define MAX_PAGE_SIZE	(4096)
#define MAX_SPARE_SIZE	(256)

struct cvsnfc_host;

struct cvsnfc_host {
	struct nand_chip *chip;
	struct mtd_info *mtd;
	struct cvsnfc_op spi[CONFIG_CVSNFC_MAX_CHIP];
	struct cvsfc_cmd_option cmd_option;

	void __iomem *tx_iobase;
	void __iomem *rx_iobase;
	void __iomem *regbase;
	void __iomem *dmabase;

	unsigned int NFC_CFG;

	unsigned int offset;

	struct device *dev;

	unsigned int flags;

	unsigned int addr_cycle;
	unsigned int addr_value[2]; // 0:column, 1:row address
	unsigned int cache_addr_value[2];
	unsigned int column;
	unsigned int block_page_cnt;
	unsigned int block_page_mask;

	unsigned int dma_buffer;
	unsigned int dma_oob;

	unsigned int ecctype;
	unsigned int pagesize;
	unsigned int oobsize;
	unsigned long long diesize;

	/* This is maybe an un-aligment address, only for malloc or free */
	char *buforg;
	char *buffer;

	int add_partition;

	/* BOOTROM read two bytes to detect the bad block flag */
#define HINFC_BAD_BLOCK_POS              0
	unsigned char *bbm;  /* nand bad block mark */
	unsigned short *epm;  /* nand empty page mark */

	unsigned int uc_er;

	void (*set_system_clock)(struct spi_op_info *op, int clk_en);

	void (*send_cmd_pageprog)(struct cvsnfc_host *host);
	void (*send_cmd_status)(struct cvsnfc_host *host);
	void (*send_cmd_readstart)(struct cvsnfc_host *host);
	void (*send_cmd_erase)(struct cvsnfc_host *host);
	void (*send_cmd_readid)(struct cvsnfc_host *host);
	void (*send_cmd_reset)(struct cvsnfc_host *host);
	struct cvsnfc_chip_info *nand_chip_info;
};

struct cvsnfc_dly_param_grp {
	unsigned int start;
	unsigned int count;
};
/*****************************************************************************/
#define cvsfc_read(_host, _reg) \
	readl((_host)->regbase + (_reg))

#define cvsfc_write(_host, _reg, _value) \
	writel((_value), ((_host)->regbase + (_reg)))

void cvsnfc_register_dump(void);
void cvsnfc_dump_save_register(void);

/*****************************************************************************/
#define DBG_BUG(fmt, args...) do { \
	printf("%s(%d): BUG: " fmt, __FILE__, __LINE__, ##args); \
	while (1) \
		; \
} while (0)

/*****************************************************************************/
#define CVSNFC_WAIT_TIMEOUT 1000000

#define CVSNFC_CMD_WAIT_CPU_FINISH(_host) do { \
	unsigned int regval, timeout = CVSNFC_WAIT_TIMEOUT; \
	do { \
		regval = cvsfc_read((_host), REG_SPI_NAND_INT); \
		--timeout; \
	} while ((regval & BIT_REG_TRX_DONE_INT) == 0 && timeout); \
	if (!timeout) \
		DBG_BUG("cvsnfc wait cmd cpu finish timeout!\n"); \
} while (0)

#define CVSNFC_WAIT_DMA_FINISH(_host) do { \
	unsigned int regval, timeout = CVSNFC_WAIT_TIMEOUT; \
	do { \
		regval = cvsfc_read((_host), REG_SPI_NAND_INT); \
		--timeout; \
	} while ((regval & BIT_REG_DMA_DONE_INT) == 0 && timeout); \
	if (!timeout) \
		DBG_BUG("cvsnfc  wait dma finish timeout!\n"); \
} while (0)

/*****************************************************************************/
#define CVSNFC_DMA_WAIT_INT_FINISH(_host) do { \
	unsigned int regval, timeout = CVSNFC_WAIT_TIMEOUT; \
	do { \
		regval = cvsfc_read((_host), CVSNFC_INT); \
		--timeout; \
	} while ((!(regval & CVSNFC_INT_OP_DONE) && timeout)); \
	if (!timeout) \
		DBG_BUG("cvsnfc100 wait dma int finish timeout!\n"); \
} while (0)

/*****************************************************************************/
#define CVSNFC_DMA_WAIT_CPU_FINISH(_host) do { \
	unsigned int regval, timeout = CVSNFC_WAIT_TIMEOUT; \
	do { \
		regval = cvsfc_read((_host), CVSNFC_OP_CTRL); \
		--timeout; \
	} while ((regval & CVSNFC_OP_CTRL_OP_READY) && timeout); \
	if (!timeout) \
		DBG_BUG("dma wait cpu finish timeout\n"); \
} while (0)

static inline void CVSNFC_CLEAR_INT(struct cvsnfc_host *host)
{
	unsigned int regval;

	regval = cvsfc_read((host), REG_SPI_NAND_INT);
	cvsfc_write((host), REG_SPI_NAND_INT, regval);
}

void cvsnfc_ctrl_ecc(struct mtd_info *mtd, bool enable);
int spi_nand_adjust_max_freq(struct cvsnfc_host *host, uint32_t *pattern1, uint32_t *pattern2);
int check_and_update_fip_bin(void);
int do_cvi_update_spinand(uint32_t component, void *addr);
void bbt_dump_buf(char *s, void *buf, int len);
struct cvsnfc_host *cvsnfc_get_host(void);
int cvsnfc_read_page(struct mtd_info *mtd, struct nand_chip *chip,
		     uint8_t *buf, int bytes, int page);
int cvsnfc_write_page(struct mtd_info *mtd, struct nand_chip *chip,
		      const uint8_t *buf, int oob_required, int page);
int cvsnfc_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
			 uint8_t *buf, int oob_required, int page);

void cvsnfc_send_cmd_erase(struct cvsnfc_host *host);
/******************************************************************************/
#endif /* __CVSNFC_H__ */

