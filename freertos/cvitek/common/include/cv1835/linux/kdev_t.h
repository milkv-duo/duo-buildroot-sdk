#ifndef _LINUX_KDEV_T_H
#define _LINUX_KDEV_T_H

#define MINORBITS	20
#define MINORMASK	((1U << MINORBITS) - 1)

#define MAJOR(dev)	((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)	((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma, mi)	(((ma) << MINORBITS) | (mi))

#define module_param(a, b, c)
#endif
