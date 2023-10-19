#include "linux/property.h"
#include "linux/types.h"
#include "linux/of.h"

int device_property_read_u32_array(struct device *dev, const char *propname,
	u32 *val, size_t nval)
{
	int i;
	struct property *dvp;

	dvp = dev->of_node->properties;
	if (!dev)
		return -1;
	for (i = 0; i < 16, dvp; i++) {
		if (!strcmp(dvp->name, propname)) {
			*val = dvp->value;
			return 0;
		}
		dvp = dvp->next;
	}
	return -1;
}
