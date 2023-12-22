
#ifndef _BLE_MESH_REG_ONOFF_CLIENT_CMD_H_
#define _BLE_MESH_REG_ONOFF_CLIENT_CMD_H_

#include "ssv_ble_mesh_generic_model_api.h"

void ble_mesh_register_gen_onoff_client(void);
int ble_mesh_generic_onoff_client_model(int argc, char **argv);
void ble_mesh_generic_onoff_client_model_cb(ssv_ble_mesh_generic_client_cb_event_t event,
        ssv_ble_mesh_generic_client_cb_param_t *param);


#endif
