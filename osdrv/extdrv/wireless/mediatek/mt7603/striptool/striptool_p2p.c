#ifdef OS_ABL_SUPPORT
#include <striptool_p2p.h>

#define BUILD_CMD_DIRECTORY "build_cmd"

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
};

static char *RELEASE_FILE[] = {
	"common/cmm_profile.c",
	"common/cmm_info.c",
};

static char *INCLUDE_FILE[] = {
};

int fnStripCmd(void)
{
    FILE *ConfigFile;
    char Command[255], Line[2000];;
    int index;

    /* create directories */
    sprintf(Command, "rm -Rf %s", BUILD_CMD_DIRECTORY);
    system(Command);
    sprintf(Command, "mkdir %s", BUILD_CMD_DIRECTORY);
    system(Command);	
   
//	sprintf(Command, "cp -f Makefile.netif %s/Makefile", BUILD_NETIF_DIRECTORY);
//    system(Command);
   
    for (index = 0; index < sizeof(RELEASE_DIRECTORY)/sizeof(char *);index++)
    {
        sprintf(Command, "mkdir %s/%s", BUILD_CMD_DIRECTORY, RELEASE_DIRECTORY[index]);
        system(Command);
    } /* End of for */

	for (index = 0; index < sizeof(RELEASE_DIRECTORY2)/sizeof(char *);index++)
    {
        sprintf(Command, "mkdir %s/%s", BUILD_CMD_DIRECTORY, RELEASE_DIRECTORY2[index]);
        system(Command);
    } /* End of for */

	/* cp header files */
    for (index = 0; index < sizeof(INCLUDE_FILE)/sizeof(char *);index++)
    {
        sprintf(Command, "cp -f %s %s/%s", INCLUDE_FILE[index], BUILD_CMD_DIRECTORY, INCLUDE_FILE[index]);
        system(Command);
    } /* End of for */

    /* remove all CVS directory */
    sprintf(Command, "find %s -name CVS -prune -exec rm -rf {} \\;", BUILD_CMD_DIRECTORY);
    system(Command);

    /* copy file */
    for (index = 0; index < sizeof(RELEASE_FILE)/sizeof(char *);index++)
    {
        sprintf(Command, "cp -f %s %s/%s", RELEASE_FILE[index], BUILD_CMD_DIRECTORY, RELEASE_FILE[index]);
        system(Command);
    } /* End of for */

	fnCommStrip(BUILD_CMD_DIRECTORY, RELEASE_CMD_PATH);

remove_build:
//    sprintf(Command, "rm -Rf %s", BUILD_CMD_DIRECTORY);
//    system(Command);
	
    return 0;
} /* End of main */
#endif // OS_ABL_SUPPORT //

