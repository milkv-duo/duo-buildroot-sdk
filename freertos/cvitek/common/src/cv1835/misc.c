/*
 * linux/drivers/char/misc.c
 *
 * Generic misc open routine by Johan Myreen
 *
 * Based on code from Linus
 *
 * Teemu Rantanen's Microsoft Busmouse support and Derrick Cole's
 *   changes incorporated into 0.97pl4
 *   by Peter Cervasio (pete%q106fm.uucp@wupost.wustl.edu) (08SEP92)
 *   See busmouse.c for particulars.
 *
 * Made things a lot mode modular - easy to compile in just one or two
 * of the misc drivers, as they are now completely independent. Linus.
 *
 * Support for loadable modules. 8-Sep-95 Philip Blundell <pjb27@cam.ac.uk>
 *
 * Fixed a failing symbol register to free the device registration
 *		Alan Cox <alan@lxorguk.ukuu.org.uk> 21-Jan-96
 *
 * Dynamic minors and /proc/mice by Alessandro Rubini. 26-Mar-96
 *
 * Renamed to misc and miscdevice to be more accurate. Alan Cox 26-Mar-96
 *
 * Handling of mouse minor numbers for kerneld:
 *  Idea by Jacques Gelinas <jack@solucorp.qc.ca>,
 *  adapted by Bjorn Ekwall <bj0rn@blox.se>
 *  corrected by Alan Cox <alan@lxorguk.ukuu.org.uk>
 *
 * Changes for kmod (from kerneld):
 *	Cyrus Durgin <cider@speakeasy.org>
 *
 * Added devfs support. Richard Gooch <rgooch@atnf.csiro.au>  10-Jan-1998
 */

//#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
//#include <linux/major.h>
//#include <linux/mutex.h>
//#include <linux/proc_fs.h>
//#include <linux/seq_file.h>
//#include <linux/stat.h>
//#include <linux/init.h>
#include <linux/device.h>
//#include <linux/tty.h>
//#include <linux/kmod.h>
//#include <linux/gfp.h>
#include "linux/platform_device.h"
#include "FreeRTOS_POSIX.h"
#include "mutex.h"
#include "linux/types.h"
#include "linux/err.h"
#include "linux/kdev_t.h"
#include "linux/bug.h"

#ifndef find_first_zero_bit

/*
 * Find the first cleared bit in a memory region.
 */

static __always_inline unsigned long __ffs(unsigned long word)
{
	return __builtin_ctzl(word);
}

/*
 * ffz - find first zero in word.
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
#define ffz(x)  __ffs(~(x))

unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
	unsigned long idx;

	for (idx = 0; idx * BITS_PER_LONG < size; idx++) {
		if (addr[idx] != ~0UL)
			return min(idx * BITS_PER_LONG + ffz(addr[idx]), size);
	}

	return size;
}
//EXPORT_SYMBOL(find_first_zero_bit);
#endif
/*
 * Head entry for the doubly linked miscdevice list
 */
static LIST_HEAD(misc_list);
//static DEFINE_MUTEX(misc_mtx);
static mutex misc_mtx;
/*
 * Assigned numbers, used for dynamic minors
 */
#define DYNAMIC_MINORS 64 /* like dynamic majors */
static DECLARE_BITMAP(misc_minors, DYNAMIC_MINORS);

#if 0
static int misc_open(struct inode *inode, struct file *file)
{
	int minor = iminor(inode);
	struct miscdevice *c;
	int err = -ENODEV;
	const struct file_operations *new_fops = NULL;

	mutex_lock(&misc_mtx);

	list_for_each_entry(c, &misc_list, list) {
		if (c->minor == minor) {
			new_fops = (c->fops);
			//new_fops = fops_get(c->fops);
			break;
		}
	}
	if (!new_fops)
		goto fail;

#if 0
	if (!new_fops) {
		mutex_unlock(&misc_mtx);
//		request_module("char-major-%d-%d", MISC_MAJOR, minor);
		mutex_lock(&misc_mtx);

		list_for_each_entry(c, &misc_list, list) {
			if (c->minor == minor) {
				new_fops = (c->fops);
				//new_fops = fops_get(c->fops);
				break;
			}
		}
		if (!new_fops)
			goto fail;
	}
#endif
	/*
	 * Place the miscdevice in the file's
	 * private_data so it can be used by the
	 * file operations, including f_op->open below
	 */
	file->private_data = c;

	err = 0;
//	replace_fops(file, new_fops);
	if (file->f_op->open)
		err = file->f_op->open(inode, file);
fail:
	mutex_unlock(&misc_mtx);
	return err;
}

static const struct file_operations misc_fops = {
	.owner		= THIS_MODULE,
	.open		= misc_open,
//	.llseek		= noop_llseek,
};
#endif

static struct class *misc_class;

/**
 *	misc_register	-	register a miscellaneous device
 *	@misc: device structure
 *
 *	Register a miscellaneous device with the kernel. If the minor
 *	number is set to %MISC_DYNAMIC_MINOR a minor number is assigned
 *	and placed in the minor field of the structure. For other cases
 *	the minor number requested is used.
 *
 *	The structure passed is linked into the kernel and may not be
 *	destroyed until it has been unregistered. By default, an open()
 *	syscall to the device sets file->private_data to point to the
 *	structure. Drivers don't need open in fops for this.
 *
 *	A zero is returned on success and a negative errno code for
 *	failure.
 */

int misc_register(struct miscdevice *misc)
{
	dev_t dev;
	int err = 0;
	bool is_dynamic = (misc->minor == MISC_DYNAMIC_MINOR);

	INIT_LIST_HEAD(&misc->list);

	mutex_lock(&misc_mtx);

	if (is_dynamic) {
		int i = find_first_zero_bit(misc_minors, DYNAMIC_MINORS);

		if (i >= DYNAMIC_MINORS) {
			err = -EBUSY;
			goto out;
		}
		misc->minor = DYNAMIC_MINORS - i - 1;
		set_bit(i, misc_minors);
	} else {
		struct miscdevice *c;

		list_for_each_entry(c, &misc_list, list) {
			if (c->minor == misc->minor) {
				err = -EBUSY;
				goto out;
			}
		}
	}

	dev = MKDEV(MISC_MAJOR, misc->minor);
	misc->this_device =
//		device_create_with_groups(misc_class, misc->parent, dev,
//					  misc, misc->groups, "%s", misc->name);
		device_create(misc_class, misc->parent, dev,
					  misc, "%s", misc->name);
	if (IS_ERR(misc->this_device)) {
		if (is_dynamic) {
			int i = DYNAMIC_MINORS - misc->minor - 1;

			if (i < DYNAMIC_MINORS && i >= 0)
				clear_bit(i, misc_minors);
			misc->minor = MISC_DYNAMIC_MINOR;
		}
		err = PTR_ERR(misc->this_device);
		printf("misc->this_device error=%d\n", err);
		goto out;
	}

	/*
	 * Add it to the front, so that later devices can "override"
	 * earlier defaults
	 */
	list_add(&misc->list, &misc_list);
 out:
	mutex_unlock(&misc_mtx);
	return err;
}

struct device *misc_find_device(const char *name)
{
	struct miscdevice *misc;

	list_for_each_entry(misc, &misc_list, list) {
		if (misc->name) {
			if (!strcmp(misc->name, name)) {
				return misc->this_device;
			}
		}
	}
	return -ENODEV;
}
struct miscdevice *misc_find(const char *name)
{
	struct miscdevice *misc;

	list_for_each_entry(misc, &misc_list, list) {
		if (misc->name) {
			if (!strcmp(misc->name, name)) {
				return misc;
			}
		}
	}
	return -ENODEV;
}
/**
 *	misc_deregister - unregister a miscellaneous device
 *	@misc: device to unregister
 *
 *	Unregister a miscellaneous device that was previously
 *	successfully registered with misc_register().
 */
void misc_deregister(struct miscdevice *misc)
{
	int i = DYNAMIC_MINORS - misc->minor - 1;
	struct device *dev;

	if (WARN_ON(list_empty(&misc->list)))
		return;

	mutex_lock(&misc_mtx);
	list_del(&misc->list);
	dev = misc->this_device;
	if (dev) {
		dev->release(dev);
	}
	if (i < DYNAMIC_MINORS && i >= 0)
		clear_bit(i, misc_minors);
	mutex_unlock(&misc_mtx);
}
int __init misc_init(void)
{
	int err;

	misc_class = class_create(THIS_MODULE, "misc");
	err = PTR_ERR(misc_class);
	if (IS_ERR(misc_class))
		goto fail_remove;
	printf("misc_init\n");
	mutex_init(&misc_mtx);
	err = -EIO;
	return 0;

fail_printk:
	printf("unable to get major %d for misc devices\n", MISC_MAJOR);
	class_destroy(misc_class);
fail_remove:
	return err;
}
//subsys_initcall(misc_init);
