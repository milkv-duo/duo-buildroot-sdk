// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Machine driver for CVITEK ADC on CVITEK CV1835
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
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include "../codecs/cv1835adc.h"

bool proc_ai_not_allocted = true;

static int cv1835_adc_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	return 0;
}

static int cv1835_adc_codec_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}
static struct snd_soc_ops cv1835_adc_ops = {
	.hw_params = cv1835_adc_hw_params,
};

static struct snd_soc_dai_link cv1835_adc_dai = {
	.name = "cv1835-i2s-adc",
	.stream_name = "cv1835-adc",
	.cpu_dai_name = "4100000.i2s",
	.codec_dai_name = "cv1835adc",
	.platform_name = "4100000.i2s",
	.codec_name = "300a000.adc",
	.ops = &cv1835_adc_ops,
	.init = cv1835_adc_codec_init,
	.dai_fmt = SND_SOC_DAIFMT_LEFT_J
	| SND_SOC_DAIFMT_NB_IF
	| SND_SOC_DAIFMT_CBM_CFM,
};


static struct snd_soc_card cv1835_adc = {
	.owner = THIS_MODULE,
	.dai_link = &cv1835_adc_dai,
	.num_links = 1,

};


static const struct of_device_id cvi_audio_match_ids[] = {
	{
		.compatible = "cvitek,cv1835-adc",
		//.data = (void *) &cv1835_adc_dai,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, cvi_audio_match_ids);

static int cv183x_adc_proc_show(struct seq_file *m, void *v)
{
	void __iomem *i2s0;
	void __iomem *adc;
	void __iomem *audio_pll;
	void __iomem *sdma_pll;
	u32 audio_freq;
	u32 val1, val2, val3;

	i2s0 = ioremap(0x04100000, 0x100);
	adc = ioremap(0x0300A000, 0x100);
	audio_pll = ioremap(0x3002854, 0x10);
	sdma_pll = ioremap(0x3002004, 0x10);
	if (readl(audio_pll) == 0x179EDCFA)
		audio_freq = 22579200;
	else
		audio_freq = 24576000;


	seq_puts(m, "\n------------- CVI AI ATTRIBUTE -------------\n");
	seq_puts(m, "AiDev    Workmode    SampleRate    BitWidth\n");
	val1 = (readl(i2s0) >> 1) & 0x1;
	val2 = audio_freq / ((readl(i2s0 + 0x64) >> 16) * ((readl(i2s0 + 0x4) & 0x000001ff) + 1));
	val3 = ((readl(i2s0 + 0x10) >> 1) & 0x3) * 16;
	seq_printf(m, "  %d       %s        %6d        %2d\n", 0, val1 == 0 ? "slave" : "master", val2, val3);
	seq_puts(m, "\n");
	seq_puts(m, "-------------  CVI AI STATUS   -------------\n");

	val1 = (readl(i2s0 + 0x18));
	seq_printf(m, "I2S0 is %s\n", val1 == 1 ? "on" : "off");
	seq_puts(m, "\n");
	val1 = (readl(sdma_pll) & 0x00000002) >> 1;
	seq_printf(m, "SDMA clk is %s\n", val1 == 1 ? "on" : "off");
	seq_puts(m, "\n");

	val1 = (readl(adc + 0x4) & 0x00000004) >> 2;
	val2 = (readl(adc + 0x4) & 0x00000008) >> 3;
	seq_puts(m, "L-Power       R-Power\n");
	seq_printf(m, "  %s          %s\n", val1 == 1 ? "on" : "off",
		   val2 == 1 ? "on" : "off");
	seq_puts(m, "\n");

	val1 = (readl(adc) & 0x07000000) >> 24;
	val2 = (readl(adc) & 0x00070000) >> 16;
	seq_puts(m, "L-Mute   R-Mute      L-Vol           R-Vol\n");
	seq_printf(m, "  %s       %s          %d              %d\n", val1 == 7 ? "yes" : "no", val2 == 7 ? "yes" : "no",
		   (7 - val1), (7 - val2));
	seq_puts(m, "\n");

	val1 = (readl(adc + 0x4) & 0x00000080) >> 7;
	val2 = (readl(adc + 0x4) & 0x00000040) >> 6;
	val3 = (readl(adc + 0x20) & 0x00000001);
	seq_puts(m, "L-HighPass       R-HighPass    AGC\n");
	seq_printf(m, "  %s          %s       %s\n", val1 == 1 ? "enable" : "disable",
		   val2 == 1 ? "enable" : "disable", val3 == 1 ? "enable" : "disable");
	seq_puts(m, "\n");

	iounmap(i2s0);
	iounmap(adc);
	iounmap(audio_pll);
	iounmap(sdma_pll);
	return 0;
}

static int seq_cv183x_adc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cv183x_adc_proc_show, PDE_DATA(inode));
}
static const struct file_operations cv183x_adc_proc_ops = {
	.owner  = THIS_MODULE,
	.open = seq_cv183x_adc_open,
	.read = seq_read,
	.release = single_release,
};

static int cv1835_adc_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct device_node *np = pdev->dev.of_node;
	struct proc_dir_entry *proc_ai;


	dev_dbg(&pdev->dev, "%s, dev name=%s\n", __func__, dev_name(&pdev->dev));

	card = &cv1835_adc;

	if (np) {

		of_property_read_string(np, "cvi,card_name", &card->name);
		card->dev = &pdev->dev;

		if (!proc_audio_dir) {
			proc_audio_dir = proc_mkdir("audio_debug", NULL);
			if (!proc_audio_dir)
				dev_err(&pdev->dev, "Error creating audio_debug proc folder entry\n");
		}

		if (proc_audio_dir && (proc_ai_not_allocted == true)) {
			proc_ai = proc_create_data("cv183x_adc", 0444, proc_audio_dir, &cv183x_adc_proc_ops, np);
			if (!proc_ai)
				dev_err(&pdev->dev, "Create cv183x_adc proc failed!\n");
			else
				proc_ai_not_allocted = false;
		}

		platform_set_drvdata(pdev, card);
		return devm_snd_soc_register_card(&pdev->dev, card);
	}
	return 0;

}

static struct platform_driver cv1835_adc_driver = {
	.driver = {
		.name = "cv1835-adc",
		.pm = &snd_soc_pm_ops,
		.of_match_table = cvi_audio_match_ids,
	},
	.probe = cv1835_adc_probe,
};

module_platform_driver(cv1835_adc_driver);

MODULE_AUTHOR("EthanChen");
MODULE_DESCRIPTION("ALSA SoC cv1835 adc driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv1835-adc");
