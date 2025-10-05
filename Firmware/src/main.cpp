#include <Arduino.h>
#include "ISR.hpp"
#include "Gyroscope.hpp"
#include "PWM.hpp"
#include "Control.hpp"

Gyroscope* gyro;
Control* control;

void setup() {
  Serial.begin(115200);

  //gyro = Gyroscope::Init_Gyroscope();

  control = Control::Init_Control();

  Init_ISR();

  vTaskDelay(10);
}

void loop() {

  //gyro->loop();
  
  control->loop();
  
  vTaskDelay(1);
}