rtos: memory-map
	$(call print_target)
ifeq ($(CHIP_ARCH_L),$(filter $(CHIP_ARCH_L), cv180x))
	cd ${FREERTOS_PATH}/cvitek && ./build_cv180x.sh
else
	cd ${FREERTOS_PATH}/cvitek && ./build_cv181x.sh
endif

rtos-clean:
ifeq (${CONFIG_ENABLE_FREERTOS},y)
	$(call print_target)
	cd ${FREERTOS_PATH}/cvitek && rm -rf build 
endif
