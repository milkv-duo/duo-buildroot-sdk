/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MMIO_H__
#define __MMIO_H__

//#include <stdint.h>
#include "linux/types.h"
#include "io.h"

#define mmio_wr32 mmio_write_32
#define mmio_rd32 mmio_read_32

static inline void mmio_write_8(uintptr_t addr, uint8_t value)
{
	writeb(value, (void *) addr);
}

static inline uint8_t mmio_read_8(uintptr_t addr)
{
	return readb((void *) addr);
}

static inline void mmio_write_16(uintptr_t addr, uint16_t value)
{
	writew(value, (void *) addr);
}

static inline uint16_t mmio_read_16(uintptr_t addr)
{
	return readw((void *) addr);
}

static inline void mmio_write_32(uintptr_t addr, uint32_t value)
{
	writel(value, (void *) addr);
}

static inline uint32_t mmio_read_32(uintptr_t addr)
{
	return readl((void *) addr);
}

static inline void mmio_write_64(uintptr_t addr, uint64_t value)
{
	writeq(value, (void *) addr);
}

static inline uint64_t mmio_read_64(uintptr_t addr)
{
	return readq((void *) addr);
}

static inline void mmio_clrbits_32(uintptr_t addr, uint32_t clear)
{
	writel(readl((void *) addr) & ~clear , (void *) addr);
}

static inline void mmio_setbits_32(uintptr_t addr, uint32_t set)
{
	writel(readl((void *) addr) | set , (void *) addr);
}

static inline void mmio_clrsetbits_32(uintptr_t addr, uint32_t clear,
				      uint32_t set)
{
	writel((readl((void *) addr) & ~clear) | set , (void *) addr);
}

#endif /* __MMIO_H__ */
