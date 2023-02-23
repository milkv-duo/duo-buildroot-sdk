
#ifndef DG_H
#define DG_H

/* Include files */
#include <stddef.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif
#include "struct.h"
#include "tmwtypes.h"

/* Function Declarations */
extern void DG_para(dg_struct *dg, ssp_para_struct *para);
extern void apply_DG(short *pin, short *pout, dg_struct *dg, int frame_size);
extern void compen_DG(short *pin, short *pout, dg_struct *dg, int frame_size);

#endif


