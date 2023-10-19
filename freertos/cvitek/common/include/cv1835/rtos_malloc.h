#ifndef  _MEM_HEADER_
#define  _MEM_HEADER_
//#include <linux/types.h>
#include <stdbool.h>
#include <stddef.h>

#define CACHE_LINE        0x40

struct header {
    unsigned int size;
    struct header *next_vpa;
	char *heap_ptr_vpa;
   	struct header *next;
	char *heap_ptr;
} __attribute__ ((aligned (CACHE_LINE)));

typedef struct shm_malloc_t shm_malloc_t;
typedef struct shm_memcpy_t shm_memcpy_t;
typedef struct header Header;

struct shm_malloc_t{
    union{
        size_t size;
        size_t shm_va_addr;
    };
};

struct shm_memcpy_t{
    size_t size;
    size_t shm_va_addr;
    void *mem_buf;
};

void memory_init(void *prt);
void *memory_alloc(size_t bytes);
void memory_free(void* ptr);
void memory_set(char *ptr, char value, size_t size);
#endif // _MEM_HEADER_
