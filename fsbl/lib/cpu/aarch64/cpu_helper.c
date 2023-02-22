#include <mmio.h>
#include <debug.h>
#include <assert.h>
#include <bl_common.h>
#include <platform.h>
#include <cpu.h>
#include <utils.h>

void sync_cache(void)
{
	asm volatile("ic iallu\n"
		     "isb\n"
		     :
		     :
		     : "memory");
}

void cpu_report_exception(unsigned int exception_type)
{
}
