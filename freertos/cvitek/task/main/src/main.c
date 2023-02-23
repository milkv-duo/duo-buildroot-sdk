/*
 * FreeRTOS Kernel V10.3.0
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

/******************************************************************************
 *
 * See http://www.FreeRTOS.org/RTOS-Xilinx-UltraScale_MPSoC_64-bit.html for
 * additional information on this demo.
 *
 * NOTE 1:  This project provides two demo applications.  A simple blinky
 * style project, and a more comprehensive test and demo application.  The
 * RUN_TYPE in build.sh setting in main.c is used to select between the two.
 * See the notes on using RUN_TYPE in build.sh where it is defined below.
 *
 * NOTE 2:  This file only contains the source code that is not specific to
 * either the simply blinky or full demos - this includes initialisation code
 * and callback functions.
 *
 * NOTE 3:  This project builds the FreeRTOS source code, so is expecting the
 * BSP project to be configured as a 'standalone' bsp project rather than a
 * 'FreeRTOS' bsp project.  However the BSP project MUST still be build with
 * the FREERTOS_BSP symbol defined (-DFREERTOS_BSP must be added to the
 * command line in the BSP configuration).
 */
#ifdef __riscv
/* Standard includes. */
#include <stdio.h>
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

#else
#include "linux/types.h"
/* Scheduler include files. */
#include "FreeRTOS_POSIX.h"
#include "task.h"

/* Xilinx includes. */
#include "xscugic.h"
#if ( configUSE_TRACE_FACILITY == 1 )
#include "trcRecorder.h"
#endif

/* The interrupt controller is initialised in this file, and made available to
other modules. */
XScuGic xInterruptController;

#endif
// #include "sleep.h"

/* RUN_TYPE in build.sh is used to select between two demo applications,
 * as described at the top of this file.
 *
 * When RUN_TYPE is set to BLINKY_DEMO the simple blinky example will
 * be run.
 *
 * When RUN_TYPE is set to FULL_DEMO the comprehensive test and demo
 * application will be run.
 */

/*-----------------------------------------------------------*/

/*
 * Configure the hardware as necessary to run this demo.
 */
static void prvSetupHardware(void);

/*
 * See the comments at the top of this file and above the
 * RUN_TYPE in build.sh definition.
 */
#ifdef CVIRTOS
extern void main_cvirtos(void);
#elif defined BLINKY_DEMO
extern void main_blinky(void);
#elif defined FULL_DEMO
extern void main_full(void);
#elif defined POSIX_DEMO
extern void main_posix(void);
#else
#error Invalid RUN_TYPE setting in build.sh.  See the comments at the top of this file and above the RUN_TYPE definition.
#endif

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/* configAPPLICATION_ALLOCATED_HEAP is set to 1 in FreeRTOSConfig.h so the
application can define the array used as the FreeRTOS heap.  This is done so the
heap can be forced into fast internal RAM - useful because the stacks used by
the tasks come from this space. */
#ifdef configAPPLICATION_ALLOCATED_HEAP
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ( ( section( ".heap" ) ) );
#endif
/*-----------------------------------------------------------*/

int main(void)
{
	pre_system_init();
	printf("CVIRTOS Build Date:%s  (Time :%s) \n", __DATE__, __TIME__);
#ifndef __riscv
	mmu_enable();
	printf("enable I/D cache & MMU done\n");
#endif
	/* Configure the hardware ready to run the demo. */
	prvSetupHardware();
#if ( configUSE_TRACE_FACILITY == 1 )
	vTraceEnable(TRC_START);
#endif
	post_system_init();

#ifdef CVIRTOS
	{
		main_cvirtos();
	}
#elif defined BLINKY_DEMO
	{
		main_blinky();
	}
#elif defined FULL_DEMO
	{
		main_full();
	}
#elif defined POSIX_DEMO
	{
		main_posix();
	}
#else
#error "Not correct running definition"
#endif

	/* Don't expect to reach here. */
	return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware(void)
{
#ifdef __riscv
#else
	BaseType_t xStatus;
	XScuGic_Config *pxGICConfig;

	/* Ensure no interrupts execute while the scheduler is in an inconsistent
	state.  Interrupts are automatically enabled when the scheduler is
	started. */
	portDISABLE_INTERRUPTS();

	/* Obtain the configuration of the GIC. */
	pxGICConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);

	/* Sanity check the FreeRTOSConfig.h settings are correct for the
	hardware. */
	configASSERT(pxGICConfig);
	configASSERT(pxGICConfig->CpuBaseAddress ==
		     (configINTERRUPT_CONTROLLER_BASE_ADDRESS +
		      configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET));
	configASSERT(pxGICConfig->DistBaseAddress ==
		     configINTERRUPT_CONTROLLER_BASE_ADDRESS);

	/* Install a default handler for each GIC interrupt. */
	xStatus = XScuGic_CfgInitialize(&xInterruptController, pxGICConfig,
					pxGICConfig->CpuBaseAddress);
	configASSERT(xStatus == XST_SUCCESS);
	(void)xStatus; /* Remove compiler warning if configASSERT() is not defined. */
#endif
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	taskDISABLE_INTERRUPTS();
	dump_uart_disable();
	printf("vApplicationMallocFailedHook\n");
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;
	dump_uart_disable();
	printf("%s %s\n", __func__, pcTaskName);
	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
	volatile size_t xFreeHeapSpace;

	/* This is just a trivial example of an idle hook.  It is called on each
	cycle of the idle task.  It must *NOT* attempt to block.  In this case the
	idle task just queries the amount of FreeRTOS heap that remains.  See the
	memory management section on the http://www.FreeRTOS.org web site for memory
	management options.  If there is a lot of heap memory free then the
	configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
	RAM. */
	xFreeHeapSpace = xPortGetFreeHeapSize();

	/* Remove compiler warning about xFreeHeapSpace being set but never used. */
	(void)xFreeHeapSpace;
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
#ifdef FULL_DEMO
	{
		/* Only the comprehensive demo actually uses the tick hook. */
		extern void vFullDemoTickHook(void);
		vFullDemoTickHook();
	}
#endif
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
				   StackType_t **ppxIdleTaskStackBuffer,
				   uint32_t *pulIdleTaskStackSize)
{
	/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
				    StackType_t **ppxTimerTaskStackBuffer,
				    uint32_t *pulTimerTaskStackSize)
{
	/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/

void vMainAssertCalled(const char *pcFileName, uint32_t ulLineNumber)
{
	dump_uart_disable(); 
	printf("ASSERT!  Line %d of file %s\r\n", ulLineNumber, pcFileName);
	taskENTER_CRITICAL();
	for (;;)
		;
}
