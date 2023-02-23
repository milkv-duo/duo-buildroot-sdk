/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%*/
/*%       eq.c*/
/*%       Author: Sharon Lee*/
/*%       History:*/
/*%                  Created by Sharon Lee in January, 2022*/
/*%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

 /* Include files */
#include <stdio.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif
#include "struct.h"
#include "define.h"
#include "eq.h"

/* Function Definitions */
void equalizer_para(cascaded_iir_struct *spk_eq_obj, ssp_para_struct *para, float Fs)
{
  short i, j;
  ssp_para_struct *ssp_para;
  float kcoef[5], vcoef[5], qfactor[5];
  float num[3], den[3], filter_gain;
  static float fs8k_kcoef_tab[10] = {0.03929011F,0.07870170F,0.09849140F,0.13831677F,0.19891237F,
  0.32491970F,0.50952548F,1.49660575F,3.54573250F,254.64660645F};
  static float fs16k_kcoef_tab[11] = {0.01963748F,0.03929011F,0.04912685F,0.06883073F,0.09849140F,
  0.15838444F,0.24007876F,0.53451115F,0.75698018F,0.99608070F,509.29516602F};
  static float vcoef_tab[61] = {0.01000000F,0.01122018F,0.01258925F,0.01412538F,0.01584893F,
  0.01778279F,0.01995262F,0.02238721F,0.02511887F,0.02818383F,0.03162277F,0.03548134F,
  0.03981072F,0.04466836F,0.05011872F,0.05623413F,0.06309573F,0.07079457F,0.07943282F,
  0.08912510F,0.10000000F,0.11220185F,0.12589253F,0.14125375F,0.15848932F,0.17782794F,
  0.19952624F,0.22387211F,0.25118864F,0.28183830F,0.31622776F,0.35481340F,0.39810717F,
  0.44668359F,0.50118721F,0.56234133F,0.63095737F,0.70794576F,0.79432821F,0.89125091F,
  1.00000000F,1.12201846F,1.25892544F,1.41253757F,1.58489323F,1.77827942F,1.99526227F,
  2.23872113F,2.51188636F,2.81838298F,3.16227770F,3.54813385F,3.98107171F,4.46683598F,
  5.01187229F,5.62341309F,6.30957365F,7.07945776F,7.94328213F,8.91250896F,10.00000000F};
  static float qfactor_tab[18] = {0.01F, 0.05F, 0.1F, 0.2F, 0.3F, 0.4F, 0.5F, 0.6F, 0.7F, 0.8F, 0.9F, 1.0F, 1.5F,
  2.0F, 3.0F, 9.0F, 27.0F, 45.0F};

  /* Equalizer Parameter */
  ssp_para = para;
  ssp_para->para_spk_eq_nband = MIN(MAX(ssp_para->para_spk_eq_nband, 1), 5);
  for(i = 0; i < ssp_para->para_spk_eq_nband; i++) {
    if (8000.0F == Fs) {
      ssp_para->para_spk_eq_freq[i] = MIN(MAX(ssp_para->para_spk_eq_freq[i], 0), 9);
      kcoef[i] = fs8k_kcoef_tab[ssp_para->para_spk_eq_freq[i]];
    } else {
      ssp_para->para_spk_eq_freq[i] = MIN(MAX(ssp_para->para_spk_eq_freq[i], 0), 10);
      kcoef[i] = fs16k_kcoef_tab[ssp_para->para_spk_eq_freq[i]];
    }

    ssp_para->para_spk_eq_gain[i] = MIN(MAX(ssp_para->para_spk_eq_gain[i], 0), 60);
    vcoef[i] = vcoef_tab[ssp_para->para_spk_eq_gain[i]];

    ssp_para->para_spk_eq_qfactor[i] = MIN(MAX(ssp_para->para_spk_eq_qfactor[i], 0), 17);
    qfactor[i] = qfactor_tab[ssp_para->para_spk_eq_qfactor[i]];
  }

  /* IIR filter */
  spk_eq_obj->b0 = FLOAT_ONE;
  for(i = 0; i < ssp_para->para_spk_eq_nband; i++) {
    if (FLOAT_ONE == vcoef[i]) {  /* linear gain = 1 */
      filter_gain = FLOAT_ONE;
      den[0] = FLOAT_ONE;
      num[0] = FLOAT_ONE;
      den[1] = FLOAT_ZERO;
      num[1] = FLOAT_ZERO;
      den[2] = FLOAT_ZERO;
      num[2] = FLOAT_ZERO;
    } else {
      num[0] = 1.0F+vcoef[i]*kcoef[i]/qfactor[i]+kcoef[i]*kcoef[i];
      num[1] = 2.0F*(kcoef[i]*kcoef[i]-1);
      num[2] = 1.0F-vcoef[i]*kcoef[i]/qfactor[i] + kcoef[i]*kcoef[i];
      den[0] = 1.0F+kcoef[i]/qfactor[i] + kcoef[i]*kcoef[i];
      den[1] = 2.0F*(kcoef[i]*kcoef[i]-1);
      den[2] = 1.0F-kcoef[i]/qfactor[i] + kcoef[i]*kcoef[i];
      filter_gain = num[0]/den[0];
      for (j = 2; j >= 0 ; j--) {
        num[j] = num[j]/num[0];
        den[j] = den[j]/den[0];
      }
    }
    spk_eq_obj->coef[0+4*i] = num[2];
    spk_eq_obj->coef[1+4*i] = num[1];
    spk_eq_obj->coef[2+4*i] = den[2];
    spk_eq_obj->coef[3+4*i] = den[1];
    spk_eq_obj->b0 *= filter_gain;
  }

  spk_eq_obj->stage = ssp_para->para_spk_eq_nband;

}

void equalizer_init(cascaded_iir_struct *spk_eq_obj, float *spk_eq_state)
{
  short i;

  for(i = 0; i < 12; i++) {
    spk_eq_state[i] = FLOAT_ZERO;
  }
  spk_eq_obj->pstate = spk_eq_state;

}

void equalizer(short *pin, short *pout, cascaded_iir_struct *eq, short frame_size)
{
  short i, j;
  short *ptr1, *ptr2;
  float *pst, *pcoef;
  float acc0, acc1, xn, xn1, xn2, yn2;
  float yn1 = 0.0;

  ptr1 = pin;
  ptr2 = pout;

  for(i = 0; i < frame_size; i++) {
  	pcoef = &eq->coef[0];
	pst = eq->pstate;
	xn = (float)(*ptr1++)/(float)(32768.0F);    /* x(n) */
  	acc0 = xn*eq->b0;
	//acc0 = xn*ABS(eq->b0);

	for(j = eq->stage; j > 0; j--) {
		xn2 = *pst++;    /* take x(n-2) */
		acc1 = acc0 + (xn2)*(*pcoef++);

		xn1 = *pst++;    /* take x(n-1) */
		acc1 += (xn1)*(*pcoef++);

		yn2 = *pst++;    /* take y(n-2) */
		acc1 -= (yn2)*(*pcoef++);

		yn1 = *pst;    /* take y(n-1) */

		pst -= 3;    /* point to x(n-2) */
		*pst++ = xn1;    /* update x(n-2) */
		*pst++ = acc0;    /* update x(n-1), point to y(n-2) */
		acc0 = acc1 - (yn1)*(*pcoef++);
	}

	*pst++ = yn1;    /* update y(n-2) */
	*pst = acc0;    /* update y(n-1) */

	acc0 = MIN(MAX(acc0, -1.0F), 1.0F);
	if (acc0 < 0.0F)
		*ptr2++ = ROUND_NEG(acc0*(int)(32768));    /* y(n) */
	else
		*ptr2++ = ROUND_POS(acc0*(short)(32767));    /* y(n) */

  }

}


