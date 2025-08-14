/*
 * ble_gatt.h
 *
 *  Created on: 9 Aug 2025
 *      Author: gsabi
 */

#ifndef MAIN_BLE_GATT_H_
#define MAIN_BLE_GATT_H_

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

// === ESP-IDF Core ===
#include "esp_err.h" // esp_err_t error handling
#include "esp_log.h" // ESP_LOGI(), ESP_LOGE(), logging macros

#include "host/ble_gatt.h" // GATT definitions (services, characteristics)

#include "hid_vars.h"

#ifdef __cplusplus
extern "C" {
#endif

// Access all HID service characteristics
extern int hid_svc_chr_access(uint16_t conn_handle, uint16_t attr_handle,
							  struct ble_gatt_access_ctxt *ctxt, void *arg);

// Access all HID service descriptors
extern int hid_svc_dsc_access(uint16_t conn_handle, uint16_t attr_handle,
							  struct ble_gatt_access_ctxt *ctxt, void *arg);

extern const struct ble_gatt_svc_def GATT_SVR_SVCS_DEF[];

#ifdef __cplusplus
}
#endif

#endif /* MAIN_BLE_GATT_H_ */
