#include <Arduino.h>
#include "PWM.hpp"
#include "ISR.hpp"
#include "driver/timer.h"
#include "driver/mcpwm.h"
#include "GPIOS.h"

volatile uint8_t percentDutyM1 = 20;
volatile uint8_t percentDutyM2 = 45;
volatile uint8_t percentDutyM3 = 70;
volatile uint8_t percentDutyM4 = 90;

volatile uint32_t countTimerPWM = 0;

hw_timer_t* pwmTimer = NULL;

void Init_PWMs(){
    pinMode(PIN_M1, OUTPUT);
    pinMode(PIN_M2, OUTPUT);
    pinMode(PIN_M3, OUTPUT);
    pinMode(PIN_M4, OUTPUT);

    pwmTimer = timerBegin(1, 80, true); // Timer 1, prescaler 80 (1 us per tick), count up
    timerAttachInterrupt(pwmTimer, &PWMTimer, true);
    timerAlarmWrite(pwmTimer, PERIOD_ISR_PWM, true); // Alarme a cada 100 us
    timerAlarmEnable(pwmTimer);
}

void IRAM_ATTR PWMTimer(){
    countTimerPWM++;
    if(countTimerPWM >= REAL_PERIOD) countTimerPWM = 0; // Reset a cada período do PWM

    // Motor 1
    if(countTimerPWM < (percentDutyM1 * REAL_PERIOD / 100)){
        digitalWrite(PIN_M1, HIGH);
    } else {
        digitalWrite(PIN_M1, LOW);
    }

    // Motor 2
    if(countTimerPWM < (percentDutyM2 * REAL_PERIOD / 100)){
        digitalWrite(PIN_M2, HIGH);
    } else {
        digitalWrite(PIN_M2, LOW);
    }

    // Motor 3
    if(countTimerPWM < (percentDutyM3 * REAL_PERIOD / 100)){
        digitalWrite(PIN_M3, HIGH);
    } else {
        digitalWrite(PIN_M3, LOW);
    }

    // Motor 4
    if(countTimerPWM < (percentDutyM4 * REAL_PERIOD / 100)){
        digitalWrite(PIN_M4, HIGH);
    } else {
        digitalWrite(PIN_M4, LOW);
    }
}

void SET_DUTY_CYCLE_M1(uint8_t percentDutyCycle){
    if(percentDutyCycle > 100) percentDutyCycle = 100;
    percentDutyM1 = percentDutyCycle;
}

void SET_DUTY_CYCLE_M2(uint8_t percentDutyCycle){
    if(percentDutyCycle > 100) percentDutyCycle = 100;
    percentDutyM2 = percentDutyCycle;
}

void SET_DUTY_CYCLE_M3(uint8_t percentDutyCycle){
    if(percentDutyCycle > 100) percentDutyCycle = 100;
    percentDutyM3 = percentDutyCycle;
}

void SET_DUTY_CYCLE_M4(uint8_t percentDutyCycle){
    if(percentDutyCycle > 100) percentDutyCycle = 100;
    percentDutyM4 = percentDutyCycle;
}


uint8_t GET_DUTY_CYCLE_M1(){
    return percentDutyM1;
}

uint8_t GET_DUTY_CYCLE_M2(){
    return percentDutyM2;
}

uint8_t GET_DUTY_CYCLE_M3(){
    return percentDutyM3;
}

uint8_t GET_DUTY_CYCLE_M4(){
    return percentDutyM4;
}
