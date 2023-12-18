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
 	Dma.h
 
	Abstract:
    DMA function header file

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   08-21-2002    created

*/

#ifndef	__DMA_H__
#define	__DMA_H__




/*
User1(Random length) [17]
User2(Inc length) [18]
User3(Dec length) [19]
User4(Data+CRC) [20]
Qos(ART) [21]
Qos(ART N) [22]
Qos(ART Rs) [23]
Qos(ART Rs Al) [24]
Qos(ART Rs Al Mt)
*/





typedef	struct _RXINFO_STRUC//RXD
{
	// DWORD 0
	ULONG		BA:1;
	ULONG		DATA:1;
	ULONG		NULLDATA:1;
	ULONG		FRAG:1;
	ULONG		U2M:1;              // 1: this RX frame is unicast to me
	ULONG		Mcast:1;            // 1: this is a multicast frame
	ULONG		Bcast:1;            // 1: this is a broadcast frame	
	ULONG		MyBss:1;  	// 1: this frame belongs to the same BSSID	
	ULONG		CrcErr:1;              // 1: CRC error
	ULONG		IcvErr:1;
	ULONG		MicErr:1;
	ULONG		AMSDU:1;		// rx with 802.3 header, not 802.11 header.
	ULONG		HTC:1;
	ULONG		RSSI:1;
	ULONG		L2PAD:1;
	ULONG		AMPDU:1;	
	ULONG		Decrypted:1;	// this frame is being decrypted.

	ULONG		BSSIDx3:1;
	ULONG		WAPI_KID:1;
	ULONG		PN_LEN:3;
	ULONG		Rsv:6;
	ULONG		TCHKBPS:1;
	ULONG		ICHKBPS:1;
	ULONG		TcpErr:1;
	ULONG		IpErr:1;	
}RXINFO_STRUC, *PRXINFO_STRUC;

//****************************************************
//2860c RX Wireless Information
//****************************************************
typedef	struct _RX_WI_STRUC
{
	// DWORD 0
	ULONG WCID:8;               //Wireless cilent ID
	ULONG KEY_IDX:2;          //As shared key index
	ULONG BSS_IDX:3;         
	ULONG UDF:3;                 //User defined fields
	ULONG MPDU_SIZE:14;   //MPDU total length
	ULONG Reserved_1:1;
	ULONG TheEOF:1;

	// DWORD 1
	ULONG TID:4;//FN:4;                   //Fragment number 
	ULONG SN:12;                 //Sequence number
	ULONG MCS:7;
	ULONG BW:2;
	ULONG SGI:1;
	ULONG STBC:1;
	ULONG ETXBF:1;
	ULONG ITXBF:1;
	ULONG PHY_MODE:3;
	
	// DWORD 2
	ULONG RSSI_0:8;
	ULONG RSSI_1:8;
	ULONG RSSI_2:8;
	ULONG Reserved_3:8;

	// DWORD 3
	ULONG Reserved_4:8;
	ULONG Reserved_5:8;
	ULONG SNR_0:8;
	ULONG SNR_1:8;

	// DWORD 4
	ULONG SNR_2:8;
	ULONG BFSNR_0:8;
	ULONG BFSNR_1:8;
	ULONG BFSNR_2:8;

	// DWORD 4 //RT5572
//	ULONG RxPerPktSetting1;

	// DWORD 5 //RT5572
	ULONG RxPerPktSetting2;

	// DWORD 6 //RT85592, RT6590
	ULONG RxPerPktSetting3;
}RX_WI_STRUC, *PRX_WI_STRUC;
//****************************************************

//****************************************************
// FW
//****************************************************
//
// Information type (INFO_TYPE_XXX)
//
#define INFO_TYPE_DOT11_PKT		(0)
#define INFO_TYPE_PKT_CMD		(1)
#define INFO_TYPE_RSP_EVT		(1)

//
// Response event type (RSP_EVT_TYPE_XXX)
//
#define RSP_EVT_TYPE_DONE		(0)
#define RSP_EVT_TYPE_RETRY		(1)
#define RSP_EVT_TYPE_ERROR		(2)

//
// Port types (PORT_TYPE_XXX)
//
#define PORT_TYPE_WLAN_PORT				(0)
#define PORT_TYPE_CPU_RX_PORT			(1)
#define PORT_TYPE_CPU_TX_PORT			(2)
#define PORT_TYPE_HOST_PORT				(3)
#define PORT_TYPE_VIRTUAL_CPU_RX_PORT	(4)
#define PORT_TYPE_VIRTUAL_CPU_TX_PORT	(5)
#define PORT_TYPE_DISCARD				(6)

typedef	struct _FW_TXD_STRUC 
{
		// DWORD 0
	ULONG SDP0;
	
	// DWORD 1
	ULONG SDL1:14;
	ULONG LS1:1;
	ULONG Burst:1;	
	ULONG SDL0:14;
	ULONG LS0:1;
	ULONG DDONE:1;

	// DWORD 2
	ULONG SDP1;	

	// DWORD 3
	// Tx FCE Info
	ULONG PktLength:16;
	ULONG CMD_SEQ:4;
	ULONG CMD_TYPE:7;
	ULONG D_PORT:3;
	ULONG INFO_TYPE:2;
}FW_TXD_STRUC, *PFW_TXD_STRUC;

typedef	struct _FW_CMD_REQ_STRUC 
{
	// Tx FCE Info
	FW_TXD_STRUC FwTxInfo;

	// Payload
	UCHAR Payload[128];
	
}FW_CMD_REQ_STRUC, *PFW_CMD_REQ_STRUC;

typedef	struct _FW_RXD_STRUC
{
	// DWORD 0
	ULONG		SDP0;
	
	// DWORD 1
	ULONG		SDL1:14;
	ULONG		Rsv:2;
	ULONG		SDL0:14;
	ULONG		LS0:1;
	ULONG		DDONE:1;
	
	// DWORD 2
	ULONG		SDP1;
	
	// DWORD 3
	// Rx FCE Info
	ULONG	PktLength:13; // Packet length
	ULONG	Reserved1:1; // Reserved
	ULONG	Reserved2:1; // Reserved
	ULONG	SelfGenRspEvt:1; // Self-generated response event
	ULONG	CMD_SEQ:4; // Packet command sequence
	ULONG	RspEvtType:4; // Response event type (RSP_EVT_TYPE_XXX)
	ULONG	Reserved3:1; // Reserved
	ULONG	Reserved4:2; // Reserved
	ULONG	S_PORT:3; // Which S-Port (PORT_TYPE_XXX)
	ULONG	INFO_TYPE:2; // Information type (INFO_TYPE_XXX)
}FW_RXD_STRUC, *PFW_RXD_STRUC;

typedef	struct _FW_CMD_RSP_STRUC 
{
	// Rx FCE Info
	FW_RXD_STRUC FwRxInfo;

	// Payload
	UCHAR Payload[128];
	
}FW_CMD_RSP_STRUC, *PFW_CMD_RSP_STRUC;
//****************************************************

typedef	struct _PAYLOAD_DEBUG_INFO {
	TXD_STRUC			TxdDescriptor;
	SCATTER_CONTROL	ScatterControl;
}PAYLOAD_DEBUG_INFO, *PPAYLOAD_DEBUG_INFO;

VOID InitDMA(RTMP_ADAPTER *pAd);

NTSTATUS 
ProbeDmaData(
	IN PDEVICE_OBJECT DeviceObject,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
	);

NTSTATUS 
GetDmaAdapter( 
	IN PDEVICE_OBJECT pDevObj 
	);

NTSTATUS	
AllocateDMAMemory(
	IN PDEVICE_OBJECT DeviceObject
	);

NTSTATUS	
FreeDMAMemory(
	IN PDEVICE_OBJECT DeviceObject
	);

NTSTATUS 
TransferDMADataBuffer(
	IN PDEVICE_OBJECT DeviceObject,
	IN PUCHAR pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
	);

NTSTATUS 
TransferDMADescriptor(
	IN PDEVICE_OBJECT DeviceObject,
	IN PUCHAR pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
	);

NTSTATUS 
DumpDMA(
	IN PDEVICE_OBJECT DeviceObject,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
	);

NTSTATUS 
EraseDMA(
	IN PDEVICE_OBJECT DeviceObject,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
	);


void InitSequenceNumber();

void IncSequenceNumber(UCHAR* pDesBuffer, USHORT RingNum);

USHORT PayloadLenRandom(RTMP_ADAPTER *pDevExt, USHORT min, USHORT max, TXD_STRUC* pTxD);
VOID PayloadDataRandom(RTMP_ADAPTER *pDevExt, USHORT StartOffset, USHORT Length, UCHAR* pDesBuffer);

#define DTXWI_SIZE 32

#endif	// __DMA_H__
