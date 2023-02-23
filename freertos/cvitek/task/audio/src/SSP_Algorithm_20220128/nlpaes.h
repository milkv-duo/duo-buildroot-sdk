
#ifndef NLPAES_H
#define NLPAES_H

/* Include files */
#include "define.h"
#include "struct.h"
#include "tmwtypes.h"

/* Function Declarations */
extern void NLP_AES_para(ssp_para_struct *para, float *std_thrd, float *aes_supp_coef1, float *aes_supp_coef2);
extern NonLinearEchoState *NLP_AES_init(int frame_size, float sampling_rate, float std_thrd, float aes_supp_coef1, float aes_supp_coef2);
extern void NLP_AES_free(NonLinearEchoState *st);
extern void NLP_AES(NonLinearEchoState *aes_state, short *y, float aes_frame, boolean_T speech_vad);
extern void std_residual_echo(NonLinearEchoState *aes_state, float *yout, int len, boolean_T speech_vad);

#endif


