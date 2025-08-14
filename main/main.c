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
    union ble_store_key key;
    union ble_store_value val;
    int rc;
    int idx = 0;

    memset(&key, 0, sizeof(key));

    ESP_LOGI("BOND_LOG", "All Saved Bonds:");

    while (true) {
        rc = ble_store_read(BLE_STORE_OBJ_TYPE_PEER_ADDR, &key, &val);
        if (rc != 0) {
            break; // no more entries
        }

        ESP_LOGI("BOND_LOG", "Bonded Device: %02X:%02X:%02X:%02X:%02X:%02X",
                 val.local_irk.addr.val[5], val.local_irk.addr.val[4], val.local_irk.addr.val[3],
                 val.local_irk.addr.val[2], val.local_irk.addr.val[1], val.local_irk.addr.val[0]);

        idx++;
        key.sec.idx++; // move to next index
    }

    if (idx == 0) {
        ESP_LOGI("BOND_LOG", "...No bonded devices found.");
    }
}


void app_main(void) {
	startNVS();
	startBLE();
	// log_bonded_peers(); // Doesn't work
	buttonInit();
}
