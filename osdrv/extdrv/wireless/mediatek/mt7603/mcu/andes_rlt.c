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
	andes_rlt.c

	Abstract:
	on-chip CPU related codes

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/

#include	"rt_config.h"
#ifdef RTMP_PCI_SUPPORT
INT32 AndesRltPciLoadRomPatch(RTMP_ADAPTER *ad)
{
	int ret = NDIS_STATUS_SUCCESS;
	UINT32 start_offset, end_offset;
	UINT32 loop = 0, idx = 0, val = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	UINT32 mac_value;
	POS_COOKIE obj = (POS_COOKIE)ad->OS_Cookie;
	
	if (cap->rom_code_protect) {
loadfw_protect:
		RTMP_IO_READ32(ad, SEMAPHORE_03, &mac_value);
		loop++;

		if (((mac_value & 0x01) == 0x00) && (loop < GET_SEMAPHORE_RETRY_MAX)) {
			RtmpOsMsDelay(1);
			goto loadfw_protect;
		}
		
		if (loop >= GET_SEMAPHORE_RETRY_MAX) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s: can not get the hw semaphore\n", __FUNCTION__));
			return NDIS_STATUS_FAILURE;
		}
	}

	/* check rom patch if ready */
	if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3)) {
		RTMP_IO_READ32(ad, CLOCK_CTL, &mac_value);
	} else {
		RTMP_IO_READ32(ad, COM_REG0, &mac_value);
	}

	if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3)) {
		if (((mac_value & 0x01) == 0x01) && (cap->rom_code_protect)) {
			goto done;
		}
	} else {
		if (((mac_value & 0x02) == 0x02) && (cap->rom_code_protect)) {
			goto done;
		}
	}
	
	if (cap->load_code_method == BIN_FILE_METHOD) {
		DBGPRINT(RT_DEBUG_OFF, ("load rom patch from /lib/firmware/%s\n", cap->rom_patch_bin_file_name));
		OS_LOAD_CODE_FROM_BIN(&cap->rom_patch, cap->rom_patch_bin_file_name, obj->pci_dev, &cap->rom_patch_len); 
	} else {
		cap->rom_patch = cap->rom_patch_header_image;
	}

	if (!cap->rom_patch) {
		if (cap->load_code_method == BIN_FILE_METHOD) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Please assign a rom patch(/lib/firmware/%s), load_method(%d)\n", __FUNCTION__, cap->rom_patch_bin_file_name, cap->load_code_method));
		} else {
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Please assign a rom patch, load_method(%d)\n", 
				__FUNCTION__, cap->load_code_method));
		}
		ret = NDIS_STATUS_FAILURE;
		goto done;
	}

	/* get rom patch information */
	DBGPRINT(RT_DEBUG_OFF, ("build time = \n")); 
	
	for (loop = 0; loop < 16; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + loop)));

	if (IS_MT76x2(ad)) {
		if (((strncmp(cap->rom_patch, "20130809", 8) >= 0)) && (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3))) {
			DBGPRINT(RT_DEBUG_OFF, ("rom patch for E3 IC\n"));

		} else if (((strncmp(cap->rom_patch, "20130809", 8) < 0)) && (MT_REV_LT(ad, MT76x2, REV_MT76x2E3))){

			DBGPRINT(RT_DEBUG_OFF, ("rom patch for E2 IC\n"));
		} else {
			DBGPRINT(RT_DEBUG_OFF, ("rom patch do not match IC version\n"));
			RTMP_IO_READ32(ad, 0x0, &mac_value);
			DBGPRINT(RT_DEBUG_OFF, ("IC version(%x)\n", mac_value));
			ret = NDIS_STATUS_FAILURE;
			goto done;
		}
	}
	
	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	DBGPRINT(RT_DEBUG_OFF, ("platform = \n"));

	for (loop = 0; loop < 4; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + 16 + loop)));
	
	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	DBGPRINT(RT_DEBUG_OFF, ("hw/sw version = \n"));

	for (loop = 0; loop < 4; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + 20 + loop)));
	
	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	DBGPRINT(RT_DEBUG_OFF, ("patch version = \n"));

	for (loop = 0; loop < 4; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + 24 + loop)));

	DBGPRINT(RT_DEBUG_OFF, ("\n"));
		

	RTMP_IO_WRITE32(ad, PCIE_REMAP_BASE4, cap->rom_patch_offset - 10000);
		
	start_offset = PATCH_INFO_SIZE;
	end_offset = cap->rom_patch_len;
	
	/* Load rom patch code */
	for (idx = start_offset; idx < end_offset; idx += 4)
	{
		val = (*(cap->rom_patch + idx)) +
		   (*(cap->rom_patch + idx + 3) << 24) +
		   (*(cap->rom_patch + idx + 2) << 16) +
		   (*(cap->rom_patch + idx + 1) << 8);

		RTMP_IO_WRITE32(ad, 0x90000 + (idx - PATCH_INFO_SIZE), val);
	}
	
	RTMP_IO_WRITE32(ad, PCIE_REMAP_BASE4, 0x0);

	/* Trigger rom */
	RTMP_IO_WRITE32(ad, INT_LEVEL, 0x04);
	
	/* check rom if ready */
	loop = 0;
	do
	{
		if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3)) {
			RTMP_IO_READ32(ad, CLOCK_CTL, &mac_value);
			if ((mac_value & 0x01)== 0x01)
				break;
		} else {
			RTMP_IO_READ32(ad, COM_REG0, &mac_value);
			if ((mac_value & 0x02)== 0x02)
				break;
		}

		RtmpOsMsDelay(10);
		loop++;
	} while (loop <= 200);

	if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3)) {
		DBGPRINT(RT_DEBUG_TRACE, ("%s: CLOCK_CTL(0x%x) = 0x%x\n", __FUNCTION__, CLOCK_CTL, mac_value));
		if ((mac_value & 0x01) != 0x01)
		ret = NDIS_STATUS_FAILURE;
	} else {
		DBGPRINT(RT_DEBUG_TRACE, ("%s: COM_REG0(0x%x) = 0x%x\n", __FUNCTION__, COM_REG0, mac_value));
		if ((mac_value & 0x02) != 0x02)
			ret = NDIS_STATUS_FAILURE;
	}	

done:
	if (cap->rom_code_protect)
		RTMP_IO_WRITE32(ad, SEMAPHORE_03, 0x1);
	
	return ret;
}


INT32 AndesRltPciEraseRomPatch(RTMP_ADAPTER *ad)
{
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	
	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));

	if (cap->load_code_method == BIN_FILE_METHOD) {
		if (cap->rom_patch)
			os_free_mem(NULL, cap->rom_patch);
			cap->rom_patch = NULL;
	}

	return 0;
}


INT32 AndesRltPciEraseFw(RTMP_ADAPTER *ad)
{
	int ret = NDIS_STATUS_SUCCESS;
	UINT32 ilm_len, dlm_len;
	UINT16 fw_ver, build_ver;
	UINT32 loop = 0, idx = 0, val = 0;
	UINT32 mac_value;
	UINT32 start_offset, end_offset;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	
	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));

	if (!ad->chipCap.ram_code_protect) {

		ilm_len = (*(cap->FWImageName + 3) << 24) | (*(cap->FWImageName + 2) << 16) |
				 (*(cap->FWImageName + 1) << 8) | (*cap->FWImageName);

		dlm_len = (*(cap->FWImageName + 7) << 24) | (*(cap->FWImageName + 6) << 16) |
				 (*(cap->FWImageName + 5) << 8) | (*(cap->FWImageName + 4));

		fw_ver = (*(cap->FWImageName + 11) << 8) | (*(cap->FWImageName + 10));

		build_ver = (*(cap->FWImageName + 9) << 8) | (*(cap->FWImageName + 8));
	
		DBGPRINT(RT_DEBUG_TRACE, ("FW Version:%d.%d.%02d ", (fw_ver & 0xf000) >> 8,
							(fw_ver & 0x0f00) >> 8, fw_ver & 0x00ff));
		DBGPRINT(RT_DEBUG_TRACE, ("Build:%x\n", build_ver));
		DBGPRINT(RT_DEBUG_TRACE, ("Build Time:"));

		for (loop = 0; loop < 16; loop++)
			DBGPRINT(RT_DEBUG_TRACE, ("%c", *(cap->FWImageName + 16 + loop)));

		DBGPRINT(RT_DEBUG_TRACE, ("\n"));

		DBGPRINT(RT_DEBUG_TRACE, ("ILM Length = %d(bytes)\n", ilm_len));
		DBGPRINT(RT_DEBUG_TRACE, ("DLM Length = %d(bytes)\n", dlm_len));
	
		RTMP_IO_WRITE32(ad, PCIE_REMAP_BASE4, cap->ilm_offset);

		if (IS_MT76x2(ad)) {
			start_offset = FW_INFO_SIZE;
			RTMP_IO_WRITE32(ad, INT_LEVEL, 0x1);
			RtmpOsMsDelay(20);
		} else {
			if (cap->ram_code_protect) {
				RTMP_IO_WRITE32(ad, CPU_CTL, 0x0);
				RTMP_IO_WRITE32(ad, CPU_CTL, (0x1<<20));
				RTMP_IO_WRITE32(ad, RESET_CTL, 0x10);
				RTMP_IO_WRITE32(ad, RESET_CTL, 0x300);
				RTMP_IO_WRITE32(ad, COM_REG0, 0x0);
	
				start_offset = FW_INFO_SIZE + IV_SIZE;
			} else {
				RTMP_IO_WRITE32(ad, RESET_CTL, 0x10); /* reset fce */
				RTMP_IO_WRITE32(ad, RESET_CTL, 0x200); /* reset cpu */
				RTMP_IO_WRITE32(ad, COM_REG0, 0x0); /* clear mcu ready bit */

				start_offset = FW_INFO_SIZE;
			}
		}

		end_offset = FW_INFO_SIZE + ilm_len;

		if (!IS_MT76x2(ad)) {
			/* erase ilm */
			for (idx = start_offset; idx < end_offset; idx += 4) {
				val = (*(cap->FWImageName + idx)) +
				   (*(cap->FWImageName + idx + 3) << 24) +
				   (*(cap->FWImageName + idx + 2) << 16) +
				   (*(cap->FWImageName + idx + 1) << 8);

				RTMP_IO_WRITE32(ad, 0x80000 + (idx - FW_INFO_SIZE), 0);
			}

			if (cap->ram_code_protect) {
				/* Loading IV part into last 64 bytes of ILM */
				start_offset = FW_INFO_SIZE;
				end_offset = FW_INFO_SIZE + IV_SIZE;
	
				for (idx = start_offset; idx < end_offset; idx += 4)
				{
					val = (*(cap->FWImageName + idx)) +
						(*(cap->FWImageName + idx + 3) << 24) +
						(*(cap->FWImageName + idx + 2) << 16) +
						(*(cap->FWImageName + idx + 1) << 8);

					RTMP_IO_WRITE32(ad, 0x80000 + (0x54000 - IV_SIZE) + (idx - FW_INFO_SIZE), 0);
				}
			}

			RTMP_IO_WRITE32(ad, PCIE_REMAP_BASE4, cap->dlm_offset);

			start_offset = 32 + ilm_len;
			end_offset = 32 + ilm_len + dlm_len;
	
			/* erase dlm */
			for (idx = start_offset; idx < end_offset; idx += 4) {
				val = (*(cap->FWImageName + idx)) +
					(*(cap->FWImageName + idx + 3) << 24) +
					(*(cap->FWImageName + idx + 2) << 16) +
					(*(cap->FWImageName + idx + 1) << 8);

				RTMP_IO_WRITE32(ad, 0x80000 + (0x54000 - IV_SIZE) + (idx - FW_INFO_SIZE), 0);
			}
		}
	
		RTMP_IO_READ32(ad, COM_REG0, &mac_value);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: COM_REG0(0x%x) = 0x%x\n", __FUNCTION__, COM_REG0, mac_value));
		RTMP_IO_READ32(ad, RESET_CTL, &mac_value);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: RESET_CTL(0x%x) = 0x%x\n", __FUNCTION__, RESET_CTL, mac_value));
	}
	
	if (cap->load_code_method == BIN_FILE_METHOD) {
		if (cap->FWImageName)
			os_free_mem(NULL, cap->FWImageName);
			cap->FWImageName = NULL;
	}

	return ret;
}


NDIS_STATUS AndesRltPciLoadFw(RTMP_ADAPTER *ad)
{
	int ret = NDIS_STATUS_SUCCESS;
	UINT32 ilm_len, dlm_len;
	USHORT fw_ver, build_ver;
	UINT32 loop = 0, idx = 0, val = 0;
	UINT32 mac_value;
	UINT32 start_offset, end_offset;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	POS_COOKIE obj = (POS_COOKIE)ad->OS_Cookie;

	if (cap->ram_code_protect) {
loadfw_protect:
		RTMP_IO_READ32(ad, SEMAPHORE_00, &mac_value);
		loop++;

		if (((mac_value & 0x01) == 0) && (loop < GET_SEMAPHORE_RETRY_MAX)) {
			RtmpOsMsDelay(1);
			goto loadfw_protect;
		}

		if (loop >= GET_SEMAPHORE_RETRY_MAX) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s: can not get the hw semaphore\n", __FUNCTION__));
			return NDIS_STATUS_FAILURE;
		}
	}

	/* check MCU if ready */
	RTMP_IO_READ32(ad, COM_REG0, &mac_value);

	if (((mac_value & 0x01) == 0x01) && (cap->ram_code_protect)) {
		goto done;
	}
	
	if (cap->load_code_method == BIN_FILE_METHOD) {
		DBGPRINT(RT_DEBUG_OFF, ("load fw image from /lib/firmware/%s\n", cap->fw_bin_file_name));
		OS_LOAD_CODE_FROM_BIN(&cap->FWImageName, cap->fw_bin_file_name, obj->pci_dev, &cap->fw_len);
	} else {
		cap->FWImageName = cap->fw_header_image;
	}

	if (!cap->FWImageName) {
		if (cap->load_code_method == BIN_FILE_METHOD) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Please assign a fw image(/lib/firmware/%s), load_method(%d)\n", __FUNCTION__, cap->fw_bin_file_name, cap->load_code_method));
		} else {
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Please assign a fw image, load_method(%d)\n", 
				__FUNCTION__, cap->load_code_method));
		}
		ret = NDIS_STATUS_FAILURE;
		goto done;
	}
	
	ilm_len = (*(cap->FWImageName + 3) << 24) | (*(cap->FWImageName + 2) << 16) |
			 (*(cap->FWImageName + 1) << 8) | (*cap->FWImageName);

	dlm_len = (*(cap->FWImageName + 7) << 24) | (*(cap->FWImageName + 6) << 16) |
			 (*(cap->FWImageName + 5) << 8) | (*(cap->FWImageName + 4));

	fw_ver = (*(cap->FWImageName + 11) << 8) | (*(cap->FWImageName + 10));

	build_ver = (*(cap->FWImageName + 9) << 8) | (*(cap->FWImageName + 8));
	
	DBGPRINT(RT_DEBUG_OFF, ("FW Version:%d.%d.%02d ", (fw_ver & 0xf000) >> 8,
						(fw_ver & 0x0f00) >> 8, fw_ver & 0x00ff));
	DBGPRINT(RT_DEBUG_OFF, ("Build:%x\n", build_ver));
	DBGPRINT(RT_DEBUG_OFF, ("Build Time:"));

	for (loop = 0; loop < 16; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->FWImageName + 16 + loop)));

	DBGPRINT(RT_DEBUG_OFF, ("\n"));
	
	if (IS_MT76x2(ad)) {
		if (((strncmp(cap->FWImageName + 16, "20130811", 8) >= 0)) && (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3))) {
			DBGPRINT(RT_DEBUG_OFF, ("fw for E3 IC\n"));

		} else if (((strncmp(cap->FWImageName + 16, "20130811", 8) < 0)) && (MT_REV_LT(ad, MT76x2, REV_MT76x2E3))){

			DBGPRINT(RT_DEBUG_OFF, ("fw for E2 IC\n"));
		} else {
			DBGPRINT(RT_DEBUG_OFF, ("fw do not match IC version\n"));
			RTMP_IO_READ32(ad, 0x0, &mac_value);
			DBGPRINT(RT_DEBUG_OFF, ("IC version(%x)\n", mac_value));
			ret = NDIS_STATUS_FAILURE;
			goto done;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("ILM Length = %d(bytes)\n", ilm_len));
	DBGPRINT(RT_DEBUG_TRACE, ("DLM Length = %d(bytes)\n", dlm_len));
	
	RTMP_IO_WRITE32(ad, PCIE_REMAP_BASE4, cap->ilm_offset);

	if (cap->ram_code_protect)
		start_offset = FW_INFO_SIZE + IV_SIZE;
	else
		start_offset = FW_INFO_SIZE;

	end_offset = FW_INFO_SIZE + ilm_len;
	
	/* Load ILM code */
	for (idx = start_offset; idx < end_offset; idx += 4)
	{
		val = (*(cap->FWImageName + idx)) +
		   (*(cap->FWImageName + idx + 3) << 24) +
		   (*(cap->FWImageName + idx + 2) << 16) +
		   (*(cap->FWImageName + idx + 1) << 8);

		RTMP_IO_WRITE32(ad, 0x80000 + (idx - FW_INFO_SIZE), val);
	}

	if (cap->ram_code_protect)
	{
		/* Loading IV part into last 64 bytes of ILM */
		start_offset = FW_INFO_SIZE;
		end_offset = FW_INFO_SIZE + IV_SIZE;
	
		for (idx = start_offset; idx < end_offset; idx += 4)
		{
			val = (*(cap->FWImageName + idx)) +
				(*(cap->FWImageName + idx + 3) << 24) +
				(*(cap->FWImageName + idx + 2) << 16) +
				(*(cap->FWImageName + idx + 1) << 8);

			RTMP_IO_WRITE32(ad, 0x80000 + (0x54000 - IV_SIZE) + (idx - FW_INFO_SIZE), val);
		}
	}

	if (IS_MT76x2(ad)) {
		RTMP_IO_WRITE32(ad, PCIE_REMAP_BASE4, cap->dlm_offset - 0x10000);
	} else
		RTMP_IO_WRITE32(ad, PCIE_REMAP_BASE4, cap->dlm_offset);

	start_offset = FW_INFO_SIZE + ilm_len;
	end_offset = FW_INFO_SIZE + ilm_len + dlm_len;
	
	/* Load DLM code */
	for (idx = start_offset; idx < end_offset; idx += 4)
	{
		val = (*(cap->FWImageName + idx)) +
		   (*(cap->FWImageName + idx + 3) << 24) +
		   (*(cap->FWImageName + idx + 2) << 16) +
		   (*(cap->FWImageName + idx + 1) << 8);
	
		if (IS_MT76x2(ad)) {
			if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3))
				RTMP_IO_WRITE32(ad, 0x90800 + (idx - FW_INFO_SIZE - ilm_len), val);
			else
				RTMP_IO_WRITE32(ad, 0x90000 + (idx - FW_INFO_SIZE - ilm_len), val);
		} else
			RTMP_IO_WRITE32(ad, 0x80000 + (idx - FW_INFO_SIZE - ilm_len), val);
	}

	RTMP_IO_WRITE32(ad, PCIE_REMAP_BASE4, 0x0);

	if (cap->ram_code_protect)
	{
		/* Trigger Firmware */
		RTMP_IO_WRITE32(ad, INT_LEVEL, 0x03);
	}
	else
	{
		if (IS_MT76x2(ad))
			RTMP_IO_WRITE32(ad, INT_LEVEL, 0x2);
		else
			RTMP_IO_WRITE32(ad, RESET_CTL, 0x300);
	}

	/* check MCU if ready */
	loop = 0;
	do
	{
		RTMP_IO_READ32(ad, COM_REG0, &mac_value);
		if ((mac_value & 0x01)== 0x1)
			break;
		RtmpOsMsDelay(10);
		loop++;
	} while (loop <= 200);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: COM_REG0(0x%x) = 0x%x\n", __FUNCTION__, COM_REG0, mac_value));
	
	RTMP_IO_READ32(ad, COM_REG0, &mac_value);
	mac_value |= (1 << 1);
	RTMP_IO_WRITE32(ad, COM_REG0, mac_value);

	if ((mac_value & 0x01 ) != 0x1)
		ret = NDIS_STATUS_FAILURE;

done:
	if (cap->ram_code_protect)
		RTMP_IO_WRITE32(ad, SEMAPHORE_00, 0x1);
	
	return ret;
}
#endif


#ifdef RTMP_MAC_USB
INT32 AndesRltUsbEnablePatch(RTMP_ADAPTER *ad)
{
	int ret = NDIS_STATUS_SUCCESS;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	
	/* enable patch command */
	UINT8 cmd[11];
	cmd[0] = 0x6F;
	cmd[1] = 0xFC;
	cmd[2] = 0x08;
	cmd[3] = 0x01;
	cmd[4] = 0x20;
	cmd[5] = 0x04;
	cmd[6] = 0x00;
	cmd[7] = (cap->rom_patch_offset & 0xFF);
	cmd[8] = (cap->rom_patch_offset & 0xFF00) >> 8;
	cmd[9] = (cap->rom_patch_offset & 0xFF0000) >> 16;
	cmd[10] = (cap->rom_patch_offset & 0xFF000000) >> 24;
	
	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));

	ret = RTUSB_VendorRequest(ad,
							  USBD_TRANSFER_DIRECTION_OUT,
							  DEVICE_CLASS_REQUEST_OUT,
							  0x01,
							  0x12,
							  0x00,
							  cmd,
							  11);

	return ret;
}


INT32 AndesRltUsbResetWmt(RTMP_ADAPTER *ad)
{
	int ret = NDIS_STATUS_SUCCESS;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	
	/* reset command */
	UINT8 cmd[8] = {0x6F, 0xFC, 0x05, 0x01, 0x07, 0x01, 0x00, 0x04}; 

	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));
	
	RTUSB_VendorRequest(ad,
						USBD_TRANSFER_DIRECTION_OUT,
						DEVICE_CLASS_REQUEST_OUT,
						0x01,
						0x12,
						0x00,
						cmd,
						8);

	return ret;
}


UINT16 Checksume16(UINT8 *pData, int len)
{
	int sum = 0;

	while (len > 1) {
		sum += *((UINT16*)pData);

		pData = pData + 2;
		
		if (sum & 0x80000000) 
			sum = (sum & 0xFFFF) + (sum >> 16);

		len -= 2;
	}

	if (len)
		sum += *((UINT8*)pData);

	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	return ~sum;
}


INT32 AndesRltUsbChkCrc(RTMP_ADAPTER *ad, UINT32 checksum_len)
{
	int ret = 0;
	UINT8 cmd[8];
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	
	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));

	NdisMoveMemory(cmd, &cap->rom_patch_offset, 4);
	NdisMoveMemory(&cmd[4], &checksum_len, 4);

	ret = RTUSB_VendorRequest(ad,
							  USBD_TRANSFER_DIRECTION_OUT,
							  DEVICE_VENDOR_REQUEST_OUT,
							  0x01,
							  0x20,
							  0x00,
							  cmd,
							  8);

	return ret;

}


UINT16 AndesRltUsbGetCrc(RTMP_ADAPTER *ad)
{
	int ret = 0;
	UINT16 crc, count = 0;

	while (1) {
	
		ret = RTUSB_VendorRequest(ad,
								 (USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK),
								 DEVICE_VENDOR_REQUEST_IN,
								 0x01,
								 0x21,
								 0x00,
								 &crc,
								 2);

		if (crc != 0xFFFF)
			break;

		RtmpOsMsDelay(100);
	
		if (count++ > 100) {
			DBGPRINT(RT_DEBUG_ERROR, ("Query CRC over %d times\n", count));
			break;
		}
	}

	return crc;
}


VOID UsbUploadRomPatchComplete(purbb_t urb, pregs *pt_regs)
{
	RTMP_OS_COMPLETION *load_rom_patch_done = (RTMP_OS_COMPLETION *)RTMP_OS_USB_CONTEXT_GET(urb);

	RTMP_OS_COMPLETE(load_rom_patch_done);
}


INT32 AndesRltUsbLoadRomPatch(RTMP_ADAPTER *ad)
{
	PURB urb;
	POS_COOKIE obj = (POS_COOKIE)ad->OS_Cookie;
	ra_dma_addr_t rom_patch_dma;
	PUCHAR rom_patch_data;
	TXINFO_NMAC_CMD *tx_info;	
	INT32 sent_len;
	UINT32 cur_len = 0;
	UINT32 mac_value, loop = 0;
	UINT16 value;
	int ret = 0, total_checksum = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	USB_DMA_CFG_STRUC cfg;
	UINT32 patch_len = 0;
	RTMP_OS_COMPLETION load_rom_patch_done;
	
	if (cap->rom_code_protect) {
load_patch_protect:
		RTMP_IO_READ32(ad, SEMAPHORE_03, &mac_value);
		loop++;

		if (((mac_value & 0x01) == 0x00) && (loop < GET_SEMAPHORE_RETRY_MAX)) {
			RtmpOsMsDelay(1);
			goto load_patch_protect;
		}

		if (loop >= GET_SEMAPHORE_RETRY_MAX) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s: can not get the hw semaphore\n", __FUNCTION__));
			return NDIS_STATUS_FAILURE;
		}
	}

	/* Check rom patch if ready */
	if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3)) {
		RTMP_IO_READ32(ad, CLOCK_CTL, &mac_value);

		if (((mac_value & 0x01) == 0x01) && (cap->rom_code_protect)) {
			goto error0;
		}
	} else {
		RTMP_IO_READ32(ad, COM_REG0, &mac_value);

		if (((mac_value & 0x02) == 0x02) && (cap->rom_code_protect)) {
			goto error0;
		}
	}
	
	/* Enable USB_DMA_CFG */
	USB_CFG_READ(ad, &cfg.word);
	cfg.word |= 0x00c00020;
	USB_CFG_WRITE(ad, cfg.word);
	
	if (cap->load_code_method == BIN_FILE_METHOD)
		OS_LOAD_CODE_FROM_BIN(&cap->rom_patch, cap->rom_patch_bin_file_name, obj->pUsb_Dev, &cap->rom_patch_len); 
	else	
		cap->rom_patch = cap->rom_patch_header_image;

	if (!cap->rom_patch) {
		if (cap->load_code_method == BIN_FILE_METHOD) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Please assign a rom patch(/lib/firmware/%s), load_method(%d)\n", __FUNCTION__, cap->rom_patch_bin_file_name, cap->load_code_method));
		} else {
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Please assign a rom patch, load_method(%d)\n", 
				__FUNCTION__, cap->load_code_method));
		}
		ret = NDIS_STATUS_FAILURE;
		goto error0;
	}

	RTUSBVenderReset(ad);
	RtmpOsMsDelay(5);

	/* get rom patch information */
	DBGPRINT(RT_DEBUG_OFF, ("build time = \n")); 
	
	for (loop = 0; loop < 16; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + loop)));
	
	if (IS_MT76x2(ad)) {
		if (((strncmp(cap->rom_patch, "20130809", 8) >= 0)) && (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3))) {
			DBGPRINT(RT_DEBUG_OFF, ("rom patch for E3 IC\n"));

		} else if (((strncmp(cap->rom_patch, "20130809", 8) < 0)) && (MT_REV_LT(ad, MT76x2, REV_MT76x2E3))){

			DBGPRINT(RT_DEBUG_OFF, ("rom patch for E2 IC\n"));
		} else {
			DBGPRINT(RT_DEBUG_OFF, ("rom patch do not match IC version\n"));
			RTMP_IO_READ32(ad, 0x0, &mac_value);
			DBGPRINT(RT_DEBUG_OFF, ("IC version(%x)\n", mac_value));
			ret = NDIS_STATUS_FAILURE;
			goto error0;
		}
	}
	
	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	DBGPRINT(RT_DEBUG_OFF, ("platform = \n"));

	for (loop = 0; loop < 4; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + 16 + loop)));
	
	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	DBGPRINT(RT_DEBUG_OFF, ("hw/sw version = \n"));

	for (loop = 0; loop < 4; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + 20 + loop)));
	
	DBGPRINT(RT_DEBUG_OFF, ("\n"));

	DBGPRINT(RT_DEBUG_OFF, ("patch version = \n"));

	for (loop = 0; loop < 4; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->rom_patch + 24 + loop)));

	DBGPRINT(RT_DEBUG_OFF, ("\n"));
	
	/* Enable FCE */
	RTUSBWriteMACRegister(ad, FCE_PSE_CTRL, 0x01, FALSE);

	/* FCE tx_fs_base_ptr */
	RTUSBWriteMACRegister(ad, TX_CPU_PORT_FROM_FCE_BASE_PTR, 0x400230, FALSE);

	/* FCE tx_fs_max_cnt */
	RTUSBWriteMACRegister(ad, TX_CPU_PORT_FROM_FCE_MAX_COUNT, 0x01, FALSE); 

	/* FCE pdma enable */
	RTUSBWriteMACRegister(ad, FCE_PDMA_GLOBAL_CONF, 0x44, FALSE);  

	/* FCE skip_fs_en */
	RTUSBWriteMACRegister(ad, FCE_SKIP_FS, 0x03, FALSE);
	
	/* Allocate URB */
	urb = RTUSB_ALLOC_URB(0);

	if (!urb)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate URB\n"));
		ret = NDIS_STATUS_RESOURCES; 
		goto error0;
	}

	/* Allocate TransferBuffer */
	rom_patch_data = RTUSB_URB_ALLOC_BUFFER(obj->pUsb_Dev, UPLOAD_PATCH_UNIT, &rom_patch_dma);
		
	if (!rom_patch_data)
	{
		ret = NDIS_STATUS_RESOURCES;
		goto error1;
	}

	DBGPRINT(RT_DEBUG_OFF, ("loading rom patch"));
	
	RTMP_OS_INIT_COMPLETION(&load_rom_patch_done);
		
	cur_len = 0x00;
	patch_len = cap->rom_patch_len - PATCH_INFO_SIZE;
	
	/* loading rom patch */
	while (1) {
		INT32 sent_len_max = UPLOAD_PATCH_UNIT - sizeof(*tx_info) - USB_END_PADDING;	
		sent_len = (patch_len - cur_len) >=  sent_len_max ? sent_len_max : (patch_len - cur_len);

		DBGPRINT(RT_DEBUG_OFF, ("patch_len = %d\n", patch_len));
		DBGPRINT(RT_DEBUG_OFF, ("cur_len = %d\n", cur_len));
		DBGPRINT(RT_DEBUG_OFF, ("sent_len = %d\n", sent_len));

		if (sent_len > 0) {
			tx_info = (TXINFO_NMAC_CMD *)rom_patch_data;
			tx_info->info_type = CMD_PACKET;
			tx_info->pkt_len = sent_len;
			tx_info->d_port = CPU_TX_PORT;

#ifdef RT_BIG_ENDIAN
			RTMPDescriptorEndianChange((PUCHAR)tx_info, TYPE_TXINFO);
#endif
			NdisMoveMemory(rom_patch_data + sizeof(*tx_info), cap->rom_patch + PATCH_INFO_SIZE + cur_len, sent_len);

			/* four zero bytes for end padding */	
			NdisZeroMemory(rom_patch_data + sizeof(*tx_info) + sent_len, 4);

			value = (cur_len + cap->rom_patch_offset) & 0xFFFF;

			DBGPRINT(RT_DEBUG_OFF, ("rom_patch_offset = %x\n", cap->rom_patch_offset));

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
										 USBD_TRANSFER_DIRECTION_OUT,
										 DEVICE_VENDOR_REQUEST_OUT,
										 0x42,
										 value,
										 0x230,
										 NULL,
										 0);


			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}
			
			value = (((cur_len + cap->rom_patch_offset) & 0xFFFF0000) >> 16);

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
										 USBD_TRANSFER_DIRECTION_OUT,
										 DEVICE_VENDOR_REQUEST_OUT,
										 0x42,
										 value,
										 0x232,
										 NULL,
										 0);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			cur_len += sent_len;

			while ((sent_len % 4) != 0)
				sent_len++;

			value = ((sent_len << 16) & 0xFFFF);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
										 USBD_TRANSFER_DIRECTION_OUT,
										 DEVICE_VENDOR_REQUEST_OUT,
										 0x42,
										 value,
										 0x234,
										 NULL,
										 0);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}
			
			value = (((sent_len << 16) & 0xFFFF0000) >> 16);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
										 USBD_TRANSFER_DIRECTION_OUT,
										 DEVICE_VENDOR_REQUEST_OUT,
										 0x42,
										 value,
										 0x236,
										 NULL,
										 0);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}
		
			/* Initialize URB descriptor */
			RTUSB_FILL_HTTX_BULK_URB(urb,
									 obj->pUsb_Dev,
									 cap->CommandBulkOutAddr,
									 rom_patch_data,
									 sent_len + sizeof(*tx_info) + 4,
									 UsbUploadRomPatchComplete,
									 &load_rom_patch_done,
									 rom_patch_dma);

			ret = RTUSB_SUBMIT_URB(urb);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("submit urb fail\n"));
				goto error2;
			}

			DBGPRINT(RT_DEBUG_INFO, ("%s: submit urb, sent_len = %d, patch_ilm = %d, cur_len = %d\n", __FUNCTION__, sent_len, patch_len, cur_len));
	
			if (!RTMP_OS_WAIT_FOR_COMPLETION_TIMEOUT(&load_rom_patch_done, RTMPMsecsToJiffies(1000)))
			{
				RTUSB_UNLINK_URB(urb);
				ret = NDIS_STATUS_FAILURE;
				DBGPRINT(RT_DEBUG_ERROR, ("upload fw timeout\n"));
				goto error2;
			}
			DBGPRINT(RT_DEBUG_OFF, ("."));

			RTMP_IO_READ32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, &mac_value);
			mac_value++;
			RTUSBWriteMACRegister(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, mac_value, FALSE);
	
			RtmpOsMsDelay(5);
		}
		else
		{
			break;
		}		

	}
	
	RTMP_OS_EXIT_COMPLETION(&load_rom_patch_done);

	total_checksum = Checksume16(cap->rom_patch + PATCH_INFO_SIZE, patch_len);
	 
	RtmpOsMsDelay(5);
	DBGPRINT(RT_DEBUG_OFF, ("Send checksum req..\n"));
	AndesRltUsbChkCrc(ad, patch_len);
	RtmpOsMsDelay(20);

	if (total_checksum != AndesRltUsbGetCrc(ad)) {
		DBGPRINT(RT_DEBUG_OFF, ("checksum fail!, local(0x%x) <> fw(0x%x)\n", total_checksum, AndesRltUsbGetCrc(ad)));

		ret = NDIS_STATUS_FAILURE;
		goto error2;
	}

	ret = AndesRltUsbEnablePatch(ad);

	if (ret) {
		ret = NDIS_STATUS_FAILURE;
		goto error2;
	}

	ret = AndesRltUsbResetWmt(ad);

	RtmpOsMsDelay(20);

	/* Check ROM_PATCH if ready */
	loop = 0;

	do {
		if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3)) {
			RTMP_IO_READ32(ad, CLOCK_CTL, &mac_value);
			if ((mac_value & 0x01) == 0x1)
				break;
		} else {
			RTMP_IO_READ32(ad, COM_REG0, &mac_value);
			if ((mac_value & 0x02) == 0x2)
				break;
		}

		RtmpOsMsDelay(10);
		loop++;
	} while (loop <= 100);

	if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3)) {
		DBGPRINT(RT_DEBUG_TRACE, ("%s: CLOCK_CTL(0x%x) = 0x%x\n", __FUNCTION__, CLOCK_CTL, mac_value));

		if ((mac_value & 0x01) != 0x1)
			ret = NDIS_STATUS_FAILURE;
	} else {
		DBGPRINT(RT_DEBUG_TRACE, ("%s: CLOCK_CTL(0x%x) = 0x%x\n", __FUNCTION__, COM_REG0, mac_value));

		if ((mac_value & 0x02) != 0x2)
			ret = NDIS_STATUS_FAILURE;
	}

error2:
	/* Free TransferBuffer */
	RTUSB_URB_FREE_BUFFER(obj->pUsb_Dev, UPLOAD_PATCH_UNIT, rom_patch_data, rom_patch_dma);

error1:
	/* Free URB */
	RTUSB_FREE_URB(urb);

error0: 
	if (cap->rom_code_protect)
		RTUSBWriteMACRegister(ad, SEMAPHORE_03, 0x1, FALSE);

	return ret;
}


INT32 AndesRltUsbEraseRomPatch(RTMP_ADAPTER *ad)
{
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	
	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));

	if (cap->load_code_method == BIN_FILE_METHOD) {
		if (cap->rom_patch)
			os_free_mem(NULL, cap->rom_patch);
			cap->rom_patch = NULL;
	}

	return 0;
}


VOID UsbUploadFwComplete(purbb_t urb, pregs *pt_regs)
{
	RTMP_OS_COMPLETION *load_fw_done = (RTMP_OS_COMPLETION *)RTMP_OS_USB_CONTEXT_GET(urb);

	RTMP_OS_COMPLETE(load_fw_done);
}


static NDIS_STATUS UsbLoadIvb(RTMP_ADAPTER *ad)
{
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;
	RTMP_CHIP_CAP *cap = &ad->chipCap;


	if (cap->load_iv) {
		Status = RTUSB_VendorRequest(ad,
									 USBD_TRANSFER_DIRECTION_OUT,
									 DEVICE_VENDOR_REQUEST_OUT,
									 0x01,
									 0x12,
									 0x00,
									 cap->FWImageName + 32,
									 64);
	} else {
		Status = RTUSB_VendorRequest(ad,
									 USBD_TRANSFER_DIRECTION_OUT,
									 DEVICE_VENDOR_REQUEST_OUT,
									 0x01,
									 0x12,
									 0x00,
									 NULL,
									 0x00);

	}

	if (Status)
	{
			DBGPRINT(RT_DEBUG_ERROR, ("Upload IVB Fail\n"));
			return Status;
	}

	return Status;
}


NDIS_STATUS AndesRltUsbLoadFw(RTMP_ADAPTER *ad)
{
	PURB urb;
	POS_COOKIE obj = (POS_COOKIE)ad->OS_Cookie;
	ra_dma_addr_t fw_dma;
	PUCHAR fw_data;
	TXINFO_NMAC_CMD *tx_info;	
	INT32 sent_len;
	UINT32 cur_len = 0;
	UINT32 mac_value, loop = 0;
	UINT16 value;
	int ret = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	USB_DMA_CFG_STRUC cfg;
	UINT32 ilm_len = 0, dlm_len = 0;
	UINT16 fw_ver, build_ver;
	RTMP_OS_COMPLETION load_fw_done;

	if (cap->ram_code_protect) {
loadfw_protect:
		RTMP_IO_READ32(ad, SEMAPHORE_00, &mac_value);
		loop++;

		if (((mac_value & 0x01) == 0x00) && (loop < GET_SEMAPHORE_RETRY_MAX)) {
			RtmpOsMsDelay(1);
			goto loadfw_protect;
		}

		if (loop >= GET_SEMAPHORE_RETRY_MAX) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s: can not get the hw semaphore\n", __FUNCTION__));
			return NDIS_STATUS_FAILURE;
		}
	}
	
	/* Check MCU if ready */
	RTMP_IO_READ32(ad, COM_REG0, &mac_value);

	if (((mac_value & 0x01) == 0x01) && (cap->ram_code_protect)) {
		goto error0;
	}
	
	RTUSBVenderReset(ad);
	RtmpOsMsDelay(5);
	
	/* Enable USB_DMA_CFG */
	USB_CFG_READ(ad, &cfg.word);
	cfg.word |= 0x00c00020;
	USB_CFG_WRITE(ad, cfg.word);

	if (cap->load_code_method == BIN_FILE_METHOD)
		OS_LOAD_CODE_FROM_BIN(&cap->FWImageName, cap->fw_bin_file_name, obj->pUsb_Dev, &cap->fw_len); 
	else
		cap->FWImageName = cap->fw_header_image;

	if (!cap->FWImageName) {
		if (cap->load_code_method == BIN_FILE_METHOD) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Please assign a fw image(/lib/firmware/%s), load_method(%d)\n", __FUNCTION__, cap->fw_bin_file_name, cap->load_code_method));
		} else {
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Please assign a fw image, load_method(%d)\n", 
				__FUNCTION__, cap->load_code_method));
		}
		ret = NDIS_STATUS_FAILURE;
		goto error0;
	}

	/* Get FW information */
	ilm_len = (*(cap->FWImageName + 3) << 24) | (*(cap->FWImageName + 2) << 16) |
			 (*(cap->FWImageName + 1) << 8) | (*cap->FWImageName);

	dlm_len = (*(cap->FWImageName + 7) << 24) | (*(cap->FWImageName + 6) << 16) |
			 (*(cap->FWImageName + 5) << 8) | (*(cap->FWImageName + 4));

	fw_ver = (*(cap->FWImageName + 11) << 8) | (*(cap->FWImageName + 10));

	build_ver = (*(cap->FWImageName + 9) << 8) | (*(cap->FWImageName + 8));
	
	DBGPRINT(RT_DEBUG_OFF, ("fw version:%d.%d.%02d ", (fw_ver & 0xf000) >> 8,
						(fw_ver & 0x0f00) >> 8, fw_ver & 0x00ff));
	DBGPRINT(RT_DEBUG_OFF, ("build:%x\n", build_ver));
	DBGPRINT(RT_DEBUG_OFF, ("build time:"));

	for (loop = 0; loop < 16; loop++)
		DBGPRINT(RT_DEBUG_OFF, ("%c", *(cap->FWImageName + 16 + loop)));

	DBGPRINT(RT_DEBUG_OFF, ("\n"));
	
	if (IS_MT76x2(ad)) {
		if (((strncmp(cap->FWImageName + 16, "20130811", 8) >= 0)) && (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3))) {
			DBGPRINT(RT_DEBUG_OFF, ("fw for E3 IC\n"));

		} else if (((strncmp(cap->FWImageName + 16, "20130811", 8) < 0)) && (MT_REV_LT(ad, MT76x2, REV_MT76x2E3))){

			DBGPRINT(RT_DEBUG_OFF, ("fw for E2 IC\n"));
		} else {
			DBGPRINT(RT_DEBUG_OFF, ("fw do not match IC version\n"));
			RTMP_IO_READ32(ad, 0x0, &mac_value);
			DBGPRINT(RT_DEBUG_OFF, ("IC version(%x)\n", mac_value));
			ret = NDIS_STATUS_FAILURE;
			goto error0;
		}
	}

	DBGPRINT(RT_DEBUG_OFF, ("ilm length = %d(bytes)\n", ilm_len));
	DBGPRINT(RT_DEBUG_OFF, ("dlm length = %d(bytes)\n", dlm_len));
	
	/* Enable FCE to send in-band cmd */
	RTUSBWriteMACRegister(ad, FCE_PSE_CTRL, 0x01, FALSE);

	/* FCE tx_fs_base_ptr */
	RTUSBWriteMACRegister(ad, TX_CPU_PORT_FROM_FCE_BASE_PTR, 0x400230, FALSE);

	/* FCE tx_fs_max_cnt */
	RTUSBWriteMACRegister(ad, TX_CPU_PORT_FROM_FCE_MAX_COUNT, 0x01, FALSE); 

	/* FCE pdma enable */
	RTUSBWriteMACRegister(ad, FCE_PDMA_GLOBAL_CONF, 0x44, FALSE);  

	/* FCE skip_fs_en */
	RTUSBWriteMACRegister(ad, FCE_SKIP_FS, 0x03, FALSE);

#if defined(MT76x0) || defined(MT76x2)
	if (IS_MT76x0(ad)) {
		USB_CFG_READ(ad, &cfg.word);

		cfg.field_76xx.UDMA_TX_WL_DROP = 1;

		USB_CFG_WRITE(ad, cfg.word);		

		cfg.field_76xx.UDMA_TX_WL_DROP = 0;

		USB_CFG_WRITE(ad, cfg.word);
	}
#endif /* defined(MT76x0) || defined(MT76x2) */
		
	/* Allocate URB */
	urb = RTUSB_ALLOC_URB(0);

	if (!urb)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate URB\n"));
		ret = NDIS_STATUS_RESOURCES; 
		goto error0;
	}

	/* Allocate TransferBuffer */
	fw_data = RTUSB_URB_ALLOC_BUFFER(obj->pUsb_Dev, UPLOAD_FW_UNIT, &fw_dma);
		
	if (!fw_data)
	{
		ret = NDIS_STATUS_RESOURCES;
		goto error1;
	}

	DBGPRINT(RT_DEBUG_OFF, ("loading fw"));
	
	RTMP_OS_INIT_COMPLETION(&load_fw_done);

	if (cap->load_iv)
		cur_len = 0x40;
	else
		cur_len = 0x00;

	/* Loading ILM */
	while (1)
	{
		INT32 sent_len_max = UPLOAD_FW_UNIT - sizeof(*tx_info) - USB_END_PADDING;	
		sent_len = (ilm_len - cur_len) >=  sent_len_max ? sent_len_max : (ilm_len - cur_len);

		if (sent_len > 0)
		{
			tx_info = (TXINFO_NMAC_CMD *)fw_data;
			tx_info->info_type = CMD_PACKET;
			tx_info->pkt_len = sent_len;
			tx_info->d_port = CPU_TX_PORT;

#ifdef RT_BIG_ENDIAN
			RTMPDescriptorEndianChange((PUCHAR)tx_info, TYPE_TXINFO);
#endif
			NdisMoveMemory(fw_data + sizeof(*tx_info), cap->FWImageName + FW_INFO_SIZE + cur_len, sent_len);

			/* four zero bytes for end padding */	
			NdisZeroMemory(fw_data + sizeof(*tx_info) + sent_len, USB_END_PADDING);

			value = (cur_len + cap->ilm_offset) & 0xFFFF;

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
										 USBD_TRANSFER_DIRECTION_OUT,
										 DEVICE_VENDOR_REQUEST_OUT,
										 0x42,
										 value,
										 0x230,
										 NULL,
										 0);


			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}
			
			value = (((cur_len + cap->ilm_offset) & 0xFFFF0000) >> 16);

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
										 USBD_TRANSFER_DIRECTION_OUT,
										 DEVICE_VENDOR_REQUEST_OUT,
										 0x42,
										 value,
										 0x232,
										 NULL,
										 0);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			

			cur_len += sent_len;

			while ((sent_len % 4) != 0)
				sent_len++;

			value = ((sent_len << 16) & 0xFFFF);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
										 USBD_TRANSFER_DIRECTION_OUT,
										 DEVICE_VENDOR_REQUEST_OUT,
										 0x42,
										 value,
										 0x234,
										 NULL,
										 0);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}
			
			value = (((sent_len << 16) & 0xFFFF0000) >> 16);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
										 USBD_TRANSFER_DIRECTION_OUT,
										 DEVICE_VENDOR_REQUEST_OUT,
										 0x42,
										 value,
										 0x236,
										 NULL,
										 0);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}
		
			/* Initialize URB descriptor */
			RTUSB_FILL_HTTX_BULK_URB(urb,
									 obj->pUsb_Dev,
									 cap->CommandBulkOutAddr,
									 fw_data,
									 sent_len + sizeof(*tx_info) + USB_END_PADDING,
									 UsbUploadFwComplete,
									 &load_fw_done,
									 fw_dma);

			ret = RTUSB_SUBMIT_URB(urb);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("submit urb fail\n"));
				goto error2;
			}

			DBGPRINT(RT_DEBUG_INFO, ("%s: submit urb, sent_len = %d, ilm_ilm = %d, cur_len = %d\n", __FUNCTION__, sent_len, ilm_len, cur_len));
	
			if (!RTMP_OS_WAIT_FOR_COMPLETION_TIMEOUT(&load_fw_done, RTMPMsecsToJiffies(UPLOAD_FW_TIMEOUT)))
			{
				RTUSB_UNLINK_URB(urb);
				ret = NDIS_STATUS_FAILURE;
				DBGPRINT(RT_DEBUG_ERROR, ("upload fw timeout(%dms)\n", UPLOAD_FW_TIMEOUT));
				DBGPRINT(RT_DEBUG_ERROR, ("%s: submit urb, sent_len = %d, ilm_ilm = %d, cur_len = %d\n", __FUNCTION__, sent_len, ilm_len, cur_len));
	
				goto error2;
			}
			DBGPRINT(RT_DEBUG_OFF, ("."));

			RTMP_IO_READ32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, &mac_value);
			mac_value++;
			RTUSBWriteMACRegister(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, mac_value, FALSE);
	
			RtmpOsMsDelay(5);
		}
		else
		{
			break;
		}		

	}
	
	RTMP_OS_EXIT_COMPLETION(&load_fw_done);
	
	/* Re-Initialize completion */
	RTMP_OS_INIT_COMPLETION(&load_fw_done);
	
	cur_len = 0x00;

	/* Loading DLM */
	while (1)
	{
		INT32 sent_len_max = UPLOAD_FW_UNIT - sizeof(*tx_info) - USB_END_PADDING;	
		sent_len = (dlm_len - cur_len) >= sent_len_max ? sent_len_max : (dlm_len - cur_len);

		if (sent_len > 0)
		{
			tx_info = (TXINFO_NMAC_CMD *)fw_data;
			tx_info->info_type = CMD_PACKET;
			tx_info->pkt_len = sent_len;
			tx_info->d_port = CPU_TX_PORT;

#ifdef RT_BIG_ENDIAN
			RTMPDescriptorEndianChange((PUCHAR)tx_info, TYPE_TXINFO);
#endif
			NdisMoveMemory(fw_data + sizeof(*tx_info), cap->FWImageName + FW_INFO_SIZE + ilm_len + cur_len, sent_len);
	
			NdisZeroMemory(fw_data + sizeof(*tx_info) + sent_len, USB_END_PADDING);

			if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3))
				value = ((cur_len + (cap->dlm_offset + 0x800)) & 0xFFFF);
			else
				value = ((cur_len + (cap->dlm_offset)) & 0xFFFF);

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
										 USBD_TRANSFER_DIRECTION_OUT,
										 DEVICE_VENDOR_REQUEST_OUT,
										 0x42,
										 value,
										 0x230,
										 NULL,
										 0);


			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}
			
			if (MT_REV_GTE(ad, MT76x2, REV_MT76x2E3))
				value = (((cur_len + (cap->dlm_offset + 0x800)) & 0xFFFF0000) >> 16);
			else
				value = (((cur_len + (cap->dlm_offset)) & 0xFFFF0000) >> 16);

			/* Set FCE DMA descriptor */
			ret = RTUSB_VendorRequest(ad,
									  USBD_TRANSFER_DIRECTION_OUT,
									  DEVICE_VENDOR_REQUEST_OUT,
									  0x42,
									  value,
									  0x232,
									  NULL,
									  0);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma descriptor fail\n"));
				goto error2;
			}

			

			cur_len += sent_len;

			while ((sent_len % 4) != 0)
				sent_len++;

			value = ((sent_len << 16) & 0xFFFF);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
									  USBD_TRANSFER_DIRECTION_OUT,
									  DEVICE_VENDOR_REQUEST_OUT,
									  0x42,
									  value,
									  0x234,
									  NULL,
									  0);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}
			
			value = (((sent_len << 16) & 0xFFFF0000) >> 16);

			/* Set FCE DMA length */
			ret = RTUSB_VendorRequest(ad,
								  	  USBD_TRANSFER_DIRECTION_OUT,
									  DEVICE_VENDOR_REQUEST_OUT,
									  0x42,
									  value,
									  0x236,
									  NULL,
									  0);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("set fce dma length fail\n"));
				goto error2;
			}
		
			/* Initialize URB descriptor */
			RTUSB_FILL_HTTX_BULK_URB(urb,
									 obj->pUsb_Dev,
									 cap->CommandBulkOutAddr,
									 fw_data,
									 sent_len + sizeof(*tx_info) + USB_END_PADDING,
									 UsbUploadFwComplete,
									 &load_fw_done,
									 fw_dma);

			ret = RTUSB_SUBMIT_URB(urb);

			if (ret)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("submit urb fail\n"));
				goto error2;
			}

			DBGPRINT(RT_DEBUG_INFO, ("%s: submit urb, sent_len = %d, dlm_len = %d, cur_len = %d\n", __FUNCTION__, sent_len, dlm_len, cur_len));
	
			if (!RTMP_OS_WAIT_FOR_COMPLETION_TIMEOUT(&load_fw_done, RTMPMsecsToJiffies(UPLOAD_FW_TIMEOUT)))
			{
				RTUSB_UNLINK_URB(urb);
				ret = NDIS_STATUS_FAILURE;
				DBGPRINT(RT_DEBUG_ERROR, ("upload fw timeout(%dms)\n", UPLOAD_FW_TIMEOUT));
				DBGPRINT(RT_DEBUG_INFO, ("%s: submit urb, sent_len = %d, dlm_len = %d, cur_len = %d\n", __FUNCTION__, sent_len, dlm_len, cur_len));
	
				goto error2;
			}
			DBGPRINT(RT_DEBUG_OFF, ("."));

			RTMP_IO_READ32(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, &mac_value);
			mac_value++;
			RTUSBWriteMACRegister(ad, TX_CPU_PORT_FROM_FCE_CPU_DESC_INDEX, mac_value, FALSE);
			RtmpOsMsDelay(5);
		}
		else
		{
			break;
		}		

	}

	RTMP_OS_EXIT_COMPLETION(&load_fw_done);

	/* Upload new 64 bytes interrupt vector or reset andes */
	DBGPRINT(RT_DEBUG_OFF, ("\n"));
	UsbLoadIvb(ad);

	/* Check MCU if ready */
	loop = 0;
	do
	{
		RTMP_IO_READ32(ad, COM_REG0, &mac_value);
		if ((mac_value & 0x01) == 0x01)
			break;
		RtmpOsMsDelay(10);
		loop++;
	} while (loop <= 100);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: COM_REG0(0x%x) = 0x%x\n", __FUNCTION__, COM_REG0, mac_value));

	RTMP_IO_READ32(ad, COM_REG0, &mac_value);
	mac_value |= (1 << 1);
	RTMP_IO_WRITE32(ad, COM_REG0, mac_value);

	if ((mac_value & 0x01) != 0x01)
		ret = NDIS_STATUS_FAILURE;
	
error2:
	/* Free TransferBuffer */
	RTUSB_URB_FREE_BUFFER(obj->pUsb_Dev, UPLOAD_FW_UNIT, fw_data, fw_dma);

error1:
	/* Free URB */
	RTUSB_FREE_URB(urb);

error0: 
	if (cap->ram_code_protect)
		RTUSBWriteMACRegister(ad, SEMAPHORE_00, 0x1, FALSE);

	/* Enable FCE to send in-band cmd */
	RTUSBWriteMACRegister(ad, FCE_PSE_CTRL, 0x01, FALSE);
	
	return ret;
}
#endif


INT32 AndesRltUsbEraseFw(RTMP_ADAPTER *ad)
{
	RTMP_CHIP_CAP *cap = &ad->chipCap;

	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));

	if (cap->load_code_method == BIN_FILE_METHOD) {
		if (cap->FWImageName)
			os_free_mem(NULL, cap->FWImageName);
			cap->FWImageName = NULL;
	}

	return 0;
}


VOID AndesRltRxProcessCmdMsg(RTMP_ADAPTER *ad, struct cmd_msg *rx_msg)
{
	PNDIS_PACKET net_pkt = rx_msg->net_pkt;
	struct cmd_msg *msg, *msg_tmp;
	RXFCE_INFO_CMD *rx_info = (RXFCE_INFO_CMD *)GET_OS_PKT_DATAPTR(net_pkt);
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	unsigned long flags;
#ifdef MT76XX_BTCOEX_SUPPORT
	PUCHAR pRxRspEvtPayload;
	PULONG pDW;
#endif /*MT76XX_BTCOEX_SUPPORT*/
#ifdef RT_BIG_ENDIAN
	RTMPDescriptorEndianChange((PUCHAR)rx_info, TYPE_RXINFO);
#endif

#if 1
	DBGPRINT(RT_DEBUG_INFO, ("(andex_rx_cmd)info_type=%d,evt_type=%d,d_port=%d,"
                                "qsel=%d,pcie_intr=%d,cmd_seq=%d,"
                                "self_gen=%d,pkt_len=%d\n",
                                rx_info->info_type, rx_info->evt_type,rx_info->d_port,
                                rx_info->qsel, rx_info->pcie_intr, rx_info->cmd_seq,
                                rx_info->self_gen, rx_info->pkt_len));
#endif

	if ((rx_info->info_type != CMD_PACKET)) {
		DBGPRINT(RT_DEBUG_ERROR, ("packet is not command response/self event\n"));
		return;
	} 
	
#ifdef MT76XX_BTCOEX_SUPPORT
	if (rx_info->evt_type == RSP_EVT_TYPE_COEX_RSP_EVENT)
	{
		
		pRxRspEvtPayload=GET_OS_PKT_DATAPTR(net_pkt) + sizeof(*rx_info);
		DBGPRINT(RT_DEBUG_ERROR, ("%s: RSP_EVT_TYPE_COEX_RSP_EVENT, RspEvtType = %d (len=%d)\n", 
					__FUNCTION__, 
					rx_info->evt_type, rx_info->pkt_len));

		pDW = (PULONG)(&pRxRspEvtPayload[0]);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x%04X\n", 
			__FUNCTION__, 
			*pDW));
		
		pDW = (PULONG)(&pRxRspEvtPayload[4]);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x%04X\n", 
			__FUNCTION__, 
			*pDW));
		
		pDW = (PULONG)(&pRxRspEvtPayload[8]);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x%04X\n", 
			__FUNCTION__, 
			*pDW));
#if 0
		pDW = (PULONG)(&pRxRspEvtPayload[12]);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x%04X\n", 
			__FUNCTION__, 
			*pDW));
		
		pDW = (PULONG)(&pRxRspEvtPayload[16]);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x%04X\n", 
			__FUNCTION__, 
			*pDW));
		
		pDW = (PULONG)(&pRxRspEvtPayload[20]);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x%04X\n", 
			__FUNCTION__, 
			*pDW));
		
		pDW = (PULONG)(&pRxRspEvtPayload[24]);
			DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x%04X\n", 
			__FUNCTION__, 
			*pDW));
			
		pDW = (PULONG)(&pRxRspEvtPayload[28]);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x%04X\n", 
			__FUNCTION__, 
			*pDW));
		
		pDW = (PULONG)(&pRxRspEvtPayload[32]);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: 0x%04X\n", 
			__FUNCTION__, 
			*pDW));
#endif
        pDW = (PULONG)(&pRxRspEvtPayload[0]);
			
        switch (*pDW)
        {

            case RSP_UpdateCOEXMode:
                DBGPRINT(RT_DEBUG_ERROR, ("Receive COEX OPERATION RSP_UpdateCOEXMode\n"));
                pDW = (PULONG)(&pRxRspEvtPayload[4]);
                ad->CoexMode.UpdateMode = (*pDW);
                DBGPRINT(RT_DEBUG_ERROR, ("Receive COEX update mode %d\n",*pDW));
                if (!ad->CoexMode.bForce)
                {
                    ad->CoexMode.CurrentMode =  ad->CoexMode.UpdateMode;
                }
            
            break;
            
            case RSP_BT_DEVICE_LINK_STATUS:
                DBGPRINT(RT_DEBUG_ERROR, ("Receive COEX OPERATION RSP_BT_DEVICE_LINK_STATUS\n"));
                pDW = (PULONG)(&pRxRspEvtPayload[4]);
                ad->BTStatusFlags = (*pDW);
                CoexParseBTStatus(ad);
                DBGPRINT(RT_DEBUG_ERROR, ("Receive COEX BT_DEVICE_LINK_STATUS = %X\n",*pDW));

            
            break;
            
            default:
                DBGPRINT(RT_DEBUG_ERROR, ("Receive COEX update packets (%d) BUT is a unkonw COEX Operation\n", *pDW));
                break;

          }	

		return;	
	}
#endif

	if (rx_info->self_gen) {
		/* if have callback function */
		RTEnqueueInternalCmd(ad, CMDTHREAD_RESPONSE_EVENT_CALLBACK, 
								GET_OS_PKT_DATAPTR(net_pkt) + sizeof(*rx_info), rx_info->pkt_len);
	} else {
#ifdef RTMP_USB_SUPPORT
		RTMP_SPIN_LOCK_IRQ(&ctl->ackq_lock);
#endif

#ifdef RTMP_PCI_SUPPORT
		RTMP_SPIN_LOCK_IRQSAVE(&ctl->ackq_lock, &flags);
#endif
		DlListForEachSafe(msg, msg_tmp, &ctl->ackq, struct cmd_msg, list) {
			if (msg->seq == rx_info->cmd_seq)
			{
				_AndesUnlinkCmdMsg(msg, &ctl->ackq);
#ifdef RTMP_USB_SUPPORT
				RTMP_SPIN_UNLOCK_IRQ(&ctl->ackq_lock);
#endif

#ifdef RTMP_PCI_SUPPORT
				//RTMP_SPIN_UNLOCK_IRQRESTORE(&ctl->ackq_lock, &flags);
#endif
				
				if ((msg->rsp_payload_len == rx_info->pkt_len) && (msg->rsp_payload_len != 0))
				{
					msg->rsp_handler(msg, GET_OS_PKT_DATAPTR(net_pkt) + sizeof(*rx_info), rx_info->pkt_len);
				}
				else if ((msg->rsp_payload_len == 0) && (rx_info->pkt_len == 8))
				{
					DBGPRINT(RT_DEBUG_INFO, ("command response(ack) success\n"));
				}
				else
				{
					DBGPRINT(RT_DEBUG_ERROR, ("expect response len(%d), command response len(%d) invalid\n", msg->rsp_payload_len, rx_info->pkt_len));
					msg->rsp_payload_len = rx_info->pkt_len;
				}

				if (msg->need_wait) {
#ifdef RTMP_PCI_SUPPORT
					msg->ack_done = TRUE;
#endif

#ifdef RTMP_USB_SUPPORT
					RTMP_OS_COMPLETE(&msg->ack_done);
#endif
				} else {
					AndesFreeCmdMsg(msg);
				}
#ifdef RTMP_USB_SUPPORT	
				RTMP_SPIN_LOCK_IRQ(&ctl->ackq_lock);
#endif

#ifdef RTMP_PCI_SUPPORT
				//RTMP_SPIN_LOCK_IRQSAVE(&ctl->ackq_lock, &flags);
#endif
				break;
			}
		}

#ifdef RTMP_USB_SUPPORT
		RTMP_SPIN_UNLOCK_IRQ(&ctl->ackq_lock);
#endif

#ifdef RTMP_PCI_SUPPORT
		RTMP_SPIN_UNLOCK_IRQRESTORE(&ctl->ackq_lock, &flags);
#endif
	}
}


#ifdef RTMP_PCI_SUPPORT
#ifdef RLT_MAC
VOID PciRxCmdMsgComplete(RTMP_ADAPTER *ad, RXFCE_INFO *fce_info)
{
	struct cmd_msg *msg;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		return;
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));

	msg = AndesAllocCmdMsg(ad, sizeof(*fce_info) + fce_info->pkt_len);
	
	if (!msg)
		return;

	AndesAppendCmdMsg(msg, (char *)fce_info, sizeof(*fce_info) + fce_info->pkt_len);
	
	AndesRxProcessCmdMsg(ad, msg);

	AndesFreeCmdMsg(msg);
}
#endif /* RLT_MAC */
#endif /* RTMP_PCI_SUPPORT */


#ifdef RLT_MAC
static UCHAR *txinfo_type_str[]={"PKT", "CMD", "RSV"};
static UCHAR *txinfo_d_port_str[]={"WLAN", "CPU_RX", "CPU_TX", "HOST", "VIRT_RX", "VIRT_TX", "DROP"};

VOID DumpCmdTxInfo(RTMP_ADAPTER *ad, TXINFO_STRUC *pTxInfo)
{
	DBGPRINT(RT_DEBUG_OFF, ("TxInfo:\n"));
	{
		struct _TXINFO_NMAC_CMD *cmd_txinfo = (struct _TXINFO_NMAC_CMD *)pTxInfo;
	
		hex_dump("Raw Data: ", (UCHAR *)pTxInfo, sizeof(TXINFO_STRUC));
		DBGPRINT(RT_DEBUG_OFF, ("\t Info_Type=%d(%s)\n", cmd_txinfo->info_type, txinfo_type_str[cmd_txinfo->info_type]));
		DBGPRINT(RT_DEBUG_OFF, ("\t d_port=%d(%s)\n", cmd_txinfo->d_port, txinfo_d_port_str[cmd_txinfo->d_port]));
		DBGPRINT(RT_DEBUG_OFF, ("\t cmd_type=%d\n", cmd_txinfo->cmd_type));
		DBGPRINT(RT_DEBUG_OFF, ("\t cmd_seq=%d\n", cmd_txinfo->cmd_seq));
		DBGPRINT(RT_DEBUG_OFF, ("\t pkt_len=0x%x\n", cmd_txinfo->pkt_len));
	}

	DBGPRINT(RT_DEBUG_OFF, ("\t"));
}


VOID AndesRltFillCmdHeader(struct cmd_msg *msg, PNDIS_PACKET net_pkt)
{
	TXINFO_NMAC_CMD *tx_info;

	tx_info = (TXINFO_NMAC_CMD *)OS_PKT_HEAD_BUF_EXTEND(net_pkt, sizeof(*tx_info));
	tx_info->info_type = CMD_PACKET;
	tx_info->d_port = msg->pq_id;
	tx_info->cmd_type = msg->cmd_type;
	tx_info->cmd_seq = msg->seq;
	tx_info->pkt_len = GET_OS_PKT_LEN(net_pkt) - sizeof(*tx_info);

#ifdef RT_BIG_ENDIAN
	RTMPDescriptorEndianChange((PUCHAR)tx_info, TYPE_TXINFO);
#endif

#if 0
		hex_dump("cmd_msg", (char *)(GET_OS_PKT_DATAPTR(net_pkt)), GET_OS_PKT_LEN(net_pkt));
#endif	
} 


#ifdef RTMP_PCI_SUPPORT
INT32 AndesRltPciKickOutCmdMsg(
	PRTMP_ADAPTER ad,
	struct cmd_msg *msg)
{
	int ret = NDIS_STATUS_SUCCESS;
	unsigned long flags = 0;
	ULONG FreeNum;
	PNDIS_PACKET net_pkt = msg->net_pkt;
	UINT32 SwIdx = 0, SrcBufPA;
	UCHAR *pSrcBufVA;
	UINT SrcBufLen = 0;
	PACKET_INFO PacketInfo;
	TXD_STRUC *pTxD;
	TXINFO_STRUC *pTxInfo;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
#ifdef RT_BIG_ENDIAN
	TXD_STRUC *pDestTxD;
#endif

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return -1;
	
	FreeNum = GET_CTRLRING_FREENO(ad);
	
	if (FreeNum == 0) {
		DBGPRINT(RT_DEBUG_WARN, ("%s FreeNum == 0 (TxCpuIdx = %d, TxDmaIdx = %d, TxSwFreeIdx = %d)\n",
		__FUNCTION__, ad->CtrlRing.TxCpuIdx, ad->CtrlRing.TxDmaIdx, ad->CtrlRing.TxSwFreeIdx));
		return NDIS_STATUS_FAILURE;
	}

	RTMP_SPIN_LOCK_IRQSAVE(&ad->CtrlRingLock, &flags);	

	RTMP_QueryPacketInfo(net_pkt, &PacketInfo, &pSrcBufVA, &SrcBufLen);
	
	if (pSrcBufVA == NULL) {
		RTMP_SPIN_UNLOCK_IRQRESTORE(&ad->CtrlRingLock, &flags);
		return NDIS_STATUS_FAILURE;
	}

	SwIdx = ad->CtrlRing.TxCpuIdx;
#ifdef RT_BIG_ENDIAN
	pDestTxD  = (PTXD_STRUC)ad->CtrlRing.Cell[SwIdx].AllocVa;
#else
	pTxD  = (PTXD_STRUC)ad->CtrlRing.Cell[SwIdx].AllocVa;
#endif

	pTxInfo = (TXINFO_STRUC *)((UCHAR *)pTxD + sizeof(TXD_STRUC));
	NdisMoveMemory(pTxInfo, pSrcBufVA, TXINFO_SIZE);

	ad->CtrlRing.Cell[SwIdx].pNdisPacket = net_pkt;
	ad->CtrlRing.Cell[SwIdx].pNextNdisPacket = NULL;

	SrcBufPA = PCI_MAP_SINGLE(ad, (pSrcBufVA) + 4, (SrcBufLen) - 4, 0, RTMP_PCI_DMA_TODEVICE);

	pTxD->LastSec0 = 1;
	pTxD->LastSec1 = 0;
	pTxD->SDLen0 = (SrcBufLen - TXINFO_SIZE);
	pTxD->SDLen1 = 0;
	pTxD->SDPtr0 = SrcBufPA;
	pTxD->DMADONE = 0;
	
#ifdef RT_BIG_ENDIAN
	RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
	WriteBackToDescriptor((PUCHAR)pDestTxD, (PUCHAR)pTxD, FALSE, TYPE_TXD);
#endif

	/* flush dcache if no consistent memory is supported */
	RTMP_DCACHE_FLUSH(SrcBufPA, SrcBufLen);
	RTMP_DCACHE_FLUSH(ad->CtrlRing.Cell[SwIdx].AllocPa, TXD_SIZE);

   	/* Increase TX_CTX_IDX, but write to register later.*/
	INC_RING_INDEX(ad->CtrlRing.TxCpuIdx, MGMT_RING_SIZE);

	if (msg->need_rsp)
		AndesQueueTailCmdMsg(&ctl->ackq, msg, wait_cmd_out_and_ack);
	else
		AndesQueueTailCmdMsg(&ctl->kickq, msg, wait_cmd_out);

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		RTMP_SPIN_UNLOCK_IRQRESTORE(&ad->CtrlRingLock, &flags);
		return -1;
	}
	
	RTMP_IO_WRITE32(ad, ad->CtrlRing.hw_cidx_addr, ad->CtrlRing.TxCpuIdx);

	RTMP_SPIN_UNLOCK_IRQRESTORE(&ad->CtrlRingLock, &flags);

	return ret;
}
#endif /* RTMP_PCI_SUPPORT */
#endif /* RLT_MAC */


#ifdef RTMP_USB_SUPPORT
INT32 AndesRltUsbKickOutCmdMsg(PRTMP_ADAPTER ad, struct cmd_msg *msg)
{
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	POS_COOKIE pObj = (POS_COOKIE)ad->OS_Cookie;
	int ret = 0;
	PNDIS_PACKET net_pkt = msg->net_pkt;
	RTMP_CHIP_CAP *pChipCap = &ad->chipCap;

	if (msg->state != tx_retransmit) {	
		/* append four zero bytes padding when usb aggregate enable */	
		memset(OS_PKT_TAIL_BUF_EXTEND(net_pkt, USB_END_PADDING), 0x00, USB_END_PADDING);
	}

	RTUSB_FILL_BULK_URB(msg->urb, pObj->pUsb_Dev,
						usb_sndbulkpipe(pObj->pUsb_Dev, pChipCap->CommandBulkOutAddr),	
						GET_OS_PKT_DATAPTR(net_pkt), GET_OS_PKT_LEN(net_pkt), UsbKickOutCmdMsgComplete, net_pkt);
	
	if (msg->need_rsp)
		AndesQueueTailCmdMsg(&ctl->ackq, msg, wait_cmd_out_and_ack);
	else
		AndesQueueTailCmdMsg(&ctl->kickq, msg, wait_cmd_out);
	
	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return -1;

	ret = RTUSB_SUBMIT_URB(msg->urb);
	
	if (ret) {
		if (!msg->need_rsp) {
			AndesUnlinkCmdMsg(msg, &ctl->kickq);
			AndesQueueTailCmdMsg(&ctl->tx_doneq, msg, tx_kickout_fail);
			AndesIncErrorCount(ctl, error_tx_kickout_fail);
		} else {
			AndesUnlinkCmdMsg(msg, &ctl->ackq);
			msg->state = tx_kickout_fail;
			AndesIncErrorCount(ctl, error_tx_kickout_fail);
			RTMP_OS_COMPLETE(&msg->ack_done);
		}
			
		DBGPRINT(RT_DEBUG_ERROR, ("%s:submit urb fail(%d)\n", __FUNCTION__, ret));
	}

	return ret;
}
#endif /* RTMP_USB_SUPPORT */


static VOID AndesRltPwrEventHandler(RTMP_ADAPTER *ad, char *payload, UINT16 payload_len)
{


}


static VOID AndesRltWowEventHandler(RTMP_ADAPTER *ad, char *payload, UINT16 payload_len)
{


}


static VOID AndesRltCarrierDetectEventHandler(RTMP_ADAPTER *ad, char *payload, UINT16 payload_len)
{



}


static VOID AndesRltDfsDetectEventHandler(PRTMP_ADAPTER ad, char *payload, UINT16 payload_len)
{



}


MSG_EVENT_HANDLER msg_event_handler_tb[] =
{
	AndesRltPwrEventHandler,
	AndesRltWowEventHandler,
	AndesRltCarrierDetectEventHandler,
	AndesRltDfsDetectEventHandler,
};


INT32 AndesRltBurstWrite(RTMP_ADAPTER *ad, UINT32 offset, UINT32 *data, UINT32 cnt)
{
	struct cmd_msg *msg;
	unsigned int var_len, offset_num, cur_len = 0, sent_len;
	UINT32 value, i, cur_index = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	int ret = 0;
	BOOLEAN last_packet = FALSE;

	if (!data)
		return -1;

	offset_num = cnt / ((cap->InbandPacketMaxLen - sizeof(offset)) / 4);

	if (cnt % ((cap->InbandPacketMaxLen - sizeof(offset)) / 4))
		var_len = sizeof(offset) * (offset_num + 1) + 4 * cnt;
	else
		var_len = sizeof(offset) * offset_num + 4 * cnt;

	while (cur_len < var_len) {
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen 
									? cap->InbandPacketMaxLen : (var_len - cur_len);

		if (((sent_len < cap->InbandPacketMaxLen) || ((cur_len + cap->InbandPacketMaxLen) == var_len)))
			last_packet = TRUE;

		msg = AndesAllocCmdMsg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}

		if (last_packet) {
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_BURST_WRITE, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);
		}else {
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_BURST_WRITE, CMD_NA, EXT_CMD_NA, FALSE, 0, FALSE, FALSE, 0, NULL, NULL);
		}
			
	
		value = cpu2le32(offset + cap->WlanMemmapOffset + cur_index * 4);
		AndesAppendCmdMsg(msg, (char *)&value, 4);

		for (i = 0; i < ((sent_len - 4) / 4); i++) {
			value = cpu2le32(data[i + cur_index]);
			AndesAppendCmdMsg(msg, (char *)&value, 4);
		}
		
		ret = AndesSendCmdMsg(ad, msg);

		
		cur_index += ((sent_len - 4) / 4);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}


static VOID AndesRltBurstReadCallback(struct cmd_msg *msg, char *rsp_payload, UINT16 rsp_payload_len)
{
	UINT32 i;
	UINT32 *data;
	NdisMoveMemory(msg->rsp_payload, rsp_payload + 4, rsp_payload_len - 4);

	for (i = 0; i < (msg->rsp_payload_len - 4) / 4; i++) {
		data = (UINT32 *)(msg->rsp_payload + i * 4);
		*data = le2cpu32(*data);
	}
}


INT32 AndesRltBurstRead(RTMP_ADAPTER *ad, UINT32 offset, UINT32 cnt, UINT32 *data)
{
	struct cmd_msg *msg;
	unsigned int cur_len = 0, rsp_len, offset_num, receive_len;
	UINT32 value, cur_index = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	int ret = 0;

	if (!data)
		return -1;

	offset_num = cnt / ((cap->InbandPacketMaxLen - sizeof(offset)) / 4);

	if (cnt % ((cap->InbandPacketMaxLen - sizeof(offset)) / 4))
		rsp_len = sizeof(offset) * (offset_num + 1) + 4 * cnt;
	else
		rsp_len = sizeof(offset) * offset_num + 4 * cnt;

	while (cur_len < rsp_len) {
		receive_len = (rsp_len - cur_len) > cap->InbandPacketMaxLen 
									   ? cap->InbandPacketMaxLen 
									   : (rsp_len - cur_len);

		msg = AndesAllocCmdMsg(ad, 8);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}
	
		AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_BURST_READ, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, receive_len, 
									(char *)(&data[cur_index]), AndesRltBurstReadCallback);

		value = cpu2le32(offset + cap->WlanMemmapOffset + cur_index * 4);
		AndesAppendCmdMsg(msg, (char *)&value, 4);

		value = cpu2le32((receive_len - 4) / 4);
		AndesAppendCmdMsg(msg, (char *)&value, 4);

		ret = AndesSendCmdMsg(ad, msg);

		if (ret) {
			if (cnt == 1)
				*data = 0xffffffff;
		}
		
		cur_index += ((receive_len - 4) / 4);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}


static VOID AndesRltRandomReadCallback(struct cmd_msg *msg, char *rsp_payload, 
											UINT16 rsp_payload_len)
{
	UINT32 i;
	RTMP_REG_PAIR *reg_pair = (RTMP_REG_PAIR *)msg->rsp_payload;
	
	for (i = 0; i < msg->rsp_payload_len / 8; i++) {
		NdisMoveMemory(&reg_pair[i].Value, rsp_payload + 8 * i + 4, 4);
		reg_pair[i].Value = le2cpu32(reg_pair[i].Value);
	}
}


INT32 AndesRltRandomRead(RTMP_ADAPTER *ad, RTMP_REG_PAIR *reg_pair, UINT32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, receive_len;
	UINT32 i, value, cur_index = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	int ret = 0;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len)
	{
		receive_len = (var_len - cur_len) > cap->InbandPacketMaxLen 
									   ? cap->InbandPacketMaxLen 
									   : (var_len - cur_len);

		msg = AndesAllocCmdMsg(ad, receive_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}
		
		AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_RANDOM_READ, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, receive_len, 
									(char *)&reg_pair[cur_index], AndesRltRandomReadCallback);

		for (i = 0; i < receive_len / 8; i++) {
			value = cpu2le32(reg_pair[i + cur_index].Register + cap->WlanMemmapOffset);
			AndesAppendCmdMsg(msg, (char *)&value, 4);
			value = 0;
			AndesAppendCmdMsg(msg, (char *)&value, 4);
		}
	

		ret = AndesSendCmdMsg(ad, msg);

		
		cur_index += receive_len / 8;
		cur_len += cap->InbandPacketMaxLen;
	}

error:	
	return ret;
}


static VOID AndesRltRfRandomReadCallback(struct cmd_msg *msg, char *rsp_payload, UINT16 rsp_payload_len)
{
	UINT32 i;
	BANK_RF_REG_PAIR *reg_pair = (BANK_RF_REG_PAIR *)msg->rsp_payload;
			
	for (i = 0; i < msg->rsp_payload_len / 8; i++) {
		NdisMoveMemory(&reg_pair[i].Value, rsp_payload + 8 * i + 4, 1);
	}
}


INT32 AndesRltRfRandomRead(RTMP_ADAPTER *ad, BANK_RF_REG_PAIR *reg_pair, UINT32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, receive_len;
	UINT32 i, value, cur_index = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	int ret = 0;
	
	if (!reg_pair)
		return -1;

	while (cur_len < var_len)
	{
		receive_len = (var_len - cur_len) > cap->InbandPacketMaxLen 
									   ? cap->InbandPacketMaxLen 
									   : (var_len - cur_len);

		msg = AndesAllocCmdMsg(ad, receive_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}
		
		AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_RANDOM_READ, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, receive_len, 
									(char *)&reg_pair[cur_index], AndesRltRfRandomReadCallback);

		for (i = 0; i < (receive_len) / 8; i++)
		{
			value = 0;
	
			/* RF selection */
			value = (value & ~0x80000000) | 0x80000000;

			/* RF bank */
			value = (value & ~0x00ff0000) | (reg_pair[i + cur_index].Bank << 16);

			/* RF Index */
			value = (value & ~0x0000ffff) | reg_pair[i + cur_index].Register;

			value = cpu2le32(value);
			AndesAppendCmdMsg(msg, (char *)&value, 4);
			value = 0;
			AndesAppendCmdMsg(msg, (char *)&value, 4);
		}

		ret = AndesSendCmdMsg(ad, msg);
	
	
		cur_index += receive_len / 8;
		cur_len += cap->InbandPacketMaxLen;
	}
	
error:
	return ret;
}


INT32 AndesRltReadModifyWrite(RTMP_ADAPTER *ad, R_M_W_REG *reg_pair, UINT32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 12, cur_len = 0, sent_len;
	UINT32 value, i, cur_index = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	int ret = 0;
	BOOLEAN last_packet = FALSE;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len)
	{
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen 
									? cap->InbandPacketMaxLen : (var_len - cur_len);
		
		if ((sent_len < cap->InbandPacketMaxLen) || (cur_len + cap->InbandPacketMaxLen) == var_len)
			last_packet = TRUE;
		
		msg = AndesAllocCmdMsg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}
		
		if (last_packet)
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_READ_MODIFY_WRITE, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);
		else
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_READ_MODIFY_WRITE, CMD_NA, EXT_CMD_NA, FALSE, 0, FALSE, FALSE, 0, NULL, NULL);

		for (i = 0; i < (sent_len / 12); i++)
		{
			/* Address */
			value = cpu2le32(reg_pair[i + cur_index].Register + cap->WlanMemmapOffset);
			AndesAppendCmdMsg(msg, (char *)&value, 4);

			/* ClearBitMask */
			value = cpu2le32(reg_pair[i + cur_index].ClearBitMask);
			AndesAppendCmdMsg(msg, (char *)&value, 4);

			/* UpdateData */
			value = cpu2le32(reg_pair[i + cur_index].Value);
			AndesAppendCmdMsg(msg, (char *)&value, 4);
		}

		ret = AndesSendCmdMsg(ad, msg);
	
	
		cur_index += (sent_len / 12);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}


INT32 AndesRltRfReadModifyWrite(RTMP_ADAPTER *ad, RF_R_M_W_REG *reg_pair, UINT32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 12, cur_len = 0, sent_len;
	UINT32 value, i, cur_index = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	int ret = 0;
	BOOLEAN last_packet = FALSE;
	
	if (!reg_pair)
		return -1;

	while (cur_len < var_len)
	{
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen 
									? cap->InbandPacketMaxLen : (var_len - cur_len);
		
		if ((sent_len < cap->InbandPacketMaxLen) || (cur_len + cap->InbandPacketMaxLen) == var_len)
			last_packet = TRUE;

		msg = AndesAllocCmdMsg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}
		
		if (last_packet)
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_READ_MODIFY_WRITE, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);
		else
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_READ_MODIFY_WRITE, CMD_NA, EXT_CMD_NA, FALSE, 0, FALSE, FALSE, 0, NULL, NULL);
		
		for (i = 0; i < sent_len / 12; i++)
		{
			value = 0;
			/* RF selection */
			value = (value & ~0x80000000) | 0x80000000;

			/* RF bank */
			value = (value & ~0x00ff0000) | (reg_pair[i + cur_index].Bank << 16);

			/* RF Index */
			value = (value & ~0x000000ff) | reg_pair[i + cur_index].Register;
			value = cpu2le32(value);
			AndesAppendCmdMsg(msg, (char *)&value, 4);
			
			value = 0;
			/* ClearBitMask */
			value = (value & ~0x000000ff) | reg_pair[i + cur_index].ClearBitMask;
			value = cpu2le32(value);
			AndesAppendCmdMsg(msg, (char *)&value, 4);

			value = 0;
			/* UpdateData */
			value = (value & ~0x000000ff) | reg_pair[i + cur_index].Value;
			value = cpu2le32(value);
			AndesAppendCmdMsg(msg, (char *)&value, 4);
		}
	
		ret = AndesSendCmdMsg(ad, msg);
	
		cur_index += (sent_len / 12);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}


INT32 AndesRltRandomWrite(RTMP_ADAPTER *ad, RTMP_REG_PAIR *reg_pair, UINT32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, sent_len;
	UINT32 value, i, cur_index = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	int ret = 0;
	BOOLEAN last_packet = FALSE;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len)
	{
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen 
									? cap->InbandPacketMaxLen : (var_len - cur_len);
	
		if ((sent_len < cap->InbandPacketMaxLen) || (cur_len + cap->InbandPacketMaxLen) == var_len)
			last_packet = TRUE;
	
		msg = AndesAllocCmdMsg(ad, sent_len);
		
		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}
		
		if (last_packet)
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_RANDOM_WRITE, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);
		else
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_RANDOM_WRITE, CMD_NA, EXT_CMD_NA, FALSE, 0, FALSE, FALSE, 0, NULL, NULL);

		for (i = 0; i < (sent_len / 8); i++)
		{
			/* Address */
			value = cpu2le32(reg_pair[i + cur_index].Register + cap->WlanMemmapOffset);
			AndesAppendCmdMsg(msg, (char *)&value, 4);

			/* UpdateData */
			value = cpu2le32(reg_pair[i + cur_index].Value);
			AndesAppendCmdMsg(msg, (char *)&value, 4);
		};

		ret = AndesSendCmdMsg(ad, msg);


		cur_index += (sent_len / 8);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}


INT32 AndesRltRfRandomWrite(RTMP_ADAPTER *ad, BANK_RF_REG_PAIR *reg_pair, UINT32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, sent_len;
	UINT32 value, i, cur_index = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	int ret = 0;
	BOOLEAN last_packet = FALSE;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len)
	{
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen 
									? cap->InbandPacketMaxLen : (var_len - cur_len);
	
		if ((sent_len < cap->InbandPacketMaxLen) || (cur_len + cap->InbandPacketMaxLen) == var_len)
			last_packet = TRUE;

		msg = AndesAllocCmdMsg(ad, sent_len);

		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}
		
		if (last_packet)
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_RANDOM_WRITE, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);
		else
			AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_RANDOM_WRITE, CMD_NA, EXT_CMD_NA, FALSE, 0, FALSE, FALSE, 0, NULL, NULL);
		
		for (i = 0; i < (sent_len / 8); i++) {
			value = 0;
			/* RF selection */
			value = (value & ~0x80000000) | 0x80000000;

			/* RF bank */
			value = (value & ~0x00ff0000) | (reg_pair[i + cur_index].Bank << 16);

			/* RF Index */
			value = (value & ~0x000000ff) | reg_pair[i + cur_index].Register;
			
			value = cpu2le32(value);
			AndesAppendCmdMsg(msg, (char *)&value, 4);

			value = 0;
			/* UpdateData */
			value = (value & ~0x000000ff) | reg_pair[i + cur_index].Value;
			value = cpu2le32(value);
			AndesAppendCmdMsg(msg, (char *)&value, 4);
		}

		ret = AndesSendCmdMsg(ad, msg);


		cur_index += (sent_len / 8);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}


#ifdef CONFIG_ANDES_BBP_RANDOM_WRITE_SUPPORT
INT32 AndesBbpRandomWrite(RTMP_ADAPTER *ad, RTMP_REG_PAIR *reg_pair, UINT32 num)
{
	struct cmd_msg *msg;
	unsigned int var_len = num * 8, cur_len = 0, sent_len;
	UINT32 value, i, cur_index = 0;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	int ret = 0;
	BOOLEAN last_packet = FALSE;

	if (!reg_pair)
		return -1;

	while (cur_len < var_len)
	{
		sent_len = (var_len - cur_len) > cap->InbandPacketMaxLen 
									? cap->InbandPacketMaxLen : (var_len - cur_len);
	
		if ((sent_len < cap->InbandPacketMaxLen) || (cur_len + cap->InbandPacketMaxLen) == var_len)
			last_packet = TRUE;
	
		msg = AndesAllocCmdMsg(ad, sent_len);
		
		if (!msg) {
			ret = NDIS_STATUS_RESOURCES;
			goto error;
		}
		
		if (last_packet)
			AndesInitCmdMsg(msg, CMD_RANDOM_WRITE, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);
		else
			AndesInitCmdMsg(msg, CMD_RANDOM_WRITE, FALSE, 0, FALSE, FALSE, 0, NULL, NULL);

		for (i = 0; i < (sent_len / 8); i++)
		{
			/* BBP selection */
			value = 0x40000000;
		
			/* Address */
			value |= reg_pair[i + cur_index].Register;
			value = cpu2le32(value);

			AndesAppendCmdMsg(msg, (char *)&value, 4);

			/* UpdateData */
			value = cpu2le32(reg_pair[i + cur_index].Value);
			AndesAppendCmdMsg(msg, (char *)&value, 4);
		};

		ret = AndesSendCmdmsg(ad, msg);


		cur_index += (sent_len / 8);
		cur_len += cap->InbandPacketMaxLen;
	}

error:
	return ret;
}
#endif /* CONFIG_ANDES_BBP_RANDOM_WRITE_SUPPORT */


INT32 AndesRltScRandomWrite(RTMP_ADAPTER *ad, CR_REG *table, UINT32 nums, UINT32 flags)
{
	UINT32 varlen = 0, i, j;
	RTMP_REG_PAIR *sw_ch_table = NULL, temp;

	if (!table)
		return -1;

	for (i = 0; i < nums; i++) {
		if ((table[i].flags & (_BAND | _BW | _TX_RX_SETTING)) == flags) {
			varlen += sizeof(RTMP_REG_PAIR);
		}
	}
	
	os_alloc_mem(NULL, (UCHAR **)&sw_ch_table, varlen);

	if (!sw_ch_table) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s: memory is not available for allocating switch channel table\n", __FUNCTION__));
		return -1;
	}

	for (i = 0, j = 0; i < nums; i++) {
		if ((table[i].flags & (_BAND | _BW | _TX_RX_SETTING)) == flags) {
			temp.Register = table[i].offset;
			temp.Value = table[i].value;
			NdisMoveMemory(&sw_ch_table[j], &temp, sizeof(temp));
			j++;
		}
	}

	AndesRltRandomWrite(ad, sw_ch_table, varlen / sizeof(RTMP_REG_PAIR));

	os_free_mem(NULL, sw_ch_table);

	return 0;
}


INT32 AndesRltScRfRandomWrite(RTMP_ADAPTER *ad, BANK_RF_CR_REG *table, UINT32 nums, UINT32 flags)
{
	UINT32 varlen = 0, i, j;
	BANK_RF_REG_PAIR *sw_ch_table = NULL, temp;

	if (!table)
		return -1;

	for (i = 0; i < nums; i++) {
		if ((table[i].flags & (_BAND | _BW | _TX_RX_SETTING)) == flags) {
			varlen += sizeof(BANK_RF_REG_PAIR);
		}
	}
	
	os_alloc_mem(NULL, (UCHAR **)&sw_ch_table, varlen);

	if (!sw_ch_table) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s: memory is not available for allocating switch channel table\n", __FUNCTION__));
		return -1;
	}

	for (i = 0, j = 0; i < nums; i++) {
		if ((table[i].flags & (_BAND | _BW | _TX_RX_SETTING)) == flags) {
			temp.Bank = table[i].bank;
			temp.Register = table[i].offset;
			temp.Value = table[i].value;
			NdisMoveMemory(&sw_ch_table[j], &temp, sizeof(temp));
			j++;
		}
	}

	AndesRltRfRandomWrite(ad, sw_ch_table, varlen / sizeof(BANK_RF_REG_PAIR));

	os_free_mem(NULL, sw_ch_table);

	return 0;
}


INT32 AndesRltPwrSaving(RTMP_ADAPTER *ad, UINT32 op, UINT32 level, 
						 UINT32 listen_interval, UINT32 pre_tbtt_lead_time,
						 UINT8 tim_byte_offset, UINT8 tim_byte_pattern)
{
	struct cmd_msg *msg;
	unsigned int var_len;
	UINT32 value;
	int ret = 0;

	/* Power operation and Power Level */
	var_len = 8;

	if (op == RADIO_OFF_ADVANCE)
	{
		/* Listen interval, Pre-TBTT, TIM info */
		var_len += 12;
	}
	
	msg = AndesAllocCmdMsg(ad, var_len);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_POWER_SAVING_OP, CMD_NA, EXT_CMD_NA, FALSE, 0, FALSE, FALSE, 0, NULL, NULL);
	
	/* Power operation */
	value = cpu2le32(op);
	AndesAppendCmdMsg(msg, (char *)&value, 4);

	/* Power Level */
	value = cpu2le32(level);

	AndesAppendCmdMsg(msg, (char *)&value, 4);

	if (op == RADIO_OFF_ADVANCE)
	{
		/* Listen interval */
		value = cpu2le32(listen_interval);
		AndesAppendCmdMsg(msg, (char *)&value, 4);


		/* Pre TBTT lead time */
		value = cpu2le32(pre_tbtt_lead_time);
		AndesAppendCmdMsg(msg, (char*)&value, 4);

		/* TIM Info */
		value = (value & ~0x000000ff) | tim_byte_pattern;
		value = (value & ~0x0000ff00) | (tim_byte_offset << 8);
		value = cpu2le32(value);
		AndesAppendCmdMsg(msg, (char *)&value, 4);
	}

	ret = AndesSendCmdMsg(ad, msg);
	
error:
	return ret;
}


INT32 AndesRltFunSet(RTMP_ADAPTER *ad, UINT32 fun_id, UINT32 param)
{
	struct cmd_msg *msg;
	UINT32 value;
	int ret = 0;

	/* Function ID and Parameter */
	msg = AndesAllocCmdMsg(ad, 8);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	if (fun_id != Q_SELECT)
		AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_FUN_SET_OP, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);
	else 
		AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_FUN_SET_OP, CMD_NA, EXT_CMD_NA, FALSE, 0, FALSE, FALSE, 0, NULL, NULL);
	
	/* Function ID */
	value = cpu2le32(fun_id);
	AndesAppendCmdMsg(msg, (char *)&value, 4);
	
	/* Parameter */
	value = cpu2le32(param);
	AndesAppendCmdMsg(msg, (char *)&value, 4);	
		
	ret = AndesSendCmdMsg(ad, msg);

error:
	return ret;
}


INT32 AndesRltCalibration(RTMP_ADAPTER *ad, UINT32 cal_id, ANDES_CALIBRATION_PARAM *param)
{
	struct cmd_msg *msg;
	UINT32 value;
	int ret = 0;

	DBGPRINT(RT_DEBUG_INFO, ("%s:cal_id(%d)\n ", __FUNCTION__, cal_id));

#ifdef MT76x2
#ifdef RTMP_PCI_SUPPORT
	if (IS_MT76x2(ad) && (cal_id != TSSI_COMPENSATION_7662)) {
		ANDES_CALIBRATION_START(ad);
	}
#endif /* RTMP_PCI_SUPPORT */
#endif /* MT76x2 */

#ifdef MT76x2
	/* Calibration ID and Parameter */
	if (cal_id == TSSI_COMPENSATION_7662 && IS_MT76x2(ad))		
		msg = AndesAllocCmdMsg(ad, 12);
	else
#endif /* MT76x2 */
		msg = AndesAllocCmdMsg(ad, 8);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_CALIBRATION_OP, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);
	
	/* Calibration ID */
	value = cpu2le32(cal_id);
	AndesAppendCmdMsg(msg, (char *)&value, 4);

	/* Parameter */
#ifdef MT76x2
	if (cal_id == TSSI_COMPENSATION_7662 && IS_MT76x2(ad)) {
		value = cpu2le32(param->mt76x2_tssi_comp_param.pa_mode);
		AndesAppendCmdMsg(msg, (char *)&value, 4);
		
		value = cpu2le32(param->mt76x2_tssi_comp_param.tssi_slope_offset);
		AndesAppendCmdMsg(msg, (char *)&value, 4);
	} else
#endif /* MT76x2 */
	{
		value = cpu2le32(param->generic);
		AndesAppendCmdMsg(msg, (char *)&value, 4);
	}

	ret = AndesSendCmdMsg(ad, msg);

#ifdef MT76x2
#ifdef RTMP_PCI_SUPPORT
	if (IS_MT76x2(ad) && ((cal_id != TSSI_COMPENSATION_7662))) {
		ANDES_WAIT_CALIBRATION_DONE(ad);
	}
#endif /* RTMP_PCI_SUPPORT */
#endif /* MT76x2 */

error:
	return ret;
}


INT32 AndesRltLoadCr(RTMP_ADAPTER *ad, UINT32 cr_type, UINT8 temp_level, UINT8 channel)
{
	struct cmd_msg *msg;
	UINT32 value = 0;
	int ret = 0;

	DBGPRINT(RT_DEBUG_OFF, ("%s:cr_type(%d), channel(%d)\n", __FUNCTION__, cr_type, temp_level, channel));
	
	msg = AndesAllocCmdMsg(ad, 8);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_LOAD_CR, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);

	/* CR type */
	value &= ~LOAD_CR_MODE_MASK;
	value |= LOAD_CR_MODE(cr_type);

	if (cr_type == HL_TEMP_CR_UPDATE) {
		value &= ~LOAD_CR_TEMP_LEVEL_MASK;
		value |= LOAD_CR_TEMP_LEVEL(temp_level); 

		value &= ~LOAD_CR_CHL_MASK;
		value |= LOAD_CR_CHL(channel); 
	} 

	value = cpu2le32(value);
	AndesAppendCmdMsg(msg, (char *)&value, 4);

	value = 0x80000000;
	value |= ((ad->EEPROMDefaultValue[EEPROM_NIC_CFG1_OFFSET] >> 8) & 0xFF);
	value |= ((ad->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET] & 0xFF) << 8 );
	value = cpu2le32(value);
	AndesAppendCmdMsg(msg, (char *)&value, 4);

	ret = AndesSendCmdMsg(ad, msg);

error:
	return ret;
}


INT32 AndesRltSwitchChannel(RTMP_ADAPTER *ad, UINT8 channel, BOOLEAN scan, unsigned int bw, unsigned int tx_rx_setting, UINT8 bbp_ch_idx)
{
	struct cmd_msg *msg;
	UINT32 value = 0;
	int ret;

	DBGPRINT(RT_DEBUG_INFO, ("%s:channel(%d),scan(%d),bw(%d),trx(0x%x)\n", __FUNCTION__, channel, scan, bw, tx_rx_setting));

	msg = AndesAllocCmdMsg(ad, 8);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_SWITCH_CHANNEL_OP, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);

	/* 
     * switch channel related param
     * channel, scan, bw, tx_rx_setting
     */
	value &= ~SC_PARAM1_CHL_MASK;
	value |= SC_PARAM1_CHL(channel);
	value &= ~SC_PARAM1_SCAN_MASK;
	value |= SC_PARAM1_SCAN(scan);
	value &= ~SC_PARAM1_BW_MASK;
	value |= SC_PARAM1_BW(bw);
	value = cpu2le32(value);
	AndesAppendCmdMsg(msg, (char *)&value, 4);
	
	value = 0;
	value |= SC_PARAM2_TR_SETTING(tx_rx_setting);
	value = cpu2le32(value);
	AndesAppendCmdMsg(msg, (char *)&value, 4);

	ret = AndesSendCmdMsg(ad, msg);
	
	mdelay(5);

	msg = AndesAllocCmdMsg(ad, 8);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		goto error;
	}

	AndesInitCmdMsg(msg, CPU_TX_PORT, CMD_SWITCH_CHANNEL_OP, CMD_NA, EXT_CMD_NA, TRUE, 0, TRUE, TRUE, 0, NULL, NULL);

	/* 
     * switch channel related param
     * channel, scan, bw, tx_rx_setting, extension channel
     */
	value &= ~SC_PARAM1_CHL_MASK;
	value |= SC_PARAM1_CHL(channel);
	value &= ~SC_PARAM1_SCAN_MASK;
	value |= SC_PARAM1_SCAN(scan);
	value &= ~SC_PARAM1_BW_MASK;
	value |= SC_PARAM1_BW(bw);
	value = cpu2le32(value);
	AndesAppendCmdMsg(msg, (char *)&value, 4);
	
	value = 0;
	value |= SC_PARAM2_TR_SETTING(tx_rx_setting);
	value &= ~SC_PARAM2_EXTENSION_CHL_MASK;
	
	if (bbp_ch_idx == 0)
		value |= SC_PARAM2_EXTENSION_CHL(0xe0);
	else if (bbp_ch_idx == 1)
		value |= SC_PARAM2_EXTENSION_CHL(0xe1);
	else if (bbp_ch_idx == 2)
		value |= SC_PARAM2_EXTENSION_CHL(0xe2);
	else if (bbp_ch_idx == 3)
		value |= SC_PARAM2_EXTENSION_CHL(0xe3);
		
	value = cpu2le32(value);
	AndesAppendCmdMsg(msg, (char *)&value, 4);
	
	ret = AndesSendCmdMsg(ad, msg);

error:
	return ret;
}


#ifdef RTMP_PCI_SUPPORT
VOID AndesRltPciFwInit(RTMP_ADAPTER *ad)
{
	UINT32 value;

	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));
#ifdef RLT_MAC
	/* Enable Interrupt*/
	RTMP_IRQ_ENABLE(ad);
	RTMPEnableRxTx(ad);
	RTMP_SET_FLAG(ad, fRTMP_ADAPTER_START_UP);
	RTMP_SET_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	/* clear garbage interrupts*/
	RTMP_IO_READ32(ad, 0x1300, &value);
	DBGPRINT(RT_DEBUG_OFF, ("0x1300 = %08x\n", value));

#ifdef HDR_TRANS_SUPPORT
	RTMP_IO_WRITE32(ad, HEADER_TRANS_CTRL_REG, 0X2);
	RTMP_IO_WRITE32(ad, TSO_CTRL, 0x7050);
#else
	RTMP_IO_WRITE32(ad, HEADER_TRANS_CTRL_REG, 0x0);
	RTMP_IO_WRITE32(ad, TSO_CTRL, 0x0);
#endif

	AndesRltFunSet(ad, Q_SELECT, ad->chipCap.CmdRspRxRing);
	PWR_SAVING_OP(ad, RADIO_ON, 0, 0, 0, 0, 0);
#endif /* RLT_MAC */
}
#endif /* RTMP_PCI_SUPPORT */


#ifdef RTMP_USB_SUPPORT
VOID AndesRltUsbFwInit(RTMP_ADAPTER *ad)
{
	DBGPRINT(RT_DEBUG_OFF, ("%s\n", __FUNCTION__));

#ifdef HDR_TRANS_SUPPORT
	RTMP_IO_WRITE32(ad, HEADER_TRANS_CTRL_REG, 0X2);
	RTMP_IO_WRITE32(ad, TSO_CTRL, 0x7050);
#else
	RTMP_IO_WRITE32(ad, HEADER_TRANS_CTRL_REG, 0x0);
	RTMP_IO_WRITE32(ad, TSO_CTRL, 0x0);
#endif

	RT28XXDMAEnable(ad); 	
	RTMP_SET_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	AndesRltFunSet(ad, Q_SELECT, ad->chipCap.CmdRspRxRing);
	UsbRxCmdMsgsReceive(ad);
	PWR_SAVING_OP(ad, RADIO_ON, 0, 0, 0, 0, 0);
}
#endif /* RTMP_USB_SUPPORT */

