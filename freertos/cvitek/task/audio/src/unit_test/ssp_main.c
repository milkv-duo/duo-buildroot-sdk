/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *%
 *%       ssp_main.c
 *%       Author: Sharon Lee
 *%       History:
 *%                       Created by Sharon Lee for NR in August, 2019
 *%                       Add AGC by Sharon Lee in December, 2019
 *%                       Add Linear Processing AEC by Sharon Lee in April, 2020
 *%                       Add Nonlinear Processing AES by Sharon Lee in August, 2020
 *%                       Add Notch Filter and DC Filter by Sharon Lee in April, 2021
 *%                       Add SPK-path AGC by Sharon Lee in July, 2021
 *%                       Add DG and Delay by Sharon Lee in November, 2021
 *%			   Add SPK-path Equalizer by Sharon Lee in January, 2022
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*************************************************************************************/
/* This example C main file shows how to call entry-point functions and read input signals. 	   */
/* You must customize this file for your development environment/platform. 				   */
/* Modify it and integrate it into your own development environment/platform.                          	   */
/*                                                                       									   				   */
/*************************************************************************************/

/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tmwtypes.h"
#include "mmse_init.h"
#include "mmse.h"
#include "define.h"
#include "struct.h"
#include "agc_init.h"
#include "agc.h"
#include "lpaec.h"
#include "packfft.h"
#include "nlpaes.h"
#include "notch.h"
#include "dc.h"
#include "dg.h"
#include "delay.h"
#include "eq.h"
#include "functrl.h"
#include "ssp_unit_test.h"
#include "incbin.h"

#define INPUT_FILE_AS_ARG 1
//#define RTOS_CVIAUDIO_DEBUG_MODE 1
#ifdef RTOS_CVIAUDIO_DEBUG_MODE
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "rtos_cmdqu.h"
#endif


int customer_option;
INCBIN(FE_WAVE_FILE, fe_wave_input_file)
INCBIN(NE_WAVE_FILE, ne_wave_input_file)

#ifdef RTOS_CVIAUDIO_DEBUG_MODE
extern QueueHandle_t xQueueCmdqu;
#endif


extern unsigned char fe_wave_input_file[];
extern unsigned char ne_wave_input_file[];
extern unsigned char fe_wave_input_file_end[];
extern unsigned char ne_wave_input_file_end[];
unsigned int fe_wave_input_step = 0;
unsigned int ne_wave_input_step = 0;
unsigned int fe_wave_input_size = 0;
unsigned int ne_wave_input_size = 0;

short *pfe = NULL;
short *pne = NULL;

#define CVIAUDIO_SSP_MAIN_UNIT_TEST_MODE 1
#define CVIAUDIO_SSP_MAIN_FRAME_MODE 0

unsigned int count_bytes = 0;
//replace the local to global variable------------------------------------------------------------------------------->
int return_value = 0;
float fs;    /* sampling rate */
int hopsize = 160;    /* input hop size */
int len;    /* window size */
int len1;    /* overlap size */
int nFFT;
/* Delay History Buffer */
DelayState *delay_state = NULL;

/* DG History Buffer */
static dg_struct dg_obj;

/* DC Filter History Buffer */
static dcfilter_struct dc_obj;

/* Notch Filter History Buffer */
static qfilter_struct notch_obj;
static int notch_state[4];

/* LP AEC History Buffer */
LinearEchoState *aec_state = NULL;
short filter_length, init_filter_length;

/* NLP AES History Buffer */
NonLinearEchoState *aes_state = NULL;
boolean_T st_vad;
float std_thrd;
float aes_supp_coef1;
float aes_supp_coef2;
/* NR History Buffer */
short x_old[160];    /* input overlap buffer */
NRState *nr_state = NULL;
boolean_T speech_vad;
float aa, mu;
float frame;
float initial_noise_pow_time;
int nenr_silence_time, nenr_silence_cnt;

/* AGC History Buffer */
static agc_struct agc_obj;

/* Near-end Input/Output Buffer */
short save_mic_in[320];
short ne_mic_fixed_in[320];        /* near-end and fixed-point mic input */
float ne_mic_float_in[320];          /* near-end and floating-point mic input */
short fe_ref_fixed_in[320];          /* far-end and fixed-point ref input */
float ne_float_out[160];                 /* near-end and floating-point output */
short ne_fixed_out[160];              /* near-end and fixed-point output */
float tmp;

/* For Test */
short wav_header[44];
short cond1, cond2, cond3, cond4, cond5, cond6;
int i;
static ssp_para_struct ssp_para_obj;
/* Far-end Input/Output Buffer */
short fe_fixed_in[160];                  /* far-end and fixed-point input */
short fe_fixed_out[160];               /* far-end and fixed-point output */

/* AGC History Buffer in SPK Path */
static agc_struct spk_agc_obj;

/* EQ History Buffer in SPK Path */
static cascaded_iir_struct spk_eq_obj;
static float spk_eq_state[12];

short range = 0;
float mut_val = 1.0F;
int SSP_Algorithm_Init(void)
{
  printf("[Rtos]enter[%s]---------------->\n", __func__);

  customer_option = 3;// ht slave
  hopsize = 160;    /* input hop size */
  len = 2*hopsize;    /* window size */
  len1 = len/2;    /* overlap size */
  nFFT = 2*len;
  printf("pre-rest -----------------go\n");
    if( aec_state != NULL)
	LP_AEC_free(aec_state);
    if (aec_state != NULL)
      NLP_AES_free(aes_state);
    if (delay_state != NULL)
      delay_free(delay_state);

  printf("reset the global variable----------------------start\n");
  delay_state = NULL;
  memset(&dg_obj, 0, sizeof(dg_struct));
  memset(&dc_obj, 0, sizeof(dcfilter_struct));
  memset(&notch_obj, 0, sizeof(qfilter_struct));
  memset(notch_state, 0,sizeof(int)*4);
  if (!aec_state)
      memset(aec_state, 0, sizeof(LinearEchoState));
  aec_state = NULL;
  filter_length = 0;
  init_filter_length = 0;
  aes_state =NULL;
  st_vad = 0;
  std_thrd = 0;
  aes_supp_coef1 = 0;
  aes_supp_coef2 = 0;
  memset(x_old, 0, sizeof(short) * 160);
  speech_vad = 0;
  frame = 0;
  initial_noise_pow_time = 0;
  nenr_silence_time = 0;
  nenr_silence_cnt = 0;
  memset(&agc_obj, 0, sizeof(agc_struct));
  memset(save_mic_in, 0, sizeof(short)*320);
  memset(ne_mic_fixed_in, 0, sizeof(short)*320);
  memset(ne_mic_float_in, 0, sizeof(float)*320);
  memset(fe_ref_fixed_in, 0, sizeof(short)*320);
  memset(ne_float_out, 0, sizeof(float)*160);
  memset(ne_fixed_out, 0, sizeof(short)*160);
  tmp = 0;
  printf("reset the global variable-------------------------end\n");



/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/*%*/
/*% SSP Algorithm Parameter */
/*%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


  /* SSP algorithm parameter need to be adjusted according to different prototype machine from client */
  /* { 0: default }, { 1: CEOP }, { 2: HT master (ref hw) }, { 3: HT slave (ref hw) }, { 4: ZT (ref sw)} */
  /* { 5: an example of slightly enhanced low frequencies in spk-path equalizer } */
  /* { 6: an example of slightly enhanced high frequencies in spk-path equalizer } */
  ssp_para_obj.para_client_config = 3;    /* for choosing client's prototype machine, [0, 6] */
#if INPUT_FILE_AS_ARG
  ssp_para_obj.para_client_config = customer_option;
  printf("input_file_as_arg option[%d]\n", ssp_para_obj.para_client_config);
#endif
  fe_wave_input_size = fe_wave_input_file_end - fe_wave_input_file;
  ne_wave_input_size = ne_wave_input_file_end - ne_wave_input_file;
  speech_vad = speech_vad;
  /* Assign Parameter Value to SSP Algorithm */
  /* These values of parameters need to be adjusted according to the actual field try on prototype machine of client, these parameters can be pulled out to be API input arguments for SDK */
  if (1 == ssp_para_obj.para_client_config) {
  ssp_para_obj.para_fun_config = 60;
  ssp_para_obj.para_delay_sample = 1;
  ssp_para_obj.para_dg_target = 1;
  ssp_para_obj.para_aec_init_filter_len = 2;
  ssp_para_obj.para_aec_filter_len = 13;
  ssp_para_obj.para_aes_std_thrd = 37;
  ssp_para_obj.para_aes_supp_coeff = 60;
  ssp_para_obj.para_nr_init_sile_time = 0;
  ssp_para_obj.para_nr_snr_coeff = 10;
  ssp_para_obj.para_agc_max_gain = 0;
  ssp_para_obj.para_agc_target_high = 2;
  ssp_para_obj.para_agc_target_low = 6;
  ssp_para_obj.para_agc_vad_ena = 1;
  ssp_para_obj.para_notch_freq = 1;
  ssp_para_obj.para_spk_fun_config = 0;
  ssp_para_obj.para_spk_agc_max_gain = 0;
  ssp_para_obj.para_spk_agc_target_high = 8;
  ssp_para_obj.para_spk_agc_target_low = 72;
  ssp_para_obj.para_spk_eq_nband = 1;
  ssp_para_obj.para_spk_eq_freq[0] = 0;  ssp_para_obj.para_spk_eq_gain[0] = 40;  ssp_para_obj.para_spk_eq_qfactor[0] = 15;
  ssp_para_obj.para_spk_eq_freq[1] = 0;  ssp_para_obj.para_spk_eq_gain[1] = 40;  ssp_para_obj.para_spk_eq_qfactor[1] = 15;
  ssp_para_obj.para_spk_eq_freq[2] = 0;  ssp_para_obj.para_spk_eq_gain[2] = 40;  ssp_para_obj.para_spk_eq_qfactor[2] = 15;
  ssp_para_obj.para_spk_eq_freq[3] = 0;  ssp_para_obj.para_spk_eq_gain[3] = 40;  ssp_para_obj.para_spk_eq_qfactor[3] = 15;
  ssp_para_obj.para_spk_eq_freq[4] = 0;  ssp_para_obj.para_spk_eq_gain[4] = 40;  ssp_para_obj.para_spk_eq_qfactor[4] = 15;
  } else if (2 == ssp_para_obj.para_client_config) {
  ssp_para_obj.para_fun_config = 15;
  ssp_para_obj.para_delay_sample = 1;
  ssp_para_obj.para_dg_target = 1;
  ssp_para_obj.para_aec_init_filter_len = 2;
  ssp_para_obj.para_aec_filter_len = 13;
  ssp_para_obj.para_aes_std_thrd = 37;
  ssp_para_obj.para_aes_supp_coeff = 60;
  ssp_para_obj.para_nr_init_sile_time = 0;
  ssp_para_obj.para_nr_snr_coeff = 15;
  ssp_para_obj.para_agc_max_gain = 0;
  ssp_para_obj.para_agc_target_high = 18;
  ssp_para_obj.para_agc_target_low = 66;
  ssp_para_obj.para_agc_vad_ena = 1;
  ssp_para_obj.para_notch_freq = 0;
  ssp_para_obj.para_spk_fun_config = 0;
  ssp_para_obj.para_spk_agc_max_gain = 0;
  ssp_para_obj.para_spk_agc_target_high = 8;
  ssp_para_obj.para_spk_agc_target_low = 72;
  ssp_para_obj.para_spk_eq_nband = 1;
  ssp_para_obj.para_spk_eq_freq[0] = 0;  ssp_para_obj.para_spk_eq_gain[0] = 40;  ssp_para_obj.para_spk_eq_qfactor[0] = 15;
  ssp_para_obj.para_spk_eq_freq[1] = 0;  ssp_para_obj.para_spk_eq_gain[1] = 40;  ssp_para_obj.para_spk_eq_qfactor[1] = 15;
  ssp_para_obj.para_spk_eq_freq[2] = 0;  ssp_para_obj.para_spk_eq_gain[2] = 40;  ssp_para_obj.para_spk_eq_qfactor[2] = 15;
  ssp_para_obj.para_spk_eq_freq[3] = 0;  ssp_para_obj.para_spk_eq_gain[3] = 40;  ssp_para_obj.para_spk_eq_qfactor[3] = 15;
  ssp_para_obj.para_spk_eq_freq[4] = 0;  ssp_para_obj.para_spk_eq_gain[4] = 40;  ssp_para_obj.para_spk_eq_qfactor[4] = 15;
  } else if (3 == ssp_para_obj.para_client_config) {
  ssp_para_obj.para_fun_config = 15;
  ssp_para_obj.para_delay_sample = 1;
  ssp_para_obj.para_dg_target = 1;
  ssp_para_obj.para_aec_init_filter_len = 1;
  ssp_para_obj.para_aec_filter_len = 2;
  ssp_para_obj.para_aes_std_thrd = 37;
  ssp_para_obj.para_aes_supp_coeff = 60;
  ssp_para_obj.para_nr_init_sile_time = 0;
  ssp_para_obj.para_nr_snr_coeff = 15;
  ssp_para_obj.para_agc_max_gain = 0;
  ssp_para_obj.para_agc_target_high = 15;
  ssp_para_obj.para_agc_target_low = 15;
  ssp_para_obj.para_agc_vad_ena = 1;
  ssp_para_obj.para_notch_freq = 0;
  ssp_para_obj.para_spk_fun_config = 0;
  ssp_para_obj.para_spk_agc_max_gain = 0;
  ssp_para_obj.para_spk_agc_target_high = 8;
  ssp_para_obj.para_spk_agc_target_low = 72;
  ssp_para_obj.para_spk_eq_nband = 1;
  ssp_para_obj.para_spk_eq_freq[0] = 0;  ssp_para_obj.para_spk_eq_gain[0] = 40;  ssp_para_obj.para_spk_eq_qfactor[0] = 15;
  ssp_para_obj.para_spk_eq_freq[1] = 0;  ssp_para_obj.para_spk_eq_gain[1] = 40;  ssp_para_obj.para_spk_eq_qfactor[1] = 15;
  ssp_para_obj.para_spk_eq_freq[2] = 0;  ssp_para_obj.para_spk_eq_gain[2] = 40;  ssp_para_obj.para_spk_eq_qfactor[2] = 15;
  ssp_para_obj.para_spk_eq_freq[3] = 0;  ssp_para_obj.para_spk_eq_gain[3] = 40;  ssp_para_obj.para_spk_eq_qfactor[3] = 15;
  ssp_para_obj.para_spk_eq_freq[4] = 0;  ssp_para_obj.para_spk_eq_gain[4] = 40;  ssp_para_obj.para_spk_eq_qfactor[4] = 15;
  } else if (4 == ssp_para_obj.para_client_config) {
  ssp_para_obj.para_fun_config = 207;
  ssp_para_obj.para_delay_sample = 450;
  ssp_para_obj.para_dg_target = 6;
  ssp_para_obj.para_aec_init_filter_len = 13;
  ssp_para_obj.para_aec_filter_len = 13;
  ssp_para_obj.para_aes_std_thrd = 37;
  ssp_para_obj.para_aes_supp_coeff = 60;
  ssp_para_obj.para_nr_init_sile_time = 0;
  ssp_para_obj.para_nr_snr_coeff = 15;
  ssp_para_obj.para_agc_max_gain = 0;
  ssp_para_obj.para_agc_target_high = 2;
  ssp_para_obj.para_agc_target_low = 72;
  ssp_para_obj.para_agc_vad_ena = 1;
  ssp_para_obj.para_notch_freq = 0;
  ssp_para_obj.para_spk_fun_config = 1;
  ssp_para_obj.para_spk_agc_max_gain = 0;
  ssp_para_obj.para_spk_agc_target_high = 8;
  ssp_para_obj.para_spk_agc_target_low = 72;
  ssp_para_obj.para_spk_eq_nband = 1;
  ssp_para_obj.para_spk_eq_freq[0] = 0;  ssp_para_obj.para_spk_eq_gain[0] = 40;  ssp_para_obj.para_spk_eq_qfactor[0] = 15;
  ssp_para_obj.para_spk_eq_freq[1] = 0;  ssp_para_obj.para_spk_eq_gain[1] = 40;  ssp_para_obj.para_spk_eq_qfactor[1] = 15;
  ssp_para_obj.para_spk_eq_freq[2] = 0;  ssp_para_obj.para_spk_eq_gain[2] = 40;  ssp_para_obj.para_spk_eq_qfactor[2] = 15;
  ssp_para_obj.para_spk_eq_freq[3] = 0;  ssp_para_obj.para_spk_eq_gain[3] = 40;  ssp_para_obj.para_spk_eq_qfactor[3] = 15;
  ssp_para_obj.para_spk_eq_freq[4] = 0;  ssp_para_obj.para_spk_eq_gain[4] = 40;  ssp_para_obj.para_spk_eq_qfactor[4] = 15;
  } else if (5 == ssp_para_obj.para_client_config) {
  ssp_para_obj.para_fun_config = 0;
  ssp_para_obj.para_delay_sample = 1;
  ssp_para_obj.para_dg_target = 1;
  ssp_para_obj.para_aec_init_filter_len = 2;
  ssp_para_obj.para_aec_filter_len = 13;
  ssp_para_obj.para_aes_std_thrd = 37;
  ssp_para_obj.para_aes_supp_coeff = 60;
  ssp_para_obj.para_nr_init_sile_time = 0;
  ssp_para_obj.para_nr_snr_coeff = 15;
  ssp_para_obj.para_agc_max_gain = 0;
  ssp_para_obj.para_agc_target_high = 2;
  ssp_para_obj.para_agc_target_low = 72;
  ssp_para_obj.para_agc_vad_ena = 1;
  ssp_para_obj.para_notch_freq = 0;
  ssp_para_obj.para_spk_fun_config = 2;
  ssp_para_obj.para_spk_agc_max_gain = 0;
  ssp_para_obj.para_spk_agc_target_high = 8;
  ssp_para_obj.para_spk_agc_target_low = 72;
  ssp_para_obj.para_spk_eq_nband = 4;
  ssp_para_obj.para_spk_eq_freq[0] = 6;  ssp_para_obj.para_spk_eq_gain[0] = 43;  ssp_para_obj.para_spk_eq_qfactor[0] = 0;
  ssp_para_obj.para_spk_eq_freq[1] = 7;  ssp_para_obj.para_spk_eq_gain[1] = 38;  ssp_para_obj.para_spk_eq_qfactor[1] = 11;
  ssp_para_obj.para_spk_eq_freq[2] = 8;  ssp_para_obj.para_spk_eq_gain[2] = 39;  ssp_para_obj.para_spk_eq_qfactor[2] = 11;
  ssp_para_obj.para_spk_eq_freq[3] = 9;  ssp_para_obj.para_spk_eq_gain[3] = 37;  ssp_para_obj.para_spk_eq_qfactor[3] = 0;
  ssp_para_obj.para_spk_eq_freq[4] = 0;  ssp_para_obj.para_spk_eq_gain[4] = 40;  ssp_para_obj.para_spk_eq_qfactor[4] = 15;
  } else if (6 == ssp_para_obj.para_client_config) {
  ssp_para_obj.para_fun_config = 0;
  ssp_para_obj.para_delay_sample = 1;
  ssp_para_obj.para_dg_target = 1;
  ssp_para_obj.para_aec_init_filter_len = 2;
  ssp_para_obj.para_aec_filter_len = 13;
  ssp_para_obj.para_aes_std_thrd = 37;
  ssp_para_obj.para_aes_supp_coeff = 60;
  ssp_para_obj.para_nr_init_sile_time = 0;
  ssp_para_obj.para_nr_snr_coeff = 15;
  ssp_para_obj.para_agc_max_gain = 0;
  ssp_para_obj.para_agc_target_high = 2;
  ssp_para_obj.para_agc_target_low = 72;
  ssp_para_obj.para_agc_vad_ena = 1;
  ssp_para_obj.para_notch_freq = 0;
  ssp_para_obj.para_spk_fun_config = 2;
  ssp_para_obj.para_spk_agc_max_gain = 0;
  ssp_para_obj.para_spk_agc_target_high = 8;
  ssp_para_obj.para_spk_agc_target_low = 72;
  ssp_para_obj.para_spk_eq_nband = 3;
  ssp_para_obj.para_spk_eq_freq[0] = 7;  ssp_para_obj.para_spk_eq_gain[0] = 42;  ssp_para_obj.para_spk_eq_qfactor[0] = 9;
  ssp_para_obj.para_spk_eq_freq[1] = 8;  ssp_para_obj.para_spk_eq_gain[1] = 40;  ssp_para_obj.para_spk_eq_qfactor[1] = 9;
  ssp_para_obj.para_spk_eq_freq[2] = 9;  ssp_para_obj.para_spk_eq_gain[2] = 43;  ssp_para_obj.para_spk_eq_qfactor[2] = 0;
  ssp_para_obj.para_spk_eq_freq[3] = 0;  ssp_para_obj.para_spk_eq_gain[3] = 40;  ssp_para_obj.para_spk_eq_qfactor[3] = 15;
  ssp_para_obj.para_spk_eq_freq[4] = 0;  ssp_para_obj.para_spk_eq_gain[4] = 40;  ssp_para_obj.para_spk_eq_qfactor[4] = 15;
  } else {
  ssp_para_obj.para_fun_config = 15;
  ssp_para_obj.para_delay_sample = 1;
  ssp_para_obj.para_dg_target = 1;
  ssp_para_obj.para_aec_init_filter_len = 2;
  ssp_para_obj.para_aec_filter_len = 13;
  ssp_para_obj.para_aes_std_thrd = 37;
  ssp_para_obj.para_aes_supp_coeff = 60;
  ssp_para_obj.para_nr_init_sile_time = 0;
  ssp_para_obj.para_nr_snr_coeff = 15;
  ssp_para_obj.para_agc_max_gain = 0;
  ssp_para_obj.para_agc_target_high = 2;
  ssp_para_obj.para_agc_target_low = 72;
  ssp_para_obj.para_agc_vad_ena = 1;
  ssp_para_obj.para_notch_freq = 0;
  ssp_para_obj.para_spk_fun_config = 1;
  ssp_para_obj.para_spk_agc_max_gain = 0;
  ssp_para_obj.para_spk_agc_target_high = 8;
  ssp_para_obj.para_spk_agc_target_low = 72;
  ssp_para_obj.para_spk_eq_nband = 1;
  ssp_para_obj.para_spk_eq_freq[0] = 0;  ssp_para_obj.para_spk_eq_gain[0] = 40;  ssp_para_obj.para_spk_eq_qfactor[0] = 15;
  ssp_para_obj.para_spk_eq_freq[1] = 0;  ssp_para_obj.para_spk_eq_gain[1] = 40;  ssp_para_obj.para_spk_eq_qfactor[1] = 15;
  ssp_para_obj.para_spk_eq_freq[2] = 0;  ssp_para_obj.para_spk_eq_gain[2] = 40;  ssp_para_obj.para_spk_eq_qfactor[2] = 15;
  ssp_para_obj.para_spk_eq_freq[3] = 0;  ssp_para_obj.para_spk_eq_gain[3] = 40;  ssp_para_obj.para_spk_eq_qfactor[3] = 15;
  ssp_para_obj.para_spk_eq_freq[4] = 0;  ssp_para_obj.para_spk_eq_gain[4] = 40;  ssp_para_obj.para_spk_eq_qfactor[4] = 15;
  }

/*% SSP Algorithm Applied to MIC Path */
/*% */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
  /* Get the sampling rate from CODEC configuration if run with system codes */
  //fread(&wav_header[0], 1, 44, fp_test_fe_ref_in);    /* wav header */
  //fread(&wav_header[0], 1, 44, fp_test_ne_mic_in);    /* wav header */

  //fs = (float)(wav_header[12]);    /* sampling rate recorded in 24th byte with 4-byte length, little endian */
  fs = 8000;
  cond1 = ssp_para_obj.para_fun_config & DELAY_ENABLE;
  if (cond1) {
  	delay_para(&ssp_para_obj);
	delay_state = delay_init(&ssp_para_obj);
  }

  cond1 = ssp_para_obj.para_fun_config & DG_ENABLE;
  if (cond1) {
  	DG_para(&dg_obj, &ssp_para_obj);
  }

  cond1 = ssp_para_obj.para_fun_config & DCREMOVER_ENABLE;
  if (cond1) {
  	DC_remover_init(&dc_obj, fs);
  }

  cond1 = ssp_para_obj.para_fun_config & NOTCH_ENABLE;
  if (cond1) {
  	notch_para(&notch_obj, &ssp_para_obj, fs);
  	notch_init(&notch_obj, notch_state);
  }

  cond1 = ssp_para_obj.para_fun_config & LP_AEC_ENABLE;
  cond2 = ssp_para_obj.para_fun_config & NLP_AES_ENABLE;
  if (cond1 | cond2) {
  	LP_AEC_para(&ssp_para_obj, &filter_length, &init_filter_length);
  	aec_state = LP_AEC_init(hopsize, filter_length, init_filter_length, fs);
  }
  if (cond2) {
  	NLP_AES_para(&ssp_para_obj, &std_thrd, &aes_supp_coef1, &aes_supp_coef2);
  	aes_state = NLP_AES_init(hopsize, fs, std_thrd, aes_supp_coef1, aes_supp_coef2);
  	aes_state->echo_state = aec_state;
  }

  if (!(ssp_para_obj.para_fun_config & AGC_ENABLE)) {    /* add fool-proof design for parameter configuration */
  	ssp_para_obj.para_agc_vad_ena = 0;
  }
  cond1 = ssp_para_obj.para_fun_config & NR_ENABLE;
  cond2 = ssp_para_obj.para_fun_config & AGC_ENABLE;
  if (cond1 | (cond2 && ssp_para_obj.para_agc_vad_ena)) {
  	NR_para(&ssp_para_obj, &aa, &mu);
  	nr_state = NR_init(hopsize, fs, aa, mu);
  }
  if (cond2) {
  	AGC_para(&agc_obj, &ssp_para_obj, 1);
  	AGC_init(&agc_obj, &ssp_para_obj, fs, hopsize, 1);
  }

  speech_vad = true;    /* default */
  st_vad = false;    /* default */
  frame = 1.0F;    /* set initial value here in case of NR off */
  nenr_silence_cnt = 0;
  ssp_para_obj.para_nr_init_sile_time = MIN(MAX(ssp_para_obj.para_nr_init_sile_time, 0), 250);
  if (fs == 16000.0F) {
  	initial_noise_pow_time = 14;
  	nenr_silence_time = ssp_para_obj.para_nr_init_sile_time*2;
  } else {
  	initial_noise_pow_time = 7;
  	nenr_silence_time = ssp_para_obj.para_nr_init_sile_time;
  }

  count_bytes = 0;
  printf("[Rtos]leave[%s]---------------->\n", __func__);
  //add for test case
  range = 0;
  mut_val = 1.0F;

  return 1;
}

/* Function Definitions */
int SSP_Algorithm(int test_mode, short *pinput, uint64_t pout)
{
  //printf("input_file vvvv[%d][0x%x][0x%x]\n", ssp_para_obj.para_client_config, cond1, cond2);
  memset(ne_fixed_out, 0, sizeof(short)*160);
  memset(fe_ref_fixed_in, 0, sizeof(short)*160);
  for (;;)	/* Main Frame Loop in MIC Path */
  {
	/* This section of codes have to be replaced by reading the bitstream from system layer in real platform */
#if 0//TODO: replace by pointer
	cond1 = fread(&ne_mic_fixed_in[0], sizeof(short), hopsize, fp_test_ne_mic_in);    /* get current near-end frame data */
	cond2 = fread(&fe_ref_fixed_in[0], sizeof(short), hopsize, fp_test_fe_ref_in);    /* get current far-end frame data */
#else
  //fill in the data
  if (test_mode == CVIAUDIO_SSP_MAIN_UNIT_TEST_MODE) {
    pfe = (short *)&fe_wave_input_file[count_bytes];
    pne = (short *)&ne_wave_input_file[count_bytes];
    for (i = 0; i < hopsize; i++) {
      //save_mic_in[i] =  pne[i];
      //ne_mic_fixed_in[i] = pfe[i];
      //fe_ref_fixed_in[i] = pne[i];
      ne_mic_fixed_in[i] = pne[i];
      fe_ref_fixed_in[i] = pfe[i];
    }

    count_bytes += hopsize * sizeof(short);
    //if (count_bytes >= (fe_wave_input_size)/3) {
    if (count_bytes >= (fe_wave_input_size)) {
      printf("ssp_main unit test reach the end of file ...force break\n");
      return_value = 0;
      return return_value;

    } else {
      return_value = 1;
    }
  } else if (test_mode == CVIAUDIO_SSP_MAIN_FRAME_MODE) {

    for (i = 0; i < hopsize; i++) {
      ne_mic_fixed_in[i] = pinput[i];
    }

    for (i = hopsize; i < hopsize*2; i++) {
      fe_ref_fixed_in[i] = pinput[i];
    }
    return_value = 1;
  }


  #endif
#if 0
  cond1 = cond2 = hopsize; //hopsize = 160 samples, total 320 bytes
	if ((cond1 != hopsize) | (cond2 != hopsize)) {
		cond1 = ssp_para_obj.para_fun_config & LP_AEC_ENABLE;
		cond2 = ssp_para_obj.para_fun_config & NLP_AES_ENABLE;
		cond3 = ssp_para_obj.para_fun_config & NR_ENABLE;
  		cond4 = ssp_para_obj.para_fun_config & AGC_ENABLE;
		cond6 = ssp_para_obj.para_fun_config & DELAY_ENABLE;
		if (cond1 | cond2) {
			LP_AEC_free(aec_state);
		}
		if (cond2) {
			if (aec_state != NULL)
				NLP_AES_free(aes_state);
		}
		if (cond3 | (cond4 && ssp_para_obj.para_agc_vad_ena)) {
      if (nr_state != NULL)
			  NR_free(nr_state);
		}
		if (cond6) {
			delay_free(delay_state);
		}
		//fclose(fp_test_ne_mic_in);
		//fclose(fp_test_fe_ref_in);
		//fclose(fp_test_ne_out);
    printf("illegal end of file:force end in [%s][%d]\n", __func__, __LINE__);
    return 0;
    //goto Pattern_EOF;
	}
#endif
	if (2 == ssp_para_obj.para_client_config) {
		for (i = 0; i < hopsize; i++) {
			tmp = (float)(fe_ref_fixed_in[i])/(float)(32768.0F) * 15.84F;
  			tmp = MIN(MAX(tmp, -1.0F), 1.0F);
  			fe_ref_fixed_in[i] = ROUND_POS(tmp*(short)(32767));
			ne_mic_fixed_in[i] = ROUND_POS(((float)(ne_mic_fixed_in[i])/(float)(32768.0F) * 0.5F)*(short)(32767));
		}
	}

	if (!(ssp_para_obj.para_fun_config)) {
		for (i = 0; i < hopsize; i++)
			ne_fixed_out[i] = ne_mic_fixed_in[i];    /* save MCPS if all off */
	} else {

		/* Delay ref signal, frame-by-frame processing */
		cond1 = ssp_para_obj.para_fun_config & DELAY_ENABLE;
		if (cond1) {
			delay_ref(fe_ref_fixed_in, delay_state, hopsize);
		}

		/* Apply DG, frame-by-frame processing */
		cond1 = ssp_para_obj.para_fun_config & DG_ENABLE;
		if (cond1) {
			apply_DG(ne_mic_fixed_in, ne_mic_fixed_in, &dg_obj, hopsize);
		}

		/* DC filter, frame-by-frame processing */
		cond1 = ssp_para_obj.para_fun_config & DCREMOVER_ENABLE;
		if (cond1) {
			DC_remover(ne_mic_fixed_in, ne_mic_fixed_in, &dc_obj, hopsize);
		}

		/* Notch filter, frame-by-frame processing */
		cond1 = ssp_para_obj.para_fun_config & NOTCH_ENABLE;
		if (cond1) {
			notch_filter(ne_mic_fixed_in, ne_mic_fixed_in, &notch_obj, hopsize);
		}

		/* Linear AEC, frame-by-frame processing */
		cond1 = ssp_para_obj.para_fun_config & LP_AEC_ENABLE;
		cond2 = ssp_para_obj.para_fun_config & NLP_AES_ENABLE;
		if (cond1 | cond2) {
			LP_AEC(aec_state, ne_mic_fixed_in, fe_ref_fixed_in, ne_fixed_out);
			if (cond2) {
				for (i = 0; i < hopsize; i++)
					aes_state->aes_std_in[i] = ne_mic_fixed_in[i];
			}
			for (i = 0; i < hopsize; i++)
				ne_mic_fixed_in[i] = ne_fixed_out[i];
		}

		cond3 = ssp_para_obj.para_fun_config & NR_ENABLE;
		cond4 = ssp_para_obj.para_fun_config & AGC_ENABLE;
		if (!(cond2 | cond3 | cond4)) {
			/* Compensate DG, frame-by-frame processing */
			cond1 = ssp_para_obj.para_fun_config & DG_ENABLE;
			if (cond1) {
				compen_DG(ne_mic_fixed_in, ne_mic_fixed_in, &dg_obj, hopsize);
			}
			for (i = 0; i < hopsize; i++)
				ne_fixed_out[i] = ne_mic_fixed_in[i];    /* save MCPS if NR+AES+AGC off */
		} else {
			if (cond3 | ssp_para_obj.para_agc_vad_ena) {
				/* Stationary NR, frame-by-frame processing */
				if (nenr_silence_cnt >= nenr_silence_time) {
					NR(ne_mic_fixed_in, initial_noise_pow_time, &frame, nr_state, &speech_vad, ne_float_out);
				} else {
					for (i = 0; i < hopsize; i++) {
						ne_float_out[i] = 0.0F;
						nr_state->x_old[i] = ne_mic_fixed_in[i];    /* update input overlap data */
					}
				}
			}
//TODO:
			if (cond3) {
				/* Transform to fixed-point with 16-bit length for output */
				for (i = 0; i < hopsize; i++) {
					ne_float_out[i] = MIN(MAX(ne_float_out[i], -1.0F), 1.0F);    /* saturation protection to avoid overflow */
					if (ne_float_out[i] < 0.0F)
						ne_fixed_out[i] = ROUND_NEG(ne_float_out[i]*(int)(32768));
					else
						ne_fixed_out[i] = ROUND_POS(ne_float_out[i]*(short)(32767));
				}
			} else {
				for (i = 0; i < hopsize; i++)
					ne_fixed_out[i] = ne_mic_fixed_in[i];
			}

			/* Nonlinear AES, frame-by-frame processing */
			if (cond2) {
				NLP_AES(aes_state, ne_fixed_out, frame, speech_vad);
				st_vad = (boolean_T)aes_state->st_vad;
			}

			if (2 == ssp_para_obj.para_client_config) {
				for (i = 0; i < hopsize; i++) {
					tmp = (float)(ne_fixed_out[i])/(float)(32768.0F) * 1.9952F;
					tmp = MIN(MAX(tmp, -1.0F), 1.0F);
					ne_fixed_out[i] = ROUND_POS(tmp*(short)(32767));
				}
			}

			/* Compensate DG, frame-by-frame processing */
			cond1 = ssp_para_obj.para_fun_config & DG_ENABLE;
			if (cond1) {
				compen_DG(ne_fixed_out, ne_fixed_out, &dg_obj, hopsize);
			}

			/* AGC, frame-by-frame processing */
			if (cond4) {
				AGC(ne_fixed_out, ne_fixed_out, &agc_obj, &ssp_para_obj, speech_vad, st_vad, nenr_silence_time, nenr_silence_cnt, 1);
			}

		}
	}

	if (nenr_silence_cnt < nenr_silence_time)
		nenr_silence_cnt += 1;
	else
		nenr_silence_cnt = nenr_silence_time + 1;    /* avoid overflow */

	if (nenr_silence_cnt > nenr_silence_time) {
		if (frame < initial_noise_pow_time)
			frame += 1;
		else
			frame = initial_noise_pow_time + 1;
	}

	//fwrite(&ne_fixed_out[0], sizeof(short), hopsize, fp_test_ne_out);

  memcpy(pout, ne_fixed_out, sizeof(short)*hopsize);
  //memcpy(pout, save_mic_in, sizeof(short)*hopsize);
  //memcpy(pout, ne_mic_fixed_in, sizeof(short)*hopsize);

  return return_value;
  }

  //Pattern_EOF:
  //  printf("Test Pattern EOF in MIC Path!\n");    /* EOF, set breakpoint here for VC++ version */

}

#if 0
#if INPUT_FILE_AS_ARG
static void _ssp_main_usage(void)
{
  printf("-----------------ssp main 20211202 usage -----------------\n");
  printf("=============================================================\n");
  printf("input arg 1: NE file\n");
  printf("input arg 2: FE file\n");
  printf("input arg 3: SPK input test file\n");
  printf("ssp main will produce , near end result and spk result\n");
  printf("=============================================================\n");
}
#endif

int main(int argc, char * const argv[])
{
#ifndef INPUT_FILE_AS_ARG
  (void)argc;
  (void)argv;
#else
  if (argc < 4) {
    printf("Need more input arguements ssp algo 20211202!!!\n");
    _ssp_main_usage();
    return -1;
  }
#if 0
FILE *fp_test_ne_mic_in;
FILE *fp_test_fe_ref_in;
FILE *fp_test_ne_out;
FILE *fp_test_fe_in;
FILE *fp_test_fe_out;

char *mic_ne_filename;
char *mic_fe_filename;
char *spk_in_filename;
#endif
  mic_ne_filename = argv[1];
  if (access(mic_ne_filename, 0) < 0) {
    printf("[Error]file name for mic_ne not exist[%s]\n", mic_ne_filename);
    return -1;
  }
  if ((fp_test_ne_mic_in = fopen(mic_ne_filename, "rb")) == NULL) {
    printf("[Error]Cannot open input file NE\n");
    return -1;
  }

  mic_fe_filename = argv[2];
  if (access(mic_fe_filename, 0) < 0) {
    printf("[Error]file name for mic_fe not exist[%s]\n", mic_fe_filename);
    return -1;
  }
  if ((fp_test_fe_ref_in = fopen(mic_fe_filename, "rb")) == NULL) {
    printf("[Error]Cannot open input file FE\n");
    return -1;
  }

  spk_in_filename = argv[3];
  if (access(spk_in_filename, 0) < 0) {
    printf("[Error]file name for SPK IN  not exist[%s]\n", spk_in_filename);
    return -1;
  }
  if ((fp_test_fe_in = fopen(spk_in_filename, "rb")) == NULL) {
    printf("[Error]Cannot open input file FE for spk\n");
    return -1;
  }
  //setup the output filename
  if ((fp_test_ne_out = fopen("mic_output.pcm", "wb")) == NULL) {
  	printf("Cannot open output file \n");
  	exit(2);
  }

  if ((fp_test_fe_out = fopen("spk_output.pcm", "wb")) == NULL) {
  printf("Cannot open output file \n");
  exit(2);
  }
    /* {0: default}, {1: CEOP}, {2: HT master (ref hw)}, {3: HT slave (ref hw)}, {4: ZT (ref sw)} */
  printf("input customer option:\n");
  printf("{0: default}, {1: CEOP}, {2: HT master (ref hw)}, {3: HT slave (ref hw)}, {4: ZT (ref sw)} :\n\t");
  scanf("%d", &customer_option);
  printf("Your option is[%d]\n", customer_option);
#endif

  SSP_Algorithm();

  return 0;
}
#endif


