#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/delay.h>

#define UPDATE_REMAP	(1 << 31)
void __iomem *remap_subsys_base;
u32 ch_remap[8] = {0};

struct cvi_sysdma_remap_dev {
	void __iomem *subsys_base;
	struct device *dev;
};

static int cv1835_sysdma_remap_probe(struct platform_device *pdev)
{
	struct cvi_sysdma_remap_dev *dev;
	struct resource *res;
	u32 val;

	dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	dev->subsys_base = devm_ioremap_resource(&pdev->dev, res);
	remap_subsys_base = dev->subsys_base;

	if (IS_ERR(dev->subsys_base))
		return PTR_ERR(dev->subsys_base);
	dev->dev = &pdev->dev;

	device_property_read_u32_array(&pdev->dev, "ch-remap", ch_remap, 8);

	val = UPDATE_REMAP
		| (ch_remap[0])
		| (ch_remap[1] << 8)
		| (ch_remap[2] << 16)
		| (ch_remap[3] << 24);

	writel(val, dev->subsys_base);

	val = UPDATE_REMAP
		| (ch_remap[4])
		| (ch_remap[5] << 8)
		| (ch_remap[6] << 16)
		| (ch_remap[7] << 24);

	writel(val, dev->subsys_base + 0x4);

	if (device_property_present(&pdev->dev, "int_mux_base") &&
		device_property_present(&pdev->dev, "int_mux")) {
		u32 int_mux_base = 0;
		u32 int_mux = 0;
		void __iomem *int_mux_reg;

		/* Set sysDMA interrupt receiver of IC after CV181X */
		device_property_read_u32(&pdev->dev, "int_mux_base", &int_mux_base);
		if (int_mux_base != 0x0) {
			int_mux_reg = ioremap(int_mux_base, 0x4);
			device_property_read_u32(&pdev->dev, "int_mux", &int_mux);
			writel(int_mux, int_mux_reg);
			iounmap(int_mux_reg);
		}
	}

	return 0;
}

static const struct of_device_id cv1835_sysdma_remap_id_match[] = {
	{
		.compatible = "cvitek,sysdma_remap",
	},
	{},
};

#ifdef CONFIG_PM_SLEEP
static int dma_remap_suspend_late(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);

	return 0;
}

static int dma_remap_resume_early(struct device *dev)
{
	u32 val;

	val = UPDATE_REMAP
		| (ch_remap[0])
		| (ch_remap[1] << 8)
		| (ch_remap[2] << 16)
		| (ch_remap[3] << 24);

	writel(val, remap_subsys_base);

	val = UPDATE_REMAP
		| (ch_remap[4])
		| (ch_remap[5] << 8)
		| (ch_remap[6] << 16)
		| (ch_remap[7] << 24);

	writel(val, remap_subsys_base + 0x4);

	return 0;
}
#else
#define dma_remap_suspend_late	NULL
#define dma_remap_resume_early	NULL
#endif /* CONFIG_PM_SLEEP */

static const struct dev_pm_ops dma_remap_pm_ops = {
	SET_LATE_SYSTEM_SLEEP_PM_OPS(dma_remap_suspend_late, dma_remap_resume_early)
};

static struct platform_driver cv1835_sysdma_remap_driver = {
	.driver = {
		.name = "cv1835-sysdma-remap",
		.owner = THIS_MODULE,
		.pm = &dma_remap_pm_ops,
		.of_match_table = of_match_ptr(cv1835_sysdma_remap_id_match),
	},
	.probe = cv1835_sysdma_remap_probe,
};

static int __init cv1835_sysdma_remap_init(void)
{
	return platform_driver_register(&cv1835_sysdma_remap_driver);
}

arch_initcall(cv1835_sysdma_remap_init);
