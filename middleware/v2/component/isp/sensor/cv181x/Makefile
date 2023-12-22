SHELL = /bin/bash

ifeq ($(PARAM_FILE), )
PARAM_FILE=../../../../../$(shell echo $(MW_VER))/Makefile.param
include $(PARAM_FILE)
endif
include ../../../../../$(shell echo $(MW_VER))/component/isp/sensor.mk

define MAKE_SENSOR
	pushd $(1) && \
	$(MAKE) all && \
	popd
endef

.PHONY : prepare clean $(sensor-y)
all: prepare $(sensor-y) all_sensor

prepare:
	@echo "#################################################"
	@echo "#"
	@echo "# Compiling 'component libs' Configs as below..."
	@echo "# SENSOR_LIST=$(sensor-y)"
	@echo "#"
	@echo "#################################################"

brigates_bg0808:
	$(call MAKE_SENSOR, ${@})

gcore_gc02m1:
	$(call MAKE_SENSOR, ${@})
gcore_gc0312:
	$(call MAKE_SENSOR, ${@})

gcore_gc0329:
	$(call MAKE_SENSOR, ${@})

gcore_gc1054:
	$(call MAKE_SENSOR, ${@})

gcore_gc2053:
	$(call MAKE_SENSOR, ${@})

gcore_gc2053_slave:
	$(call MAKE_SENSOR, ${@})

gcore_gc2053_1L:
	$(call MAKE_SENSOR, ${@})

gcore_gc2083:
	$(call MAKE_SENSOR, ${@})

gcore_gc2093:
	$(call MAKE_SENSOR, ${@})

gcore_gc2145:
	$(call MAKE_SENSOR, ${@})

gcore_gc4023:
	$(call MAKE_SENSOR, ${@})

gcore_gc4653:
	$(call MAKE_SENSOR, ${@})

imgds_mis2008:
	$(call MAKE_SENSOR, ${@})

nextchip_n5:
	$(call MAKE_SENSOR, ${@})

nextchip_n6:
	$(call MAKE_SENSOR, ${@})

ov_os04a10:
	$(call MAKE_SENSOR, ${@})

ov_os04c10:
	$(call MAKE_SENSOR, ${@})

ov_os08a20:
	$(call MAKE_SENSOR, ${@})

ov_ov4689:
	$(call MAKE_SENSOR, ${@})

ov_ov5647:
	$(call MAKE_SENSOR, ${@})

ov_ov6211:
	$(call MAKE_SENSOR, ${@})

ov_ov7251:
	$(call MAKE_SENSOR, ${@})

pixelplus_pr2020:
	$(call MAKE_SENSOR, ${@})

pixelplus_pr2100:
	$(call MAKE_SENSOR, ${@})

sms_sc035gs:
	$(call MAKE_SENSOR, ${@})

sms_sc035gs_1L:
	$(call MAKE_SENSOR, ${@})

sms_sc035hgs:
	$(call MAKE_SENSOR, ${@})

sms_sc1346_1L:
	$(call MAKE_SENSOR, ${@})

sms_sc1346_1L_slave:
	$(call MAKE_SENSOR, ${@})

sms_sc200ai:
	$(call MAKE_SENSOR, ${@})

sms_sc301iot:
	$(call MAKE_SENSOR, ${@})

sms_sc401ai:
	$(call MAKE_SENSOR, ${@})

sms_sc500ai:
	$(call MAKE_SENSOR, ${@})

sms_sc501ai_2L:
	$(call MAKE_SENSOR, ${@})

sms_sc3332:
	$(call MAKE_SENSOR, ${@})

sms_sc531ai_2L:
	$(call MAKE_SENSOR, ${@})

sms_sc3335:
	$(call MAKE_SENSOR, ${@})

sms_sc3336:
	$(call MAKE_SENSOR, ${@})

sms_sc2331_1L:
	$(call MAKE_SENSOR, ${@})

sms_sc2335:
	$(call MAKE_SENSOR, ${@})

sms_sc2336:
	$(call MAKE_SENSOR, ${@})

sms_sc2336p:
	$(call MAKE_SENSOR, ${@})

sms_sc4336:
	$(call MAKE_SENSOR, ${@})

sms_sc4336p:
	$(call MAKE_SENSOR, ${@})

sms_sc5336_2L:
	$(call MAKE_SENSOR, ${@})

soi_f23:
	$(call MAKE_SENSOR, ${@})

soi_f35:
	$(call MAKE_SENSOR, ${@})

soi_f37p:
	$(call MAKE_SENSOR, ${@})

soi_q03:
	$(call MAKE_SENSOR, ${@})

soi_q03p:
	$(call MAKE_SENSOR, ${@})

soi_k06:
	$(call MAKE_SENSOR, ${@})

sony_imx307:
	$(call MAKE_SENSOR, ${@})

sony_imx307_slave:
	$(call MAKE_SENSOR, ${@})

sony_imx307_2L:
	$(call MAKE_SENSOR, ${@})

sony_imx327:
	$(call MAKE_SENSOR, ${@})

sony_imx327_slave:
	$(call MAKE_SENSOR, ${@})

sony_imx327_2L:
	$(call MAKE_SENSOR, ${@})

sony_imx327_fpga:
	$(call MAKE_SENSOR, ${@})

sony_imx327_sublvds:
	$(call MAKE_SENSOR, ${@})

sony_imx335:
	$(call MAKE_SENSOR, ${@})

techpoint_tp2825:
	$(call MAKE_SENSOR, ${@})
	
techpoint_tp2863:
	$(call MAKE_SENSOR, ${@})

lontium_lt6911:
	$(call MAKE_SENSOR, ${@})

all_sensor:
	@$(MAKE) -f Makefile_full || exit 1;

clean:
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" `; \
	    do cd `dirname $$x`; if [ $$? ]; then $(MAKE) clean; cd -; fi; done
	@echo "#"
	@$(MAKE) clean -f Makefile_full || exit 1;
