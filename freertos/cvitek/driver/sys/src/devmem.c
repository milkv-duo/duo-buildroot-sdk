/*
 ** read/write phy addr in userspace
 ** open /dev/mem
 ** taiqiang.cao@bitmain.com
 */
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/mman.h>
#include "xil_types.h"
#include <linux/mman.h>
//#include <unistd.h>
#include <errno.h>
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/errno.h"
//#include <termios.h>
#include <linux/termios.h>
//#include <signal.h>
#include "FreeRTOS_POSIX/signal.h"
//#include <fcntl.h>
#include <linux/fcntl.h>
//#include <ctype.h>
//#include <inttypes.h>
#include "linux/confname.h"

#include "devmem.h"

int devm_open(void)
{
	int fd;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1) {
		printf("cannot open '/dev/mem'\n");
		goto open_err;
	}

	return fd;
open_err:
	return -1;
}

int devm_open_cached(void)
{
	int fd;

	fd = open("/dev/mem", O_RDWR);
	if (fd == -1) {
		printf("cannot open '/dev/mem'\n");
		goto open_err;
	}

	return fd;
open_err:
	return -1;
}

void devm_close(int fd)
{
	if (fd)
		close(fd);
}

void *devm_map(int fd, uint64_t phy_addr, size_t len)
{
	off_t offset;
	void *map_base;
	uint32_t padding;

	offset = phy_addr & ~(sysconf(_SC_PAGE_SIZE) - 1);
	padding = phy_addr - offset;

	map_base = mmap(NULL, len + padding, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, offset);

	if (map_base == MAP_FAILED) {
		perror("mmap failed\n");
		//poshiun
//		printf("phy_addr = %#"PRIx64", length = %zu\t, mapped memory length = %zu\t, pa_offset = %#"PRIx64"\n"
//			, phy_addr, len, len + padding, offset);
		printf("phy_addr = %#lx, length = %zu\t, mapped memory length = %zu\t, pa_offset = %lx\n"
			, phy_addr, len, len + padding, offset);
		goto mmap_err;
	}

	return map_base + padding;

mmap_err:
	return NULL;
}

void devm_unmap(void *virt_addr, size_t len)
{
	uint64_t addr;

	/* page align */
	addr = (((uint64_t)(uintptr_t)virt_addr) & ~(sysconf(_SC_PAGE_SIZE) - 1));

	munmap((void *)(uintptr_t)addr, len + (unsigned long) virt_addr - addr);
}
