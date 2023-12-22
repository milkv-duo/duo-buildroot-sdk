
#ifndef	__QAUSB_H__
#define	__QAUSB_H__


typedef struct _MUSBD_WORK_ITEM
{
	PVOID DeviceExtension;
	WORK_QUEUE_ITEM WorkItem;
	PVOID UrbEtc;
	ULONG IoControlCode;
	ULONG Arg2;
}MUSBD_WORK_ITEM, *PMUSBD_WORK_ITEM;

#define PPT_SET_RELATIVE_TIMEOUT_IN_MILLISECONDS(VARIABLE, VALUE) (VARIABLE).QuadPart = -( (LONGLONG) (VALUE)*10*1000 )



#define USB_IO_WAIT 1000


/////////////////////////////////////////////////////////////////////////////
//	Our device extension
VOID WorkItemProcess_MUsb(PVOID Context);

NTSTATUS CallUSBDI( IN RTMP_ADAPTER *dx, IN PVOID UrbEtc,
				    IN ULONG IoControlCode,
				    IN ULONG Arg2);

#endif
