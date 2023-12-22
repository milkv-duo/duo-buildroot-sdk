#include <striptool.h>

#ifdef OS_ABL_SUPPORT
static char *UTIL_LINK_FILE1[] = {
	"include/iface/iface_util.h",
};

static char *UTIL_LINK_FILE2[] = {
	"include/rtmp_type.h",
	"include/rtmp_os.h",
	"include/link_list.h",
	"include/rtmp_cmd.h",
	"include/rt_os_util.h",
	"include/rtmp_osabl.h",
	"include/rtmp_comm.h",
};

static char *UTIL_LINK_FILE3[] = {
	"include/os/rt_linux.h",
	"include/os/rt_os.h",
	"include/os/rt_linux_cmm.h",
};

static char *MODULE_LINK_FILE1[] = {
	"include/rt_os_net.h",
};

static char *MODULE_LINK_FILE2[] = {
	"include/chip/chip_id.h",
};
#endif // OS_ABL_SUPPORT //

int main(int argc ,char *argv[])
{
	char Command[255];
    int index;

	
	fnStripModule();

#ifdef OS_ABL_SUPPORT
	fnStripUtil();
	fnStripNetif();

	/* Create directories */
    sprintf(Command, "rm -Rf %s", RELEASE_PATH);
    system(Command);
    sprintf(Command, "mkdir %s", RELEASE_PATH);
    system(Command);

	/* build symbol link for Makefile */
	sprintf(Command, "mv %s/Makefile %s/Makefile.inc", RELEASE_MODULE_PATH, RELEASE_PATH);
	system(Command);
	sprintf(Command, "rm -f %s/Makefile", RELEASE_UTIL_PATH);
	system(Command);
	sprintf(Command, "rm -f %s/Makefile", RELEASE_NETIF_PATH);
	system(Command);

	sprintf(Command, "ln -s ../Makefile.inc %s/Makefile", RELEASE_MODULE_PATH);
	system(Command);
	sprintf(Command, "ln -s ../Makefile.inc %s/Makefile", RELEASE_UTIL_PATH);
	system(Command);
	sprintf(Command, "ln -s ../Makefile.inc %s/Makefile", RELEASE_NETIF_PATH);
	system(Command);

	/* build symbol link for Makefile.clean */
	sprintf(Command, "mv %s/os/linux/Makefile.clean %s/", RELEASE_MODULE_PATH, RELEASE_PATH);
	system(Command);

	sprintf(Command, "ln -s ../../../Makefile.clean %s/os/linux/Makefile.clean", RELEASE_MODULE_PATH);
	system(Command);
	sprintf(Command, "ln -s ../../../Makefile.clean %s/os/linux/Makefile.clean", RELEASE_UTIL_PATH);
	system(Command);
	sprintf(Command, "ln -s ../../../Makefile.clean %s/os/linux/Makefile.clean", RELEASE_NETIF_PATH);
	system(Command);

	/* build symbol link for config.mk */
	sprintf(Command, "mv %s/os/linux/config.mk %s/", RELEASE_MODULE_PATH, RELEASE_PATH);
	system(Command);
	sprintf(Command, "rm -f %s/os/linux/config.mk", RELEASE_UTIL_PATH);
	system(Command);
	sprintf(Command, "rm -f %s/os/linux/config.mk", RELEASE_NETIF_PATH);
	system(Command);

	sprintf(Command, "ln -s ../../../config.mk %s/os/linux/config.mk", RELEASE_MODULE_PATH);
	system(Command);
	sprintf(Command, "ln -s ../../../config.mk %s/os/linux/config.mk", RELEASE_UTIL_PATH);
	system(Command);
	sprintf(Command, "ln -s ../../../config.mk %s/os/linux/config.mk", RELEASE_NETIF_PATH);
	system(Command);

	/* build symbol link for include/iface between UTIL and MODULE */
    for (index = 0; index < sizeof(UTIL_LINK_FILE1)/sizeof(char *);index++)
    {
        sprintf(Command, "ln -s ../../../%s/%s %s/%s",
				RELEASE_UTIL_PATH, UTIL_LINK_FILE1[index],
				RELEASE_MODULE_PATH, UTIL_LINK_FILE1[index]);
        system(Command);
    }

	/* build symbol link for include/iface between UTIL and NETIF */
    for (index = 0; index < sizeof(UTIL_LINK_FILE1)/sizeof(char *);index++)
    {
        sprintf(Command, "ln -s ../../../%s/%s %s/%s",
				RELEASE_UTIL_PATH, UTIL_LINK_FILE1[index],
				RELEASE_NETIF_PATH, UTIL_LINK_FILE1[index]);
        system(Command);
    }

	/* build symbol link for include between UTIL and MODULE */
    for (index = 0; index < sizeof(UTIL_LINK_FILE2)/sizeof(char *);index++)
    {
        sprintf(Command, "ln -s ../../%s/%s %s/%s",
				RELEASE_UTIL_PATH, UTIL_LINK_FILE2[index],
				RELEASE_MODULE_PATH, UTIL_LINK_FILE2[index]);
        system(Command);
    }

	/* build symbol link for include between UTIL and NETIF */
    for (index = 0; index < sizeof(UTIL_LINK_FILE2)/sizeof(char *);index++)
    {
        sprintf(Command, "ln -s ../../%s/%s %s/%s",
				RELEASE_UTIL_PATH, UTIL_LINK_FILE2[index],
				RELEASE_NETIF_PATH, UTIL_LINK_FILE2[index]);
        system(Command);
    }

	/* build symbol link for include/os between UTIL and NETIF */
    for (index = 0; index < sizeof(UTIL_LINK_FILE3)/sizeof(char *);index++)
    {
        sprintf(Command, "ln -s ../../../%s/%s %s/%s",
				RELEASE_UTIL_PATH, UTIL_LINK_FILE3[index],
				RELEASE_NETIF_PATH, UTIL_LINK_FILE3[index]);
        system(Command);
    }

	/* build symbol link for include between MODULE and NETIF */
    for (index = 0; index < sizeof(MODULE_LINK_FILE1)/sizeof(char *);index++)
    {
        sprintf(Command, "ln -s ../../%s/%s %s/%s",
				RELEASE_MODULE_PATH, MODULE_LINK_FILE1[index],
				RELEASE_NETIF_PATH, MODULE_LINK_FILE1[index]);
        system(Command);
    }

    for (index = 0; index < sizeof(MODULE_LINK_FILE2)/sizeof(char *);index++)
    {
        sprintf(Command, "ln -s ../../../%s/%s %s/%s",
				RELEASE_MODULE_PATH, MODULE_LINK_FILE2[index],
				RELEASE_NETIF_PATH, MODULE_LINK_FILE2[index]);
        system(Command);
    }

	/* move UTIL/MODULE/NETIF to release directory */
	sprintf(Command, "mv %s %s/ ; mv %s %s/; mv %s %s/", 
						RELEASE_MODULE_PATH, RELEASE_PATH,
						RELEASE_UTIL_PATH, RELEASE_PATH,
						RELEASE_NETIF_PATH, RELEASE_PATH);

	system(Command);

	sprintf(Command, "cp -f Makefile.OSABL %s/Makefile;", RELEASE_PATH);
    system(Command);

	sprintf(Command, "cp -f cp_module.sh %s/; cp -f cp_util.sh %s/;", 
						RELEASE_PATH, RELEASE_PATH);
    system(Command);
#endif // OS_ABL_SUPPORT //

    return 0;
}

