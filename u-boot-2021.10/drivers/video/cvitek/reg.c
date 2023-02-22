#include <common.h>
#include <asm/io.h>

#include "scaler_reg.h"
#include "reg.h"

void _reg_write_mask(uintptr_t addr, u32 mask, u32 data)
{
	u32 value;

	value = readl((void __iomem *)addr) & ~mask;
	value |= (data & mask);
	writel(value, (void __iomem *)addr);
}
