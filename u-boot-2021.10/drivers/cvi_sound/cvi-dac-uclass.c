#define LOG_CATEGORY UCLASS_AUDIO_CODEC

#include <common.h>
#include <dm.h>
#include "cvi-dac.h"
#include <audio_codec.h>
#include <dm/uclass.h>

int audio_codec_set_params(struct udevice *dev, int interface, int rate,
			   int mclk_freq, int bits_per_sample, uint channels)
{
	struct audio_codec_ops *ops = audio_codec_get_ops(dev);

	if (!ops->set_params)
		return -ENOSYS;

	return ops->set_params(dev, interface, rate, mclk_freq, bits_per_sample,
				   channels);
}

int audio_codec_close(struct udevice *dev)
{
	struct audio_codec_ops *ops = audio_codec_get_ops(dev);

	if (!ops->codec_close)
		return -ENOSYS;

	return ops->codec_close(dev);
}

UCLASS_DRIVER(audio_codec) = {
	.id		= UCLASS_AUDIO_CODEC,
	.name		= "audio-codec",
};
