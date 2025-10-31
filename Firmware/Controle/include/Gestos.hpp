#ifndef GESTOS_HPP
#define GESTOS_HPP

#include <Arduino.h>

class Gestos {
private:
    Gestos();
    static Gestos* instance;

public:
    static Gestos* Init_Gestos();
    int loop();
};

#endif // GESTOS_HPP
