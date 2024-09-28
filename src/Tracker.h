#ifndef TRACKER_H
#define TRACKER_H

#include <Arduino.h>
#include "Result.h"
#include "StepMotor.h"

// Состояние трекера
enum class TrackerState {
    Offline, // Моторы отключены, трекер не управляется
    Online   // Моторы включены, трекер управляется
};

// Трекер
class Tracker
{
private:
    StepMotor *_azimuthMotor; // Шаговый двигатель вращения по азимуту
    uint32_t _azimPulsPerAngle; // Количество импульсов для поворота на 1 градус по азимуту
    uint16_t _azimSmothAngle; // Угол по азимуту на котором применяется постепенное ускорение или торможение поворота
    bool _azimDirectionInversion; // Инверсия вращения мотора по азимуту
    int16_t _currentAzimuth; // Текущий азимут от 0 до 360 град, 0 - является поворотной точкой
    TrackerState _currentState; // Текущее состояние трекера

    Result checkAzimuth(int16_t azimuth); // Проверка допустимости азимута

public:
    /* motor - Шаговый двигатель
    azimPulsPerAngle - Количество импульсов для поворота на 1 градус по азимуту
    azimSmothAngle - Угол по азимуту на котором применяется постепенное ускорение или торможение поворота
    azimDirectionInversion - Инверсия вращения мотора по азимуту 
    state - состояние в которое перевести трекер*/    
    Tracker(StepMotor* motor, uint32_t azimPulsPerAngle, uint16_t azimSmothAngle, bool azimDirectionInversion, TrackerState state);
    // Режим онлайн - программное управление
    Result online();
    // Режим оффлайн - отключение приводов
    Result offline();       
    // Поворот по азимуту на заданный угол
    Result turnAzimuthAngle(int16_t angle);
    // Поворот в азимут
    Result turnToAzimuth(int16_t azimuth);
    // Установить текущий азимут
    Result setCurrentAzimuth(int16_t azimuth);
    // Получить текущий азимут
    int16_t getCurrentAzimuth() {return _currentAzimuth;}
};

#endif // TRACKER_H