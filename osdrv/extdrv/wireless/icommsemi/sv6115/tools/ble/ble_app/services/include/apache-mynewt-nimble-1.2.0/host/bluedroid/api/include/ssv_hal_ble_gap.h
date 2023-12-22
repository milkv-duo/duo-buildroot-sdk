#ifndef __SSV_HAL_BLE_GAP_H__
#define __SSV_HAL_BLE_GAP_H__
#include "ssv_gap_ble_api.h"

#define SSV_API_GAP_CFG_ADV_DATA_RAW             (1 << 0)
#define SSV_API_GAP_CFG_ADV_DATA                 (1 << 1)
#define SSV_API_GAP_CFG_SCAN_RSP_DATA_RAW        (1 << 2)
#define SSV_API_GAP_CFG_SCAN_RSP_DATA            (1 << 3)
#define SSV_API_GAP_ADV_START_EVENT              (1 << 4)
#define SSV_API_GAP_ADV_STOP_EVENT               (1 << 5)
#define SSV_API_GAP_SCAN_PARAM_SET_COMPLETE_EVT  (1 << 6)
#define SSV_API_GAP_SCAN_START_COMPLETE_EVT      (1 << 7)
#define SSV_API_GAP_SCAN_STOP_COMPLETE_EVT       (1 << 8)

typedef void (*ssv_hal_gap_ble_event_cb_t)(ssv_gap_ble_cb_event_t event,
    ssv_ble_gap_cb_param_t *param);

int ssv_hal_ble_gap_init();
int ssv_hal_ble_gap_deinit();
void ssv_api_gap_event_set(uint32_t bit);
void ssv_api_gap_event_clear_bit(uint32_t bitval);
void ssv_api_gap_event_set_bit(uint32_t bitval);
int ssv_api_gap_wait_event(uint32_t bit, char *failmsg);
void ssv_hal_ble_gap_set_event_cb(ssv_hal_gap_ble_event_cb_t cb);
#endif