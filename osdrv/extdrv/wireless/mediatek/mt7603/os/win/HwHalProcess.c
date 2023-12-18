#include "config.h"


ULONG g_RfRxAntennaSelect = 0;

UINT32 mt_mac_cr_range[] = {	
	0x60000000, 0x20000, 0x200, /* WF_CFG */	
		0x60100000, 0x21000, 0x200, /* WF_TRB */	
		0x60110000, 0x21200, 0x200, /* WF_AGG */	
		0x60120000, 0x21400, 0x200, /* WF_ARB */	
		0x60130000, 0x21600, 0x200, /* WF_TMAC */	
		0x60140000, 0x21800, 0x200, /* WF_RMAC */	
		0x60150000, 0x21A00, 0x200, /* WF_SEC */	
		0x60160000, 0x21C00, 0x200, /* WF_DMA */	
		0x60170000, 0x21E00, 0x200, /* WF_CFGOFF */	
		0x60180000, 0x22000, 0x1000, /* WF_PF */	
		0x60190000, 0x23000, 0x200, /* WF_WTBLOFF */	
		0x601A0000, 0x23200, 0x200, /* WF_ETBF */		
		0x60300000, 0x24000, 0x400, /* WF_LPON */	
		0x60310000, 0x24400, 0x200, /* WF_INT */	
		0x60320000, 0x28000, 0x4000, /* WF_WTBLON */	
		0x60330000, 0x2C000, 0x200, /* WF_MIB */	
		0x60400000, 0x2D000, 0x200, /* WF_AON */	
		0x80020000, 0x00000, 0x2000, /* TOP_CFG */		
		0x80000000, 0x02000, 0x2000, /* MCU_CFG */	
		0x50000000, 0x04000, 0x4000, /* PDMA_CFG */	
		0xA0000000, 0x08000, 0x8000, /* PSE_CFG */	
		0x60200000, 0x10000, 0x10000, /* WF_PHY */		
		0x0, 0x0, 0x0,};

UINT32 mt_physical_addr_map(UINT32 addr)
{	
	UINT32 global_addr = 0x0, idx = 1;	
	extern UINT32 mt_mac_cr_range[];	
	if (addr < 0x2000)		
		global_addr = 0x80020000 + addr;	
	else if ((addr >= 0x2000) && (addr < 0x4000))		
		global_addr = 0x80000000 + addr - 0x2000;	
	else if ((addr >= 0x4000) && (addr < 0x8000))		
		global_addr = 0x50000000 + addr - 0x4000;	
	else if ((addr >= 0x8000) && (addr < 0x10000))		
		global_addr = 0xa0000000 + addr - 0x8000;	
	else if ((addr >= 0x10000) && (addr < 0x20000))		
		global_addr = 0x60200000 + addr - 0x10000;	
	else if ((addr >= 0x20000) && (addr < 0x40000))	
	{		
		do 
		{			
			if ((addr >= mt_mac_cr_range[idx]) && (addr < (mt_mac_cr_range[idx]+mt_mac_cr_range[idx+1]))) 
			{				
				global_addr = mt_mac_cr_range[idx-1]+(addr-mt_mac_cr_range[idx]);				
				break;			
			}			
			idx += 3;		
		}while (mt_mac_cr_range[idx] != 0);		
		if (mt_mac_cr_range[idx] == 0)		
		{			
			DBGPRINT(RT_DEBUG_TRACE,("%s unknow addr range = %x !!!\n",__FUNCTION__));					
		}	
	}	
	else		
		global_addr = addr;	

	return global_addr;
}


NTSTATUS HwHal_ReadMacRegisters( IN RTMP_ADAPTER *dx, ULONG Offset, ULONG Length, ULONG* pData)
{
	USHORT i;
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadMacRegisters ===>\n"));
	for(i=0; i<Length/4; i++)
	{
		RTMP_IO_READ32(dx, Offset+i*4, (pData+i));
	}

	return STATUS_SUCCESS;
}


NTSTATUS HwHal_WriteMacRegisters( IN RTMP_ADAPTER *dx, ULONG Offset, ULONG Length, ULONG* pData)
{
	USHORT i;
	
	for(i=0; i<Length/4; i++)
	{
		RTMP_IO_WRITE32(dx, Offset+i*4, *(pData+i));
	}

	return STATUS_SUCCESS;
}


NTSTATUS HwHal_ReadEeRegister_u8( IN RTMP_ADAPTER *dx, USHORT Offset, UCHAR* pucData)
{
	NTSTATUS	ntStatus = STATUS_SUCCESS;
	USHORT		innerOffset;
	USHORT		innerLength;
	USHORT		innerData;	

	innerOffset = (Offset/2)*2;
	innerLength = 2;
	
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadEeRegister_u8()>>>"));
	
	HwHal_ReadEeRegisters(dx, innerOffset, innerLength, &innerData);

	if((Offset%2) == 0)
	{
		*pucData = (UCHAR)(innerData&0x00FF);
	}
	else
	{
		*pucData = (UCHAR)((innerData&0xFF00)>>8);	
	}

	DBGPRINT(RT_DEBUG_TRACE,("Offset = 0x%04x, pData[0]=0x%02x", Offset, pucData[0]));
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadEeRegister_u8()<<<"));
	
	return ntStatus;
}


NTSTATUS HwHal_WriteEeRegister_u8( IN RTMP_ADAPTER *dx, USHORT Offset, UCHAR* pucData)
{
	NTSTATUS	ntStatus = STATUS_SUCCESS;
	USHORT		innerOffset;
	USHORT		innerLength;
	USHORT		innerData;	
	USHORT		TempSrcData;

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteEeRegister_u8()>>>"));
	DBGPRINT(RT_DEBUG_TRACE,("Offset = 0x%04x, pData[0]=0x%04x", Offset, pucData[0]));

	innerOffset = (Offset/2)*2;
	innerLength = 2;

	HwHal_ReadEeRegisters(dx, innerOffset, innerLength, &innerData);

	TempSrcData = (USHORT)(*pucData);
	if((Offset%2) == 0)
	{
		innerData = (innerData&0xFF00 | (TempSrcData));
	}
	else
	{
		innerData = (innerData&0x00FF | (TempSrcData<<8));
	}
	
	HwHal_WriteEeRegisters(dx, innerOffset, innerLength, &innerData);
	



	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteEeRegisters()<<<"));
	
	return ntStatus;
}


#ifdef _USB
NTSTATUS HwHal_USBWriteEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData)
{
	NTSTATUS	ntStatus;

//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteEeRegisters()>>>\n");
//	DBGPRINT(RT_DEBUG_TRACE,("Offset = 0x%04x, , Length = 0x%04x, pData[0]=0x%04x\n", Offset, Length, pData[0]);

	// Allocate memory for URB
	USHORT UrbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	PURB urb = (PURB)ExAllocatePool(NonPagedPool, UrbSize);
	if( urb==NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("No URB memory\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

//	DBGPRINT(RT_DEBUG_TRACE,("DeviceIoControl: IOCTL_USBQA_USB_EEPROM_WRITE\n");
	// Build URB to send vendor control request on Default pipe
	UsbBuildVendorRequest(urb,
		URB_FUNCTION_VENDOR_DEVICE, UrbSize,
		USBD_TRANSFER_DIRECTION_OUT,	// Direction out
		DEVICE_VENDOR_REQUEST_OUT,						// Reserved bits
		0x8,						// Request
		0x0,					// Value
		Offset,			// index
		pData, NULL, Length,				// Output data
		NULL);

	// Call the USB driver
	ntStatus = CallUSBDI( dx, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, 0);
	// Check statuses
	if( !NT_SUCCESS(ntStatus) || !USBD_SUCCESS( urb->UrbHeader.Status))
	{
		DBGPRINT(RT_DEBUG_TRACE,("status %x URB status %x\n", ntStatus, urb->UrbHeader.Status));
		ntStatus = STATUS_UNSUCCESSFUL;
	}
	ExFreePool(urb);

//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteEeRegisters()<<<\n");
	
	return ntStatus;
}


NTSTATUS HwHal_USBReadEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData)
{
	NTSTATUS	ntStatus;

//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadEeRegisters()>>>\n");
	
	// Allocate memory for URB
	USHORT UrbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	PURB urb = (PURB)ExAllocatePool(NonPagedPool, UrbSize);
	if( urb==NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("No URB memory\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Build URB to send vendor control request on Default pipe
	UsbBuildVendorRequest(urb,
		URB_FUNCTION_VENDOR_DEVICE, UrbSize,
		USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,	// Direction out
		DEVICE_VENDOR_REQUEST_IN,						// Reserved bits
		0x9,						// Request
		0x0,					// Value
		Offset,			// index
		pData, NULL, Length,				// Output data
		NULL);

	// Call the USB driver
	ntStatus = CallUSBDI( dx, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, 0);
	// Check statuses
	if( !NT_SUCCESS(ntStatus) || !USBD_SUCCESS( urb->UrbHeader.Status))
	{
		DBGPRINT(RT_DEBUG_TRACE,("status %x URB status %x\n", ntStatus, urb->UrbHeader.Status));
		ntStatus = STATUS_UNSUCCESSFUL;
	}
	ExFreePool(urb);

//	DBGPRINT(RT_DEBUG_TRACE,("Offset = 0x%04x, , Length = 0x%04x, pData[0]=0x%04x\n", Offset, Length, pData[0]);
//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadEeRegisters()<<<\n");
	
	return ntStatus;
}
#endif


#ifdef RTMP_PCI_SUPPORT
NTSTATUS HwHal_PCIReadEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData)
{
	USHORT i;

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadEeRegisters()>>>"));
	for(i=0; i<Length/2; i++)
	{
		*(pData+i) = ReadEEprom(dx, Offset/2+i, dx->ADDRESSSIZE);
	}

	if (dx->AntennaDiversity)
	{
		ULONG	Value = 0xFFFFFFFF;
		//RTMP_IO_READ32(dx, RA_EE_CTRL,&Value);
		Value &= ~EESK;
		Value |= dx->SelAnt;
		//RTMP_Real_IO_WRITE32(dx, RA_EE_CTRL, Value);
	}

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadEeRegisters()<<<"));
	
	return STATUS_SUCCESS;
}


NTSTATUS HwHal_PCIWriteEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData)
{
	USHORT i;

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteEeRegisters()>>>"));
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteEeRegisters() Offset = %x, Length =%x", Offset, Length));	
	for(i=0; i<Length/2; i++)
	{
		WriteEEprom(dx, Offset/2+i, dx->ADDRESSSIZE, *(pData+i));
	}

	if (dx->AntennaDiversity)
	{
		ULONG	Value = 0xFFFFFFFF;
		//RTMP_IO_READ32(dx, RA_EE_CTRL,&Value);
		Value &= ~EESK;
		Value |= dx->SelAnt;
		//RTMP_Real_IO_WRITE32(dx, RA_EE_CTRL, Value);
	}

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteEeRegisters()<<<"));
	
	return STATUS_SUCCESS;
}
#endif /* RTMP_PCI_SUPPORT */


NTSTATUS HwHal_ReadEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData)
{
#ifdef _USB
	return HwHal_USBReadEeRegisters(dx, Offset, Length, pData);
#endif /* _USB */

#ifdef RTMP_PCI_SUPPORT
	return HwHal_PCIReadEeRegisters(dx, Offset, Length, pData);
#endif /* RTMP_PCI_SUPPORT */

	return STATUS_SUCCESS;
}


NTSTATUS HwHal_WriteEeRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, USHORT* pData)
{
#ifdef _USB
	return HwHal_USBWriteEeRegisters(dx, Offset, Length, pData);
#endif /* _USB */

#ifdef RTMP_PCI_SUPPORT
	return HwHal_PCIWriteEeRegisters(dx, Offset, Length, pData);
#endif /* RTMP_PCI_SUPPORT */

	return STATUS_SUCCESS;
}


NTSTATUS HwHal_ReadBbpRegister( IN RTMP_ADAPTER *dx, UCHAR Id, UCHAR* pValue)
{
	ULONG	i = 0;
	BBPCSR_STRUC	BbpCsrStruct;

	static const ULONG busy = 1;
	static const ULONG retry_limit = 10;

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadBbpRegister()>>>\n"));

	BbpCsrStruct.Group= 0;
	BbpCsrStruct.field.Busy = 1;
	BbpCsrStruct.field.Value = 0;
	BbpCsrStruct.field.WriteControl = 1;
	BbpCsrStruct.field.RegNum = Id;
	
	//RTMP_IO_WRITE32(dx, RA_BBP_CSR_CFG, BbpCsrStruct.Group);
	do
	{
		//RTMP_IO_READ32(dx, RA_BBP_CSR_CFG, &BbpCsrStruct.Group);

		DBGPRINT(RT_DEBUG_TRACE,("Bbp Debug1 Value = 0x%02x\n", BbpCsrStruct.field.Value));
		
		if (!(BbpCsrStruct.field.Busy & busy))
			break;
		i++;
	}
	while (i < retry_limit);

	if (i == retry_limit)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Retry Count Exhausted!!!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	//RTMP_IO_READ32(dx, RA_BBP_CSR_CFG, &BbpCsrStruct.Group);
	DBGPRINT(RT_DEBUG_TRACE,("Bbp Debug2 Value = 0x%02x\n", BbpCsrStruct.field.Value));
	
	*pValue = (UCHAR)BbpCsrStruct.field.Value;
	
	return STATUS_SUCCESS;
}


NTSTATUS HwHal_WriteBbpRegister( IN RTMP_ADAPTER *dx, UCHAR Id, UCHAR Value)
{
	ULONG	i = 0;
	BBPCSR_STRUC	BbpCsrStruct;

	static const ULONG busy = 1;
	static const ULONG retry_limit = 10;


	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteBbpRegister()>>>\n"));
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteBbpRegister() Id = %d, Value = 0x%x\n", Id, Value));

	BbpCsrStruct.Group= 0;
	BbpCsrStruct.field.Busy = 1;	
	BbpCsrStruct.field.WriteControl = 0;
	BbpCsrStruct.field.RegNum = Id;
	BbpCsrStruct.field.Value = Value;
	//RTMP_IO_WRITE32(dx, RA_BBP_CSR_CFG, BbpCsrStruct.Group);

	do
	{
		//RTMP_IO_READ32(dx, RA_BBP_CSR_CFG, &BbpCsrStruct.Group);
		if (!(BbpCsrStruct.field.Busy & busy))
			break;
		i++;
	}
	while (i < retry_limit);

	if (i == retry_limit)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Retry Count Exhausted!!!\n"));
		return STATUS_UNSUCCESSFUL;
	}
	
	return STATUS_SUCCESS;	
}

NTSTATUS HwHal_WriteRfRegister( IN RTMP_ADAPTER *dx, IN ULONG Value)
{
	USHORT	temp;
	USHORT	i = 0;
	RFCSR_STRUC	RfCsrStruct;
	//static const USHORT busy = 1;
	static const USHORT retry_limit = 10;

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteRfRegister()>>>\n"));
	DBGPRINT(RT_DEBUG_TRACE,("RfWValue=0x%08x\n", Value));

	RtlZeroMemory(&RfCsrStruct,sizeof(RfCsrStruct));

	RTMPusecDelay(200);

	do
	{
		//RTMP_IO_READ32(dx, RA_RF_CSR_CFG0, &RfCsrStruct.Group);
		if (!(RfCsrStruct.field.Busy))
			break;
		i++;
	}
	while (i < retry_limit);

	if (i == retry_limit)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Retry Count Exhausted!!!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	//RTMP_IO_WRITE32(dx, RA_RF_CSR_CFG0, Value);
	
	return STATUS_SUCCESS;

}

NTSTATUS HwHal_SetChannel( IN RTMP_ADAPTER *dx, IN ULONG Channel)
{
	return STATUS_SUCCESS;					
}

VOID HwHal_LockChannel( IN RTMP_ADAPTER *dx, IN UCHAR Channel)
{
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_LockChannel()>>>"));
}

NTSTATUS HwHal_SetTxPower( IN RTMP_ADAPTER *dx, IN CHAR TxPower)
{
	return STATUS_SUCCESS;			
}

NTSTATUS HwHal_SetRxAntenna( IN RTMP_ADAPTER *dx, ULONG Antenna)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_SetRxAntenna()>>>"));

	return ntStatus;		
}

NTSTATUS HwHal_SetTxAntenna( IN RTMP_ADAPTER *dx, ULONG Antenna)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_SetTxAntenna()>>>\n"));

#ifdef RTMP_PCI_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_SetTxAntenna()>>> RfType = %d\n", dx->PortCfg.RfType));
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_SetTxAntenna()>>> NumberOfAntenna = %d\n", dx->PortCfg.NumberOfAntenna));
#endif /* RTMP_PCI_SUPPORT */
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_SetTxAntenna()>>> Antenna = %d\n", Antenna));



	return ntStatus;		
}

NTSTATUS HwHal_SetRfAntenna( IN RTMP_ADAPTER *dx, ULONG Antenna)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;

	DBGPRINT(RT_DEBUG_TRACE,("HwHal_SetRfAntenna()>>>"));

	g_RfRxAntennaSelect = Antenna;
		
	return ntStatus;		
}

NTSTATUS HwHal_SetFrequencyOffset( IN RTMP_ADAPTER *dx, IN UCHAR FrequencyOffset)
{
	DBGPRINT(RT_DEBUG_TRACE,("HwHal_SetFrequencyOffset()>>>"));

	return STATUS_SUCCESS;			
}

#ifdef _USB
//USB
NTSTATUS USBHwHal_ReadMacRegister( IN RTMP_ADAPTER *dx, ULONG Offset, ULONG* pValue)
{	
	NTSTATUS	ntStatus;
	USHORT HighByte = 0;
	USHORT LowByte =0;

	DBGPRINT(RT_DEBUG_TRACE,("%s() >>>\n",__FUNCTION__));
	//ntStatus = USBHwHal_ReadMacRegisters(dx, Offset, 4, pValue);
	HighByte = (Offset&0xFFFF0000)>>16;
	LowByte = Offset&0x0000FFFF;
	ntStatus = USBHwHal_ReadMacRegister63(dx,HighByte,LowByte,pValue,4);
	
	DBGPRINT(RT_DEBUG_TRACE,("%s()<<<\n",__FUNCTION__));
		
	return ntStatus;
}


NTSTATUS USBHwHal_ReadMacRegister47( IN RTMP_ADAPTER *dx, USHORT Offset, ULONG* pValue)
{
	NTSTATUS	ntStatus;

	DBGPRINT(RT_DEBUG_TRACE,("%s()>>>\n",__FUNCTION__));

	ntStatus = USBHwHal_ReadMacRegisters47(dx, Offset, 4, pValue);

	DBGPRINT(RT_DEBUG_TRACE,("%s()<<<\n",__FUNCTION__));
		
	return ntStatus;
}


NTSTATUS USBHwHal_ReadMacRegister63( IN RTMP_ADAPTER *dx, USHORT HighByte, USHORT LowByte, ULONG* pValue,ULONG Length)
{	
	NTSTATUS	ntStatus;

	DBGPRINT(RT_DEBUG_TRACE,("%s()>>>\n",__FUNCTION__));

	ntStatus = USBHwHal_ReadMacRegisters63(dx, HighByte,LowByte, (USHORT)Length, pValue);

	DBGPRINT(RT_DEBUG_TRACE,("%s()<<<\n",__FUNCTION__));
		
	return ntStatus;
}


NTSTATUS USBHwHal_ReadMacRegisters( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, ULONG* pData)
{
	NTSTATUS	ntStatus = 0;

//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadMacRegisters()>>>\n");

	// Allocate memory for URB
	USHORT UrbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	PURB urb = (PURB)ExAllocatePool(NonPagedPool, UrbSize);
	if( urb==NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("No URB memory\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Build URB to send vendor control request on Default pipe
	UsbBuildVendorRequest(urb,
		URB_FUNCTION_VENDOR_DEVICE, UrbSize,
		USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,	// Direction out
		DEVICE_VENDOR_REQUEST_IN,						// Reserved bits
		0x7,						// Request
		0x0,					// Value
		Offset,			// index
		(USHORT*)pData, NULL, Length,				// Output data
		NULL);

	// Call the USB driver
	ntStatus = CallUSBDI( dx, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, 0);
	// Check statuses
	if( !NT_SUCCESS(ntStatus) || !USBD_SUCCESS( urb->UrbHeader.Status))
	{
		DBGPRINT(RT_DEBUG_TRACE,("status %x URB status %x\n", ntStatus, urb->UrbHeader.Status));
		ntStatus = STATUS_UNSUCCESSFUL;
	}
	ExFreePool(urb);

	return ntStatus;
}


NTSTATUS USBHwHal_ReadMacRegisters47( IN RTMP_ADAPTER *dx, USHORT Offset, USHORT Length, ULONG* pData)
{
	NTSTATUS	ntStatus;

//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadMacRegisters()>>>\n");

	// Allocate memory for URB
	USHORT UrbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	PURB urb = (PURB)ExAllocatePool(NonPagedPool, UrbSize);
	if( urb==NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("No URB memory\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Build URB to send vendor control request on Default pipe
	UsbBuildVendorRequest(urb,
		URB_FUNCTION_VENDOR_DEVICE, UrbSize,
		USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,	// Direction out
		DEVICE_VENDOR_REQUEST_IN,						// Reserved bits
		0x47,						// Request
		0x0,					// Value
		Offset,			// index
		(USHORT*)pData, NULL, Length,				// Output data
		NULL);

	// Call the USB driver
	ntStatus = CallUSBDI( dx, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, 0);
	// Check statuses
	if( !NT_SUCCESS(ntStatus) || !USBD_SUCCESS( urb->UrbHeader.Status))
	{
		DBGPRINT(RT_DEBUG_TRACE,("status %x URB status %x\n", ntStatus, urb->UrbHeader.Status));
		ntStatus = STATUS_UNSUCCESSFUL;
	}
	ExFreePool(urb);

//	DBGPRINT(RT_DEBUG_TRACE,("Offset = 0x%04x, , Length = 0x%04x, pData[0]=0x%08x\n", Offset, Length, pData[0]);
//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadMacRegisters()<<<\n");
	
	return ntStatus;
}


NTSTATUS USBHwHal_ReadMacRegisters63( IN RTMP_ADAPTER *dx, USHORT HighByte,USHORT LowByte, USHORT Length, ULONG* pData)
{
	NTSTATUS	ntStatus;
	USHORT UrbSize;
	PURB urb;

	DBGPRINT(RT_DEBUG_TRACE,("%s HighByte=0x%08x LowByte=0x%08x,Length =%d\n",__FUNCTION__,HighByte,LowByte,Length));

	// Allocate memory for URB
	UrbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	urb = (PURB)ExAllocatePool(NonPagedPool, UrbSize);
	if( urb==NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s No URB memory\n",__FUNCTION__));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Build URB to send vendor control request on Default pipe
	UsbBuildVendorRequest(urb,
		URB_FUNCTION_VENDOR_DEVICE, UrbSize,
		USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,	// Direction out
		DEVICE_VENDOR_REQUEST_IN,						// Reserved bits
		0x63,						// Request
		HighByte,					// Value
		LowByte,			// index
		(USHORT*)pData, NULL, Length,				// Output data
		NULL);

	// Call the USB driver
	ntStatus = CallUSBDI( dx, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, 0);
	//DBGPRINT(RT_DEBUG_TRACE,("%s HighByte=0x%08x LowByte=0x%08x  get pData=%08x\n",__FUNCTION__,HighByte,LowByte,*pData);
	// Check statuses
	if( !NT_SUCCESS(ntStatus) || !USBD_SUCCESS( urb->UrbHeader.Status))
	{
		DBGPRINT(RT_DEBUG_TRACE,("status %x URB status %x\n", ntStatus, urb->UrbHeader.Status));
		ntStatus = STATUS_UNSUCCESSFUL;
	}
	ExFreePool(urb);

//	DBGPRINT(RT_DEBUG_TRACE,("Offset = 0x%04x, , Length = 0x%04x, pData[0]=0x%08x\n", Offset, Length, pData[0]);
//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_ReadMacRegisters()<<<\n");
	
	return ntStatus;

}


NTSTATUS USBHwHal_WriteMacRegister( IN RTMP_ADAPTER *dx, ULONG addr, ULONG Value)
{
	NTSTATUS	ntStatus;
	USHORT HighByte = 0;
	USHORT LowByte =0;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));

	HighByte = (addr&0xFFFF0000)>>16;
	LowByte = addr&0x0000FFFF;
	ntStatus = USBHwHal_WriteMacRegister66(dx,HighByte,LowByte,Value);	
	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));
	return ntStatus;
}


NTSTATUS USBHwHal_WriteMacRegister66( IN RTMP_ADAPTER *dx, USHORT HighByte,USHORT LowByte, ULONG Value)
{
	NTSTATUS	ntStatus;

	DBGPRINT(RT_DEBUG_TRACE,("%s -->\n",__FUNCTION__));
	DBGPRINT(RT_DEBUG_TRACE,("%s HighByte=0x%08x LowByte=0x%08x set=0x%x\n",__FUNCTION__,HighByte,LowByte,Value));
	ntStatus = USBHwHal_WriteMacRegisters66(dx, HighByte,LowByte, 4, &Value);

	DBGPRINT(RT_DEBUG_TRACE,("%s <--\n",__FUNCTION__));
			
	return ntStatus;
}


NTSTATUS USBHwHal_WriteMacRegisters66( IN RTMP_ADAPTER *dx, USHORT HighByte,USHORT LowByte, USHORT Length, ULONG* pData)
{
	NTSTATUS	ntStatus;
	USHORT UrbSize;
	PURB urb;

//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteMacRegisters()>>>\n");
//	DBGPRINT(RT_DEBUG_TRACE,("Offset = 0x%04x, , Length = 0x%04x, pData[0]=0x%08x\n", Offset, Length, pData[0]);

	// Allocate memory for URB
	UrbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	urb = (PURB)ExAllocatePool(NonPagedPool, UrbSize);
	if( urb==NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s No URB memory\n",__FUNCTION__));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Build URB to send vendor control request on Default pipe
	UsbBuildVendorRequest(urb,
		URB_FUNCTION_VENDOR_DEVICE, UrbSize,
		USBD_TRANSFER_DIRECTION_OUT,	// Direction out
		DEVICE_VENDOR_REQUEST_OUT,						// Reserved bits
		0x66,						// Request
		HighByte,					// Value
		LowByte,			// index
		pData, NULL, Length,				// Output data
		NULL);

	//DBGPRINT(RT_DEBUG_TRACE,("%s HighByte=0x%08x LowByte=0x%08x set=0x%x\n",__FUNCTION__,HighByte,LowByte,*pData);
	// Call the USB driver
	ntStatus = CallUSBDI( dx, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, 0);
	// Check statuses
	if( !NT_SUCCESS(ntStatus) || !USBD_SUCCESS( urb->UrbHeader.Status))
	{
		DBGPRINT(RT_DEBUG_TRACE,("status %x URB status %x\n", ntStatus, urb->UrbHeader.Status));
		ntStatus = STATUS_UNSUCCESSFUL;
	}
	ExFreePool(urb);

//	DBGPRINT(RT_DEBUG_TRACE,("HwHal_WriteMacRegisters()<<<\n");
	
	return ntStatus;
}

NTSTATUS RTMP_IO_READ32(IN RTMP_ADAPTER *deviceExtension, ULONG addr, ULONG *Value)
{
	NTSTATUS	ntStatus = STATUS_UNSUCCESSFUL;
	UINT32 PhysicalAddr = 0;
	
	PhysicalAddr = mt_physical_addr_map(addr);
	DBGPRINT(RT_DEBUG_TRACE,("%s()Convert to Physical addr 0x%08x\n",__FUNCTION__, PhysicalAddr));
	
	USBHwHal_ReadMacRegister(deviceExtension,PhysicalAddr,Value);
	return ntStatus;

}
NTSTATUS RTMP_IO_WRITE32(IN RTMP_ADAPTER *deviceExtension, ULONG addr, ULONG Value)
{
	NTSTATUS	ntStatus = STATUS_UNSUCCESSFUL;
	UINT32 PhysicalAddr = 0;
	
	PhysicalAddr = mt_physical_addr_map(addr);
	DBGPRINT(RT_DEBUG_TRACE,("%s()Convert to Physical addr 0x%08x\n",__FUNCTION__, PhysicalAddr));
		
	USBHwHal_WriteMacRegister(deviceExtension,PhysicalAddr,Value);
	return ntStatus;
}
#endif
