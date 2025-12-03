#include <Arduino.h>
#include "ISR.hpp"
#include "Control.hpp"

void Init_ISR(){ 
    Serial.println("|ISR| ---------- Iniciando configuração de interrupções ----------");
    
    pinMode(INT_GYRO_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_GYRO_PIN), onMpuInterrupt, RISING);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButtonPress, CHANGE);
    
    Serial.println("|ISR| ---------- Configuração de interrupções finalizada ----------");
}

void IRAM_ATTR onMpuInterrupt() {
    Gyroscope::mpuInterrupt = true;
}

void IRAM_ATTR onButtonPress() {
    //Serial.println("|ISR| - Interrupção do botão detectada");
    uint8_t level = digitalRead(PIN_BUTTON);

    if(level == 0) {
        Control::buttonPressed = true;
        Control::timeButton = millis();
    } else {
        Control::buttonPressed = false;
        uint32_t deltaTime = millis() - Control::timeButton;
        Control::Init_Control()->checkTimeButton(deltaTime);
    }
}