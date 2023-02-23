/* Kernel includes. */
#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
#include "task.h"
#include "semphr.h"

/* Xilinx includes. */
#include "printf.h"
#include "rtos_queue.h"
#include "venc/main_enc_cvitest.h"
#include "cvi_venc.h"

extern QueueHandle_t xQueueVcodec;
extern QueueHandle_t xQueueCmdqu;

// just to build & compiler venc sdk, do not need run this
__attribute__((optimize("-O0"))) void cvi_venc_test(void)
{
	int venc_test = 0;

	if (venc_test != 0) {
		cvitest_venc_main(0, 0);
	//	venc_main(0, 0);
		CVI_VENC_CreateChn(0, 0);
		CVI_VENC_SendFrame(0, 0, 0);
		CVI_VENC_StartRecvFrame(0, 0);
		CVI_VENC_StopRecvFrame(0);
		CVI_VENC_QueryStatus(0, 0);
		CVI_VENC_GetStream(0, 0, 0);
		CVI_VENC_ReleaseStream(0, 0);
		CVI_VENC_DestroyChn(0);
		CVI_VENC_ResetChn(0);
		CVI_VENC_GetFd(0);
		CVI_VENC_SetJpegParam(0, 0);
		CVI_VENC_GetJpegParam(0, 0);
		CVI_VENC_RequestIDR(0, 0);
		CVI_VENC_SetChnAttr(0, 0);
		CVI_VENC_GetChnAttr(0, 0);
		CVI_VENC_GetRcParam(0, 0);
		CVI_VENC_SetRcParam(0, 0);
		CVI_VENC_GetRefParam(0, 0);
		CVI_VENC_SetRefParam(0, 0);
		CVI_VENC_GetRoiAttr(0, 0, 0);
		CVI_VENC_SetFrameLostStrategy(0, 0);
		CVI_VENC_SetRoiAttr(0, 0);
		CVI_VENC_GetFrameLostStrategy(0, 0);
		CVI_VENC_SetChnParam(0, 0);
		CVI_VENC_GetChnParam(0, 0);
	}
}

void prvVcodecRunTask(void *pvParameters)
{
	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;
	cmdqu_t rtos_cmdq;

	printf("prvVcodecRunTask run\n");
	// init
	vpu_init();
	printf("init done\n");
	cvi_venc_test();

	for (;;) {
		xQueueReceive(xQueueVcodec, &rtos_cmdq, portMAX_DELAY);
		printf("prvVcodecRunTask id=%d cmd=%d para=%lx\n",
			rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);
		if (rtos_cmdq.ip_id == 1) {
			xQueueSend(xQueueCmdqu, &rtos_cmdq, 0U);
		}
		rtos_cmdq.ip_id = -1;
	}
}
