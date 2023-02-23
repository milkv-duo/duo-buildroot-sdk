/* Kernel includes. */
#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
#include "task.h"
#include "semphr.h"

/* Xilinx includes. */
#include "printf.h"
#include "rtos_queue.h"

#include "vip_main.h"

void prvVipRunTask(void *pvParameters)
{
	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;
	cmdqu_t rtos_cmdq;

	printf("prvVipRunTask run\n");

	for (;;) {
		xQueueReceive(xQueueVip, &rtos_cmdq, portMAX_DELAY);
		printf("prvVipRunTask id=%d cmd=%d para=%lx\n", rtos_cmdq.ip_id,
		       rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);
		if (rtos_cmdq.ip_id == 3) {
			xQueueSend(xQueueCmdqu, &rtos_cmdq, 0U);
		}
		rtos_cmdq.ip_id = -1;
	}
}
