#include <Arduino.h>
#include "ISR.hpp"
#include "Control.hpp"

hw_timer_t* mainTimer = NULL;

void Init_ISR(){ 
    Serial.println("|ISR| ---------- Iniciando configuração de interrupções ----------");
    
    mainTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1 us per tick), count up
    timerAttachInterrupt(mainTimer, &MainTimer, true);
    timerAlarmWrite(mainTimer, 1000, true); // Alarme a cada 1000 us (1 ms)
    timerAlarmEnable(mainTimer);
    
    pinMode(INT_GYRO_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_GYRO_PIN), onMpuInterrupt, RISING);

    Serial.println("|ISR| ---------- Configuração de interrupções finalizada ----------");
}

void IRAM_ATTR onMpuInterrupt() {
    mpuInterrupt = true;
}

void IRAM_ATTR MainTimer() {
    // Código a ser executado a cada 1 ms
    if(Control::timeState) { Control::timeState--; }
}
