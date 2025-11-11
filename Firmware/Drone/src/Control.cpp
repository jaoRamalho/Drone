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
            commonValueServants = OFFSET_MOTORS;
            resetMotorsValues();
            timeState = 5000;
            m1.write(commonValueServants);
            m2.write(commonValueServants);
            m3.write(commonValueServants);
            m4.write(commonValueServants);
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            Serial.println("| CONTROL | - Set Estado ARMED");
            break;
        }
        case WAKEUP_M1: {
            Serial.println("| CONTROL | - Estado WAKEUP_M1");
            valueM1 = OFFSET_MOTORS;
            timeState = 5000;
            m1.write(valueM1);
            previousState = state;
            state = ARMED_M1;
            Serial.println("| CONTROL | - Set Estado ARMED_M1");
            break;
        }
        case WAKEUP_M2: {
            Serial.println("| CONTROL | - Estado WAKEUP_M2");
            valueM2 = OFFSET_MOTORS;
            timeState = 5000;
            m2.write(valueM2);
            previousState = state;
            state = ARMED_M2;
            Serial.println("| CONTROL | - Set Estado ARMED_M2");
            break;
        }
        case WAKEUP_M3: {
            Serial.println("| CONTROL | - Estado WAKEUP_M3");
            valueM3 = OFFSET_MOTORS;
            timeState = 5000;
            m3.write(valueM3);
            previousState = state;
            state = ARMED_M3;
            Serial.println("| CONTROL | - Set Estado ARMED_M3");
            break;
        }
        case WAKEUP_M4: {
            Serial.println("| CONTROL | - Estado WAKEUP_M4");
            valueM4 = OFFSET_MOTORS;
            timeState = 5000;
            m4.write(valueM4);
            previousState = state;
            state = ARMED_M4;
            Serial.println("| CONTROL | - Set Estado ARMED_M4");
            break;
        }
        case PREVIOUS_ARMED_ALL: {
            if (!timeState) {
                if (commonValueServants >= MAX_MOTORS) {
                    Serial.println("| CONTROL | - Set Estado FLYING");
                    state = FLYING;
                    timeState = 10000;
                }
                else {
                    commonValueServants += 5;
                    resetMotorsValues();
                    timeState = 1000;
                }
                previousState = state;
                state = ARMED_ALL;
            }
            break;
        }
        case ARMED_ALL: {
            Serial.println("| CONTROL | - Estado ARMED");
            m1.write(commonValueServants);
            m2.write(commonValueServants);
            m3.write(commonValueServants);
            m4.write(commonValueServants);
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            break;
        }
        case ARMED_M1: {
            Serial.println("| CONTROL | - Estado ARMED_M1");
            m1.write(commonValueServants);
            previousState = state;
            state = FLYING;
            break;
        }
        case ARMED_M2: {
            Serial.println("| CONTROL | - Estado ARMED_M2");
            m2.write(commonValueServants);
            previousState = state;
            state = FLYING;
            break;
        }
        case ARMED_M3: {
            Serial.println("| CONTROL | - Estado ARMED_M3");
            m3.write(commonValueServants);
            previousState = state;
            state = FLYING;
            break;
        }
        case ARMED_M4: {
            Serial.println("| CONTROL | - Estado ARMED_M4");
            m4.write(commonValueServants);
            previousState = state;
            state = FLYING;
            break;
        }
        case FLYING: {
            if (!timeState) {
                commonValueServants -= 5;
                resetMotorsValues();
                if (commonValueServants <= OFFSET_MOTORS){
                    commonValueServants = OFFSET_MOTORS;
                    state = WAKEUP_ALL;
                    timeState = 5000;
                }
                else{
                    timeState = 1000;
                }
                m1.write(commonValueServants);
                m2.write(commonValueServants);
                m3.write(commonValueServants);
                m4.write(commonValueServants);
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
