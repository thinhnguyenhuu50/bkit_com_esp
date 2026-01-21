#include "bkit_protocol.h"
#include "hw_driver.h"
#include <pb_encode.h>
#include <pb_decode.h>
#include <Arduino.h> // For millis()

// CRC-16-CCITT implementation [cite: 101]
uint16_t calculate_crc16(const uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
            else crc <<= 1;
        }
    }
    return crc;
}

bool bkit_send_message(const SensorData* data) {
    uint8_t buffer[MAX_PAYLOAD_SIZE];
    
    // 1. Encode Protobuf [cite: 159]
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    if (!pb_encode(&stream, SensorData_fields, data)) {
        Serial.println("Protobuf Encode Failed");
        return false;
    }
    uint16_t payload_len = stream.bytes_written;

    // 2. Prepare Header
    uint8_t flags = 0x00; // No encryption
    
    // Calculate CRC (Flags + Payload)
    // We create a temporary buffer to calc CRC over Flags + Data
    uint8_t crc_buffer[MAX_PAYLOAD_SIZE + 1];
    crc_buffer[0] = flags;
    memcpy(&crc_buffer[1], buffer, payload_len);
    uint16_t crc = calculate_crc16(crc_buffer, payload_len + 1);

    // 3. Send Frame [cite: 108]
    hw_send_byte(BKIT_START_BYTE);
    hw_send_byte((uint8_t)(payload_len & 0xFF));        // Length Low
    hw_send_byte((uint8_t)((payload_len >> 8) & 0xFF)); // Length High
    hw_send_byte((uint8_t)(crc & 0xFF));                // CRC Low
    hw_send_byte((uint8_t)((crc >> 8) & 0xFF));         // CRC High
    hw_send_byte(flags);                                // Flags
    
    for (size_t i = 0; i < payload_len; i++) {
        hw_send_byte(buffer[i]);
    }
    
    return true;
}

// Helper to read N bytes with timeout
bool read_n_bytes(uint8_t* buf, size_t n, uint32_t timeout_ms) {
    uint32_t start = millis();
    size_t count = 0;
    while (count < n) {
        if (hw_read_byte(&buf[count])) {
            count++;
        }
        if (millis() - start > timeout_ms) return false;
    }
    return true;
}

bool bkit_receive_message(SensorData* data) {
    uint8_t byte;
    
    // 1. Wait for Start Byte
    if (!hw_read_byte(&byte)) return false;
    if (byte != BKIT_START_BYTE) return false;

    // 2. Read Length (2 bytes)
    uint8_t len_buf[2];
    if (!read_n_bytes(len_buf, 2, 100)) return false;
    uint16_t payload_len = len_buf[0] | (len_buf[1] << 8);

    if (payload_len > MAX_PAYLOAD_SIZE) return false; // Sanity check

    // 3. Read CRC (2 bytes)
    uint8_t crc_buf[2];
    if (!read_n_bytes(crc_buf, 2, 100)) return false;
    uint16_t received_crc = crc_buf[0] | (crc_buf[1] << 8);

    // 4. Read Flags (1 byte)
    uint8_t flags;
    if (!read_n_bytes(&flags, 1, 100)) return false;

    // 5. Read Payload
    uint8_t payload_buf[MAX_PAYLOAD_SIZE];
    if (!read_n_bytes(payload_buf, payload_len, 200)) return false;

    // 6. Verify CRC [cite: 112]
    uint8_t check_buf[MAX_PAYLOAD_SIZE + 1];
    check_buf[0] = flags;
    memcpy(&check_buf[1], payload_buf, payload_len);
    uint16_t calculated_crc = calculate_crc16(check_buf, payload_len + 1);

    if (calculated_crc != received_crc) {
        Serial.printf("CRC Error: Recv %04X != Calc %04X\n", received_crc, calculated_crc);
        return false;
    }

    // 7. Decode Protobuf [cite: 162]
    pb_istream_t stream = pb_istream_from_buffer(payload_buf, payload_len);
    if (!pb_decode(&stream, SensorData_fields, data)) {
        Serial.println("Protobuf Decode Failed");
        return false;
    }

    return true;
}