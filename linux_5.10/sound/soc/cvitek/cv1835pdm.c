// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * CVITEK CV1835 PDM driver
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
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include "cv1835pdm.h"

static inline void pdm_write_reg(void __iomem *io_base, int reg, u32 val)
{
	writel(val, io_base + reg);
}

static inline u32 pdm_read_reg(void __iomem *io_base, int reg)
{
	return readl(io_base + reg);
}

static int cv1835pdm_set_dai_fmt(struct snd_soc_dai *dai,
				 unsigned int fmt)
{

	struct cvi1835pdm *pdm = snd_soc_dai_get_drvdata(dai);

	if (!pdm->dev)
		dev_err(pdm->dev, "dev is NULL\n");


	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		dev_dbg(pdm->dev, "set PDM to MASTER mode\n");
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		dev_err(pdm->dev, "Cannot set PDM to SLAVE mode\n");
		return -EINVAL;
	default:
		dev_err(pdm->dev, "Cannot support this role mode\n");
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_IB_NF:
		dev_dbg(pdm->dev, "set codec to IB_NF\n");
		break;
	default:
		dev_err(pdm->dev, "Cannot suuport this format\n");
		break;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		dev_dbg(pdm->dev, "set codec to I2S mode\n");
		break;
	default:
		dev_err(pdm->dev, "Cannot support this mode\n");
		break;
	}
	return 0;
}

static int cv1835pdm_hw_params(struct snd_pcm_substream *substream,
			       struct snd_pcm_hw_params *params,
			       struct snd_soc_dai *dai)
{

	struct cvi1835pdm *pdm = snd_soc_dai_get_drvdata(dai);
	int pdm_setting = pdm_read_reg(pdm->pdm_base, PDM_SETTING_REG);
	int i2s_setting = pdm_read_reg(pdm->pdm_base, I2S_SETTING_REG);
	int rate;

	rate = params_rate(params);
	if (rate >= 8000 && rate <= 192000) {
		dev_dbg(pdm->dev, "%s, set rate to %d\n", __func__, rate);

		switch (rate) {
		case 48000:
			pdm_write_reg(pdm->pdm_base, PDM_SETTING_REG, (pdm_setting & PDM_CLK_DIV_MASK) | PDM_48K);
			pdm_write_reg(pdm->pdm_base, I2S_SETTING_REG, (i2s_setting & I2S_CLK_DIV_MASK) | I2S_48K);
			dev_dbg(pdm->dev, "%s, set sample rate with 48KHz\n", __func__);
			break;
		case 44100:
			pdm_write_reg(pdm->pdm_base, PDM_SETTING_REG, (pdm_setting & PDM_CLK_DIV_MASK) | PDM_44_1K);
			pdm_write_reg(pdm->pdm_base, I2S_SETTING_REG, (i2s_setting & I2S_CLK_DIV_MASK) | I2S_44_1K);
			dev_dbg(pdm->dev, "%s, set sample rate with 44.1KHz\n", __func__);
			break;
		case 22050:
			pdm_write_reg(pdm->pdm_base, PDM_SETTING_REG, (pdm_setting & PDM_CLK_DIV_MASK) | PDM_22_05K);
			pdm_write_reg(pdm->pdm_base, I2S_SETTING_REG, (i2s_setting & I2S_CLK_DIV_MASK) | I2S_22_05K);
			dev_dbg(pdm->dev, "%s, set sample rate with 22.05KHz\n", __func__);
			break;
		case 16000:
			pdm_write_reg(pdm->pdm_base, PDM_SETTING_REG, (pdm_setting & PDM_CLK_DIV_MASK) | PDM_16K);
			pdm_write_reg(pdm->pdm_base, I2S_SETTING_REG, (i2s_setting & I2S_CLK_DIV_MASK) | I2S_16K);
			dev_dbg(pdm->dev, "%s, set sample rate with 16KHz\n", __func__);
			break;
		case 11025:
			pdm_write_reg(pdm->pdm_base, PDM_SETTING_REG, (pdm_setting & PDM_CLK_DIV_MASK) | PDM_11_025K);
			pdm_write_reg(pdm->pdm_base, I2S_SETTING_REG, (i2s_setting & I2S_CLK_DIV_MASK) | I2S_11_025K);
			dev_dbg(pdm->dev, "%s, set sample rate with 11.025KHz\n", __func__);
			break;
		case 8000:
			pdm_write_reg(pdm->pdm_base, PDM_SETTING_REG, (pdm_setting & PDM_CLK_DIV_MASK) | PDM_8K);
			pdm_write_reg(pdm->pdm_base, I2S_SETTING_REG, (i2s_setting & I2S_CLK_DIV_MASK) | I2S_8K);
			dev_dbg(pdm->dev, "%s, set sample rate with 8KHz\n", __func__);
			break;
		default:
			pdm_write_reg(pdm->pdm_base, PDM_SETTING_REG, (pdm_setting & PDM_CLK_DIV_MASK) | PDM_8K);
			pdm_write_reg(pdm->pdm_base, I2S_SETTING_REG, (i2s_setting & I2S_CLK_DIV_MASK) | I2S_8K);
			dev_dbg(pdm->dev, "%s, set sample rate with default 48KHz\n", __func__);
			break;
		}
	} else {
		dev_err(pdm->dev, "Rate: %d is not supported\n", rate);
		return -EINVAL;
	}

	i2s_setting = pdm_read_reg(pdm->pdm_base, I2S_SETTING_REG);
	switch (params_width(params)) {
	case 16:
		pdm_write_reg(pdm->pdm_base, I2S_SETTING_REG,
			      (i2s_setting & I2S_CHN_WIDTH_MASK) | I2S_CHN_WIDTH(I2S_CHN_WIDTH_16BIT));
		break;
	default:
		dev_err(pdm->dev, "Cannot support this width %d\n", params_width(params));
		return -EINVAL;
	}

	return 0;
}

static int cv1835pdm_startup(struct snd_pcm_substream *substream,
			     struct snd_soc_dai *dai)
{
	struct cvi1835pdm *pdm = snd_soc_dai_get_drvdata(dai);

	dev_dbg(pdm->dev, "%s\n", __func__);

	return 0;
}

static void cv1835pdm_shutdown(struct snd_pcm_substream *substream,
			       struct snd_soc_dai *dai)
{
	struct cvi1835pdm *pdm = snd_soc_dai_get_drvdata(dai);

	dev_dbg(pdm->dev, "%s\n", __func__);
}

static int cv1835pdm_trigger(struct snd_pcm_substream *substream,
			     int cmd, struct snd_soc_dai *dai)
{
	struct cvi1835pdm *pdm = snd_soc_dai_get_drvdata(dai);
	int ret = 0;

	dev_dbg(pdm->dev, "%s, cmd=%d\n", __func__, cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		pdm_write_reg(pdm->pdm_base, PDM_EN_REG, PDM_EN);
		//i2s_start(dev, substream);
		break;

	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		pdm_write_reg(pdm->pdm_base, PDM_EN_REG, PDM_OFF);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static const struct snd_soc_dai_ops cv1835pdm_dai_ops = {
	.hw_params	= cv1835pdm_hw_params,
	.set_fmt	= cv1835pdm_set_dai_fmt,
	.startup	= cv1835pdm_startup,
	.shutdown	= cv1835pdm_shutdown,
	.trigger	= cv1835pdm_trigger,
};

static struct snd_soc_dai_driver cv1835pdm_dai = {
	.name		= "cv1835pdm",
	.capture	= {
		.stream_name	= "Capture",
		.channels_min	= 2,
		.channels_max	= 2,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = SNDRV_PCM_FMTBIT_S32_LE
		| SNDRV_PCM_FMTBIT_S24_LE
		| SNDRV_PCM_FMTBIT_S16_LE,
	},
	.ops		= &cv1835pdm_dai_ops,
};

static int cv1835pdm_component_probe(struct snd_soc_component *component)
{
	pr_info("%s\n", __func__);
	return 0;
}

static const struct snd_soc_component_driver soc_component_dev_cv1835pdm = {
	.probe = cv1835pdm_component_probe,
};

static int cv1835pdm_probe(struct platform_device *pdev)
{
	struct cvi1835pdm *pdm;
	struct resource *res;
	struct miscdevice *miscdev;
	int ret;

	dev_info(&pdev->dev, "%s\n", __func__);

	pdm = devm_kzalloc(&pdev->dev, sizeof(*pdm), GFP_KERNEL);
	if (!pdm)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pdm->pdm_base = devm_ioremap_resource(&pdev->dev, res);

	if (IS_ERR(pdm->pdm_base))
		return PTR_ERR(pdm->pdm_base);

	miscdev = &pdm->miscdev;
	miscdev->minor = MISC_DYNAMIC_MINOR;
	miscdev->name = "cv1835pdm";
	//miscdev->fops = &adc_fops;
	miscdev->parent = NULL;

	ret = misc_register(miscdev);
	if (ret) {
		pr_err("pdm: failed to register misc device.\n");
		return ret;
	}
	dev_set_drvdata(&pdev->dev, pdm);
	pdm->dev = &pdev->dev;

	return devm_snd_soc_register_component(&pdev->dev, &soc_component_dev_cv1835pdm,
					       &cv1835pdm_dai, 1);
}

#ifdef CONFIG_OF
static const struct of_device_id cvitek_pdm_of_match[] = {
	{ .compatible = "cvitek,cv1835pdm", },
	{},
};

MODULE_DEVICE_TABLE(of, cvitek_pdm_of_match);
#endif

static struct platform_driver cv1835pdm_platform_driver = {
	.probe		= cv1835pdm_probe,
	.driver		= {
		.name	= "cv1835pdm",
		.of_match_table = of_match_ptr(cvitek_pdm_of_match),
	},
};
module_platform_driver(cv1835pdm_platform_driver);

MODULE_DESCRIPTION("ASoC CVITEK CV1835PDM driver");
MODULE_AUTHOR("Ethan Chen <ethan.chen@wisecore.com.tw>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:cv1835pdm");
