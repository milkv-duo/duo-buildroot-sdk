#include <linux/types.h>

#ifdef __LINUX__
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/of.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <linux/version.h>
#include "cvi_log_proc.h"
#include "cvi_sys_proc.h"

#else
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#endif

#include "base.h"
#include "mw/cvi_base.h"

#define BASE_CLASS_NAME "cvi-base"
#define BASE_DEV_NAME "cvi-base"

static void __iomem *top_base;
static struct proc_dir_entry *proc_dir;
struct class *pbase_class;

#ifdef CONFIG_ARCH_CV182X_FPGA
#define FPGA_EARLY_PORTING_CHIP_ID E_CHIPID_CV1822
#endif

static int __init base_init(void);
static void __exit base_exit(void);

static ssize_t base_efuse_shadow_show(struct class *class,
				      struct class_attribute *attr, char *buf)
{
	int ret = 0;

	ret = tee_cv_efuse_read(0, PAGE_SIZE, buf);

	return ret;
}

static ssize_t base_efuse_shadow_store(struct class *class,
				       struct class_attribute *attr,
				       const char *buf, size_t count)
{
	unsigned long addr;
	uint32_t value = 0xDEAFBEEF;
	int ret;

	ret = kstrtoul(buf, 0, &addr);
	if (ret < 0) {
		pr_err("efuse_read: ret=%d\n", ret);
		return ret;
	}

	ret = tee_cv_efuse_read(addr, 4, &value);
	pr_info("efuse_read: 0x%04lx=0x%08x ret=%d\n", addr, value, ret);

	return count;
}

static ssize_t base_efuse_prog_show(struct class *class,
				    struct class_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%s\n", "PROG_SHOW");
	return 0;
}

static ssize_t base_efuse_prog_store(struct class *class,
				     struct class_attribute *attr,
				     const char *buf, size_t count)
{
	int err;
	uint32_t addr = 0, value = 0;

	if (sscanf(buf, "0x%x=0x%x", &addr, &value) != 2)
		return -ENOMEM;

	pr_info("addr=%x value=%x\n", addr, value);
	err = tee_cv_efuse_write(addr, value);
	if (err < 0)
		return err;

	return count;
}

CLASS_ATTR_RW(base_efuse_shadow);
CLASS_ATTR_RW(base_efuse_prog);

unsigned int cvi_base_read_chip_id(void)
{
#ifndef FPGA_EARLY_PORTING_CHIP_ID
//	unsigned int chip_id = ioread32(top_base + GP_REG3_OFFSET) & GP_REG_CHIP_ID_MASK;
	unsigned int chip_id = 0x1835;

	pr_debug("chip_id=0x%x\n", chip_id);

	switch (chip_id) {
	case 0x1822:
		return E_CHIPID_CV1822;
	case 0x1826:
		return E_CHIPID_CV1826;
	case 0x1832:
		return E_CHIPID_CV1832;
	case 0x1838:
		return E_CHIPID_CV1838;
	case 0x1829:
		return E_CHIPID_CV1829;
	case 0x1835:
	default:
		return E_CHIPID_CV1835;
	}
#else
	return FPGA_EARLY_PORTING_CHIP_ID;
#endif
}
EXPORT_SYMBOL_GPL(cvi_base_read_chip_id);

unsigned int cvi_base_read_chip_version(void)
{
	unsigned int chip_version = 0x18802001;

	// chip_version = ioread32(top_base);

	pr_debug("chip_version=0x%x\n", chip_version);

	switch (chip_version) {
	case 0x18802000:
		return E_CHIPVERSION_U01;
	case 0x18802001:
		return E_CHIPVERSION_U02;
	default:
		return E_CHIPVERSION_U01;
	}
}
EXPORT_SYMBOL_GPL(cvi_base_read_chip_version);

static int base_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int base_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int base_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct base_device *ndev = container_of(filp->private_data, struct base_device, miscdev);
	unsigned long vm_start = vma->vm_start;
	unsigned int vm_size = vma->vm_end - vma->vm_start;
	unsigned int offset = vma->vm_pgoff << PAGE_SHIFT;
	void *pos = ndev->shared_mem;

	if (offset < 0 || (vm_size + offset) > BASE_SHARE_MEM_SIZE)
		return -EINVAL;

	while (vm_size > 0) {
		if (remap_pfn_range(vma, vm_start, virt_to_pfn(pos), PAGE_SIZE, vma->vm_page_prot))
			return -EAGAIN;
		pr_info("mmap vir(%p) phys(%#llx)\n", pos, virt_to_phys((void *) pos));
		vm_start += PAGE_SIZE;
		pos += PAGE_SIZE;
		vm_size -= PAGE_SIZE;
	}

	return 0;
}

static long base_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long ret = 0;

	switch (cmd) {
	case IOCTL_READ_CHIP_ID: {
		unsigned long chip_id = 0;

		chip_id = cvi_base_read_chip_id();
		if (copy_to_user((uint32_t *) arg, &chip_id, sizeof(unsigned int)))
			return -EFAULT;
		break;
	}
	case IOCTL_READ_CHIP_VERSION: {
		unsigned long chip_version = 0;

		chip_version = cvi_base_read_chip_version();
		if (copy_to_user((uint32_t *) arg, &chip_version, sizeof(unsigned int)))
		return -EFAULT;
		break;
	}
	default:
		pr_err("Not support functions");
		return -ENOTTY;
	}
	return ret;
}

static long compat_ptr_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	if (!file->f_op->unlocked_ioctl)
		return -ENOIOCTLCMD;

	return file->f_op->unlocked_ioctl(file, cmd, (unsigned long)compat_ptr(arg));
	return 0;
}

static const struct file_operations base_fops = {
	.owner = THIS_MODULE,
	.open = base_open,
	.release = base_release,
	.unlocked_ioctl = base_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = compat_ptr_ioctl,
#endif
};

static int _register_dev(struct base_device *ndev)
{
	int rc;

	ndev->miscdev.minor = MISC_DYNAMIC_MINOR;
	ndev->miscdev.name = BASE_DEV_NAME;
	ndev->miscdev.fops = &base_fops;

	rc = misc_register(&ndev->miscdev);
	if (rc) {
		dev_err(ndev->dev, "cvi_base: failed to register misc device.\n");
		return rc;
	}

	return 0;
}

static int base_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct base_device *ndev;
	int ret;

	pr_debug("base_probe start\n");

	ndev = devm_kzalloc(&pdev->dev, sizeof(*ndev), GFP_KERNEL);
	if (!ndev)
		return -ENOMEM;

	ndev->shared_mem = kzalloc(BASE_SHARE_MEM_SIZE, GFP_KERNEL);
	if (!ndev->shared_mem)
		return -ENOMEM;

	proc_dir = proc_mkdir("cvitek", NULL);
	if (vb_proc_init(proc_dir, ndev->shared_mem) < 0)
		pr_err("vb proc init failed\n");

	if (log_proc_init(proc_dir, ndev->shared_mem) < 0)
		pr_err("log proc init failed\n");

	if (sys_proc_init(proc_dir, ndev->shared_mem) < 0)
		pr_err("sys proc init failed\n");

	ndev->dev = dev;

	ret = _register_dev(ndev);
	if (ret < 0) {
		pr_err("regsiter base chrdev error\n");
		return ret;
	}

	platform_set_drvdata(pdev, ndev);
	pr_debug("%s DONE\n", __func__);

	return 0;
}

static int base_remove(struct platform_device *pdev)
{

	return 0;
}

static const struct of_device_id cvi_base_dt_match[] = { { .compatible = "cvitek,base" }, {} };

static struct platform_driver base_driver = {
	.probe = base_probe,
	.remove = base_remove,
	.driver = {
		.name = BASE_DEV_NAME,
		.owner = THIS_MODULE,
		.of_match_table = cvi_base_dt_match,
	},
};

static void base_cleanup(void)
{
	// class_remove_file(pbase_class, &class_attr_base_efuse_shadow);
	// class_remove_file(pbase_class, &class_attr_base_efuse_prog);
	// class_destroy(pbase_class);
}

static int __init base_init(void)
{
	int rc = 0;

	// top_base = ioremap(TOP_BASE, TOP_REG_BANK_SIZE);

	// pbase_class = class_create(THIS_MODULE, BASE_CLASS_NAME);
	// if (IS_ERR(pbase_class)) {
	//	pr_err("create class failed\n");
	//	rc = PTR_ERR(pbase_class);
	//	goto cleanup;
	// }

	// rc = class_create_file(pbase_class, &class_attr_base_efuse_shadow);
	// if (rc) {
	//	pr_err("base: can't create sysfs base_efuse_shadow file\n");
	//	goto cleanup;
	// }

	// rc = class_create_file(pbase_class, &class_attr_base_efuse_prog);
	// if (rc) {
	//	pr_err("base: can't create sysfs base_efuse_prog) file\n");
	//	goto cleanup;
	// }

	// rc = platform_driver_register(&base_driver);
	// pr_notice("CVITEK CHIP ID = %d\n", cvi_base_read_chip_id());

	return 0;

cleanup:
	base_cleanup();

	return rc;
}

static void __exit base_exit(void)
{
	platform_driver_unregister(&base_driver);
	base_cleanup();

	iounmap(top_base);
}

MODULE_DESCRIPTION("Cvitek base driver");
MODULE_LICENSE("GPL");

module_init(base_init);
module_exit(base_exit);
