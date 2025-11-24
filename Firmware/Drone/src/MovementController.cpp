#include "MovementController.hpp"

MovementController::MovementController()
    : command(MoveCommand::NONE), basePower(0)
{
}

void MovementController::setCommand(MoveCommand cmd)
{
    command = cmd;
}

int MovementController::clamp(int v)
{
    if (v < 0) return 0;
    if (v > 100) return 100;
    return v;
}

void MovementController::computeMotors(uint8_t& m1, uint8_t& m2, uint8_t& m3, uint8_t& m4)
{
    int M1 = 60;
    int M2 = 60;
    int M3 = 60;
    int M4 = 60;

    const int delta = 20;

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
        M1 = 
        M2 = 
        M3 = 
        M4 = 60;
        break;

    case MoveCommand::NONE:
    default:
        break;
    }

    m1 = clamp(M1);
    m2 = clamp(M2);
    m3 = clamp(M3);
    m4 = clamp(M4);
}
