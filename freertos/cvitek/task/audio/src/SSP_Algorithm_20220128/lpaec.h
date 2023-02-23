
#ifndef LPAEC_H
#define LPAEC_H

 /* Include files */
#include "define.h"
#include "struct.h"
#include "tmwtypes.h"

/* Function Declarations */
extern void LP_AEC_para(ssp_para_struct *para, short *filter_length, short *init_filter_length);
extern LinearEchoState *LP_AEC_init(int frame_size, short filter_length, short init_filter_length, float Fs);
extern void LP_AEC_free(LinearEchoState *st);
extern void LP_AEC(LinearEchoState *st, const short *rec, const short *play, short *out);
extern void echo_state_reset(LinearEchoState *st);

#endif


