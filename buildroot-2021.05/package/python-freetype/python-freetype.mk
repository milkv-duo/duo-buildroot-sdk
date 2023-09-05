################################################################################
#
# freetype-py
#
################################################################################

PYTHON_FREETYPE_VERSION = 2.4.0
PYTHON_FREETYPE_SOURCE = freetype-py-$(PYTHON_FREETYPE_VERSION).zip
PYTHON_FREETYPE_SITE = https://files.pythonhosted.org/packages/a7/6d/66c222fae5dc5fd13fd840ab75feb49f78963f560a56e134c85f1236a94c
PYTHON_FREETYPE_SETUP_TYPE = setuptools
PYTHON_FREETYPE_LICENSE = Revised BSD License
PYTHON_FREETYPE_LICENSE_FILES = LICENSE.txt

define PYTHON_FREETYPE_EXTRACT_CMDS
	$(UNZIP) -d $(@D) $(PYTHON_FREETYPE_DL_DIR)/$(PYTHON_FREETYPE_SOURCE)
	mv $(@D)/freetype-py-$(PYTHON_FREETYPE_VERSION)/* $(@D)
	$(RM) -r $(@D)/freetype-py-$(PYTHON_FREETYPE_VERSION)
endef

$(eval $(python-package))
