/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   */
/*%                                                                 */
/*%       mmse.c                                                    */
/*%       Author: Sharon Lee                                        */
/*%       History:                                                  */
/*%       Created by Sharon Lee in August, 2019                     */
/*%                                                                 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   */

/* Include files */
#include <math.h>
#include <string.h>
#include "mmse_init.h"
#include "mmse.h"
#include "bessel.h"
#include "power.h"
#include "sum.h"
#include "log.h"
#include "dr_detection.h"
#include "slope_detection.h"
#include "abs.h"
#include "mmse_rtwutil.h"
#include "packfft.h"
#include "define.h"
#include "tmwtypes.h"
#include "struct.h"

//extern *fp_test;
//extern frame_cnt;
float gammak[321];
float ksi[321];
float tmp_fft_out[642];
float tmp_ifft_out[642];
float nr_sig[321];
creal32_T nr_fd_half[321];

#ifndef SSP_CALLOC
#define SSP_CALLOC(TYPE, COUNT) ((TYPE *)calloc(COUNT, sizeof(TYPE)))
#endif

#ifndef SSP_SAFE_FREE_BUF
#define SSP_SAFE_FREE_BUF(OBJ) {if (NULL != OBJ) {free(OBJ); OBJ = NULL; } }
#endif

#ifndef SSP_CHECK_NULL_PTR
#define SSP_CHECK_NULL_PTR(ptr)                                                                                            \
	do {                                                                                                           \
		if (!(ptr)) {                                                                                     \
			printf("func:%s,line:%d, NULL pointer\n", __func__, __LINE__);                                 \
			return;                                                                            \
		}                                                                                                      \
	} while (0)
#endif

/* Function Definitions */
EXPORT void NR(short *x, float initial_noise_pow_time, float *frame, NRState *st, boolean_T *speech_vad, float *y)
{
  int i, j, k;
  int len, len1, nFFT;
  float c;
  float qkr;
#ifdef SSP_STACK_TO_HEAP
  //float gammak[321];
  //float ksi[321];
  //float tmp_ifft_out[642];
  //float nr_sig[321];
  float *gammak = SSP_CALLOC(float, 321);
  float *ksi = SSP_CALLOC(float, 321);
  float *tmp_fft_out = SSP_CALLOC(float, 642);
  float *tmp_ifft_out = SSP_CALLOC(float, 642);
  float *nr_sig = SSP_CALLOC(float, 321);
  creal32_T *nr_fd_half = SSP_CALLOC(creal32_T, 321);
  SSP_CHECK_NULL_PTR(gammak);
  SSP_CHECK_NULL_PTR(ksi);
  SSP_CHECK_NULL_PTR(tmp_fft_out);
  SSP_CHECK_NULL_PTR(tmp_ifft_out);
  SSP_CHECK_NULL_PTR(nr_sig);
  SSP_CHECK_NULL_PTR(nr_fd_half);
#else


#endif
  float xtmp_re;
  float xtmp_im;
  float lambda;
  float absar;
  boolean_T vad;
  float b_c[3];
	/* fft_out[640] bin box order: [1].re, [2].re,[2].im, ... ,[320].re,[320].im,[321].re */
  	/* ifft_out[640] bin box order: [1].re, [2].re,[2].im, ... ,[320].re,[320].im,[321].re */
  //creal32_T nr_fd_half[321];

  float tmp, tmp1, alpha, alpha2;


  /* Preparation */
  c = 0.886226925F;
  qkr = 2.33333333F;
  len = 2*st->frame_size;    /* window size */
  len1 = st->frame_size;    /* overlap size */
  nFFT = 2*2*st->frame_size;
  for (i = 0; i < st->frame_size; i++) {
    x[i+st->frame_size] = x[i];    /* go first to avoid overwriting by x_old[], applicable for AEC on or AEC off */
    x[i] = st->x_old[i];
    st->x_old[i] = x[i+st->frame_size];    /* update input overlap data */
  }
  /* Transform to floating-point with float precision for input signals, input sample have to be 16-bit length */
  for (i = 0; i < 2*st->frame_size; i++) {
    st->float_x[i] = (float)(x[i])/(float)(32768.0F);
  }

  /* Apply analysis window */
  for (i = 0; i < (nFFT/2); i++) {
    tmp_ifft_out[i] = st->ana_win[i] * st->float_x[i];
    tmp_ifft_out[(nFFT/2) + i] = 0.0F;
  }

  /* Take fourier transform of one frame */
  speech_fft(st->nr_fft_table, tmp_ifft_out, tmp_fft_out);    /* faster than matlab fft() */
  nr_fd_half[0].re = tmp_fft_out[0] * nFFT;    /* multiply nFFT to get values as in matlab */
  nr_fd_half[0].im = (float)3.3495873e-008;    /* assign a small value to avoid dividing by zero */
  for (k = 1, j = 1; j < (nFFT/2); k+=2, j++) {
    nr_fd_half[j].re = tmp_fft_out[k] * nFFT;
    nr_fd_half[j].im = tmp_fft_out[k+1] * nFFT;
  }
  nr_fd_half[nFFT/2].re = tmp_fft_out[nFFT-1] * nFFT;
  nr_fd_half[nFFT/2].im = (float)3.3495873e-008;    /* assign a small value to avoid dividing by zero */

  a_abs(nr_fd_half, nr_sig, (nFFT/2+1));    /* amplitude */
  c_power(nr_sig, tmp_ifft_out, (nFFT/2+1));    /* power */

  /* Solve the issue of silence segment appeared in the beginning */
 //tmp1 = c_sum(nr_sig, (nFFT/2+1));
 tmp1 = c_sum(nr_sig, (nFFT/2));
  if (tmp1/(nFFT/2.0F) < 0.0003F) {    /* small signal segment */
    *frame = 1.0F;    /* reset initial noise power estimate */
    st->vad_slope_cnt = 0.0F;
    st->vad_slope = 0.0F;
    st->vad_dr_cnt = 0.0F;
    st->vad_dr = 0.0F;
    st->vad_mmse = 0.0F;
    st->vad_mmse_cnt = 0.0F;
    xtmp_im = nFFT / 2.0F;
    for (i = 0; i < (int)(xtmp_im + 1.0F); i++) {
      st->noise_mean[i] = 0.0F;
      st->noise_mu2[i] = 0.0F;
      st->Xk_prev[i] = 0.0F;
    }
    for (i = 0; i < 5; i++) {
      st->noisy_st_trough[i] = 0.0F;
      st->noisy_st_peak[i] = 0.0F;
      for (j = 0; j < 3; j++) {
        st->amp_inst_band[i+5*j] = 0.0F;
      }
    }
    for (i = 0; i < 3; i++) {
      st->amp_inst_sum_hist[i] = 0.0F;
    }
  }

  if (*frame <= initial_noise_pow_time) {    /* amplitude of accumulating the first handful of frames */
    for (i = 0; i < 321; i++) {
      st->noise_mean[i] += nr_sig[i];
      tmp_fft_out[i] = st->noise_mean[i] / (*frame);
    }
    c_power(tmp_fft_out, st->noise_mu2, 321);	    /* average power for one frame */
    /*for (i = 0; i < 321; i++)
	fprintf(fp_test, "%f \n", st->noise_mu2[i]);*/
  }

  for (i = 0; i < (nFFT/2+1); i++) {
    if (st->noise_mu2[i] == 0.0F) {    /* zero denominator */
      if (tmp_ifft_out[i] == 0.0F) {
        gammak[i] = 0.0F;
      } else {
        gammak[i] = 40.0F;
      }
    } else {
      absar = tmp_ifft_out[i] / st->noise_mu2[i];
      if (!(absar < 40.0F)) {
        absar = 40.0F;
      }
      gammak[i] = absar;    /* posteriori SNR */
    }
  }

  if (*frame == 1.0F) {    /* priori SNR for the first frame */
    for (k = 0; k < 321; k++) {
      xtmp_im = gammak[k] - 1.0F;
      xtmp_im = MAX(xtmp_im, 0.0F);
      ksi[k] = st->aa + (1.0F - st->aa) * xtmp_im;
    }
  } else {    /* decision-directed estimate for priori SNR */
    for (k = 0; k < 321; k++) {
      xtmp_im = gammak[k] - 1.0F;
      xtmp_im = MAX(xtmp_im, 0.0F);
      if (0.0F == st->noise_mu2[k])
        st->noise_mu2[k] = (float)3.3495873e-008;    /* assign a small value to avoid dividing by zero */
      ksi[k] = st->aa * st->Xk_prev[k] / st->noise_mu2[k] + (1.0F - st->aa) * xtmp_im;
      ksi[k] = MAX(ksi[k], st->ksi_min);    /* limit ksi greater than -25dB in power */
    }
  }

  /* slope detection */
  for (i = 0; i < 5; i++) {
    st->amp_inst_band[10 + i] = st->amp_inst_band[5 + i];
    st->amp_inst_band[5 + i] = st->amp_inst_band[i];
  }

  st->amp_inst_band[0] = c_sum(*(float (*)[20])&nr_sig[4], 20);               /* 20 */
  st->amp_inst_band[1] = c_sum(*(float (*)[20])&nr_sig[24], 20);             /* 20 */
  st->amp_inst_band[2] = c_sum(*(float (*)[40])&nr_sig[44], 40);             /* 40 */
  st->amp_inst_band[3] = c_sum(*(float (*)[80])&nr_sig[84], 80);             /* 80 */
  st->amp_inst_band[4] = c_sum(*(float (*)[156])&nr_sig[164], 156);      /* 156 */
  /*for (i = 0; i < 5; i++)
 	fprintf(fp_test, "%f \n", st->amp_inst_band[i]);*/

  slope_detection(st);

  /* dynamic range detection */
  dr_detection(st);

  if ((st->vad_slope != 0.0F) || (st->vad_dr != 0.0F)) {
    vad = true;
  } else {
    vad = false;
  }

  /* log_sigma_k calculation for mmse vad */
  for (i = 0; i < 321; i++) {
    tmp_fft_out[i] = 1.0F + ksi[i];
  }
  b_log(tmp_fft_out);	/* C = log(1+ksi) */

  for (k = 0; k < 321; k++) {
    if (0.0F == (1.0F + ksi[k]))
      tmp_fft_out[k] = gammak[k] * ksi[k] / ((float)3.3495873e-008) - tmp_fft_out[k];    /* assign a small value to avoid dividing by zero */
    else
      tmp_fft_out[k] = gammak[k] * ksi[k] / (1.0F + ksi[k]) - tmp_fft_out[k];    /* log_sigma_k */
  }

  xtmp_im = c_sum(tmp_fft_out, nFFT/2);    /* here only consider log_sigma_k(1:nFFT/2) */
  xtmp_im = xtmp_im/(nFFT/2.0F);

  if (xtmp_im <= st->eta) {
    st->vad_mmse_cnt = MAX(0.0F, st->vad_mmse_cnt - 1.0F);
    if (st->vad_mmse_cnt == 0.0F) {
      st->vad_mmse = 0.0F;
    }
  } else {
    st->vad_mmse = 1.0F;
    st->vad_mmse_cnt = st->vad_mmse_cnt_para;
  }

  /* noise power estimate correction when it is too high */
  /* small speech in quiet after big statioanry noise, correct wrong */
  st->amp_inst_sum_hist[2] = st->amp_inst_sum_hist[1];
  st->amp_inst_sum_hist[1] = st->amp_inst_sum_hist[0];
  st->amp_inst_sum_hist[0] = c_sum(st->amp_inst_band, 5);
  //fprintf(fp_test, "%f \n", st->amp_inst_sum_hist[0]);

  c_power(st->amp_inst_sum_hist, b_c, 3);    /* power based */
  /*for (k = 0; k < 3; k++)
  	fprintf(fp_test, "%f \n", b_c[k]);*/

  for (k = 0; k < 3; k++) {
    b_c[k] *= 0.3333F;    /* average power */
  }

  xtmp_im = b_c[0];
  for (k = 0; k < 2; k++) {
    xtmp_im += b_c[k + 1];
  }

  absar = st->noise_mu2[4];
  for (k = 0; k < 315; k++) {
    absar += st->noise_mu2[k + 5];
  }

  if (absar / 8.0F > xtmp_im) {	/* +/-18dB for statioanry noise variance for the whole signals, assumption */
    for (k = 0; k < 321; k++) {
      st->noise_mu2[k] = 0.8F * st->noise_mu2[k] + 0.2F * tmp_ifft_out[k];    /* here faster to update noise power */
    }
  }

  /* %% keep better speech quality, not too easy to update noise power %% */
  *speech_vad = true;
  if ((st->vad_mmse == 0.0F) || (!vad)) {
    for (k = 0; k < 321; k++) {
      st->noise_mu2[k] = st->mu * st->noise_mu2[k] + (1.0F - st->mu) * tmp_ifft_out[k];    /* update noise power */
    }
    *speech_vad = false;
  }

  /*  === end of vad ===  */


  /* v(k) calculation */
  for (k = 0; k < 321; k++) {
    if (0.0F == (1.0F + ksi[k]))
      xtmp_im = ksi[k] * gammak[k] / ((float)3.3495873e-008);    /* assign a small value to avoid dividing by zero */
    else
      xtmp_im = ksi[k] * gammak[k] / (1.0F + ksi[k]);    /* v(k) */
    tmp_ifft_out[k] = xtmp_im;
    tmp_fft_out[321+k] = xtmp_im / 2.0F;
    tmp_fft_out[k] = (float)exp(-0.5F * xtmp_im);    /* C = exp(-0.5*vk) */
  }

  d_power(tmp_ifft_out, &tmp_ifft_out[321]);    /* vk.^0.5 */
  for (i = 0; i < 321; i++) {
      //if ((tmp_ifft_out[321+i] == 0.0F) && (gammak[i] == 0.0F)) {    /* bug fixed to avoid zero denominator */
      if ((gammak[i] == 0.0F)) {
        tmp_ifft_out[321+i] = 0.0F;
      } else {
        tmp_ifft_out[321+i] = (c * tmp_ifft_out[321+i] * tmp_fft_out[i]) / gammak[i];
      }
  }

  bessel_function(0.0F, &tmp_fft_out[321], tmp_fft_out);    /* zero order modified Bessel function */
  bessel_function(1.0F, &tmp_fft_out[321], &tmp_fft_out[321]);    /* first order modified Bessel function */

  /* G(ksi, gammak) estimation */
  for (k = 0; k < 321; k++) {
    tmp = tmp_ifft_out[321+k];
    xtmp_re = tmp_fft_out[k] * (1.0F + tmp_ifft_out[k]);
    tmp1 = tmp_fft_out[321+k] * tmp_ifft_out[k];
    if (0.0F == (1.0F + ksi[k]))
      lambda = qkr * (float)exp(tmp_ifft_out[k]) / ((float)3.3495873e-008);    /* assign a small value to avoid dividing by zero */
    else
      lambda = qkr * (float)exp(tmp_ifft_out[k]) / (1.0F + ksi[k]);
    xtmp_re = nr_sig[k] * (tmp * (xtmp_re + tmp1));
    if (0.0F == (1.0F + lambda))
      tmp = lambda / ((float)3.3495873e-008) * xtmp_re;    /* assign a small value to avoid dividing by zero */
    else
      tmp = lambda / (1.0F + lambda) * xtmp_re;    /* estimated amplitude of clean speech */

    //Xk_prev[k].re = c_sig[k].re * c_sig[k].re - c_sig[k].im * c_sig[k].im;    /* use for updating ksi in next frame */
    //Xk_prev[k].im = c_sig[k].re * c_sig[k].im + c_sig[k].im * c_sig[k].re;    /* use for updating ksi in next frame */
    st->Xk_prev[k] = tmp * tmp;    /* use for updating ksi in next frame */
    tmp_ifft_out[321+k] = tmp;
  }

  for (k = 0; k < (nFFT/2+1); k++) {
    nr_sig[k] = (float)atan2(nr_fd_half[k].im, nr_fd_half[k].re);    /* theta */
  }

  for (k = 0; k < (nFFT/2+1); k++) {
    if (nr_sig[k] == 0.0F) {
      xtmp_re = 1.0F;
      xtmp_im = 0.0F;
    } else {
      xtmp_re = (float)cos(nr_sig[k]);    /* Euler's formula */
      xtmp_im = (float)sin(nr_sig[k]);    /* Euler's formula */
    }
    nr_fd_half[k].re = xtmp_re;    /* phase part */
    nr_fd_half[k].im = xtmp_im;    /* phase part */
  }

  for (k = 0; k < 321; k++) {
    //tmp1 = c_sig[k].re;
    //c_sig[k].re = c_sig[k].re * nr_fd_half[k].re - c_sig[k].im * nr_fd_half[k].im;    /* estimated clean speech signal */
    //c_sig[k].im = tmp1 * nr_fd_half[k].im + c_sig[k].im * nr_fd_half[k].re;    /* estimated clean speech signal */
    tmp = tmp_ifft_out[321+k];
    nr_fd_half[k].re = tmp * nr_fd_half[k].re;    /* estimated clean speech signal */
    nr_fd_half[k].im = tmp * nr_fd_half[k].im;    /* estimated clean speech signal */
  }

  tmp_fft_out[0] = nr_fd_half[0].re;
  for (k = 1, j = 1; j < (nFFT/2); k+=2, j++) {
    tmp_fft_out[k] = nr_fd_half[j].re;
    tmp_fft_out[k+1] = nr_fd_half[j].im;
  }
  tmp_fft_out[nFFT-1] = nr_fd_half[nFFT/2].re;

  /* restore nFFT points before doing inverse FFT, do conjugate symmetric for the lower half points inside speech_ifft() */
  speech_ifft(st->nr_fft_table, tmp_fft_out, tmp_ifft_out);    /* faster than matlab ifft(), in/out not use the same buffer */
  for (k = 0; k < len; k++) {    /* ignore output samples from (len+1) to (2*len) */
    tmp_ifft_out[k] /= nFFT;    /* divide by nFFT to get values as in matlab */
  }

  for (k = 0; k < len1; k++) {
    y[k] = st->y_old[k] + (tmp_ifft_out[k] * st->syn_win[k]);    /* apply synthesis window and 50% OLA for output */
    st->y_old[k] = tmp_ifft_out[len1 + k] * st->syn_win[len1 + k];    /* apply synthesis window and update output OLA buffer */
  }

  if (1 == (unsigned short)*frame)
    st->nr_frame1_last_sample = y[st->frame_size-1];
  if (2 == (unsigned short)*frame) {
    alpha2 = 1.0F/160.0F;
    alpha = 1 - alpha2;
    y[0] = alpha * st->nr_frame1_last_sample + alpha2 * y[0];
    for (i = 0; i < st->frame_size-1; i++) {
      alpha2 = (float)(1.0F + i)/160.0F;
      alpha = 1 - alpha2;
      y[i+1] = alpha * y[i] + alpha2 * y[i+1];
    }
  }
#ifdef SSP_STACK_TO_HEAP
  SSP_SAFE_FREE_BUF(gammak);
  SSP_SAFE_FREE_BUF(ksi);
  SSP_SAFE_FREE_BUF(tmp_fft_out);
  SSP_SAFE_FREE_BUF(tmp_ifft_out);
  SSP_SAFE_FREE_BUF(nr_sig);
  SSP_SAFE_FREE_BUF(nr_fd_half);
#endif
}


