ifeq (${CONFIG_ATF_SRC},)
FIP_PRE_MERGE_DEPS :=
else ifeq (${CONFIG_ATF_SRC},y)
# Build arm-trusted-firmware and bm_bld tnd put into rel-bin/* git repo
FIP_PRE_MERGE_DEPS := arm-trusted-firmware
else
$(error CONFIG_ATF_SRC=${CONFIG_ATF_SRC} is not used)
endif

FIP_PRE_BIN_DIR := ${OUTPUT_DIR}/fip_pre

${FIP_PRE_BIN_DIR}:
	${Q}mkdir -p $@

ifeq (${ATF_KEY_SEL},clear)
define pad_atf_crc_action
	${BUILD_PATH}/scripts/pad_atf_crc.py --pad-crc ${1}
endef
else
define pad_atf_crc_action
endef
endif

FIP_PRE_SUFFIX := $(if ${CONFIG_MULTI_FIP},_single)
FIP_PRE_SUFFIX_EXTRA :=

ifeq ($(ENABLE_ALIOS_FASTBOOT), y)
FIP_PRE_SUFFIX_EXTRA := _alios
endif

ATF_FIP_BIN_PATH := ${RELEASE_BIN_ATF_DIR}/fip_atf_${CHIP_ARCH_L}_${ATF_KEY_SEL}${FIP_PRE_SUFFIX}${FIP_PRE_SUFFIX_EXTRA}.bin
BLD_TAR_PATH := ${RELEASE_BIN_BLD_DIR}/bld_${CHIP_ARCH_L}_${CHIP}_${BOARD}_${ATF_KEY_SEL}${FIP_PRE_SUFFIX}.tar

# Generate fip_pre.bin from rel-bin/* git repo
fip-pre-merge-build: ${FIP_PRE_BIN_DIR} ${FIP_PRE_MERGE_DEPS}
	$(call print_target)
	${Q}cp ${RELEASE_BIN_BLP_DIR}/blp_${CHIP_ARCH_L}_${CHIP}.bin ${FIP_PRE_BIN_DIR}/blp.bin
	${Q}cp ${RELEASE_BIN_BLDP_DIR}/bldp_${CHIP_ARCH_L}_${CHIP}_${DDR_CFG}.bin ${FIP_PRE_BIN_DIR}/bldp.bin
	${call pad_atf_crc_action,${FIP_PRE_BIN_DIR}/blp.bin}
	${call pad_atf_crc_action,${FIP_PRE_BIN_DIR}/bldp.bin}
	${Q}python3 ${TOOLS_PATH}/${CHIP_ARCH_L}/pack_fip/pack_fip.py \
		${ATF_FIP_BIN_PATH} \
		$(if ${CONFIG_MULTI_FIP},--multibin) \
		--output=${FIP_PRE_BIN_DIR}/fip_pre.bin \
		--add-license_file=${LICENSE_PATH} \
		--add-blp=${FIP_PRE_BIN_DIR}/blp.bin \
		--add-ddrc=${FIP_PRE_BIN_DIR}/bldp.bin \
		--add-bld-tar=${BLD_TAR_PATH}
	${Q}ls -l ${FIP_PRE_BIN_DIR}/fip_pre.bin

ifeq ($(wildcard ${RELEASE_BIN_BLD_DIR}/*),)
# No rel_bin. Use install/../fip_pre/
fip-pre-merge:
else
# rel_bin. Use rel_bin/....
fip-pre-merge: fip-pre-merge-build

ifeq ($(wildcard ${RELEASE_BIN_LICENSE_DIR}/*),)
fip-pre-merge: export LICENSE_PATH=${ATF_PATH}/tools/license.scm.enc
else
fip-pre-merge: export LICENSE_PATH=${RELEASE_BIN_LICENSE_DIR}/license_${CHIP_SEGMENT}_for_Customer.scm.enc
endif

endif

u-boot-dep: fip-pre-merge u-boot-build $(if ${CONFIG_ENABLE_FREERTOS},rtos)
	$(call print_target)
	$(call uboot_compress_action)
ifeq (${CONFIG_MULTI_FIP},y)
	${Q}python3 ${TOOLS_PATH}/${CHIP_ARCH_L}/pack_fip/pack_fip_multibin.py --multibin \
		${FIP_PRE_BIN_DIR}/fip_pre.bin \
		--fastboot=${FREERTOS_PATH}/cvirtos.bin \
		--bl33 ${UBOOT_PATH}/${UBOOT_OUTPUT_FOLDER}/u-boot.bin --output ${FIP_PRE_BIN_DIR}/fip.bin
	${Q}python3 ${IMGTOOL_PATH}/raw2cimg.py ${FIP_PRE_BIN_DIR}/fip_2nd.bin ${OUTPUT_DIR} ${FLASH_PARTITION_XML}
	${Q}cp ${FIP_PRE_BIN_DIR}/fip_1st.bin ${OUTPUT_DIR}/fip.bin
else
	${Q}python3 ${TOOLS_PATH}/${CHIP_ARCH_L}/pack_fip/pack_fip.py ${FIP_PRE_BIN_DIR}/fip_pre.bin \
		--add-bl33 ${UBOOT_PATH}/${UBOOT_OUTPUT_FOLDER}/u-boot.bin --output ${OUTPUT_DIR}/fip.bin
endif
