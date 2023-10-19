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
 * NOTE 1:  This project provides two demo applications.  A simple blinky
 * style project, and a more comprehensive test and demo application.  The
 * mainSELECTED_APPLICATION setting in main.c is used to select between the
 * two.  See the notes on using mainSELECTED_APPLICATION in main.c.  This file
 * implements the simply blinky style version.
 *
 * NOTE 2:  This file only contains the source code that is specific to the
 * simple demo.  Generic functions, such FreeRTOS hook functions, and functions
 * required to configure the hardware are defined in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 200 milliseconds, before sending the value 100 to the queue that
 * was created within main_blinky().  Once the value is sent, the task loops
 * back around to block for another 200 milliseconds...and so on.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * main_blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, outputs a message to the
 * UART.  The 'block time' parameter passed to the queue receive function
 * specifies that the task should be held in the Blocked state indefinitely to
 * wait for data to be available on the queue.  The queue receive task will only
 * leave the Blocked state when the queue send task writes to the queue.  As the
 * queue send task writes to the queue every 200 milliseconds, the queue receive
 * task leaves the Blocked state every 200 milliseconds, and therefore outputs
 * a message every 200 milliseconds.
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Xilinx includes. */
#include "xil_printf.h"
#include "cmdqueue.h"
/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 5000 )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );
static void prvISPRunTask( void *pvParameters );
static void prvVcodecRunTask( void *pvParameters );
static void prvVpssRunTask( void *pvParameters );
static void prvCmdQuRunTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;
static QueueHandle_t xQueueIsp = NULL;
static QueueHandle_t xQueueVcodec = NULL;
static QueueHandle_t xQueueVpss = NULL;
static QueueHandle_t xQueueCmdqu = NULL;

/*-----------------------------------------------------------*/

void main_blinky( void )
{
	/* Create the queue. */
	xQueue       = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );
    xQueueIsp    = xQueueCreate( mainQUEUE_LENGTH, sizeof( cmdqu_t ) );
    xQueueVcodec = xQueueCreate( mainQUEUE_LENGTH, sizeof( cmdqu_t ) );
    xQueueVpss   = xQueueCreate( mainQUEUE_LENGTH, sizeof( cmdqu_t ) );
    xQueueCmdqu  = xQueueCreate( mainQUEUE_LENGTH, sizeof( cmdqu_t ) );

	if( xQueue != NULL && xQueueIsp != NULL && xQueueVcodec != NULL && xQueueVpss != NULL)
	{
		/* Start the two tasks as described in the comments at the top of this
		file. */
		xTaskCreate( prvQueueReceiveTask,				/* The function that implements the task. */
					"Rx", 								/* The text name assigned to the task - for debug only as it is not used by the kernel. */
					configMINIMAL_STACK_SIZE, 			/* The size of the stack to allocate to the task. */
					NULL, 								/* The parameter passed to the task - not used in this case. */
					mainQUEUE_RECEIVE_TASK_PRIORITY, 	/* The priority assigned to the task. */
					NULL );								/* The task handle is not required, so NULL is passed. */

		xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );

		xTaskCreate( prvISPRunTask, "ISP", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );
		xTaskCreate( prvVcodecRunTask, "Vcodec", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );
		xTaskCreate( prvVpssRunTask, "Vpss", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );

		xTaskCreate( prvCmdQuRunTask, "Vpss", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );

		/* Start the tasks and timer running. */
		vTaskStartScheduler();
	}

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was either insufficient FreeRTOS heap memory available for the idle
	and/or timer tasks to be created, or vTaskStartScheduler() was called from
	User mode.  See the memory management section on the FreeRTOS web site for
	more details on the FreeRTOS heap http://www.freertos.org/a00111.html.  The
	mode from which main() is called is set in the C start up code and must be
	a privileged mode (not user mode). */
	for( ;; );
}
/*-----------------------------------------------------------*/
static void prvCmdQuRunTask( void *pvParameters )
{
	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
    cmdqu_t rtos_cmdq;

    struct shm_para_t *shm_para = 0x120000040;
    xil_printf("prvCmdQuRunTask run\n");

    for( ;; )
    {
        xQueueReceive( xQueueCmdqu, &rtos_cmdq, portMAX_DELAY );
        /* send command to linux*/
        queue_enqueue(&shm_para->rtos_cmd_queue, &rtos_cmdq);
        volatile int * addr=0x1F01F00;
        *addr=0x10008;
        asm volatile("dsb sy;");
        asm volatile("isb sy;");
    }
}
static void prvISPRunTask( void *pvParameters )
{
	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
    cmdqu_t rtos_cmdq;
    xil_printf("prvISPRunTask run\n");

    for( ;; )
    {
        xQueueReceive( xQueueIsp, &rtos_cmdq, portMAX_DELAY );
        xil_printf("prvISPRunTask id=%d cmd=%d para=%lx\n", rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);
        if(rtos_cmdq.ip_id ==0) {
		    xQueueSend( xQueueCmdqu, &rtos_cmdq, 0U );
        }
        rtos_cmdq.ip_id = -1;
    }
}
static void prvVcodecRunTask( void *pvParameters )
{
    /* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

    xil_printf("prvVcodecRunTask run\n");
    cmdqu_t rtos_cmdq;
    for( ;; )
    {
        xQueueReceive( xQueueVcodec, &rtos_cmdq, portMAX_DELAY );
        xil_printf("prvVcodecRunTask id=%d cmd=%d para=%lx\n", rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);
        if(rtos_cmdq.ip_id == 1) {
		    xQueueSend( xQueueCmdqu, &rtos_cmdq, 0U );
        }
        rtos_cmdq.ip_id = -1;
    }
}
static void prvVpssRunTask( void *pvParameters )
{
    /* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

    xil_printf("prvVpssRunTask run\n");
    cmdqu_t rtos_cmdq;
    for( ;; )
    {
        xQueueReceive( xQueueVpss, &rtos_cmdq, portMAX_DELAY );
        xil_printf("prvVpssRunTask id=%d cmd=%d para=%lx\n", rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);
        if(rtos_cmdq.ip_id == 2) {
		    xQueueSend( xQueueCmdqu, &rtos_cmdq, 0U );
        }
        rtos_cmdq.ip_id = -1;
    }
}
static void prvQueueSendTask( void *pvParameters )
{
TickType_t xNextWakeTime;
const uint32_t ulValueToSend = 100UL;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xQueueSend( xQueue, &ulValueToSend, 0U );
	}
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
uint32_t ulReceivedValue;
const uint32_t ulExpectedValue = 100UL;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	for( ;; )
	{
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		/* To get here something must have been received from the queue, but is
		it the expected value?  If it is, toggle the LED. */
		if( ulReceivedValue == ulExpectedValue )
		{
			xil_printf( "100 received\r\n" );
			ulReceivedValue = 0U;
		}
	}
}
enum IP_TYPE{
    IP_ISP,
    IP_VCODEC,
    IP_VPSS,
};
/*-----------------------------------------------------------*/

void prvQueueISR(void)
{
    xil_printf("prvQueueISR\n");
    struct shm_para_t *shm_para = 0x120000040;
    queue_t *linux_cmd_queue = &shm_para->linux_cmd_queue;
    xil_printf("buffer =%lx\n",((unsigned long)shm_para->linux_cmd_queue.queue_buffer));
    xil_printf("offset =%lx\n", shm_para->virt_phys_offset);
    xil_printf("queue  =%lx\n", linux_cmd_queue);

    while(!queue_is_empty(linux_cmd_queue))
    {
        /* receive command from linux*/
        cmdqu_t * cmdq;
        cmdqu_t rtos_cmdq;
        cmdq = queue_peek(linux_cmd_queue);
        xil_printf("cmdq id = %lx\n", cmdq->ip_id);
        xil_printf("cmdq cmd = %lx\n", cmdq->cmd_id);
        xil_printf("cmdq param = %lx\n", cmdq->param_ptr);

        rtos_cmdq.ip_id = cmdq->ip_id;
        rtos_cmdq.cmd_id = cmdq->cmd_id;
        rtos_cmdq.param_ptr = cmdq->param_ptr;

        queue_dequeue(linux_cmd_queue);
        switch (rtos_cmdq.ip_id) {
            case IP_ISP:
//                if(!xQueueIsQueueFullFromISR(xQueueIsp))
                xQueueSendFromISR( xQueueIsp, &rtos_cmdq, 0U );
//                else
//                    xil_printf("ISP Queue is full\n");
                break;
            case IP_VCODEC:
                xQueueSendFromISR( xQueueVcodec, &rtos_cmdq, 0U );
                break;
            case IP_VPSS:
                xQueueSendFromISR( xQueueVpss, &rtos_cmdq, 0U );
                break;
            default:
                break;
        }
    }
}
