#include "bits.h"
#include "csr.h"
#include <platform.h>
#include <riscv/asm_macros.S>

	.globl  bl1_entrypoint
	.globl  bl1_entrypoint_end
	.globl  bl1_entrypoint_rel

	/* -----------------------------------------------------
	 * bl1_entrypoint() is the entry point into the trusted
	 * firmware code when a cpu is released from warm or
	 * cold reset.
	 * -----------------------------------------------------
	 */
	.pushsection .entry, "ax"
bl1_entrypoint:
    la x1, bl1_entrypoint_rel
    jr x1
bl1_entrypoint_end:
    .popsection

  .option norvc
  .section .text.init,"ax",@progbits
  .globl bl1_entrypoint_rel
bl1_entrypoint_rel:
  atf_state_set x28, x29, ATF_STATE_BL1_ENTRY_POINT
	atf_err_set x28, x29, ATF_ERR_NONE

  li x1, 0
  li x2, 0
  li x3, 0
  li x4, 0
  li x5, 0
  li x6, 0
  li x7, 0
  li x8, 0
  li x9, 0
  li x10, 0
  li x11, 0
  li x12, 0
  li x13, 0
  li x14, 0
  li x15, 0
  li x16, 0
  li x17, 0
  li x18, 0
  li x19, 0
  li x20, 0
  li x21, 0
  li x22, 0
  li x23, 0
  li x24, 0
  li x25, 0
  li x26, 0
  li x27, 0
  li x28, 0
  li x29, 0
  li x30, 0
  li x31, 0

  csrw mscratch, x0

  # write mtvec and make sure it sticks
  la t0, trap_vector
  csrw mtvec, t0

  # set mxstatus to init value
  li x3, 0xc0638000
  csrw mxstatus, x3

  # set plic_ctrl = 1 for linux plic
  li x3, 0x701FFFFC # plic_base + 0x1FFFFC
  li x4, 1
  sw x4 , 0(x3)

  // invalidate all memory for BTB,BHT,DCACHE,ICACHE
  li x3, 0x30013
  csrs mcor, x3
  // enable ICACHE,DCACHE,BHT,BTB,RAS,WA
  li x3, 0x7f
  csrs mhcr, x3
  // enable data_cache_prefetch, amr
  li x3, 0x610c
  csrs mhint, x3 #mhint

  # enable fp
  li x3, 0x1 << 13
  csrs mstatus, x3

  la sp, __STACKS_END__

  call bl1_early_platform_setup

  call bl1_main
  j die

  .balign 4
trap_vector:
die:
  j panic_handler
  j die

  .bss
  .align RISCV_PGSHIFT
stacks:
  .skip RISCV_PGSIZE

  .section .rodata

