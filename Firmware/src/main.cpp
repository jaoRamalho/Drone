#include <Arduino.h>
#include "ISR.hpp"
#include "Gyroscope.hpp"

Gyroscope* gyro;

void setup() {
  Serial.begin(115200);

  gyro = Gyroscope::Init_Gyroscope();
  Init_ISR();

  vTaskDelay(10);
}

void loop() {
  gyro->loop();
 
  vTaskDelay(10);
}