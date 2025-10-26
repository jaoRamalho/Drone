#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "Timer.hpp"

/*------------------------------------------ ARDUINO ------------------------------------------------------*/
// #define CE_PIN 7
// #define CSN_PIN 8
/*------------------------------------------ ARDUINO ------------------------------------------------------*/

/*------------------------------------------- ESP32 ------------------------------------------------------*/
#define CE_PIN 22
#define CSN_PIN 21
// #include "driver/timer.h"
// extern hw_timer_t *timer2;

// // Função de inicialização
// void Init_Comunication();

// // Função de callback do timer
// void IRAM_ATTR onTimer();
/*------------------------------------------- ESP32 ------------------------------------------------------*/

extern RF24 radio;

extern volatile bool sendPingFlag;

extern unsigned long lastCommandMillis;
extern const unsigned long COMMAND_INTERVAL;

// Endereços (podem ser parametrizados depois)
extern const uint8_t txAddress[6]; // controle envia/drone envia telemetria
extern const uint8_t rxAddress[6]; // drone recebe/controle recebe telemetria

class Communication {
private:
    // Dados de cada comando
    uint8_t action;   // 0: frente, 1: trás, 2: esquerda, 3: direita, 4: subir, 5: descer
    uint8_t power;    // intensidade 0-255
    
    // Dados do Drone
    uint8_t battery;      // % de bateria
    int16_t altitude;     // altura em cm

    bool isControl;
    
    // Singleton - ponteiro estático para a instância única
    static Communication* instance;
    
    // Construtor privado para singleton
    Communication(bool isControl);
    
public:
    
    ~Communication();
    
    // Método estático para obter a instância única
    static Communication* getInstance(bool isControl = false);

    // inicializa o rádio
    void begin();

    void setCommand(uint8_t newAction, uint8_t newPower);
    uint8_t getAction();
    uint8_t getPower();

    void setTelemetry(uint8_t newBattery, int16_t newAltitude);
    uint8_t getBattery();
    int16_t getAltitude();

    const bool getIsControl();

    // Controle: envia comando e lê telemetria do ACK
    void sendCommand();

    void sendThing();

    // Drone: recebe comando e envia telemetria via ACK
    void receiveCommand();

    // Envia ping para atualizar telemetria mesmo sem novo comando
    void sendPing(uint8_t pingValue = 0xFF);
};
