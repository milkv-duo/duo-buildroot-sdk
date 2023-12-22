/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
	rt_threadx.c

    Abstract:

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/

#include "rt_config.h"

ULONG RTDebugLevel = RT_DEBUG_TRACE;
ULONG RTDebugFunc = 0;

#ifdef VENDOR_FEATURE4_SUPPORT
ULONG	OS_NumOfMemAlloc = 0, OS_NumOfMemFree = 0;
#endif /* VENDOR_FEATURE4_SUPPORT */
#ifdef VENDOR_FEATURE2_SUPPORT
ULONG	OS_NumOfPktAlloc = 0, OS_NumOfPktFree = 0;
#endif /* VENDOR_FEATURE2_SUPPORT */

#ifdef SIGMATEL_SDK

UCHAR *RT_WIFI_DevCtrl = NULL;
static UINT32 RtmpTxBootStrapTaskStack[RTMP_TX_TASK_STACK_SIZE /sizeof(UINT32)];
static TX_THREAD RTMP_TX_BOOTSTRAP_TASK;
DC_NVRAM_OBJECT_t NVRAMWIFISetting;
DC_WIFI_MAC_CONFIGURATION WiFiMacConfDefault;
DC_WIFI_MAC_CONFIGURATION WiFiMacConfCache;
#endif /* SIGMATEL_SDK */

UCHAR   RALINK_OUI[]  = {0x00, 0x0c, 0x43};

/***********************************************************************************
 *
 *	Definition of printing, debugging and string handling related functions
 *
 ***********************************************************************************/
char *strsep(register char **stringp, register const char *delim)
{
	register char *s;
	register const char *spanp;
	register int c, sc;
	char *tok;

	if ((s = *stringp) == NULL)
		return (NULL);
	for (tok = s;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}


unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
        unsigned long result = 0,value;

        if (!base) {
                base = 10;
                if (*cp == '0') {
                        base = 8;
                        cp++;
                        if ((*cp == 'x') && isxdigit(cp[1])) {
                                cp++;
                                base = 16;
                        }
                }
        }
        while (isxdigit(*cp) &&
               (value = isdigit(*cp) ? *cp-'0' : toupper(*cp)-'A'+10) < base) {
                result = result*base + value;
                cp++;
        }
        if (endp)
                *endp = (char *)cp;
        return result;
}


long simple_strtol(const char *cp,char **endp,unsigned int base)
{
        if(*cp=='-')
                return -simple_strtoul(cp+1,endp,base);
        return simple_strtoul(cp,endp,base);
}

/**
 * kstrtol - convert a string to a long
 * @s: The start of the string. The string must be null-terminated, and may also
 *  include a single newline before its terminating null. The first character
 *  may also be a plus sign or a minus sign.
 * @base: The number base to use. The maximum supported base is 16. If base is
 *  given as 0, then the base of the string is automatically detected with the
 *  conventional semantics - If it begins with 0x the number will be parsed as a
 *  hexadecimal (case insensitive), if it otherwise begins with 0, it will be
 *  parsed as an octal number. Otherwise it will be parsed as a decimal.
 * @res: Where to write the result of the conversion on success.
 *
 * Returns 0 on success, -ERANGE on overflow and -EINVAL on parsing error.
 * Used as a replacement for the obsolete simple_strtoull. Return code must
 * be checked.
 */
inline int kstrtol(const char *s, unsigned int base, long *res)
{
	/*
	 * We want to shortcut function call, but
	 * __builtin_types_compatible_p(long, long long) = 0.
	 */
	if (sizeof(long) == sizeof(long long) &&
	    __alignof__(long) == __alignof__(long long))
		return kstrtoll(s, base, (long long *)res);
	else
		return _kstrtol(s, base, res);
}

void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen)
{
	unsigned char *pt;
	int x;
	char tmpBuf[8] = {0};

	if (RTDebugLevel < RT_DEBUG_TRACE)
		return;
	
	pt = pSrcBufVA;
	dc_log_printf("%s: %p, len = %d\n",str,  pSrcBufVA, SrcBufLen);
	for (x =0; x < SrcBufLen; x+=8)
	{
		if ((SrcBufLen -x) < 8)
		{
			memcpy(&tmpBuf[0], pt, SrcBufLen -x);
			pt = &tmpBuf[0];
		}
		
		dc_log_printf("0x%02x:%02x%02x%02x%02x %02x%02x%02x%02x",
					x, pt[0] & 0xff, pt[1] & 0xff, pt[2] & 0xff, pt[3] & 0xff,
					pt[4] & 0xff, pt[5] & 0xff, pt[6] & 0xff, pt[7] & 0xff);
		pt+=8;
	}
}


/***********************************************************************************
 *
 *	Definition of timer and time related functions
 *
 ***********************************************************************************/
/* Unify all delay routine by using udelay */
VOID RtmpusecDelay(ULONG usec)
{
#ifdef SIGMATEL_SDK
	UINT16 cycles;
	/* 
		For SigmaTel SDK, this function is a busy-waits for at least the specified number of main 
		clock cycles and will not voluntarily release the CPU to another process and then return 
		to the calling program.

		Only allow in threads.
	*/
	/* TODO: How to convert the usec to cycles?? */
	cycles = usec;
	dc_time_wait_cycles(cycles);
#endif /* SIGMATEL_SDK */
}


VOID RtmpOsMsDelay(
	IN	ULONG	msec)
{
	mdelay(msec);
}


long RtmpMSleep(IN ULONG mSec)
{
#ifdef SIGMATEL_SDK
	UINT16 cycles;
	/* 
		For SigmaTel SDK, this function is a busy-waits for at least the specified number of main 
		clock cycles and will not voluntarily release the CPU to another process and then return 
		to the calling program.

		Only allow in threads.
	*/
	/* TODO: How to convert the msec to cycles?? */
	cycles = mSec;
	dc_time_wait_cycles(cycles);

	return cycles;
#endif /* SIGMATEL_SDK */
}


void RTMP_GetCurrentSystemTime(LARGE_INTEGER *NowTime)
{
	NowTime->u.HighPart = 0;
	NowTime->u.LowPart = tx_time_get();
}


/* timeout -- ms */
VOID RTMP_SetPeriodicTimer(
	IN	NDIS_MINIPORT_TIMER *pTimer, 
	IN	unsigned long timeout)
{

	UINT status, isActive;
	char *timerName;
	ULONG remaing_ticks, resch_ticks;
	TX_TIMER *pNextTimer;

		
	status = tx_timer_info_get(pTimer, &timerName, &isActive, &remaing_ticks, &resch_ticks, &pNextTimer);
	if (status == TX_SUCCESS)
	{
		if(isActive == TX_TRUE)
			status =tx_timer_deactivate(pTimer);
		
		timeout = (timeout * OS_HZ) /1000;
		status = tx_timer_change(pTimer, timeout, 0);
		status = tx_timer_activate (pTimer);
		if (status != TX_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Add_Timer failed(%d)!\n", status));
		}
	}
	else 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("timerQuery failed(%d)!\n", status));
	}
}


/* convert NdisMInitializeTimer --> RTMP_OS_Init_Timer */
VOID RTMP_OS_Init_Timer(
	IN	VOID 					*pReserved,
	IN	NDIS_MINIPORT_TIMER		*pTimerOrg, 
	IN	TIMER_FUNCTION			function,
	IN	PVOID					data,
	IN	LIST_HEADER				*pTimerList)
{
	UINT status;
	ULONG timeout_ticks = 1, resch_ticks =0;
	
	/*
		For threadX, the timeout value should be decided when do tx_timer_create(), but for RT_WIFI, all timeout value
		should be assigned when call "RTMPSetTimer", so we set the "timeout_ticks" and "resch_ticks" dummy here.
	*/
	status = tx_timer_create(pTimer, "", function, (ULONG)data, timeout_ticks, resch_ticks, TX_NO_ACTIVATE);
	if (status != TX_SUCCESS)
	{
		PRALINK_TIMER_STRUCT pRaTimer;
		TX_TIMER *pNextTimer;
		char *timerName;
		UINT isActive;
		
		status = tx_timer_info_get(pTimer, &timerName, &isActive, &timeout_ticks, &resch_ticks, &pNextTimer);
		if (status == TX_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("timer already init-ed!\n"));
			return;
		}
		pRaTimer = (PRALINK_TIMER_STRUCT)data;
		pRaTimer->Valid = FALSE;
		DBGPRINT(RT_DEBUG_ERROR, ("init_Timer failed!\n"));
	}
}


VOID RTMP_OS_Add_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer,
	IN	unsigned long timeout)
{
	UINT status, isActive;
	char *timerName;
	ULONG remaing_ticks, resch_ticks;
	TX_TIMER *pNextTimer;

		
	status = tx_timer_info_get(pTimer, &timerName, &isActive, &remaing_ticks, &resch_ticks, &pNextTimer);
	if (status == TX_SUCCESS)
	{
		if(isActive == TX_TRUE)
			status =tx_timer_deactivate(pTimer);
		
		timeout = (timeout * OS_HZ) /1000;
		status = tx_timer_change(pTimer, timeout, 0);
		status = tx_timer_activate (pTimer);
		if (status != TX_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Add_Timer failed(%d)!\n", status));
		}
	}
	else 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("timerQuery failed(%d)!\n", status));
	}
}


VOID RTMP_OS_Mod_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer,
	IN	unsigned long timeout)
{
	RTMP_OS_Add_Timer(pTimer, timeout);
}


VOID RTMP_OS_Del_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer,
	OUT	BOOLEAN					*pCancelled)
{
	UINT status;

/*
	For threadX, there are two functions to make the timer stop, 
		=> "tx_timer_deactivate()": which make the timer deactivated.
		=> "tx_timer_delete()": which really delete the specified timer.

	Here we use tx_timer_deactivate() to implement this wrapper function because
	actually the Del_Timer() in RT_WIFI just deactivate the timer, after Del_Timer,
	RT_WIFI may re-enable this timer again.
*/

	/* TODO: We need to know what the tx_timer_delete did and shall we call this function when driver exit?? */
	status = tx_timer_deactivate(pTimer);

	if (status == TX_SUCCESS)
	{
		*pCancelled = TRUE;
	}
	else
	{
		dc_log_printf("CancelTimer failed!\n");
	}
}

VOID RTMP_OS_Release_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer)
{
}


/*******************************************************************************

	Normal memory allocation related functions.
	
 *******************************************************************************/
/* pAd MUST allow to be NULL */
NDIS_STATUS os_alloc_mem(
	IN	VOID *pAd,
	OUT	UCHAR **mem,
	IN	ULONG  size)
{	
	*mem = (PUCHAR) kmalloc(size, 0);
	if (in_interrupt())
		dc_log_printf("%s(): kmalloc memory pointer=0x%x!\n", __FUNCTION__, (unsigned long)(*mem),0,0,0,0);
	
	if (*mem)
		return (NDIS_STATUS_SUCCESS);
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("malloc failed size = %d\n", size));
		return (NDIS_STATUS_FAILURE);
	}
}

NDIS_STATUS os_alloc_mem_suspend(
	IN	VOID					*pReserved,
	OUT	UCHAR					**mem,
	IN	ULONG					size)
{
	return os_alloc_mem(pReserved, mem, size);
}

/* pAd MUST allow to be NULL */
NDIS_STATUS os_free_mem(
	IN	VOID *pAd,
	IN	PVOID mem)
{
	
	ASSERT(mem);
	kfree(mem);
	return (NDIS_STATUS_SUCCESS);
}


/*******************************************************************************

	Device IRQ related functions.
	
 *******************************************************************************/
int RtmpOSIRQRequest(IN PNET_DEV pNetDev)
{
#if 0 /* def SIGMATEL_SDK */
	DC_RETURN_CODE status;

	/* 
		Registe the interrupt handler for the specified interrupt source. 
		DC_INT_IRQ_NUM indicates the interrupt identifier that corresponds to a 
			DigiColor interrupt source.
	*/
	status = dc_int_register(DC_INT_IRQ_NUM, rt2860_interrupt, interrup_priority);

	/* enable the inteerupt*/
	status = dc_int_irq_enable(DC_INT_IRQ_NUM);
#endif /* SIGMATEL_SDK */

	return 0; 
}


int RtmpOSIRQRelease(
	IN	PNET_DEV				pNetDev,
	IN	UINT32					infType,
	IN	PPCI_DEV				pci_dev,
	IN	BOOLEAN					*pHaveMsi)
{
#if 0 /*def SIGMATEL_SDK */
	DC_RETURN_CODE status;

	status = dc_int_disable(DC_INT_IRQ_NUM);
#endif /* SIGMATEL_SDK */

	return 0;
}


/*******************************************************************************

	File open/close related functions.
	
 *******************************************************************************/
RTMP_OS_FD RtmpOSFileOpen(char *pPath,  int flag, int mode)
{
	/* TODO: ThreadX don't support file systems. so the open/close file left empty now. */
	DBGPRINT(RT_DEBUG_OFF, ("%s(): Function not implemented!\n", __FUNCTION__));
	
	return -1;
}

int RtmpOSFileClose(RTMP_OS_FD osfd)
{
	/* TODO: ThreadX don't support file systems. so the open/close file left empty now. */
	DBGPRINT(RT_DEBUG_OFF, ("%s(): Function not implemented!\n", __FUNCTION__));

	return -1;
}


void RtmpOSFileSeek(RTMP_OS_FD osfd, int offset)
{
	/* TODO: ThreadX don't support file systems. so the open/close file left empty now. */
	DBGPRINT(RT_DEBUG_OFF, ("%s(): Function not implemented!\n", __FUNCTION__));

	return -1;
}


int RtmpOSFileRead(RTMP_OS_FD osfd, char *pDataPtr, int readLen)
{
	/* TODO: ThreadX don't support file systems. so the open/close file left empty now. */
	DBGPRINT(RT_DEBUG_OFF, ("%s(): Function not implemented!\n", __FUNCTION__));

	return -1;
}


int RtmpOSFileWrite(RTMP_OS_FD osfd, char *pDataPtr, int writeLen)
{
	/* TODO: ThreadX don't support file systems. so the open/close file left empty now. */
	DBGPRINT(RT_DEBUG_OFF, ("%s(): Function not implemented!\n", __FUNCTION__));

	return -1;
}


void RtmpOSFSInfoChange(RTMP_OS_FS_INFO *pOSFSInfo, BOOLEAN bSet)
{
	return;
}



/*******************************************************************************

	Task create/management/kill related functions.
	
 *******************************************************************************/
#ifndef NET_TASK_PRIORITY
/*
	Known Issues:
		(1). For ThreadX, the priority of dbgmon is 16, so we use 17 so that our threads will not 
			effect the dbgmon in development stage. is that ok??
		(2). About the arrangement of stack, we should implment a central contorl mechanism to 
			manage all threads created by RT_WIFI.
*/
#define NET_TASK_PRIORITY       16
#endif
NDIS_STATUS RtmpOSTaskAttach(
	IN RTMP_OS_TASK *pTask, 
	IN RTMP_OS_TASK_CALLBACK fn, 
	IN ULONG arg)
{
	UINT status;
	void *stackOffset;
	UCHAR *pTaskMem;

	pTaskMem = (UCHAR *)pTask->taskPID;
	stackOffset = (void *)(pTaskMem + ((sizeof(struct TX_THREAD_STRUCT) + 3) & (~3)));

	dc_log_printf("creatTask(%s)\n", pTask->taskName);
	status = tx_thread_create(pTask->taskPID, 
							&pTask->taskName[0], 
							fn, 
							(ULONG)arg, 
							stackOffset,
							RTMP_TX_TASK_STACK_SIZE, 
							RTMP_TX_TASK_PRIORITY,
							RTMP_TX_TASK_PRIORITY, 
							0, 
							TX_AUTO_START);
	
	if (status != TX_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Attach task(%s) failed!\n", pTask->taskName));
		pTask->taskStatus = RTMP_TASK_STAT_STOPED;
		return NDIS_STATUS_FAILURE;
	}
	else
		return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS RtmpOSTaskInit(
	IN RTMP_OS_TASK *pTask,
	RTMP_STRING *pTaskName,
	VOID *pPriv,
	LIST_HEADER				*pTaskList,
	LIST_HEADER				*pSemList)
{
	struct TX_THREAD_STRUCT *pTxTaskInstance;
	int len, taskStructSize;
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)pPriv;
	
	UCHAR *pTaskAddr;
	ra_dma_addr_t *pDmaAddr;
	RTMP_DC_RAM_BLOCK_ELEMENT * pDcRamBlk;


	ASSERT(pTask);

	/*
		First we allocate memory for TX_THREAD_STRUCT and Stack memory
	*/
	taskStructSize = ((sizeof(struct TX_THREAD_STRUCT) + 3) & (~3));
	
	pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_STACK];
	RtmpDCRamBlockMemAlloc(pDcRamBlk, RTMP_TX_TASK_STACK_SIZE + taskStructSize, (void **)&pTxTaskInstance, &pDmaAddr);

	if (pTxTaskInstance == NULL)
	{
		dc_log_printf("Alloc(%s)failed\n", pTaskName);
		return NDIS_STATUS_FAILURE;
	}

	if (0)/*pTask == &pAd->cmdQTask) */
	{
		dc_log_printf("Init for %s, return!\n", pTaskName);
		return NDIS_STATUS_SUCCESS;
	}

	NdisZeroMemory((PUCHAR)(pTask), sizeof(RTMP_OS_TASK));
	pTask->taskPID = pTxTaskInstance;
	len = strlen(pTaskName);
	len = len > (RTMP_OS_TASK_NAME_LEN -1) ? (RTMP_OS_TASK_NAME_LEN-1) : len;
	NdisMoveMemory(&pTask->taskName[0], pTaskName, len); 
	
	RTMP_SEM_EVENT_INIT_LOCKED(&(pTask->taskSema), NULL);

	pTask->priv = pPriv;
	
	/* TODO: For ThreadX, we need to implement a wait_completion mechanism to make sure task running successfully. */
	/*init_completion (&pTask->taskComplete); */
	dc_log_printf("InitTask(%s)\n", pTask->taskName);
	
	return NDIS_STATUS_SUCCESS;
}


INT RtmpOSTaskNotifyToExit(
	IN RTMP_OS_TASK *pTask)
{
	/* TODO: Shall we do any customization for ThreadX tasks? */
	dc_log_printf("Into %s()\n", __FUNCTION__);
	return 0;
}


void RtmpOSTaskCustomize(
	IN RTMP_OS_TASK *pTask)
{
	/* TODO: Shall we do any customization for ThreadX tasks? */
	dc_log_printf("Into %s()\n", __FUNCTION__);
	return;
}


/* TODO: Shiang, need to check following conditions */
NDIS_STATUS RtmpOSTaskKill(
	IN RTMP_OS_TASK *pTask)
{
	UINT retVal;

	dc_log_printf("Into %s()\n", __FUNCTION__);
/*
	For ThreadX, there is another function "tx_thread_terminate(TX_THREAD*)" which can use
	to terminate the specified application thread regardless of whether the thread is suspended 
	or not.
	
	But, in our original design, when we kill a task, we need to waiting for the task to exit itself
	because the task may need to remove some messsages and related memory spaces.
	An alternate option is use the combination of "tx_thread_wait_abort(TX_THREAD *)" and 
	"pTask->task_killed" to make the task terminated itself.
	
	Hmm....for this solution, is there any hole or timing issue for the "abort()"?????
	
	Shall we take care these here????
*/
	RTMP_SEM_EVENT_DESTORY(&pTask->taskSema);
#if 0
	retVal = tx_thread_delete(pTask->taskPID);
#else
	retVal = tx_thread_wait_abort(pTask->taskPID);

	/* After abort the waiting status, we waiting for the task exit itself */
	/*wait_for_completion(&pTask->taskComplete); */

	/* After thread is terminated, we delete the thread */
	tx_thread_delete(pTask->taskPID);
#endif
	pTask->task_killed = 0;

	/* Free the stack and TX_THREAD_STRUCT */
	/* TODO: need to free the memory */
	
	return NDIS_STATUS_SUCCESS;
}


#ifdef CONFIG_AP_SUPPORT
VOID SendSignalToDaemon(
	IN INT		sig,
	IN ULONG	pid)
{
	return;
}
#endif /* CONFIG_AP_SUPPORT */

/*
 ========================================================================
 Routine Description:
    Build a L2 frame to upper layer.

 Arguments:

 Return Value:
    TRUE - send successfully
    FAIL - send fail

 Note:
 ========================================================================
*/
BOOLEAN RTMPL2FrameTxAction(
	IN  PRTMP_ADAPTER		pAd,
	IN	PNET_DEV			pNetDev,
	IN	RTMP_CB_8023_PACKET_ANNOUNCE _announce_802_3_packet,
	IN	UCHAR				apidx,
	IN	PUCHAR				pData,
	IN	UINT32				data_len)
{
	return TRUE;
}

void RTMP_IndicateMediaState(	
	IN	PRTMP_ADAPTER		pAd,
	IN  NDIS_MEDIA_STATE	media_state)
{
		if (pAd->IndicateMediaState == NdisMediaStateConnected)
		{
			RTMPSendWirelessEvent(pAd, IW_STA_LINKUP_EVENT_FLAG, pAd->MacTab.Content[BSSID_WCID].Addr, BSS0, 0);
		}
		else
		{							
			RTMPSendWirelessEvent(pAd, IW_STA_LINKDOWN_EVENT_FLAG, pAd->MacTab.Content[BSSID_WCID].Addr, BSS0, 0); 		
		}	
}

#ifdef SYSTEM_LOG_SUPPORT
#ifdef SIGMATEL_SDK
void RTMPSendWirelessEvent(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Event_flag,
	IN	PUCHAR 			pAddr,
	IN	UCHAR			wdev_idx,
	IN	CHAR			Rssi)
{
	PSY_CONTROL pSyControl; 
	
	pSyControl = &pAd->StaCfg.SYControl;

	switch(Event_flag)
	{
	case IW_SCAN_COMPLETED_EVENT_FLAG:
		if (pSyControl->SYWiFiAPScanTrigger)
			SYScanCompletedEventHdlr(pAd, wdev_idx);
		break;
	case IW_WPS_END_EVENT_FLAG:
		if (pSyControl->SYWiFiWPSTrigger)
			SYWPSEndEventHdlr(pAd, wdev_idx);
		break;

	case IW_WSC_NEXT_CANDIDATE:
		if (pSyControl->SYWiFiWPSTrigger)
			SYNextCanidatedEventHdlr(pAd, wdev_idx);
		break;
	case IW_WSC_STATUS_FAIL:
		if (pSyControl->SYWiFiWPSTrigger)
			SYWSCStatusFailEventHdlr(pAd, wdev_idx);
		break;
	case IW_STA_LINKUP_EVENT_FLAG:
		if (pSyControl->SYWiFiWPSTrigger)
			SYSTALinkUpEventHdlr(pAd, wdev_idx);
		break;

	default:
		break;
	}
}
#endif /* SIGMATEL_SDK */
#endif /* SYSTEM_LOG_SUPPORT */



int RtmpOSWrielessEventSend(
	IN	PNET_DEV	pNetDev,
	IN UINT32		eventType,
	IN INT			flags,
	IN PUCHAR		pSrcMac,
	IN PUCHAR		pData,
	IN UINT32		dataLen)
{
	POS_COOKIE	pObj;

	pObj = (POS_COOKIE)pAd->OS_Cookie;

	if (eventType == IWEVCUSTOM)
	{
		switch(flags)
		{
#ifdef WSC_AP_SUPPORT
			case RT_WSC_UPNP_EVENT_FLAG:
				if (pObj->WscMsgCallBack)
					pObj->WscMsgCallBack(pData, dataLen);
				break;
#endif /* WSC_AP_SUPPORT */
			default:
				break;
		}
	}
	return 0;
}



VOID	RTMPFreeAdapter(
	IN	VOID		*pAdSrc)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;
	POS_COOKIE os_cookie;
	
	os_cookie=(POS_COOKIE)pAd->OS_Cookie;

	/* TODO: confirm about the free of PSB buffer of ThreadX */
	/* For ThreadX, because we use PSB memory block for the RTMP_ADAPTER structure, so we don't need to free it here. */
	/*kfree(pAd); */
	RT_WIFI_DevCtrl = NULL;
	pAd = NULL;

	kfree(os_cookie);
}


RTMP_DC_RAM_BLOCK_ELEMENT RtmpDCRamMgrPSBBuf[]={
	{	/* RtmpDCRammgrBuf_DevCrl*/
		.Id = RTMP_DC_RAM_ID_DEVCTRL,
		.name="RtmpDrvCtrl",
		.blkSize = sizeof(RTMP_ADAPTER),
		.blkNum = 1,
		.alignment = 2,
		.bValid= FALSE,
	},
	{	/* RtmpDCRammgrBuf_Stack */
		.Id = RTMP_DC_RAM_ID_STACK,
		.name = "RtmpTaskletStack",
		.blkSize = RTMP_TX_TASK_STACK_SIZE + ((sizeof(struct TX_THREAD_STRUCT) + 3) & (~3)),
		.blkNum = RTMP_TX_TASK_NUM,
		.alignment = 2,
		.bValid= FALSE,
	},
	{	/* RtmpDCRammgrBuf_HTTx */
		.Id = RTMP_DC_RAM_ID_HTTX,
		.name = "RtmpHTTxDataBuf",
		.blkSize = sizeof(HTTX_BUFFER),
		.blkNum = NUM_OF_TX_RING,
		.alignment = RTMP_DC_USB_ALIGN_ORDER,
		.bValid= FALSE,
	},
	{	/* RtmpDCRammgrBuf_HTTx */
		.Id = RTMP_DC_RAM_ID_HTTX_ALIGN,
		.name = "RtmpHTTxDataBufAgg",
		.blkSize = 0x8000,
		.blkNum = NUM_OF_TX_RING,
		.alignment = RTMP_DC_USB_ALIGN_ORDER,
		.bValid= FALSE,
	},
	{	/* RtmpDCRammgrBuf_Tx */
		.Id = RTMP_DC_RAM_ID_TX,
		.name = "RtmpTxDataBuf",
		.blkSize = sizeof(TX_BUFFER),
		.blkNum = 5,	/* 2 beaconRing, 1 RTS, 1 pspoll, 1 null */
		.alignment = RTMP_DC_USB_ALIGN_ORDER,
		.bValid= FALSE,
	},
	{	/* RtmpDCRammgrBuf_Rx */
		.Id = RTMP_DC_RAM_ID_RX,
		.name = "RtmpRxDataBuf",
		.blkSize = MAX_RXBULK_SIZE,
		.blkNum = RX_RING_SIZE,
		.alignment = RTMP_DC_USB_ALIGN_ORDER,
		.bValid= FALSE,
	},
	{	/* RtmpDCRammgrBuf_CtrlPipe */
		.Id = RTMP_DC_RAM_ID_CTRL_PIPE,
		.name = "RtmpCtrlPipeBuf",
		.blkSize = RTMP_DC_RAM_CTRL_PIPE_SIZE,
		.blkNum = RTMP_DC_RAM_CTRL_PIPE_NUM,
		.alignment = 6,
		.bValid= FALSE,
	},
	{	/* RtmpDCRammgrBuf_URB */
		.Id = RTMP_DC_RAM_ID_URB,
		.name = "RtmpDcUrb",
		.blkSize = sizeof(DC_URB),
		.blkNum = RTMP_DC_RAM_CTRL_PIPE_NUM + RX_RING_SIZE + NUM_OF_TX_RING + 5 + MGMT_RING_SIZE * 2,
		.alignment = 5,
		.bValid= FALSE,
	},
#ifdef EBS_RTIP
	{	/* RtmpDCRammgrBuf_NetBuf */
		.Id = RTMP_DC_RAM_ID_NETBUF,
		.name = "RtmpNetBuf",
		.blkSize = sizeof(RTMP_RTIP_NET_BUF),
		.blkNum = (MAX_PACKETS_IN_QUEUE * NUM_OF_TX_RING),
		.alignment = 2,
		.bValid= FALSE,
	},
	{	/* RtmpDCRammgrBuf_Pkt */
		.Id = RTMP_DC_RAM_ID_PKT,
		.name = "RtmpNetPkt",
		.blkSize = 4000,
		.blkNum = MAX_PACKETS_IN_QUEUE * (NUM_OF_TX_RING),
		.alignment = RTMP_DC_USB_ALIGN_ORDER,	/* Here we use 64 bytes aligned because we may use this buffer for MgmtTxData packet */
		.bValid= FALSE,
	},
#endif /* EBS_RTIP */
};


static void hex_dump_DcRamBlk(RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamBlk)
{
	RTMP_DC_RAM_LIST *pList;
	int cnt = 1;
	UINT32 alignedBlkSize;


	if (pDcRamBlk == NULL)
		return;
	
	alignedBlkSize = (pDcRamBlk->blkSize + ((1 <<pDcRamBlk->alignment) -1)) & (~((1 <<pDcRamBlk->alignment) -1));
	
	dc_log_printf("%s(): pDcRamBlock(%s) Element:\n", __FUNCTION__, pDcRamBlk->name);
	dc_log_printf("\tID:%d\n", pDcRamBlk->Id);
	dc_log_printf("\tBaseAddr:0x%x\n", pDcRamBlk->baseAddr);
	dc_log_printf("\tEndAddr:0x%x\n", pDcRamBlk->endAddr);
	dc_log_printf("\tSize:%d\n", pDcRamBlk->size);
	dc_log_printf("\tAlign:2^%d(%d)\n", pDcRamBlk->alignment, (1 <<pDcRamBlk->alignment));
	dc_log_printf("\tblkNum:%d\n", pDcRamBlk->blkNum);
	dc_log_printf("\tblkSize:%d\n", pDcRamBlk->blkSize);
	dc_log_printf("\tAlignedblkSize:%d\n", alignedBlkSize);
	dc_log_printf("\tbValid:%d\n", pDcRamBlk->bValid);

	if (pDcRamBlk->bValid == FALSE)
		return;

	dc_log_printf("\tfreeCnt:%d\n", pDcRamBlk->freeCnt);
	dc_log_printf("\tfreeList:\n");	
	pList = pDcRamBlk->pFreeList;
	while(pList)
	{
		dc_log_printf("    [%d]Addr=0x%x, nextAddr=0x%x!\n", cnt, pList, pList->next);
		pList = pList->next;
		cnt++;
	}
	
}

	
void *RtmpDCRamBlockMemAlloc(
	IN RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamBlk,
	IN UINT32 size, 
	INOUT void **pVirtAddr, 
	INOUT ra_dma_addr_t **DMAAddr)
{
	RTMP_DC_RAM_LIST *pListHead;
	*pVirtAddr = NULL;
	
	/*dc_log_printf("%s(%s):size(%d), freeCnt=%d\n", __FUNCTION__, pDcRamBlk->name, size, pDcRamBlk->freeCnt); */

	/* Here we use RTMP_SEM_LOCK() instead of RTMP_IRQ_LOCK() becasue we don't have pAd here! */
	RTMP_SEM_LOCK(&pDcRamBlk->DcRamBlkLock);
	if (pDcRamBlk->bValid == TRUE)
	{
		pListHead = pDcRamBlk->pFreeList;
		*pVirtAddr = (void *)pListHead;
		if (pListHead != NULL)
		{
			pDcRamBlk->pFreeList =  pListHead->next;
			pDcRamBlk->freeCnt--;
		}
		/*dc_log_printf("%s():success, addr=0x%x, freeCnt=%d\n", __FUNCTION__, *pVirtAddr, pDcRamBlk->freeCnt); */
	}

	if (*pVirtAddr == NULL)
	{	
		dc_log_printf("AllocFailed(%s, %d)\n", pDcRamBlk->name, size);
		hex_dump_DcRamBlk( pDcRamBlk);
	}
	RTMP_SEM_UNLOCK(&pDcRamBlk->DcRamBlkLock);

#if 0 /* for debug */
	if (*pVirtAddr != NULL)
	{
		RTMP_SEM_LOCK(&pDcRamBlk->DcRamBlkLock);
		hex_dump_DcRamBlk(pDcRamBlk);
		RTMP_SEM_UNLOCK(&pDcRamBlk->DcRamBlkLock);	
	}
#endif
	return *pVirtAddr;
}


int RtmpDCRamBlockMemFree(
	IN void *pMemAddr)
{
	RTMP_DC_RAM_LIST *pListHead, *pListNode;
	int ramBlkIdx;
	RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamBlk;

	ASSERT(pMemAddr);
	
	pListNode = (RTMP_DC_RAM_LIST *)pMemAddr;
	for (ramBlkIdx = 0; ramBlkIdx < (sizeof(RtmpDCRamMgrPSBBuf)/sizeof(RTMP_DC_RAM_BLOCK_ELEMENT)); ramBlkIdx++)
	{
		pDcRamBlk = &RtmpDCRamMgrPSBBuf[ramBlkIdx];
		if ((pMemAddr >= pDcRamBlk->baseAddr) && (pMemAddr < pDcRamBlk->endAddr))
		{
			/* 
				Here we use RTMP_SEM_LOCK() instead of RTMP_IRQ_LOCK() becasue we 
				don't have pAd here! 
			*/
			RTMP_SEM_LOCK(&pDcRamBlk->DcRamBlkLock);
			/*hex_dump_DcRamBlk(pDcRamBlk); */
			if (pDcRamBlk->bValid == TRUE)
			{
				pListHead = pDcRamBlk->pFreeList;
				pListNode->next  = (pListHead == NULL) ? NULL : pListHead;
				pDcRamBlk->pFreeList = pListNode;
				pDcRamBlk->freeCnt++;
			}
			/*hex_dump_DcRamBlk(pDcRamBlk); */
			RTMP_SEM_UNLOCK(&pDcRamBlk->DcRamBlkLock);	
			break;
		}
	}

	return 0;
}


static DC_RETURN_CODE RtmpDCBufFragment(
	IN RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamBlock)
{
	int alignedBlkSize, cnt = 0;
	UINT32 blockAddr;
	RTMP_DC_RAM_LIST *freeListPtr;

	
	if (pDcRamBlock == NULL)
	{
		dc_log_printf("pDcRamBlock == NULL!\n");
		return DC_FAILURE;
	}

	/*hex_dump_DcRamBlk(pDcRamBlock); */
	
	alignedBlkSize = (pDcRamBlock->blkSize + ((1 <<pDcRamBlock->alignment) -1)) & (~((1 <<pDcRamBlock->alignment) -1));	
	if((alignedBlkSize * pDcRamBlock->blkNum) > pDcRamBlock->size)
	{
		dc_log_printf("requriedSize=%d, pDcRamBlock->size = %d, !\n", alignedBlkSize * pDcRamBlock->blkNum, pDcRamBlock->size);
		return DC_FAILURE;
	}

	blockAddr = pDcRamBlock->baseAddr;
	pDcRamBlock->pFreeList = (RTMP_DC_RAM_LIST *)blockAddr;
	freeListPtr = pDcRamBlock->pFreeList;
	cnt = 1;
	
	do
	{
		blockAddr += alignedBlkSize;
		if (blockAddr + alignedBlkSize <= pDcRamBlock->endAddr)
		{
			freeListPtr->next = (RTMP_DC_RAM_LIST *)blockAddr;
			/*dc_log_printf("Build_1 Addr = 0x%x, next=0x%x, cnt=%d\n", freeListPtr, freeListPtr->next, cnt); */
			freeListPtr = freeListPtr->next;
		}
		else
		{
			freeListPtr->next = 0;
			/*dc_log_printf("Build_2 Addr = 0x%x, next=0x%x, cnt=%d\n", freeListPtr, freeListPtr->next, cnt); */
			break;
		}
		cnt++;

	}while(cnt <= pDcRamBlock->blkNum);
	
	if (cnt != pDcRamBlock->blkNum)
	{
		dc_log_printf("cnt(%d)!= num(%d)\n", cnt, pDcRamBlock->blkNum);
		return DC_FAILURE;
	}

	hex_dump_DcRamBlk(pDcRamBlock);

#if 0 /* for debug only */
	dc_log_printf("Now Dump the DCBufFragmentList(%s): cnt=%d!\n", pDcRamBlock->name, cnt);
	freeListPtr = pDcRamBlock->pFreeList;
	cnt = 0;
	while(freeListPtr != NULL)
	{
		cnt++;
		dc_log_printf("Addr = 0x%x, next=0x%x, cnt=%d\n", freeListPtr, freeListPtr->next, cnt);
		freeListPtr = freeListPtr->next;
	}
#endif

	return DC_SUCCESS;
}


static DC_RETURN_CODE RtmpDCBufCallback(DC_RAMMGR_BUFFER_ENTRY *entry)
{
	UINT32 bufferId;
	RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamBlock;
	DC_RETURN_CODE retCode = DC_SUCCESS;
	int blkIdx;
	
	DBGPRINT(RT_DEBUG_TRACE, ("%s():entry->status=%d, bufferId=%d!\n", 
								__FUNCTION__, entry->status, entry->bufferId));

	bufferId = entry->bufferId;
	if((entry->status == DC_BUFFER_AVAILABLE) && ( bufferId < RTMP_DC_RAM_ID_MAX))
	{
		for (blkIdx =0 ; blkIdx < (sizeof(RtmpDCRamMgrPSBBuf) / sizeof(RTMP_DC_RAM_BLOCK_ELEMENT)); blkIdx++)
		{
			if (bufferId == RtmpDCRamMgrPSBBuf[blkIdx].Id)
			{
				pDcRamBlock = &RtmpDCRamMgrPSBBuf[blkIdx];
				pDcRamBlock->baseAddr = CACHE_BYPASS(entry->address);
				pDcRamBlock->endAddr = pDcRamBlock->baseAddr + pDcRamBlock->size;
				DBGPRINT(RT_DEBUG_TRACE, ("%s(%s): base=%x, size=%x\n", 
											__FUNCTION__, pDcRamBlock->name, 
											pDcRamBlock->baseAddr, pDcRamBlock->size));

				retCode = RtmpDCBufFragment(pDcRamBlock);
				NdisAllocateSpinLock(NULL, &pDcRamBlock->DcRamBlkLock);
				if ((retCode == DC_SUCCESS) && (pDcRamBlock->DcRamBlkLock.mutex_status == TX_SUCCESS))
				{
					/* Don't need protect here becasue we are in the initialization stage now */
					pDcRamBlock->freeCnt = pDcRamBlock->blkNum;
					pDcRamBlock->bValid = TRUE;
					DBGPRINT(RT_DEBUG_TRACE, ("%s: init DC RAM Block for (%s) success!\n", __FUNCTION__, pDcRamBlock->name));
				}
				break;
			}
		}
	}
	
	return retCode;
	
}


DC_RETURN_CODE RtmpTxBootStrapTask(void)
{
	DC_RETURN_CODE retCode = DC_SUCCESS;
	static UINT32 OnlyOnce = 0;

	
	dc_log_printf("IntoBootStrap\n");

	retCode = RtmpDcUSBTaskInit();
	if (retCode != DC_SUCCESS)
		return retCode;
	DBGPRINT(RT_DEBUG_TRACE, ("Create USBTask success\n"));

	if (OnlyOnce == 0)
	{
		xn_bind_rt_wifi(MINOR_0);
		OnlyOnce = 1;
	}

	return retCode;
}

	
/*
	This is the entry point for RT_WIFI to do initialization for ThreadX. In this function, we do all necessary 
	initialization for RT_WIFI. Here list all the resources RT_WIFI need to do in the initialization stage.
	(1). memory buffer
	(2). task
	(3). net device allocation

	NOTE: Please don't enable the interrupt in this function or any function called by this function. Because
		this function will be called by threadX system function "tx_application_define()"
*/
DC_RETURN_CODE RtmpTX_application_define(void *ignore)
{
	UINT status;
	int idx;
	
	/*
	tx_byte_pool_create();
	tx_byte_allocate();
	tx_byte_pool_delete();
	
	tx_block_pool_create();
	tx_block_allocate();
	tx_block_release();
	tx_block_delete();


	tx_thread_create();
	tx_queue_create();
	*/

#ifdef SIGMATEL_SDK
	RTMP_DC_RAM_BLOCK_ELEMENT *pPSBMemEntry;
	DC_RETURN_CODE retCode;

	
	/*
		Allocate memory for dynamical memory allocation.
	*/
	/* TODO: We need a way to figure the minimum memory requirement of RT_WIFI exactlly */
	retCode = dc_am_global_setup(0x100000);
	if (retCode != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("dc_am_global_setup failed(%d)!\n", retCode));
		return retCode;
	}
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): dc_am_global_setup() return success\n", __FUNCTION__));
	
	/*
		Memoy allocation for variuos memory segment requried for RT_WIFI.
			=> RTMP_ADAPTER memory
			=> tasklet stack memory
			=> Tx/Rx/CtrlPipe Ring memory
			=> DCU wrapper buffers
			
		Step 1: For each memory segment, we need to assign the memory size, alignment requirement, 
				and blockSize of each sub-block of this memory segment
				
				RtmpDevCtrl block: sizeof(RTMP_ADAPTER)
				tasklet stacks: 
						net tasklets: ((_taskEndAddr - _taskStartAddr) / sizeof(RTMP_NET_TASK_STRUCT)) tasklets,
						mgmt tasklets: timerTask/mlmeTask/CmdQTask/WscTask 
				Tx/Rx/CtrlPipe buffers:
						"TX_RING_SIZE" Tx Ring and  "RX_RING_SIZE" Rx Ring buffer	
				RTIP Network DCU wrapper Buf:
						(MAX_PACKETS_IN_QUEUE * NUM_OF_TX_RING) * sizeof(RTMP_RTIP_NET_BUF)

		Step 2: Request the memory to OS by function call "dc_rammgr_buffer_request"

	*/
	
	for (idx= 0; idx < (sizeof(RtmpDCRamMgrPSBBuf) / sizeof(RTMP_DC_RAM_BLOCK_ELEMENT)); idx++)
	{
		UINT32 actualBlockSize;
		
		pPSBMemEntry = &RtmpDCRamMgrPSBBuf[idx];

		actualBlockSize = (pPSBMemEntry->blkSize + ((1<< pPSBMemEntry->alignment)-1)) & (~((1<< pPSBMemEntry->alignment) -1));
		pPSBMemEntry->size = actualBlockSize * pPSBMemEntry->blkNum;

		status = dc_rammgr_buffer_request(&pPSBMemEntry->dcMemBufEntry, 
								pPSBMemEntry->name,  /* Name */
								pPSBMemEntry->Id, /* */
								pPSBMemEntry->size, /* Size */
								pPSBMemEntry->alignment, /* Alignment */
								DC_TYPE_PSB, /* DC_TYPE */
								0, /* state ID*/
								RtmpDCBufCallback);
		if (status != DC_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("allocate memory for %s failed!\n", pPSBMemEntry->name));
			goto request_failed;
		}
	}

	/* Add thread to kick off Network once NVRAM is valid. */
	status = tx_thread_create(&RTMP_TX_BOOTSTRAP_TASK, 
							"RtmpTxBootStrapTask",
							RtmpTxBootStrapTask,
							0,
							RtmpTxBootStrapTaskStack,
							sizeof(RtmpTxBootStrapTaskStack),
							RTMP_TX_TASK_PRIORITY,
							RTMP_TX_TASK_PRIORITY,
							TX_NO_TIME_SLICE,
							TX_AUTO_START);
	if (status != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Create BootStrapTask failed!\n"));
		goto request_failed;
	}

	/* Ralink USB driver register API, must call after USB Core initialization */
	status = RTUSBDriverRegister();
	if (status != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("RTUSBDriverRegister failed(%d)!\n", status));
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): RTUSBDriverRegister() return success\n", __FUNCTION__));
	}

#endif /* SIGMATEL_SDK */

	return DC_SUCCESS;

request_failed:
	return DC_FAILURE;
}


DC_RETURN_CODE wifi_init(IN DC_NVRAM_STORE_t *pNvram_store, IN DC_WIFI_INIT_CONFIG *operations)
{
	DC_RETURN_CODE ret;

	/* Register wifi NVRAM object to NVRAM store */
	ret = dc_nvram_object_register(pNvram_store, /* NVRAM store from main application */
                                   &NVRAMWIFISetting, /* NVRAM object handle */
                                   &WiFiMacConfDefault, /* Default values */
                                   &WiFiMacConfCache,/*Cache of config from NVRAM */
                                   NVRAM_NAME_WIFI_MAC_CONFIG, /* Name for NVRAM object for debugging purposes - only 4 chars matter */
                                   sizeof(DC_WIFI_MAC_CONFIGURATION), /* Size of NVRAM object in bytes */
                                   1); /* Version of NVRAM object */

	if (ret != DC_SUCCESS)
    {
    	DBGPRINT(RT_DEBUG_ERROR, ("dc_nvram_object_register() ERR [%#x]", ret));
        return ret;
    }

	/* Ralink WiFi Initialization */
  	ret = RtmpTX_application_define(pNvram_store);

	if (ret != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("RtmpTX_application_define ERR [%x]", ret));
		return ret;
	}
	
	return ret;
}


UCHAR VLAN_8023_Header_Copy(
	IN	USHORT			VLAN_VID,
	IN	USHORT			VLAN_Priority,
	IN	PUCHAR			pHeader802_3,
	IN	UINT            HdrLen,
	OUT PUCHAR			pData,
	IN	UCHAR			*TPID)
{
	UINT16 TCI;
	UCHAR VLAN_Size = 0;


	if (VLAN_VID != 0)
	{
		/* need to insert VLAN tag */
		VLAN_Size = LENGTH_802_1Q;

		/* make up TCI field */
		TCI = (VLAN_VID & 0x0fff) | ((VLAN_Priority & 0x7)<<13);

#ifndef RT_BIG_ENDIAN
		TCI = SWAP16(TCI);
#endif /* RT_BIG_ENDIAN */

		/* copy dst + src MAC (12B) */
		memcpy(pData, pHeader802_3, LENGTH_802_3_NO_TYPE);

		/* copy VLAN tag (4B) */
		/* do NOT use memcpy to speed up */
		*(UINT16 *)(pData+LENGTH_802_3_NO_TYPE) = *(UINT16 *)TPID;
		*(UINT16 *)(pData+LENGTH_802_3_NO_TYPE+2) = TCI;

		/* copy type/len (2B) */
		*(UINT16 *)(pData+LENGTH_802_3_NO_TYPE+LENGTH_802_1Q) = \
				*(UINT16 *)&pHeader802_3[LENGTH_802_3-LENGTH_802_3_TYPE];

		/* copy tail if exist */
		if (HdrLen > LENGTH_802_3)
		{
			memcpy(pData+LENGTH_802_3+LENGTH_802_1Q,
					pHeader802_3+LENGTH_802_3,
					HdrLen - LENGTH_802_3);
		}
	}
	else
	{
		/* no VLAN tag is needed to insert */
		memcpy(pData, pHeader802_3, HdrLen);
	}

	return VLAN_Size;
}


inline atomic_t atomic_read(atomic_t *v)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;
	int ret;
	RTMP_SEM_LOCK(&pAd->GenericLock);
	ret = *v;
	RTMP_SEM_UNLOCK(&pAd->GenericLock);
	return ret;
}


inline void atomic_inc(atomic_t *v)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;
	RTMP_SEM_LOCK(&pAd->GenericLock);
	*v = *v + 1;
	RTMP_SEM_UNLOCK(&pAd->GenericLock);

}


inline void atomic_dec(atomic_t *v)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;
	RTMP_SEM_LOCK(&pAd->GenericLock);
	*v = *v -1;
	RTMP_SEM_UNLOCK(&pAd->GenericLock);
}


inline void atomic_set(atomic_t *v, int i)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;
	RTMP_SEM_LOCK(&pAd->GenericLock);
	*v = i;
	RTMP_SEM_UNLOCK(&pAd->GenericLock);
}


VOID RtmpOsPktInit(
	IN	PNDIS_PACKET		pNetPkt,
	IN	PNET_DEV			pNetDev,
	IN	UCHAR				*pData,
	IN	USHORT				DataSize)
{
	PNDIS_PACKET	pRxPkt;


	pRxPkt = RTPKT_TO_OSPKT(pNetPkt);

	SET_OS_PKT_NETDEV(pRxPkt, pNetDev);
	SET_OS_PKT_DATAPTR(pRxPkt, pData);
	SET_OS_PKT_LEN(pRxPkt, DataSize);
	SET_OS_PKT_DATATAIL(pRxPkt, pData, DataSize);
}

VOID RtmpUtilInit(VOID)
{
}

BOOLEAN RtmpOsStatsAlloc(
	IN	VOID					**ppStats,
	IN	VOID					**ppIwStats)
{
	return TRUE;
}

BOOLEAN RtmpOSTaskWait(
	IN	VOID					*pReserved,
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	INT32					*pStatus)
{
	RTMP_SEM_EVENT_WAIT(&(pTaskOrg->taskSema), *pStatus);
	if (*pStatus == 0)
		return TRUE;
	return FALSE;
}

INT RTMP_AP_IoctlPrepare(
	IN	RTMP_ADAPTER			*pAd,
	IN	VOID					*pCB)
{
}

VOID RtmpOsTaskPidInit(
	IN	RTMP_OS_PID				*pPid)
{
	*pPid = THREAD_PID_INIT_VALUE;
}

VOID RtmpOsTaskWakeUp(
	IN	RTMP_OS_TASK			*pTask)
{
#ifdef KTHREAD_SUPPORT
	WAKE_UP(pTask);
#else
	RTMP_SEM_EVENT_UP(&pTask->taskSema);
#endif
}

/*
========================================================================
Routine Description:
	Enable or disable wireless event sent.

Arguments:
	pReserved		- Reserved
	FlgIsWEntSup	- TRUE or FALSE

Return Value:
	None

Note:
========================================================================
*/
VOID RtmpOsWlanEventSet(
	IN	VOID					*pReserved,
	IN	BOOLEAN					*pCfgWEnt,
	IN	BOOLEAN					FlgIsWEntSup)
{
	*pCfgWEnt = FlgIsWEntSup;
}


#ifdef WDS_SUPPORT
VOID AP_WDS_KeyNameMakeUp(
	IN	RTMP_STRING *pKey,
	IN	UINT32						KeyMaxSize,
	IN	INT							KeyId)
{
	snprintf(pKey, KeyMaxSize, "Wds%dKey", KeyId);
}


NET_DEV_STATS *RT28xx_get_wds_ether_stats(
    IN PNET_DEV *net_dev)
{
    return NULL;
}
#endif /* WDS_SUPPORT */


#ifdef CONFIG_AP_SUPPORT
VOID AP_E2PROM_IOCTL_PostCtrl(
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq,
	IN	RTMP_STRING *msg)
{
}


VOID IAPP_L2_UpdatePostCtrl(RTMP_ADAPTER *pAd, UINT8 *mac_p, INT wdev_idx)
{
}
#endif /* CONFIG_AP_SUPPORT */


/*
========================================================================
Routine Description:
	Wake up the command thread.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID RtmpOsCmdUp(
	IN RTMP_OS_TASK			*pCmdQTask)
{
#ifdef KTHREAD_SUPPORT
	do{
		OS_TASK	*pTask = RTMP_OS_TASK_GET(pCmdQTask);
		{
			pTask->kthread_running = TRUE;
	        wake_up(&pTask->kthread_q);
		}
	}while(0);
#else
	do{
		OS_TASK	*pTask = RTMP_OS_TASK_GET(pCmdQTask);
		CHECK_PID_LEGALITY(pTask->taskPID)
		{
			RTMP_SEM_EVENT_UP(&(pTask->taskSema));
		}
	}while(0);
#endif /* KTHREAD_SUPPORT */
}
