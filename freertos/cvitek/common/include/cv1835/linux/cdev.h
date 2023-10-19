#ifndef _LINUX_CDEV_H
#define _LINUX_CDEV_H

//#include <linux/kobject.h>
//#include <linux/kdev_t.h>
#include <linux/list.h>
#include "linux/device.h"

const struct file_operations;
struct module;

struct cdev {
	struct module *owner;
	const struct file_operations *ops;
	struct list_head list;
	dev_t dev;
	unsigned int count;
};

void cdev_init(struct cdev *cdev, const struct file_operations *fops);
int cdev_add(struct cdev *cdev, dev_t dev, unsigned int count);
void cdev_del(struct cdev *cdev);
struct cdev *cdev_find(const char *name);

#endif
