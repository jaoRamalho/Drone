#ifndef GYROSCOPE_HPP
#define GYROSCOPE_HPP

#include <Wire.h>

#include "GPIOS.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define GYRO_ADDRESS 0x68 // Endereço I2C típico do MPU6050

class Gyroscope {
private:
    Adafruit_MPU6050 mpu;
    sensors_event_t acc, gyro, temp;

    static Gyroscope* instance;
    Gyroscope();
public:
    ~Gyroscope();
    static Gyroscope* getInstance();

    void readGyroData(float &gx, float &gy, float &gz);
};


#endif // GYROSCOPE_HPP
