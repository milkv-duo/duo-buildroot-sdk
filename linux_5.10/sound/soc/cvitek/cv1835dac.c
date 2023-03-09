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
#include "../codecs/cv1835dac.h"

static DEFINE_MUTEX(dac_mutex);

static inline void dac_write_reg(void __iomem *io_base, int reg, u32 val)
{
	writel(val, io_base + reg);
}

static inline u32 dac_read_reg(void __iomem *io_base, int reg)
{
	return readl(io_base + reg);
}

bool cv1835dac_is_mute(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *dai = NULL;
	struct cvi1835dac *dac = NULL;
	u32 temp, vol, mute;

	if (rtd == NULL)
		return false;

	dai = rtd->codec_dai;
	dac = snd_soc_dai_get_drvdata(dai);

	if (dac == NULL || dai == NULL)
		return false;

	temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);//
	vol = (temp & ~CVI_DAC_S_MASK) >> 12;
	vol = 15 - vol;
	mute  = (CVI_DAC_DEMUTE_MODE == (temp & CVI_DAC_DEMUTE_MODE)) ? 0 : 1;
	if (mute || !vol)
		return true;

	return false;
}

static int dac_open(struct inode *inode, struct file *file)
{
	if (mutex_lock_interruptible(&dac_mutex))
		return -EINTR;
	mutex_unlock(&dac_mutex);
	pr_debug("%s\n", __func__);
	return 0;
}

static int dac_close(struct inode *inode, struct file *file)
{
	if (mutex_lock_interruptible(&dac_mutex))
		return -EINTR;
	mutex_unlock(&dac_mutex);
	pr_debug("%s\n", __func__);
	return 0;
}


static int cv1835dac_set_dai_fmt(struct snd_soc_dai *dai,
				 unsigned int fmt)
{

	struct cvi1835dac *dac = snd_soc_dai_get_drvdata(dai);

	if (!dac->dev)
		dev_err(dac->dev, "dev is NULL\n");


	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		dev_err(dac->dev, "Cannot set ADC to MASTER mode\n");
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

static int cv1835dac_hw_params(struct snd_pcm_substream *substream,
			       struct snd_pcm_hw_params *params,
			       struct snd_soc_dai *dai)
{

	struct cvi1835dac *dac = snd_soc_dai_get_drvdata(dai);
	int rate;

	rate = params_rate(params);
	if (rate >= 8000 && rate <= 192000) {
		dev_dbg(dac->dev, "%s, set rate to %d\n", __func__, rate);

		switch (rate) {
		case 8000:
		case 11025:
		case 16000:
		case 22050:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_16_32);
			break;
		case 32000:
		case 44100:
		case 48000:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_32_64);
			break;
		case 96000:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_64_128);
			break;
		case 192000:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_128_192);
			break;
		default:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_16_32);
			dev_dbg(dac->dev, "%s, set sample rate with default 16KHz\n", __func__);
			break;
		}
	} else {
		dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_16_32);
		dev_err(dac->dev, "Rate: %d is not supported\n", rate);
		dev_err(dac->dev, "%s, set sample rate with default 16KHz\n", __func__);
		return 0;
	}

	return 0;
}

static int cv1835dac_startup(struct snd_pcm_substream *substream,
			     struct snd_soc_dai *dai)
{
	return 0;
}

static void cv1835dac_on(struct cvi1835dac *dac)
{
	u32 val = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);

	dev_dbg(dac->dev, "%s, before ctrl_reg val=0x%08x\n", __func__, dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG));
	if (!((val & CVI_DAC_PWDAR_DOWN) && (val & CVI_DAC_PWDAL_DOWN)))
		dev_info(dac->dev, "DAL or DAR already switched ON!!, val=0x%08x\n", val);

	val &= CVI_DAC_PWDAL_NORMAL & CVI_DAC_PWDAR_NORMAL;
	val |= CVI_DAC_EN_REF_ENABLE;
	dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, val);
	msleep(500); /* In order to avoid pop noise */
	dev_dbg(dac->dev, "%s, after ctrl_reg val=0x%08x\n", __func__, dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG));
}

static void cv1835dac_off(struct cvi1835dac *dac)
{
	u32 val = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);

	dev_dbg(dac->dev, "%s, before ctrl_reg val=0x%08x\n", __func__, dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG));
	val |= CVI_DAC_PWDAR_DOWN | CVI_DAC_PWDAL_DOWN;
	//val &= CVI_DAC_EN_REF_DISABLE;

	dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, val);
	dev_dbg(dac->dev, "%s, after ctrl_reg val=0x%08x\n", __func__, dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG));
}

static void cv1835dac_shutdown(struct snd_pcm_substream *substream,
			       struct snd_soc_dai *dai)
{
	struct cvi1835dac *dac = snd_soc_dai_get_drvdata(dai);

	dev_dbg(dac->dev, "%s\n", __func__);
}

static int cv1835dac_trigger(struct snd_pcm_substream *substream,
			     int cmd, struct snd_soc_dai *dai)
{
	struct cvi1835dac *dac = snd_soc_dai_get_drvdata(dai);
	int ret = 0;

	dev_dbg(dac->dev, "%s, cmd=%d\n", __func__, cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		snd_pcm_stream_unlock_irq(substream);
		cv1835dac_on(dac);
		snd_pcm_stream_lock_irq(substream);
		break;

	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		snd_pcm_stream_unlock_irq(substream);
		cv1835dac_off(dac);
		snd_pcm_stream_lock_irq(substream);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static struct cvi1835dac *file_dac_dev(struct file *file)
{
	return container_of(file->private_data, struct cvi1835dac, miscdev);
}

static long dac_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned int __user *argp = (unsigned int __user *)arg;
	struct cvi1835dac *dac = file_dac_dev(file);
	struct cvi_vol_ctrl vol;
	u32 val;
	u32 temp;

	if (argp != NULL) {
		if (!copy_from_user(&val, argp, sizeof(val))) {
			if (mutex_lock_interruptible(&dac_mutex)) {
				pr_debug("cv1835dac: signal arrives while waiting for lock\n");
				return -EINTR;
			}
		} else
			return -EFAULT;
	}

	switch (cmd) {
	case ACODEC_SOFT_RESET_CTRL:
		pr_debug("dac: reset\n");
		break;
	case ACODEC_SET_OUTPUT_VOL:
		pr_debug("dac: ACODEC_SET_OUTPUT_VOL with val=%d\n", val);

		if ((val < 0) | (val > 15))
			pr_err("Only support range 15 [0dB] ~ 0 [mute]\n");
		else {
			val = 15 - val;
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG) & CVI_DAC_S_MASK;
			temp |= CVI_DAC_S(val);
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		}
		break;
	case ACODEC_GET_OUTPUT_VOL:
		pr_debug("dac: ACODEC_GET_OUTPUT_VOL\n");
		temp = (dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG) & ~CVI_DAC_S_MASK) >> 12;
		temp = 15 - temp;
		pr_debug("dac: return val=%d\n", temp);
		if (copy_to_user(argp, &temp, sizeof(temp)))
			pr_err("dac, failed to return output vol\n");
		break;
	case ACODEC_SET_I2S1_FS:
		pr_debug("dac: ACODEC_SET_I2S1_FS with val=%d\n", val);
		switch (val) {
		case ACODEC_FS_8000:
		case ACODEC_FS_16000:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_16_32);
			break;
		case ACODEC_FS_32000:
		case ACODEC_FS_48000:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_32_64);
			break;
		case ACODEC_FS_96000:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_64_128);
			break;
		case ACODEC_FS_192000:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_128_192);
			break;
		default:
			dac_write_reg(dac->dac_base, CVI_DAC_FS_REG, CVI_DAC_FS_16_32);
			dev_dbg(dac->dev, "%s, set sample rate with default 16KHz\n", __func__);
			break;
		}
		break;
	case ACODEC_SET_DACL_VOL:
		pr_debug("dac: ACODEC_SET_DACL_VOL\n");
		if (copy_from_user(&vol, argp, sizeof(vol))) {
			if (mutex_is_locked(&dac_mutex))
				mutex_unlock(&dac_mutex);

			return -EFAULT;
		}
		if (vol.vol_ctrl_mute == 1) {
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
			temp &= CVI_DAC_MUTE_MODE;
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		} else if ((vol.vol_ctrl < 0) | (vol.vol_ctrl > 15))
			pr_err("dac-L: Only support range 15 [0dB] ~ 0 [-22.5dB]\n");
		else {
			vol.vol_ctrl = 15 - vol.vol_ctrl;
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG) & CVI_DAC_S_MASK;
			temp |= CVI_DAC_S(vol.vol_ctrl);
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		}
		break;
	case ACODEC_SET_DACR_VOL:
		pr_debug("dac: ACODEC_SET_DACR_VOL\n");
		if (copy_from_user(&vol, argp, sizeof(vol)))
			return -EFAULT;

		if (vol.vol_ctrl_mute == 1) {
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
			temp &= CVI_DAC_MUTE_MODE;
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		} else if ((vol.vol_ctrl < 0) | (vol.vol_ctrl > 15))
			pr_err("dac-L: Only support range 15 [0dB] ~ 0 [-22.5dB]\n");
		else {
			vol.vol_ctrl = 15 - vol.vol_ctrl;
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG) & CVI_DAC_S_MASK;
			temp |= CVI_DAC_S(vol.vol_ctrl);
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		}
		break;
	case ACODEC_SET_DACL_MUTE:
		pr_debug("dac: ACODEC_SET_DACL_MUTE, val=%d\n", val);
		temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
		if (val == 0)
			temp |= CVI_DAC_DEMUTE_MODE;
		else
			temp &= CVI_DAC_MUTE_MODE;
		dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		break;
	case ACODEC_SET_DACR_MUTE:
		pr_debug("dac: ACODEC_SET_DACR_MUTE, val=%d\n", val);
		temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
		if (val == 0)
			temp |= CVI_DAC_DEMUTE_MODE;
		else
			temp &= CVI_DAC_MUTE_MODE;
		dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		break;
	case ACODEC_GET_DACL_VOL:
		pr_debug("dac: ACODEC_GET_DACL_VOL\n");
		temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
		if (!(temp & CVI_DAC_DEMUTE_MODE)) {
			vol.vol_ctrl = 0;
			vol.vol_ctrl_mute = 1;
		} else {
			temp = (dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG) & ~CVI_DAC_S_MASK) >> 12;
			temp = 15 - temp;
			vol.vol_ctrl = temp;
			vol.vol_ctrl_mute = 0;
		}
		if (copy_to_user(argp, &vol, sizeof(vol)))
			pr_err("failed to return DACL vol\n");
		break;
	case ACODEC_GET_DACR_VOL:
		pr_debug("dac: ACODEC_GET_DACR_VOL\n");
		temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
		if (!(temp & CVI_DAC_DEMUTE_MODE)) {
			vol.vol_ctrl = 0;
			vol.vol_ctrl_mute = 1;
		} else {
			temp = (dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG) & ~CVI_DAC_S_MASK) >> 12;
			temp = 15 - temp;
			vol.vol_ctrl = temp;
			vol.vol_ctrl_mute = 0;
		}
		if (copy_to_user(argp, &vol, sizeof(vol)))
			pr_err("failed to return DACR vol\n");
		break;
	case ACODEC_SET_PD_DACL:
		pr_debug("dac: ACODEC_SET_PD_DACL, val=%d\n", val);
		if (val == 0) {
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
			temp &= CVI_DAC_PWDAL_NORMAL;
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		} else {
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
			temp |= CVI_DAC_PWDAL_DOWN;
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		}
		break;
	case ACODEC_SET_PD_DACR:
		pr_debug("dac: ACODEC_SET_PD_DACR, val=%d\n", val);
		if (val == 0) {
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
			temp &= CVI_DAC_PWDAR_NORMAL;
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		} else {
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
			temp |= CVI_DAC_PWDAR_DOWN;
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		}
		break;
	case ACODEC_SET_DAC_DE_EMPHASIS:
		pr_debug("dac: ACODEC_SET_DAC_DE_EMPHASIS, val=%d\n", val);
		if (val == 1) {
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
			temp &= CVI_DAC_DEN_ENABLE;
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		} else {
			temp = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
			temp |= CVI_DAC_DEN_DISABLE;
			dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, temp);
		}
		break;
	default:
		pr_info("%s, received unsupported cmd=%u\n", __func__, cmd);
		break;
	}

	if (mutex_is_locked(&dac_mutex))
		mutex_unlock(&dac_mutex);

	return 0;
}

static const struct snd_soc_dai_ops cv1835dac_dai_ops = {
	.hw_params	= cv1835dac_hw_params,
	.set_fmt	= cv1835dac_set_dai_fmt,
	.startup	= cv1835dac_startup,
	.shutdown	= cv1835dac_shutdown,
	.trigger	= cv1835dac_trigger,
};

static struct snd_soc_dai_driver cv1835dac_dai = {
	.name		= "cv1835dac",
	.playback	= {
		.stream_name	= "Playback",
		.channels_min	= 1,
		.channels_max	= 2,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = SNDRV_PCM_FMTBIT_S32_LE
		| SNDRV_PCM_FMTBIT_S24_LE
		| SNDRV_PCM_FMTBIT_S24_3LE
		| SNDRV_PCM_FMTBIT_U24_LE
		| SNDRV_PCM_FMTBIT_U24_3LE
		| SNDRV_PCM_FMTBIT_S16_LE,
	},
	.ops		= &cv1835dac_dai_ops,
};

static const struct snd_kcontrol_new cv1835dac_controls[] = {
	SOC_SINGLE_RANGE("DAC Playback Volume", CVI_DAC_CTRL_REG, 12, 0, 15, 1),
	SOC_DOUBLE("DAC Playback MUTE", CVI_DAC_CTRL_REG, 8, 8, 3, 0),
	SOC_DOUBLE("DAC Playback Power Up/Down", CVI_DAC_CTRL_REG, 1, 0, 1, 1),
	SOC_SINGLE("DAC Playback De-emphasis", CVI_DAC_CTRL_REG, 3, 1, 1),
};

unsigned int cv1835dac_reg_read(struct snd_soc_component *codec, unsigned int reg)
{
	int ret;
	struct cvi1835dac *dac = dev_get_drvdata(codec->dev);

	ret = dac_read_reg(dac->dac_base, reg);
	dev_dbg(dac->dev, "%s reg:%d,ret:%#x.\n", __func__, reg, ret);

	return ret;
}

int cv1835dac_reg_write(struct snd_soc_component *codec, unsigned int reg, unsigned int value)
{
	struct cvi1835dac *dac = dev_get_drvdata(codec->dev);

	dac_write_reg(dac->dac_base, reg, value);

	dev_dbg(dac->dev, "%s reg:%d,value:%#x.\n", __func__, reg, value);

	return 0;
}

static const struct snd_soc_component_driver soc_component_dev_cv1835dac = {
	.controls = cv1835dac_controls,
	.num_controls = ARRAY_SIZE(cv1835dac_controls),
	.read = cv1835dac_reg_read,
	.write = cv1835dac_reg_write,
};

static const struct file_operations dac_fops = {
	.owner = THIS_MODULE,
	.open = dac_open,
	.release = dac_close,
	.unlocked_ioctl = dac_ioctl,
	.compat_ioctl = dac_ioctl,
};

static int dac_device_register(struct cvi1835dac *dac)
{
	struct miscdevice *miscdev = &dac->miscdev;
	int ret;

	miscdev->minor = MISC_DYNAMIC_MINOR;
	miscdev->name = "cv1835dac";
	miscdev->fops = &dac_fops;
	miscdev->parent = NULL;

	ret = misc_register(miscdev);
	if (ret) {
		pr_err("dac: failed to register misc device.\n");
		return ret;
	}

	return 0;
}

static int cv1835dac_probe(struct platform_device *pdev)
{
	struct cvi1835dac *dac;
	struct resource *res;
	int ret, val;

	dev_dbg(&pdev->dev, "%s\n", __func__);

	dac = devm_kzalloc(&pdev->dev, sizeof(*dac), GFP_KERNEL);
	if (!dac)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	dac->dac_base = devm_ioremap_resource(&pdev->dev, res);
	dev_dbg(&pdev->dev, "%s get dac_base=0x%p\n", __func__, dac->dac_base);
	if (IS_ERR(dac->dac_base))
		return PTR_ERR(dac->dac_base);

	dev_set_drvdata(&pdev->dev, dac);
	dac->dev = &pdev->dev;

	ret = dac_device_register(dac);
	if (ret < 0) {
		pr_err("dac: register device error\n");
		return ret;
	}

	val = dac_read_reg(dac->dac_base, CVI_DAC_CTRL_REG);
	val |= CVI_DAC_PWDAR_DOWN | CVI_DAC_PWDAL_DOWN | CVI_DAC_DEMUTE_MODE;
	val &= CVI_DAC_EN_REF_DISABLE;

	dac_write_reg(dac->dac_base, CVI_DAC_CTRL_REG, val);

	dac_write_reg(dac->dac_base, CVI_DAC_AIO_DA_REG, CVI_DAV_AIO_2CH);

	dac_write_reg(dac->dac_base, CVI_DAC_TEST_MODE_REG, CVI_DAC_TM_NORMAL);

	return devm_snd_soc_register_component(&pdev->dev, &soc_component_dev_cv1835dac,
					       &cv1835dac_dai, 1);
}

static int cv1835dac_remove(struct platform_device *pdev)
{
	dev_dbg(&pdev->dev, "%s\n", __func__);
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id cvitek_dac_of_match[] = {
	{ .compatible = "cvitek,cv1835dac", },
	{},
};

MODULE_DEVICE_TABLE(of, cvitek_dac_of_match);
#endif

static struct platform_driver cv1835dac_platform_driver = {
	.probe		= cv1835dac_probe,
	.remove		= cv1835dac_remove,
	.driver		= {
		.name	= "cv1835dac",
		.of_match_table = of_match_ptr(cvitek_dac_of_match),
	},
};
module_platform_driver(cv1835dac_platform_driver);

MODULE_DESCRIPTION("ASoC CVITEK CV1835PDM driver");
MODULE_AUTHOR("Ethan Chen <ethan.chen@wisecore.com.tw>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:cv1835dac");
