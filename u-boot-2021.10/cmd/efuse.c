// SPDX-License-Identifier: GPL-2.0+

#include <common.h>
#include <command.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <mmio.h>
#include <cvi_efuse.h>

#ifdef __riscv

#define EFUSE_DEBUG 0

#define _cc_trace(fmt, ...) __trace("", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define _cc_error(fmt, ...) __trace("ERROR:", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#define ERROR(fmt, ...) __trace("ERROR:", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#if EFUSE_DEBUG

#define VERBOSE(fmt, ...) __trace("VERBOSE:", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

static int __trace(const char *prefix, const char *path, const char *func, int lineno, const char *fmt, ...)
{
	va_list ap;
	int ret;

	printf("[%s%s:%s:%d] ", prefix, path, func, lineno);
	if (!fmt || fmt[0] == '\0') {
		ret = printf("\n");
	} else {
		va_start(ap, fmt);
		ret = vprintf(fmt, ap);
		va_end(ap);
	}

	return ret;
}
#else

#define VERBOSE(fmt, ...)

static int __trace(const char *prefix, const char *path, const char *func, int lineno, const char *fmt, ...)
{
	return 0;
}
#endif

static int hex2bytes(const char *hex, unsigned char *buf, int buf_size)
{
	int i, total = 0;
	char tmp[3];

	memset(buf, 0, buf_size);

	for (i = 0; i < buf_size; i++) {
		if (!hex[0] || !hex[1])
			break;

		tmp[0] = hex[0];
		tmp[1] = hex[1];
		tmp[2] = '\0';

		buf[i] = simple_strtoul(tmp, NULL, 16);
		hex += 2;
		total += 1;
	}

	return total;
}

// ===========================================================================
// EFUSE implementation
// ===========================================================================
#define EFUSE_SHADOW_REG (EFUSE_BASE + 0x100)
#define EFUSE_SIZE 0x100

#define EFUSE_MODE (EFUSE_BASE + 0x0)
#define EFUSE_ADR (EFUSE_BASE + 0x4)
#define EFUSE_DIR_CMD (EFUSE_BASE + 0x8)
#define EFUSE_RD_DATA (EFUSE_BASE + 0xC)
#define EFUSE_STATUS (EFUSE_BASE + 0x10)
#define EFUSE_ONE_WAY (EFUSE_BASE + 0x14)

#define EFUSE_BIT_AREAD BIT(0)
#define EFUSE_BIT_MREAD BIT(1)
#define EFUSE_BIT_PRG BIT(2)
#define EFUSE_BIT_PWR_DN BIT(3)
#define EFUSE_BIT_CMD BIT(4)
#define EFUSE_BIT_BUSY BIT(0)
#define EFUSE_CMD_REFRESH (0x30)

enum EFUSE_READ_TYPE { EFUSE_AREAD, EFUSE_MREAD };

static void cvi_efuse_wait_for_ready(void)
{
	while (mmio_read_32(EFUSE_STATUS) & EFUSE_BIT_BUSY)
		;
}

static void cvi_efuse_power_on(uint32_t on)
{
	if (on)
		mmio_setbits_32(EFUSE_MODE, EFUSE_BIT_CMD);
	else
		mmio_setbits_32(EFUSE_MODE, EFUSE_BIT_PWR_DN | EFUSE_BIT_CMD);
}

static void cvi_efuse_refresh(void)
{
	mmio_write_32(EFUSE_MODE, EFUSE_CMD_REFRESH);
}

static void cvi_efuse_prog_bit(uint32_t word_addr, uint32_t bit_addr, uint32_t high_row)
{
	uint32_t phy_addr;

	// word_addr: virtual addr, take "lower 6-bits" from 7-bits (0-127)
	// bit_addr: virtual addr, 5-bits (0-31)

	// composite physical addr[11:0] = [11:7]bit_addr + [6:0]word_addr
	phy_addr = ((bit_addr & 0x1F) << 7) | ((word_addr & 0x3F) << 1) | high_row;

	cvi_efuse_wait_for_ready();

	// send efuse program cmd
	mmio_write_32(EFUSE_ADR, phy_addr);
	mmio_write_32(EFUSE_MODE, EFUSE_BIT_PRG | EFUSE_BIT_CMD);
}

static uint32_t cvi_efuse_read_from_phy(uint32_t phy_word_addr, enum EFUSE_READ_TYPE type)
{
	// power on efuse macro
	cvi_efuse_power_on(1);

	cvi_efuse_wait_for_ready();

	mmio_write_32(EFUSE_ADR, phy_word_addr);

	if (type == EFUSE_AREAD) // array read
		mmio_write_32(EFUSE_MODE, EFUSE_BIT_AREAD | EFUSE_BIT_CMD);
	else if (type == EFUSE_MREAD) // margin read
		mmio_write_32(EFUSE_MODE, EFUSE_BIT_MREAD | EFUSE_BIT_CMD);
	else {
		ERROR("EFUSE: Unsupported read type!");
		return (uint32_t)-1;
	}

	cvi_efuse_wait_for_ready();

	return mmio_read_32(EFUSE_RD_DATA);
}

static int cvi_efuse_write_word(uint32_t vir_word_addr, uint32_t val)
{
	uint32_t i, j, row_val, zero_bit;
	uint32_t new_value;
	int err_cnt = 0;

	for (j = 0; j < 2; j++) {
		VERBOSE("EFUSE: Program physical word addr #%d\n", (vir_word_addr << 1) | j);

		// array read by word address
		row_val = cvi_efuse_read_from_phy((vir_word_addr << 1) | j,
						  EFUSE_AREAD); // read low word of word_addr
		zero_bit = val & (~row_val); // only program zero bit

		// program row which bit is zero
		for (i = 0; i < 32; i++) {
			if ((zero_bit >> i) & 1)
				cvi_efuse_prog_bit(vir_word_addr, i, j);
		}

		// check by margin read
		new_value = cvi_efuse_read_from_phy((vir_word_addr << 1) | j, EFUSE_MREAD);
		VERBOSE("%s(): val=0x%x new_value=0x%x\n", __func__, val, new_value);
		if ((val & new_value) != val) {
			err_cnt += 1;
			ERROR("EFUSE: Program bits check failed (%d)!\n", err_cnt);
		}
	}

	cvi_efuse_refresh();

	return err_cnt >= 2 ? -EIO : 0;
}

static void cvi_efuse_init(void)
{
	// power on efuse macro
	cvi_efuse_power_on(1);

	// send refresh cmd to reload all eFuse values to shadow registers
	cvi_efuse_refresh();

	// efuse macro will be auto powered off after refresh cmd, so don't
	// need to turn it off manually
}

void cvi_efuse_dump(uint32_t vir_word_addr)
{
	uint32_t j, val;

	for (j = 0; j < 2; j++) {
		// check by margin read
		val = cvi_efuse_read_from_phy((vir_word_addr << 1) | j, EFUSE_MREAD);
		printf("EFUSE EFUSE_MREAD: Program bits %d check 0x%x\n", j, val);
		val = cvi_efuse_read_from_phy((vir_word_addr << 1) | j, EFUSE_AREAD);
		printf("EFUSE EFUSE_AREAD: Program bits %d check 0x%x\n", j, val);
	}
}

int64_t cvi_efuse_read_from_shadow(uint32_t addr)
{
	if (addr >= EFUSE_SIZE)
		return -EFAULT;

	if (addr % 4 != 0)
		return -EFAULT;

	return mmio_read_32(EFUSE_SHADOW_REG + addr);
}

int cvi_efuse_write(uint32_t addr, uint32_t value)
{
	int ret;

	VERBOSE("%s(): 0x%x = 0x%x\n", __func__, addr, value);

	if (addr >= EFUSE_SIZE)
		return -EFAULT;

	if (addr % 4 != 0)
		return -EFAULT;

	ret = cvi_efuse_write_word(addr / 4, value);
	VERBOSE("%s(): ret=%d\n", __func__, ret);
	cvi_efuse_init();
	cvi_efuse_wait_for_ready();

	return ret;
}

// ===========================================================================
// EFUSE API
// ===========================================================================
enum CVI_EFUSE_LOCK_WRITE_E {
	CVI_EFUSE_LOCK_WRITE_HASH0_PUBLIC = CVI_EFUSE_OTHERS + 1,
	CVI_EFUSE_LOCK_WRITE_LOADER_EK,
	CVI_EFUSE_LOCK_WRITE_DEVICE_EK,
	CVI_EFUSE_LOCK_WRITE_LAST
};

static struct _CVI_EFUSE_AREA_S {
	CVI_U32 addr;
	CVI_U32 size;
} cvi_efuse_area[] = { [CVI_EFUSE_AREA_USER] = { 0x40, 40 },
		       [CVI_EFUSE_AREA_DEVICE_ID] = { 0x8c, 8 },
		       [CVI_EFUSE_AREA_HASH0_PUBLIC] = { 0xA8, 32 },
		       [CVI_EFUSE_AREA_LOADER_EK] = { 0xD8, 16 },
		       [CVI_EFUSE_AREA_DEVICE_EK] = { 0xE8, 16 } };

static struct _CVI_EFUSE_LOCK_S {
	CVI_S32 wlock_shift;
	CVI_S32 rlock_shift;
} cvi_efuse_lock[] = { [CVI_EFUSE_LOCK_HASH0_PUBLIC] = { 0, 8 },     [CVI_EFUSE_LOCK_LOADER_EK] = { 4, 12 },
		       [CVI_EFUSE_LOCK_DEVICE_EK] = { 6, 14 },	     [CVI_EFUSE_LOCK_WRITE_HASH0_PUBLIC] = { 0, -1 },
		       [CVI_EFUSE_LOCK_WRITE_LOADER_EK] = { 4, -1 }, [CVI_EFUSE_LOCK_WRITE_DEVICE_EK] = { 6, -1 } };

static struct _CVI_EFUSE_USER_S {
	CVI_U32 addr;
	CVI_U32 size;
} cvi_efuse_user[] = {
	{ 0x40, 4 }, { 0x48, 4 }, { 0x50, 4 }, { 0x58, 4 }, { 0x60, 4 },
	{ 0x68, 4 }, { 0x70, 4 }, { 0x78, 4 }, { 0x80, 4 }, { 0x88, 4 },
};

#define CVI_EFUSE_TOTAL_SIZE 0x100

#define CVI_EFUSE_LOCK_ADDR 0xF8
#define CVI_EFUSE_SECURE_CONF_ADDR 0xA0
#define CVI_EFUSE_SCS_ENABLE_SHIFT			0
// for secure boot sign
#define CVI_EFUSE_TEE_SCS_ENABLE_SHIFT			2
#define CVI_EFUSE_ROOT_PUBLIC_KEY_SELECTION_SHIFT	20
// for secure boot encryption
#define CVI_EFUSE_BOOT_LOADER_ENCRYPTION		6
#define CVI_EFUSE_LDR_KEY_SELECTION_SHIFT		23


CVI_S32 CVI_EFUSE_GetSize(enum CVI_EFUSE_AREA_E area, CVI_U32 *size)
{
	if (area >= ARRAY_SIZE(cvi_efuse_area) || cvi_efuse_area[area].size == 0) {
		_cc_error("area (%d) is not found\n", area);
		return CVI_ERR_EFUSE_INVALID_AREA;
	}

	if (size)
		*size = cvi_efuse_area[area].size;

	return 0;
}

CVI_S32 _CVI_EFUSE_Read(CVI_U32 addr, void *buf, CVI_U32 buf_size)
{
	int64_t ret = -1;
	int i;

	VERBOSE("%s(): 0x%x(%u) to %p\n", __func__, addr, buf_size, buf);

	if (!buf)
		return CVI_ERR_EFUSE_INVALID_PARA;

	if (buf_size > EFUSE_SIZE)
		buf_size = EFUSE_SIZE;

	for (i = 0; i < buf_size; i += 4) {
		ret = cvi_efuse_read_from_shadow(addr + i);
		VERBOSE("%s(): i=%x ret=%lx\n", __func__, i, ret);
		if (ret < 0)
			return ret;

		*(uint32_t *)(buf + i) = (ret >= 0) ? ret : 0;
	}

	return 0;
}

static CVI_S32 _CVI_EFUSE_Write(CVI_U32 addr, const void *buf, CVI_U32 buf_size)
{
	_cc_trace("addr=0x%02x\n", addr);

	int ret = -1;

	CVI_U32 value;
	int i;

	if (!buf)
		return CVI_ERR_EFUSE_INVALID_PARA;

	for (i = 0; i < buf_size; i += 4) {
		memcpy(&value, buf + i, sizeof(value));

		_cc_trace("smc call: 0x%02x=0x%08x\n", addr + i, value);
		ret = cvi_efuse_write(addr + i, value);

		if (ret < 0) {
			printf("%s: error (%d)\n", __func__, ret);
			return ret;
		}
	}

	return 0;
}

CVI_S32 CVI_EFUSE_Read(enum CVI_EFUSE_AREA_E area, CVI_U8 *buf, CVI_U32 buf_size)
{
	CVI_U32 user_size = cvi_efuse_area[CVI_EFUSE_AREA_USER].size;
	CVI_U8 user[user_size], *p;
	CVI_S32 ret;
	int i;

	if (area >= ARRAY_SIZE(cvi_efuse_area) || cvi_efuse_area[area].size == 0) {
		_cc_error("area (%d) is not found\n", area);
		return CVI_ERR_EFUSE_INVALID_AREA;
	}

	if (!buf)
		return CVI_ERR_EFUSE_INVALID_PARA;

	memset(buf, 0, buf_size);

	if (buf_size > cvi_efuse_area[area].size)
		buf_size = cvi_efuse_area[area].size;

	if (area != CVI_EFUSE_AREA_USER)
		return _CVI_EFUSE_Read(cvi_efuse_area[area].addr, buf, buf_size);

	memset(user, 0, user_size);

	p = user;
	for (i = 0; i < ARRAY_SIZE(cvi_efuse_user); i++) {
		ret = _CVI_EFUSE_Read(cvi_efuse_user[i].addr, p, cvi_efuse_user[i].size);
		if (ret < 0)
			return ret;
		p += cvi_efuse_user[i].size;
	}

	memcpy(buf, user, buf_size);

	return CVI_SUCCESS;
}

CVI_S32 CVI_EFUSE_Write(enum CVI_EFUSE_AREA_E area, const CVI_U8 *buf, CVI_U32 buf_size)
{
	CVI_U32 user_size = cvi_efuse_area[CVI_EFUSE_AREA_USER].size;
	CVI_U8 user[user_size], *p;
	CVI_S32 ret;
	int i;

	if (area >= ARRAY_SIZE(cvi_efuse_area) || cvi_efuse_area[area].size == 0) {
		_cc_error("area (%d) is not found\n", area);
		return CVI_ERR_EFUSE_INVALID_AREA;
	}
	if (!buf)
		return CVI_ERR_EFUSE_INVALID_PARA;

	if (buf_size > cvi_efuse_area[area].size)
		buf_size = cvi_efuse_area[area].size;

	if (area != CVI_EFUSE_AREA_USER)
		return _CVI_EFUSE_Write(cvi_efuse_area[area].addr, buf, buf_size);

	memset(user, 0, user_size);
	memcpy(user, buf, buf_size);

	p = user;
	for (i = 0; i < ARRAY_SIZE(cvi_efuse_user); i++) {
		ret = _CVI_EFUSE_Write(cvi_efuse_user[i].addr, p, cvi_efuse_user[i].size);
		if (ret < 0)
			return ret;
		p += cvi_efuse_user[i].size;
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_EFUSE_EnableSecureBoot(uint32_t sel)
{
	CVI_U32 value = 0;

	value |= 0x3 << CVI_EFUSE_TEE_SCS_ENABLE_SHIFT;
	value |= 0x4 << CVI_EFUSE_ROOT_PUBLIC_KEY_SELECTION_SHIFT;

	if (sel != 1) {
		value |= 0x3 << CVI_EFUSE_BOOT_LOADER_ENCRYPTION;
		value |= 0x4 << CVI_EFUSE_LDR_KEY_SELECTION_SHIFT;
	}

	return _CVI_EFUSE_Write(CVI_EFUSE_SECURE_CONF_ADDR, &value, sizeof(value));
}

CVI_S32 CVI_EFUSE_IsSecureBootEnabled(void)
{
	CVI_U32 value = 0;
	CVI_S32 ret = 0;

	ret = _CVI_EFUSE_Read(CVI_EFUSE_SECURE_CONF_ADDR, &value, sizeof(value));
	_cc_trace("ret=%d value=%u\n", ret, value);
	if (ret < 0)
		return ret;

	ret = (value & (0x3 << CVI_EFUSE_TEE_SCS_ENABLE_SHIFT)) >> CVI_EFUSE_TEE_SCS_ENABLE_SHIFT;
	if (ret == 0) {
		printf("Secure Boot is disable\n");
		return 0;
	}

	ret = (value & (0x7 << CVI_EFUSE_ROOT_PUBLIC_KEY_SELECTION_SHIFT)) >> CVI_EFUSE_ROOT_PUBLIC_KEY_SELECTION_SHIFT;
	switch (ret) {
	case 0:
		printf("Secure Boot sign is enable, verity with rot_pk_a_hash\n");
		break;
	case 1:
		printf("Secure Boot sign is enable, verity with rot_pk_b_hash\n");
		break;
	case 2:
		printf("Secure Boot sign is enable, verity with rot_pk_c_hash\n");
		break;
	default:
		printf("Secure Boot sign is enable, verity with efuse KPUB HASH\n");
		break;
	}

	ret = (value & (0x3 << CVI_EFUSE_BOOT_LOADER_ENCRYPTION)) >> CVI_EFUSE_BOOT_LOADER_ENCRYPTION;
	if (ret == 0)
		return 0;

	ret = (value & (0x7 << CVI_EFUSE_LDR_KEY_SELECTION_SHIFT)) >> CVI_EFUSE_LDR_KEY_SELECTION_SHIFT;
	switch (ret) {
	case 0:
		printf("Secure Boot encryption is enable, decrypt with ldr_ek_a\n");
		break;
	case 1:
		printf("Secure Boot encryption is enable, decrypt with ldr_ek_b\n");
		break;
	case 2:
		printf("Secure Boot encryption is enable, decrypt with ldr_ek_c\n");
		break;
	default:
		printf("Secure Boot encryption is enable, decrypt with efuse LDR DES KEY\n");
		break;
	}

	return 0;
}

CVI_S32 CVI_EFUSE_Lock(enum CVI_EFUSE_LOCK_E lock)
{
	CVI_U32 value = 0;
	CVI_U32 ret = 0;

	if (lock >= ARRAY_SIZE(cvi_efuse_lock)) {
		_cc_error("lock (%d) is not found\n", lock);
		return CVI_ERR_EFUSE_INVALID_AREA;
	}

	value = 0x3 << cvi_efuse_lock[lock].wlock_shift;
	ret = _CVI_EFUSE_Write(CVI_EFUSE_LOCK_ADDR, &value, sizeof(value));
	if (ret < 0)
		return ret;

	if (cvi_efuse_lock[lock].rlock_shift >= 0) {
		value = 0x3 << cvi_efuse_lock[lock].rlock_shift;
		ret = _CVI_EFUSE_Write(CVI_EFUSE_LOCK_ADDR, &value, sizeof(value));
	}

	return ret;
}

CVI_S32 CVI_EFUSE_IsLocked(enum CVI_EFUSE_LOCK_E lock)
{
	CVI_S32 ret = 0;
	CVI_U32 value = 0;

	if (lock >= ARRAY_SIZE(cvi_efuse_lock)) {
		_cc_error("lock (%d) is not found\n", lock);
		return CVI_ERR_EFUSE_INVALID_AREA;
	}

	ret = _CVI_EFUSE_Read(CVI_EFUSE_LOCK_ADDR, &value, sizeof(value));
	_cc_trace("ret=%d value=%u\n", ret, value);
	if (ret < 0)
		return ret;

	value &= 0x3 << cvi_efuse_lock[lock].wlock_shift;
	return !!value;
}

CVI_S32 CVI_EFUSE_LockWrite(enum CVI_EFUSE_LOCK_E lock)
{
	CVI_U32 value = 0;
	CVI_S32 ret = 0;

	if (lock >= ARRAY_SIZE(cvi_efuse_lock)) {
		_cc_error("lock (%d) is not found\n", lock);
		return CVI_ERR_EFUSE_INVALID_AREA;
	}

	value = 0x3 << cvi_efuse_lock[lock].wlock_shift;
	ret = _CVI_EFUSE_Write(CVI_EFUSE_LOCK_ADDR, &value, sizeof(value));
	return ret;
}

CVI_S32 CVI_EFUSE_IsWriteLocked(enum CVI_EFUSE_LOCK_E lock)
{
	CVI_S32 ret = 0;
	CVI_U32 value = 0;

	if (lock >= ARRAY_SIZE(cvi_efuse_lock)) {
		_cc_error("lock (%d) is not found\n", lock);
		return CVI_ERR_EFUSE_INVALID_AREA;
	}

	ret = _CVI_EFUSE_Read(CVI_EFUSE_LOCK_ADDR, &value, sizeof(value));
	_cc_trace("ret=%d value=%u\n", ret, value);
	if (ret < 0)
		return ret;

	value &= 0x3 << cvi_efuse_lock[lock].wlock_shift;
	return !!value;
}

static const char *const efuse_index[] = {
	[CVI_EFUSE_AREA_USER] = "USER",
	[CVI_EFUSE_AREA_DEVICE_ID] = "DEVICE_ID",
	[CVI_EFUSE_AREA_HASH0_PUBLIC] = "HASH0_PUBLIC",
	[CVI_EFUSE_AREA_LOADER_EK] = "LOADER_EK",
	[CVI_EFUSE_AREA_DEVICE_EK] = "DEVICE_EK",
	[CVI_EFUSE_LOCK_HASH0_PUBLIC] = "LOCK_HASH0_PUBLIC",
	[CVI_EFUSE_LOCK_LOADER_EK] = "LOCK_LOADER_EK",
	[CVI_EFUSE_LOCK_DEVICE_EK] = "LOCK_DEVICE_EK",
	[CVI_EFUSE_LOCK_WRITE_HASH0_PUBLIC] = "LOCK_WRITE_HASH0_PUBLIC",
	[CVI_EFUSE_LOCK_WRITE_LOADER_EK] = "LOCK_WRITE_LOADER_EK",
	[CVI_EFUSE_LOCK_WRITE_DEVICE_EK] = "LOCK_WRITE_DEVICE_EK",
	[CVI_EFUSE_SECUREBOOT] = "SECUREBOOT",
};

static int find_efuse_by_name(const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(efuse_index); i++) {
		if (!efuse_index[i])
			continue;

		if (!strcmp(name, efuse_index[i]))
			return i;
	}

	return -1;
}

static int do_efuser(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int idx;
	int ret;
	unsigned int size;
	unsigned char buf[128];

	if (argc != 2)
		return CMD_RET_USAGE;

	_cc_trace("Read eFuse: %s\n", argv[1]);
	idx = find_efuse_by_name(argv[1]);
	if (idx < 0)
		return CMD_RET_USAGE;

	_cc_trace("idx=%d %s\n", idx, efuse_index[idx]);

	if (idx < CVI_EFUSE_AREA_LAST) {
		if (CVI_EFUSE_GetSize(idx, &size) < 0)
			return CMD_RET_FAILURE;

		_cc_trace("size=%d\n", size);

		if (CVI_EFUSE_Read(idx, buf, size) < 0)
			return CMD_RET_FAILURE;

		print_buffer(0, buf, 1, size, 0);
		return 0;
	} else if (idx < CVI_EFUSE_LOCK_LAST) {
		ret = CVI_EFUSE_IsLocked(idx);
		printf("%s is %s locked\n", efuse_index[idx], ret ? "" : "not");
		return 0;
	} else if (idx == CVI_EFUSE_SECUREBOOT) {
		ret = CVI_EFUSE_IsSecureBootEnabled();
		return 0;
	} else if (idx < CVI_EFUSE_LOCK_WRITE_LAST) {
		ret = CVI_EFUSE_IsWriteLocked(idx);
		printf("%s is %s write_locked\n", efuse_index[idx], ret ? "" : "not");
		return 0;
	}

	return CMD_RET_FAILURE;
}

static int do_efusew(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int idx;
	int ret;
	unsigned int size;
	unsigned char buf[128] = { 0 };

	_cc_trace("argc=%d\n", argc);

	if (argc != 2 && argc != 3)
		return CMD_RET_USAGE;

	_cc_trace("Write eFuse: %s=%s\n", argv[1], argv[2]);
	idx = find_efuse_by_name(argv[1]);
	if (idx < 0)
		return CMD_RET_USAGE;

	_cc_trace("idx=%d %s\n", idx, efuse_index[idx]);
	printf("Write eFuse %s(%d) with:\n", efuse_index[idx], idx);

	if (idx < CVI_EFUSE_AREA_LAST) {
		if (argc != 3)
			return CMD_RET_USAGE;

		size = hex2bytes(argv[2], buf, sizeof(buf));
		if (size <= 0)
			return CMD_RET_USAGE;

		print_buffer(0, buf, 1, size, 0);

		if (CVI_EFUSE_GetSize(idx, &size) < 0)
			return CMD_RET_FAILURE;

		_cc_trace("size=%d\n", size);

		ret = CVI_EFUSE_Write(idx, buf, size);
		if (ret < 0) {
			printf("Failed to write %s\n", efuse_index[idx]);
			return CMD_RET_FAILURE;
		}

		return 0;

	} else if (idx < CVI_EFUSE_LOCK_LAST) {
		if (CVI_EFUSE_Lock(idx) < 0) {
			printf("Failed to lock %s\n", efuse_index[idx]);
			return CMD_RET_FAILURE;
		}

		printf("%s is locked\n", efuse_index[idx]);
		return 0;
	} else if (idx == CVI_EFUSE_SECUREBOOT) {
		if (argc != 3)
			return CMD_RET_USAGE;

		uint32_t sel = simple_strtoul(argv[2], NULL, 0);

		ret = CVI_EFUSE_EnableSecureBoot(sel);
		printf("Enabled Secure Boot is %s\n", ret >= 0 ? "success" : "failed");
		return 0;
	} else if (idx < CVI_EFUSE_LOCK_WRITE_LAST) {
		if (CVI_EFUSE_LockWrite(idx) < 0) {
			printf("Failed to lock write %s\n", efuse_index[idx]);
			return CMD_RET_FAILURE;
		}

		printf("%s is locked\n", efuse_index[idx]);
		return 0;
	}

	return CMD_RET_FAILURE;
}

static int do_efusew_word(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t addr, value;
	int ret = -1;

	if (argc != 3)
		return CMD_RET_USAGE;

	addr = simple_strtoul(argv[1], NULL, 0);
	value = simple_strtoul(argv[2], NULL, 0);

	printf("Write eFuse: 0x%04x=0x%08x\n", addr, value);

	ret = cvi_efuse_write(addr, value);

	if (ret < 0) {
		printf("ERROR: ret=%d\n", ret);
		return CMD_RET_FAILURE;
	}

	return 0;
}

static int do_efuser_dump(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int i;
	uint32_t buf[EFUSE_SIZE / sizeof(uint32_t)];

	for (i = 0; i < ARRAY_SIZE(buf); i++)
		buf[i] = cvi_efuse_read_from_shadow(i * sizeof(uint32_t));

	print_buffer(0, buf, 1, sizeof(buf), 16);

	return 0;
}

U_BOOT_CMD(efuser, 9, 1, do_efuser, "Read efuse",
	   "[args..]\n"
	   "    - args ...");

U_BOOT_CMD(efusew, 9, 1, do_efusew, "Write efuse",
	   "[args..]\n"
	   "    - args ...");

U_BOOT_CMD(efusew_word, 9, 1, do_efusew_word, "Write word to efuse",
	   "efusew_word addr value\n"
	   "    - args ...");

U_BOOT_CMD(efuser_dump, 9, 1, do_efuser_dump, "Read/Dump efuse",
	   "do_efuser_dump\n"
	   "    - args ...");

#endif
