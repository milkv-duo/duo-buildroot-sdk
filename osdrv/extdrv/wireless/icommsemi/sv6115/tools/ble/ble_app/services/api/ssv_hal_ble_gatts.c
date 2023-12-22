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

#include "ssv_gatts_api.h"
#include <ssv_ble_services.h>
#include <ssv_netlink.h>
#include "ssv_nimble.h"
#include "ssv_hal_ble_gatts.h"

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
static ssv_hal_ble_gatts_profile_event_handler_callback_t gatts_ble_event_cb;

/*******************************************************************************
 *         Local Funcations
 ******************************************************************************/


/*******************************************************************************
 *         Global Funcations
 ******************************************************************************/
int ssv_hal_ble_gatts_init(void)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_GATTS_INIT;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD)); 
    return 0;
}

int ssv_hal_ble_gatts_deinit(void)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_GATTS_DEINIT;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD)); 
    return 0;
}

int ssv_hal_ble_gatts_set_profile_cb(ssv_hal_ble_gatts_profile_event_handler_callback_t profile_cb)
{
    ST_NIMBLE_CMD  bleApi;
    memset(&bleApi,0,sizeof(ST_NIMBLE_CMD));
    bleApi.cmdid=E_SSV_HAL_BLE_GATTS_SET_PROFILE_CB;
    ssv_netlink_send_cmd((char *)&bleApi,sizeof(ST_NIMBLE_CMD)); 
    gatts_ble_event_cb=profile_cb;
    return 0;
}

#if __WORDSIZE == 32
int ssv_hal_ble_gatts_create_service_by_db(ssv_gatts_attr_db_t *gatts_attr_db, uint8_t max_nb_attr, uint16_t *out_handle)
{
    ST_NIMBLE_CMD *bleApi;
    uint8_t *psrc;
    uint16_t i, buflen;
    struct gatts_create_service {
        ssv_gatts_attr_db_t *gatts_attr_db;
        uint8_t             max_nb_attr;
        uint16_t            *out_handle;
    } *pService;

    if((NULL == gatts_attr_db) || (0 == max_nb_attr))
    {
        printf("gatts_attr_db:%p, max_nb_attr:%d, failed!!\n", gatts_attr_db, max_nb_attr);
        return -1;
    }

    buflen=0;
    for(i=0; i<max_nb_attr; i++)
    {
        buflen += gatts_attr_db[i].att_desc.uuid_length + gatts_attr_db[i].att_desc.length;
    }
    buflen += sizeof(struct gatts_create_service)+max_nb_attr*sizeof(ssv_gatts_attr_db_t);

    bleApi = (ST_NIMBLE_CMD*)malloc(sizeof(ST_NIMBLE_CMD)+buflen);
    if(NULL == bleApi) {
        printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, buflen);
        return -1;
    }

    pService = (struct gatts_create_service*)bleApi->data;
    pService->gatts_attr_db = gatts_attr_db; //dummy
    pService->max_nb_attr   = max_nb_attr;
    pService->out_handle    = out_handle; //dummy

    psrc = (uint8_t*)pService;
    psrc += sizeof(struct gatts_create_service);

    memcpy(psrc, (char*)gatts_attr_db, max_nb_attr*sizeof(ssv_gatts_attr_db_t));
    psrc += max_nb_attr*sizeof(ssv_gatts_attr_db_t);

    for(i=0; i<max_nb_attr; i++)
    {
        if((NULL != gatts_attr_db[i].att_desc.uuid_p) && (gatts_attr_db[i].att_desc.uuid_length > 0))
        {
            memcpy(psrc, gatts_attr_db[i].att_desc.uuid_p, gatts_attr_db[i].att_desc.uuid_length);
            psrc += gatts_attr_db[i].att_desc.uuid_length;
        }

        if((NULL != gatts_attr_db[i].att_desc.value) && (gatts_attr_db[i].att_desc.length > 0))
        {
            memcpy(psrc, gatts_attr_db[i].att_desc.value, gatts_attr_db[i].att_desc.length);
            psrc += gatts_attr_db[i].att_desc.length;
        }
    }

    bleApi->datalen = buflen;
    bleApi->cmdid=E_SSV_HAL_BLE_GATTS_CREATE_SERVICE_DB;
    ssv_netlink_send_cmd((char *)bleApi, sizeof(ST_NIMBLE_CMD)+buflen);
    free(bleApi);

    return 0;
}

uint16_t ssv_hal_ble_gatts_send_indication(uint16_t attrib_offset, uint8_t *data, uint16_t len, bool need_confirm)
{
    ST_NIMBLE_CMD *bleApi;
    struct gatts_send_indication {
        uint16_t    attrib_offset;
        uint8_t     *data;
        uint16_t    len;
        bool        need_confirm;
    } *pIndication;

    bleApi = (ST_NIMBLE_CMD*)malloc(sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_indication)+len);
    if(NULL == bleApi) {
        printf("[%s] malloc(%u) failed!!\n", __func__, (uint32_t)(sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_indication)+len));
        return -1;
    }

    pIndication = (struct gatts_send_indication*)bleApi->data;
    pIndication->attrib_offset  = attrib_offset;
    pIndication->data           = data; //dummy
    pIndication->len            = len;
    pIndication->need_confirm   = need_confirm;

    if((NULL != data) && (len > 0))
        memcpy(bleApi->data+sizeof(struct gatts_send_indication), data, len);

    bleApi->datalen = sizeof(struct gatts_send_indication)+len;
    bleApi->cmdid=E_SSV_HAL_BLE_GATTS_SEND_INDICATION;
    ssv_netlink_send_cmd((char *)bleApi, sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_indication)+len);
    free(bleApi);

    return 0;
}

void ssv_hal_gatts_event_cb(ssv_gatts_cb_event_t evtId, char *data, unsigned int len)
{
    if((NULL == data) || (0 == len))
    {
        printf("[%s] data:%p, len:%x, failed!!\n", __func__, data, len);
        return;
    }
    switch(evtId){
        case SSV_GATTS_WRITE_EVT:
            {
                struct gatts_write_evt_param *write_param = (struct gatts_write_evt_param*)data;
                write_param->value = (uint8_t*)(data + sizeof(struct gatts_write_evt_param));
                gatts_ble_event_cb(SSV_GATTS_WRITE_EVT, 0, (ssv_ble_gatts_cb_param_t*)data);
            }
            break;
        case SSV_GATTS_CONF_EVT:
            {
                struct gatts_conf_evt_param *conf_param = (struct gatts_conf_evt_param*)data;
                conf_param->value = (uint8_t*)(data + sizeof(struct gatts_conf_evt_param));
                gatts_ble_event_cb(SSV_GATTS_CONF_EVT, 0, (ssv_ble_gatts_cb_param_t*)data);
            }
            break;
        case SSV_GATTS_CREAT_ATTR_TAB_EVT:
            {
                struct gatts_add_attr_tab_evt_param *attr_param = (struct gatts_add_attr_tab_evt_param*)data;
                attr_param->handles = (uint16_t*)(data + sizeof(struct gatts_add_attr_tab_evt_param));
                gatts_ble_event_cb(SSV_GATTS_CREAT_ATTR_TAB_EVT, 0, (ssv_ble_gatts_cb_param_t*)data);
            }
            break;
        default:
            gatts_ble_event_cb(evtId, 0, (ssv_ble_gatts_cb_param_t *)data);
            break;
    }
    return;
}

ssv_err_t ssv_ble_gatts_send_response(ssv_gatt_if_t gatts_if, uint16_t conn_id, 
                                        uint32_t trans_id, ssv_gatt_status_t status, ssv_gatt_rsp_t *rsp)
{
    ST_NIMBLE_CMD *bleApi;
    struct gatts_send_response {
        ssv_gatt_if_t       gatts_if;
        uint16_t            conn_id;
        uint32_t            trans_id;
        ssv_gatt_status_t   status;
        ssv_gatt_rsp_t      *rsp;
    } *pResponse;

    bleApi = (ST_NIMBLE_CMD*)malloc(sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_response)+sizeof(ssv_gatt_rsp_t));
    if(NULL == bleApi) {
        printf("[%s] malloc(%u) failed!!\n", __func__, (uint32_t)(sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_response)+sizeof(ssv_gatt_rsp_t)));
        return -1;
    }

    pResponse = (struct gatts_send_response*)bleApi->data;
    pResponse->gatts_if = gatts_if;
    pResponse->conn_id  = conn_id;
    pResponse->trans_id = trans_id;
    pResponse->status   = status;
    pResponse->rsp      = rsp; //dummy

    if(NULL != rsp)
        memcpy(bleApi->data+sizeof(struct gatts_send_response), rsp, sizeof(ssv_gatt_rsp_t));

    bleApi->datalen = sizeof(struct gatts_send_response)+sizeof(ssv_gatt_rsp_t);
    bleApi->cmdid = E_SSV_HAL_BLE_GATTS_SEND_RESPONSE;
    ssv_netlink_send_cmd((char *)bleApi, sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_response)+sizeof(ssv_gatt_rsp_t));
    free(bleApi);

    return 0;
}

#else //__WORDSIZE == 32

struct soc_gatts_write_evt_param {
    uint16_t conn_id;               /*!< Connection id */
    uint32_t trans_id;              /*!< Transfer id */
    ssv_bd_addr_t bda;              /*!< The bluetooth device address which been written */
    uint16_t handle;                /*!< The attribute handle */
    uint16_t offset;                /*!< Offset of the value, if the value is too long */
    uint8_t need_rsp;               /*!< The write operation need to do response */
    uint8_t is_prep;                /*!< This write operation is prepare write */
    uint16_t len;                   /*!< The write attribute value length */
    ptr_32bits value;               /*!< The write attribute value */
};                                  /*!< Gatt server callback param of SSV_GATTS_WRITE_EVT */

struct soc_gatts_conf_evt_param {
    ssv_gatt_status_t status;       /*!< Operation status */
    uint16_t conn_id;               /*!< Connection id */
    uint16_t handle;                /*!< attribute handle */
    uint16_t len;                   /*!< The indication or notification value length, len is valid when send notification or indication failed */
    ptr_32bits value;               /*!< The indication or notification value , value is valid when send notification or indication failed */
};                                  /*!< Gatt server callback param of SSV_GATTS_CONF_EVT (confirm) */

struct soc_gatts_add_attr_tab_evt_param{
    ssv_gatt_status_t status;       /*!< Operation status */
    ssv_bt_uuid_t svc_uuid;         /*!< Service uuid type */
    uint8_t svc_inst_id;            /*!< Service id */
    uint16_t num_handle;            /*!< The number of the attribute handle to be added to the gatts database */
    ptr_32bits handles;             /*!< The number to the handles */
};                                  /*!< Gatt server callback param of SSV_GATTS_CREAT_ATTR_TAB_EVT */
//////////////////////////////
void ssv_hal_gatts_event_cb(ssv_gatts_cb_event_t evtId, char *data, unsigned int len)
{
    if((NULL == data) || (0 == len))
    {
        printf("[%s] data:%p, len:%x, failed!!\n", __func__, data, len);
        return;
    }
    switch(evtId)
    {
        case SSV_GATTS_WRITE_EVT:
            {
                int buflen;
                struct gatts_write_evt_param *pdest;

                buflen = sizeof(struct gatts_write_evt_param) + ((struct soc_gatts_write_evt_param*)data)->len;
                pdest = malloc(buflen);
                if(pdest)
                {
                    pdest->conn_id  = ((struct soc_gatts_write_evt_param*)data)->conn_id;
                    pdest->trans_id = ((struct soc_gatts_write_evt_param*)data)->trans_id;

                    //pdest->bda    = ((struct soc_gatts_write_evt_param*)data)->bda;
                    memcpy(pdest->bda, ((struct soc_gatts_write_evt_param*)data)->bda, sizeof(ssv_bd_addr_t));

                    pdest->handle   = ((struct soc_gatts_write_evt_param*)data)->handle;
                    pdest->offset   = ((struct soc_gatts_write_evt_param*)data)->offset;
                    pdest->need_rsp = (uint8_t)((struct soc_gatts_write_evt_param*)data)->need_rsp;
                    pdest->is_prep  = (uint8_t)((struct soc_gatts_write_evt_param*)data)->is_prep;
                    pdest->len      = ((struct soc_gatts_write_evt_param*)data)->len;

                    pdest->value = 0;
                    if(pdest->len > 0) {
                        pdest->value = (((uint8_t*)pdest) + sizeof(struct gatts_write_evt_param));
                        memcpy(pdest+1, data+sizeof(struct soc_gatts_write_evt_param), pdest->len);
                    }
                    gatts_ble_event_cb(SSV_GATTS_WRITE_EVT, 0, (ssv_ble_gatts_cb_param_t*)pdest);
                    free(pdest);
                }
                else printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, buflen);
            }
            break;
        case SSV_GATTS_CONF_EVT:
            {
                int buflen;
                struct gatts_conf_evt_param *pdest;

                buflen = sizeof(struct gatts_conf_evt_param) + ((struct soc_gatts_conf_evt_param*)data)->len;
                pdest = malloc(buflen);
                if(pdest)
                {
                    pdest->status   = ((struct soc_gatts_conf_evt_param*)data)->status;
                    pdest->conn_id  = ((struct soc_gatts_conf_evt_param*)data)->conn_id;
                    pdest->handle   = ((struct soc_gatts_conf_evt_param*)data)->handle;
                    pdest->len      = ((struct soc_gatts_conf_evt_param*)data)->len;

                    pdest->value = 0;
                    if(pdest->len > 0) {
                        pdest->value = (((uint8_t*)pdest) + sizeof(struct gatts_conf_evt_param));
                        memcpy(pdest+1, data+sizeof(struct soc_gatts_conf_evt_param), pdest->len);
                    }
                    gatts_ble_event_cb(SSV_GATTS_CONF_EVT, 0, (ssv_ble_gatts_cb_param_t*)pdest);
                    free(pdest);
                }
                else printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, buflen);
            }
            break;
        case SSV_GATTS_CREAT_ATTR_TAB_EVT:
            {
                int buflen;
                struct gatts_add_attr_tab_evt_param *pdest;

                buflen = sizeof(struct gatts_add_attr_tab_evt_param) +
                            ((struct soc_gatts_add_attr_tab_evt_param*)data)->num_handle  * sizeof(uint16_t);
                pdest = malloc(buflen);
                if(pdest)
                {
                    pdest->status       = ((struct soc_gatts_add_attr_tab_evt_param*)data)->status;

                    pdest->svc_uuid     = ((struct soc_gatts_add_attr_tab_evt_param*)data)->svc_uuid;
                    //memcpy(pdest->svc_uuid, ((struct soc_gatts_add_attr_tab_evt_param*)data)->svc_uuid, sizeof(ssv_bt_uuid_t));

                    pdest->svc_inst_id  = ((struct soc_gatts_add_attr_tab_evt_param*)data)->svc_inst_id;
                    pdest->num_handle   = ((struct soc_gatts_add_attr_tab_evt_param*)data)->num_handle;

                    pdest->handles = 0;
                    if(pdest->num_handle > 0) {
                        pdest->handles = (uint16_t*)(((uint8_t*)pdest) + sizeof(struct gatts_add_attr_tab_evt_param));
                        memcpy(pdest+1, data+sizeof(struct soc_gatts_add_attr_tab_evt_param), pdest->num_handle * sizeof(uint16_t));
                    }

                    gatts_ble_event_cb(SSV_GATTS_CREAT_ATTR_TAB_EVT, 0, (ssv_ble_gatts_cb_param_t*)pdest);
                    free(pdest);
                }
                else printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, buflen);
            }
            break;
        default:
            gatts_ble_event_cb(evtId, 0, (ssv_ble_gatts_cb_param_t *)data);
            break;
    }
    return;
}

//////////////////////////////
typedef struct
 {
     uint16_t   uuid_length;             /*!< UUID length */
     ptr_32bits uuid_p;                  /*!< UUID value */
     uint16_t   perm;                    /*!< Attribute permission */
     uint16_t   max_length;              /*!< Maximum length of the element*/
     uint16_t   length;                  /*!< Current length of the element*/
     ptr_32bits value;                   /*!< Element value array*/
 } soc_attr_desc_t;

typedef struct
{
    ssv_attr_control_t      attr_control;  /*!< The attribute control type */
    soc_attr_desc_t         att_desc;      /*!< The attribute type */
} soc_gatts_attr_db_t;

/////////////////////////////
int ssv_hal_ble_gatts_create_service_by_db(ssv_gatts_attr_db_t *gatts_attr_db, uint8_t max_nb_attr, uint16_t *out_handle)
{
    ST_NIMBLE_CMD *bleApi;
    uint8_t *psrc;
    uint16_t i, buflen;
    struct gatts_create_service {
        ptr_32bits      gatts_attr_db;
        uint8_t         max_nb_attr;
        ptr_32bits      out_handle;
    } *pService;

    if((NULL == gatts_attr_db) || (0 == max_nb_attr))
    {
        printf("gatts_attr_db:%p, max_nb_attr:%d, failed!!\n", gatts_attr_db, max_nb_attr);
        return -1;
    }

    buflen=0;
    for(i=0; i<max_nb_attr; i++)
    {
        buflen += gatts_attr_db[i].att_desc.uuid_length + gatts_attr_db[i].att_desc.length;
    }
    buflen += sizeof(struct gatts_create_service)+max_nb_attr*sizeof(soc_gatts_attr_db_t);

    bleApi = (ST_NIMBLE_CMD*)malloc(sizeof(ST_NIMBLE_CMD)+buflen);
    if(NULL == bleApi) {
        printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, buflen);
        return -1;
    }

    pService = (struct gatts_create_service*)bleApi->data;
    pService->gatts_attr_db = ptr_dummy; //gatts_attr_db
    pService->max_nb_attr   = max_nb_attr;
    pService->out_handle    = ptr_dummy; //out_handle

    psrc = (uint8_t*)pService;
    psrc += sizeof(struct gatts_create_service);

    for(i=0; i<max_nb_attr; i++)
    {
        ((ssv_gatts_attr_db_t*)psrc)->attr_control          = gatts_attr_db[i].attr_control;

        ((ssv_gatts_attr_db_t*)psrc)->att_desc.uuid_length  = gatts_attr_db[i].att_desc.uuid_length;
        ((ssv_gatts_attr_db_t*)psrc)->att_desc.uuid_p       = gatts_attr_db[i].att_desc.uuid_p;
        ((ssv_gatts_attr_db_t*)psrc)->att_desc.perm         = gatts_attr_db[i].att_desc.perm;
        ((ssv_gatts_attr_db_t*)psrc)->att_desc.max_length   = gatts_attr_db[i].att_desc.max_length;
        ((ssv_gatts_attr_db_t*)psrc)->att_desc.length       = gatts_attr_db[i].att_desc.length;
        ((ssv_gatts_attr_db_t*)psrc)->att_desc.value        = gatts_attr_db[i].att_desc.value;

        //memcpy(psrc, (char*)gatts_attr_db, max_nb_attr*sizeof(ssv_gatts_attr_db_t));
        psrc += max_nb_attr*sizeof(soc_gatts_attr_db_t);
    }

    for(i=0; i<max_nb_attr; i++)
    {
        if((NULL != gatts_attr_db[i].att_desc.uuid_p) && (gatts_attr_db[i].att_desc.uuid_length > 0))
        {
            memcpy(psrc, gatts_attr_db[i].att_desc.uuid_p, gatts_attr_db[i].att_desc.uuid_length);
            psrc += gatts_attr_db[i].att_desc.uuid_length;
        }

        if((NULL != gatts_attr_db[i].att_desc.value) && (gatts_attr_db[i].att_desc.length > 0))
        {
            memcpy(psrc, gatts_attr_db[i].att_desc.value, gatts_attr_db[i].att_desc.length);
            psrc += gatts_attr_db[i].att_desc.length;
        }
    }

    bleApi->datalen = buflen;
    bleApi->cmdid=E_SSV_HAL_BLE_GATTS_CREATE_SERVICE_DB;
    ssv_netlink_send_cmd((char *)bleApi, sizeof(ST_NIMBLE_CMD)+buflen);
    free(bleApi);

    return 0;
}

uint16_t ssv_hal_ble_gatts_send_indication(uint16_t attrib_offset, uint8_t *data, uint16_t len, bool need_confirm)
{
    ST_NIMBLE_CMD *bleApi;
    struct gatts_send_indication {
        uint16_t    attrib_offset;
        ptr_32bits  data;
        uint16_t    len;
        uint8_t     need_confirm;
    } *pIndication;

    bleApi = (ST_NIMBLE_CMD*)malloc(sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_indication)+len);
    if(NULL == bleApi) {
        printf("[%s] malloc(%u) failed!!\n", __func__, (uint32_t)(sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_indication)+len));
        return -1;
    }

    pIndication = (struct gatts_send_indication*)bleApi->data;
    pIndication->attrib_offset  = attrib_offset;
    pIndication->data           = ptr_dummy; //data
    pIndication->len            = len;
    pIndication->need_confirm   = (uint8_t)need_confirm;

    if((NULL != data) && (len > 0))
        memcpy(bleApi->data+sizeof(struct gatts_send_indication), data, len);

    bleApi->datalen = sizeof(struct gatts_send_indication)+len;
    bleApi->cmdid=E_SSV_HAL_BLE_GATTS_SEND_INDICATION;
    ssv_netlink_send_cmd((char *)bleApi, sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_indication)+len);
    free(bleApi);

    return 0;
}

ssv_err_t ssv_ble_gatts_send_response(ssv_gatt_if_t gatts_if, uint16_t conn_id,
                                        uint32_t trans_id, ssv_gatt_status_t status, ssv_gatt_rsp_t *rsp)
{
    ST_NIMBLE_CMD *bleApi;
    struct gatts_send_response {
        ssv_gatt_if_t       gatts_if;
        uint16_t            conn_id;
        uint32_t            trans_id;
        ssv_gatt_status_t   status;
        ptr_32bits          rsp;
    } *pResponse;

    bleApi = (ST_NIMBLE_CMD*)malloc(sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_response)+sizeof(ssv_gatt_rsp_t));
    if(NULL == bleApi) {
        printf("[%s] malloc(%u) failed!!\n", __func__, (uint32_t)(sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_response)+sizeof(ssv_gatt_rsp_t)));
        return -1;
    }

    pResponse = (struct gatts_send_response*)bleApi->data;
    pResponse->gatts_if = gatts_if;
    pResponse->conn_id  = conn_id;
    pResponse->trans_id = trans_id;
    pResponse->status   = status;
    pResponse->rsp      = ptr_dummy; //rsp

    if(NULL != rsp)
        memcpy(bleApi->data+sizeof(struct gatts_send_response), rsp, sizeof(ssv_gatt_rsp_t));

    bleApi->datalen = sizeof(struct gatts_send_response)+sizeof(ssv_gatt_rsp_t);
    bleApi->cmdid = E_SSV_HAL_BLE_GATTS_SEND_RESPONSE;
    ssv_netlink_send_cmd((char *)bleApi, sizeof(ST_NIMBLE_CMD)+sizeof(struct gatts_send_response)+sizeof(ssv_gatt_rsp_t));
    free(bleApi);

    return 0;
}
#endif //__WORDSIZE == 32

