include $(RT2860_DIR)/os/ucos/config.mk 

UCOS_LIB = $(UCOS_SRC)/samples/lwip_on_ucos_test
OBJCOPY = $(CROSS_COMPILE)objcopy 
IMAGE_ELF = rt2882.elf
IMAGE_BIN = rt2882.bin
IMAGE_REC = rt2882.srec

ifeq ($(RT2860_MODE),AP)
OBJ = rt2882ap.o

RT2882_OBJ = $(RT2860_DIR)/common/md5.o\
	$(RT2860_DIR)/common/mlme.o\
	$(RT2860_DIR)/common/rtmp_wep.o\
	$(RT2860_DIR)/common/action.o\
	$(RT2860_DIR)/common/my_action.o\
	$(RT2860_DIR)/common/cmm_data.o\
	$(RT2860_DIR)/common/rtmp_init.o\
	$(RT2860_DIR)/common/rtmp_tkip.o\
	$(RT2860_DIR)/common/cmm_sync.o\
	$(RT2860_DIR)/common/cmm_sanity.o\
	$(RT2860_DIR)/common/eeprom.o\
	$(RT2860_DIR)/common/cmm_info.o\
	$(RT2860_DIR)/common/cmm_wpa.o\
	$(RT2860_DIR)/common/dfs.o\
	$(RT2860_DIR)/ap/ap.o\
	$(RT2860_DIR)/ap/ap_assoc.o\
	$(RT2860_DIR)/ap/ap_auth.o\
	$(RT2860_DIR)/ap/ap_authrsp.o\
	$(RT2860_DIR)/ap/ap_connect.o\
	$(RT2860_DIR)/ap/ap_mlme.o\
	$(RT2860_DIR)/ap/ap_sanity.o\
	$(RT2860_DIR)/ap/ap_sync.o\
	$(RT2860_DIR)/ap/ap_wpa.o\
	$(RT2860_DIR)/ap/ap_data.o\
	$(RT2860_DIR)/ap/ap_uapsd.o\
	$(RT2860_DIR)/ap/ap_mbss.o\
	$(RT2860_DIR)/os/ucos/rt_profile.o\
	$(RT2860_DIR)/os/ucos/rt_ucos.o\
	$(RT2860_DIR)/os/ucos/rt_main_dev.o\
	$(RT2860_DIR)/ap/ap_autoChSel.o
	
ifeq ($(HAS_ATE),y)
RT2882_OBJ += $(RT2860_DIR)/ap/ap_ate.o
endif

ifeq ($(HAS_WSC),y)
RT2882_OBJ += $(RT2860_DIR)/common/wsc.o\
	$(RT2860_DIR)/common/wsc_tlv.o\
	$(RT2860_DIR)/common/sha2.o\
	$(RT2860_DIR)/common/hmac.o\
	$(RT2860_DIR)/common/dh_key.o\
	$(RT2860_DIR)/common/evp_enc.o	
endif

ifeq ($(HAS_WDS),y)
RT2882_OBJ += $(RT2860_DIR)/ap/ap_wds.o
endif

#ifdef NINTENDO_AP
ifeq ($(HAS_NINTENDO),y)
RT2882_OBJ += \
	$(RT2860_DIR)/ap/ap_nintendo.o
endif
#endif // NINTENDO_AP //

ifeq ($(HAS_APCLI),y)
RT2882_OBJ += \
	$(RT2860_DIR)/ap/ap_apcli.o \
	$(RT2860_DIR)/ap/apcli_mlme.o \
	$(RT2860_DIR)/ap/apcli_ctrl.o \
	$(RT2860_DIR)/ap/apcli_sync.o \
	$(RT2860_DIR)/ap/apcli_auth.o \
	$(RT2860_DIR)/ap/apcli_assoc.o \
	$(RT2860_DIR)/common/cmm_mat.o \
	$(RT2860_DIR)/common/cmm_mat_iparp.o \
	$(RT2860_DIR)/common/cmm_mat_pppoe.o
endif

all:$(OBJ) mkimage

mkimage: $(RT2882_OBJ)
	$(LD) -T lwip.ld -g -G 0 -n -o $(IMAGE_ELF) -Map lwip_test.map --oformat elf32-tradlittlemips --gc-sections rt2882ap.o -L $(UCOS_LIB) -lucos  
	$(OBJCOPY) -O binary $(IMAGE_ELF) $(IMAGE_BIN)
	$(OBJCOPY) -O srec $(IMAGE_ELF) $(IMAGE_REC)
	
rt2882ap.o: $(RT2882_OBJ)
	$(LD) -r $^ -o $@

clean:
	rm -f $(RT2860_DIR)/ap/*.o
	rm -f $(RT2860_DIR)/common/*.o
	rm -f $(RT2860_DIR)/os/ucos/*.o
endif

ifeq ($(RT2860_MODE),STA)
OBJ = rt2882sta.o
all:$(OBJ) mkimage

rt2882sta.o: $(RT2860_DIR)/common/md5.o\
	$(RT2860_DIR)/common/mlme.o\
	$(RT2860_DIR)/common/rtmp_wep.o\
	$(RT2860_DIR)/common/action.o\
	$(RT2860_DIR)/common/my_action.o\
	$(RT2860_DIR)/common/cmm_data.o\
	$(RT2860_DIR)/common/rtmp_init.o\
	$(RT2860_DIR)/common/rtmp_tkip.o\
	$(RT2860_DIR)/common/cmm_sync.o\
	$(RT2860_DIR)/common/eeprom.o\
        $(RT2860_DIR)/common/cmm_sanity.o\
        $(RT2860_DIR)/common/cmm_info.o\
	$(RT2860_DIR)/common/cmm_wpa.o\
	$(RT2860_DIR)/common/dfs.o\
	$(RT2860_DIR)/sta/assoc.o\
	$(RT2860_DIR)/sta/aironet.o\
	$(RT2860_DIR)/sta/auth.o\
	$(RT2860_DIR)/sta/auth_rsp.o\
	$(RT2860_DIR)/sta/sync.o\
	$(RT2860_DIR)/sta/sanity.o\
	$(RT2860_DIR)/sta/rtmp_data.o\
	$(RT2860_DIR)/sta/connect.o\
	$(RT2860_DIR)/sta/wpa.o\
	$(RT2860_DIR)/os/ucos/rt_ucos.o\
        $(RT2860_DIR)/os/ucos/rt_profile.o\
	$(RT2860_DIR)/os/ucos/rt_main_dev.o\
#$(RT2860_DIR)/os/ucos/sta_ioctl.o\

rt2882sta.o: $(RT2882_OBJ)
	$(LD) -r $^ -o $@

mkimage: $(RT2882_OBJ)
	$(LD) -T lwip.ld -g -G 0 -n -o $(IMAGE_ELF) -Map lwip_test.map --oformat elf32-tradlittlemips --gc-sections rt2882sta.o -L $(UCOS_LIB) -lucos  
	$(OBJCOPY) -O binary $(IMAGE_ELF) $(IMAGE_BIN)
	$(OBJCOPY) -O srec $(IMAGE_ELF) $(IMAGE_REC)
	
release:
	echo "MAKE UCOS Station Code Release"

clean:
	rm -f $(RT2860_DIR)/sta/*.o
	rm -f $(RT2860_DIR)/common/*.o
	rm -f $(RT2860_DIR)/os/ucos/*.o

endif

