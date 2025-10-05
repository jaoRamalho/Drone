#include "Control.hpp"

Control* Control::instance = nullptr;
uint32_t Control::timeState = 0;
Control::Control() : commonValueServants(0){
    // Preparar pwm para emitir sinais para o controlador de velocidade dos motores (ESC)
    //gyro = Gyroscope::Init_Gyroscope();

    pinMode(13, OUTPUT);
    m1.attach(13); // Anexa o servo ao pino 13 (GPIO13 no ESP32)
    m1.write(commonValueServants); // Define a posição inicial do servo (pulso mínimo)

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
            commonValueServants = 42;
            timeState = 5000;
            m1.write(commonValueServants);
            // m2.write(0);
            // m3.write(0);
            // m4.write(0);
            state = ARMED;
            break;
        }
        case ARMED: {
            if(!timeState) { 
                if(commonValueServants >= 100){
                    state = FLYING;
                    timeState = 10000;
                } else {
                    commonValueServants += 5;
                    timeState = 1000;
                }
                m1.write(commonValueServants);
                // m2.write(commonValueServants);
                // m3.write(commonValueServants);
                // m4.write(commonValueServants);
            }
            break;
        }
        case FLYING : {
            if (!timeState) {
                commonValueServants -= 5;
                if(commonValueServants <= 42){
                    commonValueServants = 42;
                    state = WAKEUP;
                    timeState = 5000;
                } else {
                    timeState = 1000;
                }
                m1.write(commonValueServants);
                // m2.write(commonValueServants);
                // m3.write(commonValueServants);
                // m4.write(commonValueServants);
            }
            break;
        }
    }
}