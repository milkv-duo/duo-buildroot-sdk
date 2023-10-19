#ifdef ENV_CVITEST
#include <common.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "system_common.h"
#include "timer.h"
#elif defined(ENV_EMU)
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "emu/command.h"
#endif  // ENV_CVITEST

#include "linux/spinlock.h"
#include "linux/file.h"
#include "scaler_reg.h"
#include "dwa_reg.h"
#include "reg.h"

#ifdef ENV_CVITEST
void _reg_write_mask(uintptr_t addr, u32 mask, u32 data)
{
	u32 value;

	value = _reg_read(addr) & ~mask;
	value |= (data & mask);
	_reg_write(addr, value);
}
#elif defined(ENV_EMU)
static u32 regs_dwa[0x100];
static u32 regs_cmdq[0x30];
static u32 regs_vip[0x10];
static u32 regs_scl[0x10000];

u32 _reg_read(uintptr_t addr)
{
	if (addr > REG_VIP_SYS_BASE)
		return regs_vip[(addr - REG_VIP_SYS_BASE) >> 2];
	if (addr > REG_DWA_CMDQ_BASE)
		return regs_cmdq[(addr - REG_DWA_CMDQ_BASE) >> 2];
	if (addr > REG_DWA_BASE)
		return regs_dwa[(addr - REG_DWA_BASE) >> 2];
	if (addr > REG_SCL_TOP_BASE)
		return regs_scl[(addr - REG_SCL_TOP_BASE) >> 2];

	return 0;
}

void _reg_write(uintptr_t addr, u32 data)
{
	if (addr > REG_VIP_SYS_BASE)
		regs_vip[(addr - REG_VIP_SYS_BASE) >> 2] = data;
	else if (addr > REG_DWA_CMDQ_BASE)
		regs_cmdq[(addr - REG_DWA_CMDQ_BASE) >> 2] = data;
	else if (addr > REG_DWA_BASE)
		regs_dwa[(addr - REG_DWA_BASE) >> 2] = data;
	else if (addr > REG_SCL_TOP_BASE)
		regs_scl[(addr - REG_SCL_TOP_BASE) >> 2] = data;
}

void _reg_write_mask(uintptr_t addr, u32 mask, u32 data)
{
	u32 value;

	value = _reg_read(addr) & ~mask;
	value |= (data & mask);
	_reg_write(addr, value);
}
#else
static DEFINE_RAW_SPINLOCK(__io_lock);

void _reg_write_mask(uintptr_t addr, u32 mask, u32 data)
{
	unsigned long flags;
	u32 value;

	raw_spin_lock_irqsave(&__io_lock, flags);
	value = readl_relaxed((void __iomem *)addr) & ~mask;
	value |= (data & mask);
	writel(value, (void __iomem *)addr);
	raw_spin_unlock_irqrestore(&__io_lock, flags);
}
#endif
