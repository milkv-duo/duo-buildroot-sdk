DUO_PINMUX_SITE = $(TOPDIR)/package/duo-pinmux/src
DUO_PINMUX_VERSION = 1.0.0
DUO_PINMUX_SITE_METHOD = local

define DUO_PINMUX_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		$(@D)/*.c -o $(@D)/duo-pinmux
endef

define DUO_PINMUX_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/duo-pinmux $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
