#include <linux/device.h>

struct of_device_id *of_match_device(const struct of_device_id *matches,
                       const struct device *dev)
{
    if ((!matches) || (!dev))
        return 0;
	struct of_device_id *find_match = 0;
    for (; matches->compatible[0]; matches++) {
			if (!strcmp(matches->compatible, dev->init_name)) {
				find_match = matches;
				break;
			}
    }
    return find_match;
}

inline struct device *get_device(struct device *dev)
{
    return dev;
}

inline void put_device(struct device *dev)
{

}
