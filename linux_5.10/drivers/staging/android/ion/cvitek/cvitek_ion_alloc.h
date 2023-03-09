/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvitek_ion_alloc.h
 * Description:
 */

#ifndef __CVITEK_ION_ALLOC_H__
#define __CVITEK_ION_ALLOC_H__

#include "../../uapi/ion.h"

#define HEAP_QUERY_CNT	5

int cvi_ion_alloc(enum ion_heap_type type, size_t len, bool mmap_cache);
void cvi_ion_free(pid_t fd_pid, int fd);

struct ion_buffer *
cvi_ion_alloc_nofd(enum ion_heap_type type, size_t len, bool mmap_cache);
void cvi_ion_free_nofd(struct ion_buffer *buffer);

void cvi_ion_dump(struct ion_heap *heap);

#endif /* __CVITEK_ION_ALLOC_H__ */
