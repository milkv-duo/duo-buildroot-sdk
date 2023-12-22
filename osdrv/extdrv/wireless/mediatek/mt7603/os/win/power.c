
#include "config.h"

/////////////////////////////////////////////////////////////////////////////

NTSTATUS PowerSetPower( IN RTMP_ADAPTER *dx, IN PIRP Irp);
NTSTATUS DefaultPowerHandler( IN RTMP_ADAPTER *dx, IN PIRP Irp);
NTSTATUS OnCompleteIncreaseSystemPower( IN PDEVICE_OBJECT fdo, IN PIRP Irp, IN PVOID context);
NTSTATUS OnCompleteIncreaseDevicePower( IN PDEVICE_OBJECT fdo, IN PIRP Irp, IN PVOID context);
VOID OnCompleteDeviceSetPower( IN PDEVICE_OBJECT fdo, IN UCHAR MinorFunction,
					IN POWER_STATE PowerState, IN PVOID Context, IN PIO_STATUS_BLOCK IoStatus);

NTSTATUS SendDeviceSetPower( IN RTMP_ADAPTER *dx, IN DEVICE_POWER_STATE NewDevicePowerState);
NTSTATUS CompleteIrp( IN PIRP Irp, IN NTSTATUS status, IN ULONG info);
void SetPowerState( IN RTMP_ADAPTER *dx, IN DEVICE_POWER_STATE NewDevicePowerState);
/////////////////////////////////////////////////////////////////////////////
//	Wdm2Power:
//
//	Description:
//		Handle IRP_MJ_POWER requests
//
//	Arguments:
//		Pointer to the FDO
//		Pointer to the IRP
//			IRP_MN_WAIT_WAKE:		IrpStack->Parameters.WaitWake.Xxx
//			IRP_MN_POWER_SEQUENCE:	IrpStack->Parameters.PowerSequence.Xxx
//			IRP_MN_SET_POWER:
//			IRP_MN_QUERY_POWER:		IrpStack->Parameters.Power.Xxx
//
//	Return Value:
//		This function returns STATUS_XXX

NTSTATUS Wdm2Power(	IN PDEVICE_OBJECT fdo,
					IN PIRP Irp)
{
#ifdef _USB
	NTSTATUS           ntStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpStack;
//  PUNICODE_STRING    tagString;
    RTMP_ADAPTER *deviceExtension;

    //
    // initialize the variables
    //

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    deviceExtension = (RTMP_ADAPTER *)fdo->DeviceExtension;
    DBGPRINT(RT_DEBUG_TRACE,("USB: Wdm2Power==>\n"));
    //
    // We don't queue power Irps, we'll only check if the
    // device was removed, otherwise we'll take appropriate
    // action and send it to the next lower driver. In general
    // drivers should not cause long delays while handling power
    // IRPs. If a driver cannot handle a power IRP in a brief time,
    // it should return STATUS_PENDING and queue all incoming
    // IRPs until the IRP completes.
    //

    if(USBRemoved == deviceExtension->DeviceState) {

        //
        // Even if a driver fails the IRP, it must nevertheless call
        // PoStartNextPowerIrp to inform the Power Manager that it
        // is ready to handle another power IRP.
        //

        PoStartNextPowerIrp(Irp);

        Irp->IoStatus.Status = ntStatus = STATUS_DELETE_PENDING;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return ntStatus;
    }

    if(USBNotStarted == deviceExtension->DeviceState) {

        //
        // if the device is not started yet, pass it down
        //

        PoStartNextPowerIrp(Irp);

        IoSkipCurrentIrpStackLocation(Irp);

        return PoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);
    }

    DBGPRINT(RT_DEBUG_TRACE, ("BulkUsb_DispatchPower::"));
    BulkUsb_IoIncrement(deviceExtension);
    
    switch(irpStack->MinorFunction) {
    
    case IRP_MN_SET_POWER:

        //
        // The Power Manager sends this IRP for one of the
        // following reasons:
        // 1) To notify drivers of a change to the system power state.
        // 2) To change the power state of a device for which
        //    the Power Manager is performing idle detection.
        // A driver sends IRP_MN_SET_POWER to change the power
        // state of its device if it's a power policy owner for the
        // device.
        //

        IoMarkIrpPending(Irp);

        switch(irpStack->Parameters.Power.Type) {

        case SystemPowerState:

            HandleSystemSetPower(fdo, Irp);

//            ntStatus = STATUS_PENDING;

            break;

        case DevicePowerState:

            HandleDeviceSetPower(fdo, Irp);

//            ntStatus = STATUS_PENDING;

            break;
        }

        return STATUS_PENDING;

    case IRP_MN_QUERY_POWER:

        //
        // The Power Manager sends a power IRP with the minor
        // IRP code IRP_MN_QUERY_POWER to determine whether it
        // can safely change to the specified system power state
        // (S1-S5) and to allow drivers to prepare for such a change.
        // If a driver can put its device in the requested state,
        // it sets status to STATUS_SUCCESS and passes the IRP down.
        //

        IoMarkIrpPending(Irp);
    
        switch(irpStack->Parameters.Power.Type) {

        case SystemPowerState:
            
            HandleSystemQueryPower(fdo, Irp);

//          ntStatus = STATUS_PENDING;

            break;

        case DevicePowerState:

            HandleDeviceQueryPower(fdo, Irp);

//          ntStatus = STATUS_PENDING;

            break;
        }
        return STATUS_PENDING;

    case IRP_MN_WAIT_WAKE:

        //
        // The minor power IRP code IRP_MN_WAIT_WAKE provides
        // for waking a device or waking the system. Drivers
        // of devices that can wake themselves or the system
        // send IRP_MN_WAIT_WAKE. The system sends IRP_MN_WAIT_WAKE
        // only to devices that always wake the system, such as
        // the power-on switch.
        //

        (PIRP) InterlockedExchangePointer( (PVOID*)&deviceExtension->WaitWakeIrp,//x64 ++ (PVOID)
                                          Irp);

        if(InterlockedExchange(&deviceExtension->FlagWWDispatched, 1)){
            //
            // CancelWaitWake ran before we could store the IRP.  We must
            // cancel the IRP here.
            //

            if(InterlockedExchangePointer((PVOID*)&deviceExtension->WaitWakeIrp,//x64 ++ (PVOID)
                                          NULL)){
                //
                // CancelWaitWake cannot touch this irp now and we will complete it
                //
                PoStartNextPowerIrp(Irp);
                Irp->IoStatus.Status = STATUS_CANCELLED;
                Irp->IoStatus.Information = 0;

                IoCompleteRequest(Irp, IO_NO_INCREMENT);
                ntStatus = STATUS_CANCELLED;

                DBGPRINT(RT_DEBUG_TRACE, ("IRP_MN_WAIT_WAKE::"));
                BulkUsb_IoDecrement(deviceExtension);
                break;
            }
        }

        IoMarkIrpPending(Irp);

        IoCopyCurrentIrpStackLocationToNext(Irp);

        IoSetCompletionRoutine(
                        Irp,
                        WaitWakeCompletionRoutine,
                        deviceExtension, 
                        TRUE, 
                        TRUE, 
                        TRUE);

        PoStartNextPowerIrp(Irp);

        ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

        if(!NT_SUCCESS(ntStatus)) {

            BulkUsb_DbgPrint(1, ("Lower drivers failed the wait-wake Irp\n"));
        }

//        ntStatus = STATUS_PENDING;

        //
        // push back the count HERE and NOT in completion routine
        // a pending Wait Wake Irp should not impede stopping the device
        //

        DBGPRINT(RT_DEBUG_TRACE, ("IRP_MN_WAIT_WAKE::"));
        BulkUsb_IoDecrement(deviceExtension);
        
        return STATUS_PENDING;

    case IRP_MN_POWER_SEQUENCE:

        //
        // A driver sends this IRP as an optimization to determine
        // whether its device actually entered a specific power state.
        // This IRP is optional. Power Manager cannot send this IRP.
        //

    default:

        PoStartNextPowerIrp(Irp);

        IoSkipCurrentIrpStackLocation(Irp);

        ntStatus = PoCallDriver(deviceExtension->TopOfStackDeviceObject, Irp);

        if(!NT_SUCCESS(ntStatus)) {

            BulkUsb_DbgPrint(1, ("Lower drivers failed default power Irp\n"));
        }
        
        DBGPRINT(RT_DEBUG_TRACE, ("BulkUsb_DispatchPower::"));
        BulkUsb_IoDecrement(deviceExtension);

        break;
    }

    return ntStatus;
#endif /* _USB */

#ifdef RTMP_PCI_SUPPORT
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)fdo->DeviceExtension;
	//if( dx->IODisabled)
	//	return CompleteIrp( Irp, STATUS_DEVICE_NOT_CONNECTED, 0);
	//if (!LockDevice(dx))
	//	return CompleteIrp( Irp, STATUS_DELETE_PENDING, 0);

	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION IrpStack;
	ULONG MinorFunction;

	DBGPRINT(RT_DEBUG_TRACE,("Power %I",Irp));

	IrpStack = IoGetCurrentIrpStackLocation(Irp);
	MinorFunction = IrpStack->MinorFunction;

	if( MinorFunction==IRP_MN_SET_POWER)
		status = PowerSetPower(dx,Irp);
	else
		status = DefaultPowerHandler(dx,Irp);

	//UnlockDevice(dx);
	return status;
#endif /* RTMP_PCI_SUPPORT */
}


#ifdef RTMP_PCI_SUPPORT
/////////////////////////////////////////////////////////////////////////////
//	PowerSetPower:	Handle set system and device power

NTSTATUS PowerSetPower( IN RTMP_ADAPTER *dx, IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
	POWER_STATE_TYPE PowerType = IrpStack->Parameters.Power.Type;
	POWER_STATE PowerState = IrpStack->Parameters.Power.State;

	/////////////////////////////////////////////////////////////////////////
	//	Set System Power

	if( PowerType==SystemPowerState)
	{
		DEVICE_POWER_STATE DesiredDevicePowerState =
			(PowerState.SystemState<=PowerSystemWorking ? PowerDeviceD0 : PowerDeviceD3);
		
		if( DesiredDevicePowerState<dx->PowerState)
		{
			// This system state means we have to increase device power
			DBGPRINT(RT_DEBUG_TRACE,("System state %d.  Increase device power to %d",
						PowerState.SystemState, DesiredDevicePowerState));
			// Process on way up stack...
			PoStartNextPowerIrp(Irp);
			IoCopyCurrentIrpStackLocationToNext(Irp);
			IoSetCompletionRoutine( Irp, OnCompleteIncreaseSystemPower, NULL, TRUE, TRUE, TRUE);
			return PoCallDriver( dx->pLowerDevice, Irp);
		}
		else if( DesiredDevicePowerState>dx->PowerState)
		{
			// This system state means we have to decrease device power
			DBGPRINT(RT_DEBUG_TRACE,("System state %d.  Decrease device power to %d",
						PowerState.SystemState, DesiredDevicePowerState));
			// Send power down request to device
			status = SendDeviceSetPower( dx, DesiredDevicePowerState);
			if( !NT_SUCCESS(status))
			{
				PoStartNextPowerIrp(Irp);
				return CompleteIrp( Irp, status, 0);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////
	//	Set Device Power
	else if( PowerType==DevicePowerState)
	{
		DEVICE_POWER_STATE DesiredDevicePowerState = PowerState.DeviceState;

		if( DesiredDevicePowerState<dx->PowerState)
		{
			// Increase device power state
			DBGPRINT(RT_DEBUG_TRACE,("Increase device power to %d", DesiredDevicePowerState));
			// Process on way up stack...
			PoStartNextPowerIrp(Irp);
			IoCopyCurrentIrpStackLocationToNext(Irp);
			IoSetCompletionRoutine( Irp, OnCompleteIncreaseDevicePower, NULL, TRUE, TRUE, TRUE);
			return PoCallDriver( dx->pLowerDevice, Irp);
		}
		else if( DesiredDevicePowerState>dx->PowerState)
		{
			// Decrease device power state
			DBGPRINT(RT_DEBUG_TRACE,("Decrease device power to %d", DesiredDevicePowerState));
			// Set power state
			SetPowerState(dx,PowerState.DeviceState);
		}
	}

	/////////////////////////////////////////////////////////////////////////
	//	Unrecognised Set Power
#if DBG
	else
		DBGPRINT(RT_DEBUG_TRACE,("Power: unrecognised power type %d",PowerType));
#endif

	// Finally pass to lower drivers
	return DefaultPowerHandler(dx,Irp);
}


/////////////////////////////////////////////////////////////////////////////
//	DefaultPowerHandler:	Pass Power IRP down stack

NTSTATUS DefaultPowerHandler( IN RTMP_ADAPTER *dx, IN PIRP Irp)
{
	DBGPRINT(RT_DEBUG_TRACE,("DefaultPowerHandler"));
	// Just pass to lower driver
	PoStartNextPowerIrp( Irp);
	IoSkipCurrentIrpStackLocation(Irp);
	return PoCallDriver( dx->pLowerDevice, Irp);
}

/////////////////////////////////////////////////////////////////////////////
//	OnCompleteIncreaseSystemPower:	Set system power (increase device power) completed

NTSTATUS OnCompleteIncreaseSystemPower( IN PDEVICE_OBJECT fdo, IN PIRP Irp, IN PVOID context)
{
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)fdo->DeviceExtension;
	NTSTATUS status;
	PIO_STACK_LOCATION IrpStack;
	POWER_STATE PowerState;
	DEVICE_POWER_STATE DesiredDevicePowerState;
	
	if (Irp->PendingReturned)
		IoMarkIrpPending(Irp);

	status = Irp->IoStatus.Status;
	DBGPRINT(RT_DEBUG_TRACE,("OnCompleteIncreaseSystemPower %x",status));
	if( !NT_SUCCESS(status))
		return status;

	IrpStack = IoGetCurrentIrpStackLocation(Irp);
	PowerState = IrpStack->Parameters.Power.State;
	DesiredDevicePowerState =
		(PowerState.SystemState<=PowerSystemWorking ? PowerDeviceD0 : PowerDeviceD3);
	if( DesiredDevicePowerState<dx->PowerState)
		status = SendDeviceSetPower( dx, DesiredDevicePowerState);

	PoStartNextPowerIrp(Irp);
	return status;
}

/////////////////////////////////////////////////////////////////////////////
//	OnCompleteIncreaseDevicePower:	Set device power completed

NTSTATUS OnCompleteIncreaseDevicePower( IN PDEVICE_OBJECT fdo, IN PIRP Irp, IN PVOID context)
{
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)fdo->DeviceExtension;
	NTSTATUS status;
	PIO_STACK_LOCATION IrpStack;
	POWER_STATE PowerState;
	
	if (Irp->PendingReturned)
		IoMarkIrpPending(Irp);
	status = Irp->IoStatus.Status;
	DBGPRINT(RT_DEBUG_TRACE,("OnCompleteIncreaseDevicePower %x",status));
	if( !NT_SUCCESS(status))
		return status;

	IrpStack = IoGetCurrentIrpStackLocation(Irp);
	PowerState = IrpStack->Parameters.Power.State;
	SetPowerState(dx,PowerState.DeviceState);

	PoStartNextPowerIrp(Irp);
	return status;
}

/////////////////////////////////////////////////////////////////////////////
//	SendDeviceSetPower:	Send set power for device
//						Must not be called from set device power as it waits on an event

typedef struct _SDSP
{
	KEVENT event;
	NTSTATUS Status;
} SDSP, *PSDSP;

NTSTATUS SendDeviceSetPower( IN RTMP_ADAPTER *dx, IN DEVICE_POWER_STATE NewDevicePowerState)
{
	POWER_STATE NewState;
	SDSP sdsp;
	NTSTATUS status;
	
	DBGPRINT(RT_DEBUG_TRACE,("SendDeviceSetPower to %d", NewDevicePowerState));

	NewState.DeviceState = NewDevicePowerState;
	KeInitializeEvent( &sdsp.event, NotificationEvent, FALSE);
	sdsp.Status = STATUS_SUCCESS;
	status = PoRequestPowerIrp(dx->pPhyDeviceObj, IRP_MN_SET_POWER, NewState,
							OnCompleteDeviceSetPower, &sdsp, NULL);
	if( status==STATUS_PENDING)
	{
		KeWaitForSingleObject( &sdsp.event, Executive, KernelMode, FALSE, NULL);
		status = sdsp.Status;
	}

	// Cope with W98 not passing power irp to us
	if( NT_SUCCESS(status) && dx->PowerState!=NewDevicePowerState)
	{
		DBGPRINT(RT_DEBUG_TRACE,("SendDeviceSetPower: Device state not set properly by us.  Setting again"));
		SetPowerState(dx,NewDevicePowerState);
	}

	return status;
}

/////////////////////////////////////////////////////////////////////////////
//	OnCompleteDeviceSetPower:	Set device power IRP completed

VOID OnCompleteDeviceSetPower( IN PDEVICE_OBJECT fdo, IN UCHAR MinorFunction,
					IN POWER_STATE PowerState, IN PVOID Context, IN PIO_STATUS_BLOCK IoStatus)
{
	PSDSP psdsp;
	
	DBGPRINT(RT_DEBUG_TRACE,("OnCompleteDeviceSetPower"));
	psdsp = (PSDSP)Context;
	psdsp->Status = IoStatus->Status;
	KeSetEvent( &psdsp->event, 0, FALSE);
}


/////////////////////////////////////////////////////////////////////////////

NTSTATUS CompleteIrp( IN PIRP Irp, IN NTSTATUS status, IN ULONG info)
{
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	return status;
}
/////////////////////////////////////////////////////////////////////////////

void SetPowerState( IN RTMP_ADAPTER *dx, IN DEVICE_POWER_STATE NewDevicePowerState)
{
	POWER_STATE NewState;

	DBGPRINT(RT_DEBUG_TRACE,("SetPowerState %d", NewDevicePowerState));
	// Use KeSynchronizeExecution if necessary to actually change power in device
	// Remember new state
	dx->PowerState = NewDevicePowerState;

	NewState.DeviceState = NewDevicePowerState;
	if( KeGetCurrentIrql()==DISPATCH_LEVEL && NewDevicePowerState!=PowerDeviceD0)
	{
		DBGPRINT(RT_DEBUG_TRACE,("SetPowerState IRQL %d fail", KeGetCurrentIrql()));
	}
	else
		PoSetPowerState( dx->pDeviceObject, DevicePowerState, NewState);
}

/////////////////////////////////////////////////////////////////////////////
#endif /* RTMP_PCI_SUPPORT */
