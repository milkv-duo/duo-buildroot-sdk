
#ifndef DC_H
#define DC_H

/* Include files */
#include "define.h"
#include "struct.h"

/* Function Declarations */
extern void DC_remover_init(dcfilter_struct* dc_obj, float Fs);
extern void DC_remover(short *in, short *out, dcfilter_struct* dc_obj, int frame_size);

#endif


