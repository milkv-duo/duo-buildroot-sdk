#ifndef	__DEFINE_H__
#define	__DEFINE_H__


#include <usb.h>

#include <usbdrivr.h>
#include <Wmilib.h>

//#define MEMORY_TAG (ULONG) 'aqTM'
//#define DRV_INSTANCE_SIGNATURE (ULONG) 'aqTM'
#ifdef _USB

//
// FW TxD
//
// 
//
// Note: 
//  1. DW : double word
//
typedef struct _FIRMWARE_TXDSCR
{
    // DW0
    ULONG   Length:16;                              // [15 : 0] Equal to TxByteCount of TxD.
    ULONG   PQ_ID:16;                               // [31 : 16]    For firmware download CMD, this field should be stuff to 0x8000; for image download pkt, this field should be stuff to 0xd000.

    // DW1
    ULONG   CID:8;                                  // [7 : 0]      The CMD ID defined in firmware download protocol.
    ULONG   PktTypeID:8;                            // [15 : 8] Set to 0xA0 (FT = 0x1, HF=0x1, other fields are ignored).
    ULONG   SetQuery:8;                             // [23 : 16]    not use so far.
    ULONG   SeqNum:8;                               // [31 : 24]    The sequence number of the pkt.

    // DW2
    ULONG   Reserve;                                // [31 : 0] TBD (sync to the normal TxD format).
} FIRMWARE_TXDSCR, *PFIRMWARE_TXDSCR;
//
// Command to start loading FW
//
// 
typedef struct _CMD_START_TO_LOAD_FW_REQUEST
{
    FIRMWARE_TXDSCR FwTxD;
    ULONG               Address;        // the start address of firmware code to be copied in SRAM in Address memory.
    ULONG               Length;         // the total length of the firmware image size in bytes.
    ULONG               DataMode;       // the option field for this CMD packet.
} CMD_START_TO_LOAD_FW_REQUEST, *PCMD_START_TO_LOAD_FW_REQUEST;

//
// Command to start running FW
//
// 
typedef struct _CMD_START_FW_REQUEST
{
    FIRMWARE_TXDSCR FwTxD;
    ULONG               Override;       // B0 : 1 --> indicate to adopt value of field u4Address to be the entry point for RAM code starting up.
                                        // B0 : 0 -->use default address as entry point for RAM code starting up.
                                        // Other bits are useless
    ULONG               Address;        // the address to the entry point for RAM to start up, it is used when B0 of u4Override field is equal to 1.
} CMD_START_FW_REQUEST, *PCMD_START_FW_REQUEST;

//
// Command to Get Rom Patch Semaphore
//
// 
typedef struct _CMD_GET_PATCH_SEMAPHORE
{
	FIRMWARE_TXDSCR FwTxD;
	ULONG	ucGetSemaphore:8;
	ULONG	Reserve:24;
	
} CMD_GET_PATCH_SEMAPHORE, *PCMD_GET_PATCH_SEMAPHORE;

//7603.h
#define FW_INFO_LENGTH_7603 32

//
// The information of 7603 Firmware bin file
//
typedef struct _FW_BIN_INFO_STRUC_7603
{
	// DW0
	ULONG       MagicNumber;
	UCHAR       Version[12];
	UCHAR       Date[16];
	ULONG       ImageLength;
}  FW_BIN_INFO_STRUC_7603, *PFW_BIN_INFO_STRUC_7603;


#define FW_INFO_LENGTH_7636 36
//
// FW image feature set
//
typedef struct _FIRMWARE_FEATURE_SET
{
	UCHAR	Encryption:1; 							// [0]		0: plain image is used 1: encrypted image is used	
	UCHAR	KEY_INDEX:2; 							// [2 : 1]		key index of the cncryption
	UCHAR	Reserve:5;
} FIRMWARE_FEATURE_SET, *PFIRMWARE_FEATURE_SET;

#if 0
typedef union _FW_BIN_INFO_STRUC
{
	//
	// The information of 7603 Firmware bin file
	//
	struct	{
	    // DW0
	    ULONG       MagicNumber;
	    UCHAR       Version[12];
	    UCHAR       Date[16];
	    ULONG       ImageLength;
		ULONG	   Reserved:9;
	}	7603;
	//
	// The information of 7636 Firmware bin file
	//
	struct {
		ULONG		Address;
		FIRMWARE_FEATURE_SET		FeatureSet;
		UCHAR		Version[11];
		UCHAR		Date[16];
		ULONG		ImageLength;
	}	7636;
}  FW_BIN_INFO_STRUC, *PFW_BIN_INFO_STRUC;
#endif

//
// The information of 7636 Firmware bin file
//
typedef struct _FW_BIN_INFO_STRUC_7636
{
	ULONG		Address;
	UCHAR		ChipInfo;
	FIRMWARE_FEATURE_SET		FeatureSet;//FIRMWARE_FEATURE_SET		FeatureSet;	
	UCHAR		Version[10];
	UCHAR		Date[16];
	ULONG		ImageLength;
}  FW_BIN_INFO_STRUC_7636, *PFW_BIN_INFO_STRUC_7636;

#define	ROM_PATCH_ADDRESS	0x0008C000

//
// The header format of Patch bin file
//
typedef	struct	_ROM_PATCH_BIN_HEADER_STRUC	{
		ULONGLONG	   	TimeStampLowPart;   // ASCII code
		ULONGLONG       	TimeStampHighPart;  // ASCII code	
		ULONG       		Platform;
		ULONG       		HwSwVersion;
		ULONG       		PatchVersion;
		USHORT	  		Checksum;		
}	ROM_PATCH_BIN_HEADER_STRUC, *PROM_PATCH_BIN_HEADER_STRUC;

#define ROM_PATCH_INFO_LENGTH		30

//
// checksum caculation request data
//
typedef struct _ROMPATCH_CAL_CHECKSUM_DATA
{
	ULONG	Address;
	ULONG	Length;
} ROMPATCH_CAL_CHECKSUM_DATA, *PROMPATCH_CAL_CHECKSUM_DATA;

//
// Scheduler Register 4 (offset: 0x0594, default: 0x0000_0000) 
//
// 
//
// Note: 
//  1. DW : double word
//
typedef union _SCHEDULER_REGISTER4
{
	struct	{
	    // DW0
	    ULONG   ForceQid:4;                                 // [3 : 0]      When force_mode = 1, queue id will adopt this index.
	    ULONG   ForceMode:1;                            // [4]      Force enable dma_scheduler work in force queue without calculate TXOP time.
	    ULONG   BypassMode:1;                           // [5]      Bypass_mode:1, for PSE loopback only.
	    ULONG   HybridMode:1;                           // [6]      Hybrid_mode:1, when assert this reg, DMA scheduler would ignore the tx op time information from LMAC, and also use FFA and RSV for enqueue cal.
	    ULONG   RgPredictNoMask:1;                      // [7]      When disable high and queue mask, force in predict mode.
	    ULONG   RgResetScheduler:1;                         // [8]      DMA Scheduler soft reset. When assert this reset, dma scheduler state machine will be in INIT state and reset all queue counter.
	    ULONG   RgDoneClearHeader:1;                    // [9]      Switch header counter clear flag. 0 : Clear by tx_eof   1 : Clear by sch_txdone
	    ULONG   SwMode:1;                               // [10]     When SwMode is 1 : DMA scheduler will ignore rate mapping & txop time threshold from LMAC, and use RgRateMap & txtime_thd_* value.
	    ULONG   Reserves0:5;                            // [15 : 11]    Reserved bits.
	    ULONG   RgRateMap:14;                           // [29 : 16]    When SwMode 1 : DMA scheduler will load rate mapping information from this register, else rate mapping information from LMAC. Rate Mapping table : {mode X [2 : 0], rate [5 : 0], frmode [1 : 0], nss [1:  0], sg_flag}
	    ULONG   Reserves1:2;                            // [31 : 30]    Reserved bits.
	}	Default;
	ULONG			word;
} SCHEDULER_REGISTER4, *PSCHEDULER_REGISTER4;

//
// TOP MISC CONTROLS2 (offset: 0x0134, default: 0x0000_0000) 
//
// 
//
// Note: 
//  1. DW : double word
//
typedef union _TOP_MISC_CONTROLS2
{
	struct	{
		// DW0
	    ULONG   ReadyToLoadFW:1;                        // [0]      Reserved bit.
	    ULONG   FwIsRunning:1;                          // [1]      Check if FW need to reload.
	    ULONG   Reserves:22;                            // [23 : 0] Reserved bits.
	    ULONG   TOP_MISC_DBG_SEL:4;                     // [27 : 24]    
	    ULONG   DBG_SEL:4;                              // [31 : 28]    	}	Default;
	}	Default;
	ULONG			word;
} TOP_MISC_CONTROLS2, *PTOP_MISC_CONTROLS2;

//~7603.h


#define BULKTAG (ULONG) 'KluB'

#undef ExAllocatePool
#define ExAllocatePool(type, size) \
        ExAllocatePoolWithTag(type, size, BULKTAG)
#undef  ExFreePool
#define ExFreePool(type) \
        ExFreePoolWithTag(type,BULKTAG)


typedef enum _DEVSTATE {

    USBNotStarted,         // not started
    USBStopped,            // device stopped
    USBWorking,            // started and working
    USBPendingStop,        // stop pending
    USBPendingRemove,      // remove pending
    USBSurpriseRemoved,    // removed by surprise
    USBRemoved             // removed

} DEVSTATE;

typedef enum _QUEUE_STATE {

    HoldRequests,       // device is not started yet
    AllowRequests,      // device is ready to process
    FailRequests        // fail both existing and queued up requests

} QUEUE_STATE;

typedef struct _USB_PIPE_CONTEXT {

    BOOLEAN PipeOpen;

} USB_PIPE_CONTEXT, *PUSB_PIPE_CONTEXTT;

typedef enum _WDM_VERSION {

    Win2kOrBetter = 1,
    WinXpOrBetter,
    WinVistaOrBetter

} WDM_VERSION;

typedef struct _MTKDRV_EXTENSION {

	ULONG                   Signature; // must be PCIDRV_FDO_INSTANCE_SIGNATURE
	
	// Functional Device Object
	PDEVICE_OBJECT FunctionalDeviceObject;

	// Device object we call when submitting Urbs
	PDEVICE_OBJECT NextLowerDriver;

	// The bus driver object
	PDEVICE_OBJECT PhysicalDeviceObject;

	// Name buffer for our named Functional device object link
	// The name is generated based on the driver's class GUID
	UNICODE_STRING InterfaceName;

	// Bus drivers set the appropriate values in this structure in response
	// to an IRP_MN_QUERY_CAPABILITIES IRP. Function and filter drivers might
	// alter the capabilities set by the bus driver.
	DEVICE_CAPABILITIES DeviceCapabilities;

	// ************************************************************************
	// **   PCI
	// ************************************************************************
	BUS_INTERFACE_STANDARD  BusInterface;

	// IDs
	UCHAR                   RevsionID;
	USHORT                  SubVendorID;
	USHORT                  SubSystemID;

	// ************************************************************************
	// **   USB
	// ************************************************************************

	// Configuration Descriptor
	PUSB_CONFIGURATION_DESCRIPTOR UsbConfigurationDescriptor;

	// Interface Information structure
	PUSBD_INTERFACE_INFORMATION UsbInterface;

	// Pipe context for the bulkusb driver
	PUSB_PIPE_CONTEXTT PipeContext;

	// current state of device
	DEVSTATE DeviceState;

	// state prior to removal query
	DEVSTATE PrevDevState;

	// obtain and hold this lock while changing the device state,
	// the queue state and while processing the queue.
	KSPIN_LOCK DevStateLock;

	// current system power state
	SYSTEM_POWER_STATE SysPower;

	// current device power state
	DEVICE_POWER_STATE DevPower;

	// Pending I/O queue state
	QUEUE_STATE QueueState;

	// Pending I/O queue
	LIST_ENTRY NewRequestsQueue;

	// I/O Queue Lock
	KSPIN_LOCK QueueLock;

	KEVENT RemoveEvent;

	KEVENT StopEvent;

	LONG OutStandingIO;

	KSPIN_LOCK IOCountLock;

	// selective suspend variables

	LONG SSEnable;

	LONG SSRegistryEnable;

	PUSB_IDLE_CALLBACK_INFO IdleCallbackInfo;
	//PIRP PendingIdleIrp;
	LONG IdleReqPend;

	LONG FreeIdleIrpCount;

	KSPIN_LOCK IdleReqStateLock;

	KEVENT NoIdleReqPendEvent;

	// default power state to power down to on self-susped
	ULONG PowerDownLevel;

	// remote wakeup variables
	PIRP WaitWakeIrp;

	LONG FlagWWCancel;

	LONG FlagWWOutstanding;

	LONG WaitWakeEnable;

	LONG FlagWWDispatched;

	// open handle count
	LONG OpenHandleCount;

	// selective suspend model uses timers, dpcs and work item.
	KTIMER Timer;

	KDPC DeferredProcCall;

	// This event is cleared when a DPC/Work Item is queued.
	// and signaled when the work-item completes.
	// This is essential to prevent the driver from unloading
	// while we have DPC or work-item queued up.
	KEVENT NoDpcWorkItemPendingEvent;

	// WMI information
	WMILIB_CONTEXT WmiLibInfo;

	// WDM version
	WDM_VERSION WdmVersion;

} MTKDRV_EXTENSION , *PMTKDRV_EXTENSION ;

typedef struct _BULKUSB_PIPE_CONTEXT {

    BOOLEAN PipeOpen;

} BULKUSB_PIPE_CONTEXT, *PBULKUSB_PIPE_CONTEXT;

#define MAX_TX_BULK_PIPE_NUM 6
#define MAX_RX_BULK_PIPE_NUM 2


#define NOR_FLASH_SIZE_TYPE_8_BIT		0
#define NOR_FLASH_SIZE_TYPE_8_16_BIT		1
//
// A structure representing the instance information associated with
// this particular device.
//

typedef struct _RTMP_ADAPTER {

    // Functional Device Object
    PDEVICE_OBJECT FunctionalDeviceObject;

    // Device object we call when submitting Urbs
    PDEVICE_OBJECT TopOfStackDeviceObject;

    // The bus driver object
    PDEVICE_OBJECT PhysicalDeviceObject;

    //PDEVICE_OBJECT	NextStackDevice;//++werner

    UNICODE_STRING	ifSymLinkName;//++werner

    // Name buffer for our named Functional device object link
    // The name is generated based on the driver's class GUID
    UNICODE_STRING InterfaceName;

    // Bus drivers set the appropriate values in this structure in response
    // to an IRP_MN_QUERY_CAPABILITIES IRP. Function and filter drivers might
    // alter the capabilities set by the bus driver.
    DEVICE_CAPABILITIES DeviceCapabilities;

    // Configuration Descriptor
    PUSB_CONFIGURATION_DESCRIPTOR UsbConfigurationDescriptor;

    // Interface Information structure
    PUSBD_INTERFACE_INFORMATION UsbInterface;

    // Pipe context for the bulkusb driver
    PBULKUSB_PIPE_CONTEXT PipeContext;

    // current state of device
    DEVSTATE DeviceState;

    // state prior to removal query
    DEVSTATE PrevDevState;

    // obtain and hold this lock while changing the device state,
    // the queue state and while processing the queue.
    KSPIN_LOCK DevStateLock;

    // current system power state
    SYSTEM_POWER_STATE SysPower;

    // current device power state
    DEVICE_POWER_STATE DevPower;

    // Pending I/O queue state
    QUEUE_STATE QueueState;

    // Pending I/O queue
    LIST_ENTRY NewRequestsQueue;

    // I/O Queue Lock
    KSPIN_LOCK QueueLock;

    KEVENT RemoveEvent;

    KEVENT StopEvent;
    
    ULONG OutStandingIO;

    KSPIN_LOCK IOCountLock;

    // selective suspend variables

    //LONG SSEnable;

    LONG SSRegistryEnable;

    PUSB_IDLE_CALLBACK_INFO IdleCallbackInfo;
    PIRP PendingIdleIrp;
    LONG IdleReqPend;

    LONG FreeIdleIrpCount;

    KSPIN_LOCK IdleReqStateLock;

    KEVENT NoIdleReqPendEvent;

    // default power state to power down to on self-susped
    ULONG PowerDownLevel;
    
    // remote wakeup variables
    PIRP WaitWakeIrp;

    LONG FlagWWCancel;

    LONG FlagWWOutstanding;

    //LONG WaitWakeEnable;

    LONG FlagWWDispatched;

    // open handle count
    LONG OpenHandleCount;

    // selective suspend model uses timers, dpcs and work item.
    KTIMER Timer;

    KDPC DeferredProcCall;

    // This event is cleared when a DPC/Work Item is queued.
    // and signaled when the work-item completes.
    // This is essential to prevent the driver from unloading
    // while we have DPC or work-item queued up.
    KEVENT NoDpcWorkItemPendingEvent;

    // WMI information
    WMILIB_CONTEXT WmiLibInfo;

    // WDM version
    WDM_VERSION WdmVersion;

	//++werner-->
	BOOLEAN GotResources;	// Not stopped
	BOOLEAN Paused;		// Stop or remove pending
	BOOLEAN IODisabled;	// Paused or stopped


	LONG UsageCount;		// Pending I/O Count
	BOOLEAN Stopping;			// In process of stopping
	KEVENT StoppingEvent;	// Set when all pending I/O complete


	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	USBD_CONFIGURATION_HANDLE UsbConfigurationHandle;	// Selected Configuration handle

	PUSBD_PIPE_INFORMATION UsbIntrInPipeInfo;
	PUSBD_PIPE_INFORMATION UsbBulkInPipeInfo;
	PUSBD_PIPE_INFORMATION UsbBulkOutPipeInfo;
	
	USBD_PIPE_HANDLE UsbIntrInPipeHandle;	// Handle to input interrupt pipe
	USBD_PIPE_HANDLE UsbBulkInPipeHandle[MAX_RX_BULK_PIPE_NUM];	// Handle to input Bulk pipe
	USBD_PIPE_HANDLE UsbBulkOutPipeHandle[MAX_TX_BULK_PIPE_NUM];	// Handle to Output Bulk pipe
	
	USHORT                   UsbBulkOutMaxPacketSize;
	USHORT                   UsbBulkInMaxPacketSize;
	
	PURB				pRxUrb[MAX_RX_BULK_PIPE_NUM];
	PIRP					pRxIrp[MAX_RX_BULK_PIPE_NUM];
	PUCHAR				RxBuffer[MAX_RX_BULK_PIPE_NUM];
	USHORT				RxBufferLength[MAX_RX_BULK_PIPE_NUM];
	ULONG				ulBulkInRunning[MAX_RX_BULK_PIPE_NUM];

	PURB				pTxUrb[MAX_TX_BULK_PIPE_NUM];
	PIRP					pTxIrp[MAX_TX_BULK_PIPE_NUM];
	PUCHAR				TxBuffer[MAX_TX_BULK_PIPE_NUM];
	ULONG				TxBufferLength[MAX_TX_BULK_PIPE_NUM];
	//UCHAR				TxBufferForIncLenTest[8192];//[USB_BULK_BUFFER_SIZE];
	BOOLEAN				bBulkOutRunning;
	BOOLEAN				bBulkOutRunning1;
	USHORT				bMaxLen;
	USHORT				bMinLen;
	USHORT				bCurrlength;

	KEVENT  				CancelRxIrpEvent[MAX_RX_BULK_PIPE_NUM];      // for cancelling bulk In IRPs.
	KEVENT  				CancelTxIrpEvent[MAX_TX_BULK_PIPE_NUM];   	// for cancelling bulk Out IRPs.

	// Fw Req & Rsp
	USHORT				FwReqSequence;
	UCHAR				FwReceiveBuffer[1024];
	
	ULONG				TransmittedCount;
	ULONG				TxBulkCount[MAX_TX_BULK_PIPE_NUM];
	LARGE_INTEGER		ReceivedCount[MAX_RX_BULK_PIPE_NUM];	
	COUNTER_802_11		WlanCounters;
	OTHER_STATISTICS	OtherCounters;
	ULONG				SecurityFail ;
	//LONG	RSSI0;
	//LONG	RSSI1;
	//LONG	RSSI2;
	//LONG	SNR0;
	//LONG	SNR1;
	//LONG	SNR2;
	//LONG	BF_SNR0;
	//LONG	BF_SNR1;
	//LONG	BF_SNR2;
	
	BOOLEAN				Seq_dbg;
	BOOLEAN				ContinBulkOut;
	ULONG				BulkOutRemained;
	ULONG				BulkOutWhichPipeNeedRun;
	ULONG				BulkOutTxType;

	KSPIN_LOCK 			BulkInIoCallDriverSpinLock[MAX_RX_BULK_PIPE_NUM];
	BOOLEAN				bBulkInIoCallDriverFlag[MAX_RX_BULK_PIPE_NUM];	
	BOOLEAN				ContinBulkIn[MAX_RX_BULK_PIPE_NUM];
	USHORT				BulkInRemained[MAX_RX_BULK_PIPE_NUM];

	KSPIN_LOCK 			RxSpinLock[MAX_RX_BULK_PIPE_NUM];		// Spin lock for sniffer momory move
	BOOLEAN				bRxEnable[MAX_RX_BULK_PIPE_NUM];
	BOOLEAN				RxAggregate;    //enable rx aggregate
	BOOLEAN				TxAggregate;    //enable Tx aggregate
	BOOLEAN				bNextVLD;
 	
	KSPIN_LOCK 			SnifferSpinLock;		// Spin lock for sniffer momory move
	//PUCHAR				SnifferBuffer;
	BOOLEAN				bEnableSniffer;
	USHORT				SnifferDriverCP;
	USHORT				SnifferAppCP;
	ULONG				bDriverSupportSniffer;

	//RX MPDU
	PUCHAR				RXMPDUBuffer;
	ULONG				RXMPDULength;
	USHORT				MPDUDriverCP;
	USHORT				MPDUAppCP;
	KSPIN_LOCK 			MPDUSpinLock;		// Spin lock for MPDU momory move
	
	KSEMAPHORE			IoCtrlSemaphore;

	ULONG				MACVersion;
	ULONG				FMode;

	ULONG 					ulSetNorFlashSizeType;
	//--------------------------------------------------------------------------

	//-----------------------------------------------------
	ULONG bAutoTuningEnabled;

	BOOLEAN bExternalLnaEnabled;
	UCHAR	ucFrameEndType;

	BOOLEAN bRadarDetected;

	//--------------------------------------------------------------------------

	BOOLEAN bIsTxOfdm;

	//--------------------------------------------------------------------------
	
	ULONG UsbTimeout;	// Read timeout in seconds

	// Resource allocations
	BOOLEAN GotPortOrMemory;
	BOOLEAN PortInIOSpace;
	BOOLEAN PortNeedsMapping;
	PUCHAR PortBase;
	PHYSICAL_ADDRESS PortStartAddress;
	ULONG PortLength;

	BOOLEAN GotInterrupt;
	ULONG Vector;
	KIRQL Irql;
	KINTERRUPT_MODE Mode;
	KAFFINITY Affinity;
	PKINTERRUPT InterruptObject;

	ULONG					ShiftReg;

	BOOLEAN SurprisedRemoved;	// Surprised removed device
	//++werner<--
	//for Efuse using
	UCHAR	EfuseMode;// Buffer:0, Efuse:1, EEPROM:2
	UCHAR	EfuseContent[EFUSESIZE];

	//TxSetFrequencyOffset
	BOOLEAN					IsTxSetFrequency;// check if is if first time to run TxSetFrequencyOffset
	
	//FW
	UCHAR		FWImage[MAXFIRMWARESIZE];//From UI
	ULONG		FWSize;//From UI
	BOOLEAN		IsFWImageInUse;
	BOOLEAN		IsUISetFW;
	ULONG		LoadingFWCount;

	BOOLEAN		bLoadingFW;
	KEVENT		LoadFWEvent;
	KEVENT		WaitFWEvent;
	BOOLEAN		bIsWaitFW;

	KEVENT		RomPatchEvent;
	UCHAR		RomPatchSemStatus;
	
	ULONG		FwCmdSeqNum;
} RTMP_ADAPTER;


#define INITIALIZE_PNP_STATE(_Data_)    \
        (_Data_)->DeviceState =  USBNotStarted;\
        (_Data_)->PrevDevState = USBNotStarted;

#define SET_NEW_PNP_STATE(_Data_, _state_) \
        (_Data_)->PrevDevState =  (_Data_)->DeviceState;\
        (_Data_)->DeviceState = (_state_);

#define RESTORE_PREVIOUS_PNP_STATE(_Data_)   \
        (_Data_)->DeviceState =   (_Data_)->PrevDevState;






#endif

#endif

