
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include "cvi_audio.h"
#include "cvi_mp3_decode.h"

#define SAVE_THROUGH_MP3_DEC_CALLBACK 1
#ifndef BYTES_PER_SAMPLE_IN_AUDIO
#define BYTES_PER_SAMPLE_IN_AUDIO 2
#endif

#define MIN_AUDIO_PERIDO_SIZE 160
char *pCbBuffer;
AUDIO_FRAME_S gstFrame;
ST_MP3_DEC_INFO stFilePlayMpeDecInfo;

int changePeriodSize;

static int cvimp3_decode_usage(void)
{
	printf("==================================================\n");
	printf("cvi_mp3player usage\n");
	printf("cvi_mp3player $(var1).mp3\n");
	printf("create mp3_output.pcm file after decoded complete\n");
	printf("play out the mp3  through cvitek audio out modules\n");
	printf("==================================================\n");
	return 0;
}

static int _stop_cvi_ao(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_AO_DisableChn(0, 0);
	s32Ret = CVI_AO_Disable(0);
	if (s32Ret != CVI_SUCCESS)
		printf("CVI_AO_Disable failed with %#x!\n", s32Ret);

	return s32Ret;

}

static int _send_cvi_ao(int channel_cnt, char *pBuff, int size)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	gstFrame.u64VirAddr[0] = (CVI_U8  *)pBuff;
	gstFrame.u32Len = size / (channel_cnt * 2);//samples size for each channel
	gstFrame.u64TimeStamp = 0;
	gstFrame.enSoundmode = channel_cnt - 1;
	gstFrame.enBitwidth = AUDIO_BIT_WIDTH_16;

	s32Ret = CVI_AO_SendFrame(0, 0, (const AUDIO_FRAME_S *)&gstFrame, 1000);
	if (s32Ret != CVI_SUCCESS)
		printf("CVI_AO_SendFrame failed with %#x!\n", s32Ret);

	return s32Ret;
}

static int _start_cvi_ao(int channel_cnt, int sample_rate)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	AIO_ATTR_S AudoutAttr;

	AudoutAttr.u32ChnCnt = channel_cnt;
	AudoutAttr.enSoundmode	  = (AUDIO_SOUND_MODE_E)(channel_cnt-1);
	AudoutAttr.enSamplerate =
		(AUDIO_SAMPLE_RATE_E)sample_rate;
	AudoutAttr.enWorkmode	  = AIO_MODE_I2S_MASTER;
	AudoutAttr.u32EXFlag	  = 0;
	AudoutAttr.u32FrmNum	  = 10; /* only use in bind mode */
	AudoutAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	if (changePeriodSize == 0)
		AudoutAttr.u32PtNumPerFrm = 480;
	else {
		printf("Input file size is very small..filemode[save the dec out file]\n");
		AudoutAttr.u32PtNumPerFrm = changePeriodSize;
	}
	AudoutAttr.u32ClkSel	  = 0;
	AudoutAttr.enI2sType = AIO_I2STYPE_INNERCODEC;
	CVI_AO_SetVolume(0, 9);

	s32Ret |= CVI_AO_SetPubAttr(0, &AudoutAttr);
	s32Ret |= CVI_AO_Enable(0);
	s32Ret |= CVI_AO_EnableChn(0, 0);
	if (s32Ret == CVI_FAILURE) {
		printf("[Error]rate[%d] chn[%d] enable aout chn failure\n",
			sample_rate, channel_cnt);
		printf("[Error]in downlink audio setup\n");
		s32Ret = CVI_FAILURE;

	} else {
		printf("AO Enable with chn_cnt[%d] sample_rate[%d] ..success\n",
			AudoutAttr.u32ChnCnt, sample_rate);
		s32Ret = CVI_SUCCESS;
	}
	printf("set volume 9 , do the unmute\n");




	return s32Ret;
}

//callback function should exist in sample code
static int _mp3_dec_callback_forAppSmallFilePlay(void *inst, ST_MP3_DEC_INFO *pMp3_DecInfo, char *pBuff, int size)
{

	if (inst == NULL) {
		printf("Null pt [%s][%d]\n", __func__, __LINE__);
		printf("Null pt in callback function force return\n");
		return -1;
	}

	if (pMp3_DecInfo == NULL) {
		printf("Null pt [%s][%d]\n", __func__, __LINE__);
		printf("Null pt in callback pMp3_DecInfo force return\n");
		return -1;
	}

	if (pBuff == NULL) {
		printf("Null pt [%s][%d]\n", __func__, __LINE__);
		printf("Null pt in callback pBuff force return\n");
		return -1;
	}

	//save the data or play out to AO
	if (pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_GET_FIRST_INFO) {
		printf("stage:CVI_MP3_DEC_CB_STATUS_GET_FIRST_INFO\n");
		printf("========================================\n");
		printf("[channel]:[%d]\n", pMp3_DecInfo->channel_num);
		printf("[sample rate]:[%d]\n", pMp3_DecInfo->sample_rate);
		printf("[bit rate]:[%d]\n", pMp3_DecInfo->bit_rate);
		printf("[frame cnt]:[%d]\n", pMp3_DecInfo->frame_cnt);
		printf("[callback state] [0x%x]\n", pMp3_DecInfo->cbState);
		printf("[size in bytes]:[%d]\n", size);
		printf("=========================================\n");
		//create ao channel
		memcpy(&stFilePlayMpeDecInfo, pMp3_DecInfo, sizeof(ST_MP3_DEC_INFO));
		printf("[%s][%d] get the dec header info\n",  __func__, __LINE__);
	} else if (pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_STABLE) {
		//dont send file in small file mode
	} else if (pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_SMP_RATE_CHG ||
		pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_INFO_CHG) {
		printf("stage:CVI_MP3_DEC_CB_STATUS_INFO_CHG\n");
		printf("new chn[%d]\n", pMp3_DecInfo->channel_num);
		printf("new rate[%d]\n", pMp3_DecInfo->sample_rate);
		printf("[%s][%d] get the dec header info\n",  __func__, __LINE__);
		memcpy(&stFilePlayMpeDecInfo, pMp3_DecInfo, sizeof(ST_MP3_DEC_INFO));
	} else if (pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_INIT) {
		printf("stage:CVI_MP3_DEC_CB_STATUS_INIT\n");
		printf("Not decode any mp3 frame info yet........\n");
	}
	return 0;
}


//callback function should exist in sample code
static int _mp3_dec_callback_forApp(void *inst, ST_MP3_DEC_INFO *pMp3_DecInfo, char *pBuff, int size)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (inst == NULL) {
		printf("Null pt [%s][%d]\n", __func__, __LINE__);
		printf("Null pt in callback function force return\n");
		return -1;
	}

	if (pMp3_DecInfo == NULL) {
		printf("Null pt [%s][%d]\n", __func__, __LINE__);
		printf("Null pt in callback pMp3_DecInfo force return\n");
		return -1;
	}

	if (pBuff == NULL) {
		printf("Null pt [%s][%d]\n", __func__, __LINE__);
		printf("Null pt in callback pBuff force return\n");
		return -1;
	}

	//save the data or play out to AO

	if (pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_GET_FIRST_INFO) {
		printf("stage:CVI_MP3_DEC_CB_STATUS_GET_FIRST_INFO\n");
		printf("========================================\n");
		printf("[channel]:[%d]\n", pMp3_DecInfo->channel_num);
		printf("[sample rate]:[%d]\n", pMp3_DecInfo->sample_rate);
		printf("[bit rate]:[%d]\n", pMp3_DecInfo->bit_rate);
		printf("[frame cnt]:[%d]\n", pMp3_DecInfo->frame_cnt);
		printf("[callback state] [0x%x]\n", pMp3_DecInfo->cbState);
		printf("[size in bytes]:[%d]\n", size);
		printf("=========================================\n");
		//create ao channel
		s32Ret = _start_cvi_ao(pMp3_DecInfo->channel_num, pMp3_DecInfo->sample_rate);
		if (s32Ret != CVI_SUCCESS)
			printf("[Error]_start_cvi_ao failure\n");
		if (size != 0) {
			printf("CVI_MP3_DEC_CB_STATUS_GET_FIRST_INFO frm[%d]\n", size);
		} else {
			printf("CVI_MP3_DEC_CB_STATUS_GET_FIRST_INFO ------>size 0\n");
		}


	} else if (pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_STABLE) {
		//send the data to ao channel
		memcpy(pCbBuffer, pBuff, size);
		s32Ret = _send_cvi_ao(pMp3_DecInfo->channel_num, pCbBuffer, size);
		if (s32Ret != CVI_SUCCESS)
			printf("[Error]_send_cvi_ao failure\n");
	} else if (pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_SMP_RATE_CHG ||
		pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_INFO_CHG) {
		printf("stage:CVI_MP3_DEC_CB_STATUS_INFO_CHG\n");
		printf("new chn[%d]\n", pMp3_DecInfo->channel_num);
		printf("new rate[%d]\n", pMp3_DecInfo->sample_rate);
		//close ao , recreate ao channel
		s32Ret = _stop_cvi_ao();
		if (s32Ret != CVI_SUCCESS) {
			printf("[Error]Info change stop failure[%s][%d]\n", __func__, __LINE__);
			printf("[Error]IN RESTART _stop_cvi_ao failure\n");
		}

		s32Ret = _start_cvi_ao(pMp3_DecInfo->channel_num, pMp3_DecInfo->sample_rate);
		if (s32Ret != CVI_SUCCESS) {
			printf("[Error]Info change restart failure[%s][%d]\n", __func__, __LINE__);
			printf("[Error]IN RESTART _start_cvi_ao failure\n");
		} else {
			printf("Info change, retrigger AO channel success\n");
			printf("New sample rate[%d] channel[%d]\n", pMp3_DecInfo->sample_rate,
								pMp3_DecInfo->channel_num);
		}
	} else if (pMp3_DecInfo->cbState == CVI_MP3_DEC_CB_STATUS_INIT) {
		printf("stage:CVI_MP3_DEC_CB_STATUS_INIT\n");
		printf("Not decode any mp3 frame info yet........\n");
	}
	return 0;
}

static int _small_file_padding(int count)
{
	CVI_S32 s32PadCnt = count;
	CVI_S32 s32FrameBytes = stFilePlayMpeDecInfo.channel_num * MIN_AUDIO_PERIDO_SIZE * 2;
	char *zero_buffer = malloc(s32FrameBytes);
	AUDIO_FRAME_S stFrame;

	memset(zero_buffer, 0, s32FrameBytes);
	stFrame.u64VirAddr[0] = (CVI_U8  *)zero_buffer;
	stFrame.u32Len = MIN_AUDIO_PERIDO_SIZE;//samples size for each channel
	stFrame.u64TimeStamp = 0;
	stFrame.enSoundmode = stFilePlayMpeDecInfo.channel_num - 1;
	stFrame.enBitwidth = AUDIO_BIT_WIDTH_16;
	do {
		CVI_AO_SendFrame(0, 0, (const AUDIO_FRAME_S *)&stFrame, 1000);
	} while (s32PadCnt--);

	if (zero_buffer != NULL)
		free(zero_buffer);
	return s32PadCnt;
}


int main(int argc, char *argv[])
{
#define FILE_NAME_LEN	128
	char *input_filename;
	char output_filename[FILE_NAME_LEN] = {0};
	unsigned char *input_buffer = NULL;
	unsigned char *output_buffer = NULL;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 s32OutLen;
	int file_read_size = 1024;
	FILE *pfd_in = NULL;
	FILE *pfd_out = NULL;

	if (argc < 2) {
		printf("Not enough input\n");
		cvimp3_decode_usage();
		return 0;
	}

	//step1:start init
	void *pMp3DecHandler;

	int read_size = 0;
	//step2:open file
	int filecnt = 1;

	CVI_AUDIO_INIT();
	//CVI_AO_SetMute(0, CVI_TRUE, NULL);
	CVI_AO_SetMute(0, CVI_FALSE, NULL);

	for (filecnt = 1; filecnt <= (argc-1); filecnt++) {

		pMp3DecHandler = CVI_MP3_Decode_Init(NULL);
		if (pMp3DecHandler == NULL) {
			printf("[error][%s][%d]\n", __func__, __LINE__);
			return -1;
		}


		input_filename = argv[filecnt];
		if (access(input_filename, 0) <  0) {
			printf("[Error]check file[%s] not exist!....error\n", input_filename);
			return (-1);

		} else
			printf("Play mp3:[%s]\n", input_filename);


		snprintf(output_filename, FILE_NAME_LEN, "/tmp/mp3dec_out.pcm");
		pfd_in = fopen(input_filename, "rb");

		fseek(pfd_in, 0, SEEK_END);
		int fsize = ftell(pfd_in);

		fseek(pfd_in, 0, SEEK_SET);
		printf("input file total size bytesxxxxxx[%d]\n", fsize);
		if (fsize < 1024) {
			printf("---small input file do not need callback func\n");
			file_read_size = 90;
			changePeriodSize = MIN_AUDIO_PERIDO_SIZE;//save dec file first ..then play the pcm file
			s32Ret = CVI_MP3_Decode_InstallCb(pMp3DecHandler, _mp3_dec_callback_forAppSmallFilePlay);
			pfd_out = fopen(output_filename, "wb");
		} else {
			printf("---large input file: using callback func to playout\n");
			file_read_size = 1024;
			changePeriodSize = 0;//using callback to push data to aout
			s32Ret = CVI_MP3_Decode_InstallCb(pMp3DecHandler, _mp3_dec_callback_forApp);
		}
		input_buffer = (unsigned char *)malloc(fsize);
		output_buffer = (unsigned char *)malloc(fsize * 8);
		pCbBuffer = (char *)malloc(1152 * 4 * 2);
		printf("read each size[%d] from file-------------->\n", file_read_size);
		while (1) {
			read_size = fread(input_buffer, 1, file_read_size, pfd_in);
			if (read_size == 0) {
				printf("read file end ...break\n");
				break;
			}

			s32Ret = CVI_MP3_Decode((CVI_VOID *)pMp3DecHandler,
						(CVI_VOID *)input_buffer,
						(CVI_VOID *)output_buffer,
						(CVI_S32)read_size,
						(CVI_S32 *)&s32OutLen);

			if (s32Ret != CVI_SUCCESS)
				printf("[error][%s][%d]\n", __func__, __LINE__);

			if (changePeriodSize == MIN_AUDIO_PERIDO_SIZE) {
				if (pfd_out != NULL) {
					//printf("output length s32OutLen[%d]\n", s32OutLen);
					if (s32OutLen  > fsize * 8) {
						printf("Error outbuf size is not enough [%d] < [%d]\n",
							fsize * 8, s32OutLen);
					}
					fwrite(output_buffer, 1, (s32OutLen), pfd_out);
				}
			}
		}

		if (pCbBuffer != NULL) {
			free(pCbBuffer);
			pCbBuffer = NULL;
		}

		if (pfd_in != NULL) {
			fclose(pfd_in);
			pfd_in = NULL;
		}

		if (pfd_out != NULL) {
			fclose(pfd_out);
			pfd_out = NULL;
		}
		printf("read each size[%d] from file-------------->end\n", file_read_size);

		if (input_buffer != NULL) {
			free(input_buffer);
			input_buffer = NULL;
		}

		if (output_buffer != NULL) {
			free(output_buffer);
			output_buffer = NULL;
		}


		//play the small file, do not need using callback to play small size file
		if (changePeriodSize != 0) {
			FILE *fp_play = NULL;

			fp_play = fopen("/tmp/mp3dec_out.pcm", "rb");
			CVI_S32 s32FrameBytes = stFilePlayMpeDecInfo.channel_num * changePeriodSize * 2;
			char *pBuffer = malloc(s32FrameBytes);
			int num_readbytes = 0;
			AUDIO_FRAME_S stFrame;

			memset(pBuffer, 0, s32FrameBytes);

			s32Ret = _start_cvi_ao(stFilePlayMpeDecInfo.channel_num, stFilePlayMpeDecInfo.sample_rate);
			if (s32Ret != CVI_SUCCESS)
				printf("[Error]_start_cvi_ao failure\n");

			_small_file_padding(3);
			while (1) {
				memset(pBuffer, 0, s32FrameBytes);
				num_readbytes = fread(pBuffer, 1, s32FrameBytes, fp_play);
				if (num_readbytes > 0) {
					stFrame.u64VirAddr[0] = (CVI_U8  *)pBuffer;
					stFrame.u32Len = changePeriodSize;//samples size for each channel
					stFrame.u64TimeStamp = 0;
					stFrame.enSoundmode = stFilePlayMpeDecInfo.channel_num - 1;
					stFrame.enBitwidth = AUDIO_BIT_WIDTH_16;
					//printf("ao send u32len[%d]\n", stFrame.u32Len);
					s32Ret = CVI_AO_SendFrame(0, 0, (const AUDIO_FRAME_S *)&stFrame, 1000);
					if (s32Ret != CVI_SUCCESS)
						printf("CVI_AO_SendFrame failed with %#x!\n", s32Ret);
				} else {
					printf("num_framebytes %d.\n", num_readbytes);
					//padding 0 buffer
					_small_file_padding(5);
					break;
				}
			}

			if (pBuffer != NULL)
				free(pBuffer);
			if (fp_play)
				fclose(fp_play);
		} else {
			//play the decode pcm in mp3 callback
		}

		CVI_MP3_Decode_DeInit(pMp3DecHandler);
		_stop_cvi_ao();
	}

	CVI_AUDIO_DEINIT();
	printf("Exit program!!!\n");
	return 0;
}


