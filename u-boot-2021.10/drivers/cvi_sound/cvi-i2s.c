#include <common.h>
#include <dm.h>
#include <i2s.h>
#include <log.h>
#include <time.h>
#include <dm/device.h>
#include <asm/io.h>
#include <linux/delay.h>
#include "cvi-i2s.h"

#define AMP_PWR_GPIO_BASE 0x05021000
#define AMP_PWR_GPIO_INOUT 0x004
#define AMP_PWR_GPIO_PN 0x000

void i2s_reg_debug(struct i2s_tdm_regs *i2s_reg, struct i2s_sys_regs *i2s_sys_reg)
{
#ifdef __DEBUG__
	printf("[i2s_tdm_reg]:\n");
	printf("blk_mode_setting:0x%x\n", readl(&i2s_reg->blk_mode_setting));
	printf("frame_setting:0x%x\n", readl(&i2s_reg->frame_setting));
	printf("slot_setting1:0x%x\n", readl(&i2s_reg->slot_setting1));
	printf("slot_setting2:0x%x\n", readl(&i2s_reg->slot_setting2));
	printf("data_format:0x%x\n", readl(&i2s_reg->data_format));
	printf("blk_cfg:0x%x\n", readl(&i2s_reg->blk_cfg));

	printf("i2s_enable:0x%x\n", readl(&i2s_reg->i2s_enable));
	printf("i2s_reset:0x%x\n", readl(&i2s_reg->i2s_reset));
	printf("i2s_int_en:0x%x\n", readl(&i2s_reg->i2s_int_en));
	printf("i2s_int:0x%x\n", readl(&i2s_reg->i2s_int));
	printf("fifo_threshold:0x%x\n", readl(&i2s_reg->fifo_threshold));
	printf("fifo_reset:0x%x\n", readl(&i2s_reg->fifo_reset));

	printf("i2s_clk_ctrl0:0x%x\n", readl(&i2s_reg->i2s_clk_ctrl0));
	printf("i2s_clk_ctrl1:0x%x\n", readl(&i2s_reg->i2s_clk_ctrl1));

	printf("[i2s_sys_regs][0x%p]:\n", i2s_sys_reg);

	printf("i2s_tdm_sclk_in_sel:0x%x\n", i2s_sys_reg->i2s_tdm_sclk_in_sel);
	printf("i2s_tdm_fs_in_sel:0x%x\n", i2s_sys_reg->i2s_tdm_fs_in_sel);
	printf("i2s_tdm_sdi_in_sel:0x%x\n", i2s_sys_reg->i2s_tdm_sdi_in_sel);
	printf("i2s_tdm_sdo_out_sel:0x%x\n", i2s_sys_reg->i2s_tdm_sdo_out_sel);
#endif
}

static void i2s_write_reg(volatile u32 *addr, u32 val)
{
	writel(val, addr);
}

static void muteamp(bool enable)
{
	debug("[%s]IN/OUT:0x%x, P/N:0x%x\n", __func__,
		readl((volatile u32 *)(AMP_PWR_GPIO_BASE + AMP_PWR_GPIO_INOUT)),
		readl((volatile u32 *)(AMP_PWR_GPIO_BASE + AMP_PWR_GPIO_PN)));
	if (enable) {//0

		i2s_write_reg((volatile u32 *)(AMP_PWR_GPIO_BASE + AMP_PWR_GPIO_INOUT), 0);//OUT
		i2s_write_reg((volatile u32 *)(AMP_PWR_GPIO_BASE + AMP_PWR_GPIO_PN), 0);//DOWN
	} else {//1

		i2s_write_reg((volatile u32 *)(AMP_PWR_GPIO_BASE + AMP_PWR_GPIO_INOUT), 4);//OUT
		i2s_write_reg((volatile u32 *)(AMP_PWR_GPIO_BASE + AMP_PWR_GPIO_PN), 4);//DOWN
	}
	debug("[%s]IN/OUT:0x%x, P/N:0x%x\n", __func__,
		readl((volatile u32 *)(AMP_PWR_GPIO_BASE + AMP_PWR_GPIO_INOUT)),
		readl((volatile u32 *)(AMP_PWR_GPIO_BASE + AMP_PWR_GPIO_PN)));
}

void i2s_set_clk_source(struct i2s_tdm_regs *i2s_reg, unsigned int src)
{
	u32 tmp = readl(&i2s_reg->i2s_clk_ctrl0) & ~(AUD_CLK_SOURCE_MASK);

	switch (src) {
	case AUD_CLK_FROM_MCLK_IN:
		tmp |= AUD_CLK_FROM_MCLK_IN | AUD_ENABLE;
		break;
	case AUD_CLK_FROM_PLL:
		tmp |= AUD_CLK_FROM_PLL | AUD_ENABLE;
		break;
	}

	i2s_write_reg(&i2s_reg->i2s_clk_ctrl0, tmp);
	debug("[%s]Set clk source from %d\n", __func__, src);
}

void i2s_set_interrupt(struct i2s_tdm_regs *i2s_reg)
{
	i2s_write_reg(&i2s_reg->i2s_int_en, I2S_INT_EN_ALL); /* enable all interrupt mask for TX and RX */
}

void i2s_set_sample_rate(struct i2s_tdm_regs *i2s_reg, unsigned int sample_rate)
{
	u32 frame_setting = 0;
	u32 slot_setting = 0;
	u32 data_format = 0;
	u32 clk_ctrl = 0;
	u32 div_multiplier = 1; /* if use audio PLL (25 or 24.576Mhz), div_multiplier should be 2 */

	debug("[%s]Set sample rate to %d\n", __func__, sample_rate);

	frame_setting = readl(&i2s_reg->frame_setting);
	slot_setting = readl(&i2s_reg->slot_setting1);
	data_format = readl(&i2s_reg->data_format);
	clk_ctrl = readl(&i2s_reg->i2s_clk_ctrl1);

	frame_setting &= ~(FRAME_LENGTH_MASK | FS_ACT_LENGTH_MASK);
	slot_setting &= ~(SLOT_SIZE_MASK | DATA_SIZE_MASK);
	data_format &= ~(WORD_LENGTH_MASK);
#if defined(CONFIG_USE_AUDIO_PLL)
	clk_ctrl = MCLK_DIV(2); /* audio PLL is 25 or 24.576 Mhz, need to div with 2*/
	div_multiplier = 1;
#else
	clk_ctrl = MCLK_DIV(1); /* mclk_in is 12.288 Mhz, no need to div*/
#endif

		switch (sample_rate) {
		case 8000:
			frame_setting |= FRAME_LENGTH(32) | FS_ACT_LENGTH(16);
			slot_setting |= SLOT_SIZE(16) | DATA_SIZE(16);
			data_format = WORD_LEN_16;
			clk_ctrl |= BCLK_DIV(48 * div_multiplier);
			break;
		case 12000:
			frame_setting |= FRAME_LENGTH(64) | FS_ACT_LENGTH(32);
			slot_setting |= SLOT_SIZE(32) | DATA_SIZE(32);
			data_format = WORD_LEN_32;
			clk_ctrl |= BCLK_DIV(16 * div_multiplier);
			break;
		case 16000:
			frame_setting |= FRAME_LENGTH(32) | FS_ACT_LENGTH(16);
			slot_setting |= SLOT_SIZE(16) | DATA_SIZE(16);
			data_format = WORD_LEN_16;
			//clk_ctrl |= BCLK_DIV(12 * div_multiplier);
			clk_ctrl |= BCLK_DIV(24 * div_multiplier);
			break;
		case 24000:
			frame_setting |= FRAME_LENGTH(64) | FS_ACT_LENGTH(32);
			slot_setting |= SLOT_SIZE(32) | DATA_SIZE(32);
			data_format = WORD_LEN_32;
			clk_ctrl |= BCLK_DIV(8 * div_multiplier);
			break;
		case 32000:
			frame_setting |= FRAME_LENGTH(64) | FS_ACT_LENGTH(32);
			slot_setting |= SLOT_SIZE(32) | DATA_SIZE(32);
			data_format = WORD_LEN_32;
			clk_ctrl |= BCLK_DIV(6 * div_multiplier);
			break;
		case 48000:
			frame_setting |= FRAME_LENGTH(64) | FS_ACT_LENGTH(32);
			slot_setting |= SLOT_SIZE(32) | DATA_SIZE(32);
			data_format = WORD_LEN_32;
			clk_ctrl |= BCLK_DIV(4 * div_multiplier);
			break;
		case 96000:
			frame_setting |= FRAME_LENGTH(64) | FS_ACT_LENGTH(32);
			slot_setting |= SLOT_SIZE(32) | DATA_SIZE(32);
			data_format = WORD_LEN_32;
			clk_ctrl |= BCLK_DIV(2 * div_multiplier);
			break;
		case 192000:
			frame_setting |= FRAME_LENGTH(64) | FS_ACT_LENGTH(32);
			slot_setting |= SLOT_SIZE(32) | DATA_SIZE(32);
			data_format = WORD_LEN_32;
			clk_ctrl |= BCLK_DIV(1 * div_multiplier);
			break;
		}

	 i2s_write_reg(&i2s_reg->frame_setting, frame_setting);
	 i2s_write_reg(&i2s_reg->slot_setting1, slot_setting);
	 i2s_write_reg(&i2s_reg->data_format, data_format);
	 i2s_write_reg(&i2s_reg->i2s_clk_ctrl1, clk_ctrl);
}

int i2s_set_fmt(struct i2s_tdm_regs *i2s_reg,
				unsigned char role,
				unsigned char aud_mode,
				unsigned int fmt,
				unsigned char slot_no)
{
	unsigned int tmp = 0;
	unsigned int tmp2 = 0;
	//unsigned int codec_fmt = 0;

	tmp = readl(&i2s_reg->frame_setting) & ~(FS_OFFSET_MASK | FS_IDEF_MASK | FS_ACT_LENGTH_MASK);
	tmp2 = readl(&i2s_reg->slot_setting1) & ~(SLOT_NUM_MASK);

	switch (aud_mode) {
	case I2S_MODE:
		tmp |= FS_OFFSET_1_BIT | FS_IDEF_I2S_LR | FS_ACT_LENGTH(((tmp & FRAME_LENGTH_MASK) + 1) / 2);
		i2s_write_reg(&i2s_reg->frame_setting, tmp);
		tmp2 |= SLOT_NUM(slot_no);
		i2s_write_reg(&i2s_reg->slot_setting1, tmp2);
		//codec_fmt |= SND_SOC_DAIFMT_I2S;
		break;
	case LJ_MODE:
		tmp |= NO_FS_OFFSET | FS_IDEF_FRAME_SYNC | FS_ACT_LENGTH(((tmp & FRAME_LENGTH_MASK) + 1) / 2);
		i2s_write_reg(&i2s_reg->frame_setting, tmp);
		tmp2 |= SLOT_NUM(slot_no);
		i2s_write_reg(&i2s_reg->slot_setting1, tmp2);
		//codec_fmt |= SND_SOC_DAIFMT_LEFT_J;
		break;
	case RJ_MODE:
		tmp |= (NO_FS_OFFSET | FS_IDEF_FRAME_SYNC |
			FS_ACT_LENGTH(((tmp & FRAME_LENGTH_MASK) + 1) / 2));
		i2s_write_reg(&i2s_reg->frame_setting, tmp);
		tmp2 &= ~(FB_OFFSET_MASK);
		tmp2 |= (SLOT_NUM(slot_no) |
			FB_OFFSET((((tmp & FS_ACT_LENGTH_MASK) >> 16) - ((tmp2 & DATA_SIZE_MASK) >> 16))));
		i2s_write_reg(&i2s_reg->slot_setting1, tmp2);
		//codec_fmt |= SND_SOC_DAIFMT_RIGHT_J;
		break;
	case PCM_A_MODE:
		tmp |= FS_OFFSET_1_BIT | FS_IDEF_FRAME_SYNC | FS_ACT_LENGTH(1);
		i2s_write_reg(&i2s_reg->frame_setting, tmp);
		tmp2 |= SLOT_NUM(slot_no);
		i2s_write_reg(&i2s_reg->slot_setting1, tmp2);
		//codec_fmt |= SND_SOC_DAIFMT_DSP_A;
		break;
	case PCM_B_MODE:
		tmp |= NO_FS_OFFSET | FS_IDEF_FRAME_SYNC | FS_ACT_LENGTH(1);
		i2s_write_reg(&i2s_reg->frame_setting, tmp);
		tmp2 |= SLOT_NUM(slot_no);
		i2s_write_reg(&i2s_reg->slot_setting1, tmp2);
		//codec_fmt |= SND_SOC_DAIFMT_DSP_B;
		break;
	case TDM_MODE:
		tmp |= NO_FS_OFFSET | FS_IDEF_FRAME_SYNC | FS_ACT_LENGTH(1);
		i2s_write_reg(&i2s_reg->frame_setting, tmp);
		tmp2 |= SLOT_NUM(slot_no);
		i2s_write_reg(&i2s_reg->slot_setting1, tmp2);
		i2s_write_reg(&i2s_reg->slot_setting2, 0x0f); /* enable slot 0-3 for TDM */
		//codec_fmt |= SND_SOC_DAIFMT_PDM;
		break;
	default:
		log_err("%s: Invalid format\n", __func__);
		return -1;
	}

	tmp = readl(&i2s_reg->blk_mode_setting) &
			~(SAMPLE_EDGE_MASK | FS_SAMPLE_RX_DELAY_MASK); /* clear bit 2~4 to set frame format */
	tmp2 = readl(&i2s_reg->frame_setting) & ~(FS_POLARITY_MASK); /* clear bit 12 to set fs polarity */
	if ((aud_mode == I2S_MODE) || (aud_mode == LJ_MODE) || (aud_mode == RJ_MODE)) {
		switch (fmt) {
		case FMT_IB_NF:
			debug("Set format to IBNF\n");
#ifdef CONFIG_SHIFT_HALF_T
			// if (concurrent_rx_enable == true)
			// 	tmp |= RX_SAMPLE_EDGE_N | TX_SAMPLE_EDGE_P; /* for crx */
			// else
				tmp |= RX_SAMPLE_EDGE_P | TX_SAMPLE_EDGE_N;
#else
			tmp |= RX_SAMPLE_EDGE_N | TX_SAMPLE_EDGE_N;
#endif
			i2s_write_reg(&i2s_reg->blk_mode_setting, tmp);
			tmp2 |= FS_ACT_LOW;
			i2s_write_reg(&i2s_reg->frame_setting, tmp2);
			//codec_fmt |= SND_SOC_DAIFMT_IB_NF;
			break;
		case FMT_IB_IF:
			debug("Set format to IBIF\n");
#ifdef CONFIG_SHIFT_HALF_T
			//if (concurrent_rx_enable == true)
			//	tmp |= RX_SAMPLE_EDGE_N | TX_SAMPLE_EDGE_P; /* for crx */
			//else
				tmp |= RX_SAMPLE_EDGE_P | TX_SAMPLE_EDGE_N;
#else
			tmp |= RX_SAMPLE_EDGE_N | TX_SAMPLE_EDGE_N;
#endif
			i2s_write_reg(&i2s_reg->blk_mode_setting, tmp);
			tmp2 |= FS_ACT_HIGH;
			i2s_write_reg(&i2s_reg->frame_setting, tmp2);
			//codec_fmt |= SND_SOC_DAIFMT_IB_IF;
			break;
		case FMT_NB_NF:
			debug("Set format to NBNF\n");
#ifdef CONFIG_SHIFT_HALF_T
			//if (concurrent_rx_enable == true)
			//	tmp |= RX_SAMPLE_EDGE_P | TX_SAMPLE_EDGE_N; /* for crx  */
			//else
				tmp |= RX_SAMPLE_EDGE_N | TX_SAMPLE_EDGE_P;
#else
			tmp |= RX_SAMPLE_EDGE_P | TX_SAMPLE_EDGE_P;
#endif
		i2s_write_reg(&i2s_reg->blk_mode_setting, tmp);
		tmp2 |= FS_ACT_LOW;
		i2s_write_reg(&i2s_reg->frame_setting, tmp2);
		//codec_fmt |= SND_SOC_DAIFMT_NB_NF;
		break;
		case FMT_NB_IF:
			debug("Set format to NBIF\n");
#ifdef CONFIG_SHIFT_HALF_T
			//if (concurrent_rx_enable == true)
			//	tmp |= RX_SAMPLE_EDGE_P | TX_SAMPLE_EDGE_N; /* for crx */
			//else
				tmp |= RX_SAMPLE_EDGE_N | TX_SAMPLE_EDGE_P;
#else
			tmp |= RX_SAMPLE_EDGE_P | TX_SAMPLE_EDGE_P;
#endif
			i2s_write_reg(&i2s_reg->blk_mode_setting, tmp);
			tmp2 |= FS_ACT_HIGH;
			i2s_write_reg(&i2s_reg->frame_setting, tmp2);
			//codec_fmt |= SND_SOC_DAIFMT_NB_IF;
			break;
		default:
			log_err("%s: Invalid clock ploarity input\n", __func__);
			return -1;
		}
	} else {
#ifdef CONFIG_SHIFT_HALF_T
		if (role == MASTER_MODE)
			tmp |= RX_SAMPLE_EDGE_P | TX_SAMPLE_EDGE_N;
		else
			tmp |= RX_SAMPLE_EDGE_N | TX_SAMPLE_EDGE_N;
#else
		tmp |= RX_SAMPLE_EDGE_P | TX_SAMPLE_EDGE_P;
#endif
		i2s_write_reg(&i2s_reg->blk_mode_setting, tmp);
		tmp2 |= FS_ACT_HIGH;
		i2s_write_reg(&i2s_reg->frame_setting, tmp2);
		//codec_fmt |= SND_SOC_DAIFMT_IB_IF;
	}

	tmp=readl(&i2s_reg->blk_mode_setting) & ~(ROLE_MASK); /* clear bit 2~4 to set frame format */
	switch (role) {
	case MASTER_MODE:
		tmp |= MASTER_MODE;
		i2s_write_reg(&i2s_reg->blk_mode_setting, tmp);
		//i2s_set_audio_gpio(MASTER_MODE);
		//codec_fmt |= SND_SOC_DAIFMT_CBS_CFS; /* Set codec to slave */
		debug("master:%s\n", __func__);
		break;
	case SLAVE_MODE:
		tmp |= SLAVE_MODE;
		i2s_write_reg(&i2s_reg->blk_mode_setting, tmp);
		//i2s_set_audio_gpio(SLAVE_MODE);
		//codec_fmt |= SND_SOC_DAIFMT_CBM_CFM; /* Set codec to master*/
		debug("slave:%s\n", __func__);
		break;
	default:
		log_err("%s: Invalid master selection\n", __func__);
		return -1;
	}

	// printf("Set codes fmt\n");
	// if (skip_codec_setting == false)
	// 	adau1372_set_dai_fmt(codec_fmt);
	printf("[%s]end\n", __func__);
	return 0;
}

void i2s_config_dma(struct i2s_tdm_regs *i2s_reg, bool on)
{
	u32 blk_mode_setting = 0;
	//u32 blk_cfg = 0;

	blk_mode_setting = readl(&i2s_reg->blk_mode_setting) & ~(DMA_MODE_MASK);
	if (on == true) {
		i2s_write_reg(&i2s_reg->blk_mode_setting, blk_mode_setting | HW_DMA_MODE); /*not to use FIFO */
		i2s_write_reg(&i2s_reg->fifo_threshold,
			(RX_FIFO_THRESHOLD(15) | TX_FIFO_THRESHOLD(15) | TX_FIFO_HIGH_THRESHOLD(31)));
	} else {
		i2s_write_reg(&i2s_reg->blk_mode_setting, blk_mode_setting | SW_MODE); /*not to use FIFO */
		i2s_write_reg(&i2s_reg->fifo_threshold,
			(RX_FIFO_THRESHOLD(1) | TX_FIFO_THRESHOLD(15) | TX_FIFO_HIGH_THRESHOLD(31)));
	}

	//blk_cfg = readl(&i2s_reg->blk_cfg);
	//printf("[%s]blk_cfg:0x%x\n", __func__, readl(&i2s_reg->blk_cfg));

}

static int cvitekub_i2s_init(struct i2s_uc_priv *priv)
{
	int ret = 0;
	struct i2s_tdm_regs *i2s_reg = (struct i2s_tdm_regs *)(uintptr_t)priv->base_address;

	i2s_set_clk_source(i2s_reg, AUD_CLK_FROM_PLL);
	i2s_set_interrupt(i2s_reg);
	/* disable i2s transfer flag and flush the fifo */
	i2s_set_sample_rate(i2s_reg, priv->samplingrate); /* sample rate must first prior to fmt */
	ret = i2s_set_fmt(i2s_reg, MASTER_MODE, I2S_MODE, FMT_IB_IF, priv->channels);
	if (ret != 0) {
		log_err("%s:set format failed\n", __func__);
		return -1;
	}

	i2s_config_dma(i2s_reg, false);
	return 0;
}

void i2s_switch(int on, struct i2s_tdm_regs *i2s_reg)
{
	u32 i2s_enable = readl(&i2s_reg->i2s_enable);
	u32 aud_enable = readl(&i2s_reg->i2s_clk_ctrl0);
	u32 role = (readl(&i2s_reg->blk_mode_setting) & ROLE_MASK);

	if (on) {
		if (i2s_enable == I2S_OFF)
			i2s_write_reg(&i2s_reg->i2s_enable, I2S_ON);
	} else {

		if (i2s_enable == I2S_ON)
			i2s_write_reg(&i2s_reg->i2s_enable, I2S_OFF);

		if (((aud_enable & AUD_ENABLE) == AUD_ENABLE) && role == MASTER_MODE)
			i2s_write_reg(&i2s_reg->i2s_clk_ctrl0, aud_enable & ~(AUD_ENABLE));
	}
}


void i2s_txctrl(struct i2s_tdm_regs *i2s_reg, int on)
{
	u32 blk_mode_setting = 0;
	u32 clk_ctrl = 0;

	blk_mode_setting = (readl(&i2s_reg->blk_mode_setting) & ~(TXRX_MODE_MASK));
	clk_ctrl = (readl(&i2s_reg->i2s_clk_ctrl0) & ~(AUD_SWITCH));

	blk_mode_setting |= TX_MODE;
	i2s_write_reg(&i2s_reg->blk_mode_setting, blk_mode_setting);
	debug("txctrl:%p, 0x%x\n", i2s_reg, readl(&i2s_reg->blk_mode_setting));
	if ((blk_mode_setting & ROLE_MASK) == MASTER_MODE) {
		if (on) {
			i2s_write_reg(&i2s_reg->i2s_clk_ctrl0, clk_ctrl | AUD_ENABLE);
			debug("Enable aud_en 0x%x\n", i2s_reg->i2s_clk_ctrl0);
		} else {
			debug("Disalbe aud_en\n");
			i2s_write_reg(&i2s_reg->i2s_clk_ctrl0, clk_ctrl & ~(AUD_ENABLE));
		}
	} else {
		i2s_write_reg(&i2s_reg->i2s_clk_ctrl0, clk_ctrl & ~(AUD_ENABLE));
	}

}

void i2s_sw_reset(struct i2s_tdm_regs *i2s_reg)
{

	if ((readl(&i2s_reg->blk_mode_setting) & TXRX_MODE_MASK) == TX_MODE) {
		//debug("Reset i2s TX\n");
		i2s_write_reg(&i2s_reg->fifo_reset, TX_FIFO_RESET_PULL_UP);
		udelay(10);
		i2s_write_reg(&i2s_reg->fifo_reset, TX_FIFO_RESET_PULL_DOWN);
		i2s_write_reg(&i2s_reg->i2s_reset, I2S_RESET_TX_PULL_UP);
		ulong start = timer_get_us() + I2S_TIMEOUT;

		udelay(10);
		while (1) {
			printf("[debug]tx_status:%04X\n", readl(&i2s_reg->tx_status));
			if ((readl(&i2s_reg->tx_status) & RESET_TX_SCLK) >> 23) {
				printf("TX Reset complete\n");
				break;
			} else if ((long)(timer_get_us() - start) > 0) {
				printf("TX Reset Timeout\n");
				break;
			}
		}
		 i2s_write_reg(&i2s_reg->i2s_reset, I2S_RESET_TX_PULL_DOWN);

	} else { /* reset RX*/
		//debug("Reset i2s RX\n");
		i2s_write_reg(&i2s_reg->fifo_reset, RX_FIFO_RESET_PULL_UP);
		udelay(10);
		i2s_write_reg(&i2s_reg->fifo_reset, RX_FIFO_RESET_PULL_DOWN);
		i2s_write_reg(&i2s_reg->i2s_reset, I2S_RESET_RX_PULL_UP);
		ulong start = timer_get_us() + I2S_TIMEOUT;

		udelay(10);
		while (1) {
			u32 tmp = readl(&i2s_reg->rx_status);
			u32 tmp2 = readl(&i2s_reg->i2s_clk_ctrl0);
			printf("rx_status=0x%x, clk_ctrl0=0x%x\n", tmp, tmp2);
			if ((tmp & RESET_RX_SCLK) >> 23) {
				//debug("RX Reset complete\n");
				break;
			} else if ((long)(timer_get_us() - start) > 0) {
				printf("RX Reset Timeout\n");
				break;
			}
		}
		 i2s_write_reg(&i2s_reg->i2s_reset, I2S_RESET_RX_PULL_DOWN);
	}
}

static int i2s_send_data(struct i2s_tdm_regs *i2s_reg, const void *byte_data, int nbytes)
{
	struct i2s_sys_regs *i2s_sys_reg = (struct i2s_sys_regs *)CONFIG_SYS_I2S_SYS_BASE;
	u32 *send_data = (u32 *)byte_data;
	int frame_num = nbytes / 4;//2chn 16bit
	int i = 0;

	//u32 overrun = 0;
	u32 fifo_wb = 0;
	u32 fifo_depth = 16;
	u32 cycle_cnt = fifo_depth;
	u32 remainder = frame_num % cycle_cnt;
	u32 integer = frame_num / cycle_cnt + 1;

	//printf("%s fifo_depth = %d , frame_num = %d(%d)(%d), send_data = %p, i2s_reg = %p\n",
	//	__func__, fifo_depth, frame_num, integer, remainder, send_data, i2s_reg);

	i2s_txctrl(i2s_reg, I2S_TX_ON);
	i2s_sw_reset(i2s_reg);
	i2s_switch(I2S_ON, i2s_reg);
	muteamp(false);

	while (integer) {
		//overrun = readl(&i2s_reg->i2s_int);
		//if (((overrun & I2S_INT_TXDA) == I2S_INT_TXDA)
			//|| ((overrun & I2S_INT_TXDA_RAW) == I2S_INT_TXDA_RAW)) {
			// Write 1 to clear.
		   // i2s_write_reg(&i2s_reg->i2s_int, overrun | I2S_INT_TXDA | I2S_INT_TXDA_RAW);
		fifo_wb = readl((void *)0x0413004c) & 0x3f;

		if (fifo_wb > 0xf) {
			if (fifo_wb == 0x20)
				printf("u\n");

			if (integer > 1) {
				for (i = 0; i < cycle_cnt / 2; i++) {
					i2s_write_reg(&i2s_reg->tx_wr_port_ch0, *send_data);
					send_data++;
					i2s_write_reg(&i2s_reg->tx_wr_port_ch0, *send_data);
					send_data++;
				}

			} else if (integer == 1) {
				for (i = 0; i < remainder; i++) {
					i2s_write_reg(&i2s_reg->tx_wr_port_ch0, *send_data);
					send_data++;
				}

			}
			integer--;
			//udelay(10);//100(u)- 70(0)
		} else if (fifo_wb == 0) {
			//printf("o:%x\n", readl(&i2s_reg->i2s_int));
		}

	}

	muteamp(true);
	i2s_reg_debug(i2s_reg, i2s_sys_reg);
	i2s_txctrl(i2s_reg, I2S_TX_OFF);
	i2s_switch(I2S_OFF, i2s_reg);

	printf("%s end\n", __func__);
	return 0;
}

static int cvitekub_i2s_tx_data(struct udevice *dev, void *data, uint data_size)
{
	struct i2s_uc_priv *priv = dev_get_uclass_priv(dev);
	struct i2s_tdm_regs *i2s_reg = (struct i2s_tdm_regs *)(uintptr_t)priv->base_address;

	return i2s_send_data(i2s_reg, data, data_size);
}

static int cvitekub_i2s_probe(struct udevice *dev)
{
	struct i2s_uc_priv *priv = dev_get_uclass_priv(dev);

	priv->base_address = CONFIG_SYS_I2S3_BASE;
	priv->id = 1;
	priv->audio_pll_clk = 24.576 * 1000 * 1000;
	priv->samplingrate = 16000;
	priv->bitspersample = 16;
	priv->channels = 2;
	priv->rfs = 64;
	priv->bfs = 32;
	debug("cvitekub_i2s_probe\n");
	return cvitekub_i2s_init(priv);
}

static const struct i2s_ops cvitekub_i2s_ops = {
	.tx_data	= cvitekub_i2s_tx_data,
};

static const struct udevice_id cvitekub_i2s_ids[] = {
	{ .compatible = "cvitek,cv1835-i2s" },
	{ }
};

U_BOOT_DRIVER(cvitekub_i2s) = {
	.name		= "cvitekub_i2s",
	.id		= UCLASS_I2S,
	.of_match	= cvitekub_i2s_ids,
	.probe		= cvitekub_i2s_probe,
	.ops		= &cvitekub_i2s_ops,
	//.priv_auto	= sizeof(struct broadwell_i2s_priv),
};
