#ifndef __CPU_H__
#define __CPU_H__

#if defined(AARCH64)
#include <aarch64/cpu_helper.h>
#elif defined(RISCV)
#include <riscv/cpu_helper.h>
#else
#error "Unknown CPU type"
#endif

#ifndef __ASSEMBLY__

struct cache_map {
	uint64_t ro_start;
	uint32_t ro_size;

	uint64_t rw_start;
	uint32_t rw_size;

	uint64_t io_start;
	uint32_t io_size;
};

void enable_cache(struct cache_map *map);
void sync_cache(void);
void cpu_report_exception(unsigned int exception_type);

void jump_to_monitor(uintptr_t monitor_entry, uintptr_t next_addr);
void jump_to_loader_2nd(uintptr_t loader_2nd_entry);

#endif /* __ASSEMBLY__ */

#endif /* __CPU_H__ */
