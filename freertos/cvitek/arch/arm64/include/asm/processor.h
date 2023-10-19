#ifndef __ASM_PROCESSOR_H
#define __ASM_PROCESSOR_H


static inline void cpu_relax(void)
{
	asm volatile("yield" ::: "memory");
}

#define cpu_relax_lowlatency()                cpu_relax()

#endif
