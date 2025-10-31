#include "Control.hpp"

Control* Control::instance = nullptr;
uint32_t Control::timeState = 0;
Control::Control() : commonValueServants(0), valueM1(0), valueM2(0), valueM3(0), valueM4(0) {    
    Serial.println("| CONTROL | ---------- Iniciando Control --------");

    gyro = Gyroscope::Init_Gyroscope();

    pinMode(13, OUTPUT);
    m1.attach(13); 
    m1.write(commonValueServants); 

    pinMode(12, OUTPUT);
    m2.attach(12); 
    m2.write(commonValueServants); 

    pinMode(14, OUTPUT);
    m3.attach(14); 
    m3.write(commonValueServants); 

    pinMode(27, OUTPUT);
    m4.attach(27); 
    m4.write(commonValueServants); 

    state = WAKEUP_ALL;
    previousState = state;

    Serial.println("| CONTROL | ---------- Control Iniciado --------");
}

Control::~Control(){

}

void Control::resetMotorsValues(){
    valueM1 = commonValueServants;
    valueM2 = commonValueServants;
    valueM3 = commonValueServants;
    valueM4 = commonValueServants;
}

Control* Control::Init_Control(){
    if (instance == nullptr) {
        instance = new Control();
    }
    return instance;
}

void Control::loop(){
    switch(state){
        case WAKEUP_ALL: {
            Serial.println("| CONTROL | - Estado WAKEUP");
            commonValueServants = 42;
            resetMotorsValues();
            timeState = 5000;
            m1.write(commonValueServants);
            m2.write(0);
            m3.write(0);
            m4.write(0);
            previousState = state;
            state = PREVIOUS_ARMED_ALL;
            Serial.println("| CONTROL | - Set Estado ARMED");
            break;
        }
        case WAKEUP_M1: {
            Serial.println("| CONTROL | - Estado WAKEUP_M1");
            valueM1 = 42;
            timeState = 5000;
            m1.write(valueM1);
            previousState = state;
            state = ARMED_M1;
            Serial.println("| CONTROL | - Set Estado ARMED_M1");
            break;
        }
        case WAKEUP_M2: {
            Serial.println("| CONTROL | - Estado WAKEUP_M2");
            valueM2 = 42;
            timeState = 5000;
            m2.write(valueM2);
            previousState = state;
            state = ARMED_M2;
            Serial.println("| CONTROL | - Set Estado ARMED_M2");
            break;
        }
        case WAKEUP_M3: {
            Serial.println("| CONTROL | - Estado WAKEUP_M3");
            valueM3 = 42;
            timeState = 5000;
            m3.write(valueM3);
            previousState = state;
            state = ARMED_M3;
            Serial.println("| CONTROL | - Set Estado ARMED_M3");
            break;
        }
        case WAKEUP_M4: {
            Serial.println("| CONTROL | - Estado WAKEUP_M4");
            valueM4 = 42;
            timeState = 5000;
            m4.write(valueM4);
            previousState = state;
            state = ARMED_M4;
            Serial.println("| CONTROL | - Set Estado ARMED_M4");
            break;
        }
        case PREVIOUS_ARMED_ALL: {
            if (!timeState) {
                if (commonValueServants >= 100) {
                    Serial.println("| CONTROL | - Set Estado FLYING");
                    state = FLYING;
                    timeState = 10000;
                } else {
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
        case FLYING : {
            if (!timeState) {
                commonValueServants -= 5;
                resetMotorsValues();
                if (commonValueServants <= 42) {
                    commonValueServants = 42;
                    state = WAKEUP_ALL;
                    timeState = 5000;
                } else {
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