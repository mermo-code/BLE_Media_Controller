/*
 * ble_init.h
 *
 *  Created on: 9 Aug 2025
 *      Author: gsabi
 */

#ifndef MAIN_BLE_INIT_H_
#define MAIN_BLE_INIT_H_

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

// === ESP-IDF Core ===
#include "esp_err.h" // esp_err_t error handling
#include "esp_log.h" // ESP_LOGI(), ESP_LOGE(), logging macros
#include "freertos/idf_additions.h"
#include "host/ble_att.h"
#include "host/ble_hs_adv.h"
#include "host/ble_hs_mbuf.h"
#include "host/ble_sm.h"
#include "host/ble_store.h"
#include "nvs_flash.h" // non-volatile storage (used for BLE bonding info)
#include "portmacro.h"
#include "services/ans/ble_svc_ans.h"
#include "store/config/ble_store_config.h"

// === NimBLE Host + Controller Setup ===
#include "esp_nimble_hci.h"		// Initialize HCI (interface to controller)
#include "nimble/nimble_port.h" // Main NimBLE stack integration
#include "nimble/nimble_port_freertos.h" // Runs NimBLE on FreeRTOS task

// === BLE Host Layer ===
#include "host/ble_gap.h"	// GAP (advertising, connection handling)
#include "host/ble_gatt.h"	// GATT definitions (services, characteristics)
#include "host/ble_hs.h"	// Main BLE host API (GAP, GATT, SM config)
#include "host/ble_uuid.h"	// UUID handling (16/128-bit UUIDs)
#include "host/util/util.h" // Helpers for address copy, buffer mgmt, etc.

// === BLE Services ===
#include "services/dis/ble_svc_dis.h"
#include "services/gap/ble_svc_gap.h" // Generic Access Profile (GAP) service
#include "services/gatt/ble_svc_gatt.h" // Generic Attribute Profile (GATT) service

// === OS-Level Buffer (used for characteristic values) ===
#include "os/os_mbuf.h" // os_mbuf_append(), os_mbuf operations

#include "ble_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

extern esp_err_t ret;
extern const char *TAG;
extern uint8_t own_addr_type;

int gatt_svr_init(void);
void bleprph_on_reset(int reason);
void bleprph_on_sync(void);
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
void bleprph_host_task(void *param);

void startNVS();
void startBLE();
void stopBLE();

extern char *ble_addr_type_str(uint8_t type);
extern char *addr_str(const uint8_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_BLE_INIT_H_ */
