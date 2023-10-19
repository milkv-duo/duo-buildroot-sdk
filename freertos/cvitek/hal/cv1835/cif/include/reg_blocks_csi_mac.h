#ifndef _REG_BLOCKS_CSI_MAC_H_
#define _REG_BLOCKS_CSI_MAC_H_

//#include <stdint.h>
#include "linux/types.h"

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_SENSOR_MAC_1C4D_T {
	union REG_SENSOR_MAC_1C4D_00            REG_00;
	uint32_t                                _resv_0x4[3];
	union REG_SENSOR_MAC_1C4D_10            REG_10;
	union REG_SENSOR_MAC_1C4D_14            REG_14;
	union REG_SENSOR_MAC_1C4D_18            REG_18;
	union REG_SENSOR_MAC_1C4D_1C            REG_1C;
	union REG_SENSOR_MAC_1C4D_20            REG_20;
	union REG_SENSOR_MAC_1C4D_24            REG_24;
	union REG_SENSOR_MAC_1C4D_28            REG_28;
	uint32_t                                _resv_0x2c[1];
	union REG_SENSOR_MAC_1C4D_30            REG_30;
	uint32_t                                _resv_0x34[3];
	union REG_SENSOR_MAC_1C4D_40            REG_40;
	union REG_SENSOR_MAC_1C4D_44            REG_44;
	union REG_SENSOR_MAC_1C4D_48            REG_48;
	union REG_SENSOR_MAC_1C4D_4C            REG_4C;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_SUBLVDS_CTRL_TOP_T {
	union REG_SUBLVDS_CTRL_TOP_00           REG_00;
	union REG_SUBLVDS_CTRL_TOP_04           REG_04;
	union REG_SUBLVDS_CTRL_TOP_08           REG_08;
	union REG_SUBLVDS_CTRL_TOP_0C           REG_0C;
	union REG_SUBLVDS_CTRL_TOP_10           REG_10;
	union REG_SUBLVDS_CTRL_TOP_14           REG_14;
	union REG_SUBLVDS_CTRL_TOP_18           REG_18;
	union REG_SUBLVDS_CTRL_TOP_1C           REG_1C;
	union REG_SUBLVDS_CTRL_TOP_20           REG_20;
	union REG_SUBLVDS_CTRL_TOP_24           REG_24;
	union REG_SUBLVDS_CTRL_TOP_28           REG_28;
	union REG_SUBLVDS_CTRL_TOP_2C           REG_2C;
	union REG_SUBLVDS_CTRL_TOP_30           REG_30;
	uint32_t                                _resv_0x34[3];
	union REG_SUBLVDS_CTRL_TOP_40           REG_40;
	uint32_t                                _resv_0x44[3];
	union REG_SUBLVDS_CTRL_TOP_50           REG_50;
	union REG_SUBLVDS_CTRL_TOP_54           REG_54;
	union REG_SUBLVDS_CTRL_TOP_58           REG_58;
	uint32_t                                _resv_0x5c[1];
	union REG_SUBLVDS_CTRL_TOP_60           REG_60;
	union REG_SUBLVDS_CTRL_TOP_64           REG_64;
	union REG_SUBLVDS_CTRL_TOP_68           REG_68;
	union REG_SUBLVDS_CTRL_TOP_6C           REG_6C;
	union REG_SUBLVDS_CTRL_TOP_70           REG_70;
	union REG_SUBLVDS_CTRL_TOP_74           REG_74;
};

/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_CSI_CTRL_TOP_T {
	union REG_CSI_CTRL_TOP_00               REG_00;
	union REG_CSI_CTRL_TOP_04               REG_04;
	union REG_CSI_CTRL_TOP_08               REG_08;
	union REG_CSI_CTRL_TOP_0C               REG_0C;
	union REG_CSI_CTRL_TOP_10               REG_10;
	uint32_t                                _resv_0x14[11];
	union REG_CSI_CTRL_TOP_40               REG_40;
	uint32_t                                _resv_0x44[1];
	union REG_CSI_CTRL_TOP_48               REG_48;
	union REG_CSI_CTRL_TOP_4C               REG_4C;
	union REG_CSI_CTRL_TOP_50               REG_50;
	union REG_CSI_CTRL_TOP_54               REG_54;
	union REG_CSI_CTRL_TOP_58               REG_58;
	union REG_CSI_CTRL_TOP_5C               REG_5C;
	union REG_CSI_CTRL_TOP_60               REG_60;
	union REG_CSI_CTRL_TOP_64               REG_64;
	uint32_t                                _resv_0x68[2];
	union REG_CSI_CTRL_TOP_70               REG_70;
	union REG_CSI_CTRL_TOP_74               REG_74;
};

#endif // _REG_BLOCKS_CSI_MAC_H_
