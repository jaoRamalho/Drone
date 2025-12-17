#include "MovementController.hpp"
#include "GPIOS.h"
#include "Gyroscope.hpp"


static const uint16_t OFFSET_MOTORS = 42;
static const uint16_t MAX_MOTORS = 171;

static const double KP_pitch = 0.008;    // Ganho Proporcional do Pitch
static const double KI_pitch = 0.00134;    // Ganho Integral do Pitch
static const double KD_pitch = 0.00166; // Ganho Derivativo do Pitch

static const double KP_roll = 0.008;     // Ganho Proporcional do Roll
static const double KI_roll = 0.00134;     // Ganho Integral do Roll
static const double KD_roll = 0.00166;  // Ganho Derivativo do Roll

static const double KP_yaw = 1;      // Ganho Proporcional do Yaw
static const double KI_yaw = 1;      // Ganho Integral do Y
static const double KD_yaw = 1;   // Ganho Derivativo do Yaw


static const double I_MAX = 100.0;
static const double I_MIN = -100.0;

MovementController::MovementController() : command(MoveCommand::NONE), AcX(0), AcY(0), AcZ(0), M1(0), M2(0), M3(0), M4(0),
    pitch(0), pitchSetpoint(0), pitchError(0), pitchPrevError(0), outputPitch(0),
    roll(0), rollSetpoint(0), rollError(0), rollPrevError(0), outputRoll(0),
    yaw(0), yawSetpoint(0), yawError(0), yawPrevError(0), outputYaw(0), GyX(0), GyY(0), GyZ(0), Tmp(0)
{   
    resetPIDValues();
    setupMotors();
}

void MovementController::setCommand(MoveCommand cmd){
    command = cmd;
}

void MovementController::resetPIDValues(){
    pitchI = 0;
    rollI = 0;
    yawI = 0;
    pitchPrevError = 0;
    rollPrevError = 0;
    yawPrevError = 0;
    pitchP = 0;
    rollP = 0;
    yawP = 0;
    pitchD = 0;
    rollD = 0;
    yawD = 0;
    outputPitch = 0;
    outputRoll = 0;
    outputYaw = 0;
}

void MovementController::loopAcceleration(){
    uint8_t m1 = M1;
    uint8_t m2 = M2;
    uint8_t m3 = M3;
    uint8_t m4 = M4;

    if(setPointM1 > m1) m1++;
    else if(setPointM1 < m1) m1--;

    if(setPointM2 > m2) m2++;
    else if(setPointM2 < m2) m2--;

    if(setPointM3 > m3) m3++;
    else if(setPointM3 < m3) m3--;

    if(setPointM4 > m4) m4++;
    else if(setPointM4 < m4) m4--;

    setPercentVelocityMotor(m1, MOTOR_1);
    setPercentVelocityMotor(m2, MOTOR_2);
    setPercentVelocityMotor(m3, MOTOR_3);
    setPercentVelocityMotor(m4, MOTOR_4);
}

void MovementController::offMotors(){
    MovementController::setPercentVelocityMotor(0, MOTOR_ALL);
}

uint8_t MovementController::convertPercentForPeriod(uint8_t percent){
    if (percent > 50) percent = 50;
    if (percent < 0) percent = 0;

    return OFFSET_MOTORS + ((MAX_MOTORS - OFFSET_MOTORS) * percent) / 100;;
}

void MovementController::setupMotors() {

    pinMode(PIN_M1, OUTPUT);
    m1.attach(PIN_M1);
    
    pinMode(PIN_M2, OUTPUT);
    m2.attach(PIN_M2);
    
    pinMode(PIN_M3, OUTPUT);
    m3.attach(PIN_M3);
    
    pinMode(PIN_M4, OUTPUT);
    m4.attach(PIN_M4);
    
    setPercentVelocityMotor(0, MOTOR_ALL); 
}

void MovementController::setPercentVelocityMotor(uint8_t percent, MotorNumber motorNumber){
    uint8_t valueToSet = convertPercentForPeriod(percent);
    switch (motorNumber){
        case MOTOR_1:
            M1 = valueToSet;
            m1.write(M1);
            break;
        case MOTOR_2:
            M2 = valueToSet;
            m2.write(M2);
            break;
        case MOTOR_3:
            M3 = valueToSet;
            m3.write(M3);
            break;
        case MOTOR_4:
            M4 = valueToSet;
            m4.write(M4);
            break;
        case MOTOR_ALL:
            M1 = valueToSet;
            m1.write(M1);
            M2 = valueToSet;
            m2.write(M2);
            M3 = valueToSet;
            m3.write(M3);
            M4 = valueToSet;
            m4.write(M4);
            break;
        default:
            break;
    }
}


/*
    dt -> tempo em segundos desde a última chamada
*/
void MovementController::PID_Pitch(double dt){
    double accPitch = atan2((double)AcY / 100.0, (double)AcZ / 100.0) * 180 / PI;
    pitch += GyX * dt;

    //filtro complementar
    pitch = 0.98 * pitch + 0.02 * accPitch;

    //PID
    pitchError = pitchSetpoint - pitch;
    pitchP = KP_pitch * pitchError;

    pitchI += KI_pitch * pitchError * dt;


    pitchD = KD_pitch * (pitchError - pitchPrevError) / dt;
    pitchPrevError = pitchError;
    outputPitch = pitchP + pitchI + pitchD;
}   

/*
    dt -> tempo em segundos desde a última chamada
*/
void MovementController::PID_Roll(double dt){
    double accRoll = atan2(-(double)AcX / 100.0, (double)AcZ / 100.0) * 180 / PI;

    roll += GyY * dt;

    //filtro complementar
    roll = 0.98 * roll + 0.02 * accRoll;
    
    //PID
    rollError = rollSetpoint - roll;
    rollP = KP_roll * rollError;

    
    rollI += KI_roll * rollError * dt;


    rollD = KD_roll * (rollError - rollPrevError) / dt;
    rollPrevError = rollError;
    outputRoll = rollP + rollI + rollD;
}

void MovementController::PID_Yaw(double dt){
    yaw += GyZ * dt;

}

float limitPID(float value, float min, float max){
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

void MovementController::ApllyEffectsMotors(){
    // Ainda não vou aplicar, apenas logar os valores para plotagem

    setPointM1 = limitPID(20 + outputPitch + outputRoll, 20, 80);
    setPointM2 = limitPID(20 + outputPitch - outputRoll, 20, 80);
    setPointM3 = limitPID(20 - outputPitch - outputRoll, 20, 80);
    setPointM4 = limitPID(20 - outputPitch + outputRoll, 20, 80);
}

void MovementController::computeInclination()
{   

    Gyroscope* gyro = Gyroscope::Init_Gyroscope();
    gyro->getData(&AcX, &AcY, &AcZ, &Tmp, &GyX, &GyY, &GyZ);

    switch (command) {
        case MoveCommand::UP :
            Serial.println("SUBINDO");
            break;

        case MoveCommand::DOWN : 
            Serial.println("DESCENDO");
            break;

        case MoveCommand::FORWARD : 
            Serial.println("AVANÇANDO");
            break;

        case MoveCommand::BACKWARD :
            Serial.println("RETROCEDENDO");
            break;

        case MoveCommand::LEFT :
            Serial.println("ESQUERDA");
            break;

        case MoveCommand::RIGHT :
            Serial.println("DIREITA");
            break;

        case MoveCommand::STOP : 
            Serial.println("PARANDO");
            break;

        case MoveCommand::NONE :
        default:
            Serial.println("NONE");
            break;
    }

    command = MoveCommand::NONE;
}
