// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Machine driver for CVITEK PDM on CVITEK CV1835
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

static int cv1835_pdm_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	return 0;
}

static int cv1835_pdm_codec_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}
static struct snd_soc_ops cv1835_pdm_ops = {
	.hw_params = cv1835_pdm_hw_params,
};
static struct snd_soc_dai_link_component cv1835_pdm_cpus = {
	.name = "4110000.i2s",
	.dai_name = "4110000.i2s",

};

static struct snd_soc_dai_link_component cv1835_pdm_codecs = {
	.name = "41d0c00.pdm",
	.dai_name = "cv1835pdm",

};

static struct snd_soc_dai_link_component cv1835_pdm_platform = {
	.name = "4110000.i2s",
	.dai_name = "4110000.i2s",

};
static struct snd_soc_dai_link cv1835_pdm_dai = {
	.name = "cv1835-i2s-1",
	.stream_name = "cv1835-pdm",
	.cpus = &cv1835_pdm_cpus,
	.num_cpus = 1,
	.codecs = &cv1835_pdm_codecs,
	.num_codecs = 1,
	.platforms = &cv1835_pdm_platform,
	.num_platforms = 1,
	.ops = &cv1835_pdm_ops,
	.init = cv1835_pdm_codec_init,
	.dai_fmt = SND_SOC_DAIFMT_I2S
	| SND_SOC_DAIFMT_IB_NF
	| SND_SOC_DAIFMT_CBM_CFM,
};


static struct snd_soc_card cv1835_pdm = {
	.owner = THIS_MODULE,
	.dai_link = &cv1835_pdm_dai,
	.num_links = 1,

};


static const struct of_device_id cvi_audio_match_ids[] = {
	{
		.compatible = "cvitek,cv182x-pdm",
		//.data = (void *) &cv1835_pdm_dai,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, cvi_audio_match_ids);

static int cv1835_pdm_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct device_node *np = pdev->dev.of_node;

	dev_info(&pdev->dev, "%s, dev name=%s\n", __func__, dev_name(&pdev->dev));

	card = &cv1835_pdm;

	if (np) {

		of_property_read_string(np, "cvi,card_name", &card->name);
		card->dev = &pdev->dev;
		platform_set_drvdata(pdev, card);
		return devm_snd_soc_register_card(&pdev->dev, card);
	}
	return 0;

}

static struct platform_driver cv1835_pdm_driver = {
	.driver = {
		.name = "cv1835-pdm",
		.pm = &snd_soc_pm_ops,
		.of_match_table = cvi_audio_match_ids,
	},
	.probe = cv1835_pdm_probe,
};

module_platform_driver(cv1835_pdm_driver);

MODULE_AUTHOR("EthanChen");
MODULE_DESCRIPTION("ALSA SoC cv1835 pdm driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv1835-pdm");
