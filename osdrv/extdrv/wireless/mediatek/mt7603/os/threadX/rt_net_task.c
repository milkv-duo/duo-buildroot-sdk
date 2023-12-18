/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2010, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rt_net_task.c

	Abstract:

	Revision History:
	Who			When		What
	--------	----------	----------------------------------------------
	Name		Date		Modification logs
	
*/

#include "rt_config.h"

/*
 * NetTaskExec - net task execution entry
 *
 * @ULONG entryInput
 * return value : NONE
 */
void NetTaskExec(IN ULONG entryInput)
{
	DC_RETURN_CODE ret;
	ULONG queueMsg;
	PRTMP_NET_TASK_STRUCT NetTask;
	PRTMP_NET_TASK_QUEUE pNetTaskQueue;
	RTMP_ADAPTER *pAd;
	int count =0;
	pNetTaskQueue = (PRTMP_NET_TASK_QUEUE)entryInput;
	pAd = (PRTMP_ADAPTER)RT_WIFI_DevCtrl;

	/* receive messge from netJobQueue */
	while(1)
	{

		ret = tx_queue_receive(&pNetTaskQueue->NetTaskQueue,		/* The queue to wait on */
							   &queueMsg,							/* Where to put the received message */
							   TX_WAIT_FOREVER);

		if(ret != TX_SUCCESS)
		{
			/* There was some issue in receiving the message from the queue. */
            /* Simply skip any message processing and wait for the next */
            /* message to come in. */
            continue;
        }

		/* Pull net task address out of the received message */
		NetTask = (PRTMP_NET_TASK_STRUCT)(queueMsg);

		/*DBGPRINT(RT_DEBUG_TRACE, ("before NetTask(0x%x) Que=0x%x\n", NetTask, queueMsg)); */
		/* Execute netTask Job */
		NetTask->funcPtr(NetTask->data);

	}
}


int RTMPCreateNetTaskQueue(IN const char *NetTaskQueName, IN UINT Priority, 
					   IN ULONG TimeSlice, OUT PRTMP_NET_TASK_QUEUE pNetTaskQueue)
{
	int ret;

	/* Copy NetTaskQueName into pNetTaskQueue structure */ 
	sprintf(pNetTaskQueue->NetTaskQueueName, "%sQueue", NetTaskQueName);
	
	/* Create the NetTask queue for receiving "NET Task" to execute */
	ret = tx_queue_create(&pNetTaskQueue->NetTaskQueue,				/* The message queue object */
						  NetTaskQueName,							/* The message queue name */
						  TX_1_ULONG,								/* Each message is 1 ULONG in length */
						  &pNetTaskQueue->NetTaskQueueData[0],		/* The message queue memory */
						  sizeof(pNetTaskQueue->NetTaskQueueData));	/* The sizes of the memory queue memory */

	if (ret != TX_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("tx_queue_create() : create %s fialed [%x]", NetTaskQueName, ret));
		goto ERROR_FAIL;
	}

	/* Create NetTask Serivce Thread and entry function is NetTaskExec */
	ret = tx_thread_create(&pNetTaskQueue->NetTaskServiceThread,
						   NetTaskQueName,
						   NetTaskExec,
						   pNetTaskQueue,  /* entryInput */
						   pNetTaskQueue->NetTaskServiceThreadStack,
						   sizeof(pNetTaskQueue->NetTaskServiceThreadStack),
						   Priority,
						   Priority,
						   TimeSlice,
						   TX_AUTO_START);

	if(ret != TX_SUCCESS)
	{
		dc_log_printf("tx_thread_create() : create NetTask Serivce Thread failed [%x]", ret);
		goto ERROR_FREE_NET_TASK_QUEUE;
	}

	return NDIS_STATUS_SUCCESS;

ERROR_FREE_NET_TASK_QUEUE:
	tx_queue_delete(&pNetTaskQueue->NetTaskQueue);

ERROR_FAIL:
	return NDIS_STATUS_FAILURE;
}


void RTMPDestroyNetTaskQueue(IN PRTMP_NET_TASK_QUEUE pNetTaskQueue)
{
	int ret;

	/* Delete the NetTask queue for receiving "NET Task" to execute */
	ret = tx_queue_delete(&pNetTaskQueue->NetTaskQueue);

	if (ret != TX_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("tx_queue_delete(): delete %s failed[%d]\n", 
												pNetTaskQueue->NetTaskQueueName, ret)); 
		return NDIS_STATUS_FAILURE; 
	}

	/* Delete NetTask Serivce Thread */
	ret = tx_thread_delete(&pNetTaskQueue->NetTaskServiceThread);

	if (ret != TX_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("tx_thread_delete(): delete %s service thread failed[%d]\n",
												pNetTaskQueue->NetTaskQueueName, ret));
		return NDIS_STATUS_FAILURE;
	}

	return NDIS_STATUS_FAILURE;
}


int RTMPQueueNetTask(IN PRTMP_NET_TASK_QUEUE pNetTaskQueue, IN PRTMP_NET_TASK_STRUCT pNetTask)
{
	ULONG queueMsg;

	INT32 ret;
	
	/* Create a message to store address of netTask address */
	queueMsg = (UINT32)pNetTask;

	/* Send message to netJobQueue */
	ret = tx_queue_send(&pNetTaskQueue->NetTaskQueue,
						&queueMsg,
						TX_NO_WAIT);

	if (ret != TX_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("RTMPQueueNetTask fail(%d), QueMsg = %d\n", ret, queueMsg));
		return NDIS_STATUS_FAILURE;
	}	

	return NDIS_STATUS_SUCCESS;
}


/* TODO: */
int RTMPCancelDelayedNetTask(IN PRTMP_NET_TASK_STRUCT pNetTask)
{

}
