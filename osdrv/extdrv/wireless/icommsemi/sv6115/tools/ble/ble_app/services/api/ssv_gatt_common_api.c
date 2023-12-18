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
#include "ssv_hal_ble_gap.h"
#include <ssv_gatt_common_api.h>


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
ssv_err_t ssv_ble_gatt_set_local_mtu(uint16_t mtu)
{
#if 1
    ST_NIMBLE_CMD  *pbleApi=NULL;
    ST_SSV_NIMBLE_SET_LOCAL_MTU *pmtu=NULL;
    unsigned int size = sizeof(ST_NIMBLE_CMD)+sizeof(ST_SSV_NIMBLE_SET_LOCAL_MTU);
    pbleApi=malloc(size);
    if(NULL!=pbleApi)
    {
        memset(pbleApi,0,sizeof(size));
        pbleApi->cmdid=E_SSV_BLE_GATT_SET_LOCAL_MTU;
        pbleApi->datalen=sizeof(ST_SSV_NIMBLE_SET_LOCAL_MTU);
        pmtu=(ST_SSV_NIMBLE_SET_LOCAL_MTU *)pbleApi->data;
        if(mtu > SSV_GATT_MAX_MTU_SIZE) mtu = SSV_GATT_MAX_MTU_SIZE;
        pmtu->mtu=mtu;
        ssv_netlink_send_cmd((char *)pbleApi,size); 
        free(pbleApi);
    }
    return 0;
#else
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=SSV_BLE_GATT_SET_LOCAL_MTU;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD)); 
    return 0;
#endif
}


