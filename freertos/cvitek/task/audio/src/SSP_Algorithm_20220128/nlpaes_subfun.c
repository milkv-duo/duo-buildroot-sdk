
/* Include files */
#include <math.h>
#include "define.h"
#include "struct.h"
#include "memalloc.h"
#include "packfft.h"
#include "nlpaes_subfun.h"

/* Function Definitions */
FilterBank *filterbank_create(int banks, float sampling, int len, int type)
{
   FilterBank *bank;
   float df;
   float max_mel, mel_interval;
   int i;
   int id1;
   int id2;

   type = type;
   df = sampling / (float)(2 * len);
   max_mel = (float)toBARK(sampling/2);
   mel_interval = max_mel / (float)(banks - 1);
   
   bank = (FilterBank*)mem_alloc(sizeof(FilterBank));
   bank->nb_banks = banks;
   bank->len = len;
   bank->bank_left = (int*)mem_alloc(len*sizeof(int));
   bank->bank_right = (int*)mem_alloc(len*sizeof(int));
   bank->filter_left = (float*)mem_alloc(len*sizeof(float));
   bank->filter_right = (float*)mem_alloc(len*sizeof(float));
   
   //bank->scaling = (float*)mem_alloc(banks*sizeof(float));

   for (i=0;i<len;i++) {
      float curr_freq;
      float mel;
      float val;
      curr_freq = (float)(i * df);
      mel = (float)toBARK(curr_freq);
      if (mel > max_mel)
         break;

      id1 = (int)(floor(mel/mel_interval));
      if (id1>banks-2) {
         id1 = banks-2;
         val = FLOAT_ONE;
      } else {
         val = (mel - id1*mel_interval) / mel_interval;
      }
      id2 = id1+1;
      bank->bank_left[i] = id1;
      bank->filter_left[i] = FLOAT_ONE - val;
      bank->bank_right[i] = id2;
      bank->filter_right[i] = val;
   }
   
   #if (0)   
   for (i=0;i<bank->nb_banks;i++)
      bank->scaling[i] = 0;
   for (i=0;i<bank->len;i++) {
      int id = bank->bank_left[i];
      bank->scaling[id] += bank->filter_left[i];
      id = bank->bank_right[i];
      bank->scaling[id] += bank->filter_right[i];
   }
   for (i=0;i<bank->nb_banks;i++)
      bank->scaling[i] = FLOAT_ONE/(bank->scaling[i]);
   #endif
   
   return bank;
}

inline void filterbank_free(FilterBank *bank)
{
   mem_free(bank->bank_left);
   mem_free(bank->bank_right);
   mem_free(bank->filter_left);
   mem_free(bank->filter_right);
   //mem_free(bank->scaling);
   mem_free(bank);
}

inline void filterbank_linear2sb(FilterBank *bank, float *ps, float *mel)
{
   int i;
   for (i=0;i<bank->nb_banks;i++)
      mel[i] = 0;

   for (i=0;i<bank->len;i++) {
      int id;
      id = bank->bank_left[i];
      mel[id] += (bank->filter_left[i] * ps[i]);
      id = bank->bank_right[i];
      mel[id] += (bank->filter_right[i] * ps[i]);
   }
   
   /*for (i=0;i<bank->nb_banks;i++)
      mel[i] = (bank->scaling[i]) * mel[i];
   */

}

inline void filterbank_sb2linear(FilterBank *bank, float *mel, float *ps)
{
   int i;
   for (i=0;i<bank->len;i++) {
      float tmp;
      int id1, id2;
      id1 = bank->bank_left[i];
      id2 = bank->bank_right[i];
      tmp =  mel[id1] * bank->filter_left[i];
      tmp += (mel[id2] * bank->filter_right[i]);
      ps[i] = tmp;
   }
}

inline void aes_conj_window(float *w, int len)  /* Hanning */
{
   int i;
   for (i=0;i<len;i++) {
      float tmp;    
      float x = (float)(4.f * i) / (float)len;
      int inv = 0;
      if (x<1.f) {
      } else if (x<2.f) {
         x = 2.f-x;
         inv = 1;
      } else if (x<3.f) {
         x = x-2.f;
         inv = 1;
      } else {
         x = 4.f-x;
      }
      x = 1.271903f * x;
      tmp = .5f-.5f * (float)math_cos_norm(x);  /* Hanning */
      tmp = tmp * tmp;
      if (inv)
         tmp = FLOAT_ONE - tmp;
      w[i] = (float)math_sqrt(tmp);

   }
}

inline float aes_hypergeom_gain(float vk)
{
   int ind;
   float integer, frac;
   float x;
   static const float table[21] = {
   0.82157f, 1.02017f, 1.20461f, 1.37534f, 1.53363f, 1.68092f, 1.81865f,
   1.94811f, 2.07038f, 2.18638f, 2.29688f, 2.40255f, 2.50391f, 2.60144f,
   2.69551f, 2.78647f, 2.87458f, 2.96015f, 3.04333f, 3.12431f, 3.20326f};

   x = AES_EXPIN_SCALING*vk;
   integer = (float)floor(2*x);
   ind = (int)integer;
   if (ind<0)
      return AES_FRAC_SCALING;
   if (ind>19)
      return (float)(AES_FRAC_SCALING*(1.f+.1296/x));
   frac = 2*x-integer;
   return (float)(AES_FRAC_SCALING*((1.f-frac)*table[ind] + frac*table[ind+1])/sqrt(x+.0001f));
   
}

inline float aes_qcurve(float x)
{
   return 1.f/(1.f+.15f/(AES_SNR_SCALING*x));
}

inline void aes_gain_floor(float effective_echo_suppress, float *echo, float *gain_floor, int len)
{
   int i;
   float echo_floor;

   echo_floor = (float)exp(.2302585f * effective_echo_suppress);

   /* Compute the gain floor for the residual echo */
   for (i=0;i<len;i++)
      gain_floor[i] = AES_FRAC_SCALING * (float)sqrt(echo_floor * echo[i]) / (float)sqrt(1.f + echo[i]);
}

inline void aes_analysis(NonLinearEchoState *st, short *x)
{
   int i;
   int N = st->ps_size;
   int N3 = 2*N - st->frame_size;
   float *ps = st->ps;

   /* Build input buffer for windowing */      
   for (i=0;i<st->frame_size;i++) {
      st->frame[i] = st->inbuf[i];  /* take data in previous frame */
      st->frame[N3+i] = x[i];  /* take data in current frame */
      st->inbuf[i] = x[i];  /* update data in previous frame */
   }

   /* Windowing */
   for (i=0;i<2*N;i++)
      st->frame[i] = st->frame[i] * st->window[i];
   
   /* Perform FFT */
   speech_fft(st->fft_lookup, st->frame, st->ft);

   /* Power spectrum */
   ps[0] = st->ft[0] * st->ft[0];  /* re^2 */
   for (i=1;i<N;i++)  /* 0 to N-1 is enough for filterbank computation */
      ps[i] = st->ft[2*i-1] * st->ft[2*i-1] + st->ft[2*i] * st->ft[2*i];  /* re^2 + im^2 */

   filterbank_linear2sb(st->bank, ps, ps+N);
   
}


