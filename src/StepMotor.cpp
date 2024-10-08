#include "StepMotor.h"

StepMotor::StepMotor(uint8_t stepPin, uint8_t directionPin, uint8_t enablePin, uint32_t pulseMinDurationMicSec, uint32_t pulseMaxDurationMicSec)
    : _stepPin(stepPin), _directionPin(directionPin), _enablePin(enablePin), _pulseMinDurationMicSec(pulseMinDurationMicSec), _pulseMaxDurationMicSec(pulseMaxDurationMicSec)
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

void StepMotor::pulse(uint32_t pulseDurationMicSec) {
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(pulseDurationMicSec);
  digitalWrite(_stepPin, LOW);
  delayMicroseconds(pulseDurationMicSec);
}

void StepMotor::turnSlowly(uint32_t pulseCnt, TurnDirection dir) {
  Serial.println("StepMotor::turnSlowly begin");
  digitalWrite(_directionPin, uint8_t(dir));
  for(int i = 0; i < pulseCnt; i++) {
    pulse(_pulseMaxDurationMicSec);
  }
  Serial.println("StepMotor::turnSlowly end");
}

Result StepMotor::turnSmoothly(uint32_t pulseCnt, uint32_t pulseCntSmothly, TurnDirection dir) {
  Serial.println("StepMotor::turnSmoothly: begin");
  if (pulseCntSmothly == 0) {return Result(ResultCode::ParamOutOfRange, "Количество импульсов для сглаживания не может быть равно 0");}
  if (pulseCntSmothly > pulseCnt / 2) {return Result(ResultCode::ParamOutOfRange, "Количество импульсов для сглаживания не может быть больше pulseCnt/2");}
  
  digitalWrite(_directionPin, uint8_t(dir));
  float_t deltaMicSec = float_t(_pulseMaxDurationMicSec - _pulseMinDurationMicSec) / float_t(pulseCntSmothly);
  float_t durationMicSec = _pulseMaxDurationMicSec; // начинаем с низкой скорости
  for(int i = 0; i < pulseCnt; i++) {
    // уменьшаем длительность импульса, увеличиваем скорость
    if (i < pulseCntSmothly) {durationMicSec -= deltaMicSec;}
    // увеличиваем длительность импульса, уменьшаем скорость
    if (i > pulseCnt - pulseCntSmothly) {durationMicSec += deltaMicSec;}

    pulse(uint32_t(durationMicSec));
  }

  Serial.println("StepMotor::turnSmoothly: end");
  return Result(ResultCode::Success, "");
}   
