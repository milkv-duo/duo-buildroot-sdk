#include "linux/compiler.h"
#include "linux/fcntl.h"
#include "linux/fs.h"
#include "linux/miscdevice.h"
#include "linux/err.h"
#include "linux/errno.h"
#include "linux/cdev.h"
#include "malloc.h"

struct file *do_filp_open(struct file *filp, const char *pathname)
{
	struct miscdevice *misc;
	struct cdev *cdev;

	misc = misc_find(pathname);
	if (!IS_ERR(misc)) {
		filp->f_op = misc->fops;
		filp->private_data = misc;
		return filp;
	}

	cdev = cdev_find(pathname);
	if (!IS_ERR(cdev)) {
		filp->f_op = cdev->ops;
		filp->private_data = cdev;
		return filp;
	}
	return -ENODEV;
}

//long do_sys_open(int dfd, const char __user *filename, int flags, umode_t mode)
long open(const char __user *filename, int flags)
{
	int fd;

	fd = get_unused_fd_flags(flags);
	if (fd >= 0) {
		// need to connect and set file here. register file & fd with freertos task create
		struct file *f;

		f = malloc(sizeof(*f));
		if (IS_ERR(do_filp_open(f, filename))) {
			put_unused_fd(fd);
			fd = PTR_ERR(f);
		} else {
			fd_install(fd, f);
		}
	}
	return fd;
}

int close(unsigned int fd)
{
	int retval = __close_fd(fd);

	return retval;
}

