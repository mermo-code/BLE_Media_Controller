/*
 * ble_gap.h
 *
 *  Created on: 11 Aug 2025
 *      Author: gsabi
 */

#ifndef MAIN_BLE_GAP_H_
#define MAIN_BLE_GAP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "nimble/ble.h"

extern uint16_t conn_handle;
extern ble_addr_t connected_peer_addr;
extern bool pairingMode;

void releaseButtons();
int sendReport(uint8_t BUTTON_FLAG);

void bleprph_advertise(void);


#endif /* MAIN_BLE_GAP_H_ */
