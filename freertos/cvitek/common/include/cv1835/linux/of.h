#ifndef _LINUX_OF_H
#define _LINUX_OF_H
/*
 * Definitions for talking to the Open Firmware PROM on
 * Power Macintosh and other computers.
 *
 * Copyright (C) 1996-2005 Paul Mackerras.
 *
 * Updates for PPC64 by Peter Bergner & David Engebretsen, IBM Corp.
 * Updates for SPARC64 by David S. Miller
 * Derived from PowerPC and Sparc prom.h files by Stephen Rothwell, IBM Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/types.h>

struct property {
	char *name;
	int length;
	void *value;
	struct property *next;
	//    unsigned long _flags;
	unsigned int unique_id;
	//    struct bin_attribute attr;
};

struct device_node {
	const char *name;
	//    const char *type;
	struct property *properties;
	void *data;
};

struct device_node *of_parse_phandle(const struct device_node *np,
				     const char *phandle_name, int index);
inline void of_node_put(struct device_node *node) {};

#if 0
inline struct device_node *node of_node_get(const struct device_node *np)
{
	return np;
}
#endif

const void *of_get_property(const struct device_node *np, const char *name,
			    int *lenp);

#endif /* _LINUX_OF_H */
