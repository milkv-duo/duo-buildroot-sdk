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

/**
 * @file app_ctl.c
 * @brief application control functions.
 */


/*******************************************************************************
 *         Include Files
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>

#include <ssv_ble_services.h>
#include <ssv_netlink.h>
#include "ssv_nimble.h"


/*******************************************************************************
 *         Local Defines
 ******************************************************************************/


/*******************************************************************************
 *         Local Enumerations
 ******************************************************************************/


/*******************************************************************************
 *         Local Structures
 ******************************************************************************/


/*******************************************************************************
 *         Global Variables
 ******************************************************************************/


/*******************************************************************************
 *         Local Variables
 ******************************************************************************/


/*******************************************************************************
 *         Local Funcations
 ******************************************************************************/


/*******************************************************************************
 *         Global Funcations
 ******************************************************************************/
int ssv_hal_ble_common_init(void)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_COMMON_INIT;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD)); 
    return 0;
}

int ssv_hal_ble_common_deinit(void)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_COMMON_DEINIT;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD)); 
    return 0;
}

