/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    rt_pci_rbus.c

    Abstract:
    Create and register network interface.

    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
*/

#include <rt_config.h>


static void rx_done_tasklet(unsigned long data);
#ifdef CONFIG_ANDES_SUPPORT
static void rx1_done_tasklet(unsigned long data);
#endif /* CONFIG_ANDES_SUPPORT */
static void mgmt_dma_done_tasklet(unsigned long data);
static void ac0_dma_done_tasklet(unsigned long data);
static void ac1_dma_done_tasklet(unsigned long data);
static void ac2_dma_done_tasklet(unsigned long data);
static void ac3_dma_done_tasklet(unsigned long data);
static void hcca_dma_done_tasklet(unsigned long data);
static void fifo_statistic_full_tasklet(unsigned long data);

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_SUPPORT
static void uapsd_eosp_sent_tasklet(unsigned long data);
#endif /* UAPSD_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


VOID Invalid_Remaining_Packet(
	IN	PRTMP_ADAPTER pAd,
	IN	 ULONG VirtualAddress)
{
}

NDIS_STATUS RtmpMgmtTaskInit(IN RTMP_ADAPTER *pAd)
{
	RTMP_OS_TASK *pTask;
	NDIS_STATUS status;
	
	/*
		Creat TimerQ Thread
	*/
	/* We need init timerQ related structure before create the timer thread. */
	RtmpTimerQInit(pAd);

	pTask = &pAd->timerTask;
	
	RTMP_OS_TASK_INIT(&pAd->timerTask, "RtmpTimerTask", pAd);
	status = RtmpOSTaskAttach(pTask, RtmpTimerQThread, (ULONG)(&pAd->timerTask));
	if (status == NDIS_STATUS_FAILURE) 
	{
		printf("%s: unable to start RtmpTimerQThread\n",RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev));
	}

#ifdef WSC_INCLUDED
	/* start the crediential write task first. */
	WscThreadInit(pAd);
#endif /* WSC_INCLUDED */

	return status;
	
}


/*
========================================================================
Routine Description:
    Close kernel threads.

Arguments:
	*pAd				the raxx interface data pointer

Return Value:
    NONE

Note:
========================================================================
*/
VOID RtmpMgmtTaskExit(
	IN RTMP_ADAPTER *pAd)
{
	RTMP_OS_TASK	*pTask;
	NDIS_STATUS		ret;
	
	
	/* We need clear timerQ related structure before exits of the timer thread. */
	RtmpTimerQExit(pAd);

#ifdef WSC_INCLUDED
	WscThreadExit(pAd);
#endif /* WSC_INCLUDED */

	/* Terminate Threads */
	pTask = &pAd->timerTask;
	ret = RtmpOSTaskKill(pTask);
	if (ret == NDIS_STATUS_FAILURE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: kill task(%s) failed!\n", 
					RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev), pTask->taskName));
	}

}


NDIS_STATUS RtmpNetTaskInit(IN RTMP_ADAPTER *pAd)
{
	POS_COOKIE pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	DBGPRINT(RT_DEBUG_ERROR, ("===>RtmpNetTaskInit\n")); 

	RTMP_NET_TASK_INIT(&pObj->rx_done_task, rx_done_tasklet, (unsigned long)pAd);
#ifdef CONFIG_ANDES_SUPPORT
	RTMP_NET_TASK_INIT(&pObj->rx1_done_task, rx1_done_tasklet, (unsigned long)pAd);
#endif /* CONFIG_ANDES_SUPPORT */
	RTMP_NET_TASK_INIT(&pObj->mgmt_dma_done_task, mgmt_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac0_dma_done_task, ac0_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac1_dma_done_task, ac1_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac2_dma_done_task, ac2_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac3_dma_done_task, ac3_dma_done_tasklet, (unsigned long)pAd);
		
	RTMP_NET_TASK_INIT(&pObj->hcca_dma_done_task, hcca_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->tbtt_task, tbtt_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->fifo_statistic_full_task, fifo_statistic_full_tasklet, (unsigned long)pAd);

	strcpy(&pObj->rx_done_task.taskName[0], "rx");
#ifdef CONFIG_ANDES_SUPPORT
		strcpy(&pObj->rx1_done_task.taskName[0], "rx1");
#endif /* CONFIG_ANDES_SUPPORT */
	strcpy(&pObj->mgmt_dma_done_task.taskName[0], "mgmt");
	strcpy(&pObj->ac0_dma_done_task.taskName[0], "ac0");
	strcpy(&pObj->ac1_dma_done_task.taskName[0], "ac1");
	strcpy(&pObj->ac2_dma_done_task.taskName[0], "ac2");
	strcpy(&pObj->ac3_dma_done_task.taskName[0], "ac3");
	strcpy(&pObj->hcca_dma_done_task.taskName[0], "hcca");
	strcpy(&pObj->tbtt_task.taskName[0], "tbtt");
	strcpy(&pObj->fifo_statistic_full_task.taskName[0], "fifo");
	
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef UAPSD_SUPPORT	
		RTMP_NET_TASK_INIT(&pObj->uapsd_eosp_sent_task, uapsd_eosp_sent_tasklet, (unsigned long)pAd);
		strcpy(&pObj->uapsd_eosp_sent_task.taskName[0], "uapsd");
#endif /* UAPSD_SUPPORT */

#ifdef DFS_SUPPORT
		RTMP_NET_TASK_INIT(&pObj->pulse_radar_detect_task, pulse_radar_detect_tasklet, (unsigned long)pAd);
		RTMP_NET_TASK_INIT(&pObj->width_radar_detect_task, width_radar_detect_tasklet, (unsigned long)pAd);
#endif /* DFS_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT
		RTMP_NET_TASK_INIT(&pObj->carrier_sense_task, carrier_sense_tasklet, (unsigned long)pAd);
#endif /* CARRIER_DETECTION_SUPPORT */

#ifdef NEW_DFS
		RTMP_NET_TASK_INIT(&pObj->dfs_task, dfs_tasklet, (unsigned long)pAd);
#endif /* NEW_DFS */
	}
#endif /* CONFIG_AP_SUPPORT */

	DBGPRINT(RT_DEBUG_ERROR, ("<===RtmpNetTaskInit\n")); 

	return NDIS_STATUS_SUCCESS;
}


void RtmpNetTaskExit(IN RTMP_ADAPTER *pAd)
{
	/* TODO: Shiang, need to fix that */
}


static inline void rt2860_int_enable(PRTMP_ADAPTER pAd, unsigned int mode)
{
	UINT32 regValue;

	pAd->int_disable_mask &= ~(mode);
	regValue = pAd->int_enable_reg & ~(pAd->int_disable_mask);		
	RTMP_IO_WRITE32(pAd, INT_MASK_CSR, regValue);     /* 1:enable */
	if (regValue != 0)
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);
}


static inline void rt2860_int_disable(PRTMP_ADAPTER pAd, unsigned int mode)
{
	UINT32 regValue;

	pAd->int_disable_mask |= mode;
	regValue = pAd->int_enable_reg & ~(pAd->int_disable_mask);
	RTMP_IO_WRITE32(pAd, INT_MASK_CSR, regValue);     /* 0: disable */
	if (regValue == 0)
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);		
	}
}


/***************************************************************************
  *
  *	tasklet related procedures.
  *
  **************************************************************************/
static void mgmt_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;
	
	pObj = (POS_COOKIE) pAd->OS_Cookie;

	pAd->int_pending &= ~RTMP_INT_MGMT_DLY;
	RTMPHandleMgmtRingDmaDoneInterrupt(pAd);

	/* if you use RTMP_SEM_LOCK, sometimes kernel will hang up, no any */
	/* bug report output */
	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if (pAd->int_pending & RTMP_INT_MGMT_DLY) 
	{
		tasklet_hi_schedule(&pObj->mgmt_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_MGMT_DLY);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);    
}


static void rx_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	BOOLEAN	bReschedule = 0;
	POS_COOKIE pObj;
	UINT32 INT_RX = 0;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		INT_RX = RLT_INT_RX_DATA;
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		INT_RX = RTMP_INT_RX;
#endif /* RTMP_MAC */
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		RTMP_INT_LOCK(&pAd->irq_lock, flags);
		pAd->int_disable_mask &= ~(INT_RX); 
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}
#ifdef UAPSD_SUPPORT
	UAPSD_TIMING_RECORD(pAd, UAPSD_TIMING_RECORD_TASKLET);
#endif /* UAPSD_SUPPORT */

	pObj = (POS_COOKIE) pAd->OS_Cookie;	
	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	pAd->int_pending &= ~(INT_RX);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);

	bReschedule = rtmp_rx_done_handle(pAd);

#ifdef UAPSD_SUPPORT
		UAPSD_TIMING_RECORD_STOP();
#endif /* UAPSD_SUPPORT */

	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/*
	 * double check to avoid rotting packet 
	 */
	if (pAd->int_pending & INT_RX || bReschedule) 
	{
		tasklet_hi_schedule(&pObj->rx_done_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, INT_RX);    
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
}

#ifdef CONFIG_ANDES_SUPPORT
static void rx1_done_tasklet(unsigned long data)
{
	unsigned long flags;
	BOOLEAN	bReschedule = 0;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;

	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		pAd->int_disable_mask &= ~(RLT_INT_RX_CMD); 
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
		return;
	}
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	pAd->int_pending &= ~(RLT_INT_RX_CMD);
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);

	bReschedule = RxRing1DoneInterruptHandle(pAd);

	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	/* double check to avoid rotting packet  */
	if ((pAd->int_pending & RLT_INT_RX_CMD) || bReschedule) 
	{
		RTMP_OS_TASKLET_SCHE(&pObj->rx1_done_task);
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);    
		return;
	}

	/* enable Rx1INT again */
	rt2860_int_enable(pAd, RLT_INT_RX_CMD);
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
}
#endif /* CONFIG_ANDES_SUPPORT */


void fifo_statistic_full_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	UINT32 FifoStaFullInt = 0;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		FifoStaFullInt = RLT_FifoStaFullInt;
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		FifoStaFullInt = RTMP_FifoStaFullInt;
#endif /* RTMP_MAC */
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
	 {
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		pAd->int_disable_mask &= ~(FifoStaFullInt); 
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
		return;
	  }
		
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	pAd->int_pending &= ~(FifoStaFullInt); 
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
		
	NICUpdateFifoStaCounters(pAd);
		
	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);	
	/*
	 * double check to avoid rotting packet 
	 */
	if (pAd->int_pending & FifoStaFullInt) 
	{
		RTMP_OS_TASKLET_SCHE(&pObj->fifo_statistic_full_task);
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);	
		return;
	}
	
	rt2860_int_enable(pAd, FifoStaFullInt);
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
}


static void hcca_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	BOOLEAN bReschedule = 0;	
	UINT32 INT_HCCA_DLY = 0;
	
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		INT_HCCA_DLY = RLT_INT_HCCA_DLY;
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		INT_HCCA_DLY = RTMP_INT_HCCA_DLY;
#endif /* RTMP_MAC */
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		pAd->int_disable_mask &= ~INT_HCCA_DLY; 
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
		return;
	}
		
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	pAd->int_pending &= ~INT_HCCA_DLY;
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
		RTMPHandleTxRing8DmaDoneInterrupt(pAd);
	}
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		bReschedule = RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_HCCA_DONE);
#endif /* RTMP_MAC */		
	
	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	/* double check to avoid lose of interrupts */
	if ((pAd->int_pending & INT_HCCA_DLY) || bReschedule)
	{
		RTMP_OS_TASKLET_SCHE(&pObj->hcca_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);	
		return;
	}
	
	rt2860_int_enable(pAd, INT_HCCA_DLY);
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);	
}


static void ac3_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	BOOLEAN bReschedule = 0;
	UINT32 INT_AC3_DLY = 0;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		INT_AC3_DLY = RLT_INT_AC3_DLY;
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		INT_AC3_DLY = RTMP_INT_AC3_DLY;
#endif /* RTMP_MAC */

	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		pAd->int_disable_mask &= ~(INT_AC3_DLY); 
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
		return;
	}

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	pAd->int_pending &= ~INT_AC3_DLY;
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);

	bReschedule = RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC3_DONE);

	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	/* double check to avoid lose of interrupts */
	if ((pAd->int_pending & INT_AC3_DLY) || bReschedule)
	{
		RTMP_OS_TASKLET_SCHE(&pObj->ac3_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);	
		return;
	}

	rt2860_int_enable(pAd, INT_AC3_DLY);
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);  
}


static void ac2_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	BOOLEAN bReschedule = 0;
	UINT32 INT_AC2_DLY = 0;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		INT_AC2_DLY = RLT_INT_AC2_DLY;
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		INT_AC2_DLY = RTMP_INT_AC2_DLY;
#endif /* RTMP_MAC */
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		pAd->int_disable_mask &= ~(INT_AC2_DLY); 
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
		return;
	}

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	pAd->int_pending &= ~INT_AC2_DLY;
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);

	bReschedule = RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC2_DONE);

	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	/* double check to avoid lose of interrupts */
	if ((pAd->int_pending & INT_AC2_DLY) || bReschedule) 
	{
		RTMP_OS_TASKLET_SCHE(&pObj->ac2_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);	
		return;
	}

	rt2860_int_enable(pAd, INT_AC2_DLY);
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
}


static void ac1_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	BOOLEAN bReschedule = 0;
	UINT32 INT_AC1_DLY = 0;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		INT_AC1_DLY = RLT_INT_AC1_DLY;
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		INT_AC1_DLY = RTMP_INT_AC1_DLY;
#endif /* RTMP_MAC */
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		pAd->int_disable_mask &= ~(INT_AC1_DLY); 
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
		return;
	}

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	pAd->int_pending &= ~INT_AC1_DLY;
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);

	bReschedule = RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC1_DONE);

	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	/* double check to avoid lose of interrupts */
	if ((pAd->int_pending & INT_AC1_DLY) || bReschedule) 
	{
		RTMP_OS_TASKLET_SCHE(&pObj->ac1_dma_done_task);

		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);    
		return;
	}

	rt2860_int_enable(pAd, INT_AC1_DLY);
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);    
}


static void ac0_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	BOOLEAN bReschedule = 0;
	UINT32 INT_AC0_DLY = 0;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		INT_AC0_DLY = RLT_INT_AC0_DLY;
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		INT_AC0_DLY = RTMP_INT_AC0_DLY;
#endif /* RTMP_MAC */

	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		pAd->int_disable_mask &= ~(INT_AC0_DLY); 
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
		return;
	}
	
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
	pAd->int_pending &= ~INT_AC0_DLY;
	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	bReschedule = RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC0_DONE);
	
	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if ((pAd->int_pending & INT_AC0_DLY) || bReschedule)
	{
		RTMP_OS_TASKLET_SCHE(&pObj->ac0_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);    
		return;
	}

	rt2860_int_enable(pAd, INT_AC0_DLY);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);    
}


#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_SUPPORT
/*
========================================================================
Routine Description:
    Used to send the EOSP frame.

Arguments:
    data			Pointer to our adapter

Return Value:
    None

Note:
========================================================================
*/
static void uapsd_eosp_sent_tasklet(unsigned long data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, WCID_ALL, MAX_TX_PROCESS);
}
#endif /* UAPSD_SUPPORT */

#ifdef DFS_HARDWARE_SUPPORT
void schedule_dfs_task(PRTMP_ADAPTER pAd)
{
	POS_COOKIE pObj;
	
    pObj = (POS_COOKIE) pAd->OS_Cookie;
	tasklet_hi_schedule(&pObj->dfs_task);
}

static void dfs_tasklet(unsigned long data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	
	if (pAd->CommonCfg.DFSAPRestart == 1)
	{
		int i, j;

		pAd->CommonCfg.dfs_w_counter += 10;
		/* reset period table */
		for (i = 0; i < NEW_DFS_MAX_CHANNEL; i++)
		{
			for (j = 0; j < NEW_DFS_MPERIOD_ENT_NUM; j++)
			{
				pAd->CommonCfg.DFS_T[i][j].period = 0;
				pAd->CommonCfg.DFS_T[i][j].idx = 0;
				pAd->CommonCfg.DFS_T[i][j].idx2 = 0;
			}
		}

		APStop(pAd);
		APStartUp(pAd);
		pAd->CommonCfg.DFSAPRestart = 0;
	}
	else
	/* check radar here */
	{
		int idx;
		
		if (pAd->CommonCfg.radarDeclared == 0)
		{
			for (idx = 0; idx < 3; idx++)
			{
				if (SWRadarCheck(pAd, idx) == 1)
				{
					pAd->CommonCfg.radarDeclared = 1;
					break;
				}
			}
		}
	}
	
	
}
#endif /* NEW_DFS */
#endif /* CONFIG_AP_SUPPORT */


/***************************************************************************
  *
  *	interrupt handler related procedures.
  *
  **************************************************************************/
VOID rt2860_interrupt(void *dev_instance)
{
	PNET_DEV           pNetDev;
	RTMP_ADAPTER *pAd;
	UINT32 IntSource;
	POS_COOKIE pObj;
	unsigned long flags=0;
	UINT32 INT_RX_DATA = 0, INT_RX_CMD=0, TxCoherent = 0, RxCoherent = 0, FifoStaFullInt = 0;
	UINT32 INT_MGMT_DLY = 0, INT_HCCA_DLY = 0, INT_AC3_DLY = 0, INT_AC2_DLY = 0, INT_AC1_DLY = 0, INT_AC0_DLY = 0;
	UINT32 PreTBTTInt = 0, TBTTInt = 0, GPTimeOutInt = 0, RadarInt = 0, AutoWakeupInt = 0;

	/*
	 * Note 03312008: we can not return here before
	 * RTMP_IO_READ32(pAd, INT_SOURCE_CSR, &IntSource.word);
	 * RTMP_IO_WRITE32(pAd, INT_SOURCE_CSR, IntSource.word);
	 * Or kernel will panic after ifconfig ra0 down sometimes
	 */

	pNetDev = (PNET_DEV) dev_instance;
	pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(pNetDev);
	pObj = (POS_COOKIE) pAd->OS_Cookie;

	/* Inital the Interrupt source. */
	IntSource = 0x00000000L;

	RTMP_IO_READ32(pAd, INT_SOURCE_CSR, &IntSource);
	RTMP_IO_WRITE32(pAd, INT_SOURCE_CSR, IntSource); /* write 1 to clear */

	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP))
		return;

	if (RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS |fRTMP_ADAPTER_HALT_IN_PROGRESS)))
		return;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
			INT_RX_DATA = RLT_INT_RX_DATA;
			INT_RX_CMD = RLT_INT_RX_CMD;
			TxCoherent = RLT_TxCoherent;
			RxCoherent = RLT_RxCoherent;
			FifoStaFullInt = RLT_FifoStaFullInt;
			INT_MGMT_DLY = RLT_INT_MGMT_DLY;
			INT_HCCA_DLY = RLT_INT_HCCA_DLY;
			INT_AC3_DLY = RLT_INT_AC3_DLY;
			INT_AC2_DLY = RLT_INT_AC2_DLY;
			INT_AC1_DLY = RLT_INT_AC1_DLY;
			INT_AC0_DLY = RLT_INT_AC0_DLY;
			PreTBTTInt = RLT_PreTBTTInt;
			TBTTInt = RLT_TBTTInt;
			GPTimeOutInt = RLT_GPTimeOutInt;
			RadarInt = RLT_RadarInt;
			AutoWakeupInt = RLT_AutoWakeupInt;
			//McuCommand = RLT_McuCommand;
		}
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP) {
			INT_RX_DATA = RTMP_INT_RX;
			TxCoherent = RTMP_TxCoherent;
			RxCoherent = RTMP_RxCoherent;
			FifoStaFullInt = RTMP_FifoStaFullInt;
			INT_MGMT_DLY = RTMP_INT_MGMT_DLY;
			INT_HCCA_DLY = RTMP_INT_HCCA_DLY;
			INT_AC3_DLY = RTMP_INT_AC3_DLY;
			INT_AC2_DLY = RTMP_INT_AC2_DLY;
			INT_AC1_DLY = RTMP_INT_AC1_DLY;
			INT_AC0_DLY = RTMP_INT_AC0_DLY;
			PreTBTTInt = RTMP_PreTBTTInt;
			TBTTInt = RTMP_TBTTInt;
			GPTimeOutInt = RTMP_GPTimeOutInt;
			RadarInt = RTMP_RadarInt;
			AutoWakeupInt = RTMP_AutoWakeupInt;
			//McuCommand = RTMP_McuCommand;
		}
#endif /* RTMP_MAC */

	pAd->bPCIclkOff = FALSE;

	/* If required spinlock, each ISR has to acquire and release itself. */

	/* Do nothing if NIC doesn't exist */
	if (IntSource == 0xffffffff)
	{
		RTMP_SET_FLAG(pAd, (fRTMP_ADAPTER_NIC_NOT_EXIST | fRTMP_ADAPTER_HALT_IN_PROGRESS));
		return;
	}

	if (IntSource & TxCoherent)
	{
		DBGPRINT(RT_DEBUG_ERROR, (">>>TxCoherent<<<\n"));
		RTMPHandleRxCoherentInterrupt(pAd);
	}

	if (IntSource & RxCoherent)
	{
		DBGPRINT(RT_DEBUG_ERROR, (">>>RxCoherent<<<\n"));
		RTMPHandleRxCoherentInterrupt(pAd);
	}

	if (IntSource & FifoStaFullInt) 
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		if ((pAd->int_disable_mask & FifoStaFullInt) == 0) 
		{
			rt2860_int_disable(pAd, FifoStaFullInt);
			RTMP_OS_TASKLET_SCHE(&pObj->fifo_statistic_full_task);
		}
		pAd->int_pending |= FifoStaFullInt; 
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	}

	if (IntSource & INT_MGMT_DLY) 
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		if ((pAd->int_disable_mask & INT_MGMT_DLY) ==0)
		{
			rt2860_int_disable(pAd, INT_MGMT_DLY);
			RTMP_OS_TASKLET_SCHE(&pObj->mgmt_dma_done_task);			
		}
		pAd->int_pending |= INT_MGMT_DLY ;
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	}

	if (IntSource & INT_RX_DATA)
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		if ((pAd->int_disable_mask & INT_RX_DATA) == 0) 
		{
#ifdef UAPSD_SUPPORT
			UAPSD_TIMING_RECORD_START();
			UAPSD_TIMING_RECORD(pAd, UAPSD_TIMING_RECORD_ISR);
#endif /* UAPSD_SUPPORT */
	
			rt2860_int_disable(pAd, INT_RX_DATA);
			RTMP_OS_TASKLET_SCHE(&pObj->rx_done_task);
		}
		pAd->int_pending |= INT_RX_DATA;
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	}

#ifdef CONFIG_ANDES_SUPPORT
	if (IntSource & INT_RX_CMD)
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		if ((pAd->int_disable_mask & INT_RX_CMD) == 0)
		{
			/* mask INT_R1 */
			rt2860_int_disable(pAd, INT_RX_CMD);
			RTMP_OS_TASKLET_SCHE(&pObj->rx1_done_task);
		}
		pAd->int_pending |= INT_RX_CMD;
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	}
#endif /* CONFIG_ANDES_SUPPORT */

	if (IntSource & INT_HCCA_DLY)
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		if ((pAd->int_disable_mask & INT_HCCA_DLY) == 0) 
		{
			rt2860_int_disable(pAd, INT_HCCA_DLY);
			RTMP_OS_TASKLET_SCHE(&pObj->hcca_dma_done_task);
		}
		pAd->int_pending |= INT_HCCA_DLY;						
	        RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	}

	if (IntSource & INT_AC3_DLY)
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		if ((pAd->int_disable_mask & INT_AC3_DLY) == 0) 
		{
			rt2860_int_disable(pAd, INT_AC3_DLY);
			RTMP_OS_TASKLET_SCHE(&pObj->ac3_dma_done_task);
		}
		pAd->int_pending |= INT_AC3_DLY;						
	       	RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	}

	if (IntSource & INT_AC2_DLY)
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		if ((pAd->int_disable_mask & INT_AC2_DLY) == 0) 
		{
			rt2860_int_disable(pAd, INT_AC2_DLY);
			RTMP_OS_TASKLET_SCHE(&pObj->ac2_dma_done_task);
		}
		pAd->int_pending |= INT_AC2_DLY;
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	}

	if (IntSource & INT_AC1_DLY)
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		pAd->int_pending |= INT_AC1_DLY;

		if ((pAd->int_disable_mask & INT_AC1_DLY) == 0) 
		{
			rt2860_int_disable(pAd, INT_AC1_DLY);
			RTMP_OS_TASKLET_SCHE(&pObj->ac1_dma_done_task);
		}
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	}

	if (IntSource & INT_AC0_DLY)
	{
		RTMP_INT_LOCK(&pAd->LockInterrupt, flags);
		pAd->int_pending |= INT_AC0_DLY;
		if ((pAd->int_disable_mask & INT_AC0_DLY) == 0) 
		{
			rt2860_int_disable(pAd, INT_AC0_DLY);
			RTMP_OS_TASKLET_SCHE(&pObj->ac0_dma_done_task);
		}
		RTMP_INT_UNLOCK(&pAd->LockInterrupt, flags);
	}

	if (IntSource & PreTBTTInt)
		RTMPHandlePreTBTTInterrupt(pAd);
	
	if (IntSource & TBTTInt) {
		RTMPHandleTBTTInterrupt(pAd);
	}
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef DFS_SUPPORT
		if (IntSource & GPTimeOutInt)
			  NewTimerCB_Radar(pAd);
#endif /* DFS_SUPPORT */
	
#ifdef CARRIER_DETECTION_SUPPORT
		if ((IntSource & RadarInt))
		{
			if (pAd->CommonCfg.CarrierDetect.Enable == TRUE)
				RTMPHandleRadarInterrupt(pAd);
		}
#endif /* CARRIER_DETECTION_SUPPORT*/	
	}
#endif /* CONFIG_AP_SUPPORT */

	return;
}

