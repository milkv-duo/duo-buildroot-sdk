#ifdef OS_ABL_SUPPORT
#include <striptool.h>

#define BUILD_NETIF_DIRECTORY "build_netif"

static char *RELEASE_DIRECTORY[] = {
	"include",
    "include/iface",
    "include/os",
    "include/chip",
};

static char *RELEASE_DIRECTORY2[] = {
	"ap",
	"os",
    "os/linux",
    "common",
#ifdef INCLUDE_CFG80211_SUPPORT
        "os/linux/cfg80211",
#endif /* INCLUDE_CFG80211_SUPPORT */
};

static char *RELEASE_FILE[] = {
#ifdef INCLUDE_AP
	"ap/ap_mbss_inf.c",
	"os/linux/ap_ioctl.c",
	"os/linux/rt_main_dev.c",
#ifndef INCLUDE_P2P
	"ap/ap_wds_inf.c",
#endif // INCLUDE_P2P //
	"ap/ap_apcli_inf.c",
#endif // INCLUDE_AP //
#ifdef INCLUDE_STA
	"os/linux/sta_ioctl.c",
	"os/linux/rt_main_dev.c",
#endif // INCLUDE_STA //
#ifdef INCLUDE_MESH
	"common/mesh_inf.c",
#endif // INCLUDE_MESH //
#ifdef INCLUDE_P2P
	"common/p2p_inf.c",
#endif // INCLUDE_P2P //
#ifdef RTMP_USB_SUPPORT
	"common/rtusb_dev_id.c",
	"os/linux/usb_main_dev.c",
#endif // RTMP_USB_SUPPORT //
#ifdef RTMP_PCI_SUPPORT
	"os/linux/rt_pci_rbus.c",
	"os/linux/pci_main_dev.c",
#endif // RTMP_PCI_SUPPORT //
	"os/linux/cfg80211.c",
	"os/linux/config.mk",
	"os/linux/Makefile.6.netif",
	"os/linux/Makefile.4.netif",
	"include/cfg80211.h",
	"include/cfg80211extr.h",
};

static char *INCLUDE_FILE[] = {
};

int fnStripNetif(void)
{
    FILE *ConfigFile;
    char Command[255], Line[2000];;
    int index;

    /* create directories */
    sprintf(Command, "rm -Rf %s", BUILD_NETIF_DIRECTORY);
    system(Command);
    sprintf(Command, "mkdir %s", BUILD_NETIF_DIRECTORY);
    system(Command);

	sprintf(Command, "cp -f Makefile.netif %s/Makefile", BUILD_NETIF_DIRECTORY);
    system(Command);
   
    for (index = 0; index < sizeof(RELEASE_DIRECTORY)/sizeof(char *);index++)
    {
        sprintf(Command, "mkdir %s/%s", BUILD_NETIF_DIRECTORY, RELEASE_DIRECTORY[index]);
        system(Command);
    } /* End of for */

	for (index = 0; index < sizeof(RELEASE_DIRECTORY2)/sizeof(char *);index++)
    {
        sprintf(Command, "mkdir %s/%s", BUILD_NETIF_DIRECTORY, RELEASE_DIRECTORY2[index]);
        system(Command);
    } /* End of for */

	/* cp header files */
    for (index = 0; index < sizeof(INCLUDE_FILE)/sizeof(char *);index++)
    {
        sprintf(Command, "cp -f %s %s/%s", INCLUDE_FILE[index], BUILD_NETIF_DIRECTORY, INCLUDE_FILE[index]);
        system(Command);
    } /* End of for */

    /* remove all CVS directory */
    sprintf(Command, "find %s -name CVS -prune -exec rm -rf {} \\;", BUILD_NETIF_DIRECTORY);
    system(Command);

    /* copy file */
    for (index = 0; index < sizeof(RELEASE_FILE)/sizeof(char *);index++)
    {
        sprintf(Command, "cp -f %s %s/%s", RELEASE_FILE[index], BUILD_NETIF_DIRECTORY, RELEASE_FILE[index]);
        system(Command);
    } /* End of for */

	fnCommStrip(BUILD_NETIF_DIRECTORY, RELEASE_NETIF_PATH);

remove_build:
    sprintf(Command, "rm -Rf %s", BUILD_NETIF_DIRECTORY);
    system(Command);
	
    return 0;
} /* End of main */
#endif // OS_ABL_SUPPORT //

