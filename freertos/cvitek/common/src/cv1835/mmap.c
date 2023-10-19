#include "linux/mman.h"
#include "linux/errno.h"
//#include "FreeRTOS_POSIX/sys/types.h"

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	void *mapaddr;

	mapaddr = (void *) addr;
	if (addr != NULL)
		return -EINVAL;

	/* now va = pa, so just return offset */
	mapaddr = offset;
	return (void *) mapaddr;
}

int munmap(void *start, size_t length)
{
	return TRUE;
}
