/*
 * Cvitek SoCs Gpio Wiegand Driver
 *
 * Copyright CviTek Technologies. All Rights Reserved.
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/gpio.h>
#include <linux/version.h>

#include "cvi_wiegand_gpio.h"

#define CVI_WIEGAND_CDEV_NAME "cvi-wiegand"
#define CVI_WIEGAND_CLASS_NAME "cvi-wiegand"

static struct wgn_tx_cfg tx_cfg;
static struct wgn_rx_cfg rx_cfg;

static struct class *wiegand_class;
static dev_t wiegand_cdev_id;
static struct cvi_wiegand_device *ndev;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0))
typedef struct legacy_timer_emu {
	struct timer_list t;
	void (*function)(unsigned long);
	unsigned long data;
} _timer;
#else
typedef struct timer_list _timer;
#endif //(LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0))

static _timer timer;

static char flag = 'n';
static DECLARE_WAIT_QUEUE_HEAD(wq);

static int gpio_num[4] = {-1, -1, -1, -1};
module_param_array(gpio_num, int, NULL, 0664);

static bool support_tx;
static bool support_rx;

struct gpio_resource {
	int gpio;
	char *name;
	int irq;
};

struct wng_receive_data {
	uint64_t rx_data;
	unsigned int FacilityCode;
	unsigned int UserCode;
	int startParity;
	int endParity;
	char start_parity[5];
	char end_parity[5];
};

static struct gpio_resource wiegand_gpio[] = {
	[0] = {
		.name = "WIEGAND_IN0"
	},
	[1] = {
		.name = "WIEGAND_IN1"
	},
	[2] = {
		.name = "WIEGAND_OUT0"
	},
	[3] = {
		.name = "WIEGAND_OUT1"
	}
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0))
static void legacy_timer_emu_func(struct timer_list *t)
{
	struct legacy_timer_emu *lt = from_timer(lt, t, t);

	lt->function(lt->data);
}
#endif //(LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0))

static void cvi_wiegand_init(void)
{
	ndev->FacilityCode = 0;
	ndev->UserCode = 0;
	ndev->currentBit = 0;
	ndev->startParity = 0;
	ndev->endParity = 0;
	memset(ndev->buffer, 0, MAX_WIEGAND_BYTES);

	tx_cfg.tx_lowtime = 200; //us
	tx_cfg.tx_hightime = 3;  //ms
	tx_cfg.tx_bitcount = 26;
	tx_cfg.tx_msb1st = 1;

	rx_cfg.rx_idle_timeout = 100; //ms
	rx_cfg.rx_bitcount = 26;
	rx_cfg.rx_msb1st = 1;
}

static bool cvi_check_parity(int parityCheck, uint32_t bitcount, bool flag)
{
	int i;
	int bit = 0;
	int mask;
	int parity = parityCheck;

	switch (bitcount) {
	case BITCOUNT_26:
		if (flag) {
			mask = 0x80;
			for (bit = 0; bit < 8; bit++) {
				if (mask & ndev->buffer[0])
					parity++;
				if ((bit <= 3) && (mask & ndev->buffer[1]))
					parity++;
				mask >>= 1;
			}
		} else {
			mask = 0x80;
			for (bit = 0; bit < 8; bit++) {
				if ((bit >= 4) && (mask & ndev->buffer[1]))
					parity++;
				if (mask & ndev->buffer[2])
					parity++;
				mask >>= 1;
			}
		}
		break;

	case BITCOUNT_34:
		if (flag) {
			for (i = 0; i < 2; i++) {
				mask = 0x80;
				for (bit = 0; bit < 8; bit++) {
					if (mask & ndev->buffer[i])
						parity++;
					mask >>= 1;
				}
			}
		} else {
			for (i = 2; i < 4; i++) {
				mask = 0x80;
				for (bit = 0; bit < 8; bit++) {
					if (mask & ndev->buffer[i])
						parity++;
					mask >>= 1;
				}
			}
		}
		break;

	default:
		pr_debug("bitcount not support yet\n");
		break;
	}

	return (parity % 2) == 1;
}

static void cvi_wiegand_gpio_timer(unsigned long data)
{
	char *lcn;

	ndev->endParity = (ndev->buffer[(ndev->currentBit - 2) / 8] & (0x80 >> ((ndev->currentBit - 2) % 8)))
						 >> (7 - (ndev->currentBit - 2) % 8);
	ndev->currentBit = 0;

	pr_debug("wiegand gpio read complete.\n");

	ndev->FacilityCode = (unsigned int)ndev->buffer[0];

	ndev->UserCode = 0;
	lcn = (char *)&ndev->UserCode;
	if (rx_cfg.rx_bitcount == 26) {
		lcn[0] = ndev->buffer[2];
		lcn[1] = ndev->buffer[1];
	}
	if (rx_cfg.rx_bitcount == 34) {
		lcn[0] = ndev->buffer[3];
		lcn[1] = ndev->buffer[2];
		lcn[2] = ndev->buffer[1];
	}

	flag = 'y';
	wake_up_interruptible(&wq);

	pr_debug("new read available: %d:%d\n", ndev->FacilityCode, ndev->UserCode);
}

static irqreturn_t cvi_wiegand_gpio_irq(int irq, void *dev_id)
{
	int data0, data1;
	int value;

	data0 = gpio_get_value(wiegand_gpio[WDIN0].gpio);
	data1 = gpio_get_value(wiegand_gpio[WDIN1].gpio);

	if ((data0 == 1) && (data1 == 1)) //rising edge, ignore
		return IRQ_HANDLED;

	value = ((data0 == 1) && (data1 == 0)) ? 0x80 : 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0))
	del_timer(&timer.t);
#else
	del_timer(&timer);
#endif

	if (ndev->currentBit == 0)
		ndev->startParity = value >> 7;
	else
		ndev->buffer[(ndev->currentBit - 1) / 8] |= (value >> ((ndev->currentBit - 1) % 8));

	ndev->currentBit++;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0))
	timer.t.expires = jiffies + msecs_to_jiffies(rx_cfg.rx_idle_timeout);
	add_timer(&timer.t);
#else
	timer.expires = jiffies + msecs_to_jiffies(rx_cfg.rx_idle_timeout);
	add_timer(&timer);
#endif

	return IRQ_HANDLED;
}

static int cvi_wiegand_set_tx_cfg(struct wgn_tx_cfg *tx_cfg_ptr)
{
	pr_debug("cvi_wiegand_set_tx_cfg\n");

	if (tx_cfg_ptr->tx_lowtime > 1000)
		tx_cfg_ptr->tx_lowtime = 1000; //us

	if (tx_cfg_ptr->tx_hightime > 25)
		tx_cfg_ptr->tx_hightime = 25; //ms

	if (tx_cfg_ptr->tx_bitcount > 34)
		tx_cfg_ptr->tx_bitcount = 34;

	if (tx_cfg_ptr->tx_msb1st > 1)
		tx_cfg_ptr->tx_msb1st = 1;

	memset(&tx_cfg, 0, sizeof(tx_cfg));
	memcpy(&tx_cfg, tx_cfg_ptr, sizeof(tx_cfg));

	return 0;
}

static int cvi_wiegand_set_rx_cfg(struct wgn_rx_cfg *rx_cfg_ptr)
{
	pr_debug("cvi_wiegand_set_rx_cfg\n");

	if (rx_cfg_ptr->rx_idle_timeout > 250)
		rx_cfg_ptr->rx_idle_timeout = 250; //ms

	if (rx_cfg_ptr->rx_bitcount > 34)
		rx_cfg_ptr->rx_bitcount = 34;

	if (rx_cfg_ptr->rx_msb1st > 1)
		rx_cfg_ptr->rx_msb1st = 1;

	memset(&rx_cfg, 0, sizeof(rx_cfg));
	memcpy(&rx_cfg, rx_cfg_ptr, sizeof(rx_cfg));

	return 0;
}

static int cvi_wiegand_gpio_tx(struct cvi_wiegand_device *ndev)
{
	int i;

	pr_debug("cvi_wiegand_gpio_tx\n");
	pr_debug("low tx_data: %#X\n", (uint32_t)(ndev->tx_data));
	pr_debug("high tx_data: %#X\n", (uint32_t)(ndev->tx_data >> 32));

	gpio_set_value(wiegand_gpio[WDOUT0].gpio, 1);
	gpio_set_value(wiegand_gpio[WDOUT1].gpio, 1);

	msleep(tx_cfg.tx_hightime);

	for (i = 0; i < tx_cfg.tx_bitcount; i++) {
		if (ndev->tx_data & (((uint64_t)0x1 << (tx_cfg.tx_bitcount - 1)) >> i)) {
			gpio_set_value(wiegand_gpio[WDOUT1].gpio, 0);
			usleep_range(tx_cfg.tx_lowtime, tx_cfg.tx_lowtime + 10);
			gpio_set_value(wiegand_gpio[WDOUT1].gpio, 1);
		} else {
			gpio_set_value(wiegand_gpio[WDOUT0].gpio, 0);
			usleep_range(tx_cfg.tx_lowtime, tx_cfg.tx_lowtime + 10);
			gpio_set_value(wiegand_gpio[WDOUT0].gpio, 1);
		}

		msleep(tx_cfg.tx_hightime);
	}

	return 0;
}

static int cvi_wiegand_gpio_rx(struct cvi_wiegand_device *ndev)
{
	pr_debug("cvi_wiegand_gpio_rx\n");
	pr_debug("Scheduling Out\n");
	wait_event_interruptible(wq, flag == 'y');
	flag = 'n';
	pr_debug("Woken Up\n");

	return 0;
}

static int cvi_wiegand_gpio_get_result(struct wng_receive_data *rx_result)
{
	int ret = 0;

	ret = cvi_wiegand_gpio_rx(ndev);

	if (rx_cfg.rx_bitcount == 26)
		rx_result->rx_data = ((((ndev->FacilityCode << 16) | ndev->UserCode) << 1) | ndev->endParity)
								| (ndev->startParity << 25);
	if (rx_cfg.rx_bitcount == 34)
		rx_result->rx_data = ((((ndev->FacilityCode << 24) | ndev->UserCode) << 1) | ndev->endParity)
								| ((uint64_t)ndev->startParity << 33);

	rx_result->FacilityCode = ndev->FacilityCode;
	rx_result->UserCode = ndev->UserCode;
	rx_result->startParity = ndev->startParity;
	rx_result->endParity = ndev->endParity;

	if (cvi_check_parity(ndev->startParity, rx_cfg.rx_bitcount, 1))
		strcpy(rx_result->start_parity, "fail");
	else
		strcpy(rx_result->start_parity, "pass");

	if (!cvi_check_parity(ndev->endParity, rx_cfg.rx_bitcount, 0))
		strcpy(rx_result->end_parity, "fail");
	else
		strcpy(rx_result->end_parity, "pass");

	return ret;
}

static ssize_t cvi_wiegand_gpio_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	int ret;
	struct wng_receive_data rx_result;

	ret = cvi_wiegand_gpio_get_result(&rx_result);

	if (copy_to_user((struct wng_receive_data *)buff, &rx_result, count))
		return -EFAULT;

	return ret;
}

static ssize_t cvi_wiegand_gpio_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	uint64_t tx_data;

	if (copy_from_user(&tx_data, buff, count))
		return -EFAULT;

	ndev->tx_data = tx_data;
	cvi_wiegand_gpio_tx(ndev);

	return count;
}

static long cvi_wiegand_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long ret = 0;
	struct wgn_tx_cfg tx_cfg_tmp;
	struct wgn_rx_cfg rx_cfg_tmp;

	uint64_t tx_data;
	struct wng_receive_data rx_result;

	switch (cmd) {
	case IOCTL_WGN_SET_TX_CFG:
		if (copy_from_user(&tx_cfg_tmp, (void *)arg, sizeof(tx_cfg_tmp))) {
			pr_err("copy_from_user failed.\n");
			break;
		}
		ret = cvi_wiegand_set_tx_cfg(&tx_cfg_tmp);
		break;

	case IOCTL_WGN_SET_RX_CFG:
		if (copy_from_user(&rx_cfg_tmp, (void *)arg, sizeof(rx_cfg_tmp))) {
			pr_err("copy_from_user failed.\n");
			break;
		}
		ret = cvi_wiegand_set_rx_cfg(&rx_cfg_tmp);
		break;

	case IOCTL_WGN_TX:
		if (copy_from_user(&tx_data, (void *)arg, sizeof(tx_data))) {
			pr_err("copy_from_user failed.\n");
			break;
		}
		ndev->tx_data = tx_data;
		ret = cvi_wiegand_gpio_tx(ndev);
		break;

	case IOCTL_WGN_RX:
		ret = cvi_wiegand_gpio_get_result(&rx_result);
		if (copy_to_user((struct wng_receive_data *)arg, &rx_result, sizeof(rx_result)))
			return -EFAULT;
		break;

	default:
		return -ENOTTY;
	}

	return ret;
}

static int cvi_wiegand_open(struct inode *inode, struct file *filp)
{
	unsigned long flags = 0;

	pr_debug("cvi_wiegand_open\n");

	spin_lock_irqsave(&ndev->close_lock, flags);

	ndev->use_count++;

	spin_unlock_irqrestore(&ndev->close_lock, flags);

	filp->private_data = ndev;

	return 0;
}

static int cvi_wiegand_close(struct inode *inode, struct file *filp)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&ndev->close_lock, flags);

	ndev->use_count--;

	spin_unlock_irqrestore(&ndev->close_lock, flags);

	filp->private_data = NULL;

	pr_debug("cvi_wiegand_close\n");
	return 0;
}

static const struct file_operations wiegand_fops = {
	.owner = THIS_MODULE,
	.open = cvi_wiegand_open,
	.release = cvi_wiegand_close,
	.read = cvi_wiegand_gpio_read,
	.write = cvi_wiegand_gpio_write,
	.unlocked_ioctl = cvi_wiegand_ioctl,
	.compat_ioctl = cvi_wiegand_ioctl,
};

static int cvi_wiegand_gpio_config(void)
{
	int i;

	if ((gpio_num[WDIN0] >= GPIO_NUM_MIN && gpio_num[WDIN0] <= GPIO_NUM_MAX) &&
		(gpio_num[WDIN1] >= GPIO_NUM_MIN && gpio_num[WDIN1] <= GPIO_NUM_MAX)) {
		support_rx = 1;
		pr_debug("Supported wiegand gpio rx.\n");
	} else {
		if ((gpio_num[WDIN0] == -1) && (gpio_num[WDIN1] == -1)) {
			support_rx = 0;
			pr_debug("Unsupported wiegand gpio rx.\n");
		} else {
			pr_debug("Invalid gpio num.\n");
			return -EPERM;
		}
	}

	if ((gpio_num[WDOUT0] >= GPIO_NUM_MIN && gpio_num[WDOUT0] <= GPIO_NUM_MAX) &&
		(gpio_num[WDOUT1] >= GPIO_NUM_MIN && gpio_num[WDOUT1] <= GPIO_NUM_MAX)) {
		support_tx = 1;
		pr_debug("Supported wiegand gpio tx.\n");
	} else {
		if ((gpio_num[WDOUT0] == -1) && (gpio_num[WDOUT1] == -1)) {
			support_tx = 0;
			pr_debug("Unsupported wiegand gpio rx.\n");
		} else {
			pr_debug("Invalid gpio num.\n");
			return -EPERM;
		}
	}

	for (i = 0; i < ARRAY_SIZE(wiegand_gpio); i++) {
		wiegand_gpio[i].gpio = gpio_num[i];
		if ((i < 2 && support_rx) || (i >= 2 && support_tx))
			gpio_request(wiegand_gpio[i].gpio, wiegand_gpio[i].name);
	}

	if (support_rx) {
		gpio_direction_input(wiegand_gpio[WDIN0].gpio);
		gpio_direction_input(wiegand_gpio[WDIN1].gpio);

		wiegand_gpio[WDIN0].irq = gpio_to_irq(wiegand_gpio[WDIN0].gpio);
		if (request_irq(wiegand_gpio[WDIN0].irq, cvi_wiegand_gpio_irq,
				IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "wiegand_gpio", ndev)) {
			pr_debug("register irq failed %d\n", wiegand_gpio[WDIN0].irq);
			return -EIO;
		}

		wiegand_gpio[WDIN1].irq = gpio_to_irq(wiegand_gpio[WDIN1].gpio);
		if (request_irq(wiegand_gpio[WDIN1].irq, cvi_wiegand_gpio_irq,
				IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "wiegand_gpio", ndev)) {
			pr_debug("register irq failed %d\n", wiegand_gpio[WDIN1].irq);
			return -EIO;
		}
	}

	if (support_tx) {
		gpio_direction_output(wiegand_gpio[WDOUT0].gpio, 1);
		gpio_direction_output(wiegand_gpio[WDOUT1].gpio, 1);
	}

	return 0;
}

static int cvi_wiegand_register_cdev(struct cvi_wiegand_device *ndev)
{
	int ret;

	wiegand_class = class_create(THIS_MODULE, CVI_WIEGAND_CLASS_NAME);
	if (IS_ERR(wiegand_class)) {
		pr_err("create class failed\n");
		return PTR_ERR(wiegand_class);
	}

	ret = alloc_chrdev_region(&wiegand_cdev_id, 0, 1, CVI_WIEGAND_CDEV_NAME);
	if (ret < 0) {
		pr_err("alloc chrdev failed\n");
		return ret;
	}

	cdev_init(&ndev->cdev, &wiegand_fops);
	ndev->cdev.owner = THIS_MODULE;
	cdev_add(&ndev->cdev, wiegand_cdev_id, 1);

	device_create(wiegand_class, ndev->dev, wiegand_cdev_id, NULL, "%s%d",
		      CVI_WIEGAND_CDEV_NAME, 0);

	return 0;
}

static int __init cvi_wiegand_gpio_init(void)
{
	int ret = 0;

	pr_debug("cvi_wiegand_gpio_init start\n");

	ndev = kzalloc(sizeof(*ndev), GFP_KERNEL);
	if (!ndev)
		return -ENOMEM;

	cvi_wiegand_init();

	ret = cvi_wiegand_gpio_config();
	if (ret < 0) {
		pr_debug("wiegand gpio config error\n");
		return ret;
	}

	ret = cvi_wiegand_register_cdev(ndev);
	if (ret < 0) {
		pr_debug("wiegand register cdev error\n");
		return ret;
	}

	if (support_rx) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0))
		timer_setup(&timer.t, legacy_timer_emu_func, 0);
#else
		init_timer(&timer);
#endif
		timer.function = cvi_wiegand_gpio_timer;
		timer.data = (unsigned long) ndev;
	}

	pr_debug("cvi_wiegand_gpio_init end\n");

	return ret;
}

static void __exit cvi_wiegand_gpio_exit(void)
{
	int i;

	pr_debug("cvi_wiegand_gpio_exit start\n");

	if (support_rx) {
		free_irq(wiegand_gpio[WDIN0].irq, ndev);
		free_irq(wiegand_gpio[WDIN1].irq, ndev);
	}

	for (i = 0; i < ARRAY_SIZE(wiegand_gpio); i++) {
		if ((i < 2 && support_rx) || (i >= 2 && support_tx))
			gpio_free(wiegand_gpio[i].gpio);
	}

	device_destroy(wiegand_class, wiegand_cdev_id);

	cdev_del(&ndev->cdev);
	unregister_chrdev_region(wiegand_cdev_id, 1);

	class_destroy(wiegand_class);

	kfree(ndev);
	ndev = NULL;

	pr_debug("cvi_wiegand_gpio_exit end\n");
}

module_init(cvi_wiegand_gpio_init);
module_exit(cvi_wiegand_gpio_exit);

MODULE_DESCRIPTION("Wiegand GPIO Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cvitek");
