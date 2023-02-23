/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *%
 *%       notch.c
 *%       Author: Sharon Lee
 *%       History:
 *%                  Created by Sharon Lee in April, 2021
 *%
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

 /* Include files */
#include <stdio.h>
#include <stdlib.h>
#include "struct.h"
#include "define.h"
#include "notch.h"

/* Function Definitions */
void notch_para(qfilter_struct *notch_obj, ssp_para_struct *para, float Fs)
{
  ssp_para_struct *para_obj;
  unsigned short tmp;

  /* The last coef is scale value of Hd */
  static short nb_notch_coef[2][6] = {
      {7415,-10487,7415,23068,-16239, 18020},
      {5441,10882,5441,-32196,-15812,24237}
  };
  static short wb_notch_coef[2][6] = {
      {7255,-13406,7255,30207,-16311,18459},
      {8192,0,8192,0,-16096, 16240}
  };

  /* Notch Filter Parameter */
  para_obj = para;
  tmp = MIN(MAX(para_obj->para_notch_freq, 0), 1);    /* the notch freq, 0 for 1kHz and 1 for 4kHz */
  if (Fs == 8000.0F)
    notch_obj->qcoef = &nb_notch_coef[tmp][0];
  else
    notch_obj->qcoef = &wb_notch_coef[tmp][0];
  
}

void notch_init(qfilter_struct *notch_obj, int *notch_state)
{
  unsigned short i;

  for(i=0; i<4;i++) {
    notch_state[i] = 0;
  }

  notch_obj->nq = 1;
  notch_obj->pstate = notch_state;
  
}

void notch_filter(short *pin, short *pout, qfilter_struct *filter, int frame_size)
{
  long long acc0;
  int i, j, *pst, xn, xn1, xn2, yn1, yn2;
  short *pcoef, *ptr1, *ptr2;
  short coef_tmp1, coef_tmp2, coef_tmp3, coef_tmp4, coef_tmp5;
  short state_tmp1, state_tmp2, state_tmp3, state_tmp4, state_tmp5;

  //initialize
  acc0 =0;

	ptr1 = pin;
	ptr2 = pout;

	for(i = frame_size; i > 0; i--) {
		pcoef = filter->qcoef;
		pst = filter->pstate;
	 	xn = (((int) (*ptr1++))<<16);

		for(j = filter->nq; j > 0; j--) {
			coef_tmp1 = *pcoef;  /* for high part */
			state_tmp1 = ((short)(xn>>0x10));  /* for high part */
			acc0 = ((unsigned short)(xn))*(*pcoef++);  /* low part */

			xn1 = *pst;  /* take x(n-1) */
			*pst++ = xn;  /* update x(n-1)*/

			coef_tmp2 = *pcoef;  /* for high part */
			state_tmp2 = ((short)(xn1>>0x10));  /* for high part */
			acc0 += ((unsigned short)(xn1))*(*pcoef++);  /* low part */

			xn2 = *pst;  /* take x(n-2) */
			*pst++ = xn1;  /* update x(n-2) */

			coef_tmp3 = *pcoef;  /* for high part */
			state_tmp3 = ((short)(xn2>>0x10));  /* for high part */
			acc0 += ((unsigned short)(xn2))*(*pcoef++);  /* low part */

			yn1 = *pst++;  /* take y(n-1) */

			coef_tmp4 = *pcoef;  /* for high part */
			state_tmp4 = ((short)(yn1>>0x10));  /* for high part */
			acc0 += ((unsigned short)(yn1))*(*pcoef++);  /* low part */

			yn2 = *pst;  /* take y(n-2) */

			coef_tmp5 = *pcoef;  /* for high part */
			state_tmp5 = ((short)(yn2>>0x10));  /* for high part */
			acc0 += ((unsigned short)(yn2))*(*pcoef++);  /* low part */

			*pst++ = yn1;  /* update y(n-2) */

			acc0 >>= 0x10;  /* be low part */
			acc0 += ((short)(state_tmp1))*coef_tmp1;  /* high part */
			acc0 += ((short)(state_tmp2))*coef_tmp2;  /* high part */
			acc0 += ((short)(state_tmp3))*coef_tmp3;  /* high part */
			acc0 += ((short)(state_tmp4))*coef_tmp4;  /* high part */
			acc0 += ((short)(state_tmp5))*coef_tmp5;  /* high part */

			acc0 = MAX(((int)0x80000000), (MIN(0x7fffffff, (((int)acc0)<<2))));  /* Q2.14 */

			pst[-2] = (int)acc0 ;  /* update y(n-1) */
			xn =  (int)acc0;  /* update x(n) for next IIR filter */

		}

		if(acc0>=0)
			acc0 = (((int)((int)acc0>>16)) * (*pcoef));
		else
			acc0 = -(((int)(-(int)acc0>>16)) * (*pcoef));  /* remove zero-inputs-limit-cycle osicillation */

		//acc0 = MAX(((int)0x80000000), (MIN(0x7fffffff, (((int)acc0)<<3))));
		acc0 = MAX(((int)0x80000000), (MIN(0x7fffffff, ((acc0)<<3))));  /* Q3.13 */
		*ptr2++ = (short)(acc0>>16);

	}

}


