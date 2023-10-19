#ifndef __OF_RESERVED_MEM_H
#define __OF_RESERVED_MEM_H

#include <linux/device.h>
#include <linux/types.h>

struct of_phandle_args;
struct reserved_mem_ops;

struct reserved_mem {
    const char          *name;
    phys_addr_t         base;
    phys_addr_t         size;
};
struct reserved_mem *of_reserved_mem_lookup(struct device_node *np);
void rtos_init_reserved_mem(struct reserved_mem *res_mem, int count);
#endif /* __OF_RESERVED_MEM_H */
