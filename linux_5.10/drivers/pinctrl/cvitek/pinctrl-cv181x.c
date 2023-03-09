#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include "pinctrl-cv181x.h"

struct cvitek_pinctrl {
	struct device *dev;
	void __iomem *regs;
	size_t regs_size;

	uint32_t *saved_regs;
};

static int cvi_pinctrl_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct cvitek_pinctrl *pinctrl;

	pinctrl = devm_kzalloc(&pdev->dev, sizeof(*pinctrl), GFP_KERNEL);
	if (!pinctrl)
		return -ENOMEM;

	pinctrl->saved_regs =
		devm_kzalloc(&pdev->dev, CVITEK_PINMUX_REG_LAST, GFP_KERNEL);
	if (!pinctrl->saved_regs)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "Failed to get pinctrl io resource.\n");
		return -EINVAL;
	}

	pinctrl->regs_size = resource_size(res);
	pinctrl->regs = devm_ioremap_resource(&pdev->dev, res);
	if (!pinctrl->regs)
		return -ENOMEM;

	platform_set_drvdata(pdev, pinctrl);

	dev_info(&pdev->dev, "%s(): reg=%p,%zu CVITEK_PINMUX_REG_LAST=0x%x\n",
		 __func__, pinctrl->regs, pinctrl->regs_size,
		 CVITEK_PINMUX_REG_LAST);

	return 0;
}

static int cvi_pinctrl_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "%s()\n", __func__);
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int cvitek_pinctrl_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct cvitek_pinctrl *pinctrl = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s()\n", __func__);

	memcpy_fromio(pinctrl->saved_regs, pinctrl->regs,
		      CVITEK_PINMUX_REG_LAST);

	return 0;
}

static int cvitek_pinctrl_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct cvitek_pinctrl *pinctrl = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "%s()\n", __func__);

	memcpy_toio(pinctrl->regs, pinctrl->saved_regs, CVITEK_PINMUX_REG_LAST);

	return 0;
}
#endif /* CONFIG_PM_SLEEP */

static const struct of_device_id cvi_pinctrl_of_match[] = {
	{ .compatible = "cvitek,pinctrl-cv181x" },
	{},
};

static const struct dev_pm_ops cvitek_pinctrl_pm_ops = {
	SET_LATE_SYSTEM_SLEEP_PM_OPS(cvitek_pinctrl_suspend,
				     cvitek_pinctrl_resume)
};

static struct platform_driver cvi_pinctrl_driver = {
	.probe = cvi_pinctrl_probe,
	.remove	= cvi_pinctrl_remove,
	.driver = {
		.name = "cvitek,pinctrl-cv181x",
		.of_match_table = cvi_pinctrl_of_match,
#ifdef CONFIG_PM_SLEEP
		.pm	= &cvitek_pinctrl_pm_ops,
#endif
	},
};

module_platform_driver(cvi_pinctrl_driver);

MODULE_DESCRIPTION("Cvitek pinctrl");
MODULE_AUTHOR("Cvitek");
MODULE_LICENSE("GPL v2");
