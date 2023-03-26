/*
 * Copyright (c) 2019, Cvitek. All rights reserved.
 *
 */
#ifndef __CV_EFUSE_H
#define __CV_EFUSE_H

#define SEC_EFUSE_BASE (SEC_SUBSYS_BASE + 0x000C0000)
#define SEC_EFUSE_SHADOW_REG (SEC_EFUSE_BASE + 0x100)
#define EFUSE_SIZE      0x100
#define EFUSE_STATUS    0x010
#define EFUSE_MODE      0x000
#define EFUSE_ADR       0x004
#define EFUSE_RD_DATA   0x00c

#define EFUSE_LDR_DES_KEY_REG (SEC_EFUSE_SHADOW_REG + 0xD8)

#define EFUSE_KPUB_HASH_REG (SEC_EFUSE_SHADOW_REG + 0xA8)

#define EFUSE_SCS_CONFIG_REG (SEC_EFUSE_SHADOW_REG + 0xA0)
#define BIT_SCS_ENABLE 0
#define BIT_TEE_SCS_ENABLE 2
#define BIT_BOOT_LOADER_ENCRYPTION 6

#define EFUSE_W_LOCK0_REG (EFUSE_BASE + 0x198)
#define BIT_FTSN3_LOCK 3
#define BIT_FTSN4_LOCK 4

int efuse_power_on(void);
int efuse_program_bit(uint32_t addr, const uint32_t bit);
int efuse_refresh_shadow(void);
int efuse_power_off(void);

#endif /*__CV_EFUSE_H*/
