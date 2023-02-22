#ifndef _CVI_REG_H_
#define _CVI_REG_H_

#include <asm/io.h>

#define _reg_read(addr) readl((void __iomem *)addr)
#define _reg_write(addr, data) writel(data, (void __iomem *)addr)

void _reg_write_mask(uintptr_t addr, u32 mask, u32 data);

#endif //_CVI_REG_H_
