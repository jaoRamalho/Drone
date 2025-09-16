#ifndef ISR_H
#define ISR_H

#include "Gyroscope.hpp"
#include "GPIOS.h"

extern hw_timer_t* mainTimer;

void Init_ISR();
void IRAM_ATTR onMpuInterrupt();
void IRAM_ATTR MainTimer();

#endif // ISR_H