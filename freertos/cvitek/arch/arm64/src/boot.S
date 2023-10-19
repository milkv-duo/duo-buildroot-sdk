/******************************************************************************
*
* Copyright (C) 2014 - 2018 Xilinx, Inc. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file boot.S
*
* @addtogroup a53_64_boot_code Cortex A53 64bit Processor Boot Code
* @{
* <h2> boot.S </h2>
*
* The boot code performs minimum configuration which is required for an
* application. Cortex-A53 starts by checking current exception level. If the
* current exception level is EL3 and BSP is built for EL3, it will do
* initialization required for application execution at EL3. Below is a
* sequence illustrating what all configuration is performed before control
* reaches to main function for EL3 execution.
*
* 1. Program vector table base for exception handling
* 2. Set reset vector table base address
* 3. Program stack pointer for EL3
* 4. Routing of interrupts to EL3
* 5. Enable ECC protection
* 6. Program generic counter frequency
* 7. Invalidate instruction cache, data cache and TLBs
* 8. Configure MMU registers and program base address of translation table
* 9. Transfer control to _start which clears BSS sections and runs global
*    constructor before jumping to main application
*
* If the current exception level is EL1 and BSP is also built for EL1_NONSECURE
* it will perform initialization required for application execution at EL1
* non-secure. For all other combination, the execution will go into infinite
* loop. Below is a sequence illustrating what all configuration is performed
* before control reaches to main function for EL1 execution.
*
* 1. Program vector table base for exception handling
* 2. Program stack pointer for EL1
* 3. Invalidate instruction cache, data cache and TLBs
* 4. Configure MMU registers and program base address of translation table
* 5. Transfer control to _start which clears BSS sections and runs global
*    constructor before jumping to main application
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------- -------- ---------------------------------------------------
* 5.00  pkp	05/21/14 Initial version
* 6.00	pkp     07/25/16 Program the counter frequency
* 6.02  pkp	01/22/17 Added support for EL1 non-secure
* 6.02	pkp	01/24/17 Clearing status of FPUStatus variable to ensure it
*			 holds correct value.
* 6.3   mus 04/20/17 CPU Cache protection bit in the L2CTLR_EL1 will be in
*                    set state on reset. So, setting that bit through boot
*                    code is redundant, hence removed the code which sets
*                    CPU cache protection bit.
* 6.4   mus      08/11/17 Implemented ARM erratum 855873.It fixes
*                         CR#982209.
* 6.6   mus      01/19/18 Added isb after writing to the cpacr_el1/cptr_el3,
*                         to ensure floating-point unit is disabled, before
*                         any subsequent instruction.
*
*
******************************************************************************/

#include "xparameters.h"
#include "bspconfig.h"

.globl MMUTableL1
.globl MMUTableL2
.global _prestart
.global _boot

.global __el3_stack
.global __el2_stack
.global __el1_stack
.global __el0_stack
.global _vector_table

.set EL3_stack,		__el3_stack
.set EL2_stack,		__el2_stack
.set EL1_stack,		__el1_stack
.set EL0_stack,		__el0_stack

.set TT_S1_FAULT,	0x0
.set TT_S1_TABLE,	0x3

.set L1Table,	MMUTableL1
.set L2Table,	MMUTableL2
.set vector_base,	_vector_table
.set rvbar_base,	0xFD5C0040

.set counterfreq,	XPAR_CPU_CORTEXA53_0_TIMESTAMP_CLK_FREQ
.set MODE_EL1, 0x5
.set DAIF_BIT,	0x1C0

.section .boot,"ax"


/* this initializes the various processor modes */

_prestart:
_boot:
	mov      x0, #0
	mov      x1, #0
	mov      x2, #0
	mov      x3, #0
	mov      x4, #0
	mov      x5, #0
	mov      x6, #0
	mov      x7, #0
	mov      x8, #0
	mov      x9, #0
	mov      x10, #0
	mov      x11, #0
	mov      x12, #0
	mov      x13, #0
	mov      x14, #0
	mov      x15, #0
	mov      x16, #0
	mov      x17, #0
	mov      x18, #0
	mov      x19, #0
	mov      x20, #0
	mov      x21, #0
	mov      x22, #0
	mov      x23, #0
	mov      x24, #0
	mov      x25, #0
	mov      x26, #0
	mov      x27, #0
	mov      x28, #0
	mov      x29, #0
	mov      x30, #0
#if 0 //dont put other a53 cpus in wfi
   //Which core am I
   // ----------------
	mrs      x0, MPIDR_EL1
	and      x0, x0, #0xFF                        //Mask off to leave Aff0
	cbz      x0, OKToRun                          //If core 0, run the primary init code
EndlessLoop0:
	wfi
	b        EndlessLoop0
#endif
	bl	el2_setup			// Drop to EL1, w0=cpu_boot_mode
OKToRun:

	mrs	x0, currentEL
	cmp	x0, #0xC
	beq	InitEL3

	cmp	x0, #0x4
	beq	InitEL1

	b 	error			// go to error if current exception level is neither EL3 nor EL1
InitEL3:
.if (EL3 == 1)
	/*Set vector table base address*/
	ldr	x1, =vector_base
	msr	VBAR_EL3,x1

	/* Set reset vector address */
	/* Get the cpu ID */
	mrs  x0, MPIDR_EL1
	and  x0, x0, #0xFF
	mov  w0, w0
	ldr	 w2, =rvbar_base
	/* calculate the rvbar base address for particular CPU core */
	mov	 w3, #0x8
	mul	 w0, w0, w3
	add	 w2, w2, w0
	/* store vector base address to RVBAR */
	str  x1, [x2]

	/*Define stack pointer for current exception level*/
	ldr	 x2,=EL3_stack
	mov	 sp,x2

	/* Enable Trapping of SIMD/FPU register for standalone BSP */
	mov      x0, #0
#ifndef FREERTOS_BSP
	orr      x0, x0, #(0x1 << 10)
#endif
	msr      CPTR_EL3, x0
	isb

	/*
	 * Clear FPUStatus variable to make sure that it contains current
	 * status of FPU i.e. disabled. In case of a warm restart execution
	 * when bss sections are not cleared, it may contain previously updated
	 * value which does not hold true now.
	 */
#ifndef FREERTOS_BSP
	 ldr x0,=FPUStatus
	 str xzr, [x0]
#endif
	/* Configure SCR_EL3 */
	mov      w1, #0              	//; Initial value of register is unknown
	orr      w1, w1, #(1 << 11)  	//; Set ST bit (Secure EL1 can access CNTPS_TVAL_EL1, CNTPS_CTL_EL1 & CNTPS_CVAL_EL1)
	orr      w1, w1, #(1 << 10)  	//; Set RW bit (EL1 is AArch64, as this is the Secure world)
	orr      w1, w1, #(1 << 3)   	//; Set EA bit (SError routed to EL3)
	orr      w1, w1, #(1 << 2)   	//; Set FIQ bit (FIQs routed to EL3)
	orr      w1, w1, #(1 << 1)   	//; Set IRQ bit (IRQs routed to EL3)
	msr      SCR_EL3, x1

	/*configure cpu auxiliary control register EL1 */
	ldr	x0,=0x80CA000 		// L1 Data prefetch control - 5, Enable device split throttle, 2 independent data prefetch streams
#if CONFIG_ARM_ERRATA_855873
        /*
	 *  Set ENDCCASCI bit in CPUACTLR_EL1 register, to execute data
	 *  cache clean operations as data cache clean and invalidate
	 *
	 */
        orr     x0, x0, #(1 << 44)      //; Set ENDCCASCI bit
#endif
	msr	S3_1_C15_C2_0, x0 	//CPUACTLR_EL1

	/* program the counter frequency */
	ldr	x0,=counterfreq
	msr	CNTFRQ_EL0, x0

	/*Enable hardware coherency between cores*/
	mrs      x0, S3_1_c15_c2_1  	//Read EL1 CPU Extended Control Register
	orr      x0, x0, #(1 << 6)  	//Set the SMPEN bit
	msr      S3_1_c15_c2_1, x0  	//Write EL1 CPU Extended Control Register
	isb

	tlbi 	ALLE3
	ic      IALLU                  	//; Invalidate I cache to PoU
	bl 	invalidate_dcaches
	dsb	 sy
	isb

//	ldr      x1, =L0Table 		//; Get address of level 0 for TTBR0_EL3
	msr      TTBR0_EL3, x1		//; Set TTBR0_EL3

	/**********************************************
	* Set up memory attributes
	* This equates to:
	* 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
	* 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
	* 2 = b00000000 = Device-nGnRnE
	* 3 = b00000100 = Device-nGnRE
	* 4 = b10111011 = Normal, Inner/Outer WT/WA/RA
	**********************************************/
	ldr      x1, =0x000000BB0400FF44
	msr      MAIR_EL3, x1

	/**********************************************
	 * Set up TCR_EL3
	 * Physical Address Size PS =  010 -> 40bits 1TB
	 * Granual Size TG0 = 00 -> 4KB
	 * size offset of the memory region T0SZ = 24 -> (region size 2^(64-24) = 2^40)
	 ***************************************************/
	ldr     x1,=0x80823518
	msr     TCR_EL3, x1
	isb

	/* Enable SError Exception for asynchronous abort */
	mrs 	x1,DAIF
	bic	x1,x1,#(0x1<<8)
	msr	DAIF,x1

	/* Configure SCTLR_EL3 */
	mov      x1, #0                //Most of the SCTLR_EL3 bits are unknown at reset
	orr      x1, x1, #(1 << 12)	//Enable I cache
	orr      x1, x1, #(1 << 3)	//Enable SP alignment check
	orr      x1, x1, #(1 << 2)	//Enable caches
	orr      x1, x1, #(1 << 0)	//Enable MMU
	msr      SCTLR_EL3, x1
	dsb	 sy
	isb

	b 	 _startup		//jump to start
.else
	b 	error			// present exception level and selected exception level mismatch
.endif

InitEL1:
.if (EL1_NONSECURE == 1)
	/*Set vector table base address*/
	ldr	x1, =vector_base
	msr	VBAR_EL1,x1

	/* Disable Trap floating point access. */
	mrs	x0, CPACR_EL1
	orr	x0, x0, #(0x3 << 20)
	msr	CPACR_EL1, x0
	isb

	#disable MMU & I/D cache
	mrs     x0, SCTLR_EL1
	# mmu
	bic     x0, x0, #(1<<0)
	# dcache
	bic     x0, x0, #(1<<2)
	# icache
	bic     x0, x0, #(1<<12)
	msr     SCTLR_EL1, x0
	isb

	bl __asm_invalidate_tlb_all
	bl __asm_invalidate_icache_all
	bl __asm_invalidate_dcache_all

	/* enable i/d cache ASAP*/
	mrs     x0, SCTLR_EL1
	orr     x0, x0, #(1<<12)
	orr     x0, x0, #(1<<2)
	msr     SCTLR_EL1, x0
	isb

	/*
	 * Clear FPUStatus variable to make sure that it contains current
	 * status of FPU i.e. disabled. In case of a warm restart execution
	 * when bss sections are not cleared, it may contain previously updated
	 * value which does not hold true now.
	 */
#ifndef FREERTOS_BSP
	 ldr x0,=FPUStatus
	 str xzr, [x0]
#endif
	/*Define stack pointer for current exception level*/
	ldr	 x2,=EL1_stack
	mov	 sp,x2

	/* Enable SError Exception for asynchronous abort */
	mrs 	x1,DAIF
	bic	x1,x1,#(0x1<<8)
	msr	DAIF,x1

	bl 	 _startup		//jump to start
	b	error
.else
	b 	error			// present exception level and selected exception level mismatch
.endif

error: 	b	error


invalidate_dcaches:

	dmb     ISH
	mrs     x0, CLIDR_EL1          //; x0 = CLIDR
	ubfx    w2, w0, #24, #3        //; w2 = CLIDR.LoC
	cmp     w2, #0                 //; LoC is 0?
	b.eq    invalidateCaches_end   //; No cleaning required and enable MMU
	mov     w1, #0                 //; w1 = level iterator

invalidateCaches_flush_level:
	add     w3, w1, w1, lsl #1     //; w3 = w1 * 3 (right-shift for cache type)
	lsr     w3, w0, w3             //; w3 = w0 >> w3
	ubfx    w3, w3, #0, #3         //; w3 = cache type of this level
	cmp     w3, #2                 //; No cache at this level?
	b.lt    invalidateCaches_next_level

	lsl     w4, w1, #1
	msr     CSSELR_EL1, x4         //; Select current cache level in CSSELR
	isb                            //; ISB required to reflect new CSIDR
	mrs     x4, CCSIDR_EL1         //; w4 = CSIDR

	ubfx    w3, w4, #0, #3
	add    	w3, w3, #2             //; w3 = log2(line size)
	ubfx    w5, w4, #13, #15
	ubfx    w4, w4, #3, #10        //; w4 = Way number
	clz     w6, w4                 //; w6 = 32 - log2(number of ways)

invalidateCaches_flush_set:
	mov     w8, w4                 //; w8 = Way number
invalidateCaches_flush_way:
	lsl     w7, w1, #1             //; Fill level field
	lsl     w9, w5, w3
	orr     w7, w7, w9             //; Fill index field
	lsl     w9, w8, w6
	orr     w7, w7, w9             //; Fill way field
	dc      CISW, x7               //; Invalidate by set/way to point of coherency
	subs    w8, w8, #1             //; Decrement way
	b.ge    invalidateCaches_flush_way
	subs    w5, w5, #1             //; Descrement set
	b.ge    invalidateCaches_flush_set

invalidateCaches_next_level:
	add     w1, w1, #1             //; Next level
	cmp     w2, w1
	b.gt    invalidateCaches_flush_level

invalidateCaches_end:
	ret

.set BOOT_CPU_MODE_EL1,	(0xe11)
.set BOOT_CPU_MODE_EL2,	(0xe12)
/* Hyp Configuration Register (HCR) bits */
.set HCR_E2H,         ((1) << 34)
.set HCR_TGE,         ((1) << 27)

/* AArch64 SPSR bits */
.set PSR_F_BIT,	0x00000040
.set PSR_I_BIT,	0x00000080
.set PSR_A_BIT,	0x00000100
.set PSR_D_BIT,	0x00000200
.set PSR_MODE_EL1h,	0x00000005
.set CurrentEL_EL2,		(2 << 2)
.set HCR_RW,		((1) << 31)

/*
 * If we're fortunate enough to boot at EL2, ensure that the world is
 * sane before dropping to EL1.
 *
 * Returns either BOOT_CPU_MODE_EL1 or BOOT_CPU_MODE_EL2 in x20 if
 * booted in EL1 or EL2 respectively.
 */
.global el2_setup;
el2_setup:
	mrs	x0, CurrentEL
	cmp	x0, #CurrentEL_EL2
	b.ne	1f
	mrs	x0, sctlr_el2
	orr	x0, x0, #(1 << 25)		// Set the EE bit for EL2
	bic	x0, x0, #(1 << 25)		// Clear the EE bit for EL2
	msr	sctlr_el2, x0
	b	2f
1:	mrs	x0, sctlr_el1
	orr	x0, x0, #(3 << 24)		// Set the EE and E0E bits for EL1
	bic	x0, x0, #(3 << 24)		// Clear the EE and E0E bits for EL1
	msr	sctlr_el1, x0
	mov	w0, #BOOT_CPU_MODE_EL1		// This cpu booted in EL1
	isb
	ret

2:
#ifdef CONFIG_ARM64_VHE
	/*
	 * Check for VHE being present. For the rest of the EL2 setup,
	 * x2 being non-zero indicates that we do have VHE, and that the
	 * kernel is intended to run at EL2.
	 */
	mrs	x2, id_aa64mmfr1_el1
	ubfx	x2, x2, #8, #4
#else
	mov	x2, xzr
#endif

	/* Hyp configuration. */
	mov	x0, #HCR_RW			// 64-bit EL1
	cbz	x2, set_hcr
	orr	x0, x0, #HCR_TGE		// Enable Host Extensions
	orr	x0, x0, #HCR_E2H
set_hcr:
	msr	hcr_el2, x0
	isb

	/* Generic timers. */
	mrs	x0, cnthctl_el2
	orr	x0, x0, #3			// Enable EL1 physical timers
	msr	cnthctl_el2, x0
	msr	cntvoff_el2, xzr		// Clear virtual offset

#ifdef CONFIG_ARM_GIC_V3
	/* GICv3 system register access */
	mrs	x0, id_aa64pfr0_el1
	ubfx	x0, x0, #24, #4
	cmp	x0, #1
	b.ne	3f

	mrs_s	x0, ICC_SRE_EL2
	orr	x0, x0, #ICC_SRE_EL2_SRE	// Set ICC_SRE_EL2.SRE==1
	orr	x0, x0, #ICC_SRE_EL2_ENABLE	// Set ICC_SRE_EL2.Enable==1
	msr_s	ICC_SRE_EL2, x0
	isb					// Make sure SRE is now set
	mrs_s	x0, ICC_SRE_EL2			// Read SRE back,
	tbz	x0, #0, 3f			// and check that it sticks
	msr_s	ICH_HCR_EL2, xzr		// Reset ICC_HCR_EL2 to defaults

3:
#endif

	/* Populate ID registers. */
	mrs	x0, midr_el1
	mrs	x1, mpidr_el1
	msr	vpidr_el2, x0
	msr	vmpidr_el2, x1

	/*
	 * When VHE is not in use, early init of EL2 and EL1 needs to be
	 * done here.
	 * When VHE _is_ in use, EL1 will not be used in the host and
	 * requires no configuration, and all non-hyp-specific EL2 setup
	 * will be done via the _EL1 system register aliases in __cpu_setup.
	 */
	cbnz	x2, 1f

	/* sctlr_el1 */
	mov	x0, #0x0800			// Set/clear RES{1,0} bits
	//movk	x0, #0x33d0, lsl #16		// Set EE and E0E on BE systems
	//movk	x0, #0x30d0, lsl #16		// Clear EE and E0E on LE systems
	msr	sctlr_el1, x0

	/* Coprocessor traps. */
	mov	x0, #0x33ff
	msr	cptr_el2, x0			// Disable copro. traps to EL2
1:

#ifdef CONFIG_COMPAT
	msr	hstr_el2, xzr			// Disable CP15 traps to EL2
#endif

	/* EL2 debug */
	mrs	x0, id_aa64dfr0_el1		// Check ID_AA64DFR0_EL1 PMUVer
	sbfx	x0, x0, #8, #4
	cmp	x0, #1
	b.lt	4f				// Skip if no PMU present
	mrs	x0, pmcr_el0			// Disable debug access traps
	ubfx	x0, x0, #11, #5			// to EL2 and allow access to
4:
	csel	x0, xzr, x0, lt			// all PMU counters from EL1
	msr	mdcr_el2, x0			// (if they exist)

	/* Stage-2 translation */
	msr	vttbr_el2, xzr

	cbz	x2, install_el2_stub

	mov	w0, #BOOT_CPU_MODE_EL2		// This CPU booted in EL2
	isb
	ret

install_el2_stub:
	/* Hypervisor stub */
	//adrp	x0, __hyp_stub_vectors
	//add	x0, x0, #:lo12:__hyp_stub_vectors
	//msr	vbar_el2, x0

	/* spsr */
	mov	x0, #(PSR_F_BIT | PSR_I_BIT | PSR_A_BIT | PSR_D_BIT |\
		      PSR_MODE_EL1h)
	msr	spsr_el2, x0
	//msr	elr_el2, lr
	msr	elr_el2, x30 // Need to change to r14 if 32bit mode
	mov	w0, #BOOT_CPU_MODE_EL2		// This CPU booted in EL2
	eret
.type el2_setup, %function;
.end el2_setup;
.size el2_setup,.-el2_setup

.end
/**
* @} End of "addtogroup a53_64_boot_code".
*/
