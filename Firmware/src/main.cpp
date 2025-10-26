#include <Arduino.h>
#include "ISR.hpp"
#include "Gyroscope.hpp"
// #include "PWM.hpp"
#include "Control.hpp"

Gyroscope* gyro;
Control* control;

void setup() {
  Serial.begin(115200);

  Serial.println("| MAIN | ---------- Iniciando setup --------");

  gyro = Gyroscope::Init_Gyroscope();
 
  control = Control::Init_Control();

  Init_ISR();

  vTaskDelay(10);
}

void loop() {
  gyro->loop();
  
  control->loop();
  
  vTaskDelay(1);
}

/*------------------------------------------------------------------- RECEPTOR -------------------------------------------------------------------*/
// #include "Comunication.hpp"

// Communication* drone = Communication::getInstance(false);

// void setup() {
//   Serial.begin(9600);
//   drone->begin();
// }

// void loop() {
//   drone->receiveCommand();
// }
/*------------------------------------------------------------------- RECEPTOR -------------------------------------------------------------------*/


/*------------------------------------------------------------------ TRANSMISSOR ------------------------------------------------------------------*/
// #include "Comunication.hpp"

// Communication* controle = Communication::getInstance(true);

// void setup() {
//   Serial.begin(9600);
//   controle->begin();
//   Init_Comunication();
// }

// void loop() {
//   controle->sendThing();
// }
/*------------------------------------------------------------------ TRANSMISSOR ------------------------------------------------------------------*/
