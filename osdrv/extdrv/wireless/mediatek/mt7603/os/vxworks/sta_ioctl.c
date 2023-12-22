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
    sta_ioctl.c

    Abstract:
    IOCTL related subroutines

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/

#include	"rt_config.h"



INT rt28xx_sta_ioctl(
	IN	END_OBJ	*endDev, 
	IN	int		cmd, 
	IN	caddr_t	data)
{
	POS_COOKIE			pObj;
	RTMP_ADAPTER        *pAd = NULL;
	INT					Status = NDIS_STATUS_SUCCESS;
	UINT32 subcmd;
	struct iwreq *pPrivCmd;


	pAd = (RTMP_ADAPTER *)endDev->devObject.pDevice;
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	DBGPRINT(RT_DEBUG_TRACE, ("rt28xx_sta_ioctl(): IOCTL, \n"));
	
	switch(cmd)
	{
		case VX_RT_PRIV_IOCTL:
		case VX_RT_PRIV_IOCTL_EXT:
			{
				UINT32 subcmd;
				struct iwreq *pPrivCmd;

				pPrivCmd = (struct iwreq *)data;
				subcmd = pPrivCmd->u.data.flags;
				if (subcmd & OID_GET_SET_TOGGLE)
					Status = RTMPSetInformation(pAd, pPrivCmd, subcmd);
				else
					Status = RTMPQueryInformation(pAd, pPrivCmd, subcmd);
				break;
			}
	}
	return Status;
	
}

