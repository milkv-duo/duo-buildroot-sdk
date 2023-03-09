#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/of.h>
#include "../pinctrl-utils.h"
#include "pinctrl-cv1835.h"


MODULE_DESCRIPTION("Cvitek pinctrl");
MODULE_AUTHOR("Cvitek");
MODULE_LICENSE("GPL v2");
