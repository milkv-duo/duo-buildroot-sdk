/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_sample_multicipher.c
 * Description:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <cvi_unf_cipher.h>
#include <cvi_sample_all.h>

static const CVI_U8 des_key[32] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
static const CVI_U8 des_IV[32] = { 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
static const CVI_U8 des_ecb_enc_src_buf[16] = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
static const CVI_U8 des_ecb_enc_dst_buf[16] = { 0x17, 0x66, 0x8D, 0xFC, 0x72, 0x92, 0x53, 0x2D };
static const CVI_U8 des_ecb_dec_src_buf[16] = { 0x17, 0x66, 0x8D, 0xFC, 0x72, 0x92, 0x53, 0x2D };
static const CVI_U8 des_ecb_dec_dst_buf[16] = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };

static CVI_U8 aes_key[16] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
			      0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
static CVI_U8 aes_IV[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			     0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
static CVI_U8 aes_cbc_enc_src_buf[16] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
					  0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };
static CVI_U8 aes_cbc_enc_dst_buf[16] = { 0x76, 0x49, 0xAB, 0xAC, 0x81, 0x19, 0xB2, 0x46,
					  0xCE, 0xE9, 0x8E, 0x9B, 0x12, 0xE9, 0x19, 0x7D };
static CVI_U8 aes_cbc_dec_src_buf[16] = { 0x76, 0x49, 0xAB, 0xAC, 0x81, 0x19, 0xB2, 0x46,
					  0xCE, 0xE9, 0x8E, 0x9B, 0x12, 0xE9, 0x19, 0x7D };
static CVI_U8 aes_cbc_dec_dst_buf[16] = { 0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
					  0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A };

static CVI_S32 printBuffer(const CVI_CHAR *string, const CVI_U8 *pu8Input, CVI_U32 u32Length)
{
	CVI_U32 i = 0;

	if (string != NULL) {
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

static CVI_S32 CVI_Test_SetConfig(CVI_CIPHER_HANDLE chnHandle, CVI_UNF_CIPHER_ALG_E alg,
				  CVI_UNF_CIPHER_WORK_MODE_E mode, CVI_UNF_CIPHER_KEY_LENGTH_E keyLen,
				  const CVI_U8 u8KeyBuf[16], const CVI_U8 u8IVBuf[16])
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_UNF_CIPHER_CTRL_S CipherCtrl;

	memset(&CipherCtrl, 0, sizeof(CVI_UNF_CIPHER_CTRL_S));
	CipherCtrl.enAlg = alg;
	CipherCtrl.enWorkMode = mode;
	CipherCtrl.enBitWidth = CVI_UNF_CIPHER_BIT_WIDTH_64BIT;
	CipherCtrl.enKeyLen = keyLen;
	if (CipherCtrl.enWorkMode != CVI_UNF_CIPHER_WORK_MODE_ECB) {
		CipherCtrl.stChangeFlags.bit1IV = CIPHER_IV_CHANGE_ALL_PKG;
		memcpy(CipherCtrl.u32IV, u8IVBuf, 16);
	}

	memcpy(CipherCtrl.u32Key, u8KeyBuf, 16);

	s32Ret = CVI_UNF_CIPHER_ConfigHandle(chnHandle, &CipherCtrl);
	if (s32Ret != CVI_SUCCESS) {
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

static CVI_S32 multiCipher_DES_ECB_ENC(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 8;

	CVI_U8 *pInputAddr[3];
	CVI_U8 *pOutputAddr[3];

	CVI_CIPHER_HANDLE hTestchnid = 0;

	CVI_UNF_CIPHER_DATA_S cipherDataArray[3];
	CVI_U32 i;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;

	memset(&cipherDataArray, 0, sizeof(cipherDataArray));

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher init failed.\n");

		return CVI_FAILURE;
	}
	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher create handle failed.\n");
		goto __CIPHER_EXIT__;
	}
	for (i = 0; i < 3; i++) {
		cipherDataArray[i].szSrcAddr = CVI_Test_Alloc(u32TestDataLen);

		if (cipherDataArray[i].szSrcAddr == 0) {
			CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
		pInputAddr[i] = (CVI_U8 *)(cipherDataArray[i].szSrcAddr);

		cipherDataArray[i].szDestAddr = CVI_Test_Alloc(u32TestDataLen);
		if (cipherDataArray[i].szDestAddr == 0) {
			CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
		pOutputAddr[i] = (CVI_U8 *)(cipherDataArray[i].szDestAddr);

		cipherDataArray[i].u32ByteLength = u32TestDataLen;
		memset(pInputAddr[i], 0x0, u32TestDataLen);
		memcpy(pInputAddr[i], des_ecb_enc_src_buf, u32TestDataLen);
		memset(pOutputAddr[i], 0x0, u32TestDataLen);
	}

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_DES, CVI_UNF_CIPHER_WORK_MODE_ECB,
				    CVI_UNF_CIPHER_KEY_DES_2KEY, des_key, des_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher setconfiginfo failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_EncryptMulti(hTestchnid, cipherDataArray, 3);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encryptMulti failed.\n");
		goto __CIPHER_EXIT__;
	}

	for (i = 0; i < 3; i++) {
		if (memcmp(pOutputAddr[i], des_ecb_enc_dst_buf, u32TestDataLen) != 0) {
			CVI_ERR_CIPHER("MultiCipher DES ECB Encryption run failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 multiCipher_DES_ECB_DEC(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 8;

	CVI_U8 *pInputAddr[3];
	CVI_U8 *pOutputAddr[3];

	CVI_CIPHER_HANDLE hTestchnid = 0;

	CVI_UNF_CIPHER_DATA_S cipherDataArray[3];
	CVI_U32 i;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;

	memset(&cipherDataArray, 0, sizeof(cipherDataArray));

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher Init failed.\n");

		return CVI_FAILURE;
	}

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher Create handle failed.\n");
		goto __CIPHER_EXIT__;
	}

	for (i = 0; i < 3; i++) {
		cipherDataArray[i].szSrcAddr = CVI_Test_Alloc(u32TestDataLen);
		if (cipherDataArray[i].szSrcAddr == 0) {
			CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
		pInputAddr[i] = (CVI_U8 *)(cipherDataArray[i].szSrcAddr);

		cipherDataArray[i].szDestAddr = CVI_Test_Alloc(u32TestDataLen);
		if (cipherDataArray[i].szDestAddr == 0) {
			CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
		pOutputAddr[i] = (CVI_U8 *)(cipherDataArray[i].szDestAddr);

		cipherDataArray[i].u32ByteLength = u32TestDataLen;
		memset(pInputAddr[i], 0x0, u32TestDataLen);
		memcpy(pInputAddr[i], des_ecb_dec_src_buf, u32TestDataLen);
		memset(pOutputAddr[i], 0x0, u32TestDataLen);
	}

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_DES, CVI_UNF_CIPHER_WORK_MODE_ECB,
				    CVI_UNF_CIPHER_KEY_DES_2KEY, des_key, des_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher CVI_Test_SetConfig failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_DecryptMulti(hTestchnid, cipherDataArray, 3);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher DecryptMulti failed.\n");
		goto __CIPHER_EXIT__;
	}

	for (i = 0; i < 3; i++) {
		if (memcmp(pOutputAddr[i], des_ecb_dec_dst_buf, u32TestDataLen) != 0) {
			CVI_ERR_CIPHER("MultiCipher DES ECB Decryption run failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 multiCipher_ex_AES_CBC_ENC(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 16;
	CVI_U8 *pInputAddr[3] = { 0 };
	CVI_U8 *pOutputAddr[3] = { 0 };
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;
	CVI_UNF_CIPHER_DATA_S cipherDataArray[3];
	CVI_U32 i;

	memset(&cipherDataArray, 0, sizeof(cipherDataArray));

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher init failed.\n");

		return CVI_FAILURE;
	}

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher create handle failed.\n");
		goto __CIPHER_EXIT__;
	}

	for (i = 0; i < 3; i++) {
		cipherDataArray[i].szSrcAddr = CVI_Test_Alloc(u32TestDataLen);
		if (cipherDataArray[i].szSrcAddr == 0) {
			CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
		pInputAddr[i] = (CVI_U8 *)(cipherDataArray[i].szSrcAddr);

		cipherDataArray[i].szDestAddr = CVI_Test_Alloc(u32TestDataLen);
		if (cipherDataArray[i].szDestAddr == 0) {
			CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
		pOutputAddr[i] = (CVI_U8 *)(cipherDataArray[i].szDestAddr);

		cipherDataArray[i].u32ByteLength = u32TestDataLen;
		memset(pInputAddr[i], 0x0, u32TestDataLen);
		memcpy(pInputAddr[i], aes_cbc_enc_src_buf, u32TestDataLen);
		memset(pOutputAddr[i], 0x0, u32TestDataLen);
	}

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CBC,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher setconfiginfo failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_EncryptMulti(hTestchnid, cipherDataArray, 3);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher encryptMulti failed.\n");
		goto __CIPHER_EXIT__;
	}

	for (i = 0; i < 3; i++) {
		if (memcmp(pOutputAddr[i], aes_cbc_enc_dst_buf, u32TestDataLen) != 0) {
			CVI_ERR_CIPHER("MultiCipher AES CBC Encryption run failed on array %d!\n", i);
			printBuffer("ENC", pOutputAddr[i], u32TestDataLen);
			printBuffer("GOLD", aes_cbc_enc_dst_buf, u32TestDataLen);
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
	}
	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

static CVI_S32 multiCipher_ex_AES_CBC_DEC(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32TestDataLen = 16;
	CVI_U8 *pInputAddr[3] = { 0 };
	CVI_U8 *pOutputAddr[3] = { 0 };
	CVI_CIPHER_HANDLE hTestchnid = 0;
	CVI_UNF_CIPHER_ATTS_S stCipherAttr;
	CVI_UNF_CIPHER_DATA_S cipherDataArray[3];
	CVI_U32 i;

	memset(&cipherDataArray, 0, sizeof(cipherDataArray));

	s32Ret = CVI_UNF_CIPHER_Init();
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher init failed.\n");

		return CVI_FAILURE;
	}

	stCipherAttr.enCipherType = CVI_UNF_CIPHER_TYPE_NORMAL;
	s32Ret = CVI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher Create handle failed.\n");
		goto __CIPHER_EXIT__;
	}

	for (i = 0; i < 3; i++) {
		cipherDataArray[i].szSrcAddr = CVI_Test_Alloc(u32TestDataLen);
		if (cipherDataArray[i].szSrcAddr == 0) {
			CVI_ERR_CIPHER("Error: Get memory for input failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
		pInputAddr[i] = (CVI_U8 *)(cipherDataArray[i].szSrcAddr);

		cipherDataArray[i].szDestAddr = CVI_Test_Alloc(u32TestDataLen);
		if (cipherDataArray[i].szDestAddr == 0) {
			CVI_ERR_CIPHER("Error: Get memory for outPut failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
		pOutputAddr[i] = (CVI_U8 *)(cipherDataArray[i].szDestAddr);

		cipherDataArray[i].u32ByteLength = u32TestDataLen;
		memset(pInputAddr[i], 0x0, u32TestDataLen);
		memcpy(pInputAddr[i], aes_cbc_dec_src_buf, u32TestDataLen);
		memset(pOutputAddr[i], 0x0, u32TestDataLen);
	}

	s32Ret = CVI_Test_SetConfig(hTestchnid, CVI_UNF_CIPHER_ALG_AES, CVI_UNF_CIPHER_WORK_MODE_CBC,
				    CVI_UNF_CIPHER_KEY_AES_128BIT, aes_key, aes_IV);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher setconfiginfo failed.\n");
		goto __CIPHER_EXIT__;
	}

	s32Ret = CVI_UNF_CIPHER_DecryptMulti(hTestchnid, cipherDataArray, 3);
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Cipher DecryptMulti failed.\n");
		goto __CIPHER_EXIT__;
	}

	for (i = 0; i < 3; i++) {
		if (memcmp(pOutputAddr[i], aes_cbc_dec_dst_buf, u32TestDataLen) != 0) {
			CVI_ERR_CIPHER("MultiCipher AES CBC Decryption run failed!\n");
			s32Ret = CVI_FAILURE;
			goto __CIPHER_EXIT__;
		}
	}

	TEST_END_PASS();

__CIPHER_EXIT__:
	CVI_UNF_CIPHER_DestroyHandle(hTestchnid);
	CVI_UNF_CIPHER_DeInit();

	return s32Ret;
}

int sample_mutiltcipher(void)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = multiCipher_DES_ECB_ENC();
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Multi cipher des ecb encrypt failed.\n");
		return s32Ret;
	}

	s32Ret = multiCipher_DES_ECB_DEC();
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Multi cipher des ecb decrypt failed.\n");
		return s32Ret;
	}

	s32Ret = multiCipher_ex_AES_CBC_ENC();
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Multi cipher ex aes cbc encrypt failed.\n");
		return s32Ret;
	}

	s32Ret = multiCipher_ex_AES_CBC_DEC();
	if (s32Ret != CVI_SUCCESS) {
		CVI_ERR_CIPHER("Multi cipher ex aes cbc decrypt failed.\n");
		return s32Ret;
	}

	return CVI_SUCCESS;
}
