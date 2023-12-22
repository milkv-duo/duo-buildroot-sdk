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
	Who 		When			What
	--------	----------		----------------------------------------------
*/

#include "rt_config.h"

#ifdef CONFIG_FPGA_MODE

#ifdef CAPTURE_MODE
#define DSCOPE_CTL		0x0c00
#define DSCOPE_STS		0x0c04
#define CAP_CTL			0x0c08

/* 1: ADC6, 2: ADC8, 3: FEQ */
typedef enum _CAP_MODE_TYPE{
	CAP_MODE_ADC6 = 1,
	CAP_MODE_ADC8 = 2,
	CAP_MODE_FEQ = 3,
}CAP_MODE_TYPE;


typedef enum _CAP_TRIGGER_TYPE{
	CAP_TRIGGER_MANUAL = 1,
	CAP_TRIGGER_AUTO = 2,
}CAP_TRIGGER_MODE;


static INT asic_pbf_reset(RTMP_ADAPTER *pAd)
{
	UINT32 mac_val;

	DBGPRINT(RT_DEBUG_OFF, ("%s(): Do PBF reset\n",__FUNCTION__));

	RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &mac_val);
	mac_val |= 0x8;
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, mac_val);
	RtmpusecDelay(10000);
	RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &mac_val);
	mac_val &= (~0x8);
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, mac_val);

	return TRUE;
}


static INT asic_cap_mode_set(RTMP_ADAPTER *pAd, BOOLEAN cap)
{
	UINT32 mac_val;

	/* set to cap mode */
	RTMP_IO_READ32(pAd, RLT_PBF_CFG, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("%s(): cap=%d, mac_val=0x%x\n",
				__FUNCTION__, cap, mac_val));
	if (cap == TRUE)
		mac_val |= 0x200;
	else
		mac_val &= (~0x200);
	RTMP_IO_WRITE32(pAd, RLT_PBF_CFG, mac_val);

	RTMP_IO_READ32(pAd, RLT_PBF_CFG, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("%s(): Config CaptureMode for RLT_PBF_CFG=0x%x\n",
				__FUNCTION__, mac_val));

	return TRUE;
}



INT asic_cap_config(RTMP_ADAPTER *pAd)
{
	UINT32 mac_val, mask = 0;
	UINT32 bbp_val;

	/* capture mode */
	
	switch (pAd->fpga_ctl.cap_type)
	{
		case CAP_MODE_FEQ:
			mask = 0x20000;
			break;
		case CAP_MODE_ADC6:
			mask = 0x10000;
			break;
		case CAP_MODE_ADC8:
		default:
			mask = 0x0;
			break;
	}
	/*
		for trigger offset,
		if use bit 0~ 14, it means in unit of bytes,
		if use bit 2~15, it means in unit of bits
	*/
	mask |= (pAd->fpga_ctl.trigger_offset & 0x7fff);
	RTMP_IO_READ32(pAd, CAP_CTL, &mac_val);
	mac_val &= (~0x3ffff);
	mac_val |= mask;
	RTMP_IO_WRITE32(pAd, CAP_CTL, mac_val);

	/* capture data source */
	RTMP_BBP_IO_READ32(pAd, DSC_R8, &bbp_val);
	bbp_val &= (~0xff);
	RTMP_BBP_IO_WRITE32(pAd, DSC_R8, bbp_val);

	
	return TRUE;
}


typedef	union _DATA_SCOPE_BUFFER {
	struct 
	{
		INT32 BYTE0:8;
		INT32 BYTE1:8;
		INT32 BYTE2:8;
		INT32 BYTE3:8;
	} field;
	INT32 Value;
}DATA_SCOPE_BUFFER, *PDATA_SCOPE_BUFFER;


VOID cap_dump(RTMP_ADAPTER *pAd, CHAR *bank1, CHAR *bank2, INT len)
{
	int i;
	CHAR *ptr1, *ptr2;
	DATA_SCOPE_BUFFER *cap_entry1, *cap_entry2;
	CHAR str_buf[64], sign[4];
	
	ptr1 = bank1;
	ptr2 = bank2;
	for (i = 0; i < len; i++)
	{
		// Tn:
		// Rx0_I / Rx0_Q
		cap_entry1 = (DATA_SCOPE_BUFFER *)(ptr1 + i *sizeof(DATA_SCOPE_BUFFER));
		sign[0] = (cap_entry1->field.BYTE3 > 0) ? '+' : ' ';
		sign[1] = (cap_entry1->field.BYTE2 > 0) ? '+' : ' ';

		// Rx1_I / Rx1_Q
		cap_entry2 = (DATA_SCOPE_BUFFER *)(ptr2 + i *sizeof(DATA_SCOPE_BUFFER));
		sign[2] = (cap_entry2->field.BYTE3 > 0) ? '+' : ' ';
		sign[3] = (cap_entry2->field.BYTE2 > 0) ? '+' : ' ';

		sprintf(str_buf, "%c%d\t%c%d\t%c%d\t%c%d\n",
					sign[0], cap_entry1->field.BYTE3,
					sign[1], cap_entry1->field.BYTE2,
					sign[2], cap_entry2->field.BYTE3,
					sign[3], cap_entry2->field.BYTE2);

		DBGPRINT(RT_DEBUG_OFF, ("%s", str_buf));
		NdisZeroMemory(str_buf, 64);
		
		// T(n+1)
		// Rx0_I / Rx0_Q
		sign[0] = (cap_entry1->field.BYTE1 > 0) ? '+' : ' ';
		sign[1] = (cap_entry1->field.BYTE0 > 0) ? '+' : ' ';

		// Rx1_I / Rx1_Q
		sign[2] = (cap_entry2->field.BYTE1 > 0) ? '+' : ' ';
		sign[3] = (cap_entry2->field.BYTE0 > 0) ? '+' : ' ';
		sprintf(str_buf, "%c%d\t%c%d\t%c%d\t%c%d\n",
					sign[0], cap_entry1->field.BYTE1,
					sign[1], cap_entry1->field.BYTE0,
					sign[2], cap_entry2->field.BYTE1,
					sign[3], cap_entry2->field.BYTE0);

		DBGPRINT(RT_DEBUG_OFF, ("%s", str_buf));
		NdisZeroMemory(str_buf, 64);
	}

	DBGPRINT(RT_DEBUG_OFF, ("\n"));
}


static INT asic_cap_buf_get(RTMP_ADAPTER *pAd, INT bank, UCHAR *buf, UINT32 cap_offset)
{
	UINT32 mac_val, s_addr, e_addr;
	UCHAR *buf_ptr;
	INT len;


	s_addr = 0x8000 + cap_offset;
	if (cap_offset == 0)
		e_addr = 0xffff;
	else
		e_addr = s_addr - 1;
		
	/* bank switch */
	RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &mac_val);
	mac_val &= (~0x60000);
	if (bank == 1)
		mac_val |= 0x20000;
	else
		mac_val |= 0x40000;
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, mac_val);

	len = 0;
	buf_ptr = buf;

	for (s_addr = cap_offset + 0x8000; s_addr < 0xffff; s_addr += 4)
	{
		RTMP_IO_READ32(pAd, s_addr, buf_ptr);
		len += 4;
		buf_ptr += 4;
	}

	if (e_addr < 0xffff) {
		for (s_addr = 0x8000; s_addr < e_addr; s_addr += 4)
		{
			RTMP_IO_READ32(pAd, s_addr, buf_ptr);
			len += 4;
			buf_ptr += 4;
		}
	}

	DBGPRINT(RT_DEBUG_OFF, ("%s(): Read Bank %d Info, startAddr=0x%x, endAddr=0x%x, len=%d\n",
				__FUNCTION__, bank, cap_offset + 0x8000, e_addr, len));

	return len;
}


INT cap_status_chk_and_get(RTMP_ADAPTER *pAd)
{
	INT cnt = 0;
	UINT32 mac_val, cap_offset;
	BOOLEAN read_cap = FALSE;


	if (pAd->fpga_ctl.do_cap && (pAd->fpga_ctl.cap_done == FALSE)) {
		RTMP_IO_READ32(pAd, DSCOPE_STS, &mac_val);
		if ((mac_val & 0x10000) == 0x0) {
			pAd->fpga_ctl.do_cap = FALSE;
			pAd->fpga_ctl.cap_done = TRUE;
			read_cap = TRUE;
			cap_offset = mac_val & 0x7fff;
			DBGPRINT(RT_DEBUG_OFF, ("%s():Capture done(DSCOPE_STS=0x%x, cap_offset=0x%x):\n",
						__FUNCTION__, mac_val, cap_offset));
		}
	}

	if (pAd->fpga_ctl.cap_done && (read_cap == TRUE))
	{
		CHAR *mem, *mem_bank1, *mem_bank2, buf_ptr;
		ULONG irq_flags;
		UINT32 len, s_addr, e_addr, mac_mask, pbf_reg_val;

		mem = pAd->fpga_ctl.cap_buf;
		if (!mem)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s(%d):malloc failed\n",
						__FUNCTION__, __LINE__));
			return FALSE;
		}

		s_addr = 0x8000 + cap_offset;
		if (cap_offset == 0)
			e_addr = 0xffff;
		else
			e_addr = s_addr - 1;
		buf_ptr = mem;
		mem_bank1 = mem;
		mem_bank2 = mem + 32 * 1024;

		DBGPRINT(RT_DEBUG_OFF, ("%s():Capture Info:\n", __FUNCTION__));
		DBGPRINT(RT_DEBUG_OFF, ("\tCaptureMode=%s\n", 
					(pAd->fpga_ctl.cap_type == CAP_MODE_ADC6 ? "ADC6" : 
					(pAd->fpga_ctl.cap_type == CAP_MODE_ADC8 ? "ADC8" : "FEQ"))));
		DBGPRINT(RT_DEBUG_OFF, ("\tTriggerMode=%s\n",
					pAd->fpga_ctl.cap_trigger == CAP_TRIGGER_AUTO ? "Auto" : "Manual"));
		DBGPRINT(RT_DEBUG_OFF, ("\tTriggerOffset=%d\n", pAd->fpga_ctl.trigger_offset));
		DBGPRINT(RT_DEBUG_OFF, ("\tStartAddr=0x%x\n", s_addr));
		DBGPRINT(RT_DEBUG_OFF, ("\tEndAddr=0x%x\n", e_addr));

		RTMP_INT_LOCK(&pAd->irq_lock, irq_flags);

		/* swap the share memory to packet buffer */
		RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &pbf_reg_val);
		mac_val = pbf_reg_val;
		mac_val &= (~0x80000);
		RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, mac_val);

		asic_cap_buf_get(pAd, 1, mem_bank1, cap_offset);
		asic_cap_buf_get(pAd, 2, mem_bank2, cap_offset);

//		RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &mac_val);
//		mac_val |= 0x80000;
		RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, pbf_reg_val);
		RTMP_INT_UNLOCK(&pAd->irq_lock, irq_flags);
	}

	return TRUE;
}


INT asic_cap_stop(RTMP_ADAPTER *pAd)
{
	UINT32 mac_val;

	RTMP_IO_READ32(pAd, DSCOPE_CTL, &mac_val);
	mac_val |= 0x200;
	RTMP_IO_WRITE32(pAd, DSCOPE_CTL, mac_val);

	return TRUE;
}


INT asic_cap_start(RTMP_ADAPTER *pAd)
{
	UINT32 mac_val, bbp_val;


	asic_cap_config(pAd);

	if (pAd->fpga_ctl.cap_buf)
		NdisZeroMemory(pAd->fpga_ctl.cap_buf, 64 * 1024);

	/* capture trigger condition */
	if (pAd->fpga_ctl.cap_trigger == CAP_TRIGGER_AUTO)
	{
		RTMP_BBP_IO_READ32(pAd, DSC_R0, &bbp_val);
		bbp_val |= 0x1;
		RTMP_BBP_IO_WRITE32(pAd, DSC_R0, bbp_val);	
	}

	RTMP_IO_READ32(pAd, DSCOPE_CTL, &mac_val);
	mac_val |= 0x113;
	RTMP_IO_WRITE32(pAd, DSCOPE_CTL, mac_val);

	if (pAd->fpga_ctl.cap_trigger == CAP_TRIGGER_MANUAL)
	{
		RTMP_IO_READ32(pAd, CAP_CTL, &mac_val);
		mac_val |= 0x100000;
		RTMP_IO_WRITE32(pAd, CAP_CTL, mac_val);
	}
	pAd->fpga_ctl.cap_done = FALSE;
	pAd->fpga_ctl.do_cap = TRUE;

	DBGPRINT(RT_DEBUG_OFF, ("%s(): Config CaptureMode for CAP Registers\n", __FUNCTION__));
	RTMP_BBP_IO_READ32(pAd, DSC_R0, &bbp_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tDSC_RO=0x%x\n", bbp_val));
	RTMP_IO_READ32(pAd, DSCOPE_CTL, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tDSCOPE_CTL=0x%x\n", mac_val));
	RTMP_IO_READ32(pAd, CAP_CTL, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tCAP_CTL=0x%x\n", mac_val));
	RTMP_IO_READ32(pAd, DSCOPE_STS, &mac_val);
	DBGPRINT(RT_DEBUG_OFF, ("\tDSCOPE_STS=0x%x\n", mac_val));
	
	return TRUE;
}


VOID cap_mode_init(RTMP_ADAPTER *pAd)
{
	UINT32 mac_val;


	DBGPRINT(RT_DEBUG_OFF, ("%s():CaptureMode=%d\n",
				__FUNCTION__, pAd->fpga_ctl.cap_support));
	pAd->fpga_ctl.cap_buf = RtmpOsVmalloc(0x10000);
	if (pAd->fpga_ctl.cap_buf == NULL)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s():cap buffer allocation failed!\n", __FUNCTION__));
		return;
	}

	if (pAd->fpga_ctl.cap_support == TRUE)
	{
		RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &mac_val);
		mac_val &= (~0xf00000);
		mac_val |= 0x800;
		RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, mac_val);

		asic_pbf_reset(pAd);
		asic_cap_mode_set(pAd, TRUE);
	}

	
}


VOID cap_mode_deinit(RTMP_ADAPTER *pAd)
{
	UINT32 mac_val;

	DBGPRINT(RT_DEBUG_OFF, ("CaptureMode=%d\n", pAd->fpga_ctl.cap_support));
	if (pAd->fpga_ctl.cap_support)
	{
		asic_pbf_reset(pAd);
		asic_cap_mode_set(pAd, FALSE);		
	}

	if (pAd->fpga_ctl.cap_buf) {
		RtmpOsVfree(pAd->fpga_ctl.cap_buf);
		pAd->fpga_ctl.cap_buf = NULL;
	}
}
#endif /* CAPTURE_MODE */


struct fpga_cmd{
	RTMP_STRING *cmd_id;
	INT (*fpga_func)(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
};


#if 0
INT set_phy_rate(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	return TRUE;
}


struct fpga_cmd cmd_list[] = 
{
	{"on",						set_fpga_mode}
	{"TxKickCnt",					set_tx_kickcnt},
	{"dataphy",					set_data_phy_mode},
	{"datamcs",					set_data_mcs},
};


INT fpga_cmd_handler(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{


}
#endif
#endif /* CONFIG_FPGA_MODE */

