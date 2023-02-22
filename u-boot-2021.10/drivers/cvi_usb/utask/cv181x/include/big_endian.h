#ifndef BYTEORDER_BENDIAN_H
#define BYTEORDER_BENDIAN_H

#include "swap.h"

#define cpuToLe32(x) ((uint32_t)swap32(x))
#define le32ToCpu(x) ((uint32_t)swap32(x))
#define cpuToLe16(x) ((uint16_t)swap16(x))
#define le16ToCpu(x) ((uint16_t)swap16(x))

#define cpuToBe32(x) (x)
#define be32ToCpu(x) (x)
#define cpuToBe16(x) (x)
#define be16ToCpu(x) (x)

/**
 * Macros used for reading 16-bits and 32-bits data from memory which
 * starting address could be unaligned.
 */
#define ptrToWord(ptr) ((*(uint8_t *)ptr) | (*((uint8_t *)(ptr + 1)) << 8))
#define ptrToDword(ptr) ((*(uint8_t *)ptr) | (*((uint8_t *)(ptr + 1)) << 8) | \
	(*((uint8_t *)(ptr + 2)) << 16) | (*((uint8_t *)(ptr + 3)) << 24))

#endif /* BYTEORDER_BENDIAN_H */
