#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H

#include <Arduino.h>
#include "Result.h"

// Направление поворота
enum class TurnDirection {Plus = LOW, Minus = HIGH};

class StepMotor
{
private:
    // подключение к PUL+
    uint8_t _stepPin;
    // подключение к DIR+
    uint8_t _directionPin;
    // подключение к ENA+
    uint8_t _enablePin;
    // Минимальная длительность импульса микросекунд (влияет на максимальную скорость вращения)
    uint32_t _pulseMinDurationMicSec;
    // Максимальная длительность импульса микросекунд (влияет на минимальную скорость вращения)
    uint32_t _pulseMaxDurationMicSec;

    // Один импульс для двигателя (не всегда равен шагу)
    void pulse(uint32_t pulseDurationMicSec);    
public:
    /*stepPin - подключение к PUL+
    directionPin - подключение к DIR+
    enablePin - подключение к ENA+
    pulseMinDurationMicSec - Минимальная длительность импульса микросекунд (определяет максимальную скорость вращения)
    pulseMaxDurationMicSec - Максимальная длительность импульса микросекунд (определяет минимальную скорость вращения)*/
    StepMotor(uint8_t stepPin, uint8_t directionPin, uint8_t enablePin, uint32_t pulseMinDurationMicSec, uint32_t pulseMaxDurationMicSec);
    // Включение управления мотором
    void enable();
    // Отключение управления мотором
    void disable();    
    // Медленный поворот в заданном направлении
    void turnSlowly(uint32_t pulseCnt, TurnDirection dir);
    // Плавный поворот в заданном направлении
    Result turnSmoothly(uint32_t pulseCnt, uint32_t pulseCntSmothly, TurnDirection dir);    
};

#endif // STEP_MOTOR_H