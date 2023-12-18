/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADV_H_
#define _ADV_H_

#include "mesh_bearer_adapt.h"
#include "ssv_ble_mesh_defs.h"

/* Convert from ms to 0.625ms units */
#define ADV_SCAN_UNIT(_ms) ((_ms) * 8 / 5)

/* SSV Maximum advertising data payload for a single data type */
#define BLE_MESH_SSV_ADV_DATA_SIZE 40


/* Maximum advertising data payload for a single data type */
#define BLE_MESH_ADV_DATA_SIZE 29

/* The user data is a pointer (4 bytes) to struct bt_mesh_adv */
#define BLE_MESH_ADV_USER_DATA_SIZE 4

#define BLE_MESH_ADV(buf) (*(struct bt_mesh_adv **)net_buf_user_data(buf))

typedef struct bt_mesh_msg {
    bool  relay;        /* Flag indicates if the packet is a relayed one */
    void *arg;          /* Pointer to the struct net_buf */
    u16_t src;          /* Source address for relay packets */
    u16_t dst;          /* Destination address for relay packets */
    u32_t timestamp;    /* Timestamp recorded when the relay packet is posted to queue */
} bt_mesh_msg_t;

enum bt_mesh_adv_type {
    BLE_MESH_ADV_PROV,
    BLE_MESH_ADV_DATA,
    BLE_MESH_ADV_BEACON,
    BLE_MESH_ADV_URI,
    BLE_MESH_SSV_SYNCED_ACK,
};

typedef void (*bt_mesh_adv_func_t)(struct net_buf *buf, u16_t duration,
                                   int err, void *user_data);

struct bt_mesh_adv {
    const struct bt_mesh_send_cb *cb;
    void *cb_data;

    u8_t      type: 3,
              busy: 1;
    u8_t      xmit;

    union {
        /* Address, used e.g. for Friend Queue messages */
        u16_t addr;

        /* For transport layer segment sending */
        struct {
            u8_t attempts;
        } seg;
    };
};


#define NUM_TX_NODE 10
struct bt_mesh_ssv_sync_ack {
    u8_t  acked;
    u16_t from;
    u32_t seq;
    struct ble_mesh_sync_bitmap bitmap;
};


typedef struct bt_mesh_adv *(*bt_mesh_adv_alloc_t)(int id);

/* xmit_count: Number of retransmissions, i.e. 0 == 1 transmission */
struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, u8_t xmit,
                                   s32_t timeout);

typedef enum {
    BLE_MESH_BUF_REF_EQUAL,
    BLE_MESH_BUF_REF_SMALL,
    BLE_MESH_BUF_REF_MAX,
} bt_mesh_buf_ref_flag_t;

void bt_mesh_adv_buf_ref_debug(const char *func, struct net_buf *buf,
                               u8_t ref_cmp, bt_mesh_buf_ref_flag_t flag);

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool,
        bt_mesh_adv_alloc_t get_id,
        enum bt_mesh_adv_type type,
        u8_t xmit, s32_t timeout);

void bt_mesh_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                      void *cb_data);

const bt_mesh_addr_t *bt_mesh_pba_get_addr(void);

struct net_buf *bt_mesh_relay_adv_create(enum bt_mesh_adv_type type, u8_t xmit,
        s32_t timeout);

void bt_mesh_relay_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
                            void *cb_data, u16_t src, u16_t dst);

u16_t bt_mesh_get_stored_relay_count(void);

u16_t bt_mesh_get_stored_adv_count(void);/*ssv++*/

void bt_mesh_adv_update(void);

void bt_mesh_adv_init(void);

int bt_mesh_scan_enable(void);

int bt_mesh_scan_disable(void);

void save_ssv_ack_info(u16_t from, u32_t seq, u32_t bitmap_low, u32_t bitmap_high);
void get_ssv_ack_bitmap(struct ble_mesh_sync_bitmap *bitmap, u16_t from);
void clean_ssv_ack_info(u16_t from);
void init_sync_adv(void);
u8_t golden_bitmap_valid(void);
u8_t golden_bitmap_match(void);
u8_t is_bitmap_the_same(struct ble_mesh_sync_bitmap a, struct ble_mesh_sync_bitmap b);
void assign_bitmap(struct ble_mesh_sync_bitmap *a, struct ble_mesh_sync_bitmap b);
void or_bitmap(struct ble_mesh_sync_bitmap *a, struct ble_mesh_sync_bitmap b);
u8_t get_shift_bit(u16_t addr, u8_t *shift_bit, u8_t *low);
#endif /* _ADV_H_ */
