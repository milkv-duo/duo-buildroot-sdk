
#ifndef NLPAES_SUBFUN_H
#define NLPAES_SUBFUN_H

 /* Include files */
#include "define.h"
#include "struct.h"

/* Function Declarations */
#ifndef PLATFORM_RTOS
extern FilterBank *filterbank_create(int banks, float sampling, int len, int type);
extern inline void filterbank_free(FilterBank *bank);
extern inline void filterbank_linear2sb(FilterBank *bank, float *ps, float *mel);
extern inline void filterbank_sb2linear(FilterBank *bank, float *mel, float *psd);
extern inline void aes_conj_window(float *w, int len);
extern inline float aes_hypergeom_gain(float vk);
extern inline float aes_qcurve(float x);
extern inline void aes_gain_floor(float effective_echo_suppress, float *echo, float *gain_floor, int len);
extern inline void aes_analysis(NonLinearEchoState *st, short *x);
#else 
FilterBank *filterbank_create(int banks, float sampling, int len, int type);
void filterbank_free(FilterBank *bank);
void filterbank_linear2sb(FilterBank *bank, float *ps, float *mel);
void filterbank_sb2linear(FilterBank *bank, float *mel, float *psd);
void aes_conj_window(float *w, int len);
float aes_hypergeom_gain(float vk);
float aes_qcurve(float x);
void aes_gain_floor(float effective_echo_suppress, float *echo, float *gain_floor, int len);
void aes_analysis(NonLinearEchoState *st, short *x);
#endif 
#endif


