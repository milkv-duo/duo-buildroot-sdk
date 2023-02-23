/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *%
 *%       dc.c
 *%       Author: Sharon Lee
 *%       History:
 *%                  Created by Sharon Lee in April, 2021
 *%
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/* Include files */
#include "define.h"
#include "struct.h"
#include "dc.h"

/* Function Definitions */
void DC_remover_init(dcfilter_struct* dc_obj, float Fs)
{
   int i;
   int sampling_rate;

   sampling_rate = (int)Fs;

   if (sampling_rate == 8000)
      dc_obj->dcfilter_radius = .9f;
   if (sampling_rate == 16000)
      dc_obj->dcfilter_radius = .982f;
   
   for (i=0;i<2;i++)
      dc_obj->dcfilter_mem[i] = 0;
   
}

void DC_remover(short *in, short *out, dcfilter_struct* dc_obj, int frame_size)
{
   int i;
   float den2;
   float tmp;

   den2 = dc_obj->dcfilter_radius*dc_obj->dcfilter_radius + .7f*(1-dc_obj->dcfilter_radius)*(1-dc_obj->dcfilter_radius);
 
   for (i=0;i<frame_size;i++) {
      float vin = in[i];
      float vout = dc_obj->dcfilter_mem[0] + vin;
      dc_obj->dcfilter_mem[0] = dc_obj->dcfilter_mem[1] + 2*(-vin + dc_obj->dcfilter_radius*vout);  /* history state */
      dc_obj->dcfilter_mem[1] = vin - den2 * vout;  /* history state */
      tmp = dc_obj->dcfilter_radius * vout;
      out[i] = (short)WORD2INT(tmp);
   }
   
}


