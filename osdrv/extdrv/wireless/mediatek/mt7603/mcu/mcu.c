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
	rtmp_mcu.c

	Abstract:

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/


#include	"rt_config.h"

INT MCUBurstWrite(PRTMP_ADAPTER pAd, UINT32 Offset, UINT32 *Data, UINT32 Cnt)
{
#ifdef RTMP_USB_SUPPORT
	RTUSBMultiWrite_nBytes(pAd, (USHORT)Offset, (UCHAR *)Data, (USHORT)(Cnt * 4), 64);
#endif /* RTMP_USB_SUPPORT */
	return 0;
}


INT MCURandomWrite(PRTMP_ADAPTER pAd, RTMP_REG_PAIR *RegPair, UINT32 Num)
{
	UINT32 Index;
	
	for (Index = 0; Index < Num; Index++)
		RTMP_IO_WRITE32(pAd, RegPair->Register, RegPair->Value);

	return 0;
}


INT32 MCUSysInit(RTMP_ADAPTER *pAd)
{
	INT32 Ret = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));

	MCU_CTRL_INIT(pAd);
	
#ifdef MT_MAC
	if (pAd->chipOps.FwInit && (pAd->chipCap.hif_type == HIF_MT))
		pAd->chipOps.FwInit(pAd);
	
#ifndef RTMP_SDIO_SUPPORT	
#ifdef MT7636
    if (IS_MT7636(pAd)) {	
		if (load_patch(pAd) != NDIS_STATUS_SUCCESS) {
			DBGPRINT_ERR(("load patch failed!\n"));
			return FALSE;
		}	
	}
#endif /* MT7636 */	
#endif /*leonardo temporari skip rom patch download for FPGA*/	

#endif /* MT_MAC */

	Ret = NICLoadFirmware(pAd);
	
	if (Ret != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("%s: NICLoadFirmware failed, Status[=0x%08x]\n", __FUNCTION__, Ret));
		return -1;
	}

#ifdef MT_MAC
	if (pAd->chipCap.hif_type == HIF_MT) {
#if defined(RTMP_PCI_SUPPORT) || defined(RTMP_USB_SUPPORT)
		AsicDMASchedulerInit(pAd, DMA_SCH_LMAC);
#endif
	}
#endif /* MT_MAC */

	return Ret;
}


INT32 MCUSysExit(RTMP_ADAPTER *pAd)
{
	INT32 Ret = 0;

	if (pAd->chipOps.FwExit)
		pAd->chipOps.FwExit(pAd);

	MCU_CTRL_EXIT(pAd);

	return Ret;
}


VOID ChipOpsMCUHook(PRTMP_ADAPTER pAd, enum MCU_TYPE MCUType)
{
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;

#ifdef CONFIG_SWMCU_SUPPORT
	if (MCUType == SWMCU) 
	{
		pChipOps->eraseFirmware = NULL;
		pChipOps->loadFirmware = NULL;
		pChipOps->sendCommandToMcu = RtmpAsicSendCommandToSwMcu;

		return;
	}
#endif

#ifdef CONFIG_M8051_SUPPORT
	if (MCUType == M8051)
	{
#ifdef RTMP_PCI_SUPPORT
		pChipOps->fw_init = m51_pci_fw_init;
#endif

#ifdef RTMP_USB_SUPPORT
		pChipOps->fw_init = m51_usb_fw_init;
#endif

		pChipOps->eraseFirmware = NULL;
		pChipOps->loadFirmware = RtmpAsicLoadFirmware;
		pChipOps->sendCommandToMcu = RtmpAsicSendCommandToMcu;

		return;
	}
#endif

#ifdef CONFIG_ANDES_SUPPORT
	if (MCUType == ANDES) {
		RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;

#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {	

#ifdef RTMP_PCI_SUPPORT
			if (pChipCap->need_load_rom_patch) {
				pChipOps->load_rom_patch = AndesRltPciLoadRomPatch;
				pChipOps->erase_rom_patch = AndesRltPciEraseRomPatch;
			}

			if (pChipCap->need_load_fw) {
				pChipOps->loadFirmware = AndesRltPciLoadfw;
				pChipOps->eraseFirmware = AndesRltPciErasefw;
				pChipOps->pci_kick_out_cmd_msg = AndesRltPciKickOutCmdMsg;
			}

			pChipOps->FwInit = AndesRltPciFwInit;
#endif /* RTMP_PCI_SUPPORT */

#ifdef RTMP_USB_SUPPORT
			if (pChipCap->need_load_rom_patch) {
				pChipOps->load_rom_patch = AndesRltUsbLoadRomPatch;
				pChipOps->erase_rom_patch = AndesRltUsbEraseRomPatch;
			}
		
			if (pChipCap->need_load_fw) {
				pChipOps->loadFirmware = AndesRltUsbLoadFw;
				pChipOps->eraseFirmware = AndesRltUsbEraseFw;
				pChipOps->usb_kick_out_cmd_msg = AndesRltUsbKickOutCmdMsg;
			}	

			pChipOps->FwInit = AndesRltUsbFwInit;
#endif /* RTMP_USB_SUPPORT */

			if (pChipCap->need_load_fw) {
				//pChipOps->sendCommandToMcu = andes_send_cmd_msg;
				pChipOps->Calibration = AndesRltCalibration;
				pChipOps->BurstWrite =  AndesRltBurstWrite;
				pChipOps->BurstRead = AndesRltBurstRead;
				pChipOps->RandomRead = AndesRltRandomRead;
				pChipOps->RFRandomRead = AndesRltRfRandomRead;
				pChipOps->ReadModifyWrite = AndesRltReadModifyWrite;
				pChipOps->RFReadModifyWrite = AndesRltRfReadModifyWrite;
				pChipOps->RandomWrite = AndesRltRandomWrite;
				pChipOps->RFRandomWrite = AndesRltRfRandomWrite;
#ifdef CONFIG_ANDES_BBP_RANDOM_WRITE_SUPPORT
				pChipOps->BBPRandomWrite = AndesBbpRandomWrite;
#endif /* CONFIG_ANDES_BBP_RANDOM_WRITE_SUPPORT */
				pChipOps->sc_random_write = AndesRltScRandomWrite;
				pChipOps->sc_rf_random_write = AndesRltScRfRandomWrite;
				pChipOps->PwrSavingOP = AndesRltPwrSaving;
				pChipOps->andes_fill_cmd_header = AndesRltFillCmdHeader;
			}
		}
#endif /* RLT_MAC */
#ifdef MT_MAC
		if (pAd->chipCap.hif_type == HIF_MT) {
#ifdef RTMP_USB_SUPPORT
			pChipOps->FwInit = AndesMTUsbFwInit;

			if (pChipCap->need_load_fw) {
				pChipOps->usb_kick_out_cmd_msg = AndesMTUSBKickOutCmdMsg;
			}

			if (pChipCap->need_load_rom_patch)
			{
				pChipOps->AndesMTChkCrc = AndesMTUsbChkCrc;
				pChipOps->AndesMTGetCrc = AndesMTUsbGetCrc;
			}
#endif /* RTMP_USB_SUPPORT */

#ifdef RTMP_PCI_SUPPORT
			pChipOps->FwInit = AndesMTPciFwInit;
			pChipOps->FwExit = AndesMTPciFwExit;

			if (pChipCap->need_load_fw) {
				pChipOps->pci_kick_out_cmd_msg = AndesMTPciKickOutCmdMsg;
			}
#endif /* RTMP_PCI_SUPPORT */

#ifdef RTMP_SDIO_SUPPORT
			pChipOps->FwInit = AndesMTSdioFwInit;
			pChipOps->FwExit = AndesMTSdioFwExit;

			if (pChipCap->need_load_fw) {
				pChipOps->sdio_kick_out_cmd_msg = AndesMTSdioKickOutCmdMsg;
			}

			if (pChipCap->need_load_rom_patch)
			{
				pChipOps->AndesMTChkCrc = AndesMTSdioChkCrc;
				pChipOps->AndesMTGetCrc = AndesMTSdioGetCrc;
			}
#endif

			if (pChipCap->need_load_fw) {
				pChipOps->loadFirmware = AndesMTLoadFw;
				pChipOps->eraseFirmware = AndesMTEraseFw;
				pChipOps->andes_fill_cmd_header = AndesMTFillCmdHeader;
				pChipOps->rx_event_handler = AndesMTRxEventHandler;
			}

			if (pChipCap->need_load_rom_patch) {
				pChipOps->load_rom_patch = AndesMTLoadRomPatch;
				//pChipOps->erase_rom_patch = AndesMTEraseRomPatch;
			}
		}
#endif /* MT_MAC */
				
		pChipOps->MCUCtrlInit = AndesCtrlInit;
		pChipOps->MCUCtrlExit = AndesCtrlExit;
	}
#endif /* CONFIG_ANDES_SUPPORT */
}

