#ifndef __SSV_HAL_BLE_GATTS_H__
#define __SSV_HAL_BLE_GATTS_H__

#include "ssv_gatt_defs.h"
#include "ssv_gatts_api.h"

#define GATTS_PROFILE_NUM 1
#define GATTS_PROFILE_A_APP_ID 0

#define SSV_API_GATTS_CREATE_ATT_ATB             (1 << 0)
#define SSV_API_GATTS_SEND_INDICATION             (1 << 1)
//#define SSV_API_GATTS_START_SERVICE              (1 << 1)

typedef void (*ssv_hal_ble_gatts_profile_event_handler_callback_t)(ssv_gatts_cb_event_t event,ssv_gatt_if_t gatts_if,
    ssv_ble_gatts_cb_param_t *param);

int ssv_hal_ble_gatts_init(void);
int ssv_hal_ble_gatts_deinit(void);

int ssv_hal_ble_gatts_create_service_by_db(
	ssv_gatts_attr_db_t *gatts_attr_db,	uint8_t max_nb_attr,
	uint16_t *out_handle);

uint16_t ssv_hal_ble_gatts_get_mut(void);
uint16_t ssv_hal_ble_gatts_send_indication(uint16_t attrib_offset,
	uint8_t *data, uint16_t len, bool need_confirm);

int ssv_hal_ble_gatts_set_profile_cb(
    ssv_hal_ble_gatts_profile_event_handler_callback_t profile_cb);

void ssv_api_gatts_event_set(uint32_t bit);
void ssv_api_gatts_event_clear_bit(uint32_t bitval);
int ssv_api_gatts_wait_event(uint32_t bit, char *failmsg);
#endif