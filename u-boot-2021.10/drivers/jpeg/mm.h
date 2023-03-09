#ifndef __JPU_VIDEO_MEMORY_MANAGEMENT_H__
#define __JPU_VIDEO_MEMORY_MANAGEMENT_H__

typedef struct _jmem_info_struct {
	unsigned long total_pages;
	unsigned long alloc_pages;
	unsigned long free_pages;
	unsigned long page_size;
} jmem_info_t;

#if defined(WIN32) || defined(WIN64)
#if (_MSC_VER == 1200)
typedef _int64 vmem_key_t;
#else
typedef unsigned long long vmem_key_t;
#endif
#else
typedef unsigned long long vmem_key_t;
#endif

#define VMEM_PAGE_SIZE (16 * 1024)

#define MAKE_KEY(_a, _b) (((vmem_key_t)(_a)) << 32 | (_b))
#define KEY_TO_VALUE(_key) ((_key) >> 32)

typedef struct page_struct {
	int pageno;
	unsigned long addr;
	int used;
	int alloc_pages;
	int first_pageno;
} page_t;

typedef struct avl_node_struct {
	vmem_key_t key;
	int height;
	page_t *page;
	struct avl_node_struct *left;
	struct avl_node_struct *right;
} avl_node_t;

typedef struct _jpeg_mm_struct {
	avl_node_t *free_tree;
	avl_node_t *alloc_tree;
	page_t *page_list;
	int num_pages;
	unsigned long base_addr;
	unsigned long mem_size;
	void *mutex;
	int free_page_count;
	int alloc_page_count;
} jpeg_mm_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern int jmem_init(jpeg_mm_t *mm, unsigned long addr, unsigned long size);

extern int jmem_exit(jpeg_mm_t *mm);

extern unsigned long jmem_alloc(jpeg_mm_t *mm, unsigned int size, unsigned long pid);

extern int jmem_free(jpeg_mm_t *mm, unsigned long ptr, unsigned long pid);

extern int jmem_get_info(jpeg_mm_t *mm, jmem_info_t *info);

#if defined(__cplusplus)
}
#endif
#endif /* __JPU_VIDEO_MEMORY_MANAGEMENT_H__ */