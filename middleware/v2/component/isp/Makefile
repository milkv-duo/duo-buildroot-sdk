SHELL = /bin/bash

include $(BUILD_PATH)/.config
include sensor.mk

chip_arch = $(shell echo $(CVIARCH) | tr A-Z a-z)

all:
	pushd sensor/$(chip_arch) && \
	$(MAKE) all && \
	popd;

clean:
	pushd sensor/$(chip_arch) && \
	$(MAKE) clean && \
	popd;
