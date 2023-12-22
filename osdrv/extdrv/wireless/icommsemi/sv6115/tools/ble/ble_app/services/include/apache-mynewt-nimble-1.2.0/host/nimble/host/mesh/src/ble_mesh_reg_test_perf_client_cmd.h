
#ifndef _BLE_MESH_REG_TEST_PERF_CLIENT_CMD_H_
#define _BLE_MESH_REG_TEST_PERF_CLIENT_CMD_H_

#if CONFIG_PERFORMANCE_CLIENT_MODEL
void ble_mesh_register_mesh_test_performance_client(void);
int ble_mesh_test_performance_client_model(int argc, char **argv);
int ble_mesh_test_performance_client_model_performance(int argc, char **argv);
int ble_mesh_test_performance_client_model_ssv_test(int argc, char **argv);

#endif

#endif
