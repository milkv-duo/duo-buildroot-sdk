
#ifndef EQ_H
#define EQ_H

/* Include files */
#include <stddef.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif 
#include "struct.h"
#include "tmwtypes.h"

/* Function Declarations */
void equalizer_para(cascaded_iir_struct *spk_eq_obj, ssp_para_struct *para, float Fs);
void equalizer_init(cascaded_iir_struct *spk_eq_obj, float *spk_eq_state);
void equalizer(short *pin, short *pout, cascaded_iir_struct *eq, short frame_size);

#endif


