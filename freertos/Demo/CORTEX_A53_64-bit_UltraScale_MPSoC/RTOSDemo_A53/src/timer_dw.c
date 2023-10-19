#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include "debug.h"
#include "mmio.h"
#include "timer_dw.h"

#define DBG_DW_TIMER

void dw_timer_start(uint32_t timer_id)
{
	// disable timer
	mmio_write_32(REG_TIMER1_CONTROL + timer_id * 0x14, 0x0);

	// set timer load count
	mmio_write_32(REG_TIMER1_LOADCNT + timer_id * 0x14, PRELOAD_CNT);

	// enable timer
	mmio_write_32(REG_TIMER1_CONTROL + timer_id * 0x14, 0x1);

	INFO("dw_timer_start\n");
}

void dw_timer_stop(uint32_t timer_id, uint32_t *write_addr)
{
	uint32_t time_end;
	uint32_t timer_interval_ms;

	time_end = mmio_read_32(REG_TIMER1_CURRENT_VALUE + timer_id * 0x14);
	INFO("BLD:  time_end=0x%x\n", time_end);

	// disable timer
	mmio_write_32(REG_TIMER1_CONTROL + timer_id * 0x14, 0x0);

	timer_interval_ms = (PRELOAD_CNT - time_end) * NS_PER_TICK / 1000000;

	INFO("timer ms = %d\n", timer_interval_ms);

	*write_addr = timer_interval_ms;
}
