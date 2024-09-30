#include "Tracker.h"
#include <math.h>

Tracker::Tracker(StepMotor *motor, uint32_t azimPulsPerAngle, uint16_t azimSmothAngle, bool directionInversion, TrackerState state)
    : _azimuthMotor(motor), _azimPulsPerAngle(azimPulsPerAngle), _azimSmothAngle(azimSmothAngle), _azimDirectionInversion(directionInversion) 
    , _currentLocation(Location(48, 37))
    {
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

Result Tracker::setCurrentLocation(Location loc) {
    if (!loc.isValid()) {return Result(ResultCode::ParamOutOfRange, "Ошибка в локации");}

    _currentLocation = loc;
    
    return Result(ResultCode::Success, "Новая локация трекера задана");
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

RadialLocation Tracker::calcRadialLocation(double lat, double lon) {
    const double R = 6371000; // Радиус Земли в метрах

    // Преобразование градусов в радианы
    double lat1Rad = _currentLocation._lat * M_PI / 180.0;
    double lat2Rad = lat * M_PI / 180.0;
    double lon1Rad = _currentLocation._lon * M_PI / 180.0;
    double lon2Rad = lon * M_PI / 180.0;

    // Разница координат
    double dLat = lat2Rad - lat1Rad;
    double dLon = lon2Rad - lon1Rad;

    // Расстояние по широте
    double latDistance = R * dLat;

    // Расстояние по долготе
    double lonDistance = R * dLon * cos((lat1Rad + lat2Rad) / 2);

    RadialLocation result;
    result.distance = sqrt(latDistance*latDistance + lonDistance*lonDistance);
    result.azimuth = atan2(lonDistance, latDistance) * 180 / PI;
    if (result.azimuth < 0) {result.azimuth += 360.0;}

    return result;
}