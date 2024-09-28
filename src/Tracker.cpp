#include "Tracker.h"

Tracker::Tracker(StepMotor *motor, uint32_t azimPulsPerAngle, uint16_t azimSmothAngle, bool directionInversion, TrackerState state)
    : _azimuthMotor(motor), _azimPulsPerAngle(azimPulsPerAngle), _azimSmothAngle(azimSmothAngle), _azimDirectionInversion(directionInversion) {
        if (state == TrackerState::Offline) { this->offline();}
        if (state == TrackerState::Online) { this->online();}
    }

Result Tracker::online() {
    _currentState = TrackerState::Online;
    _azimuthMotor->enable();

    return Result(
        ResultCode::Success,
        "Трекер переведен в онлайн, управление активировано"
    );
}

Result Tracker::offline() {
    _currentState = TrackerState::Offline;
    _azimuthMotor->disable();

    return Result(
        ResultCode::Success,
        "Трекер переведен в оффлайн, отключение управления"
    );
}

Result Tracker::checkAzimuth(int16_t azimuth) {
    if (azimuth < 0 || azimuth >= 360) {
        return Result(
            ResultCode::ParamOutOfRange,
            "Азимут вне допустимого диапазона (0-359)"
        );
    }

    return Result(ResultCode::Success, "");   
}


Result Tracker::setCurrentAzimuth(int16_t azimuth) {
    Result res = checkAzimuth(azimuth);
    if (res.code != ResultCode::Success) {return res;}

    _currentAzimuth = azimuth;

    return Result(
        ResultCode::Success,
        "Установлен текущий азимут " + String(_currentAzimuth)
    );
}

Result Tracker::turnAzimuthAngle(int16_t angle) {    
    if (_currentState == TrackerState::Offline) {
        return Result(
            ResultCode::TrackerIsOffline,
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

    // определяем скорость вращения
    if (abs(calcAngle) <= 2*_azimSmothAngle) {_azimuthMotor->turnSlowly(pulses, dir);} // медленно
    else {_azimuthMotor->turnSmoothly(pulses, _azimSmothAngle*_azimPulsPerAngle, dir);} // с ускорением
    
    _currentAzimuth = newAzim;

    return Result(
        ResultCode::Success,
        "Трекер повернулся по азимуту на " + String(calcAngle) + " градусов"
    );
}

Result Tracker::turnToAzimuth(int16_t azimuth) {
    Result res = checkAzimuth(azimuth);
    if (res.code != ResultCode::Success) {return res;}

    int16_t angle = azimuth - _currentAzimuth;
    
    res = turnAzimuthAngle(angle);
    if (res.code != ResultCode::Success) {return res;}

    return Result(
        ResultCode::Success,
        "Трекер повернулся на азимут " + String(azimuth)
    );
}  

