// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Machine driver for CVITEK DAC on CVITEK CV181X
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
#include "../codecs/cv181xadac.h"
#include <linux/version.h>

bool proc_ao_not_allocted = true;

static int cv181x_dac_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	return 0;
}

static int cv181x_dac_codec_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}

static struct snd_soc_ops cv181x_dac_ops = {
	.hw_params = cv181x_dac_hw_params,
};

static struct snd_soc_dai_link_component cv181x_dac_cpus = {
	.name = "4130000.i2s",
	.dai_name = "4130000.i2s",

};

static struct snd_soc_dai_link_component cv181x_dac_codecs = {
	.name = "300a000.dac",
	.dai_name = "cvitekadac",

};

static struct snd_soc_dai_link_component cv181x_dac_platform = {
	.name = "4130000.i2s",
	.dai_name = "4130000.i2s",

};
static struct snd_soc_dai_link cv181x_dac_dai = {
	.name = "cviteka-i2s-dac",
	.stream_name = "cviteka-dac",
	.cpus = &cv181x_dac_cpus,
	.num_cpus = 1,
	.codecs = &cv181x_dac_codecs,
	.num_codecs = 1,
	.platforms = &cv181x_dac_platform,
	.num_platforms = 1,
	.ops = &cv181x_dac_ops,
	.init = cv181x_dac_codec_init,
	.dai_fmt = SND_SOC_DAIFMT_I2S
	| SND_SOC_DAIFMT_IB_IF
	| SND_SOC_DAIFMT_CBS_CFS,
	.playback_only = 1,
};


static struct snd_soc_card cv181x_dac = {
	.owner = THIS_MODULE,
	.dai_link = &cv181x_dac_dai,
	.num_links = 1,

};


static int cv181x_dac_proc_show(struct seq_file *m, void *v)
{

	void __iomem *i2s3;
	void __iomem *dac;
	void __iomem *audio_pll;
	void __iomem *sdma_pll;
	u32 audio_freq;
	u32 val1, val2, val3;

	i2s3 = ioremap(0x04130000, 0x100);
	dac = ioremap(0x0300A000, 0x100);
	audio_pll = ioremap(0x3002854, 0x10);
	sdma_pll = ioremap(0x3002004, 0x10);
	if (readl(audio_pll) == 0x179EDCFA)//406.4256MHz
		audio_freq = 22579200;
	else if (readl(audio_pll) == 0x249f0000) {
		audio_freq = 16384000;
	} else
		audio_freq = 24576000;

	seq_puts(m, "\n------------- CVI AO ATTRIBUTE -------------\n");
	seq_puts(m, "AiDev    Workmode    SampleRate    BitWidth\n");
	val1 = (readl(i2s3) >> 1) & 0x1;
	val2 = audio_freq/((readl(i2s3 + 0x64) & 0x0000ffff)*(0x1 << (readl(dac + AUDIO_PHY_TXDAC_CTRL1) & 0x3))*64*4);
	val3 = ((readl(i2s3 + 0x10) >> 1) & 0x3) * 16;
	seq_printf(m, "  %d       %s        %6d        %2d\n", 1, val1 == 0 ? "slave" : "master", val2, val3);
	seq_puts(m, "\n");
	seq_puts(m, "-------------  CVI AO STATUS   -------------\n");
	val1 = (readl(i2s3 + 0x18));
	seq_printf(m, "I2S3 is %s\n", val1 == 1 ? "on" : "off");
	seq_puts(m, "\n");

	val1 = (readl(sdma_pll) & 0x00000002) >> 1;
	seq_printf(m, "SDMA clk is %s\n", val1 == 1 ? "on" : "off");

	val1 = (readl(dac + AUDIO_PHY_TXDAC_CTRL0)
		& (AUDIO_PHY_REG_TXDAC_EN_MASK | AUDIO_PHY_REG_I2S_RX_EN_MASK));
	seq_printf(m, "DAC is %s (%d)\n", val1 == 3 ? "on" : "off", val1);

	val2 = (readl(dac + AUDIO_PHY_TXDAC_ANA2) & AUDIO_PHY_REG_DA_DEML_TXDAC_OW_EN_MASK) >> 16;
	val3 = (readl(dac + AUDIO_PHY_TXDAC_ANA2) & AUDIO_PHY_REG_DA_DEMR_TXDAC_OW_EN_MASK) >> 17;
	seq_puts(m, "L-Mute   R-Mute\n");
	seq_printf(m, "  %s       %s\n", val2 == 1 ? "yes" : "no", val3 == 1 ? "yes" : "no");
	seq_puts(m, "\n");

	val2 = ((readl(dac + AUDIO_PHY_TXDAC_AFE1) & AUDIO_PHY_REG_TXDAC_GAIN_UB_0_MASK) + 1)
					/ CV181X_DAC_VOL_STEP;
	val3 = (((readl(dac + AUDIO_PHY_TXDAC_AFE1) & AUDIO_PHY_REG_TXDAC_GAIN_UB_1_MASK) >> 16) + 1)
					/ CV181X_DAC_VOL_STEP;
	seq_puts(m, "L-Vol           R-Vol\n");
	seq_printf(m, "  %d             %d\n", val2, val3);
	seq_puts(m, "\n");

	iounmap(i2s3);
	iounmap(dac);
	iounmap(audio_pll);
	iounmap(sdma_pll);
	return 0;
}

static int seq_cv181x_dac_open(struct inode *inode, struct file *file)
{
	return single_open(file, cv181x_dac_proc_show, PDE_DATA(inode));
}

static const struct proc_ops cv181x_dac_proc_ops = {
	.proc_read	= seq_read,
	.proc_open	= seq_cv181x_dac_open,
	.proc_release	= single_release,
};

static const struct of_device_id cvi_audio_match_ids[] = {
	{
		.compatible = "cvitek,cv182xa-dac",
		//.data = (void *) &cv182xa_dac_dai,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, cvi_audio_match_ids);

static int cv181x_dac_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct device_node *np = pdev->dev.of_node;
	struct proc_dir_entry *proc_ao;

	dev_info(&pdev->dev, "cviteka_dac_probe, dev name=%s\n", dev_name(&pdev->dev));

	/* Ethan add, set pinmux by hard code temporary */
	card = &cv181x_dac;

	if (np) {

		of_property_read_string(np, "cvi,card_name", &card->name);
		card->dev = &pdev->dev;

		if (!proc_audio_dir) {
			proc_audio_dir = proc_mkdir("audio_debug", NULL);
			if (!proc_audio_dir)
				dev_err(&pdev->dev, "Error creating audio_debug proc folder entry\n");
		}

		if (proc_audio_dir && (proc_ao_not_allocted == true)) {
			proc_ao = proc_create_data("cviteka_dac", 0444, proc_audio_dir, &cv181x_dac_proc_ops, np);
			if (!proc_ao)
				dev_err(&pdev->dev, "Create cviteka_dac proc failed!\n");
			proc_ao_not_allocted = false;
		}

		platform_set_drvdata(pdev, card);
		return devm_snd_soc_register_card(&pdev->dev, card);
	}
	return 0;

}

#ifdef CONFIG_PM
int cv181x_cv181xdac_suspend(struct device *dev)
{
	return 0;
}

int cv181x_cv181xdac_resume(struct device *dev)
{
	return 0;
}

int cv181x_cv181xdac_poweroff(struct device *dev)
{
	return 0;
}

#else
#define cv181x_cv181xdac_suspend	NULL
#define cv181x_cv181xdac_resume		NULL
#define cv181x_cv181xdac_poweroff	NULL
#endif

const struct dev_pm_ops cv181x_cv181xdac_pm_ops = {
	.suspend = cv181x_cv181xdac_suspend,
	.resume = cv181x_cv181xdac_resume,
	.freeze = cv181x_cv181xdac_suspend,
	.thaw = cv181x_cv181xdac_resume,
	.poweroff = cv181x_cv181xdac_poweroff,
	.restore = cv181x_cv181xdac_resume,
};

static struct platform_driver cv181x_dac_driver = {
	.driver = {
		.name = "cviteka-dac",
		.pm = &cv181x_cv181xdac_pm_ops,
		.of_match_table = cvi_audio_match_ids,
	},
	.probe = cv181x_dac_probe,
};

module_platform_driver(cv181x_dac_driver);

MODULE_AUTHOR("EthanChen");
MODULE_DESCRIPTION("ALSA SoC cviteka dac driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cviteka-dac");
