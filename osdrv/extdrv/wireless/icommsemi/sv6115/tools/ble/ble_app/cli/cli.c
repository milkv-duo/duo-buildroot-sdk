/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>

#include <ssv_ble_services.h>
#include <ssv_nimble.h>
#include <ssv_hal_ble_gap.h>
#include <ssv_hal_ble_gatts.h>
#include <ssv_hal_ble_gattc.h>
#include <ssv_hal_ble_common.h>
#include <ssv_gatt_common_api.h>

#include "cli.h"


#if (CLI_ENABLE==1)

/* Command Line: */
static char    *sgArgV[CLI_ARG_SIZE];
static unsigned int     sgArgC;
static unsigned int     sgCurPos = 0;
static unsigned char    s_interactive = true;

/* cli: */
bool cli_isstop=false;
char gCmdBuffer[CLI_BUFFER_SIZE+1];

#if (CLI_HISTORY_ENABLE==1)

char gCmdHistoryBuffer[CLI_HISTORY_NUM][CLI_BUFFER_SIZE+1];
signed char gCmdHistoryIdx;
signed char gCmdHistoryCnt;

#endif//CLI_HISTORY_ENABLE


/* Command Table: */
//extern CLICmds gCliCmdTable[];
extern unsigned char batch_mode;

extern void ssv_gatts_profile_cb(ssv_gatts_cb_event_t event,ssv_gatt_if_t gatts_if,ssv_ble_gatts_cb_param_t *param);
extern void ssv_gap_event_handler(ssv_gap_ble_cb_event_t event, ssv_ble_gap_cb_param_t *param);
//----------------------------------------

static uint8_t raw_scan_rsp_data[16+1] = { //'SSV_GATTS_DEMO'
    0x0F,0x09,0x53,0x53,0x56,0x5F,0x47,0x41,0x54,0x54,0x53,0x5F,0x44,0x45,0x4D,0x4F,0x00
};

uint8_t sg_ble_start = 0;
ssv_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x30,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

enum
{
    AT_IDX_SVC,
    AT_IDX_CHAR_A,
    AT_IDX_CHAR_VAL_A,
    AT_IDX_CHAR_B,
    AT_IDX_CHAR_VAL_B,
    AT_HRS_IDX_NB,
};

static const uint8_t AT_GATTS_CHAR_UUID_TEST_A[] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00
};

static const uint8_t AT_GATTS_CHAR_UUID_TEST_B[] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x02, 0xFF, 0x00, 0x00
};

#define OLD_PRIMARY_SERVICE
#ifdef OLD_PRIMARY_SERVICE
static const uint16_t AT_GATTS_SERVICE_UUID      = 0x00FF;

static uint8_t raw_adv_data[] = {
    0x02, 0x01, 0x06,
    0x02, 0x0a, 0xec,
    0x03, 0x03, 0xd0, 0xfd,
    0x12, 0xFF, 0x01, 0x66, 0x48, 0x55, 0x4e, 0x74, 0x70, 0x43, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//0xFF:manufacturer_data
};
#else
static uint8_t AT_GATTS_SERVICE_UUID[] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00
};

static uint8_t manufacturerID[] = {
    0x01, 0x66, 0x48, 0x55, 0x4e, 0x74, 0x70, 0x43 //max 8 bytes(ble advert data max is 31)
};

static ssv_ble_adv_data_t adv_data128 = {
	.set_scan_rsp        = 0,
	.include_name        = 0,
	.include_txpower     = 1,
	.min_interval        = 0x20,
	.max_interval        = 0x40,
	.appearance          = 0x00,
	.manufacturer_len    = sizeof(manufacturerID),
	.p_manufacturer_data = manufacturerID,
	.service_data_len    = 0,
	.p_service_data      = NULL,
	.service_uuid_len    = sizeof(AT_GATTS_SERVICE_UUID),
	.p_service_uuid      = AT_GATTS_SERVICE_UUID,
	.flag                = (SSV_BLE_ADV_FLAG_GEN_DISC | SSV_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
#endif

static const uint16_t at_primary_service_uuid         = SSV_GATT_UUID_PRI_SERVICE;
static const uint16_t at_character_declaration_uuid   = SSV_GATT_UUID_CHAR_DECLARE;
static const uint16_t at_character_client_config_uuid = SSV_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t at_char_prop_read                =  SSV_GATT_CHAR_PROP_BIT_READ;
static const uint8_t at_char_prop_write               = SSV_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t at_char_prop_read_write_notify   = SSV_GATT_CHAR_PROP_BIT_WRITE | SSV_GATT_CHAR_PROP_BIT_READ;

static const uint8_t at_char_value[4]                 = {0x11, 0x22, 0x33, 0x44};
#define AT_CHAR_DECLARATION_SIZE    (sizeof(uint8_t))
#define AT_GATTS_CHAR_VAL_LEN_MAX   (128)

static ssv_gatts_attr_db_t gatt_db[AT_HRS_IDX_NB] =
{
    // Service Declaration
    [AT_IDX_SVC]        =
    {{SSV_GATT_AUTO_RSP}, {SSV_UUID_LEN_16, (uint8_t *)&at_primary_service_uuid, SSV_GATT_PERM_READ,
#ifdef OLD_PRIMARY_SERVICE
      sizeof(uint16_t), sizeof(AT_GATTS_SERVICE_UUID), (uint8_t *)&AT_GATTS_SERVICE_UUID}},
#else
      sizeof(AT_GATTS_SERVICE_UUID), sizeof(AT_GATTS_SERVICE_UUID), (uint8_t *)AT_GATTS_SERVICE_UUID}},
#endif
    /* Characteristic Declaration */
    [AT_IDX_CHAR_A]     =
    {{SSV_GATT_AUTO_RSP}, {SSV_UUID_LEN_16, (uint8_t *)&at_character_declaration_uuid, SSV_GATT_PERM_READ,
      AT_CHAR_DECLARATION_SIZE, AT_CHAR_DECLARATION_SIZE, (uint8_t *)&at_char_prop_read_write_notify}},

    /* Characteristic Value */
    [AT_IDX_CHAR_VAL_A] =
    {{SSV_GATT_AUTO_RSP}, {SSV_UUID_LEN_128, (uint8_t *)&AT_GATTS_CHAR_UUID_TEST_A, SSV_GATT_PERM_READ | SSV_GATT_PERM_WRITE,
      AT_GATTS_CHAR_VAL_LEN_MAX, sizeof(at_char_value), (uint8_t *)at_char_value}},

    /* Characteristic Declaration */
    [AT_IDX_CHAR_B]      =
    {{SSV_GATT_AUTO_RSP}, {SSV_UUID_LEN_16, (uint8_t *)&at_character_declaration_uuid, SSV_GATT_PERM_READ,
      AT_CHAR_DECLARATION_SIZE, AT_CHAR_DECLARATION_SIZE, (uint8_t *)&at_char_prop_read}},

    /* Characteristic Value */
    [AT_IDX_CHAR_VAL_B]  =
    {{SSV_GATT_RSP_BY_APP}, {SSV_UUID_LEN_128, (uint8_t *)&AT_GATTS_CHAR_UUID_TEST_B, SSV_GATT_PERM_READ | SSV_GATT_PERM_WRITE,
      AT_GATTS_CHAR_VAL_LEN_MAX, sizeof(at_char_value), (uint8_t *)at_char_value}},
};

static bool ble_isinit=false;
//////////////////////////////
static void cmd_init(int argc, char *argv[])
{
    bool bInit=true;

    if(argc > 1) bInit = atoi(argv[1]);
    printf("bleinit=%d\n", bInit?1:0);

    ble_isinit = bInit;
    if(bInit) {
        ssv_hal_ble_gatts_set_profile_cb(ssv_gatts_profile_cb);
        ssv_hal_ble_gap_set_event_cb(ssv_gap_event_handler);    
        ssv_hal_ble_common_init();
        ssv_hal_ble_gap_init();
        ssv_hal_ble_gatts_init();
        //ssv_hal_ble_gattc_init();
        ssv_ble_gatt_set_local_mtu(SSV_GATT_MAX_MTU_SIZE); // 128

        sg_ble_start = 1;
    }
    else {
        ssv_hal_ble_common_deinit();
        ssv_hal_ble_gap_deinit();
        ssv_hal_ble_gatts_deinit();
        //ssv_hal_ble_gattc_deinit();

        sg_ble_start = 0;
    }
}

static void cmd_advert(int argc, char *argv[])
{
    bool bStart=true;
    uint8_t *p_rsp=NULL;
    unsigned char rsp_len=0;

    if(argc > 1) bStart = atoi(argv[1]);
    if(argc > 2) {
        rsp_len = strlen(argv[2])+3;
        p_rsp = malloc(rsp_len);
        if(p_rsp) {
            p_rsp[0] = rsp_len-2;
            p_rsp[1] = 0x09;
            memcpy(&p_rsp[2], argv[2], rsp_len-2);
        }
    }
    else {
        rsp_len = sizeof(raw_scan_rsp_data);
        p_rsp = malloc(rsp_len);
        if(p_rsp) memcpy(p_rsp, raw_scan_rsp_data, rsp_len);
    }
    if(p_rsp == NULL) {
        printf("malloc fail\n");
        return;
    }
    printf("bleADV=%d,%s\n", bStart?1:0, p_rsp+2);

    if(true==bStart)
    {
        //config adv data
    #ifdef OLD_PRIMARY_SERVICE
        ssv_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
    #else
        ssv_ble_gap_config_adv_data(&adv_data128);
    #endif
        //config scan data
        ssv_ble_gap_config_scan_rsp_data_raw(p_rsp, rsp_len-1);
        //start adv
        ssv_ble_gap_start_advertising(&adv_params);
    }
    else ssv_ble_gap_stop_advertising();

    free(p_rsp);
}

static void cmd_addservice(int argc, char *argv[])
{
    ssv_err_t api_ret=0;
    uint16_t handle;

    api_ret = ssv_hal_ble_gatts_create_service_by_db(gatt_db, AT_HRS_IDX_NB, &handle);

    if (api_ret)
        printf("ssv_hal_ble_gatts_create_service_by_db api_ret = %d", api_ret);
}

static void cmd_send(int argc, char *argv[])
{
    int len, attrib_offset;

    if (argc <= 2) {
        printf("send=1,sendtest\n");
        return;
    }

    attrib_offset = strtoul(argv[1], NULL, 10);
    len = strlen(argv[2]);

    ssv_hal_ble_gatts_send_indication(attrib_offset, (uint8_t*)argv[2], len, false);
}

static void cmd_disconnect(int argc, char *argv[])
{
    char *remote_bda = NULL;
    int i;
    ssv_bd_addr_t st_remote_bda;

    if (argc <= 1) {
        printf("disconnect=remote_bda\n");
        return;
    }

    remote_bda = argv[1];
    for (i=0; i<SSV_BD_ADDR_LEN; i++)
        st_remote_bda[SSV_BD_ADDR_LEN-1-i] = strtoul(remote_bda + 3*i, NULL, 16);

    ssv_ble_gap_disconnect(st_remote_bda);
}

static void cmd_scanparams(int argc, char *argv[])
{
    ssv_ble_scan_params_t ble_scan_params;

    if (argc <= 6) {
        printf("blesetscanparams=scan_type,own_addr_type,scan_filter_policy,"
                "scan_interval,scan_window,scan_duplicate\n");
        return;
    }

    ble_scan_params.scan_type          = strtoul(argv[1], NULL, 10);
    ble_scan_params.own_addr_type      = strtoul(argv[2], NULL, 10);
    ble_scan_params.scan_filter_policy = strtoul(argv[3], NULL, 10);
    ble_scan_params.scan_interval      = strtoul(argv[4], NULL, 10);
    ble_scan_params.scan_window        = strtoul(argv[5], NULL, 10);
    ble_scan_params.scan_duplicate     = strtoul(argv[6], NULL, 10);

    printf("blesetscanparams=%d,%d,%d,%d,%d,%d\n", 
            ble_scan_params.scan_type, ble_scan_params.own_addr_type, ble_scan_params.scan_filter_policy,
            ble_scan_params.scan_interval, ble_scan_params.scan_window, ble_scan_params.scan_duplicate);
    ssv_ble_gap_set_scan_params(&ble_scan_params);
}

static void cmd_startscan(int argc, char *argv[])
{
    uint32_t duration;

    if (argc <= 1) {
        printf("blestartscan=duration\n");
        return;
    }
    duration = strtoul(argv[1], NULL, 10);

    printf("blestartscan=%d\n", duration);
    if(duration) ssv_ble_gap_start_scanning(duration);
    else ssv_ble_gap_stop_scanning();
}

#ifdef BLE_GATTC_EN
static void cmd_gattcopen(int argc, char *argv[])
{
    ssv_bd_addr_t st_remote_bda;
    uint8_t gattc_if, addr_type, is_direct;
    int i;

    if (argc <= 4) {
        printf("blegattcopen=gattc_if,remote_bda,remote_addr_type,is_direct\n");
        return;
    }

    gattc_if    = strtoul(argv[1], NULL, 10);
    addr_type   = strtoul(argv[3], NULL, 10);
    is_direct   = strtoul(argv[4], NULL, 10);

    for (i = 0; i < SSV_BD_ADDR_LEN; i++)
        st_remote_bda[SSV_BD_ADDR_LEN-1-i] = strtoul(argv[2]+3*i, NULL, 16);

    printf("blegattcopen=%d,%02x:%02x:%02x:%02x:%02x:%02x,%d,%d\n", 
            gattc_if,
            st_remote_bda[5], st_remote_bda[4], st_remote_bda[3], st_remote_bda[2], st_remote_bda[1], st_remote_bda[0],
            addr_type, is_direct);

    ssv_hal_ble_gattc_open(gattc_if, st_remote_bda, addr_type, is_direct);
}
#endif

static void cmd_help(int argc, char *argv[])
{
    printf("\r=================ble command usage=================\n");
#ifdef BLE_UART_DUMMY
#endif
    printf("init - ble init/deinit\n");
    printf("\033[1;31meg. init=1\033[0m\n");
    printf("\033[1;36m    1: init 0: deinit\033[0m\n\n");

    printf("advert - ble advert start/stop\n");
    printf("\033[1;31meg. advert=1,icommtest\033[0m\n");
    printf("\033[1;36m    1: start 0: stop, icommtest: device name\033[0m\n\n");

    printf("addservice - ble add service\n\n");

    printf("send - ble send data\n");
    printf("\033[1;31meg. send=1,sendtest\033[0m\n");
    printf("\033[1;36m    1: attrib_offset, sendtest: send' string\033[0m\n\n");

    printf("disconnect - ble disconnect\n");
    printf("\033[1;31meg. disconnect=44:57:18:3F:49:01\033[0m\n");
    printf("\033[1;36m    44:57:18:3F:49:01 ble device address\033[0m\n\n");

    printf("scanparams - ble set scan params\n");
    printf("\033[1;31meg. scanparams=1,0,0,320,32,16\033[0m\n");
    printf("\033[1;36m    1: scan_type, 0: own_addr_type, 0: scan_filter_policy, 320: scan_interval, 32: scan_window, 16: scan_duplicate\033[0m\n\n");

    printf("scan - ble start scan N second\n");
    printf("\033[1;31meg. scan=8\033[0m\n");
    printf("\033[1;36m    8: scan time 8 second, 0: stop scan\033[0m\n\n");

#ifdef BLE_GATTC_EN
    printf("gattcopen - ble connect MAC'addr device\n");
    printf("\033[1;31meg. gattcopen=0,44:57:18:3f:49:01,0,1\033[0m\n");
    printf("\033[1;36m    0: connect 44:57:18:3f:49:01: device mac-addr 0: 1: \033[0m\n\n");
#endif
    printf("help - ble command usage, params description\n");
    printf("exit - exit application(\033[1;31mis auto deinit\033[0m)\n");
    printf("======================================================");
}

void cmd_exit(int argc, char *argv[])
{
    cli_isstop = true;
    //ssv_ble_services_stop(SIGTERM);
}

CLICmds gCliCmdTable[] =
{
    { "init",           cmd_init,           "ble init"},
    { "advert",         cmd_advert,         "ble adverting"},
    { "addservice",     cmd_addservice,     "ble add service"},
    { "send",           cmd_send,           "ble send data"},
    { "disconnect",     cmd_disconnect,     "ble disconnect"},
    { "scanparams",     cmd_scanparams,     "ble set scan params"},
    { "scan",           cmd_startscan,      "ble start scan"},
#ifdef BLE_GATTC_EN
    { "gattcopen",      cmd_gattcopen,      "ble gattc connect"},
#endif
    { "help",           cmd_help,           "ble command help"},
    { "exit",           cmd_exit,           "exit"},
    { (const char *)NULL, (CliCmdFunc)NULL, (const char *)NULL},
};

/////////////////////////////////
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
        if ( (ch==0) && ((*pch!=' ')||(*pch!='=')||(*pch!=',')) && (*pch!=0x0a) && (*pch!=0x0d) )
        {
            ch = 1;
            sgArgV[sgArgC] = pch;
        }

        if ( (ch==1) && (((*pch==' ')||(*pch=='=')||(*pch==','))||(*pch=='\t')||(*pch==0x0a)||(*pch==0x0d)) )
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

static int __g_cli_sig_pipefd = -1;
void cli_set_sig_pipefd(int fd)
{
    __g_cli_sig_pipefd = fd;
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
    FD_SET(__g_cli_sig_pipefd, &rfds);
    while(0 == select(__g_cli_sig_pipefd+1, &rfds, NULL, NULL, NULL))

    if(FD_ISSET(__g_cli_sig_pipefd, &rfds))
    {
        close(__g_cli_sig_pipefd);
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
    if(ble_isinit)
    {
        ssv_hal_ble_common_deinit();
        ssv_hal_ble_gap_deinit();
        ssv_hal_ble_gatts_deinit();
    }
    ssv_ble_services_stop(SIGTERM);
    reset_term();
}
#endif
