#ifndef __SSV_HAL_BLE_GATTC_H__
#define __SSV_HAL_BLE_GATTC_H__

#include "ssv_gatt_defs.h"
#include "ssv_gattc_api.h"

#define SSV_HAL_GATTC_PROFILE_NUM      1
#define SSV_HAL_GATTC_PROFILE_A_APP_ID 0
//#define INVALID_HANDLE   0


#define SSV_API_GATTC_OPEN_EVT             (1 << 0)
#define SSV_API_GATTC_WRITE_CHAR_EVT       (1 << 0)

typedef void (*ssv_hal_ble_gattc_profile_event_handler_callback_t)(ssv_gattc_cb_event_t event,
    ssv_gatt_if_t gattc_if, ssv_ble_gattc_cb_param_t *param);


int ssv_hal_ble_gattc_init();
int ssv_hal_ble_gattc_deinit();
void ssv_hal_ble_gattc_set_profile_cb(ssv_hal_ble_gattc_profile_event_handler_callback_t cb);
int ssv_hal_ble_gattc_open(ssv_gatt_if_t gattc_if, ssv_bd_addr_t remote_bda,
    ssv_ble_addr_type_t remote_addr_type, bool is_direct);

void ssv_api_gattc_event_set(uint32_t bit);
void ssv_api_gattc_event_clear_bit(uint32_t bitval);
int ssv_api_gattc_wait_event(uint32_t bit, char *failmsg);
#endif