#include "Control.hpp"

volatile bool Control::buttonPressed = false;
volatile StateSystem Control::systemState = IDLE;
volatile uint32_t Control::timeButton = 0;
Control *Control::instance = nullptr;
uint32_t Control::timeState = 0;
uint8_t Control::ledState = 0;
uint32_t Control::timeLed = 250;
volatile uint32_t Control::timeBattery = 0;
Control::Control() : setupTimeLED(250), valueBattery(0) {
    Serial.println("| CONTROL | ---------- Iniciando Control --------");

    pinMode(LED, OUTPUT);
    digitalWrite(LED, ledState);

    gyro = Gyroscope::Init_Gyroscope();

    Serial.println("| CONTROL | ---------- Control Iniciado --------");

    state = WAKEUP_ALL;
    previousState = state;
}

Control::~Control()
{
}

void Control::checkBattery(){
    if (timeBattery == 0){
        timeBattery = 1000;
        valueBattery = (uint8_t)(analogRead(V_BAT_PIN) / 10.53);
        Serial.println("| CONTROL | - Nível da bateria: " + String(valueBattery) + "%");
    }
}

uint8_t Control::convertPercentForPeriod(uint8_t percent){
    return OFFSET_MOTORS + ((MAX_MOTORS - OFFSET_MOTORS) * percent) / 100;;
}

Control *Control::Init_Control()
{
    if (instance == nullptr){
        instance = new Control();
    }
    return instance;
}

void Control::checkTimeButton(uint32_t deltaTime){
    if(deltaTime > 100) {
        if (systemState == IDLE) {
            Serial.println("| CONTROL | - Entrando em START");
            systemState = START;
            movement.setupMotors();
        } else if (systemState == START) {
            Serial.println("| CONTROL | - Entrando em STOP");
            systemState = IDLE;
            movement.offMotors();
        } 
    }
}


uint8_t commonValueServants = 0;
void Control::loopMotors()
{
    switch (state){
        case WAKEUP_ALL: {
            Serial.println("| CONTROL | - Estado WAKEUP");
            timeState = 5000;
            movement.setPercentVelocityMotor(0, MOTOR_ALL);
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            Serial.println("| CONTROL | - Set Estado ARMED");
            break;
        }
        case PREVIOUS_ARMED_ALL: {
            if (!timeState) {
                if (commonValueServants >= 45) {
                    Serial.println("| CONTROL | - Set Estado FLYING");
                    state = FLYING;
                    timeState = 10000;
                    commonValueServants = 0;
                }
                else {
                    commonValueServants += 5;
                    movement.setPercentVelocityMotor(commonValueServants, MOTOR_ALL);
                    timeState = 1000;
                }
                previousState = state;
                state = ARMED_ALL;
            }
            break;
        }
        case ARMED_ALL: {
            Serial.println("| CONTROL | - Estado ARMED");
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            break;
        }
        case FLYING: {
            movement.computeInclination();
            break;
        }
    }
}

void Control::loopConfig(){
    if(buttonPressed){
        digitalWrite(LED, LOW);
    } else if(!timeLed){
        ledState = !ledState;
        digitalWrite(LED, ledState);
        timeLed = setupTimeLED;
    }
}

void Control::loop(){
    switch (systemState){
    case IDLE:
        loopConfig();
        checkBattery();
        break;
    case START:
        gyro->loop();
        loopMotors();
        loopConfig();
        checkBattery();
        break;
    case STOP:
        movement.offMotors();
        break;
    default:
        break;
    }
}

void Control::setMovementCommand(MoveCommand cmd)
{
    movement.setCommand(cmd);
}

