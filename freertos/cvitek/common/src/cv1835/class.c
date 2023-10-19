#include "malloc.h"
#include "linux/device.h"
#include "linux/errno.h"
#include "linux/err.h"
static void class_create_release(struct class *cls)
{
	//printf("%s called for %s\n", __func__, cls->name);
	kfree(cls);
}

struct class *__class_create(struct module *owner, const char *name)
{
	struct class *cls;
	int retval;

	cls = kzalloc(sizeof(*cls), GFP_KERNEL);
	if (!cls) {
		retval = -ENOMEM;
		goto error;
	}

	cls->name = name;
	cls->owner = owner;
	cls->class_release = class_create_release;

	//retval = __class_register(cls, key);
	//if (retval)
	//    goto error;

	return cls;

error:
	kfree(cls);
	return ERR_PTR(retval);
}

void class_destroy(struct class *cls)
{
	if ((cls == NULL) || (IS_ERR(cls)))
		return;
	class_create_release(cls);
//    class_unregister(cls);
}


