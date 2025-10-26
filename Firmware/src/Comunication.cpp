#include "Comunication.hpp"

RF24 radio(CE_PIN, CSN_PIN);

// Endereços (podem ser parametrizados depois)
const uint8_t txAddress[6] = "00001"; // controle envia/drone envia telemetria
const uint8_t rxAddress[6] = "00002"; // drone recebe/controle recebe telemetria

volatile bool sendPingFlag = false;

unsigned long lastCommandMillis = 0;
const unsigned long COMMAND_INTERVAL = 5000UL; // 5 segundos

/*----------------------------------- ESP32 ------------------------------------------------------*/
// hw_timer_t *timer2 = nullptr;

// void Init_Comunication(){
//     Serial.println("|Comunication| ---------- Iniciando configuração de interrupções ----------");

//     // configura Timer 2
//     timer2 = timerBegin(2, 80, true); // timer 2, prescaler 80 (1 us por tick), count up
//     timerAttachInterrupt(timer2, &onTimer, true);
//     timerAlarmWrite(timer2, 100000, true); // 100000 us = 100 ms
//     timerAlarmEnable(timer2);

//     Serial.println("|Comunication| ---------- Configuração de interrupções finalizada ----------");
// }

// void IRAM_ATTR onTimer() {
//     // verifica se o ponteiro está definido
//     if(!sendPingFlag) sendPingFlag = true;
// }
/*----------------------------------- ESP32 ------------------------------------------------------*/


// Inicialização do ponteiro estático
Communication* Communication::instance = nullptr;

Communication::Communication(bool isControl) : 
    action(4), 
    power(10), 
    battery(100),
    altitude(0),
    isControl(isControl)
{
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

void Communication::begin()
{
    Serial.println("|Comunication| Iniciando rádio...");
    radio.begin();
    // radio.setDataRate(RF24_250KBPS);
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    radio.setPALevel(RF24_PA_MIN);

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

    if (!radio.isChipConnected()) {
        Serial.println("ERRO: RF24 não conectado!");
        return;
    }
    Serial.println("|Comunication| Rádio iniciado.");
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

void Communication::sendCommand()
{
    // Cria array com os dados do comando (action + power)
    uint8_t commandData[2] = {action, power};

    if (radio.write(commandData, sizeof(commandData))) {
        if (radio.isAckPayloadAvailable()) {
            // Lê telemetria: [battery, altitude_high_byte, altitude_low_byte]
            uint8_t telemetryData[3];
            radio.read(telemetryData, sizeof(telemetryData));

            Serial.print("Bateria: ");
            Serial.print(telemetryData[0]);
            Serial.print("%, Altitude: ");
            Serial.print((telemetryData[1] << 8) | telemetryData[2]);
            Serial.println(" cm");

            battery = telemetryData[0];
            altitude = (telemetryData[1] << 8) | telemetryData[2]; // reconstrói int16_t
        }
    }
}


void Communication::sendThing()
{
    unsigned long now = millis();

    // Quando é hora de enviar o comando principal?
    if (now - lastCommandMillis >= COMMAND_INTERVAL) {
        sendCommand();
        lastCommandMillis = now;
        // NÃO usamos delay longo aqui — apenas registramos o envio
    }

    // Calcular quanto falta para o próximo comando
    unsigned long timeSinceLast = now - lastCommandMillis;
    unsigned long timeToNext = (timeSinceLast >= COMMAND_INTERVAL) ? 0 : (COMMAND_INTERVAL - timeSinceLast);

    // Janela de silêncio antes do comando principal para evitar colisões
    const unsigned long SILENCE_WINDOW_MS = 200UL; // ajuste conforme necessário

    // Processa ping apenas se a flag estiver setada e estivermos fora da janela de silêncio
    if (sendPingFlag && timeToNext > SILENCE_WINDOW_MS) {
        sendPingFlag = false; // consome a flag
        sendPing();
    }

    // Evita 100% CPU: libera o processador por um curto instante.
    // Em ESP32/Arduino use `yield()` para entregar o tempo ao RTOS; se preferir, diminua para delay(1).
    yield();
}


void Communication::receiveCommand()
{
    if (radio.available()) {
        // Lê comando: [action, power]
        uint8_t cmd[2];
        radio.read(cmd, sizeof(cmd));

        if(cmd[0] != 255 && cmd[1] != 0) {
            Serial.print("Ação = "); 
            Serial.print(cmd[0]);
            Serial.print(", Potência = "); 
            Serial.println(cmd[1]);
        } else {
            Serial.println("|Comunication| Ping recebido.");
        }

        action = cmd[0];
        power = cmd[1];

        // Prepara telemetria para envio: [battery, altitude_high_byte, altitude_low_byte]
        uint8_t telemetryData[3];
        telemetryData[0] = battery;
        telemetryData[1] = (altitude >> 8) & 0xFF; // byte alto
        telemetryData[2] = altitude & 0xFF;        // byte baixo

        // envia telemetria no ACK
        radio.writeAckPayload(1, telemetryData, sizeof(telemetryData));
    }

    yield();
}

void Communication::sendPing(uint8_t pingValue)
{
    // Cria array com valor de ping (1 byte)
    uint8_t zero = 0;
    uint8_t pingData[2] = { pingValue, zero };

    if (radio.write(pingData, sizeof(pingData))) {
        if (radio.isAckPayloadAvailable()) {
            // Lê telemetria retornada no ACK
            uint8_t telemetryData[3];
            radio.read(telemetryData, sizeof(telemetryData));

            Serial.print("|Comunication| Ping ACK recebido. ");
            Serial.print("Bateria: ");
            Serial.print(telemetryData[0]);
            Serial.print("%, Altitude: ");
            Serial.print((telemetryData[1] << 8) | telemetryData[2]);
            Serial.println(" cm");

            battery = telemetryData[0];
            altitude = (telemetryData[1] << 8) | telemetryData[2];
        }
    }
}

