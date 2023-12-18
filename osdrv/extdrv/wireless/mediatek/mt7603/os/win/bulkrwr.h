/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    bulkrwr.h

Abstract:

Environment:

    Kernel mode

Notes:

    Copyright (c) 2000 Microsoft Corporation.  
    All Rights Reserved.

--*/
#ifndef _BULKUSB_RWR_H
#define _BULKUSB_RWR_H

typedef struct _BULKUSB_RW_CONTEXT {

    PURB              Urb;
    PMDL              Mdl;
    ULONG             Length;         // remaining to xfer
    ULONG             Numxfer;        // cumulate xfer
    ULONG_PTR         VirtualAddress; // va for next segment of xfer.
    RTMP_ADAPTER *DeviceExtension;

} BULKUSB_RW_CONTEXT, * PBULKUSB_RW_CONTEXT;
IO_COMPLETION_ROUTINE BulkUsb_ReadWriteCompletion;
DRIVER_DISPATCH BulkUsb_DispatchReadWrite;

PBULKUSB_PIPE_CONTEXT
BulkUsb_PipeWithName(
    IN PDEVICE_OBJECT  DeviceObject,
    IN PUNICODE_STRING FileName
    );


VOID
WaitWakeCallback( 
    IN PDEVICE_OBJECT   DeviceObject,
    IN UCHAR            MinorFunction,
    IN POWER_STATE      PowerState,
    IN PVOID            Context,
    IN PIO_STATUS_BLOCK IoStatus
    );

VOID
WWIrpCompletionFunc(
    IN PDEVICE_OBJECT   DeviceObject,
    IN UCHAR            MinorFunction,
    IN POWER_STATE      PowerState,
    IN PVOID            Context,
    IN PIO_STATUS_BLOCK IoStatus
    );

#endif

