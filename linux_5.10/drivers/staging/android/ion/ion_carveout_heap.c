// SPDX-License-Identifier: GPL-2.0
/*
 * drivers/staging/android/ion/ion_carveout_heap.c
 *
 * Copyright (C) 2011 Google, Inc.
 */
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/genalloc.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/of.h>
#include "ion.h"

#define ION_CARVEOUT_ALLOCATE_FAIL	-1

struct ion_carveout_heap {
	struct ion_heap heap;
	struct gen_pool *pool;
	phys_addr_t base;
};

static phys_addr_t ion_carveout_allocate(struct ion_heap *heap,
					 unsigned long size)
{
	struct ion_carveout_heap *carveout_heap =
		container_of(heap, struct ion_carveout_heap, heap);
	unsigned long offset = gen_pool_alloc(carveout_heap->pool, size);

	if (!offset)
		return ION_CARVEOUT_ALLOCATE_FAIL;

	return offset;
}

static void ion_carveout_free(struct ion_heap *heap, phys_addr_t addr,
			      unsigned long size)
{
	struct ion_carveout_heap *carveout_heap =
		container_of(heap, struct ion_carveout_heap, heap);

	if (addr == ION_CARVEOUT_ALLOCATE_FAIL)
		return;
	gen_pool_free(carveout_heap->pool, addr, size);
}

static int ion_carveout_heap_allocate(struct ion_heap *heap,
				      struct ion_buffer *buffer,
				      unsigned long size,
				      unsigned long flags)
{
	struct sg_table *table;
	phys_addr_t paddr;
	int ret;

	table = kmalloc(sizeof(*table), GFP_KERNEL);
	if (!table)
		return -ENOMEM;
	ret = sg_alloc_table(table, 1, GFP_KERNEL);
	if (ret)
		goto err_free;

	paddr = ion_carveout_allocate(heap, size);
	if (paddr == ION_CARVEOUT_ALLOCATE_FAIL) {
		ret = -ENOMEM;
		goto err_free_table;
	}

	sg_set_page(table->sgl, pfn_to_page(PFN_DOWN(paddr)), size, 0);
	buffer->sg_table = table;
#ifdef CONFIG_ION_CVITEK
	buffer->paddr = paddr;
#endif
	return 0;

err_free_table:
	sg_free_table(table);
err_free:
	kfree(table);
	return ret;
}

static void ion_carveout_heap_free(struct ion_buffer *buffer)
{
	struct ion_heap *heap = buffer->heap;
	struct sg_table *table = buffer->sg_table;
	struct page *page = sg_page(table->sgl);
	phys_addr_t paddr = PFN_PHYS(page_to_pfn(page));

	//remove it for saving cpu usage and vmap/unvmap frequency
	//ion_heap_buffer_zero(buffer);
#ifdef CONFIG_ION_CVITEK
	if (buffer->name)
		vfree(buffer->name);
#endif
	ion_carveout_free(heap, paddr, buffer->size);
	sg_free_table(table);
	kfree(table);
}

static struct ion_heap_ops carveout_heap_ops = {
	.allocate = ion_carveout_heap_allocate,
	.free = ion_carveout_heap_free,
	.map_user = ion_heap_map_user,
	.map_kernel = ion_heap_map_kernel,
	.unmap_kernel = ion_heap_unmap_kernel,
};

#ifdef CONFIG_ION_CVITEK
void cvi_get_rtos_ion_size(size_t *psize)
{
	struct device_node * np;
	u32 rtos_ion_size;
	int ret;

	np = of_find_compatible_node(NULL, NULL, "cvitek,rtos_image");
	if (!np) {
		*psize = 0;
		return;
	}

	ret = of_property_read_u32(np, "ion-size", &rtos_ion_size);
	*psize = rtos_ion_size;
	if(ret)
		*psize = 0;

	of_node_put(np);

	pr_info("%s, rtos ion_size get:0x%lx\n", __func__, *psize);
}
#endif

struct ion_heap *ion_carveout_heap_create(struct ion_platform_heap *heap_data)
{
	struct ion_carveout_heap *carveout_heap;
	int ret;

	struct page *page;
	size_t size;
#ifdef CONFIG_ION_CVITEK
	size_t rtos_ion_size;
	struct page *rtos_end_page;
	phys_addr_t heap_end_addr;
#endif

	page = pfn_to_page(PFN_DOWN(heap_data->base));
	size = heap_data->size;

	pr_info("%s, size=0x%lx\n", __func__, size);

#ifdef CONFIG_ION_CVITEK
	/* cvitek: freertos will use this space before linux boot up, we shouldn't modify it. */
	cvi_get_rtos_ion_size(&rtos_ion_size);
	if(rtos_ion_size){
		heap_end_addr = heap_data->base + size;
		rtos_end_page = pfn_to_page(PFN_UP(heap_data->base + rtos_ion_size));
		size = heap_end_addr - page_to_phys(rtos_end_page);
		page = rtos_end_page;
		pr_info("%s, size(exclusion of rtos_ion_size)=0x%lx\n", __func__, size);
	}
#endif

	ret = ion_heap_pages_zero(page, size, pgprot_writecombine(PAGE_KERNEL));
	if (ret)
		return ERR_PTR(ret);

	carveout_heap = kzalloc(sizeof(*carveout_heap), GFP_KERNEL);
	if (!carveout_heap)
		return ERR_PTR(-ENOMEM);

	carveout_heap->pool = gen_pool_create(PAGE_SHIFT, -1);
	if (!carveout_heap->pool) {
		kfree(carveout_heap);
		return ERR_PTR(-ENOMEM);
	}

	gen_pool_set_algo(carveout_heap->pool, gen_pool_best_fit, NULL);
	carveout_heap->base = heap_data->base;
	gen_pool_add(carveout_heap->pool, carveout_heap->base, heap_data->size,
		     -1);
	carveout_heap->heap.ops = &carveout_heap_ops;
	carveout_heap->heap.type = ION_HEAP_TYPE_CARVEOUT;
#ifndef CONFIG_ION_CVITEK
	carveout_heap->heap.flags = ION_HEAP_FLAG_DEFER_FREE;
#endif
    carveout_heap->heap.name = heap_data->name;
#ifdef CONFIG_ION_CVITEK
	carveout_heap->heap.total_size = heap_data->size;
#endif
	return &carveout_heap->heap;
}
