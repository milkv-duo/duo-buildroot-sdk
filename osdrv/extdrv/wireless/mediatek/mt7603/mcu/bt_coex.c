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
	Who		 When		  What
	--------	----------	----------------------------------------------
*/

#ifdef MT76XX_BTCOEX_SUPPORT
#include	"rt_config.h"

VOID SendAndesCoexFrameInfo(
	IN PRTMP_ADAPTER pAd, 
	IN ULONG TriggerNumber) 
{
	   INT ret;
	 struct cmd_msg *msg;
	//struct CMD_UNIT CmdUnit;
	COEX_PROTECTION_FRAME_INFO coexProtectionFrameInfo = {0};
	USHORT coexProtectionFrameInfoLength = 0;

	printk("%s: -->\n", __FUNCTION__);

	coexProtectionFrameInfo.CoexOperation = TypeProtectionFrame;
	coexProtectionFrameInfo.Triggernumber = pAd->NullFrameSpace[TriggerNumber].Triggernumber;
	coexProtectionFrameInfo.Valid = pAd->NullFrameSpace[TriggerNumber].Valid;
	coexProtectionFrameInfo.NodeType = pAd->NullFrameSpace[TriggerNumber].NodeType;
	coexProtectionFrameInfo.BssHashID = pAd->NullFrameSpace[TriggerNumber].BssHashID;
	coexProtectionFrameInfo.FrameType = pAd->NullFrameSpace[TriggerNumber].FrameType;
	
	coexProtectionFrameInfoLength = sizeof(coexProtectionFrameInfo);

	printk("%s: Triggernumber = %d, Valid = %d, NodeType = %d, BssHashID = %d, , FrameType = %d, CmdParametersLength = %d\n", 
		__FUNCTION__, 
		coexProtectionFrameInfo.Triggernumber, 
		coexProtectionFrameInfo.Valid, 
		coexProtectionFrameInfo.NodeType,
			  coexProtectionFrameInfo.BssHashID,
			  coexProtectionFrameInfo.FrameType,
			  coexProtectionFrameInfoLength
			  );

		msg = AndesAllocCmdMsg(pAd, coexProtectionFrameInfoLength);
		AndesInitCmdMsg(msg, PKT_CMD_TYPE_COEX_OP, FALSE, 0, TRUE, FALSE, 0, NULL, NULL);
	  	AndesAppendCmdMsg(msg, (char *)&coexProtectionFrameInfo,coexProtectionFrameInfoLength);
		ret = AndesSendCmdMsg(pAd, msg);
#if 0
	TxPktCmd(pAd, PKT_CMD_TYPE_COEX_OP, NO_PKT_CMD_RSP_EVENT, 
		&coexProtectionFrameInfo, coexProtectionFrameInfoLength, 0);

		NdisZeroMemory(&CmdUnit, sizeof(CmdUnit));
		CmdUnit.u.ANDES.Type = PKT_CMD_TYPE_COEX_OP;
		CmdUnit.u.ANDES.CmdPayloadLen = coexProtectionFrameInfoLength;
		CmdUnit.u.ANDES.CmdPayload = &coexProtectionFrameInfo;

		CmdUnit.u.ANDES.NeedRsp = FALSE;
		CmdUnit.u.ANDES.NeedWait = FALSE;
		CmdUnit.u.ANDES.Timeout = 0;

		ret = AsicSendCmdToAndes(pAd, &CmdUnit);
#endif
	printk("%s: <--\n", __FUNCTION__);
	
}


VOID UpdateAndesNullFrameSpace(
	IN PRTMP_ADAPTER	pAd)
{
	char iter = 0;
	for (iter=0; iter < NULLFRAMESPACE; iter++ )
	{
		if (pAd->NullFrameSpace[iter].Occupied != 0 ) 
		{
	 	   DBGPRINT(RT_DEBUG_TRACE, ("Coex: Send protection frame %d\n",iter));		
		   SendAndesCoexFrameInfo(pAd, iter);
		}
		
	}

}


VOID SendAndesAFH(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			BBPCurrentBW,
	IN UCHAR			Channel,
	IN UCHAR			CentralChannel,
	IN BOOLEAN			Disable,
	IN ULONG					 BssHashID) 
{
	
	COEX_AFH coexAFH = {0};
	USHORT coexAFHLength = 0;
	INT ret;
	//struct CMD_UNIT CmdUnit;
	struct cmd_msg *msg;
	printk("%s: -->\n", __FUNCTION__);
	coexAFH.CoexOperation = TypeAFH;
		if (!IS_MT76XXBTCOMBO(pAd))
			return;

		if (BBPCurrentBW == BW_40)
		{
			coexAFH.BW= BW_40 + COEXNOZEROSHIFT ;
			coexAFH.Channel= CentralChannel;
		}
		else if (BBPCurrentBW == BW_20)
		{
			coexAFH.BW= BW_20 + COEXNOZEROSHIFT;
			coexAFH.Channel= Channel;
		}
		else if (BBPCurrentBW == BW_80)
		{
			coexAFH.BW= BW_80 + COEXNOZEROSHIFT;
			coexAFH.Channel= Channel;
		}

		if (Channel > 14)
		{
			coexAFH.BW= 0;
		}
		
		if (Disable == FALSE)
		{
			coexAFH.LinkStatus = COEX_WIFI_LINK_UP;
		}
		else
		{
		coexAFH.LinkStatus = COEX_WIFI_LINK_DOWN;
		}


		coexAFH.BssHashID = BssHashID;

		coexAFHLength = sizeof(coexAFH);

		msg = AndesAllocCmdMsg(pAd, coexAFHLength);
		AndesInitCmdMsg(msg, PKT_CMD_TYPE_COEX_OP, FALSE, 0, TRUE, FALSE, 0, NULL, NULL);
	  	AndesAppendCmdMsg(msg, (char *)&coexAFH,coexAFHLength);
		ret = AndesSendCmdMsg(pAd, msg);
		
	printk("%s: LinkStatus = %d, BW = %d, Channel = %d\n,  BssHashID = %d PktLength = %d\n", 
		__FUNCTION__, 
		coexAFH.LinkStatus, 
		coexAFH.BW, 
		coexAFH.Channel,
		coexAFH.BssHashID, 
		coexAFHLength
		);
#if 0
		NdisZeroMemory(&CmdUnit, sizeof(CmdUnit));
		CmdUnit.u.ANDES.Type = PKT_CMD_TYPE_COEX_OP;
		CmdUnit.u.ANDES.CmdPayloadLen = coexAFHLength;
		CmdUnit.u.ANDES.CmdPayload = &coexAFH;

		CmdUnit.u.ANDES.NeedRsp = FALSE;
		CmdUnit.u.ANDES.NeedWait = FALSE;
		CmdUnit.u.ANDES.Timeout = 0;

		ret = AsicSendCmdToAndes(pAd, &CmdUnit);
		
	TxPktCmd(pAd, PKT_CMD_TYPE_COEX_OP, NO_PKT_CMD_RSP_EVENT, 
		&coexAFH, coexAFHLength, 0);
#endif
	printk("%s: <--\n", __FUNCTION__);
	
}


VOID BtAFHCtl(
		IN PRTMP_ADAPTER	pAd,
		IN UCHAR			BBPCurrentBW,
		IN UCHAR			Channel,
		IN UCHAR			CentralChannel,
		IN BOOLEAN			Disable)
{
	UCHAR Kstart = 0, Kend = 0;
	BT_FUN_INFO_STRUC btFunInfo={0};
	
	if (!(IS_MT76XXBTCOMBO(pAd) || Channel>14))
		return;

	if (BBPCurrentBW == BW_40)
	{
		if (CentralChannel <= 4 )
		{
			Kstart = 0;
			Kend = 53;
		}
		else if (CentralChannel >=10)
		{
			Kstart = 25;
			Kend = 78;		
		}
		else
		{
			Kstart = 2 + (CentralChannel-5)*5;
			Kend = Kstart + 55;
		}
	}
	else
	{
		if (Channel <= 3 )
		{
			Kstart = 0;
			Kend = 47;
		}
		else if (Channel >=10)
		{
			Kstart = 31;
			Kend = 78;		
		}
		else
		{
			Kstart = 1 + (Channel-4)*5;
			Kend = Kstart + 48;
		}
	}

	RTMP_IO_READ32(pAd, BT_FUN_INFO, &btFunInfo.word);
	btFunInfo.word &= ~(0x3FFFFFFF); //Clear Power and AFH but keep active bit 
	if (!Disable)
	{
		btFunInfo.field.AFH_START_CH = Kstart;
		btFunInfo.field.AFH_END_CH = Kend;
		/* These Code and Definition are gone
		if (pAd->CommonCfg.BBPCurrentBW == BW_40)
			//0x04 // -14
			btFunInfo.field.BTPower0 = 0x0c;	//-8
		else
			btFunInfo.field.BTPower0 = 0x1c;	//-2
		*/
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s: COEX AFH Start Ch = %d, AFH End Ch = %d, Channel = %d, CentralChannel = %d\n",
		__FUNCTION__, 
		btFunInfo.field.AFH_START_CH,
		btFunInfo.field.AFH_END_CH,
		Channel,
		CentralChannel));
	
	RTMP_IO_WRITE32(pAd, BT_FUN_INFO, btFunInfo.word);
		// Hook andes AFH command
	   
	// High BT Priority Mode
	//RTMP_IO_WRITE32(pAd, 0x5c, 0x8000);		
}


ULONG QueryHashID(
	IN PRTMP_ADAPTER pAd, 
	PUCHAR pAddr,
	BOOLEAN RemoveBSS) 
{
	ULONG HashIdx;
	UCHAR i,j = 0;

	for(j = 1; j < NULLFRAMESPACE; j++)
	{
		if (MAC_ADDR_EQUAL(&pAd->HASH_BSSID[j][0],pAddr))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("COEX: j = %d\n",j));
  			DBGPRINT(RT_DEBUG_TRACE, ("COEX: Find BSSID Address = %02X:%02X:%02X:%02X:%02X:%02X\n",pAd->HASH_BSSID[j][0],pAd->HASH_BSSID[j][1],
			pAd->HASH_BSSID[j][2],pAd->HASH_BSSID[j][3],
			pAd->HASH_BSSID[j][4], pAd->HASH_BSSID[j][5]));	
			HashIdx = j;
			if (RemoveBSS == TRUE)
			{
				pAd->HASH_BSSID[j][MAC_ADDR_LEN] = 0;
				DBGPRINT(RT_DEBUG_TRACE, ("COEX: Remove BSSID Address  = %02X:%02X:%02X:%02X:%02X:%02X\n",pAd->HASH_BSSID[j][0],pAd->HASH_BSSID[j][1],
				pAd->HASH_BSSID[j][2],pAd->HASH_BSSID[j][3],
				pAd->HASH_BSSID[j][4], pAd->HASH_BSSID[j][5]));	
				
			}
	return HashIdx;
}
		//MAC_ADDR_EQUAL(&pAd->HASH_BSSID[j][0],)
		DBGPRINT(RT_DEBUG_TRACE, ("COEX:Miss search  j = %d\n",j));
	}
	for(j = 1; j < NULLFRAMESPACE; j++)
	{
		if (pAd->HASH_BSSID[j][MAC_ADDR_LEN] == 0)
		{
			COPY_MAC_ADDR(&pAd->HASH_BSSID[j][0], pAddr);
			pAd->HASH_BSSID[j][MAC_ADDR_LEN] = 1;
			HashIdx = j;		
		
			DBGPRINT(RT_DEBUG_TRACE, ("COEX: free space j = %d\n",j));
			DBGPRINT(RT_DEBUG_TRACE, ("COEX: Insert MAC Address  = %02X:%02X:%02X:%02X:%02X:%02X\n",pAd->HASH_BSSID[j][0],pAd->HASH_BSSID[j][1],
			pAd->HASH_BSSID[j][2],pAd->HASH_BSSID[j][3],
			pAd->HASH_BSSID[j][4], pAd->HASH_BSSID[j][5]));   
			return HashIdx;
		}
		//debug if there is an overflow issue
		if (j == (NULLFRAMESPACE -1 ))
		{
			COPY_MAC_ADDR(&pAd->HASH_BSSID[j][0], pAddr);
			pAd->HASH_BSSID[1][MAC_ADDR_LEN] = 1;
			pAd->HASH_BSSID[2][MAC_ADDR_LEN] = 0;
			pAd->HASH_BSSID[3][MAC_ADDR_LEN] = 0;
			HashIdx = 1;	

			return HashIdx;
		}
	}
	return 0;
}


VOID RemoveProtectionFrameSpace(
	IN	PRTMP_ADAPTER	pAd,
	IN	  PUCHAR  pAddr
)
{
	ULONG BssHashID =  QueryHashID(pAd, pAddr, FALSE);
	UCHAR iter = 0;
	for (iter=0; iter < NULLFRAMESPACE; iter++ )
	{
		if (pAd->NullFrameSpace[iter].BssHashID = BssHashID)
		{
			 pAd->NullFrameSpace[iter].Occupied = FALSE;   
		}
	}

}


VOID FillProtectionFrameSpace(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG	Triggernumber,
	IN	  ULONG	Valid,
	IN	  ULONG	NodeType,
	IN	  PUCHAR  pAddr,
	IN	  ULONG	FrameType
)
{
	pAd->NullFrameSpace[Triggernumber].Occupied = TRUE;
	pAd->NullFrameSpace[Triggernumber].Triggernumber = Triggernumber;
	pAd->NullFrameSpace[Triggernumber].Valid = Valid;
	pAd->NullFrameSpace[Triggernumber].NodeType = NodeType;
	pAd->NullFrameSpace[Triggernumber].BssHashID = QueryHashID(pAd, pAddr, FALSE);
	pAd->NullFrameSpace[Triggernumber].FrameType= FrameType;

}


VOID PrepareProtectionFrame(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG		   Type,
	IN	ULONG		   Number,
	IN	ULONG		NAV,
	IN	ULONG		   OPMode,
	IN	PMAC_TABLE_ENTRY pEntry)
{
	HEADER_802_11	   ProtectionFrame ={0};
	TXWI_STRUC		  ProtectionFrameTxWI ={0};
	TXWI_STRUC		  *pTxWI;
	UCHAR			   *ptr = NULL;
	UINT				i = 0;
	//PHY_CFG PhyCfg = {0};
	UCHAR			   Wcid = 0;
	UCHAR			   Length = 0;
	ULONG			   FrameAddress = 0;
	BOOLEAN			 Ack = FALSE;
	MAC_TX_INFO mac_info;
	
	DBGPRINT(RT_DEBUG_TRACE, ("==>PrepareProtectionFrame\n"));	

	NdisZeroMemory(&ProtectionFrameTxWI,pAd->chipCap.TXWISize);
	NdisZeroMemory(&ProtectionFrame, sizeof(HEADER_802_11));
	
	//ProtectionFrame.FC.Type = BTYPE_DATA;
		switch (Type){
			
			case CTSTOSELF:
			ProtectionFrame.FC.Type =FC_TYPE_CNTL;
				ProtectionFrame.FC.SubType = SUBTYPE_CTS;
				break;
				
			case CFEND:
			ProtectionFrame.FC.Type = FC_TYPE_CNTL;
				ProtectionFrame.FC.SubType = SUBTYPE_CFEND;
				break;
				
			case POWERSAVE1:
			ProtectionFrame.FC.Type =FC_TYPE_DATA;
				ProtectionFrame.FC.SubType = SUBTYPE_DATA_NULL; 
				ProtectionFrame.FC.PwrMgmt = PWR_SAVE;
				ProtectionFrame.FC.ToDs = 1;
		   ProtectionFrame.FC.FrDs = 0;		
				break;
				
			case POWERSAVE0:
			ProtectionFrame.FC.Type =FC_TYPE_DATA;
				ProtectionFrame.FC.SubType = SUBTYPE_DATA_NULL;	  
				ProtectionFrame.FC.PwrMgmt = PWR_ACTIVE;
				ProtectionFrame.FC.ToDs = 1;
		   ProtectionFrame.FC.FrDs = 0;
				break;						 
		}

	   if (Type == CTSTOSELF)  
	   {
			COPY_MAC_ADDR(ProtectionFrame.Addr1, pAd->CurrentAddress);
			ProtectionFrame.Duration = (USHORT)NAV;
			Wcid = 0xff;
			Length = 10;
	   }
	   else if (Type == CFEND)
	   {
			 COPY_MAC_ADDR(ProtectionFrame.Addr1, BROADCAST_ADDR);
			 COPY_MAC_ADDR(ProtectionFrame.Addr2, pAd->CurrentAddress);
			 ProtectionFrame.Duration = 0;
			 Wcid = 0xff;
			 Length = 16;
	   }
	   else
	   {
			pAd->Sequence	   = (pAd->Sequence+1) & MAXSEQ;//((pAd->Sequence) + 1) & (MAX_SEQ_NUMBER);
		   ProtectionFrame.Sequence = pAd->Sequence;
			  ProtectionFrame.Duration = RTMPCalcDuration(pAd, RATE_1, 14);				

		COPY_MAC_ADDR(ProtectionFrame.Addr1, pEntry->Addr);
				COPY_MAC_ADDR(ProtectionFrame.Addr2,  pAd->CurrentAddress);
				COPY_MAC_ADDR(ProtectionFrame.Addr3,  pAd->CommonCfg.Bssid);
			  Ack =TRUE;
			  Wcid = BSSID_WCID;
			  Length =  sizeof(HEADER_802_11) ;
	   }

	pTxWI = (TXWI_STRUC *)&ProtectionFrameTxWI;

	NdisZeroMemory((UCHAR *)&mac_info, sizeof(mac_info));	
	mac_info.FRAG = FALSE;
	
	mac_info.CFACK = FALSE;
	mac_info.InsTimestamp = FALSE;
	mac_info.AMPDU = FALSE;
	
	mac_info.BM = IS_BM_MAC_ADDR(ProtectionFrame.Addr1);
	mac_info.Ack = FALSE; // why don't use "Ack" paramter checked in previous?
	mac_info.NSeq = FALSE;
	mac_info.BASize = 0;
	
	mac_info.WCID = pEntry->Aid;
	mac_info.Length = Length;
	mac_info.PID = 0;
	
	mac_info.TID = 0;
	mac_info.TxRate = (UCHAR)pAd->CommonCfg.MlmeTransmit.field.MCS;
	mac_info.Txopmode = IFS_HTTXOP;
	mac_info.Preamble = LONG_PREAMBLE;
	mac_info.q_idx = Q_IDX_AC0;

	write_tmac_info(pAd, (UCHAR *)pTxWI, &mac_info, &pAd->CommonCfg.MlmeTransmit);

	pTxWI->TXWI_N.PHYMODE = MODE_CCK;
	pTxWI->TXWI_N.MCS= 0;

	  if (Number == 0)
	  {
			FrameAddress = HW_NULL_BASE + 0x4000;
	  }
	  else if (Number == 1)
	  {
			FrameAddress = HW_NULL2_BASE + 0x4000;
	  }
	  else
	  {
		//trigger number 0/1 is belong to null number address 
		//Beacon address from D000
			FrameAddress = 0xD000 + (0x200*(Number-2));
	  }
	  DBGPRINT(RT_DEBUG_TRACE, ("Protection FrameAddress =%x \n",FrameAddress));
	//
	// Move TXWI and frame content to on-chip memory
	//
	ptr = (PUCHAR)&ProtectionFrameTxWI;
	for (i=0; i<pAd->chipCap.TXWISize; i+=4)  // 24-byte TXINFO field
	{
		RTMP_IO_WRITE32(pAd, FrameAddress + i, *((UINT32 *)ptr));
		ptr +=4 ;
	}

	
	ptr = (PUCHAR)&ProtectionFrame;
	DBGPRINT(RT_DEBUG_TRACE, ("Type =%x \n",Type));
	hex_dump("PrepareProtectionFrame" ,ptr, Length);
	for (i = 0; i < Length; i+=4)
	{
		RTMP_IO_WRITE32(pAd, FrameAddress + pAd->chipCap.TXWISize + i, *((UINT32 *)ptr));
		ptr +=4;
	}
}


VOID InvalidProtectionFrameSpace(
	IN	PRTMP_ADAPTER	pAd,
	IN	  PUCHAR  pAddr
)
{
	ULONG BssHashID =  QueryHashID(pAd, pAddr, FALSE);
	UCHAR iter = 0;
	for (iter=0; iter < NULLFRAMESPACE; iter++ )
	{
		if (pAd->NullFrameSpace[iter].BssHashID = BssHashID)
		{
			 pAd->NullFrameSpace[iter].Valid= FALSE;   
		}		
	}

}


UCHAR CheckAvailableNullFrameSpace(
	IN PRTMP_ADAPTER	pAd)
{
	char iter = 0;
	for (iter=0; iter < NULLFRAMESPACE; iter++ )
	{
		if ((pAd->NullFrameSpace[iter].Occupied) == 0 ) 
		{
			return iter;
		}
		
	}

	return NULLFRAMESPACE;

}


VOID EstablishFrameBundle(
	IN	PRTMP_ADAPTER	pAd,
	IN	  PUCHAR  pAddr,
	IN	  ULONG  OPMode,
	IN	PMAC_TABLE_ENTRY pEntry
)
{
	
	UCHAR n0, n1, n2, n3 = 0;
	n0 = CheckAvailableNullFrameSpace(pAd);
	DBGPRINT(RT_DEBUG_TRACE, ("COEX: Protection FrameBaseNumber=%d\n",n0));
	if (n0 != NULLFRAMESPACE)
	{
		PrepareProtectionFrame(pAd, CTSTOSELF, n0, 2500, OPMode, pEntry);
	 FillProtectionFrameSpace(pAd, n0, PROTECTIONFRAMEREADY  , OPMode, pAddr, CTSTOSELF);
	}
	n1 = CheckAvailableNullFrameSpace(pAd);
	DBGPRINT(RT_DEBUG_TRACE, ("COEX: Protection FrameBaseNumber=%d\n",n1));
	if (n1 != NULLFRAMESPACE)
	{
		PrepareProtectionFrame(pAd, POWERSAVE1, n1, 0, OPMode, pEntry);
		FillProtectionFrameSpace(pAd, n1, PROTECTIONFRAMEREADY  , OPMode, pAddr, POWERSAVE1);
	}
	n2 = CheckAvailableNullFrameSpace(pAd);
	DBGPRINT(RT_DEBUG_TRACE, ("COEX: Protection FrameBaseNumber=%d\n",n2));
	if (n2 != NULLFRAMESPACE)
	{
		PrepareProtectionFrame(pAd, CFEND, n2, 0, OPMode, pEntry);
		FillProtectionFrameSpace(pAd, n2, PROTECTIONFRAMEREADY  , OPMode, pAddr, CFEND);
	}
	n3 = CheckAvailableNullFrameSpace(pAd);
	DBGPRINT(RT_DEBUG_TRACE, ("COEX: Protection FrameBaseNumber=%d\n",n3));
	if (n3 != NULLFRAMESPACE)
	{
		PrepareProtectionFrame(pAd, POWERSAVE0, n3, 0, OPMode, pEntry);
		FillProtectionFrameSpace(pAd, n3, PROTECTIONFRAMEREADY  , OPMode, pAddr, POWERSAVE0);
	}
  
	
}


VOID SendAndesWLANStatus(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			WlanStatus,
	IN ULONG			PrivilegeTime, 
	IN ULONG					 BssHashID
	)
{
	
	COEX_WLAN_STATUS wlanStatus = {0};
	USHORT wlanStatusLength = 0;
	INT ret;
	//struct CMD_UNIT CmdUnit;
	struct cmd_msg *msg;
	printk("%s: -->\n", __FUNCTION__);
	wlanStatus.CoexOperation = TypeWiFiStatus;
	wlanStatus.WLANStatus= WlanStatus;
	wlanStatus.PrivilegeTime= PrivilegeTime;
	wlanStatus.BssHashID = BssHashID;
	wlanStatusLength = sizeof(wlanStatus);


	msg = AndesAllocCmdMsg(pAd, wlanStatusLength);
	AndesInitCmdMsg(msg, PKT_CMD_TYPE_COEX_OP, FALSE, 0, FALSE, FALSE, 0, NULL, NULL);
	AndesAppendCmdMsg(msg, (char *)&wlanStatus, wlanStatusLength);
	ret = AndesSendCmdMsg(pAd, msg);

	printk("%s: CoexOperation = %d, WlanStatus = %d, PrivilegeTime = %d,BssHashID = %d, PktLength = %d\n", 
		__FUNCTION__, 
		wlanStatus.CoexOperation, 
		wlanStatus.WLANStatus,
		wlanStatus.PrivilegeTime,
		wlanStatus.BssHashID,
		wlanStatusLength
		);
#if 0
		NdisZeroMemory(&CmdUnit, sizeof(CmdUnit));
		CmdUnit.u.ANDES.Type = PKT_CMD_TYPE_COEX_OP;
		CmdUnit.u.ANDES.CmdPayloadLen = wlanStatusLength;
		CmdUnit.u.ANDES.CmdPayload = &wlanStatus;

		CmdUnit.u.ANDES.NeedRsp = FALSE;
		CmdUnit.u.ANDES.NeedWait = FALSE;
		CmdUnit.u.ANDES.Timeout = 0;

		ret = AsicSendCmdToAndes(pAd, &CmdUnit);

	TxPktCmd(pAd, PKT_CMD_TYPE_COEX_OP, NO_PKT_CMD_RSP_EVENT, 
		&wlanStatus, wlanStatusLength, 0);
#endif
	printk("%s: <--\n", __FUNCTION__);
	
}


VOID CoexFDDRXAGCGain(
	IN PRTMP_ADAPTER pAd, 
	IN CHAR rssi) 
{
///the following code block shoule not be enabled.
#if 0
	CHAR LMthreshold, MHthreshold = 0;
	UCHAR idx = 0;
	UINT32 bbp_val, bbp_reg = AGC1_R8;
	
	if (pAd->coexRXManualAGCGain.bEnable)
	{
		LMthreshold = pAd->coexRXManualAGCGain.LMthreshold;
		MHthreshold = pAd->coexRXManualAGCGain.MHthreshold;
	}
	else
	{
		LMthreshold = -pAd->CommonCfg.CoexRXAGCLMTreshold;
		MHthreshold = -pAd->CommonCfg.CoexRXAGCMHTreshold; 
	}

	DBGPRINT(RT_DEBUG_TRACE, ("COEX: LMthreshold = %d, MHthreshold = %d\n",LMthreshold,MHthreshold));
	if (pAd->coexRXManualAGCGain.bStopAGC)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("COEX: Stop AGC "));
	}
	else
	{
		
		if (rssi > LMthreshold)
		{
			RTMP_BBP_IO_READ32(pAd, bbp_reg, &bbp_val);
			BTCOEX_BB_BITWISE_WRITE(bbp_val, (BIT6+BIT7), BIT6); 
			RTMP_BBP_IO_WRITE32(pAd, bbp_reg, bbp_val);
		}
		else if ((rssi <= LMthreshold) &&(rssi > MHthreshold))
		{
			RTMP_BBP_IO_READ32(pAd, bbp_reg, &bbp_val);
			BTCOEX_BB_BITWISE_WRITE(bbp_val, (BIT6+BIT7), BIT7); 
			RTMP_BBP_IO_WRITE32(pAd, bbp_reg, bbp_val);
		}
		else
		{
			RTMP_BBP_IO_READ32(pAd, bbp_reg, &bbp_val);
			BTCOEX_BB_BITWISE_WRITE(bbp_val, (BIT6+BIT7), (BIT6+BIT7)); 
			RTMP_BBP_IO_WRITE32(pAd, bbp_reg, bbp_val);
		}
	}
#endif
}


VOID CoexTDDRXAGCGain(
	IN PRTMP_ADAPTER pAd 
	) 
{
	UCHAR idx = 0;
	UINT32 bbp_val, bbp_reg = AGC1_R8;

		if (pAd->coexRXManualAGCGain.bStopAGC)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("COEX: Stop AGC "));
		}
		else
		{
		RTMP_BBP_IO_READ32(pAd, bbp_reg, &bbp_val);
		BTCOEX_BB_BITWISE_WRITE(bbp_val, (BIT6+BIT7), (BIT6+BIT7)); 
		RTMP_BBP_IO_WRITE32(pAd, bbp_reg, bbp_val);
	}
}


VOID TDDFDDCoexBACapability( 
 IN PRTMP_ADAPTER pAd,
 UCHAR CoexMode )
{
	//PRTMP_PORT	pPort = RTMPGetActivePort(pAd);
	MAC_TABLE_ENTRY *pEntry;
	pEntry = &pAd->MacTab.Content[BSSID_WCID];

	if (!(IS_MT7650(pAd) || IS_MT7630(pAd) || IS_MT76x2(pAd)))
	{
		return;
	}

	if (!(INFRA_ON(pAd)))
		return;
	
	if (CoexMode == COEX_MODE_TDD)
	{
		if (BT_STATUS_TEST_FLAG(pAd, fBTSTATUS_BT_BWL))
		{
			// Update BAWinLimit
			if (pAd->CommonCfg.BACapability.field.RxBAWinLimit != COEX_BARXSIZE_A2DP)
			{
				pAd->CommonCfg.BACapability.field.RxBAWinLimit = COEX_BARXSIZE_A2DP;
				//pAd->CommonCfg.BACapability.field.TxBAWinLimit = 0x40;
				
				//pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;	
	
				//BATableTearRECEntry(pAd, pPort, 0, BSSID_WCID,TRUE);
				//BATableTearORIEntry(pAd, pPort, 0, BSSID_WCID, TRUE, TRUE);
				BAOriSessionTearDown(pAd, BSSID_WCID, 0, FALSE, FALSE);
				BARecSessionTearDown(pAd, BSSID_WCID, 0, FALSE);
				//expect to build BA
				DBGPRINT(RT_DEBUG_TRACE, ("COEX: TDD mode: Set RxBASize to %d\n", pAd->CoexMode.RxBAWinLimit));
				BAOriSessionSetUp(pAd, pEntry, 0, 0, 100, TRUE);
			}
		}
		else if (BT_STATUS_TEST_FLAG(pAd, fBTSTATUS_BT_SYNC))
		{
			// Update BAWinLimit
			if (pAd->CommonCfg.BACapability.field.RxBAWinLimit != COEX_BARXSIZE_SCO_ESCO)
			{
				pAd->CommonCfg.BACapability.field.RxBAWinLimit = COEX_BARXSIZE_SCO_ESCO;
				//pAd->CommonCfg.BACapability.field.TxBAWinLimit = 0x40;
				
				//pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;	

				//BATableTearRECEntry(pAd, pPort, 0, BSSID_WCID,TRUE);
				//BATableTearORIEntry(pAd, pPort, 0, BSSID_WCID, TRUE, TRUE);
				BAOriSessionTearDown(pAd, BSSID_WCID, 0, FALSE, FALSE);
				BARecSessionTearDown(pAd, BSSID_WCID, 0, FALSE);
				//expect to build BA
				DBGPRINT(RT_DEBUG_TRACE, ("COEX: TDD mode: Set RxBASize to %d\n", pAd->CoexMode.RxBAWinLimit));
				BAOriSessionSetUp(pAd, pEntry, 0, 0, 100, TRUE);	
			}
		}
		else 
		{
			// Update BAWinLimit
			if (pAd->CommonCfg.BACapability.field.RxBAWinLimit != COEX_BARXSIZE_OPP)
			{
				pAd->CommonCfg.BACapability.field.RxBAWinLimit = COEX_BARXSIZE_OPP;
				//pAd->CommonCfg.BACapability.field.TxBAWinLimit = 0x40;
				
				//pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;	

				//BATableTearRECEntry(pAd, pPort, 0, BSSID_WCID,TRUE);
				//BATableTearORIEntry(pAd, pPort, 0, BSSID_WCID, TRUE, TRUE);
				BAOriSessionTearDown(pAd, BSSID_WCID, 0, FALSE, FALSE);
				BARecSessionTearDown(pAd, BSSID_WCID, 0, FALSE);
				//expect to build BA
				DBGPRINT(RT_DEBUG_TRACE, ("COEX: TDD mode: Set RxBASize to %d\n", pAd->CoexMode.RxBAWinLimit));
				BAOriSessionSetUp(pAd, pEntry, 0, 0, 100, TRUE);	
			}
		}
		// Need to consider 5G cases
	}
	else if  (CoexMode == COEX_MODE_FDD || CoexMode == COEX_MODE_RESET)
	{
		// Update BAWinLimit
		if (pAd->CommonCfg.BACapability.field.RxBAWinLimit != pAd->CommonCfg.REGBACapability.field.RxBAWinLimit )
		{		
			pAd->CommonCfg.BACapability.field.RxBAWinLimit = pAd->CommonCfg.REGBACapability.field.RxBAWinLimit;
			pAd->CommonCfg.BACapability.field.TxBAWinLimit = pAd->CommonCfg.REGBACapability.field.TxBAWinLimit;
			
			//pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;	

			//BATableTearRECEntry(pAd, pPort, 0, BSSID_WCID,TRUE);
			//BATableTearORIEntry(pAd, pPort, 0, BSSID_WCID, TRUE, TRUE);
			BAOriSessionTearDown(pAd, BSSID_WCID, 0, FALSE, FALSE);
			BARecSessionTearDown(pAd, BSSID_WCID, 0, FALSE);
			DBGPRINT(RT_DEBUG_TRACE, ("COEX: FDD (Reset)mode: Set RxBASize to %d\n", pAd->CoexMode.RxBAWinLimit));
			//expect to build BA
			BAOriSessionSetUp(pAd, pEntry, 0, 0, 100, TRUE);
		}
	}
}


VOID TDDFDDExclusiveRequest(
		IN PRTMP_ADAPTER pAd, 
	UCHAR CoexMode 
	) 
{
	if (CoexMode == COEX_MODE_FDD)
	{
		pAd->CoexMode.FDDRequest = TRUE;
		pAd->CoexMode.TDDRequest = FALSE;
	}
	else if  (CoexMode == COEX_MODE_TDD)
	{
		pAd->CoexMode.FDDRequest = FALSE;
		pAd->CoexMode.TDDRequest = TRUE;
	}
	else if (CoexMode == COEX_MODE_RESET)
	{
		pAd->CoexMode.FDDRequest = FALSE;
		pAd->CoexMode.TDDRequest = FALSE;
	}
	
}


VOID SendAndesTFSWITCH(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR			CoexMode
	)
{
	
	COEX_TF_SWITCH coexTF = {0};
	USHORT coexTFLength = 0;
	INT ret;
	//struct CMD_UNIT CmdUnit;
	struct cmd_msg *msg;
	printk("%s: CoexMode=%d-->\n", __FUNCTION__, CoexMode);
	coexTF.CoexOperation = TypeTFSwitch;
	   coexTF.CoexMode = CoexMode;
	   
	coexTFLength = sizeof(coexTF);

	msg = AndesAllocCmdMsg(pAd, coexTFLength);
	AndesInitCmdMsg(msg, PKT_CMD_TYPE_COEX_OP, FALSE, 0, TRUE, FALSE, 0, NULL, NULL);

	AndesAppendCmdMsg(msg, (char *)&coexTF,coexTFLength);
	
	ret = AndesSendCmdMsg(pAd, msg);
		
	DBGPRINT(RT_DEBUG_OFF, ("%s: CoexOperation = %d, CoexMode = %d\n, PktLength = %d\n", 
		__FUNCTION__, 
		coexTF.CoexOperation, 
		coexTF.CoexMode,
		coexTFLength
		));
	
	TDDFDDExclusiveRequest(pAd, CoexMode);
	printk("%s: <--\n", __FUNCTION__);
	   
	
}


VOID InitBTCoexistence(
	IN PRTMP_ADAPTER pAd)
{	
	if(pAd == NULL)
	{
		return;
	}

	if (IS_MT7650(pAd)  || IS_MT7630(pAd) || IS_MT76x2(pAd))
	{
		pAd->CommonCfg.CoexFDDRSSITreshold = 70;
		pAd->CommonCfg.CoexTDDRSSITreshold = 76;
		pAd ->CommonCfg.CoexRXAGCLMTreshold = 51;
		pAd ->CommonCfg.CoexRXAGCMHTreshold = 66;
		pAd ->CommonCfg.CoexWLANPrivilegeTime = 0x50000044;
		//Coex paramter
		pAd->CommonCfg.CoexDefaultMode = COEX_MODE_TDD;
		if (pAd->CommonCfg.CoexDefaultMode & 0xF0000000)
		{
			pAd->CoexMode.DefaultMode = (pAd->CommonCfg.CoexDefaultMode & 0x0FFFFFFF);
		}
		else
		{
			pAd->CoexMode.DefaultMode = pAd->CommonCfg.CoexDefaultMode;
		}

		printk("==>InitBTCoexistence CoexMode.DefaultMode(%d)\n",pAd->CoexMode.DefaultMode);
		pAd->CoexMode.CurrentMode = pAd->CoexMode.DefaultMode;
		pAd->CoexMode.CoexFDDRSSITreshold = -pAd->CommonCfg.CoexFDDRSSITreshold;
		pAd->CoexMode.CoexTDDRSSITreshold = -pAd->CommonCfg.CoexTDDRSSITreshold;
		pAd->coexRXManualAGCGain.LMthreshold = -pAd ->CommonCfg.CoexRXAGCLMTreshold;
		pAd->coexRXManualAGCGain.MHthreshold = -pAd ->CommonCfg.CoexRXAGCMHTreshold;

	}
}



VOID MLMEHook(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		WlanStatus,
	IN ULONG			  BssHashID
	) 
	{
	ULONG unit = ((pAd->CommonCfg.CoexWLANPrivilegeTime & 0xF0000000)>>28)*100; //5ms*100 = 500
	ULONG times = 0;
   
	if (!(IS_MT7650(pAd)  || IS_MT7630(pAd) || IS_MT7662(pAd) || IS_MT7632(pAd)))
	{
		return;
	}
	DBGPRINT(RT_DEBUG_TRACE, ("COEX: CoexWLANPrivilegeTime = %x\n",  pAd->CommonCfg.CoexWLANPrivilegeTime));
		DBGPRINT(RT_DEBUG_TRACE, ("COEX: unit = %x\n",  unit));

	switch (WlanStatus)
	{
		case WLAN_Device_ON :
			SendAndesWLANStatus(pAd, WlanStatus, 0, 0);		
			break;

		case WLAN_CONNECTION_START :
			times = (pAd->CommonCfg.CoexWLANPrivilegeTime & 0xF0)>>4;
			SendAndesWLANStatus(pAd, WlanStatus, unit*times, BssHashID);
			break;		
#if 0			
		case BT_HCI_Create_Physical_Link :
		case BT_MAC_Start_Completed :
			times = pAd->CommonCfg.CoexWLANPrivilegeTime & 0xF;
			if (pAd->BTMode == BTMODE_RESPONDER)
			{
				SendAndesWLANStatus(pAd, WlanStatus, unit*times, BssHashID);
			}
			else if  (pAd->BTMode == BTMODE_INITIATOR)
			{
				SendAndesWLANStatus(pAd, WlanStatus, 0, BssHashID);
			}
			 break;
  #endif				   
			
		default:
			SendAndesWLANStatus(pAd, WlanStatus, 0, BssHashID);
	}

 }

//
//COEX periodic checking
//
VOID CoexistencePeriodicRoutine(
		IN PRTMP_ADAPTER pAd
		)
{
	WLAN_FUN_INFO_STRUC wlanFunInfo = {.word = 0};
	RTMP_IO_READ32(pAd, WLAN_FUN_INFO, &wlanFunInfo.word);
				  
	//DBGPRINT(RT_DEBUG_TRACE,("COEX: StaCfg.AvgRssi0 = %d\n",pAd->StaCfg.RssiSample.AvgRssi0));
	//DBGPRINT(RT_DEBUG_TRACE,("COEX:CoexMode = %d\n",pAd->CoexMode.CurrentMode));

	//Process Block Ack for aggregation
	if (IS_MT76XXBTCOMBO(pAd) && (BT_STATUS_TEST_FLAG(pAd,fBTSTATUS_BT_ACTIVE)))
	{
		if (pAd->CoexMode.CurrentMode == COEX_MODE_FDD)
		{
			TDDFDDCoexBACapability(pAd, COEX_MODE_FDD);
		}
		else if (pAd->CoexMode.CurrentMode == COEX_MODE_TDD)
		{
			TDDFDDCoexBACapability(pAd, COEX_MODE_TDD);
		}
		else 
		{
			DBGPRINT(RT_DEBUG_ERROR,("!!COEX:Strange CoexMode = %d\n",pAd->CoexMode.CurrentMode));
		}
	}
	else if ((IS_MT7650(pAd) || IS_MT7630(pAd) || IS_MT76x2(pAd)))
	{
		TDDFDDCoexBACapability(pAd, COEX_MODE_RESET);
	}

	//Process TDD/FDD CR
	if ((IS_MT7650(pAd) || IS_MT7630(pAd) || IS_MT76x2(pAd))  && INFRA_ON(pAd))
	{	
		DBGPRINT(RT_DEBUG_INFO,("COEX: pAd->CoexMode.CoexTDDRSSITreshold = %d\n",pAd->CoexMode.CoexTDDRSSITreshold));
		DBGPRINT(RT_DEBUG_INFO,("COEX: pAd->CoexMode.CoexFDDRSSITreshold = %d\n",pAd->CoexMode.CoexFDDRSSITreshold));
		//MT76xx coex RX LNA gain adjustment
		if (pAd->CoexMode.CurrentMode == COEX_MODE_FDD)
		{
			if (pAd->StaCfg.RssiSample.AvgRssi0 < (pAd->CoexMode.CoexTDDRSSITreshold) )
			{
				if (pAd->CoexMode.TDDRequest == FALSE)
				{
					//SendAndesTFSWITCH(pAd, COEX_MODE_TDD);
				}
				
			}
			if (pAd->StaCfg.RssiSample.AvgRssi0 > (pAd->CoexMode.CoexFDDRSSITreshold) )
			{
				if (pAd->CoexMode.FDDRequest == FALSE)
				{
					//SendAndesTFSWITCH(pAd, COEX_MODE_FDD);
				}
				
			}				   
			CoexFDDRXAGCGain(pAd, pAd->StaCfg.RssiSample.AvgRssi0);
		}
		else if (pAd->CoexMode.CurrentMode == COEX_MODE_TDD)
		{
			if (pAd->StaCfg.RssiSample.AvgRssi0 > (pAd->CoexMode.CoexFDDRSSITreshold) )
			{
				if (pAd->CoexMode.FDDRequest == FALSE)
				{
					/* 20130523-sarick
					   Do not set to FDD now. */
					//SendAndesTFSWITCH(pAd, COEX_MODE_FDD);
				}
			}
			if (pAd->StaCfg.RssiSample.AvgRssi0 < (pAd->CoexMode.CoexTDDRSSITreshold) )
			{
				if (pAd->CoexMode.TDDRequest == FALSE)
				{
					//SendAndesTFSWITCH(pAd, COEX_MODE_TDD);
				}
			}
			CoexTDDRXAGCGain(pAd); 
		}
	}

}


VOID COEXLinkDown(
	IN PRTMP_ADAPTER pAd,
	PUCHAR  pAddr
)
{
	//Coex release protection frame
	InvalidProtectionFrameSpace(pAd, pAddr);
	UpdateAndesNullFrameSpace(pAd);
	RemoveProtectionFrameSpace(pAd, pAddr);
	TDDFDDExclusiveRequest(pAd,COEX_MODE_RESET);
	{
		BtAFHCtl(pAd, pAd->CommonCfg.BBPCurrentBW, pAd->CommonCfg.Channel, pAd->CommonCfg.CentralChannel, TRUE);		
		SendAndesAFH(pAd, pAd->CommonCfg.BBPCurrentBW,  pAd->CommonCfg.Channel, pAd->CommonCfg.CentralChannel, TRUE, QueryHashID(pAd, pAddr, FALSE));
	}

}


VOID CoexParseBTStatus(
	IN PRTMP_ADAPTER pAd
	) 
{
	PBT_STATUS_REPORT_STRUC pBtstatus =(PBT_STATUS_REPORT_STRUC) &(pAd->BTStatusFlags);

	if (BT_STATUS_TEST_FLAG(pAd,fBTSTATUS_BT_ACTIVE))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("COEX: BT ACTIVE !!!! \n"));
	}
	if  (BT_STATUS_TEST_FLAG(pAd,fBTSTATUS_BT_BWL))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("COEX:  bandwidth-limited link such as A2DP\n"));		
	}
	if  (BT_STATUS_TEST_FLAG(pAd,fBTSTATUS_BT_BE))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("COEX: best effort to serve non-QoS-guaranteed link such as PAN, FTP\n"));
	}
	if  (BT_STATUS_TEST_FLAG(pAd,fBTSTATUS_BT_SYNC))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("COEX: synchronous link(s) such as SCO, eSCO\n"));		
	}
	if  (BT_STATUS_TEST_FLAG(pAd,fBTSTATUS_BT_SNIFF))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("COEX: periodic transport(s) such as SNIFF, LE_CONN, LE_ADV\n"));		
	}
	if  (BT_STATUS_TEST_FLAG(pAd,fBTSTATUS_BT_SCATTER))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("COEX: scatter operation either PAGE or INQUIRY\n"));   
	}

	DBGPRINT(RT_DEBUG_TRACE, ("COEX: BT_LINK_CNT = %d, BTE_LINK_CNT= %d, TotalCount = %d, BT_SLAVE_IND = %d\n",
						pBtstatus->field.BT_LINK_CNT,
						pBtstatus->field.BLE_LINK_CNT,
						pBtstatus->field.BT_LINK_CNT + pBtstatus->field.BLE_LINK_CNT,
						pBtstatus->field.BT_SLAVE_IND));	  

}

#endif /* MT76XX_BTCOEX_SUPPORT */
