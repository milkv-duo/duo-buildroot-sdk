#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "cvi_aac_dec.h"
#include "cvi_audio.h"

#define CVI_AAC_CB_STATUS_START 0x2
#define CVI_AAC_CB_STATUS_SEND_DATA 0x3
#define CVI_AAC_CB_CHANGE_INFO 0x4


AUDIO_FRAME_S gstFrame;
char *pCbBuffer;

void dumpdata(char *filename, char *buffer, unsigned int len_byte)
{
	if (!filename) {
		return;
	}
	FILE *fp = fopen(filename, "ab");

	if (!fp) {
		return;
	}
	fwrite(buffer, 1, len_byte, fp);
	fclose(fp);
}
static int _stop_cvi_ao(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_AO_DisableChn(0, 0);
	s32Ret = CVI_AO_Disable(0);
	if (s32Ret != CVI_SUCCESS)
		printf("CVI_AO_Disable failed with %#x!\n", s32Ret);

	CVI_AUDIO_DEINIT();
	return s32Ret;

}
static int _send_cvi_ao(int channel_cnt, char *pBuff, int size)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	gstFrame.u64VirAddr[0] = (unsigned char *)pBuff;
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

	CVI_AUDIO_INIT();
	AudoutAttr.u32ChnCnt = channel_cnt;
	AudoutAttr.enSoundmode	  = (AUDIO_SOUND_MODE_E)(channel_cnt - 1);
	AudoutAttr.enSamplerate =
		(AUDIO_SAMPLE_RATE_E)sample_rate;
	AudoutAttr.enWorkmode	  = AIO_MODE_I2S_MASTER;
	AudoutAttr.u32EXFlag	  = 0;
	AudoutAttr.u32FrmNum	  = 10; /* only use in bind mode */
	AudoutAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	AudoutAttr.u32PtNumPerFrm = 480;
	AudoutAttr.u32ClkSel	  = 0;
	AudoutAttr.enI2sType = AIO_I2STYPE_INNERCODEC;
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
	CVI_AO_SetVolume(0, 9);
	CVI_AO_SetMute(0, CVI_FALSE, NULL);


	return s32Ret;
}




static int _aac_dec_callback(void *paac_handle, ST_AAC_DEC_INFO *aac_info,
				 char *pBuff, int byte)
{

	int Ret = 0;

	if (aac_info->cbState == CVI_AAC_CB_STATUS_START) {//ao init

		printf("========================================\n");
		printf("[channel]:[%d]\n", aac_info->channel_num);
		printf("[sample rate]:[%d]\n", aac_info->sample_rate);
		printf("[callback state] [0x%x]\n", aac_info->cbState);
		printf("[one aac data byte include ADTS]:[%d]\n", aac_info->frame_byte);
		printf("=========================================\n");

		Ret = _start_cvi_ao(aac_info->channel_num, aac_info->sample_rate);
		if (Ret != 0)
			printf("[Error]_start_cvi_ao failure\n");

		memcpy(pCbBuffer, pBuff, byte);
		Ret = _send_cvi_ao(aac_info->channel_num, pCbBuffer, byte);
		if (Ret != 0)
			printf("[Error]_send_cvi_ao failure\n");

		aac_info->cbState = CVI_AAC_CB_STATUS_SEND_DATA;

	} else if (aac_info->cbState == CVI_AAC_CB_STATUS_SEND_DATA) {

		memcpy(pCbBuffer, pBuff, byte);
		Ret = _send_cvi_ao(aac_info->channel_num, pCbBuffer, byte);
		if (Ret != 0)
			printf("[Error]_send_cvi_ao failure\n");

	}

	// FILE *fp_local;
	// fp_local = fopen("aac_dec_cbDump.pcm", "ab+");
	// fwrite(pBuff, 1, byte, fp_local);
	// fclose(fp_local);

	return 0;
}

int main(int argc, char *argv[])
{

	void *paac_dec_handle;
	FILE *input_fp, *output_fp;
	char *filename, *out_filename;
	int Ret, output_malloc_size, i;
	int len;

	if (argc < 3) {
		printf("use : aac_decode_demo input.aac output.aac.pcm\n");
		return -1;
	}

	filename = argv[1];
	out_filename = argv[2];

	input_fp = fopen(filename, "rb");
	output_fp = fopen(out_filename, "wb");

	paac_dec_handle = CVI_AAC_Decode_Init(CVI_DECODE_AAC_TYPE, input_fp);


	output_malloc_size = 8 * sizeof(INT_PCM) * 2048 * 50;
	unsigned char *pInputBuf = (unsigned char *)malloc(1024 * 10);
	INT_PCM *decode_buf = (INT_PCM *)malloc(output_malloc_size);


	pCbBuffer = (char *)malloc(1024 * 4 * 2);
	memset(pInputBuf, 0, 1024 * 10);
	memset(decode_buf, 0, output_malloc_size);

	Ret = CVI_AAC_Decode_InstallCb(paac_dec_handle, _aac_dec_callback);

	while (1) {

		unsigned int frame_byte = 1024 * 2; //16bit,1024sample

		unsigned int valid;
		int outframe_size = 0;

		len = fread(pInputBuf, 1, frame_byte, input_fp);
		if (len < frame_byte) {
			printf("last data len = %d\n", len);
		}
		if (len == 0) {
			printf("decode finally\n");
			break;
		}
		valid = len;
		Ret = CVI_AAC_Decode(paac_dec_handle, pInputBuf, decode_buf, valid,
					 &outframe_size);
		if (Ret != 0) {
			printf("aac decode faile\n");
			break;
		}

		fwrite(decode_buf, 1, outframe_size * 2, output_fp);
	}

	free(pInputBuf);
	free(decode_buf);
	fclose(input_fp);
	fclose(output_fp);

	CVI_AAC_Decode_Dinit(paac_dec_handle);
	CVI_AUDIO_DEINIT();

	return 0;
}


