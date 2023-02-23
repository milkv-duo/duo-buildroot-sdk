
#ifndef AGC_H
#define AGC_H

/* Include files */
#include <stddef.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif
#include "struct.h"
#include "tmwtypes.h"

/* Function Declarations */
extern void AGC(short *sin, short *sout, agc_struct *agc_obj, ssp_para_struct *para_obj, boolean_T speech_vad, boolean_T st_vad, int nenr_silence_time, int nenr_silence_cnt, short path);

#endif


