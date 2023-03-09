#include <stdlib.h>
#include <common.h>
#include <command.h>

extern int jpeg_decoder(void *bs_addr, void *yuv_addr, int size);
extern int get_jpeg_size(int *width_addr, int *height_addr);

static int do_cvi_jpeg_dec(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	char *bs_addr = NULL;
	char *yuv_addr = NULL;
	int size = 0;


	if (argc != 4) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	bs_addr = (char *)simple_strtol(argv[1], NULL, 16);

	if (!bs_addr) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	yuv_addr = (char *)simple_strtol(argv[2], NULL, 16);

	if (!yuv_addr) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	size = (int)simple_strtol(argv[3], NULL, 16);

	if (!size) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	printf("\nstart jpeg dec task!, bs_addr %p, yuv_addr %p, size %d\n", bs_addr, yuv_addr, size);

	jpeg_decoder(bs_addr, yuv_addr, size);
	get_jpeg_size((int *)(bs_addr + size - 8), (int *)(bs_addr + size - 4));

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(cvi_jpeg_dec, 4, 0, do_cvi_jpeg_dec, "Jpeg decoder ", "\n"
);
