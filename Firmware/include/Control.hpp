#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "Gyroscope.hpp"
#include "PWM.hpp"

   
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

// Classe que será responsável pelo controle do drone
class Control {
private:
    Gyroscope* gyro;

public:
    Control(Gyroscope* g);
    ~Control();

    void update(float setpoint, float measured, float dt);
    
    float getControlSignal();
};


#endif // CONTROL_HPP