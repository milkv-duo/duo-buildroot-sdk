#ifndef __VIP_SOURCE_H__
#define __VIP_SOURCE_H__
#include "linux/ioport.h"
#include "linux/platform_device.h"
#include "linux/of_reserved_mem.h"
#include "linux/of.h"

struct resource;
struct platform_device;
struct reserved_mem;
struct device_node;

struct resource cvitek_vip_resource[] __initdata = {
	[0] =	{
			.name = "sc",
			.start = 0x0a080000,
			.end = 0x0a080000 + 0x00010000 - 1,
			.flags = IORESOURCE_MEM,
		},
	[1] =	{
			.name = "dwa",
			.start = 0x0a0a0000,
			.end = 0x0a0a0000 + 0x00002000 - 1,
			.flags = IORESOURCE_MEM,
		},
	[2] =	{
			.name = "vip_sys",
			.start = 0x0a0c8000,
			.end = 0x0a0c8000 + 0x000000a0 - 1,
			.flags = IORESOURCE_MEM,
		},
	[3] =	{
			.name = "isp",
			.start = 0x0a000000,
			.end = 0x0a000000 + 0x00080000 - 1,
			.flags = IORESOURCE_MEM,
		},
	[4] =	{
			.name = "dphy",
			.start = 0x0300c000,
			.end = 0x0300c000 + 0x00000100 - 1,
			.flags = IORESOURCE_MEM,
		},
};

struct platform_device vip_platform_device __initdata = {
	.name = "vip",
	.id = -1,
};

struct reserved_mem vip_reserved_mem __initdata = {
	.name = "vip",
	.size = 0x06000000,
};

struct device_node vip_mem_node __initdata = {
	.name = "vip",
	.data = &vip_reserved_mem,
};

struct property vip_freq_property __initdata = {
	.name = "clock-freq-vip-sys1",
	.value = 300000000,
	.next = 0,
};

struct property vip_clk_property __initdata = {
	.name = "clk-name",
	.value = 0, //&cvitek_vip_resource,
	.next = &vip_freq_property,
};

struct property vip_reg_property __initdata = {
	.name = "reg-name",
	.value = &cvitek_vip_resource,
	.next = &vip_clk_property,
};

struct property vip_mem_property __initdata = {
	.name = "memory-region",
	.value = &vip_mem_node,
	.next = &vip_reg_property,
};

struct property vip_comp_property __initdata = {
	.name = "compatible",
	.value = "cvitek,vip",
	.next = &vip_mem_property,
};

struct device_node vip_device_node[] __initdata = {
	[0] =	{
			.name = "vip",
			.properties = &vip_comp_property,
		},
};

struct platform_device cif_platform_device __initdata = {
	.name = "cif",
	.id = -1,
};

struct resource cvitek_cif_resource[] __initdata = {
	[0] =	{
			.name = "csi_mac0",
			.start = 0x0a0c2000,
			.end = 0x0a0c2000 + 0x00002000 - 1,
			.flags = IORESOURCE_MEM,
		},
	[1] =	{
			.name = "csi_wrap0",
			.start = 0x0300b000,
			.end = 0x0300b000 + 0x00001000 - 1,
			.flags = IORESOURCE_MEM,
		},
	[2] =	{
			.name = "csi_mac1",
			.start = 0x0a0c4000,
			.end = 0x0a0c4000 + 0x00002000 - 1,
			.flags = IORESOURCE_MEM,
		},
	[3] =	{
			.name = "csi_wrap1",
			.start = 0x0300d000,
			.end = 0x0300d000 + 0x00001000 - 1,
			.flags = IORESOURCE_MEM,
		},
};

struct property cif_clk_property __initdata = {
	.name = "clk-name",
	.value = 0,
	.next = 0, // end of property
};

struct property cif_reg_property __initdata = {
	.name = "reg-name",
	.value = &cvitek_cif_resource,
	.next = &cif_clk_property,
};

struct property cif_comp_property __initdata = {
	.name = "compatible",
	.value = "cvitek,cif",
	.next = &cif_reg_property,
};

struct device_node cif_device_node[] __initdata = {
	[0] =	{
			.name = "cif",
			.properties = &cif_comp_property,
		},
};

#endif
