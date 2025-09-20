#ifndef PWM_HPP
#define PWM_HPP

#include "stdint.h"


#define PERIOD_ISR_PWM 100 // Período da ISR do PWM em us
#define PERIOD_BASE_TIMER (1000000 / PERIOD_ISR_PWM) // Período base do timer em us
#define FREQUENCY_ESC 50 // Frequência típica ESC em hz
#define REAL_PERIOD (PERIOD_BASE_TIMER / FREQUENCY_ESC) // Período real do PWM em us

extern volatile uint8_t percentDutyM1;
extern volatile uint8_t percentDutyM2;
extern volatile uint8_t percentDutyM3;
extern volatile uint8_t percentDutyM4;

extern hw_timer_t* pwmTimer;
extern volatile uint32_t countTimerPWM;

void IRAM_ATTR PWMTimer();
void Init_PWMs();

void SET_DUTY_CYCLE_M1(uint8_t percentDutyCycle);
void SET_DUTY_CYCLE_M2(uint8_t percentDutyCycle);
void SET_DUTY_CYCLE_M3(uint8_t percentDutyCycle);
void SET_DUTY_CYCLE_M4(uint8_t percentDutyCycle);

uint8_t GET_DUTY_CYCLE_M1();
uint8_t GET_DUTY_CYCLE_M2();
uint8_t GET_DUTY_CYCLE_M3();
uint8_t GET_DUTY_CYCLE_M4();

#endif // PWM_HPP