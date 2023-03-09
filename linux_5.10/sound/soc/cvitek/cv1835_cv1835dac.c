// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Machine driver for CVITEK DAC on CVITEK CV1835
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
#include "../codecs/cv1835dac.h"

bool proc_ao_not_allocted = true;

static int cv1835_dac_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	return 0;
}

static int cv1835_dac_codec_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}
static struct snd_soc_ops cv1835_dac_ops = {
	.hw_params = cv1835_dac_hw_params,
};

static struct snd_soc_dai_link cv1835_dac_dai = {
	.name = "cv1835-i2s-dac",
	.stream_name = "cv1835-dac",
	.cpu_dai_name = "4130000.i2s",
	.codec_dai_name = "cv1835dac",
	.platform_name = "4130000.i2s",
	.codec_name = "300a400.dac",
	.ops = &cv1835_dac_ops,
	.init = cv1835_dac_codec_init,
	.dai_fmt = SND_SOC_DAIFMT_I2S
	| SND_SOC_DAIFMT_IB_NF
	| SND_SOC_DAIFMT_CBS_CFS,
};


static struct snd_soc_card cv1835_dac = {
	.owner = THIS_MODULE,
	.dai_link = &cv1835_dac_dai,
	.num_links = 1,

};


static int cv183x_dac_proc_show(struct seq_file *m, void *v)
{

	void __iomem *i2s3;
	void __iomem *dac;
	void __iomem *audio_pll;
	void __iomem *sdma_pll;
	u32 audio_freq;
	u32 val1, val2, val3;

	i2s3 = ioremap(0x04130000, 0x100);
	dac = ioremap(0x0300A400, 0x100);
	audio_pll = ioremap(0x3002854, 0x10);
	sdma_pll = ioremap(0x3002004, 0x10);
	if (readl(audio_pll) == 0x179EDCFA)
		audio_freq = 22579200;
	else
		audio_freq = 24576000;


	seq_puts(m, "\n------------- CVI AO ATTRIBUTE -------------\n");
	seq_puts(m, "AiDev    Workmode    SampleRate    BitWidth\n");
	val1 = (readl(i2s3) >> 1) & 0x1;
	val2 = audio_freq / ((readl(i2s3 + 0x64) >> 16) * ((readl(i2s3 + 0x4) & 0x000001ff) + 1));
	val3 = ((readl(i2s3 + 0x10) >> 1) & 0x3) * 16;
	seq_printf(m, "  %d       %s        %6d        %2d\n", 1, val1 == 0 ? "slave" : "master", val2, val3);
	seq_puts(m, "\n");
	seq_puts(m, "-------------  CVI AO STATUS   -------------\n");
	val1 = (readl(i2s3 + 0x18));
	seq_printf(m, "I2S3 is %s\n", val1 == 1 ? "on" : "off");
	seq_puts(m, "\n");

	val1 = (readl(sdma_pll) & 0x00000002) >> 1;
	seq_printf(m, "SDMA clk is %s\n", val1 == 1 ? "on" : "off");

	val1 = (readl(dac + 0x4) & 0x2) >> 1;
	val2 = (readl(dac + 0x4) & 0x1);
	val3 = (readl(dac + 0x4) & 0x0000f000) >> 12;
	seq_puts(m, "L-Mute   R-Mute      L-Vol           R-Vol\n");
	seq_printf(m, "  %s       %s        %d              %d\n", val1 == 1 ? "yes" : "no", val2 == 1 ? "yes" : "no",
		   (15 - val3), (15 - val3));
	seq_puts(m, "\n");
	val1 = (readl(dac + 0x4) & 0x00000008) >> 3;
	seq_puts(m, "De-emphasis\n");
	seq_printf(m, "  %s\n", val1 == 0 ? "enable" : "disable");
	seq_puts(m, "\n");

	iounmap(i2s3);
	iounmap(dac);
	iounmap(audio_pll);
	iounmap(sdma_pll);
	return 0;
}

static int seq_cv183x_dac_open(struct inode *inode, struct file *file)
{
	return single_open(file, cv183x_dac_proc_show, PDE_DATA(inode));
}
static const struct file_operations cv183x_dac_proc_ops = {
	.owner  = THIS_MODULE,
	.open = seq_cv183x_dac_open,
	.read = seq_read,
	.release = single_release,
};

static const struct of_device_id cvi_audio_match_ids[] = {
	{
		.compatible = "cvitek,cv1835-dac",
		//.data = (void *) &cv1835_dac_dai,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, cvi_audio_match_ids);

static int cv1835_dac_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct device_node *np = pdev->dev.of_node;
	struct proc_dir_entry *proc_ao;

	dev_dbg(&pdev->dev, "%s, dev name=%s\n", __func__, dev_name(&pdev->dev));

	/* Ethan add, set pinmux by hard code temporary */
	card = &cv1835_dac;

	if (np) {

		of_property_read_string(np, "cvi,card_name", &card->name);
		card->dev = &pdev->dev;

		if (!proc_audio_dir) {
			proc_audio_dir = proc_mkdir("audio_debug", NULL);
			if (!proc_audio_dir)
				dev_err(&pdev->dev, "Error creating audio_debug proc folder entry\n");
		}

		if (proc_audio_dir && (proc_ao_not_allocted == true)) {
			proc_ao = proc_create_data("cv183x_dac", 0444, proc_audio_dir, &cv183x_dac_proc_ops, np);
			if (!proc_ao)
				dev_err(&pdev->dev, "Create cv183x_dac proc failed!\n");
			proc_ao_not_allocted = false;
		}

		platform_set_drvdata(pdev, card);
		return devm_snd_soc_register_card(&pdev->dev, card);
	}
	return 0;

}

static struct platform_driver cv1835_dac_driver = {
	.driver = {
		.name = "cv1835-dac",
		.pm = &snd_soc_pm_ops,
		.of_match_table = cvi_audio_match_ids,
	},
	.probe = cv1835_dac_probe,
};

module_platform_driver(cv1835_dac_driver);

MODULE_AUTHOR("EthanChen");
MODULE_DESCRIPTION("ALSA SoC cv1835 dac driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv1835-dac");
