#include <striptool.h>

#define BUILD_MODULE_DIRECTORY "build"
#define BUILD_CMD_DIRECTORY "build_cmd"
/* 	
	TDLS is general function in STA mode except DPO/STA.

	For AP release :
		DPA - basic function + WPS.
		DPB - basic function + WPS + Mesh + Gemtek proprietary function.
		DPC - basic function + WPS + Mesh.
		DPI - basic function + WPS. Exclude IDS, WAPI.
		
	For STA release :
		DPA: WPS + ACM + P2P
		DPAW: WPS + ACM + P2P + Intel WIDI
		DPB: Full function ¡V WPS + Ethernet Convert + Mesh + ACM + P2P
		DPC: WPS + Mesh + ACM + P2P
		DPD: WPS + Ethernet Convert + ACM + P2P
		DPO: Without WPS, Ethernet Convert, Mesh
		
	For Arch release :
		1.	AP + STA mode
		2. 	without ACM, 11R_FT, 11K_RRM.
		note : In MakeFile, 
				CHIPSET = SOC
				RELEASE = ARCH

	For Samsung release :
		DPA_S: DPA + Auto Provision + Modifications for Samsung

	For Toshiba release :
		DPA_T: DPA + Modifications for Toshiba
	
 */

static char *RELEASE_DIRECTORY[] = {
	"chips",
	"common",
	"conf",
	"doc",
	"rate_ctrl",
	"hw_ctrl",
	"hif",
	"mac",
	"mcu",
	"mcu/bin",
	"eeprom",
	"mgmt",
	"naf",
	"phy",
	"tx_rx",
	"include",
	"include/os",
	"include/chip",
	"include/iface",
    "include/mac",

    // TODO: strip nmac, omac, mac_mt
    "include/mac/mac_ral",
    "include/mac/mac_ral/nmac",
    "include/mac/mac_ral/omac",
    "include/mac/mac_mt",
	"include/eeprom",
	"include/mcu",
	"include/phy",
#ifdef CONFIG_FPGA_MODE
	"include/fpga",
#endif /* CONFIG_FPGA_MODE */
#ifdef CONFIG_SNIFFER_SUPPORT
	"include/sniffer",
	"sniffer",
#endif /* CONFIG_SNIFFER_SUPPORT */
	"os",
#ifdef INCLUDE_ATE
	"ate",
	"ate/common",
	"ate/include",
	"ate/mt_mac",
	"ate/inf",
#endif
#ifdef LINUX
	"os/linux",
#ifdef INCLUDE_CFG80211_SUPPORT
	"os/linux/cfg80211",
#endif /* INCLUDE_CFG80211_SUPPORT */
#endif
#ifdef VXWORKS
	"os/vxworks",
#endif
#ifdef ECOS
	"os/ecos",
#endif
#ifdef INCLUDE_AP
	"ap",
#endif
#ifdef INCLUDE_STA
	"sta",
#endif
#ifdef LINUX	
	"tools",
	"tools/plug_in",
#endif
};

/*
 * - Sort by interface 
 * - Sort by mcu
 * - Sort by mac type 
 * - Sort by chipset 
 * - Sort by OS 
 * - Sort by Function
 */
static char *EXCLUDE_FILE[] = {
/* ======================= 
 *	Sort by interface 
 * ======================= */	 
#ifndef RTMP_USB_SUPPORT
	"include/iface/rtmp_usb.h", 
    #ifdef LINUX
	    "os/linux/usb_main_dev.c",
    #endif // LINUX // 	
#endif // RTMP_USB_SUPPORT // 

#ifndef RTMP_PCI_SUPPORT
	"include/iface/rtmp_pci.h",  
    #ifdef LINUX
	    "os/linux/pci_main_dev.c",
    #endif // LINUX //
	    "hif/hif_pci.c",
#endif // RTMP_PCI_SUPPORT // 

#ifndef RTMP_RBUS_SUPPORT    
    "include/iface/rtmp_rbs.h", 
#ifndef RTMP_FLASH_SUPPORT 
    "common/ee_flash.c",
#endif // RTMP_FLASH_SUPPORT //
#ifndef RT35xx
	"common/dfs_mcu.c",	
#endif // RT35xx //
	"common/rtmp_swmcu.c",
    #ifdef LINUX    
        "os/linux/rbus_main_dev.c", 
    #endif // LINUX //   
#endif // RTMP_RBUS_SUPPORT //

/* ======================= 
 *	Sort by mac type 
 * ======================= */
#ifndef RTMP_MAC_USB
	"common/rt2870.bin.gpio",
	"common/rt2870.bin.host_status",
	"common/rt2870_wow.bin",
	"common/rt2870.bin",
	"common/cmm_data_usb.c",
	"common/cmm_mac_usb.c",
	"common/rtusb_dev_id.c",
	"common/rtusb_bulk.c",
	"common/rtusb_data.c",
	"common/rtusb_io.c",		
	"include/chip/mac_usb.h",
	"include/mac_ral/mac_usb.h",
	"include/rtusb_io.h",	
	"ate/common/ate_usb.c",
    #ifdef LINUX
	    "os/linux/rt_usb.c",
	    "os/linux/rt_usb_util.c",
    #endif // LINUX //
#endif // RTMP_MAC_USB //

#ifdef RTMP_MAC_USB
#ifndef INCLUDE_WOW
#ifndef INCLUDE_WOW_GPIO
	"common/rt2870.bin.gpio",
#endif // INCLUDE_WOW_GPIO //
#ifndef INCLUDE_WOW_HOST_STATUS
	"common/rt2870.bin.host_status",
#endif // INCLUDE_WOW_HOST_STATUS //
	"common/rt2870_wow.bin",
#endif // INCLUDE_WOW //
#endif // RTMP_MAC_USB //

#ifndef RTMP_MAC_PCI
	"common/rt2860.bin",
	"common/cmm_data_pci.c",
	"common/cmm_mac_pci.c",
	"include/chip/mac_pci.h",	
	"include/mac/mac_ral/mac_pci.h",
	"ate/common/ate_pci.c",
    #ifdef LINUX
	    "os/linux/rt_pci_rbus.c",
	    "os/linux/rt_rbus_pci_util.c",
    #endif // LINUX //	
#endif // RTMP_MAC_PCI //

/* ======================= 
 *	Sort by chipset 
 * ======================= */
#ifndef RT30xx 
	"include/chip/rt30xx.h",
	"chips/rt30xx.c",
	"ate/chips/rt30xx_ate.c",
#endif // RT30xx //

#ifndef RT35xx 
	"include/chip/rt35xx.h",
	"chips/rt35xx.c",
	"ate/chips/rt35xx_ate.c",
#endif // RT35xx //

#ifndef RT305x 
	"include/chip/rt305x.h",
	"chips/rt305x.c",
	"ate/chips/rt305x_ate.c",
#endif // RT305x //

#ifndef RT33xx 
	"include/chip/rt33xx.h",
	"chips/rt33xx.c",
	"ate/chips/rt33xx_ate.c",
#endif // RT33xx //

#ifndef RT2860 
	"include/chip/rt2860.h",
#endif // RT2860 //
#ifndef RT2870 
	"include/chip/rt2870.h",
#endif // RT2870 //

#ifndef RT2880 
	"include/chip/rt2880.h",
	"chips/rt2880.c",
	"os/linux/Makefile.2880.ap",
	"os/linux/Makefile.2880.sta",
	"os/linux/Makefile.release.2880",
#endif // RT2880 //

#ifndef RT2883 
	"include/chip/rt2883.h",
	"chips/rt2883.c",
	"ate/chips/rt2883_ate.c",
#endif // RT2883 //

#ifndef RT3883 
	"include/chip/rt3883.h",
	"chips/rt3883.c",
	"ate/chips/rt3883_ate.c",
#endif // RT3883 //

#ifndef RT3070 
	"include/chip/rt3070.h",
	"chips/rt3070.c",
#endif // RT3070 //

#ifndef RT3090 
	"include/chip/rt3090.h",
	"chips/rt3090.c",
#endif // RT3090 //

#ifndef RT3370 
	"include/chip/rt3370.h",
	"chips/rt3370.c",
#endif // RT3370 //

#ifndef RT3390 
	"include/chip/rt3390.h",
	"chips/rt3390.c",
#endif // RT3390 //

#ifndef RT3593 
	"include/chip/rt3593.h",
	"chips/rt3593.c",
	"ate/chips/rt3593_ate.c",
#endif // RT3593 //

#ifndef RT5350
	"include/chip/rt5350.h",
	"chips/rt5350.c",
	"ate/chips/rt5350_ate.c",
#endif // RT5350 //

#ifndef RT3352
	"include/chip/rt3352.h",
	"chips/rt3352.c",
	"ate/chips/rt3352_ate.c",
#endif // RT3352 //

#if !defined(RT5370) && !defined(RT5372) && !defined(RT5390) && !defined(RT5392)
	"include/chip/rt5390.h",
	"chips/rt5390.c",
	"ate/chips/rt5390_ate.c",
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) */

#ifndef RT5592
	"include/chip/rt5592.h",
	"chips/rt5592.c",
	"ate/chips/rt5592_ate.c",
#endif

#ifndef RT28xx
	"include/chip/rt28xx.h",
#endif

#ifndef RT3290
	"include/chip/rt3290.h",
	"chips/rt3290.c",
#endif

#ifndef RT6352
	"include/chip/rt6352.h",
	"chips/rt6352.c",
	"ate/chips/rt6352_ate.c",
#endif

#ifndef RT6590
	"chips/rt6592.c",
#endif

#ifndef MT7601
	"include/chip/mt7601.h",
	"chips/mt7601.c",
	"ate/chips/mt7601_ate.c",
	"mcu/bin/MT7601.bin",
#endif

#ifndef RT8592
	"chips/rt85592.c",
#endif

#ifndef RT65xx
	"include/chip/rt65xx.h"
#endif

#ifndef MT76x0
	"include/chip/mt76x0.h",
	"chips/mt76x0.c",
#endif

#if !defined(MT7650) && !defined(MT7630)
	"include/mcu/mt7650_firmware.h",
#endif

#ifndef MT7610
	"include/mcu/mt7610_firmware.h",
#endif

#ifndef MT76x2
	"include/chip/mt76x2.h",
	"chips/mt76x2.c",
#endif

#if !defined(MT7662) && !defined(MT7632)
	"include/mcu/mt7662_firmware.h",
#endif

#ifndef MT7612
	"include/mcu/mt7612_firmware.h",
#endif

#ifndef RLT_MAC
	"mac/mac_ral/nmac/ral_nmac.h",
	"mac/mac_ral/nmac/ral_nmac_pbf.h",
	"mac/mac_ral/nmac/ral_nmac_pci.h",
#endif

#ifndef RTMP_MAC
	"mac/mac_ral/omac/ral_omac.h",
	"mac/mac_ral/omac/ral_omac_pbf.h",
	"mac/mac_ral/omac/ral_omac_pci.h",
	"mac/mac_ral/omac/ral_omac_rf_ctrl.h",
#endif

/* ======================= 
 *	Sort by OS 
 * ======================= */
#ifndef LINUX
	"include/os/rt_linux.h",
	"include/os/rt_linux_cmm.h",
#endif // LINUX //
#ifndef ECOS
	"include/os/rt_ecos.h",
	"include/os/rt_ecos_type.h",
#endif // ECOS //
#ifndef UCOS
	"include/os/rt_ucos.h",
#endif // UCOS //
#ifndef VXWORKS
	"include/os/rt_vxworks.h",
#endif // VXWORKS //
#ifndef THREADX
	"include/os/rt_threadx.h",
#endif // THREADX //
	"include/os/rt_win.h",
	"include/os/rt_wince.h",

/* ======================= 
 *	Sort by Function
 * ======================= */
#ifndef RTMP_EFUSE_SUPPORT 
	"common/ee_efuse.c",
#endif // RTMP_EFUSE_SUPPORT //
 
#ifndef INCLUDE_WAPI
	"include/wapi_sms4.h",
    "include/wapi_def.h",
	"include/wapi.h",    
	"common/wapi.c",
	"os/linux/Makefile.libwapi.4",
	"os/linux/Makefile.libwapi.6",	 
#endif // INCLUDE_WAPI //    
    "common/wapi_sms4.c",
    "common/wapi_crypt.c",    

	"include/dh_key.h",    
    "include/evp_enc.h",  
	"common/dh_key.c",
	"common/evp_enc.c",    	    
#ifndef INCLUDE_WPS
    "include/crypt_biginteger.h",    
    "include/crypt_dh.h",
    "include/wsc_tlv.h",
    "common/crypt_biginteger.c",
    "common/crypt_dh.c",
    "common/wsc.c",
    "common/wsc_tlv.c",
#endif // INCLUDE_WPS //
#ifndef INCLUDE_WPS_V2
    "common/wsc_v2.c",
#endif // INCLUDE_WPS_V2 //

#ifndef INCLUDE_IWPS 
    "sta/sta_iwsc.c",
#endif // INCLUDE_IWPS //

#ifndef INCLUDE_DFS 
    "common/cmm_dfs.c",
#endif // INCLUDE_DFS //

#ifndef INCLUDE_ADHOC_WPA2PSK
    "common/cmm_wpa_adhoc.c",
#endif // INCLUDE_ADHOC_WPA2PSK //

#ifndef INCLUDE_MAT
    "include/mat.h",
    "common/cmm_mat.c",
    "common/cmm_mat_iparp.c",
    "common/cmm_mat_ipv6.c",
    "common/cmm_mat_pppoe.c",    
#endif // INCLUDE_MAT //

#ifndef INCLUDE_MESH
    "include/mesh.h",
    "include/mesh_def.h",
    "include/mesh_mlme.h",
    "include/mesh_sanity.h",
	"common/mesh.c",
    "common/mesh_bmpkt.c",
    "common/mesh_ctrl.c",
	"common/mesh_forwarding.c",    
    "common/mesh_link_mng.c",
    "common/mesh_path_mng.c",    
    "common/mesh_sanity.c",    
    "common/mesh_tlv.c",            
    "common/mesh_inf.c",            
#endif // INCLUDE_MESH //

#ifndef INCLUDE_ACM
    "include/acm_comm.h",
    "include/acm_edca.h",
	"include/acm_extr.h",
    "common/acm_comm.c",
    "common/acm_edca.c",
    "common/acm_iocl.c",
#endif // INCLUDE_ACM //

#ifndef INCLUDE_11R_FT
	"include/ft.h",
	"include/ft_cmm.h",
	"common/ft.c",
	"common/ft_iocl.c",
	"common/ft_rc.c",
	"common/ft_tlv.c",	
#endif // INCLUDE_11R_FT //

#ifndef INCLUDE_11K_RRM
	"include/dot11k_rrm.h",
	"include/rrm.h",
	"include/rrm_cmm.h",	
	"common/rrm.c",
	"common/rrm_sanity.c",
	"common/rrm_tlv.c",
#endif // INCLUDE_11R_FT //

#ifndef INCLUDE_TDLS
	"include/dot11z_tdls.h",
	"include/tdls.h",
	"include/tdls_cmm.h",
	"include/tdls_uapsd.h",
#endif // INCLUDE_TDLS //

#if !defined(INCLUDE_TDLS) && !defined(INCLUDE_11R_FT)
	"include/dot11r_ft.h",
#endif

#ifndef INCLUDE_11W_PMF
	"include/dot11w_pmf.h",
	"include/pmf.h",
	"include/pmf_cmm.h",
	"common/pmf.c",
#endif // INCLUDE_11W_PMF //

#ifndef INCLUDE_11V_WNM_SUPPORT
	"common/wnm.c",
	"common/wnm_tlv.c",
	"include/dot11v_wnm.h",
	"include/wnm.h",
	"include/wnm_cmm.h",
#endif // INCLUDE_11V_WNM_SUPPORT //

#ifndef INCLUDE_P2P
	"common/p2p_inf.c",
	"common/p2p_dbg.c",
    	"common/p2p.c",
    	"common/p2p_packet.c",
    	"common/p2p_action.c",
    	"common/p2p_table.c",
    	"common/p2p_ctrl.c",
    	"common/p2p_disc_mng.c",
    	"common/p2p_nego_mng.c",
    	"common/p2p_dbg.c",
	"common/p2p.c",
	"common/p2p_packet.c",
	"common/p2p_action.c",
	"common/p2p_table.c",
	"common/p2p_ctrl.c",
	"common/p2p_disc_mng.c",
	"common/p2p_nego_mng.c",
	"common/p2p_cfg.c",
	"sta/p2pcli.c",
	"sta/p2pcli_ctrl.c",
	"sta/p2pcli_sync.c",
	"sta/p2pcli_auth.c",
	"sta/p2pcli_assoc.c",
	"include/p2p.h",
	"include/p2p_cmm.h",
	"include/p2p_inf.h",
	"include/p2pcli.h",
//	"include/wfa_p2p.h",
#endif // INCLUDE_P2P //

#ifndef INCLUDE_MT76XX_BT_COEXISTENCE_SUPPORT
	"mcu/bt_coex.c",
#endif // INCLUDE_MT76XX_BT_COEXISTENCE_SUPPORT //

#ifndef INCLUDE_WIDI
	"common/l2sd_ta_mod.c",
	"include/l2sd_ta.h",
#endif // INCLUDE_WIDI //

#ifndef LINUX    
    "include/rt_linux.h",
#endif

#ifndef INCLUDE_SAMSUNG_SUPPORT
	"common/auto_provision.c",
#endif // INCLUDE_SAMSUNG_SUPPORT //

#ifndef LED_CONTROL_SUPPORT
	"common/rt_led.c",	
	"include/rt_led.h",	
#endif // LED_CONTROL_SUPPORT //

#ifndef INCLUDE_SMART_ANTENNA
	"common/smartant.c",
	"include/smartant.h",
#endif // INCLUDE_SMART_ANTENNA //

#ifndef TXBF_SUPPORT
	"common/cmm_txbf.c",
	"common/cmm_txbf_cal.c",
	"include/rt_txbf.h"
#endif // TXBF_SUPPORT //

#ifndef INCLUDE_AP  
	"RT2860AP.dat",              
	"common/igmp_snoop.c",
	"common/rt2860.bin.dfs",
	"common/wsc_ufd.c",
	"include/ap_apcli.h",
	"include/ap_autoChSel.h",
	"include/ap_autoChSel_cmm.h",
	"include/ap_cfg.h",
	"include/ap_ids.h",
	"include/ap_mbss.h",
	"include/ap_uapsd.h",
	"include/ap_wds.h",
	"include/igmp_snoop.h",
    #ifdef LINUX    
        "os/linux/ap_ioctl.c",     
    #endif // LINUX //
#else
    #ifndef INCLUDE_11R_FT
	    "ap/ap_ftkd.c",
    	"ap/ap_ftrc.c",
    #endif // INCLUDE_11R_FT //
    #ifndef INCLUDE_NINTENDO_AP
    	"ap/ap_nintendo.c",
    #endif // INCLUDE_NINTENDO_AP //
#ifdef INCLUDE_P2P
#ifdef MODE_APSTA
	"include/ap_wds.h",
#endif
#endif
#endif // INCLUDE_AP //

#ifndef INCLUDE_STA   
	"RT2860STA.dat",      
    #ifdef LINUX    
        "os/linux/sta_ioctl.c",
    #endif // LINUX //    
#else
    #ifndef INCLUDE_11R_FT
	    "sta/ft_action.c",
    	"sta/ft_auth.c",
    #endif // INCLUDE_11R_FT //
	#ifndef INCLUDE_TDLS
		"sta/tdls.c",
    	"sta/tdls_ctrl.c",
		"sta/tdls_link_mng.c",
		"sta/tdls_tlv.c",
		"sta/tdls_chswitch_mng.c",
		"sta/tdls_uapsd.c",		
	#endif // INCLUDE_TDLS //
	#if !defined(INCLUDE_ETH_CONVERT) && !defined(INCLUDE_P2P)
		"include/ipv6.h",
	#endif // INCLUDE_ETH_CONVERT //
	#ifndef INCLUDE_SAMSUNG_STA_SUPPORT
		"sta/sta_auto_provision.c",
	#endif // INCLUDE_SAMSUNG_STA_SUPPORT //
		"os/linux/load",
		"os/linux/unload",
#endif // INCLUDE_STA //

#ifdef INCLUDE_P2P
    	"ap/ap_apcli.c",
    	"ap/ap_apcli_inf.c",
    	"ap/apcli_assoc.c",
    	"ap/apcli_auth.c",
    	"ap/apcli_ctrl.c",
    	"ap/apcli_sync.c",
#ifdef MODE_APSTA
	"ap/ap_wds.c",
	"ap/ap_cfg.c",
#endif
#endif

#ifdef LINUX
#ifdef OS_ABL_SUPPORT
	"include/iface/iface_util.h",
	"include/rtmp_type.h",
	"include/rtmp_os.h",
	"include/link_list.h",
	"include/rtmp_cmd.h",
	"include/rtmp_comm.h",
	"include/rt_os_util.h",
	"include/rtmp_osabl.h",

	"common/rt_os_util.c",
	"os/linux/rt_linux_symb.c",
	"os/linux/rt_linux.c",
	"os/linux/rt_usb_util.c",
	"os/linux/rt_rbus_pci_util.c",
	"os/linux/vr_bdlt.c",
	"os/linux/br_ftph.c",
	"os/linux/Makefile.6.util",
	"os/linux/Makefile.4.util",

	"ap/ap_mbss_inf.c",
	"os/linux/ap_ioctl.c",
	"os/linux/sta_ioctl.c",
	"os/linux/rt_main_dev.c",
	"ap/ap_wds_inf.c",
	"ap/ap_apcli_inf.c",
	"common/mesh_inf.c",
	"common/p2p_inf.c",
	"common/rtusb_dev_id.c",
	"os/linux/usb_main_dev.c",
	"os/linux/rt_pci_rbus.c",
	"os/linux/pci_main_dev.c",
	"os/linux/cfg80211.c",
	"os/linux/Makefile.6.netif",
	"os/linux/Makefile.4.netif",
#endif // OS_ABL_SUPPORT //
#endif // LINUX //

/* ======================= 
 *	Sort by mcu
 * ======================= */
#ifndef INCLUDE_M8051_SUPPORT
	"include/mcu/mcu_51.h",
	"mcu/mcu_51.c",
#endif

#ifndef INCLUDE_ANDES_SUPPORT
	"include/mcu/mcu_and.h",
	"mcu/mcu_and.c",
#endif
};

static char *RELEASE_ROOT_FILE[] = {
	"Makefile",
	"Kconfig",
	"Kconfig.mt_wifi",
#ifdef INCLUDE_AP	
	"doc/History.txt",
#ifdef RTMP_MAC_PCI
	"conf/RT2860AP.dat",
	"conf/RT2860APCard.dat",
#endif
#ifdef RTMP_MAC_USB
	"conf/RT2870AP.dat",
	"conf/RT2870APCard.dat",
#endif
#endif // INCLUDE_AP //
#ifdef INCLUDE_STA
#ifdef RTMP_MAC_USB
	"conf/RT2870STA.dat",
	"conf/RT2870STACard.dat",
	"doc/README_STA_usb",
#endif // RTMP_MAC_USB //
#ifdef RTMP_MAC_PCI
	"conf/RT2860STA.dat",
	"conf/RT2860STACard.dat",
	"doc/README_STA_pci",
#endif // RTMP_MAC_PCI //
#ifdef INCLUDE_WPS
	"doc/wps_iwpriv_usage.txt",
#endif // INCLUDE_WPS //
	"doc/iwpriv_usage.txt",
	"doc/sta_ate_iwpriv_usage.txt",
#endif // INCLUDE_STA //
};

static char *RELEASE_FILE[] = {
        "common/cmm_profile.c",
        "common/cmm_info.c",
};

int fnStripModule()
{
    char Command[255];
    int index;

    /* create directories */
    sprintf(Command, "rm -Rf %s", BUILD_MODULE_DIRECTORY);
    system(Command);

    sprintf(Command, "mkdir %s", BUILD_MODULE_DIRECTORY);
    system(Command);

#ifdef RELEASE_ARCH    
    sprintf(Command, "cp -f * %s/", BUILD_MODULE_DIRECTORY); //Only copy files
    system(Command);  
#else    
    for (index = 0; index < sizeof(RELEASE_ROOT_FILE)/sizeof(char *);index++)
    {        
        sprintf(Command, "cp -f %s %s/", RELEASE_ROOT_FILE[index], BUILD_MODULE_DIRECTORY);
		//printf("%s\n", Command);
        system(Command);
    } /* End of for */
#endif
   
    for (index = 0; index < sizeof(RELEASE_DIRECTORY)/sizeof(char *);index++)
    {
        sprintf(Command, "mkdir %s/%s", BUILD_MODULE_DIRECTORY, RELEASE_DIRECTORY[index]);
        system(Command);
        sprintf(Command, "cp -f %s/* %s/%s", RELEASE_DIRECTORY[index], BUILD_MODULE_DIRECTORY, RELEASE_DIRECTORY[index]);
		// printf("%s\n", Command);
        system(Command);
    } /* End of for */

#ifdef INCLUDE_P2P
#ifdef OS_ABL_SUPPORT
    /* copy file */
    for (index = 0; index < sizeof(RELEASE_FILE)/sizeof(char *);index++)
    {
        sprintf(Command, "cp -f %s%s %s/%s", RELEASE_CMD_PATH, RELEASE_FILE[index], BUILD_MODULE_DIRECTORY, RELEASE_FILE[index]);
        system(Command);
    } /* End of for */
#endif /* OS_ABL_SUPPORT */
#endif /* INCLUDE_P2P */

    /* remove all CVS directory */
    sprintf(Command, "find %s -name CVS -prune -exec rm -rf {} \\;", BUILD_MODULE_DIRECTORY);
    system(Command);

    /* remove file */
    for (index = 0; index < sizeof(EXCLUDE_FILE)/sizeof(char *);index++)
    {
        sprintf(Command, "rm -f %s/%s", BUILD_MODULE_DIRECTORY, EXCLUDE_FILE[index]);
        system(Command);
    } /* End of for */

#ifdef RTMP_MAC_USB
#ifdef INCLUDE_WOW
#ifdef INCLUDE_WOW_GPIO
    system("mv ./build/common/rt2870.bin.gpio ./build/common/rt2870_wow.bin");
#elif defined(INCLUDE_WOW_HOST_STATUS)
    system("mv ./build/common/rt2870.bin.host_status ./build/common/rt2870_wow.bin");
#endif /* INCLUDE_WOW_GPIO */
#endif /* INCLUDE_WOW */
#endif /* RTMP_MAC_USB */

#ifdef OS_ABL_SUPPORT
//	sprintf(Command, "cp -f Makefile.module %s/Makefile", BUILD_MODULE_DIRECTORY);
//	system(Command);

	fnCommStrip(BUILD_MODULE_DIRECTORY, RELEASE_MODULE_PATH);

#else
	fnCommStrip(BUILD_MODULE_DIRECTORY, RELEASE_PATH);
#endif // OS_ABL_SUPPORT //

remove_build:
    sprintf(Command, "rm -Rf %s", BUILD_MODULE_DIRECTORY);
    system(Command);                
    sprintf(Command, "rm -Rf %s", BUILD_CMD_DIRECTORY);
    system(Command);
#ifdef INCLUDE_P2P
#ifdef OS_ABL_SUPPORT
    sprintf(Command, "rm -Rf %s", RELEASE_CMD_PATH);
    system(Command); 
#endif /* OS_ABL_SUPPORT */   
#endif
    return 0;
} /* End of main */

