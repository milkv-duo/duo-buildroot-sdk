/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    Hal.c

Abstract:

Environment:

    Kernel mode

Notes:

    Copyright (c) 2013 Microsoft Corporation.  
    All Rights Reserved.

--*/
#include "config.h"

//7603 init Flow brgin====================================================
/**
 *1.mt_asic_top_init
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * 
 *
 * @return void
 * Otherwise, an error code is returned.
 */
int  mt_asic_top_init(RTMP_ADAPTER *pAd)
{
	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));	
#ifdef MT7603_FPGA
	UINT32 mac_val;		
	RTMP_IO_WRITE32(pAd, 0x24088, 0x900); // Set 40MHz Clock
	RTMP_IO_WRITE32(pAd, 0x2d034, 0x64180003);	// Set 32k clock, this clock is used for lower power.
#endif /* MT7603_FPGA */
	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));	
	return TRUE;
}
/**
 * 2.mt_hif_sys_init
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * 
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void mt_hif_sys_init(RTMP_ADAPTER *pAd)
{
#ifdef _USB
//:::TODO
	//U3DMA_WLCFG             U3DMAWLCFG;
        // Set DMA scheduler to Bypass mode.
        // Enable Tx/Rx
        //RTMP_ADAPTER *dx = (RTMP_ADAPTER *)pDeviceObject->DeviceExtension;
	//RTMP_IO_READ32(dx, 0x50029018,(PULONG)&U3DMAWLCFG);

	//RTMP_IO_READ32USB(dx, 0x50029018,&U3DMAWLCFG);//UDMA_WLCFG_0
       /* DBGPRINT(RT_DEBUG_ERROR,("1 U3DMAWLCFG = %x\n", U3DMAWLCFG));
        U3DMAWLCFG.WL_RX_AGG_EN = 1;
        U3DMAWLCFG.WL_RX_AGG_LMT = 0x15;
        U3DMAWLCFG.WL_RX_AGG_TO = 0x80;
        DBGPRINT(RT_DEBUG_ERROR,("2 U3DMAWLCFG = %x\n", U3DMAWLCFG));
        RTMP_IO_WRITE32(pAd, UDMA_WLCFG_0, (PUCHAR)&U3DMAWLCFG, sizeof(U3DMA_WLCFG));*/
#else//PCIe
	UINT32 mac_val;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));	
	
	RTMP_IO_READ32(pAd, RA_WPDMA_GLO_CFG, &mac_val);
	//mac_val |= 0xb0; // bit 7/5~4 => 1
	mac_val = 0x52000850;
	RTMP_IO_WRITE32(pAd, RA_WPDMA_GLO_CFG, mac_val);
	InitDMA(pAd);//enable RA_WPDMA_GLO_CFG TX/RX and set ring
#endif
	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));	
}


/**
 * 3.Firmware Download, HQADLL do this.

    NOTE: 
        a).For PCI interface, before firmware download, make sure "MT_WPDMA_GLO_CFG" as enable the Tx/Rx
        b).For firmware FPGA-RAM code, we need to make sure the firmware download is success by Event Report with status = SUCCESS!
        c).After firmware download success, we need to polling CR (0x80021134 bit 1) is set as 1, 
            which means the firmware init(including MAC/BBP/RF early initialization)

        d).After all upper cases are done, we can go init MAC/BBP/RF for host driver
 */



/**
 *4.Init MAC 
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * 
 *
 * @return void
 * Otherwise, an error code is returned.
 */
int asic_set_tmac_info_template(RTMP_ADAPTER *pAd)
{
#ifdef _USB
	PDEVICE_OBJECT pDeviceObject;
#endif
	UINT32 dw[5];
	TMAC_TXD_2 *dw2 = (TMAC_TXD_2 *)(&dw[0]);
	TMAC_TXD_3 *dw3 = (TMAC_TXD_3 *)(&dw[1]);
	TMAC_TXD_4 *dw4 = (TMAC_TXD_4 *)(&dw[2]);
	TMAC_TXD_5 *dw5 = (TMAC_TXD_5 *)(&dw[3]);
	TMAC_TXD_6 *dw6 = (TMAC_TXD_6 *)(&dw[4]);
	
	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));	

	RtlZeroMemory((UCHAR *)(&dw[0]), sizeof(dw));
	
	dw2->htc_vld = 0;
	dw2->frag = 0;
	dw2->max_tx_time = 0;
	dw2->fix_rate = 0;

	dw3->remain_tx_cnt = MT_TX_RETRY_CNT;
	dw3->sn_vld = 0;
	dw3->pn_vld = 0;

	dw5->pid = PID_DATA_AMPDU;
	dw5->tx_status_fmt = 0;
	dw5->tx_status_2_host = 1;
	dw5->bar_sn_ctrl = 0; //HW
	dw5->pwr_mgmt = TMI_PM_BIT_CFG_BY_HW; // HW

#ifdef _USB
//USB
	USBHwHal_WriteMacRegister(pAd, 0x800C0040, dw[0]);
	USBHwHal_WriteMacRegister(pAd, 0x800C0044, dw[1]);
	USBHwHal_WriteMacRegister(pAd, 0x800C0048, dw[2]);
	USBHwHal_WriteMacRegister(pAd, 0x800C004C, dw[3]);
	USBHwHal_WriteMacRegister(pAd, 0x800C0050, dw[4]);
#endif /* _USB */

#ifdef RTMP_PCI_SUPPORT
//PCIe
// TODO: shaing, for MT7628, may need to change this as RTMP_MAC_PCI
#ifdef MT7603_FPGA
	RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, 0x80080000);
#endif /* MT7603_FPGA */

	RTMP_IO_WRITE32(pAd, 0xc0040, dw[0]);
	RTMP_IO_WRITE32(pAd, 0xc0044, dw[1]);
	RTMP_IO_WRITE32(pAd, 0xc0048, dw[2]);
	RTMP_IO_WRITE32(pAd, 0xc004c, dw[3]);
	RTMP_IO_WRITE32(pAd, 0xc0050, dw[4]);

#ifdef MT7603_FPGA
// TODO: shaing, for MT7628, may need to change this as RTMP_MAC_PCI
	// After change the Tx Padding CR of PCI-E Client, we need to re-map for PSE region
	RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, PSE_PHYSICAL_BASEADDR);// MT_PSE_BASE_ADDR
#endif // MT7603_FPGA 

#endif // RTMP_PCI_SUPPORT


	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));	
	return TRUE;

}


/**
 * 4.DMA scheduer reservation page assignment 
 */
 /*
    DMA scheduer reservation page assignment
*/
INT32 SetResrvPage(RTMP_ADAPTER *pAd)
{
	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));	

	/* Resvervation page */
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_0, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_1, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_2, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_3, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_4, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_5, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_6, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_7, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_8, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_9, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_10, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_11, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_12, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_13, 0x10);
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_14, 0x10);
#ifdef MT7603_FPGA
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_15, 0x10);
#else
	RTMP_IO_WRITE32(pAd, RA_PAGE_CNT_15, 0x20);
#endif
	
	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));	
	return TRUE;
}



/**
 * 5.AsicDMASchedulerInit
 */
 /*
    DMA scheduer reservation page assignment
*/
int AsicDMASchedulerInit(RTMP_ADAPTER *pAd, int iMode)
{
	ULONG mac_val;
	UINT32 page_size = 128;
	UINT32 page_cnt = 0x5a;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));	
	
	/*
		Q0~Q3: WMM1
		Q4: Management queue
		Q7: Beacon
		Q8: MC/BC
		Q9~Q12: WMM2
		Q13: Management queue
	*/

#ifdef MT7603_FPGA
	page_size = 256;
#endif /* MT7603_FPGA */

	if (iMode == DMA_SCH_HYBRID)
	{
		/* Highest Priority */
		//mac_val = 0xdddddddd; // Eddy's code
		mac_val = 0x76543210;
		RTMP_IO_WRITE32(pAd, RA_HIGH_PRIORITY1, mac_val);
		mac_val = 0xffdcba98;
		//mac_val = 0xf784dddd; // Eddy's code
		RTMP_IO_WRITE32(pAd, RA_HIGH_PRIORITY2, mac_val);

		/* Queue Priority */
		mac_val = 0x76543210;
		//mac_val = 0xb1a09566; // Eddy's code
		RTMP_IO_WRITE32(pAd, RA_QUEUE_PRIORITY1, mac_val);
		mac_val = 0xffdcba98;
		//mac_val = 0xf784d3c2; // Eddy's code
		RTMP_IO_WRITE32(pAd, RA_QUEUE_PRIORITY2, mac_val);
		RTMP_IO_WRITE32(pAd, RA_GROUP_THD0, page_cnt);
		RTMP_IO_WRITE32(pAd, RA_BMAP0, 0xffff);
		//RTMP_IO_WRITE32(pAd, MT_BMAP_0, 0xbfff); // Eddy's code

		RTMP_IO_WRITE32(pAd, RA_PRIORITY_MASK, 0x8000ffff);

		/* Schedule Priority, page size/FFA, FFA = (page_cnt * page_size) */
		mac_val = (3 << 28) | (page_cnt);
		RTMP_IO_WRITE32(pAd, RA_SCHEDULER_REG1, mac_val);
		mac_val = 4096 / page_size; /* Max packet size */
		RTMP_IO_WRITE32(pAd, RA_SCHEDULER_REG2, mac_val);
		/* FFA1 max threshold */
		RTMP_IO_WRITE32(pAd, RA_RSV_MAX_THD, page_cnt);
		//+++Add by Eddy
		SetResrvPage(pAd);

		//Disable DMA scheduler
		//RTMP_IO_WRITE32(pAd, AGG_DSCR1, 0x80000000);
//---Add by Eddy

		/* config as hybrid mode */
		RTMP_IO_WRITE32(pAd, RA_SCHEDULER_REG4, 1<<6);

#ifndef _USB
//PCIe
		/*
			Wei-Guo's comment:
			2DW/7DW => 0x800C_006C[14:12] = 3'b0
			3DW/8DW =>0x800C_006C[14:12] = 3'b1
		*/
		// In FPGA mode, we need to change tx pad by different DMA scheduler setting
		RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, 0x80080000);
		RTMP_IO_READ32(pAd, 0xc006c, &mac_val);
		mac_val &= (~(7<<12));
		RTMP_IO_WRITE32(pAd, 0xc006c, mac_val);
		// After change the Tx Padding CR of PCI-E Client, we need to re-map for PSE region
		RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, PSE_PHYSICAL_BASEADDR);	
#endif /* RTMP_PCI_SUPPORT */

		// Disable TxD padding
		RTMP_IO_READ32(pAd, DMA_DCR1, &mac_val);
		mac_val &= (~(0x7<<8));
		RTMP_IO_WRITE32(pAd, DMA_DCR1, mac_val);

#if 0
		// Set to disable DMA scheduler, work-around solution
		// Set LMAC DMA scheduler: DISABLE @6011_00B4 = 32'hA000_0000 (bit[31:30] = 2'b11)
		// Set HIF DMA scheduler: BYPASS  Not BYPASS
		// Set TXDSCR_PAD (0x60160004.bit[10:8]) = 1
		// TODO: shiang-MT7603, remove this if we want to test DMA scheduler!
		RTMP_IO_READ32(pAd, AGG_DSCR1, &mac_val);
		mac_val |= 0xa0000000;
		RTMP_IO_WRITE32(pAd, AGG_DSCR1, mac_val);
		// work around for un-sync of TxD between HIF and LMAC
		RTMP_IO_READ32(pAd, DMA_DCR1, &mac_val);
		mac_val |= (0x1<<8);
		RTMP_IO_WRITE32(pAd, DMA_DCR1, mac_val);
#endif



	}


	// TODO: shiang-7603, force loopback mode!
	if (iMode == DMA_SCH_BYPASS)
	{
		RTMP_IO_WRITE32(pAd, RA_SCHEDULER_REG4, 1<<5);

		// Set to disable DMA scheduler
		// TODO: shiang-MT7603, remove this if we want to test DMA scheduler!
		RTMP_IO_READ32(pAd, AGG_DSCR1, &mac_val);
		mac_val |= 0x80000000;
		RTMP_IO_WRITE32(pAd, AGG_DSCR1, mac_val);

#ifndef _USB
//PCIe	
		/*
			Wei-Guo's comment:
			2DW/7DW => 0x800C_006C[14:12] = 3'b0
			3DW/8DW =>0x800C_006C[14:12] = 3'b1
		*/
		// In FPGA mode, we need to change tx pad by different DMA scheduler setting!
// TODO: shaing, for MT7628, may need to change this as RTMP_MAC_PCI
		RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, 0x80080000);//CFGREMAP2_BASEADDR
		RTMP_IO_READ32(pAd, 0xc006c, &mac_val);
		mac_val &= (~(7<<12));
		mac_val |= (1<<12);
		RTMP_IO_WRITE32(pAd, 0xc006c, mac_val);
		// After change the Tx Padding CR of PCI-E Client, we need to re-map for PSE region
		RTMP_IO_WRITE32(pAd, MCU_PCIE_REMAP_2, PSE_PHYSICAL_BASEADDR);//MT_PSE_BASE_ADDR

#endif

		// work around for un-sync of TxD between HIF and LMAC
		RTMP_IO_READ32(pAd, DMA_DCR1, &mac_val);
		mac_val &= (~(0x7<<8));
		mac_val |= (0x1<<8);
		RTMP_IO_WRITE32(pAd, DMA_DCR1, mac_val);

	}


	if (iMode == DMA_SCH_LMAC)
	{
		/* Highest Priority */
		mac_val = 0xdddddddd;
		RTMP_IO_WRITE32(pAd, RA_HIGH_PRIORITY1 , mac_val);
		mac_val = 0xf784dddd;
		RTMP_IO_WRITE32(pAd, RA_HIGH_PRIORITY2 , mac_val);

		/* Queue Priority */
		mac_val = 0xb1a09566;
		RTMP_IO_WRITE32(pAd, RA_QUEUE_PRIORITY1, mac_val);
		mac_val = 0xf784d3c2; 
		RTMP_IO_WRITE32(pAd, RA_QUEUE_PRIORITY2, mac_val);
		RTMP_IO_WRITE32(pAd, RA_GROUP_THD0, 0x3a);
		RTMP_IO_WRITE32(pAd, RA_BMAP0, 0xbfff);
		
		RTMP_IO_WRITE32(pAd, RA_PRIORITY_MASK, 0xa190ffff);

		/* Schedule Priority, page size/FFA, FFA = (page_cnt * page_size) */
		mac_val = (3 << 28) | (page_cnt);
		RTMP_IO_WRITE32(pAd, RA_SCHEDULER_REG1, mac_val);
		mac_val = 4096 / page_size; /* Max packet size */
		RTMP_IO_WRITE32(pAd, RA_SCHEDULER_REG2, mac_val);
		/* FFA1 max threshold */
		RTMP_IO_WRITE32(pAd, RA_RSV_MAX_THD, 0x3a);

		//Resvervation page
		SetResrvPage(pAd);
				
		/* config as LMAC prediction mode */
		RTMP_IO_WRITE32(pAd, RA_SCHEDULER_REG4, 0x0);

	}
	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));	
	return TRUE;
}

 /**
 *
 */
 /*
    DMA scheduer reservation page assignment
*/
int mt7603_init_mac_cr(RTMP_ADAPTER *pAd)
{
	ULONG mac_val;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));	
	
	AsicDMASchedulerInit(pAd,DMA_SCH_HYBRID);
#ifdef MT7603_FPGA
	// enable MAC2MAC mode
	RTMP_IO_READ32(pAd, RMAC_MISC, &mac_val);
	mac_val |= BIT(18);
	RTMP_IO_WRITE32(pAd, RMAC_MISC, mac_val);
#endif /* MT7603_FPGA */

	/* Preparation of TxD DW2~DW6 when we need run 3DW format */
	asic_set_tmac_info_template(pAd);

	/* A-MPDU Agg limit control in range 1/2/4/8/10/12/14/16 */
	RTMP_IO_READ32(pAd, AGG_AWSCR, &mac_val);
	mac_val = ((1 << 0) |(2<<8) | (4 << 16) | (8 << 24));
	RTMP_IO_WRITE32(pAd, AGG_AWSCR, mac_val);
	
	RTMP_IO_READ32(pAd, AGG_AWSCR1, &mac_val);
	mac_val = ((10 << 0) |(12<<8) | (14 << 16) | (16 << 24));
	RTMP_IO_WRITE32(pAd, AGG_AWSCR1, mac_val);
	
	RTMP_IO_WRITE32(pAd, AGG_AALCR, 0);
	RTMP_IO_WRITE32(pAd, AGG_AALCR1, 0);

	/* Vector report queue setting */
	RTMP_IO_READ32(pAd, DMA_VCFR0, &mac_val);
	mac_val |= BIT(13);
	RTMP_IO_WRITE32(pAd, DMA_VCFR0, mac_val);

	/* TxStatus report queue setting */
	RTMP_IO_READ32(pAd, DMA_TCFR1, &mac_val);
	mac_val |= BIT(14);
	RTMP_IO_WRITE32(pAd, DMA_TCFR1, mac_val);

	RTMP_IO_READ32(pAd, RMAC_TMR_PA, &mac_val);
    	mac_val = mac_val & ~BIT31;
    	RTMP_IO_WRITE32(pAd, RMAC_TMR_PA, mac_val);
	/* Configure all rx packets to HIF, except WOL2M packet */
	RTMP_IO_READ32(pAd, DMA_RCFR0, &mac_val);
	mac_val = 0x00010000; // drop duplicate
	// TODO: shiang-MT7603, remove me after FPGA verification done
	mac_val |= 0x00200000; // receive BA/CF_End/Ack/RTS/CTS/CTRL_RSVED
	RTMP_IO_WRITE32(pAd, DMA_RCFR0, mac_val);

	/* Configure Rx Vectors report to HIF */
	RTMP_IO_READ32(pAd, DMA_VCFR0, &mac_val);
	mac_val &= (~0x1); // To HIF
	mac_val |= 0x2000; // RxRing 1
	RTMP_IO_WRITE32(pAd, DMA_VCFR0, mac_val);

	/* AMPDU BAR setting */
	/* Disable BAR tx of HW */
	RTMP_IO_READ32(pAd, AGG_MRCR, &mac_val);
	mac_val &= (~0xf00);
	RTMP_IO_WRITE32(pAd, AGG_MRCR, mac_val);

	/* Configure the BAR rate setting */
	RTMP_IO_READ32(pAd, AGG_ACR, &mac_val);
	mac_val &= (~0xfff00000);
	RTMP_IO_WRITE32(pAd, AGG_ACR, mac_val);

	/* AMPDU Statistics Range Control setting
		0 < agg_cnt - 1 <= range_cr(0),				=> 1
		range_cr(0) < agg_cnt - 1 <= range_cr(4),		=> 2~5
		range_cr(4) < agg_cnt - 1 <= range_cr(14),	=> 6~15
		range_cr(14) < agg_cnt - 1,					=> 16~
	*/
	RTMP_IO_READ32(pAd, AGG_ASRCR, &mac_val);
	mac_val =  (0 << 0) | (4 << 8) | (14 << 16);
	RTMP_IO_WRITE32(pAd, AGG_ASRCR, mac_val);

	// Enable MIB counters
	RTMP_IO_WRITE32(pAd, ARB_MSCR0, 0x7fffffff);
	RTMP_IO_WRITE32(pAd, ARB_PBSCR, 0xffffffff);
	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));	
	return TRUE;

 }

 /**
 * 8.Others
 */
 /*
    DMA scheduer reservation page assignment
*/
int mt7603_DoOtherConfig(RTMP_ADAPTER *pAd)
{
	ULONG mac_val = 0x00001002;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));				

	RTMP_IO_WRITE32(pAd,RMAC_RFCR,mac_val);

	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));				
	return TRUE;
 }

//7603 init Flow end====================================================

//RTMP_IO_READ32/RTMP_IO_WRITE32..
/**
 * Stop ContinuTX Signal Test
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * 
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void StopContinuTXTest(RTMP_ADAPTER *pAd)
{	
	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));				
#if 0
	ULONG Value = 0;
	RTMP_IO_READ32(pAd, CR_TXFD_1,&Value);
	Value = (Value>>24);
	Value &= 0x00000007;//keep 3 bits

	if (0==Value)
	{
		RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x00000004);//1
	}
	else
	{
		RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x00000001);//1
	}

	
	RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x00000000);//2
	
	RTMP_IO_WRITE32(pAd,CR_PHYCK_CTRL,0x00000000);//1
	RTMP_IO_WRITE32(pAd,CR_FR_CKG_CTRL,0x00000000);//2
	RTMP_IO_WRITE32(pAd,CR_FFT_MANU_CTRL,0x00000000);//3
	RTMP_IO_WRITE32(pAd,CR_PHYCTRL_0,0x00000000);//4
	RTMP_IO_WRITE32(pAd,CR_PHYCTRL_DBGCTRL,0x00000000);//5
	RTMP_IO_WRITE32(pAd,CR_PHYMUX_3,0x00000000);//6
	RTMP_IO_WRITE32(pAd,CR_PHYMUX_5,0x00000000);//7
	RTMP_IO_WRITE32(pAd,CR_PHYMUX_10,0x00000000);//8
	RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x00000000);//9
	RTMP_IO_WRITE32(pAd,CR_TXFD_0,0x00500000);//10
	RTMP_IO_WRITE32(pAd,CR_TXFD_3,0x00000000);//11
	RTMP_IO_WRITE32(pAd,R_WF_TMAC_PCTSR,0x00000000);//12	
#endif
/*
TOPCR	60204704	00000005	;	
TOPCR	60204704	00000000	;	
TOPCR	60200000	00000045	;
TOPCR	60200004	00000045	;
TOPCR	60200704	00000000	;
TOPCR	60204100	00000000	;	// [25:24] CR_BW_SEL = 0 (20M)
TOPCR	60204140	00000000	;	//[31] CR_PHYCRL_UP, [5:4] FFT ON
TOPCR	6020420C	00000000	;
TOPCR	60204214	00000580	;	// WF0 manual RF on
TOPCR	60205214	00000000	;	// WF1 manual RF off
TOPCR	60204228	00000000	;	// [26:20] TX_PWR_DBM (step 0.5dB)
TOPCR	6020422C	00000000	;	// [31][29] CR_PHYMUX_TX0_ON
TOPCR	60200A0C	00000000	;	//  CR_TXFE_ENAB
TOPCR	60201A0C	00000000	;	//  CR_TXFE_ENAB
TOPCR	60204700	00000000	;	// [27:26] CR_TX_CBW = 0, [25:24] CR_TX_DBW = 0
TOPCR	6020470C	00000000	;	// [27:21] CR_FRM_RATE = 0(CCK1M) 
TOPCR	60130108	00000000
TOPCR	60200008	FFFFFFFF
TOPCR	60204004	FFFFFFFF
*/
	RTMP_IO_WRITE32(pAd,0x14704,0x00000005);
	RTMP_IO_WRITE32(pAd,0x14704,0x00000000);	
	RTMP_IO_WRITE32(pAd,0x10000,0x00000045);
	RTMP_IO_WRITE32(pAd,0x10004,0x00000045);
	RTMP_IO_WRITE32(pAd,0x10704,0x00000000);//3
	RTMP_IO_WRITE32(pAd,0x14100,0x00000000);//4
	RTMP_IO_WRITE32(pAd,0x14140,0x00000000);
	RTMP_IO_WRITE32(pAd,0x1420C,0x00000000);
	RTMP_IO_WRITE32(pAd,0x14214,0x00000580);
	RTMP_IO_WRITE32(pAd,0x15214,0x00000000);
	RTMP_IO_WRITE32(pAd,0x14228,0x00000000);
	RTMP_IO_WRITE32(pAd,0x1422C,0x00000000);
	RTMP_IO_WRITE32(pAd,0x10A0C,0x00000000);
	RTMP_IO_WRITE32(pAd,0x11A0C,0x00000000);
	RTMP_IO_WRITE32(pAd,0x14700,0x00000000);
	RTMP_IO_WRITE32(pAd,0x1470C,0x00000000);
	RTMP_IO_WRITE32(pAd,0x21708,0x00000000);
	RTMP_IO_WRITE32(pAd,0x10008,0xFFFFFFFF);
	RTMP_IO_WRITE32(pAd,0x14004,0xFFFFFFFF);
}


/**
 * Start ContinuTX Signal Test
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param iTestType ,indicate which TX type.
 *CONTI_STF,CONTI_LTF,CONTI_OFDM,CONTI_CCK,CONTI_CCK_PI2_ROTATE,CONTI_CCK_PI1_ROTATE
 *
 * @return void.
 * Otherwise, an error code is returned.
 */
void StartContinuesTXTest(RTMP_ADAPTER *pAd,UCHAR Modulation, UCHAR BW, UCHAR pri_ch, UCHAR rate,UCHAR WFSelect)
{
	ULONG CRValue = 0x0;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));			
	DBGPRINT(RT_DEBUG_TRACE,("%s Modulation = %d, BW=%d, pri_ch = %d, rate = %d, WFSelect = %d-->\n",__FUNCTION__, Modulation, BW, pri_ch, rate, WFSelect));	
	RTMP_IO_WRITE32(pAd,0x10008,0xFFFFFFFF);//0
	RTMP_IO_WRITE32(pAd,0x14004,0xFFFFFFFF);//
	RTMP_IO_WRITE32(pAd,0x10000,0x00000038);//
	RTMP_IO_WRITE32(pAd,0x10004,0x00000038);//3
	RTMP_IO_WRITE32(pAd,0x10704,0x0AA00000);//3
	//
	CRValue = 0x0;
	if(BANDWIDTH_20==BW)
	{
		CRValue = 0x00000000;
	}
	else if(BANDWIDTH_40==BW)
	{
		CRValue = 0x01000000;

	}
	else if(BANDWIDTH_80==BW)
	{
		CRValue = 0x02000000;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s BW = %d error!!!\n",__FUNCTION__, BW));	
		return;
	}
	if( EXTCHA_ABOVE == pri_ch)
	{
		CRValue |= 0x00000000;
	}
	else if( EXTCHA_BELOW == pri_ch)
	{
		CRValue |= 0x00100000;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s pri_ch = %d error!!!\n",__FUNCTION__, pri_ch));	
		return;
	}
	RTMP_IO_WRITE32(pAd,0x14100,CRValue);//3
	//
	RTMP_IO_WRITE32(pAd,0x14140,0x80000030);//3
	RTMP_IO_WRITE32(pAd,0x1420C,0x80680008);//3

	//
	if( WF_0 == WFSelect) //WF0
	{
		RTMP_IO_WRITE32(pAd,0x14214,0x00000597);//3
		RTMP_IO_WRITE32(pAd,0x15214,0x00000010);//3
	}
	else if( WF_1 == WFSelect)//WF1
	{
		RTMP_IO_WRITE32(pAd,0x1520C,0x80680008);//3
		RTMP_IO_WRITE32(pAd,0x14214,0x00000590);//3
		RTMP_IO_WRITE32(pAd,0x15214,0x00000017);//3
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s WFSelect = %d error!!!\n",__FUNCTION__, WFSelect));	
		return;
	}

	//
	if(BANDWIDTH_20==BW)
	{
		RTMP_IO_WRITE32(pAd,0x14700,0x00030000);//3
	}
	else if(BANDWIDTH_40==BW)
	{
		RTMP_IO_WRITE32(pAd,0x14700,0x14030000);//3
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s BW = %d error!!!\n",__FUNCTION__, BW));
		return;
	}

	//
	if(MODULATION_SYSTEM_CCK == Modulation)
	{
		CRValue = 0x0;
		switch(rate)
		{
			case MCS_0:
			case MCS_8:
				CRValue = 0x00000000;;
				break;
			case MCS_1:
			case MCS_9:
				CRValue = 0x00200000;;
				break;
			case MCS_2:
			case MCS_10:
				CRValue = 0x00400000;;;
				break;
			case MCS_3:
			case MCS_11:
				CRValue = 0x00600000;
				break;
			default:
				DBGPRINT(RT_DEBUG_TRACE,("%s CCK rate = %d error!!!\n",__FUNCTION__, rate));	
				return;
		}
		RTMP_IO_WRITE32(pAd,0x1470C,CRValue);
	}
	else if(MODULATION_SYSTEM_OFDM == Modulation)
	{
		CRValue = 0x0;
		switch(rate)
		{
			case MCS_0:
				CRValue = 0x01600000;
				break;
			case MCS_1:
				CRValue = 0x01E00000;
				break;
			case MCS_2:
				CRValue = 0x01400000;
				break;
			case MCS_3:
				CRValue = 0x01C00000;
				break;
			case MCS_4:
				CRValue = 0x01200000;
				break;
			case MCS_5:
				CRValue = 0x01900000;
				break;
			case MCS_6:
				CRValue = 0x01000000;
				break;
			case MCS_7:
				CRValue = 0x01800000;
				break;
			default:
				DBGPRINT(RT_DEBUG_TRACE,("%s OFDM rate = %d error!!!\n",__FUNCTION__, rate));		
				return;
		}
		RTMP_IO_WRITE32(pAd,0x1470C,CRValue);
	}
	else if(MODULATION_SYSTEM_HT20 == Modulation ||MODULATION_SYSTEM_HT40 == Modulation)
	{
		CRValue = 0x0;
		switch(rate)
		{		
			case MCS_0:
				CRValue = 0x00000000;
				break;
			case MCS_1:
				CRValue = 0x00200000;
				break;
			case MCS_2:
				CRValue = 0x00400000;
				break;
			case MCS_3:
				CRValue = 0x00600000;
				break;
			case MCS_4:
				CRValue = 0x00800000;
				break;
			case MCS_5:
				CRValue = 0x00A00000;
				break;
			case MCS_6:
				CRValue = 0x00C00000;
				break;
			case MCS_7:
				CRValue = 0x00E00000;
				break;
			case MCS_32:
				CRValue = 0x04000000;
				break;
			default:
				DBGPRINT(RT_DEBUG_TRACE,("%s OFDM HT MCS = %d error!!!\n",__FUNCTION__, rate));	
				return;
		}
		RTMP_IO_WRITE32(pAd,0x1470C,CRValue);
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s Modulation = %d error!!!\n",__FUNCTION__, Modulation));
		return;
	}
	
	RTMP_IO_WRITE32(pAd,0x14228,0x09400000);//3
	//
	if( WF_0 == WFSelect) //WF0
	{
		RTMP_IO_WRITE32(pAd,0x1422C,0xA0000000);//3
	}
	else if( WF_1 == WFSelect)//WF1
	{
		RTMP_IO_WRITE32(pAd,0x1422C,0x90000000);//3
	}
	
	if(PREAMBLE_CCK == Modulation)
	{
		RTMP_IO_WRITE32(pAd,0x14704,0x300000F8);
	}
	else if(PREAMBLE_OFDM == Modulation)
	{
		RTMP_IO_WRITE32(pAd,0x14704,0x310000F2);
	}
	else if(PREAMBLE_GREEN_FIELD == Modulation ||PREAMBLE_MIX_MODE == Modulation)
	{
		RTMP_IO_WRITE32(pAd,0x14704,0x320000F2);
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s Modulation = %d error!!!\n",__FUNCTION__, Modulation));
		return;
	}
	
	RTMP_IO_WRITE32(pAd,0x10A0C,0x000000C0);
	RTMP_IO_WRITE32(pAd,0x11A0C,0x000000C0);	

	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));			

}


void StartContinuTXTest(RTMP_ADAPTER *pAd,int iTestType)
{
	DBGPRINT(RT_DEBUG_TRACE,("%s iTestType=%d-->\n",__FUNCTION__,iTestType));		
	StopContinuTXTest(pAd);
	if (CONTI_STF == iTestType)
	{
		RTMP_IO_WRITE32(pAd,R_WF_TMAC_PCTSR,0x80000000);//0
		RTMP_IO_WRITE32(pAd,CR_PHYCK_CTRL,0x00000038);//1
		RTMP_IO_WRITE32(pAd,CR_FR_CKG_CTRL,0x00000038);//2
		RTMP_IO_WRITE32(pAd,CR_FFT_MANU_CTRL,0x0AA00000);//3
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_0,0x02000000);//4
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_DBGCTRL,0x00000030);//5
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_3,0x80680008);//6
		
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_5,0x00000597);//6
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_10,0x09400000);//7
		RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);//8
		RTMP_IO_WRITE32(pAd,CR_TXFD_0,0x0A500000);//9
		RTMP_IO_WRITE32(pAd,CR_TXFD_3,0x01800000);//10
		RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x11000002);//11
		
	}
	else if (CONTI_LTF == iTestType)
	{
		RTMP_IO_WRITE32(pAd,R_WF_TMAC_PCTSR,0x80000000);//0
		RTMP_IO_WRITE32(pAd,CR_PHYCK_CTRL,0x00000038);//1
		RTMP_IO_WRITE32(pAd,CR_FR_CKG_CTRL,0x00000038);//2
		RTMP_IO_WRITE32(pAd,CR_FFT_MANU_CTRL,0x0AA00000);//3
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_0,0x02000000);//4
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_DBGCTRL,0x00000030);//5
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_3,0x80680008);//6
		
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_5,0x00000597);//6
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_10,0x09400000);//7
		RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);//8
		RTMP_IO_WRITE32(pAd,CR_TXFD_0,0x0A500000);//9
		RTMP_IO_WRITE32(pAd,CR_TXFD_3,0x01800000);//10
		RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x21000002);//11
			
	}
	else if (CONTI_OFDM == iTestType)
	{
		RTMP_IO_WRITE32(pAd,R_WF_TMAC_PCTSR,0x80000000);//0
		RTMP_IO_WRITE32(pAd,CR_PHYCK_CTRL,0x00000038);//1
		RTMP_IO_WRITE32(pAd,CR_FR_CKG_CTRL,0x00000038);//2
		RTMP_IO_WRITE32(pAd,CR_FFT_MANU_CTRL,0x0AA00000);//3
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_0,0x02000000);//4
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_DBGCTRL,0x00000030);//5
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_3,0x80680008);//6
		
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_5,0x00000597);//6
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_10,0x09400000);//7
		RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);//8
		RTMP_IO_WRITE32(pAd,CR_TXFD_0,0x00530000);//9
		RTMP_IO_WRITE32(pAd,CR_TXFD_3,0x01800000);//10
		RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x31000002);//11
			
	}
	else if (CONTI_CCK == iTestType)
	{
		RTMP_IO_WRITE32(pAd,R_WF_TMAC_PCTSR,0x80000000);//0
		RTMP_IO_WRITE32(pAd,CR_PHYCK_CTRL,0x00000038);//1
		RTMP_IO_WRITE32(pAd,CR_FR_CKG_CTRL,0x00000038);//2
		RTMP_IO_WRITE32(pAd,CR_FFT_MANU_CTRL,0x0AA00000);//3
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_0,0x00000000);//4
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_DBGCTRL,0x00000030);//5
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_3,0x80680008);//6
		
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_5,0x00000597);//6
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_10,0x0940E400);//7
		RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);//8
		RTMP_IO_WRITE32(pAd,CR_TXFD_0,0x00530000);//9
		RTMP_IO_WRITE32(pAd,CR_TXFD_3,0x00600000);//10
		RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x30000008);//11
			
	}
	else if (CONTI_CCK_PI2_ROTATE == iTestType)
	{		
		RTMP_IO_WRITE32(pAd,R_WF_TMAC_PCTSR,0x80000000);//0
		RTMP_IO_WRITE32(pAd,CR_PHYCK_CTRL,0x00000038);//1
		RTMP_IO_WRITE32(pAd,CR_FR_CKG_CTRL,0x00000038);//2
		RTMP_IO_WRITE32(pAd,CR_FFT_MANU_CTRL,0x0AA00000);//3
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_0,0x00000000);//4
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_DBGCTRL,0x00000030);//5
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_3,0x80680008);//6
		
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_5,0x00000597);//6
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_10,0x0940E400);//7
		RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);//8
		RTMP_IO_WRITE32(pAd,CR_TXFD_0,0x00500000);//9
		RTMP_IO_WRITE32(pAd,CR_TXFD_3,0x00600000);//10
		RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x40000008);//11
	}
	else if (CONTI_CCK_PI1_ROTATE == iTestType)
	{
		RTMP_IO_WRITE32(pAd,R_WF_TMAC_PCTSR,0x80000000);//0
		RTMP_IO_WRITE32(pAd,CR_PHYCK_CTRL,0x00000038);//1
		RTMP_IO_WRITE32(pAd,CR_FR_CKG_CTRL,0x00000038);//2
		RTMP_IO_WRITE32(pAd,CR_FFT_MANU_CTRL,0x0AA00000);//3
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_0,0x00000000);//4
		RTMP_IO_WRITE32(pAd,CR_PHYCTRL_DBGCTRL,0x00000030);//5
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_3,0x80680008);//6
		
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_5,0x00000597);//6
		RTMP_IO_WRITE32(pAd,CR_PHYMUX_10,0x0940E400);//7
		RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);//8
		RTMP_IO_WRITE32(pAd,CR_TXFD_0,0x00500000);//9
		RTMP_IO_WRITE32(pAd,CR_TXFD_3,0x00600000);//10
		RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x50000008);//11			
	}
	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));	
}


/**
 * Stop TX Tone Test
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * 
 *
 * @return If the function succeeds, the return value is FSAL_OK.
 * Otherwise, an error code is returned.
 */
void StopTXToneTest(RTMP_ADAPTER *pAd)
{	
	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));

	RTMP_IO_WRITE32(pAd,CR_PHYCK_CTRL,0x00000000);
	RTMP_IO_WRITE32(pAd,CR_FR_CKG_CTRL,0x00000000);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x0D00,0x80274027);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x0D04,0xC0000800);
	RTMP_IO_WRITE32(pAd,CR_PHYMUX_3,0x00000008);
	RTMP_IO_WRITE32(pAd,CR_PHYMUX_5,0x00000580);
	RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x00000000);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x0D24,0x00000000);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x0A08,0x00000000);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A0,0x00000000);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A4,0x00000000);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x020C,0x00000000);
	RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x00000000);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x090C,0x20300604);
	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));
}


/**
 * Start TX Tone Test
 *
 * @param pstFSAL pointer to FSAL structure.
 * @param uOffset offsets in unit of bytes.
 *
 * @return If the function succeeds, the return value is FSAL_OK.
 * Otherwise, an error code is returned.
 */
void StartTXToneTest(RTMP_ADAPTER *pAd,int iTestType)
{
	DBGPRINT(RT_DEBUG_TRACE,("%s iTestType%d-->\n",__FUNCTION__,iTestType));

	StopTXToneTest(pAd);
	DBGPRINT(RT_DEBUG_TRACE,("%s Start-->\n",__FUNCTION__));
	//CLock Setup
	RTMP_IO_WRITE32(pAd,CR_PHYCK_CTRL,0x00000021);
	RTMP_IO_WRITE32(pAd,CR_FR_CKG_CTRL,0x00000021);
	
	//TXSetup	
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x0D00,0x00274027);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x0D04,0xC0000400);
	RTMP_IO_WRITE32(pAd,CR_PHYMUX_3,0x80000008);
	RTMP_IO_WRITE32(pAd,CR_PHYMUX_5,0x00000597);
	RTMP_IO_WRITE32(pAd,CR_TXFD_1,0x10000000);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x0D24,0x60000000);
	RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x0A08,0xF0000000);	
	
	//Tone Gen
	if (CONTI_5M_TONE == iTestType)
	{
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A0,0x000C100C);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A4,0x00000000);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x020C,0x00010122);
				RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);
	}
	else if (CONTI_10M_TONE == iTestType)
	{
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A0,0x000C101C);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A4,0x00000000);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x020C,0x00010122);
				RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);
	}
	else if (CONTI_20M_TONE == iTestType)
	{
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A0,0x000C102C);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A4,0x00000000);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x020C,0x00010122);
				RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);
	}
	else if (CONTI_RAMP == iTestType)
	{
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A0,0x0009200C);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A4,0x00000000);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x090C,0x30300604);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x020C,0x00010122);
				RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);
	}
	else if (CONTI_DC == iTestType)
	{
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A0,0x000C1048);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x01A4,0x06000600);
				RTMP_IO_WRITE32(pAd,WIFI_PHY_BASE+0x020C,0x00010122);
				RTMP_IO_WRITE32(pAd,CR_TXFE_4,0x000000C0);		
	}
	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));
	
}


/**
 * Set AIFS value, 0 is not allowed.
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param iSetType, which queue you want to set.
 * @param iSetValue, which AIFS value you want to set.
 *
 * @return false If the function succeeds, the return value is true.
 * Otherwise, an error code is returned.
 */
 BOOLEAN SetAIFS(RTMP_ADAPTER *pAd,ULONG iSetType,ULONG iSetValue)
{
		ULONG  AIFSsetValue = 0;
		ULONG RandomsetValue = 0;
		ULONG Temp = 0;

		DBGPRINT(RT_DEBUG_TRACE,("%s iSetType=%d iSetValue=0x%x<--\n",__FUNCTION__,iSetType,iSetValue));
	
		if (0 == iSetValue)
			return FALSE;//0 is not allowed			
			
		AIFSsetValue = iSetValue/9;
		//Set DRNGR
		if(AIFSsetValue>15)
		{
				RandomsetValue = (1<<16);
				RandomsetValue |= ((iSetValue - 15*9)/9);
				if(RandomsetValue>0x0000FFFF)
					RandomsetValue = 0x0001FFFF;//max value, 1 is FIXED
					
				RTMP_IO_WRITE32(pAd,DRNGR,RandomsetValue);	
				AIFSsetValue = 15;
		}
		else
		{
				RTMP_IO_WRITE32(pAd,DRNGR,0x00000000);				
		}			
		
		//Set AIFSR0 or AIFSR1
		if(iSetType<=AIFS_BMC)
		{
				Temp = AIFSsetValue<<(iSetType*4);
				RTMP_IO_WRITE32(pAd,AIFSR0,Temp);	
		}
		else
		{
				Temp = AIFSsetValue<<((iSetType-AIFS_10)*4);
				RTMP_IO_WRITE32(pAd,AIFSR1,Temp);	
		}	
		DBGPRINT(RT_DEBUG_TRACE,("%s<--\n",__FUNCTION__));
		
		return TRUE;
}


/**
 * Set Tx Power Range
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param ucMaxPowerDbm, Max Power Dbm
 * @param ucMinPowerDbm, Min Power Dbm
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void TxConfigPowerRange(RTMP_ADAPTER *pAd, IN UCHAR ucMaxPowerDbm,IN UCHAR ucMinPowerDbm)
{
    ULONG u4RegValue;

	DBGPRINT(RT_DEBUG_TRACE,("%s-->\n",__FUNCTION__));
	
	RTMP_IO_READ32(pAd,TMAC_FPCR,&u4RegValue);    

    u4RegValue &= ~(FPCR_FRAME_POWER_MAX_DBM | FPCR_FRAME_POWER_MIN_DBM);
    u4RegValue |= ((ucMaxPowerDbm << FPCR_FRAME_POWER_MAX_DBM_OFFSET) & FPCR_FRAME_POWER_MAX_DBM);
    u4RegValue |= ((ucMinPowerDbm << FPCR_FRAME_POWER_MIN_DBM_OFFSET) & FPCR_FRAME_POWER_MIN_DBM);

    RTMP_IO_WRITE32(pAd,TMAC_FPCR,u4RegValue);	
	DBGPRINT(RT_DEBUG_TRACE,("%s<--\n",__FUNCTION__));
}


/**
 * Tx Set FrameP ower, this base dbm in TX power.
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param eModulationSystem, MODULATION_SYSTEM_CCK,MODULATION_SYSTEM_OFDM,...
 * @param eModulationType, MODULATION_TYPE_CCK_BPSK, MODULATION_TYPE_DSSS_QPSK...
 * @param	ucFramePowerDbm, power dmb. ####This value is 2's complement.####
*ex:
 *0000000: 0 dBm
 *0000001: 0.5 dBm
 *0111111: 31.5dBm
 *1111111: -0.5dBm
 *1000000: -32 dBm
 * @return void
 * Otherwise, an error code is returned.
 */
void TxSetFramePower (RTMP_ADAPTER *pAd, ULONG eModulationSystem, ULONG eModulationType,UCHAR ucFramePowerDbm)
{
	ULONG u4PowerBaseReg = 0, u4PowerBaseReg2 = 0;
    	//ULONG Temp = 0;
	ULONG CRValue = 0;
	UCHAR bit6 = 0;
	ULONG PowDelta = 0;// eeprom 50
	UCHAR bit6PowDelta = 0;

	ULONG Temp = 0;
	ULONG Lo0 = 0,Lo1 = 0,Lo2 = 0,Lo3 = 0;

	DBGPRINT(RT_DEBUG_TRACE,("%s--> eModulationSystem=%d,eModulationType=%d,ucFramePowerDbm=0x%x\n",
				__FUNCTION__,eModulationSystem,eModulationType,ucFramePowerDbm));
#if 0
	Temp |= ucFramePowerDbm;
	Temp |= (ucFramePowerDbm<<8);
	Temp |= (ucFramePowerDbm<<16);
	
	RTMP_IO_WRITE32(pAd,TMAC_FP0R6,Temp);
	RTMP_IO_WRITE32(pAd,TMAC_FP0R7,Temp);
	RTMP_IO_WRITE32(pAd,TMAC_FP0R8,Temp);

	Temp |= (ucFramePowerDbm<<24);
	RTMP_IO_WRITE32(pAd,TMAC_FP0R0,Temp);
	RTMP_IO_WRITE32(pAd,TMAC_FP0R1,Temp);
	RTMP_IO_WRITE32(pAd,TMAC_FP0R2,Temp);
	RTMP_IO_WRITE32(pAd,TMAC_FP0R3,Temp);
	RTMP_IO_WRITE32(pAd,TMAC_FP0R4,Temp);
	RTMP_IO_WRITE32(pAd,TMAC_FP0R5,Temp);
#endif


	RTMP_IO_READ32(pAd,TMAC_FP0R3,&CRValue);
	CRValue &= (~0x00007F00);//clear bit 8~14
	ucFramePowerDbm &=0x7F; //clear bit7 to use 0~6 bit only
	DBGPRINT(RT_DEBUG_TRACE,("6013002C[14:8] = 0x%x\n",ucFramePowerDbm));
	CRValue |= (ucFramePowerDbm <<8);// shift 8
	RTMP_IO_WRITE32(pAd,TMAC_FP0R3,CRValue);


	//TMAC_FP0R0
	CRValue = 0;
	//RTMP_IO_READ32_HAL(pDeviceObject,TMAC_FP0R0,&CRValue);
	//CRValue &= (~0x80808080);// reserve bit7, 15, 23, 31
	DBGPRINT(RT_DEBUG_TRACE,("temp  CRValue begin= 0x%x\n",CRValue));
	//60130020 [6:0] = 6013002C[14:8] + eeprom_A2[7]*((eeprom_A2[6]==1)*2 - 1)* eeprom_A2[5:0]
	if((pAd->EfuseContent[0xA2] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	DBGPRINT(RT_DEBUG_TRACE,("EfuseContent[0xA2]= 0x%x\n",pAd->EfuseContent[0xA2]));
	DBGPRINT(RT_DEBUG_TRACE,("EfuseContent[0xA2]>>7= 0x%x\n",pAd->EfuseContent[0xA2]>>7));
	DBGPRINT(RT_DEBUG_TRACE,("bit6= 0x%x\n",bit6));
	CRValue |= (0x0000007F & (ucFramePowerDbm + (pAd->EfuseContent[0xA2] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA2] & 0x3F)));
	DBGPRINT(RT_DEBUG_TRACE,("60130020 [6:0]  CRValue = 0x%x\n",CRValue));
	Lo0 = (ucFramePowerDbm + (pAd->EfuseContent[0xA2] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA2] & 0x3F));



	DBGPRINT(RT_DEBUG_TRACE,("60130020 [6:0] = 0x%x\n",(ucFramePowerDbm + (pAd->EfuseContent[0xA2] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA2] & 0x3F))));
	//60130020 [14:8] = 6013002C[14:8] + eeprom_A3[7]* ((eeprom_A3[6]==1)*2 - 1)* eeprom_A3[5:0]
	//LG 12M/18M Target power =   LG 54M Target power + TX rate delta of 12M/18M (read from eeprom:0xA3)
	if((pAd->EfuseContent[0xA3] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x00007F00 & ((ucFramePowerDbm + (pAd->EfuseContent[0xA3] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA3] & 0x3F)) << 8));

	Lo1 = (ucFramePowerDbm + (pAd->EfuseContent[0xA3] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA3] & 0x3F));
	Lo1 = Lo1<<8; 

	DBGPRINT(RT_DEBUG_TRACE,("60130020 [14:8] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xA3] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA3] & 0x3F)) << 8)));
	//60130020 [22:16] = 6013002C[14:8] + eeprom_A4[7]* ((eeprom_A4[6]==1)*2 - 1)* eeprom_A4[5:0]
	//LG 24M/36M Target power =   LG 54M Target power + TX rate delta of 24M/36M(read from eeprom:0xA4)
	if((pAd->EfuseContent[0xA4] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x007F0000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xA4] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA4] & 0x3F)) << 16));
	Lo2 = (ucFramePowerDbm + (pAd->EfuseContent[0xA4] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA4] & 0x3F)) ;
	Lo2= Lo2<<16; 

	DBGPRINT(RT_DEBUG_TRACE,("60130020 [22:16] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xA4] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA4] & 0x3F)) << 16)));
	//60130020 [30:24] = 6013002C[14:8] + eeprom_A5[7]* ((eeprom_A5[6]==1)*2 - 1)* eeprom_A5[5:0]
	//LG 48M Target power =   LG 54M Target power + TX rate delta of 48M (read from eeprom:0xA5)
	if((pAd->EfuseContent[0xA5] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x7F000000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xA5] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA5] & 0x3F)) << 24));

	Lo3 = (ucFramePowerDbm + (pAd->EfuseContent[0xA5] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA5] & 0x3F));
	Lo3 = Lo3<<24; 

	DBGPRINT(RT_DEBUG_TRACE,("60130020 [30:24] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xA5] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA5] & 0x3F)) << 24)));
	DBGPRINT(RT_DEBUG_TRACE,("TMAC_FP0R0,CRValue 60130020 = 0x%x\n", CRValue));
	RTMP_IO_WRITE32(pAd,TMAC_FP0R0,CRValue);

	//TMAC_FP0R1
	CRValue = 0;
	//RTMP_IO_READ32(pAd,TMAC_FP0R1,&CRValue);
	CRValue &= (~0x80808080);// reserve bit7, 15, 23, 31
	
	//
	//HT 20M0 Target power =   LG 54M Target power + TX rate delta of M0  (read from eeprom:0xA7)
	//60130024 [6:0] = 6013002C[14:8] + eeprom_A7[7]* ((eeprom_A7[6]==1)*2 - 1)* eeprom_A7[5:0]
	if((pAd->EfuseContent[0xA7] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x0000007F & (ucFramePowerDbm + (pAd->EfuseContent[0xA7] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA7] & 0x3F)));
	DBGPRINT(RT_DEBUG_TRACE,("60130024 [6:0] = 0x%x\n",(ucFramePowerDbm + (pAd->EfuseContent[0xA7] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA7] & 0x3F))));
	//HT20 M1/M2 Target power =   LG 54M Target power + TX rate delta of M1/M2  (read from eeprom:0xA9)
	//60130024 [14:8] = 6013002C[14:8] + eeprom_A9[7]* ((eeprom_A9[6]==1)*2 - 1)* eeprom_A9[5:0]
	if((pAd->EfuseContent[0xA9] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x00007F00 & ((ucFramePowerDbm + (pAd->EfuseContent[0xA9] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA9] & 0x3F)) << 8));	
	DBGPRINT(RT_DEBUG_TRACE,("60130024 [14:8] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xA9] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA9] & 0x3F)) << 8)));
	//HT20 M3/M4 Target power =   LG 54M Target power + TX rate delta of M3/M4  (read from eeprom:0xAA)
	//60130024 [22:16] = 6013002C[14:8] + eeprom_AA[7]* ((eeprom_AA[6]==1)*2 - 1)* eeprom_AA[5:0]
	if((pAd->EfuseContent[0xAA] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x007F0000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xAA] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAA] & 0x3F)) << 16));	
	DBGPRINT(RT_DEBUG_TRACE,("60130024 [22:16] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xAA] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAA] & 0x3F)) << 16)));
	//HT20 M5 Target power =   LG 54M Target power + TX rate delta of M5  (read from eeprom:0xAB)
	//60130024 [30:24] = 6013002C[14:8] + eeprom_AB[7]* ((eeprom_AB[6]==1)*2 - 1)* eeprom_AB[5:0]

	if((pAd->EfuseContent[0xAB] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x7F000000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xAB] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAB] & 0x3F)) << 24));
	DBGPRINT(RT_DEBUG_TRACE,("60130024 [30:24] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xAA] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAA] & 0x3F)) << 24)));
	DBGPRINT(RT_DEBUG_TRACE,("TMAC_FP0R1,CRValue 60130024 = 0x%x\n", CRValue));	
	RTMP_IO_WRITE32(pAd,TMAC_FP0R1,CRValue);

	//TMAC_FP0R3
	//HT20 M32 Target power =   LG 54M Target power + TX rate delta of M32  (read from eeprom:0xA8) 6013002C
	//6013002C [30:24] = 6013002C[14:8] + eeprom_A8[7]*eeprom_A8[6]* eeprom_A8[5:0]*0.5
	CRValue = 0;
	//RTMP_IO_READ32(pDeviceObject,TMAC_FP0R3,&CRValue);
	CRValue &= (~0x80808080);// reserve bit7, 15, 23, 31
	//OFDM4
	//6013002C [14:8]
	CRValue |= (0x00007F00 & (ucFramePowerDbm <<8));
	if((pAd->EfuseContent[0xA8] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x7F000000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xA8] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA8] & 0x3F)) << 24));
	DBGPRINT(RT_DEBUG_TRACE,("6013002C [30:24] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xA8] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA8] & 0x3F)) << 24)));
	//CCK 1M/2M Target power = LG 54M Target power + TX rate delta of 1M/2M  (read from eeprom:0xA0)
	//6013002C [6:0] = 6013002C[14:8] + eeprom_A0[7]* ((eeprom_A0[6]==1)*2 - 1)* eeprom_A0[5:0]
	if((pAd->EfuseContent[0xA0] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x0000007F & (ucFramePowerDbm + (pAd->EfuseContent[0xA0] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA0] & 0x3F)));
	DBGPRINT(RT_DEBUG_TRACE,("6013002C [6:0] = 0x%x\n",(ucFramePowerDbm + (pAd->EfuseContent[0xA0] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA0] & 0x3F)) ));
	//CCK 5.5M/11M Target power = LG 54M Target power + TX rate delta of 5.5M/11M  (read from eeprom:0xA1)
	//6013002C [22:16] = 6013002C[14:8] + eeprom_A1[7]* ((eeprom_A1[6]==1)*2 - 1)* eeprom_A1[5:0]
	if((pAd->EfuseContent[0xA1] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x007F0000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xA1] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA1] & 0x3F))<< 16));
	DBGPRINT(RT_DEBUG_TRACE,("6013002C [22:16] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xA1] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA1] & 0x3F)) << 16 )));
	DBGPRINT(RT_DEBUG_TRACE,("TMAC_FP0R3,CRValue 6013002C = 0x%x\n", CRValue));
	RTMP_IO_WRITE32(pAd,TMAC_FP0R3,CRValue);
	//TMAC_FP0R4==================
	CRValue = 0;
	//RTMP_IO_READ32_HAL(pAd,TMAC_FP0R4,&CRValue);
	CRValue &= (~0x80808080);// reserve bit7, 15, 23, 31
	//HT20 M6 Target power =   LG 54M Target power + TX rate delta of M6  (read from eeprom:0xAC)
	//60130030 [6:0] = 6013002C[14:8] + eeprom_AC[7]* ((eeprom_AC[6]==1)*2 - 1)* eeprom_AC[5:0]
	if((pAd->EfuseContent[0xAC] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x0000007F & (ucFramePowerDbm + (pAd->EfuseContent[0xAC] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAC] & 0x3F)));
	DBGPRINT(RT_DEBUG_TRACE,("60130030 [6:0] = 0x%x\n",(ucFramePowerDbm + (pAd->EfuseContent[0xA1] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA1] & 0x3F)) ));
	//HT20 M7 Target power = LG 54M Target power + TX rate delta of M7  (read from eeprom:0xAD)
	//60130030 [14:8] = 6013002C[14:8] + eeprom_AD[7]* ((eeprom_AD[6]==1)*2 - 1)* eeprom_AD[5:0]
	if((pAd->EfuseContent[0xAD] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x00007F00 & ((ucFramePowerDbm + (pAd->EfuseContent[0xAD] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAD] & 0x3F)) << 8));		
	DBGPRINT(RT_DEBUG_TRACE,("60130030 [14:8] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xAD] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAD] & 0x3F)) << 8 )));
	//HT40
	//HT40 pow delta
	if((pAd->EfuseContent[0x50] & 0x40) == 0x40)
	{
		bit6PowDelta = 1;
	}
	else
	{
		bit6PowDelta = 0;
	}
	PowDelta = (pAd->EfuseContent[0x50] >> 7) * (bit6PowDelta*2 - 1)* (pAd->EfuseContent[0x50] & 0x3F);	
	DBGPRINT(RT_DEBUG_TRACE,( "HT40  PowDelta = 0x%x\n", PowDelta));
	//HT40 M6 A1
	//60130030 [22:16] 
	if((pAd->EfuseContent[0xA1] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x007F0000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xA1] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA1] & 0x3F)+ PowDelta) <<16) )  ;	
	DBGPRINT(RT_DEBUG_TRACE,("60130030 [22:16] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xA1] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA1] & 0x3F)+ PowDelta) <<16)) );
	//HT40 M7 AD
	//60130030 [30:24] 
	if((pAd->EfuseContent[0xAD] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x7F000000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xAD] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAD] & 0x3F)+ PowDelta)<< 24)) ;
	DBGPRINT(RT_DEBUG_TRACE,("60130030 [30:24] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xAD] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAD] & 0x3F)+ PowDelta) <<24)) );
	DBGPRINT(RT_DEBUG_TRACE,("TMAC_FP0R4,CRValue 60130030 = 0x%x\n", CRValue));
	RTMP_IO_WRITE32(pAd,TMAC_FP0R4,CRValue);


	//TMAC_FP0R2 HT40
	CRValue = 0;
	//RTMP_IO_READ32_HAL(pDeviceObject,TMAC_FP0R2,&CRValue);
	CRValue &= (~0x80808080);// reserve bit7, 15, 23, 31
	
	
	
	
	//HT40 M0 
	//60130024 [6:0]
	if((pAd->EfuseContent[0xA7] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x0000007F & ((ucFramePowerDbm + (pAd->EfuseContent[0xA7] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA7] & 0x3F))) + PowDelta) ;	
	DBGPRINT(RT_DEBUG_TRACE,("60130028 [6:0] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xA7] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA7] & 0x3F)))+ PowDelta));

	//HT40 M1/M2 A9 
	//60130024 [14:8]
	if((pAd->EfuseContent[0xA9] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x00007F00 &  ((ucFramePowerDbm + (pAd->EfuseContent[0xA9] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA9] & 0x3F) + PowDelta)<< 8)) ;
	DBGPRINT(RT_DEBUG_TRACE,("60130028 [30:24] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xA9] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xA9] & 0x3F)+ PowDelta) <<8)) );

	//HT40 M3/M4 AA 
	//60130024 [22:16]
	if((pAd->EfuseContent[0xAA] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x007F0000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xAA] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAA] & 0x3F) + PowDelta)<<16)) ;	
	DBGPRINT(RT_DEBUG_TRACE,("60130028 [22:16] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xAA] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAA] & 0x3F)+ PowDelta) <<16)) );
	//HT40 M5 AB 
	//60130024 [30:24]
	if((pAd->EfuseContent[0xAB] & 0x40) == 0x40)
	{
		bit6 = 1;
	}
	else
	{
		bit6 = 0;
	}
	CRValue |= (0x7F000000 & ((ucFramePowerDbm + (pAd->EfuseContent[0xAB] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAB] & 0x3F) + PowDelta)<<24)) ;
	DBGPRINT(RT_DEBUG_TRACE,("60130028 [30:24] = 0x%x\n",((ucFramePowerDbm + (pAd->EfuseContent[0xAB] >> 7) * (bit6*2 - 1)* (pAd->EfuseContent[0xAB] & 0x3F)+ PowDelta) <<24)) );
	DBGPRINT(RT_DEBUG_TRACE,("TMAC_FP0R2, CRValue 60130028 = 0x%x\n", CRValue));
	RTMP_IO_WRITE32(pAd,TMAC_FP0R2,CRValue);
	

	
/*    do 
	{
            // FRAME_POWER_0 
            u4PowerBaseReg = TMAC_FP0R0;
            u4PowerBaseReg2 = TMAC_FP0R4;

        switch (eModulationSystem) 
		{
            case MODULATION_SYSTEM_CCK:    // CCK, OFDM 54.
            	DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_SYSTEM_CCK \n",__FUNCTION__));
                u4PowerBaseReg = u4PowerBaseReg + 12;
                break;

            case MODULATION_SYSTEM_OFDM: // Other OFDM.
            	DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_SYSTEM_OFDM \n",__FUNCTION__));
                if (eModulationType == MODULATION_TYPE_54M_MCS7) {
                    u4PowerBaseReg = u4PowerBaseReg + 12;
                }
                else {
                    u4PowerBaseReg = u4PowerBaseReg;
                }
                break;

            case MODULATION_SYSTEM_HT20: // HT20
	            DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_SYSTEM_HT20 \n",__FUNCTION__));
                u4PowerBaseReg = u4PowerBaseReg + 4;
                break;

            case MODULATION_SYSTEM_HT40:  // HT 40
            	DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_SYSTEM_HT40 \n",__FUNCTION__));
                if (eModulationType == MODULATION_TYPE_MCS32) {
                    u4PowerBaseReg = u4PowerBaseReg + 12;
                }
                else {
                    u4PowerBaseReg = u4PowerBaseReg + 8;
                }
                break;

            default:
                break;
        }

        switch (eModulationType) 
		{
            case MODULATION_TYPE_CCK_BPSK:
				DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_TYPE_CCK_BPSK \n",__FUNCTION__));
                if (eModulationSystem == MODULATION_SYSTEM_CCK) 
		  {
		      RTMP_IO_READ32(pAd, u4PowerBaseReg,&Temp);
		      Temp &= ~(FP0R3_CCK1_FRAME_POWER0_DBM);
		      Temp |= (ucFramePowerDbm << FP0R3_CCK1_FRAME_POWER0_DBM_OFFSET) & FP0R3_CCK1_FRAME_POWER0_DBM;
		      RTMP_IO_WRITE32(pAd,u4PowerBaseReg,Temp);
		      //HAL_REG_32(u4PowerBaseReg) &= ~(FP0R3_CCK1_FRAME_POWER0_DBM);
                    //HAL_REG_32(u4PowerBaseReg) |= (ucFramePowerDbm << FP0R3_CCK1_FRAME_POWER0_DBM_OFFSET) & FP0R3_CCK1_FRAME_POWER0_DBM;
                    break;
                }
   	         RTMP_IO_READ32(pAd, u4PowerBaseReg,&Temp);
		  Temp &= ~(FPR_BPSK_FRAME_POWER_DBM);
		  Temp |= (ucFramePowerDbm << FPR_BPSK_FRAME_POWER_DBM_OFFSET) & FPR_BPSK_FRAME_POWER_DBM;
		  RTMP_IO_WRITE32(pAd,u4PowerBaseReg,Temp);
                //HAL_REG_32(u4PowerBaseReg) &= ~(FPR_BPSK_FRAME_POWER_DBM);
                //HAL_REG_32(u4PowerBaseReg) |= (ucFramePowerDbm << FPR_BPSK_FRAME_POWER_DBM_OFFSET) & FPR_BPSK_FRAME_POWER_DBM;
                break;

            case MODULATION_TYPE_DSSS_QPSK:
				DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_TYPE_DSSS_QPSK \n",__FUNCTION__));
                if (eModulationSystem == MODULATION_SYSTEM_CCK) 
		  {
		       RTMP_IO_READ32(pAd, u4PowerBaseReg,&Temp);
		  	Temp &= ~(FP0R3_CCK0_FRAME_POWER0_DBM);
		  	Temp |= (ucFramePowerDbm) & FP0R3_CCK0_FRAME_POWER0_DBM;
		  	RTMP_IO_WRITE32(pAd,u4PowerBaseReg,Temp);
                    //HAL_REG_32(u4PowerBaseReg) &= ~(FP0R3_CCK0_FRAME_POWER0_DBM);
                    //HAL_REG_32(u4PowerBaseReg) |= (ucFramePowerDbm) & FP0R3_CCK0_FRAME_POWER0_DBM;
                    break;
                }
		   RTMP_IO_READ32(pAd, u4PowerBaseReg,&Temp);
		   Temp &= ~(FPR_QPSK_FRAME_POWER_DBM);
		   Temp |= ((ucFramePowerDbm << FPR_QPSK_FRAME_POWER_DBM_OFFSET) & FPR_QPSK_FRAME_POWER_DBM);
		   RTMP_IO_WRITE32(pAd,u4PowerBaseReg,Temp);
                //HAL_REG_32(u4PowerBaseReg) &= ~(FPR_QPSK_FRAME_POWER_DBM);
                //HAL_REG_32(u4PowerBaseReg) |= ((ucFramePowerDbm << FPR_QPSK_FRAME_POWER_DBM_OFFSET) & FPR_QPSK_FRAME_POWER_DBM);
                break;

            case MODULATION_TYPE_16QAM:
				DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_TYPE_16QAM \n",__FUNCTION__));
                if (eModulationSystem == MODULATION_SYSTEM_CCK) 
		  {
                    break;
                }
		   RTMP_IO_READ32(pAd, u4PowerBaseReg,&Temp);
		   Temp &= ~(FPR_16_QAM_FRAME_POWER_DBM);
		   Temp |= (ucFramePowerDbm << FPR_16_QAM_FRAME_POWER_DBM_OFFSET) & FPR_16_QAM_FRAME_POWER_DBM;
		   RTMP_IO_WRITE32(pAd,u4PowerBaseReg,Temp);
                //HAL_REG_32(u4PowerBaseReg) &= ~(FPR_16_QAM_FRAME_POWER_DBM);
                //HAL_REG_32(u4PowerBaseReg) |= (ucFramePowerDbm << FPR_16_QAM_FRAME_POWER_DBM_OFFSET) & FPR_16_QAM_FRAME_POWER_DBM;
                break;

            case MODULATION_TYPE_64QAM:
				DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_TYPE_64QAM \n",__FUNCTION__));
                if (eModulationSystem == MODULATION_SYSTEM_CCK) 
		  {
                    break;
                }
		   RTMP_IO_READ32(pAd, u4PowerBaseReg,&Temp);
		   Temp &= ~(FPR_64_QAM_FRAME_POWER_DBM);
		   Temp |= ((ucFramePowerDbm << FPR_64_QAM_FRAME_POWER_DBM_OFFSET) & FPR_64_QAM_FRAME_POWER_DBM);
		   RTMP_IO_WRITE32(pAd,u4PowerBaseReg,Temp);
                //HAL_REG_32(u4PowerBaseReg) &= ~(FPR_64_QAM_FRAME_POWER_DBM);
                //HAL_REG_32(u4PowerBaseReg) |= ((ucFramePowerDbm << FPR_64_QAM_FRAME_POWER_DBM_OFFSET) & FPR_64_QAM_FRAME_POWER_DBM);
                break;

            case MODULATION_TYPE_MCS6:
				DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_SYSTEM_CCK \n",__FUNCTION__));
                switch (eModulationSystem) 
		{
                case MODULATION_SYSTEM_HT20:
		     RTMP_IO_READ32(pAd, u4PowerBaseReg2,&Temp);
		     Temp &= ~(FPR_BPSK_FRAME_POWER_DBM);
		     Temp |= (ucFramePowerDbm << FPR_BPSK_FRAME_POWER_DBM_OFFSET) & FPR_BPSK_FRAME_POWER_DBM;
		     RTMP_IO_WRITE32(pAd,u4PowerBaseReg2,Temp);
                    //HAL_REG_32(u4PowerBaseReg2) &= ~(FPR_BPSK_FRAME_POWER_DBM);
                    //HAL_REG_32(u4PowerBaseReg2) |= (ucFramePowerDbm << FPR_BPSK_FRAME_POWER_DBM_OFFSET) & FPR_BPSK_FRAME_POWER_DBM;
                    break;

                case MODULATION_SYSTEM_HT40:					
		      RTMP_IO_READ32(pAd, u4PowerBaseReg2,&Temp);
		      Temp &= ~(FPR_16_QAM_FRAME_POWER_DBM);
		      Temp |= (ucFramePowerDbm << FPR_16_QAM_FRAME_POWER_DBM_OFFSET) & FPR_16_QAM_FRAME_POWER_DBM;
		      RTMP_IO_WRITE32(pAd,u4PowerBaseReg2,Temp);
                    //HAL_REG_32(u4PowerBaseReg2) &= ~(FPR_16_QAM_FRAME_POWER_DBM);
                    //HAL_REG_32(u4PowerBaseReg2) |= (ucFramePowerDbm << FPR_16_QAM_FRAME_POWER_DBM_OFFSET) & FPR_16_QAM_FRAME_POWER_DBM;
                    break;

                default:
                    break;
                }
                break;

            case MODULATION_TYPE_54M_MCS7:
				DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_TYPE_54M_MCS7 \n",__FUNCTION__));
                switch (eModulationSystem) 
		 {
                case MODULATION_SYSTEM_HT20:
		      RTMP_IO_READ32(pAd, u4PowerBaseReg2,&Temp);
		      Temp &= ~(FPR_QPSK_FRAME_POWER_DBM);
		      Temp |= ((ucFramePowerDbm << FPR_QPSK_FRAME_POWER_DBM_OFFSET) & FPR_QPSK_FRAME_POWER_DBM);
		      RTMP_IO_WRITE32(pAd,u4PowerBaseReg2,Temp);
                    //HAL_REG_32(u4PowerBaseReg2) &= ~(FPR_QPSK_FRAME_POWER_DBM);
                    //HAL_REG_32(u4PowerBaseReg2) |= ((ucFramePowerDbm << FPR_QPSK_FRAME_POWER_DBM_OFFSET) & FPR_QPSK_FRAME_POWER_DBM);
                    break;

                case MODULATION_SYSTEM_HT40:
		      RTMP_IO_READ32(pAd, u4PowerBaseReg2,&Temp);
		      Temp &= ~(FPR_64_QAM_FRAME_POWER_DBM);
		      Temp |= ((ucFramePowerDbm << FPR_64_QAM_FRAME_POWER_DBM_OFFSET) & FPR_64_QAM_FRAME_POWER_DBM);
		      RTMP_IO_WRITE32(pAd,u4PowerBaseReg2,Temp);
                    //HAL_REG_32(u4PowerBaseReg2) &= ~(FPR_64_QAM_FRAME_POWER_DBM);
                    //HAL_REG_32(u4PowerBaseReg2) |= ((ucFramePowerDbm << FPR_64_QAM_FRAME_POWER_DBM_OFFSET) & FPR_64_QAM_FRAME_POWER_DBM);
                    break;

                case MODULATION_SYSTEM_OFDM:
		      RTMP_IO_READ32(pAd, u4PowerBaseReg,&Temp);
		      Temp &= ~(FPR_QPSK_FRAME_POWER_DBM);
		      Temp |= ((ucFramePowerDbm << FPR_QPSK_FRAME_POWER_DBM_OFFSET) & FPR_QPSK_FRAME_POWER_DBM);
		      RTMP_IO_WRITE32(pAd,u4PowerBaseReg,Temp);
                    //HAL_REG_32(u4PowerBaseReg) &= ~(FPR_QPSK_FRAME_POWER_DBM);
                    //HAL_REG_32(u4PowerBaseReg) |= ((ucFramePowerDbm << FPR_QPSK_FRAME_POWER_DBM_OFFSET) & FPR_QPSK_FRAME_POWER_DBM);
                    break;

                default:
                    break;
                }
                break;

            case MODULATION_TYPE_MCS32:
				DBGPRINT(RT_DEBUG_TRACE,("%s MODULATION_TYPE_MCS32 \n",__FUNCTION__));
                switch (eModulationSystem) 
		 {
                case MODULATION_SYSTEM_HT40:
		      RTMP_IO_READ32(pAd, u4PowerBaseReg,&Temp);
		      Temp &= ~(FPR_64_QAM_FRAME_POWER_DBM);
		      Temp |= ((ucFramePowerDbm << FPR_64_QAM_FRAME_POWER_DBM_OFFSET) & FPR_64_QAM_FRAME_POWER_DBM);
		      RTMP_IO_WRITE32(pAd,u4PowerBaseReg,Temp);
                    //HAL_REG_32(u4PowerBaseReg) &= ~(FPR_64_QAM_FRAME_POWER_DBM);
                    //HAL_REG_32(u4PowerBaseReg) |= ((ucFramePowerDbm << FPR_64_QAM_FRAME_POWER_DBM_OFFSET) & FPR_64_QAM_FRAME_POWER_DBM);
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
        }

    } while (FALSE);
*/
    return;
}


/**
 * Tx Set Frequency Offset
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param iOffset value
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void TxSetFrequencyOffset (RTMP_ADAPTER *pAd, ULONG iOffset)
{
	ULONG Value = 0;

	DBGPRINT(RT_DEBUG_TRACE,("%s iOffset=0x%x-->\n",__FUNCTION__,iOffset));

	if(pAd->IsTxSetFrequency == FALSE)
	{
		//RG_XO_C2[8:14]  Set 60 (0x3C )at first
		RTMP_IO_READ32(pAd,RG_XO_C2,&Value);		
		Value = (Value & 0xFFFF80FF) | (0x3C << 8);
		RTMP_IO_WRITE32(pAd,RG_XO_C2,Value);
		//RG_XO_C2_MANUAL [8:14]  Set 0x7F at first
		RTMP_IO_READ32(pAd,RG_XO_C2_MANUAL,&Value);
		Value = (Value & 0xFFFF80FF) | (0x7F << 8);
		RTMP_IO_WRITE32(pAd,RG_XO_C2_MANUAL,Value);
		//only set at first time
		pAd->IsTxSetFrequency = TRUE;
	}

	//RG_XO_C2[8:14] set the offset by user intend	
	RTMP_IO_READ32(pAd,RG_XO_C2,&Value);	
	Value = (Value & 0xFFFF80FF) | (iOffset << 8);
	RTMP_IO_WRITE32(pAd,RG_XO_C2,Value);

#if 0	
	if (iOffset>127)
	{
		temp = 127;//Max value is 127, 7bits
	}
	else
	{
		temp = iOffset;
	}

	RTMP_IO_READ32(pAd,XTAL_CTL4,&Value);
	Value &= (~0x00007F00);
	Value |= (temp<<8);
	RTMP_IO_WRITE32(pAd,XTAL_CTL4,Value);//1
#endif	
}


/**
 * ThalConfigMIBCounter, enable or disable counter
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param iOffset value
 *
 * @return void//ex SetConfigMIBCounter(pDeviceObject,STATISTICS_DEFAULT_EN_COUNTS,0, TRUE);
 * Otherwise, an error code is returned.
 */
 //ULONG g_u4MdrdyCnt = 0;
void SetConfigMIBCounter(RTMP_ADAPTER *pAd, ULONG u4CntBitmap,ULONG u4PerBSSCntBitmap, BOOLEAN fgReadClear)
{
    ULONG u4RegValue, u4CntBitmapTmp;

	DBGPRINT(RT_DEBUG_TRACE,("%s u4CntBitmap=0x%x,u4PerBSSCntBitmap=0x%x,fgReadClear=%d-->\n",__FUNCTION__,u4CntBitmap,u4PerBSSCntBitmap,fgReadClear));

    RTMP_IO_READ32(pAd,ARB_MSCR0,&u4RegValue);
    //u4RegValue = HAL_REG_32(ARB_MSCR0);

    u4CntBitmapTmp = u4CntBitmap;

    // These two fields should not be overwritten. These two functions are measurement function. 
    u4RegValue &= (MSCR_CCA_TIME_EN_MASK | 
                   MSCR_CCA_NAV_TX_TIME_EN_MASK |
                   MSCR_PED_TIME_EN_MASK | 
                   MSCR_OFDM_CCK_MDRDY_TIME_EN_MASK);

    u4CntBitmap &= ~(MSCR_CCA_TIME_EN_MASK | 
                     MSCR_CCA_NAV_TX_TIME_EN_MASK |
                     MSCR_PED_TIME_EN_MASK | 
                     MSCR_OFDM_CCK_MDRDY_TIME_EN_MASK);

    u4RegValue |= u4CntBitmap;

    if (fgReadClear){
        u4RegValue &= ~MSCR_MIB_COUNTER_RD_CONTROL;
    }
    else {
        u4RegValue |= MSCR_MIB_COUNTER_RD_CONTROL;
    }

	RTMP_IO_WRITE32(pAd,ARB_MSCR0,u4RegValue);// 1
	RTMP_IO_WRITE32(pAd,ARB_PBSCR,u4PerBSSCntBitmap);// 1
    //HAL_REG_32(ARB_MSCR0) = u4RegValue;
    //HAL_REG_32(ARB_PBSCR) = u4PerBSSCntBitmap;


    return;
}


/**
 * ThalConfigMIBCounter, enable or disable counter
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param iOffset value
 *
 * @return void//ex SetConfigMIBCounter(pDeviceObject,STATISTICS_DEFAULT_EN_COUNTS,0, TRUE);
 * Otherwise, an error code is returned.
 */
void GetStaticNumber(RTMP_ADAPTER *pAd, ULONG *pu4MdrdyCnt)
{
	ULONG GetValue = 0;

	RTMP_IO_READ32(pAd,ARB_MSDR10,&GetValue);
	GetValue &= BITS(0,25);
	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));
	//DBGPRINT(RT_DEBUG_TRACE,("%s, g_u4MdrdyCnt=%d, GetValue=%d-->\n",__FUNCTION__,g_u4MdrdyCnt,GetValue));
	//g_u4MdrdyCnt += GetValue;
	//*pu4MdrdyCnt = g_u4MdrdyCnt;
	*pu4MdrdyCnt = GetValue;

	DBGPRINT(RT_DEBUG_TRACE,("%s, u4MdrdyCnt=%d-->\n",__FUNCTION__,GetValue));

}

/**
 * Get PHY Mdrdy count
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param iOffset value
 *
 * @return void//ex SetConfigMIBCounter(pDeviceObject,STATISTICS_DEFAULT_EN_COUNTS,0, TRUE);
 * Otherwise, an error code is returned.
 */
void GetPHYMdrdy(RTMP_ADAPTER *pAd, ULONG *pu4MdrdyCntOFDM, ULONG *pu4MdrdyCntCCK)
{
	ULONG GetValue = 0;

	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));

	RTMP_IO_READ32(pAd,RO_PHYCTRL_STS5,&GetValue);
	*pu4MdrdyCntOFDM = (GetValue&0xFFFF0000)>>16;
	*pu4MdrdyCntCCK = (GetValue&0x0000FFFF);

	DBGPRINT(RT_DEBUG_TRACE,("%s, MdrdyCntOFDM=%d ,MdrdyCntCCK=%d-->\n",__FUNCTION__,*pu4MdrdyCntOFDM,*pu4MdrdyCntCCK));
}


/**
 * Set TSSI on/off
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param bOnOff 1:on, 0:off
 * @param WFSelect 2:both 1:WF1, 0:WF0
 * @return void
 * Otherwise, an error code is returned.
WF0 TSSI ON :  0x60200D04 32'hE1010800 
WF0 TSSI OFF:  0x60200D04 32'hE3F3F800
WF1 TSSI ON :  0x60201D04 32'hE1010800 
WF1 TSSI OFF:  0x60201D04 32'hE3F3F800
 */
void SetTSSI(RTMP_ADAPTER *pAd,ULONG bOnOff, UCHAR WFSelect)
{	
	ULONG CRValue = 0x0;	
	ULONG WF0Offset = 0x10D04;
	ULONG WF1Offset = 0x11D04;

	if(FALSE == bOnOff)
	{//off
		CRValue = 0xE3F3F800;		
	}
	else
	{//on
		CRValue = 0xE1010800;		
	}
	
	if((WF_0 == WFSelect) || (WF_ALL == WFSelect))
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s, Set WF#%d TSSI off\n",__FUNCTION__, WFSelect));
		RTMP_IO_WRITE32(pAd,WF0Offset,CRValue);// 3
		if(FALSE == bOnOff)
		{//off
			RTMP_IO_WRITE32(pAd,0x10D18,0x0);// 3	
		}
	}

	if((WF_1 == WFSelect) || (WF_ALL == WFSelect))
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s, Set WF#%d TSSI on\n",__FUNCTION__, WFSelect));
		RTMP_IO_WRITE32(pAd,WF1Offset,CRValue);// 3
		if(FALSE == bOnOff)
		{//off
			RTMP_IO_WRITE32(pAd,0x11D18,0x0);// 3
		}
	}
	
}
/**
 * Set DPD on/off
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param bOnOff 1:on, 0:off
 * @param WFSelect 2:both 1:WF1, 0:WF0
 * @return void
 * Otherwise, an error code is returned.
WF0 DPD OFF: 0x60200A08[31:28] 4'b1111
WF0 DPD ON:  0x60200A08[31:28] 4'b0000
WF1 DPD OFF: 0x60201A08[31:28] 4'b1111
WF1 DPD ON:  0x60201A08[31:28] 4'b0000

 */

void SetDPD(RTMP_ADAPTER *pAd,ULONG bOnOff, UCHAR WFSelect)
{	
	ULONG CRValue = 0x0;
	ULONG WF0Offset = 0x10A08;
	ULONG WF1Offset = 0x11A08;

	if(FALSE == bOnOff)
	{
		//WF0
		if((WF_0 == WFSelect) || (WF_ALL == WFSelect))
		{
			RTMP_IO_READ32(pAd, WF0Offset, &CRValue);
			CRValue |= 0xF0000000;
			RTMP_IO_WRITE32(pAd,WF0Offset,CRValue);//3
		}
		//WF1
		if((WF_1 == WFSelect) || (WF_ALL == WFSelect))
		{
			RTMP_IO_READ32(pAd, WF1Offset, &CRValue);
			CRValue |= 0xF0000000;
			RTMP_IO_WRITE32(pAd,WF1Offset,CRValue);//3
		}
		DBGPRINT(RT_DEBUG_TRACE,("%s, Set WFSelect: %d DPD off\n",__FUNCTION__, WFSelect));
	}
	else
	{
		if((WF_0 == WFSelect) || (WF_ALL == WFSelect))
		{
			RTMP_IO_READ32(pAd, WF0Offset, &CRValue);
			CRValue &= (~0xF0000000);
			RTMP_IO_WRITE32(pAd,WF0Offset,CRValue);//3
		}
		if((WF_1 == WFSelect) || (WF_ALL == WFSelect))
		{
			RTMP_IO_READ32(pAd, WF1Offset, &CRValue);
			CRValue &= (~0xF0000000);
			RTMP_IO_WRITE32(pAd,WF1Offset,CRValue);//3		
		}
		DBGPRINT(RT_DEBUG_TRACE,("%s, Set WFSelect: %d DPD on\n",__FUNCTION__, WFSelect));
	}
}

/**
 * Get MAC FCS Error Count
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param value void
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void GetMACFCSErrCnt(RTMP_ADAPTER *pAd,ULONG *pu4MACFCSErrCount)
{
	ULONG FcsErrorCnt = 0;

	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));

	RTMP_IO_READ32(pAd,ARB_MSDR4,&FcsErrorCnt);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
	*pu4MACFCSErrCount = FcsErrorCnt;
	DBGPRINT(RT_DEBUG_TRACE,("%s, <-- return pu4MACFCSErrCount %d\n",__FUNCTION__,*pu4MACFCSErrCount));
}

/**
 * Get PHY Mdrdy Error Count
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param value void
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void GetPHYMdrdyErrCnt(RTMP_ADAPTER *pAd,ULONG *pu4PhyMdrdyCountOFDM,ULONG *pu4PhyMdrdyCountCCK)
{
	ULONG Temp = 0, TempOFDM = 0, TempCCK = 0;
	
	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));

	RTMP_IO_READ32(pAd,RO_PHYCTRL_STS5,&Temp);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
	TempOFDM = Temp>>16;
	TempCCK = Temp&0x0000FFFF;

	*pu4PhyMdrdyCountOFDM = TempOFDM;
	*pu4PhyMdrdyCountCCK  = TempCCK;
	DBGPRINT(RT_DEBUG_TRACE,("%s, <-- return pu4PhyMdrdyCountOFDM %d,pu4PhyMdrdyCountCCK %d\n",__FUNCTION__,*pu4PhyMdrdyCountOFDM,*pu4PhyMdrdyCountCCK));
}

/**
 * Get PHY FCS Error Count
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param value void
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void GetPHYFCSErrCnt(RTMP_ADAPTER *pAd,ULONG *pu4PhyFCSErrCountOFDM,ULONG *pu4PhyFCSErrCountCCK)
{
	ULONG Temp = 0, TempOFDM = 0, TempCCK = 0;
	
	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));

	RTMP_IO_READ32(pAd,RO_PHYCTRL_STS4,&Temp);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
	TempOFDM = Temp>>16;
	TempCCK = Temp&0x0000FFFF;

	*pu4PhyFCSErrCountOFDM = TempOFDM;
	*pu4PhyFCSErrCountCCK  = TempCCK;
	DBGPRINT(RT_DEBUG_TRACE,("%s, <-- return pu4PhyFCSErrCountOFDM %d,pu4PhyFCSErrCountCCK %d\n",__FUNCTION__,*pu4PhyFCSErrCountOFDM,*pu4PhyFCSErrCountCCK));
}

/**
 * Get PHY PD
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param value void
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void GetPhyPD(RTMP_ADAPTER *pAd,ULONG *pu4PhyPDOFDM,ULONG *pu4PhyPDCCK)
{
	ULONG Temp = 0, TempOFDM = 0, TempCCK = 0;
	
	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));

	RTMP_IO_READ32(pAd,RO_PHYCTRL_STS0,&Temp);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
	TempCCK = Temp&0x0000FFFF;
	TempOFDM = (Temp>>16);

	*pu4PhyPDCCK = TempCCK;
	*pu4PhyPDOFDM = TempOFDM;
	DBGPRINT(RT_DEBUG_TRACE,("%s, <-- return pu4PhyPDCCK %d,pu4PhyPDOFDM %d\n",__FUNCTION__,*pu4PhyPDCCK,*pu4PhyPDOFDM));
}
/**
 *Get PHY CCK of  SIG , SFD
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param value void
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void GetPhyCCK_SIG_SFD(RTMP_ADAPTER *pAd,ULONG *pu4SIG,ULONG *pu4SFD)
{
	ULONG Temp = 0, TempSig = 0, TempSFD = 0;
	
	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));

	RTMP_IO_READ32(pAd,RO_PHYCTRL_STS1,&Temp);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
	TempSig = Temp>>16;
	TempSFD = Temp&0x0000FFFF;

	*pu4SIG = TempSig;
	*pu4SFD = TempSFD;
	DBGPRINT(RT_DEBUG_TRACE,("%s, <-- return pu4SIG %d,pu4PhyPDOFDM %d\n",__FUNCTION__,*pu4SIG,*pu4SFD));

}
/**
 *Get PHY OFDM Error
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param value void
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void GetPhyOFDMErr(RTMP_ADAPTER *pAd,ULONG *pu4SIG,ULONG *pu4TAG)
{
	ULONG Temp = 0, TempSig = 0, TempTAG = 0;
	
	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));

	RTMP_IO_READ32(pAd,RO_PHYCTRL_STS2,&Temp);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
	TempSig = Temp>>16;
	TempTAG = Temp&0x0000FFFF;

	*pu4SIG = TempSig;
	*pu4TAG = TempTAG;
	DBGPRINT(RT_DEBUG_TRACE,("%s, <-- return pu4SIG %d,pu4PhyPDOFDM %d\n",__FUNCTION__,*pu4SIG,*pu4TAG));

}

/**
 *Get Phy OFDM Error
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param value void
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void GetPhyRSSI(RTMP_ADAPTER *pAd,ULONG *pWBRSSI0,ULONG *pIBRSSI0,ULONG *pWBRSSI1,ULONG *pIBRSSI1)
{
	ULONG Temp = 0, WBRSSI0 = 0, WBRSSI1 = 0, IBRSSI0 = 0, IBRSSI1 = 0;
	
	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));

	RTMP_IO_READ32(pAd,RO_AGC_DEBUG_2,&Temp);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
	*pIBRSSI0 = (Temp&0xFF000000)>>24;
	*pWBRSSI0 = (Temp&0x00FF0000)>>16;
	*pIBRSSI1 = (Temp&0x0000FF00)>>8;
	*pWBRSSI1 = (Temp&0x000000FF);

	if(*pIBRSSI0>128)
		*pIBRSSI0-=256;

	if(*pWBRSSI0>128)
		*pWBRSSI0-=256;

	if(*pIBRSSI1>128)
		*pIBRSSI1-=256;

	if(*pWBRSSI1>128)
		*pWBRSSI1-=256;
	
	DBGPRINT(RT_DEBUG_TRACE,("%s, <-- return pIBRSSI0 %d,pWBRSSI0 %d,pIBRSSI1 %d,pWBRSSI1 %d\n",__FUNCTION__,
		*pIBRSSI0,*pWBRSSI0,*pIBRSSI1,*pWBRSSI1));

}


/**
 *Get Phy OFDM Error
 *
 * @param pDeviceObject pointer PDEVICE_OBJECT
 * @param value void
 *
 * @return void
 * Otherwise, an error code is returned.
 */
void SetTMR(RTMP_ADAPTER *pAd,UCHAR enable)
{
	ULONG value = 0;

	DBGPRINT(RT_DEBUG_TRACE,("%s, -->\n",__FUNCTION__));		

	switch (enable)
	{
		case 1://initialiter
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: enable TMR report, as Initialiter\n", __FUNCTION__));
			RTMP_IO_READ32(pAd,RMAC_TMR_PA, &value);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
			value = value | BIT31;
			value = value & ~BIT30;
			value = value | 0x34;//Action frame register
			RTMP_IO_WRITE32(pAd,RMAC_TMR_PA, value);
		}
		break;
		case 2://responder
       	{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: enable TMR report, as Responser\n", __FUNCTION__));
			RTMP_IO_READ32(pAd,RMAC_TMR_PA, &value);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
			value = value | BIT31;
			value = value | BIT30;
			value = value | 0x34;//Action frame register
			RTMP_IO_WRITE32(pAd,RMAC_TMR_PA, value);
		}
		break;
		case 0://disable
		default:
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: disable TMR report\n", __FUNCTION__));			
			RTMP_IO_READ32(pAd,RMAC_TMR_PA, &value);//::TODO CMD_GET_MACFCSERRORCNT_COUNT
			value = value & ~BIT31;
			RTMP_IO_WRITE32(pAd,RMAC_TMR_PA, value);
		}
	}

}
