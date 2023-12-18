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
    Rory Chen   11-18-2002    created

*/

// Bug List : (Not Change yet)
// --> pComTxBuf resource doesn't release buffer.
//
// Change List :
//
// 0. [DebugPrint] --> for dump message
//	(1) ¥[¤J DebugPrint define
//
// 1. [040816 - P1] Old Hardware Control invalid
//	(a) Old Hardware Control replace to Dummy.
//	(b) New Hardware Control add "Real" word.
//
// 2. [040816 - P2] New Register Definition
//
// 3. [040816 - P3] Change DMA ring size and buffer ==> Dma.cpp
//	TxRing, AtimRing, PrioRing change to Ac0RingMain, Ac1RingMain, Ac2RingMain, Ac3RingMain, MgmtRingMain, HccaRingMain.
//	
// 4. [040816 - P4] Change Tx/Rx Descriptor ==>  Dma.h
//
// 5. [040819 - P1] Instruction of enabling TX
//
// 6. [040819 - P2] Instruction of enabling RX

#ifndef	__RT2860_H__
#define	__RT2860_H__


#define IS_RT8592(_MACVersion)				((_MACVersion & 0xFFFF0000) == 0x85920000)
#define IS_MT7650(_MACVersion)				((_MACVersion & 0xFFFF0000) == 0x76500000)
#define IS_MT7630(_MACVersion)				((_MACVersion & 0xFFFF0000) == 0x76300000)
#define IS_MT7610(_MACVersion)				((_MACVersion & 0xFFFF0000) == 0x76100000)
#define IS_MT7662(_MACVersion)				((_MACVersion & 0xFFFF0000) == 0x76620000)

#define	MAX_RX_PROCESS			7
#define	MAX_TX_PROCESS			7

#define MAC_ADDR_LEN			6


//Delay
//#define   DELAY_VALUE                   0x85328532              
//#define IRQ_MSK_SPECIAL_BITS	 0x3
//#define IRQ_MASK				 0x3
//normal
#define   DELAY_VALUE				0x00000000
#define IRQ_MSK_SPECIAL_BITS	0x3fff3 //0x83ff3
#define IRQ_MASK					0x3fff3 //0x83ff3


#define INC_COUNTER(Val)		\
{								\
	if ((++Val.LowPart) == 0)	\
	{							\
		Val.HighPart++;			\
	}							\
}

#define	RATE_1					0
#define	RATE_2					1
#define	RATE_5_5				2
#define	RATE_11					3


// value domain of MacHdr.tyte, which is b3..b2 of the 1st-byte of MAC header
#define BTYPE_MGMT              0   // 00
#define BTYPE_CNTL              1   // 01
#define BTYPE_DATA              2   // 10

// value domain of MacHdr.subtype, which is b7..4 of the 1st-byte of MAC header
// Management frame
#define SUBTYPE_ASSOC_REQ       0
#define SUBTYPE_ASSOC_RSP       1
#define SUBTYPE_REASSOC_REQ     2
#define SUBTYPE_REASSOC_RSP     3
#define SUBTYPE_PROBE_REQ       4
#define SUBTYPE_PROBE_RSP       5
#define SUBTYPE_BEACON          8
#define SUBTYPE_ATIM            9
#define SUBTYPE_DISASSOC        10
#define SUBTYPE_AUTH            11
#define SUBTYPE_DEAUTH          12

// Control Frame
#define SUBTYPE_PS_POLL         10
#define SUBTYPE_RTS             11  // 1011
#define SUBTYPE_CTS             12  // 1100
#define SUBTYPE_ACK             13  // 1101
#define SUBTYPE_CFEND           14
#define SUBTYPE_CFEND_CFACK     15

// Data Frame
#define SUBTYPE_DATA                0
#define SUBTYPE_DATA_CFACK          1
#define SUBTYPE_DATA_CFPOLL         2
#define SUBTYPE_DATA_CFACK_CFPOLL   3
#define SUBTYPE_NULL_FUNC           4
#define SUBTYPE_CFACK               5  // 0101
#define SUBTYPE_CFPOLL              6
#define SUBTYPE_CFACK_CFPOLL        7

//
// Tx /	Rx / Prio /	Atim ring descriptor definition
//
#define	DESC_OWN_HOST		0
#define	DESC_OWN_NIC		1
#define	DESC_DMA_DONE		1


#define	PCI_CFG_ADDR_PORT		0xcf8
#define	PCI_CFG_DATA_PORT		0xcfc

#define	RICOH					0x1180
#define	O2MICRO					0x1217
#define	TI						0x104c
#define	RALINK					0x1814
#define	TOSHIBA					0x1179
#define	ENE						0x1524
#define	UNKNOWN					0xffff

#define PCIBUS_INTEL_VENDOR           0x8086          // CardBus bridge class & subclass
#define PCIBUS_AMD_VENDOR1           0x1022          // One of AMD's Vendor ID
#define PCIBUS_ATI_VENDOR1           0x1002          // One of ATI's Vendor ID
#define PCIBUS_NVIDIA_VENDOR1           0x10DE          // One of NVidia's Vendor ID
#define CARD_PCIBRIDGEPCI_CLASS           0x0604          // CardBus bridge class & subclass
#define CARD_BRIDGE_CLASS           0x0607          // CardBus bridge class & subclass

#define MAX_PCI_DEVICE              32      // support up to 32 devices per bus
#define MAX_PCI_BUS                 32      // support 10 buses
#define MAX_FUNC_NUM                8


// Mac Address data structure
typedef struct _MACADDR {
    UCHAR     Octet[MAC_ADDR_LEN];
} MACADDR, *PMACADDR;

typedef	struct	_FRAME_CONTROL	{
	USHORT		Ver:2;				// Protocol version
	USHORT		Type:2;				// MSDU type
	USHORT		Subtype:4;			// MSDU subtype
	USHORT		ToDs:1;				// To DS indication
	USHORT		FrDs:1;				// From DS indication
	USHORT		MoreFrag:1;			// More fragment bit
	USHORT		Retry:1;			// Retry status bit
	USHORT		PwrMgt:1;			// Power management bit
	USHORT		MoreData:1;			// More data bit
	USHORT		Wep:1;				// Wep data
	USHORT		Order:1;			// Strict order expected
}	FRAME_CONTROL, *PFRAME_CONTROL;

//
// P802.11 intermediate header format
//
typedef	struct	_CONTROL_HEADER	{
	FRAME_CONTROL	Frame;				// Frame control structure
	USHORT			Duration;			// Duration value
	MACADDR			Addr1;				// Address 1 field
	MACADDR			Addr2;				// Address 2 field
}	CONTROL_HEADER, *PCONTROL_HEADER;

//
// P802.11 header format
//
typedef	struct	_HEADER_802_11	{
	CONTROL_HEADER	Controlhead;
	MACADDR			Addr3;				// Address 3 field
	USHORT			Frag:4;				// Fragment number
	USHORT			Sequence:12;		// Sequence number
}	HEADER_802_11, *PHEADER_802_11;


#endif __RT2860_H__
