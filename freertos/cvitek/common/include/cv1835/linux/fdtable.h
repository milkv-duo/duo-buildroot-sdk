/*
 * descriptor table internals; you almost certainly want file.h instead.
 */

#ifndef __LINUX_FDTABLE_H
#define __LINUX_FDTABLE_H

//#include <linux/posix_types.h>
#include <linux/compiler.h>
#include <linux/spinlock.h>
//#include <linux/rcupdate.h>
#include <linux/types.h>
//#include <linux/init.h>
#include <linux/fs.h>

#include <linux/atomic.h>

/*
 * The default fd array needs to be at least BITS_PER_LONG,
 * as this is the granularity returned by copy_fdset().
 */
#define NR_OPEN_DEFAULT BITS_PER_LONG

struct fdtable {
	unsigned int max_fds;
	struct file __rcu **fd;      /* current fd array */
//	unsigned long *close_on_exec;
	unsigned long *open_fds;
	unsigned long *full_fds_bits;
//	struct rcu_head rcu;
};
#if 0
static inline bool close_on_exec(unsigned int fd, const struct fdtable *fdt)
{
	return test_bit(fd, fdt->close_on_exec);
}

static inline bool fd_is_open(unsigned int fd, const struct fdtable *fdt)
{
	return test_bit(fd, fdt->open_fds);
}
#endif
/*
 * Open file table structure
 */
struct files_struct {
  /*
   * read mostly part
   */
	atomic_t count;
	//bool resize_in_progress;
	//wait_queue_head_t resize_wait;

	struct fdtable __rcu *fdt;
	struct fdtable fdtab;
  /*
   * written part on a separate cache line in SMP
   */
	spinlock_t file_lock;
	//spinlock_t file_lock ____cacheline_aligned_in_smp;
	unsigned int next_fd;
	unsigned long close_on_exec_init[1];
	unsigned long open_fds_init[1];
	unsigned long full_fds_bits_init[1];
	struct file __rcu *fd_array[NR_OPEN_DEFAULT];
};
extern int __alloc_fd(struct files_struct *files,
	unsigned int start, unsigned int end, unsigned int flags);
extern void __fd_install(struct files_struct *files,
	unsigned int fd, struct file *file);
extern int __close_fd(unsigned int fd);

//extern struct kmem_cache *files_cachep;

/*
 * The caller must ensure that fd table isn't shared or hold rcu or file lock
 */
static inline struct file *__fcheck_files(struct files_struct *files, unsigned int fd)
{
	struct fdtable *fdt = files->fdt; //rcu_dereference_raw(files->fdt);

	if (fd < fdt->max_fds)
		return fdt->fd[fd];
	return NULL;
}

static inline struct file *fcheck_files(struct files_struct *files, unsigned int fd)
{
	return __fcheck_files(files, fd);
}


#endif /* __LINUX_FDTABLE_H */
