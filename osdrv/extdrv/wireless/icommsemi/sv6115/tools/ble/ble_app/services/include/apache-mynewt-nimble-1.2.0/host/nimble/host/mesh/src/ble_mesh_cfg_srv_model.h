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

#include "ble_mesh_adapter.h"


#ifndef _BLE_MESH_CFG_SRV_MODEL_H_
#define _BLE_MESH_CFG_SRV_MODEL_H_

#include "ssv_ble_mesh_defs.h"
#include "ssv_ble_mesh_config_model_api.h"
#include "ssv_ble_mesh_health_model_api.h"
#include "health_srv.h"

#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
#include "ssv_ble_mesh_generic_model_api.h"
#endif //CONFIG_BLE_MESH_GENERIC_ONOFF_CLI

#define NODE_MAX_GROUP_CONFIG 3
#define CID_SSV 0x039B

extern uint8_t dev_uuid[16];

typedef struct {
    uint16_t net_idx;
    uint16_t unicast_addr;
} ble_mesh_node_config_params;
ble_mesh_node_config_params ble_mesh_node_prestore_params[NODE_MAX_GROUP_CONFIG];

extern ssv_ble_mesh_prov_t prov;

extern ssv_ble_mesh_model_pub_t vendor_model_pub_config;

// configure server module
extern ssv_ble_mesh_cfg_srv_t cfg_srv;

extern ssv_ble_mesh_model_t config_server_models[];

extern ssv_ble_mesh_elem_t config_server_elements[];

extern ssv_ble_mesh_comp_t config_server_comp;

// config client model
ssv_ble_mesh_client_t cfg_cli;
extern ssv_ble_mesh_model_t config_client_models[];

extern ssv_ble_mesh_elem_t config_client_elements[];

extern ssv_ble_mesh_comp_t config_client_comp;

// configure special module
extern ssv_ble_mesh_model_op_t gen_onoff_srv_model_op_config[];

extern ssv_ble_mesh_model_t gen_onoff_srv_models[];

extern ssv_ble_mesh_elem_t gen_onoff_srv_elements[];

extern ssv_ble_mesh_comp_t gen_onoff_srv_comp;

// config generic onoff client
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)

extern ssv_ble_mesh_client_t gen_onoff_cli;

extern ssv_ble_mesh_model_t gen_onoff_cli_models[];

extern ssv_ble_mesh_elem_t gen_onoff_cli_elements[];

extern ssv_ble_mesh_comp_t gen_onoff_cli_comp;
#endif //CONFIG_BLE_MESH_GENERIC_ONOFF_CLI

//CONFIG VENDOR MODEL TEST PERFORMANCE
#define SSV_BLE_MESH_VND_MODEL_ID_TEST_PERF_SRV 0x2000
#define SSV_BLE_MESH_VND_MODEL_ID_TEST_PERF_CLI 0x2001

#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_GET                 SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_GET, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET                 SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_SET, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET_UNACK           SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_SET_UNACK, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS              SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_STATUS, CID_SSV)
//++ssv -->
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_ITEM_SET           SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_ITEM_SET, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_ITEM_SET_STATUS   SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_ITEM_SET_STATUS, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_ITEM_STOP           SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_ITEM_STOP, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_ITEM_STOP_STATUS   SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_ITEM_STOP_STATUS, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_TEST               SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_TEST, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_TEST_UNACK        SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_TEST_UNACK, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_TEST_STATUS       SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_TEST_STATUS, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_TEST_RESULT         SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_TEST_RESULT, CID_SSV)
#define SSV_BLE_MESH_VND_MODEL_OP_TEST_PERF_TEST_RESULT_STATUS SSV_BLE_MESH_MODEL_OP_3(VENDOR_MODEL_PERF_OPERATION_TYPE_TEST_RESULT_STATUS, CID_SSV)
//++ssv <--

extern ssv_ble_mesh_client_t test_perf_cli;

extern ssv_ble_mesh_model_op_t test_perf_srv_op[];

extern ssv_ble_mesh_model_op_t test_perf_cli_op[];

extern ssv_ble_mesh_model_t config_models[];

extern ssv_ble_mesh_model_t test_perf_cli_models[];

extern ssv_ble_mesh_model_t gen_perf_srvcli_models[];//++ssv

extern ssv_ble_mesh_elem_t test_perf_cli_elements[];

extern ssv_ble_mesh_comp_t test_perf_cli_comp;

extern ssv_ble_mesh_model_t test_perf_srv_models[];

extern ssv_ble_mesh_elem_t test_perf_srv_elements[];

extern ssv_ble_mesh_comp_t test_perf_srv_comp;

#endif //_BLE_MESH_CFG_SRV_MODEL_H_

