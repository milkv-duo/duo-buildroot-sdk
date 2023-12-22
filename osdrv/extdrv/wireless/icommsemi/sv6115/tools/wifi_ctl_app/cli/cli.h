/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _CLI_H_
#define _CLI_H_
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CLI_ENABLE 1
#define CLI_HISTORY_ENABLE 1

#define CLI_HISTORY_NUM 100
#define CLI_BUFFER_SIZE 1000
#define CLI_ARG_SIZE 22

#define CLI_PROMPT "wifi>"

#define hal_getchar     getch
#define hal_print       printf
#define hal_putchar     putchar
#define __SSV_UNIX_SIM__
#define FFLUSH(x) fflush(x)

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef void (*CliCmdFunc) (int, char ** );


typedef struct CLICmds_st
{
        const char           *Cmd;
        CliCmdFunc          CmdHandler;
        const char           *CmdUsage;
        
} CLICmds, *PCLICmds;


int Cli_RunCmd(char *CmdBuffer);
void *Cli_Init(void *args);
void Cli_Start(void);
void Cli_Task( void *args );


int getch( void );
int get_term(void);
int reset_term(void);
int kbhit ( void );

#endif /* _CLI_H_ */

