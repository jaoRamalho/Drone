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

void Gestos::loop() {
    while (Serial.available() > 0) {
        int id = Serial.read();
        Serial.print("ID recebido: ");
        Serial.println(id);
    }
}
