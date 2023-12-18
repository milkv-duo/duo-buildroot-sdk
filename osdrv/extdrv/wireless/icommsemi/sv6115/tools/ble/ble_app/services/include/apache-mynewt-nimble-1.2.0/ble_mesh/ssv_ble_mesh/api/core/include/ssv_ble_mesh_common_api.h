// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SSV_BLE_MESH_COMMON_API_H_
#define _SSV_BLE_MESH_COMMON_API_H_

#include "ssv_ble_mesh_defs.h"

/**
 * @brief         Initialize BLE Mesh module.
 *                This API initializes provisioning capabilities and composition data information.
 *
 * @note          After calling this API, the device needs to call ssv_ble_mesh_prov_enable()
 *                to enable provisioning functionality again.
 *
 * @param[in]     prov: Pointer to the device provisioning capabilities. This pointer must
 *                      remain valid during the lifetime of the BLE Mesh device.
 * @param[in]     comp: Pointer to the device composition data information. This pointer
 *                      must remain valid during the lifetime of the BLE Mesh device.
 *
 * @return        SSV_BM_OK on success or error code otherwise.
 *
 */
ssv_err_t ssv_ble_mesh_init(ssv_ble_mesh_prov_t *prov, ssv_ble_mesh_comp_t *comp);
ssv_err_t ssv_ble_mesh_config_dbg_gpio(u32 test_start_end, u32 test_fail, u32 syncadv_start, u32 ack_start_end);
#endif /* _SSV_BLE_MESH_COMMON_API_H_ */
