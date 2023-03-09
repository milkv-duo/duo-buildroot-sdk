
#include <stdlib.h>
#include "mm.h"
#include "jdi_osal.h"
#include <common.h>
#include "jpulog.h"

#define P_ALLOC(_x)		OSAL_MALLOC(_x)
#define P_FREE(_x)		OSAL_FREE(_x)
//#define assert(_exp)		//    if (!(_exp)) { osal_printf("assert at %s:%d\n", __FILE__, __LINE__); while(1); }
#define HEIGHT(_tree)		(!(_tree) ? -1 : _tree->height)

/*
 * doubly linked list
 */
#define MAX(_a, _b) ((_a) >= (_b) ? (_a) : (_b))

enum {
	LEFT,
	RIGHT
} rotation_dir_t;

struct avl_node_data {
	int		key;
	page_t *page;
} avl_node_data_t;

static avl_node_t *make_avl_node(vmem_key_t key, page_t *page)
{
	avl_node_t *node = (avl_node_t *)

	josal_malloc(sizeof(avl_node_t));

	node->key		= key;
	node->page		= page;
	node->height		= 0;
	node->left		= NULL;
	node->right		= NULL;

	return node;
}

static int get_balance_factor(avl_node_t *tree)
{
	int factor = 0;

	if (tree) {
		factor = HEIGHT(tree->right) - HEIGHT(tree->left);
	}

	return factor;
}

/*
 * Left Rotation
 *
 *	A		       B
 *	 \		      / \
 *	  B	    =>	     A	 C
 *	 /  \		      \
 *	D    C		       D
 *
 */
static avl_node_t *rotation_left(avl_node_t *tree)
{
	avl_node_t *rchild;
	avl_node_t *lchild;

	if (!tree)
		return NULL;

	rchild			= tree->right;

	if (!rchild) {
		return tree;
	}

	lchild			= rchild->left;
	rchild->left		= tree;
	tree->right		= lchild;

	tree->height		= MAX(HEIGHT(tree->left), HEIGHT(tree->right)) + 1;
	rchild->height		= MAX(HEIGHT(rchild->left), HEIGHT(rchild->right)) + 1;

	return rchild;
}

/*
 * Reft Rotation
 *
 *	   A		      B
 *	 \		    /  \
 *	B	  =>	   D	A
 *    /  \		       /
 *   D	  C		      C
 *
 */
static avl_node_t *rotation_right(avl_node_t *tree)
{
	avl_node_t *rchild;
	avl_node_t *lchild;

	if (!tree)
		return NULL;

	lchild			= tree->left;

	if (!lchild)
		return NULL;

	rchild			= lchild->right;
	lchild->right		= tree;
	tree->left		= rchild;

	tree->height		= MAX(HEIGHT(tree->left), HEIGHT(tree->right)) + 1;
	lchild->height		= MAX(HEIGHT(lchild->left), HEIGHT(lchild->right)) + 1;

	return lchild;
}

static avl_node_t *do_balance(avl_node_t *tree)
{
	int bfactor = 0, child_bfactor;	/* balancing factor */

	bfactor = get_balance_factor(tree);

	if (bfactor >= 2) {
		child_bfactor		= get_balance_factor(tree->right);

		if (child_bfactor == 1 || child_bfactor == 0) {
			tree			= rotation_left(tree);
		} else if (child_bfactor == -1) {
			tree->right		= rotation_right(tree->right);
			tree			= rotation_left(tree);
		} else {
			//fprintf(stderr, "invalid balancing factor: %d\n", child_bfactor);
			assert(0);
			return NULL;
		}
	} else if (bfactor <= -2) {
		child_bfactor		= get_balance_factor(tree->left);

		if (child_bfactor == -1 || child_bfactor == 0) {
			tree			= rotation_right(tree);
		} else if (child_bfactor == 1) {
			tree->left		= rotation_left(tree->left);
			tree			= rotation_right(tree);
		} else {
			//fprintf(stderr, "invalid balancing factor: %d\n", child_bfactor);
			assert(0);
			return NULL;
		}
	}

	return tree;
}

static avl_node_t *unlink_end_node(avl_node_t *tree, int dir, avl_node_t **found_node)
{
	//    avl_node_t* node;
	*found_node		= NULL;

	if (!tree)
		return NULL;

	if (dir == LEFT) {
		if (!tree->left) {
			*found_node		= tree;
			return NULL;
		}
	} else {
		if (!tree->right) {
			*found_node		= tree;
			return NULL;
		}
	}

	if (dir == LEFT) {
		//	  node = tree->left;
		tree->left		= unlink_end_node(tree->left, LEFT, found_node);

		if (!tree->left) {
			tree->left		= (*found_node)->right;
			(*found_node)->left	= NULL;
			(*found_node)->right	= NULL;
		}
	} else {
		//	  node = tree->right;
		tree->right		= unlink_end_node(tree->right, RIGHT, found_node);

		if (!tree->right) {
			tree->right		= (*found_node)->left;
			(*found_node)->left	= NULL;
			(*found_node)->right	= NULL;
		}
	}

	tree->height		= MAX(HEIGHT(tree->left), HEIGHT(tree->right)) + 1;

	return do_balance(tree);
}

static avl_node_t *avltree_insert(avl_node_t *tree, vmem_key_t key, page_t *page)
{
	if (!tree) {
		tree			= make_avl_node(key, page);
	} else {
		if (key >= tree->key) {
			tree->right		= avltree_insert(tree->right, key, page);
		} else {
			tree->left		= avltree_insert(tree->left, key, page);
		}
	}

	tree			= do_balance(tree);
	tree->height		= MAX(HEIGHT(tree->left), HEIGHT(tree->right)) + 1;

	return tree;
}

static avl_node_t *do_unlink(avl_node_t *tree)
{
	avl_node_t *node;
	avl_node_t *end_node;

	node			= unlink_end_node(tree->right, LEFT, &end_node);

	if (node) {
		tree->right		= node;
	} else {
		node			= unlink_end_node(tree->left, RIGHT, &end_node);

		if (node)
			tree->left = node;
	}

	if (!node) {
		node			= tree->right ? tree->right : tree->left;
		end_node		= node;
	}

	if (end_node) {
		end_node->left		= (tree->left != end_node) ? tree->left : end_node->left;
		end_node->right	= (tree->right != end_node) ? tree->right : end_node->right;
		end_node->height	= MAX(HEIGHT(end_node->left), HEIGHT(end_node->right)) + 1;
	}

	tree			= end_node;

	return tree;
}

static avl_node_t *avltree_remove(avl_node_t *tree, avl_node_t **found_node, vmem_key_t key)
{
	*found_node = NULL;

	if (!tree) {
	//	  DPRINT("failed to find key %d\n", key);
		return NULL;
	}

	if (key == tree->key) {
		*found_node = tree;
		tree = do_unlink(tree);
	} else if (key > tree->key) {
		tree->right = avltree_remove(tree->right, found_node, key);
	} else {
		tree->left  = avltree_remove(tree->left, found_node, key);
	}

	if (tree)
		tree->height = MAX(HEIGHT(tree->left), HEIGHT(tree->right)) + 1;

	tree = do_balance(tree);

	return tree;

}

void jpu_avltree_free(avl_node_t *tree)
{
	if (!tree)
		return;

	if (!tree->left && !tree->right) {
		P_FREE(tree);
		return;
	}

	jpu_avltree_free(tree->left);
	tree->left		= NULL;
	jpu_avltree_free(tree->right);
	tree->right		= NULL;
}

static avl_node_t *remove_approx_value(avl_node_t *tree, avl_node_t **found, vmem_key_t key)
{
	*found			= NULL;

	if (!tree) {
		return NULL;
	}

	if (key == tree->key) {
		*found			= tree;
		tree			= do_unlink(tree);
	} else if (key > tree->key) {
		tree->right		= remove_approx_value(tree->right, found, key);
	} else {
		tree->left		= remove_approx_value(tree->left, found, key);

		if (!*found) {
			*found			= tree;
			tree			= do_unlink(tree);
		}
	}

	if (tree)
		tree->height = MAX(HEIGHT(tree->left), HEIGHT(tree->right)) + 1;

	tree			= do_balance(tree);

	return tree;
}

static void set_blocks_free(jpeg_mm_t *mm, int pageno, int npages)
{
	int		last_pageno = pageno + npages - 1;
	int		i;
	page_t *page;
	page_t *last_page;

	assert(npages);

	if (last_pageno >= mm->num_pages) {
		//	  DPRINT("set_blocks_free: invalid last page number: %d\n", last_pageno);
		assert(0);
		return;
	}

	for (i = pageno; i <= last_pageno; i++) {
		mm->page_list[i].used	= 0;
		mm->page_list[i].alloc_pages = 0;
		mm->page_list[i].first_pageno = -1;
	}

	page			= &mm->page_list[pageno];
	page->alloc_pages	= npages;
	last_page		= &mm->page_list[last_pageno];
	last_page->first_pageno = pageno;

	mm->free_tree		= avltree_insert(mm->free_tree, MAKE_KEY(npages, pageno), page);
}

static void set_blocks_alloc(jpeg_mm_t *mm, int pageno, int npages)
{
	int		last_pageno = pageno + npages - 1;
	int		i;
	page_t *page;
	page_t *last_page;

	if (last_pageno >= mm->num_pages) {
		//	  DPRINT("set_blocks_free: invalid last page number: %d\n", last_pageno);
		assert(0);
		return;
	}

	for (i = pageno; i <= last_pageno; i++) {
		mm->page_list[i].used	= 1;
		mm->page_list[i].alloc_pages = 0;
		mm->page_list[i].first_pageno = -1;
	}

	page			= &mm->page_list[pageno];
	page->alloc_pages	= npages;

	last_page		= &mm->page_list[last_pageno];
	last_page->first_pageno = pageno;

	mm->alloc_tree		= avltree_insert(mm->alloc_tree, MAKE_KEY(page->addr, 0), page);
}

int jmem_init(jpeg_mm_t *mm, unsigned long addr, unsigned long size)
{
	int		i;

	mm->base_addr		= (addr + (VMEM_PAGE_SIZE - 1)) & ~(VMEM_PAGE_SIZE - 1);
	mm->mem_size		= size & ~VMEM_PAGE_SIZE;
	mm->num_pages		= mm->mem_size / VMEM_PAGE_SIZE;
	mm->page_list		= (page_t *)
	P_ALLOC(mm->num_pages * sizeof(page_t));
	mm->free_tree		= NULL;
	mm->alloc_tree		= NULL;
	mm->free_page_count	= mm->num_pages;
	mm->alloc_page_count	= 0;

	//	printf("page list addr: 0x%llx\n", mm->page_list);
	for (i = 0; i < mm->num_pages; i++) {
		mm->page_list[i].pageno = i;
		mm->page_list[i].addr	= mm->base_addr + i * VMEM_PAGE_SIZE;
		mm->page_list[i].alloc_pages = 0;
		mm->page_list[i].used	= 0;
		mm->page_list[i].first_pageno = -1;
	}

	set_blocks_free(mm, 0, mm->num_pages);
	return 0;
}

int jmem_exit(jpeg_mm_t *mm)
{
	if (!mm) {
		//	  DPRINT("vmem_exit: invalid handle\n");
		return -1;
	}

	if (mm->free_tree) {
		jpu_avltree_free(mm->free_tree);
	}

	if (mm->alloc_tree) {
		jpu_avltree_free(mm->alloc_tree);
	}

	P_FREE(mm->page_list);

	return 0;
}

unsigned long jmem_alloc(jpeg_mm_t *mm, unsigned int size, unsigned long pid)
{
	avl_node_t *node;
	page_t *free_page;
	int		npages, free_size;
	int		alloc_pageno;
	unsigned long	ptr;

	if (!mm) {
//		DPRINT("vmem_alloc: invalid handle\n");
		return -1;
	}

	if (size <= 0)
		return -1;

	npages			= (size + VMEM_PAGE_SIZE - 1) / VMEM_PAGE_SIZE;

	mm->free_tree		= remove_approx_value(mm->free_tree, &node, MAKE_KEY(npages, 0));

	if (!node) {
		return -1;
	}

	free_page		= node->page;
	free_size		= KEY_TO_VALUE(node->key);

	alloc_pageno		= free_page->pageno;
	set_blocks_alloc(mm, alloc_pageno, npages);

	if (npages != free_size) {
		int		free_pageno = alloc_pageno + npages;

		set_blocks_free(mm, free_pageno, (free_size - npages));
	}

	P_FREE(node);

	ptr			= mm->page_list[alloc_pageno].addr;
	mm->alloc_page_count	+= npages;
	mm->free_page_count	-= npages;

	return ptr;
}

int jmem_free(jpeg_mm_t *mm, unsigned long ptr, unsigned long pid)
{
	unsigned long	addr;
	avl_node_t *found;
	page_t *page;
	int		pageno, prev_free_pageno, next_free_pageno;
	int		prev_size, next_size;
	int		merge_page_no, merge_page_size, free_page_size;

	if (!mm) {
		//		DPRINT("vmem_free: invalid handle\n");
		return -1;
	}

	addr			= ptr;

	mm->alloc_tree		= avltree_remove(mm->alloc_tree, &found, MAKE_KEY(addr, 0));

	if (!found) {
		//	DPRINT("vmem_free: 0x%08x not found\n", addr);
		return -1;
	}

	/* find previous free block */
	page			= found->page;
	pageno			= page->pageno;
	free_page_size		= page->alloc_pages;
	prev_free_pageno	= pageno - 1;
	prev_size		= -1;

	if (prev_free_pageno >= 0) {
		if (mm->page_list[prev_free_pageno].used == 0) {
			prev_free_pageno	= mm->page_list[prev_free_pageno].first_pageno;
			prev_size		= mm->page_list[prev_free_pageno].alloc_pages;
		}
	}

	/* find next free block */
	next_free_pageno	= pageno + page->alloc_pages;
	next_free_pageno	= (next_free_pageno == mm->num_pages) ? -1 : next_free_pageno;
	next_size		= -1;

	if (next_free_pageno >= 0) {
		if (mm->page_list[next_free_pageno].used == 0) {
			next_size		= mm->page_list[next_free_pageno].alloc_pages;
		}
	}

	P_FREE(found);

	/* merge */
	merge_page_no = page->pageno;
	merge_page_size = page->alloc_pages;

	if (prev_size >= 0) {
		mm->free_tree = avltree_remove(mm->free_tree, &found, MAKE_KEY(prev_size, prev_free_pageno));

		if (!found) {
			assert(0);
			return -1;
		}

		merge_page_no = found->page->pageno;
		merge_page_size += found->page->alloc_pages;
		P_FREE(found);
	}

	if (next_size >= 0) {
		mm->free_tree = avltree_remove(mm->free_tree, &found, MAKE_KEY(next_size, next_free_pageno));

		if (!found) {
			assert(0);
			return -1;
		}

		merge_page_size	+= found->page->alloc_pages;
		P_FREE(found);
	}

	page->alloc_pages	= 0;
	page->first_pageno	= -1;

	set_blocks_free(mm, merge_page_no, merge_page_size);

	mm->alloc_page_count	-= free_page_size;
	mm->free_page_count	+= free_page_size;

	return 0;
}

int jmem_get_info(jpeg_mm_t *mm, jmem_info_t *info)
{
	if (!mm) {
		//		DPRINT("vmem_get_info: invalid handle\n");
		return -1;
	}

	if (!info) {
		return -1;
	}

	info->total_pages	= mm->num_pages;
	info->alloc_pages	= mm->alloc_page_count;
	info->free_pages	= mm->free_page_count;
	info->page_size		= VMEM_PAGE_SIZE;

	return 0;
}
