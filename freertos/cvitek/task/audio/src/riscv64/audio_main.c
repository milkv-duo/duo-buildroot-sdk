/* Standard includes. */
#include <stdio.h>
//#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "malloc.h"
#include "sleep.h"

/* cvitek includes. */
#include "printf.h"
#include "rtos_cmdqu.h"
#include "arch_helpers.h"
/* audio only */
#ifdef AUDIO_UNIT_TEST
#include "ssp_unit_test.h"
#endif
#include "cviaudio_rtos_cmd.h"
#include "cvi_comm_aio.h"
#include "comm.h"
#include "cviaudio_algo_interface.h"
#include "cviaudio_rtos_align.h"
/*define the audio print macro---------*/
#define __AUDIO_LOG_ON__
#define AUD_RTOS_LOG_LEVEL 0

#ifdef __AUDIO_LOG_ON__
#define aud_printf printf
#else
#define aud_printf(...)
#endif

#define aud_error(fmt, args...) \
	do { \
		if (AUD_RTOS_LOG_LEVEL >= 0) \
			aud_printf("[audio][rtos][error]<%s,%d> "fmt, __func__, __LINE__, ##args);\
	} while (0)

#define aud_info(fmt, args...) \
	do { \
		if (AUD_RTOS_LOG_LEVEL >= 1) \
			aud_printf("[audio][rtos][info]<%s,%d> "fmt, __func__, __LINE__, ##args);\
	} while (0)
#define aud_debug(fmt, args...) \
	do { \
		if (AUD_RTOS_LOG_LEVEL >= 2) \
			aud_printf("[rtos][v]<%s,%d> "fmt, __func__, __LINE__, ##args);\
	} while (0)

/*define the audio SSP CMD---------------end*/
QueueHandle_t xQueueAudio;
QueueHandle_t xQueueAudioCmdqu;
int unit_test_triggered = 0;
unsigned char ssp_running = 0;
void *paudio_ssp_handle = NULL;
void *paudio_ssp_spk_handle = NULL;
//for ssp parameters after ssp_init
AI_TALKVQE_CONFIG_S *pstVqeConfig = NULL;
ST_SSP_RTOS_INDICATOR *pindicator = NULL;
ST_SSP_BUFTBL	*pstSspBufTbl = NULL;
//for block mode global variables
void *paudio_ssp_block = NULL;

#define CVIAUDIO_BASIC_AEC_LENGTH 160
#ifndef CVIAUDIO_BYTES_PER_SAMPLE
#define CVIAUDIO_BYTES_PER_SAMPLE 2
#endif

#define CVIAUDIO_DUMP_PARAM 1

//#define AUDIO_MEASURE_3A_LOADING

#ifndef CVIAUDIO_CHECK_NULL
#define CVIAUDIO_CHECK_NULL(ptr)	\
	do {	\
		if (!(ptr)) {	\
			printf("func:%s,line:%d, NULL pointer\n", __func__, __LINE__);	\
		}	\
	} while (0)
#endif
// just to build & compiler venc sdk, do not need run this
__attribute__((optimize("-O0"))) void cvi_audio_init_test(void)
{

	aud_printf("Enter cvitek audio ssp algorithm test mode\n");
	if (unit_test_triggered == 1) {
		//already trigger
		return;

	} else {
		aud_printf("audio_ssp  first trigger test  begin.....!!!\n");
		aud_printf("audio_ssp  first trigger test  end!\n");
		unit_test_triggered = 1;
	}
}

static void _cviaudio_clear_cmdq(cmdqu_t *p_cmdqu)
{
	if (p_cmdqu == NULL)
		return;

	p_cmdqu->ip_id = -1;
	p_cmdqu->cmd_id = -1;
	p_cmdqu->param_ptr = -1;
}

static unsigned char _audio_task_check_ssp_proc_data_valid(ST_SSP_RTOS_INDICATOR *pIndicator)
{
	unsigned char ret = 1;
	unsigned char check_val = 1;

	if (!pIndicator) {
		aud_error("[error][rtos][audio_task]Null pt detect in proc data...\n");
		return 0;
	}

	if (pIndicator->channel_nums > 2 || pIndicator->channel_nums < 1) {
		check_val = 0;
		aud_error("[error][rtos][audio_task]channel numbers[%d] invalid\n",
			pIndicator->channel_nums);
	}

	if (pIndicator->chunks_number <= 0 ||
		pIndicator->chunks_number > CVIAUDIO_SSP_CHUNK_NUMBERS) {
		check_val = 0;
		aud_error("[error][rtos][audio_task]invalid chunks_number[%d]\n",
			pIndicator->chunks_number);
	}

	if (pIndicator->Wpt_index > CVIAUDIO_SSP_CHUNK_NUMBERS) {
		check_val = 0;
		aud_error("[error][rtos][audio_task]wpt_index invalid range[%d]\n",
			pIndicator->Wpt_index);
	}

	if (!pIndicator->ssp_on) {
		check_val = 0;
		aud_error("[error][rtos][audio_task]ssp_on invalid[%d]\n",
			pIndicator->ssp_on);
	}
	return (ret&check_val);

}
void prvAudioRunTask(void *pvParameters)
{
	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;
	cmdqu_t rtos_cmdq;
	unsigned char ret_val;
	//ST_SSP_BUFTBL	*pstSspBufTbl;//for unit test mode
	unsigned int prev_msg_counter = -1;//counter to check the package from cviaudio_core.ko to rtos
	//for spk out algorithm
	ST_SSP_RTOS_SPK_DATA_RET *pSpkSspRtosData_Ret;
	#ifdef AUDIO_UNIT_TEST
	cmdqu_t rtos_cmdq_back2Kernel;
	#endif

	#ifdef AUDIO_MEASURE_3A_LOADING
	TickType_t xTimer3AProcStart;
	TickType_t xTimer3AProcEnd;
	static int frame_count;
	static int total_diff;
	#endif

	cvi_audio_init_test();
	aud_info("pAudioRunTask run Version:20220519\n");

	xQueueAudioCmdqu = main_GetMODHandle(E_QUEUE_CMDQU);
	xQueueAudio = main_GetMODHandle(E_QUEUE_AUDIO);

	for (;;) {
		xQueueReceive(xQueueAudio, &rtos_cmdq, portMAX_DELAY);

		aud_info("prvAudioRunTask_cmd ip=%d cmd=%d para=%x\n",
			rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);
		if (rtos_cmdq.ip_id != IP_AUDIO) {
			//send back the err msg
			aud_error("invalid ip_id[%d] into AudioRunTask\n", rtos_cmdq.ip_id);
			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			goto LEAVE_AUDIO_RUN_TASK;
		}

		if (rtos_cmdq.cmd_id > CVIAUDIO_RTOS_CMD_SSP_MAX) {
			aud_error("invalid cmd[%d]...\n", rtos_cmdq.cmd_id);
			goto LEAVE_AUDIO_RUN_TASK;
		}



		//printf("rtos_cmdq.cmd_id[%d]\n", rtos_cmdq.cmd_id);
		switch (rtos_cmdq.cmd_id) {
		case CVIAUDIO_RTOS_CMD_SSP_INIT:
			{
			aud_info("CVIAUDIO_RTOS_CMD_SSP_INIT\n");
			ST_CVIAUDIO_MAILBOX *gpstCviaudioMailBox= (ST_CVIAUDIO_MAILBOX *)rtos_cmdq.param_ptr;

			inv_dcache_range((uintptr_t)gpstCviaudioMailBox, sizeof(ST_CVIAUDIO_MAILBOX));
			aud_debug("CVIAUDIO_RTOS_CMD_SSP_INIT free_rtos addrPhy[0x%x]\n", rtos_cmdq.param_ptr);
			aud_debug("magic word[0x%lx]\n", gpstCviaudioMailBox->u64RevMask);
			if (gpstCviaudioMailBox->u64RevMask == CVIAUDIO_RTOS_MAGIC_WORD_KERNEL_BIND_MODE) {
				pstVqeConfig = (AI_TALKVQE_CONFIG_S *)gpstCviaudioMailBox->AinVqeCfgPhy;
				pstSspBufTbl = (ST_SSP_BUFTBL	*)gpstCviaudioMailBox->buffertblPhy;
				pindicator = (ST_SSP_RTOS_INDICATOR *)gpstCviaudioMailBox->indicatorPhy;
				aud_debug("[SSP_INIT]show phy address vqe[0x%p] bufTbl[0x%p] indicator[0x%p]\n",
					pstVqeConfig, pstSspBufTbl, pindicator);
				inv_dcache_range((uintptr_t)gpstCviaudioMailBox->AinVqeCfgPhy,
							sizeof(AI_TALKVQE_CONFIG_S));
				inv_dcache_range((uintptr_t)gpstCviaudioMailBox->buffertblPhy,
							sizeof(ST_SSP_BUFTBL));
				inv_dcache_range((uintptr_t)gpstCviaudioMailBox->indicatorPhy,
							sizeof(ST_SSP_RTOS_INDICATOR));
				aud_debug("[SSP_INIT]inv dcache ...done!!\n");

			} else {
				aud_error("[SSP]ERROR [%s][%d]...force break\n", __func__, __LINE__);
				break;
			}
			//dump out the vqe config ------------------------------------------start
			aud_debug("SSP_INIT dump-----------------------------------------------------\n");
			aud_debug("para_client_config[%d]\n", pstVqeConfig->para_client_config);
			aud_debug("u32OpenMask[0x%x]\n", pstVqeConfig->u32OpenMask);
			aud_debug("s32WorkSampleRate[%d]\n", pstVqeConfig->s32WorkSampleRate);
			aud_debug("stAecCfg.para_aec_filter_len[%d]\n", pstVqeConfig->stAecCfg.para_aec_filter_len);
			aud_debug("stAecCfg.para_aes_std_thrd[%d]\n", pstVqeConfig->stAecCfg.para_aes_std_thrd);
			aud_debug("stAecCfg.para_aes_supp_coeff[%d]\n", pstVqeConfig->stAecCfg.para_aes_supp_coeff);
			aud_debug("SSP_INIT dump-----------------------------------------------------[end]\n");
			//dump out the vqe config -------------------------------------------end
			if (paudio_ssp_handle != NULL) {
				aud_error("Warning ssp audio handle is not NULL\n");
			}
			paudio_ssp_handle = CviAud_Algo_Init(pstVqeConfig->u32OpenMask, pstVqeConfig);
			if (paudio_ssp_handle == NULL) {
				aud_error("get null pt in init ssp\n");
			} else
				ssp_running = 1;

			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_DEINIT:
			{
			aud_info("CVIAUDIO_RTOS_CMD_SSP_DEINIT\n");
			if (paudio_ssp_handle == NULL) {
				aud_error("Error cannot de-init null pt\n");
			} else {
				CviAud_Algo_DeInit(paudio_ssp_handle);
				paudio_ssp_handle = NULL;
			}
			ssp_running = 0;
			rtos_cmdq.ip_id = IP_AUDIO;
			rtos_cmdq.cmd_id = CVIAUDIO_RTOS_CMD_SSP_DEINIT;
			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_PROCESS:
			{
			aud_info("CVIAUDIO_RTOS_CMD_SSP_PROCESS\n");
			if (paudio_ssp_handle == NULL || pstSspBufTbl == NULL) {
				aud_error("Error cannot process with NULL handle[%s][%d]\n", __func__, __LINE__);
				break;

			} else {
				do {
				ret_val = _audio_task_check_ssp_proc_data_valid(pindicator);
				if (!ret_val) {
					aud_error("Error[%s][%d]..force break!!\n", __func__, __LINE__);
					break;
				}

				//if ((pindicator->msg_counter == prev_msg_counter) && (repeat_cnt < 100)) {
				if ((pindicator->msg_counter == prev_msg_counter)) {
					aud_error("[audio_task] counter same[%d][%d]\n",
						prev_msg_counter, pindicator->msg_counter);

					inv_dcache_range((uintptr_t)pindicator, sizeof(ST_SSP_RTOS_INDICATOR));
					inv_dcache_range((uintptr_t)pstSspBufTbl,
							sizeof(ST_SSP_BUFTBL) * CVIAUDIO_SSP_CHUNK_NUMBERS);
					if (pindicator) {
						if (pindicator->chunks_number >= 5)
							vTaskDelay(pdMS_TO_TICKS(10));
						else if (pindicator->chunks_number >= 3)
							vTaskDelay(pdMS_TO_TICKS(5));
						else
							usleep(3000);
					}
				}

				//start process the data with the physical table: pstSspBufTbl
				unsigned char bCheck_input_full = 1;
				unsigned char cur_pt, proc_pkt_cnt, target_pkt_cnt, chn_num;

				proc_pkt_cnt = 0;
				target_pkt_cnt = pindicator->chunks_number;
				cur_pt = pindicator->Ppt_index;
				chn_num = pindicator->channel_nums;


				while (proc_pkt_cnt < target_pkt_cnt) {
					if (pstSspBufTbl[cur_pt].bBufOccupy != CVIAUDIO_BUF_TBL_INPUT) {
						bCheck_input_full = 0;
						inv_dcache_range((uintptr_t)pindicator, sizeof(ST_SSP_RTOS_INDICATOR));
						inv_dcache_range((uintptr_t)pstSspBufTbl,
							sizeof(ST_SSP_BUFTBL) * CVIAUDIO_SSP_CHUNK_NUMBERS);
						break;
					}
					cur_pt = (cur_pt + 1) % CVIAUDIO_SSP_CHUNK_NUMBERS;
					proc_pkt_cnt += 1;
				}
				cur_pt = pindicator->Ppt_index;
				chn_num = pindicator->channel_nums;
				proc_pkt_cnt = 0;
				while ((proc_pkt_cnt != target_pkt_cnt) && (bCheck_input_full)) {
					if (chn_num == 2 || pindicator->ssp_with_aec) {
						ret_val = CviAud_Algo_Process(paudio_ssp_handle,
								pstSspBufTbl[cur_pt].mic_in_addr,
								pstSspBufTbl[cur_pt].ref_in_addr,
								pstSspBufTbl[cur_pt].output_addr,
								CVIAUDIO_BASIC_AEC_LENGTH);
					} else {
						ret_val = CviAud_Algo_Process(paudio_ssp_handle,
								pstSspBufTbl[cur_pt].mic_in_addr,
								(uint64_t)NULL,
								pstSspBufTbl[cur_pt].output_addr,
								CVIAUDIO_BASIC_AEC_LENGTH);
					}
					if (ret_val <= 0)
						aud_error("[audio_task]CviAudio_Algo_Process error!!!\n");
					else
						pstSspBufTbl[cur_pt].bBufOccupy = CVIAUDIO_BUF_TBL_AFTER_SSP;
					proc_pkt_cnt += 1;
					cur_pt = (cur_pt + 1) % CVIAUDIO_SSP_CHUNK_NUMBERS;
					if (proc_pkt_cnt > (CVIAUDIO_SSP_CHUNK_NUMBERS + 1)) {
						aud_error("[audio_task]proc_pkt_cnt abnormal..[%d]\n",
							proc_pkt_cnt);
						break;
					}
				}
				if (bCheck_input_full) {
					pindicator->Ppt_index = cur_pt;
					prev_msg_counter = pindicator->msg_counter;
					pindicator->msg_counter += 1;
					clean_dcache_range((uintptr_t)pindicator,
						sizeof(ST_SSP_RTOS_INDICATOR));
					clean_dcache_range((uintptr_t)pstSspBufTbl,
						sizeof(ST_SSP_BUFTBL) * CVIAUDIO_SSP_CHUNK_NUMBERS);
					xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
				}
				} while ((ret_val > 0) && ssp_running);
			}
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_SPK_INIT:
			{
			//TODO: remain only mic in algo
			rtos_cmdq.cmd_id = CVIAUDIO_RTOS_CMD_SSP_SPK_INIT;
			rtos_cmdq.ip_id = IP_AUDIO;
			//rtos_cmdq.param_ptr = p_ret_status;
			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_SPK_DEINIT:
			{
			//TODO: remain only mic in algo
			rtos_cmdq.ip_id = IP_AUDIO;
			rtos_cmdq.cmd_id = CVIAUDIO_RTOS_CMD_SSP_SPK_DEINIT;
			//rtos_cmdq.param_ptr = p_ret_status;
			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_SPK_PROCESS:
			{
			//TODO: remain only mic in algo
			rtos_cmdq.ip_id = IP_AUDIO;
			rtos_cmdq.cmd_id = CVIAUDIO_RTOS_CMD_SSP_SPK_PROCESS;
			rtos_cmdq.param_ptr = pSpkSspRtosData_Ret;
			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_DEBUG:
			{
			//testing the mailbox transition
			aud_info("Entering debug mode---------------------->>\n");

			aud_info("Leaving debug mode<<----------------------\n");
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST_BLOCK_MODE_INIT:
			{
			//testing the mailbox transition
			aud_info("RTOS_CMD_SSP_UNIT_TEST_BLOCK_MODE_INIT---------------------->>\n");
			#ifdef AUDIO_UNIT_TEST
			SSP_Algorithm_Init();
			#endif
			aud_info("[RTOS]Leaving CMD_SSP_UNIT_TEST_BLOCK_MODE_INIT<<-------------->>\n");
			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			aud_info("[RTOS]Leaving\n");
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST_BLOCK_MODE_GET:
			{
			//testing the mailbox transition
			int ret = 0;
			uint64_t	output_addr = rtos_cmdq.param_ptr;

			inv_dcache_range((uintptr_t)output_addr, 320);
			aud_debug("dump 19 no critical / with o2/ with poshiun patch [0x%lx]\n", output_addr);
			#ifdef USING_CRITICAL_PROTECT
			taskENTER_CRITICAL();
			#endif
			#ifdef AUDIO_UNIT_TEST
			ret = SSP_Algorithm(1, NULL, output_addr);
			#endif
			if (ret == 0) {
				aud_info("[Audio]Unit test file go to an end....finishedxxxxxxxxxxxxxxxxxxxx\n");
				break;
			} else
				aud_info("[Rtos][Audio]Unit test file going-------------------------------------->\n");
			#ifdef USING_CRITICAL_PROTECT
			taskEXIT_CRITICAL();
			#endif
			clean_dcache_range(output_addr, 320);
			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);

			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST:
			{
				//only support for mic in AEC unit test
				aud_info("Enter CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST mode!!\n");
				aud_info("Keep sending back data to cviaudio_core irq_handler while occupied\n");
				#ifdef AUDIO_UNIT_TEST
				int ret = 0;
				unsigned char bCheck_input_full = 1;
				unsigned char index = 0;
				int count = 0;
				unsigned int counter = 0;
				#endif
				ST_SSP_RTOS_INIT *pSspRtosCfg = rtos_cmdq.param_ptr;
				aud_debug("[Rtos][Rtos space]s32RevMask[0x%x]\n",pSspRtosCfg->s32RevMask);
				pstSspBufTbl = &pSspRtosCfg->buffertbl[0];
				pSpkSspRtosData_Ret = (ST_SSP_RTOS_SPK_DATA_RET *)pSspRtosCfg->CbPhyAddr;
				aud_debug("[Rtos][Rtos_space]xxxpSspRtosCfg[0x%p]\n", pSspRtosCfg);
				#if 0
				for (index = 0; index < CVIAUDIO_SSP_CHUNK_NUMBERS; index++) {
					aud_debug("[Rtos]index[%d] occupy[%d] mic_in[0x%lx]\n",
					index,
					pSspRtosCfg->buffertbl[index].bBufOccupy,
					pSspRtosCfg->buffertbl[index].mic_in_addr);

				}
				#endif
				aud_debug("[%s][%d]CbPhyAddr[0x%x]\n",
					__func__, __LINE__,
					pSspRtosCfg->CbPhyAddr);
				aud_debug("pSpkSspRtosData_Ret[0%p]\n", pSpkSspRtosData_Ret);
				#ifdef AUDIO_UNIT_TEST
				SSP_Algorithm_Init();
				do {
					for (index = 0; index < CVIAUDIO_SSP_CHUNK_NUMBERS; index++) {
						#if 0
						aud_debug("index[%d] occupy[%d] mic_in[0x%lx]\n", index, pSspRtosCfg->buffertbl[index].bBufOccupy, pSspRtosCfg->buffertbl[index].mic_in_addr);
						#endif
						bCheck_input_full = bCheck_input_full*pSspRtosCfg->buffertbl[index].bBufOccupy;
						if (!bCheck_input_full) {
							//aud_debug("[Rtos][%s][%d]CbPhyAddr[0x%llx]xxxxxx\n", __func__, __LINE__, pSspRtosCfg->CbPhyAddr);//
							inv_dcache_range((uintptr_t)pSspRtosCfg, sizeof(ST_SSP_RTOS_INIT));
							break;
						}
					}
					if (bCheck_input_full) {

						for (index = 0; index < CVIAUDIO_SSP_CHUNK_NUMBERS; index++) {
						pstSspBufTbl[index].bBufOccupy = 0;
						ret = SSP_Algorithm(1, NULL, pstSspBufTbl[index].output_addr);
						if (ret == 0) {
							aud_info("Unit test file go to an end....finished\n");
							break;
						}
						}
						//send back to cviaudio_core.ko since all chunks been process, and the
						//occupy flag been pull to 0
						if (pSspRtosCfg->CbPhyAddr == 0)
							aud_error("[Rtos][Error]Get physical address 000000\n");
						else {

							pSpkSspRtosData_Ret->cb_command = CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST;
							pSpkSspRtosData_Ret->status = 1;
						}
						rtos_cmdq_back2Kernel.ip_id = IP_AUDIO;
						rtos_cmdq_back2Kernel.cmd_id = CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST;
						rtos_cmdq_back2Kernel.param_ptr = pSpkSspRtosData_Ret;
						rtos_cmdq_back2Kernel.resv.valid.linux_valid = 1;
						rtos_cmdq_back2Kernel.resv.valid.rtos_valid = 0;
						aud_debug("[Rtos][%s][%d]\n", __func__, __LINE__);
						aud_debug("[Rtos]rtos_cmdq[%x]\n", rtos_cmdq.block);
						aud_debug("[Rtos]rtos_cmdq linux_valid[%x]\n", rtos_cmdq.resv.valid.linux_valid);
						aud_debug("[Rtos]rtos_cmdq rtos_valid[%x]\n", rtos_cmdq.resv.valid.rtos_valid);
						aud_debug("[Rtos]rtos_cmdq[%x]\n", rtos_cmdq.param_ptr);

						aud_debug("[Rtos]rtos_cmdq_back2Kernel[%x]\n", rtos_cmdq_back2Kernel.block);
						aud_debug("[Rtos]rtos_cmdq_back2Kernel[%x]\n", rtos_cmdq_back2Kernel.ip_id);
						aud_debug("[Rtos]rtos_cmdq_back2Kernel[%x]\n", rtos_cmdq_back2Kernel.cmd_id);
						aud_debug("[Rtos]rtos_cmdq_back2Kernel[%x]\n", rtos_cmdq_back2Kernel.resv.valid.linux_valid);
						aud_debug("[Rtos]rtos_cmdq_back2Kernel[%x]\n", rtos_cmdq_back2Kernel.resv.valid.rtos_valid);
						aud_debug("[Rtos]rtos_cmdq_back2Kernel[%x]\n", rtos_cmdq_back2Kernel.param_ptr);

						aud_debug("[Rtos][%s][%d]....flush\n", __func__, __LINE__);
						clean_dcache_range(pSspRtosCfg, sizeof(ST_SSP_RTOS_INIT));
						aud_debug("[Rtos][flush_dcache_range][0x%lx][%d]....flush\n",pSspRtosCfg, sizeof(ST_SSP_RTOS_INIT));
						counter += 1;
						xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
						aud_debug("[Rtos][%s][%d]....xxxxxxxcounter[%d]\n", __func__, __LINE__, counter);
						aud_debug("[Rtos]keep waiting....\n");
					} else {
						//aud_info("[Rtos][%s][%d]index[%d]not full\n", __func__, __LINE__, index);
						//count += 1;
						//if (count % 1000000 == 0) {
						//	aud_info("[Rtos][%s][%d]out count[%d]index[%d]\n", __func__, __LINE__, count, index);
						//}
					}
					bCheck_input_full = 1; //reset the check flag


				} while (ret != 0);
				#endif
				aud_info("[Rtos]out\n");
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK:
			{
			aud_info("CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK\n");
			ST_CVIAUDIO_MAILBOX_BLOCK *pstAudBlockMailBox =
						(ST_CVIAUDIO_MAILBOX_BLOCK *)rtos_cmdq.param_ptr;
			AI_TALKVQE_CONFIG_S_RTOS *_pstVqeConfig = NULL;

			inv_dcache_range((uintptr_t)pstAudBlockMailBox, sizeof(ST_CVIAUDIO_MAILBOX_BLOCK));
			aud_debug("CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK phy[0x%x]\n", rtos_cmdq.param_ptr);
			if (pstAudBlockMailBox->u64RevMask != CVIAUDIO_RTOS_MAGIC_WORD_USERSPACE_BLOCK_MODE) {
				aud_error("[CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK]magic word mismatch[0x%lx]\n",
						pstAudBlockMailBox->u64RevMask);
				rtos_cmdq.param_ptr = CVIAUDIO_RTOS_BLOCK_MODE_FAILURE_FLAG;
				clean_dcache_range((uintptr_t)pstAudBlockMailBox, sizeof(ST_CVIAUDIO_MAILBOX_BLOCK));
				xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
				break;
			} else
				aud_debug("CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK magic word matched\n");

			_pstVqeConfig = (AI_TALKVQE_CONFIG_S_RTOS *)pstAudBlockMailBox->AinVqeCfgPhy;
			inv_dcache_range((uintptr_t)_pstVqeConfig, sizeof(AI_TALKVQE_CONFIG_S_RTOS));

			aud_info("SSP_INIT dump-----------------------------------------------------\n");
			aud_info("para_client_config[%d]\n", _pstVqeConfig->para_client_config);
			aud_info("u32OpenMask[0x%x]\n", _pstVqeConfig->u32OpenMask);
			aud_info("s32WorkSampleRate[%d]\n", _pstVqeConfig->s32WorkSampleRate);
			aud_info("stAecCfg.para_aec_filter_len[%d]\n", _pstVqeConfig->stAecCfg.para_aec_filter_len);
			aud_info("stAecCfg.para_aes_std_thrd[%d]\n", _pstVqeConfig->stAecCfg.para_aes_std_thrd);
			aud_info("stAecCfg.para_aes_supp_coeff[%d]\n", _pstVqeConfig->stAecCfg.para_aes_supp_coeff);
			aud_info("SSP_INIT dump-----------------------------------------------------[end]\n");

			AI_TALKVQE_CONFIG_S VqeConfig;
			AI_TALKVQE_CONFIG_S *pVqeConfigSsp = &VqeConfig;

			pVqeConfigSsp->para_client_config = _pstVqeConfig->para_client_config;
			pVqeConfigSsp->u32OpenMask = _pstVqeConfig->u32OpenMask;
			pVqeConfigSsp->s32WorkSampleRate = _pstVqeConfig->s32WorkSampleRate;
			pVqeConfigSsp->stAecCfg.para_aec_filter_len = _pstVqeConfig->stAecCfg.para_aec_filter_len;
			pVqeConfigSsp->stAecCfg.para_aes_std_thrd = _pstVqeConfig->stAecCfg.para_aes_std_thrd;
			pVqeConfigSsp->stAecCfg.para_aes_supp_coeff = _pstVqeConfig->stAecCfg.para_aes_supp_coeff;
			pVqeConfigSsp->stAnrCfg.para_nr_snr_coeff = _pstVqeConfig->stAnrCfg.para_nr_snr_coeff;
			pVqeConfigSsp->stAnrCfg.para_nr_init_sile_time = _pstVqeConfig->stAnrCfg.para_nr_init_sile_time;
			pVqeConfigSsp->stAgcCfg.para_agc_max_gain = _pstVqeConfig->stAgcCfg.para_agc_max_gain;
			pVqeConfigSsp->stAgcCfg.para_agc_target_high = _pstVqeConfig->stAgcCfg.para_agc_target_high;
			pVqeConfigSsp->stAgcCfg.para_agc_target_low = _pstVqeConfig->stAgcCfg.para_agc_target_low;
			pVqeConfigSsp->stAgcCfg.para_agc_vad_ena = _pstVqeConfig->stAgcCfg.para_agc_vad_ena;
			pVqeConfigSsp->stAecDelayCfg.para_aec_init_filter_len =
								_pstVqeConfig->stAecDelayCfg.para_aec_init_filter_len;
			pVqeConfigSsp->stAecDelayCfg.para_dg_target = _pstVqeConfig->stAecDelayCfg.para_dg_target;
			pVqeConfigSsp->stAecDelayCfg.para_delay_sample = _pstVqeConfig->stAecDelayCfg.para_delay_sample;
			pVqeConfigSsp->s32RevMask = _pstVqeConfig->s32RevMask;
			pVqeConfigSsp->para_notch_freq = _pstVqeConfig->para_notch_freq;


			if (paudio_ssp_block == NULL) {
				paudio_ssp_block = CviAud_Algo_Init(pVqeConfigSsp->u32OpenMask, pVqeConfigSsp);
				if (paudio_ssp_block == NULL) {
					aud_error("[CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK]paudio_ssp_block failure\n");
					rtos_cmdq.param_ptr = CVIAUDIO_RTOS_BLOCK_MODE_FAILURE_FLAG;
					clean_dcache_range((uintptr_t)pstAudBlockMailBox, sizeof(ST_CVIAUDIO_MAILBOX_BLOCK));
					xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
					break;
				} else
					aud_info("CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK init success!!\n");
			} else
				aud_error("warning paudio_ssp_blcok not Null..\n");

			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_CONFIG_BLOCK:
			{
			aud_info("CVIAUDIO_RTOS_CMD_SSP_CONFIG_BLOCK\n");
			ST_CVIAUDIO_MAILBOX_BLOCK *pstAudBlockMailBox =
						(ST_CVIAUDIO_MAILBOX_BLOCK *)rtos_cmdq.param_ptr;
			AI_TALKVQE_CONFIG_S_RTOS *_pstVqeConfig = NULL;

			inv_dcache_range((uintptr_t)pstAudBlockMailBox, sizeof(ST_CVIAUDIO_MAILBOX_BLOCK));
			aud_debug("CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK phy[0x%x]\n", rtos_cmdq.param_ptr);
			if (pstAudBlockMailBox->u64RevMask != CVIAUDIO_RTOS_MAGIC_WORD_USERSPACE_BLOCK_MODE) {
				aud_error("[CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK]magic word mismatch[0x%lx]\n",
						pstAudBlockMailBox->u64RevMask);
				rtos_cmdq.param_ptr = CVIAUDIO_RTOS_BLOCK_MODE_FAILURE_FLAG;
				clean_dcache_range((uintptr_t)pstAudBlockMailBox, sizeof(ST_CVIAUDIO_MAILBOX_BLOCK));
				xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
				break;
			} else
				aud_debug("CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK magic word matched\n");

			_pstVqeConfig = (AI_TALKVQE_CONFIG_S_RTOS *)pstAudBlockMailBox->AinVqeCfgPhy;
			inv_dcache_range((uintptr_t)_pstVqeConfig, sizeof(AI_TALKVQE_CONFIG_S_RTOS));

			aud_debug("u32OpenMask[0x%x]\n", _pstVqeConfig->u32OpenMask);

			if (paudio_ssp_block != NULL) {
				CviAud_Algo_Fun_Config(paudio_ssp_block, _pstVqeConfig->u32OpenMask);
				aud_info("CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK init success!!\n");
			} else
				aud_error("warning paudio_ssp_blcok not Null..\n");

			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_DEINIT_BLOCK:
			{
			aud_info("CVIAUDIO_RTOS_CMD_SSP_DEINIT_BLOCK\n");
			if (paudio_ssp_block) {
				CviAud_Algo_DeInit(paudio_ssp_block);
				paudio_ssp_block = NULL;
			} else
				aud_error("Error cannot de-init null pt\n");
			aud_info("CVIAUDIO_RTOS_CMD_SSP_DEINIT_BLOCK\n");
			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			aud_info("[RTOS]Leaving\n");
			}
			break;
		case CVIAUDIO_RTOS_CMD_SSP_PROC_BLOCK:
			{
			aud_debug("CVIAUDIO_RTOS_CMD_SSP_PROC_BLOCK\n");
			int ret = 1;
			//short *pvincent;//[v]

			if (!paudio_ssp_block) {
				aud_error("Error cannot proc with null paudio_ssp_block\n");
				rtos_cmdq.param_ptr = CVIAUDIO_RTOS_BLOCK_MODE_FAILURE_FLAG;
				xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
				break;
			}
			ST_CVIAUDIO_BLOCK_FRAME *pstBlockFrm = (ST_CVIAUDIO_BLOCK_FRAME *)rtos_cmdq.param_ptr;

			inv_dcache_range((uintptr_t)pstBlockFrm, sizeof(ST_CVIAUDIO_BLOCK_FRAME));
			inv_dcache_range((uintptr_t)pstBlockFrm->mic_in_addr, 1280);
			inv_dcache_range((uintptr_t)pstBlockFrm->output_addr, 1280);
			inv_dcache_range((uintptr_t)pstBlockFrm->ref_in_addr, 1280);
			if (pstBlockFrm->u64RevMask != CVIAUDIO_RTOS_MAGIC_WORD_USERSPACE_BLOCK_MODE) {
				aud_error("[CVIAUDIO_RTOS_CMD_SSP_PROC_BLOCK]magic word mismatch[0x%lx]\n",
						pstBlockFrm->u64RevMask);
				rtos_cmdq.param_ptr = CVIAUDIO_RTOS_BLOCK_MODE_FAILURE_FLAG;
				clean_dcache_range((uintptr_t)pstBlockFrm, sizeof(ST_CVIAUDIO_BLOCK_FRAME));
				xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
				break;

			} else {
				#ifdef AUDIO_MEASURE_3A_LOADING
				xTimer3AProcStart = xTaskGetTickCount();
				#endif
				ret = CviAud_Algo_Process(paudio_ssp_block,
						pstBlockFrm->mic_in_addr,
						pstBlockFrm->ref_in_addr,
						pstBlockFrm->output_addr,
						CVIAUDIO_BASIC_AEC_LENGTH);
				if (ret < 0) {
					aud_error("Error in CviAud_Algo_Process\n");
					rtos_cmdq.param_ptr = CVIAUDIO_RTOS_BLOCK_MODE_FAILURE_FLAG;
				}
				#ifdef AUDIO_MEASURE_3A_LOADING
				//only measure 8K, not support 16K
				frame_count++;
				xTimer3AProcEnd = xTaskGetTickCount();
				total_diff += xTimer3AProcEnd - xTimer3AProcStart;
				if (frame_count == 8000/CVIAUDIO_BASIC_AEC_LENGTH*3) {
					aud_info("total_diff = %ld, cpu = %ld, portTICK_PERIOD_MS=%d\n",
							total_diff,
							total_diff*portTICK_PERIOD_MS/30,
							portTICK_PERIOD_MS);
					frame_count = 0;
					total_diff = 0;
				}
				#endif
			}
			//pvincent = (short *)pstBlockFrm->output_addr;
			//aud_info("CVIAUDIO_RTOS_CMD_SSP_PROC_BLOCK---success[0x%x][0x%x][0x%x]\n", pvincent[0], pvincent[1], pvincent[2]);
			clean_dcache_range((uintptr_t)pstBlockFrm, sizeof(ST_CVIAUDIO_BLOCK_FRAME));
			clean_dcache_range((uintptr_t)pstBlockFrm->mic_in_addr, 1280);
			clean_dcache_range((uintptr_t)pstBlockFrm->output_addr, 1280);
			clean_dcache_range((uintptr_t)pstBlockFrm->ref_in_addr, 1280);
			xQueueSend(xQueueAudioCmdqu, &rtos_cmdq, 0U);
			aud_info("[RTOS]Leaving\n");
			}
			break;
		default:
			aud_error("[error][rtos][audio_task]unrecognized cmd error[%d]\n", rtos_cmdq.cmd_id);
			break;

		}

LEAVE_AUDIO_RUN_TASK:
		//after parsing and operating audio command, return to linux kernel the status or pointer
		/* send cmd back to Cmdqu task and send mailbox to linux */

		//clear_reset the command q old data
		_cviaudio_clear_cmdq(&rtos_cmdq);

	}
}
