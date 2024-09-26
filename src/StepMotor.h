#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H

#include <Arduino.h>

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
    // Длительность импульса микросекунд (влияет на скорость вращения)
    uint32_t _pulseDurationMicSec;
public:
    /*stepPin - подключение к PUL+
    directionPin - подключение к DIR+
    enablePin - подключение к ENA+
    pulseDurationMicSec - длительность импульса микросекунд (влияет на скорость вращения) */
    StepMotor(uint8_t stepPin, uint8_t directionPin, uint8_t enablePin, uint32_t pulseDurationMicSec);
    // Включение управления мотором
    void enable();
    // Отключение управления мотором
    void disable();    
    // Один импульс для двигателя (не всегда равен шагу)
    void pulse();
    // Поворот в заданном направлении
    void turn(int pulseCnt, TurnDirection dir);
};

#endif // STEP_MOTOR_H