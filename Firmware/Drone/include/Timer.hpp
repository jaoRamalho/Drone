#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>

extern hw_timer_t* mainTimer;
extern hw_timer_t *timer2;

void Init_Timer();
void IRAM_ATTR MainTimer();

// Interrupção no timer 2 para mandar Ping e manter dados do controle atualizados
void Init_Comunication();
void IRAM_ATTR onTimer();

#endif // TIMER_HPP