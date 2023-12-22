/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/sample_all.c
 * Description:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <cvi_unf_cipher.h>
#include <cvi_sample_all.h>

CVI_SIZE_T CVI_Test_Alloc(CVI_U32 len)
{
	void *p = malloc(len);

	return (CVI_SIZE_T)p;
}

CVI_S32 CVI_Test_PrintBuffer(const CVI_CHAR *string, const CVI_U8 *pu8Input, CVI_U32 u32Length)
{
	CVI_U32 i = 0;

	if (string) {
		printf("%s\n", string);
	}

	for (i = 0; i < u32Length; i++) {
		if ((i % 16 == 0) && (i != 0))
			printf("\n");
		printf("0x%02x ", pu8Input[i]);
	}
	printf("\n");

	return CVI_SUCCESS;
}

// clang-format off
#define ADD_TEST(name) {#name, name}
struct _T {
	const char *name;
	int (*func)(void);
} tests[] = {
	{"rng", sample_rng},
	{"hash", sample_hash},
	{"cipher", sample_cipher},
	{"mutilcipher", sample_mutiltcipher},
	{"rsa_enc", sample_rsa_enc},
	{"rsa_sign", sample_rsa_sign},
	{"efuse", sample_efuse},
};
// clang-format on

int main(int argc, char *argv[])
{
	CVI_BOOL all = 0;
	const char *test_name = "";
	size_t i;
	int r;

	if (argc <= 1)
		all = 1;
	else
		test_name = argv[1];

	for (i = 0; i < ARRAY_SIZE(tests); i++) {
		if (all || strcmp(test_name, tests[i].name) == 0) {
			printf("[CIPHER] Run test `%s`\n", tests[i].name);
			r = tests[i].func();
			if (r == CVI_SUCCESS) {
				printf("[CIPHER] test `%s` pass\n", tests[i].name);
			} else {
				printf("[CIPHER] test `%s` fail\n", tests[i].name);
				return 1;
			}
		}
	}

	return 0;
}
