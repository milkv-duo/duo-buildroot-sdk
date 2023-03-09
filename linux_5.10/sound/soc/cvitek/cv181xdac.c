// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * CVITEK CV181X DAC driver
 *
 * Copyright 2020 CVITEK Inc.
 *
 */

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/pm.h>
#include <linux/mutex.h>
#include <linux/miscdevice.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/control.h>
#include "cv1835_ioctl.h"
#include "../codecs/cv181xadac.h"
#include "cv1835_i2s_subsys.h"
#include <linux/gpio.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_gpio.h>

static DEFINE_MUTEX(cv181xdac_mutex);
int mute_pin_l; // 495
int mute_pin_r; // 510


void muteAmp(bool enable)
{
	if (enable) {
		if (mute_pin_l != -EINVAL) {
			gpio_set_value(mute_pin_l, 0);
		}
		if (mute_pin_r != -EINVAL) {
			gpio_set_value(mute_pin_r, 0);
		}
	} else {
		if (mute_pin_l != -EINVAL) {
			gpio_set_value(mute_pin_l, 1);
		}
		if (mute_pin_r != -EINVAL) {
			gpio_set_value(mute_pin_r, 1);
		}
	}
}

static inline void dac_write_reg(void __iomem *io_base, int reg, u32 val)
{
	writel(val, io_base + reg);
}

static inline u32 dac_read_reg(void __iomem *io_base, int reg)
{
	return readl(io_base + reg);
}

static int dac_open(struct inode *inode, struct file *file)
{
	if (mutex_lock_interruptible(&cv181xdac_mutex))
		return -EINTR;
	mutex_unlock(&cv181xdac_mutex);
	pr_debug("%s\n", __func__);
	return 0;
}

static int dac_close(struct inode *inode, struct file *file)
{
	if (mutex_lock_interruptible(&cv181xdac_mutex))
		return -EINTR;
	mutex_unlock(&cv181xdac_mutex);
	pr_debug("%s\n", __func__);
	return 0;
}


static int cv181xdac_set_dai_fmt(struct snd_soc_dai *dai,
					unsigned int fmt)
{

	struct cv181xdac *dac = snd_soc_dai_get_drvdata(dai);

	if (!dac->dev)
		dev_err(dac->dev, "dev is NULL\n");


	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		dev_err(dac->dev, "Cannot set DAC to MASTER mode\n");
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		dev_dbg(dac->dev, "Set DAC to SLAVE mode\n");
		break;
	default:
		dev_err(dac->dev, "Cannot support this role mode\n");
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		dev_dbg(dac->dev, "set codec to NB_NF\n");
		break;
	case SND_SOC_DAIFMT_IB_NF:
		dev_dbg(dac->dev, "set codec to IB_NF\n");
		break;
	case SND_SOC_DAIFMT_IB_IF:
		dev_dbg(dac->dev, "set codec to IB_IF\n");
		break;
	case SND_SOC_DAIFMT_NB_IF:
		dev_dbg(dac->dev, "set codec to NB_IF\n");
		break;
	default:
		dev_err(dac->dev, "Cannot support this format\n");
		break;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		dev_dbg(dac->dev, "set codec to I2S mode\n");
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		dev_dbg(dac->dev, "set codec to LEFT-JUSTIFY mode\n");
		break;
	default:
		dev_err(dac->dev, "Cannot support this mode\n");
		break;
	}
	return 0;
}

static int cv181xdac_hw_params(struct snd_pcm_substream *substream,
				  struct snd_pcm_hw_params *params,
				  struct snd_soc_dai *dai)
{
	struct cv181xdac *dac = snd_soc_dai_get_drvdata(dai);
	int rate;
	u32 ctrl1 = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL1) & ~AUDIO_PHY_REG_TXDAC_CIC_OPT_MASK;
	u32 tick = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE0) & ~AUDIO_PHY_REG_TXDAC_INIT_DLY_CNT_MASK;
	u32 chan_nr = 0;
	u32 ana2 = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);

	chan_nr = params_channels(params);

	switch (chan_nr) {
	case 1:
		//ana2 |= AUDIO_PHY_REG_DA_DEMR_TXDAC_OW_EN_ON; /* turn R-channel off */
		ana2 &= AUDIO_PHY_REG_DA_DEMR_TXDAC_OW_EN_OFF; /* turn R-channel on */
		dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, ana2);
		break;
	default:
		ana2 &= AUDIO_PHY_REG_DA_DEMR_TXDAC_OW_EN_OFF; /* turn R-channel on */
		dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, ana2);
		break;
	}

	rate = params_rate(params);
	if (rate >= 8000 && rate <= 48000) {
		dev_dbg(dac->dev, "dac_hw_params, set rate to %d\n", rate);

		switch (rate) {
		case 8000:
			ctrl1 |= TXDAC_CIC_DS_512;
			tick |= 0x21;
			break;
		case 11025:
			ctrl1 |= TXDAC_CIC_DS_256;
			tick |= 0x17;
			break;
		case 16000:
			ctrl1 |= TXDAC_CIC_DS_256;
			tick |= 0x21;
			break;
		case 22050:
			ctrl1 |= TXDAC_CIC_DS_128;
			tick |= 0x17;
			break;
		case 32000:
			ctrl1 |= TXDAC_CIC_DS_128;
			tick |= 0x21;
			break;
		case 44100:
			ctrl1 &= TXDAC_CIC_DS_64;
			tick |= 0x17;
			break;
		case 48000:
			ctrl1 &= TXDAC_CIC_DS_64;
			tick |= 0x19;
			break;
		default:
			ctrl1 |= TXDAC_CIC_DS_256;
			tick |= 0x21;
			dev_dbg(dac->dev, "dac_hw_params, set sample rate with default 16KHz\n");
			break;
		}
	} else {
		dev_err(dac->dev, "dac_hw_params, unsupported sample rate\n");
		return 0;
	}
	dev_dbg(dac->dev, "dac_hw_params, ctrl1=0x%x\n", ctrl1);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL1, ctrl1);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE0, tick);

	return 0;
}

static int cv181xdac_startup(struct snd_pcm_substream *substream,
		struct snd_soc_dai *dai)
{
	return 0;
}

static void cv181xdac_on(struct cv181xdac *dac)
{

	u32 val = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0);

	dev_dbg(dac->dev, "dac_on, before ctrl0_reg val=0x%08x\n", val);

	if ((val & AUDIO_PHY_REG_TXDAC_EN_ON) | (val & AUDIO_PHY_REG_I2S_RX_EN_ON))
		dev_info(dac->dev, "DAC already switched ON!!, val=0x%08x\n", val);

	val |= AUDIO_PHY_REG_TXDAC_EN_ON | AUDIO_PHY_REG_I2S_RX_EN_ON;
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0, val);

	dev_dbg(dac->dev, "dac_on, after ctrl0_reg val=0x%08x\n",
	dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0));

}

static void cv181xdac_off(struct cv181xdac *dac)
{
	u32 val = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0);

	dev_dbg(dac->dev, "dac_off, before ctrl_reg val=0x%08x\n",
	 dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0));

	val &= AUDIO_PHY_REG_TXDAC_EN_OFF & AUDIO_PHY_REG_I2S_RX_EN_OFF;
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0, val);

	dev_dbg(dac->dev, "dac_off, after ctrl_reg val=0x%08x\n",
	dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0));
}

static void cv181xdac_shutdown(struct snd_pcm_substream *substream,
		struct snd_soc_dai *dai)
{
	struct cv181xdac *dac = snd_soc_dai_get_drvdata(dai);

	dev_dbg(dac->dev, "dac_shutdown\n");
	muteAmp(true);
	cv182xa_reset_dac();
}

static int cv181xdac_trigger(struct snd_pcm_substream *substream,
		int cmd, struct snd_soc_dai *dai)
{
	struct cv181xdac *dac = snd_soc_dai_get_drvdata(dai);
	int ret = 0;

	dev_dbg(dac->dev, "dac_trigger, cmd=%d\n", cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		snd_pcm_stream_unlock_irq(substream);
		cv181xdac_on(dac);
		muteAmp(false);
		snd_pcm_stream_lock_irq(substream);
		break;

	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		snd_pcm_stream_unlock_irq(substream);
		muteAmp(true);
		cv181xdac_off(dac);
		snd_pcm_stream_lock_irq(substream);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static int cv181xdac_prepare(struct snd_pcm_substream *substream,
		struct snd_soc_dai *dai)
{
	struct cv181xdac *dac = snd_soc_dai_get_drvdata(dai);
	u32 val;

	//need to rewrite the register if called cv182xa_reset_dac
	val = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1, val);
	val = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, val);
	val = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0, val);

	return 0;
}

static struct cv181xdac *file_dac_dev(struct file *file)
{
	return container_of(file->private_data, struct cv181xdac, miscdev);
}

static long dac_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	unsigned int __user *argp = (unsigned int __user *)arg;
	struct cv181xdac *dac = file_dac_dev(file);
	struct cvi_vol_ctrl vol;
	u32 val;
	u32 temp;

	if (argp != NULL) {
		if (!copy_from_user(&val, argp, sizeof(val))) {
			if (mutex_lock_interruptible(&cv181xdac_mutex)) {
				pr_debug("cvitekadac: signal arrives while waiting for lock\n");
				return -EINTR;
			}
		} else
			return -EFAULT;
	}

	switch (cmd) {
	case ACODEC_SOFT_RESET_CTRL:
		cv182xa_reset_dac();
		break;

	case ACODEC_SET_OUTPUT_VOL:
		pr_debug("dac: ACODEC_SET_OUTPUT_VOL with val=%d\n", val);

		if ((val < 0) | (val > 32))
			pr_err("Only support range 0 [mute] ~ 32 [maximum]\n");
		else {
			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1)
					& ~(AUDIO_PHY_REG_TXDAC_GAIN_UB_0_MASK | AUDIO_PHY_REG_TXDAC_GAIN_UB_1_MASK);
			temp |= DAC_VOL_L(val) | DAC_VOL_R(val);
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1, temp);
		}
		break;

	case ACODEC_GET_OUTPUT_VOL:
		pr_debug("dac: ACODEC_GET_OUTPUT_VOL\n");
		temp = ((dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1)
				& AUDIO_PHY_REG_TXDAC_GAIN_UB_0_MASK) + 1) / CV181X_DAC_VOL_STEP;
		pr_debug("dac: return val=%d\n", temp);
		if (copy_to_user(argp, &temp, sizeof(temp)))
			pr_err("dac, failed to return output vol\n");
		break;

	case ACODEC_SET_I2S1_FS:
		pr_debug("dac: ACODEC_SET_I2S1_FS is not support\n");
		break;

	case ACODEC_SET_DACL_VOL:
		pr_debug("dac: ACODEC_SET_DACL_VOL\n");
		if (copy_from_user(&vol, argp, sizeof(vol))) {
			if (mutex_is_locked(&cv181xdac_mutex))
				mutex_unlock(&cv181xdac_mutex);

			return -EFAULT;
		}
		if (vol.vol_ctrl_mute == 1) {
			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);
			temp |= AUDIO_PHY_REG_DA_DEML_TXDAC_OW_EN_ON;
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, temp);
		} else if ((vol.vol_ctrl < 0) | (vol.vol_ctrl > 32))
			pr_err("dac-L: Only support range 0 [mute] ~ 32 [maximum]\n");
		else {
			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1) & ~AUDIO_PHY_REG_TXDAC_GAIN_UB_0_MASK;
			temp |= DAC_VOL_L(vol.vol_ctrl);
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1, temp);

			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);
			temp &= AUDIO_PHY_REG_DA_DEML_TXDAC_OW_EN_OFF;
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, temp);
		}
		break;

	case ACODEC_SET_DACR_VOL:
		pr_debug("dac: ACODEC_SET_DACR_VOL\n");
		if (copy_from_user(&vol, argp, sizeof(vol)))
			return -EFAULT;

		if (vol.vol_ctrl_mute == 1) {
			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);
			temp |= AUDIO_PHY_REG_DA_DEMR_TXDAC_OW_EN_ON;
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, temp);
		} else if ((vol.vol_ctrl < 0) | (vol.vol_ctrl > 32))
			pr_err("dac-L: Only support range 0 [mute] ~ 32 [maximum]\n");
		else {
			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1) & ~AUDIO_PHY_REG_TXDAC_GAIN_UB_1_MASK;
			temp |= DAC_VOL_R(vol.vol_ctrl);
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1, temp);

			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);
			temp &= AUDIO_PHY_REG_DA_DEMR_TXDAC_OW_EN_OFF;
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, temp);
		}
		break;

	case ACODEC_SET_DACL_MUTE:
		pr_debug("dac: ACODEC_SET_DACL_MUTE, val=%d\n", val);
		temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);
		if (val == 0)
			temp &= AUDIO_PHY_REG_DA_DEML_TXDAC_OW_EN_OFF;
		else
			temp |= AUDIO_PHY_REG_DA_DEML_TXDAC_OW_EN_ON;

		dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, temp);
		break;
	case ACODEC_SET_DACR_MUTE:
		pr_debug("dac: ACODEC_SET_DACR_MUTE, val=%d\n", val);
		temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);
		if (val == 0)
			temp &= AUDIO_PHY_REG_DA_DEMR_TXDAC_OW_EN_OFF;
		else
			temp |= AUDIO_PHY_REG_DA_DEMR_TXDAC_OW_EN_ON;
		dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, temp);
		break;

	case ACODEC_GET_DACL_VOL:
		pr_debug("dac: ACODEC_GET_DACL_VOL\n");
		temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);
		if (temp & AUDIO_PHY_REG_DA_DEML_TXDAC_OW_EN_MASK) {
			vol.vol_ctrl = 0;
			vol.vol_ctrl_mute = 1;
		} else {
			temp = ((dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1)
			& AUDIO_PHY_REG_TXDAC_GAIN_UB_0_MASK) + 1) / CV181X_DAC_VOL_STEP;
			vol.vol_ctrl = temp;
			vol.vol_ctrl_mute = 0;
		}
		if (copy_to_user(argp, &vol, sizeof(vol)))
			pr_err("failed to return DACL vol\n");
		break;
	case ACODEC_GET_DACR_VOL:
		temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);
		pr_debug("dac: ACODEC_GET_DACR_VOL, txdac_ana2=0x%x\n", temp);
		if (temp & AUDIO_PHY_REG_DA_DEMR_TXDAC_OW_EN_MASK) {
			vol.vol_ctrl = 0;
			vol.vol_ctrl_mute = 1;
		} else {
			temp = (((dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1)
			& AUDIO_PHY_REG_TXDAC_GAIN_UB_1_MASK) >> 16) + 1) / CV181X_DAC_VOL_STEP;
			vol.vol_ctrl = temp;
			vol.vol_ctrl_mute = 0;
		}
		if (copy_to_user(argp, &vol, sizeof(vol)))
			pr_err("failed to return DACR vol\n");
		break;

	case ACODEC_SET_PD_DACL:
		pr_debug("dac: ACODEC_SET_PD_DACL, val=%d\n", val);
		if (val == 0) {
			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0);
			temp &= AUDIO_PHY_REG_TXDAC_EN_ON | AUDIO_PHY_REG_I2S_RX_EN_ON;
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0, temp);
		} else {
			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0);
			temp &= AUDIO_PHY_REG_TXDAC_EN_OFF & AUDIO_PHY_REG_I2S_RX_EN_OFF;
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0, temp);
		}
		break;
	case ACODEC_SET_PD_DACR:
		pr_debug("dac: ACODEC_SET_PD_DACR, val=%d\n", val);
		if (val == 0) {
			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0);
			temp &= AUDIO_PHY_REG_TXDAC_EN_ON | AUDIO_PHY_REG_I2S_RX_EN_ON;
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0, temp);
		} else {
			temp = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0);
			temp &= AUDIO_PHY_REG_TXDAC_EN_OFF & AUDIO_PHY_REG_I2S_RX_EN_OFF;
			dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0, temp);
		}
		break;
	case ACODEC_SET_DAC_DE_EMPHASIS:
		pr_info("dac: ACODEC_SET_DAC_DE_EMPHASIS is not support\n");
		break;
	default:
		pr_info("%s, received unsupported cmd=%u\n", __func__, cmd);
		break;
	}

	if (mutex_is_locked(&cv181xdac_mutex))
		mutex_unlock(&cv181xdac_mutex);

	return 0;
}

static const struct snd_soc_dai_ops cv181xdac_dai_ops = {
	.hw_params	= cv181xdac_hw_params,
	.set_fmt	= cv181xdac_set_dai_fmt,
	.startup	= cv181xdac_startup,
	.shutdown	= cv181xdac_shutdown,
	.trigger	= cv181xdac_trigger,
	.prepare	= cv181xdac_prepare,
};

static struct snd_soc_dai_driver cv181xdac_dai = {
	.name		= "cvitekadac",
	.playback	= {
		.stream_name	= "Playback",
		.channels_min	= 1,
		.channels_max	= 2,
		.rates = SNDRV_PCM_RATE_8000_48000,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,
	},
	.ops		= &cv181xdac_dai_ops,
};

static const struct snd_kcontrol_new cv181xdac_controls[] = {
	SOC_DOUBLE("DAC Playback Power Up/Down", AUDIO_PHY_TXDAC_CTRL0, 1, 0, 1, 1),
	SOC_DOUBLE("DAC Playback Volume", AUDIO_PHY_TXDAC_AFE1, 0, 16, 32, 1),
	SOC_DOUBLE("DAC Playback MUTE", AUDIO_PHY_TXDAC_ANA2, 16, 17, 1, 0),
};

unsigned int cv181xdac_reg_read(struct snd_soc_component *codec, unsigned int reg)
{
	struct cv181xdac *dac = dev_get_drvdata(codec->dev);
	int ret;
	u32 temp_lval = 0;
	u32 temp_rval = 0;

	ret = dac_read_reg(dac->dac_base, reg);

	if (reg == AUDIO_PHY_TXDAC_AFE1) {
		temp_lval = ((ret & 0x000001ff) + 1) / 16;
		temp_rval = (((ret >> 16) & 0x000001ff) + 1) / 16;
		dev_info(dac->dev, "Get DAC Vol reg:%d,ret:0x%x temp_lval=%d.\n", reg, ret, temp_lval);
		ret = (temp_rval<<16)|temp_lval;
	}

	dev_info(dac->dev, "dac_reg_read reg:%d,ret:%#x.\n", reg, ret);

	return ret;
}

int cv181xdac_reg_write(struct snd_soc_component *codec, unsigned int reg, unsigned int value)
{
	struct cv181xdac *dac = dev_get_drvdata(codec->dev);
	u32 temp_lval;
	u32 temp_rval;

	if (reg == AUDIO_PHY_TXDAC_AFE1 && value) {
		temp_lval = value & 0xffff;
		temp_rval = (value >> 16) & 0xffff;
		if (temp_lval > 32)
			temp_lval = 32;
		if (temp_rval > 32)
			temp_rval = 32;
		value = DAC_VOL_L(temp_lval)|DAC_VOL_R(temp_rval);
	}

	dac_write_reg(dac->dac_base, reg, value);
	dev_info(dac->dev, "dac_reg_write reg:%d,value:%#x.\n", reg, value);

	return 0;
}

static const struct snd_soc_component_driver soc_component_dev_cv181xdac = {
	.controls = cv181xdac_controls,
	.num_controls = ARRAY_SIZE(cv181xdac_controls),
	.read = cv181xdac_reg_read,
	.write = cv181xdac_reg_write,
};

static const struct file_operations dac_fops = {
	.owner = THIS_MODULE,
	.open = dac_open,
	.release = dac_close,
	.unlocked_ioctl = dac_ioctl,
	.compat_ioctl = dac_ioctl,
};

static int dac_device_register(struct cv181xdac *dac)
{
	struct miscdevice *miscdev = &dac->miscdev;
	int ret;

	miscdev->minor = MISC_DYNAMIC_MINOR;
	miscdev->name = "cvitekadac";
	miscdev->fops = &dac_fops;
	miscdev->parent = NULL;

	ret = misc_register(miscdev);
	if (ret) {
		pr_err("dac: failed to register misc device.\n");
		return ret;
	}

	return 0;
}

static int cv181xdac_probe(struct platform_device *pdev)
{
	struct cv181xdac *dac;
	struct resource *res;
	int ret;
	enum of_gpio_flags flags;

	mute_pin_l =  -EINVAL;
	mute_pin_r =  -EINVAL;

	dev_info(&pdev->dev, "cvitekadac_probe\n");

	dac = devm_kzalloc(&pdev->dev, sizeof(*dac), GFP_KERNEL);
	if (!dac)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	dac->dac_base = devm_ioremap_resource(&pdev->dev, res);
	dev_dbg(&pdev->dev, "cvitekadac_probe get dac_base=0x%p\n", dac->dac_base);
	if (IS_ERR(dac->dac_base))
		return PTR_ERR(dac->dac_base);

	dev_set_drvdata(&pdev->dev, dac);
	dac->dev = &pdev->dev;

	ret = dac_device_register(dac);
	if (ret < 0) {
		pr_err("dac: register device error\n");
		return ret;
	}

	mute_pin_l = of_get_named_gpio_flags(pdev->dev.of_node,
		"mute-gpio-l", 0, &flags);
	mute_pin_r = of_get_named_gpio_flags(pdev->dev.of_node,
		"mute-gpio-r", 0, &flags);

	if (!gpio_is_valid(mute_pin_l)) {
		pr_err("cvitekadac_probe gpio_is_valid mute_pin_l\n");
		mute_pin_l =  -EINVAL;
	} else {
		gpio_request(mute_pin_l, "mute_pin_l");
		gpio_direction_output(mute_pin_l, 1);
		gpio_set_value(mute_pin_l, 0);
	}

	if (!gpio_is_valid(mute_pin_r)) {
		pr_err("cvitekadac_probe gpio_is_valid mute_pin_r\n");
		mute_pin_r =  -EINVAL;
	} else {
		gpio_request(mute_pin_r, "mute_pin_r");
		gpio_direction_output(mute_pin_r, 1);
		gpio_set_value(mute_pin_r, 0);
	}
	return devm_snd_soc_register_component(&pdev->dev, &soc_component_dev_cv181xdac,
					  &cv181xdac_dai, 1);
}

static int cv181xdac_remove(struct platform_device *pdev)
{
	muteAmp(true);
	dev_dbg(&pdev->dev, "cvitekadac_remove\n");
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id cvitek_dac_of_match[] = {
	{ .compatible = "cvitek,cv182xadac", },
	{},
};

MODULE_DEVICE_TABLE(of, cvitek_dac_of_match);
#endif

#ifdef CONFIG_PM_SLEEP
static int cv181xdac_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct cv181xdac *dac = platform_get_drvdata(pdev);

	muteAmp(true);
	if (!dac->reg_ctx) {
		dac->reg_ctx = devm_kzalloc(dac->dev, sizeof(struct cv181xdac_context), GFP_KERNEL);
		if (!dac->reg_ctx)
			return -ENOMEM;
	}

	dac->reg_ctx->ctl0 = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0);
	dac->reg_ctx->ctl1 = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL1);
	dac->reg_ctx->afe0 = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE0);
	dac->reg_ctx->afe1 = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1);
	dac->reg_ctx->ana0 = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA0);
	dac->reg_ctx->ana1 = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA1);
	dac->reg_ctx->ana2 = dac_read_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2);

	return 0;
}

static int cv181xdac_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct cv181xdac *dac = platform_get_drvdata(pdev);

	muteAmp(false);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL0, dac->reg_ctx->ctl0);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_CTRL1, dac->reg_ctx->ctl1);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE0, dac->reg_ctx->afe0);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_AFE1, dac->reg_ctx->afe1);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA0, dac->reg_ctx->ana0);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA1, dac->reg_ctx->ana1);
	dac_write_reg(dac->dac_base, AUDIO_PHY_TXDAC_ANA2, dac->reg_ctx->ana2);

	return 0;
}

static SIMPLE_DEV_PM_OPS(cv181xdac_pm_ops, cv181xdac_suspend,
			 cv181xdac_resume);
#endif

static struct platform_driver cv181xdac_platform_driver = {
	.probe		= cv181xdac_probe,
	.remove		= cv181xdac_remove,
	.driver		= {
		.name	= "cvitekadac",
		.of_match_table = of_match_ptr(cvitek_dac_of_match),
#ifdef CONFIG_PM_SLEEP
		.pm	= &cv181xdac_pm_ops,
#endif
	},
};
module_platform_driver(cv181xdac_platform_driver);

MODULE_DESCRIPTION("ASoC CVITEK cvitekaDAC driver");
MODULE_AUTHOR("Ethan Chen <ethan.chen@wisecore.com.tw>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:cvitekadac");
