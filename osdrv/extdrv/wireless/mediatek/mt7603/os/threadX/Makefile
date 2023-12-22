include $(RT28xx_DIR)/os/ThreadX/config.mk

#ifdef CONFIG_AP_SUPPORT
rt$(CHIPSET)ap-objs := \
	$(RT28xx_DIR)/common/crypt_md5.o\
	$(RT28xx_DIR)/common/crypt_sha2.o\
	$(RT28xx_DIR)/common/crypt_hmac.o\
	$(RT28xx_DIR)/common/crypt_aes.o\
	$(RT28xx_DIR)/common/crypt_arc4.o\
	$(RT28xx_DIR)/common/mlme.o\
	$(RT28xx_DIR)/common/cmm_wep.o\
	$(RT28xx_DIR)/common/action.o\
	$(RT28xx_DIR)/common/cmm_data.o\
	$(RT28xx_DIR)/common/rtmp_init.o\
	$(RT28xx_DIR)/common/cmm_tkip.o\
	$(RT28xx_DIR)/common/cmm_aes.o\
	$(RT28xx_DIR)/common/cmm_sync.o\
	$(RT28xx_DIR)/common/eeprom.o\
	$(RT28xx_DIR)/common/cmm_sanity.o\
	$(RT28xx_DIR)/common/cmm_info.o\
	$(RT28xx_DIR)/common/cmm_cfg.o\
	$(RT28xx_DIR)/common/cmm_wpa.o\
	$(RT28xx_DIR)/common/dfs.o\
	$(RT28xx_DIR)/common/spectrum.o\
	$(RT28xx_DIR)/common/rtmp_timer.o\
	$(RT28xx_DIR)/common/rt_channel.o\
	$(RT28xx_DIR)/common/cmm_profile.o\
	$(RT28xx_DIR)/common/cmm_asic.o\
	$(RT28xx_DIR)/common/cmm_cmd.o\
	$(RT28xx_DIR)/common/rtmp_init_inf.o\
	$(RT28xx_DIR)/common/rt_os_util.o\
	$(RT28xx_DIR)/chips/rtmp_chip.o\
	$(RT28xx_DIR)/ap/ap.o\
	$(RT28xx_DIR)/ap/ap_assoc.o\
	$(RT28xx_DIR)/ap/ap_auth.o\
	$(RT28xx_DIR)/ap/ap_connect.o\
	$(RT28xx_DIR)/ap/ap_mlme.o\
	$(RT28xx_DIR)/ap/ap_sanity.o\
	$(RT28xx_DIR)/ap/ap_sync.o\
	$(RT28xx_DIR)/ap/ap_wpa.o\
	$(RT28xx_DIR)/ap/ap_data.o\
	$(RT28xx_DIR)/ap/ap_uapsd.o\
	$(RT28xx_DIR)/ap/ap_mbss.o\
	$(RT28xx_DIR)/os/ThreadX/rt_threadx.o\
	$(RT28xx_DIR)/os/ThreadX/rt_ebs_rtip.o\
	$(RT28xx_DIR)/os/ThreadX/rt_profile.o\
	$(RT28xx_DIR)/os/ThreadX/rt_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/net_task.o\
	$(RT28xx_DIR)/ap/ap_autoChSel.o\
	$(RT28xx_DIR)/ap/ap_qload.o\
	$(RT28xx_DIR)/ap/ap_cfg.o

#ifdef DOT11_N_SUPPORT
ifeq ($(HAS_DOT11_N_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/common/ba_action.o
endif
#endif // DOT11_N_SUPPORT //

#ifdef BG_FT_SUPPORT
ifeq ($(HAS_BGFP_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/common/br_ftph.o
endif
#endif // BG_FT_SUPPORT //

#ifdef RT2860
ifeq ($(CHIPSET),2860)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o
endif
#endif // RT2860 //

#ifdef RT3090
ifeq ($(CHIPSET),3090)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt3090.o
endif
#endif // RT3090 //

#ifdef RT2870
ifeq ($(CHIPSET),2870)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o
endif
#endif // RT2870 //

#ifdef RT3070
ifeq ($(CHIPSET),2070)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt3070.o
endif

ifeq ($(CHIPSET),3070)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt3070.o
endif
#endif // RT3070 //

#ifdef RT2880
ifeq ($(CHIPSET),2880)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/ee_flash.o\
	$(RT28xx_DIR)/common/led.o\
	$(RT28xx_DIR)/common/rtmp_swmcu.o\
	$(RT28xx_DIR)/chips/rt2880.o

ifeq ($(HAS_DFS_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/common/dfs_mcu.o
endif
endif
#endif // RT2880 //

#ifdef RT35xx
ifeq ($(CHIPSET),3572)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt35xx.o
endif

ifeq ($(CHIPSET),3062)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt35xx.o
endif

ifeq ($(CHIPSET),3562)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt35xx.o
endif
#endif // RT35xx //

#ifdef RT3390
ifeq ($(CHIPSET),3390)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt33xx.o\
	$(RT28xx_DIR)/chips/rt3390.o
endif
#endif // RT3390 //

#ifdef RT3370
ifeq ($(CHIPSET),3370)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt33xx.o\
	$(RT28xx_DIR)/chips/rt3370.o\
	$(RT28xx_DIR)/chips/rt3070.o
endif
#endif // RT3370 //

ifeq ($(HAS_RT2880_RT2860_COEXIST),y)	
RT28XX_AP_OBJ += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/common/dfs.o
endif

ifeq ($(HAS_ATE),y)
rt$(CHIPSET)ap-objs += $(RT28xx_DIR)/common/rt_ate.o
endif

#ifdef WSC_INCLUDED
ifeq ($(HAS_WSC),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/common/wsc.o\
	$(RT28xx_DIR)/common/wsc_tlv.o\
	$(RT28xx_DIR)/common/wsc_ufd.o\
	$(RT28xx_DIR)/common/crypt_biginteger.o\
	$(RT28xx_DIR)/common/crypt_dh.o
endif
#endif // WSC_INCLUDED //

#ifdef NINTENDO_AP
ifeq ($(HAS_NINTENDO),y)
rt$(CHIPSET)ap-objs += $(RT28xx_DIR)/ap/ap_nintendo.o
endif
#endif // NINTENDO_AP //

ifeq ($(HAS_WDS),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/ap/ap_wds.o \
	$(RT28xx_DIR)/ap/ap_wds_inf.o
endif

#ifdef APCLI_SUPPORT
ifeq ($(HAS_APCLI),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/ap/ap_apcli.o \
	$(RT28xx_DIR)/ap/ap_apcli_inf.o \
	$(RT28xx_DIR)/ap/apcli_ctrl.o \
	$(RT28xx_DIR)/ap/apcli_sync.o \
	$(RT28xx_DIR)/ap/apcli_auth.o \
	$(RT28xx_DIR)/ap/apcli_assoc.o \
	$(RT28xx_DIR)/common/cmm_mat.o \
	$(RT28xx_DIR)/common/cmm_mat_iparp.o \
	$(RT28xx_DIR)/common/cmm_mat_pppoe.o \
	$(RT28xx_DIR)/common/cmm_mat_ipv6.o
endif
#endif // APCLI_SUPPORT //

ifeq ($(HAS_BLOCK_NET_IF),y)
rt$(CHIPSET)ap-objs += $(RT28xx_DIR)/common/netif_block.o
endif

ifeq ($(HAS_IGMP_SNOOP_SUPPORT),y)
rt$(CHIPSET)ap-objs += $(RT28xx_DIR)/common/igmp_snoop.o
endif

#ifdef WMM_ACM_SUPPORT
ifeq ($(HAS_WMM_ACM_SUPPORT),y)
rt$(CHIPSET)ap-objs += $(RT28xx_DIR)/common/acm_edca.o\
	$(RT28xx_DIR)/common/acm_comm.o\
	$(RT28xx_DIR)/common/acm_iocl.o
endif
#endif // WMM_ACM_SUPPORT //

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
rt$(CHIPSET)ap-objs += $(RT28xx_DIR)/ap/ap_dls.o
endif

ifeq ($(HAS_IDS_SUPPORT),y)
rt$(CHIPSET)ap-objs += $(RT28xx_DIR)/ap/ap_ids.o
endif

ifeq ($(PLATFORM),IKANOS_V160)
rt$(CHIPSET)ap-objs += $(RT28xx_DIR)/os/ThreadX/vr_ikans.o
endif

ifeq ($(PLATFORM),IKANOS_V180)
rt$(CHIPSET)ap-objs += $(RT28xx_DIR)/os/ThreadX/vr_ikans.o
endif

#ifdef MESH_SUPPORT
ifeq ($(HAS_MESH_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/common/mesh_bmpkt.o \
	$(RT28xx_DIR)/common/mesh_ctrl.o \
	$(RT28xx_DIR)/common/mesh_link_mng.o \
	$(RT28xx_DIR)/common/mesh_sanity.o \
	$(RT28xx_DIR)/common/mesh_tlv.o \
	$(RT28xx_DIR)/common/mesh.o \
	$(RT28xx_DIR)/common/mesh_inf.o \
	$(RT28xx_DIR)/common/mesh_forwarding.o \
	$(RT28xx_DIR)/common/mesh_path_mng.o
endif
#endif // MESH_SUPPORT //

#ifdef WAPI_SUPPORT
ifeq ($(HAS_WAPI_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/common/wapi.o
endif
#endif // WAPI_SUPPORT //

#ifdef DOT11R_FT_SUPPORT
ifeq ($(HAS_DOT11R_FT_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/common/ft.o\
	$(RT28xx_DIR)/common/ft_tlv.o\
	$(RT28xx_DIR)/common/ft_iocl.o\
	$(RT28xx_DIR)/common/ft_rc.o\
	$(RT28xx_DIR)/ap/ap_ftkd.o
endif
#endif // DOT11R_FT_SUPPORT //

#ifdef DOT11K_RRM_SUPPORT
ifeq ($(HAS_DOT11K_RRM_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/common/rrm_tlv.o\
	$(RT28xx_DIR)/common/rrm_sanity.o\
	$(RT28xx_DIR)/common/rrm.o
endif
#endif // DOT11K_RRM_SUPPORT //

#ifdef RTMP_IOCTL_SUPPORT
ifeq ($(HAS_IOCTL_SUPPORT), y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/os/ThreadX/ap_ioctl.o\
	$(RT28xx_DIR)/os/ThreadX/rt_ioctl.o
endif
#endif // RTMP_IOCTL_SUPPORT //

ifeq ($(HAS_CLIENT_WDS_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	$(RT28xx_DIR)/common/client_wds.o
endif
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
rt$(CHIPSET)sta-objs := \
	$(RT28xx_DIR)/common/crypt_md5.o\
	$(RT28xx_DIR)/common/crypt_sha2.o\
	$(RT28xx_DIR)/common/crypt_hmac.o\
	$(RT28xx_DIR)/common/crypt_aes.o\
	$(RT28xx_DIR)/common/crypt_arc4.o\
	$(RT28xx_DIR)/common/mlme.o\
	$(RT28xx_DIR)/common/cmm_wep.o\
	$(RT28xx_DIR)/common/action.o\
	$(RT28xx_DIR)/common/cmm_data.o\
	$(RT28xx_DIR)/common/rtmp_init.o\
	$(RT28xx_DIR)/common/cmm_tkip.o\
	$(RT28xx_DIR)/common/cmm_aes.o\
	$(RT28xx_DIR)/common/cmm_sync.o\
	$(RT28xx_DIR)/common/eeprom.o\
	$(RT28xx_DIR)/common/cmm_sanity.o\
	$(RT28xx_DIR)/common/cmm_info.o\
	$(RT28xx_DIR)/common/cmm_cfg.o\
	$(RT28xx_DIR)/common/cmm_wpa.o\
	$(RT28xx_DIR)/common/cmm_wpa_adhoc.o\
	$(RT28xx_DIR)/common/dfs.o\
	$(RT28xx_DIR)/common/spectrum.o\
	$(RT28xx_DIR)/common/rtmp_timer.o\
	$(RT28xx_DIR)/common/rt_channel.o\
	$(RT28xx_DIR)/common/cmm_profile.o\
	$(RT28xx_DIR)/common/cmm_asic.o\
	$(RT28xx_DIR)/common/cmm_cmd.o\
	$(RT28xx_DIR)/common/rtmp_init_inf.o\
	$(RT28xx_DIR)/common/rt_os_util.o\
	$(RT28xx_DIR)/chips/rtmp_chip.o\
	$(RT28xx_DIR)/sta/assoc.o\
	$(RT28xx_DIR)/sta/auth.o\
	$(RT28xx_DIR)/sta/auth_rsp.o\
	$(RT28xx_DIR)/sta/sync.o\
	$(RT28xx_DIR)/sta/sanity.o\
	$(RT28xx_DIR)/sta/rtmp_data.o\
	$(RT28xx_DIR)/sta/connect.o\
	$(RT28xx_DIR)/sta/wpa.o\
	$(RT28xx_DIR)/sta/sta_cfg.o\
	$(RT28xx_DIR)/os/ThreadX/rt_threadx.o\
	$(RT28xx_DIR)/os/ThreadX/rt_ebs_rtip.o\
	$(RT28xx_DIR)/os/ThreadX/rt_profile.o\
	$(RT28xx_DIR)/os/ThreadX/rt_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/net_task.o

#ifdef DOT11_N_SUPPORT
ifeq ($(HAS_DOT11_N_SUPPORT),y)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/common/ba_action.o
endif
#endif // DOT11_N_SUPPORT //

#ifdef ETH_CONVERT
ifeq ($(HAS_ETH_CONVERT_SUPPORT), y)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/common/cmm_mat.o \
	$(RT28xx_DIR)/common/cmm_mat_iparp.o \
	$(RT28xx_DIR)/common/cmm_mat_pppoe.o \
	$(RT28xx_DIR)/common/cmm_mat_ipv6.o
endif
#endif // ETH_CONVERT //

#ifdef WSC_INCLUDED
ifeq ($(HAS_WSC),y)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/common/wsc.o\
	$(RT28xx_DIR)/common/wsc_tlv.o\
	$(RT28xx_DIR)/common/crypt_biginteger.o\
	$(RT28xx_DIR)/common/crypt_dh.o
endif
#endif // WSC_INCLUDED //

ifeq ($(HAS_BLOCK_NET_IF),y)
rt$(CHIPSET)sta-objs += $(RT28xx_DIR)/common/netif_block.o
endif

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
rt$(CHIPSET)sta-objs += $(RT28xx_DIR)/sta/dls.o
endif

#ifdef DOT11Z_TDLS_SUPPORT
ifeq ($(HAS_DOT11Z_TDLS_SUPPORT),y)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/sta/tdls.o\
	$(RT28xx_DIR)/sta/tdls_ctrl.o\
	$(RT28xx_DIR)/sta/tdls_tlv.o\
	$(RT28xx_DIR)/sta/tdls_link_mng.o
endif
#endif // DOT11Z_TDLS_SUPPORT //

#ifdef RT2860
ifeq ($(CHIPSET),2860)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o
endif
#endif // RT2860 //

#ifdef RT3090
ifeq ($(CHIPSET),3090)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt3090.o
endif
#endif // RT3090 //

#ifdef RT2870
ifeq ($(CHIPSET),2870)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o
endif
#endif // RT2870 //

#ifdef RT3070
ifeq ($(CHIPSET),2070)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt3070.o
endif

ifeq ($(CHIPSET),3070)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt3070.o
endif
#endif // RT3070 //

#ifdef RT2880
ifeq ($(CHIPSET),2880)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/rbus_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/ee_flash.o\
	$(RT28xx_DIR)/common/rtmp_swmcu.o\
	$(RT28xx_DIR)/chips/rt2880.o
endif
#endif // RT2880 //

#ifdef RT35xx
ifeq ($(CHIPSET),3572)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt35xx.o
endif

ifeq ($(CHIPSET),3062)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt35xx.o
endif

ifeq ($(CHIPSET),3562)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt35xx.o
endif
#endif // RT35xx //

#ifdef RT3390
ifeq ($(CHIPSET),3390)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt33xx.o\
	$(RT28xx_DIR)/chips/rt3390.o
endif
#endif // RT3390 //

#ifdef RT3370
ifeq ($(CHIPSET),3370)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt33xx.o\
	$(RT28xx_DIR)/chips/rt3370.o\
	$(RT28xx_DIR)/chips/rt3070.o
endif
#endif // RT3370 //

ifeq ($(HAS_ATE),y)
rt$(CHIPSET)sta-objs += $(RT28xx_DIR)/common/rt_ate.o
endif

#ifdef WMM_ACM_SUPPORT
ifeq ($(HAS_WMM_ACM_SUPPORT),y)
rt$(CHIPSET)sta-objs += $(RT28xx_DIR)/common/acm_edca.o\
	$(RT28xx_DIR)/common/acm_comm.o\
	$(RT28xx_DIR)/common/acm_iocl.o
endif
#endif // WMM_ACM_SUPPORT //

#ifdef MESH_SUPPORT
ifeq ($(HAS_MESH_SUPPORT),y)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/common/mesh_bmpkt.o \
	$(RT28xx_DIR)/common/mesh_ctrl.o \
	$(RT28xx_DIR)/common/mesh_link_mng.o \
	$(RT28xx_DIR)/common/mesh_sanity.o \
	$(RT28xx_DIR)/common/mesh_tlv.o \
	$(RT28xx_DIR)/common/mesh.o \
	$(RT28xx_DIR)/common/mesh_inf.o \
	$(RT28xx_DIR)/common/mesh_forwarding.o \
	$(RT28xx_DIR)/common/mesh_path_mng.o
endif
#endif // MESH_SUPPORT //

#ifdef WAPI_SUPPORT
ifeq ($(HAS_WAPI_SUPPORT),y)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/common/wapi.o
endif
#endif // WAPI_SUPPORT //

#ifdef DOT11R_FT_SUPPORT
ifeq ($(HAS_DOT11R_FT_SUPPORT),y)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/common/ft.o\
	$(RT28xx_DIR)/common/ft_tlv.o\
	$(RT28xx_DIR)/common/ft_iocl.o\
	$(RT28xx_DIR)/common/ft_rc.o\
	$(RT28xx_DIR)/sta/ft_auth.o\
	$(RT28xx_DIR)/sta/ft_action.o
endif
#endif // DOT11R_FT_SUPPORT //


#ifdef RTMP_IOCTL_SUPPORT
ifeq ($(HAS_IOCTL_SUPPORT), y)
rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/sta/sta_cfg.o\
	$(RT28xx_DIR)/os/ThreadX/rt_ioctl.o\
	$(RT28xx_DIR)/os/ThreadX/sta_ioctl.o
endif

ifeq ($(HAS_SY_IOCTL_SUPPORT), y)
	rt$(CHIPSET)sta-objs += \
	$(RT28xx_DIR)/os/ThreadX/sy_sta_ioctl.o
endif
#endif // RTMP_IOCTL_SUPPORT //

#endif // CONFIG_STA_SUPPORT //

#ifdef CONFIG_APSTA_SUPPORT
rt$(CHIPSET)apsta-objs := \
	$(RT28xx_DIR)/common/crypt_md5.o\
	$(RT28xx_DIR)/common/crypt_sha2.o\
	$(RT28xx_DIR)/common/crypt_hmac.o\
	$(RT28xx_DIR)/common/crypt_aes.o\
	$(RT28xx_DIR)/common/crypt_arc4.o\
	$(RT28xx_DIR)/common/mlme.o\
	$(RT28xx_DIR)/common/cmm_wep.o\
	$(RT28xx_DIR)/common/action.o\
	$(RT28xx_DIR)/common/cmm_data.o\
	$(RT28xx_DIR)/common/rtmp_init.o\
	$(RT28xx_DIR)/common/cmm_tkip.o\
	$(RT28xx_DIR)/common/cmm_aes.o\
	$(RT28xx_DIR)/common/cmm_sync.o\
	$(RT28xx_DIR)/common/eeprom.o\
	$(RT28xx_DIR)/common/cmm_sanity.o\
	$(RT28xx_DIR)/common/cmm_info.o\
	$(RT28xx_DIR)/common/cmm_cfg.o\
	$(RT28xx_DIR)/common/cmm_wpa.o\
	$(RT28xx_DIR)/common/dfs.o\
	$(RT28xx_DIR)/common/spectrum.o\
	$(RT28xx_DIR)/common/rtmp_timer.o\
	$(RT28xx_DIR)/common/rt_channel.o\
	$(RT28xx_DIR)/common/cmm_profile.o\
	$(RT28xx_DIR)/common/cmm_asic.o\
	$(RT28xx_DIR)/common/cmm_cmd.o\
	$(RT28xx_DIR)/common/rtmp_init_inf.o\
	$(RT28xx_DIR)/common/rt_os_util.o\
	$(RT28xx_DIR)/chips/rtmp_chip.o\
	$(RT28xx_DIR)/ap/ap.o\
	$(RT28xx_DIR)/ap/ap_assoc.o\
	$(RT28xx_DIR)/ap/ap_auth.o\
	$(RT28xx_DIR)/ap/ap_connect.o\
	$(RT28xx_DIR)/ap/ap_mlme.o\
	$(RT28xx_DIR)/ap/ap_sanity.o\
	$(RT28xx_DIR)/ap/ap_sync.o\
	$(RT28xx_DIR)/ap/ap_wpa.o\
	$(RT28xx_DIR)/ap/ap_data.o\
	$(RT28xx_DIR)/ap/ap_uapsd.o\
	$(RT28xx_DIR)/ap/ap_mbss.o\
	$(RT28xx_DIR)/sta/assoc.o\
	$(RT28xx_DIR)/sta/auth.o\
	$(RT28xx_DIR)/sta/auth_rsp.o\
	$(RT28xx_DIR)/sta/sync.o\
	$(RT28xx_DIR)/sta/sanity.o\
	$(RT28xx_DIR)/sta/rtmp_data.o\
	$(RT28xx_DIR)/sta/connect.o\
	$(RT28xx_DIR)/sta/wpa.o\
	$(RT28xx_DIR)/sta/sta_cfg.o\
	$(RT28xx_DIR)/os/ThreadX/rt_threadx.o\
	$(RT28xx_DIR)/os/ThreadX/rt_ebs_rtip.o\
	$(RT28xx_DIR)/os/ThreadX/rt_profile.o\
	$(RT28xx_DIR)/os/ThreadX/rt_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/ap_ioctl.o\
	$(RT28xx_DIR)/os/ThreadX/sta_ioctl.o\
	$(RT28xx_DIR)/ap/ap_autoChSel.o\
	$(RT28xx_DIR)/ap/ap_qload.o\
	$(RT28xx_DIR)/ap/ap_cfg.o

#ifdef DOT11_N_SUPPORT
ifeq ($(HAS_DOT11_N_SUPPORT),y)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/common/ba_action.o
endif
#endif // DOT11_N_SUPPORT //

#ifdef RT2860
ifeq ($(CHIPSET),2860)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o
endif
#endif // RT2860 //

#ifdef RT3090
ifeq ($(CHIPSET),3090)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt3090.o
endif
#endif // RT3090 //

#ifdef RT2870
ifeq ($(CHIPSET),2870)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o
endif
#endif // RT2870 //

#ifdef RT3070
ifeq ($(CHIPSET),2070)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt3070.o
endif

ifeq ($(CHIPSET),3070)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt3070.o
endif
#endif // RT3070 //

#ifdef WSC_INCLUDED
ifeq ($(HAS_WSC),y)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/common/wsc.o\
	$(RT28xx_DIR)/common/wsc_tlv.o\
	$(RT28xx_DIR)/common/crypt_biginteger.o\
	$(RT28xx_DIR)/common/crypt_dh.o
endif
#endif // WSC_INCLUDED //

#ifdef RT3390
ifeq ($(CHIPSET),3390)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/os/ThreadX/pci_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_pci_rbus.o\
	$(RT28xx_DIR)/common/cmm_mac_pci.o\
	$(RT28xx_DIR)/common/cmm_data_pci.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt33xx.o\
	$(RT28xx_DIR)/chips/rt3390.o
endif
#endif // RT3390 //

#ifdef RT3370
ifeq ($(CHIPSET),3370)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/os/ThreadX/usb_main_dev.o\
	$(RT28xx_DIR)/os/ThreadX/rt_usb.o\
	$(RT28xx_DIR)/common/cmm_mac_usb.o\
	$(RT28xx_DIR)/common/rtusb_dev_id.o\
	$(RT28xx_DIR)/common/rtusb_io.o\
	$(RT28xx_DIR)/common/rtusb_bulk.o\
	$(RT28xx_DIR)/common/rtusb_data.o\
	$(RT28xx_DIR)/common/cmm_data_usb.o\
	$(RT28xx_DIR)/common/rtmp_mcu.o\
	$(RT28xx_DIR)/common/ee_prom.o\
	$(RT28xx_DIR)/common/ee_efuse.o\
	$(RT28xx_DIR)/common/rt_rf.o\
	$(RT28xx_DIR)/chips/rt30xx.o\
	$(RT28xx_DIR)/chips/rt33xx.o\
	$(RT28xx_DIR)/chips/rt3370.o\
	$(RT28xx_DIR)/chips/rt3070.o
endif
#endif // RT3070 //

#ifdef NINTENDO_AP
ifeq ($(HAS_NINTENDO),y)
rt$(CHIPSET)apsta-objs += $(RT28xx_DIR)/ap/ap_nintendo.o
endif
#endif // NINTENDO_AP //

ifeq ($(HAS_WDS),y)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/ap/ap_wds.o \
	$(RT28xx_DIR)/ap/ap_wds_inf.o
endif

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
rt$(CHIPSET)apsta-objs += $(RT28xx_DIR)/ap/ap_dls.o
endif

ifeq ($(HAS_IDS_SUPPORT),y)
rt$(CHIPSET)apsta-objs += $(RT28xx_DIR)/ap/ap_ids.o
endif

#ifdef APCLI_SUPPORT
ifeq ($(HAS_APCLI),y)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/ap/ap_apcli.o \
	$(RT28xx_DIR)/ap/ap_apcli_inf.o \
	$(RT28xx_DIR)/ap/apcli_ctrl.o \
	$(RT28xx_DIR)/ap/apcli_sync.o \
	$(RT28xx_DIR)/ap/apcli_auth.o \
	$(RT28xx_DIR)/ap/apcli_assoc.o \
	$(RT28xx_DIR)/common/cmm_mat.o \
	$(RT28xx_DIR)/common/cmm_mat_iparp.o \
	$(RT28xx_DIR)/common/cmm_mat_pppoe.o \
	$(RT28xx_DIR)/common/cmm_mat_ipv6.o
endif
#endif // APCLI_SUPPORT //

#ifdef ETH_CONVERT
ifeq ($(HAS_ETH_CONVERT_SUPPORT), y)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/common/cmm_mat.o \
	$(RT28xx_DIR)/common/cmm_mat_iparp.o \
	$(RT28xx_DIR)/common/cmm_mat_pppoe.o \
	$(RT28xx_DIR)/common/cmm_mat_ipv6.o
endif
#endif // ETH_CONVERT //

ifeq ($(HAS_BLOCK_NET_IF),y)
rt$(CHIPSET)apsta-objs += $(RT28xx_DIR)/common/netif_block.o
endif

ifeq ($(HAS_IGMP_SNOOP_SUPPORT),y)
rt$(CHIPSET)apsta-objs += $(RT28xx_DIR)/common/igmp_snoop.o
endif

#ifdef MESH_SUPPORT
ifeq ($(HAS_MESH_SUPPORT),y)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/common/mesh_bmpkt.o \
	$(RT28xx_DIR)/common/mesh_ctrl.o \
	$(RT28xx_DIR)/common/mesh_link_mng.o \
	$(RT28xx_DIR)/common/mesh_sanity.o \
	$(RT28xx_DIR)/common/mesh_tlv.o \
	$(RT28xx_DIR)/common/mesh.o \
	$(RT28xx_DIR)/common/mesh_inf.o \
	$(RT28xx_DIR)/common/mesh_forwarding.o \
	$(RT28xx_DIR)/common/mesh_path_mng.o
endif
#endif // MESH_SUPPORT //

#ifdef WAPI_SUPPORT
ifeq ($(HAS_WAPI_SUPPORT),y)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/common/libwapi.a\
	$(RT28xx_DIR)/common/wapi.o
endif
#endif // WAPI_SUPPORT //

#ifdef DOT11R_FT_SUPPORT
ifeq ($(HAS_DOT11R_FT_SUPPORT),y)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/common/ft.o\
	$(RT28xx_DIR)/common/ft_tlv.o\
	$(RT28xx_DIR)/common/ft_iocl.o\
	$(RT28xx_DIR)/common/ft_rc.o\
	$(RT28xx_DIR)/ap/ap_ftkd.o\
	$(RT28xx_DIR)/sta/ft_auth.o\
	$(RT28xx_DIR)/sta/ft_action.o
endif
#endif // DOT11R_FT_SUPPORT //

ifeq ($(HAS_CLIENT_WDS_SUPPORT),y)
rt$(CHIPSET)apsta-objs += \
	$(RT28xx_DIR)/common/client_wds.o
endif

#endif // CONFIG_APSTA_SUPPORT //


#ifdef CONFIG_AP_SUPPORT
ifeq ($(RT28xx_MODE),AP)
RT_WIFI_OBJS = $(rt$(CHIPSET)ap-objs)
DAT_PATH = /etc/Wireless/RT$(CHIPSET)AP
DAT_FILE_NAME = RT$(CHIPSET)AP.dat
endif
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
ifeq ($(RT28xx_MODE), STA)
RT_WIFI_OBJS = $(rt$(CHIPSET)sta-objs)
DAT_PATH = /etc/Wireless/RT$(CHIPSET)STA
DAT_FILE_NAME = RT$(CHIPSET)STA.dat
endif
#endif // CONFIG_STA_SUPPORT //

#ifdef CONFIG_APSTA_SUPPORT
ifeq ($(RT28xx_MODE), APSTA)
RT_WIFI_OBJS = $(rt$(CHIPSET)apsta)
endif
#endif // CONFIG_APSTA_SUPPORT //

export RT_WIFI_OBJS

dummy:
	echo $(RT_WIFI_OBJS)
	
clean:
	rm -f $(RT28xx_DIR)/common/*.o
	rm -f $(RT28xx_DIR)/common/.*.{cmd,flags,d}
	rm -f $(RT28xx_DIR)/os/ThreadX/*.{o,ko,mod.{o,c}}
	rm -f $(RT28xx_DIR)/os/ThreadX/.*.{cmd,flags,d}
	rm -fr $(RT28xx_DIR)/os/ThreadX/.tmp_versions
	rm -f $(RT28xx_DIR)/chips/*.o
	rm -f $(RT28xx_DIR)/chips/.*.{cmd,flags,d}
ifeq ($(RT28xx_MODE),AP)
	rm -f $(RT28xx_DIR)/ap/*.o
	rm -f $(RT28xx_DIR)/ap/.*.{cmd,flags,d}
else
ifeq ($(RT28xx_MODE),STA)
	rm -f $(RT28xx_DIR)/sta/*.o
	rm -f $(RT28xx_DIR)/sta/.*.{cmd,flags,d}
else
ifeq ($(RT28xx_MODE),APSTA)
	rm -f $(RT28xx_DIR)/ap/*.o
	rm -f $(RT28xx_DIR)/ap/.*.{cmd,flags,d}
	rm -f $(RT28xx_DIR)/sta/*.o
	rm -f $(RT28xx_DIR)/sta/.*.{cmd,flags,d}
endif
endif
endif
