#include <Arduino.h>
#include "ISR.hpp"
#include "Gyroscope.hpp"
// #include "PWM.hpp"
#include "Control.hpp"
#include "Gestos.hpp"
#include "Comunication.hpp"

Communication* controle;
Gyroscope* gyro;
Control* control;
Gestos* gestos;

void setup() {
  Serial.begin(9600);

  Serial.println("| MAIN | ---------- Iniciando setup --------");

  // gyro = Gyroscope::Init_Gyroscope();
 
  // control = Control::Init_Control();

  gestos = Gestos::Init_Gestos();

  controle = Communication::getInstance();

  controle->begin();
  Init_Comunication();

  Init_ISR();

  vTaskDelay(10);
}

void loop() {
  // gyro->loop();
  
  // control->loop();

  int act = gestos->loop();

  if (act != 0){
    controle->setCommand(act, controle->getPower());
  }
  controle->sendThing();
    vTaskDelay(1);
}


/*---------------------------------------------------------------- RECEPTOR(ARDUINO) -------------------------------------------------------------------*/
// #include "Comunication.hpp"

// Communication* drone = Communication::getInstance(false);

// void setup() {
//   Serial.begin(9600);
//   drone->begin();
// }

// void loop() {
//   drone->receiveCommand();
//   delay(300);
// }
/*---------------------------------------------------------------- RECEPTOR(ARDUINO) -------------------------------------------------------------------*/


/*--------------------------------------------------------------- TRANSMISSOR(ESP32) ------------------------------------------------------------------*/
// #include "Comunication.hpp"

// Communication* controle = Communication::getInstance(true);

// void setup() {
//   Serial.begin(9600);
//   controle->begin();
//   Init_Comunication();
// }

// void loop() {
//   controle->sendThing();
//   delay(300);
// }
/*--------------------------------------------------------------- TRANSMISSOR(ESP32) ------------------------------------------------------------------*/
