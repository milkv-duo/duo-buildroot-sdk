// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 cvitek All rights reserved.
 * Author: jinyu zhao <jinyu.zhaok@cvitek.com>
 *
 * cvitek PWM driver for U-Boot
 */
#include <common.h>
#include <dm.h>
#include <errno.h>
#include <pwm.h>
#include <asm/io.h>
#include <log.h>

#define CVITEK_PWM_CLK_MHZ 100

enum {
	CHANNEL0 = 0,
	CHANNEL1,
	CHANNEL2,
	CHANNEL3,
	MAX_CHANNEL,
};

struct cvitek_pwm_regs {
	unsigned int hlperiod0;/* 0x00 */
	unsigned int period0;/* 0x04 */
	unsigned int hlperiod1;/* 0x08 */
	unsigned int period1;/* 0x0c */
	unsigned int hlperiod2;/* 0x10 */
	unsigned int period2;/* 0x14 */
	unsigned int hlperiod3;/* 0x18 */
	unsigned int period3;/* 0x1c */
	unsigned int reserved_1[8];
	unsigned int polarity;/* 0x40 */
	unsigned int pwmstart;/* 0x44 */
	unsigned int pwmdone;/* 0x48 */
	unsigned int pwmupdate;/* 0x4c */
	unsigned int pcount0;/* 0x50 */
	unsigned int pcount1;/* 0x54 */
	unsigned int pcount2;/* 0x58 */
	unsigned int pcount3;/* 0x5c */
	unsigned int pulsecount0;/* 0x60 */
	unsigned int pulsecount1;/* 0x64 */
	unsigned int pulsecount2;/* 0x68 */
	unsigned int pulsecount3;/* 0x6c */
	unsigned int reserved_2[4];
	unsigned int shiftcount0;/* 0x80 */
	unsigned int shiftcount1;/* 0x84 */
	unsigned int shiftcount2;/* 0x88 */
	unsigned int shiftcount3;/* 0x8c */
	unsigned int shiftstart;/* 0x90 */
	unsigned int reserved_3[15];
	unsigned int pwm_oe;/* 0xd0 */
};

struct pwm_chip_priv {
	const char *chip_name;
	struct udevice *dev;		/* Device, NULL for invalid pwm */
	void __iomem	*base;
	int pwm_base;			/* this device pwm base number */
	int pwm_count;
	unsigned long flags;
};

static int cvitek_pwm_set_config(struct udevice *dev, uint channel, uint period_ns, uint duty_ns)
{
	struct pwm_chip_priv *chip = dev_get_priv(dev);
	struct cvitek_pwm_regs *regs = (struct cvitek_pwm_regs *)chip->base;
	unsigned int period_val, hlperiod_val;

	if (channel < 0 || channel >= chip->pwm_count)
		return -EINVAL;
	if (duty_ns <= 0)
		duty_ns = 1;
	if (duty_ns >= period_ns)
		duty_ns = period_ns - 1;

	period_val = CVITEK_PWM_CLK_MHZ * period_ns / 1000;
	hlperiod_val = CVITEK_PWM_CLK_MHZ * (period_ns - duty_ns) / 1000;

	switch (channel) {
	case CHANNEL0:
		/* set duty cycles */
		writel(hlperiod_val, &regs->hlperiod0);
		/* set period cycles */
		writel(period_val, &regs->period0);
		break;
	case CHANNEL1:
		/* set duty cycles */
		writel(hlperiod_val, &regs->hlperiod1);
		/* set period cycles */
		writel(period_val, &regs->period1);
		break;
	case CHANNEL2:
		/* set duty cycles */
		writel(hlperiod_val, &regs->hlperiod2);
		/* set period cycles */
		writel(period_val, &regs->period2);
		break;
	case CHANNEL3:
		/* set duty cycles */
		writel(hlperiod_val, &regs->hlperiod3);
		/* set period cycles */
		writel(period_val, &regs->period3);
		break;
	default:
		break;
	}

	return 0;
}

static int cvitek_pwm_set_enable(struct udevice *dev, uint channel, bool enable)
{
	struct pwm_chip_priv *chip = dev_get_priv(dev);
	struct cvitek_pwm_regs *regs = (struct cvitek_pwm_regs *)chip->base;
	unsigned int value;

	if (channel < 0 || channel >= chip->pwm_count)
		return -EINVAL;

	value = readl(&regs->pwm_oe);
	writel(value | (0x1 << channel), &regs->pwm_oe);

	value = readl(&regs->pwmstart);
	writel(value & ~(0x1 << channel), &regs->pwmstart);

	/* enable pwmstart */
	if (enable)
		writel(value | (0x1 << channel), &regs->pwmstart);

	return 0;
}

static int cvitek_pwm_set_invert(struct udevice *dev, uint channel, bool polarity)
{
	struct pwm_chip_priv *chip = dev_get_priv(dev);
	struct cvitek_pwm_regs *regs = (struct cvitek_pwm_regs *)chip->base;
	unsigned int value;

	if (channel < 0 || channel >= chip->pwm_count)
		return -EINVAL;

	value = readl(&regs->polarity);
	/* polarity: default high level output */
	value = polarity ? value | (0x1 << channel) :
				value & ~(0x1 << channel);
	writel(value, &regs->polarity);

	return 0;
}

static const struct pwm_ops cvitek_pwm_ops = {
	.set_config	= cvitek_pwm_set_config,
	.set_enable	= cvitek_pwm_set_enable,
	.set_invert	= cvitek_pwm_set_invert,
};

static int cvitek_pwm_probe(struct udevice *dev)
{
	struct pwm_chip_priv *chip = dev_get_priv(dev);
	const char *name;
	fdt_addr_t base;

	base = dev_read_addr(dev);
	if (base == FDT_ADDR_T_NONE) {
		pr_err("Can't get the PWM register base address\n");
		return -ENXIO;
	}

	chip->base = (void *)base;
	chip->pwm_count = MAX_CHANNEL;

	/* Ensure that we have a base for each bank */
	name = dev_read_name(dev);
	if (!name)
		return -ENOENT;

	chip->pwm_base = dev->seq_ * chip->pwm_count;
	if (chip->pwm_base < 0)
		return -ENOENT;

	return 0;
}

static const struct udevice_id cvitek_pwm_ids[] = {
	{ .compatible = "cvitek,cvi-pwm" },
	{ }
};

U_BOOT_DRIVER(pwm_cvitek) = {
	.name		= "pwm_cvitek",
	.id		= UCLASS_PWM,
	.of_match	= cvitek_pwm_ids,
	.ops		= &cvitek_pwm_ops,
	.probe		= cvitek_pwm_probe,
	.priv_auto	= sizeof(struct pwm_chip_priv),
};
