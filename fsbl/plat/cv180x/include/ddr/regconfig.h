#ifndef __REG_CFG_H__
#define __REG_CFG_H__

// #include <stdint.h>
// #include "ddr_sys.h"

// #define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof((arr)[0]))

struct regconf {
	uint32_t addr;
	uint32_t val;
};

struct regpatch {
	uint32_t addr;
	uint32_t mask;
	uint32_t val;
};

// #define DDR_CFG_TYPE_DDR3	1
// #define DDR_CFG_TYPE_DDR2	2
// #define DDR_CFG_TYPE_DDR3_AUTO	3
// #define DDR_CFG_TYPE_DDR2_AUTO	4

extern struct regpatch ddr_patch_regs[];
extern uint32_t ddr_patch_regs_count;

#endif /* __REG_CFG_H__ */
