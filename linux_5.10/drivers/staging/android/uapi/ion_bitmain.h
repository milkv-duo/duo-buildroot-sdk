/*
 * Bitmain SoC custom ion ioctl
 *
 * Copyright (c) 2018 Bitmain Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __ION_BITMAIN_H__
#define __ION_BITMAIN_H__

#include "ion.h"

struct bitmain_cache_range {
	void *start;
	size_t size;
};

struct bitmain_heap_info {
	unsigned int id;
	unsigned long total_size;
	unsigned long avail_size;
};

#define ION_IOC_BITMAIN_FLUSH_RANGE		1
#define ION_IOC_BITMAIN_GET_HEAP_INFO		2

#endif /* __ION_BITMAIN_H__ */
