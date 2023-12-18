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
 	Driver.h
 
	Abstract:
    Miniport generic portion header file

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   08-21-2002    created

*/

#ifndef	__DRIVER_H__
#define	__DRIVER_H__

#include "config.h"


#define BIT0		(1 << 0)
#define BIT1		(1 << 1)
#define BIT2		(1 << 2)
#define BIT3		(1 << 3)
#define BIT4		(1 << 4)
#define BIT5		(1 << 5)
#define BIT6		(1 << 6)
#define BIT7		(1 << 7)
#define BIT8		(1 << 8)
#define BIT9		(1 << 9)
#define BIT10	(1 << 10)
#define BIT11	(1 << 11)
#define BIT12	(1 << 12)
#define BIT13	(1 << 13)
#define BIT14	(1 << 14)
#define BIT15	(1 << 15)
#define BIT16	(1 << 16)
#define BIT17	(1 << 17)
#define BIT18	(1 << 18)
#define BIT19	(1 << 19)
#define BIT20	(1 << 20)
#define BIT21	(1 << 21)
#define BIT22	(1 << 22)
#define BIT23	(1 << 23)
#define BIT24	(1 << 24)
#define BIT25	(1 << 25)
#define BIT26	(1 << 26)
#define BIT27	(1 << 27)
#define BIT28	(1 << 28)
#define BIT29	(1 << 29)
#define BIT30	(1 << 30)
#define BIT31	(1 << 31)

#define RT_DEBUG_OFF          0
#define RT_DEBUG_ERROR      1
#define RT_DEBUG_WARN       2
#define RT_DEBUG_TRACE      3
#define RT_DEBUG_INFO        4
#define RT_DEBUG_LOUD        5


#if DBG
extern ULONG    RTDebugLevel;

#define DBGPRINT_RAW(Level, Fmt)    \
{                                   \
	if (Level <= RTDebugLevel)      \
	{                               \
		DbgPrint  Fmt;               \
	}                               \
}

//#define DBGPRINT(Level, Fmt)    DBGPRINT_RAW(Level, Fmt)
#ifdef _USB
#define DBGPRINT(Level, Fmt)          \
{                                   \
	DbgPrint("USB:");          \
	DbgPrint Fmt;                  \
}
#else
#define DBGPRINT(Level, Fmt)          \
{                                   \
	DbgPrint("PCIe:");          \
	DbgPrint Fmt;                  \
}
#endif

#define DBGPRINT_ERR(Fmt)           \
{                                   \
	DbgPrint("ERROR!!! ");          \
	DbgPrint Fmt;                  \
}

#else
#define DBGPRINT(Level, Fmt)
#define DBGPRINT_RAW(Level, Fmt)
#define DBGPRINT_ERR(Fmt)
#endif



#define PRINT_MESSAGE TRUE
#if PRINT_MESSAGE
//#define InitDebugPrint DbgPrint
//#define LoopBackDbg DbgPrint
//#define AsicDebugPrint DbgPrint
#define DebugPrint DbgPrint
//#define SlaveDebugPrint DbgPrint
#define SEQdbg DbgPrint
#define Testdbg DbgPrint
#define DebugPrintWhereAmI() \
{	\
	DbgPrint("WhereAmI? %s, LINE%d\n",__FUNCTION__,__LINE__);	\
}

#else
//#define InitDebugPrint 
//#define AsicDebugPrint
//#define DebugPrint 
#define SEQdbg
#define Testdbg
#define DebugPrintWhereAmI
#endif

#include "HwHalDefine.h"


typedef enum{
	Stopped,
	Started,
	Removed
}DRIVER_STATE ;


#define CM_RESOURCE_BUF_SIZE   (sizeof(CM_RESOURCE_LIST) + (sizeof(CM_FULL_RESOURCE_DESCRIPTOR)) +(sizeof(CM_PARTIAL_RESOURCE_LIST))+(10*sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)))

// Statistic Counter
/*typedef struct _COUNTER_802_11 {
	LARGE_INTEGER   CRCErrorCount;
	LARGE_INTEGER   RXOverFlowCount;
	LARGE_INTEGER   PHYErrorCount;
	LARGE_INTEGER   FalseCCACount;
	LARGE_INTEGER   U2MDataCount;
	LARGE_INTEGER   OtherDataCount;
	LARGE_INTEGER   BeaconCount;
	LARGE_INTEGER   othersCount;	
	LARGE_INTEGER   ErrorIdx;
} COUNTER_802_11, *PCOUNTER_802_11;

/*typedef struct _OTHER_STATISTICS {
	ULONG					TxRepeatCount;
	ULONG					TransmittedCount;
	ULONG					Rx0ReceivedCount;
	ULONG					ReceivedCount;//++werner //loopback receive count
	ULONG					Ac0TxedCount;
	ULONG					Ac1TxedCount;
	ULONG					Ac2TxedCount;
	ULONG					Ac3TxedCount;
	ULONG					Ac4TxedCount;
	ULONG					Ac5TxedCount;
	ULONG					Ac6TxedCount;
	ULONG					Ac7TxedCount;
	ULONG					Ac8TxedCount;
	ULONG					Ac9TxedCount;
	ULONG					Ac10TxedCount;
	ULONG					Ac11TxedCount;	
	ULONG					MgmtTxedCount;
	ULONG					HccaTxedCount;
	LONG					RSSI0;
	LONG					RSSI1;
	LONG					RSSI2;
	LONG					SNR0;
	LONG					SNR1;
	LONG					SNR2;
	ULONG					UdpErr;
	ULONG					TcpErr;
	ULONG					IpErr;
	LONG					RCPI0;
	LONG					RCPI1;
	LONG					FreqOffstFromRX;
} OTHER_STATISTICS, *POTHER_STATISTICS;*/

typedef struct _PCI_ADDRESS_DATA 
{
	ULONG	TranslateAddress;
	ULONG	MapIoAddress;
	ULONG	ShareMemorySize;
} PCI_ADDRESS_DATA, *PPCI_ADDRESS_DATA;


typedef struct _EXTENDED_PCI_ADDRESS
{
	PCI_ADDRESS_DATA PciAddress[6];
	int		Count;
} EXTENDED_PCI_ADDRESS, *PEXTENDED_PCI_ADDRESS;

#define	LFSR_MASK	0x80000057



//
//	Data buffer for DMA operation, the buffer must be contiguous physical memory
//  Both DMA to / from CPU use the same structure.
//
typedef	struct	_RTMP_DMABUF
{
	ULONG					AllocSize;
	PVOID					AllocVa;			// TxBuf virtual address
	PHYSICAL_ADDRESS		AllocPa; 			// TxBuf physical address
}	RTMP_DMABUF, *PRTMP_DMABUF;

//
// Control block (Descriptor) for DMA operation, buffer must be contiguous physical memory
// TxRing, AtimRing, PriorityRing. RxRingMain all use the same descriptor except associated
// buffer size differs.
//
typedef	struct _RTMP_DMACB
{
	ULONG					AllocSize;			// Control block size
	PVOID					AllocVa;			// Control block virtual address
	PHYSICAL_ADDRESS		AllocPa;			// Control block physical address
	RTMP_DMABUF				DmaBuf;				// Associated DMA buffer structure 
}	RTMP_DMACB, *PRTMP_DMACB;

#define	SNIFFER_COUNT					5000
#define	RX_RING_PACKET_BUFFER			1000




#define	MAX_COM_BUFFER_SIZE				512

typedef struct _COM_TX_BUFFER {
	ULONG	TxInsertPointer;
	ULONG	TxRemovePointer;
	ULONG	TxFreeCount;
	PUCHAR	pComTxBuf;
} COM_TX_BUFFER, *PCOM_TX_BUFFER;

typedef struct _COM_RX_BUFFER {
	ULONG	RxInsertPointer;
	ULONG	RxRemovePointer;
	ULONG	RxFreeCount;
	PUCHAR	pComRxBuf;
} COM_RX_BUFFER, *PCOM_RX_BUFFER;

typedef struct _SCATTER_CONTROL {
	ULONG					TotLength;
	ULONG					ScatterNum;
	ULONG					ScatterSht[5];
	ULONG					ScatterLen[5];	
}SCATTER_CONTROL, *PSCATTER_CONTROL;

//------------------------------------------------------



// structure to tune BBP R17 "RX AGC VGC init"
typedef struct _RT_802_11_RX_AGC_VGC_TUNING {
    UCHAR   FalseCcaLowerThreshold;  // 0-255, def 10
    UCHAR   FalseCcaUpperThreshold;  // 0-255, def 100
    UCHAR   VgcDelta;                // R17 +-= VgcDelta whenever flase CCA over UpprThreshold
                                     // or lower than LowerThresholdupper threshold
    UCHAR   VgcUpperBound;           // max value of R17
} RT_802_11_RX_AGC_VGC_TUNING, *PRT_802_11_RX_AGC_VGC_TUNING;

// PortConfig
typedef struct _PORT_CONFIG {
	UCHAR		ChannelTxPower[MAX_LEN_OF_CHANNELS];		// Store Tx power value for all channels.
	UCHAR		ChannelTssiRef[MAX_LEN_OF_CHANNELS];		// Store Tssi Reference value for all channels.
	UCHAR		ChannelTssiDelta;							// Store Tx TSSI delta increment / decrement value
	BOOLEAN		bAutoTxAgc;								// Enable driver auto Tx Agc control
	UCHAR		ChannelList[MAX_LEN_OF_CHANNELS];         // list all supported channels for site survey
	UCHAR		ChannelListNum;                           // number of channel in ChannelList[]
	//BOOLEAN		bShowHiddenSSID;							// Show all known SSID in SSID list get operation

	//----------------------------------
	UCHAR		NumberOfAntenna;
	UCHAR		CurrentTxAntenna;
	UCHAR		CurrentRxAntenna;
	UCHAR		RfType;
	//---------
	UCHAR		TxRxFixed;
	//----------------------------------

	UCHAR		CountryRegion;	// Enum of country region, 0:FCC, 1:IC, 2:ETSI, 3:SPAIN, 4:France, 5:MKK, 6:MKK1, 7:Israel
	UCHAR		PhyMode;    // PHY_11A, PHY_11B, PHY_11BG_MIXED, PHY_ABG_MIXED

	RT_802_11_RX_AGC_VGC_TUNING			BbpTuning;

}PORT_CONFIG, *PPORT_CONFIG;


#ifdef RTMP_PCI_SUPPORT
typedef struct _RTMP_ADAPTER {
	PDRIVER_OBJECT			pDriverObject;
	PUNICODE_STRING			RegistryPath;
	PDEVICE_OBJECT			pDeviceObject;
	PDEVICE_OBJECT			pPhyDeviceObj;

	PULONG					virtualaddr;
	ULONG					MemorySize;
	PKINTERRUPT				pIntObj;	// the interrupt object
	KIRQL					IRQL;
	ULONG					InterruptLevel;
	ULONG					Vector;
	KAFFINITY					Affinity;
	ULONG					Irq;
	ULONG					IntSource;
	ULONG					IntrMask;
	ULONG					MACVersion;
#ifdef MSI
	//MSI used
	PVOID					pConnectionContext;
	IO_CONNECT_INTERRUPT_PARAMETERS params;
	ULONG					TypeOfInterruptVersion;
	BOOLEAN					bConnectedInterrupt;
#endif	
	PHYSICAL_ADDRESS			ConfigurationSpacePhyAddr;
	ULONG					OSMajorVersion;

	PDEVICE_OBJECT			pLowerDevice;
	ULONG					DeviceNumber;
	UNICODE_STRING			ustrDeviceName;	// internal name
	UNICODE_STRING			ustrSymLinkName;	// external name
	PUCHAR					deviceBuffer;		// temporary pool buffer
	UINT32					deviceBufferSize;
	DRIVER_STATE			state;		// current state of driver
	EXTENDED_PCI_ADDRESS	ExtendPciAddress;

	//DMA ADAPTER
	BOOLEAN					DmaStatus;
	PDMA_ADAPTER			pDmaAdapter;
	ULONG					CurRx0Index;
	ULONG					CurRx1Index;	
	//ULONG					CurTxIndex;
	//ULONG					NextTxDoneIndex;

	ULONG					CurAc0TxIndex;
	ULONG					NextAc0TxDoneIndex;
	ULONG					CurAc1TxIndex;
	ULONG					NextAc1TxDoneIndex;
	ULONG					CurAc2TxIndex;
	ULONG					NextAc2TxDoneIndex;
	ULONG					CurAc3TxIndex;
	ULONG					NextAc3TxDoneIndex;
	ULONG					CurAc4TxIndex;
	ULONG					NextAc4TxDoneIndex;
	ULONG					CurAc5TxIndex;
	ULONG					NextAc5TxDoneIndex;
	ULONG					CurAc6TxIndex;
	ULONG					NextAc6TxDoneIndex;
	ULONG					CurAc7TxIndex;
	ULONG					NextAc7TxDoneIndex;

	ULONG					CurAc8TxIndex;
	ULONG					NextAc8TxDoneIndex;
	ULONG					CurAc9TxIndex;
	ULONG					NextAc9TxDoneIndex;
	ULONG					CurAc10TxIndex;
	ULONG					NextAc10TxDoneIndex;
	ULONG					CurAc11TxIndex;
	ULONG					NextAc11TxDoneIndex;
	
	ULONG					CurMgmtTxIndex;
	ULONG					NextMgmtTxDoneIndex;
	ULONG					CurHccaTxIndex;
	ULONG					NextHccaTxDoneIndex;

	ULONG					Ac0_MAX_TX_PROCESS;
	ULONG					Ac1_MAX_TX_PROCESS;
	ULONG					Ac2_MAX_TX_PROCESS;
	ULONG					Ac3_MAX_TX_PROCESS;
	ULONG					Ac4_MAX_TX_PROCESS;
	ULONG					Ac5_MAX_TX_PROCESS;
	ULONG					Ac6_MAX_TX_PROCESS;
	ULONG					Ac7_MAX_TX_PROCESS;
	ULONG					Ac8_MAX_TX_PROCESS;
	ULONG					Ac9_MAX_TX_PROCESS;
	ULONG					Ac10_MAX_TX_PROCESS;
	ULONG					Ac11_MAX_TX_PROCESS;	
	ULONG					Mgmt_MAX_TX_PROCESS;
	ULONG					Hcca_MAX_TX_PROCESS;	

	int 						bNeedPayloadLenRandom;
	int 						bNeedPayloadDataRandom;
	
	ULONG					FrameType;
	ULONG					CurLength;
	ULONG					MaxLength;
	ULONG					MinLength;
	ULONG					RingType;

	USHORT					SetTXWI_NSEQ;
				
	ULONG					ShiftReg;

	// resource for DMA operation

#if 0	
	RTMP_DMACB				TxRing[TX_RING_SIZE];		// Tx Ring
	RTMP_DMACB				AtimRing[ATIM_RING_SIZE];	// Atim Ring
	RTMP_DMACB				PrioRing[PRIO_RING_SIZE];	// Priority Ring
#endif

	RTMP_DMACB				Ac0RingMain[AC0_RING_SIZE+AC0_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac1RingMain[AC1_RING_SIZE+AC1_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac2RingMain[AC2_RING_SIZE+AC2_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac3RingMain[AC3_RING_SIZE+AC3_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac4RingMain[AC4_RING_SIZE+AC4_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac5RingMain[AC5_RING_SIZE+AC5_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac6RingMain[AC6_RING_SIZE+AC6_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac7RingMain[AC7_RING_SIZE+AC7_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac8RingMain[AC7_RING_SIZE+AC8_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac9RingMain[AC8_RING_SIZE+AC9_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac10RingMain[AC9_RING_SIZE+AC10_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				Ac11RingMain[AC10_RING_SIZE+AC11_TEMP_SIZE];		// Tx Ring	
	RTMP_DMACB				MgmtRingMain[MGMT_RING_SIZE+MGMT_TEMP_SIZE];		// Tx Ring
	RTMP_DMACB				HccaRingMain[HCCA_RING_SIZE+HCCA_TEMP_SIZE];		// Tx Ring	
	RTMP_DMACB				Rx0RingMain[RX0_RING_SIZE];		// Rx Ring
	RTMP_DMACB				Rx1RingMain[RX1_RING_SIZE];		// Rx Ring
		
	RTMP_DMABUF				RingMain;						// Shared memory for all descriptors
	//--------------------------------------------------------------------------

	// Fw Req & Rsp
	BOOLEAN				IsFwRsp;
	USHORT				FwReqSequence;
	UCHAR				FwReceiveBuffer[1024];
	
	//--------------------------------------------------------------------------

	ULONG					FirmwareVersion;        // byte 0: Minor version, byte 1: Major version, otherwise unused.
	PUCHAR					pFirmwareBuffer;

	//--------------------------------------------------------------------------

	PORT_CONFIG				PortCfg;
	
	UCHAR					ucFrameEndType;

	//-----------------------------------------------------

	// File Operation
	PHANDLE					FileHandle;
	POBJECT_ATTRIBUTES		ObjectAttributes;

	COUNTER_802_11			WlanCounters;
	OTHER_STATISTICS		OtherCounters;
	ULONG					SecurityFail;
	ULONG					FirmwarePktCount;
	
	
	LONG					BF_SNR0;
	LONG					BF_SNR1;
	LONG					BF_SNR2;
	
	BOOLEAN					bStartTx;
	BOOLEAN					bStartRx;

	USHORT					ADDRESSSIZE;
	

	DEVICE_POWER_STATE		PowerState;	// Our device power state
	BOOLEAN					bTxAck;
	ULONG					ulCipher;
	KSPIN_LOCK				RxRingMainLock;

	// Sniffer Function
#ifdef	SNIFF
	BOOLEAN					bStartSniff;
	USHORT					PacketBufferDriverIdx;				// point to current driver buffer idx
	USHORT					PacketBufferSnifferIdx;				// point to current Sniffer Buffer idx
	BOOLEAN					PacketBufferWrapAround;				// 
	PVOID					pPacketBuf;
	ULONG					ulTimestamp;
#endif

	//Com Port function
	COM_TX_BUFFER			comTxBuffer;
	COM_RX_BUFFER			comRxBuffer;

	ULONG					AntennaDiversity;
	ULONG					SelAnt;

	ULONG					SetTcpIpPayload;
	int 						IsSaveRXVLog;
	HANDLE   				RXVFileHandle;
	//int 						RX0DataCount;

	//for Efuse using
	UCHAR	EfuseMode;// Buffer:0, Efuse:1, EEPROM:2
	UCHAR	EfuseContent[EFUSESIZE];

	//TxSetFrequencyOffset
	BOOLEAN					IsTxSetFrequency;// check if is if first time to run TxSetFrequencyOffset
} RTMP_ADAPTER, *PRTMP_ADAPTER;
#endif


typedef	union	_HOST_CMD_CSR_STRUC	{
	struct	{
	    ULONG   HostCommand:8;
	    ULONG   Rsv:24;
	}	field;
	ULONG			word;
}	HOST_CMD_CSR_STRUC, *PHOST_CMD_CSR_STRUC;

typedef union  _H2M_MAILBOX_STRUC {
	struct {
		ULONG       LowByte:8;
		ULONG       HighByte:8;
		ULONG       CmdToken:8;
		ULONG       Owner:8;
	}   field;
	ULONG           word;
} H2M_MAILBOX_STRUC, *PH2M_MAILBOX_STRUC;

typedef	union	_WPDMA_GLO_CFG_STRUC	{
	struct	{
		ULONG		EnableTxDMA:1;
		ULONG		TxDMABusy:1;
		ULONG		EnableRxDMA:1;
		ULONG		RxDMABusy:1;
		ULONG       	WPDMABurstSIZE:2;
		ULONG       	EnTXWriteBackDDONE:1;
		ULONG       	BigEndian:1;
		ULONG       	RXHdrScater:8;
		ULONG       	:16;
	}	field;
	ULONG			word;
} WPDMA_GLO_CFG_STRUC, *PWPDMA_GLO_CFG_STRUC;

typedef	union	_US_CYC_CNT_STRUC	{
	struct	{
	    ULONG  UsCycCnt:8;
		ULONG  BtModeEn:1;
		ULONG  rsv1:7;
		ULONG  TestSel:8;
		ULONG  TestEn:1;
		ULONG  rsv2:7;
	}	field;
	ULONG			word;
}	US_CYC_CNT_STRUC, *PUS_CYC_CNT_STRUC;



//
//For RT3290
//
#define CMB_CTRL			0x20
#ifdef BIG_ENDIAN
typedef	union	_CMB_CTRL_STRUC	{
	struct	{
		ULONG       	LDO0_EN:1;
		ULONG       	LDO3_EN:1;
		ULONG       	LDO_BGSEL:2;		//LDO band-gap selection
		ULONG       	LDO_CORE_LEVEL:4;	//LDO0/3 volrage level selection
		ULONG       	PLL_LD:1;			//BBP PLL lock done
		ULONG       	XTAL_RDY:1;		
		ULONG       	Rsv:2;
		ULONG		LDO25_FRC_ON:1;//4      		
		ULONG		LDO25_LARGEA:1;		
		ULONG		LDO25_LEVEL:2;		//LDO25 voltage level selection
		ULONG		AUX_OPT_Bit15_Two_AntennaMode:1;			// or one antenna mode			
		ULONG		AUX_OPT_Bit14_TRSW1_as_GPIO:1;			// or as BT TRSW		
		ULONG		AUX_OPT_Bit13_GPIO7_as_GPIO:1;			// or as BT_PA_EN		
		ULONG		AUX_OPT_Bit12_TRSW0_as_WLAN_ANT_SEL:1;	// or WLAN TRSW		
		ULONG		AUX_OPT_Bit11_Rsv:1;		
		ULONG		AUX_OPT_Bit10_NotSwap_WL_LED_ACT_RDY:1;			
		ULONG		AUX_OPT_Bit9_GPIO3_as_GPIO:1;				// or PCIe Wake		
		ULONG		AUX_OPT_Bit8_AuxPower_Exists:1;		
		ULONG		AUX_OPT_Bit7_KeepInterfaceClk:1;			// or slow down interface clock when only BT is enabled		
		ULONG		AUX_OPT_Bit6_KeepXtal_On:1;				// or power down Xtal when all functions are off		
		ULONG		AUX_OPT_Bit5_RemovePCIePhyClk_BTOff:1;	// or despite BT on or off		
		ULONG		AUX_OPT_Bit4_RemovePCIePhyClk_WLANOff:1;	// or despite WLAN on or off
		ULONG		AUX_OPT_Bit3_PLLOn_L1:1;
		ULONG		AUX_OPT_Bit2_PCIeCoreClkOn_L1:1;
		ULONG		AUX_OPT_Bit1_PCIePhyClkOn_L1:1;		
		ULONG		AUX_OPT_Bit0_InterfaceClk_40Mhz:1;			// or enable interface clock power saving	
		//ULONG		AUX_OPT:15;		
	}	;
	ULONG			word;
}	CMB_CTRL_STRUC, *PCMB_CTRL_STRUC;
#else
typedef	union	_CMB_CTRL_STRUC	{
	struct	{
		//ULONG		AUX_OPT:15;
		ULONG		AUX_OPT_Bit0_InterfaceClk_40Mhz:1;			// or enable interface clock power saving
		ULONG		AUX_OPT_Bit1_PCIePhyClkOn_L1:1;		
		ULONG		AUX_OPT_Bit2_PCIeCoreClkOn_L1:1;
		ULONG		AUX_OPT_Bit3_PLLOn_L1:1;
		ULONG		AUX_OPT_Bit4_RemovePCIePhyClk_WLANOff:1;	// or despite WLAN on or off
		ULONG		AUX_OPT_Bit5_RemovePCIePhyClk_BTOff:1;	// or despite BT on or off
		ULONG		AUX_OPT_Bit6_KeepXtal_On:1;				// or power down Xtal when all functions are off
		ULONG		AUX_OPT_Bit7_KeepInterfaceClk:1;			// or slow down interface clock when only BT is enabled
		ULONG		AUX_OPT_Bit8_AuxPower_Exists:1;
		ULONG		AUX_OPT_Bit9_GPIO3_as_GPIO:1;				// or PCIe Wake
		ULONG		AUX_OPT_Bit10_NotSwap_WL_LED_ACT_RDY:1;	
		ULONG		AUX_OPT_Bit11_Rsv:1;
		ULONG		AUX_OPT_Bit12_TRSW0_as_WLAN_ANT_SEL:1;	// or WLAN TRSW
		ULONG		AUX_OPT_Bit13_GPIO7_as_GPIO:1;			// or as BT_PA_EN
		ULONG		AUX_OPT_Bit14_TRSW1_as_GPIO:1;			// or as BT TRSW
		ULONG		AUX_OPT_Bit15_Two_AntennaMode:1;			// or one antenna mode		
		ULONG		LDO25_LEVEL:2;		//LDO25 voltage level selection
		ULONG		LDO25_LARGEA:1;
		ULONG		LDO25_FRC_ON:1;//4      
		ULONG       	Rsv:2;
		ULONG       	XTAL_RDY:1;
		ULONG       	PLL_LD:1;			//BBP PLL lock done
		ULONG       	LDO_CORE_LEVEL:4;	//LDO0/3 volrage level selection
		ULONG       	LDO_BGSEL:2;		//LDO band-gap selection
		ULONG       	LDO3_EN:1;
		ULONG       	LDO0_EN:1;
	};
	ULONG			word;
} CMB_CTRL_STRUC, *PCMB_CTRL_STRUC;
#endif


#define WLAN_FUN_CTRL		0x80
#ifdef BIG_ENDIAN
typedef	union	_WLAN_FUN_CTRL_STRUC	{
	struct	{
		ULONG		GPIO0_OUT_OE_N:8;		//GPIO0 output enable
		ULONG		GPIO0_OUT:8;			//GPIO0 output data
		ULONG		GPIO0_IN:8;				//GPIO0 input data
		ULONG		WLAN_ACC_BT:1;			//WLAN driver access BT enable
		ULONG		INV_TR_SW0:1;			//invert TR_SW0
		ULONG		Rsv2:1;
		ULONG		PCIE_APP0_CLK_REQ:1;	//PCIE function0(WLAN) clock request
		ULONG		WLAN_RESET:1;			//Reset WLAN
		ULONG		Rsv1:1;
		ULONG		WLAN_CLK_EN:1;			//WLAN clock enable
		ULONG		WLAN_EN:1;				//WLAN function enable
	};
	ULONG			word;
}	WLAN_FUN_CTRL_STRUC, *PWLAN_FUN_CTRL_STRUC;
#else
typedef	union	_WLAN_FUN_CTRL_STRUC	{
	struct	{
		ULONG		WLAN_EN:1;				//WLAN function enable
		ULONG		WLAN_CLK_EN:1;			//WLAN clock enable
		ULONG		Rsv1:1;
		ULONG		WLAN_RESET:1;			//Reset WLAN
		ULONG		PCIE_APP0_CLK_REQ:1;	//PCIE function0(WLAN) clock request
		ULONG		Rsv2:1;
		ULONG		INV_TR_SW0:1;			//invert TR_SW0
		ULONG		WLAN_ACC_BT:1;			//WLAN driver access BT enable
		ULONG		GPIO0_IN:8;				//GPIO0 input data
		ULONG		GPIO0_OUT:8;			//GPIO0 output data		
		ULONG		GPIO0_OUT_OE_N:8;		//GPIO0 output enable
	};
	ULONG			word;
} WLAN_FUN_CTRL_STRUC, *PWLAN_FUN_CTRL_STRUC;
#endif




// Forward declarations

NTSTATUS AddDevice (	
						IN PDRIVER_OBJECT pDriverObject,
						IN PDEVICE_OBJECT pdo );

NTSTATUS DispPnp(	
					IN PDEVICE_OBJECT pDO,
					IN PIRP pIrp );

NTSTATUS Wdm2Power(	IN PDEVICE_OBJECT fdo,
					IN PIRP Irp);

NTSTATUS DispPower(	
					IN PDEVICE_OBJECT pDO,
					IN PIRP pIrp );

NTSTATUS PassDownPnP(	
						IN PDEVICE_OBJECT pDO,
						IN PIRP pIrp );

VOID StartIo(
				IN PDEVICE_OBJECT pDO,
				IN PIRP pIrp );

NTSTATUS HandleStartDevice(	
							IN PDEVICE_OBJECT pDO,
							IN PIRP pIrp );

NTSTATUS HandleStopDevice(	
							IN PDEVICE_OBJECT pDO,
							IN PIRP pIrp );

NTSTATUS HandleRemoveDevice(	
								IN PDEVICE_OBJECT pDO,
								IN PIRP pIrp );

static VOID DriverUnload (	
							IN PDRIVER_OBJECT	pDriverObject );

static NTSTATUS DispatchCreate (	
									IN PDEVICE_OBJECT	pDevObj,
									IN PIRP				pIrp );

static NTSTATUS DispatchClose (	
								IN PDEVICE_OBJECT	pDevObj,
								IN PIRP				pIrp );

NTSTATUS DispatchDeviceControl (	
									IN PDEVICE_OBJECT	pDevObj,
									IN PIRP				pIrp );

NTSTATUS
ReadWriteConfigSpace(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG          ReadOrWrite, // 0 for read 1 for write
    IN PVOID          Buffer,
    IN ULONG          Offset,
    IN ULONG          Length
    );

NTSTATUS
ReadMemory(
	IN PDEVICE_OBJECT DeviceObject,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    );

NTSTATUS
WriteMemory(
	IN PDEVICE_OBJECT DeviceObject,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    );

NTSTATUS
BufferReadMemory(
	IN PDEVICE_OBJECT DeviceObject,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    );

NTSTATUS
BufferWriteMemory(
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    );

BOOLEAN 
Isr (	
	IN PKINTERRUPT pIntObj,
	IN PVOID pServiceContext 
	);

VOID 
RTPCIDpcForIsr(	
	IN PKDPC pDpc,
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp,
	IN PVOID pContext 
	);

NTSTATUS
DriverInit(
    IN	PDEVICE_OBJECT	pDO
    );

VOID
RTPCIHandleRx0DoneInterrupt(
	IN	PVOID			pContext 
	);

VOID
RTPCIHandleRx1DoneInterrupt(
	IN	PVOID			pContext 
	);

VOID
RTMPHandleAc0TxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTMPHandleAc1TxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTMPHandleAc2TxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTMPHandleAc3TxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTMPHandleAc4TxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTMPHandleAc5TxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTMPHandleAc6TxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTMPHandleAc7TxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTMPHandleMgmtTxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTMPHandleHccaTxRingTxDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTPCIHandleDecryptionDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTPCIHandleEncryptionDoneInterrupt(
	IN PVOID pContext 
	);

#ifdef _USB

#else
// PCIe
VOID 
LfsrInit(
    IN PDEVICE_OBJECT	pDevObj, 
    IN ULONG Seed
	);
#endif

UCHAR 
RandomByte(
    IN RTMP_ADAPTER *pDevExt
	);

USHORT 
RandomShort(
    IN RTMP_ADAPTER *pDevExt
	);
	
ULONG	RTMPReadCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset);

VOID	RTMPWriteCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset,
	IN	ULONG	Value);

VOID	RTMPPatchCardBus(
	IN	RTMP_ADAPTER *pAdapter);

VOID	RTMPPatchRalinkCardBus(
	IN	RTMP_ADAPTER *pAdapter,
	IN	ULONG			Bus);

NTSTATUS NICInit(
	IN	RTMP_ADAPTER *pAdapter);

NTSTATUS NICLoadRomPatch(
 	IN	RTMP_ADAPTER *pAdapter);

NTSTATUS NICLoadFirmware(
 	IN	RTMP_ADAPTER *pAdapter);

VOID RTMPEnableWlan(
	IN	RTMP_ADAPTER *pAdapter,
	IN 	BOOLEAN bOn);

ULONG GetTXWISize (
	IN	RTMP_ADAPTER *pAdapter);
	
ULONG GetRXWISize (
	IN	RTMP_ADAPTER *pAdapter);

BOOLEAN
AsicSetDevMac(
    IN	PDEVICE_OBJECT	pDO,
    IN PUCHAR addr);


VOID WriteRXVToFile(
    IN	HANDLE   	handle,
    IN	PCHAR buffer,
    IN	ULONG buffersize);

ULONG RTMPEqualMemory(
	IN  PVOID   pSrc1,
	IN  PVOID   pSrc2,
	IN  ULONG   Length);

VOID RTMPZeroMemory(
	IN  PVOID   pSrc,
	IN  ULONG   Length);

ULONG RTMPCompareMemory(
	IN  PVOID   pSrc1,
	IN  PVOID   pSrc2,
	IN  ULONG   Length);

VOID RTMPMoveMemory(
	OUT PVOID   pDest,
	IN  PVOID   pSrc,
	IN  ULONG   Length);

VOID HandleRXVector(
	IN PUCHAR   pData,
	IN	RTMP_ADAPTER *pAdapter
	);
VOID CheckSecurityResult(
	IN PULONG   pData,
	IN	RTMP_ADAPTER *pAdapter
	);

NTSTATUS FirmwareCommnadRestartDownloadFW(
	IN  RTMP_ADAPTER   *pAdapter
	);

NTSTATUS FirmwareCommnadStartToLoadFW(
	IN  RTMP_ADAPTER   *pAdapter,
	IN  ULONG           ImageLength,
	IN  ULONG		Address,
	IN  ULONG		DataMode
	);

NTSTATUS FirmwareScatters(
	IN  RTMP_ADAPTER   *pAdapter,
	IN  PUCHAR          pFirmwareImage,
	IN  ULONG           ImageLength
	);

NTSTATUS FirmwareCommnadStartToRunFW(
	IN  RTMP_ADAPTER   *pAdapter
	);

//
// firmware command start to load patch
//
NTSTATUS FirmwareCommnadStartToLoadROMPatch(
	IN	RTMP_ADAPTER	*pAdapter,
	IN	ULONG			Address,
	IN	ULONG			Length
	);
//
// firmware command patch finish
//
NTSTATUS FirmwareCommnadLoadROMPatchFinish(
	IN  RTMP_ADAPTER   *pAdapter
	);
//
// Rom patch Get Semaphore
//
NTSTATUS FirmwareCommnadGetPatchSemaphore(
	IN	RTMP_ADAPTER	*pAdapter
	);

//
// Load Andes ROM patch code
// MT7636 FPGA
//
NTSTATUS LoadROMPatchProcess(
	IN RTMP_ADAPTER *pAdapter,
	IN PUCHAR	pFileName,
	UINT32		FileLength
	);

NTSTATUS LoadFwImageProcess(
	IN RTMP_ADAPTER *pAdapter,
	IN PUCHAR	pFileName,
	IN UINT32	FileLength,
	IN UCHAR	Type
	);

NTSTATUS	ROMPatchPowerOnWiFiSys(
	IN	RTMP_ADAPTER	*pAdapter
	);

NTSTATUS	USBROMPatchCalChecksum(
	IN	RTMP_ADAPTER	*pAdapter,
	IN	PVOID			pData,
	IN	USHORT			Length
	);

NTSTATUS	USBROMPatchGetChecksum(
	IN	RTMP_ADAPTER	*pAdapter,
	IN	PVOID			pData,
	IN	USHORT			Length
	);
#ifdef _USB
//#define MAX_TX_BULK_PIPE_NUM 6
//#define MAX_RX_BULK_PIPE_NUM 2


//#define NOR_FLASH_SIZE_TYPE_8_BIT		0
//#define NOR_FLASH_SIZE_TYPE_8_16_BIT		1
/*typedef struct _UsbQa_DEVICE_EXTENSION
{
	PDEVICE_OBJECT	fdo;
	PDEVICE_OBJECT	pdo;
	PDEVICE_OBJECT	NextStackDevice;
	UNICODE_STRING	ifSymLinkName;

	bool GotResources;	// Not stopped
	bool Paused;		// Stop or remove pending
	bool IODisabled;	// Paused or stopped


	LONG OpenHandleCount;	// Count of open handles

	LONG UsageCount;		// Pending I/O Count
	bool Stopping;			// In process of stopping
	KEVENT StoppingEvent;	// Set when all pending I/O complete


	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	USBD_CONFIGURATION_HANDLE UsbConfigurationHandle;	// Selected Configuration handle

	PUSBD_PIPE_INFORMATION UsbIntrInPipeInfo;
	PUSBD_PIPE_INFORMATION UsbBulkInPipeInfo;
	PUSBD_PIPE_INFORMATION UsbBulkOutPipeInfo;
	
	USBD_PIPE_HANDLE UsbIntrInPipeHandle;	// Handle to input interrupt pipe
	USBD_PIPE_HANDLE UsbBulkInPipeHandle[MAX_RX_BULK_PIPE_NUM];	// Handle to input Bulk pipe
	USBD_PIPE_HANDLE UsbBulkOutPipeHandle[MAX_TX_BULK_PIPE_NUM];	// Handle to Output Bulk pipe
	
	USHORT                   UsbBulkOutMaxPacketSize;
	USHORT                   UsbBulkInMaxPacketSize;
	
	PURB				pRxUrb[MAX_RX_BULK_PIPE_NUM];
	PIRP					pRxIrp[MAX_RX_BULK_PIPE_NUM];
	PUCHAR				RxBuffer[MAX_RX_BULK_PIPE_NUM];
	USHORT				RxBufferLength[MAX_RX_BULK_PIPE_NUM];
	ULONG				ulBulkInRunning[MAX_RX_BULK_PIPE_NUM];

	PURB				pTxUrb[MAX_TX_BULK_PIPE_NUM];
	PIRP					pTxIrp[MAX_TX_BULK_PIPE_NUM];
	PUCHAR				TxBuffer[MAX_TX_BULK_PIPE_NUM];
	USHORT				TxBufferLength[MAX_TX_BULK_PIPE_NUM];
	UCHAR				TxBufferForIncLenTest[8192];//[USB_BULK_BUFFER_SIZE];
	BOOLEAN				bBulkOutRunning;
	BOOLEAN				bBulkOutRunning1;
	USHORT				bMaxLen;
	USHORT				bMinLen;
	USHORT				bCurrlength;

	KEVENT  				CancelRxIrpEvent[MAX_RX_BULK_PIPE_NUM];      // for cancelling bulk In IRPs.
	KEVENT  				CancelTxIrpEvent[MAX_TX_BULK_PIPE_NUM];   	// for cancelling bulk Out IRPs.

	// Fw Req & Rsp
	USHORT				FwReqSequence;
	UCHAR				FwReceiveBuffer[1024];
	
	ULONG				TransmittedCount;
	ULONG				TxBulkCount[MAX_TX_BULK_PIPE_NUM];
	LARGE_INTEGER		ReceivedCount[MAX_RX_BULK_PIPE_NUM];	
	COUNTER_802_11		WlanCounters;
	OTHER_STATISTICS	OtherCounters;
	LONG	RSSI0;
	LONG	RSSI1;
	LONG	RSSI2;
	LONG	SNR0;
	LONG	SNR1;
	LONG	SNR2;
	LONG	BF_SNR0;
	LONG	BF_SNR1;
	LONG	BF_SNR2;
	
	BOOLEAN				Seq_dbg;
	BOOLEAN				ContinBulkOut;
	ULONG				BulkOutRemained;
	ULONG				BulkOutWhichPipeNeedRun;
	ULONG				BulkOutTxType;

	KSPIN_LOCK 			BulkInIoCallDriverSpinLock[MAX_RX_BULK_PIPE_NUM];
	BOOLEAN				bBulkInIoCallDriverFlag[MAX_RX_BULK_PIPE_NUM];	
	BOOLEAN				ContinBulkIn[MAX_RX_BULK_PIPE_NUM];
	USHORT				BulkInRemained[MAX_RX_BULK_PIPE_NUM];

	KSPIN_LOCK 			RxSpinLock[MAX_RX_BULK_PIPE_NUM];		// Spin lock for sniffer momory move
	BOOLEAN				bRxEnable[MAX_RX_BULK_PIPE_NUM];
	BOOLEAN				RxAggregate;    //enable rx aggregate
	BOOLEAN				TxAggregate;    //enable Tx aggregate
	BOOLEAN				bNextVLD;;    
 	
	KSPIN_LOCK 			SnifferSpinLock;		// Spin lock for sniffer momory move
	PUCHAR				SnifferBuffer;
	BOOLEAN				bEnableSniffer;
	USHORT				SnifferDriverCP;
	USHORT				SnifferAppCP;
	ULONG				bDriverSupportSniffer;

	//RX MPDU
	PUCHAR				RXMPDUBuffer;
	ULONG				RXMPDULength;
	USHORT				MPDUDriverCP;
	USHORT				MPDUAppCP;
	KSPIN_LOCK 			MPDUSpinLock;		// Spin lock for MPDU momory move
	
	KSEMAPHORE			IoCtrlSemaphore;

	ULONG				MACVersion;
	ULONG				FMode;

	ULONG 					ulSetNorFlashSizeType;
	//--------------------------------------------------------------------------

	//-----------------------------------------------------
	ULONG bAutoTuningEnabled;

	BOOLEAN bExternalLnaEnabled;
	UCHAR	ucFrameEndType;

	BOOLEAN bRadarDetected;

	//--------------------------------------------------------------------------

	BOOLEAN bIsTxOfdm;

	//--------------------------------------------------------------------------
	
	ULONG UsbTimeout;	// Read timeout in seconds

	// Resource allocations
	bool GotPortOrMemory;
	bool PortInIOSpace;
	bool PortNeedsMapping;
	PUCHAR PortBase;
	PHYSICAL_ADDRESS PortStartAddress;
	ULONG PortLength;

	bool GotInterrupt;
	ULONG Vector;
	KIRQL Irql;
	KINTERRUPT_MODE Mode;
	KAFFINITY Affinity;
	PKINTERRUPT InterruptObject;

	ULONG					ShiftReg;

	bool SurprisedRemoved;	// Surprised removed device
} UsbQa_DEVICE_EXTENSION, *PUsbQa_DEVICE_EXTENSION;
*/


#endif




#endif	// __DRIVER_H__
