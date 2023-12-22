#ifndef _TEST_DATA_H_
#define _TEST_DATA_H_
#include "ssv_ble_mesh_defs.h"
#include "ssv_ble_mesh_generic_model_api.h"

#define SSV_MESH_TEST_MAX_SENDER 10

#define ONOFF_GROUP_ADDR0 0xc000 /*onoff subscribe group 0 addr*/
#define ONOFF_GROUP_ADDR1 0xc001 /*onoff subscribe group 1 addr*/
#define ONOFF_GROUP_ADDR2 0xc002 /*onoff subscribe group 2 addr*/
#define ONOFF_GROUP_ADDR3 0xc003 /*onoff subscribe group 3 addr*/
#define ONOFF_GROUP_ADDR4 0xc004 /*onoff subscribe group 4 addr*/



#define VENDOR_GROUP_ADDR0 0xc010 /*vendor subscribe group 0 addr*/
#define VENDOR_GROUP_ADDR1 0xc011 /*vendor subscribe group 1 addr*/
#define VENDOR_GROUP_ADDR2 0xc012 /*vendor subscribe group 2 addr*/
#define VENDOR_GROUP_ADDR3 0xc013 /*vendor subscribe group 3 addr*/
#define VENDOR_GROUP_ADDR4 0xc014 /*vendor subscribe group 4 addr*/

#define SEND_INTERVAL_500MS 5
#define SEND_INTERVAL_1200MS 12
#define SEND_INTERVAL_2400MS 24


#define SEND_INTERVAL_1S 10

#define SEND_TEST_MESSAGE_TIMEOUT (5000/portTICK_RATE_MS)

#define BLE_MESH_TEST_VDNDOR_MODEL_LENGTH 64
#define BLE_MESH_TEST_VDNDOR_MODEL_ACK_LENGTH 20

typedef struct ssv_ble_mesh_test_sender ssv_ble_mesh_test_sender_t;
typedef struct ssv_ble_mesh_test_item ssv_ble_mesh_test_item_t;
typedef struct ssv_ble_mesh_test_collect_result_addr ssv_ble_mesh_test_collect_result_addr_t;
typedef struct ssv_ble_mesh_test_collect_result_group ssv_ble_mesh_test_collect_result_group_t;


struct ssv_ble_mesh_test_sender {
    /** sender address */
    uint8_t addr;

    /** on off opcode or vendor model 64 byte */
    uint8_t is_on_off:1;

    /** this msg require ack or not */
    uint8_t need_ack:1;

    /** msg send interval, unit is 100ms */
    uint8_t send_interval:6;

    /** send to subscribe group addr */
    uint16_t groups;

    /** sender expect how many status will receive */
    uint8_t expect_ack_count;
};

struct ssv_ble_mesh_test_collect_result_addr {
    /** start collect address */
    uint8_t start;

    /** end collect address*/
    uint8_t end;
};

struct ssv_ble_mesh_test_collect_result_group {
    ssv_ble_mesh_test_collect_result_addr_t *addr;
    uint8_t count;
};


struct ssv_ble_mesh_test_item {
    /** receiver expect msg count for no ack msg*/
    uint8_t receiver_expect_msg_count;

     /** count of sender address */
    uint8_t senders_count;

    /** sender address */
    ssv_ble_mesh_test_sender_t *senders;

    /** collectresult address */
    ssv_ble_mesh_test_collect_result_group_t *collect_group;
};

struct ssv_ble_mesh_test_thread_parameter {
    uint32_t test_count;
    uint32_t sbgr;/*sleep before get reult*/
    uint16_t test_interval;
    uint8_t sender_index;
    uint8_t test_item;
    uint8_t sender_count;
};



typedef enum {
    VENDOR_MODEL_TEST,
    ONOFF_MODEL_TEST,
} ble_mesh_test_model_flag_t;



/*typedef enum {
    TEST_OPCODE_START_TEST,
    TEST_OPCODE_START_TEST_ACK,
    TEST_OPCODE_STOP_TEST,
    TEST_OPCODE_GET_RESULT,
    TEST_OPCODE_RESULT_RETURN,
} ble_mesh_test_opcode_flag_t;*/

/*test item, test count(unit 100) 2 byte(0xAB), end sleep time*/
typedef enum {
    //TEST_OPCODE_START_OPCODE,
    TEST_START_TEST_ITEM,
    TEST_START_TEST_COUNT_A,
    TEST_START_TEST_COUNT_B,
    TEST_START_INTERVAL,
    TEST_START_SBGR,//sleep before get result
    TEST_START_SCAN_WIN,
    TEST_START_SCAN_INTERVAL,
    TEST_START_END,
} ble_mesh_test_opcode_start_location_flag_t;
/*
typedef enum {
    TEST_OPCODE_STOP_OPCODE,
    TEST_OPCODE_STOP_END,
} ble_mesh_test_opcode_stop_location_flag_t;

typedef enum {
    TEST_OPCODE_GETRESULT_OPCODE,
    TEST_OPCODE_GETRESULT_END,
} ble_mesh_test_opcode_get_result_location_flag_t;
*/

typedef enum {
    TEST_RESULT_RETUEN_ACK_COUNT_A,//device under test addr, ack msg's device
    TEST_RESULT_RETUEN_ACK_COUNT_B,
    TEST_RESULT_RETUEN_ACK_ADDR,/* device under test's addr*/
    TEST_RESULT_RETUEN_END,
} ble_mesh_test_opcode_result_return_location_flag_t;


typedef enum {
    NO_ACK,
    ACK,
} ble_mesh_test_ack_flag_t;


#define SSV_BLE_MESH_TEST_SENDER(_addr, _is_on_off_model, _need_ack, _send_interval, _groups, _ack_count) \
{                                                                       \
    .addr = _addr,                                       \
    .is_on_off = _is_on_off_model,                                              \
    .need_ack = _need_ack,                                                          \
    .send_interval = _send_interval,                              \
    .groups = _groups,                                                        \
    .expect_ack_count = _ack_count,                                                        \
}

#define SSV_BLE_MESH_TEST_COLLECT_RESULT_ADDR(_start, _end) \
{                                                                       \
    .start = _start,                                       \
    .end = _end,                                              \
}

#define SSV_BLE_MESH_TEST_COLLECT_RESULT_GROUP(_addr) \
    .addr = _addr,                                       \
    .count = ARRAY_SIZE(_addr),


#define SSV_BLE_MESH_TEST_TEST_ITEM(__receiver_expect_msg_count, _sender, _result_group) \
{                                                                       \
    .receiver_expect_msg_count = __receiver_expect_msg_count,    \
    .senders_count = ARRAY_SIZE(_sender),   \
    .senders = _sender,                 \
    .collect_group = &_result_group,             \
}

#define GENON_OFF_RECEIVE_ACK (1 << 0)
#define VENDOR_RECEIVE_ACK    (1 << 1)
#define VENDOR_RECEIVE       (1 << 2)
#define MSG_TIMEOUT_EVT       (1 << 3)
#define MSG_FAIL_EVT          (1 << 4)
#define GET_TEST_RESULT       (1 << 5)

#define MESH_TEST_EVENTS  (GENON_OFF_RECEIVE_ACK | VENDOR_RECEIVE_ACK | GET_TEST_RESULT | MSG_TIMEOUT_EVT)

void ble_mesh_test_clean_msg_count();
uint16_t ble_mesh_test_get_msg_count();
void ble_mesh_test_add_msg_count();

u8 ble_mesh_test_get_item_ack_count(void);
u8 ble_mesh_test_get_ack_count_remain(void);
void ble_mesh_test_set_expect_ack_count(u8 ack_count);
void ble_mesh_test_decrease_expect_ack_count(u16 type);

void ble_mesh_test_event_set_bit(u16 bit);
uint8_t get_publish_ack_count(void);
void add_publish_ack_count(uint16_t remote_addr);
void set_publish_ack_count(uint8_t ack_count);
uint8_t ble_mesh_do_test_item(u8_t *msg_data, uint16_t length);
void ble_mesh_test_get_result_return(uint16_t ack_addr, u8_t *msg_data, uint16_t length);
void ssv_handle_gen_onoff_msg(ssv_ble_mesh_model_t *model,
    ssv_ble_mesh_msg_ctx_t *ctx, ssv_ble_mesh_server_recv_gen_onoff_set_t *set);

#endif
