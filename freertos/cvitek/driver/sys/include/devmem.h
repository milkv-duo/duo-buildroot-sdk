#ifndef MODULES_COMMON_INCLUDE_DEVMEM_H_
#define MODULES_COMMON_INCLUDE_DEVMEM_H_
#include "xil_types.h"
//#include <stdint.h>

int devm_open(void);
int devm_open_cached(void);
void devm_close(int fd);
void *devm_map(int fd, uint64_t addr, size_t len);
void devm_unmap(void *virt_addr, size_t len);

#endif // MODULES_COMMON_INCLUDE_DEVMEM_H_
