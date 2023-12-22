/*
 * Copyright (C) 2023 bitmain
 */

#ifndef __CVI_I2S_H_
#define __CVI_I2S_H_

#define I2S0    0
#define I2S1    1
#define I2S2    2
#define I2S3    3

#define CONFIG_SYS_I2S0_BASE	0x04100000
#define CONFIG_SYS_I2S1_BASE	0x04110000
#define CONFIG_SYS_I2S2_BASE	0x04120000
#define CONFIG_SYS_I2S3_BASE	0x04130000

#define CONFIG_SYS_I2S_SYS_BASE	0x04108000

#define REG_AUDIO_SRC_BASE 		0x041D0000
#define REG_AUDIO_MISC_BASE 	0x041D0C00

#define REG_AUDIO_GPIO_BASE		0x0300A12C
#define CONFIG_USE_AUDIO_PLL
#define CONFIG_SHIFT_HALF_T
//shit 1/2 T for inv, ex master generate data at falling edge and let codec sample at rising edge//

#define I2S_BITS_PER_LONG 32
#define BIT(nr)			(1UL << (nr))
#define I2S_GENMASK(h, l) \
	(((~0UL) << (l)) & (~0UL >> (I2S_BITS_PER_LONG - 1 - (h))))

struct i2s_tdm_regs {
	u32 blk_mode_setting;		/* 0x00 */
	u32 frame_setting;		/* 0x04 */
	u32 slot_setting1;		/* 0x08 */
	u32 slot_setting2;	    /* 0x0c */
	u32 data_format;	    /* 0x10 */
	u32 blk_cfg;	    /* 0x14 */
	u32 i2s_enable;	    /* 0x18 */
	u32 i2s_reset;      /* 0x1C */
	u32 i2s_int_en;	/* 0x20 */
	u32 i2s_int;	/* 0x24 */
	u32 fifo_threshold;	        /* 0x28 */
	u32 dummy1[1];	/* 0x2c */
	u32 fifo_reset;	        /* 0x30 */
	u32 dummy2[3];	/* 0x34 ~ 0x3C */
	u32 rx_status;	    /* 0x40 */
	u32 dummy3[1];	/* 0x44 */
	u32 tx_status;	    /* 0x48 */
	u32 dummy4[5];	/* 0x4c ~ 0x5c */
	u32 i2s_clk_ctrl0;		/* 0x60 */
	u32 i2s_clk_ctrl1;	/* 0x64 */
	u32 dummy5[6];		/* 0x68 ~ 0x7c */
	u32 rx_rd_port_ch0;	    /* 0x80 */
	u32 rx_rd_port_ch1;	    /* 0x84 */
	u32 dummy6[14];	    /* 0x88 ~ 0xbc */
	u32 tx_wr_port_ch0;	    /* 0xc0 */
	u32 tx_wr_port_ch1;	    /* 0xc4 */
};

struct i2s_sys_regs {
	u32 i2s_tdm_sclk_in_sel; /* 0x000 */
	u32 i2s_tdm_fs_in_sel; /* 0x004 */
	u32 i2s_tdm_sdi_in_sel; /* 0x008 */
	u32 i2s_tdm_sdo_out_sel; /* 0x00C */
	u32 dummy1[4];
	u32 i2s_tdm_multi_sync; /* 0x020 */
	u32 dummy2[3];
	u32 i2s_bclk_oen_sel; /* 0x030 */
	u32 i2s_bclk_out_ctrl; /* 0x034 */
	u32 dummy3[2];
	u32 audio_pdm_ctrl; /* 0x040 */
	u32 dummy4[3];
	u32 audio_phy_bypass1; /* 0x050 */
	u32 audio_phy_bypass2; /* 0x054 */
	u32 dummy5[6];
	u32 i2s_sys_clk_ctrl; /* 0x070 */
	u32 dummy6[3];
	u32 i2s0_master_clk_ctrl0; /* 0x080 */
	u32 i2s0_master_clk_ctrl1; /* 0x084 */
	u32 dummy7[2];
	u32 i2s1_master_clk_ctrl0; /* 0x090 */
	u32 i2s1_master_clk_ctrl1; /* 0x094 */
	u32 dummy8[2];
	u32 i2s2_master_clk_ctrl0; /* 0x0A0 */
	u32 i2s2_master_clk_ctrl1; /* 0x0A4 */
	u32 dummy9[2];
	u32 i2s3_master_clk_ctrl0; /* 0x0B0 */
	u32 i2s3_master_clk_ctrl1; /* 0x0B4 */
	u32 dummy10[2];
	u32 i2s_sys_lrck_ctrl; /* 0x0C0 */
};

struct reg_audio_misc_regs {
	u32 dummy[12];
	u32 reg_pdm_en; /* 0x30 */
	u32 reg_pdm_clk; /* 0x34 */
	u32 reg_pdm_i2s; /* 0x38 */
};

struct reg_audio_src_regs {
	u32 reg_src_en;	/* 0x00 */
	u32 reg_src_sel; /* 0x04 */
	u32 reg_src_setting; /* 0x08 */
	u32 reg_src_format; /* 0x0c */
	u32 dummy1[5];
	u32 reg_src_ratio; /* 0x24 */
	u32 reg_src_fsi_init; /* 0x28 */
	u32 dummy2[1];
	u32 reg_src_i2s_status; /* 0x30 */
	u32 dummy3[11];
	u32 reg_src_data_out; /* 0x60 */
	u32 reg_dma_th; /* 0x64 */
};

#define I2S_TIMEOUT 2000000

#define I2S_MODE            0x0
#define LJ_MODE             0x1
#define RJ_MODE             0x2
#define PCM_A_MODE          0x3
#define PCM_B_MODE          0x4
#define TDM_MODE            0x5

/* define value of each configuration of register BLK_MODE_SETTING */
#define RX_MODE	            0x0 << 0
#define TX_MODE             0x1 << 0
#define SLAVE_MODE	        0x0 << 1
#define MASTER_MODE	        0x1 << 1
#define RX_SAMPLE_EDGE_N    0x0 << 2 /* Negative edge */
#define RX_SAMPLE_EDGE_P    0x1 << 2 /* Positive edge */
#define TX_SAMPLE_EDGE_N    0x0 << 3
#define TX_SAMPLE_EDGE_P    0x1 << 3
#define FS_SAMPLE_EDGE_N    0x0 << 4
#define FS_SAMPLE_EDGE_P    0x1 << 4
#define FS_SAMPLE_RX_DELAY	0x1 << 5
#define FS_SAMPLE_RX_NO_DELAY	0x0 << 5
#define SW_MODE             0x0 << 7
#define HW_DMA_MODE         0x1 << 7
#define MULTI_I2S_SYNC      0x1 << 8
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
#define FS_ACT_LOW          0x0 << 12
#define FS_ACT_HIGH         0x1 << 12
#define NO_FS_OFFSET        0x0 << 13
#define FS_OFFSET_1_BIT     0x1 << 13
#define FS_IDEF_FRAME_SYNC  0x0 << 14 /* frame sync*/
#define FS_IDEF_I2S_LR      0x1 << 14 /* frame sync*/
#define FS_IDEF_CH_SYNC         0x1 << 14 /* channel sync */
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
#define WORD_LENGTH_MASK    0x00000006
#define WORD_LEN_8          0x0 << 1
#define WORD_LEN_16         0x1 << 1
#define WORD_LEN_32         0x2 << 1

/* define value of each configuration of register BLK_CFG */
#define AUTO_DISABLE_W_CH_EN    0x1 << 4
#define RX_START_WAIT_DMA_EN	0x1 << 6

/* define value of each configuration of register I2S_RESET */
#define I2S_RESET_RX_PULL_UP 0x00000001
#define I2S_RESET_RX_PULL_DOWN 0x00000000
#define I2S_RESET_TX_PULL_UP 0x00000002
#define I2S_RESET_TX_PULL_DOWN 0x00000000

/* define value of each configuration of register I2S_INT_EN */
#define I2S_INT_EN_ALL		0x00000177

/* define value of each configuration of register I2S_INT */
#define I2S_INT_RXDA		0x1 << 0 /* RX FIFO data available interrupt status */
#define I2S_INT_RXFO		0x1 << 1 /* RX FIFO overflow interrupt status */
#define I2S_INT_RXFU		0x1 << 2 /* RX FIFO underflow interrupt status */
#define I2S_INT_TXDA		0x1 << 4 /* RX FIFO data available interrupt status */
#define I2S_INT_TXFO		0x1 << 5 /* RX FIFO overflow interrupt status */
#define I2S_INT_TXFU		0x1 << 6 /* RX FIFO underflow interrupt status */
#define I2S_INT_RXDA_RAW	0x1 << 8 /* RX FIFO data available interrupt raw status */
#define I2S_INT_RXFO_RAW	0x1 << 9 /* RX FIFO overflow interrupt raw status */
#define I2S_INT_RXFU_RAW	0x1 << 10 /* RX FIFO underflow interrupt raw status */
#define I2S_INT_TXDA_RAW	0x1 << 12 /* RX FIFO data available interrupt raw status */
#define I2S_INT_TXFO_RAW	0x1 << 13 /* RX FIFO overflow interrupt raw status */
#define I2S_INT_TXFU_RAW	0x1 << 14 /* RX FIFO underflow interrupt raw status */

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
#define AUD_CLK_FROM_PLL        0x0 << 0
#define AUD_CLK_FROM_MCLK_IN    0x1 << 0
#define ADU_BCLK_OUT_EN		0x1 << 6
#define AUD_MCLK_OUT_EN		0x1 << 7
#define AUD_DISABLE             0x0 << 8
#define AUD_ENABLE              0x1 << 8

/* define value of each configuration of register CLK_CTRL1 */
#define MCLK_DIV(l)             ((l << 0) & 0x0000FFFF)
#define BCLK_DIV(l)             ((l << 16) & 0xFFFF0000)

#define FMT_IB_NF    0  /* sample at falling edge and sync polarity is active low*/
#define FMT_IB_IF    1
#define FMT_NB_NF    2
#define FMT_NB_IF    3

#define I2S_ON	1
#define I2S_OFF 0

/* I2S Tx Control */
#define I2S_TX_ON	1
#define I2S_TX_OFF	0

/* I2S Rx Control */
#define I2S_RX_ON	1
#define I2S_RX_OFF	0

#define WSS_16_CLKCYCLE   0x20
#define WSS_24_CLKCYCLE   0x30
#define WSS_32_CLKCYCLE   0x40
#define WSS_256_CLKCYCLE  0x200

#define AUD_SRC_EN		0x1 << 0
#define AUD_SRC_OFF		0x0 << 0

#define AUD_SRC_FSI(v)		((v << 0) & 0x000003FF)
#define AUD_SRC_FSO(v)		((v << 16) & 0x03FF0000)

/* This structure stores the i2s related information */
struct i2stx_info {
	unsigned int rfs;		/* LR clock frame size */
	unsigned int sclkg;		/* sclk gate */
	/* unsigned int audio_pll_clk;*/	/* Audio pll frequency in Hz */
	unsigned int samplingrate;	/* sampling rate */
	unsigned int mclk_out_en;
	unsigned int clk_src;
	unsigned int bitspersample;	/* bits per sample */
	unsigned int channels;		/* audio channels */
	struct i2s_tdm_regs *base_address;	/* I2S Register Base */
	struct i2s_sys_regs *sys_base_address;
	unsigned int id;		/* I2S controller id */
	unsigned char role;     /* Master mode or slave mode*/
	unsigned char slot_no;
	unsigned int inv;       /* Normal or invert BCLK, normal or invert WS CLK (FSYNC)*/
	unsigned char aud_mode; /*I2S mode, Left justified mode or Right justified mode*/
	u16 mclk_div;
	u16 bclk_div;
	u16 sync_div;
};

struct i2s_tdm_regs *i2s_get_base(unsigned int i2s_no);
struct i2s_sys_regs *i2s_get_sys_base(void);

// void i2s_set_clk_source(struct i2s_tdm_regs *i2s_reg, unsigned int src);
// void i2s_set_mclk_out_enable(struct i2s_tdm_regs *i2s_reg, unsigned int enable);
// void i2s_set_sample_rate(struct i2s_tdm_regs *i2s_reg, unsigned int sample_rate);
// void i2s_set_ws_clock_cycle(struct i2s_tdm_regs *i2s_reg, unsigned int ws_clk, u8 aud_mode);
// void i2s_set_resolution(struct i2s_tdm_regs *i2s_reg, unsigned int data_size, unsigned int slot_size);
// int i2s_set_fmt(struct i2s_tdm_regs *i2s_reg,
//                 unsigned char role,
//                 unsigned char aud_mode,
//                 unsigned int fmt,
//                 unsigned char slot_no);
// int i2s_init(struct i2stx_info *pi2s_tx, unsigned int *data);
// void i2s_loop_test(struct i2stx_info *pi2s_tx, unsigned int sec, unsigned int *data);
// void i2s_pdm_loop_test(struct i2stx_info *pi2s_tx, unsigned int sec, unsigned int *data);
// void i2s_src_test(struct i2stx_info *pi2s_tx);
// int i2s_test_rx(struct i2stx_info *pi2s_tx, unsigned int resolution);

// int i2s_receive_rx_data(struct i2stx_info *pi2s_tx, unsigned int *data);
// int i2s_transfer_tx_data(struct i2stx_info *pi2s_tx, unsigned int *data, unsigned long data_size);

// void concurrent_rx(void);

#endif /* __CVI_I2S_H_ */
