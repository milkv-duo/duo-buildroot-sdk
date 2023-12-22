/*
 * Copyright (c) 2020 iComm-semi Ltd.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Common defines and declarations for host driver and firmware for all
 * platforms.
 */


#ifndef __WIFI_APP_COMMON_H__
#define __WIFI_APP_COMMON_H__


/*******************************************************************************
 *         Include Files
 ******************************************************************************/


/*******************************************************************************
 *         Defines
 ******************************************************************************/
#ifndef SSV_PACKED_STRUCT
//SSV PACK Definition
#define SSV_PACKED_STRUCT_BEGIN
#define SSV_PACKED_STRUCT               //__attribute__ ((packed))
#define SSV_PACKED_STRUCT_END           //__attribute__((packed))
#define SSV_PACKED_STRUCT_STRUCT        __attribute__ ((packed))
#define SSV_PACKED_STRUCT_FIELD(x)      x
#endif

#define WIFI_APP_CMD_HDR_LEN            ((size_t)(((ST_WIFI_APP_CMD *)0)->data))
#define WIFI_APP_EVT_HDR_LEN            ((size_t)(((ST_WIFI_APP_EVT *)0)->data))


/*******************************************************************************
 *         Enumerations
 ******************************************************************************/
typedef enum en_wifi_app_cmd_type
{
    E_WIFI_APP_CMD_TYPE_START = 0,
    E_WIFI_APP_CMD_TYPE_SCAN,
    E_WIFI_APP_CMD_TYPE_JOIN,
    E_WIFI_APP_CMD_TYPE_LEAVE,
    E_WIFI_APP_CMD_TYPE_ENTER_LOWPOWER,
    E_WIFI_APP_CMD_TYPE_EXIT_LOWPOWER,
    E_WIFI_APP_CMD_TYPE_TEST_REQ,
    E_WIFI_APP_CMD_TYPE_ATCMD,
    E_WIFI_APP_CMD_TYPE_TXTPUT,
    E_WIFI_APP_CMD_TYPE_RXTPUT,
    E_WIFI_APP_CMD_TYPE_GET_MAC_ADDR,
    E_WIFI_APP_CMD_TYPE_END = 0xFFFF
} EN_WIFI_APP_CMD_TYPE;

typedef enum en_wifi_app_evt_type
{
    E_WIFI_APP_EVT_TYPE_START = 0,
    E_WIFI_APP_EVT_TYPE_APLIST,
    E_WIFI_APP_EVT_TYPE_WIFI_CONNECT,
    E_WIFI_APP_EVT_TYPE_WIFI_DISCONNECT,
    E_WIFI_APP_EVT_TYPE_ENTER_LOWPOWER,
    E_WIFI_APP_EVT_TYPE_EXIT_LOWPOWER,
    E_WIFI_APP_EVT_TYPE_TEST_RSP,
    E_WIFI_APP_EVT_TYPE_ATCMD_RSP,
    E_WIFI_APP_EVT_TYPE_TXTPUT_RSP,
    E_WIFI_APP_EVT_TYPE_GET_MAC_ADDR_RSP,
    E_WIFI_APP_EVT_TYPE_END = 0xFFFF
} EN_WIFI_APP_RSP_TYPE;


/*******************************************************************************
 *         Structures
 ******************************************************************************/
SSV_PACKED_STRUCT_BEGIN
typedef struct st_wifi_app_cmd
{
    unsigned int cmdid;
    unsigned int datalen;
    unsigned char data[0];
}SSV_PACKED_STRUCT_STRUCT ST_WIFI_APP_CMD;
SSV_PACKED_STRUCT_END

SSV_PACKED_STRUCT_BEGIN
typedef struct st_wifi_app_evt
{
    unsigned int evtid;
    unsigned int datalen;
    unsigned char data[0];
}SSV_PACKED_STRUCT_STRUCT ST_WIFI_APP_EVT;
SSV_PACKED_STRUCT_END


/*******************************************************************************
 *         Variables
 ******************************************************************************/


/*******************************************************************************
 *         Functions
 ******************************************************************************/


#endif /* __WIFI_APP_COMMON_H__ */
