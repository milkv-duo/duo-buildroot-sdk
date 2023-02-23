/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%*/
/*%       delay.c*/
/*%       Author: Sharon Lee*/
/*%       History:*/
/*%                  Created by Sharon Lee in November, 2021*/
/*%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

 /* Include files */
#include <stdio.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#endif
#include "memalloc.h"
#include "struct.h"
#include "define.h"
#include "delay.h"

/* Function Definitions */
EXPORT void delay_para(ssp_para_struct *para)
{
  ssp_para_struct *ssp_para_obj;

  /* Delay Parameter */
  ssp_para_obj = para;
  ssp_para_obj->para_delay_sample = MIN(MAX(ssp_para_obj->para_delay_sample, 1), 3000);  /* delay in sample, delay 0 sample if disable DELAY_ENABLE */

}

EXPORT DelayState *delay_init(ssp_para_struct *para)
{
  int i;
  ssp_para_struct *ssp_para_obj;
  DelayState *st = (DelayState *)mem_alloc(sizeof(DelayState));

  ssp_para_obj = para;
  st->size = 3001;

  st->dly_ref_buf = (short*)mem_alloc(st->size*sizeof(short));

  for (i=0;i<st->size;i++)
    st->dly_ref_buf[i] = 0;

  st->addr = st->dly_ref_buf;
  st->rdptr = st->addr;
  st->wrptr = st->addr + ssp_para_obj->para_delay_sample;

  return st;

}

EXPORT void delay_free(DelayState *st)
{
  mem_free(st->dly_ref_buf);
  mem_free(st);

}

EXPORT void delay_ref(short *pin, DelayState *delay_state, int frame_size)
{
  int i;
  DelayState *st;
  short *ptr1, *ptr2, *ptr3;

  st = delay_state;

  ptr1 = st->wrptr;  /* write pointer in circular butter */
  ptr2 = pin;
  ptr3 = st->rdptr;  /* read pointer in circular butter */

  for (i=0; i<frame_size; i++) {
    *ptr1++ = *ptr2;  /* read from input buffer and write to circular buffer */
    if(ptr1 >= (st->addr + st->size))
      ptr1 = st->addr;
    *ptr2++ = *ptr3++;  /* read from circular buffer and write to input buffer */
    if(ptr3 >= (st->addr + st->size))
      ptr3 = st->addr;
  }

  st->wrptr = ptr1;  /* restore for next frame */
  st->rdptr = ptr3;  /* restore for next frame */

}


