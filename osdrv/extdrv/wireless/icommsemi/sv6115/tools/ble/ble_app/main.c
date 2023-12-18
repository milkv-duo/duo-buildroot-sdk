#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/signal.h>

#include <ssv_ble_services.h>
#include <ssv_nimble.h>
#include <ssv_hal_ble_gap.h>
#include <ssv_hal_ble_gatts.h>
#include <ssv_hal_ble_common.h>
#include <ssv_gatt_common_api.h>

const static const char *ssv_gatts_evt_name[] = {
    "REG", //SSV_GATTS_REG_EVT = 0
    "READ", //SSV_GATTS_READ_EVT = 1
    "WRITE", //SSV_GATTS_WRITE_EVT = 2
    "EXEC_WRITE", //SSV_GATTS_EXEC_WRITE_EVT= 3
    "MTU", //SSV_GATTS_MTU_EVT = 4
    "CONF", //SSV_GATTS_CONF_EVT = 5
    "UNREG", //SSV_GATTS_UNREG_EVT = 6
    "CREATE", //SSV_GATTS_CREATE_EVT = 7
    "ADD_INCL_SRVC", //SSV_GATTS_ADD_INCL_SRVC_EVT = 8
    "ADD_CHAR", //SSV_GATTS_ADD_CHAR_EVT = 9
    "ADD_CHAR_DESCR", //SSV_GATTS_ADD_CHAR_DESCR_EVT = 10
    "DELETE", //SSV_GATTS_DELETE_EVT = 11
    "START", //SSV_GATTS_START_EVT = 12
    "STOP", //SSV_GATTS_STOP_EVT = 13
    "CONNECT", //SSV_GATTS_CONNECT_EVT = 14
    "DISCONNECT", //SSV_GATTS_DISCONNECT_EVT = 15
    "OPEN", //SSV_GATTS_OPEN_EVT = 16
    "CANCEL_OPEN", //SSV_GATTS_CANCEL_OPEN_EVT = 17
    "CLOSE", //SSV_GATTS_CLOSE_EVT= 18
    "LISTEN", //SSV_GATTS_LISTEN_EVT = 19
    "CONGEST", //SSV_GATTS_CONGEST_EVT = 20
    "RESPONSE", //SSV_GATTS_RESPONSE_EVT = 21
    "CREAT_ATTR_TAB", //SSV_GATTS_CREAT_ATTR_TAB_EVT = 22
    "SET_ATTR_VAL", //SSV_GATTS_SET_ATTR_VAL_EVT = 23
    "SEND_SERVICE_CHANGE" //SSV_GATTS_SEND_SERVICE_CHANGE_EVT = 24
};

const static const char *ssv_gap_evt_name[] = {
    "ADV_DATA_SET_COMPLETE", //SSV_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT = 0
    "SCAN_RSP_DATA_SET_COMPLETE", //SSV_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT
    "SCAN_PARAM_SET_COMPLETE", //SSV_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT
    "SCAN_RESULT", //SSV_GAP_BLE_SCAN_RESULT_EVT
    "ADV_DATA_RAW_SET_COMPLETE", //SSV_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT
    "SCAN_RSP_DATA_RAW_SET_COMPLETE", //SSV_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT
    "ADV_START_COMPLETE", //SSV_GAP_BLE_ADV_START_COMPLETE_EVT
    "SCAN_START_COMPLETE", //SSV_GAP_BLE_SCAN_START_COMPLETE_EVT = 7
    "AUTH_CMPL", //SSV_GAP_BLE_AUTH_CMPL_EVT
    "KEY", //SSV_GAP_BLE_KEY_EVT,
    "SEC_REQ", //SSV_GAP_BLE_SEC_REQ_EVT,
    "PASSKEY_NOTIF", //SSV_GAP_BLE_PASSKEY_NOTIF_EVT,
    "PASSKEY_REQ", //SSV_GAP_BLE_PASSKEY_REQ_EVT,
    "OOB_REQ", //SSV_GAP_BLE_OOB_REQ_EVT,
    "LOCAL_IR", //SSV_GAP_BLE_LOCAL_IR_EVT,
    "LOCAL_ER", //SSV_GAP_BLE_LOCAL_ER_EVT,
    "NC_REQ", //SSV_GAP_BLE_NC_REQ_EVT,
    "ADV_STOP_COMPLETE", //SSV_GAP_BLE_ADV_STOP_COMPLETE_EVT = 17
    "SCAN_STOP_COMPLETE", //SSV_GAP_BLE_SCAN_STOP_COMPLETE_EVT
    "SET_STATIC_RAND_ADDR", //SSV_GAP_BLE_SET_STATIC_RAND_ADDR_EVT
    "UPDATE_CONN_PARAMS", //SSV_GAP_BLE_UPDATE_CONN_PARAMS_EVT
    "SET_PKT_LENGTH_COMPLETE", //SSV_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT
    "SET_LOCAL_PRIVACY_COMPLETE", //SSV_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT
    "REMOVE_BOND_DEV_COMPLETE", //SSV_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT
    "CLEAR_BOND_DEV_COMPLETE", //SSV_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT
    "GET_BOND_DEV_COMPLETE", //SSV_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT
    "READ_RSSI_COMPLETE", //SSV_GAP_BLE_READ_RSSI_COMPLETE_EVT
    "UPDATE_WHITELIST_COMPLETE", //SSV_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT
    "UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE", //SSV_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT
    "READ_PHY_COMPLETE", //SSV_GAP_BLE_READ_PHY_COMPLETE_EVT = 29
    "SET_PREFERED_DEFAULT_PHY_COMPLETE", //SSV_GAP_BLE_SET_PREFERED_DEFAULT_PHY_COMPLETE_EVT
    "SET_PREFERED_PHY_COMPLETE", //SSV_GAP_BLE_SET_PREFERED_PHY_COMPLETE_EVT
    "EXT_ADV_SET_RAND_ADDR_COMPLETE", //SSV_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT = 32
    "EXT_ADV_SET_PARAMS_COMPLETE", //SSV_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT
    "EXT_ADV_DATA_SET_COMPLETE", //SSV_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT
    "EXT_SCAN_RSP_DATA_SET_COMPLETE", //SSV_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT
    "EXT_ADV_START_COMPLETE", //SSV_GAP_BLE_EXT_ADV_START_COMPLETE_EVT
    "EXT_ADV_STOP_COMPLETE", //SSV_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT
    "EXT_ADV_SET_REMOVE_COMPLETE", //SSV_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT = 38
    "EXT_ADV_SET_CLEAR_COMPLETE", //SSV_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT
    "PERIODIC_ADV_SET_PARAMS_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT
    "PERIODIC_ADV_DATA_SET_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT
    "PERIODIC_ADV_START_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT
    "PERIODIC_ADV_STOP_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT
    "PERIODIC_ADV_CREATE_SYNC_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT
    "PERIODIC_ADV_SYNC_CANCEL_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT
    "PERIODIC_ADV_SYNC_TERMINATE_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT
    "PERIODIC_ADV_ADD_DEV_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT
    "PERIODIC_ADV_REMOVE_DEV_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT
    "PERIODIC_ADV_CLEAR_DEV_COMPLETE", //SSV_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT
    "BLE_SET_EXT_SCAN_PARAMS_COMPLETE", //SSV_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT
    "EXT_SCAN_START_COMPLETE", //SSV_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT
    "EXT_SCAN_STOP_COMPLETE", //SSV_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT
    "PREFER_EXT_CONN_PARAMS_SET_COMPLETE", //SSV_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT
    "PHY_UPDATE_COMPLETE", //SSV_GAP_BLE_PHY_UPDATE_COMPLETE_EVT
    "EXT_ADV_REPORT", //SSV_GAP_BLE_EXT_ADV_REPORT_EVT = 55
    "SCAN_TIMEOUT", //SSV_GAP_BLE_SCAN_TIMEOUT_EVT
    "ADV_TERMINATED", //SSV_GAP_BLE_ADV_TERMINATED_EVT = 57
    "SCAN_REQ_RECEIVED", //SSV_GAP_BLE_SCAN_REQ_RECEIVED_EVT
    "CHANNEL_SELETE_ALGORITHM", //SSV_GAP_BLE_CHANNEL_SELETE_ALGORITHM_EVT
    "PERIODIC_ADV_REPORT", //SSV_GAP_BLE_PERIODIC_ADV_REPORT_EVT
    "PERIODIC_ADV_SYNC_LOST", //SSV_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT
    "PERIODIC_ADV_SYNC_ESTAB" //SSV_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT = 62
};

////////////////////////////////
static void sig_handler(int signum)
{
    extern bool cli_isstop;
    cli_isstop = true;
    //ssv_ble_services_stop(signum);
}

void ssv_gatts_profile_cb(ssv_gatts_cb_event_t event,ssv_gatt_if_t gatts_if,ssv_ble_gatts_cb_param_t *param)
{
    switch(event)
    {
        case SSV_GATTS_CONNECT_EVT:
            {
                ssv_ble_gatts_cb_param_t *connect = (ssv_ble_gatts_cb_param_t *)param;
                printf("Connect id=%d \r\n",connect->connect.conn_id);
                printf("Remote BD=%02X:%02X:%02X:%02X:%02X:%02X\n",
                    connect->connect.remote_bda[5], connect->connect.remote_bda[4], connect->connect.remote_bda[3],
                    connect->connect.remote_bda[2], connect->connect.remote_bda[1], connect->connect.remote_bda[0]);
            }
            break;
        case SSV_GATTS_WRITE_EVT:
            if (!param->write.is_prep)
            {
                printf("SSV_GATTS_WRITE_EVT handle:%d, len:%d, rsp:%d\n", param->write.handle, param->write.len, param->write.need_rsp);
                printf("recv: %x %x %x %x\n", param->write.value[0], param->write.value[1], param->write.value[2], param->write.value[3]);

                /* send response when param->write.need_rsp is true*/
                if (param->write.need_rsp){
                    ssv_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, SSV_GATT_OK, NULL);
                }
            }
            else printf("SSV_GATTS_WRITE_EVT: prepare write\n");
            break;
        case SSV_GATTS_CREAT_ATTR_TAB_EVT:
            {
                int i;
                printf("SSV_GATTS_CREAT_ATTR_TAB_EVT num_handle:%d\n", param->add_attr_tab.num_handle);
                for(i=0; i<param->add_attr_tab.num_handle; i++)
                    printf("%x ", param->add_attr_tab.handles[i]);
                printf("\n");
            }
            break;
        case SSV_GATTS_READ_EVT:
            printf("SSV_GATTS_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);
            ssv_gatt_rsp_t rsp;
            memset((char*)&rsp, 0, sizeof(ssv_gatt_rsp_t));
            rsp.attr_value.handle = param->read.handle;
            rsp.attr_value.len = 4;/*ascii code "ABC"*/
            rsp.attr_value.value[0] = 0x41;
            rsp.attr_value.value[1] = 0x42;
            rsp.attr_value.value[2] = 0x43;
            rsp.attr_value.value[3] = 0x0;
            ssv_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, SSV_GATT_OK, &rsp);
            break;
        case SSV_GATTS_DISCONNECT_EVT:
            {
                extern uint8_t sg_ble_start;
                extern ssv_ble_adv_params_t adv_params;

                if(sg_ble_start) ssv_ble_gap_start_advertising(&adv_params);
            }
            break;
        default:
            if(event < sizeof(ssv_gatts_evt_name)/sizeof(char*))
                printf("SSV_GATTS_%s_EVT\n", ssv_gatts_evt_name[event]);
            else
                printf("\33[32m%s():%d \33[0m\r\n",__FUNCTION__,__LINE__);
            break;
    }
    return;
}

void ssv_gap_event_handler(ssv_gap_ble_cb_event_t event, ssv_ble_gap_cb_param_t *param)
{
    switch(event)
    {
        case SSV_GAP_BLE_SCAN_RESULT_EVT:
        {
            ssv_ble_gap_cb_param_t *scan_result = (ssv_ble_gap_cb_param_t *)param;

            switch ((unsigned char)scan_result->scan_rst.search_evt) {
                case SSV_GAP_SEARCH_INQ_RES_EVT:
                    printf("Find:%02X:%02X:%02X:%02X:%02X:%02X[rssi:%d]\n",
                        scan_result->scan_rst.bda[5], scan_result->scan_rst.bda[4], scan_result->scan_rst.bda[3],
                        scan_result->scan_rst.bda[2], scan_result->scan_rst.bda[1], scan_result->scan_rst.bda[0],
                        scan_result->scan_rst.rssi);

                    uint8_t *adv_name = NULL;
                    uint8_t adv_name_len = 0;
                    char str[32];

                    adv_name = ssv_ble_resolve_adv_data(scan_result->scan_rst.ble_adv, SSV_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
                    if (adv_name_len && (adv_name_len < 30)) {
                        memcpy(str, adv_name, adv_name_len);
                        str[adv_name_len] = 0;
                        printf("Name:%s\n", str);
                    }
                    break;
                default:
                    printf("\33[32m%s():%d \33[0m\r\n",__FUNCTION__,__LINE__);
                    break;
            }
            break;
        }
        default:
            if(event < sizeof(ssv_gap_evt_name)/sizeof(char*))
                printf("SSV_GAP_BLE_%s_EVT\n", ssv_gap_evt_name[event]);
            else
                printf("\33[32m%s():%d \33[0m\r\n",__FUNCTION__,__LINE__);
            break;
    }
}

int main(void)
{
    signal(SIGHUP, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    if(-1 == ssv_ble_services_start())
    {
        return -1;
    }
    sleep(1);
#if 0
    ssv_hal_ble_gatts_set_profile_cb(ssv_gatts_profile_cb);
    ssv_hal_ble_gap_set_event_cb(ssv_gap_event_handler);    
    ssv_hal_ble_common_init();
    ssv_hal_ble_gap_init();
    ssv_hal_ble_gatts_init();
    ssv_ble_gatt_set_local_mtu(128);
#endif
    //ssv_ble_services_running();
    printf("\n");
    return 0;
}

