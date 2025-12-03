#include "MovementController.hpp"
#include "GPIOS.h"

MovementController::MovementController() : command(MoveCommand::NONE),
M1(OFFSET_M1), M2(OFFSET_M2), M3(OFFSET_M3), M4(OFFSET_M4) {

}

void MovementController::setCommand(MoveCommand cmd){
    command = cmd;
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
    m1.write(OFFSET_M1);

    pinMode(PIN_M2, OUTPUT);
    m2.attach(PIN_M2);
    m2.write(OFFSET_M2);

    pinMode(PIN_M3, OUTPUT);
    m3.attach(PIN_M3);
    m3.write(OFFSET_M3);

    pinMode(PIN_M4, OUTPUT);
    m4.attach(PIN_M4);
    m4.write(OFFSET_M4);
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

void MovementController::computeInclination()
{
    switch (command) {
    case MoveCommand::UP :
        AcX = 0;
        AcY = 0;
        AcZ = 0;

        M1 += delta;
        M2 += delta;
        M3 += delta;
        M4 += delta;
        break;

    case MoveCommand::DOWN : 
        AcX = 0;
        AcY = 0;
        AcZ = 0;

        M1 -= delta;
        M2 -= delta;
        M3 -= delta;
        M4 -= delta;
        break;

    case MoveCommand::FORWARD : 
        AcX = 0;
        AcY = 0;
        AcZ = 0;
        
        break;

    case MoveCommand::BACKWARD :
        AcX = 0;
        AcY = 0;
        AcZ = 0;
        
        break;

    case MoveCommand::LEFT :
        AcX = 0;
        AcY = 0;
        AcZ = 0;

        break;

    case MoveCommand::RIGHT :
        AcX = 0;
        AcY = 0;
        AcZ = 0;
        
        break;

    case MoveCommand::STOP : 
        AcX = 0;
        AcY = 0;
        AcZ = 0;
        break;

    case MoveCommand::NONE :
    default:
        break;
    }
}
