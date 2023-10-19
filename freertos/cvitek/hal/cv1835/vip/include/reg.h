#ifndef _CVI_REG_H_
#define _CVI_REG_H_

#ifdef ENV_CVITEST
#define _reg_read(addr) readl((uintptr_t)addr)
#define _reg_write(addr, data) writel((uintptr_t)addr, data)
void _reg_write_mask(uintptr_t addr, u32 mask, u32 data);

#elif defined(ENV_EMU)
u32 _reg_read(uintptr_t addr);
void _reg_write(uintptr_t addr, u32 data);
void _reg_write_mask(uintptr_t addr, u32 mask, u32 data);

#else
#include "io.h"
extern int dump_reg;

#define _reg_read(addr) readl((void __iomem *)addr)

#if 1
#define _reg_write(addr, data) writel(data, (void __iomem *)addr)
#else
#define _reg_write(addr, data) \
	{ \
		writel(data, (void __iomem *)addr); \
		if (dump_reg) \
			pr_info("MWriteS32 %#x %#x\n", (u32)(addr), (u32)(data)); \
	}
#endif

void _reg_write_mask(uintptr_t addr, u32 mask, u32 data);
#endif

#endif //_CVI_REG_H_
