#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "Gyroscope.hpp"
#include "MovementController.hpp"

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

// Classe que será responsável pelo controle do drone
class Control {
private:
    StateControl state, previousState;
    static volatile StateSystem systemState;
    
    Gyroscope* gyro;

    MovementController movement;

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
    void checkBattery();

    uint8_t getBatteryLevel() { return valueBattery; }
    uint8_t getState() { return state; }

    static uint8_t convertPercentForPeriod(uint8_t percent);

    void checkTimeButton(uint32_t deltaTime);
    void loopMotors();
    void loopConfig();
    void loop();

    void setMovementCommand(MoveCommand cmd);
};


#endif // CONTROL_HPP