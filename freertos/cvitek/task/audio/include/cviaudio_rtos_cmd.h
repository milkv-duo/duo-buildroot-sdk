/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cviaudio_rtos_cmd.h
 * Description:
 */

#ifndef __CVIAUDIO_RTOS_CMD_H__
#define __CVIAUDIO_RTOS_CMD_H__

#include <stdbool.h>
#include <stddef.h>
//#include <stdint.h>
#include "cvi_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*----------------------------------------------
 * The common data type
 *----------------------------------------------
 */
#define CVIAUDIO_SSP_CHUNK_NUMBERS 25
#define CVIAUDIO_SSP_SINGLE_CHUNK_SIZE_BYTES (160*2) //1 chunk = 160 samples
#define CVIAUDIO_SSP_SINGLE_CHUNK_SAMPLES	(160)
#define CVIAUDIO_MAX_BUFFER_SIZE	((320)*(CVIAUDIO_SSP_CHUNK_NUMBERS))
//4000bytes 0.5 sec for 8k, 0.25 sec for 16k
typedef struct _st_ssp_buffer_table {
	unsigned char bBufOccupy;
	uint64_t	mic_in_addr;//save the phy address
	uint64_t	ref_in_addr;
	uint64_t	output_addr;
} ST_SSP_BUFTBL;
//data struct for CVIAUDIO_RTOS_CMD_SSP_INIT
typedef struct _ssp_rots_init {
	//AI_TALKVQE_CONFIG_S stAinVqeConfig;
	CVI_S32 s32RevMask;
	uint32_t CbPhyAddr;
	ST_SSP_BUFTBL	buffertbl[CVIAUDIO_SSP_CHUNK_NUMBERS];
	char *pmic_in_vir;
	char *pref_in_vir;
	char *poutput_vir;
	char *CbVirAddr;
} ST_SSP_RTOS_INIT;

typedef struct _ssp_mic_vqe_addr {
	uint64_t AinVqeCfgPhy;
	AI_TALKVQE_CONFIG_S *pAinVqeCfg;
} ST_SSP_MIC_VQE_ADDR;

typedef struct _ssp_mic_buf_table_addr {
	uint64_t	buffertblPhy;
	uint64_t	mic_in_phy;
	uint64_t	ref_in_phy;
	uint64_t	output_phy;
	ST_SSP_BUFTBL	*pbuffertbl;//buffertbl[CVIAUDIO_SSP_CHUNK_NUMBERS];
	char *pmic_in_vir;
	char *pref_in_vir;
	char *poutput_vir;
} ST_SSP_MIC_BUF_TABLE_ADDR;

typedef struct _ssp_rtos_indicator {
	unsigned char chunks_number;
	unsigned char Wpt_index;//for input pcm original pcm data
	unsigned char Rpt_index;//for user space copy to user
	unsigned char Ppt_index;//for RTOS process data
	unsigned char channel_nums;
	unsigned char ssp_on;
	unsigned char ssp_with_aec;
	unsigned char reserved_bytes[3];//3 bytes reserve, force 64bytes alignment
	unsigned int msg_counter;
} ST_SSP_RTOS_INDICATOR;
typedef struct _ssp_rtos_indicator_addr {
	uint64_t indicatorPhy;
	ST_SSP_RTOS_INDICATOR *pindicator;
} ST_SSP_RTOS_INDICATOR_ADDR;
typedef struct _st_cviaduio_mailbox {
	uint64_t u64RevMask;
	uint64_t AinVqeCfgPhy;
	uint64_t buffertblPhy;
	uint64_t indicatorPhy;
} ST_CVIAUDIO_MAILBOX;

//for block mode command from user space ---- start
typedef struct _st_cviaudio_mailbox_block {
	uint64_t u64RevMask;
	uint64_t AinVqeCfgPhy;
} ST_CVIAUDIO_MAILBOX_BLOCK;

typedef struct _st_civaudio_block_frame {
	uint64_t u64RevMask;
	uint64_t mic_in_addr;
	uint64_t ref_in_addr;
	uint64_t output_addr;
} ST_CVIAUDIO_BLOCK_FRAME;
//for block mode command from user space ---- end

typedef struct _st_spk_ssp_buffer_table {
	unsigned char bBufOccupy;
	size_t spk_in_addr;
	size_t spk_out_addr;
} ST_SPK_SSP_BUFTBL;
//data struct for CVIAUDIO_RTOS_CMD_SSP_SPK_INIT
typedef struct _ssp_spk_rtos_init {
	AO_VQE_CONFIG_S stAoutVqeConfig;
	ST_SPK_SSP_BUFTBL spkbuffertbl[CVIAUDIO_SSP_CHUNK_NUMBERS];
} ST_SPK_SSP_RTOS_INIT;

//data struct for CVIAUDIO_RTOS_CMD_SSP_SPK_PROCESS
typedef struct _spk_ssp_data_rtos {
	unsigned char chunks_number;
	unsigned char wpt_index;
	unsigned char channel_nums;
	unsigned int msg_spk_counter;
} ST_SSP_RTOS_SPK_DATA;

typedef struct _ssp_spk_data_rtos_ret {
	unsigned char status;
	unsigned int cb_command;
} ST_SSP_RTOS_SPK_DATA_RET;


/*----------------------------------------------
 * definition of cmd
 *----------------------------------------------
 */
/*define for the magic word noted for transmittion between kernel and rtos */
#define CVIAUDIO_RTOS_MAGIC_WORD_UNIT_TEST_MODE 0x38
#define CVIAUDIO_RTOS_MAGIC_WORD_KERNEL_BIND_MODE 0x99
#define CVIAUDIO_RTOS_MAGIC_WORD_USERSPACE_BLOCK_MODE 0xbb
#define CVIAUDIO_RTOS_BLOCK_MODE_FAILURE_FLAG 0xfa
#define CVIAUDIO_RTOS_TRIGGER_THRESHOLD	3
#define CVAUDIO_AUDIO_ION_SIZE_REQUIRE 0x1000
#ifndef CVIAUDIO_BYTES_ALIGNMENT
#define CVIAUDIO_BYTES_ALIGNMENT  64
#endif
#ifndef CVIAUDIO_ALIGN
#define CVIAUDIO_ALIGN(x, a)      (((x) + ((a)-1)) & ~((a)-1))
#endif
#define CVIAUDIO_BUF_TBL_UNOCCUPIED 0
#define CVIAUDIO_BUF_TBL_INPUT	1
#define CVIAUDIO_BUF_TBL_AFTER_SSP 2

/*define the audio SSP CMD---------------start*/
#define CVIAUDIO_RTOS_CMD_SSP_INIT 0x01
#define CVIAUDIO_RTOS_CMD_SSP_DEINIT 0x02
#define CVIAUDIO_RTOS_CMD_SSP_PROCESS 0x03
#define CVIAUDIO_RTOS_CMD_SSP_SPK_INIT 0x04
#define CVIAUDIO_RTOS_CMD_SSP_SPK_DEINIT 0x05
#define CVIAUDIO_RTOS_CMD_SSP_SPK_PROCESS 0x06
#define CVIAUDIO_RTOS_CMD_SSP_DEBUG 0x07
#define CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST 0x08
#define CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST_BLOCK_MODE_INIT	0x09
#define CVIAUDIO_RTOS_CMD_SSP_UNIT_TEST_BLOCK_MODE_GET	0x0A
#define CVIAUDIO_RTOS_CMD_SSP_INIT_BLOCK 0x0B
#define CVIAUDIO_RTOS_CMD_SSP_PROC_BLOCK 0x0C
#define CVIAUDIO_RTOS_CMD_SSP_DEINIT_BLOCK 0x0D
#define CVIAUDIO_RTOS_CMD_SSP_CONFIG_BLOCK 0x0E

#define CVIAUDIO_RTOS_CMD_SSP_MAX	0xFF


#define CVIAUDIO_CMD_STATUS_RECEIVE 0x10
#define CVIAUDIO_CMD_STATUS_OPERATE 0x20
#define CVIAUDIO_CMD_STATUS_FINISHED 0x40
#define CVIAUDIO_CMD_STATUS_ERROR    0x80


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVIAUDIO_RTOS_CMD_H__ */
