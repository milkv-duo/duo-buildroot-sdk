#include <mmio.h>
#include <debug.h>
#include <assert.h>
#include <bl_common.h>
#include <platform.h>
#include <cpu.h>
#include "csr.h"

#include <arch_helpers.h>

void sync_cache(void)
{
	asm volatile(
			"icache.iall\n"
			"sync.i\n"
			:
			:
			: "memory");
}

void cpu_report_exception(unsigned int exception_type)
{
}

void enable_cache(struct cache_map *map)
{
}
