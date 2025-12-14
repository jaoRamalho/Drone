#include "Gyroscope.hpp"
#include "driver/timer.h"
#include "Arduino.h"
#include "GPIOS.h"

volatile bool Gyroscope::mpuInterrupt = false;
Gyroscope* Gyroscope::instance = nullptr;
Gyroscope::Gyroscope() : initSuccess(0), AcX(0), AcY(0), AcZ(0), Tmp(0), GyX(0), GyY(0), GyZ(0)
{
    Serial.println("|Gyroscope| ------- Iniciando MPU6050 ---------");
    configMPU6050();
    Serial.println("|Gyroscope| -------- MPU6050 iniciado com sucesso! ---------");
}

void Gyroscope::getData(int16_t* ax, int16_t* ay, int16_t* az, int16_t* temp, int16_t* gx, int16_t* gy, int16_t* gz){
    *ax = AcX;
    *ay = AcY;
    *az = AcZ;
    *temp = Tmp;
    *gx = GyX;
    *gy = GyY;
    *gz = GyZ;
}

void Gyroscope::loop(){    
    if (mpuInterrupt) {
        mpuInterrupt = false;
        readData();
    }
}

void Gyroscope::readData()
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);  // primeiro registrador de dados (AcX)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 14, 1);  // pede 14 bytes de uma vez

    int16_t tempAcX = Wire.read() << 8 | Wire.read();
    int16_t tempAcY = Wire.read() << 8 | Wire.read();
    int16_t tempAcZ = Wire.read() << 8 | Wire.read();
    int16_t tempTmp = Wire.read() << 8 | Wire.read();
    int16_t tempGyX = Wire.read() << 8 | Wire.read();
    int16_t tempGyY = Wire.read() << 8 | Wire.read();
    int16_t tempGyZ = Wire.read() << 8 | Wire.read();

    // Atualiza valores com valores fisicos
    AcX = (int16_t)((tempAcX / ACCEL_SCALE) * 100);
    AcY = (int16_t)((tempAcY / ACCEL_SCALE) * 100);
    AcZ = (int16_t)((tempAcZ / ACCEL_SCALE) * 100);
    Tmp = (int16_t)((tempTmp / 340.0 + 36.53) * 100); // em graus Celsius
    GyX = (int16_t)((tempGyX / GYRO_SCALE) * 100);
    GyY = (int16_t)((tempGyY / GYRO_SCALE) * 100);
    GyZ = (int16_t)((tempGyZ / GYRO_SCALE) * 100);

    // Serial.print("|Gyroscope| - AcX: "); Serial.print(AcX / 100.0);
    // Serial.print(" | AcY: "); Serial.print(AcY / 100.0);
    // Serial.print(" | AcZ: "); Serial.print(AcZ / 100.0);
    // Serial.print(" | Tmp: "); Serial.print(Tmp / 100.0);
    // Serial.print(" | GyX: "); Serial.print(GyX / 100.0);
    // Serial.print(" | GyY: "); Serial.print(GyY / 100.0);
    // Serial.print(" | GyZ: "); Serial.print(GyZ / 100.0);
    // Serial.println();

}

void Gyroscope::configMPU6050(){
    initSuccess = 1;
    uint8_t resultDebug = 0;
    // Acorda o MPU6050
    Serial.println("|Gyroscope| - Acordando o MPU6050...");
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B);      //registrador PWR_MGMT_1
    Wire.write(0x00);      // coloca em 0 → acorda o sensor
    resultDebug = Wire.endTransmission(true);
    vTaskDelay(5);
    if (resultDebug != 0) {
        Serial.print("|Gyroscope| - Erro ao acordar MPU6050! Código: ");
        Serial.println(resultDebug);
        initSuccess = 0;
    } else {
        Serial.println("|Gyroscope| - MPU6050 acordado!");
    }

    // Habilita interrupção de "data ready" no MPU6050
    Serial.println("|Gyroscope| - Configurando interrupção do MPU6050...");
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x38);   // registrador INT_ENABLE
    Wire.write(0x01);   // bit 0: DATA_RDY_EN
    resultDebug = Wire.endTransmission(true);
    vTaskDelay(5);
    if (resultDebug != 0) {
        Serial.print("|Gyroscope| - Erro ao configurar interrupção do MPU6050! Código: ");
        Serial.println(resultDebug);
        initSuccess = 0;
    } else {
        Serial.println("|Gyroscope| - Interrupção do MPU6050 configurada!");
    }

    // Configura taxa de trasmisao de dados para 250Hz
    Serial.println("|Gyroscope| - Configurando taxa de transmissão do MPU6050 para 1kHz...");
    // configurar DLPF para usar output rate 1 kHz
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x1A);      // CONFIG (DLPF_CFG)
    Wire.write(0x03);      // DLPF_CFG = 3 (exemplo)
    Wire.endTransmission(true);
    vTaskDelay(5);
    if (resultDebug != 0) {
        Serial.print("|Gyroscope| - Erro ao configurar taxa de transmissão do MPU6050! Código: ");
        Serial.println(resultDebug);
        initSuccess = 0;
    } else {
        Serial.println("|Gyroscope| - Taxa de transmissão do MPU6050 configurada!");
    }

    // sample rate = 1kHz / (1 + SMPLRT_DIV)
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x19);      // SMPLRT_DIV
    Wire.write(0x01);      // SMPLRT_DIV = 0 -> 1kHz
    Wire.endTransmission(true);
    vTaskDelay(5);
    if (resultDebug != 0) {
        Serial.print("|Gyroscope| - Erro ao configurar taxa de transmissão do MPU6050! Código: ");
        Serial.println(resultDebug);
        initSuccess = 0;
    } else {
        Serial.println("|Gyroscope| - Taxa de transmissão do MPU6050 configurada!");
    }

    if (initSuccess) {
        Serial.println("|Gyroscope| - MPU6050 configurado com sucesso!");
    } else {
        Serial.println("|Gyroscope| - Erro na configuração do MPU6050!");
    }
}

Gyroscope::~Gyroscope() {

}

Gyroscope* Gyroscope::Init_Gyroscope() {
    if (instance == nullptr) {
        Serial.println("|Gyroscope| - Iniciando I2C para MPU6050...");
        Wire.begin(PIN_SDA, PIN_SCL);
        vTaskDelay(5);
        Serial.println("|Gyroscope| - I2C iniciado com sucesso!");

        instance = new Gyroscope();
    }
    return instance;
}