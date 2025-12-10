#include "MovementController.hpp"
#include "GPIOS.h"
#include "Gyroscope.hpp"


static const uint16_t OFFSET_MOTORS = 42;
static const uint16_t MAX_MOTORS = 171;

static const float KP_pitch = 1.5;    // Ganho Proporcional do Pitch
static const float KI_pitch = 0.0;    // Ganho Integral do Pitch
static const float KD_pitch = 0.5; // Ganho Derivativo do Pitch

static const float KP_roll = 1.5;     // Ganho Proporcional do Roll
static const float KI_roll = 0.0;     // Ganho Integral do Roll
static const float KD_roll = 0.5;  // Ganho Derivativo do

static const float KP_yaw = 1.5;      // Ganho Proporcional do Yaw
static const float KI_yaw = 0.0;      // Ganho Integral do Y
static const float KD_yaw = 0.5;   // Ganho Derivativo do Yaw

MovementController::MovementController() : command(MoveCommand::NONE), AcX(0), AcY(0), AcZ(0), M1(0), M2(0), M3(0), M4(0),
    pitch(0), pitchSetpoint(0), pitchError(0), pitchPrevError(0), outputPitch(0),
    roll(0), rollSetpoint(0), rollError(0), rollPrevError(0), outputRoll(0),
    yaw(0), yawSetpoint(0), yawError(0), yawPrevError(0), outputYaw(0), GyX(0), GyY(0), GyZ(0), Tmp(0)
{
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


void MovementController::offMotors(){
    m1.detach();
    m2.detach();
    m3.detach();
    m4.detach();

    // garante pinos em LOW (evita saída flutuante)
    pinMode(PIN_M1, OUTPUT);
    digitalWrite(PIN_M1, LOW);

    pinMode(PIN_M2, OUTPUT);
    digitalWrite(PIN_M2, LOW);

    pinMode(PIN_M3, OUTPUT);
    digitalWrite(PIN_M3, LOW);

    pinMode(PIN_M4, OUTPUT);
    digitalWrite(PIN_M4, LOW);

    // zera valores internos
    M4 = M3 = M2 = M1 = 0;
}

uint8_t MovementController::convertPercentForPeriod(uint8_t percent){
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
void MovementController::PID_Pitch(float dt){
    float accPitch = atan2(AcY / 100.0, AcZ / 100.0) * 180 / PI;
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
void MovementController::PID_Roll(float dt){
    float accRoll = atan2(-AcX / 100.0, AcZ / 100.0) * 180 / PI;
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

void MovementController::PID_Yaw(float dt){
    yaw += GyZ * dt;

}

void MovementController::ApllyEffectsMotors(){
    // Ainda não vou aplicar, apenas logar os valores para plotagem
    
}

void MovementController::computeInclination()
{   

    Gyroscope* gyro = Gyroscope::Init_Gyroscope();
    gyro->getData(&AcX, &AcY, &AcZ, &Tmp, &GyX, &GyY, &GyZ);

    switch (command) {
        case MoveCommand::UP :
            break;

        case MoveCommand::DOWN : 
            break;

        case MoveCommand::FORWARD : 
            break;

        case MoveCommand::BACKWARD :
            break;

        case MoveCommand::LEFT :
            break;

        case MoveCommand::RIGHT :
            break;

        case MoveCommand::STOP : 
            break;

        case MoveCommand::NONE :
        default:
            break;
    }

    command = MoveCommand::NONE;
}
