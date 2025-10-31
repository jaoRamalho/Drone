#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "Timer.hpp"
#define CE_PIN 22
#define CSN_PIN 21

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

    bool newCommandAvailable;

    // Sequência/protocolo confiável
    uint16_t seqCounter;      // sequência do próximo comando a enviar
    uint16_t lastAckedSeq;    // última sequência confirmada (no transmissor)
    uint16_t lastReceivedSeq; // última sequência recebida (no receptor)

    // Parâmetros do protocolo
    uint8_t maxRetries;
    unsigned long ackTimeoutMs;
    
    // Singleton - ponteiro estático para a instância única
    static Communication* instance;
    
    // Construtor privado para singleton
    Communication();

    // Helper: checksum simples (xor)
    uint8_t calcChecksum(const uint8_t* data, size_t len);
    bool verifyChecksum(const uint8_t* data, size_t len);

    // Helper: envia pacote com retries (usado no transmissor)
    bool transmitWithRetries(const uint8_t* pkt, size_t pktLen);
    
public:
    
    ~Communication();
    
    // Método estático para obter a instância única
    static Communication* getInstance();

    // inicializa o rádio
    void begin();

    void setCommand(uint8_t newAction, uint8_t newPower);
    uint8_t getAction();
    uint8_t getPower();

    // Controle: envia comando e lê telemetria do ACK
    void sendCommand();

    // Controle: envia ping ou comando periodicamente
    void sendThing();

    // Envia ping para atualizar telemetria mesmo sem novo comando
    void sendPing(uint8_t pingValue = 0xFF);
};
