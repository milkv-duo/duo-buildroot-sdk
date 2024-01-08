################################################################################
#
# ncnn
#
################################################################################

NCNN_VERSION = 20240102
NCNN_SITE = $(call github,Tencent,ncnn,$(NCNN_VERSION))
NCNN_LICENSE = BSD 3-Clause License
NCNN_LICENSE_FILES = LICENSE.txt

NCNN_CFLAGS = $(TARGET_CFLAGS)
NCNN_CFLAGS += -static
NCNN_CXXFLAGS = $(TARGET_CXXFLAGS)
NCNN_CXXFLAGS += -static
NCNN_LDFLAGS = $(TARGET_LDFLAGS)

NCNN_CONF_OPTS = \
	-DCMAKE_SYSTEM_NAME="Linux" \
	-DCMAKE_SYSTEM_PROCESSOR="riscv64" \
	-DC906=True

NCNN_CONF_OPTS += \
	-DCMAKE_C_FLAGS="$(NCNN_CFLAGS)" \
	-DCMAKE_CXX_FLAGS="$(NCNN_CXXFLAGS)" \
	-DCMAKE_LD_FLAGS="$(NCNN_LDFLAGS)" \
	-DCMAKE_BUILD_TYPE=release

NCNN_CONF_OPTS += \
	-DNCNN_OPENMP=OFF \
	-DNCNN_THREADS=OFF \
	-DNCNN_RUNTIME_CPU=OFF \
	-DNCNN_RVV=ON \
	-DNCNN_SIMPLEOCV=ON \
	-DNCNN_BUILD_TESTS=OFF

ifeq ($(BR2_PACKAGE_NCNN_BENCH),y)
NCNN_CONF_OPTS += -DNCNN_BUILD_BENCHMARK=ON
else
NCNN_CONF_OPTS += -DNCNN_BUILD_BENCHMARK=OFF
endif

ifeq ($(BR2_PACKAGE_NCNN_EXAMPLE),y)
NCNN_CONF_OPTS += -DNCNN_BUILD_EXAMPLES=ON
else
NCNN_CONF_OPTS += -DNCNN_BUILD_EXAMPLES=OFF
endif

define NCNN_CREAT_FOLDER
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/usr/local/share/ncnn
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/usr/local/share/ncnn/param
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/usr/local/share/ncnn/images
endef

define NCNN_COPY_EXAMPLE_FILES
	$(INSTALL) -D -m 0755 $(@D)/examples/squeezenet $(TARGET_DIR)/usr/bin/ncnn-squeezenet
	$(INSTALL) -D -m 0755 $(@D)/examples/squeezenet_c_api $(TARGET_DIR)/usr/bin/ncnn-squeezenet_c_api
	$(INSTALL) -D -m 0755 $(@D)/examples/fasterrcnn $(TARGET_DIR)/usr/bin/ncnn-fasterrcnn
	$(INSTALL) -D -m 0755 $(@D)/examples/rfcn $(TARGET_DIR)/usr/bin/ncnn-rfcn
	$(INSTALL) -D -m 0755 $(@D)/examples/yolov2 $(TARGET_DIR)/usr/bin/ncnn-yolov2
	$(INSTALL) -D -m 0755 $(@D)/examples/yolov3 $(TARGET_DIR)/usr/bin/ncnn-yolov3
	$(INSTALL) -D -m 0755 $(@D)/examples/yolov5 $(TARGET_DIR)/usr/bin/ncnn-yolov5
	$(INSTALL) -D -m 0755 $(@D)/examples/yolov5_pnnx $(TARGET_DIR)/usr/bin/ncnn-yolov5_pnnx
	$(INSTALL) -D -m 0755 $(@D)/examples/yolov7_pnnx $(TARGET_DIR)/usr/bin/ncnn-yolov7_pnnx
	$(INSTALL) -D -m 0755 $(@D)/examples/yolov7 $(TARGET_DIR)/usr/bin/ncnn-yolov7
	$(INSTALL) -D -m 0755 $(@D)/examples/yolox $(TARGET_DIR)/usr/bin/ncnn-yolox
	$(INSTALL) -D -m 0755 $(@D)/examples/mobilenetv2ssdlite $(TARGET_DIR)/usr/bin/ncnn-mobilenetv2ssdlite
	$(INSTALL) -D -m 0755 $(@D)/examples/mobilenetssd $(TARGET_DIR)/usr/bin/ncnn-mobilenetssd
	$(INSTALL) -D -m 0755 $(@D)/examples/squeezenetssd $(TARGET_DIR)/usr/bin/ncnn-squeezenetssd
	$(INSTALL) -D -m 0755 $(@D)/examples/shufflenetv2 $(TARGET_DIR)/usr/bin/ncnn-shufflenetv2
	$(INSTALL) -D -m 0755 $(@D)/examples/peleenetssd_seg $(TARGET_DIR)/usr/bin/ncnn-peleenetssd_seg
	$(INSTALL) -D -m 0755 $(@D)/examples/simplepose $(TARGET_DIR)/usr/bin/ncnn-simplepose
	$(INSTALL) -D -m 0755 $(@D)/examples/retinaface $(TARGET_DIR)/usr/bin/ncnn-retinaface
	$(INSTALL) -D -m 0755 $(@D)/examples/yolact $(TARGET_DIR)/usr/bin/ncnn-yolact
	$(INSTALL) -D -m 0755 $(@D)/examples/nanodet $(TARGET_DIR)/usr/bin/ncnn-nanodet
	$(INSTALL) -D -m 0755 $(@D)/examples/nanodetplus_pnnx $(TARGET_DIR)/usr/bin/ncnn-nanodetplus_pnnx
	$(INSTALL) -D -m 0755 $(@D)/examples/scrfd $(TARGET_DIR)/usr/bin/ncnn-scrfd
	$(INSTALL) -D -m 0755 $(@D)/examples/scrfd_crowdhuman $(TARGET_DIR)/usr/bin/ncnn-scrfd_crowdhuman
	$(INSTALL) -D -m 0755 $(@D)/images/* $(TARGET_DIR)/usr/local/share/ncnn/images
	$(INSTALL) -D -m 0755 $(@D)/examples/*.param $(TARGET_DIR)/usr/local/share/ncnn/param
	$(INSTALL) -D -m 0755 $(@D)/examples/*.bin $(TARGET_DIR)/usr/local/share/ncnn/param
endef

define NCNN_COPY_BENCH_FILES
	$(INSTALL) -D -m 0755 $(@D)/benchmark/benchncnn $(TARGET_DIR)/usr/bin/benchncnn
	$(INSTALL) -D -m 0755 $(@D)/benchmark/*.param $(TARGET_DIR)/usr/local/share/ncnn/param
endef

NCNN_POST_INSTALL_TARGET_HOOKS += NCNN_CREAT_FOLDER

ifeq ($(BR2_PACKAGE_NCNN_BENCH),y)
NCNN_POST_INSTALL_TARGET_HOOKS += NCNN_COPY_BENCH_FILES
endif

ifeq ($(BR2_PACKAGE_NCNN_EXAMPLE),y)
NCNN_POST_INSTALL_TARGET_HOOKS += NCNN_COPY_EXAMPLE_FILES
endif

$(eval $(cmake-package))
