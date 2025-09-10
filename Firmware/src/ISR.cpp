#include <Arduino.h>
#include "ISR.hpp"

void Init_ISR(){
    Serial.println("Configurando interrupção do MPU6050...");
    pinMode(INT_GYRO_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_GYRO_PIN), onMpuInterrupt, RISING);
    Serial.println("Interrupção do MPU6050 configurada!");
}

void IRAM_ATTR onMpuInterrupt() {
    mpuInterrupt = true;
}