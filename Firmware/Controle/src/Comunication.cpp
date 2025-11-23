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
    action(Command::NONE),
    power(100),
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
bool Communication::transmitWithRetries(const uint8_t *pkt, size_t pktLen, uint32_t* rttUsOut) {    
    if (rttUsOut) *rttUsOut = 0;
    unsigned long start = 0;
    uint32_t startUs = 0;
    for (uint8_t attempt = 0; attempt < maxRetries; ++attempt) {
        // marca o início imediatamente antes de escrever
        startUs = micros();

        if (radio.write(pkt, pktLen)) {
            // aguarda ack payload ficar disponível até ackTimeoutMs
            start = millis();
            while ((millis() - start) < ackTimeoutMs) {
                if (radio.isAckPayloadAvailable()) {
                    // marca o tempo assim que o ACK payload fica disponível
                    uint32_t endUs = micros();
                    if (rttUsOut) {
                        // micros() é unsigned long (wrap é tratado automaticamente)
                        *rttUsOut = endUs - startUs;
                    }
                    return true; // ack disponível — chamador lerá o payload
                }
                yield();
            }
            // se chegou aqui, ack não chegou dentro do timeout -> tentar novamente
        }
        // pequena pausa entre tentativas pode ajudar (opcional)
        // delay(1);
    }
    // nenhum ack recebido após retries
    if (rttUsOut) {
        *rttUsOut = 0; // ou use UINT32_MAX para sinalizar "sem resposta"
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

void Communication::setAction(const Command newAction)
{
    if(action != newAction){
        newCommandAvailable = true;
    }
    action = newAction;
}

const Command Communication::getAction() const
{
    return action;
}

void Communication::setPower(const uint8_t newPower)
{
    uint8_t clampedPower = (newPower > 100) ? 100 : newPower;
    if(power != clampedPower){
        newCommandAvailable = true;
    }
    power = clampedPower;
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

    uint32_t rttUs = 0;
    bool ackAvailable = transmitWithRetries(pkt, sizeof(pkt), &rttUs);
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
                Serial.print("|Transmissor| Bateria = ");
                Serial.print(battery);
                Serial.print("%, Altitude = ");
                Serial.print(altitude);
                Serial.print(" cm, Latência = ");
                Serial.print(rttUs);
                Serial.println(" us");

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
    // unsigned long now = millis();

    if (newCommandAvailable) {
        sendCommand();
        newCommandAvailable = false;
    }

    if (sendPingFlag) {
        sendPingFlag = false;
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

    uint32_t rttUs = 0;
    bool ackAvailable = transmitWithRetries(pkt, sizeof(pkt), &rttUs);
    if (ackAvailable) {
        uint8_t ack[7];
        radio.read(ack, sizeof(ack));
        if (verifyChecksum(ack, sizeof(ack))) {
            uint16_t ackSeq = (uint16_t(ack[0]) << 8) | ack[1];
            battery = ack[2];
            altitude = (int16_t)((int16_t(ack[3]) << 8) | ack[4]);
            lastAckedSeq = ackSeq;
            Serial.print("|Transmissor| Bateria = ");
            Serial.print(battery);
            Serial.print("%, Altitude = ");
            Serial.print(altitude);
            Serial.print(" cm");
            Serial.print(", Latência = ");
            Serial.print(rttUs);
            Serial.println(" us");
        } else {
            Serial.println("|Transmissor| Ping ACK corrompido.");
        }
    } else {
        Serial.println("|Transmissor| Ping ACK não recebido.");
    }
}

