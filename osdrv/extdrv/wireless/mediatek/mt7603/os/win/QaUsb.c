
#include "config.h"
//#include "UsbBulk.h"

/////////////////////////////////////////////////////////////////////////////
//	CallUSBDI:	Send off URB etc and wait for IOCTL to complete
//				Build Internal IOCTL IRP to send to USBDI
//				Call USBDI and wait for IRP to complete
//				Must be called at PASSIVE_LEVEL
 #pragma warning(disable : 4995)
 #pragma warning(disable : 4996)
NTSTATUS CallUSBDI( IN RTMP_ADAPTER *dx, IN PVOID UrbEtc,
					IN ULONG IoControlCode,
				    IN ULONG Arg2)
{
	NTSTATUS 			ntStatus = 0;
	PMUSBD_WORK_ITEM	workitem;
	LARGE_INTEGER       Timeout;

	DBGPRINT(RT_DEBUG_TRACE,("CallUSBDI()>>>\n"));

	ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

	if(KeGetCurrentIrql() < DISPATCH_LEVEL)
	{
		IO_STATUS_BLOCK IoStatus;
		KEVENT event;
		PIRP Irp;
		PIO_STACK_LOCATION NextIrpStack;
		
		DBGPRINT(RT_DEBUG_TRACE,("KeGetCurrentIrql() < DISPATCH_LEVEL (no QueueWorkItem)\n"));
			
		// Initialise IRP completion event
		KeInitializeEvent(&event, NotificationEvent, FALSE);
	
		// Build Internal IOCTL IRP
		Irp = IoBuildDeviceIoControlRequest(
						IoControlCode, dx->TopOfStackDeviceObject,
						NULL, 0,	// Input buffer
						NULL, 0,	// Output buffer
						TRUE, &event, &IoStatus);
	
		// Get IRP stack location for next driver down (already set up)
		NextIrpStack = IoGetNextIrpStackLocation(Irp);
		// Store pointer to the URB etc
		NextIrpStack->Parameters.Others.Argument1 = UrbEtc;
		NextIrpStack->Parameters.Others.Argument2 = (PVOID)((ULONGLONG)Arg2);
	
		// Call the driver and wait for completion if necessary
		ntStatus = IoCallDriver( dx->TopOfStackDeviceObject, Irp);

		if (ntStatus == STATUS_PENDING)
		{
			// If the request is pending, wait until it completes
			DBGPRINT(RT_DEBUG_TRACE,("CallUSBDI: waiting for URB completion\n"));

			PPT_SET_RELATIVE_TIMEOUT_IN_MILLISECONDS(Timeout, USB_IO_WAIT);
			ntStatus = KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, &Timeout);
			if (ntStatus == STATUS_TIMEOUT)
			{
				//
				// To prevent pending IRP still in the bus driver, we need to cancel it.
				//
				IoCancelIrp(Irp);
				KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
				DBGPRINT(RT_DEBUG_TRACE,("OSWaitEvent STATUS_TIMEOUT\n"));
				return(STATUS_TIMEOUT);
			}
			//IoWaitStatus = NdisWaitEvent((PNDIS_EVENT)&IoEvent,NDIS_IO_WAIT);
			//if (!IoWaitStatus)
			//{
			//	DBGPRINT_RAW(RT_DEBUG_ERROR,("OSWaitEvent STATUS_TIMEOUT\n"));
			//	Status = STATUS_TIMEOUT;
			//	return(Status);
			//}
		}
		else
		{
			IoStatus.Status = ntStatus;
		}

	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("KeGetCurrentIrql() >= DISPATCH_LEVEL (QueueWorkItem)\n"));
		
		ntStatus = STATUS_PENDING;

		workitem = (PMUSBD_WORK_ITEM)ExAllocatePool(NonPagedPool, sizeof(MUSBD_WORK_ITEM));

		if(workitem)
		{
			//::TODO //ExInitializeWorkItem show warning, use IoAllocateWorkItem in the future
			// use  #pragma warning(disable : 4995) noe.
			ExInitializeWorkItem(&workitem->WorkItem, WorkItemProcess_MUsb, workitem);

			workitem->DeviceExtension = dx;
			workitem->UrbEtc = UrbEtc;
			workitem->IoControlCode = IoControlCode;
			workitem->Arg2 = Arg2;

			//::TODO #param warning(disable:4996)
			ExQueueWorkItem(&workitem->WorkItem, DelayedWorkQueue);
		}
		else
		{
			ntStatus = STATUS_INSUFFICIENT_RESOURCES;
		}
	}

	return ntStatus;
}

VOID WorkItemProcess_MUsb(PVOID Context)
{
	NTSTATUS ntStatus;
	PMUSBD_WORK_ITEM workItem = (PMUSBD_WORK_ITEM)Context;

	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)workItem->DeviceExtension;
	PVOID UrbEtc = workItem->UrbEtc;
	ULONG IoControlCode = workItem->IoControlCode;
	ULONG Arg2 = workItem->Arg2;
	
	IO_STATUS_BLOCK IoStatus;
	KEVENT event;
	PIRP Irp;
	PIO_STACK_LOCATION NextIrpStack;
	
	DBGPRINT(RT_DEBUG_TRACE,("WorkItemProcess_MUsb()>>>\n"));
	
		// Initialise IRP completion event
		KeInitializeEvent(&event, NotificationEvent, FALSE);
	
		// Build Internal IOCTL IRP
		Irp = IoBuildDeviceIoControlRequest(
						IoControlCode, dx->TopOfStackDeviceObject,
						NULL, 0,	// Input buffer
						NULL, 0,	// Output buffer
						TRUE, &event, &IoStatus);
	
		// Get IRP stack location for next driver down (already set up)
		NextIrpStack = IoGetNextIrpStackLocation(Irp);
		// Store pointer to the URB etc
		NextIrpStack->Parameters.Others.Argument1 = UrbEtc;
		NextIrpStack->Parameters.Others.Argument2 = (PVOID)((ULONGLONG)Arg2);
	
		// Call the driver and wait for completion if necessary
		ntStatus = IoCallDriver( dx->TopOfStackDeviceObject, Irp);
		if (ntStatus == STATUS_PENDING)
		{
	//		DBGPRINT(RT_DEBUG_TRACE,("CallUSBDI: waiting for URB completion\n");
			ntStatus = KeWaitForSingleObject( &event, Suspended, KernelMode, FALSE, NULL);
			ntStatus = IoStatus.Status;
		}	


	ExFreePool(workItem);
		
}


