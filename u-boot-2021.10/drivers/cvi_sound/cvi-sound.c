// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2023 bitmain
 */
#include <common.h>
#include <linux/types.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <audio_codec.h>
#include <sound.h>
#include <dm.h>
#include <i2s.h>
#include <log.h>
#include <sound.h>

static int cvitekub_sound_setup(struct udevice *dev)
{
	struct sound_uc_priv *uc_priv = dev_get_uclass_priv(dev);
	struct i2s_uc_priv *i2c_priv = dev_get_uclass_priv(uc_priv->i2s);
	int ret = 0;

	if (uc_priv->setup_done) {
		printf("areadly init\n");
		return ret;
	}

	ret = audio_codec_set_params(uc_priv->codec, i2c_priv->id,
					 i2c_priv->samplingrate,
					 i2c_priv->samplingrate * i2c_priv->rfs,
					 i2c_priv->bitspersample,
					 i2c_priv->channels);
	if (ret)
		return ret;
	uc_priv->setup_done = true;

	return 0;
}

static int cvitekub_sound_play(struct udevice *dev, void *data, uint data_size)
{
	struct sound_uc_priv *uc_priv = dev_get_uclass_priv(dev);

	return i2s_tx_data(uc_priv->i2s, data, data_size);
}

static int cvitekub_sound_stop_play(struct udevice *dev)
{
	struct sound_uc_priv *uc_priv = dev_get_uclass_priv(dev);

	audio_codec_close(uc_priv->codec); //because of pop
	uc_priv->setup_done = false;
	return 0;
}

static int cvitekub_sound_probe(struct udevice *dev)
{
	printf("cvitekub_sound_probe\n");
	int ret = 0;
	struct sound_uc_priv *uc_priv = dev_get_uclass_priv(dev);

	ret = uclass_get_device_by_driver(UCLASS_AUDIO_CODEC, DM_DRIVER_GET(cvitekub_dac),
				  &uc_priv->codec);
	if (ret) {
		printf("[error][%s]no cvitekub_dac device ret:%d\n", __func__, ret);
		return -1;
	}

	ret = uclass_get_device_by_name(UCLASS_I2S, "i2s@04130000",
				  &uc_priv->i2s);
	if (ret) {
		printf("[error][%s]no cvitekub_i2s device ret:%d\n", __func__, ret);
		return -1;
	}

	printf("Probed sound '%s' with codec '%s' and i2s '%s'\n", dev->name,
		  uc_priv->codec->name, uc_priv->i2s->name);
	return 0;
}

static const struct sound_ops cvitekub_sound_ops = {
	.setup	= cvitekub_sound_setup,
	.play	= cvitekub_sound_play,
	.stop_play = cvitekub_sound_stop_play,
};

static const struct udevice_id cvitekub_sound_ids[] = {
	{ .compatible = "cvitek,cv182xa-dac" },
	{ }
};

U_BOOT_DRIVER(cvitekub_sound) = {
	.name		= "cvitekub_sound",
	.id		= UCLASS_SOUND,
	.of_match	= cvitekub_sound_ids,
	.probe		= cvitekub_sound_probe,
	.ops		= &cvitekub_sound_ops,
};
