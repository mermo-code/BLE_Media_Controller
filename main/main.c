#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"

#include "ble_init.h"
#include "gpio_fun.h"

/*************LOGGING CODE************/
static void log_bonded_peers(void) {
	union ble_store_value st_val;
	union ble_store_key key;
	int rc;
	int idx = 0;

	memset(&key, 0, sizeof(key));
	// key.peer_addr.type = BLE_ADDR_ANY; // match any address

	ESP_LOGI("BOND_LOG", "All Saved Bonds:");

	while (true) {
		rc = ble_store_read(BLE_STORE_OBJ_TYPE_PEER_ADDR, &key, &st_val);
		if (rc != 0) {
			break; // No more entries
		}
		
		ESP_LOGI("BOND_LOG", "Bonded Device: %02X:%02X:%02X:%02X:%02X:%02X",
				 st_val.sec.peer_addr.val[5], st_val.sec.peer_addr.val[4],
				 st_val.sec.peer_addr.val[3], st_val.sec.peer_addr.val[2],
				 st_val.sec.peer_addr.val[1], st_val.sec.peer_addr.val[0]);
		ESP_LOGI("BOND_LOG",
				 "LTK: %02X %02X %02X %02X %02X %02X %02X %02X "
				 "%02X %02X %02X %02X %02X %02X %02X %02X",
				 st_val.sec.ltk[15], st_val.sec.ltk[14], st_val.sec.ltk[13],
				 st_val.sec.ltk[12], st_val.sec.ltk[11], st_val.sec.ltk[10],
				 st_val.sec.ltk[9], st_val.sec.ltk[8], st_val.sec.ltk[7],
				 st_val.sec.ltk[6], st_val.sec.ltk[5], st_val.sec.ltk[4],
				 st_val.sec.ltk[3], st_val.sec.ltk[2], st_val.sec.ltk[1],
				 st_val.sec.ltk[0]);
		// ESP_LOGI("BOND_LOG", "  Authenticated: %s",
		//		 sec_val.authenticated ? "yes" : "no");
		// ESP_LOGI("BOND_LOG", "  Encrypted: %s",
		//		 sec_val.encrypted ? "yes" : "no");
		// ESP_LOGI("BOND_LOG", "  LTK stored: %s",
		//		 sec_val.ltk_present ? "yes" : "no");

		// Prepare for next
		// memcpy(&key.peer_addr, &sec_val.peer_addr, sizeof(key.peer_addr));
		key.sec.idx++;
	}

	if (idx == 0) {
		ESP_LOGI("BOND_LOG", "No bonded devices found.");
	}
}

void app_main(void) {
	startNVS();
	startBLE();
	log_bonded_peers();
	buttonInit();
}
