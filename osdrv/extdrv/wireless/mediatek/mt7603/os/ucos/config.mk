# Support ATE function
HAS_ATE=n

# Support WSC function
HAS_WSC=n

#ifdef NINTENDO_AP
HAS_NINTENDO=n
#endif // NINTENDO_AP //

# Support LLTD function
HAS_LLTD=n

# Support WDS function
HAS_WDS=n

# Support AP-Client function
HAS_APCLI=n

# Support Wpa_Supplicant
HAS_WPA_SUPPLICANT=n

# Support for STA 11n WiFi test
HAS_STA_WIFI_TEST=n

#Support Net interface block while Tx-Sw queue full
HAS_BLOCK_NET_IF=n

#Support IGMP-Snooping function.
HAS_IGMP_SNOOP_SUPPORT=n

#Support DFS function
HAS_DFS_SUPPORT=n

#Support Carrier-Sense function
HAS_CS_SUPPORT=n

# Support for STA Ethernet Converter
HAS_STA_ETH_CONVERT_SUPPORT=n

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

WFLAGS := -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT  -Wall -Wstrict-prototypes -Wno-trigraphs 

ifeq ($(RT2860_MODE),AP)
WFLAGS += -DCONFIG_AP_SUPPORT  -DUAPSD_AP_SUPPORT -DDBG

ifeq ($(HAS_ATE),y)
WFLAGS += -DCONFIG_ATE
#ifdef GEMTEK_ATE
ifeq ($(RELEASE), DPB)
WFLAGS += -DGEMTEK_ATE
endif
#endif // GEMTEK_ATE //
endif

ifeq ($(HAS_STA_WIFI_TEST),y)
WFLAGS += -DWIFI_TEST
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
WFLAGS += -DAPCLI_SUPPORT -DMAT_SUPPORT
endif

ifeq ($(HAS_BLOCK_NET_IF),y)
WFLAGS += -DBLOCK_NET_IF
endif

ifeq ($(HAS_IGMP_SNOOP_SUPPORT),y)
WFLAGS += -DIGMP_SNOOP_SUPPORT
endif

ifeq ($(HAS_CS_SUPPORT),y)
WFLAGS += -DCARRIER_DETECTION_SUPPORT
endif
endif

ifeq ($(HAS_DFS_SUPPORT),y)
WFLAGS += -DDFS_SUPPORT
endif

ifeq ($(RT2860_MODE),STA)
WFLAGS += -DCONFIG_STA_SUPPORT -DDBG 
ifeq ($(HAS_WPA_SUPPLICANT),y)
WFLAGS += -DWPA_SUPPLICANT_SUPPORT
endif

ifeq ($(HAS_STA_ETH_CONVERT_SUPPORT), y)
WFLAGS += -DSTA_ETH_CONVERT_SUPPORT  -DMAT_SUPPORT
endif
endif

ifeq ($(HAS_LLTD),y)
WFLAGS += -DLLTD_SUPPORT
endif

WFLAGS += -DCONFIG_RALINK_RT2880_VER_A

CFLAGS :=  -c -gdwarf-2 -G 0 -falign-functions -falign-jumps -falign-loops -falign-labels -funit-at-a-time -finline-functions -mips32 -mno-abicalls -fno-pic -pipe  -fno-builtin -mtune=4kec -mno-long-calls -ffreestanding -Wall -Wstrict-prototypes -Wno-trigraphs  -mabi=32 -fomit-frame-pointer -D__KERNEL__ -DUCOS -I$(UCOS_SRC)/bsp/cpu/inc -I$(UCOS_SRC)/bsp/rt2882/inc -I$(UCOS_SRC)/kernel/ucos -I$(UCOS_SRC)/include -I$(RT2860_DIR)/include -I$(UCOS_SRC)/bsp/lib/inc -I$(UCOS_SRC)/samples/lwip_on_ucos_test -I$(UCOS_SRC)/net/lwip/src/include -I$(UCOS_SRC)/net/lwip/src/arch/ucosII/include -I$(UCOS_SRC)/net/lwip/src/include/ipv4 $(WFLAGS)

export CFLAGS

