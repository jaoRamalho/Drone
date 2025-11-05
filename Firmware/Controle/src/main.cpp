#include <Arduino.h>
#include "Gestos.hpp"
#include "Comunication.hpp"

Communication* controle;
Gestos* gestos;

void setup() {
  Serial.begin(9600);

  Serial.println("| MAIN | ---------- Iniciando setup --------");

  gestos = Gestos::Init_Gestos();

  controle = Communication::getInstance();

  controle->begin();

  Init_Timers();

  Serial.println("| MAIN | -------- Setup concluído ---------");

  vTaskDelay(10);
}

void loop() {
  int act = gestos->loop();

  if (act != 0){
    controle->setCommand(act, controle->getPower());
  }
  controle->sendThing();
  vTaskDelay(1);
}