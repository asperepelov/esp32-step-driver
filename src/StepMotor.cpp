#include "StepMotor.h"

StepMotor::StepMotor(uint8_t stepPin, uint8_t directionPin, uint8_t enablePin, uint32_t pulseDurationMicSec)
    : _stepPin(stepPin), _directionPin(directionPin), _enablePin(enablePin), _pulseDurationMicSec(pulseDurationMicSec)
{
    pinMode(_stepPin, OUTPUT);
    pinMode(_directionPin, OUTPUT);
    pinMode(_enablePin, OUTPUT);
}

void StepMotor::enable(){
  digitalWrite(_enablePin, LOW); // Включение драйвера двигателя
}

void StepMotor::disable(){
  digitalWrite(_enablePin, HIGH); // Отключение драйвера двигателя
} 

void StepMotor::pulse() {
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(_pulseDurationMicSec);
  digitalWrite(_stepPin, LOW);
  delayMicroseconds(_pulseDurationMicSec);
}

void StepMotor::turn(int pulseCnt, TurnDirection dir) {
  digitalWrite(_directionPin, uint8_t(dir));
  for(int i = 0; i < pulseCnt; i++) {
    pulse();
  }
}