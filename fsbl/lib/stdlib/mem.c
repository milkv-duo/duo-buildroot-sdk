/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h> /* size_t */
#include <limits.h>
#include <stdint.h>
#include <string.h>

/*
 * Compare @len bytes of @s1 and @s2
 */
int memcmp(const void *s1, const void *s2, size_t len)
{
	const unsigned char *s = s1;
	const unsigned char *d = s2;
	unsigned char sc;
	unsigned char dc;

	while (len--) {
		sc = *s++;
		dc = *d++;
		if (sc - dc)
			return (sc - dc);
	}

	return 0;
}

/*
 * Move @len bytes from @src to @dst
 */
void *memmove(void *dst, const void *src, size_t len)
{
	/*
	 * The following test makes use of unsigned arithmetic overflow to
	 * more efficiently test the condition !(src <= dst && dst < str+len).
	 * It also avoids the situation where the more explicit test would give
	 * incorrect results were the calculation str+len to overflow (though
	 * that issue is probably moot as such usage is probably undefined
	 * behaviour and a bug anyway.
	 */
	if ((size_t)dst - (size_t)src >= len) {
		/* destination not in source data, so can safely use memcpy */
		return memcpy(dst, src, len);
	} else {
		/* copy backwards... */
		const char *end = dst;
		const char *s = (const char *)src + len;
		char *d = (char *)dst + len;
		while (d != end)
			*--d = *--s;
	}
	return dst;
}

/*
 * Scan @len bytes of @src for value @c
 */
void *memchr(const void *src, int c, size_t len)
{
	const char *s = src;

	while (len--) {
		if (*s == c)
			return (void *) s;
		s++;
	}

	return NULL;
}

void *_memcpy(void *dst, const void *src, size_t len)
{
	const char *s = src;
	char *d = dst;

	while (len--)
		*d++ = *s++;

	return dst;
}

#define SS (sizeof(size_t))
#define ALIGN (sizeof(size_t)-1)
#define ONES ((size_t)-1/UCHAR_MAX)

/*
 * Fill @count bytes of memory pointed to by @dst with @val
 */
void *memset(void *dest, int c, size_t n)
{
	unsigned char *s = dest;

	c = (unsigned char)c;

	for (; ((uintptr_t)s & ALIGN) && n; n--)
		*s++ = c;

	if (n) {
		size_t *w, k = ONES * c;

		for (w = (void *)s; n >= SS; n -= SS, w++)
			*w = k;
		for (s = (void *)w; n; n--, s++)
			*s = c;
	}

	return dest;
}

/*
 * Copy @len bytes from @src to @dst
 */
void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

	if (((uintptr_t)d & ALIGN) != ((uintptr_t)s & ALIGN))
		goto misaligned;

	for (; ((uintptr_t)d & ALIGN) && n; n--)
		*d++ = *s++;
	if (n) {
		size_t *wd = (void *)d;
		const size_t *ws = (const void *)s;

		for (; n >= SS; n -= SS)
			*wd++ = *ws++;
		d = (void *)wd;
		s = (const void *)ws;
misaligned:
		for (; n; n--)
			*d++ = *s++;
	}
	return dest;
}
