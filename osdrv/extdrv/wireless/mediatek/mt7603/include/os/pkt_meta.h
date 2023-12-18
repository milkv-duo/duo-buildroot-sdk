/*

*/

#define MIN_NET_DEVICE_FOR_MBSSID		0x00		/*0x00,0x10,0x20,0x30 */
#define MIN_NET_DEVICE_FOR_WDS			0x10		/*0x40,0x50,0x60,0x70 */
#define MIN_NET_DEVICE_FOR_APCLI		0x20
#define MIN_NET_DEVICE_FOR_MESH			0x30
#ifdef CONFIG_STA_SUPPORT
#define MIN_NET_DEVICE_FOR_DLS			0x40
#define MIN_NET_DEVICE_FOR_TDLS			0x50
#ifdef MESH_SUPPORT
#undef MIN_NET_DEVICE_FOR_DLS
#undef MIN_NET_DEVICE_FOR_TDLS
#define MIN_NET_DEVICE_FOR_DLS			(MIN_NET_DEVICE_FOR_MESH + 0x10)
#define MIN_NET_DEVICE_FOR_TDLS			(MIN_NET_DEVICE_FOR_MESH + 0x20)
#endif /* MESH_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
#ifdef P2P_SUPPORT
#define MIN_NET_DEVICE_FOR_P2P_CLI		(MIN_NET_DEVICE_FOR_TDLS + 0x10)
#define MIN_NET_DEVICE_FOR_P2P_GO			(MIN_NET_DEVICE_FOR_TDLS + 0x20)
#endif /* P2P_SUPPORT */

#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
#define MIN_NET_DEVICE_FOR_CFG80211_VIF_P2P_CLI      (MIN_NET_DEVICE_FOR_TDLS + 0x21)
#define MIN_NET_DEVICE_FOR_CFG80211_VIF_P2P_GO       (MIN_NET_DEVICE_FOR_TDLS + 0x22)
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */

#define NET_DEVICE_REAL_IDX_MASK		0x0f		/* for each operation mode, we maximum support 15 entities. */


/******************************************************************************
	Packet Meta info fields
	NOTE: the meta info may stored in some os-dependent packet buffer and may
			have extra limitations.
		1. Linux:
			a).re-use sk_budd->cb[]
			b).Max space of skb->cb = 48B = [CB_OFF+38]
		2. VxWorks:
			a).
			b).Max space: 32!!
******************************************************************************/
/* [CB_OFF + 0]  */
/* User Priority */
#define RTMP_SET_PACKET_UP(_p, _prio)			(PACKET_CB(_p, 0) = _prio)
#define RTMP_GET_PACKET_UP(_p)					(PACKET_CB(_p, 0))

/* [CB_OFF + 1]  */
/* Fragment # */
#define RTMP_SET_PACKET_FRAGMENTS(_p, _num)	(PACKET_CB(_p, 1) = _num)
#define RTMP_GET_PACKET_FRAGMENTS(_p)		(PACKET_CB(_p, 1))

/* [CB_OFF + 2]  */
/*
	0x0 ~ 0xff: index mapping to MAC_TABLE_ENTRY
				for Tx, search by eth_hdr.addr1
				for Rx, get from ASIC(RxWI)/search by wlan_hdr.addr2
*/
#define RTMP_SET_PACKET_WCID(_p, _wdsidx)		(PACKET_CB(_p, 2) = _wdsidx)
#define RTMP_GET_PACKET_WCID(_p)				(PACKET_CB(_p, 2))


/* [CB_OFF + 3]  */
/* From which Interface, wdev index */
#define RTMP_SET_PACKET_WDEV(_p, _wdev_idx)		(PACKET_CB(_p, 3) = _wdev_idx)
#define RTMP_GET_PACKET_WDEV(_p)					(PACKET_CB(_p, 3))

#if 0
#define RTMP_SET_PACKET_NET_DEVICE_APCLI(_p, _idx)	\
			RTMP_SET_PACKET_IF((_p), ((_idx) + MIN_NET_DEVICE_FOR_APCLI))
#define RTMP_SET_PACKET_NET_DEVICE_P2P(_p, _idx) \
			RTMP_SET_PACKET_IF((_p), ((_idx) + MIN_NET_DEVICE_FOR_P2P_GO))
#endif


/* [CB_OFF + 4]  */
/* If this flag is set, it indicates that this EAPoL frame MUST be clear. */
#define RTMP_SET_PACKET_CLEAR_EAP_FRAME(_p, _flg)   (PACKET_CB(_p, 4) = _flg)
#define RTMP_GET_PACKET_CLEAR_EAP_FRAME(_p)         (PACKET_CB(_p, 4))


/* [CB_OFF + 5]  */
#define RTMP_SET_PACKET_MOREDATA(_p, _morebit)		(PACKET_CB(_p, 5) = _morebit)
#define RTMP_GET_PACKET_MOREDATA(_p)					(PACKET_CB(_p, 5))


/* [CB_OFF + 6]  */
/*
	Sepcific Pakcet Type definition
*/
#define RTMP_PACKET_SPECIFIC_CB_OFFSET	6

#define RTMP_PACKET_SPECIFIC_DHCP		0x01
#define RTMP_PACKET_SPECIFIC_EAPOL	0x02
#define RTMP_PACKET_SPECIFIC_IPV4		0x04
#define RTMP_PACKET_SPECIFIC_WAI		0x08
#define RTMP_PACKET_SPECIFIC_VLAN		0x10
#define RTMP_PACKET_SPECIFIC_LLCSNAP	0x20
#define RTMP_PACKET_SPECIFIC_TDLS		0x40
#define RTMP_PACKET_SPECIFIC_DISASSOC	0x80

/* Specific */
#define RTMP_SET_PACKET_SPECIFIC(_p, _flg)	   	(PACKET_CB(_p, 6) = _flg)

/* DHCP */
#define RTMP_SET_PACKET_DHCP(_p, _flg)   								\
			do{															\
				if (_flg)												\
					PACKET_CB(_p, 6) |= (RTMP_PACKET_SPECIFIC_DHCP);	\
				else													\
					PACKET_CB(_p, 6) &= (~RTMP_PACKET_SPECIFIC_DHCP);	\
			}while(0)
#define RTMP_GET_PACKET_DHCP(_p)\
			(PACKET_CB(_p, 6) & RTMP_PACKET_SPECIFIC_DHCP)

/* EAPOL */
#define RTMP_SET_PACKET_EAPOL(_p, _flg)   								\
			do{															\
				if (_flg)												\
					PACKET_CB(_p, 6) |= (RTMP_PACKET_SPECIFIC_EAPOL);	\
				else													\
					PACKET_CB(_p, 6) &= (~RTMP_PACKET_SPECIFIC_EAPOL);	\
			}while(0)
#define RTMP_GET_PACKET_EAPOL(_p)\
			(PACKET_CB(_p, 6) & RTMP_PACKET_SPECIFIC_EAPOL)

/* WAI */
#define RTMP_SET_PACKET_WAI(_p, _flg)   								\
			do{															\
				if (_flg)												\
					PACKET_CB(_p, 6) |= (RTMP_PACKET_SPECIFIC_WAI);	\
				else													\
					PACKET_CB(_p, 6) &= (~RTMP_PACKET_SPECIFIC_WAI);	\
			}while(0)
#define RTMP_GET_PACKET_WAI(_p) \
			(PACKET_CB(_p, 6) & RTMP_PACKET_SPECIFIC_WAI)

#define RTMP_GET_PACKET_FORCE_TX(_p)\
		(PACKET_CB(_p, 6) & (RTMP_PACKET_SPECIFIC_EAPOL | \
							RTMP_PACKET_SPECIFIC_WAI))

#define RTMP_GET_PACKET_LOWRATE(_p) \
			(PACKET_CB(_p, 6) & (RTMP_PACKET_SPECIFIC_EAPOL |\
								RTMP_PACKET_SPECIFIC_DHCP |\
								RTMP_PACKET_SPECIFIC_WAI))

/* VLAN */
#define RTMP_SET_PACKET_VLAN(_p, _flg)   								\
			do{															\
				if (_flg)												\
					PACKET_CB(_p, 6) |= (RTMP_PACKET_SPECIFIC_VLAN);	\
				else													\
					PACKET_CB(_p, 6) &= (~RTMP_PACKET_SPECIFIC_VLAN);	\
			}while(0)
#define RTMP_GET_PACKET_VLAN(_p) \
			(PACKET_CB(_p, 6) & RTMP_PACKET_SPECIFIC_VLAN)

/* LLC/SNAP */
#define RTMP_SET_PACKET_LLCSNAP(_p, _flg)   								\
			do{																\
				if (_flg)													\
					PACKET_CB(_p, 6) |= (RTMP_PACKET_SPECIFIC_LLCSNAP);	\
				else														\
					PACKET_CB(_p, 6) &= (~RTMP_PACKET_SPECIFIC_LLCSNAP);	\
			}while(0)
			
#define RTMP_GET_PACKET_LLCSNAP(_p) \
			(PACKET_CB(_p, 6) & RTMP_PACKET_SPECIFIC_LLCSNAP)

/* IP */
#define RTMP_SET_PACKET_IPV4(_p, _flg)									\
			do{															\
				if (_flg)												\
					PACKET_CB(_p, 6) |= (RTMP_PACKET_SPECIFIC_IPV4);	\
				else													\
					PACKET_CB(_p, 6) &= (~RTMP_PACKET_SPECIFIC_IPV4);	\
			}while(0)
			
#define RTMP_GET_PACKET_IPV4(_p) \
			(PACKET_CB(_p, 6) & RTMP_PACKET_SPECIFIC_IPV4)

/* TDLS */
#define RTMP_SET_PACKET_TDLS_MMPDU(_p, _flg)									\
			do{															\
				if (_flg)												\
					PACKET_CB(_p, 6) |= (RTMP_PACKET_SPECIFIC_TDLS);	\
				else														\
					PACKET_CB(_p, 6) &= (~RTMP_PACKET_SPECIFIC_TDLS);	\
			}while(0)
			
#define RTMP_GET_PACKET_TDLS_MMPDU(_p) \
			(PACKET_CB(_p, 6) & RTMP_PACKET_SPECIFIC_TDLS)

/* DISASSOC */
#define RTMP_SET_PACKET_DISASSOC(_p, _flg)                                                      \
            do{                                                                             \
                if (_flg)                                                                   \
                    (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_DISASSOC);     \
                else                                                                        \
                    (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) &= (~RTMP_PACKET_SPECIFIC_DISASSOC);    \
            }while(0)
#define RTMP_GET_PACKET_DISASSOC(_p)   \
			(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_DISASSOC)


/* [CB_OFF + 7]  */
#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_CONCURRENT_DEVICE) || defined(CFG80211_MULTI_STA)
#define RTMP_SET_PACKET_OPMODE(_p, _flg)   (PACKET_CB(_p, 7) = _flg)
#define RTMP_GET_PACKET_OPMODE(_p)         (PACKET_CB(_p, 7))
#endif /* P2P_SUPPORT  || RT_CFG80211_P2P_CONCURRENT_DEVICE || CFG80211_MULTI_STA */


/* [CB_OFF + 8 ~ 9]  */
#ifdef UAPSD_SUPPORT
/* if we queue a U-APSD packet to any software queue, we will set the U-APSD
   flag and its physical queue ID for it */
#define RTMP_SET_PACKET_UAPSD(_p, _flg_uapsd, _que_id) \
                    (PACKET_CB(_p, 8) = ((_flg_uapsd<<7) | _que_id))

#define RTMP_SET_PACKET_QOS_NULL(_p)			(PACKET_CB(_p, 8) = 0xff)
#define RTMP_GET_PACKET_QOS_NULL(_p)			(PACKET_CB(_p, 8))
#define RTMP_SET_PACKET_NON_QOS_NULL(_p)		(PACKET_CB(_p, 8) = 0x00)
#define RTMP_GET_PACKET_UAPSD_Flag(_p)		((PACKET_CB(_p, 8) & 0x80) >> 7)
#define RTMP_GET_PACKET_UAPSD_QUE_ID(_p)		(PACKET_CB(_p, 8) & 0x7f)

#define RTMP_SET_PACKET_EOSP(_p, _flg)			(PACKET_CB(_p, 9) = _flg)
#define RTMP_GET_PACKET_EOSP(_p)				(PACKET_CB(_p, 9))
#endif /* UAPSD_SUPPORT */


/* [CB_OFF + 10 ~ 11]  */
#define RTMP_SET_PACKET_PROTOCOL(_p, _protocol) {\
	(PACKET_CB(_p, 10) = (UINT8)((_protocol) & 0x00ff)); \
	(PACKET_CB(_p, 11) = (UINT8)(((_protocol) & 0xff00) >> 8)); \
}

#define RTMP_GET_PACKET_PROTOCOL(_p) \
	((((UINT16)PACKET_CB(_p, 11)) << 8) \
	| ((UINT16)PACKET_CB(_p, 10)))


/* [CB_OFF + 12]  */
#define RTMP_SET_PACKET_TXTYPE(_p, _mode)		(PACKET_CB(_p, 12) = _mode)
#define RTMP_GET_PACKET_TXTYPE(_p)			(PACKET_CB(_p, 12))

/* [CB_OFF + 13]  */
/* TX rate index */
#define RTMP_SET_PACKET_TXRATE(_p, _rate)		(PACKET_CB(_p, 13) = _rate)
#define RTMP_GET_PACKET_TXRATE(_p)			(PACKET_CB(_p, 13))


/* [CB_OFF + 14]  */
#ifdef MAC_REPEATER_SUPPORT
#define RTMP_SET_PKT_MAT_FREE(_p, _flg)	(PACKET_CB(_p, 14) = (_flg))
#define RTMP_GET_PKT_MAT_FREE(_p)			(PACKET_CB(_p, 14))
#endif /* MAC_REPEATER_SUPPORT */


/* [CB_OFF + 15 ~ 19]  */
#ifdef WMM_ACM_SUPPORT
/* total [CB_OFF+15] ~ [CB_OFF+19] 5B */

/* [CB_OFF+15], 1B (4bit vs 4bit, Packet Type vs UP), are reserved for ACM classification use */
/* reference to ACM_PACKET_INFO in acm_extr.h */
/* used in ACMP_DataPacketQueue() */
#define RTMP_GET_PACKET_ACM_PARAM_INFO(_p)		(&(PACKET_CB(_p, 15)))

/* [CB_OFF+16] (bit0~2, Phy Mode) ~ [CB_OFF+17] (bit0~7, MCS), 2B, are reserved for minimum tx rate use */
/* used in ACMP_PacketPhyModeMCSCheck() */
#define RTMP_SET_PACKET_ACM_MIN_PHY_MODE(_p, _mode)			\
	PACKET_CB(_p, 16) = (PACKET_CB(_p, 16) & 0xF8) | (_mode & 0x07);
#define RTMP_GET_PACKET_ACM_MIN_PHY_MODE(_p)				\
	(PACKET_CB(_p, 16) & 0x07)

#define RTMP_SET_PACKET_ACM_MIN_PHY_MCS(_p, _mcs)	(PACKET_CB(_p, 17) = _mcs)
#define RTMP_GET_PACKET_ACM_MIN_PHY_MCS(_p)			(PACKET_CB(_p, 17))

/* [CB_OFF+16] (bit4~7), are reserved for TSPEC TSID (0~15) use */
/* used in ACMP_MsduClassify() */
#define RTMP_SET_PACKET_STM_TSID(_p, _tsid)		\
	PACKET_CB(_p, 16) = (PACKET_CB(_p, 16) & 0x0F) | ((_tsid & 0x0F) << 4);
#define RTMP_GET_PACKET_STM_TSID(_p)			\
	((PACKET_CB(_p, 16) & 0xF0) >> 4)

/* [CB_OFF+18] ~ [CB_OFF+19], 2B, are reserved for tx time use */
/* used in ACMP_DataPacketQueue() */
#define RTMP_SET_PACKET_TX_TIME(_p, _txtime)	(*(UINT16 *)&PACKET_CB(_p, 18) = _txtime)
#define RTMP_GET_PACKET_TX_TIME(_p)			(*(UINT16 *)&PACKET_CB(_p, 18))
#endif /* WMM_ACM_SUPPORT */

/* use bit3 of cb[CB_OFF+16] */
#define RTMP_SET_PACKET_MGMT_PKT(_p, _flg)	        \
        PACKET_CB(_p, 16) = (PACKET_CB(_p, 16) & 0xF7) | ((_flg & 0x01) << 3);
#define RTMP_GET_PACKET_MGMT_PKT(_p)				\
		((PACKET_CB(_p, 16) & 0x08) >> 3)

/* use bit0 of cb[CB_OFF+20] */
#define RTMP_SET_PACKET_MGMT_PKT_DATA_QUE(_p, _flg)	\
        PACKET_CB(_p, 20) = (PACKET_CB(_p, 20) & 0xFE) | (_flg & 0x01);
#define RTMP_GET_PACKET_MGMT_PKT_DATA_QUE(_p)		\
		(PACKET_CB(_p, 20) & 0x01)


/* [CB_OFF+21 ~ 22]  */
#ifdef MESH_SUPPORT
#define RTMP_SET_MESH_ROUTE_ID(_p, _flg)	(PACKET_CB(_p, 21) = _flg)
#define RTMP_GET_MESH_ROUTE_ID(_p)		(PACKET_CB(_p, 21))
#define RTMP_SET_MESH_SOURCE(_p, _flg)		(PACKET_CB(_p, 22) = _flg)
#define RTMP_GET_MESH_SOURCE(_p)			(PACKET_CB(_p, 22))
#endif /* MESH_SUPPORT */


/* [CB_OFF + 23]  */
/*
 *	TDLS Sepcific Pakcet Type definition
*/
#define RTMP_TDLS_SPECIFIC_WAIT_ACK		0x01
#define RTMP_TDLS_SPECIFIC_NOACK			0x02
#define RTMP_TDLS_SPECIFIC_PKTQ_HCCA		0x04
#define RTMP_TDLS_SPECIFIC_PKTQ_EDCA		0x08

#define RTMP_SET_PACKET_TDLS_WAIT_ACK(_p, _flg)						\
			do{															\
				if (_flg)													\
					PACKET_CB(_p, 23) |= (RTMP_TDLS_SPECIFIC_WAIT_ACK);	\
				else														\
					PACKET_CB(_p, 23) &= (~RTMP_TDLS_SPECIFIC_WAIT_ACK);	\
			}while(0)
			
#define RTMP_GET_PACKET_TDLS_WAIT_ACK(_p)		(PACKET_CB(_p, 23) & RTMP_TDLS_SPECIFIC_WAIT_ACK)

#define RTMP_SET_PACKET_TDLS_NO_ACK(_p, _flg)						\
			do{														\
				if (_flg)												\
					PACKET_CB(_p, 23) |= (RTMP_TDLS_SPECIFIC_NOACK);	\
				else													\
					PACKET_CB(_p, 23) &= (~RTMP_TDLS_SPECIFIC_NOACK);	\
			}while(0)
			
#define RTMP_GET_PACKET_TDLS_NO_ACK(_p)		(PACKET_CB(_p, 23) & RTMP_TDLS_SPECIFIC_NOACK)

#define RTMP_SET_TDLS_SPECIFIC_PACKET(_p, _flg)   (PACKET_CB(_p, 23) = _flg)
#define RTMP_GET_TDLS_SPECIFIC_PACKET(_p)         (PACKET_CB(_p, 23))


/* [CB_OFF + 24]  */
#ifdef INF_AMAZON_SE
/* [CB_OFF+25], 1B, Iverson patch for WMM A5-T07 ,WirelessStaToWirelessSta do not bulk out aggregate */
#define RTMP_SET_PACKET_NOBULKOUT(_p, _morebit)			(PACKET_CB(_p, 24) = _morebit)
#define RTMP_GET_PACKET_NOBULKOUT(_p)					(PACKET_CB(_p, 24))
#endif /* INF_AMAZON_SE */
/* Max skb->cb = 48B = [CB_OFF+38] */


/* [CB_OFF + 25]  */
#if 0 //def DOT11V_WNM_SUPPORT
#define RTMP_SET_WNM_DMS(_p, _flg)   (PACKET_CB(_p, 25) = _flg)
#define RTMP_GET_WNM_DMS(_p)         (PACKET_CB(_p, 25))
#endif /* DOT11V_WNM_SUPPORT */


/* [CB_OFF + 26]  */
#ifdef CONFIG_CSO_SUPPORT
#define RTMP_SET_TCP_CHKSUM_FAIL(_p, _flg) 	(PACKET_CB(_p, 26) = _flg);
#define RTMP_GET_TCP_CHKSUM_FAIL(_p)		(PACKET_CB(_p, 26))
#endif /* CONFIG_CSO_SUPPORT */


/* [CB_OFF + 27 ~ 31]  */
#ifdef CONFIG_TSO_SUPPORT
#define NAF_TYPE_CSO	1<<0
#define NAF_TYPE_TSO	1<<1

#define RTMP_SET_PKT_CSO(_p, _flg)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+27]) |= ((_flg) << 0))
#define RTMP_GET_PKT_CSO(_p)		((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+27]) & NAF_TYPE_CSO)
#define RTMP_SET_PKT_TSO(_p, _flg)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+27]) |= ((_flg)<<1))
#define RTMP_GET_PKT_TSO(_p)		((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+27]) & NAF_TYPE_TSO)
#define RTMP_GET_PKT_NAF_TYPE(_p)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+27]) & (NAF_TYPE_CSO | NAF_TYPE_TSO))

#define RTMP_SET_PKT_TCP(_p, _flg)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+27]) |= ((_flg) << 2))
#define RTMP_GET_PKT_TCP(_p)		((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+27]) & (1<<2))

#define RTMP_SET_PKT_SG(_p, _flg)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+27]) |= ((_flg) << 3))
#define RTMP_GET_PKT_SG(_p)		((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+27]) & (1<<3))

#define RTMP_SET_PKT_TUPS(_p, _tups)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+28]) = (_tups))
#define RTMP_GET_PKT_TUPS(_p)		((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+28]))

#define RTMP_SET_PKT_IPS(_p, _ips)	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+29]) = (_ips))
#define RTMP_GET_PKT_IPS(_p)		((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+29]))

#define RTMP_SET_PKT_MSS(_p, _mss)	\
	do {\
		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+30]) = ((_mss) & 0xff);\
		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+31]) = ((_mss) >> 8);\
	}while(0)

#define RTMP_GET_PKT_MSS(_p)	\
	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+30]) | \
	((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+31]) << 8))
#endif /* CONFIG_TSO_SUPPORT */


/* [CB_OFF + 32]  */
/* RTS/CTS-to-self protection method */
#define RTMP_SET_PACKET_RTS(_p, _num)			(PACKET_CB(_p, 32) = _num)
#define RTMP_GET_PACKET_RTS(_p)				(PACKET_CB(_p, 32))
/* see RTMP_S(G)ET_PACKET_EMACTAB */

/* [CB_OFF + 33]  */
#ifdef CONFIG_5VT_ENHANCE
#define RTMP_SET_PACKET_5VT(_p, _flg)   (PACKET_CB(_p, 33) = _flg)
#define RTMP_GET_PACKET_5VT(_p)         (PACKET_CB(_p, 33))
#endif /* CONFIG_5VT_ENHANCE */

/* [CB_OFF + 34]  */
#ifdef DOT11V_WNM_SUPPORT
#define RTMP_SET_WNM_DMS(_p, _flg)   (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+34] = _flg)
#define RTMP_GET_WNM_DMS(_p)         (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+34])
#endif /* DOT11V_WNM_SUPPORT */

