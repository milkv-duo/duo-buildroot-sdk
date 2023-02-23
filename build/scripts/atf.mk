
################################################################################
# BLD targets
################################################################################
BM_BLD_OUTPUT := ${BM_BLD_PATH}/out

bld-build:
	${Q}mkdir -p $(RELEASE_BIN_BLD_DIR)
	${Q}mkdir -p $(RELEASE_BIN_BLDP_DIR)
	${Q}mkdir -p $(RELEASE_BIN_BLP_DIR)

bld: export ARCH=$(patsubst "%",%,$(CONFIG_ARCH))
bld: bld-build
	$(call print_target)
	${Q}$(MAKE) -C ${BM_BLD_PATH} bld
	${Q}cp ${BM_BLD_OUTPUT}/bldp.bin ${RELEASE_BIN_BLDP_DIR}/bldp_${CHIP_ARCH_L}_${CHIP}_${DDR_CFG}.bin
	${Q}cp ${BM_BLD_OUTPUT}/blp.bin ${RELEASE_BIN_BLP_DIR}/blp_${CHIP_ARCH_L}_${CHIP}.bin
	${Q}git -C ${BM_BLD_PATH} log --pretty=oneline -n 1 > ${RELEASE_BIN_BLDP_DIR}/bldp_${CHIP_ARCH_L}_${CHIP}_${DDR_CFG}.txt
	${Q}git -C ${BM_BLD_PATH} log --pretty=oneline -n 1 > ${RELEASE_BIN_BLP_DIR}/blp_${CHIP_ARCH_L}_${CHIP}.txt

bld-clean:
	$(call print_target)
	${Q}$(MAKE) -C ${BM_BLD_PATH} clean

################################################################################
# arm-trusted-firmware targets
################################################################################
ATF_FIP_PATH := ${ATF_PATH}/build/${CHIP}_${SUBTYPE}/release/fip.bin

# clear is not encrypted
# key0 is encrypted by keys in ATF git repo
export ATF_KEY_SEL := $(subst default,,${ATF_KEY_SEL})

ifeq ($(CHIP_ARCH_L),$(filter $(CHIP_ARCH_L),cv183x cv75x1 cv952x))
define atf_post_action
	${Q}mv ${ATF_FIP_PATH} ${ATF_FIP_PATH}.ori
	# append blp.bin and bldp.bin to fip.bin
	${Q}dd if=${ATF_FIP_PATH}.ori of=${ATF_FIP_PATH} bs=2K conv=sync
	${Q}dd if=${BLP_PATH} bs=2K conv=sync >> ${ATF_FIP_PATH}
	${Q}dd if=${DDRC_PATH} bs=2K conv=sync >> ${ATF_FIP_PATH}
endef
    ATF_KEY_SEL := $(or ${ATF_KEY_SEL},key0)
else
    ATF_KEY_SEL := $(or ${ATF_KEY_SEL},clear)
endif

export IMG_ENC_KPATH :=
export IMG_ENC_KSRC := dev

ifeq (${ATF_KEY_SEL},clear)
    export ATF_TBBR := 0
    export IMG_ENC := 0
    export ATF_CRC := 1
else
    export ATF_TBBR := 1
    export IMG_ENC := 1
    export ATF_CRC := 0
endif

# ARM ATF TBBR configuration
TBBR_MAKE_OPT :=
TBBR_MAKE_TGT :=
ifeq (${ATF_TBBR},1)
    TBBR_MAKE_OPT := TRUSTED_BOARD_BOOT=1
    #TGT is short for TarGeT
    TBBR_MAKE_TGT := certificates
endif

# ARM ATF bl32
SPD_MAKE_OPT :=
ifeq (${ATF_BL32},1)
    SPD_MAKE_OPT := SPD=opteed
endif

FAKE_BL31_32_TGT :=
ifeq (${FAKE_BL31_32},1)
    FAKE_BL31_32_TGT := fake_bl31_32
endif

ifeq ($(wildcard ${BM_BLD_PATH}/*),)
arm-trusted-firmware-build: export BLP_PATH=${ATF_PATH}/tools/blp.bin
arm-trusted-firmware-build: export DDRC_PATH=${ATF_PATH}/tools/bldp.bin
arm-trusted-firmware: export SCP_BL2=${ATF_PATH}/tools/bld.bin
else
ifeq (${CONFIG_FIP_V1},y)
arm-trusted-firmware-build: bld
arm-trusted-firmware-build: export BLP_PATH=${BM_BLD_OUTPUT}/blp.bin
arm-trusted-firmware-build: export DDRC_PATH=${BM_BLD_OUTPUT}/bldp.bin
arm-trusted-firmware: export RTC_CORE_SRAM_BIN_PATH=${BM_BLD_OUTPUT}/blds.bin
else
arm-trusted-firmware-build: export BLP_PATH=${ATF_PATH}/tools/blp.bin
arm-trusted-firmware-build: export DDRC_PATH=${ATF_PATH}/tools/bldp.bin
arm-trusted-firmware-build: export SCP_BL2=${ATF_PATH}/tools/bld.bin
arm-trusted-firmware-build: export RTC_CORE_SRAM_BIN_PATH=${ATF_PATH}/tools/bld.bin
endif

ifeq (${CONFIG_MULTI_FIP},y)
ATF_DEFAULT_SUFFIX := ${ATF_DEFAULT_SUFFIX}_single
arm-trusted-firmware: export MULTI_FIP=1
arm-trusted-firmware: export SCP_BL2=${ATF_PATH}/tools/fastboot/fake_bld_enc.bin
arm-trusted-firmware: export DDR_INIT=${BM_BLD_OUTPUT}/bld.bin
else
arm-trusted-firmware: export SCP_BL2=${BM_BLD_OUTPUT}/bld.bin
endif

endif

arm-trusted-firmware-build: export CROSS_COMPILE=${CROSS_COMPILE_64}
arm-trusted-firmware-build: export BL33=${ATF_PATH}/build/fake_bl33.bin
arm-trusted-firmware-build:
	$(call print_target)
	${Q}mkdir -p $(dir ${BL33})
	${Q}mkdir -p ${RELEASE_BIN_ATF_DIR}
	${Q}printf 'BL33............' > ${BL33}
	${Q}echo "SCP_BL2=${SCP_BL2}"
	${Q}$(MAKE) -j${NPROC} -C ${ATF_PATH} \
		CRC=${ATF_CRC} IMG_BLD=1 DEBUG=0 ENABLE_ASSERTIONS=1 \
		${SPD_MAKE_OPT} ${TBBR_MAKE_TGT} ${TBBR_MAKE_OPT} ${FAKE_BL31_32_TGT} all fip
	$(call atf_post_action)

ifeq (${ATF_TBBR},0)
    ATF_DEFAULT_SUFFIX := clear
else
    ATF_DEFAULT_SUFFIX := key0
endif

ifeq (${CONFIG_FIP_V1},y)
arm-trusted-firmware-pack: arm-trusted-firmware-build
	$(call print_target)
	${Q}cp ${ATF_FIP_PATH} ${RELEASE_BIN_ATF_DIR}/fip_atf_${CHIP_ARCH_L}_${ATF_DEFAULT_SUFFIX}.bin
ifneq ($(wildcard ${BM_BLD_PATH}/*),)
	${Q}python3 ${TOOLS_PATH}/${CHIP_ARCH_L}/pack_fip/pack_fip.py $(if ${CONFIG_MULTI_FIP},--multibin) \
		--tar-bld ${ATF_FIP_PATH} \
		--output ${RELEASE_BIN_BLD_DIR}/bld_${CHIP_ARCH_L}_${CHIP}_${BOARD}_${ATF_DEFAULT_SUFFIX}.tar
	${Q}git -C ${BM_BLD_PATH} log --pretty=oneline -n 1 > ${RELEASE_BIN_BLD_DIR}/bld_${CHIP_ARCH_L}_${CHIP}_${BOARD}_${ATF_DEFAULT_SUFFIX}.txt
endif
	${Q}git -C ${ATF_PATH} log --pretty=oneline -n 1 > ${RELEASE_BIN_ATF_DIR}/fip_atf_${CHIP_ARCH_L}_${ATF_DEFAULT_SUFFIX}.txt

arm-trusted-firmware: arm-trusted-firmware-pack
endif

arm-trusted-firmware: arm-trusted-firmware-build

arm-trusted-firmware-clean: bld-clean
	$(call print_target)
	${Q}$(MAKE) -C ${ATF_PATH} clean
	${Q}$(MAKE) -C ${ATF_PATH}/tools/fiptool clean

