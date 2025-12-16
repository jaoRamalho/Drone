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
  MoveCommand it = MoveCommand::NONE;
  // avança para o próximo comando a cada 500 ms
  static uint32_t _lastMillis = 0;
  static int _cmdIndex = static_cast<int>(MoveCommand::NONE);
  uint32_t _now = millis();
  if (_now - _lastMillis >= 500) {
    _lastMillis = _now;
    _cmdIndex++;
    // ajuste CMD_MAX para o maior valor do enum MoveCommand do seu projeto
    constexpr int CMD_MAX = 11;
    if (_cmdIndex > CMD_MAX) _cmdIndex = 0;
  }
  MoveCommand act = static_cast<MoveCommand>(_cmdIndex);
  // MoveCommand act = static_cast<MoveCommand>(gestos->loop());

  if (act != MoveCommand::NONE) {
    controle->setAction(act);
  }
  controle->sendThing();
  vTaskDelay(1);
}