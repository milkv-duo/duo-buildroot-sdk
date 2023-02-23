/* Kernel includes. */
#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
#include "task.h"
#include "semphr.h"
#include "io.h"

/* Xilinx includes. */
#include "printf.h"
#include "rtos_queue.h"
#include "rtos_cmdqu.h"
#include "comm.h"

#define mainQUEUE_LENGTH (1)
#define mainQUEUE_Cmdqu_TASK_PRIORITY (tskIDLE_PRIORITY + 5)
#define mainQUEUE_ISP_TASK_PRIORITY (tskIDLE_PRIORITY + 3)
#define mainQUEUE_Vcodec_TASK_PRIORITY (tskIDLE_PRIORITY + 3)
#define mainQUEUE_Vip_TASK_PRIORITY (tskIDLE_PRIORITY + 3)
#define mainQUEUE_Display_TASK_PRIORITY (tskIDLE_PRIORITY + 3)

void prvQueueISR(void);
void prvCmdQuRunTask(void *pvParameters);

QueueHandle_t xQueueIsp;
QueueHandle_t xQueueVcodec;
QueueHandle_t xQueueVip;
QueueHandle_t xQueueDisplay;
QueueHandle_t xQueueCmdqu;

void main_cvirtos(void)
{
	printf("create cvi task\n");
	/* register prvQueueISR to IPI_8 */
	request_sgirq(8, prvQueueISR, 0, "prvQueueISR");
	xQueueIsp = xQueueCreate(mainQUEUE_LENGTH, sizeof(cmdqu_t));
	xQueueVcodec = xQueueCreate(mainQUEUE_LENGTH, sizeof(cmdqu_t));
	xQueueVip = xQueueCreate(mainQUEUE_LENGTH, sizeof(cmdqu_t));
	xQueueDisplay = xQueueCreate(mainQUEUE_LENGTH, sizeof(cmdqu_t));
	xQueueCmdqu = xQueueCreate(mainQUEUE_LENGTH, sizeof(cmdqu_t));

	if (xQueueIsp != NULL && xQueueVcodec != NULL && xQueueVip != NULL &&
	    xQueueDisplay != NULL) {
		xTaskCreate(prvISPRunTask, "ISP", configMINIMAL_STACK_SIZE,
			    NULL, mainQUEUE_ISP_TASK_PRIORITY, NULL);
		xTaskCreate(prvVcodecRunTask, "Vcodec",
			    configMINIMAL_STACK_SIZE, NULL,
			    mainQUEUE_Vcodec_TASK_PRIORITY, NULL);
		xTaskCreate(prvVipRunTask, "Vip", configMINIMAL_STACK_SIZE,
			    NULL, mainQUEUE_Vip_TASK_PRIORITY, NULL);
		xTaskCreate(prvDisplayRunTask, "Display",
			    configMINIMAL_STACK_SIZE, NULL,
			    mainQUEUE_Display_TASK_PRIORITY, NULL);
		xTaskCreate(prvCmdQuRunTask, "CmdQu", configMINIMAL_STACK_SIZE,
			    NULL, mainQUEUE_Cmdqu_TASK_PRIORITY, NULL);

		/* Start the tasks and timer running. */
		vTaskStartScheduler();
	}
}

void prvCmdQuRunTask(void *pvParameters)
{
	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;

	struct rtos_shm_t *shm_para = 0x120000000;
	cmdqu_t rtos_cmdq;
	size_t malloc_addr;
	printf("prvCmdQuRunTask run\n");

	for (;;) {
		xQueueReceive(xQueueCmdqu, &rtos_cmdq, portMAX_DELAY);
		printf("prvCmdQuRunTask id=%d cmd=%d para=%lx\n",
		       rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);
		if (rtos_cmdq.ip_id == IP_SYSTEM) {
			switch (rtos_cmdq.cmd_id) {
			case 0:
				/* RTOS init malloc done */
				memory_init(shm_para);
				/* RTOS init queue done */
				//queue_init(shm_para);
				/* send command to linux */
				queue_enqueue(&shm_para->rtos_cmd_queue,
					      &rtos_cmdq);
				break;

			case 1:
				/* RTOS allocate memory */
				malloc_addr = memory_alloc(rtos_cmdq.param_ptr);
				rtos_cmdq.param_ptr = malloc_addr;
				/* send command to linux */
				queue_enqueue(&shm_para->rtos_cmd_queue,
					      &rtos_cmdq);
				writel(0x10008, 0x1F01F00);
				__iowmb();
				break;

			case 2:
				/* RTOS free memory */
				memory_free(rtos_cmdq.param_ptr);
				/* send command to linux */
				queue_enqueue(&shm_para->rtos_cmd_queue,
					      &rtos_cmdq);
				break;

			default:
				break;
			}
		} else {
			/* send command to linux*/
			queue_enqueue(&shm_para->rtos_cmd_queue, &rtos_cmdq);
			writel(0x10008, 0x1F01F00);
			__iowmb();
		}
		rtos_cmdq.ip_id = -1;
	}
}

void prvQueueISR(void)
{
	printf("prvQueueISR\n");
	struct rtos_shm_t *shm_para = 0x120000000;
	queue_t *linux_cmd_queue = (queue_t *)&shm_para->linux_cmd_queue;
	//	printf("buffer =%lx\n", ((unsigned long)shm_para->linux_cmd_queue.queue_buffer));
	//	printf("offset =%lx\n", shm_para->virt_phys_offset);
	//	printf("queue  =%lx\n", linux_cmd_queue);

	while (!queue_is_empty(linux_cmd_queue)) {
		/* receive command from linux*/
		cmdqu_t *cmdq;
		cmdqu_t rtos_cmdq;
		cmdq = queue_peek(linux_cmd_queue);
		printf("cmdq id = %lx\n", cmdq->ip_id);
		printf("cmdq cmd = %lx\n", cmdq->cmd_id);
		printf("cmdq param = %lx\n", cmdq->param_ptr);

		rtos_cmdq.ip_id = cmdq->ip_id;
		rtos_cmdq.cmd_id = cmdq->cmd_id;
		rtos_cmdq.param_ptr = cmdq->param_ptr;

		static int queue_init_done = 0;
		/*
		 * RTOS run queue_init here because need to run init,
		 * before using queue_dequeue & queue_enqueue function.
		 */
		if (!queue_init_done) {
			queue_init(shm_para);
			queue_init_done = 1;
		}

		queue_dequeue(linux_cmd_queue);
		switch (rtos_cmdq.ip_id) {
		case IP_ISP:
			//if(!xQueueIsQueueFullFromISR(xQueueIsp))
			xQueueSendFromISR(xQueueIsp, &rtos_cmdq, 0U);
			//else
			//	printf("ISP Queue is full\n");
			break;
		case IP_VCODEC:
			xQueueSendFromISR(xQueueVcodec, &rtos_cmdq, 0U);
			break;
		case IP_VIP:
			xQueueSendFromISR(xQueueVip, &rtos_cmdq, 0U);
			break;
		case IP_DISPLAY:
			xQueueSendFromISR(xQueueDisplay, &rtos_cmdq, 0U);
			break;
		case IP_SYSTEM:
			xQueueSendFromISR(xQueueCmdqu, &rtos_cmdq, 0U);
			break;
		default:
			break;
		}
	}
}
