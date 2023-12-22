/*
 * Copyright (c) 2017 Linaro Limited
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_TYPES_H_
#define _BLE_MESH_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ssv_err.h"
#include "soc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t u64_t;
typedef int64_t s64_t;
typedef int bt_mesh_atomic_t;
//#define OS_TASK_PRIOBASE     0
#define OS_TASK_HIGHEST (OS_TASK_PRIOBASE + 6)
#define OS_TASK_HIGH (OS_TASK_PRIOBASE + 5)
#define OS_TASK_MIDDLE (OS_TASK_PRIOBASE + 4)
#define OS_TASK_LOW (OS_TASK_PRIOBASE + 3)

/*
typedef signed char         s8_t;
typedef signed short        s16_t;
typedef signed int          s32_t;
typedef signed long long    s64_t;

typedef unsigned char       u8_t;
typedef unsigned short      u16_t;
typedef unsigned int        u32_t;
typedef unsigned long long  u64_t;

typedef int         bt_mesh_atomic_t;

#ifndef bool
#define bool        int8_t
#endif

#ifndef false
#define false       0
#endif

#ifndef true
#define true        1
#endif
*/


#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_TYPES_H_ */
