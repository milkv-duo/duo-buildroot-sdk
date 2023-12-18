# Support ATE function
HAS_ATE=n

# Support 28xx QA ATE function
HAS_QA_SUPPORT=n

#ifdef WSC_INCLUDED
# Support WSC function
HAS_WSC=y
#endif // WSC_INCLUDED //

#ifdef NINTENDO_AP
HAS_NINTENDO=n
#endif // NINTENDO_AP //

# Support LLTD function
HAS_LLTD=n

# Support WDS function
HAS_WDS=n

#ifdef APCLI_SUPPORT
# Support AP-Client function
HAS_APCLI=n
#endif // APCLI_SUPPORT //

#ifdef WPA_SUPPLICANT_SUPPORT
# Support Wpa_Supplicant
HAS_WPA_SUPPLICANT=n
#endif // WPA_SUPPLICANT_SUPPORT //

#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
# Support Native WpaSupplicant for Network Maganger
HAS_NATIVE_WPA_SUPPLICANT_SUPPORT=n
#endif // NATIVE_WPA_SUPPLICANT_SUPPORT //

#Support Net interface block while Tx-Sw queue full
HAS_BLOCK_NET_IF=n

#Support IGMP-Snooping function.
HAS_IGMP_SNOOP_SUPPORT=n

#Support DFS function
HAS_DFS_SUPPORT=n

#Support Carrier-Sense function
HAS_CS_SUPPORT=n

#ifdef ETH_CONVERT
# Support for STA Ethernet Converter
HAS_ETH_CONVERT_SUPPORT=n
#endif // ETH_CONVERT //

# Support user specific transmit rate of Multicast packet.
HAS_MCAST_RATE_SPECIFIC_SUPPORT=n

#ifdef MULTI_CARD
# Support for Multiple Cards
HAS_MC_SUPPORT=n
#endif // MULTI_CARD //

#Support for PCI-MSI
HAS_MSI_SUPPORT=n

#ifdef WMM_ACM_SUPPORT
# Support for WMM ACM v1.1
HAS_WMM_ACM_SUPPORT=n
#endif // WMM_ACM_SUPPORT //

#Support for IEEE802.11e DLS
HAS_QOS_DLS_SUPPORT=n

#Support for EXT_CHANNEL
HAS_EXT_BUILD_CHANNEL_LIST=n

#Support for IDS 
HAS_IDS_SUPPORT=n

#ifdef MESH_SUPPORT
#Support for MESH
HAS_MESH_SUPPORT=n
#endif // MESH_SUPPORT //

#Support for Net-SNMP
HAS_SNMP_SUPPORT=n

#ifdef DOT11N_DRAFT3
#Support features of 802.11n Draft3
HAS_DOT11N_DRAFT3_SUPPORT=n
#endif // DOT11N_DRAFT3 //

#Support features of Single SKU. 
HAS_SINGLE_SKU_SUPPORT=n

#ifdef DOT11_N_SUPPORT
#Support features of 802.11n
HAS_DOT11_N_SUPPORT=y
#endif // DOT11_N_SUPPORT //

#ifdef WAPI_SUPPORT
#Support for WAPI
HAS_WAPI_SUPPORT=n
#endif // WAPI_SUPPORT //

#ifdef DOT11R_FT_SUPPORT
#Support for dot11r FT
HAS_DOT11R_FT_SUPPORT=n
#endif // DOT11R_FT_SUPPORT //

#Support for 2860/2880 co-exist 
HAS_RT2880_RT2860_COEXIST=n

HAS_KTHREAD_SUPPORT=n

#ifdef DOT11K_RRM_SUPPORT
#Support for dot11k RRM
HAS_DOT11K_RRM_SUPPORT=n
#endif // DOT11K_RRM_SUPPORT //

#ifdef DOT11Z_TDLS_SUPPORT
#Support for dot11z TDLS
HAS_DOT11Z_TDLS_SUPPORT=n
#endif // DOT11Z_TDLS_SUPPORT //

#Support for Auto channel select enhance
HAS_AUTO_CH_SELECT_ENHANCE=n

#Support statistics count
HAS_STATS_COUNT=y

#ifdef DOT11W_PMF_SUPPORT
#Support for dot11w Protected Management Frame
HAS_DOT11W_PMF_SUPPORT=n
#endif // DOT11W_PMF_SUPPORT //

#Support Antenna Diversity
HAS_ANTENNA_DIVERSITY_SUPPORT=n

#Client support WDS function
HAS_CLIENT_WDS_SUPPORT=n

#Support for Bridge Fast Path & Bridge Fast Path function open to other module
HAS_BGFP_SUPPORT=n
HAS_BGFP_OPEN_SUPPORT=n

# Support HOSTAPD function
HAS_HOSTAPD_SUPPORT=n

# Support ThreadX IOCTL Support
HAS_IOCTL_SUPPORT=y

# Support SigmalTel Puma system manager
HAS_SY_IOCTL_SUPPORT=y

#ifdef RELASE_EXCLUDE
#Support GPL Cryptography
HAS_GPL_ALGORITHM_SUPPORT=y
#endif //RELASE_EXCLUDE //
#################################################

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

WFLAGS := -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT
ifeq ($(TARGET),LINUX)
WFLAGS += -DLINUX -Wall -Wstrict-prototypes -Wno-trigraphs 

#removed for building in RT3052 ApSoc SDK
ifneq ($(PLATFORM), RALINK_3052)
WFLAGS += -Wpointer-sign
endif

endif

ifeq ($(TARGET),THREADX)
WFLAGS += -DTHREADX -DEBS_RTIP
endif


ifeq ($(HAS_KTHREAD_SUPPORT),y)
WFLAGS += -DKTHREAD_SUPPORT
endif

#ifdef RELASE_EXCLUDE
ifeq ($(HAS_GPL_ALGORITHM_SUPPORT),y)
WFLAGS += -DCRYPT_GPL_ALGORITHM
endif
#endif //RELASE_EXCLUDE //

#ifdef CONFIG_AP_SUPPORT
# config for AP mode

ifeq ($(RT28xx_MODE),AP)
WFLAGS += -DCONFIG_AP_SUPPORT  -DUAPSD_SUPPORT -DMBSS_SUPPORT -DIAPP_SUPPORT -DDBG

ifeq ($(HAS_HOSTAPD_SUPPORT),y)
WFLAGS += -DHOSTAPD_SUPPORT
endif

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

#ifdef WSC_INCLUDED
ifeq ($(HAS_WSC),y)
WFLAGS += -DWSC_AP_SUPPORT
endif
#endif // WSC_INCLUDED //

ifeq ($(HAS_WDS),y)
WFLAGS += -DWDS_SUPPORT
endif

#ifdef APCLI_SUPPORT
ifeq ($(HAS_APCLI),y)
WFLAGS += -DAPCLI_SUPPORT -DMAT_SUPPORT
#ifeq ($(HAS_ETH_CONVERT_SUPPORT), y)
#WFLAGS += -DETH_CONVERT_SUPPORT
#endif 
endif
#endif // APCLI_SUPPORT //

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

#ifdef WMM_ACM_SUPPORT
ifeq ($(HAS_WMM_ACM_SUPPORT),y)
WFLAGS += -DWMM_ACM_SUPPORT
endif
#endif // WMM_ACM_SUPPORT //

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
WFLAGS += -DQOS_DLS_SUPPORT
endif

ifeq ($(HAS_SNMP_SUPPORT),y)
WFLAGS += -DSNMP_SUPPORT
endif


ifeq ($(HAS_SINGLE_SKU_SUPPORT),y)
WFLAGS += -DSINGLE_SKU
endif

#ifdef DOT11_N_SUPPORT
ifeq ($(HAS_DOT11_N_SUPPORT),y)
WFLAGS += -DDOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
ifeq ($(HAS_DOT11N_DRAFT3_SUPPORT),y)
WFLAGS += -DDOT11N_DRAFT3
endif
#endif // DOT11N_DRAFT3 //
endif
#endif // DOT11_N_SUPPORT //

ifeq ($(HAS_AUTO_CH_SELECT_ENHANCE),y)
WFLAGS += -DAUTO_CH_SELECT_ENHANCE
endif

ifeq ($(HAS_STATS_COUNT),y)
WFLAGS += -DSTATS_COUNT_SUPPORT
endif

ifeq ($(HAS_ANTENNA_DIVERSITY_SUPPORT),y)
WFLAGS += -DANT_DIVERSITY_SUPPORT
endif

endif #// endif of RT2860_MODE == AP //
#endif // CONFIG_AP_SUPPORT //

#################################################

#ifdef CONFIG_STA_SUPPORT
# config for STA mode

ifeq ($(RT28xx_MODE),STA)
WFLAGS += -DCONFIG_STA_SUPPORT -DDBG

#ifdef WPA_SUPPLICANT_SUPPORT
ifeq ($(HAS_WPA_SUPPLICANT),y)
WFLAGS += -DWPA_SUPPLICANT_SUPPORT
#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
ifeq ($(HAS_NATIVE_WPA_SUPPLICANT_SUPPORT),y)
WFLAGS += -DNATIVE_WPA_SUPPLICANT_SUPPORT
endif
#endif // NATIVE_WPA_SUPPLICANT_SUPPORT //
endif
#endif // WPA_SUPPLICANT_SUPPORT //

#ifdef WSC_INCLUDED
ifeq ($(HAS_WSC),y)
WFLAGS += -DWSC_STA_SUPPORT
endif
#endif // WSC_INCLUDED //

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

#ifdef WMM_ACM_SUPPORT
ifeq ($(HAS_WMM_ACM_SUPPORT),y)
WFLAGS += -DWMM_ACM_SUPPORT
endif
#endif // WMM_ACM_SUPPORT //

ifeq ($(HAS_SNMP_SUPPORT),y)
WFLAGS += -DSNMP_SUPPORT
endif

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
WFLAGS += -DQOS_DLS_SUPPORT
endif

#ifdef DOT11_N_SUPPORT
ifeq ($(HAS_DOT11_N_SUPPORT),y)
WFLAGS += -DDOT11_N_SUPPORT
endif
#endif // DOT11_N_SUPPORT //

#ifdef DOT11Z_TDLS_SUPPORT
ifeq ($(HAS_DOT11Z_TDLS_SUPPORT),y)
WFLAGS += -DDOT11Z_TDLS_SUPPORT
endif
#endif // DOT11Z_TDLS_SUPPORT //

ifeq ($(HAS_CS_SUPPORT),y)
WFLAGS += -DCARRIER_DETECTION_SUPPORT
endif

ifeq ($(HAS_STATS_COUNT),y)
WFLAGS += -DSTATS_COUNT_SUPPORT
endif

ifeq ($(HAS_ANTENNA_DIVERSITY_SUPPORT),y)
WFLAGS += -DANT_DIVERSITY_SUPPORT
endif

endif
# endif of ifeq ($(RT28xx_MODE),STA)
#endif // CONFIG_STA_SUPPORT //

#################################################

#ifdef CONFIG_APSTA_SUPPORT
# config for APSTA

ifeq ($(RT28xx_MODE),APSTA)
WFLAGS += -DCONFIG_AP_SUPPORT -DCONFIG_STA_SUPPORT -DCONFIG_APSTA_MIXED_SUPPORT -DUAPSD_SUPPORT -DMBSS_SUPPORT -DIAPP_SUPPORT -DDBG

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

#ifdef WSC_INCLUDED
ifeq ($(HAS_WSC),y)
WFLAGS += -DWSC_AP_SUPPORT -DWSC_STA_SUPPORT
endif
#endif // WSC_INCLUDED //

ifeq ($(HAS_WDS),y)
WFLAGS += -DWDS_SUPPORT
endif

#ifdef APCLI_SUPPORT
ifeq ($(HAS_APCLI),y)
WFLAGS += -DAPCLI_SUPPORT -DMAT_SUPPORT
endif
#endif // APCLI_SUPPORT //

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

#ifdef WPA_SUPPLICANT_SUPPORT
ifeq ($(HAS_WPA_SUPPLICANT),y)
WFLAGS += -DWPA_SUPPLICANT_SUPPORT
#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
ifeq ($(HAS_NATIVE_WPA_SUPPLICANT_SUPPORT),y)
WFLAGS += -DNATIVE_WPA_SUPPLICANT_SUPPORT
endif
#endif // NATIVE_WPA_SUPPLICANT_SUPPORT //
endif
#endif // WPA_SUPPLICANT_SUPPORT //

#ifdef ETH_CONVERT
ifeq ($(HAS_ETH_CONVERT_SUPPORT), y)
WFLAGS += -DETH_CONVERT_SUPPORT  -DMAT_SUPPORT
endif
#endif // ETH_CONVERT //

#ifdef DOT11_N_SUPPORT
ifeq ($(HAS_DOT11_N_SUPPORT),y)
WFLAGS += -DDOT11_N_SUPPORT
endif
#endif // DOT11_N_SUPPORT //

ifeq ($(HAS_CS_SUPPORT),y)
WFLAGS += -DCARRIER_DETECTION_SUPPORT
endif

ifeq ($(HAS_STATS_COUNT),y)
WFLAGS += -DSTATS_COUNT_SUPPORT
endif

endif
# endif of ifeq ($(RT28xx_MODE),APSTA)
#endif // CONFIG_APSTA_SUPPORT //
#################################################

#
# Common compiler flag
#

#ifdef RELASE_EXCLUDE
#Remove internal debug code when offical release 
WFLAGS += -DRELASE_EXCLUDE
#endif // RELASE_EXCLUDE //

#ifdef MESH_SUPPORT
ifeq ($(HAS_MESH_SUPPORT),y)
WFLAGS += -DMESH_SUPPORT -DINTEL_CMPC
endif
#endif // MESH_SUPPORT //

#ifdef DOT11R_FT_SUPPORT
ifeq ($(HAS_DOT11R_FT_SUPPORT),y)
WFLAGS += -DDOT11R_FT_SUPPORT
endif
#endif // DOT11R_FT_SUPPORT //

#ifdef DOT11K_RRM_SUPPORT
ifeq ($(HAS_DOT11K_RRM_SUPPORT),y)
WFLAGS += -DDOT11K_RRM_SUPPORT
endif
#endif // DOT11K_RRM_SUPPORT //

ifeq ($(HAS_EXT_BUILD_CHANNEL_LIST),y)
WFLAGS += -DEXT_BUILD_CHANNEL_LIST
endif

ifeq ($(HAS_IDS_SUPPORT),y)
WFLAGS += -DIDS_SUPPORT
endif

#ifdef WAPI_SUPPORT
ifeq ($(HAS_WAPI_SUPPORT),y)
WFLAGS += -DWAPI_SUPPORT -DSOFT_ENCRYPT
endif
#endif // WAPI_SUPPORT //

ifeq ($(HAS_CLIENT_WDS_SUPPORT),y)
WFLAGS += -DCLIENT_WDS
endif

ifeq ($(HAS_BGFP_SUPPORT),y)
WFLAGS += -DBG_FT_SUPPORT
endif

ifeq ($(HAS_BGFP_OPEN_SUPPORT),y)
WFLAGS += -DBG_FT_OPEN_SUPPORT
endif

ifeq ($(HAS_IOCTL_SUPPORT), y)
WFLAGS += -DRTMP_IOCTL_SUPPORT
endif

#################################################
# ChipSet specific definitions.
#
ifeq ($(CHIPSET),2860)
WFLAGS +=-DRTMP_MAC_PCI -DRTMP_PCI_SUPPORT -DRT2860
endif

ifeq ($(CHIPSET),3090)
WFLAGS +=-DRTMP_MAC_PCI -DRT30xx -DRT3090  -DRTMP_PCI_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT
endif

ifeq ($(CHIPSET),2870)
WFLAGS +=-DRTMP_MAC_USB -DRTMP_USB_SUPPORT -DRT2870 -DRTMP_TIMER_TASK_SUPPORT
endif

ifeq ($(CHIPSET),2070)
WFLAGS +=-DRTMP_MAC_USB -DRT30xx -DRT3070 -DRT2070 -DRTMP_USB_SUPPORT -DRTMP_TIMER_TASK_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT
endif

ifeq ($(CHIPSET),3070)
WFLAGS +=-DRTMP_MAC_USB -DRT30xx -DRT3070 -DRTMP_USB_SUPPORT -DRTMP_TIMER_TASK_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT
endif

ifeq ($(CHIPSET),2880)
WFLAGS += -DRT2880 -DRTMP_MAC_PCI -DCONFIG_RALINK_RT2880_MP -DRTMP_RBUS_SUPPORT -DMERGE_ARCH_TEAM
ifeq ($(HAS_DFS_SUPPORT),y)
WFLAGS += -DNEW_DFS -DDFS_FCC_BW40_FIX -DDFS_DEBUG
endif
endif

ifeq ($(CHIPSET),3572)
WFLAGS +=-DRTMP_MAC_USB -DRTMP_USB_SUPPORT -DRT2870 -DRT30xx -DRT35xx -DRTMP_TIMER_TASK_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT
endif

ifeq ($(CHIPSET),3062)
WFLAGS +=-DRTMP_MAC_PCI -DRT2860 -DRT30xx -DRT35xx -DRTMP_PCI_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT
endif

ifeq ($(CHIPSET),3562)
WFLAGS +=-DRTMP_MAC_PCI -DRT2860 -DRT30xx -DRT35xx -DRTMP_PCI_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT
endif

ifeq ($(CHIPSET),USB_COMBO)
WFLAGS +=-DRTMP_MAC_USB -DRTMP_USB_SUPPORT -DRT2870 -DRT3070 -DRT2070 -DRT30xx -DRT35xx -DRTMP_TIMER_TASK_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT
endif

ifeq ($(CHIPSET),3390)
WFLAGS +=-DRTMP_MAC_PCI -DRT30xx -DRT33xx  -DRTMP_PCI_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT -DRT3390
endif

ifeq ($(CHIPSET),3370)
WFLAGS +=-DRTMP_MAC_USB -DRT30xx -DRT33xx  -DRTMP_USB_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT -DRT3370 -DRTMP_TIMER_TASK_SUPPORT
WFLAGS += -DRT3070
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
ifneq (,$(findstring 2.4,$(LINUX_SRC)))
# Linux 2.4
WFLAGS += -DINF_DANUBE -DRT_BIG_ENDIAN
else
# Linux 2.6
WFLAGS += -DRT_BIG_ENDIAN
endif
endif

ifeq ($(PLATFORM),INF_AR9)
WFLAGS += -DRT_BIG_ENDIAN -DINF_AR9
endif

ifeq ($(PLATFORM),CAVM_OCTEON)
WFLAGS += -DRT_BIG_ENDIAN
endif

ifeq ($(PLATFORM),BRCM_6358)
WFLAGS += -DRT_BIG_ENDIAN
endif

ifeq ($(PLATFORM),INF_AMAZON_SE)
WFLAGS += -DRT_BIG_ENDIAN -DINF_AMAZON_SE
endif

ifeq ($(PLATFORM),ST)
#WFLAGS += -DST
WFLAGS += -DST
endif

#kernel build options for 2.4
# move to Makefile outside LINUX_SRC := /opt/star/kernel/linux-2.4.27-star

ifeq ($(PLATFORM),RALINK_3052)
CFLAGS := -D__KERNEL__ -I$(LINUX_SRC)/include/asm-mips/mach-generic -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -pipe  -finline-limit=100000 -march=mips2 -mabi=32 -Wa,--trap -DLINUX -nostdinc -iwithprefix include $(WFLAGS)
export CFLAGS
endif

ifeq ($(PLATFORM), RALINK_2880)
CFLAGS := -D__KERNEL__ -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -pipe  -finline-limit=100000 -march=mips2 -mabi=32 -Wa,--trap -DLINUX -nostdinc -iwithprefix include $(WFLAGS)
export CFLAGS
endif

ifeq ($(PLATFORM),STAR)
CFLAGS := -D__KERNEL__ -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -Uarm -fno-common -pipe -mapcs-32 -D__LINUX_ARM_ARCH__=4 -march=armv4  -mshort-load-bytes -msoft-float -Uarm -DMODULE -DMODVERSIONS -include $(LINUX_SRC)/include/linux/modversions.h $(WFLAGS)

export CFLAGS
endif

ifeq ($(PLATFORM),SIGMA)
CFLAGS := -D__KERNEL__ -I$(RT28xx_DIR)/include -I$(LINUX_SRC)/include -I$(LINUX_SRC)/include/asm/gcc -I$(LINUX_SRC)/include/asm-mips/mach-tango2 -I$(LINUX_SRC)/include/asm-mips/mach-tango2 -DEM86XX_CHIP=EM86XX_CHIPID_TANGO2 -DEM86XX_REVISION=6 -I$(LINUX_SRC)/include/asm-mips/mach-generic -I$(RT2860_DIR)/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -ffreestanding -O2     -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -pipe  -mabi=32 -march=mips32r2 -Wa,-32 -Wa,-march=mips32r2 -Wa,-mips32r2 -Wa,--trap -DMODULE $(WFLAGS) -DSIGMA863X_PLATFORM

export CFLAGS
endif

ifeq ($(PLATFORM),SIGMA_8622)
CFLAGS := -D__KERNEL__ -I$(CROSS_COMPILE_INCLUDE)/include -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -fno-common -pipe -fno-builtin -D__linux__ -DNO_MM -mapcs-32 -march=armv4 -mtune=arm7tdmi -msoft-float -DMODULE -mshort-load-bytes -nostdinc -iwithprefix -DMODULE $(WFLAGS)
export CFLAGS
endif

ifeq ($(PLATFORM),5VT)
CFLAGS := -D__KERNEL__ -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -O3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=apcs-gnu -mno-thumb-interwork -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm926ej-s --param max-inline-insns-single=40000  -Uarm -Wdeclaration-after-statement -Wno-pointer-sign -DMODULE $(WFLAGS) 

export CFLAGS
endif

ifeq ($(PLATFORM),IKANOS_V160)
CFLAGS := -D__KERNEL__ -I$(LINUX_SRC)/include -I$(LINUX_SRC)/include/asm/gcc -I$(LINUX_SRC)/include/asm-mips/mach-tango2 -I$(LINUX_SRC)/include/asm-mips/mach-tango2 -I$(LINUX_SRC)/include/asm-mips/mach-generic -I$(RT28xx_DIR)/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -ffreestanding -O2 -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -pipe -march=lx4189 -Wa, -DMODULE $(WFLAGS)
export CFLAGS
endif

ifeq ($(PLATFORM),IKANOS_V180)
CFLAGS := -D__KERNEL__ -I$(LINUX_SRC)/include -I$(LINUX_SRC)/include/asm/gcc -I$(LINUX_SRC)/include/asm-mips/mach-tango2 -I$(LINUX_SRC)/include/asm-mips/mach-tango2 -I$(LINUX_SRC)/include/asm-mips/mach-generic -I$(RT28xx_DIR)/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -ffreestanding -O2 -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -pipe -mips32r2 -Wa, -DMODULE $(WFLAGS)
export CFLAGS
endif

ifeq ($(PLATFORM),INF_TWINPASS)
CFLAGS := -D__KERNEL__ -DMODULE -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -G 0 -mno-abicalls -fno-pic -march=4kc -mips32 -Wa,--trap -pipe -mlong-calls $(WFLAGS)
export CFLAGS
endif

ifeq ($(PLATFORM),INF_DANUBE)
CFLAGS := -I$(RT28xx_DIR)/include $(WFLAGS) -Wundef -fno-strict-aliasing -fno-common -ffreestanding -Os -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -pipe -msoft-float  -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap -I$(LINUX_SRC)/include/asm-mips/mach-generic
export CFLAGS
endif

ifeq ($(PLATFORM),INF_AR9)
CFLAGS := -I$(RT28xx_DIR)/include $(WFLAGS) -Wundef -fno-strict-aliasing -fno-common -fno-pic -ffreestanding -Os -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -pipe -msoft-float  -mabi=32 -mlong-calls -march=mips32r2 -mtune=34kc -march=mips32r2 -Wa,-32 -Wa,-march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I$(LINUX_SRC)/include/asm-mips/mach-generic
export CFLAGS
endif

ifeq ($(PLATFORM),BRCM_6358)
CFLAGS := $(WFLAGS) -I$(RT28xx_DIR)/include -nostdinc -iwithprefix include -D__KERNEL__ -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -I $(LINUX_SRC)/include/asm/gcc -G 0 -mno-abicalls -fno-pic -pipe  -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap -I$(LINUX_SRC)/include/asm-mips/mach-bcm963xx -I$(LINUX_SRC)/include/asm-mips/mach-generic  -Os -fomit-frame-pointer -Wdeclaration-after-statement  -DMODULE -mlong-calls
export CFLAGS
endif

ifeq ($(PLATFORM),INF_AMAZON_SE)
CFLAGS := -D__KERNEL__ -DMODULE=1 -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -DCONFIG_IFX_ALG_QOS -DCONFIG_WAN_VLAN_SUPPORT -fomit-frame-pointer -DIFX_PPPOE_FRAME -G 0 -fno-pic -mno-abicalls -mlong-calls -pipe -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap -nostdinc -iwithprefix include $(WFLAGS)
export CFLAGS
endif

ifeq ($(PLATFORM),ST)
CFLAGS := -D__KERNEL__ -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -Wall -O2 -Wundef -Wstrict-prototypes -Wno-trigraphs -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-aliasing -fno-common -fomit-frame-pointer -ffreestanding -o $(WFLAGS) 
export CFLAGS
endif

ifeq ($(PLATFORM),KODAK_DC)
RT_WIFI_CFLAGS := -I$(RT28xx_DIR)/include -O2 --gnu -DSIGMATEL_SDK $(WFLAGS) -DOA_CHIPID=7 -DTHREADX_RX_ONE_COPY
RT_WIFI_CFLAGS += -DRTMP_USB_BULKOUT_DEFAULT
#RT_WIFI_CFLAGS += -DRTMP_USB_BULKOUT_ALIGNED
export RT_WIFI_CFLAGS
endif

ifeq ($(PLATFORM),PC)
    ifneq (,$(findstring 2.4,$(LINUX_SRC)))
	# Linux 2.4
	CFLAGS := -D__KERNEL__ -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -pipe -mpreferred-stack-boundary=2 -march=i686 -DMODULE -DMODVERSIONS -include $(LINUX_SRC)/include/linux/modversions.h $(WFLAGS)
	export CFLAGS
    else
	# Linux 2.6
	EXTRA_CFLAGS := $(WFLAGS) -I$(RT28xx_DIR)/include
    endif
endif

ifeq ($(PLATFORM),IXP)
        EXTRA_CFLAGS := -v $(WFLAGS) -I$(RT28xx_DIR)/include -mbig-endian
endif

ifeq ($(PLATFORM),SMDK)
        EXTRA_CFLAGS := $(WFLAGS) -I$(RT28xx_DIR)/include
endif

ifeq ($(PLATFORM),CAVM_OCTEON)
	EXTRA_CFLAGS := $(WFLAGS) -I$(RT28xx_DIR)/include \
				    -mabi=64 $(WFLAGS)
export CFLAGS
endif
