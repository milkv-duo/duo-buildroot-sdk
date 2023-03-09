/* SPDX-License-Identifier: GPL-2.0-or-later
 * I2S driver on CVITEK CV1835
 *
 * Copyright 2018 CVITEK
 *
 * Author: EthanChen
 *
 *
 */

#ifndef __CVITEK_LOCAL_H__
#define __CVITEK_LOCAL_H__

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/types.h>
#include <sound/dmaengine_pcm.h>
#include <sound/pcm.h>
#include <sound/cv1835_i2s.h>

#define I2S_MODE            0x0
#define LJ_MODE             0x1
#define RJ_MODE             0x2
#define PCM_A_MODE          0x3
#define PCM_B_MODE          0x4
#define TDM_MODE            0x5

#define I2STDM_FIFO_DEPTH	32 /* 32 items */
#define I2STDM_FIFO_WIDTH	4 /* 32 bits */

/* common register offset */
#define BLK_MODE_SETTING	0x00
#define FRAME_SETTING		0x04
#define SLOT_SETTING1		0x08
#define SLOT_SETTING2		0x0C
#define DATA_FORMAT			0x10
#define BLK_CFG				0x14
#define I2S_ENABLE			0x18
#define I2S_RESET			0x1C
#define I2S_INT_EN			0x20
#define I2S_INT				0x24
#define FIFO_THRESHOLD		0x28
#define I2S_LRCK_MASTER		0x2C
#define FIFO_RESET			0x30
#define RX_STATUS			0x40
#define TX_STATUS			0x48
#define DMA_REQ_COUNT		0x50
#define DMA_ACK_COUNT		0x54
#define I2S_CLK_CTRL0		0x60
#define I2S_CLK_CTRL1		0x64
#define I2S_PCM_SYNTH		0x68
#define RX_RD_PORT_CH0		0x80
#define RX_RD_PORT_CH1		0x84
#define TX_WR_PORT_CH0		0xC0
#define TX_WR_PORT_CH1		0xC4

/* subsystem register offset */
#define I2S_TDM_SCLK_IN_SEL	0x000
#define I2S_TDM_FS_IN_SEL	0x004
#define I2S_TDM_SDI_IN_SEL	0x008
#define I2S_TDM_SDO_OUT_SEL	0x00C
#define I2S_TDM_MULTI_SYNC	0x020
#define I2S_BCLK_OEN_SEL	0x030


/* define value of each configuration of register BLK_MODE_SETTING */
#define RX_MODE	            (0x0 << 0)
#define TX_MODE             (0x1 << 0)
#define SLAVE_MODE	        (0x0 << 1)
#define MASTER_MODE	        (0x1 << 1)
#define RX_SAMPLE_EDGE_N    (0x0 << 2) /* Negative edge */
#define RX_SAMPLE_EDGE_P    (0x1 << 2) /* Positive edge */
#define TX_SAMPLE_EDGE_N    (0x0 << 3)
#define TX_SAMPLE_EDGE_P    (0x1 << 3)
#define FS_SAMPLE_EDGE_N    (0x0 << 4)
#define FS_SAMPLE_EDGE_P    (0x1 << 4)
#define FS_SAMPLE_RX_DELAY	(0x1 << 5)
#define FS_SAMPLE_RX_NO_DELAY	(0x0 << 5)
#define SW_MODE             (0x0 << 7)
#define HW_DMA_MODE         (0x1 << 7)
#define MULTI_I2S_SYNC      (0x1 << 8)
#define TXRX_MODE_MASK      0x00000001
#define ROLE_MASK           0x00000002
#define SAMPLE_EDGE_MASK    0x0000001C
#define FS_SAMPLE_RX_DELAY_MASK	0x00000020
#define DMA_MODE_MASK       0x00000080
#define MULTI_I2S_MODE_MASK 0x00000100

/* define value of each configuration of register FRAME_SETTING */
#define FS_POLARITY_MASK    0x00001000
#define FS_OFFSET_MASK      0x00002000
#define FS_IDEF_MASK        0x00004000
#define FS_ACT_LENGTH_MASK  0x00FF0000
#define FRAME_LENGTH_MASK   0x000001FF
#define FRAME_LENGTH(l)     (((l-1) << 0) & FRAME_LENGTH_MASK) /* frame length between 0~511 = 1~512 bits */
#define FS_ACT_LOW          (0x0 << 12)
#define FS_ACT_HIGH         (0x1 << 12)
#define NO_FS_OFFSET        (0x0 << 13)
#define FS_OFFSET_1_BIT     (0x1 << 13)
#define FS_IDEF_FRAME_SYNC	(0x0 << 14) /* frame sync*/
#define FS_IDEF_CH_SYNC		(0x1 << 14) /* channel sync */
#define FS_ACT_LENGTH(l)    (((l-1) << 16) & FS_ACT_LENGTH_MASK) /* frame active length between 0~255 = 1~256 bits*/


/* define value of each configuration of register SLOT_SETTING1 */

#define SLOT_NUM_MASK       0x0000000F
#define SLOT_SIZE_MASK      0x00003F00
#define DATA_SIZE_MASK      0x001F0000
#define FB_OFFSET_MASK      0x1F000000
#define SLOT_NUM(l)         (((l-1) << 0) & SLOT_NUM_MASK)
#define SLOT_SIZE(l)        (((l-1) << 8) & SLOT_SIZE_MASK)
#define DATA_SIZE(l)        (((l-1) << 16) & DATA_SIZE_MASK)
#define FB_OFFSET(l)        ((l << 24) & FB_OFFSET_MASK)

/* define value of each configuration of register DATA_FORMAT */
#define WORD_LENGTH_MASK    (0x3 << 1)
#define WORD_LEN_8          (0x0 << 1)
#define WORD_LEN_16         (0x1 << 1)
#define WORD_LEN_32         (0x2 << 1)
#define SKIP_RX_INACT_SLOT_MASK (0x1 << 4)
#define SKIP_RX_INACT_SLOT (0x1 << 4)
#define SKIP_TX_INACT_SLOT_MASK (0x1 << 5)
#define SKIP_TX_INACT_SLOT (0x1 << 5)

/* define value of each configuration of register I2S_RESET */
#define I2S_RESET_RX_PULL_UP 0x00000001
#define I2S_RESET_RX_PULL_DOWN 0x00000000
#define I2S_RESET_TX_PULL_UP 0x00000002
#define I2S_RESET_TX_PULL_DOWN 0x00000000

/* define value of each configuration of register I2S_INT_EN */
#define I2S_INT_EN_ALL		0x00000077

/* define value of each configuration of register I2S_INT */
#define I2S_INT_RXDA		(0x1 << 0) /* RX FIFO data available interrupt status */
#define I2S_INT_RXFO		(0x1 << 1) /* RX FIFO overflow interrupt status */
#define I2S_INT_RXFU		(0x1 << 2) /* RX FIFO underflow interrupt status */
#define I2S_INT_TXDA		(0x1 << 4) /* RX FIFO data available interrupt status */
#define I2S_INT_TXFO		(0x1 << 5) /* RX FIFO overflow interrupt status */
#define I2S_INT_TXFU		(0x1 << 6) /* RX FIFO underflow interrupt status */
#define I2S_INT_RXDA_RAW	(0x1 << 8) /* RX FIFO data available interrupt raw status */
#define I2S_INT_RXFO_RAW	(0x1 << 9) /* RX FIFO overflow interrupt raw status */
#define I2S_INT_RXFU_RAW	(0x1 << 10) /* RX FIFO underflow interrupt raw status */
#define I2S_INT_TXDA_RAW	(0x1 << 12) /* RX FIFO data available interrupt raw status */
#define I2S_INT_TXFO_RAW	(0x1 << 13) /* RX FIFO overflow interrupt raw status */
#define I2S_INT_TXFU_RAW	(0x1 << 14) /* RX FIFO underflow interrupt raw status */

/* define value of each configuration of register FIFO_THRESHOLD */
#define RX_FIFO_THRESHOLD_MASK  0x0000001F
#define TX_FIFO_THRESHOLD_MASK  0x001F0000
#define TX_FIFO_HIGH_THRESHOLD_MASK  0x1F000000
#define RX_FIFO_THRESHOLD(v)    ((v << 0) & RX_FIFO_THRESHOLD_MASK)
#define TX_FIFO_THRESHOLD(v)    ((v << 16) & TX_FIFO_THRESHOLD_MASK)
#define TX_FIFO_HIGH_THRESHOLD(v)    ((v << 24) & TX_FIFO_HIGH_THRESHOLD_MASK)

/* define value of each configuration of register FIFO_RESET */
#define RX_FIFO_RESET_PULL_UP 0x00000001
#define RX_FIFO_RESET_PULL_DOWN 0x00000000
#define TX_FIFO_RESET_PULL_UP 0x00010000
#define TX_FIFO_RESET_PULL_DOWN 0x00000000

/* define value of each configuration of register RX_STATUS */
#define RESET_RX_SCLK           0x00800000

/* define value of each configuration of register TX_STATUS */
#define RESET_TX_SCLK           0x00800000

/* define value of each configuration of register CLK_CTRL0 */
#define AUD_CLK_SOURCE_MASK     0x00000001
#define AUD_SWITCH              0x00000100
#define AUD_CLK_FROM_PLL        (0x0 << 0)
#define AUD_CLK_FROM_MCLK_IN    (0x1 << 0)
#define AUD_DISABLE             (0x0 << 8)
#define AUD_ENABLE              (0x1 << 8)
#define MCLK_OUT_EN		(0x1 << 7)
#define BCLK_OUT_FORCE_EN	(0x1 << 6)

/* define value of each configuration of register CLK_CTRL1 */
#define MCLK_DIV(l)             ((l << 0) & 0x0000FFFF)
#define BCLK_DIV(l)             ((l << 16) & 0xFFFF0000)


#define FMT_IB_NF    0x0  /* sample at falling edge and sync polarity is active low*/
#define FMT_IB_IF    0x1
#define FMT_NB_NF    0x2
#define FMT_NB_IF    0x3




#define I2S_ON	0x1
#define I2S_OFF 0x0

/* I2S Tx Control */
#define I2S_TX_ON	0x1
#define I2S_TX_OFF	0x0

/* I2S Rx Control */
#define I2S_RX_ON	0x1
#define I2S_RX_OFF	0x0

#define WSS_16_CLKCYCLE   0x20
#define WSS_24_CLKCYCLE   0x30
#define WSS_32_CLKCYCLE   0x40
#define WSS_256_CLKCYCLE  0x200



union cvi_i2s_snd_dma_data {
	struct i2s_dma_data pd;
	struct snd_dmaengine_dai_dma_data dt;
};

#ifdef CONFIG_PM_SLEEP
struct cvi_i2s_reg_context {
	u32 blk_setting;
	u32 frame_setting;
	u32 slot_setting1;
	u32 slot_setting2;
	u32 data_format;
	u32 blk_cfg;
	u32 i2s_en;
	u32 i2s_int_en;
	u32 fifo_threshold;
	u32 i2s_lrck_master;
	u32 i2s_clk_ctl0;
	u32 i2c_clk_ctl1;
	u32 i2s_pcm_synth;
};
#endif

struct cvi_i2s_dev {
	void __iomem *i2s_base;
	struct clk *clk;
	int active;
	unsigned int capability;
	unsigned int quirks;
	struct device *dev;
	u32 wss;
	u32 fifo_th;
	u32 dev_id;
	u8 mode;
	u8 sample_edge;
	u8 role;

	/* data related to DMA transfers b/w i2s and DMAC */
	union cvi_i2s_snd_dma_data play_dma_data;
	union cvi_i2s_snd_dma_data capture_dma_data;
	struct i2s_clk_config_data config;
	int (*i2s_clk_cfg)(struct i2s_clk_config_data *config);

	struct snd_pcm_substream __rcu *tx_substream;
	unsigned int (*tx_fn)(struct cvi_i2s_dev *dev,
			struct snd_pcm_runtime *runtime, unsigned int tx_ptr,
			bool *period_elapsed);
	unsigned int tx_ptr;
	bool mclk_out;
#ifdef CONFIG_PM_SLEEP
	struct cvi_i2s_reg_context *reg_ctx;
#endif
};

#if IS_ENABLED(CONFIG_SND_CV1835_PCM)
void cvi_pcm_push_tx(struct cvi_i2s_dev *dev);
int cvi_pcm_register(struct platform_device *pdev);
#else
void cvi_pcm_push_tx(struct cvi_i2s_dev *dev) { }
int cvi_pcm_register(struct platform_device *pdev)
{
	return -EINVAL;
}
#endif

#endif  /* __CVITEK_LOCAL_H__ */
