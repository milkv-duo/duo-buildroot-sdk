
/* Include files */
#include <math.h>
#include "define.h"
#include "lpaec_subfun.h"
#ifndef PLATFORM_RTOS
not suppose to enter here
inline void DC_filter(const short *in, float radius, float *out, int len, float *mem)
#else 
/* Function Definitions */
void DC_filter(const short *in, float radius, float *out, int len, float *mem)
#endif
{
   int i;
   float den2;

   den2 = radius*radius + .7f*(1-radius)*(1-radius);

   for (i=0;i<len;i++) {
      float vin = in[i];
      float vout = mem[0] + vin;
      mem[0] = mem[1] + 2*(-vin + radius*vout);  /* history buffer */
      mem[1] = vin - den2 * vout;  /* history buffer */
      out[i] = radius * vout;
   }
}

inline float inner_prod(const float *x, const float *y, int len)
{
   float sum = 0;

   len >>= 1;
   while(len--) {
      float part = 0;
      part = part + (*x++) * (*y++);
      part = part + (*x++) * (*y++);
      sum = sum + part;
   }
   return sum;
}

inline void bin_power_spectrum(const float *X, float *ps, int N)
{
   int i, j;

   ps[0] = X[0] * X[0];
   for (i=1,j=1;i<N-1;i+=2,j++) {  /* half-complex vector */
      ps[j] =  X[i] * X[i] + X[i+1] * X[i+1];
   }
   ps[j] = X[i] * X[i];
}

inline void accum_power_spectrum(const float *X, float *ps, int N)
{
   int i, j;

   ps[0] += (X[0] * X[0]);  /* ps[0] = ps[0] + X[0] * X[0], real part only, the im part in first point is zero */
   for (i=1,j=1;i<N-1;i+=2,j++) {
      ps[j] +=  (X[i] * X[i] + X[i+1] * X[i+1]);
   }
   ps[j] += (X[i] * X[i]);  /* real part only, the im part in (N/2+1)th point is zero */
}

inline void accum_spectral_mul(const float *X, const float *Y, float *acc, int N, int M)
{
   int i,j;

   for (i=0;i<N;i++)
      acc[i] = 0;
   for (j=0;j<M;j++) {
      acc[0] += (X[0]*Y[0]);  /* real part only for the first point of X and Y */
      for (i=1;i<N-1;i+=2) {  /* X[i] and Y[i] are the real part of X and Y respectively, X[i+1] and Y[i+1] are the imaginary part of X and Y respectively */
         acc[i] += (X[i]*Y[i] - X[i+1]*Y[i+1]);  /* real part */
         acc[i+1] += (X[i+1]*Y[i] + X[i]*Y[i+1]);  /* imaginary part */
      }
      acc[i] += (X[i]*Y[i]);  /* real part only for the (N/2+1)th point of X and Y */
      X += N;
      Y += N;
   }
}

inline void weighted_gradient_phi(const float *w, const float p, const float *X, const float *Y, float *prod, int N)
{
   int i, j;
   float W;

   W = p * w[0];  /* real value */
   prod[0] = W * X[0] * Y[0];  /* real part only for the first point of X and Y */
   for (i=1,j=1;i<N-1;i+=2,j++) {
      W = p * w[j];  /* real value */
      /* perform conj(X)*Y, conjugation of X[i] + X[i+1]j = X[i] - X[i+1]j */
      prod[i] = W * (X[i] * Y[i] + X[i+1] * Y[i+1]);  /* real part */
      prod[i+1] = W * (-X[i+1] * Y[i] +  X[i] * Y[i+1]);  /* imaginary part */
   }
   W = p * w[j];  /* real value */
   prod[i] = W * X[i] * Y[i];  /* real part only for the (N/2+1)th point of X and Y */
}

inline void aec_adjust_prop(const float *W, int N, int M, float *prop)
{
   int i, j;
   float max_sum = 1;
   float prop_sum = 1;
   //float max_sum_matlab[20];

   for (i=0;i<M;i++) {
      float tmp = 1;
      for (j=0;j<N;j++)
         /* tmp(m) = tmp(m) + W(k, m) * W(k, m) */
         tmp += (W[i*N+j] * W[i*N+j]);  /* sum the power of the (N/2+1) frequency bin in each taplen */
      prop[i] = (float)math_sqrt(tmp);

      #if (1)  /* better */
      if (prop[i] > max_sum)
         max_sum = prop[i];
      #else
      if (prop[i] < 1.0)
         max_sum_matlab[i] = 1.0;
      else
         max_sum_matlab[i] = prop[i];
      #endif

   }

   for (i=0;i<M;i++) {

      #if (1)  /* better */
      prop[i] += (.1f * max_sum);
      #else
      prop[i] += (.1f * max_sum_matlab[i]);
      #endif

      prop_sum += prop[i];  /* sum(prop(m)) */
   }
   for (i=0;i<M;i++) {
      prop[i] = (.99f * prop[i]) / prop_sum;
   }

}


