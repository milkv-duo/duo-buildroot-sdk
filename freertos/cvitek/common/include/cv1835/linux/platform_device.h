#ifndef _PLATFORM_DEVICE_H_
#define _PLATFORM_DEVICE_H_

#include <linux/device.h>
#include <linux/types.h>

#define THIS_MODULE 0
#define PLATFORM_DEVID_NONE (-1)

#define platform_driver_register(drv)                                          \
	__platform_driver_register(drv, THIS_MODULE)

#define platform_driver_unregister(drv)                                        \
	{                                                                      \
	}
#define platform_device_unregister(drv)                                        \
	{                                                                      \
	}

struct device_driver;
struct device;

struct platform_device {
	const char *name;
	int id;
	struct device dev;
	u32 num_resources;
	struct resource *resource;
};

struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	//    int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;
};

struct resource *platform_get_resource(struct platform_device *dev,
				       unsigned int type, unsigned int num);

#define to_platform_driver(drv)                                                \
	(container_of((drv), struct platform_driver, driver))

#define to_platform_device(x) container_of((x), struct platform_device, dev)

static inline void *platform_get_drvdata(const struct platform_device *pdev)
{
	return dev_get_drvdata(&pdev->dev);
}

static inline void platform_set_drvdata(struct platform_device *pdev,
					void *data)
{
	dev_set_drvdata(&pdev->dev, data);
}

void rtos_init_platform_device(size_t *pdevices, unsigned int num);
void rtos_init_device_node(size_t *pdevices, unsigned int num, size_t *pnode);

#endif
