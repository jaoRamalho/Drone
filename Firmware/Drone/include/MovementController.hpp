#ifndef MOVEMENT_CONTROLLER_HPP
#define MOVEMENT_CONTROLLER_HPP

#include <stdint.h>

static const uint8_t OFFSET_M1 = 60;
static const uint8_t OFFSET_M2 = 60;
static const uint8_t OFFSET_M3 = 60;
static const uint8_t OFFSET_M4 = 60;

static const uint8_t delta = 10;

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
    void computeMotors(uint8_t& m1, uint8_t& m2, uint8_t& m3, uint8_t& m4);

private:
    MoveCommand command;

    uint8_t M1, M2, M3, M4; 
};

#endif
