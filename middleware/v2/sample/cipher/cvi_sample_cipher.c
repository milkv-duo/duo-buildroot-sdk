/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_sample_cipher.c
 * Description:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <cvi_unf_cipher.h>
#include <cvi_sample_all.h>

static CVI_S32 CVI_Test_SetConfig(CVI_CIPHER_HANDLE chnHandle, CVI_UNF_CIPHER_ALG_E alg,
				  CVI_UNF_CIPHER_WORK_MODE_E mode, CVI_UNF_CIPHER_KEY_LENGTH_E keyLen,
				  const CVI_U8 u8KeyBuf[32], const CVI_U8 u8IVBuf[16])
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_UNF_CIPHER_CTRL_S CipherCtrl;

	memset(&CipherCtrl, 0, sizeof(CVI_UNF_CIPHER_CTRL_S));
	CipherCtrl.enAlg = alg;
	CipherCtrl.enWorkMode = mode;
	CipherCtrl.enBitWidth = CVI_UNF_CIPHER_BIT_WIDTH_128BIT;
	CipherCtrl.enKeyLen = keyLen;
	if (CipherCtrl.enWorkMode != CVI_UNF_CIPHER_WORK_MODE_ECB) {
		CipherCtrl.stChangeFlags.bit1IV = 1;
		memcpy(CipherCtrl.u32IV, u8IVBuf, 16);
	}

	if (keyLen == CVI_UNF_CIPHER_KEY_AES_128BIT) {
		memcpy(CipherCtrl.u32Key, u8KeyBuf, 16);
	} else {
		memcpy(CipherCtrl.u32Key, u8KeyBuf, 32);
	}

	s32Ret = CVI_UNF_CIPHER_ConfigHandle(chnHandle, &CipherCtrl);
	if (s32Ret != CVI_SUCCESS) {
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

static CVI_S32 ECB_AES256(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 32;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;

	CVI_U8 aes_key[32] = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
			       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
			       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
	CVI_U8 aes_src[32] = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
			       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
			       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
	CVI_U8 aes_dst[32] = { 0x86, 0x3a, 0xe9, 0x1f, 0xd4, 0xcf, 0x51, 0x13, 0x2d, 0xc3, 0xe4,
			       0xed, 0xf0, 0x9d, 0xbc, 0x6e, 0x86, 0x3a, 0xe9, 0x1f, 0xd4, 0xcf,
			       0x51, 0x13, 0x2d, 0xc3, 0xe4, 0xed, 0xf0, 0x9d, 0xbc, 0x6e };
	CVI_U8 aes_IV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}

	printf("hTestchnid 0x%zx\n", hTestchnid);

	szInputAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);
	szOutPutAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_ECB,
				    CVI_UNF_CIPHER_KEY_AES_256BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("CBC-AES-128-ORI:", aes_src, sizeof(aes_src));

	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CBC-AES-128-ENC:", pOutputAddr, sizeof(aes_dst));

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("cipher encrypt, memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_ECB,
				    CVI_UNF_CIPHER_KEY_AES_256BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CBC-AES-128-DEC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("cipher decrypt, memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 CBC_AES256(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 16;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;

	CVI_U8 aes_key[32] = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
			       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
			       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
	CVI_U8 aes_IV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
	CVI_U8 aes_src[16] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
			       0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
	CVI_U8 aes_dst[16] = { 0xe0, 0xfa, 0xab, 0x42, 0xd5, 0xa5, 0x5c, 0x18,
			       0xb0, 0x13, 0x1f, 0x47, 0x6c, 0xa5, 0xd0, 0x0a };

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}

	printf("hTestchnid 0x%zx\n", hTestchnid);

	szInputAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);
	szOutPutAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CBC,
				    CVI_UNF_CIPHER_KEY_AES_256BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("CBC-AES-128-ORI:", aes_src, sizeof(aes_src));

	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CBC-AES-128-ENC:", pOutputAddr, sizeof(aes_dst));

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("cipher encrypt, memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CBC,
				    CVI_UNF_CIPHER_KEY_AES_256BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CBC-AES-128-DEC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("cipher decrypt, memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 CBC_AES192(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 16;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;

	CVI_U8 aes_key[32] = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
			       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
			       0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
	CVI_U8 aes_IV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
	CVI_U8 aes_src[16] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
			       0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
	CVI_U8 aes_dst[16] = { 0x69, 0x25, 0x8f, 0xc2, 0x8d, 0x58, 0x8b, 0x84,
			       0x67, 0x87, 0x76, 0x02, 0x2f, 0x9c, 0x1c, 0x91 };

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}

	printf("hTestchnid 0x%zx\n", hTestchnid);

	szInputAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);
	szOutPutAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CBC,
				    CVI_UNF_CIPHER_KEY_AES_192BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("CBC-AES-128-ORI:", aes_src, sizeof(aes_src));

	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CBC-AES-128-ENC:", pOutputAddr, sizeof(aes_dst));

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("cipher encrypt, memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CBC,
				    CVI_UNF_CIPHER_KEY_AES_192BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CBC-AES-128-DEC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("cipher decrypt, memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	TEST_END_PASS();

__CIPHER_EXIT__:

	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 CBC_AES128(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 16;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;

	CVI_U8 aes_key[16] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
			       0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
	CVI_U8 aes_IV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
	CVI_U8 aes_src[16] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
			       0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
	CVI_U8 aes_dst[16] = { 0x76, 0x49, 0xAB, 0xAC, 0x81, 0x19, 0xB2, 0x46,
			       0xCE, 0xE9, 0x8E, 0x9B, 0x12, 0xE9, 0x19, 0x7D };

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}

	printf("hTestchnid 0x%zx\n", hTestchnid);

	szInputAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);
	szOutPutAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CBC,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("CBC-AES-128-ORI:", aes_src, sizeof(aes_src));

	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CBC-AES-128-ENC:", pOutputAddr, sizeof(aes_dst));

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("cipher encrypt, memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CBC,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CBC-AES-128-DEC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("cipher decrypt, memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 CFB_AES128(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 32;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;
	CVI_U8 aes_key[16] = { "\x2B\x7E\x15\x16\x28\xAE\xD2\xA6\xAB\xF7\x15\x88\x09\xCF\x4F\x3C" };
	CVI_U8 aes_IV[16] = { "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" };
	CVI_U8 aes_src[32] = { "\x6B\xC1\xBE\xE2\x2E\x40\x9F\x96\xE9\x3D\x7E\x11\x73\x93\x17\x2A\xAE\x2D\x8A\x57\x1E"
			       "\x03\xAC\x9C\x9E\xB7\x6F\xAC\x45\xAF\x8E\x51" };
	CVI_U8 aes_dst[32] = { "\x3B\x3F\xD9\x2E\xB7\x2D\xAD\x20\x33\x34\x49\xF8\xE8\x3C\xFB\x4A\xC8\xA6\x45\x37\xA0"
			       "\xB3\xA9\x3F\xCD\xE3\xCD\xAD\x9F\x1C\xE5\x8B" };

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}
	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}
	szInputAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);
	szOutPutAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CFB,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("CFB-AES-128-ORI:", aes_src, u32TestDataLen);

	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CFB-AES-128-ENC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CFB,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CFB-AES-128-DEC", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		goto __CIPHER_EXIT__;
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 CFB_AES128_COPY_AVOID(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 32;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;
	CVI_U8 aes_key[16] = { "\x2B\x7E\x15\x16\x28\xAE\xD2\xA6\xAB\xF7\x15\x88\x09\xCF\x4F\x3C" };
	CVI_U8 aes_IV[16] = { "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" };
	CVI_U8 aes_src[32] = { "\x6B\xC1\xBE\xE2\x2E\x40\x9F\x96\xE9\x3D\x7E\x11\x73\x93\x17\x2A\xAE\x2D\x8A\x57\x1E"
			       "\x03\xAC\x9C\x9E\xB7\x6F\xAC\x45\xAF\x8E\x51" };
	CVI_U8 aes_dst[32] = { "\x3B\x3F\xD9\x2E\xB7\x2D\xAD\x20\x33\x34\x49\xF8\xE8\x3C\xFB\x4A\xC8\xA6\x45\x37\xA0"
			       "\xB3\xA9\x3F\xCD\xE3\xCD\xAD\x9F\x1C\xE5\x8B" };

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}
	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_COPY_AVOID;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}
	szInputAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);
	szOutPutAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CFB,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("CFB-AES128-COPY-AVOID-ORI:", aes_src, u32TestDataLen);

	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CFB-AES128-COPY-AVOID-ENC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CFB,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CFB-AES128-COPY-AVOID-DEC", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 CTR_AES128(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 32;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;
	CVI_U8 aes_key[16] = { "\x7E\x24\x06\x78\x17\xFA\xE0\xD7\x43\xD6\xCE\x1F\x32\x53\x91\x63" };
	CVI_U8 aes_IV[16] = { "\x00\x6C\xB6\xDB\xC0\x54\x3B\x59\xDA\x48\xD9\x0B\x00\x00\x00\x01" };
	CVI_U8 aes_src[32] = { "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14"
			       "\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F" };
	CVI_U8 aes_dst[32] = { "\x51\x04\xA1\x06\x16\x8A\x72\xD9\x79\x0D\x41\xEE\x8E\xDA\xD3\x88\xEB\x2E\x1E\xFC\x46"
			       "\xDA\x57\xC8\xFC\xE6\x30\xDF\x91\x41\xBE\x28" };

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}
	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}
	szInputAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);
	szOutPutAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CTR,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("CTR-AES-128-ORI:", aes_src, u32TestDataLen);

	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CTR-AES-128-ENC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CTR,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Set config info failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CTR-AES-128-DEC", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

CVI_S32 CCM_AES128(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 4;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_SIZE_T szAadAddr = 0;
	CVI_U32 u32TagLen;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_U8 *pAadAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_CTRL_EX_S CipherCtrl;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;
	CVI_UNF_CIPHER_CTRL_AES_CCM_GCM_S stCtrl;
	CVI_U8 aes_key[16] = { "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f" };
	CVI_U8 aes_n[7] = { "\x10\x11\x12\x13\x14\x15\x16" };
	CVI_U8 aes_a[8] = { "\x00\x01\x02\x03\x04\x05\x06\x07" };
	CVI_U8 aes_src[4] = { "\x20\x21\x22\x23" };
	CVI_U8 aes_dst[4] = { "\x71\x62\x01\x5b" };
	CVI_U8 aes_tag[4] = { "\x4d\xac\x25\x5d" };
	CVI_U8 out_tag[16];

	printf("\n--------------------------%s-----------------------\n", __func__);

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	memset(out_tag, 0x0, 16);

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}

	szInputAddr = CVI_Test_Alloc(32);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);
	szOutPutAddr = CVI_Test_Alloc(32);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	szAadAddr = CVI_Test_Alloc(sizeof(aes_a));
	if (szAadAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for AAD failed!\n");
		goto __CIPHER_EXIT__;
	}
	pAadAddr = (CVI_U8 *)(szAadAddr);

	memset(&CipherCtrl, 0, sizeof(CVI_UNF_CIPHER_CTRL_EX_S));
	CipherCtrl.enAlg = CVI_UNF_CIPHER_ALG_AES;
	CipherCtrl.enWorkMode = CVI_UNF_CIPHER_WORK_MODE_CCM;
	stCtrl.enKeyLen = CVI_UNF_CIPHER_KEY_AES_128BIT;
	stCtrl.u32IVLen = sizeof(aes_n);
	stCtrl.u32TagLen = sizeof(aes_tag);
	stCtrl.u32ALen = sizeof(aes_a);
	stCtrl.szAdataAddr = szAadAddr;
	memcpy(stCtrl.u32IV, aes_n, sizeof(aes_n));
	memcpy(stCtrl.u32Key, aes_key, 16);
	memcpy(pAadAddr, aes_a, sizeof(aes_a));
	CipherCtrl.pParam = &stCtrl;
	s32Ret = CVI_UNF_CIPHER_ConfigHandleEx(hTestchnid, &CipherCtrl);
	if (s32Ret != CVI_SUCCESS) {
		return CVI_FAILURE;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("CCM-AES-128-ORI:", aes_src, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);
	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CCM-AES-128-ENC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	u32TagLen = 16;
	s32Ret = CVI_UNF_CIPHER_GetTag(hTestchnid, out_tag, &u32TagLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Get tag failed.\n");
		goto __CIPHER_EXIT__;
	}
	CVI_Test_PrintBuffer("CCM-AES-128-TAG", out_tag, sizeof(aes_tag));
	if (memcmp(out_tag, aes_tag, sizeof(aes_tag)) != 0) {
		CVI_ERR_CIPHER("Tag compare failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_ConfigHandleEx(hTestchnid, &CipherCtrl);
	if (s32Ret != CVI_SUCCESS) {
		return CVI_FAILURE;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CCM-AES-128-DEC", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	u32TagLen = 16;
	s32Ret = CVI_UNF_CIPHER_GetTag(hTestchnid, out_tag, &u32TagLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Get tag failed.\n");
		goto __CIPHER_EXIT__;
	}
	CVI_Test_PrintBuffer("CCM-AES-128-TAG", out_tag, sizeof(aes_tag));
	if (memcmp(out_tag, aes_tag, sizeof(aes_tag)) != 0) {
		CVI_ERR_CIPHER("Tag compare failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}
	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 CCM_AES128_2(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 32;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_SIZE_T szAadAddr = 0;
	CVI_U32 u32TagLen, i;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_U8 *pAadAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_CTRL_EX_S CipherCtrl;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;
	CVI_UNF_CIPHER_CTRL_AES_CCM_GCM_S stCtrl;
	CVI_U8 aes_key[16] = { "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f" };
	CVI_U8 aes_n[13] = { "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c" };
	static CVI_U8 aes_a[65536];
	CVI_U8 aes_src[32] = { "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
			       "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f" };
	CVI_U8 aes_dst[32] = { "\x69\x91\x5d\xad\x1e\x84\xc6\x37\x6a\x68\xc2\x96\x7e\x4d\xab\x61"
			       "\x5a\xe0\xfd\x1f\xae\xc4\x4c\xc4\x84\x82\x85\x29\x46\x3c\xcf\x72" };
	CVI_U8 aes_tag[14] = { "\xb4\xac\x6b\xec\x93\xe8\x59\x8e\x7f\x0d\xad\xbc\xea\x5b" };
	CVI_U8 out_tag[16];

	printf("\n--------------------------%s-----------------------\n", __func__);

	for (i = 0; i < 65536; i++) {
		aes_a[i] = i;
	}

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	memset(out_tag, 0x0, 16);

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}
	szInputAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);

	szOutPutAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	szAadAddr = CVI_Test_Alloc(sizeof(aes_a));
	if (szAadAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for AAD failed!\n");
		goto __CIPHER_EXIT__;
	}
	pAadAddr = (CVI_U8 *)(szAadAddr);

	memset(&CipherCtrl, 0, sizeof(CVI_UNF_CIPHER_CTRL_EX_S));
	CipherCtrl.enAlg = CVI_UNF_CIPHER_ALG_AES;
	CipherCtrl.enWorkMode = CVI_UNF_CIPHER_WORK_MODE_CCM;
	stCtrl.enKeyLen = CVI_UNF_CIPHER_KEY_AES_128BIT;
	stCtrl.u32IVLen = sizeof(aes_n);
	stCtrl.u32TagLen = sizeof(aes_tag);
	stCtrl.u32ALen = sizeof(aes_a);
	stCtrl.szAdataAddr = szAadAddr;
	memcpy(stCtrl.u32IV, aes_n, sizeof(aes_n));
	memcpy(stCtrl.u32Key, aes_key, 16);
	memcpy(pAadAddr, aes_a, sizeof(aes_a));
	CipherCtrl.pParam = &stCtrl;
	s32Ret = CVI_UNF_CIPHER_ConfigHandleEx(hTestchnid, &CipherCtrl);
	if (s32Ret != CVI_SUCCESS) {
		return CVI_FAILURE;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("CCM-AES-128-ORI:", aes_src, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);
	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CCM-AES-128-ENC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	u32TagLen = 16;
	s32Ret = CVI_UNF_CIPHER_GetTag(hTestchnid, out_tag, &u32TagLen);

	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Get tag failed.\n");
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CCM-AES-128-ORITAG:", aes_tag, u32TagLen);
	CVI_Test_PrintBuffer("CCM-AES-128-GETTAG", out_tag, sizeof(aes_tag));

	if (memcmp(out_tag, aes_tag, sizeof(aes_tag)) != 0) {
		CVI_ERR_CIPHER("Tag compare failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_ConfigHandleEx(hTestchnid, &CipherCtrl);
	if (s32Ret != CVI_SUCCESS) {
		return CVI_FAILURE;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("CCM-AES-128-DEC", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	u32TagLen = 16;
	s32Ret = CVI_UNF_CIPHER_GetTag(hTestchnid, out_tag, &u32TagLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Get tag failed.\n");
		goto __CIPHER_EXIT__;
	}
	CVI_Test_PrintBuffer("CCM-AES-128-TAG", out_tag, sizeof(aes_tag));
	if (memcmp(out_tag, aes_tag, sizeof(aes_tag)) != 0) {
		CVI_ERR_CIPHER("Tag compare failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}
	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 GCM_AES128(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 60;
	CVI_SIZE_T szInputAddr = 0;
	CVI_SIZE_T szOutPutAddr = 0;
	CVI_SIZE_T szAadAddr = 0;
	CVI_U8 *pInputAddr = CVI_NULL;
	CVI_U8 *pOutputAddr = CVI_NULL;
	CVI_U8 *pAadAddr = CVI_NULL;
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_U32 u32TagLen;
	CVI_UNF_CIPHER_CTRL_EX_S CipherCtrl;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;
	CVI_UNF_CIPHER_CTRL_AES_CCM_GCM_S stCtrl;
	CVI_U8 aes_key[32] = { "\xfe\xff\xe9\x92\x86\x65\x73\x1c\x6d\x6a\x8f\x94\x67\x30\x83\x08" };
	CVI_U8 aes_iv[12] = { "\xca\xfe\xba\xbe\xfa\xce\xdb\xad\xde\xca\xf8\x88" };
	CVI_U8 aes_a[20] = { "\xfe\xed\xfa\xce\xde\xad\xbe\xef\xfe\xed\xfa\xce\xde\xad\xbe\xef\xab\xad\xda\xd2" };
	CVI_U8 aes_src[60] = { "\xd9\x31\x32\x25\xf8\x84\x06\xe5\xa5\x59\x09\xc5\xaf\xf5\x26\x9a"
			       "\x86\xa7\xa9\x53\x15\x34\xf7\xda\x2e\x4c\x30\x3d\x8a\x31\x8a\x72"
			       "\x1c\x3c\x0c\x95\x95\x68\x09\x53\x2f\xcf\x0e\x24\x49\xa6\xb5\x25"
			       "\xb1\x6a\xed\xf5\xaa\x0d\xe6\x57\xba\x63\x7b\x39" };
	CVI_U8 aes_dst[60] = { "\x42\x83\x1e\xc2\x21\x77\x74\x24\x4b\x72\x21\xb7\x84\xd0\xd4\x9c"
			       "\xe3\xaa\x21\x2f\x2c\x02\xa4\xe0\x35\xc1\x7e\x23\x29\xac\xa1\x2e"
			       "\x21\xd5\x14\xb2\x54\x66\x93\x1c\x7d\x8f\x6a\x5a\xac\x84\xaa\x05"
			       "\x1b\xa3\x0b\x39\x6a\x0a\xac\x97\x3d\x58\xe0\x91" };
	CVI_U8 aes_tag[16] = { "\x5b\xc9\x4f\xbc\x32\x21\xa5\xdb\x94\xfa\xe9\x5a\xe7\x12\x1a\x47" };
	CVI_U8 out_tag[16];

	printf("\n--------------------------%s-----------------------\n", __func__);

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	memset(out_tag, 0x0, 16);

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_UNF_CIPHER_DeInit();
		return s32Ret;
	}
	szInputAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szInputAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
		goto __CIPHER_EXIT__;
	}
	pInputAddr = (CVI_U8 *)(szInputAddr);

	szOutPutAddr = CVI_Test_Alloc(u32TestDataLen);
	if (szOutPutAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
		goto __CIPHER_EXIT__;
	}
	pOutputAddr = (CVI_U8 *)(szOutPutAddr);

	szAadAddr = CVI_Test_Alloc(sizeof(aes_a));
	if (szAadAddr == 0) {
		CVI_ERR_CIPHER("Error: Get memory for AAD failed!\n");
		goto __CIPHER_EXIT__;
	}
	pAadAddr = (CVI_U8 *)(szAadAddr);

	memset(&CipherCtrl, 0, sizeof(CVI_UNF_CIPHER_CTRL_EX_S));
	CipherCtrl.enAlg = CVI_UNF_CIPHER_ALG_AES;
	CipherCtrl.enWorkMode = CVI_UNF_CIPHER_WORK_MODE_GCM;
	stCtrl.enKeyLen = CVI_UNF_CIPHER_KEY_AES_128BIT;
	stCtrl.u32IVLen = sizeof(aes_iv);
	stCtrl.u32TagLen = sizeof(aes_tag);
	stCtrl.u32ALen = sizeof(aes_a);
	stCtrl.szAdataAddr = szAadAddr;
	memcpy(stCtrl.u32IV, aes_iv, sizeof(aes_iv));
	memcpy(stCtrl.u32Key, aes_key, 16);
	memcpy(pAadAddr, aes_a, sizeof(aes_a));
	CipherCtrl.pParam = &stCtrl;
	s32Ret = CVI_UNF_CIPHER_ConfigHandleEx(hTestchnid, &CipherCtrl);
	if (s32Ret != CVI_SUCCESS) {
		return CVI_FAILURE;
	}

	memset(pInputAddr, 0x0, u32TestDataLen);
	memcpy(pInputAddr, aes_src, u32TestDataLen);
	CVI_Test_PrintBuffer("GCM-AES-128-ORI:", aes_src, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);
	s32Ret = CVI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("GCM-AES-128-ENC:", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_dst, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	u32TagLen = 16;
	s32Ret = CVI_UNF_CIPHER_GetTag(hTestchnid, out_tag, &u32TagLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Get tag failed.\n");
		goto __CIPHER_EXIT__;
	}
	CVI_Test_PrintBuffer("GCM-AES-128-TAG", out_tag, 16);
	if (memcmp(out_tag, aes_tag, 16) != 0) {
		CVI_ERR_CIPHER("Tag compare failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_ConfigHandleEx(hTestchnid, &CipherCtrl);
	if (s32Ret != CVI_SUCCESS) {
		return CVI_FAILURE;
	}

	memcpy(pInputAddr, aes_dst, u32TestDataLen);
	memset(pOutputAddr, 0x0, u32TestDataLen);

	s32Ret = CVI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddr, szOutPutAddr, u32TestDataLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher decrypt failed.\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	CVI_Test_PrintBuffer("GCM-AES-128-DEC", pOutputAddr, u32TestDataLen);

	if (memcmp(pOutputAddr, aes_src, u32TestDataLen) != 0) {
		CVI_ERR_CIPHER("Memcmp failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	u32TagLen = 16;
	s32Ret = CVI_UNF_CIPHER_GetTag(hTestchnid, out_tag, &u32TagLen);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Get tag failed.\n");
		goto __CIPHER_EXIT__;
	}
	CVI_Test_PrintBuffer("GCM-AES-128-TAG", out_tag, 16);
	if (memcmp(out_tag, aes_tag, 16) != 0) {
		CVI_ERR_CIPHER("Tag compare failed!\n");
		s32Ret = CVI_FAILURE;
		goto __CIPHER_EXIT__;
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

int sample_cipher(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = ECB_AES256();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	s32Ret = CBC_AES256();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	s32Ret = CBC_AES192();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	s32Ret = CBC_AES128();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	s32Ret = CFB_AES128();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	s32Ret = CFB_AES128_COPY_AVOID();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	s32Ret = CTR_AES128();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	s32Ret = CCM_AES128();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	s32Ret = CCM_AES128_2();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	s32Ret = GCM_AES128();
	if (s32Ret != CVI_SUCCESS) {
		return s32Ret;
	}

	return CVI_SUCCESS;
}
