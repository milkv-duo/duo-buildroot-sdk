/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%*/
/*%       dg.c*/
/*%       Author: Sharon Lee*/
/*%       History:*/
/*%                  Created by Sharon Lee in November, 2021*/
/*%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

 /* Include files */
#include <stdio.h>
#include <stdlib.h>
#include "struct.h"
#include "define.h"
#include "dg.h"

/* Function Definitions */
void DG_para(dg_struct *dg, ssp_para_struct *para)
{
  dg_struct *dg_obj;
  ssp_para_struct *ssp_para_obj;
  static float dg_cut_tab[12] = {0.891251F, 0.794328F, 0.707946F, 0.630957F, 0.562341F, 0.501187F,
                                                             0.446684F, 0.398107F, 0.354813F, 0.316228F, 0.281838F, 0.251189F};
  static float dg_boost_tab[12] = {1.122018F, 1.258925F, 1.412538F, 1.584893F, 1.778279F, 1.995262F,
                                                                  2.238721F, 2.511886F, 2.818383F, 3.162278F, 3.548134F, 3.981072F};

  /* DG Parameter */
  ssp_para_obj = para;
  dg_obj = dg;
  ssp_para_obj->para_dg_target = MIN(MAX(ssp_para_obj->para_dg_target, 1), 12);  /* target 0dB if disable DG_ENABLE */
  dg_obj->cut_gain = dg_cut_tab[ssp_para_obj->para_dg_target-1];
  dg_obj->boost_gain = dg_boost_tab[ssp_para_obj->para_dg_target-1];

}

void apply_DG(short *pin, short *pout, dg_struct *dg, int frame_size)
{
  int i;
  float tmp;
  dg_struct *dg_obj;

  dg_obj = dg;
  for (i=0;i<frame_size;i++) {
    tmp = (float)(pin[i])/(float)(32768.0F)*dg_obj->cut_gain;
    if (tmp < 0.0F) {
      pout[i] = ROUND_NEG(tmp*(int)(32768));
    } else {
      pout[i] = ROUND_POS(tmp*(short)(32767));
    }
  }

}

void compen_DG(short *pin, short *pout, dg_struct *dg, int frame_size)
{
  int i;
  float tmp;
  dg_struct *dg_obj;

  dg_obj = dg;
  for (i=0;i<frame_size;i++) {
    tmp = (float)(pin[i])/(float)(32768.0F)*dg_obj->boost_gain;
    tmp = MIN(MAX(tmp, -1.0F), 1.0F);
    if (tmp < 0.0F) {
      pout[i] = ROUND_NEG(tmp*(int)(32768));
    } else {
      pout[i] = ROUND_POS(tmp*(short)(32767));
    }
  }

}


