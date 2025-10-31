#include <Arduino.h>
#include "ISR.hpp"
#include "Control.hpp"

void Init_ISR(){ 
    Serial.println("|ISR| ---------- Iniciando configuração de interrupções ----------");
    
    pinMode(INT_GYRO_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_GYRO_PIN), onMpuInterrupt, RISING);
    
    Serial.println("|ISR| ---------- Configuração de interrupções finalizada ----------");
}

void IRAM_ATTR onMpuInterrupt() {
    mpuInterrupt = true;
}


