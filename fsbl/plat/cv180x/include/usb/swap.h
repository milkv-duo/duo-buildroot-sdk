#ifndef BYTEORDER_SWAB_H
#define BYTEORDER_SWAB_H

#define swap16(x) \
	((uint16_t)((((uint16_t) (x) & (uint16_t) 0x00ffU) << 8) | (((uint16_t) (x) & (uint16_t) 0xff00U) >> 8)))

#define swap32(x) (((uint32_t) ( \
	(((uint32_t) (x) & (uint32_t) 0x000000ffUL) << 24) | \
	(((uint32_t) (x) & (uint32_t) 0x0000ff00UL) <<  8) | \
	(((uint32_t) (x) & (uint32_t) 0x00ff0000UL) >>  8) | \
	(((uint32_t) (x) & (uint32_t) 0xff000000UL) >> 24))))

#endif /* BYTEORDER_SWAB_H */
