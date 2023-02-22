/*
 * Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <delay_timer.h>
#include <platform.h>
#include <mmio.h>
#include "csr.h"

#define SYS_COUNTER_FREQ_IN_US (SYS_COUNTER_FREQ_IN_SECOND / 1000000)

static uint32_t get_timer_value(void)
{
	/*
	 * Generic delay timer implementation expects the timer to be a down
	 * counter. We apply bitwise NOT operator to the tick values returned
	 * by read_cntpct_el0() to simulate the down counter. The value is
	 * clipped from 64 to 32 bits.
	 */
	return (uint32_t)(~read_csr(time));
}

void trig_simulation_timer(uint32_t usec)
{
	uint32_t total_delta = (usec * SYS_COUNTER_FREQ_IN_US);

	// Trig simulation bench to increse cntpct_el0
	mmio_write_32(REG_GP_REG2, total_delta);
}

/***********************************************************
 * Delay for the given number of microseconds. The driver must
 * be initialized before calling this function.
 ***********************************************************/
void udelay(uint32_t usec)
{
	uint32_t start, delta, total_delta;

	start = get_timer_value();

	total_delta = (usec * SYS_COUNTER_FREQ_IN_US);

	// Trig simulation bench to increse cntpct_el0
	mmio_write_32(REG_GP_REG2, total_delta);

	do {
		/*
		 * If the timer value wraps around, the subtraction will
		 * overflow and it will still give the correct result.
		 */
		delta = start - get_timer_value(); /* Decreasing counter */

	} while (delta < total_delta);
}

/***********************************************************
 * Delay for the given number of milliseconds. The driver must
 * be initialized before calling this function.
 ***********************************************************/
void mdelay(uint32_t msec)
{
	udelay(msec * 1000);
}

uint32_t get_timer(uint32_t base)
{
	if (base == 0)
		return get_timer_value();
	else
		return (base - get_timer_value()) / SYS_COUNTER_FREQ_IN_US / 1000; // ms
}

uint32_t get_random_from_timer(uint32_t base)
{
	if (base == 0)
		return get_timer_value();
	else
		return (base - get_timer_value()) / SYS_COUNTER_FREQ_IN_US % 100000;
}

void timer_init(void)
{
//	write_cntfrq_el0(SYS_COUNTER_FREQ_IN_SECOND);
}

uint32_t read_count_tick(void)
{
	return read_csr(time) / SYS_COUNTER_FREQ_IN_US;
//	return read_cntpct_el0() / SYS_COUNTER_FREQ_IN_US;
}
