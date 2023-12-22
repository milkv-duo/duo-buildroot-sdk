// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2023 bitmain, Inc
 *
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <sound.h>
#include <mapmem.h>
#include <dm/uclass.h>
#include <dm/uclass.h>
#include "../drivers/cvi_sound/cvi-src.h"

static int do_play(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int ret = 0;
	struct udevice *dev;
	const struct sound_ops *ops;

	printf("enter cviplay\n");
	ret = uclass_get_device_by_driver(UCLASS_SOUND, DM_DRIVER_GET(cvitekub_sound), &dev);
	if (ret) {
		printf("[error][%s]no cvitekub_sound device ret:%d\n", __func__, ret);
		return -1;
	}
	ops = dev->driver->ops;
	if (!ops->setup || !ops->play || !ops->stop_play) {
		printf("[error][%s]setup:%p, play:%p, stop_play:%p\n",
			__func__, ops->setup, ops->play, ops->stop_play);
		return -1;
	}
	ops->setup(dev);
	ret = ops->play(dev, src_tx_data, SRC_BUFF_SIZE);
	//printf("UPDATE_ADDR:%ld\n", (uintptr_t)UPDATE_ADDR);
	//ret = ops->play(dev, (void *)(uintptr_t)UPDATE_ADDR, 726904);
	if (ret) {
		printf("[error][%s]play error:%d\n", __func__, ret);
		return -1;
	}

	ops->stop_play(dev);

	return 0;
}

U_BOOT_CMD(cviplay,   1,      1,      do_play,
	"perform cviplay",
	"play sound\n"
);
