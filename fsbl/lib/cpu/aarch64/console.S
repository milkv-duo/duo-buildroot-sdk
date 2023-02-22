/*
 * Copyright (c) 2015-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <aarch64/asm_macros.S>
#include <aarch64/el3_common_macros.S>
#include <debug.h>

	.globl	console_init
	.globl	console_putc
	.globl	console_getc
	.globl	console_flush
	.globl	console_tstc

	/* -----------------------------------------------
	 * int console_init(uintptr_t not_used,
	 * unsigned int uart_clk, unsigned int baud_rate)
	 * Function to initialize the console without a
	 * C Runtime to print debug information. It saves
	 * the console base to the data section.
	 * In: x0 - not used
	 *     w1 - Uart clock in Hz
	 *     w2 - Baud rate
	 * out: return 1 on success else 0 on error
	 * Clobber list : x1 - x4
	 * -----------------------------------------------
	 */
func console_init
	mov_imm x0, PLAT_BOOT_UART_BASE
	b	console_core_init
	ret
endfunc console_init

	/* ---------------------------------------------
	 * int console_putc(int c)
	 * Function to output a character over the
	 * console. It returns the character printed on
	 * success or -1 on error.
	 * In : x0 - character to be printed
	 * Out : return -1 on error else return character.
	 * Clobber list : x1, x2
	 * ---------------------------------------------
	 */
func console_putc
	mov_imm x1, PLAT_BOOT_UART_BASE
	b	console_core_putc
endfunc console_putc

	/* ---------------------------------------------
	 * int console_getc(void)
	 * Function to get a character from the console.
	 * It returns the character grabbed on success
	 * or -1 on error.
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
func console_getc
	mov_imm x0, PLAT_BOOT_UART_BASE
	b	console_core_getc
endfunc console_getc

func console_tstc
	mov_imm x0, PLAT_BOOT_UART_BASE
	b	console_core_tstc
endfunc console_tstc

	/* ---------------------------------------------
	 * int console_flush(void)
	 * Function to force a write of all buffered
	 * data that hasn't been output. It returns 0
	 * upon successful completion, otherwise it
	 * returns -1.
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
func console_flush
	mov_imm x0, PLAT_BOOT_UART_BASE
	b	console_core_flush
endfunc console_flush
