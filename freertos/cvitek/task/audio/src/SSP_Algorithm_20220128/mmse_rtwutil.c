
/* Include files */
#include <math.h>
#include "mmse_rtwutil.h"

/*extern *fp_test;
extern debug_cnt;
extern frame_cnt;*/

/* Function Definitions */
double rt_hypotd(double u0, double u1)
{
  double y;
  double a;
  double b;

  //if (frame_cnt == 300)
  //	printf("");

  #if 0
  a = u0;
  b = u1;
  y =  (double)sqrt(a * a + b * b);
  if (y == 0.0F) 	/* can avoid y to be zero if as denominator */
  {
      a = fabs(u0);
      b = fabs(u1);
      if (a < b) {
          a /= b;
          y = b * sqrt(a * a + 1.0);
      } else if (a > b) {
          b /= a;
          y = a * sqrt(b * b + 1.0);
      } else {
          y = a * 1.4142135623730951;
      }
  }
  #else
  a = fabs(u0);
  b = fabs(u1);
  if (a < b) {
    a /= b;
    y = b * sqrt(a * a + 1.0);
  } else if (a > b) {
    b /= a;
    y = a * sqrt(b * b + 1.0);
  } else {
    y = a * 1.4142135623730951;
  }
  #endif

  /*fprintf(fp_test, "%.8lf \n", y);
  debug_cnt++;
  if (debug_cnt == 80943)
  	printf("");*/

  return y;
}

float rt_hypotf(float u0, float u1)
{
  float y;
  float a;
  float b;
  
  #if 1
  a = u0;
  b = u1;
  y =  (float)sqrt(a * a + b * b);
  if (y == 0.0F)    /* can avoid y to be zero if as denominator */
  {
      a = (float)fabs(u0);
      b = (float)fabs(u1);
      if (a < b) {
          a /= b;
          y = b * (float)sqrt(a * a + 1.0F);
      } else if (a > b) {
          b /= a;
          y = a * (float)sqrt(b * b + 1.0F);
      } else {
          y = a * 1.41421354F;
      }
  }
  #else
  a = (float)fabs(u0);
  b = (float)fabs(u1);
  if (a < b) {
    a /= b;
    y = b * (float)sqrt(a * a + 1.0F);
  } else if (a > b) {
    b /= a;
    y = a * (float)sqrt(b * b + 1.0F);
  } else {
    y = a * 1.41421354F;
  }
  #endif

  return y;
}


