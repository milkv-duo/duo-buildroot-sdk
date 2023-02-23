
/* Include files */
#include "define.h"
#include "db2linear.h"

/* Function Definitions */
short db_to_linear(short db_num)	/* Calculate the linear scale for dB input */
{
  short i;
  short tmp16;
  
  tmp16 = 0x7fff;
  db_num = MAX(0,db_num);
  for (i = 0; i < db_num; i++)
  	tmp16 = (short)(((long)tmp16 * (long)29204) >> 15);
  return tmp16;
  
}

/* y(n) = (short)((short)y(n-1)*(short)alpha) + (short)((short)x(n)*(1-alpha)) */
/* size is block size, it can be 1 for single sample */
void smooth16bit(short *state_p, short *in_p, short alpha, short size)
{
  short i;
  short xn;
  short yn1;
  short alpha2;
  long long acc0;

  xn = *in_p++;
  yn1 = *state_p++;
  alpha2 = -alpha-((short)0x8000);    /* 1-alpha */
  alpha2 = MAX(0, (MIN(0x7fff, (unsigned short)alpha2)) );

  for (i = size; i > 0 ; i--) {
  	acc0 = ((long)yn1*(long)alpha) << 1;
  	yn1 =*state_p--;
  	acc0 += ((xn*alpha2) << 1);
  	xn = *in_p++;
  	acc0 = MAX(((long)0x80000000l), (MIN(0x7fffffffl, acc0)));
  	*state_p = (short)(acc0 >> 16);
  	state_p += 2;
  }
}


