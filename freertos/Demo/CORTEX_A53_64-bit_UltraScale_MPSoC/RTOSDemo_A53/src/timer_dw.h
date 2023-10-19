#ifndef __DW_TIMER_H
#define __DW_TIMER_H

#include <stdint.h>
#include <platform_def.h>

#define NS_PER_TICK		(1000000000UL / PLAT_DW_TIMER_CLK_HZ)
#define PRELOAD_CNT		0xFFFFFFFF

#define TIMER_ID1		0x0
#define TIMER_ID2		0x1
#define TIMER_ID3		0x2
#define TIMER_ID4		0x3
#define TIMER_ID5		0x4
#define TIMER_ID6		0x5
#define TIMER_ID7		0x6
#define TIMER_ID8		0x7

#define REG_TIMER_BASE					0x030A0000

#define REG_TIMER1_BASE					(REG_TIMER_BASE + 0x00)
#define REG_TIMERS_INTSTATUS			(REG_TIMER_BASE + 0xA0)
#define REG_TIMERS_EOI					(REG_TIMER_BASE + 0xA4)
#define REG_TIMERS_RAW_INTSTATUS		(REG_TIMER_BASE + 0xA8)
#define REG_TIMERS_COMP_VERSION			(REG_TIMER_BASE + 0xAC)
#define REG_TIMERN_LOADCNT2_BASE		(REG_TIMER_BASE + 0xB0)

#define REG_TIMER1_LOADCNT				REG_TIMER1_BASE
#define REG_TIMER1_LOADCNT2				(REG_TIMERN_LOADCNT2_BASE + 0x00)
#define REG_TIMER1_CURRENT_VALUE		(REG_TIMER1_BASE + 0x04)
#define REG_TIMER1_CONTROL				(REG_TIMER1_BASE + 0x08)
#define REG_TIMER1_EOI					(REG_TIMER1_BASE + 0x0C)
#define REG_TIMER1_INTSTATUS			(REG_TIMER1_BASE + 0x10)

void dw_timer_start(uint32_t timer_id);
void dw_timer_stop(uint32_t timer_id, uint32_t *write_addr);

#endif /* __DW_TIMER_H */
