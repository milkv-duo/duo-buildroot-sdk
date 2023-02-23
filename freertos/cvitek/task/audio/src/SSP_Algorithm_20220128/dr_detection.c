
/* Include files */
#include "mmse.h"
#include "struct.h"
#include "dr_detection.h"

/* Function Definitions */
void dr_detection(NRState *st)
{
  float DR_bin_cnt;
  int k;
  float alpha;

  /*  DR based VAD is good at low and high SNRs, but not good for tone */
  /*  alike signals and also sensitive to DC setup */
  DR_bin_cnt = 0.0F;
  for (k = 0; k < 5; k++) {
    if (st->amp_inst_band[k] - st->noisy_st_trough[k] > 0.0F) {
      alpha = 0.98F;
    } else {
      alpha = 0.5F;
    }

    st->noisy_st_trough[k] = alpha * st->noisy_st_trough[k] + (1.0F - alpha) * st->amp_inst_band[k];
    if (st->amp_inst_band[k] - st->noisy_st_peak[k] > 0.0F) {
      alpha = 0.9F;
    } else {
      alpha = 0.93F;
    }

    st->noisy_st_peak[k] = alpha * st->noisy_st_peak[k] + (1.0F - alpha) * st->amp_inst_band[k];

    /*  to make sure noisy_st_peak cannot go up too much, now maximal 24 dB range */
    if (st->noisy_st_peak[k] / 16.0F - st->noisy_st_trough[k] > 0.0F) {
      st->noisy_st_peak[k] = st->noisy_st_trough[k] * 16.0F;
    }

    /*  the smaller the threshold, the faster to suppress noise but easily introduce more speech distortion as trade-off */
    if (st->noisy_st_peak[k] * 0.5012F - st->noisy_st_trough[k] > 0.0F) {
      DR_bin_cnt++;
    }
  }

  if (DR_bin_cnt == 0.0F) {
    DR_bin_cnt = st->vad_dr_cnt - 1.0F;
    if (0.0F > DR_bin_cnt) {
      DR_bin_cnt = 0.0F;
    }

    st->vad_dr_cnt = DR_bin_cnt;
    if (DR_bin_cnt == 0.0F) {
      st->vad_dr = 0.0F;
    }
  } else {
    st->vad_dr = 1.0F;
    if (st->Fs == 16000.0F) {
      st->vad_dr_cnt = 7.0F;
    } else {
      if (st->Fs == 8000.0F) {
        st->vad_dr_cnt = 7.0F;
      }
    }
  }

}


