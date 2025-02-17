/*
 * Copyright (c) 2016-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Defines a simple and generic interface to access eMMC device.
 */

#include <cpu.h>
#include <debug.h>
#include <emmc.h>
#include <errno.h>
#include <string.h>
#include <utils.h>
#include <platform.h>
#include <delay_timer.h>

static const emmc_ops_t *ops;
static unsigned int emmc_ocr_value;
static emmc_cid_t emmc_cid;
static emmc_csd_t emmc_csd;
static unsigned int emmc_flags;

static int is_cmd23_enabled(void)
{
	return (!!(emmc_flags & EMMC_FLAG_CMD23));
}

static int emmc_device_state(void)
{
	emmc_cmd_t cmd;
	int ret;
	unsigned int emmc_timer = 0;

	emmc_timer = get_timer(0);
	do {
		zeromem(&cmd, sizeof(emmc_cmd_t));
		cmd.cmd_idx = EMMC_CMD13;
		cmd.cmd_arg = EMMC_FIX_RCA << RCA_SHIFT_OFFSET;
		cmd.resp_type = EMMC_RESPONSE_R1;
		ret = ops->send_cmd(&cmd);
		if (ret < 0)
			return ret;
		if ((cmd.resp_data[0] & STATUS_SWITCH_ERROR) != 0) {
			ERROR("%s Response with Error: 0x%08x\n", __func__, cmd.resp_data[0]);
			return -EIO;
		}
		if (get_timer(emmc_timer) > EMMC_READY_FOR_DATA_TIMEOUT) {
			ERROR("%s Card stuck in programming state: 0x%08x\n", __func__, cmd.resp_data[0]);
			return -EIO;
		}
	} while ((cmd.resp_data[0] & STATUS_READY_FOR_DATA) == 0);

	return EMMC_GET_STATE(cmd.resp_data[0]);
}

static int emmc_set_ext_csd(unsigned int ext_cmd, unsigned int value)
{
	emmc_cmd_t cmd;
	int ret, state;

	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD6;
	cmd.cmd_arg = EXTCSD_WRITE_BYTES | EXTCSD_CMD(ext_cmd) | EXTCSD_VALUE(value) | 1;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	/* wait to exit PRG state */
	do {
		state = emmc_device_state();
		if (state < 0)
			return state;
	} while (state == EMMC_STATE_PRG);

	return ret;
}

extern void bm_emmc_set_ext_csd(unsigned int index, unsigned int value);

static int emmc_set_ios(int clk, int bus_width)
{
	int ret;

	/* set IO speed & IO bus width */
	if (emmc_csd.spec_vers == 4) {
		ret = emmc_set_ext_csd(CMD_EXTCSD_BUS_WIDTH, bus_width);
		if (ret < 0)
			return ret;
	}

	return ops->set_ios(clk, bus_width);
}

#define MAX_EMMC_INIT_ROUND 2

static int emmc_enumerate(int clk, int bus_width)
{
	emmc_cmd_t cmd;
	int ret, state;
	uint32_t init_round = 0;
	uint32_t emmc_timer = 0;

	ops->init();

retry:
	init_round++;

	if (init_round > MAX_EMMC_INIT_ROUND) {
		ERROR("eMMC init failed, %d\n", init_round);
		return -EIO;
	}

	ops->reset();

	/* CMD0: reset to IDLE */
	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD0;
	ret = ops->send_cmd(&cmd);
	if (ret != 0)
		goto retry;

	emmc_timer = get_timer(0);
	while (1) {
		/* CMD1: get OCR register */
		zeromem(&cmd, sizeof(emmc_cmd_t));
		cmd.cmd_idx = EMMC_CMD1;
		cmd.cmd_arg = OCR_SECTOR_MODE | OCR_VDD_MIN_1V7;
		cmd.resp_type = EMMC_RESPONSE_R3;
		ret = ops->send_cmd(&cmd);
		if (ret != 0)
			goto retry;

		emmc_ocr_value = cmd.resp_data[0];
		if (emmc_ocr_value & OCR_POWERUP)
			break;

		if (get_timer(emmc_timer) > EMMC_IDENTIFIED_TIMEOUT) {
			ERROR("eMMC wait POWRUP ready timeout: 0x%08x\n", emmc_ocr_value);
			return -EIO;
		}
	}

	/* CMD2: Card Identification */
	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD2;
	cmd.resp_type = EMMC_RESPONSE_R2;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	memcpy(&emmc_cid, &cmd.resp_data, sizeof(cmd.resp_data));
	INFO("cid manufacturer_id:0x%x card_bga:0x%x name:%s\n", emmc_cid.manufacturer_id, emmc_cid.card_bga,
	     emmc_cid.product_name);

	/* CMD3: Set Relative Address */
	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD3;
	cmd.cmd_arg = EMMC_FIX_RCA << RCA_SHIFT_OFFSET;
	cmd.resp_type = EMMC_RESPONSE_R1;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	/* CMD9: CSD Register */
	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD9;
	cmd.cmd_arg = EMMC_FIX_RCA << RCA_SHIFT_OFFSET;
	cmd.resp_type = EMMC_RESPONSE_R2;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	memcpy(&emmc_csd, &cmd.resp_data, sizeof(cmd.resp_data));
	INFO("csd spec_vers:0x%x erase_grp_size:0x%x\n", emmc_csd.spec_vers, emmc_csd.erase_grp_size);

	/* CMD7: Select Card */
	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD7;
	cmd.cmd_arg = EMMC_FIX_RCA << RCA_SHIFT_OFFSET;
	cmd.resp_type = EMMC_RESPONSE_R1;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	/* wait to TRAN state */
	do {
		state = emmc_device_state();
		if (state < 0)
			return state;
	} while (state != EMMC_STATE_TRAN);

	return emmc_set_ios(clk, bus_width);
}

size_t emmc_read_blocks(int lba, uintptr_t buf, size_t size)
{
	emmc_cmd_t cmd;
	int ret;

	if (!((ops != 0) && (ops->read != 0) && ((buf & EMMC_BLOCK_MASK) == 0) && ((size & EMMC_BLOCK_MASK) == 0)))
		return -EINVAL;

	inv_dcache_range(buf, size);
	ret = ops->prepare(lba, buf, size); // bm_emmc_prepare
	if (ret < 0)
		return ret;

	if (is_cmd23_enabled()) {
		zeromem(&cmd, sizeof(emmc_cmd_t));
		/* set block count */
		cmd.cmd_idx = EMMC_CMD23;
		cmd.cmd_arg = size / EMMC_BLOCK_SIZE;
		cmd.resp_type = EMMC_RESPONSE_R1;
		ret = ops->send_cmd(&cmd);
		if (ret < 0)
			return ret;

		zeromem(&cmd, sizeof(emmc_cmd_t));
		cmd.cmd_idx = EMMC_CMD18;
	} else {
		if (size > EMMC_BLOCK_SIZE)
			cmd.cmd_idx = EMMC_CMD18;
		else
			cmd.cmd_idx = EMMC_CMD17;
	}
	if ((emmc_ocr_value & OCR_ACCESS_MODE_MASK) == OCR_BYTE_MODE)
		cmd.cmd_arg = lba * EMMC_BLOCK_SIZE;
	else
		cmd.cmd_arg = lba;

	cmd.resp_type = EMMC_RESPONSE_R1;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	ret = ops->read(lba, buf, size);
	if (ret < 0)
		return ret;

	if (is_cmd23_enabled() == 0) {
		if (size > EMMC_BLOCK_SIZE) {
			zeromem(&cmd, sizeof(emmc_cmd_t));
			cmd.cmd_idx = EMMC_CMD12;
			ret = ops->send_cmd(&cmd);
			if (ret < 0)
				return ret;
		}
	}

	/* wait buffer empty */
	ret = emmc_device_state();
	if (ret < 0)
		return ret;

	return size;
}

size_t emmc_write_blocks(int lba, const uintptr_t buf, size_t size)
{
	emmc_cmd_t cmd;
	int ret;

	if (!((ops != 0) && (ops->write != 0) && ((buf & EMMC_BLOCK_MASK) == 0) && ((size & EMMC_BLOCK_MASK) == 0)))
		return -EINVAL;

	clean_dcache_range(buf, size);
	ret = ops->prepare(lba, buf, size);
	if (ret < 0)
		return ret;

	if (is_cmd23_enabled()) {
		/* set block count */
		zeromem(&cmd, sizeof(emmc_cmd_t));
		cmd.cmd_idx = EMMC_CMD23;
		cmd.cmd_arg = size / EMMC_BLOCK_SIZE;
		cmd.resp_type = EMMC_RESPONSE_R1;
		ret = ops->send_cmd(&cmd);
		if (ret < 0)
			return ret;

		zeromem(&cmd, sizeof(emmc_cmd_t));
		cmd.cmd_idx = EMMC_CMD25;
	} else {
		zeromem(&cmd, sizeof(emmc_cmd_t));
		if (size > EMMC_BLOCK_SIZE)
			cmd.cmd_idx = EMMC_CMD25;
		else
			cmd.cmd_idx = EMMC_CMD24;
	}
	if ((emmc_ocr_value & OCR_ACCESS_MODE_MASK) == OCR_BYTE_MODE)
		cmd.cmd_arg = lba * EMMC_BLOCK_SIZE;
	else
		cmd.cmd_arg = lba;

	cmd.resp_type = EMMC_RESPONSE_R1;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	ret = ops->write(lba, buf, size);
	if (ret < 0)
		return ret;

	if (is_cmd23_enabled() == 0) {
		if (size > EMMC_BLOCK_SIZE) {
			zeromem(&cmd, sizeof(emmc_cmd_t));
			cmd.cmd_idx = EMMC_CMD12;
			ret = ops->send_cmd(&cmd);
			if (ret < 0)
				return ret;
		}
	}

	/* wait buffer empty */
	ret = emmc_device_state();
	if (ret < 0)
		return ret;

	return size;
}

size_t emmc_erase_blocks(int lba, size_t size)
{
	emmc_cmd_t cmd;
	int ret, state;

	if (ops == 0)
		return -EINVAL;

	if ((size == 0) || ((size % EMMC_BLOCK_SIZE) != 0))
		return -EINVAL;

	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD35;
	cmd.cmd_arg = lba;
	cmd.resp_type = EMMC_RESPONSE_R1;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD36;
	cmd.cmd_arg = lba + (size / EMMC_BLOCK_SIZE) - 1;
	cmd.resp_type = EMMC_RESPONSE_R1;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	zeromem(&cmd, sizeof(emmc_cmd_t));
	cmd.cmd_idx = EMMC_CMD38;
	cmd.resp_type = EMMC_RESPONSE_R1B;
	ret = ops->send_cmd(&cmd);
	if (ret < 0)
		return ret;

	/* wait to TRAN state */
	do {
		state = emmc_device_state();
		if (state < 0)
			return state;
	} while (state != EMMC_STATE_TRAN);

	return size;
}

static inline int emmc_rw_enable(emmc_partition_type type)
{
	int ret;

	if ((type == 0) && (type == EMMC_PARTITION_MAX))
		return -EINVAL;

	switch (type) {
	case EMMC_PARTITION_BOOT1:
		ret = emmc_set_ext_csd(CMD_EXTCSD_PARTITION_CONFIG,
				       PART_CFG_BOOT_PARTITION1_ENABLE | PART_CFG_PARTITION_BOOT1_ACCESS);
		break;

	case EMMC_PARTITION_BOOT2:
		ret = emmc_set_ext_csd(CMD_EXTCSD_PARTITION_CONFIG,
				       PART_CFG_BOOT_PARTITION1_ENABLE | PART_CFG_PARTITION_BOOT2_ACCESS);
		break;

	case EMMC_PARTITION_RPMB:
		ret = emmc_set_ext_csd(CMD_EXTCSD_PARTITION_CONFIG,
				       PART_CFG_BOOT_PARTITION1_ENABLE | PART_CFG_PARTITION_RPMB_ACCESS);
		break;

	case EMMC_PARTITION_GP1:
		ret = emmc_set_ext_csd(CMD_EXTCSD_PARTITION_CONFIG,
				       PART_CFG_BOOT_PARTITION1_ENABLE | PART_CFG_PARTITION_GP1_ACCESS);
		break;

	case EMMC_PARTITION_GP2:
		ret = emmc_set_ext_csd(CMD_EXTCSD_PARTITION_CONFIG,
				       PART_CFG_BOOT_PARTITION1_ENABLE | PART_CFG_PARTITION_GP2_ACCESS);
		break;

	case EMMC_PARTITION_GP3:
		ret = emmc_set_ext_csd(CMD_EXTCSD_PARTITION_CONFIG,
				       PART_CFG_BOOT_PARTITION1_ENABLE | PART_CFG_PARTITION_GP3_ACCESS);
		break;

	case EMMC_PARTITION_GP4:
		ret = emmc_set_ext_csd(CMD_EXTCSD_PARTITION_CONFIG,
				       PART_CFG_BOOT_PARTITION1_ENABLE | PART_CFG_PARTITION_GP4_ACCESS);
		break;

	default:
		return -EINVAL;
	}

	return ret;
}

static inline int emmc_rw_disable(void)
{
	return emmc_set_ext_csd(CMD_EXTCSD_PARTITION_CONFIG, PART_CFG_BOOT_PARTITION1_ENABLE);
}

size_t emmc_partition_read_blocks(emmc_partition_type type, int lba, uintptr_t buf, size_t size)
{
	size_t size_read;
	int ret;

	ret = emmc_rw_enable(type);
	if (ret < 0)
		return ret;

	size_read = emmc_read_blocks(lba, buf, size);

	ret = emmc_rw_disable();
	if (ret < 0)
		return ret;

	return size_read;
}

size_t emmc_partition_write_blocks(emmc_partition_type type, int lba, const uintptr_t buf, size_t size)
{
	size_t size_written;
	int ret;

	ret = emmc_rw_enable(type);
	if (ret < 0)
		return ret;

	size_written = emmc_write_blocks(lba, buf, size);

	ret = emmc_rw_disable();
	if (ret < 0)
		return ret;

	return size_written;
}

size_t emmc_partition_erase_blocks(emmc_partition_type type, int lba, size_t size)
{
	size_t size_erased;
	int ret;

	ret = emmc_rw_enable(type);
	if (ret < 0)
		return ret;

	size_erased = emmc_erase_blocks(lba, size);

	ret = emmc_rw_disable();
	if (ret < 0)
		return ret;

	return size_erased;
}

size_t emmc_boot1_read_blocks(int lba, uintptr_t buf, size_t size)
{
	return emmc_partition_read_blocks(EMMC_PARTITION_BOOT1, lba, buf, size);
}

size_t emmc_boot1_write_blocks(int lba, uintptr_t buf, size_t size)
{
	return emmc_partition_write_blocks(EMMC_PARTITION_BOOT1, lba, buf, size);
}

#if DEBUG && ((defined(IMAGE_BL1) && defined(BL1_INIT_EMMC)) || (defined(IMAGE_BL2) && defined(BL2_INIT_EMMC)))
#define EMMC_TEST_SECTOR_NUMBER 0x64
#define EMMC_MICRON_8G_MAX_SECTOR_NUMBER 0xE30000
#define EMMC_TEST_LARGE_BLOCKS_EWR (64 * 1024)
#define EMMC_TEST_START_LBA 0x0
#define EMMC_TEST_RAM_ADDR 0x100000000
#define EMMC_TEST_NUM_LOOP 10

unsigned int wbuf[128 * 2] __attribute__((aligned(EMMC_BLOCK_SIZE))) = {
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
	0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5, 0xa5a5a5a5,
};
unsigned int rbuf[128 * 2] __attribute__((aligned(EMMC_BLOCK_SIZE))) = { 0 };

static void emmc_partition_ewr_test(emmc_partition_type type, int lba)
{
	int i;

	INFO("\r\n================type=%d lba=0x%x one sector erase write read start================== \r\n", type,
	     lba);
	if (type <= EMMC_PARTITION_GP4) {
		INFO("start erase \r\n");
		emmc_partition_erase_blocks(type, lba, EMMC_BLOCK_SIZE);
		INFO("start write \r\n");
		emmc_partition_write_blocks(type, lba, (uintptr_t)wbuf, EMMC_BLOCK_SIZE);
		INFO("start read \r\n");
		memset(rbuf, 0, EMMC_BLOCK_SIZE);
		emmc_partition_read_blocks(type, lba, (uintptr_t)rbuf, EMMC_BLOCK_SIZE);
	} else {
		INFO("start erase \r\n");
		emmc_erase_blocks(lba, EMMC_BLOCK_SIZE);
		INFO("start write \r\n");
		emmc_write_blocks(lba, (uintptr_t)wbuf, EMMC_BLOCK_SIZE);
		INFO("start read \r\n");
		memset(rbuf, 0, EMMC_BLOCK_SIZE);
		emmc_read_blocks(lba, (uintptr_t)rbuf, EMMC_BLOCK_SIZE);
	}

	for (i = 0; i < 128; i++) {
		//INFO("%x ",rbuf[i]);
		if (wbuf[i] != rbuf[i]) {
			ERROR("\r\ndata error:%d w:0x%x r:0x%x\n", i, wbuf[i], rbuf[i]);
			return;
		}
	}
	INFO("\r\n================type=%d lba=0x%x one sector erase write read end================== \r\n", type, lba);
}

static void emmc_partition_blocks_ewr_test(emmc_partition_type type, int lba)
{
	unsigned int i, j, m, n, k, sector;
	unsigned int *p = (unsigned int *)EMMC_TEST_RAM_ADDR;

	sector = lba;
	INFO("\r\n EWR blocks start type:%d \r\n", type);
	m = (EMMC_TEST_NUM_LOOP * EMMC_TEST_LARGE_BLOCKS_EWR) / EMMC_BLOCK_SIZE;
	INFO("Total sectors:%d \r\n", m);

	for (n = 0; n < m; n++) {
		for (j = 0; j < (EMMC_BLOCK_SIZE / sizeof(unsigned int)); j++) {
			*p++ = n + j;
			//INFO("%d %d",*(p-1),n+j);
		}
	}
	INFO("write ram *p:%d sectors:%d \r\n", (unsigned int)(*(p - 1)),
	     (unsigned int)((*(p - 1)) - (EMMC_BLOCK_SIZE / sizeof(unsigned int)) + 2));

	p = (unsigned int *)EMMC_TEST_RAM_ADDR;
	for (i = 0; i < EMMC_TEST_NUM_LOOP; i++) {
		//emmc_erase_blocks(lba,EMMC_TEST_LARGE_BLOCKS_EWR);
		emmc_partition_erase_blocks(type, sector, EMMC_TEST_LARGE_BLOCKS_EWR);
		//INFO("write large blocks start \r\n");
		emmc_partition_write_blocks(type, sector, (uintptr_t)p, EMMC_TEST_LARGE_BLOCKS_EWR);
		//INFO("read large blocks start \n");
		for (j = 0; j < EMMC_TEST_LARGE_BLOCKS_EWR / EMMC_BLOCK_SIZE; j++) {
			//INFO("sectors:%d \r\n",i*(EMMC_TEST_LARGE_BLOCKS_EWR/EMMC_BLOCK_SIZE)+j);
			memset((char *)rbuf, 0x0, sizeof(rbuf));
			emmc_partition_read_blocks(type, i * (EMMC_TEST_LARGE_BLOCKS_EWR / EMMC_BLOCK_SIZE) + j,
						   (uintptr_t)rbuf, EMMC_BLOCK_SIZE);

			for (k = 0; k < EMMC_BLOCK_SIZE / sizeof(unsigned int); k++) {
				if (rbuf[k] != k + j + (i * (EMMC_TEST_LARGE_BLOCKS_EWR / EMMC_BLOCK_SIZE))) {
					ERROR("\r\nError rbuf[%d]=%d content:%d \r\n", k, rbuf[k],
					      k + j + i * ((EMMC_TEST_LARGE_BLOCKS_EWR / EMMC_BLOCK_SIZE)));
					return;
				}
			}
		}
		sector += EMMC_TEST_LARGE_BLOCKS_EWR / EMMC_BLOCK_SIZE;
		p += (EMMC_TEST_LARGE_BLOCKS_EWR) / sizeof(unsigned int);
	}
	INFO("\r\nType:%d EWR blocks test end rbuf[%d]=%d content:%d \r\n", type, k - 1, rbuf[k - 1],
	     k - 1 + j - 1 + ((i - 1) * (EMMC_TEST_LARGE_BLOCKS_EWR / EMMC_BLOCK_SIZE)));
}

void emmc_ut(void)
{
	unsigned int i, j;

	INFO("\r\n================50Mhz erase write read start===================== \r\n");
	emmc_partition_ewr_test(EMMC_PARTITION_BOOT1, 0);
	emmc_partition_ewr_test(EMMC_PARTITION_BOOT2, 0);
	//emmc_partition_ewr_test(EMMC_PARTITION_RPMB,0);
	emmc_partition_ewr_test(EMMC_PARTITION_UDA, 0);
	emmc_partition_ewr_test(EMMC_PARTITION_UDA, (EMMC_MICRON_8G_MAX_SECTOR_NUMBER - 1));

	//INFO("\r\n================switch clk to 50MHZ start======================= \r\n");
	//send to device change to hight speed.
	//emmc_set_ext_csd(CMD_EXTCSD_HS_TIMING, 0x0);// 0X1 high speed  2 hs200  3 hs400
	//bm_emmc_set_clk(50*1000*1000);
	//INFO("\r\n================switch clk to 50MHZ end========================= \r\n");

	emmc_partition_ewr_test(EMMC_PARTITION_BOOT1, 0);
	emmc_partition_ewr_test(EMMC_PARTITION_BOOT2, 0);
	//emmc_partition_ewr_test(EMMC_PARTITION_RPMB,0);
	emmc_partition_ewr_test(EMMC_PARTITION_UDA, 0);
	emmc_partition_ewr_test(EMMC_PARTITION_UDA, (EMMC_MICRON_8G_MAX_SECTOR_NUMBER - 1));

	INFO("\r\nEMMC_PARTITION_BOOT1 write start\r\n");
	for (i = 0; i < EMMC_TEST_SECTOR_NUMBER; i += 2) {
		//INFO("sector:%d \r\n",i);
		emmc_partition_write_blocks(EMMC_PARTITION_BOOT1, i, (uintptr_t)wbuf, sizeof(wbuf));
	}

	INFO("\r\nEMMC_PARTITION_BOOT1 read start\r\n");
	for (i = 0; i < EMMC_TEST_SECTOR_NUMBER; i += 2) {
		memset((char *)rbuf, 0x0, sizeof(rbuf));
		emmc_partition_read_blocks(EMMC_PARTITION_BOOT1, i, (uintptr_t)rbuf, sizeof(rbuf));
		for (j = 0; j < sizeof(rbuf) / sizeof(unsigned int); j++) {
			if (wbuf[j] != rbuf[j]) {
				ERROR("\r\nboot1 data error:%d w:0x%x r:0x%x\r\n", j, wbuf[j], rbuf[j]);
				return;
			}
		}
	}

	INFO("\r\nEMMC_PARTITION_BOOT2 write start\r\n");
	for (i = 0; i < EMMC_TEST_SECTOR_NUMBER; i += 2)
		emmc_partition_write_blocks(EMMC_PARTITION_BOOT2, i, (uintptr_t)wbuf, sizeof(wbuf));

	INFO("\r\nEMMC_PARTITION_BOOT2 read start\r\n");
	for (i = 0; i < EMMC_TEST_SECTOR_NUMBER; i += 2) {
		memset((char *)rbuf, 0x0, sizeof(rbuf));
		emmc_partition_read_blocks(EMMC_PARTITION_BOOT2, i, (uintptr_t)rbuf, sizeof(rbuf));
		for (j = 0; j < sizeof(rbuf) / sizeof(unsigned int); j++) {
			if (wbuf[j] != rbuf[j]) {
				ERROR("\r\nboot2 data error:%d w:0x%x r:0x%x\r\n", j, wbuf[j], rbuf[j]);
				return;
			}
		}
	}

	INFO("\r\nEMMC_PARTITION_UDA writing low address start \r\n");
	for (i = 0; i < EMMC_TEST_SECTOR_NUMBER; i += 2)
		emmc_write_blocks(i, (uintptr_t)wbuf, sizeof(wbuf));

	INFO("\r\nEMMC_PARTITION_UDA reading low address start\r\n");
	for (i = 0; i < EMMC_TEST_SECTOR_NUMBER; i += 2) {
		memset((char *)rbuf, 0x0, sizeof(rbuf));
		emmc_read_blocks(i, (uintptr_t)rbuf, sizeof(rbuf));
		for (j = 0; j < sizeof(rbuf) / sizeof(unsigned int); j++) {
			if (wbuf[j] != rbuf[j]) {
				ERROR("\r\nuda data error:%d w:0x%x r:0x%x\n", j, wbuf[j], rbuf[j]);
				return;
			}
		}
	}
	INFO("\r\nEMMC_PARTITION_UDA writing high address start\r\n");
	for (i = EMMC_MICRON_8G_MAX_SECTOR_NUMBER - EMMC_TEST_SECTOR_NUMBER; i < EMMC_MICRON_8G_MAX_SECTOR_NUMBER;
	     i += 2) {
		emmc_write_blocks(i, (uintptr_t)wbuf, sizeof(wbuf));
	}

	INFO("\r\nEMMC_PARTITION_UDA reading high address start\r\n");
	for (i = EMMC_MICRON_8G_MAX_SECTOR_NUMBER - EMMC_TEST_SECTOR_NUMBER; i < EMMC_MICRON_8G_MAX_SECTOR_NUMBER;
	     i += 2) {
		memset((char *)rbuf, 0x0, sizeof(rbuf));
		emmc_read_blocks(i, (uintptr_t)rbuf, sizeof(rbuf));
		for (j = 0; j < sizeof(rbuf) / sizeof(unsigned int); j++) {
			if (wbuf[j] != rbuf[j]) {
				ERROR("\r\nuda data error:%d w:0x%x r:0x%x\n", j, wbuf[j], rbuf[j]);
				return;
			}
		}
	}
	emmc_partition_blocks_ewr_test(EMMC_PARTITION_BOOT1, 0x0);
	emmc_partition_blocks_ewr_test(EMMC_PARTITION_BOOT2, 0x0);
	emmc_partition_blocks_ewr_test(EMMC_PARTITION_UDA, 0x0);
	INFO("\r\n================50MHZ erase write read end==================== \r\n");
}
#endif

int emmc_init(const emmc_ops_t *ops_ptr, int clk, int width, unsigned int flags)
{
	if (!((ops_ptr != 0) && (ops_ptr->init != 0) && (ops_ptr->reset != 0) && (ops_ptr->send_cmd != 0) &&
	      (ops_ptr->set_ios != 0) && (ops_ptr->prepare != 0) && (ops_ptr->read != 0) && (ops_ptr->write != 0) &&
	      (clk != 0) &&
	      ((width == EMMC_BUS_WIDTH_1) || (width == EMMC_BUS_WIDTH_4) || (width == EMMC_BUS_WIDTH_8))))
		return -EINVAL;

	ops = ops_ptr;
	emmc_flags = flags;

	return emmc_enumerate(clk, width);
}
