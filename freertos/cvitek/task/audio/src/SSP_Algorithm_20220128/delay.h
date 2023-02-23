
#ifndef DELAY_H
#define DELAY_H

/* Include files */
#include "define.h"
#include "struct.h"

/* Function Declarations */
extern void delay_para(ssp_para_struct *para);
extern DelayState *delay_init(ssp_para_struct *para);
extern void delay_free(DelayState *st);
extern void delay_ref(short *pin, DelayState *delay_state, int frame_size);

#endif


