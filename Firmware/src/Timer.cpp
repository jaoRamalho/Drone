#include "Timer.hpp"
#include "Control.hpp"

hw_timer_t* mainTimer = nullptr;

void Init_Timer() {
    // Configura o timer para disparar a cada 1 ms (1000 microsegundos)
    mainTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (1 MHz), contador ascendente
    timerAttachInterrupt(mainTimer, &MainTimer, true);
    timerAlarmWrite(mainTimer, 1000, true); // Dispara a cada 1000 microsegundos (1 ms)
    timerAlarmEnable(mainTimer);
}


void IRAM_ATTR MainTimer() {
    // Esta função é chamada a cada 1 ms
    if (Control::timeState > 0) { Control::timeState--; }
}