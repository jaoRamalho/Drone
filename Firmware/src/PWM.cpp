#include "PWM.hpp"
#include "driver/timer.h"
#include "driver/mcpwm.h"
#include "GPIOS.h"

volatile float percentDutyM1 = 0;
volatile float percentDutyM2 = 0;
volatile float percentDutyM3 = 0;
volatile float percentDutyM4 = 0;

void INIT_PWMs(){
    // inicializa MCPWM: mapa de canais exemplo para 4 motores
    // M1 -> MCPWM_UNIT_0, operator A
    // M2 -> MCPWM_UNIT_0, operator B
    // M3 -> MCPWM_UNIT_1, operator A
    // M4 -> MCPWM_UNIT_1, operator B
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PIN_M1);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, PIN_M2);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1A, PIN_M3);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1B, PIN_M4);

    mcpwm_config_t pwm_config;
    pwm_config.frequency = FREQUENCY_ESC;          // frequência típica ESC (ajuste conforme necessário)
    pwm_config.cmpr_a = 0;              // duty inicial %
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);

    SET_DUTY_CYCLE_M1(50); // 50% duty cycle inicial
    SET_DUTY_CYCLE_M2(50); // 50% duty cycle inicial
    SET_DUTY_CYCLE_M3(50); // 50% duty cycle inicial
    SET_DUTY_CYCLE_M4(50); // 50% duty cycle inicial
}

void SET_DUTY_CYCLE_M1(float percentDutyCycle){
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, percentDutyCycle);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    percentDutyM1 = percentDutyCycle;
}

void SET_DUTY_CYCLE_M2(float percentDutyCycle){
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, percentDutyCycle);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    percentDutyM2 = percentDutyCycle;
}

void SET_DUTY_CYCLE_M3(float percentDutyCycle){
    mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, percentDutyCycle);
    mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    percentDutyM3 = percentDutyCycle;
}

void SET_DUTY_CYCLE_M4(float percentDutyCycle){
    mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B, percentDutyCycle);
    mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    percentDutyM4 = percentDutyCycle;
}


float GET_DUTY_CYCLE_M1(){
    return percentDutyM1;
}

float GET_DUTY_CYCLE_M2(){
    return percentDutyM2;
}

float GET_DUTY_CYCLE_M3(){
    return percentDutyM3;
}

float GET_DUTY_CYCLE_M4(){
    return percentDutyM4;
}
