#include "Comunication.hpp"

RF24 radio(CE_PIN, CSN_PIN);

hw_timer_t *timer2 = nullptr;

// Endereços (podem ser parametrizados depois)
const uint8_t txAddress[6] = "00001"; // controle envia/drone envia telemetria
const uint8_t rxAddress[6] = "00002"; // drone recebe/controle recebe telemetria

void Init_Comunication(){
    Serial.println("|Comunication| ---------- Iniciando configuração de interrupções ----------");

    // configura Timer 2
    timer2 = timerBegin(2, 80, true); // timer 2, prescaler 80 (1 us por tick), count up
    timerAttachInterrupt(timer2, &onTimer, true); 
    timerAlarmWrite(timer2, 100000, true); // 100000 us = 100 ms
    timerAlarmEnable(timer2);

    Serial.println("|Comunication| ---------- Configuração de interrupções finalizada ----------");
}

// Inicialização do ponteiro estático
Communication* Communication::instance = nullptr;

void IRAM_ATTR onTimer() {
    // verifica se o ponteiro está definido
    Communication* control =  Communication::getInstance();
    if(control->getIsControl())
        control->sendPing();
}

Communication::Communication(bool isControl) : 
    action(4), 
    power(0), 
    battery(100),
    altitude(0),
    isControl(isControl)
{
    begin(isControl);
}

Communication::~Communication()
{
}

Communication* Communication::getInstance(bool isControl)
{
    if (instance == nullptr) {
        instance = new Communication(isControl);
    }
    return instance;
}

void Communication::begin(bool isControl)
{
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_250KBPS);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();

    if(isControl) {
        // controle: envia comando, recebe telemetria
        radio.openWritingPipe(txAddress);
        radio.openReadingPipe(1, rxAddress);
        radio.stopListening(); // inicialmente transmissor
    } else {
        // drone: recebe comando, envia telemetria via ACK
        radio.openWritingPipe(rxAddress); // envia telemetria para controle
        radio.openReadingPipe(1, txAddress); // recebe comando
        radio.startListening(); // inicialmente receptor
    }
}

void Communication::setCommand(uint8_t newAction, uint8_t newPower)
{
    action = newAction;
    power = newPower;
}

uint8_t Communication::getAction()
{
    return action;
}

uint8_t Communication::getPower()
{
    return power;
}

void Communication::setTelemetry(uint8_t newBattery, int16_t newAltitude)
{
    battery = newBattery;
    altitude = newAltitude;
}

uint8_t Communication::getBattery()
{
    return battery;
}

int16_t Communication::getAltitude()
{
    return altitude;
}

const bool Communication::getIsControl()
{
    return isControl;
}

bool Communication::sendCommand()
{
    radio.stopListening(); // modo transmissor

    // Cria array com os dados do comando (action + power)
    uint8_t commandData[2] = {action, power};

    if (radio.write(commandData, sizeof(commandData))) {
        if (radio.isAckPayloadAvailable()) {
            // Lê telemetria: [battery, altitude_high_byte, altitude_low_byte]
            uint8_t telemetryData[3];
            radio.read(telemetryData, sizeof(telemetryData));

            battery = telemetryData[0];
            altitude = (telemetryData[1] << 8) | telemetryData[2]; // reconstrói int16_t
        }
        return true;
    }
    return false;
}

bool Communication::receiveCommand()
{
    if (radio.available()) {
        // Lê comando: [action, power]
        uint8_t commandData[2];
        radio.read(commandData, sizeof(commandData));

        action = commandData[0];
        power = commandData[1];

        // Prepara telemetria para envio: [battery, altitude_high_byte, altitude_low_byte]
        uint8_t telemetryData[3];
        telemetryData[0] = battery;
        telemetryData[1] = (altitude >> 8) & 0xFF; // byte alto
        telemetryData[2] = altitude & 0xFF;        // byte baixo

        // envia telemetria no ACK
        radio.writeAckPayload(1, telemetryData, sizeof(telemetryData));
        return true;
    }
    return false;
}

bool Communication::sendPing(uint8_t pingValue)
{
    radio.stopListening(); // modo transmissor

    // Cria array com valor de ping (1 byte)
    uint8_t pingData[1] = { pingValue };

    if (radio.write(pingData, sizeof(pingData))) {
        if (radio.isAckPayloadAvailable()) {
            // Lê telemetria retornada no ACK
            uint8_t telemetryData[3];
            radio.read(telemetryData, sizeof(telemetryData));

            battery = telemetryData[0];
            altitude = (telemetryData[1] << 8) | telemetryData[2];
        }
        return true;
    }
    return false;
}

