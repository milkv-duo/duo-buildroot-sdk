#ifndef _DEVICE_H_
#define _DEVICE_H_
#include <linux/types.h>
#include <linux/spinlock.h>
#include <stdarg.h>
#include <mutex.h>

struct device_driver;
struct device;
#undef dev_t
#define dev_t  u32
/*
 * Struct used for matching a device
 */
struct of_device_id {
    char    name[32];
    char    type[32];
    char    compatible[128];
    const void *data;
};

struct device_driver {
    const char      *name;
    void       *owner;
    const struct of_device_id   *of_match_table;
    int (*probe) (struct device *dev);
//    int (*remove) (struct device *dev);
//    void (*shutdown) (struct device *dev);
//    int (*suspend) (struct device *dev, pm_message_t state);
//    int (*resume) (struct device *dev);
};


struct device {
	struct device	*parent;
	const char	*init_name; /* initial name of the device */
	mutex	mutex;  /* mutex to synchronize calls to its driver. */
	struct	device_driver *driver;   /* which driver has allocated this device */
	void	*platform_data; /* Platform specific data, device core doesn't touch it */
	void	*driver_data;   /* Driver data, set and get with dev_set/get_drvdata */
	struct	device_node  *of_node; /* associated device tree node */
	dev_t	devt;   /* dev_t, creates the sysfs "dev" */
	u32	id; /* device instance */
	spinlock_t	devres_lock;
	struct class	*class;
	void	(*release)(struct device *dev);
};

struct device *get_device(struct device *dev);
void put_device(struct device *dev);

static inline void *dev_get_drvdata(const struct device *dev)
{
    return dev->driver_data;
}

static inline void dev_set_drvdata(struct device *dev, void *data)
{
	printf("dev->driver_data=%lx\n", dev->driver_data);
	printf("data=%lx\n", data);
    dev->driver_data = data;
}

struct of_device_id *of_match_device(const struct of_device_id *matches,
                       const struct device *dev);

struct class {
	const char      *name;
	struct module       *owner;

	void (*class_release)(struct class *class);
	//void (*dev_release)(struct device *dev);
	//int (*suspend)(struct device *dev, pm_message_t state);
	//int (*resume)(struct device *dev);
	//int (*shutdown)(struct device *dev);
};

extern void class_destroy(struct class *cls);
extern struct class * __must_check __class_create(struct module *owner,
	const char *name);

/* This is a #define to keep the compiler from merging different
 * instances of the __key variable
 */
#define class_create(owner, name)       \
({                      \
	__class_create(owner, name);    \
})

static inline const char *dev_name(const struct device *dev)
{
    /* Use the init name until the kobject becomes available */
    if (dev->init_name)
        return dev->init_name;

    return "NULL";
}



/*
 * Easy functions for dynamically creating devices on the fly
 */
struct device *device_create_vargs(struct class *cls, struct device *parent,
	dev_t devt, void *drvdata, const char *fmt, va_list vargs);
struct device *device_create(struct class *cls, struct device *parent,
	dev_t devt, void *drvdata, const char *fmt, ...);
#endif

