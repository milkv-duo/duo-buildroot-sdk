#ifndef __ARCH_HELPERS_H__
#define __ARCH_HELPERS_H__
//#include <cpu.h>    /* for additional register definitions */
#include <cdefs.h>  /* For __dead2 */
#include <stdint.h>
#include <sys/types.h>

void flush_dcache_range(uintptr_t addr, size_t size);
void clean_dcache_range(uintptr_t addr, size_t size);
void inv_dcache_range(uintptr_t addr, size_t size);
void enable_dcache(void);
void disable_dcache(void);

#endif /* __ARCH_HELPERS_H__ */
