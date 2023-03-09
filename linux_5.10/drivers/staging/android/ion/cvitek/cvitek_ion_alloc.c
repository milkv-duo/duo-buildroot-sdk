/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvitek_ion.c
 * Description:
 */

#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/uaccess.h>

#include "../ion.h"
#include "cvitek_ion_alloc.h"

struct cvi_ion_alloc_info {
	size_t len;
	unsigned int heap_id_mask;
	unsigned int flags;
	struct ion_buffer *buf;
};

int _cvi_ion_alloc(enum ion_heap_type type, size_t len, bool mmap_cache, struct cvi_ion_alloc_info *ion_alloc_info)
{
	struct ion_heap_query query;
	int ret = 0, index;
	unsigned int heap_id;
	struct ion_heap_data *heap_data;
#if defined(__arm__) || defined(__aarch64__)
	mm_segment_t old_fs = get_fs();
#endif
	memset(&query, 0, sizeof(struct ion_heap_query));
	query.cnt = HEAP_QUERY_CNT;
	heap_data = vzalloc(sizeof(*heap_data) * HEAP_QUERY_CNT);
	query.heaps = (unsigned long)heap_data;
	if (!query.heaps)
		return -ENOMEM;

	pr_debug("%s: len %zu looking for type %d and mmap it as %s\n",
		 __func__, len, type,
		 (mmap_cache) ? "cacheable" : "un-cacheable");

#if defined(__arm__) || defined(__aarch64__)
	set_fs(KERNEL_DS);
#else
	mm_segment_t old_fs = force_uaccess_begin();
#endif
	ret = ion_query_heaps(&query, true);

#if defined(__arm__) || defined(__aarch64__)
	set_fs(old_fs);
#else
	force_uaccess_end(old_fs);
#endif

	if (ret != 0)
		return ret;

	heap_id = HEAP_QUERY_CNT + 1;
	/* here only return the 1st match
	 * heap id that user requests for
	 */
	for (index = 0; index < query.cnt; index++) {
		if (heap_data[index].type == type) {
			heap_id = heap_data[index].heap_id;
			break;
		}
	}
	vfree(heap_data);

	ion_alloc_info->len = len;
	ion_alloc_info->heap_id_mask = 1 << heap_id;
	ion_alloc_info->flags = ((mmap_cache) ? 1 : 0);

	return 0;
}

struct ion_buffer *
cvi_ion_alloc_nofd(enum ion_heap_type type, size_t len, bool mmap_cache)
{
	int ret;
	struct cvi_ion_alloc_info ion_alloc_info;

	ret = _cvi_ion_alloc(type, len, mmap_cache, &ion_alloc_info);
	if (ret)
		return ERR_PTR(ret);

	return ion_alloc_nofd(ion_alloc_info.len, ion_alloc_info.heap_id_mask,
			      ion_alloc_info.flags);
}
EXPORT_SYMBOL(cvi_ion_alloc_nofd);

int cvi_ion_alloc(enum ion_heap_type type, size_t len, bool mmap_cache)
{
	int ret;
	struct cvi_ion_alloc_info ion_alloc_info;

	ret = _cvi_ion_alloc(type, len, mmap_cache, &ion_alloc_info);
	if (ret)
		return ret;

	return ion_alloc(ion_alloc_info.len, ion_alloc_info.heap_id_mask,
			 ion_alloc_info.flags, &ion_alloc_info.buf);
}
EXPORT_SYMBOL(cvi_ion_alloc);

void cvi_ion_free(pid_t fd_pid, int fd)
{
	ion_free(fd_pid, fd);
}
EXPORT_SYMBOL(cvi_ion_free);

void cvi_ion_free_nofd(struct ion_buffer *buffer)
{
	ion_free_nofd(buffer);
}
EXPORT_SYMBOL(cvi_ion_free_nofd);

void cvi_ion_dump(struct ion_heap *heap)
{
	struct ion_device *dev = heap->dev;
	struct ion_buffer *pos, *n;
	size_t total_size = heap->total_size;
	size_t alloc_size;
	u64 alloc_bytes_wm;
	int usage_rate = 0;
	int rem;
	u64 tmp;

	spin_lock(&heap->stat_lock);
	alloc_size = heap->num_of_alloc_bytes;
	alloc_bytes_wm = heap->alloc_bytes_wm;
	spin_unlock(&heap->stat_lock);

	pr_err("Summary:\n");

	tmp = (uint64_t)alloc_size * 100;
	rem = do_div(tmp, total_size);
	usage_rate = tmp;
	if (rem)
		usage_rate += 1;

	pr_err("[%d] %s heap size:%zu bytes, used:%zu bytes\n",
	       heap->id, heap->name, total_size, alloc_size);

	pr_err("usage rate:%d%%, memory usage peak %llu bytes\n",
	       usage_rate, alloc_bytes_wm);

	pr_err("\nDetails:\n%16s %16s %16s %16s %16s\n", "heap_id",
	       "alloc_buf_size", "phy_addr", "kmap_cnt", "buffer name");
	mutex_lock(&dev->buffer_lock);
	rbtree_postorder_for_each_entry_safe(pos, n, &dev->buffers, node) {
		/* only heap id matches will show buffer info */
		if (heap->id == pos->heap->id)
			pr_err("%16d %16zu %16llx %16d %16s\n",
			       pos->heap->id, pos->size, pos->paddr,
			       pos->kmap_cnt, pos->name);
	}
	mutex_unlock(&dev->buffer_lock);
	pr_err("\n");
}

