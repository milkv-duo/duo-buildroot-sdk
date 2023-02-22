/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_sample_efuse.c
 * Description:
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <cvi_unf_cipher.h>
#include <cvi_sample_all.h>
#include <cvi_misc.h>

#define EFUSE_WRITE_TEST 0

#define CHECK_RESULT(ret, ...)                                                                                         \
	do {                                                                                                           \
		if (ret < 0) {                                                                                         \
			CVI_ERR_CIPHER(__VA_ARGS__);                                                                   \
			return CVI_FAILURE;                                                                            \
		}                                                                                                      \
	} while (0)

static CVI_U8 buf[0x100000];

#if EFUSE_WRITE_TEST
static CVI_U8 test_data[] = "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
			    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
			    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"
			    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F";

static CVI_U8 test_loader_ek[16] = { 0xC0, 0x1B, 0xEF, 0x05, 0x6A, 0x8B, 0xEA, 0x9F,
				     0x53, 0xAB, 0x7A, 0xC3, 0x93, 0x87, 0x5B, 0xB8 };

unsigned char test_hash0_public[32] = { 0x54, 0xBC, 0x85, 0x7C, 0x76, 0x6E, 0x64, 0xDC, 0xC0, 0x2C, 0xDA,
					0x6D, 0x4A, 0x98, 0xA8, 0x46, 0x7C, 0xA5, 0x5D, 0x74, 0xF3, 0xB2,
					0x3D, 0x93, 0x37, 0x38, 0xF9, 0x6E, 0x84, 0x8A, 0x53, 0xE6 };
#endif

int sample_efuse(void)
{
	CVI_S32 ret = CVI_SUCCESS;
	CVI_U32 size;
	int i;

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	ret = CVI_EFUSE_IsSecureBootEnabled();
	CHECK_RESULT(ret, "CVI_EFUSE_IsSecureBootEnabled() ret=%d\n", ret);
	CVI_INFO_CIPHER("CVI_EFUSE_IsSecureBootEnabled(): %d\n", ret);

	for (i = 0; i < CVI_EFUSE_LOCK_LAST; i++) {
		ret = CVI_EFUSE_IsLocked(i);
		CHECK_RESULT(ret, "CVI_EFUSE_IsLocked() ret=%d\n", ret);
		CVI_INFO_CIPHER("CVI_EFUSE_IsLocked(): %d locked? = %d\n", i, ret);
	}

	for (i = 0; i < CVI_EFUSE_AREA_LAST; i++) {
		size = 0;
		ret = CVI_EFUSE_GetSize(i, &size);
		CHECK_RESULT(ret, "CVI_EFUSE_GetSize() ret=%d\n", ret);
		CVI_INFO_CIPHER("CVI_EFUSE_GetSize(): area %d is %u bytes\n", i, size);

		ret = CVI_EFUSE_Read(i, buf, size);
		CHECK_RESULT(ret, "CVI_EFUSE_Read() ret=%d\n", ret);
		CVI_Test_PrintBuffer("eFuse", buf, size);
	}

#if EFUSE_WRITE_TEST
	// Secure Boot test
	CVI_INFO_CIPHER("CVI_EFUSE_Write(): Write to SECURITY AREA\n");

	CVI_INFO_CIPHER("CVI_EFUSE_Write(): Write to CVI_EFUSE_AREA_HASH0_PUBLIC\n");
	ret = CVI_EFUSE_Write(CVI_EFUSE_AREA_HASH0_PUBLIC, test_hash0_public, 32);

	CVI_INFO_CIPHER("CVI_EFUSE_Write(): Write to CVI_EFUSE_AREA_LOADER_EK\n");
	ret = CVI_EFUSE_Write(CVI_EFUSE_AREA_LOADER_EK, test_loader_ek, 16);

	CVI_EFUSE_Lock(CVI_EFUSE_LOCK_HASH0_PUBLIC);
	CVI_INFO_CIPHER("CVI_EFUSE_Lock(): ret=%d lock=%d\n", ret, CVI_EFUSE_LOCK_HASH0_PUBLIC);
	CVI_EFUSE_Lock(CVI_EFUSE_LOCK_LOADER_EK);
	CVI_INFO_CIPHER("CVI_EFUSE_Lock(): ret=%d lock=%d\n", ret, CVI_EFUSE_LOCK_LOADER_EK);

	ret = CVI_EFUSE_EnableSecureBoot();
	CVI_INFO_CIPHER("CVI_EFUSE_EnableSecureBoot(): ret=%d\n", ret);
#endif

#if EFUSE_WRITE_TEST
	// User data test
	CVI_INFO_CIPHER("CVI_EFUSE_Write(): Write to USER area\n");

	CVI_INFO_CIPHER("CVI_EFUSE_Write(): Write to CVI_EFUSE_AREA_DEVICE_ID\n");
	ret = CVI_EFUSE_Write(CVI_EFUSE_AREA_DEVICE_ID, test_data, 8);

	CVI_INFO_CIPHER("CVI_EFUSE_Write(): Write to CVI_EFUSE_AREA_DEVICE_EK\n");
	ret = CVI_EFUSE_Write(CVI_EFUSE_AREA_DEVICE_EK, test_data, 16);

	CVI_INFO_CIPHER("CVI_EFUSE_Write(): Write to USER area\n");

	ret = CVI_EFUSE_Write(CVI_EFUSE_AREA_USER, test_data, 3);
	CHECK_RESULT(ret, "CVI_EFUSE_Write() ret=%d\n", ret);

	ret = CVI_EFUSE_Write(CVI_EFUSE_AREA_USER, test_data, 8);
	CHECK_RESULT(ret, "CVI_EFUSE_Write() ret=%d\n", ret);

	ret = CVI_EFUSE_Write(CVI_EFUSE_AREA_USER, test_data, 40);
	CHECK_RESULT(ret, "CVI_EFUSE_Write() ret=%d\n", ret);

	CVI_EFUSE_Lock(CVI_EFUSE_LOCK_DEVICE_EK);
	CVI_INFO_CIPHER("CVI_EFUSE_Lock(): ret=%d lock=%d\n", ret, CVI_EFUSE_LOCK_DEVICE_EK);
#endif

	return CVI_SUCCESS;
}
