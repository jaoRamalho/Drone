#ifndef GPIOS_H
#define GPIOS_H

#include "stdint.h"

static const uint8_t PIN_LED_STATUS = 2; // GPIO do LED de status

static const uint8_t PIN_BUTTON = 0;     // GPIO do botão

static const uint8_t PIN_M1 = 18; 
static const uint8_t PIN_M2 = 19; 
static const uint8_t PIN_M3 = 21;
static const uint8_t PIN_M4 = 2;

// GYROSCOPE (MPU6050)
static const uint8_t SDA_PIN = 5; // GPIO do barramento I2C - SDA
static const uint8_t SCL_PIN = 4; // GPIO do barramento I2C - SCL
static const uint8_t INT_GYRO_PIN = 10; // GPIO de interrupção do giroscópio

#endif // GPIOS_H