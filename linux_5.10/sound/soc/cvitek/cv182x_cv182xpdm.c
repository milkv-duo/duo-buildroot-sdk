// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Machine driver for CVITEK PDM on CVITEK CV182X
 *
 * Copyright 2019 CVITEK
 *
 * Author: EthanChen
 *
 */

#include <linux/module.h>
#include <linux/device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <linux/io.h>
#include "cv1835pdm.h"

static int cv182x_pdm_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	return 0;
}

static int cv182x_pdm_codec_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}
static struct snd_soc_ops cv182x_pdm_ops = {
	.hw_params = cv182x_pdm_hw_params,
};

static struct snd_soc_dai_link cv182x_pdm_dai = {
	.name = "cv182x-i2s-pdm",
	.stream_name = "cv182x-pdm",
	.cpu_dai_name = "4110000.i2s",
	.codec_dai_name = "cv1835pdm",
	.platform_name = "4110000.i2s",
	.codec_name = "41d0c00.pdm",
	.ops = &cv182x_pdm_ops,
	.init = cv182x_pdm_codec_init,
	.dai_fmt = SND_SOC_DAIFMT_I2S
	| SND_SOC_DAIFMT_IB_NF
	| SND_SOC_DAIFMT_CBM_CFM,
};

static struct snd_soc_card cv182x_pdm = {
	.owner = THIS_MODULE,
	.dai_link = &cv182x_pdm_dai,
	.num_links = 1,

};


static const struct of_device_id cvi_audio_match_ids[] = {
	{
		.compatible = "cvitek,cv182x-pdm",
		//.data = (void *) &cv182x_pdm_dai,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, cvi_audio_match_ids);

static int cv182x_pdm_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct device_node *np = pdev->dev.of_node;

	dev_info(&pdev->dev, "%s, dev name=%s\n", __func__, dev_name(&pdev->dev));

	card = &cv182x_pdm;

	if (np) {

		of_property_read_string(np, "cvi,card_name", &card->name);
		card->dev = &pdev->dev;
		platform_set_drvdata(pdev, card);
		return devm_snd_soc_register_card(&pdev->dev, card);
	}
	return 0;

}

static struct platform_driver cv182x_pdm_driver = {
	.driver = {
		.name = "cv182x-pdm",
		.pm = &snd_soc_pm_ops,
		.of_match_table = cvi_audio_match_ids,
	},
	.probe = cv182x_pdm_probe,
};

module_platform_driver(cv182x_pdm_driver);

MODULE_AUTHOR("EthanChen");
MODULE_DESCRIPTION("ALSA SoC cv182x pdm driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv182x-pdm");
