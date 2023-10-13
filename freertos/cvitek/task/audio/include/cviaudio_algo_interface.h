/* Include files */
#ifndef __CVIAUDIO_ALGO_INTERFACE__
#define __CVIAUDIO_ALGO_INTERFACE__
#include <stdio.h>
void *CviAud_Algo_Init(int s32FunctMask, void *param_info);
int CviAud_Algo_Process(void *pHandle,  uint64_t mic_in,
			uint64_t ref_in, uint64_t out, int iLength);
int CviAud_Algo_Fun_Config(void *pHandle, int u32OpenMask);
void CviAud_Algo_DeInit(void *pHandle);
void  CviAud_Algo_GetVersion(char *pstrVersion);
#ifdef NEXT_SSP_ALGO
void CviAud_SpkAlgo_DeInit(void *pHandle);
int CviAud_SpkAlgo_Process(void *pHandle,  short *spk_in,
			short *spk_out, int iLength);
void *CviAud_SpkAlgo_Init(int s32FunctMask, void *param_info);
#endif
#endif
