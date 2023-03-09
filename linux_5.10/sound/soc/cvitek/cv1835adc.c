// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * CVITEK CV1835 ADC driver
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
#include "../codecs/cv1835adc.h"

static DEFINE_MUTEX(adc_mutex);

u32 old_gsell_mic;
u32 old_gselr_mic;

static int adc_open(struct inode *inode, struct file *file)
{
	if (mutex_lock_interruptible(&adc_mutex))
		return -EINTR;
	mutex_unlock(&adc_mutex);
	pr_debug("%s\n", __func__);
	return 0;
}

static int adc_close(struct inode *inode, struct file *file)
{
	if (mutex_lock_interruptible(&adc_mutex))
		return -EINTR;
	mutex_unlock(&adc_mutex);
	pr_debug("%s\n", __func__);
	return 0;
}

static inline void adc_write_reg(void __iomem *io_base, int reg, u32 val)
{
	writel(val, io_base + reg);
}

static inline u32 adc_read_reg(void __iomem *io_base, int reg)
{
	return readl(io_base + reg);
}

static int cv1835adc_set_dai_fmt(struct snd_soc_dai *dai,
				 unsigned int fmt)
{

	struct cvi1835adc *adc = snd_soc_dai_get_drvdata(dai);

	if (!adc->dev)
		dev_err(adc->dev, "dev is NULL\n");


	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		dev_dbg(adc->dev, "Set ADC to MASTER mode\n");
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		dev_err(adc->dev, "Cannot set DAC to SLAVE mode, only support MASTER mode\n");
		break;
	default:
		dev_err(adc->dev, "Cannot support this role mode\n");
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_IF:
		dev_dbg(adc->dev, "set codec to NB_IF\n");
		break;
	default:
		dev_err(adc->dev, "Cannot suuport this format\n");
		break;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_LEFT_J:
		dev_dbg(adc->dev, "set codec to LEFT-JUSTIFY mode\n");
		break;
	default:
		dev_err(adc->dev, "Cannot support this mode\n");
		break;
	}
	return 0;
}

static int cv1835adc_hw_params(struct snd_pcm_substream *substream,
			       struct snd_pcm_hw_params *params,
			       struct snd_soc_dai *dai)
{

	struct cvi1835adc *adc = snd_soc_dai_get_drvdata(dai);
	int rate;

	rate = params_rate(params);
	if (rate >= 8000 && rate <= 48000) {
		dev_dbg(adc->dev, "%s, set rate to %d\n", __func__, rate);

		switch (rate) {
		case 8000:
		case 11025:
		case 12000:
			adc_write_reg(adc->adc_base, CVI_ADC_FS_SEL_REG, CVI_ADC_RATIO_1024);
			break;
		case 16000:
		case 22050:
		case 24000:
			adc_write_reg(adc->adc_base, CVI_ADC_FS_SEL_REG, CVI_ADC_RATIO_512);
			break;
		case 32000:
		case 44100:
		case 48000:
			adc_write_reg(adc->adc_base, CVI_ADC_FS_SEL_REG, CVI_ADC_RATIO_256);
			break;
		default:
			adc_write_reg(adc->adc_base, CVI_ADC_FS_SEL_REG, CVI_ADC_RATIO_512);
			dev_dbg(adc->dev, "%s, set sample rate with default 16KHz\n", __func__);
			break;
		}
	} else {
		adc_write_reg(adc->adc_base, CVI_ADC_FS_SEL_REG, CVI_ADC_RATIO_512);
		dev_err(adc->dev, "Rate: %d Hz is not supported\n", rate);
		dev_err(adc->dev, "%s, set sample rate with default 16KHz\n", __func__);
		return 0;
	}

	if (params_width(params) != 16) {
		dev_err(adc->dev, "Only support I2S channel width with 16 bits\n");
		dev_err(adc->dev, "Set I2S channel width with 16bits\n");
		return 0;
	}

	return 0;
}

static int cv1835adc_startup(struct snd_pcm_substream *substream,
			     struct snd_soc_dai *dai)
{
	struct cvi1835adc *adc = snd_soc_dai_get_drvdata(dai);

	dev_dbg(adc->dev, "%s\n", __func__);

	return 0;
}

static void cv1835adc_on(struct cvi1835adc *adc)
{
	u32 val = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);

	dev_dbg(adc->dev, "%s, before ctrl_reg val=0x%08x\n", __func__, adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG));

	if ((val & CVI_ADC_ENADR_NORMAL) | (val & CVI_ADC_ENADL_NORMAL))
		dev_info(adc->dev, "ADL or ADR already switched ON!!, val=0x%08x\n", val);

	val |= CVI_ADC_ENADR_NORMAL | CVI_ADC_ENADL_NORMAL | CVI_ADC_VERF_NORMAL;
	adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, val);

	dev_dbg(adc->dev, "%s, after ctrl_reg val=0x%08x\n", __func__, adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG));
}

static void cv1835adc_off(struct cvi1835adc *adc)
{

	u32 val = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);

	val &= CVI_ADC_ENADR_POWER_DOWN & CVI_ADC_ENADL_POWER_DOWN & CVI_ADC_VREF_POWER_DOWN;
	adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, val);
	dev_dbg(adc->dev, "%s, after ctrl_reg val=0x%08x\n", __func__, adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG));

}

static void cv1835adc_shutdown(struct snd_pcm_substream *substream,
			       struct snd_soc_dai *dai)
{
	struct cvi1835adc *adc = snd_soc_dai_get_drvdata(dai);

	dev_dbg(adc->dev, "%s, before ctrl_reg val=0x%08x\n", __func__, adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG));

}

static int cv1835adc_trigger(struct snd_pcm_substream *substream,
			     int cmd, struct snd_soc_dai *dai)
{
	struct cvi1835adc *adc = snd_soc_dai_get_drvdata(dai);
	int ret = 0;

	dev_dbg(adc->dev, "%s, cmd=%d\n", __func__, cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		cv1835adc_on(adc);
		break;

	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		cv1835adc_off(adc);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static struct cvi1835adc *file_adc_dev(struct file *file)
{
	return container_of(file->private_data, struct cvi1835adc, miscdev);
}


static long adc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned int __user *argp = (unsigned int __user *)arg;
	struct cvi1835adc *adc = file_adc_dev(file);
	struct cvi_vol_ctrl vol;
	u32 val;
	u32 temp;
	u32 input_type;

	if (argp != NULL) {
		if (!copy_from_user(&val, argp, sizeof(val))) {
			if (mutex_lock_interruptible(&adc_mutex)) {
				pr_debug("cv1835adc: signal arrives while waiting for lock\n");
				return -EINTR;
			}
		} else
			return -EFAULT;
	}

	pr_debug("%s, received cmd=%u, val=%d\n", __func__, cmd, val);

	switch (cmd) {
	case ACODEC_SOFT_RESET_CTRL:
		pr_debug("adc: reset\n");
		break;
	case ACODEC_SET_INPUT_VOL:
		pr_debug("adc: ACODEC_SET_INPUT_VOL\n");
		input_type = (adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG) & CVI_ADC_INSELL_MIC) >> 1;
		switch (input_type) {
		case 0: /* line in */
			if ((val < 0) | (val > 31))
				pr_err("Only support range 31 [6dB] ~ 0 [mute]\n");
			else {
				val = 31 - val; /* invert 31 to 0, 0 to 31, vise versa */
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG)
				       & CVI_ADC_GSELR_MASK
				       & CVI_ADC_GSELL_MASK;

				temp |= CVI_ADC_GSELR(val) | CVI_ADC_GSELL(val);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			}
			break;
		case 1: /* mic in */
			if ((val < 0) | (val > 7))
				pr_err("Only support range 7 [40dB] ~ 0 [mute]\n");
			else {
				val = 31 - val; /* invert 31 to 0, 0 to 31, vise versa */
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG)
				       & CVI_ADC_GSELR_MIC_MASK
				       & CVI_ADC_GSELL_MIC_MASK;

				temp |= CVI_ADC_GSELR_MIC(val) | CVI_ADC_GSELL_MIC(val);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			}
			break;
		}
		break;
	case ACODEC_GET_INPUT_VOL:
		pr_debug("adc: ACODEC_GET_INPUT_VOL\n");
		input_type = (adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG) & CVI_ADC_INSELL_MIC) >> 1;
		switch (input_type) {
		case 0: /* line in */
			temp = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELL_MASK) >> 8;
			temp = 31 - temp;
			if (copy_to_user(argp, &temp, sizeof(temp)))
				pr_err("adc, failed to return input vol\n");
			break;
		case 1: /* MIC in */
			temp = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELL_MIC_MASK) >> 24;
			temp = 7 - temp;
			if (copy_to_user(argp, &temp, sizeof(temp)))
				pr_err("adc, failed to return input vol\n");
			break;
		}
		break;
	case ACODEC_SET_I2S1_FS:
		pr_debug("adc: ACODEC_SET_I2S1_FS\n");
		switch (val) {
		case ACODEC_FS_8000:
		case ACODEC_FS_12000:
			adc_write_reg(adc->adc_base, CVI_ADC_FS_SEL_REG, CVI_ADC_RATIO_1024);
			break;
		case ACODEC_FS_16000:
		case ACODEC_FS_24000:
			adc_write_reg(adc->adc_base, CVI_ADC_FS_SEL_REG, CVI_ADC_RATIO_512);
			break;
		case ACODEC_FS_32000:
		case ACODEC_FS_48000:
			adc_write_reg(adc->adc_base, CVI_ADC_FS_SEL_REG, CVI_ADC_RATIO_256);
			break;
		default:
			pr_debug("Not support this FS setting\n");
			break;
		}
		break;
	case ACODEC_SET_MIXER_MIC:
		temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG) & CVI_ADC_INSELL_LINE & CVI_ADC_INSELR_LINE;
		if (val == CVI_MIXER_LINEIN) /* input from linein */
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
		else if (val == CVI_MIXER_MIC_IN) /* input from MIC */
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp | CVI_ADC_INSELL_MIC | CVI_ADC_INSELR_MIC);
		else
			pr_err("Not support this kind of input\n");
		break;
	case ACODEC_SET_GAIN_MICL:
		pr_debug("adc: ACODEC_SET_GAIN_MICL\n");
		if ((val < 0) | (val > 7))
			pr_err("Only support range 7 [40dB] ~ 0 [mute]\n");
		else {
			val = 7 - val;
			temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELL_MIC_MASK;
			temp |= CVI_ADC_GSELL_MIC(val);
			old_gsell_mic = val;
			adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
		}
		break;
	case ACODEC_SET_GAIN_MICR:
		pr_debug("adc: ACODEC_SET_GAIN_MICR\n");
		if ((val < 0) | (val > 7))
			pr_err("Only support range 7 [40dB] ~ 0 [mute]\n");
		else {
			val = 7 - val;
			temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELR_MIC_MASK;
			temp |= CVI_ADC_GSELR_MIC(val);
			old_gselr_mic = val;
			adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
		}
		break;
	case ACODEC_SET_ADCL_VOL:

		if (copy_from_user(&vol, argp, sizeof(vol))) {
			if (mutex_is_locked(&adc_mutex))
				mutex_unlock(&adc_mutex);

			return -EFAULT;
		}

		pr_debug("adc: ACODEC_SET_ADCL_VOL to %d, mute=%d\n", vol.vol_ctrl, vol.vol_ctrl_mute);

		input_type = (adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG) & CVI_ADC_INSELL_MIC) >> 1;

		switch (input_type) {
		case 0: /* use linein */
			if (vol.vol_ctrl_mute == 1) {
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELL_MASK;
				temp |= CVI_ADC_GSELL(31);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			} else if ((vol.vol_ctrl < 0) | (vol.vol_ctrl > 31))
				pr_err("adc-R: Only support range 31 [6dB] ~ 0 [mute]\n");
			else {
				vol.vol_ctrl = 31 - vol.vol_ctrl;
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELL_MASK;
				temp |= CVI_ADC_GSELL(vol.vol_ctrl);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			}
			break;
		case 1: /* use MIC */
			if (vol.vol_ctrl_mute == 1) {
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELL_MIC_MASK;
				temp |= CVI_ADC_GSELL_MIC(7);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			} else if ((vol.vol_ctrl < 0) | (vol.vol_ctrl > 7))
				pr_err("adc-R: Only support range 7 [40dB] ~ 0 [mute]\n");
			else {
				vol.vol_ctrl = 7 - vol.vol_ctrl;
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELL_MIC_MASK;
				temp |= CVI_ADC_GSELL_MIC(vol.vol_ctrl);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			}
			break;
		}
		break;
	case ACODEC_SET_ADCR_VOL:
		if (copy_from_user(&vol, argp, sizeof(vol))) {
			if (mutex_is_locked(&adc_mutex))
				mutex_unlock(&adc_mutex);

			return -EFAULT;
		}

		pr_debug("adc: ACODEC_SET_ADCR_VOL to %d, mute=%d\n", vol.vol_ctrl, vol.vol_ctrl_mute);

		input_type = (adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG) & CVI_ADC_INSELR_MIC);

		switch (input_type) {
		case 0: /* use linein */
			if (vol.vol_ctrl_mute == 1) {
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELR_MASK;
				temp |= CVI_ADC_GSELR(31);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			} else if ((vol.vol_ctrl < 0) | (vol.vol_ctrl > 31))
				pr_err("adc-R: Only support range 31 [6dB] ~ 0 [mute]\n");
			else {
				vol.vol_ctrl = 31 - vol.vol_ctrl;
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELR_MASK;
				temp |= CVI_ADC_GSELR(vol.vol_ctrl);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			}
			break;
		case 1: /* use MIC */
			if (vol.vol_ctrl_mute == 1) {
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELR_MIC_MASK;
				temp |= CVI_ADC_GSELR_MIC(7);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			} else if ((vol.vol_ctrl < 0) | (vol.vol_ctrl > 7))
				pr_err("adc-R: Only support range 7 [40dB] ~ 0 [mute]\n");
			else {
				vol.vol_ctrl = 7 - vol.vol_ctrl;
				temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELR_MIC_MASK;
				temp |= CVI_ADC_GSELR_MIC(vol.vol_ctrl);
				adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
			}
			break;
		}
		break;
	case ACODEC_SET_MICL_MUTE:
		pr_debug("adc: ACODEC_SET_MICL_MUTE\n");
		if (val == 0) {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELL_MIC_MASK;
			if (old_gsell_mic == 7)
				temp |= CVI_ADC_GSELL_MIC(6);
			else
				temp |= CVI_ADC_GSELL_MIC(old_gsell_mic);
		} else {
			old_gsell_mic = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELL_MIC_MASK) >> 24;
			temp |= CVI_ADC_GSELL_MIC(7);
		}
		adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
		break;
	case ACODEC_SET_MICR_MUTE:
		pr_debug("adc: ACODEC_SET_MICR_MUTE\n");
		if (val == 0) {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & CVI_ADC_GSELR_MIC_MASK;
			if (old_gselr_mic == 7)
				temp |= CVI_ADC_GSELR_MIC(6);
			else
				temp |= CVI_ADC_GSELR_MIC(old_gselr_mic);
		} else {
			old_gselr_mic = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELR_MIC_MASK) >> 16;
			temp |= CVI_ADC_GSELR_MIC(7);
		}
		adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, temp);
		break;
	case ACODEC_GET_GAIN_MICL:
		pr_debug("adc: ACODEC_GET_GAIN_MICL\n");
		temp = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELL_MIC_MASK) >> 24;
		temp = 7 - temp;
		if (copy_to_user(argp, &temp, sizeof(temp)))
			pr_err("failed to return MICL gain\n");
		break;
	case ACODEC_GET_GAIN_MICR:
		pr_debug("adc: ACODEC_GET_GAIN_MICR\n");
		temp = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELR_MIC_MASK) >> 16;
		temp = 7 - temp;
		if (copy_to_user(argp, &temp, sizeof(temp)))
			pr_err("failed to return MICR gain\n");
		break;
	case ACODEC_GET_ADCL_VOL:
		pr_debug("adc: ACODEC_GET_ADCL_VOL\n");

		input_type = (adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG) & CVI_ADC_INSELL_MIC) >> 1;

		switch (input_type) {
		case 0: /* line in */
			temp = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELL_MASK) >> 8;
			if (temp == 31)
				vol.vol_ctrl_mute = 1;
			else
				vol.vol_ctrl_mute = 0;

			vol.vol_ctrl = 31 - temp;
			break;
		case 1: /* MIC in */
			temp = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELL_MIC_MASK) >> 24;
			if (temp == 7)
				vol.vol_ctrl_mute = 1;
			else
				vol.vol_ctrl_mute = 0;

			vol.vol_ctrl = 7 - temp;
			break;
		}

		if (copy_to_user(argp, &vol, sizeof(vol)))
			pr_err("failed to return ADCL vol\n");

		break;
	case ACODEC_GET_ADCR_VOL:
		pr_debug("adc: ACODEC_GET_ADCR_VOL\n");

		input_type = (adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG) & CVI_ADC_INSELR_MIC);

		switch (input_type) {
		case 0: /* line in */
			temp = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELR_MASK) >> 0;
			if (temp == 31)
				vol.vol_ctrl_mute = 1;
			else
				vol.vol_ctrl_mute = 0;

			vol.vol_ctrl = 31 - temp;
			break;
		case 1: /* MIC in */
			temp = (adc_read_reg(adc->adc_base, CVI_ADC_GSEL_REG) & ~CVI_ADC_GSELR_MIC_MASK) >> 16;
			if (temp == 7)
				vol.vol_ctrl_mute = 1;
			else
				vol.vol_ctrl_mute = 0;

			vol.vol_ctrl = 7 - temp;
			break;
		}

		if (copy_to_user(argp, &vol, sizeof(vol)))
			pr_err("failed to return ADCR vol\n");

		break;
	case ACODEC_SET_PD_ADCL:
		pr_debug("adc: ACODEC_SET_PD_ADCL, val=%d\n", val);
		if (val == 0) {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp |= CVI_ADC_ENADL_NORMAL;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
		} else {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp &= CVI_ADC_ENADL_POWER_DOWN;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
		}
		break;
	case ACODEC_SET_PD_ADCR:
		pr_debug("adc: ACODEC_SET_PD_ADCR, val=%d\n", val);
		if (val == 0) {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp |= CVI_ADC_ENADR_NORMAL;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
		} else {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp &= CVI_ADC_ENADR_POWER_DOWN;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
		}
		break;
	case ACODEC_SET_PD_LINEINL:
		pr_debug("adc: ACODEC_SET_PD_LINEINL, val=%d\n", val);
		if (val == 0) {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp |= CVI_ADC_ENADL_NORMAL;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
		} else {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp &= CVI_ADC_ENADL_POWER_DOWN;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
		}
		break;
	case ACODEC_SET_PD_LINEINR:
		pr_debug("adc: ACODEC_SET_PD_LINEINR, val=%d\n", val);
		if (val == 0) {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp |= CVI_ADC_ENADR_NORMAL;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
		} else {
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp &= CVI_ADC_ENADR_POWER_DOWN;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
		}
		break;
	case ACODEC_SET_ADC_HP_FILTER:
		pr_debug("adc: ACODEC_SET_ADC_HP_FILTER\n");
		switch (val) {
		case 0:
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp &= CVI_ADC_HPR_BYPASS & CVI_ADC_HPL_BYPASS;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
			break;
		case 1:
			temp = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG);
			temp |= CVI_ADC_HPR_ENABLE | CVI_ADC_HPL_ENABLE;
			adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, temp);
			break;
		default:
			pr_info("Not support this val %d\n", val);
			break;
		}
		break;
	default:
		pr_info("%s, received unsupport cmd=%u\n", __func__, cmd);
		break;
	}

	if (mutex_is_locked(&adc_mutex))
		mutex_unlock(&adc_mutex);

	return 0;
}
static const struct snd_soc_dai_ops cv1835adc_dai_ops = {
	.hw_params	= cv1835adc_hw_params,
	.set_fmt	= cv1835adc_set_dai_fmt,
	.startup	= cv1835adc_startup,
	.shutdown	= cv1835adc_shutdown,
	.trigger	= cv1835adc_trigger,
};

static struct snd_soc_dai_driver cv1835adc_dai = {
	.name		= "cv1835adc",
	.capture	= {
		.stream_name	= "Capture",
		.channels_min	= 1,
		.channels_max	= 2,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,
	},
	.ops		= &cv1835adc_dai_ops,
};

static const char *const cv1835adc_input_text[] = {
	"LineIn",
	"MicIn",
};

static SOC_ENUM_SINGLE_DECL(cv1835adc_l_input_enum,
			    CVI_ADC_CTRL_REG, 1, cv1835adc_input_text);

static SOC_ENUM_SINGLE_DECL(cv1835adc_r_input_enum,
			    CVI_ADC_CTRL_REG, 0, cv1835adc_input_text);

static const struct snd_kcontrol_new cv1835adc_controls[] = {
	SOC_DOUBLE("ADC Capture Line Volume", CVI_ADC_GSEL_REG, 0, 8, 31, 1),
	SOC_DOUBLE("ADC Capture Mic Volume", CVI_ADC_GSEL_REG, 16, 24, 7, 1),
	SOC_ENUM("ADC Capture L-Path", cv1835adc_l_input_enum),
	SOC_ENUM("ADC Capture R-Path", cv1835adc_r_input_enum),
	SOC_DOUBLE("ADC Capture High-Pass Filter", CVI_ADC_CTRL_REG, 7, 6, 1, 0),
	SOC_SINGLE("ADC Capture DAGC", CVI_ADC_DAGC_CTRL_REG, 0, 1, 0),
};

unsigned int cv1835adc_reg_read(struct snd_soc_component *codec, unsigned int reg)
{
	int ret;
	struct cvi1835adc *adc = dev_get_drvdata(codec->dev);

	ret = adc_read_reg(adc->adc_base, reg);
	dev_dbg(adc->dev, "%s reg:%d,ret:%#x.\n", __func__, reg, ret);

	return ret;
}

int cv1835adc_reg_write(struct snd_soc_component *codec, unsigned int reg, unsigned int value)
{
	struct cvi1835adc *adc = dev_get_drvdata(codec->dev);

	adc_write_reg(adc->adc_base, reg, value);

	dev_dbg(adc->dev, "%s reg:%d,value:%#x.\n", __func__, reg, value);

	return 0;
}

static const struct snd_soc_component_driver soc_component_dev_cv1835adc = {
	.controls = cv1835adc_controls,
	.num_controls = ARRAY_SIZE(cv1835adc_controls),
	.read = cv1835adc_reg_read,
	.write = cv1835adc_reg_write,
};

static const struct file_operations adc_fops = {
	.owner = THIS_MODULE,
	.open = adc_open,
	.release = adc_close,
	.unlocked_ioctl = adc_ioctl,
	.compat_ioctl = adc_ioctl,
};

static int adc_device_register(struct cvi1835adc *adc)
{
	struct miscdevice *miscdev = &adc->miscdev;
	int ret;

	miscdev->minor = MISC_DYNAMIC_MINOR;
	miscdev->name = "cv1835adc";
	miscdev->fops = &adc_fops;
	miscdev->parent = NULL;

	ret = misc_register(miscdev);
	if (ret) {
		pr_err("adc: failed to register misc device.\n");
		return ret;
	}

	return 0;
}

static int cv1835adc_probe(struct platform_device *pdev)
{
	struct cvi1835adc *adc;
	struct resource *res;
	int ret, val;

	dev_dbg(&pdev->dev, "%s\n", __func__);

	adc = devm_kzalloc(&pdev->dev, sizeof(*adc), GFP_KERNEL);
	if (!adc)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	adc->adc_base = devm_ioremap_resource(&pdev->dev, res);
	dev_dbg(&pdev->dev, "%s get adc_base=0x%p\n", __func__, adc->adc_base);
	if (IS_ERR(adc->adc_base))
		return PTR_ERR(adc->adc_base);

	dev_set_drvdata(&pdev->dev, adc);
	adc->dev = &pdev->dev;

	ret = adc_device_register(adc);
	if (ret < 0) {
		pr_err("adc: register device error\n");
		return ret;
	}

	/* set default input is MIC in*/
	val = adc_read_reg(adc->adc_base, CVI_ADC_CTRL_REG) | CVI_ADC_INSELL_MIC | CVI_ADC_INSELR_MIC;

	val |= CVI_ADC_ZCD_ENABLE;

	/* disable high pass to avoid pop noise while turn on ADC*/
	//val &= CVI_ADC_HPL_BYPASS & CVI_ADC_HPR_BYPASS;

	adc_write_reg(adc->adc_base, CVI_ADC_CTRL_REG, val);

	/* set default MIC gain to 21 dB and LINE in gain to -1 dB*/
	adc_write_reg(adc->adc_base, CVI_ADC_GSEL_REG, 0x03030606);

	/* enable AGC */
	val = adc_read_reg(adc->adc_base, CVI_ADC_DAGC_CTRL_REG) | CVI_ADC_DAGC_ENABLE;
	adc_write_reg(adc->adc_base, CVI_ADC_DAGC_CTRL_REG, val);

	adc_write_reg(adc->adc_base, CVI_ADC_TEST_MODE_REG, CVI_ADC_TM_NORMAL);

	return devm_snd_soc_register_component(&pdev->dev, &soc_component_dev_cv1835adc,
					       &cv1835adc_dai, 1);
}

static int cv1835adc_remove(struct platform_device *pdev)
{
	dev_dbg(&pdev->dev, "%s\n", __func__);
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id cvitek_adc_of_match[] = {
	{ .compatible = "cvitek,cv1835adc", },
	{},
};

MODULE_DEVICE_TABLE(of, cvitek_adc_of_match);
#endif

static struct platform_driver cv1835adc_platform_driver = {
	.probe		= cv1835adc_probe,
	.remove		= cv1835adc_remove,
	.driver		= {
		.name	= "cv1835adc",
		.of_match_table = of_match_ptr(cvitek_adc_of_match),
	},
};
module_platform_driver(cv1835adc_platform_driver);

MODULE_DESCRIPTION("ASoC CVITEK CV1835PDM driver");
MODULE_AUTHOR("Ethan Chen <ethan.chen@wisecore.com.tw>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:cv1835adc");
