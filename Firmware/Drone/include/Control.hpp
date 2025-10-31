#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "Gyroscope.hpp"
// #include "PWM.hpp"
#include <ESP32Servo.h>
   
/*
O controle do PID dos motores funciona da seguinte forma:
1 - Separação por eixos:
   - Roll (Eixo X): Controle do movimento lateral (esquerda/direita)
   - Pitch (Eixo Y): Controle do movimento longitudinal (frente/trás)
   - Yaw (Eixo Z): Controle da rotação em torno do eixo vertical

2 - Mistura dos sinais para os motores:
   - Cada motor recebe uma combinação dos sinais de controle dos três eixos.
   - A mistura é feita somando ou subtraindo os sinais de controle de cada eixo ao sinal de throttle (aceleração base).

   Para 4 motores (M1, M2, M3, M4), a distribuição típica dos sinais de controle é a seguinte:

   MOTOR | Roll | Pitch | Yaw |
    M1   |   +  |   +   |  -  | 
    M2   |   -  |   +   |  +  |  
    M3   |   -  |   -   |  -  | 
    M4   |   +  |   -   |  +  |  

    Onde o comando final de cada motor é:
    MotorX = throttle + roll_PID + pitch_PID + yaw_PID


*/

struct PID {
    float kp;
    float ki;
    float kd;
    float previous_error;
    float integral;
};

enum StateControl {
    WAKEUP_ALL,
    WAKEUP_M1,
    WAKEUP_M2,
    WAKEUP_M3,
    WAKEUP_M4,
    PREVIOUS_ARMED_ALL,
    ARMED_ALL,
    ARMED_M1,
    ARMED_M2,
    ARMED_M3,
    ARMED_M4,
    FLYING
};

// Classe que será responsável pelo controle do drone
class Control {
private:
    StateControl state, previousState;
    Servo m1, m2, m3, m4;
    uint8_t commonValueServants, valueM1, valueM2, valueM3, valueM4;
    
    Gyroscope* gyro;


    static Control* instance;
    Control();
public:
    ~Control();

    static Control* Init_Control();
    static uint32_t timeState;
    void resetMotorsValues();

    void loop();
};


#endif // CONTROL_HPP