// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Machine driver for CVITEK ADC on CVITEK CV182X
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
#include "../codecs/cv182xadac.h"
#include <linux/version.h>

bool proc_ai_not_allocted = true;

static int cv182x_adc_vol_list[25] = {
	ADC_VOL_GAIN_0,
	ADC_VOL_GAIN_1,
	ADC_VOL_GAIN_2,
	ADC_VOL_GAIN_3,
	ADC_VOL_GAIN_4,
	ADC_VOL_GAIN_5,
	ADC_VOL_GAIN_6,
	ADC_VOL_GAIN_7,
	ADC_VOL_GAIN_8,
	ADC_VOL_GAIN_9,
	ADC_VOL_GAIN_10,
	ADC_VOL_GAIN_11,
	ADC_VOL_GAIN_12,
	ADC_VOL_GAIN_13,
	ADC_VOL_GAIN_14,
	ADC_VOL_GAIN_15,
	ADC_VOL_GAIN_16,
	ADC_VOL_GAIN_17,
	ADC_VOL_GAIN_18,
	ADC_VOL_GAIN_19,
	ADC_VOL_GAIN_20,
	ADC_VOL_GAIN_21,
	ADC_VOL_GAIN_22,
	ADC_VOL_GAIN_23,
	ADC_VOL_GAIN_24
};

static int cv182x_adc_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	return 0;
}

static int cv182x_adc_codec_init(struct snd_soc_pcm_runtime *rtd)
{
	return 0;
}
static struct snd_soc_ops cv182x_adc_ops = {
	.hw_params = cv182x_adc_hw_params,
};

static struct snd_soc_dai_link_component cv182x_adc_cpus = {
	.name = "4100000.i2s",
	.dai_name = "4100000.i2s",

};

static struct snd_soc_dai_link_component cv182x_adc_codecs = {
	.name = "300a100.adc",
	.dai_name = "cv182xadc",

};

static struct snd_soc_dai_link_component cv182x_adc_platform = {
	.name = "4100000.i2s",
	.dai_name = "4100000.i2s",

};

static struct snd_soc_dai_link cv182x_adc_dai = {
	.name = "cv182x-i2s-adc",
	.stream_name = "cv182x-adc",
	.cpus = &cv182x_adc_cpus,
	.num_cpus = 1,
	.codecs = &cv182x_adc_codecs,
	.num_codecs = 1,
	.platforms = &cv182x_adc_platform,
	.num_platforms = 1,
	.ops = &cv182x_adc_ops,
	.init = cv182x_adc_codec_init,
	.dai_fmt = SND_SOC_DAIFMT_I2S
	| SND_SOC_DAIFMT_IB_NF
	| SND_SOC_DAIFMT_CBM_CFM,
	.capture_only = 1,
};

static struct snd_soc_card cv182x_adc = {
	//.owner = THIS_MODULE,
	.name = "cv182x-i2s-adc",
	.dai_link = &cv182x_adc_dai,
	.num_links = 1,

};


static const struct of_device_id cvi_audio_match_ids[] = {
	{
		.compatible = "cvitek,cv182x-adc",
		//.data = (void *) &cv182x_adc_dai,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, cvi_audio_match_ids);

static int cv182x_adc_proc_show(struct seq_file *m, void *v)
{
	void __iomem *i2s0;
	void __iomem *adc;
	void __iomem *audio_pll;
	void __iomem *sdma_pll;
	u32 audio_freq;
	u32 val1, val2, val3;
	u32 temp1, temp2;

	i2s0 = ioremap(0x04100000, 0x100);
	adc = ioremap(0x0300A100, 0x100);
	audio_pll = ioremap(0x3002854, 0x10);
	sdma_pll = ioremap(0x3002004, 0x10);
	if (readl(audio_pll) == 0x179EDCFA)
		audio_freq = 22579200;
	else
		audio_freq = 24576000;


	seq_puts(m, "\n------------- CVI AI ATTRIBUTE -------------\n");
	seq_puts(m, "AiDev    Workmode    SampleRate    BitWidth\n");
	val1 = (readl(i2s0) >> 1) & 0x1;
	//samplerate = audio_freq/(mclk_div * CIC_mask * 64 * cofe(4))
	val2 = audio_freq / ((readl(i2s0 + 0x64) & 0x0000ffff)*((readl(adc + AUDIO_PHY_RXADC_CTRL1) & 0x1) + 1)*64*4);
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

	val1 = (readl(adc + AUDIO_PHY_RXADC_CTRL0) &
		(AUDIO_PHY_REG_RXADC_EN_MASK | AUDIO_PHY_REG_I2S_TX_EN_MASK));
	seq_printf(m, "ADC is %s (%d)\n", val1 == 3 ? "on" : "off", val1);
	seq_puts(m, "\n");

	val1 = (readl(adc + AUDIO_PHY_RXADC_ANA2) & AUDIO_PHY_REG_MUTEL_RXPGA_MASK);
	val2 = (readl(adc + AUDIO_PHY_RXADC_ANA2) & AUDIO_PHY_REG_MUTER_RXPGA_MASK) >> 1;
	seq_puts(m, "L-Mute   R-Mute\n");
	seq_printf(m, "  %s       %s\n", val1 == 1 ? "yes" : "no", val2 == 1 ? "yes" : "no");
	seq_puts(m, "\n");

	val1 = (readl(adc + AUDIO_PHY_RXADC_ANA0) & 0xffff);
	val2 = (readl(adc + AUDIO_PHY_RXADC_ANA0) & 0xffff0000) >> 16;

	for (temp1 = 0; temp1 < 25; temp1++) {
		if (val1 == cv182x_adc_vol_list[temp1])
			break;
	}
	for (temp2 = 0; temp2 < 25; temp2++) {
		if (val2 == cv182x_adc_vol_list[temp2])
			break;
	}

	seq_puts(m, "L-Vol           R-Vol\n");
	seq_printf(m, "  %d              %d\n", temp1, temp2);
	seq_puts(m, "\n");



	iounmap(i2s0);
	iounmap(adc);
	iounmap(audio_pll);
	iounmap(sdma_pll);
	return 0;
}

static int seq_cv182x_adc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cv182x_adc_proc_show, PDE_DATA(inode));
}

static const struct proc_ops cv182x_adc_proc_ops = {
	.proc_read	= seq_read,
	.proc_open	= seq_cv182x_adc_open,
	.proc_release	= single_release,
};


static int cv182x_adc_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct device_node *np = pdev->dev.of_node;
	struct proc_dir_entry *proc_ai;


	dev_info(&pdev->dev, "%s, dev name=%s\n", __func__, dev_name(&pdev->dev));
	card = &cv182x_adc;

	if (np) {

		of_property_read_string(np, "cvi,card_name", &card->name);
		card->dev = &pdev->dev;

		if (!proc_audio_dir) {
			proc_audio_dir = proc_mkdir("audio_debug", NULL);
			if (!proc_audio_dir)
				dev_err(&pdev->dev, "Error creating audio_debug proc folder entry\n");
		}

		if (proc_audio_dir && (proc_ai_not_allocted == true)) {
			proc_ai = proc_create_data("cv182x_adc", 0444, proc_audio_dir, &cv182x_adc_proc_ops, np);
			if (!proc_ai)
				dev_err(&pdev->dev, "Create cv182x_adc proc failed!\n");
			else
				proc_ai_not_allocted = false;
		}

		platform_set_drvdata(pdev, card);
		dev_info(&pdev->dev, "%s start devm_snd_soc_register_card\n", __func__);
		return devm_snd_soc_register_card(&pdev->dev, card);
	}
	return 0;

}

#ifdef CONFIG_PM
int cv182x_cv182xadc_suspend(struct device *dev)
{
	return 0;
}

int cv182x_cv182xadc_resume(struct device *dev)
{
	return 0;
}

int cv182x_cv182xadc_poweroff(struct device *dev)
{
	return 0;
}

#else
#define cv182x_cv182xadc_suspend	NULL
#define cv182x_cv182xadc_resume		NULL
#define cv182x_cv182xadc_poweroff	NULL
#endif

const struct dev_pm_ops cv182x_cv182xadc_pm_ops = {
	.suspend = cv182x_cv182xadc_suspend,
	.resume = cv182x_cv182xadc_resume,
	.freeze = cv182x_cv182xadc_suspend,
	.thaw = cv182x_cv182xadc_resume,
	.poweroff = cv182x_cv182xadc_poweroff,
	.restore = cv182x_cv182xadc_resume,
};

static struct platform_driver cv182x_adc_driver = {
	.driver = {
		.name = "cv182x-adc",
		.pm = &cv182x_cv182xadc_pm_ops,
		.of_match_table = cvi_audio_match_ids,
	},
	.probe = cv182x_adc_probe,
};

module_platform_driver(cv182x_adc_driver);

MODULE_AUTHOR("EthanChen");
MODULE_DESCRIPTION("ALSA SoC cv182x adc driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cv182x-adc");
