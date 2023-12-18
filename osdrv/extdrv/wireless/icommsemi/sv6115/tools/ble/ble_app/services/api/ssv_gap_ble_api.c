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
 * @file ssv_gap_ble_api.c
 * @brief application api functions.
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
#include "ssv_err.h"
#include "ssv_nimble.h"

#include "ssv_gap_ble_api.h"

#define STREAM_TO_UINT8(u8, p)   {u8 = (uint8_t)(*(p)); (p) += 1;}

#define BTM_BLE_ADV_DATA_LEN_MAX        31
#define BTM_BLE_CACHE_ADV_DATA_MAX      62

/*******************************************************************************
 *         Global Funcations
 ******************************************************************************/
uint8_t *BTM_CheckAdvData(uint8_t *p_adv, uint8_t type, uint8_t *p_length)
{
    uint8_t *p = p_adv;
    uint8_t length;
    uint8_t adv_type;

    STREAM_TO_UINT8(length, p);

    while ( length && (p - p_adv <= BTM_BLE_CACHE_ADV_DATA_MAX)) {
        STREAM_TO_UINT8(adv_type, p);

        if ( adv_type == type ) {
            /* length doesn't include itself */
            *p_length = length - 1; /* minus the length of type */
            return p;
        }
        p += length - 1; /* skip the length of data */
        STREAM_TO_UINT8(length, p);
    }

    *p_length = 0;
    return NULL;
}

uint8_t *ssv_ble_resolve_adv_data( uint8_t *adv_data, uint8_t type, uint8_t *length)
{
    if (((type < SSV_BLE_AD_TYPE_FLAG) || (type > SSV_BLE_AD_TYPE_128SERVICE_DATA)) &&
            (type != SSV_BLE_AD_MANUFACTURER_SPECIFIC_TYPE)) {
        printf("the eir type not define, type = %x\n", type);
        return NULL;
    }

    if (adv_data == NULL) {
        printf("Invalid p_eir data.\n");
        return NULL;
    }

    return (BTM_CheckAdvData(adv_data, type, length));
}

static inline void *create_cmdbuf(uint8_t *buffer, uint32_t buflen)
{
    ST_NIMBLE_CMD *cmd = (ST_NIMBLE_CMD*)malloc(sizeof(ST_NIMBLE_CMD)+buflen);
    if(cmd) {
        memcpy(cmd->data, buffer, buflen);
        cmd->datalen = buflen;
    }
    else printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, (unsigned int)(sizeof(ST_NIMBLE_CMD)+buflen));
    return cmd;
}

ssv_err_t ssv_ble_gap_config_adv_data_raw(uint8_t *raw_data, uint32_t raw_data_len)
{
    ST_NIMBLE_CMD *bleApi=create_cmdbuf(raw_data, raw_data_len);
    if(bleApi) {
        bleApi->cmdid=E_SSV_HAL_BLE_GAP_CONFIG_ADV_DATA_RAW;
        ssv_netlink_send_cmd((char *)bleApi, sizeof(ST_NIMBLE_CMD)+raw_data_len);
        free(bleApi);
    }
    return 0;
}

#if __WORDSIZE == 32
ssv_err_t ssv_ble_gap_config_adv_data(ssv_ble_adv_data_t *adv_data)
{
    ST_NIMBLE_CMD *bleApi;
    uint16_t buflen;
    uint8_t *psrc;

    if(adv_data == NULL) return -1;
    buflen = sizeof(ssv_ble_adv_data_t) + adv_data->manufacturer_len + adv_data->service_data_len + adv_data->service_uuid_len;

    bleApi = (ST_NIMBLE_CMD*)malloc(sizeof(ST_NIMBLE_CMD)+buflen);
    if(NULL == bleApi) {
        printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, buflen);
        return -1;
    }

    psrc = bleApi->data;
    memcpy(psrc, adv_data, sizeof(ssv_ble_adv_data_t));
    psrc += sizeof(ssv_ble_adv_data_t);

    if(adv_data->manufacturer_len) {
        memcpy(psrc, adv_data->p_manufacturer_data, adv_data->manufacturer_len);
        psrc += adv_data->manufacturer_len;
    }
    if(adv_data->service_data_len) {
        memcpy(psrc, adv_data->p_service_data, adv_data->service_data_len);
        psrc += adv_data->service_data_len;
    }
    if(adv_data->service_uuid_len) {
        memcpy(psrc, adv_data->p_service_uuid, adv_data->service_uuid_len);
        psrc += adv_data->service_uuid_len;
    }

    bleApi->datalen = buflen;
    bleApi->cmdid=E_SSV_HAL_BLE_GAP_CONFIG_ADV_DATA;
    ssv_netlink_send_cmd((char *)bleApi, sizeof(ST_NIMBLE_CMD)+buflen);
    free(bleApi);

    return 0;
}
#else
ssv_err_t ssv_ble_gap_config_adv_data(ssv_ble_adv_data_t *adv_data)
{
    ST_NIMBLE_CMD *bleApi;
    uint16_t buflen;
    uint8_t *psrc;
    struct ssv_ble_adv_data {
        uint8_t     set_scan_rsp;
        uint8_t     include_name;
        uint8_t     include_txpower;
        int         min_interval;
        int         max_interval;
        int         appearance;
        uint16_t    manufacturer_len;
        ptr_32bits  p_manufacturer_data;
        uint16_t    service_data_len;
        ptr_32bits  p_service_data;
        uint16_t    service_uuid_len;
        ptr_32bits  p_service_uuid;
        uint8_t     flag;
    } *padv;

    if(adv_data == NULL) return -1;
    buflen = sizeof(struct ssv_ble_adv_data) + adv_data->manufacturer_len + adv_data->service_data_len + adv_data->service_uuid_len;

    bleApi = (ST_NIMBLE_CMD*)malloc(sizeof(ST_NIMBLE_CMD)+buflen);
    if(NULL == bleApi) {
        printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, buflen);
        return -1;
    }

    padv = (struct ssv_ble_adv_data *)bleApi->data;
    padv->set_scan_rsp      = (uint8_t)adv_data->set_scan_rsp;
    padv->include_name      = (uint8_t)adv_data->include_name;
    padv->include_txpower   = (uint8_t)adv_data->include_txpower;
    padv->min_interval      = adv_data->min_interval;
    padv->max_interval      = adv_data->max_interval;
    padv->appearance        = adv_data->appearance;
    padv->manufacturer_len  = adv_data->manufacturer_len;
    padv->service_data_len  = adv_data->service_data_len;
    padv->service_uuid_len  = adv_data->service_uuid_len;
    padv->flag              = adv_data->flag;
    padv->p_manufacturer_data = 0;
    padv->p_service_data    = 0;
    padv->p_service_uuid    = 0;

    psrc = (uint8_t *)padv;
    psrc += sizeof(struct ssv_ble_adv_data);
    if(adv_data->manufacturer_len) {
        memcpy(psrc, adv_data->p_manufacturer_data, adv_data->manufacturer_len);
        psrc += adv_data->manufacturer_len;
    }
    if(adv_data->service_data_len) {
        memcpy(psrc, adv_data->p_service_data, adv_data->service_data_len);
        psrc += adv_data->service_data_len;
    }
    if(adv_data->service_uuid_len) {
        memcpy(psrc, adv_data->p_service_uuid, adv_data->service_uuid_len);
        psrc += adv_data->service_uuid_len;
    }

    bleApi->datalen = buflen;
    bleApi->cmdid=E_SSV_HAL_BLE_GAP_CONFIG_ADV_DATA;
    ssv_netlink_send_cmd((char *)bleApi, sizeof(ST_NIMBLE_CMD)+buflen);
    free(bleApi);

    return 0;
}
#endif

ssv_err_t ssv_ble_gap_config_scan_rsp_data_raw(uint8_t *raw_data, uint32_t raw_data_len)
{
    ST_NIMBLE_CMD *bleApi=create_cmdbuf(raw_data, raw_data_len);
    if(bleApi) {
        bleApi->cmdid=E_SSV_HAL_BLE_GAP_CONFIG_SCAN_RSP_DATA;
        ssv_netlink_send_cmd((char *)bleApi,sizeof(ST_NIMBLE_CMD)+raw_data_len);
        free(bleApi);
    }
    return 0;
}

ssv_err_t ssv_ble_gap_start_advertising(ssv_ble_adv_params_t *adv_params)
{
    ST_NIMBLE_CMD *bleApi=create_cmdbuf((uint8_t*)adv_params, sizeof(ssv_ble_adv_params_t));
    if(bleApi) {
        bleApi->cmdid=E_SSV_HAL_BLE_GAP_START_ADVERTISING;
        ssv_netlink_send_cmd((char *)bleApi,sizeof(ST_NIMBLE_CMD)+sizeof(ssv_ble_adv_params_t));
        free(bleApi);
    }
    return 0;
}

ssv_err_t ssv_ble_gap_stop_advertising(void)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_GAP_STOP_ADVERTISING;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD));
    return 0;
}

ssv_err_t ssv_ble_gap_set_scan_params(ssv_ble_scan_params_t *scan_params)
{
    ST_NIMBLE_CMD *bleApi=create_cmdbuf((uint8_t*)scan_params, sizeof(ssv_ble_scan_params_t));
    if(bleApi) {
        bleApi->cmdid=E_SSV_HAL_BLE_GAP_SET_SCAN_PARAMS;
        ssv_netlink_send_cmd((char *)bleApi,sizeof(ST_NIMBLE_CMD)+sizeof(ssv_ble_scan_params_t));
        free(bleApi);
    }
    return 0;
}

ssv_err_t ssv_ble_gap_start_scanning(uint32_t duration)
{
    ST_NIMBLE_CMD *bleApi=create_cmdbuf((uint8_t*)&duration, sizeof(uint32_t));
    if(bleApi) {
        bleApi->cmdid=E_SSV_HAL_BLE_GAP_START_SCANNING;
        ssv_netlink_send_cmd((char *)bleApi,sizeof(ST_NIMBLE_CMD)+sizeof(uint32_t));
        free(bleApi);
    }
    return 0;
}

ssv_err_t ssv_ble_gap_stop_scanning(void)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_GAP_STOP_SCANNING;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD));
    return 0;
}

ssv_err_t ssv_ble_gap_disconnect(ssv_bd_addr_t remote_device)
{
    ST_NIMBLE_CMD *bleApi=create_cmdbuf((uint8_t*)remote_device, sizeof(ssv_bd_addr_t));
    if(bleApi) {
        bleApi->cmdid=E_SSV_HAL_BLE_GAP_DISCONNECT;
        ssv_netlink_send_cmd((char *)bleApi,sizeof(ST_NIMBLE_CMD)+sizeof(ssv_bd_addr_t));
        free(bleApi);
    }
    return 0;
}

