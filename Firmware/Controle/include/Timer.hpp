#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>

extern hw_timer_t* mainTimer;
extern hw_timer_t *timer2;

void Init_Timers();
void IRAM_ATTR MainTimer();
void IRAM_ATTR onTimer();

#endif // TIMER_HPP