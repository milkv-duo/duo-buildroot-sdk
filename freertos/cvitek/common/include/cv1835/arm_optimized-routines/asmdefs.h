/*
 * Macros for asm code.
 *
 * Copyright (c) 2019, Arm Limited.
 * SPDX-License-Identifier: MIT
 */

#ifndef _ASMDEFS_H
#define _ASMDEFS_H

#if defined(__aarch64__)

/* Define aarch64 function to name what we use. */
#define __memcmp_aarch64 memcmp
#define __memcpy_aarch64 memcpy
#define __memchr_aarch64 memchr
#define __memrchr_aarch64 memrchr
#define __memmove_aarch64 memmove
#define __memset_aarch64 memset
#define __strchr_aarch64 strchr
#define __strchrnul_aarch64 strchrnul
#define __strcpy_aarch64 strcpy
#define __strncpy_aarch64 strncpy
#define __strrchr_aarch64 strrchr
#define __strcmp_aarch64 strcmp
#define __strncmp_aarch64 strncmp
#define __strlen_aarch64 strlen
#define __strnlen_aarch64 strnlen
/* More simd/sve need to be re-define.
 * __memcmp_aarch64_sve
 * __memcpy_aarch64_simd
 * __memchr_aarch64_mte
 * __memchr_aarch64_sve
 * __memmove_aarch64_simd
 * __strchr_aarch64_mte
 * __strchrnul_aarch64_mte
 * __strrchr_aarch64_sve
 * __strrchr_aarch64_mte
 * __strcmp_aarch64_sve
 * __strcmp_aarch64_mte
 * __strncmp_aarch64_mte
 * __strncmp_aarch64_sve
 * __strlen_aarch64_sve
 * __strlen_aarch64_mte
 * __strnlen_aarch64_sve
 * */

/* Branch Target Identitication support.  */
#define BTI_C		hint	34
#define BTI_J		hint	36
/* Return address signing support (pac-ret).  */
#define PACIASP		hint	25; .cfi_window_save
#define AUTIASP		hint	29; .cfi_window_save

/* GNU_PROPERTY_AARCH64_* macros from elf.h.  */
#define FEATURE_1_AND 0xc0000000
#define FEATURE_1_BTI 1
#define FEATURE_1_PAC 2

/* Add a NT_GNU_PROPERTY_TYPE_0 note.  */
#define GNU_PROPERTY(type, value)	\
  .section .note.gnu.property, "a";	\
  .p2align 3;				\
  .word 4;				\
  .word 16;				\
  .word 5;				\
  .asciz "GNU";				\
  .word type;				\
  .word 4;				\
  .word value;				\
  .word 0;				\
  .text

/* If set then the GNU Property Note section will be added to
   mark objects to support BTI and PAC-RET.  */
#ifndef WANT_GNU_PROPERTY
#define WANT_GNU_PROPERTY 1
#endif

#if WANT_GNU_PROPERTY
/* Add property note with supported features to all asm files.  */
GNU_PROPERTY (FEATURE_1_AND, FEATURE_1_BTI|FEATURE_1_PAC)
#endif

#define ENTRY_ALIGN(name, alignment)	\
  .global name;		\
  .type name,%function;	\
  .align alignment;		\
  name:			\
  .cfi_startproc;	\
  BTI_C;

#else

/* Define arm function to name what we use. */
#define __strcmp_arm strcmp
#define __memcpy_arm memcpy
#define __strlen_armv6t2 strlen

/* More functions need to be re-define.
 * __strlen_armv6t2
 * __strcmp_armv6m
 * */

#define END_FILE

#define ENTRY_ALIGN(name, alignment)	\
  .global name;		\
  .type name,%function;	\
  .align alignment;		\
  name:			\
  .cfi_startproc;

#endif

#define ENTRY(name)	ENTRY_ALIGN(name, 6)

#define ENTRY_ALIAS(name)	\
  .global name;		\
  .type name,%function;	\
  name:

#define END(name)	\
  .cfi_endproc;		\
  .size name, .-name;

#define L(l) .L ## l

#endif
