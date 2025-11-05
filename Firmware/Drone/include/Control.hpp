#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "Gyroscope.hpp"
#include <ESP32Servo.h>


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