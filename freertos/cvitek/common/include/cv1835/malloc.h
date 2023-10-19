#ifndef __MALLOC_H___
#define __MALLOC_H__
#include <linux/types.h>
#include "mmu.h"

/* here use freertos malloc & free function*/
extern void *pvPortMalloc(size_t xWantedSize);
extern void vPortFree(void *pv);
struct device;

/*
 * Replace the normal allocation functions with the versions from
 * <linux/decompress/mm.h>. vfree() needs to support vfree(NULL)
 */
#undef malloc
#undef free
#undef kmalloc
#undef kfree
#undef vmalloc
#undef vfree
#define malloc(size) pvPortMalloc(size)
#define free(ptr) vPortFree(ptr)
#define kmalloc(size, flags) pvPortMalloc(size)
#define kcalloc(nelem, size, flags) calloc(nelem, size)
#define kzalloc(size, flags) calloc(1, size)
#define devm_kzalloc(nelem, size, flags) calloc(1, size)
#define kfree(ptr) vPortFree(ptr)
#define vmalloc(size) pvPortMalloc(size)
#define vfree(ptr)				\
	do {                        \
		if (ptr != NULL)        \
			vPortFree(ptr);     \
	} while (0)

uintptr_t *calloc(size_t nelem, size_t size);


/* copy from linux/dma-direction.h */
enum dma_data_direction {
        DMA_BIDIRECTIONAL = 0,
        DMA_TO_DEVICE = 1,
        DMA_FROM_DEVICE = 2,
        DMA_NONE = 3,
};

/* from linux/io.h */
extern void __iomem *devm_ioremap_nocache(struct device *dev, resource_size_t offset,
				   resource_size_t size);

/* align addr on a size boundary - adjust address up/down if needed */
#define _ALIGN_UP(addr, size)	(((addr)+((size)-1))&(~((typeof(addr))(size)-1)))
#define _ALIGN_DOWN(addr, size)	((addr)&(~((typeof(addr))(size)-1)))

/* align addr on a size boundary - adjust address up if needed */
#define _ALIGN(addr,size)     _ALIGN_UP(addr,size)

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)	_ALIGN(addr, PAGE_SIZE)

#endif // end of __MALLOC_H__
