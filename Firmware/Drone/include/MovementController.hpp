#ifndef MOVEMENT_CONTROLLER_HPP
#define MOVEMENT_CONTROLLER_HPP

#include <stdint.h>

enum class MoveCommand {
    NONE,
    UP,
    DOWN,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    STOP
};

class MovementController {
public:
    MovementController();
    void setCommand(MoveCommand cmd);
    void computeMotors(uint8_t& m1, uint8_t& m2, uint8_t& m3, uint8_t& m4);

private:
    MoveCommand command;
    uint8_t basePower;

    int clamp(int v);
};

#endif
