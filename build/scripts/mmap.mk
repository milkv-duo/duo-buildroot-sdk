.PHONY: memory-map

CVI_BOARD_MEMMAP_H_PATH := ${BUILD_PATH}/output/${PROJECT_FULLNAME}/cvi_board_memmap.h
CVI_BOARD_MEMMAP_CONF_PATH := ${BUILD_PATH}/output/${PROJECT_FULLNAME}/cvi_board_memmap.conf
CVI_BOARD_MEMMAP_LD_PATH:= ${BUILD_PATH}/output/${PROJECT_FULLNAME}/cvi_board_memmap.ld

BOARD_MMAP_PATH := ${BORAD_FOLDER_PATH}/memmap.py
MMAP_CONV_PY := ${BUILD_PATH}/scripts/mmap_conv.py


${CVI_BOARD_MEMMAP_H_PATH}: ${BOARD_MMAP_PATH} ${MMAP_CONV_PY}
	$(call print_target)
	mkdir -p $(dir $@)
	@${MMAP_CONV_PY} --type h $< $@

${CVI_BOARD_MEMMAP_CONF_PATH}: ${BOARD_MMAP_PATH} ${MMAP_CONV_PY}
	$(call print_target)
	@mkdir -p $(dir $@)
	@${MMAP_CONV_PY} --type conf $< $@

${CVI_BOARD_MEMMAP_LD_PATH}: ${BOARD_MMAP_PATH} ${MMAP_CONV_PY}
	$(call print_target)
	@mkdir -p $(dir $@)
	@${MMAP_CONV_PY} --type ld $< $@

ifeq ($(wildcard ${BOARD_MMAP_PATH}),)
memory-map:
else
memory-map: ${CVI_BOARD_MEMMAP_H_PATH} ${CVI_BOARD_MEMMAP_CONF_PATH} ${CVI_BOARD_MEMMAP_LD_PATH}
endif
