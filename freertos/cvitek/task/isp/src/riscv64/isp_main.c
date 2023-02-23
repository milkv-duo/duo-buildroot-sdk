/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "arch_helpers.h"

/* cvitek includes. */
#include "printf.h"
#include "rtos_cmdqu.h"
#include "fast_image.h"
#include "comm.h"

#include "rtos_isp_cmd.h"
#include "vi_uapi.h"
#include "vi_tun_cfg.h"
#include "isp_mgr_buf.h"
#include "rtos_isp_mgr.h"

static QueueHandle_t gISPQueHandle;
static QueueHandle_t gQueueISPCmdqu;
extern struct transfer_config_t transfer_config;

extern void AE_RtosBufInit(CVI_U8 sID);
extern void AWB_RtosBufInit(CVI_U8 sID);

#include "sample_comm.h"
//static CVI_VOID isp_init();

CVI_VOID *g_pIspShardBuffer;

static void runIspInitCmd(cmdqu_t *rtos_cmdq)
{
	g_pIspShardBuffer = ISP_PTR_CAST_VOID(rtos_cmdq->param_ptr);
	struct rtos_isp_cmd_event *ev = (struct rtos_isp_cmd_event *)
		ISP_PTR_CAST_VOID(rtos_cmdq->param_ptr);

	inv_dcache_range(rtos_cmdq->param_ptr, sizeof(struct rtos_isp_cmd_event));

	isp_mgr_buf_init(ev->ViPipe);

	isp_mgr_buf_invalid_cache(ev->ViPipe);

	isp_tun_buf_ctrl_init(ev->ViPipe);

	isp_feature_ctrl_init(ev->ViPipe);

	AE_RtosBufInit(ev->ViPipe);
	AWB_RtosBufInit(ev->ViPipe);

	isp_mgr_buf_flush_cache(ev->ViPipe);
}

static void runIspEventCmd(cmdqu_t *rtos_cmdq)
{
	struct rtos_isp_cmd_event *ev = (struct rtos_isp_cmd_event *)
		ISP_PTR_CAST_VOID(rtos_cmdq->param_ptr);

	isp_mgr_buf_invalid_cache(ev->ViPipe);

	switch (rtos_cmdq->cmd_id) {
	case RTOS_ISP_CMD_PRE_SOF:
		rtos_isp_mgr_pre_sof(ev->ViPipe, ev->frame_idx);
		break;
	case RTOS_ISP_CMD_PRE_EOF:
		rtos_isp_mgr_pre_eof(ev->ViPipe, ev->frame_idx);
		break;
	case RTOS_ISP_CMD_POST_EOF:
		rtos_isp_mgr_post_eof(ev->ViPipe, ev->frame_idx);
		break;
	default:
		break;
	}

	ev->is_slave_done = CVI_TRUE;

	isp_mgr_buf_flush_cache(ev->ViPipe);
}

static void runIspDeinitCmd(cmdqu_t *rtos_cmdq)
{
	struct rtos_isp_cmd_event *ev = (struct rtos_isp_cmd_event *)
		ISP_PTR_CAST_VOID(rtos_cmdq->param_ptr);

	inv_dcache_range(rtos_cmdq->param_ptr, sizeof(struct rtos_isp_cmd_event));

	isp_feature_ctrl_uninit(ev->ViPipe);

	isp_tun_buf_ctrl_uninit(ev->ViPipe);

	isp_mgr_buf_uninit(ev->ViPipe);
}

void prvISPRunTask(void *pvParameters)
{
	cmdqu_t rtos_cmdq;
	//struct transfer_config_t *p_transfer_config;

	printf("prvISPRunTask run\n");
	gISPQueHandle = main_GetMODHandle(E_QUEUE_ISP);
	gQueueISPCmdqu = main_GetMODHandle(E_QUEUE_CMDQU);

	/* get isp buffer information */
	printf("isp buffer addr = %x\n", transfer_config.isp_buffer_addr);
	printf("isp buffer size = %x\n", transfer_config.isp_buffer_size);

	// isp_init();

	for (;;) {
		xQueueReceive(gISPQueHandle, &rtos_cmdq, portMAX_DELAY);
		//TODO: wait cv181x command queue and add action for command
		//printf("prvIspRunTask ip=%d cmd=%d para=%lx\n",
		//	rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);

		CVI_BOOL need_ack = CVI_TRUE;

		switch (rtos_cmdq.cmd_id) {
#ifdef FAST_IMAGE_ENABLE
		// cmd from isp drv
		case ISP_CMDQ_RECV_EV:
		{
			struct vi_event ev;

			ev = *((struct vi_event *)rtos_cmdq.param_ptr);
			printf("dev(%d) fidx(%d) sidx(%d)\n", ev.dev_id, ev.frame_sequence, ev.stt_idx);

			switch(ev.type) {
			case VI_EVENT_PRE0_SOF:
				rtos_isp_mgr_pre_sof(0, ev.frame_sequence);
				rtos_isp_mgr_pre_fe_sof(0, ev.frame_sequence);
				rtos_isp_mgr_pre_be_sof(0, ev.frame_sequence);
			break;
			case VI_EVENT_PRE1_SOF:
				rtos_isp_mgr_pre_sof(1, ev.frame_sequence);
				rtos_isp_mgr_pre_fe_sof(1, ev.frame_sequence);
				rtos_isp_mgr_pre_be_sof(1, ev.frame_sequence);
			break;
			case VI_EVENT_PRE0_EOF:
				rtos_isp_mgr_pre_eof(0, ev.frame_sequence);
				rtos_isp_mgr_pre_fe_eof(0, ev.frame_sequence);
				rtos_isp_mgr_pre_be_eof(0, ev.frame_sequence);
			break;
			case VI_EVENT_PRE1_EOF:
				rtos_isp_mgr_pre_eof(1, ev.frame_sequence);
				rtos_isp_mgr_pre_fe_eof(1, ev.frame_sequence);
				rtos_isp_mgr_pre_be_eof(1, ev.frame_sequence);
			break;
			case VI_EVENT_POST_EOF:
				isp_sts_ctrl_set_post_idx(0, ev.stt_idx);
				rtos_isp_mgr_post_eof(0, ev.frame_sequence);
			break;
			case VI_EVENT_POST1_EOF:
				isp_sts_ctrl_set_post_idx(1, ev.stt_idx);
				rtos_isp_mgr_post_eof(1, ev.frame_sequence);
			break;
			}
			need_ack = CVI_FALSE;
		}
		break;
		case ISP_CMDQ_GET_STS_MEM:
		{
			//struct cvi_isp_sts_mem *sts_mem;

			//sts_mem = ((struct cvi_isp_sts_mem *)rtos_cmdq.param_ptr);
			//isp_sts_ctrl_set_mem(sts_mem->raw_num, sts_mem);
			need_ack = CVI_FALSE;
		}
		break;
		case ISP_CMDQ_GET_TUNING_NODE_MEM:
			//isp_tun_buf_ctrl_set_buf(0, (struct isp_tuning_cfg *)rtos_cmdq.param_ptr);
			//isp_tun_buf_ctrl_set_buf(1, (struct isp_tuning_cfg *)rtos_cmdq.param_ptr);
			need_ack = CVI_FALSE;
		break;
#endif
/*---------------------------------------------------------------------------------------------------*/
		case RTOS_ISP_CMD_INIT:
			runIspInitCmd(&rtos_cmdq);
		break;
		case RTOS_ISP_CMD_PRE_SOF:
		case RTOS_ISP_CMD_PRE_EOF:
		case RTOS_ISP_CMD_POST_EOF:
			runIspEventCmd(&rtos_cmdq);
		break;
		case RTOS_ISP_CMD_POST_DONE_SYNC:
		break;
		case RTOS_ISP_CMD_DEINIT:
			runIspDeinitCmd(&rtos_cmdq);
		break;
		default:
			printf("[ERROR] isp unknown cmd %d\n", rtos_cmdq.cmd_id);
			break;
		}

		if (need_ack == CVI_TRUE) {
			xQueueSend(gQueueISPCmdqu, &rtos_cmdq, 0U);
		}

		rtos_cmdq.ip_id = -1;
	}

	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;
}

// Test code
//static CVI_VOID isp_init()
//{
//	SAMPLE_VI_CONFIG_S pstViConfig;
//
//	pstViConfig.s32WorkingViNum = 1;
//	pstViConfig.as32WorkingViId[0] = 0;
//	pstViConfig.astViInfo[0].stPipeInfo.aPipe[0] = 0;
//	pstViConfig.astViInfo[0].stPipeInfo.aPipe[1] = -1;
//	pstViConfig.astViInfo[0].stPipeInfo.aPipe[2] = -1;
//	pstViConfig.astViInfo[0].stPipeInfo.aPipe[3] = -1;
//	pstViConfig.astViInfo[0].stSnsInfo.enSnsType = SONY_IMX307_MIPI_2M_30FPS_12BIT;
//
//	SAMPLE_COMM_VI_CreateIsp(&pstViConfig);
//}
