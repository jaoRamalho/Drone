#include "Comunication.hpp"

RF24 radio(CE_PIN, CSN_PIN);

// Endereços (podem ser parametrizados depois)
const uint8_t txAddress[6] = "00001"; // controle envia/drone envia telemetria
const uint8_t rxAddress[6] = "00002"; // drone recebe/controle recebe telemetria

volatile bool sendPingFlag = false;

unsigned long lastCommandMillis = 0;
const unsigned long COMMAND_INTERVAL = 5000UL; // 5 segundos

// Inicialização do ponteiro estático
Communication* Communication::instance = nullptr;

Communication::Communication() :
    battery(100),
    altitude(0),
    action(0),
    power(0),
    lastReceivedSeq(0xFFFF) // inválido inicialmente
{
}

// Helper: checksum simples XOR
uint8_t Communication::calcChecksum(const uint8_t* data, size_t len) {
    uint8_t cs = 0;
    for (size_t i = 0; i < len; ++i) cs ^= data[i];
    return cs;
}

bool Communication::verifyChecksum(const uint8_t* data, size_t len) {
    if (len == 0) return false;
    uint8_t cs = calcChecksum(data, len - 1);
    return cs == data[len - 1];
}

Communication::~Communication()
{
}

Communication* Communication::getInstance()
{
    if (instance == nullptr) {
        instance = new Communication();
    }
    return instance;
}

void Communication::begin()
{
    Serial.println("|Comunication| Iniciando rádio no drone...");
    radio.begin();
    // radio.setDataRate(RF24_250KBPS);
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    radio.setPALevel(RF24_PA_MIN);

    // drone: recebe comando, envia telemetria via ACK
    radio.openWritingPipe(rxAddress); // envia telemetria para controle
    radio.openReadingPipe(1, txAddress); // recebe comando
    radio.startListening();

    if (!radio.isChipConnected()) {
        Serial.println("ERRO: RF24 não conectado!");
        return;
    }
    Serial.println("|Comunication| Rádio iniciado no drone.");
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


void Communication::receiveCommand()
{
    if (radio.available()) {
        // Lê pacote (5 bytes esperado). Use payloadSize se dinâmico.
        uint8_t cmd[5];
        radio.read(cmd, sizeof(cmd));

        // Verifica checksum
        if (!verifyChecksum(cmd, sizeof(cmd))) {
            Serial.println("|Receptor| Pacote corrompido (checksum inválido). Ignorando.");
            yield();
            return;
        }

        uint16_t seq = (uint16_t(cmd[0]) << 8) | cmd[1];
        uint8_t receivedAction = cmd[2];
        uint8_t receivedPower = cmd[3];

        bool isDuplicate = (seq == lastReceivedSeq);

        if (receivedAction == 255 && receivedPower == 0) {
            Serial.println("|Receptor| Pacote de ping recebido.");
        } else if (isDuplicate) {
            Serial.print("|Receptor| Pacote duplicado seq ");
            Serial.println(seq);
        } else {
            // novo pacote — atualiza ação e potência
            action = receivedAction;
            power = receivedPower;
            lastReceivedSeq = seq;

            Serial.print("Ação = ");
            Serial.print(action);
            Serial.print(", Potência = ");
            Serial.print(power);
            Serial.print(", Seq = ");
            Serial.println(seq);
            // Aqui você aplicaria o comando ao drone ou enfileiraria para processamento
        }

        // Prepara telemetria para envio: seq_hi, seq_lo, battery, alt_hi, alt_lo, statusFlags, checksum
        uint8_t ackPayload[7];
        ackPayload[0] = (uint8_t)((seq >> 8) & 0xFF);
        ackPayload[1] = (uint8_t)(seq & 0xFF);
        ackPayload[2] = battery;
        ackPayload[3] = (uint8_t)((altitude >> 8) & 0xFF);
        ackPayload[4] = (uint8_t)(altitude & 0xFF);
        ackPayload[5] = 0x00; // statusFlags (reservado)
        ackPayload[6] = calcChecksum(ackPayload, 6);

        // envia telemetria no ACK (pipe 1)
        radio.writeAckPayload(1, ackPayload, sizeof(ackPayload));
    }

    yield();
}

