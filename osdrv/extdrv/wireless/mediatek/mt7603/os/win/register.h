/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

 	Module Name:
 	Register.h
 
	Abstract:
    Register Definition.

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   08-22-2002    created

*/

#ifndef	__REGISTER_H__
#define	__REGISTER_H__




#define DMA_PHYSICAL_ADDR			0x50000000
#define RA_U3DMA_WLCFG			(HIF_PHYSICAL_BASEADDR+0x00029018)

#define PCI_PHYSICAL_BASEADDR				0x80020000
#define MCU_PHYSICAL_BASEADDR				0x80000000
#define HIF_PHYSICAL_BASEADDR				0x50000000
#define PSE_PHYSICAL_BASEADDR				0xA0000000
#define WF_PHY_PHYSICAL_BASEADDR			0x60200000
#define WF_MAC_PHYSICAL_BASEADDR			0x60000000

#define RBIST_BASE		0x810C0000

#define PCI_BASEADDR				0x0000//TOP_CFG
#define MCU_BASEADDR				0x2000
#define HIF_BASEADDR				0x4000
#define PSE_BASEADDR				0x8000
#define WF_PHY_BASEADDR				0x10000
#define WF_MAC_BASEADDR				0x20000
#define CFGREMAP1_BASEADDR			0x40000
#define CFGREMAP2_BASEADDR			0x80000


#define MCU_CFG_BASE		0x2000
#define MCU_PCIE_REMAP_1	(MCU_CFG_BASE + 0x500)
#define MCU_PCIE_REMAP_2	(MCU_CFG_BASE + 0x504)
/*#ifdef _USB
#define DMA_BASEADDR				DMA_PHYSICAL_ADDR
#else
#define DMA_BASEADDR				HIF_BASEADDR
#endif*/

//#define DMA_PHYSICAL_BASEADDR		DMA_PHYSICAL_ADDR
#define DMA_BASEADDR				HIF_BASEADDR


#define HIF_CLIENT				0x800C0000
#define HIF_CLIENT_RXINFO		(HIF_CLIENT + 0x68)
//****************************************************************************************
//
//  For 2860c
//
//****************************************************************************************
//********************************************
// PCI Registers
//********************************************
#define RA_EE_CTRL				0x81070000//PCIe remapping address, remap 81070000 on 00040000


#define RA_REMAP_ADDR			0x2500
#define RA_REMAP_ADDR2			0x2504
#define RA_CONFIG_REMAP		0x00040000//PCIe configurable remap

#define RA_EFUSE_CTRL			0x81070008
#define RA_EFUSE_RDATA0			0x81070030
#define RA_EFUSE_RDATA1			0x81070034
#define RA_EFUSE_RDATA2			0x81070038
#define RA_EFUSE_RDATA3			0x8107003C

#define RA_EFUSE_WDATA0			0x81070010
#define RA_EFUSE_WDATA1			0x81070014
#define RA_EFUSE_WDATA2			0x81070018
#define RA_EFUSE_WDATA3			0x8107001C


//#define CLK_ENABLE				0x14C


#define RA_FW_STATUS			(PCI_BASEADDR+0x1134)
#define SYS_STRAP_STA			(PCI_BASEADDR+0x1010)

#define MT_PSE_WTBL_2_ADDR			0xa5000000
//********************************************
// SCH/DMA Registers
//********************************************
#define RA_INT_STATUS			(DMA_BASEADDR+0x0200)
#define RA_INT_MASK				(DMA_BASEADDR+0x0204)
#define RA_WPDMA_GLO_CFG		(DMA_BASEADDR+0x0208)
#define RA_WPDMA_RST_IDX		(DMA_BASEADDR+0x020c)

//#ifndef _AP
#define RA_TX_BASE_PTR0			(DMA_BASEADDR+0x0300)
#define RA_TX_MAX_CNT0			(DMA_BASEADDR+0x0304)
#define RA_TX_CTX_IDX0			(DMA_BASEADDR+0x0308)
#define RA_TX_DTX_IDX0			(DMA_BASEADDR+0x030C)

#define RA_TX_BASE_PTR1			(DMA_BASEADDR+0x0310)
#define RA_TX_MAX_CNT1			(DMA_BASEADDR+0x0314)
#define RA_TX_CTX_IDX1			(DMA_BASEADDR+0x0318)
#define RA_TX_DTX_IDX1			(DMA_BASEADDR+0x031C)

#define RA_TX_BASE_PTR2			(DMA_BASEADDR+0x0320)
#define RA_TX_MAX_CNT2			(DMA_BASEADDR+0x0324)
#define RA_TX_CTX_IDX2			(DMA_BASEADDR+0x0328)
#define RA_TX_DTX_IDX2			(DMA_BASEADDR+0x032C)

#define RA_TX_BASE_PTR3			(DMA_BASEADDR+0x0330)
#define RA_TX_MAX_CNT3			(DMA_BASEADDR+0x0334)
#define RA_TX_CTX_IDX3			(DMA_BASEADDR+0x0338)
#define RA_TX_DTX_IDX3			(DMA_BASEADDR+0x033C)

#define RA_TX_BASE_PTR4			(DMA_BASEADDR+0x0340)
#define RA_TX_MAX_CNT4			(DMA_BASEADDR+0x0344)
#define RA_TX_CTX_IDX4			(DMA_BASEADDR+0x0348)
#define RA_TX_DTX_IDX4			(DMA_BASEADDR+0x034C)

#define RA_TX_BASE_PTR5			(DMA_BASEADDR+0x0350)
#define RA_TX_MAX_CNT5			(DMA_BASEADDR+0x0354)
#define RA_TX_CTX_IDX5			(DMA_BASEADDR+0x0358)
#define RA_TX_DTX_IDX5			(DMA_BASEADDR+0x035C)

#define RA_TX_BASE_PTR6			(DMA_BASEADDR+0x0360)
#define RA_TX_MAX_CNT6			(DMA_BASEADDR+0x0364)
#define RA_TX_CTX_IDX6			(DMA_BASEADDR+0x0368)
#define RA_TX_DTX_IDX6			(DMA_BASEADDR+0x036C)

#define RA_TX_BASE_PTR7			(DMA_BASEADDR+0x0370)
#define RA_TX_MAX_CNT7			(DMA_BASEADDR+0x0374)
#define RA_TX_CTX_IDX7			(DMA_BASEADDR+0x0378)
#define RA_TX_DTX_IDX7			(DMA_BASEADDR+0x037C)

#define RA_TX_BASE_PTR8			(DMA_BASEADDR+0x0380)
#define RA_TX_MAX_CNT8			(DMA_BASEADDR+0x0384)
#define RA_TX_CTX_IDX8			(DMA_BASEADDR+0x0388)
#define RA_TX_DTX_IDX8			(DMA_BASEADDR+0x038C)

#define RA_TX_BASE_PTR9			(DMA_BASEADDR+0x0390)
#define RA_TX_MAX_CNT9			(DMA_BASEADDR+0x0394)
#define RA_TX_CTX_IDX9			(DMA_BASEADDR+0x0398)
#define RA_TX_DTX_IDX9			(DMA_BASEADDR+0x039C)

#define RA_TX_BASE_PTR10			(DMA_BASEADDR+0x03A0)
#define RA_TX_MAX_CNT10			(DMA_BASEADDR+0x03A4)
#define RA_TX_CTX_IDX10			(DMA_BASEADDR+0x03A8)
#define RA_TX_DTX_IDX10			(DMA_BASEADDR+0x03AC)

#define RA_TX_BASE_PTR11			(DMA_BASEADDR+0x03B0)
#define RA_TX_MAX_CNT11			(DMA_BASEADDR+0x03B4)
#define RA_TX_CTX_IDX11			(DMA_BASEADDR+0x03B8)
#define RA_TX_DTX_IDX11			(DMA_BASEADDR+0x03BC)

#define RA_TX_BASE_PTR12			(DMA_BASEADDR+0x03C0)
#define RA_TX_MAX_CNT12			(DMA_BASEADDR+0x03C4)
#define RA_TX_CTX_IDX12			(DMA_BASEADDR+0x03C8)
#define RA_TX_DTX_IDX12			(DMA_BASEADDR+0x03CC)

#define RA_TX_BASE_PTR13			(DMA_BASEADDR+0x03D0)
#define RA_TX_MAX_CNT13			(DMA_BASEADDR+0x03D4)
#define RA_TX_CTX_IDX13			(DMA_BASEADDR+0x03D8)
#define RA_TX_DTX_IDX13			(DMA_BASEADDR+0x03DC)

#define RA_RX_BASE_PTR0			(DMA_BASEADDR+0x0400)
#define RA_RX_MAX_CNT0				(DMA_BASEADDR+0x0404)
#define RA_RX_CALC_IDX0			(DMA_BASEADDR+0x0408)
#define RA_FS_DRX_IDX0				(DMA_BASEADDR+0x040C)

#define RA_RX_BASE_PTR1			(DMA_BASEADDR+0x0410)
#define RA_RX_MAX_CNT1				(DMA_BASEADDR+0x0414)
#define RA_RX_CALC_IDX1			(DMA_BASEADDR+0x0418)
#define RA_FS_DRX_IDX1				(DMA_BASEADDR+0x041C)
//#endif

#define	RA_PSE_CLOCL		PCI_BASEADDR + 0x1100

//********************************************
// DMA Scheduler Registers
//********************************************
//USB
/*#define RA_PHYSICAL_QUEUE_PRIORITY1		(DMA_PHYSICAL_BASEADDR+0x0580)
#define RA_PHYSICAL_QUEUE_PRIORITY2		(DMA_PHYSICAL_BASEADDR+0x0584)

#define RA_PHYSICAL_SCHEDULER_REG1		(DMA_PHYSICAL_BASEADDR+0x0588)
#define RA_PHYSICAL_SCHEDULER_REG2		(DMA_PHYSICAL_BASEADDR+0x058C)
#define RA_PHYSICAL_SCHEDULER_REG3		(DMA_PHYSICAL_BASEADDR+0x0590)
#define RA_PHYSICAL_SCHEDULER_REG4		(DMA_PHYSICAL_BASEADDR+0x0594)

#define RA_PHYSICAL_GROUP_THD0 		(DMA_PHYSICAL_BASEADDR+0x0598)
#define RA_PHYSICAL_GROUP_THD1 		(DMA_PHYSICAL_BASEADDR+0x059C)
#define RA_PHYSICAL_GROUP_THD2 		(DMA_PHYSICAL_BASEADDR+0x05A0)
#define RA_PHYSICAL_GROUP_THD3 		(DMA_PHYSICAL_BASEADDR+0x05A4)
#define RA_PHYSICAL_GROUP_THD4 		(DMA_PHYSICAL_BASEADDR+0x05A8)
#define RA_PHYSICAL_GROUP_THD5 		(DMA_PHYSICAL_BASEADDR+0x05AC)

#define RA_PHYSICAL_BMAP0 				(DMA_PHYSICAL_BASEADDR+0x05B0)
#define RA_PHYSICAL_BMAP1 				(DMA_PHYSICAL_BASEADDR+0x05B4)
#define RA_PHYSICAL_BMAP2 				(DMA_PHYSICAL_BASEADDR+0x05B8)

#define RA_PHYSICAL_HIGH_PRIORITY1 		(DMA_PHYSICAL_BASEADDR+0x05BC)
#define RA_PHYSICAL_HIGH_PRIORITY2 		(DMA_PHYSICAL_BASEADDR+0x05C0)
#define RA_PHYSICAL_PRIORITY_MASK 		(DMA_PHYSICAL_BASEADDR+0x05C4)

#define RA_PHYSICAL_RSV_MAX_THD 		(DMA_PHYSICAL_BASEADDR+0x05C8)
*/
//PCIe
#define RA_QUEUE_PRIORITY1		(DMA_BASEADDR+0x0580)
#define RA_QUEUE_PRIORITY2		(DMA_BASEADDR+0x0584)

#define RA_SCHEDULER_REG1		(DMA_BASEADDR+0x0588)
#define RA_SCHEDULER_REG2		(DMA_BASEADDR+0x058C)
#define RA_SCHEDULER_REG3		(DMA_BASEADDR+0x0590)
#define RA_SCHEDULER_REG4		(DMA_BASEADDR+0x0594)

#define RA_GROUP_THD0 		(DMA_BASEADDR+0x0598)
#define RA_GROUP_THD1 		(DMA_BASEADDR+0x059C)
#define RA_GROUP_THD2 		(DMA_BASEADDR+0x05A0)
#define RA_GROUP_THD3 		(DMA_BASEADDR+0x05A4)
#define RA_GROUP_THD4 		(DMA_BASEADDR+0x05A8)
#define RA_GROUP_THD5 		(DMA_BASEADDR+0x05AC)

#define RA_BMAP0 				(DMA_BASEADDR+0x05B0)
#define RA_BMAP1 				(DMA_BASEADDR+0x05B4)
#define RA_BMAP2 				(DMA_BASEADDR+0x05B8)

#define RA_HIGH_PRIORITY1 		(DMA_BASEADDR+0x05BC)
#define RA_HIGH_PRIORITY2 		(DMA_BASEADDR+0x05C0)
#define RA_PRIORITY_MASK 		(DMA_BASEADDR+0x05C4)

#define RA_RSV_MAX_THD 		(DMA_BASEADDR+0x05C8)

#define per_port_pause_enable_control_1 0xA38

#define RA_PAGE_CNT_0 HIF_BASEADDR+0x540
#define	RA_PAGE_CNT_1 HIF_BASEADDR+0x544
#define RA_PAGE_CNT_2 HIF_BASEADDR+0x548
#define RA_PAGE_CNT_3 HIF_BASEADDR+0x54c
#define	RA_PAGE_CNT_4 HIF_BASEADDR+0x550
#define	RA_PAGE_CNT_5 HIF_BASEADDR+0x554
#define RA_PAGE_CNT_6 HIF_BASEADDR+0x558
#define RA_PAGE_CNT_7 HIF_BASEADDR+0x55c
#define RA_PAGE_CNT_8 HIF_BASEADDR+0x560
#define RA_PAGE_CNT_9 HIF_BASEADDR+0x564
#define RA_PAGE_CNT_10 HIF_BASEADDR+0x568
#define RA_PAGE_CNT_11 HIF_BASEADDR+0x56c
#define RA_PAGE_CNT_12 HIF_BASEADDR+0x570
#define RA_PAGE_CNT_13 HIF_BASEADDR+0x574
#define RA_PAGE_CNT_14 HIF_BASEADDR+0x578
#define RA_PAGE_CNT_15 HIF_BASEADDR+0x57c



#define CR_PHY_CTRL_2 WF_PHY_BASEADDR + 0x4108
#define RO_PHYCTRL_STS0 WF_PHY_BASEADDR + 0x410C
#define RO_PHYCTRL_STS1 WF_PHY_BASEADDR + 0x4110
#define RO_PHYCTRL_STS2 WF_PHY_BASEADDR + 0x4114
#define RO_PHYCTRL_STS4 WF_PHY_BASEADDR + 0x411C
#define RO_PHYCTRL_STS5 WF_PHY_BASEADDR + 0x4120

#define RO_AGC_DEBUG_2	WF_PHY_BASEADDR + 0x05A4
//****************************************************************************************
// End of 2860c define
//****************************************************************************************

#define WF_CFG_BASE		WF_MAC_BASEADDR+0x0
#define WF_TRB_BASE		WF_MAC_BASEADDR+0x1000
#define WF_AGG_BASE		WF_MAC_BASEADDR+0x01200
#define WF_ARB_BASE		WF_MAC_BASEADDR+0x1400
#define WF_TMAC_BASE	WF_MAC_BASEADDR+0x1600
#define WF_RMAC_BASE	WF_MAC_BASEADDR+0x1800
#define WF_SEC_BASE		WF_MAC_BASEADDR+0x1A00
#define WF_DMA_BASE		WF_MAC_BASEADDR+0x1C00
#define WF_CFGOFF_BASE	WF_MAC_BASEADDR+0x1E00
#define WF_PF_BASE		WF_MAC_BASEADDR+0x2000
#define WF_WTBLOFF_BASE	WF_MAC_BASEADDR+0x3000
#define WF_ETBF_BASE	WF_MAC_BASEADDR+0x3200
#define WF_LPON_BASE	WF_MAC_BASEADDR+0x4000
#define WF_INT_BASE		WF_MAC_BASEADDR+0x4400
#define WF_WTBLON_BASE	WF_MAC_BASEADDR+0x8000
#define WF_MIB_BASE		WF_MAC_BASEADDR+0xC000
#define WF_AON_BASE		WF_MAC_BASEADDR+0xD000
#define CMDBT_OFF_BASE	WF_MAC_BASEADDR+0xE000
#define CMDBT_AON_BASE	WF_MAC_BASEADDR+0xE200


//WF_ARB_BASE
#define ARB_GTQR0		(WF_ARB_BASE + 0x010)
#define ARB_GTQR1		(WF_ARB_BASE + 0x014)
#define ARB_GTQR2		(WF_ARB_BASE + 0x018)
#define ARB_GTQR3		(WF_ARB_BASE + 0x01c)
#define ARB_AIFSR0		(WF_ARB_BASE + 0x020)
#define ARB_AIFSR1		(WF_ARB_BASE + 0x024)
#define ARB_ACCWXR0		(WF_ARB_BASE + 0x028)
#define ARB_ACCWXR1		(WF_ARB_BASE + 0x02c)
#define ARB_ACCWXR2		(WF_ARB_BASE + 0x030)
#define ARB_ACCWXR3		(WF_ARB_BASE + 0x034)
#define ARB_ACCWXR4		(WF_ARB_BASE + 0x038)
#define ARB_ACCWXR5		(WF_ARB_BASE + 0x03c)
#define ARB_ACCWIR0		(WF_ARB_BASE + 0x040)
#define ARB_RQCR		(WF_ARB_BASE + 0x070)
#define ARB_RQCR_RX_START	0x01
#define ARB_RQCR_RXV_START	0x10
#define ARB_RQCR_RXV_R_START	0x80
#define ARB_RQCR_RXV_T_START	0x100
#define ARB_TQCR0		(WF_ARB_BASE + 0x0100)


//RMAC
#define RMAC_RFCR		(WF_RMAC_BASE + 0x00)
#define RMAC_CB0R0		(WF_RMAC_BASE + 0x04)
#define RMAC_CB0R1		(WF_RMAC_BASE + 0x08)
#define RMAC_CB1R0		(WF_RMAC_BASE + 0x0c)
#define RMAC_CB1R1		(WF_RMAC_BASE + 0x10)
#define RMAC_CB2R0		(WF_RMAC_BASE + 0x14)
#define RMAC_CB2R1		(WF_RMAC_BASE + 0x18)
#define RMAC_CB3R0		(WF_RMAC_BASE + 0x1c)
#define RMAC_CB3R1		(WF_RMAC_BASE + 0x20)
#define RMAC_OMA0R0		WF_RMAC_BASE+0x24
#define RMAC_OMA0R1		(WF_RMAC_BASE + 0x028)
#define RMAC_OMA1R0		(WF_RMAC_BASE + 0x02c)
#define RMAC_OMA1R1		(WF_RMAC_BASE + 0x030)
#define RMAC_OMA2R0		(WF_RMAC_BASE + 0x034)
#define RMAC_OMA2R1		(WF_RMAC_BASE + 0x038)
#define RMAC_OMA3R0		(WF_RMAC_BASE + 0x03c)
#define RMAC_OMA3R1		(WF_RMAC_BASE + 0x040)
#define RMAC_RMCR				(WF_RMAC_BASE + 0x080)
#define RMAC_MISC				(WF_RMAC_BASE + 0x094)
#define RMAC_TMR_PA		(WF_RMAC_BASE + 0x0e0)

#define RMAC_RMCR_SMPS_MODE		(0x3 << 20)
#define RMAC_RMCR_SMPS_BY_RTS		(1 << 25)
#define RMAC_RMCR_RX_STREAM_0		1<<22
#define RMAC_RMCR_RX_STREAM_1		1<<23
#define RMAC_RMCR_RX_STREAM_2		1<<24

// WF_MIB_BASE
/***** MIB Counter *****/
#define ARB_MSCR0              (WF_MIB_BASE + 0x0000)
#define ARB_PBSCR              (WF_MIB_BASE + 0x0004)

#define ARB_MSDR0              (WF_MIB_BASE + 0x0008)
#define ARB_MSDR4              (WF_MIB_BASE + 0x0018)
#define ARB_MSDR5              (WF_MIB_BASE + 0x001C)
#define ARB_MSDR6              (WF_MIB_BASE + 0x0020)
#define ARB_MSDR7              (WF_MIB_BASE + 0x0024)
#define ARB_MSDR8              (WF_MIB_BASE + 0x0028)
#define ARB_MSDR9              (WF_MIB_BASE + 0x002C)
#define ARB_MSDR10             (WF_MIB_BASE + 0x0030)
#define ARB_MSDR11             (WF_MIB_BASE + 0x0034)
#define ARB_MSDR12              (WF_MIB_BASE + 0x0038)
#define ARB_MSDR13              (WF_MIB_BASE + 0x003C)
#define ARB_MSDR14              (WF_MIB_BASE + 0x0040)
#define ARB_MSDR15              (WF_MIB_BASE + 0x0044)
#define ARB_MSDR16              (WF_MIB_BASE + 0x0048)
#define ARB_MSDR17              (WF_MIB_BASE + 0x004C)
#define ARB_MSDR18              (WF_MIB_BASE + 0x0050)
#define ARB_MSDR19              (WF_MIB_BASE + 0x0054)
#define ARB_MSDR20              (WF_MIB_BASE + 0x0058)
#define ARB_MSDR21              (WF_MIB_BASE + 0x005C)
#define ARB_MSDR22             (WF_MIB_BASE + 0x0060)	//RX_AMPDU_COUNT
#define ARB_MSDR23             (WF_MIB_BASE + 0x0064)	//RX_TOTBYTE_COUNT
#define ARB_MSDR24             (WF_MIB_BASE + 0x0068)	//RX_VALIDSF_COUNT
#define ARB_MSDR25             (WF_MIB_BASE + 0x006C)	//RX_VALIDBYTE_COUNT
#define ARB_MSDR26             (WF_MIB_BASE + 0x0070)
#define ARB_MSDR27             (WF_MIB_BASE + 0x0074)
#define ARB_MSDR28             (WF_MIB_BASE + 0x0078)

#define ARB_MDR0                (WF_MIB_BASE + 0x00A0)
#define ARB_MDR1                (WF_MIB_BASE + 0x00A4)
#define ARB_MDR2                (WF_MIB_BASE + 0x00A8)
#define ARB_MDR3                (WF_MIB_BASE + 0x00AC)

#define ARB_MB0SDR0             (WF_MIB_BASE + 0x0100)
#define ARB_MB0SDR1             (WF_MIB_BASE + 0x0104)
#define ARB_MB0SDR2             (WF_MIB_BASE + 0x0108)
#define ARB_MB1SDR0             (WF_MIB_BASE + 0x0110)
#define ARB_MB1SDR1             (WF_MIB_BASE + 0x0114)
#define ARB_MB1SDR2             (WF_MIB_BASE + 0x0118)
#define ARB_MB2SDR0             (WF_MIB_BASE + 0x0120)
#define ARB_MB2SDR1             (WF_MIB_BASE + 0x0124)
#define ARB_MB2SDR2             (WF_MIB_BASE + 0x0128)
#define ARB_MB3SDR0             (WF_MIB_BASE + 0x0130)
#define ARB_MB3SDR1             (WF_MIB_BASE + 0x0134)
#define ARB_MB3SDR2             (WF_MIB_BASE + 0x0138)

//TMAC
#define TMAC_TCR		(WF_TMAC_BASE + 0x00)

#define TMAC_FP0R0		(WF_TMAC_BASE + 0x020)
#define TMAC_FP0R1		(WF_TMAC_BASE + 0x024)
#define TMAC_FP0R2		(WF_TMAC_BASE + 0x028)
#define TMAC_FP0R3		(WF_TMAC_BASE + 0x02C)
#define TMAC_FP0R4		(WF_TMAC_BASE + 0x030)
#define TMAC_FP0R5		(WF_TMAC_BASE + 0x034)
#define TMAC_FP0R6		(WF_TMAC_BASE + 0x038)
#define TMAC_FP0R7		(WF_TMAC_BASE + 0x03C)
#define TMAC_FP0R8		(WF_TMAC_BASE + 0x084)

//AGG
#define AGG_ARCR		(WF_AGG_BASE + 0x010)
#define AGG_ARUCR		(WF_AGG_BASE + 0x014)
#define AGG_ARDCR		(WF_AGG_BASE + 0x018)



#define AGG_AALCR		(WF_AGG_BASE + 0x040)
#define AGG_AALCR1		(WF_AGG_BASE + 0x044)
#define AGG_AWSCR		(WF_AGG_BASE + 0x048)
#define AGG_AWSCR1		(WF_AGG_BASE + 0x04c)
#define AGG_PCR			(WF_AGG_BASE + 0x050)
#define AGG_PCR1		(WF_AGG_BASE + 0x054)
#define AGG_ASRCR		(WF_AGG_BASE + 0x060)
#define AGG_BQCR		(WF_AGG_BASE + 0x064)
#define AGG_BCR			(WF_AGG_BASE + 0x068)
#define AGG_ACR			(WF_AGG_BASE + 0x070)
#define AGG_ACR1		(WF_AGG_BASE + 0x074)
#define AGG_DSCR1		(WF_AGG_BASE + 0xb4)	/* 0x212b4 */
#define AGG_BWCR		(WF_AGG_BASE + 0x0ec)
#define AGG_MRCR		(WF_AGG_BASE + 0x0f4)	/* 0x212f4 */



//WF_DMA_BASE
#define DMA_DCR0		(WF_DMA_BASE + 0x000)
#define DMA_DCR1		(WF_DMA_BASE + 0x004)	/* 0x21c04 */
#define DMA_RCFR0		(WF_DMA_BASE + 0x070)	
#define DMA_VCFR0		(WF_DMA_BASE + 0x07c)	/* 0x21c7c */

#define DMA_TCFR1		(WF_DMA_BASE + 0x084)	/* 0x21c84 */

typedef struct _RMAC_OMA0R0_STRUC{	
	UINT32 addr_31_0;
}RMAC_OMA0R0_STRUC;

typedef union _RMAC_OMA0R1_STRUC{	
	struct {		
		UINT32 addr_47_32: 16;		
		UINT32 own_addr_en:1;		
		UINT32 rsv_17: 15;	
		} field;	
	UINT32 word;
}RMAC_OMA0R1_STRUC;

//#ifndef _AP

//WPDMA_GLO_CFG BIT 

#define TX_WB_DDONE 		0x00000040
#define RX_DMA_BUSY  		0x00000008
#define RX_DMA_EN      		0x00000004
#define TX_DMA_BUSY  		0x00000002 
#define TX_DMA_EN      		0x00000001 
#define TX_DMA_BIGENDIAN    0x00000080 
#define FIFO_LITTLEENDIAN    0x00000800
#define MULTI_DMA_EN    		0x00000400
#define TX_BT_SIZE			0x00000020 
//
// UARTTX: UART Tx Register
//
typedef	union	_UARTTX_STRUC	{
	struct	{
		ULONG		TXD:8;
		ULONG		TxBufFull:1;
		ULONG		TxFrCnt:5;
		ULONG		TxBufErr:1;
		ULONG		Rsvd:17;
	}	field;
	ULONG			word;
}	UARTTX_STRUC, *PUARTTX_STRUC;

//
// UARTRX: UART Rx Register
//
typedef	union	_UARTRX_STRUC	{
	struct	{
		ULONG		RXD:8;
		ULONG		RxDv:1;
		ULONG		RxByteCnt:5;
		ULONG		RxPtyErr:1;
		ULONG		RxStopErr:1;
		ULONG		RxBufErr:1;
		ULONG		Rsvd:15;
	}	field;
	ULONG			word;
}	UARTRX_STRUC, *PUARTRX_STRUC;

//
// BBPCSR: BBP serial control register
//
typedef	union	_BBPCSR_STRUC	{
	struct	{
		ULONG		Value:8;			// Register	value to program into BBP
		ULONG		RegNum:7;			// Selected	BBP	register
		ULONG		WriteControl:1;		// 0: Write	BBP, 1:	Read BBP		
		ULONG		Busy:1;				// 1: ASIC is busy execute BBP programming.	
		ULONG		Rsvd:15;
	}	field;
	ULONG			Group;
}	BBPCSR_STRUC, *PBBPCSR_STRUC;

//
// RFCSR: RF serial control register
//
typedef	union	_RFCSR_STRUC	{
	struct	{
		ULONG		RFRegValue:24;		// Register	value (include register	id)	serial out to RF/IF	chip.
		ULONG		NumberOfBits:5;		// Number of bits used in RFRegValue (I:20,	RFMD:22)		
		ULONG		RFLESTBY:1;			// RF_LE standby mode
		ULONG		RFLESelect:1;		// 0: select RFLE0, 1: select RFLE1 
		ULONG		Busy:1;				// 1: ASIC is busy execute RF programming.
	}	field;
	ULONG			Group;
}	RFCSR_STRUC, *PRFCSR_STRUC;

//
// Register set pair for initialzation register set definition
//
typedef	struct	_RTMP_REG_PAIR
{
	ULONG	Register;
	ULONG	Value;
}	RTMP_REG_PAIR, *PRTMP_REG_PAIR;

//==============================================
#define RBISTCR0 (RBIST_BASE+0x90)
#define RBISTCR1 (RBIST_BASE+0x94)
#define RBISTCR2 (RBIST_BASE+0x98)
#define RBISTCR3 (RBIST_BASE+0x9C)
#define RBISTCR4 (RBIST_BASE+0xA0)
#define RBISTCR5 (RBIST_BASE+0xA4)
#define RBISTCR6 (RBIST_BASE+0xA8)
#define RBISTCR7 (RBIST_BASE+0xAC)
#define RBISTCR8 (RBIST_BASE+0xb0)
#define RBISTCR9 (RBIST_BASE+0xb4)
//#endif
#endif	// __REGISTER_H__
