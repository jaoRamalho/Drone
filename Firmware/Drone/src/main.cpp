#include <Arduino.h>
#include "ISR.hpp"
#include "Gyroscope.hpp"
#include "Control.hpp"
#include "Comunication.hpp"
#include "Timer.hpp"

Communication *receiver = nullptr;
Gyroscope *gyro = nullptr;
Control *control = nullptr;

void setup(){

  Serial.begin(9600);

  Serial.println("| MAIN | ---------- Iniciando setup --------");

  gyro = Gyroscope::Init_Gyroscope();

  control = Control::Init_Control();

  receiver = Communication::getInstance();
  receiver->begin();

  Init_Timer();

  Init_ISR();

  Serial.println("| MAIN | ---------- Setup finalizado --------");

  vTaskDelay(10);
}

void loop(){
  receiver->receiveCommand();

  control->setMovementCommand(receiver->getAction());

  control->loop();
  
  vTaskDelay(1);
}