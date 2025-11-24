#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "Gyroscope.hpp"
#include <ESP32Servo.h>
#include "MovementController.hpp"




static const uint16_t OFFSET_MOTORS = 42;
static const uint16_t MAX_MOTORS = 115;

struct PID {
    float kp;
    float ki;
    float kd;
    float previous_error;
    float integral;
};

enum StateSystem : uint8_t {
    IDLE = 0,
    START = 1,
    STOP
};

enum StateControl : uint8_t {
    WAKEUP_ALL = 0,
    WAKEUP_M1 = 1,
    WAKEUP_M2 = 2,
    WAKEUP_M3 = 3,
    WAKEUP_M4 = 4,
    PREVIOUS_ARMED_ALL = 5,
    ARMED_ALL = 6,
    ARMED_M1 = 7,
    ARMED_M2 = 8,
    ARMED_M3 = 9,
    ARMED_M4 = 10,
    FLYING = 11
};

enum MotorNumber : uint8_t {
    MOTOR_ALL = 0,
    MOTOR_1 = 1,
    MOTOR_2 = 2,
    MOTOR_3 = 3,
    MOTOR_4 = 4
};

// Classe que será responsável pelo controle do drone
class Control {
private:
    StateControl state, previousState;
    static volatile StateSystem systemState;
    uint8_t commonValueServants, valueM1, valueM2, valueM3, valueM4;
    Servo m1, m2, m3, m4;
    
    Gyroscope* gyro;

    MovementController* movement;


    uint32_t setupTimeLED;
    uint8_t valueBattery;
    
    static Control* instance;
    Control();
public:
    ~Control();
    static uint32_t timeState;
    static uint8_t ledState;
    static uint32_t timeLed;

    static volatile bool buttonPressed;
    static volatile uint32_t timeButton;
    static volatile uint32_t timeBattery;
    
    static Control* Init_Control();
    void resetMotorsValues();
    void setupMotors();

    void setPercentVelocityMotor(uint8_t percent, MotorNumber motorNumber);
    void offMotors();
    void checkBattery();

    uint8_t getBatteryLevel() { return valueBattery; }
    uint8_t getValueM1() { return valueM1; }
    uint8_t getValueM2() { return valueM2; }
    uint8_t getValueM3() { return valueM3; }
    uint8_t getValueM4() { return valueM4; }

    void checkTimeButton(uint32_t deltaTime);
    void loopMotors();
    void loopConfig();
    void loop();

    void setMovementCommand(MoveCommand cmd);
};


#endif // CONTROL_HPP