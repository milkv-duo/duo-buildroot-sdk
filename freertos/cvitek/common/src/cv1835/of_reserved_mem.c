/*
 * Device tree based initialization code for reserved memory.
 *
 * Copyright (c) 2013, 2015 The Linux Foundation. All Rights Reserved.
 * Copyright (c) 2013,2014 Samsung Electronics Co., Ltd.
 *      http://www.samsung.com
 * Author: Marek Szyprowski <m.szyprowski@samsung.com>
 * Author: Josh Cartwright <joshc@codeaurora.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License or (at your optional) any later version of the license.
 */

#include <linux/of_reserved_mem.h>
#include <linux/of.h>
#include "malloc.h"

struct reserved_mem *reserved_mem;
int reserved_mem_count;

/**
 * of_reserved_mem_lookup() - acquire reserved_mem from a device node
 * @np:     node pointer of the desired reserved-memory region
 *
 * This function allows drivers to acquire a reference to the reserved_mem
 * struct based on a device node handle.
 *
 * Returns a reserved_mem reference, or NULL on error.
 */
struct reserved_mem *of_reserved_mem_lookup(struct device_node *np)
{
    const char *name;
    int i;

    if (!np->name)
        return 0;
	size_t *reserved_mem_ptr = reserved_mem;
	printf("reserved_mem_ptr=%lx\n");
//	struct reserved_mem *res_mem = reserved_mem;
    //name = kbasename(np->full_name);
    for (i = 0; i < reserved_mem_count; i++, reserved_mem_ptr++) {
		struct reserved_mem *res_mem = (struct reserved_mem*) ((size_t) (*reserved_mem_ptr));
		struct reserved_mem *np_mem = np->data;
		printf("res_mem=%lx\n", res_mem);
		printf("res_mem->name =%s\n", res_mem->name);
		printf("np->name =%s\n", np_mem->name);
        if (!strcmp(res_mem->name, np_mem->name))
            return res_mem;
	}
    return 0;
}

/**
 * rtos_init_reserved_mem - allocate and init all saved reserved memory regions
 */
void rtos_init_reserved_mem(struct reserved_mem *res_mem, int count)
//void __init rtos_init_reserved_mem(struct *res_mem, int count)
{
	int i;
	size_t *reserved_mem_ptr = res_mem;
	reserved_mem_count = count;
	reserved_mem = res_mem;
    for (i = 0; i < reserved_mem_count; i++, reserved_mem_ptr++) {
        struct reserved_mem *rmem = (struct reserved_mem*) ((size_t) (*reserved_mem_ptr));
        //struct reserved_mem *rmem = reserved_mem[i];
		printf("rmem=%lx\n", rmem);
		printf("rmem size=%lx\n", rmem->size);
		rmem->base = calloc(1, rmem->size);
		printf("rmem base=%lx\n", rmem->base);
    }
}   

