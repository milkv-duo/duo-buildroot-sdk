#include <stddef.h>
#include <stdint.h>

#pragma GCC push_options
#pragma GCC target("+nothing+crc")

#if !defined(__ARM_FEATURE_CRC32)
#error "__ARM_FEATURE_CRC32"
#endif

static inline uint32_t __extension__ __attribute__((__always_inline__)) __crc32b(uint32_t __a, uint8_t __b)
{
	return __builtin_aarch64_crc32b(__a, __b);
}

static inline uint32_t __extension__ __attribute__((__always_inline__)) __crc32d(uint32_t __a, uint64_t __b)
{
	return __builtin_aarch64_crc32x(__a, __b);
}

/**
 * crc-32
 * Width	32
 * Poly	0x04c11db7
 * Reflect In	True
 * XOR In	0xffffffff
 * Reflect Out	True
 * XOR Out	0xffffffff
 * Short command	pycrc.py --model crc-32
 * Extended command	pycrc.py --width 32 --poly 0x04c11db7
 *		--reflect-in True --xor-in 0xffffffff --reflect-out True --xor-out 0xffffffff
 * Check	0xcbf43926
 */
uint32_t armv8_crc32_little(unsigned long crc, const unsigned char *buf, size_t len)
{
	uint32_t c = (uint32_t)~crc;

	while (len && ((uintptr_t)buf & 7)) {
		c = __crc32b(c, *buf++);
		--len;
	}

	const uint64_t *buf8 = (const uint64_t *)buf;

	while (len >= 64) {
		c = __crc32d(c, *buf8++);
		c = __crc32d(c, *buf8++);
		c = __crc32d(c, *buf8++);
		c = __crc32d(c, *buf8++);

		c = __crc32d(c, *buf8++);
		c = __crc32d(c, *buf8++);
		c = __crc32d(c, *buf8++);
		c = __crc32d(c, *buf8++);
		len -= 64;
	}

	while (len >= 8) {
		c = __crc32d(c, *buf8++);
		len -= 8;
	}

	buf = (const unsigned char *)buf8;

	while (len--) {
		c = __crc32b(c, *buf++);
	}

	return ~c;
}
#pragma GCC pop_options