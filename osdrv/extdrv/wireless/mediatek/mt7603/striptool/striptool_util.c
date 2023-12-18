#ifdef OS_ABL_SUPPORT

#include <striptool.h>

#define BUILD_UTIL_DIRECTORY "build_util"

static char *RELEASE_DIRECTORY[] = {
	"include",
    "include/iface",
    "include/os",
};

static char *RELEASE_DIRECTORY2[] = {
	"common",
	"os",
    "os/linux",
#ifdef INCLUDE_CFG80211_SUPPORT
        "os/linux/cfg80211",
#endif /* INCLUDE_CFG80211_SUPPORT */
};

static char *RELEASE_FILE[] = {
	"common/rt_os_util.c",
	"os/linux/rt_linux_symb.c",
	"os/linux/rt_linux.c",
	"os/linux/rt_usb_util.c",
	"os/linux/rt_rbus_pci_util.c",
#ifdef PLATFORM_BL2348
	"os/linux/vr_bdlt.c",
#endif // PLATFORM_BL2348 //
#ifdef BG_FT_SUPPORT
	"os/linux/br_ftph.c",
#endif // BG_FT_SUPPORT //
	"os/linux/Makefile.6.util",
	"os/linux/Makefile.4.util",
};

static char *INCLUDE_FILE[] = {
	"include/iface/iface_util.h",
	"include/os/rt_linux.h",
	"include/os/rt_linux_cmm.h",
	"include/os/rt_os.h",
	"include/rtmp_type.h",
	"include/rtmp_os.h",
	"include/link_list.h",
	"include/rtmp_cmd.h",
	"include/rt_os_util.h",
	"include/rtmp_osabl.h",
	"include/rtmp_comm.h",
	"include/cfg80211.h",
	"include/cfg80211extr.h",
};

int fnStripUtil(void)
{
    FILE *ConfigFile;
    char Command[255], Line[2000];;
    int index;

    /* create directories */
    sprintf(Command, "rm -Rf %s", BUILD_UTIL_DIRECTORY);
    system(Command);
    sprintf(Command, "mkdir %s", BUILD_UTIL_DIRECTORY);
    system(Command);

	sprintf(Command, "cp -f Makefile.util %s/Makefile", BUILD_UTIL_DIRECTORY);
    system(Command);
   
    for (index = 0; index < sizeof(RELEASE_DIRECTORY)/sizeof(char *);index++)
    {
        sprintf(Command, "mkdir %s/%s", BUILD_UTIL_DIRECTORY, RELEASE_DIRECTORY[index]);
        system(Command);
    } /* End of for */

	for (index = 0; index < sizeof(RELEASE_DIRECTORY2)/sizeof(char *);index++)
    {
        sprintf(Command, "mkdir %s/%s", BUILD_UTIL_DIRECTORY, RELEASE_DIRECTORY2[index]);
        system(Command);
    } /* End of for */

	/* cp header files */
    for (index = 0; index < sizeof(INCLUDE_FILE)/sizeof(char *);index++)
    {
        sprintf(Command, "cp -f %s %s/%s", INCLUDE_FILE[index], BUILD_UTIL_DIRECTORY, INCLUDE_FILE[index]);
        system(Command);
    } /* End of for */

    /* remove all CVS directory */
    sprintf(Command, "find %s -name CVS -prune -exec rm -rf {} \\;", BUILD_UTIL_DIRECTORY);
    system(Command);

    /* copy file */
    for (index = 0; index < sizeof(RELEASE_FILE)/sizeof(char *);index++)
    {
        sprintf(Command, "cp -f %s %s/%s", RELEASE_FILE[index], BUILD_UTIL_DIRECTORY, RELEASE_FILE[index]);
        system(Command);
    } /* End of for */

	fnCommStrip(BUILD_UTIL_DIRECTORY, RELEASE_UTIL_PATH);

remove_build:
    sprintf(Command, "rm -Rf %s", BUILD_UTIL_DIRECTORY);
    system(Command);
	
    return 0;
} /* End of main */
#endif // OS_ABL_SUPPORT //

