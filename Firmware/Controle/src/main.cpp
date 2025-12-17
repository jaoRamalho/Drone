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
  
  MoveCommand act = static_cast<MoveCommand>(gestos->loop());
  Serial.print("Gesto lido: ");
  Serial.println(static_cast<uint8_t>(act));

  if (act != MoveCommand::NONE) {
    controle->setAction(act);
  }
  controle->sendThing();
  vTaskDelay(1);
}