#ifndef _CIF_REG_H_
#define _CIF_REG_H_

#include "reg_fields_csi_mac.h"
#include "reg_fields_csi_wrap.h"
#include "reg_blocks_csi_mac.h"
#include "reg_blocks_csi_wrap.h"

#define CIF_MAC_TOP_PHY_REG_BASE    (0x0A0C2000)
#define CIF_WRAP_TOP_PHY_REG_BASE   (0x0300B000)
#define CIF_MAC1_TOP_PHY_REG_BASE    (0x0A0C4000)
#define CIF_WRAP1_TOP_PHY_REG_BASE   (0x0300D000)
#define DRAM_PHY_BASE           (0x100000000)
#define CIF_BLK_REGS_BITW       (9)
#define CIF_BLK_ID_BITW         (4)

#define MAP_CIF_BLOCK_ID(_ba)   (((_ba) >> CIF_BLK_REGS_BITW) \
				& ((1 << CIF_BLK_ID_BITW) - 1))

/* CIF REG FIELD DEFINE */

/* CIF CSI MAC BLOCK ADDR OFFSET DEFINE */
#define CIF_MAC_BLK_BA_TOP         (0x00000000)
#define CIF_MAC_BLK_BA_SLVDS       (0x00000200)
#define CIF_MAC_BLK_BA_CSI         (0x00000400)

enum CIF_MAC_BLK_ID_T {
	CIF_MAC_BLK_ID_TOP      = MAP_CIF_BLOCK_ID(CIF_MAC_BLK_BA_TOP),
	CIF_MAC_BLK_ID_SLVDS    = MAP_CIF_BLOCK_ID(CIF_MAC_BLK_BA_SLVDS),
	CIF_MAC_BLK_ID_CSI      = MAP_CIF_BLOCK_ID(CIF_MAC_BLK_BA_CSI),
	CIF_MAC_BLK_ID_MAX
};

/* CIF CSI WRAP BLOCK ADDR OFFSET DEFINE */
#define CIF_WRAP_BLK_BA_TOP         (0x00000000)

enum CIF_WRAP_BLK_ID_T {
	CIF_WRAP_BLK_ID_TOP      = MAP_CIF_BLOCK_ID(CIF_WRAP_BLK_BA_TOP),
	CIF_WRAP_BLK_ID_MAX
};

#endif //_CIF_REG_H_
