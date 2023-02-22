#include <string.h>

void *memcpy16(void *dest, const void *src, size_t length)
{
	return memcpy(dest, src, length);
}
void zeromem(void *dst, size_t size)
{
	memset(dst, 0, size);
}

