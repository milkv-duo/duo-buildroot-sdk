#ifndef BYTEORDER_LENDIAN_H
#define BYTEORDER_LENDIAN_H

#include "swap.h"

#define cpuToLe32(x) (x)
#define le32ToCpu(x) (x)
#define cpuToLe16(x) ((uint16_t) (x))
#define le16ToCpu(x) ((uint16_t) (x))

#define cpuToBe32(x) ((uint32_t) swap32(x))
#define be32ToCpu(x) ((uint32_t) swap32(x))
#define cpuToBe16(x) ((uint16_t) swap16(x))
#define be16ToCpu(x) ((uint16_t) swap16(x))

/**
 * Macros used for reading 16-bits and 32-bits data from memory which
 * starting addres could be unaligned.
 */
#define ptrToWord(ptr) ((*(uint8_t *) ptr << 8) | (*(uint8_t *) (ptr + 1)))
#define ptrToDword(ptr) ((*(uint8_t *) ptr << 24) | ((*(uint8_t *) (ptr + 1)) << 16) | \
	(*((uint8_t *) (ptr + 2)) << 8) | (*((uint8_t *) (ptr + 3))))

#endif /* BYTEORDER_LENDIAN_H */

