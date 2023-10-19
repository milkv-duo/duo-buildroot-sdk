#include <linux/device.h>
#include <linux/errno.h>
//#include <linux/fwnode.h>
//#include <linux/init.h>
//#include <linux/module.h>
//#include <linux/slab.h>
//#include <linux/string.h>
//#include <linux/kdev_t.h>
//#include <linux/notifier.h>
#include <linux/of.h>
//#include <linux/of_device.h>
//#include <linux/genhd.h>
//#include <linux/kallsyms.h>
//#include <linux/mutex.h>
//#include <linux/pm_runtime.h>
//#include <linux/netdevice.h>
#include <linux/err.h>

//#include "base.h"
//#include "power/power.h"
#include "mutex.h"
#include "malloc.h"

void device_initialize(struct device *dev)
{
//    dev->kobj.kset = devices_kset;
//    kobject_init(&dev->kobj, &device_ktype);
//    INIT_LIST_HEAD(&dev->dma_pools);
	mutex_init(&dev->mutex);
//    lockdep_set_novalidate_class(&dev->mutex);
	spin_lock_init(&dev->devres_lock);
//    INIT_LIST_HEAD(&dev->devres_head);
//    device_pm_init(dev);
//    set_dev_node(dev, -1);
#ifdef CONFIG_GENERIC_MSI_IRQ
	INIT_LIST_HEAD(&dev->msi_list);
#endif
}

int device_register(struct device *dev)
{
	device_initialize(dev);
	//return device_add(dev);
	return 0;
}


static void device_create_release(struct device *dev)
{
	//pr_debug("device: '%s': %s\n", dev_name(dev), __func__);

	if (dev->init_name)
		free(dev->init_name);
	kfree(dev);
}


static struct device *
device_create_groups_vargs(struct class *class, struct device *parent,
	dev_t devt, void *drvdata,
	//        const struct attribute_group **groups,
	const char *fmt, va_list args)
{
	struct device *dev = NULL;
	int retval = -ENODEV;
	char *name;

	if (class == NULL || IS_ERR(class))
		goto error;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		retval = -ENOMEM;
		goto error;
	}

	device_initialize(dev);
	dev->devt = devt;
	dev->class = class;
	dev->parent = parent;
	dev->release = device_create_release;
	dev_set_drvdata(dev, drvdata);
	/* it might be memory leak*/
	name = malloc(0x20);
	vsnprintf(name, 0x20-1, fmt, args);
	dev->init_name = name;
//    retval = device_add(dev);
//    if (retval)
//        goto error;

	return dev;

error:
//    put_device(dev);
	return ERR_PTR(retval);
}


struct device *device_create_vargs(struct class *class, struct device *parent,
	dev_t devt, void *drvdata, const char *fmt, va_list args)
{
	return device_create_groups_vargs(class, parent, devt, drvdata,
		fmt, args);
}

struct device *device_create(struct class *class, struct device *parent,
	dev_t devt, void *drvdata, const char *fmt, ...)
{
	va_list vargs;
	struct device *dev;

	va_start(vargs, fmt);
	dev = device_create_vargs(class, parent, devt, drvdata, fmt, vargs);
	va_end(vargs);
	return dev;
}

struct device *device_find(struct class *class, const char *name)
{
	if (!name)
		return 0;
}
