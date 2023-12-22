/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    bulkrwr.h

Abstract:

Environment:

    Kernel mode

Notes:

    Copyright (c) 2000 Microsoft Corporation.  
    All Rights Reserved.

--*/
#ifndef _HAL_H
#define _HAL_H


#define WIFI_PHY_BASE           0x10000//0x60200000

#define WIFI_MAC_BASE          0x20000 	//0x60100000
#define WIFI_TMAC_BASE         WIFI_MAC_BASE + 0x1600 //(WIFI_MAC_BASE + 0x00030000)//
#define TMAC_FPCR               (WIFI_TMAC_BASE + 0x0080)

#define CR_BASE_HIF		0x50000000
#define CR_BASE_UDMA	0x50029000

#define CR_TXFD_1	WIFI_PHY_BASE+0x4704

#define CR_PHYCK_CTRL	WIFI_PHY_BASE
#define CR_FR_CKG_CTRL WIFI_PHY_BASE+0x0004
#define CR_FFT_MANU_CTRL WIFI_PHY_BASE+0x0704
#define CR_PHYCTRL_0	WIFI_PHY_BASE+0x4100
#define CR_PHYCTRL_DBGCTRL WIFI_PHY_BASE+0x4140
#define CR_PHYMUX_3 WIFI_PHY_BASE+0x420C
#define CR_PHYMUX_5	WIFI_PHY_BASE+0x4214
#define CR_PHYMUX_10	WIFI_PHY_BASE+0x4228
#define CR_TXFE_4	WIFI_PHY_BASE+0x0A0C
#define CR_TXFD_0	WIFI_PHY_BASE+0x4700
#define CR_TXFD_3 WIFI_PHY_BASE+0x470C
#define R_WF_TMAC_PCTSR WIFI_TMAC_BASE+0x0108

#define CR_BASE_TOP             0x80020000

#define UDMA_WLCFG_0		(CR_BASE_UDMA+0x0018)
#define SCH_REG1				(CR_BASE_HIF + 0x588) 
#define SCH_REG2				(CR_BASE_HIF + 0x58c) 
#define SCH_REG3				(CR_BASE_HIF + 0x590) 
#define SCH_REG4				(CR_BASE_HIF + 0x594) 

#define TOP_MISC2               (CR_BASE_TOP + 0x1134) 
#define SW_SYNC				0x81021250

#define TOP_CFG	0x0000
#define XTAL_CTL4 TOP_CFG+0x1210
#define RG_XO_C2 TOP_CFG+0x1234
#define RG_XO_C2_MANUAL TOP_CFG+0x1238

#define ARBBASE 0x21400//60120000//physical
#define AIFSR0 ARBBASE+0x20
#define AIFSR1 ARBBASE+0x24
#define DRNGR  ARBBASE+0x68

#define AIFS_0 		0
#define AIFS_1 		1
#define AIFS_2 		2
#define AIFS_3 		3
#define AIFS_4 		4
#define AIFS_5 		5
#define AIFS_BCN 	6
#define AIFS_BMC 	7
#define AIFS_10 	8
#define AIFS_11 	9
#define AIFS_12 	10
#define AIFS_13 	11
#define AIFS_14 	12



#define BIT(n)                          ((ULONG) 1 << (n))
#define BITS2(m,n)                      (BIT(m) | BIT(n) )
#define BITS3(m,n,o)                    (BIT(m) | BIT (n) | BIT (o))
#define BITS4(m,n,o,p)                  (BIT(m) | BIT (n) | BIT (o) | BIT(p))

#define BITS(m,n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
/* FPCR */
#define FPCR_POWER2_OFFSET_EN           BIT(31)
#define FPCR_FRAME_POWER_MAX_DBM        BITS(24, 30)
#define FPCR_FRAME_POWER_MAX_DBM_OFFSET 24
#define FPCR_POWER1_OFFSET_EN           BIT(23)
#define FPCR_FRAME_POWER_MIN_DBM        BITS(16, 22)
#define FPCR_FRAME_POWER_MIN_DBM_OFFSET 16


/* FPSET */
#define FPR_64_QAM_FRAME_POWER_DBM BITS(24, 30)
#define FPR_64_QAM_FRAME_POWER_DBM_OFFSET 24
#define FPR_16_QAM_FRAME_POWER_DBM BITS(16,22)
#define FPR_16_QAM_FRAME_POWER_DBM_OFFSET 16
#define FPR_QPSK_FRAME_POWER_DBM BITS(8, 14)
#define FPR_QPSK_FRAME_POWER_DBM_OFFSET 8
#define FPR_BPSK_FRAME_POWER_DBM BITS(0,6)
#define FPR_BPSK_FRAME_POWER_DBM_OFFSET 0

/* FP0R0 */
#define FP0R0_LG_OFDM3_FRAME_POWER_DBM BITS(24, 30)
#define FP0R0_LG_OFDM2_FRAME_POWER_DBM BITS(16,22)
#define FP0R0_LG_OFDM1_FRAME_POWER_DBM BITS(8, 14)
#define FP0R0_LG_OFDM0_FRAME_POWER_DBM BITS(0,6)
/* FP0R1 */
#define FP0R1_HT20_3_FRAME_POWER_DBM BITS(24, 30)
#define FP0R1_HT20_2_FRAME_POWER_DBM BITS(16,22)
#define FP0R1_HT20_1_FRAME_POWER_DBM BITS(8, 14)
#define FP0R1_HT20_0_FRAME_POWER_DBM BITS(0,6)
/* FP0R2 */
#define FP0R2_HT40_3_FRAME_POWER_DBM BITS(24, 30)
#define FP0R2_HT40_2_FRAME_POWER_DBM BITS(16,22)
#define FP0R2_HT40_1_FRAME_POWER_DBM BITS(8, 14)
#define FP0R2_HT40_0_FRAME_POWER_DBM BITS(0,6)
/* FP0R3 */
#define FP0R3_CCK1_FRAME_POWER0_DBM     BITS(16, 22) 
/* For CCK 5.5M & 11M */
#define FP0R3_CCK1_FRAME_POWER0_DBM_OFFSET 16
#define FP0R3_LG_OFDM4_FRAME_POWER0_DBM BITS(8, 14)  
/* For OFDM 54M */
#define FP0R3_CCK0_FRAME_POWER0_DBM     BITS(0, 6)   
/* For CCK 1M & 2M *//* FP0R4 */
#define FP0R4_HT40_5_FRAME_POWER_DBM BITS(24, 30)   
/* For MCS 7 */
#define FP0R4_HT40_4_FRAME_POWER_DBM BITS(16,22)    
/* For MCS 6 */
#define FP0R4_HT20_5_FRAME_POWER_DBM BITS(8, 14)     
/* For MCS 7 */
#define FP0R4_HT20_4_FRAME_POWER_DBM BITS(0,6)        
/* For MCS 6 *//* FP0R5 */
#define FP0R5_VHT20_3_FRAME_POWER_DBM       BITS(24,30)   
/* For MCS 5, 6 */
#define FP0R5_VHT20_2_FRAME_POWER_DBM       BITS(16,22)   
/* For 16-QAM */
#define FP0R5_VHT20_1_FRAME_POWER_DBM       BITS(8,14)    
/* For QPSK */
#define FP0R5_VHT20_0_FRAME_POWER_DBM       BITS(0,6)     
/* For BPSK *//* FP0R6 */
#define FP0R6_VHT20_6_FRAME_POWER_DBM        BITS(16,22)   
/* For MCS 9 */
#define FP0R6_VHT20_5_FRAME_POWER_DBM        BITS(8,14)    
/* For MCS 8 */
#define FP0R6_VHT20_4_FRAME_POWER_DBM        BITS(0,6)     
/* For MCS 7 *//* FP0R7 */
#define FP0R7_VHT160_FRAME_POWER_OFFET_DBM   BITS(16,22)
#define FP0R7_VHT80_FRAME_POWER_OFFET_DBM    BITS(8,14)
#define FP0R7_VHT40_FRAME_POWER_OFFET_DBM    BITS(0,6)

/***** MIB Counter *****/
/* MSCR */
#define MSCR_MIB_COUNTER_RD_CONTROL    BIT(31)

#define MSCR_PED_TIME_EN_MASK           BITS(28,30)
#define MSCR_PED_TIME_OFFSET            28
#define MSCR_OFDM_CCK_MDRDY_TIME_EN_MASK BITS(25,27)
#define MSCR_OFDM_CCK_MDRDY_TIME_OFFSET 25

#define MSCR_PHY_MIB_CNT1_EN            BIT(24)
#define MSCR_PHY_MIB_CNT0_EN            BIT(23)
#define MSCR_AMPDU_TX_COUNT_EN          BIT(22)
#define MSCR_TX_AGG_RANGE_COUNT_EN      BIT(21)
#define MSCR_TX_BW_COUNT_EN             BIT(20)
#define MSCR_ARB_RWP_FAIL_COUNT_EN      BIT(19)
#define MSCR_ARB_RWP_NEED_COUNT_EN      BIT(18)
#define MSCR_BCN_TX_COUNT_EN            BIT(17)

#define MSCR_CCA_NAV_TX_TIME_EN_MASK    BITS(14,16)
#define MSCR_CCA_NAV_TX_TIME_OFFSET     14

#define MSCR_CCA_TIME_EN_MASK           BITS(11,13)
#define MSCR_CCA_TIME_OFFSET            11

#define MSCR_AMPDU_RX_COUNT_EN          BIT(10)
#define MSCR_RX_LEN_MIS_COUNT_EN        BIT(9)
#define MSCR_PF_DROP_COUNT_EN           BIT(8)
#define MSCR_MDRDY_EVENT_EN             BIT(7)
#define MSCR_VEC_MISMATCH_EN            BIT(6)
#define MSCR_DELIMITER_FAIL_COUNT_EN    BIT(5)
#define MSCR_VEC_DROP_COUNT_EN          BIT(4)
#define MSCR_CH_IDLE_COUNT_EN           BIT(3)
#define MSCR_RX_MPDU_COUNT_EN           BIT(2)
#define MSCR_RX_FIFO_FULL_COUNT_EN      BIT(1)
#define MSCR_RX_FCS_ERR_COUNT_EN        BIT(0)

#define MSCR_MIB_ALL_COUNT_EN          BITS(0,30)

#define STATISTICS_DEFAULT_EN_COUNTS \
	(MSCR_MIB_ALL_COUNT_EN)

typedef	union	_U3DMA_WLCFG	{
	struct	{
	ULONG	WL_RX_AGG_TO:8; 						// [7 : 0]		WLAN UDMA RX Aggregation Time-Out Value. Unit:1us.
	ULONG	WL_RX_AGG_LMT:8; 						// [15 : 8]	WLAN UDMA RX Aggregation Limit.
	ULONG	Reserve1:2; 								// [17 : 16]	Reserved bits.
	ULONG	WL_RX_MPSZ_PAD0:1;					// [18]		Padding 1DW if the length of IN data is with maxima packet size.
	ULONG	WL_TX_CLEAR:1;							// [19]		WLAN UDMA UDMA TX Clear
	ULONG	WL_LPK_EN:1;							// [20]		WLAN U3DMA loopback mode enable
	ULONG	WL_RX_AGG_EN:1;						// [21]		WLAN UDMA RX Aggregation Function Enable
	ULONG	WL_RX_EN:1;							// [22]		WLAN UDMA RX Enable
	ULONG	WL_TX_EN:1;							// [23]		WLAN UDMA TX Enable
	ULONG	Reserve2:6;								// [29 : 24]	Reserved bits
	ULONG	WL_RX_BUSY:1;							// [30]		Rx Busy Status. 0 : idls   1:busy
	ULONG	WL_TX_BUSY:1;							// [31]		Tx Busy Status. 0 : idls   1:busy
	}	Default;
	ULONG			word;
} U3DMA_WLCFG, *PU3DMA_WLCFG;




int  mt_asic_top_init(RTMP_ADAPTER *pAd);
void mt_hif_sys_init(RTMP_ADAPTER *pDeviceObject);
int asic_set_tmac_info_template(RTMP_ADAPTER *pDeviceObject);
INT32  SetResrvPage(RTMP_ADAPTER *pAd);
INT32 AsicDMAScheduler_init(RTMP_ADAPTER *pAd, int iMode);
int mt7603_init_mac_cr(RTMP_ADAPTER *pAd);
int mt7603_DoOtherConfig(RTMP_ADAPTER *pAd);
void StopContinuTXTest(RTMP_ADAPTER *pAd);
void StartContinuTXTest(RTMP_ADAPTER *pAd,int iTestType);
void StopTXToneTest(RTMP_ADAPTER *pAd);
 void StartTXToneTest(RTMP_ADAPTER *pAd,int iTestType);
BOOLEAN SetAIFS(RTMP_ADAPTER *pAd,ULONG iSetType,ULONG iSetValue);
void TxConfigPowerRange(RTMP_ADAPTER *pAd, IN UCHAR ucMaxPowerDbm,IN UCHAR ucMinPowerDbm);
void TxSetFramePower (RTMP_ADAPTER *pAd, ULONG eModulationSystem, ULONG eModulationType,UCHAR ucFramePowerDbm);
void TxSetFrequencyOffset (RTMP_ADAPTER *pAd, ULONG iOffset);
void GetStaticNumber(RTMP_ADAPTER *pAd, ULONG *pu4MdrdyCnt);
void GetPHYMdrdy(RTMP_ADAPTER *pAd, ULONG *pu4MdrdyCntOFDM, ULONG *pu4MdrdyCntCCK);
void SetConfigMIBCounter(RTMP_ADAPTER *pAd, ULONG u4CntBitmap,ULONG u4PerBSSCntBitmap, BOOLEAN fgReadClear);
void StartContinuesTXTest(RTMP_ADAPTER *pAd,UCHAR Modulation, UCHAR BW, UCHAR pri_ch, UCHAR rate,UCHAR WFSelect);
void SetTSSI(RTMP_ADAPTER *pAd,ULONG bOnOff, UCHAR WFSelect);
void SetDPD(RTMP_ADAPTER *pAd,ULONG bOnOff, UCHAR WFSelect);

void GetMACFCSErrCnt(RTMP_ADAPTER *pAd,ULONG *pu4MACFCSErrCount);
void GetPHYMdrdyErrCnt(RTMP_ADAPTER *pAd,ULONG *pu4PhyMdrdyCountOFDM,ULONG *pu4PhyMdrdyCountCCK);
void GetPHYFCSErrCnt(RTMP_ADAPTER *pAd,ULONG *pu4PhyFCSErrCountOFDM,ULONG *pu4PhyFCSErrCountCCK);
void GetPhyPD(RTMP_ADAPTER *pAd,ULONG *pu4PhyPDOFDM,ULONG *pu4PhyPDCCK);
void GetPhyCCK_SIG_SFD(RTMP_ADAPTER *pAd,ULONG *pu4SIG,ULONG *pu4SFD);
void GetPhyOFDMErr(RTMP_ADAPTER *pAd,ULONG *pu4SIG,ULONG *pu4TAG);
void GetPhyRSSI(RTMP_ADAPTER *pAd,ULONG *pWBRSSI0,ULONG *pIBRSSI0,ULONG *pWBRSSI1,ULONG *pIBRSSI1);
void SetTMR(RTMP_ADAPTER *pAd,UCHAR enable);

#endif

