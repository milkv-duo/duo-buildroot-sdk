/*
 * Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __DELAY_TIMER_H__
#define __DELAY_TIMER_H__

#include <stdint.h>
#include <utils_def.h>

void mdelay(uint32_t msec);
void udelay(uint32_t usec);
void timer_init(void);
uint32_t get_timer(uint32_t base);
uint32_t get_random_from_timer(uint32_t base);
uint32_t read_count_tick(void);
void trig_simulation_timer(uint32_t usec);

static inline uint32_t read_time_ms(void)
{
	return DIV_ROUND_UP(read_count_tick(), 1000);
}

#endif /* __DELAY_TIMER_H__ */
