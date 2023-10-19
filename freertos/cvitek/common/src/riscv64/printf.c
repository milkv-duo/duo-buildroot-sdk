/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//#include <stdio.h>
#include <stdarg.h>
#include <time.h>

/* Choose max of 128 chars for now. */
#define PRINT_BUFFER_SIZE 128

int printf(const char *fmt, ...)
{
	va_list args;
	char buf[PRINT_BUFFER_SIZE];
	int count;
	int pos;
	long sec, usec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec;
	usec = tv.tv_usec;
	pos = snprintf(buf, sizeof(buf) - 1, "[%d.%6d]", sec, usec);

	va_start(args, fmt);
	vsnprintf(buf + pos, sizeof(buf) - 1 - pos, fmt, args);
	va_end(args);

	/* Use putchar directly as 'puts()' adds a newline. */
	buf[PRINT_BUFFER_SIZE - 1] = '\0';
	count = uart_put_buff(buf);
	return (count + pos);
}
