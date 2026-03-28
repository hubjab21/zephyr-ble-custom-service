# 📡 Bluetooth Low Energy Custom Service (Zephyr / nRF52840)

Embedded project implementing a custom Bluetooth Low Energy (BLE) GATT service using Zephyr RTOS on nRF52840.

---

## 🎯 Project Overview

This project implements a **custom BLE peripheral device** based on Zephyr RTOS.

The system:

* advertises over BLE
* accepts connections from a central device (e.g. phone)
* exposes a custom GATT service
* handles data exchange via BLE characteristics

The implementation is designed for:

* learning BLE protocol
* creating custom services
* embedded communication systems

---

## ⚙️ Hardware

* nRF52840 (Nordic Semiconductor)
* Development board (custom / nrf52840_bsks)
* Buttons and LEDs

---

## 🧠 Software

* Zephyr RTOS
* Bluetooth Low Energy (BLE stack)
* Custom GATT service implementation

---

## 📡 BLE Functionality

The device operates as a **BLE peripheral**:

* advertising device name: `"BSKS 11"` 
* accepts connections
* exposes custom service with multiple characteristics

---

## 🧩 Custom GATT Service

Custom 128-bit UUID service defined in:

```c
BT_UUID_BSKS_SERVICE
```

### Characteristics:

* **Nonce (READ)**
* **Hash (WRITE)**
* **Data (READ / WRITE / INDICATE)**
* **Control (READ / WRITE)**

Defined using Zephyr GATT API 

---

## 🔄 Data Flow

```text
Phone → BLE → GATT write → STM32 (callback)
STM32 → GATT indication → Phone
```

When data is received:

* callback is triggered
* data is processed by application

```c
if (bsks_service_cb_ptr.on_data) {
    bsks_service_cb_ptr.on_data(buf, len);
}
```



---

## ⚙️ Application Logic

Main features:

* BLE initialization (`bt_enable`)
* advertising setup
* connection handling
* LED blinking (status indicator)
* button handling

Main loop:

* toggles LED every 500 ms 

---

## 🔌 BLE Events

Handles:

* connection / disconnection
* parameter updates
* indication enable/disable

---

## 🧪 Features

* BLE peripheral device
* custom GATT service
* bidirectional data exchange
* callback-based architecture
* Zephyr-based implementation

---

## 🚧 Project Status

⚠️ **Work in progress**

Not fully implemented yet:

* data buffering
* full read/write logic
* advanced control handling
* error handling

---

## 🔧 How to Build

Using Zephyr:

```bash
west build -b nrf52840_bsks
west flash
```

---

## 🚀 Possible Improvements

* full GATT implementation
* BLE security (pairing / bonding)
* mobile app integration
* better data protocol (binary format)
* power optimization

---

🎓 Academic Context

This project was developed as part of the course:

Wireless Control and Sensor Networks
at AGH University of Science and Technology.

---

## 👨‍💻 Author

Hubert Jabłoński

