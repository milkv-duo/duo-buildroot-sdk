#ifndef __CVI_AUDIO_DL_ADP_H__
#define __CVI_AUDIO_DL_ADP_H__

#include "cvi_type.h"

CVI_S32 CVI_Audio_Dlpath(CVI_CHAR *pChLibPath);

CVI_S32 CVI_Audio_Dlopen(CVI_VOID **pLibhandle, CVI_CHAR *pChLibName);

CVI_S32 CVI_Audio_Dlsym(CVI_VOID **pFunchandle, CVI_VOID *Libhandle,
			CVI_CHAR *pChFuncName);

CVI_S32 CVI_Audio_Dlclose(CVI_VOID *Libhandle);

#endif
