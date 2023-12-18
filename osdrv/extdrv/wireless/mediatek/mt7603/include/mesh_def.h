/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2005, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attempt
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	mesh_def.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2007-06-25		To Support Mesh (802.11s) feature.
*/

#ifndef __MESH_DEF_H__
#define __MESH_DEF_H__

#include "rtmp_type.h"
#include "mesh_mlme.h"

#define MAX_HOST_NAME_LEN				MAX_HOST_NAME_LENGTH + 1
#define MAX_MESH_ID_LEN					MAX_MESH_ID_LENGTH + 1
#define MAX_MESH_LINKS					MAX_MESH_LINK_NUM
#define MAX_NEIGHBOR_MP					MAX_NEIGHBOR_NUM
#define MAX_HASH_NEIGHBOR_MP			(2 * MAX_NEIGHBOR_MP)
#define MAX_BMPKTSIG_TAB_SIZE			64
/* Size of hash tab must be power of 2. */
#define MAX_HASH_BMPKTSIG_TAB_SIZE		(2 * MAX_BMPKTSIG_TAB_SIZE)
#define MULTIPATH_HASH_TAB_SIZE			256
#define MULTIPATH_POOL_SIZE				256
#define MAX_ROUTE_TAB_SIZE				64
#define MAX_HASH_ROUTE_TAB_SIZE		(2 * MAX_ROUTE_TAB_SIZE)
#define MAX_HASH_ENTRY_TAB_SIZE		256
#define MAX_HASH_PROXY_ENTRY_TAB_SIZE	256

#define MESH_BMPKT_RECORD_TIME			500
#define MESH_MAX_MSAIE_LEN				90	
#define MESH_MAX_PMKID_LEN				(2 * LEN_PMKID)
#define MESH_PMK_LEN					32
#define MESH_PMK_NAME_LEN				16

#define MESH_TTL						20
#define MAX_OPEN_RETRY					7
#define NEIGHBOR_MP_IDLE_CNT			2
#define MESH_AGEOUT_TIME				(10 * 1000 )
#define MESH_BMPKT_RECORD_TIME			500
#define MULTIPATH_AGEOUT				(60 * 1000 ) /* 10 seconds. */
#define MESH_PROXY_AGEOUT				(OS_HZ*1800) /*30mins */


#define DEFAULT_MESH_ID					"MESH"
#define DEFAULT_MESH_HOST_NAME			"RALINK_MESH"

#define MESH_TU				1
#define RANDOM_TIME(_pAd)	(RandomByte((_pAd)) * 1000)	/* 0 ~ 255 seconds */
#define PLD_TIME			(20 * 1000)	/* 20 seconds */
#define TOR_TIME			(1 * 1000)	/* 1 seconds */
#define TOC_TIME			(1 * 1000)	/* 1 seconds */
#define TOH_TIME			(150)	/* 150 ms */

#define MESH_NEIGHBOR_BEACON_IDLE_TIME	(5 * 1000)	/* 5 seconds. */

#define BMCAST_ROUTE_ID		0xff

/* Mesh Link Type */
#define MESH_LINK_DYNAMIC	0
#define MESH_LINK_STATIC	1

/* Mesh Operation Mode */
#define MESH_MP				(1 << 0)
#define MESH_AP				(1 << 1)
#define MESH_POTAL			(1 << 2)
#define MESH_MKD			(1 << 3)

/* Mesh Direction */
#define MESH_PROXY			0
#define MESH_FORWARD		1

/* Mesh version */
#define MESH_VERSION		1

#define NULL_PROTOCOL		255
/* Mesh Path selection protocols. */
#define MESH_HWMP			0
#define MESH_OLSR			1

/* Mesh Path Metric protocols. */
#define MESH_AIRTIME		0

/* FC Type field. */
#define BTYPE_MESH			FC_TYPE_MGMT

/* FC Subtype field. */
#define SUBTYPE_MULTIHOP	0xf

/* Mesh Reason Code */
#define MESH_LINK_CANCELLED							2
#define MESH_MAX_PEERS								3
#define MESH_CAPABILITY_POLICY_VIOLATION			4
#define MESH_CLOSE_RCVD								5
#define MESH_MAX_RETRIES							6
#define MESH_CONFIRM_TIMEOUT						7
#define MESH_SECURITY_ROLE_NEGOTIATION_DIFFERS		8
#define MESH_SECURITY_AUTHENTICATION_IMPOSSBLE		9
#define MESH_SECURITY_FAILED_VERIFICATION			10
#define MESH_INVALID_GTK							11
#define MESH_MISMATCH_GTK							12
#define MESH_INCONSISTENT_PARAMETERS				13

/* Mesh status codes */
#define MESH_LINK_ESTABLISHED							14
#define MESH_LINK_CLOSED								15
#define MESH_NO_LISTED_KEY_HOLDER_TRANSPORT_TYPE		16
#define MESH_NO_LISTED_KEY_HOLDER_SECURITY_HANDSHAKE	17
#define MESH_PEER_LINK_MAX_RETRIES						18
#define MESH_PEER_LINK_NO_PMK							19
#define MESH_PEER_LINK_ALT_PMK							20
#define MESH_PEER_LINK_NO_AKM							21
#define MESH_PEER_LINK_ALT_AKM							22
#define MESH_PEER_LINK_NO_KDF							23
#define MESH_PEER_LINK_SA_ESTABLISHED					24
#define MESH_AUTH_REJE_AN_ANTI_CLOGGING_TOKEN_REQUIRE	25

/* Peer Link Management SubType */
#define SUBTYPE_PEER_LINK_OPEN			0
#define SUBTYPE_PEER_LINK_CONFIRM		1
#define SUBTYPE_PEER_LINK_CLOSE			2

/* Category field */
#define CATEGORY_MESH_PEER_LINK			15
#define CATEGORY_MESH_LINK_METRIC		16
#define CATEGORY_MESH_PATH_SELECTION	17
#define CATEGORY_MESH_INTERWORKING		18
#define CATEGORY_MESH_RES_COORDINATION	19
#define CATEGORY_MESH_MSA				20
#define CATEGORY_MESH_PROXY_FORWARDING	21

/* Link Metric Action Code */
#define ACT_CODE_LINK_METRIC_REQ	0
#define ACT_CODE_LINK_METRIC_REP	1

/* Peer Link Management Action Code */
#define ACT_CODE_PEER_LINK_OPEN		0
#define ACT_CODE_PEER_LINK_CONFIRM		1
#define ACT_CODE_PEER_LINK_CLOSE		2

/* Path Selection Action Code */
#define ACT_CODE_PATH_REQUEST		0
#define ACT_CODE_PATH_REPLY		1
#define ACT_CODE_PATH_ERROR		2
#define ACT_CODE_MULTIPATH_NOTICE	3

/* Resource Coordination Action Code */
#define RESOURCE_CONGESTION_CONTROL_NOTIFICATION		0
#define RESOURCE_MDA_SETUP_REQUEST						1
#define RESOURCE_MDA_SETUP_REPLY						2
#define RESOURCE_MDAOP_ADVERTISEMENT_REQUEST			3
#define RESOURCE_MDAOP_ADVERTISEMENT					4
#define RESOURCE_MDAOP_SET_TEARDOWN						5
#define RESOURCE_BEACON_TIMING_REQUEST					6
#define RESOURCE_BEACON_TIMING_RESPONSE					7
/*#define RESOURCE_TBTT_ADJUSTMENT_REQUEST				8 // new add in Version D1.08 */
#define RESOURCE_CHANNEL_SWITCH_ANNOUNCEMENT			8 /* change to 9 in Version D1.08 */
#define RESOURCE_CONNECTIVITY_REPORT					9 /* remove from Version D1.08 */

/* Proxy Forwarding Action Code */
#define ACT_CODE_PROXY_UPDATE					0
#define ACT_CODE_PROXY_UPDATE_CONFIRMATION		1

/* HWMP default value */
#define MESH_MAX_LEN_OF_FORWARD_TABLE	MESH_MAX_FORWARD_ENTRY_NUM
#define HWMP_PREQ_REFRESH_PERIOD		(15 * 1000)
#define HWMP_PATH_LOSS_THRESHOLD		2
#define HWMP_ACTIVE_PATH_TIMEOUT		(5 * 1000)
#define HWMP_PREQ_RATELIMIT				2
#define HWMP_NET_DIAMETER				20
#define HWMP_MP_TRAVERSAL_TIME			40
#define HWMP_NETDIAMTER_TRAVERSAL_TIME	(HWMP_NET_DIAMETER*HWMP_MP_TRAVERSAL_TIME)
#define HWMP_NETDIAMETER_TRAVERSAL_TIME	(2 * HWMP_NETDIAMTER_TRAVERSAL_TIME)
#define HWMP_MAX_PREQ_RETRIES			3
#define HWMP_FORWARD_TABLE_LIFE_TIME	300

/* Mesh Element IEs. */
#define MESH_IE_OFFSET						100
#define IE_MESH_CONFIGURATION				21 + MESH_IE_OFFSET
#define IE_MESH_ID							22 + MESH_IE_OFFSET
#define IE_MESH_LINK_METRIC_REPORT			23 + MESH_IE_OFFSET
#define IE_MESH_CONGESTION_NOTIFICATION		24 + MESH_IE_OFFSET
#define IE_MESH_PEER_LINK_MANAGEMENT		25 + MESH_IE_OFFSET
#define IE_MESH_CHANNEL_SWITCH_ANNOUNCEMENT	26 + MESH_IE_OFFSET
#define IE_MESH_NEIGHBOR_LIST				27 + MESH_IE_OFFSET
#define IE_MESH_TIM							28 + MESH_IE_OFFSET
#define IE_MESH_ATIM_WINDOW_PARAMETER		29 + MESH_IE_OFFSET
#define IE_MESH_BEACOM_TIMING				30 + MESH_IE_OFFSET
#define IE_MESH_MDAOP_SETUP_REQUEST			31 + MESH_IE_OFFSET
#define IE_MESH_MDAOP_SETUP_REPLY			32 + MESH_IE_OFFSET
#define	IE_MESH_MDAOP_ADVERTISEMENTS		33 + MESH_IE_OFFSET
#define IE_MESH_MDAOP_SET_TEARDOWN			34 + MESH_IE_OFFSET
#define IE_MESH_CONNECTIVITY_REPORT			35 + MESH_IE_OFFSET
#define IE_MESH_PANN						36 + MESH_IE_OFFSET
#define IE_MESH_RANN						37 + MESH_IE_OFFSET
#define IE_MESH_PREQ						38 + MESH_IE_OFFSET
#define IE_MESH_PREP						39 + MESH_IE_OFFSET
#define IE_MESH_PERR						40 + MESH_IE_OFFSET
#define IE_MESH_PU							41 + MESH_IE_OFFSET
#define IE_MESH_PUC							42 + MESH_IE_OFFSET
#define IE_MESH_HELLO						43 + MESH_IE_OFFSET
#define IE_MESH_TC							44 + MESH_IE_OFFSET
#define IE_MESH_MID							45 + MESH_IE_OFFSET
#define IE_MESH_LABA						46 + MESH_IE_OFFSET
#define IE_MESH_LABCA						47 + MESH_IE_OFFSET
#define IE_MESH_ABBR						48 + MESH_IE_OFFSET
#define IE_MESH_MSCIE						49 + MESH_IE_OFFSET
#define IE_MESH_MSAIE						50 + MESH_IE_OFFSET
#define IE_MESH_MULITI_PATH_NOTICE_IE		51 + MESH_IE_OFFSET

/*for mesh proxy */
#define SignalLinkListInsertHeadQueue(Array,QueueHeader,QueueEntryIndex)	\
{												\
	Array[QueueEntryIndex].Next=(QueueHeader)->Head;	\
	(QueueHeader)->Head=QueueEntryIndex;	\
	if ((QueueHeader)->Tail==-1)				\
		(QueueHeader)->Tail=QueueEntryIndex;	 \
	(QueueHeader)->Number++;		\
}

#define SignalLinkListRemoveFromHeadQueue(Array,QueueHeader)	\
(QueueHeader)->Head;					\
{					\
	SHORT Next;						\
	if ((QueueHeader)->Head != -1)			\
	{										\
		Next=Array[(QueueHeader)->Head].Next;          \
		(QueueHeader)->Head = Next;                \
		if (Next==-1)						\
			(QueueHeader)->Tail = -1;             \
		(QueueHeader)->Number--;                    \
	}												\
}

#define DoubleLinkListInsertTailQueue(Array,QueueHeader,QueueEntryIndex)	\
{				\
	Array[QueueEntryIndex].DLNext=-1;	\
	Array[QueueEntryIndex].DLPrev=(QueueHeader)->Tail;		\
	if ((QueueHeader)->Tail!=-1)				\
		Array[(QueueHeader)->Tail].DLNext=QueueEntryIndex;		\
	(QueueHeader)->Tail=QueueEntryIndex;	\
	if ((QueueHeader)->Head==-1)				\
		(QueueHeader)->Head=QueueEntryIndex;	 \
	(QueueHeader)->Number++;		\
}

#define DoubleLinkListRemoveHeadQueue(Array,QueueHeader)	\
(QueueHeader)->Head;			\
{								\
	SHORT Next;						\
	if ((QueueHeader)->Head != -1)			\
	{										\
		Next=Array[(QueueHeader)->Head].DLNext;          \
		(QueueHeader)->Head = Next;                \
		if (Next==-1)						\
			(QueueHeader)->Tail = -1;             \
		else								\
			Array[(QueueHeader)->Head].DLPrev=-1;	\
		(QueueHeader)->Number--;                    \
	}												\
}

#define DoubleLinkListRemoveNode(Array,QueueHeader,QueueEntryIndex)	\
{						\
	if (Array[QueueEntryIndex].DLPrev==-1 && (QueueHeader)->Head==QueueEntryIndex)	\
		(QueueHeader)->Head =Array[(QueueHeader)->Head].DLNext;			\
	else				\
		Array[Array[QueueEntryIndex].DLPrev].DLNext=Array[QueueEntryIndex].DLNext;		\
					\
	if (Array[QueueEntryIndex].DLNext==-1 && (QueueHeader)->Tail==QueueEntryIndex)	\
		(QueueHeader)->Tail =Array[(QueueHeader)->Tail].DLPrev;			\
	else				\
		Array[Array[QueueEntryIndex].DLNext].DLPrev=Array[QueueEntryIndex].DLPrev;		\
	(QueueHeader)->Number--;                    \
}
/*for mesh proxy end */

typedef union _MESH_FLAG
{
	struct
	{
#ifdef RT_BIG_ENDIAN
		UINT8 Reserv:6;
		UINT8 AE:2;
#else
		UINT8 AE:2;
		UINT8 Reserv:6;
#endif
	}field;
	UINT8 word;
} MESH_FLAG, *PMESH_FLAG;

typedef struct GNU_PACKED _MESH_HEADER
{
	UINT8 MeshFlag;
	UINT8 MeshTTL;
	UINT32 MeshSeq;
} MESH_HEADER, *PMESH_HEADER;

typedef union _MESH_PREQ_FLAG
{
	struct
	{
#ifdef RT_BIG_ENDIAN
		UINT8 Reserv2:1;
		UINT8 AE:1;
		UINT8 Reserv:3;
		UINT8 ProactivePrep:1;
		UINT8 AddrType:1;
		UINT8 PortalRole:1;
#else
		UINT8 PortalRole:1;
		UINT8 AddrType:1;
		UINT8 ProactivePrep:1;
		UINT8 Reserv:3;
		UINT8 AE:1;
		UINT8 Reserv2:1;
#endif
	}field;
	UINT8 word;
} MESH_PREQ_FLAG, *PMESH_PREQ_FLAG;

typedef union _MESH_PREP_FLAG
{
	struct
	{
#ifdef RT_BIG_ENDIAN
		UINT8 Reserv1:1;
		UINT8 AE:1;
		UINT8 Reserv:6;
#else
		UINT8 Reserv:6;
		UINT8 AE:1;
		UINT8 Reserv1:1;
#endif
	}field;
	UINT8 word;
} MESH_PREP_FLAG, *PMESH_PREP_FLAG;

typedef union _MESH_PER_DEST_FLAG
{
	struct
	{
#ifdef RT_BIG_ENDIAN
		UINT8 Reserv:6;
		UINT8 RF:1;
		UINT8 DO:1;
#else
		UINT8 DO:1;
		UINT8 RF:1;
		UINT8 Reserv:6;
#endif
	}field;
	UINT8 word;
} MESH_PER_DEST_FLAG, *PMESH_PER_DEST_FLAG;

typedef union GNU_PACKED _MESH_SECURITY_CONFIGURATION
{
	struct {
#ifdef RT_BIG_ENDIAN
		UINT8 Reserv:5;
		UINT8 DefaultRole:1;
		UINT8 ConnectedToMKD:1;
		UINT8 MeshAuthenticator:1;
#else
		UINT8 MeshAuthenticator:1;
		UINT8 ConnectedToMKD:1;
		UINT8 DefaultRole:1;
		UINT8 Reserv:5;
#endif
	}field;
	UINT8 word;
} MESH_SECURITY_CONFIGURATION, *PMESH_SECURITY_CONFIGURATION;

typedef union GNU_PACKED _MESH_HANDSHAKE_CONTROL
{
	struct {
#ifdef RT_BIG_ENDIAN
		UINT8 Reserv:7;
		UINT8 RequestAuth:1;
#else
		UINT8 RequestAuth:1;
		UINT8 Reserv:7;
#endif
	}field;
	UINT8 word;
} MESH_HANDSHAKE_CONTROL, *PMESH_HANDSHAKE_CONTROL;

typedef union _MESH_CAPABILITY
{
	struct
	{
#ifdef RT_BIG_ENDIAN
		UINT16 Reserv:10;
		UINT16 PowerSaveLevel:1;
		UINT16 TBTTAdjustEnable:1;
		UINT16 BeaconTimingReportEnable:1;
		UINT16 Forwarding:1;
		UINT16 MDAEnable:1;
		UINT16 AcceptPeerLinks:1;
#else
		UINT16 AcceptPeerLinks:1;
		UINT16 MDAEnable:1;
		UINT16 Forwarding:1;
		UINT16 BeaconTimingReportEnable:1;
		UINT16 TBTTAdjustEnable:1;
		UINT16 PowerSaveLevel:1;
		UINT16 Reserv:10;
#endif
	}field;
	UINT16 word;
} MESH_CAPABILITY, *PMESH_CAPABILITY;

typedef struct _MESH_CONFIGURAION_IE
{
	UCHAR Version;
	UCHAR PathSelProtocolId;
	UCHAR PathSelMetricId;
	UINT32 CPI;
	MESH_CAPABILITY MeshCapability;
} MESH_CONFIGURAION_IE, *PMESH_CONFIGURAION_IE;

typedef struct _MESH_MULTIPATH_ENTRY
{
	struct _MESH_MULTIPATH_ENTRY *pNext;
	ULONG ReferTime;
	UCHAR MeshSA[MAC_ADDR_LEN];
} MESH_MULTIPATH_ENTRY, *PMESH_MULTIPATH_ENTRY;

/* MSC_IE */
typedef struct GNU_PACKED _MESH_SECURITY_CAPABILITY_IE
{	
	UCHAR	MKDDID[MAC_ADDR_LEN];	
	MESH_SECURITY_CONFIGURATION	MeshSecurityConfig;
} MESH_SECURITY_CAPABILITY_IE, *PMESH_SECURITY_CAPABILITY_IE;

/* MSA_IE */
typedef struct GNU_PACKED _MSA_HANDSHAKE_IE
{
	MESH_HANDSHAKE_CONTROL MeshHSControl;
	UCHAR	MA_ID[MAC_ADDR_LEN];
	UCHAR	SelectedAKM[LEN_OUI_SUITE];
	UCHAR	SelectedPairwiseCipher[LEN_OUI_SUITE];
	UCHAR	Octet[0];
} MSA_HANDSHAKE_IE, *PMSA_HANDSHAKE_IE;

typedef struct _MESH_LINK_ENTRY
{
	UCHAR 					OneSecBeaconCount;
	ULONG					OpenRetyCnt;
	ULONG					LastBeaconTime;
	UCHAR					LinkType;			/* 0: Dynamic, 1: Static. */
	BOOLEAN					Valid;				/* 1: Candidate MP decided, 0: no candidate MP. */

	UCHAR					PeerMacAddr[MAC_ADDR_LEN];
	USHORT					MacTabMatchWCID;	/* ASIC */
	UINT16					LocalLinkId;
	UINT16					PeerLinkId;
	UINT32					Metrics; 		/* link cost. */
	UINT16					PeerMeshSeq;
	LIST_HEADER				MultiPathHash[MULTIPATH_HASH_TAB_SIZE];
	
	UCHAR					LocalMsaIe[MESH_MAX_MSAIE_LEN];		/* Local MP's MSAIE */
	UCHAR					LocalMsaIeLen;

	UCHAR					RcvdMsaIe[MESH_MAX_MSAIE_LEN];		/* Save the received peer MP's MSAIE */
	UCHAR					RcvdMsaIeLen;
	
	MESH_SECURITY_CAPABILITY_IE	RcvdMscIe;						/* Save the received peer MP's MSCIE */
	
	UINT8					ExtChOffset;			/* save ExtChOffset to detect neighbor's change */
	
	UCHAR           		RcvdRSNIE[MAX_LEN_OF_RSNIE];
	UCHAR           		RcvdRSNIE_Len;
	
	UCHAR					RcvdPMKID[MESH_MAX_PMKID_LEN];
	UCHAR					RcvdPMKID_Len;

	BOOLEAN					bInitialMsaLink;			/* This is an Initial MSA link, no matter local or peer MP */
	BOOLEAN					bValidLocalMpAsSelector;
	BOOLEAN					bValidLocalMpAsAuthenticator;

	MESH_KEY_SELECTION		MeshKeySelection;

	UCHAR					PMK_MA[MESH_PMK_LEN];
	UCHAR					PMK_MA_NAME[MESH_PMK_NAME_LEN];	
		
} MESH_LINK_ENTRY, *PMESH_LINK_ENTRY;

typedef struct _MESH_LINK
{
	MESH_LINK_ENTRY			Entry;
	MESH_LINK_MNG_STATE		CurrentState;
	RALINK_TIMER_STRUCT		TOR, TOC, TOH;
} MESH_LINK, *PMESH_LINK;

typedef struct _MESH_NEIGHBOR
{
	struct _MESH_NEIGHBOR *pNext;
	BOOLEAN		Valid;
	/* needed attri flag such as MAP, Portal.. */
	
	UCHAR		PeerMac[MAC_ADDR_LEN];
	UCHAR		HostName[MAX_HOST_NAME_LEN];
	UCHAR		MeshId[MAX_MESH_ID_LEN];
	UCHAR		MeshIdLen;
	UINT8		Version;
	UINT32		CPI;
	UINT32		Channel;
	UINT8		ChBW;
	UINT8		ExtChOffset;
	UINT8		PathProtocolId;		/* 0: mean HWMP. */
	UINT8		PathMetricId;		/* 0: airtime. */
	MESH_CAPABILITY	MeshCapability;
	CHAR		RealRssi;

	UINT		IdleCnt;
	ULONG		LastBeaconTime;
	MESH_NEIGH_STATE	State;
	UCHAR		MeshLinkIdx;
	/*UCHAR		MeshEncrypType; */
	USHORT		CapabilityInfo;
	UCHAR		RSNIE[MAX_LEN_OF_RSNIE];
	USHORT		RSNIE_Len;
} MESH_NEIGHBOR_ENTRY, *PMESH_NEIGHBOR_ENTRY;

typedef struct _MESH_NEIGHBOR_TAB
{
	UCHAR NeighborNr;
	PMESH_NEIGHBOR_ENTRY Hash[MAX_HASH_NEIGHBOR_MP];
	MESH_NEIGHBOR_ENTRY NeighborMP[MAX_NEIGHBOR_MP];
} MESH_NEIGHBOR_TAB, *PMESH_NEIGHBOR_TAB;

typedef struct _MESH_ROUTING_ENTRY
{
	struct _MESH_ROUTING_ENTRY	*pNext;

	BOOLEAN	Valid;
	UCHAR	MeshDA[MAC_ADDR_LEN];
	UINT32	Dsn;
	UCHAR	NextHop[MAC_ADDR_LEN];
	UCHAR	NextHopLinkID;
	UINT32	PathMetric;
	BOOLEAN	bPrecursor;
	UCHAR	Precursor[MAC_ADDR_LEN];
	UINT32	LifeTime;
	UCHAR	Idx;
} MESH_ROUTING_ENTRY, *PMESH_ROUTING_ENTRY;

typedef struct _MESH_ROUTING_TABLE {
	USHORT	Size;
	PMESH_ROUTING_ENTRY	Hash[MAX_HASH_ENTRY_TAB_SIZE];
	MESH_ROUTING_ENTRY	Content[MAX_ROUTE_TAB_SIZE];
} MESH_ROUTING_TABLE, *PMESH_ROUTING_TABLE;

typedef struct _PATH_REQ_ENTRY{
	UCHAR	RetryCount;
	BOOLEAN	bProxied;
	UCHAR	ProxyAddr[MAC_ADDR_LEN];
	UCHAR	DestAddr[MAC_ADDR_LEN];
	UINT32	DestDsn;
	BOOLEAN	PathReqTimerRunning; 
	RALINK_TIMER_STRUCT PathReqTimer;
	PVOID	pAdapter;
} PATH_REQ_ENTRY, *PPATH_REQ_ENTRY;

typedef struct _MESH_ENTRY
{
	struct _MESH_ENTRY	*pNext;

	BOOLEAN	Valid;
	UCHAR	Idx;
	UCHAR	DestAddr[MAC_ADDR_LEN];
	BOOLEAN	PathReqTimerRunning;
	UINT32	LifeTime;
	PATH_REQ_ENTRY	*PathReq;
} MESH_ENTRY, *PMESH_ENTRY;

typedef struct _MESH_ENTRY_TABLE {
	USHORT		Size;
	PMESH_ENTRY	Hash[MAX_HASH_ENTRY_TAB_SIZE];
} MESH_ENTRY_TABLE, *PMESH_ENTRY_TABLE;

typedef struct  _QUEUE_HEADER_FOR_ARRAY   {
	SHORT    Head;
	SHORT    Tail;
	USHORT    Number;
}   QUEUE_HEADER_FOR_ARRAY, *PQUEUE_HEADER_FOR_ARRAY;

typedef struct _MESH_PROXY_ENTRY
{
	SHORT	Next;		/*mac hash link list next */
	SHORT  	DLNext;		/*double link list next */
	SHORT  	DLPrev;	/*doubli link list prev */

	BOOLEAN	isMesh;
	BOOLEAN	isProxied;
	ULONG 	LastUpdateTime;
	UCHAR	MacAddr[MAC_ADDR_LEN];
	UCHAR	Owner[MAC_ADDR_LEN];
} MESH_PROXY_ENTRY, *PMESH_PROXY_ENTRY;

typedef struct _MESH_PROXY_ENTRY_TABLE {
	QUEUE_HEADER_FOR_ARRAY AgeList;
	QUEUE_HEADER_FOR_ARRAY FreeEntryList;
	USHORT				Size;
	SHORT	Hash[MAX_HASH_ENTRY_TAB_SIZE];
	MESH_PROXY_ENTRY Content[MAX_HASH_PROXY_ENTRY_TAB_SIZE];
} MESH_PROXY_ENTRY_TABLE, *PMESH_PROXY_ENTRY_TABLE;

typedef struct _MESH_BMPKTSIG_ENTRY
{
	struct _MESH_BMPKTSIG_ENTRY	*pNext;

	BOOLEAN	Valid;
	UCHAR	MeshSA[MAC_ADDR_LEN];
	UCHAR	Precursor[MAC_ADDR_LEN];
	UINT32	MeshSeqBased;
	UINT32	Offset[4];
	/* mesh unicast packet doesn't need to check packet signature. */
	/*UINT32	UcaseMeshSeq;*/
	ULONG	LastRefTime;
} MESH_BMPKTSIG_ENTRY, *PMESH_BMPKTSIG_ENTRY;

typedef struct _MESH_BMPKTSIG_TAB
{
	UCHAR Size;
	PMESH_BMPKTSIG_ENTRY Hash[MAX_HASH_BMPKTSIG_TAB_SIZE];
	MESH_BMPKTSIG_ENTRY Content[MAX_BMPKTSIG_TAB_SIZE];
} MESH_BMPKTSIG_TAB, *PMESH_BMPKTSIG_TAB;

typedef struct GNU_PACKED _MESH_DEST_ENTRY
{
	UINT8	PerDestFlag;
    UCHAR   DestAddr[MAC_ADDR_LEN];
	UINT32	Dsn;
} MESH_DEST_ENTRY, *PMESH_DEST_ENTRY;

typedef struct GNU_PACKED _MESH_PERR_ENTRY
{
	UCHAR   DestAddr[MAC_ADDR_LEN];
	UINT32   Dsn;
} MESH_PERR_ENTRY, *PMESH_PERR_ENTRY;

typedef struct GNU_PACKED _MESH_PERR_TABLE
{
	UCHAR size;
	MESH_PERR_ENTRY PerrEntry[MAX_ROUTE_TAB_SIZE];
} MESH_PERR_TABLE, *PMESH_PERR_TABLE;

typedef struct GNU_PACKED _MESH_PREP_DEPENDENT_ENTRY
{
	UCHAR   DependAddr[MAC_ADDR_LEN];
	UINT32   Dsn;
} MESH_PREP_DEPENDENT_ENTRY, *PMESH_PREP_DEPENDENT_ENTRY;

typedef struct GNU_PACKED _MESH_PREP_DEPENDENT_TABLE
{
	UCHAR size;
	MESH_PREP_DEPENDENT_ENTRY PrepDependEntry[MAX_ROUTE_TAB_SIZE];
} MESH_PREP_DEPENDENT_TABLE, *PMESH_PREP_DEPENDENT_TABLE;

typedef struct GNU_PACKED _MESH_DEPENDENT_ENTRY
{
	UCHAR   DependAddr[MAC_ADDR_LEN];
	UINT32   Dsn;
} MESH_DEPENDENT_ENTRY, *PMESH_DEPENDENT_ENTRY;

#endif /* __MESH_DEF_H__ */

