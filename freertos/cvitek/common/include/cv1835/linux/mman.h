#include "xil_types.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/sys/types.h"
#include "mman-common.h"

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
int munmap(void *start, size_t length);

struct vm_area_struct {};
