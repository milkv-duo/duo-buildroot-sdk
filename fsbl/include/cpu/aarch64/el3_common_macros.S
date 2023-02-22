/*
 * Copyright (c) 2015-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __EL3_COMMON_MACROS_S__
#define __EL3_COMMON_MACROS_S__

#include <cpu.h>
#include <aarch64/asm_macros.S>

	/*
	 * Helper macro to initialise EL3 registers we care about.
	 */
	.macro el3_arch_init_common _exception_vectors
	/* ---------------------------------------------------------------------
	 * SCTLR_EL3 has already been initialised - read current value before
	 * modifying.
	 *
	 * SCTLR_EL3.I: Enable the instruction cache.
	 *
	 * SCTLR_EL3.SA: Enable Stack Aligment check. A SP alignment fault
	 *  exception is generated if a load or store instruction executed at
	 *  EL3 uses the SP as the base address and the SP is not aligned to a
	 *  16-byte boundary.
	 *
	 * SCTLR_EL3.A: Enable Alignment fault checking. All instructions that
	 *  load or store one or more registers have an alignment check that the
	 *  address being accessed is aligned to the size of the data element(s)
	 *  being accessed.
	 * ---------------------------------------------------------------------
	 */
	mov	x1, #(SCTLR_I_BIT | SCTLR_A_BIT | SCTLR_SA_BIT)
	mrs	x0, sctlr_el3
	orr	x0, x0, x1
	msr	sctlr_el3, x0
	isb

	/* ---------------------------------------------------------------------
	 * Set the exception vectors.
	 * ---------------------------------------------------------------------
	 */
	adr	x0, \_exception_vectors
	msr	vbar_el3, x0
	isb

	/* ---------------------------------------------------------------------
	 * Initialise SCR_EL3, setting all fields rather than relying on hw.
	 * All fields are architecturally UNKNOWN on reset. The following fields
	 * do not change during the TF lifetime. The remaining fields are set to
	 * zero here but are updated ahead of transitioning to a lower EL in the
	 * function cm_init_context_common().
	 *
	 * SCR_EL3.TWE: Set to zero so that execution of WFE instructions at
	 *  EL2, EL1 and EL0 are not trapped to EL3.
	 *
	 * SCR_EL3.TWI: Set to zero so that execution of WFI instructions at
	 *  EL2, EL1 and EL0 are not trapped to EL3.
	 *
	 * SCR_EL3.SIF: Set to one to disable instruction fetches from
	 *  Non-secure memory.
	 *
	 * SCR_EL3.SMD: Set to zero to enable SMC calls at EL1 and above, from
	 *  both Security states and both Execution states.
	 *
	 * SCR_EL3.EA: Set to one to route External Aborts and SError Interrupts
	 *  to EL3 when executing at any EL.
	 * ---------------------------------------------------------------------
	 */
	mov	x0, #((SCR_RESET_VAL | SCR_EA_BIT | SCR_SIF_BIT) \
			& ~(SCR_TWE_BIT | SCR_TWI_BIT | SCR_SMD_BIT))
	msr	scr_el3, x0

	/* ---------------------------------------------------------------------
	 * Initialise MDCR_EL3, setting all fields rather than relying on hw.
	 * Some fields are architecturally UNKNOWN on reset.
	 *
	 * MDCR_EL3.SDD: Set to one to disable AArch64 Secure self-hosted debug.
	 *  Debug exceptions, other than Breakpoint Instruction exceptions, are
	 *  disabled from all ELs in Secure state.
	 *
	 * MDCR_EL3.SPD32: Set to 0b10 to disable AArch32 Secure self-hosted
	 *  privileged debug from S-EL1.
	 *
	 * MDCR_EL3.NSPB (ARM v8.2): SPE enabled in non-secure state and
	 * disabled in secure state. Accesses to SPE registers at SEL1 generate
	 * trap exceptions to EL3.
	 *
	 * MDCR_EL3.TDOSA: Set to zero so that EL2 and EL2 System register
	 *  access to the powerdown debug registers do not trap to EL3.
	 *
	 * MDCR_EL3.TDA: Set to zero to allow EL0, EL1 and EL2 access to the
	 *  debug registers, other than those registers that are controlled by
	 *  MDCR_EL3.TDOSA.
	 *
	 * MDCR_EL3.TPM: Set to zero so that EL0, EL1, and EL2 System register
	 *  accesses to all Performance Monitors registers do not trap to EL3.
	 * ---------------------------------------------------------------------
	 */
	mov_imm	x0, ((MDCR_EL3_RESET_VAL | MDCR_SDD_BIT | MDCR_SPD32(MDCR_SPD32_DISABLE)) \
			& ~(MDCR_TDOSA_BIT | MDCR_TDA_BIT | MDCR_TPM_BIT))

	msr	mdcr_el3, x0

	/* ---------------------------------------------------------------------
	 * Enable External Aborts and SError Interrupts now that the exception
	 * vectors have been setup.
	 * ---------------------------------------------------------------------
	 */
	msr	daifclr, #DAIF_ABT_BIT

	/* ---------------------------------------------------------------------
	 * Initialise CPTR_EL3, setting all fields rather than relying on hw.
	 * All fields are architecturally UNKNOWN on reset.
	 *
	 * CPTR_EL3.TCPAC: Set to zero so that any accesses to CPACR_EL1,
	 *  CPTR_EL2, CPACR, or HCPTR do not trap to EL3.
	 *
	 * CPTR_EL3.TTA: Set to zero so that System register accesses to the
	 *  trace registers do not trap to EL3.
	 *
	 * CPTR_EL3.TFP: Set to zero so that accesses to Advanced SIMD and
	 *  floating-point functionality do not trap to EL3.
	 * ---------------------------------------------------------------------
	 */
	mov_imm x0, (CPTR_EL3_RESET_VAL & ~(TCPAC_BIT | TTA_BIT | TFP_BIT))
	msr	cptr_el3, x0
	.endm
#endif /* __EL3_COMMON_MACROS_S__ */
