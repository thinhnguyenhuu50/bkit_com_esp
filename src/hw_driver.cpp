#include "hw_driver.h"
#include <Arduino.h>

// Use Serial2 for communication to keep Serial0 free for debug logs
#define COMM_SERIAL Serial2
#define BAUD_RATE 115200
#define RX_PIN 16 // Change based on your wiring
#define TX_PIN 17 // Change based on your wiring

bool hw_init(void) {
    // Initialize UART [cite: 73]
    COMM_SERIAL.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
    return true;
}

bool hw_send_byte(uint8_t data) {
    return COMM_SERIAL.write(data) == 1;
}

bool hw_read_byte(uint8_t* data) {
    if (COMM_SERIAL.available()) {
        *data = (uint8_t)COMM_SERIAL.read();
        return true;
    }
    return false;
}

uint32_t hw_available(void) {
    return COMM_SERIAL.available();
}