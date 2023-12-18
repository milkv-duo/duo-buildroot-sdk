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
static ssv_hal_gap_ble_event_cb_t gap_ble_event_cb=NULL;

/*******************************************************************************
 *         Local Funcations
 ******************************************************************************/


/*******************************************************************************
 *         Global Funcations
 ******************************************************************************/
int ssv_hal_ble_gap_init(void)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_GAP_INIT;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD)); 
    return 0;
}

int ssv_hal_ble_gap_deinit(void)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_GAP_DEINIT;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD)); 
    return 0;
}

void ssv_hal_ble_gap_set_event_cb(ssv_hal_gap_ble_event_cb_t cb)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_GAP_SET_EVENT_CB;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD)); 
    gap_ble_event_cb = cb;
    return;
}

#if __WORDSIZE == 32
void ssv_hal_gap_event_cb(ssv_gap_ble_cb_event_t evtId,char *data, unsigned int len)
{
    switch(evtId){
        case SSV_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT:
            {
                struct ble_get_bond_dev_cmpl_evt_param *bond_pram = (struct ble_get_bond_dev_cmpl_evt_param*)data;
                bond_pram->bond_dev = (ssv_ble_bond_dev_t*)(data + sizeof(struct ble_get_bond_dev_cmpl_evt_param));
                gap_ble_event_cb(evtId, (ssv_ble_gap_cb_param_t *)data);
            }
            break;
        default:            
            gap_ble_event_cb(evtId, (ssv_ble_gap_cb_param_t *)data);
            break;
    }
    return;
}

#else

struct soc_ble_get_bond_dev_cmpl_evt_param {
    ssv_bt_status_t status;                     /*!< Indicate the get bond device operation success status */
    uint8_t dev_num;                            /*!< Indicate the get number device in the bond list */
    ptr_32bits bond_dev;                        /*!< the pointer to the bond device Structure */
};                                              /*!< Event parameter of SSV_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT */
////////////////////////////
void ssv_hal_gap_event_cb(ssv_gap_ble_cb_event_t evtId, char *data, unsigned int len)
{
    switch(evtId)
    {
        case SSV_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT:
            {
                int buflen;
                struct ble_get_bond_dev_cmpl_evt_param *pdest;

                buflen = sizeof(struct ble_get_bond_dev_cmpl_evt_param) +
                            ((struct soc_ble_get_bond_dev_cmpl_evt_param*)data)->dev_num * sizeof(ssv_ble_bond_dev_t);

                pdest = malloc(buflen);
                if(pdest)
                {
                    pdest->status = ((struct soc_ble_get_bond_dev_cmpl_evt_param*)data)->status;
                    pdest->dev_num = ((struct soc_ble_get_bond_dev_cmpl_evt_param*)data)->dev_num;

                    pdest->bond_dev = 0;
                    if(pdest->dev_num > 0) {
                        pdest->bond_dev = (ssv_ble_bond_dev_t*)(((uint8_t*)pdest) + sizeof(struct ble_get_bond_dev_cmpl_evt_param));
                        memcpy(pdest+1, data+sizeof(struct soc_ble_get_bond_dev_cmpl_evt_param), sizeof(ssv_ble_bond_dev_t)*pdest->dev_num);
                    }
                    gap_ble_event_cb(evtId, (ssv_ble_gap_cb_param_t *)pdest);
                    free(pdest);
                }
                else printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, buflen);
            }
            break;
        default:
            gap_ble_event_cb(evtId, (ssv_ble_gap_cb_param_t *)data);
            break;
    }
    return;
}
#endif

