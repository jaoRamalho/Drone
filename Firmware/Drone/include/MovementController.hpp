#ifndef MOVEMENT_CONTROLLER_HPP
#define MOVEMENT_CONTROLLER_HPP

#include <stdint.h>
#include <ESP32Servo.h>

static const uint16_t freq = 250; // Frequencia de amostragem do controle PID em Hz

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
    void PID_Pitch(double dt);
    void PID_Roll(double dt);
    void PID_Yaw(double dt);
    void ApllyEffectsMotors();
    void resetPIDValues();
    void loopAcceleration();



private:
    Servo m1, m2, m3, m4;
    MoveCommand command;

    int16_t AcX, AcY, AcZ, GyX, GyY, GyZ, Tmp;    
    uint8_t M1, M2, M3, M4;
    uint8_t setPointM1, setPointM2, setPointM3, setPointM4;


    double pitchP, pitchI, pitchD, pitch, pitchSetpoint, pitchError, pitchPrevError, outputPitch;
    double rollP, rollI, rollD, roll, rollSetpoint, rollError, rollPrevError, outputRoll;
    double yawP, yawI, yawD, yaw, yawSetpoint, yawError, yawPrevError, outputYaw;
};

#endif
