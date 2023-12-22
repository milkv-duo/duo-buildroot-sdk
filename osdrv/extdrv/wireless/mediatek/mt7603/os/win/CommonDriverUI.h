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
 	COmmonDriverUI.h
 
	Abstract:
    define common value between dll,ui and driver

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Werner Liao   05-24-2024    created

*/
#ifndef	__COMMONDRIVERUI_H__
#define	__COMMONDRIVERUI_H__
#pragma once
#define UINT_32 ULONG
#define UINT_8  UCHAR

#define DEVICE_UNKNOW	-1
#define DEVICE_PCIE		0
#define DEVICE_USB		1
#define DEVICE_APSOC	2
#define DEVICE_SDIO		3

#define TXWI_SHORT_SIZE	12

#define RING_TYPE_AC0			0
#define RING_TYPE_AC1			1
#define RING_TYPE_AC2			2
#define RING_TYPE_AC3			3
#define RING_TYPE_AC4			4
#define RING_TYPE_AC5			5
#define RING_TYPE_AC6			6
#define RING_TYPE_AC7			7
#define RING_TYPE_AC8			8
#define RING_TYPE_AC9			9
#define RING_TYPE_AC10			10
#define RING_TYPE_AC11			11
#define RING_TYPE_MGMT			12
#define RING_TYPE_HCCA			13
#define RING_TYPE_RX0			14
#define RING_TYPE_RX1			15

#define	PREAMBLE_CCK			0
#define	PREAMBLE_OFDM			1
#define PREAMBLE_MIX_MODE     	2
#define PREAMBLE_GREEN_FIELD	3
#define PREAMBLE_VHT			4

#define BANDWIDTH_20	0
#define BANDWIDTH_40	1
#define BANDWIDTH_80	2
#define BANDWIDTH_10	3
#define BANDWIDTH_5		4
#define BANDWIDTH_160	5

#define WF_0			0
#define WF_1			1
#define WF_ALL			2

#define PER_PKT_BW20	0
#define PER_PKT_BW40	1
#define PER_PKT_BW80	2

#define Enable_All_Tx	0
#define Enable_Tx0		1
#define Enable_Tx1		2
#define Enable_Tx2		3

#define Enable_All_Rx	0
#define Enable_Rx0		1
#define Enable_Rx1		2
#define Enable_Rx2		3



#define TX_START				1
#define TX_STOP					0

//#define UN_SET					255

#define RX_START				1
#define RX_STOP					0
//
// Entry number for each DMA descriptor ring
//
#define	RING_TX_DESCRIPTOR_SIZE	16
#define	RING_RX_DESCRIPTOR_SIZE	16

#define	AC0_RING_SIZE			8
#define	AC1_RING_SIZE			8
#define	AC2_RING_SIZE			8
#define	AC3_RING_SIZE			8
#define	AC4_RING_SIZE			8
#define	AC5_RING_SIZE			8
#define	AC6_RING_SIZE			8
#define	AC7_RING_SIZE			8
//++werner->
#define	AC8_RING_SIZE			8
#define	AC9_RING_SIZE			8
#define	AC10_RING_SIZE			8
#define	AC11_RING_SIZE			8
//++werner<-
#define	MGMT_RING_SIZE		8
#define	HCCA_RING_SIZE		8
#define	RX0_RING_SIZE			8
#define	RX1_RING_SIZE			8

#define	AC0_TEMP_SIZE			0
#define	AC1_TEMP_SIZE			0
#define	AC2_TEMP_SIZE			0
#define	AC3_TEMP_SIZE			0
#define	AC4_TEMP_SIZE			0
#define	AC5_TEMP_SIZE			0
#define	AC6_TEMP_SIZE			0
#define	AC7_TEMP_SIZE			0
#define	AC8_TEMP_SIZE			0
#define	AC9_TEMP_SIZE			0
#define	AC10_TEMP_SIZE		0
#define	AC11_TEMP_SIZE		0
#define	MGMT_TEMP_SIZE		0
#define	HCCA_TEMP_SIZE		0


#define	AC0_BUFFER_SIZE			8192
#define	AC1_BUFFER_SIZE			8192
#define	AC2_BUFFER_SIZE			8192
#define	AC3_BUFFER_SIZE			8192
#define	AC4_BUFFER_SIZE			8192
#define	AC5_BUFFER_SIZE			8192
#define	AC6_BUFFER_SIZE			8192
#define	AC7_BUFFER_SIZE			8192
#define	AC8_BUFFER_SIZE			8192
#define	AC9_BUFFER_SIZE			8192
#define	AC10_BUFFER_SIZE			8192
#define	AC11_BUFFER_SIZE			8192
#define	MGMT_BUFFER_SIZE			8192
#define	HCCA_BUFFER_SIZE			8192
#define	RX_BUFFER_SIZE				8192

//--- INT bit ---

//individual TX/RX INT
#define INT_Bit_ANY_TxDone		0x0003FFF0 //0x08083FF0
#define INT_Bit_ANY_RxDone		0x00000003 //0x04080003

#define INT_Bit_BEACON_TIMER	0x00000004
#define INT_Bit_Reserved_Bit03	0x00000008
#define INT_Bit_ANY_TxAbort		0x00000010
#define INT_Bit_Reserved_Bit05	0x00000020
#define INT_Bit_Reserved_Bit06	0x00000040
#define INT_Bit_Reserved_Bit07	0x00000080
#define INT_Bit_Reserved_Bit08	0x00000100
#define INT_Bit_Reserved_Bit09	0x00000200
#define INT_Bit_Reserved_Bit10	0x00000400
#define INT_Bit_Reserved_Bit11	0x00000800
#define INT_Bit_Reserved_Bit12	0x00001000
#define INT_Bit_Reserved_Bit13	0x00002000
#define INT_Bit_Reserved_Bit14	0x00004000
#define INT_Bit_Reserved_Bit15	0x00008000
//---
#define INT_Bit_RX0TxDone		0x00000001
#define INT_Bit_RX1TxDone		0x00000002

#define INT_Bit_Ac0TxDone		0x00000010
#define INT_Bit_Ac1TxDone		0x00000020
#define INT_Bit_Ac2TxDone		0x00000040
#define INT_Bit_Ac3TxDone		0x00000080
#define INT_Bit_Ac4TxDone		0x00000100
#define INT_Bit_Ac5TxDone		0x00000200
#define INT_Bit_Ac6TxDone		0x00000400
#define INT_Bit_Ac7TxDone		0x00000800
#define INT_Bit_Ac8TxDone		0x00001000
#define INT_Bit_Ac9TxDone		0x00002000
#define INT_Bit_Ac10TxDone		0x00004000
#define INT_Bit_Ac11TxDone		0x00008000
#define INT_Bit_MgmtTxDone		0x00010000
#define INT_Bit_HccaTxDone		0x00020000



#define INT_Bit_Reserved_Bit22	0x00400000
#define INT_Bit_Reserved_Bit23	0x00800000
#define INT_Bit_Reserved_Bit24	0x01000000
#define INT_Bit_Reserved_Bit25	0x02000000
#define INT_Bit_Reserved_Bit26	0x04000000
#define INT_Bit_Reserved_Bit27	0x08000000
#define INT_Bit_Reserved_Bit28	0x10000000
#define INT_Bit_Reserved_Bit29	0x20000000
#define INT_Bit_Reserved_Bit30	0x40000000
#define INT_Bit_Reserved_Bit31	0x80000000
//---

#ifndef WIN32
typedef unsigned int UINT32;
typedef unsigned char BOOLEAN;
typedef unsigned short UINT16;
typedef unsigned char UCHAR;
typedef signed char CHAR;
#endif
 //DMA D
typedef	struct _TXD_STRUC {
	// DWORD 0
	UINT32 SDP0;
	
	// DWORD 1
	UINT32 SDL1:14;
	UINT32 LS1:1;
	UINT32 Burst:1;	
	UINT32 SDL0:14;
	UINT32 LS0:1;
	UINT32 DDONE:1;

	// DWORD 2
	UINT32 SDP1;	

	// DWORD 3
	UINT32 PktLength:16;
	UINT32 NextVLD:1;
	UINT32 TxBurst:1;
	UINT32 Reserved_1:1;
	UINT32 Is802dot11:1;
	UINT32 TSO:1;
	UINT32 CSO:1;
	UINT32 Reserved_2:2;
	UINT32 WIV:1;
	UINT32 PBF_QSEL:2;
	UINT32 D_PORT:3;
	UINT32 INFO_TYPE:2;
}TXD_STRUC, *PTXD_STRUC;

//****************************************************
//2860c TX Wireless Information
//****************************************************
/*typedef	struct _TX_WI_STRUC
{
	// DWORD 0
	UINT32 FRAG:1;
	UINT32 MMPS:1;               //MIMO power save
	UINT32 CFACK:1;             //Piggyback ACK enable
	UINT32 TS:1;                   //Insert timestamp
	UINT32 AMPDU:1;            //AMPDU enable
	UINT32 MPDU_Dens:3;    //MPDU density
	UINT32 TXOP_MODE:2;
	UINT32 NDPS:2;
	UINT32 NDPBW:1;
	UINT32 SOUND:1;
	UINT32 Reserved_1:2;
	UINT32 MCS:7;
	UINT32 BW:2;
	UINT32 SGI:1;
	UINT32 STBC:1;
	UINT32 ETXBF:1;
	UINT32 ITXBF:1;
	UINT32 PHY_MODE:3;
	
	// DWORD 1
	UINT32 ACK:1;                  //ACK/BA required
	UINT32 NSEQ:1;               //Insert sequence number
	UINT32 BAWinSize:6;      //BLOCK ACK WINDOW SIZE
	UINT32 WCID:8;              //Wireless cilent ID
	UINT32 MPDU_SIZE:14;
	UINT32 Reserved_2:2;     //TX packet ID

	// DWORD 2
	UINT32 IV;

	// DWORD 3
	UINT32 EIV;

	// DWORD 4
	UINT32 TX_EAP_ID:8;
	UINT32 TX_Stream_Mode:8;
	UINT32 TX_Power_Adjust:4;
	UINT32 Reserved_3:4;
	UINT32 TX_PKT_ID:8;
}TX_WI_STRUC, *PTX_WI_STRUC;
*/
//TXD 7603
typedef	struct _TX_WI_STRUC 
{
	// TX Descriptor in Long Format (FT = 1)
	// DWORD 0
	UINT32	TXByteCount:16;
	UINT32	EtherTypeOffset: 7;
	UINT32	I:1;
	UINT32	UT:1;
	UINT32	UNxV:1;
	UINT32	UTxB:1;
	UINT32	Q_IDX:4;
	UINT32	P_IDX:1;

	//DWORD 1
	UINT32	WLANIndex:8;
	UINT32	HeaderLength:5;
	UINT32	HF:2;
	UINT32	FT:1;
	UINT32	HeaderPadding:3;
	UINT32	NA:1;
	UINT32	TID:3;
	UINT32	PF:1;
	UINT32	Rsv0:2;
	UINT32	QM:6;

	// DWORD 2
	UINT32	SubType:4;
	UINT32	Type:2;
	UINT32	NDP:1;
	UINT32	NDPA:1;
	UINT32	SD:1;
	UINT32	RTS:1;
	UINT32	BM:1;
	UINT32	B:1;
	UINT32	DU:1;
	UINT32	HE:1;
	UINT32	FRAG:2;
	UINT32	MaxTXTime:8;
	UINT32	PowerOffset:5;
	UINT32	BA_DIS:1;
	UINT32	TM:1;
	UINT32	FR:1;

	// DWORD 3
	UINT32	Rsv1:6;
	UINT32	TXCount:5;
	UINT32	RemainingTXCount:5;
	UINT32	SN:12;
	UINT32	Rsv2:2;
	UINT32	PN_VLD:1;
	UINT32	SN_VLD:1;

	// DWORD 4
	UINT32	PN0:32;

	// DWORD 5
	UINT32	PID:8;
	UINT32	TXSFM:1;
	UINT32	TXS2M:1;
	UINT32	TXS2H:1;
	UINT32	DAS:1;
	UINT32	BSN:1;
	UINT32	PM:1;
	UINT32	Rsv3:2;
	UINT32	PN1:16;

	// DWORD 6
	UINT32	FRM:1;
	UINT32	Rsv4:1;
	UINT32	ANT_ID:6;
	UINT32	BW:3;
	UINT32	SPE_EN:1;
	UINT32	ANT_PRI:3;
	UINT32	DYN_BW:1;
	UINT32	ETXBF:1;
	UINT32	ITXBF:1;
	//UINT32	FixedRate:12;
	//FixedRate begin	
	UINT32	TXRate:6;
	UINT32	TXMode:3;
	UINT32	Nsts:2;
	UINT32	STBC:1;
	//FixedRate end
	UINT32	LDPC:1;
	UINT32	GI:1;

	
	// DWORD 7
	UINT32	SchedulerTXTime:16;
	UINT32	SWField:16;

}TX_WI_STRUC, *PTX_WI_STRUC;
//****************************************************

typedef	struct _RXD_STRUC
{
	// DWORD 0
	UINT32		SDP0;
	
	// DWORD 1
	UINT32		SDL1:14;
	UINT32		Rsv:2;
	UINT32		SDL0:14;
	UINT32		LS0:1;
	UINT32		DDONE:1;
	
	// DWORD 2
	UINT32		SDP1;
	
	// DWORD 3
	UINT32 PktLength:14;
	UINT32 Reserved_2:1;
	UINT32 Reserved_3:1;
	UINT32 UDPerr:1;
	UINT32 TCPerr:1;
	UINT32 IPerr:1;
	UINT32 Is802dot11:1;
	UINT32 L3L4Done:1;
	UINT32 MAC_LENOFS:3;
	UINT32 PCIeINT:1;
	UINT32 QSEL:2;
	UINT32 S_PORT:3;
	UINT32 INFO_TYPE:2;
	
}RXD_STRUC, *PRXD_STRUC;

#define MAX_FW_DOWNLOAD_SEQ	100
#define MIN_FW_PACKETCMD_SEQ_BEGIN	101

#define MAX_FW_IMAGE_SIZE	512000
#define FW_READ_SIZE		1024
#define FW_IMAGE_7603_NAME		"FW_7603.bin"
#define FW_IMAGE_7636_ROM_NAME	"FW_7636_ROM.bin"
#define FW_IMAGE_7636_RAM_NAME	"FW_7636_RAM.bin"
#define FW_TAIL_LENGTH		36

#ifdef NEW_CMD
typedef	struct _FwTxD_STRUC 
{
	//DROWD 0
	ULONG	u2Length:16;
	ULONG	u2PQ_ID: 16;
	//DROWD 1
	ULONG	ucCID:8;
	ULONG	ucPktTypeID:8;
	ULONG	ucSetQuery:8;
	ULONG	ucSeqNum:8;
	//DROWD 2
	ULONG	ucD2B0Rev:8;
	ULONG	ucExteendCID:8;	
	ULONG	ucD2B2Rev:8;
	ULONG	ucExtCmdOption:8;
	//DROWD 3~7
	ULONG	au4D3toD7Rev[5];
	//ULONG	aucBuffer[0];

}FwTxD_STRUC, *PFwTxD_STRUC;

typedef	struct _FwDownLoadD_STRUC 
{
	//DROWD 0
	ULONG	u2Length:16;
	ULONG	u2PQ_ID: 16;
	//DROWD 1
	ULONG	ucCID:8;
	ULONG	ucPktTypeID:8;
	ULONG	ucSetQuery:8;
	ULONG	ucSeqNum:8;
	//DROWD 2
	ULONG	ucExteendCID:8;	
	ULONG	ucExtCmdOption:8;
	ULONG	ucReserved1:8;
	ULONG	ucReserved2:8;

}FwDownLoadD_STRUC, *PFwDownLoadD_STRUC;

#else
typedef	struct _FwTxD_STRUC 
{
	//DROWD 0
	UINT32	u2Length:16;
	UINT32	u2PQ_ID: 16;
	//DROWD 1
	UINT32	ucCID:8;
	UINT32	ucPktTypeID:8;
	UINT32	ucSetQuery:8;
	UINT32	ucSeqNum:8;
	//DROWD 2
	UINT32	ucExteendCID:8;	
	UINT32	ucExtCmdOption:8;
	UINT32	ucReserved1:8;
	UINT32	ucReserved2:8;

}FwTxD_STRUC, *PFwTxD_STRUC;
#endif


typedef	struct _FwEventTxD_STRUC 
{
	UINT32	u2RxByteCount:16;
	UINT32	u2PacketType: 16;
	UINT32	ucEID:8;
	UINT32	ucSeqNum:8;
	UINT32	aucReserved0:8;
	UINT32	aucReserved1:8;
}FwEventTxD_STRUC, *PFwEventTxD_STRUC;

typedef	struct _FwCMDReqTxD_STRUC//_FwCMDRequestTxD_STRUC 
{
#ifdef NEW_CMD
	FwDownLoadD_STRUC FwTxD;
#else
	FwTxD_STRUC FwTxD;
#endif
	UINT32	u4Address;
	UINT32	u4Length;
	UINT32	u4DataMode;
}FwCMDReqTxD_STRUC, *PFwCMDReqTxD_STRUC;


typedef	struct _FwCMDRspTxD_STRUC//_FwRequestResponseTxD_STRUC ,_FwStartRResponseTxD_STRUC
{
	FwEventTxD_STRUC FwEventTxD;
	UINT32	ucStatus:8;
	UINT32	aucReserved:24;
}FwCMDRspTxD_STRUC, *PFwCMDRspTxD_STRUC;

typedef	struct _FwStartReqTxD_STRUC//_FwStartRequestTxD_STRUC
{
#ifdef NEW_CMD
	FwDownLoadD_STRUC FwTxD;
#else
	FwTxD_STRUC FwTxD;
#endif
	UINT32	u4Override;
	UINT32	u4Address;
}FwStartReqTxD_STRUC, *PFwStartReqTxD_STRUC;


//List download FW status
#define FW_RSP_STATUS_SUCCESS				0	
/*#define FW_STATUS_NULL							0//Didn't load FW before	
#define FW_STATUS_SEND_RESTART_REQ			1//Send restart request
#define FW_STATUS_GET_RESTART_RSP			2//Get restart response
#define FW_STATUS_SEND_CMD_REQ				3
#define FW_STATUS_GET_CMD_RSP				4
#define FW_STATUS_SCATTERS					5
#define FW_STATUS_CMD_START_REQ				6
#define FW_STATUS_CMD_START_RSP				7*/

/*#define FWD_ID_RESTART		0xef//FW download ID(CID)
#define FWD_ID_RESTART_RSP		0xef
#define FWD_ID_CMD_REQ	0x01
#define FWD_ID_CMD_RSP		0x01
#define FWD_ID_SCATTER		0xee
#define FWD_ID_START_REQ		0x02
#define FWD_ID_START_RSP		0x01*/


#define QA_COMBO_FRAME_TYPE_User1_Random_length	17
#define QA_COMBO_FRAME_TYPE_User2_Inc_length			18
#define QA_COMBO_FRAME_TYPE_User3_Dec_length		19
#define QA_COMBO_FRAME_TYPE_User4_Data_CRC			20


#define QA_COMBO_FRAME_TYPE_Qos_Data					21

#define MAX_LEN_OF_CHANNELS 14

#define   MAX_FRAME_SIZE                                  8192

//***********************************************
// TXRX Page definition
#define RX_ACTION_RX_STOP			0
#define RX_ACTION_RX_START			1
#define RX_ACTION_QUERY			2
#define RX_ACTION_RESET			3
#define RX_ACTION_SNIFF_START		4
#define RX_ACTION_SNIFF_STOP		5

typedef	struct	_HQA_TX_FRAMES_AP {
	BOOLEAN				FromQA;
	TX_WI_STRUC		TxD;		//PCIe : TxD
	//TX_INF_STRUC	TxINF;		//USB : TxInfo
	//TX_WI_STRUC		TxWI;		//TxWI
	UINT32			FrameType;		// Tx frame type
	UINT32			PacketCount;	//TxCount
	UINT16			PacketSize;		//TxLength
	UINT16			FC;			//Frame control
	UINT16			Duration;	//Druation
	UCHAR			Addr1[6];	//DA
	UCHAR			Addr2[6];	//SA
	UCHAR			Addr3[6];	//BSSID
	UINT16			Seq;		//Sequence
} HQA_TX_FRAMES_AP, *PHQA_TX_FRAMES_AP;

typedef	struct	_HQA_TX_FRAMES {
	UINT32		PacketCount;	//TxCount
	UINT16		PacketSize;		//TxLength
	UCHAR		FC[2];			//Frame control
	UCHAR		Duration[2];	//Druation
	UCHAR		Addr1[6];	//DA
	UCHAR		Addr2[6];	//SA
	UCHAR		Addr3[6];	//BSSID	
	UCHAR		Seq[2];		//Sequence
	TX_WI_STRUC	InfoTXWI;	
	int		iRingNumber;
	int		iPatternType;
	CHAR		chPattern[100];
} HQA_TX_FRAMES, *PHQA_TX_FRAMES;

#define BW_20		0 // 20MHz
#define BW_40		1 // 40MHz
#define BW_80		2 // 80MHz
#define BW_160		3 // 160MHz
#define BW_10		4 // 10MHz
#define BW_ALL		0xFF // All BW

#define PATTERN_TYPE_NORMAL				0
#define PATTERN_TYPE_REPEAT				1
#define PATTERN_TYPE_RANDOM				2
//USB
#define USB_BULK_BUFFER_COUNT			20
#define USB_BULK_BUFFER_SIZE				8192
#define USB_BULK_MAX_TRANSFER_SIZE		8192

#define BULKIN_PIPENUMBER_0 0
#define BULKIN_PIPENUMBER_1 1

#define BULKOUT_PIPENUMBER_0 0
#define BULKOUT_PIPENUMBER_1 1
#define BULKOUT_PIPENUMBER_2 2
#define BULKOUT_PIPENUMBER_3 3
#define BULKOUT_PIPENUMBER_4 4
#define BULKOUT_PIPENUMBER_5 5


//efuse
#define EFUSE_LOGICAL_SHOW_EMPTY_VALUE 0xFFFFFFFF
#define EFUSE_TABLE_SIZE					30
#define EFUSE_TABLE_START					0x1E0
#define EFUSE_TABLE_END						0x1FD

//FW Packet command
typedef	struct _BIN_CONNECT_T
{
	UCHAR	ucAddr;
	UCHAR	ucValue;	
}BIN_CONNECT_T, *PBIN_CONNECT_T;

typedef	struct _EXT_CMD_TX_POWER_CTRL_T
{
	UCHAR	ucSourceMode;
	UCHAR	ucChannel;
	UCHAR	ucTargetPowerCount;
	UCHAR	ucCHPowerDeltaCount;
	UCHAR	ucBWPowerDeltaCount;
	UCHAR	ucRatePowerDeltaCount;
	UCHAR	u4Reserve[2];	
}EXT_CMD_TX_POWER_CTRL_T, *PEXT_CMD_TX_POWER_CTRL_T;

typedef	struct _EXT_CMD_SET_THERMO_CAL
{
	UCHAR	ucEnable;
	UCHAR	ucSourceMode;
	UCHAR	ucPeriodTrigger;
	UCHAR	ucTempDiffTrigger;
	UCHAR	ucReserve[2];
	BIN_CONNECT_T	ucThermoSetting[3];	
}EXT_CMD_SET_THERMO_CAL, *PEXT_CMD_SET_THERMO_CAL;

typedef enum _ENUM_MULTI_CR_ACCESS {    
	CMD_MULTI_MAC_CR = 0,    
	CMD_MULTI_RF_CR
} _ENUM_MULTI_CR_ACCESS, *P_ENUM_MULTI_CR_ACCESS;

typedef	struct _CMD_MULTI_CR_ACCESS_T
{
	UINT32  u4Type;//0:mac cr, 1:RF CR
	UINT32	u4Addr;
	UINT32	u4Data;
}CMD_MULTI_CR_ACCESS_T, *PCMD_MULTI_CR_ACCESS_T;

typedef	struct _CMD_ACCESS_REG
{
	UINT32	u4Address;
	UINT32	u4Data;
}CMD_ACCESS_REG, *PCMD_ACCESS_REG;

typedef	struct _CMD_RF_ACCESS_REG
{
	UINT32	u4WiFiStream;//WF0:0 , WF1:1
	UINT32	u4Adddr;
	UINT32	u4Data;
}CMD_RF_ACCESS_REG, *PCMD_RF_ACCESS_REG;

typedef	struct _CMD_ACCESS_EEPROM
{
	UINT32	u4Address;
	UINT32	u4Length;
	UCHAR	aucData[64];
}CMD_ACCESS_EEPROM, *PCMD_ACCESS_EEPROM;

typedef	struct _CMD_ACCESS_EFUSE
{
	UINT32	u4Address;
	UINT32	u4Valid;
	UCHAR	aucData[16];
}CMD_ACCESS_EFUSE, *PCMD_ACCESS_EFUSE;

#define CH_BW20_40		0
#define CH_BW80			1
#define CH_BW160		2
#define CH_BW80Add80	3
#define CH_BW10			4
#define CH_BW5			5


typedef enum _EXT_CMD_CHAN_SWITCH {    
	EXT_CH_BW20 = 0,    
	EXT_CH_BW40,    
	EXT_CH_BW80,
	EXT_CH_BW160,
	EXT_CH_BW10,
	EXT_CH_BW5
} EXT_CMD_CHAN_SWITCH, *PEXT_CMD_CHAN_SWITCH;

typedef struct _CMD_CH_PRIVILEGE_T {
	UCHAR ucBssIndex;
	UCHAR ucTokenID;
	UCHAR ucAction;
	UCHAR ucPrimaryChannel;
	UCHAR ucRfso;
	UCHAR ucRfBand;
	UCHAR ucRfChannelWidth;
	UCHAR ucRfCenterFreqSeq1;
	UCHAR ucRfCenterFreqSeq2;
	UCHAR ucReqType;
	UCHAR auReserved[2];
	//UCHAR u4MaxInterval;
	UINT32 u4MaxInterval;
} CMD_CH_PRIVILEGE_T, *PCMD_CH_PRIVILEGE_T;

typedef struct _EXT_CMD_CHAN_SWITCH_T {
	UINT8 ucCtrCh;
	UINT8 ucCentralCh;
	UINT8 ucBW;
	UINT8 ucTxStreamNum;
	UINT8 ucRxStreamNum;
	UINT8 aucReserve[3];
} EXT_CMD_CHAN_SWITCH_T, *P_EXT_CMD_CHAN_SWITCH_T;

typedef struct _EXT_CMD_SEC_ADDREMOVE_KEY_T {
    
    UINT8      ucAddRemove; /* 0: add, 1: remove */
    UINT8      ucTxKey;   /* no use */
    UINT8      ucKeyType; /* 0: SHAREDKEYTABLE, 1: PAIRWISEKEYTABLE */
    UINT8      ucIsAuthenticator; /* no use */
    UINT8      aucPeerAddr[6];
    UINT8      ucBssIndex; /* no use */
    UINT8      ucAlgorithmId; /* refer to ENUM_CIPHER_SUIT_T256 */
    UINT8      ucKeyId;
    UINT8      ucKeyLen;
	UINT8      ucWlanIndex;
    UINT8      ucReverved;
    UINT8      aucKeyRsc[16]; /* no use */
	UINT8      aucKeyMaterial[32];

} EXT_CMD_SEC_ADDREMOVE_KEY_T, *P_EXT_CMD_SEC_ADDREMOVE_KEY_T;

typedef enum _ENUM_CIPHER_SUIT_T
{
    CIPHER_SUIT_NONE = 0,
    CIPHER_SUIT_WEP_40,
    CIPHER_SUIT_TKIP_W_MIC,
    CIPHER_SUIT_TKIP_WO_MIC,
    CIPHER_SUIT_CCMP_W_MIC, //for DFP or 802.11w MFP
    CIPHER_SUIT_WEP_104,
    CIPHER_SUIT_BIP,
    CIPHER_SUIT_WEP_128 = 7,
    CIPHER_SUIT_WPI,
    CIPHER_SUIT_CCMP_W_MIC_FOR_CCX = 9,  //for DFP or CCX MFP
    CIPHER_SUIT_CCMP_256,
    CIPHER_SUIT_GCMP_128,
    CIPHER_SUIT_GCMP_256
} ENUM_CIPHER_SUIT_T256, *P_ENUM_CIPHER_SUIT_T;

typedef enum _ENUM_CIPHER_ADD_T
{
    CIPHER_ADD = 0,
    CIPHER_REMOVE
} ENUM_CIPHER_ADD_T, *P_ENUM_CIPHER_ADD_T;

	
typedef struct _PARAM_MTK_WIFI_TEST_STRUC_T {
    UINT_32         u4FuncIndex;
    UINT_32         u4FuncData;
} PARAM_MTK_WIFI_TEST_STRUC_T, *P_PARAM_MTK_WIFI_TEST_STRUC_T;
#pragma pack(1)
typedef struct _CMD_TEST_CTRL_T {
    UINT_8          ucAction;
    UINT_8          aucReserved[3];
    union {
        UINT_32                     u4OpMode;
        UINT_32                     u4ChannelFreq;
        PARAM_MTK_WIFI_TEST_STRUC_T rRfATInfo;
    } u;
} CMD_TEST_CTRL_T, *P_CMD_TEST_CTRL_T;
#pragma pack()
#define ACTION_SWITCH_NORMAL_TEST			0
#define ACTION_TRIGGER_RF_TEST				1

#define OPERATION_NORMAL_MODE				0
#define OPERATION_TEST_MODE					1

#define  CAL_2_4G_R 				0
#define  CAL2_4G_TEMP_SENSOR		1
#define  CAL_RX_DCOC				2
#define  CAL_RC						3
#define  CAL_SX_LOGEN				4    	                           
#define  CAL_TX_LOFT				5
#define  CAL_TX_IQ			       	6
#define  CAL_TSSI					7
#define  CAL_TSSI_COMPENSATION		8
#define  CAL_DPD					9
#define  CAL_RX_FIIQ			   10
#define  CAL_RX_FDIQ			   11
#define  CAL_TX_LPFG			   12
#define  CAL_RSSI_DCOC			   13


#define TWO_FOUR_G	1
#define FIVE_G		2

#define FW_PKT_CMD_CH_PRIVILEGE			0x20
#define FW_PKT_ACCESS_REG				0xC2
#define FW_PKT_ACCESS_WF				0xED//WF,eeprom,efuse,calibration 
#define FW_PKT_CMD_CH_PRIVILEGE_EVENT	0x18

#define FW_PKT_EXTEND_ID_EFUSE_RW		0x01
#define FW_PKT_EXTEND_ID_RF_RW			0x02
#define FW_PKT_EXTEND_ID_EEPROM_RW		0x03
#define FW_PKT_EXTEND_ID_RF_TEST		0x04
#define FW_PKT_EXTEND_ID_CHANNEL_SWITCH	0x08
#define FW_PKT_EXTEND_ID_MULTIPLE_REG_ACCESS	0x0E
#define FW_PKT_EXTEND_ID_THERMO_CAL		0x11
#define FW_PKT_EXTEND_ID_ADDREMOVE_KEY	0x10

//WTBL
#define WF_WTBL_ON_BASE		0x28000
#define WTBL_WTBL1DW0		(WF_WTBL_ON_BASE + 0x00)
#ifdef MT7603_FPGA
#define MT_PSE_PAGE_SIZE		256
#define MT_WTBL_SIZE			20
#else
#define MT_PSE_PAGE_SIZE		128
#define MT_WTBL_SIZE			128
#endif


typedef union _WTBL_1_DW0
{
	struct
	{
		UINT32 addr_4:8;
		UINT32 addr_5:8;
		UINT32 muar_idx:6;
		UINT32 rc_a1:1;
		UINT32 kid:2;
		UINT32 rc_id:1;
		UINT32 rkv:1;
		UINT32 ikv:1;
		UINT32 rv:1;
		UINT32 rc_a2:1;
		UINT32 sw:1;
		UINT32 wm:1;
	} field;

	UINT32 word;
} WTBL_1_DW0, *PWTBL_1_DW0;



typedef union _WTBL_1_DW1
{
	struct
	{
		UINT32 addr_0;
	} field;
	
	UINT32 word;
} WTBL_1_DW1, *PWTBL_1_DW1;


typedef enum _WTBL_CIPHER_SUIT
{
	WTBL_CIPHER_NONE = 0,
	WTBL_CIPHER_WEP_40 = 1,
	WTBL_CIPHER_TKIP_MIC = 2,
	WTBL_CIPHER_TKIP_NO_MIC = 3,
	WTBL_CIPHER_CCMP_128_PMF = 4,
	WTBL_CIPHER_WEP_104 = 5,
	WTBL_CIPHER_BIP_CMAC_128 = 6,
	WTBL_CIPHER_WEP_128 = 7,
	WTBL_CIPHER_WPI_128 = 8,
	WTBL_CIPHER_CCMP_128_CCX = 9,
	WTBL_CIPHER_CCMP_256 = 10,
	WTBL_CIPHER_GCMP_128 = 11,
	WTBL_CIPHER_GCMP_256 = 12,
} WTBL_CIPHER_SUIT, *PWTBL_CIPHER_SUIT;


typedef union _WTBL_1_DW2
{
	struct
	{
		UINT32 mm:3;
		UINT32 cipher_suit:4;
		UINT32 wpi_even:1;
		UINT32 td:1;
		UINT32 fd:1;
		UINT32 dis_rhtr:1;
		UINT32 af:3;
		UINT32 rx_ps:1;
		UINT32 r:1;
		UINT32 rts:1;
		UINT32 cf_ack:1;
		UINT32 rdg_ba:1;
		UINT32 smps:1;
		UINT32 baf_en:1;
		UINT32 dyn_bw:1;
		UINT32 ldpc:1;
		UINT32 tibf:1;
		UINT32 tebf:1;
		UINT32 txop_ps_cap:1;
		UINT32 mesh:1;
		UINT32 qos:1;
		UINT32 ht:1;
		UINT32 vht:1;
		UINT32 adm:1;
		UINT32 gid:1;
	}field;
	
	UINT32 word;
}WTBL_1_DW2, *PWTBL_1_DW2;

typedef union _WTBL_1_DW3
{
	struct
	{
		UINT32 wtbl2_fid:11;
		UINT32 wtbl2_eid:5;
		UINT32 wtbl4_fid:11;
		UINT32 chk_per:1;
		UINT32 du_j_psm:1;
		UINT32 i_psm:1;
		UINT32 psm:1;
		UINT32 skip_tx:1;
	} field;
	
	UINT32 word;
} WTBL_1_DW3, *PWTBL_1_DW3;

typedef union _WTBL_1_DW4
{
	struct
	{
		UINT32 wtbl3_fid:11;
		UINT32 wtbl3_eid:5;
		UINT32 wtbl4_eid:6;
		UINT32 partial_aid:9;
	} field;
	
	UINT32 word;
} WTBL_1_DW4, *PWTBL_1_DW4;


#define WTBL_BTCRn		(WF_WTBL_ON_BASE + 0x2000)

#define GNU_PACKED
typedef struct _WTBL_1_STRUC
{
	WTBL_1_DW0 WTBL1DW0;
	WTBL_1_DW1 WTBL1DW1;
	WTBL_1_DW2 WTBL1DW2;
	WTBL_1_DW3 WTBL1DW3;
	WTBL_1_DW4 WTBL1DW4;
} WTBL_1_STRUC, *PWTBL_1_STRUC;

/*
	WTBL segment 2 definitions
*/
typedef struct _WTBL_TX_RATE
{
	UINT16	TxRate:6;
	UINT16	TxMode:3;
	UINT16	n_sts:2;
	UINT16	STBC:1;
	UINT16	Reserve:4;
} WTBL_TX_RATE, *PWTBL_TX_RATE;

typedef struct _WTBL_2_STRUC
{
	UINT32 pn_0;

	UINT32 pn_32:16;
	UINT32 com_sn:12;
	UINT32 rsv_1_28:4;

	UINT32 tid_ac_0_sn:12;
	UINT32 tid_ac_1_sn:12;
	UINT32 tid_ac_2_sn_0:8;

	UINT32 tid_ac_2_sn_9:4;
	UINT32 tid_ac_3_sn:12;
	UINT32 tid_4_sn:12;
	UINT32 tid_5_sn_0:4;

	UINT32 tid_5_sn_5:8;
	UINT32 tid_6_sn:12;
	UINT32 tid_7_sn:12;

	UINT32 rate_1_tx_cnt:16;
	UINT32 rate_1_fail_cnt:16;

	UINT32 rate_2_tx_cnt:8;
	UINT32 rate_3_tx_cnt:8;
	UINT32 rate_4_tx_cnt:8;
	UINT32 rate_5_tx_cnt:8;

	UINT32 current_bw_tx_cnt:16;
	UINT32 current_bw_fail_cnt:16;

	UINT32 other_bw_tx_cnt:16;
	UINT32 other_bw_fail_cnt:16;

	UINT32 tx_pwr_offset:5;
	UINT32 spe_en:1;
	UINT32 ant_pri:3;
	UINT32 ccbw_sel:2;
	UINT32 cbrn:3;
	UINT32 fcap:2;
	UINT32 g2:1;
	UINT32 g4:1;
	UINT32 g8:1;
	UINT32 g16:1;
	UINT32 rsv_9_20:3;
	UINT32 mpdu_fail_cnt:3;
	UINT32 mpdu_ok_cnt:3;
	UINT32 rate_idx:3;

	UINT32 rate_1:12;  /* format refer to WTBL_TX_RATE */
	UINT32 rate_2:12;
	UINT32 rate_3_0:8;

	UINT32 rate_3_8:4;
	UINT32 rate_4:12;
	UINT32 rate_5:12;
	UINT32 rate_6_0:4;
	
	UINT32 rate_6_4:8;
	UINT32 rate_7:12;
	UINT32 rate_8:12;

	UINT32 resp_rcpi_0:8;
	UINT32 resp_rcpi_1:8;
	UINT32 resp_rcpi_2:8;
	UINT32 rsv_13_24:8;

	UINT32 sts_1_ch_cap_noise:7;
	UINT32 sts_2_ch_cap_noise:7;
	UINT32 sts_3_ch_cap_noise:7;
	UINT32 ce_rmsd:4;
	UINT32 cc_noise_sel:1;
	UINT32 ant_sel:6;

	UINT32 ba_win_size_tid_0:3;
	UINT32 ba_win_size_tid_1:3;
	UINT32 ba_win_size_tid_2:3;
	UINT32 ba_win_size_tid_3:3;
	UINT32 ba_win_size_tid_4:3;
	UINT32 ba_win_size_tid_5:3;
	UINT32 ba_win_size_tid_6:3;
	UINT32 ba_win_size_tid_7:3;
	UINT32 ba_en:8;
}WTBL_2_STRUC, *PWTBL_2_STRUC;


/*
	WTBL segment 3 definitions
*/

/* Used for Cipher Suit != (WEP_xx | WPI) */
typedef struct _WTBL_CIPHER_128
{
	UINT32 cipher_key_0;
	UINT32 cipher_key_32;
	UINT32 cipher_key_64;
	UINT32 cipher_key_96;

	UINT32 rx_mic_bip_key_0;
	UINT32 rx_mic_bip_key_32;

	UINT32 tx_mic_bip_key_0;
	UINT32 tx_mic_bip_key_32;
} WTBL_CIPHER_128, *PWTBL_CIPHER_128;

/* Used for CCMP-256 /GCMP-256, store cipher or MIC key only */
typedef struct _WTBL_CIPHER_256 {
	UINT32 key_0;
	UINT32 key_1;
	UINT32 key_2;
	UINT32 key_3;
	UINT32 key_4;
	UINT32 key_5;
	UINT32 key_6;
	UINT32 key_7;
} WTBL_CIPHER_256, *PWTBL_CIPHER_256;

/* Used for Cipher Suit == WPI */
typedef struct _WTBL_CIPHER_WPI
{
	UINT32 cipher_key0_0;
	UINT32 cipher_key0_1;
	UINT32 cipher_key0_2;
	UINT32 cipher_key0_3;

	UINT32 mic_key0_0;
	UINT32 mic_key0_1;
	UINT32 mic_key0_2;
	UINT32 mic_key0_3;

	UINT32 cipher_key1_0;
	UINT32 cipher_key1_1;
	UINT32 cipher_key1_2;
	UINT32 cipher_key1_3;

	UINT32 mic_key1_0;
	UINT32 mic_key1_1;
	UINT32 mic_key1_2;
	UINT32 mic_key1_3;
} WTBL_CIPHER_WPI, *PWTBL_CIPHER_WPI;

/* Used for Cipher Suit == WEP */
typedef struct _WTBL_CIPHER_WEP
{
	UINT32 cipher_key0_0;
	UINT32 cipher_key0_1;
	UINT32 cipher_key0_2;
	UINT32 cipher_key0_3;

	UINT32 cipher_key1_0;
	UINT32 cipher_key1_1;
	UINT32 cipher_key1_2;
	UINT32 cipher_key1_3;

	UINT32 cipher_key2_0;
	UINT32 cipher_key2_1;
	UINT32 cipher_key2_2;
	UINT32 cipher_key2_3;

	UINT32 cipher_key3_0;
	UINT32 cipher_key3_1;
	UINT32 cipher_key3_2;
	UINT32 cipher_key3_3;
} WTBL_CIPHER_WEP, *PWTBL_CIPHER_WEP;

typedef union _WTBL_3_STRUC
{
	WTBL_CIPHER_WEP	CipherWEP;
	WTBL_CIPHER_WPI	CipherWPI;
	WTBL_CIPHER_128		Cipher128;
	WTBL_CIPHER_256		Cipher256;
} WTBL_3_STRUC, *PWTBL_3_STRUC;

/*
	WTBL segment 4 definitions
*/
typedef union _WTBL_4_AC
{
	struct {
		/* DWORD 0 */
		UINT32 ac_ctt:24;
		UINT32 rsv_0_24:8;

		/* DWORD 1 */
		UINT32 ac_ctb;
	}field;

	UINT32 word[2];
} WTBL_4_AC, *PWTBL_4_AC;


typedef struct _WTBL_4_STRUC
{
	WTBL_4_AC	AC0;
	WTBL_4_AC	AC1;
	WTBL_4_AC	AC2;
	WTBL_4_AC	AC3;
} WTBL_4_STRUC, *PWTBL_4_STRUC;

typedef struct _WLAN_TABLE_ENTRY {
	UCHAR	Index; // WLan Table Index

	WTBL_1_STRUC	WTBL1;	//	struct wtbl_1_struc wtbl_1;
	WTBL_2_STRUC	WTBL2;	//	struct wtbl_2_struc wtbl_2;
	WTBL_3_STRUC	WTBL3;	//	union wtbl_3_struc wtbl_3;
	WTBL_4_STRUC	WTBL4;	//	struct wtbl_4_struc wtbl_4;

	UINT32	Address[4];	// Address of WLan Table 1/2/3/4
	UINT16	FrameId[4];	// Frame Id of WLan Table 1/2/3/4
	UINT16	EntryId[4];	// Entry Id of WLan Table 1/2/3/4
} WLAN_TABLE_ENTRY, *PWLAN_TABLE_ENTRY;

typedef struct _WTBL_CONTROL
{
	UCHAR	WTBLEntryCount[4]; //wtbl_entry_cnt[4];
	UCHAR	WTBLEntrySize[4]; //wtbl_entry_size[4];
	UINT32	WTBLBaseAddress[4]; //wtbl_base_addr[4]; // base address for WTBL2/3/4
	UINT32	WTBLBaseFrameId[4]; //wtbl_base_fid[4];
	UINT32	PageSize; //page_size
} WTBL_CONTROL, *PWTBL_CONTROL;

//
#define MCAST_WCID	0x0	 
#define BSS0Mcast_WCID	0x0	 
#define RESERVED_WCID	0xff

// definition of pAd->OpMode
#define OPMODE_STA                  0
#define OPMODE_AP                   1
#define OPMODE_APCLIENT             2

#define ASIC_MAC_TX		1
#define ASIC_MAC_RX		2
#define ASIC_MAC_TXRX	3

#define ARB_SCR		(WF_ARB_BASE + 0x080)  /* 0x21480 */
#define MT_ARB_SCR_TXDIS	(1 << 8)
#define MT_ARB_SCR_RXDIS	(1 << 9)

/* TMAC_TXD_1.hdr_format */
#define TMI_HDR_FT_NON_80211	0x0
#define TMI_HDR_FT_CMD		0x1
#define TMI_HDR_FT_NOR_80211	0x2//normal_80211
#define TMI_HDR_FT_ENH_80211	0x3

#define MT_TX_RETRY_UNLIMIT		0x1f
#define MT_TX_RETRY_LIMITED		0x0f
#define MT_TX_RETRY_CNT			MT_TX_RETRY_LIMITED

/* TXDSCR_DW6.FixedRateMode */
#define TMI_FIX_RATE_BY_TXD	0
#define TMI_FIX_RATE_BY_CR		1

#define TXD_BANDWIDTH_NOFIXED_BW		0
#define TXD_BANDWIDTH_BW20				4
#define TXD_BANDWIDTH_BW40				5
#define TXD_BANDWIDTH_BW80				6
#define TXD_BANDWIDTH_BW160				7


#define MAC_ADDR_LEN 6
// 2-byte Frame control field
typedef	struct	{
	UINT16		Ver:2;				// Protocol version
	UINT16		Type:2;				// MSDU type
	UINT16		SubType:4;			// MSDU subtype
	UINT16		ToDs:1;				// To DS indication
	UINT16		FrDs:1;				// From DS indication
	UINT16		MoreFrag:1;			// More fragment bit
	UINT16		Retry:1;			// Retry status bit
	UINT16		PwrMgmt:1;			// Power management bit
	UINT16		MoreData:1;			// More data bit
	UINT16		Wep:1;				// Wep data
	UINT16		Order:1;			// Strict order expected
}	QAFRAME_CONTROL, *PQAFRAME_CONTROL;
typedef	struct	_QAHEADER_802_11	{
	QAFRAME_CONTROL   FC;
	UINT16          Duration;
	UCHAR           Addr1[MAC_ADDR_LEN];
	UCHAR           Addr2[MAC_ADDR_LEN];
	UCHAR			Addr3[MAC_ADDR_LEN];
	UINT16			Frag:4;
	UINT16			Sequence:12;
}	QAHEADER_802_11, *PQAHEADER_802_11;

typedef struct GNU_PACKED tmac_txd_0 {
	/* DWORD 0 */
	UINT32 tx_byte_cnt:16; 		/* in unit of bytes */
	UINT32 eth_type_offset:7;		/* in unit of WORD(2 bytes) */
	UINT32 ip_sum:1;
	UINT32 ut_sum:1;
	UINT32 UNxV:1;
	UINT32 UTxB:1;
	UINT32 q_idx:4;				/* Q_IDX_XXX */
	UINT32 p_idx:1;				/* P_IDX_XXX */
}TMAC_TXD_0;

typedef struct GNU_PACKED tmac_txd_1 {
	/* DWORD 1 */
	UINT32 wlan_idx:8;
	UINT32 hdr_info:5;			/* in unit of WORD(2 bytes) */
	UINT32 hdr_format:2;			/* TMI_HDR_FT_XXX */
	UINT32 ft:1;					/* TMI_FT_XXXX */
	UINT32 hdr_pad:3;
	UINT32 no_ack:1;
	UINT32 tid:3;
	UINT32 protect_frm:1;
	UINT32 rsv_1_24:2;
	UINT32 own_mac:6;
}TMAC_TXD_1;

typedef struct GNU_PACKED tmac_txd_2 {
	/* DWORD 2 */
	UINT32 sub_type:4;
	UINT32 frm_type:2;
	UINT32 ndp:1;
	UINT32 ndpa:1;
	UINT32 sounding:1;
	UINT32 rts:1;
	UINT32 bc_mc_pkt:1;
	UINT32 bip:1;
	UINT32 duration:1;
	UINT32 htc_vld:1;
	UINT32 frag:2;
	UINT32 max_tx_time:8;
	UINT32 pwr_offset:5;
	UINT32 ba_disable:1;
	UINT32 timing_measure:1;	
	UINT32 fix_rate:1;
}TMAC_TXD_2;

typedef struct GNU_PACKED tmac_txd_3 {
	/* DWORD 3 */
	UINT32 rsv3_0:6;
	UINT32 tx_cnt:5;
	UINT32 remain_tx_cnt:5;
	UINT32 sn:12;
	UINT32 rsv3_28:2;
	UINT32 pn_vld:1;	
	UINT32 sn_vld:1;
}TMAC_TXD_3;

typedef struct GNU_PACKED tmac_txd_4 {
	/* DWORD 4 */
	UINT32 pn_low;
}TMAC_TXD_4;

typedef struct GNU_PACKED tmac_txd_5 {
	/* DWORD 5 */
	UINT32 pid:8;
	UINT32 tx_status_fmt:1;
	UINT32 tx_status_2_mcu:1;
	UINT32 tx_status_2_host:1;
	UINT32 da_select:1;
	UINT32 bar_sn_ctrl:1;
	UINT32 pwr_mgmt:1;
	UINT32 rsv5_14:2;	
	UINT32 pn_high:16;
}TMAC_TXD_5;



typedef struct GNU_PACKED tmac_txd_6 {
	/* DWORD 6 */
	UINT32 fix_rate_mode:1;
	UINT32 rsv_6_1:1;
	UINT32 ant_id:6;
	UINT32 bw:3;
	UINT32 spe_en:1;
	UINT32 ant_pri:3;
	UINT32 dyn_bw:1;
	UINT32 ETxBF:1;
	UINT32 ITxBF:1;
	UINT32 tx_rate:12;
	UINT32 ldpc:1;
	UINT32 gi:1;
}TMAC_TXD_6;

/* TMAC_TXD_5.pwr_mgmt */
#define TMI_PM_BIT_CFG_BY_HW	0
#define TMI_PM_BIT_CFG_BY_SW	1

#define PID_MGMT			0x05
#define PID_BEACON			0x0c
#define PID_DATA_NORMALUCAST	 	0x02
#define PID_DATA_AMPDU	 	0x04
#define PID_DATA_NO_ACK    	0x08
#define PID_DATA_NOT_NORM_ACK	 	0x03

#define DMA_SCH_LMAC		0
#define DMA_SCH_BYPASS		1
#define DMA_SCH_HYBRID		2

#define CONTI_STF 					0
#define CONTI_LTF 					1
#define CONTI_OFDM 					2
#define CONTI_CCK 					3
#define CONTI_CCK_PI2_ROTATE		4
#define CONTI_CCK_PI1_ROTATE		5
#define CONTI_STOP					10

#define CONTI_5M_TONE 					0
#define CONTI_10M_TONE 					1
#define CONTI_20M_TONE 					2
#define CONTI_RAMP		 				3
#define CONTI_DC						4

#define CMD_ASIC_TOPINIT	0x1
#define CMD_HIF_INIT		0x2
#define CMD_TMAC_INIT		0x3
#define CMD_INITMAC			0x4

#define CMD_CONTI_TX_START						0x10
#define CMD_CONTI_TX_STOP							0x11
#define CMD_CONTI_TONE_START						0x12
#define CMD_CONTI_TONE_STOP						0x13
#define CMD_GET_MAC_MDRDY							0x14
#define CMD_SET_TXPOWER							0x15
#define CMD_SET_AIFS								0x16//(IPG)
#define CMD_FREQ_OFFSET							0x17
#define CMD_TXPOWER_RANGE							0x18
#define CMD_GET_PHY_MDRDY							0x19
#define CMD_GET_DRIVER_RX_COUNT					0x1A
#define CMD_GET_RXSNR								0x1B
#define CMD_GET_RCPI								0x1C
#define CMD_GET_FREQOFFSETFROM_RX				0x1D
#define CMD_SET_TSSI								0x1E
#define CMD_SET_DPD								0x1F
#define CMD_GET_TX_COUNT							0x20//
#define CMD_GET_MACFCSERRORCNT_COUNT			0x21//Get Mac fcsERROR COUNT
#define CMD_GET_PHY_MDRDYCOUNT					0x22
#define CMD_GET_PHY_FCSERRCOUNT					0x23
#define CMD_GET_PD									0x24
#define CMD_GET_CCK_SIG_SFD						0x25
#define CMD_GET_OFDM_ERR							0x26
#define CMD_GET_RSSI								0x27
#define CMD_SET_STBC								0x28
#define CMD_SET_SGI									0x29
#define CMD_TMR_SETTING							0x2A
#define CMD_GET_SECURITY_RESULT					0x2B
#define CMD_SET_LDPC							0x2C

#define PKTCMD_EVENT_BUFFER 128
/*
// MCS for CCK.  BW.SGI.STBC are reserved
#define MCS_LONGP_RATE_1                      0	 // long preamble CCK 1Mbps
#define MCS_LONGP_RATE_2                      1	// long preamble CCK 1Mbps
#define MCS_LONGP_RATE_5_5                    2
#define MCS_LONGP_RATE_11                     3
#define MCS_SHORTP_RATE_1                      4	 // long preamble CCK 1Mbps. short is forbidden in 1Mbps
#define MCS_SHORTP_RATE_2                      5	// short preamble CCK 2Mbps
#define MCS_SHORTP_RATE_5_5                    6
#define MCS_SHORTP_RATE_11                     7
// To send duplicate legacy OFDM. set BW=BW_40.  SGI.STBC are reserved
#define MCS_RATE_6                      0   // legacy OFDM
#define MCS_RATE_9                      1   // OFDM
#define MCS_RATE_12                     2   // OFDM
#define MCS_RATE_18                     3   // OFDM
#define MCS_RATE_24                     4  // OFDM
#define MCS_RATE_36                     5   // OFDM
#define MCS_RATE_48                     6  // OFDM
#define MCS_RATE_54                     7 // OFDM	
*/
// HT
#define MCS_0		0	// 1S
#define MCS_1		1
#define MCS_2		2
#define MCS_3		3
#define MCS_4		4
#define MCS_5		5
#define MCS_6		6
#define MCS_7		7
#define MCS_8		8	// 2S
#define MCS_9		9
#define MCS_10		10
#define MCS_11		11
#define MCS_12		12
#define MCS_13		13
#define MCS_14		14
#define MCS_15		15
#define MCS_16		16	// 3*3
#define MCS_17		17
#define MCS_18		18
#define MCS_19		19
#define MCS_20		20
#define MCS_21		21
#define MCS_22		22
#define MCS_23		23
#define MCS_32		32
#define MCS_AUTO	33
#define MCS_AUTO_SGI_GF	77

// OID_HTPHYMODE
// Extension channel offset
#define EXTCHA_ABOVE	0
#define EXTCHA_BELOW	1


/* Used for HAL */
typedef enum _ENUM_MODULATION_SYSTEM_T {    
	MODULATION_SYSTEM_CCK = 0,    
	MODULATION_SYSTEM_OFDM,    
	MODULATION_SYSTEM_HT20,    
	MODULATION_SYSTEM_HT40,    
	MODULATION_SYSTEM_NUM
} ENUM_MODULATION_SYSTEM_T, *P_ENUM_MODULATION_SYSTEM_T;

/* Used for HAL */
typedef enum _ENUM_MODULATION_TYPE_T {    
	MODULATION_TYPE_CCK_BPSK = 0, // including 5.5M, 11M    
	MODULATION_TYPE_DSSS_QPSK,    // including 1M, 2M    
	MODULATION_TYPE_16QAM,    
	MODULATION_TYPE_64QAM,    
	MODULATION_TYPE_MCS6,    
	MODULATION_TYPE_54M_MCS7,    
	MODULATION_TYPE_MCS32,    
	MODULATION_TYPE_NUM
} ENUM_MODULATION_TYPE_T, *P_ENUM_MODULATION_TYPE_T;

typedef struct RX_STASTIC {
	UINT32 FCSErr_CCK;
	UINT32 FCSErr_OFDM;
	UINT32 CCK_PD;
	UINT32 OFDM_PD;
	UINT32 CCK_SIG_Err;
	UINT32 CCK_SFD_Err;
	UINT32 OFDM_SIG_Err;
	UINT32 OFDM_TAG_Err;
	ULONG WB_RSSI0;
	ULONG IB_RSSI0;
	ULONG WB_RSSI1;	
	ULONG IB_RSSI1;
	UINT32 PhyMdrdyCCK;
	UINT32 PhyMdrdyOFDM;
	UINT32 DriverRxCount;
	UINT32 RCPI0;
	UINT32 RCPI1;	
	UINT32 FreqOffsetFromRX;
	UINT32 MACMdrdy;
	UINT32 RXOK;
	float PER;
}RX_STASTIC, *PRX_STASTIC;

//TX Status
typedef struct TX_STATUS_DW0 {
	UINT32 TXRate:12;
	UINT32 FR:1;
	UINT32 TXSFM:1;
	UINT32 TXS2M:1;
	UINT32 TXS2H:1;
	UINT32 ME:1;
	UINT32 RE:1;
	UINT32 LE:1;
	UINT32 BE:1;
	UINT32 TXOP:1;	
	UINT32 PS:1;
	UINT32 BAF:1;
	UINT32 TID:3;
	UINT32 ANT_ID:6;	
}TX_STATUS_DW0, *PTX_STATUS_DW0;

typedef struct TX_STATUS_DW1 {//TXSFM =1
	UINT32 Noise0:8;
	UINT32 Noise1:8;
	UINT32 Noise2:8;
	UINT32 Reserve:8;
}TX_STATUS_DW1, *PTX_STATUS_DW1;

typedef struct TX_STATUS_DW2 {//TXSFM =1
	UINT32 RCPI0:8;
	UINT32 RCPI1:8;
	UINT32 RCPI2:8;
	UINT32 Reserve:1;
	UINT32 TXPower:7;
}TX_STATUS_DW2, *PTX_STATUS_DW2;

typedef struct TX_STATUS_DW3 {//TXSFM =1
	UINT32 TransmissionDelay:16;
	UINT32 RXV_SN:8;
	UINT32 WLAN_Index:8;	
}TX_STATUS_DW3, *PTX_STATUS_DW3;

typedef struct TX_STATUS_DW4 {//TXSFM =1
	UINT32 TSSI:12;
	UINT32 TBW:2;
	UINT32 PID:8;	
	UINT32 FM:1;	
	UINT32 AM:1;	
	UINT32 MPDUTXCount:5;	
	UINT32 LastTXRateIndex:3;	
}TX_STATUS_DW4, *PTX_STATUS_DW4;

typedef struct TX_STATUS {//TXSFM =1
	TX_STATUS_DW0 DW0;	
	TX_STATUS_DW1 DW1;	
	TX_STATUS_DW2 DW2;	
	TX_STATUS_DW3 DW3;	
	TX_STATUS_DW4 DW4;	
}TX_STATUS, *PTX_STATUS;

//RXINFO
typedef struct RX_INFO_DW0 {//RX vector, group3
	UINT32 RxByteCount:16;
	UINT32 EtherTypeoffset:7;
	UINT32 IP:1;
	UINT32 UT:1;
	UINT32 GROUP_VLD:4;
	UINT32 PKT_TYPE:3;
}RX_INFO_DW0, *PRX_INFO_DW0;

//RX vector, group3
typedef struct RX_G3ONE_VECTOR {//first vector//RX vector, group3
	UINT32 txrate:7;
	UINT32 HT_stbc:2;
	UINT32 HT_adcode:1;
	UINT32 HT_extitf:2;
	UINT32 txmode:3;
	UINT32 frmode:2;
	UINT32 VHTA1_B22:1;
	UINT32 HT_aggregation:1;
	UINT32 HT_shortgi:1;
	UINT32 HT_smooth:1;
	UINT32 HT_no_sound:1;
	UINT32 VHTA2_B8_B1:8;
	UINT32 VHTA1_B5_B4:2;
}RX_G3ONE_VECTOR, *PRX_G3ONE_VECTOR;

typedef struct RX_G3TWO_VECTOR {//first vector//RX vector, group3
	UINT32 Length :21;
	UINT32 VHTA1_B16_B6:11;	
}RX_G3TWO_VECTOR, *PRX_G3TWO_VECTOR;

typedef struct RX_G3THREE_VECTOR {//first vector//RX vector, group3
	UINT32 VHTA1_B21_B17 :5;
	UINT32 OFDM_FreqTrans_DET:1;
	UINT32 ACI_DETx:1;
	UINT32 SEL_ANT:1;
	UINT32 RCPI0:8;
	UINT32 FAGC0_EQ_CAL:1;
	UINT32 FGAC0_CAL_GAIN:3;
	UINT32 RCPI1:8;
	UINT32 FAGC1_EQ_CAL:1;
	UINT32 FGAC1_CAL_GAIN:3;
}RX_G3THREE_VECTOR, *PRX_G3THREE_VECTOR;


typedef struct RX_G3FOUR_VECTOR {//RX vector, group3
	UINT32 IB_RSSIx:8;
	UINT32 WB_RSSIx:8;
	UINT32 FAGC_LPF_GAINx:4;
	UINT32 IB_RSSI1:8;
	UINT32 FAGC_LPF_GAIN1:4;		
}RX_G3FOUR_VECTOR, *PRX_G3FOUR_VECTOR;

typedef struct RX_G3FIVE_VECTOR {//RX vector, group3
	UINT32 FAGC_LNA_GAIN0:2;
	UINT32 FAGC_LNA_GAIN1:2;
	UINT32 cagc_state:3;
	//MISC
	UINT32 FOE:12;
	UINT32 LTF_PROC_TIME:7;
	UINT32 LTF_SNR0:6;//only for OFDM	
}RX_G3FIVE_VECTOR, *PRX_G3FIVE_VECTOR;

typedef struct RX_G3SIX_VECTOR {//RX vector, group3
	UINT32 NF0:8;
	UINT32 NF1:8;
	UINT32 WB_RSSI1:8;
	UINT32 RXValidIndicator:1;
	UINT32 NSTSFiled:3;
	UINT32 cagc_state0:3;//only for OFDM
	UINT32 Reserved:1;
}RX_G3SIX_VECTOR, *PRX_G3SIX_VECTOR;


typedef struct RX_V_GROUP3 {//first vector//RX vector, group3
	RX_G3ONE_VECTOR RXV1;
	RX_G3TWO_VECTOR RXV2;
	RX_G3THREE_VECTOR RXV3;
	RX_G3FOUR_VECTOR RXV4;
	RX_G3FIVE_VECTOR RXV5;
	RX_G3SIX_VECTOR RXV6;
}RX_V_GROUP3, *PRX_V_GROUP3;


//RXV group1
typedef struct RX_V_GROUP1 {//first vector//RX vector, group3
	UINT32 reserve1;
	UINT32 reserve2;
	UINT32 reserve3;
	UINT32 reserve4;
}RX_V_GROUP1, *PRX_V_GROUP1;
//RXV group2
typedef struct RX_V_GROUP2 {//first vector//RX vector, group3
	UINT32 Timestamp;
	UINT32 CRC;
}RX_V_GROUP2, *PRX_V_GROUP2;

//RXV group2
typedef struct RX_V_GROUP4 {//first vector//RX vector, group3
	UINT32 reserve1;
	UINT32 reserve2;
	UINT32 reserve3;
	UINT32 reserve4;
}RX_V_GROUP4, *PRX_V_GROUP4;

typedef struct RXV_HEADER {//RXvector header
	UINT32 RXByteCount:16;//DW0
	UINT32 RXV_CNT:5;
	UINT32 Reserved:8;//DW1
	UINT32 PKT_TYPE:3;
}RXV_HEADER, *PRXV_HEADER;

typedef struct RXV_FIRST_CYCLE {//after payload
	UINT32 Prim_ITFR_ENV:1;//DW0
	UINT32 Sec_ITFR_EVN:1;
	UINT32 Sec40_ITFR_EVN:1;//DW1
	UINT32 BT_EVN:1;
	UINT32 OFDM_BPSK_LQ:7;
	UINT32 Reserved:21;
}RXV_FIRST_CYCLE, *PRXV_FIRST_CYCLE;

typedef struct RXV_SECOND_CYCLE {//after payload
	UINT32 Reserved1:7;//DW0
	UINT32 Capacity_LQ:7;
	UINT32 BTD_NOTCH_LOC:7;//DW1
	UINT32 OFDM_LTF_SNR1:6;//RX1
	UINT32 Reserved2:2;
	UINT32 DYN_BANDWIDTH_IN_NOT_HT:1;
	UINT32 CH_BANDWIDTH_IN_NOT_HT:2;
}RXV_SECOND_CYCLE, *PRXV_SECOND_CYCLE;

typedef struct RXV_THREE_CYCLE {//after payload
	UINT32 HT_STF_DET:1;
	UINT32 BFAGC_LNA_GAINx:2;
	UINT32 Reserved1:1;
	UINT32 BFAGC_LPF_GAINx:4;
	UINT32 BFAGC_IB_RSSIx:8;
	UINT32 BFAGC_WB_RSSIx:8;
	UINT32 Reserved2:1;
	UINT32 OFDM_CE_RMSD_Id:3;
	UINT32 OFDM_CE_GIC_ENB:1;
	UINT32 OFDM_CE_LTF_COMB:1;
	UINT32 OFDM_DEW_MODE_DET:1;
	UINT32 FCS_ERR:1;
}RXV_THREE_CYCLE, *PRXV_THREE_CYCLE;

typedef struct RXV_AFTERPAYLOAD {//RXvector
	RXV_FIRST_CYCLE FirstCycle;
	RXV_SECOND_CYCLE SecondCycle;
	RXV_THREE_CYCLE ThreeCycle;
}RXV_AFTERPAYLOAD, *PRXV_AFTERPAYLOAD;

typedef struct RXV {//RXvector
	UINT32 TA0;//DW0
	UINT32 TA1:16;
	UINT32 RXV_SN:8;//DW1
	UINT32 TR:1;
	UINT32 Reserved:7;
	RX_V_GROUP3 vector1;
	RXV_AFTERPAYLOAD vector2;
}RXV, *PRXV;

#define RX_PKT_TYPE_RX_TXS		0x0
#define RX_PKT_TYPE_RX_TXRXV	0x1
#define RX_PKT_TYPE_RX_NORMAL	0x2
#define RX_PKT_TYPE_RX_DUP_FRB	0x3
#define RX_PKT_TYPE_RX_TMR		0x4

//Continue TX
#define CONUTI_TX_CCK_1M_WF0		0x0
#define CONUTI_TX_CCK_1M_WF1		0x1
#define CONUTI_TX_OFDM_6M_WF0		0x2
#define CONUTI_TX_OFDM_6M_WF1		0x3
#define CONUTI_TX_HT20_MCS0_WF0		0x4
#define CONUTI_TX_HT20_MCS0_WF1		0x5
#define CONUTI_TX_HT40_MCS0_WF0		0x6
#define CONUTI_TX_HT40_MCS0_WF1		0x7
//Tone
#define WF0_TX_ONE_TONE_5M		0x0
#define WF0_TX_TWO_TONE_5M		0x1
#define WF1_TX_ONE_TONE_5M		0x2
#define WF1_TX_TWO_TONE_5M		0x3
#define WF0_TX_ONE_TONE_10M		0x4
#define WF1_TX_ONE_TONE_10M		0x5
#define WF0_TX_ONE_TONE_DC		0x6
#define WF1_TX_ONE_TONE_DC		0x7



//SetChannel INFO
#define SETCH_2_4G		0x1
#define SETCH_5G		0x2

#define SETCH_SCN		0x0//none
#define SETCH_SCA		0x1//none, 40MB above
#define SETCH_SCB		0x3//none, 40MB below
#define SETCH_BW_20_40	0x0//none, 20 or 40BW
#define SETCH_BW_80		0x1//none,	80MBW
#define SETCH_BW_160	0x2//none,	160MBW
#define SETCH_BW_80_80	0x3//none,	80+80MBW

#define MAXFIRMWARESIZE	1024*256

//#define BUFFER_MODE	0x0
//#define EEPROM_MODE	0x1
//#define EFUSE_MODE		0x2

/* Used for Rom Mode */
typedef enum _ENUM_ROM_MODE {    
	MODE_BUFFER = 0,
	MODE_EFUSE,
	MODE_EEPROM,
} ENUM_ROM_MODE, *P_ENUM__ROM_MODE;

#define EFUSESIZE 512
typedef struct _EFUSE_SETTINGS {
	USHORT ChipID;
	ULONG TxPower;
	USHORT FrequencyOffset;
	UCHAR  Tssi;
	UCHAR  ePA;
} EFUSE_SETTINGS, *PEFUSE_SETTINGS;

typedef enum _ENUM_TMR_SETTING {    
	TMR_DISABLE = 0,    
	TMR_INITIALZER,    
	TMR_RESPONDER
} ENUM_TMR_SETTING, *PENUM_TMR_SETTING;

typedef enum _ENUM_DOFW_TYPE {    
	DOFW_TYPE_7603 = 0,
	DOFW_TYPE_7636_ROM_PATCH,
	DOFW_TYPE_7636_RAM_DOWNLOAD,
} ENUM_DOFW_TYPE, *P_ENUM_DOFW_TYPE;

#endif
