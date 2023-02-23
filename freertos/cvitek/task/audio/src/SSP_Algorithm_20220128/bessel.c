
/* Include files */
#include <string.h>
#include "bessel.h"
#include "tmwtypes.h"

/* Function Definitions */
void bessel_function(float v, const float x[321], float bessel[321])
{
  /*  ((4^m)*factorial(m)*factorial(m+v)) for zero order (v = 0) */
  static float bessel_v0_f[13] = { 4, 64, 2304, 147456, 14745600, 2.123366400000000e+09, 4.161798144000000e+11, 1.065420324864000e+14, 3.451961852559360e+16, (float)1.380784741023744e+19, (float)6.682998146554921e+21,
  (float)3.849406932415634e+24, (float)2.602199086312969e+27 };
  static double bessel_v0_d[5] = { 2.040124083669368e+30, 1.836111675302431e+33, 1.880178355509689e+36, 2.173486178969201e+39, 2.816838087944084e+42 };

  /*  ((4^m)*factorial(m)*factorial(m+v)) for first order (v = 1) */
  static float bessel_v1_f[13] = { 8, 192, 9216, 737280, 88473600, 1.486356480000000e+10, 3.329438515200000e+12, 9.588782923776000e+14, (float)3.451961852559360e+17, (float)1.518863215126118e+20, (float)8.019597775865905e+22,
  (float)5.004229012140325e+25, (float)3.643078720838156e+28 };
  static double bessel_v1_d[5] = { 3.060186125504052e+31, 2.937778680483889e+34, 3.196303204366472e+37, 3.912275122144561e+40, 5.351992367093760e+43 };

  float *bessel_f_ptr, *bessel_f_ptr0;
  double *bessel_d_ptr, *bessel_d_ptr0;
  float acc0_f, acc1_f;
  float tmp2_f;
  double acc0_d, acc1_d;
  double tmp2_d;
  boolean_T short_iteration;
  int i, m;
  float tmp1;

  if (v == 0.0F) {
    bessel_f_ptr0 = &bessel_v0_f[0];
    bessel_d_ptr0 = &bessel_v0_d[0];
  } else {
    bessel_f_ptr0 = &bessel_v1_f[0];
    bessel_d_ptr0 = &bessel_v1_d[0];
  }

  for (i = 0; i < 321; i++) {    /*  input vector length */
    acc0_f = 1.0F;     /*  result for m = 0 */
    acc1_f = acc0_f;
    tmp2_f = 1.0F;
    tmp1 = x[i] * x[i];  
    bessel_f_ptr = bessel_f_ptr0;
    short_iteration = false;

    for (m = 0; m < 13; m++) {
      if ((double)(tmp2_f*tmp1) > 3.4E+38)    /* positive for sure */
        tmp2_f = (float)3.4E+38;
      else
        tmp2_f *= tmp1;
      acc0_f = acc0_f + tmp2_f / (*bessel_f_ptr);
      if (acc0_f == acc1_f) {
        short_iteration = true;
        break;
      }
      acc1_f = acc0_f;
      bessel_f_ptr++;
    }

    if (short_iteration == true) {
      /* bessel(i,1) = ((x/2)^v)*acc0 */
      if (v == 0.0F) {
        bessel[i] = acc0_f;
      } else {
        bessel[i] = x[i] / 2.0F * acc0_f;
      }
      goto bessel_end;
    }
    
    tmp2_d = (double)tmp2_f;
    acc0_d = (double)acc0_f;
    acc1_d = acc0_d;
    bessel_d_ptr = bessel_d_ptr0;

    for (m = 0; m < 5; m++) {
      tmp2_d *= tmp1;    /* positive for sure */
      if (tmp2_d > 1.7E+200)    /* 1.7E+308 */
        break;
      acc0_d = acc0_d + tmp2_d / (*bessel_d_ptr);
      if ((float)(acc0_d) == (float)(acc1_d))
         break;
      acc1_d = acc0_d;
      bessel_d_ptr++;
    }

    /* bessel(i,1) = ((x/2)^v)*acc0 */
    if (v == 0.0F) {
      bessel[i] = (float)(acc0_d);
    } else {
      bessel[i] = (float)(x[i] / 2.0F * acc0_d);
    }
  bessel_end:
    m = 0;    /* meaningless, add to avoid error from compiler */ 
  }

}


