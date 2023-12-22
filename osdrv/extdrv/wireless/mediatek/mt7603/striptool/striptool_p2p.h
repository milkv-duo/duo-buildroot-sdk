#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define YACC_CONFIG_FILE "striptool/parser.config"
#define TRUE    0
#define FALSE   1

#if defined(MODE_AP) || defined(MODE_APSTA) || defined(RELEASE_ARCH)
#define INCLUDE_AP
#endif
#if defined(MODE_STA) || defined(MODE_APSTA) || defined(RELEASE_ARCH)
#define INCLUDE_STA
#endif

#define INCLUDE_WAPI
//#define INCLUDE_ACM
//#define INCLUDE_CONFIG_SWMCU_SUPPORT /* APSOC */
//#define INCLUDE_USE_GPL_ALGORITHM
//#define INCLUDE_11R_FT
//#define INCLUDE_11K_RRM
//#define INCLUDE_11W_PMF
//#define INCLUDE_11V_WNM_SUPPORT
//#define INCLUDE_HW_COEXISTENCE_SUPPORT
//#define INCLUDE_BT_COEXISTENCE_SUPPORT
//#define INCLUDE_DFS

#ifdef RELEASE_ARCH
	#define RELEASE_PATH    "ARCH/"
	#define INCLUDE_WPS
	#define INCLUDE_MESH
	#define INCLUDE_MAT
	#ifdef INCLUDE_AP
		#define INCLUDE_NINTENDO_AP
		#define INCLUDE_GEMTEK_ATE
		#define INCLUDE_SAMSUNG_SUPPORT
	#endif
	#ifdef INCLUDE_STA
		#define INCLUDE_TDLS
		#define INCLUDE_ETH_CONVERT
	#endif

	#define INCLUDE_CONFIG_ZTE_RADIO_ONOFF
#endif // RELEASE_ARCH //

#ifdef RELEASE_DPAW
    #define RELEASE_PATH		"DPAW/"
    #define INCLUDE_WPS
	#define INCLUDE_P2P
    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
    	#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
    #endif // INCLUDE_STA
#endif // RELEASE_DPAW //

#ifdef RELEASE_DPA
    #define RELEASE_PATH		"DPA/"
    #define INCLUDE_WPS
	#define INCLUDE_P2P
    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
    	#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
    #endif // INCLUDE_STA
#endif // RELEASE_DPA //

#ifdef RELEASE_DPB
    #define RELEASE_PATH		"DPB/"
	#ifdef INCLUDE_AP
		#define INCLUDE_NINTENDO_AP
		#define INCLUDE_GEMTEK_ATE
	#endif
	#define INCLUDE_WPS
	#define INCLUDE_MESH
	#define INCLUDE_P2P
	#define INCLUDE_MAT
	#ifdef INCLUDE_STA
		#define INCLUDE_ETH_CONVERT
		#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
	#endif
#endif

#ifdef RELEASE_DPC
    #define RELEASE_PATH		"DPC/"
    #define INCLUDE_WPS
    #define INCLUDE_MESH
	#define INCLUDE_P2P
    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
    	#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
    #endif // INCLUDE_STA
#endif

#ifdef RELEASE_DPD
    #define RELEASE_PATH		"DPD/"
	#define INCLUDE_WPS
	#define INCLUDE_P2P
	#ifdef INCLUDE_STA
		#define INCLUDE_ETH_CONVERT
		#define INCLUDE_MAT
		#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
	#endif
#endif

	/* This is a specific release for infineon/AR9 project */
#ifdef RELEASE_DPI
    #define RELEASE_PATH		"DPI/"
    #define INCLUDE_WPS
    #define INCLUDE_MAT
    #undef INCLUDE_WAPI
#endif // RELEASE_DPA //

#ifdef RELEASE_DPO
    #define RELEASE_PATH    "DPO/"
	#undef INCLUDE_WAPI
	#undef INCLUDE_ACM
#endif

#ifdef RELEASE_DPA_S
	#define RELEASE_PATH    "DPA_S/"
	#define INCLUDE_WPS
    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
    	#define INCLUDE_TDLS
    #endif // INCLUDE_STA
	/*#define INCLUDE_USE_GPL_ALGORITHM*/
	#define INCLUDE_SAMSUNG_SUPPORT
	#define INCLUDE_SAMSUNG_STA_SUPPORT
#endif

#ifdef RELEASE_DPA_T
	#define RELEASE_PATH    "DPA_T/"
	#define INCLUDE_WPS
    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
    	#define INCLUDE_TDLS
    #endif // INCLUDE_STA
	/*#define INCLUDE_USE_GPL_ALGORITHM*/
	#define INCLUDE_TOSHIBA_SUPPORT
	#define INCLUDE_WIDI
#endif

/* Specific release for Linux kernel to support PCI, USB and PCIe chipsets */
#ifdef RELEASE_GPL
    #define RELEASE_PATH    "GPL/"
	#define RTMP_MAC_USB
	#define RTMP_MAC_PCI
	#define RTMP_USB_SUPPORT
	#define RTMP_PCI_SUPPORT
	#define RTMP_EFUSE_SUPPORT
	#define INCLUDE_USE_GPL_ALGORITHM

	#undef OS_ABL_SUPPORT
	#undef INCLUDE_WAPI
	#undef INCLUDE_ACM
	#undef INCLUDE_WPS
	#undef INCLUDE_MESH
	#undef INCLUDE_ADHOC_WPA2PSK
	#undef INCLUDE_11K_RRM
	#undef INCLUDE_TDLS
	#undef INCLUDE_11W_PMF
	#undef INCLUDE_11V_WNM_SUPPORT
	#undef INCLUDE_SAMSUNG_SUPPORT
	#undef LED_CONTROL_SUPPORT
	#undef INCLUDE_NINTENDO_AP
	#undef INCLUDE_11R_FT
	#undef INCLUDE_SAMSUNG_STA_SUPPORT

	//Only for LINUX
	#define LINUX
	#undef ECOS
	#undef UCOS
	#undef VXWORKS
	#undef THREADX

	//Include USB
	#define RT2070
	#define RT2870
	#define RT3070
	#define RT3370
	#define RT3572
	#define RT30xx
	#define RT33xx
	#define RT35xx
	#undef CHIPSET_2070

	//Include PCI
	#define RT2860
	#define RT3062
	#define RT3562

	//Include PCIe
	#define RT3090
	#define RT3390
	#define RT3592
	#define RT3593

	//Not include APSOC
	#undef RT2880
	#undef RT2883
	#undef RT3050
	#undef RT3052
	#undef RT305x
	#undef RT3350
	#undef RT3352
	#undef RT3883
	#undef RALINK_3052

#endif

#ifdef OS_ABL_SUPPORT
	/* UTIL/NETIF and MODULE */
	#define RELEASE_MODULE_PATH	"MODULE/"
	#define RELEASE_UTIL_PATH	"UTIL/"
	#define RELEASE_NETIF_PATH	"NETIF/"
	#define RELEASE_CMD_PATH	"CMD/"
#endif // OS_ABL_SUPPORT //

#ifdef LINUX
#define CONFIG_MK    "os/linux/config.mk"
#endif // LINUX //

#ifdef ECOS
#define CONFIG_MK    "os/ecos/config.mk"
#define RT305x
#define RT3050
#define RT3052
#define RT3350
#define RT3352
#define RTMP_FLASH_SUPPORT
#endif // ECOS //

typedef struct _RTDefineTag {
    char DefineTag[255];
    int IsDEF;
} RTDefineTag, *PRTDefineTag;

static RTDefineTag DefineTagList[] = {

#ifdef P2P_SUPPORT
#ifdef MODE_APSTA
	{"WDS_SUPPORT", FALSE},
	{"CONFIG_AP_SUPPORT", FALSE},
#endif
#endif // P2P_SUPPORT //

};

void	fnCommStrip(char *pBuildDirectory, char *pReleasePath);
int 	fnStripCmd(void);

