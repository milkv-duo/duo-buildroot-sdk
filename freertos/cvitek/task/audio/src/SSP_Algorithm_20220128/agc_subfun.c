
/* Include files */
#include "struct.h"
#include "define.h"
#include "agc_subfun.h"

/* Function Definitions */
short envelope(short* sin, short frame_size)	/* Envelope detector based on frame */
{
  short i;
  short temp;
  short temp_max;
  short y;
  short *ptr_sin;
  //int y1, y2;

  ptr_sin = sin;
  temp_max = 0x4;    /* keep a minimal DC here */
  for (i = 0; i < frame_size; i++) {
  	temp = *ptr_sin++;
  	if (temp == (short)0x8000)
  		temp = (short)0x8001;
  	temp = ABS(temp);
  	if (temp > temp_max)
  		temp_max = temp;
  }
  
  #if 1
  y = temp_max;
  #else
  y1 = ((int)(agc_obj->alpha1)* (int)(agc_obj->env)) << 1;
  y2 = ((int)(agc_obj->alpha) * (int)(temp_max)) << 1;
  y = (y1 + y2) >> 16;
  #endif
  
  return y;

}

void update_gain(int diff, agc_struct *agc_obj)
{
  int delta32;
  int gain32;
  short step_size;

  if (diff > 32767)
  	diff = 32767;
  else if (diff < -32768)
  	diff = -32768;

  if (diff > 0)    /* attack time and release time constants should be separated here!!! */
  	step_size = agc_obj->stepsize_release;	/* usually gain slowly increase, so release time constant here slowly */
  else
  	step_size = agc_obj->stepsize_attack;	/* usually gain quickly decrease, so attack time constant here quickly */

  delta32 = ((int)(step_size) * (int)(diff)) << 1;
  gain32 = agc_obj->gain32 + delta32;

  //if(gain32 <0)
  //	printf("");
  if (((unsigned int)gain32) > 32767*65536)    /* saturation restriction */
  	gain32 = 32767*65536;    /* 0x7FFF0000 */

  agc_obj->gain32 = gain32;

 }


