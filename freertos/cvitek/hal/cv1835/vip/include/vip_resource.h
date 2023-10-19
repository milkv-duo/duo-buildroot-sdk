//#include "resource.h"
#include "ioport.h"
struct resource;

static struct resource cvitek_vip_resource[ ] = { 
         [ 0] = {
				   . name = "sc",
                   . start = 0x0a080000, 
                   . end = 0x0a080000 + 0x00010000 - 1, 
                   . flags = IORESOURCE_MEM, 
         } , 
         [ 1] = { 
				   . name = "dwa",
                   . start = 0x0a0a0000, 
                   . end = 0x0a0a0000 + 0x00002000 - 1, 
                   . flags = IORESOURCE_MEM, 
         } , 
         [ 2] = { 
				   . name = "vip_sys",
                   . start = 0x0a0c8000, 
                   . end = 0x0a0c8000 + 0x000000a0 - 1, 
                   . flags = IORESOURCE_MEM, 
         } , 
         [ 3] = { 
				   . name = "isp",
                   . start = 0x0a000000, 
                   . end = 0x0a000000 + 0x00080000 - 1, 
                   . flags = IORESOURCE_MEM, 
         } , 
         [ 4] = { 
				   . name = "dphy",
                   . start = 0x0300c000, 
                   . end = 0x0300c000 + 0x00000100 - 1, 
                   . flags = IORESOURCE_MEM, 
         } , 
} ; 
