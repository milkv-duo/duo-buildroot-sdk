// SPDX-License-Identifier: GPL-2.0-only

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>

#define RTC_CTRL0_UNLOCKKEY		0x4
#define RTC_CTRL0				0x8
#define RTC_CTRL0_STATUS0		0xC
#define RTC_EN_PWR_WAKEUP		0xBC
#define RTC_EN_SHDN_REQ			0xC0
#define RTC_EN_PWR_CYC_REQ		0xC8
#define RTC_EN_WARM_RST_REQ		0xCC
#define RTC_EN_WDT_RST_REQ		0xE0
#define RTC_EN_SUSPEND_REQ		0xE4
#define RSM_STATE				0xD4
#define ST_ON					0x3

static void __iomem *base;

static int cvi_restart_handler(struct notifier_block *this,
				unsigned long mode, void *cmd)
{
	void __iomem *REG_RTC_CTRL_BASE = base;
	void __iomem *REG_RTC_BASE = base + 0x1000;

	/* Enable power suspend wakeup source mask */
	writel(0x1, REG_RTC_BASE + 0x3C); // 1 = select prdata from 32K domain

	writel(0xAB18, REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY);

	writel(0x1, REG_RTC_BASE + RTC_EN_WARM_RST_REQ);

	while (readl(REG_RTC_BASE + RTC_EN_WARM_RST_REQ) != 0x01)
		;

	while (readl(REG_RTC_BASE + RSM_STATE) != ST_ON)
		;

	writel(0xFFFF0800 | (0x1 << 4), REG_RTC_CTRL_BASE + RTC_CTRL0);

	return NOTIFY_DONE;
}

static void cvi_do_pwroff(void)
{
	void __iomem *REG_RTC_CTRL_BASE = base;
	void __iomem *REG_RTC_BASE = base + 0x1000;

	/* Enable power suspend wakeup source mask */
	writel(0x1, REG_RTC_BASE + 0x3C); // 1 = select prdata from 32K domain

	writel(0xAB18, REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY);

	writel(0x1, REG_RTC_BASE + RTC_EN_SHDN_REQ);

	while (readl(REG_RTC_BASE + RTC_EN_SHDN_REQ) != 0x01)
		;

	writel(0xFFFF0800 | (0x1 << 0), REG_RTC_CTRL_BASE + RTC_CTRL0);

	/* Wait some time until system down, otherwise, notice with a warn */
	mdelay(1000);

	WARN_ONCE(1, "Unable to power off system\n");
}

static struct notifier_block cvi_restart_nb = {
	.notifier_call = cvi_restart_handler,
	.priority = 128,
};

static int cvi_reboot_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	int err;

	base = of_iomap(np, 0);
	if (!base) {
		WARN(1, "failed to map base address");
		return -ENODEV;
	}

	err = register_restart_handler(&cvi_restart_nb);
	if (err) {
		dev_err(&pdev->dev, "cannot register restart handler (err=%d)\n",
			err);
		iounmap(base);
	}

	pm_power_off = &cvi_do_pwroff;

	return err;
}

static const struct of_device_id cvi_reboot_of_match[] = {
	{ .compatible = "cvitek,restart" },
	{}
};

static struct platform_driver cvi_reboot_driver = {
	.probe = cvi_reboot_probe,
	.driver = {
		.name = "cvi-reboot",
		.of_match_table = cvi_reboot_of_match,
	},
};
module_platform_driver(cvi_reboot_driver);
