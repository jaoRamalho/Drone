#include "Control.hpp"

Control* Control::instance = nullptr;
uint32_t Control::timeState = 0;
Control::Control() : generalDuty(0.0) {
    // Preparar pwm para emitir sinais para o controlador de velocidade dos motores (ESC)
    //gyro = Gyroscope::Init_Gyroscope();
    
    Init_PWMs();

    state = WAKEUP;
}

Control::~Control(){

}

Control* Control::Init_Control(){
    if (instance == nullptr) {
        instance = new Control();
    }
    return instance;
}

void Control::loop(){
    switch(state){
        case WAKEUP: {
            generalDuty = 4.0;
            SET_DUTY_CYCLE_M1(generalDuty);
            SET_DUTY_CYCLE_M2(generalDuty);
            SET_DUTY_CYCLE_M3(generalDuty);
            SET_DUTY_CYCLE_M4(generalDuty);
            timeState = 2200;
            state = ARMED;
            break;
        }
        case ARMED: {
            if(!timeState) { 
                if(generalDuty >= 8.0){
                    generalDuty = 6.0;
                    SET_DUTY_CYCLE_M1(generalDuty);
                    SET_DUTY_CYCLE_M2(generalDuty);
                    SET_DUTY_CYCLE_M3(generalDuty);
                    SET_DUTY_CYCLE_M4(generalDuty);
                    state = FLYING;
                    timeState = 0;
                } else {
                    generalDuty += 0.5;
                    SET_DUTY_CYCLE_M1(generalDuty);
                    SET_DUTY_CYCLE_M2(generalDuty);
                    SET_DUTY_CYCLE_M3(generalDuty);
                    SET_DUTY_CYCLE_M4(generalDuty);
                    timeState = 400;
                }
            }
            break;
        }
        case FLYING : {
            break;
        }
    }
}