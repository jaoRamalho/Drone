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
  Command it = Command::NONE;
  // avança para o próximo comando a cada 500 ms
  static uint32_t _lastMillis = 0;
  static int _cmdIndex = static_cast<int>(Command::NONE);
  uint32_t _now = millis();
  if (_now - _lastMillis >= 500) {
    _lastMillis = _now;
    _cmdIndex++;
    // ajuste CMD_MAX para o maior valor do enum Command do seu projeto
    constexpr int CMD_MAX = 11;
    if (_cmdIndex > CMD_MAX) _cmdIndex = 0;
  }
  Command act = static_cast<Command>(_cmdIndex);
  // Command act = static_cast<Command>(gestos->loop());

  if (act != Command::NONE) {
    controle->setAction(act);
  }
  controle->sendThing();
  vTaskDelay(1);
}