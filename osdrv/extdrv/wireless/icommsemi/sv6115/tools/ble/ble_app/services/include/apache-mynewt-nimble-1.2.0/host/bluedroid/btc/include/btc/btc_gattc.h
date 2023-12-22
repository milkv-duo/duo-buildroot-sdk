#ifndef __BTC_GATTC_H__
#define __BTC_GATTC_H__


#include "btc/btc_task.h"
#include "ssv_bt_defs.h"
#include "ssv_gatt_defs.h"
#include "ssv_gattc_api.h"
#include "osi/future.h"

typedef enum {
    BTC_GATTC_ACT_APP_REGISTER = 0,
    BTC_GATTC_ACT_APP_UNREGISTER,
    BTC_GATTC_ACT_OPEN,
    BTC_GATTC_ACT_CLOSE,
    BTC_GATTC_ACT_CFG_MTU,
    BTC_GATTC_ACT_SEARCH_SERVICE,
    BTC_GATTC_ACT_READ_CHAR,
    BTC_GATTC_ACT_READ_MULTIPLE_CHAR,
    BTC_GATTC_ACT_READ_CHAR_DESCR,
    BTC_GATTC_ACT_WRITE_CHAR,
    BTC_GATTC_ACT_WRITE_CHAR_DESCR,
    BTC_GATTC_ACT_PREPARE_WRITE,
    BTC_GATTC_ACT_PREPARE_WRITE_CHAR_DESCR,
    BTC_GATTC_ACT_EXECUTE_WRITE,
    BTC_GATTC_ACT_REG_FOR_NOTIFY,
    BTC_GATTC_ACT_UNREG_FOR_NOTIFY,
    BTC_GATTC_ACT_CACHE_REFRESH,
    BTC_GATTC_ACT_CACHE_ASSOC,
    BTC_GATTC_ATC_CACHE_GET_ADDR_LIST,
    BTC_GATTC_ACT_CACHE_CLEAN,
} btc_gattc_act_t;

/* btc_ble_gattc_args_t */
typedef union {
    //BTC_GATTC_ACT_APP_REGISTER,
    struct app_reg_arg {
        uint16_t app_id;
    } app_reg;
    //BTC_GATTC_ACT_APP_UNREGISTER,
    struct app_unreg_arg {
        ssv_gatt_if_t gattc_if;
    } app_unreg;
    //BTC_GATTC_ACT_OPEN,
    struct open_arg {
        ssv_gatt_if_t gattc_if;
        ssv_bd_addr_t remote_bda;
        ssv_ble_addr_type_t remote_addr_type;
        bool is_direct;
    } open;
    //BTC_GATTC_ACT_CLOSE,
    struct close_arg {
        uint16_t conn_id;
    } close;
    //BTC_GATTC_ACT_CFG_MTU,
    struct cfg_mtu_arg {
        uint16_t conn_id;
    } cfg_mtu;
    //BTC_GATTC_ACT_SEARCH_SERVICE,
    struct search_srvc_arg {
        uint16_t conn_id;
        bool filter_uuid_enable;
        ssv_bt_uuid_t filter_uuid;
    } search_srvc;
    //BTC_GATTC_ACT_GET_CHAR,
    struct get_char_arg {
        uint16_t conn_id;
        uint16_t handle;
    } get_char;
    //BTC_GATTC_ACT_GET_DESCR,
    struct get_descr_arg {
        uint16_t conn_id;
        uint16_t handle;
    } get_descr;
    //BTC_GATTC_ACT_GET_FIRST_INCL_SERVICE,
    struct get_first_incl_srvc_arg {
        uint16_t conn_id;
        uint16_t handle;
    } get_first_incl_srvc;
    //BTC_GATTC_ACT_GET_NEXT_INCL_SERVICE,
    struct get_next_incl_srvc_arg {
        uint16_t conn_id;
        uint16_t handle;
    } get_next_incl_srvc;
    //BTC_GATTC_ACT_READ_CHAR,
    struct read_char_arg {
        uint16_t conn_id;
        uint16_t handle;
        ssv_gatt_auth_req_t auth_req;
    } read_char;
    //BTC_GATTC_ACT_READ_MULTIPLE_CHAR
    struct read_multiple_arg {
        uint16_t conn_id;
        uint8_t  num_attr;
        uint16_t handles[SSV_GATT_MAX_READ_MULTI_HANDLES];
        ssv_gatt_auth_req_t auth_req;
    } read_multiple;
    //BTC_GATTC_ACT_READ_CHAR_DESCR,
    struct read_descr_arg {
        uint16_t conn_id;
        uint16_t handle;
        ssv_gatt_auth_req_t auth_req;
    } read_descr;
    //BTC_GATTC_ACT_WRITE_CHAR,
    struct write_char_arg {
        uint16_t conn_id;
        uint16_t value_len;
        uint16_t handle;
        uint8_t *value;
        ssv_gatt_write_type_t write_type;
        ssv_gatt_auth_req_t auth_req;
    } write_char;
    //BTC_GATTC_ACT_WRITE_CHAR_DESCR,
    struct write_descr_arg {
        uint16_t conn_id;
        uint16_t value_len;
        uint16_t handle;
        uint8_t *value;
        ssv_gatt_write_type_t write_type;
        ssv_gatt_auth_req_t auth_req;
    } write_descr;
    //BTC_GATTC_ACT_PREPARE_WRITE,
    struct prep_write_arg {
        uint16_t conn_id;
        uint16_t handle;
        uint16_t offset;
        uint16_t value_len;
        uint8_t *value;
        ssv_gatt_auth_req_t auth_req;
    } prep_write;
    //BTC_GATTC_ACT_PREPARE_WRITE_CHAR_DESCR,
    struct prep_write_descr_arg {
        uint16_t conn_id;
        uint16_t handle;
        uint16_t offset;
        uint16_t value_len;
        uint8_t *value;
        ssv_gatt_auth_req_t auth_req;
    } prep_write_descr;
    //BTC_GATTC_ACT_EXECUTE_WRITE,
    struct exec_write_arg {
        uint16_t conn_id;
        bool is_execute;
    } exec_write;
    //BTC_GATTC_ACT_REG_FOR_NOTIFY,
    struct reg_for_notify_arg {
        ssv_gatt_if_t gattc_if;
        ssv_bd_addr_t remote_bda;
        uint16_t handle;
    } reg_for_notify;
    //BTC_GATTC_ACT_UNREG_FOR_NOTIFY
    struct unreg_for_notify_arg {
        ssv_gatt_if_t gattc_if;
        ssv_bd_addr_t remote_bda;
        uint16_t handle;
    } unreg_for_notify;
    //BTC_GATTC_ACT_CACHE_REFRESH,
    struct cache_refresh_arg {
        ssv_bd_addr_t remote_bda;
    } cache_refresh;
    //BTC_GATTC_ACT_CACHE_ASSOC
    struct cache_assoc_arg {
        ssv_gatt_if_t gattc_if;
        ssv_bd_addr_t src_addr;
        ssv_bd_addr_t assoc_addr;
        bool is_assoc;
    } cache_assoc;
    //BTC_GATTC_ATC_CACHE_GET_ADDR_LIST
    struct cache_get_addr_list_arg {
        ssv_gatt_if_t gattc_if;
    }get_addr_list;
    //BTC_GATTC_ACT_CACHE_CLEAN,
    struct cache_clean_arg {
        ssv_bd_addr_t remote_bda;
    } cache_clean;
} btc_ble_gattc_args_t;


void btc_gattc_call_handler(btc_msg_t *msg);
void btc_gattc_cb_handler(btc_msg_t *msg);
void btc_gattc_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
ssv_gatt_status_t btc_gattc_get_attr_value(uint16_t attr_handle, uint16_t *length, uint8_t **value);
void btc_gattc_cb_handler(btc_msg_t *msg);
void btc_gattc_call_handler(btc_msg_t *msg);

ssv_gatt_status_t btc_ble_gattc_get_service(uint16_t conn_id, ssv_bt_uuid_t *svc_uuid,
    ssv_gattc_service_elem_t *result,
    uint16_t *count, uint16_t offset);

ssv_gatt_status_t btc_ble_gattc_get_all_char(uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    ssv_gattc_char_elem_t *result,
    uint16_t *count, uint16_t offset);

ssv_gatt_status_t btc_ble_gattc_get_all_descr(uint16_t conn_id,
    uint16_t char_handle,
    ssv_gattc_descr_elem_t *result,
    uint16_t *count, uint16_t offset);

ssv_gatt_status_t btc_ble_gattc_get_descr(uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    ssv_gattc_descr_elem_t *result,
    uint16_t *count, uint16_t offset);

ssv_gatt_status_t btc_ble_gattc_get_char_by_uuid(uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    ssv_bt_uuid_t char_uuid,
    ssv_gattc_char_elem_t *result,
    uint16_t *count);

ssv_gatt_status_t btc_ble_gattc_get_descr_by_uuid(uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    ssv_bt_uuid_t char_uuid,
    ssv_bt_uuid_t descr_uuid,
    ssv_gattc_descr_elem_t *result,
    uint16_t *count);

ssv_gatt_status_t btc_ble_gattc_get_descr_by_char_handle(uint16_t conn_id,
    uint16_t char_handle,
    ssv_bt_uuid_t descr_uuid,
    ssv_gattc_descr_elem_t *result,
    uint16_t *count);

ssv_gatt_status_t btc_ble_gattc_get_include_service(uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    ssv_bt_uuid_t *incl_uuid,
    ssv_gattc_incl_svc_elem_t *result,
    uint16_t *count);

ssv_gatt_status_t btc_ble_gattc_get_attr_count(uint16_t conn_id,
    ssv_gatt_db_attr_type_t type,
    uint16_t start_handle,
    uint16_t end_handle,
    uint16_t char_handle,
    uint16_t *count);

ssv_gatt_status_t btc_ble_gattc_get_db(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle,
    ssv_gattc_db_elem_t *db, uint16_t *count);

#define CLIENT_IF_ID 2
#define BTC_GATTC_GET_ALL_CHAR_QUEUE_SIZE 20
#endif /* __BTC_GATTC_H__ */