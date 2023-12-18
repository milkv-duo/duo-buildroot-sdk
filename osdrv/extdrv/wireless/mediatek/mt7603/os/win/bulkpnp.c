/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    bulkpnp.c

Abstract:

    Bulk USB device driver for Intel 82930 USB test board
    Plug and Play module.
    This file contains routines to handle pnp requests.
    These routines are not USB specific but is required
    for every driver which conforms to the WDM model.

Environment:

    Kernel mode

Notes:

    Copyright (c) 2000 Microsoft Corporation.  
    All Rights Reserved.

--*/

//#include "bulkusb.h"
#include "config.h"
//#include <Usbdlib.h>

/*#include "bulkpwr.h"
#include "bulkdev.h"
#include "bulkrwr.h"
#include "bulkwmi.h"
#include "bulkusr.h"*/


LONG
BulkUsb_IoIncrement(
    IN OUT RTMP_ADAPTER *DeviceExtension
    )
/*++
 
Routine Description:

    This routine bumps up the I/O count.
    This routine is typically invoked when any of the
    dispatch routines handle new irps for the driver.

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    new value

--*/
{
    LONG  result = 0;
    KIRQL oldIrql;

    KeAcquireSpinLock(&DeviceExtension->IOCountLock, &oldIrql);

    result = InterlockedIncrement((PLONG)&DeviceExtension->OutStandingIO);

    //
    // when OutStandingIO bumps from 1 to 2, clear the StopEvent
    //

    if(result == 2) {

        KeClearEvent(&DeviceExtension->StopEvent);
    }

    KeReleaseSpinLock(&DeviceExtension->IOCountLock, oldIrql);

    //BulkUsb_DbgPrint(3, ("BulkUsb_IoIncrement::%d\n", result));

    return result;
}



BOOLEAN
CanDeviceSuspend(
    IN RTMP_ADAPTER *DeviceExtension
    )
/*++
 
Routine Description:

    This is the routine where we check if the device
    can selectively suspend. 

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    TRUE - if the device can suspend
    FALSE - otherwise.

--*/
{
    //BulkUsb_DbgPrint(3, ("CanDeviceSuspend\n"));

    if((DeviceExtension->OpenHandleCount == 0) &&
        (DeviceExtension->OutStandingIO == 1)) {
        
        return TRUE;
    }
    else {

        return FALSE;
    }
}

NTSTATUS
IrpCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    )
/*++
 
Routine Description:

    This routine is a completion routine.
    In this routine we set an event.

    Since the completion routine returns 
    STATUS_MORE_PROCESSING_REQUIRED, the Irps,
    which set this routine as the completion routine,
    should be marked pending.

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet
    Context - 

Return Value:

    NT status value

--*/
{
    PKEVENT event = (PKEVENT)Context;

    KeSetEvent(event, 0, FALSE);

    return STATUS_MORE_PROCESSING_REQUIRED;
    
    UNREFERENCED_PARAMETER( Irp );
    UNREFERENCED_PARAMETER( DeviceObject );
}

NTSTATUS
ReadandSelectDescriptors(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++
 
Routine Description:

    This routine configures the USB device.
    In this routines we get the device descriptor, 
    the configuration descriptor and select the
    configuration descriptor.

Arguments:

    DeviceObject - pointer to a device object

Return Value:

    NTSTATUS - NT status value.

--*/
{
    PURB                   urb;
    ULONG                  siz;
    NTSTATUS               ntStatus = 0;
    PUSB_DEVICE_DESCRIPTOR deviceDescriptor;
    
    //
    // initialize variables
    //

    urb = NULL;
    deviceDescriptor = NULL;

    //
    // 1. Read the device descriptor
    //

    urb = (PURB)ExAllocatePool(NonPagedPool, 
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if(urb) {

        siz = sizeof(USB_DEVICE_DESCRIPTOR);
        deviceDescriptor = (PUSB_DEVICE_DESCRIPTOR)ExAllocatePool(NonPagedPool, siz);

        if(deviceDescriptor) {

            UsbBuildGetDescriptorRequest(
                    urb, 
                    (USHORT) sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    USB_DEVICE_DESCRIPTOR_TYPE, 
                    0, 
                    0, 
                    deviceDescriptor, 
                    NULL, 
                    siz, 
                    NULL);

            ntStatus = CallUSBD(DeviceObject, urb);

            if(NT_SUCCESS(ntStatus)) {

                ASSERT(deviceDescriptor->bNumConfigurations);
                ntStatus = ConfigureDevice(DeviceObject);    
            }
                            
            ExFreePool(urb);                
            ExFreePool(deviceDescriptor);
        }
        else {

            //DBGPRINT(RT_DEBUG_TRACE,("Failed to allocate memory for deviceDescriptor\n"));

            ExFreePool(urb);
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }
    }
    else {

        //DBGPRINT(RT_DEBUG_TRACE,("Failed to allocate memory for urb\n"));

        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    return ntStatus;
}
NTSTATUS
CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB           Urb
    )
/*++
 
Routine Description:

    This routine synchronously submits an urb down the stack.

Arguments:

    DeviceObject - pointer to device object
    Urb - USB request block

Return Value:

--*/
{
    PIRP               irp;
    KEVENT             event;
    NTSTATUS           ntStatus;
    IO_STATUS_BLOCK    ioStatus;
    PIO_STACK_LOCATION nextStack;
    RTMP_ADAPTER * deviceExtension;

    //
    // initialize the variables
    //

    irp = NULL;
    deviceExtension = (RTMP_ADAPTER *)DeviceObject->DeviceExtension;
    
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_SUBMIT_URB, 
                                        deviceExtension->TopOfStackDeviceObject,
                                        NULL, 
                                        0, 
                                        NULL, 
                                        0, 
                                        TRUE, 
                                        &event, 
                                        &ioStatus);

    if(!irp) {

        //DBGPRINT(RT_DEBUG_TRACE,("IoBuildDeviceIoControlRequest failed\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);
    nextStack->Parameters.Others.Argument1 = Urb;

   // BulkUsb_DbgPrint(3, ("CallUSBD::"));
    BulkUsb_IoIncrement(deviceExtension);

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, irp);

    if(ntStatus == STATUS_PENDING) {

        KeWaitForSingleObject(&event, 
                              Executive, 
                              KernelMode, 
                              FALSE, 
                              NULL);

        ntStatus = ioStatus.Status;
    }
    
    //BulkUsb_DbgPrint(3, ("CallUSBD::"));
    BulkUsb_IoDecrement(deviceExtension);
    return ntStatus;
}
LONG
BulkUsb_IoDecrement(
    IN OUT RTMP_ADAPTER *DeviceExtension
    )
/*++
 
Routine Description:

    This routine decrements the outstanding I/O count
    This is typically invoked after the dispatch routine
    has finished processing the irp.

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    new value

--*/
{
    LONG  result = 0;
    KIRQL oldIrql;

    KeAcquireSpinLock(&DeviceExtension->IOCountLock, &oldIrql);

    result = InterlockedDecrement((PLONG)&DeviceExtension->OutStandingIO);

    if(result == 1) {

        KeSetEvent(&DeviceExtension->StopEvent, IO_NO_INCREMENT, FALSE);
    }

    if(result == 0) {

        ASSERT(USBRemoved == DeviceExtension->DeviceState);

        KeSetEvent(&DeviceExtension->RemoveEvent, IO_NO_INCREMENT, FALSE);
    }

    KeReleaseSpinLock(&DeviceExtension->IOCountLock, oldIrql);

    //BulkUsb_DbgPrint(3, ("BulkUsb_IoDecrement::%d\n", result));

    return result;
}
NTSTATUS
ConfigureDevice(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    This helper routine reads the configuration descriptor
    for the device in couple of steps.

Arguments:

    DeviceObject - pointer to a device object

Return Value:

    NTSTATUS - NT status value

--*/
{
    PURB                          urb;
    ULONG                         siz;
    NTSTATUS                      ntStatus = 0;
    RTMP_ADAPTER *deviceExtension;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor;

    //
    // initialize the variables
    //

    urb = NULL;
    configurationDescriptor = NULL;
    deviceExtension = (RTMP_ADAPTER *)DeviceObject->DeviceExtension;

    ASSERT(deviceExtension->UsbConfigurationDescriptor == NULL);

    //
    // Read the first configuration descriptor
    // This requires two steps:
    // 1. Read the fixed sized configuration desciptor (CD)
    // 2. Read the CD with all embedded interface and endpoint descriptors
    //

    urb = (PURB)ExAllocatePool(NonPagedPool, 
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if(urb) {

        siz = sizeof(USB_CONFIGURATION_DESCRIPTOR);
        configurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)ExAllocatePool(NonPagedPool, siz);

        if(configurationDescriptor) {

            UsbBuildGetDescriptorRequest(
                    urb, 
                    (USHORT) sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    USB_CONFIGURATION_DESCRIPTOR_TYPE, 
                    0, 
                    0, 
                    configurationDescriptor,
                    NULL, 
                    sizeof(USB_CONFIGURATION_DESCRIPTOR), 
                    NULL);

            ntStatus = CallUSBD(DeviceObject, urb);

            if(!NT_SUCCESS(ntStatus)) {

                DBGPRINT(RT_DEBUG_TRACE,("UsbBuildGetDescriptorRequest failed\n"));
                goto ConfigureDevice_Exit;
            }
        }
        else {

            DBGPRINT(RT_DEBUG_TRACE,("Failed to allocate mem for config Descriptor\n"));

            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto ConfigureDevice_Exit;
        }

        siz = configurationDescriptor->wTotalLength;

        ExFreePool(configurationDescriptor);

        configurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)ExAllocatePool(NonPagedPool, siz);

        if(configurationDescriptor) {

            UsbBuildGetDescriptorRequest(
                    urb, 
                    (USHORT)sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                    USB_CONFIGURATION_DESCRIPTOR_TYPE,
                    0, 
                    0, 
                    configurationDescriptor, 
                    NULL, 
                    siz, 
                    NULL);

            ntStatus = CallUSBD(DeviceObject, urb);

            if(!NT_SUCCESS(ntStatus)) {

                BulkUsb_DbgPrint(1,("Failed to read configuration descriptor\n"));
                goto ConfigureDevice_Exit;
            }
        }
        else {

            DBGPRINT(RT_DEBUG_TRACE,("Failed to alloc mem for config Descriptor\n"));
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            goto ConfigureDevice_Exit;
        }
    }
    else {

        DBGPRINT(RT_DEBUG_TRACE,("Failed to allocate memory for urb\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto ConfigureDevice_Exit;
    }

    if(configurationDescriptor) {

        //
        // save a copy of configurationDescriptor in deviceExtension
        // remember to free it later.
        //
        deviceExtension->UsbConfigurationDescriptor = configurationDescriptor;

        /*if(configurationDescriptor->bmAttributes & REMOTE_WAKEUP_MASK)
        {
            //
            // this configuration supports remote wakeup
            //
            deviceExtension->WaitWakeEnable = 1;
        }
        else
        {
            deviceExtension->WaitWakeEnable = 0;
        }*/

        ntStatus = SelectInterfaces(DeviceObject, configurationDescriptor);
    }

ConfigureDevice_Exit:

    if(urb) {

        ExFreePool(urb);
    }

    if(configurationDescriptor &&
       deviceExtension->UsbConfigurationDescriptor == NULL) {

        ExFreePool(configurationDescriptor);
    }

    return ntStatus;
}
NTSTATUS
SelectInterfaces(
    IN PDEVICE_OBJECT                DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    )
/*++
 
Routine Description:

    This helper routine selects the configuration

Arguments:

    DeviceObject - pointer to device object
    ConfigurationDescriptor - pointer to the configuration
    descriptor for the device

Return Value:

    NT status value

--*/
{
    LONG                        numberOfInterfaces, 
                                interfaceNumber, 
                                interfaceindex;
    ULONG                       i;
    PURB                        urb;
//    PUCHAR                      pInf;
    NTSTATUS                    ntStatus = 0;
    RTMP_ADAPTER *deviceExtension;
    PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptor;
    PUSBD_INTERFACE_LIST_ENTRY  interfaceList, 
                                tmp;
    PUSBD_INTERFACE_INFORMATION Interface;
	int TxPipeCnt;
	int RxPipeCnt;
	
    //
    // initialize the variables
    //

    urb = NULL;
    Interface = NULL;
    interfaceDescriptor = NULL;
    deviceExtension = (RTMP_ADAPTER *)DeviceObject->DeviceExtension;
    numberOfInterfaces = ConfigurationDescriptor->bNumInterfaces;
    interfaceindex = interfaceNumber = 0;

    //
    // Parse the configuration descriptor for the interface;
    //

    tmp = interfaceList =
       (PUSBD_INTERFACE_LIST_ENTRY) ExAllocatePool(
               NonPagedPool, 
               sizeof(USBD_INTERFACE_LIST_ENTRY) * (numberOfInterfaces + 1));

    if(!tmp) {

        DBGPRINT(RT_DEBUG_TRACE,("Failed to allocate mem for interfaceList\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }


    while(interfaceNumber < numberOfInterfaces) {

        interfaceDescriptor = USBD_ParseConfigurationDescriptorEx(
                                            ConfigurationDescriptor, 
                                            ConfigurationDescriptor,
                                            interfaceindex,
                                            0, -1, -1, -1);

        if(interfaceDescriptor) {

            interfaceList->InterfaceDescriptor = interfaceDescriptor;
            interfaceList->Interface = NULL;
            interfaceList++;
            interfaceNumber++;
        }

        interfaceindex++;
    }

    interfaceList->InterfaceDescriptor = NULL;
    interfaceList->Interface = NULL;
    urb = USBD_CreateConfigurationRequestEx(ConfigurationDescriptor, tmp);

    if(urb) {

        Interface = &urb->UrbSelectConfiguration.Interface;

        for(i=0; i<Interface->NumberOfPipes; i++) {

            //
            // perform pipe initialization here
            // set the transfer size and any pipe flags we use
            // USBD sets the rest of the Interface struct members
            //

            Interface->Pipes[i].MaximumTransferSize = 
                                USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;
        }

        ntStatus = CallUSBD(DeviceObject, urb);

        if(NT_SUCCESS(ntStatus)) {

            //
            // save a copy of interface information in the device extension.
            //
            deviceExtension->UsbInterface = (PUSBD_INTERFACE_INFORMATION)ExAllocatePool(NonPagedPool,
                                                           Interface->Length);

            if(deviceExtension->UsbInterface) {
                
                RtlCopyMemory(deviceExtension->UsbInterface,
                              Interface,
                              Interface->Length);
            }
            else {

                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                DBGPRINT(RT_DEBUG_TRACE,("memory alloc for UsbInterface failed\n"));
            }

            //
            // Dump the interface to the debugger
            //

            Interface = &urb->UrbSelectConfiguration.Interface;
	     
            BulkUsb_DbgPrint(3, ("---------\n"));
            BulkUsb_DbgPrint(3, ("NumberOfPipes 0x%x\n", 
                                 Interface->NumberOfPipes));
            BulkUsb_DbgPrint(3, ("Length 0x%x\n", 
                                 Interface->Length));
            BulkUsb_DbgPrint(3, ("Alt Setting 0x%x\n", 
                                 Interface->AlternateSetting));
            BulkUsb_DbgPrint(3, ("Interface Number 0x%x\n", 
                                 Interface->InterfaceNumber));
            BulkUsb_DbgPrint(3, ("Class, subclass, protocol 0x%x 0x%x 0x%x\n",
                                 Interface->Class,
                                 Interface->SubClass,
                                 Interface->Protocol));
            //
            // Initialize the PipeContext
            // Dump the pipe info
            //
            TxPipeCnt = 0;
            RxPipeCnt = 0;

            if(Interface->NumberOfPipes) {
                deviceExtension->PipeContext = 
                     (PBULKUSB_PIPE_CONTEXT)ExAllocatePool(NonPagedPool,
                                  Interface->NumberOfPipes *
                                   sizeof(BULKUSB_PIPE_CONTEXT));

                if(deviceExtension->PipeContext) {
                
                    for(i=0; i<Interface->NumberOfPipes; i++) {

                        deviceExtension->PipeContext[i].PipeOpen = FALSE;

			//-->werner++
			DBGPRINT(RT_DEBUG_TRACE,("i = %d\n", i));
			DBGPRINT(RT_DEBUG_TRACE,("InterfaceInfo->Pipes[%d].PipeType = %d\n",i, Interface->Pipes[i].PipeType));
			if ((Interface->Pipes[i].PipeType == UsbdPipeTypeBulk) && (USB_ENDPOINT_DIRECTION_IN(Interface->Pipes[i].EndpointAddress)))
			{
				if(RxPipeCnt < MAX_RX_BULK_PIPE_NUM)
				{
					DBGPRINT(RT_DEBUG_TRACE,("BULK IN EndpointAddress = 0x%02x\n", Interface->Pipes[i].EndpointAddress));
									
					Interface->Pipes[i].MaximumTransferSize = USB_BULK_MAX_TRANSFER_SIZE;				

					deviceExtension->UsbBulkInPipeInfo = (PUSBD_PIPE_INFORMATION)&Interface->Pipes[i];
					deviceExtension->UsbBulkInPipeHandle[RxPipeCnt] = Interface->Pipes[i].PipeHandle;
					deviceExtension->UsbBulkInMaxPacketSize = Interface->Pipes[i].MaximumPacketSize;

					DBGPRINT(RT_DEBUG_TRACE,("BULK IN MaximumPacketSize = %d\n", deviceExtension->UsbBulkInMaxPacketSize));

					RxPipeCnt++;
				}
				else
				{
					DBGPRINT(RT_DEBUG_TRACE,("%s Warnning: Too much bulk pipe!\n",__FUNCTION__));
					continue;
				}
			}
			else if ((Interface->Pipes[i].PipeType == UsbdPipeTypeBulk) && (USB_ENDPOINT_DIRECTION_OUT(Interface->Pipes[i].EndpointAddress)))
			{
				if(TxPipeCnt < MAX_TX_BULK_PIPE_NUM)
				{
					DBGPRINT(RT_DEBUG_TRACE,("BULK OUT EndpointAddress = 0x%02x\n", Interface->Pipes[i].EndpointAddress));
					DBGPRINT(RT_DEBUG_TRACE,("BULK OUT EndpointAddress = 0x%02x\n", Interface->Pipes[i].EndpointAddress));
					DBGPRINT(RT_DEBUG_TRACE,("i = %d, txPipeCnt = %d\n", i, TxPipeCnt));

					Interface->Pipes[i].MaximumTransferSize = USB_BULK_MAX_TRANSFER_SIZE;


					deviceExtension->UsbBulkOutPipeInfo = (PUSBD_PIPE_INFORMATION)&Interface->Pipes[i];
					deviceExtension->UsbBulkOutPipeHandle[TxPipeCnt] = Interface->Pipes[i].PipeHandle;
					deviceExtension->UsbBulkOutMaxPacketSize = Interface->Pipes[i].MaximumPacketSize;
					/*if ( TxPipeCnt == 0 ) // the first is for mgmt
					{
						deviceExtension->UsbBulkOutPipeInfo = (PUSBD_PIPE_INFORMATION)&Interface->Pipes[i];
						deviceExtension->UsbBulkOutPipeHandle[5] = Interface->Pipes[i].PipeHandle;
						deviceExtension->UsbBulkOutMaxPacketSize = Interface->Pipes[i].MaximumPacketSize;
					}
					else
					{
						deviceExtension->UsbBulkOutPipeInfo = (PUSBD_PIPE_INFORMATION)&Interface->Pipes[i];
						deviceExtension->UsbBulkOutPipeHandle[TxPipeCnt-1] = Interface->Pipes[i].PipeHandle;
						deviceExtension->UsbBulkOutMaxPacketSize = Interface->Pipes[i].MaximumPacketSize;
					}*/

					DBGPRINT(RT_DEBUG_TRACE,("BULK OUT MaximumPacketSize = %d\n", deviceExtension->UsbBulkOutMaxPacketSize));

					TxPipeCnt++;
				}
				else
				{
					DBGPRINT(RT_DEBUG_TRACE,("Warnning: Too much bulk pipe!\n"));
					continue;
				}
			}
			//<--werner++						
                    }
                }
                else {
                    
                    ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                    //DBGPRINT(RT_DEBUG_TRACE,("memory alloc for UsbInterface failed\n"));
                }
            }

            for(i=0; i<Interface->NumberOfPipes; i++) {

                DBGPRINT(RT_DEBUG_TRACE, ("---------\n"));
                DBGPRINT(RT_DEBUG_TRACE, ("PipeType 0x%x\n", 
                                     Interface->Pipes[i].PipeType));
                DBGPRINT(RT_DEBUG_TRACE, ("EndpointAddress 0x%x\n", 
                                     Interface->Pipes[i].EndpointAddress));
                DBGPRINT(RT_DEBUG_TRACE, ("MaxPacketSize 0x%x\n", 
                                    Interface->Pipes[i].MaximumPacketSize));
                DBGPRINT(RT_DEBUG_TRACE, ("Interval 0x%x\n", 
                                     Interface->Pipes[i].Interval));
                DBGPRINT(RT_DEBUG_TRACE, ("Handle 0x%p\n", 
                                     Interface->Pipes[i].PipeHandle));
                DBGPRINT(RT_DEBUG_TRACE, ("MaximumTransferSize 0x%x\n", 
                                    Interface->Pipes[i].MaximumTransferSize));
            }

            //BulkUsb_DbgPrint(3, ("---------\n"));
        }
        else {

            DBGPRINT(RT_DEBUG_TRACE,("Failed to select an interface\n"));
        }
    }
    else {
        
        DBGPRINT(RT_DEBUG_TRACE,("USBD_CreateConfigurationRequestEx failed\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if(tmp) {

        ExFreePool(tmp);
    }

    if(urb) {

        ExFreePool(urb);
    }

    return ntStatus;
}

NTSTATUS
HandleQueryStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    This routine services the Irps of minor type IRP_MN_QUERY_STOP_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    NTSTATUS          ntStatus;
    RTMP_ADAPTER *deviceExtension;

    //BulkUsb_DbgPrint(3, ("HandleQueryStopDevice - begins\n"));

    //
    // initialize variables
    //

    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

    //
    // If we can stop the device, we need to set the QueueState to 
    // HoldRequests so further requests will be queued.
    //

    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);
    
    SET_NEW_PNP_STATE(deviceExtension, USBPendingStop);
    deviceExtension->QueueState = HoldRequests;
    
    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

    //
    // wait for the existing ones to be finished.
    // first, decrement this operation
    //

    //BulkUsb_DbgPrint(3, ("HandleQueryStopDevice::"));
    BulkUsb_IoDecrement(deviceExtension);

    KeWaitForSingleObject(&deviceExtension->StopEvent, 
                          Executive, 
                          KernelMode, 
                          FALSE, 
                          NULL);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoSkipCurrentIrpStackLocation(Irp);

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    //BulkUsb_DbgPrint(3, ("HandleQueryStopDevice - ends\n"));

    return ntStatus;
}

NTSTATUS
HandleCancelStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    This routine services Irp of minor type IRP_MN_CANCEL_STOP_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT value

--*/
{
    KIRQL             oldIrql;    
    KEVENT            event;
    NTSTATUS          ntStatus;
    RTMP_ADAPTER *deviceExtension;

    //BulkUsb_DbgPrint(3, ("HandleCancelStopDevice - begins\n"));

    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

    // 
    // First check to see whether you have received cancel-stop
    // without first receiving a query-stop. This could happen if someone
    // above us fails a query-stop and passes down the subsequent
    // cancel-stop.
    //

    if(USBPendingStop != deviceExtension->DeviceState) {
        // spurious Irp
        //
        // If the device is already in an active state when the driver
        // receives this IRP, a function driver simply sets status to
        // success and passes the IRP to the next driver.
        //
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
    }

    //
    // Send this IRP down and wait for it to come back.
    // Set the QueueState flag to AllowRequests, 
    // and process all the previously queued up IRPs.
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoSetCompletionRoutine(Irp, 
                           IrpCompletionRoutine, 
                           (PVOID)&event, 
                           TRUE, 
                           TRUE, 
                           TRUE);

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    if(ntStatus == STATUS_PENDING) {

        KeWaitForSingleObject(&event, 
                              Executive, 
                              KernelMode, 
                              FALSE, 
                              NULL);
        ntStatus = Irp->IoStatus.Status;
    }



    if(NT_SUCCESS(ntStatus) &&
       USBPendingStop == deviceExtension->DeviceState) {

        KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);

        RESTORE_PREVIOUS_PNP_STATE(deviceExtension);
        deviceExtension->QueueState = AllowRequests;
        ASSERT(deviceExtension->DeviceState == USBWorking);

        KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

        ProcessQueuedRequests(deviceExtension);
    }

    //
    // Complete the IRP that our completion routine pended
    //
    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    //BulkUsb_DbgPrint(3, ("HandleCancelStopDevice - ends\n"));

    return ntStatus;
}

VOID
ProcessQueuedRequests(
    IN OUT RTMP_ADAPTER *DeviceExtension
    )
/*++
 
Routine Description:

    Remove and process the entries in the queue. If this routine is called
    when processing IRP_MN_CANCEL_STOP_DEVICE, IRP_MN_CANCEL_REMOVE_DEVICE
    or IRP_MN_START_DEVICE, the requests are passed to the next lower driver.
    If the routine is called when IRP_MN_REMOVE_DEVICE is received, the IRPs
    are complete with STATUS_DELETE_PENDING

Arguments:

    DeviceExtension - pointer to device extension

Return Value:

    None

--*/
{
    KIRQL       oldIrql;
    PIRP        nextIrp,
                cancelledIrp;
    PDRIVER_CANCEL       cancelRoutine;
    LIST_ENTRY  cancelledIrpList;
    PLIST_ENTRY listEntry;

    //BulkUsb_DbgPrint(3, ("ProcessQueuedRequests - begins\n"));

    //
    // initialize variables
    //

    cancelRoutine = NULL;
    InitializeListHead(&cancelledIrpList);

    //
    // 1.  dequeue the entries in the queue
    // 2.  reset the cancel routine
    // 3.  process them
    // 3a. if the device is active, send them down
    // 3b. else complete with STATUS_DELETE_PENDING
    //

    while(1) {

        KeAcquireSpinLock(&DeviceExtension->QueueLock, &oldIrql);

        if(IsListEmpty(&DeviceExtension->NewRequestsQueue)) {

            KeReleaseSpinLock(&DeviceExtension->QueueLock, oldIrql);
            break;
        }
    
        //
        // Remove a request from the queue
        //

        listEntry = RemoveHeadList(&DeviceExtension->NewRequestsQueue);
        nextIrp = CONTAINING_RECORD(listEntry, IRP, Tail.Overlay.ListEntry);

        //
        // set the cancel routine to NULL
        //

        cancelRoutine = IoSetCancelRoutine(nextIrp, NULL);

        //
        // check if its already cancelled
        //

        if(nextIrp->Cancel) {
            if(cancelRoutine) {

                //
                // the cancel routine for this IRP hasnt been called yet
                // so queue the IRP in the cancelledIrp list and complete
                // after releasing the lock
                //
                
                InsertTailList(&cancelledIrpList, listEntry);
            }
            else {

                //
                // the cancel routine has run
                // it must be waiting to hold the queue lock
                // so initialize the IRPs listEntry
                //

                InitializeListHead(listEntry);
            }

            KeReleaseSpinLock(&DeviceExtension->QueueLock, oldIrql);
        }
        else {

            KeReleaseSpinLock(&DeviceExtension->QueueLock, oldIrql);

            if(FailRequests == DeviceExtension->QueueState) {

                nextIrp->IoStatus.Information = 0;
                nextIrp->IoStatus.Status = STATUS_DELETE_PENDING;
                IoCompleteRequest(nextIrp, IO_NO_INCREMENT);
            }
            else {

                //PIO_STACK_LOCATION irpStack;

                //BulkUsb_DbgPrint(3, ("ProcessQueuedRequests::"));
                BulkUsb_IoIncrement(DeviceExtension);

                IoSkipCurrentIrpStackLocation(nextIrp);
                IoCallDriver(DeviceExtension->TopOfStackDeviceObject, nextIrp);
               
                //BulkUsb_DbgPrint(3, ("ProcessQueuedRequests::"));
                BulkUsb_IoDecrement(DeviceExtension);
            }
        }
    } // while loop

    //
    // walk through the cancelledIrp list and cancel them
    //

    while(!IsListEmpty(&cancelledIrpList)) {

        PLIST_ENTRY cancelEntry = RemoveHeadList(&cancelledIrpList);
        
        cancelledIrp = CONTAINING_RECORD(cancelEntry, IRP, Tail.Overlay.ListEntry);

        cancelledIrp->IoStatus.Status = STATUS_CANCELLED;
        cancelledIrp->IoStatus.Information = 0;

        IoCompleteRequest(cancelledIrp, IO_NO_INCREMENT);
    }

    //BulkUsb_DbgPrint(3, ("ProcessQueuedRequests - ends\n"));

    return;
}

NTSTATUS
HandleQueryRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    This routine services Irp of minor type IRP_MN_QUERY_REMOVE_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    NTSTATUS          ntStatus;
    RTMP_ADAPTER *deviceExtension;

    //BulkUsb_DbgPrint(3, ("HandleQueryRemoveDevice - begins\n"));

    //
    // initialize variables
    //

    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

    //
    // If we can allow removal of the device, we should set the QueueState
    // to HoldRequests so further requests will be queued. This is required
    // so that we can process queued up requests in cancel-remove just in 
    // case somebody else in the stack fails the query-remove. 
    // 

    ntStatus = CanRemoveDevice(DeviceObject, Irp);

    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);

    deviceExtension->QueueState = HoldRequests;
    SET_NEW_PNP_STATE(deviceExtension, USBPendingRemove);

    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

    //BulkUsb_DbgPrint(3, ("HandleQueryRemoveDevice::"));
    BulkUsb_IoDecrement(deviceExtension);

    //
    // wait for all the requests to be completed
    //

    KeWaitForSingleObject(&deviceExtension->StopEvent, 
                          Executive,
                          KernelMode, 
                          FALSE, 
                          NULL);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoSkipCurrentIrpStackLocation(Irp);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    //BulkUsb_DbgPrint(3, ("HandleQueryRemoveDevice - ends\n"));

    return ntStatus;

}

NTSTATUS
CanRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    This routine determines whether the device can be safely removed. In our 
    particular case, we'll assume we can always remove the device.
    A device shouldn't be removed if, for example, it has open handles or
    removing the device could result in losing data (plus the reasons 
    mentioned at CanStopDevice). The PnP manager on Windows 2000 fails 
    on its own any attempt to remove, if there any open handles to the device. 
    However on Win9x, the driver must keep count of open handles and fail 
    query_remove if there are any open handles.

Arguments:

    DeviceObject - pointer to the device object.
    
    Irp - pointer to the current IRP.
    
Return Value:

    STATUS_SUCCESS if the device can be safely removed, an appropriate 
    NT Status if not.

--*/
{
   //
   // We assume we can remove the device
   //

   UNREFERENCED_PARAMETER(DeviceObject);
   UNREFERENCED_PARAMETER(Irp);

   return STATUS_SUCCESS;
}

NTSTATUS
HandleCancelRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    This routine services Irp of minor type IRP_MN_CANCEL_REMOVE_DEVICE

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    KEVENT            event;
    NTSTATUS          ntStatus;
    RTMP_ADAPTER *deviceExtension;

    //BulkUsb_DbgPrint(3, ("HandleCancelRemoveDevice - begins\n"));

    //
    // initialize variables
    //

    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

    //
    // We need to reset the QueueState flag to ProcessRequest, 
    // since the device resume its normal activities.
    //

    //
    // First check to see whether you have received cancel-remove
    // without first receiving a query-remove. This could happen if 
    // someone above us fails a query-remove and passes down the 
    // subsequent cancel-remove.
    //

    if(USBPendingRemove == deviceExtension->DeviceState) {

        KeInitializeEvent(&event, NotificationEvent, FALSE);

        IoCopyCurrentIrpStackLocationToNext(Irp);
        IoSetCompletionRoutine(Irp, 
                               IrpCompletionRoutine, 
                               (PVOID)&event, 
                               TRUE, 
                               TRUE, 
                               TRUE);
        ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

        if(ntStatus == STATUS_PENDING) {

            KeWaitForSingleObject(&event, 
                                  Executive, 
                                  KernelMode, 
                                  FALSE, 
                                  NULL);

            ntStatus = Irp->IoStatus.Status;
        }

        if(NT_SUCCESS(ntStatus)) {

            KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);

            deviceExtension->QueueState = AllowRequests;
            RESTORE_PREVIOUS_PNP_STATE(deviceExtension);

            KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);
            //
            // process the queued requests that arrive between 
            // QUERY_REMOVE and CANCEL_REMOVE
            //
            
            ProcessQueuedRequests(deviceExtension);
            
        }
        //
        // Complete the IRP that our completion routine pended
        //
        Irp->IoStatus.Status = ntStatus;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

    }
    else {

        // 
        // spurious cancel-remove
        //
        // If the device is already started when the driver receives
        // this IRP, the driver simply sets status to success and passes
        // the IRP to the next driver.  For such a cancel-remove IRP, a
        // function driver need not set a completion routine. The device
        // may not be in the remove-pending state, because, for example,
        // the driver failed the previous IRP_MN_QUERY_REMOVE_DEVICE.  
        //
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;

        IoSkipCurrentIrpStackLocation(Irp);
        ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);
    }

    //BulkUsb_DbgPrint(3, ("HandleCancelRemoveDevice - ends\n"));

    return ntStatus;
}

NTSTATUS
HandleSurpriseRemoval(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    This routine services Irp of minor type IRP_MN_SURPRISE_REMOVAL

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value

--*/
{
    KIRQL             oldIrql;
    NTSTATUS          ntStatus;
    RTMP_ADAPTER *deviceExtension;

    //BulkUsb_DbgPrint(3, ("HandleSurpriseRemoval - begins\n"));

    //
    // initialize variables
    //

    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

    //
    // 1. fail pending requests
    // 2. return device and memory resources
    // 3. disable interfaces
    //

    /*if(deviceExtension->WaitWakeEnable) {
    
        CancelWaitWake(deviceExtension);
    }*/


    if(WinXpOrBetter == deviceExtension->WdmVersion) {

        /*if(deviceExtension->SSEnable) {

            //
            // Cancel the timer so that the DPCs are no longer fired.
            // we do not need DPCs because the device has been surprise
            // removed
            //  
        
            KeCancelTimer(&deviceExtension->Timer);

            deviceExtension->SSEnable = 0;

            //  
            // make sure that if a DPC was fired before we called cancel timer,
            // then the DPC and work-time have run to their completion
            //
            KeWaitForSingleObject(&deviceExtension->NoDpcWorkItemPendingEvent, 
                                  Executive, 
                                  KernelMode, 
                                  FALSE, 
                                  NULL);

            //
            // make sure that the selective suspend request has been completed.
            //
            KeWaitForSingleObject(&deviceExtension->NoIdleReqPendEvent, 
                                  Executive, 
                                  KernelMode, 
                                  FALSE, 
                                  NULL);
        }*/
    }

    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);

    deviceExtension->QueueState = FailRequests;
    SET_NEW_PNP_STATE(deviceExtension, USBSurpriseRemoved);

    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

    ProcessQueuedRequests(deviceExtension);

    ntStatus = IoSetDeviceInterfaceState(&deviceExtension->InterfaceName, 
                                         FALSE);

    if(!NT_SUCCESS(ntStatus)) {

        //DBGPRINT(RT_DEBUG_TRACE,("IoSetDeviceInterfaceState::disable:failed\n"));
    }

    RtlFreeUnicodeString(&deviceExtension->InterfaceName);

   // BulkUsb_WmiDeRegistration(deviceExtension);

    BulkUsb_AbortPipes(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoSkipCurrentIrpStackLocation(Irp);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    //BulkUsb_DbgPrint(3, ("HandleSurpriseRemoval - ends\n"));

    return ntStatus;

}

NTSTATUS
BulkUsb_AbortPipes(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++
 
Routine Description

    sends an abort pipe request for open pipes.

Arguments:

    DeviceObject - pointer to device object

Return Value:

    NT status value

--*/
{
    PURB                        urb;
    ULONG                       i;
    NTSTATUS                    ntStatus;
    RTMP_ADAPTER *deviceExtension;
    PBULKUSB_PIPE_CONTEXT       pipeContext;
//    PUSBD_PIPE_INFORMATION      pipeInformation;
    PUSBD_INTERFACE_INFORMATION interfaceInfo;

    //
    // initialize variables
    //
    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;
    pipeContext = deviceExtension->PipeContext;
    interfaceInfo = deviceExtension->UsbInterface;
    
    //BulkUsb_DbgPrint(3, ("BulkUsb_AbortPipes - begins\n"));
    
    if(interfaceInfo == NULL || pipeContext == NULL) {

        return STATUS_SUCCESS;
    }

    for(i=0; i<interfaceInfo->NumberOfPipes; i++) {

        if(pipeContext[i].PipeOpen) {

            //BulkUsb_DbgPrint(3, ("Aborting open pipe %d\n", i));
    
            urb = (PURB)ExAllocatePool(NonPagedPool,
                                 sizeof(struct _URB_PIPE_REQUEST));

            if(urb) {

                urb->UrbHeader.Length = sizeof(struct _URB_PIPE_REQUEST);
                urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
                urb->UrbPipeRequest.PipeHandle = 
                                        interfaceInfo->Pipes[i].PipeHandle;

                ntStatus = CallUSBD(DeviceObject, urb);

                ExFreePool(urb);
            }
            else {

                //DBGPRINT(RT_DEBUG_TRACE,("Failed to alloc memory for urb\n"));

                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                return ntStatus;
            }

            if(NT_SUCCESS(ntStatus)) {

                pipeContext[i].PipeOpen = FALSE;
            }
        }
    }

    //BulkUsb_DbgPrint(3, ("BulkUsb_AbortPipes - ends\n"));

    return STATUS_SUCCESS;
}


VOID
IdleRequestWorkerRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID          Context
    )
/*++
 
Routine Description:

    This is the work item fired from the DPC.
    This workitem checks the idle state of the device
    and submits an idle request.

Arguments:

    DeviceObject - pointer to device object
    Context - context for the work item.

Return Value:

    None

--*/
{
//    PIRP                   irp;
    NTSTATUS               ntStatus;
    RTMP_ADAPTER *deviceExtension;
    PIO_WORKITEM           workItem;

    //BulkUsb_DbgPrint(3, ("IdleRequestWorkerRoutine - begins\n"));

    //
    // initialize variables
    //
    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;
    workItem = (PIO_WORKITEM) Context;

    if(CanDeviceSuspend(deviceExtension)) {

        //BulkUsb_DbgPrint(3, ("Device is idle\n"));

        ntStatus = SubmitIdleRequestIrp(deviceExtension);

        if(!NT_SUCCESS(ntStatus)) {

            //DBGPRINT(RT_DEBUG_TRACE,("SubmitIdleRequestIrp failed\n"));
        }
    }
    else {

        //BulkUsb_DbgPrint(3, ("Device is not idle\n"));
    }

    IoFreeWorkItem(workItem);

    //
    // signal the NoDpcWorkItemPendingEvent.
    //
    KeSetEvent(&deviceExtension->NoDpcWorkItemPendingEvent,
               IO_NO_INCREMENT,
               FALSE);

    //BulkUsb_DbgPrint(3, ("IdleRequestsWorkerRoutine - ends\n"));
}


NTSTATUS
HandleQueryCapabilities(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    This routine services Irp of minor type IRP_MN_QUERY_CAPABILITIES

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager.

Return Value:

    NT status value  

--*/
{
    ULONG                i;
    KEVENT               event;
    NTSTATUS             ntStatus;
    RTMP_ADAPTER *deviceExtension;
    PDEVICE_CAPABILITIES pdc;
    PIO_STACK_LOCATION   irpStack;

    BulkUsb_DbgPrint(3, ("HandleQueryCapabilities - begins\n"));

    //
    // initialize variables
    //

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;
    pdc = irpStack->Parameters.DeviceCapabilities.Capabilities;

    //
    // We will provide here an example of an IRP that is processed
    // both on its way down and on its way up: there might be no need for
    // a function driver process this Irp (the bus driver will do that).
    // The driver will wait for the lower drivers (the bus driver among 
    // them) to process this IRP, then it processes it again.
    //

    if(pdc->Version < 1 || pdc->Size < sizeof(DEVICE_CAPABILITIES)) {
        
        DBGPRINT(RT_DEBUG_TRACE,("HandleQueryCapabilities::request failed\n"));
        ntStatus = STATUS_UNSUCCESSFUL;
        return ntStatus;
    }

    //
    // Add in the SurpriseRemovalOK bit before passing it down.
    //
    pdc->SurpriseRemovalOK = TRUE;
    Irp->IoStatus.Status = STATUS_SUCCESS;

    KeInitializeEvent(&event, NotificationEvent, FALSE);
        
    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoSetCompletionRoutine(Irp, 
                           IrpCompletionRoutine, 
                           (PVOID)&event, 
                           TRUE, 
                           TRUE, 
                           TRUE);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

    if(ntStatus == STATUS_PENDING) {

        KeWaitForSingleObject(&event, 
                              Executive, 
                              KernelMode, 
                              FALSE, 
                              NULL);
        ntStatus = Irp->IoStatus.Status;
    }

    //
    // initialize PowerDownLevel to disabled
    //

    deviceExtension->PowerDownLevel = PowerDeviceUnspecified;

    if(NT_SUCCESS(ntStatus)) {

        deviceExtension->DeviceCapabilities = *pdc;
       
        for(i = PowerSystemSleeping1; i <= PowerSystemSleeping3; i++) {

            if(deviceExtension->DeviceCapabilities.DeviceState[i] < 
                                                            PowerDeviceD3) {

                deviceExtension->PowerDownLevel = 
                    deviceExtension->DeviceCapabilities.DeviceState[i];
            }
        }

        //
        // since its safe to surprise-remove this device, we shall
        // set the SurpriseRemoveOK flag to supress any dialog to 
        // user.
        //

        pdc->SurpriseRemovalOK = 1;
    }

    if(deviceExtension->PowerDownLevel == PowerDeviceUnspecified ||
       deviceExtension->PowerDownLevel <= PowerDeviceD0) {
    
        deviceExtension->PowerDownLevel = PowerDeviceD2;
    }

    BulkUsb_DbgPrint(3, ("HandleQueryCapabilities - ends\n"));

    return ntStatus;
}


NTSTATUS
BulkUsb_DispatchClean(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++
 
Routine Description:

    Dispatch routine for IRP_MJ_CLEANUP

Arguments:

    DeviceObject - pointer to device object
    Irp - I/O request packet sent by the pnp manager

Return Value:

    NT status value

--*/
{
    RTMP_ADAPTER *deviceExtension;
    KIRQL              oldIrql;
    LIST_ENTRY         cleanupList;
    PLIST_ENTRY        thisEntry, 
                       nextEntry, 
                       listHead;
    PIRP               pendingIrp;
    PIO_STACK_LOCATION pendingIrpStack, 
                       irpStack;
//    NTSTATUS           ntStatus;

    //
    // initialize variables
    //

    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    InitializeListHead(&cleanupList);

    BulkUsb_DbgPrint(3, ("BulkUsb_DispatchClean::"));
    BulkUsb_IoIncrement(deviceExtension);

    //
    // acquire queue lock
    //
    KeAcquireSpinLock(&deviceExtension->QueueLock, &oldIrql);

    //
    // remove all Irp's that belong to input Irp's fileobject
    //

    listHead = &deviceExtension->NewRequestsQueue;

    for(thisEntry = listHead->Flink, nextEntry = thisEntry->Flink;
       thisEntry != listHead;
       thisEntry = nextEntry, nextEntry = thisEntry->Flink) {

        pendingIrp = CONTAINING_RECORD(thisEntry, IRP, Tail.Overlay.ListEntry);

        pendingIrpStack = IoGetCurrentIrpStackLocation(pendingIrp);

        if(irpStack->FileObject == pendingIrpStack->FileObject) {

            RemoveEntryList(thisEntry);

            //
            // set the cancel routine to NULL
            //
            if(NULL == IoSetCancelRoutine(pendingIrp, NULL)) {

                InitializeListHead(thisEntry);
            }
            else {

                InsertTailList(&cleanupList, thisEntry);
            }
        }
    }

    //
    // Release the spin lock
    //

    KeReleaseSpinLock(&deviceExtension->QueueLock, oldIrql);

    //
    // walk thru the cleanup list and cancel all the Irps
    //

    while(!IsListEmpty(&cleanupList)) {

        //
        // complete the Irp
        //
        thisEntry = RemoveHeadList(&cleanupList);

        pendingIrp = CONTAINING_RECORD(thisEntry, IRP, Tail.Overlay.ListEntry);

        pendingIrp->IoStatus.Information = 0;
        pendingIrp->IoStatus.Status = STATUS_CANCELLED;

        IoCompleteRequest(pendingIrp, IO_NO_INCREMENT);
    }

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    BulkUsb_DbgPrint(3, ("BulkUsb_DispatchClean::"));
    BulkUsb_IoDecrement(deviceExtension);

    return STATUS_SUCCESS;
}

NTSTATUS
ReleaseMemory(
    IN PDEVICE_OBJECT DeviceObject
    )
/*++
 
Routine Description:

    This routine returns all the memory allocations acquired during
    device startup. 
    
Arguments:

    DeviceObject - pointer to the device object.
        
    
Return Value:

    STATUS_SUCCESS if the device can be safely removed, an appropriate 
    NT Status if not.

--*/
{
    //
    // Disconnect from the interrupt and unmap any I/O ports
    //
    
    RTMP_ADAPTER *deviceExtension;

    deviceExtension = (RTMP_ADAPTER *)DeviceObject->DeviceExtension;

    if(deviceExtension->UsbConfigurationDescriptor) {

        ExFreePool(deviceExtension->UsbConfigurationDescriptor);
        deviceExtension->UsbConfigurationDescriptor = NULL;
    }

    if(deviceExtension->UsbInterface) {
        
        ExFreePool(deviceExtension->UsbInterface);
        deviceExtension->UsbInterface = NULL;
    }

    if(deviceExtension->PipeContext) {

        ExFreePool(deviceExtension->PipeContext);
        deviceExtension->PipeContext = NULL;
    }

    return STATUS_SUCCESS;
}

