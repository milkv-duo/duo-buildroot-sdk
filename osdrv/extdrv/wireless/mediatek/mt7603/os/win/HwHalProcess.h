#ifndef _HW_HAL_PROCESS_H
#define _HW_HAL_PROCESS_H


//---------- Dummy ----------
// for Software Debug Used
#define READ_Dummy_REGISTER_UCHAR(_A) 0
#define WRITE_Dummy_REGISTER_UCHAR(_A, _V) ;

#define READ_Dummy_REGISTER_USHORT(_A) 0
#define WRITE_Dummy_REGISTER_USHORT(_A, _V) ;

#define READ_Dummy_REGISTER_ULONG(_A) 0
#define WRITE_Dummy_REGISTER_ULONG(_A, _V) ;

#define RTMP_Dummy_IO_READ32(_A, _R) 0
#define RTMP_Dummy_IO_WRITE32(_A, _R, _V) ;

//---------- Real ----------
#ifdef RTMP_PCI_SUPPORT
#define READ_Real_REGISTER_UCHAR(_A) 	READ_REGISTER_UCHAR(_A)

#define WRITE_Real_REGISTER_UCHAR(_A, _V) \
{	\
	WRITE_REGISTER_UCHAR(_A, _V); \
}

#define READ_Real_REGISTER_USHORT(_A) READ_REGISTER_USHORT(_A)
	
#define WRITE_Real_REGISTER_USHORT(_A, _V) \
{	\
	WRITE_REGISTER_USHORT(_A, _V); \
}

#define READ_Real_REGISTER_ULONG(_A) READ_REGISTER_ULONG(_A)
 
#define WRITE_Real_REGISTER_ULONG(_A, _V) \
{	\
	WRITE_REGISTER_ULONG(_A, _V); \
}

// divided 4 means bytes to ULONG
#define RTMP_Real_IO_READ32(_A, _R)	READ_Real_REGISTER_ULONG(((PULONG)((_A)->virtualaddr ) + (_R)/4))
#define RTMP_Real_IO_WRITE32(_A, _R, _V) 	\
{	\
if(_R<0x4000 || _R>0x8000)	\
	DebugPrint("!!!!!!!Write Not HIF Register!!!!fun=%s    line=%d!!!!!!!\n",__FUNCTION__,__LINE__); \
WRITE_Real_REGISTER_ULONG(((PULONG)((_A)->virtualaddr )+ (_R)/4), (_V))	\
}
#endif /* RTMP_PCI_SUPPORT */

/*#define WRITE_Real_REGISTER_ULONG(_A, _V) \
{	\
if(_V<0x4000 || _V>0x8000)	\
	DebugPrint("!!!!!!!Not HIF Register!!!!!!!!!!!!!!1\n"); \
WRITE_REGISTER_ULONG(_A, _V); \
}*/

#if 0
#define RTMP_IO_READ32(_A, _R, _pV) 	\
{	\
if(_R<0x4000 || _R>0x8000)	\
	DebugPrint("!!!!!!!Read Not HIF Register fun=%s    line=%d!!!!!!!\n",__FUNCTION__,__LINE__); \
*_pV = RTMP_Real_IO_READ32(_A, _R);	\
}

#define RTMP_IO_WRITE32(_A, _R, _V) 	\
{	\
	DebugPrint("Write Register  %s line=%d  addr=0x%08X,  setdata=0x%08X\n",__FUNCTION__,__LINE__,_R,_V); \
WRITE_Real_REGISTER_ULONG(((PULONG)((_A)->virtualaddr )+ (_R)/4), (_V))	\
}
#else

#ifdef _USB
NTSTATUS RTMP_IO_READ32( IN RTMP_ADAPTER *deviceExtension, ULONG addr, ULONG *Value);
NTSTATUS RTMP_IO_WRITE32( IN RTMP_ADAPTER *deviceExtension, ULONG addr, ULONG Value);
#endif /* _USB */

#ifdef RTMP_PCI_SUPPORT //PCIe
#define RTMP_IO_READ32(_A, _R, _pV) 	\
{	\
if(_R<0x4000 || _R>0x8000)	\
	DebugPrint("fun=%s   read addr=0x%08X!!!!!!!\n",__FUNCTION__,_R); \
	/* *_pV = RTMP_Real_IO_READ32((PEXTENDED_PCI_DATA)_A->DeviceExtension, _R); */ \
	*(_pV) = READ_Real_REGISTER_ULONG(((PULONG)((_A)->virtualaddr ) + (_R)/4));\
}

#define RTMP_IO_WRITE32(_A, _R, _V) 	\
{	\
	DebugPrint("Write Register  %s line=%d  addr=0x%08X,  setdata=0x%08X\n",__FUNCTION__,__LINE__,_R,_V); \
	/*WRITE_Real_REGISTER_ULONG(((PULONG)(((PEXTENDED_PCI_DATA)_A->DeviceExtension)->virtualaddr )+ (_R)/4), (_V));*/	\
	WRITE_Real_REGISTER_ULONG(((PULONG)((_A)->virtualaddr )+ (_R)/4), (_V));\
}
#endif /* RTMP_PCI_SUPPORT */

#endif

//------------------------------

//
// BBP & RF	register has to	check busy & idle condition
//
#define	BUSY		1
#define	IDLE		0

#define	MAX_BUSY_COUNT	40//10			// Nunber of retry before failing access BBP & RF indirect register

NTSTATUS HwHal_ReadMacRegisters( IN RTMP_ADAPTER *dx, ULONG Offset, ULONG Length, ULONG* pData);
NTSTATUS HwHal_WriteMacRegisters( IN RTMP_ADAPTER *dx, ULONG Offset, ULONG Length, ULONG* pData);

NTSTATUS HwHal_ReadEeRegister_u8( IN RTMP_ADAPTER *dx, USHORT Offset, UCHAR* pucData);
NTSTATUS HwHal_WriteEeRegister_u8( IN RTMP_ADAPTER *dx, USHORT Offset, UCHAR* pucData);
NTSTATUS HwHal_ReadEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData);
NTSTATUS HwHal_WriteEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData);

NTSTATUS HwHal_ReadBbpRegister( IN RTMP_ADAPTER *dx, UCHAR Id, UCHAR* pValue);
NTSTATUS HwHal_WriteBbpRegister( IN RTMP_ADAPTER *dx, UCHAR Id, UCHAR Value);

NTSTATUS HwHal_WriteRfRegister( IN RTMP_ADAPTER *dx, IN ULONG Value);

NTSTATUS HwHal_EraseNorFlashBlock( IN RTMP_ADAPTER *dx, IN ULONG ValueAndIndex);
NTSTATUS HwHal_WriteNorFlash( IN RTMP_ADAPTER *dx, IN ULONG ValueAndIndex, UCHAR* pData, USHORT Length);
NTSTATUS HwHal_ReadNorFlash( IN RTMP_ADAPTER *dx, IN ULONG ValueAndIndex, UCHAR* pData, USHORT Length);

NTSTATUS HwHal_EraseNandFlashBlock( IN RTMP_ADAPTER *dx, IN USHORT BlockNumber);

NTSTATUS HwHal_DeleteRxPendingPacketsInAsic( IN RTMP_ADAPTER *dx);


NTSTATUS HwHal_SetChannel( IN RTMP_ADAPTER *dx, IN ULONG Channel);
NTSTATUS HwHal_SetTxPower( IN RTMP_ADAPTER *dx, IN CHAR TxPower);
NTSTATUS HwHal_SetRxAntenna( IN RTMP_ADAPTER *dx, ULONG Antenna);
NTSTATUS HwHal_SetTxAntenna( IN RTMP_ADAPTER *dx, ULONG Antenna);
NTSTATUS HwHal_SetRfAntenna( IN RTMP_ADAPTER *dx, ULONG Antenna);

NTSTATUS HwHal_SetFrequencyOffset( IN RTMP_ADAPTER *dx, IN UCHAR FrequencyOffset);

NTSTATUS HwHal_SetStartContiTx( IN RTMP_ADAPTER *dx);
NTSTATUS HwHal_SetStartCarrierTestTx( IN RTMP_ADAPTER *dx);
NTSTATUS HwHal_SetStartCarrierSuppressionTx( IN RTMP_ADAPTER *dx);

NTSTATUS HwHal_SetStopTx( IN RTMP_ADAPTER *dx);
NTSTATUS HwHal_SetStartRx( IN RTMP_ADAPTER *dx);
NTSTATUS HwHal_SetStopRx( IN RTMP_ADAPTER *dx);

//NTSTATUS HwHal_SetRadarDetection( IN RTMP_ADAPTER *dx, IN ULONG RadarDetection);
//ULONG HwHal_GetRadarSignal(IN	PEXTENDED_PCI_DATA	dx);


NTSTATUS HwHal_FwDisconnect( IN RTMP_ADAPTER *dx);
NTSTATUS HwHal_FwMemExternal( IN RTMP_ADAPTER *dx);
NTSTATUS HwHal_FwMemInternal( IN RTMP_ADAPTER *dx);

NTSTATUS HwHal_BugFix_MacInitValue( IN RTMP_ADAPTER *dx);
NTSTATUS HwHal_BugFix_BbpInitValue( IN RTMP_ADAPTER *dx);
NTSTATUS HwHal_BugFix_RfInitValue( IN RTMP_ADAPTER *dx);
NTSTATUS HwHal_BugFix_EepromInitValue( IN RTMP_ADAPTER *dx);

#ifdef _USB
//NTSTATUS HwHal_BbpTuning( IN RTMP_ADAPTER *dx, ULONG IsNeedTuning);
//USB
//read
NTSTATUS USBHwHal_ReadMacRegister( IN RTMP_ADAPTER *dx, ULONG Offset, ULONG* pValue);
NTSTATUS USBHwHal_ReadMacRegister47( IN RTMP_ADAPTER *dx, USHORT Offset, ULONG* pValue);
NTSTATUS USBHwHal_ReadMacRegister63( IN RTMP_ADAPTER *dx, USHORT HighByte, USHORT LowByte, ULONG* pValue,ULONG Length);
//write
NTSTATUS USBHwHal_WriteMacRegister66( IN RTMP_ADAPTER *dx, USHORT HighByte,USHORT LowByte, ULONG Value);
NTSTATUS USBHwHal_WriteMacRegister( IN RTMP_ADAPTER *dx, ULONG addr, ULONG Value);

//s read
NTSTATUS USBHwHal_ReadMacRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, ULONG* pData);
NTSTATUS USBHwHal_ReadMacRegisters47( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, ULONG* pData);
NTSTATUS USBHwHal_ReadMacRegisters63( IN RTMP_ADAPTER *dx, USHORT HighByte,USHORT LowByte, USHORT Length, ULONG* pData);

//s write
NTSTATUS USBHwHal_WriteMacRegisters66( IN RTMP_ADAPTER *dx, USHORT HighByte,USHORT LowByte, USHORT Length, ULONG* pData);
//eeprom
NTSTATUS HwHal_USBReadEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData);
NTSTATUS HwHal_USBWriteEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData);

#endif
#endif
