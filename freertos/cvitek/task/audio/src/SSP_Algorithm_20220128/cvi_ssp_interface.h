/* Include files */
#ifndef __CVI_SSP_INTERFACE__
#define __CVI_SSP_INTERFACE__
#include <stdio.h>

#define AEC_PRO_DATA_LEN (160)
//for uplink VQE(MIC in)
void *audio_ssp_init(void *para_obj, unsigned int sample_rate);
int audio_ssp_fun_config(void *handle, int u32OpenMask);
int audio_ssp_process(void *handle, short *mic_in, short *ref_in, short *dataout, int iLength);
int audio_ssp_deinit(void *handle);

//for downlink VQE(SPK OUT)
void *audio_ssp_spk_init(void *para_obj, unsigned int sample_rate);
int audio_ssp_spk_process(void *handle, short *spk_in, short *dataout, int iLength);
int audio_ssp_spk_deinit(void *handle);

#endif
