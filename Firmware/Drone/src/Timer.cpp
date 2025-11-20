#include "Timer.hpp"
#include "Control.hpp"
#include "Comunication.hpp"

hw_timer_t* mainTimer = nullptr;

hw_timer_t *timer2 = nullptr;

void Init_Timer() {
    // Configura o timer para disparar a cada 1 ms (1000 microsegundos)
    mainTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1 MHz), contador ascendente
    timerAttachInterrupt(mainTimer, &MainTimer, true);
    timerAlarmWrite(mainTimer, 1000, true); // Dispara a cada 1000 microsegundos (1 ms)
    timerAlarmEnable(mainTimer);
}


void IRAM_ATTR MainTimer() {
    // Esta função é chamada a cada 1 ms
    if (Control::timeState) { Control::timeState--; }
    if (Control::timeLed) { Control::timeLed--; }
    if(Control::timeBattery) { Control::timeBattery--; }
}


void Init_Comunication(){
    Serial.println("|Comunication| ---------- Iniciando configuração de interrupções ----------");

    // configura Timer 2
    timer2 = timerBegin(2, 80, true); // timer 2, prescaler 80 (1 us por tick), count up
    timerAttachInterrupt(timer2, &onTimer, true);
    timerAlarmWrite(timer2, 100000, true); // 100000 us = 100 ms
    timerAlarmEnable(timer2);

    Serial.println("|Comunication| ---------- Configuração de interrupções finalizada ----------");
}

void IRAM_ATTR onTimer() {
    // verifica se o ponteiro está definido
    if(!sendPingFlag) sendPingFlag = true;
}