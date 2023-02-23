/* Kernel includes. */
#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/time.h"
#include "task.h"
#include "semphr.h"

#include "timers.h"
/* Xilinx includes. */
#include "printf.h"
#include "rtos_queue.h"
//#include "cmdqueue.h"
#include "cvi_vip_cif_uapi.h"

extern QueueHandle_t xQueueIsp;
extern QueueHandle_t xQueueCmdqu;
extern int sample_snsr_test(void);
extern int cvi_cif_probe(void);
void prvISPRunTask(void *pvParameters)
{
	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;
	cmdqu_t rtos_cmdq;
	printf("prvISPRunTask run\n");
	cvi_cif_init();
	cvi_vip_init();

	sample_snsr_test();
	for (;;) {
		xQueueReceive(xQueueIsp, &rtos_cmdq, portMAX_DELAY);
		printf("prvISPRunTask id=%d cmd=%d para=%lx\n", rtos_cmdq.ip_id,
		       rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);
		if (rtos_cmdq.ip_id == 0) {
			xQueueSend(xQueueCmdqu, &rtos_cmdq, 0U);
		}
		rtos_cmdq.ip_id = -1;
	}
}
