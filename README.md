# BLE Media Controller

A custom Bluetooth Low Energy (BLE) media controller built for ESP32 devices using the NimBLE stack. This project allows you to control media playback on a paired device, including volume, playback, and track navigation, while optimizing for low power consumption.

---

## Features

- **Volume Control:** Increase or decrease the volume.
- **Play / Pause:** Toggle media playback.
- **Next / Previous Track:** Skip forward or back in your playlist.
- **Fast Forward / Rewind:** Navigate through tracks quickly.
- **Mute:** Mute the audio.
- **Single Device Pairing:** Pairs with one device at a time and automatically enters pairing mode on power-up.
- **Battery Optimization:** Enters light sleep mode after 1 minute of inactivity to conserve power.

---

## Hardware

- Primarily designed for **ESP32-WROOM-32** but should be compatible with other suitable ESP32 chips.
- Easily adjustable GPIO pins in gpio_fun.c

---

## Software

- **ESP-IDF** framework with **NimBLE** stack for BLE functionality.
- Implements manual bonding and connection handling to a single device.
- Verbose logs for all events.

---

## Build & Flash Guide

1. Install **ESP-IDF** and the required tools according to the [ESP-IDF documentation](https://docs.espressif.com/projects/esp-idf/en/release-v4.1/get-started/index.html).

2. Configure the project:

```bash
idf.py menuconfig
```
- Enable Bluetooth
- Enable "NimBLE - BLE only" Host
- Maximum number of concurrent connections = 1
- Maximum nuumber of bonds saved = 1
- Enable "Persist BLE Bonding keys in NVS"
Save sdkconfig
3. Build the project
```bash
idf.py build
```
4. Flash the project
```bash
idf.py flash
```
5. If you want to monitor the log, run
```bash
idf.py monitor
```

---

## Usage

- Power on the device: it enters pairing mode automatically.
- Once paired, will automatically reconnect (unsure how to correctly connect to different devices)
- If no buttons are pressed for 1 minute, the ESP32 enters light sleep. Pressing a button wakes it up.

---

## Notes

- Only a single device can be bonded at a time.
- BLE logs will show warnings if a bonding attempt or connection fails.
- Compatible with Windows, Android, and other BLE-enabled media devices.
