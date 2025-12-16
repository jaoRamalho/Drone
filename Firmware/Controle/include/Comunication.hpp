#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "Timer.hpp"
#define CE_PIN 22
#define CSN_PIN 21

#define MIN_SAFE_BATTERY 20     // % → inicia pouso automático

extern RF24 radio;

extern volatile bool sendPingFlag;

extern unsigned long lastCommandMillis;
extern const unsigned long COMMAND_INTERVAL;

// Endereços (podem ser parametrizados depois)
extern const uint8_t txAddress[6]; // controle envia/drone envia telemetria
extern const uint8_t rxAddress[6]; // drone recebe/controle recebe telemetria

enum class MoveCommand : uint8_t {
    NONE = 0,
    UP = 1, 
    DOWN = 2,
    FORWARD = 3,
    BACKWARD = 4,
    LEFT = 5,
    RIGHT = 6,
    STOP = 7
};

class Communication
{
private:
    // Dados de cada comando
    MoveCommand action;

    // Dados de telemetria recebidos
    uint8_t battery;
    uint16_t altitude; // altitude em cm
    uint8_t state;

    bool newCommandAvailable;

    // Sequência/protocolo confiável
    uint16_t seqCounter;      // sequência do próximo comando a enviar
    uint16_t lastAckedSeq;    // última sequência confirmada (no transmissor)
    uint16_t lastReceivedSeq; // última sequência recebida (no receptor)

    // Parâmetros do protocolo
    uint8_t maxRetries;
    unsigned long ackTimeoutMs;

    // Singleton - ponteiro estático para a instância única
    static Communication *instance;

    // Construtor privado para singleton
    Communication();

    // Helper: checksum simples (xor)
    uint8_t calcChecksum(const uint8_t *data, size_t len);
    bool verifyChecksum(const uint8_t *data, size_t len);

    // Helper: envia pacote com retries (usado no transmissor)
    bool transmitWithRetries(const uint8_t *pkt, size_t pktLen, uint32_t* rttUsOut);

public:
    ~Communication();

    // Método estático para obter a instância única
    static Communication *getInstance();

    // inicializa o rádio
    void begin();

    void setAction(const MoveCommand newAction);
    const MoveCommand getAction() const;

    const uint8_t getBattery() const { return battery; }
    const uint16_t getAltitude() const { return altitude; }
    const uint8_t getState() const { return state; }

    // Controle: envia comando e lê telemetria do ACK
    void sendCommand();

    // Controle: envia ping ou comando periodicamente
    void sendThing();

    // Envia ping para atualizar telemetria mesmo sem novo comando
    void sendPing(MoveCommand pingValue = MoveCommand::NONE);
};
