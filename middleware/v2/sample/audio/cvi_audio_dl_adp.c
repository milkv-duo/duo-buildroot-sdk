#include "cvi_audio_dl_adp.h"
#include <stdio.h>
#include <dlfcn.h>

#define UNUSED_REF(X)  ((X) = (X))

CVI_S32 CVI_Audio_Dlpath(CVI_CHAR *pChLibPath)
{
	UNUSED_REF(pChLibPath);
	return CVI_SUCCESS;
}

CVI_S32 CVI_Audio_Dlopen(CVI_VOID **pLibhandle, CVI_CHAR *pChLibName)
{
	if (CVI_NULL == pLibhandle || CVI_NULL == pChLibName)
		return CVI_FAILURE;

	*pLibhandle = CVI_NULL;
	*pLibhandle = dlopen(pChLibName, RTLD_LAZY | RTLD_LOCAL);

	if (*pLibhandle == CVI_NULL) {
		printf("dlopen %s failed!\n", pChLibName);
		printf("[error]%s\n", dlerror());
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_Audio_Dlsym(CVI_VOID **pFunchandle, CVI_VOID *Libhandle,
			CVI_CHAR *pChFuncName)
{
	if (Libhandle == CVI_NULL) {
		printf("LibHandle is empty!");
		return CVI_FAILURE;
	}

	*pFunchandle = CVI_NULL;
	*pFunchandle = dlsym(Libhandle, pChFuncName);


	if (*pFunchandle == CVI_NULL) {
		printf("dlsym %s fail,error msg is %s!\n", pChFuncName, dlerror());

		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_Audio_Dlclose(CVI_VOID *Libhandle)
{
	if (Libhandle == CVI_NULL) {
		printf("LibHandle is empty!");
		return CVI_FAILURE;
	}

	dlclose(Libhandle);


	return CVI_SUCCESS;
}
