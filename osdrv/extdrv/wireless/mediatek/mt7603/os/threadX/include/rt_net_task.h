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
    rt_net_task.h

    Abstract:
   	NET Task handling for threadX

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/
#ifndef __RT_NET_TASK_H__
#define __RT_NET_TASK_H__

/* Public Definition */
typedef struct _RTMP_NET_TASK_QUEUE
{
	CHAR NetTaskQueueName[30];
	TX_QUEUE NetTaskQueue;
	ULONG NetTaskQueueData[60];
	TX_THREAD NetTaskServiceThread;
	UINT32 NetTaskServiceThreadStack[0x8000];
}RTMP_NET_TASK_QUEUE, *PRTMP_NET_TASK_QUEUE;

typedef void (*PNETFUNC)(ULONG arg1);

typedef enum _RTMP_NET_TASK_STATUS_
{
	RTMP_NET_TASK_UNKNOWN = 0,
	RTMP_NET_TASK_INITED = 1,
	RTMP_NET_TASK_RUNNING = 2,
	RTMP_NET_TASK_PENDING = 4,
	RTMP_NET_TASK_STOP = 8,
}RTMP_NET_TASK_STATUS;

typedef struct _RTMP_NET_TASK_STRUCT_
{
	PNETFUNC				funcPtr;
	ULONG					data;
	RTMP_NET_TASK_STATUS	taskStatus;
	char					taskName[8];
}RTMP_NET_TASK_STRUCT, *PRTMP_NET_TASK_STRUCT;

#define RTMP_NET_TASK_INIT(_pNetTask, _pFuncPtr, _pData)	\
	do{														\
		(_pNetTask)->funcPtr = (_pFuncPtr);					\
		(_pNetTask)->data = (unsigned int)(_pData);			\
		(_pNetTask)->taskStatus = RTMP_NET_TASK_INITED;		\
	}while(0)

/* Public API */
extern int RTMPCreateNetTaskQueue(const char *NetTaskQueName, UINT Priority, 
					   			  ULONG TimeSlice, PRTMP_NET_TASK_QUEUE pNetTaskQueue);
extern void RTMPDestroyNetTaskQueue(PRTMP_NET_TASK_QUEUE pNetTaskQueue);
extern int RTMPQueueNetTask(PRTMP_NET_TASK_QUEUE pNetTaskQueue, PRTMP_NET_TASK_STRUCT pNetTask);
extern int RTMPCancelDelayedNetTask(PRTMP_NET_TASK_STRUCT pNetTask);

#endif /* __RT_NET_TASK_H__ */
