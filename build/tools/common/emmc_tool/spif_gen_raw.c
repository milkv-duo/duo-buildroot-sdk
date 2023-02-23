#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

//#define HAS_KERNEL

#define PATCH_TABLE1_OFFSET (128 * 1024)
#define PATCH_TABLE2_OFFSET ((128 + 32) * 1024)
#define PATCH_TABLE3_OFFSET ((128 + 64) * 1024)
#define PATCH_TABLE4_OFFSET ((128 + 96) * 1024)
#define FIP_OFFSET (256 * 1024)
#ifdef HAS_KERNEL
#define ITB_OFFSET (2 * 1024 * 1024)
#define END_OFFSET (16 * 1024 * 1024)
#else
#define END_OFFSET ((256 + 640) * 1024)
#endif

int main(int argc, char const *argv[])
{
	int ret, fd, fd2, fd3, i;
	struct stat fd_statbuf;
	unsigned char raw_byte, *ptr;
	unsigned char zero_byte = 0;

#ifdef bm1684
	int patch_table1[] = { // PCIe ctrl, single card
	};

	int patch_table2[] = { // PCIe ctrl, multi card
	};

	int patch_table3[128] = { // PCIe PHY
		[0]  = 0x1b1b1b1b,
		[1]  = 0x5fa01034, [2]	= 4, [3]  = 0x01c201c2,
		[4]  = 0x5fa01434, [5]	= 4, [6]  = 0x01c201c2,
		[7]  = 0x5fa01030, [8]	= 4, [9]  = 0x150400c0,
		[10] = 0x5fa01430, [11] = 4, [12] = 0x150400c0,
		[13] = 0x5fa01014, [14] = 4, [15] = 0x00540054,
		[16] = 0x5fa01414, [17] = 4, [18] = 0x00540054,
		[19] = 0x5fa01000, [20] = 4, [21] = 0x00002460,
		[22] = 0x5fa00e4c, [23] = 4, [24] = 0x00000606,

		// udelay(100), SPI @ 10MHz, that's about 125bytes, so here we set 256bytes
		[25] = 0x10100000, [26] = 256, [27] = 0xA, [90] = 0xB,

		[91] = 0x5fa00e4c, [92] = 4, [93] = 0x00000202,

		// udelay(25), SPI @ 10MHz, that's about 32bytes, so here we set 64bytes
		[94] = 0x10100100, [95]= 64, [96] = 0xC, [111] = 0xD,

		[112] = 0x5fa00e4c, [113] = 4, [114] = 0x0,
		[115] = 0x1e1e1e1e,
	};

	printf("using bm1684 patch table\n");
#else
        int patch_table1[] = {
	};

        int patch_table2[] = {
	};

	int patch_table3[] = {
	};

	printf("using empty patch table\n");
#endif

	fd = open("./spi_flash.bin", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0) {
		printf("open file1 failed %d\n", errno);
		return errno;
	}
	fd2 = open("./bl1.bin", O_RDONLY, 0);
	if (fd2 < 0) {
		printf("open file2 failed %d\n", errno);
		return errno;
	}
	stat("./bl1.bin", &fd_statbuf);
	if (fd_statbuf.st_size > PATCH_TABLE1_OFFSET) {
		printf("bl1 too big %ld\n", fd_statbuf.st_size);
		return -1;
	}
	fd3 = open("./fip.bin", O_RDONLY, 0);
	if (fd3 < 0) {
		printf("open file3 failed %d\n", errno);
		return errno;
	}
#ifdef HAS_KERNEL
	int fd4 = open("./ramboot_mini.itb", O_RDONLY, 0);
	if (fd4 < 0) {
		printf("open file4 fail %d", errno);
		return errno;
	}
#endif
	// fill in bl1.bin
	printf("i=0x%x, bl1.bin 0x%lx\n", i, fd_statbuf.st_size);
	for (i = 0; i < fd_statbuf.st_size; i++) {
		read(fd2, &raw_byte, 1);
		write(fd, &raw_byte, 1);
	}
	for ( ; i < PATCH_TABLE1_OFFSET; i++) {
		write(fd, &zero_byte, 1);
	}
	// fill in patch table 1
	printf("i=0x%x, patch table 1\n", i);
	ptr = (char *)&patch_table1;
	for ( ; i < PATCH_TABLE1_OFFSET + sizeof(patch_table1); i++) {
		write(fd, ptr + i - PATCH_TABLE1_OFFSET, 1);
	}
	for ( ; i < PATCH_TABLE2_OFFSET; i++) {
		write(fd, &zero_byte, 1);
	}
	// fill in patch table 2
	printf("i=0x%x, patch table 2\n", i);
	ptr = (char *)&patch_table2;
	for ( ; i < PATCH_TABLE2_OFFSET + sizeof(patch_table2); i++) {
		write(fd, ptr + i - PATCH_TABLE2_OFFSET, 1);
	}
	for ( ; i < PATCH_TABLE3_OFFSET; i++) {
		write(fd, &zero_byte, 1);
	}
	// fill in patch table 3
	printf("i=0x%x, patch table 3\n", i);
	ptr = (char *)&patch_table3;
	for ( ; i < PATCH_TABLE3_OFFSET + sizeof(patch_table3); i++) {
		write(fd, ptr + i - PATCH_TABLE3_OFFSET, 1);
	}
	for ( ; i < PATCH_TABLE4_OFFSET; i++) {
		write(fd, &zero_byte, 1);
	}
	// fill in patch table 4
	printf("i=0x%x, patch table 4\n", i);
	for ( ; i < FIP_OFFSET; i++) {
		write(fd, &zero_byte, 1);
	}
	// fill in fip.bin
	stat("./fip.bin", &fd_statbuf);
	printf("i=0x%x, fip.bin 0x%lx\n", i, fd_statbuf.st_size);
	for ( ; i < FIP_OFFSET + fd_statbuf.st_size; i++) {
		read(fd3, &raw_byte, 1);
		write(fd, &raw_byte, 1);
	}
#ifdef HAS_KERNEL
	for ( ; i < ITB_OFFSET; i++) {
		write(fd, &zero_byte, 1);
	}
	// fill in ramboot_mini.itb
	stat("./ramboot_mini.itb", &fd_statbuf);
	printf("i=0x%x, ramboot_mini.itb 0x%lx\n", i, fd_statbuf.st_size);
	for ( ; i < ITB_OFFSET + fd_statbuf.st_size; i++) {
		read(fd4, &raw_byte, 1);
		write(fd, &raw_byte, 1);
	}
#endif
	for ( ; i < END_OFFSET; i++) {
		write(fd, &zero_byte, 1);
	}
	printf("i=0x%x\n", i);
out:
	close(fd);
	close(fd2);
	close(fd3);
	return 0;
}
