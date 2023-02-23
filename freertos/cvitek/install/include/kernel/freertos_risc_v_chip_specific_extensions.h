/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code that tailors the port to a specific RISC-V chip:
 *
 * + FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S contains the code that
 *   is common to all currently supported RISC-V chips.  There is only one
 *   portASM.S file because the same file is built for all RISC-V target chips.
 *
 * + Header files called freertos_risc_v_chip_specific_extensions.h contain the
 *   code that tailors the FreeRTOS kernel's RISC-V port to a specific RISC-V
 *   chip.  There are multiple freertos_risc_v_chip_specific_extensions.h files
 *   as there are multiple RISC-V chip implementations.
 *
 * !!!NOTE!!!
 * TAKE CARE TO INCLUDE THE CORRECT freertos_risc_v_chip_specific_extensions.h
 * HEADER FILE FOR THE CHIP IN USE.  This is done using the assembler's (not the
 * compiler's!) include path.  For example, if the chip in use includes a core
 * local interrupter (CLINT) and does not include any chip specific register
 * extensions then add the path below to the assembler's include path:
 * FreeRTOS\Source\portable\GCC\RISC-V-RV32\chip_specific_extensions\RV32I_CLINT_no_extensions
 *
 */


#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#define portasmHAS_SIFIVE_CLINT 1
#define portasmHAS_MTIME 1
#ifdef RISCV_FPU
#define portasmADDITIONAL_CONTEXT_SIZE (34) /*in test_if_asynchronous, it will store mepc in 0 * sp position*/
#else
#define portasmADDITIONAL_CONTEXT_SIZE (0) /* Must be even number on 32-bit cores. */
#endif

#define portasmHANDLE_INTERRUPT do_irq

.macro portasmSAVE_ADDITIONAL_REGISTERS
	/* No additional registers to save, so this macro does nothing. */
#ifdef RISCV_FPU
	addi sp, sp, -(portasmADDITIONAL_CONTEXT_SIZE * portWORD_SIZE)
	store_f f0, 1 * portWORD_SIZE( sp )
	store_f f1, 2 * portWORD_SIZE( sp )
	store_f f2, 3 * portWORD_SIZE( sp )
	store_f f3, 4 * portWORD_SIZE( sp )
	store_f f4, 5 * portWORD_SIZE( sp )
	store_f f5, 6 * portWORD_SIZE( sp )
	store_f f6, 7 * portWORD_SIZE( sp )
	store_f f7, 8 * portWORD_SIZE( sp )
	store_f f8, 9 * portWORD_SIZE( sp )
	store_f f9, 10 * portWORD_SIZE( sp )
	store_f f10, 11 * portWORD_SIZE( sp )
	store_f f11, 12 * portWORD_SIZE( sp )
	store_f f12, 13 * portWORD_SIZE( sp )
	store_f f13, 14 * portWORD_SIZE( sp )
	store_f f14, 15 * portWORD_SIZE( sp )
	store_f f15, 16 * portWORD_SIZE( sp )
	store_f f16, 17 * portWORD_SIZE( sp )
	store_f f17, 18 * portWORD_SIZE( sp )
	store_f f18, 19 * portWORD_SIZE( sp )
	store_f f19, 20 * portWORD_SIZE( sp )
	store_f f20, 21 * portWORD_SIZE( sp )
	store_f f21, 22 * portWORD_SIZE( sp )
	store_f f22, 23 * portWORD_SIZE( sp )
	store_f f23, 24 * portWORD_SIZE( sp )
	store_f f24, 25 * portWORD_SIZE( sp )
	store_f f25, 26 * portWORD_SIZE( sp )
	store_f f26, 27 * portWORD_SIZE( sp )
	store_f f27, 28 * portWORD_SIZE( sp )
	store_f f28, 29 * portWORD_SIZE( sp )
	store_f f29, 30 * portWORD_SIZE( sp )
	store_f f30, 31 * portWORD_SIZE( sp )
	store_f f31, 32 * portWORD_SIZE( sp )
	csrr  t0, fcsr
	store_x t0, 33 * portWORD_SIZE( sp )
#endif
	.endm

.macro portasmRESTORE_ADDITIONAL_REGISTERS
	/* No additional registers to restore, so this macro does nothing. */
#ifdef RISCV_FPU
	load_f f0, 1 * portWORD_SIZE( sp )
	load_f f1, 2 * portWORD_SIZE( sp )
	load_f f2, 3 * portWORD_SIZE( sp )
	load_f f3, 4 * portWORD_SIZE( sp )
	load_f f4, 5 * portWORD_SIZE( sp )
	load_f f5, 6 * portWORD_SIZE( sp )
	load_f f6, 7 * portWORD_SIZE( sp )
	load_f f7, 8 * portWORD_SIZE( sp )
	load_f f8, 9 * portWORD_SIZE( sp )
	load_f f9, 10 * portWORD_SIZE( sp )
	load_f f10, 11 * portWORD_SIZE( sp )
	load_f f11, 12 * portWORD_SIZE( sp )
	load_f f12, 13 * portWORD_SIZE( sp )
	load_f f13, 14 * portWORD_SIZE( sp )
	load_f f14, 15 * portWORD_SIZE( sp )
	load_f f15, 16 * portWORD_SIZE( sp )
	load_f f16, 17 * portWORD_SIZE( sp )
	load_f f17, 18 * portWORD_SIZE( sp )
	load_f f18, 19 * portWORD_SIZE( sp )
	load_f f19, 20 * portWORD_SIZE( sp )
	load_f f20, 21 * portWORD_SIZE( sp )
	load_f f21, 22 * portWORD_SIZE( sp )
	load_f f22, 23 * portWORD_SIZE( sp )
	load_f f23, 24 * portWORD_SIZE( sp )
	load_f f24, 25 * portWORD_SIZE( sp )
	load_f f25, 26 * portWORD_SIZE( sp )
	load_f f26, 27 * portWORD_SIZE( sp )
	load_f f27, 28 * portWORD_SIZE( sp )
	load_f f28, 29 * portWORD_SIZE( sp )
	load_f f29, 30 * portWORD_SIZE( sp )
	load_f f30, 31 * portWORD_SIZE( sp )
	load_f f31, 32 * portWORD_SIZE( sp )
	load_x t0, 33 * portWORD_SIZE( sp )
	csrw  fcsr, t0
	addi sp, sp, (portasmADDITIONAL_CONTEXT_SIZE * portWORD_SIZE)
#endif
	.endm

#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
