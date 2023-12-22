
#ifndef _BLE_MESH_REGISTER_NODE_CMD_H_
#define _BLE_MESH_REGISTER_NODE_CMD_H_

int ble_mesh_node_enable_bearer(int argc, char **argv);
int ble_mesh_provisioner_enable_bearer(int argc, char **argv);
int ble_mesh_register_node_cb(int argc, char** argv);
int ble_mesh_init(int argc, char **argv);
int ble_mesh_power_set(int argc, char **argv);
int ble_mesh_load_oob(int argc, char **argv);
void ble_mesh_register_mesh_node(void);
int ble_mesh_node_reset(int argc, char** argv);
#if CONFIG_PERFORMANCE_CLIENT_MODEL
int ble_mesh_node_statistics_regist(int argc, char **argv);
#endif
int ble_mesh_node_enter_network_auto(int argc, char **argv);
int ble_mesh_get_publish_ack_count(int argc, char **argv);

int ble_mesh_node_suspend(int argc, char** argv);
int ble_mesh_node_resume(int argc, char** argv);

#endif
