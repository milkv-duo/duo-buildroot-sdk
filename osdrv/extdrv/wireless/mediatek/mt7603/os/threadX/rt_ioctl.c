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
    rt_ioctl.c

    Abstract:
    IOCTL related subroutines

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/

#include	"rt_config.h"

/*---------------------------------------------------------------------*/
/* Symbol & Macro Definitions                                          */
/*---------------------------------------------------------------------*/
int iwpriv(char *pCmd)
{
	int len, cnt, retVal = -1;
	char *pPtr, *pParamArray[3]; /* *pToken */
	
	if (pCmd == NULL)
		return -1;
	
	if ((len = strlen(pCmd)) >255)
	{
		dc_log_printf("the iwpriv cmd string size too long\n");
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

	retVal = iwpriv_cmd_handle((RTMP_IWPRIV_CMD_STRUCT *)pParamArray);

	return retVal;
}


int iwpriv_usage()
{
	DBGPRINT(RT_DEBUG_TRACE, ("iwpriv \"devName cmd parameters\"\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("\tdevName: the device name you want to configure\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("\tcmd: set/show/mac/bbp/e2p\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("\tparamters: all original Linux iwpriv command can support now.\n"));

	return 0;
}
