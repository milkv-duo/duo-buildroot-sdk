#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
#include "linux/platform_device.h"
#include "linux/ioport.h"
#include "linux/of.h"
#include "linux/kernel.h"

#define MAX_PLAT_DEVICE 16
static struct platform_device *cvitek_pdevs[MAX_PLAT_DEVICE];
static unsigned int platform_device_num;
struct device_node;

/**
 * __platform_driver_register - register a driver for platform-level devices
 * @drv: platform driver structure
 * @owner: owning module/driver
 */
int __platform_driver_register(struct platform_driver *drv,
			       struct module *owner)
{
	int ret = -1;
	drv->driver.owner = owner;

	// find out platform_device
	for (int i = 0; i < platform_device_num; i++) {
		struct platform_device *pdev;

		pdev = cvitek_pdevs[i];
		if (!strcmp(pdev->name, drv->driver.name))
			if (drv->probe) {
				pdev->dev.driver = drv;
				ret = drv->probe(pdev);
			}
	}
	return ret;
}

int platform_device_add_resources(struct platform_device *pdev,
				  const struct resource *res, unsigned int num)
{
	pdev->resource = res;
	pdev->num_resources = num;
	return 0;
}

struct resource *platform_get_resource(struct platform_device *dev,
				       unsigned int type, unsigned int num)
{
	int i;

	for (i = 0; i < dev->num_resources; i++) {
		struct resource *r = &dev->resource[i];

		if (type == resource_type(r) && num-- == 0)
			return r;
	}
	return NULL;
}

void rtos_init_platform_device(size_t *devices, unsigned int num)
{
	int i;
	if (!devices || num <= 0 || num > MAX_PLAT_DEVICE)
		return 0;
	platform_device_num = num;
	for (i = 0; i < num; i++) {
		struct platform_device *pdev;

		pdev = (struct platform_device *)devices[i];
		pdev->dev.init_name = pdev->name;
		pdev->dev.platform_data = pdev;
		cvitek_pdevs[i] = pdev;
	}
}

void rtos_init_device_node(size_t *pdevices, unsigned int num, size_t *pnode)
{
	int i;
	for (i = 0; i < num; i++) {
		struct platform_device *pdev;

		pdev = (struct platform_device *)pdevices[i];
		pdev->dev.of_node = (struct device_node *)pnode[i];
	}
}
