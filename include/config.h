#ifndef CONFIG_H
#define CONFIG_H

// UNCOMMENT ONE LINE TO SELECT INTERFACE
#define BKIT_USE_UART
// #define BKIT_USE_SPI
// #define BKIT_USE_I2C

#ifdef BKIT_USE_UART
    #define COMM_SERIAL Serial2
    #define RX_PIN 16
    #define TX_PIN 17
    #define BAUD_RATE 115200
#endif

#ifdef BKIT_USE_SPI
    #define SPI_CS_PIN 5
    #define SPI_SCK_PIN 18
    #define SPI_MISO_PIN 19
    #define SPI_MOSI_PIN 23
#endif

#ifdef BKIT_USE_I2C
    #define I2C_SDA_PIN 21
    #define I2C_SCL_PIN 22
    #define I2C_ADDR 0x42
#endif

#endif // CONFIG_H
