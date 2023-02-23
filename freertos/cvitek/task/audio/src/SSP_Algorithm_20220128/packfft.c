
 /* Include files */
 #include <math.h>
#include "define.h"
#include "memalloc.h"
#include "fftcore.h"
#include "packfft.h"

/* Function Definitions */
void *speech_fft_init(int size)
{
   struct drft_lookup *table;
   table = mem_alloc(sizeof(struct drft_lookup));
   core_drft_init((struct drft_lookup *)table, size);
   return (void*)table;
}

void speech_fft_free(void *table)
{
   core_drft_clear(table);
   mem_free(table);
}

void speech_fft(void *table, float *in, float *out)
{
   if (in==out) {
      int i;
      float scale = (float)(1./((struct drft_lookup *)table)->n);
      //speech_warning("FFT should not be done in-place");
      for (i=0;i<((struct drft_lookup *)table)->n;i++)
         out[i] = scale*in[i];
   } else {
      int i;
      float scale = (float)(1./((struct drft_lookup *)table)->n);
      for (i=0;i<((struct drft_lookup *)table)->n;i++)
         out[i] = scale*in[i];
   }
   core_drft_forward((struct drft_lookup *)table, out);
}

void speech_ifft(void *table, float *in, float *out)
{
   if (in==out) {
      //speech_warning("FFT should not be done in-place");
   } else {
      int i;
      for (i=0;i<((struct drft_lookup *)table)->n;i++)
         out[i] = in[i];
   }
   core_drft_backward((struct drft_lookup *)table, out);
}


