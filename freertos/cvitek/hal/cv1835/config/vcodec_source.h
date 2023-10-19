#ifndef __VCODEC_SOURCE_H__
#define __VCODEC_SOURCE_H__
#include "linux/ioport.h"
#include "linux/platform_device.h"
#include "linux/of_reserved_mem.h"
#include "linux/of.h"

struct resource;
struct platform_device;
struct reserved_mem;
struct device_node;
/* reg */
struct resource cvitek_vcodec_resource[] __initdata = {
	[0] =	{
			.name = "h265",
			.start = 0x0b020000,
			.end = 0x0b020000 + 0x00010000 - 1,
			.flags = IORESOURCE_MEM,
		},
	[1] =	{
			.name = "h264",
			.start = 0x0b010000,
			.end = 0x0b010000 + 0x00010000 - 1,
			.flags = IORESOURCE_MEM,
		},
};

struct platform_device vcodec_platform_device __initdata = {
	.name = "cvitek,vcodec",
	.id = -1,
};

struct property vcode_clk_property __initdata = {
	.name = "clk-name",
	.value = 0,
	.next = 0, // end of property
};

struct property vcode_reg_property __initdata = {
	.name = "reg-name",
	.value = &cvitek_vcodec_resource,
	.next = &vcode_clk_property,
};

struct reserved_mem vcode_reserved_mem __initdata = {
	.name = "vcodec",
	.size = 0x100000,
};

struct device_node vcode_mem_node __initdata = {
	.name = "vcodec",
	.data = &vcode_reserved_mem,
};
struct property vcode_mem_property __initdata = {
	.name = "memory-region",
	.value = &vcode_mem_node,
	.next = &vcode_reg_property,
};

struct property vcode_comp_property __initdata = {
	.name = "compatible",
	.value = "cvitek,vcodec",
	.next = &vcode_mem_property,
};
struct device_node vcode_device_node[] __initdata = {
	[0] =	{
			.name = "vcodec",
			.properties = &vcode_comp_property,
		},
};

#endif
