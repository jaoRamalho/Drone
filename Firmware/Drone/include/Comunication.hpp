#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "Timer.hpp"
#define CE_PIN 22
#define CSN_PIN 21

#define MIN_SAFE_BATTERY 20     // % → inicia pouso automático
#define CONNECTION_TIMEOUT 3000 // ms sem sinal

extern RF24 radio;

extern volatile bool sendPingFlag;

extern unsigned long now;
extern unsigned long lastCommandReceivedMillis;
extern unsigned long lastCommandMillis;
extern const unsigned long COMMAND_INTERVAL;

// Endereços (podem ser parametrizados depois)
extern const uint8_t txAddress[6]; // controle envia/drone envia telemetria
extern const uint8_t rxAddress[6]; // drone recebe/controle recebe telemetria

class Communication
{
private:
    // Dados do Controle
    uint8_t action; // ação do controle
    uint8_t power;  // potência do controle

    // Dados do Drone
    uint8_t battery;  // % de bateria
    int16_t altitude; // altura em cm

    // Sequência/protocolo confiável
    uint16_t lastReceivedSeq; // última sequência recebida (no receptor)

    // Singleton - ponteiro estático para a instância única
    static Communication *instance;

    // Construtor privado para singleton
    Communication();

    // Helper: checksum simples (xor)
    uint8_t calcChecksum(const uint8_t *data, size_t len);
    bool verifyChecksum(const uint8_t *data, size_t len);

public:
    ~Communication();

    // Método estático para obter a instância única
    static Communication *getInstance();

    // inicializa o rádio
    void begin();

    void setTelemetry(uint8_t newBattery, int16_t newAltitude);
    uint8_t getBattery();
    int16_t getAltitude();

    // Drone: recebe comando e envia telemetria via ACK
    void receiveCommand();
};
