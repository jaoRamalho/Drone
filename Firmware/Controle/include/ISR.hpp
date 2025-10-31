#ifndef ISR_H
#define ISR_H

#include "Gyroscope.hpp"
#include "GPIOS.h"

void Init_ISR();
void IRAM_ATTR onMpuInterrupt();

#endif // ISR_H