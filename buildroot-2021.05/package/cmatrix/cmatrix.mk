###############################################################################
#
# Cmatrix
#
###############################################################################

CMATRIX_VERSION = 5c082c64a1296859a11bee60c8c086655953a416
CMATRIX_SITE = $(call github,abishekvashok,cmatrix,$(CMATRIX_VERSION))
CMATRIX_SITE_LICENSE = GNU GPL v3
CMATRIX_SITE_LICENSE_FILES = COPYING

CMATRIX_DEPENDENCIES = ncurses

CMATRIX_CFLAGS = $(TARGET_CFLAGS)
CMATRIX_LDFLAGS = $(TARGET_LDFLAGS) -lncursesw
CMATRIX_CFLAGS += -DHAVE_SETFONT

define CMATRIX_BUILD_CMDS
	(cd $(@D); \
	gunzip -c matrix.psf.gz > matrix.psf; \
	autoreconf -i; \
	./configure --host=$(BR2_TOOLCHAIN_EXTERNAL_PREFIX) CC=$(TARGET_CC) LD=$(TARGET_LD) --without-fonts; \
	make \
	CROSS_COMPILE=$(TARGET_CROSS) \
	CFLAGS='$(CMATRIX_CFLAGS)' \
	LDFLAGS='$(CMATRIX_LDFLAGS)' \
	)
endef

define CMATRIX_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/cmatrix $(TARGET_DIR)/usr/bin/cmatrix
	$(INSTALL) -D -m 0755 $(@D)/matrix.psf $(TARGET_DIR)/usr/share/fonts/matrix.psf
endef

$(eval $(generic-package))
