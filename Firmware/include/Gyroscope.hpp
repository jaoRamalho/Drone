#ifndef GYROSCOPE_HPP
#define GYROSCOPE_HPP

#include <Wire.h>

#include "GPIOS.h"

#define MPU_ADDR 0x68 // Endereço I2C típico do MPU6050

// Fatores de conversão (datasheet do MPU6050)
const float ACCEL_SCALE = 16384.0;
const float GYRO_SCALE  = 131.0;

extern volatile bool mpuInterrupt;

class Gyroscope {
private:
    int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
    uint8_t initSuccess;

    void configMPU6050();

    static Gyroscope* instance;
    void readData();
    Gyroscope();
public:
    static Gyroscope* Init_Gyroscope();
    ~Gyroscope();

    void getData(int16_t* ax, int16_t* ay, int16_t* az, int16_t* temp, int16_t* gx, int16_t* gy, int16_t* gz);
    void loop();
};

#endif // GYROSCOPE_HPP
