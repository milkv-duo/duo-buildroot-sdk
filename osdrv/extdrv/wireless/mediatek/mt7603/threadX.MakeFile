################################################################################
## MAKEFILE
## LIB:         RT_WIFI
## DESCRIPTION: library to control the swath generator
################################################################################
!include "..\config.mak"

################################################################################
## PREDEFINES - neeeded for MakefileLib
################################################################################
# Library Name
LIBNAME = rt_wifi

# SDK Header
SDK_HEADER = -ID:\code_dev\tronfirmware\firmware\dc_sdk\public_include

#MODE - STA or AP or APSTA.
RT28xx_MODE = STA

#TARTET - THREADX.
TARGET = THREADX

#CHIPSET - 2860 or 2870 or 2880 or 2070 or 3070 or 3090 or 3572 or 3062 or 3562
CHIPSET = 3370

#PLATFORM: Target platform
PLATFORM = KODAK_DC

#RELEASE Package
RELEASE = DPA

#Include RT_WIFI Makfile
!include "os\threadX\Makefile.nmk" 

# Objects
VARIANT_OBJ_C = $(RT_WIFI_OBJS)
	
OBJf = $(VARIANT_OBJ_C)

# CFLAGS

# Module CFLAGS
MODULE_CFLAGS = $(SDK_HEADER) $(RT_WIFI_CFLAGS) -DENABLE_WIFI

# Dependency files
DEPSf =

# Directory Paths
FWDIR = $(MAKEDIR)\..
OALIBDIR = $(FWDIR)\oalib

!if "$(ARCH)" == "ARM9_RV"
MODULE_CFLAGS = --gnu --c99 --signed_chars
!endif

################################################################################
## INCLUDES
################################################################################
!include "$(OALIBDIR)\MakefileLib"

################################################################################
## DEFINES
################################################################################

################################################################################
## RULES
################################################################################

# Setup Rules for source files that do not reside in the same directory as their related object file

$(VARIANT_OBJ_C) : $(DEPSf) deps
	$(CC)  $(SDK_HEADER) $(RT_WIFI_CFLAGS) $(CFLAGS) -o $*.o -c $(*R).c

################################################################################
## TARGETS
################################################################################
hal_fw: $(LIBNAME)

mfp_fw: $(LIBNAME)

!IF "$(NETSWITCH)"=="DISABLE_WIFI"
rt_wifi: 
    @echo **** WIFI DISABLED! ****
!ELSE
rt_wifi: 
    @echo **** START RT_WIFI building **** 
    set TARGET_LIBRARY=$(LIBDIR)\$(*B)       
    set DELIVERY_MODULE=$(DELIVERY_RT_WIFI) 
    $(MAKE) /$(MAKEFLAGS) $(LIBDIR)\$(*B).l
    @echo **** DONE RT_WIFI  ****
!ENDIF

delivery:
    @echo **** DELIVERY RT_WIFI ****
    set DELIVERY_MODULE=$(DELIVERY_RT_WIFI)
    $(MAKE) /$(MAKEFLAGS) delivery_map

clean:
    set DELIVERY_MODULE=$(DELIVERY_RT_WIFI)
    $(MAKE) /$(MAKEFLAGS) clean_map

depreport:
    cd $(FWDIR)
    deliveryBuilder -s $(NWK_INITDIR) -report -d depdir
