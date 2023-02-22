/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <console.h>
#include <assert.h>
#include <debug.h>

#include "platform.h"

/* Putchar() should either return the character printed or EOF in case of error.
 * Our current console_putc() function assumes success and returns the
 * character. Write all other printing functions in terms of putchar(), if
 * possible, so they all benefit when this is improved.
 */
int putchar_l(int c, int level)
{
	uint32_t *const log_size = (void *)BOOT_LOG_LEN_ADDR;
#ifdef __riscv
	uint8_t *const log_buf = (void *)(phys_to_dma(BOOT_LOG_BUF_BASE) | SYSMAP_MIRROR_OFFSET);
#else
	uint8_t *const log_buf = (void *)phys_to_dma(BOOT_LOG_BUF_BASE);
#endif

	int res;

	if (*log_size >= BOOT_LOG_BUF_SIZE)
		*log_size = 0;

	log_buf[(*log_size)++] = c;

	if (level > LOG_LEVEL)
		return c;

	if (get_sw_info()->dis_uart_msg)
		return c;

	if (console_putc((unsigned char)c) >= 0)
		res = c;
	else
		res = EOF;

	return res;
}
