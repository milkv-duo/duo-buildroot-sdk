/*
 * FreeRTOS V202107.00
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
 * https://www.FreeRTOS.org
 * https://www.github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "riscv-virt.h"
#include "arch_cpu.h"
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* See https://www.freertos.org/Using-FreeRTOS-on-RISC-V.html */


#define _WINDOWS_
#ifdef RISCV_QEMU
#define configMTIME_BASE_ADDRESS		( CLINT_ADDR + CLINT_MTIME )
#else
#undef configMTIME_BASE_ADDRESS  // get MTIME from rdtime
#endif
#define configMTIMECMP_BASE_ADDRESS		( CLINT_ADDR + CLINT_MTIMECMP )

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				0
#define configUSE_TICK_HOOK				1
#define configTICK_RATE_HZ				( ( TickType_t ) 200 )
#define configMAX_PRIORITIES			( 8 )
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 1024 )
#ifndef FAST_IMAGE_ENABLE
#define configTOTAL_HEAP_SIZE			( ( size_t ) 1 * 512 * 1024 )
#else
#define configTOTAL_HEAP_SIZE			( ( size_t ) 1 * 650 * 1024 )
#endif
#define configMAX_TASK_NAME_LEN			( 16 )
#define configUSE_TRACE_FACILITY		0
#define configUSE_16_BIT_TICKS			0
#define configUSE_TICKLESS_IDLE         0
#define configIDLE_SHOULD_YIELD			1
#define configUSE_MUTEXES				1
#define configQUEUE_REGISTRY_SIZE		50
#define configCHECK_FOR_STACK_OVERFLOW	2
#define configUSE_RECURSIVE_MUTEXES		1
#define configUSE_MALLOC_FAILED_HOOK	1
#define configUSE_APPLICATION_TASK_TAG	0
#define configUSE_COUNTING_SEMAPHORES	1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_STATIC_ALLOCATION	1
#define configGENERATE_RUN_TIME_STATS	0
#define configSUPPORT_STATIC_ALLOCATION 1
#define configAPPLICATION_ALLOCATED_HEAP 1
//#define configSUPPORT_DYNAMIC_ALLOCATION 1
/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 			0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS				1
#define configTIMER_TASK_PRIORITY		( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH		5
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )


#define configTASK_NOTIFICATION_ARRAY_ENTRIES 3
/* RISC-V definitions. */
#define configISR_STACK_SIZE_WORDS		1024

/* Task priorities.  Allow these to be overridden. */
#ifndef uartPRIMARY_PRIORITY
	#define uartPRIMARY_PRIORITY		( configMAX_PRIORITIES - 3 )
#endif

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			1
#define INCLUDE_vTaskDelete					1
#define INCLUDE_vTaskCleanUpResources		1
#define INCLUDE_vTaskSuspend				1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay					1
#define INCLUDE_eTaskGetState				1
#define INCLUDE_xTimerPendFunctionCall		1
#define INCLUDE_xTaskAbortDelay				1
#define INCLUDE_xTaskGetHandle				1
#define INCLUDE_xSemaphoreGetMutexHolder	1

#define fabs(x) __builtin_fabs(x)


/* FreeRTOS+POSIX
 * Portable Operating System Interface (POSIX threading wrapper) for FreeRTOS
 *
 * Dependencies
 * Both configUSE_POSIX_ERRNO and configUSE_APPLICATION_TASK_TAG must be set to 1 in FreeRTOSConfig.h.
 */
#define configUSE_POSIX_ERRNO 1


/* Overwrite some of the stack sizes allocated to various test and demo tasks.
Like all task stack sizes, the value is the number of words, not bytes. */
#define bktBLOCK_TIME_TASK_STACK_SIZE 100
#define notifyNOTIFIED_TASK_STACK_SIZE 110
#define priSUSPENDED_RX_TASK_STACK_SIZE 90
#define tmrTIMER_TEST_TASK_STACK_SIZE 100
#define ebRENDESVOUS_TEST_TASK_STACK_SIZE 100
#define ebEVENT_GROUP_SET_BITS_TEST_TASK_STACK_SIZE 115
#define genqMUTEX_TEST_TASK_STACK_SIZE 90
#define recmuRECURSIVE_MUTEX_TEST_TASK_STACK_SIZE 110

/* Integrates the Tracealyzer recorder with FreeRTOS */
#if ( configUSE_TRACE_FACILITY == 1 )
#include "trcRecorder.h"
#endif

#endif /* FREERTOS_CONFIG_H */
