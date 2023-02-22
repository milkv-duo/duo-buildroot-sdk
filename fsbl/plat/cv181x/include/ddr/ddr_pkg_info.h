#ifndef __DDR_PKG_INFO_H__
#define __DDR_PKG_INFO_H__
#include <stdint.h>

#define DDR_VENDOR_UNKNOWN		0b00000
#define DDR_VENDOR_NY_4G		0b00001
#define DDR_VENDOR_NY_2G		0b00010
#define DDR_VENDOR_ESMT_1G		0b00011
#define DDR_VENDOR_ESMT_512M_DDR2	0b00100
#define DDR_VENDOR_ETRON_1G		0b00101
#define DDR_VENDOR_ESMT_2G		0b00110
#define DDR_VENDOR_PM_2G		0b00111
#define DDR_VENDOR_PM_1G		0b01000
#define DDR_VENDOR_ETRON_512M_DDR2	0b01001
#define DDR_VENDOR_ESMT_N25_1G		0b01010

#define DDR_CAPACITY_UNKNOWN		0b000
#define DDR_CAPACITY_512M		0b001
#define DDR_CAPACITY_1G			0b010
#define DDR_CAPACITY_2G			0b011
#define DDR_CAPACITY_4G			0b100

#define PKG_UNKNOWN			0b000
#define PKG_QFN				0b001
#define PKG_BGA				0b010

#define DDR_TYPE_UNKNOWN		0
#define DDR_TYPE_DDR2			1
#define DDR_TYPE_DDR3			2

extern uint32_t ddr_data_rate;

void read_ddr_pkg_info(void);
uint8_t get_ddr_vendor(void);
uint8_t get_ddr_capacity(void);
uint8_t get_pkg(void);
uint8_t get_ddr_type(void);

#endif /* __DDR_PKG_INFO_H__ */
