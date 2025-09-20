#ifndef PWM_HPP
#define PWM_HPP

#include "stdint.h"

#define MAX_DUTY_CYCLE 100
#define MIN_DUTY_CYCLE 0

#define FREQUENCY_ESC 50 // Frequência típica ESC em hz

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