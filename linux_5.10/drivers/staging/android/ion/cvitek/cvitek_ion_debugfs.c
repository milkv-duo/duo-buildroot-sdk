/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvitek_ion_debugfs.c
 * Description:
 */

#define pr_fmt(fmt) "Ion: " fmt

#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>
#include "../ion.h"

#include "../../uapi/ion_cvitek.h"
#include <linux/genalloc.h>
#include <linux/types.h>

struct ion_carveout_heap {
	struct ion_heap heap;
	struct gen_pool *pool;
	phys_addr_t base;
};

static inline size_t chunk_size(const struct gen_pool_chunk *chunk)
{
	return chunk->end_addr - chunk->start_addr + 1;
}

void show_bitmap_mem_usage(struct seq_file *s, u32 heap_id, struct gen_pool_chunk *chunk, u32 size_in_bit, int order)
{
	unsigned long *map = chunk->bits;
	phys_addr_t region_start_addr;
	phys_addr_t region_end_addr;
	u64 region_len;
	u32 free_index_start;
	u32 free_index_end = 0;
	phys_addr_t chunk_start_addr = chunk->start_addr;

	seq_printf(s, "\n\nminimum ion allocate unit = %u\n", (u32)(2 << (order - 1)));

	seq_puts(s, "free memory regions:\n");

	seq_printf(s, "%16s %16s %16s %16s\n", "heap_id", "start", "end", "length");

	while (free_index_end < size_in_bit) {
		free_index_start = find_next_zero_bit(map, size_in_bit, free_index_end);
		if (free_index_start >= size_in_bit) {
			break;
		}

		free_index_end = find_next_bit(map, size_in_bit, free_index_start);

		region_start_addr = chunk_start_addr + (free_index_start << order);
		region_end_addr = chunk_start_addr + (free_index_end << order);
		region_len = (free_index_end - free_index_start) << order;

		seq_printf(s, "%16d %16llx %16llx %16llu\n", heap_id, region_start_addr, region_end_addr, region_len);
	}

}

void show_carveout_heap_usage(struct seq_file *s, struct ion_heap *heap)
{
	int end_bit;
	struct ion_carveout_heap *carveout_heap;
	struct gen_pool *pool;
	struct gen_pool_chunk *chunk;
	int order;

	if (heap->type != ION_HEAP_TYPE_CARVEOUT)
		return;

	carveout_heap = container_of(heap, struct ion_carveout_heap, heap);
	pool = carveout_heap->pool;
	order = pool->min_alloc_order;

	#ifndef CONFIG_ARCH_HAVE_NMI_SAFE_CMPXCHG
	if (in_nmi())
		return;
	#endif

	rcu_read_lock();
	list_for_each_entry_rcu(chunk, &pool->chunks, next_chunk) {
		end_bit = chunk_size(chunk) >> order;
		show_bitmap_mem_usage(s, heap->id, chunk, end_bit, order);
	}
	rcu_read_unlock();
}

static int cvi_ion_debug_heap_show(struct seq_file *s, void *unused)
{
	struct ion_heap *heap = s->private;
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

	seq_puts(s, "Summary:\n");
	if (heap->debug_show)
		heap->debug_show(heap, s, unused);

	tmp = (uint64_t)alloc_size * 100;
	rem = do_div(tmp, total_size);
	usage_rate = tmp;
	if (rem)
		usage_rate += 1;

	seq_printf(s, "[%d] %s heap size:%zu bytes, used:%zu bytes\n",
		   heap->id, heap->name, total_size, alloc_size);

	seq_printf(s, "usage rate:%d%%, memory usage peak %llu bytes\n",
		   usage_rate, alloc_bytes_wm);

	seq_printf(s, "\nDetails:\n%16s %16s %16s %16s %16s\n", "heap_id",
		   "alloc_buf_size", "phy_addr", "kmap_cnt", "buffer name");
	mutex_lock(&dev->buffer_lock);
	rbtree_postorder_for_each_entry_safe(pos, n, &dev->buffers, node) {
		/* only heap id matches will show buffer info */
		if (heap->id == pos->heap->id)
			seq_printf(s, "%16d %16zu %16llx %16d %16s\n",
				   pos->heap->id, pos->size, pos->paddr,
				   pos->kmap_cnt, pos->name);
	}
	mutex_unlock(&dev->buffer_lock);
	show_carveout_heap_usage(s, heap);
	seq_puts(s, "\n");

	return 0;
}

static int cvi_ion_debug_heap_open(struct inode *inode, struct file *file)
{
	return single_open(file, cvi_ion_debug_heap_show, inode->i_private);
}

static const struct file_operations debug_heap_fops = {
	.open = cvi_ion_debug_heap_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int cvi_debugfs_get_alloc_mem(void *data, u64 *val)
{
	struct ion_heap *heap = data; 

	spin_lock(&heap->stat_lock);
	*val = heap->num_of_alloc_bytes;
	spin_unlock(&heap->stat_lock);

	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(cvi_get_alloc_mem, cvi_debugfs_get_alloc_mem, NULL, "%llu\n");

static int cvi_debugfs_get_total_size(void *data, u64 *val)
{
	struct ion_heap *heap = data;
	
	*val = heap->total_size;

	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(cvi_get_total_size, cvi_debugfs_get_total_size, NULL, "%llu\n");

static int cvi_get_peak(void *data, u64 *val)
{
	struct ion_heap *heap = data;

	spin_lock(&heap->stat_lock);
	*val = heap->alloc_bytes_wm;
	spin_unlock(&heap->stat_lock);

	return 0;
}

static int cvi_clear_peak(void *data, u64 val)
{
	struct ion_heap *heap = data;

	spin_lock(&heap->stat_lock);
	heap->alloc_bytes_wm = 0;
	spin_unlock(&heap->stat_lock);

	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(heap_peak, cvi_get_peak, cvi_clear_peak, "%llu\n");

void cvi_ion_create_debug_info(struct ion_heap *heap)
{
	struct dentry *debug_file;
	struct ion_device *dev = heap->dev;
	char debug_heap_name[64];

	snprintf(debug_heap_name, 64, "cvi_%s_heap_dump", heap->name);

	heap->heap_dfs_root =
		debugfs_create_dir(debug_heap_name, dev->debug_root);
	if (!heap->heap_dfs_root) {
		pr_err("%s: failed to create debugfs root directory.\n",
			   debug_heap_name);
		return;
	}
	debug_file = debugfs_create_file(
		"summary", 0644, heap->heap_dfs_root, heap,
		&debug_heap_fops);
	if (!debug_file) {
		char buf[256], *path;

		path = dentry_path(dev->debug_root, buf, 256);
		pr_err("Failed to create heap debugfs at %s/%s\n",
			   path, debug_heap_name);
	}
	debug_file = debugfs_create_file("peak", 0755,
					 heap->heap_dfs_root, heap,
					 &heap_peak);
	if (!debug_file) {
		char buf[256], *path;

		path = dentry_path(heap->heap_dfs_root, buf, 256);
		pr_err("Failed to create clear_peak heap debugfs at %s/%s\n",
			   path, debug_heap_name);
	}

	debug_file = debugfs_create_file("alloc_mem", 0644,
					 heap->heap_dfs_root,
					 heap,
					 &cvi_get_alloc_mem);
	if (!debug_file) {
		char buf[256], *path;

		path = dentry_path(heap->heap_dfs_root, buf, 256);
		pr_err("Failed to create ava_mem heap debugfs at %s/%s\n",
			   path, debug_heap_name);
	}

	debug_file = debugfs_create_file("total_mem", 0644,
					 heap->heap_dfs_root, heap,
					 &cvi_get_total_size);
	if (!debug_file) {
		char buf[256], *path;

		path = dentry_path(heap->heap_dfs_root, buf, 256);
		pr_err("Failed to create all_mem heap debugfs at %s/%s\n",
			   path, debug_heap_name);
	}
}
