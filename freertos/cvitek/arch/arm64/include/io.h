/*
 * Based on arch/arm/include/asm/io.h
 *
 * Copyright (C) 1996-2000 Russell King
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_IO_H
#define __ASM_IO_H

#include "asm/barrier.h"
#include "xil_types.h"
/*
 * Generic IO read/write.  These perform native-endian accesses.
 */
#ifdef __CHECKER__
#define __iomem __attribute__((noderef, address_space(2)))
#else
#define __iomem
#endif

#define __raw_writeb __raw_writeb
static inline void __raw_writeb(u8 val, volatile void __iomem *addr)
{
	asm volatile("strb %w0, [%1]" : : "rZ" (val), "r" (addr));
}

#define __raw_writew __raw_writew
static inline void __raw_writew(u16 val, volatile void __iomem *addr)
{
	asm volatile("strh %w0, [%1]" : : "rZ" (val), "r" (addr));
}

#define __raw_writel __raw_writel
static inline void __raw_writel(u32 val, volatile void __iomem *addr)
{
	asm volatile("str %w0, [%1]" : : "rZ" (val), "r" (addr));
}

#define __raw_writeq __raw_writeq
static inline void __raw_writeq(u64 val, volatile void __iomem *addr)
{
	asm volatile("str %x0, [%1]" : : "rZ" (val), "r" (addr));
}

#define __raw_readb __raw_readb
static inline u8 __raw_readb(const volatile void __iomem *addr)
{
	u8 val;
	asm volatile("ldrb %w0, [%1]"
		     : "=r" (val) : "r" (addr));
	return val;
}

#define __raw_readw __raw_readw
static inline u16 __raw_readw(const volatile void __iomem *addr)
{
	u16 val;

	asm volatile("ldrh %w0, [%1]"
		     : "=r" (val) : "r" (addr));
	return val;
}

#define __raw_readl __raw_readl
static inline u32 __raw_readl(const volatile void __iomem *addr)
{
	u32 val;
	asm volatile("ldr %w0, [%1]"
		     : "=r" (val) : "r" (addr));
	return val;
}

#define __raw_readq __raw_readq
static inline u64 __raw_readq(const volatile void __iomem *addr)
{
	u64 val;
	asm volatile("ldr %0, [%1]"
		     : "=r" (val) : "r" (addr));
	return val;
}

/* IO barriers */
#define __iormb()		rmb()
#define __iowmb()		wmb()

#define mmiowb()		do { } while (0)


/* move from uapi/linux/byteorder/little_endian.h */
#define cpu_to_le64(x) ((__force __le64)(__u64)(x))
#define le64_to_cpu(x) ((__force __u64)(__le64)(x))
#define cpu_to_le32(x) ((__force __le32)(__u32)(x))
#define le32_to_cpu(x) ((__force __u32)(__le32)(x))
#define cpu_to_le16(x) ((__force __le16)(__u16)(x))
#define le16_to_cpu(x) ((__force __u16)(__le16)(x))

/*
 * Relaxed I/O memory access primitives. These follow the Device memory
 * ordering rules but do not guarantee any ordering relative to Normal memory
 * accesses.
 */

#define readb_relaxed(c)    ({ u8  __r = __raw_readb(c); __r; })
#define readw_relaxed(c)    ({ u16 __r = le16_to_cpu((__force __le16)__raw_readw(c)); __r; })
#define readl_relaxed(c)    ({ u32 __r = le32_to_cpu((__force __le32)__raw_readl(c)); __r; })
#define readq_relaxed(c)    ({ u64 __r = le64_to_cpu((__force __le64)__raw_readq(c)); __r; })

#define writeb_relaxed(v, c) ((void)__raw_writeb((v), (c)))
#define writew_relaxed(v, c) ((void)__raw_writew((__force u16)cpu_to_le16(v), (c)))
#define writel_relaxed(v, c) ((void)__raw_writel((__force u32)cpu_to_le32(v), (c)))
#define writeq_relaxed(v, c) ((void)__raw_writeq((__force u64)cpu_to_le64(v), (c)))

/*
 * I/O memory access primitives. Reads are ordered relative to any
 * following Normal memory access. Writes are ordered relative to any prior
 * Normal memory access.
 */
#define readb(c)		({ u8  __v = readb_relaxed(c); __iormb(); __v; })
#define readw(c)		({ u16 __v = readw_relaxed(c); __iormb(); __v; })
#define readl(c)		({ u32 __v = readl_relaxed(c); __iormb(); __v; })
#define readq(c)		({ u64 __v = readq_relaxed(c); __iormb(); __v; })

#define writeb(v, c)		({ __iowmb(); writeb_relaxed((v), (c)); })
#define writew(v, c)		({ __iowmb(); writew_relaxed((v), (c)); })
#define writel(v, c)		({ __iowmb(); writel_relaxed((v), (c)); })
#define writeq(v, c)		({ __iowmb(); writeq_relaxed((v), (c)); })

#define ioread8 readb
#define ioread16 readw
#define ioread32 readl
#define ioread64 readq

#define iowrite8 writeb
#define iowrite16 writew
#define iowrite32 writel
#define iowrite64 writeq

#endif	/* __ASM_IO_H */
