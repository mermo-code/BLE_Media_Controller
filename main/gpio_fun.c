/*
 * gpio_fun.c
 *
 *  Created on: 11 Aug 2025
 *      Author: gsabi
 */

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "esp_err.h" // esp_err_t error handling
#include "esp_log.h" // ESP_LOGI(), ESP_LOGE(), logging macros
#include "freertos/idf_additions.h"

#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "hid_vars.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_store.h"
#include "nimble/ble.h"
#include "soc/gpio_num.h"

#include "ble_gap.h"
#include "gpio_fun.h"

TaskHandle_t bttn_detection_task_handle = NULL;

typedef struct {
	gpio_num_t pin;
	bool notify;
	bool prev_state;
	uint8_t button_flag;
	const char *name;
} ButtonConfig;

ButtonConfig buttons[] = {
	{GPIO_NUM_4, false, false, BUTTON_FLAG_PAIR, "Pair"},
	{GPIO_NUM_21, true, false, BUTTON_FLAG_PAUSE_PLAY, "Pause/Play"},
	{GPIO_NUM_18, true, false, BUTTON_FLAG_PLUS_VOL, "Plus Volume"},
	{GPIO_NUM_19, true, false, BUTTON_FLAG_MINUS_VOL, "Minus Volume"},
	{GPIO_NUM_22, true, false, BUTTON_FLAG_SCAN_PREVIOUS_TRACK,
	 "Scan Previous Track"},
	{GPIO_NUM_23, true, false, BUTTON_FLAG_SCAN_NEXT_TRACK, "Scan Next Track"},
	{GPIO_NUM_15, true, false, BUTTON_FLAG_FAST_FORWARD, "Fast Forward"},
	{GPIO_NUM_2, true, false, BUTTON_FLAG_REWIND, "Rewind"},
	{GPIO_NUM_27, true, false, BUTTON_FLAG_MUTE, "Mute"},
};

/*************MAIN DETECTION FUNCTION**************/
void detectButtonPress(void *parameters) {
	while (1) {
		for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
			bool curr_bttn = gpio_get_level(buttons[i].pin);

			if (curr_bttn && !buttons[i].prev_state) { // On Button Press
				ESP_LOGI("Button Detection", "%s button pressed",
						 buttons[i].name);
				if (buttons[i].notify) {
					sendReport(buttons[i].button_flag);
				} else {
					// ESP_LOGI("Button Detection","")
				}
			}
			if (!curr_bttn && buttons[i].prev_state) { // On Button Release
				ESP_LOGI("Button Detection", "%s button released",
						 buttons[i].name);
				if (buttons[i].notify) {
					sendReport(BUTTON_FLAG_RELEASE_ALL);
				} else {
					switch (buttons[i].button_flag) {
					case BUTTON_FLAG_PAIR:
						/*pairingMode = true;
						ble_gap_unpair();
						bleprph_advertise();*/
						break;
					}
				}
			}

			buttons[i].prev_state = curr_bttn;
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

/****************BUTTON INITIATION****************/
void buttonInit() {
	uint64_t pin_mask = 0;

	for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
		pin_mask |= (1ULL << buttons[i].pin);
	}

	gpio_config_t button_io_config = {.pin_bit_mask = pin_mask,
									  .mode = GPIO_MODE_INPUT,
									  .pull_down_en = GPIO_PULLDOWN_ENABLE,
									  .pull_up_en = GPIO_PULLUP_DISABLE,
									  .intr_type = GPIO_INTR_DISABLE};
	gpio_config(
		&button_io_config); // Configure all buttons as pulldown input pins

	xTaskCreate(detectButtonPress, "Button Detection", 4096, NULL, 1,
				&bttn_detection_task_handle);
}