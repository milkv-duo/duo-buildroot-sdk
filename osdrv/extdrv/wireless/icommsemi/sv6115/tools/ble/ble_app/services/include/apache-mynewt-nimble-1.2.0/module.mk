LIB_SRC :=



ifeq ($(strip $(BLE_GAP_EN)), 0)
ifeq ($(strip $(BLE_GATTS_EN)), 1)
$(warning BLE_GATTS_EN is 1, please enable BLE_GAP_EN)
endif
endif

ifeq ($(strip $(BLE_GAP_EN)), 0)
ifeq ($(strip $(BLE_GATTC_EN)), 1)
$(warning BLE_GATTC_EN is 1, please enable BLE_GAP_EN)
endif
endif

ifeq ($(strip $(MESH_BLE_EN)), 1)
#ssv mesh main
LIB_SRC += host/nimble/host/mesh/src/ble_mesh_cfg_srv_model.c
LIB_SRC += host/nimble/host/mesh/src/argtable3.c
LIB_SRC += host/nimble/host/mesh/src/ble_mesh_console_lib.c
LIB_SRC += host/nimble/host/mesh/src/ble_mesh_adapter.c
LIB_SRC += host/nimble/host/mesh/src/ble_mesh_reg_gen_onoff_client_cmd.c
LIB_SRC += host/nimble/host/mesh/src/ble_mesh_register_provisioner_cmd.c
LIB_SRC += host/nimble/host/mesh/src/ble_mesh_register_node_cmd.c
LIB_SRC += host/nimble/host/mesh/src/ble_mesh_reg_test_perf_client_cmd.c
LIB_SRC += host/nimble/host/mesh/src/ble_mesh_reg_cfg_client_cmd.c
LIB_SRC += host/nimble/host/mesh/src/test_data.c

#mesh core
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/access.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/adv.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/beacon.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/cfg_cli.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/cfg_srv.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/crypto.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/friend.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/health_cli.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/health_srv.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/lpn.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/main.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/net.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/prov.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/provisioner_beacon.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/provisioner_main.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/provisioner_prov.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/proxy_client.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/proxy_server.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/settings.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/transport.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/nimble_host/mesh_bearer_adapt.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/test.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/settings.c

#mesh models
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/client/client_common.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/client/generic_client.c
ifeq ($(strip $(MESH_BLE_OTHER_MODEL_EN)), 1)
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/client/lighting_client.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/client/sensor_client.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/client/time_scene_client.c
endif
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/server/device_property.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/server/generic_server.c
ifeq ($(strip $(MESH_BLE_OTHER_MODEL_EN)), 1)
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/server/lighting_server.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/server/sensor_server.c
endif
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/server/server_common.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/server/state_binding.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/server/state_transition.c
ifeq ($(strip $(MESH_BLE_OTHER_MODEL_EN)), 1)
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_models/server/time_scene_server.c
endif


#btc
LIB_SRC += ble_mesh/ssv_ble_mesh/btc/btc_ble_mesh_config_model.c
LIB_SRC += ble_mesh/ssv_ble_mesh/btc/btc_ble_mesh_generic_model.c
LIB_SRC += ble_mesh/ssv_ble_mesh/btc/btc_ble_mesh_prov.c
LIB_SRC += ble_mesh/ssv_ble_mesh/btc/btc_ble_mesh_health_model.c
ifeq ($(strip $(MESH_BLE_OTHER_MODEL_EN)), 1)
LIB_SRC += ble_mesh/ssv_ble_mesh/btc/btc_ble_mesh_time_scene_model.c
LIB_SRC += ble_mesh/ssv_ble_mesh/btc/btc_ble_mesh_lighting_model.c
LIB_SRC += ble_mesh/ssv_ble_mesh/btc/btc_ble_mesh_sensor_model.c
endif

#mesh API
#Core
LIB_SRC += ble_mesh/ssv_ble_mesh/api/core/ssv_ble_mesh_common_api.c
LIB_SRC += ble_mesh/ssv_ble_mesh/api/core/ssv_ble_mesh_local_data_operation_api.c
LIB_SRC += ble_mesh/ssv_ble_mesh/api/core/ssv_ble_mesh_low_power_api.c
LIB_SRC += ble_mesh/ssv_ble_mesh/api/core/ssv_ble_mesh_networking_api.c
LIB_SRC += ble_mesh/ssv_ble_mesh/api/core/ssv_ble_mesh_provisioning_api.c


#gap/gappc/gapps
LIB_SRC += ble_mesh/ssv_ble_mesh/api/core/ssv_ble_mesh_provisioning_api.c

#models
LIB_SRC += ble_mesh/ssv_ble_mesh/api/models/ssv_ble_mesh_config_model_api.c
LIB_SRC += ble_mesh/ssv_ble_mesh/api/models/ssv_ble_mesh_generic_model_api.c
LIB_SRC += ble_mesh/ssv_ble_mesh/api/models/ssv_ble_mesh_health_model_api.c
ifeq ($(strip $(MESH_BLE_OTHR_MODEL_EN)), 1)
LIB_SRC += ble_mesh/ssv_ble_mesh/api/models/ssv_ble_mesh_lighting_model_api.c
LIB_SRC += ble_mesh/ssv_ble_mesh/api/models/ssv_ble_mesh_sensor_model_api.c
LIB_SRC += ble_mesh/ssv_ble_mesh/api/models/ssv_ble_mesh_time_scene_model_api.c
endif
#mesh common
#LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_common/mesh_aes_encrypt.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_common/mesh_buf.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_common/mesh_common.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_common/mesh_kernel.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_common/mesh_atomic.c
LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_common/mesh_util.c

#ssv_common
LIB_SRC += ble_mesh/ssv_common/src/ssv_timer.c
LIB_SRC += ble_mesh/ssv_common/src/ssv_timer_impl.c

LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_core/storage/settings_ssv.c

LIB_SRC += host/nimble/host/mesh/src/ssv_ble_mesh_main.c
endif #MESH_BLE_EN



LIB_SRC += ble_mesh/ssv_ble_mesh/mesh_common/mesh_aes_encrypt.c
LIB_SRC += host/bluedroid/api/ssv_ble_main.c
#system log
LIB_SRC += ble_mesh/syslog/log/log.c

LIB_SRC += host/bluedroid/api/ssv_gatt_api.c

#from bluedroid
ifeq ($(strip $(BLE_GAP_EN)), 1)
LIB_SRC += host/bluedroid/btc/btc_gap_ble.c
LIB_SRC += host/bluedroid/btc/btc_util.c
LIB_SRC += host/bluedroid/btc/btc_gatt_util.c
LIB_SRC += host/bluedroid/btc/btc_gatt_common.c
LIB_SRC += host/bluedroid/btc/btc_dev.c
LIB_SRC += host/bluedroid/api/ssv_bt_device.c
LIB_SRC += host/bluedroid/api/ssv_gap_ble_api.c
LIB_SRC += host/bluedroid/src/ssv_gap_test.c
LIB_SRC += host/bluedroid/stack/btm/btm_ble_gap.c
#ifeq ($(strip $(BLE_GAP_ATCMD_EN)), 1)
LIB_SRC += host/bluedroid/api/ssv_hal_ble_common.c
LIB_SRC += host/bluedroid/api/ssv_hal_ble_gap.c
#endif
endif

ifeq ($(strip $(BLE_GATTC_EN)), 1)
LIB_SRC += host/bluedroid/btc/btc_gattc.c
LIB_SRC += host/bluedroid/api/ssv_gattc_api.c
LIB_SRC += host/bluedroid/src/ssv_gattc_test.c
#feq ($(strip $(BLE_GATTC_ATCMD_EN)), 1)
LIB_SRC += host/bluedroid/api/ssv_hal_ble_gattc.c
#endif
endif

ifeq ($(strip $(BLE_GATTS_EN)), 1)
LIB_SRC += host/bluedroid/btc/btc_gatts.c
LIB_SRC += host/bluedroid/api/ssv_gatts_api.c
LIB_SRC += host/bluedroid/src/ssv_gatts_test.c
#ifeq ($(strip $(BLE_GATTS_ATCMD_EN)), 1)
LIB_SRC += host/bluedroid/api/ssv_hal_ble_common.c
LIB_SRC += host/bluedroid/api/ssv_hal_ble_gatts.c
#endif
endif

#common btc
LIB_SRC += common/btc/core/btc_manage.c
LIB_SRC += common/btc/core/btc_alarm.c
LIB_SRC += common/btc/core/btc_task.c

#common osi
LIB_SRC += common/osi/alarm.c
LIB_SRC += common/osi/allocator.c
LIB_SRC += common/osi/buffer.c
#LIB_SRC += common/osi/config.c
LIB_SRC += common/osi/fixed_queue.c
LIB_SRC += common/osi/future.c
LIB_SRC += common/osi/hash_functions.c
LIB_SRC += common/osi/hash_map.c
LIB_SRC += common/osi/list.c
LIB_SRC += common/osi/mutex.c
LIB_SRC += common/osi/osi.c
LIB_SRC += common/osi/semaphore.c
LIB_SRC += common/osi/thread.c

LIB_SRC += host/bluedroid/api/ssv_gatt_common_api.c

#util
LIB_SRC += host/nimble/host/util/src/addr.c

#Store config
LIB_SRC += host/nimble/host/store/config/src/ble_store_config.c


#host src
LIB_SRC += host/nimble/host/src/ble_att.c
LIB_SRC += host/nimble/host/src/ble_att_clt.c
LIB_SRC += host/nimble/host/src/ble_att_cmd.c
LIB_SRC += host/nimble/host/src/ble_att_svr.c
LIB_SRC += host/nimble/host/src/ble_eddystone.c
LIB_SRC += host/nimble/host/src/ble_gap.c
LIB_SRC += host/nimble/host/src/ble_gattc.c
LIB_SRC += host/nimble/host/src/ble_gatts.c
LIB_SRC += host/nimble/host/src/ble_gatts_lcl.c
LIB_SRC += host/nimble/host/src/ble_hs.c
LIB_SRC += host/nimble/host/src/ble_hs_adv.c
LIB_SRC += host/nimble/host/src/ble_hs_atomic.c
LIB_SRC += host/nimble/host/src/ble_hs_cfg.c
LIB_SRC += host/nimble/host/src/ble_hs_conn.c
LIB_SRC += host/nimble/host/src/ble_hs_dbg.c
LIB_SRC += host/nimble/host/src/ble_hs_flow.c
LIB_SRC += host/nimble/host/src/ble_hs_hci.c
LIB_SRC += host/nimble/host/src/ble_hs_hci_cmd.c
LIB_SRC += host/nimble/host/src/ble_hs_hci_evt.c
LIB_SRC += host/nimble/host/src/ble_hs_hci_util.c
LIB_SRC += host/nimble/host/src/ble_hs_id.c
LIB_SRC += host/nimble/host/src/ble_hs_log.c
LIB_SRC += host/nimble/host/src/ble_hs_mbuf.c
LIB_SRC += host/nimble/host/src/ble_hs_misc.c
LIB_SRC += host/nimble/host/src/ble_hs_mqueue.c
LIB_SRC += host/nimble/host/src/ble_hs_periodic_sync.c
LIB_SRC += host/nimble/host/src/ble_hs_pvcy.c
LIB_SRC += host/nimble/host/src/ble_hs_shutdown.c
LIB_SRC += host/nimble/host/src/ble_hs_startup.c
LIB_SRC += host/nimble/host/src/ble_hs_stop.c
LIB_SRC += host/nimble/host/src/ble_ibeacon.c
LIB_SRC += host/nimble/host/src/ble_l2cap.c
LIB_SRC += host/nimble/host/src/ble_l2cap_coc.c
LIB_SRC += host/nimble/host/src/ble_l2cap_sig.c
LIB_SRC += host/nimble/host/src/ble_l2cap_sig_cmd.c
LIB_SRC += host/nimble/host/src/ble_monitor.c
LIB_SRC += host/nimble/host/src/ble_sm.c
LIB_SRC += host/nimble/host/src/ble_sm_alg.c
LIB_SRC += host/nimble/host/src/ble_sm_cmd.c
LIB_SRC += host/nimble/host/src/ble_sm_lgcy.c
LIB_SRC += host/nimble/host/src/ble_sm_sc.c
LIB_SRC += host/nimble/host/src/ble_store.c
LIB_SRC += host/nimble/host/src/ble_store_util.c
LIB_SRC += host/nimble/host/src/ble_uuid.c

#porting src
LIB_SRC += porting/nimble/src/endian.c
#LIB_SRC += porting/nimble/src/hal_timer.c
LIB_SRC += porting/nimble/src/mem.c
LIB_SRC += porting/nimble/src/nimble_port.c
LIB_SRC += porting/nimble/src/os_cputime.c
LIB_SRC += porting/nimble/src/os_cputime_pwr2.c
LIB_SRC += porting/nimble/src/os_mbuf.c
LIB_SRC += porting/nimble/src/os_mempool.c
LIB_SRC += porting/nimble/src/os_msys_init.c

#porting examples
#LIB_SRC += porting/examples/linux_blemesh/ble.c

#porting npl
LIB_SRC += porting/npl/freertos/src/nimble_port_freertos.c
LIB_SRC += porting/npl/freertos/src/npl_os_freertos.c

#ext
#LIB_SRC += ext/tinycrypt/src/aes_decrypt.c
#LIB_SRC += ext/tinycrypt/src/aes_encrypt.c
LIB_SRC += ext/tinycrypt/src/cbc_mode.c
LIB_SRC += ext/tinycrypt/src/ccm_mode.c
#LIB_SRC += ext/tinycrypt/src/cmac_mode.c
LIB_SRC += ext/tinycrypt/src/ctr_mode.c
LIB_SRC += ext/tinycrypt/src/ctr_prng.c
LIB_SRC += ext/tinycrypt/src/ecc.c
LIB_SRC += ext/tinycrypt/src/ecc_dh.c
LIB_SRC += ext/tinycrypt/src/ecc_dsa.c
LIB_SRC += ext/tinycrypt/src/ecc_platform_specific.c
LIB_SRC += ext/tinycrypt/src/hmac.c
LIB_SRC += ext/tinycrypt/src/hmac_prng.c
LIB_SRC += ext/tinycrypt/src/sha256.c
LIB_SRC += ext/tinycrypt/src/utils.c

#transport
#LIB_SRC += host/nimble/transport/uart/src/ble_hci_uart.c
LIB_SRC += host/nimble/transport/ram/src/ble_hci_ram.c
LIB_SRC += host/nimble/hal/hal_uart_mesh.c

#store/ram
LIB_SRC += host/nimble/host/store/ram/src/ble_store_ram.c

#gap
LIB_SRC += host/nimble/host/services/gap/src/ble_svc_gap.c

#gatt
LIB_SRC += host/nimble/host/services/gatt/src/ble_svc_gatt.c


LIB_ASRC :=
LIBRARY_NAME := condor_mesh_ble
LOCAL_CFLAGS += -Wno-address

LOCAL_AFLAGS +=
##$(warning TOPDIR is $(TOPDIR))
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/host/mesh/include/mesh
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/host/mesh/src
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/porting/nimble/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/hal/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/porting/npl/freertos/include

LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/host/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ext/tinycrypt/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/host/services/gatt/include
#LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/transport/uart/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/transport/ram/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/host/store/ram/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/host/services/gap/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/host/services/gatt/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/porting/npl/linux/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/host/util/include

#btm
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/bluedroid/stack/btm/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/bluedroid/stack


LOCAL_INC += -I$(TOPDIR)/components/ble/lib
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/host/store/config/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/bluedroid
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/bluedroid/src
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/mesh_common/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/mesh_core
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/mesh_core/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/mesh_models/client/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/mesh_models/server/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/mesh_models/common/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/common
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/common/osi/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/common/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/common/btc/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/log/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/nimble/include/nimble
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_common/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/bluedroid/btc/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/api/models/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/api/core/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/api
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/mesh_core/storage
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/ssv_ble_mesh/btc/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/commmon/btc/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/host/bluedroid/api/include
LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/syslog/log/include
# LOCAL_INC += -I$(TOPDIR)/components/third_party/apache-mynewt-nimble-1.2.0/ble_mesh/freertos/include/freertos
LOCAL_INC += -I$(TOPDIR)/components/third_party/mbedtls/include
LOCAL_INC += -I$(TOPDIR)/components/osal/freertos/nds32
LOCAL_INC += -I$(TOPDIR)/components/ble/inc/common
LOCAL_INC += -I$(TOPDIR)/components/tools/cli
LOCAL_INC += -I$(TOPDIR)/components/crypto
LOCAL_INC += -I$(TOPDIR)/components/crypto_hw
RELEASE_SRC := 2

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
