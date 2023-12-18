#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define YACC_CONFIG_FILE "striptool/parser.config"
#define TRUE    0
#define FALSE   1

#if defined(MODE_AP) || defined(MODE_APSTA) || defined(CHIPSET_RBUS)
#define INCLUDE_AP
#define INCLUDE_GREENAP_SUPPORT
#endif
#if defined(MODE_STA) || defined(MODE_APSTA) || defined(CHIPSET_RBUS)
#define INCLUDE_STA
#endif

#define INCLUDE_WAPI
//#define INCLUDE_ACM
//#define INCLUDE_CONFIG_SWMCU_SUPPORT /* APSOC */
//#define INCLUDE_USE_GPL_ALGORITHM
//#define INCLUDE_11R_FT
//#define INCLUDE_11K_RRM
#define INCLUDE_11W_PMF
//#define INCLUDE_11V_WNM_SUPPORT
//#define INCLUDE_HW_COEXISTENCE_SUPPORT
//#define INCLUDE_BT_COEXISTENCE_SUPPORT
#define INCLUDE_DFS
#define INCLUDE_WOW_GPIO
//#define INCLUDE_WOW_HOST_STATUS
#define INCLUDE_TXBF_SUPPORT
#define INCLUDE_MAC_REPEATER_SUPPORT
#define INCLUDE_FLASH
#define RTMP_FLASH_SUPPORT
#define LED_CONTROL_SUPPORT

#define INCLUDE_ATE
#ifdef RELEASE_ARCH
	#define RELEASE_PATH    "ARCH/"
	#define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_HS
	#define INCLUDE_DOT11U_INTERWORKING
	#define INCLUDE_DOT11V_WNM
	#define INCLUDE_MESH
	#define INCLUDE_MAT
	#define INCLUDE_COC_SUPPORT
	#define INCLUDE_LED_CONTROL_SUPPORT
	#define INCLUDE_CONFIG_SWMCU_SUPPORT /* APSOC */
	#ifdef INCLUDE_AP
		#define INCLUDE_NINTENDO_AP
		#define INCLUDE_GEMTEK_ATE
		#define INCLUDE_SAMSUNG_SUPPORT
		#define INCLUDE_RT3XXX_AP_ANTENNA_DIVERSITY_SUPPORT
	#endif
	#ifdef INCLUDE_STA
		#define INCLUDE_TDLS
		#define INCLUDE_ETH_CONVERT
		#define INCLUDE_IWPS
	#endif

	#define INCLUDE_CONFIG_ZTE_RADIO_ONOFF
#endif // RELEASE_ARCH //

#ifdef RELEASE_ARCH_DPA
	#define RELEASE_PATH    "ARCH_DPA/"
	#define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_MAT
	#define INCLUDE_COC_SUPPORT
	#define INCLUDE_LED_CONTROL_SUPPORT
	#define INCLUDE_CONFIG_SWMCU_SUPPORT
	#define SPECIFIC_BCN_BUF_SUPPORT
	#define INCLUDE_P2P
	#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
	#define INCLUDE_IWPS
#endif // RELEASE_ARCH_DPA //

#ifdef RELEASE_ARCH_DPB
	#define RELEASE_PATH    "ARCH_DPB/"
	#define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_MAT
	#define INCLUDE_NINTENDO_AP
	#define INCLUDE_GEMTEK_ATE
	#define INCLUDE_MESH
	#define INCLUDE_COC_SUPPORT
	#define INCLUDE_LED_CONTROL_SUPPORT
	#define INCLUDE_CONFIG_SWMCU_SUPPORT
	#define SPECIFIC_BCN_BUF_SUPPORT
	#define INCLUDE_P2P
	#define INCLUDE_TDLS
	#define INCLUDE_ETH_CONVERT
    	#define INCLUDE_ADHOC_WPA2PSK
	#define INCLUDE_IWPS
#endif // RELEASE_ARCH_DPB //

#ifdef RELEASE_ARCH_DPC
	#define RELEASE_PATH    "ARCH_DPC/"
	#define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_MAT
	#define INCLUDE_MESH
	#define INCLUDE_COC_SUPPORT
	#define INCLUDE_LED_CONTROL_SUPPORT
	#define INCLUDE_CONFIG_SWMCU_SUPPORT
	#define SPECIFIC_BCN_BUF_SUPPORT
	#define INCLUDE_P2P
	#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
	#define INCLUDE_IWPS
#endif // RELEASE_ARCH_DPA //

#ifdef RELEASE_ARCH_DPD
	#define RELEASE_PATH    "ARCH_DPD/"
	#define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_MAT
	#define INCLUDE_COC_SUPPORT
	#define INCLUDE_LED_CONTROL_SUPPORT
	#define INCLUDE_CONFIG_SWMCU_SUPPORT
	#define SPECIFIC_BCN_BUF_SUPPORT
	#define INCLUDE_P2P
	#define INCLUDE_TDLS
	#define INCLUDE_ETH_CONVERT
    	#define INCLUDE_ADHOC_WPA2PSK
	#define INCLUDE_IWPS
#endif // RELEASE_ARCH_DPA //

#ifdef RELEASE_DPAW
    #define RELEASE_PATH		"DPAW/"
    #define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_HS
	#define INCLUDE_DOT11U_INTERWORKING
	#define INCLUDE_DOT11V_WNM
    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
		#define INCLUDE_P2P
	#define INCLUDE_WFD
    	#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
		#define INCLUDE_WIDI
		#define INCLUDE_INTEL_WFD
		#define INCLUDE_WFA_WFD
		#define INCLUDE_IWPS
    #endif // INCLUDE_STA
	#define INCLUDE_LED_CONTROL_SUPPORT
#endif // RELEASE_DPAW //

#ifdef RELEASE_DPOA
	#define RELEASE_PATH                "DPOA/"
    	#define INCLUDE_CFG80211_SUPPORT	
    	#define INCLUDE_CFG_P2P		
	#define INCLUDE_WOW
	#define INCLUDE_LED_CONTROL_SUPPORT
	
	/* Optional Feature List for DPOA */
	#undef INCLUDE_WAPI
	#undef INCLUDE_MAC_REPEATER_SUPPORT	
	#undef INCLUDE_GREENAP_SUPPORT
	
	#ifdef MODE_AP
	#endif /* MODE_AP */
#endif // RELEASE_DPOA //

#ifdef RELEASE_DPA
    #define RELEASE_PATH		"DPA/"
    #define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_HS
	#define INCLUDE_DOT11U_INTERWORKING
	#define INCLUDE_DOT11V_WNM
    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
	#define INCLUDE_P2P
	#define INCLUDE_WFD
    	#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
	#define INCLUDE_IWPS
	#define INCLUDE_CFG80211_SUPPORT        
        #define INCLUDE_CFG_P2P         
        #define INCLUDE_WOW
    #endif // INCLUDE_STA
	#define INCLUDE_LED_CONTROL_SUPPORT
	#undef INCLUDE_TXBF
#endif // RELEASE_DPA //

#ifdef RELEASE_DPB
    #define RELEASE_PATH		"DPB/"
	#ifdef INCLUDE_AP
		#define INCLUDE_NINTENDO_AP
		#define INCLUDE_GEMTEK_ATE
	#endif
	#define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_HS
	#define INCLUDE_DOT11U_INTERWORKING
	#define INCLUDE_DOT11V_WNM
	#define INCLUDE_MAT
	#ifdef INCLUDE_STA
        	#define INCLUDE_P2P
		#define INCLUDE_ETH_CONVERT
		#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
		#define INCLUDE_IWPS
	#endif
	#define INCLUDE_LED_CONTROL_SUPPORT
#endif

#ifdef RELEASE_DPC
    #define RELEASE_PATH		"DPC/"
    #define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_HS
	#define INCLUDE_DOT11U_INTERWORKING
	#define INCLUDE_DOT11V_WNM
    #define INCLUDE_MESH
    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
	#define INCLUDE_P2P
    	#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
		#define INCLUDE_IWPS
    #endif // INCLUDE_STA
	#define INCLUDE_LED_CONTROL_SUPPORT
#endif

#ifdef RELEASE_DPD
    #define RELEASE_PATH		"DPD/"
	#define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_HS
	#define INCLUDE_DOT11U_INTERWORKING
	#define INCLUDE_DOT11V_WNM
	#ifdef INCLUDE_STA
        	#define INCLUDE_P2P
		#define INCLUDE_ETH_CONVERT
		#define INCLUDE_MAT
		#define INCLUDE_TDLS
    	#define INCLUDE_ADHOC_WPA2PSK
		#define INCLUDE_IWPS
	#endif
	#define INCLUDE_LED_CONTROL_SUPPORT
#endif

	/* This is a specific release for infineon/AR9 project */
#ifdef RELEASE_DPI
    #define RELEASE_PATH		"DPI/"
    #define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_HS
	#define INCLUDE_DOT11U_INTERWORKING
	#define INCLUDE_DOT11V_WNM
    #define INCLUDE_MAT
	#define INCLUDE_LED_CONTROL_SUPPORT
    #undef INCLUDE_WAPI
#endif // RELEASE_DPI //

#ifdef RELEASE_DPO
    #define RELEASE_PATH    "DPO/"
	#define INCLUDE_LED_CONTROL_SUPPORT
	#undef INCLUDE_WAPI
	#undef INCLUDE_ACM
	#undef INCLUDE_DFS
	#undef INCLUDE_WOW
	#undef INCLUDE_WOW_GPIO
	#undef INCLUDE_ATE
#endif

#ifdef RELEASE_DPA_S
	#define RELEASE_PATH    "DPA_S/"
	#define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_HS
	#define INCLUDE_DOT11U_INTERWORKING
	#define INCLUDE_DOT11V_WNM
	#define INCLUDE_P2P
	#define INCLUDE_WFD
	#define INCLUDE_WIDI
	#define INCLUDE_INTEL_WFD
	#define INCLUDE_WFA_WFD

    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
    	#define INCLUDE_TDLS
		#define INCLUDE_ADHOC_WPA2PSK
		#define INCLUDE_IWPS
    #endif // INCLUDE_STA
	/*#define INCLUDE_USE_GPL_ALGORITHM*/
	#define INCLUDE_SAMSUNG_SUPPORT
	#define INCLUDE_SAMSUNG_STA_SUPPORT
	#define INCLUDE_LED_CONTROL_SUPPORT
#endif

#ifdef RELEASE_DPA_T
	#define RELEASE_PATH    "DPA_T/"
	#define INCLUDE_WPS
	#define INCLUDE_WPS_V2
	#define INCLUDE_HS
	#define INCLUDE_DOT11U_INTERWORKING
	#define INCLUDE_DOT11V_WNM
	#define INCLUDE_P2P
    #ifdef INCLUDE_AP
        #define INCLUDE_MAT
    #endif // INCLUDE_AP //
    #ifdef INCLUDE_STA
		#define INCLUDE_ETH_CONVERT
		#define INCLUDE_MAT
    	#define INCLUDE_TDLS
		#define INCLUDE_ADHOC_WPA2PSK
		#define INCLUDE_IWPS
    #endif // INCLUDE_STA
	/*#define INCLUDE_USE_GPL_ALGORITHM*/
	#define INCLUDE_TOSHIBA_SUPPORT
	#define INCLUDE_WIDI
	#define INCLUDE_LED_CONTROL_SUPPORT
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
	#define INCLUDE_LED_CONTROL_SUPPORT

	#undef OS_ABL_SUPPORT
	#undef INCLUDE_WAPI
	#undef INCLUDE_ACM
	#undef INCLUDE_WPS
	#undef INCLUDE_HS
	#undef INCLUDE_DOT11U_INTERWORKING
	#undef INCLUDE_DOT11V_WNM
	#undef INCLUDE_WPS_V2
	#undef INCLUDE_IWPS
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
	#define RT3573
	#define RT5370
	#define RT5572
	#define MT7601
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
	#define RT5390

	//Not include APSOC
	#undef RT2880
	#undef RT2883
	#undef RT3050
	#undef RT3052
	#undef RT305x
	#undef RT3350
	#undef RT3352
	#undef RT3883
	#undef RT5350
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
#define RT5350
#define RTMP_FLASH_SUPPORT
#define RTMP_RBUS_SUPPORT
#define DRTMP_MAC_PCI 
#define RTMP_RBUS_SUPPORT
#define RTMP_RF_RW_SUPPORT 
#define SPECIFIC_BCN_BUF_SUPPORT
#define VCORECAL_SUPPORT 
#define CONFIG_SWMCU_SUPPORT
#define RTMP_INTERNAL_TX_ALC
#define INCLUDE_CONFIG_SWMCU_SUPPORT
//Not include
#undef RT2070
#undef RT2870
#undef RT3070
#undef RT3370
#undef RT3572
#undef RT30xx
#undef RT33xx
#undef RT35xx
#undef CHIPSET_2070
#undef RT2860
#undef RT3062
#undef RT3562
#undef RT3090
#undef RT3390
#undef RT3592
#undef RT3593
#undef RT5370
#undef RT5372
#undef RT5390
#undef RT5392
#undef RT5592
#undef RT5572
#undef INF_AMAZON_SE
#undef RTMP_MAC_USB
#undef RTMP_USB_SUPPORT
#undef RTMP_EFUSE_SUPPORT
#undef WSC_V2_SUPPORT
#undef INCLUDE_USE_GPL_ALGORITHM
#undef OS_ABL_SUPPORT
#undef OS_ABL_FUNC_SUPPORT
#undef CONFIG_WIFI_LED_SHARE
#undef INCLUDE_WAPI
#undef INCLUDE_ACM
#undef INCLUDE_MESH
#undef INCLUDE_ADHOC_WPA2PSK
#undef INCLUDE_11K_RRM
#undef INCLUDE_TDLS
#undef INCLUDE_11W_PMF
#undef INCLUDE_11V_WNM_SUPPORT
#undef INCLUDE_SAMSUNG_SUPPORT
#undef INCLUDE_NINTENDO_AP
#undef INCLUDE_11R_FT
#undef INCLUDE_SAMSUNG_STA_SUPPORT
#undef RTMP_TEMPERATURE_COMPENSATION /*Only for RT5392*/
#endif // ECOS //

/* inclusion per chip capability */
#ifdef RT5592
#define CONFIG_CSO_SUPPORT 
#define INCLUDE_M8051_SUPPORT
#endif

#ifdef RT2880
#define INCLUDE_WIFI_LED_SHARE
#endif

#ifdef RT3052
#define INCLUDE_WIFI_LED_SHARE
#endif

#ifdef RT3352
#define INCLUDE_WIFI_LED_SHARE
#endif

#ifdef RT5350
#define INCLUDE_WIFI_LED_SHARE
#endif

#ifdef MT76x0
#define INCLUDE_ANDES_SUPPORT
#endif

#ifdef MT76x2
#define INCLUDE_ANDES_SUPPORT
#endif

#ifdef MT7603
#define INCLUDE_ANDES_SUPPORT
#endif

typedef struct _RTDefineTag {
    char DefineTag[255];
    int IsDEF;
} RTDefineTag, *PRTDefineTag;

static RTDefineTag DefineTagList[] = {
	{"WEXT_WPS", FALSE},
    {"RELEASE_EXCLUDE", FALSE},
    {"CRYPT_TESTPLAN", FALSE},
	{"QOS_DLS_SUPPORT", FALSE},
#ifdef INCLUDE_USE_GPL_ALGORITHM
	{"CRYPT_GPL_ALGORITHM", TRUE},
#else
	{"CRYPT_GPL_ALGORITHM", FALSE},
#endif // INCLUDE_USE_GPL_ALGORITHM //

#ifndef INCLUDE_COC_SUPPORT
	{"COC_SUPPORT", FALSE},
#endif
#ifdef INCLUDE_AP
#ifndef CHIPSET_RBUS
#ifndef MODE_APSTA
	{"WPA_SUPPLICANT_SUPPORT", FALSE},
	{"NATIVE_WPA_SUPPLICANT_SUPPORT", FALSE},
	{"DOT11Z_TDLS_SUPPORT", FALSE},
#endif /* MODE_APSTA */
#endif /* CHIPSET_RBUS */
#ifndef INCLUDE_NINTENDO_AP
		{"NINTENDO_AP", FALSE},
#endif /* INCLUDE_NINTENDO_AP */

#ifndef INCLUDE_GEMTEK_ATE
		{"GEMTEK_ATE", FALSE},
#endif /* INCLUDE_GEMTEK_ATE */
#else
    {"CONFIG_AP_SUPPORT", FALSE},
    {"CONFIG_APSTA_SUPPORT", FALSE},
#endif // INCLUDE_AP //
#ifndef INCLUDE_STA
    {"CONFIG_STA_SUPPORT", FALSE},
    {"CONFIG_APSTA_SUPPORT", FALSE},
#endif

#ifndef INCLUDE_GREENAP_SUPPORT
    {"GREENAP_SUPPORT", FALSE},
#endif /* INCLUDE_GREENAP_SUPPORT */

#ifndef INCLUDE_SAMSUNG_SUPPORT
	{"EASY_CONFIG_SETUP", FALSE},
	{"CONFIG_SAMSUNG_REGULATORY_SUPPORT", FALSE},
	{"WAC_SUPPORT", FALSE},
	{"WAC_QOS_PRIORITY", FALSE},
#endif

#ifndef INCLUDE_SAMSUNG_STA_SUPPORT
	{"STA_EASY_CONFIG_SETUP", FALSE},
#endif

#ifndef SIGMATEL_SDK
	{"SIGMATEL_SDK", FALSE},
#endif

/* ======================= 
 *	Sort by interface 
 * ======================= */
#ifndef RTMP_USB_SUPPORT
	{"RTMP_USB_SUPPORT", FALSE},
#endif // RTMP_USB_SUPPORT // 

#ifndef RTMP_PCI_SUPPORT
	{"RTMP_PCI_SUPPORT", FALSE},
#endif // RTMP_PCI_SUPPORT // 

#ifndef RTMP_RBUS_SUPPORT    
    {"RTMP_RBUS_SUPPORT", FALSE}, 
#endif // RTMP_RBUS_SUPPORT //

/* ======================= 
 *	Sort by mac type 
 * ======================= */
#ifndef RTMP_MAC_USB
	{"RTMP_MAC_USB", FALSE}, 
#endif // RTMP_MAC_USB //

#ifndef RTMP_MAC_PCI
	{"RTMP_MAC_PCI", FALSE}, 
#endif // RTMP_MAC_PCI //

#ifndef RTMP_MAC_SDIO
	{"RTMP_MAC_SDIO", FALSE},
#endif // RTMP_MAC //

/* ======================= 
 *	Sort by OS 
 * ======================= */
#ifndef LINUX
	{"LINUX", FALSE}, 
#endif // LINUX //
#ifndef ECOS
	{"__ECOS", FALSE}, 
#endif // ECOS //
#ifndef UCOS
	{"UCOS", FALSE}, 
#endif // UCOS //
#ifndef VXWORKS
	{"VXWORKS", FALSE}, 
#endif // VXWORKS //
#ifndef THREADX
	{"THREADX", FALSE}, 
#endif // THREADX //

	{"NDIS_WINS", FALSE},
/* ======================= 
 *	Sort by chipset 
 * ======================= */
#ifndef RT2860
	{"RT2860", FALSE},
#endif

#ifndef RT2870
	{"RT2870", FALSE},
#endif

#ifndef RT30xx
    {"RT30xx", FALSE},
#endif

#ifndef RT3062
	{"RT3062", FALSE},
#endif

#ifndef RT3090
    {"RT3090", FALSE},
#endif

#ifndef RT3070
    {"RT3070", FALSE},
#endif

#ifndef RT2070
	{"RT2070", FALSE},
#endif

#ifndef RT2880
	{"RT2880", FALSE},
#endif

#ifndef RT2883
	{"RT2883", FALSE},
#endif

#ifndef RT3883
	{"RT3883", FALSE},
#endif

#ifndef RT35xx
	{"RT35xx", FALSE},
#endif

#ifndef RT305x
	{"RT305x", FALSE},
#endif

#ifndef RT3052
	{"RT3052", FALSE},
#endif

#ifndef RT33xx
	{"RT33xx", FALSE},
#endif

#ifndef RT3290
	{"RT3290", FALSE},
#endif

#ifndef RT3390
	{"RT3390", FALSE},
#endif

#ifndef RT3370
	{"RT3370", FALSE},
#endif

#ifndef RT3562
	{"RT3562", FALSE},
#endif

#ifndef RT3572
	{"RT3572", FALSE},
#endif

#ifndef RT3592
	{"RT3592", FALSE},
#endif

#ifndef RT3573
	{"RT3573", FALSE},
#endif

#ifndef RT3593
	{"RT3593", FALSE},
#endif

#ifndef RT5350
        {"RT5350", FALSE},
#endif

#ifndef RT5390
	{"RT5390", FALSE},
#endif

#ifndef RT5392
	{"RT5392", FALSE},
#endif

#ifndef RT5370
        {"RT5370", FALSE},
#endif

#ifndef RT5372
        {"RT5372", FALSE},
#endif

#ifndef RT5592
	{"RT5592", FALSE},
#endif

#ifndef RT5572
	{"RT5572", FALSE},
#endif

#ifndef RT3352
	{"RT3352", FALSE},
#endif

#ifndef RT6352
	{"RT6352", FALSE},
#endif

#ifdef CHIPSET_2070
    {"DOT11_N_SUPPORT", FALSE},
    {"DOT11N_DRAFT3", FALSE},
#endif

#ifndef RT8592
	{"RT8592", FALSE},
#endif

#ifndef RT65xx
	{"RT65xx", FALSE},
#endif

#ifndef MT7628
	{"MT7628", FALSE},
#endif

#ifndef MT7636
	{"MT7636", FALSE},
#endif

#ifndef MT7601
	{"MT7601", FALSE},
	{"MT7601U", FALSE},
	{"MT7601E", FALSE},
#endif

#ifndef MT76x0
	{"MT76x0", FALSE},
	{"MT7650", FALSE},
	{"MT7630", FALSE},
	{"MT7610", FALSE},
#endif

#ifndef MT76x2
	{"MT76x2", FALSE},
	{"MT7662", FALSE},
	{"MT7632", FALSE},
	{"MT7612", FALSE},
#endif

/* ======================= 
 *	Sort by Function
 * ======================= */
#ifndef INCLUDE_WPS
	{"WSC_INCLUDED", FALSE},
	{"WSC_STA_SUPPORT", FALSE},
	{"WSC_AP_SUPPORT", FALSE},
#endif
#ifndef INCLUDE_WPS_V2
	{"WSC_V2_SUPPORT", FALSE},
#endif
#ifndef INCLUDE_IWPS
	{"IWSC_SUPPORT", FALSE},
	{"IWSC_TEST_SUPPORT", FALSE},
#endif

#ifndef INCLUDE_HS
	{"CONFIG_HOTSPOT", FALSE},
#endif

#ifndef INCLUDE_HS2
        {"CONFIG_HOTSPOT_R2", FALSE},
#endif

#ifndef INCLUDE_DOT11U_INTERWORKING
	{"CONFIG_DOT11U_INTERWORKING", FALSE},
#endif

#ifndef INCLUDE_DOT11V_WNM
	{"CONFIG_DOT11V_WNM", FALSE},
#endif

#ifndef INCLUDE_MESH
	{"MESH_SUPPORT", FALSE},
#endif
#ifndef INCLUDE_MAT	
	{"MAT_SUPPORT", FALSE},
#endif
#ifndef INCLUDE_ETH_CONVERT
	{"ETH_CONVERT_SUPPORT", FALSE},
#endif
#ifndef INCLUDE_ADHOC_WPA2PSK
	{"ADHOC_WPA2PSK_SUPPORT", FALSE},
#endif
#ifndef INCLUDE_WAPI 
    {"WAPI_SUPPORT", FALSE},    
#endif // INCLUDE_WAPI //

#ifndef INCLUDE_WOW 
    {"WOW_SUPPORT", FALSE},
#endif // INCLUDE_WOW //  

#ifndef INCLUDE_ATE
    {"CONFIG_ATE", FALSE},
    {"CONFIG_QA", FALSE},
#endif // INCLUDE_ATE //  

#ifndef INCLUDE_TXBF
    {"TXBF_SUPPORT", FALSE},    
#endif // INCLUDE_TXBF //    

#ifndef INCLUDE_ACM  
    {"WMM_ACM_SUPPORT", FALSE},    
#endif // INCLUDE_ACM //    
#ifndef INCLUDE_11R_FT 
    {"DOT11R_FT_SUPPORT", FALSE},    
#endif // INCLUDE_11R_FT //
#ifndef INCLUDE_CONFIG_SWMCU_SUPPORT
    {"CONFIG_SWMCU_SUPPORT", FALSE},    
#endif // INCLUDE_CONFIG_SWMCU_SUPPORT //
#ifndef INCLUDE_11K_RRM 
    {"DOT11K_RRM_SUPPORT", FALSE},        
#endif // INCLUDE_11K_RRM // 

#ifndef INCLUDE_11W_PMF
	{"DOT11W_PMF_SUPPORT", FALSE},  
#endif // INCLUDE_11W_PMF //

#ifndef RTMP_RF_RW_SUPPORT
	{"RTMP_RF_RW_SUPPORT", FALSE},        
#endif // RTMP_RF_RW_SUPPORT // 
#ifndef INCLUDE_TDLS
	{"DOT11Z_TDLS_SUPPORT", FALSE},
#endif // INCLUDE_TDLS //
#ifndef INCLUDE_ANT_DIVERSITY
	{"ANT_DIVERSITY_SUPPORT", FALSE},
#endif // INCLUDE_ANT_DIVERSITY //
#ifndef INCLUDE_11V_WNM_SUPPORT
	{"DOT11V_WNM_SUPPORT", FALSE},
#endif // INCLUDE_11V_WNM_SUPPORT //
#ifndef INCLUDE_HW_COEXISTENCE_SUPPORT
	{"HW_COEXISTENCE_SUPPORT", FALSE},
#endif // INCLUDE_HW_COEXISTENCE_SUPPORT //
#ifndef INCLUDE_BT_COEXISTENCE_SUPPORT
	{"BT_COEXISTENCE_SUPPORT", FALSE},
#endif // INCLUDE_BT_COEXISTENCE_SUPPORT //

#ifndef INCLUDE_MT76XX_BT_COEXISTENCE_SUPPORT
	{"MT76XX_BTCOEX_SUPPORT", FALSE},
#endif // INCLUDE_MT76XX_BT_COEXISTENCE_SUPPORT //

#ifndef INCLUDE_CONFIG_ZTE_RADIO_ONOFF
	{"CONFIG_ZTE_RADIO_ONOFF", FALSE},
#endif // INCLUDE_CONFIG_ZTE_RADIO_ONOFF

#ifndef HUAWEI_ATE
	{"HUAWEI_ATE", FALSE},
#endif // HUAWEI_ATE //

#ifndef INCLUDE_LED_CONTROL_SUPPORT
	{"LED_CONTROL_SUPPORT", FALSE},
#endif // INCLUDE_LED_CONTROL_SUPPORT //

#ifndef INCLUDE_WIFI_LED_SHARE
	{"CONFIG_WIFI_LED_SHARE", FALSE},
#endif // INCLUDE_WIFI_LED_SHARE

#ifndef INCLUDE_WIDI
	{"WIDI_SUPPORT", FALSE},
#endif // INCLUDE_WIDI //

#ifndef INCLUDE_INTEL_WFD
	{"INTEL_WFD_SUPPORT", FALSE},
#endif // INCLUDE_INTEL_WFD //

#ifndef INCLUDE_WFA_WFD
	{"WFA_WFD_SUPPORT", FALSE},
#endif // INCLUDE_WFA_WFD //

#ifndef INCLUDE_DFS
	{"DFS_SUPPORT", FALSE},
#endif // INCLUDE_DFS //

#if defined(INCLUDE_P2P) || defined(INCLUDE_CFG_P2P)
//#ifndef CHIPSET_RBUS
//#ifdef MODE_APSTA

	{"WDS_SUPPORT", FALSE},
//#endif
//#endif
#endif // INCLUDE_P2P //

#ifndef INCLUDE_P2P
	{"P2P_SUPPORT", FALSE},
	{"P2P_APCLI_SUPPORT", FALSE},
#endif // INCLUDE_P2P //

#ifndef INCLUDE_WFD
	{"WFD_SUPPORT", FALSE},
#endif // INCLUDE_WFD //

#ifndef INCLUDE_CSO_SUPPORT
	{"CONFIG_CSO_SUPPORT", FALSE},
#endif // INCLUDE_CSO_SUPPORT //

#ifndef INCLUDE_TSO_SUPPORT
	{"CONFIG_TSO_SUPPORT", FALSE},
#endif // INCLUDE_TSO_SUPPORT //

#ifndef IQ_CAL_SUPPORT
	{"IQ_CAL_SUPPORT", FALSE},
#endif // IQ_CAL_SUPPORT // 

#ifndef RTMP_FREQ_CALIBRATION_SUPPORT
	{"RTMP_FREQ_CALIBRATION_SUPPORT", FALSE},
#endif // RTMP_FREQ_CALIBRATION_SUPPORT //

#ifndef VCORECAL_SUPPORT
	{"VCORECAL_SUPPORT", FALSE},
#endif // VCORECAL_SUPPORT //

#ifndef SPECIFIC_BCN_BUF_SUPPORT
	{"SPECIFIC_BCN_BUF_SUPPORT", FALSE},
#endif // SPECIFIC_BCN_BUF_SUPPORT //

#ifndef INCLUDE_SMART_ANTENNA
	{"SMART_ANTENNA", FALSE},
#endif // INCLUDE_SMART_ANTENNA //

#ifndef INCLUDE_TXBF_SUPPORT
	{"TXBF_SUPPORT", FALSE},
#endif // INCLUDE_TXBF_SUPPORT //

#ifndef INCLUDE_MAC_REPEATER_SUPPORT
        {"MAC_REPEATER_SUPPORT", FALSE},
#endif // INCLUDE_MAC_REPEATER_SUPPORT //

#ifndef INCLUDE_FLASH
	{"RTMP_FLASH_SUPPORT", FALSE},
#endif // INCLUDE_FLASH //

/* ======================= 
 *	Sort by mcu
 * ======================= */
#ifndef INCLUDE_M8051_SUPPORT
	{"CONFIG_M8051_SUPPORT", FALSE},
#endif

#ifndef INCLUDE_ANDES_SUPPORT
	{"CONFIG_ANDES_SUPPORT", FALSE},
#endif

/* ======================= 
 *	Sort by company
 * ======================= */
#ifndef INCLUDE_SAMSUNG_SUPPORT
	{"DPA_S", FALSE},
#endif

#ifndef INCLUDE_TOSHIBA_SUPPORT
	{"DPA_T", FALSE},
#endif

};

void	fnCommStrip(char *pBuildDirectory, char *pReleasePath);
int		fnStripModule();
#ifdef OS_ABL_SUPPORT
int		fnStripUtil(void);
int 	fnStripNetif(void);
#endif // OS_ABL_SUPPORT //

