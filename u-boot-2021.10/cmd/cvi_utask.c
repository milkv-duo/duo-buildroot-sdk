#include <stdlib.h>
#include <common.h>
#include <command.h>
#include <cvi_utask.h>

__weak int cvi_usb_polling(void)
{
	return 0;
}

__weak void acm_patch_id(unsigned short vid, unsigned short pid)
{
}

static int do_cvi_utask(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	printf("\nstart usb task!\n");

	if (argc == 3 && strncmp(argv[1], "pid", 3) == 0) {
		int pid = (int)simple_strtoul(argv[2], NULL, 10);

		acm_patch_id(0, pid);
	} else if (argc == 5 && (strncmp(argv[1], "vid", 3) == 0) && (strncmp(argv[3], "pid", 3) == 0)) {
		int vid = (int)simple_strtoul(argv[2], NULL, 10);
		int pid = (int)simple_strtoul(argv[4], NULL, 10);

		acm_patch_id(vid, pid);
	}

	cvi_usb_polling();

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(cvi_utask, 5, 0, do_cvi_utask,
	   "bootloader control block command",
	   "cvi_bcb <interface> <dev> <varname>\n"
);

