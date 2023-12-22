/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#include <unistd.h>
#include <sys/select.h>

#include "cli.h"


#if (CLI_ENABLE==1)

/* Command Line: */
static char    *sgArgV[CLI_ARG_SIZE];
static unsigned int     sgArgC;
static unsigned int     sgCurPos = 0;
static unsigned char    s_interactive = true;

/* cli: */
char gCmdBuffer[CLI_BUFFER_SIZE+1];



#if (CLI_HISTORY_ENABLE==1)

char gCmdHistoryBuffer[CLI_HISTORY_NUM][CLI_BUFFER_SIZE+1];
signed char gCmdHistoryIdx;
signed char gCmdHistoryCnt;

#endif//CLI_HISTORY_ENABLE


/* Command Table: */
extern CLICmds gCliCmdTable[];
extern unsigned char batch_mode;
extern int cli_isstop;

//----------------------------------------

#if (CLI_HISTORY_ENABLE==1)

static void Cli_EraseCmdInScreen(void)
{
    unsigned int i;
    for(i= 0;i < (strlen(gCmdBuffer)+strlen(CLI_PROMPT));i++)
    {
        hal_putchar(0x08);
        hal_putchar(0x20);
        hal_putchar(0x08);
    }
    FFLUSH(stdout);
}

static void Cli_PrintCmdInScreen(void)
{
    unsigned int i;
    printf("%s", CLI_PROMPT);
    FFLUSH(stdout);
    for(i= 0;i<strlen(gCmdBuffer);i++)
    {
         hal_putchar(gCmdBuffer[i]);
    }
    FFLUSH(stdout);
}



static unsigned char Cli_GetPrevHistoryIdx(void)
{
    return (gCmdHistoryIdx<=0) ? 0 : (gCmdHistoryIdx-1);
}


static unsigned char Cli_GetNextHistoryIdx(void)
{
    signed char CmdIdx = gCmdHistoryIdx;

    CmdIdx++;

    if(CmdIdx >= CLI_HISTORY_NUM || CmdIdx > gCmdHistoryCnt)
        CmdIdx--;


    return CmdIdx;
}



static inline void Cli_StoreCmdBufToHistory(unsigned char history)
{
    unsigned int len = strlen((const char*)gCmdBuffer);
    memcpy((unsigned char*)&gCmdHistoryBuffer[history][0], (unsigned char*)gCmdBuffer, len);
    gCmdHistoryBuffer[history][len]=0x00;
}


static inline void Cli_RestoreHistoryToCmdBuf(unsigned char history)
{
    unsigned int len = strlen((const char*)&gCmdHistoryBuffer[history]);
    memcpy((unsigned char*)gCmdBuffer, (unsigned char*)&gCmdHistoryBuffer[history][0], len);
    gCmdBuffer[len]= 0x00;
    sgCurPos = len;
}




static void Cli_MovetoPrevHistoryCmdBuf(void)
{
    unsigned char CmdIdx = gCmdHistoryIdx;
    unsigned char NewCmdIdx = Cli_GetPrevHistoryIdx();

    if(CmdIdx == NewCmdIdx)
        return;

    Cli_EraseCmdInScreen();
    Cli_StoreCmdBufToHistory(CmdIdx);
    Cli_RestoreHistoryToCmdBuf(NewCmdIdx);
    Cli_PrintCmdInScreen();
    gCmdHistoryIdx = NewCmdIdx;
}






static void Cli_MovetoNextHistoryCmdBuf(void)
{
    unsigned char CmdIdx = gCmdHistoryIdx;
        unsigned char NewCmdIdx = Cli_GetNextHistoryIdx();

        if(CmdIdx == NewCmdIdx)
            return;

        Cli_EraseCmdInScreen();
        Cli_StoreCmdBufToHistory(CmdIdx);
        Cli_RestoreHistoryToCmdBuf(NewCmdIdx);
        Cli_PrintCmdInScreen();
        gCmdHistoryIdx = NewCmdIdx;
}



static void Cli_RecordInHistoryCmdBuf(void)
{
    unsigned int i = CLI_HISTORY_NUM-2;
    unsigned int len;

    if(sgCurPos)
    {
        //shift history log
        for(i =  CLI_HISTORY_NUM-2; i>0 ; i--)
        {
            len = strlen((const char*)&gCmdHistoryBuffer[i]);
            memcpy((unsigned char*)&gCmdHistoryBuffer[i+1][0], (unsigned char*)&gCmdHistoryBuffer[i][0], len);
            gCmdHistoryBuffer[i+1][len] = 0x00;
        }


        //copy bud to index 1
        len = strlen((const char*)&gCmdBuffer);
        memcpy((unsigned char*)&gCmdHistoryBuffer[1][0], (unsigned char*)&gCmdBuffer, len);
        gCmdHistoryBuffer[1][len] = 0x00;


        //Record how much history we record
        gCmdHistoryCnt++;
        if(gCmdHistoryCnt>=CLI_HISTORY_NUM)
            gCmdHistoryCnt = CLI_HISTORY_NUM-1;
    }

    //Reset buf
    gCmdHistoryBuffer[0][0]=0x00;
    gCmdHistoryIdx = 0;


}
#endif

//----------------------------------------

static void _CliCmdUsage( void )
{
    CLICmds *CmdPtr;

    printf("\r\nUsage:\r\n");
    for( CmdPtr=gCliCmdTable; CmdPtr->Cmd; CmdPtr ++ )
    {
        printf("%-20s\t\t%s\r\n", CmdPtr->Cmd, CmdPtr->CmdUsage);
    }
    printf("\r\n%s", CLI_PROMPT);
    FFLUSH(stdout);
}



int Cli_RunCmd(char *CmdBuffer)
{
    CLICmds *CmdPtr;
    unsigned char ch;
    char *pch;
    
    for( sgArgC=0,ch=0, pch=CmdBuffer; (*pch!=0x00)&&(sgArgC<CLI_ARG_SIZE); pch++ )
    {
        if ( (ch==0) && ((*pch!=' ')||(*pch!=',')) && (*pch!=0x0a) && (*pch!=0x0d) )
        {
            ch = 1;
            sgArgV[sgArgC] = pch;
        }

        if ( (ch==1) && (((*pch==' ')||(*pch==','))||(*pch=='\t')||(*pch==0x0a)||(*pch==0x0d)) )
        {
            *pch = 0x00;
            ch = 0;
            sgArgC ++;
        }
    }

    if(sgArgC==CLI_ARG_SIZE)
    {
        printf("Total nummber of arg are over %d \r\n",CLI_ARG_SIZE);
        return 0;
    }
    
    if ( ch == 1)
    {
        sgArgC ++;
    }
    else if ( sgArgC > 0 )
    {
//        *(pch-1) = ' ';
    }

    if (sgArgC == 0)
    {
        return 0;
    }
    
    /* Dispatch command */
    for( CmdPtr=gCliCmdTable; CmdPtr->Cmd; CmdPtr ++ )
    {   
        if ( !strcmp(sgArgV[0], CmdPtr->Cmd) )
        { 
            CmdPtr->CmdHandler( sgArgC, sgArgV );
            break;
        }
    }
    if (NULL == CmdPtr->Cmd){
        return -1;
    }
    return 0;
}

static int _g_cli_sig_pipefd = -1;
void cli_set_sig_pipefd(int fd)
{
    _g_cli_sig_pipefd = fd;
}

void *Cli_Init(void *args)
{
    memset((void *)sgArgV, 0, sizeof(unsigned char *)*CLI_ARG_SIZE);
    gCmdBuffer[0]     = 0x00;
    sgCurPos         = 0;
    sgArgC             = 0;
    s_interactive     = true;

#if (CLI_HISTORY_ENABLE==1)
    {
        int i;
        gCmdHistoryIdx = 0;
        gCmdHistoryCnt = 0;

        for(i=0;i<CLI_HISTORY_NUM; i++)
        gCmdHistoryBuffer[i][0]=0x00;       
    }
        
#endif
    
    memset((void *)gCmdBuffer, 0, CLI_BUFFER_SIZE+1);

    Cli_Task(NULL);

    return NULL;
}

void Cli_Start(void)
{
    fd_set rfds;
//    CLICmds *CmdPtr;
    unsigned char ch;
//    char *pch;

    if (s_interactive == false)
    {
       if (Cli_RunCmd(gCmdBuffer) < 0)
       {
            printf("\nCommand not found!!\r\n");
       }
       else
       {
            printf("\r\n");
       }
       FFLUSH(stdout);
       gCmdBuffer[0] = 0x00;
    }

#ifdef __SSV_UNIX_SIM__
    //if ( !kbhit() )
    //{
    //    return;
    //}
#endif

    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    FD_SET(_g_cli_sig_pipefd, &rfds);
    while(0 == select(_g_cli_sig_pipefd+1, &rfds, NULL, NULL, NULL))

    if(FD_ISSET(_g_cli_sig_pipefd, &rfds))
    {
        close(_g_cli_sig_pipefd);
        return;
    }

    if(FD_ISSET(STDIN_FILENO, &rfds))
    {
    switch ( (ch=hal_getchar()) )
    {
        case 0x00: /* Special Key, read again for real data */
            ch = hal_getchar();
            break;

    #ifndef __SSV_UNIX_SIM__
        //Windows mode
        case 0x08: /* Backspace */
    #else
        //Linux mode
        case 0x7f:
        case 0x08:
        //ch = hal_getchar();//Get extra data
    #endif
            if ( 0 < sgCurPos )
            {
                hal_putchar(0x08);
                hal_putchar(0x20);
                hal_putchar(0x08);
                FFLUSH(stdout);
                sgCurPos --;
                gCmdBuffer[sgCurPos] = 0x00;
            }
            break;

    // Changed __linux__ to __SSV_UNIX_SIM__ for CYGWIN compatibility
    #ifdef __SSV_UNIX_SIM__
        case 0x0a: /* Enter */
    #else
        case 0x0d: /* Enter */
    #endif
        //invoke_command:

    #if (CLI_HISTORY_ENABLE==1)

            Cli_RecordInHistoryCmdBuf();

    #endif

            if (sgCurPos > 0) {
                printf("\r\n");
                if (Cli_RunCmd(gCmdBuffer) < 0)
                {
                    printf("\nCommand not found!!\r\n");
                }
                else
                {
                    printf("\r\n");
                }
                FFLUSH(stdout);
                memset((void *)sgArgV, 0, sizeof(unsigned char *)*CLI_ARG_SIZE);
                memset((void *)gCmdBuffer, 0, CLI_BUFFER_SIZE+1);
            }
    #if 0

            for( sgArgC=0,ch=0, pch=gCmdBuffer; (*pch!=0x00)&&(sgArgC<CLI_ARG_SIZE); pch++ )
            {
                if ( (ch==0) && (*pch!=' ') )
                {
                    ch = 1;
                    sgArgV[sgArgC] = pch;
                }

                if ( (ch==1) && (*pch==' ') )
                {
                    *pch = 0x00;
                    ch = 0;
                    sgArgC ++;
                }
            }
            if ( ch == 1)
            {
                sgArgC ++;
            }
            else if ( sgArgC > 0 )
            {
                *(pch-1) = ' ';
            }

            if ( sgArgC > 0 )
            {
                /* Dispatch command */
                for( CmdPtr=gCliCmdTable; CmdPtr->Cmd; CmdPtr ++ )
                {
                    if ( !ssv6xxx_strcmp(sgArgV[0], CmdPtr->Cmd) )
                    {
                        printf("\n");
                        FFLUSH(stdout);
                        CmdPtr->CmdHandler( sgArgC, sgArgV );
                        break;
                    }
                }
                if ( (NULL==CmdPtr->Cmd) && (0!=sgCurPos) )
                {
                    printf("\nCommand not found!!\n");
                    FFLUSH(stdout);
                }
            }
    #endif
            if(!cli_isstop){
            printf("\r\n%s", CLI_PROMPT);


            FFLUSH(stdout);

            gCmdBuffer[0] = 0x00;
            sgCurPos = 0;
            }
            break;
    #ifdef _WIN32_
    //Windows platform
        case 0xe0://special key
            {
                ch=hal_getchar();

    #if (CLI_HISTORY_ENABLE==1)
                if(0x48 == ch)//up arrow key
                {
                    Cli_MovetoNextHistoryCmdBuf();
                }
                else if(0x50 == ch)//down arrow key
                {

                    Cli_MovetoPrevHistoryCmdBuf();
                }
                else
                {

                }

    #endif//#if (CLI_HISTORY_ENABLE==1)

            }
            break;
    #else//__SSV_UNIX_SIM__
        //Linux platform

        case 0x1b:
        {

    #if (CLI_HISTORY_ENABLE==1)
            ch=hal_getchar();
            if(0x5b==ch)
            {
                ch=hal_getchar();

                if(0x41==ch){//Up arrow key

                    Cli_MovetoNextHistoryCmdBuf();
                }
                else if(0x42==ch){//down arrow key

                    Cli_MovetoPrevHistoryCmdBuf();
                }
                else if(0x43==ch){//right arrow key
                }
                else if(0x44==ch){//left arrow key



                }
                else
                {;}


            }
    #endif//#if (CLI_HISTORY_ENABLE==1)


        }
            break;

    #endif//__SSV_UNIX_SIM__

        case '?': /* for help */
            hal_putchar(ch);
            FFLUSH(stdout);
            _CliCmdUsage();
            break;

        default: /* other characters */


            if(ch>=0x20&&ch<=0x7e)
            {
                if ( (CLI_BUFFER_SIZE-1) > sgCurPos )
                {  
                    gCmdBuffer[sgCurPos++] = ch;
                    gCmdBuffer[sgCurPos] = 0x00;
                    hal_putchar(ch);
                    FFLUSH(stdout);

                }
            }
            break;
        } //end of switch ( (ch=hal_getchar()) )
    } //end of if (FD_ISSET(STDIN_FILENO, &rfds))
}



extern unsigned char g_cli_joining;

/**
 *  CLI (Command Line Interface) Task:
 *
 *  Create CLI Task for console mode debugging.
 */
void Cli_Task( void *args )
{
    printf("\r\n%s",CLI_PROMPT);
    FFLUSH(stdout);
    get_term();
    kbhit();

    while(!cli_isstop)
    {
        Cli_Start();
    }
    reset_term();
}
#endif