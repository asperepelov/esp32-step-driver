#ifndef TRACKER_H
#define TRACKER_H

#include <Arduino.h>
#include "StepMotor.h"

// Состояние трекера
enum class TrackerState {
    Offline, // Моторы отключены, трекер не управляется
    Online   // Моторы включены, трекер управляется
};

// Коды результатов операций трекера
enum class TrackerResultCode {
    Success,
    TrackerIsOffline,
    AngleOutOfRange,
    UnknownCommand,
    CommandError
};

// Структура для возврата результата операции
struct TrackerResult {
    TrackerResultCode code;
    String message;

    TrackerResult(TrackerResultCode c, const String& msg) 
        : code(c), message(msg) {}
};

class Tracker
{
private:
    StepMotor *_azimuthMotor; // Шаговый двигатель вращения по азимуту
    int _azimPulsPerAngle; // Количество импульсов для поворота на 1 градус по азимуту
    bool _azimDirectionInversion; // Инверсия вращения мотора по азимуту
    int16_t _currentAzimuth; // Текущий азимут от 0 до 360 град, 0 - является поворотной точкой
    TrackerState _currentState; // Текущее состояние трекера

    TrackerResult checkAzimuth(int16_t azimuth); // Проверка допустимости азимута

public:
    /* *motor - указатель StepMotor
    azimPulsPerAngle - Количество импульсов для поворота на 1 градус по азимуту
    azimDirectionInversion - Инверсия вращения мотора по азимуту */    
    Tracker(StepMotor *motor, int azimPulsPerAngle, bool azimDirectionInversion, TrackerState state);
    // Режим онлайн - программное управление
    TrackerResult online();
    // Режим оффлайн - отключение приводов
    TrackerResult offline();       
    // Поворот по азимуту на заданный угол
    TrackerResult turnAzimuthAngle(int16_t angle);
    // Поворот в азимут
    TrackerResult turnToAzimuth(int16_t azimuth);
    // Установить текущий азимут
    TrackerResult setCurrentAzimuth(int16_t azimuth);
    // Получить текущий азимут
    int16_t getCurrentAzimuth() {return _currentAzimuth;}
};

#endif // TRACKER_H