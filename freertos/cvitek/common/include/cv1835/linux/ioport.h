/*
 * ioport.h Definitions of routines for detecting, reserving and
 *      allocating system resources.
 *
 * Authors: Linus Torvalds
 */

#ifndef _LINUX_IOPORT_H
#define _LINUX_IOPORT_H

#include <linux/compiler.h>
#include <linux/types.h>
/*
 * Resources are tree-like, allowing
 * nesting etc..
 */
struct resource {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    //const char name[32];
    unsigned long flags;
//    unsigned long desc;
//    struct resource *parent, *sibling, *child;
};

//#define IORESOURCE_BITS     0x000000ff  /* Bus-specific bits */

#define IORESOURCE_TYPE_BITS    0x00001f00  /* Resource type */
//#define IORESOURCE_IO       0x00000100  /* PCI/ISA I/O ports */
#define IORESOURCE_MEM      0x00000200
//#define IORESOURCE_REG      0x00000300  /* Register offsets */
#define IORESOURCE_IRQ      0x00000400
//#define IORESOURCE_DMA      0x00000800
//#define IORESOURCE_BUS      0x00001000

static inline unsigned long resource_type(const struct resource *res)
{
    return res->flags & IORESOURCE_TYPE_BITS;
}

#endif // end of _LINUX_IOPORT_H
