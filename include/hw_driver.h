#ifndef HW_DRIVER_H
#define HW_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// Initialize the hardware (UART, SPI, or I2C)
bool hw_init(void);

// Send a single byte
bool hw_send_byte(uint8_t data);

// Receive a single byte (blocking or with timeout implementation in higher layers)
// Returns true if byte received, false if timeout/error
bool hw_read_byte(uint8_t* data);

// Check if data is available
uint32_t hw_available(void);

#endif