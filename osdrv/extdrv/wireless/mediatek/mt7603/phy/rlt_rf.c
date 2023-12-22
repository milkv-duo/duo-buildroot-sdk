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

        Abstract:

        Revision History:
        Who             When                    What
        --------        ----------              ----------------------------------------------
*/


#ifdef RLT_RF

#include "rt_config.h"


static inline BOOLEAN rf_csr_poll_idle(RTMP_ADAPTER *pAd, UINT32 *rfcsr)
{
	RF_CSR_CFG_STRUC *csr_val;
	BOOLEAN idle = BUSY;
	INT i = 0;

	do
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			break;

		RTMP_IO_READ32(pAd, RF_CSR_CFG, rfcsr);

		csr_val = (RF_CSR_CFG_STRUC *)rfcsr;
#ifdef RT6352
		if (IS_RT6352(pAd))
			idle = csr_val->bank_6352.RF_CSR_KICK;
		else
#endif /* RT6352 */
#ifdef RT65xx
		if (IS_RT65XX(pAd))
			idle = csr_val->bank_65xx.RF_CSR_KICK;
		else
#endif /* RT65xx */
#ifdef MT7601
		if (IS_MT7601(pAd))
			idle = csr_val->bank_7601.RF_CSR_KICK;
		else
#endif /* MT7601 */
			idle = csr_val->non_bank.RF_CSR_KICK;

		if (idle == IDLE)
			break;

		i++;
	} while (i < MAX_BUSY_COUNT);

	if ((i == MAX_BUSY_COUNT) || (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		DBGPRINT_RAW(RT_DEBUG_ERROR, ("Retry count exhausted or device removed(cnt=%d)!\n", i));
	}

	return idle;
}


NDIS_STATUS rlt_rf_write(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR bank,
	IN UCHAR regID,
	IN UCHAR value)
{
	RF_CSR_CFG_STRUC rfcsr = { { 0 } };
	NDIS_STATUS	 ret;

#ifdef MT76x0
	if (IS_MT7610U(pAd)) {
		BANK_RF_REG_PAIR reg;

		reg.Bank = bank;
		reg.Register = regID;
		reg.Value = value;

		RF_RANDOM_WRITE(pAd, &reg, 1);
		return NDIS_STATUS_SUCCESS;
	}
#endif /* MT76x0 */

#ifdef RTMP_MAC_PCI
	if ((pAd->bPCIclkOff == TRUE) || (pAd->LastMCUCmd == SLEEP_MCU_CMD))
	{
		DBGPRINT_ERR(("rlt_rf_write. Not allow to write RF 0x%x : fail\n",  regID));	
		return STATUS_UNSUCCESSFUL;
	}
#endif /* RTMP_MAC_PCI */

#ifdef RLT_MAC
	// TODO: shiang-usw, why we need to check this for MT7601?? Get these code from MT7601!
	if (pAd->chipCap.hif_type == HIF_RLT) {
		if (pAd->WlanFunCtrl.field.WLAN_EN == 0)
		{
			DBGPRINT_ERR(("rlt_rf_write. Not allow to write RF 0x%x : fail\n",  regID));	
			return STATUS_UNSUCCESSFUL;
		}
	}
#endif /* RLT_MAC */

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		RTMP_SEM_EVENT_WAIT(&pAd->reg_atomic, ret);
		if (ret != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", ret));
			return STATUS_UNSUCCESSFUL;
		}
	}
#endif /* RTMP_MAC_USB */

	ASSERT((regID <= pAd->chipCap.MaxNumOfRfId));

	ret = STATUS_UNSUCCESSFUL;
	if (rf_csr_poll_idle(pAd, &rfcsr.word) != IDLE)
		goto done;

#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		rfcsr.bank_6352.RF_CSR_WR = 1;
		rfcsr.bank_6352.RF_CSR_KICK = 1;
		rfcsr.bank_6352.TESTCSR_RFACC_REGNUM = (regID | (bank << 6));
		rfcsr.bank_6352.RF_CSR_DATA = value;
	}
	else
#endif /* RT6352 */
#ifdef RT65xx
	if (IS_RT65XX(pAd)) {
		rfcsr.bank_65xx.RF_CSR_WR = 1;
		rfcsr.bank_65xx.RF_CSR_KICK = 1;
		rfcsr.bank_65xx.RF_CSR_REG_BANK = bank;
		rfcsr.bank_65xx.RF_CSR_REG_ID = regID;
		rfcsr.bank_65xx.RF_CSR_DATA = value;
	}
	else
#endif /* RT65xx */
#ifdef MT7601
	if (IS_MT7601(pAd)) {
		rfcsr.bank_7601.RF_CSR_WR = 1;
		rfcsr.bank_7601.RF_CSR_KICK = 1;
		rfcsr.bank_7601.RF_CSR_REG_BANK = bank;
		rfcsr.bank_7601.RF_CSR_REG_ID = regID;
		rfcsr.bank_7601.RF_CSR_DATA = value;
	}
	else
#endif /* MT7601 */
	{
		DBGPRINT_ERR(("%s():RF write with wrong handler!\n", __FUNCTION__));
		goto done;
	}
	RTMP_IO_WRITE32(pAd, RF_CSR_CFG, rfcsr.word);

	ret = NDIS_STATUS_SUCCESS;

done:
#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		RTMP_SEM_EVENT_UP(&pAd->reg_atomic);
	}
#endif /* RTMP_MAC_USB */

	return ret;
}


/*
	========================================================================
	
	Routine Description: Read RF register through MAC

	Arguments:

	Return Value:

	IRQL = 
	
	Note:
	
	========================================================================
*/
NDIS_STATUS rlt_rf_read(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR bank,
	IN UCHAR regID,
	IN UCHAR *pValue)
{
	RF_CSR_CFG_STRUC rfcsr = { { 0 } };
	UINT i=0, k=0;
	BOOLEAN rf_status;
	NDIS_STATUS	 ret = STATUS_UNSUCCESSFUL;

#ifdef MT76x0
	if (IS_MT7610U(pAd)) {
		BANK_RF_REG_PAIR reg;
		reg.Bank = bank;
		reg.Register = regID;
		RF_RANDOM_READ(pAd, &reg, 1);
	
		*pValue = reg.Value;
		return NDIS_STATUS_SUCCESS;
	}
#endif /* MT76x0 */

#ifdef RTMP_MAC_PCI
	if ((pAd->bPCIclkOff == TRUE) || (pAd->LastMCUCmd == SLEEP_MCU_CMD))
	{
		DBGPRINT_ERR(("%s():Not allow to read RF 0x%x : fail\n",  __FUNCTION__, regID));
		return STATUS_UNSUCCESSFUL;
	}
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		RTMP_SEM_EVENT_WAIT(&pAd->reg_atomic, i);
		if (i != 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("reg_atomic get failed(ret=%d)\n", i));
			return STATUS_UNSUCCESSFUL;
		}
	}
#endif /* RTMP_MAC_USB */

	ASSERT((regID <= pAd->chipCap.MaxNumOfRfId));

	rfcsr.word = 0;
	
	for (i=0; i<MAX_BUSY_COUNT; i++)
	{
		if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			goto done;

		rf_status = rf_csr_poll_idle(pAd, &rfcsr.word);
		if ( rf_status == BUSY)
			break;

		rfcsr.word = 0;
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
			rfcsr.bank_6352.RF_CSR_WR = 0;
			rfcsr.bank_6352.RF_CSR_KICK = 1;
			rfcsr.bank_6352.TESTCSR_RFACC_REGNUM = (regID | (bank << 6));
		}
		else
#endif /* RT6352 */
#ifdef RT65xx
		if (IS_RT65XX(pAd)) {
			rfcsr.bank_65xx.RF_CSR_WR = 0;
			rfcsr.bank_65xx.RF_CSR_KICK = 1;
			rfcsr.bank_65xx.RF_CSR_REG_ID = regID;
			rfcsr.bank_65xx.RF_CSR_REG_BANK = bank;
		}
		else
#endif /* RT65xx */
#ifdef MT7601
		if (IS_MT7601(pAd)) {
			rfcsr.bank_7601.RF_CSR_WR = 0;
			rfcsr.bank_7601.RF_CSR_KICK = 1;
			rfcsr.bank_7601.RF_CSR_REG_ID = regID;
			rfcsr.bank_7601.RF_CSR_REG_BANK = bank;
		}
		else
#endif /* MT7601 */
		{
			DBGPRINT_ERR(("RF[%d] read function for non-supported chip[0x%x]\n", regID, pAd->MACVersion));
			break;
		}
		
		RTMP_IO_WRITE32(pAd, RF_CSR_CFG, rfcsr.word);

		rf_status = rf_csr_poll_idle(pAd, &rfcsr.word);
		if (rf_status == IDLE)
		{
#ifdef RT6352
			if (IS_RT6352(pAd) && ((rfcsr.bank_6352.TESTCSR_RFACC_REGNUM & 0x3F) == regID))
			{
				*pValue = (UCHAR)(rfcsr.bank_6352.RF_CSR_DATA);
				break;
			}
#endif /* RT6352 */
#ifdef RT65xx
			if (IS_RT65XX(pAd) && (rfcsr.bank_65xx.RF_CSR_REG_ID == regID) &&
				(rfcsr.bank_65xx.RF_CSR_REG_BANK == bank))
			{
				*pValue = (UCHAR)(rfcsr.bank_65xx.RF_CSR_DATA);
				break;
			}
#endif /* RT65xx */
#ifdef MT7601
			if (IS_MT7601(pAd) && (rfcsr.bank_7601.RF_CSR_REG_ID == regID) &&
				(rfcsr.bank_7601.RF_CSR_REG_BANK == bank))
			{
				*pValue = (UCHAR)(rfcsr.bank_7601.RF_CSR_DATA);
				break;
			}
#endif /* MT7601 */
		}
	}

	if (rf_status == BUSY)
	{																	
		DBGPRINT_ERR(("RF read R%d=0x%X fail, i[%d], k[%d]\n", regID, rfcsr.word,i,k));
		goto done;
	}
	ret = STATUS_SUCCESS;
#if 0
	if (bank >= RF_BANK4)
	{
		printk("TESTCSR_RFACC_REGNUM = %x, RF_CSR_DATA = %x !!!\n", rfcsr.field.TESTCSR_RFACC_REGNUM, rfcsr.field.RF_CSR_DATA);
	}
#endif

done:
#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		RTMP_SEM_EVENT_UP(&pAd->reg_atomic);
	}
#endif /* RTMP_MAC_USB */

	return ret;
}

#endif /* RLT_RF */

