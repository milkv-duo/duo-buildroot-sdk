
#ifndef AGC_INIT_H
#define AGC_INIT_H

/* Include files */
#include <stddef.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif
#include "struct.h"
#include "tmwtypes.h"

/* Function Declarations */
extern void AGC_init(agc_struct *agc, ssp_para_struct *para_obj, float Fs, int frame_size, short path);
extern void AGC_para(agc_struct *agc, ssp_para_struct *para, short path);

#endif


