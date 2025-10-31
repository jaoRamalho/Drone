#include "Gestos.hpp"

Gestos* Gestos::instance = nullptr;

Gestos::Gestos() {
}

Gestos* Gestos::Init_Gestos() {
    if (!instance) {
        instance = new Gestos();
    }
    return instance;
}

int Gestos::loop() {
    if (Serial.available()) {
        int id = Serial.read();
        return id;
    } 
    return 0;
}
