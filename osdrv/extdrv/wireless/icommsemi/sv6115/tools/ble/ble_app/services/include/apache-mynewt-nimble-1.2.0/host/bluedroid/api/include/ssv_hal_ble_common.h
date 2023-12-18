#ifndef __SSV_HAL_BLE_COMMON_H__
#define __SSV_HAL_BLE_COMMON_H__

#define API_EVENT_WAIT_TIME 2000

#define SSV_API_CHECK_BIT(var, bitval) (var & bitval)
#define SSV_API_SET_BIT(var, bitval) (var |= bitval)
#define SSV_API_CLEAR_BIT(var, bitval) (var &= ~bitval)

#define CHECK_RESULT(ret, msg)  \
    if (ret) {  \
            printf("%s %d\n", msg, ret);   \
            break;  \
        }   \

#define WAIT_API_EVENT(_grouphandle, _event, _timeout, _failmsg, tag)    \
    evtrlt = xEventGroupWaitBits(_grouphandle, _event, pdTRUE, pdFALSE, _timeout / portTICK_PERIOD_MS);    \
    if (!(evtrlt & _event)) {    \
        SSV_LOGE(tag, "%s", _failmsg);    \
        ret = SSV_FAIL;    \
    }    \


int ssv_hal_ble_common_init(void);
int ssv_hal_ble_common_deinit(void);


#endif