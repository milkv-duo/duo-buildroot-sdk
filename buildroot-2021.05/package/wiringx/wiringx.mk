WIRINGX_SITE = $(TOPDIR)/package/wiringx
WIRINGX_VERSION = 2023.05.30
WIRINGX_SITE_METHOD = local

WIRINGX_INSTALL_STAGING = YES

define WIRINGX_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(@D)/src/wiringx.h $(STAGING_DIR)/usr/include/
	$(INSTALL) -D -m 0644 $(@D)/src/libwiringx.so $(STAGING_DIR)/usr/lib/
endef

define WIRINGX_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/src/libwiringx.so $(TARGET_DIR)/usr/lib/
endef

$(eval $(generic-package))
