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
	mesh_mlme.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2007-06-25		To Support Mesh (802.11s) feature.
*/

#ifndef __MESH_MLME_H__
#define __MESH_MLME_H__

#include "mesh_def.h"

#define VALID_MESH_LINK_ID(_idx)	((_idx) < MAX_MESH_LINKS )

#define IS_MESH_IF(pObj)			(pObj->ioctl_if_type == INT_MESH)

typedef enum _MESH_NEIGH_STATE
{
	NEIGHBOR_MP,
	CANDIDATE_MP,
	ASSOCIATION_PENDING,
	LINK_UNAVAILABLE,
	LINK_AVAILABLE,	
} MESH_NEIGH_STATE;

/* states of mesh ctrl state machine */
typedef enum _MESH_CTRL_STATE
{
	MESH_CTRL_IDLE,
	MESH_CTRL_DISCOVERY,
	MESH_CTRL_ACTIVATED,
	MESH_CTRL_UCG,
	MESH_CTRL_MAX_STATES,
} MESH_CTRL_STATE;

/* events of mesh ctrl state machine */
typedef enum _MESH_CTRL_EVENT
{
	/* Events from SME. */
	MESH_CTRL_JOIN,			/* join a mesh. */
	MESH_CTRL_DEL_LINK,		/* delete a specific link. */
	MESH_CTRL_PLD,			/* discovery peer link. */
	MESH_CTRL_DISCONNECT,	/* leave the mesh. */

	/* Internal Events. */
	APMT2_MLME_SCAN_FINISH,
	APMT2_MLME_SCAN_ABORT,
	MESH_CTRL_UCG_EVT,
	MESH_PEER_UCG_EVT,

	/* Timer Events. */
	MESH_CTRL_MCS_TIMEOUT,
	MESH_CTRL_MAX_EVENTS,
} MESH_CTRL_EVENT;

#define	MESH_CTRL_FUNC_SIZE	(MESH_CTRL_MAX_STATES * MESH_CTRL_MAX_EVENTS)

/* states of mesh link management state machine */
typedef enum _MESH_LINK_MNG_STATE
{
	MESH_LINK_MNG_IDLE,
	MESH_LINK_MNG_LISTEN,
	MESH_LINK_MNG_OPN_SNT,
	MESH_LINK_MNG_CFN_RCVD,
	MESH_LINK_MNG_OPN_RCVD,
	MESH_LINK_MNG_ESTAB,
	MESH_LINK_MNG_HOLDING,
	MESH_LINK_MNG_MAX_STATES,
} MESH_LINK_MNG_STATE;

/* events of mesh link management state machine */
typedef enum _MESH_LINK_MNG_EVENT
{
	/* Events from SME. */
	MESH_LINK_MNG_CNCL,
	MESH_LINK_MNG_PASOPN,
	MESH_LINK_MNG_ACTOPN,

	/* External Events. */
	MESH_LINK_MNG_CLS_ACPT,
	MESH_LINK_MNG_CLS_IGNR,
	MESH_LINK_MNG_OPEN_ACPT,
	MESH_LINK_MNG_OPEN_RJCT,
	MESH_LINK_MNG_OPEN_IGNR,
	MESH_LINK_MNG_CFN_ACPT,
	MESH_LINK_MNG_CFN_RJCT,
	MESH_LINK_MNG_CFN_IGNR,

	/* Timer Events. */
	MESH_LINK_MNG_TOR1,
	MESH_LINK_MNG_TOR2,
	MESH_LINK_MNG_TOC,
	MESH_LINK_MNG_TOH,
	MESH_LINK_MNG_MAX_EVENTS,
} MESH_LINK_MNG_EVENT;

#define	MESH_LINK_MNG_FUNC_SIZE	(MESH_LINK_MNG_MAX_STATES * MESH_LINK_MNG_MAX_EVENTS)

typedef struct _MESH_LINK_OPEN_MSG_STRUCT
{
	UINT32 CPI;
	UINT16 MeshCapabilty;
	UINT16 PeerLinkId;
	UINT16 ReasonCode;
	UCHAR MaxSupportedRate;
	UCHAR SupRateLen;
	HT_CAPABILITY_IE HTCapability;
	UCHAR HTCapability_Len;
	BOOLEAN bWmmCapable;
} MESH_LINK_OPEN_MSG_STRUCT, *PMESH_LINK_OPEN_MSG_STRUCT;

typedef struct _MESH_LINK_CONFIRM_MSG_STRUCT
{
	UINT32 CPI;
	UINT16 StatusCode;
	UINT16 Aid;
	UINT16 MeshCapabilty;
	UINT16 LocalLinkId;
	UINT16 PeerLinkId;
	UINT16 ReasonCode;
} MESH_LINK_CONFIRM_MSG_STRUCT, *PMESH_LINK_CONFIRM_MSG_STRUCT;

typedef struct _MESH_LINK_CLOSE_MSG_STRUCT
{
	UINT16 LocalLinkId;
	UINT16 PeerLinkId;
	UINT16 ReasonCode;
} MESH_LINK_CLOSE_MSG_STRUCT, *PMESH_LINK_CLOSE_MSG_STRUCT;

typedef struct _MESH_CH_SW_ANN_MSG_STRUCT
{
	UINT32 NewCPI;
	UINT32 MeshSeq;
	UINT8 MeshTTL;
	UINT8 ChSwCnt;
	UINT8 NewCh;
	UINT8 ChSwMode;
	UINT8 LinkId;
	UCHAR MeshSA[MAC_ADDR_LEN];
} MESH_CH_SW_ANN_MSG_STRUCT, *PMESH_CH_SW_ANN_MSG_STRUCT;

#endif /* __MESH_MLME_H__ */
