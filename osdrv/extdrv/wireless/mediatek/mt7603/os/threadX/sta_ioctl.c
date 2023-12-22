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

#include "rt_config.h"

int iwpriv_set_cmd(
	IN RTMP_ADAPTER *pAd, 
	IN UCHAR *pCmdStr)
{
	RTMP_STRING *this_char = pCmdStr;
	RTMP_STRING *value;
	INT Status = 0;

	if (!*this_char)
		return EINVAL;
	                                                                                                                            
	if ((value = rtstrchr(this_char, '=')) != NULL)
	    *value++ = 0;
	                                                                                                                            
	if (!value && (strcmp(this_char, "SiteSurvey") != 0))                                                                                                      
	    return EINVAL;                                                                                                                  
	else
		goto SET_PROC;

	/* reject setting nothing besides ANY ssid(ssidLen=0) */
    if (!*value && (strcmp(this_char, "SSID") != 0))
        return EINVAL;

SET_PROC:
	Status = RTMPSTAPrivIoctlSet(pAd, this_char, value);
		
    return -Status;
}


int iwpriv_cmd_handle(RTMP_IWPRIV_CMD_STRUCT *pRtmpShellCmd)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;
	struct iwreq  iwReq;
	RTMP_STRING *pParam = pRtmpShellCmd->pParam;
	RTMP_STRING *pCmd = pRtmpShellCmd->pCmd;
	UINT32 Value;
	INT32 Status = 0;
	
		/* Check the cmd type, currently we only support set/show/mac/bpp/e2p */
	if (strcmp(pCmd, "set") == 0)
	{
		Status = iwpriv_set_cmd(pAd, pParam);
	}
	else if (strcmp(pCmd, "show") == 0)
	{
		/*Result = iwpriv_show_cmd(pAd, pParam); */
	}
#ifdef DBG
	else if (strcmp(pCmd, "mac") == 0)
	{
		iwReq.u.data.pointer = pParam;
		iwReq.u.data.length= strlen(pParam);
		/*RTMPIoctlMAC(pAd, &iwReq); */
	}
	else if (strcmp(pCmd, "bbp") == 0)
	{
		iwReq.u.data.pointer = pParam;
		iwReq.u.data.length= strlen(pParam);
		/*RTMPIoctlBBP(pAd, &iwReq); */
	}
	else if (strcmp(pCmd, "e2p") == 0)
	{
		iwReq.u.data.pointer = pParam;
		iwReq.u.data.length= strlen(pParam);
		/*RTMPIoctlE2PROM(pAd, &iwReq); */
	}
#ifdef RTMP_RF_RW_SUPPORT
	else if (strcmp(pCmd, "rf") == 0)
	{
		iwReq.u.data.pointer = pParam;
		iwReq.u.data.length= strlen(pParam);
		/*RTMPIoctlRF(pAd, &iwReq); */
	}
#endif /* RTMP_RF_RW_SUPPORT */
#endif /* DBG */
#ifdef WSC_INCLUDED
	else if (strcmp(pCmd, "wsc_cred_count") == 0)
	{

	}
	else if (strcmp(pCmd, "wsc_cred_ssid") == 0)
	{

	}
	else if (strcmp(pCmd, "wsc_cred_auth") == 0)
	{
	
	}
	else if (strcmp(pCmd, "wsc_cred_encr") == 0)
	{

	}
	else if (strcmp(pCmd, "wsc_cred_keyIdx") == 0)
	{


	}
	else if (strcmp(pCmd, "wsc_cred_key") == 0)
	{

	}
	else if (strcmp(pCmd, "wsc_cred_mac") == 0)
	{

	}
	else if (strcmp(pCmd, "wsc_conn_by_idx") == 0)
	{

	}
	else if (strcmp(pCmd, "wsc_auto_conn") == 0)
	{

	}
	else if (strcmp(pCmd, "wsc_conf_mode") == 0)
	{
		Value = atoi(pParam);
		if (Value == 2)
			Value = 4;
		
		DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_CONF_MODE, value = %d\n", __FUNCTION__, Value));

		switch (Value)
		{
			case WSC_DISABLE:
				Set_WscConfMode_Proc(pAd, "0");
				break;
			case WSC_ENROLLEE:
				Set_WscConfMode_Proc(pAd, "1");
				break;
			case WSC_REGISTRAR:
				Set_WscConfMode_Proc(pAd, "2");
				break;
			default:
				Status = -1;
				break;
		} /* end of switch */
	}
	else if (strcmp(pCmd, "wsc_mode") == 0)
	{
        Value = atoi(pParam);
		DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_MODE, value = %d\n", __FUNCTION__, Value));
		switch(Value)
		{
        	case WSC_PIN_MODE:
				Set_WscMode_Proc(pAd, "1");
                break;
			case WSC_PBC_MODE:
                Set_WscMode_Proc(pAd, "2");
                break;
            default:
                Status = -1;
                break;
         } /* end of switch */
	}
	else if (strcmp(pCmd, "wsc_pin") == 0)
	{
		
		DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_PIN, value = (%s)\n", __FUNCTION__, pParam));
		if (!Set_WscPinCode_Proc(pAd, pParam))
					Status = -1;
	}
	else if (strcmp(pCmd, "wsc_ssid") == 0)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_SSID(%s)\n", __FUNCTION__, pParam));
		Set_WscSsid_Proc(pAd, (RTMP_STRING *) pParam);
	}
	else if (strcmp(pCmd, "wsc_start") == 0)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_START\n", __FUNCTION__));
		Set_WscGetConf_Proc(pAd, "1");
				
	}
	else if (strcmp(pCmd, "wsc_stop") == 0)
	{

	}
	else if (strcmp(pCmd, "wsc_gen_pincode") == 0)
	{

	}
	else if (strcmp(pCmd, "wsc_ap_band") == 0)
	{

	}
#endif /* WSC_INCLUDED */
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Error: Unsupported cmd:%s!\n", pCmd));
		Status = -1;
	}

	return Status;
}



INT rt28xx_sta_ioctl(
	IN	PIFACE	pi, 
	IN	int		cmd, 
	IN	char		*data)
{
	POS_COOKIE			pObj;
	RTMP_ADAPTER        *pAd = NULL;
	INT					Status = NDIS_STATUS_SUCCESS;
	UINT32 subcmd;
	struct iwreq *pPrivCmd;


	pAd = (RTMP_ADAPTER *)RTMP_OS_NETDEV_GET_PRIV(pi);
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	DBGPRINT(RT_DEBUG_TRACE, ("rt28xx_sta_ioctl(): IOCTL, \n"));
	
	return Status;
	
}

