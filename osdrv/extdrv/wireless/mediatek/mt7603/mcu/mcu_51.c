/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rtmp_M51.c

	Abstract:

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/


#include	"rt_config.h"
#include 	"firmware.h"

#if 0
#define BIN_IN_FILE /* use *.bin firmware */
#endif

#ifdef RTMP_MAC_USB

/* RT2870 Firmware Spec only used 1 oct for version expression*/

#define FIRMWARE_MINOR_VERSION	7
#endif /* RTMP_MAC_USB */

/* New 8k byte firmware size for RT3071/RT3072*/
#define FIRMWAREIMAGE_MAX_LENGTH	0x2000
#ifdef WOW_SUPPORT 
#define FIRMWAREIMAGE_WOW_LENGTH	0x3000 /* WOW support firmware(12KB) */
#endif/*WOW_SUPPORT*/
#define FIRMWAREIMAGE_LENGTH			(sizeof (FirmwareImage) / sizeof(UCHAR))
#define FIRMWARE_MAJOR_VERSION		0

#define FIRMWAREIMAGEV1_LENGTH		0x1000
#define FIRMWAREIMAGEV2_LENGTH		0x1000
#ifdef WOW_SUPPORT 
#define FIRMWAREIMAGEV3_LENGTH		0x2000 /* WOW support firmware */
#endif/*WOW_SUPPORT*/

#ifdef RTMP_MAC_PCI
#define FIRMWARE_MINOR_VERSION		2
#endif /* RTMP_MAC_PCI */

const unsigned short ccitt_16Table[] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};
#define ByteCRC16(v, crc) \
	(unsigned short)((crc << 8) ^  ccitt_16Table[((crc >> 8) ^ (v)) & 255])

unsigned char BitReverse(unsigned char x)
{
	int i;
	unsigned char Temp=0;
	for(i=0; ; i++)
	{
		if(x & 0x80)	Temp |= 0x80;
		if(i==7)		break;
		x	<<= 1;
		Temp >>= 1;
	}
	return Temp;
}


/*
	========================================================================
	
	Routine Description:
		erase 8051 firmware image in MAC ASIC

	Arguments:
		Adapter						Pointer to our adapter

	IRQL = PASSIVE_LEVEL
		
	========================================================================
*/
INT RtmpAsicEraseFirmware(
	IN PRTMP_ADAPTER pAd)
{
	UINT32 i;

	for(i = 0; i < MAX_FIRMWARE_IMAGE_SIZE; i += 4)
		RTMP_IO_WRITE32(pAd, FIRMWARE_IMAGE_BASE + i, 0);

	return 0;
}

NDIS_STATUS isMCUNeedToLoadFIrmware(
	IN PRTMP_ADAPTER pAd)
{
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	ULONG			Index;
	UINT32			MacReg;
	
	Index = 0;

	do {
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))			
			return NDIS_STATUS_FAILURE;
		
		RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &MacReg);

		if (MacReg & 0x100) /* check bit 8*/
			break;
		
		RtmpusecDelay(1000);
	} while (Index++ < 100);

	if (Index >= 100)
		Status = NDIS_STATUS_FAILURE;

	return Status;
}

NDIS_STATUS isMCUnotReady(
	IN PRTMP_ADAPTER pAd)
{
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	ULONG			Index;
	UINT32			MacReg;

#ifdef RT65xx
	// TODO: shiang-6590, fix me, currently firmware is not ready yet, so ignore it!
	if (IS_RT65XX(pAd)) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): Ignore for MCU status check for 6590 now!\n", __FUNCTION__));
		return Status;
	}
#endif /* RT65xx */

#ifdef MT7601
	if (IS_MT7601(pAd)) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): Ignore for MCU status check for MT7601 now!\n", __FUNCTION__));
		return Status;
	}
#endif /* MT7601 */

	Index = 0;

	do {
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))			
			return NDIS_STATUS_FAILURE;
		
		RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &MacReg);

		if (MacReg & 0x80) /* check bit 7*/
			break;
		
		RtmpusecDelay(1000);
	} while (Index++ < 1000);

	if (Index >= 1000)
		Status = NDIS_STATUS_FAILURE;

	return Status;
}


#ifdef RELEASE_EXCLUDE
	/*
	1. For PCI:
        (1) Write SYS_CTRL bit16(HST_PM_SEL) to 1
        (2) Write 8051 firmware to RAM.
        (3) Write SYS_CTRL to 0.
        (4) Write SYS_CTRL bit0(MCU_RESET) to 1 to do MCU HW reset.

	2. For USB:
        (1)Get current firmware operation mode via VendorRequest(0x1, 0x11) command.
        (2) Write SYS_CTRL bit7(MCU_READY) to 0.
        (3) Write SYS_CTRL bit0(MCU_RESET) to 1 to do MCU reset to run 8051 on ROM.
        (4) Check MCU ready via SYS_CTRL bit7(MCU_READY).
        (5) Write 8051 firmware to RAM.
        (6) Write MAC 0x7014 to 0xffffffff.
        (7) Write MAC 0x701c to 0xffffffff.
        (8) Change 8051 from ROM to RAM site via VendorRequest(0x01, 0x8) command.
	*/
#endif /* RELEASE_EXCLUDE */
/*
	========================================================================
	
	Routine Description:
		Load 8051 firmware file into MAC ASIC

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS         firmware image load ok
		NDIS_STATUS_FAILURE         image not found

	IRQL = PASSIVE_LEVEL
		
	========================================================================
*/
NDIS_STATUS RtmpAsicLoadFirmware(
	IN PRTMP_ADAPTER pAd)
{
#ifdef BIN_IN_FILE
#define NICLF_DEFAULT_USE()	\
	flg_default_firm_use = TRUE; \
	DBGPRINT(RT_DEBUG_OFF, ("%s - Use default firmware!\n", __FUNCTION__));

	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	PUCHAR			src;
	RTMP_OS_FD		srcf;
	INT 				retval, i;
	PUCHAR			pFirmwareImage;
	INT				FileLength = 0;
	UINT32			MacReg;
	ULONG			Index;
	ULONG			firm;
	BOOLEAN			flg_default_firm_use = FALSE;
	RTMP_OS_FS_INFO	osFSInfo;

	DBGPRINT(RT_DEBUG_TRACE, ("===> %s\n", __FUNCTION__));

	/* init */
	pFirmwareImage = NULL;
	src = RTMP_FIRMWARE_FILE_NAME;

	RtmpOSFSInfoChange(&osFSInfo, TRUE);

	pAd->FirmwareVersion = (FIRMWARE_MAJOR_VERSION << 8) + \
						   FIRMWARE_MINOR_VERSION;


	/* allocate firmware buffer */
	os_alloc_mem(pAd, (UCHAR **)&pFirmwareImage, MAX_FIRMWARE_IMAGE_SIZE);
	if (pFirmwareImage == NULL)
	{
		/* allocate fail, use default firmware array in firmware.h */
		DBGPRINT(RT_DEBUG_ERROR, ("%s - Allocate memory fail!\n", __FUNCTION__));
		NICLF_DEFAULT_USE();
	}
	else
	{
		/* allocate ok! zero the firmware buffer */
		memset(pFirmwareImage, 0x00, MAX_FIRMWARE_IMAGE_SIZE);
	}


	/* if ok, read firmware file from *.bin file */
	if (flg_default_firm_use == FALSE)
	{
		do
		{
			/* open the bin file */
			srcf = RtmpOSFileOpen(src, O_RDONLY, 0);

			if (IS_FILE_OPEN_ERR(srcf)) 
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s - Error opening file %s\n", __FUNCTION__, src));
				NICLF_DEFAULT_USE();
				break;
			}


			/* read the firmware from the file *.bin */
			FileLength = RtmpOSFileRead(srcf, pFirmwareImage, MAX_FIRMWARE_IMAGE_SIZE);
			if (FileLength != MAX_FIRMWARE_IMAGE_SIZE)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s: error file length (=%d) in RT2860AP.BIN\n",
					   __FUNCTION__, FileLength));
				NICLF_DEFAULT_USE();
				break;
			}
			else
			{
				PUCHAR ptr = pFirmwareImage;
				USHORT crc = 0xffff;


				/* calculate firmware CRC */
				for(i=0; i<(MAX_FIRMWARE_IMAGE_SIZE-2); i++, ptr++)
					crc = ByteCRC16(BitReverse(*ptr), crc);

				if ((pFirmwareImage[MAX_FIRMWARE_IMAGE_SIZE-2] != \
								(UCHAR)BitReverse((UCHAR)(crc>>8))) ||
					(pFirmwareImage[MAX_FIRMWARE_IMAGE_SIZE-1] != \
								(UCHAR)BitReverse((UCHAR)crc)))
				{
					/* CRC fail */
					DBGPRINT(RT_DEBUG_ERROR, ("%s: CRC = 0x%02x 0x%02x "
						   "error, should be 0x%02x 0x%02x\n",
						   __FUNCTION__,
						   pFirmwareImage[MAX_FIRMWARE_IMAGE_SIZE-2],
						   pFirmwareImage[MAX_FIRMWARE_IMAGE_SIZE-1],
						   (UCHAR)(crc>>8), (UCHAR)(crc)));
					NICLF_DEFAULT_USE();
					break;
				}
				else
				{
					/* firmware is ok */
					pAd->FirmwareVersion = \
						(pFirmwareImage[MAX_FIRMWARE_IMAGE_SIZE-4] << 8) +
						pFirmwareImage[MAX_FIRMWARE_IMAGE_SIZE-3];

					/* check if firmware version of the file is too old */
					if ((pAd->FirmwareVersion) < \
											((FIRMWARE_MAJOR_VERSION << 8) +
									  	 	 FIRMWARE_MINOR_VERSION))
					{
						DBGPRINT(RT_DEBUG_ERROR, ("%s: firmware version too old!\n", __FUNCTION__));
						NICLF_DEFAULT_USE();
						break;
					}
				}

				DBGPRINT(RT_DEBUG_TRACE,
						 ("NICLoadFirmware: CRC ok, ver=%d.%d\n",
						  pFirmwareImage[MAX_FIRMWARE_IMAGE_SIZE-4],
						  pFirmwareImage[MAX_FIRMWARE_IMAGE_SIZE-3]));
			}
			break;
		} while(TRUE);

		/* close firmware file */
		if (IS_FILE_OPEN_ERR(srcf))
			;
		else
		{
			retval = RtmpOSFileClose(srcf);
			if (retval)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("--> Error %d closing %s\n", -retval, src));
			}
		}
	}


	/* write firmware to ASIC */
	if (flg_default_firm_use == TRUE)
	{
		/* use default fimeware, free allocated buffer */
		if (pFirmwareImage != NULL)
			os_free_mem(NULL, pFirmwareImage);

		/* use default *.bin array */
		pFirmwareImage = FirmwareImage;
		FileLength = sizeof(FirmwareImage);
	}

	/* enable Host program ram write selection */
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0x10000); 

	for(i=0; i<FileLength; i+=4)
	{
		firm = pFirmwareImage[i] +
			   (pFirmwareImage[i+3] << 24) +
			   (pFirmwareImage[i+2] << 16) +
			   (pFirmwareImage[i+1] << 8);

		RTMP_IO_WRITE32(pAd, FIRMWARE_IMAGE_BASE + i, firm);
	}

	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0x00000);
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0x00001);

	/* initialize BBP R/W access agent */
	RTMP_IO_WRITE32(pAd, H2M_BBP_AGENT, 0);
	RTMP_IO_WRITE32(pAd, H2M_MAILBOX_CSR, 0);

	if (flg_default_firm_use == FALSE)
	{
		/* use file firmware, free allocated buffer */
		if (pFirmwareImage != NULL)
			os_free_mem(NULL, pFirmwareImage);
	}

	RtmpOSFSInfoChange(&osFSInfo, FALSE);
#else

	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	PUCHAR			pFirmwareImage;
	ULONG			FileLength;
	UINT32			Version = (pAd->MACVersion >> 16);
#ifdef RTMP_MAC_USB
	BOOLEAN			Equal = TRUE;
	UINT32			MacReg1 = 0;
	UCHAR			FVer;
	UINT16			FCS;
#endif


	pFirmwareImage = FirmwareImage;
	FileLength = sizeof(FirmwareImage);

#ifdef RTMP_MAC_USB
	/* check firmware version and checksum in RAM and firmware.h */
	/* if they are equal, then will skip firmware load procedure */
	if (isMCUNeedToLoadFIrmware(pAd) == NDIS_STATUS_SUCCESS)
	{
		RTMP_IO_READ32(pAd, 0x3FFC, &MacReg1);
		FVer = (MacReg1 >> 8) & 0x00FF;
		FCS = (MacReg1 >> 16) & 0xFFFF;

	
#ifdef WOW_SUPPORT
		if (pAd->WOW_Cfg.bWOWFirmware == TRUE)
		{
			UCHAR ver = FirmwareImage[FIRMWAREIMAGEV3_LENGTH+0xFFD];
			UINT16 sum ;

			NdisCopyMemory(&sum, &FirmwareImage[FIRMWAREIMAGEV3_LENGTH+0xFFE], 2);
			printk("%s: ver %x/%x, sum %x/%x, mac %x\n", __FUNCTION__, FVer, ver, FCS, sum, MacReg1);
			if ( FVer != ver || FCS != sum )
				Equal = FALSE;
		}
		else
#endif /* WOW_SUPPORT */
		{
			UCHAR ver = FirmwareImage[FIRMWAREIMAGEV2_LENGTH+0xFFD];
			UINT16 sum ;

			NdisCopyMemory(&sum, &FirmwareImage[FIRMWAREIMAGEV2_LENGTH+0xFFE], 2);
			printk("%s: ver %x/%x, sum %x/%x, mac %x\n", __FUNCTION__, FVer, ver, FCS, sum, MacReg1);
			if ( FVer != ver || FCS != sum )
				Equal = FALSE;
		}	

		/* do not need to load firmware */
		if (Equal == FALSE)
		{
			DBGPRINT(RT_DEBUG_OFF, ("%s: WOW stops to go into 4K ram codes ...\n", __FUNCTION__));

#ifdef WOW_SUPPORT
			/* Disable MAC TX/RX */
			AsicSetMacTxRx(pAd, ASIC_MAC_TXRX, FALSE);
	
			/* Disable USB TX/RX DMA */
			USB_CFG_READ(pAd, &MacReg1);
			MacReg1 &= ~0xC00000;
			USB_CFG_WRITE(pAd, MacReg1);
	
			/* Clear Firmware(bit 8) ready bit to force firmware download*/
			RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &MacReg1);
			MacReg1 &= ~0x100;
			RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, MacReg1);

#endif /* WOW_SUPPORT */

			/* prevent CPU to run in 4K ram codes */
			/* only allow USB vendor request */
			Status = RTUSB_VendorRequest(
			pAd,
			USBD_TRANSFER_DIRECTION_OUT,
			DEVICE_VENDOR_REQUEST_OUT,
			0x14,
			0x0,
			0,
			NULL,
			0);

			/* disable external functions */
			Status = RTUSB_VendorRequest(
			pAd,
			USBD_TRANSFER_DIRECTION_OUT,
			DEVICE_VENDOR_REQUEST_OUT,
			0x12,
			0x0,
			0,
			NULL,
			0);
		}
	}

#endif /* RTMP_MAC_USB */

	/* New 8k byte firmware size for RT3071/RT3072*/
	/*DBGPRINT(RT_DEBUG_TRACE, ("Usb Chip\n"));*/
	if (FIRMWAREIMAGE_LENGTH == FIRMWAREIMAGE_MAX_LENGTH)
	/*The firmware image consists of two parts. One is the origianl and the other is the new.*/
	/*Use Second Part*/
	{
#ifdef RTMP_MAC_PCI
		if ((Version == 0x2860) || (Version == 0x3572) || IS_RT3090(pAd) 
			|| IS_RT3390(pAd) || IS_RT3593(pAd) || IS_RT5390(pAd) || IS_RT5392(pAd))
		{
			pFirmwareImage = FirmwareImage;
			FileLength = FIRMWAREIMAGE_LENGTH;
		}
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		if ((Version != 0x2860) && (Version != 0x2872) && (Version != 0x3070)) 
		{	/* Use Firmware V2.*/
			/*printk("KH:Use New Version,part2\n");*/
			pFirmwareImage = (PUCHAR)&FirmwareImage[FIRMWAREIMAGEV1_LENGTH];
			FileLength = FIRMWAREIMAGEV2_LENGTH;
		}
		else
		{
			/*printk("KH:Use New Version,part1\n");*/
			pFirmwareImage = FirmwareImage;
			FileLength = FIRMWAREIMAGEV1_LENGTH;
		}
#endif /* RTMP_MAC_USB */
	}
	else
	{
#if defined(WOW_SUPPORT) && defined(RTMP_MAC_USB)
		/* WOW firmware is 12KB */
		if ((Version != 0x2860) && (Version != 0x2872) && (Version != 0x3070))
		{
			if (FIRMWAREIMAGE_LENGTH == FIRMWAREIMAGE_WOW_LENGTH) /* size 0x3000 */
			{
				if (pAd->WOW_Cfg.bWOWFirmware == TRUE)
				{
					pFirmwareImage = (PUCHAR)&FirmwareImage[FIRMWAREIMAGEV3_LENGTH]; /* WOW offset: 0x2000 */
					FileLength = FIRMWAREIMAGEV1_LENGTH; /* 0x1000 */
					DBGPRINT(RT_DEBUG_OFF, ("%s: Load WOW firmware!!\n", __FUNCTION__));
				}
				else
				{
					pFirmwareImage = (PUCHAR)&FirmwareImage[FIRMWAREIMAGEV2_LENGTH]; /* normal offset: 0x1000 */
					FileLength = FIRMWAREIMAGEV1_LENGTH; /* 0x1000 */
					DBGPRINT(RT_DEBUG_OFF, ("%s: Load normal firmware!!\n", __FUNCTION__));
				}

			}
		}
		else
#endif /* defined(WOW_SUPPORT) && defined(RTMP_MAC_USB) */
		{
			DBGPRINT(RT_DEBUG_ERROR, ("KH: bin file should be 8KB.\n"));
			Status = NDIS_STATUS_FAILURE;
		}
	}

#ifdef RELEASE_EXCLUDE
	/* 
	   per Alex Lin's comment, 
	   take PBF_SYS_CTRL bit8 for judge Firmware is loaded or not.
	   if bit8 = 0, which means firmware isn't loaded before,
	   then we need to load firmware, firmware will set bit8 to 1

	   if bit8 = 1, the firmware doesn't needed to load again.
	   but need to reset MCU and wait 1ms,
	   then send command 0x1, and value 0x8 to make firmware run into RAM mode.

	   this bit will be cleared by plug-out dongle.
	   
	   But reset MCU will reult in the system issue, so if the firmware is already loaded,
	   just bypass the firmware loading procedure.        -- Brian 20111027
	*/
#endif
#ifdef RTMP_MAC_USB
	/* firmware is never loaded or the loadable firmware is different with the firmware in the RAM */
	if (isMCUNeedToLoadFIrmware(pAd) || Equal == FALSE )
	{
		DBGPRINT(RT_DEBUG_ERROR, ("NICLoadFirmware: We need to load firmware\n"));	
		RTMP_WRITE_FIRMWARE(pAd, pFirmwareImage, FileLength); /* FirmwareRun VndReq 0x1/0x8 --> initDone = 1 */
	}
	else {
		DBGPRINT(RT_DEBUG_ERROR, ("NICLoadFirmware: firmware loaded already\n"));
		RTUSB_VendorRequest(
                pAd,
                USBD_TRANSFER_DIRECTION_OUT,
                DEVICE_VENDOR_REQUEST_OUT,
                0x01,
                0x8,
                0,
                NULL,
                0);
	}
#else
	RTMP_WRITE_FIRMWARE(pAd, pFirmwareImage, FileLength);
#endif /* RTMP_MAC_USB */

#endif

	if (isMCUnotReady(pAd))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): MCU is not ready!\n\n\n", __FUNCTION__));
		Status = NDIS_STATUS_FAILURE;
	}
#ifdef RTMP_USB_SUPPORT
#ifdef RELEASE_EXCLUDE
	/* 
		2009/12/10: fonchi patch for resolving RT3070 unable to connect with AP issue.
		Change initialization sequence of loading FW steps.
		Make sure the MCU is well running before trigger it up. 
	*/
#endif /* RELEASE_EXCLUDE */
	else
	{
#ifdef RELEASE_EXCLUDE
		/* 2008/11/28: KH add to fix the dead rf frequency offset bug. */
#endif /* RELEASE_EXCLUDE */
		RTUSBWriteMACRegister(pAd, H2M_BBP_AGENT, 0, FALSE); /* initialize BBP R/W access agent. */
		RTUSBWriteMACRegister(pAd,H2M_MAILBOX_CSR,0, FALSE);
		RTUSBWriteMACRegister(pAd, H2M_INT_SRC, 0, FALSE);
		AsicSendCommandToMcu(pAd, 0x72, 0x00, 0x00, 0x00, FALSE); /* reset rf by MCU supported by new firmware */
	}
#ifdef WOW_SUPPORT
	if (pAd->WOW_Cfg.bEnable == TRUE)
	{
		/* Enable MAC TX/RX */
		AsicSetMacTxRx(pAd, ASIC_MAC_TXRX, TRUE);

		/* Enble USB TX/RX DMA */
		USB_CFG_READ(pAd, &MacReg1);
		MacReg1 |= 0xC00000;
		USB_CFG_WRITE(pAd, MacReg1);

		/* Set Firmware ready bit(bit8) */
		RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &MacReg1);
		MacReg1 |= 0x100;
		RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, MacReg1);

		/* need to reset, because interface-up will be normal firmware */
		pAd->WOW_Cfg.bWOWFirmware = FALSE;

	}
#endif /* WOW_SUPPORT */

	if (Equal == FALSE)
	{
		/* allow all requests (USB vendor request, sleep, wake up, led) */
		Status = RTUSB_VendorRequest(
		pAd,
		USBD_TRANSFER_DIRECTION_OUT,
		DEVICE_VENDOR_REQUEST_OUT,
		0x13,
		0x0,
		0,
		NULL,
		0);
	}

#endif /* RTMP_USB_SUPPORT */

    DBGPRINT(RT_DEBUG_TRACE, ("<=== %s (status=%d)\n", __FUNCTION__, Status));

    return Status;
}


BOOLEAN RtmpAsicCheckCommanOk(RTMP_ADAPTER *pAd, UCHAR Command)
{
	UINT32	CmdStatus = 0, CID = 0, i;
	UINT32	ThisCIDMask = 0;
#ifdef SPECIFIC_BCN_BUF_SUPPORT
	ULONG	IrqFlags = 0;
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

#ifdef SPECIFIC_BCN_BUF_SUPPORT
	RTMP_MAC_SHR_MSEL_PROTECT_LOCK(pAd, IrqFlags);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
	i = 0;
	do
	{
		RTMP_IO_READ32(pAd, H2M_MAILBOX_CID, &CID);
		/* Find where the command is. Because this is randomly specified by firmware.*/
		if ((CID & CID0MASK) == Command)
		{
			ThisCIDMask = CID0MASK;
			break;
		}
		else if ((((CID & CID1MASK)>>8) & 0xff) == Command)
		{
			ThisCIDMask = CID1MASK;
			break;
		}
		else if ((((CID & CID2MASK)>>16) & 0xff) == Command)
		{
			ThisCIDMask = CID2MASK;
			break;
		}
		else if ((((CID & CID3MASK)>>24) & 0xff) == Command)
		{
			ThisCIDMask = CID3MASK;
			break;
		}

		RtmpusecDelay(100);
		i++;
	}while (i < 200);

	/* Get CommandStatus Value*/
	RTMP_IO_READ32(pAd, H2M_MAILBOX_STATUS, &CmdStatus);
	
	/* This command's status is at the same position as command. So AND command position's bitmask to read status.	*/
	if (i < 200)
	{
		/* If Status is 1, the comamnd is success.*/
		if (((CmdStatus & ThisCIDMask) == 0x1) || ((CmdStatus & ThisCIDMask) == 0x100) 
			|| ((CmdStatus & ThisCIDMask) == 0x10000) || ((CmdStatus & ThisCIDMask) == 0x1000000))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("--> AsicCheckCommanOk CID = 0x%x, CmdStatus= 0x%x \n", CID, CmdStatus));
			RTMP_IO_WRITE32(pAd, H2M_MAILBOX_STATUS, 0xffffffff);
			RTMP_IO_WRITE32(pAd, H2M_MAILBOX_CID, 0xffffffff);
#ifdef SPECIFIC_BCN_BUF_SUPPORT
			RTMP_MAC_SHR_MSEL_PROTECT_UNLOCK(pAd, IrqFlags);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
			return TRUE;
		}
		DBGPRINT(RT_DEBUG_TRACE, ("--> AsicCheckCommanFail1 CID = 0x%x, CmdStatus= 0x%x \n", CID, CmdStatus));
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("--> AsicCheckCommanFail2 Timeout Command = %d, CmdStatus= 0x%x \n", Command, CmdStatus));
	}
	/* Clear Command and Status.*/
	RTMP_IO_WRITE32(pAd, H2M_MAILBOX_STATUS, 0xffffffff);
	RTMP_IO_WRITE32(pAd, H2M_MAILBOX_CID, 0xffffffff);
#ifdef SPECIFIC_BCN_BUF_SUPPORT
   	RTMP_MAC_SHR_MSEL_PROTECT_UNLOCK(pAd, IrqFlags);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

	return FALSE;
}


INT RtmpAsicSendCommandToMcu(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			Command,
	IN UCHAR			Token,
	IN UCHAR			Arg0,
	IN UCHAR			Arg1,
	IN BOOLEAN			FlgIsNeedLocked)
{
	HOST_CMD_CSR_STRUC	H2MCmd;
	H2M_MAILBOX_STRUC	H2MMailbox;
	INT i = 0;
	int ret;


#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT
	ULONG	IrqFlags = 0;
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */

#ifdef CONFIG_STA_SUPPORT
#ifdef PCIE_PS_SUPPORT
	ULONG	Configuration;
	ULONG	offset;
#endif /* PCIE_PS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

#if 0
	if (FlgIsNeedLocked == TRUE)
	{
		RTMP_MAC_SHR_MSEL_PROTECT_LOCK(pAd, IrqFlags);
	}
#endif

#ifdef CONFIG_STA_SUPPORT
#ifdef PCIE_PS_SUPPORT
#if defined(RT5390) || defined(RT5392) || defined(RT5592)
         /* 
         	FW v.30; for 0x30 MCU CMD, Arg1 is not L1/L0 state anymore
	 	Arg 1 set to 0x5A, then FW will not turn off PCIe CLK for PM4 (Associate-Idle)
	 	default it will be turned off; and can be configured to not turn off at PM4
	  */
	if (Command == SLEEP_MCU_CMD)
	{
		Arg1 = 0x00;		/* set default to 0 */ 
	
		// TODO: shiang, for this Arg1 adjust, only work in RT5390 chipset???
		if ((INFRA_ON(pAd)) && (pAd->OpMode == OPMODE_STA)
#ifdef RT3290
			&& (!IS_RT3290(pAd))
#endif /* RT3290 */
		)
		{
			if ((OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED)) && (pAd->StaCfg.PSControl.field.PM4PCIeCLKOn))
			{
				Arg1 = (UCHAR)0x5A;		/* Arg1 set to 0x5A, FW will not turn off PCIe CLK */
			}
		}
	}
	/*
		When calling from RTMP_BBP_IO_READ8_BY_REG_ID or RTMP_BBP_IO_WRITE8_BY_REG_ID,
	 	MCUCommandLock already locked.  So don't lock here again.
	*/
#endif /* defined(RT5390) || defined(RT5392) */

#ifdef RT3290
	if (IS_RT3290(pAd))
	{
		if (Command == WAKE_MCU_CMD)
		{
			RTMPEnableWlan(pAd, TRUE, FALSE);
		}
		else if (Command == SLEEP_MCU_CMD)
		{
			WPDMA_GLO_CFG_STRUC GloCfg;

			RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
			GloCfg.field.EnableTxDMA = 0;
			RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, GloCfg.word);

			// wait TX DMA idle
			i = 0;
			do
			{
				RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
				if (GloCfg.field.TxDMABusy == 0)
					break;
				RtmpusecDelay(10);
				i++;
			}while(i < 1000);

			if (i >= 1000)
			{
				DBGPRINT(RT_DEBUG_ERROR, (" %s: [boundary]DMA Tx keeps busy.  %d\n", __FUNCTION__, i ));
			}			
		}
	}
#endif /* RT3290 */

	/* 3090F power solution 3 has hw limitation that needs to ban all mcu command */
	/* when firmware is in radio state.  For other chip doesn't have this limitation. */
	if ((((IS_RT3090(pAd) || IS_RT3572(pAd) || IS_RT3390(pAd) 
		|| IS_RT3593(pAd) || IS_RT5390(pAd) || IS_RT5392(pAd)) 
		&& IS_VERSION_AFTER_F(pAd)) || IS_RT5592(pAd) || IS_RT3290(pAd))
		&& (pAd->StaCfg.PSControl.field.rt30xxPowerMode == 3) 
		&& (pAd->StaCfg.PSControl.field.EnableNewPS == TRUE))
	{

		RTMP_SEM_LOCK(&pAd->McuCmdLock);

		if ((pAd->brt30xxBanMcuCmd == TRUE)
			&& (Command != WAKE_MCU_CMD) && (Command != RFOFF_MCU_CMD))
		{

			DBGPRINT(RT_DEBUG_TRACE, (" Ban Mcu Cmd %x in sleep mode\n",  Command));
			RTMP_SEM_UNLOCK(&pAd->McuCmdLock);
			return FALSE;
		}
		else if ((Command == SLEEP_MCU_CMD)
			||(Command == RFOFF_MCU_CMD))
		{
			pAd->brt30xxBanMcuCmd = TRUE;
		}
		else if (Command != WAKE_MCU_CMD)
		{
			pAd->brt30xxBanMcuCmd = FALSE;
		}

		RTMP_SEM_UNLOCK(&pAd->McuCmdLock);


	}

#if 0
	if (Command == SLEEP_MCU_CMD)
	{
		POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

		/* Write L1 latency*/
		if ((pAd->StaCfg.PSControl.field.AMDNewPSOn == TRUE) && IS_DEV_RT3xxx(pAd) 
			&& (pAd->HostVendor != PCIBUS_INTEL_VENDOR))
		{
			offset = 0x70F;
/*			Configuration = RTMPReadCBConfigXP(pAd, offset);*/
			pci_read_config_dword(pObj->pci_dev, offset, &Configuration);
			Configuration &= 0xffffff00;
			Configuration |= (0x13);	/* set Latency to default*/
/*			RTMPWriteCBConfigXP(pAd, offset, Configuration);*/
			pci_write_config_dword(pObj->pci_dev, offset, &Configuration);
			DBGPRINT(RT_DEBUG_TRACE, ("Write 70f; offset = %x, Configuration = %x. \n", offset, Configuration));
		}

		pAd->LastMCUCmd = Command;
	}
	else if (Command == WAKE_MCU_CMD)
	{
		/* Write L1 latency*/
		if ((pAd->StaCfg.PSControl.field.AMDNewPSOn == TRUE) && IS_DEV_RT3xxx(pAd) 
			&& (pAd->HostVendor != PCIBUS_INTEL_VENDOR))
		{
			offset = 0x70F;
/*			Configuration = RTMPReadCBConfigXP(pAd, offset);*/
			pci_read_config_dword(pObj->pci_dev, offset, &Configuration);
			Configuration &= 0xffffff00;
			Configuration |= (0x7F);	/* Set to long latency*/
/*			RTMPWriteCBConfigXP(pAd, offset, Configuration);*/
			pci_write_config_dword(pObj->pci_dev, offset, &Configuration);
			DBGPRINT(RT_DEBUG_TRACE, ("RadioOnExec restore 70f; offset = %x, Configuration = %x. \n", offset, Configuration));
		}
	}
#endif /* 0 */
#if defined(RT5390) || defined(RT5392) || defined(RT5592)
	if (Command == SLEEP_MCU_CMD)
        {
                /* Write L1 latency */
		if ((pAd->StaCfg.PSControl.field.AMDNewPSOn == TRUE) &&
			((IS_RT30xx(pAd) ||IS_RT5390(pAd) || IS_RT5392(pAd) || IS_RT5592(pAd))
			&& (pAd->HostVendor != PCIBUS_INTEL_VENDOR)))
		{
			offset = 0x70F;
			pci_read_config_word(((POS_COOKIE)pAd->OS_Cookie)->pci_dev, offset, &Configuration); 
			Configuration=le2cpu16(Configuration);
			Configuration &= 0xffffff00;
			Configuration |= (0x13);	/* set Latency to default */
			Configuration=le2cpu16(Configuration);
			pci_write_config_word(((POS_COOKIE)pAd->OS_Cookie)->pci_dev, offset, Configuration);
			DBGPRINT(RT_DEBUG_TRACE, ("Write 70f; offset = %x, Configuration = %x. \n", offset, Configuration));
		}
		pAd->LastMCUCmd = Command;
        }
        else if (Command == WAKE_MCU_CMD)
	{
		/* Write L1 latency */
		if ((pAd->StaCfg.PSControl.field.AMDNewPSOn == TRUE) && (IS_RT3090(pAd) || IS_RT3572(pAd) 
			|| IS_RT3390(pAd) || IS_RT3593(pAd) || IS_RT5390(pAd) || IS_RT5392(pAd)
			|| IS_RT5592(pAd))
			&& (pAd->HostVendor != PCIBUS_INTEL_VENDOR))
		{
			offset = 0x70F;
			/* Configuration = RTMPReadCBConfigXP(pAd, offset); */
			pci_read_config_word(((POS_COOKIE)pAd->OS_Cookie)->pci_dev, offset, &Configuration);
			Configuration=le2cpu16(Configuration);

			Configuration &= 0xffffff00;
			Configuration |= (0x7F);	// Set to long latency
			Configuration=le2cpu16(Configuration);

			pci_write_config_word(((POS_COOKIE)pAd->OS_Cookie)->pci_dev, offset, Configuration);

			DBGPRINT(RT_DEBUG_TRACE, ("RadioOnExec restore 70f; offset = %x, Configuration = %x. \n", offset, Configuration));
		}
	}

#endif /* defined(RT5390) || defined(RT5392) */

	if (((IS_RT3090(pAd) || IS_RT3572(pAd) || IS_RT3390(pAd) 
		|| IS_RT3593(pAd) || IS_RT5390(pAd) || IS_RT5392(pAd)
		|| IS_RT5592(pAd)) 
		&& IS_VERSION_AFTER_F(pAd)
		&& (pAd->StaCfg.PSControl.field.rt30xxPowerMode == 3) 
		&& (pAd->StaCfg.PSControl.field.EnableNewPS == TRUE)
		&& (Command == WAKE_MCU_CMD))
#ifdef RT3290
		|| (IS_RT3290(pAd) && (Command == WAKE_MCU_CMD))
#endif /* RT3290 */
	)
	{
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT
		if (FlgIsNeedLocked == TRUE)
		{
		           RTMP_MAC_SHR_MSEL_PROTECT_LOCK(pAd, IrqFlags);
		}
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */
		/* don't check MailBox for 0x84, 0x31*/
		if ((Command != 0x84) && (Command != WAKE_MCU_CMD))
		{
			do
			{
				RTMP_IO_FORCE_READ32(pAd, H2M_MAILBOX_CSR, &H2MMailbox.word);
				if (H2MMailbox.field.Owner == 0)
					break;

				if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))				
				{
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT	
					if (FlgIsNeedLocked == TRUE)
						RTMP_MAC_SHR_MSEL_PROTECT_UNLOCK(pAd, IrqFlags);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */
					return FALSE;
				}
				RtmpusecDelay(2);
				DBGPRINT(RT_DEBUG_INFO, ("AsicSendCommanToMcu::Mail box is busy\n"));
			} while(i++ < 100);

			if (i >= 100)
			{
				DBGPRINT_ERR(("H2M_MAILBOX still hold by MCU. command fail\n"));
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT	
				if (FlgIsNeedLocked == TRUE)
					RTMP_MAC_SHR_MSEL_PROTECT_UNLOCK(pAd, IrqFlags);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */
					return FALSE;
			}
		}

		H2MMailbox.field.Owner = 1;	   /* pass ownership to MCU*/
		H2MMailbox.field.CmdToken = Token;
		H2MMailbox.field.HighByte = Arg1;
		H2MMailbox.field.LowByte = Arg0;
		RTMP_IO_FORCE_WRITE32(pAd, H2M_MAILBOX_CSR, H2MMailbox.word);

		H2MCmd.word = 0;
		H2MCmd.field.HostCommand = Command;
		RTMP_IO_FORCE_WRITE32(pAd, HOST_CMD_CSR, H2MCmd.word);
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT
			if (FlgIsNeedLocked == TRUE)
				RTMP_MAC_SHR_MSEL_PROTECT_UNLOCK(pAd, IrqFlags);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */

	}
	else
#endif /* PCIE_PS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
	{
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT
		if (FlgIsNeedLocked == TRUE)
		{
		           RTMP_MAC_SHR_MSEL_PROTECT_LOCK(pAd, IrqFlags);
		}
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
		if (IS_USB_INF(pAd) && (!FlgIsNeedLocked))
		{
			RTMP_SEM_EVENT_WAIT(&pAd->reg_atomic, ret);
			if (ret != 0)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
				return FALSE;
			}
		}
#endif /* RTMP_MAC_USB */

		ret = FALSE;
	do
	{
		RTMP_IO_READ32(pAd, H2M_MAILBOX_CSR, &H2MMailbox.word);
		if (H2MMailbox.field.Owner == 0)
			break;

		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))				
		{
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT	
			if (FlgIsNeedLocked == TRUE)
				RTMP_MAC_SHR_MSEL_PROTECT_UNLOCK(pAd, IrqFlags);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */
				goto done;
		}
		RtmpusecDelay(2);
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("AsicSendCommandToMcu::Mail box is busy\n"));
#endif /* RELEASE_EXCLUDE */
	} while(i++ < 100);

	if (i >= 100)
	{
#ifdef RTMP_MAC_PCI
#ifdef CONFIG_ATE
			if (IS_PCI_INF(pAd) && (pAd->ate.bFWLoading == TRUE))
				{
			; /* wait for firmware loading */
		}
		else
#endif /* CONFIG_ATE */
#endif /* RTMP_MAC_PCI */
		{
		DBGPRINT_ERR(("H2M_MAILBOX still hold by MCU. command fail\n"));
		}
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT	
		if (FlgIsNeedLocked == TRUE)
			RTMP_MAC_SHR_MSEL_PROTECT_UNLOCK(pAd, IrqFlags);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */
			goto done;
	}
#ifdef RTMP_MAC_PCI
#ifdef CONFIG_ATE
		else if (IS_PCI_INF(pAd) && (pAd->ate.bFWLoading == TRUE))
	{
		/* mail box is not busy anymore */
		/* reloading of firmware is completed */
		pAd->ate.bFWLoading = FALSE;
	}
#endif /* CONFIG_ATE */
#endif /* RTMP_MAC_PCI */

	H2MMailbox.field.Owner	  = 1;	   /* pass ownership to MCU*/
	H2MMailbox.field.CmdToken = Token;
	H2MMailbox.field.HighByte = Arg1;
	H2MMailbox.field.LowByte  = Arg0;
	RTMP_IO_WRITE32(pAd, H2M_MAILBOX_CSR, H2MMailbox.word);

	H2MCmd.word 			  = 0;
	H2MCmd.field.HostCommand  = Command;
	RTMP_IO_WRITE32(pAd, HOST_CMD_CSR, H2MCmd.word);
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT
			if (FlgIsNeedLocked == TRUE)
				RTMP_MAC_SHR_MSEL_PROTECT_UNLOCK(pAd, IrqFlags);
#endif // SPECIFIC_BCN_BUF_SUPPORT //
#endif /* RTMP_MAC_PCI */
#ifdef RELEASE_EXCLUDE
	if (Command != 0x80)
	{
		DBGPRINT(RT_DEBUG_INFO, ("SW interrupt MCU (cmd=0x%02x, token=0x%02x, arg1,arg0=0x%02x,0x%02x)\n",
			H2MCmd.field.HostCommand, Token, Arg1, Arg0));
	}
#endif /* RELEASE_EXCLUDE */
}

#ifdef CONFIG_STA_SUPPORT
#ifdef PCIE_PS_SUPPORT
	/* 3090 MCU Wakeup command needs more time to be stable. */
	/* Before stable, don't issue other MCU command to prevent from firmware error.*/
	if ((((IS_RT3090(pAd) || IS_RT3572(pAd) || IS_RT3390(pAd) 
		|| IS_RT3593(pAd) || IS_RT5390(pAd) || IS_RT5392(pAd)) && IS_VERSION_AFTER_F(pAd)) || IS_RT5592(pAd) || IS_RT3290(pAd))
		&& (pAd->StaCfg.PSControl.field.rt30xxPowerMode == 3) 
		&& (pAd->StaCfg.PSControl.field.EnableNewPS == TRUE))
	{
		/*Put this is after RF programming.  */
		/*NdisAcquireSpinLock(&pAd->McuCmdLock);*/
		/*pAd->brt30xxBanMcuCmd = FALSE;*/
		/*NdisReleaseSpinLock(&pAd->McuCmdLock);*/
		switch (Command)
		{
			case WAKE_MCU_CMD :
				RtmpusecDelay(2500);

				if ((pAd->StaCfg.PSControl.field.rt30xxPowerMode == 3) &&
					(pAd->StaCfg.PSControl.field.EnableNewPS == TRUE))
				{
					/* Put this is after RF program. */ 
					pAd->brt30xxBanMcuCmd = FALSE;
				}
				break;
			case SLEEP_MCU_CMD :
				RtmpusecDelay(2000);
				break;
	}
	}

#endif /* PCIE_PS_SUPPORT */	
#endif /* CONFIG_STA_SUPPORT */

	if (Command == WAKE_MCU_CMD)
		pAd->LastMCUCmd = Command;

	ret = TRUE;

done:
#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd) && (!FlgIsNeedLocked))
	{
		RTMP_SEM_EVENT_UP(&pAd->reg_atomic);
	}
#endif /* RTMP_MAC_USB */

	return ret;
}


INT M51PwrSavingOP(PRTMP_ADAPTER pAd, UINT32 PwrOP, UINT32 PwrLevel, 
					 UINT32 ListenInterval, UINT32 PreTBTTLeadTime,
					 UINT8 TIMByteOffset, UINT8 TIMBytePattern)
{
	BOOLEAN brc;
	UINT RetryRound = 0;

	if (PwrOP == RADIO_ON)
	{
		/* 1. Send wake up command.*/
		RetryRound = 0;

		do
		{
			brc = AsicSendCommandToMcu(pAd, RADIO_ON, PowerWakeCID, 0x00, 0x02, FALSE);   
			if (brc)
			{
				/* Wait command ok.*/
				brc = AsicCheckCommandOk(pAd, PowerWakeCID);
			}
			if(brc){
				break;      /* PowerWakeCID cmd successed*/
			}
			DBGPRINT(RT_DEBUG_WARN, ("PSM :WakeUp Cmd Failed, retry %d\n", RetryRound));

			/* try 10 times at most*/
			if ((RetryRound++) > 10)
				break;
			/* delay and try again*/
			RtmpusecDelay(200);
		} while (TRUE);

		if (RetryRound > 10)
			DBGPRINT(RT_DEBUG_WARN, ("PSM :ASIC 0x31 WakeUp Cmd may Fail %d*******\n", RetryRound));
	
	}
	else if (PwrOP == RADIO_OFF) 
	{
		AsicSendCommandToMcu(pAd, RADIO_OFF, 0xff, 0xff, 0x02, FALSE);   /* send POWER-SAVE command to MCU. Timeout 40us.*/
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Unknown %s:PwrOP(%d)\n", __FUNCTION__, PwrOP));
	}

}

void M51InitLEDMode(IN RTMP_ADAPTER *pAd)
{
	PLED_CONTROL pLedCntl = &pAd->LedCntl;

	if (pLedCntl->MCULedCntl.word == 0xFF)
	{
		pLedCntl->MCULedCntl.word = 0x01;
		pLedCntl->LedAGCfg = 0x5555;
		pLedCntl->LedACTCfg= 0x2221;

#ifdef RTMP_MAC_PCI
		pLedCntl->LedPolarity = 0xA9F8;
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		pLedCntl->LedPolarity = 0x5627; 
#endif /* RTMP_MAC_USB */
	}
	
	AsicSendCommandToMcu(pAd, MCU_SET_LED_AG_CFG, 0xff, (UCHAR)pLedCntl->LedAGCfg, (UCHAR)(pLedCntl->LedAGCfg >> 8), FALSE);
	AsicSendCommandToMcu(pAd, MCU_SET_LED_ACT_CFG, 0xff, (UCHAR)pLedCntl->LedACTCfg, (UCHAR)(pLedCntl->LedACTCfg >> 8), FALSE);
	AsicSendCommandToMcu(pAd, MCU_SET_LED_POLARITY, 0xff, (UCHAR)pLedCntl->LedPolarity, (UCHAR)(pLedCntl->LedPolarity >> 8), FALSE);
	AsicSendCommandToMcu(pAd, MCU_SET_LED_GPIO_SIGNAL_CFG, 0xff, 0, pLedCntl->MCULedCntl.field.Polarity, FALSE);

	pAd->LedCntl.LedIndicatorStrength = 0xFF;
	RTMPSetSignalLED(pAd, -100);	/* Force signal strength Led to be turned off, before link up */

	RTMPStartLEDMode(pAd);
}

#ifdef RTMP_PCI_SUPPORT
void m51_pci_fw_init(RTMP_ADAPTER *ad)
{
		/* initialize BBP R/W access agent*/
		RTMP_IO_WRITE32(pAd, H2M_BBP_AGENT, 0);
		RTMP_IO_WRITE32(pAd, H2M_MAILBOX_CSR, 0);

		/* fix the dead rf frequency offset */
		AsicSendCommandToMcu(pAd, 0x72, 0, 0, 0, FALSE);

		RtmpusecDelay(1000);
		pAd->LastMCUCmd = 0x72;
}
#endif /* RTMP_PCI_SUPPORT */

#ifdef RTMP_USB_SUPPORT
void m51_usb_fw_init(RTMP_ADAPTER *ad)
{
		/* initialize BBP R/W access agent. */
		RTUSBWriteMACRegister(pAd, H2M_BBP_AGENT, 0, FALSE);
		RTUSBWriteMACRegister(pAd,H2M_MAILBOX_CSR,0, FALSE);
		RTUSBWriteMACRegister(pAd, H2M_INT_SRC, 0, FALSE);
		/* reset rf by MCU supported by new firmware */
		AsicSendCommandToMcu(pAd, 0x72, 0x00, 0x00, 0x00, FALSE);

		RtmpusecDelay(1000);
		pAd->LastMCUCmd = 0x72;
}
#endif /* RTMP_USB_SUPPORT */

