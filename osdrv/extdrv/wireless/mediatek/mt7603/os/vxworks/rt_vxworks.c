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
	rt_vxworks.c

    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/

#include "rt_config.h"

/*
	[Problem] For RT2860PCI on the vxWorks, can not use RT_DEBUG_TRACE;
	Or when you switch channel during traffic throughput is running,
	the RT2860PCI AP on the RT2880 board will not send out any packet
	in new channel.
	[Cause] Unknown.
	[Solution] Use RT_DEBUG_ERROR.
*/
ULONG RTDebugLevel = RT_DEBUG_TRACE;
ULONG RTDebugFunc = 0;
UCHAR VxDbgBuffer[256];

#ifdef CACHE_DMA_SUPPORT
UCHAR CACHE_DMA_SIGNAUTURE[]={0x52,0x54,0x4D,0x50};
#endif /* CACHE_DMA_SUPPORT */

#ifdef VENDOR_FEATURE4_SUPPORT
ULONG	OS_NumOfMemAlloc = 0, OS_NumOfMemFree = 0;
#endif /* VENDOR_FEATURE4_SUPPORT */
#ifdef VENDOR_FEATURE2_SUPPORT
ULONG	OS_NumOfPktAlloc = 0, OS_NumOfPktFree = 0;
#endif /* VENDOR_FEATURE2_SUPPORT */


/*
	This function copy from target/usr/src/libdl/strsep.c
*/
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


void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen)
{
	unsigned char *pt;
	int x;

	if (RTDebugLevel < RT_DEBUG_TRACE)
		return;
	
	pt = pSrcBufVA;
	DBGPRINT(RT_DEBUG_ERROR, ("%s: %p, len = %d\n",str,  pSrcBufVA, SrcBufLen));
	for (x=0; x<SrcBufLen; x++)
	{
		if (x % 16 == 0) 
			DBGPRINT(RT_DEBUG_ERROR, ("0x%04x : ", x));
		DBGPRINT(RT_DEBUG_ERROR, ("%02x ", ((unsigned char)pt[x])));
		if (x%16 == 15) DBGPRINT(RT_DEBUG_ERROR,("\n"));
	}
	DBGPRINT(RT_DEBUG_ERROR,("\n"));
}


#ifdef DBG
void dump_RxDesc(PRXD_STRUC pRxDesc)
{
	printf("Dump RxDesc = 0x%x!\n", pRxDesc);
	hex_dump("RxDescRawData", (PUCHAR)pRxDesc, sizeof(RXD_STRUC));
	
	/* Word 0 */
	printf("\tSDP0=0x%x!\n", pRxDesc->SDP0);

	/*Word 1 */
	printf("\tSDL1=0x%x!\n", pRxDesc->SDL1);
	printf("\tSDL0=0x%x!\n", pRxDesc->SDL0);
	printf("\tLS0=0x%x!\n", pRxDesc->LS0);
	printf("\tDDONE=0x%x!\n", pRxDesc->DDONE);

	/* Word 2 */
	printf("\tSDP1=0x%x!\n", pRxDesc->SDP1);

	/* Word 3 */
	printf("\tBA=0x%x!\n", pRxDesc->BA);
	printf("\tDATA=0x%x!\n", pRxDesc->DATA);
	printf("\tNULLDATA=0x%x!\n", pRxDesc->NULLDATA);
	printf("\tFRAG=0x%x!\n", pRxDesc->FRAG);
	printf("\tU2M=0x%x!\n", pRxDesc->U2M);			/* 1: this RX frame is unicast to me */
	printf("\tMcast=0x%x!\n", pRxDesc->Mcast);			/* 1: this is a multicast frame */
	printf("\tBcast=0x%x!\n", pRxDesc->Bcast);			/* 1: this is a broadcast frame */
	printf("\tMyBss=0x%x!\n", pRxDesc->MyBss);			/* 1: this frame belongs to the same BSSID */
	printf("\tCrc=0x%x!\n", pRxDesc->Crc);				/* 1: CRC error */
	printf("\tCipherErr=0x%x!\n", pRxDesc->CipherErr);	/* 0: decryption okay, 1:ICV error, 2:MIC error, 3:KEY not valid */
	printf("\tAMSDU=0x%x!\n", pRxDesc->AMSDU);		/* rx with 802.3 header, not 802.11 header. */
	printf("\tHTC=0x%x!\n", pRxDesc->HTC);
	printf("\tRSSI=0x%x!\n", pRxDesc->RSSI);
	printf("\tL2PAD=0x%x!\n", pRxDesc->L2PAD);
	printf("\tAMPDU=0x%x!\n", pRxDesc->AMPDU);
	printf("\tDecrypted=0x%x!\n", pRxDesc->Decrypted);	/* this frame is being decrypted. */
}
#endif /* DBG */


void hex_dump_mBlk(PUCHAR callerStr, M_BLK_ID pMblkBuf, BOOLEAN bDumpPkt)
{
	int headroom;
	CL_BUF_ID	pClBuf;
	
	headroom = (pMblkBuf->m_data - pMblkBuf->pClBlk->clNode.pClBuf);
	pClBuf = (CL_BUF *)pMblkBuf->pClBlk->clNode.pClBuf;
	
	printf("%s(): pMblk->m_pkthdr.rcvif=0x%x! \n"
			  "\tpClBuf=0x%x, pClPool=0x%x, mBlkHdr_mData=0x%x, headroom = %d!\n" 
			  "\tclSize=%d, mBlkHdr_mLen=%d.\n" 
			  "\tmBlkHdr_mType=0x%x, tmBlkHdr_mFlags=0x%x.\n" 
			  "\tmBlkPktHdr.len=%d\n"
			  "\t.mBlkHdr.reserved=0x%x\n",
			callerStr, 
			GET_OS_PKT_NETDEV(pMblkBuf), 
			pClBuf, CL_BUF_TO_CL_POOL(pClBuf), GET_OS_PKT_DATAPTR(pMblkBuf), headroom,
			pMblkBuf->pClBlk->clSize, GET_OS_PKT_LEN(pMblkBuf), pMblkBuf->mBlkHdr.mType, pMblkBuf->mBlkHdr.mFlags,
			/*pMblkBuf->mBlkPktHdr.header,*/ pMblkBuf->mBlkPktHdr.len, pMblkBuf->mBlkHdr.reserved);
	
	if (bDumpPkt)
		hex_dump(callerStr, pMblkBuf->pClBlk->clNode.pClBuf, headroom + pMblkBuf->m_len);

}


void hex_dump_NetPool(PUCHAR callerStr, NET_POOL_ID pNetPool, BOOLEAN bTestPool)
{
	int clTbIdx;
	CL_POOL 		*pClPool;
	CL_BUF_ID		pClBuf;


	printf("%s(): Start Dump pNetPool(Addr=0x%x)\n", callerStr, pNetPool);

	/* mBlkConfig */
	printf("\tpNetPool->pmBlkHead=0x%x!\n", pNetPool->pmBlkHead);
	printf("\tpNetPool->pClBlkHead=0x%x!\n", pNetPool->pClBlkHead);
	printf("\tpNetPool->mBlkCnt=0x%x!\n", pNetPool->mBlkCnt);
	printf("\tpNetPool->mBlkFree=0x%x!\n", pNetPool->mBlkFree);

	/* clCluster */
	printf("\tpNetPool->clMask=0x%x!\n", pNetPool->clMask);
#ifdef VXWORKS_6X
	printf("\tpNetPool->clOffset=0x%x!\n", pNetPool->clOffset);
#endif /* VXWORKS_6X */
	printf("\tpNetPool->clLg2Max=0x%x!\n", pNetPool->clLg2Max);
	printf("\tpNetPool->clSizeMax=0x%x!\n", pNetPool->clSizeMax);
	printf("\tpNetPool->clLg2Min=0x%x!\n", pNetPool->clLg2Min);
	printf("\tpNetPool->clSizeMin=0x%x!\n", pNetPool->clSizeMin);
#ifndef VXWORKS_PCD10
	printf("\tpNetPool->clBlkOutstanding=0x%x!\n", pNetPool->clBlkOutstanding);

	/* flags */
	printf("\tpNetPool->flag=0x%x!\n", pNetPool->flag);
	printf("\tpNetPool->attachRefCount=%d!\n", pNetPool->attachRefCount);
	printf("\tpNetPool->bindRefCount=%d!\n", pNetPool->bindRefCount);
	printf("\tpNetPool->pParent=0x%x!\n", pNetPool->pParent);
#endif /* VXWORKS_PCD10 */

	/* statistics */
	if (pNetPool->pPoolStat != NULL)
	{
		printf("\tpNetPool->pPoolStat:\n");
		printf("\t\tpPoolStat->mNum=%d!\n", pNetPool->pPoolStat->mNum);
		printf("\t\tpPoolStat->mDrops=%d!\n", pNetPool->pPoolStat->mDrops);
		printf("\t\tpPoolStat->mWait=%d!\n", pNetPool->pPoolStat->mWait);
		printf("\t\tpPoolStat->mDrain=%d!\n", pNetPool->pPoolStat->mDrain);
	}
	
#ifndef VXWORKS_PCD10	
	/* Attribute */
	if (pNetPool->pAttrSet != NULL)
	{
		printf("\tpNetPool->pAttrSet:\n");
		printf("\t\tpAttrSet->attribute=0x%x!\n", pNetPool->pAttrSet->attribute);
		printf("\t\tpAttrSet->refCount=0x%x!\n", pNetPool->pAttrSet->refCount);
		printf("\t\tpAttrSet->pDomain=0x%x!\n", pNetPool->pAttrSet->pDomain);
		printf("\t\tpAttrSet->ctrlAlign=0x%x!\n", pNetPool->pAttrSet->ctrlAlign);
		printf("\t\tpAttrSet->clusterAlign=0x%x!\n", pNetPool->pAttrSet->clusterAlign);
	}
#endif /* VXWORKS_PCD10 */
	
	/* cluster table */
	for ( clTbIdx= 0; clTbIdx < CL_TBL_SIZE; clTbIdx++)
	{
		pClPool = pNetPool->clTbl[clTbIdx];
		if (pClPool)
		{
			printf("\tpNetPool->clTbl[%d] = 0x%x, pClPool=0x%x!\n", clTbIdx, pNetPool->clTbl[clTbIdx], pClPool);
			printf("\t\tpClPool->clSize=%d!\n", pClPool->clSize);
			printf("\t\tpClPool->clLg2 = %d!\n", pClPool->clLg2);
			printf("\t\tpClPool->pNetPool=0x%x!\n", pClPool->pNetPool);
			printf("\t\tpClPool->pClHead=0x%x!\n", pClPool->pClHead);
			printf("\t\tpClPool->clNum=%d!\n", pClPool->clNum);
			printf("\t\tpClPool->clNumFree=%d!\n", pClPool->clNumFree);

			pClBuf = pClPool->pClHead;
			while(pClBuf != NULL)
			{
				if ((((ULONG)pClBuf) % 16) != 0)
					printf("pClBuf(0x%x) is not in the cache_align addr!\n", pClBuf);
				pClBuf = pClBuf->pClNext;
			}
		}
	}

	
	if (bTestPool)
	{
		int count = 0, i, size;
		M_BLK_ID pTuple;
		PULONG pTupleArray = NULL;
		
		pTupleArray = malloc(sizeof(void *) * pNetPool->mBlkCnt);
		if (pTupleArray)
		{
			memset((PUCHAR)pTupleArray, 0, sizeof(pTupleArray));
			count = i = 0;
			do
			{
				pTuple = RtmpVxNetTupleGet(pNetPool, 100, M_DONTWAIT, MT_DATA, 1);
				if (pTuple == NULL)
				{
					DBGPRINT(RT_DEBUG_ERROR, ("%s(): RtmpVxNetTupleGet with size(100) failed!\n", __FUNCTION__));
					break;
				}
				pTupleArray[count] = (ULONG)pTuple;
				count++;
#if 0
				printf("Count[%d]: pTuple=0x%x - 0x%x!\n", count, pTuple, pTupleArray[count]);
				if (count < 5)
				{
					hex_dump_mBlk(__FUNCTION__, pTuple, TRUE);
					NdisZeroMemory(pTuple->pClBlk->clNode.pClBuf, (pTuple->mBlkHdr.mData - pTuple->pClBlk->clNode.pClBuf));
				}
#endif
			}while ((pTuple != NULL) && (count <pNetPool->mBlkCnt));

			printf("Total Count=%d, mclBlkConfig.mBlkNum=%d!\n", count, pNetPool->mBlkCnt);
			printf("Free the all allocted tuple back!\n");
			for (i = 0 ; i < count; i++)
			{
				if (pTupleArray[i])
					netMblkClChainFree((M_BLK_ID)pTupleArray[i]);
			}
			free(pTupleArray);
			
			printf("After test the pool:\n");
			printf("\tpNetPool->pmBlkHead=0x%x!\n", pNetPool->pmBlkHead);
			printf("\tpNetPool->pClBlkHead=0x%x!\n", pNetPool->pClBlkHead);
			printf("\tpNetPool->mBlkCnt=0x%x!\n", pNetPool->mBlkCnt);
			printf("\tpNetPool->mBlkFree=0x%x!\n", pNetPool->mBlkFree);

			/* statistics */
			if (pNetPool->pPoolStat != NULL)
			{
				printf("\tpNetPool->pPoolStat:\n");
				printf("\t\tpPoolStat->mNum=%d!\n", pNetPool->pPoolStat->mNum);
				printf("\t\tpPoolStat->mDrops=%d!\n", pNetPool->pPoolStat->mDrops);
				printf("\t\tpPoolStat->mWait=%d!\n", pNetPool->pPoolStat->mWait);
				printf("\t\tpPoolStat->mDrain=%d!\n", pNetPool->pPoolStat->mDrain);
			}
		}
		
		for (size = 64; size < 4096; size*=2)
		{
			pTuple = RtmpVxNetTupleGet(pNetPool, i, M_DONTWAIT, MT_DATA, 1);
			if (pTuple == NULL)
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): RtmpVxNetTupleGet with size(i) failed!\n", __FUNCTION__, size));
			else
				netMblkClChainFree(pTuple);
		}
	}

	printf("\n----------\n");
	
}


/***********************************************************************************
 *
 *	Definition of timer and time related functions
 *
 ***********************************************************************************/
 
/* timeout -- ms */
VOID RTMP_SetPeriodicTimer(
	IN	NDIS_MINIPORT_TIMER *pTimer, 
	IN	unsigned long timeout)
{
	STATUS status = ERROR;
	
	if (!(pTimer->id))
		pTimer->id = wdCreate();

	if (pTimer->id)
	{	timeout = (timeout * sysClkRateGet()) /1000;
		status = wdStart(pTimer->id, timeout, pTimer->func, (int)pTimer->data);
	}
	
	if (status == ERROR)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("setPeriodicTimer failed!\n"));
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
	pTimer->id = wdCreate();
	if (pTimer->id)
	{
		pTimer->func = (FUNCPTR)function;
		pTimer->data = data;
	}
	else
	{
		PRALINK_TIMER_STRUCT	pTimer;

		pTimer = (PRALINK_TIMER_STRUCT)data;
		pTimer->Valid = FALSE;
		DBGPRINT(RT_DEBUG_ERROR, ("init_Timer failed!\n"));
	}
}


VOID RTMP_OS_Add_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer,
	IN	unsigned long timeout)
{
	STATUS status = ERROR;
	
	if (!(pTimer->id))
		pTimer->id = wdCreate();
		
	if (pTimer->id)
	{
		timeout = (timeout * sysClkRateGet()) /1000;
		status = wdStart (pTimer->id, timeout, pTimer->func, (int)pTimer->data);
	}
	
	if (status == ERROR)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Add_Timer failed!\n"));
	}
}


VOID RTMP_OS_Mod_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer,
	IN	unsigned long timeout)
{
	STATUS status = ERROR;
	
	if (!(pTimer->id))
		pTimer->id = wdCreate();
		
	if (pTimer->id)
	{
		timeout = (timeout * sysClkRateGet()) /1000;
		status = wdStart (pTimer->id, timeout, pTimer->func, (int)pTimer->data);
	}

	if (status == ERROR)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Mod_Timer failed!\n"));
	}
}


VOID RTMP_OS_Del_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer,
	OUT	BOOLEAN					*pCancelled)
{
	STATUS status = OK;

	if (pTimer->id)
		status = wdDelete(pTimer->id);

	if (status == OK)
	{
		pTimer->id = NULL;
		*pCancelled = TRUE;
	}
	else
	{
		logMsg("CancelTimer(func=0x%x) failed!\n", (ULONG)pTimer->func,0,0,0,0,0);
	}
}

VOID RTMP_OS_Release_Timer(
	IN	NDIS_MINIPORT_TIMER		*pTimer)
{
}

/* Unify all delay routine by using udelay */
VOID RtmpusecDelay(ULONG usec)
{
	sysUSecDelay((UINT32)usec);
}


VOID RtmpOsMsDelay(
	IN	ULONG	msec)
{
	mdelay(msec);
}


long RtmpMSleep(IN ULONG mSec)
{
    struct timespec ntp, otp;
 
    ntp.tv_sec = 0;
    ntp.tv_nsec = mSec * 1000;
 
    nanosleep(&ntp, &otp);
 
    return((unsigned)otp.tv_sec);	
}


void RTMP_GetCurrentSystemTime(LARGE_INTEGER *NowTime)
{
	NowTime->u.HighPart = 0;
	time((time_t *)&NowTime->u.LowPart);
}


/*******************************************************************************

	Flash Read/Write related functions.
	
 *******************************************************************************/
#ifdef RTMP_RBUS_SUPPORT
#ifndef CONFIG_RALINK_FLASH_API
void FlashWrite(UCHAR *p, ULONG offset, ULONG size)
{
	sysFlashSet(p, size, offset + RTMP_FLASH_BASE_ADDR);
}


void FlashRead(UCHAR *p, ULONG a, ULONG b)
{
	UCHAR	*pPtr, *pAddr;
	ULONG 	offset;
	
	pAddr = (UCHAR *)(RTMP_FLASH_BASE_ADDR + a);
	pPtr = p;
	for(offset =0 ; offset < b; offset++, pAddr++, pPtr++)
	{
		*pPtr = *pAddr;
	}

	hex_dump("FlashRead", p, b);

}
#endif /* CONFIG_RALINK_FLASH_API */
#endif /* RTMP_RBUS_SUPPORT */

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
	if (INT_CONTEXT())
		logMsg("%s(): kmalloc memory pointer=0x%x!\n", __FUNCTION__, (unsigned long)(*mem),0,0,0,0);
	
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
	return 0; 
}


int RtmpOSIRQRelease(
	IN	PNET_DEV				pNetDev,
	IN	UINT32					infType,
	IN	PPCI_DEV				pci_dev,
	IN	BOOLEAN					*pHaveMsi)
{
	return 0;
}


/*******************************************************************************

	File open/close related functions.
	
 *******************************************************************************/
RTMP_OS_FD RtmpOSFileOpen(char *pPath,  int flag, int mode)
{
	return open(pPath, flag, mode);
}

int RtmpOSFileClose(RTMP_OS_FD osfd)
{
	return close(osfd);
}


void RtmpOSFileSeek(RTMP_OS_FD osfd, int offset)
{
	lseek(osfd, offset, SEEK_SET);
}


int RtmpOSFileRead(RTMP_OS_FD osfd, char *pDataPtr, int readLen)
{
	return read(osfd, pDataPtr, readLen);
}


int RtmpOSFileWrite(RTMP_OS_FD osfd, char *pDataPtr, int writeLen)
{
	return write(osfd, pDataPtr, writeLen);
}


void RtmpOSFSInfoChange(RTMP_OS_FS_INFO *pOSFSInfo, BOOLEAN bSet)
{
	return;
}



/*******************************************************************************

	Task create/management/kill related functions.
	
 *******************************************************************************/
#ifndef NET_TASK_PRIORITY
#define NET_TASK_PRIORITY       50
#endif
NDIS_STATUS RtmpOSTaskAttach(
	IN RTMP_OS_TASK *pTask, 
	IN RTMP_OS_TASK_CALLBACK fn, 
	IN ULONG arg)
{
	NDIS_STATUS status;
	
	pTask->taskPID = taskSpawn(pTask->taskName, NET_TASK_PRIORITY, RTMP_OS_MGMT_TASK_FLAGS, 4096, 
					(FUNCPTR)fn, (int)arg,  0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (pTask->taskPID == ERROR) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Attach task(%s) failed!\n", pTask->taskName));
		pTask->taskPID = -1;
		status = NDIS_STATUS_FAILURE;
	}
	else
		status = NDIS_STATUS_SUCCESS;
	
	return status;
}


NDIS_STATUS RtmpOSTaskInit(
	IN RTMP_OS_TASK *pTask,
	RTMP_STRING *pTaskName,
	VOID *pPriv,
	LIST_HEADER				*pTaskList,
	LIST_HEADER				*pSemList)
{
	int len;
	
	ASSERT(pTask);

	NdisZeroMemory((PUCHAR)(pTask), sizeof(RTMP_OS_TASK));

	len = strlen(pTaskName);
	len = len > (RTMP_OS_TASK_NAME_LEN -1) ? (RTMP_OS_TASK_NAME_LEN-1) : len;
	NdisMoveMemory(&pTask->taskName[0], pTaskName, len); 
	
	RTMP_SEM_EVENT_INIT_LOCKED(&(pTask->taskSema), &pAd->RscSemMemList);

	pTask->priv = pPriv;
	
	/* TODO: For vxWorks, we need to implement a wait_completion mechanism to make sure task running successfully. */
	/*init_completion (&pTask->taskComplete); */
	
	return NDIS_STATUS_SUCCESS;
}


INT RtmpOSTaskNotifyToExit(
	IN RTMP_OS_TASK *pTask)
{
	/* TODO: Shall we do any customization for vxWorks tasks? */
	return 0;
}


void RtmpOSTaskCustomize(
	IN RTMP_OS_TASK *pTask)
{
	/* TODO: Shall we do any customization for vxWorks tasks? */
	return;
}


NDIS_STATUS RtmpOSTaskKill(
	IN RTMP_OS_TASK *pTask)
{
	STATUS retVal;
	
	RTMP_SEM_EVENT_DESTORY(&pTask->taskSema);
	retVal = taskDelete(pTask->taskPID);

	pTask->task_killed = 0;
	
	return NDIS_STATUS_SUCCESS;
}


#ifdef VXWORKS_5X
M_BLK_ID RtmpVxNetTupleGet(
	NET_POOL_ID		pNetPool,	/* pointer to the net pool */
	int			bufSize,	/* size of the buffer to get */
	int			canWait,	/* wait or don't wait */
	UCHAR		type,		/* type of data */
	BOOL		bestFit		/* TRUE/FALSE */
)
{
	M_BLK_ID pMblk;
	int actualBufSize;

	actualBufSize = bufSize + NETBUF_LEADING_SPACE;
	pMblk = netTupleGet(pNetPool, actualBufSize, canWait, type, /*bestFit*/ FALSE);
	if (pMblk)
	{
		pMblk->mBlkHdr.mData  = (caddr_t)(pMblk->pClBlk->clNode.pClBuf + NETBUF_LEADING_SPACE);
		NdisZeroMemory(pMblk->pClBlk->clNode.pClBuf, CB_MAX_OFFSET);
	}
	else
	{
#if 0
		CL_POOL 		*pClPool;
		pClPool = pNetPool->clTbl[0];
		if (INT_CONTEXT())
		{
			logMsg("int context, pClPool->clNumFree=%d, clSize=%d, netBufSize=%d! errno=0x%x, MEMORY=0x%x, M_netBufLib=0x%x!\n", 
					pClPool->clNumFree, pClPool->clSize, bufSize,errnoGet(),S_netBufLib_NO_POOL_MEMORY,M_netBufLib);
		}
		else
		{
			printf("pClPool->clNumFree=%d, clSize=%d, netBufSize=%d! errno=0x%x, MEMORY=0x%x, M_netBufLib=0x%x!\n", 
					pClPool->clNumFree, pClPool->clSize, bufSize,errno,S_netBufLib_NO_POOL_MEMORY,M_netBufLib);
		}
#endif
	}
	return pMblk;
	
}
#endif /* VXWORKS_5X */

#ifdef VXWORKS_6X
M_BLK_ID RtmpVxNetTupleGet(
	NET_POOL_ID		pNetPool,	/* pointer to the net pool */
	int			bufSize,	/* size of the buffer to get */
	int			canWait,	/* wait or don't wait */
	UCHAR		type,		/* type of data */
	BOOL		noSmaller	/* TRUE/FALSE */
)
{
	return netTupleGet(pNetPool, bufSize, canWait, type, noSmaller);
}

#endif /* VXWORKS_6X */


PNDIS_PACKET RtmpOSNetPktAlloc(
	IN RTMP_ADAPTER *pAd, 
	IN int size)
{
	struct mBlk *pMblk;
	/* Add 2 more bytes for ip header alignment*/
	pMblk = RtmpVxNetTupleGet(pAd->net_dev->pNetPool, (size+2), M_DONTWAIT, MT_DATA, 1);
	
	return ((PNDIS_PACKET)pMblk);
}


PNDIS_PACKET RTMP_AllocateFragPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length)
{
	struct mBlk *pMblk;
	POS_COOKIE	pObj;

	
	pObj = (POS_COOKIE)pAd->OS_Cookie;
	pMblk = RtmpVxNetTupleGet(pObj->pNetPool[RTMP_NETPOOL_RX], Length, M_DONTWAIT, MT_DATA, 1);
	
	if (!pMblk)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("can't allocate frag rx %ld size packet\n",Length));
	}

	return (PNDIS_PACKET) pMblk;
	
}

#if 0
VOID build_tx_packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR	pFrame,
	IN	ULONG	FrameLen)
{
	M_BLK_ID	pTxPkt;

	ASSERT(pPacket);
	pTxPkt = RTPKT_TO_OSPKT(pPacket);

	NdisMoveMemory(m_data_put(pTxPkt, FrameLen), pFrame, FrameLen);
	GET_OS_PKT_TOTAL_LEN(pTxPkt) += FrameLen;
}

PNDIS_PACKET RTMP_AllocateTxPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress)
{
	M_BLK_ID pPkt;

	pPkt = RtmpVxNetTupleGet(pAd->net_dev->pNetPool, Length, M_DONTWAIT, MT_DATA, 1);

	if (pPkt == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("can't allocate tx %ld size packet\n",Length));
		*VirtualAddress = (PVOID) NULL;

		return NULL;
	}

	*VirtualAddress = (PVOID) pPkt->m_data;	

	return (PNDIS_PACKET) pPkt;
}

BOOLEAN OS_Need_Clone_Packet(void)
{
    /* Shiang: Not used now!! */
    return FALSE;
}

NDIS_STATUS RTMPCloneNdisPacket(
	IN	PRTMP_ADAPTER	pAd,
	IN	BOOLEAN			pInsAMSDUHdr,
	IN	PNDIS_PACKET	pInPacket,
	OUT PNDIS_PACKET   *ppOutPacket)
{
    /* Shiang: Not used now!! */
    return NDIS_STATUS_SUCCESS;
}
#endif /* Unused */

/* the allocated NDIS PACKET must be freed via RTMPFreeNdisPacket() */
NDIS_STATUS RTMPAllocateNdisPacket(
	IN	PRTMP_ADAPTER	pAd,
	OUT PNDIS_PACKET   *ppPacket,
	IN	PUCHAR			pHeader,
	IN	UINT			HeaderLen,
	IN	PUCHAR			pData,
	IN	UINT			DataLen)
{
	PNDIS_PACKET	pPacket;

	
	ASSERT(pData);
	ASSERT(DataLen);

	pPacket = (PNDIS_PACKET) RtmpVxNetTupleGet(pAd->net_dev->pNetPool, HeaderLen + DataLen, M_DONTWAIT, MT_DATA, 1);
	if (pPacket == NULL)
	{
		*ppPacket = NULL;
		DBGPRINT(RT_DEBUG_ERROR, ("RTMPAllocateNdisPacket Fail\n\n"));
		return NDIS_STATUS_FAILURE;
	}

	/* 2. clone the frame content */
	if ((HeaderLen > 0) && (pHeader != NULL))
	{
		NdisMoveMemory(GET_OS_PKT_DATAPTR(pPacket), pHeader, HeaderLen);
	}
	if ((DataLen > 0) && (pData != NULL))
	{
		NdisMoveMemory(GET_OS_PKT_DATAPTR(pPacket) + HeaderLen, pData, DataLen);
	}

	/* 3. update length of packet */
	GET_OS_PKT_LEN(pPacket) = (HeaderLen + DataLen);

/*	DBGPRINT(RT_DEBUG_TRACE, ("%s : pPacket = %p, len = %d\n", */
/*				__FUNCTION__, pPacket, GET_OS_PKT_LEN(pPacket))); */
	*ppPacket = pPacket;
	return NDIS_STATUS_SUCCESS;
}


/*
  ========================================================================
  Description:
	This routine frees a miniport internally allocated NDIS_PACKET and its
	corresponding NDIS_BUFFER and allocated memory.
  ========================================================================
*/
VOID RTMPFreeNdisPacket(
	IN PRTMP_ADAPTER pAd,
	IN PNDIS_PACKET  pPacket)
{
/*
	What purpose for PktHdr->mNext = 0?

	History:
		We will use packet->Next pointer to link next packet in our software
		queue.  You can see in InsertTailQueue(),
		(QueueHeader)->Tail->Next = (PQUEUE_ENTRY)(QueueEntry);

	Problem:
		But in vxWorks, packet->Next is mNext, not mNextPkt, if we free a packet
		without mNext = 0, in mBlkFree() of vxWorks, it will reset the Next of
		next packet to the freed packet, such as

		1. head --> packet a --> packet b --> packet c
		2. prepare to free packet a
			a. head --> packet b --> packet c
			b. the Next pointer of packet b is the packet c
		3. the Next pointer of packet a is the packet b, not 0; free packet a
		4. mBlkFree() will change the Next pointer of packet b to the packet a
		5. so we can not free packet c, the Next pointer of packet b is modified
			to the wrong address.

	Solution:
		We must change the mNext to 0 before we free the packet.

		And we will merge packets of same chain when we send the packet, i.e.
		mNext of packet is not 0.
		So always single packet is put in our queue.

	Reference to Tornado2.2\target\h\wrn\coreip\netBufLib.h:
		typedef struct mHdr
		{
			struct mBlk *	mNext;		==> next buffer in chain
			struct mBlk *	mNextPkt;	==> next chain in queue/record
			char *		mData;
			int			mLen;
			UINT16		mType;
			UINT16		mFlags;
			UINT16		reserved;
			UINT16		offset1;
	    } M_BLK_HDR;
*/

	M_BLK_ID pMblk = (M_BLK_ID)pPacket;
	POS_COOKIE pObj;
	NET_POOL_ID pNetPool;
	
	pObj = (POS_COOKIE)(pAd->OS_Cookie);
	pNetPool = pMblk->pClBlk->pNetPool;

	if (pNetPool == pObj->pNetPool[RTMP_NETPOOL_RX]  || pNetPool == pObj->pNetPool[RTMP_NETPOOL_TX])
		pMblk->mBlkHdr.mNext = 0;

#ifdef VXWORKS_5X
	SET_OS_PKT_NETDEV(pPacket, NULL);
#endif /* VXWORKS_5X */
	netMblkClChainFree(pMblk);

}


void * m_data_put(struct mBlk *skb, int n)
{
	PUCHAR p;

	ASSERT(skb);

	p = GET_OS_PKT_DATATAIL(skb);
	skb->m_len += n;
		
	/* TODO: should we check if skb->m_data + n > end of buffer?? */
	
	return p;
}


void * m_data_push(struct mBlk *pMblkBuf, int n)
{
	ASSERT(pMblkBuf);

	pMblkBuf->mBlkHdr.mData -= n;
	pMblkBuf->mBlkHdr.mLen += n;
	pMblkBuf->mBlkPktHdr.len += n;
		
	return pMblkBuf->m_data;
}


PNDIS_PACKET skb_copy(PNDIS_PACKET pSrcPkt, INT flags)
{
	M_BLK_ID pNewMblk, pOldMblk;
	struct netPool *pNetPool;
	struct end_object *pEndObj;
	int bufLen;
	
	pOldMblk = (M_BLK_ID)pSrcPkt;
	pEndObj = (END_OBJ *)(pOldMblk->mBlkPktHdr.rcvif);
	if (pEndObj == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): pEndObj is NULL!\n", __FUNCTION__));
		return NULL;
	}

	pNetPool = pEndObj->pNetPool;
	bufLen = ((pOldMblk->mBlkPktHdr.len > pOldMblk->mBlkHdr.mLen) ? pOldMblk->mBlkPktHdr.len : pOldMblk->mBlkHdr.mLen);
	pNewMblk = RtmpVxNetTupleGet(pNetPool, bufLen /*pOldMblk->mBlkHdr.mLen*/, M_DONTWAIT, MT_DATA, 1);
	if (pNewMblk == NULL)
	{
		END_ERR_ADD(pEndObj, MIB2_OUT_ERRS, +1);
		DBGPRINT(RT_DEBUG_ERROR, ("%s Tx: cannot get mblk\n", RTMP_OS_NETDEV_GET_DEVNAME(pEndObj)));
		return NULL;
	}

	bufLen = netMblkToBufCopy (pOldMblk, pNewMblk->mBlkHdr.mData, NULL);
	ASSERT((pOldMblk->mBlkPktHdr.len == bufLen));
	
	pNewMblk->mBlkHdr.mLen 	= bufLen;
	pNewMblk->mBlkHdr.mType = pOldMblk->mBlkHdr.mType;
	pNewMblk->mBlkHdr.mFlags = pOldMblk->mBlkHdr.mFlags;
	pNewMblk->mBlkPktHdr.len = pOldMblk->mBlkPktHdr.len;
	SET_OS_PKT_NETDEV(pNewMblk, pEndObj);

	return (PNDIS_PACKET)pNewMblk;
		
}


PNDIS_PACKET skb_clone(PNDIS_PACKET pSrcPkt, int flags)
{
	M_BLK_ID pNewMblk, pOldMblk;
	struct netPool *pNetPool;
	/*END_OBJ  *pEndObj; */

	pOldMblk = (M_BLK_ID)pSrcPkt;
	pNetPool = MBLK_TO_NET_POOL(pOldMblk);

	pNewMblk = NULL;
	if (pNetPool)
	{
		pNewMblk = netMblkGet (pNetPool,  M_DONTWAIT, MT_DATA);
	if (pNewMblk == NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): get Mblk failed!\n", __FUNCTION__));
		return NULL;
	}

		if (NULL == netMblkDup(pOldMblk, pNewMblk))
	{
			netMblkFree (pNetPool, pNewMblk);
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): call netMblkDup() failed!\n", __FUNCTION__));
		return NULL;
	}
	}
	return pNewMblk;
}


void RTMP_QueryPacketInfo(
	IN  PNDIS_PACKET pPacket,
	OUT PACKET_INFO  *pPacketInfo,
	OUT PUCHAR		 *pSrcBufVA,
	OUT	UINT		 *pSrcBufLen)
{
	pPacketInfo->BufferCount = 1;
	pPacketInfo->pFirstBuffer = GET_OS_PKT_DATAPTR(pPacket);
	pPacketInfo->PhysicalBufferCount = 1;
	pPacketInfo->TotalPacketLength = GET_OS_PKT_LEN(pPacket);

	*pSrcBufVA = GET_OS_PKT_DATAPTR(pPacket);
	*pSrcBufLen = GET_OS_PKT_LEN(pPacket); 
}

#if 0
void RTMP_QueryNextPacketInfo(
	IN  PNDIS_PACKET *ppPacket,
	OUT PACKET_INFO  *pPacketInfo,
	OUT PUCHAR		 *pSrcBufVA,
	OUT	UINT		 *pSrcBufLen)
{
    /* Shiang: Not used now!! */
}
#endif /* Unused */


UCHAR VLAN_8023_Header_Copy(
	IN	USHORT VLAN_VID,
	IN	USHORT VLAN_Priority,
	IN	UCHAR *pHeader802_3,
	IN	UINT HdrLen,
	OUT UCHAR *pData,
	IN	UCHAR *TPID)
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


PNDIS_PACKET ClonePacket(PNET_DEV ndev, PNDIS_PACKET pkt, UCHAR *buf, ULONG sz)
{
	M_BLK_ID pClonedMblk, pOldMblk;
	struct netPool *pNetPool;
	END_OBJ  *pEndObj;

	ASSERT(sz < 1530);	
	
	pEndObj = (END_OBJ *)ndev;
	pClonedMblk = skb_clone(pkt, MEM_ALLOC_FLAG);
	if (pClonedMblk != NULL)
	{
	    	/* set the correct dataptr and data len */
	    	SET_OS_PKT_NETDEV(pClonedMblk, pEndObj);
		pClonedMblk->mBlkHdr.mData = buf;
		pClonedMblk->mBlkHdr.mLen = sz;
		pClonedMblk->mBlkPktHdr.len = sz;
	}
	
	return pClonedMblk;
}


/* TODO: need to check is there any diffence between following four functions???? */
PNDIS_PACKET DuplicatePacket(PNET_DEV pNetDev, PNDIS_PACKET pPacket)
{
	PNDIS_PACKET	pRetPacket = NULL;
	USHORT			DataSize;
	UCHAR			*pData;

	DataSize = (USHORT) GET_OS_PKT_LEN(pPacket);
	pData = (PUCHAR) GET_OS_PKT_DATAPTR(pPacket);	


	pRetPacket = skb_clone(RTPKT_TO_OSPKT(pPacket), MEM_ALLOC_FLAG);
	if (pRetPacket)
		SET_OS_PKT_NETDEV(pRetPacket, pNetDev);

	/*DBGPRINT(RT_DEBUG_TRACE, ("%s(): pRetPacket = 0x%x!\n", __FUNCTION__, (unsigned int)pRetPacket)); */
	
	return pRetPacket;
}


PNDIS_PACKET duplicate_pkt_vlan(
	IN PNET_DEV pNetDev,
	IN USHORT VLAN_VID,
	IN USHORT VLAN_Priority,
	IN PUCHAR pHeader802_3,
	IN UINT HdrLen,
	IN PUCHAR pData,
	IN ULONG DataSize,
	IN UCHAR *TPID)
{
	M_BLK_ID		pMblk;
	POS_COOKIE		pObj;
	PNDIS_PACKET	pPacket = NULL;
	UINT16			VLAN_Size;
	int bufLen = HdrLen + DataSize + 2;
	PUCHAR			pMdataPtr;
	END_OBJ 		*pEndObj;

#ifdef WIFI_VLAN_SUPPORT
	if (VLAN_VID != 0)
		bufLen += LENGTH_802_1Q;
#endif /* WIFI_VLAN_SUPPORT */

	pObj = pAd->OS_Cookie;
	pMblk = RtmpVxNetTupleGet(pObj->pNetPool[RTMP_NETPOOL_TX], bufLen, M_DONTWAIT, MT_DATA, 1);

	if (pMblk != NULL)
	{
		skb_reserve(pMblk, 2);

		/* copy header (maybe +VLAN tag) */
		VLAN_Size = VLAN_8023_Header_Copy(VLAN_VID, VLAN_Priority,
											pHeader802_3, HdrLen,
											pMblk->m_data, TPID);
		skb_put(pMblk, HdrLen + VLAN_Size);

		/* copy data body */
		pMdataPtr = GET_OS_PKT_DATATAIL(pMblk);
		NdisMoveMemory(pMdataPtr, pData, DataSize);
		skb_put(pMblk, DataSize);
		
		pEndObj = (END_OBJ *)pNetDev;
		SET_OS_PKT_NETDEV(pMblk, pEndObj);

		pPacket = OSPKT_TO_RTPKT(pMblk);
	}

	return pPacket;
}


#define TKIP_TX_MIC_SIZE		8
PNDIS_PACKET duplicate_pkt_with_TKIP_MIC(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket)
{
	M_BLK_ID		pMblk, pNewMblk;
	int 				bufLen, tailRoom;
	POS_COOKIE		pObj;
	struct end_object *pEndObj;
	
	pObj = pAd->OS_Cookie;
	pMblk = RTPKT_TO_OSPKT(pPacket);
	pEndObj = GET_OS_PKT_NETDEV(pMblk);
	tailRoom = (pMblk->pClBlk->clNode.pClBuf + pMblk->pClBlk->clSize) - (pMblk->m_data + pMblk->m_len);
	if (tailRoom < TKIP_TX_MIC_SIZE)
	{
		bufLen = pMblk->m_len + TKIP_TX_MIC_SIZE;
		/* alloc a new skb and copy the packet */
		pNewMblk = RtmpVxNetTupleGet(pObj->pNetPool[RTMP_NETPOOL_TX], bufLen, M_DONTWAIT, MT_DATA, 1);
		if (pNewMblk == NULL)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Extend Tx.MIC for packet failed!, dropping packet!\n"));
			return NULL;
		}

		bufLen = netMblkToBufCopy(pMblk, pNewMblk->mBlkHdr.mData, NULL);
		ASSERT((pMblk->mBlkPktHdr.len == bufLen));
	
		pNewMblk->mBlkHdr.mLen 	= bufLen;
		pNewMblk->mBlkHdr.mType = pMblk->mBlkHdr.mType;
		pNewMblk->mBlkHdr.mFlags = pMblk->mBlkHdr.mFlags;
		pNewMblk->mBlkPktHdr.len = pMblk->mBlkPktHdr.len;
		SET_OS_PKT_NETDEV(pNewMblk, pEndObj);

		/* release old packet and assign the pointer to the new one */
		RTMPFreeNdisPacket(pAd, pMblk);
		pMblk = pNewMblk;
	}
		
	return OSPKT_TO_RTPKT(pMblk);
}


/* */
/* change OS packet DataPtr and DataLen */
/* */
VOID RtmpOsPktInit(
	IN	PNDIS_PACKET		pRxPacket,
	IN	PNET_DEV			pNetDev,
	IN	UCHAR				*pData,
	IN	USHORT				DataSize)
{
	struct mBlk	*pMblkPkt;

	ASSERT(pRxPacket);
	pMblkPkt = RTPKT_TO_OSPKT(pRxPacket);

	SET_OS_PKT_NETDEV(pMblkPkt, pNetDev);
	pMblkPkt->mBlkHdr.mData = pData;
	pMblkPkt->mBlkHdr.mLen = DataSize;
	pMblkPkt->mBlkPktHdr.len  = DataSize;
	pMblkPkt->mBlkHdr.mFlags |= (M_PKTHDR | M_EXT);
	pMblkPkt->m_next = NULL;
}


void wlan_802_11_to_802_3_packet(
	IN PNET_DEV pNetDev,
	IN UCHAR OpMode,
	IN USHORT VLAN_VID,
	IN USHORT VLAN_Priority,
	IN PNDIS_PACKET pRxPacket,
	IN UCHAR *pData,
	IN ULONG DataSize,
	IN PUCHAR pHeader802_3,
	IN UCHAR *TPID)
{
	struct mBlk	*pMblkPkt;

	ASSERT(pRxBlk->pRxPacket);
	ASSERT(pHeader802_3);

	pMblkPkt = RTPKT_TO_OSPKT(pRxBlk->pRxPacket);
	SET_OS_PKT_NETDEV(pMblkPkt, pNetDev);

/*	DBGPRINT(RT_DEBUG_TRACE, ("pRxBlk->pData=0x%x, DataSize=%d!\n", pRxBlk->pData, pRxBlk->DataSize)); */
/*	hex_dump("pRxBlk", pRxBlk->pData, pRxBlk->DataSize); */
	pMblkPkt->mBlkHdr.mData = pRxBlk->pData;
	pMblkPkt->mBlkHdr.mLen = pMblkPkt->mBlkPktHdr.len = pRxBlk->DataSize;
	pMblkPkt->mBlkHdr.mFlags |= (M_PKTHDR | M_EXT);
	pMblkPkt->mBlkHdr.mType = MT_DATA;
	pMblkPkt->m_next = NULL;
	
		
	/* copy 802.3 header */
#ifdef CONFIG_AP_SUPPORT
/*	IF_DEV_CONFIG_OPMODE_ON_AP(pAd) */
	if (OpMode == OPMODE_AP)
	{
		/* maybe insert VLAN tag to the received packet */
		UCHAR VLAN_Size = 0;
		UCHAR *data_p;

		/* VLAN related */
		if (VLAN_VID != 0)
			VLAN_Size = LENGTH_802_1Q;

		data_p = skb_push(pMblkPkt, LENGTH_802_3+VLAN_Size);

		VLAN_8023_Header_Copy(VLAN_VID, VLAN_Priority,
								pHeader802_3, LENGTH_802_3,
								data_p, TPID);
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	if (OpMode == OPMODE_STA)
		NdisMoveMemory(skb_push(pMblkPkt, LENGTH_802_3), pHeader802_3, LENGTH_802_3);
#endif /* CONFIG_STA_SUPPORT */
/*	DBGPRINT(RT_DEBUG_TRACE, ("After Header Copy: pMblkPkt->mData=0x%x, mLen=%d, len=%d!\n", */
/*				pMblkPkt->mBlkHdr.mData, pMblkPkt->mBlkHdr.mLen, pMblkPkt->mBlkPktHdr.len)); */
/*	hex_dump("pMblkPkt", pMblkPkt->mBlkHdr.mData, pMblkPkt->mBlkHdr.mLen); */

}


void announce_802_3_packet(
	IN	VOID			*pAdSrc,
	IN	PNDIS_PACKET	pPacket)
{
	END_OBJ 	*pEndObj;

	pEndObj  = GET_OS_PKT_NETDEV(pPacket);
	
#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (MATPktRxNeedConvert(pAd, pEndObj))
			MATEngineRxHandle(pAd, pPacket, 0);
	}
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef ETH_CONVERT_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->EthConvert.ECMode & ETH_CONVERT_MODE_DONGLE)
			MATEngineRxHandle(pAd, pPacket, 0);
	}
#endif /* ETH_CONVERT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	/*hex_dump_mBlk(__FUNCTION__, (M_BLK_ID)pPacket, TRUE); */

#ifdef VXWORKS_5X
	SET_OS_PKT_NETDEV(pPacket, NULL);
#endif /* VXWORKS_5X */

#ifdef CACHE_DMA_SUPPORT
	memcpy(GET_OS_PKT_HEAD(pPacket), CACHE_DMA_SIGNAUTURE, sizeof(CACHE_DMA_SIGNAUTURE));
#endif /* CACHE_DMA_SUPPORT */

	END_RCV_RTN_CALL(pEndObj, pPacket);
}


PRTMP_SCATTER_GATHER_LIST
rt_get_sg_list_from_packet(PNDIS_PACKET pPacket, RTMP_SCATTER_GATHER_LIST *sg)
{
    /*Shiang: Not used now!!!! */
    return NULL;
}

#ifdef SYSTEM_LOG_SUPPORT
/* Shiang: This function is Linux specific feature and should we mapping to a VxWorks-style function??? */
VOID RTMPSendWirelessEvent(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Event_flag,
	IN	PUCHAR 			pAddr,
	IN	UCHAR			wdev_idx,
	IN	CHAR			Rssi)
{
	if (pAd->CommonCfg.bWirelessEvent == FALSE)
		return;
}
#endif /* SYSTEM_LOG_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
VOID SendSignalToDaemon(
	IN INT		sig,
	IN RTMP_OS_PID	pid,
	IN unsigned long pid_no)
{
	return;
}

/*
 ========================================================================
 Routine Description:
    Send Leyer 2 Frame to notify 802.1x daemon. This is a internal command

 Arguments:

 Return Value:
	TRUE - send successfully
	FAIL - send fail

 Note:
 ========================================================================
*/
BOOLEAN Dot1xInternalCmdAction(
	IN  PRTMP_ADAPTER	pAd,
    IN  MAC_TABLE_ENTRY *pEntry,
    IN	UINT8			cmd)
{
	INT				apidx = MAIN_MBSSID;
	UCHAR 			Header802_3[14];
	UCHAR 			RalinkIe[9] = {221, 7, 0x00, 0x0c, 0x43, 0x00, 0x00, 0x00, 0x00};
	UCHAR			s_addr[MAC_ADDR_LEN];
	
	INT size = sizeof(Header802_3) + sizeof(RalinkIe);
	struct mBlk *skb = RtmpVxNetTupleGet(pAd->net_dev->pNetPool, (size+2), M_DONTWAIT, MT_DATA, 1);

	DBGPRINT(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));
	
	if (pEntry)
	{
		apidx = pEntry->func_tb_idx;
		NdisMoveMemory(s_addr, pEntry->Addr, MAC_ADDR_LEN);
	}
	else
	{
		/* Fake a Source Address for transmission */
		NdisMoveMemory(s_addr, pAd->ApCfg.MBSSID[apidx].Bssid, MAC_ADDR_LEN);
		s_addr[0] |= 0x80;
	}

	MAKE_802_3_HEADER(Header802_3, 
					  pAd->ApCfg.MBSSID[apidx].Bssid, 
					  s_addr, EAPOL);
		
	if (!skb)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Error! Can't allocate a skb.\n"));
		return FALSE;
	}
	
	SET_OS_PKT_NETDEV(skb, pAd->ApCfg.MBSSID[apidx].wdev.if_dev);

	skb_reserve(skb, 2);	/* 16 byte align the IP header */
	NdisMoveMemory(GET_OS_PKT_DATAPTR(skb), Header802_3, LENGTH_802_3);
	
	/* Prepare internal command */
	RalinkIe[5] = cmd;
	NdisMoveMemory(GET_OS_PKT_DATAPTR(skb) + LENGTH_802_3, RalinkIe, sizeof(RalinkIe));

	skb_put(GET_OS_PKT_TYPE(skb), size);

	DBGPRINT(RT_DEBUG_TRACE, ("%s : cmd(%d)\n", __FUNCTION__, cmd));

	announce_802_3_packet(pAd, skb);

	return TRUE;
}

/*
 ========================================================================
 Routine Description:
	Send Leyer 2 Frame to trigger 802.1x EAP state machine.     

 Arguments:

 Return Value:
	TRUE - send successfully
	FAIL - send fail

 Note:
 ========================================================================
*/
BOOLEAN Dot1xEapTriggerAction(
	IN  PRTMP_ADAPTER	pAd,
	IN  MAC_TABLE_ENTRY *pEntry)
{	
	return TRUE;
}	    

#endif /* CONFIG_AP_SUPPORT */


#ifdef CONFIG_SNIFFER_SUPPORT
void STA_MonPktSend(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RX_BLK			*pRxBlk)
{

}
#endif /* CONFIG_SNIFFER_SUPPORT */


void RTMP_IndicateMediaState(	
	IN	PRTMP_ADAPTER		pAd,
	IN  NDIS_MEDIA_STATE	media_state)
{


}

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


/*
  *	Assign the network dev name for created Ralink WiFi interface.
  */
static int RtmpOSNetDevRequestName(
	IN RTMP_ADAPTER *pAd, 
	IN RTMP_STRING *pPrefixStr, 
	IN INT	devIdx,
	OUT INT	*avilNum)
{
	int	ifNameIdx;
	int	Status;
	
	for (ifNameIdx = devIdx; ifNameIdx < 32; ifNameIdx++)
	{
		if (muxDevExists(pPrefixStr, ifNameIdx) == FALSE)
			break;
	}
	
	if(ifNameIdx < 32)
	{
		*avilNum = ifNameIdx;
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("Assign the net device name as %s%d\n", pPrefixStr, *avilNum));
#endif /* RELEASE_EXCLUDE */
		Status = NDIS_STATUS_SUCCESS;
	} 
	else 
	{
		DBGPRINT(RT_DEBUG_ERROR, 
					("Cannot request DevName with preifx(%s) and in range(0~32) as suffix from OS!\n", pPrefixStr));
		Status = NDIS_STATUS_FAILURE;
	}

	return Status;
}


void RtmpOSNetDevClose(
	IN PNET_DEV pNetDev)
{

}


void RtmpOSNetDevFree(PNET_DEV pNetDev)
{
	ASSERT(pNetDev);
	muxDevUnload(pNetDev->devObject.name, pNetDev->devObject.unit);
	kfree(pNetDev);
}


int RtmpOSNetDevAddrSet(
	IN UCHAR	OpMode,
	IN PNET_DEV pNetDev,
	IN PUCHAR	pMacAddr,
	IN PUCHAR	dev_name)
{
	 bcopy (pMacAddr, (char *)(&pNetDev->mib2Tbl.ifPhysAddress.phyAddress), MAC_ADDR_LEN);
	 pNetDev->mib2Tbl.ifPhysAddress.addrLength = MAC_ADDR_LEN;
	 
	return 0;
}


void RtmpOSNetDevDetach(
	IN PNET_DEV pNetDev)
{
	void *pCookie = (void *)(pNetDev);
	muxDevStop(pCookie);
}


int RtmpOSNetDevAttach(
	IN UCHAR					OpMode,
	IN PNET_DEV					pNetDev, 
	IN RTMP_OS_NETDEV_OP_HOOK	*pDevOpHook)
{
	END_OBJ *pEndObj;
	
	pEndObj = (END_OBJ *)pNetDev;
	RtmpOSNetDevAddrSet(pNetDev, &pDevOpHook->devAddr[0]);
	
	muxDevStart(pNetDev);
	
	END_FLAGS_SET (pEndObj, IFF_UP | IFF_RUNNING);
	
	return NDIS_STATUS_SUCCESS;
	
}


VOID	RTMPFreeAdapter(
	IN	VOID		*pAdSrc)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;
	POS_COOKIE os_cookie;
	
	os_cookie=(POS_COOKIE)pAd->OS_Cookie;
	free(pAd);
	free(os_cookie);
}


/*
	VxWorks sepecific functions for creations of EndDevice and realted procedures.
*/
int RtmpENDDevInitStrParse(char *initString, RTMP_VX_INIT_STRING *pInitToken)
{
	int count = 0;
	char     *tok, *pPtr = NULL; 
	int 		*pTokenValue = NULL;
	

	/*
		initString format:
			phyNum:devNum:infType:irqNum:devType:privSize:devName
	*/
	tok = strtok_r(initString, ":", &pPtr);
	pTokenValue = (int *)pInitToken;
	while(tok)
	{
		printf("tok=%s! count=%d!\n", tok, count);
		if (count == 7)
			memcpy(pInitToken->devName, tok, strlen(tok));
		else
			*(pTokenValue + count) = atoi(tok);
		tok = strtok_r (NULL, ":", &pPtr);
		count++;
	}

	printf("count=%d!\n", count);
	if (count != 8)
		return NDIS_STATUS_FAILURE;

	printf("The initString info:\n");
	printf("\tphyNum=%d!\n", pInitToken->phyNum);
	printf("\tdevNum=%d!\n", pInitToken->devNum);
	printf("\tinfType=%d!\n", pInitToken->infType);
	printf("\tirqNum=%d!\n", pInitToken->irqNum);
	printf("\tdevType=%d!\n", pInitToken->devType);
	printf("\tprivSize=%d!\n", pInitToken->privSize);
	printf("\tcsrAddr=0x%x!\n", pInitToken->csrAddr);
	printf("\tdevName=%s!\n", pInitToken->devName);
	
	return NDIS_STATUS_SUCCESS;
	
}


END_OBJ *RtmpVirtualENDDevCreate(
	IN char *initString, 
	IN void *pBSP)
{
	END_OBJ *pENDDev;
	NET_FUNCS *pEndFuncTb;
	RTMP_VX_INIT_STRING initToken;
	int status;
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)pBSP;
	/*void 			*pPrivPtr; */
	char				devName[IFNAMSIZ];

	
	printf("%s(): Into phase 2, received initSting=%s!\n", __FUNCTION__, initString);
	memset((unsigned char *)&initToken, 0, sizeof(RTMP_VX_INIT_STRING));
	status = RtmpENDDevInitStrParse(initString, &initToken);
	if(status == NDIS_STATUS_FAILURE)
	{
		printf("Parsing the initString failed!\n");
        	return NULL;
	}

	pENDDev = (END_OBJ *)malloc(sizeof(END_OBJ));
	if (pENDDev == NULL) 
	{
		printf("Could not alloc memory for device structures\n");
        	return NULL;
	}
	NdisZeroMemory(pENDDev, sizeof(END_OBJ));
	pENDDev->pNetPool = pAd->net_dev->pNetPool;

	
	switch(initToken.devType)
	{
#ifdef MBSS_SUPPORT
		case INT_MBSSID:
			pEndFuncTb = &RtmpMBSSEndFuncTable;
			break;
#endif /* MBSS_SUPPORT */
#ifdef WDS_SUPPORT
		case INT_WDS:
			pEndFuncTb = &RtmpWDSEndFuncTable;
			break;
#endif /* WDS_SUPPORT */
#ifdef APCLI_SUPPORT
		case INT_APCLI:
			pEndFuncTb = &RtmpApCliEndFuncTable;
			break;
#endif /* APCLI_SUPPORT */
#ifdef MESH_SUPPORT
		case INT_MESH:
			pEndFuncTb = &RtmpMeshEndFuncTable;
			break;
#endif /* MESH_SUPPORT */
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("ERROR: Not supported device type(%d)!\n", initToken.devType));
			goto err;
	}


	sprintf(devName, "%s%d", &initToken.devName[0], initToken.phyNum);
	printf("%s(): devName=%s!\n", __FUNCTION__, devName);
	if ((END_OBJ_INIT(pENDDev, (DEV_OBJ *)pAd, &initToken.devName[0],
					initToken.phyNum, pEndFuncTb, &devName[0]) == ERROR)) 
	{
		printf("Can not init net device %s!\n", &devName[0]);
		goto err;
	}

	if ((END_MIB_INIT(pENDDev, M2_ifType_ethernet_csmacd,
	              &ZERO_MAC_ADDR[0],  6, ETHERMTU, END_SPEED) == ERROR))
	{
		printf("Can not init MIB of net device %s!\n", RTMP_OS_NETDEV_GET_DEVNAME(pENDDev));
		return NULL;
	}

	/*END_OBJ_READY(pENDDev, IFF_NOTRAILERS |IFF_SIMPLEX |IFF_BROADCAST |IFF_ALLMULTI); */
	END_OBJ_READY(pENDDev, 	IFF_NOTRAILERS |IFF_BROADCAST |IFF_MULTICAST);

	printf("<---%s()!\n", __FUNCTION__);
	
	return pENDDev;

err:
	if (pENDDev)
		free(pENDDev);
	return NULL;
	
}


#ifdef MESH_SUPPORT
END_OBJ *RtmpMeshENDDevLoad(
	IN char *initString, 
	IN void *pBSP)
{

	ASSERT(initString);
	
	/*  First pass, we just return the device name for function "muxDevLoad()" to check if the device exists */
	if (initString[0] == '\0') {
		bcopy(INF_MESH_DEV_NAME, initString, strlen(INF_MESH_DEV_NAME));
		printf("%s():Into phase 1, return initSting=%s!\n", __FUNCTION__, initString);
		return NULL;
	}

	/*   Now we are phase 2, now do END_OBJ related initialization */
	return RtmpVirtualENDDevCreate(initString, pBSP);
	
}
#endif /* MESH_SUPPORT */


#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
END_OBJ *RtmpApCliENDDevLoad(
	IN char *initString, 
	IN void *pBSP)
{

	ASSERT(initString);
	
	/*  First pass, we just return the device name for function "muxDevLoad()" to check if the device exists */
	if (initString[0] == '\0') {
		bcopy(INF_APCLI_DEV_NAME, initString, strlen(INF_APCLI_DEV_NAME));
		printf("%s():Into phase 1, return initSting=%s!\n", __FUNCTION__, initString);
		return NULL;
	}

	/*   Now we are phase 2, now do END_OBJ related initialization */
	return RtmpVirtualENDDevCreate(initString, pBSP);
}
#endif /* APCLI_SUPPORT */


#ifdef WDS_SUPPORT
END_OBJ *RtmpWDSENDDevLoad(
	IN char *initString, 
	IN void *pBSP)
{
	ASSERT(initString);
	
	/*  First pass, we just return the device name for function "muxDevLoad()" to check if the device exists */
	if (initString[0] == '\0') {
		bcopy(INF_WDS_DEV_NAME, initString, strlen(INF_WDS_DEV_NAME));
		printf("%s():Into phase 1, return initSting=%s!\n", __FUNCTION__, initString);
		return NULL;
	}

	/*   Now we are phase 2, now do END_OBJ related initialization */
	return RtmpVirtualENDDevCreate(initString, pBSP);
}
#endif /* WDS_SUPPORT */


#ifdef MBSS_SUPPORT
END_OBJ *RtmpMBSSENDDevLoad(
	IN char *pInitString, 
	IN void *pBSP)
{	
	ASSERT(pInitString);
	
	/*  First pass, we just return the device name for function "muxDevLoad()" to check if the device exists */
	if (pInitString[0] == '\0') {
		bcopy(INF_MAIN_DEV_NAME, pInitString, strlen(INF_MAIN_DEV_NAME));
		DBGPRINT(RT_DEBUG_TRACE, ("%s():Into phase 1, return pInitString=%s!\n", __FUNCTION__, pInitString));
		return NULL;
	}

	/*   Now we are phase 2, now do END_OBJ related initialization */
	return RtmpVirtualENDDevCreate(pInitString, pBSP);
}
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


PNET_DEV RtmpOSNetDevCreate(
	IN RTMP_ADAPTER *pAd,
	IN INT 			devType, 
	IN INT			devNum,
	IN INT			privMemSize,
	IN RTMP_STRING *pNamePrefix)
{
	END_OBJ 		*pNetDev = NULL;
	struct os_cookie	*pObj = pAd->OS_Cookie;
	char        		devInitStr[32];
	int				status, availableNum;
	void 			*endLoadFunc;
	char				devName[IFNAMSIZ];


	memset(devInitStr, 0, 32);
	memset(devName, 0, IFNAMSIZ);
	switch(devType)
	{
#ifdef CONFIG_AP_SUPPORT
#ifdef MBSS_SUPPORT
		case INT_MBSSID:
			endLoadFunc = RtmpMBSSENDDevLoad;
			break;
#endif /* MBSS_SUPPORT */
#ifdef WDS_SUPPORT
		case INT_WDS:
			endLoadFunc = RtmpWDSENDDevLoad;
			break;
#endif /* WDS_SUPPORT */
#ifdef APCLI_SUPPORT
		case INT_APCLI:
			endLoadFunc = RtmpApCliENDDevLoad;
			break;
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
#ifdef MESH_SUPPORT
		case INT_MESH:
			endLoadFunc = RtmpMeshENDDevLoad;
			break;
#endif /* MESH_SUPPORT */
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("ERROR: Not supported device type(%d)!\n", devType));
			return NULL;
	}


	/*
		Prepare the devInitStr
		initString format:
			phyNum:devNum:infType:irqNum:devType:privSize:devName
	*/
	memset(&devInitStr[0], 0, sizeof(devInitStr));
	sprintf(devInitStr, "%d:%d:%d:%d:%d:0:", devNum, pAd->infType, 0, devType, privMemSize);

	ASSERT((strlen(pNamePrefix) < IFNAMSIZ));
	strcpy(devName, pNamePrefix);
#ifdef MULTIPLE_CARD_SUPPORT
	if (pAd->MC_RowID >= 0)
	{
		char suffixName[4];
		sprintf(suffixName, "%02d_", pAd->MC_RowID);
		strcat(devName, suffixName);
	}
#endif /* MULTIPLE_CARD_SUPPORT */

	ASSERT(((strlen(devInitStr) + strlen(devName)) < 32));
	strcat(devInitStr, devName);
	status = RtmpOSNetDevRequestName(pAd, devName, devNum, &availableNum);
	if (status == NDIS_STATUS_FAILURE)
	{
		printf("%s(): request devNum failed!\n", __FUNCTION__);
		return NULL;
	}

	
	/*  Find the endLoadFunc depends on the namePrefix */
	/* allocate a new network device */
	printf("%s(): Prepare to call muxDevLoad, the devInitStr=%s, unitNum=%d, availableNum=%d\n", 
			__FUNCTION__, devInitStr, pObj->unitNum, availableNum);
	if ((pNetDev = muxDevLoad(availableNum, endLoadFunc, devInitStr, FALSE, pAd)) == NULL) {
		DBGPRINT(RT_DEBUG_ERROR, ("Error in create END_OBJ(%s%d) with available num=%d!\n", 
									devInitStr, devNum, availableNum));
		return NULL;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("The name of the new %s interface is %s, unit=%d...\n", devInitStr, pNetDev->devObject.name, pNetDev->devObject.unit));
	
	return pNetDev;
}


/*
 * This function is going to be called by MuxLoad through endDevTbl which is
 * defined in configNet.h. That header file also defines the init string.
 */
END_OBJ *sysRtmpEndLoad(char *initString, void *arg)
{
	END_OBJ					*pENDDev = NULL;
	int						status;
	RTMP_VX_INIT_STRING		initToken;


	if (initString == NULL)
	{
		printf("ERROR: null initString!\n");
		return NULL;
	}

	/* if the strlen(initString) is zero, it's phase one, return default device name prefix */
	if (initString[0] == EOS)
	{
	  bcopy ((char *)INF_MAIN_DEV_NAME, initString, sizeof(INF_MAIN_DEV_NAME));
		/*printf("First phase! the return iniString=%s!\n", initString); */
		return NULL;
	}

	/* strlen(initString) is not zero, it's phase two, parsing the string and do initiailization. */
	memset(&initToken, 0, sizeof(RTMP_VX_INIT_STRING));
	status = RtmpENDDevInitStrParse(initString, &initToken);
	if (status == NDIS_STATUS_FAILURE)
	{
		printf("initString format wrong!str=%s!\n", initString);
		return NULL;
	}

	switch(initToken.infType)
	{
#ifdef RTMP_PCI_SUPPORT
		case RTMP_DEV_INF_PCI:
				pENDDev = RtmpVxPciEndLoad(&initToken);
				break;
#endif /* RTMP_PCI_SUPPORT */
#ifdef RTMP_USB_SUPPORT
		case RTMP_DEV_INF_USB:
				pENDDev = RtmpVxUsbEndLoad(&initToken);
				break;
#endif /* RTMP_USB_SUPPORT */
#ifdef RTMP_RBUS_SUPPORT
		case RTMP_DEV_INF_RBUS:
				pENDDev = RtmpVxRbusEndLoad(&initToken);
				break;
#endif /* RTMP_RBUS_SUPPORT */
		default:
				printf("Un-supported interface type:%d!\n", initToken.infType);
				break;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<=== sysRtmpEndLoad(), pENDDev=0x%x!\n", pENDDev));
	
	return pENDDev;
	
}


STATUS sysRtmpEndUnLoad(END_OBJ *pEndDev)
{	
	RTMP_ADAPTER *pAd;
	STATUS retval;
	
	/* Unregister network device */
	if (pEndDev == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("sysRtmpEndUnLoad(): pEndDev is NULL!\n"));
		return ERROR;
	}

	END_OBJECT_UNLOAD(pEndDev);
	DBGPRINT(RT_DEBUG_TRACE, ("sysRtmpEndUnLoad(): (), dev->name=%s!\n", pEndDev->devObject.name));
	pAd = RTMP_OS_NETDEV_GET_PRIV(pEndDev);
	
	if (pAd != NULL)
	{	
		retval = EALREADY;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("sysRtmpEndUnLoad(): (), pAd is NULL!\n"));
		retval = ERROR;
	}
  	
	return retval;

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
     if ((wrq->u.data.flags== RT_OID_802_11_HARDWARE_REGISTER) && (wrq->u.data.length >= strlen(msg))) {
		wrq->u.data.length = strlen(msg);
		copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);
	} else {
		DBGPRINT(RT_DEBUG_OFF, ("%s\n", msg));	
	}
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
