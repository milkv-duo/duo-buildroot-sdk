BLCP_PATH = ../fast_image_mcu/riscv/output/fast_image_mcu.bin
ifeq ("$(wildcard $(BLCP_PATH))","")
BLCP_PATH = test/empty.bin
endif
FIP_COMPRESS ?= lzma

CHIP_CONF_PATH = ${BUILD_PLAT}/chip_conf.bin

ifeq (${BOOT_CPU},aarch64)
MONITOR_PATH = plat/${CHIP_ARCH}/prebuilt/bl31.bin
else ifeq (${BOOT_CPU},riscv)
MONITOR_PATH = ../opensbi/build/platform/generic/firmware/fw_dynamic.bin
endif

ifeq (${ENABLE_BURN_BUTTON},y)
LOADER_2ND_PATH_B=${UBOOT_PATH}/${UBOOT_OUTPUT_FOLDER}/u-boot-raw.bin
else
LOADER_2ND_PATH_B=
endif

fip%: export BLCP_IMG_RUNADDR=0x05200200
fip%: export BLCP_PARAM_LOADADDR=0
fip%: export NAND_INFO=00000000
ifeq (${FSBL_FASTBOOT},y)
$(eval $(call add_define,FSBL_FASTBOOT))
fip%: export NOR_INFO=01C0080000430000A93B060000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
else ifeq (${BUILD_FASTBOOT0},y)
$(eval $(call add_define,ENABLE_FASTBOOT0))
fip%: export NOR_INFO=01C0080000430000A93B060000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
else
fip%: export NOR_INFO=$(shell printf '%72s' | tr ' ' 'FF')
endif
fip%: export DDR_PARAM_TEST_PATH = test/cv181x/ddr_param.bin

# Defaults for keys dir and fipsign script
FIPSIGN_SCRIPT ?= ./plat/${CHIP_ARCH}/fipsign.py
FSBL_KEYS_DIR ?= ./plat/keys

ifeq (${CONFIG_ENABLE_EMMC_HW_RESET_QFN},y)
$(eval $(call add_define,CONFIG_ENABLE_EMMC_HW_RESET_QFN))
endif
${BUILD_PLAT}:
	@mkdir -p '${BUILD_PLAT}'

gen-chip-conf:
	$(print_target)
	${Q}mkdir -p '${BUILD_PLAT}'
	${Q}./plat/${CHIP_ARCH}/chip_conf.py ${CHIP_CONF_PATH}

macro_to_env = ${NM} '${BLMACROS_ELF}' | awk '/DEF_${1}/ { rc = 1; print "${1}=0x" $$1 } END { exit !rc }' >> ${BUILD_PLAT}/blmacros.env

blmacros-env: blmacros
	$(print_target)
	${Q}> ${BUILD_PLAT}/blmacros.env  # clear .env first
	${Q}$(call macro_to_env,MONITOR_RUNADDR)
	${Q}$(call macro_to_env,BLCP_2ND_RUNADDR)
ifeq (${BUILD_BOOT0},y)
$(eval $(call add_define,ENABLE_BOOT0))
fip: fip_boot0
else
fip: fip-all
endif

fip-dep: bl2 blmacros-env gen-chip-conf


fip_boot0: fip-dep
	$(print_target)
	${Q}echo "  [GEN] boot0"
	${Q}. ${BUILD_PLAT}/blmacros.env && \
	${FIPTOOL} -v genfip \
		'${BUILD_PLAT}/boot0' \
		--MONITOR_RUNADDR="$${MONITOR_RUNADDR}" \
		--BLCP_2ND_RUNADDR="$${BLCP_2ND_RUNADDR}" \
		--CHIP_CONF='${CHIP_CONF_PATH}' \
		--NOR_INFO='${NOR_INFO}' \
		--NAND_INFO='${NAND_INFO}'\
		--BL2='${BUILD_PLAT}/bl2.bin'\
		--LOADER_2ND_B='${LOADER_2ND_PATH_B}'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/boot0')
	${Q}cp ${BUILD_PLAT}/boot0 ${OUTPUT_DIR}

fip-simple: fip-dep
	$(print_target)
	${Q}echo "  [GEN] fip.bin"
	${Q}${FIPTOOL} -v genfip \
		'${BUILD_PLAT}/fip.bin' \
		--CHIP_CONF='${CHIP_CONF_PATH}' \
		--NOR_INFO='${NOR_INFO}' \
		--NAND_INFO='${NAND_INFO}'\
		--BL2='${BUILD_PLAT}/bl2.bin'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/fip.bin')

fip-all: fip-dep
	$(print_target)
	${Q}echo "  [GEN] fip.bin"
	${Q}. ${BUILD_PLAT}/blmacros.env && \
	${FIPTOOL} -v genfip \
		'${BUILD_PLAT}/fip.bin' \
		--MONITOR_RUNADDR="$${MONITOR_RUNADDR}" \
		--BLCP_2ND_RUNADDR="$${BLCP_2ND_RUNADDR}" \
		--CHIP_CONF='${CHIP_CONF_PATH}' \
		--NOR_INFO='${NOR_INFO}' \
		--NAND_INFO='${NAND_INFO}'\
		--BL2='${BUILD_PLAT}/bl2.bin' \
		--BLCP_IMG_RUNADDR=${BLCP_IMG_RUNADDR} \
		--BLCP_PARAM_LOADADDR=${BLCP_PARAM_LOADADDR} \
		--BLCP=${BLCP_PATH} \
		--BLCP_2ND='${BLCP_2ND_PATH}' \
		--MONITOR='${MONITOR_PATH}' \
		--LOADER_2ND='${LOADER_2ND_PATH}' \
		--compress='${FIP_COMPRESS}'\
		--LOADER_2ND_B='${LOADER_2ND_PATH_B}'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/fip.bin')
		--compress='${FIP_COMPRESS}'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/fip.bin')

ifeq (${FSBL_SECURE_BOOT_SUPPORT},1)
.PHONY: fip-sign fip-sign-enc boot0-sign boot0-sign-enc
ifeq (${BUILD_BOOT0},y)
fip:boot0-sign boot0-sign-enc
boot0-sign: fip_boot0
	$(print_target)
	${Q}echo "  [SIGN] boot0 -> boot0_signed"
	${Q}python3 ${FIPSIGN_SCRIPT} sign \
		--root-priv='${FSBL_KEYS_DIR}/rsa_hash0.pem' \
		--bl-priv='${FSBL_KEYS_DIR}/bl_priv.pem' \
		'${BUILD_PLAT}/boot0' \
		'${BUILD_PLAT}/boot0_signed'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/boot0_signed')
boot0-sign-enc: fip_boot0
	$(print_target)
	${Q}echo "  [SIGN-ENC] boot0 -> boot0_signed_enc"
	${Q}python3 ${FIPSIGN_SCRIPT} sign-enc \
		--root-priv='${FSBL_KEYS_DIR}/rsa_hash0.pem' \
		--bl-priv='${FSBL_KEYS_DIR}/bl_priv.pem' \
		--ldr-ek='${FSBL_KEYS_DIR}/root_ek.key' \
		--bl-ek='${FSBL_KEYS_DIR}/bl_ek.key' \
		'${BUILD_PLAT}/boot0' \
		'${BUILD_PLAT}/boot0_signed_enc'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/boot0_signed')
else
fip:fip-sign fip-sign-enc
fip-sign: fip-all
	$(print_target)
	${Q}echo "  [SIGN] fip.bin -> fip_signed.bin"
	${Q}python3 ${FIPSIGN_SCRIPT} sign \
		--root-priv='${FSBL_KEYS_DIR}/rsa_hash0.pem' \
		--bl-priv='${FSBL_KEYS_DIR}/bl_priv.pem' \
		'${BUILD_PLAT}/fip.bin' \
		'${BUILD_PLAT}/fip_signed.bin'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/fip_signed.bin')

fip-sign-enc: fip-all
	$(print_target)
	${Q}echo "  [SIGN-ENC] fip.bin -> fip_signed_enc.bin"
	${Q}python3 ${FIPSIGN_SCRIPT} sign-enc \
		--root-priv='${FSBL_KEYS_DIR}/rsa_hash0.pem' \
		--bl-priv='${FSBL_KEYS_DIR}/bl_priv.pem' \
		--ldr-ek='${FSBL_KEYS_DIR}/root_ek.key' \
		--bl-ek='${FSBL_KEYS_DIR}/bl_ek.key' \
		'${BUILD_PLAT}/fip.bin' \
		'${BUILD_PLAT}/fip_signed_enc.bin'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/fip_signed_enc.bin')
endif
endif
