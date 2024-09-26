#ifndef TRACKER_H
#define TRACKER_H

#include <Arduino.h>
#include "StepMotor.h"

class Tracker
{
private:
    // Шаговый двигатель вращения по азимуту
    StepMotor *_azimutMotor;
    int _azimPulsPerAngle;
    bool _directionInversion;
public:
    /* *motor - указатель StepMotor
    azimPulsPerAngle - Количество импульсов, которые надо подать, чтобы устройство повернулось на 1 градус по азимуту
    directionInversion - Инверсия направления вращения */    
    Tracker(StepMotor *motor, int azimPulsPerAngle, bool directionInversion);
    // Режим онлайн - программное управление
    void online();
    // Режим оффлайн - отключение двигателей
    void offline();       
    // Поворот на заданный угол от -360 до 360
    void turnAzimut(int16_t angle);
};

#endif // TRACKER_H