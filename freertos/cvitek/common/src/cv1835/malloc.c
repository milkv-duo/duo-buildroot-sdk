#include <string.h>
#include <malloc.h>
#include <linux/types.h>
#include <linux/device.h>

uintptr_t *calloc(size_t nelem, size_t size)
{
    uintptr_t *ptr;
    if (nelem == 0 || size == 0)
        return NULL;
    ptr = malloc(nelem * size);
    if (ptr)
        memset(ptr, 0, nelem * size);
    return ptr;
}

void __iomem *devm_ioremap_nocache(struct device *dev, resource_size_t offset,
				   resource_size_t size)
{
	//TODO
	dev = dev;
	size = size;
	return offset;
}
