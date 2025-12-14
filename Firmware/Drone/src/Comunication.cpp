#include "Comunication.hpp"

RF24 radio(CE_PIN, CSN_PIN);

// Endereços (podem ser parametrizados depois)
const uint8_t txAddress[6] = "00001"; // controle envia/drone envia telemetria
const uint8_t rxAddress[6] = "00002"; // drone recebe/controle recebe telemetria

volatile bool sendPingFlag = false;

unsigned long now = 0;
unsigned long lastCommandReceivedMillis = 0;

int potenciaMovemento = 50;
int potenciaAscDesc = 80;

// Inicialização do ponteiro estático
Communication* Communication::instance = nullptr;

Communication::Communication() :
    battery(100),
    altitude(0),
    action(MoveCommand::NONE),
    state(0),
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
    Serial.println("| Receptor | ---------- Iniciando Rádio --------");
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
        Serial.println("| Receptor | ERRO: RF24 não conectado!");
        return;
    }
    Serial.println("| Receptor | ---------- Rádio Iniciado ----------");
}

void Communication::setBattery(const uint8_t newBattery)
{
    battery = newBattery;
}

const uint8_t Communication::getBattery() const
{
    return battery;
}

void Communication::setAltitude(const int16_t newAltitude)
{
    altitude = newAltitude;
}

const int16_t Communication::getAltitude() const
{
    return altitude;
}

void Communication::setState(const uint8_t newState)
{
    state = newState;
}

const uint8_t Communication::getState() const
{
    return state;
}

void Communication::receiveCommand()
{
    if (radio.available()) {
        // Lê pacote (4 bytes esperado).
        uint8_t cmd[4];
        radio.read(cmd, sizeof(cmd));

        if (lastCommandReceivedMillis != 0) {
            now = millis();
            if (now - lastCommandReceivedMillis > CONNECTION_TIMEOUT) {
                Serial.println("| Receptor | - [EMERGÊNCIA] Sinal perdido - iniciando pouso seguro!");
                // função para pouso seguro (a ser implementada)
                return;
            }
        }

        // Verifica checksum
        if (!verifyChecksum(cmd, sizeof(cmd))) {
            Serial.println("| Receptor | - Pacote corrompido (checksum inválido). Ignorando.");
            yield();
            return;
        }

        uint16_t seq = (uint16_t(cmd[0]) << 8) | cmd[1];
        MoveCommand receivedAction = (MoveCommand) cmd[2];
        
        bool isDuplicate = (seq == lastReceivedSeq);

        lastCommandReceivedMillis = millis(); // Atualiza o tempo da última comunicação

        if (receivedAction != MoveCommand::NONE) {
            if (isDuplicate) {
                Serial.print("| Receptor | - Pacote duplicado seq ");
                Serial.println(seq);
            } else {
                // novo pacote — atualiza ação e potência
                action = receivedAction;
                lastReceivedSeq = seq;

                Serial.print("| Receptor | Ação = ");
                Serial.println(static_cast<int>(action));
                
                // Aqui você aplicaria o comando ao drone ou enfileiraria para processamento

            }
        } else {
            //Serial.println("| Receptor | Pacote de ping recebido.");
        }

        setState(Control::Init_Control()->getState());

        // Prepara telemetria para envio: seq_hi, seq_lo, battery, alt_hi, alt_lo, state, statusFlags, checksum
        uint8_t ackPayload[8];
        ackPayload[0] = (uint8_t)((seq >> 8) & 0xFF);
        ackPayload[1] = (uint8_t)(seq & 0xFF);
        ackPayload[2] = battery;
        ackPayload[3] = (uint8_t)((altitude >> 8) & 0xFF);
        ackPayload[4] = (uint8_t)(altitude & 0xFF);
        ackPayload[5] = state;
        ackPayload[6] = 0x00; // statusFlags (reservado)
        ackPayload[7] = calcChecksum(ackPayload, sizeof(ackPayload) - 1);

        if (battery <= MIN_SAFE_BATTERY) {
            Serial.println("| Receptor | - [AVISO] Bateria baixa - pouso automático!");
            // função para pouso seguro (a ser implementada)
        }

        // envia telemetria no ACK (pipe 1)
        radio.writeAckPayload(1, ackPayload, sizeof(ackPayload));
    }

    yield();
}

