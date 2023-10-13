/* SPDX-License-Identifier: GPL-2.0 */
/*
 * CVITEK u-boot header
 */

#ifndef __BOOT0_H__
#define __BOOT0_H__

/* BOOT0 header information */
	j boot0_time_recode
	.balign 4
	.word 0x33334c42  /* b'BL33' */
	.word 0xdeadbeea  /* CKSUM */
	.word 0xdeadbeeb  /* SIZE */
#ifdef CONFIG_SPL_BUILD
	.quad	CONFIG_SPL_TEXT_BASE
#else
	.quad   CONFIG_SYS_TEXT_BASE
#endif
	.word 0xdeadbeec
	.balign 4
	j boot0_time_recode
	.balign 4
/* BOOT0 header end */
boot0_time_recode:
	csrr x1, time
	la x2, BOOT0_START_TIME
	sw x1, 0(x2)
	j _start_real

	.global BOOT0_START_TIME
BOOT0_START_TIME:
	.word 0

#endif /* __BOOT0_H__ */
