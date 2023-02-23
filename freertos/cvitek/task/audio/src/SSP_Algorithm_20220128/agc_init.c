/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                                             */
/*%       agc_init.c                                            */
/*%       Author: Sharon Lee                                    */
/*%       History:                                              */
/*%       Created by Sharon Lee in October, 2019                */
/*%                                                             */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/* Include files */
#include <string.h>
#include <stdio.h>
#include "agc_init.h"
#include "struct.h"
#include "define.h"
#include "db2linear.h"

/* Function Definitions */
void AGC_init(agc_struct *agc, ssp_para_struct *para_obj, float Fs, int frame_size, short path)
{
  agc_struct *agc_obj;

  agc_obj = agc;
  agc_obj->Fs = (short)Fs;
  agc_obj->frame_size = (short)frame_size;
  agc_obj->env = 0L;

  agc_obj->fixed_step_size = 1 << 10;
  agc_obj->stepsize_release = agc_obj->fixed_step_size;
  agc_obj->stepsize_attack = MAX( agc_obj->fixed_step_size, (1 << 13));

  agc_obj->pcb_noise_thrd = 38;
  if (agc_obj->Fs == 16000)    /* WB */
  	agc_obj->pcb_noise_hold_time = 50*(3+1);    /* unit is frame */
  else    /* NB */
  	agc_obj->pcb_noise_hold_time = 25*(3+1);    /* unit is frame */
  agc_obj->pcb_noise_cnt = 0;

  if (1 == path) {
  	agc_obj->unit_gain_change = para_obj->para_agc_cut6_ena;    /* allow to drop unit gain 6dB and only for one time, controlled here!!! */
  	if (!para_obj->para_agc_max_gain)
  		agc_obj->unit_gain_change = false;
  } else {
  	  agc_obj->unit_gain_change = false;
  }

  agc_obj->unit_gain_change_det = false;
  agc_obj->sout_hist = 0;
  agc_obj->smooth_cnt = 0;
  agc_obj->alpha = (short)((float)(9.0F/10.0F) * 32767);    /* coef for Lagrange Interpolation */
  agc_obj->alpha2 = 32767 - agc_obj->alpha;

  agc_obj->agc_vad1 = false;
  agc_obj->agc_vad1_cnt = 0;
  agc_obj->agc_vad2 = false;
  agc_obj->agc_vad2_cnt = 0;
  agc_obj->agc_vad_hist = false;

}

void AGC_para(agc_struct *agc, ssp_para_struct *para, short path)
{
  long unit_gain;
  agc_struct *agc_obj;
  ssp_para_struct *para_obj;
  static short agc_gain_table[7] = {14, 13, 12, 11, 10, 9, 8};

  /* restrict parameter range */
  para_obj = para;
  agc_obj = agc;

  if (1 == path) {
  	para_obj->para_agc_cut6_ena = 1;
  	para_obj->para_agc_vad_cnt = 7;
  	para_obj->para_agc_max_gain = MIN(MAX(para_obj->para_agc_max_gain, 0), 6);
  	para_obj->para_agc_target_high = MIN(MAX(para_obj->para_agc_target_high, 0), 36);
  	para_obj->para_agc_target_low = MIN(MAX(para_obj->para_agc_target_low, 0), 72);
  	para_obj->para_agc_vad_ena = MIN(MAX(para_obj->para_agc_vad_ena, 0), 1);
  	//para_obj->para_agc_vad_cnt = MIN(MAX(para_obj->para_agc_vad_cnt, 1), 25);
  	//para_obj->para_agc_cut6_ena = MIN(MAX(para_obj->para_agc_cut6_ena, 0), 1);
	agc_obj->max_gain = agc_gain_table[para_obj->para_agc_max_gain];
	agc_obj->target_high = db_to_linear(para_obj->para_agc_target_high);
	agc_obj->target_low = db_to_linear(para_obj->para_agc_target_low);
  } else {
	para_obj->para_spk_agc_cut6_ena = 0;
	para_obj->para_spk_agc_vad_ena = 0;
	para_obj->para_spk_agc_vad_cnt = 7;
	para_obj->para_spk_agc_max_gain = MIN(MAX(para_obj->para_spk_agc_max_gain, 0), 6);
	para_obj->para_spk_agc_target_high = MIN(MAX(para_obj->para_spk_agc_target_high, 0), 36);
	para_obj->para_spk_agc_target_low = MIN(MAX(para_obj->para_spk_agc_target_low, 0), 72);
	agc_obj->max_gain = agc_gain_table[para_obj->para_spk_agc_max_gain];
	agc_obj->target_high = db_to_linear(para_obj->para_spk_agc_target_high);
	agc_obj->target_low = db_to_linear(para_obj->para_spk_agc_target_low);
  }

  unit_gain = (1L << (agc_obj->max_gain));    /* if max is Q3.13,  i.e 2^13=8192 */
  agc_obj->unit_gain = agc_obj->max_gain;
  agc_obj->gain32 = unit_gain << 16;    /* start from unit gain */

  agc_obj->target_high_Q15 = agc_obj->target_high;    /* target high in Q1.15 format */

  agc_obj->target_low = MIN(agc_obj->target_low, agc_obj->target_high);
  agc_obj->target_low_Q15 = agc_obj->target_low;    /* target low in Q1.15 format */

  agc_obj->target_high = (short)((agc_obj->target_high*unit_gain) >> 15);    /* proper Q format according to agc max_gain */
  agc_obj->target_low = (short)((agc_obj->target_low*unit_gain) >> 15);    /* proper Q format according to agc max_gain */

}


