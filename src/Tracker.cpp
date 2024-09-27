#include "Tracker.h"

Tracker::Tracker(StepMotor *motor, int azimPulsPerAngle, bool directionInversion, TrackerState state)
    : _azimuthMotor(motor), _azimPulsPerAngle(azimPulsPerAngle), _azimDirectionInversion(directionInversion) {
        if (state == TrackerState::Offline) { this->offline();}
        if (state == TrackerState::Online) { this->online();}
    }

TrackerResult Tracker::online() {
    _currentState = TrackerState::Online;
    _azimuthMotor->enable();

    return TrackerResult(
        TrackerResultCode::Success,
        "Трекер переведен в онлайн, управление активировано"
    );
}

TrackerResult Tracker::offline() {
    _currentState = TrackerState::Offline;
    _azimuthMotor->disable();

    return TrackerResult(
        TrackerResultCode::Success,
        "Трекер переведен в оффлайн, отключение управления"
    );
}

TrackerResult Tracker::checkAzimuth(int16_t azimuth) {
    if (azimuth < 0 || azimuth >= 360) {
        return TrackerResult(
            TrackerResultCode::AngleOutOfRange,
            "Азимут вне допустимого диапазона (0-359)"
        );
    }

    return TrackerResult(TrackerResultCode::Success, "");   
}


TrackerResult Tracker::setCurrentAzimuth(int16_t azimuth) {
    TrackerResult res = checkAzimuth(azimuth);
    if (res.code != TrackerResultCode::Success) {return res;}

    _currentAzimuth = azimuth;

    return TrackerResult(
        TrackerResultCode::Success,
        "Установлен текущий азимут " + String(_currentAzimuth)
    );
}

TrackerResult Tracker::turnAzimuthAngle(int16_t angle) {    
    if (_currentState == TrackerState::Offline) {
        return TrackerResult(
            TrackerResultCode::TrackerIsOffline,
            "Трекер находится в оффлайн"
        );
    }
    
    // Азимут всегда от 0 до 360
    int16_t calcAngle = angle % 360;     
   
    int16_t newAzim = _currentAzimuth + calcAngle;
    
    // 0 - является поворотной точкой
    if (newAzim < 0) {
        newAzim += 360;
    } 
    if (newAzim > 360) {
        newAzim -= 360;
    }

    calcAngle = newAzim - _currentAzimuth; //реальный угол поворота
   
    int16_t pulses = abs(calcAngle) * _azimPulsPerAngle;
    TurnDirection dir;

    if (calcAngle > 0) {
        if (_azimDirectionInversion) {dir = TurnDirection::Minus;}
        else {dir = TurnDirection::Plus;}
    }
    else {
        if (_azimDirectionInversion) {dir = TurnDirection::Plus;}
        else {dir = TurnDirection::Minus;}
    }

    _azimuthMotor->turn(pulses, dir);
    _currentAzimuth = newAzim;

    return TrackerResult(
        TrackerResultCode::Success,
        "Трекер повернулся по азимуту на " + String(calcAngle) + " градусов"
    );
}

TrackerResult Tracker::turnToAzimuth(int16_t azimuth) {
    TrackerResult res = checkAzimuth(azimuth);
    if (res.code != TrackerResultCode::Success) {return res;}

    int16_t angle = azimuth - _currentAzimuth;
    
    res = turnAzimuthAngle(angle);
    if (res.code != TrackerResultCode::Success) {return res;}

    return TrackerResult(
        TrackerResultCode::Success,
        "Трекер повернулся на азимут " + String(azimuth)
    );
}  

