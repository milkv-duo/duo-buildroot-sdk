/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_sample_rng.c
 * Description:
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <cvi_unf_cipher.h>
#include <cvi_sample_all.h>

#define RNG_TEST_SIZE 16

int sample_rng(void)
{
	CVI_S32 ret = CVI_SUCCESS;
	CVI_U32 n = 0;
	CVI_U32 prev = 0;
	int i;

	ret = CVI_UNF_CIPHER_Init();
	if (ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("CVI_UNF_CIPHER_Init fail (%d).\n", ret);
		return ret;
	}

	for (i = 0; i < RNG_TEST_SIZE; i++) {
		ret = CVI_UNF_CIPHER_GetRandomNumber(&n);
		if (n == prev) {
			CVI_ERR_CIPHER("CVI_UNF_CIPHER_GetRandomNumber fail (%d). Get the same number 0x%08x.\n", ret,
				       n);
			return CVI_FAILURE;
		}
		prev = n;
		printf("Random number: %08x\n", n);
	}

	ret = CVI_UNF_CIPHER_DeInit();
	if (ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("CVI_UNF_CIPHER_DeInit fail (%d).\n", ret);
		return ret;
	}

	return ret;
}
