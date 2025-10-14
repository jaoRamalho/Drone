#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>

extern hw_timer_t* mainTimer;

void Init_Timer();
void IRAM_ATTR MainTimer();

#endif // TIMER_HPP