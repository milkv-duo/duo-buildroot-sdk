/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
        rtmp_usb.h

    Abstract:
        Ralink USB related definitions and data dtructures

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */


#ifndef __RTMP_USB_H__
#define __RTMP_USB_H__

#include "rtusb_io.h"

extern UCHAR EpToQueue[6];

#ifdef DPA_T
#define RXBULKAGGRE_SIZE			5
#else /* DPA_T */
#define RXBULKAGGRE_SIZE			12
#endif /* !DPA_T */
#define MAX_TXBULK_LIMIT			(LOCAL_TXBUF_SIZE*(BULKAGGRE_SIZE-1))
#define MAX_TXBULK_SIZE			(LOCAL_TXBUF_SIZE*BULKAGGRE_SIZE)
#define MAX_RXBULK_SIZE			(LOCAL_TXBUF_SIZE*RXBULKAGGRE_SIZE)
#define MAX_MLME_HANDLER_MEMORY 20
#define CMD_RSP_BULK_SIZE	1024

#ifdef USB_BULK_BUF_ALIGMENT

#define ALIGMENT_BULKAGGRE_SIZE	7/* must >=  4*/


#define MAX_ALIGMENT_TXBULK_SIZE			(LOCAL_TXBUF_SIZE*ALIGMENT_BULKAGGRE_SIZE)
//#define BULKOUT_SIZE	0x00006000
#define BULKOUT_SIZE	(LOCAL_TXBUF_SIZE * (ALIGMENT_BULKAGGRE_SIZE -2))

#endif /* USB_BULK_BUF_ALIGMENT */


/*Power saving */
#define PowerWakeCID		3
#define CID0MASK		0x000000ff
#define CID1MASK		0x0000ff00
#define CID2MASK		0x00ff0000
#define CID3MASK		0xff000000


/* Flags for Bulkflags control for bulk out data */
#define	fRTUSB_BULK_OUT_DATA_NULL			0x00000001
#define	fRTUSB_BULK_OUT_RTS					0x00000002
#define	fRTUSB_BULK_OUT_MLME				0x00000004
#ifdef MT_MAC
#define	fRTUSB_BULK_OUT_BCN 				0x00000008
#endif /* MT_MAC */

#define	fRTUSB_BULK_OUT_PSPOLL				0x00000010
#define	fRTUSB_BULK_OUT_DATA_FRAG			0x00000020
#define	fRTUSB_BULK_OUT_DATA_FRAG_2			0x00000040
#define	fRTUSB_BULK_OUT_DATA_FRAG_3			0x00000080
#define	fRTUSB_BULK_OUT_DATA_FRAG_4			0x00000100

#define	fRTUSB_BULK_OUT_DATA_NORMAL			0x00010000
#define	fRTUSB_BULK_OUT_DATA_NORMAL_2		0x00020000
#define	fRTUSB_BULK_OUT_DATA_NORMAL_3		0x00040000
#define	fRTUSB_BULK_OUT_DATA_NORMAL_4		0x00080000
#define fRTUSB_BULK_OUT_DATA_NORMAL_5       0x00100000
#define	fRTUSB_BULK_OUT_DATA_WMM1_FRAG		0x00002000
#define	fRTUSB_BULK_OUT_DATA_WMM1_NORMAL	0x02000000

/* TODO:move to ./ate/include/iface/ate_usb.h */
#ifdef CONFIG_ATE
#define	fRTUSB_BULK_OUT_DATA_ATE			0x00100000
#endif /* CONFIG_ATE */

#define fRTMP_ADAPTER_NEED_STOP_TX		\
		(fRTMP_ADAPTER_NIC_NOT_EXIST | fRTMP_ADAPTER_HALT_IN_PROGRESS |	\
		 fRTMP_ADAPTER_RESET_IN_PROGRESS | fRTMP_ADAPTER_BULKOUT_RESET | \
		 fRTMP_ADAPTER_RADIO_OFF | fRTMP_ADAPTER_REMOVE_IN_PROGRESS)


#define FREE_HTTX_RING(_pCookie, _pipeId, _txContext)			\
{										\
	if ((_txContext)->ENextBulkOutPosition == (_txContext)->CurWritePosition)	\
	{																	\
		(_txContext)->bRingEmpty = TRUE;			\
	}																	\
	/*NdisInterlockedDecrement(&(_p)->TxCount); */\
}

#define NT_SUCCESS(status)			(((status) >=0) ? (TRUE):(FALSE))


#if 0
/* */
/* NdisMCancelTimer just canced the timer and not mean release the timer. */
/* And don't set the "Valid" to False. So that we can use this timer again. */
/* NOTE: now we didn't implement this feature!!! */
/* */
#define NdisMCancelTimer		RTMPCancelTimer
#define PMDL		PVOID
/*#define NdisMSendComplete			RTMP_SendComplete */
#endif


#define PIRP		PVOID
/*#define NDIS_OID	UINT */
#ifndef USB_ST_NOERROR
#define USB_ST_NOERROR     0
#endif

#ifdef THREADX
#undef  USB_ST_NOERROR
#define USB_ST_NOERROR		DC_URB_COMPLETE
#endif /* THREADX */

/* vendor-specific control operations */
#define CONTROL_TIMEOUT_JIFFIES ( (300 * OS_HZ) / 1000)
/*#define UNLINK_TIMEOUT_MS		3 // os abl move */


#define DEVICE_CLASS_REQUEST_OUT		0x20
#define DEVICE_VENDOR_REQUEST_OUT       0x40
#define DEVICE_VENDOR_REQUEST_IN        0xc0
/*#define INTERFACE_VENDOR_REQUEST_OUT    0x41 */
/*#define INTERFACE_VENDOR_REQUEST_IN     0xc1 */
#define BULKOUT_MGMT_RESET_FLAG		0x80

#define RTUSB_SET_BULK_FLAG(_M, _F)	((_M)->BulkFlags |= (_F))
#define RTUSB_CLEAR_BULK_FLAG(_M, _F)	((_M)->BulkFlags &= ~(_F))
#define RTUSB_TEST_BULK_FLAG(_M, _F)	(((_M)->BulkFlags & (_F)) != 0)

struct _MGMT_STRUC;
struct _TX_BLK;

NTSTATUS RTUSB_VendorRequest(
	IN	struct _RTMP_ADAPTER *pAd,
	IN	UINT32 TxFlags,
	IN	UCHAR ReservedBits,
	IN	UCHAR Request,
	IN	USHORT val,
	IN	USHORT idx,
	IN	PVOID txbuf,
	IN	UINT32 txbuf_len);

NTSTATUS RTUSBMultiRead(struct _RTMP_ADAPTER *pAd, USHORT Offset, UCHAR *buf, USHORT len);
NTSTATUS RTUSBMultiWrite(struct _RTMP_ADAPTER *pAd, USHORT Offset, UCHAR *buf, USHORT len, BOOLEAN bWriteHigh);
NTSTATUS RTUSBMultiWrite_nBytes(struct _RTMP_ADAPTER *pAd, USHORT Offset, UCHAR *buf, USHORT len, USHORT batchLen);
NTSTATUS RTUSBMultiWrite_OneByte(struct _RTMP_ADAPTER *pAd, USHORT Offset, UCHAR *pData);
NTSTATUS RTUSBSingleWrite(struct _RTMP_ADAPTER *pAd, USHORT Offset, USHORT val, BOOLEAN bWriteHigh);

NTSTATUS RTUSBReadBBPRegister(struct _RTMP_ADAPTER *pAd, UCHAR Id, UCHAR *pValue);
NTSTATUS RTUSBWriteBBPRegister(struct _RTMP_ADAPTER *pAd, UCHAR Id, UCHAR Value);
NTSTATUS RTUSBWriteRFRegister(struct _RTMP_ADAPTER *pAd, UINT32 Value);
NTSTATUS RTUSBWriteMACRegister(struct _RTMP_ADAPTER *pAd, USHORT Offset, UINT32 val, BOOLEAN bWriteHigh);
NTSTATUS RTUSBReadMACRegister(struct _RTMP_ADAPTER *pAd, USHORT Offset, UINT32 *val);

NTSTATUS RTUSBReadEEPROM(struct _RTMP_ADAPTER *pAd, USHORT Offset, UCHAR *buf, USHORT len);
NTSTATUS RTUSBWriteEEPROM(struct _RTMP_ADAPTER *pAd, USHORT Offset, UCHAR *buf, USHORT len);

NTSTATUS RTUSBFirmwareWrite(struct _RTMP_ADAPTER *pAd, UCHAR *pFwImage, ULONG FwLen);

NTSTATUS RTUSBVenderReset(struct _RTMP_ADAPTER *pAd);

BOOLEAN AsicCheckCommandOk(struct _RTMP_ADAPTER *pAd, UCHAR cmd);


NDIS_STATUS RTUSBEnqueueCmdFromNdis(struct _RTMP_ADAPTER *pAd, NDIS_OID Oid, BOOLEAN SetInfo, PVOID pInfoBuf, UINT32 BufLen);
VOID RTUSBDequeueCmd(PCmdQ cmdq, PCmdQElmt *pcmdqelmt);
INT RTUSBCmdThread(ULONG Context);


VOID RTUSBBssBeaconExit(struct _RTMP_ADAPTER *pAd);
VOID RTUSBBssBeaconStop(struct _RTMP_ADAPTER *pAd);
VOID RTUSBBssBeaconStart(struct _RTMP_ADAPTER * pAd);
VOID RTUSBBssBeaconInit(struct _RTMP_ADAPTER *pAd);


NDIS_STATUS RTUSBSetHardWareRegister(struct _RTMP_ADAPTER *pAd, PVOID pBuf);
NDIS_STATUS RTUSBWriteHWMACAddress(struct _RTMP_ADAPTER *pAd);
NDIS_STATUS RTUSBQueryHardWareRegister(struct _RTMP_ADAPTER *pAd, PVOID pBuf);

VOID RTUSBMlmeHardTransmit(struct _RTMP_ADAPTER *pAd, struct _MGMT_STRUC *pMgmt);

NDIS_STATUS RTUSBFreeDescRequest(struct _RTMP_ADAPTER *pAd, UCHAR BulkOutPipeId, UINT32 req_cnt);
BOOLEAN	RTUSBNeedQueueBackForAgg(struct _RTMP_ADAPTER *pAd, UCHAR BulkOutPipeId);

USHORT RtmpUSB_WriteSubTxResource(struct _RTMP_ADAPTER *pAd, struct _TX_BLK *pTxBlk, BOOLEAN bIsLast, USHORT *freeCnt);
USHORT RtmpUSB_WriteSingleTxResource(struct _RTMP_ADAPTER *pAd, struct _TX_BLK *pTxBlk, BOOLEAN bIsLast, USHORT *freeCnt);
USHORT RtmpUSB_WriteFragTxResource(struct _RTMP_ADAPTER *pAd, struct _TX_BLK *pTxBlk, UCHAR fragNum, USHORT *freeCnt);
USHORT RtmpUSB_WriteMultiTxResource(struct _RTMP_ADAPTER *pAd, struct _TX_BLK *pTxBlk, UCHAR frmNum, USHORT *freeCnt);
VOID RtmpUSB_FinalWriteTxResource(struct _RTMP_ADAPTER *pAd, struct _TX_BLK *pTxBlk, USHORT mpdu_len, USHORT TxIdx);

VOID RtmpUSBDataLastTxIdx(struct _RTMP_ADAPTER *pAd, UCHAR QueIdx, USHORT TxIdx);
VOID RtmpUSBDataKickOut(struct _RTMP_ADAPTER *pAd, struct _TX_BLK *pTxBlk, UCHAR QueIdx);
int RtmpUSBMgmtKickOut(struct _RTMP_ADAPTER *pAd, UCHAR QIdx, PNDIS_PACKET pkt, UCHAR *pSrcBufVA, UINT SrcBufLen);
VOID RtmpUSBNullFrameKickOut(struct _RTMP_ADAPTER *pAd, UCHAR QIdx, UCHAR *pNullFrm, UINT32 frmLen);

//VOID RTUSBWatchDog(struct _RTMP_ADAPTER *pAd);

VOID RTUSBPutToSleep(struct _RTMP_ADAPTER *pAd);
NTSTATUS RTUSBWakeUp(struct _RTMP_ADAPTER *pAd);
	
VOID RtmpUsbStaAsicForceWakeupTimeout(PVOID arg1, PVOID FuncContext, PVOID arg2, PVOID arg3);

VOID RT28xxUsbStaAsicForceWakeup(struct _RTMP_ADAPTER *pAd, BOOLEAN bFromTx);

VOID RT28xxUsbStaAsicSleepThenAutoWakeup(struct _RTMP_ADAPTER *pAd, USHORT TbttNumToNextWakeUp);

VOID UsbMlmeRadioOn(struct _RTMP_ADAPTER *pAd);
VOID UsbMlmeRadioOFF(struct _RTMP_ADAPTER *pAd);
VOID RT28xxUsbAsicRadioOn(struct _RTMP_ADAPTER *pAd);
VOID RT28xxUsbAsicRadioOff(struct _RTMP_ADAPTER *pAd);

VOID RT28xxUsbAsicWOWEnable(struct _RTMP_ADAPTER *pAd);
VOID RT28xxUsbAsicWOWDisable(struct _RTMP_ADAPTER *pAd);
NTSTATUS mtusb_read_reg(struct _RTMP_ADAPTER *pAd, UINT32 addr, UINT32 *value);
NTSTATUS mtusb_write_reg(struct _RTMP_ADAPTER *pAd, UINT32 addr, UINT32 value);

struct usb_control {
	BOOLEAN usb_aggregation;
};

enum {
	RX_FREEQ,
	RX_BULKINQ,
	RX_PROCESSINGQ,
};

UINT32 RTUSBQueueLen(struct _RTMP_ADAPTER *pAd, DL_LIST *list);

#endif /* __RTMP_USB_H__ */

