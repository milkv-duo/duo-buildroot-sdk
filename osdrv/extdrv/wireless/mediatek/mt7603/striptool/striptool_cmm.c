#include <striptool.h>

void fnCommStrip(char *pBuildDirectory, char *pReleasePath)
{
	FILE *ConfigFile;
    char Command[255], Line[2000];
    int index;

	/* Create Yacc config file */
    ConfigFile = fopen(YACC_CONFIG_FILE, "w+");
    if (ConfigFile == NULL) {
        printf("Error: create config file failure");
        return;
    }

    sprintf(Line, "SRC_ROOT_DIR = %s/\n", pBuildDirectory);
    fwrite(Line, 1, strlen(Line), ConfigFile);

    sprintf(Line, "OUTPUT_DIR = %s\n", pReleasePath);
    fwrite(Line, 1, strlen(Line), ConfigFile);

	sprintf(Line, "DEFINE_FLAGS = ");
    for (index = 0; index < sizeof(DefineTagList)/sizeof(struct _RTDefineTag);index++)
    {
        if (DefineTagList[index].IsDEF == TRUE)
            sprintf(Line, "%s %s", Line, DefineTagList[index].DefineTag);
    }
    sprintf(Line, "%s\n", Line);
    fwrite(Line, 1, strlen(Line), ConfigFile);

    sprintf(Line, "UNDEF_FLAGS = ");
    for (index = 0; index < sizeof(DefineTagList)/sizeof(struct _RTDefineTag);index++)
    {
        if (DefineTagList[index].IsDEF == FALSE)
            sprintf(Line, "%s %s", Line, DefineTagList[index].DefineTag);
    }
    sprintf(Line, "%s\n", Line);
    fwrite(Line, 1, strlen(Line), ConfigFile);

    sprintf(Line, "IFDEF_REMOVE = y\n");
    fwrite(Line, 1, strlen(Line), ConfigFile);

    sprintf(Line, "EVALUATE_NUM = y\n");
    fwrite(Line, 1, strlen(Line), ConfigFile);

    sprintf(Line, "FILTER_SUFFIX = .c .h .mk .4 .6 .util .netif\n");
    fwrite(Line, 1, strlen(Line), ConfigFile);
    
    fclose(ConfigFile);

#ifdef ECOS
    sprintf(Command, "mv %s/os/ecos/Makefile %s/os/ecos/Makefile.6", pBuildDirectory, pBuildDirectory);
    system(Command);
#endif /* ECOS */

    /* Execute Yacc */
    sprintf(Command, "mkdir -p %s", pReleasePath);
    system(Command);
    sprintf(Command, "striptool/parser < %s", YACC_CONFIG_FILE);
    system(Command);

#ifdef ECOS
    sprintf(Command, "mv %s/os/ecos/Makefile.6 %s/os/ecos/Makefile", pReleasePath, pReleasePath);
    system(Command);
#endif /* ECOS */

#if 1
    FILE *pReadFile, *pWriteFile;
    char ReadFileName[255], WriteFileName[255];
    char *ReadLine = NULL;
    size_t result, len;

    /* Handle Special Tags - Makefile */
    sprintf(Command, "mv %sMakefile %sMakefile.old", pReleasePath, pReleasePath);
    system(Command);
    sprintf(ReadFileName, "%sMakefile.old", pReleasePath);
    sprintf(WriteFileName, "%sMakefile", pReleasePath);
    
    pReadFile = fopen(ReadFileName, "r");
    pWriteFile = fopen(WriteFileName, "w+");
    if ((pReadFile != NULL) && (pWriteFile != NULL)) {
        fseek (pReadFile , 0 , SEEK_END);
        rewind (pReadFile);

        while ((result = getline(&ReadLine, &len, pReadFile)) != -1) {
            if (!(((strlen(ReadLine) >= 5) && (memcmp(ReadLine, "#MODE", 5) == 0))
               || ((strlen(ReadLine) >= 7) && (memcmp(ReadLine, "#TARTET", 7) == 0))
               || ((strlen(ReadLine) >= 8) && (memcmp(ReadLine, "#CHIPSET", 8) == 0)))) {
                fwrite(ReadLine, 1, strlen(ReadLine), pWriteFile);
            }
        }
        fclose(pReadFile);
        fclose(pWriteFile);
    }
    sprintf(Command, "rm -Rf %sMakefile.old", pReleasePath);
    system(Command);    

    /* Handle Special Tags - os/linuc/config.mk */
    sprintf(Command, "mv %s%s %s%s.old", pReleasePath, CONFIG_MK, pReleasePath, CONFIG_MK);
    system(Command);
    sprintf(ReadFileName, "%s%s.old", pReleasePath, CONFIG_MK);
    sprintf(WriteFileName, "%s%s", pReleasePath, CONFIG_MK);

    pReadFile = fopen(ReadFileName, "r");
    pWriteFile = fopen(WriteFileName, "w+");
    if ((pReadFile != NULL) && (pWriteFile != NULL)) {
        fseek (pReadFile , 0 , SEEK_END);
        rewind (pReadFile);

        while ((result = getline(&ReadLine, &len, pReadFile)) != -1) {
            if (!(((strlen(ReadLine) >= 5) && (memcmp(ReadLine, "#ifdef", 5) == 0))
               || ((strlen(ReadLine) >= 5) && (memcmp(ReadLine, "#endif", 5) == 0)))) {
                fwrite(ReadLine, 1, strlen(ReadLine), pWriteFile);
            }
        }
        fclose(pReadFile);
        fclose(pWriteFile);
    }

	sprintf(Command, "rm -Rf %s%s.old", pReleasePath, CONFIG_MK);
    system(Command);
#endif
}



