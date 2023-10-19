#ifndef _LINUX_FS_H
#define _LINUX_FS_H
#include "linux/types.h"
#include "linux/device.h"
//#include "FreeRTOS_POSIX/sys/types.h"
struct file;
const struct file_operations;
struct vm_area_struct;

struct file {
//    union {
//        struct llist_node   fu_llist;
//        struct rcu_head     fu_rcuhead;
//    } f_u;
//    struct path     f_path;
//    struct inode        *f_inode;   /* cached value */
	const struct file_operations	*f_op;

	/*
	 * Protects f_ep_links, f_flags.
	 * Must not be taken from IRQ context.
	 */
//    spinlock_t      f_lock;
//    atomic_long_t       f_count;
	unsigned int        f_flags;
	fmode_t	f_mode;
//    struct mutex        f_pos_lock;
//    loff_t          f_pos;
//    struct fown_struct  f_owner;
//    const struct cred   *f_cred;
//    struct file_ra_state    f_ra;

//    u64         f_version;
#ifdef CONFIG_SECURITY
//    void            *f_security;
#endif
	/* needed for tty driver, and maybe others */
	void	*private_data;

#ifdef CONFIG_EPOLL
    /* Used by fs/eventpoll.c to link all the hooks to this file */
//    struct list_head    f_ep_links;
//    struct list_head    f_tfile_llink;
#endif /* #ifdef CONFIG_EPOLL */
//    struct address_space    *f_mapping;
} __aligned(4); //__attribute__((aligned(4)));  /* lest something weird decides that 2 is OK */

const struct file_operations {
	struct module *owner;
	//loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	long (*unlocked_ioctl)(struct file *, unsigned int, unsigned long);
	//long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open)(struct inode *, struct file *);
	int (*release)(struct inode *, struct file *);
};

/* File is opened with O_PATH; almost nothing can be done with it */
#define FMODE_PATH      ((__force fmode_t)0x4000)
#define get_file_rcu(x) atomic_long_inc_not_zero(&(x)->f_count)
#define CHRDEV_MAJOR_DYN_END	234
#define CHRDEV_MAJOR_HASH_SIZE	255

int register_chrdev_region(dev_t from, unsigned int count, const char *name);
int alloc_chrdev_region(dev_t *dev, unsigned int baseminor, unsigned int count,
	const char *name);

static inline int register_chrdev(unsigned int major, const char *name,
	const struct file_operations *fops)
{
	return __register_chrdev(major, 0, 256, name, fops);
}

static inline void unregister_chrdev(unsigned int major, const char *name)
{
	__unregister_chrdev(major, 0, 256, name);
}

extern long open(const char __user *filename, int flags);
extern int close(unsigned int fd);

#endif
