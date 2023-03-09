// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Audio Dummy driver on CVITEK CV1835
 *
 * Copyright 2021 CVITEK
 *
 * Author: RuiLong
 *
 */

#include <linux/module.h>
#include <linux/device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <linux/io.h>
#include <linux/delay.h>

u16 tdm_slot_no2 = 2;

static int cv1835_dummy_hw_params(struct snd_pcm_substream *substream,
				  struct snd_pcm_hw_params *params)
{
	return 0;
}

static struct snd_soc_ops cv1835_dummy_ops = {
	.hw_params = cv1835_dummy_hw_params,
};

static struct snd_soc_dai_link cv1835_dummy_dai[] = {
	{
		.ops = &cv1835_dummy_ops,
	},
#if defined(CONFIG_SND_SOC_CV1835_CONCURRENT_I2S)
	{
		.ops = &cv1835_dummy_ops,
	},
#endif
};

static struct snd_soc_card cv1835_dummy = {
	.owner = THIS_MODULE,
	.dai_link = cv1835_dummy_dai,
	.num_links = ARRAY_SIZE(cv1835_dummy_dai),
};

static const struct of_device_id cvi_dummy_match_ids[] = {
	{
		.compatible = "cvitek,cv1835-dummy-card",
	},
};
MODULE_DEVICE_TABLE(of, cvi_dummy_match_ids);



static int cv1835_dummy_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct device_node *np = pdev->dev.of_node, *dai;
	const char *mode;
	const char *fmt;
	const char *role;
	u8 idx = 0;
	u32	slot_no;

	dev_info(&pdev->dev, "%s dev name=%s\n",  __func__, dev_name(&pdev->dev));
	card = &cv1835_dummy;

	if (np) {

		of_property_read_string(np, "cvi,card_name", &card->name);
		of_property_read_u32(np, "cvi,slot_no", &slot_no);
		of_property_read_string(np, "cvi,mode", &mode);
		of_property_read_string(np, "cvi,fmt", &fmt);

		for_each_child_of_node(np, dai) {
			of_property_read_string(dai, "cvi,dai_name", &card->dai_link[idx].name);
			of_property_read_string(dai, "cvi,stream_name", &card->dai_link[idx].stream_name);
			of_property_read_string(dai, "cvi,cpu_dai_name", &card->dai_link[idx].cpu_dai_name);

			of_property_read_string(dai, "cvi,codec_dai_name", &card->dai_link[idx].codec_dai_name);
			of_property_read_string(dai, "cvi,platform_name", &card->dai_link[idx].platform_name);
			of_property_read_string(dai, "cvi,codec_name", &card->dai_link[idx].codec_name);

			card->dev = &pdev->dev;

			of_property_read_string(dai, "cvi,role", &role);

			if (!strcmp(mode, "I2S"))
				card->dai_link[idx].dai_fmt = SND_SOC_DAIFMT_I2S;
			else if (!strcmp(mode, "LEFT_J"))
				card->dai_link[idx].dai_fmt = SND_SOC_DAIFMT_LEFT_J;
			else if (!strcmp(mode, "RIGHT_J"))
				card->dai_link[idx].dai_fmt = SND_SOC_DAIFMT_RIGHT_J;
			else if (!strcmp(mode, "DSP_A"))
				card->dai_link[idx].dai_fmt = SND_SOC_DAIFMT_DSP_A; /* PCM and TDM belong to it */
			else if (!strcmp(mode, "DSP_B"))
				card->dai_link[idx].dai_fmt = SND_SOC_DAIFMT_DSP_B; /* PCM and TDM belong to it */
			else if (!strcmp(mode, "PDM"))
				card->dai_link[idx].dai_fmt = SND_SOC_DAIFMT_PDM;
			else
				dev_err(&pdev->dev, "%s, not support this mode\n", __func__);

			if ((!strcmp(mode, "I2S")) || (!strcmp(mode, "LEFT_J")) || (!strcmp(mode, "RIGHT_J"))) {
				if (!strcmp(fmt, "IBNF"))
					card->dai_link[idx].dai_fmt |= SND_SOC_DAIFMT_IB_NF;
				else if (!strcmp(fmt, "IBIF"))
					card->dai_link[idx].dai_fmt |= SND_SOC_DAIFMT_IB_IF;
				else if (!strcmp(fmt, "NBNF"))
					card->dai_link[idx].dai_fmt |= SND_SOC_DAIFMT_NB_NF;
				else if (!strcmp(fmt, "NBIF"))
					card->dai_link[idx].dai_fmt |= SND_SOC_DAIFMT_NB_IF;
				else
					dev_err(&pdev->dev, "%s, not support this sample format\n", __func__);
			} else
				card->dai_link[idx].dai_fmt |= SND_SOC_DAIFMT_NB_IF;
			/* DSP_A, DSP_B and PDM(TDM) use NB_IF format */

			if (!strcmp(role, "master"))
				card->dai_link[idx].dai_fmt |= SND_SOC_DAIFMT_CBS_CFS;
			else if (!strcmp(role, "slave"))
				card->dai_link[idx].dai_fmt |= SND_SOC_DAIFMT_CBM_CFM;
			else
				dev_err(&pdev->dev, "%s, not support this role\n", __func__);


			if (!strcmp(mode, "PDM"))
				tdm_slot_no2 = slot_no; /* tdm_slot_no is only valid when mode is TDM/PDM */
			else if (slot_no != 2)
				dev_err(&pdev->dev, "Wrong solt number setting in %s mode\n", mode);

			dev_info(&pdev->dev, "%s, set DAI fmt to 0x%08x\n", __func__, card->dai_link[idx].dai_fmt);
			idx++;
		}

		platform_set_drvdata(pdev, card);
		return devm_snd_soc_register_card(&pdev->dev, card);
	}
	return 0;

}

static struct platform_driver cv1835_dummy_driver = {
	.driver = {
		.name = "cv1835-dummy",
		.pm = &snd_soc_pm_ops,
		.of_match_table = cvi_dummy_match_ids,
	},
	.probe = cv1835_dummy_probe,
};

module_platform_driver(cv1835_dummy_driver);

MODULE_AUTHOR("RuiLong.Chen <ruilong.chen@cvitek.com>");
MODULE_DESCRIPTION("ALSA SoC cv1835 dummy driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv1835-dummy");

