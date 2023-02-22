/*
 * Copyright (c) 2014-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <assert.h>
#include <debug.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>

#include <delay_timer.h>


/***********************************************************
 * The tf_printf implementation for all BL stages
 ***********************************************************/

#define get_num_va_args(args, lcount) \
	(((lcount) > 1) ? va_arg(args, long long int) :	\
	((lcount) ? va_arg(args, long int) : va_arg(args, int)))

#define get_unum_va_args(args, lcount) \
	(((lcount) > 1) ? va_arg(args, unsigned long long int) :	\
	((lcount) ? va_arg(args, unsigned long int) : va_arg(args, unsigned int)))

static void string_print(const char *str, int level)
{
	while (*str) {
		putchar_l(*str++, level);
	}
}

static void unsigned_num_print(unsigned long long unum, unsigned int radix, int level, int zeropad)
{
	/* Just need enough space to store 64 bit decimal integer */
	unsigned char num_buf[20];
	int i = 0, rem;

	do {
		rem = unum % radix;
		if (rem < 0xa)
			num_buf[i++] = '0' + rem;
		else
			num_buf[i++] = 'a' + (rem - 0xa);
	} while (unum /= radix);

	zeropad -= i;
	while (zeropad-- > 0) {
		putchar_l('0', level);
	}

	while (--i >= 0) {
		putchar_l(num_buf[i], level);
	}
}

#if PRINTF_TIMESTAMP
void unsigned_hex_print(unsigned long long _unum, int level)
{
	extern char const hex2ascii_data[36];

	uint32_t unum = _unum;

	putchar_l('0', level);
	putchar_l('x', level);

#define __PUT_HEX(n, i) putchar_l(hex2ascii_data[(n >> (i * 4)) & 0xf], level)

	__PUT_HEX(unum, 7);
	__PUT_HEX(unum, 6);
	__PUT_HEX(unum, 5);
	__PUT_HEX(unum, 4);
	__PUT_HEX(unum, 3);
	__PUT_HEX(unum, 2);
	__PUT_HEX(unum, 1);
	__PUT_HEX(unum, 0);
#undef __PUT_HEX
}
#endif

/*******************************************************************
 * Reduced format print for Trusted firmware.
 * The following type specifiers are supported by this print
 * %x - hexadecimal format
 * %s - string format
 * %d or %i - signed decimal format
 * %u - unsigned decimal format
 * %p - pointer format
 *
 * The following length specifiers are supported by this print
 * %l - long int (64-bit on AArch64)
 * %ll - long long int (64-bit on AArch64)
 * %z - size_t sized integer formats (64 bit on AArch64)
 *
 * The print exits on all other formats specifiers other than valid
 * combinations of the above specifiers.
 *******************************************************************/
void tf_printf(const char *fmt, ...)
{
	va_list args;
	int zeropad = 0;
	int l_count;
	long long int num;
	unsigned long long int unum;
	char *str;

	int level = LOG_LEVEL;

#if PRINTF_TIMESTAMP
	const uint32_t ts = read_count_tick();
#endif

	if (fmt && fmt[0] == MSG_SOH_ASCII && fmt[1]) {
		switch (fmt[1]) {
		case '0' ... '4':
			level = fmt[1] - '0';
			fmt += 2;
		}
	}

	level = level;

	va_start(args, fmt);
	while (*fmt) {
		zeropad = 0;
		l_count = 0;

		if (*fmt == '%') {
			fmt++;
			/* Check the format specifier */
loop:
			switch (*fmt) {
			case '0':
				fmt++;
				if ((*fmt >= '0') && (*fmt <= '9'))
					zeropad = *fmt - '0';
				fmt++;
				goto loop;
			case 'i': /* Fall through to next one */
			case 'd':
				num = get_num_va_args(args, l_count);
				if (num < 0) {
					putchar_l('-', level);
					unum = (unsigned long long int)-num;
				} else
					unum = (unsigned long long int)num;
				unsigned_num_print(unum, 10, level, zeropad);
				break;
			case 's':
				str = va_arg(args, char *);
				string_print(str, level);
				break;
			case 'p':
				unum = (uintptr_t)va_arg(args, void *);
				if (unum)
					string_print("0x", level);
				unsigned_num_print(unum, 16, level, zeropad);
				break;
			case 'x':
				unum = get_unum_va_args(args, l_count);
				unsigned_num_print(unum, 16, level, zeropad);
				break;
			case 'z':
				if (sizeof(size_t) == 8)
					l_count = 2;

				fmt++;
				goto loop;
			case 'l':
				l_count++;
				fmt++;
				goto loop;
			case 'u':
				unum = get_unum_va_args(args, l_count);
				unsigned_num_print(unum, 10, level, zeropad);
				break;
			default:
				/* Exit on any other format specifier */
				goto exit;
			}
			fmt++;
			continue;
		}

#if PRINTF_TIMESTAMP
		if (*fmt == '\n') {
			putchar_l('\t', level);
			putchar_l('{', level);
			unsigned_num_print(ts, 10, level, zeropad);
			// unsigned_hex_print(ts, level);
			putchar_l('}', level);
		}
#endif

		putchar_l(*fmt++, level);
	}
exit:
	va_end(args);
}

#define MAX_COLUMNS 16
void tf_print_buffer(const void *buf, size_t size, const char *name)
{
	const unsigned char *p = buf;
	int i;

	if (!name)
		name = "buf";

	tf_printf("%s@%p(%lu):\n", name, buf, size);

	for (i = 0; i < size; i++) {
		tf_printf("%x ", p[i]);
		if (i % MAX_COLUMNS == MAX_COLUMNS - 1)
			tf_printf("\n");
	}
	tf_printf("---\n");
}
