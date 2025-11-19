#ifndef ISR_H
#define ISR_H

#include "Gyroscope.hpp"
#include "Control.hpp"

void Init_ISR();
void IRAM_ATTR onMpuInterrupt();
void IRAM_ATTR onButtonPress();

#endif // ISR_H