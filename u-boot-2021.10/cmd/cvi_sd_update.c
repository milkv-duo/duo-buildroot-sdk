#include <stdlib.h>
#include <common.h>
#include <config.h>
#include <command.h>
#include <cvsnfc.h>

#define		BIT_WRITE_FIP_BIN	BIT(0)
#define		BIT_WRITE_ROM_PATCH	BIT(1)
#define		BIT_WRITE_BLD		BIT(2)

#define		COMPARE_STRING_LEN	6

//------------------------------------------------------------------------------
//  data type definitions: typedef, struct or class
//------------------------------------------------------------------------------
#define PTR_INC(base, offset) (void *)((uint8_t *)(base) + (offset))

static int do_cvi_sd_update(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	char *addr;
	uint32_t component = 0;

	if (argc != 4) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	addr = (char *)simple_strtol(argv[1], NULL, 16);

	if (!addr) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	printf("addr %p\n", addr);

	if (!strncmp(argv[3], "fip", COMPARE_STRING_LEN)) {
		printf("fip.bin\n");
		component |= BIT_WRITE_FIP_BIN;

	} else if (!strncmp(argv[3], "patch", COMPARE_STRING_LEN)) {
		printf("patch\n");
		component |= BIT_WRITE_ROM_PATCH;
	} else if (!strncmp(argv[3], "all", COMPARE_STRING_LEN)) {
		printf("all\n");
		component |= BIT_WRITE_FIP_BIN | BIT_WRITE_ROM_PATCH | BIT_WRITE_BLD;
	} else {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (!strncmp(argv[2], "spinand", COMPARE_STRING_LEN)) {
		do_cvi_update_spinand(component, addr);
	} else {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	return 0;
}

U_BOOT_CMD(cvi_sd_update, 4, 0, do_cvi_sd_update,
	   "cvi_sd_update - write images to SPI NAND\n",
	   "cvi_sd_update addr dev_type img_type  - Print a report\n"
	   "addr     : data memory address\n"
	   "dev_type : spinand\n"
	   "img_type : fip/patch/all\n"
);
