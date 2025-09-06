#include "Gyroscope.hpp"

Gyroscope* Gyroscope::instance = nullptr;
Gyroscope::Gyroscope() {
    uint32_t initTime = millis();
    while(!mpu.begin(GYRO_ADDRESS)){
        Serial.println("Failed to find MPU6050 chip");
        if (millis() - initTime > 1000) { // Timeout after 5 segundos
            Serial.println("MPU6050 initialization timed out!");
            return;
        }
    }
    Serial.println("MPU6050 Found!");

}

Gyroscope::~Gyroscope() {
    // Destrutor
}

Gyroscope* Gyroscope::getInstance() {
    if (instance == nullptr) {
        instance = new Gyroscope();
    }
    return instance;
}

void Gyroscope::readGyroData(float &gx, float &gy, float &gz){
    mpu.getEvent(&acc, &gyro, &temp);
    gx = gyro.gyro.x;
    gy = gyro.gyro.y;
    gz = gyro.gyro.z;
}
