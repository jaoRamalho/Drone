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
    action(4),
    power(10),
    battery(0),
    altitude(0),
    seqCounter(1),
    lastAckedSeq(0),
    lastReceivedSeq(0xFFFF), // inválido inicialmente
    maxRetries(3),
    ackTimeoutMs(200),
    newCommandAvailable(false)
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

// Helper: transmite com retries e espera por ACK (o ack payload é lido por radio.read)
bool Communication::transmitWithRetries(const uint8_t* pkt, size_t pktLen) {
    unsigned long start = 0;
    for (uint8_t attempt = 0; attempt < maxRetries; ++attempt) {
        if (radio.write(pkt, pktLen)) {
            // aguarda ack payload ficar disponível até ackTimeoutMs
            start = millis();
            while ((millis() - start) < ackTimeoutMs) {
                if (radio.isAckPayloadAvailable()) {
                    return true; // ack disponível — chamador lerá o payload
                }
                // pequena espera cooperativa
                yield();
            }
            // se chegou aqui, ack não chegou dentro do timeout -> tentar novamente
        }
    }
    return false;
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
    Serial.println("|Comunication| Iniciando rádio...");
    radio.begin();
    // radio.setDataRate(RF24_250KBPS);
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    radio.setPALevel(RF24_PA_MIN);

    // controle: envia comando, recebe telemetria
    radio.openWritingPipe(txAddress);
    radio.openReadingPipe(1, rxAddress);
    radio.stopListening();

    if (!radio.isChipConnected()) {
        Serial.println("ERRO: RF24 não conectado!");
        return;
    }
    Serial.println("|Comunication| Rádio iniciado.");
}

void Communication::setCommand(uint8_t newAction, uint8_t newPower)
{
    if(action != newAction){
        newCommandAvailable = true;
    }
    action = newAction;
    power = newPower;
}

const uint8_t Communication::getAction() const
{
    return action;
}

const uint8_t Communication::getPower() const
{
    return power;
}

void Communication::sendCommand()
{
    // Monta pacote: seq_hi, seq_lo, action, power, checksum
    uint8_t pkt[5];
    pkt[0] = (uint8_t)((seqCounter >> 8) & 0xFF);
    pkt[1] = (uint8_t)(seqCounter & 0xFF);
    pkt[2] = action;
    pkt[3] = power;
    pkt[4] = calcChecksum(pkt, 4);

    bool ackAvailable = transmitWithRetries(pkt, sizeof(pkt));
    if (ackAvailable) {
        // Lê o ACK payload esperado (formato: seq_hi, seq_lo, battery, alt_hi, alt_lo, statusFlags, checksum)
        uint8_t ack[7];
        radio.read(ack, sizeof(ack));
        if (verifyChecksum(ack, sizeof(ack))) {
            uint16_t ackSeq = (uint16_t(ack[0]) << 8) | ack[1];
            if (ackSeq == seqCounter) {
                // Confirma telemetria
                battery = ack[2];
                altitude = (int16_t)((int16_t(ack[3]) << 8) | ack[4]);
                lastAckedSeq = ackSeq;
                // avançar sequência para próximo comando
                seqCounter++;
                Serial.print("|Transmissor| ACK válido recebido. Bateria = ");
                Serial.print(battery);
                Serial.print("%, Altitude = ");
                Serial.print(altitude);
                Serial.print(" cm");

                if (battery <= MIN_SAFE_BATTERY) {
                    Serial.println("[AVISO] Bateria baixa - pouso automático!");
                }

                return;
            } else {
                Serial.print("|Transmissor| ACK seq mismatch (esperado ");
                Serial.print(seqCounter);
                Serial.print(", recebido ");
                Serial.print(ackSeq);
                Serial.println(")");
            }
        } else {
            Serial.println("|Transmissor| ACK corrompido (checksum inválido).");
        }
    } else {
        Serial.println("|Transmissor| ACK não recebido após retries.");
    }
}


void Communication::sendThing()
{
    unsigned long now = millis();

    if (now - lastCommandMillis >= COMMAND_INTERVAL || newCommandAvailable) {
        sendCommand();
        lastCommandMillis = now;
        newCommandAvailable = false;
    }

    // Calcular quanto falta para o próximo comando
    unsigned long timeSinceLast = now - lastCommandMillis;
    unsigned long timeToNext = (timeSinceLast >= COMMAND_INTERVAL) ? 0 : (COMMAND_INTERVAL - timeSinceLast);

    // Janela de silêncio antes do comando principal para evitar colisões
    const unsigned long SILENCE_WINDOW_MS = 200UL; // 200 ms antes do comando

    // Processa ping apenas se a flag estiver setada e estivermos fora da janela de silêncio
    if (sendPingFlag && timeToNext > SILENCE_WINDOW_MS) {
        sendPingFlag = false; // consome a flag
        sendPing();
    }

    // Evita 100% CPU: libera o processador por um curto instante.
    yield();
}

void Communication::sendPing(uint8_t pingValue)
{
    if (!isControl) return;

    // Usamos seqCounter mas não incrementamos; ping usa action=pingValue e power=0
    uint8_t pkt[5];
    pkt[0] = (uint8_t)((seqCounter >> 8) & 0xFF);
    pkt[1] = (uint8_t)(seqCounter & 0xFF);
    pkt[2] = pingValue;
    pkt[3] = 0;
    pkt[4] = calcChecksum(pkt, 4);

    radio.stopListening();

    bool ackAvailable = transmitWithRetries(pkt, sizeof(pkt));
    if (ackAvailable) {
        uint8_t ack[7];
        radio.read(ack, sizeof(ack));
        if (verifyChecksum(ack, sizeof(ack))) {
            uint16_t ackSeq = (uint16_t(ack[0]) << 8) | ack[1];
            battery = ack[2];
            altitude = (int16_t)((int16_t(ack[3]) << 8) | ack[4]);
            lastAckedSeq = ackSeq;
            Serial.print("|Transmissor| Ping ACK seq ");
            Serial.print(ackSeq);
            Serial.print(" bateria ");
            Serial.print(battery);
            Serial.print("% alt ");
            Serial.print(altitude);
            Serial.println(" cm");
        } else {
            Serial.println("|Transmissor| Ping ACK corrompido.");
        }
    } else {
        Serial.println("|Transmissor| Ping ACK não recebido.");
    }
}

