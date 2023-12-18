
#ifndef _SSV_BLE_MESH_MAIN_H_
#define _SSV_BLE_MESH_MAIN_H_


#define BLE_HCI_CMD         0x01
#define BLE_HCI_ACL         0x02
#define BLE_HCI_SCO         0x03
#define BLE_HCI_EVT         0x04

void ble_to_nimble_rx_evt_ssv(uint8_t *data, uint16_t len);
void ble_to_nimble_rx_acl_ssv(const uint8_t *data, uint16_t len);
#endif
