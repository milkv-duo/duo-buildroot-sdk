#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>

uint32_t crc32(uint32_t crc, const uint8_t *buf, uint32_t len);

#endif  /* _CRC_H_ */

