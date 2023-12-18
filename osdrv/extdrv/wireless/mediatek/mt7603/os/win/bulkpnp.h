/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    bulkpnp.h

Abstract:

Environment:

    Kernel mode

Notes:

    Copyright (c) 2000 Microsoft Corporation.  
    All Rights Reserved.

--*/

#ifndef _BULKUSB_PNP_H
#define _BULKUSB_PNP_H


#define REMOTE_WAKEUP_MASK 0x20

//PIO_COMPLETION_ROUTINE IrpCompletionRoutine;
DRIVER_DISPATCH BulkUsb_DispatchClean;
NTSTATUS
IrpCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    );

NTSTATUS
SelectInterfaces(
	IN PDEVICE_OBJECT                DeviceObject,
	IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    );

NTSTATUS
HandleQueryStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleCancelStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

VOID
ProcessQueuedRequests(
    IN OUT RTMP_ADAPTER *DeviceExtension
    );

NTSTATUS
HandleQueryRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleCancelRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
HandleSurpriseRemoval(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
BulkUsb_AbortPipes(
    IN PDEVICE_OBJECT DeviceObject
    );


NTSTATUS
HandleQueryCapabilities(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

//PIO_COMPLETION_ROUTINE IrpCompletionRoutine;
/*#define REMOTE_WAKEUP_MASK 0x20

IO_COMPLETION_ROUTINE IrpCompletionRoutine;
IO_WORKITEM_ROUTINE IdleRequestWorkerRoutine;
DRIVER_DISPATCH BulkUsb_DispatchPnP;
DRIVER_DISPATCH BulkUsb_DispatchClean;

NTSTATUS
HandleStartDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );








NTSTATUS
HandleRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );



NTSTATUS
HandleStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );



NTSTATUS
DeconfigureDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB           Urb
    );



NTSTATUS
BulkUsb_GetRegistryDword(
    IN     PWCHAR RegPath,
    IN     PWCHAR ValueName,
    IN OUT PULONG Value
    );

VOID
DpcRoutine(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );










LONG
BulkUsb_IoDecrement(
    IN OUT RTMP_ADAPTER *DeviceExtension
    );
*/

NTSTATUS
ReleaseMemory(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
CanRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

NTSTATUS
CanStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

LONG
BulkUsb_IoIncrement(
    IN OUT RTMP_ADAPTER *DeviceExtension
    );

BOOLEAN
CanDeviceSuspend(
    IN RTMP_ADAPTER *DeviceExtension
    );
NTSTATUS
ConfigureDevice(
	IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB           Urb
    );
LONG
BulkUsb_IoDecrement(
    IN OUT RTMP_ADAPTER *DeviceExtension
    );
NTSTATUS
ReadandSelectDescriptors(
    IN PDEVICE_OBJECT DeviceObject
    );
/*PCHAR
PnPMinorFunctionString (
    IN UCHAR MinorFunction
    );
*/
#endif

