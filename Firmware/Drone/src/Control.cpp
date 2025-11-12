#include "Control.hpp"

Control *Control::instance = nullptr;
uint32_t Control::timeState = 0;
uint8_t Control::ledState = 0;
uint32_t Control::timeLed = 500;
Control::Control() : commonValueServants(0), valueM1(0), valueM2(0), valueM3(0), valueM4(0), setupTimeLED(500)
{
    Serial.println("| CONTROL | ---------- Iniciando Control --------");

    pinMode(LED, OUTPUT);
    digitalWrite(LED, ledState);

    pinMode(PIN_BUTTON, INPUT_PULLUP);

    gyro = Gyroscope::Init_Gyroscope();

    setupMotors();

    Serial.println("| CONTROL | ---------- Control Iniciado --------");
}

Control::~Control()
{
}

void Control::resetMotorsValues()
{
    valueM1 = commonValueServants;
    valueM2 = commonValueServants;
    valueM3 = commonValueServants;
    valueM4 = commonValueServants;
}

void Control::setupMotors(){

    pinMode(PIN_M1, OUTPUT);
    m1.attach(PIN_M1);
    m1.write(commonValueServants);

    pinMode(PIN_M2, OUTPUT);
    m2.attach(PIN_M2);
    m2.write(commonValueServants);

    pinMode(PIN_M3, OUTPUT);
    m3.attach(PIN_M3);
    m3.write(commonValueServants);

    pinMode(PIN_M4, OUTPUT);
    m4.attach(PIN_M4);
    m4.write(commonValueServants);

    state = WAKEUP_ALL;
    previousState = state;
}

void Control::setPercentVelocityMotor(uint8_t percent, uint8_t motorNumber){
    uint8_t valueToSet = OFFSET_MOTORS + ((MAX_MOTORS - OFFSET_MOTORS) * percent) / 100;
    switch (motorNumber){
        case 1:
            valueM1 = valueToSet;
            m1.write(valueM1);
            break;
        case 2:
            valueM2 = valueToSet;
            m2.write(valueM2);
            break;
        case 3:
            valueM3 = valueToSet;
            m3.write(valueM3);
            break;
        case 4:
            valueM4 = valueToSet;
            m4.write(valueM4);
            break;
        case 0:
            valueM1 = valueToSet;
            m1.write(valueM1);
            valueM2 = valueToSet;
            m2.write(valueM2);
            valueM3 = valueToSet;
            m3.write(valueM3);
            valueM4 = valueToSet;
            m4.write(valueM4);
            commonValueServants = valueToSet;
            break;
        default:
            break;
    }
}

Control *Control::Init_Control()
{
    if (instance == nullptr){
        instance = new Control();
    }
    return instance;
}

void Control::loopMotors(){
    switch (state){
        case WAKEUP_ALL: {
            Serial.println("| CONTROL | - Estado WAKEUP");
            timeState = 5000;
            setPercentVelocityMotor(0, 0);
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            Serial.println("| CONTROL | - Set Estado ARMED");
            break;
        }
        case WAKEUP_M1: {
            Serial.println("| CONTROL | - Estado WAKEUP_M1");
            setPercentVelocityMotor(0, 1);
            timeState = 5000;
            previousState = state;
            state = ARMED_M1;
            Serial.println("| CONTROL | - Set Estado ARMED_M1");
            break;
        }
        case WAKEUP_M2: {
            Serial.println("| CONTROL | - Estado WAKEUP_M2");
            setPercentVelocityMotor(0, 2);
            timeState = 5000;
            previousState = state;
            state = ARMED_M2;
            Serial.println("| CONTROL | - Set Estado ARMED_M2");
            break;
        }
        case WAKEUP_M3: {
            Serial.println("| CONTROL | - Estado WAKEUP_M3");
            setPercentVelocityMotor(0, 3);
            timeState = 5000;
            previousState = state;
            state = ARMED_M3;
            Serial.println("| CONTROL | - Set Estado ARMED_M3");
            break;
        }
        case WAKEUP_M4: {
            Serial.println("| CONTROL | - Estado WAKEUP_M4");
            setPercentVelocityMotor(0, 4);
            timeState = 5000;
            previousState = state;
            state = ARMED_M4;
            Serial.println("| CONTROL | - Set Estado ARMED_M4");
            break;
        }
        case PREVIOUS_ARMED_ALL: {
            if (!timeState) {
                if (commonValueServants >= 80) {
                    Serial.println("| CONTROL | - Set Estado FLYING");
                    state = FLYING;
                    timeState = 10000;
                }
                else {
                    commonValueServants += 5;
                    setPercentVelocityMotor(commonValueServants, 0);
                    timeState = 1000;
                }
                previousState = state;
                state = ARMED_ALL;
            }
            break;
        }
        case ARMED_ALL: {
            Serial.println("| CONTROL | - Estado ARMED");
            setPercentVelocityMotor(commonValueServants, 0);
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            break;
        }
        case ARMED_M1: {
            Serial.println("| CONTROL | - Estado ARMED_M1");
            setPercentVelocityMotor(commonValueServants, 1);
            previousState = state;
            state = FLYING;
            break;
        }
        case ARMED_M2: {
            Serial.println("| CONTROL | - Estado ARMED_M2");
            setPercentVelocityMotor(commonValueServants, 2);
            previousState = state;
            state = FLYING;
            break;
        }
        case ARMED_M3: {
            Serial.println("| CONTROL | - Estado ARMED_M3");
            setPercentVelocityMotor(commonValueServants, 3);
            previousState = state;
            state = FLYING;
            break;
        }
        case ARMED_M4: {
            Serial.println("| CONTROL | - Estado ARMED_M4");
            setPercentVelocityMotor(commonValueServants, 4);
            previousState = state;
            state = FLYING;
            break;
        }
        case FLYING: {
            if (!timeState) {
                commonValueServants -= 5;
                resetMotorsValues();
                if (commonValueServants <= 0){
                    commonValueServants = 0;
                    state = WAKEUP_ALL;
                    timeState = 5000;
                }
                else{
                    timeState = 1000;
                }
                setPercentVelocityMotor(commonValueServants, 0);
            }
            break;
        }
    }
}

void Control::loopConfig(){
    if(!timeLed){
        ledState = !ledState;
        digitalWrite(LED, ledState);
        timeLed = setupTimeLED;
    }

}

void Control::loop(){
    loopMotors();
    loopConfig();
}
