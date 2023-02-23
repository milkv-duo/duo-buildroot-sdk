
#ifndef PACKFFT_H
#define PACKFFT_H

 /* Include files */
#include "define.h"

/* Function Declarations */
extern void *speech_fft_init(int size);
extern void speech_fft_free(void *table);
extern void speech_fft(void *table, float *in, float *out);
extern void speech_ifft(void *table, float *in, float *out);

#endif


