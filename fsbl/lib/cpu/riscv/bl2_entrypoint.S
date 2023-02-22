#include "bits.h"
#include "csr.h"
#include <riscv/asm_macros.S>

  .option norvc
  .section .text.init,"ax",@progbits
  .globl bl2_entrypoint
bl2_entrypoint:
	j bl2_entrypoint_real
	.word 0 // resvered
	.word 0 // BL2 MSID
	.word 0 // BL2 version
	.word 0 //
	.word 0
	.word 0
	.word 0

bl2_entrypoint_real:
  atf_state_set x28, x29, ATF_STATE_BL2_ENTRY_POINT
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

  # set plic_ctrl = 1
  li x3, 0x701FFFFC # plic_base + 0x1FFFFC
  li x4, 1
  sw x4 , 0(x3)

  # invalid I-cache
  li x3, 0x33
  csrc mcor, x3
  li x3, 0x11
  csrs mcor, x3
  # enable I-cache
  li x3, 0x1
  csrs mhcr, x3
  # invalid D-cache
  li x3, 0x33
  csrc mcor, x3
  li x3, 0x12
  csrs mcor, x3
  # enable D-cache
  li x3, 0x2
  csrs mhcr, x3

  la sp, __STACKS_END__

  la a3, __BSS_START__
  la a4, __BSS_END__
  sub a4, a4, a3

bss_clear:
  sd x0, 0(a3)
  addi a3, a3, 8
  addi a4, a4, -8
  bnez a4, bss_clear

  call bl2_main
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

