#include <dm/uclass.h>
#include <sound.h>

UCLASS_DRIVER(sound) = {
	.id		= UCLASS_SOUND,
	.name		= "sound",
	.per_device_auto	= sizeof(struct sound_uc_priv),
};
