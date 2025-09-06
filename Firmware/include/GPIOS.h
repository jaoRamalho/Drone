#ifndef GPIOS_H
#define GPIOS_H

#include "stdint.h"

static const uint8_t PIN_LED_STATUS = 2; // GPIO do LED de status

static const uint8_t PIN_BUTTON = 0;     // GPIO do botão

static const uint8_t PIN_M1 = 19;
static const uint8_t PIN_M2 = 18;
static const uint8_t PIN_M3 = 5;
static const uint8_t PIN_M4 = 17;

static const uint8_t PIN_SDA = 21; // GPIO do barramento I2C - SDA
static const uint8_t PIN_SCL = 22; // GPIO do barramento I2


#endif // GPIOS_H