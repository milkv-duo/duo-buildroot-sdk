#include "linux/device.h"
#include "linux/types.h"

inline int device_property_read_u32(struct device *dev,
			const char *propname, u32 *val)
{
	return device_property_read_u32_array(dev, propname, val, 1);
}

