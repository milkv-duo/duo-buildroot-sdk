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
	Network Acceleration related function

	Revision History:
	Who 		When			What
	--------	----------		----------------------------------------------
*/

#include "rt_config.h"


#if defined(CONFIG_CSO_SUPPORT) || defined(CONFIG_TSO_SUPPORT)
VOID dump_tsoinfo(TSO_INFO *tinfo)
{
	DBGPRINT(RT_DEBUG_OFF, ("TSO_INFO:\n"));
	hex_dump("\traw data", (UCHAR *)tinfo, sizeof(TSO_INFO));	
	DBGPRINT(RT_DEBUG_OFF, ("\ttcp=%d\n", tinfo->tcp));
	DBGPRINT(RT_DEBUG_OFF, ("\ttups=%d\n", tinfo->tups));
	DBGPRINT(RT_DEBUG_OFF, ("\tips=%d\n", tinfo->ips));
	DBGPRINT(RT_DEBUG_OFF, ("\tmss=%d\n", tinfo->mss));
}

#ifdef CONFIG_TSO_SUPPORT
INT rlt_tso_info_write(RTMP_ADAPTER *pAd, TSO_INFO *tinfo, TX_BLK *pTxBlk, int hdr_len)
{
	ULONG offset = GET_OS_PKT_DATAPTR(pTxBlk->pPacket) - pTxBlk->pSrcBufData + hdr_len;
	
	tinfo->ips = pTxBlk->tso_info.ips + offset;
	tinfo->tups = pTxBlk->tso_info.tups;
	tinfo->tcp = pTxBlk->tso_info.tcp;
	tinfo->mss = pTxBlk->tso_info.mss;

	if (pTxBlk->naf_type & NAF_TYPE_TSO)
		dump_tsoinfo(tinfo);
	return TRUE;
}
#endif /* CONFIG_TSO_SUPPORT */

INT rlt_net_acc_exit(RTMP_ADAPTER *pAd)
{

	return TRUE;
}


INT rlt_net_acc_init(RTMP_ADAPTER *pAd)
{
	UINT32 val;

#ifdef RT5592
	if (IS_ASIC_CAP(pAd, fASIC_CAP_CSO))
		rtmp_cso_enable(pAd);
#endif /* RT5592 */

#ifdef RT65xx
	if (IS_RT65XX(pAd)) {
		//RTMP_IO_READ32(pAd, FCE_L2_STUFF, &val);
		val = 0x3ff023b;
		if (IS_ASIC_CAP(pAd, fASIC_CAP_CSO | fASIC_CAP_TSO))
			val |= 0x8; /* enable TSO_INFO field removal operation for FCE */
		else
			val &= (~0x8);
		RTMP_IO_WRITE32(pAd, FCE_L2_STUFF, val);

		if (IS_ASIC_CAP(pAd, fASIC_CAP_TSO)) {
			RTMP_IO_WRITE32(pAd, TSO_CTRL, 0x50050);
		} else {
			RTMP_IO_WRITE32(pAd, TSO_CTRL, 0);
		}
		
		if (IS_ASIC_CAP(pAd, fASIC_CAP_CSO)) {
			RTMP_IO_WRITE32(pAd, FCE_PARAMETERS, 0xf6157f0f);
			RTMP_IO_WRITE32(pAd, FCE_CSO, 0x30f); /* MT7650E2 only support CSO RX. */
		}
	}
#endif /* RT65xx */

	return TRUE;
}
#endif /* defined(CONFIG_CSO_SUPPORT) || defined(CONFIG_TSO_SUPPORT) */

