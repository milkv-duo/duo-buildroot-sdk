/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_sample_all.h
 * Description:
 */

#ifndef SAMPLE_ALL_H_GPPRF9BL
#define SAMPLE_ALL_H_GPPRF9BL

#include <unistd.h>
#include <termios.h>

#define CVI_ERR_CIPHER(...)                                                                                            \
	do {                                                                                                           \
		printf("\033[0;1;31m");                                                                                \
		printf(__VA_ARGS__);                                                                                   \
		printf("\033[0m");                                                                                     \
		fflush(NULL);                                                                                          \
		tcdrain(1);                                                                                            \
	} while (0)
#define CVI_INFO_CIPHER(...)                                                                                   \
	do {                                                                                                           \
		printf(__VA_ARGS__);                                                                                   \
		fflush(NULL);                                                                                          \
		tcdrain(1);                                                                                            \
	} while (0)
#define TEST_END_PASS() CVI_INFO_CIPHER("%s PASS\n", __func__)
#define TEST_END_FAIL() CVI_ERR_CIPHER("%s FAIL\n", __func__)
#define TEST_RESULT_PRINT()                                                                                            \
	{                                                                                                              \
		if (ret)                                                                                               \
			TEST_END_FAIL();                                                                               \
		else                                                                                                   \
			TEST_END_PASS();                                                                               \
	}

CVI_S32 CVI_Test_PrintBuffer(const CVI_CHAR *string, const CVI_U8 *pu8Input, CVI_U32 u32Length);
CVI_SIZE_T CVI_Test_Alloc(CVI_U32 len);
CVI_U8 *CVI_Test_Map(CVI_SIZE_T szInputAddr);

int sample_hash(void);
int sample_cipher(void);
int sample_rng(void);
int sample_mutiltcipher(void);
int sample_rsa_enc(void);
int sample_rsa_sign(void);
int sample_efuse(void);

#endif /* end of include guard: SAMPLE_ALL_H_GPPRF9BL */
