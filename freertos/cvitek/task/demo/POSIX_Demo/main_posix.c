/*
 * FreeRTOS POSIX Demo V1.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/*
 ******************************************************************************
 * -NOTE- The Win32 port is a simulation (or is that emulation?) only!  Do not
 * expect to get real time behaviour from the Win32 port or this demo
 * application.  It is provided as a convenient development and demonstration
 * test bed only.  This was tested using Windows 10 with Intel Core i7-6700 CPU.
 *
 * Windows will not be running the FreeRTOS simulator threads continuously, so
 * the timing information has no meaningful units. See the documentation page for
 * the Windows simulator for an explanation of the slow timing:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 * - READ THE WEB DOCUMENTATION FOR THIS PORT FOR MORE INFORMATION ON USING IT -
 *
 * Documentation for this demo can be found on:
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_POSIX/demo/posix_demo.html
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+POSIX. */
#include "FreeRTOS_POSIX/unistd.h"

/* System headers */
#include <stdio.h>

/* demo include */
#include "posix_demo.h"

/* Demo task priority */
#define mainPOSIX_DEMO_PRIORITY (tskIDLE_PRIORITY + 4)

/*-----------------------------------------------------------*/

int main_posix(void)
{
	TickType_t xLastExecutionTime;

	configASSERT((mainPOSIX_DEMO_PRIORITY < configMAX_PRIORITIES));

	const uint32_t ulLongTime_ms = pdMS_TO_TICKS(1000UL);

	printf("FreeRTOS POSIX demo\n");

	/* Start the task to run POSIX demo */
	xTaskCreate(vStartPOSIXDemo, "posix", configMINIMAL_STACK_SIZE, NULL,
		    mainPOSIX_DEMO_PRIORITY, NULL);

	vTaskStartScheduler();

	/* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
    works correctly. */
	xLastExecutionTime = xTaskGetTickCount();

	/* If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the idle and/or
     * timer tasks to be created.  See the memory management section on the
     * FreeRTOS web site for more details (this is standard text that is not
     * really applicable to the Win32 simulator port). */
	for (;;) {
		/* Delay until it is time to execute again. */
		vTaskDelayUntil(&xLastExecutionTime, ulLongTime_ms);
	}

	return 0;
}

/*-----------------------------------------------------------*/

void vAssertCalled(const char *pcFile, uint32_t ulLine)
{
	const uint32_t ulLongSleep = 1000UL;
	volatile uint32_t ulBlockVariable = 0UL;
	volatile char *pcFileName = (volatile char *)pcFile;
	volatile uint32_t ulLineNumber = ulLine;

	(void)pcFileName;
	(void)ulLineNumber;

	printf("vAssertCalled %s, %ld\n", pcFile, (long)ulLine);
	fflush(stdout);

	/* Setting ulBlockVariable to a non-zero value in the debugger will allow
     * this function to be exited. */
	taskDISABLE_INTERRUPTS();
	{
		while (ulBlockVariable == 0UL) {
			sleep(ulLongSleep);
		}
	}
	taskENABLE_INTERRUPTS();
}
