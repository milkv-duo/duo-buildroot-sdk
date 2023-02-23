#ifndef _CIF_REG_H_
#define _CIF_REG_H_

#include "mmio.h"
#include "reg_fields_csi_mac.h"
#include "reg_fields_csi_wrap.h"
#include "reg_blocks_csi_mac.h"
#include "reg_blocks_csi_wrap.h"

// #define CIF_MAC0_TOP_PHY_REG_BASE    (0x0A0C2000)
// #define CIF_MAC1_TOP_PHY_REG_BASE    (0x0A0C4000)
// #define CIF_MAC2_TOP_PHY_REG_BASE    (0x0A0C6000)

// #define CIF_WRAP_TOP_PHY_REG_BASE    (0x0A0D0000)
// #define CIF_WRAP_TOP_PHY4L_REG_BASE  (0x0A0D3000)
// #define CIF_WRAP_TOP_PHY2L_REG_BASE  (0x0A0D0000)

#define DRAM_PHY_BASE           (0x100000000)
#define CIF_BLK_REGS_BITW       (9)
#define CIF_BLK_ID_BITW         (4)

#define MAP_CIF_BLOCK_ID(_ba)   (((_ba) >> CIF_BLK_REGS_BITW) \
				& ((1 << CIF_BLK_ID_BITW) - 1))

/* CIF REG FIELD DEFINE */

/* CIF CSI MAC BLOCK ADDR OFFSET DEFINE */
#define CIF_MAC_BLK_BA_TOP         (0x00000000)	//sensor mac
#define CIF_MAC_BLK_BA_SLVDS       (0x00000200) //subLVDS
#define CIF_MAC_BLK_BA_CSI         (0x00000400) //csi_ctrl_top

enum CIF_MAC_BLK_ID_T {
	CIF_MAC_BLK_ID_TOP      = MAP_CIF_BLOCK_ID(CIF_MAC_BLK_BA_TOP),
	CIF_MAC_BLK_ID_SLVDS    = MAP_CIF_BLOCK_ID(CIF_MAC_BLK_BA_SLVDS),
	CIF_MAC_BLK_ID_CSI      = MAP_CIF_BLOCK_ID(CIF_MAC_BLK_BA_CSI),
	CIF_MAC_BLK_ID_MAX
};

/* CIF CSI WRAP BLOCK ADDR OFFSET DEFINE */
#define CIF_WRAP_BLK_BA_TOP         (0x00000000)
#define CIF_WRAP_BLK_BA_4L          (0x00000300)
#define CIF_WRAP_BLK_BA_2L          (0x00000600)

enum CIF_WRAP_BLK_ID_T {
	CIF_WRAP_BLK_ID_TOP     = MAP_CIF_BLOCK_ID(CIF_WRAP_BLK_BA_TOP),
	CIF_WRAP_BLK_ID_4L      = MAP_CIF_BLOCK_ID(CIF_WRAP_BLK_BA_4L),
	CIF_WRAP_BLK_ID_2L      = MAP_CIF_BLOCK_ID(CIF_WRAP_BLK_BA_2L),
	CIF_WRAP_BLK_ID_MAX
};

#endif //_CIF_REG_H_
