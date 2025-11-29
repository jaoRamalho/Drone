#include "MovementController.hpp"
#include "Control.hpp"

MovementController::MovementController()
    : command(MoveCommand::NONE), M1(OFFSET_M1), M2(OFFSET_M2), M3(OFFSET_M3), M4(OFFSET_M4)
{
}

void MovementController::setCommand(MoveCommand cmd)
{
    command = cmd;
}


void MovementController::computeMotors(uint8_t& m1, uint8_t& m2, uint8_t& m3, uint8_t& m4)
{
    switch (command) {
    case MoveCommand::UP:
        M1 += delta; 
        M2 += delta; 
        M3 += delta; 
        M4 += delta;
        break;

    case MoveCommand::DOWN:
        M1 -= 2*delta; 
        M2 -= 2*delta; 
        M3 -= 2*delta; 
        M4 -= 2*delta;
        break;

    case MoveCommand::FORWARD:
        M1 -= delta; 
        M2 -= delta;
        M3 += delta; 
        M4 += delta;
        break;

    case MoveCommand::BACKWARD:
        M1 += delta; 
        M2 += delta;
        M3 -= delta; 
        M4 -= delta;
        break;

    case MoveCommand::LEFT:
        M1 += delta; 
        M4 += delta;
        M2 -= delta; 
        M3 -= delta;
        break;

    case MoveCommand::RIGHT:
        M1 -= delta; 
        M4 -= delta;
        M2 += delta; 
        M3 += delta;
        break;

    case MoveCommand::STOP:
        M1 = OFFSET_M1; 
        M2 = OFFSET_M2;
        M3 = OFFSET_M3;
        M4 = OFFSET_M4;
        break;

    case MoveCommand::NONE:
    default:
        break;
    }

    m1 = M1;
    m2 = M2;
    m3 = M3;
    m4 = M4;
}
