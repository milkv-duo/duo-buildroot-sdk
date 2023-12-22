/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

 	Module Name:
	rtusb_io.c

	Abstract:

	Revision History:
	Who			When	    What
	--------	----------  ----------------------------------------------
	Name		Date	    Modification logs
*/

#ifdef RTMP_MAC_USB


#include	"rt_config.h"

/*
	========================================================================

	Routine Description: NIC initialization complete

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
#ifdef RTMP_MAC
void usb_cfg_read_v1(RTMP_ADAPTER *ad, UINT32 *value)
{
	RTMP_IO_READ32(ad, RTMP_USB_DMA_CFG, value);
}


// For MT76x0 and older
void usb_cfg_write_v1(RTMP_ADAPTER *ad, UINT32 value)
{
	RTUSBWriteMACRegister(ad, RTMP_USB_DMA_CFG, value, FALSE);
}
#endif /* RTMP_MAC */

#ifdef RLT_MAC
// For MT76x0 and older
void usb_cfg_read_v2(RTMP_ADAPTER *ad, UINT32 *value)
{
	RTMP_IO_READ32(ad, RLT_USB_DMA_CFG, value);
}


// For MT76x0 and older
void usb_cfg_write_v2(RTMP_ADAPTER *ad, UINT32 value)
{
	RTUSBWriteMACRegister(ad, RLT_USB_DMA_CFG, value, FALSE);
}


#ifdef MT76x2
// For MT7662 and newer
void usb_cfg_read_v3(RTMP_ADAPTER *ad, UINT32 *value)
{
	int ret;
	UINT32 io_value;

	ret = RTUSB_VendorRequest(ad,
							  (USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK),
							  DEVICE_VENDOR_REQUEST_IN,
							  0x47,
							  0,
							  U3DMA_WLCFG,
							  &io_value,
							  4);

	*value = le2cpu32(io_value);

	if (ret)
		*value = 0xffffffff;
}

// For MT7662 and newer
void usb_cfg_write_v3(RTMP_ADAPTER *ad, UINT32 value)
{
	int ret;
	UINT32 io_value;


	io_value = cpu2le32(value);

	ret = RTUSB_VendorRequest(ad,
							  USBD_TRANSFER_DIRECTION_OUT,
							  DEVICE_VENDOR_REQUEST_OUT,
							  0x46,
							  0,
							  U3DMA_WLCFG,
							  &io_value,
							  4);


	if (ret) {
		DBGPRINT(RT_DEBUG_ERROR, ("usb cfg write fail\n"));
		return;
	}
}
#endif /* MT76x2 */
#endif /* RLT_MAC */

static NTSTATUS RTUSBFirmwareRun(RTMP_ADAPTER *pAd)
{
	NTSTATUS Status;

	Status = RTUSB_VendorRequest(
		pAd,
		USBD_TRANSFER_DIRECTION_OUT,
		DEVICE_VENDOR_REQUEST_OUT,
		0x01,
		0x8,
		0,
		NULL,
		0);

	return Status;
}



/*
	========================================================================

	Routine Description: Get current firmware operation mode (Return Value)

	Arguments:

	Return Value:
		0 or 1 = Downloaded by host driver
		others = Driver doesn't download firmware

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBFirmwareOpmode(RTMP_ADAPTER *pAd, UINT32 *pValue)
{
	NTSTATUS	Status;

	Status = RTUSB_VendorRequest(
		pAd,
		(USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK),
		DEVICE_VENDOR_REQUEST_IN,
		0x1,
		0x11,
		0,
		pValue,
		4);
	return Status;
}


/*
	========================================================================

	Routine Description: Write Firmware to NIC.

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBFirmwareWrite(RTMP_ADAPTER *pAd, UCHAR *pFwImage, ULONG FwLen)
{
	UINT32 MacReg;
	NTSTATUS Status;
	USHORT writeLen;

	Status = RTMP_IO_READ32(pAd, MAC_CSR0, &MacReg);

#if 0
	/* the method has side effect so disable it.*/
	RTUSBFirmwareOpmode(pAd,&FMode);
	DBGPRINT(RT_DEBUG_ERROR, ("====> NICFirmwareMode=%lx\n", FMode));

	/* if FMode == 3, it means it is in CD-ROM mode, driver should not load firmware, otherwise load the firmware. when RT2870, RT2872*/
	/* if FMode == 2, it means it is in CD-ROM mode, driver should not load firmware, otherwise load the firmware. when RT3070 or later*/
	if ((IS_RT2872(pAd) && ((FMode & 0x3) != 3)) ||
		(!IS_RT2872(pAd) && ((FMode & 0x3) != 2)))
	{
#ifdef RELEASE_EXCLUDE
	/*
		Recorder: Sample Lin
		Co-worker: Albert Yang, Alex Lin, Albert Liu, Shawn Hsiao
		Date: 2009/12/29

		1. Motivation: Large power consumption problem in RT2870 only.

		2. Problem: When we insert/remove rt2870 station and then insert
			rt2870 ap driver, the current will reach 7xx mA, but the maximum
			legal current is 500mA in USB device.

		3. Phenomenon:
			We found 8051 firmware will update 0x1328 to default value 0x0f50f,
			i.e. enable PA and LNA for 2.4G and 5GHz.

			But if AP is located in 2.4GHz, no need to enable PA/LNA for 5GHz;
			if AP is located in 5GHz, no need to enable PA/LNA for 2.4GHz.

		4. Analysis:
			(1) Our driver will change 0x1328 only when channel switch to
				correct value 0x0f50a (2.4GHz) or 0x0f505 (5GHz);
				But the value will be changed to the wrong value 0x0f50f
				after interface up.
			(2) The we found if we issue the MCU command 0x30 in radio off
				function of station driver, the MAC 0x1328 will be changed
				automatically in AP driver.
			(3) Albert Liu said that 8051 only update MAC 0x1328 when it
				receives 0x31 MCU command.
			(4) If we do not upload 8051 firmware in AP driver after station
				driver is removed, no the problem occur.
				So we think that when 8051 is on work, if we upload 8051
				firmware, any unpredicted result will occur.

		5. Solution:
			(1) From Alex:
				Use new firmware and send 0x31 MCU command before channel
				switch. The new firmware will save 0x1328 value when you issue
				0x30 MCU sleep command and restore 0x1328 value when you issue
				0x31 MCU wake up command.
			(2) From Albert Liu: (final solution, no need to patch firmware)
				Or reset MCU before 8051 firmware upload and 8051 will reset
				anything in AP or STA driver for USB device.
				The solution can not be used in PCI/PCIe device because
				USB device has a ROM kept a simple 8051 firmware but no ROM
				on PCI/PCIe device:

				a. When USB up, it will run the 8051 on ROM;
				b. When we issue a reset MCU command, USB will translate
					to the 8051 on ROM if we have ever run 8051 before;
				c. And we upload firmware;
				d. Issue a vendor request 0x01, 0x8 to run the 8051 on RAM;

				So we must reset 8051 before firmware upload.
				But the side effect is we often receive CRC-ERROR packets.
			(3) Modify EEPROM 0x2e1 = 0x80 to disable 8051 recover 0x1328
				value when 1st beacon is sent.
	*/
#endif /* RELEASE_EXCLUDE */
		UINT32 Value, Index;
		UINT32 MacReg = 0;

		/* clear MCU ready bit */
		RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &MacReg);
		MacReg &= (~0x80);
		RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, MacReg);

		/* do MCU reset for 8051 on ROM */
		/*
			Note: For USB device, we have a default 8051 firmware on ROM of USB,
			but it only has simple function, not LED or DFS, etc.
			So we need to load new firmware to the RAM of USB, and change 8051 to
			run on RAM for new firmware.
		*/
		RTMP_IO_READ32(pAd, 0x400, &Value);
		Value |= 0x01;
		RTUSBWriteMACRegister(pAd, 0x400, Value);
		RtmpusecDelay(10000);

		/* check if MCU is ready */
		Index = 0;
		do
		{
			if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
				return NDIS_STATUS_FAILURE;

			RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &MacReg);

			if (MacReg & 0x80)
				break;

			RtmpusecDelay(1000);
		} while (Index++ < 1000);

	    if (Index >= 1000)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s(): MCU is not ready\n\n\n", __FUNCTION__));
			Status = NDIS_STATUS_FAILURE;
		}

		/* write firmware */
		writeLen = FwLen;
		RTUSBMultiWrite(pAd, FIRMWARE_IMAGE_BASE, pFwImage, writeLen);

		Status = RTUSBWriteMACRegister(pAd, 0x7014, 0xffffffff);
		Status = RTUSBWriteMACRegister(pAd, 0x701c, 0xffffffff);
	}
#endif

	/* write firmware */
	writeLen = FwLen;
#ifdef USB_FIRMWARE_MULTIBYTE_WRITE
	DBGPRINT(RT_DEBUG_TRACE, ("USB_FIRMWARE_MULTIBYTE_WRITE defined! Write_Bytes = %d\n", MULTIWRITE_BYTES));
	RTUSBMultiWrite_nBytes(pAd, FIRMWARE_IMAGE_BASE, pFwImage, writeLen, MULTIWRITE_BYTES);
#else
	DBGPRINT(RT_DEBUG_TRACE, ("USB_FIRMWARE_MULTIBYTE_WRITE not defined!\n"));
	RTUSBMultiWrite(pAd, FIRMWARE_IMAGE_BASE, pFwImage, writeLen, FALSE);
#endif
	Status = RTUSBWriteMACRegister(pAd, 0x7014, 0xffffffff, FALSE);
	Status = RTUSBWriteMACRegister(pAd, 0x701c, 0xffffffff, FALSE);

	/* change 8051 from ROM to RAM */
	Status = RTUSBFirmwareRun(pAd);

#if 0 /* Move to back */
	/* The commands to firmware should be after these commands, these commands will init firmware*/
	RtmpusecDelay(10000);
	RTUSBWriteMACRegister(pAd, H2M_BBP_AGENT, 0);	/* initialize BBP R/W access agent*/
	RTUSBWriteMACRegister(pAd,H2M_MAILBOX_CSR,0);
	RTUSBWriteMACRegister(pAd, H2M_INT_SRC, 0);
	AsicSendCommandToMcu(pAd, 0x72, 0x00, 0x00, 0x00, FALSE);/*reset rf by MCU supported by new firmware*/
#endif

	return Status;
}


NTSTATUS RTUSBVenderReset(RTMP_ADAPTER *pAd)
{
	NTSTATUS Status;
	DBGPRINT_RAW(RT_DEBUG_ERROR, ("-->RTUSBVenderReset\n"));
	Status = RTUSB_VendorRequest(
		pAd,
		USBD_TRANSFER_DIRECTION_OUT,
		DEVICE_VENDOR_REQUEST_OUT,
		0x01,
		0x1,
		0,
		NULL,
		0);

	DBGPRINT_RAW(RT_DEBUG_ERROR, ("<--RTUSBVenderReset\n"));
	return Status;
}


/*
	========================================================================

	Routine Description: Read various length data from RT2573

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBMultiRead(RTMP_ADAPTER *pAd, USHORT addr, UCHAR *buf, USHORT len)
{
	return RTUSB_VendorRequest(pAd, (USBD_TRANSFER_DIRECTION_IN |
										USBD_SHORT_TRANSFER_OK),
								DEVICE_VENDOR_REQUEST_IN,
								0x7, 0, addr, buf, len);
}


/*
	========================================================================

	Routine Description: Write various length data to RT USB Wifi device, the maxima length should not large than 65535 bytes.

	Arguments:

	Return Value:

	IRQL =

	Note:
		Use this funciton carefully cause it may not stable in some special USB host controllers.

	========================================================================
*/
NTSTATUS RTUSBMultiWrite_nBytes(
	IN RTMP_ADAPTER *pAd,
	IN USHORT Offset,
	IN UCHAR *buf,
	IN USHORT len,
	IN USHORT batchLen)
{
	NTSTATUS Status = STATUS_SUCCESS;
	USHORT index = Offset, actLen = batchLen, leftLen = len;
	UCHAR *pSrc = buf;


	do
	{
		actLen = (actLen > batchLen ? batchLen : actLen);
		Status = RTUSB_VendorRequest(
			pAd,
			USBD_TRANSFER_DIRECTION_OUT,
			DEVICE_VENDOR_REQUEST_OUT,
			0x6,
			0,
			index,
			pSrc,
			actLen);

		if (Status != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("VendrCmdMultiWrite_nBytes failed!\n"));
			break;
		}

		index += actLen;
		leftLen -= actLen;
		pSrc = pSrc + actLen;
	}while(leftLen > 0);

	return Status;
}


/*
	========================================================================

	Routine Description: Write various length data to RT2573

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBMultiWrite_OneByte(RTMP_ADAPTER *pAd, USHORT Offset, UCHAR *pData)
{
	/* TODO: In 2870, use this funciton carefully cause it's not stable.*/
	return RTUSB_VendorRequest(pAd, USBD_TRANSFER_DIRECTION_OUT,
								DEVICE_VENDOR_REQUEST_OUT,
								0x6, 0, Offset, pData, 1);
}

NTSTATUS RTUSBMultiWrite(
	IN RTMP_ADAPTER *pAd,
	IN USHORT Offset,
	IN UCHAR *pData,
	IN USHORT length,
	IN BOOLEAN bWriteHigh)
{
	NTSTATUS Status;
	USHORT index = 0,Value;
	UCHAR *pSrc = pData;
	USHORT resude = 0;

	resude = length % 2;
	length  += resude;
	do
	{
		Value =(USHORT)( *pSrc  | (*(pSrc + 1) << 8));
		Status = RTUSBSingleWrite(pAd,Offset + index, Value, bWriteHigh);
		index +=2;
		length -= 2;
		pSrc = pSrc + 2;
	}while(length > 0);

	return Status;
}


NTSTATUS RTUSBSingleWrite(
	IN 	RTMP_ADAPTER *pAd,
	IN	USHORT Offset,
	IN	USHORT Value,
	IN	BOOLEAN WriteHigh)
{
	return RTUSB_VendorRequest(pAd, USBD_TRANSFER_DIRECTION_OUT,
								DEVICE_VENDOR_REQUEST_OUT,
								(WriteHigh == TRUE) ? 0x10 : 0x2,
								Value, Offset, NULL, 0);
}


/*
	========================================================================

	Routine Description: Read 32-bit MAC register

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBReadMACRegister(RTMP_ADAPTER *pAd, USHORT Offset, UINT32 *pValue)
{
	NTSTATUS Status = 0;
	UINT32 localVal;

	Status = RTUSB_VendorRequest(
		pAd,
		(USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK),
		DEVICE_VENDOR_REQUEST_IN,
		0x7,
		0,
		Offset,
		&localVal,
		4);

	*pValue = le2cpu32(localVal);


	if (Status != 0)
		*pValue = 0xffffffff;

	return Status;
}


/*
	========================================================================

	Routine Description: Write 32-bit MAC register

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBWriteMACRegister(
	IN RTMP_ADAPTER *pAd,
	IN USHORT Offset,
	IN UINT32 Value,
	IN BOOLEAN bWriteHigh)
{
	NTSTATUS Status;
	UINT32 localVal;

	localVal = Value;

	/* MT76xx HW has 4 byte alignment constrained */    
	if (IS_MT76xx(pAd))
	{   
		Status = RTUSBMultiWrite_nBytes(
		pAd,
		Offset,
		&Value,
		4,
		4);
	}
	else
	{
		Status = RTUSBSingleWrite(pAd, Offset, (USHORT)(localVal & 0xffff), bWriteHigh);
		Status = RTUSBSingleWrite(pAd, Offset + 2, (USHORT)((localVal & 0xffff0000) >> 16), bWriteHigh);
	}

	return Status;
}


int write_reg(RTMP_ADAPTER *ad, UINT32 base, UINT16 offset, UINT32 val)
{
	NTSTATUS ret;
	UINT8 req;
	UINT32 io_value;

	if (base == 0x40)
		req = 0x46;

	io_value = cpu2le32(val);

	ret = RTUSB_VendorRequest(ad,
							  USBD_TRANSFER_DIRECTION_OUT,
							  DEVICE_VENDOR_REQUEST_OUT,
							  req,
							  0,
							  offset,
							  &io_value,
							  4);


	if (ret) {
		DBGPRINT(RT_DEBUG_ERROR, ("write reg fail\n"));
	}

	return ret;
}


int read_reg(RTMP_ADAPTER *ad, UINT32 base, UINT16 offset, UINT32 *value)
{
	NTSTATUS ret;
	UINT8 req;
	UINT32 io_value;
	
	if (base == 0x40)
		req = 0x47;
	else if (base == 0x41)
		req = 0x7;

	ret = RTUSB_VendorRequest(ad,
							  (USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK),
							  DEVICE_VENDOR_REQUEST_IN,
							  req,
							  0,
							  offset,
							  &io_value,
							  4);

	*value = le2cpu32(io_value);

	if (ret)
		*value = 0xffffffff;

	return ret;
}

int usb2_disconnect_cmd(RTMP_ADAPTER *ad, UINT8 Discon_mode)
{
	NTSTATUS ret = NDIS_STATUS_FAILURE;
	UINT16 index = 0;

	index = Discon_mode;

	DBGPRINT(RT_DEBUG_OFF, ("usb2 disconnect command\n"));
	ret = RTUSB_VendorRequest(ad,
			USBD_TRANSFER_DIRECTION_OUT,
			DEVICE_VENDOR_REQUEST_OUT,
			0x91,
			0x01,
			index,
			0,
			0);

	if (ret)
		DBGPRINT(RT_DEBUG_ERROR, ("usb2 disconnect command fail\n"));

	return ret;
}


#ifdef RTMP_BBP
/*
	========================================================================

	Routine Description: Read 8-bit BBP register directly, i.e., not through the MCU

	Arguments:

	Return Value:
	
	Note: Obsoleted
	========================================================================
*/
NTSTATUS RTUSBReadBBPRegister_Direct(RTMP_ADAPTER *pAd, UCHAR Id, UCHAR *buf)
{
	BBP_CSR_CFG_STRUC BbpCsr;
	UINT i = 0;
	NTSTATUS status;
	int RET = 0;
	BOOLEAN read_done;

	RTMP_SEM_EVENT_WAIT(&(pAd->reg_atomic), RET);
	if (RET != 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", RET));
		return STATUS_UNSUCCESSFUL;
	}

	/* Verify the busy condition*/
	read_done = FALSE;
	for (i = 0; i < RETRY_LIMIT; i++)
	{
		status = RTMP_IO_READ32(pAd, BBP_CSR_CFG, &BbpCsr.word);
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			break;

		if (status >= 0 && (BbpCsr.field.Busy == IDLE))
		{
			if (read_done == FALSE)
			{
				/* Prepare for write material */
				BbpCsr.word = 0;
				BbpCsr.field.fRead = 1;
				BbpCsr.field.Busy = 1;
				BbpCsr.field.RegNum = Id;
				RTUSBWriteMACRegister(pAd, BBP_CSR_CFG, BbpCsr.word, FALSE);
				read_done = TRUE;
				i = 0;
			} else {
				*buf = (UCHAR)BbpCsr.field.Value;
				break;
			}
		}
	}
	DBGPRINT(RT_DEBUG_TRACE, ("%s():RetryCnt=%d!read_done=%d\n",
								__FUNCTION__, i, read_done));
			
	if ((i == RETRY_LIMIT) || (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		/* Read failed then Return Default value.*/
		*buf = pAd->BbpWriteLatch[Id];
	  	RTMP_SEM_EVENT_UP(&(pAd->reg_atomic));
		DBGPRINT_RAW(RT_DEBUG_ERROR, ("Retry count exhausted or device removed!!!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	RTMP_SEM_EVENT_UP(&pAd->reg_atomic);

	return STATUS_SUCCESS;
}


/*
	========================================================================

	Routine Description: Read 8-bit BBP register via firmware

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBReadBBPRegister(RTMP_ADAPTER *pAd, UCHAR Id, UCHAR *pValue)
{
	BBP_CSR_CFG_STRUC BbpCsr = {.word = 0};
	int i, k, ret;
	UINT32 bbp_agent;
	BOOLEAN via_mcu = TRUE;

#ifdef MT7601
	if (IS_MT7601(pAd)) {
		if (pAd->WlanFunCtrl.field.WLAN_EN == 0)
		{
			DBGPRINT_ERR(("MT7601_BBP_read. Not allow to read BBP 0x%x : fail\n",  Id));	
			return STATUS_UNSUCCESSFUL;
		}
		via_mcu = FALSE;
	}
#endif /* MT7601 */

	bbp_agent = ((via_mcu == TRUE) ? H2M_BBP_AGENT : BBP_CSR_CFG);

	RTMP_SEM_EVENT_WAIT(&pAd->reg_atomic, ret);
	if (ret != 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
		return STATUS_UNSUCCESSFUL;
	}

	for (i=0; i<MAX_BUSY_COUNT; i++)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			break;

		RTMP_IO_READ32(pAd, bbp_agent, &BbpCsr.word);
		if (BbpCsr.field.Busy == BUSY)
			continue;

		BbpCsr.word = 0;
		BbpCsr.field.fRead = 1;
		BbpCsr.field.BBP_RW_MODE = 1;
		BbpCsr.field.Busy = 1;
		BbpCsr.field.RegNum = Id;
		RTUSBWriteMACRegister(pAd, bbp_agent, BbpCsr.word, FALSE);
		if (via_mcu)
			AsicSendCommandToMcu(pAd, 0x80, 0xff, 0x0, 0x0, TRUE);

		for (k=0; k<MAX_BUSY_COUNT; k++)
		{
			RTMP_IO_READ32(pAd, bbp_agent, &BbpCsr.word);
			if ((BbpCsr.field.Busy == IDLE) || (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
				break;
		}

		if ((BbpCsr.field.Busy == IDLE) &&
			(BbpCsr.field.RegNum == Id))
		{
			*pValue = (UCHAR)BbpCsr.field.Value;
			break;
		}
	}

	RTMP_SEM_EVENT_UP(&pAd->reg_atomic);
	
	if ((BbpCsr.field.Busy == BUSY) || (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		DBGPRINT_ERR(("BBP read R%d=0x%x fail\n", Id, BbpCsr.word));
		*pValue = pAd->BbpWriteLatch[Id];
		return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}


/*
	========================================================================

	Routine Description: Write 8-bit BBP register

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBWriteBBPRegister_Direct(RTMP_ADAPTER *pAd, UCHAR Id, UCHAR Value)
{
	BBP_CSR_CFG_STRUC	BbpCsr;
	UINT			i = 0;
	NTSTATUS		status;
	int				RET = 0;

	RTMP_SEM_EVENT_WAIT(&(pAd->reg_atomic), RET);

	/* Verify the busy condition*/
	do
	{
		status = RTMP_IO_READ32(pAd, BBP_CSR_CFG, &BbpCsr.word);
		if (status >= 0)
		{
			if (!(BbpCsr.field.Busy == BUSY))
			break;
		}
		DBGPRINT(RT_DEBUG_TRACE, ("RTUSBWriteBBPRegister(BBP_CSR_CFG):retry count=%d!\n", i));
		i++;
	} while ((i < RETRY_LIMIT) && (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)));

	if ((i == RETRY_LIMIT) || (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		DBGPRINT_RAW(RT_DEBUG_ERROR, ("Retry count exhausted or device removed!!!\n"));
	  	RTMP_SEM_EVENT_UP(&(pAd->reg_atomic));
		return STATUS_UNSUCCESSFUL;
	}

	/* Prepare for write material*/
	BbpCsr.word = 0;
	BbpCsr.field.fRead = 0;
	BbpCsr.field.Value = Value;
	BbpCsr.field.Busy = 1;
	BbpCsr.field.RegNum = Id;
	RTUSBWriteMACRegister(pAd, BBP_CSR_CFG, BbpCsr.word, FALSE);

	pAd->BbpWriteLatch[Id] = Value;
  	RTMP_SEM_EVENT_UP(&(pAd->reg_atomic));

	return STATUS_SUCCESS;
}


/*
	========================================================================

	Routine Description: Write 8-bit BBP register via firmware

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBWriteBBPRegister(RTMP_ADAPTER *pAd, UCHAR Id, UCHAR Value)
{
	BBP_CSR_CFG_STRUC BbpCsr;
	int BusyCnt, ret;
	UINT32 bbp_agent;
	BOOLEAN via_mcu = TRUE;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return STATUS_UNSUCCESSFUL;
	}


#ifdef MT7601
	if (IS_MT7601(pAd)) {
		if (pAd->WlanFunCtrl.field.WLAN_EN == 0)
		{
			DBGPRINT_ERR(("MT7601_BBP_read. Not allow to read BBP 0x%x : fail\n",  Id));	
			return STATUS_UNSUCCESSFUL;
		}
		via_mcu = FALSE;
	}
#endif /* MT7601 */

	bbp_agent = ((via_mcu == TRUE) ? H2M_BBP_AGENT : BBP_CSR_CFG);

	RTMP_SEM_EVENT_WAIT(&pAd->reg_atomic, ret);
	if (ret != 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
		return STATUS_UNSUCCESSFUL;
	}

	for (BusyCnt = 0; BusyCnt < MAX_BUSY_COUNT; BusyCnt++)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			break;

		RTMP_IO_READ32(pAd, bbp_agent, &BbpCsr.word);
		if (BbpCsr.field.Busy == BUSY)
			continue;
		BbpCsr.word = 0;
		BbpCsr.field.fRead = 0;
		BbpCsr.field.BBP_RW_MODE = 1;
		BbpCsr.field.Busy = 1;
		BbpCsr.field.Value = Value;
		BbpCsr.field.RegNum = Id;
		RTMP_IO_WRITE32(pAd, bbp_agent, BbpCsr.word);
		if (via_mcu)
			AsicSendCommandToMcu(pAd, 0x80, 0xff, 0x0, 0x0, TRUE);
		pAd->BbpWriteLatch[Id] = Value;
		break;
	}

	RTMP_SEM_EVENT_UP(&pAd->reg_atomic);
	
	if ((BusyCnt == MAX_BUSY_COUNT) || (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		DBGPRINT_ERR(("BBP write R%d=0x%x fail\n", Id, BbpCsr.word));
		return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}


/*
	========================================================================

	Routine Description: Write RF register through MAC

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBWriteRFRegister(RTMP_ADAPTER *pAd, UINT32 Value)
{
	RF_CSR_CFG0_STRUC PhyCsr4;
	UINT i = 0;
	NTSTATUS status;

	NdisZeroMemory(&PhyCsr4, sizeof(RF_CSR_CFG0_STRUC));

	RTMP_SEM_EVENT_WAIT(&pAd->reg_atomic, status);
	if (status != 0) {
		DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", status));
		return STATUS_UNSUCCESSFUL;
	}

	status = STATUS_UNSUCCESSFUL;
	do
	{
		status = RTMP_IO_READ32(pAd, RF_CSR_CFG0, &PhyCsr4.word);
		if (status >= 0)
		{
		if (!(PhyCsr4.field.Busy))
			break;
		}
		DBGPRINT(RT_DEBUG_TRACE, ("RTUSBWriteRFRegister(RF_CSR_CFG0):retry count=%d!\n", i));
		i++;
	}
	while ((i < RETRY_LIMIT) && (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)));

	if ((i == RETRY_LIMIT) || (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		DBGPRINT_RAW(RT_DEBUG_ERROR, ("Retry count exhausted or device removed!!!\n"));
		goto done;
	}

	RTUSBWriteMACRegister(pAd, RF_CSR_CFG0, Value, FALSE);
	status = STATUS_SUCCESS;

done:
	RTMP_SEM_EVENT_UP(&pAd->reg_atomic);
	
	return status;
}
#endif /* RTMP_BBP */


/*
	========================================================================

	Routine Description:

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
VOID RTUSBPutToSleep(RTMP_ADAPTER *pAd)
{
	UINT32 value;

	/* Timeout 0x40 x 50us*/
	value = (SLEEPCID<<16)+(OWNERMCU<<24)+ (0x40<<8)+1;
	RTUSBWriteMACRegister(pAd, 0x7010, value, FALSE);
	RTUSBWriteMACRegister(pAd, 0x404, 0x30, FALSE);
	DBGPRINT_RAW(RT_DEBUG_ERROR, ("Sleep Mailbox testvalue %x\n", value));
}


/*
	========================================================================

	Routine Description:

	Arguments:

	Return Value:

	IRQL =

	Note:

	========================================================================
*/
NTSTATUS RTUSBWakeUp(RTMP_ADAPTER *pAd)
{
	return RTUSB_VendorRequest(pAd, USBD_TRANSFER_DIRECTION_OUT,
								DEVICE_VENDOR_REQUEST_OUT,
								0x01, 0x09, 0, NULL, 0);
}
/*Nobody uses it currently*/
#if 0
VOID RTUSBWatchDog(RTMP_ADAPTER *pAd)
{
	PHT_TX_CONTEXT pHTTXContext;
	int idx;
	ULONG irqFlags;
	PURB pUrb;
	BOOLEAN needDumpSeq = FALSE;
	UINT32 MACValue;

	if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;

#ifdef CONFIG_STA_SUPPORT
	if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
		return;
#endif /* CONFIG_STA_SUPPORT */

#ifdef RLT_MAC
	// TODO: shiang-usw, need to check if this function still required!!
	if (pAd->chipCap.hif_type == HIF_RLT)
		return;
#endif /* RLT_MAC */

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return;
	}


#ifdef RTMP_MAC
	idx = 0;
	RTMP_IO_READ32(pAd, TXRXQ_PCNT, &MACValue);
	if ((MACValue & 0xff) !=0 )
	{
		DBGPRINT(RT_DEBUG_TRACE, ("TX QUEUE 0 Not EMPTY(Value=0x%0x). !!!!!!!!!!!!!!!\n", MACValue));
		RTMP_IO_WRITE32(pAd, PBF_CFG, 0xf40012);
		while((MACValue &0xff) != 0 && (idx++ < 10))
		{
		        RTMP_IO_READ32(pAd, TXRXQ_PCNT, &MACValue);
		        RtmpusecDelay(1);
		}
		RTMP_IO_WRITE32(pAd, PBF_CFG, 0xf40006);
	}

	idx = 0;
	if ((MACValue & 0xff00) !=0 )
	{
		DBGPRINT(RT_DEBUG_TRACE, ("TX QUEUE 1 Not EMPTY(Value=0x%0x). !!!!!!!!!!!!!!!\n", MACValue));
		RTMP_IO_WRITE32(pAd, PBF_CFG, 0xf4000a);
		while((MACValue &0xff00) != 0 && (idx++ < 10))
		{
			RTMP_IO_READ32(pAd, TXRXQ_PCNT, &MACValue);
			RtmpusecDelay(1);
		}
		RTMP_IO_WRITE32(pAd, PBF_CFG, 0xf40006);
	}


	if (pAd->watchDogRxOverFlowCnt >= 2)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Maybe the Rx Bulk-In hanged! Cancel the pending Rx bulks request!\n"));
		if ((!RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS |
									fRTMP_ADAPTER_BULKIN_RESET |
									fRTMP_ADAPTER_HALT_IN_PROGRESS |
									fRTMP_ADAPTER_NIC_NOT_EXIST))))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Call CMDTHREAD_RESET_BULK_IN to cancel the pending Rx Bulk!\n"));
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_BULKIN_RESET);
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_IN, NULL, 0);
			needDumpSeq = TRUE;
		}
		pAd->watchDogRxOverFlowCnt = 0;
	}


	for (idx = 0; idx < NUM_OF_TX_RING; idx++)
	{
		pUrb = NULL;

		RTMP_IRQ_LOCK(&pAd->BulkOutLock[idx], irqFlags);
/*		if ((pAd->BulkOutPending[idx] == TRUE) && pAd->watchDogTxPendingCnt)*/
		if (pAd->BulkOutPending[idx] == TRUE)
		{
			pAd->watchDogTxPendingCnt[idx]++;

			if ((pAd->watchDogTxPendingCnt[idx] > 2) &&
				 (!RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS | fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST | fRTMP_ADAPTER_BULKOUT_RESET)))
				)
			{
				/* FIXME: Following code just support single bulk out. If you wanna support multiple bulk out. Modify it!*/
				pHTTXContext = (PHT_TX_CONTEXT)(&pAd->TxContext[idx]);
				if (pHTTXContext->IRPPending)
				{	/* Check TxContext.*/
#ifdef USB_BULK_BUF_ALIGMENT
					pUrb = pHTTXContext->pUrb[pHTTXContext->CurtBulkIdx];
#else
					pUrb = pHTTXContext->pUrb;
#endif /* USB_BULK_BUF_ALIGMENT */
				}
				else if (idx == MGMTPIPEIDX)
				{
					PTX_CONTEXT pMLMEContext, pNULLContext, pPsPollContext;

					/*Check MgmtContext.*/
					pMLMEContext = (PTX_CONTEXT)(pAd->MgmtRing.Cell[pAd->MgmtRing.TxDmaIdx].AllocVa);
					pPsPollContext = (PTX_CONTEXT)(&pAd->PsPollContext);
					pNULLContext = (PTX_CONTEXT)(&pAd->NullContext);

					if (pMLMEContext->IRPPending)
					{
						ASSERT(pMLMEContext->IRPPending);
						pUrb = pMLMEContext->pUrb;
					}
					else if (pNULLContext->IRPPending)
					{
						ASSERT(pNULLContext->IRPPending);
						pUrb = pNULLContext->pUrb;
					}
					else if (pPsPollContext->IRPPending)
					{
						ASSERT(pPsPollContext->IRPPending);
						pUrb = pPsPollContext->pUrb;
					}
				}

				RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[idx], irqFlags);

				DBGPRINT(RT_DEBUG_TRACE, ("Maybe the Tx Bulk-Out hanged! Cancel the pending Tx bulks request of idx(%d)!\n", idx));
				if (pUrb)
				{
					DBGPRINT(RT_DEBUG_TRACE, ("Unlink the pending URB!\n"));
					/* unlink it now*/
					RTUSB_UNLINK_URB(pUrb);
					/* Sleep 200 microseconds to give cancellation time to work*/
					RtmpusecDelay(200);
					needDumpSeq = TRUE;
				}
				else
				{
					DBGPRINT(RT_DEBUG_ERROR, ("Unkonw bulkOut URB maybe hanged!!!!!!!!!!!!\n"));
				}
			}
			else
			{
				RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[idx], irqFlags);
			}
		}
		else
		{
			RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[idx], irqFlags);
		}
	}

#ifdef DOT11_N_SUPPORT
	/* For Sigma debug, dump the ba_reordering sequence.*/
	if((needDumpSeq == TRUE) /*&& (pAd->CommonCfg.bDisableReordering == 0)*/)
	{
		USHORT				Idx;
		PBA_REC_ENTRY		pBAEntry = NULL;
		UCHAR				count = 0;
		struct reordering_mpdu *mpdu_blk;

		Idx = pAd->MacTab.Content[BSSID_WCID].BARecWcidArray[0];

		pBAEntry = &pAd->BATable.BARecEntry[Idx];
		if((pBAEntry->list.qlen > 0) && (pBAEntry->list.next != NULL))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("NICUpdateRawCounters():The Queueing pkt in reordering buffer:\n"));
			NdisAcquireSpinLock(&pBAEntry->RxReRingLock);
			mpdu_blk = pBAEntry->list.next;
			while (mpdu_blk)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("\t%d:Seq-%d, bAMSDU-%d!\n", count, mpdu_blk->Sequence, mpdu_blk->bAMSDU));
				mpdu_blk = mpdu_blk->next;
				count++;
			}

			DBGPRINT(RT_DEBUG_TRACE, ("\npBAEntry->LastIndSeq=%d!\n", pBAEntry->LastIndSeq));
			NdisReleaseSpinLock(&pBAEntry->RxReRingLock);
		}
	}
#endif /* DOT11_N_SUPPORT */
#endif /* RTMP_MAC */
}
#endif
#endif /* RTMP_MAC_USB */

