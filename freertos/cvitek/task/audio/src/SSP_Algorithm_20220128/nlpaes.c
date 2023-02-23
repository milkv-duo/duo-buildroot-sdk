/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/*%									*/
/*%       nlpaes.c							*/
/*%       Author: Sharon Lee						*/
/*%       History:							*/
/*%                  Created by Sharon Lee in August, 2020		*/
/*%									*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	*/

/* Include files */
#include <math.h>
#include "tmwtypes.h"
#include "define.h"
#include "struct.h"
#include "nlpaes_subfun.h"
#include "memalloc.h"
#include "packfft.h"
#include "lpaec_subfun.h"
#include "nlpaes.h"

#ifdef SSP_STACK_TO_HEAP
   short *mic_raw_std;
   float *tmp1;
   float *tmp2;
#endif

#ifndef SSP_SAFE_FREE_BUF
#define SSP_SAFE_FREE_BUF(OBJ) {if (NULL != OBJ) {free(OBJ); OBJ = NULL; } }
#endif

#ifndef SSP_CALLOC
#define SSP_CALLOC(TYPE, COUNT) ((TYPE *)calloc(COUNT, sizeof(TYPE)))
#endif

/* Function Definitions */
EXPORT void NLP_AES_para(ssp_para_struct *para, float *std_thrd, float *aes_supp_coef1, float *aes_supp_coef2)
{
   ssp_para_struct *para_obj;
   unsigned short tmp;

   /* NLP AES Parameters */
   para_obj = para;

   tmp = MIN(MAX(para_obj->para_aes_std_thrd, 0), 39);    /* the threshold level for residual echo */
   *std_thrd = 0.6f + tmp * 0.01f;    /* [0.6, 0.99], each step 0.01 */

   tmp = MIN(MAX(para_obj->para_aes_supp_coeff, 0), 100);    /* the suppression level for residual echo */
   *aes_supp_coef1 = 1.0f + tmp * 0.125f;    /* [1.0, 13.5], each step 0.125 */
   *aes_supp_coef2 = 2.0f + tmp * 0.25f;    /* [2.0, 27.0], each step 0.25 */

}

EXPORT NonLinearEchoState *NLP_AES_init(int frame_size, float sampling_rate, float std_thrd, float aes_supp_coef1, float aes_supp_coef2)
{
   int i;
   int N, N3, M;

   NonLinearEchoState *st = (NonLinearEchoState *)mem_alloc(sizeof(NonLinearEchoState));
   st->fdstd_thrd = std_thrd;
   st->aes_supp_coef1 = aes_supp_coef1;
   st->aes_supp_coef2 = aes_supp_coef2;
   st->frame_size = frame_size;
   st->ps_size = st->frame_size;
   N = st->ps_size;  /* power spectrum size */
   N3 = 2*N - st->frame_size;
   st->sampling_rate = (int)sampling_rate;

   st->echo_state = NULL;
   st->nbands = 2;
   M = st->nbands;
   st->bank = filterbank_create(M, sampling_rate, N, 1);
   st->frame = (float*)mem_alloc(2*N*sizeof(float));
   st->window = (float*)mem_alloc(2*N*sizeof(float));
   st->ft = (float*)mem_alloc(2*N*sizeof(float));
   st->ps = (float*)mem_alloc((N+M)*sizeof(float));
   st->echo_noise = (float*)mem_alloc((N+M)*sizeof(float));
   st->residual_echo = (float*)mem_alloc((N+M)*sizeof(float));
   st->old_ps = (float*)mem_alloc((N+M)*sizeof(float));
   st->prior = (float*)mem_alloc((N+M)*sizeof(float));
   st->post = (float*)mem_alloc((N+M)*sizeof(float));
   st->gain = (float*)mem_alloc((N+M)*sizeof(float));
   st->gain2 = (float*)mem_alloc((N+M)*sizeof(float));
   st->gain_floor = (float*)mem_alloc((N+M)*sizeof(float));
   st->zeta = (float*)mem_alloc((N+M)*sizeof(float));

   st->inbuf = (float*)mem_alloc(N3*sizeof(float));
   st->outbuf = (float*)mem_alloc(N3*sizeof(float));

   aes_conj_window(st->window, 2*N3);

   for (i=0;i<N+M;i++) {
      st->old_ps[i] = FLOAT_ONE;
      st->gain[i] = FLOAT_ONE;
      st->post[i] = FLOAT_ONE;
      st->prior[i] = FLOAT_ONE;
      st->echo_noise[i] = FLOAT_ZERO;
      st->zeta[0] = FLOAT_ZERO;
   }

   st->echo_sup = -40;
   st->echo_sup_active = -15;

   for (i=0;i<N3;i++) {
      st->inbuf[i] = 0;
      st->outbuf[i] = 0;
   }

   st->fft_lookup = speech_fft_init(2*N);

   st->nb_adapt = 0;
   st->min_count = 0;

   /* STD/DTD */
   st->aes_std_in = (short*)mem_alloc(frame_size*sizeof(short));
   st->mic_raw_ola = (short*)mem_alloc(frame_size*sizeof(short));
   //st->fdstd_mic_in_sm = (float*)mem_alloc(frame_size*sizeof(float));
   //st->fdstd_linear_echo_sm = (float*)mem_alloc(frame_size*sizeof(float));

   for (i=0;i < st->frame_size;i++) {
      st->mic_raw_ola[i] = 0;
      //st->fdstd_mic_in_sm[i] = FLOAT_ZERO;
      //st->fdstd_linear_echo_sm[i] = FLOAT_ZERO;
   }
   st->RER_smooth = FLOAT_ZERO;
   st->RER_vad = 0;
   st->st_vad = 0;
   st->fdstd_vad = 0;
   st->frame1_last_sample = 0;

   return st;

}

EXPORT void NLP_AES_free(NonLinearEchoState *st)
{
   mem_free(st->frame);
   mem_free(st->ft);
   mem_free(st->ps);
   mem_free(st->gain2);
   mem_free(st->gain_floor);
   mem_free(st->window);
   mem_free(st->old_ps);
   mem_free(st->gain);
   mem_free(st->prior);
   mem_free(st->post);
   mem_free(st->echo_noise);
   mem_free(st->residual_echo);
   mem_free(st->zeta);

   mem_free(st->inbuf);
   mem_free(st->outbuf);

   speech_fft_free(st->fft_lookup);
   filterbank_free(st->bank);

   mem_free(st->aes_std_in);
   mem_free(st->mic_raw_ola);
   //mem_free(st->fdstd_mic_in_sm);
   //mem_free(st->fdstd_linear_echo_sm);

   mem_free(st);
#ifdef SSP_STACK_TO_HEAP
   //SSP_SAFE_FREE_BUF(mic_raw_std);
   SSP_SAFE_FREE_BUF(tmp1);
   SSP_SAFE_FREE_BUF(tmp2);
#endif

}

void std_residual_echo(NonLinearEchoState *st, float *residual_echo, int len, boolean_T speech_vad)
{
   int i;
   float leak2;
   int N;
   //float rer_coef;

   N = st->echo_state->window_size;
	len = len;
   /* Apply hanning window (should pre-compute it) */
   for (i=0;i<N;i++)
      st->echo_state->y[i] = st->echo_state->window[i] * st->echo_state->last_y[i];

   /* Compute power spectrum of the linear echo in each bin */
   speech_fft(st->echo_state->fft_table, st->echo_state->y, st->echo_state->Y);
   bin_power_spectrum(st->echo_state->Y, residual_echo, N);  /* powe-based linear echo */

   //rer_coef = 0.75f;
   //st->RER_smooth = rer_coef*st->RER_smooth + (FLOAT_ONE-rer_coef)*st->echo_state->RER;  /* not better */
   //if (st->RER_smooth*32767 > 2000)
   if (st->echo_state->RER*32767 > 3000)
   	st->RER_vad = 1;
   else
   	st->RER_vad = 0;


   {
#ifdef SSP_STACK_TO_HEAP
      if (tmp1 == NULL)
         tmp1 = SSP_CALLOC(float, 320);
      if (tmp2 == NULL)
         tmp2 = SSP_CALLOC(float, 320);

      memset(tmp1, 0, 320 * sizeof(float));
      memset(tmp2, 0, 320 * sizeof(float));
#else
      float tmp1[320], tmp2[320];
#endif
      float fdstd_tau, fdstd_tau_sum;
      int start_f, end_f;
   
      for (i=0;i < st->frame_size;i++) {
        tmp1[i] = st->mic_raw_ola[i];  /* previous frame */
        tmp1[i+st->frame_size] = st->aes_std_in[i];  /* current frame */
        st->mic_raw_ola[i] = st->aes_std_in[i];    /* update input overlap data */
      }

      for (i=0;i<N;i++)
        tmp1[i] = st->echo_state->window[i] * tmp1[i];
      speech_fft(st->echo_state->fft_table, tmp1, tmp2);
      bin_power_spectrum(tmp2, tmp1, N);

      fdstd_tau_sum = FLOAT_ZERO;
      start_f = (int)(ceil(100 * N / st->sampling_rate));
      end_f = (int)(floor(3800 * N / st->sampling_rate));
      for (i = start_f; i <= end_f; i++) {
        fdstd_tau = (float)sqrt(residual_echo[i]/(tmp1[i] + 0.000000000465f));  /* add small DC to avoid zero denominator */
        fdstd_tau_sum += fdstd_tau;
      }

      if (fdstd_tau_sum > (float)(end_f - start_f + 1))  /* fix abnormal situation */
        fdstd_tau_sum = (float)(end_f - start_f + 1);

      st->fdstd_tau_mean = fdstd_tau_sum/(end_f - start_f + 1);  /* frame-based */
      if (st->fdstd_tau_mean > st->fdstd_thrd)
        st->fdstd_vad = 1;
      else
        st->fdstd_vad = 0;

      if ((st->RER_vad) && (st->fdstd_vad))
        st->st_vad = 1;
      else
        st->st_vad = 0;

   }

   if (st->st_vad) {
         if (st->echo_state->leak_estimate > .5)  /* the bigger the value, the more abrupt the signal */
            leak2 = st->aes_supp_coef1;
         else
            leak2 = st->aes_supp_coef2 * st->echo_state->leak_estimate;
   } else {
         if (!(st->RER_vad) && (speech_vad)) { 
           leak2 = 0.0f;  /* keep clean speech signal not to be attenuated */
         } else {
           if (st->echo_state->leak_estimate > .5)
              leak2 = 1.0f;
           else
              leak2 = 2.0f * st->echo_state->leak_estimate;
         }
   }

   for (i=0;i<=st->frame_size;i++) {
      if (st->st_vad)
         residual_echo[i] = leak2 * residual_echo[i];
      else
         residual_echo[i] = (short)(leak2 * residual_echo[i]);
   }

}

EXPORT void NLP_AES(NonLinearEchoState *aes_state, short *y, float aes_frame, boolean_T speech_vad)
{
   int i;
   int M;
   int N;
   int N3;
   float *ps;
   float Zframe;
   float Pframe;
   float beta, beta_1;
   float effective_echo_suppress;
   NonLinearEchoState *st;
   float alpha, alpha2;
   float y_tmp[160];

   st = aes_state;
   N = st->ps_size;
   N3 = 2*N - st->frame_size;
   ps = st->ps;

   st->nb_adapt++;
   if (st->nb_adapt>20000)
      st->nb_adapt = 20000;
   st->min_count++;

   beta = MAX(.03f, (float)(FLOAT_ONE / st->nb_adapt));
   beta_1 = FLOAT_ONE - beta;
   beta_1 = beta_1;
   M = st->nbands;

   /* Deal with residual echo if provided */
   if (st->echo_state) {
      std_residual_echo(st, st->residual_echo, N, speech_vad);

      /* If there are ridiculous values, it'll show up in the DC and just reset everything to zero */
      if (!(st->residual_echo[0] >=0 && st->residual_echo[0]<N*1e9f)) {
         for (i=0;i<N;i++)
            st->residual_echo[i] = 0;
      }

      for (i=0;i<N;i++)
         st->echo_noise[i] = MAX(.6f * st->echo_noise[i], st->residual_echo[i]);  /* power-based */
      filterbank_linear2sb(st->bank, st->echo_noise, st->echo_noise+N);
   } else {
      for (i=0;i<N+M;i++)
         st->echo_noise[i] = 0;
   }

   aes_analysis(st, y);

   /* Special case for first frame */
   if (st->nb_adapt==1)
      for (i=0;i<N+M;i++)
         st->old_ps[i] = ps[i];

   for (i=0;i<N+M;i++) {
      float gamma;

      /* Total noise estimate */
      float tot_noise = 1.f + st->echo_noise[i];

      /* Posteriori SNR */
      st->post[i] = ps[i] / tot_noise - 1.f;
      st->post[i]=MIN(st->post[i], 100.f);

      /* Update gamma */
      gamma = .1f + .89f * SQR(st->old_ps[i] / (st->old_ps[i] + tot_noise));

      /* Priori SNR */
      st->prior[i] = gamma * MAX(0, st->post[i]) + (FLOAT_ONE-gamma) * (st->old_ps[i] / tot_noise);
      st->prior[i] = MIN(st->prior[i], 100.f);
   }

   /* Recursive average of the priori SNR */
   st->zeta[0] = .7f * st->zeta[0] + .3f * st->prior[0];
   for (i=1;i<N-1;i++)
      st->zeta[i] = .7f * st->zeta[i] + .15f * st->prior[i] + .075f * st->prior[i-1] + .075f * st->prior[i+1];
   for (i=N-1;i<N+M;i++)
      st->zeta[i] = .7f * st->zeta[i] + .3f * st->prior[i];

   Zframe = 0;
   for (i=N;i<N+M;i++)
      Zframe = Zframe + st->zeta[i];

   Pframe = .1f + .899f * aes_qcurve(Zframe / st->nbands);
   effective_echo_suppress = (FLOAT_ONE - Pframe) * st->echo_sup + Pframe * st->echo_sup_active;
   aes_gain_floor(effective_echo_suppress, st->echo_noise+N, st->gain_floor+N, M);
         
   /* Compute speech probability of presence */
   for (i=N;i<N+M;i++) {
      float theta;
      float MM;  /* gain from hypergeometric function */
      float prior_ratio;  /* Weiner filter gain */
      float P1;  /* priority probability of speech presence */
      float q;  /* speech absence the priori probability */

      prior_ratio = st->prior[i] / (st->prior[i] + 1.f);
      theta = prior_ratio * (1.f + st->post[i]);

      MM = aes_hypergeom_gain(theta);
      st->gain[i] = MIN(FLOAT_ONE, prior_ratio * MM);  /* gain with bound */
      st->old_ps[i] = .2f * st->old_ps[i] + .8f * SQR(st->gain[i]) * ps[i];  /* save old power spectrum */

      P1 = .199f + .8f * aes_qcurve(st->zeta[i]);
      q = FLOAT_ONE - Pframe * P1;
      st->gain2[i] = 1/(1.f + (q/(1.f-q))*(1+st->prior[i])*(float)exp(-theta));  /* lambda/(1+lambda) */

   }

   filterbank_sb2linear(st->bank,st->gain2+N, st->gain2);
   filterbank_sb2linear(st->bank,st->gain+N, st->gain);
   filterbank_sb2linear(st->bank,st->gain_floor+N, st->gain_floor);

   /* Compute gain */
   for (i=0;i<N;i++) {
      float MM;
      float theta;
      float prior_ratio;
      float tmp;
      float p;
      float g;

      prior_ratio = st->prior[i] / (st->prior[i] + 1.f);
      theta = prior_ratio * (1.f + st->post[i]);

      /* Optimal estimator for loudness domain */
      MM = aes_hypergeom_gain(theta);
      g = MIN(FLOAT_ONE, prior_ratio * MM);  /* gain with bound */
      p = st->gain2[i];  /* interpolated speech probability of presence */

      /* Constrain the gain to be close to the scale gain */
      if ((.333f *g) > st->gain[i])
         g = 3.f * st->gain[i];
      st->gain[i] = g;

      /* save old power spectrum */
      st->old_ps[i] = .2f * st->old_ps[i] + .8f * SQR(st->gain[i]) * ps[i];

      /* Apply gain floor */
      if (st->gain[i] < st->gain_floor[i])
         st->gain[i] = st->gain_floor[i];
         
      /* Take into account speech probability of presence */
      tmp = p * (float)math_sqrt(st->gain[i]) + (FLOAT_ONE -p) * (float)math_sqrt(st->gain_floor[i]);
      st->gain2[i] = SQR(tmp);

      //if (st->st_vad)
      //if (st->RER_vad)
         st->gain2[i] = st->gain2[i] * st->gain[i];

      }


   for (i=1;i<N;i++) {
      st->ft[2*i-1] = st->gain2[i] * st->ft[2*i-1];
      st->ft[2*i] = st->gain2[i] * st->ft[2*i];
   }
   st->ft[0] = st->gain2[0] * st->ft[0];
   st->ft[2*N-1] = st->gain2[N-1] * st->ft[2*N-1];

   speech_ifft(st->fft_lookup, st->ft, st->frame);

   /* Apply synthesis window (for WOLA) */
   for (i=0;i<2*N;i++)
      st->frame[i] = st->frame[i] * st->window[i];

   /* Perform overlap and add */
   for (i=0;i<N3;i++) {
      y_tmp[i] = st->outbuf[i] + st->frame[i];
   }

   if (1 == (unsigned short)aes_frame)
      st->frame1_last_sample = y_tmp[N3-1];
   if (2 == (unsigned short)aes_frame) {
      alpha2 = 1.0F/160.0F;
      alpha = 1 - alpha2;
      y_tmp[0] = alpha * st->frame1_last_sample + alpha2 * y_tmp[0];
      for (i = 0; i < N3-1; i++) {
         alpha2 = (float)(1.0F + i)/160.0F;
         alpha = 1 - alpha2;
         y_tmp[i+1] =  alpha * y_tmp[i] + alpha2 * y_tmp[i+1];
      }
   }

   for (i=0;i<N3;i++)
      y[i] = (short)WORD2INT(y_tmp[i]);

   /* Update OLA outbuf */
   for (i=0;i<N3;i++)
      st->outbuf[i] = st->frame[st->frame_size+i];  /* output 50% OLA */

}


