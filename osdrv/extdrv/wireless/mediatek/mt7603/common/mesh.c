/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	mesh.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2007-06-25      For mesh (802.11s) support.
*/

#ifdef MESH_SUPPORT


#include "rt_config.h"
#include "mesh_sanity.h"

extern UCHAR OUI_WPA_NONE_AKM[4];
extern UCHAR OUI_WPA_TKIP[4];
extern UCHAR OUI_WPA_CCMP[4];
extern UCHAR OUI_WPA2_TKIP[4];
extern UCHAR OUI_WPA2_CCMP[4];
extern UCHAR OUI_MSA_8021X_AKM[4];		/* Not yet final - IEEE 802.11s-D1.06 */
extern UCHAR OUI_MSA_PSK_AKM[4];		/* Not yet final - IEEE 802.11s-D1.06 */


typedef VOID (*MESH_ACT_FRAME_HANDLER_FUNC)(IN PRTMP_ADAPTER pAd, IN RX_BLK *pRxBlk);

typedef struct _MESH_ACTION_HANDLER
{
	UINT8 Category;
	UINT8 ActionCode;
	MESH_ACT_FRAME_HANDLER_FUNC pHandle;
} MESH_ACTION_HANDLER, *PMESH_ACTION_HANDLER;

static MESH_ACTION_HANDLER MeshActHandler[] = 
{
	/* Peer Link Management. */
	{CATEGORY_MESH_PEER_LINK, ACT_CODE_PEER_LINK_OPEN, (MESH_ACT_FRAME_HANDLER_FUNC)MeshPeerLinkOpenProcess},
	{CATEGORY_MESH_PEER_LINK, ACT_CODE_PEER_LINK_CONFIRM, (MESH_ACT_FRAME_HANDLER_FUNC)MeshPeerLinkConfirmProcess},
	{CATEGORY_MESH_PEER_LINK, ACT_CODE_PEER_LINK_CLOSE, (MESH_ACT_FRAME_HANDLER_FUNC)MeshPeerLinkCloseProcess},

	/* Peer Link Metric. */
	{CATEGORY_MESH_LINK_METRIC, ACT_CODE_LINK_METRIC_REP, (MESH_ACT_FRAME_HANDLER_FUNC)MeshPeerLinkMetricReportProcess},

	/* HWMP. */
	{CATEGORY_MESH_PATH_SELECTION, ACT_CODE_PATH_REQUEST, (MESH_ACT_FRAME_HANDLER_FUNC)MeshPreqRcvProcess},
	{CATEGORY_MESH_PATH_SELECTION, ACT_CODE_PATH_REPLY, (MESH_ACT_FRAME_HANDLER_FUNC)MeshPrepRcvProcess},
	{CATEGORY_MESH_PATH_SELECTION, ACT_CODE_PATH_ERROR, (MESH_ACT_FRAME_HANDLER_FUNC)MeshPerrRcvProcess},
	{CATEGORY_MESH_PATH_SELECTION, ACT_CODE_MULTIPATH_NOTICE,
															(MESH_ACT_FRAME_HANDLER_FUNC)MeshMultipathNoticeRcvProcess},

	{CATEGORY_MESH_RES_COORDINATION, RESOURCE_CHANNEL_SWITCH_ANNOUNCEMENT,
															(MESH_ACT_FRAME_HANDLER_FUNC)MeshChSwAnnounceProcess}
};
#define MESH_ACT_HANDLER_TAB_SIZE (sizeof(MeshActHandler) / sizeof(MESH_ACTION_HANDLER))



#if 0 /* os abl move to common/mesh_inf.c */
#ifdef LINUX
#if (WIRELESS_EXT >= 12)
struct iw_statistics *Mesh_VirtualIF_get_wireless_stats(
	IN  struct net_device *net_dev);
#endif
#endif /* LINUX */
#endif /* 0 */

INT Set_MeshId_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT   success = FALSE;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	if(strlen(arg) <= MAX_MESH_ID_LEN)
	{
		NdisZeroMemory(pAd->MeshTab.MeshId, MAX_MESH_ID_LEN);
		NdisMoveMemory(pAd->MeshTab.MeshId, arg, strlen(arg));
		pAd->MeshTab.MeshIdLen = (UCHAR)strlen(arg);
		success = TRUE;

		DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) Set_MeshId_Proc::(Len=%d,MeshId=%s)\n",
			pAd->MeshTab.MeshIdLen, pAd->MeshTab.MeshId));
	
		MeshDown(pAd, TRUE);
		MeshUp(pAd);		
	}
	else
		success = FALSE;

	return success;
}

INT Set_MeshHostName_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT   success = FALSE;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	if(strlen(arg) <= MAX_HOST_NAME_LEN)
	{
		NdisZeroMemory(pAd->MeshTab.HostName, MAX_HOST_NAME_LEN);
		NdisMoveMemory(pAd->MeshTab.HostName, arg, strlen(arg));
		success = TRUE;

		DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) Set_MeshHostName_Proc::(HostName=%s)\n",
			pAd->MeshTab.HostName));
	}
	else
		success = FALSE;

	return success;
}

INT Set_MeshAutoLink_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT Enable;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	Enable = simple_strtol(arg, 0, 16);

	pAd->MeshTab.MeshAutoLink = (Enable > 0) ? TRUE : FALSE;

	DBGPRINT(RT_DEBUG_TRACE, ("%s::(enable = %d)\n", __FUNCTION__, pAd->MeshTab.MeshAutoLink));
	
	return TRUE;
}


INT Set_MeshForward_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT Enable;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	Enable = simple_strtol(arg, 0, 16);

	pAd->MeshTab.MeshCapability.field.Forwarding = (Enable > 0) ? (1) : (0);

	DBGPRINT(RT_DEBUG_TRACE, ("%s::(enable = %d)\n", __FUNCTION__, pAd->MeshTab.MeshCapability.field.Forwarding));
	
	return TRUE;
}


#ifdef RELEASE_EXCLUDE
/*
========================================================================
Note:
		if we r not MAP or MP with proxy , we will drop all packets SA is not us or MeshDA and DA are not us.
		if MP is bridged , it must enable proxy or layer 3 packets will not be forwarded correctly
		because bridge mac will not be the same as MP wireless card mac.
========================================================================
*/
#endif /* RELEASE_EXCLUDE */
INT Set_MeshPortal_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT Enable;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	Enable = simple_strtol(arg, 0, 16);

	if (Enable)
		pAd->MeshTab.OpMode |= MESH_POTAL;
	else
		pAd->MeshTab.OpMode ^= MESH_POTAL;	
	

	DBGPRINT(RT_DEBUG_TRACE, ("%s::(enable = %d)\n", __FUNCTION__, pAd->MeshTab.OpMode));
	
	return TRUE;
}

INT Set_MeshAddLink_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT i;
	RTMP_STRING *value;
	UCHAR PeerMac[MAC_ADDR_LEN];	
	ULONG LinkIdx;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	if(strlen(arg) == 17)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 */
	{
		for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"), i++) 
		{
			if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
				return FALSE;  /*Invalid */

			AtoH(value, &PeerMac[i], 1);
		}

		if(i != 6)
			return FALSE;  /*Invalid */
	}

	LinkIdx = GetMeshLinkId(pAd, (PCHAR)PeerMac);
	if (LinkIdx == BSS_NOT_FOUND)
	{
		LinkIdx = MeshLinkAlloc(pAd, PeerMac, MESH_LINK_STATIC);
		if (LinkIdx == BSS_NOT_FOUND)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s() All Mesh-Links been occupied.\n", __FUNCTION__));
			return FALSE;
		}
	}

	if (!VALID_MESH_LINK_ID(LinkIdx))
		return FALSE;

	MlmeEnqueue(pAd, MESH_LINK_MNG_STATE_MACHINE, MESH_LINK_MNG_ACTOPN, 0, NULL, LinkIdx);

	DBGPRINT(RT_DEBUG_TRACE, ("%s::(LinkIdx = %ld)\n", __FUNCTION__, LinkIdx));
	
	return TRUE;
}

INT Set_MeshDelLink_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT i;
	RTMP_STRING *value;
	UCHAR PeerMac[MAC_ADDR_LEN];	
	ULONG LinkIdx;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	if(strlen(arg) == 17)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 */
	{
		for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"), i++) 
		{
			if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
				return FALSE;  /*Invalid */

			AtoH(value, &PeerMac[i], 1);
		}

		if(i != 6)
			return FALSE;  /*Invalid */
	}

	LinkIdx = GetMeshLinkId(pAd, (PCHAR)PeerMac);
	if (!VALID_MESH_LINK_ID(LinkIdx))
		return FALSE;

	pAd->MeshTab.MeshLink[LinkIdx].Entry.LinkType = MESH_LINK_DYNAMIC;
	MlmeEnqueue(pAd, MESH_LINK_MNG_STATE_MACHINE, MESH_LINK_MNG_CNCL, 0, NULL, LinkIdx);

	DBGPRINT(RT_DEBUG_TRACE, ("%s::(LinkIdx = %ld)\n", __FUNCTION__, LinkIdx));
	
	return TRUE;
}

INT Set_MeshMaxTxRate_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT Rate;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	Rate = simple_strtol(arg, 0, 10);

	if (Rate <= 12)
		pAd->MeshTab.MeshMaxTxRate = Rate;
	else 
		DBGPRINT(RT_DEBUG_ERROR, ("%s::Wrong Tx Rate setting(%d), (0 ~ 12))\n", __FUNCTION__, Rate));

	DBGPRINT(RT_DEBUG_TRACE, ("%s::(Max Tx Rate = %ld)\n", __FUNCTION__, pAd->MeshTab.MeshMaxTxRate));
	
	return TRUE;
}

INT Set_MeshRouteAdd_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT	success = TRUE;
#if 0
	PUCHAR thisChar = NULL;
	UCHAR DestAddr[MAC_ADDR_LEN];
	UCHAR MeshDestAddr[MAC_ADDR_LEN];
	UCHAR NextHop[MAC_ADDR_LEN];
	UCHAR Addr[MAC_ADDR_LEN];
	ULONG Metric = 0;
	UCHAR i;
	PUCHAR value;
	UCHAR Count = 0;
	PMESH_ROUTING_ENTRY pEntry = NULL;
	PMAC_TABLE_ENTRY pMacEntry = NULL;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	while ((thisChar = strsep((char **)&arg, "-")) != NULL)
	{
		if (Count == 0)
		{
			/* Get Dest Addr */
			if(strlen(thisChar) == 17)
			{
				for (i=0, value = rstrtok(thisChar,":"); value; value = rstrtok(NULL,":"))
				{
					if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
						return FALSE;  /*Invalid */

					AtoH(value, &Addr[i++], 1);
				}

				if(i != 6)
					return FALSE;  /*Invalid */
			}
			else
			{
				return FALSE;  /*Invalid */
			}

			COPY_MAC_ADDR(DestAddr, Addr);
		}
		else if (Count == 1)
		{
			/* Get Mesh DA */
			if(strlen(thisChar) == 17)
			{
				for (i=0, value = rstrtok(thisChar,":"); value; value = rstrtok(NULL,":"))
				{
					if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
						return FALSE;  /*Invalid */

					AtoH(value, &Addr[i++], 1);
				}

				if(i != 6)
					return FALSE;  /*Invalid */
			}
			else
			{
				return FALSE;	
			}

			COPY_MAC_ADDR(MeshDestAddr, Addr);
		}
		else if (Count == 2)
		{
			/* Get Next Hop */
			if(strlen(thisChar) == 17)
			{
				for (i=0, value = rstrtok(thisChar,":"); value; value = rstrtok(NULL,":"))
				{
					if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
						return FALSE;  /*Invalid */

					AtoH(value, &Addr[i++], 1);
				}

				if(i != 6)
					return FALSE;  /*Invalid */
			}
			else
			{
				return FALSE;	
			}

			COPY_MAC_ADDR(NextHop, Addr);
		}
		else if (Count == 3)
		{
			/* Get Metric */
			if ((thisChar != NULL) && (strlen(thisChar) > 0))
				Metric = simple_strtol(thisChar, 0, 10);
			else
				return FALSE;
		}
		else
		{
			return FALSE;
		}

		Count++;
	}

	if (MeshEntryTableLookUp(pAd, DestAddr))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("The Dest Mesh Point aleady on Route Table!!!\n"));
		return FALSE;
	}
	else
	{
		pMacEntry = MeshTableLookup(pAd, NextHop, TRUE);

		if (pMacEntry == NULL)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Next Hop have not on Mac Table !!!\n"));
			return FALSE;
		}

		pEntry = MeshRoutingTableLookup(pAd, MeshDestAddr);
		if (!(pEntry && MAC_ADDR_EQUAL(pEntry->NextHop, NextHop)))
		{
			pEntry = MeshRoutingTableInsert(pAd, DestAddr, MeshDestAddr, 0, NextHop, pMacEntry->func_tb_idx, Metric);
			if (!pEntry)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Add Dest Mesh Point Route fail !!!\n"));
				return FALSE;
			}
		}

		if (!MeshEntryTableInsert(pAd, DestAddr, pEntry->Idx))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Add Dest Mesh Point Route fail !!!\n"));
			return FALSE;
		}

		if (MeshTableLookup(pAd, DestAddr, FALSE) == NULL)
		{
			MeshEntryTableInsert(pAd, MeshDestAddr, pEntry->Idx);
		}
	}
#endif
	return success;
}

INT Set_MeshRouteDelete_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT	success = TRUE;
#if 0
	PUCHAR value;
	UCHAR DestAddr[MAC_ADDR_LEN];
	UCHAR i;
	PMESH_ENTRY pMeshEntry = NULL;
	PMESH_ROUTING_ENTRY pRouteEntry = NULL;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	if(strlen(arg) == 17)
	{
		for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":"))
		{
			if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
				return FALSE;  /*Invalid */

			AtoH(value, &DestAddr[i++], 1);
		}

		if(i != 6)
			return FALSE;  /*Invalid */
	}
	else
	{
		return FALSE;  /*Invalid */
	}

	pMeshEntry = MeshEntryTableLookUp(pAd, DestAddr);
	pRouteEntry = MeshRoutingTableLookup(pAd, DestAddr);

	if (pMeshEntry && pRouteEntry)
		MeshRoutingTableDelete(pAd, DestAddr);
	else
	MeshEntryTableDelete(pAd, DestAddr);
#endif
	return success;
}

INT Set_MeshRouteUpdate_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT	success = TRUE;
#if 0
	PUCHAR thisChar = NULL;
	UCHAR DestAddr[MAC_ADDR_LEN];
	UCHAR MeshDestAddr[MAC_ADDR_LEN];
	UCHAR NextHop[MAC_ADDR_LEN];
	UCHAR Addr[MAC_ADDR_LEN];
	ULONG Metric = 0;
	UCHAR i;
	PUCHAR value;
	UCHAR Count = 0;
	PMESH_ENTRY pMeshEntry = NULL;
	PMESH_ROUTING_ENTRY pRouteEntry = NULL;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	while ((thisChar = strsep((char **)&arg, "-")) != NULL)
	{
		if (Count == 0)
		{
			/* Get Dest Addr */
			if(strlen(thisChar) == 17)
			{
				for (i=0, value = rstrtok(thisChar,":"); value; value = rstrtok(NULL,":"))
				{
					if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
						return FALSE;  /*Invalid */

					AtoH(value, &Addr[i++], 1);
				}

				if(i != 6)
					return FALSE;  /*Invalid */
			}
			else
			{
				return FALSE;  /*Invalid */
			}

			COPY_MAC_ADDR(DestAddr, Addr);
		}
		else if (Count == 1)
		{
			/* Get Mesh DA */
			if(strlen(thisChar) == 17)
			{
				for (i=0, value = rstrtok(thisChar,":"); value; value = rstrtok(NULL,":"))
				{
					if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
						return FALSE;  /*Invalid */

					AtoH(value, &Addr[i++], 1);
				}

				if(i != 6)
					return FALSE;  /*Invalid */
			}
			else
			{
				return FALSE;	
			}

			COPY_MAC_ADDR(MeshDestAddr, Addr);
		}
		else if (Count == 2)
		{
			/* Get Next Hop */
			if(strlen(thisChar) == 17)
			{
				for (i=0, value = rstrtok(thisChar,":"); value; value = rstrtok(NULL,":"))
				{
					if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
						return FALSE;  /*Invalid */

					AtoH(value, &Addr[i++], 1);
				}

				if(i != 6)
					return FALSE;  /*Invalid */
			}
			else
			{
				return FALSE;	
			}

			COPY_MAC_ADDR(NextHop, Addr);
		}
		else if (Count == 3)
		{
			/* Get Metric */
			if ((thisChar != NULL) && (strlen(thisChar) > 0))
				Metric = simple_strtol(thisChar, 0, 10);
			else
				return FALSE;
		}
		else
		{
			return FALSE;
		}

		Count++;
	}

	pMeshEntry = MeshEntryTableLookUp(pAd, DestAddr);
	pRouteEntry = MeshRoutingTableLookup(pAd, MeshDestAddr);
	
	if (pMeshEntry)
	{
		pRouteEntry = &pAd->MeshTab.MeshRouteTab.Content[pMeshEntry->Idx];
		
		COPY_MAC_ADDR(pRouteEntry->MeshDestAddr, MeshDestAddr);
		COPY_MAC_ADDR(pRouteEntry->NextHop, NextHop);
		pRouteEntry->PathMetric = Metric;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("The can not find the Mesh Route !!!\n"));
		return FALSE;
	}
#endif
	return success;
}

INT Set_MeshMultiCastAgeOut_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT AgeTime;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	AgeTime = simple_strtol(arg, 0, 10);

	if ((AgeTime <= 65535) && (AgeTime >= 1))
		pAd->MeshTab.MeshMultiCastAgeOut = AgeTime;
	else 
		DBGPRINT(RT_DEBUG_ERROR, ("%s::Wrong MeshMultiCastAgeOut setting(%d), (1 ~ 65535))\n", __FUNCTION__, AgeTime));

	DBGPRINT(RT_DEBUG_TRACE, ("%s::(MeshMultiCastAgeOut = %ld)\n", __FUNCTION__, pAd->MeshTab.MeshMultiCastAgeOut));

	pAd->MeshTab.MeshMultiCastAgeOut = (AgeTime * 1000);

	return TRUE;
}

INT Set_MeshAuthMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR	i;
	INT		success = TRUE;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	struct wifi_dev *wdev;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	wdev = &pAd->MeshTab.wdev;
	if (rtstrcasecmp(arg, "WPANONE") == TRUE)
		wdev->AuthMode = Ndis802_11AuthModeWPANone;		
	else
		wdev->AuthMode = Ndis802_11AuthModeOpen;

	/* Set all mesh link as Port_Not_Secure */
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		if (IS_ENTRY_MESH(&pAd->MacTab.Content[i]))
			pAd->MacTab.tr_entry[i].PortSecured  = WPA_802_1X_PORT_NOT_SECURED;
	}
		
	RTMPMakeRSNIE(pAd, wdev->AuthMode, wdev->WepStatus, MIN_NET_DEVICE_FOR_MESH);
	
	if(wdev->AuthMode >= Ndis802_11AuthModeWPA)
	{	
		if (wdev->AuthMode == Ndis802_11AuthModeWPANone)
			wdev->DefaultKeyId = 0;
		else
			wdev->DefaultKeyId = 1;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) Set_MeshAuthMode_Proc::(MeshAuthMode(%d)=%s)\n",
			wdev->AuthMode, GetAuthMode(wdev->AuthMode)));
	
	return success;
}

INT Set_MeshEncrypType_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT	success = TRUE;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	struct wifi_dev *wdev;


	if(!IS_MESH_IF(pObj))
		return FALSE;

	wdev = &pAd->MeshTab.wdev;
	if (rtstrcasecmp(arg, "WEP") == TRUE)
    {
		if (wdev->AuthMode < Ndis802_11AuthModeWPA)
			wdev->WepStatus = Ndis802_11WEPEnabled;				  
	}
	else if (rtstrcasecmp(arg, "TKIP") == TRUE)
	{
		if (wdev->AuthMode >= Ndis802_11AuthModeWPA)
			wdev->WepStatus = Ndis802_11TKIPEnable;                       
    }
	else if (rtstrcasecmp(arg, "AES") == TRUE)
	{
		if (wdev->AuthMode >= Ndis802_11AuthModeWPA)
			wdev->WepStatus = Ndis802_11AESEnable;                            
	}    
	else
	{
		wdev->WepStatus = Ndis802_11WEPDisabled;                 
	}

	if(wdev->WepStatus >= Ndis802_11TKIPEnable)
	{		
		if (wdev->AuthMode == Ndis802_11AuthModeWPANone)
			wdev->DefaultKeyId = 0;
		else
			wdev->DefaultKeyId = 1;
	}
								
	RTMPMakeRSNIE(pAd, wdev->AuthMode, wdev->WepStatus, MIN_NET_DEVICE_FOR_MESH);

	DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) Set_MeshEncrypType_Proc::(MeshEncrypType(%d)=%s)\n",
			wdev->WepStatus, GetEncryptType(wdev->WepStatus)));

	return success;
}

INT Set_MeshDefaultkey_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ULONG		KeyIdx;
	INT			success = TRUE;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	struct wifi_dev *wdev;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	wdev = &pAd->MeshTab.wdev;
	KeyIdx = simple_strtol(arg, 0, 10);
	if((KeyIdx >= 1 ) && (KeyIdx <= 4))
		wdev->DefaultKeyId = (UCHAR) (KeyIdx - 1);
	else
		wdev->DefaultKeyId = 0;	/* Default value */

	DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) Set_MeshDefaultkey_Proc::(MeshDefaultkey=%d)\n",
										wdev->DefaultKeyId));

	return success;
}

INT Set_MeshWEPKEY_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR		i;
	UCHAR		KeyLen;
	UCHAR		CipherAlg = CIPHER_NONE;
	INT			success = TRUE;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	KeyLen = strlen(arg);

	switch (KeyLen)
	{
		case 5: /*wep 40 Ascii type */
		case 13: /*wep 104 Ascii type */
			pAd->MeshTab.SharedKey.KeyLen = KeyLen;
			NdisMoveMemory(pAd->MeshTab.SharedKey.Key, arg, KeyLen);
			if (KeyLen == 5)
				CipherAlg = CIPHER_WEP64;
			else
				CipherAlg = CIPHER_WEP128;

			NdisMoveMemory(pAd->MeshTab.DesiredWepKey, arg, KeyLen);
			pAd->MeshTab.DesiredWepKeyLen= KeyLen;
			
			DBGPRINT(RT_DEBUG_TRACE, ("IF(mesh0) Set_MeshWEPKRY_Proc::(WepKey=%s ,type=%s, Alg=%s)\n", arg, "Ascii", CipherName[CipherAlg]));		
			break;
		case 10: /*wep 40 Hex type */
		case 26: /*wep 104 Hex type */
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(arg+i)) )
					return FALSE;  /*Not Hex value; */
			}
			pAd->MeshTab.SharedKey.KeyLen = KeyLen/2 ;
			AtoH(arg, pAd->MeshTab.SharedKey.Key, KeyLen/2);
			if (KeyLen == 10)
				CipherAlg = CIPHER_WEP64;
			else
				CipherAlg = CIPHER_WEP128;

			NdisMoveMemory(pAd->MeshTab.DesiredWepKey, arg, KeyLen);
			pAd->MeshTab.DesiredWepKeyLen = KeyLen;
			
			DBGPRINT(RT_DEBUG_TRACE, ("IF(mesh0) Set_MeshWEPKRY_Proc::(WepKey=%s, type=%s, Alg=%s)\n", arg, "Hex", CipherName[CipherAlg]));		
			break;				
		default: /*Invalid argument */
			pAd->MeshTab.SharedKey.KeyLen = 0;
			pAd->MeshTab.DesiredWepKeyLen = KeyLen;
			DBGPRINT(RT_DEBUG_ERROR, ("IF(mesh0) Set_MeshWEPKRY_Proc::Invalid argument (=%s)\n", arg));		
			return FALSE;
	}

	pAd->MeshTab.SharedKey.CipherAlg = CipherAlg;
    

	return success;
}


INT Set_MeshWPAKEY_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	/*UCHAR	keyMaterial[40]; */
	INT		success = TRUE;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

#if 0
	if ((strlen(arg) < 8) || (strlen(arg) > 64))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("IF(mesh0) Set failed!!(PassPhrasKey=%s), the key string required 8 ~ 64 characters \n", arg));
		return FALSE;
	}

	NdisMoveMemory(pAd->MeshTab.WPAPassPhraseKey, arg, strlen(arg));
	pAd->MeshTab.WPAPassPhraseKeyLen = strlen(arg);

	if (strlen(arg) == 64)
	{
	    AtoH(arg, pAd->MeshTab.PMK, 32);
	}
	else
	{
	    RtmpPasswordHash((char *)arg, pAd->MeshTab.MeshId, pAd->MeshTab.MeshIdLen, keyMaterial);
	    NdisMoveMemory(pAd->MeshTab.PMK, keyMaterial, 32);
	}
#else
	success = RT_CfgSetWPAPSKKey(pAd, arg, strlen(arg), pAd->MeshTab.MeshId, pAd->MeshTab.MeshIdLen, pAd->MeshTab.PMK);
	if (success == FALSE)
		return FALSE;

	NdisMoveMemory(pAd->MeshTab.WPAPassPhraseKey, arg, strlen(arg));
	pAd->MeshTab.WPAPassPhraseKeyLen = strlen(arg);

#endif
	DBGPRINT(RT_DEBUG_TRACE, ("IF(mesh0) Set_MeshWPAKEY_Proc::PassPhrasKey (=%s)\n", arg));		
	
	return success;
}

INT Set_MeshRouteInfo_Display_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT i;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	PMESH_ROUTING_TABLE	pRouteTab = pAd->MeshTab.pMeshRouteTab;

	if (pRouteTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Route Table doesn't exist.\n", __FUNCTION__));
		return FALSE;
	}

	if(!IS_MESH_IF(pObj))
		return FALSE;

	DBGPRINT(RT_DEBUG_OFF, ("\n\n"));

	DBGPRINT(RT_DEBUG_OFF, ("\n%-19s%-6s%-19s%-16s%-12s%-9s\n",
		"MESH DA", "DSN", "NEXTHOP", "NEXTHOPLINKID", "METRICS", "ROUTE_IDX"));
	
	for (i=0; i<MAX_ROUTE_TAB_SIZE; i++)
	{
		PMESH_ROUTING_ENTRY pEntry = &pRouteTab->Content[i];
		if (pEntry->Valid)
		{
			DBGPRINT(RT_DEBUG_OFF, ("%02X:%02X:%02X:%02X:%02X:%02X  ", PRINT_MAC(pEntry->MeshDA)));
			DBGPRINT(RT_DEBUG_OFF, ("%-6d", pEntry->Dsn));
			DBGPRINT(RT_DEBUG_OFF, ("%02X:%02X:%02X:%02X:%02X:%02X  ", PRINT_MAC(pEntry->NextHop)));
			DBGPRINT(RT_DEBUG_OFF, ("%-16d", (int)pEntry->NextHopLinkID));
			DBGPRINT(RT_DEBUG_OFF, ("%-12d", pEntry->PathMetric));
			DBGPRINT(RT_DEBUG_OFF, ("%-9d\n", (int)pEntry->Idx));
		}
	} 

	return TRUE;
}


INT Set_MeshProxyInfo_Display_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{

	PMESH_PROXY_ENTRY	pEntry = NULL;
	SHORT	EntryIndex;
	PMESH_PROXY_ENTRY_TABLE	pProxyTab = pAd->MeshTab.pMeshProxyTab;
	int i=0;

	DBGPRINT(RT_DEBUG_OFF, ("pProxyTab size:%d free:%d\n",pProxyTab->AgeList.Number,pProxyTab->FreeEntryList.Number));

	EntryIndex=pProxyTab->AgeList.Head;
	if (EntryIndex >= 0)
	pEntry=&pProxyTab->Content[EntryIndex];

	DBGPRINT(RT_DEBUG_OFF, ("Mesh Proxy Age Out List\n"));
	DBGPRINT(RT_DEBUG_OFF, ("Index,DLNext,DLPrev,HashNext,Mac\n"));
	while(EntryIndex!=-1)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%02d %02d %02d %02d MAC:%02x:%02x:%02x:%02x:%02x:%02x \n",
			
			EntryIndex,pEntry->DLNext,pEntry->DLPrev,pEntry->Next,
			pEntry->MacAddr[0],pEntry->MacAddr[1],pEntry->MacAddr[2],
			pEntry->MacAddr[3],pEntry->MacAddr[4],pEntry->MacAddr[5]));

		EntryIndex=pEntry->DLNext;
		pEntry=&pProxyTab->Content[EntryIndex];
	
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Mesh Proxy Mac Hash Table \n"));
	for(i=0;i<MAX_HASH_ENTRY_TAB_SIZE;i++)
	{
		if (pProxyTab->Hash[i]!=-1)
			DBGPRINT(RT_DEBUG_TRACE, ("%03d : %d \n",i,pProxyTab->Hash[i]));
	
	}


	DBGPRINT(RT_DEBUG_OFF, ("Mesh Content Table \n"));
	DBGPRINT(RT_DEBUG_OFF, ("No,HashNext,DLNext,DLPrev\n"));
	for(i=0;i<MAX_HASH_PROXY_ENTRY_TAB_SIZE;i++)
	{
		DBGPRINT(RT_DEBUG_OFF, (" %d: %02d %02d %02d\n",i,pProxyTab->Content[i].Next,pProxyTab->Content[i].DLNext,pProxyTab->Content[i].DLPrev));
	}

	return TRUE;
}

INT Set_MeshEntryInfo_Display_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	DBGPRINT(RT_DEBUG_OFF, ("\n%-19s%-10s\n", "DESTMAC", "ROUTE_IDX"));
	
	MeshEntryTableGet(pAd);
	return TRUE;
}

INT Set_MeshInfo_Display_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT i;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	struct wifi_dev *wdev;
	if(!IS_MESH_IF(pObj))
		return FALSE;

	wdev = &pAd->MeshTab.wdev;
	DBGPRINT(RT_DEBUG_OFF, ("HostName = %s, Len=%d\n", pAd->MeshTab.HostName, strlen((RTMP_STRING *)pAd->MeshTab.HostName)));
	DBGPRINT(RT_DEBUG_OFF, ("Mesh Id = %s, Len=%d\n", pAd->MeshTab.MeshId, pAd->MeshTab.MeshIdLen));
	DBGPRINT(RT_DEBUG_OFF, ("Mesh AutoLink = %s\n", pAd->MeshTab.MeshAutoLink == TRUE ? "Enable" : "Disable"));
	DBGPRINT(RT_DEBUG_OFF, ("Channel Precedence (CPI) = %d\n", pAd->MeshTab.CPI));
	DBGPRINT(RT_DEBUG_OFF, ("mesh ctrl current state =%d\n", pAd->MeshTab.CtrlCurrentState));
	DBGPRINT(RT_DEBUG_OFF, ("Mesh AuthMode(%d)=%s, EncrypType(%d)=%s \n", 
							wdev->AuthMode, GetAuthMode(wdev->AuthMode), 
							wdev->WepStatus, GetEncryptType(wdev->WepStatus)));


	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		DBGPRINT(RT_DEBUG_OFF, ("mesh link (%d) current state =%d,", i, pAd->MeshTab.MeshLink[i].CurrentState));
		DBGPRINT(RT_DEBUG_OFF, (" Valid =%d,", pAd->MeshTab.MeshLink[i].Entry.Valid));
		DBGPRINT(RT_DEBUG_OFF, (" MatchWcid =%d,", pAd->MeshTab.MeshLink[i].Entry.MacTabMatchWCID));
		DBGPRINT(RT_DEBUG_OFF, (" LocalId =%x,", pAd->MeshTab.MeshLink[i].Entry.LocalLinkId));
		DBGPRINT(RT_DEBUG_OFF, (" PeerId =%x,", pAd->MeshTab.MeshLink[i].Entry.PeerLinkId));
		DBGPRINT(RT_DEBUG_OFF, (" ExtChOffset =%d,", pAd->MeshTab.MeshLink[i].Entry.ExtChOffset));
		DBGPRINT(RT_DEBUG_OFF, (" PeerMacAddr =%02x:%02x:%02x:%02x:%02x:%02x\n",
			PRINT_MAC(pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr)));
	}
	DBGPRINT(RT_DEBUG_OFF, ("\n\n"));

	DBGPRINT(RT_DEBUG_OFF, ("\n%-19s%-4s%-4s%-4s%-7s%-7s%-7s%-10s%-6s%-6s%-6s%-6s\n",
			"MAC", "IDX", "AID", "PSM", "RSSI0", "RSSI1", "RSSI2", "PhMd", "BW", "MCS", "SGI", "STBC"));
	
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
		if (IS_ENTRY_MESH(pEntry))
		{
			DBGPRINT(RT_DEBUG_OFF, ("%02X:%02X:%02X:%02X:%02X:%02X  ",
				pEntry->Addr[0], pEntry->Addr[1], pEntry->Addr[2],
				pEntry->Addr[3], pEntry->Addr[4], pEntry->Addr[5]));
			DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->func_tb_idx));
			DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->Aid));
			DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->PsMode));
			DBGPRINT(RT_DEBUG_OFF, ("%-7d", pEntry->RssiSample.AvgRssi[0]));
			DBGPRINT(RT_DEBUG_OFF, ("%-7d", pEntry->RssiSample.AvgRssi[1]));
			DBGPRINT(RT_DEBUG_OFF, ("%-7d", pEntry->RssiSample.AvgRssi[2]));
#ifdef DOT11_N_SUPPORT
			DBGPRINT(RT_DEBUG_OFF, ("%-10s", get_phymode_str(pEntry->HTPhyMode.field.MODE)));
			DBGPRINT(RT_DEBUG_OFF, ("%-6s", get_bw_str(pEntry->HTPhyMode.field.BW)));
			DBGPRINT(RT_DEBUG_OFF, ("%-6d", pEntry->HTPhyMode.field.MCS));
			DBGPRINT(RT_DEBUG_OFF, ("%-6d", pEntry->HTPhyMode.field.ShortGI));
			DBGPRINT(RT_DEBUG_OFF, ("%-6d\n", pEntry->HTPhyMode.field.STBC));
#endif /* DOT11_N_SUPPORT */
		}
	} 

	return TRUE;
}

INT Set_NeighborInfo_Display_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	int i;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	if (pAd->MeshTab.pMeshNeighborTab == NULL)
	{
		DBGPRINT(RT_DEBUG_OFF, ("Mesh Neighbor Tab not ready.\n"));
		return TRUE;
	}
		

	if (pAd->MeshTab.pMeshNeighborTab->NeighborNr == 0)
	{
		DBGPRINT(RT_DEBUG_OFF, ("Mesh Neighbor Tab empty.\n"));
		return TRUE;
	}

	DBGPRINT(RT_DEBUG_OFF, ("Neighbor MP Num = %d\n", pAd->MeshTab.pMeshNeighborTab->NeighborNr));
#if 0
	for (i = 0; i < MAX_NEIGHBOR_MP; i++ )
	{
		if (pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].Valid != TRUE)
			continue;

		DBGPRINT(RT_DEBUG_OFF, ("%d, HostName=%s,", i, pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].HostName));
		DBGPRINT(RT_DEBUG_OFF, (" Rssi=%d,", pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].RealRssi));
		DBGPRINT(RT_DEBUG_OFF, (" Ch=%d,", pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].Channel));
		DBGPRINT(RT_DEBUG_OFF, (" CPI=%d,", pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].CPI));
		DBGPRINT(RT_DEBUG_OFF, (" State=%d,", pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].State));
		DBGPRINT(RT_DEBUG_OFF, (" LinkId=%d,", pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].MeshLinkIdx));
		DBGPRINT(RT_DEBUG_OFF, (" MeshId=%s, len=%d", pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].MeshId,
			pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].MeshIdLen));
		DBGPRINT(RT_DEBUG_OFF, (" Mac=%02x:%02x:%02x:%02x:%02x:%02x,",
			pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].PeerMac[0], pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].PeerMac[1],
			pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].PeerMac[2], pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].PeerMac[3],
			pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].PeerMac[4], pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].PeerMac[5]));

		if (pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].MeshEncrypType == ENCRYPT_OPEN_WEP)
			DBGPRINT(RT_DEBUG_OFF, (" Encryption=OPEN-WEP\n"));
		else if (pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].MeshEncrypType == ENCRYPT_WPANONE_TKIP) 
			DBGPRINT(RT_DEBUG_OFF, (" Encryption=WPANONE-TKIP\n"));
		else if (pAd->MeshTab.pMeshNeighborTab->NeighborMP[i].MeshEncrypType == ENCRYPT_WPANONE_AES) 
			DBGPRINT(RT_DEBUG_OFF, (" Encryption=WPANONE-AES\n"));
		else
			DBGPRINT(RT_DEBUG_OFF, (" Encryption=OPEN-NONE\n"));
		
	}
#else
	DBGPRINT(RT_DEBUG_OFF, ("\n%-4s%-19s%-6s%-4s%-4s%-6s%-8s%-6s%-8s%-14s%-16s%-6s\n",
		"IDX", "MAC", "MRSI", "CH", "BW", "CHOF", "CPI", "STATE", "LINKID", "ECRP", "MESHID", "HSTN"));
	
	for (i=0; i<MAX_NEIGHBOR_MP; i++)
	{
		UCHAR	MeshEncrypType = ENCRYPT_OPEN_NONE;
		PMESH_NEIGHBOR_ENTRY pEntry = &pAd->MeshTab.pMeshNeighborTab->NeighborMP[i];
		if (pEntry->Valid)
		{
			DBGPRINT(RT_DEBUG_OFF, ("%-4d", i));
			DBGPRINT(RT_DEBUG_OFF, ("%02X:%02X:%02X:%02X:%02X:%02X  ",
				pEntry->PeerMac[0], pEntry->PeerMac[1], pEntry->PeerMac[2],
				pEntry->PeerMac[3], pEntry->PeerMac[4], pEntry->PeerMac[5]));
			DBGPRINT(RT_DEBUG_OFF, ("%-6d", (int)pEntry->RealRssi));
			DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->Channel));
			DBGPRINT(RT_DEBUG_OFF, ("%-4d", (int)pEntry->ChBW));
			DBGPRINT(RT_DEBUG_OFF, ("%-6d", (int)pEntry->ExtChOffset));
			DBGPRINT(RT_DEBUG_OFF, ("%-8d", (int)pEntry->CPI));
			DBGPRINT(RT_DEBUG_OFF, ("%-6d", (int)pEntry->State));
			DBGPRINT(RT_DEBUG_OFF, ("%-8x", (int)pEntry->MeshLinkIdx));
			MeshEncrypType = MeshCheckPeerMpCipher(pEntry->CapabilityInfo, pEntry->RSNIE, pEntry->RSNIE_Len);
			if (MeshEncrypType == ENCRYPT_OPEN_WEP)
			{
				DBGPRINT(RT_DEBUG_OFF, ("%-14s", "OPEN-WEP"));
			}
			else if (MeshEncrypType == ENCRYPT_WPANONE_TKIP) 
			{
				DBGPRINT(RT_DEBUG_OFF, ("%-14s", "WPANONE-TKIP"));
			}
			else if (MeshEncrypType == ENCRYPT_WPANONE_AES) 
			{
				DBGPRINT(RT_DEBUG_OFF, ("%-14s", "WPANONE-AES"));
			}
			else
			{
				DBGPRINT(RT_DEBUG_OFF, ("%-14s", "OPEN-NONE"));
			}
			DBGPRINT(RT_DEBUG_OFF, ("%-16s", pEntry->MeshId));
			DBGPRINT(RT_DEBUG_OFF, ("%s\n", pEntry->HostName));
		}
	} 
#endif

	return TRUE;
}

INT Set_MultipathInfo_Display_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT i;
	LONG HashId;
	PMESH_MULTIPATH_ENTRY pEntry;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		if (!PeerLinkValidCheck(pAd, i))
			continue;
		DBGPRINT(RT_DEBUG_OFF, ("Link(%d) ", i));
		for (HashId = 0; HashId < MULTIPATH_HASH_TAB_SIZE; HashId++)
		{
			pEntry = (PMESH_MULTIPATH_ENTRY)(pAd->MeshTab.MeshLink[i].Entry.MultiPathHash[HashId].pHead);
			if (pEntry == NULL)
				continue;

			DBGPRINT(RT_DEBUG_OFF, (" HashId(%ld):", HashId));
			while (pEntry)
			{
				DBGPRINT(RT_DEBUG_OFF, ("SA=%02x:%02x:%02x:%02x:%02x:%02x ",
					PRINT_MAC(pEntry->MeshSA)));
				pEntry = pEntry->pNext;
			}
		}
		DBGPRINT(RT_DEBUG_OFF, ("\n"));
	}

	return TRUE;
}

INT Set_MultiCastAgeOut_Display_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	DBGPRINT(RT_DEBUG_OFF, ("Multi Cast Age Timeout = %ld sec\n", (pAd->MeshTab.MeshMultiCastAgeOut) / 1000));
	
	return TRUE;
}

INT Set_MeshOnly_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT Enable;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	Enable = (UINT) simple_strtol(arg, 0, 16);

	pAd->MeshTab.MeshOnly= (Enable > 0) ? TRUE : FALSE;

	DBGPRINT(RT_DEBUG_TRACE, ("%s::(enable = %d)\n", __FUNCTION__, pAd->MeshTab.MeshOnly));
	
	return TRUE;
}

INT Set_PktSig_Display_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT i;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	PMESH_BMPKTSIG_TAB pTab = pAd->MeshTab.pBMPktSigTab;

	if(!IS_MESH_IF(pObj))
		return FALSE;

	DBGPRINT(RT_DEBUG_OFF, ("\n%-4s%-19s%-4s\n", "IDX", "MAC", "SEQ"));
	for (i=0; i<MAX_BMPKTSIG_TAB_SIZE; i++)
	{
		PMESH_BMPKTSIG_ENTRY pEntry = &(pTab->Content[i]);
		if (pTab->Content[i].Valid == FALSE)
			continue;

		DBGPRINT(RT_DEBUG_OFF, ("%-4d", i));
		DBGPRINT(RT_DEBUG_OFF, ("%02x:%02x:%02x:%02x:%02x:%02x  ",
				PRINT_MAC(pEntry->MeshSA)));
		DBGPRINT(RT_DEBUG_OFF, ("%-8x:%-9x%-9x%-9x%-9x",
			pEntry->MeshSeqBased, pEntry->Offset[0], pEntry->Offset[1],
			pEntry->Offset[2], pEntry->Offset[3]));
		DBGPRINT(RT_DEBUG_OFF, ("\n"));
	}

	return TRUE;
}

/* --------------------------------- Public -------------------------------- */
/*
========================================================================
Routine Description:
    Close Mesh network interface.

Arguments:
    ad_p            points to our adapter

Return Value:
    None

Note:
========================================================================
*/
VOID RTMP_Mesh_Close(
	IN PRTMP_ADAPTER pAd)
{
	struct wifi_dev *wdev;

	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s --->\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
	/* free Mesh Tables and allocate spin locks */
	NdisFreeSpinLock(&pAd->MeshTabLock);

	wdev = &pAd->MeshTab.wdev;
	/* close virtual interface. */
	if (wdev->if_dev)
		RtmpOSNetDevClose(wdev->if_dev);
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s <---\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */

}


/*
========================================================================
Routine Description:
    Remove Mesh network interface.

Arguments:
    ad_p            points to our adapter

Return Value:
    None

Note:
========================================================================
*/
VOID MESH_Remove(
	IN PRTMP_ADAPTER pAd)
{
	struct wifi_dev *wdev;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s --->\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */

#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_USB
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		BOOLEAN Cancelled = FALSE;
#ifndef BCN_OFFLOAD_SUPPORT
		RTMPCancelTimer(&pAd->CommonCfg.BeaconUpdateTimer, &Cancelled);
#endif
		RTUSBBssBeaconExit(pAd);
	}
#endif /* RTMP_MAC_USB */
#endif /* CONFIG_STA_SUPPORT */

	wdev = &pAd->MeshTab.wdev;
	/* remove virtual interface. */
	if (wdev->if_dev)
	{
		RtmpOSNetDevProtect(1);
		RtmpOSNetDevDetach(wdev->if_dev);
		RtmpOSNetDevProtect(0);

		rtmp_wdev_idx_unreg(pAd, wdev);
		RtmpOSNetDevFree(wdev->if_dev);
	}

	NeighborTableDestroy(pAd);
	BMPktSigTabExit(pAd);
	MultipathPoolExit(pAd);

	MeshRoutingTable_Exit(pAd);
	MeshEntryTable_Exit(pAd);
	MeshProxyEntryTable_Exit(pAd);
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s <---\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */

}


/* --------------------------------- Private -------------------------------- */

VOID MeshUp(
	IN PRTMP_ADAPTER pAd)
{
	BOOLEAN TxPreamble;
	struct wifi_dev *wdev = &pAd->MeshTab.wdev;

    DBGPRINT(RT_DEBUG_TRACE, ("%s: ===> \n", __FUNCTION__));
	if ( RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS) ||
		 RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))
		 /*||!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP)) */
		return;

	pAd->MeshTab.MeshChannel = pAd->CommonCfg.Channel;

	/* Make regular Beacon frame */
	MeshMakeBeacon(pAd, MESH_BEACON_IDX(pAd));

	/* Check if the security is supported */
	if (pAd->MeshTab.EasyMeshSecurity)
	{
#ifdef RELEASE_EXCLUDE
		/* 1. This mode can support OPEN, WEP, WPANONE and Mesh-PSK. */
		/* 2. These setting doesn't need MKD and MA. An MP can connect */
		/* 	  to the other MP directly. */
		/* 3. This is does't be defined in IEEE 802.11s standard. */
#endif /* RELEASE_EXCLUDE */
		if (wdev->AuthMode != Ndis802_11AuthModeOpen && 
			wdev->AuthMode != Ndis802_11AuthModeWPANone)
		{
			wdev->AuthMode = Ndis802_11AuthModeOpen;
			wdev->WepStatus = Ndis802_11WEPDisabled;
		}			
		pAd->MeshTab.OpMode &= ~(MESH_MKD);
		
		DBGPRINT(RT_DEBUG_TRACE, ("MeshUp: the Easy MSA is enabled. \n"));
	}
	else
	{
#ifdef RELEASE_EXCLUDE
		/* 1. It refer to the definition of IEEE 802.11s */
		/* 2. It ONLY supports Mesh-PSK and Mesh-802.1X authentication. */
		/* 3. An MP can't connect to the other MP directly. */
		/*	  It MUST bulid its security link with an MA or MKD. */
#endif /* RELEASE_EXCLUDE */
		if (wdev->AuthMode != Ndis802_11AuthModeWPA2 && 
			wdev->AuthMode != Ndis802_11AuthModeWPA2PSK)
		{
			wdev->AuthMode = Ndis802_11AuthModeWPA2;
			wdev->WepStatus = Ndis802_11AESEnable;
		}

		DBGPRINT(RT_DEBUG_TRACE, ("MeshUp: the Easy MSA is disabled. \n"));
	}

#ifdef RELEASE_EXCLUDE
	/* If the MP implements the MKD function, set value into the MKDD-ID field. */
	/* Otherwise, zero the MKDD-ID value. */
#endif /* RELEASE_EXCLUDE */
	if (pAd->MeshTab.OpMode & MESH_MKD)
	{
		NdisMoveMemory(pAd->MeshTab.LocalMSCIE.MKDDID, wdev->if_addr, MAC_ADDR_LEN);
		pAd->MeshTab.bInitialMsaDone = TRUE;
		pAd->MeshTab.bKeyholderDone  = TRUE;
		pAd->MeshTab.bConnectedToMKD = TRUE;
	}
	else
	{
		NdisZeroMemory(&pAd->MeshTab.LocalMSCIE, sizeof(MESH_SECURITY_CAPABILITY_IE));
		pAd->MeshTab.bInitialMsaDone = FALSE;
		pAd->MeshTab.bKeyholderDone  = FALSE;
		pAd->MeshTab.bConnectedToMKD = FALSE;
	}

#ifdef CONFIG_STA_SUPPORT
	/* set my current address as my BSSID */
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	AsicSetBssid(pAd, pAd->CurrentAddress, 0x0);
#endif /* CONFIG_STA_SUPPORT */

	/* Init PMKID */
	pAd->MeshTab.PMKID_Len = 0;
	NdisZeroMemory(pAd->MeshTab.PMKID, LEN_PMKID);

	TxPreamble = (pAd->CommonCfg.TxPreamble == Rt802_11PreambleLong ? 0 : 1);

	pAd->MeshTab.CapabilityInfo =
			CAP_GENERATE(0, 0, (wdev->WepStatus != Ndis802_11EncryptionDisabled), TxPreamble, pAd->CommonCfg.bUseShortSlotTime, 0);

	if (wdev->AuthMode == Ndis802_11AuthModeWPANone)
	{
		wdev->DefaultKeyId = 0;	/* always be zero */
		
        NdisZeroMemory(&pAd->MeshTab.SharedKey, sizeof(CIPHER_KEY));  
		pAd->MeshTab.SharedKey.KeyLen = LEN_TK;

		NdisMoveMemory(pAd->MeshTab.SharedKey.Key, pAd->MeshTab.PMK, LEN_TK);
            
        if (wdev->WepStatus == Ndis802_11TKIPEnable)
        {
    		NdisMoveMemory(pAd->MeshTab.SharedKey.RxMic, &pAd->MeshTab.PMK[16], LEN_TKIP_MIC);
    		NdisMoveMemory(pAd->MeshTab.SharedKey.TxMic, &pAd->MeshTab.PMK[16], LEN_TKIP_MIC);
        }

		/* Decide its ChiperAlg */
		if (wdev->WepStatus == Ndis802_11TKIPEnable)
			pAd->MeshTab.SharedKey.CipherAlg = CIPHER_TKIP;
		else if (wdev->WepStatus == Ndis802_11AESEnable)
			pAd->MeshTab.SharedKey.CipherAlg = CIPHER_AES;
		else
        {         
            DBGPRINT(RT_DEBUG_WARN, ("Unknow Cipher (=%d), set Cipher to AES\n", wdev->WepStatus));
			pAd->MeshTab.SharedKey.CipherAlg = CIPHER_AES;
        } 
	}

	DBGPRINT(RT_DEBUG_TRACE, ("!!! %s - AuthMode(%d)=%s, WepStatus(%d)=%s !!!\n", 
									__FUNCTION__,
									wdev->AuthMode, GetAuthMode(wdev->AuthMode),
									wdev->WepStatus, GetEncryptType(wdev->WepStatus)));

	MlmeEnqueue(pAd, MESH_CTRL_STATE_MACHINE, MESH_CTRL_JOIN, 0, NULL, 0);
	DBGPRINT(RT_DEBUG_TRACE, ("%s: <=== \n", __FUNCTION__));
}

BOOLEAN MeshLinkCheck(
	IN VOID *pAdSrc)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;
	BOOLEAN WaitMeshClose = FALSE;
	INT idx;


	for (idx = 0; idx < MAX_MESH_LINKS; idx++)
	{
		if (PeerLinkMngRuning(pAd, idx) || PeerLinkValidCheck(pAd, idx))
			WaitMeshClose = TRUE;
	}
	return WaitMeshClose;
}

VOID MeshDown(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN WaitFlag)
{
	INT idx;
#if 0 /* os abl move */
	ULONG WaitCnt;
#endif /* 0 */

	/* clear PMKID */
	pAd->MeshTab.PMKID_Len = 0;
	NdisZeroMemory(pAd->MeshTab.PMKID, LEN_PMKID);

	/* clear these flag */
	pAd->MeshTab.bInitialMsaDone = FALSE;	
	pAd->MeshTab.bKeyholderDone  = FALSE;
	pAd->MeshTab.bConnectedToMKD = FALSE;

#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_USB
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		RTUSBBssBeaconStop(pAd);
	}
#endif /* RTMP_MAC_USB */
#endif /* CONFIG_STA_SUPPORT */

	MlmeEnqueue(pAd, MESH_CTRL_STATE_MACHINE, MESH_CTRL_DISCONNECT, 0, NULL, 0);
	RTMP_MLME_HANDLER(pAd);

#if 0 /* os abl move */
	WaitCnt = 0;
	do
	{
		BOOLEAN WaitMeshClose;
		wait_queue_head_t wait;

		if (WaitFlag == FALSE)
			break;

		WaitMeshClose = FALSE;
		init_waitqueue_head(&wait);

		for (idx = 0; idx < MAX_MESH_LINKS; idx++)
		{
			if (PeerLinkMngRuning(pAd, idx) || PeerLinkValidCheck(pAd, idx))
				WaitMeshClose = TRUE;
		}

		if(WaitMeshClose == TRUE)
			wait_event_interruptible_timeout(wait, 0, 10 * OS_HZ/1000);
		else
			break;
	} while (WaitCnt++ < 1000);
#endif /* 0 */

	RtmpMeshDown(pAd, WaitFlag, MeshLinkCheck);

	/* delete all mesh links. */
	for (idx = 0; idx < MAX_MESH_LINKS; idx++)
	{
		if (pAd->MeshTab.MeshLink[idx].Entry.Valid)
			MeshLinkDelete(pAd, pAd->MeshTab.MeshLink[idx].Entry.PeerMacAddr, idx);
	}

	/* when the ra interface is down, do not send its beacon frame */
	MeshCleanBeaconFrame(pAd, MESH_BEACON_IDX(pAd));

#ifdef CONFIG_STA_SUPPORT
	/* resume BSSID for infra mode */
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	AsicSetBssid(pAd, pAd->CommonCfg.Bssid);
#endif /* CONFIG_STA_SUPPORT */
}

VOID MeshHalt(
	IN PRTMP_ADAPTER pAd)
{
	int idx;
	BOOLEAN 	  Cancelled;

	RTMPCancelTimer(&pAd->MeshTab.PldTimer, &Cancelled);
	RTMPCancelTimer(&pAd->MeshTab.McsTimer, &Cancelled);
	for (idx = 0; idx < MAX_MESH_LINKS; idx++)
	{
		RTMPCancelTimer(&pAd->MeshTab.MeshLink[idx].TOR, &Cancelled);
		RTMPCancelTimer(&pAd->MeshTab.MeshLink[idx].TOC, &Cancelled);
		RTMPCancelTimer(&pAd->MeshTab.MeshLink[idx].TOH, &Cancelled);
	}
}

VOID TearDownAllMeshLink(
		IN PRTMP_ADAPTER pAd)
{
	INT i;
	PMESH_NEIGHBOR_ENTRY pNeighbor = NULL;
	DBGPRINT(RT_DEBUG_TRACE, ("%s: tear down all Mesh Link.\n", __FUNCTION__));

	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
			if(PeerLinkValidCheck(pAd, i))
			{
				/*
				SendMeshPeerLinkClose(pAd, pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr,
				pAd->MeshTab.MeshLink[i].Entry.LocalLinkId,
				pAd->MeshTab.MeshLink[i].Entry.PeerLinkId,
				MESH_LINK_CANCELLED);
				*/
				/*send link close message */
				{
					PUCHAR pPeerMac=pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr;
					UINT32 LocalLinkId=pAd->MeshTab.MeshLink[i].Entry.LocalLinkId;
					UINT32 PeerLinkId=pAd->MeshTab.MeshLink[i].Entry.PeerLinkId;
					HEADER_802_11 MeshHdr;
					PUCHAR pOutBuffer = NULL;
					NDIS_STATUS NStatus;
					ULONG FrameLen;
					MESH_FLAG MeshFlag;
					UINT32 MeshSeq = INC_MESH_SEQ(pAd->MeshTab.MeshSeq);

					NStatus = MlmeAllocateMemory(pAd, (PVOID)&pOutBuffer);  /*Get an unused nonpaged memory */
					if(NStatus != NDIS_STATUS_SUCCESS)
					{
						DBGPRINT(RT_DEBUG_TRACE, ("%s() allocate memory failed \n", __FUNCTION__));
						return;
					}

					MeshHeaderInit(pAd, &MeshHdr,
						pPeerMac,		/* addr1 */
						pAd->MeshTab.wdev.if_addr,							/* addr2 */
						ZERO_MAC_ADDR);		/* addr3 */
					NdisMoveMemory(pOutBuffer, (PCHAR)&MeshHdr, sizeof(HEADER_802_11));
					FrameLen = sizeof(HEADER_802_11);

					/* Mesh Header */
					MeshFlag.word = 0;
					MeshFlag.field.AE = 0;	/* Peer-Link manager frame never carry 6 addresses. */
					InsertMeshHeader(pAd, (pOutBuffer + FrameLen), &FrameLen, MeshFlag.word,
						pAd->MeshTab.TTL, MeshSeq, NULL, NULL, NULL);

					/* Action field */
					InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_PEER_LINK, ACT_CODE_PEER_LINK_CLOSE);

					/* Reason code */
					InsertReasonCode(pAd, (pOutBuffer + FrameLen), &FrameLen, MESH_LINK_CANCELLED);

					/* Mesh Peer Link Management IE */
					InsertMeshPeerLinkMngIE(pAd, (pOutBuffer + FrameLen), &FrameLen, SUBTYPE_PEER_LINK_CLOSE,
						LocalLinkId, PeerLinkId, MESH_LINK_CANCELLED);

					MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
					MlmeFreeMemory(pAd, pOutBuffer);

					DBGPRINT(RT_DEBUG_TRACE, ("%s: LocalLinkId=%x, PeerLinkId=%x, Reason=%d\n",
						__FUNCTION__, LocalLinkId, PeerLinkId, MESH_LINK_CANCELLED));

				}

			
				pNeighbor = NeighborSearch(pAd, pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr);
				if (pNeighbor)
				{
					pNeighbor->State = NEIGHBOR_MP;
					pNeighbor->MeshLinkIdx = 0;
				}

				if (pAd->MeshTab.LinkSize == 0)
				{
					pAd->MeshTab.bInitialMsaDone = FALSE;
				}

				MultipathListDelete(pAd, i);


				if(MeshTableLookup(pAd, pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr, TRUE))
					MacTableDeleteMeshEntry(pAd, pAd->MeshTab.MeshLink[i].Entry.MacTabMatchWCID,
						pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr);


				if (pAd->MeshTab.MeshLink[i].Entry.Valid)
				{	
					MeshLinkDelete(pAd, pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr, i);
				}
			}
	}
}

BOOLEAN MeshAcceptPeerLink(
	IN PRTMP_ADAPTER pAd)
{
	return (pAd->MeshTab.LinkSize < MAX_MESH_LINKS) ? 1 : 0;
}

#ifdef RELEASE_EXCLUDE
/*
================================================================
Description : because Mesh and CLI share the same WCID table in ASIC. 
Mesh entry also insert to pAd->MacTab.content[].
Also fills the pairwise key.
Because front MAX_AID_BA entries have direct mapping to BAEntry, which is only used as CLI. So we insert Mesh
from index MAX_AID_BA.
================================================================
*/
#endif /* RELEASE_EXCLUDE */
MAC_TABLE_ENTRY *MacTableInsertMeshEntry(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PUCHAR pAddr,
	IN  UINT MeshLinkIdx)
{
	PMAC_TABLE_ENTRY pEntry = NULL;
	struct wifi_dev *wdev;

	/* if FULL, return */
	if (pAd->MacTab.Size >= MAX_LEN_OF_MAC_TABLE)
		return NULL;

	wdev = &pAd->MeshTab.wdev;
	do
	{
		if((pEntry = MeshTableLookup(pAd, pAddr, TRUE)) != NULL)
			break;

		/* allocate one MAC entry */
		pEntry = MacTableInsertEntry(pAd, pAddr, wdev, ENTRY_MESH, OPMODE_AP, TRUE);
		if (pEntry)
		{
			pAd->MeshTab.MeshLink[MeshLinkIdx].Entry.MacTabMatchWCID = pEntry->wcid;
			pEntry->func_tb_idx = MeshLinkIdx;

			pEntry->MaxHTPhyMode.word = wdev->MaxHTPhyMode.word;
			pEntry->MinHTPhyMode.word = wdev->MinHTPhyMode.word;
			pEntry->HTPhyMode.word = wdev->HTPhyMode.word;
			
#ifdef DOT11_N_SUPPORT
			if (wdev->HTPhyMode.field.MODE >= MODE_HTMIX)
			{
				if (wdev->DesiredTransmitSetting.field.MCS != MCS_AUTO)
				{
					DBGPRINT(RT_DEBUG_TRACE, ("IF-mesh0 : Desired MCS = %d\n",
						wdev->DesiredTransmitSetting.field.MCS));
					
					if (wdev->DesiredTransmitSetting.field.MCS == 32)
					{
						/* Fix MCS as HT Duplicated Mode */
						pEntry->MaxHTPhyMode.field.BW = 1;
						pEntry->MaxHTPhyMode.field.MODE = MODE_HTMIX;
						pEntry->MaxHTPhyMode.field.STBC = 0;
						pEntry->MaxHTPhyMode.field.ShortGI = 0;
						pEntry->MaxHTPhyMode.field.MCS = 32;
					}
					else if (pEntry->MaxHTPhyMode.field.MCS > wdev->HTPhyMode.field.MCS)
					{
						/* STA supports fixed MCS */
						pEntry->MaxHTPhyMode.field.MCS = wdev->HTPhyMode.field.MCS;
					}
				}
			}
#endif /* DOT11_N_SUPPORT */
			
			DBGPRINT(RT_DEBUG_TRACE, ("MacTableInsertMeshEntry - allocate entry #%d, Total= %d\n",pEntry->wcid, pAd->MacTab.Size));
			break;
		}
	} while(FALSE);

	return pEntry;
}


/*
	==========================================================================
	Description:
		Delete all Mesh Entry in pAd->MacTab
	==========================================================================
 */
BOOLEAN MacTableDeleteMeshEntry(
	IN PRTMP_ADAPTER pAd,
	IN USHORT wcid,
	IN PUCHAR pAddr)
{
	if (!VALID_WCID(wcid))
		return FALSE;

	MacTableDeleteEntry(pAd, wcid, pAddr);
	MeshCreatePerrAction(pAd, pAddr);

	return TRUE;
}

MAC_TABLE_ENTRY *MeshTableLookup(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pAddr,
	IN BOOLEAN bResetIdelCount)
{
	ULONG HashIdx;
	MAC_TABLE_ENTRY *pEntry = NULL;

	RTMP_SEM_LOCK(&pAd->MacTabLock);
	HashIdx = MAC_ADDR_HASH_INDEX(pAddr);
	pEntry = pAd->MacTab.Hash[HashIdx];

	while (pEntry)
	{
		if (IS_ENTRY_MESH(pEntry)
			&& MAC_ADDR_EQUAL(pEntry->Addr, pAddr))
		{
			if(bResetIdelCount) {
				pEntry->NoDataIdleCount = 0;
				// TODO: shiang-usw,  remove upper setting becasue we need to migrate to tr_entry!
				pAd->MacTab.tr_entry[pEntry->wcid].NoDataIdleCount = 0;
			}
			break;
		}
		else
			pEntry = pEntry->pNext;
	}

	RTMP_SEM_UNLOCK(&pAd->MacTabLock);
	return pEntry;
}

MAC_TABLE_ENTRY *MeshTableLookupByWcid(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR wcid,
	IN PUCHAR pAddr,
	IN BOOLEAN bResetIdelCount)
{
	ULONG MeshIndex;
	PMAC_TABLE_ENTRY pCurEntry = NULL;
	PMAC_TABLE_ENTRY pEntry = NULL;

	if (!VALID_WCID(wcid))
		return NULL;

	RTMP_SEM_LOCK(&pAd->MeshTabLock);
	RTMP_SEM_LOCK(&pAd->MacTabLock);

	do
	{
		pCurEntry = &pAd->MacTab.Content[wcid];

		MeshIndex = 0xff;
		if ((pCurEntry) && IS_ENTRY_MESH(pCurEntry))
		{
			MeshIndex = pCurEntry->wdev_idx;
		}

		if (MeshIndex == 0xff)
			break;

		if (pAd->MeshTab.MeshLink[MeshIndex].Entry.Valid != TRUE)
			break;

		if (MAC_ADDR_EQUAL(pCurEntry->Addr, pAddr))
		{
			if(bResetIdelCount) {
				pCurEntry->NoDataIdleCount = 0;
				// TODO: shiang-usw,  remove upper setting becasue we need to migrate to tr_entry!
				pAd->MacTab.tr_entry[pCurEntry->wcid].NoDataIdleCount = 0;
			}
			pEntry = pCurEntry;
			break;
		}
	} while(FALSE);

	RTMP_SEM_UNLOCK(&pAd->MacTabLock);
	RTMP_SEM_UNLOCK(&pAd->MeshTabLock);

	return pEntry;
}

MAC_TABLE_ENTRY *FindMeshEntry(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR 			Wcid,
	IN PUCHAR			pAddr)
{
	MAC_TABLE_ENTRY *pEntry;

	/* lookup the match wds entry for the incoming packet. */
	pEntry = MeshTableLookupByWcid(pAd, Wcid, pAddr, TRUE);

	return pEntry;
}


VOID MlmeHandleRxMeshFrame(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	int i;
	HEADER_802_11 *pHeader = pRxBlk->pHeader;
	UCHAR Category;
	UCHAR ActionField;
	UCHAR MeshHdrLen;

	if (!MESH_ON(pAd))
		return;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> CmmRxMeshMgmtFrameHandle\n"));
#endif /* RELEASE_EXCLUDE */
	

#ifdef WDS_SUPPORT
	if (WdsTableLookup(pAd, pHeader->Addr2, FALSE) != NULL)
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("%s() Peer WDS entry.\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
		
		return;
	}
#endif /* WDS_SUPPORT */


	pRxBlk->pData = (UCHAR *)pHeader;
	pRxBlk->pData += LENGTH_802_11;
	pRxBlk->DataSize -= LENGTH_802_11;

	MeshHdrLen = GetMeshHederLen(pRxBlk->pData);
	/* get Category */
	NdisMoveMemory(&Category, pRxBlk->pData + MeshHdrLen, 1);
	/* get ActionField */
	NdisMoveMemory(&ActionField, pRxBlk->pData + MeshHdrLen + 1, 1);

	for (i = 0;  i < MESH_ACT_HANDLER_TAB_SIZE; i++)
	{
		if ((Category == MeshActHandler[i].Category)
			&& (ActionField == MeshActHandler[i].ActionCode)
			&& (MeshActHandler[i].pHandle))
		{
			(*(MeshActHandler[i].pHandle))(pAd, pRxBlk);
			break;
		}
	}
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- CmmRxMeshMgmtFrameHandle\n"));
#endif /* RELEASE_EXCLUDE */

}

LONG
PathRouteIDSearch(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR	pAddr)
{
	MESH_ENTRY *pEntry = NULL;
	UCHAR	DestAddr[MAC_ADDR_LEN];

	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("-----> PathRouteIDSearch\n"));
#endif /* RELEASE_EXCLUDE */
	if (*pAddr & 0x01) /* B/Mcast packet. */
		return BMCAST_ROUTE_ID;

	COPY_MAC_ADDR(DestAddr, pAddr);
	pEntry = MeshEntryTableLookUp(pAd, DestAddr);

	if (pEntry)
	{
		if (pEntry->PathReqTimerRunning)
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("%s ---> PathReqTimerRunning is TRUE\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
			
			return -1;
		}
		else
			return (ULONG)pEntry->Idx;
	}

	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<----- PathRouteIDSearch\n"));
#endif /* RELEASE_EXCLUDE */
	return -1;
}

PUCHAR
PathRouteAddrSearch(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR	RouteIdx)
{
	MESH_ROUTING_ENTRY *pEntry = NULL;
	PMESH_ROUTING_TABLE	pRouteTab = pAd->MeshTab.pMeshRouteTab;

	if (pRouteTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Route Table doesn't exist.\n", __FUNCTION__));
		return NULL;
	}

	if (RouteIdx >= MAX_ROUTE_TAB_SIZE)
		return NULL;

	if (pRouteTab->Content[RouteIdx].Valid == TRUE)
	{
		pEntry = &pRouteTab->Content[RouteIdx];
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Can't find the Route Index = (%d)\n", (int)RouteIdx));
		return NULL;
	}

	return pEntry->MeshDA;
}

INT
PathMeshLinkIDSearch(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR	RouteIdx)
{
	MESH_ROUTING_ENTRY *pEntry = NULL;
	PMESH_ROUTING_TABLE	pRouteTab = pAd->MeshTab.pMeshRouteTab;

	if (pRouteTab == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh Route Table doesn't exist.\n", __FUNCTION__));
		return -1;
	}

	if (RouteIdx >= MAX_ROUTE_TAB_SIZE)
		return -1;

	if (pRouteTab->Content[RouteIdx].Valid == TRUE)
	{
		pEntry = &pRouteTab->Content[RouteIdx];
		return pEntry->NextHopLinkID;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Can't find the Route Index = (%d)\n", (int)RouteIdx));
	}

	return -1;
}

UINT GetMeshHederLen(
	IN PUCHAR pSrcBufVA)
{
	UINT MeshHdrLen = 0;

	switch(GetMeshFlagAE(pSrcBufVA))
	{
		case 0:
			MeshHdrLen = 6;
			break;
		case 1:
			MeshHdrLen = 12;
			break;
		case 2:
			MeshHdrLen = 18;
			break;
		case 3:
			MeshHdrLen = 24;
			break;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Undown Mesh AE type=%x\n", __FUNCTION__, GetMeshFlagAE(pSrcBufVA)));
			break;
	}


	return MeshHdrLen; 
}

UINT8 GetMeshFlag(
	IN PUCHAR pSrcBufVA)
{
	PMESH_HEADER pMeshHead;

	pMeshHead = (PMESH_HEADER)(pSrcBufVA);

	return pMeshHead->MeshFlag; 
}

UINT8 GetMeshFlagAE(
	IN PUCHAR pSrcBufVA)
{
	PMESH_HEADER pMeshHead;

	pMeshHead = (PMESH_HEADER)(pSrcBufVA);

	return (UINT8)((PMESH_FLAG)&pMeshHead->MeshFlag)->field.AE;
}

UINT8 GetMeshTTL(
	IN PUCHAR pSrcBufVA)
{
	PMESH_HEADER pMeshHead;

	pMeshHead = (PMESH_HEADER)(pSrcBufVA);

	return pMeshHead->MeshTTL; 
}

UINT32 GetMeshSeq(
	IN PUCHAR pSrcBufVA)
{
	PMESH_HEADER pMeshHead;
	UINT32 MeshSeq;

	pMeshHead = (PMESH_HEADER)(pSrcBufVA);
	MeshSeq = le2cpu32(pMeshHead->MeshSeq);

	return MeshSeq;
}

PUCHAR GetMeshAddr4(
	IN PUCHAR pSrcBufVA)
{
	PMESH_HEADER pMeshHead;

	pMeshHead = (PMESH_HEADER)(pSrcBufVA);
	if (((PMESH_FLAG)&pMeshHead->MeshFlag)->field.AE == 1
		|| ((PMESH_FLAG)&pMeshHead->MeshFlag)->field.AE == 3)
		return (PUCHAR)(pSrcBufVA + 6);
	else
		return NULL; 
}

PUCHAR GetMeshAddr5(
	IN PUCHAR pSrcBufVA)
{
	PMESH_HEADER pMeshHead;

	pMeshHead = (PMESH_HEADER)(pSrcBufVA);
	if (((PMESH_FLAG)&pMeshHead->MeshFlag)->field.AE == 2)
		return (PUCHAR)(pSrcBufVA + 6);
	else if (((PMESH_FLAG)&pMeshHead->MeshFlag)->field.AE == 3)
		return (PUCHAR)(pSrcBufVA + 6 + MAC_ADDR_LEN);
	else
		return NULL; 
}

PUCHAR GetMeshAddr6(
	IN PUCHAR pSrcBufVA)
{
	PMESH_HEADER pMeshHead;

	pMeshHead = (PMESH_HEADER)(pSrcBufVA);
	if (((PMESH_FLAG)&pMeshHead->MeshFlag)->field.AE == 2)
		return (PUCHAR)(pSrcBufVA + 6 + MAC_ADDR_LEN);
	else if (((PMESH_FLAG)&pMeshHead->MeshFlag)->field.AE == 3)
		return (PUCHAR)(pSrcBufVA + 6 + MAC_ADDR_LEN + MAC_ADDR_LEN);
	else
		return NULL; 
}

ULONG GetMeshLinkId(
	IN PRTMP_ADAPTER pAd,
	IN PCHAR PeerMacAddr)
{
	ULONG i;

	for (i = 0; i < MAX_MESH_LINKS; i++)
	{
		if (MAC_ADDR_EQUAL(PeerMacAddr, pAd->MeshTab.MeshLink[i].Entry.PeerMacAddr))
			break;
	}

	if (i == MAX_MESH_LINKS)
		return BSS_NOT_FOUND;

	return (ULONG)i;
}

VOID MeshDataPktProcess(
	IN PRTMP_ADAPTER pAd,
	IN PNDIS_PACKET pPacket,
	IN USHORT MeshLinkIdx,
	OUT PNDIS_PACKET *pMeshForwardPacket,
	OUT BOOLEAN *pbDirectForward,
	OUT BOOLEAN *pbAnnounce)

{
	PUCHAR pHeader802_3 = GET_OS_PKT_DATAPTR(pPacket);
	PUCHAR pMeshHdr = pHeader802_3 + LENGTH_802_3;
	UINT MeshHdrLen = GetMeshHederLen(pMeshHdr);
	UINT8 MeshTTL = GetMeshTTL(pMeshHdr);
	UINT8 MeshFlagAE = GetMeshFlagAE(pMeshHdr);
	INT PktLen;

	do
	{
		*pbAnnounce = FALSE;
		*pbDirectForward = FALSE;

		if (!MESH_ON(pAd))
			break;

		if (*pHeader802_3 & 0x01)
		{
			if (--MeshTTL > 0)
				*pbDirectForward = TRUE;
			else
				*pbDirectForward = FALSE;
			*pbAnnounce = TRUE;
		}
		else
		{
			if (MAC_ADDR_EQUAL(pHeader802_3, pAd->MeshTab.wdev.if_addr))
			{
				*pbAnnounce = TRUE;
				*pbDirectForward = FALSE;

#ifdef RELEASE_EXCLUDE
				/*if we r not MAP or MP with proxy , we will drop all packets which MeshDa is us and Da is not. */
				/*if MP is bridged , it must enable proxy or layer 3 packets will not be forwarded correctly */
				/*because bridge mac will not be the same as MP wireless card mac. */
#endif /* RELEASE_EXCLUDE */
				if ((!(pAd->MeshTab.OpMode & MESH_AP) && !(pAd->MeshTab.OpMode & MESH_POTAL)) &&
				     (MeshFlagAE == 2 && !MAC_ADDR_EQUAL(GetMeshAddr5(pMeshHdr),pAd->MeshTab.wdev.if_addr)))
					*pbAnnounce = FALSE;
			}
			else
			{
				if (--MeshTTL > 0)
					*pbDirectForward = TRUE;
				else
					*pbDirectForward = FALSE;
				*pbAnnounce = FALSE;
			}
		}

		if (pAd->MeshTab.MeshCapability.field.Forwarding == 0)
			*pbDirectForward = FALSE;

		if (*pbDirectForward == TRUE)
		{
			PUCHAR pFwdPktHeader = NULL;
			if (*pbAnnounce == TRUE)
			{
				*pMeshForwardPacket = (PNDIS_PACKET)OS_PKT_COPY(RTPKT_TO_OSPKT(pPacket));
				if (*pMeshForwardPacket == NULL)
				{
					/*ASSERT(*pMeshForwardPacket); */
					*pbAnnounce = FALSE;
					*pbDirectForward = FALSE;
					break;
				}
				pFwdPktHeader = GET_OS_PKT_DATAPTR(*pMeshForwardPacket);
			}
			else
				pFwdPktHeader = pHeader802_3;
			
#ifdef RELEASE_EXCLUDE
			/* override eth type/len field. */
			/* ApHardTransmit will check the field to decide that to add LLC or not. */
			/* Thus override eth type/lens field here let ApHardTransmit added LLC for the packet. */
#endif /* RELEASE_EXCLUDE */

			if (NdisEqualMemory(SNAP_802_1H, pFwdPktHeader + LENGTH_802_3 + MeshHdrLen, 6)  || 
				NdisEqualMemory(SNAP_BRIDGE_TUNNEL, pFwdPktHeader + LENGTH_802_3 + MeshHdrLen, 6))   
			{
				NdisMoveMemory(pFwdPktHeader + 12, pFwdPktHeader + LENGTH_802_3 + MeshHdrLen + 6, 2);
			}
			else
			{	
				PktLen=GET_OS_PKT_LEN(pPacket)-LENGTH_802_3;
				*(pFwdPktHeader+12)=(UCHAR)(PktLen / 256);
				*(pFwdPktHeader+13)=(UCHAR)(PktLen % 256);
			}			
		}

		if (*pbAnnounce == TRUE)
		{
			PUCHAR pSrcBuf,pData=pMeshHdr + MeshHdrLen;	
			UINT Offset;
			BOOLEAN WithLLC=TRUE;
			UCHAR Header802_3[14];
	

			if (NdisEqualMemory(SNAP_802_1H, pData, 6)  || NdisEqualMemory(SNAP_BRIDGE_TUNNEL, pData, 6))                 
			{  
				pSrcBuf = pMeshHdr + MeshHdrLen + 6 - LENGTH_802_3_NO_TYPE; 	
			}
			else
			{
				pSrcBuf = pMeshHdr + MeshHdrLen - LENGTH_802_3;
				WithLLC=FALSE;
			}


			if (MeshFlagAE == 2)
			{	/* the lenght of hdr shall be 16 bytes here. */
				COPY_MAC_ADDR(Header802_3,GetMeshAddr5(pMeshHdr));
				COPY_MAC_ADDR(Header802_3+MAC_ADDR_LEN,GetMeshAddr6(pMeshHdr));
			}
			else if(MeshFlagAE == 1)
			{
				/* Mesh Data frame never AE=1. */
				/* drop the frame. */
				DBGPRINT(RT_DEBUG_ERROR, ("%s: Receive Mesh-Data frame carry AE=1. Drop the frame.\n", __FUNCTION__));
				*pbAnnounce = FALSE;
				*pbDirectForward = FALSE;
				if (*pMeshForwardPacket != NULL)
					RELEASE_NDIS_PACKET(pAd, *pMeshForwardPacket, NDIS_STATUS_FAILURE);
				*pMeshForwardPacket = NULL;
				break;
			}
			else
			{
				COPY_MAC_ADDR(Header802_3,pHeader802_3);
				COPY_MAC_ADDR(Header802_3+MAC_ADDR_LEN,pHeader802_3 + MAC_ADDR_LEN);				
			}

			Offset = pSrcBuf - pHeader802_3;
/*			GET_OS_PKT_DATAPTR(pPacket) = pSrcBuf; */
/*			GET_OS_PKT_LEN(pPacket) -= Offset; */
			SET_OS_PKT_DATAPTR(pPacket, pSrcBuf);
			SET_OS_PKT_LEN(pPacket, (GET_OS_PKT_LEN(pPacket) - Offset));

			if (WithLLC==FALSE)
			{
				PktLen=GET_OS_PKT_LEN(pPacket)-14;
				Header802_3[12]=(UCHAR)(PktLen / 256); 
				Header802_3[13]=(UCHAR)(PktLen % 256);
				memcpy(pSrcBuf,Header802_3,14);
			}
			else
			{
				memcpy(pSrcBuf,Header802_3,12);
			}
			

		}
	} while (FALSE);

	return;
}

ULONG InsertPktMeshHeader(
	IN PRTMP_ADAPTER pAd,
	IN TX_BLK *pTxBlk, 
	IN PUCHAR *pHeaderBufPtr)
{
	ULONG TempLen = 0;
	MESH_FLAG MeshFlag;
	UINT16 MeshTTL;
	UINT32 MeshSeq;
	PUCHAR pMeshAddr5 = NULL;
	PUCHAR pMeshAddr6 = NULL;

	PerpareMeshHeader(pAd, pTxBlk, &MeshFlag, &MeshTTL, &MeshSeq, &pMeshAddr5, &pMeshAddr6);
	InsertMeshHeader(pAd, *pHeaderBufPtr, &TempLen, MeshFlag.word, MeshTTL, MeshSeq,
						NULL, pMeshAddr5, pMeshAddr6);

	*pHeaderBufPtr += TempLen;
	pTxBlk->MpduHeaderLen += TempLen;

	return TempLen;
}

UINT32 RandomMeshCPI(
	IN PRTMP_ADAPTER pAd)
{
	return (UINT32)((RandomByte(pAd) << 8) + (RandomByte(pAd)));
}

UINT16 RandomLinkId(
	IN PRTMP_ADAPTER pAd)
{
	return (UINT16)((RandomByte(pAd) << 8) + (RandomByte(pAd)));
}

UINT8 RandomChSwWaitTime(
	IN PRTMP_ADAPTER pAd)
{
	UINT8 ChSwCnt = RandomByte(pAd);

	ChSwCnt = (ChSwCnt >= 50) ? ChSwCnt : (UINT8)(50 + RandomByte(pAd));
	return ChSwCnt;
}

void rtmp_read_mesh_from_file(
	IN PRTMP_ADAPTER pAd,
	RTMP_STRING *tmpbuf,
	RTMP_STRING *buffer)
{
	/*UCHAR		keyMaterial[40]; */
	ULONG		KeyIdx;
	ULONG		KeyLen;
	struct wifi_dev *wdev;

	/*MeshId */
	wdev = &pAd->MeshTab.wdev;
	if(RTMPGetKeyParameter("MeshId", tmpbuf, 255, buffer, FALSE))
	{
		/*MeshId acceptable strlen must be less than 32 and bigger than 0. */
		if((strlen(tmpbuf) < 0) || (strlen(tmpbuf) > 32))
			pAd->MeshTab.MeshIdLen = 0;
		else
			pAd->MeshTab.MeshIdLen = strlen(tmpbuf);

		if(pAd->MeshTab.MeshIdLen > 0)
		{
			NdisMoveMemory(&pAd->MeshTab.MeshId, tmpbuf, pAd->MeshTab.MeshIdLen);
		}
		else
		{
			NdisZeroMemory(&pAd->MeshTab.MeshId, MAX_MESH_ID_LEN);
		}
		DBGPRINT(RT_DEBUG_TRACE, ("MeshIdLen=%d, MeshId=%s\n", pAd->MeshTab.MeshIdLen, pAd->MeshTab.MeshId));
	}

	/*MeshAutoLink */
	if (RTMPGetKeyParameter("MeshAutoLink", tmpbuf, 255, buffer, TRUE))
	{
		LONG Enable;
		Enable = simple_strtol(tmpbuf, 0, 10);
		pAd->MeshTab.MeshAutoLink = (Enable > 0) ? TRUE : FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("%s::(MeshAutoLink=%d)\n", __FUNCTION__, pAd->MeshTab.MeshAutoLink));
	}
	else 
		pAd->MeshTab.MeshAutoLink = TRUE;

	/*MeshForward */
	if (RTMPGetKeyParameter("MeshForward", tmpbuf, 255, buffer, TRUE))
	{
		LONG Enable;
		Enable = simple_strtol(tmpbuf, 0, 10);
		pAd->MeshTab.MeshCapability.field.Forwarding = (Enable > 0) ? (1) : (0);
		DBGPRINT(RT_DEBUG_TRACE, ("%s::(MeshForward=%d)\n", __FUNCTION__, pAd->MeshTab.MeshCapability.field.Forwarding));
	}

	/*MeshAuthMode */
	if (RTMPGetKeyParameter("MeshAuthMode", tmpbuf, 255, buffer, TRUE))
	{										
		if ((strncmp(tmpbuf, "WPANONE", 7) == 0) || (strncmp(tmpbuf, "wpanone", 7) == 0))
			wdev->AuthMode = Ndis802_11AuthModeWPANone;				
		else
			wdev->AuthMode = Ndis802_11AuthModeOpen;
			
		DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) MeshAuthMode(%d)=%s \n", wdev->AuthMode, GetAuthMode(wdev->AuthMode)));
		RTMPMakeRSNIE(pAd, wdev->AuthMode, wdev->WepStatus, MIN_NET_DEVICE_FOR_MESH);		

		if(wdev->AuthMode >= Ndis802_11AuthModeWPA)
		{	
			if (wdev->AuthMode == Ndis802_11AuthModeWPANone)
				wdev->DefaultKeyId = 0;
			else
				wdev->DefaultKeyId = 1;
		}

	}

	/*MeshEncrypType */
	if (RTMPGetKeyParameter("MeshEncrypType", tmpbuf, 255, buffer, TRUE))
	{										
		if (rtstrcasecmp(tmpbuf, "WEP") == TRUE)
        {
			if (wdev->AuthMode < Ndis802_11AuthModeWPA)
				wdev->WepStatus = Ndis802_11WEPEnabled;				  
		}
		else if (rtstrcasecmp(tmpbuf, "TKIP") == TRUE)
		{
			if (wdev->AuthMode >= Ndis802_11AuthModeWPA)
				wdev->WepStatus = Ndis802_11TKIPEnable;                       
        }
		else if (rtstrcasecmp(tmpbuf, "AES") == TRUE)
		{
			if (wdev->AuthMode >= Ndis802_11AuthModeWPA)
				wdev->WepStatus = Ndis802_11AESEnable;                            
		}    
		else
		{
			wdev->WepStatus = Ndis802_11WEPDisabled;                 
		}
							
		DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) MeshEncrypType(%d)=%s \n", wdev->WepStatus, GetEncryptType(wdev->WepStatus)));
		RTMPMakeRSNIE(pAd, wdev->AuthMode, wdev->WepStatus, MIN_NET_DEVICE_FOR_MESH);
		
	}
	
	/*MeshWPAKEY */
	if (RTMPGetKeyParameter("MeshWPAKEY", tmpbuf, 255, buffer, TRUE))
	{
		/* The WPA KEY must be 8~64 characters */
#if 0
		if((strlen(tmpbuf) >= 8) && (strlen(tmpbuf) <= 64))
		{
			NdisMoveMemory(pAd->MeshTab.WPAPassPhraseKey, tmpbuf, strlen(tmpbuf));
			pAd->MeshTab.WPAPassPhraseKeyLen = strlen(tmpbuf);
						
			if (strlen(tmpbuf) == 64)
			{/* Hex mode */
				AtoH(tmpbuf, pAd->MeshTab.PMK, 32);
			}
			else
			{/* ASCII mode */
				RtmpPasswordHash((char *)tmpbuf, pAd->MeshTab.MeshId, pAd->MeshTab.MeshIdLen, keyMaterial);
				NdisMoveMemory(pAd->MeshTab.PMK, keyMaterial, 32);
			}	
			DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) MeshWPAKEY=%s, its length=%d\n", pAd->MeshTab.WPAPassPhraseKey, pAd->MeshTab.WPAPassPhraseKeyLen));
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("I/F(mesh0) set MeshWPAKEY fail, key string required 8 ~ 64 characters!!!\n"));
		}
#else
		int success;

		success = RT_CfgSetWPAPSKKey(pAd, tmpbuf, strlen(tmpbuf), pAd->MeshTab.MeshId, pAd->MeshTab.MeshIdLen, pAd->MeshTab.PMK);
		if (success == TRUE)
		{
			NdisMoveMemory(pAd->MeshTab.WPAPassPhraseKey, tmpbuf, strlen(tmpbuf));
			pAd->MeshTab.WPAPassPhraseKeyLen = strlen(tmpbuf);
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("I/F(mesh0) set MeshWPAKEY fail, key string required 8 ~ 64 characters!!!\n"));
		}
#endif
																				
	}

	/*MeshDefaultkey */
	if (RTMPGetKeyParameter("MeshDefaultkey", tmpbuf, 255, buffer, TRUE))
	{								
		KeyIdx = simple_strtol(tmpbuf, 0, 10);
		if((KeyIdx >= 1 ) && (KeyIdx <= 4))
			wdev->DefaultKeyId = (UCHAR) (KeyIdx - 1);
		else
			wdev->DefaultKeyId = 0;

		DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) DefaultKeyID(0~3)=%d\n", wdev->DefaultKeyId));
	}

	/*MeshWEPKEY */
	if (RTMPGetKeyParameter("MeshWEPKEY", tmpbuf, 255, buffer, TRUE))
	{		    		                
		KeyLen = strlen(tmpbuf);
			
		/* Hex type */
		if((KeyLen == 10) || (KeyLen == 26))
		{
			pAd->MeshTab.SharedKey.KeyLen = KeyLen / 2;
			AtoH(tmpbuf, pAd->MeshTab.SharedKey.Key, KeyLen / 2);
			if (KeyLen == 10)
				pAd->MeshTab.SharedKey.CipherAlg = CIPHER_WEP64;
			else
				pAd->MeshTab.SharedKey.CipherAlg = CIPHER_WEP128;

			NdisMoveMemory(pAd->MeshTab.DesiredWepKey, tmpbuf, KeyLen);
			pAd->MeshTab.DesiredWepKeyLen = KeyLen;
							
			DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) MeshWEPKEY=%s, it's HEX type and %s\n", tmpbuf, (KeyLen == 10) ? "wep64":"wep128"));
		}
		/* ASCII type */
		else if ((KeyLen == 5) || (KeyLen == 13))
		{
			pAd->MeshTab.SharedKey.KeyLen = KeyLen;
			NdisMoveMemory(pAd->MeshTab.SharedKey.Key, tmpbuf, KeyLen);
			if (KeyLen == 5)
				pAd->MeshTab.SharedKey.CipherAlg = CIPHER_WEP64;
			else
				pAd->MeshTab.SharedKey.CipherAlg = CIPHER_WEP128;

			NdisMoveMemory(pAd->MeshTab.DesiredWepKey, tmpbuf, KeyLen);
			pAd->MeshTab.DesiredWepKeyLen = KeyLen;
									
			DBGPRINT(RT_DEBUG_TRACE, ("I/F(mesh0) MeshWEPKEY=%s, it's ASCII type and %s\n", tmpbuf, (KeyLen == 5) ? "wep64":"wep128"));
		}
		/*Invalid key length */
		else
		{ 
			pAd->MeshTab.DesiredWepKeyLen = 0;
			DBGPRINT(RT_DEBUG_ERROR, ("I/F(mesh0) MeshWEPKEY is Invalid key length(%d)!\n", (UCHAR)KeyLen));
		}
	}	

	return;
}


VOID LocalMsaIeInit(
	IN PRTMP_ADAPTER pAd,
	IN INT			 idx)
{
	PMSA_HANDSHAKE_IE	pLocalMsaIe;
	struct wifi_dev *wdev;

	/* clear the local MP's MSAIE field */
	wdev = &pAd->MeshTab.wdev;
	NdisZeroMemory(pAd->MeshTab.MeshLink[idx].Entry.LocalMsaIe, MESH_MAX_MSAIE_LEN);	
	pAd->MeshTab.MeshLink[idx].Entry.LocalMsaIeLen = 0;

	pLocalMsaIe = (PMSA_HANDSHAKE_IE)pAd->MeshTab.MeshLink[idx].Entry.LocalMsaIe;

#ifdef RELEASE_EXCLUDE
	/* Requests Authentication subfield of the Handshake Control field shall be set to 1 if the local MP */
	/* requests Initial MSA Authentication during this MSA authentication mechanism. This subfield */
	/* shall be set to zero if the PMKID list field of the RSNIE contains one or more entries. */
#endif /* RELEASE_EXCLUDE */

	pLocalMsaIe->MeshHSControl.word = 0;	
	if (pAd->MeshTab.bInitialMsaDone == FALSE || pAd->MeshTab.PMKID_Len == 0)
		pLocalMsaIe->MeshHSControl.field.RequestAuth = 1;	
	
#ifdef RELEASE_EXCLUDE
	/* Selected AKM and Pairwise-Cipher Suite */
	/* If the local MP is the Selector MP, the field shall contain the local MP's selection */
#endif /* RELEASE_EXCLUDE */
	if (pAd->MeshTab.MeshLink[idx].Entry.bValidLocalMpAsSelector)
	{
		UCHAR	AuthMode = wdev->AuthMode;
		UCHAR	EncrypType = wdev->WepStatus;
	
		switch (AuthMode)
        {
           	case Ndis802_11AuthModeWPA2:            	
               	NdisMoveMemory(pLocalMsaIe->SelectedAKM, OUI_MSA_8021X_AKM, LEN_OUI_SUITE);
            break;

          	case Ndis802_11AuthModeWPA2PSK:
           		NdisMoveMemory(pLocalMsaIe->SelectedAKM, OUI_MSA_PSK_AKM, LEN_OUI_SUITE);
			break;

#ifdef WPA3_SUPPORT
			case Ndis802_11AuthModeWPA3SAE:
				NdisMoveMemory(pLocalMsaIe->SelectedAKM, OUI_MSA_SAE_AKM, LEN_OUI_SUITE);
				break;
#endif

			/*default: */
			case Ndis802_11AuthModeWPANone:                
	            NdisMoveMemory(pLocalMsaIe->SelectedAKM, OUI_WPA_NONE_AKM, LEN_OUI_SUITE);
            break;
        }	
			
		switch (EncrypType)
        {
           	case Ndis802_11TKIPEnable:  
				if (wdev->AuthMode == Ndis802_11AuthModeWPANone)
	               	NdisMoveMemory(pLocalMsaIe->SelectedPairwiseCipher, OUI_WPA_TKIP, LEN_OUI_SUITE);
				else
					NdisMoveMemory(pLocalMsaIe->SelectedPairwiseCipher, OUI_WPA2_TKIP, LEN_OUI_SUITE);
			break;

          	case Ndis802_11AESEnable:
           		if (wdev->AuthMode == Ndis802_11AuthModeWPANone)
	               	NdisMoveMemory(pLocalMsaIe->SelectedPairwiseCipher, OUI_WPA_CCMP, LEN_OUI_SUITE);
				else
					NdisMoveMemory(pLocalMsaIe->SelectedPairwiseCipher, OUI_WPA2_CCMP, LEN_OUI_SUITE);
            break;			
        }			
	}

	pAd->MeshTab.MeshLink[idx].Entry.LocalMsaIeLen = sizeof(MSA_HANDSHAKE_IE);

}

/* 
    ==========================================================================
    Description:
        It shall be queried about mesh security information through IOCTL     
	Arguments:
	    pAd		Pointer to our adapter
	    wrq		Pointer to the ioctl argument
    ==========================================================================
*/
VOID RTMPIoctlQueryMeshSecurityInfo(
		IN PRTMP_ADAPTER pAd, 
		IN RTMP_IOCTL_INPUT_STRUCT *wrq)
{
	UCHAR	key_len = 0;
	MESH_SECURITY_INFO	meshInfo;
	struct wifi_dev *wdev;


	DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlQueryMeshSecurityInfo==>\n"));
	
	NdisZeroMemory((PUCHAR)&meshInfo, sizeof(MESH_SECURITY_INFO));
	wdev = &pAd->MeshTab.wdev;
	if (wdev->AuthMode == Ndis802_11AuthModeOpen && wdev->WepStatus == Ndis802_11WEPEnabled)
		meshInfo.EncrypType = ENCRYPT_OPEN_WEP;		/* 1 - 	OPEN-WEP */
	else if (wdev->AuthMode == Ndis802_11AuthModeWPANone && wdev->WepStatus == Ndis802_11TKIPEnable)
		meshInfo.EncrypType = ENCRYPT_WPANONE_TKIP;	/* 2 -	WPANONE-TKIP */
	else if (wdev->AuthMode == Ndis802_11AuthModeWPANone && wdev->WepStatus == Ndis802_11AESEnable)
		meshInfo.EncrypType = ENCRYPT_WPANONE_AES;	/* 3 -	WPANONE-AES */
	else
		meshInfo.EncrypType = ENCRYPT_OPEN_NONE;	/* 0 - 	OPEN-NONE */

	meshInfo.KeyIndex = wdev->DefaultKeyId + 1;
				
	if (meshInfo.EncrypType == ENCRYPT_OPEN_WEP)
	{
		key_len = pAd->MeshTab.DesiredWepKeyLen;
		if (key_len > 0)
		{
			meshInfo.KeyLength = key_len;
			NdisMoveMemory(meshInfo.KeyMaterial, pAd->MeshTab.DesiredWepKey, key_len);
		}
	}
	else if (meshInfo.EncrypType == ENCRYPT_WPANONE_TKIP || meshInfo.EncrypType == ENCRYPT_WPANONE_AES)
	{
		key_len = pAd->MeshTab.WPAPassPhraseKeyLen;
		if (key_len > 0)
		{
			meshInfo.KeyLength = key_len;
			NdisMoveMemory(meshInfo.KeyMaterial, pAd->MeshTab.WPAPassPhraseKey, key_len);
		}
	}

	wrq->u.data.length = sizeof(MESH_SECURITY_INFO);
	if (copy_to_user(wrq->u.data.pointer, &meshInfo, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: copy_to_user() fail\n", __FUNCTION__));
	}

}


/* 
    ==========================================================================
    Description:
        It shall be set mesh security through IOCTL        
	Arguments:
	    pAd		Pointer to our adapter
	    wrq		Pointer to the ioctl argument
    ==========================================================================
*/
INT RTMPIoctlSetMeshSecurityInfo(
		IN PRTMP_ADAPTER pAd, 
		IN RTMP_IOCTL_INPUT_STRUCT *wrq)
{	
	INT 	Status = NDIS_STATUS_SUCCESS;
	MESH_SECURITY_INFO	meshInfo;
	UCHAR	MeshKey[255];
	struct wifi_dev *wdev;

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlSetMeshSecurityInfo==>\n"));

	wdev = &pAd->MeshTab.wdev;
	if (wrq->u.data.length != sizeof(MESH_SECURITY_INFO))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: the length is too large \n", __FUNCTION__));
		return -EINVAL;
	}

	if (copy_from_user(&meshInfo, wrq->u.data.pointer, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: copy_from_user() fail\n", __FUNCTION__));
		return -EFAULT;
	}

	/* Default security mode is OPEN-NONE */
	wdev->AuthMode = Ndis802_11AuthModeOpen;
	wdev->WepStatus = Ndis802_11EncryptionDisabled;
	NdisZeroMemory((PUCHAR)&pAd->MeshTab.SharedKey, sizeof(CIPHER_KEY));

	/* Set default key index */
	if((meshInfo.KeyIndex >= 1) && (meshInfo.KeyIndex <= 4))
		wdev->DefaultKeyId = meshInfo.KeyIndex - 1;
	else
		wdev->DefaultKeyId = 0;
		
	NdisZeroMemory(MeshKey, 255);
	if (meshInfo.KeyLength > 0)
		NdisMoveMemory(MeshKey, meshInfo.KeyMaterial, meshInfo.KeyLength);
	MeshKey[sizeof(MeshKey)-1] = 0x00;
		
	/* OPEN-WEP */
	if (meshInfo.EncrypType == ENCRYPT_OPEN_WEP)					
	{
		if (!Set_MeshWEPKEY_Proc(pAd, (RTMP_STRING *) MeshKey))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh WEP key isn't valid \n", __FUNCTION__));
			return -EFAULT;
		}

		wdev->AuthMode = Ndis802_11AuthModeOpen;
		wdev->WepStatus = Ndis802_11WEPEnabled;
	}
	else if (meshInfo.EncrypType == ENCRYPT_WPANONE_TKIP || meshInfo.EncrypType == ENCRYPT_WPANONE_AES) 
	{
		if (!Set_MeshWPAKEY_Proc(pAd, (RTMP_STRING *) MeshKey))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Mesh WPA key isn't valid \n", __FUNCTION__));
			return -EFAULT;
		}

		wdev->AuthMode = Ndis802_11AuthModeWPANone;
		if (meshInfo.EncrypType == ENCRYPT_WPANONE_TKIP)
			wdev->WepStatus = Ndis802_11TKIPEnable;
		else
			wdev->WepStatus = Ndis802_11AESEnable;

		RTMPMakeRSNIE(pAd, wdev->AuthMode, wdev->WepStatus, MIN_NET_DEVICE_FOR_MESH);
	}
			
	return Status;

}

UCHAR GetMeshSecurity(RTMP_ADAPTER *pAd)
{
	struct wifi_dev *wdev;
	UCHAR	EncrypType = ENCRYPT_OPEN_NONE;

	wdev = &pAd->MeshTab.wdev;
	if (wdev->AuthMode==Ndis802_11AuthModeOpen
	&& wdev->WepStatus == Ndis802_11WEPDisabled)
	{

	}
	else if (wdev->AuthMode==Ndis802_11AuthModeOpen
	&& wdev->WepStatus == Ndis802_11WEPEnabled)
	{

		EncrypType = ENCRYPT_OPEN_WEP;
	}
	else if (wdev->AuthMode==Ndis802_11AuthModeWPANone
	&& wdev->WepStatus == Ndis802_11TKIPEnable)
	{
		EncrypType = ENCRYPT_WPANONE_TKIP;
	}
	else if (wdev->AuthMode==Ndis802_11AuthModeWPANone
	&& wdev->WepStatus == Ndis802_11AESEnable)
	{
		EncrypType = ENCRYPT_WPANONE_AES;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR,
			("%s: Can not find correct mesh security ,return OPEN_NONE Auth:%d WepStatus:%d \n",
			__FUNCTION__, wdev->AuthMode,wdev->WepStatus));
	}


	return EncrypType;
}

UCHAR MeshCheckPeerMpCipher(
		IN USHORT 		 CapabilityInfo, 
		IN PUCHAR 		 pVIE,
		IN USHORT		 LenVIE)
{
	UCHAR	EncrypType = ENCRYPT_OPEN_NONE;
	
	if (CAP_IS_PRIVACY_ON(CapabilityInfo))
	{
		PUCHAR  		pVarIEs;
		USHORT			VarIELen;				
		PEID_STRUCT     pEid;

		EncrypType = ENCRYPT_OPEN_WEP;
			
		pVarIEs  = pVIE;
		VarIELen = LenVIE;
		
		while (VarIELen > 0)
		{
			pEid = (PEID_STRUCT) pVarIEs;

			if ((pEid->Eid == IE_WPA) && (VarIELen > (sizeof(RSNIE) + 2)) && (NdisEqualMemory(pEid->Octet, WPA_OUI, 4)))
			{
				RSNIE	*pRsnie;

				pRsnie = (RSNIE*)pEid->Octet;

				if(RTMPEqualMemory(OUI_WPA_TKIP, pRsnie->ucast[0].oui, 4))						
				EncrypType = ENCRYPT_WPANONE_TKIP;
				else if (RTMPEqualMemory(OUI_WPA_CCMP, pRsnie->ucast[0].oui, 4))			
				EncrypType = ENCRYPT_WPANONE_AES;			

				break;
		}
			/* For WPA2/WPA2PSK */
#if 0
			else if ((pEid->Eid == IE_RSN) && (VarIELen > sizeof(RSNIE2)) && (NdisEqualMemory(pEid->Octet + 2, RSN_OUI, 3)))
		{
				/* Not implement yet */
			}
#endif
			
			pVarIEs   += (pEid->Len + 2);
			VarIELen  -= (pEid->Len + 2);
		}
														
	}

	return EncrypType;
}

BOOLEAN MeshAllowToSendPacket(
	IN RTMP_ADAPTER *pAd,
	IN struct wifi_dev *wdev,
	IN PNDIS_PACKET pPacket,
	OUT UCHAR		*pWcid)
{
	BOOLEAN	allowed = FALSE;
	PMESH_PROXY_ENTRY	pMeshProxyEntry = NULL;
	PUCHAR pSrcBufVA = GET_OS_PKT_DATAPTR(pPacket);
//+++Add by shiang for merge code
	PMESH_STRUCT pMesh;

	pMesh = (PMESH_STRUCT)&pAd->MeshTab;
	do
	{
		if (MeshValid(pMesh) != TRUE)
			break;
		/* find the device in our Mesh list */
		if (pMesh->wdev.if_dev == pDev)
		{
			/* ya! find it */
			pAd->RalinkCounters.PendingNdisPacketCount ++;
			RTMP_SET_PACKET_MOREDATA(pPktSrc, FALSE);
			RTMP_SET_PACKET_WDEV(pPktSrc, wdev->wdev_idx);
			SET_OS_PKT_NETDEV(pPktSrc, pAd->net_dev);
#ifdef RELEASE_EXCLUDE			
			if (!(*(GET_OS_PKT_DATAPTR(pPktSrc)) & 0x01))
			{
				DBGPRINT(RT_DEBUG_INFO,
							("%s(Mesh) - unicast packet to "
							"(Mesh0)\n", __FUNCTION__));
			}
 #endif /* RELEASE_EXCLUDE */


			/* transmit the packet */
			return Func(RTPKT_TO_OSPKT(pPktSrc));
		}
    } while(FALSE);
//---Add by shiang for merge code


	if (MESH_ON(pAd))
	{
		LONG RouteId = PathRouteIDSearch(pAd, pSrcBufVA);

		if (!MAC_ADDR_EQUAL(pAd->MeshTab.wdev.if_addr, pSrcBufVA + MAC_ADDR_LEN))
		{
			if ((pAd->MeshTab.OpMode & MESH_AP) || (pAd->MeshTab.OpMode & MESH_POTAL))
			{
				pMeshProxyEntry = MeshProxyEntryTableLookUp(pAd, pSrcBufVA + MAC_ADDR_LEN);

				if (!pMeshProxyEntry)
					pMeshProxyEntry = MeshProxyEntryTableInsert(pAd, pAd->MeshTab.wdev.if_addr, pSrcBufVA + MAC_ADDR_LEN);
			}
			else
			{
#ifdef RELEASE_EXCLUDE
				/*if we r not MAP or MP with proxy , we will drop all packets which sa is not us. */
				/*if MP is bridged , it must enable proxy or layer 3 packets will not be forwarded correctly */
				/*because bridge mac will not be the same as MP wireless card mac. */
#endif /* RELEASE_EXCLUDE */
				return FALSE;
			}
		}

		if (RouteId == BMCAST_ROUTE_ID)
		{
			MeshClonePacket(pAd, pPacket, MESH_PROXY, 0);
		}
		else if (RouteId >= 0)
		{
			INT LinkId;

			LinkId = PathMeshLinkIDSearch(pAd, RouteId);
			if ((LinkId >= 0) && VALID_MESH_LINK_ID(LinkId))
			{
				*pWcid = pAd->MeshTab.MeshLink[LinkId].Entry.MacTabMatchWCID;
				RTMP_SET_MESH_ROUTE_ID(pPacket, (UINT8)RouteId);

				/* MESH_PROXY indicate the packet come from os layer to mesh0 virtual interface. */
				RTMP_SET_MESH_SOURCE(pPacket, MESH_PROXY);
				allowed = TRUE;
			}
		}
		else
		{
			/* entity is not exist, start path discovery. */
			if (MAC_ADDR_EQUAL(pSrcBufVA + MAC_ADDR_LEN, pAd->MeshTab.wdev.if_addr))
				MeshCreatePreqAction(pAd, NULL, pSrcBufVA);
			else
				MeshCreatePreqAction(pAd, pSrcBufVA + MAC_ADDR_LEN, pSrcBufVA);
		}
	}
 
	return allowed;
}


VOID PerpareMeshHeader(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK *pTxBlk,
	OUT PMESH_FLAG pMeshFlag,
	OUT UINT16 *pMeshTTL,
	OUT UINT32 *pMeshSeq,
	OUT PUCHAR *ppMeshAddr5,
	OUT PUCHAR *ppMeshAddr6)
{
	UINT MeshHdrLen = 0;
	PNDIS_PACKET pPacket = pTxBlk->pPacket;
	PUCHAR pSrcBufVA = GET_OS_PKT_DATAPTR(pPacket);
	UINT8 TXWISize = pAd->chipCap.TXWISize;
	PHEADER_802_11 pHeader_802_11 = (PHEADER_802_11)&pTxBlk->HeaderBuf[TXINFO_SIZE + pAd->chipCap.TXWISize];

	if (RTMP_GET_MESH_SOURCE(pPacket) == MESH_PROXY)
	{
		pMeshFlag->word = 0;
		*pMeshTTL = pAd->MeshTab.TTL;
		*pMeshSeq = INC_MESH_SEQ(pAd->MeshTab.MeshSeq);
		if (( (*pSrcBufVA & 0x01) /* B/Mcast packet. */
				|| MAC_ADDR_EQUAL(pSrcBufVA, pHeader_802_11->Addr3)) /* or DA is a MP. */
			&& MAC_ADDR_EQUAL(pSrcBufVA + MAC_ADDR_LEN, pAd->MeshTab.wdev.if_addr))
		{	
#ifdef RELEASE_EXCLUDE
			/* the packet if come from current MP to another MP. */
			/* Since DA equal to MesH DA, and SA equal to current MP's MAC addr. */
#endif /* RELEASE_EXCLUDE */
			pMeshFlag->field.AE = 0;
			*ppMeshAddr5 = NULL;
			*ppMeshAddr6 = NULL;
		}
		else
		{
			pMeshFlag->field.AE = 2;
			*ppMeshAddr5 = pSrcBufVA;
			*ppMeshAddr6 = pSrcBufVA + MAC_ADDR_LEN;
		}
	}
	else
	{
#ifdef RELEASE_EXCLUDE
		/* must be MESH_FORWARD here. */
		/* shift pSrcBufVA pointer to Mesh header. */
#endif /* RELEASE_EXCLUDE */
		pSrcBufVA += LENGTH_802_3;
		MeshHdrLen = GetMeshHederLen(pSrcBufVA);
		pMeshFlag->word = GetMeshFlag(pSrcBufVA);
		*pMeshTTL = GetMeshTTL(pSrcBufVA) - 1;
		*pMeshSeq = GetMeshSeq(pSrcBufVA);
		*ppMeshAddr5 = GetMeshAddr5(pSrcBufVA);
		*ppMeshAddr6 = GetMeshAddr6(pSrcBufVA);

		if (NdisEqualMemory(SNAP_802_1H, pTxBlk->pSrcBufData + MeshHdrLen, 6)  || NdisEqualMemory(SNAP_BRIDGE_TUNNEL, pTxBlk->pSrcBufData + MeshHdrLen, 6))             
		{
			/* skip Mesh header and LLC Header (8 Bytes). */
			pTxBlk->pSrcBufData = pTxBlk->pSrcBufData + MeshHdrLen + 8;
			pTxBlk->SrcBufLen  -= (MeshHdrLen + 8);
		}
		else
		{
			pTxBlk->pSrcBufData = pTxBlk->pSrcBufData + MeshHdrLen;
			pTxBlk->SrcBufLen  -= (MeshHdrLen);		
		}
	}

	return;
}

BOOLEAN MeshChCheck(
	IN RTMP_ADAPTER *pAd,
	IN PMESH_NEIGHBOR_ENTRY pNeighborEntry)
{
	BOOLEAN result = FALSE;
	
#ifdef DOT11_N_SUPPORT
	if (WMODE_CAP_N(pAd->CommonCfg.PhyMode)
		&& (pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40)
		&& (pAd->CommonCfg.AddHTInfo.AddHtInfo.RecomWidth == BW_40)
		&& (pNeighborEntry->ChBW == BW_40)
		)
	{
		result = ((pNeighborEntry->Channel == pAd->CommonCfg.Channel)
					&& (pNeighborEntry->ChBW == pAd->CommonCfg.AddHTInfo.AddHtInfo.RecomWidth)
					&& (pNeighborEntry->ExtChOffset == pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset)) ? TRUE : FALSE;
	}
	else
#endif /* DOT11_N_SUPPORT */
	{	
		result = (pNeighborEntry->Channel == pAd->CommonCfg.Channel) ? TRUE : FALSE;
	}


	return result;
}

/*
	==========================================================================
	Description:
		Pre-build a BEACON frame in the shared memory
	==========================================================================
*/
VOID MeshMakeBeacon(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			idx)
{
	UCHAR DsLen = 1, SsidLen, *tmac_info = (UCHAR *)(&pAd->BeaconTxWI);
	UCHAR RSNIe=IE_WPA;
	HEADER_802_11 BcnHdr;
	LARGE_INTEGER FakeTimestamp;
	ULONG FrameLen = 0;
	PUCHAR pBeaconFrame = (PUCHAR)pAd->MeshTab.BeaconBuf;
	UCHAR *ptr;
	UINT i;
	UINT32 longValue;
	UCHAR ASupRate[] = {0x8C, 0x12, 0x98, 0x24, 0xb0, 0x48, 0x60, 0x6C};
	UCHAR ASupRateLen = sizeof(ASupRate)/sizeof(UCHAR);
	ULONG TmpLen;
	HTTRANSMIT_SETTING BeaconTransmit;   /* MGMT frame PHY rate setting when operatin at Ht rate. */
	UINT8 TXWISize = pAd->chipCap.TXWISize;
	struct wifi_dev *wdev;
	MAC_TX_INFO mac_info;

	/* ignore SSID for MP. Refer to IEEE 802.11s-D1.06 */
	SsidLen = 0;
	wdev = &pAd->MeshTab.wdev;
	if (pAd->MeshTab.bcn_buf.bBcnSntReq == FALSE)
		return;

#ifdef RTMP_MAC_USB
	RTUSBBssBeaconStop(pAd);
#endif /* RTMP_MAC_USB */

	DBGPRINT(RT_DEBUG_TRACE, ("MeshMakeBeacon - %02x:%02x:%02x:%02x:%02x:%02x\n",
					PRINT_MAC(wdev->if_addr)));
	MgtMacHeaderInit(pAd, &BcnHdr, SUBTYPE_BEACON, 0, BROADCAST_ADDR,
						wdev->if_addr, wdev->if_addr);

	if (wdev->AuthMode == Ndis802_11AuthModeWPANone)		
		RSNIe = IE_WPA;
	else if ((wdev->AuthMode == Ndis802_11AuthModeWPA2) || 
			(wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)
#ifdef WPA3_SUPPORT
			|| (wdev->AuthMode == Ndis802_11AuthModeWPA3SAE)
#endif
			)
		RSNIe = IE_WPA2;
	
	/* for update framelen to TxWI later. */
	MakeOutgoingFrame(pBeaconFrame,                  &FrameLen,
					sizeof(HEADER_802_11),           &BcnHdr, 
					TIMESTAMP_LEN,                   &FakeTimestamp,
					2,                               &pAd->CommonCfg.BeaconPeriod,
					2,                               &pAd->MeshTab.CapabilityInfo,
					1,                               &SsidIe, 
					1,                               &SsidLen,
					END_OF_ARGS);

	if (pAd->CommonCfg.Channel <= 14)
	{
		MakeOutgoingFrame(pBeaconFrame+FrameLen,         &TmpLen,
					1,                               &SupRateIe, 
					1,                               &pAd->CommonCfg.SupRateLen,
					pAd->CommonCfg.SupRateLen,       pAd->CommonCfg.SupRate, 
						END_OF_ARGS);
		FrameLen += TmpLen;
	}
	else
	{
		MakeOutgoingFrame(pBeaconFrame+FrameLen,         &TmpLen,
						1,                               &SupRateIe, 
						1,                               &ASupRateLen,
						pAd->CommonCfg.SupRateLen,       ASupRate, 
						END_OF_ARGS);
		FrameLen += TmpLen;
	}

	MakeOutgoingFrame(pBeaconFrame+FrameLen,         &TmpLen,
					1,                               &DsIe, 
					1,                               &DsLen, 
					1,                               &pAd->MeshTab.MeshChannel,
					END_OF_ARGS);
	FrameLen += TmpLen;

#ifdef RELEASE_EXCLUDE
	/* 5G band supported rates all list on support rate IE. */
	/* no necessary extending rate here. */
#endif /* RELEASE_EXCLUDE */
	if ((pAd->CommonCfg.Channel <= 14)
		&& (pAd->CommonCfg.ExtRateLen))
	{
		MakeOutgoingFrame(pBeaconFrame+FrameLen,         &TmpLen,
						1,                               &ExtRateIe, 
						1,                               &pAd->CommonCfg.ExtRateLen,
						pAd->CommonCfg.ExtRateLen,           pAd->CommonCfg.ExtRate, 
						END_OF_ARGS);
		FrameLen += TmpLen;
	}

	/* Append RSN_IE when  WPA OR WPAPSK, */
	if (wdev->AuthMode >= Ndis802_11AuthModeWPA)
	{
		MakeOutgoingFrame(pBeaconFrame+FrameLen,        &TmpLen,
						  1,                            &RSNIe,
						  1,                            &pAd->MeshTab.RSNIE_Len,
						  pAd->MeshTab.RSNIE_Len,      	pAd->MeshTab.RSN_IE,
						  END_OF_ARGS);
		FrameLen += TmpLen;
	}
        
#ifdef DOT11_N_SUPPORT
	/* AP Channel Report */
	if (pAd->CommonCfg.PhyMode == (WMODE_B | WMODE_G | WMODE_GN)
		&& ((pAd->CommonCfg.HT_DisallowTKIP == FALSE)
			|| (pAd->CommonCfg.HT_DisallowTKIP
				&& !IS_INVALID_HT_SECURITY(wdev->WepStatus))) 
		) /*wayne_note */
	{
		UCHAR APChannelReportIe = IE_AP_CHANNEL_REPORT;
		ULONG	TmpLen;

		/* 802.11n D2.0 Annex J */
		/* USA */
		/* regulatory class 32, channel set 1~7 */
		/* regulatory class 33, channel set 5-11 */

		UCHAR rclass32[]={32, 1, 2, 3, 4, 5, 6, 7};
        UCHAR rclass33[]={33, 5, 6, 7, 8, 9, 10, 11};
		UCHAR rclasslen = 8; /*sizeof(rclass32); */

		
			MakeOutgoingFrame(pBeaconFrame+FrameLen,&TmpLen,
							  1,                    &APChannelReportIe,
							  1,                    &rclasslen,
							  rclasslen,            rclass32,
   							  1,                    &APChannelReportIe,
							  1,                    &rclasslen,
							  rclasslen,            rclass33,
							  END_OF_ARGS);
			FrameLen += TmpLen;		
		}
#endif /* DOT11_N_SUPPORT */
    
	BeaconTransmit.word = 0;

	NdisZeroMemory((UCHAR *)&mac_info, sizeof(mac_info));	
	mac_info.FRAG = FALSE;
	
	mac_info.CFACK = FALSE;
	mac_info.InsTimestamp = TRUE;
	mac_info.AMPDU = FALSE;
	
	mac_info.BM = 1;
	mac_info.Ack = FALSE;
	mac_info.NSeq = TRUE;
	mac_info.BASize = 0;
	
	mac_info.WCID = BSS0Mcast_WCID;
	mac_info.Length = FrameLen;
	mac_info.PID = PID_MGMT;
	
	mac_info.TID = 0;
	mac_info.TxRate = 0;
	mac_info.Txopmode = IFS_HTTXOP;
	mac_info.Preamble = LONG_PREAMBLE;
	// TODO: shiang-MT7603
	mac_info.q_idx = Q_IDX_BCN;

	mac_info.SpeEn = 1;

	write_tmac_info(pAd, tmac_info, &mac_info, &BeaconTransmit);

	/*
		step 6. move BEACON TXD and frame content to on-chip memory
	*/
	ptr = (PUCHAR)&pAd->BeaconTxWI;
#ifdef RT_BIG_ENDIAN
    RTMPWIEndianChange(pAd, ptr, TYPE_TXWI);
#endif


	for (i=0; i<TXWISize; i+=4)
	{
		longValue =  *ptr + (*(ptr+1)<<8) + (*(ptr+2)<<16) + (*(ptr+3)<<24);
		RTMP_CHIP_UPDATE_BEACON(pAd, pAd->BeaconOffset[idx] + i, longValue, 4);
		ptr +=4;
	}


	/* update BEACON frame content. start right after the 24-byte TXINFO field */
	ptr = (PUCHAR)pAd->MeshTab.BeaconBuf;
#ifdef RT_BIG_ENDIAN
    RTMPFrameEndianChange(pAd, ptr, DIR_WRITE, FALSE);
#endif


	for (i= 0; i< FrameLen; i+=4)
	{
		longValue =  *ptr + (*(ptr+1)<<8) + (*(ptr+2)<<16) + (*(ptr+3)<<24);
		RTMP_CHIP_UPDATE_BEACON(pAd, pAd->BeaconOffset[idx] + TXWISize + i, longValue, 4);
		ptr += 4;
	}


	pAd->MeshTab.TimIELocationInBeacon = (UCHAR)FrameLen; 
	pAd->MeshTab.bcn_buf.cap_ie_pos = sizeof(HEADER_802_11) + TIMESTAMP_LEN + 2;

#ifdef RTMP_MAC_USB
	RTUSBBssBeaconStart(pAd);
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_STA_SUPPORT
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type != HIF_MT) {
		RTMP_IO_READ32(pAd, MAC_BSSID_DW1, &longValue);
		longValue &= 0x0000FFFF;
		RTMP_IO_WRITE32(pAd, MAC_BSSID_DW1, longValue);
	}
#endif /* CONFIG_STA_SUPPORT */

}

/*
	==========================================================================
	Description:
		Update the BEACON frame in the shared memory. Because TIM IE is variable
		length. other IEs after TIM has to shift and total frame length may change
		for each BEACON period.
	Output:
		pAd->ApCfg.MBSSID[apidx].CapabilityInfo
		pAd->ApCfg.ErpIeContent
	==========================================================================
*/
VOID MeshUpdateBeaconFrame(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			idx) 
{
	PUCHAR pBeaconFrame = (PUCHAR)pAd->MeshTab.BeaconBuf;
	UCHAR *ptr;
	ULONG FrameLen = pAd->MeshTab.TimIELocationInBeacon;
	ULONG UpdatePos = pAd->MeshTab.TimIELocationInBeacon;
	HTTRANSMIT_SETTING BeaconTransmit;   /* MGMT frame PHY rate setting when operatin at Ht rate. */
	struct wifi_dev *wdev;
	MAC_TX_INFO mac_info;

	if (pAd->MeshTab.bcn_buf.bBcnSntReq == FALSE)
		return;

	/* The beacon of mesh isn't be initialized */
	if (FrameLen == 0)
		return;

	wdev = &pAd->MeshTab.wdev;
	/* */
	/* step 1 - update BEACON's Capability */
	/* */
	ptr = pBeaconFrame + pAd->MeshTab.bcn_buf.cap_ie_pos;
	*ptr = (UCHAR)(pAd->MeshTab.CapabilityInfo & 0x00ff);
	*(ptr+1) = (UCHAR)((pAd->MeshTab.CapabilityInfo & 0xff00) >> 8);

#ifdef DOT11_N_SUPPORT
	/* */
	/* step 5. Update HT. Since some fields might change in the same BSS. */
	/* */
	if (WMODE_CAP_N(pAd->CommonCfg.PhyMode)
		&& ((pAd->CommonCfg.HT_DisallowTKIP == FALSE)
		|| (pAd->CommonCfg.HT_DisallowTKIP
			&& !IS_INVALID_HT_SECURITY(wdev->WepStatus))) 
	) /*wayne_note */
	{
		ULONG TmpLen;
		UCHAR HtLen, HtLen1;
		/*UCHAR i; */

#ifdef RT_BIG_ENDIAN
		HT_CAPABILITY_IE HtCapabilityTmp;
		ADD_HT_INFO_IE	addHTInfoTmp;
		USHORT	b2lTmp, b2lTmp2;
#endif

		/* add HT Capability IE */
		HtLen = sizeof(pAd->CommonCfg.HtCapability);
		HtLen1 = sizeof(pAd->CommonCfg.AddHTInfo);
#ifndef RT_BIG_ENDIAN
		MakeOutgoingFrame(pBeaconFrame+FrameLen,         &TmpLen,
								  1,                                &HtCapIe,
								  1,                                &HtLen,
								 HtLen,          &pAd->CommonCfg.HtCapability, 
								  1,                                &AddHtInfoIe,
								  1,                                &HtLen1,
								 HtLen1,          &pAd->CommonCfg.AddHTInfo, 
						  END_OF_ARGS);
#else
		NdisMoveMemory(&HtCapabilityTmp, &pAd->CommonCfg.HtCapability, HtLen);
		*(USHORT *)(&HtCapabilityTmp.HtCapInfo) = SWAP16(*(USHORT *)(&HtCapabilityTmp.HtCapInfo));
		*(USHORT *)(&HtCapabilityTmp.ExtHtCapInfo) = SWAP16(*(USHORT *)(&HtCapabilityTmp.ExtHtCapInfo));

		NdisMoveMemory(&addHTInfoTmp, &pAd->CommonCfg.AddHTInfo, HtLen1);
		*(USHORT *)(&addHTInfoTmp.AddHtInfo2) = SWAP16(*(USHORT *)(&addHTInfoTmp.AddHtInfo2));
		*(USHORT *)(&addHTInfoTmp.AddHtInfo3) = SWAP16(*(USHORT *)(&addHTInfoTmp.AddHtInfo3));

		MakeOutgoingFrame(pBeaconFrame+FrameLen,         &TmpLen,
								  1,                                &HtCapIe,
								  1,                                &HtLen,
								 HtLen,                   &HtCapabilityTmp, 
								  1,                                &AddHtInfoIe,
								  1,                                &HtLen1,
								 HtLen1,                   &addHTInfoTmp, 
						  END_OF_ARGS);
#endif
		FrameLen += TmpLen;

	}
#endif /* DOT11_N_SUPPORT */

#if 0	/* remove it temporarily */
	/* add WMM IE here */
	if (pAd->ApCfg.MBSSID[apidx].wdev.bWmmCapable)
	{
		ULONG TmpLen;
		UCHAR i;
		UCHAR WmeParmIe[26] = {IE_VENDOR_SPECIFIC, 24, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01, 0, 0}; 
		WmeParmIe[8] = pAd->ApCfg.BssEdcaParm.EdcaUpdateCount & 0x0f;
#ifdef UAPSD_SUPPORT
        UAPSD_MR_IE_FILL(WmeParmIe[8], pAd);
#endif /* UAPSD_SUPPORT */
		for (i=QID_AC_BE; i<=QID_AC_VO; i++)
		{
			WmeParmIe[10+ (i*4)] = (i << 5)                                         +     /* b5-6 is ACI */
								   ((UCHAR)pAd->ApCfg.BssEdcaParm.bACM[i] << 4)     +     /* b4 is ACM */
								   (pAd->ApCfg.BssEdcaParm.Aifsn[i] & 0x0f);              /* b0-3 is AIFSN */
			WmeParmIe[11+ (i*4)] = (pAd->ApCfg.BssEdcaParm.Cwmax[i] << 4)           +     /* b5-8 is CWMAX */
								   (pAd->ApCfg.BssEdcaParm.Cwmin[i] & 0x0f);              /* b0-3 is CWMIN */
			WmeParmIe[12+ (i*4)] = (UCHAR)(pAd->ApCfg.BssEdcaParm.Txop[i] & 0xff);        /* low byte of TXOP */
			WmeParmIe[13+ (i*4)] = (UCHAR)(pAd->ApCfg.BssEdcaParm.Txop[i] >> 8);          /* high byte of TXOP */
		}

		MakeOutgoingFrame(pBeaconFrame+FrameLen,         &TmpLen,
						  26,                            WmeParmIe,
						  END_OF_ARGS);
		FrameLen += TmpLen;
	}
#endif

#ifdef DOT11_N_SUPPORT
	if (WMODE_CAP_N(pAd->CommonCfg.PhyMode)
		&& ((pAd->CommonCfg.HT_DisallowTKIP == FALSE)
 		|| (pAd->CommonCfg.HT_DisallowTKIP 
			&& !IS_INVALID_HT_SECURITY(wdev->WepStatus))) 
		)/*wayne_note */
	{
		ULONG TmpLen;
		UCHAR HtLen, HtLen1;
		/*UCHAR i; */
#ifdef RT_BIG_ENDIAN
		HT_CAPABILITY_IE HtCapabilityTmp;
		ADD_HT_INFO_IE	addHTInfoTmp;
		USHORT	b2lTmp, b2lTmp2;
#endif
		/* add HT Capability IE */
		HtLen = sizeof(pAd->CommonCfg.HtCapability);
		HtLen1 = sizeof(pAd->CommonCfg.AddHTInfo);

		if (pAd->bBroadComHT == TRUE)
		{
			UCHAR epigram_ie_len;
			UCHAR BROADCOM_HTC[4] = {0x0, 0x90, 0x4c, 0x33};
			UCHAR BROADCOM_AHTINFO[4] = {0x0, 0x90, 0x4c, 0x34};


			epigram_ie_len = HtLen + 4;
#ifndef RT_BIG_ENDIAN
			MakeOutgoingFrame(pBeaconFrame + FrameLen,      &TmpLen,
						  1,                                &WpaIe,
						  1,                                &epigram_ie_len,
						  4,                                &BROADCOM_HTC[0],
						  HtLen,          					&pAd->CommonCfg.HtCapability, 
						  END_OF_ARGS);
#else
			NdisMoveMemory(&HtCapabilityTmp, &pAd->CommonCfg.HtCapability, HtLen);
			*(USHORT *)(&HtCapabilityTmp.HtCapInfo) = SWAP16(*(USHORT *)(&HtCapabilityTmp.HtCapInfo));
			*(USHORT *)(&HtCapabilityTmp.ExtHtCapInfo) = SWAP16(*(USHORT *)(&HtCapabilityTmp.ExtHtCapInfo));

			MakeOutgoingFrame(pBeaconFrame + FrameLen,       &TmpLen,
						1,                               &WpaIe,
						1,                               &epigram_ie_len,
						4,                               &BROADCOM_HTC[0], 
						HtLen,                           &HtCapabilityTmp,
						END_OF_ARGS);
#endif

			FrameLen += TmpLen;

			epigram_ie_len = HtLen1 + 4;
#ifndef RT_BIG_ENDIAN
			MakeOutgoingFrame(pBeaconFrame + FrameLen,        &TmpLen,
						  1,                                &WpaIe,
						  1,                                &epigram_ie_len,
						  4,                                &BROADCOM_AHTINFO[0],
						  HtLen1, 							&pAd->CommonCfg.AddHTInfo, 
						  END_OF_ARGS);
#else
			NdisMoveMemory(&addHTInfoTmp, &pAd->CommonCfg.AddHTInfo, HtLen1);
			*(USHORT *)(&addHTInfoTmp.AddHtInfo2) = SWAP16(*(USHORT *)(&addHTInfoTmp.AddHtInfo2));
			*(USHORT *)(&addHTInfoTmp.AddHtInfo3) = SWAP16(*(USHORT *)(&addHTInfoTmp.AddHtInfo3));

			MakeOutgoingFrame(pBeaconFrame + FrameLen,         &TmpLen,
							1,                             &WpaIe,
							1,                             &epigram_ie_len,
							4,                             &BROADCOM_AHTINFO[0],
							HtLen1,                        &addHTInfoTmp,
							END_OF_ARGS);
#endif
			FrameLen += TmpLen;
		}

	}

	/* P802.11n_D1.10 */
	/* 7.3.2.27 Extended Capabilities IE */
	/* HT Information Exchange Support */
	if (WMODE_CAP_N(pAd->CommonCfg.PhyMode)
		&& ((pAd->CommonCfg.HT_DisallowTKIP == FALSE) 
		|| (pAd->CommonCfg.HT_DisallowTKIP 
			&& !IS_INVALID_HT_SECURITY(wdev->WepStatus))) 
		) /*wayne_note */
	{
		ULONG TmpLen;
		UCHAR ExtCapIe[3] = {IE_EXT_CAPABILITY, 1, 0x01};
		MakeOutgoingFrame(pBeaconFrame+FrameLen, &TmpLen,
							3,                   ExtCapIe,
							END_OF_ARGS);
		FrameLen += TmpLen;

	}
#endif /* DOT11_N_SUPPORT */

   	/* add Ralink-specific IE here - Byte0.b0=1 for aggregation, Byte0.b1=1 for piggy-back */
   	if ((pAd->CommonCfg.HT_DisallowTKIP == FALSE) || (pAd->CommonCfg.HT_DisallowTKIP && !IS_INVALID_HT_SECURITY(wdev->WepStatus)))	
	{/*wayne_note */
		ULONG TmpLen;
		UCHAR RalinkSpecificIe[9] = {IE_VENDOR_SPECIFIC, 7, 0x00, 0x0c, 0x43, 0x00, 0x00, 0x00, 0x00};

		if (pAd->CommonCfg.bAggregationCapable)
			RalinkSpecificIe[5] |= 0x1;
		if (pAd->CommonCfg.bPiggyBackCapable)
			RalinkSpecificIe[5] |= 0x2;
#ifdef DOT11_N_SUPPORT
		if (pAd->CommonCfg.bRdg)
			RalinkSpecificIe[5] |= 0x4;
#endif /* DOT11_N_SUPPORT */
		MakeOutgoingFrame(pBeaconFrame+FrameLen, &TmpLen,
							9,                   RalinkSpecificIe,
							END_OF_ARGS);
		FrameLen += TmpLen;
	}
	
	/* Insert MeshIDIE and MeshConfigurationIE in Beacon frame */
	/*if (MeshValid(&pAd->MeshTab)) */
	{
		pAd->MeshTab.MeshCapability.field.AcceptPeerLinks = MeshAcceptPeerLink(pAd);
		InsertMeshIdIE(pAd, pBeaconFrame+FrameLen, &FrameLen);
		InsertMeshConfigurationIE(pAd, pBeaconFrame+FrameLen, &FrameLen, FALSE);
		InsertMeshHostNameIE(pAd, pBeaconFrame+FrameLen, &FrameLen);
	}
	
	/* Insert MSCIE */
	InsertMSCIE(pAd, pBeaconFrame+FrameLen, &FrameLen);
	
	/* */
	/* step 6. Since FrameLen may change, update TXWI. */
	/* */
	/* Update in real buffer */
	/* Update sw copy. */
	if (pAd->CommonCfg.Channel <= 14)
		BeaconTransmit.word = 0;
	else
		BeaconTransmit.word = 0x4000;
	
	NdisZeroMemory((UCHAR *)&mac_info, sizeof(mac_info));	
	mac_info.FRAG = FALSE;
	
	mac_info.CFACK = FALSE;
	mac_info.InsTimestamp = TRUE;
	mac_info.AMPDU = FALSE;
	
	mac_info.BM = 1;
	mac_info.Ack = FALSE;
	mac_info.NSeq = TRUE;
	mac_info.BASize = 0;
	
	mac_info.WCID = RESERVED_WCID;
	mac_info.Length = FrameLen;
	mac_info.PID = PID_MGMT;
	
	mac_info.TID = QID_MGMT;
	mac_info.TxRate = 0;
	mac_info.Txopmode = IFS_HTTXOP;
	mac_info.Preamble = LONG_PREAMBLE;
	// TODO: shiang-MT7603
	mac_info.q_idx = Q_IDX_BCN;

	mac_info.SpeEn = 1;

	write_tmac_info(pAd, (UCHAR *)&pAd->BeaconTxWI, &mac_info, &BeaconTransmit);

	/*
		step 7. move BEACON TXWI and frame content to on-chip memory
	*/
	RT28xx_UpdateBeaconToAsic(pAd, idx + MIN_NET_DEVICE_FOR_MESH, FrameLen, UpdatePos);
}

VOID MeshCleanBeaconFrame(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			idx) 
{
	INT i;
	UINT8 TXWISize = pAd->chipCap.TXWISize;


	/* when the ra interface is down, do not send its beacon frame */
	/* clear all zero */
	for(i=0; i < TXWISize; i+=4)
		RTMP_CHIP_UPDATE_BEACON(pAd, pAd->BeaconOffset[idx] + i, 0, 4);



}

/*
	==========================================================================
	Description:
	Note: 
		BEACON frame in shared memory should be built ok before this routine
		can be called. Otherwise, a garbage frame maybe transmitted out every
		Beacon period.
 
	==========================================================================
 */
VOID AsicEnableMESHSync(
	IN PRTMP_ADAPTER pAd)
{
	BCN_TIME_CFG_STRUC csr;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): Not support for HIF_MT yet!\n",
					__FUNCTION__));
		return FALSE;
	}

	RTMP_IO_READ32(pAd, BCN_TIME_CFG, &csr.word);

#ifdef RELEASE_EXCLUDE
	/* */
	/* For Wi-Fi faily generated beacons between participating stations. */
	/* Set TBTT phase adaptive adjustment step to 8us (default 16us) */
	/* don't change settings 2006-5- by Jerry */
	/* RTMP_IO_WRITE32(pAd, TBTT_SYNC_CFG, 0x00001010); */
#endif /* RELEASE_EXCLUDE */
	
	/* start sending BEACON */
	csr.field.BeaconInterval = pAd->CommonCfg.BeaconPeriod << 4; /* ASIC register in units of 1/16 TU */
	csr.field.bTsfTicking = 1;
	csr.field.TsfSyncMode = 3; /* sync TSF in IBSS mode */
	csr.field.bTBTTEnable = 1;
	csr.field.bBeaconGen = 1;
	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr.word);
}


#ifdef CONFIG_STA_SUPPORT
BOOLEAN MeshWirelessForward(
	IN RTMP_ADAPTER *pAd,
	IN PNDIS_PACKET pPacket,
	IN ULONG wdev_idx)
{
	BOOLEAN bAnnounce, bDirectForward;
	UCHAR *pHeader802_3;
	PNDIS_PACKET pForwardPacket = NULL;
	PNDIS_PACKET pMeshForwardPacket = NULL;
	struct wifi_dev *wdev;
	
	ASSERT(wdev_idx < WDEV_NUM_MAX);

	if (wdev_idx >= WDEV_NUM_MAX)
		return FALSE;

	wdev = pAd->wdev_list[wdev_idx];
	if (wdev->wdev_type != WDEV_TYPE_MESH)
		return TRUE;

	ASSERT(wdev->func_idx < MAX_MESH_LINKS);
	pHeader802_3 = GET_OS_PKT_DATAPTR(pPacket);
	MeshDataPktProcess(pAd, pPacket, wdev->func_idx,
							&pMeshForwardPacket, &bDirectForward, &bAnnounce);

	if (bDirectForward)
	{
		LONG RouteId;

		/* build an NDIS packet */
		if ((bDirectForward == TRUE) && (bAnnounce == TRUE))
			pForwardPacket = pMeshForwardPacket;
		else
			pForwardPacket = DuplicatePacket(wdev->if_dev, pPacket);

		if (pForwardPacket == NULL)
			return bAnnounce;

		RouteId = PathRouteIDSearch(pAd, pHeader802_3);
			if (RouteId == BMCAST_ROUTE_ID)
			{
				MeshClonePacket(pAd, pForwardPacket, MESH_FORWARD, wdev->func_idx);
				/* release packet */
				RELEASE_NDIS_PACKET(pAd, pForwardPacket, NDIS_STATUS_SUCCESS);
			}
			else if (RouteId >= 0)
			{
				INT LinkId;

				LinkId = PathMeshLinkIDSearch(pAd, RouteId);
				if ((LinkId >= 0) && VALID_MESH_LINK_ID(LinkId))
				{
					RTMP_SET_PACKET_WDEV(pForwardPacket, wdev_idx);
					RTMP_SET_PACKET_MOREDATA(pForwardPacket, FALSE);
					RTMP_SET_PACKET_WCID(pForwardPacket,
											pAd->MeshTab.MeshLink[LinkId].Entry.MacTabMatchWCID);
					RTMP_SET_MESH_ROUTE_ID(pForwardPacket, RouteId);
					RTMP_SET_MESH_SOURCE(pForwardPacket, MESH_FORWARD);
					STASendPacket(pAd, pForwardPacket);
				}
				else
				{
					/* release packet */
					RELEASE_NDIS_PACKET(pAd, pForwardPacket, NDIS_STATUS_FAILURE);
				}
			}
			else
			{
				/* entity is not exist. */
				/* start path discovery. */
				MeshCreatePreqAction(pAd, NULL, pHeader802_3);

				/* release packet */
				RELEASE_NDIS_PACKET(pAd, pForwardPacket, NDIS_STATUS_FAILURE);
			}

		RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, WCID_ALL, MAX_TX_PROCESS);
	}
	
	return bAnnounce;
}
#endif /* CONFIG_STA_SUPPORT */


UINT32 MeshAirLinkTime(
	IN PRTMP_ADAPTER pAd,
	IN PMAC_TABLE_ENTRY pMacEntry,
	IN HTTRANSMIT_SETTING HTTxMode)
{
#ifdef RELEASE_EXCLUDE
	/*
		metric = (Phy-Overhead + (Bt / Rate))(1 /(1 - Pkt-Error-Rate))
		Rate: (Mbit / sescon).
		Pkt Error Rate: 
		Bt: 8192.
		Phy-Overhead: 
		   CCK: 75us + 110 us
		   OFDM: 335us + 364us
	*/
#endif /* RELEASE_EXCLUDE */

typedef struct __TX_RATE
{
	UINT8 PhyMode;
	UINT8 MCS;
	UINT8 RateIdx;
	ULONG DataRate;
} _TX_RATE;

_TX_RATE TxRate[] =
{
	{MODE_CCK,    0,   0, 10}, /* 1 Mbps */
	{MODE_CCK,    1,   1, 20}, /* 2 Mbps */
	{MODE_CCK,    2,   2, 55}, /* 5.5 Mbps */
	{MODE_CCK,    3,   3, 110}, /* 11 Mbps */

	{MODE_OFDM,   0,   4, 60}, /* 6 Mbps */
	{MODE_OFDM,   1,   5, 90}, /* 9 Mbps */
	{MODE_OFDM,   2,   6, 120}, /* 12 Mbps */
	{MODE_OFDM,   3,   7, 180}, /* 18 Mbps */
	{MODE_OFDM,   4,   8, 240}, /* 24 Mbps */
	{MODE_OFDM,   5,   9, 360}, /* 36 Mbps */
	{MODE_OFDM,   6,  10, 480}, /* 48 Mbps */
	{MODE_OFDM,   7,  11, 540}, /* 54 Mbps */

#ifdef DOT11_N_SUPPORT
	{MODE_HTMIX,  0,  12, 65}, /* 6.5 Mbps */
	{MODE_HTMIX,  1,  13, 130}, /* 13 Mbps */
	{MODE_HTMIX,  2,  14, 195}, /* 19.5 Mbps */
	{MODE_HTMIX,  3,  15, 260}, /* 26 Mbps */
	{MODE_HTMIX,  4,  16, 390}, /* 39 Mbps */
	{MODE_HTMIX,  5,  17, 520}, /* 52 Mbps */
	{MODE_HTMIX,  6,  18, 585}, /* 58.5 Mbps */
	{MODE_HTMIX,  7,  19, 650}, /* 65 Mbps */
	{MODE_HTMIX,  8,  20, 130}, /* 13 Mbps */
	{MODE_HTMIX,  9,  21, 260}, /* 26 Mbps */
	{MODE_HTMIX, 10,  22, 390}, /* 39 Mbps */
	{MODE_HTMIX, 11,  23, 520}, /* 52 Mbps */
	{MODE_HTMIX, 12,  24, 780}, /* 78 Mbps */
	{MODE_HTMIX, 13,  25, 1040}, /* 104 Mbps */
	{MODE_HTMIX, 14,  26, 1170}, /* 117 Mbps */
	{MODE_HTMIX, 15,  27, 1300}, /* 130 Mbps */
#endif /* DOT11_N_SUPPORT */
};
#define _TX_RATE_TAB_SIZE (sizeof(TxRate) /  sizeof(_TX_RATE))

	INT loopIdx;
	ULONG Rate = 10;
	ULONG ErrRate = 0;
	UINT PhyMode;
	ULONG Metric;
	ULONG PhyOverHead;

	PhyMode = HTTxMode.field.MODE;
#ifdef DOT11_N_SUPPORT
	PhyMode = (HTTxMode.field.MODE > MODE_HTMIX) ?
					MODE_HTMIX : HTTxMode.field.MODE;
#endif /* DOT11_N_SUPPORT */

	for (loopIdx = 0; loopIdx < _TX_RATE_TAB_SIZE; loopIdx++)
	{
		if ((TxRate[loopIdx].PhyMode == HTTxMode.field.MODE)
			&& (TxRate[loopIdx].MCS == HTTxMode.field.MCS))
		{
			Rate = TxRate[loopIdx].DataRate;
			if (pMacEntry != NULL)
				ErrRate = pMacEntry->PER[TxRate[loopIdx].RateIdx] >= 100
							? 99 : pMacEntry->PER[TxRate[loopIdx].RateIdx];
			else
				ErrRate = 0;
			break;
		}
	}

#ifdef DOT11_N_SUPPORT
	if ((PhyMode > MODE_OFDM)
			&& (HTTxMode.field.BW == 1))
	{
		Rate *= 2;
	}
#endif /* DOT11_N_SUPPORT */

	if (PhyMode == MODE_CCK)
		PhyOverHead = 335 + 346;
	else
		PhyOverHead = 75 + 110;

	Metric = (PhyOverHead + (8192 * 10 / Rate)) * (100 /(100 - ErrRate));

	return Metric;
}

UINT32 MESH_LinkMetricUpdate(
	IN PRTMP_ADAPTER pAd,
	IN PCHAR pDestAddr)
{
	PMAC_TABLE_ENTRY pEntry;
	HTTRANSMIT_SETTING HTTxMode;
	ULONG Idx = GetMeshLinkId(pAd, pDestAddr);

	pEntry = MacTableLookup(pAd, (PUCHAR)pDestAddr);
	if (pEntry == NULL)
		HTTxMode.word = pAd->CommonCfg.MlmeTransmit.word;
	else 
		HTTxMode.word = pEntry->HTPhyMode.word;

	if (Idx < MAX_MESH_LINKS)
		pAd->MeshTab.MeshLink[Idx].Entry.Metrics =
			MeshAirLinkTime(pAd, pEntry, HTTxMode);

	return pAd->MeshTab.MeshLink[Idx].Entry.Metrics;
}

VOID EnqueuePeerLinkMetricReport(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pDestAddr,
	IN UINT32 Metric)
{
	HEADER_802_11 MeshHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen;
	MESH_FLAG MeshFlag;
	UINT32 MeshSeq = INC_MESH_SEQ(pAd->MeshTab.MeshSeq);

	NStatus = MlmeAllocateMemory(pAd, (PVOID)&pOutBuffer);  /*Get an unused nonpaged memory */
	if(NStatus != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s() allocate memory failed \n", __FUNCTION__));
		return;
	}

	MeshHeaderInit(pAd, &MeshHdr,
		pDestAddr,		/* addr1 */
		pAd->MeshTab.wdev.if_addr,							/* addr2 */
		ZERO_MAC_ADDR);		/* addr3 */
	NdisMoveMemory(pOutBuffer, (PCHAR)&MeshHdr, sizeof(HEADER_802_11));
	FrameLen = sizeof(HEADER_802_11);

	/* Mesh Header */
	MeshFlag.word = 0;
	MeshFlag.field.AE = 0;	/* Peer-Link manager frame never carry 6 addresses. */
	InsertMeshHeader(pAd, (pOutBuffer + FrameLen), &FrameLen, MeshFlag.word,
		pAd->MeshTab.TTL, MeshSeq, NULL, NULL, NULL);

	/* Action field */
	InsertMeshActField(pAd, (pOutBuffer + FrameLen), &FrameLen, CATEGORY_MESH_LINK_METRIC, ACT_CODE_LINK_METRIC_REP);

	/* Insert Link Metric Report IE */
	InsertLinkMetricReportIE(pAd, (pOutBuffer+FrameLen), &FrameLen, Metric);

	MiniportMMRequest(pAd, QID_AC_BE, pOutBuffer, FrameLen);
	MlmeFreeMemory(pAd, pOutBuffer);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: Dest address(%02x:%02x:%02x:%02x:%02x:%02x)\n",
		__FUNCTION__, pDestAddr[0], pDestAddr[1], pDestAddr[2], 
		pDestAddr[4], pDestAddr[4], pDestAddr[5]));

	return;
}

VOID MeshPeerLinkMetricReportProcess(
	IN PRTMP_ADAPTER pAd,
	IN RX_BLK *pRxBlk)
{
	PHEADER_802_11 pHeader = (PHEADER_802_11)pRxBlk->pHeader;
	PUCHAR pFrame;
	ULONG FrameLen;
	UINT MeshLinkId = 0;
	UINT32 LinkMetric;

	do 
	{
		ULONG Idx;
		UCHAR MeshHdrLen;

		Idx = GetMeshLinkId(pAd, (PCHAR)pHeader->Addr2);
		if (Idx == BSS_NOT_FOUND)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s() Link Metric Report Ignore, PeerLink Not found.\n", __FUNCTION__));
			return;
		}
		MeshLinkId = (UINT)Idx;

		MeshHdrLen = GetMeshHederLen(pRxBlk->pData);
		/* skip Mesh Header */
		pRxBlk->pData += MeshHdrLen;
		pRxBlk->DataSize -= MeshHdrLen;

		/* skip Category and ActionCode */
		pFrame = (PUCHAR)(pRxBlk->pData + 2);
		FrameLen = pRxBlk->DataSize - 2;

		MeshLinkMetricReportSanity(	pAd,
							pFrame,
							FrameLen,
							&LinkMetric);

		DBGPRINT(RT_DEBUG_TRACE, ("%s: LinkMetric=%d, PeerMac=%02x:%02x:%02x:%02x:%02x:%02x\n",
			__FUNCTION__, LinkMetric, pHeader->Addr2[0], pHeader->Addr2[1],
			pHeader->Addr2[2], pHeader->Addr2[3], pHeader->Addr2[4], pHeader->Addr2[5]));

		pAd->MeshTab.MeshLink[Idx].Entry.Metrics = LinkMetric;
	} while (FALSE);

	return;
}

#ifdef RTMP_MAC_USB
VOID MeshTimerInit(
	IN PRTMP_ADAPTER pAd)
{
#ifndef BCN_OFFLOAD_SUPPORT
	/* for OS_ABL */
	RTMPInitTimer(pAd, &pAd->CommonCfg.BeaconUpdateTimer, GET_TIMER_FUNCTION(BeaconUpdateExec), pAd, TRUE);
#endif
}
#endif /* RTMP_MAC_USB */

VOID MeshCfgInit(RTMP_ADAPTER *pAd, RTMP_STRING *pHostName)
{
	INT	i;

	/* default configuration of Mesh. */
	pAd->MeshTab.OpMode = MESH_MP;

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		pAd->MeshTab.OpMode |= MESH_AP;
#endif /* CONFIG_AP_SUPPORT */

	pAd->MeshTab.PathProtocolId = MESH_HWMP;
	pAd->MeshTab.PathMetricId = MESH_AIRTIME;
	pAd->MeshTab.ContgesionCtrlId = NULL_PROTOCOL;
	pAd->MeshTab.TTL = MESH_TTL;
	pAd->MeshTab.MeshMaxTxRate = 0;
	pAd->MeshTab.MeshMultiCastAgeOut = MULTIPATH_AGEOUT;
	pAd->MeshTab.UCGEnable = FALSE;
	pAd->MeshTab.MeshCapability.field.Forwarding = 1;
	if (pAd->MeshTab.MeshIdLen == 0)
	{
		pAd->MeshTab.MeshIdLen = strlen(DEFAULT_MESH_ID);
		NdisMoveMemory(pAd->MeshTab.MeshId, DEFAULT_MESH_ID, pAd->MeshTab.MeshIdLen);
	}

	/* initialize state */
	pAd->MeshTab.EasyMeshSecurity = TRUE;	/* Default is TRUE for CMPC */
	pAd->MeshTab.bInitialMsaDone = FALSE;
	pAd->MeshTab.bKeyholderDone  = FALSE;
	pAd->MeshTab.bConnectedToMKD = FALSE;
	pAd->MeshTab.MeshOnly = FALSE;

	pAd->MeshTab.wdev.bAutoTxRateSwitch = TRUE;
	pAd->MeshTab.wdev.DesiredTransmitSetting.field.MCS = MCS_AUTO;

	for (i = 0; i < MAX_MESH_LINKS; i++)
		NdisZeroMemory(&pAd->MeshTab.MeshLink[i].Entry, sizeof(MESH_LINK_ENTRY));

	if (strlen(pHostName) > 0)
	{
		if (strlen(pHostName) < MAX_HOST_NAME_LEN)
			strcpy((RTMP_STRING *) pAd->MeshTab.HostName, pHostName);
		else
			strncpy((RTMP_STRING *) pAd->MeshTab.HostName, pHostName, MAX_HOST_NAME_LEN-1);
	}
	else
		strcpy((RTMP_STRING *) pAd->MeshTab.HostName, DEFAULT_MESH_HOST_NAME);

}


VOID MeshInit(
	IN PRTMP_ADAPTER pAd,
	IN RTMP_OS_NETDEV_OP_HOOK *pNetDevOps)
{
#define MESH_MAX_DEV_NUM	32
	PNET_DEV	new_dev_p;
	struct wifi_dev *wdev;

	
	/* sanity check to avoid redundant virtual interfaces are created */
	if (pAd->flg_mesh_init != FALSE)
		return;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s --->\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */

	wdev = &pAd->MeshTab.wdev;

	/* create virtual network interface */
	do {
		UINT32 MC_RowID = 0, IoctlIF = 0;
		char *dev_name;

#ifdef MULTIPLE_CARD_SUPPORT
		MC_RowID = pAd->MC_RowID;
#endif /* MULTIPLE_CARD_SUPPORT */
#ifdef HOSTAPD_SUPPORT
		IoctlIF = pAd->IoctlIF;
#endif /* HOSTAPD_SUPPORT */

		dev_name = get_dev_name_prefix(pAd, INT_MESH);
		new_dev_p = RtmpOSNetDevCreate(MC_RowID, &IoctlIF, INT_MESH, 0, sizeof(struct mt_dev_priv), dev_name);
#ifdef HOSTAPD_SUPPORT
		pAd->IoctlIF = IoctlIF;
#endif /* HOSTAPD_SUPPORT */
		if (new_dev_p == NULL)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Allocate network device fail (MESH)...\n"));
			break;
		}

		wdev->wdev_type = WDEV_TYPE_MESH;
		wdev->func_dev = &pAd->MeshTab;
		wdev->func_idx = 0;
		wdev->sys_handle = (void *)pAd;
		wdev->if_dev = new_dev_p; 
		wdev->tx_pkt_allowed = MeshAllowToSendPacket;
		// TODO: shiang-usw, modify this to MeshSendPacket!
		wdev->tx_pkt_handle = APSendPacket;
		wdev->wdev_hard_tx = APHardTransmit;
		wdev->rx_pkt_foward = mesh_rx_foward_handle;

		RTMP_OS_NETDEV_SET_PRIV(new_dev_p, pAd);
		RTMP_OS_NETDEV_SET_WDEV(new_dev_p, wdev);

		if (rtmp_wdev_idx_reg(pAd, wdev) < 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("Assign wdev idx for %s failed, free net device!\n",
						RTMP_OS_NETDEV_GET_DEVNAME(new_dev_p)));
			RtmpOSNetDevFree(new_dev_p);
			break;
		}

		/* init MAC address of virtual network interface */
		COPY_MAC_ADDR(wdev->if_addr, pAd->CurrentAddress);
#ifdef CONFIG_AP_SUPPORT
		if (pAd->chipCap.MBSSIDMode == MBSSID_MODE1)
		{
		if (pAd->ApCfg.BssidNum > 0) 
		{
			/* 	
				Refer to HW definition - 
					Bit1 of MAC address Byte0 is local administration bit 
					and should be set to 1 in extended multiple BSSIDs'
					Bit3~ of MAC address Byte0 is extended multiple BSSID index.
			 */ 
			wdev->if_addr[0] += 2; 	
			wdev->if_addr[0] += ((pAd->ApCfg.BssidNum - 1) << 2);
		}
		}
		else
		{
			wdev->if_addr[MAC_ADDR_LEN - 1] =
					(wdev->if_addr[MAC_ADDR_LEN - 1] + pAd->ApCfg.BssidNum) & 0xFF;
		}
#endif /* CONFIG_AP_SUPPORT */

#if 0 /* os able move */
		/* init operation functions */
#ifdef CONFIG_STA_SUPPORT
#if WIRELESS_EXT >= 12
		if (pAd->OpMode == OPMODE_STA)
		{
			pNetDevOps->iw_handler = (void *)&rt28xx_iw_handler_def;
		}
#endif /*WIRELESS_EXT >= 12 */
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_APSTA_MIXED_SUPPORT
#if WIRELESS_EXT >= 12
		if (pAd->OpMode == OPMODE_AP)
		{
			pNetDevOps->iw_handler = &rt28xx_ap_iw_handler_def;
		}
#endif /*WIRELESS_EXT >= 12 */
#endif /* CONFIG_APSTA_MIXED_SUPPORT */
#endif /* 0 */

		pNetDevOps->priv_flags = INT_MESH; /* we are virtual interface */
		pNetDevOps->needProtcted = TRUE;
		pNetDevOps->wdev = wdev;
		NdisMoveMemory(&pNetDevOps->devAddr[0], &wdev->if_addr[0], MAC_ADDR_LEN);
		
		/* register this device to OS */
		RtmpOSNetDevAttach(pAd->OpMode, new_dev_p, pNetDevOps);
	} while(FALSE);

	/* Initialize Mesh configuration */
	/*MeshCfgInit(pAd, pHostName); */

	/* initialize Mesh Tables and allocate spin locks */
	NdisAllocateSpinLock(pAd, &pAd->MeshTabLock);

	NeighborTableInit(pAd);
	BMPktSigTabInit(pAd);
	MultipathPoolInit(pAd);

	MeshRoutingTable_Init(pAd);
	MeshEntryTable_Init(pAd);
	MeshProxyEntryTable_Init(pAd);

#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_USB
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		MeshTimerInit(pAd);
/*		RTMPInitTimer(pAd, &pAd->CommonCfg.BeaconUpdateTimer, GET_TIMER_FUNCTION(BeaconUpdateExec), pAd, TRUE); */

		RTUSBBssBeaconInit(pAd);
	}
#endif /* RTMP_MAC_USB */
#endif /* CONFIG_STA_SUPPORT */

	pAd->flg_mesh_init = TRUE;
}


INT MESH_OpenPre(PNET_DEV pDev)
{
	PRTMP_ADAPTER pAd;


	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);
	if (ADHOC_ON(pAd))
		return -1;

	pAd->MeshTab.bcn_buf.bBcnSntReq = TRUE;
	return 0;
}


INT MESH_OpenPost(PNET_DEV pDev)
{
	PRTMP_ADAPTER pAd;


	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);

	/* Statup Mesh Protocol Stack. */
	MeshUp(pAd);

#ifdef CONFIG_STA_SUPPORT
	AsicSetPreTbtt(pAd, TRUE);
	AsicEnableMESHSync(pAd);
#endif /* CONFIG_STA_SUPPORT */
	return 0;
}


INT MESH_Close(PNET_DEV pDev)
{
	PRTMP_ADAPTER pAd;


	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);
	MeshDown(pAd, TRUE);

	pAd->MeshTab.bcn_buf.bBcnSntReq = FALSE;

#ifdef CONFIG_STA_SUPPORT
	/* Disable pre-tbtt interrupt */
	AsicSetPreTbtt(pAd, FALSE);

	/*update beacon Sync */
	/*if rausb0 is up => stop beacon */
	/*if rausb0 is down => we will call AsicDisableSync() in usb_rtusb_close_device() */
	if (INFRA_ON(pAd))
		AsicEnableBssSync(pAd, pAd->CommonCfg.BeaconPeriod);
	else if (ADHOC_ON(pAd))
		AsicEnableIbssSync(pAd);
	else
		AsicDisableSync(pAd);
#endif /* CONFIG_STA_SUPPORT */

	pAd->MeshTab.bcn_buf.bBcnSntReq = FALSE;

#ifdef CONFIG_AP_SUPPORT
	APMakeAllBssBeacon(pAd);
	APUpdateAllBeaconFrame(pAd);
#endif /* CONFIG_AP_SUPPORT */
	return 0;
}

#endif /* MESH_SUPPORT */

