
#ifndef NOTCH_H
#define NOTCH_H

/* Include files */
#include <stddef.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif
#include "struct.h"
#include "tmwtypes.h"

/* Function Declarations */
extern void notch_para(qfilter_struct *notch_obj, ssp_para_struct *para, float Fs);
extern void notch_init(qfilter_struct *notch_obj, int *notch_state);
extern void notch_filter(short *pin, short *pout, qfilter_struct *filter, int frame_size);

#endif


