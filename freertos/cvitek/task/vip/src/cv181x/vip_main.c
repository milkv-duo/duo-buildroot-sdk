/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* cvitek includes. */
#include "printf.h"
#include "rtos_cmdqu.h"

#include "vip_main.h"

static QueueHandle_t xQueueVip;
static QueueHandle_t xQueueCmdqu;

void prvVipRunTask(void *pvParameters)
{
	// Remove compiler warning about unused parameter.
	(void)pvParameters;
	cmdqu_t rtos_cmdq;

	printf("prvVipRunTask run\n");

	for (;;) {
		xQueueReceive(xQueueVip, &rtos_cmdq, portMAX_DELAY);
		// wait cv181x command queue and add action for command
		printf("prvVipRunTask id=%d cmd=%d para=%lx\n",
			rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);
		// send cmd back to Cmdqu task and send mailbox to linux
		if (rtos_cmdq.ip_id == IP_VIP) {
			xQueueSend(xQueueCmdqu, &rtos_cmdq, 0U);
		}
		rtos_cmdq.ip_id = -1;
	}
}
