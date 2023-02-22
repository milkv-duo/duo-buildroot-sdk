// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 cvitek All rights reserved.
 * Author: jinyu zhao <jinyu.zhaok@cvitek.com>
 *
 * cvitek SARADC driver for U-Boot
 */
#include <common.h>
#include <errno.h>
#include <dm.h>
#include <adc.h>
#include <asm/io.h>

#define CVITEK_ADC_MAX_CHANNELS 6
#define CVITEK_ADC_DATA_BITS 12

enum channel {
	/* Top domain ADC ch1, ch2, ch3 */
	ADC1 = 1,
	ADC2,
	ADC3,
	/* no die domain ADC ch1, ch2, ch3 */
	PWR_ADC1,/* ADC4 <== PWR_GPIO2 */
	PWR_ADC2,/* ADC5 <== PWR_GPIO1 */
	PWR_ADC3,/* ADC6 <== PWR_VBAT_DET */
};

struct cvitek_adc_regs {
	unsigned int reserved_1[1];
	unsigned int ctrl;/* 0x04 */
	unsigned int status;/* 0x08 */
	unsigned int cyc_set;/* 0x0c */
	unsigned int reserved_2[1];
	unsigned int ch1_result;/* 0x14 */
	unsigned int ch2_result;/* 0x18 */
	unsigned int ch3_result;/* 0x1c */
	unsigned int intr_en;/* 0x20 */
	unsigned int intr_clr;/* 0x24 */
	unsigned int intr_sta;/* 0x28 */
	unsigned int intr_raw;/* 0x2c */
};

struct cvitek_adc_priv {
	struct udevice *dev;		/* Device, NULL for invalid adc */
	void __iomem	*top_domain_base;
	void __iomem	*rtc_domain_base;
	int active_channel;
};

static void cvitek_adc_cyc_setting(struct cvitek_adc_regs *regs)
{
	uint32_t value;

	value = readl(&regs->cyc_set);
	value &= ~(0xf << 12);
	value |= (0xf << 12);//set saradc clock cycle=840ns
	writel(value, &regs->cyc_set);
}

int cvitek_adc_channel_data(struct udevice *dev, int channel,
			    unsigned int *data)
{
	struct adc_uclass_plat *uc_pdata = dev_get_uclass_plat(dev);
	struct cvitek_adc_priv *priv = dev_get_priv(dev);
	struct cvitek_adc_regs *regs;
	unsigned int value;

	if (channel != priv->active_channel) {
		pr_err("Requested channel is not active!");
		return -EINVAL;
	}

	switch (channel) {
	case ADC1:
	case ADC2:
	case ADC3:
		regs = (struct cvitek_adc_regs *)priv->top_domain_base;
		break;
	case PWR_ADC1:
	case PWR_ADC2:
	case PWR_ADC3:
		regs = (struct cvitek_adc_regs *)priv->rtc_domain_base;
		break;
	}

	// Trigger measurement
	value = readl(&regs->ctrl);
	writel(value | 0x1, &regs->ctrl);

	// Check busy status
	while (readl(&regs->status) & 0x1)
		;

	switch (channel) {
	case PWR_ADC1:
	case ADC1:
		value = readl(&regs->ch1_result) & uc_pdata->data_mask;
		break;
	case PWR_ADC2:
	case ADC2:
		value = readl(&regs->ch2_result) & uc_pdata->data_mask;
		break;
	case PWR_ADC3:
	case ADC3:
		value = readl(&regs->ch3_result) & uc_pdata->data_mask;
		break;
	}

	*data = value;

	return 0;
}

int cvitek_adc_start_channel(struct udevice *dev, int channel)
{
	struct cvitek_adc_priv *priv = dev_get_priv(dev);
	struct cvitek_adc_regs *regs;
	unsigned int value;

	priv->active_channel = channel;

	switch (channel) {
	case ADC1:
	case ADC2:
	case ADC3:
		regs = (struct cvitek_adc_regs *)priv->top_domain_base;
		break;
	case PWR_ADC1:
	case PWR_ADC2:
	case PWR_ADC3:
		channel -= 3;
		regs = (struct cvitek_adc_regs *)priv->rtc_domain_base;
		break;
	}

	// Disable saradc interrupt
	writel(0x0, &regs->intr_en);

	// Set saradc cycle
	cvitek_adc_cyc_setting(regs);

	// Set channel
	value = readl(&regs->ctrl);
	writel(value | (1 << (4 + channel)),
	       &regs->ctrl);

	return 0;
}

int cvitek_adc_stop(struct udevice *dev)
{
	struct cvitek_adc_priv *priv = dev_get_priv(dev);
	struct cvitek_adc_regs *top_regs, *rtcsyc_regs;
	unsigned int value;

	top_regs = (struct cvitek_adc_regs *)priv->top_domain_base;
	rtcsyc_regs = (struct cvitek_adc_regs *)priv->rtc_domain_base;

	// disable measurement
	value = readl(&top_regs->ctrl);
	writel(value & ~0x1, &top_regs->ctrl);
	value = readl(&rtcsyc_regs->ctrl);
	writel(value & ~0x1, &rtcsyc_regs->ctrl);

	priv->active_channel = -1;

	return 0;
}

int cvitek_adc_probe(struct udevice *dev)
{
	struct adc_uclass_plat *uc_pdata = dev_get_uclass_plat(dev);
	struct cvitek_adc_priv *priv = dev_get_priv(dev);
	fdt_addr_t base;

	base = dev_read_addr_index(dev, 0);
	if (base == FDT_ADDR_T_NONE) {
		pr_err("Can't get the top SARADC register base address\n");
		return -ENXIO;
	}
	priv->top_domain_base = (void *)base;
	base = dev_read_addr_index(dev, 1);
	if (base == FDT_ADDR_T_NONE) {
		pr_err("Can't get the rtcsys SARADC register base address\n");
		return -ENXIO;
	}
	priv->rtc_domain_base = (void *)base;

	priv->dev = dev;
	priv->active_channel = -1;

	uc_pdata->data_mask = (1 << CVITEK_ADC_DATA_BITS) - 1;
	uc_pdata->data_format = 0;
	uc_pdata->data_timeout_us = 15;
	/* Mask available channel bits: [1:5] */
	uc_pdata->channel_mask = (2 << CVITEK_ADC_MAX_CHANNELS) - 2;

	return 0;
}

static const struct adc_ops cvitek_adc_ops = {
	.start_channel = cvitek_adc_start_channel,
	.channel_data = cvitek_adc_channel_data,
	.stop = cvitek_adc_stop,
};

static const struct udevice_id cvitek_adc_ids[] = {
	{ .compatible = "cvitek,saradc" },
	{ }
};

U_BOOT_DRIVER(cvitek_adc) = {
	.name		= "cvitek-adc",
	.id		= UCLASS_ADC,
	.of_match	= cvitek_adc_ids,
	.ops		= &cvitek_adc_ops,
	.probe		= cvitek_adc_probe,
	.priv_auto	= sizeof(struct cvitek_adc_priv),
};
