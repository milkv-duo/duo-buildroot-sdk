#include "linux/err.h"
#include "linux/errno.h"
#include "linux/fs.h"
#include "linux/file.h"

long vfs_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int error = -ENOTTY;

	if (!filp->f_op->unlocked_ioctl)
		goto out;

	error = filp->f_op->unlocked_ioctl(filp, cmd, arg);
	if (error == -ENOIOCTLCMD)
		error = -ENOTTY;
out:
	return error;
}

int do_vfs_ioctl(struct file *filp, unsigned int fd, unsigned int cmd,
	unsigned long arg)
{
	int error = 0;

	error = vfs_ioctl(filp, cmd, arg);
	return error;
}


int ioctl(unsigned int fd, unsigned int cmd, unsigned long arg)
{
	int error;
	struct fd f = fdget(fd);

	if (!f.file)
		return -EBADF;
	error = do_vfs_ioctl(f.file, fd, cmd, arg);
	return error;
}

