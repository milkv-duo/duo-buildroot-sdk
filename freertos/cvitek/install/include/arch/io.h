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
