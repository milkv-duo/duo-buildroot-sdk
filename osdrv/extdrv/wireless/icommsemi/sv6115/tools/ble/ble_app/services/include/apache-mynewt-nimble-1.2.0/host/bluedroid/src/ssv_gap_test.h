#ifndef _SSV_GAP_TEST_H_
#define _SSV_GAP_TEST_H_

#include "ssv_gap_ble_api.h"
#if BLE_GATTC_EN
#include "ssv_gattc_api.h"
#endif
#define CONFIG_SET_RAW_ADV_DATA 1

#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)

#if BLE_GATTC_EN
struct gattc_profile_inst {
    ssv_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    ssv_bd_addr_t remote_bda;
};
#endif


void gap_event_handler(ssv_gap_ble_cb_event_t event, ssv_ble_gap_cb_param_t *param);

#endif