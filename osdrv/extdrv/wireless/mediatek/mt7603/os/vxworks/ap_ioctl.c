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
	ap_ioctl.c

    Abstract:
    IOCTL related subroutines

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/

#include "rt_config.h"
#ifdef RTMP_VX_SHELL_CMD_SUPPORT
#ifdef VXWORKS_6X
#include <shellInterpCmdLib.h>
#include <private/shellInternalLibP.h>
#endif /* VXWORKS_6X */
#endif /* RTMP_VX_SHELL_CMD_SUPPORT */
#include "rtmp.h"

#ifdef RTMP_VX_SHELL_CMD_SUPPORT
typedef struct _RTMP_VX_IWPRIV_CMD_STRUCT_
{
	RTMP_STRING *pDevName;
	RTMP_STRING *pCmd;
	RTMP_STRING *pParam;
}RTMP_VX_IWPRIV_CMD_STRUCT;
#endif /* RTMP_VX_SHELL_CMD_SUPPORT */



INT rt28xx_ap_ioctl(
	IN	END_OBJ		*pEndDev, 
	IN	INT			cmd, 
	IN	PUCHAR		pData)
{
	RTMP_ADAPTER	*pAd = NULL;
	INT				retVal = 0;
	POS_COOKIE		pObj;
	UCHAR			apidx=0;
	long value;
	UINT32 subcmd;
	struct iwreq *pPrivCmd, iwReq;

		
	pAd = RTMP_OS_NETDEV_GET_PRIV(pEndDev);
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free; So the net_dev->priv will be NULL in 2rd open */
		return (EINVAL);
	}

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	switch(cmd)
	{
		case EIOCSADDR:		/* Set MAC address */
			if (pData == NULL)
				return (EINVAL);
			bcopy (pData, (char *)RTMP_OS_NETDEV_GET_PHYADDR(pEndDev), RTMP_OS_NETDEV_PHYADDR_LEN(pEndDev));
			/* TODO: Shall we copy the address to our data structure?? */
			break;
			
		case EIOCGADDR:	/* Get MAC Address */
			if (pData == NULL)
				return (EINVAL);
			bcopy ((char *)RTMP_OS_NETDEV_GET_PHYADDR(pEndDev), (char *)pData, RTMP_OS_NETDEV_PHYADDR_LEN(pEndDev));
			break;

		case EIOCSFLAGS:	/* set (or clear) flags */
			value = (long)pData;
			if (value < 0)
			{
				value = -value;
				value--;
				END_FLAGS_CLR (pEndDev, value);
			}
			else
			{
				END_FLAGS_SET (pEndDev, value);
			}
			RtmpVxIoctlConfig (pEndDev);
			break;
			
		case EIOCGFLAGS:	/* get flags */
			*(int *)pData = END_FLAGS_GET(pEndDev);
			break;
			
		case EIOCGFBUF:		/* return minimum First Buffer for chaining */
			if (pData == NULL)
				return (EINVAL);
			*(int *)pData = MGMT_DMA_BUFFER_SIZE;
			break;
			
		case EIOCPOLLSTART:
			retVal = ENOTSUP;
			break;

		case EIOCPOLLSTOP:
			retVal = ENOTSUP;
			break;
#ifndef VXWORKS_PCD10
		case EIOCGSTYLE:
			*pData = END_STYLE_END;
			break;
#endif /* VXWORKS_PCD10 */
			
		case EIOCGHDRLEN:
			if (pData == NULL)
				return (EINVAL);
			*(int *)pData = LENGTH_802_3;
			break;
		case EIOCGNAME:
			strcpy((char*)pData, END_DEV_NAME(pEndDev));
			break;

		case EIOCGMIB2:		/* return MIB information */
#ifdef VXWORKS_PCD10
			if (pData == NULL)
				return (EINVAL);
			bcopy((char *)&pEndDev->mib2Tbl, (char *)pData, sizeof(pEndDev->mib2Tbl));
#else
			retVal = endM2Ioctl(pEndDev, cmd, pData);
#endif /* VXWORKS_PCD10 */
			break;
		
		case VX_RT_PRIV_IOCTL:
		case VX_RT_PRIV_IOCTL_EXT:
			{
				pPrivCmd = (struct iwreq *)pData;
				subcmd = pPrivCmd->u.data.flags;
				if (subcmd & OID_GET_SET_TOGGLE)
					retVal = RTMPAPSetInformation(pAd, pPrivCmd, subcmd);
				else
				{
					switch(subcmd)
					{
#ifdef LLTD_SUPPORT
						case RT_OID_GET_PHY_MODE:
				            DBGPRINT(RT_DEBUG_TRACE, ("Query::Get phy mode (%02X) \n", pAd->CommonCfg.PhyMode));
				            wrqin->u.mode = (UINT32)pAd->CommonCfg.PhyMode;
							break;
#endif /* LLTD_SUPPORT */

						default:
							retVal = RTMPAPQueryInformation(pAd, pPrivCmd, subcmd);
							break;
					}
				}
				break;
			}
		
		case VX_RTPRIV_IOCTL_SET:
			{	
				retVal = RTMPAPPrivIoctlSet(pAd, (struct iwreq *)pData);
			}
			break;
		    
		case VX_RTPRIV_IOCTL_SHOW:
			{
				retVal = RTMPAPPrivIoctlShow(pAd, (struct iwreq *)pData);
			}
			break;		    

		case VX_RTPRIV_IOCTL_GET_MAC_TABLE:
			RTMPIoctlGetMacTable(pAd,(struct iwreq *)pData);
		    break;

#ifdef AP_SCAN_SUPPORT
		case VX_RTPRIV_IOCTL_GSITESURVEY:
			RTMPIoctlGetSiteSurvey(pAd,(struct iwreq *)pData);
			break;
#endif /* AP_SCAN_SUPPORT */

		case VX_RTPRIV_IOCTL_STATISTICS:
			RTMPIoctlStatistics(pAd, (struct iwreq *)pData);
			break;

		case VX_RTPRIV_IOCTL_RADIUS_DATA:
		    RTMPIoctlRadiusData(pAd, (struct iwreq *)pData);
		    break;

		case VX_RTPRIV_IOCTL_ADD_WPA_KEY:
		    RTMPIoctlAddWPAKey(pAd, (struct iwreq *)pData);
		    break;

		case VX_RTPRIV_IOCTL_ADD_PMKID_CACHE:
		    RTMPIoctlAddPMKIDCache(pAd, (struct iwreq *)pData);
			break;

		case VX_RTPRIV_IOCTL_STATIC_WEP_COPY:
		    RTMPIoctlStaticWepCopy(pAd, (struct iwreq *)pData);
			break;

#ifdef WSC_AP_SUPPORT
		case VX_RTPRIV_IOCTL_WSC_PROFILE:
			RTMPIoctlWscProfile(pAd, (struct iwreq *)pData);
			break;
#endif /* WSC_AP_SUPPORT */
#ifdef DOT11_N_SUPPORT
		case VX_RTPRIV_IOCTL_QUERY_BATABLE:
		    RTMPIoctlQueryBaTable(pAd, (struct iwreq *)pData);
		    break;
#endif /* DOT11_N_SUPPORT */

#ifdef CONFIG_ATE
#ifdef CONFIG_QA
		case VX_RTPRIV_IOCTL_ATE:
			RtmpDoAte(pAd, (struct iwreq *)pData, ((struct iwreq *)pData)->ifr_name);
			break;
#endif /* CONFIG_QA */ 
#endif /* CONFIG_ATE */

		case VX_RT_PRIV_IOCTL_PRIV:
			/* TODO: shall we copy the ap_private to user in vxworks? */
			break;

#ifdef DBG
		case VX_RTPRIV_IOCTL_BBP:
			{
				struct iwreq *pReq;
				
				pReq = (struct iwreq *)pData;
				/* When use ioctl from endIoctl by UI, it cannot dump msg to the console! */
				pReq->u.data.flags = RTPRIV_IOCTL_FLAG_NODUMPMSG;
				RTMPAPIoctlBBP(pAd, (struct iwreq *)pData);
			}
			break;
			
		case VX_RTPRIV_IOCTL_MAC:
			{
				struct iwreq *pReq;
				
				pReq = (struct iwreq *)pData;
				pReq->u.data.flags = RTPRIV_IOCTL_FLAG_NODUMPMSG;
				RTMPIoctlMAC(pAd, (struct iwreq *)pData);
			}
			break;

		case VX_RTPRIV_IOCTL_E2P:
			{
				struct iwreq *pReq;
				
				pReq = (struct iwreq *)pData;
				pReq->u.data.flags = RTPRIV_IOCTL_FLAG_NODUMPMSG;
				RTMPAPIoctlE2PROM(pAd, (struct iwreq *)pData);
			}
			break;
#ifdef RTMP_RF_RW_SUPPORT
		case VX_RTPRIV_IOCTL_RF:
			{
				struct iwreq *pReq;
				
				pReq = (struct iwreq *)pData;
				pReq->u.data.flags = RTPRIV_IOCTL_FLAG_NODUMPMSG;
				RTMPAPIoctlRF(pAd, (struct iwreq *)pData);
			}
			break;
#endif /* RTMP_RF_RW_SUPPORT */
#endif /* DBG */

#ifdef WSC_INCLUDED
		case VX_RTPRIV_IOCTL_WSC_CALLBACK:
			{
				POS_COOKIE pObj;

				pObj = (POS_COOKIE)pAd->OS_Cookie;
				pObj->WscMsgCallBack = pData;
				printf("Set pObj->WscMsgCallBack = 0x%x!\n", pData);
			}
			break;
#endif /* WSC_INCLUDED */
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("rt28xx_ap_ioctl():Not supported ioctl cmd:0x%x!\n", cmd));
			retVal = EINVAL;
			break;
	}

	return retVal;
}


/*
	Linux-like command for VxWorks Platform.
*/
#ifdef RTMP_VX_SHELL_CMD_SUPPORT

END_OBJ *RtmpVxFindDevByName(RTMP_STRING *pDevString)
{
	END_OBJ *pEndObj = NULL;
	unsigned char *pDevIdx, *pPrefixStr;
	int position, len, ifNameIdx = -1;


	ASSERT(pDevString);
	
	pPrefixStr = pDevIdx = pDevString;
	len = strlen(pDevString);
	for (position = 0; position < len; position++)
	{
		if(isdigit(pDevString[position]))
		{
			pDevIdx = &pDevString[position];
			ifNameIdx = atoi(pDevIdx);
			pDevString[position] = '\0';
			break;
		}
	}

	/*printf("pPrefixStr=%s, ifNameIdx=%d!\n", pPrefixStr, ifNameIdx);*/
	if ((ifNameIdx >=0) && strlen(pPrefixStr))
		pEndObj = endFindByName(pPrefixStr, ifNameIdx); 
	
	return pEndObj;
}


int iwpriv_ioctl(RTMP_STRING *pDevName, RTMP_STRING *pCmd, RTMP_STRING *pParam)
{
	END_OBJ * pEndObj;
	int (*endIoctl) (END_OBJ *, int, caddr_t);
	int retval;
	/*UINT32	ioctlCmd; */
	
	/* first fine end object depends on the device name */
	pEndObj = RtmpVxFindDevByName(pDevName);
	if (pEndObj == NULL)
		return -1;
	
	if (pEndObj == NULL || pEndObj->pFuncTable == NULL
		|| (endIoctl = pEndObj->pFuncTable->ioctl) == NULL)
		return ERROR;

	/* now check the sub-command OID of our priv ioctl cmd */
	if (isxdigit(*pCmd))
	{
	}
	
	
	retval = endIoctl(pEndObj, VX_RT_PRIV_IOCTL, (caddr_t)&pParam);
	if (retval == OK)
	{
		
	}

	return retval;
	
}
int iwpriv_show_cmd(
	IN RTMP_ADAPTER *pAd, 
	IN UCHAR *pCmdStr)
{
	CHAR *this_char;
	CHAR *value = NULL;
	INT status = 0;
	struct iwreq iwReq;

	iwReq.u.data.pointer = pCmdStr;
	iwReq.u.data.length = strlen(pCmdStr);
	iwReq.u.data.flags = 0;
	
	status = RTMPAPPrivIoctlShow(pAd, &iwReq);

	return 0;
}


int iwpriv_set_cmd(
	IN RTMP_ADAPTER *pAd, 
	IN UCHAR *pCmdStr)
{
	CHAR *this_char;
	CHAR *value;
	INT 	   status = 0;
	struct iwreq iwReq;

	iwReq.u.data.pointer = pCmdStr;
	iwReq.u.data.length = strlen(pCmdStr);
	iwReq.u.data.flags = 0;
	status = RTMPAPPrivIoctlSet(pAd, &iwReq);


	return 0;
}


	
int iwpriv_usage()
	{
	printf("iwpriv \"devName cmd parameters\"\n");
	printf("\tdevName: the device name you want to configure\n");
	printf("\tcmd: set/show/mac/bbp/e2p\n");
	printf("\tparamters: all original Linux iwpriv command can support now.\n");

	return 0;
	}


int iwpriv_cmd_handle(RTMP_VX_IWPRIV_CMD_STRUCT *pRtmpVxCmd)
{
	END_OBJ *pEndObj;
	RTMP_ADAPTER *pAd;
	POS_COOKIE pObj;
	int infIdx, status = 0;
	struct iwreq  iwReq;
	
	/* Check if the net_device exists */
	pEndObj = RtmpVxFindDevByName(pRtmpVxCmd->pDevName);

	if ( pEndObj == NULL)
		return -1;

	pAd = RTMP_OS_NETDEV_GET_PRIV(pEndObj);
	if (pAd == NULL)
		return -1;
	
	/* determine this ioctl command is comming from which interface. */
	if (pEndObj->pFuncTable == &RtmpMAINEndFuncTable)
	{
		pObj = (POS_COOKIE)pAd->OS_Cookie;
		pObj->ioctl_if_type = INT_MAIN;
		pObj->ioctl_if = MAIN_MBSSID;
		DBGPRINT(RT_DEBUG_INFO, ("%s(): I/F(ra%d)\n", __FUNCTION__, pObj->ioctl_if));
	}
#ifdef MBSS_SUPPORT
	else if (pEndObj->pFuncTable == &RtmpMBSSEndFuncTable)
	{
		pObj = (POS_COOKIE)pAd->OS_Cookie;
		pObj->ioctl_if_type = INT_MBSSID;
		for (infIdx = 1; infIdx < pAd->ApCfg.BssidNum; infIdx++)
		{
			if (pAd->ApCfg.MBSSID[infIdx].MSSIDDev == pEndObj)
			{
				pObj->ioctl_if = infIdx;
				DBGPRINT(RT_DEBUG_INFO, ("%s(): I/F(ra%d)\n", __FUNCTION__, infIdx));
				break;
			}
		}
		
		/* Interface not found! */
		if(infIdx == pAd->ApCfg.BssidNum)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s(): can not find I/F\n", __FUNCTION__));
			return -1;
		}
		MBSS_MR_APIDX_SANITY_CHECK(pAd, pObj->ioctl_if);
	}
#endif /* MBSS_SUPPORT */
#ifdef APCLI_SUPPORT
	else if (pEndObj->pFuncTable == &RtmpApCliEndFuncTable)
	{
		pObj = (POS_COOKIE)pAd->OS_Cookie;
		pObj->ioctl_if_type = INT_APCLI;
		for (infIdx = 0; infIdx < MAX_APCLI_NUM; infIdx++)
		{
			if (pAd->ApCfg.ApCliTab[infIdx].dev == pEndObj)
			{
				pObj->ioctl_if = infIdx;

#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("%s(): I/F(apcli%d)\n", __FUNCTION__, pObj->ioctl_if));
#endif /* RELEASE_EXCLUDE */
				break;
			}

			if(infIdx == MAX_APCLI_NUM)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): can not find I/F\n", __FUNCTION__));
				return -1;
			}
		}
		APCLI_MR_APIDX_SANITY_CHECK(pObj->ioctl_if);
	}
#endif /* APCLI_SUPPORT */
#ifdef MESH_SUPPORT
	else if (pEndObj->pFuncTable == &RtmpMeshEndFuncTable)
	{
		pObj->ioctl_if_type = INT_MESH;
		pObj->ioctl_if = 0;
	}
#endif /* MESH_SUPPORT */
    else
	{
		DBGPRINT(RT_DEBUG_WARN, ("IOCTL is not supported in WDS interface\n"));
		return -1;
	}

	
	/* Check the cmd type, currently we only support set/show/mac/bpp/e2p */
	if (strcmp(pRtmpVxCmd->pCmd, "set") ==0)
	{
		status = iwpriv_set_cmd(pAd, pRtmpVxCmd->pParam);
	}
	else if (strcmp(pRtmpVxCmd->pCmd, "show") ==0)
	{
		status = iwpriv_show_cmd(pAd, pRtmpVxCmd->pParam);
	}
#ifdef DBG
	else if (strcmp(pRtmpVxCmd->pCmd, "mac") ==0)
	{
		iwReq.u.data.pointer = pRtmpVxCmd->pParam;
		iwReq.u.data.length= strlen(pRtmpVxCmd->pParam);
		RTMPIoctlMAC(pAd, &iwReq);
	}
	else if (strcmp(pRtmpVxCmd->pCmd, "bbp") ==0)
	{
		iwReq.u.data.pointer = pRtmpVxCmd->pParam;
		iwReq.u.data.length= strlen(pRtmpVxCmd->pParam);
		RTMPAPIoctlBBP(pAd, &iwReq);	
	}
	else if (strcmp(pRtmpVxCmd->pCmd, "e2p") ==0)
	{
		iwReq.u.data.pointer = pRtmpVxCmd->pParam;
		iwReq.u.data.length= strlen(pRtmpVxCmd->pParam);
		RTMPAPIoctlE2PROM(pAd, &iwReq);
	}
#ifdef RTMP_RF_RW_SUPPORT
	else if (strcmp(pRtmpVxCmd->pCmd, "rf") ==0)
	{
		iwReq.u.data.pointer = pRtmpVxCmd->pParam;
		iwReq.u.data.length= strlen(pRtmpVxCmd->pParam);
		RTMPAPIoctlRF(pAd, &iwReq);
	}
#endif /* RTMP_RF_RW_SUPPORT */
#endif /* DBG */
	else
	{
		printf("Error: Unsupported cmd:%s!\n", pRtmpVxCmd->pCmd);
		status = -1;
	}

	
	return status;

}


int iwpriv(char *pCmd)
{
	int len, cnt, retVal = -1;
	char *pPtr, *pParamArray[3]; /* *pToken */
	
	if (pCmd == NULL)
		return -1;
	
	if ((len = strlen(pCmd)) >255)
	{
		printf("the iwpriv cmd string size too long\n");
		return -1;
	}

	NdisZeroMemory(pParamArray, sizeof(pParamArray));
	cnt = 0;
	/* Now parsing the receiving cmd string */
	pPtr = pCmd;
	while(*pPtr !='\0')
	{
		/* First remove the leading space characters*/
		while((*pPtr == 0x20) && (len > 0))
		{
			pPtr++;
			len--;
		}

		if(len && strlen(pPtr))
		{
			pParamArray[cnt] = pPtr;
			pPtr = rtstrstr(pPtr, " ");
			if (pPtr)
			{
				*pPtr = '\0';
				pPtr++;
				len = strlen(pPtr);
			}
			else
				break;
			cnt++;
		}

		if (cnt == 3)
			break;
	}

	if (cnt < 2)
	{
		iwpriv_usage();
		return -1;
	}

	retVal = iwpriv_cmd_handle((RTMP_VX_IWPRIV_CMD_STRUCT *)pParamArray);

	return retVal;
}


/*

*/
#ifdef VXWORKS_6X
int iwpriv_cmd(
	IN SHELL_OPTION options[])
{
	STATUS status;
	int opt_idx = 0;
	RTMP_VX_IWPRIV_CMD_STRUCT RtmpVxCmd;
	
	for(;;)
	{
		if (options[opt_idx].string == NULL)
			break;

		if (options[opt_idx++].isLast)
			break;
	}

	if (opt_idx < 2)
	{
		printf("Error: not enough parameters\n");
		return -1;
	}

	RtmpVxCmd.pDevName = options[0].string;
	RtmpVxCmd.pCmd = options[1].string;
	RtmpVxCmd.pParam = options[2].string;
	
	status = iwpriv_cmd_handle(&RtmpVxCmd);
	
	return status;
}



int iwconfig_cmd(
	IN SHELL_OPTION options[])
{
	STATUS status = OK;
	return 0;
}


static SHELL_CMD RTMP_VX_SHELL_CMD[]={
	{"iwpriv", 
	iwpriv_cmd, 
	"",
	"Linux-like cmd for Ralink Wireless device configuration setting",
	"%s"},
	{"iwconfig",
	iwconfig_cmd,
	""
	"Linux-like iwconfig cmd for Ralink Wireless device configuration setting",
	"%s"},
	{NULL, NULL, NULL, NULL, NULL, NULL}
};


STATUS RtmpVxShellCmdArrayAdd(void)
{
	STATUS status;

	/* Add a new topic to the shell commands */
	status = shellCmdTopicAdd ("rt_wireless",
							"List of the shell commands related to Ralink"
							"wireless configuration.");
	if (status == OK)
	{	
		/* Add shell command */
		status = shellCmdArrayAdd("rt_wireless", RTMP_VX_SHELL_CMD);
		if (status != OK)
			logMsg("%s(): Call shellCmdArrayAdd() failed: status = %d!\n", __FUNCTION__, status, 0,0,0,0);
	}
	else
		logMsg("%s(): Call shellCmdTopicAdd() failed: status = %d!\n", __FUNCTION__, status, 0,0,0,0);

	return status;
}
#endif /* VXWORKS_6X */

#endif /* RTMP_VX_SHELL_CMD_SUPPORT */

