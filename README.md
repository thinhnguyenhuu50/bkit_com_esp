# BKIT-COM: Multi-Interface IoT Communication Protocol

## 1. Project Overview

**BKIT-COM** is a custom multi-layered communication protocol designed for efficient and secure data exchange between heterogeneous IoT devices. The system uses a layered architecture to ensure that high-level application logic remains independent of the underlying hardware interface (UART, SPI, I2C).

This repository contains the **Phase 1** implementation (UART + Protobuf + CRC) and outlines the **Phase 2** expansion tasks for the team.

---

## 2. Current Implementation Status

The current codebase implements the full protocol stack using **UART (Serial2)** on the ESP32.

### 2.1 Protocol Architecture

The system follows the required 3-layer design:

| Layer | Component | Status | Description |
| --- | --- | --- | --- |
| Layer 3 | Application | ✅ Done | High-level APIs (bkit_send_message, bkit_receive_message) handling sensor data. |
| Layer 2 | Message Processing | ✅ Done | Handles framing, header construction, CRC-16 generation/validation, and Protobuf encoding (nanopb). |
| Layer 1 | Hardware Interface | ⚠️ Partial | UART driver is implemented. SPI and I2C are pending. |

### 2.2 Frame Structure

The Message Processing Layer constructs packets with the following format:

| Byte Offset | Field | Size | Description |
| --- | --- | --- | --- |
| 0 | Start Byte | 1 Byte | Fixed marker 0x7E. |
| 1-2 | Length | 2 Bytes | Payload length (Little Endian). |
| 3-4 | CRC | 2 Bytes | CRC-16 checksum covers (Flags + Payload). |
| 5 | Flags | 1 Byte | Metadata/Security flags (currently 0x00). |
| 6+ | Payload | Variable | Protobuf encoded SensorData message. |

---

## 3. Expansion Roadmap (Phase 2)

**Objective:** Demonstrate multi-interface compatibility by implementing SPI and I2C drivers and a switching mechanism. The goal is to swap drivers with minimal changes to the rest of the stack.

### 3.1 Configuration Strategy

We will introduce a `config.h` file to control which interface is active at compile time.

---

## 4. Teammate Assignments

Please implement the following modules. Ensure all drivers adhere to the function signatures defined in `hw_driver.h` so the upper layers remain unaffected.

### [Task B] SPI Driver Implementation

- Assignee: __________________
- Objective: Implement low-level SPI communication.
- Requirements:Hardware: Use VSPI or HSPI on ESP32.Function: hw_init(): Configure Clock Polarity (CPOL) and Phase (CPHA) correctly. Initialize the Chip Select (CS) pin.Function: hw_send_byte(): Pull CS low, transfer byte, pull CS high (or handle framing as needed for the specific SPI slave device logic).Reference: See regarding interface flexibility.

### [Task C] I2C Driver Implementation

- Assignee: __________________
- Objective: Implement low-level I2C communication.
- Requirements:Hardware: Use ESP32 Wire library.Function: hw_init(): Set the I2C address (slave mode) or clock speed (master mode).Error Handling: Handle "bus collision" or NACK errors gracefully.Function: hw_read_byte(): Must check Wire.available() before reading.

### [Task D] Validation & Testing

- Assignee: __________________
- Objective: Verify that switching interfaces works.
- Requirements:Test 1: Connect two ESP32s via SPI and verify data transmission.Test 2: Connect two ESP32s via I2C and verify data transmission.Output: Provide console logs or screenshots showing successful bkit_receive_message decoding for each interface.Robustness: Manually disconnect a wire and confirm the Application Layer handles the timeout/error without crashing.

---

## 5. Development Guidelines

- Do not modify bkit_protocol.cpp. The goal is to prove that changing the driver does not break the application.
- Documentation: Add comments to your driver code explaining specific register settings or library calls (e.g., SPI.beginTransaction).
- Build: Ensure the project compiles clean in PlatformIO after every commit.