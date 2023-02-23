
/* Include files */
#include <math.h>
#include "define.h"
#include "memalloc.h"
#include "packfft.h"
#include "struct.h"
#include "lpaec.h"
#include "lpaec_subfun.h"

/* Function Definitions */
EXPORT void LP_AEC_para(ssp_para_struct *para, short *filter_length, short *init_filter_length)
{
   ssp_para_struct *para_obj;
   unsigned short tmp;

   /* LP AEC Parameters */
   para_obj = para;

   tmp = MIN(MAX(para_obj->para_aec_init_filter_len, 1), 13);    /* 8kHz: [20ms, 260ms], each step 20ms; 16kHz: [10ms, 130ms], each step 10ms */
   *init_filter_length = tmp;

   tmp = MIN(MAX(para_obj->para_aec_filter_len, 1), 13);    /* 8kHz: [20ms, 260ms], each step 20ms; 16kHz: [10ms, 130ms], each step 10ms */
   *filter_length = tmp;

}

EXPORT LinearEchoState *LP_AEC_init(int frame_size, short filter_length, short init_filter_length, float Fs)
{

   int i, N, M;
   LinearEchoState *st = (LinearEchoState *)mem_alloc(sizeof(LinearEchoState));

   st->frame_size = frame_size;
   st->window_size = 2*frame_size;
   N = st->window_size;
   st->M = filter_length;
   st->initial_filter_len = init_filter_length;
   M = st->initial_filter_len;

   st->ss = .35f;  // fe_smooth_coef;
   st->cancel_count = 0;
   st->sum_adapt = 0;
   st->saturated = 0;
   st->screwed_up = 0;

   st->sampling_rate = (int)Fs;
   st->spec_average = (float)st->frame_size/(float)st->sampling_rate;
   st->beta0 = (2.0f*st->frame_size)/st->sampling_rate;
   st->beta_max = (.5f*st->frame_size)/st->sampling_rate;
   st->leak_estimate = 0;

   st->fft_table = speech_fft_init(N);

   st->e = (float*)mem_alloc(N*sizeof(float));
   st->x = (float*)mem_alloc(N*sizeof(float));
   st->input = (float*)mem_alloc(st->frame_size*sizeof(float));
   st->y = (float*)mem_alloc(N*sizeof(float));
   st->last_y = (float*)mem_alloc(N*sizeof(float));
   st->Yf = (float*)mem_alloc((st->frame_size+1)*sizeof(float));
   st->Rf = (float*)mem_alloc((st->frame_size+1)*sizeof(float));
   st->Xf = (float*)mem_alloc((st->frame_size+1)*sizeof(float));
   st->Yh = (float*)mem_alloc((st->frame_size+1)*sizeof(float));
   st->Eh = (float*)mem_alloc((st->frame_size+1)*sizeof(float));

   st->X = (float*)mem_alloc((13+1)*N*sizeof(float));
   st->Y = (float*)mem_alloc(N*sizeof(float));
   st->E = (float*)mem_alloc(N*sizeof(float));
   st->W = (float*)mem_alloc(13*N*sizeof(float));
#ifdef TWO_FILTER
   st->foreground = (float*)mem_alloc(13*N*sizeof(float));
#endif
   st->PHI = (float*)mem_alloc(N*sizeof(float));
   st->power = (float*)mem_alloc((frame_size+1)*sizeof(float));
   st->power_1 = (float*)mem_alloc((frame_size+1)*sizeof(float));
   st->window = (float*)mem_alloc(N*sizeof(float));
   st->prop = (float*)mem_alloc(13*sizeof(float));
   st->wtmp = (float*)mem_alloc(N*sizeof(float));
   for (i=0;i<N;i++)
      st->window[i] = .5f-.5f*(float)cos(2*M_PI*i/N);	/* Hanning window */
   for (i=0;i<=st->frame_size;i++)
      st->power_1[i] = FLOAT_ONE;
   for (i=0;i<N*13;i++)
      st->W[i] = 0;
   {
      float sum = 0;
      /* Ratio of ~10 between adaptation rate of first and last block */
      float decay = (float)math_exp(-(2.4/M));
      st->prop[0] = .7f;
      sum = st->prop[0];
      for (i=1;i<M;i++) {
         st->prop[i] = st->prop[i-1] * decay;
         sum = sum + st->prop[i];
      }
      for (i=M-1;i>=0;i--) {
         st->prop[i] = (.8f * st->prop[i])/sum;
      }
   }

   st->memX = (float*)mem_alloc(sizeof(float));
   st->memD = (float*)mem_alloc(sizeof(float));
   st->memE = (float*)mem_alloc(sizeof(float));
   st->preemph = .9f;

   if (st->sampling_rate<12000)
      st->notch_radius = .9f;
   else if (st->sampling_rate<24000)
      st->notch_radius = .982f;
   else
      st->notch_radius = .992f;
   st->notch_mem = (float*)mem_alloc(2*sizeof(float));
   for (i=0;i<2;i++)
      st->notch_mem[i] = 0;

   st->adapted = 0;
   st->Pey = st->Pyy = FLOAT_ONE;

#ifdef TWO_FILTER
   st->Davg1 = st->Davg2 = FLOAT_ZERO;
   st->Dvar1 = st->Dvar2 = FLOAT_ZERO;
#endif

   st->aec_min_leak = .005f;
   st->aec_var1_smooth = .36f;
   st->aec_var2_smooth = .7225f;
   st->aec_var1_update = .5f;
   st->aec_var2_update = .25f;
   st->aec_var_backtrack = 4.f;

   st->echo_cnt = 0;

   return st;

}

void echo_state_reset(LinearEchoState *st)
{

   int i, M, N;

   st->cancel_count = 0;
   st->screwed_up = 0;
   N = st->window_size;
   M = st->M;

   M = M;
   for (i=0;i<N*13;i++)
      st->W[i] = 0;
#ifdef TWO_FILTER
   for (i=0;i<N*13;i++)
      st->foreground[i] = 0;
#endif
   for (i=0;i<N*(13+1);i++)
      st->X[i] = 0;
   for (i=0;i<=st->frame_size;i++) {
      st->power[i] = 0;
      st->power_1[i] = FLOAT_ONE;
      st->Eh[i] = 0;
      st->Yh[i] = 0;
   }
   for (i=0;i<st->frame_size;i++) {
      st->last_y[i] = 0;
   }
   for (i=0;i<N;i++) {
      st->E[i] = 0;
   }
   for (i=0;i<N;i++) {
      st->x[i] = 0;
   }
   for (i=0;i<2;i++)
      st->notch_mem[i] = 0;
   for (i=0;i<1;i++)
      st->memD[i] = st->memE[i] = 0;
   for (i=0;i<1;i++)
      st->memX[i] = 0;

   st->saturated = 0;
   st->adapted = 0;
   st->sum_adapt = 0;
   st->Pey = st->Pyy = FLOAT_ONE;
#ifdef TWO_FILTER
   st->Davg1 = st->Davg2 = FLOAT_ZERO;
   st->Dvar1 = st->Dvar2 = FLOAT_ZERO;
#endif

}

EXPORT void LP_AEC_free(LinearEchoState *st)
{

   speech_fft_free(st->fft_table);

   mem_free(st->e);
   mem_free(st->x);
   mem_free(st->input);
   mem_free(st->y);
   mem_free(st->last_y);
   mem_free(st->Yf);
   mem_free(st->Rf);
   mem_free(st->Xf);
   mem_free(st->Yh);
   mem_free(st->Eh);

   mem_free(st->X);
   mem_free(st->Y);
   mem_free(st->E);
   mem_free(st->W);
#ifdef TWO_FILTER
   mem_free(st->foreground);
#endif
   mem_free(st->PHI);
   mem_free(st->power);
   mem_free(st->power_1);
   mem_free(st->window);
   mem_free(st->prop);
   mem_free(st->wtmp);
   mem_free(st->memX);
   mem_free(st->memD);
   mem_free(st->memE);
   mem_free(st->notch_mem);

   mem_free(st);
}

EXPORT void LP_AEC(LinearEchoState *st, const short *in, const short *far_end, short *out)
{

   int i , j;
   int N, M;
   float Syy, See, Sxx, Sdd, Sff;
#ifdef TWO_FILTER
   float Dbf;  /* the variance of e(n) */
   int update_foreground;  /* update foreground flag */
#endif
   float Sey;
   float ss, ss_1;
   float Pey = FLOAT_ONE, Pyy = FLOAT_ONE;
   float alpha, alpha_1;
   float RER;  /* Residual to Error Ratio */
   float tmp32;


   if (st->echo_cnt > (st->sampling_rate/st->frame_size*3)) {
      M = st->M;  /* from parameter */
      st->echo_cnt = (st->sampling_rate/st->frame_size*3)+1;  /* avoid cnt overflow */
   } else {
      M = st->initial_filter_len;
   }

   N = st->window_size;
   st->cancel_count++;

   //ss = 0.3f / M;
   ss = st->ss / M;
   ss_1 = 1 - ss;


   /******************************************************************************/
   /*                                                     TX Signal Preprocessing*/
   /******************************************************************************/
   /* Apply DC filter to make sure DC doesn't end up causing problems */
   DC_filter(in, st->notch_radius, st->input, st->frame_size, st->notch_mem);

   /* Copy input data to buffer and apply pre-emphasis filter */
   for (i=0;i<st->frame_size;i++) {
      float tmp32;
      /* H = 1-0.9z^(-1), FIR filter */
      /* tmp32 = input(n) - preemph * input(n-1), input(n) is d(n) */
      tmp32 = st->input[i] - st->preemph * st->memD[0];
      st->memD[0] = st->input[i];
      st->input[i] = tmp32;
   }

   /******************************************************************************/
   /*                                                     RX Signal Preprocessing*/
   /******************************************************************************/
   for (i=0;i<st->frame_size;i++) {
      float tmp32;
      /* x[1:2*frame_size]: |--previous frame--|--current frame--| */
      st->x[i] = st->x[i+st->frame_size];  /* perform pre-emphasis filtering for previous frame */
      /* tmp32 = far_end(n) - preemph * far_end(n -1) */
      tmp32 = far_end[i] - st->preemph * st->memX[0];
      st->x[i+st->frame_size] = tmp32;  /* perform pre-emphasis filtering for current frame */
      st->memX[0] = far_end[i];
   }

   /* Shift the filter length buffer of far-end in frequency domain */
   for (j=M-1;j>=0;j--) {
      for (i=0;i<N;i++)
         st->X[(j+1)*N+i] = st->X[j*N+i];
   }
   speech_fft(st->fft_table, st->x, &st->X[0]);

   Sxx = 0;
   Sxx += inner_prod(st->x+st->frame_size, st->x+st->frame_size, st->frame_size);  /* the variance of x(n) */
   accum_power_spectrum(st->X, st->Xf, N);  /* Xf(k) = pow(X(k)), the power spectrum of X(k) */

   Sff = 0;
#ifdef TWO_FILTER
   /* Compute foreground filter */
   /* Y(k) = X(k) * W(k) */
   accum_spectral_mul(st->X, st->foreground, st->Y, N, M);
   speech_ifft(st->fft_table, st->Y, st->e);
   for (i=0;i<st->frame_size;i++) {
	/* e(n) = d(n) - y(n), e(n) use the first part, y(n) use the last part */
	st->e[i] = st->input[i] - st->e[i+st->frame_size];
   }
   /* the variance of e(n) */
   Sff += inner_prod(st->e, st->e, st->frame_size);
#endif

   /* Adjust proportional adaption rate */
   if (st->adapted)
      aec_adjust_prop (st->W, N, M, st->prop);
   if (st->saturated == 0) {
      for (j=M-1;j>=0;j--) {
         /* Compute delta_W(k, m, j), PHI(i) is delta_W(k, m, j) */
         /* delta_W(k, m, j) = power_1(k) * prop(m) * conj[X(k, m, j-1)] * E(k, j-1)*/
         /* st->power_1: optima l step size, length is frame size+1 */
         /* st->prop:         proportianal step size, length is M */
         /* st->X:                focus on j+1, means the previous X state, X length is (M+1)*N */
         /* st->E:                error in frequency domain */
         /*                             [0] = matlab[1].re (matlab[1].im = 0), [1] = [2].re, [2] = [2].im, [3] = [3].re, [4] = [3].im, ..., */
         /*                             [317] = [160].re, [318] = [160].im, [319] = [161].re (matlab[161].im = 0) */
         /* st->PHI:            length is window size N */
         weighted_gradient_phi(st->power_1, st->prop[j], &st->X[(j+1)*N], st->E, st->PHI, N);
	 /* compute weight gradient */
         for (i=0;i<N;i++)  /* only calculate (N/2+1) frequency bin, two data(re/im) in each bin, so N data in total */
	    /* W(k, m, j) = W(k, m, j-1) + delta_W(k, m, j) */
            st->W[ j*N + i] += st->PHI[i];
      }
   } else {
      st->saturated--;
   }

   /* Overlap-store method to let circular convolution become linear convolution */
   for (j=0;j<M;j++) {
      //if (j==0 || ((2+st->cancel_count)%(M-1)) == j+1) {
      if (j==0 || st->cancel_count%(M-1) == j-1) {  /* better */
         speech_ifft(st->fft_table, &st->W[j*N], st->wtmp);
         for (i=st->frame_size;i<N;i++) {
            st->wtmp[i] = 0;  /* set 0 to the last part */
         }
         speech_fft(st->fft_table, st->wtmp, &st->W[j*N]);
      }
   }

   /* Reset to zeros for accumulation */
   for (i=0;i<=st->frame_size;i++)
      st->Rf[i] = st->Yf[i] = st->Xf[i] = 0;

   Dbf = 0;
   See = 0;
#ifdef TWO_FILTER
   /* Difference in response, this is used to estimate the variance of the residual power estimate */
   /* st.Y(:) = st.Y(:) + st.X(:, j) .* st.W(:, j); */
   accum_spectral_mul(st->X, st->W, st->Y, N, M);
   /* st.y : [ ~ | leak background ] */
   speech_ifft(st->fft_table, st->Y, st->y);
   for (i=0;i<st->frame_size;i++) {
      st->e[i] = st->e[i+st->frame_size] - st->y[i+st->frame_size];
   }
   Dbf += 10 + inner_prod(st->e, st->e, st->frame_size);  /* the variance of e(n) */
   for (i=0;i<st->frame_size;i++)
      st->e[i] = st->input[i] - st->y[i+st->frame_size];
   See += inner_prod(st->e, st->e, st->frame_size);  /* the variance of e(n) */
#endif

#ifndef TWO_FILTER
   Sff = See;
#endif

#ifdef TWO_FILTER
   /* For two time windows, compute the mean of the energy difference, as well as the variance */
   st->Davg1 = .6f * st->Davg1 + .4f * (Sff - See);
   st->Davg2 = .85f * st->Davg2 + .15f * (Sff - See);
   st->Dvar1 = st->aec_var1_smooth * st->Dvar1 + .16f * Sff * Dbf ;
   st->Dvar2 = st->aec_var2_smooth * st->Dvar2 + .0225f * Sff * Dbf ;

   update_foreground = 0;
   /* Inspect if there is a statistically significant reduction in the residual echo */
   if (((Sff - See) * ABS(Sff - See)) > (Sff * Dbf))
      update_foreground = 1;
   else if ((st->Davg1 * ABS(st->Davg1)) > (st->aec_var1_update * st->Dvar1))
      update_foreground = 1;
   else if ((st->Davg2 * ABS(st->Davg2)) > (st->aec_var2_update * st->Dvar2))
      update_foreground = 1;

   if (update_foreground) {  /* update foreground filter */
      st->Davg1 = st->Davg2 = FLOAT_ZERO;
      st->Dvar1 = st->Dvar2 = FLOAT_ZERO;
      /* Copy background filter to foreground filter */
      for (i=0;i<N*M;i++)  /* copy data for (N/2+1) frequency bin */
         st->foreground[i] = st->W[i];
      /* Apply a smooth transition to avoid blocking artifacts */
      /* y1(n) = window(n+N/2)y1(n) + window(n)y2(n) */
      for (i=0;i<st->frame_size;i++)
         /* Perform windowing and 50% OLA, output = window first part * leak background(new) + window last part * leak forground(old) */
	 /* st.e last part still be leak(y) foreground */
         st->e[i+st->frame_size] = st->window[i+st->frame_size] * st->e[i+st->frame_size] + st->window[i] * st->y[i+st->frame_size];

   } else {  /* check if the background filter is significantly worse */
      int reset_background = 0;
      if ((-(Sff - See) * ABS(Sff - See)) > (st->aec_var_backtrack * Sff * Dbf))
         reset_background = 1;
      if ((-(st->Davg1) * ABS(st->Davg1)) > (st->aec_var_backtrack * st->Dvar1))
         reset_background = 1;
      if ((-(st->Davg2) * ABS(st->Davg2)) > (st->aec_var_backtrack * st->Dvar2))
         reset_background = 1;

      if (reset_background) {
         /* Copy foreground filter to background filter */
         for (i=0;i<N*M;i++)  /* copy data for (N/2+1) frequency bin */
            st->W[i] = st->foreground[i];
         for (i=0;i<st->frame_size;i++)
            st->y[i+st->frame_size] = st->e[i+st->frame_size];
         for (i=0;i<st->frame_size;i++)
            st->e[i] = st->input[i] - st->y[i+st->frame_size];

         See = Sff;  /* reset error power, copy foreground to background */
         st->Davg1 = st->Davg2 = FLOAT_ZERO;
         st->Dvar1 = st->Dvar2 = FLOAT_ZERO;
      }
   }
#endif

   Sey = Syy = Sdd = 0;
   /* Compute error signal (for the output with de-emphasis) */
   for (i=0;i<st->frame_size;i++) {
      float tmp_out;
#ifdef TWO_FILTER
      /* The estimated echo e is processed after windowing and OLA */
      tmp_out = st->input[i] - st->e[i+st->frame_size];
#else
      tmp_out = st->input[i] - st->y[i+st->frame_size];
#endif
      /* H = 1/(1-0.9z^(-1)), IIR, de-emphasis filtering */
      tmp_out = tmp_out + st->preemph * st->memE[0];
	/* this is an arbitrary test for saturation in the mic signal */
      if (in[i] <= -32000 || in[i] >= 32000) {
         if (st->saturated == 0)
            st->saturated = 1;
      }
      out[i] = (short)WORD2INT(tmp_out);  /* the output data is processed after windowing and OLA cuz e(n) */
      st->memE[0] = tmp_out;  /* state out(n-1) */
   }

   /* Compute error signal */
   for (i=0;i<st->frame_size;i++) {
      st->e[i+st->frame_size] = st->e[i];
      st->e[i] = 0;
   }

   /* Compute a bunch of correlations */
   Sey += inner_prod(st->e+st->frame_size, st->y+st->frame_size, st->frame_size);  /* the covariance of e(n) and y(n) */
   Syy += inner_prod(st->y+st->frame_size, st->y+st->frame_size, st->frame_size);  /* the variance of y(n) */
   Sdd += inner_prod(st->input, st->input, st->frame_size);  /* the variance of d(n) */

   speech_fft(st->fft_table, st->e, st->E);
   for (i=0;i<st->frame_size;i++)
      st->y[i] = 0;
   speech_fft(st->fft_table, st->y, st->Y);

   accum_power_spectrum(st->E, st->Rf, N);  /* Rf(k) = the power spectrum of E(k), error power in each frequency bin (N/2+1) */
   accum_power_spectrum(st->Y, st->Yf, N);  /* Yf(k) = the power spectrum of Y(k) , estimated echo power in each frequency bin (N/2+1) */


   /* Sanity check */
   if (!(Syy>=0 && Sxx>=0 && See >= 0)) {  /* things have gone really bad */
      st->screwed_up += 50;
      for (i=0;i<st->frame_size;i++)
         out[i] = 0;
   } else if (Sff > (Sdd + (float)(N * 10000))) {  /* if foreground error signal is a lot greater than mic in */
      st->screwed_up++;
   } else {  /* everything is fine */
      st->screwed_up = 0;
   }

   if (st->screwed_up>=50) {
      //speech_warning("AEC started behaving unreasonable and need to reset");
      echo_state_reset(st);
      return;
   }

   See = MAX(See, (float)(N * 100));  /* add a small noise floor to make sure not to have problems when dividing */

   Sxx = 0;
   Sxx += inner_prod(st->x+st->frame_size, st->x+st->frame_size, st->frame_size);  /* far-end (TD) current frame power sum */
   accum_power_spectrum(st->X, st->Xf, N);  /* Xf(k) = far-end(FD) power in each frequency bin (N/2+1) */

   for (j=0;j<=st->frame_size;j++)
      st->power[j] = ss_1 * st->power[j] + 1 + ss * st->Xf[j];  /* smooth far end power estimate over time */

   /* Compute filtered spectra and cross-correlations */
   for (j=st->frame_size;j>=0;j--) {
      float Eh, Yh;

      Eh = st->Rf[j] - st->Eh[j];
      Yh = st->Yf[j] - st->Yh[j];
      Pey = Pey + Eh * Yh;
      Pyy = Pyy + Yh * Yh;
      st->Eh[j] = (1-st->spec_average)*st->Eh[j] + st->spec_average*st->Rf[j];
      st->Yh[j] = (1-st->spec_average)*st->Yh[j] + st->spec_average*st->Yf[j];
   }

   Pyy = (float)math_sqrt(Pyy);  /* the standard deviation of Y(k) */
   Pey = Pey / Pyy;  /* the correlation coefficient of E(k) and Y(k) */

   /* Compute correlation update rate */
   /* beta0 is the base learning rate for the leakage estimate */
   tmp32 = st->beta0 * Syy;
   if (tmp32 > (st->beta_max * See))
      tmp32 = st->beta_max * See;
   alpha = tmp32 / See;
   alpha_1 = FLOAT_ONE - alpha;

   /* Update correlations (recursive average) */
   st->Pey = alpha_1 * st->Pey + alpha * Pey;
   st->Pyy = alpha_1 * st->Pyy + alpha * Pyy;

   if (st->Pyy < FLOAT_ONE)
      st->Pyy = FLOAT_ONE;

   if (st->Pey < st->aec_min_leak * st->Pyy)
      st->Pey = st->aec_min_leak * st->Pyy;

   if (st->Pey > st->Pyy)
      st->Pey = st->Pyy;

   st->leak_estimate = st->Pey / st->Pyy;
   if (st->leak_estimate > 16383)
      st->leak_estimate = 32767;


   /* Compute Residual to Error Ratio */
   RER = (float)((.0001*Sxx + 3*st->leak_estimate*Syy) / See);
   if (RER < Sey*Sey/(1+See*Syy))
      RER = Sey*Sey/(1+See*Syy);
   if (RER > .5)
      RER = .5;
   st->RER = RER;

   if (st->RER*32767 > 1000)
      st->echo_cnt++;

   if (!st->adapted && st->sum_adapt > M && st->leak_estimate*Syy > .03f*Syy) {
      st->adapted = 1;
   }

   if (st->adapted) {
      /* Normal learning rate calculation once it's past the minimal adaptation phase */
      for (i=0;i<=st->frame_size;i++) {
         float r, e;
         //int tmp;

         #if (0)  /* unhelpful */
         tmp = st->sampling_rate;
         if ((i*tmp/st->window_size<=1000) && (i*tmp/st->window_size>=600)) {
            st->leak_estimate = st->leak_estimate - 0.03f;
            if (st->leak_estimate < 0) {
                st->leak_estimate = 0;
            }
         } else if (i*tmp/st->window_size>1000) {
            st->leak_estimate = st->leak_estimate - 0.1f;
            if (st->leak_estimate < 0) {
                st->leak_estimate = 0;
            }
         }
         #endif

         /* Compute frequency-domain adaptation mask */
         r = st->leak_estimate * st->Yf[i];
         e = st->Rf[i] + 1;  /* the power spectrum of E(k) */
         if (r>.5f*e)
            r = .5f*e;

         #if (0)
         r = .3f * r + .7f * RER * e;
         #else  /* better */
         r = .7f * r + .3f * RER * e;
         #endif

         //st->power_1[i] = (r * adapt_rate) / (e * (st->power[i] + 1));
         st->power_1[i] = r / (e * (st->power[i] + 10));

      }
   } else {

      /* Temporary adaption rate if filter is not yet adapted enough */
      float adapt_rate = 0;

      if (Sxx > (float)(N * 1000)) {  /* far-end exists a certain level of energy */
         tmp32 = .25f * Sxx;
         if (tmp32 > .25*See)
            tmp32 = (float)(.25*See);
         adapt_rate = tmp32 / See;  /* residual echo/error signal */
      }

      for (i=0;i<=st->frame_size;i++)
         st->power_1[i] = adapt_rate / (st->power[i] + 10);

      /* How much has it adapted so far */
      st->sum_adapt = st->sum_adapt + adapt_rate;  /* the bigger the value, the bigger the echo */
   }

   #if (1)
   for (i=0;i<st->frame_size;i++)
      st->last_y[i] = st->last_y[st->frame_size+i];
   if (st->adapted) {
      /* if the filter is adapted, take the filtered echo */
      for (i=0;i<st->frame_size;i++)
         st->last_y[st->frame_size+i] = (float)(in[i]-out[i]);
   } else {
      /* otherwise, take the far-end signal */
      for (i=0;i<N;i++)
         st->last_y[i] = st->x[i];
   }
   #endif

}


