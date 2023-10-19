/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//#include <stdio.h>
#include <stdarg.h>
#include <linux/spinlock.h>
#include "linux/file.h"

/* Choose max of 128 chars for now. */
#define PRINT_BUFFER_SIZE 128
DEFINE_RAW_SPINLOCK(printf_lock);
int printf(const char *fmt, ...)
{
	va_list args;
	char buf[PRINT_BUFFER_SIZE];
	int count;
	int flags;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf) - 1, fmt, args);
	va_end(args);

	/* Use putchar directly as 'puts()' adds a newline. */
	buf[PRINT_BUFFER_SIZE - 1] = '\0';
	count = 0;
	raw_spin_lock_irqsave(&printf_lock, flags);
	while (buf[count]) {
		if (uart_putc(buf[count]) != EOF) {
			//if (putchar(buf[count]) != EOF) {
			count++;
		} else {
			count = EOF;
			break;
		}
	}
	raw_spin_unlock_irqrestore(&printf_lock, flags);
	return count;
}
