#include <string.h>
#include <stdint.h>
#include <malloc.h>

void *calloc(size_t nelem, size_t size)
{
    uintptr_t *ptr;
    if (nelem == 0 || size == 0)
        return NULL;
    ptr = malloc(nelem * size);
    if (ptr)
        memset(ptr, 0, nelem * size);
    return ptr;
}
