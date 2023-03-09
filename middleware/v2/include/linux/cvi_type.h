/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_type.h
 * Description:
 */

#ifndef __CVI_TYPE_H__
#define __CVI_TYPE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifdef __KERNEL__
#else
#include <stdint.h>
#endif

/*----------------------------------------------
 * The common data type
 *----------------------------------------------
 */

typedef unsigned char           CVI_UCHAR;
typedef unsigned char           CVI_U8;
typedef unsigned short          CVI_U16;
typedef unsigned int            CVI_U32;
typedef unsigned int            CVI_HANDLE;

typedef signed char             CVI_S8;
typedef char                    CVI_CHAR;
typedef short                   CVI_S16;
typedef int                     CVI_S32;

typedef unsigned long           CVI_UL;
typedef signed long             CVI_SL;

typedef float                   CVI_FLOAT;
typedef double                  CVI_DOUBLE;

typedef void                    CVI_VOID;
typedef unsigned char           CVI_BOOL;

typedef uint64_t                CVI_U64;
typedef int64_t                 CVI_S64;

typedef unsigned int            CVI_SIZE_T;

/*----------------------------------------------
 * const defination
 *----------------------------------------------
 */

#define CVI_NULL                0L
#define CVI_SUCCESS             0
#define CVI_FAILURE             (-1)
#define CVI_FAILURE_ILLEGAL_PARAM (-2)
#define CVI_TRUE                1
#define CVI_FALSE               0

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_TYPE_H__ */
