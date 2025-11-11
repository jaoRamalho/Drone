#ifndef GPIOS_H
#define GPIOS_H

#include "stdint.h"

// LED de status
static const uint8_t LED = 17; 

// Botão start
static const uint8_t PIN_BUTTON = 27;


static const uint8_t PIN_I00_ESP = 0; // GPIO0

// Motores
static const uint8_t PIN_M1 = 4;
static const uint8_t PIN_M2 = 16;
static const uint8_t PIN_M3 = 13;
static const uint8_t PIN_M4 = 14;

// I2C
static const uint8_t PIN_SCK = 18;
static const uint8_t PIN_MISO = 19;
static const uint8_t PIN_SDA = 21;
static const uint8_t PIN_SCL = 22;
static const uint8_t PIN_MOSI = 23;

// Comunicação (se forem usados)
static const uint8_t PIN_TX_USB = 1;
static const uint8_t PIN_RX_USB = 3;

// Outros sinais (se usados para módulo RF ou sensores)
static const uint8_t PIN_CE  = 32;
static const uint8_t PIN_CSN = 33;
static const uint8_t PIN_IRQ = 35;

static const uint8_t INT_GYRO_PIN = 34; // GPIO de interrupção do giroscópio

#endif // GPIOS_H