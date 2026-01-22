#include <Arduino.h>
#include "../include/hw_driver.h"
#include "../include/bkit_protocol.h"

#define MASTER 0

SensorData my_data = SensorData_init_zero;
unsigned long last_send = 0;
uint32_t device_id; // Unique ID for this device

void setup() {
    Serial.begin(115200);

    // Initialize Hardware Interface [cite: 73]
    if (!hw_init()) {
        Serial.println("Hardware Init Failed!");
        while (1)
            ;
    }

    // Generate a random Device ID based on the chip's unique MAC address
    // This ensures Board A has a different ID from Board B automatically
    uint64_t mac = ESP.getEfuseMac();
    device_id = (uint32_t)(mac & 0xFFFFFFFF);

    Serial.printf("BKIT-COM Started. My Device ID: %u\n", device_id);
}

void loop() {
    if (MASTER) {
        // --- SEND LOGIC ---
        if (millis() - last_send > 10000) {
            my_data.sensor_id = device_id; // Send my unique ID
            my_data.sensor_value = 20.0 + (random(0, 500) / 10.0);
            my_data.timestamp = millis();

            if (bkit_send_message(&my_data)) {
                Serial.println("<< Master sent:");
                Serial.printf("   Master ID: %u\n", my_data.sensor_id);
                Serial.printf("   Value: %.2f\n", my_data.sensor_value);
            }
            last_send = millis();
        }
    } else {
        // --- RECEIVE LOGIC ---
        // Always check for incoming messages
        if (hw_available()) {
            SensorData received_data = SensorData_init_zero;
            if (bkit_receive_message(&received_data)) {
                Serial.println("<< Slave received:");
                Serial.printf("   Master ID: %u\n", received_data.sensor_id);
                Serial.printf("   Value: %.2f\n", received_data.sensor_value);
            }
        }
    }
}