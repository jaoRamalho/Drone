#ifndef PWM_HPP
#define PWM_HPP

#include "stdint.h"

#define MAX_DUTY_CYCLE 1023
#define MIN_DUTY_CYCLE 0

#define FREQUENCY_ESC 20000 // Frequência típica ESC

extern volatile float percentDutyM1;
extern volatile float percentDutyM2;
extern volatile float percentDutyM3;
extern volatile float percentDutyM4;


void INIT_PWMs();

void SET_DUTY_CYCLE_M1(float percentDutyCycle);
void SET_DUTY_CYCLE_M2(float percentDutyCycle);
void SET_DUTY_CYCLE_M3(float percentDutyCycle);
void SET_DUTY_CYCLE_M4(float percentDutyCycle);

float GET_DUTY_CYCLE_M1();
float GET_DUTY_CYCLE_M2();
float GET_DUTY_CYCLE_M3();
float GET_DUTY_CYCLE_M4();

#endif // PWM_HPP