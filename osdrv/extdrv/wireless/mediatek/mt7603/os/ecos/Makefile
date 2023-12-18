include $(TOPDIR)/.config
include $(TOPDIR)/rules.mak
include $(RT28xx_DIR)/os/ecos/config.mk

#ifdef CONFIG_AP_SUPPORT
rt$(CHIPSET)ap-objs := \
	../../common/crypt_md5.o\
	../../common/crypt_sha2.o\
	../../common/crypt_hmac.o\
	../../common/crypt_aes.o\
	../../common/crypt_arc4.o\
	../../common/mlme.o\
	../../common/cmm_wep.o\
	../../common/action.o\
	../../common/cmm_data.o\
	../../common/rtmp_init.o\
	../../common/rtmp_init_inf.o\
	../../common/cmm_tkip.o\
	../../common/cmm_aes.o\
	../../common/cmm_sync.o\
	../../common/eeprom.o\
	../../common/cmm_sanity.o\
	../../common/cmm_info.o\
	../../common/cmm_cfg.o\
	../../common/cmm_wpa.o\
	../../common/dfs.o\
	../../common/spectrum.o\
	../../common/rtmp_timer.o\
	../../common/rt_channel.o\
	../../common/cmm_profile.o\
	../../common/cmm_asic.o\
	../../common/cmm_cmd.o\
	../../os/ecos/rt_profile.o \
	../../ap/ap_mbss.o\
	../../chips/rtmp_chip.o\
	../../ap/ap.o\
	../../ap/ap_assoc.o\
	../../ap/ap_auth.o\
	../../ap/ap_connect.o\
	../../ap/ap_mlme.o\
	../../ap/ap_sanity.o\
	../../ap/ap_sync.o\
	../../ap/ap_wpa.o\
	../../ap/ap_data.o\
	../../ap/ap_uapsd.o\
	../../ap/ap_autoChSel.o\
	../../ap/ap_qload.o\
	../../ap/ap_cfg.o

ifeq ($(OSABL),NO)
rt$(CHIPSET)ap-objs += \
	../../ap/ap_mbss_inf.o\
	../../common/rt_os_util.o\
	../../os/ecos/ap_ioctl.o\
	../../os/ecos/rt_ecos.o \
	../../os/ecos/rt_main_end.o
endif

#ifdef DOT11_N_SUPPORT
ifeq ($(HAS_DOT11_N_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/ba_action.o
endif
#endif // DOT11_N_SUPPORT //

#ifdef BG_FT_SUPPORT
ifeq ($(HAS_BGFP_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/br_ftph.o
endif
#endif // BG_FT_SUPPORT //

#ifdef LED_CONTROL_SUPPORT
ifeq ($(HAS_LED_CONTROL_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/rt_led.o
endif
#endif // LED_CONTROL_SUPPORT //

#ifdef RT3050
ifeq ($(CHIPSET),3050)
rt$(CHIPSET)ap-objs += \
	../../common/cmm_mac_pci.o\
	../../common/cmm_data_pci.o\
	../../common/ee_flash.o\
	../../common/rtmp_swmcu.o\
	../../common/rt_rf.o\
	../../chips/rt305x.o

ifeq ($(HAS_DFS_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/dfs_mcu.o
endif

ifeq ($(OSABL),NO)
rt$(CHIPSET)ap-objs += \
	../../os/ecos/rt_pci_rbus.o\
	../../os/ecos/rbus_main_end.o\
	../../os/ecos/rt_rbus_pci_util.o
endif
endif
#endif // RT3050 //

#ifdef RT3052
ifeq ($(CHIPSET),3052)
rt$(CHIPSET)ap-objs += \
	../../common/cmm_mac_pci.o\
	../../common/cmm_data_pci.o\
	../../common/ee_flash.o\
	../../common/rtmp_swmcu.o\
	../../common/rt_rf.o\
	../../chips/rt305x.o

ifeq ($(HAS_DFS_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/dfs_mcu.o
endif

ifeq ($(OSABL),NO)
rt$(CHIPSET)ap-objs += \
	../../os/ecos/rt_pci_rbus.o\
	../../os/ecos/rbus_main_end.o\
	../../os/ecos/rt_rbus_pci_util.o
endif
endif
#endif // RT3052 //

#ifdef RT3352
ifeq ($(CHIPSET),3352)
rt$(CHIPSET)ap-objs += \
	../../common/cmm_mac_pci.o\
	../../common/cmm_data_pci.o\
	../../common/ee_flash.o\
	../../common/rtmp_swmcu.o\
	../../common/rt_rf.o\
	../../chips/rt305x.o\
	../../chips/rt3352.o

ifeq ($(HAS_DFS_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/dfs_mcu.o
endif

ifeq ($(OSABL),NO)
rt$(CHIPSET)ap-objs += \
	../../os/ecos/rt_pci_rbus.o\
	../../os/ecos/rbus_main_end.o\
	../../os/ecos/rt_rbus_pci_util.o
endif
endif
#endif // RT3352 //

#ifdef RT3350
ifeq ($(CHIPSET),3350)
rt$(CHIPSET)ap-objs += \
	../../common/cmm_mac_pci.o\
	../../common/cmm_data_pci.o\
	../../common/ee_flash.o\
	../../common/rtmp_swmcu.o\
	../../common/rt_rf.o\
	../../chips/rt305x.o

ifeq ($(HAS_DFS_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/dfs_mcu.o
endif

ifeq ($(OSABL),NO)
rt$(CHIPSET)ap-objs += \
	../../os/ecos/rt_pci_rbus.o\
	../../os/ecos/rbus_main_end.o\
	../../os/ecos/rt_rbus_pci_util.o
endif
endif
#endif // RT3350 //

#ifdef RT5350
ifeq ($(CHIPSET),5350)
rt$(CHIPSET)ap-objs += \
	../../common/cmm_mac_pci.o\
	../../common/cmm_data_pci.o\
	../../common/ee_flash.o\
	../../common/rtmp_swmcu.o\
	../../common/rt_rf.o\
	../../chips/rt305x.o\
	../../chips/rt5350.o

ifeq ($(HAS_DFS_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/dfs_mcu.o
endif

ifeq ($(OSABL),NO)
rt$(CHIPSET)ap-objs += \
	../../os/ecos/rt_pci_rbus.o\
	../../os/ecos/rbus_main_end.o\
	../../os/ecos/rt_rbus_pci_util.o
endif
endif
#endif // RT5350 //

ifeq ($(HAS_ATE),y)
rt$(CHIPSET)ap-objs += ../../common/rt_ate.o
endif

#ifdef WSC_INCLUDED
ifeq ($(HAS_WSC),y)
rt$(CHIPSET)ap-objs += \
	../../common/wsc.o\
	../../common/wsc_tlv.o\
	../../common/wsc_ufd.o\
	../../common/crypt_biginteger.o\
	../../common/crypt_dh.o	
endif
ifeq ($(HAS_WSC_V2),y)
rt$(CHIPSET)ap-objs += ../../common/wsc_v2.o
endif
#endif // WSC_INCLUDED //

#ifdef EASY_CONFIG_SETUP
ifeq ($(HAS_EASY_CONFIG_SETUP_SUPPORT),y)
ifneq ($(PLATFORM),RALINK_3052)
rt$(CHIPSET)ap-objs += ../../common/auto_provision.o
endif
endif
#endif // EASY_CONFIG_SETUP //

#ifdef NINTENDO_AP
ifeq ($(HAS_NINTENDO),y)
rt$(CHIPSET)ap-objs += ../../ap/ap_nintendo.o
endif
#endif // NINTENDO_AP //

ifeq ($(HAS_WDS),y)
rt$(CHIPSET)ap-objs += ../../ap/ap_wds.o

ifeq ($(OSABL),NO)
rt$(CHIPSET)ap-objs += \
	../../ap/ap_wds_inf.o
endif
endif

#ifdef APCLI_SUPPORT
ifeq ($(HAS_APCLI),y)
rt$(CHIPSET)ap-objs += \
	../../ap/ap_apcli.o \
	../../ap/apcli_ctrl.o \
	../../ap/apcli_sync.o \
	../../ap/apcli_auth.o \
	../../ap/apcli_assoc.o \
	../../common/cmm_mat.o \
	../../common/cmm_mat_iparp.o \
	../../common/cmm_mat_pppoe.o \
	../../common/cmm_mat_ipv6.o

ifeq ($(OSABL),NO)
rt$(CHIPSET)ap-objs += \
	../../ap/ap_apcli_inf.o
endif
endif
#endif // APCLI_SUPPORT //

ifeq ($(HAS_BLOCK_NET_IF),y)
rt$(CHIPSET)ap-objs += ../../common/netif_block.o
endif

ifeq ($(HAS_IGMP_SNOOP_SUPPORT),y)
rt$(CHIPSET)ap-objs += ../../common/igmp_snoop.o
endif

#ifdef WMM_ACM_SUPPORT
ifeq ($(HAS_WMM_ACM_SUPPORT),y)
rt$(CHIPSET)ap-objs += ../../common/acm_edca.o\
	../../common/acm_comm.o\
	../../common/acm_iocl.o
endif
#endif // WMM_ACM_SUPPORT //

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
rt$(CHIPSET)ap-objs += ../../ap/ap_dls.o
endif

ifeq ($(HAS_IDS_SUPPORT),y)
rt$(CHIPSET)ap-objs += ../../ap/ap_ids.o
endif

ifeq ($(PLATFORM),IKANOS_V160)
rt$(CHIPSET)ap-objs += ../../os/linux/vr_ikans.o
endif

ifeq ($(PLATFORM),IKANOS_V180)
rt$(CHIPSET)ap-objs += ../../os/linux/vr_ikans.o
endif

#ifdef MESH_SUPPORT
ifeq ($(HAS_MESH_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/mesh_bmpkt.o \
	../../common/mesh_ctrl.o \
	../../common/mesh_link_mng.o \
	../../common/mesh_sanity.o \
	../../common/mesh_tlv.o \
	../../common/mesh.o \
	../../common/mesh_forwarding.o \
	../../common/mesh_path_mng.o

ifeq ($(OSABL),NO)
rt$(CHIPSET)ap-objs += \
	../../common/mesh_inf.o
endif
endif
#endif // MESH_SUPPORT //

#ifdef WAPI_SUPPORT
ifeq ($(HAS_WAPI_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/wapi.o
endif
#endif // WAPI_SUPPORT //

#ifdef DOT11R_FT_SUPPORT
ifeq ($(HAS_DOT11R_FT_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/ft.o\
	../../common/ft_tlv.o\
	../../common/ft_iocl.o\
	../../common/ft_rc.o\
	../../ap/ap_ftkd.o
endif
#endif // DOT11R_FT_SUPPORT //

#ifdef DOT11K_RRM_SUPPORT
ifeq ($(HAS_DOT11K_RRM_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/rrm_tlv.o\
	../../common/rrm_sanity.o\
	../../common/rrm.o
endif
#endif // DOT11K_RRM_SUPPORT //

#ifdef DOT11V_WNM_SUPPORT
ifeq ($(HAS_DOT11V_WNM_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
        ../../common/wnm_tlv.o\
        ../../common/wnm.o
endif
#endif // DOT11V_WNM_SUPPORT //

#ifdef DOT11W_PMF_SUPPORT
ifeq ($(HAS_DOT11W_PMF_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/pmf.o
endif
#endif // DOT11W_PMF_SUPPORT //

ifeq ($(HAS_CLIENT_WDS_SUPPORT),y)
rt$(CHIPSET)ap-objs += \
	../../common/client_wds.o
endif
#endif // CONFIG_AP_SUPPORT //





# Compiler #
ifeq ($(RT28xx_MODE),AP)
	ALL_OBJS = $(rt$(CHIPSET)ap-objs)
else
ifeq ($(RT28xx_MODE),STA)
	ALL_OBJS = $(rt$(CHIPSET)sta-objs)
else
ifeq ($(RT28xx_MODE),APSTA)
	ALL_OBJS = $(rt$(CHIPSET)apsta-objs)
endif
endif
endif

EXTRA_CFLAGS = -I../../include -D_KERNEL $(WFLAGS)

all: $(MODULE)

$(MODULE): $(ALL_OBJS:.o=.d)
	$(XLD) -r $(ENDIAN) -o $(MODULE) $(ALL_OBJS)

clean:
	rm -f $(MODULE) $(ALL_OBJS)
	rm -f ../../common/*.o
	rm -f ../../common/*.d
	rm -f ../../os/ecos/*.d
	rm -f ../../os/ecos/.o
	rm -fr ../../os/ecos/.tmp_versions
	rm -f ../../chips/*.o
	rm -f ../../chips/*d
ifeq ($(RT28xx_MODE),AP)
	rm -f ../../ap/*.o
	rm -f ../../ap/*.d
else
ifeq ($(RT28xx_MODE),STA)
	rm -f ../../sta/*.o
	rm -f ../../sta/*.d
else
ifeq ($(RT28xx_MODE),APSTA)
	rm -f ../../ap/*.o
	rm -f ../../ap/*.d
	rm -f ../../sta/*.o
	rm -f ../../sta/*d
endif
endif
endif

