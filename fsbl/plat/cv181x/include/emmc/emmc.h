/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __EMMC_H__
#define __EMMC_H__

#include <stdint.h>

#define EMMC_IDENTIFIED_TIMEOUT (1500)
#define EMMC_CMD_TIMEOUT (100) // Follow same setting
#define EMMC_DATA_CMD_TIMEOUT (10000) // Follow linux kernel setting
#define EMMC_READY_FOR_DATA_TIMEOUT (5000) // Should not be happened

#define EMMC_BLOCK_SIZE 512
#define EMMC_BLOCK_MASK (EMMC_BLOCK_SIZE - 1)
#define EMMC_BOOT_CLK_RATE (400 * 1000)

#define EMMC_CMD0 0
#define EMMC_CMD1 1
#define EMMC_CMD2 2
#define EMMC_CMD3 3
#define EMMC_CMD6 6
#define EMMC_CMD7 7
#define EMMC_CMD8 8
#define EMMC_CMD9 9
#define EMMC_CMD12 12
#define EMMC_CMD13 13
#define EMMC_CMD17 17
#define EMMC_CMD18 18
#define EMMC_CMD23 23
#define EMMC_CMD24 24
#define EMMC_CMD25 25
#define EMMC_CMD35 35
#define EMMC_CMD36 36
#define EMMC_CMD38 38

#define OCR_POWERUP (1 << 31)
#define OCR_BYTE_MODE (0 << 29)
#define OCR_SECTOR_MODE (2 << 29)
#define OCR_ACCESS_MODE_MASK (3 << 29)
#define OCR_VDD_MIN_2V7 (0x1ff << 15)
#define OCR_VDD_MIN_2V0 (0x7f << 8)
#define OCR_VDD_MIN_1V7 (1 << 7)

#define EMMC_RESPONSE_R1 1
#define EMMC_RESPONSE_R1B 1
#define EMMC_RESPONSE_R2 4
#define EMMC_RESPONSE_R3 1
#define EMMC_RESPONSE_R4 1
#define EMMC_RESPONSE_R5 1

#define EMMC_FIX_RCA 6 /* > 1 */
#define RCA_SHIFT_OFFSET 16

#define CMD_EXTCSD_PARTITION_CONFIG 179
#define CMD_EXTCSD_BUS_WIDTH 183
#define CMD_EXTCSD_HS_TIMING 185

#define PART_CFG_BOOT_PARTITION1_ENABLE (1 << 3)
#define PART_CFG_BOOT_PARTITION2_ENABLE (2 << 3)
#define PART_CFG_PARTITION_BOOT1_ACCESS (1 << 0)
#define PART_CFG_PARTITION_BOOT2_ACCESS (2 << 0)
#define PART_CFG_PARTITION_RPMB_ACCESS (3 << 0)
#define PART_CFG_PARTITION_GP1_ACCESS (4 << 0)
#define PART_CFG_PARTITION_GP2_ACCESS (5 << 0)
#define PART_CFG_PARTITION_GP3_ACCESS (6 << 0)
#define PART_CFG_PARTITION_GP4_ACCESS (7 << 0)

/* values in EXT CSD register */
#define EMMC_BUS_WIDTH_1 0
#define EMMC_BUS_WIDTH_4 1
#define EMMC_BUS_WIDTH_8 2
#define EMMC_BOOT_MODE_BACKWARD (0 << 3)
#define EMMC_BOOT_MODE_HS_TIMING (1 << 3)
#define EMMC_BOOT_MODE_DDR (2 << 3)

#define EXTCSD_SET_CMD (0 << 24)
#define EXTCSD_SET_BITS (1 << 24)
#define EXTCSD_CLR_BITS (2 << 24)
#define EXTCSD_WRITE_BYTES (3 << 24)
#define EXTCSD_CMD(x) (((x) & 0xff) << 16)
#define EXTCSD_VALUE(x) (((x) & 0xff) << 8)

#define STATUS_CURRENT_STATE(x) (((x) & 0xf) << 9)
#define STATUS_READY_FOR_DATA (1 << 8)
#define STATUS_SWITCH_ERROR (1 << 7)
#define EMMC_GET_STATE(x) (((x) >> 9) & 0xf)
#define EMMC_STATE_IDLE 0
#define EMMC_STATE_READY 1
#define EMMC_STATE_IDENT 2
#define EMMC_STATE_STBY 3
#define EMMC_STATE_TRAN 4
#define EMMC_STATE_DATA 5
#define EMMC_STATE_RCV 6
#define EMMC_STATE_PRG 7
#define EMMC_STATE_DIS 8
#define EMMC_STATE_BTST 9
#define EMMC_STATE_SLP 10

#define EMMC_FLAG_CMD23 (1 << 0)
typedef enum {
	EMMC_PARTITION_BOOT1 = 1,
	EMMC_PARTITION_BOOT2 = 2,
	EMMC_PARTITION_RPMB = 3,
	EMMC_PARTITION_GP1 = 4,
	EMMC_PARTITION_GP2 = 5,
	EMMC_PARTITION_GP3 = 6,
	EMMC_PARTITION_GP4 = 7,
	EMMC_PARTITION_UDA = 8,

	EMMC_PARTITION_MAX = 0xFF
} emmc_partition_type;

typedef struct emmc_cmd {
	unsigned int cmd_idx;
	unsigned int cmd_arg;
	unsigned int resp_type;
	unsigned int resp_data[4];
} emmc_cmd_t;

typedef struct emmc_ops {
	void (*init)(void);
	void (*reset)(void);
	int (*send_cmd)(emmc_cmd_t *cmd);
	int (*set_ios)(int clk, int width);
	int (*prepare)(int lba, uintptr_t buf, size_t size);
	int (*read)(int lba, uintptr_t buf, size_t size);
	int (*write)(int lba, const uintptr_t buf, size_t size);
} emmc_ops_t;

typedef struct emmc_cid {
	unsigned int manufacturing_date : 8;
	unsigned int product_serial_low : 24;
	unsigned short product_serial_high : 8;
	unsigned short product_revision : 8;
	char product_name[6];
	unsigned int oem_application_id : 8;
	unsigned int card_bga : 2;
	unsigned int reserverd_0 : 6;
	unsigned int manufacturer_id : 8;
	unsigned int reserved_1 : 8; /*dw spec page 231 response135-104 */
} emmc_cid_t;

typedef struct emmc_csd {
	/*data0*/
	unsigned int ecc : 2;
	unsigned int file_format : 2;
	unsigned int tmp_write_protect : 1;
	unsigned int perm_write_protect : 1;
	unsigned int copy : 1;
	unsigned int file_format_grp : 1;
	unsigned int content_prot_app : 1;
	unsigned int reserved_1 : 4;
	unsigned int write_bl_partial : 1;
	unsigned int write_bl_len : 4;
	unsigned int r2w_factor : 3;
	unsigned int default_ecc : 2;
	unsigned int wp_grp_enable : 1;
	unsigned int wp_grp_size : 5;
	unsigned int erase_grp_mult_low : 3;

	/*data1*/
	unsigned int erase_grp_mult_high : 2;
	unsigned int erase_grp_size : 5;
	unsigned int c_size_mult : 3;
	unsigned int vdd_w_curr_max : 3;
	unsigned int vdd_w_curr_min : 3;
	unsigned int vdd_r_curr_max : 3;
	unsigned int vdd_r_curr_min : 3;
	unsigned int c_size_low : 10;

	/*data2*/
	unsigned int c_size_high : 2;
	unsigned int reserved_2 : 2;
	unsigned int dsr_imp : 1;
	unsigned int read_blk_misalign : 1;
	unsigned int write_blk_misalign : 1;
	unsigned int read_bl_partial : 1;
	unsigned int read_bl_len : 4;
	unsigned int ccc : 12;
	unsigned int tran_speed : 8;

	/*data3*/
	unsigned int nsac : 8; /*bit 104*/
	unsigned int taac : 8;
	unsigned int reserved_3 : 2;
	unsigned int spec_vers : 4;
	unsigned int csd_structure : 2;
	unsigned int reserved_4 : 8; /*dw spec page 231 response135-104 */
} emmc_csd_t;

size_t emmc_read_blocks(int lba, uintptr_t buf, size_t size);
size_t emmc_write_blocks(int lba, const uintptr_t buf, size_t size);
size_t emmc_erase_blocks(int lba, size_t size);
size_t emmc_partition_read_blocks(emmc_partition_type type, int lba, uintptr_t buf, size_t size);
size_t emmc_partition_write_blocks(emmc_partition_type type, int lba, const uintptr_t buf, size_t size);
size_t emmc_partition_erase_blocks(emmc_partition_type type, int lba, size_t size);
int emmc_init(const emmc_ops_t *ops, int clk, int bus_width, unsigned int flags);
size_t emmc_read_block_cmd8(int lba, uintptr_t buf, size_t size);
size_t emmc_boot1_read_blocks(int lba, uintptr_t buf, size_t size);
size_t emmc_boot1_write_blocks(int lba, uintptr_t buf, size_t size);
void emmc_ut(void);

#include <cv_emmc.h>

#endif /* __EMMC_H__ */
