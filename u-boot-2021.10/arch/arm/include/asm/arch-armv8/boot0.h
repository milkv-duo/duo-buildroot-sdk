/* SPDX-License-Identifier: GPL-2.0 */
/*
 * CVITEK u-boot header
 */

#ifndef __BOOT0_H__
#define __BOOT0_H__

/* BOOT0 header information */
	b boot0_time_recode
	.word 0x33334c42  /* b'BL33' */
	.word 0xdeadbee1  /* CKSUM */
	.word 0xdeadbee4  /* SIZE */
	.quad CONFIG_SYS_TEXT_BASE /* RUNADDR */
	.word 0xdeadbee6
	b boot0_time_recode
/* BOOT0 header end */
boot0_time_recode:
	mrs x0, cntpct_el0
	ldr x1, =BOOT0_START_TIME
	str w0, [x1]
	b reset

	.global BOOT0_START_TIME
BOOT0_START_TIME:
	.word 0

#endif /* __BOOT0_H__ */
