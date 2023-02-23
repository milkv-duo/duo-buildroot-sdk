/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                                             */
/*%       agc.c                                                 */
/*%       Author: Sharon Lee                                    */
/*%       History:                                              */
/*%       Created by Sharon Lee in October, 2019                */
/*%                                                             */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/* Include files */
#include "tmwtypes.h"
#include "struct.h"
#include "define.h"
#include "db2linear.h"
#include "agc_subfun.h"
#include "agc_init.h"
#include "agc.h"

/* Function Definitions */
void AGC(short *sin, short *sout, agc_struct *agc_obj, ssp_para_struct *para_obj, boolean_T speech_vad, boolean_T st_vad, int nenr_silence_time, int nenr_silence_cnt, short path)
{
  short i;
  short unit_gain;
  short frame_signal_identify;
  short tmp16;
  short sig_in;
  short sig_out;
  short sig_err;
  int temp32;
  int diff;

  if (1 == path) {
  	if (para_obj->para_agc_vad_ena == true) {
  		if (speech_vad == false) {
			agc_obj->agc_vad1_cnt = MAX(0, agc_obj->agc_vad1_cnt - 1);
			if (!agc_obj->agc_vad1_cnt)
      				agc_obj->agc_vad1 = false;
  		} else {
			agc_obj->agc_vad1 = true;
			if (agc_obj->Fs == 16000)
  				agc_obj->agc_vad1_cnt = para_obj->para_agc_vad_cnt * 2;    /* keep it longer than speech vad in order to have better speech fidelity at the end of speech segment */
  			else
  				agc_obj->agc_vad1_cnt = para_obj->para_agc_vad_cnt;           /* keep it longer than speech vad in order to have better speech fidelity at the end of speech segment */
  		}

		#if (1)
		if (st_vad == false) {
			agc_obj->agc_vad2 = true;
			agc_obj->agc_vad2_cnt = 3;
  		} else {
			agc_obj->agc_vad2_cnt = MAX(0, agc_obj->agc_vad2_cnt - 1);
			if (!agc_obj->agc_vad2_cnt)
      				agc_obj->agc_vad2 = false;
  		}
		#endif
  	}
  }

  if (nenr_silence_cnt < nenr_silence_time) {    /* be in silence to solve CODEC initial random signal issue and not to perform AGC as well */
	agc_obj->agc_vad1 = false;
	agc_obj->agc_vad1_cnt = 0;
  }

  agc_obj->env = envelope(sin, agc_obj->frame_size);	/* calculate once every 20ms@NB or 10ms@WB */

  unit_gain = (1L << (agc_obj->max_gain));

  frame_signal_identify = 0;
  if (agc_obj->env > agc_obj->pcb_noise_thrd)
  	frame_signal_identify = 1;    /* no matter it is noise or speech signals */

  for (i = 0; i < agc_obj->frame_size; i++) {

  	sig_in = sin[i];

  	if (agc_obj->unit_gain_change_det == true) {
		if (ABS(agc_obj->env) < (short)(0.0316*32768)) {
			agc_obj->max_gain++;    /* drop 6dB to max gain */
			unit_gain = (1L << (agc_obj->max_gain));
			agc_obj->gain32 = unit_gain << 16;
			agc_obj->target_high = (agc_obj->target_high_Q15*unit_gain) >> 15;
			agc_obj->target_low = (agc_obj->target_low_Q15*unit_gain) >> 15;
			agc_obj->unit_gain = agc_obj->max_gain;
			agc_obj->unit_gain_change_det = false;
			agc_obj->smooth_cnt = 10;
		}
 	}

	if (1 == path) {
		if (para_obj->para_agc_vad_ena == true) {
			/* speech-activated AGC */
			if ((agc_obj->agc_vad1 == true) && (agc_obj->agc_vad2 == true)) {
  				agc_obj->target_high = (agc_obj->target_high_Q15*unit_gain) >> 15;
				agc_obj->target_low = (agc_obj->target_low_Q15*unit_gain) >> 15;
				agc_obj->agc_vad3 = true;
			} else {
				agc_obj->target_high = (agc_obj->env*unit_gain) >> 15;    /* better for keeping speech fidelity */
				agc_obj->target_low = (agc_obj->env*unit_gain) >> 15;     /* better for keeping speech fidelity */
				agc_obj->agc_vad3 = false;
			}
		}
	}


	tmp16 = agc_obj->gain32 >> 16;    /* leave space for gain>1, gain has Q format, unit_gain is 1 */
	sig_err = ((int)agc_obj->env)*((int)tmp16) >> 15;    /* use the MSB */

	if ( agc_obj->pcb_noise_cnt < agc_obj->pcb_noise_hold_time) {
		diff = 0;
		tmp16 = (agc_obj->env*unit_gain) >> 15;

		/* release */
		if (tmp16 < agc_obj->target_low)
			diff = (agc_obj->target_low) - sig_err;

		/* attack */
		if (tmp16 > agc_obj->target_high)
			diff = (agc_obj->target_high) - sig_err;

		/* as the original, gain is about one(unit_gain) */
		if (tmp16 >= agc_obj->target_low && tmp16 <= agc_obj->target_high)
			diff = tmp16 - sig_err;

		update_gain(diff, agc_obj);	   /* update agc_obj->gain32 sample by sample */
	}

	temp32 = ((int)(agc_obj->gain32 >> 16)* (int)(sig_in)) >> agc_obj->unit_gain;
	while (ABS(temp32) > 32767L) {    /* overflow limiter */
		agc_obj->gain32 = (((int)(agc_obj->gain32 >> 16) * MINUS_halfdB) << 1);	    /* MINUS_halfdB is Q1.15, so gain32 is Q1.31 */
		temp32 = ((int)(agc_obj->gain32 >> 16)*(int)(sig_in)) >> agc_obj->unit_gain;
	}
	sig_out = sin[i]*(agc_obj->gain32 >> 16) >> agc_obj->unit_gain;

	#if (1)    /* smooth output to avoid pop noise when unit gain changed */
	if (agc_obj->smooth_cnt) {
		sig_out = ((agc_obj->alpha * agc_obj->sout_hist) >> 15) + ((agc_obj->alpha2 * sig_out) >> 15);
		agc_obj->alpha -= 3276;    /* each step 1/10 */
		agc_obj ->alpha2 += 3276;    /* each step 1/10 */
		agc_obj->smooth_cnt--;
	}
	#endif

  	sout[i] = sig_out;

	/* modify for release processing, automaticly drop 6dB on max gain if big signal come in */
	if (agc_obj->unit_gain_change == true) {
		if (ABS(sig_in) > (short)(0.223*32768)) {
			agc_obj->unit_gain_change = false;
			agc_obj->unit_gain_change_det = true;
		}
	}
	agc_obj->sout_hist = sig_out;    /* update y(n-1) */

  }

  #if (1)
  if (frame_signal_identify == 0) {
  	agc_obj->pcb_noise_cnt++;
  	agc_obj->pcb_noise_cnt = MIN(agc_obj->pcb_noise_hold_time, agc_obj->pcb_noise_cnt);
  }
  else
  	agc_obj->pcb_noise_cnt = 0;

  if ( agc_obj->pcb_noise_cnt == agc_obj->pcb_noise_hold_time) {
	tmp16 = (agc_obj->gain32 >> 16) >> 7;
  	tmp16 = MAX(tmp16, 1);
  	tmp16 = (agc_obj->gain32 >> 16) - tmp16;
  	agc_obj->gain32 = ((long)tmp16) << 16;    /* cut down gain32 */

  	if ((agc_obj->gain32 >> 16) < unit_gain)
  		agc_obj->gain32 = unit_gain << 16;
  }
  #endif

  if (1 == path) {
  	if (para_obj->para_agc_vad_ena == true) {
  		agc_obj->agc_vad_hist = agc_obj->agc_vad3;
  	}
  }

}


