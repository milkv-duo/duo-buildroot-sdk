/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SSV_GATT_API_H_
#define _SSV_GATT_API_H_

#include <string.h>
#include "mesh_types.h"
#include "mesh_util.h"
#include "mesh_buf.h"
#include "mesh_uuid.h"


#include "sdkconfig.h"

#include "host/ble_gap.h"


void bt_gattc_disconnect(uint16_t conn_handle);

typedef void (*gatt_mtu_cb_t)(uint16_t mtu);
void bt_gattc_exchanged_mtu(uint16_t conn_handle, gatt_mtu_cb_t cb, void *cb_arg);
typedef void (*gapc_evt_cb_t)(uint16_t conn_handle, uint16_t event_type);
int bt_gattc_conn_create(void *param, u8_t *addr, u8_t addr_type, gapc_evt_cb_t gapc_cb);
void bt_gatt_init(struct ble_gatt_svc_def *defs);

int bt_gatt_set_preferred_mtu(uint16_t value);
u16_t bt_gatt_get_preferred_mtu(void);

typedef void (*gaps_hal_evt_cb_t)(struct ble_gap_event *event);
struct ssv_bt_adv_param {
    /** Bit-field of advertising options */
    u8_t  options;

    /** Minimum Advertising Interval (N * 0.625) */
    u16_t interval_min;

    /** Maximum Advertising Interval (N * 0.625) */
    u16_t interval_max;
};

/** Description of different data types that can be encoded into
  * advertising data. Used to form arrays that are passed to the
  * bt_le_adv_start() function.
  */
struct ssv_bt_adv_data {
    u8_t type;
    u8_t data_len;
    const u8_t *data;
};
int bt_le_adv_enable(const struct ssv_bt_adv_param *param,
                    const struct ssv_bt_adv_data *ad, size_t ad_len,
                    const struct ssv_bt_adv_data *sd, size_t sd_len,
                    gaps_hal_evt_cb_t gaps_cb);
int bt_le_adv_disable(void);


uint16_t get_indication_handle();


typedef struct {
    u8_t type;
    u8_t val[6];
} ssv_bt_addr_t;

typedef void ssv_bt_scan_cb_t(const ssv_bt_addr_t *addr, s8_t rssi,
                               u8_t adv_type, u8_t *buf, u32_t len);

struct ssv_bt_scan_param {
    /** Scan type (BLE_MESH_SCAN_ACTIVE or BLE_MESH_SCAN_PASSIVE) */
    u8_t  type;

    /** Duplicate filtering (BLE_MESH_SCAN_FILTER_DUP_ENABLE or
     *  BLE_MESH_SCAN_FILTER_DUP_DISABLE)
     */
    u8_t  filter_dup;

    /** Scan interval (N * 0.625 ms) */
    u16_t interval;

    /** Scan window (N * 0.625 ms) */
    u16_t window;
};
int bt_le_scan_enable(const struct ssv_bt_scan_param *param, ssv_bt_scan_cb_t cb);

int bt_le_scan_disable(void);


int set_ad(const struct ssv_bt_adv_data *ad, size_t ad_len, u8_t *buf, u8_t *buf_len);

#endif /* _SSV_GATT_API_H_ */

