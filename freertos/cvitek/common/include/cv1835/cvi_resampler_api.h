/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_resampler_api.h
 * Description: basic audio decoder  api for application layer
 */


#ifndef _CVI_RESAMPLER_API_H_
#define _CVI_RESAMPLER_API_H_

#include "cvi_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAXFRAMESIZE 2048

CVI_VOID *CVI_Resampler_Create(CVI_S32 s32Inrate, CVI_S32 s32Outrate,
			       CVI_S32 s32Chans);


/**************************************************************************************
 * Function:    CVI_Resampler_Process
 *
 * Description: Resample pcm data to specific samplerate, only for interlaced format
 *
 * Inputs:      inst: valid Resampler instance pointer (HResampler)
 *              inbuf:   pointer to inputbuf
 *              insamps: input number of sample pointers
 * Outputs:     outbuf:  pointer to outputbuf
 *
 * Return:      output sample number per-channel
 * Notes:       sure insamps < MAXFRAMESIZE


 **************************************************************************************/
CVI_S32 CVI_Resampler_Process(CVI_VOID *inst, CVI_S16 *s16Inbuf,
			      CVI_S32 s32Insamps, CVI_S16 *s16Outbuf);


/**************************************************************************************
 * Function:    CVI_Resampler_Destroy
 *
 * Description: free platform-specific data allocated by ResamplerCreate
 *
 * Inputs:      valid Resampler instance pointer (HResampler)
 * Outputs:     none
 *
 * Return:      none
 **************************************************************************************/
CVI_VOID CVI_Resampler_Destroy(CVI_VOID *inst);


/*******************************************************************************
 * Function:	CVI_Resampler_GetMaxOutputNum
 *
 * Description: Caculate max output number at specific input number
 *
 * Inputs:		inst:	  valid Resampler instance pointer (CVI_HANDLE)
 *				insamps:  input data number per-channel, insamps must be even
 * Outputs: none
 * Return:		>=0:	  Success, return the max output number per-channel
 *				other:	  Fail, return error code
 * Notes:
 * 1  if stereo(chans==2), sure insamps%2 == 0
 ******************************************************************************/
CVI_S32 CVI_Resampler_GetMaxOutputNum(CVI_VOID *inst, CVI_S32 s32Insamps);


#ifdef __cplusplus
}
#endif



#endif
