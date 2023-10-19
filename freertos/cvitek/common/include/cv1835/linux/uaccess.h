/*
 *  arch/arm/include/asm/uaccess.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _ASMARM_UACCESS_H
#define _ASMARM_UACCESS_H

/*
 * User space memory access functions
 */
//#include <linux/string.h>
//#include <linux/thread_info.h>
//#include <asm/errno.h>
//#include <asm/memory.h>
//#include <asm/domain.h>
//#include <asm/unified.h>
#include "linux/compiler.h"
#include <string.h>

#define __range_ok(addr, size) ((void)(addr), 0)
#define access_ok(type, addr, size) (__range_ok(addr, size) == 0)

#define __copy_from_user(to, from, n) (memcpy(to, (void __force *)from, n), 0)
#define __copy_to_user(to, from, n) (memcpy((void __force *)to, from, n), 0)
#define __clear_user(addr, n) (memset((void __force *)addr, 0, n), 0)

static inline unsigned long __must_check copy_from_user(void *to,
							const void __user *from,
							unsigned long n)
{
	unsigned long res = n;

	if (likely(access_ok(VERIFY_READ, from, n)))
		res = __copy_from_user(to, from, n);
	if (unlikely(res))
		memset(to + (n - res), 0, res);
	return res;
}

static inline unsigned long __must_check copy_to_user(void __user *to,
						      const void *from,
						      unsigned long n)
{
	if (access_ok(VERIFY_WRITE, to, n))
		n = __copy_to_user(to, from, n);
	return n;
}

#define __copy_to_user_inatomic __copy_to_user
#define __copy_from_user_inatomic __copy_from_user

static inline unsigned long __must_check clear_user(void __user *to,
						    unsigned long n)
{
	if (access_ok(VERIFY_WRITE, to, n))
		n = __clear_user(to, n);
	return n;
}

#endif /* _ASMARM_UACCESS_H */
