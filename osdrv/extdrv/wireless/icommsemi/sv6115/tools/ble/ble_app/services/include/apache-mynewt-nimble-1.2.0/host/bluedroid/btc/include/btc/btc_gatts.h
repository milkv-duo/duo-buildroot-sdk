#ifndef __BTC_GATTS_H__
#define __BTC_GATTS_H__


#include "btc/btc_task.h"
#include "ssv_bt_defs.h"
#include "ssv_gatt_defs.h"
#include "ssv_gatts_api.h"
#include "osi/future.h"

#define SERVER_IF_ID 1

typedef enum {
    BTC_GATTS_ACT_APP_REGISTER = 0,
    BTC_GATTS_ACT_APP_UNREGISTER,
    BTC_GATTS_ACT_CREATE_SERVICE,
    BTC_GATTS_ACT_CREATE_ATTR_TAB,
    BTC_GATTS_ACT_DELETE_SERVICE,
    BTC_GATTS_ACT_START_SERVICE,
    BTC_GATTS_ACT_STOP_SERVICE,
    BTC_GATTS_ACT_ADD_INCLUDE_SERVICE,
    BTC_GATTS_ACT_ADD_CHAR,
    BTC_GATTS_ACT_ADD_CHAR_DESCR,
    BTC_GATTS_ACT_SEND_INDICATE,
    BTC_GATTS_ACT_SEND_RESPONSE,
    BTC_GATTS_ACT_SET_ATTR_VALUE,
    BTC_GATTS_ACT_OPEN,
    BTC_GATTS_ACT_CLOSE,
    BTC_GATTS_ACT_SEND_SERVICE_CHANGE,
} btc_gatts_act_t;

/* btc_ble_gatts_args_t */
typedef union {
    //BTC_GATTS_ACT_APP_REGISTER = 0,
    struct app_reg_args {
        uint16_t app_id;
    } app_reg;

    //BTC_GATTS_ACT_APP_UNREGISTER,
    struct app_unreg_args {
        ssv_gatt_if_t gatts_if;
    } app_unreg;

    //BTC_GATTS_ACT_CREATE_SERVICE,
    struct create_srvc_args {
        ssv_gatt_if_t gatts_if;
        ssv_gatt_srvc_id_t service_id;
        uint16_t num_handle;
    } create_srvc;

    //BTC_GATTS_ACT_CREATE_ATTR_TAB
    struct create_attr_tab_args{
        ssv_gatt_if_t gatts_if;
        uint8_t srvc_inst_id;
        uint8_t max_nb_attr;
        ssv_gatts_attr_db_t *gatts_attr_db;
    }create_attr_tab;

    //BTC_GATTS_ACT_DELETE_SERVICE,
    struct delete_srvc_args {
        uint16_t service_handle;
    } delete_srvc;

    //BTC_GATTS_ACT_START_SERVICE,
    struct start_srvc_args {
        uint16_t service_handle;
    } start_srvc;

    //BTC_GATTS_ACT_STOP_SERVICE,
    struct stop_srvc_args {
        uint16_t service_handle;
    } stop_srvc;

    //BTC_GATTS_ACT_ADD_INCLUDE_SERVICE,
    struct add_incl_srvc_args {
        uint16_t service_handle;
        uint16_t included_service_handle;
    } add_incl_srvc;

    //BTC_GATTS_ACT_ADD_CHAR,
    struct add_char_args {
        uint16_t service_handle;
        ssv_bt_uuid_t char_uuid;
        ssv_gatt_perm_t perm;
        ssv_gatt_char_prop_t property;
        ssv_attr_control_t attr_control;
        ssv_attr_value_t char_val;
    } add_char;

    //BTC_GATTS_ACT_ADD_CHAR_DESCR,
    struct add_descr_args {
        uint16_t  service_handle;
        ssv_bt_uuid_t descr_uuid;
        ssv_gatt_perm_t perm;
        ssv_attr_control_t attr_control;
        ssv_attr_value_t descr_val;
    } add_descr;

    //BTC_GATTS_ACT_SEND_INDICATE,
    struct send_indicate_args {
        uint16_t conn_id;
        uint16_t attr_handle;
        bool need_confirm;
        uint16_t value_len;
        uint8_t *value;
    } send_ind;

    //BTC_GATTS_ACT_SEND_RESPONSE,
    struct send_rsp_args {
        uint16_t conn_id;
        uint32_t trans_id;
        ssv_gatt_status_t status;
        ssv_gatt_rsp_t *rsp;
    } send_rsp;

    //BTC_GATTS_SET_ATTR_VALUE
    struct set_attr_val_args {
        uint16_t handle;
        uint16_t length;
        uint8_t *value;
    } set_attr_val;

    //BTC_GATTS_ACT_OPEN,
    struct open_args {
        ssv_gatt_if_t gatts_if;
        ssv_bd_addr_t remote_bda;
        bool is_direct;
    } open;

    //BTC_GATTS_ACT_CLOSE,
    struct close_args {
        uint16_t conn_id;
    } close;

    //BTC_GATTS_ACT_SEND_SERVICE_CHANGE,
    struct send_service_change_args {
        ssv_gatt_if_t gatts_if;
        ssv_bd_addr_t remote_bda;
    } send_service_change;

} btc_ble_gatts_args_t;

typedef struct {
    future_t *complete_future;
    uint16_t svc_start_hdl;
    ssv_bt_uuid_t svc_uuid;
    bool        is_tab_creat_svc;
    bool      is_use_svc;
    uint8_t   num_handle;
    uint8_t   handle_idx;
    uint16_t handles[SSV_GATT_ATTR_HANDLE_MAX];
} ssv_btc_creat_tab_t;

void btc_gatts_call_handler(btc_msg_t *msg);
void btc_gatts_cb_handler(btc_msg_t *msg);
void btc_gatts_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
ssv_gatt_status_t btc_gatts_get_attr_value(uint16_t attr_handle, uint16_t *length, uint8_t **value);
void btc_gatts_free(void);
#endif /* __BTC_GATTS_H__ */