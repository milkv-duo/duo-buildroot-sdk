/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

/* cvitek includes. */
#include "printf.h"
#include "rtos_cmdqu.h"
#include "fast_image.h"
#include "intr_conf.h"
#include "comm.h"
#include "cvi_vi.h"
#include "vi_uapi.h"

static QueueHandle_t gVIQueHandle;
static QueueHandle_t gCMDQUQueHandle;
extern struct transfer_config_t transfer_config;

void prvVIRunTask(void *pvParameters)
{
	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;
	struct transfer_config_t *p_transfer_config;
	cmdqu_t rtos_cmdq;

	printf("%s:%d run\n", __FUNCTION__, __LINE__);
	gVIQueHandle = main_GetMODHandle(E_QUEUE_VI);
	gCMDQUQueHandle = main_GetMODHandle(E_QUEUE_CMDQU);
	/* get isp buffer information */
	printf("isp buffer addr = %x\n", transfer_config.isp_buffer_addr);
	printf("isp buffer size = %x\n", transfer_config.isp_buffer_size);

	CVI_VI_Start(transfer_config.isp_buffer_addr, transfer_config.isp_buffer_size);

	for (;;) {
		xQueueReceive(gVIQueHandle, &rtos_cmdq, portMAX_DELAY);
		printf("prvVIRunTask ip=%d, cmd=%d, para=0x%x\n",
			rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);

		if (rtos_cmdq.ip_id == IP_VI) {
			switch (rtos_cmdq.cmd_id) {
				case SYS_CMD_INFO_TRANS:
					/* send from linux side */
					p_transfer_config = (struct transfer_config_t *) rtos_cmdq.param_ptr;
					printf("trans isp buff addr = %x\n", p_transfer_config->isp_buffer_addr);
					printf("trans isp buff size = %x\n", p_transfer_config->isp_buffer_size);
					/* update rtos local buffer parameters */
					transfer_config.isp_buffer_addr = p_transfer_config->isp_buffer_addr;
					transfer_config.isp_buffer_size = p_transfer_config->isp_buffer_size;
					break;
				case SYS_CMD_INFO_STOP_ISR:
					/* do something to stop isr */
					CVI_VI_Stop();

					/* info linux that rtos isr is disabled */
					disable_irq(ISP_INT);
					rtos_cmdq.cmd_id = SYS_CMD_INFO_STOP_ISR_DONE;
					xQueueSend(gCMDQUQueHandle, &rtos_cmdq, 0U);
					break;
				case VI_CMDQ_INFO_RECV_BUF_ACK:
					CVI_VI_QBUF();
					break;
				default:
					break;
			}
		} else {
			//send back the err msg
			printf("VIRunTask got invalid ip_id[%d]\n", rtos_cmdq.ip_id);
			xQueueSend(gCMDQUQueHandle, &rtos_cmdq, 0U);
			goto WRONG_CMD_IP_ID;
		}

WRONG_CMD_IP_ID:
		rtos_cmdq.ip_id  = -1;
		rtos_cmdq.cmd_id = -1;
		rtos_cmdq.param_ptr = NULL;
	}
}
