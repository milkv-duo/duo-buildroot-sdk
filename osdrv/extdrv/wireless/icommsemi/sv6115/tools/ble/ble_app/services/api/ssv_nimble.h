/*
 * Copyright (c) 2020 iComm-semi Ltd.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @file ssv_ble_def.h
 * @brief Common defines and declarations for host driver for all platforms.
 */


#ifndef __SSV_BLE_DEF_H__
#define __SSV_BLE_DEF_H__
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 *         Include Files
 ******************************************************************************/


/*******************************************************************************
 *         Defines
 ******************************************************************************/
#ifndef _SSV_TYPES_H_
//SSV PACK Definition
#define SSV_PACKED_STRUCT_BEGIN
#define SSV_PACKED_STRUCT               //__attribute__ ((packed))
#define SSV_PACKED_STRUCT_END           //__attribute__((packed))
#define SSV_PACKED_STRUCT_STRUCT        __attribute__ ((packed))
#define SSV_PACKED_STRUCT_FIELD(x)      x
#endif

#if __WORDSIZE == 32
    #define ptr_32bits      void*
#else
    #define ptr_32bits      uint32_t
    #define ptr_dummy       (0x55aa)
#endif

/*******************************************************************************
 *         Enumerations
 ******************************************************************************/

#define NIMBLE_CMD_HDR_LEN            ((size_t)(((ST_NIMBLE_CMD *)0)->data))
#define NIMBLE_EVT_HDR_LEN            ((size_t)(((ST_NIMBLE_EVT *)0)->data))

typedef enum {
    E_SSV_HAL_BLE_COMMON_INIT                       = 0,
    E_SSV_HAL_BLE_GAP_INIT                          = 1,
    E_SSV_HAL_BLE_GATTS_INIT                        = 2,
    E_SSV_HAL_BLE_GATTC_INIT                        = 3,

    E_SSV_HAL_BLE_GATTS_SET_PROFILE_CB              = 4,
    E_SSV_HAL_BLE_GAP_SET_EVENT_CB                  = 5,
    E_SSV_BLE_GATT_SET_LOCAL_MTU                    = 6,

    E_SSV_HAL_BLE_COMMON_DEINIT                     = 7,
    E_SSV_HAL_BLE_GAP_DEINIT                        = 8,
    E_SSV_HAL_BLE_GATTS_DEINIT                      = 9,
    E_SSV_HAL_BLE_GATTC_DEINIT                      = 10,

    E_SSV_HAL_BLE_GAP_CONFIG_ADV_DATA_RAW           = 11,
    E_SSV_HAL_BLE_GAP_CONFIG_ADV_DATA               = 12,
    E_SSV_HAL_BLE_GAP_CONFIG_SCAN_RSP_DATA          = 13,
    E_SSV_HAL_BLE_GAP_START_ADVERTISING             = 14,
    E_SSV_HAL_BLE_GAP_STOP_ADVERTISING              = 15,

    E_SSV_HAL_BLE_GAP_SET_SCAN_PARAMS               = 16,
    E_SSV_HAL_BLE_GAP_START_SCANNING                = 17,
    E_SSV_HAL_BLE_GAP_STOP_SCANNING                 = 18,

    E_SSV_HAL_BLE_GATTS_CREATE_SERVICE_DB           = 19,
    E_SSV_HAL_BLE_GATTS_SEND_INDICATION             = 20,

    E_SSV_HAL_BLE_GAP_DISCONNECT                    = 21,
    E_SSV_HAL_BLE_GATTS_SEND_RESPONSE               = 22,

    E_SSV_BLE_API_MAX                               ,
}SSV_NIMBLE_CMD;

/*typedef enum {
    E_SSV_GAP_BLE_SCAN_RESULT_EVT                       = 0,
    E_SSV_GATTS_CONNECT_EVT                             = 1,    
    E_SSV_BLE_APIS_EVT_MAX                               ,
}SSV_NIMBLE_EVT;*/

#define E_SSV_GATTS_EVT_HOST_FLAG   0x10000 //must > 16bits Number
#define E_SSV_GAP_EVT_HOST_FLAG     0x00000

/*******************************************************************************
 *         Structures
 ******************************************************************************/
/// Structure of SSV control driver command. .
SSV_PACKED_STRUCT_BEGIN
typedef struct st_nimble_cmd
{
    unsigned int cmdid;
    unsigned int datalen;
    unsigned char data[0];
}SSV_PACKED_STRUCT_STRUCT ST_NIMBLE_CMD;
SSV_PACKED_STRUCT_END

/// Structure of SSV control driver event.
SSV_PACKED_STRUCT_BEGIN
typedef struct st_nimble_evt
{
    unsigned int evtid;
    unsigned int datalen;
    unsigned char data[0];
}SSV_PACKED_STRUCT_STRUCT ST_NIMBLE_EVT;
SSV_PACKED_STRUCT_END

SSV_PACKED_STRUCT_BEGIN
typedef struct st_nimble_set_local_mtu
{
    unsigned int mtu;
}SSV_PACKED_STRUCT_STRUCT ST_SSV_NIMBLE_SET_LOCAL_MTU;
SSV_PACKED_STRUCT_END


#ifdef __cplusplus
}
#endif
#endif /* __SSV_BLE_DEF_H__ */
