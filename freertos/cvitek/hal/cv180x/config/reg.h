#ifndef _CVI_REG_H_
#define _CVI_REG_H_
#include "mmio.h"

#define _reg_read(addr) mmio_read_32(addr)
#define _reg_write(addr, data) mmio_write_32(addr, data)
#define _reg_write_mask(addr, mask, data) mmio_clrsetbits_32(addr, mask, data)

#endif //_CVI_REG_H_
