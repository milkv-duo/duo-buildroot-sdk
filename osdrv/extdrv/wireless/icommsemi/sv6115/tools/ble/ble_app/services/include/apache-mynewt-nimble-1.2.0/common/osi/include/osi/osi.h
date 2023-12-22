
#ifndef _OSI_H_
#define _OSI_H_

#include <stdbool.h>
#include <stdint.h>
#if CONFIG_BLE_MESH_SETTINGS
#include "fsal.h"
#endif
#define UNUSED_ATTR __attribute__((unused))

#define CONCAT(a, b) a##b
#define COMPILE_ASSERT(x)

int osi_init(void);
void osi_deinit(void);
#if CONFIG_BLE_MESH_SETTINGS
int32_t osi_fswrite(const char *key, void*data, size_t len);
int osi_fsread(const char *key, u8_t *buf, size_t buf_len);
size_t osi_fsget_length(const char* path);
#endif

#endif /*_OSI_H_*/
