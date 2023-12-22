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

#ifndef _SSV_BLE_MESH_CONFIG_MODEL_API_H_
#define _SSV_BLE_MESH_CONFIG_MODEL_API_H_

#include "ssv_ble_mesh_defs.h"

/** @def    SSV_BLE_MESH_MODEL_CFG_SRV
 *
 *  @brief  Define a new Config Server Model.
 *
 *  @note   The Config Server Model can only be included by a Primary Element.
 *
 *  @param  srv_data Pointer to a unique Config Server Model user_data.
 *
 *  @return New Config Server Model instance.
 */
#define SSV_BLE_MESH_MODEL_CFG_SRV(srv_data)                              \
        SSV_BLE_MESH_SIG_MODEL(SSV_BLE_MESH_MODEL_ID_CONFIG_SRV,          \
                  NULL, NULL, srv_data)

/** @def    SSV_BLE_MESH_MODEL_CFG_CLI
 *
 *  @brief  Define a new Config Client Model.
 *
 *  @note   The Config Client Model can only be included by a Primary Element.
 *
 *  @param  cli_data Pointer to a unique struct ssv_ble_mesh_client_t.
 *
 *  @return New Config Client Model instance.
 */
#define SSV_BLE_MESH_MODEL_CFG_CLI(cli_data)                              \
        SSV_BLE_MESH_SIG_MODEL(SSV_BLE_MESH_MODEL_ID_CONFIG_CLI,          \
                  NULL, NULL, cli_data)

/** Configuration Server Model context */
typedef struct ssv_ble_mesh_cfg_srv {
    ssv_ble_mesh_model_t *model;    /*!< Pointer to Configuration Server Model */

    uint8_t net_transmit;           /*!< Network Transmit state */
    uint8_t relay;                  /*!< Relay Mode state */
    uint8_t relay_retransmit;       /*!< Relay Retransmit state */
    uint8_t beacon;                 /*!< Secure Network Beacon state */
    uint8_t gatt_proxy;             /*!< GATT Proxy state */
    uint8_t friend_state;           /*!< Friend state */
    uint8_t default_ttl;            /*!< Default TTL */

    /** Heartbeat Publication */
    struct {
        struct k_delayed_work timer;    /*!< Heartbeat Publication timer */

        uint16_t dst;                   /*!< Destination address for Heartbeat messages */
        uint16_t count;                 /*!< Number of Heartbeat messages to be sent */
        uint8_t  period;                /*!< Period for sending Heartbeat messages */
        uint8_t  ttl;                   /*!< TTL to be used when sending Heartbeat messages */
        uint16_t feature;               /*!< Bit field indicating features that trigger Heartbeat messages when changed */
        uint16_t net_idx;               /*!< NetKey Index used by Heartbeat Publication */
    } heartbeat_pub;

    /** Heartbeat Subscription */
    struct {
        int64_t  expiry;                /*!< Timestamp when Heartbeat subscription period is expired */

        uint16_t src;                   /*!< Source address for Heartbeat messages */
        uint16_t dst;                   /*!< Destination address for Heartbeat messages */
        uint16_t count;                 /*!< Number of Heartbeat messages received */
        uint8_t  min_hops;              /*!< Minimum hops when receiving Heartbeat messages */
        uint8_t  max_hops;              /*!< Maximum hops when receiving Heartbeat messages */

        /** Optional heartbeat subscription tracking function */
        ssv_ble_mesh_cb_t heartbeat_recv_cb;
    } heartbeat_sub;
} ssv_ble_mesh_cfg_srv_t;

/** Parameters of Config Composition Data Get. */
typedef struct {
    uint8_t page;                   /*!< Page number of the Composition Data. */
} ssv_ble_mesh_cfg_composition_data_get_t;

/** Parameters of Config Model Publication Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_pub_get_t;

/** Parameters of Config SIG Model Subscription Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
} ssv_ble_mesh_cfg_sig_model_sub_get_t;

/** Parameters of Config Vendor Model Subscription Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_vnd_model_sub_get_t;

/** Parameters of Config AppKey Get. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
} ssv_ble_mesh_cfg_app_key_get_t;

/** Parameters of Config Node Identity Get. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
} ssv_ble_mesh_cfg_node_identity_get_t;

/** Parameters of Config SIG Model App Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
} ssv_ble_mesh_cfg_sig_model_app_get_t;

/** Parameters of Config Vendor Model App Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_vnd_model_app_get_t;

/** Parameters of Config Key Refresh Phase Get. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
} ssv_ble_mesh_cfg_kr_phase_get_t;

/** Parameters of Config Low Power Node PollTimeout Get. */
typedef struct {
    uint16_t lpn_addr;              /*!< The unicast address of the Low Power node */
} ssv_ble_mesh_cfg_lpn_polltimeout_get_t;

/** Parameters of Config Beacon Set. */
typedef struct {
    uint8_t beacon;                 /*!< New Secure Network Beacon state */
} ssv_ble_mesh_cfg_beacon_set_t;

/** Parameters of Config Default TTL Set. */
typedef struct {
    uint8_t ttl;                    /*!< The default TTL state value */
} ssv_ble_mesh_cfg_default_ttl_set_t;

/** Parameters of Config Friend Set. */
typedef struct {
    uint8_t friend_state;           /*!< The friend state value */
} ssv_ble_mesh_cfg_friend_set_t;

/** Parameters of Config GATT Proxy Set. */
typedef struct {
    uint8_t gatt_proxy;             /*!< The GATT Proxy state value */
} ssv_ble_mesh_cfg_gatt_proxy_set_t;

/** Parameters of Config Relay Set. */
typedef struct {
    uint8_t relay;                  /*!< The relay value */
    uint8_t relay_retransmit;       /*!< The relay retransmit value */
} ssv_ble_mesh_cfg_relay_set_t;

/** Parameters of Config NetKey Add. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint8_t  net_key[16];           /*!< The network key value */
} ssv_ble_mesh_cfg_net_key_add_t;

/** Parameters of Config AppKey Add. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint16_t app_idx;               /*!< The app key index */
    uint8_t  app_key[16];           /*!< The app key value */
} ssv_ble_mesh_cfg_app_key_add_t;

/** Parameters of Config Model App Bind. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_app_idx;         /*!< Index of the app key to bind with the model */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_app_bind_t;

/** Parameters of Config Model Publication Set. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t publish_addr;          /*!< Value of the publish address */
    uint16_t publish_app_idx;       /*!< Index of the application key */
    bool     cred_flag;             /*!< Value of the Friendship Credential Flag */
    uint8_t  publish_ttl;           /*!< Default TTL value for the publishing messages */
    uint8_t  publish_period;        /*!< Period for periodic status publishing */
    uint8_t  publish_retransmit;    /*!< Number of retransmissions and number of 50-millisecond steps between retransmissions */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_pub_set_t;

/** Parameters of Config Model Subscription Add. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t sub_addr;              /*!< The address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_sub_add_t;

/** Parameters of Config Model Subscription Delete. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t sub_addr;              /*!< The address to be removed from the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_sub_delete_t;

/** Parameters of Config Model Subscription Overwrite. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t sub_addr;              /*!< The address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_sub_overwrite_t;

/** Parameters of Config Model Subscription Virtual Address Add. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< The Label UUID of the virtual address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_sub_va_add_t;

/** Parameters of Config Model Subscription Virtual Address Delete. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< The Label UUID of the virtual address to be removed from the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_sub_va_delete_t;

/** Parameters of Config Model Subscription Virtual Address Overwrite. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< The Label UUID of the virtual address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_sub_va_overwrite_t;

/** Parameters of Config Model Publication Virtual Address Set. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< Value of the Label UUID publish address */
    uint16_t publish_app_idx;       /*!< Index of the application key */
    bool     cred_flag;             /*!< Value of the Friendship Credential Flag */
    uint8_t  publish_ttl;           /*!< Default TTL value for the publishing messages */
    uint8_t  publish_period;        /*!< Period for periodic status publishing */
    uint8_t  publish_retransmit;    /*!< Number of retransmissions and number of 50-millisecond steps between retransmissions */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_pub_va_set_t;

/** Parameters of Config Model Subscription Delete All. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_sub_delete_all_t;

/** Parameters of Config NetKey Update. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint8_t  net_key[16];           /*!< The network key value */
} ssv_ble_mesh_cfg_net_key_update_t;

/** Parameters of Config NetKey Delete. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
} ssv_ble_mesh_cfg_net_key_delete_t;

/** Parameters of Config AppKey Update. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint16_t app_idx;               /*!< The app key index */
    uint8_t  app_key[16];           /*!< The app key value */
} ssv_ble_mesh_cfg_app_key_update_t;

/** Parameters of Config AppKey Delete. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint16_t app_idx;               /*!< The app key index */
} ssv_ble_mesh_cfg_app_key_delete_t;

/** Parameters of Config Node Identity Set. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint8_t  identity;              /*!< New Node Identity state */
} ssv_ble_mesh_cfg_node_identity_set_t;

/** Parameters of Config Model App Unbind. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_app_idx;         /*!< Index of the app key to bind with the model */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} ssv_ble_mesh_cfg_model_app_unbind_t;

/** Parameters of Config Key Refresh Phase Set. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint8_t  transition;            /*!< New Key Refresh Phase Transition */
} ssv_ble_mesh_cfg_kr_phase_set_t;

/** Parameters of Config Network Transmit Set. */
typedef struct {
    uint8_t net_transmit;           /*!< Network Transmit State */
} ssv_ble_mesh_cfg_net_transmit_set_t;

/** Parameters of Config Model Heartbeat Publication Set. */
typedef struct  {
    uint16_t dst;                   /*!< Destination address for Heartbeat messages */
    uint8_t  count;                 /*!< Number of Heartbeat messages to be sent */
    uint8_t  period;                /*!< Period for sending Heartbeat messages */
    uint8_t  ttl;                   /*!< TTL to be used when sending Heartbeat messages */
    uint16_t feature;               /*!< Bit field indicating features that trigger Heartbeat messages when changed */
    uint16_t net_idx;               /*!< NetKey Index */
} ssv_ble_mesh_cfg_heartbeat_pub_set_t;

/** Parameters of Config Model Heartbeat Subscription Set. */
typedef struct {
    uint16_t src;                   /*!< Source address for Heartbeat messages */
    uint16_t dst;                   /*!< Destination address for Heartbeat messages */
    uint8_t  period;                /*!< Period for receiving Heartbeat messages */
} ssv_ble_mesh_cfg_heartbeat_sub_set_t;

/**
 * @brief For SSV_BLE_MESH_MODEL_OP_BEACON_GET
 *            SSV_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET
 *            SSV_BLE_MESH_MODEL_OP_DEFAULT_TTL_GET
 *            SSV_BLE_MESH_MODEL_OP_GATT_PROXY_GET
 *            SSV_BLE_MESH_MODEL_OP_RELAY_GET
 *            SSV_BLE_MESH_MODEL_OP_MODEL_PUB_GET
 *            SSV_BLE_MESH_MODEL_OP_FRIEND_GET
 *            SSV_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_GET
 *            SSV_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_GET
 * the get_state parameter in the ssv_ble_mesh_config_client_get_state function should not be set to NULL.
 */
typedef union {
    ssv_ble_mesh_cfg_model_pub_get_t         model_pub_get;     /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_PUB_GET. */
    ssv_ble_mesh_cfg_composition_data_get_t  comp_data_get;     /*!< For SSV_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET. */
    ssv_ble_mesh_cfg_sig_model_sub_get_t     sig_model_sub_get; /*!< For SSV_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_GET */
    ssv_ble_mesh_cfg_vnd_model_sub_get_t     vnd_model_sub_get; /*!< For SSV_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_GET */
    ssv_ble_mesh_cfg_app_key_get_t           app_key_get;       /*!< For SSV_BLE_MESH_MODEL_OP_APP_KEY_GET. */
    ssv_ble_mesh_cfg_node_identity_get_t     node_identity_get; /*!< For SSV_BLE_MESH_MODEL_OP_NODE_IDENTITY_GET. */
    ssv_ble_mesh_cfg_sig_model_app_get_t     sig_model_app_get; /*!< For SSV_BLE_MESH_MODEL_OP_SIG_MODEL_APP_GET */
    ssv_ble_mesh_cfg_vnd_model_app_get_t     vnd_model_app_get; /*!< For SSV_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_GET */
    ssv_ble_mesh_cfg_kr_phase_get_t          kr_phase_get;      /*!< For SSV_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_GET */
    ssv_ble_mesh_cfg_lpn_polltimeout_get_t   lpn_pollto_get;    /*!< For SSV_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_GET */
} ssv_ble_mesh_cfg_client_get_state_t;

/**
 * @brief For SSV_BLE_MESH_MODEL_OP_BEACON_SET
 *            SSV_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET
 *            SSV_BLE_MESH_MODEL_OP_GATT_PROXY_SET
 *            SSV_BLE_MESH_MODEL_OP_RELAY_SET
 *            SSV_BLE_MESH_MODEL_OP_MODEL_PUB_SET
 *            SSV_BLE_MESH_MODEL_OP_MODEL_SUB_ADD
 *            SSV_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD
 *            SSV_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE
 *            SSV_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE
 *            SSV_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE
 *            SSV_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE
 *            SSV_BLE_MESH_MODEL_OP_NET_KEY_ADD
 *            SSV_BLE_MESH_MODEL_OP_APP_KEY_ADD
 *            SSV_BLE_MESH_MODEL_OP_MODEL_APP_BIND
 *            SSV_BLE_MESH_MODEL_OP_NODE_RESET
 *            SSV_BLE_MESH_MODEL_OP_FRIEND_SET
 *            SSV_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET
 *            SSV_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET
 * the set_state parameter in the ssv_ble_mesh_config_client_set_state function should not be set to NULL.
 */
typedef union {
    ssv_ble_mesh_cfg_beacon_set_t             beacon_set;             /*!< For SSV_BLE_MESH_MODEL_OP_BEACON_SET */
    ssv_ble_mesh_cfg_default_ttl_set_t        default_ttl_set;        /*!< For SSV_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET */
    ssv_ble_mesh_cfg_friend_set_t             friend_set;             /*!< For SSV_BLE_MESH_MODEL_OP_FRIEND_SET */
    ssv_ble_mesh_cfg_gatt_proxy_set_t         gatt_proxy_set;         /*!< For SSV_BLE_MESH_MODEL_OP_GATT_PROXY_SET */
    ssv_ble_mesh_cfg_relay_set_t              relay_set;              /*!< For SSV_BLE_MESH_MODEL_OP_RELAY_SET */
    ssv_ble_mesh_cfg_net_key_add_t            net_key_add;            /*!< For SSV_BLE_MESH_MODEL_OP_NET_KEY_ADD */
    ssv_ble_mesh_cfg_app_key_add_t            app_key_add;            /*!< For SSV_BLE_MESH_MODEL_OP_APP_KEY_ADD */
    ssv_ble_mesh_cfg_model_app_bind_t         model_app_bind;         /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_APP_BIND */
    ssv_ble_mesh_cfg_model_pub_set_t          model_pub_set;          /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_PUB_SET */
    ssv_ble_mesh_cfg_model_sub_add_t          model_sub_add;          /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_SUB_ADD */
    ssv_ble_mesh_cfg_model_sub_delete_t       model_sub_delete;       /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE */
    ssv_ble_mesh_cfg_model_sub_overwrite_t    model_sub_overwrite;    /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE */
    ssv_ble_mesh_cfg_model_sub_va_add_t       model_sub_va_add;       /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD */
    ssv_ble_mesh_cfg_model_sub_va_delete_t    model_sub_va_delete;    /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE */
    ssv_ble_mesh_cfg_model_sub_va_overwrite_t model_sub_va_overwrite; /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE */
    ssv_ble_mesh_cfg_heartbeat_pub_set_t      heartbeat_pub_set;      /*!< For SSV_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET */
    ssv_ble_mesh_cfg_heartbeat_sub_set_t      heartbeat_sub_set;      /*!< For SSV_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET */
    ssv_ble_mesh_cfg_model_pub_va_set_t       model_pub_va_set;       /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_PUB_VIRTUAL_ADDR_SET */
    ssv_ble_mesh_cfg_model_sub_delete_all_t   model_sub_delete_all;   /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL */
    ssv_ble_mesh_cfg_net_key_update_t         net_key_update;         /*!< For SSV_BLE_MESH_MODEL_OP_NET_KEY_UPDATE */
    ssv_ble_mesh_cfg_net_key_delete_t         net_key_delete;         /*!< For SSV_BLE_MESH_MODEL_OP_NET_KEY_DELETE */
    ssv_ble_mesh_cfg_app_key_update_t         app_key_update;         /*!< For SSV_BLE_MESH_MODEL_OP_APP_KEY_UPDATE */
    ssv_ble_mesh_cfg_app_key_delete_t         app_key_delete;         /*!< For SSV_BLE_MESH_MODEL_OP_APP_KEY_DELETE */
    ssv_ble_mesh_cfg_node_identity_set_t      node_identity_set;      /*!< For SSV_BLE_MESH_MODEL_OP_NODE_IDENTITY_SET */
    ssv_ble_mesh_cfg_model_app_unbind_t       model_app_unbind;       /*!< For SSV_BLE_MESH_MODEL_OP_MODEL_APP_UNBIND */
    ssv_ble_mesh_cfg_kr_phase_set_t           kr_phase_set;           /*!< For SSV_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_SET */
    ssv_ble_mesh_cfg_net_transmit_set_t       net_transmit_set;       /*!< For SSV_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_SET */
} ssv_ble_mesh_cfg_client_set_state_t;

/** Parameter of Config Beacon Status */
typedef struct {
    uint8_t beacon;                     /*!< Secure Network Beacon state value */
} ssv_ble_mesh_cfg_beacon_status_cb_t;

/** Parameters of Config Composition Data Status */
typedef struct {
    uint8_t page;                              /*!< Page number of the Composition Data */
    struct net_buf_simple *composition_data;   /*!< Pointer to Composition Data for the identified page */
} ssv_ble_mesh_cfg_comp_data_status_cb_t;

/** Parameter of Config Default TTL Status */
typedef struct {
    uint8_t default_ttl;                /*!< Default TTL state value */
} ssv_ble_mesh_cfg_default_ttl_status_cb_t;

/** Parameter of Config GATT Proxy Status */
typedef struct {
    uint8_t gatt_proxy;                 /*!< GATT Proxy state value */
} ssv_ble_mesh_cfg_gatt_proxy_status_cb_t;

/** Parameters of Config Relay Status */
typedef struct {
    uint8_t relay;                      /*!< Relay state value */
    uint8_t retransmit;                 /*!< Relay retransmit value(number of retransmissions and number of 10-millisecond steps between retransmissions) */
} ssv_ble_mesh_cfg_relay_status_cb_t;

/** Parameters of Config Model Publication Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t publish_addr;              /*!< Value of the publish address */
    uint16_t app_idx;                   /*!< Index of the application key */
    bool     cred_flag;                 /*!< Value of the Friendship Credential Flag */
    uint8_t  ttl;                       /*!< Default TTL value for the outgoing messages */
    uint8_t  period;                    /*!< Period for periodic status publishing */
    uint8_t  transmit;                  /*!< Number of retransmissions and number of 50-millisecond steps between retransmissions */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
} ssv_ble_mesh_cfg_model_pub_status_cb_t;

/** Parameters of Config Model Subscription Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t sub_addr;                  /*!< Value of the address */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
} ssv_ble_mesh_cfg_model_sub_status_cb_t;

/** Parameters of Config NetKey Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
} ssv_ble_mesh_cfg_net_key_status_cb_t;

/** Parameters of Config AppKey Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
    uint16_t app_idx;                   /*!< Index of the application key */
} ssv_ble_mesh_cfg_app_key_status_cb_t;

/** Parameters of Config Model App Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t app_idx;                   /*!< Index of the application key */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
} ssv_ble_mesh_cfg_mod_app_status_cb_t;

/** Parameter of Config Friend Status */
typedef struct {
    uint8_t friend_state;               /*!< Friend state value */
} ssv_ble_mesh_cfg_friend_status_cb_t;

/** Parameters of Config Heartbeat Publication Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t dst;                       /*!< Destination address for Heartbeat messages */
    uint8_t  count;                     /*!< Number of Heartbeat messages remaining to be sent */
    uint8_t  period;                    /*!< Period for sending Heartbeat messages */
    uint8_t  ttl;                       /*!< TTL to be used when sending Heartbeat messages */
    uint16_t features;                  /*!< Features that trigger Heartbeat messages when changed */
    uint16_t net_idx;                   /*!< Index of the NetKey */
} ssv_ble_mesh_cfg_hb_pub_status_cb_t;

/** Parameters of Config Heartbeat Subscription Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t src;                       /*!< Source address for Heartbeat messages */
    uint16_t dst;                       /*!< Destination address for Heartbeat messages */
    uint8_t  period;                    /*!< Remaining Period for processing Heartbeat messages */
    uint8_t  count;                     /*!< Number of Heartbeat messages received */
    uint8_t  min_hops;                  /*!< Minimum hops when receiving Heartbeat messages */
    uint8_t  max_hops;                  /*!< Maximum hops when receiving Heartbeat messages */
} ssv_ble_mesh_cfg_hb_sub_status_cb_t;

/** Parameters of Config Network Transmit Status */
typedef struct {
    uint8_t net_trans_count: 3;         /*!< Number of transmissions for each Network PDU originating from the node */
    uint8_t net_trans_step : 5;         /*!< Maximum hops when receiving Heartbeat messages */
} ssv_ble_mesh_cfg_net_trans_status_cb_t;

/** Parameters of Config SIG/Vendor Subscription List */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
    struct net_buf_simple *sub_addr;    /*!< A block of all addresses from the Subscription List */
} ssv_ble_mesh_cfg_model_sub_list_cb_t;

/** Parameter of Config NetKey List */
typedef struct {
    struct net_buf_simple *net_idx;     /*!< A list of NetKey Indexes known to the node */
} ssv_ble_mesh_cfg_net_key_list_cb_t;

/** Parameters of Config AppKey List */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< NetKey Index of the NetKey that the AppKeys are bound to */
    struct net_buf_simple *app_idx;     /*!< A list of AppKey indexes that are bound to the NetKey identified by NetKeyIndex */
} ssv_ble_mesh_cfg_app_key_list_cb_t;

/** Parameters of Config Node Identity Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
    uint8_t  identity;                  /*!< Node Identity state */
} ssv_ble_mesh_cfg_node_id_status_cb_t;

/** Parameters of Config SIG/Vendor Model App List */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
    struct net_buf_simple *app_idx;     /*!< All AppKey indexes bound to the Model */
} ssv_ble_mesh_cfg_model_app_list_cb_t;

/** Parameters of Config Key Refresh Phase Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
    uint8_t  phase;                     /*!< Key Refresh Phase state */
} ssv_ble_mesh_cfg_kr_phase_status_cb_t;

/** Parameters of Config Low Power Node PollTimeout Status */
typedef struct {
    uint16_t lpn_addr;                  /*!< The unicast address of the Low Power node */
    int32_t  poll_timeout;              /*!< The current value of the PollTimeout timer of the Low Power node */
} ssv_ble_mesh_cfg_lpn_pollto_status_cb_t;

/**
 * @brief Configuration Client Model received message union
 */
typedef union {
    ssv_ble_mesh_cfg_beacon_status_cb_t      beacon_status;         /*!< The beacon status value */
    ssv_ble_mesh_cfg_comp_data_status_cb_t   comp_data_status;      /*!< The composition data status value */
    ssv_ble_mesh_cfg_default_ttl_status_cb_t default_ttl_status;    /*!< The default_ttl status value */
    ssv_ble_mesh_cfg_gatt_proxy_status_cb_t  gatt_proxy_status;     /*!< The gatt_proxy status value */
    ssv_ble_mesh_cfg_relay_status_cb_t       relay_status;          /*!< The relay status value */
    ssv_ble_mesh_cfg_model_pub_status_cb_t   model_pub_status;      /*!< The model publication status value */
    ssv_ble_mesh_cfg_model_sub_status_cb_t   model_sub_status;      /*!< The model subscription status value */
    ssv_ble_mesh_cfg_net_key_status_cb_t     netkey_status;         /*!< The netkey status value */
    ssv_ble_mesh_cfg_app_key_status_cb_t     appkey_status;         /*!< The appkey status value */
    ssv_ble_mesh_cfg_mod_app_status_cb_t     model_app_status;      /*!< The model app status value */
    ssv_ble_mesh_cfg_friend_status_cb_t      friend_status;         /*!< The friend status value */
    ssv_ble_mesh_cfg_hb_pub_status_cb_t      heartbeat_pub_status;  /*!< The heartbeat publication status value */
    ssv_ble_mesh_cfg_hb_sub_status_cb_t      heartbeat_sub_status;  /*!< The heartbeat subscription status value */
    ssv_ble_mesh_cfg_net_trans_status_cb_t   net_transmit_status;   /*!< The network transmit status value */
    ssv_ble_mesh_cfg_model_sub_list_cb_t     model_sub_list;        /*!< The model subscription list value */
    ssv_ble_mesh_cfg_net_key_list_cb_t       netkey_list;           /*!< The network key index list value */
    ssv_ble_mesh_cfg_app_key_list_cb_t       appkey_list;           /*!< The application key index list value */
    ssv_ble_mesh_cfg_node_id_status_cb_t     node_identity_status;  /*!< The node identity status value */
    ssv_ble_mesh_cfg_model_app_list_cb_t     model_app_list;        /*!< The model application key index list value */
    ssv_ble_mesh_cfg_kr_phase_status_cb_t    kr_phase_status;       /*!< The key refresh phase status value */
    ssv_ble_mesh_cfg_lpn_pollto_status_cb_t  lpn_timeout_status;    /*!< The low power node poll timeout status value */
} ssv_ble_mesh_cfg_client_common_cb_param_t;

/** Configuration Client Model callback parameters */
typedef struct {
    int error_code;                                         /*!< Appropriate error code */
    ssv_ble_mesh_client_common_param_t       *params;       /*!< The client common parameters */
    ssv_ble_mesh_cfg_client_common_cb_param_t status_cb;    /*!< The config status message callback values */
} ssv_ble_mesh_cfg_client_cb_param_t;

/** This enum value is the event of Configuration Client Model */
typedef enum {
    SSV_BLE_MESH_CFG_CLIENT_GET_STATE_EVT,
    SSV_BLE_MESH_CFG_CLIENT_SET_STATE_EVT,
    SSV_BLE_MESH_CFG_CLIENT_PUBLISH_EVT,
    SSV_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT,
    SSV_BLE_MESH_CFG_CLIENT_EVT_MAX,
} ssv_ble_mesh_cfg_client_cb_event_t;

/**
 * @brief Configuration Server model related context.
 */

typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t pub_addr;          /*!< Publish Address */
    uint16_t app_idx;           /*!< AppKey Index */
    bool     cred_flag;         /*!< Friendship Credential Flag */
    uint8_t  pub_ttl;           /*!< Publish TTL */
    uint8_t  pub_period;        /*!< Publish Period */
    uint8_t  pub_retransmit;    /*!< Publish Retransmit */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} ssv_ble_mesh_state_change_cfg_mod_pub_set_t;

/** Parameters of Config Model Subscription Add */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t sub_addr;          /*!< Subscription Address */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} ssv_ble_mesh_state_change_cfg_model_sub_add_t;

/** Parameters of Config Model Subscription Delete */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t sub_addr;          /*!< Subscription Address */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} ssv_ble_mesh_state_change_cfg_model_sub_delete_t;

/** Parameters of Config NetKey Add */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint8_t  net_key[16];       /*!< NetKey */
} ssv_ble_mesh_state_change_cfg_netkey_add_t;

/** Parameters of Config NetKey Update */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint8_t  net_key[16];       /*!< NetKey */
} ssv_ble_mesh_state_change_cfg_netkey_update_t;

/** Parameter of Config NetKey Delete */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
} ssv_ble_mesh_state_change_cfg_netkey_delete_t;

/** Parameters of Config AppKey Add */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint16_t app_idx;           /*!< AppKey Index */
    uint8_t  app_key[16];       /*!< AppKey */
} ssv_ble_mesh_state_change_cfg_appkey_add_t;

/** Parameters of Config AppKey Update */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint16_t app_idx;           /*!< AppKey Index */
    uint8_t  app_key[16];       /*!< AppKey */
} ssv_ble_mesh_state_change_cfg_appkey_update_t;

/** Parameters of Config AppKey Delete */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint16_t app_idx;           /*!< AppKey Index */
} ssv_ble_mesh_state_change_cfg_appkey_delete_t;

/** Parameters of Config Model App Bind */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t app_idx;           /*!< AppKey Index */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} ssv_ble_mesh_state_change_cfg_model_app_bind_t;

/** Parameters of Config Model App Unbind */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t app_idx;           /*!< AppKey Index */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} ssv_ble_mesh_state_change_cfg_model_app_unbind_t;

/** Parameters of Config Key Refresh Phase Set */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint8_t  kr_phase;          /*!< New Key Refresh Phase Transition */
} ssv_ble_mesh_state_change_cfg_kr_phase_set_t;

/**
 * @brief Configuration Server model state change value union
 */
typedef union {
    /**
     * The recv_op in ctx can be used to decide which state is changed.
     */
    ssv_ble_mesh_state_change_cfg_mod_pub_set_t         mod_pub_set;        /*!< Config Model Publication Set */
    ssv_ble_mesh_state_change_cfg_model_sub_add_t       mod_sub_add;        /*!< Config Model Subscription Add */
    ssv_ble_mesh_state_change_cfg_model_sub_delete_t    mod_sub_delete;     /*!< Config Model Subscription Delete */
    ssv_ble_mesh_state_change_cfg_netkey_add_t          netkey_add;         /*!< Config NetKey Add */
    ssv_ble_mesh_state_change_cfg_netkey_update_t       netkey_update;      /*!< Config NetKey Update */
    ssv_ble_mesh_state_change_cfg_netkey_delete_t       netkey_delete;      /*!< Config NetKey Delete */
    ssv_ble_mesh_state_change_cfg_appkey_add_t          appkey_add;         /*!< Config AppKey Add */
    ssv_ble_mesh_state_change_cfg_appkey_update_t       appkey_update;      /*!< Config AppKey Update */
    ssv_ble_mesh_state_change_cfg_appkey_delete_t       appkey_delete;      /*!< Config AppKey Delete */
    ssv_ble_mesh_state_change_cfg_model_app_bind_t      mod_app_bind;       /*!< Config Model App Bind */
    ssv_ble_mesh_state_change_cfg_model_app_unbind_t    mod_app_unbind;     /*!< Config Model App Unbind */
    ssv_ble_mesh_state_change_cfg_kr_phase_set_t        kr_phase_set;       /*!< Config Key Refresh Phase Set */
} ssv_ble_mesh_cfg_server_state_change_t;

/**
 * @brief Configuration Server model callback value union
 */
typedef union {
    ssv_ble_mesh_cfg_server_state_change_t state_change;  /*!< SSV_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT */
} ssv_ble_mesh_cfg_server_cb_value_t;

/** Configuration Server model callback parameters */
typedef struct {
    ssv_ble_mesh_model_t  *model;   /*!< Pointer to the server model structure */
    ssv_ble_mesh_msg_ctx_t ctx;     /*!< Context of the received message */
    ssv_ble_mesh_cfg_server_cb_value_t value;   /*!< Value of the received configuration messages */
} ssv_ble_mesh_cfg_server_cb_param_t;

/** This enum value is the event of Configuration Server model */
typedef enum {
    SSV_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT,
    SSV_BLE_MESH_CFG_SERVER_EVT_MAX,
} ssv_ble_mesh_cfg_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Config Client and Server Model functions.
 */

/**
 * @brief   Configuration Client Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* ssv_ble_mesh_cfg_client_cb_t)(ssv_ble_mesh_cfg_client_cb_event_t event,
        ssv_ble_mesh_cfg_client_cb_param_t *param);

/**
 * @brief   Configuration Server Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* ssv_ble_mesh_cfg_server_cb_t)(ssv_ble_mesh_cfg_server_cb_event_t event,
        ssv_ble_mesh_cfg_server_cb_param_t *param);

/**
 * @brief         Register BLE Mesh Config Client Model callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        SSV_BM_OK on success or error code otherwise.
 *
 */
ssv_err_t ssv_ble_mesh_register_config_client_callback(ssv_ble_mesh_cfg_client_cb_t callback);

/**
 * @brief         Register BLE Mesh Config Server Model callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        SSV_BM_OK on success or error code otherwise.
 *
 */
ssv_err_t ssv_ble_mesh_register_config_server_callback(ssv_ble_mesh_cfg_server_cb_t callback);

/**
 * @brief         Get the value of Config Server Model states using the Config Client Model get messages.
 *
 * @note          If you want to find the opcodes and corresponding meanings accepted by this API,
 *                please refer to ssv_ble_mesh_opcode_config_client_get_t in ssv_ble_mesh_defs.h
 *
 * @param[in]     params:    Pointer to BLE Mesh common client parameters.
 * @param[in]     get_state: Pointer to a union, each kind of opcode corresponds to one structure inside.
 *                           Shall not be set to NULL.
 *
 * @return        SSV_BM_OK on success or error code otherwise.
 *
 */
ssv_err_t ssv_ble_mesh_config_client_get_state(ssv_ble_mesh_client_common_param_t *params,
        ssv_ble_mesh_cfg_client_get_state_t *get_state);

/**
 * @brief         Set the value of the Configuration Server Model states using the Config Client Model set messages.
 *
 * @note          If you want to find the opcodes and corresponding meanings accepted by this API,
 *                please refer to ssv_ble_mesh_opcode_config_client_set_t in ssv_ble_mesh_defs.h
 *
 * @param[in]     params:    Pointer to BLE Mesh common client parameters.
 * @param[in]     set_state: Pointer to a union, each kind of opcode corresponds to one structure inside.
 *                           Shall not be set to NULL.
 *
 * @return        SSV_BM_OK on success or error code otherwise.
 *
 */
ssv_err_t ssv_ble_mesh_config_client_set_state(ssv_ble_mesh_client_common_param_t *params,
        ssv_ble_mesh_cfg_client_set_state_t *set_state);

#endif /** _SSV_BLE_MESH_CONFIG_MODEL_API_H_ */

