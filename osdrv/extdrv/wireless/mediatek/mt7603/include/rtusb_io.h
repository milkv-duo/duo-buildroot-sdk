/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

 	Module Name:
	rtusb_io.h

	Abstract:

	Revision History:
	Who			When	    What
	--------	----------  ----------------------------------------------
	Name		Date	    Modification logs
*/

#ifndef __RTUSB_IO_H__
#define __RTUSB_IO_H__

#include "wpa_cmm.h"
#include "rtmp_type.h"

/* First RTUSB IO command number */
#define CMDTHREAD_FIRST_CMD_ID						0x0D730101

#define CMDTHREAD_RESET_BULK_OUT						0x0D730101
#define CMDTHREAD_RESET_BULK_IN						0x0D730102
#define CMDTHREAD_CHECK_GPIO							0x0D730103
#define CMDTHREAD_SET_ASIC_WCID						0x0D730104
#define CMDTHREAD_DEL_ASIC_WCID						0x0D730105
#define CMDTHREAD_SET_CLIENT_MAC_ENTRY				0x0D730106

#ifdef CONFIG_STA_SUPPORT
#define CMDTHREAD_SET_PSM_BIT							0x0D730107
#define CMDTHREAD_FORCE_WAKE_UP						0x0D730108
#define CMDTHREAD_FORCE_SLEEP_AUTO_WAKEUP			0x0D730109
#define CMDTHREAD_QKERIODIC_EXECUT					0x0D73010A
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
#define CMDTHREAD_AP_UPDATE_CAPABILITY_AND_ERPIE	0x0D73010B
#define CMDTHREAD_AP_ENABLE_TX_BURST				0x0D73010C
#define CMDTHREAD_AP_DISABLE_TX_BURST				0x0D73010D
#define CMDTHREAD_AP_ADJUST_EXP_ACK_TIME			0x0D73010E
#define CMDTHREAD_AP_RECOVER_EXP_ACK_TIME			0x0D73010F
#define CMDTHREAD_CHAN_RESCAN						0x0D730110
#endif /* CONFIG_AP_SUPPORT */

#define CMDTHREAD_SET_LED_STATUS				0x0D730111	/* Set WPS LED status (LED_WPS_XXX). */
#ifdef WSC_INCLUDED
#ifdef WSC_LED_SUPPORT
#define CMDTHREAD_LED_WPS_MODE10					0x0D730112
#endif /* WSC_LED_SUPPORT */
#endif /* WSC_INCLUDED */

/* Security related */
#define CMDTHREAD_SET_WCID_SEC_INFO					0x0D730113
#define CMDTHREAD_SET_ASIC_WCID_IVEIV				0x0D730114
#define CMDTHREAD_SET_ASIC_WCID_ATTR				0x0D730115
#define CMDTHREAD_SET_ASIC_SHARED_KEY				0x0D730116
#define CMDTHREAD_SET_ASIC_PAIRWISE_KEY				0x0D730117
#define CMDTHREAD_REMOVE_PAIRWISE_KEY				0x0D730118
#ifdef CONFIG_STA_SUPPORT
#define CMDTHREAD_SET_PORT_SECURED					0x0D730119
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
#define CMDTHREAD_802_11_COUNTER_MEASURE			0x0D73011A
#endif /* CONFIG_AP_SUPPORT */

/* add by johnli, fix "in_interrupt" error when call "MacTableDeleteEntry" in Rx tasklet */
#define CMDTHREAD_UPDATE_PROTECT					0x0D73011B
/* end johnli */

#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
#define CMDTHREAD_REG_HINT							0x0D73011C
#define CMDTHREAD_REG_HINT_11D						0x0D73011D
#define CMDTHREAD_SCAN_END							0x0D73011E
#define CMDTHREAD_CONNECT_RESULT_INFORM				0x0D73011F
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */

#ifdef P2P_SUPPORT
#define CMDTHREAD_SET_P2P_LINK_DOWN				0x0D730120
#endif /* P2P_SUPPORT */

#ifdef BT_COEXISTENCE_SUPPORT
#define RT_CMD_COEXISTENCE_DETECTION				0x0D730121
#endif /* BT_COEXISTENCE_SUPPORT */

#ifdef RT3593
#define CMDTHREAD_UPDATE_TX_CHAIN_ADDRESS			0x0D730122
#endif

#define CMDTHREAD_ADDREMOVE_ASIC_KEY				0x0D730124
#define CMDTHREAD_PS_CLEAR							0x0D730125
#define CMDTHREAD_PS_RETRIEVE_START					0x0D730126
#define CMDTHREAD_PS_RETRIEVE_RSP					0x0D730127
#define CMDTHREAD_PS_IGNORE_PSM_SET                 0x0D730128

#ifdef CFG_TDLS_SUPPORT
#define CMDTHREAD_TDLS_SEND_CH_SW_SETUP				0x0D730129
#define CMDTHREAD_TDLS_AUTO_TEARDOWN				0x0D73012A
#endif /* CFG_TDLS_SUPPORT */

//#ifdef BCN_OFFLOAD_SUPPORT
#define HWCMD_ID_BCN_UPDATE                         0x0D73012B
#define HWCMD_ID_BMC_CNT_UPDATE                     0x0D73012C
//#endif

#define CMDTHREAD_PERODIC_CR_ACCESS_ASIC_UPDATE_PROTECT			0x0D73012D
#define CMDTHREAD_PERODIC_CR_ACCESS_MLME_DYNAMIC_TX_RATE_SWITCHING	0x0D73012E
#define CMDTHREAD_PERODIC_CR_ACCESS_NIC_UPDATE_RAW_COUNTERS	0x0D73012F
#define CMDTHREAD_PERODIC_CR_ACCESS_WTBL_RATE_TABLE_UPDATE		0x0D730130
#define CMDTHREAD_MLME_PERIOIDC_EXEC								0x0D730131

#ifdef RELEASE_EXCLUDE
/* Reserved region for cmd which are not in use */
#define CMDTHREAD_VENDOR_RESET                      			0x0D73FFFF
#define CMDTHREAD_VENDOR_UNPLUG                     			0x0D73FFFF
#define CMDTHREAD_VENDOR_SWITCH_FUNCTION			0x0D73FFFF
#define CMDTHREAD_MULTI_WRITE_MAC                   			0x0D73FFFF
#define CMDTHREAD_MULTI_READ_MAC                    			0x0D73FFFF
#define CMDTHREAD_VENDOR_EEPROM_WRITE               		0x0D73FFFF
#define CMDTHREAD_VENDOR_EEPROM_READ                		0x0D73FFFF
#define CMDTHREAD_VENDOR_ENTER_TESTMODE             		0x0D73FFFF
#define CMDTHREAD_VENDOR_EXIT_TESTMODE              		0x0D73FFFF
#define CMDTHREAD_VENDOR_WRITE_BBP                  		0x0D73FFFF
#define CMDTHREAD_VENDOR_READ_BBP                   			0x0D73FFFF
#define CMDTHREAD_VENDOR_WRITE_RF                   			0x0D73FFFF
#define CMDTHREAD_VENDOR_FLIP_IQ                    			0x0D73FFFF
#define CMDTHREAD_SET_RADIO                         				0x0D73FFFF
#define CMDTHREAD_UPDATE_TX_RATE                    			0x0D73FFFF
#define CMDTHREAD_RESET_FROM_ERROR                  		0x0D73FFFF
#define CMDTHREAD_LINK_DOWN                         			0x0D73FFFF
#define CMDTHREAD_RESET_FROM_NDIS                   			0x0D73FFFF
#define CMDTHREAD_SET_BW                            				0x0D73FFFF
#define CMDTHREAD_802_11_QUERY_HARDWARE_REGISTER 	0x0D73FFFF
#define CMDTHREAD_802_11_SET_PHY_MODE               		0x0D73FFFF
#define CMDTHREAD_802_11_SET_STA_CONFIG             		0x0D73FFFF
#define CMDTHREAD_802_11_SET_PREAMBLE               		0x0D73FFFF
#endif /* RELEASE_EXCLUDE */

typedef struct _CMDHandler_TLV {
	USHORT Offset;
	USHORT Length;
	UCHAR DataFirst;
} CMDHandler_TLV, *PCMDHandler_TLV;

#if 0
typedef struct _MLME_MEMORY_STRUCT {
	PVOID AllocVa;		/*Pointer to the base virtual address of the allocated memory */
	struct _MLME_MEMORY_STRUCT *Next;	/*Pointer to the next virtual address of the allocated memory */
} MLME_MEMORY_STRUCT, *PMLME_MEMORY_STRUCT;

typedef struct _MLME_MEMORY_HANDLER {
	BOOLEAN MemRunning;	/*The flag of the Mlme memory handler's status */
	UINT MemoryCount;	/*Total nonpaged system-space memory not size */
	UINT InUseCount;	/*Nonpaged system-space memory in used counts */
	UINT UnUseCount;	/*Nonpaged system-space memory available counts */
	INT PendingCount;	/*Nonpaged system-space memory for free counts */
	PMLME_MEMORY_STRUCT pInUseHead;	/*Pointer to the first nonpaed memory not used */
	PMLME_MEMORY_STRUCT pInUseTail;	/*Pointer to the last nonpaged memory not used */
	PMLME_MEMORY_STRUCT pUnUseHead;	/*Pointer to the first nonpaged memory in used */
	PMLME_MEMORY_STRUCT pUnUseTail;	/*Pointer to the last nonpaged memory in used */
	PULONG MemFreePending[MAX_MLME_HANDLER_MEMORY];	/*an array to keep pending free-memory's pointer (32bits) */
} MLME_MEMORY_HANDLER, *PMLME_MEMORY_HANDLER;
#endif

typedef struct _RT_SET_ASIC_WCID {
	ULONG WCID;		/* mechanism for rekeying: 0:disable, 1: time-based, 2: packet-based */
	ULONG SetTid;		/* time-based: seconds, packet-based: kilo-packets */
	ULONG DeleteTid;	/* time-based: seconds, packet-based: kilo-packets */
	UCHAR Addr[MAC_ADDR_LEN];	/* avoid in interrupt when write key */
	UCHAR Tid;
	UINT16 SN;
	UCHAR Basize;	
	INT   Ses_type;
	BOOLEAN IsAdd;
} RT_SET_ASIC_WCID, *PRT_SET_ASIC_WCID;

typedef struct _RT_ASIC_WCID_SEC_INFO {
	UCHAR BssIdx;
	UCHAR KeyIdx;
	UCHAR CipherAlg;
	UINT8 Wcid;
	UINT8 KeyTabFlag;
} RT_ASIC_WCID_SEC_INFO, *PRT_ASIC_WCID_SEC_INFO;

typedef struct _RT_ASIC_WCID_IVEIV_ENTRY {
	UINT8 Wcid;
	UINT32 Iv;
	UINT32 Eiv;
} RT_ASIC_WCID_IVEIV_ENTRY, *PRT_ASIC_WCID_IVEIV_ENTRY;

typedef struct _RT_ASIC_WCID_ATTR_ENTRY {
	UCHAR BssIdx;
	UCHAR KeyIdx;
	UCHAR CipherAlg;
	UINT8 Wcid;
	UINT8 KeyTabFlag;
} RT_ASIC_WCID_ATTR_ENTRY, *PRT_ASIC_WCID_ATTR_ENTRY;

typedef struct _RT_ASIC_PAIRWISE_KEY {
	UINT8 WCID;
	CIPHER_KEY CipherKey;
} RT_ASIC_PAIRWISE_KEY, *PRT_ASIC_PAIRWISE_KEY;

typedef struct _RT_ASIC_SHARED_KEY {
	UCHAR BssIndex;
	UCHAR KeyIdx;
	CIPHER_KEY CipherKey;
} RT_ASIC_SHARED_KEY, *PRT_ASIC_SHARED_KEY;

typedef struct _RT_ASIC_PROTECT_INFO {
	USHORT OperationMode;
	UCHAR SetMask;	
	BOOLEAN bDisableBGProtect;
	BOOLEAN bNonGFExist;	
} RT_ASIC_PROTECT_INFO, *PRT_ASIC_PROTECT_INFO;

typedef struct _MT_ASIC_SEC_INFO {
	UCHAR 			AddRemove;	
	UCHAR			BssIdx;
	UCHAR			KeyIdx;
	UCHAR			Wcid;
	UCHAR			KeyTabFlag;
	CIPHER_KEY	 	CipherKey;
	UCHAR 			Addr[MAC_ADDR_LEN];
} MT_ASIC_SEC_INFO, *PMT_ASIC_SEC_INFO;

/******************************************************************************

  	USB Cmd to ASIC Related MACRO

******************************************************************************/
/* reset MAC of a station entry to 0xFFFFFFFFFFFF */
#define RTMP_STA_ENTRY_MAC_RESET(pAd, Wcid)					\
	{	RT_SET_ASIC_WCID	SetAsicWcid;						\
		SetAsicWcid.WCID = Wcid;								\
		RTEnqueueInternalCmd(pAd, CMDTHREAD_DEL_ASIC_WCID, 	\
				&SetAsicWcid, sizeof(RT_SET_ASIC_WCID));	}

/* Set MAC register value according operation mode */
#ifdef CONFIG_AP_SUPPORT
#define RTMP_AP_UPDATE_CAPABILITY_AND_ERPIE(pAd)	\
 	RTEnqueueInternalCmd(pAd, CMDTHREAD_AP_UPDATE_CAPABILITY_AND_ERPIE, NULL, 0);
#endif /* CONFIG_AP_SUPPORT */

/* Insert the BA bitmap to ASIC for the Wcid entry */
#define RTMP_ADD_BA_SESSION_TO_ASIC(_pAd, _Aid, _TID, _SN, _basize, _type)					\
		do{																\
			RT_SET_ASIC_WCID	SetAsicWcid;							\
			SetAsicWcid.WCID = (_Aid);									\
			SetAsicWcid.SetTid = (0x10000<<(_TID));						\
			SetAsicWcid.DeleteTid = 0xffffffff;							\
			SetAsicWcid.Tid = _TID;										\
			SetAsicWcid.SN = _SN;										\
			SetAsicWcid.Basize = _basize;								\
			SetAsicWcid.Ses_type = _type;								\
			SetAsicWcid.IsAdd = 1;										\
			RTEnqueueInternalCmd((_pAd), CMDTHREAD_SET_ASIC_WCID, &SetAsicWcid, sizeof(RT_SET_ASIC_WCID));	\
		}while(0)

/* Remove the BA bitmap from ASIC for the Wcid entry */
#define RTMP_DEL_BA_SESSION_FROM_ASIC(_pAd, _Wcid, _TID, _type)				\
		do{																\
			RT_SET_ASIC_WCID	SetAsicWcid;							\
			SetAsicWcid.WCID = (_Wcid);									\
			SetAsicWcid.SetTid = (0xffffffff);							\
			SetAsicWcid.DeleteTid = (0x10000<<(_TID) );					\
			SetAsicWcid.Tid = _TID;										\
			SetAsicWcid.Basize = 0;										\
			SetAsicWcid.Ses_type = _type;								\
			SetAsicWcid.IsAdd = 0;										\
			RTEnqueueInternalCmd((_pAd), CMDTHREAD_SET_ASIC_WCID, &SetAsicWcid, sizeof(RT_SET_ASIC_WCID));	\
		}while(0)

#define RTMP_UPDATE_PROTECT(_pAd, _OperationMode, _SetMask, _bDisableBGProtect, _bNonGFExist)	\
		do {\
			RT_ASIC_PROTECT_INFO AsicProtectInfo;\
			AsicProtectInfo.OperationMode = (_OperationMode);\
			AsicProtectInfo.SetMask = (_SetMask);\
			AsicProtectInfo.bDisableBGProtect = (_bDisableBGProtect);\
			AsicProtectInfo.bNonGFExist = (_bNonGFExist);\
			RTEnqueueInternalCmd((_pAd), CMDTHREAD_UPDATE_PROTECT, &AsicProtectInfo, sizeof(RT_ASIC_PROTECT_INFO));\
		} while(0)

void usb_cfg_read_v1(struct _RTMP_ADAPTER *ad, UINT32 *value);
void usb_cfg_write_v1(struct _RTMP_ADAPTER *ad, UINT32 value);
void usb_cfg_read_v2(struct _RTMP_ADAPTER *ad, UINT32 *value);
void usb_cfg_write_v2(struct _RTMP_ADAPTER *ad, UINT32 value);
void usb_cfg_read_v3(struct _RTMP_ADAPTER *ad, UINT32 *value);
void usb_cfg_write_v3(struct _RTMP_ADAPTER *ad, UINT32 value);
void mtusb_cfg_read(struct _RTMP_ADAPTER *ad, UINT32 *value);
void mtusb_cfg_write(struct _RTMP_ADAPTER *ad, UINT32 value);
int write_reg(struct _RTMP_ADAPTER *ad, UINT32 base, UINT16 offset, UINT32 val);
int read_reg(struct _RTMP_ADAPTER *ad, UINT32 base, UINT16 offset, UINT32 *value);
int usb2_disconnect_cmd(struct _RTMP_ADAPTER *ad, UINT8 Discon_mode);

#endif /* __RTUSB_IO_H__ */
