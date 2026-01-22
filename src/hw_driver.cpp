#include "../include/hw_driver.h"
#include "../include/config.h"
#include <Arduino.h>

#ifdef BKIT_USE_UART
bool hw_init(void) {
    // Initialize UART [cite: 73]
    COMM_SERIAL.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
    return true;
}

bool hw_send_byte(uint8_t data) {
    return COMM_SERIAL.write(data) == 1;
}

bool hw_read_byte(uint8_t *data) {
    if (COMM_SERIAL.available()) {
        *data = (uint8_t)COMM_SERIAL.read();
        return true;
    }
    return false;
}

uint32_t hw_available(void) {
    return COMM_SERIAL.available();
}
#endif