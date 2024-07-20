/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <platform.h>

#ifdef __riscv
OUTPUT_FORMAT("elf64-littleriscv")
OUTPUT_ARCH(riscv)
#else
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
#endif
ENTRY(bl2_entrypoint)

MEMORY {
    RAM (rwx): ORIGIN = BL2_BASE, LENGTH = BL2_SIZE

    SRAM (rwx): ORIGIN = TPU_SRAM_BASE, LENGTH = TPU_SRAM_SIZE
}

SECTIONS
{
    . = BL2_BASE;
    ASSERT(. == ALIGN(4096),
           "BL2_BASE address is not aligned on a page boundary.")

    ro . : {
        __RO_START__ = .;
        *bl2_entrypoint.o(.text*)
        *(.vectors)
        *(.text*)
        *(.rodata*)

        __RO_END__ = .;
    } >RAM

    /*
     * .data must be placed at a lower address than the stacks if the stack
     * protector is enabled. Alternatively, the .data.stack_protector_canary
     * section can be placed independently of the main .data section.
     */
    .data . : {
        . = ALIGN(16);
        __DATA_START__ = .;
        *(.data*)
        . = ALIGN(16);
        __DATA_END__ = .;
    } >RAM

    stacks (NOLOAD) : {
        . = ALIGN(64);
        __STACKS_START__ = .;
        . += PLATFORM_STACK_SIZE;
        . = ALIGN(64);
        __STACKS_END__ = .;
    } >RAM

    /*
     * The .bss section gets initialised to 0 at runtime.
     * Its base address should be 16-byte aligned for better performance of the
     * zero-initialization code.
     */
    .bss : ALIGN(16) {
        . = ALIGN(16);
        __BSS_START__ = .;
        *(SORT_BY_ALIGNMENT(.bss*))
        *(COMMON)
        . = ALIGN(16);
        __BSS_END__ = .;
    } >RAM

    /*
     * Define a linker symbol to mark end of the RW memory area for this
     * image.
     */
    __BL2_END__ = .;

    __BSS_SIZE__ = SIZEOF(.bss);

    ASSERT(. <= (BL2_BASE + BL2_SIZE), "BL2 image has exceeded its limit.")

  .suspend_sect :
  {
    . = ALIGN(8);
    __suspend_sect_start = .;
    KEEP(*(.suspend_entry))
    KEEP(*(.suspend_func))
    . = ALIGN(8);
    __suspend_data_start = .;
    KEEP(*(.suspend_data))
    . = ALIGN(8);
    __suspend_sect_end = .;
  } > SRAM
    #include <rom_api_refer.h>
}
