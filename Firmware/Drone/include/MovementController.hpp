#ifndef MOVEMENT_CONTROLLER_HPP
#define MOVEMENT_CONTROLLER_HPP

#include <stdint.h>
#include <ESP32Servo.h>

static const uint16_t OFFSET_MOTORS = 42;
static const uint16_t MAX_MOTORS = 115;

static const uint8_t OFFSET_M1 = 60;
static const uint8_t OFFSET_M2 = 60;
static const uint8_t OFFSET_M3 = 60;
static const uint8_t OFFSET_M4 = 60;

static const uint8_t delta = 10;

struct PID {
    float kp;
    float ki;
    float kd;
    float previous_error;
    float integral;
};


enum MotorNumber : uint8_t {
    MOTOR_ALL = 0,
    MOTOR_1 = 1,
    MOTOR_2 = 2,
    MOTOR_3 = 3,
    MOTOR_4 = 4
};

enum class MoveCommand : uint8_t {
    NONE = 0,
    UP = 1, 
    DOWN = 2,
    FORWARD = 3,
    BACKWARD = 4,
    LEFT = 5,
    RIGHT = 6,
    STOP = 7
};

class MovementController {
public:
    MovementController();
    void setCommand(MoveCommand cmd);
    void computeInclination();
    void offMotors();
    void setupMotors();
    uint8_t convertPercentForPeriod(uint8_t percent);
    void setPercentVelocityMotor(uint8_t percent, MotorNumber motorNumber);

private:
    Servo m1, m2, m3, m4;
    MoveCommand command;

    uint8_t AcX, AcY, AcZ;
    uint8_t M1, M2, M3, M4;
};

#endif
