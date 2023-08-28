// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * lt9611 codec
 *
 */

#include <linux/module.h>
#include <linux/device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <linux/io.h>
#include <linux/proc_fs.h>

static int cv1835_lt9611_hw_params(struct snd_pcm_substream *substream,
				   struct snd_pcm_hw_params *params)
{
//    printk("%s\n", __func__);
	return 0;
}

static int cv1835_lt9611_codec_init(struct snd_soc_pcm_runtime *rtd)
{
    //printk(KERN_ERR "%s\n", __func__);
	return 0;
}
static struct snd_soc_ops cv1835_lt9611_ops = {
	.hw_params = cv1835_lt9611_hw_params,
};


static struct snd_soc_dai_link_component lt9611_cpus = {
	.name = "4120000.i2s",
	.dai_name = "4120000.i2s",

};

static struct snd_soc_dai_link_component lt9611_codecs = {
	.name = "dummy_codec",
	.dai_name = "dummy_codec-aif",

};

static struct snd_soc_dai_link_component lt9611_platform = {
	.name = "4120000.i2s",
	.dai_name = "4120000.i2s",

};
static struct snd_soc_dai_link cv1835_lt9611_dai = {
	.name = "cv1835-lt9611",
	.stream_name = "cv1835-lt9611",
	.cpus = &lt9611_cpus,
	.num_cpus = 1,
	.codecs = &lt9611_codecs,
	.num_codecs = 1,
	.platforms = &lt9611_platform,
	.num_platforms = 1,
	.ops = &cv1835_lt9611_ops,
	.init = cv1835_lt9611_codec_init,
	.dai_fmt = SND_SOC_DAIFMT_I2S
	| SND_SOC_DAIFMT_IB_NF
	| SND_SOC_DAIFMT_CBM_CFM,
};

static struct snd_soc_card cv1835_lt9611 = {
	.owner = THIS_MODULE,
	.dai_link = &cv1835_lt9611_dai,
	.num_links = 1,
};

static const struct of_device_id cvi_lt9611_audio_match_ids[] = {
	{
		.compatible = "cvitek,cv1835-lt9611",
	},
	{ }
};
MODULE_DEVICE_TABLE(of, cvi_lt9611_audio_match_ids);

static int cv1835_lt9611_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct device_node *np = pdev->dev.of_node;
	int  ret = 0;

	//dev_dbg(&pdev->dev, "%s, dev name=%s\n", __func__, dev_name(&pdev->dev));
//	printk("%s, dev name=%s\n", __func__, dev_name(&pdev->dev));

	/* Ethan add, set pinmux by hard code temporary */
	card = &cv1835_lt9611;

	if (np) {

		of_property_read_string(np, "cvi,card_name", &card->name);
		card->dev = &pdev->dev;

	//	of_property_read_string(np, "cvi,cpu_dai_name", &card->dai_link[0].cpu_dai_name);
	//	of_property_read_string(np, "cvi,platform_name", &card->dai_link[0].platform_name);

		platform_set_drvdata(pdev, card);
		return devm_snd_soc_register_card(&pdev->dev, card);
	}

	return 0;
}

static struct platform_driver cv1835_lt9611_driver = {
	.driver = {
		.name = "cv1835-lt9611",
		.pm = &snd_soc_pm_ops,
		.of_match_table = cvi_lt9611_audio_match_ids,
	},
	.probe = cv1835_lt9611_probe,
};

module_platform_driver(cv1835_lt9611_driver);

MODULE_AUTHOR("EthanChen");
MODULE_DESCRIPTION("lt9611 driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv1835-lt9611");

