/*
 *  linux/fs/file.c
 *
 *  Copyright (C) 1998-1999, Stephen Tweedie and Bill Hawes
 *
 *  Manage the dynamic fd arrays in the process files_struct.
 */

//#include <linux/syscalls.h>
//#include <linux/export.h>
#include <linux/fs.h>
//#include <linux/mm.h>
//#include <linux/mmzone.h>
//#include <linux/time.h>
//#include <linux/sched.h>
//#include <linux/slab.h>
//#include <linux/vmalloc.h>
#include <linux/file.h>
#include <linux/fdtable.h>
//#include <linux/bitops.h>
//#include <linux/interrupt.h>
#include <linux/spinlock.h>
//#include <linux/rcupdate.h>
//#include <linux/workqueue.h>
//#include <asm/types.h>
#include <linux/fcntl.h>
#include <linux/errno.h>
#include <linux/__ffs.h>
#include <linux/types.h>
#include "malloc.h"

static inline void __set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p |= mask;
}

static inline void __clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p &= ~mask;
}

static unsigned long _find_next_bit(const unsigned long *addr,
	unsigned long nbits, unsigned long start, unsigned long invert)
{
	unsigned long tmp;

	if (!nbits || start >= nbits)
		return nbits;

	tmp = addr[start / BITS_PER_LONG] ^ invert;

	/* Handle 1st word. */
	tmp &= BITMAP_FIRST_WORD_MASK(start);
	start = round_down(start, BITS_PER_LONG);

	while (!tmp) {
		start += BITS_PER_LONG;
		if (start >= nbits)
			return nbits;

		tmp = addr[start / BITS_PER_LONG] ^ invert;
	}

	return min(start + __ffs(tmp), nbits);
}


unsigned long find_next_zero_bit(const unsigned long *addr, unsigned long size,
	unsigned long offset)
{
	return _find_next_bit(addr, size, offset, ~0UL);
}

static inline void __clear_open_fd(unsigned int fd, struct fdtable *fdt)
{
	__clear_bit(fd, fdt->open_fds);
	__clear_bit(fd / BITS_PER_LONG, fdt->full_fds_bits);
}

struct files_struct init_files = {
	.count      = ATOMIC_INIT(1),
	.fdt        = &init_files.fdtab,
	.fdtab      = {
		.max_fds    = NR_OPEN_DEFAULT,
		.fd     = &init_files.fd_array[0],
		//.close_on_exec  = init_files.close_on_exec_init,
		.open_fds   = init_files.open_fds_init,
		.full_fds_bits  = init_files.full_fds_bits_init,
	},
	.file_lock  = __SPIN_LOCK_UNLOCKED(init_files.file_lock),
};

static unsigned int find_next_fd(struct fdtable *fdt, unsigned int start)
{
	unsigned int maxfd = fdt->max_fds;
	unsigned int maxbit = maxfd / BITS_PER_LONG;
	unsigned int bitbit = start / BITS_PER_LONG;

	bitbit = find_next_zero_bit(fdt->full_fds_bits, maxbit, bitbit) * BITS_PER_LONG;
	if (bitbit > maxfd)
		return maxfd;
	if (bitbit > start)
		start = bitbit;
	return find_next_zero_bit(fdt->open_fds, maxfd, start);
}
static inline void __set_open_fd(unsigned int fd, struct fdtable *fdt)
{
	__set_bit(fd, fdt->open_fds);
	fd /= BITS_PER_LONG;
	if (!~fdt->open_fds[fd])
		__set_bit(fd, fdt->full_fds_bits);
}

/*
 * allocate a file descriptor, mark it busy.
 */
int __alloc_fd(struct files_struct *files,
	unsigned int start, unsigned int end, unsigned int flags)
{
	unsigned int fd;
	int error = -ENMFILE;
	struct fdtable *fdt;

	if (flags != O_RDWR && flags != O_RDONLY)
		return -EINVAL;
	spin_lock(&files->file_lock);
repeat:
	fdt = files->fdt;//files_fdtable(files);
	fd = start;
	if (fd < files->next_fd)
		fd = files->next_fd;
	if (fd < fdt->max_fds)
		fd = find_next_fd(fdt, fd);
	if (fd >= end)
		goto out;

	if (start <= files->next_fd)
		files->next_fd = fd + 1;

	__set_open_fd(fd, fdt);
	error = fd;
out:
	spin_unlock(&files->file_lock);
	return error;
}

static int alloc_fd(unsigned int start, unsigned int flags)
{
	return __alloc_fd(&init_files, start, 4096, flags);
	//return __alloc_fd(&init_files, start, rlimit(RLIMIT_NOFILE), flags);
}

int get_unused_fd_flags(unsigned int flags)
{
	return __alloc_fd(&init_files, 0, 4096, flags);
	//return __alloc_fd(&init_files, 0, rlimit(RLIMIT_NOFILE), flags);
}

static void __put_unused_fd(struct files_struct *files, unsigned int fd)
{
	struct fdtable *fdt = files->fdt;//files_fdtable(files);

	__clear_open_fd(fd, fdt);
	if (fd < files->next_fd)
		files->next_fd = fd;
}

void put_unused_fd(unsigned int fd)
{
	struct files_struct *files = &init_files;

	spin_lock(&files->file_lock);
	__put_unused_fd(files, fd);
	spin_unlock(&files->file_lock);
}

int __close_fd(unsigned int fd)
{
	struct file *file;
	struct fdtable *fdt;
	struct files_struct *files = &init_files;

	spin_lock(&files->file_lock);
	fdt = files->fdt; //files_fdtable(files);

	if (fd >= fdt->max_fds)
		goto out_unlock;

	file = fdt->fd[fd];
	if (!file)
		goto out_unlock;

	__put_unused_fd(files, fd);
	/* clean fops*/
	free(fdt->fd[fd]);
	fdt->fd[fd] = NULL;
	spin_unlock(&files->file_lock);
	return 0;
out_unlock:
	spin_unlock(&files->file_lock);

	return -EBADF;
}

void __fd_install(struct files_struct *files, unsigned int fd,
	struct file *file)
{
	struct fdtable *fdt;

	fdt = files->fdt;
	//ASSERT(fdt->fd[fd] != NULL);
	fdt->fd[fd] = file;
}

void fd_install(unsigned int fd, struct file *file)
{
	__fd_install(&init_files, fd, file);
}

static struct file *__fget(unsigned int fd, fmode_t mask)
{
	struct files_struct *files = &init_files;//current->files;
	struct file *file;

	file = fcheck_files(files, fd);
	if (file) {
		if (file->f_mode & mask)
			file = NULL;
	}

	return file;
}

static unsigned long __fget_light(unsigned int fd, fmode_t mask)
{
	struct files_struct *files = &init_files;
	struct file *file;

	if (atomic_read(&files->count) == 1) {
		file = __fcheck_files(files, fd);

		//if (!file || unlikely(file->f_mode & mask)) {
		if (!file) {
			return 0;
		} else {
			return (unsigned long)file;
		}
	} else {
		file = __fget(fd, mask);
		if (!file)
			return 0;
		return FDPUT_FPUT | (unsigned long)file;
	}
}

unsigned long __fdget(unsigned int fd)
{
	return __fget_light(fd, FMODE_PATH);
}

