/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cpu.h>
#include <aarch64/el3_common_macros.S>
#include <debug.h>
#include <platform.h>


	.globl	bl1_entrypoint
	.globl	bl1_entrypoint_end
	.globl	bl1_entrypoint_rel

	/* -----------------------------------------------------
	 * bl1_entrypoint() is the entry point into the trusted
	 * firmware code when a cpu is released from warm or
	 * cold reset.
	 * -----------------------------------------------------
	 */
	.pushsection .entry, "ax"
bl1_entrypoint:
	# Relocate from 0x04400000 to mirrored ROM address 0x40000000
	ldr x0, =bl1_entrypoint_rel
	br x0
bl1_entrypoint_end:
	.popsection

func bl1_entrypoint_rel
	atf_state_set w14, x15, ATF_STATE_BL1_ENTRY_POINT
	atf_err_set w14, x15, ATF_ERR_NONE

	/*
	 * Initialise the register file as a safeguard against
	 * spurious X propagation in EDA verfication platform.
	 */
        mov     x0,  xzr
        mov     x1,  xzr
        mov     x2,  xzr
        mov     x3,  xzr
        mov     x4,  xzr
        mov     x5,  xzr
        mov     x6,  xzr
        mov     x7,  xzr
        mov     x8,  xzr
        mov     x9,  xzr
        mov     x10, xzr
        mov     x11, xzr
        mov     x12, xzr
        mov     x13, xzr
        mov     x14, xzr
        mov     x15, xzr
        mov     x16, xzr
        mov     x17, xzr
        mov     x18, xzr
        mov     x19, xzr
        mov     x20, xzr
        mov     x21, xzr
        mov     x22, xzr
        mov     x23, xzr
        mov     x24, xzr
        mov     x25, xzr
        mov     x26, xzr
        mov     x27, xzr
        mov     x28, xzr
        mov     x29, xzr
        mov     x30, xzr

	/* ---------------------------------------------------------------------
	 * Use SP_EL0 for the C runtime stack.
	 * ---------------------------------------------------------------------
	 */
	msr	spsel, #0
	ldr     x0, =__STACKS_END__
        mov     sp,  x0

	/* -------------------------------------------------------------
	* This is the initialisation of SCTLR_EL3 and so must ensure
	* that all fields are explicitly set rather than relying on hw.
	* Some fields reset to an IMPLEMENTATION DEFINED value and
	* others are architecturally UNKNOWN on reset.
	*
	* SCTLR.EE: Set the CPU endianness before doing anything that
	*  might involve memory reads or writes. Set to zero to select
	*  Little Endian.
	*
	* SCTLR_EL3.WXN: For the EL3 translation regime, this field can
	*  force all memory regions that are writeable to be treated as
	*  XN (Execute-never). Set to zero so that this control has no
	*  effect on memory access permissions.
	*
	* SCTLR_EL3.SA: Set to zero to disable Stack Aligment check.
	*
	* SCTLR_EL3.A: Set to zero to disable Alignment fault checking.
	* -------------------------------------------------------------
	*/
	mov_imm	x0, (SCTLR_RESET_VAL & ~(SCTLR_EE_BIT | SCTLR_WXN_BIT \
			| SCTLR_SA_BIT | SCTLR_A_BIT))
	msr	sctlr_el3, x0
	isb

	/* ---------------------------------------------------------------------
	 * It is a cold boot.
	 * ---------------------------------------------------------------------
	 */
	el3_arch_init_common bl1_exceptions

	/* NO C runtime environment. stack only. no .bss and .data */
	bl bl1_early_platform_setup


	/* --------------------------------------------------
	 * Initialize platform and jump to our c-entry point
	 * for this type of reset.
	 * --------------------------------------------------
	 */
	bl	bl1_main

	/* --------------------------------------------------
	 * Do the transition to next boot image.
	 * --------------------------------------------------
	 */
1:
	atf_err_set w14, x15, ATF_ERR_BL1_RETURN
	b 1b

endfunc bl1_entrypoint_rel
