/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* cvitek includes. */
#include "printf.h"
#include "rtos_cmdqu.h"
#include "fast_image.h"
#include "comm.h"
#include "h26x_enc.h"
#include "intr_conf.h"
#include "cvi_comm_video.h"
#include "vi_uapi.h"
#include "jenc.h"

QueueHandle_t xQueueVcodec;
QueueHandle_t xQueueVCCmdqu;
QueueHandle_t xQueueVICmdqu;
extern struct transfer_config_t transfer_config;

#define JPEG_ADDR_ALIGN 16


JPEG_PIC encoded_pics[MAX_JPEG_NUM];
CVI_S32 num_encoded = 0;
CVI_U32 cur_bit_addr = 0x0;

static void encode_one_jpeg(CVI_U32 width, CVI_U32 height, CVI_U32 phySrcAddr, CVI_U64 pts)
{
	int pic_size = 0;

	if (num_encoded >= MAX_JPEG_NUM) {
		printf("[vcodec][error] max number of encoded jpeg pictures reached\n");
		return;
	}

	pic_size = jpu_enc(width, height, phySrcAddr, cur_bit_addr, pts);
	if (pic_size < 0) {
		printf("[vcodec][error] jpeg encode fail\n");
		return;
	}

	encoded_pics[num_encoded].width = width;
	encoded_pics[num_encoded].height = height;
	encoded_pics[num_encoded].addr = cur_bit_addr;
	encoded_pics[num_encoded].size = pic_size;
	encoded_pics[num_encoded].pts = pts;

	printf("[vcodec] jpeg %d: %dx%d, %d bytes @ 0x%x, pts %lld\n",
		num_encoded,
		encoded_pics[num_encoded].width,
		encoded_pics[num_encoded].height,
		encoded_pics[num_encoded].size,
		encoded_pics[num_encoded].addr,
		encoded_pics[num_encoded].pts);

	cur_bit_addr += encoded_pics[num_encoded].size;
	cur_bit_addr = (cur_bit_addr + (JPEG_ADDR_ALIGN - 1)) & ~(JPEG_ADDR_ALIGN - 1);
	num_encoded++;
}

void prvVcodecRunTask(void *pvParameters)
{
	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;
	u8 encode_type;
	cmdqu_t rtos_cmdq;
	printf("prvVcodecRunTask run\n");

	xQueueVcodec = main_GetMODHandle(E_QUEUE_VCODEC);
	xQueueVCCmdqu = main_GetMODHandle(E_QUEUE_CMDQU);
	xQueueVICmdqu = main_GetMODHandle(E_QUEUE_VI);

	/* get enc buffer information */
	printf("trans enc buff addr = %x\n", transfer_config.encode_buf_addr);
	printf("trans enc buff size = %x\n", transfer_config.encode_buf_size);
	printf("trans enc img  addr = %x\n", transfer_config.encode_img_addr);
	printf("trans enc img  size = %x\n", transfer_config.encode_img_size);
	printf("trans enc type = %x\n", transfer_config.image_type);
	encode_type = transfer_config.image_type;
	cur_bit_addr = transfer_config.encode_img_addr;

	memset(&encoded_pics, 0, sizeof(encoded_pics));

#if 0
	encode_one_jpeg(352, 288, transfer_config.encode_buf_addr, 22446688);
#endif

	for (;;) {
		xQueueReceive(xQueueVcodec, &rtos_cmdq, portMAX_DELAY);
		//TODO: wait cv181x command queue and add action for command
		printf("prvVcodecRunTask ip=%d cmd=%d para=%lx\n",
			rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);

		if (rtos_cmdq.ip_id == IP_VCODEC) {
			switch (rtos_cmdq.cmd_id) {
				case SYS_CMD_INFO_STOP_ISR:
					/* do something to stop isr */

					disable_irq(JPEG_INTERRUPT);
					disable_irq(H264_INTERRUPT);
					disable_irq(H265_INTERRUPT);
					/* info linux, rtos isr is disabled */
					rtos_cmdq.cmd_id = SYS_CMD_INFO_STOP_ISR_DONE;
					xQueueSend(xQueueVCCmdqu, &rtos_cmdq, 0U);
					break;
				case VCORE_CMD_ENCODE:
					switch (encode_type) {
						case E_FAST_JEPG:
							break;
						case E_FAST_H264:
							break;
						case E_FAST_H265:
							break;
					}
					break;
				case CODEC_CMDQ_RECV_BUF:
				{
					VIDEO_FRAME_S buf;

					buf = *((struct _VIDEO_FRAME_S *)rtos_cmdq.param_ptr);

					printf("VC_RECV_BUF w_h=%d_%d, addr=0x%x, PTS=%d\n",
						buf.u32Width, buf.u32Height, buf.u64PhyAddr[0], buf.u64PTS);

					encode_one_jpeg(buf.u32Width, buf.u32Height, buf.u64PhyAddr[0], buf.u64PTS);

					rtos_cmdq.ip_id  = IP_VI;
					rtos_cmdq.cmd_id = VI_CMDQ_INFO_RECV_BUF_ACK;
					xQueueSend(xQueueVICmdqu, &rtos_cmdq, 0U);
					break;
				}
				default:
					break;
			}
		}
		rtos_cmdq.ip_id = -1;
	}
}
