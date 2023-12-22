
#ifndef _BLE_MESH_REGISTER_PROVISIONER_CMD_H_
#define _BLE_MESH_REGISTER_PROVISIONER_CMD_H_

void ble_mesh_register_mesh_provisioner(void);
int ble_mesh_provision_address(int argc, char **argv);
int ble_mesh_provisioner_bearer(int argc, char **argv);
int ble_mesh_provisioner_get_node(int argc, char **argv);
int ble_mesh_provisioner_add_node(int argc, char **argv);
int ble_mesh_provisioner_add_key(int argc, char **argv);
int ble_mesh_provision_bind_local_model(int argc, char **argv);

#endif
