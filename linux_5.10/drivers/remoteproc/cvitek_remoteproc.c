// SPDX-License-Identifier: GPL-2.0
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/mailbox_client.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/remoteproc.h>
#include <linux/reset.h>

#include "remoteproc_internal.h"

#define SEC_SUBSYS_BASE 0x02000000
#define SEC_SYS_BASE (SEC_SUBSYS_BASE + 0x000B0000)
#define SEC_CLK_ADDR ((__u32)0x3003024)

struct cvitek_rproc_mem {
	void __iomem *cpu_addr;
	phys_addr_t bus_addr;
	u32 dev_addr;
	size_t size;
};

struct cvitek_rproc {
	struct rproc *rproc;
	struct cvitek_rproc_mem mem[4];
	struct reset_control *reset;
	struct clk *clk;
	u32 clk_rate;
	struct regmap *boot_base;
	u32 boot_offset;
	int num_mems;
	struct platform_device *pdev;
};

static inline void clrbits_32(void __iomem *reg, u32 clear)
{
	iowrite32((ioread32(reg) & ~clear), reg);
}

static inline void setbits_32(void __iomem *reg, u32 set)
{
	iowrite32(ioread32(reg) | set, reg);
}

static int cvitek_rproc_start(struct rproc *rproc)
{
	struct device *dev = rproc->dev.parent;

	void __iomem *clk_reset = ioremap(SEC_CLK_ADDR, 4);

	clrbits_32(clk_reset, 1 << 6);

	udelay(10);

	setbits_32(clk_reset, 1 << 6);

	iounmap(clk_reset);

	dev_info(dev, "Started from 0x%llx\n", rproc->bootaddr);

	return 0;
}

static int cvitek_rproc_stop(struct rproc *rproc)
{
	void __iomem *clk_reset = ioremap(SEC_CLK_ADDR, 4);

	clrbits_32(clk_reset, 1 << 6);

	iounmap(clk_reset);

	return 0;
}

static int cvitek_rproc_mem_alloc(struct rproc *rproc,
				  struct rproc_mem_entry *mem)
{
	struct device *dev = rproc->dev.parent;
	void *va;

	va = ioremap_wc(mem->dma, mem->len);
	if (!va) {
		dev_err(dev, "Unable to map memory region: %pa+%zx\n",
			&mem->dma, mem->len);
		return -ENOMEM;
	}

	/* Update memory entry va */
	mem->va = va;

	return 0;
}

static int cvitek_rproc_mem_release(struct rproc *rproc,
				    struct rproc_mem_entry *mem)
{
	iounmap(mem->va);

	return 0;
}

static int cvitek_rproc_parse_fw(struct rproc *rproc, const struct firmware *fw)
{
	struct device *dev = rproc->dev.parent;
	struct device_node *np = dev->of_node;
	struct rproc_mem_entry *mem;
	struct reserved_mem *rmem;
	struct of_phandle_iterator it;
	int index = 0;

	of_phandle_iterator_init(&it, np, "memory-region", NULL, 0);
	while (of_phandle_iterator_next(&it) == 0) {
		rmem = of_reserved_mem_lookup(it.node);
		if (!rmem) {
			dev_err(dev, "unable to acquire memory-region\n");
			return -EINVAL;
		}

		/*  No need to map vdev buffer */
		if (strcmp(it.node->name, "vdev0buffer")) {
			/* Register memory region */
			mem = rproc_mem_entry_init(
				dev, NULL, (dma_addr_t)rmem->base, rmem->size,
				rmem->base, cvitek_rproc_mem_alloc,
				cvitek_rproc_mem_release, it.node->name);
		} else {
			/* Register reserved memory for vdev buffer allocation */
			mem = rproc_of_resm_mem_entry_init(dev, index,
							   rmem->size,
							   rmem->base,
							   it.node->name);
		}

		if (!mem)
			return -ENOMEM;

		rproc_add_carveout(rproc, mem);
		index++;
	}

	if (rproc_elf_load_rsc_table(rproc, fw))
		dev_warn(&rproc->dev, "no resource table found for this firmware\n");

	return 0;
}

static const struct rproc_ops cvitek_rproc_ops = {
	.start = cvitek_rproc_start,
	.stop = cvitek_rproc_stop,
	.parse_fw = cvitek_rproc_parse_fw,
	.load = rproc_elf_load_segments,
	.find_loaded_rsc_table = rproc_elf_find_loaded_rsc_table,
	.sanity_check = rproc_elf_sanity_check,
	.get_boot_addr = rproc_elf_get_boot_addr,
};

static const struct of_device_id cvitek_rproc_match[] = {
	{ .compatible = "cvitek,cv18xx-c906l-rproc", },
	{},
};
MODULE_DEVICE_TABLE(of, cvitek_rproc_match);

static const char *cvitek_rproc_get_firmware(struct platform_device *pdev)
{
	const char *fw_name;
	int ret;

	ret = of_property_read_string(pdev->dev.of_node, "firmware-name",
				      &fw_name);
	if (ret)
		return ERR_PTR(ret);

	return fw_name;
}

static int cvitek_rproc_parse_dt(struct platform_device *pdev)
{
	//TODO
	return 0;
}

static int cvitek_rproc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct of_device_id *match;
	struct cvitek_rproc *cproc;
	struct device_node *np = dev->of_node;
	struct rproc *rproc;
	const char *firmware;
	int ret;
	int i;

	match = of_match_device(cvitek_rproc_match, dev);
	if (!match) {
		dev_err(dev, "No device match found\n");
		return -ENODEV;
	}

	firmware = cvitek_rproc_get_firmware(pdev);
	if (IS_ERR(firmware))
		return PTR_ERR(firmware);

	rproc = rproc_alloc(dev, dev_name(dev), &cvitek_rproc_ops, firmware,
			    sizeof(*cproc));
	if (!rproc)
		return -ENOMEM;

	cproc = rproc->priv;

	rproc->has_iommu = false;
	cproc->pdev = pdev;

	platform_set_drvdata(pdev, rproc);

	ret = cvitek_rproc_parse_dt(pdev);
	if (ret)
		goto free_rproc;

	ret = rproc_add(rproc);
	if (ret)
		goto free_clk;

	return 0;

free_clk:
	clk_unprepare(cproc->clk);

free_rproc:
	rproc_free(rproc);
	return ret;
}

static int cvitek_rproc_remove(struct platform_device *pdev)
{
	struct rproc *rproc = platform_get_drvdata(pdev);
	struct cvitek_rproc *cproc = rproc->priv;
	int i;

	rproc_del(rproc);

	clk_disable_unprepare(cproc->clk);

	rproc_free(rproc);

	return 0;
}

static struct platform_driver cvitek_rproc_driver = {
	.probe = cvitek_rproc_probe,
	.remove = cvitek_rproc_remove,
	.driver = {
		.name = "cvitek-rproc",
		.of_match_table = of_match_ptr(cvitek_rproc_match),
	},
};
module_platform_driver(cvitek_rproc_driver);
MODULE_DESCRIPTION("Cvitek Remote Processor Control Driver");
MODULE_LICENSE("GPL v2");