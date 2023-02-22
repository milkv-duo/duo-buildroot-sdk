/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cpu.h>
#include <aarch64/asm_macros.S>
#include <bl_common.h>
#include <debug.h>


	.globl	bl2_entrypoint
	.globl	bl2_entrypoint_real

func bl2_entrypoint
	b bl2_entrypoint_real
	.word 0 // resvered
	.word 0 // BL2 MSID
	.word 0 // BL2 version
	.word 0 //
	.word 0
	.word 0
	.word 0

bl2_entrypoint_real:
	atf_state_set w14, x15, ATF_STATE_BL2_ENTRY_POINT

	/* ---------------------------------------------
	 * Sync instruction cache
	 * ---------------------------------------------
	 */
	ic	iallu
	isb

	/* ---------------------------------------------
	 * Set the exception vector to something sane.
	 * ---------------------------------------------
	 */
	adr	x0, bl2_exceptions
	msr	vbar_el3, x0
	isb

	/* ---------------------------------------------
	 * Enable the SError interrupt now that the
	 * exception vectors have been setup.
	 * ---------------------------------------------
	 */
	msr	daifclr, #DAIF_ABT_BIT

	/* ---------------------------------------------
	 * Zero out NOBITS sections. There are 2 of them:
	 *   - the .bss section;
	 *   - the coherent memory section.
	 * ---------------------------------------------
	 */
	ldr	x0, =__BSS_START__
	ldr	x1, =__BSS_SIZE__
	bl	zeromem

	/* --------------------------------------------
	 * Allocate a stack whose memory will be marked
	 * as Normal-IS-WBWA when the MMU is enabled.
	 * There is no risk of reading stale stack
	 * memory after enabling the MMU as only the
	 * primary cpu is running at the moment.
	 * --------------------------------------------
	 */
	ldr     x0, =__STACKS_END__
        mov     sp,  x0

	/* ---------------------------------------------
	 * Jump to main function.
	 * ---------------------------------------------
	 */
	bl	bl2_main

	/* ---------------------------------------------
	 * Should never reach this point.
	 * ---------------------------------------------
	 */
	no_ret	panic_handler

endfunc bl2_entrypoint


	.global		call_with_eret_to_next
	.type		call_with_eret_to_next, %function

	.global		call_with_eret
	.type		call_with_eret, %function

call_with_eret_to_next:
	mov x9, x30
	b 1f
call_with_eret:
	mov x9, x0
1:
	msr elr_el3, x9

	/* Same exception vector */
	mrs x9, vbar_el3
	msr vbar_el2, x9

	eret
	b .

func disable_mmu_el3
	mov	x1, #(SCTLR_M_BIT | SCTLR_C_BIT)
do_disable_mmu:
	mrs	x0, sctlr_el3
	bic	x0, x0, x1
	msr	sctlr_el3, x0
	isb				// ensure MMU is off
	dsb	sy
	ret
endfunc disable_mmu_el3

func disable_mmu_icache_el3
	mov	x1, #(SCTLR_M_BIT | SCTLR_C_BIT | SCTLR_I_BIT)
	b	do_disable_mmu
endfunc disable_mmu_icache_el3

	.global jump_bl31
func jump_bl31
	mov	x20, x0 // bl31_entry
	mov	x21, x1 // bl31_params

 	bl	disable_mmu_icache_el3
 	tlbi	alle3

	mov	x0, x21 // bl31_params
	mov	x1, xzr
	blr	x20

endfunc jump_bl31
