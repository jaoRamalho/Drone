#include <Arduino.h>
#include "ISR.hpp"

void Init_ISR(){
    Serial.println('|ISR| ---------- Iniciando configuração de interrupções ----------');
    
    Serial.println("|ISR| - Configurando interrupção do MPU6050...");
    pinMode(INT_GYRO_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_GYRO_PIN), onMpuInterrupt, RISING);
    Serial.println("|ISR| - Interrupção do MPU6050 configurada!");

    Serial.println("|ISR| ---------- Configuração de interrupções finalizada ----------");
}

void IRAM_ATTR onMpuInterrupt() {
    mpuInterrupt = true;
}