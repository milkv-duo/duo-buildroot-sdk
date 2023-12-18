# Support ATE function
HAS_ATE=y

# Support 28xx QA ATE function
HAS_QA_SUPPORT=y

# Support WSC function
HAS_WSC=y

#ifdef NINTENDO_AP
HAS_NINTENDO=y
#endif // NINTENDO_AP //

# Support LLTD function
HAS_LLTD=y

# Support WDS function
HAS_WDS=y

# Support AP-Client function
HAS_APCLI=y

# Support Wpa_Supplicant
HAS_WPA_SUPPLICANT=y

# Support Native WpaSupplicant for Network Maganger
HAS_NATIVE_WPA_SUPPLICANT_SUPPORT=y

#Support Net interface block while Tx-Sw queue full
HAS_BLOCK_NET_IF=y

#Support IGMP-Snooping function.
HAS_IGMP_SNOOP_SUPPORT=y

#Support DFS function
HAS_DFS_SUPPORT=y

#Support Carrier-Sense function
HAS_CS_SUPPORT=y

#ifdef ETH_CONVERT
# Support for STA Ethernet Converter
HAS_ETH_CONVERT_SUPPORT=y
#endif // ETH_CONVERT //

# Support user specific transmit rate of Multicast packet.
HAS_MCAST_RATE_SPECIFIC_SUPPORT=y

#ifdef MULTI_CARD
# Support for Multiple Cards
HAS_MC_SUPPORT=y
#endif // MULTI_CARD //

#Support for PCI-MSI
HAS_MSI_SUPPORT=y

#Support for IEEE802.11e DLS
HAS_QOS_DLS_SUPPORT=y

#Support for EXT_CHANNEL
HAS_EXT_BUILD_CHANNEL_LIST=y

#Support for IDS 
HAS_IDS_SUPPORT=y

#Support for MESH
HAS_MESH_SUPPORT=y

#Support for Net-SNMP
HAS_SNMP_SUPPORT=y

#Support features of 802.11n Draft3
HAS_DOT11N_DRAFT3_SUPPORT=y

#Support for 2860/2880 co-exist 
HAS_RT2880_RT2860_COEXIST=n


CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

WFLAGS := -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT  -DVXWORKS

ifeq ($(RT28xx_MODE),AP)
WFLAGS += -DCONFIG_AP_SUPPORT  -DUAPSD_AP_SUPPORT -DMBSS_SUPPORT -DIAPP_SUPPORT -DDBG

ifeq ($(HAS_ATE),y)
WFLAGS += -DCONFIG_ATE
ifeq ($(HAS_QA_SUPPORT),y)
WFLAGS += -DCONFIG_QA
endif
#ifdef GEMTEK_ATE
ifeq ($(RELEASE), DPB)
WFLAGS += -DGEMTEK_ATE
endif
#endif // GEMTEK_ATE //
endif

#ifdef NINTENDO_AP
ifeq ($(HAS_NINTENDO),y)
WFLAGS += -DNINTENDO_AP
endif
#endif // NINTENDO_AP //

ifeq ($(HAS_WSC),y)
WFLAGS += -DWSC_AP_SUPPORT
endif

ifeq ($(HAS_WDS),y)
WFLAGS += -DWDS_SUPPORT
endif

ifeq ($(HAS_APCLI),y)
WFLAGS += -DAPCLI_SUPPORT -DMLME_EX -DMAT_SUPPORT
#ifeq ($(HAS_ETH_CONVERT_SUPPORT), y)
#WFLAGS += -DETH_CONVERT_SUPPORT
#endif 
endif

ifeq ($(HAS_IGMP_SNOOP_SUPPORT),y)
WFLAGS += -DIGMP_SNOOP_SUPPORT
endif

ifeq ($(HAS_CS_SUPPORT),y)
WFLAGS += -DCARRIER_DETECTION_SUPPORT
endif

ifeq ($(HAS_MCAST_RATE_SPECIFIC_SUPPORT), y)
WFLAGS += -DMCAST_RATE_SPECIFIC
endif

ifeq ($(CHIPSET),2860)
ifeq ($(HAS_MSI_SUPPORT),y)
WFLAGS += -DPCI_MSI_SUPPORT
endif
endif

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
WFLAGS += -DQOS_DLS_SUPPORT
endif

ifeq ($(HAS_SNMP_SUPPORT),y)
WFLAGS += -DSNMP_SUPPORT
endif

ifeq ($(HAS_DOT11N_DRAFT3_SUPPORT),y)
WFLAGS += -DDOT11N_DRAFT3
endif

endif #// endif of RT2860_MODE == AP //
#################################################
ifeq ($(RT28xx_MODE),STA)
WFLAGS += -DCONFIG_STA_SUPPORT -DDBG 

ifeq ($(HAS_WPA_SUPPLICANT),y)
WFLAGS += -DWPA_SUPPLICANT_SUPPORT
endif

ifeq ($(HAS_NATIVE_WPA_SUPPLICANT_SUPPORT),y)
WFLAGS += -DNATIVE_WPA_SUPPLICANT_SUPPORT
endif

ifeq ($(HAS_WSC),y)
WFLAGS += -DWSC_STA_SUPPORT
endif

#ifdef ETH_CONVERT
ifeq ($(HAS_ETH_CONVERT_SUPPORT), y)
WFLAGS += -DETH_CONVERT_SUPPORT  -DMAT_SUPPORT
endif
#endif // ETH_CONVERT //

ifeq ($(HAS_ATE),y)
WFLAGS += -DCONFIG_ATE
ifeq ($(HAS_QA_SUPPORT),y)
WFLAGS += -DCONFIG_QA
endif
endif

ifeq ($(HAS_SNMP_SUPPORT),y)
WFLAGS += -DSNMP_SUPPORT
endif

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
WFLAGS += -DQOS_DLS_SUPPORT
endif

endif
# endif of ifeq ($(RT28xx_MODE),STA)
#################################################
ifeq ($(RT28xx_MODE),APSTA)
WFLAGS += -DCONFIG_AP_SUPPORT -DCONFIG_STA_SUPPORT -DCONFIG_APSTA_MIXED_SUPPORT -DUAPSD_AP_SUPPORT -DMBSS_SUPPORT -DIAPP_SUPPORT -DDBG

ifeq ($(HAS_ATE),y)
WFLAGS += -DCONFIG_ATE
ifeq ($(HAS_QA_SUPPORT),y)
WFLAGS += -DCONFIG_QA
endif
#ifdef GEMTEK_ATE
ifeq ($(RELEASE), DPB)
WFLAGS += -DGEMTEK_ATE
endif
#endif // GEMTEK_ATE //
endif

#ifdef NINTENDO_AP
ifeq ($(HAS_NINTENDO),y)
WFLAGS += -DNINTENDO_AP
endif
#endif // NINTENDO_AP //

ifeq ($(HAS_WSC),y)
WFLAGS += -DWSC_AP_SUPPORT -DWSC_STA_SUPPORT
endif

ifeq ($(HAS_WDS),y)
WFLAGS += -DWDS_SUPPORT
endif

ifeq ($(HAS_APCLI),y)
WFLAGS += -DAPCLI_SUPPORT -DMLME_EX -DMAT_SUPPORT
endif

ifeq ($(HAS_IGMP_SNOOP_SUPPORT),y)
WFLAGS += -DIGMP_SNOOP_SUPPORT
endif

ifeq ($(HAS_CS_SUPPORT),y)
WFLAGS += -DCARRIER_DETECTION_SUPPORT
endif

ifeq ($(HAS_MCAST_RATE_SPECIFIC_SUPPORT), y)
WFLAGS += -DMCAST_RATE_SPECIFIC
endif

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
WFLAGS += -DQOS_DLS_SUPPORT
endif

ifeq ($(HAS_WPA_SUPPLICANT),y)
WFLAGS += -DWPA_SUPPLICANT_SUPPORT
endif

ifeq ($(HAS_NATIVE_WPA_SUPPLICANT_SUPPORT),y)
WFLAGS += -DNATIVE_WPA_SUPPLICANT_SUPPORT
endif

#ifdef ETH_CONVERT
ifeq ($(HAS_ETH_CONVERT_SUPPORT), y)
WFLAGS += -DETH_CONVERT_SUPPORT  -DMAT_SUPPORT
endif
#endif // ETH_CONVERT //

endif
# endif of ifeq ($(RT28xx_MODE),APSTA)
#################################################

ifeq ($(HAS_MESH_SUPPORT),y)
WFLAGS += -DMESH_SUPPORT -DMLME_EX -DINTEL_CMPC
endif

ifeq ($(HAS_EXT_BUILD_CHANNEL_LIST),y)
WFLAGS += -DEXT_BUILD_CHANNEL_LIST
endif

ifeq ($(HAS_IDS_SUPPORT),y)
WFLAGS += -DIDS_SUPPORT
endif


#################################################
# ChipSet specific definitions.
#
ifeq ($(CHIPSET),2860)
WFLAGS +=-DRT2860
endif

ifeq ($(CHIPSET),2870)
WFLAGS +=-DRT2870
endif

ifeq ($(CHIPSET),2880)
WFLAGS += -DRT2880 -DRT2860
endif
#################################################


ifeq ($(PLATFORM),5VT)
#WFLAGS += -DCONFIG_5VT_ENHANCE
endif

ifeq ($(HAS_BLOCK_NET_IF),y)
WFLAGS += -DBLOCK_NET_IF
endif

ifeq ($(HAS_DFS_SUPPORT),y)
WFLAGS += -DDFS_SUPPORT
endif

#ifdef MULTI_CARD
ifeq ($(HAS_MC_SUPPORT),y)
WFLAGS += -DMULTIPLE_CARD_SUPPORT
endif
#endif // MULTI_CARD //

ifeq ($(HAS_LLTD),y)
WFLAGS += -DLLTD_SUPPORT
endif

ifeq ($(PLATFORM),IXP)
WFLAGS += -DRT_BIG_ENDIAN
endif

ifeq ($(PLATFORM),IKANOS_V160)
WFLAGS += -DRT_BIG_ENDIAN -DIKANOS_VX_1X0
endif

ifeq ($(PLATFORM),IKANOS_V180)
WFLAGS += -DRT_BIG_ENDIAN -DIKANOS_VX_1X0
endif

ifeq ($(PLATFORM),INF_TWINPASS)
WFLAGS += -DRT_BIG_ENDIAN -DINF_TWINPASS
endif

ifeq ($(PLATFORM),INF_DANUBE)
WFLAGS += -DINF_DANUBE -DRT_BIG_ENDIAN
endif


ifeq ($(PLATFORM),BRCM_6358)
WFLAGS += -DRT_BIG_ENDIAN
endif

export WFLAGS