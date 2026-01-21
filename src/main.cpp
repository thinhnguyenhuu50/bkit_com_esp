#include <Arduino.h>
#include "hw_driver.h"
#include "bkit_protocol.h"

// Define functionality based on role (simulate both or switch)
SensorData my_data = SensorData_init_zero;
unsigned long last_send = 0;

void setup() {
    // Debug Serial (USB)
    Serial.begin(115200);
    while(!Serial);
    Serial.println("BKIT-COM Protocol Starting...");

    // Hardware Interface Layer Init
    if (!hw_init()) {
        Serial.println("Hardware Init Failed!");
        while(1);
    }
    Serial.println("Hardware Initialized (UART2)");
}

void loop() {
    // -------------------------------------------------
    // SENDER ROLE (Send every 2 seconds)
    // -------------------------------------------------
    if (millis() - last_send > 2000) {
        my_data.sensor_id = 101;
        my_data.sensor_value = 25.5 + (random(0, 100) / 100.0);
        my_data.timestamp = millis();

        Serial.printf("Sending: ID=%d, Val=%.2f, Time=%llu\n", 
                      my_data.sensor_id, my_data.sensor_value, my_data.timestamp);

        if (bkit_send_message(&my_data)) {
            Serial.println(">> Message Sent");
        } else {
            Serial.println(">> Send Failed");
        }
        
        last_send = millis();
    }

    // -------------------------------------------------
    // RECEIVER ROLE (Poll continuously)
    // -------------------------------------------------
    if (hw_available()) {
        SensorData received_data = SensorData_init_zero;
        if (bkit_receive_message(&received_data)) {
            Serial.println("<< Message Received!");
            Serial.printf("   ID: %d\n", received_data.sensor_id);
            Serial.printf("   Value: %.2f\n", received_data.sensor_value);
            Serial.printf("   Timestamp: %llu\n", received_data.timestamp);
        } else {
            // Note: Receive might fail if we catch garbage bytes or CRC fails
            // bkit_receive_message handles skipping invalid start bytes
        }
    }
}