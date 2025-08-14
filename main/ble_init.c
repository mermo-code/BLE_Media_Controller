/*
 * ble_init.c
 *
 *  Created on: 9 Aug 2025
 *      Author: gsabi
 */
#include "ble_init.h"
#include "ble_gap.h"
#include "esp_err.h"
#include "esp_nimble_hci.h"
#include "host/ble_hs.h"

/* for nvs_storage*/
#define LOCAL_NAMESPACE "storage"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0)
typedef nvs_handle nvs_handle_t;
#endif

nvs_handle_t Nvs_storage_handle = 0;

const char *TAG = "BLE";

esp_err_t ret;
uint8_t own_addr_type;

void ble_store_config_init(void);

/********************BLE INITIATION*********************/
void startNVS() { //! Mandatory to initialise NVS at the start.
	/* Initialise NVS — it is used to store PHY calibration data */
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	ESP_ERROR_CHECK(
		nvs_open(LOCAL_NAMESPACE, NVS_READWRITE, &Nvs_storage_handle));
}

void startBLE() {
	int rc;
	ESP_ERROR_CHECK(ret);
	ESP_LOGI(TAG, "Initiating NimBLE Port");
	ESP_ERROR_CHECK(nimble_port_init());

	ble_hs_cfg.reset_cb = bleprph_on_reset;
	ble_hs_cfg.sync_cb = bleprph_on_sync;
	ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
	ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

	// BONDING and SECURITY
	ble_hs_cfg.sm_io_cap = BLE_HS_IO_NO_INPUT_OUTPUT;
	ble_hs_cfg.sm_bonding = 1;
	ble_hs_cfg.sm_mitm = 0;
	ble_hs_cfg.sm_sc = 1;
	ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ENC;
	ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC;

	rc = gatt_svr_init();
	assert(rc == 0);

	rc = ble_svc_gap_device_name_set("MediaTap");
	assert(rc == 0);

	ble_store_config_init();

	nimble_port_freertos_init(bleprph_host_task);

	ESP_LOGI(TAG, "Finished initiating NimBLE port");
}

void bleprph_host_task(void *param) {
	ESP_LOGI(TAG, "BLE Host Task Started");

	/* This function will return only when nimble_port_stop() is executed */
	nimble_port_run();

	nimble_port_freertos_deinit();
}

int gatt_svr_init(void) {
	int rc;

	ble_svc_gap_init();
	ble_svc_gatt_init();
	// ble_svc_dis_init();

	rc = ble_gatts_count_cfg(GATT_SVR_SVCS_DEF);
	if (rc != 0) {
		return rc;
	}

	rc = ble_gatts_add_svcs(GATT_SVR_SVCS_DEF);
	if (rc != 0) {
		return rc;
	}

	return 0;
}

void bleprph_on_reset(int reason) {
	ESP_LOGE(TAG, "Resetting state, Error=%d", reason);
}

void bleprph_on_sync(void) {
	int rc;

	ESP_LOGI(TAG, "Host and Controller synced");

	// Check for valid addr
	rc = ble_hs_util_ensure_addr(0);
	assert(rc == 0);

	// Find public address type
	rc = ble_hs_id_infer_auto(0, &own_addr_type);
	if (rc != 0) {
		ESP_LOGE(TAG, "Error determining address type, Error=%d", rc);
		return;
	}

	uint8_t addr_val[6] = {0};
	ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
	ESP_LOGI(TAG, "Determined address type = %s, Device address: %s",
			 ble_addr_type_str(own_addr_type), addr_str(addr_val));

	// struct ble_gap_conn_desc desc;
	// if(ble_gap_conn_find(conn_handle, &desc))
	bleprph_advertise();
}

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
	char buf[BLE_UUID_STR_LEN];

	switch (ctxt->op) {
	case BLE_GATT_REGISTER_OP_SVC:
		ESP_LOGI(TAG, "Registering service (%s) with handle=%d",
				 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
				 ctxt->svc.handle);
		break;
	case BLE_GATT_REGISTER_OP_CHR:
		ESP_LOGI(TAG,
				 "Registering characteristic (%s) with def_handle=%d and "
				 "val_handle=%d",
				 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
				 ctxt->chr.def_handle, ctxt->chr.val_handle);
		break;
	case BLE_GATT_REGISTER_OP_DSC:
		ESP_LOGI(TAG, "Registering descriptor (%s) with handle=%d",
				 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
				 ctxt->dsc.handle);
		break;
	default:
		assert(0);
		break;
	}
}

/**************STOP BLE****************/
void stopBLE() {
	//! Below is the sequence of APIs to be called to disable/deinit NimBLE
	//! host and ESP controller:
	ESP_LOGI(TAG, "Stopping BLE and notification task");
	// vTaskDelete(xHandle);
	int ret = nimble_port_stop();
	if (ret == 0) {
		nimble_port_deinit();

		ret = esp_nimble_hci_deinit();
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "esp_nimble_hci_deinit() failed with error: %d", ret);
		}
	}
}

/******************LOGGING FUNCTIONS******************/
char *ble_addr_type_str(uint8_t type) {
	switch (type) {
	case BLE_OWN_ADDR_PUBLIC:
		return "Public";
	case BLE_OWN_ADDR_RANDOM:
		return "Random Static";
	case BLE_OWN_ADDR_RPA_PUBLIC_DEFAULT:
		return "RPA (Public Identity)";
	case BLE_OWN_ADDR_RPA_RANDOM_DEFAULT:
		return "RPA (Random Identity)";
	default:
		return "Unknown";
	}
}

char *addr_str(const uint8_t *addr) {
	static char buf[18];
	snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", addr[5],
			 addr[4], addr[3], addr[2], addr[1], addr[0]);
	return buf;
}
