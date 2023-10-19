/*
 * Procedures for creating, accessing and interpreting the device tree.
 *
 * Paul Mackerras   August 1996.
 * Copyright (C) 1996-2005 Paul Mackerras.
 *
 *  Adapted for 64bit PowerPC by Dave Engebretsen and Peter Bergner.
 *    {engebret|bergner}@us.ibm.com
 *
 *  Adapted for sparc and sparc64 by David S. Miller davem@davemloft.net
 *
 *  Reconsolidated from arch/x/kernel/prom.c by Stephen Rothwell and
 *  Grant Likely.
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#include <linux/of.h>
/**
 * of_parse_phandle - Resolve a phandle property to a device_node pointer
 * @np: Pointer to device node holding phandle property
 * @phandle_name: Name of property holding a phandle value
 * @index: For properties holding a table of phandles, this is the index into
 *         the table
 *
 * Returns the device_node pointer with refcount incremented.  Use
 * of_node_put() on it when done.
 */

/* parse phandle with name only now*/
struct device_node *of_parse_phandle(const struct device_node *np,
				     const char *phandle_name, int index)
{
	int i;
	struct property *property_context;

	if (index < 0 || !np || !phandle_name)
		return 0;
	property_context = np->properties;
	for (i = 0; i < 16, property_context; i++) {
		if (!strcmp(phandle_name, property_context->name)) {
			return property_context->value;
		}
		property_context = property_context->next;
	}
	return 0;
}

/*
 * Find a property with a given name for a given node
 * and return the value.
 */
const void *of_get_property(const struct device_node *np, const char *name,
			    int *lenp)
{
#if 0
	struct property *pp = of_find_property(np, name, lenp);

	return pp ? pp->value : NULL;
#endif
}
