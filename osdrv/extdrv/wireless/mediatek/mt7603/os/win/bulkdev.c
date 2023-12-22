#include "config.h"

extern UCHAR g_FWSeqMCU;
extern UCHAR g_FWRspStatus;
extern UCHAR g_PacketCMDSeqMCU;
extern UCHAR g_PacketCMDRspData[PKTCMD_EVENT_BUFFER];
VOID
CancelSelectSuspend(
    IN RTMP_ADAPTER *DeviceExtension
    )
/*++
 
Routine Description:

    This routine is invoked to cancel selective suspend request.

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    None.

--*/
{
    PIRP  irp;
    KIRQL oldIrql;

    irp = NULL;

    ////DBGPRINT(RT_DEBUG_TRACE,("CancelSelectSuspend - begins\n"));

    KeAcquireSpinLock(&DeviceExtension->IdleReqStateLock, &oldIrql);

    if(!CanDeviceSuspend(DeviceExtension))
    {
        ////DBGPRINT(RT_DEBUG_TRACE,("Device is not idle\n"));
    
        irp = (PIRP) InterlockedExchangePointer(
                             (PVOID*)&DeviceExtension->PendingIdleIrp, //++(PVOID*) for x64 
                            NULL);
    }

    KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

    //
    // since we have a valid Irp ptr,
    // we can call IoCancelIrp on it,
    // without the fear of the irp 
    // being freed underneath us.
    //
    if(irp) {

        //
        // This routine has the irp pointer.
        // It is safe to call IoCancelIrp because we know that
        // the compleiton routine will not free this irp unless...
        // 
        //        
        if(IoCancelIrp(irp)) {

            ////DBGPRINT(RT_DEBUG_TRACE,("IoCancelIrp returns TRUE\n"));
        }
        else {
            ////DBGPRINT(RT_DEBUG_TRACE,("IoCancelIrp returns FALSE\n"));
        }

        //
        // ....we decrement the FreeIdleIrpCount from 2 to 1.
        // if completion routine runs ahead of us, then this routine 
        // decrements the FreeIdleIrpCount from 1 to 0 and hence shall
        // free the irp.
        //
        if(0 == InterlockedDecrement(&DeviceExtension->FreeIdleIrpCount)) {

           // DBGPRINT(RT_DEBUG_TRACE,("CancelSelectSuspend frees the irp\n"));
            IoFreeIrp(irp);

            KeSetEvent(&DeviceExtension->NoIdleReqPendEvent,
                       IO_NO_INCREMENT,
                       FALSE);
        }
    }

    //DBGPRINT(RT_DEBUG_TRACE,("CancelSelectSuspend - ends\n"));

    return;
}

NTSTATUS
SubmitIdleRequestIrp(
    IN RTMP_ADAPTER *DeviceExtension
    )
/*++
 
Routine Description:

    This routine builds an idle request irp with an associated callback routine
    and a completion routine in the driver and passes the irp down the stack.

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    NT status value

--*/
{
    PIRP                    irp;
    NTSTATUS                ntStatus;
    KIRQL                   oldIrql;
    PUSB_IDLE_CALLBACK_INFO idleCallbackInfo;
    PIO_STACK_LOCATION      nextStack;

    //
    // initialize variables
    //
    
    irp = NULL;
    idleCallbackInfo = NULL;

    //DBGPRINT(RT_DEBUG_TRACE,("SubmitIdleRequest - begins\n"));

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    if(PowerDeviceD0 != DeviceExtension->DevPower) {

        ntStatus = STATUS_POWER_STATE_INVALID;

        goto SubmitIdleRequestIrp_Exit;
    }

    KeAcquireSpinLock(&DeviceExtension->IdleReqStateLock, &oldIrql);

    if(InterlockedExchange(&DeviceExtension->IdleReqPend, 1)) {

        //BulkUsb_DbgPrint(1, ("Idle request pending..\n"));

        KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

        ntStatus = STATUS_DEVICE_BUSY;

        goto SubmitIdleRequestIrp_Exit;
    }

    //
    // clear the NoIdleReqPendEvent because we are about 
    // to submit an idle request. Since we are so early
    // to clear this event, make sure that if we fail this
    // request we set back the event.
    //
    KeClearEvent(&DeviceExtension->NoIdleReqPendEvent);

    idleCallbackInfo = (PUSB_IDLE_CALLBACK_INFO)ExAllocatePool(NonPagedPool, 
                                      sizeof(struct _USB_IDLE_CALLBACK_INFO));

    if(idleCallbackInfo) {

        idleCallbackInfo->IdleCallback = (USB_IDLE_CALLBACK)IdleNotificationCallback;

        idleCallbackInfo->IdleContext = (PVOID)DeviceExtension;

        ASSERT(DeviceExtension->IdleCallbackInfo == NULL);

        DeviceExtension->IdleCallbackInfo = idleCallbackInfo;

        //
        // we use IoAllocateIrp to create an irp to selectively suspend the 
        // device. This irp lies pending with the hub driver. When appropriate
        // the hub driver will invoked callback, where we power down. The completion
        // routine is invoked when we power back.
        //
        irp = IoAllocateIrp(DeviceExtension->TopOfStackDeviceObject->StackSize,
                            FALSE);

        if(irp == NULL) {

            //BulkUsb_DbgPrint(1, ("cannot build idle request irp\n"));

            KeSetEvent(&DeviceExtension->NoIdleReqPendEvent,
                       IO_NO_INCREMENT,
                       FALSE);

            InterlockedExchange(&DeviceExtension->IdleReqPend, 0);

            KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

            ExFreePool(idleCallbackInfo);

            ntStatus = STATUS_INSUFFICIENT_RESOURCES;

            goto SubmitIdleRequestIrp_Exit;
        }

        nextStack = IoGetNextIrpStackLocation(irp);

        nextStack->MajorFunction = 
                    IRP_MJ_INTERNAL_DEVICE_CONTROL;

        nextStack->Parameters.DeviceIoControl.IoControlCode = 
                    IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION;

        nextStack->Parameters.DeviceIoControl.Type3InputBuffer =
                    idleCallbackInfo;

        nextStack->Parameters.DeviceIoControl.InputBufferLength =
                    sizeof(struct _USB_IDLE_CALLBACK_INFO);


        IoSetCompletionRoutine(irp, 
                               IdleNotificationRequestComplete,
                               DeviceExtension, 
                               TRUE, 
                               TRUE, 
                               TRUE);

        DeviceExtension->PendingIdleIrp = irp;

        //
        // we initialize the count to 2.
        // The reason is, if the CancelSelectSuspend routine manages
        // to grab the irp from the device extension, then the last of the
        // CancelSelectSuspend routine/IdleNotificationRequestComplete routine 
        // to execute will free this irp. We need to have this schema so that
        // 1. completion routine does not attempt to touch the irp freed by 
        //    CancelSelectSuspend routine.
        // 2. CancelSelectSuspend routine doesnt wait for ever for the completion
        //    routine to complete!
        //
        DeviceExtension->FreeIdleIrpCount = 2;

        KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

        //
        // check if the device is idle.
        // A check here ensures that a race condition did not 
        // completely reverse the call sequence of SubmitIdleRequestIrp
        // and CancelSelectiveSuspend
        //

        if(!CanDeviceSuspend(DeviceExtension) ||
           PowerDeviceD0 != DeviceExtension->DevPower) {

            //
            // IRPs created using IoBuildDeviceIoControlRequest should be
            // completed by calling IoCompleteRequest and not merely 
            // deallocated.
            //
     
            //BulkUsb_DbgPrint(1, ("Device is not idle\n"));

            KeAcquireSpinLock(&DeviceExtension->IdleReqStateLock, &oldIrql);

            DeviceExtension->IdleCallbackInfo = NULL;

            DeviceExtension->PendingIdleIrp = NULL;

            KeSetEvent(&DeviceExtension->NoIdleReqPendEvent,
                       IO_NO_INCREMENT,
                       FALSE);

            InterlockedExchange(&DeviceExtension->IdleReqPend, 0);

            KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

            if(idleCallbackInfo) {

                ExFreePool(idleCallbackInfo);
            }

            //
            // it is still safe to touch the local variable "irp" here.
            // the irp has not been passed down the stack, the irp has
            // no cancellation routine. The worse position is that the
            // CancelSelectSuspend has run after we released the spin 
            // lock above. It is still essential to free the irp.
            //
            if(irp) {

                IoFreeIrp(irp);
            }

            ntStatus = STATUS_UNSUCCESSFUL;

            goto SubmitIdleRequestIrp_Exit;
        }

        //DBGPRINT(RT_DEBUG_TRACE,("Cancel the timers\n"));
        //
        // Cancel the timer so that the DPCs are no longer fired.
        // Thus, we are making judicious usage of our resources.
        // we do not need DPCs because we already have an idle irp pending.
        // The timers are re-initialized in the completion routine.
        //
        KeCancelTimer(&DeviceExtension->Timer);

        ntStatus = IoCallDriver(DeviceExtension->TopOfStackDeviceObject, irp);

        if(!NT_SUCCESS(ntStatus)) {

            //BulkUsb_DbgPrint(1, ("IoCallDriver failed\n"));

            goto SubmitIdleRequestIrp_Exit;
        }
    }
    else {

        //BulkUsb_DbgPrint(1, ("Memory allocation for idleCallbackInfo failed\n"));

        KeSetEvent(&DeviceExtension->NoIdleReqPendEvent,
                   IO_NO_INCREMENT,
                   FALSE);

        InterlockedExchange(&DeviceExtension->IdleReqPend, 0);

        KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

SubmitIdleRequestIrp_Exit:

    //DBGPRINT(RT_DEBUG_TRACE,("SubmitIdleRequest - ends\n"));

    return ntStatus;
}


VOID
IdleNotificationCallback(
    IN RTMP_ADAPTER *DeviceExtension
    )
/*++
 
Routine Description:

  "A pointer to a callback function in your driver is passed down the stack with
   this IOCTL, and it is this callback function that is called by USBHUB when it
   safe for your device to power down."

  "When the callback in your driver is called, all you really need to do is to
   to first ensure that a WaitWake Irp has been submitted for your device, if 
   remote wake is possible for your device and then request a SetD2 (or DeviceWake)"

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    NT status value

--*/
{
    NTSTATUS                ntStatus;
    POWER_STATE             powerState;
    KEVENT                  irpCompletionEvent;
    PIRP_COMPLETION_CONTEXT irpContext;

    //DBGPRINT(RT_DEBUG_TRACE,("IdleNotificationCallback - begins\n"));

    //
    // Dont idle, if the device was just disconnected or being stopped
    // i.e. return for the following DeviceState(s)
    // NotStarted, Stopped, PendingStop, PendingRemove, SurpriseRemoved, Removed
    //

    if(DeviceExtension->DeviceState != USBWorking) {

        return;
    }

    //
    // If there is not already a WW IRP pending, submit one now
    //
    /*if(DeviceExtension->WaitWakeEnable) {

        IssueWaitWake(DeviceExtension);
    }*/


    //
    // power down the device
    //

    irpContext = (PIRP_COMPLETION_CONTEXT) 
                 ExAllocatePool(NonPagedPool,
                                sizeof(IRP_COMPLETION_CONTEXT));

    if(!irpContext) {

        //BulkUsb_DbgPrint(1, ("Failed to alloc memory for irpContext\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }
    else {

        //
        // increment the count. In the HoldIoRequestWorkerRoutine, the
        // count is decremented twice (one for the system Irp and the 
        // other for the device Irp. An increment here compensates for 
        // the sytem irp..The decrement corresponding to this increment 
        // is in the completion function
        //

        //DBGPRINT(RT_DEBUG_TRACE,("IdleNotificationCallback::"));
        BulkUsb_IoIncrement(DeviceExtension);

        powerState.DeviceState = (DEVICE_POWER_STATE)DeviceExtension->PowerDownLevel;

        KeInitializeEvent(&irpCompletionEvent, NotificationEvent, FALSE);

        irpContext->DeviceExtension = DeviceExtension;
        irpContext->Event = &irpCompletionEvent;

        ntStatus = PoRequestPowerIrp(
                          DeviceExtension->PhysicalDeviceObject, 
                          IRP_MN_SET_POWER, 
                          powerState, 
                          (PREQUEST_POWER_COMPLETE) PoIrpCompletionFunc,
                          irpContext, 
                          NULL);

        if(STATUS_PENDING == ntStatus) {

            //DBGPRINT(RT_DEBUG_TRACE,("IdleNotificationCallback::"
            //               "waiting for the power irp to complete\n"));

            KeWaitForSingleObject(&irpCompletionEvent,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);
        }
	}
    
    if(!NT_SUCCESS(ntStatus)) {

        if(irpContext) {

            ExFreePool(irpContext);
        }
    }

    //DBGPRINT(RT_DEBUG_TRACE,("IdleNotificationCallback - ends\n"));
}



NTSTATUS
IdleNotificationRequestComplete(
    IN PDEVICE_OBJECT    DeviceObject,
    IN PIRP              Irp,
    IN PVOID			 Context
    )
/*++
 
Routine Description:

  Completion routine for idle notification irp

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet
    DeviceExtension - pointer to device extension

Return Value:

    NT status value

--*/
{
    NTSTATUS                ntStatus;
    POWER_STATE             powerState;
    KIRQL                   oldIrql;
    LARGE_INTEGER           dueTime;
    PIRP                    idleIrp;
    PUSB_IDLE_CALLBACK_INFO idleCallbackInfo;
	
	RTMP_ADAPTER *DeviceExtension = (RTMP_ADAPTER *)Context;
	
	UNREFERENCED_PARAMETER( DeviceObject );
	
    DBGPRINT(RT_DEBUG_TRACE,("IdleNotificationRequestCompete - begins\n"));

    idleIrp = NULL;

    //
    // check the Irp status
    //

    ntStatus = Irp->IoStatus.Status;

    if(!NT_SUCCESS(ntStatus) && ntStatus != STATUS_NOT_SUPPORTED) {

        BulkUsb_DbgPrint(1, ("Idle irp completes with error::"));

        switch(ntStatus) {
            
        case STATUS_INVALID_DEVICE_REQUEST:

            BulkUsb_DbgPrint(1, ("STATUS_INVALID_DEVICE_REQUEST\n"));

            break;

        case STATUS_CANCELLED:

            BulkUsb_DbgPrint(1, ("STATUS_CANCELLED\n"));

            break;

        case STATUS_POWER_STATE_INVALID:

            BulkUsb_DbgPrint(1, ("STATUS_POWER_STATE_INVALID\n"));

            goto IdleNotificationRequestComplete_Exit;

        case STATUS_DEVICE_BUSY:

            BulkUsb_DbgPrint(1, ("STATUS_DEVICE_BUSY\n"));

            break;

        default:

            BulkUsb_DbgPrint(1, ("default: status = %X\n", ntStatus));

            break;
        }

        //
        // if in error, issue a SetD0 (only when not in D0)
        //

        if(PowerDeviceD0 != DeviceExtension->DevPower) {
            DBGPRINT(RT_DEBUG_TRACE,("IdleNotificationRequestComplete::"));
            BulkUsb_IoIncrement(DeviceExtension);

            powerState.DeviceState = PowerDeviceD0;

            ntStatus = PoRequestPowerIrp(
                              DeviceExtension->PhysicalDeviceObject, 
                              IRP_MN_SET_POWER, 
                              powerState, 
                              (PREQUEST_POWER_COMPLETE) PoIrpAsyncCompletionFunc, 
                              DeviceExtension, 
                              NULL);

            if(!NT_SUCCESS(ntStatus)) {

                BulkUsb_DbgPrint(1, ("PoRequestPowerIrp failed\n"));
            }
        }
    }

IdleNotificationRequestComplete_Exit:

    KeAcquireSpinLock(&DeviceExtension->IdleReqStateLock, &oldIrql);

    idleCallbackInfo = DeviceExtension->IdleCallbackInfo;

    DeviceExtension->IdleCallbackInfo = NULL;

    idleIrp = (PIRP) InterlockedExchangePointer(
                                        (PVOID*)&DeviceExtension->PendingIdleIrp,//++ (PVOID*) for x64
                                        NULL);

    InterlockedExchange(&DeviceExtension->IdleReqPend, 0);

    KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);

    if(idleCallbackInfo) {

        ExFreePool(idleCallbackInfo);

    }

    //
    // since the irp was created using IoAllocateIrp, 
    // the Irp needs to be freed using IoFreeIrp.
    // Also return STATUS_MORE_PROCESSING_REQUIRED so that 
    // the kernel does not reference this in the near future.
    //

    if(idleIrp) {

        DBGPRINT(RT_DEBUG_TRACE,("completion routine has a valid irp and frees it\n"));
        IoFreeIrp(Irp);
        KeSetEvent(&DeviceExtension->NoIdleReqPendEvent,
                   IO_NO_INCREMENT,
                   FALSE);
    }
    else {

        //
        // The CancelSelectiveSuspend routine has grabbed the Irp from the device 
        // extension. Now the last one to decrement the FreeIdleIrpCount should
        // free the irp.
        //
        if(0 == InterlockedDecrement(&DeviceExtension->FreeIdleIrpCount)) {

            DBGPRINT(RT_DEBUG_TRACE,("completion routine frees the irp\n"));
            IoFreeIrp(Irp);

            KeSetEvent(&DeviceExtension->NoIdleReqPendEvent,
                       IO_NO_INCREMENT,
                       FALSE);
        }
    }

    /*if(DeviceExtension->SSEnable) {

        DBGPRINT(RT_DEBUG_TRACE,("Set the timer to fire DPCs\n"));

        dueTime.QuadPart = -10000 * IDLE_INTERVAL;               // 5000 ms

        KeSetTimerEx(&DeviceExtension->Timer, 
                     dueTime,
                     IDLE_INTERVAL,                              // 5000 ms
                     &DeviceExtension->DeferredProcCall);

        DBGPRINT(RT_DEBUG_TRACE,("IdleNotificationRequestCompete - ends\n"));
    }*/

    return STATUS_MORE_PROCESSING_REQUIRED;
}
VOID
PoIrpCompletionFunc(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    )
/*++
 
Routine Description:

    Completion routine for power irp PoRequested in 
    IdleNotificationCallback.

Arguments:

    DeviceObject - pointer to device object
    MinorFunciton - minor function for the irp.
    PowerState - irp power state
    Context - context passed to the completion function
    IoStatus - status block.

Return Value:

    None

--*/
{
    PIRP_COMPLETION_CONTEXT irpContext;
    
    UNREFERENCED_PARAMETER( DeviceObject );
    UNREFERENCED_PARAMETER( MinorFunction );
    UNREFERENCED_PARAMETER( PowerState );
    UNREFERENCED_PARAMETER( IoStatus );
    
    //
    // initialize variables
    //
    irpContext = NULL;

    if(Context) {

        irpContext = (PIRP_COMPLETION_CONTEXT) Context;
    }

    //
    // all we do is set the event and decrement the count
    //

    if(irpContext) {

        KeSetEvent(irpContext->Event, 0, FALSE);

        DBGPRINT(RT_DEBUG_TRACE,("PoIrpCompletionFunc::"));
        BulkUsb_IoDecrement(irpContext->DeviceExtension);

        ExFreePool(irpContext);

    }

    return;
}

VOID
PoIrpAsyncCompletionFunc(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    )
/*++
 
Routine Description:

    Completion routine for power irp PoRequested in IdleNotification
    RequestComplete routine.

Arguments:

    DeviceObject - pointer to device object
    MinorFunciton - minor function for the irp.
    PowerState - irp power state
    Context - context passed to the completion function
    IoStatus - status block.

Return Value:

    None

--*/
{
    RTMP_ADAPTER *DeviceExtension;
    
    UNREFERENCED_PARAMETER( DeviceObject );
    UNREFERENCED_PARAMETER( MinorFunction );
    UNREFERENCED_PARAMETER( PowerState );
    UNREFERENCED_PARAMETER( IoStatus );

    //
    // initialize variables
    //
    DeviceExtension = (RTMP_ADAPTER *) Context;

    //
    // all we do is decrement the count
    //
    
    DBGPRINT(RT_DEBUG_TRACE,("PoIrpAsyncCompletionFunc::"));
    BulkUsb_IoDecrement(DeviceExtension);

    return;

}

VOID
WWIrpCompletionFunc(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    )
/*++
 
Routine Description:

    Completion routine for PoRequest wait wake irp

Arguments:

    DeviceObject - pointer to device object
    MinorFunciton - minor function for the irp.
    PowerState - irp power state
    Context - context passed to the completion function
    IoStatus - status block.    

Return Value:

    None

--*/
{
    RTMP_ADAPTER *DeviceExtension;
    
    UNREFERENCED_PARAMETER( DeviceObject );
    UNREFERENCED_PARAMETER( MinorFunction );
    UNREFERENCED_PARAMETER( PowerState );
    UNREFERENCED_PARAMETER( IoStatus );

    //
    // initialize variables
    //
    DeviceExtension = (RTMP_ADAPTER *) Context;

    //
    // all we do is decrement the count
    //
    
    DBGPRINT(RT_DEBUG_TRACE,("WWIrpCompletionFunc::"));
    BulkUsb_IoDecrement(DeviceExtension);

    return;
}

NTSTATUS
BulkUsb_ResetPipe(
    IN PDEVICE_OBJECT         DeviceObject,
    IN PUSBD_PIPE_INFORMATION PipeInfo
    )
/*++
 
Routine Description:

    This routine synchronously submits a URB_FUNCTION_RESET_PIPE
    request down the stack.

Arguments:

    DeviceObject - pointer to device object
    PipeInfo - pointer to PipeInformation structure
               to retrieve the pipe handle

Return Value:

    NT status value

--*/
{
    PURB              urb;
    NTSTATUS          ntStatus;
    RTMP_ADAPTER *deviceExtension;

    //
    // initialize variables
    //

    urb = NULL;
    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;


    urb = (PURB)ExAllocatePool(NonPagedPool, 
                         sizeof(struct _URB_PIPE_REQUEST));

    if(urb) {

        urb->UrbHeader.Length = (USHORT) sizeof(struct _URB_PIPE_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
        urb->UrbPipeRequest.PipeHandle = PipeInfo->PipeHandle;

        ntStatus = CallUSBD(DeviceObject, urb);

        ExFreePool(urb);
    }
    else {

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if(NT_SUCCESS(ntStatus)) {
    
        DBGPRINT(RT_DEBUG_TRACE,("BulkUsb_ResetPipe - success\n"));
        ntStatus = STATUS_SUCCESS;
    }
    else {

        BulkUsb_DbgPrint(1, ("BulkUsb_ResetPipe - failed\n"));
    }

    return ntStatus;
}


NTSTATUS
BulkUsb_ResetDevice(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++
 
Routine Description:

    This routine invokes BulkUsb_ResetParentPort to reset the device

Arguments:

    DeviceObject - pointer to device object

Return Value:

    NT status value

--*/
{
    NTSTATUS ntStatus;
    ULONG    portStatus;

    DBGPRINT(RT_DEBUG_TRACE,("BulkUsb_ResetDevice - begins\n"));

    ntStatus = BulkUsb_GetPortStatus(DeviceObject, &portStatus);

    if((NT_SUCCESS(ntStatus))                 &&
       (!(portStatus & USBD_PORT_ENABLED))    &&
       (portStatus & USBD_PORT_CONNECTED)) {

        ntStatus = BulkUsb_ResetParentPort(DeviceObject);
    }

    DBGPRINT(RT_DEBUG_TRACE,("BulkUsb_ResetDevice - ends\n"));

    return ntStatus;
}


NTSTATUS
BulkUsb_GetPortStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PULONG     PortStatus
    )
/*++
 
Routine Description:

    This routine retrieves the status value

Arguments:

    DeviceObject - pointer to device object
    PortStatus - port status

Return Value:

    NT status value

--*/
{
    NTSTATUS           ntStatus;
    KEVENT             event;
    PIRP               irp;
    IO_STATUS_BLOCK    ioStatus;
    PIO_STACK_LOCATION nextStack;
    RTMP_ADAPTER *deviceExtension;

   if(NULL == DeviceObject)
	return USBD_STATUS_DEVICE_GONE;
    //
    // initialize variables
    //
    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;
    *PortStatus = 0;

    DBGPRINT(RT_DEBUG_TRACE,("BulkUsb_GetPortStatus - begins\n"));

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                    IOCTL_INTERNAL_USB_GET_PORT_STATUS,
                    deviceExtension->TopOfStackDeviceObject,
                    NULL,
                    0,
                    NULL,
                    0,
                    TRUE,
                    &event,
                    &ioStatus);

    if(NULL == irp) {

        BulkUsb_DbgPrint(1, ("memory alloc for irp failed\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nextStack = IoGetNextIrpStackLocation(irp);

    ASSERT(nextStack != NULL);

    nextStack->Parameters.Others.Argument1 = PortStatus;

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    if(STATUS_PENDING == ntStatus) {

        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
    }
    else {

        ioStatus.Status = ntStatus;
    }

    ntStatus = ioStatus.Status;

    DBGPRINT(RT_DEBUG_TRACE,("BulkUsb_GetPortStatus - ends\n"));

    return ntStatus;

}


NTSTATUS
BulkUsb_ResetParentPort(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++
 
Routine Description:

    This routine sends an IOCTL_INTERNAL_USB_RESET_PORT
    synchronously down the stack.

Arguments:

Return Value:

--*/
{
    NTSTATUS           ntStatus;
    KEVENT             event;
    PIRP               irp;
    IO_STATUS_BLOCK    ioStatus;
    PIO_STACK_LOCATION nextStack;
    RTMP_ADAPTER *deviceExtension;

    //
    // initialize variables
    //
    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

    DBGPRINT(RT_DEBUG_TRACE,("BulkUsb_ResetParentPort - begins\n"));

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                    IOCTL_INTERNAL_USB_RESET_PORT,
                    deviceExtension->TopOfStackDeviceObject,
                    NULL,
                    0,
                    NULL,
                    0,
                    TRUE,
                    &event,
                    &ioStatus);

    if(NULL == irp) {

        BulkUsb_DbgPrint(1, ("memory alloc for irp failed\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nextStack = IoGetNextIrpStackLocation(irp);

    ASSERT(nextStack != NULL);

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    if(STATUS_PENDING == ntStatus) {

        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
    }
    else {

        ioStatus.Status = ntStatus;
    }

    ntStatus = ioStatus.Status;

    DBGPRINT(RT_DEBUG_TRACE,("BulkUsb_ResetParentPort - ends\n"));

    return ntStatus;
}

VOID LfsrInit(
    IN RTMP_ADAPTER *dx, 
    IN ULONG Seed) 
{

    if (Seed == 0) 
        dx->ShiftReg = 1;
    else 
        dx->ShiftReg = Seed;
}

//BulkIn pipe 0 endpoint 0x84
//BulkIn pipe 1 endpoint 0x85

//BulkOut pipe 0 endpoint 8
//BulkOut pipe 1 endpoint 4
//BulkOut pipe 2 endpoint 5
//BulkOut pipe 3 endpoint 6
//BulkOut pipe 4 endpoint 7
//BulkOut pipe 5 endpoint 9
NTSTATUS UsbDoBulkOutTransfer(RTMP_ADAPTER *dx,int iBulkNum)
{
	NTSTATUS reStatus = 0;
	PIO_STACK_LOCATION      stack;
	//PTX_INF_STRUC pTxINF;
	PTX_WI_STRUC pTxWI;
	PIO_COMPLETION_ROUTINE CompletionRoutine;
	PULONG ptr;
		
	DBGPRINT(RT_DEBUG_TRACE,("%s -->>> iBulkNum=%d\n",__FUNCTION__,iBulkNum));
	DBGPRINT(RT_DEBUG_TRACE,("dx->BulkOutTxType = %d\n", dx->BulkOutTxType));
	DBGPRINT(RT_DEBUG_TRACE,("UsbDoBulkOutTransfer0()>>> bulkout length=0x%X\n",dx->TxBufferLength[iBulkNum]));

	if(NULL==dx->pTxIrp[iBulkNum])
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s  dx->pTxIrp==NULL don't  do UsbBuildInterruptOrBulkTransferRequest, return!!! \n",__FUNCTION__,iBulkNum));
		return reStatus;
	}	

	dx->TransmittedCount++;
	dx->TxBulkCount[BULKOUT_PIPENUMBER_0] ++;

	DBGPRINT(RT_DEBUG_TRACE,("dx->ContinBulkOut = %d, Bulk Out Remained = %d\n", dx->ContinBulkOut, dx->BulkOutRemained));
	if (dx->BulkOutRemained > 0)
	{
		dx->BulkOutRemained--;
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Remained = %d\n", dx->BulkOutRemained));
	}
		
	UsbBuildInterruptOrBulkTransferRequest(dx->pTxUrb[iBulkNum],
                                            sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                                            dx->UsbBulkOutPipeHandle[iBulkNum],
                                            dx->TxBuffer[iBulkNum],
                                            NULL,
                                            dx->TxBufferLength[iBulkNum],
                                            0,
                                            NULL);	

	ptr = (PULONG)(dx->TxBuffer[iBulkNum]);
	if(dx->TxBufferLength[iBulkNum]/4 < 20)
	{
		ULONG  i;
		
		for(i=0;i<dx->TxBufferLength[iBulkNum]/4;i++)
			DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) %d 0x%08X\n",i, *(ptr+i)));
	}
	else
	{
		int i;
		for(i=0;i<20;i++)
			DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) %d 0x%08X\n",i, *(ptr+i)));
	}
	
	stack = IoGetNextIrpStackLocation(dx->pTxIrp[iBulkNum]);
	stack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	stack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
	stack->Parameters.Others.Argument1 = (PVOID)dx->pTxUrb[iBulkNum];
	

	switch(iBulkNum)
	{
		case 0:
			CompletionRoutine = UsbDoBulkOutTransfer0Complete;
			break;

		case 1:
			CompletionRoutine = UsbDoBulkOutTransfer1Complete;
			break;

		case 2:
			CompletionRoutine = UsbDoBulkOutTransfer2Complete;
			break;

		case 3:
			CompletionRoutine = UsbDoBulkOutTransfer3Complete;
			break;

		case 4:
			CompletionRoutine = UsbDoBulkOutTransfer4Complete;
			break;

		case 5:
			CompletionRoutine = UsbDoBulkOutTransfer5Complete;
			break;

		default:
			DBGPRINT(RT_DEBUG_TRACE,("%s IoCallDriver return USBD_STATUS_INAVLID_PIPE_FLAGS error iBulkNum = %d\n",__FUNCTION__,iBulkNum));
			return USBD_STATUS_INAVLID_PIPE_FLAGS;
			break;
		

	}

	IoSetCompletionRoutine(dx->pTxIrp[iBulkNum],
                           CompletionRoutine,//UsbDoBulkOutTransfer0Complete,
                           dx,
                           TRUE,           // call on success
                           TRUE,           // call on error
                           TRUE);          // call on cancel


	dx->pTxIrp[iBulkNum]->Cancel = FALSE; // Clear cancel field in IRP
	reStatus = IoCallDriver(dx->TopOfStackDeviceObject, dx->pTxIrp[iBulkNum]);

	if( STATUS_PENDING == reStatus)
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s IoCallDriver return STATUS_PENDING\n",__FUNCTION__));
	}
	
	return reStatus;
}
VOID UsbDoBulkOutTransfer0(RTMP_ADAPTER *dx)
{
	PIO_STACK_LOCATION      stack;
	//PTX_INF_STRUC pTxINF;
	PTX_WI_STRUC pTxWI;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->>>\n",__FUNCTION__));

	DBGPRINT(RT_DEBUG_TRACE,("dx->BulkOutTxType = %d\n", dx->BulkOutTxType));
	DBGPRINT(RT_DEBUG_TRACE,("UsbDoBulkOutTransfer0()>>> bulkout length=0x%X\n",dx->TxBufferLength[BULKOUT_PIPENUMBER_0]));
	DBGPRINT(RT_DEBUG_TRACE,("UsbDoBulkOutTransfer0()>>> bulkout length=0x%X\n",dx->TxBufferLength[BULKOUT_PIPENUMBER_0]));

	dx->TransmittedCount++;
	dx->TxBulkCount[BULKOUT_PIPENUMBER_0] ++;

	DBGPRINT(RT_DEBUG_TRACE,("dx->ContinBulkOut = %d, Bulk Out Remained = %d\n", dx->ContinBulkOut, dx->BulkOutRemained));
	if (dx->BulkOutRemained > 0)
	{
		dx->BulkOutRemained--;
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Remained = %d\n", dx->BulkOutRemained));
	}
		
	UsbBuildInterruptOrBulkTransferRequest(dx->pTxUrb[BULKOUT_PIPENUMBER_0],
                                            sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                                            dx->UsbBulkOutPipeHandle[BULKOUT_PIPENUMBER_0],
                                            dx->TxBuffer[BULKOUT_PIPENUMBER_0],
                                            NULL,
                                            dx->TxBufferLength[BULKOUT_PIPENUMBER_0],
                                            0,
                                            NULL);	

	stack = IoGetNextIrpStackLocation(dx->pTxIrp[BULKOUT_PIPENUMBER_0]);
	stack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
	stack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
	stack->Parameters.Others.Argument1 = (PVOID)dx->pTxUrb[BULKOUT_PIPENUMBER_0];

	IoSetCompletionRoutine(dx->pTxIrp[BULKOUT_PIPENUMBER_0],
                           UsbDoBulkOutTransfer0Complete,
                           dx,
                           TRUE,           // call on success
                           TRUE,           // call on error
                           TRUE);          // call on cancel


	dx->pTxIrp[BULKOUT_PIPENUMBER_0]->Cancel = FALSE; // Clear cancel field in IRP
	IoCallDriver(dx->TopOfStackDeviceObject, dx->pTxIrp[BULKOUT_PIPENUMBER_0]);
                           
}
NTSTATUS UsbDoBulkOutTransfer0Complete(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
{
	//PTX_INF_STRUC pTxINF;
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)Context;

	DBGPRINT(RT_DEBUG_TRACE,("%s --->>>\n",__FUNCTION__));

	if (pIrp->IoStatus.Status == STATUS_CANCELLED)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Canceled\n"));
		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		KeSetEvent(&dx->CancelTxIrpEvent[BULKOUT_PIPENUMBER_0],1,FALSE);		
	}
	else if (pIrp->IoStatus.Status == STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Succeeded\n"));
		dx->OtherCounters.Ac0TxedCount++;
		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,0);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			/*pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_0];
			if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			{	
	 			pTxINF->NextVLD = 0;
				UsbDoBulkOutTransfer0(dx);			
			}*/
			dx->bBulkOutRunning = FALSE;
		}
	}
	else if(dx->pTxUrb[BULKOUT_PIPENUMBER_0]->UrbHeader.Status != 0)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Failed-- UrbHeader.Status = 0x%08X\n", dx->pTxUrb[BULKOUT_PIPENUMBER_0]->UrbHeader.Status));
		
		//UsbResetDevice(dx);
		BulkUsb_ResetDevice(DeviceObject);

#if 1
		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,0);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			//pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_0];
			//if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			//{	
	 		//	pTxINF->NextVLD = 0;
			//	UsbDoBulkOutTransfer0(dx);			
			//}
			dx->bBulkOutRunning = FALSE;
		}
#else		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		dx->bBulkOutRunning = FALSE;
#endif		
	}

	if (dx->bLoadingFW == TRUE)
	{
		if ((pIrp->IoStatus.Status != STATUS_SUCCESS) && (pIrp->IoStatus.Status != STATUS_CANCELLED))
		{
			DBGPRINT_RAW(RT_DEBUG_TRACE, ("pIrp->IoStatus.Status != STATUS_SUCCESS CANCELLED"));
			dx->bLoadingFW = FALSE;			
		}
		DBGPRINT_RAW(RT_DEBUG_TRACE, ("Set firmware event\n"));
		KeSetEvent(&dx->LoadFWEvent, 0, FALSE);
	}



	DBGPRINT(RT_DEBUG_TRACE,("%s ---<<<\n",__FUNCTION__));
	return STATUS_MORE_PROCESSING_REQUIRED;
}
NTSTATUS UsbDoBulkOutTransfer1Complete(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
{
	//PTX_INF_STRUC pTxINF;
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)Context;

	DBGPRINT(RT_DEBUG_TRACE,("%s --->>>\n",__FUNCTION__));

	if (pIrp->IoStatus.Status == STATUS_CANCELLED)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Canceled\n"));
		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		KeSetEvent(&dx->CancelTxIrpEvent[BULKOUT_PIPENUMBER_1],1,FALSE);		
	}
	else if (pIrp->IoStatus.Status == STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Succeeded\n"));

		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,1);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			/*pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_0];
			if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			{	
	 			pTxINF->NextVLD = 0;
				UsbDoBulkOutTransfer1(dx);			
			}*/
			dx->bBulkOutRunning = FALSE;
		}
	}
	else if(dx->pTxUrb[BULKOUT_PIPENUMBER_1]->UrbHeader.Status != 0)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Failed-- UrbHeader.Status = 0x%08X\n", dx->pTxUrb[BULKOUT_PIPENUMBER_1]->UrbHeader.Status));
		
		//UsbResetDevice(dx);
		BulkUsb_ResetDevice(DeviceObject);

#if 1
		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,1);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			//pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_1];
			//if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			//{	
	 		//	pTxINF->NextVLD = 0;
			//	UsbDoBulkOutTransfer1(dx);			
			//}
			dx->bBulkOutRunning = FALSE;
		}
#else		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		dx->bBulkOutRunning = FALSE;
#endif		
	}



	DBGPRINT(RT_DEBUG_TRACE,("%s ---<<<\n",__FUNCTION__));
	return STATUS_MORE_PROCESSING_REQUIRED;

}

NTSTATUS UsbDoBulkOutTransfer2Complete(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
{
	//PTX_INF_STRUC pTxINF;
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)Context;

	DBGPRINT(RT_DEBUG_TRACE,("%s --->>>\n",__FUNCTION__));

	if (pIrp->IoStatus.Status == STATUS_CANCELLED)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Canceled\n"));
		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		KeSetEvent(&dx->CancelTxIrpEvent[BULKOUT_PIPENUMBER_2],1,FALSE);		
	}
	else if (pIrp->IoStatus.Status == STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Succeeded\n"));

		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,2);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			/*pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_2];
			if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			{	
	 			pTxINF->NextVLD = 0;
				UsbDoBulkOutTransfer2(dx);			
			}*/
			dx->bBulkOutRunning = FALSE;
		}
	}
	else if(dx->pTxUrb[BULKOUT_PIPENUMBER_2]->UrbHeader.Status != 0)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Failed-- UrbHeader.Status = 0x%08X\n", dx->pTxUrb[BULKOUT_PIPENUMBER_2]->UrbHeader.Status));
		
		//UsbResetDevice(dx);
		BulkUsb_ResetDevice(DeviceObject);

#if 1
		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,2);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			//pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_2];
			//if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			//{	
	 		//	pTxINF->NextVLD = 0;
			//	UsbDoBulkOutTransfer2(dx);			
			//}
			dx->bBulkOutRunning = FALSE;
		}
#else		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		dx->bBulkOutRunning = FALSE;
#endif		
	}



	DBGPRINT(RT_DEBUG_TRACE,("%s ---<<<\n",__FUNCTION__));
	return STATUS_MORE_PROCESSING_REQUIRED;

}


NTSTATUS UsbDoBulkOutTransfer3Complete(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
{
	//PTX_INF_STRUC pTxINF;
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)Context;

	DBGPRINT(RT_DEBUG_TRACE,("%s --->>>\n",__FUNCTION__));

	if (pIrp->IoStatus.Status == STATUS_CANCELLED)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Canceled\n"));
		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		KeSetEvent(&dx->CancelTxIrpEvent[BULKOUT_PIPENUMBER_3],1,FALSE);		
	}
	else if (pIrp->IoStatus.Status == STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Succeeded\n"));

		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,3);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			/*pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_3];
			if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			{	
	 			pTxINF->NextVLD = 0;
				UsbDoBulkOutTransfer3(dx);			
			}*/
			dx->bBulkOutRunning = FALSE;
		}
	}
	else if(dx->pTxUrb[BULKOUT_PIPENUMBER_3]->UrbHeader.Status != 0)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Failed-- UrbHeader.Status = 0x%08X\n", dx->pTxUrb[BULKOUT_PIPENUMBER_3]->UrbHeader.Status));
		
		//UsbResetDevice(dx);
		BulkUsb_ResetDevice(DeviceObject);

#if 1
		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,3);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			//pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_3];
			//if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			//{	
	 		//	pTxINF->NextVLD = 0;
			//	UsbDoBulkOutTransfer3(dx);			
			//}
			dx->bBulkOutRunning = FALSE;
		}
#else		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		dx->bBulkOutRunning = FALSE;
#endif		
	}



	DBGPRINT(RT_DEBUG_TRACE,("%s ---<<<\n",__FUNCTION__));
	return STATUS_MORE_PROCESSING_REQUIRED;

}

NTSTATUS UsbDoBulkOutTransfer4Complete(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
{
	//PTX_INF_STRUC pTxINF;
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)Context;

	DBGPRINT(RT_DEBUG_TRACE,("%s --->>>\n",__FUNCTION__));

	if (pIrp->IoStatus.Status == STATUS_CANCELLED)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Canceled\n"));
		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		KeSetEvent(&dx->CancelTxIrpEvent[BULKOUT_PIPENUMBER_4],1,FALSE);		
	}
	else if (pIrp->IoStatus.Status == STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Succeeded\n"));

		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,4);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			/*pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_4];
			if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			{	
	 			pTxINF->NextVLD = 0;
				UsbDoBulkOutTransfer4(dx);			
			}*/
			dx->bBulkOutRunning = FALSE;
		}
	}
	else if(dx->pTxUrb[BULKOUT_PIPENUMBER_4]->UrbHeader.Status != 0)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Failed-- UrbHeader.Status = 0x%08X\n", dx->pTxUrb[BULKOUT_PIPENUMBER_4]->UrbHeader.Status));
		
		//UsbResetDevice(dx);
		BulkUsb_ResetDevice(DeviceObject);

#if 1
		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,4);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			//pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_4];
			//if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			//{	
	 		//	pTxINF->NextVLD = 0;
			//	UsbDoBulkOutTransfer4(dx);			
			//}
			dx->bBulkOutRunning = FALSE;
		}
#else		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		dx->bBulkOutRunning = FALSE;
#endif		
	}



	DBGPRINT(RT_DEBUG_TRACE,("%s ---<<<\n",__FUNCTION__));
	return STATUS_MORE_PROCESSING_REQUIRED;

}
	
NTSTATUS UsbDoBulkOutTransfer5Complete(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
{
	//PTX_INF_STRUC pTxINF;
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)Context;

	DBGPRINT(RT_DEBUG_TRACE,("%s --->>>\n",__FUNCTION__));

	if (pIrp->IoStatus.Status == STATUS_CANCELLED)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Canceled\n"));
		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		KeSetEvent(&dx->CancelTxIrpEvent[BULKOUT_PIPENUMBER_5],1,FALSE);		
	}
	else if (pIrp->IoStatus.Status == STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Succeeded\n"));

		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,5);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			/*pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_5];
			if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			{	
	 			pTxINF->NextVLD = 0;
				UsbDoBulkOutTransfer5(dx);			
			}*/
			dx->bBulkOutRunning = FALSE;
		}
	}
	else if(dx->pTxUrb[BULKOUT_PIPENUMBER_5]->UrbHeader.Status != 0)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Failed-- UrbHeader.Status = 0x%08X\n", dx->pTxUrb[BULKOUT_PIPENUMBER_5]->UrbHeader.Status));
		
		//UsbResetDevice(dx);
		BulkUsb_ResetDevice(DeviceObject);

#if 1
		if (((dx->ContinBulkOut == TRUE) || (dx->BulkOutRemained > 0)))
		{
			UsbDoBulkOutTransfer(dx,5);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("Bulk Out Stoped\n"));
			//pTxINF = ( PTX_INF_STRUC )dx->TxBuffer[BULKOUT_PIPENUMBER_5];
			//if ( dx->bNextVLD  && ( pTxINF->NextVLD != 0))
			//{	
	 		//	pTxINF->NextVLD = 0;
			//	UsbDoBulkOutTransfer5(dx);			
			//}
			dx->bBulkOutRunning = FALSE;
		}
#else		
		dx->BulkOutRemained = 0;
		dx->ContinBulkOut = FALSE;

		dx->bBulkOutRunning = FALSE;
#endif		
	}



	DBGPRINT(RT_DEBUG_TRACE,("%s ---<<<\n",__FUNCTION__));
	return STATUS_MORE_PROCESSING_REQUIRED;
}




NTSTATUS UsbDoBulkIn0TransferComplete(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
{
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)Context;
	KIRQL Irql;
	CCHAR		stackSize;

	DBGPRINT(RT_DEBUG_TRACE,("%s --->>>\n",__FUNCTION__));

	KeAcquireSpinLock(&dx->BulkInIoCallDriverSpinLock[BULKIN_PIPENUMBER_0], &Irql);
	dx->bBulkInIoCallDriverFlag[BULKIN_PIPENUMBER_0] = FALSE;
	KeReleaseSpinLock(&dx->BulkInIoCallDriverSpinLock[BULKIN_PIPENUMBER_0], Irql);	

	if (pIrp->IoStatus.Status == STATUS_CANCELLED)
	{
        	DBGPRINT(RT_DEBUG_TRACE,("Bulk In 0 Canceled\n"));
		
		dx->BulkInRemained[BULKIN_PIPENUMBER_0] = 0;
		dx->ContinBulkIn[BULKIN_PIPENUMBER_0] = FALSE;

		stackSize = dx->TopOfStackDeviceObject->StackSize;
		if(NULL==dx->pRxIrp[BULKIN_PIPENUMBER_0] )
			dx->pRxIrp[BULKIN_PIPENUMBER_0] = IoAllocateIrp(stackSize, FALSE);
		else
			DBGPRINT(RT_DEBUG_TRACE,("%s  dx->pRxIrp != NULL don't  do IoAllocateIrp  error !!!!!!!!!!\n",__FUNCTION__));

		//KeSetEvent(&dx->CancelRxIrpEvent[BULKIN_PIPENUMBER_0], 1, FALSE);

		//UsbResetDevice(dx);

	}
	else if (pIrp->IoStatus.Status == STATUS_SUCCESS)
	{
		PHEADER_802_11		pHeader;
		//PRX_FCEINFO_STRUC	pRxDMA_Length;
		PRX_WI_STRUC		pRxWI;
		//PRX_INF_STRUC		pRxINF;
		// pointer to 802.11 header
		PUCHAR				pPayload;
		ULONG		total_len = dx->pRxUrb[BULKIN_PIPENUMBER_0]->UrbBulkOrInterruptTransfer.TransferBufferLength;
		PUCHAR		pBuffer = dx->RxBuffer[BULKIN_PIPENUMBER_0];
		PULONG		pULONG = NULL;
		ULONG		agg_len = 0;
		ULONG		Offset;
		USHORT		SeqNum = 0;
		ULONG		Queidx;
		FwCMDRspTxD_STRUC FwCMDRspTxD;	
		RX_INFO_DW0 RxInfoDW0;
		memcpy(&RxInfoDW0,pBuffer,sizeof(RX_INFO_DW0));
		
        	DBGPRINT(RT_DEBUG_TRACE,("Bulk In pipe %d Succeeded\n",BULKIN_PIPENUMBER_0));
		DBGPRINT(RT_DEBUG_TRACE,("Bulk In total length = 0x%x\n", dx->pRxUrb[BULKIN_PIPENUMBER_0]->UrbBulkOrInterruptTransfer.TransferBufferLength));

		INC_COUNTER(dx->ReceivedCount[BULKIN_PIPENUMBER_0]);

		//print get input data
		DBGPRINT(RT_DEBUG_TRACE,("=========Bulk In pipe %d input data=================\n",BULKIN_PIPENUMBER_0));
		pULONG =(PULONG)pBuffer;
		if(total_len-4 < 44)
		{
			ULONG i;
			for(i=0;i<(total_len-4)/4;i++)
				DBGPRINT(RT_DEBUG_TRACE,("Bulkin 0 PGet Data %d 0x%08X\n",i, *(pULONG+i)));
		}
		else
		{
			int i;
			for(i=0;i<44/4;i++)
				DBGPRINT(RT_DEBUG_TRACE,("Get Data %d 0x%08X\n",i, *(pULONG+i)));
		}
		DBGPRINT(RT_DEBUG_TRACE,("===========================================\n"));
		
		CheckSecurityResult((PULONG)pBuffer,dx);
		
		if(*pULONG&0x70000000)
		{
			dx->OtherCounters.Rx0ReceivedCount++;
			DBGPRINT(RT_DEBUG_TRACE,("Driver Rx Count = %d\n",dx->OtherCounters.Rx0ReceivedCount));
		}
		
		if(sizeof(FwCMDRspTxD) == (total_len-4) )//4 //4 byte zero end 
		{
			memcpy(&FwCMDRspTxD,pBuffer,sizeof(FwCMDRspTxD));
			if(0xE000==FwCMDRspTxD.FwEventTxD.u2PacketType)//0xE000 if fw download format
			{
				g_FWSeqMCU = (UCHAR)FwCMDRspTxD.FwEventTxD.ucSeqNum;
				g_FWRspStatus = (UCHAR)FwCMDRspTxD.ucStatus;
				if(dx->bIsWaitFW == TRUE)
				{				
					KeSetEvent(&dx->WaitFWEvent, 0, FALSE);
					dx->bIsWaitFW =FALSE;
				}
				//Rom patch get sem event
				if(IS_MT7636(dx))
				{
					if(FwCMDRspTxD.FwEventTxD.ucEID == 0x4 && FwCMDRspTxD.FwEventTxD.ucSeqNum == SEQ_CMD_ROM_PATCH_SEMAPHORE)
					{
						dx->RomPatchSemStatus = g_FWRspStatus;
						KeSetEvent(&dx->RomPatchEvent, 0, FALSE);						
						DBGPRINT(RT_DEBUG_TRACE,("u2RxByteCount = 0x%x, ucEID = 0x%X, ucStatus = 0x%x, Seq = 0x%x\n", FwCMDRspTxD.FwEventTxD.u2RxByteCount, FwCMDRspTxD.FwEventTxD.ucEID, FwCMDRspTxD.FwEventTxD.ucSeqNum));
						
					}
				}
				switch(g_FWSeqMCU)
				{				
					case SEQ_CMD_FW_SCATTERS:
					{
						DBGPRINT(RT_DEBUG_TRACE,("#######SEQ_CMD_FW_SCATTERS  response########\n"));
					}
					break;
					case SEQ_CMD_FW_STARTREQ:
					{
						DBGPRINT(RT_DEBUG_TRACE,("#######SEQ_CMD_FW_STARTREQ  response########\n"));
					}
					break;
					case SEQ_CMD_FW_STARTTORUN:
					{
						DBGPRINT(RT_DEBUG_TRACE,("#######SEQ_CMD_FW_STARTTORUN  response########\n"));
					}					
					break;
					case SEQ_CMD_FW_RESTART:
					{
						DBGPRINT(RT_DEBUG_TRACE,("#######SEQ_CMD_FW_RESTART  response########\n"));
					}					
					case SEQ_CMD_ROM_PATCH_SEMAPHORE:					
					{
						DBGPRINT(RT_DEBUG_TRACE,("#######SEQ_CMD_ROM_PATCH_SEMAPHORE  response########\n"));
					}
					break;
					case SEQ_CMD_ROM_PATCH_STARTREQ:					
					{
						DBGPRINT(RT_DEBUG_TRACE,("#######SEQ_CMD_ROM_PATCH_STARTREQ  response########\n"));
					}
					break;
					case SEQ_CMD_ROM_PATCH_FINISH:
					{
						DBGPRINT(RT_DEBUG_TRACE,("#######SEQ_CMD_ROM_PATCH_FINISH  response########\n"));
					}
					break;
					default:
						DBGPRINT(RT_DEBUG_TRACE,("#######Unhandled response########\n"));
				}
				DBGPRINT(RT_DEBUG_TRACE,("u2RxByteCount = 0x%x, ucEID = 0x%X\n", FwCMDRspTxD.FwEventTxD.u2RxByteCount, FwCMDRspTxD.FwEventTxD.ucEID));
				DBGPRINT(RT_DEBUG_TRACE,("%s get seqnum = %d, from MCU  ,g_FWRspStatus =%d\n",__FUNCTION__,g_FWSeqMCU,g_FWRspStatus));
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE,("u2PacketType =0x%X error \n",FwCMDRspTxD.FwEventTxD.u2PacketType));
			}
			
		}
		//do
		//{

		//}while(1);
		if (dx->BulkInRemained[BULKIN_PIPENUMBER_0] > 0)
		{
			dx->BulkInRemained[BULKIN_PIPENUMBER_0]--;
			DBGPRINT(RT_DEBUG_TRACE,("%s Bulk In %d Remained = %d\n",__FUNCTION__,BULKIN_PIPENUMBER_0, dx->BulkInRemained[BULKIN_PIPENUMBER_0]));

			
		}

		if ((((dx->ContinBulkIn[BULKIN_PIPENUMBER_0] == TRUE) || (dx->BulkInRemained[BULKIN_PIPENUMBER_0] > 0)) && dx->bRxEnable[BULKIN_PIPENUMBER_0]==TRUE) || dx->bEnableSniffer==TRUE)
		{
			UsbDoBulkInTransfer(dx,BULKIN_PIPENUMBER_0);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("%s Bulk In Stoped\n",__FUNCTION__));
		}

	}
	else
	{
        	DBGPRINT(RT_DEBUG_TRACE,("%s Bulk In Failed\n",__FUNCTION__));

		BulkUsb_ResetDevice(DeviceObject);

		dx->BulkInRemained[BULKIN_PIPENUMBER_0] = 0;
		dx->ContinBulkIn[BULKIN_PIPENUMBER_0] = FALSE;
	}


	return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS UsbDoBulkIn1TransferComplete(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
{
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)Context;
	KIRQL Irql;
	CCHAR		stackSize;

	DBGPRINT(RT_DEBUG_TRACE,("%s --->>>\n",__FUNCTION__));

	KeAcquireSpinLock(&dx->BulkInIoCallDriverSpinLock[BULKIN_PIPENUMBER_1], &Irql);
	dx->bBulkInIoCallDriverFlag[BULKIN_PIPENUMBER_1] = FALSE;
	KeReleaseSpinLock(&dx->BulkInIoCallDriverSpinLock[BULKIN_PIPENUMBER_1], Irql);	

	if (pIrp->IoStatus.Status == STATUS_CANCELLED)
	{
        	DBGPRINT(RT_DEBUG_TRACE,("Bulk In 0 Canceled\n"));
		
		dx->BulkInRemained[BULKIN_PIPENUMBER_1] = 0;
		dx->ContinBulkIn[BULKIN_PIPENUMBER_1] = FALSE;

		stackSize = dx->TopOfStackDeviceObject->StackSize;
		dx->pRxIrp[BULKIN_PIPENUMBER_1] = IoAllocateIrp(stackSize, FALSE);

		//KeSetEvent(&dx->CancelRxIrpEvent[BULKIN_PIPENUMBER_0], 1, FALSE);

		//UsbResetDevice(dx);

	}
	else if (pIrp->IoStatus.Status == STATUS_SUCCESS)
	{		
		PHEADER_802_11		pHeader;
		//PRX_FCEINFO_STRUC	pRxDMA_Length;
		PRX_WI_STRUC		pRxWI;
		//PRX_INF_STRUC		pRxINF;
		// pointer to 802.11 header
		PUCHAR				pPayload;
		ULONG		total_len = dx->pRxUrb[BULKIN_PIPENUMBER_1]->UrbBulkOrInterruptTransfer.TransferBufferLength;
		PUCHAR		pBuffer = dx->RxBuffer[BULKIN_PIPENUMBER_1];
		ULONG		agg_len = 0;
		ULONG		Offset;
		USHORT		SeqNum = 0;
		ULONG		Queidx;
		PULONG		pULONG = NULL;
		PUCHAR		pTempBuffer = pBuffer;
		ULONG		PacketLength = total_len-4;//not include USB 4 byte zero end.
		FwCMDRspTxD_STRUC FwCMDRspTxD;
		CMD_CH_PRIVILEGE_T ChannelPayload;
		ULONG Temp = 0;

        	DBGPRINT(RT_DEBUG_TRACE,("Bulk In Succeeded\n"));
		DBGPRINT(RT_DEBUG_TRACE,("total length = %x\n", dx->pRxUrb[BULKIN_PIPENUMBER_1]->UrbBulkOrInterruptTransfer.TransferBufferLength));

		INC_COUNTER(dx->ReceivedCount[BULKIN_PIPENUMBER_1]);

		RtlZeroMemory(&ChannelPayload, sizeof(ChannelPayload));
		DBGPRINT(RT_DEBUG_TRACE,("%s  total_len = %d\n",__FUNCTION__, total_len));

		//print get input data
		DBGPRINT(RT_DEBUG_TRACE,("=========Bulk In pipe %d input data=================\n",BULKIN_PIPENUMBER_1));
		pULONG =(PULONG)pBuffer;
		if(total_len-4 < 44)
		{
			ULONG i;
			for(i=0;i<(total_len-4)/4;i++)
				DBGPRINT(RT_DEBUG_TRACE,("Bulkin 0 PGet Data %d 0x%08X\n",i, *(pULONG+i)));
		}
		else
		{
			int i;
			for(i=0;i<44/4;i++)
				DBGPRINT(RT_DEBUG_TRACE,("Get Data %d 0x%08X\n",i, *(pULONG+i)));
		}
		DBGPRINT(RT_DEBUG_TRACE,("===========================================\n"));
		//detect is fw download or fw cmd response.
		if(PacketLength>=sizeof(FwCMDRspTxD) )
		{
			memcpy(&FwCMDRspTxD,pBuffer,sizeof(FwCMDRspTxD));
			if(0xE000==FwCMDRspTxD.FwEventTxD.u2PacketType)
			{	
				if(FwCMDRspTxD.FwEventTxD.ucSeqNum <= MAX_FW_DOWNLOAD_SEQ)
				{//fw download seq num,FW cmd send to bulkin0, so don't do here
				  //only check fw channel resp here.
				  //switch channel event seq is 0, so detect here.
					if (FW_PKT_CMD_CH_PRIVILEGE_EVENT==FwCMDRspTxD.FwEventTxD.ucEID)
					{
						if ( (sizeof(FwCMDRspTxD)+sizeof(CMD_CH_PRIVILEGE_T)) == FwCMDRspTxD.FwEventTxD.u2RxByteCount)
						{
							pTempBuffer += sizeof(FwCMDRspTxD);
							memcpy(&ChannelPayload,pTempBuffer,sizeof(ChannelPayload));	
							g_PacketCMDSeqMCU = ChannelPayload.ucTokenID;

							DBGPRINT(RT_DEBUG_TRACE,("%s Get switchchannel seqnum =%d \n",__FUNCTION__, g_PacketCMDSeqMCU));
						}
						else
						{
							DBGPRINT(RT_DEBUG_TRACE,("%s u2RxByteCount =%d, != sizeof(FwCMDRspTxD)+sizeof(CMD_CH_PRIVILEGE_T), error  !!!\n",__FUNCTION__, FwCMDRspTxD.FwEventTxD.u2RxByteCount));
						}

					}

				}
				else
				{//packet cmd
					g_PacketCMDSeqMCU  = (UCHAR)FwCMDRspTxD.FwEventTxD.ucSeqNum;
					DBGPRINT(RT_DEBUG_TRACE,("%s Packet CMD get seqnum = %d, from MCU, u2RxByteCount=%d\n",__FUNCTION__,g_PacketCMDSeqMCU,FwCMDRspTxD.FwEventTxD.u2RxByteCount ));
					if( (FwCMDRspTxD.FwEventTxD.u2RxByteCount-sizeof(FwTxD_STRUC))<= sizeof(g_PacketCMDRspData))
					{
						memset(g_PacketCMDRspData,0,sizeof(g_PacketCMDRspData));
						memcpy(g_PacketCMDRspData,pTempBuffer,FwCMDRspTxD.FwEventTxD.u2RxByteCount);
					}
					else
					{
						DBGPRINT(RT_DEBUG_TRACE,("%s u2RxByteCount=%d > sizeof(g_PacketCMDRspData)(%d), error!!!!!\n",__FUNCTION__,FwCMDRspTxD.FwEventTxD.u2RxByteCount,sizeof(g_PacketCMDRspData)));
					}

				}
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE,("u2PacketType =0x%X not from FW(MCU) \n",FwCMDRspTxD.FwEventTxD.u2PacketType));
			}
		}


		//check if the packet is RX vector
		if(PacketLength>4)
		{
			HandleRXVector(pTempBuffer,dx);			
		}
		//check if the packet is RX vector end

		if(sizeof(FwCMDRspTxD) == total_len )
		{
			memcpy(&FwCMDRspTxD,pBuffer,sizeof(FwCMDRspTxD));
			g_FWSeqMCU = (UCHAR)FwCMDRspTxD.FwEventTxD.ucSeqNum;
			g_FWRspStatus = (UCHAR)FwCMDRspTxD.ucStatus;
			DBGPRINT(RT_DEBUG_TRACE,("%s get seqnum = %d, from MCU  ,g_FWRspStatus =%d\n",__FUNCTION__,g_FWSeqMCU,g_FWRspStatus));
		}
		//do
		//{

		//}while(1);
		if (dx->BulkInRemained[BULKIN_PIPENUMBER_1] > 0)
		{
			dx->BulkInRemained[BULKIN_PIPENUMBER_1]--;
			DBGPRINT(RT_DEBUG_TRACE,("%s Bulk In %d Remained = %d\n",__FUNCTION__,BULKIN_PIPENUMBER_1, dx->BulkInRemained[BULKIN_PIPENUMBER_1]));

			
		}

		if ((((dx->ContinBulkIn[BULKIN_PIPENUMBER_1] == TRUE) || (dx->BulkInRemained[BULKIN_PIPENUMBER_1] > 0)) && dx->bRxEnable[BULKIN_PIPENUMBER_1]==TRUE) || dx->bEnableSniffer==TRUE)
		{
			UsbDoBulkInTransfer(dx,BULKIN_PIPENUMBER_1);
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE,("%s Bulk In Stoped\n",__FUNCTION__));
		}

	}
	else
	{
        	DBGPRINT(RT_DEBUG_TRACE,("%s Bulk In Failed\n",__FUNCTION__));

		BulkUsb_ResetDevice(DeviceObject);

		dx->BulkInRemained[BULKIN_PIPENUMBER_1] = 0;
		dx->ContinBulkIn[BULKIN_PIPENUMBER_1] = FALSE;
	}


	return STATUS_MORE_PROCESSING_REQUIRED;
}


VOID UsbDoBulkInTransfer( IN RTMP_ADAPTER *dx,int iBulkNum)
{
	PIO_STACK_LOCATION      stack;
	KIRQL Irql;
	CCHAR stackSize = 0;
	PIO_COMPLETION_ROUTINE  pCompleteFunction = NULL;

	DBGPRINT(RT_DEBUG_TRACE,("%s iBulkNum=%d--->>\n",__FUNCTION__,iBulkNum));

	if(BULKIN_PIPENUMBER_0>iBulkNum || BULKIN_PIPENUMBER_1 < iBulkNum)
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s iBulkNum=%d error !!!!!!\n",__FUNCTION__,iBulkNum));
		return;
	}
	
	
	KeAcquireSpinLock(&dx->BulkInIoCallDriverSpinLock[iBulkNum], &Irql);
	if(FALSE == dx->bBulkInIoCallDriverFlag[iBulkNum])
	{
		KeReleaseSpinLock(&dx->BulkInIoCallDriverSpinLock[iBulkNum], Irql);	
		if(NULL == dx->pRxIrp[iBulkNum])
		{
			DBGPRINT(RT_DEBUG_TRACE,("%s  dx->pRxIrp = NULL do IoAllocateIrp error!!!!\n",__FUNCTION__));
			stackSize = dx->TopOfStackDeviceObject->StackSize;
			dx->pRxIrp[BULKIN_PIPENUMBER_0] = IoAllocateIrp(stackSize, FALSE);
		}
		IoReuseIrp(dx->pRxIrp[iBulkNum], STATUS_SUCCESS);

		UsbBuildInterruptOrBulkTransferRequest(dx->pRxUrb[iBulkNum],
	                                           sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
	                                           dx->UsbBulkInPipeHandle[iBulkNum],
	                                           dx->RxBuffer[iBulkNum],
	                                           NULL,
	                                           USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE,
	                                           USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,
	                                           NULL);

		stack = IoGetNextIrpStackLocation(dx->pRxIrp[iBulkNum]);
		stack->MajorFunction                            = IRP_MJ_INTERNAL_DEVICE_CONTROL;
		stack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
		stack->Parameters.Others.Argument1              = (PVOID)dx->pRxUrb[iBulkNum];

		if(BULKIN_PIPENUMBER_0==iBulkNum)
		{
			pCompleteFunction = UsbDoBulkIn0TransferComplete;
		}
		else
		{
			pCompleteFunction = UsbDoBulkIn1TransferComplete;
		}
		
		IoSetCompletionRoutine(dx->pRxIrp[iBulkNum],
	                           pCompleteFunction,
	                           dx,
	                           TRUE,         // call on success
	                           TRUE,         // call on error
	                           TRUE);        // call on cancel

		dx->pRxIrp[iBulkNum]->Cancel = FALSE; // Clear cancel field in IRP
		//IoMarkIrpPending(dx->pRxIrp[BULKIN_PIPENUMBER_0]);
		IoAcquireCancelSpinLock(&dx->pRxIrp[iBulkNum]->CancelIrql);
		IoSetCancelRoutine(dx->pRxIrp[iBulkNum], NULL);
		IoReleaseCancelSpinLock(dx->pRxIrp[iBulkNum]->CancelIrql);
		
		KeAcquireSpinLock(&dx->BulkInIoCallDriverSpinLock[iBulkNum], &Irql);
		dx->bBulkInIoCallDriverFlag[iBulkNum] = TRUE;
		KeReleaseSpinLock(&dx->BulkInIoCallDriverSpinLock[iBulkNum], Irql);	

		IoCallDriver(dx->TopOfStackDeviceObject, dx->pRxIrp[iBulkNum]);
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s bBulkInIoCallDriverFlag=%d don't call UsbBuildInterruptOrBulkTransferRequest !!!!!!\n",__FUNCTION__,dx->bBulkInIoCallDriverFlag[iBulkNum]));
		KeReleaseSpinLock(&dx->BulkInIoCallDriverSpinLock[iBulkNum], Irql);	
	}

	DBGPRINT(RT_DEBUG_TRACE,("%s <<---\n",__FUNCTION__));
}

NTSTATUS UsbBulkMemoryAlloc( IN RTMP_ADAPTER *dx)
{
	NTSTATUS	ntStatus = STATUS_SUCCESS;
	CCHAR		stackSize;
	int RxPipeCnt;
	int TxPipeCnt;

	DBGPRINT(RT_DEBUG_TRACE,("UsbBulkMemoryAlloc()>>>\n"));

	for(RxPipeCnt=0; RxPipeCnt<MAX_RX_BULK_PIPE_NUM; RxPipeCnt++)
	{
	
		dx->pRxUrb[RxPipeCnt] = (PURB)ExAllocatePool(NonPagedPool, sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));

		if ((dx->pRxUrb[RxPipeCnt] == NULL))
		{
			ntStatus = STATUS_INSUFFICIENT_RESOURCES;
			return ntStatus;		
		}
		
		//---------------------
		
		dx->RxBuffer[RxPipeCnt] = (PUCHAR)ExAllocatePool(NonPagedPool, USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE);
		
		if ((dx->RxBuffer[RxPipeCnt] == NULL))
		{
			ntStatus = STATUS_INSUFFICIENT_RESOURCES;
			return ntStatus;
		}
		
		RtlZeroMemory(dx->RxBuffer[RxPipeCnt], USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE);

		//---------------------

		stackSize = dx->TopOfStackDeviceObject->StackSize;
		dx->pRxIrp[RxPipeCnt] = IoAllocateIrp(stackSize, FALSE);

	}

	//dx->SnifferBuffer = (PUCHAR)ExAllocatePool(NonPagedPool, USB_BULK_BUFFER_SIZE*NUM_OF_MAX_SNIFFER_BLOCKS);
	//dx->SnifferBuffer = NULL;
	dx->bDriverSupportSniffer = FALSE;	
	

	dx->RXMPDUBuffer = (PUCHAR)ExAllocatePool(NonPagedPool, USB_BULK_BUFFER_SIZE);
	if ((dx->RXMPDUBuffer == NULL))
	{
		//dx->bDriverSupportSniffer = FALSE;	
		//ntStatus = STATUS_INSUFFICIENT_RESOURCES;
	}
	else
	{
		//dx->bDriverSupportSniffer = TRUE;
		RtlZeroMemory(dx->RXMPDUBuffer, USB_BULK_BUFFER_SIZE);
	}

	for(TxPipeCnt=0; TxPipeCnt<MAX_TX_BULK_PIPE_NUM; TxPipeCnt++)
	{
		//---------------------
		
		dx->pTxUrb[TxPipeCnt] = (PURB)ExAllocatePool(NonPagedPool, sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));

		if ((dx->pTxUrb[TxPipeCnt] == NULL))
		{
			ntStatus = STATUS_INSUFFICIENT_RESOURCES;
			return ntStatus;			
		}
		
		//---------------------
		
		dx->TxBuffer[TxPipeCnt] = (PUCHAR)ExAllocatePool(NonPagedPool, USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE);
		
		if ((dx->TxBuffer[TxPipeCnt] == NULL))
		{
			ntStatus = STATUS_INSUFFICIENT_RESOURCES;
			return ntStatus;			
		}
		
		RtlZeroMemory(dx->TxBuffer[TxPipeCnt], USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE);

		//---------------------

		stackSize = dx->TopOfStackDeviceObject->StackSize;
		dx->pTxIrp[TxPipeCnt] = IoAllocateIrp(stackSize, FALSE);			

	}


	return ntStatus;
}
VOID UsbBulkMemoryFree( IN PDEVICE_OBJECT	pDO)
{
	NTSTATUS	ntStatus = STATUS_SUCCESS;
	CCHAR		stackSize;
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *) pDO->DeviceExtension;
	int RxPipeCnt;
	int TxPipeCnt;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));

	//---------------------	
	for(RxPipeCnt=0; RxPipeCnt<MAX_RX_BULK_PIPE_NUM; RxPipeCnt++)
	{
		if(dx->pRxIrp[RxPipeCnt] != NULL && dx->pRxIrp[RxPipeCnt]->IoStatus.Status == STATUS_PENDING)
		{
			KeInitializeEvent(&dx->CancelRxIrpEvent[RxPipeCnt], SynchronizationEvent, FALSE);
		
			IoCancelIrp(dx->pRxIrp[RxPipeCnt]);

	    		ntStatus = KeWaitForSingleObject(
	                       &dx->CancelRxIrpEvent[RxPipeCnt],
	                       Executive,
	                       KernelMode,
	                       FALSE,
	                       NULL);   

			if(!NT_SUCCESS(ntStatus)) 
			{
				ntStatus = BulkUsb_ResetPipe(pDO, dx->UsbBulkInPipeInfo);
			}

			if(!NT_SUCCESS(ntStatus)) 
			{
				ntStatus = BulkUsb_ResetDevice(pDO);
			}		
		}

		if(dx->pRxIrp[RxPipeCnt] != NULL)
		{
			IoFreeIrp(dx->pRxIrp[RxPipeCnt]);
			dx->pRxIrp[RxPipeCnt] = NULL;
		}
	
	}
	//---------------------	
	for(TxPipeCnt=0; TxPipeCnt<MAX_TX_BULK_PIPE_NUM; TxPipeCnt++)
	{
		if(dx->pTxIrp[TxPipeCnt] != NULL && dx->pTxIrp[TxPipeCnt]->IoStatus.Status == STATUS_PENDING)
		{
			KeInitializeEvent(&dx->CancelTxIrpEvent[TxPipeCnt], SynchronizationEvent, FALSE);
		
			IoCancelIrp(dx->pTxIrp[TxPipeCnt]);		

	    		ntStatus = KeWaitForSingleObject(
	                       &dx->CancelTxIrpEvent[TxPipeCnt],
	                       Executive,
	                       KernelMode,
	                       FALSE,
	                       NULL);   

			if(!NT_SUCCESS(ntStatus)) 
			{			
				ntStatus =BulkUsb_ResetPipe(pDO, dx->UsbBulkInPipeInfo);
			}

			if(!NT_SUCCESS(ntStatus)) 
			{
				ntStatus = BulkUsb_ResetDevice(pDO);
			}		
		}

		if(dx->pTxIrp[TxPipeCnt] != NULL)
		{
			IoFreeIrp(dx->pTxIrp[TxPipeCnt]);
			dx->pTxIrp[TxPipeCnt] = NULL;
		}
	}
	//---------------------	
	for(RxPipeCnt=0; RxPipeCnt<MAX_RX_BULK_PIPE_NUM; RxPipeCnt++)
	{
		if ((dx->pRxUrb[RxPipeCnt] != NULL))
		{
			ExFreePool(dx->pRxUrb[RxPipeCnt]);
			dx->pRxUrb[RxPipeCnt] = NULL;
		}

		//---------------------

		if ((dx->RxBuffer[RxPipeCnt] != NULL))
		{
			ExFreePool(dx->RxBuffer[RxPipeCnt]);
			dx->RxBuffer[RxPipeCnt] = NULL;
		}
	}

	//---------------------	
	if ((dx->RXMPDUBuffer != NULL))
	{
		ExFreePool(dx->RXMPDUBuffer);
		dx->RXMPDUBuffer = NULL;		
	}
	
	//---------------------
	for(TxPipeCnt=0; TxPipeCnt<MAX_TX_BULK_PIPE_NUM; TxPipeCnt++)
	{
		//---------------------
		
		if ((dx->pTxUrb[TxPipeCnt] != NULL))
		{
			ExFreePool(dx->pTxUrb[TxPipeCnt]);
			dx->pTxUrb[TxPipeCnt] = NULL;
		}
		
		//---------------------
			
		if ((dx->TxBuffer[TxPipeCnt] != NULL))
		{
			ExFreePool(dx->TxBuffer[TxPipeCnt]);
			dx->TxBuffer[TxPipeCnt] = NULL;		
		}
		
		//---------------------
	}

	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));
}

NTSTATUS UsbCancelBulkInIrp( IN PDEVICE_OBJECT	pDO,int iBulkNum)
{
	NTSTATUS	ntStatus = STATUS_SUCCESS;
	PIRP  irp;
       KIRQL oldIrql;
	RTMP_ADAPTER *DeviceExtension = (RTMP_ADAPTER *)pDO->DeviceExtension;

	DBGPRINT(RT_DEBUG_TRACE,("%s >>>\n",__FUNCTION__));

       irp = NULL;

       DBGPRINT(RT_DEBUG_TRACE,("CancelSelectSuspend - begins\n"));

       KeAcquireSpinLock(&DeviceExtension->IdleReqStateLock, &oldIrql);
        irp = (PIRP) InterlockedExchangePointer(
                            (PVOID*)&DeviceExtension->pRxIrp[iBulkNum], //++ (PVOID*) for x64
                            NULL);

	KeReleaseSpinLock(&DeviceExtension->IdleReqStateLock, oldIrql);


	//
    // since we have a valid Irp ptr,
    // we can call IoCancelIrp on it,
    // without the fear of the irp 
    // being freed underneath us.
    //
    if(irp) 
    {

        //
        // This routine has the irp pointer.
        // It is safe to call IoCancelIrp because we know that
        // the compleiton routine will not free this irp unless...
        // 
        //        
        if(IoCancelIrp(irp)) 
	{
            DBGPRINT(RT_DEBUG_TRACE,("IoCancelIrp returns TRUE\n"));

	     if(DeviceExtension->pRxIrp[iBulkNum] != NULL)
	    {
		  CCHAR		stackSize;

		  IoFreeIrp(DeviceExtension->pRxIrp[iBulkNum]);
		  DeviceExtension->pRxIrp[iBulkNum] = NULL;

		  stackSize = DeviceExtension->TopOfStackDeviceObject->StackSize + 1;
		  DeviceExtension->pRxIrp[iBulkNum] = IoAllocateIrp(stackSize, FALSE);
		  DeviceExtension->ulBulkInRunning[0] = 0;
	    }
        }
        else 
	{
            DBGPRINT(RT_DEBUG_TRACE,("IoCancelIrp returns FALSE\n"));
        }

        //
        // ....we decrement the FreeIdleIrpCount from 2 to 1.
        // if completion routine runs ahead of us, then this routine 
        // decrements the FreeIdleIrpCount from 1 to 0 and hence shall
        // free the irp.
        //
        if(0 == InterlockedDecrement(&DeviceExtension->FreeIdleIrpCount))
	 {

            DBGPRINT(RT_DEBUG_TRACE,("CancelSelectSuspend frees the irp\n"));
            IoFreeIrp(irp);

            KeSetEvent(&DeviceExtension->NoIdleReqPendEvent,
                       IO_NO_INCREMENT,
                       FALSE);
        }
      }

	DBGPRINT(RT_DEBUG_TRACE,("%s <<<\n",__FUNCTION__));
       return ntStatus;
	

	
	/*DBGPRINT(RT_DEBUG_TRACE,("%s >>>\n",__FUNCTION__);

	KIRQL iRql = KeGetCurrentIrql();

	NTSTATUS	ntStatus = STATUS_SUCCESS;
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)pDO->DeviceExtension;

	if(KeGetCurrentIrql()<APC_LEVEL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s <APC_LEVEL \n",__FUNCTION__);
	}

	if(dx->pRxIrp[iBulkNum] != NULL && dx->pRxIrp[iBulkNum]->IoStatus.Status == STATUS_PENDING)
	{
		DBGPRINT(RT_DEBUG_TRACE,("UsbCancelBulkInIrp STATUS_PENDING\n");
	
		//KeInitializeEvent(&dx->CancelRxIrpEvent[iBulkNum], SynchronizationEvent, FALSE);
		KeInitializeEvent(&dx->CancelRxIrpEvent[iBulkNum], NotificationEvent, FALSE);
	
		ntStatus = IoCancelIrp(dx->pRxIrp[iBulkNum]);

		LARGE_INTEGER OneSecondTimeout;
		// Set up timeout
		//OneSecondTimeout.QuadPart = -1i64 * 1000000i64 * 10i64;
		OneSecondTimeout.QuadPart = -10000*50000;
		//memset(&OneSecondTimeout,0,sizeof(OneSecondTimeout));
		

    		ntStatus = KeWaitForSingleObject(
                       &dx->CancelRxIrpEvent[iBulkNum],
                       Executive,
                       KernelMode,
                       FALSE,
                       NULL);//&OneSecondTimeout);   

		if(!NT_SUCCESS(ntStatus)) 
		{
			DBGPRINT(RT_DEBUG_TRACE,("UsbResetPipe()\n");
		
			ntStatus = BulkUsb_ResetPipe(pDO, dx->UsbBulkInPipeInfo);
		}

		if(!NT_SUCCESS(ntStatus)) 
		{
			DBGPRINT(RT_DEBUG_TRACE,("UsbResetDevice()\n");
		
			ntStatus = BulkUsb_ResetDevice(pDO);
		}	

#if 1
		if(dx->pRxIrp[iBulkNum] != NULL)
		{
			IoFreeIrp(dx->pRxIrp[iBulkNum]);
			dx->pRxIrp[iBulkNum] = NULL;

			CCHAR		stackSize;
			stackSize = dx->TopOfStackDeviceObject->StackSize + 1;
			dx->pRxIrp[iBulkNum] = IoAllocateIrp(stackSize, FALSE);
		}				
#endif				

		
	}

	dx->ulBulkInRunning[0] = 0;

	DBGPRINT(RT_DEBUG_TRACE,("%s <<<\n",__FUNCTION__);

	return ntStatus;*/
}
